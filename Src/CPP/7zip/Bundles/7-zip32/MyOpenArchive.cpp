// MyOpenArchive.cpp: COpenArchive �N���X�̃C���v�������e�[�V����
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
#include "OpenCallbackConsole.h"		// �p�X�ύX
#include "ExtractCallbackConsole.h"		// �p�X�ύX
#include "Common/UTFConvert.h"
#include "../../UI/Common/SetProperties.h"	// �ǉ�
#include "Common/IntToString.h"				// �ǉ�
#include "../../UI/Common/PropIDUtils.h"	// �ǉ�(15120002)


static COpenArchive* pOATail;

extern CStdOutStream *g_StdStream;	// �ǉ�
extern CStdOutStream *g_ErrStream;	// �ǉ�

extern UINT32 g_ArcCodePage;		// �ǉ�

/* �ǉ��������� */
#ifndef _7ZIP_ST
#include "../../../Windows/Synchronization.h"
#endif

using namespace NWindows;

CCodecs *g_CodecsObj;

#ifdef Z7_EXTERNAL_CODECS
  CExternalCodecs g_ExternalCodecs;
  CCodecs::CReleaser g_CodecsReleaser;
#else
  CMyComPtr<IUnknown> g_CodecsRef;
#endif

#ifndef _7ZIP_ST
static NSynchronization::CCriticalSection g_CriticalSection;
#define MT_LOCK NSynchronization::CCriticalSectionLock lock(g_CriticalSection);
#else
#define MT_LOCK
#endif

void FreeGlobalCodecs()
{
  MT_LOCK

  #ifdef Z7_EXTERNAL_CODECS
  if (g_CodecsObj)
  {
    g_CodecsObj->CloseLibs();
  }
  g_CodecsReleaser.Set(NULL);
  g_CodecsObj = NULL;
  g_ExternalCodecs.ClearAndRelease();
  #else
  g_CodecsRef.Release();
  #endif
}

HRESULT LoadGlobalCodecs()
{
  MT_LOCK

  if (g_CodecsObj)
    return S_OK;

  g_CodecsObj = new CCodecs;

  #ifdef Z7_EXTERNAL_CODECS
  g_ExternalCodecs.GetCodecs = g_CodecsObj;
  g_ExternalCodecs.GetHashers = g_CodecsObj;
  g_CodecsReleaser.Set(g_CodecsObj);
  #else
  g_CodecsRef.Release();
  g_CodecsRef = g_CodecsObj;
  #endif

  RINOK(g_CodecsObj->Load());
  if (g_CodecsObj->Formats.IsEmpty())
  {
    FreeGlobalCodecs();
    return E_NOTIMPL;
  }

  #ifdef Z7_EXTERNAL_CODECS
  RINOK(g_ExternalCodecs.Load());
  #endif

  return S_OK;
}
/* �ǉ������܂� */

//////////////////////////////////////////////////////////////////////
// �\�z/����
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

// �����Y��̃I�u�W�F�N�g���폜
void COpenArchive::RemoveAll()
{
	FreeGlobalCodecs();		// �ǉ�
	while (pOATail)
		delete pOATail;
}

// �I�u�W�F�N�g�̃|�C���^������
COpenArchive* COpenArchive::FindObject(HARC hArc)
{
	for (COpenArchive* pOA = pOATail; pOA; pOA = pOA->m_pOAPrev)
	{
		if (pOA == (COpenArchive*)hArc)
			return pOA;
	}
	return NULL;
}

// ���ɂ��J��
// �߂�l S_OK:���� ERROR_PASSWORD_FILE:�J�������ǃp�X���[�h�ُ� ���̑�:���s
HRESULT COpenArchive::OpenCheck(LPCWSTR lpFileName, DWORD dwMode)
{
	HRESULT result;
	try
	{
		/* �폜��������
		CCodecs *codecs = new CCodecs;
		CMyComPtr<
			#ifdef Z7_EXTERNAL_CODECS
			ICompressCodecsInfo
			#else
			IUnknown
			#endif
			> compressCodecsInfo = codecs;
		�폜�����܂� */

		RINOK(LoadGlobalCodecs());		// �ǉ�

		CObjectVector<COpenType> types;
		CIntVector excludedFormats;		// �ύX
		CStdOutStream stdOut;
		COpenCallbackConsole openCallback;
//		openCallback.OutStream = &stdOut;	// �폜
		openCallback.Init(g_StdStream, g_ErrStream, NULL, true);		// �ǉ�
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
				
//				ecs->OutStream = &stdOut;	// �폜
				ecs->PasswordIsDefined = openCallback.PasswordIsDefined;
				ecs->Password = openCallback.Password;
				ecs->Init(g_StdStream, g_ErrStream, NULL, true);	// �ύX
				
				CArcCmdLineOptions options;
				CArcCmdLineParser parser;			// �ύX
				UStringVector commandStrings;
				commandStrings.Add(L"t");
				commandStrings.Add(lpFileName);
				parser.Parse1(commandStrings, options);
				parser.Parse2(options);

				/* �ǉ��������� */
				if (!ParseOpenTypes(*g_CodecsObj, options.ArcType, types))
					return ERROR_NOT_ARC_FILE;

				FOR_VECTOR (k, options.ExcludedArcTypes)
				{
					CIntVector tempIndices;
					if (!g_CodecsObj->FindFormatForArchiveType(options.ExcludedArcTypes[k], tempIndices)
						|| tempIndices.Size() != 1)
						return ERROR_NOT_ARC_FILE;
					excludedFormats.AddToUniqueSorted(tempIndices[0]);
					// excludedFormats.Sort();
				}
				/* �ǉ������܂� */
				/* �폜��������
				if (!codecs->FindFormatForArchiveType(options.ArcType, formatIndices))
					return ERROR_NOT_ARC_FILE;
				�폜�����܂� */
				CExtractOptions eo;
				eo.StdInMode = options.StdInMode;
				eo.StdOutMode = options.StdOutMode;
				eo.PathMode = options.Command.GetPathMode();
				eo.TestMode = options.Command.IsTestCommand();
				eo.OverwriteMode = options.ExtractOptions.OverwriteMode;
				eo.OutputDir = options.ExtractOptions.OutputDir;
				eo.YesToAll = options.YesToAll;
//				eo.CalcCrc = options.CalcCrc;	// �폜
				eo.Properties = options.Properties;

				UString errorMessage;
				CDecompressStat stat;
				/* �ǉ��������� */
				CHashBundle hb;
				IHashCalc *hashCalc = NULL;

				if (!options.HashMethods.IsEmpty())
				{
					hashCalc = &hb;
					RINOK(hb.SetMethods(EXTERNAL_CODECS_VARS_G options.HashMethods));
					// hb.Init(); 	// �ǉ�(18060001)
				}

				UStringVector ArchivePathsSorted;
				UStringVector ArchivePathsFullSorted;
				/* �ǉ������܂� */
				result = Extract(		// �ύX
					g_CodecsObj,		// �ύX
					types,				// �ǉ�
					excludedFormats,	// �ύX
					ArchivePathsSorted, // �ύX
					ArchivePathsFullSorted,	// �ύX
					options.Censor.Pairs.Front().Head,			// �ύX
					eo, &openCallback, ecs, ecs, hashCalc, errorMessage, stat);	// �ύX
				if ((ecs->NumArcsWithError != 0 || ecs->NumFileErrors != 0) && result == S_OK)	// �ύX
					result = ERROR_WARNING;
				return result;
			}
		case CHECKARCHIVE_RAPID:
			openCallback.PasswordIsDefined = true;
		case CHECKARCHIVE_BASIC:
			{
				COpenOptions options;
				options.props = NULL;
				options.codecs = g_CodecsObj;	// �ύX
				options.types = &types;
				options.excludedFormats = &excludedFormats;
				options.stdInMode = false;
				options.filePath = lpFileName;
				result = m_archiveLink.Open2(options, &openCallback);
				if (result != S_OK)
				{
					if (dwMode == CHECKARCHIVE_RAPID && m_archiveLink.PasswordWasAsked)	// �ύX
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
		if (g_CodecsObj->Formats[arc.FormatIndex].Name.IsEqualTo_NoCase(L"7z"))	// �ύX	// arc.FormatIndex��nArchiveType�ɑΉ����邩���B�����������̌݊����Ŗ��o�邩�H
			m_nArchiveType = ARCHIVETYPE_7Z;
		else if (g_CodecsObj->Formats[arc.FormatIndex].Name.IsEqualTo_NoCase(L"Zip"))	// �ύX
			m_nArchiveType = ARCHIVETYPE_ZIP;
		/* �ǉ��������� */
		else if (g_CodecsObj->Formats[arc.FormatIndex].Name.IsEqualTo_NoCase(L"APM"))
			m_nArchiveType = ARCHIVETYPE_APM;
		else if (g_CodecsObj->Formats[arc.FormatIndex].Name.IsEqualTo_NoCase(L"Ar"))
			m_nArchiveType = ARCHIVETYPE_AR;
		else if (g_CodecsObj->Formats[arc.FormatIndex].Name.IsEqualTo_NoCase(L"Arj"))
			m_nArchiveType = ARCHIVETYPE_ARJ;
		else if (g_CodecsObj->Formats[arc.FormatIndex].Name.IsEqualTo_NoCase(L"bzip2"))
			m_nArchiveType = ARCHIVETYPE_BZIP2;
		else if (g_CodecsObj->Formats[arc.FormatIndex].Name.IsEqualTo_NoCase(L"Compound"))
			m_nArchiveType = ARCHIVETYPE_COMPOUND;
		else if (g_CodecsObj->Formats[arc.FormatIndex].Name.IsEqualTo_NoCase(L"Cpio"))
			m_nArchiveType = ARCHIVETYPE_CPIO;
		else if (g_CodecsObj->Formats[arc.FormatIndex].Name.IsEqualTo_NoCase(L"CramFS"))
			m_nArchiveType = ARCHIVETYPE_CRAMFS;
		else if (g_CodecsObj->Formats[arc.FormatIndex].Name.IsEqualTo_NoCase(L"Dmg"))
			m_nArchiveType = ARCHIVETYPE_DMG;
		else if (g_CodecsObj->Formats[arc.FormatIndex].Name.IsEqualTo_NoCase(L"ELF"))
			m_nArchiveType = ARCHIVETYPE_ELF;
		else if (g_CodecsObj->Formats[arc.FormatIndex].Name.IsEqualTo_NoCase(L"Ext"))
			m_nArchiveType = ARCHIVETYPE_EXT;
		else if (g_CodecsObj->Formats[arc.FormatIndex].Name.IsEqualTo_NoCase(L"FAT"))
			m_nArchiveType = ARCHIVETYPE_FAT;
		else if (g_CodecsObj->Formats[arc.FormatIndex].Name.IsEqualTo_NoCase(L"FLV"))
			m_nArchiveType = ARCHIVETYPE_FLV;
		else if (g_CodecsObj->Formats[arc.FormatIndex].Name.IsEqualTo_NoCase(L"gzip"))
			m_nArchiveType = ARCHIVETYPE_GZIP;
		else if (g_CodecsObj->Formats[arc.FormatIndex].Name.IsEqualTo_NoCase(L"GPT"))
			m_nArchiveType = ARCHIVETYPE_GPT;
		else if (g_CodecsObj->Formats[arc.FormatIndex].Name.IsEqualTo_NoCase(L"HFS"))
			m_nArchiveType = ARCHIVETYPE_HFS;
		else if (g_CodecsObj->Formats[arc.FormatIndex].Name.IsEqualTo_NoCase(L"IHex"))
			m_nArchiveType = ARCHIVETYPE_IHEX;
		else if (g_CodecsObj->Formats[arc.FormatIndex].Name.IsEqualTo_NoCase(L"Lzh"))
			m_nArchiveType = ARCHIVETYPE_LZH;
		else if (g_CodecsObj->Formats[arc.FormatIndex].Name.IsEqualTo_NoCase(L"lzma"))
			m_nArchiveType = ARCHIVETYPE_LZMA;
		else if (g_CodecsObj->Formats[arc.FormatIndex].Name.IsEqualTo_NoCase(L"lzma86"))
			m_nArchiveType = ARCHIVETYPE_LZMA86;
		else if (g_CodecsObj->Formats[arc.FormatIndex].Name.IsEqualTo_NoCase(L"MachO"))
			m_nArchiveType = ARCHIVETYPE_MACHO;
		else if (g_CodecsObj->Formats[arc.FormatIndex].Name.IsEqualTo_NoCase(L"MBR"))
			m_nArchiveType = ARCHIVETYPE_MBR;
		else if (g_CodecsObj->Formats[arc.FormatIndex].Name.IsEqualTo_NoCase(L"MsLZ"))
			m_nArchiveType = ARCHIVETYPE_MSLZ;
		else if (g_CodecsObj->Formats[arc.FormatIndex].Name.IsEqualTo_NoCase(L"MuB"))
			m_nArchiveType = ARCHIVETYPE_MUB;
		else if (g_CodecsObj->Formats[arc.FormatIndex].Name.IsEqualTo_NoCase(L"NTFS"))
			m_nArchiveType = ARCHIVETYPE_NTFS;
		else if (g_CodecsObj->Formats[arc.FormatIndex].Name.IsEqualTo_NoCase(L"PE"))
			m_nArchiveType = ARCHIVETYPE_PE;
		else if (g_CodecsObj->Formats[arc.FormatIndex].Name.IsEqualTo_NoCase(L"COFF"))
			m_nArchiveType = ARCHIVETYPE_COFF;
		else if (g_CodecsObj->Formats[arc.FormatIndex].Name.IsEqualTo_NoCase(L"TE"))
			m_nArchiveType = ARCHIVETYPE_TE;
		else if (g_CodecsObj->Formats[arc.FormatIndex].Name.IsEqualTo_NoCase(L"Ppmd"))
			m_nArchiveType = ARCHIVETYPE_PPMD;
		else if (g_CodecsObj->Formats[arc.FormatIndex].Name.IsEqualTo_NoCase(L"QCOW"))
			m_nArchiveType = ARCHIVETYPE_QCOW;
		else if (g_CodecsObj->Formats[arc.FormatIndex].Name.IsEqualTo_NoCase(L"Rpm"))
			m_nArchiveType = ARCHIVETYPE_RPM;
		else if (g_CodecsObj->Formats[arc.FormatIndex].Name.IsEqualTo_NoCase(L"Split"))
			m_nArchiveType = ARCHIVETYPE_SPLIT;
		else if (g_CodecsObj->Formats[arc.FormatIndex].Name.IsEqualTo_NoCase(L"SquashFS"))
			m_nArchiveType = ARCHIVETYPE_SQUASHFS;
		else if (g_CodecsObj->Formats[arc.FormatIndex].Name.IsEqualTo_NoCase(L"SWFc"))
			m_nArchiveType = ARCHIVETYPE_SWFC;
		else if (g_CodecsObj->Formats[arc.FormatIndex].Name.IsEqualTo_NoCase(L"SWF"))
			m_nArchiveType = ARCHIVETYPE_SWF;
		else if (g_CodecsObj->Formats[arc.FormatIndex].Name.IsEqualTo_NoCase(L"UEFIc"))
			m_nArchiveType = ARCHIVETYPE_UEFIC;
		else if (g_CodecsObj->Formats[arc.FormatIndex].Name.IsEqualTo_NoCase(L"UEFIf"))
			m_nArchiveType = ARCHIVETYPE_UEFIF;
		else if (g_CodecsObj->Formats[arc.FormatIndex].Name.IsEqualTo_NoCase(L"VDI"))
			m_nArchiveType = ARCHIVETYPE_VDI;
		else if (g_CodecsObj->Formats[arc.FormatIndex].Name.IsEqualTo_NoCase(L"VHD"))
			m_nArchiveType = ARCHIVETYPE_VHD;
		else if (g_CodecsObj->Formats[arc.FormatIndex].Name.IsEqualTo_NoCase(L"VMDK"))
			m_nArchiveType = ARCHIVETYPE_VMDK;
		else if (g_CodecsObj->Formats[arc.FormatIndex].Name.IsEqualTo_NoCase(L"Xar"))
			m_nArchiveType = ARCHIVETYPE_XAR;
		else if (g_CodecsObj->Formats[arc.FormatIndex].Name.IsEqualTo_NoCase(L"xz"))
			m_nArchiveType = ARCHIVETYPE_XZ;
		else if (g_CodecsObj->Formats[arc.FormatIndex].Name.IsEqualTo_NoCase(L"Z"))
			m_nArchiveType = ARCHIVETYPE_Z;
		else if (g_CodecsObj->Formats[arc.FormatIndex].Name.IsEqualTo_NoCase(L"Cab"))
			m_nArchiveType = ARCHIVETYPE_CAB;
		else if (g_CodecsObj->Formats[arc.FormatIndex].Name.IsEqualTo_NoCase(L"Chm"))
			m_nArchiveType = ARCHIVETYPE_CHM;
		else if (g_CodecsObj->Formats[arc.FormatIndex].Name.IsEqualTo_NoCase(L"Hxs"))
			m_nArchiveType = ARCHIVETYPE_HXS;
		else if (g_CodecsObj->Formats[arc.FormatIndex].Name.IsEqualTo_NoCase(L"Iso"))
			m_nArchiveType = ARCHIVETYPE_ISO;
		else if (g_CodecsObj->Formats[arc.FormatIndex].Name.IsEqualTo_NoCase(L"Nsis"))
			m_nArchiveType = ARCHIVETYPE_NSIS;
		else if (g_CodecsObj->Formats[arc.FormatIndex].Name.IsEqualTo_NoCase(L"Rar"))
			m_nArchiveType = ARCHIVETYPE_RAR;
		else if (g_CodecsObj->Formats[arc.FormatIndex].Name.IsEqualTo_NoCase(L"Rar5"))
			m_nArchiveType = ARCHIVETYPE_RAR5;
		else if (g_CodecsObj->Formats[arc.FormatIndex].Name.IsEqualTo_NoCase(L"tar"))
			m_nArchiveType = ARCHIVETYPE_TAR;
		else if (g_CodecsObj->Formats[arc.FormatIndex].Name.IsEqualTo_NoCase(L"Udf"))
			m_nArchiveType = ARCHIVETYPE_UDF;
		else if (g_CodecsObj->Formats[arc.FormatIndex].Name.IsEqualTo_NoCase(L"wim"))
			m_nArchiveType = ARCHIVETYPE_WIM;
		/* �ǉ������܂� */
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
	/* �ǉ��������� */
	//���ɂ̃R�[�h�y�[�W���w��
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
	/* �ǉ������܂� */
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
				recursed = DoesNameContainWildcard(s[i]);	// �ύX
			else
				recursed = true;
		}
		NWildcard::CCensorPathProps props;// �ǉ�
		props.Recursive = recursed;// �ǉ�
		props.WildcardMatching = true;// �ǉ�
		m_pWildcardCensor->AddItem(NWildcard::k_RelatPath, true, s[i], props);// �ύX
	}

	m_aItemPos = 0;
	m_ui64ArcOriginalSize = 0;
	m_ui64ArcCompressedSize = 0;
	m_bSearchMode = true;
	return FindNext(lpSubInfo);
}

bool CensorNode_CheckPath2(const NWildcard::CCensorNode &node, const CReadArcItem &item, bool &include);	// �ǉ�

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
		if (arc.GetItem_Path(m_aItemPos, m_aFileName) == S_OK)
		{
			bool isFolder = false;
			Archive_IsItem_Dir(arc.Archive, m_aItemPos, isFolder);	// �ύX
			/* �ǉ��������� */
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
			/* �ǉ������܂� */
//			if (Censor_CheckPath(*m_pWildcardCensor, item))	// �ύX,�폜
			if (pathOk)
			{
				if (isFolder)
				{
					if (m_aFileName.Back() == '\\')		// �ύX
						m_aFileName.DeleteBack();		// �ύX
					m_aFileName += L"\\";
				}
				break;
			}
		}
		++m_aItemPos;
	}
	arc.Archive->GetProperty(m_aItemPos, kpidSize, &aPropVariant);
	ConvertPropVariantToUInt64(aPropVariant, m_ui64OriginalSize);	// �ύX
	arc.Archive->GetProperty(m_aItemPos, kpidPackSize, &aPropVariant);
//	m_ui64CompressedSize = m_bSolid ? 0 : ConvertPropVariantToUInt64(aPropVariant);	// �폜
	/* �ǉ��������� */
	if (m_bSolid)
		m_ui64CompressedSize = 0;
	else
		ConvertPropVariantToUInt64(aPropVariant, m_ui64CompressedSize);
	/* �ǉ������܂� */
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
//	m_aMode = aPropVariant.vt ? aPropVariant.bstrVal : L"";	// �폜(15120002)
	UString s;												// �ǉ�(15120002)
	if (aPropVariant.vt != VT_EMPTY)						// �ǉ�(15120002)
		ConvertPropertyToString2(s,aPropVariant,kpidMethod);	// �ǉ�(15120002)
	m_aMode = (!s.IsEmpty()) ? s.Ptr() : L"";				// �ǉ�(15120002)
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
