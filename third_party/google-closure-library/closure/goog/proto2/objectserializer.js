/**
 * @license
 * Copyright The Closure Library Authors.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @fileoverview Protocol Buffer 2 Serializer which serializes messages
 *  into anonymous, simplified JSON objects.
 */

goog.provide('goog.proto2.ObjectSerializer');

goog.require('goog.asserts');
goog.require('goog.proto2.FieldDescriptor');
goog.require('goog.proto2.Serializer');
goog.require('goog.string');
goog.requireType('goog.proto2.Message');



/**
 * ObjectSerializer, a serializer which turns Messages into simplified
 * ECMAScript objects.
 *
 * @param {goog.proto2.ObjectSerializer.KeyOption=} opt_keyOption If specified,
 *     which key option to use when serializing/deserializing.
 * @param {boolean=} opt_serializeBooleanAsNumber If specified and true, the
 *     serializer will convert boolean values to 0/1 representation.
 * @param {boolean=} opt_ignoreUnknownFields If specified and true, the
 *     serializer will ignore unknown fields in the JSON payload instead of
 *     returning an error.
 * @constructor
 * @extends {goog.proto2.Serializer}
 */
goog.proto2.ObjectSerializer = function(
    opt_keyOption, opt_serializeBooleanAsNumber, opt_ignoreUnknownFields) {
  'use strict';
  /** @const */
  this.keyOption_ = opt_keyOption;
  /** @const */
  this.serializeBooleanAsNumber_ = opt_serializeBooleanAsNumber;
  /** @const */
  this.ignoreUnknownFields_ = opt_ignoreUnknownFields;
};
goog.inherits(goog.proto2.ObjectSerializer, goog.proto2.Serializer);


/**
 * An enumeration of the options for how to emit the keys in
 * the generated simplified object.
 *
 * For serialization, the option specifies the keys to use in the serialized
 * object.
 *
 * For deserialization, the option specifies which keys are allowed; an object
 * serialized by TAG may be deserialized by TAG or by NAME or by
 * CAMEL_CASE_NAME, but an object serialized by NAME cannot be deserialized by
 * TAG.  An object serialized with any option can be deserialized by
 * CAMEL_CASE_NAME.
 *
 * @enum {number}
 */
goog.proto2.ObjectSerializer.KeyOption = {
  /**
   * Use the tag of the field as the key (default)
   */
  TAG: 0,

  /**
   * Use the name of the field as the key. Unknown fields
   * will still use their tags as keys.
   */
  NAME: 1,

  /**
   * Use the camel cased name of the field as the key.
   * Unknown fields will still use their tags as keys.
   */
  CAMEL_CASE_NAME: 2
};


/**
 * Serializes a message to an object.
 *
 * @param {goog.proto2.Message} message The message to be serialized.
 * @return {!Object} The serialized form of the message.
 * @override
 */
goog.proto2.ObjectSerializer.prototype.serialize = function(message) {
  'use strict';
  var descriptor = message.getDescriptor();
  var fields = descriptor.getFields();

  var objectValue = {};

  // Add the defined fields, recursively.
  for (var i = 0; i < fields.length; i++) {
    var field = fields[i];

    var key = field.getTag();
    switch (this.keyOption_) {
      case goog.proto2.ObjectSerializer.KeyOption.TAG:
        // no action necessary, key already has the correct value.
        break;
      case goog.proto2.ObjectSerializer.KeyOption.NAME:
        key = field.getName();
        break;
      case goog.proto2.ObjectSerializer.KeyOption.CAMEL_CASE_NAME:
        key = goog.string.toCamelCase(
            field
                .getName()
                // goog.string.toCamelCase expects a hyphen delimited string but
                // proto fields are usually underscore delimited
                // (go/proto-style-guide); the following regex converts from
                // underscore delimited form to hyphen delimited form.
                .replace(/_/g, '-'));
        break;
      default:
        // Default should never be reached unless keyOption is outside the valid
        // domain.
        goog.asserts.assert(
            this.keyOption_ !== goog.proto2.ObjectSerializer.KeyOption.TAG &&
                this.keyOption_ !==
                    goog.proto2.ObjectSerializer.KeyOption.NAME &&
                this.keyOption_ !==
                    goog.proto2.ObjectSerializer.KeyOption.CAMEL_CASE_NAME,
            'keyOption should be one of TAG, NAME, or CAMEL_CASE_NAME');
    }

    if (message.has(field)) {
      if (field.isRepeated()) {
        var array = [];
        objectValue[key] = array;

        for (var j = 0; j < message.countOf(field); j++) {
          array.push(this.getSerializedValue(field, message.get(field, j)));
        }

      } else {
        objectValue[key] = this.getSerializedValue(field, message.get(field));
      }
    }
  }

  // Add the unknown fields, if any.
  message.forEachUnknown(function(tag, value) {
    'use strict';
    // Do not set null values. This is possible when using pbliteserializer to
    // convert jsbp to closure object and then passed to this method.
    if (value !== null) {
      objectValue[tag] = value;
    }
  });

  return objectValue;
};


/** @override */
goog.proto2.ObjectSerializer.prototype.getSerializedValue = function(
    field, value) {
  'use strict';
  // Handle the case where a boolean should be serialized as 0/1.
  // Some deserialization libraries, such as GWT, can use this notation.
  if (this.serializeBooleanAsNumber_ &&
      field.getFieldType() == goog.proto2.FieldDescriptor.FieldType.BOOL &&
      typeof value === 'boolean') {
    return value ? 1 : 0;
  }

  return goog.proto2.ObjectSerializer.base(
      this, 'getSerializedValue', field, value);
};


/** @override */
goog.proto2.ObjectSerializer.prototype.getDeserializedValue = function(
    field, value) {
  'use strict';
  // Gracefully handle the case where a boolean is represented by 0/1.
  // Some serialization libraries, such as GWT, can use this notation.
  if (field.getFieldType() == goog.proto2.FieldDescriptor.FieldType.BOOL &&
      typeof value === 'number') {
    return Boolean(value);
  }

  return goog.proto2.ObjectSerializer.base(
      this, 'getDeserializedValue', field, value);
};


/**
 * Deserializes a message from an object and places the
 * data in the message.
 *
 * @param {goog.proto2.Message} message The message in which to
 *     place the information.
 * @param {*} data The data of the message.
 * @override
 */
goog.proto2.ObjectSerializer.prototype.deserializeTo = function(message, data) {
  'use strict';
  var descriptor = message.getDescriptor();

  for (var key in data) {
    var field;
    var value = data[key];

    var isNumeric = goog.string.isNumeric(key);

    if (isNumeric) {
      field = descriptor.findFieldByTag(key);
    } else {
      // We must not be in Key == TAG mode to lookup by name.
      goog.asserts.assert(
          this.keyOption_ == goog.proto2.ObjectSerializer.KeyOption.NAME ||
              this.keyOption_ ==
                  goog.proto2.ObjectSerializer.KeyOption.CAMEL_CASE_NAME,
          'Key mode ' + this.keyOption_ + 'for key ' + key + ' is not ' +
              goog.proto2.ObjectSerializer.KeyOption.NAME + ' nor ' +
              goog.proto2.ObjectSerializer.KeyOption.CAMEL_CASE_NAME);

      if (this.keyOption_ ==
          goog.proto2.ObjectSerializer.KeyOption.CAMEL_CASE_NAME) {
        key = goog.string
                  .toSelectorCase(key)
                  // goog.string.toSelectorCase returns a hyphen delimited form
                  // of the name but protos usually use an underscore delimited
                  // form (go/proto-style-guide); the following regex converts
                  // from hyphens to underscores.
                  .replace(/\-/g, '_');
      }
      field = descriptor.findFieldByName(key);
    }

    if (field) {
      if (field.isRepeated()) {
        goog.asserts.assert(
            Array.isArray(value),
            'Value for repeated field ' + field + ' must be an array.');

        for (var j = 0; j < value.length; j++) {
          message.add(field, this.getDeserializedValue(field, value[j]));
        }
      } else {
        goog.asserts.assert(
            !Array.isArray(value),
            'Value for non-repeated field ' + field + ' must not be an array.');
        message.set(field, this.getDeserializedValue(field, value));
      }
    } else {
      if (isNumeric) {
        // We have an unknown field (with a numeric tag).
        message.setUnknown(Number(key), value);
      } else {
        // Handle unknown non-numeric tag.
        if (!this.ignoreUnknownFields_) {
          // Named fields must be present.
          goog.asserts.fail('Failed to find field: ' + key);
        }
      }
    }
  }
};
