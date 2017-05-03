/** \file
\brief Stream class hierarchy
\author Adam Sawicki - sawickiap@poczta.onet.pl - http://asawicki.info/ \n

Part of CommonLib library. \n
Encoding UTF-8, end of line CR+LF \n
License: GNU LGPL. \n
Documentation: \ref Module_Stream \n
Module components: \ref code_stream
*/
#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif
#ifndef COMMON_STREAM_H_
#define COMMON_STREAM_H_

namespace common
{

/** \addtogroup code_stream Stream Module
Dokumentacja: \ref Module_Stream \n
Nag³ówek: Stream.hpp */
//@{

extern const size_t BUFFER_SIZE;

/// \internal
void _ThrowBufEndError(const tchar *File, int Line);

/// Tolerancja b³êdów podczas dekodowania danych zapisanych tekstowo
enum DECODE_TOLERANCE
{
	DECODE_TOLERANCE_NONE,       ///< ¯adne dodatkowe znaki nie bêd¹ tolerowane
	DECODE_TOLERANCE_WHITESPACE, ///< Bia³e znaki bêd¹ ignorowane i nie spowoduj¹ b³êdu
	DECODE_TOLERANCE_ALL,        ///< Wszelkie nieznane znaki bêd¹ ignorowane i nie spowoduj¹ b³¹du
};


/// Abstrakcyjna klasa bazowa strumieni danych binarnych
class Stream
{
public:
	virtual ~Stream() { }

	/** \name Zapisywanie */
	//@{

	/// Zapisuje dane
	/** (W oryginale: zg³asza b³¹d) */
	virtual void Write(const void *Data, size_t Size);
	virtual void Flush() { }

	/// Zapisuje dane, sama odczytuje rozmiar przekazanej zmiennej
	template <typename T>
	void WriteEx(const T &x) { return Write(&x, sizeof(x)); }
	/// Zapisuje d³ugoœæ na 1B i ³añcuch
	/** Jeœli ³añcuch za d³ugi - b³¹d. */
	void WriteString1(const string &s);
	void WriteString1(const char *s);
	void WriteString1(const char *s, size_t NumChars);
#ifdef _WIN32
	void WriteString1(const wstring &s);
	void WriteString1(const wchar_t *s);
	void WriteString1(const wchar_t *s, size_t NumChars);
#endif
	/// Zapisuje d³ugoœæ na 2B i ³añcuch
	/** Jeœli ³añcuch za d³ugi - b³¹d. */
	void WriteString2(const string &s);
	void WriteString2(const char *s);
	void WriteString2(const char *s, size_t NumChars);
#ifdef _WIN32
	void WriteString2(const wstring &s);
	void WriteString2(const wchar_t *s);
	void WriteString2(const wchar_t *s, size_t NumChars);
#endif
	/// Zapisuje d³ugoœæ na 4B i ³añcuch
	void WriteString4(const string &s);
	void WriteString4(const char *s);
	void WriteString4(const char *s, size_t NumChars);
#ifdef _WIN32
	void WriteString4(const wstring &s);
	void WriteString4(const wchar_t *s);
	void WriteString4(const wchar_t *s, size_t NumChars);
#endif
	/// Zapisuje ³añcuch bez d³ugoœci
	void WriteStringF(const string &s);
#ifdef _WIN32
	void WriteStringF(const wstring &s);
#endif
	/// Zapisuje wartoœæ logiczn¹ za pomoc¹ jednego bajtu
	void WriteBool(bool b);

	//@}

	/** \name Odczytywanie */
	//@{

	/// Odczytuje dane
	/** Jeœli osi¹gniêto koniec, funkcja nie zwraca b³êdu, a liczba odczytanych bajtów to 0.
	(W oryginale: zg³asza b³¹d)
	\param Size Liczba bajtów do odczytania
	\return Zwraca liczbê bajtów, jakie uda³o siê odczytaæ
	*/
	virtual size_t Read(void *Data, size_t MaxLength);
	/// Odczytuje tyle bajtów, ile siê za¿¹da
	/** Jeœli koniec pliku albo jeœli odczytano mniej, zg³asza b³¹d.
	(Mo¿na j¹ prze³adowaæ, ale nie trzeba - ma swoj¹ wersjê oryginaln¹)
	*/
	virtual void MustRead(void *Data, size_t Length);
	/// Tak samo jak MustRead(), ale sama odczytuje rozmiar przekazanej zmiennej
	/** Zwraca true, jeœli osi¹gniêto koniec strumienia
	(W oryginale: zg³asza b³¹d)
	*/
	virtual bool End();
	/// Pomija co najwy¿ej podan¹ liczbê bajtów (chyba ¿e wczeœniej osi¹gniêto koniec).
	/** (Mo¿na j¹ prze³adowaæ, ale nie trzeba - ma swoj¹ wersjê oryginaln¹)
	\return Zwraca liczbê pominiêtych bajtów.
	*/
	virtual size_t Skip(size_t MaxLength);

	/// Odczytuje dane, sama odczytuje rozmiar przekazanej zmiennej
	template <typename T>
	void ReadEx(T *x) { MustRead(x, sizeof(*x)); }
	/// Odczytuje ³añcuch poprzedzony rozmiarem 1B
	void ReadString1(string *s);
	/// Odczytuje ³añcuch poprzedzony rozmiarem 2B
	void ReadString2(string *s);
	/// Odczytuje ³añcuch poprzedzony rozmiarem 4B
	void ReadString4(string *s);
	/// Odczytuje ³añcuch bez zapisanego rozmiaru
	void ReadStringF(string *s, size_t NumChars);
	/// Odczytuje tyle znaków do ³añcucha, ile siê da do koñca strumienia
	void ReadStringToEnd(string *s);
	/// Wersje powy¿szych funkcji dla Unicode
#ifdef _WIN32
	void ReadString1(wstring *s);
	void ReadString2(wstring *s);
	void ReadString4(wstring *s);
	void ReadStringF(wstring *s, size_t NumChars);
	void ReadStringToEnd(wstring *s);
#endif
	/// Odczytuje wartoœæ logiczn¹ za pomoc¹ jednego bajtu
	void ReadBool(bool *b);
	/// Pomija koniecznie podan¹ liczbê bajtów.
	/** Jeœli wczeœniej osi¹gniêto koniec, zg³asza b³¹d. */
	void MustSkip(size_t Length);

	//@}

	/** \name Kopiowanie */
	//@{
	/// Odczytuje podan¹ co najwy¿ej liczbê bajtów z podanego strumienia
	/** \return Zwraca liczbê odczytanych bajtów */
	size_t CopyFrom(Stream *s, size_t Size);
	/// Odczytuje dok³adnie podan¹ liczbê bajtów z podanego strumienia
	/** Jeœli odczyta mniej, zg³asza b³¹d. */
	void MustCopyFrom(Stream *s, size_t Size);
	/// Odczytuje dane do koñca z podanego strumienia
	size_t CopyFromToEnd(Stream *s);
	//@}
};

/// Abstrakcyjna klasa bazowa strumieni pozwalaj¹cych na odczytywanie rozmiaru i zmianê pozycji
class SeekableStream : public Stream
{
public:
	// ======== Implementacja Stream ========
	/** W Stream domyœlnie rzuca b³¹d, tu w SeekableStream dostaje ju¿ wersjê oryginaln¹. Mo¿na j¹ dalej nadpisaæ. */
	virtual bool End();
	/** W Stream ma wersjê oryginaln¹, tu w SeekableStream dostaje lepsz¹. Mo¿na j¹ dalej nadpisaæ. */
	virtual size_t Skip(size_t MaxLength);

	/// Zwraca rozmiar
	/** W oryginale: zg³asza b³¹d. */
	virtual uint64 GetSize();
	/// Zwraca pozycjê wzglêdem pocz¹tku
	/** W oryginale: zg³asza b³¹d. */
	virtual int64 GetPos();
	/// Ustawia pozycjê wzglêdem pocz¹tku
	/** W oryginale: zg³asza b³¹d.
	\param pos Musi byæ dodatnie. */
	virtual void SetPos(int64 pos);
	/// Ustawia pozycjê wzglêdem bie¿¹cej
	/** Mo¿na j¹ nadpisaæ, ale ma wersjê oryginaln¹
	\param pos mo¿e byæ dodatnie albo ujemne. */
	virtual void SetPosFromCurrent(int64 pos);
	/// Ustawia pozycjê wzglêdem koñcowej
	/** Mo¿na j¹ nadpisaæ, ale ma wersjê oryginaln¹.
	Np. abu ustawiæ na ostatni znak, przesuñ do -1. */
	virtual void SetPosFromEnd(int64 pos);
	/// Ustawia pozycjê na pocz¹tku
	/** Mo¿na j¹ nadpisaæ, ale ma wersjê oryginaln¹. */
	virtual void Rewind();
	/// Ustawia rozmiar na podany
	/** Po tym wywo³aniu pozycja kursora jest niezdefiniowana.
	W oryginale: zg³asza b³¹d. */
	virtual void SetSize(uint64 Size);
	/// Obcina strumieñ od bie¿¹cej pozycji do koñca, czyli ustawia taki rozmiar jak bie¿¹ca pozycja.
	/** Mo¿na j¹ nadpisaæ, ale ma wersjê oryginaln¹. */
	virtual void Truncate();
	/// Opró¿nia strumieñ
	/** Mo¿na j¹ nadpisaæ, ale ma wersjê oryginaln¹. */
	virtual void Clear();
};

/// Strumieñ pusty - nic nie robi
/**
Zapis po prostu udaje ¿e zapisuje a nic nie robi.
Odczyt udaje, ¿e plik jest pusty.
Przestawianie kursora, ustawianie pozycji itp. udaje siê, ale nic nie robi.
*/
class NullStream : public SeekableStream
{
public:
	virtual void Write(const void *Data, size_t Size) { }
	virtual size_t Read(void *Data, size_t MaxLength) { return 0; }
	virtual bool End() { return true; }
	virtual size_t Skip(size_t MaxLength) { return 0; }
	virtual uint64 GetSize() { return 0; }
	virtual int64 GetPos() { return 0; }
	virtual void SetPos(int64 pos) { }
	virtual void SetPosFromCurrent(int64 pos) { }
	virtual void SetPosFromEnd(int64 pos) { }
	virtual void Rewind() { }
	virtual void SetSize(uint64 Size) { }
	virtual void Truncate() { }
	virtual void Clear() { }
};

/// Nak³adka przyspieszaj¹ca zapisuj¹ca do strumienia pojedyncze znaki i inne rzeczy.
/**
- Nie jest strumieniem.
- Tylko zapis.
- Jest szybka, poniewa¿:
  -# Nie ma metod wirtualnych.
  -# Buforuje, wiêc nie u¿ywa za ka¿dym razem metod wirtualnych strumienia.
- Sama zapisuje dane pozosta³e w buforze w destruktorze.
  Jeœli chcesz to zrobiæ wczeœniej i z kontrol¹ b³êdów, wywo³aj CharWriter::Flush().
- W trybie Debug dzia³a nawet trochê wolniej ni¿ zapisywanie po bajcie prosto do strumienia.
  W trybie Release daje przyspieszenie prawie 10x wzglêdem zapisywania po bajcie prosto do strumienia.
- Celowo nie u¿ywam w tej klasie ERR_TRY .. ERR_CATCH_FUNC, ¿eby by³o szybciej.
*/
class CharWriter
{
private:
	Stream *m_Stream;
	std::vector<char> m_Buf;
	size_t m_BufIndex;

	// Wykonuje Flush danych z bufora do strumienia nie sprawdzaj¹c ju¿, czy bufor nie jest pusty.
	// Oczywiœcie zeruje te¿ BufIndex.
	void DoFlush() { m_Stream->Write(&m_Buf[0], m_BufIndex); m_BufIndex = 0; }

public:
	CharWriter(Stream *a_Stream) : m_Stream(a_Stream), m_BufIndex(0) { m_Buf.resize(BUFFER_SIZE); }
	~CharWriter();

	/// Zapisuje pojedynczy znak
	void WriteChar(char Ch) { if (m_BufIndex == BUFFER_SIZE) DoFlush(); m_Buf[m_BufIndex++] = Ch; }
	/// Zapisuje po prostu znaki ³añcucha, bez ¿adnej d³ugoœci
	void WriteString(const string &s);
	/// Zapisuje ³añcuch zakoñczony zerem
	void WriteString(const char *s) { WriteString(s, strlen(s)); }
	/// Zapisuje ³añcuch podanej d³ugoœci
	void WriteString(const char *s, size_t NumChars) { WriteData(s, NumChars); }
	/// Zapisuje surowe dane binarne
	void WriteData(const void *Data, size_t Length);
	/// Zapisuje dowoln¹ informacjê przerobion¹ na postaæ tekstow¹, o ile da siê przerobiæ
	template <typename T> void WriteEx(const T &x) { string s; SthToStr<T>(&s, x); WriteString(s); }
	/// Wymusza opró¿nienie bufora i wys³anie pozosta³ych w nim danych do strumienia
	void Flush() { if (m_BufIndex > 0) DoFlush(); }
};

#ifdef _WIN32
/// Nak³adka przyspieszaj¹ca zapisuj¹ca do strumienia pojedyncze znaki Unicode.
/** Uwaga! Wersja Unicode nie posiada zapisywania surowych danych binarnych. */
class WCharWriter
{
private:
	Stream *m_Stream;
	std::vector<wchar_t> m_Buf;
	size_t m_BufIndex;

	// Wykonuje Flush danych z bufora do strumienia nie sprawdzaj¹c ju¿, czy bufor nie jest pusty.
	// Oczywiœcie zeruje te¿ BufIndex.
	void DoFlush() { m_Stream->Write(&m_Buf[0], m_BufIndex * sizeof(wchar_t)); m_BufIndex = 0; }

public:
	WCharWriter(Stream *a_Stream) : m_Stream(a_Stream), m_BufIndex(0) { m_Buf.resize(BUFFER_SIZE); }
	~WCharWriter();

	/// Zapisuje pojedynczy znak
	void WriteChar(wchar_t Ch) { if (m_BufIndex == BUFFER_SIZE) DoFlush(); m_Buf[m_BufIndex++] = Ch; }
	/// Zapisuje po prostu znaki ³añcucha, bez ¿adnej d³ugoœci
	void WriteString(const wstring &s);
	/// Zapisuje ³añcuch zakoñczony zerem
	void WriteString(const wchar_t *s) { WriteString(s, wcslen(s)); }
	/// Zapisuje ³añcuch podanej d³ugoœci
	void WriteString(const wchar_t *s, size_t NumChars);
	/// Wymusza opró¿nienie bufora i wys³anie pozosta³ych w nim danych do strumienia
	void Flush() { if (m_BufIndex > 0) DoFlush(); }
};
#endif

/// Nak³adka przyspieszaj¹ca odczytuj¹ca ze strumienia pojedyncze znaki i inne rzeczy.
/**
- Nie jest strumieniem.
- Tylko odczyt.
- Jest szybka, poniewa¿:
  -# Nie ma metod wirtualnych.
  -# Buforuje, wiêc nie u¿ywa za ka¿dym razem metod wirtualnych strumienia.
- Szybkoœæ wzglêdem wczytywania pojedynczych znaków prosto ze strumienia: Debug = 10 razy, Release = 450 razy!
- Celowo nie u¿ywam w tej klasie ERR_TRY .. ERR_CATCH_FUNC, ¿eby by³o szybciej.
*/
class CharReader
{
private:
	Stream *m_Stream;
	std::vector<char> m_Buf;
	// Miejsce, do którego doczyta³em z bufora
	size_t m_BufBeg;
	// Miejsce, do którego bufor jest wype³niony danymi
	size_t m_BufEnd;

	// Wczytuje now¹ porcjê danych do strumienia
	// Wywo³ywaæ tylko kiedy bufor siê skoñczy³, tzn. m_BufBeg == m_BufEnd.
	// Jeœli skoñczy³ siê strumieñ i nic nie uda³o siê wczytaæ, zwraca false.
	// Jeœli nie, zapewnia ¿e w buforze bêdzie co najmniej jeden znak.
	bool EnsureNewChars();

public:
	CharReader(Stream *a_Stream) : m_Stream(a_Stream), m_BufBeg(0), m_BufEnd(0) { m_Buf.resize(BUFFER_SIZE); }

	/// Czy jesteœmy na koñcu danych?
	bool End() {
		return (m_BufBeg == m_BufEnd) && m_Stream->End(); // Nic nie zosta³o w buforze i nic nie zosta³o w strumieniu.
	}
	/// Jeœli mo¿na odczytaæ nastêpny znak, wczytuje go i zwraca true.
	/** Jeœli nie, nie zmienia Out i zwraca false. Oznacza to koniec strumienia. */
	bool ReadChar(char *Out) { if (m_BufBeg == m_BufEnd) { if (!EnsureNewChars()) return false; } *Out = m_Buf[m_BufBeg++]; return true; }
	/// Jeœli mo¿na odczytaæ nastêpny znak, wczytuje go.
	/** Jeœli nie, rzuca wyj¹tek. */
	char MustReadChar() { if (m_BufBeg == m_BufEnd) { if (!EnsureNewChars()) _ThrowBufEndError(__TFILE__, __LINE__); } return m_Buf[m_BufBeg++]; }
	/// Jeœli mo¿na odczytaæ nastêpny znak, podgl¹da go zwracaj¹c przez Out i zwraca true. Nie przesuwa "kursora".
	/** Jeœli nie, nie zmienia Out i zwraca false. Oznacza to koniec strumienia. */
	bool PeekChar(char *Out) { if (m_BufBeg == m_BufEnd) { if (!EnsureNewChars()) return false; } *Out = m_Buf[m_BufBeg]; return true; }
	/// Jeœli mo¿na odczytaæ nastêpny znak, podgl¹da go zwracaj¹c. Nie przesuwa "kursora".
	/** Jeœli nie, rzuca wyj¹tek. */
	char MustPeekChar() { if (m_BufBeg == m_BufEnd) { if (!EnsureNewChars()) _ThrowBufEndError(__TFILE__, __LINE__); } return m_Buf[m_BufBeg]; }
	/// Wczytuje co najwy¿ej MaxLength znaków do podanego stringa.
	/** StringStream jest czyszczony - nie musi byæ pusty ani zaalokowany. (???)
	\return Zwraca liczbê odczytanych znaków. Mniej ni¿ ¿¹dano oznacza koniec strumienia. */
	size_t ReadString(string *Out, size_t MaxLength);
	/// Wczytuje Length znaków do podanego stringa.
	/** StringStream jest czyszczony - nie musi byæ pusty ani zaalokowany. (???)
	Jeœli nie uda siê odczytaæ wszystkich Length znaków, rzuca wyj¹tek. */
	void MustReadString(string *Out, size_t Length);
	/// Wczytuje co najwy¿ej MaxLength znaków pod podany wskaŸnik.
	/** \param[out] Out musi byæ tablic¹ zaalokowan¹ do d³ugoœci co najmniej MaxLength.
	\return Zwraca liczbê odczytanych znaków. Mniej ni¿ ¿¹dano oznacza koniec strumienia. */
	size_t ReadString(char *Out, size_t MaxLength);
	/// Wczytuje Length znaków pod podany wskaŸnik.
	/** Jeœli nie uda siê odczytaæ wszystkich Length znaków, rzuca wyj¹tek.
	\param[out] Out musi byæ tablic¹ zaalokowan¹ do d³ugoœci co najmniej Length. */
	void MustReadString(char *Out, size_t Length);
	/// Wczytuje co najwy¿ej MaxLength bajtów pod podany wskaŸnik.
	/** \param[out] Out musi byæ tablic¹ zaalokowan¹ do d³ugoœci co najmniej MaxLength bajtów.
	\return Zwraca liczbê odczytanych bajtów. Mniej ni¿ ¿¹dano oznacza koniec strumienia. */
	size_t ReadData(void *Out, size_t MaxLength);
	/// Wczytuje Length bajtów pod podany wskaŸnik.
	/** Jeœli nie uda siê odczytaæ wszystkich Length bajtów, rzuca wyj¹tek.
	\param[out] Out musi byæ tablic¹ zaalokowan¹ do d³ugoœci co najmniej Length bajtów. */
	void MustReadData(void *Out, size_t Length);
	/// Jeœli jest nastêpny znak do odczytania, pomija go i zwraca true.
	/** Jeœli nie, zwraca false. Oznacza to koniec strumienia. */
	bool SkipChar() { if (m_BufBeg == m_BufEnd) { if (!EnsureNewChars()) return false; } m_BufBeg++; return true; }
	/// Jeœli jest nastêpny znak do odczytania, pomija go.
	/** Jeœli nie, rzuca wyj¹tek. */
	void MustSkipChar() { if (m_BufBeg == m_BufEnd) { if (!EnsureNewChars()) _ThrowBufEndError(__TFILE__, __LINE__); } m_BufBeg++; }
	/// Pomija co najwy¿ej MaxLength znaków.
	/** \return Zwraca liczbê pominiêtych znaków. Mniej ni¿ ¿¹dano oznacza koniec strumienia. */
	size_t Skip(size_t MaxLength);
	/// Pomija Length znaków.
	/** Jeœli nie uda siê pomin¹æ wszystkich Length znaków, rzuca wyj¹tek. */
	void MustSkip(size_t Length);
	/// Wczytuje linijkê tekstu i jeœli siê uda³o, zwraca true.
	/** Jeœli nie uda³o siê wczytaæ ani jednego znaku, bo to ju¿ koniec strumienia, zwraca false.
	Czyta znaki do napotkania koñca strumienia lub koñca wiersza, którym s¹ <tt>"\r"</tt>, <tt>"\n"</tt> i <tt>"\r\n"</tt>.
	Koniec wiersza przeskakuje, ale nie do³¹cza go do Out. */
	bool ReadLine(string *Out);
	/// Wczytuje linijkê tekstu.
	/** Jeœli nie uda³o siê wczytaæ ani jednego znaku, bo to ju¿ koniec strumienia, rzuca wyj¹tek.
	Czyta znaki do napotkania koñca strumienia lub koñca wiersza, którym s¹ <tt>"\r"</tt>, <tt>"\n"</tt> i <tt>"\r\n"</tt>.
	Koniec wiersza przeskakuje, ale nie do³¹cza go do Out. */
	void MustReadLine(string *Out);
};

#ifdef _WIN32
/// Nak³adka przyspieszaj¹ca odczytuj¹ca ze strumienia pojedyncze znaki Unicode.
/** Uwaga! Wersja Unicode nie posiada odczytywania surowych danych binarnych. */
class WCharReader
{
private:
	Stream *m_Stream;
	std::vector<wchar_t> m_Buf;
	// Miejsce, do którego doczyta³em z bufora
	size_t m_BufBeg;
	// Miejsce, do którego bufor jest wype³niony danymi
	size_t m_BufEnd;

	// Wczytuje now¹ porcjê danych do strumienia
	// Wywo³ywaæ tylko kiedy bufor siê skoñczy³, tzn. m_BufBeg == m_BufEnd.
	// Jeœli skoñczy³ siê strumieñ i nic nie uda³o siê wczytaæ, zwraca false.
	// Jeœli nie, zapewnia ¿e w buforze bêdzie co najmniej jeden znak.
	bool EnsureNewChars();

public:
	WCharReader(Stream *a_Stream) : m_Stream(a_Stream), m_BufBeg(0), m_BufEnd(0) { m_Buf.resize(BUFFER_SIZE); }

	/// Czy jesteœmy na koñcu danych?
	bool End() {
		return (m_BufBeg == m_BufEnd) && m_Stream->End(); // Nic nie zosta³o w buforze i nic nie zosta³o w strumieniu.
	}
	/// Jeœli mo¿na odczytaæ nastêpny znak, wczytuje go i zwraca true.
	/** Jeœli nie, nie zmienia Out i zwraca false. Oznacza to koniec strumienia. */
	bool ReadChar(wchar_t *Out) { if (m_BufBeg == m_BufEnd) { if (!EnsureNewChars()) return false; } *Out = m_Buf[m_BufBeg++]; return true; }
	/// Jeœli mo¿na odczytaæ nastêpny znak, wczytuje go.
	/** Jeœli nie, rzuca wyj¹tek. */
	wchar_t MustReadChar() { if (m_BufBeg == m_BufEnd) { if (!EnsureNewChars()) _ThrowBufEndError(__TFILE__, __LINE__); } return m_Buf[m_BufBeg++]; }
	/// Jeœli mo¿na odczytaæ nastêpny znak, podgl¹da go zwracaj¹c przez Out i zwraca true. Nie przesuwa "kursora".
	/** Jeœli nie, nie zmienia Out i zwraca false. Oznacza to koniec strumienia. */
	bool PeekChar(wchar_t *Out) { if (m_BufBeg == m_BufEnd) { if (!EnsureNewChars()) return false; } *Out = m_Buf[m_BufBeg]; return true; }
	/// Jeœli mo¿na odczytaæ nastêpny znak, podgl¹da go zwracaj¹c. Nie przesuwa "kursora".
	/** Jeœli nie, rzuca wyj¹tek. */
	wchar_t MustPeekChar() { if (m_BufBeg == m_BufEnd) { if (!EnsureNewChars()) _ThrowBufEndError(__TFILE__, __LINE__); } return m_Buf[m_BufBeg]; }
	/// Wczytuje co najwy¿ej MaxLength znaków do podanego stringa.
	/** StringStream jest czyszczony - nie musi byæ pusty ani zaalokowany. (???)
	\return Zwraca liczbê odczytanych znaków. Mniej ni¿ ¿¹dano oznacza koniec strumienia. */
	size_t ReadString(wstring *Out, size_t MaxLength);
	/// Wczytuje Length znaków do podanego stringa.
	/** StringStream jest czyszczony - nie musi byæ pusty ani zaalokowany. (???)
	Jeœli nie uda siê odczytaæ wszystkich Length znaków, rzuca wyj¹tek. */
	void MustReadString(wstring *Out, size_t Length);
	/// Wczytuje co najwy¿ej MaxLength znaków pod podany wskaŸnik.
	/** \param[out] Out musi byæ tablic¹ zaalokowan¹ do d³ugoœci co najmniej MaxLength.
	\return Zwraca liczbê odczytanych znaków. Mniej ni¿ ¿¹dano oznacza koniec strumienia. */
	size_t ReadString(wchar_t *Out, size_t MaxLength);
	/// Wczytuje Length znaków pod podany wskaŸnik.
	/** Jeœli nie uda siê odczytaæ wszystkich Length znaków, rzuca wyj¹tek.
	\param[out] Out musi byæ tablic¹ zaalokowan¹ do d³ugoœci co najmniej Length. */
	void MustReadString(wchar_t *Out, size_t Length);
	/// Jeœli jest nastêpny znak do odczytania, pomija go i zwraca true.
	/** Jeœli nie, zwraca false. Oznacza to koniec strumienia. */
	bool SkipChar() { if (m_BufBeg == m_BufEnd) { if (!EnsureNewChars()) return false; } m_BufBeg++; return true; }
	/// Jeœli jest nastêpny znak do odczytania, pomija go.
	/** Jeœli nie, rzuca wyj¹tek. */
	void MustSkipChar() { if (m_BufBeg == m_BufEnd) { if (!EnsureNewChars()) _ThrowBufEndError(__TFILE__, __LINE__); } m_BufBeg++; }
	/// Pomija co najwy¿ej MaxLength znaków.
	/** \return Zwraca liczbê pominiêtych znaków. Mniej ni¿ ¿¹dano oznacza koniec strumienia. */
	size_t Skip(size_t MaxLength);
	/// Pomija Length znaków.
	/** Jeœli nie uda siê pomin¹æ wszystkich Length znaków, rzuca wyj¹tek. */
	void MustSkip(size_t Length);
	/// Wczytuje linijkê tekstu i jeœli siê uda³o, zwraca true.
	/** Jeœli nie uda³o siê wczytaæ ani jednego znaku, bo to ju¿ koniec strumienia, zwraca false.
	Czyta znaki do napotkania koñca strumienia lub koñca wiersza, którym s¹ <tt>"\r"</tt>, <tt>"\n"</tt> i <tt>"\r\n"</tt>.
	Koniec wiersza przeskakuje, ale nie do³¹cza go do Out. */
	bool ReadLine(wstring *Out);
	/// Wczytuje linijkê tekstu.
	/** Jeœli nie uda³o siê wczytaæ ani jednego znaku, bo to ju¿ koniec strumienia, rzuca wyj¹tek.
	Czyta znaki do napotkania koñca strumienia lub koñca wiersza, którym s¹ <tt>"\r"</tt>, <tt>"\n"</tt> i <tt>"\r\n"</tt>.
	Koniec wiersza przeskakuje, ale nie do³¹cza go do Out. */
	void MustReadLine(wstring *Out);
};
#endif

#ifdef _UNICODE
	#define TCharWriter WCharWriter
	#define TCharReader WCharReader
#else
	#define TCharWriter CharWriter
	#define TCharReader CharReader
#endif

/// Strumieñ pamiêci statycznej
/** Strumieñ ma sta³¹ d³ugoœæ i nie mo¿e byæ rozszerzany - ani automatycznie, ani rêcznie. */
class MemoryStream : public SeekableStream
{
private:
	char *m_Data;
	size_t m_Size;
	ptrdiff_t m_Pos;
	bool m_InternalAlloc;

public:
	/** \param Data Podaj NULL, aby strumieñ sam zaalkowa³, a w destruktorze zwolni³ pamiêæ.
	Podaj wskaŸnik, jeœli ma korzystaæ z twojego obszaru pamiêci.
	*/
	MemoryStream(size_t Size, void *Data = 0);
	virtual ~MemoryStream();

	virtual void Write(const void *Data, size_t Size);
	virtual size_t Read(void *Data, size_t Size);
	virtual void MustRead(void *Data, size_t Size);

	virtual uint64 GetSize();
	virtual int64 GetPos();
	virtual void SetPos(int64 pos);
	virtual void Rewind();

	char *Data() { return m_Data; }
};

/// Strumieñ pamiêci, ale dynamicznej.
/** Strumieñ mo¿na rozszerzaæ i sam te¿ siê rozszerza. */
class VectorStream : public SeekableStream
{
private:
	char *m_Data;
	size_t m_Size;
	size_t m_Capacity;
	ptrdiff_t m_Pos;

	// Zmienia capacity, a tym samym przealokowuje pamiêæ.
	// O niczym sama nie decyduje ani niczego nie sprawdza.
	void Reserve(size_t NewCapacity);

public:
	VectorStream();
	virtual ~VectorStream();

	virtual void Write(const void *Data, size_t Size);
	virtual size_t Read(void *Data, size_t Size);
	virtual void MustRead(void *Data, size_t Size);
	virtual uint64 GetSize() { return m_Size; }
	virtual int64 GetPos() { return m_Pos; }
	virtual void SetPos(int64 pos) { m_Pos = (ptrdiff_t)pos; }
	virtual void Rewind() { m_Pos = 0; }
	virtual void SetSize(uint64 Size);

	size_t GetCapacity() { return m_Capacity; }
	void SetCapacity(size_t Capacity);
	/** Nie musi pozostaæ aktualny po wywo³aniach metod tej klasy!
	(mo¿e zostaæ przealokowany) */
	char *Data() { return m_Data; }
};

/// Strumieñ oparty na ³añcuchu std::string
/** Potrafi siê rozszerzaæ. */
class StringStream : public SeekableStream
{
private:
	std::string *m_Data;
	ptrdiff_t m_Pos;
	bool m_InternalAlloc;

public:
	/** \param Data Podaj NULL, aby strumieñ sam zaalokowa³, a w destruktorze zwolni³ ³añcuch.
	Podaj wskaŸnik, jeœli ma korzystaæ z twojego obszaru pamiêci. */
	StringStream(string *Data = 0);
	virtual ~StringStream();

	virtual void Write(const void *Data, size_t Size);
	virtual size_t Read(void *Data, size_t Size);
	virtual void MustRead(void *Data, size_t Size);
	virtual uint64 GetSize() { return m_Data->length(); }
	virtual int64 GetPos() { return m_Pos; }
	virtual void SetPos(int64 pos) { m_Pos = (ptrdiff_t)pos; }
	virtual void Rewind() { m_Pos = 0; }
	virtual void SetSize(uint64 Size);
	virtual void Clear() { m_Data->clear(); }

	size_t GetCapacity() { return m_Data->capacity(); }
	void SetCapacity(size_t Capacity) { m_Data->reserve(Capacity); }
	string *Data() { return m_Data; }
};

/// Abstrakcyjna klasa bazowa nak³adek na strumienie
class OverlayStream : public Stream
{
private:
	Stream *m_Stream;

public:
	OverlayStream(Stream *a_Stream) : m_Stream(a_Stream) { }

	Stream * GetStream() { return m_Stream; }

	// ======== Implementacja Stream ========
	virtual bool End() { return m_Stream->End(); }
	virtual void Flush() { m_Stream->Flush(); }
};

/// Nak³adka na strumieñ zliczaj¹ca iloœæ zapisanych i odczytanych bajtów
class CounterOverlayStream : public OverlayStream
{
private:
	uint64 m_WriteCounter;
	uint64 m_ReadCounter;

public:
	CounterOverlayStream(Stream *a_Stream);

	// ====== Implementacja Stream ======
	virtual void Write(const void *Data, size_t Size);
	virtual size_t Read(void *Data, size_t Size);
	virtual void MustRead(void *Data, size_t Size);
    virtual void Flush() { GetStream()->Flush(); }

	uint64 GetWriteCounter() { return m_WriteCounter; }
	uint64 GetReadCounter() { return m_ReadCounter; }
	void ResetWriteCounter() { m_WriteCounter = 0; }
	void ResetReadCounter() { m_ReadCounter = 0; }
};

/// Nak³adka na strumieñ ograniczaj¹ca liczbê odczytywanych i zapisywanych bajtów.
/** Jesli przekroczy limit podczas zapisywania, zapisuje ile siê da i rzuca b³¹d zapisu.
Jeœli przekroczy limit podczas odczytywania zachowuje siê tak, jakby strumieñ osi¹gn¹³ koniec. */
class LimitOverlayStream : public OverlayStream
{
private:
	// Przechowuj¹ aktualny limit, jaki pozosta³.
	// S¹ zmniejszane przez funkcje odczytuj¹ce i zapisuj¹ce.
	uint64 m_WriteLimit;
	uint64 m_ReadLimit;

public:
	LimitOverlayStream(Stream *a_Stream, uint64 WriteLimit, uint64 ReadLimit);

	// ====== Implementacja Stream ======
	virtual void Write(const void *Data, size_t Size);
	virtual size_t Read(void *Data, size_t Size);
	virtual void Flush();

	/// Ustawia nowy limit
	void SetWriteLimit(uint64 WriteLimit);
	/// Ustawia nowy limit
	void SetReadLimit(uint64 ReadLimit);
};

class BufferingStream : public OverlayStream
{
public:
	BufferingStream(Stream *stream, size_t readBufSize, size_t writeBufSize);
	~BufferingStream();

	// ====== Implementacja Stream ======
	virtual void Write(const void *Data, size_t Size);
	virtual void Flush();
	virtual size_t Read(void *Data, size_t MaxLength);
	virtual bool End();
	virtual size_t Skip(size_t MaxLength);

	/// Writes single char/byte
	void WriteChar(char ch) { if (m_WriteBufIndex == m_WriteBufSize) DoFlush(); m_WriteBuf[m_WriteBufIndex++] = ch; }

private:
	size_t m_ReadBufSize, m_WriteBufSize;
	std::vector<char> m_ReadBuf, m_WriteBuf;
	size_t m_ReadBufBeg, m_ReadBufEnd, m_WriteBufIndex;

	void DoFlush();
	// Wczytuje now¹ porcjê danych do bufora
	// Wywo³ywaæ tylko kiedy bufor siê skoñczy³, tzn. m_ReadBufBeg == m_ReadBufEnd.
	// Jeœli skoñczy³ siê strumieñ i nic nie uda³o siê wczytaæ, zwraca false.
	// Jeœli nie, zapewnia ¿e w buforze bêdzie co najmniej jeden znak.
	bool EnsureNewChars();
};

/// Zapisuje zapisywane dane do wielu pod³¹czonych do niego strumieni na raz.
class MultiWriterStream : public Stream
{
private:
	std::vector<Stream*> m_Streams;

public:
	MultiWriterStream() { }
	MultiWriterStream(Stream *a_Streams[], size_t StreamCount);

	/// Dodaje strumieñ do listy.
	void AddStream(Stream *a_Stream) { m_Streams.push_back(a_Stream); }
	/// Usuwa strumieñ z listy. Jeœli takiego nie znajdzie, nic nie robi.
	void RemoveStream(Stream *a_Stream);
	/// Usuwa wszystkie strumienie z listy.
	void ClearStreams() { m_Streams.clear(); }
	size_t GetStreamCount() { return m_Streams.size(); }

	// ======== Implementacja Stream ========
	virtual void Write(const void *Data, size_t Size);
	virtual void Flush();
};

/// Klasa obliczaj¹ca hash 32-bitowy z kolejno podawanych bloków danych
/** Algorytm: Jenkins One-at-a-time hash, http://www.burtleburtle.net/bob/hash/doobs.html
Strumieñ tylko do zapisu.
Prawid³owe u¿ycie: Hash_Calc::Write(), Hash_Calc::Write() (lub inne funkcje zapisuj¹ce), ..., Hash_Calc::Finish().
Po wywo³aniu Hash_Calc::Finish() nie mo¿na dalej zapisywaæ danych!
Hash_Calc::Reset() - rozpoczyna liczenie sumy od nowa. */
class Hash_Calc : public Stream
{
private:
	uint32 m_Hash;

public:
	Hash_Calc() : m_Hash(0) { }

	// ======== Implementacja Stream ========
	virtual void Write(const void *Data, size_t Size);

	/// Koñczy obliczenia i zwraca policzon¹ sumê
	uint32 Finish();
	/// Rozpoczyna liczenie nowej sumy
	void Reset() { m_Hash = 0; }

	// ======== Statyczne ========
	/// Po prostu oblicza sumê kontroln¹ z podanych danych
	static uint32 Calc(const void *Buf, uint32 BufLen);
	static uint32 Calc(const string &s);
#ifdef _WIN32
	static uint32 Calc(const wstring &s);
#endif
};

/// Klasa obliczaj¹ca sumê CRC32 z kolejno podawanych bloków danych
/** Strumieñ tylko do zapisu.
CRC32_Calc::GetResult() - wyliczon¹ dotychczas sumê mo¿na otrzymywaæ w ka¿dej chwili, a potem dalej dodawaæ nowe dane.
CRC32_Calc::Reset() - rozpoczyna liczenie nowej sumy kontrolnej. */
class CRC32_Calc : public Stream
{
private:
	uint m_CRC;

public:
	CRC32_Calc() { m_CRC = 0xFFFFFFFF; }

	// ======== Implementacja Stream ========
	virtual void Write(const void *Data, size_t Size);

	/// Zwraca policzon¹ dotychczas sumê
	uint GetResult() { return ~m_CRC; }
	/// Rozpoczyna liczenie nowej sumy
	void Reset() { m_CRC = 0xFFFFFFFF; }

	// ======== Statyczne ========
	/// Po prostu oblicza sumê kontroln¹ z podanych danych
	static uint Calc(const void *Data, size_t DataLength);
};

/// Suma MD5
/** Suma ma 128 bitów - 16 bajtów. Jest typu uint8[16]. */
struct MD5_SUM
{
	uint8 Data[16];

	uint8 & operator [] (size_t i) { return Data[i]; }
	uint8 operator [] (size_t i) const { return Data[i]; }

	bool operator == (const MD5_SUM &s) const
	{
		return
			Data[ 0] == s.Data[ 0] && Data[ 1] == s.Data[ 1] && Data[ 2] == s.Data[ 2] && Data[ 3] == s.Data[ 3] &&
			Data[ 4] == s.Data[ 4] && Data[ 5] == s.Data[ 5] && Data[ 6] == s.Data[ 6] && Data[ 7] == s.Data[ 7] &&
			Data[ 8] == s.Data[ 8] && Data[ 9] == s.Data[ 9] && Data[10] == s.Data[10] && Data[11] == s.Data[11] &&
			Data[12] == s.Data[12] && Data[13] == s.Data[13] && Data[14] == s.Data[14] && Data[15] == s.Data[15];
	}
	bool operator != (const MD5_SUM &s) const
	{
		return
			Data[ 0] != s.Data[ 0] || Data[ 1] != s.Data[ 1] || Data[ 2] != s.Data[ 2] || Data[ 3] != s.Data[ 3] ||
			Data[ 4] != s.Data[ 4] || Data[ 5] != s.Data[ 5] || Data[ 6] != s.Data[ 6] || Data[ 7] != s.Data[ 7] ||
			Data[ 8] != s.Data[ 8] || Data[ 9] != s.Data[ 9] || Data[10] != s.Data[10] || Data[11] != s.Data[11] ||
			Data[12] != s.Data[12] || Data[13] != s.Data[13] || Data[14] != s.Data[14] || Data[15] != s.Data[15];
	}
	bool operator < (const MD5_SUM &s) const;
	bool operator > (const MD5_SUM &s) const;
	bool operator <= (const MD5_SUM &s) const;
	bool operator >= (const MD5_SUM &s) const;
};

void MD5ToStr(tstring *Out, const MD5_SUM &MD5);
bool StrToMD5(MD5_SUM *Out, const tstring &s);

/// Klasa obliczaj¹ca sumê MD5 z kolejno podawanych bloków danych
/** Strumieñ tylko do zapisu.
Prawid³owe u¿ycie: MD5_Calc::Write(), MD5_Calc::Write() (lub inne funkcje zapisuj¹ce), ..., MD5_Calc::Finish().
Po wywo³aniu Finish nie mo¿na dalej zapisywaæ danych!
MD5_Calc::Reset() - rozpoczyna liczenie sumy od nowa. */
class MD5_Calc : public Stream
{
private:
	uint32 total[2];
	uint32 state[4];
	uint8 buffer[64];

	void Process(uint8 data[64]);

public:
	MD5_Calc();

	// ======== Implementacja Stream ========
	virtual void Write(const void *Data, size_t Size);

	/// Koñczy obliczenia i zwraca policzon¹ sumê
	void Finish(MD5_SUM *Out);
	/// Rozpoczyna liczenie nowej sumy
	void Reset();

	// ======== Statyczne ========
	/// Po prostu oblicza sumê kontroln¹ z podanych danych
	static void Calc(MD5_SUM *Out, const void *Buf, uint32 BufLen);
};

/// Koduje lub dekoduje zapisywane/odczytywane bajty XOR podany bajt lub ci¹g bajtów.
/** Mapuje bezpoœrednio bajty na bajty strumienia do którego jest pod³¹czony,
nic nie buforuje, wiêc mo¿na operowaæ te¿ na strumieniu Stream. */
class XorCoder : public OverlayStream
{
private:
	std::vector<char> m_Buf;
	std::vector<char> m_Key;
	uint m_EncodeKeyIndex;
	uint m_DecodeKeyIndex;

public:
	XorCoder(Stream *a_Stream, char KeyByte);
	XorCoder(Stream *a_Stream, const void *KeyData, size_t KeyLength);
	XorCoder(Stream *a_Stream, const string &Key);

	// ======== Implementacja Stream ========
	virtual void Write(const void *Data, size_t Size);
	virtual size_t Read(void *Data, size_t Size);

	/// Resetuje liczniki tak ¿eby kodowanie i dekodowanie odbywa³o siê od pocz¹tku klucza
	void Reset() { m_EncodeKeyIndex = m_DecodeKeyIndex = 0; }

	// ======== Statyczne ========
	/// Po prostu przetwarza podane dane
	static void Code(void *Out, const void *Data, size_t DataLength, const void *Key, size_t KeyLength);
	/// Po prostu przetwarza podane dane
	static void Code(string *Out, const string &Data, const string &Key);
#ifdef _WIN32
	/// Po prostu przetwarza podane dane
	static void Code(wstring *Out, const wstring &Data, const wstring &Key);
#endif
};

/// Koduje strumieñ binarnych danych wejœciowych na ci¹g z³o¿ony ze znaków '0' i '1' (po 8 na ka¿dy bajt).
class BinEncoder : public OverlayStream
{
private:
	CharWriter m_CharWriter;

public:
	BinEncoder(Stream *a_Stream) : OverlayStream(a_Stream), m_CharWriter(a_Stream) { }

	// ======== Implementacja Stream ========
	virtual void Write(const void *Data, size_t Size);
	virtual void Flush() { m_CharWriter.Flush(); OverlayStream::Flush(); }

	// ======== Statyczne ========
	/// Po prostu koduje podane dane
	/*** \param[out] Out Bufor wyjœciowy Out musi mieæ zaalokowane przynajmniej DataLength * 8 bajtów. */
	static void Encode(char *Out, const void *Data, size_t DataLength);
	/// Po prostu koduje podane dane
	/** \param[out] Out £añcuch wyjœciowy Out nie musi mieæ ¿adnego konkretnego rozmiaru -
	zostaje wyczyszczony i wype³niony od nowa. */
	static void Encode(string *Out, const void *Data, size_t DataLength);
};

/// Dekoduje ci¹g znaków wejœciowych zapisanych binarnie (znakami '0' i '1') na dane binarne.
class BinDecoder : public OverlayStream
{
private:
	CharReader m_CharReader;
	DECODE_TOLERANCE m_Tolerance;

public:
	BinDecoder(Stream *a_Stream, DECODE_TOLERANCE Tolerance = DECODE_TOLERANCE_NONE);

	DECODE_TOLERANCE GetTolerance() { return m_Tolerance; }

	// ======== Implementacja Stream ========
	virtual size_t Read(void *Out, size_t Size);
	virtual bool End() { return m_CharReader.End(); }

	// ======== Statyczne ========
	/// Znajduje d³ugoœæ zakodowanych danych i zwraca true.
	/** Jeœli nie uda³o siê ustaliæ d³ugoœci, zwraca false. */
	static bool DecodeLength(size_t *OutLength, const string &s, DECODE_TOLERANCE Tolerance = DECODE_TOLERANCE_NONE);
	static bool DecodeLength(size_t *OutLength, const char *s, size_t s_Length, DECODE_TOLERANCE Tolerance = DECODE_TOLERANCE_NONE);
	/// Dekoduje dane binarne. Zwraca liczbê zdekodowanych bajtów.
	/** Jeœli nie uda³o siê zdekodowaæ (jakiœ b³¹d), zwraca MAXUINT32 (0xFFFFFFFF).
	\param[out] OutData musi byæ tablic¹ zaalokowan¹ przynajmniej na rozmiar ustalony wywo³aniem DecodeLength()
	lub jeœli nie chcesz wywo³ywaæ DecodeLength(), na d³ugoœæ równ¹ d³ugoœci danych wejœciowych / 8. */
	static size_t Decode(void *OutData, const string &s, DECODE_TOLERANCE Tolerance = DECODE_TOLERANCE_NONE);
	static size_t Decode(void *OutData, const char *s, size_t s_Length, DECODE_TOLERANCE Tolerance = DECODE_TOLERANCE_NONE);
};

/// Koduje strumieñ binarnych danych wejœciowych na zapis szesnastkowy (po 2 znaki na ka¿dy bajt).
class HexEncoder : public OverlayStream
{
private:
	CharWriter m_CharWriter;
	bool m_UpperCase;

public:
	HexEncoder(Stream *a_Stream, bool UpperCase = true) : OverlayStream(a_Stream), m_CharWriter(a_Stream), m_UpperCase(UpperCase) { }

	// ======== Implementacja Stream ========
	virtual void Write(const void *Data, size_t Size);
	virtual void Flush() { m_CharWriter.Flush(); OverlayStream::Flush(); }

	// ======== Statyczne ========
	/// Po prostu koduje podane dane
	/** \param[out] Out Bufor wyjœciowy Out musi mieæ zaalokowane przynajmniej DataLength * 2 znaków. */
	static void Encode(char *Out, const void *Data, size_t DataLength, bool UpperCase = true);
	/// Po prostu koduje podane dane
	/** \param[out] Out £añcuch wyjœciowy Out nie musi mieæ ¿adnego konkretnego rozmiaru -
	zostaje wyczyszczony i wype³niony od nowa. */
	static void Encode(string *Out, const void *Data, size_t DataLength, bool UpperCase = true);
#ifdef _WIN32
	static void Encode(wchar_t *Out, const void *Data, size_t DataLength, bool UpperCase = true);
	static void Encode(wstring *Out, const void *Data, size_t DataLength, bool UpperCase = true);
#endif
};

/// Dekoduje ci¹g znaków wejœciowych zapisanych szesnastkowo na dane binarne.
/** Akceptuje zarówno ma³e, jak i du¿e litery. */
class HexDecoder : public OverlayStream
{
private:
	CharReader m_CharReader;
	DECODE_TOLERANCE m_Tolerance;

public:
	HexDecoder(Stream *a_Stream, DECODE_TOLERANCE Tolerance = DECODE_TOLERANCE_NONE);

	DECODE_TOLERANCE GetTolerance() { return m_Tolerance; }

	// ======== Implementacja Stream ========
	virtual size_t Read(void *Out, size_t Size);
	virtual bool End() { return m_CharReader.End(); }

	// ======== Statyczne ========
	/// Znajduje d³ugoœæ zakodowanych danych i zwraca true.
	/** Jeœli nie uda³o siê ustaliæ d³ugoœci, zwraca false. */
	static bool DecodeLength(size_t *OutLength, const string &s, DECODE_TOLERANCE Tolerance = DECODE_TOLERANCE_NONE);
	static bool DecodeLength(size_t *OutLength, const char *s, size_t s_Length, DECODE_TOLERANCE Tolerance = DECODE_TOLERANCE_NONE);
	/// Dekoduje dane binarne. Zwraca liczbê zdekodowanych bajtów.
	/** Jeœli nie uda³o siê zdekodowaæ (jakiœ b³¹d), zwraca MAXUINT32 (0xFFFFFFFF).
	\param[out] OutData musi byæ tablic¹ zaalokowan¹ przynajmniej na rozmiar ustalony wywo³aniem DecodeLength
	lub jeœli nie chcesz wywo³ywaæ DecodeLength, na d³ugoœæ równ¹ d³ugoœci danych wejœciowych / 2. */
	static size_t Decode(void *OutData, const string &s, DECODE_TOLERANCE Tolerance = DECODE_TOLERANCE_NONE);
	/// Dekoduje dane binarne. Zwraca liczbê zdekodowanych bajtów.
	static size_t Decode(void *OutData, const char *s, size_t s_Length, DECODE_TOLERANCE Tolerance = DECODE_TOLERANCE_NONE);
#ifdef _WIN32
	static size_t Decode(void *OutData, const wstring &s, DECODE_TOLERANCE Tolerance = DECODE_TOLERANCE_NONE);
	static size_t Decode(void *OutData, const wchar_t *s, size_t s_Length, DECODE_TOLERANCE Tolerance = DECODE_TOLERANCE_NONE);
#endif
};

/// Koduje strumieñ binarnych danych wejœciowych na ci¹g znaków base64.
/**
- Zgodny z RFC 4648 "The Base16, Base32, and Base64 Data Encodings"
- Prawid³owe dane wyjœciowe powstaj¹ dopiero po zakoñczeniu zapisywania, które nastêpuje w destruktorze.
  Jeœli chcesz zakoñczyæ wczeœniej, wywo³aj Base64Encoder::Finish(), ale potem nie mo¿na ju¿ nic dopisywaæ.
*/
class Base64Encoder : public OverlayStream
{
private:
	CharWriter m_CharWriter;
	bool m_Finished; // Czy dane zakoñczone
	uint8 m_Buf[2];
	size_t m_BufIndex;

	void DoFinish();

public:
	Base64Encoder(Stream *a_Stream) : OverlayStream(a_Stream), m_CharWriter(a_Stream), m_Finished(false), m_BufIndex(0) { }
	~Base64Encoder();

	void Finish() { if (!m_Finished) { DoFinish(); m_Finished = true; } }

	// ======== Implementacja Stream ========
	virtual void Write(const void *Data, size_t Size);
	virtual void Flush() { m_CharWriter.Flush(); OverlayStream::Flush(); }

	// ======== Statyczne ========
	/// Oblicza rozmiar bufora na zakodowane dane
	static size_t EncodeLength(size_t DataLength);
	/// Po prostu koduje podane dane
	/** \param[out] Out Bufor wyjœciowy Out musi mieæ zaalokowane przynajmniej tyle bajtów ile wyliczy³o EncodeLength.
	\return Zwraca liczbê znaków zapisanych do Out. */
	static size_t Encode(char *Out, const void *Data, size_t DataLength);
	/// Po prostu koduje podane dane
	/** \param[out] Out £añcuch wyjœciowy Out nie musi mieæ ¿adnego konkretnego rozmiaru -
	zostaje wyczyszczony i wype³niony od nowa. */
	static size_t Encode(string *Out, const void *Data, size_t DataLength);
};

/// Dekoduje ci¹g znaków wejœciowych zapisanych w standardzie base64 na dane binarne.
/*
- Po napotkaniu koñcówki zawieraj¹cej znaki '=' funkcje nie parsuj¹ ju¿ dalej,
  ale to tylko optymalizacja i nie nale¿y na tym polegaæ.
- Sposób dzia³ania strumienia - na raz wczytywane s¹ po 4 znaki i zamieniane na
  maksymalnie 3 bajty.
*/
class Base64Decoder : public OverlayStream
{
private:
	CharReader m_CharReader;
	DECODE_TOLERANCE m_Tolerance;
	// Bajty s¹ w kolejnoœci: [2][1][0]. Jeœli jest mniej, to [1][0][-] albo [0][-][-].
	uint8 m_Buf[3];
	// Liczba zdekodowanych bajtów w buforze
	size_t m_BufLength;
	// True, jeœli sparsowano koñcówkê z '='
	bool m_Finished;

	// Czyta 4 znaki ze strumienia pod³¹czonego (0 albo 4, inaczej b³¹d) i wype³nia m_Buf maksymalnie 3 zdekodowanymi bajtami.
	// Ustawia m_BufLength i jeœli trzeba, to m_Finished.
	bool ReadNextBuf();

	bool GetNextByte(uint8 *OutByte) { if (m_BufLength == 0) { if (!ReadNextBuf()) return false; } *OutByte = m_Buf[--m_BufLength]; return true; }

public:
	Base64Decoder(Stream *a_Stream, DECODE_TOLERANCE Tolerance = DECODE_TOLERANCE_NONE);

	DECODE_TOLERANCE GetTolerance() { return m_Tolerance; }

	// ======== Implementacja Stream ========
	virtual size_t Read(void *Out, size_t Size);
	virtual bool End();

	// ======== Statyczne ========
	/// Znajduje d³ugoœæ zakodowanych danych i zwraca true.
	/** Jeœli nie uda³o siê ustaliæ d³ugoœci, zwraca false. */
	static bool DecodeLength(size_t *OutLength, const string &s, DECODE_TOLERANCE Tolerance = DECODE_TOLERANCE_NONE);
	static bool DecodeLength(size_t *OutLength, const char *s, size_t s_Length, DECODE_TOLERANCE Tolerance = DECODE_TOLERANCE_NONE);
	/// Dekoduje dane binarne. Zwraca liczbê zdekodowanych bajtów.
	/** Jeœli nie uda³o siê zdekodowaæ (jakiœ b³¹d), zwraca MAXUINT32 (0xFFFFFFFF).
	\param[out] OutData musi byæ tablic¹ zaalokowan¹ przynajmniej na rozmiar ustalony wywo³aniem DecodeLength(). */
	static size_t Decode(void *OutData, const string &s, DECODE_TOLERANCE Tolerance = DECODE_TOLERANCE_NONE);
	static size_t Decode(void *OutData, const char *s, size_t s_Length, DECODE_TOLERANCE Tolerance = DECODE_TOLERANCE_NONE);
};

/// Bufor ko³owy
/*
Dzia³a szybko (na tyle na ile szybko mo¿e dzia³aæ strumieñ, ze
swoimi metodami wirtualnymi itd.), ale ma z góry ograniczon¹ pojemnoœæ. W
przypadku przepe³nienia przy zapisie rzuca wyj¹tek.
*/
class RingBuffer : public Stream
{
public:
	RingBuffer(size_t Capacity);
	virtual ~RingBuffer();

	/// Zwraca liczbê bajtów w buforze
	uint64 GetSize() { return m_Size; }
	/// Zwraca pojemnoœæ bufora
	size_t GetCapacity() { return m_Capacity; }
	/// Zwraca true, jeœli bufor jest pusty
	bool IsEmpty() { return GetSize() == 0; }
	/// Zwraca true, jeœli bufor jest pe³ny
	bool IsFull() { return GetSize() == GetCapacity(); }

	// ======== Implementacja Stream ========
	virtual void Write(const void *Data, size_t Size);
	virtual size_t Read(void *Out, size_t MaxLength);
	virtual void MustRead(void *Out, size_t Length);
	virtual bool End();
	virtual size_t Skip(size_t MaxLength);

private:
	size_t m_Capacity;
	size_t m_Size;
	std::vector<char> m_Buf;
	size_t m_BegIndex;
	size_t m_EndIndex;
};


/// Kopiuje dane z jednego strumienia do drugiego - co najwy¿ej MaxLength bajtów.
/** \return Zwraca liczbê przekopiowanych danych. */
inline size_t Copy(Stream *Dst, Stream *Src, size_t MaxLength) { return Dst->CopyFrom(Src, MaxLength); }
/// Kopiuje dane z jednego strumienia do drugiego - dok³adnie Length bajtów.
/** Jeœli siê nie uda przekopiowaæ tylu bajtów, rzuca b³¹d. */
inline void MustCopy(Stream *Dst, Stream *Src, size_t Length) { return Dst->MustCopyFrom(Src, Length); }
/// Kopiuje dane z jednego strumienia do drugiego a¿ do koñca tego Ÿród³owego
inline size_t CopyToEnd(Stream *Dst, Stream *Src) { return Dst->CopyFromToEnd(Src); }

//@}
// code_stream

} // namespace common

/** \addtogroup code_sthtostr */
//@{

template <>
struct SthToStr_obj<common::MD5_SUM>
{
	void operator () (tstring *Str, const common::MD5_SUM &Sth)
	{
		common::MD5ToStr(Str, Sth);
	}
	static inline bool IsSupported() { return true; }
};

template <>
struct StrToSth_obj<common::MD5_SUM>
{
	bool operator () (common::MD5_SUM *Sth, const tstring &Str)
	{
		return common::StrToMD5(Sth, Str);
	}
	static inline bool IsSupported() { return true; }
};

//@}
// code_sthtostr

#endif
