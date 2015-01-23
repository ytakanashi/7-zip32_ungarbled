// OpenCallbackConsole.h

#ifndef __OPEN_CALLBACK_CONSOLE_H
#define __OPEN_CALLBACK_CONSOLE_H

#include "Common/StdOutStream.h"			// ïœçX

#include "../../UI/Common/ArchiveOpenCallback.h"	// ïœçX

class COpenCallbackConsole: public IOpenCallbackUI
{
public:
  INTERFACE_IOpenCallbackUI(;)
  
  CStdOutStream *OutStream;

  #ifndef _NO_CRYPTO
  bool PasswordIsDefined;
  bool PasswordWasAsked;
  UString Password;
  COpenCallbackConsole(): PasswordIsDefined(false), PasswordWasAsked(false) {}
  #endif
};

#endif
