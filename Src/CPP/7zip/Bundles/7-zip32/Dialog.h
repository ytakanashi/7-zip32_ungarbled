// Dialog.h: ダイアログ関係クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIALOG_H__B58B4FBD_56DB_4DEF_BF29_A19185CDB778__INCLUDED_)
#define AFX_DIALOG_H__B58B4FBD_56DB_4DEF_BF29_A19185CDB778__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/StringConvert.h"
#include "7-zip32.h"

extern HINSTANCE g_hInstance;
extern bool g_IsNT;

DWORD_PTR MySHGetFileInfo(LPCWSTR pszPath, DWORD dwFileAttributes, SHFILEINFOW *psfi, UINT uFlags);
BOOL CALLBACK GetProcessLastActivePopup(HWND hWnd, LPARAM lParam);

class CDialog  
{
public:
	int GetDlgItemTextW(int nID, UString& rString);
	BOOL SetDlgItemTextW(int nID, LPCWSTR lpszString);
	BOOL PathGetDlgItemPathW(int nID, UString& rString);
	void MoveCenter();
	INT_PTR DoModal();
	BOOL CreateModal();
	HWND m_hWnd;
	CDialog();
	virtual ~CDialog();

protected:
	BOOL m_bLoopExit;
	int m_nTemplateID;
	HWND m_hWndParent;
	void Loop();
	virtual BOOL DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	static BOOL DialogProcStatic(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static void ListView_SetItemTextW(HWND hwnd, int i, int iSubItem, LPCWSTR pszText);
};

class CProgressDialog : public CDialog
{
	typedef struct WORKFILEINFO {
		LPCWSTR lpSrcFileName;
		LPCWSTR lpDestFilePath;
		DWORD dwAttributes;
		FILETIME ftLastWriteTime;
		UINT64 nSrcFileSize;
		UINT64 nCompFileSize;
		DWORD dwCRC;
		LPCWSTR lpMethod;
		WORD wRatio;
		BOOL bSkip;		// 追加(19000002)
	} WORKFILEINFO, *LPWORKFILEINFO;

public:
	static BOOL SetOwnerWindow(HWND hWnd);
	static BOOL SetOwnerWindowEx(HWND hWnd, LPARCHIVERPROC lpArcProc);
	static BOOL SetOwnerWindowEx64(HWND hWnd, LPARCHIVERPROC lpArcProc, DWORD dwStructSize);
	static BOOL ClearOwnerWindow();
	static BOOL KillOwnerWindowEx(HWND hWnd);
	static BOOL KillOwnerWindowEx64(HWND hWnd);

public:
//	LPCWSTR GetDestFilePath() { return m_strDestFilePath; }
	
	CProgressDialog(HWND hParentWnd);
	~CProgressDialog();
	void SetArchiveFile(LPCWSTR lpFileName);
	void SetTotal(UINT64 nSize);
	void SetWorkFile(LPWORKFILEINFO lpWfi);
	void SetCompleted(UINT64 nSize);
	void AddWorkFile(LPCWSTR lpSrcFileName, LPCWSTR lpDestFilePath, DWORD dwAttributes, bool bEncrypted, FILETIME ftLastWriteTime, UINT64 nSrcFileSize, UINT64 nCompFileSize, DWORD dwCRC, LPCWSTR lpMethod);
	void AddSkipFile(LPCWSTR lpSrcFileName, LPCWSTR lpDestFilePath, DWORD dwAttributes, bool bEncrypted, FILETIME ftLastWriteTime, UINT64 nSrcFileSize, UINT64 nCompFileSize, DWORD dwCRC, LPCWSTR lpMethod);	// 追加(19000002)
	void SendExtractingInfo(int nMode);

	void SetProgressMode(wchar_t cMode) { m_cMode = cMode; }
	void SetShowDialog(BOOL bShow) { m_bShow = bShow; }
	HWND GetBecomeParentWindow() { return m_bShow ? m_hWnd : m_hWndParent; }
	UINT64 GetTotalSize() { return m_nTotalSize; }
	void SetArchiveInfo(bool bSolid, int nArchiveType) { m_bSolid = bSolid; m_nArchiveType = nArchiveType; }
//	void SeekNextFile() { if (m_wfis.Size()) { m_nLastFilePos += m_wfis[m_wfis.Size() - 1].nSrcFileSize; SetCompleted(m_nLastFilePos); }}
	void SeekNextFile(UINT64 nNextSize) { SetCompleted(m_nLastFilePos); m_nLastFilePos += nNextSize; }

protected:
	UINT64 m_nTotalSize;
	UINT64 m_nCompletedSize;
	UINT64 m_nNextItemPos;
	UINT64 m_nLastFilePos;
	int m_nItemIndex;
	DWORD m_dwStartTime;
	HWND m_hDetailWnd;
	wchar_t m_cMode;
	BOOL m_bShow;
	BOOL m_bShowDetail;
	int m_nWndHigh[2];
	int m_nArchiveType;
	bool m_bSolid;
	BOOL m_bSkip;			// 追加(19000002)
	char m_lpSkipText[64];	// 追加(19000002)
	CUIntVector m_detailItem;
	CObjectVector<WORKFILEINFO> m_wfis;
	BOOL OnInitDialog();
	virtual BOOL DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

	void ShowDetail(BOOL bShow);
	void SetDetail(LPWORKFILEINFO lpWfi);
	void SetProgressTime();
	int FindDetailIndex(UINT uID)
	{
		for (int i = 0; i < m_detailItem.Size(); ++i)
		{
			if (m_detailItem[i] == uID)
				return i;
		}
		return -1;
	}
};

class CPasswordDialog : public CDialog
{
public:
	UString GetPassword() { return m_strPassword; }
	CPasswordDialog(HWND hParent, UINT uTitleId);

protected:
	UString m_strPassword;
	UINT m_uTitleId;
	WCHAR m_cMask;
	HWND m_hPassWnd;
	virtual BOOL DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
};

class CConfirmationDialog : public CDialog
{
	/* 追加(18050002)ここから */
	typedef struct FILEINFO
	{
		bool bSizeIsDefined;
		bool bTimeIsDefined;
		UINT64 nSize;
		FILETIME ftWrite;
		UString strName;

		void SetTime(const FILETIME *ft)
		{
			if(ft == NULL)
				bTimeIsDefined = false;
			else
			{
				bTimeIsDefined = true;
				ftWrite = *ft;
			}
		}
		void SetSize(const UINT64 *n)
		{
			if(n == NULL)
				bSizeIsDefined = false;
			else
			{
				bSizeIsDefined = true;
				nSize = *n;
			}
		}
	} FILEINFO, *LPFILEINFO;
	/* 追加(18050002)ここまで */
public:
	void SetFileInfo(LPCWSTR lpNewName, const UINT64* nNewSize, const FILETIME *ftNewWrite, LPCWSTR lpOldName, const UINT64* nOldSize, const FILETIME *ftOldWrite);	// 変更(18050002)
	CConfirmationDialog();

protected:
	FILEINFO m_NewFileInfo;	// 変更(18050002)
	FILEINFO m_OldFileInfo;	// 変更(18050002)
	virtual BOOL DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
};

class CConfigDialog : public CDialog
{
public:
	CConfigDialog(HWND hParentWnd);

protected:
	virtual BOOL DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
};

class CSfxDialog : public CDialog
{
public:
	static LPVOID LoadSfxResource(UInt32& size);
	CSfxDialog(HWND hParentWnd);

	UString GetConfigText() { return m_strConfig; }

protected:
	UString m_strConfig;
	virtual BOOL DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
};

#endif // !defined(AFX_DIALOG_H__B58B4FBD_56DB_4DEF_BF29_A19185CDB778__INCLUDED_)
