/** \file
\brief Files and file system handling
\author Adam Sawicki - sawickiap@poczta.onet.pl - http://asawicki.info/ \n

Part of CommonLib library. \n
Encoding UTF-8, end of line CR+LF \n
License: GNU LGPL. \n
Documentation: \ref Module_Files \n
Module components: \ref code_files
*/
#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif
#ifndef COMMON_FILES_H_
#define COMMON_FILES_H_

#include "Stream.hpp"


namespace common
{

struct DATETIME;

/** \addtogroup code_files Files Module
Dokumentacja: \ref Module_Files \n
Nag³ówek: Files.hpp */
//@{

/// Rodzaj elementu systemu plików
enum FILE_ITEM_TYPE
{
	IT_NONE, ///< B³¹d, brak, koniec itp.
	IT_DIR,  ///< Katalog
	IT_FILE, ///< Plik
};

/// Tryb otwarcia pliku
enum FILE_MODE
{
	/** zapis: tak, odczyt: nie \n
	pozycja: 0 \n
	nie istnieje: tworzy, istnieje: opró¿nia \n
	SetPos: dzia³a */
	FM_WRITE,
	/** zapis: tak, odczyt: tak \n
	pozycja: 0 \n
	nie istnieje: tworzy, istnieje: opró¿nia \n
	SetPos: dzia³a */
	FM_WRITE_PLUS,
	/** zapis: nie, odczyt: tak \n
	pozycja: 0 \n
	nie istnieje: b³¹d, istnieje: otwiera \n
	SetPos: dzia³a */
	FM_READ,
	/** zapis: tak, odczyt: tak \n
	pozycja: 0 \n
	nie istnieje: b³¹d, istnieje: otwiera \n
	SetPos: dzia³a */
	FM_READ_PLUS,
	/** zapis: tak, odczyt: nie \n
	pozycja: koniec \n
	nie istnieje: tworzy, istnieje: otwiera \n
	SetPos: w Linuksie nie dzia³a! */
	FM_APPEND,
	/** zapis: tak, odczyt: tak \n
	pozycja: koniec \n
	nie istnieje: tworzy, istnieje: otwiera \n
	SetPos: w Linuksie nie dzia³a! */
	FM_APPEND_PLUS,
};

/// \internal
class File_pimpl;

/// Strumieñ plikowy
class FileStream : public SeekableStream
{
private:
	scoped_ptr<File_pimpl> pimpl;

public:
	FileStream(const tstring &FileName, FILE_MODE FileMode, bool Lock = true);
	virtual ~FileStream();

	virtual void Write(const void *Data, size_t Size);
	virtual size_t Read(void *Data, size_t Size);
	virtual void Flush();
	virtual uint64 GetSize();
	virtual int64 GetPos();
	virtual void SetPos(int64 pos);
	virtual void SetPosFromCurrent(int64 pos);
	virtual void SetPosFromEnd(int64 pos);
	virtual void SetSize(uint64 Size);
	virtual void Truncate();
	virtual bool End();

#ifdef _WIN32
	HANDLE GetNativeHandle();
#endif
};

/// \internal
class DirLister_pimpl;

/// Klasa do listowania zawartoœci katalogu.
/** W przypadku b³êdów rzuca wyj¹tki.
Nie listuje <tt>"."</tt> ani <tt>".."</tt>. */
class DirLister
{
private:
	scoped_ptr<DirLister_pimpl> pimpl;

public:
	DirLister(const tstring &Dir);
	~DirLister();

	/// Pobiera pierwszy/nastêpny element (plik lub katalog) i zwraca go przez parametry.
	/** Jeœli nie uda³o siê, bo ju¿ koniec, zawraca false.
	\param[out] OutName to sama nazwa, nie pe³na œcie¿ka.
	\param[out] OutType to DIR lub FILE. */
	bool ReadNext(tstring *OutName, FILE_ITEM_TYPE *OutType);
};


/// Zapisuje podany ³añcuch jako treœæ pliku
void SaveStringToFile(const tstring &FileName, const string &Data);
/// Zapisuje podany ³añcuch jako treœæ pliku
void SaveDataToFile(const tstring &FileName, const void *Data, size_t NumBytes);
/// Wczytuje ca³¹ zawartoœæ pliku do ³añcucha
void LoadStringFromFile(const tstring &FileName, string *Data);
/// Wersje Unicode
#ifdef _WIN32
void SaveStringToFile(const tstring &FileName, const wstring &Data);
void LoadStringFromFile(const tstring &FileName, wstring *Data);
#endif

/// Zwraca wybrane informacje na temat pliku/katalogu
/** Jako parametry wyjœciowe mo¿na podawaæ NULL.
Jeœli to katalog, zwrócony rozmiar jest niezdefiniowany. */
bool GetFileItemInfo(const tstring &Path, FILE_ITEM_TYPE *OutType, uint64 *OutSize, DATETIME *OutModificationTime, DATETIME *OutCreationTime = NULL, DATETIME *OutAccessTime = NULL);
void MustGetFileItemInfo(const tstring &Path, FILE_ITEM_TYPE *OutType, uint64 *OutSize, DATETIME *OutModificationTime, DATETIME *OutCreationTime = NULL, DATETIME *OutAccessTime = NULL);
/// Zwraca typ - czy to jest plik, katalog czy b³¹d/nie-istnieje.
/** Jeœli b³¹d lub nie istnieje, zwraca common::IT_NONE. */
FILE_ITEM_TYPE GetFileItemType(const tstring &Path);

/// Ustawia czas dostêpu i modyfikacji podanego pliku na bie¿¹cy
bool UpdateFileTimeToNow(const tstring &FileName);
void MustUpdateFileTimeToNow(const tstring &FileName);
/// Ustawia czas dostêpu i modyfikacji podanego pliku na podane
bool UpdateFileTime(const tstring &FileName, const DATETIME &ModificationTime, const DATETIME &AccessTime);
void MustUpdateFileTime(const tstring &FileName, const DATETIME &ModificationTime, const DATETIME &AccessTime);

/// Tworzy katalog
bool CreateDirectory(const tstring &Path);
void MustCreateDirectory(const tstring &Path);
/// Usuwa katalog
/** Katalog musi byæ pusty. */
bool DeleteDirectory(const tstring &Path);
void MustDeleteDirectory(const tstring &Path);
/// Tworzy ca³¹ œcie¿kê katalogów a¿ po podanego, np. <tt>"C:\A\B\C"</tt> nawet jeœli nie istnia³ nawet <tt>"C:\A"</tt>
bool CreateDirectoryChain(const tstring &Path);
void MustCreateDirectoryChain(const tstring &Path);

/// Usuwa plik
bool DeleteFile(const tstring &FileName);
void MustDeleteFile(const tstring &FileName);
/// Przenosi lub zmienia nazwê pliku lub katalogu
/** Katalogi mo¿na przenosiæ tylko w ramach jednej partycji. Pliki niekoniecznie. */
bool MoveItem(const tstring &OldPath, const tstring &NewPath);
void MustMoveItem(const tstring &OldPath, const tstring &NewPath);

// Pliki tekstowe z u¿yciem Unicode
#ifdef _WIN32

/// Kodowanie pliku tekstowego
enum FILE_ENCODING
{
	/** \name M³odsze 16 bitów - kodowanie */
	//@{
	FILE_ENCODING_UTF16_LE = 1, ///< Zwyk³y UTF-16 stosowany w Windows (domyœlnie dodaje BOM)
	FILE_ENCODING_UTF8     = 2, ///< UTF-8 (domyœlnie nie dodaje BOM)
	FILE_ENCODING_ANSI     = 3, ///< Natywne kodowanie systemu
	//@}

	/** \name Starsze 16 bitów - flagi dodatkowe */
	//@{
	/// Do³¹cz t¹ flagê dodatkowo | podczas zapisywania, aby wymusiæ nag³ówek BOM.
	FILE_ENCODING_FORCE_BOM  = 0x00010000,
	/// Do³¹cz t¹ flagê dodatkowo | podczas zapisywania, aby wymusiæ brak nag³ówka BOM.
	FILE_ENCODING_NO_BOM     = 0x00020000,
	/// Do³¹cz t¹ flagê dodatkowo podczas odczytywania, aby automatycznie wykryæ kodowanie.
	FILE_ENCODING_AUTODETECT = 0x00040000,
	//@}
};

/// Zapisuje tekst Unicode do pliku w podanym kodowaniu
/** Musisz podaæ jedn¹ flagê common::FILE_ENCODING z kodowaniem.
Mo¿esz dodaæ jedn¹ z flag wymuszaj¹c nag³ówek BOM lub jego brak. */
void SaveUnicodeToFile(const tstring &FileName, const wstring &Data, unsigned Encoding);
void SaveUnicodeToFile(const tstring &FileName, const wchar_t *Data, unsigned NumChars, unsigned Encoding);

void SaveUnicodeToStream(Stream *Dest, const wstring &Data, unsigned Encoding);
void SaveUnicodeToStream(Stream *Dest, const wchar_t *Data, unsigned NumChars, unsigned Encoding);

/// Wczytuje plik tekstowy do ³añcucha Unicode
/** \param Encoding:
- Mo¿esz podaæ samo kodowanie, aby wczytaæ w z góry za³o¿onym kodowaniu (du¿o szybsze!)
- Mo¿esz podaæ samo common::FILE_ENCODING_AUTODETECT, aby automatycznie wykryæ kodowanie.
- Mo¿esz podaæ common::FILE_ENCODING_AUTODETECT i dodaæ jedno z kodowañ, aby automatycznie wykryæ,
  a w przypadku w¹tpliwoœci wybraæ podane.
\param OutEncoding - zwraca wykryte kodowanie. Jeœli ciê nie interesujê, mo¿esz podaæ NULL.
*/
void LoadUnicodeFromFile(const tstring &FileName, wstring *Out, unsigned Encoding, FILE_ENCODING *OutEncoding = NULL);
/// Wczytuje plik tekstowy do ³añcucha ANSI.
/** Wspiera te same kodowania co common::OutEncoding. */
void LoadUnicodeFromFile(const tstring &FileName, string *Out, unsigned Encoding, FILE_ENCODING *OutEncoding = NULL);

void LoadUnicodeFromStream(SeekableStream *Src, wstring *Out, unsigned Encoding, FILE_ENCODING *OutEncoding = NULL);
void LoadUnicodeFromStream(SeekableStream *Src, string *Out, unsigned Encoding, FILE_ENCODING *OutEncoding = NULL);

#endif

//@}
// code_files

} // namespace common

#endif
