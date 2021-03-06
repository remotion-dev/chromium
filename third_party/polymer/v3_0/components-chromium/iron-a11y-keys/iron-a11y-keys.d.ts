/**
 * DO NOT EDIT
 *
 * This file was automatically generated by
 *   https://github.com/Polymer/tools/tree/master/packages/gen-typescript-declarations
 *
 * To modify these typings, edit the source file(s):
 *   iron-a11y-keys.js
 */

import {IronA11yKeysBehavior} from '../iron-a11y-keys-behavior/iron-a11y-keys-behavior.js';

import {Polymer} from '../polymer/lib/legacy/polymer-fn.js';

import {LegacyElementMixin} from '../polymer/lib/legacy/legacy-element-mixin.js';

interface IronA11yKeysElement extends IronA11yKeysBehavior, LegacyElementMixin, HTMLElement {
  target: Node|null;

  /**
   * Space delimited list of keys where each key follows the format:
   * `[MODIFIER+]*KEY[:EVENT]`.
   * e.g. `keys="space ctrl+shift+tab enter:keyup"`.
   * More detail can be found in the "Grammar" section of the documentation
   */
  keys: string|null|undefined;
  attached(): void;
  _targetChanged(target: any): void;
  _keysChanged(): void;
  _fireKeysPressed(event: any): void;
}

export {IronA11yKeysElement};

declare global {

  interface HTMLElementTagNameMap {
    "iron-a11y-keys": IronA11yKeysElement;
  }
}
