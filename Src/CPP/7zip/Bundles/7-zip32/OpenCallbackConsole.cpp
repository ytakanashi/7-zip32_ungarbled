// OpenCallbackConsole.cpp

#include "StdAfx.h"

#include "OpenCallbackConsole.h"					// パス変更

#include "../../UI/Console/ConsoleClose.h"			// パス変更
//#include "UserInputUtils.h"						// 削除

#include "resource.h"								// 追加

HRESULT COpenCallbackConsole::Open_CheckBreak()
{
  if (NConsoleClose::TestBreakSignal())
    return E_ABORT;
  return S_OK;
}

HRESULT COpenCallbackConsole::Open_SetTotal(const UInt64 *, const UInt64 *)
{
  return Open_CheckBreak();
}

HRESULT COpenCallbackConsole::Open_SetCompleted(const UInt64 *, const UInt64 *)
{
  return Open_CheckBreak();
}
 
#ifndef _NO_CRYPTO

HRESULT COpenCallbackConsole::Open_CryptoGetTextPassword(BSTR *password)
{
  PasswordWasAsked = true;
  RINOK(Open_CheckBreak());
  if (!PasswordIsDefined)
  {
    RINOK(GetPassword(Password, IDS_DECRYPT));		// 変更
    PasswordIsDefined = true;
  }
  return StringToBstr(Password, password);
}

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

void COpenCallbackConsole::Open_ClearPasswordWasAskedFlag()
{
  PasswordWasAsked = false;
}

#endif
