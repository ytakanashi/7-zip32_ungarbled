// SplitCmdLine.cpp: CSplitCmdLine クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SplitCmdLine.h"
#include "MainAr.h"
#include "Common/StdOutStream.h"

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

CSplitCmdLine::CSplitCmdLine()
{
	m_argc = 0;
	m_argv = NULL;
	m_lpCmdLine = NULL;
	m_lpBaseDirectory = NULL;
	m_bHide = FALSE;
	m_bSfx = FALSE;
	m_bScs = FALSE;
	m_bScc = FALSE;
	m_codePage = g_StdOut.GetUnicodeMode() ? CP_UTF8 : CP_OEMCP;
}

CSplitCmdLine::~CSplitCmdLine()
{
	if (m_lpCmdLine)
		free(m_lpCmdLine);
	if (m_argv)
		free(m_argv);
}

BOOL CSplitCmdLine::Split(const char* lpCmdLine)
{
	// コマンドライン分割
	if (lpCmdLine)
		m_lpCmdLine = _strdup(lpCmdLine);
	else
		m_lpCmdLine = _strdup("");
	char* p = m_lpCmdLine;
	char* pStart;
	while (*p)
	{
		while (*p == ' ')
		{
			*p = '\0';
			++p;
		}
		if (*p == '\"')
		{
			pStart = p;
			++p;
			while (*p && !(*p == '\"' && (p[1] == ' ' || p[1] == '\0')))
				++p;
			if (*p == '\0')
				break;
			*pStart = '\0';
			*p = '\0';
			++p;
			++m_argc;
		}
		else if (*p != '\0')
		{
			while (*p && *p != ' ')
				++p;
			++m_argc;
		}
	}
	
	// コマンドライン割り当て
	int i;
	m_argv = (char**)malloc((m_argc + 2) * sizeof(char*));
	p = m_lpCmdLine;
	BOOL bSkip = FALSE;
	m_bScs = FALSE;
	m_bScc = FALSE;
	m_codePage = g_StdOut.GetUnicodeMode() ? CP_UTF8 : CP_OEMCP;
	for (i = 0; i < m_argc; ++i)
	{
		while (*p == NULL)
			++p;
		if (*p == '/')
			*p = '-';
		if ((!_stricmp(p, "-hide")) && !bSkip)
		{
			// 非表示モード
			m_bHide = TRUE;
			--m_argc;
			--i;
		}
#ifndef Z7_EXTERNAL_CODECS	// 追加
		else if ((!_stricmp(p, "-sfx")) && !bSkip)
		{
			// SFX作成
			m_bSfx = TRUE;
			--m_argc;
			--i;
		}
#endif	// 追加
		else if (IsBaseDir(p))
		{
			// 基準ディレクトリ設定
			m_lpBaseDirectory = p;
			--m_argc;
			--i;
		}
		else
		{
			if (p[0] == '-'  && (p[1] == 'S' || p[1] == 's') && (p[2] == 'C' || p[2] == 'c') && !bSkip)
			{
				if ((p[3] == 'S' || p[3] == 's'))		// リストファイルの文字コード指定チェック
					m_bScs = TRUE;
				else if (p[3] == 'C' || p[3] == 'c')	// 入出力の文字コード指定チェック
				{
					if (_stricmp(p + 4, "UTF-8") == 0)
						m_codePage = CP_UTF8;
					else if (_stricmp(p + 4, "WIN") == 0)
						m_codePage = CP_OEMCP;
					else if (_stricmp(p + 4, "DOS") == 0)
						m_codePage = CP_ACP;
					m_bScc = TRUE;
				}
			}
			m_argv[i] = p;
			if (strcmp(p, "--") == 0)
				bSkip = TRUE;
		}
		while (*p)
			++p;
	}
	m_argv[i] = "";

	if (*m_argv[0] == 'l' || *m_argv[0] == 't')
	{
		m_bHide = TRUE;
	}
	return TRUE;
}

// 基準ディレクトリか判定
BOOL CSplitCmdLine::IsBaseDir(const char* lpOption)
{
	if (*lpOption == '-' || *lpOption == '/')
		return FALSE;

	if (lpOption[strlen(lpOption) - 1] == '\\')
		return TRUE;
	return FALSE;
}

// コマンドが書庫操作系のコマンド(a,d,u)か調査
BOOL CSplitCmdLine::IsUpdateCommands()
{
	return (_stricmp(m_argv[0], "a") == 0 || _stricmp(m_argv[0], "d") == 0 || _stricmp(m_argv[0], "u") == 0);
}
