/** \file
\brief Hierarchia klas strumieni
\author Adam Sawicki - sawickiap@poczta.onet.pl - http://regedit.gamedev.pl/ \n

Część biblioteki CommonLib \n
Kodowanie Windows-1250, koniec wiersza CR+LF, test: Zażółć gęślą jaźń \n
Licencja: GNU LGPL. \n
Dokumentacja: \ref Module_Stream \n
Elementy modułu: \ref code_stream
*/
#include "Base.hpp"
#ifdef WIN32
	#include <typeinfo.h>
#else
	#include <typeinfo>
#endif
#include <memory.h> // dla memcpy
#include "Error.hpp"
#include "Stream.hpp"


namespace common
{

const size_t BUFFER_SIZE = 4096;

const tchar * const ERRMSG_DECODE_INVALID_CHAR = _T("Stream decoding error: Invalid character.");
const tchar * const ERRMSG_UNEXPECTED_END      = _T("Stream error: Unexpected end of data.");

const char BASE64_CHARS[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Funkcje globalne wewnętrzne

// 0 = normalny znak Base64
// 1 = znak '='
// 2 = znak nieznany (biały lub nie)
inline int Base64CharType(char Ch)
{
	if ( (Ch >= 'A' && Ch <= 'Z') || (Ch >= 'a' && Ch <= 'z') || (Ch >= '0' && Ch <= '9') || Ch == '+' || Ch == '/' )
		return 0;
	else if (Ch == '=')
		return 1;
	else
		return 2;
}

// Zwraca numer odpowiadający znakowi w base64.
// Jeśli to znak '=', zwraca 0xFE.
// Jeśli znak nieznany, zwraca 0xFF.
inline uint8 Base64CharToNumber(char Ch)
{
	if (Ch >= 'A' && Ch <= 'Z')
		return (uint8)(Ch - 'A');
	else if (Ch >= 'a' && Ch <= 'z')
		return (uint8)(Ch - ('a' - 26));
	else if (Ch >= '0' && Ch <= '9')
		return (uint8)(Ch - ('0' - 52));
	else if (Ch == '+')
		return 62;
	else if (Ch == '/')
		return 63;
	else if (Ch == '=')
		return 0xFE;
	else
		return 0xFF;
}

void _ThrowBufEndError(const tchar *File, int Line)
{
	throw Error(_T("Unexpected end of stream."), File, Line);
}


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Klasa Stream

void Stream::Write(const void *Data, size_t Size)
{
	throw Error(_T("Stream class doesn't support write."), __TFILE__, __LINE__);
}

////// WriteString 1 (ANSI)

void Stream::WriteString1(const string &s)
{
	WriteString1(s.data(), s.length());
}

void Stream::WriteString1(const char *s)
{
	WriteString1(s, strlen(s));
}

void Stream::WriteString1(const char *s, unsigned NumChars)
{
	typedef uint8 T;
	size_t Limit = static_cast<size_t>(std::numeric_limits<T>::max());
	if (NumChars > Limit)
		throw Error(Format(_T("Cannot write Unicode string to stream - longer than # characters.")) % Limit, __TFILE__, __LINE__);
	T Length = static_cast<T>(NumChars);
	Write(&Length, sizeof(T));
	Write(s, NumChars);
}

////// WriteString 1 (Unicode)
#ifdef WIN32

void Stream::WriteString1(const wstring &s)
{
	WriteString1(s.data(), s.length());
}

void Stream::WriteString1(const wchar_t *s)
{
	WriteString1(s, wcslen(s));
}

void Stream::WriteString1(const wchar_t *s, unsigned NumChars)
{
	typedef uint8 T;
	size_t Limit = static_cast<size_t>(std::numeric_limits<T>::max());
	if (NumChars > Limit)
		throw Error(Format(_T("Cannot write Unicode string to stream - longer than # characters.")) % Limit, __TFILE__, __LINE__);
	T Length = static_cast<T>(NumChars);
	Write(&Length, sizeof(T));
	Write(s, NumChars * sizeof(wchar_t));
}

#endif

////// WriteString 2 (ANSI)

void Stream::WriteString2(const string &s)
{
	WriteString2(s.data(), s.length());
}

void Stream::WriteString2(const char *s)
{
	WriteString2(s, strlen(s));
}

void Stream::WriteString2(const char *s, unsigned NumChars)
{
	typedef uint16 T;
	size_t Limit = static_cast<size_t>(std::numeric_limits<T>::max());
	if (NumChars > Limit)
		throw Error(Format(_T("Cannot write Unicode string to stream - longer than # characters.")) % Limit, __TFILE__, __LINE__);
	T Length = static_cast<T>(NumChars);
	Write(&Length, sizeof(T));
	Write(s, NumChars);
}

////// WriteString 2 (Unicode)
#ifdef WIN32

void Stream::WriteString2(const wstring &s)
{
	WriteString2(s.data(), s.length());
}

void Stream::WriteString2(const wchar_t *s)
{
	WriteString2(s, wcslen(s));
}

void Stream::WriteString2(const wchar_t *s, unsigned NumChars)
{
	typedef uint16 T;
	size_t Limit = static_cast<size_t>(std::numeric_limits<T>::max());
	if (NumChars > Limit)
		throw Error(Format(_T("Cannot write Unicode string to stream - longer than # characters.")) % Limit, __TFILE__, __LINE__);
	T Length = static_cast<T>(NumChars);
	Write(&Length, sizeof(T));
	Write(s, NumChars * sizeof(wchar_t));
}

#endif

////// WriteString 4 (ANSI)

void Stream::WriteString4(const string &s)
{
	WriteString4(s.data(), s.length());
}

void Stream::WriteString4(const char *s)
{
	WriteString4(s, strlen(s));
}

void Stream::WriteString4(const char *s, unsigned NumChars)
{
	Write(&NumChars, sizeof(NumChars));
	Write(s, NumChars);
}

////// WriteString 4 (Unicode)
#ifdef WIN32

void Stream::WriteString4(const wstring &s)
{
	WriteString4(s.data(), s.length());
}

void Stream::WriteString4(const wchar_t *s)
{
	WriteString4(s, wcslen(s));
}

void Stream::WriteString4(const wchar_t *s, unsigned NumChars)
{
	Write(&NumChars, sizeof(NumChars));
	Write(s, NumChars * sizeof(wchar_t));
}

#endif

////// End of WriteString X

void Stream::WriteStringF(const string &s)
{
	Write(s.data(), s.length());
}

#ifdef WIN32
void Stream::WriteStringF(const wstring &s)
{
	Write(s.data(), s.length() * sizeof(wchar_t));
}
#endif

void Stream::WriteBool(bool b)
{
	uint8 bt = (b ? 1 : 0);
	Write(&bt, sizeof(bt));
}

size_t Stream::Read(void *Data, size_t Size)
{
	throw Error(_T("Stream class doesn't support read."), __TFILE__, __LINE__);
}

void Stream::MustRead(void *Data, size_t Size)
{
	if (Size == 0) return;
	size_t i = Read(Data, Size);
	if (i != Size)
		throw Error(Format(_T("Stream read error: #/# bytes read.")) % i % Size, __TFILE__, __LINE__);
}

size_t Stream::Skip(size_t MaxLength)
{
	// Implementacja dla klasy Stream nie posiadającej kursora.
	// Trzeba wczytywać dane po kawałku.
	// MaxLength będzie zmniejszane. Oznacza liczbę pozostałych bajtów do pominięcia.
	
	if (MaxLength == 0)
		return 0;

	char Buf[BUFFER_SIZE];
	uint BlockSize, ReadSize, Sum = 0;
	while (MaxLength > 0)
	{
		BlockSize = std::min(MaxLength, BUFFER_SIZE);
		ReadSize = Read(Buf, BlockSize);
		Sum += ReadSize;
		MaxLength -= ReadSize;
		if (ReadSize < BlockSize)
			break;
	}
	return Sum;
}

////// ReadString (ANSI)

void Stream::ReadString1(string *s)
{
	typedef uint8 T;
	T Length;
	MustRead(&Length, sizeof(T));
	scoped_ptr<char, DeleteArrayPolicy> Buf(new char[Length]);
	MustRead(Buf.get(), Length);
	s->clear();
	s->append(Buf.get(), Length);
}

void Stream::ReadString2(string *s)
{
	typedef uint16 T;
	T Length;
	MustRead(&Length, sizeof(T));
	scoped_ptr<char, DeleteArrayPolicy> Buf(new char[Length]);
	MustRead(Buf.get(), Length);
	s->clear();
	s->append(Buf.get(), Length);
}

void Stream::ReadString4(string *s)
{
	typedef uint32 T;
	T Length;
	MustRead(&Length, sizeof(T));
	scoped_ptr<char, DeleteArrayPolicy> Buf(new char[Length]);
	MustRead(Buf.get(), Length);
	s->clear();
	s->append(Buf.get(), Length);
}

void Stream::ReadStringF(string *s, size_t NumChars)
{
	scoped_ptr<char, DeleteArrayPolicy> Buf(new char[NumChars]);
	MustRead(Buf.get(), NumChars);
	s->clear();
	s->append(Buf.get(), NumChars);
}

void Stream::ReadStringToEnd(string *s)
{
	s->clear();
	scoped_ptr<char, DeleteArrayPolicy> Buf(new char[BUFFER_SIZE]);
	size_t Size = BUFFER_SIZE;
	do
	{
		Size = Read(Buf.get(), Size);
		if (Size > 0)
			s->append(Buf.get(), Size);
	} while (Size == BUFFER_SIZE);
}

////// ReadString (Unicode)
#ifdef WIN32

void Stream::ReadString1(wstring *s)
{
	typedef uint8 T;
	T Length;
	MustRead(&Length, sizeof(T));
	scoped_ptr<wchar_t, DeleteArrayPolicy> Buf(new wchar_t[Length]);
	MustRead(Buf.get(), Length * sizeof(wchar_t));
	s->clear();
	s->append(Buf.get(), Length);
}

void Stream::ReadString2(wstring *s)
{
	typedef uint16 T;
	T Length;
	MustRead(&Length, sizeof(T));
	scoped_ptr<wchar_t, DeleteArrayPolicy> Buf(new wchar_t[Length]);
	MustRead(Buf.get(), Length * sizeof(wchar_t));
	s->clear();
	s->append(Buf.get(), Length);
}

void Stream::ReadString4(wstring *s)
{
	typedef uint32 T;
	T Length;
	MustRead(&Length, sizeof(T));
	scoped_ptr<wchar_t, DeleteArrayPolicy> Buf(new wchar_t[Length]);
	MustRead(Buf.get(), Length * sizeof(wchar_t));
	s->clear();
	s->append(Buf.get(), Length);
}

void Stream::ReadStringF(wstring *s, size_t NumChars)
{
	scoped_ptr<wchar_t, DeleteArrayPolicy> Buf(new wchar_t[NumChars]);
	MustRead(Buf.get(), NumChars * sizeof(wchar_t));
	s->clear();
	s->append(Buf.get(), NumChars);
}

void Stream::ReadStringToEnd(wstring *s)
{
	s->clear();
	scoped_ptr<wchar_t, DeleteArrayPolicy> Buf(new wchar_t[BUFFER_SIZE]);
	size_t Size = BUFFER_SIZE;
	do
	{
		Size = Read(Buf.get(), Size * sizeof(wchar_t));
		// Size dostaliśmy w bajtach, przetwarzamy na znaki Unicode
		if ((Size & 0x01) != 0)
			throw Error(_T("End of data inside an Unicode character."), __TFILE__, __LINE__);
		Size /= 2;
		if (Size > 0)
			s->append(Buf.get(), Size);
	} while (Size == BUFFER_SIZE);
}

#endif
////// End of ReadString

void Stream::ReadBool(bool *b)
{
	uint8 bt;
	MustRead(&bt, sizeof(bt));
	*b = (bt != 0);
}

void Stream::MustSkip(size_t Length)
{
	if (Skip(Length) != Length)
		throw Error(Format(_T("Cannot skip # bytes - end of stream met.")) % Length, __TFILE__, __LINE__);
}

bool Stream::End()
{
	throw Error(_T("Stream class doesn't support testing for end."), __TFILE__, __LINE__);
}

size_t Stream::CopyFrom(Stream *s, size_t Size)
{
	if (Size == 0) return 0;
	// Size - liczba bajtów, jaka została do odczytania
	scoped_ptr<char, DeleteArrayPolicy> Buf(new char[BUFFER_SIZE]);
	size_t ReqSize, ReadSize, BytesRead = 0;
	do
	{
		ReadSize = ReqSize = std::min(BUFFER_SIZE, Size);
		ReadSize = s->Read(Buf.get(), ReadSize);
		if (ReadSize > 0)
		{
			Write(Buf.get(), ReadSize);
			Size -= ReadSize;
			BytesRead += ReadSize;
		}
	} while (ReadSize == ReqSize && Size > 0);
	return BytesRead;
}

void Stream::MustCopyFrom(Stream *s, size_t Size)
{
	if (Size == 0) return;
	// Size - liczba bajtów, jaka została do odczytania
	scoped_ptr<char, DeleteArrayPolicy> Buf(new char[BUFFER_SIZE]);
	size_t ReqSize;
	do
	{
		ReqSize = std::min(BUFFER_SIZE, Size);
		s->MustRead(Buf.get(), ReqSize);
		if (ReqSize > 0)
		{
			Write(Buf.get(), ReqSize);
			Size -= ReqSize;
		}
	} while (Size > 0);
}

size_t Stream::CopyFromToEnd(Stream *s)
{
	scoped_ptr<char, DeleteArrayPolicy> Buf(new char[BUFFER_SIZE]);
	size_t Size = BUFFER_SIZE, bytesProcessed = 0;
	do
	{
		Size = s->Read(Buf.get(), Size);
		if (Size > 0)
		{
			Write(Buf.get(), Size);
			bytesProcessed += Size;
		}
	} while (Size == BUFFER_SIZE);
	return bytesProcessed;
}


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Klasa SeekableStream

size_t SeekableStream::GetSize()
{
	throw Error(_T("Stream class doesn't support size retrieval."), __TFILE__, __LINE__);
}

int SeekableStream::GetPos()
{
	throw Error(_T("Stream class # doesn't support cursor position retrieval."), __TFILE__, __LINE__);
}

void SeekableStream::SetPos(int pos)
{
	throw Error(_T("Stream class # doesn't support cursor position setting."), __TFILE__, __LINE__);
}

void SeekableStream::SetPosFromCurrent(int pos)
{
	SetPos(GetPos()+pos);
}

void SeekableStream::SetPosFromEnd(int pos)
{
	SetPos((int)GetSize()+pos);
}

void SeekableStream::Rewind()
{
	SetPos(0);
}

void SeekableStream::SetSize(size_t Size)
{
	throw Error(_T("Stream class # doesn't support size setting."), __TFILE__, __LINE__);
}

void SeekableStream::Truncate()
{
	SetSize((size_t)GetPos());
}

void SeekableStream::Clear()
{
	SetSize(0);
}

bool SeekableStream::End()
{
	return (GetPos() == (int)GetSize());
}

size_t SeekableStream::Skip(size_t MaxLength)
{
	uint Pos = GetPos();
	uint Size = GetSize();
	uint BytesToSkip = std::min(MaxLength, Size - Pos);
	SetPosFromCurrent((int)BytesToSkip);
	return BytesToSkip;
}


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Klasa CharWriter

CharWriter::~CharWriter()
{
	try
	{
		Flush();
	}
	catch (...)
	{
		assert(0 && "Exception caught in CharWriter::~CharWriter while calling CharWriter::Flush.");
	}
}

void CharWriter::WriteString(const string &s)
{
	uint BlockLength, i, s_i = 0, Length = s.length();
	// Length będzie zmniejszany. Oznacza liczbę pozostałych do zapisania bajtów.

	while (Length > 0)
	{
		// Bufor pełny
		if (m_BufIndex == BUFFER_SIZE)
			DoFlush();
		// Liczba bajtów do dopisania do bufora
		BlockLength = std::min(BUFFER_SIZE - m_BufIndex, Length);
		// Dopisanie do bufora
		for (i = 0; i < BlockLength; i++, m_BufIndex++, s_i++)
			m_Buf[m_BufIndex] = s[s_i];
		Length -= BlockLength;
	}
}

void CharWriter::WriteData(const void *Data, size_t Length)
{
	const char *CharData = (const char*)Data;
	uint BlockLength;
	// Length będzie zmniejszany. Oznacza liczbę pozostałych do zapisania bajtów.
	// CharData będzie przesuwany.

	while (Length > 0)
	{
		// Bufor pełny
		if (m_BufIndex == BUFFER_SIZE)
			DoFlush();
		// Liczba bajtów do dopisania do bufora
		BlockLength = std::min(BUFFER_SIZE - m_BufIndex, Length);
		// Dopisanie do bufora
		memcpy(&m_Buf[m_BufIndex], CharData, BlockLength);

		CharData += BlockLength;
		m_BufIndex += BlockLength;
		Length -= BlockLength;
	}
}


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Klasa WCharWriter
#ifdef WIN32

WCharWriter::~WCharWriter()
{
	try
	{
		Flush();
	}
	catch (...)
	{
		assert(0 && "Exception caught in WCharWriter::~CharWriter while calling WCharWriter::Flush.");
	}
}

void WCharWriter::WriteString(const wstring &s)
{
	uint BlockLength, i, s_i = 0, Length = s.length();
	// Length będzie zmniejszany. Oznacza liczbę pozostałych do zapisania znaków.

	while (Length > 0)
	{
		// Bufor pełny
		if (m_BufIndex == BUFFER_SIZE)
			DoFlush();
		// Liczba znaków do dopisania do bufora
		BlockLength = std::min(BUFFER_SIZE - m_BufIndex, Length);
		// Dopisanie do bufora
		for (i = 0; i < BlockLength; i++, m_BufIndex++, s_i++)
			m_Buf[m_BufIndex] = s[s_i];
		Length -= BlockLength;
	}
}

void WCharWriter::WriteString(const wchar_t *s, size_t NumChars)
{
	uint BlockLength;
	// NumChars będzie zmniejszany. Oznacza liczbę pozostałych do zapisania znaków.
	// s będzie przesuwany.

	while (NumChars > 0)
	{
		// Bufor pełny
		if (m_BufIndex == BUFFER_SIZE)
			DoFlush();
		// Liczba znaków do dopisania do bufora
		BlockLength = std::min(BUFFER_SIZE - m_BufIndex, NumChars);
		// Dopisanie do bufora
		memcpy(&m_Buf[m_BufIndex], s, BlockLength * sizeof(wchar_t));

		s += BlockLength;
		m_BufIndex += BlockLength;
		NumChars -= BlockLength;
	}
}

#endif


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Klasa CharReader

bool CharReader::EnsureNewChars()
{
	assert(m_BufBeg == m_BufEnd);
	uint ReadSize = m_Stream->Read(&m_Buf[0], BUFFER_SIZE);
	m_BufBeg = 0;
	m_BufEnd = ReadSize;
	return (ReadSize > 0);
}

size_t CharReader::ReadString(string *Out, size_t MaxLength)
{
	uint BlockSize, i, Sum = 0, Out_i = 0;
	Out->clear();

	// MaxLength będzie zmniejszane.
	// Tu nie mogę zrobić Out->resize(MaxLength), bo jako MaxLength podaje się często ogromne liczby.

	while (MaxLength > 0)
	{
		if (m_BufBeg == m_BufEnd)
		{
			if (!EnsureNewChars())
				return Sum;
		}
		BlockSize = std::min(m_BufEnd - m_BufBeg, MaxLength);
		Out->resize(Out_i + BlockSize);
		for (i = 0; i < BlockSize; i++)
		{
			(*Out)[Out_i] = m_Buf[m_BufBeg];
			Out_i++;
			m_BufBeg++;
		}
		MaxLength -= BlockSize;
		Sum += BlockSize;
	}
	return Sum;
}

void CharReader::MustReadString(string *Out, size_t Length)
{
	uint BlockSize, i, Out_i = 0;
	Out->clear();
	Out->resize(Length);

	// Length będzie zmniejszane.

	while (Length > 0)
	{
		if (m_BufBeg == m_BufEnd)
		{
			if (!EnsureNewChars())
				_ThrowBufEndError(__TFILE__, __LINE__);
		}
		BlockSize = std::min(m_BufEnd - m_BufBeg, Length);
		for (i = 0; i < BlockSize; i++)
		{
			(*Out)[Out_i] = m_Buf[m_BufBeg];
			Out_i++;
			m_BufBeg++;
		}
		Length -= BlockSize;
	}
}

size_t CharReader::ReadString(char *Out, size_t MaxLength)
{
	uint BlockSize, i, Sum = 0;

	// MaxLength będzie zmniejszane.
	// Out będzie przesuwane.

	while (MaxLength > 0)
	{
		if (m_BufBeg == m_BufEnd)
		{
			if (!EnsureNewChars())
				return Sum;
		}
		BlockSize = std::min(m_BufEnd - m_BufBeg, MaxLength);
		for (i = 0; i < BlockSize; i++)
		{
			*Out = m_Buf[m_BufBeg];
			Out++;
			m_BufBeg++;
		}
		MaxLength -= BlockSize;
		Sum += BlockSize;
	}
	return Sum;
}

void CharReader::MustReadString(char *Out, size_t Length)
{
	uint BlockSize, i;

	// Length będzie zmniejszane.
	// Out będzie przesuwane.

	while (Length > 0)
	{
		if (m_BufBeg == m_BufEnd)
		{
			if (!EnsureNewChars())
				_ThrowBufEndError(__TFILE__, __LINE__);
		}
		BlockSize = std::min(m_BufEnd - m_BufBeg, Length);
		for (i = 0; i < BlockSize; i++)
		{
			*Out = m_Buf[m_BufBeg];
			Out++;
			m_BufBeg++;
		}
		Length -= BlockSize;
	}
}

size_t CharReader::ReadData(void *Out, size_t MaxLength)
{
	char *OutChars = (char*)Out;
	uint BlockSize, Sum = 0;

	// MaxLength będzie zmniejszane.
	// OutChars będzie przesuwane.

	while (MaxLength > 0)
	{
		if (m_BufBeg == m_BufEnd)
		{
			if (!EnsureNewChars())
				return Sum;
		}
		BlockSize = std::min(m_BufEnd - m_BufBeg, MaxLength);
		common_memcpy(OutChars, &m_Buf[m_BufBeg], BlockSize);
		OutChars += BlockSize;
		m_BufBeg += BlockSize;
		MaxLength -= BlockSize;
		Sum += BlockSize;
	}
	return Sum;
}

void CharReader::MustReadData(void *Out, size_t Length)
{
	char *OutChars = (char*)Out;
	uint BlockSize, i;

	// Length będzie zmniejszane.
	// OutChars będzie przesuwane.

	while (Length > 0)
	{
		if (m_BufBeg == m_BufEnd)
		{
			if (!EnsureNewChars())
				_ThrowBufEndError(__TFILE__, __LINE__);
		}
		BlockSize = std::min(m_BufEnd - m_BufBeg, Length);
		for (i = 0; i < BlockSize; i++)
		{
			*OutChars = m_Buf[m_BufBeg];
			OutChars++;
			m_BufBeg++;
		}
		Length -= BlockSize;
	}
}

size_t CharReader::Skip(size_t MaxLength)
{
	uint BlockSize, Sum = 0;

	// MaxLength będzie zmniejszane.

	while (MaxLength > 0)
	{
		if (m_BufBeg == m_BufEnd)
		{
			if (!EnsureNewChars())
				return Sum;
		}
		BlockSize = std::min(m_BufEnd - m_BufBeg, MaxLength);
		m_BufBeg += BlockSize;
		MaxLength -= BlockSize;
		Sum += BlockSize;
	}
	return Sum;
}

void CharReader::MustSkip(size_t Length)
{
	uint BlockSize;

	// Length będzie zmniejszane.

	while (Length > 0)
	{
		if (m_BufBeg == m_BufEnd)
		{
			if (!EnsureNewChars())
				_ThrowBufEndError(__TFILE__, __LINE__);
		}
		BlockSize = std::min(m_BufEnd - m_BufBeg, Length);
		m_BufBeg += BlockSize;
		Length -= BlockSize;
	}
}

bool CharReader::ReadLine(string *Out)
{
	Out->clear();
	char Ch;
	bool WasEol = false;
	while (ReadChar(&Ch))
	{
		if (Ch == '\r')
		{
			WasEol = true;
			if (PeekChar(&Ch) && Ch == '\n')
				MustSkipChar();
			break;
		}
		else if (Ch == '\n')
		{
			WasEol = true;
			break;
		}
		else
			(*Out) += Ch;
	}
	return !Out->empty() || WasEol;
}

void CharReader::MustReadLine(string *Out)
{
	Out->clear();
	char Ch;
	while (ReadChar(&Ch))
	{
		if (Ch == '\r')
		{
			if (PeekChar(&Ch) && Ch == '\n')
				MustSkipChar();
			break;
		}
		else if (Ch == '\n')
			break;
		else
			(*Out) += Ch;
	}
}


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Klasa WCharReader
#ifdef WIN32

bool WCharReader::EnsureNewChars()
{
	assert(m_BufBeg == m_BufEnd);
	uint ReadSize = m_Stream->Read(&m_Buf[0], BUFFER_SIZE * sizeof(wchar_t));

	if ((ReadSize & 0x01) != 0)
		throw Error(_T("WCharReader: End of stream inside Unicode character."), __TFILE__, __LINE__);
	ReadSize /= 2;

	m_BufBeg = 0;
	m_BufEnd = ReadSize;
	return (ReadSize > 0);
}

size_t WCharReader::ReadString(wstring *Out, size_t MaxLength)
{
	uint BlockSize, i, Sum = 0, Out_i = 0;
	Out->clear();

	// MaxLength będzie zmniejszane.
	// Tu nie mogę zrobić Out->resize(MaxLength), bo jako MaxLength podaje się często ogromne liczby.

	while (MaxLength > 0)
	{
		if (m_BufBeg == m_BufEnd)
		{
			if (!EnsureNewChars())
				return Sum;
		}
		BlockSize = std::min(m_BufEnd - m_BufBeg, MaxLength);
		Out->resize(Out_i + BlockSize);
		for (i = 0; i < BlockSize; i++)
		{
			(*Out)[Out_i] = m_Buf[m_BufBeg];
			Out_i++;
			m_BufBeg++;
		}
		MaxLength -= BlockSize;
		Sum += BlockSize;
	}
	return Sum;
}

void WCharReader::MustReadString(wstring *Out, size_t Length)
{
	uint BlockSize, i, Out_i = 0;
	Out->clear();
	Out->resize(Length);

	// Length będzie zmniejszane.

	while (Length > 0)
	{
		if (m_BufBeg == m_BufEnd)
		{
			if (!EnsureNewChars())
				_ThrowBufEndError(__TFILE__, __LINE__);
		}
		BlockSize = std::min(m_BufEnd - m_BufBeg, Length);
		for (i = 0; i < BlockSize; i++)
		{
			(*Out)[Out_i] = m_Buf[m_BufBeg];
			Out_i++;
			m_BufBeg++;
		}
		Length -= BlockSize;
	}
}

size_t WCharReader::ReadString(wchar_t *Out, size_t MaxLength)
{
	uint BlockSize, i, Sum = 0;

	// MaxLength będzie zmniejszane.
	// Out będzie przesuwane.

	while (MaxLength > 0)
	{
		if (m_BufBeg == m_BufEnd)
		{
			if (!EnsureNewChars())
				return Sum;
		}
		BlockSize = std::min(m_BufEnd - m_BufBeg, MaxLength);
		for (i = 0; i < BlockSize; i++)
		{
			*Out = m_Buf[m_BufBeg];
			Out++;
			m_BufBeg++;
		}
		MaxLength -= BlockSize;
		Sum += BlockSize;
	}
	return Sum;
}

void WCharReader::MustReadString(wchar_t *Out, size_t Length)
{
	uint BlockSize, i;

	// Length będzie zmniejszane.
	// Out będzie przesuwane.

	while (Length > 0)
	{
		if (m_BufBeg == m_BufEnd)
		{
			if (!EnsureNewChars())
				_ThrowBufEndError(__TFILE__, __LINE__);
		}
		BlockSize = std::min(m_BufEnd - m_BufBeg, Length);
		for (i = 0; i < BlockSize; i++)
		{
			*Out = m_Buf[m_BufBeg];
			Out++;
			m_BufBeg++;
		}
		Length -= BlockSize;
	}
}

size_t WCharReader::Skip(size_t MaxLength)
{
	uint BlockSize, Sum = 0;

	// MaxLength będzie zmniejszane.

	while (MaxLength > 0)
	{
		if (m_BufBeg == m_BufEnd)
		{
			if (!EnsureNewChars())
				return Sum;
		}
		BlockSize = std::min(m_BufEnd - m_BufBeg, MaxLength);
		m_BufBeg += BlockSize;
		MaxLength -= BlockSize;
		Sum += BlockSize;
	}
	return Sum;
}

void WCharReader::MustSkip(size_t Length)
{
	uint BlockSize;

	// Length będzie zmniejszane.

	while (Length > 0)
	{
		if (m_BufBeg == m_BufEnd)
		{
			if (!EnsureNewChars())
				_ThrowBufEndError(__TFILE__, __LINE__);
		}
		BlockSize = std::min(m_BufEnd - m_BufBeg, Length);
		m_BufBeg += BlockSize;
		Length -= BlockSize;
	}
}

bool WCharReader::ReadLine(wstring *Out)
{
	Out->clear();
	wchar_t Ch;
	bool WasEol = false;
	while (ReadChar(&Ch))
	{
		if (Ch == L'\r')
		{
			WasEol = true;
			if (PeekChar(&Ch) && Ch == L'\n')
				MustSkipChar();
			break;
		}
		else if (Ch == L'\n')
		{
			WasEol = true;
			break;
		}
		else
			(*Out) += Ch;
	}
	return !Out->empty() || WasEol;
}

void WCharReader::MustReadLine(wstring *Out)
{
	Out->clear();
	wchar_t Ch;
	while (ReadChar(&Ch))
	{
		if (Ch == L'\r')
		{
			if (PeekChar(&Ch) && Ch == L'\n')
				MustSkipChar();
			break;
		}
		else if (Ch == L'\n')
			break;
		else
			(*Out) += Ch;
	}
}

#endif

//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Klasa MemoryStream

MemoryStream::MemoryStream(size_t Size, void *Data)
{
	m_Data = (char*)Data;
	m_Size = Size;
	m_Pos = 0;
	m_InternalAlloc = (m_Data == 0);

	if (m_InternalAlloc)
		m_Data = new char[m_Size];
}

MemoryStream::~MemoryStream()
{
	if (m_InternalAlloc)
		delete [] m_Data;
}

void MemoryStream::Write(const void *Data, size_t Size)
{
	if (m_Pos >= 0 && m_Pos + (int)Size <= (int)m_Size)
	{
		memcpy(&m_Data[m_Pos], Data, Size);
		m_Pos += (int)Size;
	}
	else
		throw Error(Format(_T("Cannot write # bytes to memory stream - position out of range (pos: #, size: #)")) % Size % m_Pos % m_Size, __TFILE__, __LINE__);
}

size_t MemoryStream::Read(void *Data, size_t Size)
{
	if (m_Pos >= 0 && m_Pos <= (int)m_Size)
	{
		Size = std::min(Size, m_Size-m_Pos);
		memcpy(Data, &m_Data[m_Pos], Size);
		m_Pos += (int)Size;
	}
	else
		throw Error(Format(_T("Cannot read # bytes from memory stream - position out of range (pos: #, size: #)")) % Size % m_Pos % m_Size, __TFILE__, __LINE__);
	return Size;
}

void MemoryStream::MustRead(void *Data, size_t Size)
{
	if (m_Pos >= 0 && m_Pos + (int)Size <= (int)m_Size)
	{
		memcpy(Data, &m_Data[m_Pos], Size);
		m_Pos += (int)Size;
	}
	else
		throw Error(Format(_T("Cannot read (2) # bytes from memory stream - position out of range (pos: #, size: #)")) % Size % m_Pos % m_Size, __TFILE__, __LINE__);
}

size_t MemoryStream::GetSize()
{
	return m_Size;
}

int MemoryStream::GetPos()
{
	return m_Pos;
}

void MemoryStream::SetPos(int pos)
{
	m_Pos = pos;
}

void MemoryStream::Rewind()
{
	m_Pos = 0;
}


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Klasa VectorStream

void VectorStream::Reserve(size_t NewCapacity)
{
	m_Capacity = NewCapacity;
	char *NewData = new char[m_Capacity];
	if (m_Size)
		memcpy(NewData, m_Data, m_Size);
	delete [] m_Data;
	m_Data = NewData;
}

VectorStream::VectorStream()
{
	m_Size = 0;
	m_Capacity = 8;
	m_Data = new char[8];
	m_Pos = 0;
}

VectorStream::~VectorStream()
{
	delete [] m_Data;
}

void VectorStream::Write(const void *Data, size_t Size)
{
	if (m_Capacity < m_Pos + Size)
		Reserve(std::max(m_Pos + Size, m_Capacity + m_Capacity / 4));
	if (m_Size < m_Pos + Size)
		m_Size = m_Pos + Size;
	memcpy(&m_Data[m_Pos], Data, Size);
	m_Pos += (int)Size;
}

size_t VectorStream::Read(void *Data, size_t Size)
{
	if (m_Pos >= 0 && m_Pos <= (int)m_Size)
	{
		Size = std::min(Size, m_Size-m_Pos);
		memcpy(Data, &m_Data[m_Pos], Size);
		m_Pos += (int)Size;
	}
	else
		throw Error(Format(_T("Cannot read # bytes from VectorStream stream - position out of range (pos: #, size: #)")) % Size % m_Pos % m_Size, __TFILE__, __LINE__);
	return Size;
}

void VectorStream::MustRead(void *Data, size_t Size)
{
	if (m_Pos >= 0 && m_Pos + (int)Size <= (int)m_Size)
	{
		memcpy(Data, &m_Data[m_Pos], Size);
		m_Pos += (int)Size;
	}
	else
		throw Error(Format(_T("Cannot read (2) # bytes from VectorStream stream - position out of range (pos: #, size: #)")) % Size % m_Pos % m_Size, __TFILE__, __LINE__);
}

void VectorStream::SetSize(size_t Size)
{
	// zmniejszamy
	if (Size < m_Size)
	{
		m_Size = Size;
		// jeśli połowa miejsca się marnuje - pomniejsz
		if (m_Capacity >= Size*2)
		{
			size_t NewCapacity = std::max((size_t)8, m_Size);
			if (NewCapacity != m_Capacity)
				Reserve(NewCapacity);
		}
	}
	// zwiększamy
	else if (Size > m_Size)
	{
		// jeśli brakuje miejsca - powiększ
		if (Size > m_Capacity)
			Reserve(std::max(Size, m_Capacity + m_Capacity / 4));
		m_Size = Size;
	}
}

void VectorStream::SetCapacity(size_t Capacity)
{
	if (Capacity < m_Size || Capacity == 0)
		throw Error(Format(_T("Cannot change capacity of VectorStream stream (size: #, desired capacity: #).")) % m_Size % Capacity, __TFILE__, __LINE__);
	
	if (Capacity != m_Capacity)
		Reserve(Capacity);
}


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Klasa StringStream

StringStream::StringStream(string *Data)
{
	m_Data = Data;
	m_Pos = 0;
	m_InternalAlloc = (m_Data == 0);

	if (m_InternalAlloc)
		m_Data = new string;
}

StringStream::~StringStream()
{
	if (m_InternalAlloc)
		delete m_Data;
}

void StringStream::Write(const void *Data, size_t Size)
{
	if (m_Pos + Size > m_Data->length())
		m_Data->resize(m_Pos + Size);

	for (size_t i = 0; i < Size; i++)
		(*m_Data)[m_Pos+i] = reinterpret_cast<const char*>(Data)[i];

	m_Pos += (int)Size;
}

size_t StringStream::Read(void *Data, size_t Size)
{
	if (m_Pos < 0)
		throw Error(Format(_T("Cannot read # bytes to StringStream - position out of range (pos: #, size: #)")) % Size % m_Pos % m_Data->length(), __TFILE__, __LINE__);

	Size = std::min(Size, m_Data->length()-m_Pos);
	for (size_t i = 0; i < Size; i++)
	{
		reinterpret_cast<char*>(Data)[i] = (*m_Data)[m_Pos];
		m_Pos++;
	}
	return Size;
}

void StringStream::MustRead(void *Data, size_t Size)
{
	if (m_Pos < 0 || m_Pos + Size > m_Data->length())
		throw Error(Format(_T("Cannot read (2) # bytes from StringStream - position out of range (pos: #, size: #)")) % Size % m_Pos % m_Data->length(), __TFILE__, __LINE__);

	for (size_t i = 0; i < Size; i++)
		reinterpret_cast<char*>(Data)[i] = (*m_Data)[m_Pos+i];

	m_Pos += (int)Size;
}


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Klasa CounterOverlayStream

CounterOverlayStream::CounterOverlayStream(Stream *a_Stream) :
	OverlayStream(a_Stream),
	m_WriteCounter(0),
	m_ReadCounter(0)
{
}

void CounterOverlayStream::Write(const void *Data, size_t Size)
{
	GetStream()->Write(Data, Size);
	m_WriteCounter += Size;
}

size_t CounterOverlayStream::Read(void *Data, size_t Size)
{
	size_t BytesRead = GetStream()->Read(Data, Size);
	m_ReadCounter += BytesRead;
	return BytesRead;
}

void CounterOverlayStream::MustRead(void *Data, size_t Size)
{
	GetStream()->MustRead(Data, Size);
	m_ReadCounter += Size;
}

void CounterOverlayStream::Flush()
{
	GetStream()->Flush();
}


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Klasa LimitOverlayStream

const tchar * const LIMIT_OVERLAY_WRITE_ERRMSG = _T("LimitOverlayStream: Write limit exceeded.");

LimitOverlayStream::LimitOverlayStream(Stream *a_Stream, uint32 WriteLimit, uint32 ReadLimit) :
	OverlayStream(a_Stream),
	m_WriteLimit(WriteLimit),
	m_ReadLimit(ReadLimit)
{
}

void LimitOverlayStream::Write(const void *Data, size_t Size)
{
	if (m_WriteLimit == 0)
		throw Error(LIMIT_OVERLAY_WRITE_ERRMSG);
	else if (Size <= m_WriteLimit)
	{
		GetStream()->Write(Data, Size);
		m_WriteLimit -= Size;
	}
	else
	{
		GetStream()->Write(Data, m_WriteLimit);
		m_WriteLimit = 0;
		throw Error(LIMIT_OVERLAY_WRITE_ERRMSG);
	}
}

size_t LimitOverlayStream::Read(void *Data, size_t Size)
{
	if (m_ReadLimit == 0)
		return 0;
	else if (Size <= m_ReadLimit)
	{
		size_t BytesRead = GetStream()->Read(Data, Size);
		m_ReadLimit -= BytesRead;
		return BytesRead;
	}
	else
	{
		size_t BytesRead = GetStream()->Read(Data, m_ReadLimit);
		m_ReadLimit -= BytesRead;
		return BytesRead;
	}
}

void LimitOverlayStream::Flush()
{
	GetStream()->Flush();
}

void LimitOverlayStream::SetWriteLimit(uint32 WriteLimit)
{
	m_WriteLimit = WriteLimit;
}

void LimitOverlayStream::SetReadLimit(uint32 ReadLimit)
{
	m_ReadLimit = ReadLimit;
}


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// class BufferingStream

BufferingStream::BufferingStream(Stream *stream, uint readBufSize, uint writeBufSize)
: OverlayStream(stream)
, m_ReadBufSize(readBufSize)
, m_WriteBufSize(writeBufSize)
, m_ReadBuf(readBufSize)
, m_WriteBuf(writeBufSize)
, m_ReadBufBeg(0)
, m_ReadBufEnd(0)
, m_WriteBufIndex(0)
{

}

BufferingStream::~BufferingStream()
{
	if (m_WriteBufSize > 0)
	{
		try
		{
			Flush();
		}
		catch (...)
		{
			assert(0 && "Exception caught in BufferingStream::~BufferingStream while calling BufferingStream::Flush.");
		}
	}
}

void BufferingStream::Write(const void *Data, size_t Size)
{
	if (m_WriteBufSize > 0)
	{
		const char *CharData = (const char*)Data;
		uint BlockLength;
		// Size będzie zmniejszany. Oznacza liczbę pozostałych do zapisania bajtów.
		// CharData będzie przesuwany.

		while (Size > 0)
		{
			// Bufor pełny
			if (m_WriteBufIndex == m_WriteBufSize)
				DoFlush();
			// Liczba bajtów do dopisania do bufora
			BlockLength = std::min(m_WriteBufSize - m_WriteBufIndex, Size);
			// Dopisanie do bufora
			common_memcpy(&m_WriteBuf[m_WriteBufIndex], CharData, BlockLength);

			CharData += BlockLength;
			m_WriteBufIndex += BlockLength;
			Size -= BlockLength;
		}
	}
	else
		GetStream()->Write(Data, Size);
}

void BufferingStream::Flush()
{
	if (m_WriteBufSize > 0)
		DoFlush();
	GetStream()->Flush();
}

size_t BufferingStream::Read(void *Data, size_t MaxLength)
{
	if (m_ReadBufSize > 0)
	{
		char *OutChars = (char*)Data;
		uint BlockSize, Sum = 0;

		// MaxLength będzie zmniejszane.
		// OutChars będzie przesuwane.
		while (MaxLength > 0)
		{
			if (m_ReadBufBeg == m_ReadBufEnd)
			{
				if (!EnsureNewChars())
					return Sum;
			}
			BlockSize = std::min(m_ReadBufEnd - m_ReadBufBeg, MaxLength);
			common_memcpy(OutChars, &m_ReadBuf[m_ReadBufBeg], BlockSize);
			OutChars += BlockSize;
			m_ReadBufBeg += BlockSize;
			MaxLength -= BlockSize;
			Sum += BlockSize;
		}
		return Sum;
	}
	else
		return GetStream()->Read(Data, MaxLength);
}

bool BufferingStream::End()
{
	if (m_ReadBufSize > 0)
	{
		if (m_ReadBufBeg != m_ReadBufEnd)
			return false;
	}
	return GetStream()->End();
}

void BufferingStream::DoFlush()
{
	GetStream()->Write(&m_WriteBuf[0], m_WriteBufIndex);
	m_WriteBufIndex = 0;
}

bool BufferingStream::EnsureNewChars()
{
	assert(m_ReadBufBeg == m_ReadBufEnd);
	uint ReadSize = GetStream()->Read(&m_ReadBuf[0], m_ReadBufSize);
	m_ReadBufBeg = 0;
	m_ReadBufEnd = ReadSize;
	return (ReadSize > 0);
}

size_t BufferingStream::Skip( size_t MaxLength )
{
	uint BlockSize, Sum = 0;
	// MaxLength będzie zmniejszane.
	while (MaxLength > 0)
	{
		if (m_ReadBufBeg == m_ReadBufEnd)
		{
			if (!EnsureNewChars())
				return Sum;
		}
		BlockSize = std::min(m_ReadBufEnd - m_ReadBufBeg, MaxLength);
		m_ReadBufBeg += BlockSize;
		MaxLength -= BlockSize;
		Sum += BlockSize;
	}
	return Sum;
}


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Klasa MultiWriterStream

MultiWriterStream::MultiWriterStream(Stream *a_Streams[], uint StreamCount)
{
	m_Streams.resize(StreamCount);
	for (uint i = 0; i < StreamCount; i++)
		m_Streams[i] = a_Streams[i];
}

void MultiWriterStream::RemoveStream(Stream *a_Stream)
{
	for (std::vector<Stream*>::iterator it = m_Streams.begin(); it != m_Streams.end(); ++it)
	{
		if (*it == a_Stream)
		{
			m_Streams.erase(it);
			break;
		}
	}
}

void MultiWriterStream::Write(const void *Data, size_t Size)
{
	ERR_TRY;

	for (uint i = 0; i < m_Streams.size(); i++)
		m_Streams[i]->Write(Data, Size);

	ERR_CATCH_FUNC;
}

void MultiWriterStream::Flush()
{
	ERR_TRY;

	for (uint i = 0; i < m_Streams.size(); i++)
		m_Streams[i]->Flush();

	ERR_CATCH_FUNC;
}


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Klasa Hash_Calc

void Hash_Calc::Write(const void *Data, size_t Size)
{
	uint8 *ByteData = (uint8*)Data; // będzie przesuwany

	for (size_t i = 0; i < Size; i++)
	{
		m_Hash += *ByteData;
		m_Hash += (m_Hash << 10);
		m_Hash ^= (m_Hash >> 6);

		ByteData++;
	}
}

uint32 Hash_Calc::Finish()
{
	m_Hash += (m_Hash << 3);
	m_Hash ^= (m_Hash >> 11);
	m_Hash += (m_Hash << 15);

	return m_Hash;
}

uint32 Hash_Calc::Calc(const void *Buf, uint32 BufLen)
{
	uint32 Hash = 0;

	uint8 *ByteData = (uint8*)Buf; // będzie przesuwany

	for (size_t i = 0; i < BufLen; i++)
	{
		Hash += *ByteData;
		Hash += (Hash << 10);
		Hash ^= (Hash >> 6);

		ByteData++;
	}

	Hash += (Hash << 3);
	Hash ^= (Hash >> 11);
	Hash += (Hash << 15);

	return Hash;
}

uint32 Hash_Calc::Calc(const string &s)
{
	uint32 Hash = 0;

	for (size_t i = 0; i < s.length(); i++)
	{
		Hash += (uint8)s[i];
		Hash += (Hash << 10);
		Hash ^= (Hash >> 6);
	}

	Hash += (Hash << 3);
	Hash ^= (Hash >> 11);
	Hash += (Hash << 15);

	return Hash;
}

#ifdef WIN32
// To wprawdzie jest algorytm do skutecznego hashowania stringów typu single-byte,
// a ja go tutaj użyję do Unicode, ale co tam...
uint32 Hash_Calc::Calc(const wstring &s)
{
	uint32 Hash = 0;

	for (size_t i = 0; i < s.length(); i++)
	{
		Hash += s[i];
		Hash += (Hash << 10);
		Hash ^= (Hash >> 6);
	}

	Hash += (Hash << 3);
	Hash ^= (Hash >> 11);
	Hash += (Hash << 15);

	return Hash;
}
#endif


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Klasa CRC32_Calc

/*
CRC32 - 32bit Cyclic Redundancy Check
Sprawdzona, działa dobrze, zgodnie ze standardem.
Na podstawie:
  "eXtensible Data Stream 3.0", Mark T. Price
  Appendix B. Reference CRC-32 Implementation
*/

const uint32 CRC32_TABLE[256] = {
	0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA, 0x076DC419, 0x706AF48F,
	0xE963A535, 0x9E6495A3, 0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988,
	0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91, 0x1DB71064, 0x6AB020F2,
	0xF3B97148, 0x84BE41DE, 0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
	0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC, 0x14015C4F, 0x63066CD9,
	0xFA0F3D63, 0x8D080DF5, 0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172,
	0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B, 0x35B5A8FA, 0x42B2986C,
	0xDBBBC9D6, 0xACBCF940, 0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
	0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116, 0x21B4F4B5, 0x56B3C423,
	0xCFBA9599, 0xB8BDA50F, 0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,
	0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D, 0x76DC4190, 0x01DB7106,
	0x98D220BC, 0xEFD5102A, 0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
	0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818, 0x7F6A0DBB, 0x086D3D2D,
	0x91646C97, 0xE6635C01, 0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E,
	0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457, 0x65B0D9C6, 0x12B7E950,
	0x8BBEB8EA, 0xFCB9887C, 0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
	0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2, 0x4ADFA541, 0x3DD895D7,
	0xA4D1C46D, 0xD3D6F4FB, 0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0,
	0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9, 0x5005713C, 0x270241AA,
	0xBE0B1010, 0xC90C2086, 0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
	0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4, 0x59B33D17, 0x2EB40D81,
	0xB7BD5C3B, 0xC0BA6CAD, 0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A,
	0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683, 0xE3630B12, 0x94643B84,
	0x0D6D6A3E, 0x7A6A5AA8, 0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
	0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE, 0xF762575D, 0x806567CB,
	0x196C3671, 0x6E6B06E7, 0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC,
	0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5, 0xD6D6A3E8, 0xA1D1937E,
	0x38D8C2C4, 0x4FDFF252, 0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
	0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60, 0xDF60EFC3, 0xA867DF55,
	0x316E8EEF, 0x4669BE79, 0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
	0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F, 0xC5BA3BBE, 0xB2BD0B28,
	0x2BB45A92, 0x5CB36A04, 0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
	0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A, 0x9C0906A9, 0xEB0E363F,
	0x72076785, 0x05005713, 0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38,
	0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21, 0x86D3D2D4, 0xF1D4E242,
	0x68DDB3F8, 0x1FDA836E, 0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
	0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C, 0x8F659EFF, 0xF862AE69,
	0x616BFFD3, 0x166CCF45, 0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2,
	0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB, 0xAED16A4A, 0xD9D65ADC,
	0x40DF0B66, 0x37D83BF0, 0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
	0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6, 0xBAD03605, 0xCDD70693,
	0x54DE5729, 0x23D967BF, 0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94,
	0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
};

void CRC32_Calc::Write(const void *Data, size_t Size)
{
	uint32 k;
	uint8 *DataBytes = (uint8*)Data;
	while (Size > 0)
	{
		k = (m_CRC ^ (*DataBytes)) & 0x000000FF;
		m_CRC = ((m_CRC >> 8) & 0x00FFFFFF) ^ CRC32_TABLE[k];

		DataBytes++;
		Size--;
	}
}

uint CRC32_Calc::Calc(const void *Data, size_t DataLength)
{
	uint32 crc = 0xFFFFFFFF; // preconditioning sets non zero value
	
	// loop through the buffer and calculate CRC
	for(uint32 i = 0; i < DataLength; ++i)
	{
		int k = (crc ^ ((uint8*)(Data))[i]) & 0x000000FF;
		crc = ((crc >> 8) & 0x00FFFFFF) ^ CRC32_TABLE[k];
	}

	crc = ~crc; // postconditioning

	// report results
	return crc;
}


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Struktura MD5_SUM, klasa MD5_Calc itp.

/*
MD5 - Message-Digest algorithm 5
Sprawdzona, działa dobrze, zgodnie ze standardem.
Implementacja na podstawie biblioteki:
  RFC 1321 compliant MD5 implementation
  Copyright (C) 2003-2006  Christophe Devine
  Licencja: GNU LGPL 2.1
  http://xyssl.org/code/source/md5/
*/

bool MD5_SUM::operator < (const MD5_SUM &s) const
{
	return memcmp(Data, s.Data, 16) < 0;
}

bool MD5_SUM::operator > (const MD5_SUM &s) const
{
	return memcmp(Data, s.Data, 16) > 0;
}

bool MD5_SUM::operator <= (const MD5_SUM &s) const
{
	return memcmp(Data, s.Data, 16) <= 0;
}

bool MD5_SUM::operator >= (const MD5_SUM &s) const
{
	return memcmp(Data, s.Data, 16) >= 0;
}

void MD5ToStr(tstring *Out, const MD5_SUM &MD5)
{
	HexEncoder::Encode(Out, MD5.Data, 16);
}

bool StrToMD5(MD5_SUM *Out, const tstring &s)
{
	return ( HexDecoder::Decode(Out->Data, s) == 16 );
}

#define MD5_GET_UINT32_LE(n,b,i)                \
{                                               \
    (n) = ( (uint32) (b)[(i)    ]       )        \
        | ( (uint32) (b)[(i) + 1] <<  8 )        \
        | ( (uint32) (b)[(i) + 2] << 16 )        \
        | ( (uint32) (b)[(i) + 3] << 24 );       \
}

#define MD5_PUT_UINT32_LE(n,b,i)                \
{                                               \
    (b)[(i)    ] = (uint8) ( (n)       );       \
    (b)[(i) + 1] = (uint8) ( (n) >>  8 );       \
    (b)[(i) + 2] = (uint8) ( (n) >> 16 );       \
    (b)[(i) + 3] = (uint8) ( (n) >> 24 );       \
}

uint8 md5_padding[64] =
{
 0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

void MD5_Calc::Process(uint8 data[64])
{
    uint32 X[16], A, B, C, D;

    MD5_GET_UINT32_LE( X[0],  data,  0 );
    MD5_GET_UINT32_LE( X[1],  data,  4 );
    MD5_GET_UINT32_LE( X[2],  data,  8 );
    MD5_GET_UINT32_LE( X[3],  data, 12 );
    MD5_GET_UINT32_LE( X[4],  data, 16 );
    MD5_GET_UINT32_LE( X[5],  data, 20 );
    MD5_GET_UINT32_LE( X[6],  data, 24 );
    MD5_GET_UINT32_LE( X[7],  data, 28 );
    MD5_GET_UINT32_LE( X[8],  data, 32 );
    MD5_GET_UINT32_LE( X[9],  data, 36 );
    MD5_GET_UINT32_LE( X[10], data, 40 );
    MD5_GET_UINT32_LE( X[11], data, 44 );
    MD5_GET_UINT32_LE( X[12], data, 48 );
    MD5_GET_UINT32_LE( X[13], data, 52 );
    MD5_GET_UINT32_LE( X[14], data, 56 );
    MD5_GET_UINT32_LE( X[15], data, 60 );

#define S(x,n) ((x << n) | ((x & 0xFFFFFFFF) >> (32 - n)))

#define P(a,b,c,d,k,s,t)                                \
{                                                       \
    a += F(b,c,d) + X[k] + t; a = S(a,s) + b;           \
}

    A = state[0];
    B = state[1];
    C = state[2];
    D = state[3];

#define F(x,y,z) (z ^ (x & (y ^ z)))
    P( A, B, C, D,  0,  7, 0xD76AA478 );
    P( D, A, B, C,  1, 12, 0xE8C7B756 );
    P( C, D, A, B,  2, 17, 0x242070DB );
    P( B, C, D, A,  3, 22, 0xC1BDCEEE );
    P( A, B, C, D,  4,  7, 0xF57C0FAF );
    P( D, A, B, C,  5, 12, 0x4787C62A );
    P( C, D, A, B,  6, 17, 0xA8304613 );
    P( B, C, D, A,  7, 22, 0xFD469501 );
    P( A, B, C, D,  8,  7, 0x698098D8 );
    P( D, A, B, C,  9, 12, 0x8B44F7AF );
    P( C, D, A, B, 10, 17, 0xFFFF5BB1 );
    P( B, C, D, A, 11, 22, 0x895CD7BE );
    P( A, B, C, D, 12,  7, 0x6B901122 );
    P( D, A, B, C, 13, 12, 0xFD987193 );
    P( C, D, A, B, 14, 17, 0xA679438E );
    P( B, C, D, A, 15, 22, 0x49B40821 );
#undef F

#define F(x,y,z) (y ^ (z & (x ^ y)))
    P( A, B, C, D,  1,  5, 0xF61E2562 );
    P( D, A, B, C,  6,  9, 0xC040B340 );
    P( C, D, A, B, 11, 14, 0x265E5A51 );
    P( B, C, D, A,  0, 20, 0xE9B6C7AA );
    P( A, B, C, D,  5,  5, 0xD62F105D );
    P( D, A, B, C, 10,  9, 0x02441453 );
    P( C, D, A, B, 15, 14, 0xD8A1E681 );
    P( B, C, D, A,  4, 20, 0xE7D3FBC8 );
    P( A, B, C, D,  9,  5, 0x21E1CDE6 );
    P( D, A, B, C, 14,  9, 0xC33707D6 );
    P( C, D, A, B,  3, 14, 0xF4D50D87 );
    P( B, C, D, A,  8, 20, 0x455A14ED );
    P( A, B, C, D, 13,  5, 0xA9E3E905 );
    P( D, A, B, C,  2,  9, 0xFCEFA3F8 );
    P( C, D, A, B,  7, 14, 0x676F02D9 );
    P( B, C, D, A, 12, 20, 0x8D2A4C8A );
#undef F
    
#define F(x,y,z) (x ^ y ^ z)
    P( A, B, C, D,  5,  4, 0xFFFA3942 );
    P( D, A, B, C,  8, 11, 0x8771F681 );
    P( C, D, A, B, 11, 16, 0x6D9D6122 );
    P( B, C, D, A, 14, 23, 0xFDE5380C );
    P( A, B, C, D,  1,  4, 0xA4BEEA44 );
    P( D, A, B, C,  4, 11, 0x4BDECFA9 );
    P( C, D, A, B,  7, 16, 0xF6BB4B60 );
    P( B, C, D, A, 10, 23, 0xBEBFBC70 );
    P( A, B, C, D, 13,  4, 0x289B7EC6 );
    P( D, A, B, C,  0, 11, 0xEAA127FA );
    P( C, D, A, B,  3, 16, 0xD4EF3085 );
    P( B, C, D, A,  6, 23, 0x04881D05 );
    P( A, B, C, D,  9,  4, 0xD9D4D039 );
    P( D, A, B, C, 12, 11, 0xE6DB99E5 );
    P( C, D, A, B, 15, 16, 0x1FA27CF8 );
    P( B, C, D, A,  2, 23, 0xC4AC5665 );
#undef F

#define F(x,y,z) (y ^ (x | ~z))
    P( A, B, C, D,  0,  6, 0xF4292244 );
    P( D, A, B, C,  7, 10, 0x432AFF97 );
    P( C, D, A, B, 14, 15, 0xAB9423A7 );
    P( B, C, D, A,  5, 21, 0xFC93A039 );
    P( A, B, C, D, 12,  6, 0x655B59C3 );
    P( D, A, B, C,  3, 10, 0x8F0CCC92 );
    P( C, D, A, B, 10, 15, 0xFFEFF47D );
    P( B, C, D, A,  1, 21, 0x85845DD1 );
    P( A, B, C, D,  8,  6, 0x6FA87E4F );
    P( D, A, B, C, 15, 10, 0xFE2CE6E0 );
    P( C, D, A, B,  6, 15, 0xA3014314 );
    P( B, C, D, A, 13, 21, 0x4E0811A1 );
    P( A, B, C, D,  4,  6, 0xF7537E82 );
    P( D, A, B, C, 11, 10, 0xBD3AF235 );
    P( C, D, A, B,  2, 15, 0x2AD7D2BB );
    P( B, C, D, A,  9, 21, 0xEB86D391 );
#undef F

#undef P
#undef S

    state[0] += A;
    state[1] += B;
    state[2] += C;
    state[3] += D;
}

MD5_Calc::MD5_Calc()
{
	Reset();
}

void MD5_Calc::Write(const void *Buf, size_t BufLen)
{
	uint8 *ByteBuf = (uint8*)Buf;

	int fill;
	uint32 left;

	if(BufLen == 0)
		return;

	left = total[0] & 0x3F;
	fill = 64 - left;

	total[0] += BufLen;
	total[0] &= 0xFFFFFFFF;

	if(total[0] < BufLen)
		total[1]++;

	if(left && (int)BufLen >= fill)
	{
		memcpy((void*)(buffer + left), (void*)ByteBuf, fill);
		Process(buffer);
		ByteBuf += fill;
		BufLen -= fill;
		left = 0;
	}

	while(BufLen >= 64)
	{
		Process(ByteBuf);
		ByteBuf += 64;
		BufLen -= 64;
	}

	if(BufLen > 0)
		memcpy((void*)(buffer + left), (void*)ByteBuf, BufLen);
}

void MD5_Calc::Finish(MD5_SUM *Out)
{
	uint32 last, padn;
	uint32 high, low;
	uint8 msglen[8];

	high = (total[0] >> 29) | (total[1] <<  3);
	low  = (total[0] <<  3);

	MD5_PUT_UINT32_LE(low,  msglen, 0);
	MD5_PUT_UINT32_LE(high, msglen, 4);

	last = total[0] & 0x3F;
	padn = (last < 56) ? (56 - last) : (120 - last);

	Write(md5_padding, padn);
	Write(msglen, 8);

	MD5_PUT_UINT32_LE(state[0], Out->Data,  0);
	MD5_PUT_UINT32_LE(state[1], Out->Data,  4);
	MD5_PUT_UINT32_LE(state[2], Out->Data,  8);
	MD5_PUT_UINT32_LE(state[3], Out->Data, 12);
}

void MD5_Calc::Reset()
{
	total[0] = 0;
	total[1] = 0;
	state[0] = 0x67452301;
	state[1] = 0xEFCDAB89;
	state[2] = 0x98BADCFE;
	state[3] = 0x10325476;
}

void MD5_Calc::Calc(MD5_SUM *Out, const void *Buf, uint32 BufLen)
{
	MD5_Calc md5;
	md5.Write(Buf, BufLen);
	md5.Finish(Out);
}

#undef MD5_PUT_UINT32_LE
#undef MD5_GET_UINT32_LE


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Klasa XorCoder

XorCoder::XorCoder(Stream *a_Stream, char KeyByte) :
	OverlayStream(a_Stream),
	m_EncodeKeyIndex(0),
	m_DecodeKeyIndex(0)
{
	m_Buf.resize(BUFFER_SIZE);

	m_Key.push_back(KeyByte);
}

XorCoder::XorCoder(Stream *a_Stream, const void *KeyData, size_t KeyLength) :
	OverlayStream(a_Stream),
	m_EncodeKeyIndex(0),
	m_DecodeKeyIndex(0)
{
	m_Buf.resize(BUFFER_SIZE);

	m_Key.resize(KeyLength);
	memcpy(&m_Key[0], KeyData, KeyLength);
}

XorCoder::XorCoder(Stream *a_Stream, const string &Key) :
	OverlayStream(a_Stream),
	m_EncodeKeyIndex(0),
	m_DecodeKeyIndex(0)
{
	m_Buf.resize(BUFFER_SIZE);

	m_Key.resize(Key.length());
	for (uint i = 0; i < Key.length(); i++)
		m_Key[i] = Key[i];
}

void XorCoder::Write(const void *Data, size_t Size)
{
	ERR_TRY;

	const char *Bytes = (const char*)Data;
	uint BlockSize;

	while (Size > 0)
	{
		BlockSize = std::min(Size, BUFFER_SIZE);

		if (m_Key.size() == 1)
		{
			for (size_t i = 0; i < BlockSize; i++)
				m_Buf[i] = Bytes[i] ^ m_Key[0];
		}
		else
		{
			for (size_t i = 0; i < BlockSize; i++)
			{
				m_Buf[i] = Bytes[i] ^ m_Key[m_EncodeKeyIndex];
				m_EncodeKeyIndex = (m_EncodeKeyIndex + 1) % m_Key.size();
			}
		}

		GetStream()->Write(&m_Buf[0], BlockSize);

		Bytes += BlockSize;
		Size -= BlockSize;
	}

	ERR_CATCH_FUNC;
}

size_t XorCoder::Read(void *Data, size_t Size)
{
	ERR_TRY;

	char * Bytes = (char*)Data;
	uint BlockSize, ReadSize, Sum = 0;

	while (Size > 0)
	{
		BlockSize = std::min(Size, BUFFER_SIZE);
		
		ReadSize = GetStream()->Read(&m_Buf[0], BlockSize);
		
		if (m_Key.size() == 1)
		{
			for (size_t i = 0; i < BlockSize; i++)
				Bytes[i] = m_Buf[i] ^ m_Key[0];
		}
		else
		{
			for (size_t i = 0; i < BlockSize; i++)
			{
				Bytes[i] = m_Buf[i] ^ m_Key[m_DecodeKeyIndex];
				m_DecodeKeyIndex = (m_DecodeKeyIndex + 1) % m_Key.size();
			}
		}

		Sum += ReadSize;
		if (ReadSize < BlockSize)
			return Sum;
		Bytes += ReadSize;
		Size -= ReadSize;
	}

	return Sum;

	ERR_CATCH_FUNC;
}

void XorCoder::Code(void *Out, const void *Data, size_t DataLength, const void *Key, size_t KeyLength)
{
	char *OutB = (char*)Out;
	const char *DataB = (const char*)Data;
	const char *KeyB = (const char*)Key;

	if (KeyLength == 1)
	{
		for (size_t i = 0; i < DataLength; i++)
		{
			*OutB = *DataB ^ *KeyB;
			OutB++;
			DataB++;
		}
	}
	else
	{
		size_t KeyIndex = 0;
		for (size_t i = 0; i < DataLength; i++)
		{
			*OutB = *DataB ^ KeyB[KeyIndex];
			OutB++;
			DataB++;
			KeyIndex = (KeyIndex + 1) % KeyLength;
		}
	}
}

void XorCoder::Code(string *Out, const string &Data, const string &Key)
{
	Out->resize(Data.length());

	if (Key.length() == 1)
	{
		for (size_t i = 0; i < Data.length(); i++)
			(*Out)[i] = Data[i] ^ Key[0];
	}
	else
	{
		size_t KeyIndex = 0;
		for (size_t i = 0; i < Data.length(); i++)
		{
			(*Out)[i] = Data[i] ^ Key[KeyIndex];
			KeyIndex = (KeyIndex + 1) % Key.length();
		}
	}
}

#ifdef WIN32
void XorCoder::Code(wstring *Out, const wstring &Data, const wstring &Key)
{
	Out->resize(Data.length());

	if (Key.length() == 1)
	{
		for (size_t i = 0; i < Data.length(); i++)
			(*Out)[i] = Data[i] ^ Key[0];
	}
	else
	{
		size_t KeyIndex = 0;
		for (size_t i = 0; i < Data.length(); i++)
		{
			(*Out)[i] = Data[i] ^ Key[KeyIndex];
			KeyIndex = (KeyIndex + 1) % Key.length();
		}
	}
}
#endif


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Klasa BinEncoder

void BinEncoder::Write(const void *Data, size_t Size)
{
	ERR_TRY;

	const uint8 *Bytes = (const uint8*)Data;
	uint8 Byte;
	char Octet[8];

	for (uint i = 0; i < Size; i++)
	{
		Byte = *Bytes;
		Octet[0] = (Byte & 0x80) ? '1' : '0';
		Octet[1] = (Byte & 0x40) ? '1' : '0';
		Octet[2] = (Byte & 0x20) ? '1' : '0';
		Octet[3] = (Byte & 0x10) ? '1' : '0';
		Octet[4] = (Byte & 0x08) ? '1' : '0';
		Octet[5] = (Byte & 0x04) ? '1' : '0';
		Octet[6] = (Byte & 0x02) ? '1' : '0';
		Octet[7] = (Byte & 0x01) ? '1' : '0';
		m_CharWriter.WriteString(Octet, 8);
		Bytes++;
	}

	ERR_CATCH_FUNC;
}

void BinEncoder::Encode(char *Out, const void *Data, size_t DataLength)
{
	const uint8 *DataBytes = (const uint8*)Data;

	for (size_t i = 0; i < DataLength; i++)
	{
		*Out = (*DataBytes) & 0x80 ? '1' : '0'; Out++;
		*Out = (*DataBytes) & 0x40 ? '1' : '0'; Out++;
		*Out = (*DataBytes) & 0x20 ? '1' : '0'; Out++;
		*Out = (*DataBytes) & 0x10 ? '1' : '0'; Out++;
		*Out = (*DataBytes) & 0x08 ? '1' : '0'; Out++;
		*Out = (*DataBytes) & 0x04 ? '1' : '0'; Out++;
		*Out = (*DataBytes) & 0x02 ? '1' : '0'; Out++;
		*Out = (*DataBytes) & 0x01 ? '1' : '0'; Out++;
		DataBytes++;
	}
}

void BinEncoder::Encode(string *Out, const void *Data, size_t DataLength)
{
	const uint8 *DataBytes = (const uint8*)Data;
	Out->resize(DataLength*8);
	size_t Out_i = 0;

	for (size_t i = 0; i < DataLength; i++)
	{
		(*Out)[Out_i++] = (*DataBytes) & 0x80 ? '1' : '0';
		(*Out)[Out_i++] = (*DataBytes) & 0x40 ? '1' : '0';
		(*Out)[Out_i++] = (*DataBytes) & 0x20 ? '1' : '0';
		(*Out)[Out_i++] = (*DataBytes) & 0x10 ? '1' : '0';
		(*Out)[Out_i++] = (*DataBytes) & 0x08 ? '1' : '0';
		(*Out)[Out_i++] = (*DataBytes) & 0x04 ? '1' : '0';
		(*Out)[Out_i++] = (*DataBytes) & 0x02 ? '1' : '0';
		(*Out)[Out_i++] = (*DataBytes) & 0x01 ? '1' : '0';
		DataBytes++;
	}
}


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Klasa BinDecoder

BinDecoder::BinDecoder(Stream *a_Stream, DECODE_TOLERANCE Tolerance) :
	OverlayStream(a_Stream),
	m_CharReader(a_Stream),
	m_Tolerance(Tolerance)
{
}

size_t BinDecoder::Read(void *Out, size_t Size)
{
	// Size będzie zmniejszane.

	ERR_TRY;

	uint8 *OutBytes = (uint8*)Out;
	uint8 Byte;
	uint Sum = 0;
	char Ch;

	if (m_Tolerance == DECODE_TOLERANCE_NONE)
	{
		while (Size > 0)
		{
			// Pierwszy bit - może go nie być jeśli to koniec strumienia
			if (!m_CharReader.ReadChar(&Ch))
				break;
			if (Ch == '1')
				Byte = 0x80;
			else if (Ch == '0')
				Byte = 0x00;
			else
				throw Error(ERRMSG_DECODE_INVALID_CHAR, __TFILE__, __LINE__);

			// Następne 7 bitów
			Ch = m_CharReader.MustReadChar();
			if (Ch == '1') Byte |= 0x40; else if (Ch != '0') throw Error(ERRMSG_DECODE_INVALID_CHAR, __TFILE__, __LINE__);
			Ch = m_CharReader.MustReadChar();
			if (Ch == '1') Byte |= 0x20; else if (Ch != '0') throw Error(ERRMSG_DECODE_INVALID_CHAR, __TFILE__, __LINE__);
			Ch = m_CharReader.MustReadChar();
			if (Ch == '1') Byte |= 0x10; else if (Ch != '0') throw Error(ERRMSG_DECODE_INVALID_CHAR, __TFILE__, __LINE__);
			Ch = m_CharReader.MustReadChar();
			if (Ch == '1') Byte |= 0x08; else if (Ch != '0') throw Error(ERRMSG_DECODE_INVALID_CHAR, __TFILE__, __LINE__);
			Ch = m_CharReader.MustReadChar();
			if (Ch == '1') Byte |= 0x04; else if (Ch != '0') throw Error(ERRMSG_DECODE_INVALID_CHAR, __TFILE__, __LINE__);
			Ch = m_CharReader.MustReadChar();
			if (Ch == '1') Byte |= 0x02; else if (Ch != '0') throw Error(ERRMSG_DECODE_INVALID_CHAR, __TFILE__, __LINE__);
			Ch = m_CharReader.MustReadChar();
			if (Ch == '1') Byte |= 0x01; else if (Ch != '0') throw Error(ERRMSG_DECODE_INVALID_CHAR, __TFILE__, __LINE__);

			*OutBytes = Byte;
			OutBytes++;
			Sum++;
			Size--;
		}
	}
	else if (m_Tolerance == DECODE_TOLERANCE_WHITESPACE)
	{
		while (Size > 0)
		{
			// Pierwszy bit - może go nie być jeśli to koniec strumienia
			for (;;)
			{
				if (!m_CharReader.ReadChar(&Ch))
					goto break_2_WHITESPACE;
				if (Ch == '1')
				{
					Byte = 0x80;
					break;
				}
				else if (Ch == '0')
				{
					Byte = 0x00;
					break;
				}
				else if (CharIsWhitespace(Ch))
				{
					// Nic - nowy obieg tej pętli wewnętrznej
				}
				else
					throw Error(ERRMSG_DECODE_INVALID_CHAR, __TFILE__, __LINE__);
			}

			// Następne 7 bitów
			for (;;) {
				Ch = m_CharReader.MustReadChar();
				if (Ch == '1') { Byte |= 0x40; break; } else if (Ch == '0') break; else if (!CharIsWhitespace(Ch)) throw Error(ERRMSG_DECODE_INVALID_CHAR, __TFILE__, __LINE__);
			}
			for (;;) {
				Ch = m_CharReader.MustReadChar();
				if (Ch == '1') { Byte |= 0x20; break; } else if (Ch == '0') break; else if (!CharIsWhitespace(Ch)) throw Error(ERRMSG_DECODE_INVALID_CHAR, __TFILE__, __LINE__);
			}
			for (;;) {
				Ch = m_CharReader.MustReadChar();
				if (Ch == '1') { Byte |= 0x10; break; } else if (Ch == '0') break; else if (!CharIsWhitespace(Ch)) throw Error(ERRMSG_DECODE_INVALID_CHAR, __TFILE__, __LINE__);
			}
			for (;;) {
				Ch = m_CharReader.MustReadChar();
				if (Ch == '1') { Byte |= 0x08; break; } else if (Ch == '0') break; else if (!CharIsWhitespace(Ch)) throw Error(ERRMSG_DECODE_INVALID_CHAR, __TFILE__, __LINE__);
			}
			for (;;) {
				Ch = m_CharReader.MustReadChar();
				if (Ch == '1') { Byte |= 0x04; break; } else if (Ch == '0') break; else if (!CharIsWhitespace(Ch)) throw Error(ERRMSG_DECODE_INVALID_CHAR, __TFILE__, __LINE__);
			}
			for (;;) {
				Ch = m_CharReader.MustReadChar();
				if (Ch == '1') { Byte |= 0x02; break; } else if (Ch == '0') break; else if (!CharIsWhitespace(Ch)) throw Error(ERRMSG_DECODE_INVALID_CHAR, __TFILE__, __LINE__);
			}
			for (;;) {
				Ch = m_CharReader.MustReadChar();
				if (Ch == '1') { Byte |= 0x01; break; } else if (Ch == '0') break; else if (!CharIsWhitespace(Ch)) throw Error(ERRMSG_DECODE_INVALID_CHAR, __TFILE__, __LINE__);
			}

			*OutBytes = Byte;
			OutBytes++;
			Sum++;
			Size--;
		}
break_2_WHITESPACE:
		;
	}
	else // (m_Tolerance == DECODE_TOLERANCE_ALL)
	{
		while (Size > 0)
		{
			// Pierwszy bit - może go nie być jeśli to koniec strumienia
			for (;;)
			{
				if (!m_CharReader.ReadChar(&Ch))
					goto break_2_ALL;
				if (Ch == '1')
				{
					Byte = 0x80;
					break;
				}
				else if (Ch == '0')
				{
					Byte = 0x00;
					break;
				}
			}

			// Następne 7 bitów
			for (;;) {
				Ch = m_CharReader.MustReadChar();
				if (Ch == '1') { Byte |= 0x40; break; } else if (Ch == '0') break;
			}
			for (;;) {
				Ch = m_CharReader.MustReadChar();
				if (Ch == '1') { Byte |= 0x20; break; } else if (Ch == '0') break;
			}
			for (;;) {
				Ch = m_CharReader.MustReadChar();
				if (Ch == '1') { Byte |= 0x10; break; } else if (Ch == '0') break;
			}
			for (;;) {
				Ch = m_CharReader.MustReadChar();
				if (Ch == '1') { Byte |= 0x08; break; } else if (Ch == '0') break;
			}
			for (;;) {
				Ch = m_CharReader.MustReadChar();
				if (Ch == '1') { Byte |= 0x04; break; } else if (Ch == '0') break;
			}
			for (;;) {
				Ch = m_CharReader.MustReadChar();
				if (Ch == '1') { Byte |= 0x02; break; } else if (Ch == '0') break;
			}
			for (;;) {
				Ch = m_CharReader.MustReadChar();
				if (Ch == '1') { Byte |= 0x01; break; } else if (Ch == '0') break;
			}

			*OutBytes = Byte;
			OutBytes++;
			Sum++;
			Size--;
		}
break_2_ALL:
		;
	}

	return Sum;

	ERR_CATCH_FUNC;
}

bool BinDecoder::DecodeLength(uint *OutLength, const string &s, DECODE_TOLERANCE Tolerance)
{
	// Żadne dziwne znaki nie są tolerowane - po prostu zlicz znaki
	if (Tolerance == DECODE_TOLERANCE_NONE)
	{
		if ((s.length() & 0x07) != 0) // (s.length % 8) != 0
			return false;
		*OutLength = s.length() >> 3;
		return true;
	}
	// Pewne dodatkowe znaki są tolerowane - trzeba wszystkie przejrzeć i zliczyć same cyfry
	else
	{
		uint DigitCount = 0;
		for (uint i = 0; i < s.length(); i++)
		{
			if (s[i] == '1' || s[i] == '0')
				DigitCount++;
		}

		if ((DigitCount & 0x07) != 0) // (DigitCount % 8) != 0
			return false;
		*OutLength = DigitCount >> 3;
		return true;
	}
}

bool BinDecoder::DecodeLength(uint *OutLength, const char *s, uint s_Length, DECODE_TOLERANCE Tolerance)
{
	// Żadne dziwne znaki nie są tolerowane - po prostu zlicz znaki
	if (Tolerance == DECODE_TOLERANCE_NONE)
	{
		if ((s_Length & 0x07) != 0) // (s.length % 8) != 0
			return false;
		*OutLength = s_Length >> 3;
		return true;
	}
	// Pewne dodatkowe znaki są tolerowane - trzeba wszystkie przejrzeć i zliczyć same cyfry
	else
	{
		// s będzie przesuwany. s_Length będzie zmniejszany.

		uint DigitCount = 0;
		while (s_Length > 0)
		{
			if (*s == '1' || *s == '0')
				DigitCount++;
			s++;
			s_Length--;
		}

		if ((DigitCount & 0x07) != 0) // (DigitCount % 8) != 0
			return false;
		*OutLength = DigitCount >> 3;
		return true;
	}
}

size_t BinDecoder::Decode(void *OutData, const string &s, DECODE_TOLERANCE Tolerance)
{
	uint8 *OutBytes = (uint8*)OutData;
	uint8 Byte;
	uint s_i = 0, Sum = 0;
	char Ch;

	if (Tolerance == DECODE_TOLERANCE_NONE)
	{
		if ((s.length() & 0x03) != 0) return MAXUINT32;

		while (s_i < s.length())
		{
			if (s[s_i] == '1') Byte = 0x80; else if (s[s_i] == '0') Byte = 0x00; else return MAXUINT32; s_i++;

			if (s[s_i] == '1') Byte |= 0x40; else if (s[s_i] != '0') return MAXUINT32; s_i++;
			if (s[s_i] == '1') Byte |= 0x20; else if (s[s_i] != '0') return MAXUINT32; s_i++;
			if (s[s_i] == '1') Byte |= 0x10; else if (s[s_i] != '0') return MAXUINT32; s_i++;
			if (s[s_i] == '1') Byte |= 0x08; else if (s[s_i] != '0') return MAXUINT32; s_i++;
			if (s[s_i] == '1') Byte |= 0x04; else if (s[s_i] != '0') return MAXUINT32; s_i++;
			if (s[s_i] == '1') Byte |= 0x02; else if (s[s_i] != '0') return MAXUINT32; s_i++;
			if (s[s_i] == '1') Byte |= 0x01; else if (s[s_i] != '0') return MAXUINT32; s_i++;

			*OutBytes = Byte;
			OutBytes++;
			Sum++;
		}
	}
	else if (Tolerance == DECODE_TOLERANCE_WHITESPACE)
	{
		for (;;)
		{
			// Pierwszy bit - może go nie być jeśli to koniec strumienia
			for (;;)
			{
				if (s_i == s.length())
					goto break_2_WHITESPACE;
				Ch = s[s_i++];
				if (Ch == '1')
				{
					Byte = 0x80;
					break;
				}
				else if (Ch == '0')
				{
					Byte = 0x00;
					break;
				}
				else if (CharIsWhitespace(Ch))
				{
					// Nic - nowy obieg tej pętli wewnętrznej
				}
				else
					return MAXUINT32;
			}

			// Następne 7 bitów
			for (;;) {
				if (s_i == s.length()) return MAXUINT32;
				Ch = s[s_i++];
				if (Ch == '1') { Byte |= 0x40; break; } else if (Ch == '0') break; else if (!CharIsWhitespace(Ch)) return MAXUINT32;
			}
			for (;;) {
				if (s_i == s.length()) return MAXUINT32;
				Ch = s[s_i++];
				if (Ch == '1') { Byte |= 0x20; break; } else if (Ch == '0') break; else if (!CharIsWhitespace(Ch)) return MAXUINT32;
			}
			for (;;) {
				if (s_i == s.length()) return MAXUINT32;
				Ch = s[s_i++];
				if (Ch == '1') { Byte |= 0x10; break; } else if (Ch == '0') break; else if (!CharIsWhitespace(Ch)) return MAXUINT32;
			}
			for (;;) {
				if (s_i == s.length()) return MAXUINT32;
				Ch = s[s_i++];
				if (Ch == '1') { Byte |= 0x08; break; } else if (Ch == '0') break; else if (!CharIsWhitespace(Ch)) return MAXUINT32;
			}
			for (;;) {
				if (s_i == s.length()) return MAXUINT32;
				Ch = s[s_i++];
				if (Ch == '1') { Byte |= 0x04; break; } else if (Ch == '0') break; else if (!CharIsWhitespace(Ch)) return MAXUINT32;
			}
			for (;;) {
				if (s_i == s.length()) return MAXUINT32;
				Ch = s[s_i++];
				if (Ch == '1') { Byte |= 0x02; break; } else if (Ch == '0') break; else if (!CharIsWhitespace(Ch)) return MAXUINT32;
			}
			for (;;) {
				if (s_i == s.length()) return MAXUINT32;
				Ch = s[s_i++];
				if (Ch == '1') { Byte |= 0x01; break; } else if (Ch == '0') break; else if (!CharIsWhitespace(Ch)) return MAXUINT32;
			}

			*OutBytes = Byte;
			OutBytes++;
			Sum++;
		}
break_2_WHITESPACE:
		;
	}
	else // (m_Tolerance == DECODE_TOLERANCE_ALL)
	{
		for (;;)
		{
			// Pierwszy bit - może go nie być jeśli to koniec strumienia
			for (;;)
			{
				if (s_i == s.length())
					goto break_2_ALL;
				Ch = s[s_i++];
				if (Ch == '1')
				{
					Byte = 0x80;
					break;
				}
				else if (Ch == '0')
				{
					Byte = 0x00;
					break;
				}
			}

			// Następne 7 bitów
			for (;;) {
				if (s_i == s.length()) return MAXUINT32;
				Ch = s[s_i++];
				if (Ch == '1') { Byte |= 0x40; break; } else if (Ch == '0') break;
			}
			for (;;) {
				if (s_i == s.length()) return MAXUINT32;
				Ch = s[s_i++];
				if (Ch == '1') { Byte |= 0x20; break; } else if (Ch == '0') break;
			}
			for (;;) {
				if (s_i == s.length()) return MAXUINT32;
				Ch = s[s_i++];
				if (Ch == '1') { Byte |= 0x10; break; } else if (Ch == '0') break;
			}
			for (;;) {
				if (s_i == s.length()) return MAXUINT32;
				Ch = s[s_i++];
				if (Ch == '1') { Byte |= 0x08; break; } else if (Ch == '0') break;
			}
			for (;;) {
				if (s_i == s.length()) return MAXUINT32;
				Ch = s[s_i++];
				if (Ch == '1') { Byte |= 0x04; break; } else if (Ch == '0') break;
			}
			for (;;) {
				if (s_i == s.length()) return MAXUINT32;
				Ch = s[s_i++];
				if (Ch == '1') { Byte |= 0x02; break; } else if (Ch == '0') break;
			}
			for (;;) {
				if (s_i == s.length()) return MAXUINT32;
				Ch = s[s_i++];
				if (Ch == '1') { Byte |= 0x01; break; } else if (Ch == '0') break;
			}

			*OutBytes = Byte;
			OutBytes++;
			Sum++;
		}
break_2_ALL:
		;
	}

	return Sum;
}

size_t BinDecoder::Decode(void *OutData, const char *s, uint s_Length, DECODE_TOLERANCE Tolerance)
{
	uint8 *OutBytes = (uint8*)OutData;
	uint8 Byte;
	uint s_i = 0, Sum = 0;
	char Ch;

	if (Tolerance == DECODE_TOLERANCE_NONE)
	{
		if ((s_Length & 0x03) != 0) return MAXUINT32;

		while (s_i < s_Length)
		{
			if (s[s_i] == '1') Byte = 0x80; else if (s[s_i] == '0') Byte = 0x00; else return MAXUINT32; s_i++;

			if (s[s_i] == '1') Byte |= 0x40; else if (s[s_i] != '0') return MAXUINT32; s_i++;
			if (s[s_i] == '1') Byte |= 0x20; else if (s[s_i] != '0') return MAXUINT32; s_i++;
			if (s[s_i] == '1') Byte |= 0x10; else if (s[s_i] != '0') return MAXUINT32; s_i++;
			if (s[s_i] == '1') Byte |= 0x08; else if (s[s_i] != '0') return MAXUINT32; s_i++;
			if (s[s_i] == '1') Byte |= 0x04; else if (s[s_i] != '0') return MAXUINT32; s_i++;
			if (s[s_i] == '1') Byte |= 0x02; else if (s[s_i] != '0') return MAXUINT32; s_i++;
			if (s[s_i] == '1') Byte |= 0x01; else if (s[s_i] != '0') return MAXUINT32; s_i++;

			*OutBytes = Byte;
			OutBytes++;
			Sum++;
		}
	}
	else if (Tolerance == DECODE_TOLERANCE_WHITESPACE)
	{
		for (;;)
		{
			// Pierwszy bit - może go nie być jeśli to koniec strumienia
			for (;;)
			{
				if (s_i == s_Length)
					goto break_2_WHITESPACE;
				Ch = s[s_i++];
				if (Ch == '1')
				{
					Byte = 0x80;
					break;
				}
				else if (Ch == '0')
				{
					Byte = 0x00;
					break;
				}
				else if (CharIsWhitespace(Ch))
				{
					// Nic - nowy obieg tej pętli wewnętrznej
				}
				else
					return MAXUINT32;
			}

			// Następne 7 bitów
			for (;;) {
				if (s_i == s_Length) return MAXUINT32;
				Ch = s[s_i++];
				if (Ch == '1') { Byte |= 0x40; break; } else if (Ch == '0') break; else if (!CharIsWhitespace(Ch)) return MAXUINT32;
			}
			for (;;) {
				if (s_i == s_Length) return MAXUINT32;
				Ch = s[s_i++];
				if (Ch == '1') { Byte |= 0x20; break; } else if (Ch == '0') break; else if (!CharIsWhitespace(Ch)) return MAXUINT32;
			}
			for (;;) {
				if (s_i == s_Length) return MAXUINT32;
				Ch = s[s_i++];
				if (Ch == '1') { Byte |= 0x10; break; } else if (Ch == '0') break; else if (!CharIsWhitespace(Ch)) return MAXUINT32;
			}
			for (;;) {
				if (s_i == s_Length) return MAXUINT32;
				Ch = s[s_i++];
				if (Ch == '1') { Byte |= 0x08; break; } else if (Ch == '0') break; else if (!CharIsWhitespace(Ch)) return MAXUINT32;
			}
			for (;;) {
				if (s_i == s_Length) return MAXUINT32;
				Ch = s[s_i++];
				if (Ch == '1') { Byte |= 0x04; break; } else if (Ch == '0') break; else if (!CharIsWhitespace(Ch)) return MAXUINT32;
			}
			for (;;) {
				if (s_i == s_Length) return MAXUINT32;
				Ch = s[s_i++];
				if (Ch == '1') { Byte |= 0x02; break; } else if (Ch == '0') break; else if (!CharIsWhitespace(Ch)) return MAXUINT32;
			}
			for (;;) {
				if (s_i == s_Length) return MAXUINT32;
				Ch = s[s_i++];
				if (Ch == '1') { Byte |= 0x01; break; } else if (Ch == '0') break; else if (!CharIsWhitespace(Ch)) return MAXUINT32;
			}

			*OutBytes = Byte;
			OutBytes++;
			Sum++;
		}
break_2_WHITESPACE:
		;
	}
	else // (m_Tolerance == DECODE_TOLERANCE_ALL)
	{
		for (;;)
		{
			// Pierwszy bit - może go nie być jeśli to koniec strumienia
			for (;;)
			{
				if (s_i == s_Length)
					goto break_2_ALL;
				Ch = s[s_i++];
				if (Ch == '1')
				{
					Byte = 0x80;
					break;
				}
				else if (Ch == '0')
				{
					Byte = 0x00;
					break;
				}
			}

			// Następne 7 bitów
			for (;;) {
				if (s_i == s_Length) return MAXUINT32;
				Ch = s[s_i++];
				if (Ch == '1') { Byte |= 0x40; break; } else if (Ch == '0') break;
			}
			for (;;) {
				if (s_i == s_Length) return MAXUINT32;
				Ch = s[s_i++];
				if (Ch == '1') { Byte |= 0x20; break; } else if (Ch == '0') break;
			}
			for (;;) {
				if (s_i == s_Length) return MAXUINT32;
				Ch = s[s_i++];
				if (Ch == '1') { Byte |= 0x10; break; } else if (Ch == '0') break;
			}
			for (;;) {
				if (s_i == s_Length) return MAXUINT32;
				Ch = s[s_i++];
				if (Ch == '1') { Byte |= 0x08; break; } else if (Ch == '0') break;
			}
			for (;;) {
				if (s_i == s_Length) return MAXUINT32;
				Ch = s[s_i++];
				if (Ch == '1') { Byte |= 0x04; break; } else if (Ch == '0') break;
			}
			for (;;) {
				if (s_i == s_Length) return MAXUINT32;
				Ch = s[s_i++];
				if (Ch == '1') { Byte |= 0x02; break; } else if (Ch == '0') break;
			}
			for (;;) {
				if (s_i == s_Length) return MAXUINT32;
				Ch = s[s_i++];
				if (Ch == '1') { Byte |= 0x01; break; } else if (Ch == '0') break;
			}

			*OutBytes = Byte;
			OutBytes++;
			Sum++;
		}
break_2_ALL:
		;
	}

	return Sum;
}


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Klasa HexEncoder

const char * const HEX_DIGITS_U = "0123456789ABCDEF";
const char * const HEX_DIGITS_L = "0123456789abcdef";
#ifdef WIN32
const wchar_t * const WHEX_DIGITS_U = L"0123456789ABCDEF";
const wchar_t * const WHEX_DIGITS_L = L"0123456789abcdef";
#endif

void HexEncoder::Write(const void *Data, size_t Size)
{
	ERR_TRY;

	const uint8 *Bytes = (const uint8*)Data;
	uint8 Byte;

	if (m_UpperCase)
	{
		for (uint i = 0; i < Size; i++)
		{
			Byte = *Bytes;
			m_CharWriter.WriteChar(HEX_DIGITS_U[(Byte & 0xF0) >> 4]);
			m_CharWriter.WriteChar(HEX_DIGITS_U[Byte & 0x0F]);
			Bytes++;
		}
	}
	else
	{
		for (uint i = 0; i < Size; i++)
		{
			Byte = *Bytes;
			m_CharWriter.WriteChar(HEX_DIGITS_L[(Byte & 0xF0) >> 4]);
			m_CharWriter.WriteChar(HEX_DIGITS_L[Byte & 0x0F]);
			Bytes++;
		}
	}

	ERR_CATCH_FUNC;
}

void HexEncoder::Encode(char *Out, const void *Data, size_t DataLength, bool UpperCase)
{
	// DataLength będzie zmniejszane.
	// Out i InBytes błąd przesuwane.

	const uint8 *InBytes = (const uint8*)Data;
	uint8 Byte;

	if (UpperCase)
	{
		while (DataLength > 0)
		{
			Byte = *InBytes;
			*Out = HEX_DIGITS_U[(Byte & 0xF0) >> 4]; Out++;
			*Out = HEX_DIGITS_U[ Byte & 0x0F      ]; Out++;
			InBytes++;
			DataLength--;
		}
	}
	else
	{
		while (DataLength > 0)
		{
			Byte = *InBytes;
			*Out = HEX_DIGITS_L[(Byte & 0xF0) >> 4]; Out++;
			*Out = HEX_DIGITS_L[ Byte & 0x0F      ]; Out++;
			InBytes++;
			DataLength--;
		}
	}
}

void HexEncoder::Encode(string *Out, const void *Data, size_t DataLength, bool UpperCase)
{
	// DataLength będzie zmniejszane.
	// InBytes będzie przesuwane.
	Out->resize(DataLength * 2);

	const uint8 *InBytes = (const uint8*)Data;
	uint8 Byte;
	uint Out_i = 0;

	if (UpperCase)
	{
		while (DataLength > 0)
		{
			Byte = *InBytes;
			(*Out)[Out_i] = HEX_DIGITS_U[(Byte & 0xF0) >> 4]; Out_i++;
			(*Out)[Out_i] = HEX_DIGITS_U[ Byte & 0x0F      ]; Out_i++;
			InBytes++;
			DataLength--;
		}
	}
	else
	{
		while (DataLength > 0)
		{
			Byte = *InBytes;
			(*Out)[Out_i] = HEX_DIGITS_L[(Byte & 0xF0) >> 4]; Out_i++;
			(*Out)[Out_i] = HEX_DIGITS_L[ Byte & 0x0F      ]; Out_i++;
			InBytes++;
			DataLength--;
		}
	}
}

#ifdef WIN32
void HexEncoder::Encode(wchar_t *Out, const void *Data, size_t DataLength, bool UpperCase)
{
	// DataLength będzie zmniejszane.
	// Out i InBytes błąd przesuwane.

	const uint8 *InBytes = (const uint8*)Data;
	uint8 Byte;

	if (UpperCase)
	{
		while (DataLength > 0)
		{
			Byte = *InBytes;
			*Out = WHEX_DIGITS_U[(Byte & 0xF0) >> 4]; Out++;
			*Out = WHEX_DIGITS_U[ Byte & 0x0F      ]; Out++;
			InBytes++;
			DataLength--;
		}
	}
	else
	{
		while (DataLength > 0)
		{
			Byte = *InBytes;
			*Out = WHEX_DIGITS_L[(Byte & 0xF0) >> 4]; Out++;
			*Out = WHEX_DIGITS_L[ Byte & 0x0F      ]; Out++;
			InBytes++;
			DataLength--;
		}
	}
}

void HexEncoder::Encode(wstring *Out, const void *Data, size_t DataLength, bool UpperCase)
{
	// DataLength będzie zmniejszane.
	// InBytes będzie przesuwane.
	Out->resize(DataLength * 2);

	const uint8 *InBytes = (const uint8*)Data;
	uint8 Byte;
	uint Out_i = 0;

	if (UpperCase)
	{
		while (DataLength > 0)
		{
			Byte = *InBytes;
			(*Out)[Out_i] = WHEX_DIGITS_U[(Byte & 0xF0) >> 4]; Out_i++;
			(*Out)[Out_i] = WHEX_DIGITS_U[ Byte & 0x0F      ]; Out_i++;
			InBytes++;
			DataLength--;
		}
	}
	else
	{
		while (DataLength > 0)
		{
			Byte = *InBytes;
			(*Out)[Out_i] = WHEX_DIGITS_L[(Byte & 0xF0) >> 4]; Out_i++;
			(*Out)[Out_i] = WHEX_DIGITS_L[ Byte & 0x0F      ]; Out_i++;
			InBytes++;
			DataLength--;
		}
	}
}
#endif


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Klasa HexDecoder

HexDecoder::HexDecoder(Stream *a_Stream, DECODE_TOLERANCE Tolerance) :
	OverlayStream(a_Stream),
	m_CharReader(a_Stream),
	m_Tolerance(Tolerance)
{
}

size_t HexDecoder::Read(void *Out, size_t Size)
{
	// Size będzie zmniejszane.

	ERR_TRY;

	uint8 *OutBytes = (uint8*)Out;
	uint8 Byte, HexNumber;
	uint Sum = 0;
	char Ch;

	if (m_Tolerance == DECODE_TOLERANCE_NONE)
	{
		while (Size > 0)
		{
			// Pierwsza cyfra - może jej nie być jeśli to koniec strumienia
			if (!m_CharReader.ReadChar(&Ch))
				break;
			HexNumber = HexDigitToNumber(Ch);
			if (HexNumber == 0xFF)
				throw Error(ERRMSG_DECODE_INVALID_CHAR, __TFILE__, __LINE__);
			Byte = HexNumber << 4;

			// Druga cyfra
			HexNumber = HexDigitToNumber(m_CharReader.MustReadChar());
			if (HexNumber == 0xFF)
				throw Error(ERRMSG_DECODE_INVALID_CHAR, __TFILE__, __LINE__);
			Byte |= HexNumber;

			*OutBytes = Byte;
			OutBytes++;
			Sum++;
			Size--;
		}
	}
	else if (m_Tolerance == DECODE_TOLERANCE_WHITESPACE)
	{
		while (Size > 0)
		{
			// Pierwsza cyfra - może jej nie być jeśli to koniec strumienia
			for (;;)
			{
				if (!m_CharReader.ReadChar(&Ch))
					goto break_2_WHITESPACE;
				HexNumber = HexDigitToNumber(Ch);
				if (HexNumber == 0xFF)
				{
					if (!CharIsWhitespace(Ch))
						throw Error(ERRMSG_DECODE_INVALID_CHAR, __TFILE__, __LINE__);
				}
				else
				{
					Byte = HexNumber << 4;
					break;
				}
			}

			// Druga cyfra
			for (;;)
			{
				Ch = m_CharReader.MustReadChar();
				HexNumber = HexDigitToNumber(Ch);
				if (HexNumber == 0xFF)
				{
					if (!CharIsWhitespace(Ch))
						throw Error(ERRMSG_DECODE_INVALID_CHAR, __TFILE__, __LINE__);
				}
				else
				{
					Byte |= HexNumber;
					break;
				}
			}

			*OutBytes = Byte;
			OutBytes++;
			Sum++;
			Size--;
		}
break_2_WHITESPACE:
		;
	}
	else // (m_Tolerance == DECODE_TOLERANCE_ALL)
	{
		while (Size > 0)
		{
			// Pierwsza cyfra - może jej nie być jeśli to koniec strumienia
			for (;;)
			{
				if (!m_CharReader.ReadChar(&Ch))
					goto break_2_ALL;
				HexNumber = HexDigitToNumber(Ch);
				if (HexNumber != 0xFF)
				{
					Byte = HexNumber << 4;
					break;
				}
			}

			// Druga cyfra
			for (;;)
			{
				Ch = m_CharReader.MustReadChar();
				HexNumber = HexDigitToNumber(Ch);
				if (HexNumber != 0xFF)
				{
					Byte |= HexNumber;
					break;
				}
			}

			*OutBytes = Byte;
			OutBytes++;
			Sum++;
			Size--;
		}
break_2_ALL:
		;
	}

	return Sum;

	ERR_CATCH_FUNC;
}

bool HexDecoder::DecodeLength(uint *OutLength, const string &s, DECODE_TOLERANCE Tolerance)
{
	// Żadne dziwne znaki nie są tolerowane - po prostu zlicz znaki
	if (Tolerance == DECODE_TOLERANCE_NONE)
	{
		if ((s.length() & 0x07) != 0) // (s.length % 8) != 0
			return false;
		*OutLength = s.length() >> 3;
		return true;
	}
	// Pewne dodatkowe znaki są tolerowane - trzeba wszystkie przejrzeć i zliczyć same cyfry
	else
	{
		uint DigitCount = 0;
		for (uint i = 0; i < s.length(); i++)
		{
			if (s[i] == '1' || s[i] == '0')
				DigitCount++;
		}

		if ((DigitCount & 0x07) != 0) // (DigitCount % 8) != 0
			return false;
		*OutLength = DigitCount >> 3;
		return true;
	}
}

bool HexDecoder::DecodeLength(uint *OutLength, const char *s, uint s_Length, DECODE_TOLERANCE Tolerance)
{
	// Żadne dziwne znaki nie są tolerowane - po prostu zlicz znaki
	if (Tolerance == DECODE_TOLERANCE_NONE)
	{
		if ((s_Length & 0x01) != 0) // (s.length % 2) != 0
			return false;
		*OutLength = s_Length >> 1;
		return true;
	}
	// Pewne dodatkowe znaki są tolerowane - trzeba wszystkie przejrzeć i zliczyć same cyfry
	else
	{
		// s będzie przesuwany. s_Length będzie zmniejszany.

		uint DigitCount = 0;
		while (s_Length > 0)
		{
			if (CharIsHexDigit(*s))
				DigitCount++;
			s++;
			s_Length--;
		}

		if ((DigitCount & 0x01) != 0) // (DigitCount % 2) != 0
			return false;
		*OutLength = DigitCount >> 1;
		return true;
	}
}

size_t HexDecoder::Decode(void *OutData, const string &s, DECODE_TOLERANCE Tolerance)
{
	uint8 *OutBytes = (uint8*)OutData;
	uint8 Byte, HexNumber;
	uint s_i = 0, Sum = 0;
	char Ch;

	if (Tolerance == DECODE_TOLERANCE_NONE)
	{
		if ((s.length() & 0x01) != 0) return MAXUINT32;

		while (s_i < s.length())
		{
			HexNumber = HexDigitToNumber(s[s_i++]);
			if (HexNumber == 0xFF) return MAXUINT32;
			Byte = HexNumber << 4;

			HexNumber = HexDigitToNumber(s[s_i++]);
			if (HexNumber == 0xFF) return MAXUINT32;
			Byte |= HexNumber;

			*OutBytes = Byte;
			OutBytes++;
			Sum++;
		}
	}
	else if (Tolerance == DECODE_TOLERANCE_WHITESPACE)
	{
		for (;;)
		{
			// Pierwsza cyfra - może jej nie być jeśli to koniec strumienia
			for (;;)
			{
				if (s_i == s.length())
					goto break_2_WHITESPACE;
				Ch = s[s_i++];
				HexNumber = HexDigitToNumber(Ch);
				if (HexNumber == 0xFF)
				{
					if (!CharIsWhitespace(Ch))
						return MAXUINT32;
				}
				else
				{
					Byte = HexNumber << 4;
					break;
				}
			}

			// Druga cyfra
			for (;;)
			{
				if (s_i == s.length())
					return MAXUINT32;
				Ch = s[s_i++];
				HexNumber = HexDigitToNumber(Ch);
				if (HexNumber == 0xFF)
				{
					if (!CharIsWhitespace(Ch))
						return MAXUINT32;
				}
				else
				{
					Byte |= HexNumber;
					break;
				}
			}

			*OutBytes = Byte;
			OutBytes++;
			Sum++;
		}
break_2_WHITESPACE:
		;
	}
	else // (m_Tolerance == DECODE_TOLERANCE_ALL)
	{
		for (;;)
		{
			// Pierwsza cyfra - może jej nie być jeśli to koniec strumienia
			for (;;)
			{
				if (s_i == s.length())
					goto break_2_ALL;
				Ch = s[s_i++];
				HexNumber = HexDigitToNumber(Ch);
				if (HexNumber != 0xFF)
				{
					Byte = HexNumber << 4;
					break;
				}
			}

			// Druga cyfra
			for (;;)
			{
				if (s_i == s.length())
					return MAXUINT32;
				Ch = s[s_i++];
				HexNumber = HexDigitToNumber(Ch);
				if (HexNumber != 0xFF)
				{
					Byte |= HexNumber;
					break;
				}
			}

			*OutBytes = Byte;
			OutBytes++;
			Sum++;
		}
break_2_ALL:
		;
	}

	return Sum;
}

size_t HexDecoder::Decode(void *OutData, const char *s, uint s_Length, DECODE_TOLERANCE Tolerance)
{
	uint8 *OutBytes = (uint8*)OutData;
	uint8 Byte, HexNumber;
	uint s_i = 0, Sum = 0;
	char Ch;

	if (Tolerance == DECODE_TOLERANCE_NONE)
	{
		if ((s_Length & 0x01) != 0) return MAXUINT32;

		while (s_i < s_Length)
		{
			HexNumber = HexDigitToNumber(s[s_i++]);
			if (HexNumber == 0xFF) return MAXUINT32;
			Byte = HexNumber << 4;

			HexNumber = HexDigitToNumber(s[s_i++]);
			if (HexNumber == 0xFF) return MAXUINT32;
			Byte |= HexNumber;

			*OutBytes = Byte;
			OutBytes++;
			Sum++;
		}
	}
	else if (Tolerance == DECODE_TOLERANCE_WHITESPACE)
	{
		for (;;)
		{
			// Pierwsza cyfra - może jej nie być jeśli to koniec strumienia
			for (;;)
			{
				if (s_i == s_Length)
					goto break_2_WHITESPACE;
				Ch = s[s_i++];
				HexNumber = HexDigitToNumber(Ch);
				if (HexNumber == 0xFF)
				{
					if (!CharIsWhitespace(Ch))
						return MAXUINT32;
				}
				else
				{
					Byte = HexNumber << 4;
					break;
				}
			}

			// Druga cyfra
			for (;;)
			{
				if (s_i == s_Length)
					return MAXUINT32;
				Ch = s[s_i++];
				HexNumber = HexDigitToNumber(Ch);
				if (HexNumber == 0xFF)
				{
					if (!CharIsWhitespace(Ch))
						return MAXUINT32;
				}
				else
				{
					Byte |= HexNumber;
					break;
				}
			}

			*OutBytes = Byte;
			OutBytes++;
			Sum++;
		}
break_2_WHITESPACE:
		;
	}
	else // (m_Tolerance == DECODE_TOLERANCE_ALL)
	{
		for (;;)
		{
			// Pierwsza cyfra - może jej nie być jeśli to koniec strumienia
			for (;;)
			{
				if (s_i == s_Length)
					goto break_2_ALL;
				Ch = s[s_i++];
				HexNumber = HexDigitToNumber(Ch);
				if (HexNumber != 0xFF)
				{
					Byte = HexNumber << 4;
					break;
				}
			}

			// Druga cyfra
			for (;;)
			{
				if (s_i == s_Length)
					return MAXUINT32;
				Ch = s[s_i++];
				HexNumber = HexDigitToNumber(Ch);
				if (HexNumber != 0xFF)
				{
					Byte |= HexNumber;
					break;
				}
			}

			*OutBytes = Byte;
			OutBytes++;
			Sum++;
		}
break_2_ALL:
		;
	}

	return Sum;
}

#ifdef WIN32
size_t HexDecoder::Decode(void *OutData, const wstring &s, DECODE_TOLERANCE Tolerance)
{
	uint8 *OutBytes = (uint8*)OutData;
	uint8 Byte, HexNumber;
	uint s_i = 0, Sum = 0;
	wchar_t Ch;

	if (Tolerance == DECODE_TOLERANCE_NONE)
	{
		if ((s.length() & 0x01) != 0) return MAXUINT32;

		while (s_i < s.length())
		{
			HexNumber = HexDigitToNumber(s[s_i++]);
			if (HexNumber == 0xFF) return MAXUINT32;
			Byte = HexNumber << 4;

			HexNumber = HexDigitToNumber(s[s_i++]);
			if (HexNumber == 0xFF) return MAXUINT32;
			Byte |= HexNumber;

			*OutBytes = Byte;
			OutBytes++;
			Sum++;
		}
	}
	else if (Tolerance == DECODE_TOLERANCE_WHITESPACE)
	{
		for (;;)
		{
			// Pierwsza cyfra - może jej nie być jeśli to koniec strumienia
			for (;;)
			{
				if (s_i == s.length())
					goto break_2_WHITESPACE;
				Ch = s[s_i++];
				HexNumber = HexDigitToNumber(Ch);
				if (HexNumber == 0xFF)
				{
					if (!CharIsWhitespace(Ch))
						return MAXUINT32;
				}
				else
				{
					Byte = HexNumber << 4;
					break;
				}
			}

			// Druga cyfra
			for (;;)
			{
				if (s_i == s.length())
					return MAXUINT32;
				Ch = s[s_i++];
				HexNumber = HexDigitToNumber(Ch);
				if (HexNumber == 0xFF)
				{
					if (!CharIsWhitespace(Ch))
						return MAXUINT32;
				}
				else
				{
					Byte |= HexNumber;
					break;
				}
			}

			*OutBytes = Byte;
			OutBytes++;
			Sum++;
		}
break_2_WHITESPACE:
		;
	}
	else // (m_Tolerance == DECODE_TOLERANCE_ALL)
	{
		for (;;)
		{
			// Pierwsza cyfra - może jej nie być jeśli to koniec strumienia
			for (;;)
			{
				if (s_i == s.length())
					goto break_2_ALL;
				Ch = s[s_i++];
				HexNumber = HexDigitToNumber(Ch);
				if (HexNumber != 0xFF)
				{
					Byte = HexNumber << 4;
					break;
				}
			}

			// Druga cyfra
			for (;;)
			{
				if (s_i == s.length())
					return MAXUINT32;
				Ch = s[s_i++];
				HexNumber = HexDigitToNumber(Ch);
				if (HexNumber != 0xFF)
				{
					Byte |= HexNumber;
					break;
				}
			}

			*OutBytes = Byte;
			OutBytes++;
			Sum++;
		}
break_2_ALL:
		;
	}

	return Sum;
}

size_t HexDecoder::Decode(void *OutData, const wchar_t *s, uint s_Length, DECODE_TOLERANCE Tolerance)
{
	uint8 *OutBytes = (uint8*)OutData;
	uint8 Byte, HexNumber;
	uint s_i = 0, Sum = 0;
	wchar_t Ch;

	if (Tolerance == DECODE_TOLERANCE_NONE)
	{
		if ((s_Length & 0x01) != 0) return MAXUINT32;

		while (s_i < s_Length)
		{
			HexNumber = HexDigitToNumber(s[s_i++]);
			if (HexNumber == 0xFF) return MAXUINT32;
			Byte = HexNumber << 4;

			HexNumber = HexDigitToNumber(s[s_i++]);
			if (HexNumber == 0xFF) return MAXUINT32;
			Byte |= HexNumber;

			*OutBytes = Byte;
			OutBytes++;
			Sum++;
		}
	}
	else if (Tolerance == DECODE_TOLERANCE_WHITESPACE)
	{
		for (;;)
		{
			// Pierwsza cyfra - może jej nie być jeśli to koniec strumienia
			for (;;)
			{
				if (s_i == s_Length)
					goto break_2_WHITESPACE;
				Ch = s[s_i++];
				HexNumber = HexDigitToNumber(Ch);
				if (HexNumber == 0xFF)
				{
					if (!CharIsWhitespace(Ch))
						return MAXUINT32;
				}
				else
				{
					Byte = HexNumber << 4;
					break;
				}
			}

			// Druga cyfra
			for (;;)
			{
				if (s_i == s_Length)
					return MAXUINT32;
				Ch = s[s_i++];
				HexNumber = HexDigitToNumber(Ch);
				if (HexNumber == 0xFF)
				{
					if (!CharIsWhitespace(Ch))
						return MAXUINT32;
				}
				else
				{
					Byte |= HexNumber;
					break;
				}
			}

			*OutBytes = Byte;
			OutBytes++;
			Sum++;
		}
break_2_WHITESPACE:
		;
	}
	else // (m_Tolerance == DECODE_TOLERANCE_ALL)
	{
		for (;;)
		{
			// Pierwsza cyfra - może jej nie być jeśli to koniec strumienia
			for (;;)
			{
				if (s_i == s_Length)
					goto break_2_ALL;
				Ch = s[s_i++];
				HexNumber = HexDigitToNumber(Ch);
				if (HexNumber != 0xFF)
				{
					Byte = HexNumber << 4;
					break;
				}
			}

			// Druga cyfra
			for (;;)
			{
				if (s_i == s_Length)
					return MAXUINT32;
				Ch = s[s_i++];
				HexNumber = HexDigitToNumber(Ch);
				if (HexNumber != 0xFF)
				{
					Byte |= HexNumber;
					break;
				}
			}

			*OutBytes = Byte;
			OutBytes++;
			Sum++;
		}
break_2_ALL:
		;
	}

	return Sum;
}
#endif


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Klasa Base64Encoder

void Base64Encoder::DoFinish()
{
	// Ile pozostałych bajtów zalega jeszcze w buforze
	switch (m_BufIndex)
	{
	case 1:
		m_CharWriter.WriteChar( BASE64_CHARS[ m_Buf[0] >> 2 ] );
		m_CharWriter.WriteChar( BASE64_CHARS[ (m_Buf[0] & 0x3) << 4 ] );
		m_CharWriter.WriteChar( '=' );
		m_CharWriter.WriteChar( '=' );
		break;
	case 2:
		m_CharWriter.WriteChar( BASE64_CHARS[ m_Buf[0] >> 2 ] );
		m_CharWriter.WriteChar( BASE64_CHARS[ ((m_Buf[0] & 0x3) << 4) | (m_Buf[1] >> 4) ] );
		m_CharWriter.WriteChar( BASE64_CHARS[ (m_Buf[1] & 0xF) << 2 ] );
		m_CharWriter.WriteChar( '=' );
		break;
	}

	m_CharWriter.Flush();
}

Base64Encoder::~Base64Encoder()
{
	try
	{
		if (!m_Finished) DoFinish();
	}
	catch (...)
	{
		assert(0 && "Exception caught in Base64Encoder::~Base64Encoder while calling DoFinish.");
	}
}

void Base64Encoder::Write(const void *Data, size_t Size)
{
	// Będą zmniejszać Size i przesuwać ByteData

	assert(!m_Finished && "Base64Encoder::Write: Write after Finish.");

	ERR_TRY;

	uint8 *ByteData = (uint8*)Data;

	// Pętla przetwarza kolejne bajty
	while (Size > 0)
	{
		if (m_BufIndex == 2)
		{
			// Bufor pełny - mamy trzy bajty - dwa w m_Buf, trzeci w *ByteData. Czas się ich pozbyć.
			m_CharWriter.WriteChar( BASE64_CHARS[ m_Buf[0] >> 2 ] );
			m_CharWriter.WriteChar( BASE64_CHARS[ ((m_Buf[0] & 0x3) << 4) | (m_Buf[1] >> 4) ] );
			m_CharWriter.WriteChar( BASE64_CHARS[ ((m_Buf[1] & 0xF) << 2) | (*ByteData >> 6) ] );
			m_CharWriter.WriteChar( BASE64_CHARS[ (*ByteData & 0x3F) ] );
			m_BufIndex = 0;
		}
		else
			m_Buf[m_BufIndex++] = *ByteData;

		Size--;
		ByteData++;
	}

	ERR_CATCH_FUNC;
}

size_t Base64Encoder::EncodeLength(size_t DataLength)
{
	return ceil_div<size_t>(DataLength, 3) * 4;
}

size_t Base64Encoder::Encode(char *Out, const void *Data, size_t DataLength)
{
	uint8 *ByteData = (uint8*)Data;
	size_t BlockCount = DataLength / 3;
	size_t RemainingBytes = DataLength % 3;
	size_t OutLength = ceil_div<size_t>(DataLength, 3) * 4;

	size_t OutIndex = 0;

	while (BlockCount > 0)
	{
		Out[OutIndex++] = BASE64_CHARS[ ByteData[0] >> 2 ];
		Out[OutIndex++] = BASE64_CHARS[ ((ByteData[0] & 0x3) << 4) | (ByteData[1] >> 4) ];
		Out[OutIndex++] = BASE64_CHARS[ ((ByteData[1] & 0xF) << 2) | (ByteData[2] >> 6) ];
		Out[OutIndex++] = BASE64_CHARS[ (ByteData[2] & 0x3F) ];

		ByteData += 3;
		BlockCount--;
	}

	switch (RemainingBytes)
	{
	case 1:
		Out[OutIndex++] = BASE64_CHARS[ ByteData[0] >> 2 ];
		Out[OutIndex++] = BASE64_CHARS[ (ByteData[0] & 0x3) << 4 ];
		Out[OutIndex++] = '=';
		Out[OutIndex++] = '=';
		break;
	case 2:
		Out[OutIndex++] = BASE64_CHARS[ ByteData[0] >> 2 ];
		Out[OutIndex++] = BASE64_CHARS[ ((ByteData[0] & 0x3) << 4) | (ByteData[1] >> 4) ];
		Out[OutIndex++] = BASE64_CHARS[ (ByteData[1] & 0xF) << 2 ];
		Out[OutIndex++] = '=';
		break;
	}

	return OutLength;
}

size_t Base64Encoder::Encode(string *Out, const void *Data, size_t DataLength)
{
	uint8 *ByteData = (uint8*)Data;
	size_t BlockCount = DataLength / 3;
	size_t RemainingBytes = DataLength % 3;
	size_t OutLength = ceil_div<size_t>(DataLength, 3) * 4;

	Out->clear();
	Out->resize(OutLength);
	size_t OutIndex = 0;

	while (BlockCount > 0)
	{
		(*Out)[OutIndex++] = BASE64_CHARS[ ByteData[0] >> 2 ];
		(*Out)[OutIndex++] = BASE64_CHARS[ ((ByteData[0] & 0x3) << 4) | (ByteData[1] >> 4) ];
		(*Out)[OutIndex++] = BASE64_CHARS[ ((ByteData[1] & 0xF) << 2) | (ByteData[2] >> 6) ];
		(*Out)[OutIndex++] = BASE64_CHARS[ (ByteData[2] & 0x3F) ];

		ByteData += 3;
		BlockCount--;
	}

	switch (RemainingBytes)
	{
	case 1:
		(*Out)[OutIndex++] = BASE64_CHARS[ ByteData[0] >> 2 ];
		(*Out)[OutIndex++] = BASE64_CHARS[ (ByteData[0] & 0x3) << 4 ];
		(*Out)[OutIndex++] = '=';
		(*Out)[OutIndex++] = '=';
		break;
	case 2:
		(*Out)[OutIndex++] = BASE64_CHARS[ ByteData[0] >> 2 ];
		(*Out)[OutIndex++] = BASE64_CHARS[ ((ByteData[0] & 0x3) << 4) | (ByteData[1] >> 4) ];
		(*Out)[OutIndex++] = BASE64_CHARS[ (ByteData[1] & 0xF) << 2 ];
		(*Out)[OutIndex++] = '=';
		break;
	}

	return OutLength;
}


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Klasa Base64Decoder

bool Base64Decoder::ReadNextBuf()
{
	uint8 Numbers[4];

	if (m_Tolerance == DECODE_TOLERANCE_NONE)
	{
		char Chs[4];
		if (!m_CharReader.ReadChar(&Chs[0]))
			return false;
		Chs[1] = m_CharReader.MustReadChar();
		Chs[2] = m_CharReader.MustReadChar();
		Chs[3] = m_CharReader.MustReadChar();

		Numbers[0] = Base64CharToNumber(Chs[0]);
		Numbers[1] = Base64CharToNumber(Chs[1]);
		Numbers[2] = Base64CharToNumber(Chs[2]);
		Numbers[3] = Base64CharToNumber(Chs[3]);

		// Kończy się na "=" lub "=="
		if (Numbers[3] == 0xFE)
		{
			// Kończy się na "=="
			if (Numbers[2] == 0xFE)
			{
				if (Numbers[0] >= 0xFE) throw Error(ERRMSG_DECODE_INVALID_CHAR);
				if (Numbers[1] >= 0xFE) throw Error(ERRMSG_DECODE_INVALID_CHAR);

				m_Buf[0] = (Numbers[0] << 2) | (Numbers[1] >> 4);
				m_BufLength = 1;
				m_Finished = true;
				return true;
			}
			// Kończy się na "="
			else
			{
				if (Numbers[0] >= 0xFE) throw Error(ERRMSG_DECODE_INVALID_CHAR);
				if (Numbers[1] >= 0xFE) throw Error(ERRMSG_DECODE_INVALID_CHAR);
				if (Numbers[2] >= 0xFE) throw Error(ERRMSG_DECODE_INVALID_CHAR);

				m_Buf[1] = (Numbers[0] << 2) | (Numbers[1] >> 4);
				m_Buf[0] = (Numbers[1] << 4) | (Numbers[2] >> 2);
				m_BufLength = 2;
				m_Finished = true;
				return true;
			}
		}

		// Błędne znaki lub '=' tam gdzie nie trzeba.
		if (Numbers[0] >= 0xFE) throw Error(ERRMSG_DECODE_INVALID_CHAR);
		if (Numbers[1] >= 0xFE) throw Error(ERRMSG_DECODE_INVALID_CHAR);
		if (Numbers[2] >= 0xFE) throw Error(ERRMSG_DECODE_INVALID_CHAR);
		if (Numbers[3] >= 0xFE) throw Error(ERRMSG_DECODE_INVALID_CHAR);

		m_Buf[2] = (Numbers[0] << 2) | (Numbers[1] >> 4);
		m_Buf[1] = (Numbers[1] << 4) | (Numbers[2] >> 2);
		m_Buf[0] = (Numbers[2] << 6) | Numbers[3];
		m_BufLength = 3;
		return true;
	}
	else
	{
		// Wczytaj 4 znaki
		uint NumberIndex = 0; // do indeksowania Numbers
		char Ch;

		for (;;)
		{
			// Wczytaj znak
			if (!m_CharReader.ReadChar(&Ch))
			{
				// koniec danych wejściowych

				// Jesteśmy na granicy czwórki znaków - OK, koniec
				if (NumberIndex == 0)
					return false;
				// Nie jesteśmy na granicy czwórki znaków - błąd, niedokończone dane
				else
					throw Error(ERRMSG_UNEXPECTED_END);
			}

			// Nieznany znak
			if (Base64CharType(Ch) == 2)
			{
				// Opcjonalny błąd
				if (m_Tolerance == DECODE_TOLERANCE_WHITESPACE && !CharIsWhitespace(Ch))
					throw Error(ERRMSG_DECODE_INVALID_CHAR);
			}
			// Znak znany - cyfra base64 lub '='
			else
			{
				Numbers[NumberIndex++] = Base64CharToNumber(Ch);

				// To czwarty z czwórki znaków
				if (NumberIndex == 4)
				{
					// Przetwórz tą czwórkę

					// Kończy się na "=" lub "=="
					if (Numbers[3] == 0xFE)
					{
						// Kończy się na "=="
						if (Numbers[2] == 0xFE)
						{
							if (Numbers[0] >= 0xFE) throw Error(ERRMSG_DECODE_INVALID_CHAR);
							if (Numbers[1] >= 0xFE) throw Error(ERRMSG_DECODE_INVALID_CHAR);

							m_Buf[0] = (Numbers[0] << 2) | (Numbers[1] >> 4); //OutBytes++;
							m_BufLength = 1;
							m_Finished = true;
							return true;
						}
						// Kończy się na "="
						else
						{
							if (Numbers[0] >= 0xFE) throw Error(ERRMSG_DECODE_INVALID_CHAR);
							if (Numbers[1] >= 0xFE) throw Error(ERRMSG_DECODE_INVALID_CHAR);
							if (Numbers[2] >= 0xFE) throw Error(ERRMSG_DECODE_INVALID_CHAR);

							m_Buf[1] = (Numbers[0] << 2) | (Numbers[1] >> 4);
							m_Buf[0] = (Numbers[1] << 4) | (Numbers[2] >> 2);
							m_BufLength = 2;
							m_Finished = true;
							return true;
						}
					}

					// Nie kończy się - normalne znaki

					// Błędne znaki lub '=' tam gdzie nie trzeba.
					if (Numbers[0] >= 0xFE) throw Error(ERRMSG_DECODE_INVALID_CHAR);
					if (Numbers[1] >= 0xFE) throw Error(ERRMSG_DECODE_INVALID_CHAR);
					if (Numbers[2] >= 0xFE) throw Error(ERRMSG_DECODE_INVALID_CHAR);
					if (Numbers[3] >= 0xFE) throw Error(ERRMSG_DECODE_INVALID_CHAR);

					m_Buf[2] = (Numbers[0] << 2) | (Numbers[1] >> 4);
					m_Buf[1] = (Numbers[1] << 4) | (Numbers[2] >> 2);
					m_Buf[0] = (Numbers[2] << 6) | Numbers[3];

					m_BufLength = 3;
					return true;
				}
			}
		}
	}
}

Base64Decoder::Base64Decoder(Stream *a_Stream, DECODE_TOLERANCE Tolerance) :
	OverlayStream(a_Stream),
	m_CharReader(a_Stream),
	m_Tolerance(Tolerance),
	m_BufLength(0),
	m_Finished(false)
{
}

size_t Base64Decoder::Read(void *Out, size_t Size)
{
	ERR_TRY;

	uint8 *OutBytes = (uint8*)Out;
	// Size będzie zmniejszany. OutBytes będzie przesuwany.

	size_t Sum = 0;
	while (Size > 0)
	{
		if (!GetNextByte(OutBytes))
			break;
		OutBytes++;
		Size--;
		Sum++;
	}

	return Sum;

	ERR_CATCH_FUNC;
}

bool Base64Decoder::End()
{
	return (m_BufLength == 0) && (m_Finished || m_CharReader.End());
}

bool Base64Decoder::DecodeLength(uint *OutLength, const string &s, DECODE_TOLERANCE Tolerance)
{
	// są tylko dokładnie te znaki co trzeba
	if (Tolerance == DECODE_TOLERANCE_NONE)
	{
		if (s.empty()) { *OutLength = 0; return true; }
		if ((s.length() & 3) != 0) return false; // length % 4 != 0
		*OutLength = s.length() * 3 / 4;
		if (s[s.length()-2] == '=')
			(*OutLength) -= 2;
		else if (s[s.length()-1] == '=')
			(*OutLength) -= 1;
		return true;
	}
	// Mogą być też inne znaki - trzeba wszystkie przejrzeć i policzyć
	else
	{
		uint EqualCount = 0;
		uint DigitCount = 0;
		*OutLength = 0;
		for (uint i = 0; i < s.length(); i++)
		{
			switch (Base64CharType(s[i]))
			{
			case 0:
				DigitCount++;
				break;
			case 1:
				EqualCount++;
				break;
			}
		}

		if (((DigitCount+EqualCount) & 3) != 0) return false; // (DigitCount+EqualCount) % 4 != 0
		*OutLength = (DigitCount+EqualCount) * 3 / 4;
		switch (EqualCount)
		{
		case 0:
			break;
		case 2:
			(*OutLength) -= 2;
			break;
		case 1:
			(*OutLength) -= 1;
			break;
		default:
			return false;
		}
		return true;
	}
}

bool Base64Decoder::DecodeLength(uint *OutLength, const char *s, uint s_Length, DECODE_TOLERANCE Tolerance)
{
	// są tylko dokładnie te znaki co trzeba
	if (Tolerance == DECODE_TOLERANCE_NONE)
	{
		if (s_Length == 0) { *OutLength = 0; return true; }
		if ((s_Length & 3) != 0) return false; // length % 4 != 0
		*OutLength = s_Length * 3 / 4;
		if (s[s_Length-2] == '=')
			(*OutLength) -= 2;
		else if (s[s_Length-1] == '=')
			(*OutLength) -= 1;
		return true;
	}
	// Mogą być też inne znaki - trzeba wszystkie przejrzeć i policzyć
	else
	{
		uint EqualCount = 0;
		uint DigitCount = 0;
		*OutLength = 0;
		for (uint i = 0; i < s_Length; i++)
		{
			switch (Base64CharType(s[i]))
			{
			case 0:
				DigitCount++;
				break;
			case 1:
				EqualCount++;
				break;
			}
		}

		if (((DigitCount+EqualCount) & 3) != 0) return false; // (DigitCount+EqualCount) % 4 != 0
		*OutLength = (DigitCount+EqualCount) * 3 / 4;
		switch (EqualCount)
		{
		case 0:
			break;
		case 2:
			(*OutLength) -= 2;
			break;
		case 1:
			(*OutLength) -= 1;
			break;
		default:
			return false;
		}
		return true;
	}
}

size_t Base64Decoder::Decode(void *OutData, const string &s, DECODE_TOLERANCE Tolerance)
{
	uint8 *OutBytes = (uint8*)OutData;
	uint s_i = 0, Sum = 0;
	uint8 Numbers[4];

	if (Tolerance == DECODE_TOLERANCE_NONE)
	{
		if ((s.length() & 3) != 0) return MAXUINT32;

		while (s_i < s.length())
		{
			Numbers[0] = Base64CharToNumber(s[s_i++]);
			Numbers[1] = Base64CharToNumber(s[s_i++]);
			Numbers[2] = Base64CharToNumber(s[s_i++]);
			Numbers[3] = Base64CharToNumber(s[s_i++]);

			if (s_i == s.length())
			{
				// Kołczy się na "=" lub "=="
				if (Numbers[3] == 0xFE)
				{
					// Kołczy się na "=="
					if (Numbers[2] == 0xFE)
					{
						if (Numbers[0] >= 0xFE) return MAXUINT32;
						if (Numbers[1] >= 0xFE) return MAXUINT32;

						*OutBytes = (Numbers[0] << 2) | (Numbers[1] >> 4); //OutBytes++;
						Sum += 1;
						break;
					}
					// Kołczy się na "="
					else
					{
						if (Numbers[0] >= 0xFE) return MAXUINT32;
						if (Numbers[1] >= 0xFE) return MAXUINT32;
						if (Numbers[2] >= 0xFE) return MAXUINT32;

						*OutBytes = (Numbers[0] << 2) | (Numbers[1] >> 4); OutBytes++;
						*OutBytes = (Numbers[1] << 4) | (Numbers[2] >> 2); //OutBytes++;
						Sum += 2;
						break;
					}
				}
			}

			// Błędne znaki lub '=' tam gdzie nie trzeba.
			if (Numbers[0] >= 0xFE) return MAXUINT32;
			if (Numbers[1] >= 0xFE) return MAXUINT32;
			if (Numbers[2] >= 0xFE) return MAXUINT32;
			if (Numbers[3] >= 0xFE) return MAXUINT32;

			*OutBytes = (Numbers[0] << 2) | (Numbers[1] >> 4); OutBytes++;
			*OutBytes = (Numbers[1] << 4) | (Numbers[2] >> 2); OutBytes++;
			*OutBytes = (Numbers[2] << 6) | Numbers[3]; OutBytes++;

			Sum += 3;
		}
	}
	else
	{
		// Wczytuj po 4 znaki
		uint NumberIndex = 0; // do indeksowania Numbers
		char Ch;

		for (;;)
		{
			// koniec danych wejściowych
			if (s_i == s.length())
			{
				// Jesteśmy na granicy czwórki znaków - OK, koniec
				if (NumberIndex == 0)
					break;
				// Nie jesteśmy na granicy czwórki znaków - błąd, niedokończone dane
				else
					return MAXUINT32;
			}

			// Wczytaj znak
			Ch = s[s_i++];
			// Nieznany znak
			if (Base64CharType(Ch) == 2)
			{
				// Opcjonalny błąd
				if (Tolerance == DECODE_TOLERANCE_WHITESPACE && !CharIsWhitespace(Ch))
					return MAXUINT32;
			}
			// Znak znany - cyfra base64 lub '='
			else
			{
				Numbers[NumberIndex++] = Base64CharToNumber(Ch);

				// To czwarty z czwórki znaków
				if (NumberIndex == 4)
				{
					// Przetwórz tą czwórkę

					// Kończy się na "=" lub "=="
					if (Numbers[3] == 0xFE)
					{
						// Kończy się na "=="
						if (Numbers[2] == 0xFE)
						{
							if (Numbers[0] >= 0xFE) return MAXUINT32;
							if (Numbers[1] >= 0xFE) return MAXUINT32;

							*OutBytes = (Numbers[0] << 2) | (Numbers[1] >> 4); //OutBytes++;
							Sum += 1;
							break;
						}
						// Kończy się na "="
						else
						{
							if (Numbers[0] >= 0xFE) return MAXUINT32;
							if (Numbers[1] >= 0xFE) return MAXUINT32;
							if (Numbers[2] >= 0xFE) return MAXUINT32;

							*OutBytes = (Numbers[0] << 2) | (Numbers[1] >> 4); OutBytes++;
							*OutBytes = (Numbers[1] << 4) | (Numbers[2] >> 2); //OutBytes++;
							Sum += 2;
							break;
						}
					}

					// Nie kończy się - normalne znaki

					// Błędne znaki lub '=' tam gdzie nie trzeba.
					if (Numbers[0] >= 0xFE) return MAXUINT32;
					if (Numbers[1] >= 0xFE) return MAXUINT32;
					if (Numbers[2] >= 0xFE) return MAXUINT32;
					if (Numbers[3] >= 0xFE) return MAXUINT32;

					*OutBytes = (Numbers[0] << 2) | (Numbers[1] >> 4); OutBytes++;
					*OutBytes = (Numbers[1] << 4) | (Numbers[2] >> 2); OutBytes++;
					*OutBytes = (Numbers[2] << 6) | Numbers[3]; OutBytes++;

					Sum += 3;

					NumberIndex = 0;
				}
			}
		}
	}

	return Sum;
}

size_t Base64Decoder::Decode(void *OutData, const char *s, uint s_Length, DECODE_TOLERANCE Tolerance)
{
	uint8 *OutBytes = (uint8*)OutData;
	uint s_i = 0, Sum = 0;
	uint8 Numbers[4];

	if (Tolerance == DECODE_TOLERANCE_NONE)
	{
		if ((s_Length & 3) != 0) return MAXUINT32;

		while (s_i < s_Length)
		{
			Numbers[0] = Base64CharToNumber(s[s_i++]);
			Numbers[1] = Base64CharToNumber(s[s_i++]);
			Numbers[2] = Base64CharToNumber(s[s_i++]);
			Numbers[3] = Base64CharToNumber(s[s_i++]);

			if (s_i == s_Length)
			{
				// Kończy się na "=" lub "=="
				if (Numbers[3] == 0xFE)
				{
					// Kończy się na "=="
					if (Numbers[2] == 0xFE)
					{
						if (Numbers[0] >= 0xFE) return MAXUINT32;
						if (Numbers[1] >= 0xFE) return MAXUINT32;

						*OutBytes = (Numbers[0] << 2) | (Numbers[1] >> 4); //OutBytes++;
						Sum += 1;
						break;
					}
					// Kończy się na "="
					else
					{
						if (Numbers[0] >= 0xFE) return MAXUINT32;
						if (Numbers[1] >= 0xFE) return MAXUINT32;
						if (Numbers[2] >= 0xFE) return MAXUINT32;

						*OutBytes = (Numbers[0] << 2) | (Numbers[1] >> 4); OutBytes++;
						*OutBytes = (Numbers[1] << 4) | (Numbers[2] >> 2); //OutBytes++;
						Sum += 2;
						break;
					}
				}
			}

			// Błędne znaki lub '=' tam gdzie nie trzeba.
			if (Numbers[0] >= 0xFE) return MAXUINT32;
			if (Numbers[1] >= 0xFE) return MAXUINT32;
			if (Numbers[2] >= 0xFE) return MAXUINT32;
			if (Numbers[3] >= 0xFE) return MAXUINT32;

			*OutBytes = (Numbers[0] << 2) | (Numbers[1] >> 4); OutBytes++;
			*OutBytes = (Numbers[1] << 4) | (Numbers[2] >> 2); OutBytes++;
			*OutBytes = (Numbers[2] << 6) | Numbers[3]; OutBytes++;

			Sum += 3;
		}
	}
	else
	{
		// Wczytuj po 4 znaki
		uint NumberIndex = 0; // do indeksowania Numbers
		char Ch;

		for (;;)
		{
			// koniec danych wejściowych
			if (s_i == s_Length)
			{
				// Jesteśmy na granicy czwórki znaków - OK, koniec
				if (NumberIndex == 0)
					break;
				// Nie jesteśmy na granicy czwórki znaków - błąd, niedokończone dane
				else
					return MAXUINT32;
			}

			// Wczytaj znak
			Ch = s[s_i++];
			// Nieznany znak
			if (Base64CharType(Ch) == 2)
			{
				// Opcjonalny błąd
				if (Tolerance == DECODE_TOLERANCE_WHITESPACE && !CharIsWhitespace(Ch))
					return MAXUINT32;
			}
			// Znak znany - cyfra base64 lub '='
			else
			{
				Numbers[NumberIndex++] = Base64CharToNumber(Ch);

				// To czwarty z czwórki znaków
				if (NumberIndex == 4)
				{
					// Przetwórz tą czwórkę

					// Kończy się na "=" lub "=="
					if (Numbers[3] == 0xFE)
					{
						// Kończy się na "=="
						if (Numbers[2] == 0xFE)
						{
							if (Numbers[0] >= 0xFE) return MAXUINT32;
							if (Numbers[1] >= 0xFE) return MAXUINT32;

							*OutBytes = (Numbers[0] << 2) | (Numbers[1] >> 4); //OutBytes++;
							Sum += 1;
							break;
						}
						// Kończy się na "="
						else
						{
							if (Numbers[0] >= 0xFE) return MAXUINT32;
							if (Numbers[1] >= 0xFE) return MAXUINT32;
							if (Numbers[2] >= 0xFE) return MAXUINT32;

							*OutBytes = (Numbers[0] << 2) | (Numbers[1] >> 4); OutBytes++;
							*OutBytes = (Numbers[1] << 4) | (Numbers[2] >> 2); //OutBytes++;
							Sum += 2;
							break;
						}
					}

					// Nie kończy się - normalne znaki

					// Błędne znaki lub '=' tam gdzie nie trzeba.
					if (Numbers[0] >= 0xFE) return MAXUINT32;
					if (Numbers[1] >= 0xFE) return MAXUINT32;
					if (Numbers[2] >= 0xFE) return MAXUINT32;
					if (Numbers[3] >= 0xFE) return MAXUINT32;

					*OutBytes = (Numbers[0] << 2) | (Numbers[1] >> 4); OutBytes++;
					*OutBytes = (Numbers[1] << 4) | (Numbers[2] >> 2); OutBytes++;
					*OutBytes = (Numbers[2] << 6) | Numbers[3]; OutBytes++;

					Sum += 3;

					NumberIndex = 0;
				}
			}
		}
	}

	return Sum;
}


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Klasa RingBuffer

RingBuffer::RingBuffer(uint Capacity) :
	m_Capacity(Capacity),
	m_Size(0),
	m_BegIndex(0),
	m_EndIndex(0)
{
	m_Buf.resize(m_Capacity);
}

RingBuffer::~RingBuffer()
{
}

void RingBuffer::Write(const void *Data, size_t Size)
{
	// Nie zmieści się w buforze
	if (m_Size + Size > m_Capacity)
		throw Error(Format(_T("RingBuffer write error: Cannot write # bytes - capacity exceeded.")) % Size, __TFILE__, __LINE__);

	// Zmieści się całej na końcu
	if (Size <= m_Capacity - m_EndIndex)
	{
		common_memcpy(&m_Buf[m_EndIndex], Data, Size);
		m_EndIndex += Size;
	}
	// Nie zmieści się na końcu
	else {
		uint PartSize = m_Capacity - m_EndIndex;

		// Przepisz na koniec ile się zmieści
		if (PartSize)
			common_memcpy(&m_Buf[m_EndIndex], Data, PartSize);
		// Przepisz resztą na początek
		common_memcpy(&m_Buf[0], ((const char*)Data + PartSize), Size - PartSize);

		m_EndIndex = Size - PartSize;
	}
	m_Size += Size;
}

size_t RingBuffer::Read(void *Out, size_t MaxLength)
{
	if (MaxLength == 0) return 0;

	// Ustal ile bajtąw odczytać
	uint Length = std::min(MaxLength, m_Size);
	// Wszystko będzie w jednym kawałku
	if (m_Capacity - m_BegIndex >= Length)
	{
		common_memcpy(Out, &m_Buf[m_BegIndex], Length);
		m_BegIndex += Length;
	}
	// będzie w dwóch kawałkach
	else
	{
		uint PartSize = m_Capacity - m_BegIndex;

		// Kawałek z końca
		if (PartSize)
			common_memcpy(Out, &m_Buf[m_BegIndex], PartSize);
		// Kawałek z początku
		common_memcpy(((char*)Out + PartSize), &m_Buf[0], Length - PartSize);

		m_BegIndex = Length - PartSize;
	}
	m_Size -= Length;
	return Length;
}

void RingBuffer::MustRead(void *Out, size_t Length)
{
	if (Length == 0) return;

	if (m_Size < Length)
		throw Error(Format(_T("Cannot read # bytes from RingBuffer - not enought bytes to read, there is only # bytes.")) % Length % m_Size, __TFILE__, __LINE__);

	// Wszystko będzie w jednym kawałku
	if (m_Capacity - m_BegIndex >= Length)
	{
		common_memcpy(Out, &m_Buf[m_BegIndex], Length);
		m_BegIndex += Length;
	}
	// będzie w dwóch kawałkach
	else
	{
		uint PartSize = m_Capacity - m_BegIndex;

		// Kawałek z końca
		common_memcpy(Out, &m_Buf[m_BegIndex], PartSize);
		// Kawałek z początku
		common_memcpy(((char*)Out + PartSize), &m_Buf[0], Length - PartSize);

		m_BegIndex = Length - PartSize;
	}
	m_Size -= Length;
}

bool RingBuffer::End()
{
	return IsEmpty();
}

size_t RingBuffer::Skip(size_t MaxLength)
{
	if (MaxLength == 0) return 0;

	// Ustal ile bajtąw pominąć
	uint Length = std::min(MaxLength, m_Size);
	// Wszystko jest w jednym kawałku
	if (m_Capacity - m_BegIndex >= Length)
		m_BegIndex += Length;
	// Jest w dwóch kawałkach
	else
	{
		uint PartSize = m_Capacity - m_BegIndex;
		m_BegIndex = Length - PartSize;
	}
	m_Size -= Length;
	return Length;
}


} // namespace common
