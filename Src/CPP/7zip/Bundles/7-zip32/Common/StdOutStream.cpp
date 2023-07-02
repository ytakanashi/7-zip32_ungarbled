// Common/StdOutStream.cpp

#include "StdAfx.h"

#include "StdOutStream.h"
#include "Common/IntToString.h"
#include "Common/StringConvert.h"
#include "Common/UTFConvert.h"
#include "../../../UI/Common/ExitCode.h"

static const char kNewLineChar =  '\n';

CStdOutStream  g_StdOut;
CStdOutStream  g_StdErr;	// 変更

CStdOutStream ::CStdOutStream ()
{
	_streamIsOpen = false;
	IsTerminalMode = false;
	CodePage = -1;	// 追加
	_stream = stdout;
	m_strLog = L"";
	m_lpCommandLine = NULL;
	m_bUnicode = 0;
	m_lpPassword = NULL;
	m_nPriority = THREAD_PRIORITY_NORMAL;
	m_dwSystemError = 0;
	m_nLastError = 0;
	/* 追加ここから */
	m_bStdOut = FALSE;
	/* 追加ここまで */
	ReSet();
}

CStdOutStream ::~CStdOutStream ()
{
	if (m_lpCommandLine)
		free(m_lpCommandLine);
	if (m_lpPassword)
		free(m_lpPassword);
}

CStdOutStream & CStdOutStream::operator<<(CStdOutStream & (*aFunction)(CStdOutStream  &))
{
  (*aFunction)(*this);    
  return *this;
}

CStdOutStream & endl(CStdOutStream & outStream)
{
  return outStream << kNewLineChar;
}

CStdOutStream & CStdOutStream::operator<<(const char *string)
{
	m_strLog += GetUnicodeString(string);
	return *this;
}

CStdOutStream & CStdOutStream::operator<<(const wchar_t *string)
{
	m_strLog += string;
	return *this;
}

CStdOutStream & CStdOutStream::operator<<(char c)
{
	m_strLog += (wchar_t)c;	// 変更
	return *this;
}

//void StdOut_Convert_UString_to_AString(const UString &s, AString &temp)	// 削除
void CStdOutStream::Convert_UString_to_AString(const UString &s, AString &temp)	// 追加
{
  int codePage = CodePage;
  if (codePage == -1)
    codePage = CP_OEMCP;
  if (codePage == CP_UTF8)
    ConvertUnicodeToUTF8(s, temp);
  else
    UnicodeStringToMultiByte2(temp, s, (UINT)codePage);
}

void CStdOutStream::PrintUString(const UString &s, AString &temp)
{
//  StdOut_Convert_UString_to_AString(s, temp);	// 削除
  Convert_UString_to_AString(s, temp);	// 追加
  *this << (const char *)temp;
}


static const wchar_t kReplaceChar = '_';

void CStdOutStream::Normalize_UString_LF_Allowed(UString &s)
{
  unsigned len = s.Len();
  wchar_t *d = s.GetBuf();

  if (IsTerminalMode)
    for (unsigned i = 0; i < len; i++)
    {
      wchar_t c = d[i];
      if (c <= 13 && c >= 7 && c != '\n')
        d[i] = kReplaceChar;
    }
}

void CStdOutStream::Normalize_UString(UString &s)
{
  unsigned len = s.Len();
  wchar_t *d = s.GetBuf();

  if (IsTerminalMode)
    for (unsigned i = 0; i < len; i++)
    {
      wchar_t c = d[i];
      if (c <= 13 && c >= 7)
        d[i] = kReplaceChar;
    }
  else
    for (unsigned i = 0; i < len; i++)
    {
      wchar_t c = d[i];
      if (c == '\n')
        d[i] = kReplaceChar;
    }
}

void CStdOutStream::NormalizePrint_UString(const UString &s, UString &tempU, AString &tempA)
{
  tempU = s;
  Normalize_UString(tempU);
  PrintUString(tempU, tempA);
}

void CStdOutStream::NormalizePrint_UString(const UString &s)
{
  NormalizePrint_wstr(s);
}

void CStdOutStream::NormalizePrint_wstr(const wchar_t *s)
{
  UString tempU = s;
  Normalize_UString(tempU);
  AString tempA;
  PrintUString(tempU, tempA);
}


CStdOutStream & CStdOutStream::operator<<(Int32 number) throw()
{
  char s[32];
  ConvertInt64ToString(number, s);
  return operator<<(s);
}

CStdOutStream & CStdOutStream::operator<<(Int64 number) throw()
{
  char s[32];
  ConvertInt64ToString(number, s);
  return operator<<(s);
}

CStdOutStream & CStdOutStream::operator<<(UInt32 number) throw()
{
  char s[16];
  ConvertUInt32ToString(number, s);
  return operator<<(s);
}

CStdOutStream & CStdOutStream::operator<<(UInt64 number) throw()
{
  char s[32];
  ConvertUInt64ToString(number, s);
  return operator<<(s);
}


// ファイル属性を文字列に変換
void CStdOutStream::GetAttributesString(DWORD dwAttributes, bool bEncrypt, LPSTR lpAttributes)
{
	lpAttributes[0] = (dwAttributes & FILE_ATTRIBUTE_ARCHIVE)  ? 'A' : '-';
	lpAttributes[1] = (dwAttributes & FILE_ATTRIBUTE_SYSTEM)   ? 'S' : '-';
	lpAttributes[2] = (dwAttributes & FILE_ATTRIBUTE_HIDDEN)   ? 'H' : '-';
	lpAttributes[3] = (dwAttributes & FILE_ATTRIBUTE_READONLY) ? 'R' : 'W';
	if (bEncrypt)
	{
		lpAttributes[4] = (dwAttributes & FA_ENCRYPTED) ? 'G' : '-';
		lpAttributes[5] = '\0';
		return;
	}
	lpAttributes[4] = '\0';
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 7-zip32.dll 拡張

// 圧縮方式を8バイトのchar配列に入るよう短く変換(lpMethodはASCII文字のみ前提)
void CStdOutStream::GetCompactMethod(LPCWSTR lpMethod, int nArchiveType, LPSTR lpCompactMethod)
{
	switch (nArchiveType)
	{
	case ARCHIVETYPE_7Z:
		{
			int i;
			for (i = 0; i < 7; ++i)
			{
				lpCompactMethod[i] = (char)lpMethod[i];
				if (lpCompactMethod[i] == ' ' || lpCompactMethod[i] == ':' || lpCompactMethod[i] == '\0')
					break;
			}
			lpCompactMethod[i] = '\0';
			break;
		}
	case ARCHIVETYPE_ZIP:
		{
			for (LPCWSTR p = lpMethod; *p != '\0'; ++p)
			{
				if (*p == ' ')
					lpMethod = ++p;
			}
			int i;
			for (i = 0; i < 8; ++i)
			{
				lpCompactMethod[i] = (char)lpMethod[i];
				if (lpCompactMethod[i] == '\0')
					break;
			}
			if (i == 8)
			{
				switch (lpMethod[7])
				{
				case '1':	// Reduced1 -> Reduce1
				case '2':	// Reduced2 -> Reduce2
				case '3':	// Reduced3 -> Reduce3
					lpCompactMethod[6] = (char)lpMethod[7];
					lpCompactMethod[7] = '\0';
					break;
				case '6':	// Deflate64 -> Defla64
					lpCompactMethod[5] = '6';
					lpCompactMethod[6] = '4';
					lpCompactMethod[7] = '\0';
					break;
				case 'i':	// Tokenizing -> Tokeniz
				case 'd':	// PKImploding -> PKImplo
				default:	// 不明 -> 前7文字まで
					lpCompactMethod[7] = '\0';
					break;
				}
			}
			break;
		}
	default:
		lpCompactMethod[0] = '\0';
		break;
	}
}

// メンバ変数を初期化
void CStdOutStream::ReSet()
{
	m_pDlgProgress = NULL;
	m_hThread = NULL;
	m_strLog.Empty();
	if (m_lpCommandLine)
	{
		free(m_lpCommandLine);
		m_lpCommandLine = NULL;
	}
}

// 出力をコピーする
void CStdOutStream::CopyBuf(LPSTR lpCopyBuf, DWORD dwBufSize)
{
	if (lpCopyBuf == NULL)
		return;
	UString strWideChar = m_strLog;
	strWideChar.Replace(L"\n", L"\r\n");
	WideCharToMultiByte(strWideChar, lpCopyBuf, dwBufSize);
}

// 送られたコマンドをコマンドラインにして記憶
void CStdOutStream::SetCommandLine(const CSplitCmdLine& scl)
{
	UString strCmdLine = L"7-zip32.dll ";
	strCmdLine += ::MultiByteToUnicodeString(scl.m_argv[0], scl.m_codePage);
	if (scl.m_bSfx)
	{
		strCmdLine += L" \"-sfx";
		strCmdLine += fs2us(m_strSfxPath);
		strCmdLine += L"\"";
	}
	if (!scl.m_bScs && !m_bUnicode && scl.m_argc > 0)
		strCmdLine += L" -scsWIN";
	if (!scl.m_bScc && scl.m_argc > 0)
	{
		if (m_bUnicode)
			strCmdLine += L" -sccUTF-8";
		else
			strCmdLine += L" -sccWIN";
	}
	/* 追加ここから */
	if (m_bStdOut)
		strCmdLine += L" -so";
	/* 追加ここまで */
	for (int i = 1; i < scl.m_argc; ++i)
	{
		strCmdLine += L" \"";
		strCmdLine += ::MultiByteToUnicodeString(scl.m_argv[i], scl.m_codePage);
		strCmdLine += L"\"";
	}
	if (m_lpCommandLine)
		free(m_lpCommandLine);
	m_lpCommandLine = _wcsdup(strCmdLine);
}

// WCHAR文字列をモードに併せてchar文字列に変換してコピー
// 成功時は0、バッファ不足の場合は必要なサイズを返す
int CStdOutStream::WideCharToMultiByte(const UString &strWideChar, LPSTR lpMultiByteStr, int cchMultiByte)
{
	int codePage = CodePage;	// 変更
	if (codePage == -1)
		codePage = m_bUnicode ? CP_UTF8 : CP_OEMCP;
	AString dest;
	if (codePage == CP_UTF8)
		::ConvertUnicodeToUTF8(strWideChar, dest);
	else
		dest = ::UnicodeStringToMultiByte(strWideChar, (UINT)codePage);
	::lstrcpynA(lpMultiByteStr, dest, cchMultiByte);
	if (dest.Len() >= cchMultiByte)
		return dest.Len() + 1;
	return 0;
}

// char文字列をUnicodeモードに合わせてUnicodeに変換
UString CStdOutStream::ConvertUnicodeString(LPCSTR lpString)
{
	UString strUnicode;
	if (m_bUnicode)
		::ConvertUTF8ToUnicode(AString(lpString), strUnicode);// 変更(17000001)
	else
		strUnicode = ::GetUnicodeString(lpString);
	return strUnicode;
}

// Unicodeモードの保存
BOOL CStdOutStream::SetUnicodeMode(BOOL bUnicode)
{ 
	SetLastError(m_hThread ? ERROR_ALREADY_RUNNING : 0);
	if (m_hThread)
		return FALSE;
	m_bUnicode = bUnicode;
	return TRUE;
}

// グローバルなデフォルトパスワード保存
void CStdOutStream::SetDefaultPassword(LPCSTR lpPassword)
{
	if (m_lpPassword)
		free(m_lpPassword);
	m_lpPassword = NULL;
	if (lpPassword)
		m_lpPassword = _wcsdup(ConvertUnicodeString(lpPassword));
}

// スレッド優先度の設定
int CStdOutStream::SetPriority(const int nPriority)
{
	if (m_hThread)
		return ERROR_ALREADY_RUNNING;
	if (::SetThreadPriority(m_hThread, nPriority) == FALSE)
		return ERROR_INVALID_VALUE;
	m_nPriority = nPriority;
	return S_OK;
}

// エラーコードの取得
int CStdOutStream::GetLastError(LPDWORD lpdwSystemError)
{
	if (lpdwSystemError)
		*lpdwSystemError = m_dwSystemError;
	return m_nLastError;
}

// 内部エラーコードを統合アーカイバエラーコードに変換
static int ExitCodeToErrorCode(int nExitCode)
{
	switch (nExitCode)
	{
	case NExitCode::kSuccess:
		return 0;
	case NExitCode::kWarning:
		return ERROR_WARNING;
	case NExitCode::kFatalError:
		return ERROR_FATAL;
//	case NExitCode::kCRCError:
//		return ERROR_FILE_CRC;
//	case NExitCode::kLockedArchive:
//		return ERROR_SHARING;
//	case NExitCode::kWriteError:
//		return ERROR_CANNOT_WRITE;
//	case NExitCode::kOpenError:
//		return ERROR_FILE_OPEN;
	case NExitCode::kUserError:
		return ERROR_COMMAND_NAME;
	case NExitCode::kMemoryError:
		return ERROR_MORE_HEAP_MEMORY;
//	case NExitCode::kCreateFileError:
//		return ERROR_FILE_CREATE;
	case NExitCode::kUserBreak:
		return ERROR_USER_CANCEL;
	default:
		break;
	}
	return nExitCode >= 0x8000 ? nExitCode : ERROR_FATAL;
}

// エラーコードの設定
int CStdOutStream::SetLastError(int nErrorCode)
{
	m_nLastError = ExitCodeToErrorCode(nErrorCode);
	m_dwSystemError = ::GetLastError();
	return m_nLastError;
}


///////////////////////////////////////////////////////////////////////////////////////////
// グローバル関数

// パスワードダイアログ表示
HRESULT GetPassword(UString &password, UINT title)
{
	HWND hWnd = NULL;
	CProgressDialog* pProgDlg = g_StdOut.GetProgressDialog();
	if (pProgDlg == NULL)
		::EnumWindows(GetProcessLastActivePopup, (LPARAM)&hWnd);
	else
		hWnd = pProgDlg->GetBecomeParentWindow();
	CPasswordDialog dlg(hWnd, title);
	if (dlg.DoModal() == IDCANCEL)
	{
		password = L"";
		return E_ABORT;
	}
	password = dlg.GetPassword();
	return S_OK;
}
