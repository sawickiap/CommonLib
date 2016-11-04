/** \file
\brief Message logging facility
\author Adam Sawicki - sawickiap@poczta.onet.pl - http://asawicki.info/ \n

Part of CommonLib library. \n
Encoding UTF-8, end of line CR+LF \n
License: GNU LGPL. \n
Documentation: \ref Module_Logger \n
Module components: \ref code_logger
*/
#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif
#ifndef COMMON_LOGGER_H_
#define COMMON_LOGGER_H_

// Dziwaczna deklaracja zapowiadaj¹ca, ¿eby nie w³¹czaæ tu w nag³ówku <iostream>
namespace std
{
	template <typename T> struct char_traits;
	template <typename T1, typename T2> class basic_ostream;
	typedef basic_ostream<char, char_traits<char> > ostream;
};

namespace common
{

/** \addtogroup code_logger Logger Module
Dokumentacja: \ref Module_Logger \n
Nag³ówek: Logger.hpp */
//@{

/// \internal
class ILog;
/// \internal
class Logger_pimpl;

/// Logger - klasa g³ówna systemu loguj¹cego.
class Logger
{
	friend void CreateLogger(bool);
	friend void DestroyLogger();
	friend class ILog;

private:
	scoped_ptr<Logger_pimpl> pimpl;

	Logger(bool UseQueue);
	~Logger();

public:
	/** \name Konfiguracja
	Te metody nie s¹ bezpieczne w¹tkowo. */
	//@{
	/// Rejestruje log w loggerze mapuj¹c do niego okreœlone typy komunikatów.
	void AddLogMapping(uint32 Mask, ILog *Log);
	/// Ustawia mapowanie prefiksów typu dla wszystkich zarejestrowanych logów.
	void AddTypePrefixMapping(uint32 Mask, const tstring &Prefix);
	/// Ustawia format prefiksu dla wszystkich zarejestrowanych logów.
	void SetPrefixFormat(const tstring &PrefixFormat);
	//@}

	/** \name U¿ywanie loggera.
	Te metody s¹ bezpieczne w¹tkowo. */
	//@{
	/// Ustawia w³asn¹ informacjê prefiksu. Indeks: 0..2
	void SetCustomPrefixInfo(int Index, const tstring &Info);
	//// Loguje komunikat - najwa¿niejsza funkcja!
	void Log(uint32 Type, const tstring &Message);
	//@}
};

/// Abstrakcyjna kasa bazowa wszelkich logów.
class ILog
{
	friend class Logger_pimpl;

private:
	/// \internal
	class ILog_pimpl;
	scoped_ptr<ILog_pimpl> pimpl;

protected:
	/// Ma zalogowaæ podany komunikat tam gdzie trzeba
	virtual void OnLog(uint32 Type, const tstring &Prefix, const tstring &TypePrefix, const tstring &Message) = 0;

public:
	ILog();
	virtual ~ILog();

	/** \name Konfiguracja */
	//@{
	/// Dodaje mapowanie prefiksu typu dla tego loga
	void AddTypePrefixMapping(uint32 Mask, const tstring &Prefix);
	/// Ustawia format prefiksu dla tego loga
	void SetPrefixFormat(const tstring &PrefixFormat);
	//@}
};

/// Tworzy logger
void CreateLogger(bool UseQueue);
/// Usuwa logger
void DestroyLogger();
/// Pobiera logger
Logger & GetLogger();
/// Zwraca true, jeœli Logger jest zainicjowany
bool IsLogger();


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Konkretne loggery

/// Tryby, w jakich mo¿e pracowaæ logger plikowy
enum LOG_FILE_MODE
{
	/// Normalny - najszybszy
	FILE_MODE_NORMAL,
	/// Po ka¿dym komunikacie FLUSH
	FILE_MODE_FLUSH,
	/// Dla ka¿dego komunikatu osobne otwarcie pliku - najbardziej niezawodny
	FILE_MODE_REOPEN,
};

/// Log do pliku TXT
class TextFileLog : public ILog
{
private:
	/// \internal
	class TextFileLog_pimpl;
	scoped_ptr<TextFileLog_pimpl> pimpl;

protected:
	virtual void OnLog(uint32 Type, const tstring &Prefix, const tstring &TypePrefix, const tstring &Message);

public:
	TextFileLog(const tstring &FileName, LOG_FILE_MODE Mode, EOLMODE EolMode, bool Append = false, const tstring &StartText = _T(""));
	virtual ~TextFileLog();
};

/// Log do pliku HTML
class HtmlFileLog : public ILog
{
public:
	struct STYLE
	{
		uint32 BackgroundColor;
		uint32 FontColor;
		bool Bold;
		bool Italic;

		STYLE(bool Bold = false, bool Italic = false, uint FontColor = 0x000000, uint BackgroundColor = 0xFFFFFF);
	};

private:
	/// \internal
	class HtmlFileLog_pimpl;
	scoped_ptr<HtmlFileLog_pimpl> pimpl;

protected:
	virtual void OnLog(uint32 Type, const tstring &Prefix, const tstring &TypePrefix, const tstring &Message);

public:
	HtmlFileLog(const tstring &FileName, LOG_FILE_MODE Mode, bool Append = false, const tstring &StartText = _T(""));
	virtual ~HtmlFileLog();

	/** \name Konfiguracja */
	//@{
	/// Dodaje mapowanie typu komunikatu na styl
	void AddStyleMapping(uint32 Mask, const STYLE &Style);
	//@}
};

/// Log zapisuj¹cy do strumienia wyjœciowego biblioteki standardowej C++ - std::ostream.
class OstreamLog : public ILog
{
private:
	class Pimpl;
	scoped_ptr<Pimpl> pimpl;

protected:
	virtual void OnLog(uint32 Type, const tstring &Prefix, const tstring &TypePrefix, const tstring &Message);

public:
#ifdef _UNICODE
	OstreamLog(std::wostream *Ostream);
#else
	OstreamLog(std::ostream *Ostream);
#endif
	virtual ~OstreamLog();
};

//@}
// code_logger

} // namespace common

/** \addtogroup code_logger */
//@{
/// Skrót do ³atwego zalogowania ³añcucha
#define LOG(Type, s) { if (common::IsLogger()) common::GetLogger().Log((Type), (s)); else assert(0 && "LOG macro: Logger not initialized."); }
//@}

#endif
