// OpenCallbackConsole.h

#ifndef ZIP7_INC_OPEN_CALLBACK_CONSOLE_H
#define ZIP7_INC_OPEN_CALLBACK_CONSOLE_H

#include "Common/StdOutStream.h"			// ïœçX

#include "../../UI/Common/ArchiveOpenCallback.h"	// ïœçX

//#include "PercentPrinter.h"	// çÌèú

class COpenCallbackConsole: public IOpenCallbackUI
{
protected:
//  CPercentPrinter _percent;	// çÌèú

  CStdOutStream *_so;
  CStdOutStream *_se;

  // UInt64 _totalFiles;
  UInt64 _totalBytes;
  bool _totalFilesDefined;
  // bool _totalBytesDefined;

//  bool NeedPercents() const { return _percent._so != NULL; }	// çÌèú
  bool NeedPercents() const { return false; }	// í«â¡


public:

  bool MultiArcMode;

  void ClosePercents()
  {
  /* çÌèúÇ±Ç±Ç©ÇÁ
    if (NeedPercents())
      _percent.ClosePrint(true);
     çÌèúÇ±Ç±Ç‹Ç≈ */
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
  
  void Init(CStdOutStream *outStream, CStdOutStream *errorStream, CStdOutStream *percentStream)
  {
    _so = outStream;
    _se = errorStream;
//    _percent._so = percentStream;	// çÌèú
  }

  Z7_IFACE_IMP(IOpenCallbackUI)
  
  #ifndef Z7_NO_CRYPTO
  bool PasswordIsDefined;
  // bool PasswordWasAsked;
  UString Password;
  #endif
};

#endif
