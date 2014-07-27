#pragma once

#include "base/basictypes.h"

class CommandLine;

class ProfilerUIStart{
public:
  ProfilerUIStart();
  ~ProfilerUIStart();

public:
  bool Start(const CommandLine& cmdline,int* result_code);

private:
  DISALLOW_COPY_AND_ASSIGN(ProfilerUIStart);
};
