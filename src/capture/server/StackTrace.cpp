#include "StackTrace.h"

/* Resolve symbol name and source location given the path to the executable 
   and an address */
int addr2line(char const * const program_name, void const * const addr)
{
  char addr2line_cmd[512] = {0};
 
  /* have addr2line map the address to the relent line in the code */
  #ifdef __APPLE__
    /* apple does things differently... */
    sprintf(addr2line_cmd,"atos -o %.256s %p", program_name, addr); 
  #else
    sprintf(addr2line_cmd,"addr2line -f -p -e %.256s %p", program_name, addr); 
  #endif
 
  /* This will print a nicely formatted string specifying the
     function and source line of the address */
  return system(addr2line_cmd);
}

static inline void printStackTrace( FILE *out = stderr, unsigned int max_frames = 63 )
{
  // storage array for stack trace address data
  void* trace[max_frames + 1];

  // retrieve current stack addresses
  unsigned int trace_size = backtrace( trace, sizeof( trace ) / sizeof( void* ));

  if ( trace_size == 0 )
  {
    fprintf( out, "  \n" );
    return;
  }

  // resolve addresses into strings containing "filename(function+address)",
  // Actually it will be ## program address function + offset
  // this array must be free()-ed
  char** messages = backtrace_symbols( trace, trace_size );

  size_t funcnamesize = 1024;
  char funcname[1024];

  fprintf(out, "STACKTRACE!! Execution path:\n");

  // iterate over the returned symbol lines. skip the first, it is the
  // address of this function.
  for ( unsigned int i = 4; i < trace_size; i++ )
  {
    char* begin_name   = NULL;
    char* begin_offset = NULL;
    char* end_offset   = NULL;

    // find parentheses and +address offset surrounding the mangled name
#ifdef DARWIN
    // OSX style stack trace
    for ( char *p = messages[i]; *p; ++p )
    {
      if (( *p == '_' ) && ( *(p - 1) == ' ' ))
        begin_name = p - 1;
      else if ( *p == '+' )
        begin_offset = p - 1;
    }

    if ( begin_name && begin_offset && ( begin_name < begin_offset ))
    {
      *begin_name++ = '\0';
      *begin_offset++ = '\0';

      // mangled name is now in [begin_name, begin_offset) and caller
      // offset in [begin_offset, end_offset). now apply
      // __cxa_demangle():
      int status;
      char* ret = abi::__cxa_demangle( begin_name, &funcname[0],
                                       &funcnamesize, &status );
      if ( status == 0 )
      {
        funcname = ret; // use possibly realloc()-ed string
        fprintf( out, "  %-30s %-40s %s\n",
                 messages[i], funcname, begin_offset );
      } else {
        // demangling failed. Output function name as a C function with
        // no arguments.
        fprintf( out, "  %-30s %-38s() %s\n",
                 messages[i], begin_name, begin_offset );
      }

#else // !DARWIN - but is posix
    // not OSX style
    // ./module(function+0x15c) [0x8048a6d]
    for ( char *p = messages[i]; *p; ++p )
    {
      if ( *p == '(' )
        begin_name = p;
      else if ( *p == '+' )
        begin_offset = p;
      else if ( *p == ')' && ( begin_offset || begin_name ))
        end_offset = p;
    }

    if ( begin_name && end_offset && ( begin_name < end_offset ))
    {
      *begin_name++   = '\0';
      *end_offset++   = '\0';
      if ( begin_offset )
        *begin_offset++ = '\0';

      // mangled name is now in [begin_name, begin_offset) and caller
      // offset in [begin_offset, end_offset). now apply
      // __cxa_demangle():

      int status = 0;
      char* ret = abi::__cxa_demangle( begin_name, funcname,
                                       &funcnamesize, &status );
      char* fname = begin_name;
      if ( status == 0 )
        fname = ret;

      if ( begin_offset )
      {
        fprintf( out, "  %-30s ( %-40s  + %-6s) %s\n",
                 messages[i], fname, begin_offset, end_offset );
      } else {
        fprintf( out, "  %-30s ( %-40s    %-6s) %s\n",
                 messages[i], fname, "", end_offset );
      }
#endif  // !DARWIN - but is posix
    } else {
      // couldn't parse the line? print the whole line.
      fprintf(out, "  %-40s\n", messages[i]);
    }
  }

  fprintf(out, "\nWith line numbers...\n");
  for (int i = 0; i < trace_size; ++i)
  {
    fprintf(out, "  %s\n", messages[i]);
    if (addr2line("main", trace[i]) != 0)
    {
      fprintf(out, "  error determining line # for: %s\n", messages[i]);
    }
 
  }

  free(messages);
}

void handler(int signum, siginfo_t *si, void *unused);

StackTrace::StackTrace() {
  struct sigaction sa;
  sa.sa_flags = SA_SIGINFO;
  sa.sa_sigaction = handler;
  sigemptyset(&sa.sa_mask);

  sigaction( SIGABRT, &sa, NULL);
  sigaction( SIGSEGV, &sa, NULL);
  sigaction( SIGBUS, &sa, NULL);
  sigaction( SIGILL, &sa, NULL);
  sigaction( SIGFPE, &sa, NULL);
  sigaction( SIGKILL, &sa, NULL);
  sigaction( SIGINT, &sa, NULL);
  sigaction( SIGPIPE, &sa, NULL);
}

void handler(int signum, siginfo_t *si, void *unused) {
  // associate each signal with a signal name string.
  const char* name = NULL;

  switch ( signum ) {
  case SIGABRT: name = "SIGABRT";  break;
  case SIGSEGV: name = "SIGSEGV";  break;
  case SIGBUS:  name = "SIGBUS";   break;
  case SIGILL:  name = "SIGILL";   break;
  case SIGFPE:  name = "SIGFPE";   break;
  case SIGKILL: name = "SIGKILL";  break;
  case SIGINT:  name = "SIGINT";   break;
  case SIGPIPE:  name = "SIGPIPE";  break;
  }

  // Notify the user which signal was caught. We use printf, because this is the
  // most basic output function. Once you get a crash, it is possible that more
  // complex output systems like streams and the like may be corrupted. So we
  // make the most basic call possible to the lowest level, most
  // standard print function.
  if ( name )
    fprintf( stderr, "Caught signal %d (%s)\n", signum, name );
  else
    fprintf( stderr, "Caught signal %d\n", signum );

  // Dump a stack trace.
  // This is the function we will be implementing next.
  printStackTrace();

  // If you caught one of the above signals, it is likely you just
  // want to quit your program right now.
  exit( signum );
}

// #include "StackTrace.h"
 
// #define MAX_STACK_FRAMES 64

// static void *trace[MAX_STACK_FRAMES];
// void posix_print_stack_trace()
// {
//   int i, trace_size = 0;
//   char **messages = (char **)NULL;
 
//   trace_size = backtrace(trace, MAX_STACK_FRAMES);
//   messages = backtrace_symbols(trace, trace_size);
 
//   /* skip the first couple stack frames (as they are this function and
//      our handler) and also skip the last frame as it's (always?) junk. */
//   // for (i = 3; i < (trace_size - 1); ++i)
//   // we'll use this for now so you can see what's going on
//   for (i = 0; i < trace_size; ++i)
//   {
//     printf("%s\n", messages[i]);
//     if (addr2line("main", trace[i]) != 0)
//     {
//       printf("  error determining line # for: %s\n", messages[i]);
//     }
//     printf("\n");
 
//   }
//   if (messages) { free(messages); } 
// }
 
// void posix_signal_handler(int sig, siginfo_t *siginfo, void *context)
// {
//   (void)context;
//   switch(sig)
//   {
//     case SIGSEGV:
//       fputs("Caught SIGSEGV: Segmentation Fault\n", stderr);
//       break;
//     case SIGINT:
//       fputs("Caught SIGINT: Interactive attention signal, (usually ctrl+c)\n",
//             stderr);
//       break;
//     case SIGFPE:
//       switch(siginfo->si_code)
//       {
//         case FPE_INTDIV:
//           fputs("Caught SIGFPE: (integer divide by zero)\n", stderr);
//           break;
//         case FPE_INTOVF:
//           fputs("Caught SIGFPE: (integer overflow)\n", stderr);
//           break;
//         case FPE_FLTDIV:
//           fputs("Caught SIGFPE: (floating-point divide by zero)\n", stderr);
//           break;
//         case FPE_FLTOVF:
//           fputs("Caught SIGFPE: (floating-point overflow)\n", stderr);
//           break;
//         case FPE_FLTUND:
//           fputs("Caught SIGFPE: (floating-point underflow)\n", stderr);
//           break;
//         case FPE_FLTRES:
//           fputs("Caught SIGFPE: (floating-point inexact result)\n", stderr);
//           break;
//         case FPE_FLTINV:
//           fputs("Caught SIGFPE: (floating-point invalid operation)\n", stderr);
//           break;
//         case FPE_FLTSUB:
//           fputs("Caught SIGFPE: (subscript out of range)\n", stderr);
//           break;
//         default:
//           fputs("Caught SIGFPE: Arithmetic Exception\n", stderr);
//           break;
//       }
//     case SIGILL:
//       switch(siginfo->si_code)
//       {
//         case ILL_ILLOPC:
//           fputs("Caught SIGILL: (illegal opcode)\n", stderr);
//           break;
//         case ILL_ILLOPN:
//           fputs("Caught SIGILL: (illegal operand)\n", stderr);
//           break;
//         case ILL_ILLADR:
//           fputs("Caught SIGILL: (illegal addressing mode)\n", stderr);
//           break;
//         case ILL_ILLTRP:
//           fputs("Caught SIGILL: (illegal trap)\n", stderr);
//           break;
//         case ILL_PRVOPC:
//           fputs("Caught SIGILL: (privileged opcode)\n", stderr);
//           break;
//         case ILL_PRVREG:
//           fputs("Caught SIGILL: (privileged register)\n", stderr);
//           break;
//         case ILL_COPROC:
//           fputs("Caught SIGILL: (coprocessor error)\n", stderr);
//           break;
//         case ILL_BADSTK:
//           fputs("Caught SIGILL: (internal stack error)\n", stderr);
//           break;
//         default:
//           fputs("Caught SIGILL: Illegal Instruction\n", stderr);
//           break;
//       }
//       break;
//     case SIGTERM:
//       fputs("Caught SIGTERM: a termination request was sent to the program\n",
//             stderr);
//       break;
//     case SIGABRT:
//       fputs("Caught SIGABRT: usually caused by an abort() or assert()\n", stderr);
//       break;
//     default:
//       break;
//   }
//   posix_print_stack_trace();
//   _Exit(1);
// }
 
// static uint8_t alternate_stack[SIGSTKSZ];
// void set_signal_handler(int sig, siginfo_t * siginfo, void * context)
// {
//   /* setup alternate stack */
//   {
//     stack_t ss = {};
//     /* malloc is usually used here, I'm not 100% sure my static allocation
//        is valid but it seems to work just fine. */
//     ss.ss_sp = (void*)alternate_stack;
//     ss.ss_size = SIGSTKSZ;
//     ss.ss_flags = 0;
 
//     if (sigaltstack(&ss, NULL) != 0) { err(1, "sigaltstack"); }
//   }
 
//   /* register our signal handlers */
//   {
//     struct sigaction sig_action = {};
//     sig_action.sa_sigaction = posix_signal_handler;
//     sigemptyset(&sig_action.sa_mask);
 
//     #ifdef __APPLE__
//         /* for some reason we backtrace() doesn't work on osx
//            when we use an alternate stack */
//         sig_action.sa_flags = SA_SIGINFO;
//     #else
//         sig_action.sa_flags = SA_SIGINFO | SA_ONSTACK;
//     #endif
 
//     if (sigaction(SIGSEGV, &sig_action, NULL) != 0) { err(1, "sigaction"); }
//     if (sigaction(SIGFPE,  &sig_action, NULL) != 0) { err(1, "sigaction"); }
//     if (sigaction(SIGINT,  &sig_action, NULL) != 0) { err(1, "sigaction"); }
//     if (sigaction(SIGILL,  &sig_action, NULL) != 0) { err(1, "sigaction"); }
//     if (sigaction(SIGTERM, &sig_action, NULL) != 0) { err(1, "sigaction"); }
//     if (sigaction(SIGABRT, &sig_action, NULL) != 0) { err(1, "sigaction"); }
//   }
// }

// StackTrace::StackTrace() {
//   struct sigaction sa;
//   sa.sa_flags = SA_SIGINFO;
//   sa.sa_sigaction = set_signal_handler;
//   sigemptyset(&sa.sa_mask);

//   sigaction( SIGABRT, &sa, NULL);
//   sigaction( SIGSEGV, &sa, NULL);
//   sigaction( SIGBUS, &sa, NULL);
//   sigaction( SIGILL, &sa, NULL);
//   sigaction( SIGFPE, &sa, NULL);
//   sigaction( SIGKILL, &sa, NULL);
//   sigaction( SIGINT, &sa, NULL);
// }