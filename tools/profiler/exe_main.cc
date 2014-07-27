#include <windows.h>

#include "base/at_exit.h"
#include "base/command_line.h"
#include "base/logging.h"
#include "base/process/memory.h"

#include "tools/profiler/profiler_logging.h"
#include "tools/profiler/ui/exe_main_ui.h"

extern int _cdecl ProfilerMain(const CommandLine& command_line);
extern int _cdecl IsNeedElevate(bool);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine, int nCmdShow) 
{
  int argc = 0;
  char** argv = NULL;

  base::AtExitManager exit_manager;
  CommandLine::Init(argc, argv);
  base::EnableTerminationOnHeapCorruption();
	const CommandLine& command_line = *CommandLine::ForCurrentProcess();

  profiler_ui::LowLevelInit(hInstance);

#ifndef NDEBUG
  if(!IsNeedElevate(false)) {
    return 0;
  }
#else
  if(!IsNeedElevate(true)) {
    return 0;
  }
#endif

  logging::OldFileDeletionState file_state = logging::APPEND_TO_OLD_LOG_FILE;
  logging::InitProfilerLogging(command_line, file_state);

  int exit_code = ProfilerMain(command_line);

  logging::CleanupProfilerLogging();

  profiler_ui::LowLevelShutdown();

  return exit_code;
}
