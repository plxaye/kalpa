# Copyright (c) 2012 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'targets': [
    {
      'target_name': 'All',
      'type': 'none',
      'xcode_create_dependents_test_runner': 1,
      'dependencies': [
        'some.gyp:*',
        '../base/base.gyp:*',
        '../testing/gmock.gyp:*',
        '../testing/gtest.gyp:*',
        '../third_party/zlib/zlib.gyp:*',
      ],
    }, # target_name: All
  ],
}
