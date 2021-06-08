// UpdateCallbackConsole.h

#ifndef __UPDATE_CALLBACK_CONSOLE_H
#define __UPDATE_CALLBACK_CONSOLE_H

#include "Common/StdOutStream.h"	// �ύX

#include "../../UI/Common/Update.h"		// �p�X�ύX

//#include "PercentPrinter.h"	// �폜

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
protected:
//  CPercentPrinter _percent;	// �폜

  CStdOutStream *_so;
  CStdOutStream *_se;

  void CommonError(const FString &path, DWORD systemError, bool isWarning);
  
  HRESULT ScanError_Base(const FString &path, DWORD systemError);
  HRESULT OpenFileError_Base(const FString &name, DWORD systemError);
  HRESULT ReadingFileError_Base(const FString &name, DWORD systemError);

public:
//  bool NeedPercents() const { return _percent._so != NULL; };	// �폜
  bool NeedPercents() const { return false; };	// �ǉ�

  bool StdOutMode;

  bool NeedFlush;
  unsigned PercentsNameLevel;
  unsigned LogLevel;

  AString _tempA;
  UString _tempU;

  CCallbackConsoleBase():
      StdOutMode(false),
      NeedFlush(false),
      PercentsNameLevel(1),
      LogLevel(0),
      NumNonOpenFiles(0)
      {}
  
//  void SetWindowWidth(unsigned width) { _percent.MaxLen = width - 1; }	// �폜
  void SetWindowWidth(unsigned width) { }	// �ǉ�

  void Init(CStdOutStream *outStream, CStdOutStream *errorStream, CStdOutStream *percentStream)
  {
    FailedFiles.Clear();

    _so = outStream;
    _se = errorStream;
//    _percent._so = percentStream;	// �폜
  }

  void ClosePercents2()
  {
 /* �폜��������
    if (NeedPercents())
      _percent.ClosePrint(true);
 �폜�����܂� */
  }

  void ClosePercents_for_so()
  {
 /* �폜��������
    if (NeedPercents() && _so == _percent._so)
      _percent.ClosePrint(false);
 �폜�����܂� */
  }

  CErrorPathCodes FailedFiles;
  CErrorPathCodes ScanErrors;
  UInt64 NumNonOpenFiles;

  HRESULT PrintProgress(const wchar_t *name, const char *command, bool showInLog);

};

class CUpdateCallbackConsole: public IUpdateCallbackUI2, public CCallbackConsoleBase
{
  // void PrintPropPair(const char *name, const wchar_t *val);

public:
  bool DeleteMessageWasShown;

  #ifndef _NO_CRYPTO
  bool PasswordIsDefined;
  UString Password;
  bool AskPassword;
  #endif

  CUpdateCallbackConsole():
      DeleteMessageWasShown(false)
      #ifndef _NO_CRYPTO
      , PasswordIsDefined(false)
      , AskPassword(false)
      #endif
      {}

  virtual ~CUpdateCallbackConsole() {}
  
  /*
  void Init(CStdOutStream *outStream)
  {
    CCallbackConsoleBase::Init(outStream);
  }
  */
  // ~CUpdateCallbackConsole() { if (NeedPercents()) _percent.ClosePrint(); }
  INTERFACE_IUpdateCallbackUI2(;)
};

#endif
