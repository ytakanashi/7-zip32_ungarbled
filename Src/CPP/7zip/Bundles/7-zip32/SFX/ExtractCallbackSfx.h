// ExtractCallbackSfx.h

#ifndef __EXTRACT_CALLBACK_SFX_H
#define __EXTRACT_CALLBACK_SFX_H

#include "resource.h"

#include "../../../../Windows/ResourceString.h"			// �p�X�ύX

#include "../../../Archive/IArchive.h"					// �p�X�ύX

#include "../../../Common/FileStreams.h"				// �p�X�ύX
#include "../../../ICoder.h"							// �p�X�ύX

#include "../../../UI/FileManager/LangUtils.h"			// �p�X�ύX

#ifndef _NO_PROGRESS
#include "ProgressDialog.h"								// �p�X�ύX
#endif
#include "../../../UI/Common/ArchiveOpenCallback.h"		// �p�X�ύX

class CExtractCallbackImp:
  public IArchiveExtractCallback,
  public IOpenCallbackUI,
  public CMyUnknownImp
{
public:
  
  MY_UNKNOWN_IMP

  INTERFACE_IArchiveExtractCallback(;)
  INTERFACE_IOpenCallbackUI(;)

private:
  CMyComPtr<IInArchive> _archiveHandler;
  FString _directoryPath;
  UString _filePath;
  FString _diskFilePath;

  bool _extractMode;
  struct CProcessedFileInfo
  {
    FILETIME MTime;
    bool IsDir;
    UInt32 Attributes;
  } _processedFileInfo;

  COutFileStream *_outFileStreamSpec;
  CMyComPtr<ISequentialOutStream> _outFileStream;

  UString _itemDefaultName;
  FILETIME _defaultMTime;
  UInt32 _defaultAttributes;

  void CreateComplexDirectory(const UStringVector &dirPathParts);
public:
  #ifndef _NO_PROGRESS
  CProgressDialog ProgressDialog;
  #endif

  bool _isCorrupt;
  UString _message;

  void Init(IInArchive *archiveHandler,
    const FString &directoryPath,
    const UString &itemDefaultName,
    const FILETIME &defaultMTime,
    UInt32 defaultAttributes);

  #ifndef _NO_PROGRESS
  HRESULT StartProgressDialog(const UString &title, NWindows::CThread &thread)
  {
    ProgressDialog.Create(title, thread, 0);
    {
      ProgressDialog.SetText(LangString(IDS_PROGRESS_EXTRACTING));
    }

    ProgressDialog.Show(SW_SHOWNORMAL);
    return S_OK;
  }
  virtual ~CExtractCallbackImp() { ProgressDialog.Destroy(); }
  #endif

};

#endif
