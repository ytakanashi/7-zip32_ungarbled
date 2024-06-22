// Main.cpp

#include "StdAfx.h"

#include "../../../../../C/DllSecur.h"			// パス変更

#include "../../../../Common/MyWindows.h"		// パス変更
#include "../../../../Common/MyInitGuid.h"		// パス変更

#include "../../../../Common/CommandLineParser.h"		// パス変更
#include "../../../../Common/StringConvert.h"		// パス変更
#include "../../../../Common/TextConfig.h"		// パス変更

#include "../../../../Windows/DLL.h"		// パス変更
#include "../../../../Windows/ErrorMsg.h"		// パス変更
#include "../../../../Windows/FileDir.h"		// パス変更
#include "../../../../Windows/FileFind.h"		// パス変更
#include "../../../../Windows/FileIO.h"		// パス変更
#include "../../../../Windows/FileName.h"		// パス変更
#include "../../../../Windows/NtCheck.h"		// パス変更
#include "../../../../Windows/ResourceString.h"		// パス変更

#include "../../../UI/Explorer/MyMessages.h"		// パス変更

#include "../../SFXSetup/ExtractEngine.h"		// パス変更

#include "resource.h"

#include "ExtractDialog.h"				// 追加
#include "Windows/ResourceString.h"		// 追加

using namespace NWindows;
using namespace NFile;
using namespace NDir;

extern
HINSTANCE g_hInstance;
HINSTANCE g_hInstance;
extern
bool g_DisableUserQuestions;
bool g_DisableUserQuestions;

static CFSTR const kTempDirPrefix = FTEXT("7zSDJC");		// 変更

#define MY_SHELL_EXECUTE

static bool ReadDataString(CFSTR fileName, LPCSTR startID,
    LPCSTR endID, AString &stringResult)
{
  stringResult.Empty();
  NIO::CInFile inFile;
  if (!inFile.Open(fileName))
    return false;
  const size_t kBufferSize = (1 << 12);

  Byte buffer[kBufferSize];
  const unsigned signatureStartSize = MyStringLen(startID);
  const unsigned signatureEndSize = MyStringLen(endID);
  
  size_t numBytesPrev = 0;
  bool writeMode = false;
  UInt64 posTotal = 0;
  for (;;)
  {
    if (posTotal > (1 << 20))
      return (stringResult.IsEmpty());
    const size_t numReadBytes = kBufferSize - numBytesPrev;
    size_t processedSize;
    if (!inFile.ReadFull(buffer + numBytesPrev, numReadBytes, processedSize))
      return false;
    if (processedSize == 0)
      return true;
    const size_t numBytesInBuffer = numBytesPrev + processedSize;
    UInt32 pos = 0;
    for (;;)
    {
      if (writeMode)
      {
        if (pos + signatureEndSize > numBytesInBuffer)
          break;
        if (memcmp(buffer + pos, endID, signatureEndSize) == 0)
          return true;
        const Byte b = buffer[pos];
        if (b == 0)
          return false;
        stringResult += (char)b;
        pos++;
      }
      else
      {
        if (pos + signatureStartSize > numBytesInBuffer)
          break;
        if (memcmp(buffer + pos, startID, signatureStartSize) == 0)
        {
          writeMode = true;
          pos += signatureStartSize;
        }
        else
          pos++;
      }
    }
    numBytesPrev = numBytesInBuffer - pos;
    posTotal += pos;
    memmove(buffer, buffer + pos, numBytesPrev);
  }
}

static char kStartID[] = { ',','!','@','I','n','s','t','a','l','l','@','!','U','T','F','-','8','!', 0 };
static char kEndID[]   = { ',','!','@','I','n','s','t','a','l','l','E','n','d','@','!', 0 };

static struct CInstallIDInit
{
  CInstallIDInit()
  {
    kStartID[0] = ';';
    kEndID[0] = ';';
  }
} g_CInstallIDInit;


#if defined(_WIN32) && defined(_UNICODE) && !defined(_WIN64) && !defined(UNDER_CE)
#define NT_CHECK_FAIL_ACTION ShowErrorMessage(L"Unsupported Windows version"); return 1;
#endif

static void ShowErrorMessageSpec(const UString &name)
{
  UString message = NError::MyFormatMessage(::GetLastError());
  const int pos = message.Find(L"%1");
  if (pos >= 0)
  {
    message.Delete((unsigned)pos, 2);
    message.Insert((unsigned)pos, name);
  }
  ShowErrorMessage(NULL, message);
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /* hPrevInstance */,
    #ifdef UNDER_CE
    LPWSTR
    #else
    LPSTR
    #endif
    /* lpCmdLine */,int /* nCmdShow */)
{
  g_hInstance = (HINSTANCE)hInstance;

  NT_CHECK

  #ifdef _WIN32
  LoadSecurityDlls();
  #endif

  // InitCommonControls();

  UString archiveName, switches;
  #ifdef MY_SHELL_EXECUTE
  UString executeFile, executeParameters;
  #endif
  NCommandLineParser::SplitCommandLine(GetCommandLineW(), archiveName, switches);

  FString fullPath;
  NDLL::MyGetModuleFileName(fullPath);

  switches.Trim();
  bool assumeYes = false;
  if (switches.IsPrefixedBy_Ascii_NoCase("-y"))
  {
    assumeYes = true;
    switches = switches.Ptr(2);
    switches.Trim();
  }

  AString config;
  if (!ReadDataString(fullPath, kStartID, kEndID, config))
  {
    if (!assumeYes)
      ShowErrorMessageRes(IDS_CANT_LOAD_CONFIG_INFO);		// 変更
    return 1;
  }

  UString dirPrefix ("." STRING_PATH_SEPARATOR);
  UString appLaunched;
  bool showProgress = true;
  UString friendlyName;		// 追加
  UString installPrompt;	// 追加
  bool isInstaller;			// 追加
  if (!config.IsEmpty())
  {
    CObjectVector<CTextConfigPair> pairs;
    if (!GetTextConfig(config, pairs))
    {
      if (!assumeYes)
        ShowErrorMessageRes(IDS_CONFIG_FAILED);		// 変更
      return 1;
    }
    friendlyName = GetTextConfigValue(pairs, "Title");			// 変更
    installPrompt = GetTextConfigValue(pairs, "BeginPrompt");	// 変更
    isInstaller = GetTextConfigValue(pairs, "IsInstaller").IsEqualTo_NoCase(L"yes");	// 追加
    if (isInstaller)	// 追加
    {					// 追加
    UString progress = GetTextConfigValue(pairs, "Progress");
    if (progress.IsEqualTo_Ascii_NoCase("no"))
      showProgress = false;
    const int index = FindTextConfigItem(pairs, "Directory");
    if (index >= 0)
      dirPrefix = pairs[index].String;
    if (!installPrompt.IsEmpty() && !assumeYes)
    {
      if (MessageBoxW(NULL, installPrompt, friendlyName, MB_YESNO |
          MB_ICONQUESTION) != IDYES)
        return 0;
    }
    appLaunched = GetTextConfigValue(pairs, "RunProgram");
    
    #ifdef MY_SHELL_EXECUTE
    executeFile = GetTextConfigValue(pairs, "ExecuteFile");
    executeParameters = GetTextConfigValue(pairs, "ExecuteParameters");
    #endif
	}	// 追加
  }

  CTempDir tempDir;

  /* 追加 */
	UString tempDirPath;
  	if (isInstaller)
	{
		if (!tempDir.Create(kTempDirPrefix))
		{
			if (!assumeYes)
				ShowErrorMessageRes(IDS_CANT_CREATE_TEMP_FOLDER);
			return 1;
		}
		tempDirPath = GetUnicodeString(tempDir.GetPath());
	}
	else
	{
		tempDirPath = fullPath.Mid(0, fullPath.ReverseFind(L'\\') + 1);	// 変更
		if (!assumeYes)
		{
			if (friendlyName.IsEmpty())
				friendlyName = NWindows::MyLoadString(IDS_EXTRACT_FRIENDLY_NAME);
			if (installPrompt.IsEmpty())
				installPrompt = NWindows::MyLoadString(IDS_EXTRACT_INSTALL_PROMPT);
			CExtractDialog dlg;
			if (dlg.Create(friendlyName, installPrompt, tempDirPath, 0) != IDOK)
				return 0;
			tempDirPath = dlg.GetFolderName();
		}
	}
  /* 追加ここまで*/
  /* 削除 
  if (!tempDir.Create(kTempDirPrefix))
  {
    if (!assumeYes)
      ShowErrorMessage(L"Can not create temp folder archive");
    return 1;
  }
     削除ここまで */
  CCodecs *codecs = new CCodecs;
  CMyComPtr<IUnknown> compressCodecsInfo = codecs;
  {
    const HRESULT result = codecs->Load();
    if (result != S_OK)
    {
      ShowErrorMessageRes(IDS_CANT_LOAD_CODECS);			// 変更
      return 1;
    }
  }

//  const FString tempDirPath = tempDir.GetPath();	// 削除
  // tempDirPath = L"M:\\1\\"; // to test low disk space
  {
    bool isCorrupt = false;
    UString errorMessage;
    HRESULT result = ExtractArchive(codecs, fullPath, tempDirPath, showProgress,
      isCorrupt, errorMessage);
    
    if (result != S_OK)
    {
      if (!assumeYes)
      {
        if (result == S_FALSE || isCorrupt)
        {
          NWindows::MyLoadString(IDS_EXTRACTION_ERROR_MESSAGE, errorMessage);
          result = E_FAIL;
        }
        if (result != E_ABORT)
        {
          if (errorMessage.IsEmpty())
            errorMessage = NError::MyFormatMessage(result);
          ::MessageBoxW(NULL, errorMessage, NWindows::MyLoadString(IDS_EXTRACTION_ERROR_TITLE), MB_ICONERROR);
        }
      }
      return 1;
    }
  }
	if (!isInstaller)	// 追加
		return 0;		// 追加

  #ifndef UNDER_CE
  CCurrentDirRestorer currentDirRestorer;
  if (!SetCurrentDir(tempDirPath))
    return 1;
  #endif
  
  HANDLE hProcess = NULL;
#ifdef MY_SHELL_EXECUTE
  if (!executeFile.IsEmpty())
  {
    CSysString filePath (GetSystemString(executeFile));
    SHELLEXECUTEINFO execInfo;
    execInfo.cbSize = sizeof(execInfo);
    execInfo.fMask = SEE_MASK_NOCLOSEPROCESS
      #ifndef UNDER_CE
      | SEE_MASK_FLAG_DDEWAIT
      #endif
      ;
    execInfo.hwnd = NULL;
    execInfo.lpVerb = NULL;
    execInfo.lpFile = filePath;

    if (!switches.IsEmpty())
    {
      executeParameters.Add_Space_if_NotEmpty();
      executeParameters += switches;
    }

    const CSysString parametersSys (GetSystemString(executeParameters));
    if (parametersSys.IsEmpty())
      execInfo.lpParameters = NULL;
    else
      execInfo.lpParameters = parametersSys;

    execInfo.lpDirectory = NULL;
    execInfo.nShow = SW_SHOWNORMAL;
    execInfo.hProcess = NULL;
    /* BOOL success = */ ::ShellExecuteEx(&execInfo);
    UINT32 result = (UINT32)(UINT_PTR)execInfo.hInstApp;
    if (result <= 32)
    {
      if (!assumeYes)
        ShowErrorMessageRes(IDS_CANT_OPEN_FILE);			// 変更
      return 1;
    }
    hProcess = execInfo.hProcess;
  }
  else
#endif
  {
    if (appLaunched.IsEmpty())
    {
      appLaunched = L"setup.exe";
      if (!NFind::DoesFileExist_FollowLink(us2fs(appLaunched)))
      {
        if (!assumeYes)
          ShowErrorMessageRes(IDS_CANT_FIND_SETUP);			// 変更
        return 1;
      }
    }
    
    {
      FString s2 = tempDirPath;
      NName::NormalizeDirPathPrefix(s2);
      appLaunched.Replace(L"%%T" WSTRING_PATH_SEPARATOR, fs2us(s2));
    }
    
    const UString appNameForError = appLaunched; // actually we need to rtemove parameters also

    appLaunched.Replace(L"%%T", fs2us(tempDirPath));

    if (!switches.IsEmpty())
    {
      appLaunched.Add_Space();
      appLaunched += switches;
    }
    STARTUPINFO startupInfo;
    startupInfo.cb = sizeof(startupInfo);
    startupInfo.lpReserved = NULL;
    startupInfo.lpDesktop = NULL;
    startupInfo.lpTitle = NULL;
    startupInfo.dwFlags = 0;
    startupInfo.cbReserved2 = 0;
    startupInfo.lpReserved2 = NULL;
    
    PROCESS_INFORMATION processInformation;
    
    const CSysString appLaunchedSys (GetSystemString(dirPrefix + appLaunched));
    
    const BOOL createResult = CreateProcess(NULL,
        appLaunchedSys.Ptr_non_const(),
        NULL, NULL, FALSE, 0, NULL, NULL /*tempDir.GetPath() */,
        &startupInfo, &processInformation);
    if (createResult == 0)
    {
      if (!assumeYes)
      {
        // we print name of exe file, if error message is
        // ERROR_BAD_EXE_FORMAT: "%1 is not a valid Win32 application".
        ShowErrorMessageSpec(appNameForError);
      }
      return 1;
    }
    ::CloseHandle(processInformation.hThread);
    hProcess = processInformation.hProcess;
  }
  if (hProcess)
  {
    WaitForSingleObject(hProcess, INFINITE);
    ::CloseHandle(hProcess);
  }
  return 0;
}
