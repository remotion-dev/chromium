// Copyright 2022 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/ash/arc/input_overlay/actions/action_tap.h"

#include "base/strings/utf_string_conversions.h"
#include "chrome/browser/ash/arc/input_overlay/touch_id_manager.h"
#include "chrome/browser/ash/arc/input_overlay/ui/action_label.h"
#include "ui/aura/window.h"
#include "ui/events/base_event_utils.h"
#include "ui/events/keycodes/dom/dom_code.h"
#include "ui/events/keycodes/dom/keycode_converter.h"
#include "ui/gfx/geometry/point.h"
#include "ui/gfx/geometry/size.h"

namespace arc {
namespace input_overlay {
namespace {
// UI specs.
constexpr int kLabelPositionToSide = 36;
constexpr int kLabelMargin = 2;
}  // namespace

class ActionTap::ActionTapView : public ActionView {
 public:
  ActionTapView(Action* action,
                DisplayOverlayController* display_overlay_controller,
                const gfx::RectF& content_bounds)
      : ActionView(action, display_overlay_controller) {
    int radius = action->GetUIRadius(content_bounds);
    auto circle = std::make_unique<ActionCircle>(radius);
    std::string text;
    if (action->IsKeyboardBound()) {
      text = GetDisplayText(action->current_binding()->keys()[0]);
    } else if (action->IsMouseBound()) {
      text = action->current_binding()->mouse_action();
    } else {
      text = "?";
    }
    auto label = std::make_unique<ActionLabel>(base::UTF8ToUTF16(text));
    label->set_editable(true);
    auto label_size = label->GetPreferredSize();
    label->SetSize(label_size);
    int width = std::max(
        radius * 2, radius * 2 - kLabelPositionToSide + label_size.width());
    SetSize(gfx::Size(width, radius * 2));
    if (action->on_left_or_middle_side()) {
      circle->SetPosition(gfx::Point());
      label->SetPosition(
          gfx::Point(label_size.width() > kLabelPositionToSide
                         ? width - label_size.width()
                         : width - kLabelPositionToSide,
                     radius * 2 - label_size.height() - kLabelMargin));
      center_.set_x(radius);
      center_.set_y(radius);
    } else {
      circle->SetPosition(gfx::Point(width - radius * 2, 0));
      label->SetPosition(
          gfx::Point(0, radius * 2 - label_size.height() - kLabelMargin));
      center_.set_x(width - radius);
      center_.set_y(radius);
    }
    circle_ = AddChildView(std::move(circle));
    labels_.emplace_back(AddChildView(std::move(label)));
  }

  ActionTapView(const ActionTapView&) = delete;
  ActionTapView& operator=(const ActionTapView&) = delete;
  ~ActionTapView() override = default;
};

ActionTap::ActionTap(aura::Window* window) : Action(window) {}
ActionTap::~ActionTap() = default;

bool ActionTap::ParseFromJson(const base::Value& value) {
  Action::ParseFromJson(value);
  if (locations_.empty()) {
    LOG(ERROR) << "Require at least one location for tap action {" << name_
               << "}.";
    return false;
  }
  return parsed_input_sources_ == InputSource::IS_KEYBOARD
             ? ParseJsonFromKeyboard(value)
             : ParseJsonFromMouse(value);
}

bool ActionTap::ParseJsonFromKeyboard(const base::Value& value) {
  auto key = ParseKeyboardKey(value, name_);
  if (!key) {
    LOG(ERROR) << "No/invalid key code for key tap action {" << name_ << "}.";
    return false;
  }
  original_binding_ = InputElement::CreateActionTapKeyElement(key->first);
  current_binding_ = InputElement::CreateActionTapKeyElement(key->first);
  return true;
}

bool ActionTap::ParseJsonFromMouse(const base::Value& value) {
  const std::string* mouse_action = value.FindStringKey(kMouseAction);
  if (!mouse_action) {
    LOG(ERROR) << "Must include mouse action for mouse move action.";
    return false;
  }
  if (*mouse_action != kPrimaryClick && *mouse_action != kSecondaryClick) {
    LOG(ERROR) << "Not supported mouse action in mouse tap action: "
               << *mouse_action;
    return false;
  }
  original_binding_ = InputElement::CreateActionTapMouseElement(*mouse_action);
  current_binding_ = InputElement::CreateActionTapMouseElement(*mouse_action);
  return true;
}

bool ActionTap::RewriteEvent(const ui::Event& origin,
                             const gfx::RectF& content_bounds,
                             const bool is_mouse_locked,
                             std::list<ui::TouchEvent>& touch_events,
                             bool& keep_original_event) {
  if (IsNoneBound() || (IsKeyboardBound() && !origin.IsKeyEvent()) ||
      (IsMouseBound() && !origin.IsMouseEvent())) {
    return false;
  }
  DCHECK((IsKeyboardBound() && !IsMouseBound()) ||
         (!IsKeyboardBound() && IsMouseBound()));
  LogEvent(origin);
  // Rewrite for key event.
  if (IsKeyboardBound()) {
    auto* key_event = origin.AsKeyEvent();
    bool rewritten = RewriteKeyEvent(key_event, touch_events, content_bounds,
                                     keep_original_event);
    LogTouchEvents(touch_events);
    return rewritten;
  }
  // Rewrite for mouse event.
  if (!is_mouse_locked)
    return false;
  auto* mouse_event = origin.AsMouseEvent();
  auto rewritten = RewriteMouseEvent(mouse_event, touch_events, content_bounds);
  LogTouchEvents(touch_events);
  return rewritten;
}

bool ActionTap::RewriteKeyEvent(const ui::KeyEvent* key_event,
                                std::list<ui::TouchEvent>& rewritten_events,
                                const gfx::RectF& content_bounds,
                                bool& keep_original_event) {
  DCHECK(key_event);
  if (!IsSameDomCode(key_event->code(), current_binding_->keys()[0]))
    return false;

  // Ignore repeated key events, but consider it as processed.
  if (IsRepeatedKeyEvent(*key_event))
    return true;

  if (key_event->type() == ui::ET_KEY_PRESSED) {
    if (touch_id_) {
      LOG(ERROR) << "Touch ID shouldn't be set for the initial press: "
                 << ui::KeycodeConverter::DomCodeToCodeString(
                        key_event->code());
      return false;
    }

    touch_id_ = TouchIdManager::GetInstance()->ObtainTouchID();
    DCHECK(touch_id_);
    if (!touch_id_)
      return false;
    auto pos = CalculateTouchPosition(content_bounds);
    if (!pos)
      return false;
    last_touch_root_location_ = *pos;

    rewritten_events.emplace_back(ui::TouchEvent(
        ui::EventType::ET_TOUCH_PRESSED, last_touch_root_location_,
        last_touch_root_location_, key_event->time_stamp(),
        ui::PointerDetails(ui::EventPointerType::kTouch, *touch_id_)));
    ui::Event::DispatcherApi(&(rewritten_events.back()))
        .set_target(target_window_);
    if (!current_binding_->is_modifier_key()) {
      keys_pressed_.emplace(key_event->code());
    } else {
      // For modifier keys, EventRewriterChromeOS skips release event for other
      // event rewriters but still keeps the press event, so AcceleratorHistory
      // can still receive the release event. To avoid error in
      // AcceleratorHistory, original press event is still sent.
      keep_original_event = true;
      rewritten_events.emplace_back(ui::TouchEvent(
          ui::EventType::ET_TOUCH_RELEASED, last_touch_root_location_,
          last_touch_root_location_, key_event->time_stamp(),
          ui::PointerDetails(ui::EventPointerType::kTouch, *touch_id_)));
      ui::Event::DispatcherApi(&(rewritten_events.back()));
      OnTouchReleased();
    }
  } else {
    if (!touch_id_) {
      LOG(ERROR) << "There should be a touch ID for the release {"
                 << ui::KeycodeConverter::DomCodeToCodeString(key_event->code())
                 << "}.";
      return false;
    }

    rewritten_events.emplace_back(ui::TouchEvent(
        ui::EventType::ET_TOUCH_RELEASED, last_touch_root_location_,
        last_touch_root_location_, key_event->time_stamp(),
        ui::PointerDetails(ui::EventPointerType::kTouch, *touch_id_)));
    ui::Event::DispatcherApi(&(rewritten_events.back()))
        .set_target(target_window_);

    last_touch_root_location_.set_x(0);
    last_touch_root_location_.set_y(0);
    keys_pressed_.erase(key_event->code());
    OnTouchReleased();
  }
  return true;
}

bool ActionTap::RewriteMouseEvent(const ui::MouseEvent* mouse_event,
                                  std::list<ui::TouchEvent>& rewritten_events,
                                  const gfx::RectF& content_bounds) {
  DCHECK(mouse_event);

  auto type = mouse_event->type();
  if (!current_binding_->mouse_types().contains(type) ||
      (current_binding_->mouse_flags() & mouse_event->changed_button_flags()) ==
          0) {
    return false;
  }

  if (type == ui::ET_MOUSE_PRESSED)
    DCHECK(!touch_id_);
  if (type == ui::ET_MOUSE_RELEASED)
    DCHECK(touch_id_);

  if (!touch_id_) {
    touch_id_ = TouchIdManager::GetInstance()->ObtainTouchID();
    auto touch_down_pos = CalculateTouchPosition(content_bounds);
    if (touch_down_pos) {
      last_touch_root_location_ = *touch_down_pos;
    } else {
      // Primary click.
      auto root_location = mouse_event->root_location_f();
      last_touch_root_location_.SetPoint(root_location.x(), root_location.y());
      float scale = target_window_->GetHost()->device_scale_factor();
      last_touch_root_location_.Scale(scale);
    }
    rewritten_events.emplace_back(ui::TouchEvent(
        ui::EventType::ET_TOUCH_PRESSED, last_touch_root_location_,
        last_touch_root_location_, mouse_event->time_stamp(),
        ui::PointerDetails(ui::EventPointerType::kTouch, *touch_id_)));
  } else {
    rewritten_events.emplace_back(ui::TouchEvent(
        ui::EventType::ET_TOUCH_RELEASED, last_touch_root_location_,
        last_touch_root_location_, mouse_event->time_stamp(),
        ui::PointerDetails(ui::EventPointerType::kTouch, *touch_id_)));
    OnTouchReleased();
  }
  ui::Event::DispatcherApi(&(rewritten_events.back()))
      .set_target(target_window_);
  return true;
}

gfx::PointF ActionTap::GetUICenterPosition(const gfx::RectF& content_bounds) {
  auto* position = locations().front().get();
  return position->CalculatePosition(content_bounds);
}

std::unique_ptr<ActionView> ActionTap::CreateView(
    DisplayOverlayController* display_overlay_controller,
    const gfx::RectF& content_bounds) {
  auto view = std::make_unique<ActionTapView>(this, display_overlay_controller,
                                              content_bounds);
  view->set_editable(true);
  auto center_pos = GetUICenterPosition(content_bounds);
  view->SetPositionFromCenterPosition(center_pos);
  return view;
}

}  // namespace input_overlay
}  // namespace arc
