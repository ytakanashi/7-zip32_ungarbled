// OpenCallbackConsole.h

#ifndef __OPEN_CALLBACK_CONSOLE_H
#define __OPEN_CALLBACK_CONSOLE_H

#include "Common/StdOutStream.h"			// 変更

#include "../../UI/Common/ArchiveOpenCallback.h"	// 変更

//#include "PercentPrinter.h"	// 削除

class COpenCallbackConsole: public IOpenCallbackUI
{
protected:
//  CPercentPrinter _percent;	// 削除

  CStdOutStream *_so;
  CStdOutStream *_se;

  bool _totalFilesDefined;
  // bool _totalBytesDefined;
  // UInt64 _totalFiles;
  UInt64 _totalBytes;

//  bool NeedPercents() const { return _percent._so != NULL; }	// 削除
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
      _totalFilesDefined(false),
      // _totalBytesDefined(false),
      _totalBytes(0),
      MultiArcMode(false)
      
      #ifndef _NO_CRYPTO
      , PasswordIsDefined(false)
      // , PasswordWasAsked(false)
      #endif
      
      {}

  virtual ~COpenCallbackConsole() {}
  
  void Init(CStdOutStream *outStream, CStdOutStream *errorStream, CStdOutStream *percentStream)
  {
    _so = outStream;
    _se = errorStream;
//    _percent._so = percentStream;	// 削除
  }

  INTERFACE_IOpenCallbackUI(;)
  
  #ifndef _NO_CRYPTO
  bool PasswordIsDefined;
  // bool PasswordWasAsked;
  UString Password;
  #endif
};

#endif
