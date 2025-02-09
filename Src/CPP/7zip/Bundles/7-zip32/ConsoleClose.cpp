// ConsoleClose.cpp

#include "StdAfx.h"

#include "ConsoleClose.h"

#ifndef UNDER_CE

#ifdef _WIN32
#include "../../../Common/MyWindows.h"
#else
#include <stdlib.h>
#include <signal.h>
#endif

// namespace NConsoleClose {							// íœ

unsigned g_BreakCounter = 0;
static const unsigned kBreakAbortThreshold = 3;

#ifdef _WIN32

//static BOOL WINAPI HandlerRoutine(DWORD ctrlType)		// íœ
BOOL HandlerRoutine()									// ’Ç‰Á
{
  /* íœ‚±‚±‚©‚ç
  if (ctrlType == CTRL_LOGOFF_EVENT)
  {
    // printf("\nCTRL_LOGOFF_EVENT\n");
    return TRUE;
  }
     íœ‚±‚±‚Ü‚Å */ 

  if (++g_BreakCounter < kBreakAbortThreshold)
    return TRUE;
  return FALSE;
  /*
  switch (ctrlType)
  {
    case CTRL_C_EVENT:
    case CTRL_BREAK_EVENT:
      if (g_BreakCounter < kBreakAbortThreshold)
      return TRUE;
  }
  return FALSE;
  */
}
namespace NConsoleClose {			// ’Ç‰Á

CCtrlHandlerSetter::CCtrlHandlerSetter()
{
//  if (!SetConsoleCtrlHandler(HandlerRoutine, TRUE))	// íœ
//    throw 1019; // "SetConsoleCtrlHandler fails";		// íœ
}

CCtrlHandlerSetter::~CCtrlHandlerSetter()
{
//  if (!SetConsoleCtrlHandler(HandlerRoutine, FALSE))	// íœ
  {
    // warning for throw in destructor.
    // throw "SetConsoleCtrlHandler fails";
  }
  g_BreakCounter = 0;									// ’Ç‰Á
}

#else // _WIN32

static void HandlerRoutine(int)
{
  if (++g_BreakCounter < kBreakAbortThreshold)
    return;
  exit(EXIT_FAILURE);
}

CCtrlHandlerSetter::CCtrlHandlerSetter()
{
  memo_sig_int = signal(SIGINT, HandlerRoutine); // CTRL-C
  if (memo_sig_int == SIG_ERR)
    throw "SetConsoleCtrlHandler fails (SIGINT)";
  memo_sig_term = signal(SIGTERM, HandlerRoutine); // for kill -15 (before "kill -9")
  if (memo_sig_term == SIG_ERR)
    throw "SetConsoleCtrlHandler fails (SIGTERM)";
}

CCtrlHandlerSetter::~CCtrlHandlerSetter()
{
  signal(SIGINT, memo_sig_int); // CTRL-C
  signal(SIGTERM, memo_sig_term); // kill {pid}
}

#endif // _WIN32

/*
void CheckCtrlBreak()
{
  if (TestBreakSignal())
    throw CCtrlBreakException();
}
*/

}

#endif
