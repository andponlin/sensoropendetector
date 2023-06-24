/*
 * Copyright 2023, Andrew Lindesay <apl@lindesay.co.nz>
 * All Rights Reserved. Distributed under the terms of the MIT License.
 */
#ifndef HTTP_UTILS_H
#define HTTP_UTILS_H

#include <Arduino.h>

class HttpUtils {
  public:
    static String encodeFormValue(String value);

};

#endif // HTTP_UTILS_H
