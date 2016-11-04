/** \file
\brief Auxiliary module for zlib library
\author Adam Sawicki - sawickiap@poczta.onet.pl - http://asawicki.info/ \n

Part of CommonLib library. \n
Encoding UTF-8, end of line CR+LF \n
License: GNU LGPL. \n
Documentation: \ref Module_ZlibUtils\n
Module components: \ref code_zlibutils
*/
#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif
#ifndef COMMON_ZLIB_UTILS_H_
#define COMMON_ZLIB_UTILS_H_

#include "Stream.hpp"
#include "Error.hpp"

namespace common
{

/** \addtogroup code_zlibutils ZlibUtils Module
Dokumentacja: \ref Module_ZlibUtils \n
Nag³ówek: ZlibUtils.hpp */
//@{

/// Klasa wyj¹tku dla b³êdu zg³oszonego przez bibliotekê zlib
class ZlibError : public Error
{
public:
	ZlibError(int Code, const tstring &Msg = _T(""), const tstring &File = _T(""), int Line = 0);
};

/** \name Poziomy kompresji Zlib. */
//@{
/// Brak kompresji (co nie znaczy ¿e dane zostan¹ dos³ownie przepisane)
const int ZLIB_STORE_LEVEL   = 0;
/// Najs³absza kompresja, ale najszybsza
const int ZLIB_FASTEST_LEVEL = 1;
/// Najlepsza jakoœæ kompresji
const int ZLIB_BEST_LEVEL    = 9;
/// Domyœlny poziom kompresji
extern const int ZLIB_DEFAULT_LEVEL;
//@}

/// \internal
class ZlibCompressionStream_pimpl;
/// \internal
class ZlibDecompressionStream_pimpl;

/// Strumieñ do zapisu, który kompresuje dane w formacie zlib.
class ZlibCompressionStream : public OverlayStream
{
private:
	scoped_ptr<ZlibCompressionStream_pimpl> pimpl;

public:
	ZlibCompressionStream(Stream *a_Stream, int Level = ZLIB_DEFAULT_LEVEL, int WindowBits = 15);
	virtual ~ZlibCompressionStream();

	// ======== Implementacja Stream ========
	
	virtual void Write(const void *Data, size_t Size);
	/** Mo¿e spowodowaæ, ¿e skompresowane dane bêd¹ inaczej wyg³ada³y i obni¿yæ jakoœæ kompresji. */
	virtual void Flush();
	
	/// Oblicza maksymalny rozmiar bufora na skompresowane dane
	static size_t CompressLength(size_t DataLength);
	/// Po prostu kompresuje podane dane
	/**
	\return Zwraca d³ugoœæ skompresowanych danych.
	\param[out] OutData Musi byæ buforem o d³ugoœci conajmniej takiej, jak obliczona przez CompressLength.
	\param BufLength D³ugoœæ zaalokowanej pamiêci na OutData, podawana i sprawdzana na wszelki wypadek.
	*/
	static size_t Compress(void *OutData, size_t BufLength, const void *Data, size_t DataLength, int Level = ZLIB_DEFAULT_LEVEL);
};

/// Strumieñ do zapisu, który kompresuje w formacie gzip do³¹czaj¹c nag³ówek gzip z podanymi parametrami.
/** Wszystko jak w klasie ZlibCompressionStream. */
class GzipCompressionStream : public OverlayStream
{
private:
	scoped_ptr<ZlibCompressionStream_pimpl> pimpl;

public:
	/**
	\param FileName Jeœli ma nie byæ, mo¿na podaæ NULL.
	\param Comment Jeœli ma nie byæ, mo¿na podaæ NULL.
	*/
	GzipCompressionStream(Stream *a_Stream, const string *FileName, const string *Comment, int Level = ZLIB_DEFAULT_LEVEL, int WindowBits = 15);
	virtual ~GzipCompressionStream();

	// ======== Implementacja Stream ========
	
	virtual void Write(const void *Data, size_t Size);
	virtual void Flush();
};

/// Strumieñ do odczytu dekompresuj¹cy dane w formacie Zlib.
class ZlibDecompressionStream : public OverlayStream
{
private:
	scoped_ptr<ZlibDecompressionStream_pimpl> pimpl;

public:
	ZlibDecompressionStream(Stream *a_Stream, int WindowBits = 15);
	virtual ~ZlibDecompressionStream();

	// ======== Implementacja Stream ========
	
	virtual size_t Read(void *Out, size_t MaxLength);
	virtual bool End();

	/// Po prostu dekompresuje podane dane
	/**
	Rozmiar danych po dekompresji musi byæ gdzieœ zapamiêtany i odpowiednio du¿y bufor musi byæ ju¿ zaalkowany.
	\return Zwraca d³ugoœæ rozkompresowanych danych.
	\param BufLength D³ugoœæ zaalokowanej pamiêci na OutData, podawana i sprawdzana na wszelki wypadek.
	*/
	static size_t Decompress(void *OutData, size_t BufLength, const void *Data, size_t DataLength);
};

/// Strumieñ do odczytu, który dekompresuje dane w formacie gzip odczytuj¹c te¿ nag³ówek.
/** Wszystko podobnie jak w klasie ZlibCompressionStream. */
class GzipDecompressionStream : public OverlayStream
{
private:
	scoped_ptr<ZlibDecompressionStream_pimpl> pimpl;

public:
	GzipDecompressionStream(Stream *a_Stream, int WindowBits = 15);
	virtual ~GzipDecompressionStream();

	// ======== Implementacja Stream ========
	
	virtual size_t Read(void *Out, size_t MaxLength);
	virtual bool End();

	/** \name Pobieranie informacji z nag³ówka.
	Jeœli plik posiada nag³ówek, ten nag³ówek przechowuje dan¹ informacjê i
	zosta³ ju¿ w toku odczytywania odczytany, zwraca true i zwraca przez parametr
	z t¹ informacjê. */
	//@{
	bool GetHeaderFileName(string *OutFileName);
	bool GetHeaderComment(string *OutComment);
	//@}
};

/// Tryb otwarcia pliku GZip
enum GZIP_FILE_MODE
{
	GZFM_WRITE, ///< Do zapisu
	GZFM_READ,  ///< Do odczytu
};

/// \internal
class GzipFileStream_pimpl;

/// Obs³uga pliku w formacie gzip (zalecane rozszerzenie ".gz")
class GzipFileStream : public Stream
{
private:
	scoped_ptr<GzipFileStream_pimpl> pimpl;

public:
	/** Level ma znaczenie tylko przy zapisie. */
	GzipFileStream(const string &FileName, GZIP_FILE_MODE Mode, int Level = ZLIB_DEFAULT_LEVEL);
	virtual ~GzipFileStream();

	// ======== Implementacja Stream ========
	
	virtual void Write(const void *Data, size_t Size);
	virtual void Flush();
	
	virtual size_t Read(void *Out, size_t MaxLength);
	virtual bool End();
};

//@}
// code_zlibutils

} // namespace common

#endif
