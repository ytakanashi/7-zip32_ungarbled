// ExtractCallbackSfx.cpp

#include "StdAfx.h"

#include "../../../../Common/Wildcard.h"		// �p�X�ύX

#include "../../../../Windows/FileDir.h"		// �p�X�ύX
#include "../../../../Windows/FileFind.h"		// �p�X�ύX
#include "../../../../Windows/FileName.h"		// �p�X�ύX
#include "../../../../Windows/PropVariant.h"	// �p�X�ύX

#include "ExtractCallbackSfx.h"

using namespace NWindows;
using namespace NFile;
using namespace NDir;
/*  �폜
static LPCSTR const kCantDeleteFile = "Can not delete output file";
static LPCSTR const kCantOpenFile = "Can not open output file";
static LPCSTR const kUnsupportedMethod = "Unsupported Method";
  �폜�����܂� */
void CExtractCallbackImp::Init(IInArchive *archiveHandler,
    const FString &directoryPath,
    const UString &itemDefaultName,
    const FILETIME &defaultMTime,
    UInt32 defaultAttributes)
{
  _message.Empty();
  _isCorrupt = false;
  _itemDefaultName = itemDefaultName;
  _defaultMTime = defaultMTime;
  _defaultAttributes = defaultAttributes;
  _archiveHandler = archiveHandler;
  _directoryPath = directoryPath;
  NName::NormalizeDirPathPrefix(_directoryPath);
}

HRESULT CExtractCallbackImp::Open_CheckBreak()
{
  #ifndef _NO_PROGRESS
  return ProgressDialog.Sync.ProcessStopAndPause();
  #else
  return S_OK;
  #endif
}

HRESULT CExtractCallbackImp::Open_SetTotal(const UInt64 * /* numFiles */, const UInt64 * /* numBytes */)
{
  return S_OK;
}

HRESULT CExtractCallbackImp::Open_SetCompleted(const UInt64 * /* numFiles */, const UInt64 * /* numBytes */)
{
  #ifndef _NO_PROGRESS
  return ProgressDialog.Sync.ProcessStopAndPause();
  #else
  return S_OK;
  #endif
}

HRESULT CExtractCallbackImp::Open_Finished()
{
  return S_OK;
}

Z7_COM7F_IMF(CExtractCallbackImp::SetTotal(UInt64 size))
{
  #ifndef _NO_PROGRESS
  ProgressDialog.Sync.SetProgress(size, 0);
  #endif
  return S_OK;
}

Z7_COM7F_IMF(CExtractCallbackImp::SetCompleted(const UInt64 *completeValue))
{
  #ifndef _NO_PROGRESS
  RINOK(ProgressDialog.Sync.ProcessStopAndPause())
  if (completeValue != NULL)
    ProgressDialog.Sync.SetPos(*completeValue);
  #endif
  return S_OK;
}

void CExtractCallbackImp::CreateComplexDirectory(const UStringVector &dirPathParts)
{
  FString fullPath = _directoryPath;
  FOR_VECTOR (i, dirPathParts)
  {
    fullPath += us2fs(dirPathParts[i]);
    CreateDir(fullPath);
    fullPath.Add_PathSepar();
  }
}

Z7_COM7F_IMF(CExtractCallbackImp::GetStream(UInt32 index,
    ISequentialOutStream **outStream, Int32 askExtractMode))
{
  #ifndef _NO_PROGRESS
  if (ProgressDialog.Sync.GetStopped())
    return E_ABORT;
  #endif
  _outFileStream.Release();

  UString fullPath;
  {
    NCOM::CPropVariant prop;
    RINOK(_archiveHandler->GetProperty(index, kpidPath, &prop))
    if (prop.vt == VT_EMPTY)
      fullPath = _itemDefaultName;
    else
    {
      if (prop.vt != VT_BSTR)
        return E_FAIL;
      fullPath.SetFromBstr(prop.bstrVal);
    }
    _filePath = fullPath;
  }

  if (askExtractMode == NArchive::NExtract::NAskMode::kExtract)
  {
    NCOM::CPropVariant prop;
    RINOK(_archiveHandler->GetProperty(index, kpidAttrib, &prop))
    if (prop.vt == VT_EMPTY)
      _processedFileInfo.Attributes = _defaultAttributes;
    else
    {
      if (prop.vt != VT_UI4)
        return E_FAIL;
      _processedFileInfo.Attributes = prop.ulVal;
    }

    RINOK(_archiveHandler->GetProperty(index, kpidIsDir, &prop))
    _processedFileInfo.IsDir = VARIANT_BOOLToBool(prop.boolVal);

    bool isAnti = false;
    {
      NCOM::CPropVariant propTemp;
      RINOK(_archiveHandler->GetProperty(index, kpidIsAnti, &propTemp))
      if (propTemp.vt == VT_BOOL)
        isAnti = VARIANT_BOOLToBool(propTemp.boolVal);
    }

    RINOK(_archiveHandler->GetProperty(index, kpidMTime, &prop))
    switch (prop.vt)
    {
      case VT_EMPTY: _processedFileInfo.MTime = _defaultMTime; break;
      case VT_FILETIME: _processedFileInfo.MTime = prop.filetime; break;
      default: return E_FAIL;
    }

    UStringVector pathParts;
    SplitPathToParts(fullPath, pathParts);
    if (pathParts.IsEmpty())
      return E_FAIL;

    UString processedPath = fullPath;

    if (!_processedFileInfo.IsDir)
      pathParts.DeleteBack();
    if (!pathParts.IsEmpty())
    {
      if (!isAnti)
        CreateComplexDirectory(pathParts);
    }

    FString fullProcessedPath = _directoryPath + us2fs(processedPath);

    if (_processedFileInfo.IsDir)
    {
      _diskFilePath = fullProcessedPath;

      if (isAnti)
        RemoveDir(_diskFilePath);
      else
        SetDirTime(_diskFilePath, NULL, NULL, &_processedFileInfo.MTime);
      return S_OK;
    }

    NFind::CFileInfo fileInfo;
    if (fileInfo.Find(fullProcessedPath))
    {
      if (!DeleteFileAlways(fullProcessedPath))
      {
        _message = NWindows::MyLoadString(IDS_CANT_DELETE_FILE);	// �ύX
        return E_FAIL;
      }
    }

    if (!isAnti)
    {
      _outFileStreamSpec = new COutFileStream;
      CMyComPtr<ISequentialOutStream> outStreamLoc(_outFileStreamSpec);
      if (!_outFileStreamSpec->Create_ALWAYS(fullProcessedPath))
      {
        _message = NWindows::MyLoadString(IDS_CANT_OPEN_OUTPUT_FILE);		// �ύX
        return E_FAIL;
      }
      _outFileStream = outStreamLoc;
      *outStream = outStreamLoc.Detach();
    }
    _diskFilePath = fullProcessedPath;
  }
  else
  {
    *outStream = NULL;
  }
  return S_OK;
}

Z7_COM7F_IMF(CExtractCallbackImp::PrepareOperation(Int32 askExtractMode))
{
  _extractMode = (askExtractMode == NArchive::NExtract::NAskMode::kExtract);
  return S_OK;
}

Z7_COM7F_IMF(CExtractCallbackImp::SetOperationResult(Int32 resultEOperationResult))
{
  switch (resultEOperationResult)
  {
    case NArchive::NExtract::NOperationResult::kOK:
      break;

    default:
    {
      _outFileStream.Release();
      switch (resultEOperationResult)
      {
        case NArchive::NExtract::NOperationResult::kUnsupportedMethod:
          _message = NWindows::MyLoadString(IDS_UNSUPPORTED_METHOD);	// �ύX
          break;
        default:
          _isCorrupt = true;
      }
      return E_FAIL;
    }
  }
  if (_outFileStream != NULL)
  {
    _outFileStreamSpec->SetMTime(&_processedFileInfo.MTime);
    RINOK(_outFileStreamSpec->Close())
  }
  _outFileStream.Release();
  if (_extractMode)
    SetFileAttrib(_diskFilePath, _processedFileInfo.Attributes);
  return S_OK;
}
