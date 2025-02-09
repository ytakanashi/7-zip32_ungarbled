// ConsoleClose.h

#ifndef __CONSOLE_CLOSE_H
#define __CONSOLE_CLOSE_H

namespace NConsoleClose {

// class CCtrlBreakException {};
}	// ’Ç‰Á
#ifdef UNDER_CE

inline bool TestBreakSignal() { return false; }
struct CCtrlHandlerSetter {};

#else

extern unsigned g_BreakCounter;
namespace NConsoleClose {	// ’Ç‰Á

inline bool TestBreakSignal()
{
  return (g_BreakCounter != 0);
}

class CCtrlHandlerSetter
{
  #ifndef _WIN32
  void (*memo_sig_int)(int);
  void (*memo_sig_term)(int);
  #endif
public:
  CCtrlHandlerSetter();
  virtual ~CCtrlHandlerSetter();
};

#endif

}

#endif
