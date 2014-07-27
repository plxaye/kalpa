# Copyright (c) 2011 The Chromium Authors. All rights reserved.
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
      'target_name': 'profiler',
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
        '<(DEPTH)/ipc/ipc.gyp:ipc',
        '<(DEPTH)/tools/profiler_dll/profiler_dll.gyp:profiler_dll',
        '<(DEPTH)/tools/profiler/memory_watcher.gyp:memory_watcher_profiler',
        '<(DEPTH)/tools/profiler/run_with_dll.gyp:run_with_dll',
        #'<(DEPTH)/tools/profiler/run_in_snapshot.gyp:*',
      ],
      'defines': [
        'BUILD_MEMORY_WATCHER',
      ],
      'include_dirs': [
        '../..',
      ],
      'sources': [
        'exe_main.cc',
        'elevate.cc',
        'elevate.h',
        'lowpri.cc',
        'lowpri.h',
        'nt_loader.cc',
        'nt_loader.h',
        'profiler_thread.cc',
        'profiler_thread.h',
        'log_daemon.cc',
        'log_daemon.h',
        'minidump_reader.cc',
        'minidump_reader.h',
        'minidump_prober.cc',
        'minidump_prober.h',
        'profiler_ipc_messages.h',
        'profiler_ipc_messages.cc',
        'profiler_data.cc',
        'profiler_data.h',
        'profiler_cmds.cc',
        'profiler_cmds.h',
        'profiler_logging.cc',
        'profiler_logging.h',
        'profiler_main.cc',
        'profiler_main.h',
        'profiler_process.cc',
        'profiler_process.h',
        'result_codes.h',
        'ui/profiler_window.cc',
        'ui/profiler_window.h',
        'ui/exe_main_ui.h',
        'ui/exe_main_ui.cc',
        'ui/profiler_ui_start.cc',
        'ui/profiler_ui_start.h',
        'ui/profiler_views_delegate.cc',
        'ui/profiler_views_delegate.h',
		'<(DEPTH)/app/app.exe.rc',
		'<(DEPTH)/app/app.exe.h',
		'<(SHARED_INTERMEDIATE_DIR)/ui/ui_resources/ui_unscaled_resources.rc', #cursor
		'<(SHARED_INTERMEDIATE_DIR)/ui/ui_resources/grit/ui_unscaled_resources.h',
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
	  'copies': [ {
         'destination': '<(PRODUCT_DIR)',
         'files' : [
            'bin/run_in_snapshot.exe',
            'bin/run_in_snapshot_xp.exe',
            'bin/run_in_snapshot_x64.exe',
            'bin/dbghelp.dll',
         ],
        },
      ],
    },
  ],
}
