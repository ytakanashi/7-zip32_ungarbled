// OpenCallbackConsole.cpp

#include "StdAfx.h"

#include "OpenCallbackConsole.h"

#include "ConsoleClose.h"
//#include "UserInputUtils.h"						// 削除

#include "resource.h"								// 追加

static HRESULT CheckBreak2()
{
  return NConsoleClose::TestBreakSignal() ? E_ABORT : S_OK;
}

HRESULT COpenCallbackConsole::Open_CheckBreak()
{
  return CheckBreak2();
}

HRESULT COpenCallbackConsole::Open_SetTotal(const UInt64 *files, const UInt64 *bytes)
{
  if (!MultiArcMode && NeedPercents())
  {
    if (files)
    {
      _totalFilesDefined = true;
      // _totalFiles = *files;
//      _percent.Total = *files;	// 削除
    }
    else
      _totalFilesDefined = false;

    if (bytes)
    {
      // _totalBytesDefined = true;
 /* 削除ここから
      _totalBytes = *bytes;
      if (!files)
        _percent.Total = *bytes;
 削除ここまで */
    }
    else
    {
      // _totalBytesDefined = false;
 /* 削除ここから
      if (!files)
        _percent.Total = _totalBytes;
 削除ここまで */
    }
  }

  return CheckBreak2();
}

HRESULT COpenCallbackConsole::Open_SetCompleted(const UInt64 *files, const UInt64 *bytes)
{
 /* 削除ここから
  if (!MultiArcMode && NeedPercents())
  {
    if (files)
    {
      _percent.Files = *files;
      if (_totalFilesDefined)
        _percent.Completed = *files;
    }

    if (bytes)
    {
      if (!_totalFilesDefined)
        _percent.Completed = *bytes;
    }
    _percent.Print();
  }
 削除ここまで */

  return CheckBreak2();
}

HRESULT COpenCallbackConsole::Open_Finished()
{
  ClosePercents();
  return S_OK;
}


#ifndef Z7_NO_CRYPTO

HRESULT COpenCallbackConsole::Open_CryptoGetTextPassword(BSTR *password)
{
  *password = NULL;
  RINOK(CheckBreak2())

  if (!PasswordIsDefined)
  {
    ClosePercents();
    RINOK(GetPassword(Password, IDS_DECRYPT))		// 変更
    PasswordIsDefined = true;
  }
  return StringToBstr(Password, password);
}

/*
HRESULT COpenCallbackConsole::Open_GetPasswordIfAny(bool &passwordIsDefined, UString &password)
{
  passwordIsDefined = PasswordIsDefined;
  password = Password;
  return S_OK;
}

bool COpenCallbackConsole::Open_WasPasswordAsked()
{
  return PasswordWasAsked;
}

void COpenCallbackConsole::Open_Clear_PasswordWasAsked_Flag ()
{
  PasswordWasAsked = false;
}
*/

#endif
