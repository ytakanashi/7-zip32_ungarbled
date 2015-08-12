// ConsoleClose.cpp

#include "StdAfx.h"

#include "ConsoleClose.h"

#if !defined(UNDER_CE) && defined(_WIN32)
#include "../../../Common/MyWindows.h"
#endif

//namespace NConsoleClose {							// íœ

unsigned g_BreakCounter = 0;
static const unsigned kBreakAbortThreshold = 2;

#if !defined(UNDER_CE) && defined(_WIN32)
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

  g_BreakCounter++;
  if (g_BreakCounter < kBreakAbortThreshold)
    return TRUE;
  return FALSE;
  /*
  switch(ctrlType)
  {
    case CTRL_C_EVENT:
    case CTRL_BREAK_EVENT:
      if (g_BreakCounter < kBreakAbortThreshold)
      return TRUE;
  }
  return FALSE;
  */
}
#endif

namespace NConsoleClose {			// ’Ç‰Á
/*
void CheckCtrlBreak()
{
  if (TestBreakSignal())
    throw CCtrlBreakException();
}
*/

CCtrlHandlerSetter::CCtrlHandlerSetter()
{
  #if !defined(UNDER_CE) && defined(_WIN32)
//  if(!SetConsoleCtrlHandler(HandlerRoutine, TRUE))	// íœ
//    throw "SetConsoleCtrlHandler fails";				// íœ
  #endif
}

CCtrlHandlerSetter::~CCtrlHandlerSetter()
{
  #if !defined(UNDER_CE) && defined(_WIN32)
//  if(!SetConsoleCtrlHandler(HandlerRoutine, FALSE))	// íœ
//    throw "SetConsoleCtrlHandler fails";				// íœ
	g_BreakCounter = 0;									// ’Ç‰Á
  #endif
}

}
