/** \file
\brief Klasy wyj¹tków do obs³ugi b³êdów
\author Adam Sawicki - sawickiap@poczta.onet.pl - http://regedit.gamedev.pl/ \n

Czêœæ biblioteki CommonLib \n
Kodowanie Windows-1250, koniec wiersza CR+LF, test: Za¿ó³æ gêœl¹ jaŸñ \n
Licencja: GNU LGPL. \n
Dokumentacja: \ref Module_Error \n
Elementy modu³u: \ref code_error
*/
#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif
#ifndef COMMON_ERROR_H_
#define COMMON_ERROR_H_

typedef long HRESULT;
#ifdef WIN32
	#include <windows.h>
#endif
#ifdef USE_DIRECTX
	struct ID3DXBuffer;
#endif

namespace common
{

/** \addtogroup code_error Error Module
Dokumentacja: \ref Module_Error \n
Nag³ówek: Error.hpp */
//@{

/// Klasa bazowa b³êdów
class Error
{
private:
	std::vector<tstring> m_Msgs;

protected:
	/// U¿yj tego konstruktora, jeœli musisz najpierw coœ policzyæ, a dopiero potem utworzyæ komunikat o b³êdzie.
	Error() { }

public:
	/// Dodaje komunikat na stos.
	/** \param file u¿yj __TFILE__
	\param line u¿yj __LINE__ */
	void Push(const tstring &msg, const tstring &file = _T(""), int line = 0);
	/// Zwraca pe³ny opis b³êdu w jednym ³añcuchu
	/** \param Indent £añcuch od którego ma siê zaczynaæ ka¿dy komunikat (wciêcie).
	\param EOL £añcuch oddzielaj¹cy komunikaty (koniec wiersza). Nie do³¹czany na koñcu. */
	void GetMessage_(tstring *Msg, const tstring &Indent = _T(""), const tstring &eol = EOL) const;

	/// Utworzenie ogólnego b³êdu
	Error(const tstring &msg, const tstring &file = _T(""), int line = 0)
	{
		Push(msg, file, line);
	}
};

/// Wyj¹tek utworzony z systemowego b³êdu errno
class ErrnoError : public Error
{
public:
	/// Tworzy b³¹d z podanym kodem.
	ErrnoError(int err_code, const tstring &msg, const tstring &file = _T(""), int line = 0);
	/// Tworzy b³¹d z kodem pobieranym ze zmiennej errno.
	ErrnoError(const tstring &msg, const tstring &file = _T(""), int line = 0);
};

#ifdef USE_SDL
	/// Wyj¹tek z b³êdem pobranym z biblioteki SDL za pomoc¹ SDLError
	class SDLError : public Error
	{
	public:
		SDLError(const tstring &msg, const tstring &file = _T(""), int line = 0);
	};
#endif

#ifdef USE_OPENGL
	/// Wyj¹tek z b³êdem pobranym z biblioteki OpenGL za pomoc¹ OpenGLError
	class OpenGLError : public Error
	{
	public:
		OpenGLError(const tstring &msg, const tstring &file = _T(""), int line = 0);
	};
#endif

#ifdef USE_FMOD
	/// Wyj¹tek z b³êdem pobranym z biblioteki dŸwiêkowej FMOD
	class FmodError : public Error
	{
	private:
		/** \param[out] Enum Mo¿e byæ NULL.
		\param[out] Message Mo¿e byæ NULL. */
		void CodeToMessage(FMOD_RESULT code, tstring *Enum, tstring *Message);

	public:
		FmodError(FMOD_RESULT code, const tstring &msg, const tstring &file = _T(""), int line = 0);
	};
#endif

#ifdef WIN32
	/// Tworzy wyj¹tek na podstawie b³êdu WinAPI pobieranego za pomoc¹ GetLastError
	class Win32Error : public Error
	{
	public:
		Win32Error(const tstring &msg = _T(""), const tstring &file = _T(""), int line = 0);
	};
#endif

#ifdef USE_DIRECTX
	/// Tworzy wyj¹tek na podstawie b³êdu biblioteki Direct3D.
	class DirectXError : public Error
	{
	public:
		DirectXError(HRESULT hr, const tstring &Msg = _T(""), const tstring &file = _T(""), int Line = 0);
		/// U¿yj tej wersji jeœli funkcja DX któr¹ kontroluje zwraca te¿ w przypadku b³êdu bufor z komunikatem
		/** Bufor zostanie przez konstruktor tego wyj¹tku sam zwolniony, jeœli istnia³. Mo¿e nie istnieæ. */
		DirectXError(HRESULT hr, ID3DXBuffer *Buf, const tstring &Msg = _T(""), const tstring &file = _T(""), int Line = 0);
	};
#endif

#ifdef USE_WINSOCK
	/// Tworzy wyj¹tek na podstawie b³êdu biblioteki WinSock.
	class WinSockError : public Error
	{
	private:
		void CodeToStr(int Code, tstring *Name, tstring *Message);

	public:
		/// Tworzy komunikat b³êdu WinSockError
		int WinSockError(int Code, const tstring &Msg = "", const tstring &file = _T(""), int Line = 0);
		/// Tworzy komunikat b³êdu WinSockError, pobiera kod sam z WSAGetLastError()
		int WinSockError(const tstring &Msg = "", const tstring &file = _T(""), int Line = 0);
	};
#endif

#ifdef USE_DEVIL
	/// Tworzy wyj¹tek na podstawie b³êdu biblioteki graficznej DevIL
	/* Poniewa¿ nie sposób niestety w DevIL sprawdziæ czy jest b³¹d bez zdjêcia jednego
	kodu b³êdu ze szczytu stosu, nale¿y wywo³aæ ILenum code = ilGetError() i jeœli
	code != IL_NO_ERROR, to wtedy rzuciæ ten wyj¹tek podaj¹c jako pierwszy parametr code.
	*/
	class DevILError : public Error
	{
	public:
		DevILError(unsigned FirstCode, const tstring &Msg = "", const tstring &File = _T(""), int Line = 0);
	};
#endif

#ifdef USE_AVI_FILE
	/// Tworzy wyj¹tek na podstawie b³êdu biblioteki AviFile (czêœæ WinAPI)
	class AVIFileError : public Error
	{
	public:
		AVIFileError(HRESULT hr, const tstring &Msg = "", const tstring &file = _T(""), int a_Line = 0);
	};
#endif

//@}
// code_error

} // namespace common

/** \addtogroup code_error */
//@{

/// U¿yj aby dodaæ komunikat do stosu wyj¹tku
#define ERR_ADD_FUNC(exception) { (exception).Push(__TFUNCSIG__, __TFILE__, __LINE__); }
#define ERR_TRY        { try {
#define ERR_CATCH(msg) } catch(common::Error &e) { e.Push((msg), __TFILE__, __LINE__); throw; } }
#define ERR_CATCH_FUNC } catch(common::Error &e) { ERR_ADD_FUNC(e); throw; } }

#define ERR_GUARD_BOOL(Expr) { if ((Expr) == false) throw common::Error(_T(#Expr), __TFILE__, __LINE__); }
#define ERR_GUARD_DIRECTX(Expr) { HRESULT hr; if (FAILED(hr = (Expr))) throw common::DirectXError(hr, _T(#Expr), __TFILE__, __LINE__); }
#define ERR_GUARD_DEVIL(Code) { { Code ; } ILenum c = ilGetError(); if (c != IL_NO_ERROR) throw common::DevILError(c, _T(#Code), __TFILE__, __LINE__); }

#ifdef _DEBUG
	#define ERR_GUARD_BOOL_D(Expr) ERR_GUARD_BOOL(Expr)
	#define ERR_GUARD_DIRECTX_D(Expr) ERR_GUARD_DIRECTX(Expr)
#else
	#define ERR_GUARD_BOOL_D(Expr) (Expr);
	#define ERR_GUARD_DIRECTX_D(Expr) (Expr);
#endif

/// Makra, które przerabiaj¹ ka¿dy rodzaj wyj¹tku na wyj¹tek common::Error
#define ALL_EXCEPTIONS_TRY   try {
/// Makra, które przerabiaj¹ ka¿dy rodzaj wyj¹tku na wyj¹tek common::Error
#define ALL_EXCEPTIONS_CATCH   \
	} \
	catch (const common::Error&) \
	{ \
		throw; \
	} \
	catch (const std::exception &stde) \
	{ \
		throw common::Error(tstring(_T("std::exception: ")) + common::StringToTstringR(stde.what()), __TFILE__, __LINE__); \
	} \
	catch (...) \
	{ \
		throw common::Error(_T("Unknown exception."), __TFILE__, __LINE__); \
	}

//@}
// code_error

/// Rozszerzenie modu³u bazowego o wsparcie dla b³êdów
/**
\ingroup code_sthtostr
\ingroup code_error */
template <typename T>
inline void MustStrToSth(T *Sth, const tstring &Str)
{
	if (!StrToSth<T>(Sth, Str))
		throw common::Error(_T("String conversion error: ") + Str);
}

#endif
