#if !defined(SEVENZIP_H)
#define SEVENZIP_H

#define SEVENZIP32_VERSION	24070001

#ifndef FNAME_MAX32
#define FNAME_MAX32		512
#endif

/* �\���̒�` */
#ifndef ARC_DECSTRACT
#define ARC_DECSTRACT

#if !defined(__BORLANDC__)
#pragma pack(1)
#else
#pragma option -a-
#endif

#if !defined(__BORLANDC__) || __BORLANDC__ >= 0x550
typedef LONGLONG	ULHA_INT64;
#else
typedef struct {
	DWORD	LowPart;
	LONG	HighPart;
} ULHA_INT64, *LPULHA_INT64;
#endif

typedef	HGLOBAL	HARC;

typedef struct {
	DWORD	dwOriginalSize;
	DWORD	dwCompressedSize;
	DWORD	dwCRC;
	UINT	uFlag;
	UINT	uOSType;
	WORD	wRatio;
	WORD	wDate;
	WORD	wTime;
	char	szFileName[FNAME_MAX32 + 1];
	char	dummy1[3];
	char	szAttribute[8];
	char	szMode[8];
} INDIVIDUALINFO, *LPINDIVIDUALINFO;

typedef struct {
	DWORD 			dwFileSize;
	DWORD			dwWriteSize;
	char			szSourceFileName[FNAME_MAX32 + 1];
	char			dummy1[3];
	char			szDestFileName[FNAME_MAX32 + 1];
	char			dummy[3];
}	EXTRACTINGINFO, *LPEXTRACTINGINFO;

typedef struct {
	EXTRACTINGINFO exinfo;
	DWORD dwCompressedSize;
	DWORD dwCRC;
	UINT  uOSType;
	WORD  wRatio;
	WORD  wDate;
	WORD  wTime;
	char  szAttribute[8];
	char  szMode[8];
} EXTRACTINGINFOEX, *LPEXTRACTINGINFOEX;

typedef struct {
	DWORD			dwStructSize;
	EXTRACTINGINFO	exinfo;
	DWORD			dwFileSize;
	DWORD			dwCompressedSize;
	DWORD			dwWriteSize;
	DWORD			dwAttributes;
	DWORD 			dwCRC;
	UINT  			uOSType;
	WORD  			wRatio;
	FILETIME		ftCreateTime;
	FILETIME		ftAccessTime;
	FILETIME		ftWriteTime;
	char  			szMode[8];
	char			szSourceFileName[FNAME_MAX32 + 1];
	char			dummy1[3];
	char			szDestFileName[FNAME_MAX32 + 1];
	char			dummy2[3];
} EXTRACTINGINFOEX32, *LPEXTRACTINGINFOEX32;

typedef struct {
	DWORD			dwStructSize;
	EXTRACTINGINFO	exinfo;
	ULHA_INT64		llFileSize;
	ULHA_INT64		llCompressedSize;
	ULHA_INT64		llWriteSize;
	DWORD			dwAttributes;
	DWORD 			dwCRC;
	UINT  			uOSType;
	WORD  			wRatio;
	FILETIME		ftCreateTime;
	FILETIME		ftAccessTime;
	FILETIME		ftWriteTime;
	char  			szMode[8];
	char			szSourceFileName[FNAME_MAX32 + 1];
	char			dummy1[3];
	char			szDestFileName[FNAME_MAX32 + 1];
	char			dummy2[3];
} EXTRACTINGINFOEX64, *LPEXTRACTINGINFOEX64;

#if !defined(__BORLANDC__)
#pragma pack()
#else
#pragma option -a.
#endif

#endif /* ARC_DECSTRACT */

/* �E�B���h�E���b�Z�[�W */
#ifndef WM_ARCEXTRACT
#define	WM_ARCEXTRACT			"wm_arcextract"
#define	ARCEXTRACT_BEGIN		0
#define	ARCEXTRACT_INPROCESS	1
#define	ARCEXTRACT_END			2
#define ARCEXTRACT_OPEN			3
#define ARCEXTRACT_COPY			4
#define ARCEXTRACT_SKIP			5	// �ǉ�(19000002)
typedef BOOL CALLBACK ARCHIVERPROC(HWND _hwnd, UINT _uMsg, UINT _nState, LPVOID _lpEis);
typedef ARCHIVERPROC *LPARCHIVERPROC;
#endif

/* API�̐錾 */
#ifdef __cplusplus
extern "C" {
#endif

	/* LHA.DLL�݊�API */
	int   WINAPI SevenZip(const HWND _hwnd, LPCSTR _szCmdLine, LPSTR _szOutput, const DWORD _dwSize);
	WORD  WINAPI SevenZipGetVersion();
	BOOL  WINAPI SevenZipGetCursorMode();
	BOOL  WINAPI SevenZipSetCursorMode(const BOOL _CursorMode);
	BOOL  WINAPI SevenZipGetBackGroundMode();
	BOOL  WINAPI SevenZipSetBackGroundMode(const BOOL _BackGroundMode);
	WORD  WINAPI SevenZipGetCursorInterval();
	BOOL  WINAPI SevenZipSetCursorInterval(const WORD _Interval);
	BOOL  WINAPI SevenZipGetRunning();
	int   WINAPI SevenZipExtractMem(HWND _hwnd, LPCSTR _szCmdLine, LPBYTE _szBuffer, const DWORD _dwSize, time_t * _lpTime, LPWORD _lpwAttr, LPDWORD _lpdwWriteSize);	// �ǉ�
	int   WINAPI SevenZipExtractMemEx(HWND _hwnd, LPCSTR _szCmdLine, LPBYTE _szBuffer, const ULHA_INT64 _llSize, time_t * _lpTime, LPWORD _lpwAttr, LPDWORD _lpdwWriteSize);	// �ǉ�

	/* �����A�[�J�C�o����API */
	BOOL  WINAPI SevenZipConfigDialog(const HWND _hwnd, LPSTR _szOptionBuffer, const int _iMode);
	BOOL  WINAPI SevenZipCheckArchive(LPCSTR _szFileName, const int _iMode);
	int   WINAPI SevenZipGetFileCount(LPCSTR _szArcFile);
	BOOL  WINAPI SevenZipQueryFunctionList(const int _iFunction);

	/* OpenArchive�nAPI */
	HARC  WINAPI SevenZipOpenArchive(const HWND _hwnd, LPCSTR _szFileName, const DWORD _dwMode);
	int   WINAPI SevenZipCloseArchive(HARC _harc);
	int   WINAPI SevenZipFindFirst(HARC _harc, LPCSTR _szWildName, INDIVIDUALINFO *_lpSubInfo);
	int   WINAPI SevenZipFindNext(HARC _harc, INDIVIDUALINFO *_lpSubInfo);
	int   WINAPI SevenZipGetArcFileName(HARC _harc, LPSTR _lpBuffer, const int _nSize);
	DWORD WINAPI SevenZipGetArcFileSize(HARC _harc);
	DWORD WINAPI SevenZipGetArcOriginalSize(HARC _harc);
	DWORD WINAPI SevenZipGetArcCompressedSize(HARC _harc);
	WORD  WINAPI SevenZipGetArcRatio(HARC _harc);
	WORD  WINAPI SevenZipGetArcDate(HARC _harc);
	WORD  WINAPI SevenZipGetArcTime(HARC _harc);
	UINT  WINAPI SevenZipGetArcOSType(HARC _harc);
	int   WINAPI SevenZipIsSFXFile(HARC _harc);
	int   WINAPI SevenZipGetFileName(HARC _harc, LPSTR _lpBuffer, const int _nSize);
	DWORD WINAPI SevenZipGetOriginalSize(HARC _harc);
	DWORD WINAPI SevenZipGetCompressedSize(HARC _harc);
	WORD  WINAPI SevenZipGetRatio(HARC _harc);
	WORD  WINAPI SevenZipGetDate(HARC _harc);
	WORD  WINAPI SevenZipGetTime(HARC _harc);
	DWORD WINAPI SevenZipGetCRC(HARC _harc);
	int   WINAPI SevenZipGetAttribute(HARC _harc);
	UINT  WINAPI SevenZipGetOSType(HARC _harc);
	int   WINAPI SevenZipGetMethod(HARC _harc, LPSTR _lpBuffer, const int _nSize);
	DWORD WINAPI SevenZipGetWriteTime(HARC _harc);
	DWORD WINAPI SevenZipGetCreateTime(HARC _harc);
	DWORD WINAPI SevenZipGetAccessTime(HARC _harc);
	BOOL  WINAPI SevenZipGetWriteTimeEx(HARC _harc, FILETIME *_lpftLastWriteTime);
	BOOL  WINAPI SevenZipGetCreateTimeEx(HARC _harc, FILETIME *_lpftCreationTime);
	BOOL  WINAPI SevenZipGetAccessTimeEx(HARC _harc, FILETIME *_lpftLastAccessTime);
	BOOL  WINAPI SevenZipGetArcWriteTimeEx(HARC _harc, FILETIME *_lpftLastWriteTime);
	BOOL  WINAPI SevenZipGetArcCreateTimeEx(HARC _harc, FILETIME *_lpftCreationTime);
	BOOL  WINAPI SevenZipGetArcAccessTimeEx(HARC _harc, FILETIME *_lpftLastAccessTime);
	BOOL  WINAPI SevenZipGetArcFileSizeEx(HARC _harc, ULHA_INT64 *_lpllSize);
	BOOL  WINAPI SevenZipGetArcOriginalSizeEx(HARC _harc, ULHA_INT64 *_lpllSize);
	BOOL  WINAPI SevenZipGetArcCompressedSizeEx(HARC _harc, ULHA_INT64 *_lpllSize);
	BOOL  WINAPI SevenZipGetOriginalSizeEx(HARC _harc, ULHA_INT64 *_lpllSize);
	BOOL  WINAPI SevenZipGetCompressedSizeEx(HARC _harc, ULHA_INT64 *_lpllSize);

	/* SetOwnerWindow�nAPI */	
	BOOL WINAPI SevenZipSetOwnerWindow(HWND _hwnd);
	BOOL WINAPI SevenZipClearOwnerWindow();
	BOOL WINAPI SevenZipSetOwnerWindowEx(HWND _hwnd, LPARCHIVERPROC _lpArcProc);
	BOOL WINAPI SevenZipKillOwnerWindowEx(HWND _hwnd);
	BOOL WINAPI SevenZipSetOwnerWindowEx64(HWND _hwnd, LPARCHIVERPROC _lpArcProc, DWORD _dwStructSize);
	BOOL WINAPI SevenZipKillOwnerWindowEx64(HWND _hwnd);

	/* �����A�[�J�C�o�Ǝ�API */
	WORD WINAPI SevenZipGetSubVersion();
	int  WINAPI SevenZipGetArchiveType(LPCSTR _szFileName);
	BOOL WINAPI SevenZipSetUnicodeMode(BOOL _bUnicode);
	BOOL WINAPI SevenZipSetPriority(const int _nPriority);
	BOOL WINAPI SevenZipSetCP(const UINT _uCodePage);	// �ǉ�
	UINT WINAPI SevenZipGetCP();						// �ǉ�
	int WINAPI SevenZipGetLastError(LPDWORD  _lpdwSystemError);
	int WINAPI SevenZipSetDefaultPassword(HARC _harc, LPCSTR _szPassword);
	DWORD WINAPI SevenZipGetDefaultPassword(HARC _harc, LPSTR _szPassword, DWORD _dwSize);
	int WINAPI SevenZipPasswordDialog(HWND _hwnd, LPSTR _szBuffer, DWORD _dwSize);

	/* 7-zip32.dll�Ǝ�API */
	BOOL WINAPI SevenZipSfxFileStoring(LPCSTR _szFileName);
	int WINAPI SevenZipSfxConfigDialog(HWND _hwnd, LPSTR _szBuffer, DWORD _dwSize);

	/* �ǉ��������� */
#ifdef Z7_EXTERNAL_CODECS
	/* 7z.dll�Ή���7-zip32.dll�Ǝ�API */
	BOOL WINAPI SevenZipExists7zdll();
#endif
	/* �ǉ������܂� */

#ifdef __cplusplus
}
#endif

// API��������l�Ȑ��l
#if !defined(ISARC_FUNCTION_START)
#define ISARC_FUNCTION_START				0
#define ISARC								0	/* SevenZip */
#define ISARC_GET_VERSION					1	/* SevenZipGetVersion */
#define ISARC_GET_CURSOR_INTERVAL			2	/* SevenZipGetCursorInterval */
#define ISARC_SET_CURSOR_INTERVAL			3	/* SevenZipSetCursorInterval */
#define ISARC_GET_BACK_GROUND_MODE			4	/* SevenZipGetBackGroundMode */
#define ISARC_SET_BACK_GROUND_MODE			5	/* SevenZipSetBackGroundMode */
#define ISARC_GET_CURSOR_MODE				6	/* SevenZipGetCursorMode */
#define ISARC_SET_CURSOR_MODE				7	/* SevenZipSetCursorMode */
#define ISARC_GET_RUNNING					8	/* SevenZipGetRunning */
#define ISARC_EXISTS_7ZDLL					9	/* SevenZipExists7zdll */	// �ǉ�

#define ISARC_CHECK_ARCHIVE					16	/* SevenZipCheckArchive */
#define ISARC_CONFIG_DIALOG					17	/* SevenZipConfigDialog */
#define ISARC_GET_FILE_COUNT				18	/* SevenZipGetFileCount */
#define ISARC_QUERY_FUNCTION_LIST			19	/* SevenZipQueryFunctionList */
#define ISARC_HOUT							20	/* SevenZipHOut */
#define ISARC_STRUCTOUT						21	/* SevenZipStructOut */
#define ISARC_GET_ARC_FILE_INFO				22	/* SevenZipGetArcFileInfo */

#define ISARC_OPEN_ARCHIVE					23	/* SevenZipOpenArchive */
#define ISARC_CLOSE_ARCHIVE					24	/* SevenZipCloseArchive */
#define ISARC_FIND_FIRST					25	/* SevenZipFindFirst */
#define ISARC_FIND_NEXT						26	/* SevenZipFindNext */
#define ISARC_EXTRACT						27	/* SevenZipExtract */
#define ISARC_ADD							28	/* SevenZipAdd */
#define ISARC_MOVE							29	/* SevenZipMove */
#define ISARC_DELETE						30	/* SevenZipDelete */
#define ISARC_SETOWNERWINDOW				31	/* SevenZipSetOwnerWindow */
#define ISARC_CLEAROWNERWINDOW				32	/* SevenZipClearOwnerWindow */
#define ISARC_SETOWNERWINDOWEX				33	/* SevenZipSetOwnerWindowEx */
#define ISARC_KILLOWNERWINDOWEX				34	/* SevenZipKillOwnerWindowEx */

#define ISARC_GET_ARC_FILE_NAME				40	/* SevenZipGetArcFileName */
#define ISARC_GET_ARC_FILE_SIZE				41	/* SevenZipGetArcFileSize */
#define ISARC_GET_ARC_ORIGINAL_SIZE			42	/* SevenZipArcOriginalSize */
#define ISARC_GET_ARC_COMPRESSED_SIZE		43	/* SevenZipGetArcCompressedSize */
#define ISARC_GET_ARC_RATIO					44	/* SevenZipGetArcRatio */
#define ISARC_GET_ARC_DATE					45	/* SevenZipGetArcDate */
#define ISARC_GET_ARC_TIME					46	/* SevenZipGetArcTime */
#define ISARC_GET_ARC_OS_TYPE				47	/* SevenZipGetArcOSType */
#define ISARC_GET_ARC_IS_SFX_FILE			48	/* SevenZipGetArcIsSFXFile */
#define ISARC_GET_ARC_WRITE_TIME_EX			49	/* SevenZipGetArcWriteTimeEx */
#define ISARC_GET_ARC_CREATE_TIME_EX		50	/* SevenZipGetArcCreateTimeEx */
#define	ISARC_GET_ARC_ACCESS_TIME_EX		51	/* SevenZipGetArcAccessTimeEx */
#define	ISARC_GET_ARC_CREATE_TIME_EX2		52	/* SevenZipGetArcCreateTimeEx2 */
#define ISARC_GET_ARC_WRITE_TIME_EX2		53	/* SevenZipGetArcWriteTimeEx2 */
#define ISARC_GET_FILE_NAME					57	/* SevenZipGetFileName */
#define ISARC_GET_ORIGINAL_SIZE				58	/* SevenZipGetOriginalSize */
#define ISARC_GET_COMPRESSED_SIZE			59	/* SevenZipGetCompressedSize */
#define ISARC_GET_RATIO						60	/* SevenZipGetRatio */
#define ISARC_GET_DATE						61	/* SevenZipGetDate */
#define ISARC_GET_TIME						62	/* SevenZipGetTime */
#define ISARC_GET_CRC						63	/* SevenZipGetCRC */
#define ISARC_GET_ATTRIBUTE					64	/* SevenZipGetAttribute */
#define ISARC_GET_OS_TYPE					65	/* SevenZipGetOSType */
#define ISARC_GET_METHOD					66	/* SevenZipGetMethod */
#define ISARC_GET_WRITE_TIME				67	/* SevenZipGetWriteTime */
#define ISARC_GET_CREATE_TIME				68	/* SevenZipGetCreateTime */
#define ISARC_GET_ACCESS_TIME				69	/* SevenZipGetAccessTime */
#define ISARC_GET_WRITE_TIME_EX				70	/* SevenZipGetWriteTimeEx */
#define ISARC_GET_CREATE_TIME_EX			71	/* SevenZipGetCreateTimeEx */
#define ISARC_GET_ACCESS_TIME_EX			72	/* SevenZipGetAccessTimeEx */
#define ISARC_SET_ENUM_MEMBERS_PROC			80  /* SevenZipSetEnumMembersProc */
#define ISARC_CLEAR_ENUM_MEMBERS_PROC		81	/* SevenZipClearEnumMembersProc */
#define ISARC_GET_ARC_FILE_SIZE_EX			82	/* SevenZipGetArcFileSizeEx */
#define ISARC_GET_ARC_ORIGINAL_SIZE_EX		83	/* SevenZipArcOriginalSizeEx */
#define ISARC_GET_ARC_COMPRESSED_SIZE_EX	84	/* SevenZipGetArcCompressedSizeEx */
#define ISARC_GET_ORIGINAL_SIZE_EX			85	/* SevenZipGetOriginalSizeEx */
#define ISARC_GET_COMPRESSED_SIZE_EX		86	/* SevenZipGetCompressedSizeEx */
#define ISARC_SETOWNERWINDOWEX64			87	/* SevenZipSetOwnerWindowEx64 */
#define ISARC_KILLOWNERWINDOWEX64			88	/* SevenZipKillOwnerWindowEx64 */
#define ISARC_SET_ENUM_MEMBERS_PROC64		89  /* SevenZipSetEnumMembersProc64 */
#define ISARC_CLEAR_ENUM_MEMBERS_PROC64		90	/* SevenZipClearEnumMembersProc64 */
#define ISARC_OPEN_ARCHIVE2					91	/* SevenZipOpenArchive2 */
#define ISARC_GET_ARC_READ_SIZE				92	/* SevenZipGetArcReadSize */
#define ISARC_GET_ARC_READ_SIZE_EX			93	/* SevenZipGetArcReadSizeEx*/

#define ISARC_SET_PRIORITY					100	/* SevenZipSetPriority */
#define ISARC_SET_CP						102	/* SevenZipSetCP */
#define ISARC_GET_CP						103	/* SevenZipGetCP */
#define ISARC_GET_LAST_ERROR				104	/* SevenZipGetLasrError */
#define ISARC_SET_UNICODE_MODE				114 /* SevenZipSetUnicodeMode */

#define ISARC_SET_DEFAULT_PASSWORD			178 /* SevenZipSetDefaultPassword */
#define ISARC_GET_DEFAULT_PASSWORD			179 /* SevenZipGetDefaultPassword */
#define ISARC_PASSWORD_DIALOG				180 /* SevenZipPasswordDialog */
#define ISARC_SFX_CONFIG_DIALOG				210 /* SevenZipSfxConfigDialog */
#define ISARC_SFX_FILE_STORING				211 /* SevenZipSfxFileStoring */

#define ISARC_FUNCTION_END					ISARC_SFX_FILE_STORING

#endif	/* ISARC_FUNCTION_START */

// �G���[���b�Z�[�W
#if !defined(ERROR_START)
#define ERROR_START				0x8000
/* WARNING */
#define ERROR_DISK_SPACE		0x8005
#define ERROR_READ_ONLY			0x8006
#define ERROR_USER_SKIP			0x8007
#define ERROR_UNKNOWN_TYPE		0x8008
#define ERROR_METHOD			0x8009
#define ERROR_PASSWORD_FILE		0x800A
#define ERROR_VERSION			0x800B
#define ERROR_FILE_CRC			0x800C
#define ERROR_FILE_OPEN			0x800D
#define ERROR_MORE_FRESH		0x800E
#define ERROR_NOT_EXIST			0x800F
#define ERROR_ALREADY_EXIST		0x8010

#define ERROR_TOO_MANY_FILES	0x8011

/* ERROR */
#define ERROR_MAKEDIRECTORY		0x8012
#define ERROR_CANNOT_WRITE		0x8013
#define ERROR_HUFFMAN_CODE		0x8014
#define ERROR_COMMENT_HEADER	0x8015
#define ERROR_HEADER_CRC		0x8016
#define ERROR_HEADER_BROKEN		0x8017
#define ERROR_ARC_FILE_OPEN		0x8018
#define ERROR_NOT_ARC_FILE		0x8019
#define ERROR_CANNOT_READ		0x801A
#define ERROR_FILE_STYLE		0x801B
#define ERROR_COMMAND_NAME		0x801C
#define ERROR_MORE_HEAP_MEMORY	0x801D
#define ERROR_ENOUGH_MEMORY		0x801E
#if !defined(ERROR_ALREADY_RUNNING)
#define ERROR_ALREADY_RUNNING	0x801F
#endif
#define ERROR_USER_CANCEL		0x8020
#define ERROR_HARC_ISNOT_OPENED	0x8021
#define ERROR_NOT_SEARCH_MODE	0x8022
#define ERROR_NOT_SUPPORT		0x8023
#define ERROR_TIME_STAMP		0x8024
#define ERROR_TMP_OPEN			0x8025
#define ERROR_LONG_FILE_NAME	0x8026
#define ERROR_ARC_READ_ONLY		0x8027
#define ERROR_SAME_NAME_FILE	0x8028
#define ERROR_NOT_FIND_ARC_FILE 0x8029
#define ERROR_RESPONSE_READ		0x802A
#define ERROR_NOT_FILENAME		0x802B
#define ERROR_TMP_COPY			0x802C
#define ERROR_EOF				0x802D
#define ERROR_ADD_TO_LARC		0x802E
#define ERROR_TMP_BACK_SPACE	0x802F
#define ERROR_SHARING			0x8030
#define ERROR_NOT_FIND_FILE		0x8031
#define ERROR_LOG_FILE			0x8032
#define	ERROR_NO_DEVICE			0x8033
#define ERROR_GET_ATTRIBUTES	0x8034
#define ERROR_SET_ATTRIBUTES	0x8035
#define ERROR_GET_INFORMATION	0x8036
#define ERROR_GET_POINT			0x8037
#define ERROR_SET_POINT			0x8038
#define ERROR_CONVERT_TIME		0x8039
#define ERROR_GET_TIME			0x803A
#define ERROR_SET_TIME			0x803B
#define ERROR_CLOSE_FILE		0x803C
#define ERROR_HEAP_MEMORY		0x803D
#define ERROR_HANDLE			0x803E
#define ERROR_TIME_STAMP_RANGE	0x803F
#define ERROR_MAKE_ARCHIVE		0x8040
#define ERROR_NOT_CONFIRM_NAME	0x8041
#define ERROR_UNEXPECTED_EOF	0x8042
#define ERROR_INVALID_END_MARK	0x8043
#define ERROR_INVOLVED_LZH		0x8044
#define ERROR_NO_END_MARK		0x8045
#define ERROR_HDR_INVALID_SIZE	0x8046
#define ERROR_UNKNOWN_LEVEL		0x8047
#define ERROR_BROKEN_DATA		0x8048
#define ERROR_INVALID_PATH		0x8049
#define ERROR_TOO_BIG			0x804A
#define ERROR_EXECUTABLE_FILE	0x804B
#define ERROR_INVALID_VALUE		0x804C
#define ERROR_HDR_EXPLOIT		0x804D
#define ERROR_HDR_NO_CRC		0x804E
#define ERROR_HDR_NO_NAME		0x804F

#define ERROR_END	ERROR_HDR_NO_NAME
#endif /* ERROR_START */

/* OpenArchive �t���O */
#if !defined(EXTRACT_FOUND_FILE)
#define M_INIT_FILE_USE			0x00000001L
#define M_REGARDLESS_INIT_FILE	0x00000002L
#define M_NO_BACKGROUND_MODE	0x00000004L
#define M_NOT_USE_TIME_STAMP	0x00000008L
#define M_EXTRACT_REPLACE_FILE	0x00000010L
#define M_EXTRACT_NEW_FILE		0x00000020L
#define M_EXTRACT_UPDATE_FILE	0x00000040L
#define M_CHECK_ALL_PATH		0x00000100L
#define M_CHECK_FILENAME_ONLY	0x00000200L
#define M_CHECK_DISK_SIZE		0x00000400L
#define M_REGARDLESS_DISK_SIZE	0x00000800L
#define M_USE_DRIVE_LETTER		0x00001000L
#define M_NOT_USE_DRIVE_LETTER	0x00002000L
#define M_INQUIRE_DIRECTORY		0x00004000L
#define M_NOT_INQUIRE_DIRECTORY 0x00008000L
#define M_INQUIRE_WRITE			0x00010000L
#define M_NOT_INQUIRE_WRITE		0x00020000L
#define M_CHECK_READONLY		0x00040000L
#define M_REGARDLESS_READONLY	0x00080000L
#define M_REGARD_E_COMMAND		0x00100000L
#define M_REGARD_X_COMMAND		0x00200000L
#define M_ERROR_MESSAGE_ON		0x00400000L
#define M_ERROR_MESSAGE_OFF		0x00800000L
#define M_BAR_WINDOW_ON			0x01000000L
#define M_BAR_WINDOW_OFF		0x02000000L
#define M_CHECK_PATH			0x04000000L
#define M_RECOVERY_ON			0x08000000L
#define M_MAKE_INDEX_FILE		0x10000000L
#define M_NOT_MAKE_INDEX_FILE	0x20000000L
#define EXTRACT_FOUND_FILE		0x40000000L
#define EXTRACT_NAMED_FILE		0x80000000L
#endif

// �t�@�C������
#ifndef FA_RDONLY
#define FA_RDONLY		0x01			/* �ǂݎ���p */
#define FA_HIDDEN		0x02			/* �B���t�@�C�� */
#define FA_SYSTEM		0x04			/* �V�X�e���t�@�C�� */
#define FA_LABEL		0x08			/* �{�����[�����x�� */
#define FA_DIREC		0x10			/* �f�B���N�g�� */
#define FA_ARCH 		0x20			/* �A�[�J�C�u */
#define FA_ENCRYPTED	0x40			/* �p�X���[�h�ی� */
#endif /* FA_RDONLY */

// SevenZipCheckArchive �p�t���O
#if !defined(CHECKARCHIVE_RAPID)
#define	CHECKARCHIVE_RAPID		0	/* �Ȉ�(�w�b�_�Í�������) */
#define	CHECKARCHIVE_BASIC		1	/* �W��(�S�Ẵw�b�_) */
#define	CHECKARCHIVE_FULLCRC	2	/* ���S(�i�[�t�@�C���� CRC �`�F�b�N) */

/* �ȉ��̃t���O�͏�L�Ƒg�ݍ��킹�Ďg�p�B(7-Zip32.DLL�͖��T�|�[�g)*/
#define CHECKARCHIVE_RECOVERY	4   /* �j���w�b�_��ǂݔ�΂��ď��� */
#define CHECKARCHIVE_SFX		8	/* SFX ���ǂ�����Ԃ� */
#define CHECKARCHIVE_ALL		16	/* �t�@�C���̍Ō�܂Ō������� */
#define CHECKARCHIVE_ENDDATA	32	/* ���ɂ����̗]��f�[�^������ */
#endif

// 7zip32�Ǝ��G���[���b�Z�[�W
#if !defined(ERROR_7ZIP_START)
#define ERROR_7ZIP_START						0x8100

#define ERROR_WARNING							0x8101
#define ERROR_FATAL								0x8102
#define ERROR_DURING_DECOMPRESSION				0x8103
#define ERROR_DIR_FILE_WITH_64BIT_SIZE			0x8104
#define ERROR_FILE_CHANGED_DURING_OPERATION		0x8105
#define ERROR_FILE_CREATE						0x8106

#define ERROR_7ZIP_END	ERROR_FILE_CREATE
#endif /* ERROR_7ZIP_START */

// ���Ɍ`��
#ifndef ARCHIVETYPE_ZIP
#define ARCHIVETYPE_ZIP		1
#define ARCHIVETYPE_7Z		2
/* �ǉ��������� */
#define ARCHIVETYPE_APM		3
#define ARCHIVETYPE_AR		4
#define ARCHIVETYPE_ARJ		5
#define ARCHIVETYPE_BZIP2	6
#define ARCHIVETYPE_COMPOUND	7
#define ARCHIVETYPE_CPIO	8
#define ARCHIVETYPE_CRAMFS	9
#define ARCHIVETYPE_DMG		10
#define ARCHIVETYPE_ELF		11
#define ARCHIVETYPE_EXT		12
#define ARCHIVETYPE_FAT		13
#define ARCHIVETYPE_FLV		14
#define ARCHIVETYPE_GZIP	15
#define ARCHIVETYPE_GPT		16
#define ARCHIVETYPE_HFS		17
#define ARCHIVETYPE_IHEX	18
#define ARCHIVETYPE_LZH		19
#define ARCHIVETYPE_LZMA	20
#define ARCHIVETYPE_LZMA86	21
#define ARCHIVETYPE_MACHO	22
#define ARCHIVETYPE_MBR		23
#define ARCHIVETYPE_MSLZ	24
#define ARCHIVETYPE_MUB		25
#define ARCHIVETYPE_NTFS	26
#define ARCHIVETYPE_PE		27
#define ARCHIVETYPE_COFF	28
#define ARCHIVETYPE_TE		29
#define ARCHIVETYPE_PPMD	30
#define ARCHIVETYPE_QCOW	31
#define ARCHIVETYPE_RPM		32
#define ARCHIVETYPE_SPLIT	33
#define ARCHIVETYPE_SQUASHFS	34
#define ARCHIVETYPE_SWFC	35
#define ARCHIVETYPE_SWF		36
#define ARCHIVETYPE_UEFIC	37
#define ARCHIVETYPE_UEFIF	38
#define ARCHIVETYPE_VDI		39
#define ARCHIVETYPE_VHD		40
#define ARCHIVETYPE_VMDK	41
#define ARCHIVETYPE_XAR		42
#define ARCHIVETYPE_XZ		43
#define ARCHIVETYPE_Z		44
#define ARCHIVETYPE_CAB		45
#define ARCHIVETYPE_CHM		46
#define ARCHIVETYPE_HXS		47
#define ARCHIVETYPE_ISO		48
#define ARCHIVETYPE_NSIS	49
#define ARCHIVETYPE_RAR		50
#define ARCHIVETYPE_RAR5	51
#define ARCHIVETYPE_TAR		52
#define ARCHIVETYPE_UDF		53
#define ARCHIVETYPE_WIM		54
/* �ǉ������܂� */
#endif /* ARCHIVETYPE_ZIP */

#endif	/* SEVENZIP_H */
