// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MICROBE_EXAMPLES_WINDOW_H_
#define MICROBE_EXAMPLES_WINDOW_H_

#include <string>

namespace views {
namespace microbe {

enum Operation {
  DO_NOTHING_ON_CLOSE = 0,
  QUIT_ON_CLOSE,
};

// Shows a window with the views examples in it.
void ShowExamplesWindow(Operation operation);

// Log
void LogStatus(const std::string& string) ;

}  // namespace microbe
}  // namespace views

#endif  // MICROBE_EXAMPLES_WINDOW_H_
