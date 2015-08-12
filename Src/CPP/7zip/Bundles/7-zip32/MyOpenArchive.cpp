// MyOpenArchive.cpp: COpenArchive クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MyOpenArchive.h"
#include "MainAr.h"
#include "SplitCmdLine.h"
#include "Common/StdOutStream.h"
#include <time.h>

#include "Windows/PropVariant.h"
#include "Windows/PropVariantConv.h"
#include "Windows/FileDir.h"
#include "../../UI/Common/OpenArchive.h"
#include "../../UI/Common/ArchiveCommandLine.h"
#include "OpenCallbackConsole.h"		// パス変更
#include "ExtractCallbackConsole.h"		// パス変更
#include "Common/UTFConvert.h"
#include "../../UI/Common/SetProperties.h"	// 追加
#include "Common/IntToString.h"				// 追加

static COpenArchive* pOATail;

extern CStdOutStream *g_StdStream;	// 追加
extern CStdOutStream *g_ErrStream;	// 追加

extern UINT32 g_ArcCodePage;		// 追加

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

COpenArchive::COpenArchive()
{
	m_pOANext = NULL;
	m_pOAPrev = pOATail;
	if (pOATail)
		pOATail->m_pOANext = this;
	pOATail = this;
	m_lpPassword = NULL;
	m_pWildcardCensor = NULL;
	m_aNumItems = -1;
}

COpenArchive::~COpenArchive()
{
	if (m_pOAPrev)
		m_pOAPrev->m_pOANext = m_pOANext;
	if (m_pOANext)
		m_pOANext->m_pOAPrev = m_pOAPrev;
	if (pOATail == this)
		pOATail = pOATail->m_pOAPrev;

	if (m_pWildcardCensor)
		delete m_pWildcardCensor;
	if (m_lpPassword)
		free(m_lpPassword);
}

// 消し忘れのオブジェクトを削除
void COpenArchive::RemoveAll()
{
	while (pOATail)
		delete pOATail;
}

// オブジェクトのポインタを検索
COpenArchive* COpenArchive::FindObject(HARC hArc)
{
	for (COpenArchive* pOA = pOATail; pOA; pOA = pOA->m_pOAPrev)
	{
		if (pOA == (COpenArchive*)hArc)
			return pOA;
	}
	return NULL;
}

// 書庫を開く
// 戻り値 S_OK:成功 ERROR_PASSWORD_FILE:開けたけどパスワード異常 その他:失敗
HRESULT COpenArchive::OpenCheck(LPCWSTR lpFileName, DWORD dwMode)
{
	HRESULT result;
	try
	{
		CCodecs *codecs = new CCodecs;
		CMyComPtr<
			#ifdef EXTERNAL_CODECS
			ICompressCodecsInfo
			#else
			IUnknown
			#endif
			> compressCodecsInfo = codecs;
		RINOK(codecs->Load());

		CObjectVector<COpenType> types;
		CIntVector excludedFormats;		// 変更
		CStdOutStream stdOut;
		COpenCallbackConsole openCallback;
//		openCallback.OutStream = &stdOut;	// 削除
		openCallback.Init(g_StdStream, g_ErrStream, NULL);		// 追加
		if (m_lpPassword || g_StdOut.GetDefaultPassword())
		{
			openCallback.PasswordIsDefined = true;
			openCallback.Password = m_lpPassword ? m_lpPassword : g_StdOut.GetDefaultPassword();
		}
		switch (LOWORD(dwMode))
		{
		case CHECKARCHIVE_FULLCRC:
			{
				CExtractCallbackConsole *ecs = new CExtractCallbackConsole;
				CMyComPtr<IFolderArchiveExtractCallback> extractCallback = ecs;
				
//				ecs->OutStream = &stdOut;	// 削除
				ecs->PasswordIsDefined = openCallback.PasswordIsDefined;
				ecs->Password = openCallback.Password;
				ecs->Init(g_StdStream, g_ErrStream, NULL);	// 変更
				
				CArcCmdLineOptions options;
				CArcCmdLineParser parser;			// 変更
				UStringVector commandStrings;
				commandStrings.Add(L"t");
				commandStrings.Add(lpFileName);
				parser.Parse1(commandStrings, options);
				parser.Parse2(options);

				/* 追加ここから */
				if (!ParseOpenTypes(*codecs, options.ArcType, types))
					return ERROR_NOT_ARC_FILE;

				FOR_VECTOR (k, options.ExcludedArcTypes)
				{
					CIntVector tempIndices;
					if (!codecs->FindFormatForArchiveType(options.ExcludedArcTypes[k], tempIndices)
						|| tempIndices.Size() != 1)
						return ERROR_NOT_ARC_FILE;
					excludedFormats.AddToUniqueSorted(tempIndices[0]);
					// excludedFormats.Sort();
				}
				/* 追加ここまで */
				/* 削除ここから
				if (!codecs->FindFormatForArchiveType(options.ArcType, formatIndices))
					return ERROR_NOT_ARC_FILE;
				削除ここまで */
				CExtractOptions eo;
				eo.StdInMode = options.StdInMode;
				eo.StdOutMode = options.StdOutMode;
				eo.PathMode = options.Command.GetPathMode();
				eo.TestMode = options.Command.IsTestCommand();
				eo.OverwriteMode = options.ExtractOptions.OverwriteMode;
				eo.OutputDir = options.ExtractOptions.OutputDir;
				eo.YesToAll = options.YesToAll;
//				eo.CalcCrc = options.CalcCrc;	// 削除
				eo.Properties = options.Properties;

				UString errorMessage;
				CDecompressStat stat;
				/* 追加ここから */
				CHashBundle hb;
				IHashCalc *hashCalc = NULL;

				if (!options.HashMethods.IsEmpty())
				{
					hashCalc = &hb;
					RINOK(hb.SetMethods(EXTERNAL_CODECS_VARS options.HashMethods));
					hb.Init();
				}
				UStringVector ArchivePathsSorted;
				UStringVector ArchivePathsFullSorted;
				/* 追加ここまで */
				result = Extract(		// 変更
					codecs,
					types,				// 追加
					excludedFormats,	// 変更
					ArchivePathsSorted, // 変更
					ArchivePathsFullSorted,	// 変更
					options.Censor.Pairs.Front().Head,			// 変更
					eo, &openCallback, ecs, hashCalc, errorMessage, stat);	// 変更
				if ((ecs->NumArcsWithError != 0 || ecs->NumFileErrors != 0) && result == S_OK)	// 変更
					result = ERROR_WARNING;
				return result;
			}
		case CHECKARCHIVE_RAPID:
			openCallback.PasswordIsDefined = true;
		case CHECKARCHIVE_BASIC:
			{
				COpenOptions options;
				options.props = NULL;
				options.codecs = codecs;
				options.types = &types;
				options.excludedFormats = &excludedFormats;
				options.stdInMode = false;
				options.filePath = lpFileName;
				result = m_archiveLink.Open2(options, &openCallback);
				if (result != S_OK)
				{
					if (dwMode == CHECKARCHIVE_RAPID && m_archiveLink.PasswordWasAsked)	// 変更
					{
						m_nArchiveType = ARCHIVETYPE_7Z;
						return ERROR_PASSWORD_FILE;
					}
					m_nArchiveType = 0;
					return result;
				}
				if (m_lpPassword == NULL)
					m_lpPassword = _wcsdup(openCallback.Password);
				break;
			}
		default:
			return E_FAIL;
		}

        const CArc &arc = m_archiveLink.Arcs.Back();
		if (codecs->Formats[arc.FormatIndex].Name.IsEqualTo_NoCase(L"7z"))	// 変更	// arc.FormatIndexがnArchiveTypeに対応するかも。しかし将来の互換性で問題出るか？
			m_nArchiveType = ARCHIVETYPE_7Z;
		else if (codecs->Formats[arc.FormatIndex].Name.IsEqualTo_NoCase(L"Zip"))	// 変更
			m_nArchiveType = ARCHIVETYPE_ZIP;
		else
			m_nArchiveType = 0;
	}
	catch (...)
	{
		result = E_FAIL;
	}
	return result;
}

BOOL COpenArchive::Open(LPCWSTR lpFileName, DWORD dwMode)
{
	m_aItemPos = 0;
	m_ui64ArcOriginalSize = 0;
	m_ui64ArcCompressedSize = 0;
	m_dwMode = dwMode;
	m_bSearchMode = false;
	m_bSolid = false;
	m_bEncrypt = false;
	if (m_fiArchive.Find(lpFileName) == false)
	{
		g_StdOut.SetLastError(ERROR_NOT_FIND_ARC_FILE);
		return FALSE;
	}
	
	HRESULT res = OpenCheck(lpFileName, dwMode & M_ERROR_MESSAGE_ON ? CHECKARCHIVE_BASIC : CHECKARCHIVE_RAPID);
	if (res == ERROR_PASSWORD_FILE)
	{
		m_bEncrypt = true;
		g_StdOut.SetLastError(ERROR_PASSWORD_FILE);
		return TRUE;
	}
	else if (res != S_OK)
	{
		g_StdOut.SetLastError(res);
		return FALSE;
	}
	
	const CArc &arc = m_archiveLink.Arcs.Back();
	res = arc.Archive->GetNumberOfItems(&m_aNumItems);
	if (res != S_OK)
	{
		g_StdOut.SetLastError(res);
		return FALSE;
	}

	if (m_nArchiveType == ARCHIVETYPE_7Z)
	{
		NWindows::NCOM::CPropVariant aPropVariant;
		arc.Archive->GetArchiveProperty(kpidSolid, &aPropVariant);
		m_bSolid = VARIANT_BOOLToBool(aPropVariant.boolVal);
	}
	g_StdOut.SetLastError(0);
	/* 追加ここから */
	//書庫のコードページを指定
	if(g_ArcCodePage){
		CObjectVector<CProperty> properties;
		CProperty prop;
		wchar_t value[16];
		prop.Name = L"cp";
		ConvertUInt32ToString(g_ArcCodePage, value);
		prop.Value = value;
		properties.Add(prop);
		SetProperties(arc.Archive, properties);
	}
	/* 追加ここまで */
	return TRUE;
}

int COpenArchive::FindFirst(LPCWSTR lpszWildName, INDIVIDUALINFO *lpSubInfo)
{
	if (m_bEncrypt && !Open(m_fiArchive.Name, M_ERROR_MESSAGE_ON))
	{
		m_bEncrypt = true;
		return g_StdOut.SetLastError(ERROR_PASSWORD_FILE);
	}

	if (m_pWildcardCensor)
		delete m_pWildcardCensor;
	m_pWildcardCensor = new NWildcard::CCensor;
	UStringVector s;
	NCommandLineParser::SplitCommandLine(lpszWildName, s);
	if (s.Size() == 0)
		s.Add(L"*");
	for (int i = 0; i < s.Size(); ++i)
	{
		bool recursed = false;
		if (m_dwMode & M_CHECK_ALL_PATH)
		{
			if (m_dwMode & M_CHECK_FILENAME_ONLY)
				recursed = DoesNameContainWildcard(s[i]);	// 変更
			else
				recursed = true;
		}
		m_pWildcardCensor->AddItem(NWildcard::k_RelatPath, true, s[i], recursed, true);	// 変更
	}

	m_aItemPos = 0;
	m_ui64ArcOriginalSize = 0;
	m_ui64ArcCompressedSize = 0;
	m_bSearchMode = true;
	return FindNext(lpSubInfo);
}

bool CensorNode_CheckPath2(const NWildcard::CCensorNode &node, const CReadArcItem &item, bool &include);	// 追加

int COpenArchive::FindNext(INDIVIDUALINFO *lpSubInfo)
{
	if (!m_bSearchMode)
		return g_StdOut.SetLastError(ERROR_NOT_SEARCH_MODE);

	g_StdOut.SetLastError(0);
	NWindows::NCOM::CPropVariant aPropVariant;
	const CArc &arc = m_archiveLink.Arcs.Back();
	while (TRUE)
	{
		if (m_aItemPos == m_aNumItems)
		{
			m_bSearchMode = false;
			return -1;
		}
		if (arc.GetItemPath(m_aItemPos, m_aFileName) == S_OK)
		{
			bool isFolder = false;
			Archive_IsItem_Dir(arc.Archive, m_aItemPos, isFolder);	// 変更
			/* 追加ここから */
			bool isAltStream;
			Archive_IsItem_AltStream(arc.Archive, m_aItemPos, isAltStream);
			CReadArcItem item;
			arc.GetItem(m_aItemPos, item);
			bool pathOk = false;
			FOR_VECTOR (i, m_pWildcardCensor->Pairs)
			{
				bool include;
				if (CensorNode_CheckPath2(m_pWildcardCensor->Pairs[i].Head, item, include))
				{
				if (!include)
				{
					pathOk=false;
					break;
				}
				pathOk = true;
				}
			}
			/* 追加ここまで */
//			if (Censor_CheckPath(*m_pWildcardCensor, item))	// 変更,削除
			{
				if (isFolder)
				{
					if (m_aFileName.Back() == '\\')		// 変更
						m_aFileName.DeleteBack();		// 変更
					m_aFileName += L"\\";
				}
				break;
			}
		}
		++m_aItemPos;
	}
	arc.Archive->GetProperty(m_aItemPos, kpidSize, &aPropVariant);
	ConvertPropVariantToUInt64(aPropVariant, m_ui64OriginalSize);	// 変更
	arc.Archive->GetProperty(m_aItemPos, kpidPackSize, &aPropVariant);
//	m_ui64CompressedSize = m_bSolid ? 0 : ConvertPropVariantToUInt64(aPropVariant);	// 削除
	/* 追加ここから */
	if (m_bSolid)
		m_ui64CompressedSize = 0;
	else
		ConvertPropVariantToUInt64(aPropVariant, m_ui64CompressedSize);
	/* 追加ここまで */
	arc.Archive->GetProperty(m_aItemPos, kpidCRC, &aPropVariant);
	m_dwCRC = aPropVariant.vt ? aPropVariant.ulVal : 0;
	FILETIME ft = {0, 0};
	arc.Archive->GetProperty(m_aItemPos, kpidMTime, &aPropVariant);
	m_ftLastWriteTime = aPropVariant.vt ? aPropVariant.filetime : ft;
	arc.Archive->GetProperty(m_aItemPos, kpidCTime, &aPropVariant);
	m_ftLastCreateTime = aPropVariant.vt ? aPropVariant.filetime : ft;
	arc.Archive->GetProperty(m_aItemPos, kpidATime, &aPropVariant);
	m_ftLastAccessTime = aPropVariant.vt ? aPropVariant.filetime : ft;
	arc.Archive->GetProperty(m_aItemPos, kpidAttrib, &aPropVariant);
	m_dwAttribute = aPropVariant.ulVal;
	arc.Archive->GetProperty(m_aItemPos, kpidMethod, &aPropVariant);
	m_aMode = aPropVariant.vt ? aPropVariant.bstrVal : L"";
	arc.Archive->GetProperty(m_aItemPos, kpidEncrypted, &aPropVariant);
	if (aPropVariant.boolVal)
		m_dwAttribute |= FA_ENCRYPTED;

/*	if (m_nArchiveType == ARCHIVETYPE_ZIP)
	{
//		m_anArchiveHandler->GetProperty(m_aItemPos, kpidHostOS, &aPropVariant);
//		m_anArchiveHandler->GetProperty(m_aItemPos, kpidComment, &aPropVariant);
	}*/
	
	if (lpSubInfo)
	{
		lpSubInfo->uFlag = 0;
		lpSubInfo->uOSType = 0;
		lpSubInfo->dwCRC = m_dwCRC;
		lpSubInfo->dwOriginalSize = m_ui64OriginalSize < 0xFFFFFFFF ? (DWORD)m_ui64OriginalSize : -1;
		lpSubInfo->dwCompressedSize = m_ui64CompressedSize < 0xFFFFFFFF ? (DWORD)m_ui64CompressedSize : -1;
		lpSubInfo->wRatio = (m_ui64OriginalSize && !m_bSolid) ? (WORD)(m_ui64CompressedSize * 1000 / m_ui64OriginalSize) : 0;
		FILETIME ftLocal;
		::FileTimeToLocalFileTime(&m_ftLastWriteTime, &ftLocal);
		::FileTimeToDosDateTime(&ftLocal, &lpSubInfo->wDate, &lpSubInfo->wTime);
		g_StdOut.WideCharToMultiByte(m_aFileName, lpSubInfo->szFileName, FNAME_MAX32);
		g_StdOut.WideCharToMultiByte(m_aMode, lpSubInfo->szMode, 8);
		CStdOutStream::GetCompactMethod(m_aMode, m_nArchiveType, lpSubInfo->szMode);
		CStdOutStream::GetAttributesString(m_dwAttribute, true, lpSubInfo->szAttribute);
	}

	m_ui64ArcOriginalSize += m_ui64OriginalSize;
	m_ui64ArcCompressedSize += m_ui64CompressedSize;
	++m_aItemPos;

	return 0;
}

WORD COpenArchive::GetArcDate()
{
	WORD wFatDate;
	WORD wFatTime;
	FILETIME ftLocalFileTime;
	::FileTimeToLocalFileTime(&m_fiArchive.MTime, &ftLocalFileTime);
	::FileTimeToDosDateTime(&ftLocalFileTime, &wFatDate, &wFatTime);
	return wFatDate;
}

WORD COpenArchive::GetArcTime()
{
	WORD wFatDate;
	WORD wFatTime;
	FILETIME ftLocalFileTime;
	::FileTimeToLocalFileTime(&m_fiArchive.MTime, &ftLocalFileTime);
	::FileTimeToDosDateTime(&ftLocalFileTime, &wFatDate, &wFatTime);
	return wFatTime;
}

WORD COpenArchive::GetRatio()
{
	if (!m_bSearchMode)
		return -1;
	if (m_ui64OriginalSize == 0)
		return 0;
	return (WORD)(m_ui64CompressedSize * 1000 / m_ui64OriginalSize);
}

WORD COpenArchive::GetDate()
{
	if (!m_bSearchMode || *(UINT64*)&m_ftLastWriteTime == 0)
		return -1;
	WORD wFatDate;
	WORD wFatTime;
	FILETIME ftLocalFileTime;
	::FileTimeToLocalFileTime(&m_ftLastWriteTime, &ftLocalFileTime);
	::FileTimeToDosDateTime(&ftLocalFileTime, &wFatDate, &wFatTime);
	return wFatDate;
}

WORD COpenArchive::GetTime()
{
	if (!m_bSearchMode || *(UINT64*)&m_ftLastWriteTime == 0)
		return -1;
	WORD wFatDate;
	WORD wFatTime;
	FILETIME ftLocalFileTime;
	::FileTimeToLocalFileTime(&m_ftLastWriteTime, &ftLocalFileTime);
	::FileTimeToDosDateTime(&ftLocalFileTime, &wFatDate, &wFatTime);
	return wFatTime;
}

DWORD COpenArchive::GetWriteTime()
{
	if (!m_bSearchMode || *(ULONGLONG*)&m_ftLastWriteTime == 0)
		return -1;
	return (DWORD)(((*(const ULONGLONG *)&m_ftLastWriteTime) - 0x19DB1DED53E8000) / 10000000);
}

DWORD COpenArchive::GetCreateTime()
{
	if (!m_bSearchMode)
		return -1;
	if (*(ULONGLONG*)&m_ftLastCreateTime == 0)
		return 0;
	return (DWORD)(((*(const ULONGLONG *)&m_ftLastCreateTime) - 0x19DB1DED53E8000) / 10000000);
}

DWORD COpenArchive::GetAccessTime()
{
	if (!m_bSearchMode)
		return -1;
	if (*(ULONGLONG*)&m_ftLastAccessTime == 0)
		return 0;
	return (DWORD)(((*(const ULONGLONG *)&m_ftLastAccessTime) - 0x19DB1DED53E8000) / 10000000);
}

BOOL COpenArchive::GetWriteTimeEx(FILETIME *lpftLastWriteTime)
{
	if (!m_bSearchMode || *(UINT64*)&m_ftLastWriteTime == 0)
		return FALSE;
	*lpftLastWriteTime = m_ftLastWriteTime;
	return TRUE;
}

BOOL COpenArchive::GetCreateTimeEx(FILETIME *lpftLastCreateTime)
{
	if (!m_bSearchMode || *(UINT64*)&m_ftLastCreateTime == 0)
		return FALSE;
	*lpftLastCreateTime = m_ftLastCreateTime;
	return TRUE;
}

BOOL COpenArchive::GetAccessTimeEx(FILETIME *lpftLastAccessTime)
{
	if (!m_bSearchMode || *(UINT64*)&m_ftLastAccessTime == 0)
		return FALSE;
	*lpftLastAccessTime = m_ftLastAccessTime;
	return TRUE;
}

BOOL COpenArchive::GetArcCreateTimeEx(FILETIME *lpftCreationTime)
{
	*lpftCreationTime = m_fiArchive.CTime;
	return TRUE;
}

BOOL COpenArchive::GetArcAccessTimeEx(FILETIME *lpftLastAccessTime)
{
	*lpftLastAccessTime = m_fiArchive.ATime;
	return TRUE;
}

BOOL COpenArchive::GetArcWriteTimeEx(FILETIME *lpftLastWriteTime)
{
	*lpftLastWriteTime = m_fiArchive.MTime;
	return TRUE;
}

BOOL COpenArchive::GetArcFileSizeEx(ULHA_INT64* lpllSize)
{
	*lpllSize = m_fiArchive.Size;
	return TRUE;
}

BOOL COpenArchive::GetArcOriginalSizeEx(ULHA_INT64* lpllSize)
{
	*lpllSize = (ULHA_INT64)m_ui64ArcOriginalSize;
	return TRUE;
}

BOOL COpenArchive::GetArcCompressedSizeEx(ULHA_INT64* lpllSize)
{
	*lpllSize = m_bSolid ? -1 : (ULHA_INT64)m_ui64ArcCompressedSize;
	return TRUE;
}

BOOL COpenArchive::GetOriginalSizeEx(ULHA_INT64* lpllSize)
{
	if (!m_bSearchMode)
		return FALSE;
	*lpllSize = (ULHA_INT64)m_ui64OriginalSize;
	return TRUE;
}

BOOL COpenArchive::GetCompressedSizeEx(ULHA_INT64* lpllSize)
{
	if (!m_bSearchMode)
		return FALSE;
	*lpllSize = (ULHA_INT64)m_ui64CompressedSize;
	return TRUE;
}

void COpenArchive::SetDefaultPassword(LPCSTR lpPassword)
{
	if (m_lpPassword)
		free(m_lpPassword);
	m_lpPassword = NULL;
	if (lpPassword)
		m_lpPassword = _wcsdup(g_StdOut.ConvertUnicodeString(lpPassword));
}
