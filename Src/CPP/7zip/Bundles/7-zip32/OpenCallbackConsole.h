// OpenCallbackConsole.h

#ifndef ZIP7_INC_OPEN_CALLBACK_CONSOLE_H
#define ZIP7_INC_OPEN_CALLBACK_CONSOLE_H

#include "Common/StdOutStream.h"			// 変更

#include "../../UI/Common/ArchiveOpenCallback.h"	// 変更

//#include "PercentPrinter.h"	// 削除

class COpenCallbackConsole: public IOpenCallbackUI
{
protected:
//  CPercentPrinter _percent;	// 削除

  CStdOutStream *_so;
  CStdOutStream *_se;

  // UInt64 _totalFiles;
  UInt64 _totalBytes;
  bool _totalFilesDefined;
  // bool _totalBytesDefined;

//  bool NeedPercents() const { return _percent._so && !_percent.DisablePrint; }	// 削除
  bool NeedPercents() const { return false; }	// 追加


public:

  bool MultiArcMode;

  void ClosePercents()
  {
  /* 削除ここから
    if (NeedPercents())
      _percent.ClosePrint(true);
     削除ここまで */
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
//    _percent._so = percentStream;	// 削除
//    _percent.DisablePrint = disablePercents;	// 削除
  }

  Z7_IFACE_IMP(IOpenCallbackUI)
  
  #ifndef Z7_NO_CRYPTO
  bool PasswordIsDefined;
  // bool PasswordWasAsked;
  UString Password;
  #endif
};

#endif
