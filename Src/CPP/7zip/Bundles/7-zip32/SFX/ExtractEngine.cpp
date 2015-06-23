// ExtractEngine.cpp

#include "StdAfx.h"

#include "../../../../Windows/FileDir.h"		// パス変更
#include "../../../../Windows/FileName.h"		// パス変更
#include "../../../../Windows/Thread.h"		// パス変更

#include "../../../UI/Common/OpenArchive.h"		// パス変更

#include "../../../UI/FileManager/FormatUtils.h"	// パス変更

#include "ExtractCallbackSfx.h"
#include "../../SFXSetup/ExtractEngine.h"	// パス変更

using namespace NWindows;
using namespace NFile;
using namespace NDir;
/* 削除
static LPCWSTR kCantFindArchive = L"Can not find archive file";
static LPCWSTR kCantOpenArchive = L"Can not open the file as archive";
  削除ここまで */
struct CThreadExtracting
{
  CCodecs *Codecs;
  FString FileName;
  FString DestFolder;

  CExtractCallbackImp *ExtractCallbackSpec;
  CMyComPtr<IArchiveExtractCallback> ExtractCallback;

  CArchiveLink ArchiveLink;
  HRESULT Result;
  UString ErrorMessage;

  void Process2()
  {
    NFind::CFileInfo fi;
    if (!fi.Find(FileName))
    {
      ErrorMessage = NWindows::MyLoadString(IDS_CANT_FIND_ARCHIVE);	// 変更
      Result = E_FAIL;
      return;
    }

    CObjectVector<COpenType> incl;
    CIntVector excl;
    COpenOptions options;
    options.codecs = Codecs;
    options.types = &incl;
    options.excludedFormats = &excl;
    options.filePath = fs2us(FileName);
    
    Result = ArchiveLink.Open2(options, ExtractCallbackSpec);
    if (Result != S_OK)
    {
      if (Result != S_OK)
      ErrorMessage = NWindows::MyLoadString(IDS_CANT_OPNE_ARCHIVE);	// 変更
      return;
    }

    FString dirPath = DestFolder;
    NName::NormalizeDirPathPrefix(dirPath);
    
    if (!CreateComplexDir(dirPath))
    {
      ErrorMessage = MyFormatNew(IDS_CANNOT_CREATE_FOLDER,
        #ifdef LANG
        0x02000603,
        #endif
        fs2us(dirPath));
      Result = E_FAIL;
      return;
    }

    ExtractCallbackSpec->Init(ArchiveLink.GetArchive(), dirPath, L"Default", fi.MTime, 0);

    Result = ArchiveLink.GetArchive()->Extract(0, (UInt32)(Int32)-1 , BoolToInt(false), ExtractCallback);
  }

  void Process()
  {
    try
    {
      #ifndef _NO_PROGRESS
      CProgressCloser closer(ExtractCallbackSpec->ProgressDialog);
      #endif
      Process2();
    }
    catch(...) { Result = E_FAIL; }
  }
  
  static THREAD_FUNC_DECL MyThreadFunction(void *param)
  {
    ((CThreadExtracting *)param)->Process();
    return 0;
  }
};

HRESULT ExtractArchive(CCodecs *codecs, const FString &fileName, const FString &destFolder,
    bool showProgress, bool &isCorrupt, UString &errorMessage)
{
  isCorrupt = false;
  CThreadExtracting t;

  t.Codecs = codecs;
  t.FileName = fileName;
  t.DestFolder = destFolder;

  t.ExtractCallbackSpec = new CExtractCallbackImp;
  t.ExtractCallback = t.ExtractCallbackSpec;
  
  #ifndef _NO_PROGRESS

  if (showProgress)
  {
    t.ExtractCallbackSpec->ProgressDialog.IconID = IDI_ICON;
    NWindows::CThread thread;
    RINOK(thread.Create(CThreadExtracting::MyThreadFunction, &t));
    
    UString title;
    LangString(IDS_PROGRESS_EXTRACTING, title);
    t.ExtractCallbackSpec->StartProgressDialog(title, thread);
  }
  else

  #endif
  {
    t.Process2();
  }

  errorMessage = t.ErrorMessage;
  if (errorMessage.IsEmpty())
    errorMessage = t.ExtractCallbackSpec->_message;
  isCorrupt = t.ExtractCallbackSpec->_isCorrupt;
  return t.Result;
}
