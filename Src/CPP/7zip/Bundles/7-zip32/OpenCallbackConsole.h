// OpenCallbackConsole.h

#ifndef __OPEN_CALLBACK_CONSOLE_H
#define __OPEN_CALLBACK_CONSOLE_H

#include "Common/StdOutStream.h"			// •ÏX

#include "../../UI/Common/ArchiveOpenCallback.h"	// •ÏX

//#include "PercentPrinter.h"	// íœ

class COpenCallbackConsole: public IOpenCallbackUI
{
protected:
//  CPercentPrinter _percent;	// íœ

  CStdOutStream *_so;
  CStdOutStream *_se;

  bool _totalFilesDefined;
  bool _totalBytesDefined;
  // UInt64 _totalFiles;
  // UInt64 _totalBytes;

//  bool NeedPercents() const { return _percent._so != NULL; }	// íœ
  bool NeedPercents() const { return false; }	// ’Ç‰Á


public:

  bool MutiArcMode;

  void ClosePercents()
  {
  /* íœ‚±‚±‚©‚ç
    if (NeedPercents())
      _percent.ClosePrint(true);
     íœ‚±‚±‚Ü‚Å */
  }

  COpenCallbackConsole():
      _totalFilesDefined(false),
      _totalBytesDefined(false),
      MutiArcMode(false)
      
      #ifndef _NO_CRYPTO
      , PasswordIsDefined(false)
      // , PasswordWasAsked(false)
      #endif
      
      {}
  
  void Init(CStdOutStream *outStream, CStdOutStream *errorStream, CStdOutStream *percentStream)
  {
    _so = outStream;
    _se = errorStream;
//    _percent._so = percentStream;	// íœ
  }

  INTERFACE_IOpenCallbackUI(;)
  
  #ifndef _NO_CRYPTO
  bool PasswordIsDefined;
  // bool PasswordWasAsked;
  UString Password;
  #endif
};

#endif
