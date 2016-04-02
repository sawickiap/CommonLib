/** \file
\brief Obs³uga plików i systemu plików
\author Adam Sawicki - sawickiap@poczta.onet.pl - http://regedit.gamedev.pl/ \n

Czêœæ biblioteki CommonLib \n
Kodowanie Windows-1250, koniec wiersza CR+LF, test: Za¿ó³æ gêœl¹ jaŸñ \n
Licencja: GNU LGPL. \n
Dokumentacja: \ref Module_Files \n
Elementy modu³u: \ref code_files
*/
#include "Base.hpp"

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h> // dla unlink [Win, Linux ma go w unistd.h], rename [obydwa]
#ifdef WIN32
	extern "C" {
		#include <sys/utime.h> // dla utime
		#include <direct.h> // dla mkdir (Linux ma go w sys/stat.h + sys/types.h)
	}
	#include <windows.h>
#else
	extern "C" {
		#include <sys/time.h> // dla utime
		#include <sys/file.h> // dla flock
		#include <dirent.h>
		#include <utime.h> // dla utime
	}
#endif
#include <stack>

#include "Error.hpp"
#include "Files.hpp"
#include "DateTime.hpp"


namespace common
{

//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Klasa FileStream

#ifdef WIN32

	class File_pimpl
	{
	public:
		HANDLE m_File;
	};

	FileStream::FileStream(const tstring &FileName, FILE_MODE FileMode, bool Lock) :
		pimpl(new File_pimpl)
	{
		uint32 DesiredAccess, ShareMode, CreationDisposition;
		switch (FileMode)
		{
		case FM_WRITE:
			DesiredAccess = GENERIC_WRITE;
			ShareMode = 0;
			CreationDisposition = CREATE_ALWAYS;
			break;
		case FM_WRITE_PLUS:
			DesiredAccess = GENERIC_WRITE | GENERIC_READ;
			ShareMode = 0;
			CreationDisposition = CREATE_ALWAYS;
			break;
		case FM_READ:
			DesiredAccess = GENERIC_READ;
			ShareMode = FILE_SHARE_READ;
			CreationDisposition = OPEN_EXISTING;
			break;
		case FM_READ_PLUS:
			DesiredAccess = GENERIC_WRITE | GENERIC_READ;
			ShareMode = 0;
			CreationDisposition = OPEN_EXISTING;
			break;
		case FM_APPEND:
			DesiredAccess = GENERIC_WRITE;
			ShareMode = 0;
			CreationDisposition = OPEN_ALWAYS;
			break;
		case FM_APPEND_PLUS:
			DesiredAccess = GENERIC_WRITE | GENERIC_READ;
			ShareMode = 0;
			CreationDisposition = OPEN_ALWAYS;
			break;
		}
		if (!Lock)
			ShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;

		pimpl->m_File = CreateFile(FileName.c_str(), DesiredAccess, ShareMode, 0, CreationDisposition, FILE_ATTRIBUTE_NORMAL, 0);
		if (pimpl->m_File == INVALID_HANDLE_VALUE)
			throw Win32Error(_T("Cannot open file: ") + FileName, __TFILE__, __LINE__);

		if (FileMode == FM_APPEND || FileMode == FM_APPEND_PLUS)
			SetPosFromEnd(0);
	}

	FileStream::~FileStream()
	{
		CloseHandle(pimpl->m_File);
	}

	void FileStream::Write(const void *Data, size_t Size)
	{
		uint32 WrittenSize;
		if (WriteFile(pimpl->m_File, Data, Size, (LPDWORD)&WrittenSize, 0) == 0)
			throw Win32Error(Format(_T("Cannot write # bytes to file.")) % Size, __TFILE__, __LINE__);
		if (WrittenSize != Size)
			throw Error(Format(_T("Cannot write to file. #/# bytes written.")) % WrittenSize % Size, __TFILE__, __LINE__);
	}

	size_t FileStream::Read(void *Data, size_t Size)
	{
		uint32 ReadSize;
		if (ReadFile(pimpl->m_File, Data, Size, (LPDWORD)&ReadSize, 0) == 0)
			throw Win32Error(Format(_T("Cannot read # bytes from file.")) % Size, __TFILE__, __LINE__);
		return ReadSize;
	}

	void FileStream::Flush()
	{
		FlushFileBuffers(pimpl->m_File);
	}

	size_t FileStream::GetSize()
	{
		return (size_t)GetFileSize(pimpl->m_File, 0);
	}

	int FileStream::GetPos()
	{
		// Fuj! Ale to nieeleganckie. Szkoda, ¿e nie ma lepszego sposobu.
		uint32 r = SetFilePointer(pimpl->m_File, 0, 0, FILE_CURRENT);
		if (r == INVALID_SET_FILE_POINTER)
			throw Win32Error(_T("Cannot read position in file stream."), __TFILE__, __LINE__);
		return (int)r;
	}

	void FileStream::SetPos(int pos)
	{
		if (SetFilePointer(pimpl->m_File, pos, 0, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
			throw Win32Error(Format(_T("Cannot set position in file stream to # from the beginning.")) % pos, __TFILE__, __LINE__);
	}

	void FileStream::SetPosFromCurrent(int pos)
	{
		if (SetFilePointer(pimpl->m_File, pos, 0, FILE_CURRENT) == INVALID_SET_FILE_POINTER)
			throw Win32Error(Format(_T("Cannot set position in file stream to # from current.")) % pos, __TFILE__, __LINE__);
	}

	void FileStream::SetPosFromEnd(int pos)
	{
		if (SetFilePointer(pimpl->m_File, pos, 0, FILE_END) == INVALID_SET_FILE_POINTER)
			throw Win32Error(Format(_T("Cannot set position in file stream to # from the end.")) % pos, __TFILE__, __LINE__);
	}

	void FileStream::SetSize(size_t Size)
	{
		int SavedPos = GetPos();
		
		SetPos((int)Size);
		Truncate();

		SetPos(SavedPos);
	}

	void FileStream::Truncate()
	{
		if (SetEndOfFile(pimpl->m_File) == 0)
			throw Win32Error(_T("Cannot truncate file."), __TFILE__, __LINE__);
	}

	bool FileStream::End()
	{
		// Fuj! Ale to nieeleganckie. Szkoda, ¿e nie ma lepszego sposobu.
		return ((int)GetSize() == GetPos());
	}

	HANDLE FileStream::GetNativeHandle()
	{
		return pimpl->m_File;
	}

#else

	class File_pimpl
	{
	public:
		FILE *m_File;
		bool m_Lock;
	};

	FileStream::FileStream(const tstring &FileName, FILE_MODE FileMode, bool Lock) :
		pimpl(new File_pimpl)
	{
		pimpl->m_Lock = Lock;

		const char *m = 0;

		switch (FileMode)
		{
		case FM_WRITE:
			m = "wb";
			break;
		case FM_WRITE_PLUS:
			m = "w+b";
			break;
		case FM_READ:
			m = "rb";
			break;
		case FM_READ_PLUS:
			m = "r+b";
			break;
		case FM_APPEND:
			m = "ab";
			break;
		case FM_APPEND_PLUS:
			m = "a+b";
			break;
		}

		pimpl->m_File = fopen(FileName.c_str(), m);
		if (pimpl->m_File == 0)
			throw ErrnoError(Format(_T("Cannot open file \"#\" in mode \"#\".")) % FileName % m, __TFILE__, __LINE__);
		if (Lock)
		{
			if (flock(fileno(pimpl->m_File), LOCK_EX | LOCK_NB) != 0)
				throw ErrnoError(Format(_T("Cannot open file \"#\" in mode \"#\" - error while locking.")) % FileName % m, __TFILE__, __LINE__);
		}
	}

	FileStream::~FileStream()
	{
		if (pimpl->m_File != 0)
		{
			// Kiedy deskryptor int zostaje zamkniêty, podobno blokada sama siê zwalania. Ale kto tam tego Linuksa wie... :)
			if (pimpl->m_Lock)
				flock(fileno(pimpl->m_File), LOCK_UN);
			fclose(pimpl->m_File);
		}
	}

	void FileStream::Write(const void *Data, size_t Size)
	{
		size_t Written = fwrite(Data, 1, Size, pimpl->m_File);
		if (Written != Size || ferror(pimpl->m_File) != 0)
			throw Error(Format(_T("Cannot write to file. #/# bytes written.")) % Written % Size, __TFILE__, __LINE__);
	}

	size_t FileStream::Read(void *Data, size_t Size)
	{
		size_t BytesRead = fread(Data, 1, Size, pimpl->m_File);
		if (ferror(pimpl->m_File) != 0)
			throw Error(Format(_T("Cannot read from file. #/# bytes read.")) % BytesRead % Size, __TFILE__, __LINE__);
		return BytesRead;
	}

	void FileStream::Flush()
	{
		fflush(pimpl->m_File);
	}

	size_t FileStream::GetSize()
	{
		// Wersja 1
		int SavedPos = GetPos();
		SetPosFromEnd(0);
		int EndPos = GetPos();
		SetPos(SavedPos);
		return (size_t)EndPos;

		// Ta wersja jest z³a bo mo¿e nie pokazywaæ aktualnego rozmiaru, bo jest bufor
/*		// Wersja 2
		struct stat s;
		fstat(fileno(pimpl->m_File), &s);
		return s.st_size;*/
	}

	int FileStream::GetPos()
	{
		int r = ftell(pimpl->m_File);
		if (r < 0)
			throw ErrnoError(_T("Cannot get current position in file."), __TFILE__, __LINE__);
		return r;
	}

	void FileStream::SetPos(int pos)
	{
		int r = fseek(pimpl->m_File, pos, SEEK_SET);
		if (r < 0)
			throw ErrnoError(Format(_T("Cannot set position in file stream to # from the beginning.")) % pos, __TFILE__, __LINE__);
	}

	void FileStream::SetPosFromCurrent(int pos)
	{
		int r = fseek(pimpl->m_File, pos, SEEK_CUR);
		if (r < 0)
			throw ErrnoError(Format(_T("Cannot set position in file stream to # from current.")) % pos, __TFILE__, __LINE__);
	}

	void FileStream::SetPosFromEnd(int pos)
	{
		int r = fseek(pimpl->m_File, pos, SEEK_END);
		if (r < 0)
			throw ErrnoError(Format(_T("Cannot set position in file stream to # from the end.")) % pos, __TFILE__, __LINE__);
	}

	void FileStream::SetSize(size_t Size)
	{
		ftruncate(fileno(pimpl->m_File), Size);
	}

	void FileStream::Truncate()
	{
		SeekableStream::Truncate();
	}

	bool FileStream::End()
	{
		// Fuj! Ale to nieeleganckie. Szkoda, ¿e nie ma lepszego sposobu.
		// UWAGA! Jak by³o zamienione miejscami, najpierw wyliczane GetSize
		// a potem GetPos to za pierwszym wyliczaniem wychodzi³o false a dopiero
		// za drugim true - LOL!
		return (GetPos() == (int)GetSize());
	}

#endif


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Klasa DirLister

class DirLister_pimpl
{
private:
	tstring m_Dir;

	#ifdef WIN32
		WIN32_FIND_DATA m_FindData;
		HANDLE m_Handle;
		bool m_ReachedEnd;

		void GetFirst();
		void GetNext();

	#else
		DIR *m_DirHandle;

	#endif

public:
	DirLister_pimpl(const tstring &Dir);
	~DirLister_pimpl();

	bool ReadNext(tstring *OutName, FILE_ITEM_TYPE *OutType);
};

#ifdef WIN32
	void DirLister_pimpl::GetFirst()
	{
		tstring Path;
		IncludeTrailingPathDelimiter(&Path, m_Dir);
		Path += _T('*');
		m_Handle = FindFirstFile(Path.c_str(), &m_FindData);

		if (m_Handle == INVALID_HANDLE_VALUE)
		{
			if (GetLastError() == ERROR_NO_MORE_FILES)
				m_ReachedEnd = true;
			else
				throw Win32Error(_T("Cannot start directory listing: ") + m_Dir, __TFILE__, __LINE__);
		}
		else
			m_ReachedEnd = false;

		while (!m_ReachedEnd && (m_FindData.cFileName == tstring(_T(".")) || m_FindData.cFileName == tstring(_T(".."))))
			GetNext();
	}

	void DirLister_pimpl::GetNext()
	{
		if (FindNextFile(m_Handle, &m_FindData) == 0)
		{
			if (GetLastError() == ERROR_NO_MORE_FILES)
				m_ReachedEnd = true;
			else
				throw Win32Error(_T("Cannot continue directory listing: ") + m_Dir, __TFILE__, __LINE__);
		}

		while (!m_ReachedEnd && (m_FindData.cFileName == tstring(_T(".")) || m_FindData.cFileName == tstring(_T(".."))))
			GetNext();
	}

	DirLister_pimpl::DirLister_pimpl(const tstring &Dir) :
		m_Dir(Dir)
	{
		GetFirst();
	}

	DirLister_pimpl::~DirLister_pimpl()
	{
		if (m_Handle != INVALID_HANDLE_VALUE)
			FindClose(m_Handle);
	}

	bool DirLister_pimpl::ReadNext(tstring *OutName, FILE_ITEM_TYPE *OutType)
	{
		if (m_ReachedEnd)
			return false;

		*OutName = m_FindData.cFileName;
		*OutType = ( ((m_FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) ? IT_DIR : IT_FILE );

		GetNext();

		return true;
	}

#else
	DirLister_pimpl::DirLister_pimpl(const tstring &Dir) :
		m_Dir(Dir)
	{
		m_DirHandle = opendir(Dir.c_str());
		if (m_DirHandle == NULL)
			throw ErrnoError(_T("Cannot start directory listing: ") + m_Dir, __TFILE__, __LINE__);
	}

	DirLister_pimpl::~DirLister_pimpl()
	{
		if (m_DirHandle != NULL)
			closedir(m_DirHandle);
	}

	bool DirLister_pimpl::ReadNext(tstring *OutName, FILE_ITEM_TYPE *OutType)
	{
		dirent DirEnt, *DirEntPtr;
		if ( readdir_r(m_DirHandle, &DirEnt, &DirEntPtr) != 0 )
			throw ErrnoError(_T("Cannot continue directory listing: ") + m_Dir, __TFILE__, __LINE__);
		if (DirEntPtr == NULL)
			return false;
		if (strcmp(DirEnt.d_name, _T(".")) == 0 || strcmp(DirEnt.d_name, _T("..")) == 0)
			return ReadNext(OutName, OutType);
		*OutName = DirEnt.d_name;
		*OutType = ( (DirEnt.d_type == DT_DIR) ? IT_DIR : IT_FILE );
		return true;
	}

#endif

DirLister::DirLister(const tstring &Dir) :
	pimpl(new DirLister_pimpl(Dir))
{
}

DirLister::~DirLister()
{
}

bool DirLister::ReadNext(tstring *OutName, FILE_ITEM_TYPE *OutType)
{
	return pimpl->ReadNext(OutName, OutType);
}


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Funkcje globalne

void SaveStringToFile(const tstring &FileName, const string &Data)
{
	try
	{
		FileStream f(FileName, FM_WRITE);
		f.WriteStringF(Data);
	}
	catch (Error &e)
	{
		e.Push(Format(_T("Cannot write string to file: \"#\"")) % FileName, __TFILE__, __LINE__);
		throw;
	}
}

void SaveDataToFile(const tstring &FileName, const void *Data, size_t NumBytes)
{
	try
	{
		FileStream f(FileName, FM_WRITE);
		f.Write(Data, NumBytes);
	}
	catch (Error &e)
	{
		e.Push(Format(_T("Cannot write binary data to file: \"#\"")) % FileName, __TFILE__, __LINE__);
		throw;
	}
}

void LoadStringFromFile(const tstring &FileName, string *Data)
{
	try
	{
		FileStream f(FileName, FM_READ);
		f.ReadStringToEnd(Data);
	}
	catch (Error &e)
	{
		e.Push(Format(_T("Cannot read string from file: \"#\"")) % FileName, __TFILE__, __LINE__);
		throw;
	}
}

#ifdef WIN32

void SaveStringToFile(const tstring &FileName, const wstring &Data)
{
	try
	{
		FileStream f(FileName, FM_WRITE);
		f.WriteStringF(Data);
	}
	catch (Error &e)
	{
		e.Push(Format(_T("Cannot write string to file: \"#\"")) % FileName, __TFILE__, __LINE__);
		throw;
	}
}

void LoadStringFromFile(const tstring &FileName, wstring *Data)
{
	try
	{
		FileStream f(FileName, FM_READ);
		f.ReadStringToEnd(Data);
	}
	catch (Error &e)
	{
		e.Push(Format(_T("Cannot read string from file: \"#\"")) % FileName, __TFILE__, __LINE__);
		throw;
	}
}

#endif

bool GetFileItemInfo(const tstring &Path, FILE_ITEM_TYPE *OutType, uint *OutSize, DATETIME *OutModificationTime, DATETIME *OutCreationTime, DATETIME *OutAccessTime)
{
#ifdef WIN32
	struct _stat S;
	if ( _tstat(Path.c_str(), &S) != 0 )
		return false;

	if (OutType != NULL)
		*OutType = ( ((S.st_mode & S_IFDIR) != 0) ? IT_DIR : IT_FILE );
#else
	struct stat S;
	if ( stat(Path.c_str(), &S) != 0 )
		return false;

	if (OutType != NULL)
		*OutType = ( S_ISDIR(S.st_mode) ? IT_DIR : IT_FILE );
#endif

	if (OutSize != NULL)
		*OutSize = S.st_size;
	if (OutModificationTime != NULL)
		*OutModificationTime = DATETIME(S.st_mtime);
	if (OutCreationTime != NULL)
		*OutCreationTime = DATETIME(S.st_ctime);
	if (OutAccessTime != NULL)
		*OutAccessTime = DATETIME(S.st_atime);

	return true;
}

void MustGetFileItemInfo(const tstring &Path, FILE_ITEM_TYPE *OutType, uint *OutSize, DATETIME *OutModificationTime, DATETIME *OutCreationTime, DATETIME *OutAccessTime)
{
	if (!GetFileItemInfo(Path, OutType, OutSize, OutModificationTime, OutCreationTime, OutAccessTime))
		throw ErrnoError(_T("Cannot obtain information about: ") + Path, __TFILE__, __LINE__);
}

FILE_ITEM_TYPE GetFileItemType(const tstring &Path)
{
#ifdef WIN32
	// szukanie pliku
	DWORD Attr = GetFileAttributes(Path.c_str());
	// pliku nie ma
	if (Attr == INVALID_FILE_ATTRIBUTES)
		return IT_NONE;
	// to jest katalog
	if ((Attr & FILE_ATTRIBUTE_DIRECTORY) != 0)
		return IT_DIR;
	// nie - to jest plik
	else
		return IT_FILE;

#else
	struct stat s;
	if ( stat(Path.c_str(), &s) != 0 )
		return IT_NONE;
	if (S_ISDIR(s.st_mode))
		return IT_DIR;
	if (S_ISREG(s.st_mode))
		return IT_FILE;
	return IT_NONE;

#endif
}

bool UpdateFileTimeToNow(const tstring &FileName)
{
#ifdef WIN32
	return ( _tutime(FileName.c_str(), NULL) == 0 );
#else
	return ( utime(FileName.c_str(), NULL) == 0 );
#endif
}

void MustUpdateFileTimeToNow(const tstring &FileName)
{
	if (!UpdateFileTimeToNow(FileName))
		throw ErrnoError(_T("Cannot write file time to current for: ") + FileName, __TFILE__, __LINE__);
}

bool UpdateFileTime(const tstring &FileName, const DATETIME &ModificationTime, const DATETIME &AccessTime)
{
#ifdef WIN32
	_utimbuf B;
	B.modtime = ModificationTime.GetTicks();
	B.actime = AccessTime.GetTicks();
	return ( _tutime(FileName.c_str(), &B) == 0 );

#else
	utimbuf B;
	B.modtime = ModificationTime.GetTicks();
	B.actime = AccessTime.GetTicks();
	return ( utime(FileName.c_str(), &B) == 0 );

#endif
}

void MustUpdateFileTime(const tstring &FileName, const DATETIME &ModificationTime, const DATETIME &AccessTime)
{
	if (!UpdateFileTime(FileName, ModificationTime, AccessTime))
		throw ErrnoError(_T("Cannot write file time for: ") + FileName, __TFILE__, __LINE__);
}

bool CreateDirectory(const tstring &Path)
{
#ifdef WIN32
	return ( _tmkdir(Path.c_str()) == 0 );
#else
	return ( mkdir(Path.c_str(), 0777) == 0 );
#endif
}

void MustCreateDirectory(const tstring &Path)
{
	if (!CreateDirectory(Path))
		throw ErrnoError(_T("Cannot create directory: ") + Path, __TFILE__, __LINE__);
}

bool DeleteDirectory(const tstring &Path)
{
#ifdef WIN32
	return ( _trmdir(Path.c_str()) == 0 );
#else
	return ( rmdir(Path.c_str()) == 0 );
#endif
}

void MustDeleteDirectory(const tstring &Path)
{
	if (!DeleteDirectory(Path))
		throw ErrnoError(_T("Cannot delete directory: ") + Path, __TFILE__, __LINE__);
}

bool CreateDirectoryChain(const tstring &Path)
{
	if (GetFileItemType(Path) == IT_DIR)
		return true;

	size_t i2, i = Path.length()-1;
	std::stack<size_t> DirSepStack;
	for (;;)
	{
		i2 = Path.find_last_of(DIR_SEP, i-1);
		if (i2 == tstring::npos)
			break;
		if (GetFileItemType(Path.substr(0, i2)) == IT_DIR)
			break;
		DirSepStack.push(i2);
		i = i2;
	}

	while (!DirSepStack.empty())
	{
		if ( !CreateDirectory(Path.substr(0, DirSepStack.top())) )
			return false;
		DirSepStack.pop();
	}
	if ( !CreateDirectory(Path) )
		return false;
	return true;
}

void MustCreateDirectoryChain(const tstring &Path)
{
	if (GetFileItemType(Path) == IT_DIR)
		return;

	size_t i2, i = Path.length()-1;
	std::stack<size_t> DirSepStack;
	for (;;)
	{
		i2 = Path.find_last_of(DIR_SEP, i-1);
		if (i2 == tstring::npos)
			break;
		if (GetFileItemType(Path.substr(0, i2)) == IT_DIR)
			break;
		DirSepStack.push(i2);
		i = i2;
	}

	while (!DirSepStack.empty())
	{
		MustCreateDirectory(Path.substr(0, DirSepStack.top()));
		DirSepStack.pop();
	}
	MustCreateDirectory(Path);
}

bool DeleteFile(const tstring &FileName)
{
#ifdef WIN32
	return ( _tunlink(FileName.c_str()) == 0 );
#else
	return ( unlink(FileName.c_str()) == 0 );
#endif
}

void MustDeleteFile(const tstring &FileName)
{
	if (!DeleteFile(FileName))
		throw ErrnoError(_T("Cannot delete file: ") + FileName, __TFILE__, __LINE__);
}

bool MoveItem(const tstring &OldPath, const tstring &NewPath)
{
#ifdef WIN32
	return ( _trename(OldPath.c_str(), NewPath.c_str()) == 0 );
#else
	return ( rename(OldPath.c_str(), NewPath.c_str()) == 0 );
#endif
}

void MustMoveItem(const tstring &OldPath, const tstring &NewPath)
{
	if (!MoveItem(OldPath, NewPath))
		throw ErrnoError(_T("Cannot move element from \"") + OldPath + _T("\" to \"") + NewPath + _T("\""), __TFILE__, __LINE__);
}

#ifdef WIN32

void SaveUnicodeToFile(const tstring &FileName, const wstring &Data, unsigned Encoding)
{
	ERR_TRY;

	FileStream F(FileName, FM_WRITE);
	SaveUnicodeToStream(&F, Data, Encoding);

	ERR_CATCH(_T("Cannot save Unicode string to file: ") + FileName);
}

void SaveUnicodeToFile(const tstring &FileName, const wchar_t *Data, unsigned NumChars, unsigned Encoding)
{
	ERR_TRY;

	FileStream F(FileName, FM_WRITE);
	SaveUnicodeToStream(&F, Data, NumChars, Encoding);

	ERR_CATCH(_T("Cannot save Unicode characters to file: ") + FileName);
}

void SaveUnicodeToStream(Stream *Dest, const wstring &Data, unsigned Encoding)
{
	ERR_TRY;

	switch (Encoding & 0xFFFF)
	{
	case FILE_ENCODING_UTF16_LE:
		{
			// Do³¹cz nag³ówek, o ile nie wymuszono braku
			if ((Encoding & FILE_ENCODING_NO_BOM) == 0)
				Dest->Write(BOM_UTF16_LE, strlen(BOM_UTF16_LE));
			// Zapisz dane bezpoœrednio tak jak s¹ w pamiêci
			Dest->WriteStringF(Data);
		}
		break;
	case FILE_ENCODING_UTF8:
		{
			// Do³¹cz nag³ówek, jeœli wymuszono ¿e ma byæ
			if ((Encoding & FILE_ENCODING_FORCE_BOM) != 0)
				Dest->Write(BOM_UTF8, strlen(BOM_UTF8));
			// Skonwertuj Unicode na UTF8
			string utf8_string;
			if (!ConvertUnicodeToChars(&utf8_string, Data, CP_UTF8))
				throw Error(_T("Cannot convert Unicode to UTF-8."), __TFILE__, __LINE__);
			// Zapisz ³añcuch UTF-8
			Dest->WriteStringF(utf8_string);
		}
		break;
	case FILE_ENCODING_ANSI:
		{
			// Skonwertuj Unicode na ANSI
			string ansi_string;
			if (!ConvertUnicodeToChars(&ansi_string, Data, CP_ACP))
				throw Error(_T("Cannot convert Unicode to ANSI."), __TFILE__, __LINE__);
			// Zapisz ³añcuch ANSI
			Dest->WriteStringF(ansi_string);
		}
		break;
	default:
		assert(0 && "SaveUnicodeToStream: No encoding specified.");
	}

	ERR_CATCH(_T("Cannot save Unicode string to stream."));
}

void SaveUnicodeToStream(Stream *Dest, const wchar_t *Data, unsigned NumChars, unsigned Encoding)
{
	ERR_TRY;

	switch (Encoding & 0xFFFF)
	{
	case FILE_ENCODING_UTF16_LE:
		{
			// Do³¹cz nag³ówek, o ile nie wymuszono braku
			if ((Encoding & FILE_ENCODING_NO_BOM) == 0)
				Dest->Write(BOM_UTF16_LE, strlen(BOM_UTF16_LE));
			// Zapisz dane bezpoœrednio tak jak s¹ w pamiêci
			Dest->Write(Data, NumChars * sizeof(wchar_t));
		}
		break;
	case FILE_ENCODING_UTF8:
		{
			// Do³¹cz nag³ówek, jeœli wymuszono ¿e ma byæ
			if ((Encoding & FILE_ENCODING_FORCE_BOM) != 0)
				Dest->Write(BOM_UTF8, strlen(BOM_UTF8));
			// Skonwertuj Unicode na UTF8
			string utf8_string;
			if (!ConvertUnicodeToChars(&utf8_string, Data, NumChars, CP_UTF8))
				throw Error(_T("Cannot convert Unicode to UTF-8."), __TFILE__, __LINE__);
			// Zapisz ³añcuch UTF-8
			Dest->WriteStringF(utf8_string);
		}
		break;
	case FILE_ENCODING_ANSI:
		{
			// Skonwertuj Unicode na ANSI
			string ansi_string;
			if (!ConvertUnicodeToChars(&ansi_string, Data, NumChars, CP_ACP))
				throw Error(_T("Cannot convert Unicode to ANSI."), __TFILE__, __LINE__);
			// Zapisz ³añcuch ANSI
			Dest->WriteStringF(ansi_string);
		}
		break;
	default:
		assert(0 && "SaveUnicodeToStream: No encoding specified.");
	}

	ERR_CATCH(_T("Cannot save Unicode characters to stream."));
}

// Funkcja wewnêtrzna do testowania czy dane zaczynaj¹ siê od danego nag³ówka Unicode
bool HasBOM(const char *Data, size_t NumBytes, const char *BOM, size_t BOMNumBytes)
{
	if (NumBytes < BOMNumBytes)
		return false;
	
	for (size_t i = 0; i < BOMNumBytes; i++)
	{
		if (Data[i] != BOM[i])
			return false;
	}

	return true;
}

// Funkcja wewnêtrzna - wykrywa w jakim kodowaniu s¹ dane
// SuggestedEncoding ma byæ typu FILE_ENCODING (flagi z m³odszych 2 bajtów).
// Przez OutBomSize zwraca tak¿e rozmiar nag³ówka BOM w bajtach, jeœli taki jest (jeœli nie to zwraca 0).
enum ENC {
	ENC_UTF32_LE = 1, ENC_UTF32_BE,
	ENC_UTF16_LE, ENC_UTF16_BE,
	ENC_UTF8, ENC_ANSI,
};
ENC DetectEncoding(const char *Data, size_t NumBytes, unsigned SuggestedEncoding, size_t *OutBomSize)
{
	ENC Enc = (ENC)0;
	*OutBomSize = 0;

	// Nag³ówek UTF-32 LE
	if (HasBOM(Data, NumBytes, "\xFF\xFE\x00\x00", 4))
		Enc = ENC_UTF32_LE, *OutBomSize = 4;
	// Nag³ówek UTF-32 BE
	else if (HasBOM(Data, NumBytes, "\x00\x00\xFE\xFF", 4))
		Enc = ENC_UTF32_BE, *OutBomSize = 4;
	// Nag³ówek UTF-8
	else if (HasBOM(Data, NumBytes, BOM_UTF8, strlen(BOM_UTF8)))
		Enc = ENC_UTF8, *OutBomSize = strlen(BOM_UTF8);
	// Trzeba odpaliæ analizê
	else
	{
		int AnalysisFlags = IS_TEXT_UNICODE_SIGNATURE | IS_TEXT_UNICODE_REVERSE_SIGNATURE |
			IS_TEXT_UNICODE_ASCII16 | IS_TEXT_UNICODE_REVERSE_ASCII16 |
			IS_TEXT_UNICODE_ILLEGAL_CHARS | IS_TEXT_UNICODE_ODD_LENGTH | IS_TEXT_UNICODE_NULL_BYTES |
			IS_TEXT_UNICODE_STATISTICS | IS_TEXT_UNICODE_REVERSE_STATISTICS |
			IS_TEXT_UNICODE_CONTROLS | IS_TEXT_UNICODE_REVERSE_CONTROLS;
		IsTextUnicode(Data, (int)NumBytes, &AnalysisFlags);

		// Nag³ówek UTF-16 LE
		if ((AnalysisFlags & IS_TEXT_UNICODE_SIGNATURE) != 0)
			Enc = ENC_UTF16_LE, *OutBomSize = 2;
		// Nag³ówek UTF-16 BE
		else if ((AnalysisFlags & IS_TEXT_UNICODE_REVERSE_SIGNATURE) != 0)
			Enc = ENC_UTF16_BE, *OutBomSize = 2;
		// To jest na pewno UTF-16 LE
		else if ((AnalysisFlags & IS_TEXT_UNICODE_ASCII16) != 0)
			Enc = ENC_UTF16_LE;
		// To jest na pewno UTF-16 BE
		else if ((AnalysisFlags & IS_TEXT_UNICODE_REVERSE_ASCII16) != 0)
			Enc = ENC_UTF16_BE;
		// Nadal nie wiadomo...
		else
		{
			// Liczymy argumenty za UTF-16 LE i BE
			int UTF16LE_Args = 0, UTF16BE_Args = 0;

			if ((AnalysisFlags & IS_TEXT_UNICODE_NULL_BYTES) != 0)
				UTF16LE_Args++, UTF16BE_Args++;

			if ((AnalysisFlags & IS_TEXT_UNICODE_STATISTICS) != 0)
				UTF16LE_Args++;
			if ((AnalysisFlags & IS_TEXT_UNICODE_REVERSE_STATISTICS) != 0)
				UTF16BE_Args++;

			if ((AnalysisFlags & IS_TEXT_UNICODE_CONTROLS) != 0)
				UTF16LE_Args++;
			if ((AnalysisFlags & IS_TEXT_UNICODE_REVERSE_CONTROLS) != 0)
				UTF16BE_Args++;

			// To jednak na pewno nie jest UTF-16
			if ((AnalysisFlags & IS_TEXT_UNICODE_ILLEGAL_CHARS) != 0 || (AnalysisFlags & IS_TEXT_UNICODE_ODD_LENGTH) != 0)
				UTF16LE_Args = 0, UTF16BE_Args = 0;

			if (UTF16LE_Args > 0 && UTF16LE_Args >= UTF16BE_Args)
				Enc = ENC_UTF16_LE;
			else if (UTF16BE_Args > 0)
				Enc = ENC_UTF16_BE;
			// To nie jest UTF-16 - jest UTF-8 lub ANSI
			else
			{
				if (SuggestedEncoding == FILE_ENCODING_UTF8)
					Enc = ENC_UTF8;
				else if (SuggestedEncoding == FILE_ENCODING_ANSI)
					Enc = ENC_ANSI;
				// Preferowane kodowanie nie podane lub podane UTF16 - wybierz domyœlne
				else
					Enc = ENC_UTF8;
			}
		}
	}

	return Enc;
}


void LoadUnicodeFromFile(const tstring &FileName, wstring *Out, unsigned Encoding, FILE_ENCODING *OutEncoding)
{
	ERR_TRY;

	FileStream F(FileName, FM_READ);
	LoadUnicodeFromStream(&F, Out, Encoding, OutEncoding);

	ERR_CATCH(_T("Cannot load Unicode characters from file: ") + FileName);
}

void LoadUnicodeFromFile(const tstring &FileName, string *Out, unsigned Encoding, FILE_ENCODING *OutEncoding)
{
	ERR_TRY;

	FileStream F(FileName, FM_READ);
	LoadUnicodeFromStream(&F, Out, Encoding, OutEncoding);

	ERR_CATCH(_T("Cannot load Unicode characters from file: ") + FileName);
}

void LoadUnicodeFromStream(SeekableStream *Src, wstring *Out, unsigned Encoding, FILE_ENCODING *OutEncoding)
{
	ERR_TRY;

	// Wczytaj ca³y plik do pamiêci
	VectorStream VS;
	{
		VS.SetCapacity(Src->GetSize());
		CopyToEnd(&VS, Src);
		VS.Rewind();
	}

	// Nie ma automatycznego wykrywania - koniecznie podane kodowanie
	if ((Encoding & FILE_ENCODING_AUTODETECT) == 0)
	{
		switch (Encoding & 0xFFFF)
		{
		case FILE_ENCODING_UTF16_LE:
			{
				// Jeœli jest nag³ówek, pomiñ go
				size_t BOMSize = strlen(BOM_UTF16_LE);
				if (HasBOM(VS.Data(), VS.GetSize(), BOM_UTF16_LE, BOMSize))
					VS.SetPosFromCurrent(BOMSize);
				// Wczytaj dane
				VS.ReadStringToEnd(Out);
			}
			break;
		case FILE_ENCODING_UTF8:
			{
				// Jeœli jest nag³ówek, pomiñ go
				size_t BOMSize = strlen(BOM_UTF8);
				size_t Index = 0;
				if (HasBOM(VS.Data(), VS.GetSize(), BOM_UTF8, BOMSize))
					Index += BOMSize;
				// Wczytaj dane - skonwertuj z UTF-8 do Unicode
				if (!ConvertCharsToUnicode(Out, &VS.Data()[Index], VS.GetSize() - Index, CP_UTF8))
					throw Error(_T("Cannot convert UTF-8 to Unicode."), __TFILE__, __LINE__);
			}
			break;
		case FILE_ENCODING_ANSI:
			{
				// Wczytaj dane - skonwertuj z ANSI do Unicode
				if (!ConvertCharsToUnicode(Out, VS.Data(), VS.GetSize(), CP_ACP))
					throw Error(_T("Cannot convert ANSI to Unicode."), __TFILE__, __LINE__);
			}
			break;
		default:
			assert(0 && "LoadUnicodeFromStream: No encoding and no AUTODETECT specified.");
		}
	}
	// Jest automatyczne wykrywanie
	else
	{
		// Wykryj kodowanie
		size_t BomSize;
		ENC Enc = DetectEncoding(VS.Data(), VS.GetSize(), Encoding & 0xFFFF, &BomSize);

		// Rzuæ wyj¹tek, jeœli to nieob³sugiwane kodowanie
		if (Enc == ENC_UTF32_LE)
			throw Error(_T("LoadUnicodeFromStream: UTF-32 LE encoding not supported."), __TFILE__, __LINE__);
		else if (Enc == ENC_UTF32_BE)
			throw Error(_T("LoadUnicodeFromStream: UTF-32 BE encoding not supported."), __TFILE__, __LINE__);
		else if (Enc == ENC_UTF16_BE)
			throw Error(_T("LoadUnicodeFromStream: UTF-16 BE encoding not supported."), __TFILE__, __LINE__);
		// Wczytaj dane w znanym kodowaniu
		else if (Enc == ENC_UTF16_LE)
		{
			if (OutEncoding != NULL) *OutEncoding = FILE_ENCODING_UTF16_LE;
			// Pomiñ nag³ówek
			VS.SetPos(BomSize);
			// Wczytaj dane
			VS.ReadStringToEnd(Out);
		}
		else if (Enc == ENC_UTF8)
		{
			if (OutEncoding != NULL) *OutEncoding = FILE_ENCODING_UTF8;
			// Wczytaj dane - skonwertuj z UTF-8 do Unicode
			if (!ConvertCharsToUnicode(Out, &VS.Data()[BomSize], VS.GetSize() - BomSize, CP_UTF8))
			{
				// Jeœli nie uda³o siê jako UTF-8, skonwertuj do ANSI
				if (OutEncoding != NULL) *OutEncoding = FILE_ENCODING_ANSI;
				if (!ConvertCharsToUnicode(Out, VS.Data(), VS.GetSize(), CP_ACP))
					throw Error(_T("Cannot convert UTF-8 or ANSI to Unicode."), __TFILE__, __LINE__);
			}
		}
		else // if (Enc == ENC_UTF8)
		{
			if (OutEncoding != NULL) *OutEncoding = FILE_ENCODING_ANSI;
			// Wczytaj dane - skonwertuj z ANSI do Unicode
			if (!ConvertCharsToUnicode(Out, VS.Data(), VS.GetSize(), CP_ACP))
				throw Error(_T("Cannot convert ANSI to Unicode."), __TFILE__, __LINE__);
		}
	}

	ERR_CATCH(_T("Cannot load Unicode characters from stream."));
}

void LoadUnicodeFromStream(SeekableStream *Src, string *Out, unsigned Encoding, FILE_ENCODING *OutEncoding)
{
	ERR_TRY;

	// Wczytaj ca³y plik do pamiêci
	VectorStream VS;
	{
		VS.SetCapacity(Src->GetSize());
		CopyToEnd(&VS, Src);
		VS.Rewind();
	}

	// Nie ma automatycznego wykrywania - koniecznie podane kodowanie
	if ((Encoding & FILE_ENCODING_AUTODETECT) == 0)
	{
		switch (Encoding & 0xFFFF)
		{
		case FILE_ENCODING_UTF16_LE:
			{
				// Jeœli jest nag³ówek, pomiñ go
				size_t BOMSize = strlen(BOM_UTF16_LE);
				size_t Index = 0;
				if (HasBOM(VS.Data(), VS.GetSize(), BOM_UTF16_LE, BOMSize))
					Index = BOMSize;
				// Skonwertuj Unicode na ANSI
				if (!ConvertUnicodeToChars(Out, (const wchar_t*)&VS.Data()[Index], (VS.GetSize() / Index) / sizeof(wchar_t), CP_ACP))
					throw Error(_T("Cannot convert Unicode to ANSI."), __TFILE__, __LINE__);
			}
			break;
		case FILE_ENCODING_UTF8:
			{
				// Jeœli jest nag³ówek, pomiñ go
				size_t BOMSize = strlen(BOM_UTF8);
				size_t Index = 0;
				if (HasBOM(VS.Data(), VS.GetSize(), BOM_UTF8, BOMSize))
					Index += BOMSize;
				// Wczytaj dane - skonwertuj z UTF-8 do Unicode
				wstring TmpWstr;
				if (!ConvertCharsToUnicode(&TmpWstr, &VS.Data()[Index], VS.GetSize() - Index, CP_UTF8))
					throw Error(_T("Cannot convert UTF-8 to Unicode."), __TFILE__, __LINE__);
				// Skonwertuj Unicode do ANSI
				if (!ConvertUnicodeToChars(Out, TmpWstr, CP_ACP))
					throw Error(_T("Cannot convert Unicode to ANSI."), __TFILE__, __LINE__);
				// Niestaty musia³em to zrobiæ tak na dwa razy, nie ma w WinAPI funkcji UTF8 -> ANSI :(
			}
			break;
		case FILE_ENCODING_ANSI:
			{
				// Wczytaj dane - tak jak s¹
				VS.ReadStringToEnd(Out);
			}
			break;
		default:
			assert(0 && "LoadUnicodeFromStream: No encoding and no AUTODETECT specified.");
		}
	}
	// Jest automatyczne wykrywanie
	else
	{
		// Wykryj kodowanie
		size_t BomSize;
		ENC Enc = DetectEncoding(VS.Data(), VS.GetSize(), Encoding & 0xFFFF, &BomSize);

		// Rzuæ wyj¹tek, jeœli to nieob³sugiwane kodowanie
		if (Enc == ENC_UTF32_LE)
			throw Error(_T("LoadUnicodeFromStream: UTF-32 LE encoding not supported."), __TFILE__, __LINE__);
		else if (Enc == ENC_UTF32_BE)
			throw Error(_T("LoadUnicodeFromStream: UTF-32 BE encoding not supported."), __TFILE__, __LINE__);
		else if (Enc == ENC_UTF16_BE)
			throw Error(_T("LoadUnicodeFromStream: UTF-16 BE encoding not supported."), __TFILE__, __LINE__);
		// Wczytaj dane w znanym kodowaniu
		else if (Enc == ENC_UTF16_LE)
		{
			if (OutEncoding != NULL) *OutEncoding = FILE_ENCODING_UTF16_LE;
			// Skonwertuj UTF-16 na ANSI
			if (!ConvertUnicodeToChars(Out, (const wchar_t*)&VS.Data()[BomSize], (VS.GetSize() - BomSize) / sizeof(wchar_t), CP_ACP))
				throw Error(_T("Cannot convert Unicode to ANSI."), __TFILE__, __LINE__);
		}
		else if (Enc == ENC_UTF8)
		{
			if (OutEncoding != NULL) *OutEncoding = FILE_ENCODING_UTF8;
			// Wczytaj dane - skonwertuj z UTF-8 do Unicode, potem Unicode do ANSI
			wstring TmpWstr;
			if (!ConvertCharsToUnicode(&TmpWstr, &VS.Data()[BomSize], VS.GetSize() - BomSize, CP_UTF8) ||
				!ConvertUnicodeToChars(Out, TmpWstr, CP_ACP))
			{
				// Jeœli nie uda³o siê jako UTF-8, wczytaj jako ANSI
				if (OutEncoding != NULL) *OutEncoding = FILE_ENCODING_ANSI;
				VS.ReadStringToEnd(Out);
			}
		}
		else // if (Enc == ENC_UTF8)
		{
			if (OutEncoding != NULL) *OutEncoding = FILE_ENCODING_ANSI;
			// Wczytaj dane jako ANSI
			VS.ReadStringToEnd(Out);
		}
	}

	ERR_CATCH(_T("Cannot load Unicode characters from stream."));
}

#endif


} // namespace common
