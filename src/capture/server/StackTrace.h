#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <err.h>
#include <execinfo.h>
#include <stdio.h>
#include <errno.h>
#include <cxxabi.h>

#ifndef STACKTRACE_H
#define STACKTRACE_H

class StackTrace
{
public:
  StackTrace();
};

#endif