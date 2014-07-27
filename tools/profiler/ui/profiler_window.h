#pragma once

#include <string>

namespace views {
namespace profiler {

enum Operation {
  DO_NOTHING_ON_CLOSE = 0,
  QUIT_ON_CLOSE,
};

// Shows a window with the views profiler in it.
void ShowProfilerWindow(Operation operation);

// Log
void LogStatus(const std::string& string) ;

}  // namespace profiler
}  // namespace views
