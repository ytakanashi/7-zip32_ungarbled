// Common/StdOutStream.h

#pragma once 

#ifndef __COMMON_STDOUTSTREAM_H
#define __COMMON_STDOUTSTREAM_H

#include "../7-zip32.h"
#include "../Dialog.h"
#include "../SplitCmdLine.h"
#include "../MainAr.h"


#include <stdio.h>
#include <process.h>

class CStdOutStream
{
	bool _streamIsOpen;
	FILE *_stream;
public:
	bool IsTerminalMode;
	int CodePage;

	CStdOutStream ();
	CStdOutStream(FILE *stream):// �ύX
		_stream(stream),
		// _streamIsOpen(false),
		IsTerminalMode(false),
		CodePage(-1)
		{}
	~CStdOutStream ();
	operator FILE *() { return _stream; }
	bool Flush() { return true; }
	
	CStdOutStream & operator<<(CStdOutStream & (* aFunction)(CStdOutStream  &));
	CStdOutStream & operator<<(const char *string);
	CStdOutStream & operator<<(const wchar_t *string);
	CStdOutStream & operator<<(char c);
	CStdOutStream & operator<<(Int32 number) throw();
	CStdOutStream & operator<<(Int64 number) throw();
	CStdOutStream & operator<<(UInt32 number) throw();
	CStdOutStream & operator<<(UInt64 number) throw();

	void CStdOutStream::Convert_UString_to_AString(const UString &s, AString &temp);		// �ǉ�

	void PrintUString(const UString &s, AString &temp);

	void Normalize_UString_LF_Allowed(UString &s);
	void Normalize_UString(UString &s);

	void NormalizePrint_UString(const UString &s, UString &tempU, AString &tempA);
	void NormalizePrint_UString(const UString &s);
	void NormalizePrint_wstr(const wchar_t *s);

	static void GetCompactMethod(LPCWSTR lpMethod, int nArchiveType, LPSTR lpCompactMethod);
	static void GetAttributesString(DWORD dwAttributes, bool bEncrypt, LPSTR lpAttributes);

	void ReSet();
	void CopyBuf(LPSTR lpCopyBuf, DWORD dwBufSize);
	void SetCommandLine(const CSplitCmdLine& scl);
	int WideCharToMultiByte(const UString &strWideChar, LPSTR lpMultiByteStr, int cchMultiByte);
	UString ConvertUnicodeString(LPCSTR lpString);
	BOOL SetUnicodeMode(BOOL bUnicode);
	void SetDefaultPassword(LPCSTR lpPassword);
	int SetPriority(const int nPriority);
	int GetLastError(LPDWORD lpdwSystemError);
	int SetLastError(int nErrorCode);
	void SetStdOutMode(BOOL bStdOut){ m_bStdOut = bStdOut; }	// �ǉ�(19000002)

	CProgressDialog* GetProgressDialog() { return m_pDlgProgress; }
//	void CloseProgressDialog() { ::PostMessage(m_pDlgProgress->m_hWnd, WM_CLOSE, 0, 1); }	// �폜(19000002)
	/* �ǉ�(19000002)�������� */
	void CloseProgressDialog() {
		if (m_bStdOut)
			::FlushFileBuffers(GetStdHandle(STD_OUTPUT_HANDLE));
		::PostMessage(m_pDlgProgress->m_hWnd, WM_CLOSE, 0, 1);
	}
	/* �ǉ�(19000002)�����܂� */
	void SetProgressDialog(CProgressDialog* pDlg) { m_pDlgProgress = pDlg; }
	BOOL GetUnicodeMode() { return m_bUnicode; }
	LPCWSTR GetCommandLineW() { return m_lpCommandLine; }
	LPCWSTR GetDefaultPassword() { return m_lpPassword; }
	HANDLE GetThread() { return m_hThread; }
	void CreateMainThread() { UINT thrdaddr; m_hThread = (HANDLE)_beginthreadex(NULL, 0, Main, NULL, CREATE_SUSPENDED, &thrdaddr); }
	int GetPriority() { return m_nPriority; }
	void SetSfxPath(FString strSfxPath) { m_strSfxPath = strSfxPath; }
	BOOL GetStdOutMode(){ return m_bStdOut; }	// �ǉ�(19000002)

protected:
	LPWSTR m_lpCommandLine;
	FString m_strSfxPath;
	CProgressDialog* m_pDlgProgress;
	BOOL m_bUnicode;
	int m_nPriority;
	UString m_strLog;
	LPWSTR m_lpPassword;
	HANDLE m_hThread;
	DWORD m_dwSystemError;
	int m_nLastError;
	/* �ǉ�(19000002)�������� */
	BOOL m_bStdOut;
	/* �ǉ�(19000002)�����܂� */
};

CStdOutStream & endl(CStdOutStream & anOut);

extern CStdOutStream g_StdOut;
extern CStdOutStream g_StdErr;	// �ύX

//void StdOut_Convert_UString_to_AString(const UString &s, AString &temp);	// �폜

HRESULT GetPassword(UString &password, UINT title);

#endif
