#include "tools/profiler/ui/profiler_ui_start.h"
#include "tools/profiler/ui/profiler_window.h"
#include "tools/profiler/result_codes.h"

ProfilerUIStart::ProfilerUIStart()
{

}

ProfilerUIStart::~ProfilerUIStart()
{

}

bool ProfilerUIStart::Start(const CommandLine& cmdline,int* result_code)
{
	views::profiler::ShowProfilerWindow(views::profiler::QUIT_ON_CLOSE);
	views::profiler::LogStatus("hi,profiler!");

  return true;
}
