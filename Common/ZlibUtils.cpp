/** \file
\brief Auxiliary module for zlib library
\author Adam Sawicki - sawickiap@poczta.onet.pl - http://asawicki.info/ \n

Part of CommonLib library. \n
Encoding UTF-8, end of line CR+LF \n
License: GNU LGPL. \n
Documentation: \ref Module_ZlibUtils\n
Module components: \ref code_zlibutils
*/
#include "Base.hpp"
extern "C" {
	#include <errno.h>
	#include <zlib.h>
	#include <string.h> // dla strnlen
}
#include "DateTime.hpp"
#include "ZlibUtils.hpp"


namespace common
{

//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Klasa ZlibCompressionStreamError

ZlibError::ZlibError(int Code, const tstring &Msg, const tstring &File, int Line)
{
	if (Code == Z_ERRNO)
	{
	#ifdef WIN32
		#ifdef _MSC_VER
			tchar buf[256];
			_tcserror_s(buf, _countof(buf), errno);
			Push(_T("(zlib(errno),") + IntToStrR(errno) + _T(") ") + buf);
		#else
			Push(_T("(zlib(errno),") + IntToStrR(errno) + _T(") ") + _tcserror(errno));
		#endif
	#else
		Push(_T("(zlib(errno),") + IntToStrR(errno) + _T(") ") + strerror(errno));
	#endif
	}
	else
		Push(Format(_T("(zlib,#) #")) % Code % StringToTstringR(zError(Code)));

	Push(Msg, File, Line);
}


const int ZLIB_DEFAULT_LEVEL = Z_DEFAULT_COMPRESSION;


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Klasa ZlibCompressionStream_pimpl

class ZlibCompressionStream_pimpl
{
private:
	Stream *m_Stream;
	z_stream m_ZStream;
	std::vector<char> m_OutBuf;
	// Tu jest tutaj, bo choæ tego panowie szanowni autorzy zliba dziady pi*** nie udokumentowali,
	// to header podany w deflateSetHeader musi istnieæ przy wywo³aniu deflate, bo dopiero wtedy nag³ówek
	// siê zapisuje, a deflateSetHeader tylko zachowuje sobie w state wskaŸnik do niego!!!
	gz_header m_Header;
	std::vector<char> m_Header_FileName;
	std::vector<char> m_Header_Comment;

public:
	// Wersja ZlibError
	ZlibCompressionStream_pimpl(Stream *a_Stream, int Level, int WindowBits = 15);
	// Wersja Gzip
	ZlibCompressionStream_pimpl(Stream *a_Stream, const string *FileName, const string *Comment, int Level, int WindowBits = 15);
	~ZlibCompressionStream_pimpl();

	void Write(const void *Data, size_t Size);
	void Flush();
};

ZlibCompressionStream_pimpl::ZlibCompressionStream_pimpl(Stream *a_Stream, int Level, int WindowBits) :
	m_Stream(a_Stream)
{
	m_OutBuf.resize(BUFFER_SIZE);

	m_ZStream.zalloc = Z_NULL;
	m_ZStream.zfree = Z_NULL;
	m_ZStream.opaque = Z_NULL;

	int R = deflateInit2(&m_ZStream, Level, Z_DEFLATED, WindowBits, 8, Z_DEFAULT_STRATEGY);
	if (R != Z_OK)
		throw ZlibError(R, _T("Cannot initialize zlib compression."), __TFILE__, __LINE__);
}

ZlibCompressionStream_pimpl::ZlibCompressionStream_pimpl(Stream *a_Stream, const string *FileName, const string *Comment, int Level, int WindowBits) :
	m_Stream(a_Stream)
{
	m_OutBuf.resize(BUFFER_SIZE);

	m_ZStream.zalloc = Z_NULL;
	m_ZStream.zfree = Z_NULL;
	m_ZStream.opaque = Z_NULL;

	int R = deflateInit2(&m_ZStream, Level, Z_DEFLATED, WindowBits + 16, 8, Z_DEFAULT_STRATEGY);
	if (R != Z_OK)
		throw ZlibError(R, _T("Cannot initialize gzip compression."), __TFILE__, __LINE__);

	// Nag³ówek gzip
	common_memzero(&m_Header, sizeof(m_Header));
	m_Header.time = (uLong)Now().GetTicks();
#ifdef WIN32
	m_Header.os = 0;
#else
	m_Header.os = 3;
#endif
	m_Header.extra = Z_NULL;
	if (FileName != NULL && !FileName->empty())
	{
		m_Header_FileName.resize(FileName->length()+1);
		std::copy(FileName->begin(), FileName->end(), m_Header_FileName.begin());
		m_Header_FileName[FileName->length()] = '\0';
		m_Header.name = (Bytef*)&m_Header_FileName[0];
	}
	else
		m_Header.name = Z_NULL;
	if (Comment != NULL && !Comment->empty())
	{
		m_Header_Comment.resize(Comment->length()+1);
		std::copy(Comment->begin(), Comment->end(), m_Header_Comment.begin());
		m_Header_Comment[Comment->length()] = '\0';
		m_Header.comment = (Bytef*)&m_Header_Comment[0];
	}
	else
		m_Header.comment = Z_NULL;

	R = deflateSetHeader(&m_ZStream, &m_Header);
	if (R != Z_OK)
		throw ZlibError(R, _T("Nie mo¿na ustawiæ nag³ówka gzip"), __TFILE__, __LINE__);
}

ZlibCompressionStream_pimpl::~ZlibCompressionStream_pimpl()
{
	try
	{
		char Foo = '\0';
		m_ZStream.next_in = (Bytef*)&Foo;
		m_ZStream.avail_in = 0;

		int R;
		do
		{
			m_ZStream.next_out = (Bytef*)&m_OutBuf[0];
			m_ZStream.avail_out = BUFFER_SIZE;

			R = deflate(&m_ZStream, Z_FINISH);

			if (m_ZStream.avail_out < BUFFER_SIZE)
				m_Stream->Write(&m_OutBuf[0], BUFFER_SIZE - m_ZStream.avail_out);
		}
		while (R != Z_STREAM_END);

		deflateEnd(&m_ZStream);
	}
	catch (...)
	{
		assert(0 && "ZlibCompressionStream.dtor - exception.");
	}
}

void ZlibCompressionStream_pimpl::Write(const void *Data, size_t Size)
{
	if (Size == 0) return;

	m_ZStream.next_in = (Bytef*)Data;
	m_ZStream.avail_in = Size;

	do
	{
		m_ZStream.next_out = (Bytef*)&m_OutBuf[0];
		m_ZStream.avail_out = BUFFER_SIZE;

		deflate(&m_ZStream, Z_NO_FLUSH); // Nie sprawdzamy b³êdu, bo podobno nie trzeba a nawet nie nale¿y, bo zdarza siê Z_BUF_ERROR.

		if (m_ZStream.avail_out < BUFFER_SIZE)
			m_Stream->Write(&m_OutBuf[0], BUFFER_SIZE - m_ZStream.avail_out);
	}
	while (m_ZStream.avail_out == 0); // Tak, dziwne, ale w³aœnie taki warunek + ten assert jest w przyk³adzie w zlib.
	assert(m_ZStream.avail_in == 0);
}

void ZlibCompressionStream_pimpl::Flush()
{
	char Foo = '\0';
	m_ZStream.next_in = (Bytef*)&Foo;
	m_ZStream.avail_in = 0;

	do
	{
		m_ZStream.next_out = (Bytef*)&m_OutBuf[0];
		m_ZStream.avail_out = BUFFER_SIZE;

		deflate(&m_ZStream, Z_SYNC_FLUSH);

		if (m_ZStream.avail_out < BUFFER_SIZE)
			m_Stream->Write(&m_OutBuf[0], BUFFER_SIZE - m_ZStream.avail_out);
	}
	while (m_ZStream.avail_out == 0);
}


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Klasa ZlibCompressionStream

ZlibCompressionStream::ZlibCompressionStream(Stream *a_Stream, int Level, int WindowBits) :
	OverlayStream(a_Stream),
	pimpl(new ZlibCompressionStream_pimpl(a_Stream, Level, WindowBits))
{
}

ZlibCompressionStream::~ZlibCompressionStream()
{
	pimpl.reset();
}

void ZlibCompressionStream::Write(const void *Data, size_t Size)
{
	pimpl->Write(Data, Size);
}

void ZlibCompressionStream::Flush()
{
	pimpl->Flush();
}

size_t ZlibCompressionStream::CompressLength(size_t DataLength)
{
	return compressBound(DataLength);
}

size_t ZlibCompressionStream::Compress(void *OutData, size_t BufLength, const void *Data, size_t DataLength, int Level)
{
	uLongf DestLen = BufLength;
	int R = compress2((Bytef*)OutData, &DestLen, (const Bytef*)Data, DataLength, Level);
	if (R != Z_OK)
		throw ZlibError(R, _T("Cannot compress data with zlib."), __TFILE__, __LINE__);
	return DestLen;
}


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Klasa GzipCompressionStream

GzipCompressionStream::GzipCompressionStream(Stream *a_Stream, const string *FileName, const string *Comment, int Level, int WindowBits) :
	OverlayStream(a_Stream),
	pimpl(new ZlibCompressionStream_pimpl(a_Stream, FileName, Comment, Level, WindowBits))
{
}

GzipCompressionStream::~GzipCompressionStream()
{
	pimpl.reset();
}

void GzipCompressionStream::Write(const void *Data, size_t Size)
{
	pimpl->Write(Data, Size);
}

void GzipCompressionStream::Flush()
{
	pimpl->Flush();
}


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Klasa ZlibDecompressionStream_pimpl

class ZlibDecompressionStream_pimpl
{
private:
	Stream *m_Stream;
	z_stream m_ZStream;
	std::vector<char> m_InBuf;
	size_t m_InBufLen;
	bool m_OutEnd;
	gz_header m_Header;
	std::vector<char> m_Header_FileName;
	std::vector<char> m_Header_Comment;

	void EnsureBuf(Stream *a_Stream);

public:
	// Wersja zlib
	ZlibDecompressionStream_pimpl(Stream *a_Stream, bool Gzip, int WindowBits = 15);
	// Wersja gzip
	~ZlibDecompressionStream_pimpl();

	size_t Read(void *Out, size_t MaxLength);
	bool End();

	bool GetHeaderFileName(string *OutFileName);
	bool GetHeaderComment(string *OutComment);
};

void ZlibDecompressionStream_pimpl::EnsureBuf(Stream *a_Stream)
{
	// Nie pamiêtam sk¹d to siê wziê³o, ale natkn¹³em siê na b³¹d ¿e ta asercja zwraca³a false,
	// a jej zakomentowanie sprawi³o ¿e wszystko dzia³a dobrze.
//	assert(m_InBufLen == 0);
	m_InBufLen = a_Stream->Read(&m_InBuf[0], BUFFER_SIZE);
}

ZlibDecompressionStream_pimpl::ZlibDecompressionStream_pimpl(Stream *a_Stream, bool Gzip, int WindowBits) :
	m_Stream(a_Stream)
{
	m_InBuf.resize(BUFFER_SIZE);
	m_InBufLen = 0;

	m_ZStream.zalloc = Z_NULL;
	m_ZStream.zfree = Z_NULL;
	m_ZStream.opaque = Z_NULL;

	EnsureBuf(m_Stream);
	m_ZStream.next_in = (Bytef*)&m_InBuf[0];
	m_ZStream.avail_in = m_InBufLen;

	// Tak te¿ mo¿e byæ
	//m_ZStream.next_in = 0;
	//m_ZStream.avail_in = Z_NULL;

	if (Gzip)
	{
		int R = inflateInit2(&m_ZStream, WindowBits + 16);
		if (R != Z_OK)
			throw ZlibError(R, _T("Cannot initialize gzip decompression."), __TFILE__, __LINE__);

		// Odczytanie nag³ówka
		m_Header_FileName.resize(256);
		m_Header_Comment.resize(256);
		m_Header.extra = Z_NULL;
		m_Header.extra_len = 0;
		m_Header.name = (Bytef*)&m_Header_FileName[0];
		m_Header.name_max = 256;
		m_Header.comment = (Bytef*)&m_Header_Comment[0];
		m_Header.comm_max = 256;
		R = inflateGetHeader(&m_ZStream, &m_Header);
		if (R != Z_OK)
			throw ZlibError(R, _T("zlib.inflateGetHeader"), __TFILE__, __LINE__);
	}
	else
	{
		int R = inflateInit2(&m_ZStream, WindowBits);
		if (R != Z_OK)
			throw ZlibError(R, _T("Cannot initialize zlib decompression."), __TFILE__, __LINE__);
	}

	char Foo = '\0';
	m_ZStream.next_out = (Bytef*)&Foo;
	m_ZStream.avail_out = 0;
	
	int R = inflate(&m_ZStream, Z_NO_FLUSH);
	m_OutEnd = (R == Z_STREAM_END);
	// Tego jednak nie, bo tutaj jeœli nie Z_STREAM_END, to zwraca Z_BUF_ERROR, bo mamy avail_in=0 i avail_out=0
//	if (R != Z_OK && R != Z_STREAM_END)
//		throw ZlibError(R, _T("Cannot start zlib decompression."), __TFILE__, __LINE__);
}

ZlibDecompressionStream_pimpl::~ZlibDecompressionStream_pimpl()
{
	inflateEnd(&m_ZStream);
}

size_t ZlibDecompressionStream_pimpl::Read(void *Out, size_t MaxLength)
{
	if (m_OutEnd == true || MaxLength == 0)
		return 0;

	// avail_out wskazuje na liczbê pozosta³ych bajtów do odczytania w tym wywo³aniu Read.
	//   Wiêc (MaxLength - avail_out) to liczba zwróconych przez zlib bajtów w tym wywo³aniu Read.
	// avail_in to liczba niepobranych jeszcze przez zlib danych z bufora InBuf.
	//   Wiêc (InBufLen - avail_in) to liczba przetworznych ju¿ przez zlib bajtów danego bufora InBuf.

	// Miejsce do pisania
	m_ZStream.avail_out = MaxLength;
	m_ZStream.next_out = (Bytef*)Out;

	for (;;)
	{
		// Przeczytane ju¿ pe³ne MaxLength bajtów
		if (m_ZStream.avail_out == 0)
			return MaxLength;

		// Zapewnij dane do czytania
		if (m_ZStream.avail_in == 0)
		{
			EnsureBuf(m_Stream);
			// Koniec danych wejœciowych, a wci¹¿ nie koniec strumienia zlib - niew¹tpliwy b³¹d, niedokoñczone dane
			if (m_InBufLen == 0)
				throw Error(_T("Zlib decompression stream error: Unfinished data."), __TFILE__, __LINE__);
			m_ZStream.next_in = (Bytef*)&m_InBuf[0];
			m_ZStream.avail_in = m_InBufLen;
		}

		int R = inflate(&m_ZStream, Z_NO_FLUSH);

		// ZlibError wykry³ koniec strumienia
		if (R == Z_STREAM_END)
		{
			m_OutEnd = true;
			return MaxLength - m_ZStream.avail_out;
		}
		// B³¹d
		else if (R != Z_OK)
			throw ZlibError(R, _T("Zlib compression stream error."), __TFILE__, __LINE__);
	}
}

bool ZlibDecompressionStream_pimpl::End()
{
	return m_OutEnd;
}

bool ZlibDecompressionStream_pimpl::GetHeaderFileName(string *OutFileName)
{
	if (m_Header.done != 1) return false;
	if (m_Header.name == Z_NULL) return false;

	OutFileName->assign(&m_Header_FileName[0], strnlen(&m_Header_FileName[0], 256));
	return true;
}

bool ZlibDecompressionStream_pimpl::GetHeaderComment(string *OutComment)
{
	if (m_Header.done != 1) return false;
	if (m_Header.comment == Z_NULL) return false;

	OutComment->assign(&m_Header_Comment[0], strnlen(&m_Header_Comment[0], 256));
	return true;
}


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Klasa ZlibDecompressionStream

ZlibDecompressionStream::ZlibDecompressionStream(Stream *a_Stream, int WindowBits) :
	OverlayStream(a_Stream),
	pimpl(new ZlibDecompressionStream_pimpl(a_Stream, false, WindowBits))
{
}

ZlibDecompressionStream::~ZlibDecompressionStream()
{
	pimpl.reset();
}

size_t ZlibDecompressionStream::Read(void *Out, size_t MaxLength)
{
	return pimpl->Read(Out, MaxLength);
}

bool ZlibDecompressionStream::End()
{
	return pimpl->End();
}

size_t ZlibDecompressionStream::Decompress(void *OutData, size_t BufLength, const void *Data, size_t DataLength)
{
	uLongf DestLen = BufLength;
	int R = uncompress((Bytef*)OutData, &DestLen, (Bytef*)Data, DataLength);
	if (R != Z_OK)
		throw ZlibError(R, _T("Cannot decompress data with zlib."), __TFILE__, __LINE__);
	return DestLen;
}


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Klasa GzipDecompressionStream

GzipDecompressionStream::GzipDecompressionStream(Stream *a_Stream, int WindowBits) :
	OverlayStream(a_Stream),
	pimpl(new ZlibDecompressionStream_pimpl(a_Stream, true, WindowBits))
{
}

GzipDecompressionStream::~GzipDecompressionStream()
{
	pimpl.reset();
}

size_t GzipDecompressionStream::Read(void *Out, size_t MaxLength)
{
	return pimpl->Read(Out, MaxLength);
}

bool GzipDecompressionStream::End()
{
	return pimpl->End();
}

bool GzipDecompressionStream::GetHeaderFileName(string *OutFileName)
{
	return pimpl->GetHeaderFileName(OutFileName);
}

bool GzipDecompressionStream::GetHeaderComment(string *OutComment)
{
	return pimpl->GetHeaderComment(OutComment);
}


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Klasa GzipFileStream

class GzipFileStream_pimpl
{
public:
	gzFile File;
	char OneCharBuf;
	bool m_End;
};

GzipFileStream::GzipFileStream(const string &FileName, GZIP_FILE_MODE Mode, int Level) :
	pimpl(new GzipFileStream_pimpl)
{
	char ModeSz[4];
	ModeSz[0] = (Mode == GZFM_WRITE ? 'w' : 'r');
	ModeSz[1] = 'b';
	if (Level == Z_DEFAULT_COMPRESSION)
		ModeSz[2] = '\0';
	else
	{
		ModeSz[2] = (char)(Level + '0');
		ModeSz[3] = '\0';
	}

	pimpl->File = gzopen(FileName.c_str(), ModeSz);

	if (pimpl->File == NULL)
	{
		if (errno == 0)
			throw ZlibError(Z_MEM_ERROR, _T("Cannot open gzip file: ") + StringToTstringR(FileName), __TFILE__, __LINE__);
		else
			throw ErrnoError(_T("Cannot open gzip file: ") + StringToTstringR(FileName), __TFILE__, __LINE__);
	}

	if (Mode == GZFM_READ)
	{
		int R = gzread(pimpl->File, &pimpl->OneCharBuf, 1);
		if (R == -1)
			throw Error(_T("Cannot read first byte from gzip file"), __TFILE__, __LINE__);
		pimpl->m_End = (R == 0);
	}
}

GzipFileStream::~GzipFileStream()
{
	gzclose(pimpl->File);
}

void GzipFileStream::Write(const void *Data, size_t Size)
{
	size_t Written = gzwrite(pimpl->File, Data, Size);
	if (Written != Size)
		throw Error(Format(_T("Cannot write to gzip file - #/# bytes written.")) % Written % Size, __TFILE__, __LINE__);
}

void GzipFileStream::Flush()
{
	int R = gzflush(pimpl->File, Z_SYNC_FLUSH);
	if (R != Z_OK)
		throw ZlibError(R, _T("zlib.GzipFileStream.Flush"), __TFILE__, __LINE__);
}

size_t GzipFileStream::Read(void *Out, size_t MaxLength)
{
	if (pimpl->m_End) return 0;
	if (MaxLength == 0) return 0;

	char *Out2 = (char*)Out;
	*Out2 = pimpl->OneCharBuf;
	MaxLength--;
	Out2++;

	int BytesRead = gzread(pimpl->File, Out2, MaxLength);

	if (BytesRead == -1)
		throw Error(Format(_T("Cannot read # bytes from gzip file.")) % (MaxLength+1), __TFILE__, __LINE__);
	else if ((size_t)BytesRead < MaxLength)
		pimpl->m_End = 0;
	else
	{
		int R = gzread(pimpl->File, &pimpl->OneCharBuf, 1);
		if (R == -1)
			throw Error(_T("Cannot read byte from gzip file."), __TFILE__, __LINE__);
		pimpl->m_End = (R == 0);
	}

	return (size_t)BytesRead + 1;
}

bool GzipFileStream::End()
{
	return pimpl->m_End;
}

} // namespace common
