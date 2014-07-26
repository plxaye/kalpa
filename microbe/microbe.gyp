# Copyright (c) 2012 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
{
  'variables': {
    'chromium_code': 1,
  },
  'target_defaults': {
    'conditions': [
      ['use_aura==1', {
        'sources/': [ ['exclude', '_win\\.(h|cc)$'] ],
        'dependencies': [ '../aura/aura.gyp:aura', ],
      }],
      ['OS!="linux" or chromeos==1', {
        'sources/': [ ['exclude', '_linux\\.(h|cc)$'] ],
      }],
    ],
  },
  'targets': [
    {
      'target_name': 'microbe',
      'type': 'executable',
      'dependencies': [
        '<(DEPTH)/base/base.gyp:base',
        '<(DEPTH)/base/base.gyp:base_i18n',
        '<(DEPTH)/skia/skia.gyp:skia',
        '<(DEPTH)/url/url.gyp:url_lib',
        '<(DEPTH)/ui/ui.gyp:ui',
        '<(DEPTH)/ui/views/views.gyp:views',
        '<(DEPTH)/ui/ui.gyp:ui_resources', #grit/ui_resources.h
        '<(DEPTH)/app/app_resources.gyp:app_resources', #grit/app_resources.h
        '<(DEPTH)/app/app_resources.gyp:packed_resources', #resource_100_percent.pak
      ],
      'include_dirs': [
        '..',
      ],
      'sources': [
        'examples_main.cc',
        'examples_views_delegate.cc',
        'examples_views_delegate.h',
        'examples_window.cc',
        'examples_window.h',
		'<(DEPTH)/app/app.exe.rc',
		'<(DEPTH)/app/app.exe.h',
		'<(SHARED_INTERMEDIATE_DIR)/ui/ui_resources/ui_unscaled_resources.rc', #cursor
		'<(SHARED_INTERMEDIATE_DIR)/ui/ui_resources/grit/ui_unscaled_resources.h',
      ],
	  'copies': [ {
         'destination': '<(PRODUCT_DIR)',
         'files' : [
         ],
        },
      ],
      'conditions': [
        ['OS=="win"', {
          'link_settings': {
            'libraries': [
              '-limm32.lib',
              '-loleacc.lib',
            ]
          },
		  'msvs_settings': {
            'VCManifestTool': {
              'AdditionalManifestFiles': [
                '<(DEPTH)/app/app.exe.manifest',
              ],
            },
			'VCLinkerTool': {
			  'SubSystem': '2',         # Set /SUBSYSTEM:WINDOWS
			},
		  },
        }],
        ['OS=="win" and win_use_allocator_shim==1', {
          'dependencies': [
            '<(DEPTH)/base/allocator/allocator.gyp:allocator',
          ],
        }],
      ],
    },  # target_name: microbe
  ],
}
