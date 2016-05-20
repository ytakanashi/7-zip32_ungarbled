// ExtractDialog.cpp: CExtractDialog クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ExtractDialog.h"
#include "shlobj.h"
#include "Common/StringConvert.h"
#include "Windows/ResourceString.h"
#include "Windows/Shell.h"

using namespace NWindows;

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

CExtractDialog::CExtractDialog()
{

}

CExtractDialog::~CExtractDialog()
{

}

bool CExtractDialog::OnInit() 
{
	SetText(m_friendlyName);
	SetItemText(IDC_STATIC_EXTRACT_EXTRACT_TO, m_installPrompt);
	SetItemText(IDC_EXTRACT_COMBO_PATH, m_folderName);
	return CModalDialog::OnInit();
}

void CExtractDialog::OnOK()
{
	CWindow window(GetItem(IDC_EXTRACT_COMBO_PATH));
	window.GetText(m_folderName);

	CModalDialog::OnOK();
}

bool CExtractDialog::OnButtonClicked(int buttonID, HWND buttonHWND) 
{ 
	switch(buttonID)
	{
    case IDC_EXTRACT_BUTTON_SET_PATH:
		{
			UString currentPath;
			CWindow window(GetItem(IDC_EXTRACT_COMBO_PATH));
			window.GetText(currentPath);
			UString title = MyLoadString(IDS_SELECT_FOLDER_TITLE);
			UString resultPath;
			if (NShell::BrowseForFolder(HWND(*this), title, currentPath, resultPath))
				window.SetText(resultPath);
			break;
		}
	}
	return CModalDialog::OnButtonClicked(buttonID, buttonHWND);
}