// OpenCallbackConsole.cpp

#include "StdAfx.h"

#include "OpenCallbackConsole.h"

#include "ConsoleClose.h"
//#include "UserInputUtils.h"						// �폜

#include "resource.h"								// �ǉ�

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
//      _percent.Total = *files;	// �폜
    }
    else
      _totalFilesDefined = false;

    if (bytes)
    {
      // _totalBytesDefined = true;
 /* �폜��������
      _totalBytes = *bytes;
      if (!files)
        _percent.Total = *bytes;
 �폜�����܂� */
    }
    else
    {
      // _totalBytesDefined = false;
 /* �폜��������
      if (!files)
        _percent.Total = _totalBytes;
 �폜�����܂� */
    }
  }

  return CheckBreak2();
}

HRESULT COpenCallbackConsole::Open_SetCompleted(const UInt64 *files, const UInt64 *bytes)
{
 /* �폜��������
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
 �폜�����܂� */

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
    RINOK(GetPassword(Password, IDS_DECRYPT))		// �ύX
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
