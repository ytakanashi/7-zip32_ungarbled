// ExtractCallbackConsole.h

#ifndef __EXTRACT_CALLBACK_CONSOLE_H
#define __EXTRACT_CALLBACK_CONSOLE_H

#include "Common/StdOutStream.h"		// ïœçX

#include "../../IPassword.h"

#include "../../Archive/IArchive.h"

#include "../../UI/Common/ArchiveExtractCallback.h"	// ïœçX

//#include "PercentPrinter.h"	// çÌèú

#include "OpenCallbackConsole.h"

class CExtractScanConsole: public IDirItemsCallback
{
  CStdOutStream *_so;
  CStdOutStream *_se;
//  CPercentPrinter _percent;	// çÌèú

//  bool NeedPercents() const { return _percent._so != NULL; }	// çÌèú
  bool NeedPercents() const { return false; }	// í«â¡
  
  void ClosePercentsAndFlush()
  {
 /* çÌèúÇ±Ç±Ç©ÇÁ
    if (NeedPercents())
      _percent.ClosePrint(true);
 çÌèúÇ±Ç±Ç‹Ç≈ */
    if (_so)
      _so->Flush();
  }

public:
  void Init(CStdOutStream *outStream, CStdOutStream *errorStream, CStdOutStream *percentStream)
  {
    _so = outStream;
    _se = errorStream;
//    _percent._so = percentStream;	// çÌèú
  }
  
//  void SetWindowWidth(unsigned width) { _percent.MaxLen = width - 1; }	// çÌèú
  void SetWindowWidth(unsigned width) { }	// í«â¡

  void StartScanning();
  
  INTERFACE_IDirItemsCallback(;)

  void CloseScanning()
  {
 /* çÌèúÇ±Ç±Ç©ÇÁ
    if (NeedPercents())
      _percent.ClosePrint(true);
     çÌèúÇ±Ç±Ç‹Ç≈ */
  }

  void PrintStat(const CDirItemsStat &st);
};




class CExtractCallbackConsole:
  public IExtractCallbackUI,
  // public IArchiveExtractCallbackMessage,
  public IFolderArchiveExtractCallback2,
  #ifndef _NO_CRYPTO
  public ICryptoGetTextPassword,
  #endif
  public COpenCallbackConsole,
  public CMyUnknownImp
{
  AString _tempA;
  UString _tempU;

  UString _currentName;

  void ClosePercents_for_so()
  {
 /* çÌèúÇ±Ç±Ç©ÇÁ
    if (NeedPercents() && _so == _percent._so)
      _percent.ClosePrint(false);
 çÌèúÇ±Ç±Ç‹Ç≈ */
  }
  
  void ClosePercentsAndFlush()
  {
 /* çÌèúÇ±Ç±Ç©ÇÁ
    if (NeedPercents())
      _percent.ClosePrint(true);
 çÌèúÇ±Ç±Ç‹Ç≈ */
    if (_so)
      _so->Flush();
  }

public:
  MY_QUERYINTERFACE_BEGIN2(IFolderArchiveExtractCallback)
  // MY_QUERYINTERFACE_ENTRY(IArchiveExtractCallbackMessage)
  MY_QUERYINTERFACE_ENTRY(IFolderArchiveExtractCallback2)
  #ifndef _NO_CRYPTO
  MY_QUERYINTERFACE_ENTRY(ICryptoGetTextPassword)
  #endif
  MY_QUERYINTERFACE_END
  MY_ADDREF_RELEASE

  STDMETHOD(SetTotal)(UInt64 total);
  STDMETHOD(SetCompleted)(const UInt64 *completeValue);

  INTERFACE_IFolderArchiveExtractCallback(;)

  INTERFACE_IExtractCallbackUI(;)
  // INTERFACE_IArchiveExtractCallbackMessage(;)
  INTERFACE_IFolderArchiveExtractCallback2(;)

  #ifndef _NO_CRYPTO

  STDMETHOD(CryptoGetTextPassword)(BSTR *password);

  #endif
  
  UInt64 NumTryArcs;
  
  bool ThereIsError_in_Current;
  bool ThereIsWarning_in_Current;

  UInt64 NumOkArcs;
  UInt64 NumCantOpenArcs;
  UInt64 NumArcsWithError;
  UInt64 NumArcsWithWarnings;

  UInt64 NumOpenArcErrors;
  UInt64 NumOpenArcWarnings;
  
  UInt64 NumFileErrors;
  UInt64 NumFileErrors_in_Current;

  bool NeedFlush;
  unsigned PercentsNameLevel;
  unsigned LogLevel;

  CExtractCallbackConsole():
      NeedFlush(false),
      PercentsNameLevel(1),
      LogLevel(0)
      {}

//  void SetWindowWidth(unsigned width) { _percent.MaxLen = width - 1; }	// çÌèú
  void SetWindowWidth(unsigned width) { }	// í«â¡


  void Init(CStdOutStream *outStream, CStdOutStream *errorStream, CStdOutStream *percentStream)
  {
    COpenCallbackConsole::Init(outStream, errorStream, percentStream);

    NumTryArcs = 0;
    
    ThereIsError_in_Current = false;
    ThereIsWarning_in_Current = false;

    NumOkArcs = 0;
    NumCantOpenArcs = 0;
    NumArcsWithError = 0;
    NumArcsWithWarnings = 0;

    NumOpenArcErrors = 0;
    NumOpenArcWarnings = 0;
    
    NumFileErrors = 0;
    NumFileErrors_in_Current = 0;
  }
};

#endif
