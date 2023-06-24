/*
 * Copyright 2023, Andrew Lindesay <apl@lindesay.co.nz>
 * All Rights Reserved. Distributed under the terms of the MIT License.
 */
#include <Arduino.h>

#include "httputils.h"

/*
HTTP requests can carry a payload or query parameters that carry form-value
encoded text. This encoding is a form of text escaping and this function
will escape the characters as necessary.
*/

/*static*/
String HttpUtils::encodeFormValue(String value) {
  String result;
  for (int i = 0; i < value.length(); i++) {
    uint8_t ch = value.charAt(i);
    if (' ' == ch) {
      result += '+';
    }
    else {
      if (isAlphaNumeric(ch)) {
        result += (char) ch;
      }
      else {
        static char* HEXCHARS = "0123456789ABCDEF";
        result += '%';
        result += HEXCHARS[(ch >> 4)];
        result += HEXCHARS[(ch & 0x0f)];
      }
    }
  }
  return result;
}
