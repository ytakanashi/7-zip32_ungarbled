// UpdateCallbackConsole.h

#ifndef ZIP7_INC_UPDATE_CALLBACK_CONSOLE_H
#define ZIP7_INC_UPDATE_CALLBACK_CONSOLE_H

#include "Common/StdOutStream.h"	// 変更

#include "../../UI/Common/Update.h"		// パス変更

//#include "PercentPrinter.h"	// 削除

struct CErrorPathCodes
{
  FStringVector Paths;
  CRecordVector<DWORD> Codes;

  void AddError(const FString &path, DWORD systemError)
  {
    Paths.Add(path);
    Codes.Add(systemError);
  }
  void Clear()
  {
    Paths.Clear();
    Codes.Clear();
  }
};


class CCallbackConsoleBase
{
  void CommonError(const FString &path, DWORD systemError, bool isWarning);

protected:
  CStdOutStream *_so;
  CStdOutStream *_se;

  HRESULT ScanError_Base(const FString &path, DWORD systemError);
  HRESULT OpenFileError_Base(const FString &name, DWORD systemError);
  HRESULT ReadingFileError_Base(const FString &name, DWORD systemError);

public:
  bool StdOutMode;
  bool NeedFlush;
  unsigned PercentsNameLevel;
  unsigned LogLevel;

protected:
  AString _tempA;
  UString _tempU;
//  CPercentPrinter _percent;	// 削除

public:
  CErrorPathCodes FailedFiles;
  CErrorPathCodes ScanErrors;
  UInt64 NumNonOpenFiles;

  CCallbackConsoleBase():
      StdOutMode(false),
      NeedFlush(false),
      PercentsNameLevel(1),
      LogLevel(0),
      NumNonOpenFiles(0)
      {}
  
//  bool NeedPercents() const { return _percent._so != NULL; }	// 削除
  bool NeedPercents() const { return false; };	// 追加
//  void SetWindowWidth(unsigned width) { _percent.MaxLen = width - 1; }	// 削除
  void SetWindowWidth(unsigned width) { }	// 追加

  void Init(
      CStdOutStream *outStream,
      CStdOutStream *errorStream,
      CStdOutStream *percentStream,
      bool disablePercents)
  {
    FailedFiles.Clear();

    _so = outStream;
    _se = errorStream;
//    _percent._so = percentStream;	// 削除
//    _percent.DisablePrint = disablePercents;	// 削除
  }

  void ClosePercents2()
  {
 /* 削除ここから
    if (NeedPercents())
      _percent.ClosePrint(true);
 削除ここまで */
  }

  void ClosePercents_for_so()
  {
 /* 削除ここから
    if (NeedPercents() && _so == _percent._so)
      _percent.ClosePrint(false);
 削除ここまで */
  }

  HRESULT PrintProgress(const wchar_t *name, bool isDir, const char *command, bool showInLog);

  // void PrintInfoLine(const UString &s);
  // void PrintPropInfo(UString &s, PROPID propID, const PROPVARIANT *value);
};


class CUpdateCallbackConsole Z7_final:
  public IUpdateCallbackUI2,
  public CCallbackConsoleBase
{
  // void PrintPropPair(const char *name, const wchar_t *val);
  Z7_IFACE_IMP(IUpdateCallbackUI)
  Z7_IFACE_IMP(IDirItemsCallback)
  Z7_IFACE_IMP(IUpdateCallbackUI2)

  HRESULT MoveArc_UpdateStatus();

  UInt64 _arcMoving_total;
  UInt64 _arcMoving_current;
  UInt64 _arcMoving_percents;
  Int32  _arcMoving_updateMode;

public:
  bool DeleteMessageWasShown;

  #ifndef Z7_NO_CRYPTO
  bool PasswordIsDefined;
  bool AskPassword;
  UString Password;
  #endif

  CUpdateCallbackConsole():
        _arcMoving_total(0)
      , _arcMoving_current(0)
      , _arcMoving_percents(0)
      , _arcMoving_updateMode(0)
      , DeleteMessageWasShown(false)
      #ifndef Z7_NO_CRYPTO
      , PasswordIsDefined(false)
      , AskPassword(false)
      #endif
      {}

  /*
  void Init(CStdOutStream *outStream)
  {
    CCallbackConsoleBase::Init(outStream);
  }
  */
  // ~CUpdateCallbackConsole() { if (NeedPercents()) _percent.ClosePrint(); }
};

#endif
