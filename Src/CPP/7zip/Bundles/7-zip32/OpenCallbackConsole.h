// OpenCallbackConsole.h

#ifndef ZIP7_INC_OPEN_CALLBACK_CONSOLE_H
#define ZIP7_INC_OPEN_CALLBACK_CONSOLE_H

#include "Common/StdOutStream.h"			// •ÏX

#include "../../UI/Common/ArchiveOpenCallback.h"	// •ÏX

//#include "PercentPrinter.h"	// íœ

class COpenCallbackConsole: public IOpenCallbackUI
{
protected:
//  CPercentPrinter _percent;	// íœ

  CStdOutStream *_so;
  CStdOutStream *_se;

  // UInt64 _totalFiles;
  UInt64 _totalBytes;
  bool _totalFilesDefined;
  // bool _totalBytesDefined;

//  bool NeedPercents() const { return _percent._so && !_percent.DisablePrint; }	// íœ
  bool NeedPercents() const { return false; }	// ’Ç‰Á


public:

  bool MultiArcMode;

  void ClosePercents()
  {
  /* íœ‚±‚±‚©‚ç
    if (NeedPercents())
      _percent.ClosePrint(true);
     íœ‚±‚±‚Ü‚Å */
  }

  COpenCallbackConsole():
      _totalBytes(0),
      _totalFilesDefined(false),
      // _totalBytesDefined(false),
      MultiArcMode(false)
      
      #ifndef Z7_NO_CRYPTO
      , PasswordIsDefined(false)
      // , PasswordWasAsked(false)
      #endif
      
      {}

  virtual ~COpenCallbackConsole() {}
  
  void Init(
      CStdOutStream *outStream,
      CStdOutStream *errorStream,
      CStdOutStream *percentStream,
      bool disablePercents)
  {
    _so = outStream;
    _se = errorStream;
//    _percent._so = percentStream;	// íœ
//    _percent.DisablePrint = disablePercents;	// íœ
  }

  Z7_IFACE_IMP(IOpenCallbackUI)
  
  #ifndef Z7_NO_CRYPTO
  bool PasswordIsDefined;
  // bool PasswordWasAsked;
  UString Password;
  #endif
};

#endif
