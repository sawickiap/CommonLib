/** \file
\brief Parser sk³adni jêzyka podobnego do C/C++
\author Adam Sawicki - sawickiap@poczta.onet.pl - http://regedit.gamedev.pl/ \n

Czêœæ biblioteki CommonLib \n
Kodowanie Windows-1250, koniec wiersza CR+LF, test: Za¿ó³æ gêœl¹ jaŸñ \n
Licencja: GNU LGPL. \n
Dokumentacja: \ref Module_Tokenizer \n
Elementy modu³u: \ref code_tokenizer
*/
#include "Base.hpp"
#include <map>
#include "Stream.hpp"
#include "Tokenizer.hpp"


namespace common
{

TokenizerError::TokenizerError(size_t Char, size_t Row, size_t Col, const tstring &Msg, const tstring &File, int a_Line) :
	m_Char(Char),
	m_Row(Row),
	m_Col(Col)
{
	if (Msg.empty())
		Push(Format(_T("Tokenizer(row=#, col=#, char=#)")) % m_Row % m_Col % m_Char, __TFILE__, __LINE__);
	else
		Push(Format(_T("Tokenizer(row=#, col=#, char=#) #")) % m_Row % m_Col % m_Char % Msg, __TFILE__, __LINE__);
}


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Klasa Tokenizer_pimpl

typedef std::map<tstring, uint> KEYWORDS_MAP;

class Tokenizer_pimpl
{
public:
	// Które znaki s¹ symbolami
	bool m_Symbols[256];

	// Flagi
	bool m_FlagTokenEOL;
	bool m_FlagMultilineStrings;

	// Strumieñ do pobierania (jeden z tych trzech strumieni jest != NULL)
	// - Z konstruktora char*, wchar_t*, wstring - strumieñ pamiêciowy, mój w³asny
	scoped_ptr<MemoryStream> m_MemoryStream;
	// - Z konstruktora string - strumieñ ³añcuchowy, mój w³asny
	scoped_ptr<StringStream> m_StringStream;
	// - Z konstruktora Stream* - strumieñ dowolny, zewnêtrzny
	Stream *m_ExternalStream;
	// CharReader do jednego z powy¿szych strumieni
	scoped_ptr<TCharReader> m_CharReader;

	// WARSTWA I - wczytywanie znaków
	size_t m_CurrChar, m_CurrRow, m_CurrCol;
	bool m_L1End; // albo to jest true...
	tchar m_L1Char; // ...albo tu jest ostatnio wczytany znak
	void L1Next();

	// Zarejestrowane dane
	KEYWORDS_MAP m_Keywords;

	// Informacje na temat ostatnio odczytanego tokena
	size_t m_LastChar, m_LastRow, m_LastCol;
	Tokenizer::TOKEN m_LastToken;
	tstring m_LastString; // wype³niany zawsze przy odczytaniu nastêpnego tokena
	uint m_LastId; // wype³niany przy TOKEN_KEYWORD

	~Tokenizer_pimpl() { }

	void InitSymbols();

	tchar ParseStringChar();
	void Parse();

	template <typename T> bool GetUint(T *Out);
	template <typename T> bool GetInt(T *Out);
	template <typename T> T MustGetUint();
	template <typename T> T MustGetInt();
};

void Tokenizer_pimpl::L1Next()
{
	m_L1End = !m_CharReader->ReadChar(&m_L1Char);

	if (!m_L1End)
	{
		// Liczniki
		m_CurrChar++;
		if (m_L1Char == _T('\n'))
		{
			m_CurrRow++;
			m_CurrCol = 0;
		}
		else
			m_CurrCol++;

		// Sekwencja "\\\n" lub "\\\r\n" jest pomijana - ³amanie linii za pomoc¹ znaku \ ...
		if (m_L1Char == _T('\\'))
		{
			tchar Ch;
			if (m_CharReader->PeekChar(&Ch) && Ch == _T('\r'))
			{
				m_CharReader->SkipChar();
				m_CurrChar++;
			}
			if (m_CharReader->PeekChar(&Ch) && Ch == _T('\n'))
			{
				m_CharReader->SkipChar();
				m_CurrChar++;
				m_CurrRow++;
				m_CurrCol = 0;
				L1Next();
			}
		}
	}
}

void Tokenizer_pimpl::InitSymbols()
{
	std::fill(&m_Symbols[0], &m_Symbols[256], false);

	const tchar * DEFAULT_SYMBOLS = _T("`~!@#$%^&*()=[]{};:,.<>?\\|");

#ifdef WIN32
	uint SymbolCount = _tcslen(DEFAULT_SYMBOLS);
#else
	uint SymbolCount = strlen(DEFAULT_SYMBOLS);
#endif
	for (uint i = 0; i < SymbolCount; i++)
		m_Symbols[(uint8)DEFAULT_SYMBOLS[i]] = true;
}

tchar Tokenizer_pimpl::ParseStringChar()
{
	// Musi byæ tu ju¿ sprawdzone, ¿e nie jest koniec oraz ¿e m_L1Char nie jest znakiem koñcz¹cym ³añcuch/znak
	assert(!m_L1End);

	// Escape sequence
	if (m_L1Char == _T('\\'))
	{
		L1Next();
		if (m_L1End)
			throw TokenizerError(m_LastChar, m_LastRow, m_LastCol, _T("Unexpected and of data inside escape sequence."), __TFILE__, __LINE__);
		// Sta³a szesnastkowa
		if (m_L1Char == 'x')
		{
			uint8 Number1, Number2;
			L1Next();

			// Nastêpne dwa znaki

			if (m_L1End)
				throw TokenizerError(m_LastChar, m_LastRow, m_LastCol, _T("Unexpected and of data inside escape sequence."), __TFILE__, __LINE__);
			Number1 = HexDigitToNumber(m_L1Char);
			L1Next();

			if (m_L1End)
				throw TokenizerError(m_LastChar, m_LastRow, m_LastCol, _T("Unexpected and of data inside escape sequence."), __TFILE__, __LINE__);
			Number2 = HexDigitToNumber(m_L1Char);
			L1Next();

			if (Number1 == 0xFF || Number2 == 0xFF)
				throw TokenizerError(m_LastChar, m_LastRow, m_LastCol, _T("Invalid hexadecimal escape sequence."), __TFILE__, __LINE__);

			return (tchar)(Number1 << 4 | Number2);
		}
		// Zwyk³a sta³a
		else
		{
			tchar R;
			switch (m_L1Char)
			{
			case _T('n'): R = _T('\n'); break;
			case _T('t'): R = _T('\t'); break;
			case _T('v'): R = _T('\v'); break;
			case _T('b'): R = _T('\b'); break;
			case _T('r'): R = _T('\r'); break;
			case _T('f'): R = _T('\f'); break;
			case _T('a'): R = _T('\a'); break;
			case _T('0'): R = _T('\0'); break;
			case _T('\\'):
			case _T('?'):
			case _T('"'):
			case _T('\''):
				R = m_L1Char;
				// Zostaje to co jest
				break;
			default:
				throw TokenizerError(m_LastChar, m_LastRow, m_LastCol, _T("Unknown escape sequence: \\") + CharToStrR(m_L1Char), __TFILE__, __LINE__);
			}
			L1Next();
			return R;
		}
	}
	else
	{
		tchar R = m_L1Char;
		L1Next();
		return R;
	}
}

void Tokenizer_pimpl::Parse()
{
	for (;;)
	{
		m_LastChar = m_CurrChar;
		m_LastRow = m_CurrRow;
		m_LastCol = m_CurrCol;

		// EOF
		if (m_L1End)
		{
			m_LastString.clear();
			m_LastToken = Tokenizer::TOKEN_EOF;
			return;
		}

		// Symbol
		if (m_Symbols[(uint8)m_L1Char])
		{
			CharToStr(&m_LastString, m_L1Char);
			m_LastToken = Tokenizer::TOKEN_SYMBOL;
			L1Next();
			return;
		}
		// Bia³y znak
		else if (m_L1Char == _T(' ') || m_L1Char == _T('\t') || m_L1Char == _T('\r') || m_L1Char == _T('\v'))
		{
			L1Next();
			continue;
		}
		// Nag³ówek Unicode
#ifdef _UNICODE
		else if (m_L1Char == L'\xFFFE')
		{
			L1Next();
			continue;
		}
#endif
		// Koniec wiersza
		else if (m_L1Char == _T('\n'))
		{
			// Jako token
			if (m_FlagTokenEOL)
			{
				m_LastString = _T("\n");
				m_LastToken = Tokenizer::TOKEN_EOL;
				L1Next();
				return;
			}
			// Jako bia³y znak
			else
			{
				L1Next();
				continue;
			}
		}
		// Sta³a znakowa
		else if (m_L1Char == _T('\''))
		{
			// Nastêpny znak
			L1Next();
			if (m_L1End)
				throw TokenizerError(m_LastChar, m_LastRow, m_LastCol, _T("Unexpected end of data inside character constant."), __TFILE__, __LINE__);
			// Znak (sekwencja ucieczki lub zwyk³y)
			CharToStr(&m_LastString, ParseStringChar());
			// Nastêpny znak - to musi byæ zakoñczenie '
			if (m_L1End)
				throw TokenizerError(m_LastChar, m_LastRow, m_LastCol, _T("Unexpected end of data inside character constant."), __TFILE__, __LINE__);
			if (m_L1Char != _T('\''))
				throw TokenizerError(m_LastChar, m_LastRow, m_LastCol, _T("End of character constant expected: '"), __TFILE__, __LINE__);
			m_LastToken = Tokenizer::TOKEN_CHAR;
			L1Next();
			return;
		}
		// Sta³a ³añcuchowa
		else if (m_L1Char == _T('"'))
		{
			L1Next();
			m_LastString.clear();
			for (;;)
			{
				// Nastêpny znak
				if (m_L1End)
					throw TokenizerError(m_LastChar, m_LastRow, m_LastCol, _T("Unexpected end of data inside string constant."), __TFILE__, __LINE__);
				// Koniec ³añcucha
				if (m_L1Char == _T('"'))
				{
					L1Next();
					break;
				}
				// Koniec wiersza, podczas gdy nie mo¿e go byæ
				else if ( (m_L1Char == _T('\r') || m_L1Char == _T('\n')) && !m_FlagMultilineStrings )
					throw TokenizerError(m_LastChar, m_LastRow, m_LastCol, _T("Unexpected end of data inside string constant."), __TFILE__, __LINE__);
				// Znak (sekwencja ucieczki lub zwyk³y)
				else
					m_LastString += ParseStringChar();
			}
			m_LastToken = Tokenizer::TOKEN_STRING;
			return;
		}
		// Identyfikator lub s³owo kluczowe
		else if ( (m_L1Char <= _T('Z') && m_L1Char >= _T('A')) || (m_L1Char <= _T('z') && m_L1Char >= _T('a')) || m_L1Char == _T('_') )
		{
			CharToStr(&m_LastString, m_L1Char);
			L1Next();
			// Wczytaj nastêpne znaki
			while ( !m_L1End &&
				( (m_L1Char <= _T('Z') && m_L1Char >= _T('A')) || (m_L1Char <= _T('z') && m_L1Char >= _T('a')) || (m_L1Char <= _T('9') && m_L1Char >= _T('0')) || m_L1Char == _T('_') ) )
			{
				m_LastString += m_L1Char;
				L1Next();
			}
			// ZnajdŸ s³owo kluczowe
			KEYWORDS_MAP::iterator it = m_Keywords.find(m_LastString);
			if (it == m_Keywords.end())
				m_LastToken = Tokenizer::TOKEN_IDENTIFIER;
			else
			{
				m_LastToken = Tokenizer::TOKEN_KEYWORD;
				m_LastId = it->second;
			}
			return;
		}
		// Znak '/' mo¿e rozpoczynaæ komentarz // , /* lub byæ zwyk³ym symbolem
		else if (m_L1Char == _T('/'))
		{
			L1Next();
			if (!m_L1End)
			{
				// Pocz¹tek komentarza //
				if (m_L1Char == _T('/'))
				{
					L1Next();
					// Pomiñ znaki do koñca wiersza lub koñca dokumentu
					while (!m_L1End && m_L1Char != _T('\n'))
						L1Next();
					// Dalsze poszukiwanie tokena
					continue;
				}
				// Pocz¹tek komentarza /*
				else if (m_L1Char == _T('*'))
				{
					L1Next();
					bool WasAsterisk = false;
					// Czytaj kolejne znaki
					for (;;)
					{
						if (m_L1End)
							throw TokenizerError(m_LastChar, m_LastRow, m_LastCol, _T("Unfinished comment /* */"), __TFILE__, __LINE__);
						
						if (m_L1Char == _T('*'))
							WasAsterisk = true;
						else if (m_L1Char == _T('/') && WasAsterisk)
							// Koniec komentarza
							break;
						else
							WasAsterisk = false;

						L1Next();
					}
					// Dalsze poszukiwanie tokena
					L1Next();
					continue;
				}
			}
			// Zwyk³y symbol '/'
			CharToStr(&m_LastString, _T('/'));
			m_LastToken = Tokenizer::TOKEN_SYMBOL;
			L1Next();
			return;
		}
		// Liczba ca³kowita lub zmiennoprzecinkowa
		else if ( (m_L1Char <= _T('9') && m_L1Char >= _T('0')) || m_L1Char == _T('+') || m_L1Char == _T('-') )
		{
			// Pe³na poprawna sk³adnia liczb to mniej wiêcej (wyra¿enie regularne, \o \d \h to cyfra w danym systemie):
			// [+-]?(0[xX]\h+|0\o+|(\d+\.\d*|\d*\.\d+|\d+)([dDeE][+-]?\d+)?)
			// Ale ¿e nie chce mi siê tego a¿ tak dok³adnie parsowaæ, to zaakceptujê tutaj dowolny ci¹g
			// dowolnych cyfr, liter i znaków '.', '+' i '-' - ewentualny b³¹d sk³adni i tak wyjdzie, tylko ¿e póŸniej.
			// Liczba jest na pewno ca³kowita, jeœli zaczyna siê od "0x" lub "0X" (nowoœæ! tu by³ b³¹d i 0xEE rozpoznawa³o jako zmiennoprzecinkowa!)
			// Liczba jest zmiennoprzecinkowa wtedy, kiedy zawiera jeden ze znaków: [.dDeE]
			CharToStr(&m_LastString, m_L1Char);
			L1Next();
			bool IsFloat = false;
			while (!m_L1End && (
				(m_L1Char <= _T('9') && m_L1Char >= _T('0')) ||
				(m_L1Char <= _T('Z') && m_L1Char >= _T('A')) ||
				(m_L1Char <= _T('z') && m_L1Char >= _T('a')) ||
				m_L1Char == _T('+') || m_L1Char == _T('-') || m_L1Char == _T('.') ) )
			{
				if (m_L1Char == _T('.') || m_L1Char == _T('d') || m_L1Char == _T('D') || m_L1Char == _T('e') || m_L1Char == _T('E'))
					IsFloat = true;
				m_LastString += m_L1Char;
				L1Next();
			}
			if (m_LastString.length() > 1 && m_LastString[0] == _T('0') && (m_LastString[1] == _T('x') || m_LastString[1] == _T('X')))
				m_LastToken = Tokenizer::TOKEN_INTEGER;
			else
				m_LastToken = (IsFloat ? Tokenizer::TOKEN_FLOAT : Tokenizer::TOKEN_INTEGER);
			return;
		}
		// Nieznany znak
		else
			throw TokenizerError(m_LastChar, m_LastRow, m_LastCol, Format(_T("Unrecognized character (code=0x#): \'#\'.")) % IntToStrR(TCHAR_TO_INT(m_L1Char), 16) % m_L1Char, __TFILE__, __LINE__);
	}
}

template <typename T>
bool Tokenizer_pimpl::GetUint(T *Out)
{
	if (m_LastString == _T("0"))
	{
		*Out = T();
		return true;
	}

	// Ósemkowo lub szesnastkowo
	if (!m_LastString.empty() && m_LastString[0] == _T('0'))
	{
		// Szesnastkowo
		if (m_LastString.length() > 1 && (m_LastString[1] == _T('x') || m_LastString[1] == _T('X')))
			return ( StrToUint<T>(Out, m_LastString.substr(2), 16) == 0 );
		// Ósemkowo
		else
			return ( StrToUint<T>(Out, m_LastString.substr(1), 8) == 0 );
	}
	// Dziesiêtnie
	else
		return ( StrToUint<T>(Out, m_LastString) == 0 );
}

template <typename T>
bool Tokenizer_pimpl::GetInt(T *Out)
{
	if (m_LastString == _T("0") || m_LastString == _T("-0") || m_LastString == _T("+0"))
	{
		*Out = T();
		return true;
	}

	// Pocz¹tkowy '+' lub '-'
	if (!m_LastString.empty() && (m_LastString[0] == _T('+') || m_LastString[0] == _T('-')))
	{
		// Plus - tak jakby go nie by³o, ale wszystko jest o znak dalej
		if (m_LastString[0] == _T('+'))
		{
			// Ósemkowo lub szesnastkowo
			if (m_LastString.length() > 1 && m_LastString[1] == _T('0'))
			{
				// Szesnastkowo
				if (m_LastString.length() > 2 && (m_LastString[2] == _T('x') || m_LastString[2] == _T('X')))
					return ( StrToInt<T>(Out, m_LastString.substr(3), 16) == 0 );
				// Ósemkowo
				else
					return ( StrToInt<T>(Out, m_LastString.substr(2), 8) == 0 );
			}
			// Dziesiêtnie
			else
				return ( StrToInt<T>(Out, m_LastString.substr(1)) == 0 );
		}
		// Minus - uwzglêdnij go
		else
		{
			// Ósemkowo lub szesnastkowo
			if (m_LastString.length() > 1 && m_LastString[1] == _T('0'))
			{
				// Szesnastkowo
				if (m_LastString.length() > 2 && (m_LastString[2] == _T('x') || m_LastString[2] == _T('X')))
					return ( StrToInt<T>(Out, _T("-") + m_LastString.substr(3), 16) == 0 );
				// Ósemkowo
				else
					return ( StrToInt<T>(Out, _T("-") + m_LastString.substr(2), 8) == 0 );
			}
			// Dziesiêtnie
			else
				return ( StrToInt<T>(Out, _T("-") + m_LastString.substr(1)) == 0 );
		}
	}
	else
	{
		// Ósemkowo lub szesnastkowo
		if (!m_LastString.empty() && m_LastString[0] == _T('0'))
		{
			// Szesnastkowo
			if (m_LastString.length() > 1 && (m_LastString[1] == _T('x') || m_LastString[1] == _T('X')))
				return ( StrToInt<T>(Out, m_LastString.substr(2), 16) == 0 );
			// Ósemkowo
			else
				return ( StrToInt<T>(Out, m_LastString.substr(1), 8) == 0 );
		}
		// Dziesiêtnie
		else
			return ( StrToInt<T>(Out, m_LastString) == 0 );
	}
}

template <typename T>
T Tokenizer_pimpl::MustGetUint()
{
	T R;
	if (!GetUint(&R))
		throw TokenizerError(m_LastChar, m_LastRow, m_LastCol, _T("Cannot convert tstring to unsigned integer number."), __TFILE__, __LINE__);
	return R;
}

template <typename T>
T Tokenizer_pimpl::MustGetInt()
{
	T R;
	if (!GetInt(&R))
		throw TokenizerError(m_LastChar, m_LastRow, m_LastCol, _T("Cannot convert tstring to signed integer number."), __TFILE__, __LINE__);
	return R;
}


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Klasa Tokenizer

Tokenizer::Tokenizer(const tchar *Input, size_t NumChars, uint Flags) :
	pimpl(new Tokenizer_pimpl())
{
	pimpl->InitSymbols();

	pimpl->m_FlagTokenEOL = ((Flags & FLAG_TOKEN_EOL) != 0);
	pimpl->m_FlagMultilineStrings = ((Flags & FLAG_MULTILINE_STRINGS) != 0);

	pimpl->m_MemoryStream.reset(new MemoryStream(NumChars * sizeof(tchar), const_cast<tchar*>(Input)));
	pimpl->m_StringStream.reset();
	pimpl->m_ExternalStream = NULL;
	pimpl->m_CharReader.reset(new TCharReader(pimpl->m_MemoryStream.get()));

	pimpl->m_CurrChar = 0; pimpl->m_CurrRow = 1; pimpl->m_CurrCol = 0;
	pimpl->L1Next();

	pimpl->m_LastChar = 0; pimpl->m_LastRow = 1; pimpl->m_LastCol = 0;
	pimpl->m_LastToken = TOKEN_EOF;
	pimpl->m_LastId = 0;
}

Tokenizer::Tokenizer(const tstring *Input, uint Flags) :
	pimpl(new Tokenizer_pimpl())
{
	pimpl->InitSymbols();

	pimpl->m_FlagTokenEOL = ((Flags & FLAG_TOKEN_EOL) != 0);
	pimpl->m_FlagMultilineStrings = ((Flags & FLAG_MULTILINE_STRINGS) != 0);

	pimpl->m_ExternalStream = NULL;

#ifdef _UNICODE
	pimpl->m_MemoryStream.reset(new MemoryStream(Input->length() * sizeof(tchar), const_cast<tchar*>(Input->data())));
	pimpl->m_StringStream.reset();
	pimpl->m_CharReader.reset(new TCharReader(pimpl->m_MemoryStream.get()));
#else
	pimpl->m_MemoryStream.reset();
	pimpl->m_StringStream.reset(new StringStream(const_cast<tstring*>(Input)));
	pimpl->m_CharReader.reset(new TCharReader(pimpl->m_StringStream.get()));
#endif

	pimpl->m_CurrChar = 0; pimpl->m_CurrRow = 1; pimpl->m_CurrCol = 0;
	pimpl->L1Next();

	pimpl->m_LastChar = 0; pimpl->m_LastRow = 1; pimpl->m_LastCol = 0;
	pimpl->m_LastToken = TOKEN_EOF;
	pimpl->m_LastId = 0;
}

Tokenizer::Tokenizer(Stream *Input, uint Flags) :
	pimpl(new Tokenizer_pimpl())
{
	pimpl->InitSymbols();

	pimpl->m_FlagTokenEOL = ((Flags & FLAG_TOKEN_EOL) != 0);
	pimpl->m_FlagMultilineStrings = ((Flags & FLAG_MULTILINE_STRINGS) != 0);

	pimpl->m_MemoryStream.reset();
	pimpl->m_StringStream.reset();
	pimpl->m_ExternalStream = Input;
	pimpl->m_CharReader.reset(new TCharReader(Input));

	pimpl->m_CurrChar = 0; pimpl->m_CurrRow = 1; pimpl->m_CurrCol = 0;
	pimpl->L1Next();

	pimpl->m_LastChar = 0; pimpl->m_LastRow = 1; pimpl->m_LastCol = 0;
	pimpl->m_LastToken = TOKEN_EOF;
	pimpl->m_LastId = 0;
}

Tokenizer::~Tokenizer()
{
	pimpl->m_CharReader.reset();
	pimpl->m_ExternalStream = NULL;
	pimpl->m_StringStream.reset();
	pimpl->m_MemoryStream.reset();

	pimpl.reset();
}

void Tokenizer::RegisterKeyword(uint Id, const tstring &Keyword)
{
	pimpl->m_Keywords.insert(KEYWORDS_MAP::value_type(Keyword, Id));
}

void Tokenizer::RegisterKeywords(const tchar **Keywords, size_t KeywordCount)
{
	for (uint i = 0; i < KeywordCount; i++)
		RegisterKeyword(i, Keywords[i]);
}

void Tokenizer::Next()
{
	pimpl->Parse();
}

Tokenizer::TOKEN Tokenizer::GetToken()
{
	return pimpl->m_LastToken;
}

size_t Tokenizer::GetCharNum()
{
	return pimpl->m_LastChar;
}

size_t Tokenizer::GetRowNum()
{
	return pimpl->m_LastRow;
}

size_t Tokenizer::GetColNum()
{
	return pimpl->m_LastCol;
}

const tstring & Tokenizer::GetString()
{
	return pimpl->m_LastString;
}

tchar Tokenizer::GetChar()
{
	assert(GetToken() == TOKEN_CHAR || GetToken() == TOKEN_SYMBOL);

	return pimpl->m_LastString[0];
}

uint Tokenizer::GetId()
{
	assert(GetToken() == TOKEN_KEYWORD);

	return pimpl->m_LastId;
}

bool Tokenizer::GetUint1(uint8 *Out) { AssertToken(TOKEN_INTEGER); return pimpl->GetUint<uint8>(Out); }
bool Tokenizer::GetUint2(uint16 *Out) { AssertToken(TOKEN_INTEGER); return pimpl->GetUint<uint16>(Out); }
bool Tokenizer::GetUint4(uint32 *Out) { AssertToken(TOKEN_INTEGER); return pimpl->GetUint<uint32>(Out); }
bool Tokenizer::GetUint8(uint64 *Out) { AssertToken(TOKEN_INTEGER); return pimpl->GetUint<uint64>(Out); }

bool Tokenizer::GetInt1(int8 *Out) { AssertToken(TOKEN_INTEGER); return pimpl->GetInt<int8>(Out); }
bool Tokenizer::GetInt2(int16 *Out) { AssertToken(TOKEN_INTEGER); return pimpl->GetInt<int16>(Out); }
bool Tokenizer::GetInt4(int32 *Out) { AssertToken(TOKEN_INTEGER); return pimpl->GetInt<int32>(Out); }
bool Tokenizer::GetInt8(int64 *Out) { AssertToken(TOKEN_INTEGER); return pimpl->GetInt<int64>(Out); }

uint8 Tokenizer::MustGetUint1() { AssertToken(TOKEN_INTEGER); return pimpl->MustGetUint<uint8>(); }
uint16 Tokenizer::MustGetUint2() { AssertToken(TOKEN_INTEGER); return pimpl->MustGetUint<uint16>(); }
uint32 Tokenizer::MustGetUint4() { AssertToken(TOKEN_INTEGER); return pimpl->MustGetUint<uint32>(); }
uint64 Tokenizer::MustGetUint8() { AssertToken(TOKEN_INTEGER); return pimpl->MustGetUint<uint64>(); }

int8 Tokenizer::MustGetInt1() { AssertToken(TOKEN_INTEGER); return pimpl->MustGetInt<int8>(); }
int16 Tokenizer::MustGetInt2() { AssertToken(TOKEN_INTEGER); return pimpl->MustGetInt<int16>(); }
int32 Tokenizer::MustGetInt4() { AssertToken(TOKEN_INTEGER); return pimpl->MustGetInt<int32>(); }
int64 Tokenizer::MustGetInt8() { AssertToken(TOKEN_INTEGER); return pimpl->MustGetInt<int64>(); }

bool Tokenizer::GetFloat(float *Out)
{
	AssertToken(TOKEN_INTEGER, TOKEN_FLOAT);

	return ( StrToFloat(Out, pimpl->m_LastString) == 0 );
}

bool Tokenizer::GetDouble(double *Out)
{
	AssertToken(TOKEN_INTEGER, TOKEN_FLOAT);

	return ( StrToDouble(Out, pimpl->m_LastString) == 0 );
}

float Tokenizer::MustGetFloat()
{
	float R;
	if (!GetFloat(&R))
		throw TokenizerError(pimpl->m_LastChar, pimpl->m_LastRow, pimpl->m_LastCol, _T("Cannot convert tstring to float number."), __TFILE__, __LINE__);
	return R;
}

double Tokenizer::MustGetDouble()
{
	double R;
	if (!GetDouble(&R))
		throw TokenizerError(pimpl->m_LastChar, pimpl->m_LastRow, pimpl->m_LastCol, _T("Cannot convert tstring to double number."), __TFILE__, __LINE__);
	return R;
}

void Tokenizer::CreateError()
{
	throw TokenizerError(pimpl->m_LastChar, pimpl->m_LastRow, pimpl->m_LastCol, _T("Unknown error"), __TFILE__, __LINE__);
}

void Tokenizer::CreateError(const tstring &Msg)
{
	throw TokenizerError(pimpl->m_LastChar, pimpl->m_LastRow, pimpl->m_LastCol, Msg, __TFILE__, __LINE__);
}

bool Tokenizer::QueryToken(Tokenizer::TOKEN Token)
{
	return (GetToken() == Token);
}

bool Tokenizer::QueryToken(TOKEN Token1, TOKEN Token2)
{
	return (GetToken() == Token1 || GetToken() == Token2);
}

bool Tokenizer::QueryEOF()
{
	return QueryToken(TOKEN_EOF);
}

bool Tokenizer::QueryEOL()
{
	return QueryToken(TOKEN_EOL);
}

bool Tokenizer::QuerySymbol(tchar Symbol)
{
	return (GetToken() == TOKEN_SYMBOL && GetChar() == Symbol);
}

bool Tokenizer::QueryIdentifier(const tstring &Identifier)
{
	return (GetToken() == TOKEN_IDENTIFIER && GetString() == Identifier);
}

bool Tokenizer::QueryKeyword(uint KeywordId)
{
	return (GetToken() == TOKEN_KEYWORD && GetId() == KeywordId);
}

bool Tokenizer::QueryKeyword(const tstring &Keyword)
{
	return (GetToken() == TOKEN_KEYWORD && GetString() == Keyword);
}

void Tokenizer::AssertToken(Tokenizer::TOKEN Token)
{
	if (Token != GetToken())
		CreateError(_T("Expected: ") + tstring(GetTokenName(Token)));
}

void Tokenizer::AssertToken(TOKEN Token1, TOKEN Token2)
{
	if (Token1 != GetToken() && Token2 != GetToken())
		CreateError(_T("Expected: ") + tstring(GetTokenName(Token1)) + _T(" or ") + GetTokenName(Token2));
}

void Tokenizer::AssertEOF()
{
	AssertToken(TOKEN_EOF);
}

void Tokenizer::AssertEOL()
{
	AssertToken(TOKEN_EOL);
}

void Tokenizer::AssertSymbol(tchar Symbol)
{
	if (GetToken() != TOKEN_SYMBOL || GetChar() != Symbol)
		CreateError(_T("Expected symbol: ") + CharToStrR(Symbol));
}

void Tokenizer::AssertIdentifier(const tstring &Identifier)
{
	if (GetToken() != TOKEN_IDENTIFIER || GetString() != Identifier)
		CreateError(_T("Expected identifier: ") + Identifier);
}

void Tokenizer::AssertKeyword(uint KeywordId)
{
	if (GetToken() != TOKEN_KEYWORD || GetId() != KeywordId)
		CreateError(_T("Expected keyword number ") + UintToStrR(KeywordId));
}

void Tokenizer::AssertKeyword(const tstring &Keyword)
{
	if (GetToken() != TOKEN_KEYWORD || GetString() != Keyword)
		CreateError(_T("Expected keyword: ") + Keyword);
}

const tchar * Tokenizer::GetTokenName(TOKEN Token)
{
	switch (Token)
	{
	case TOKEN_EOF:         return _T("End of data");
	case TOKEN_EOL:         return _T("End of line");
	case TOKEN_SYMBOL:      return _T("Symbol");
	case TOKEN_INTEGER:     return _T("Integer number");
	case TOKEN_FLOAT:       return _T("Float number");
	case TOKEN_CHAR:        return _T("Character constant");
	case TOKEN_IDENTIFIER:  return _T("Identifier");
	case TOKEN_KEYWORD:     return _T("Keyword");
	case TOKEN_STRING:      return _T("String");
	default:                return _T("Unknown");
	}
}

//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// class TokenWriter

void TokenWriter::Escape(tstring *Out, const tstring &In, uint EscapeFlags)
{
	Out->clear();
	Out->reserve(In.length());

	bool EscapeEOL   = ( (EscapeFlags & ESCAPE_EOL  ) != 0 );
	bool EscapeOther = ( (EscapeFlags & ESCAPE_OTHER) != 0 );

	tchar Ch;
	for (uint i = 0; i < In.length(); i++)
	{
		Ch = In[i]; 
		if (Ch == _T('\r'))
		{
			if (EscapeEOL)
				*Out += _T("\\r");
			else
				*Out += _T('\r');
		}
		else if (Ch == _T('\n'))
		{
			if (EscapeEOL)
				*Out += _T("\\n");
			else
				*Out += _T('\n');
		}
		else if (Ch == _T('\\') || Ch == _T('\'') || Ch == _T('\"'))
		{
			*Out += '\\';
			*Out += Ch;
		}
		else if (Ch == _T('\0')) *Out += _T("\\0");
		else if (Ch == _T('\v')) *Out += _T("\\v");
		else if (Ch == _T('\b')) *Out += _T("\\b");
		else if (Ch == _T('\f')) *Out += _T("\\f");
		else if (Ch == _T('\a')) *Out += _T("\\a");
		else if (Ch == _T('\t')) *Out += Ch;
		else if ( ((uint8)Ch < 32 || (uint8)Ch > 126) && EscapeOther )
		{
			*Out += _T("\\x");
			tstring s;
			UintToStr2(&s, (uint8)Ch, 2, 16);
			*Out += s;
		}
		else
			*Out += Ch;
	}
}

TokenWriter::TokenWriter( tstring *outStr )
: m_OutString(outStr)
, m_OutStream(NULL)
{
	Ctor();
}

TokenWriter::TokenWriter( common::Stream *outStream )
: m_OutString(NULL)
, m_OutStream(outStream)
{
	Ctor();
}

void TokenWriter::Ctor()
{
	m_Level = 0;
	m_WasEOL = m_SpaceRequired = false;
	m_EOL = EOL;
	m_Indent = _T('\t');
	m_EscapeFlags = ESCAPE_EOL;
}

void TokenWriter::WriteSpace( tchar ch )
{
	WriteRaw(ch);
	m_WasEOL = m_SpaceRequired = false;
}

void TokenWriter::WriteSpace( const tstring &s )
{
	WriteRaw(s);
	m_WasEOL = m_SpaceRequired = false;
}

void TokenWriter::WriteEOL()
{
	WriteRaw(m_EOL);
	m_WasEOL = true;
	m_SpaceRequired = false;
}

void TokenWriter::WriteIdentifier( const tstring &s )
{
	if (m_WasEOL) WriteIndent(m_Level);
	else if (m_SpaceRequired) WriteSpace();

	WriteRaw(s);

	m_SpaceRequired = true;
	m_WasEOL = false;
}

void TokenWriter::WriteSymbol( tchar ch )
{
	int levelDelta = GetSymbolLevelDelta(ch);
	if (levelDelta == -1) LevelDec();

	if (m_WasEOL) WriteIndent(m_Level);
	else if (m_SpaceRequired && IsSymbolSpaceBefore(ch)) WriteSpace();

	WriteRaw(ch);

	if (levelDelta == 1) LevelInc();

	m_SpaceRequired = IsSymbolSpaceAfter(ch);
	m_WasEOL = false;
}

void TokenWriter::WriteSymbol( const tstring &s )
{
	assert(!s.empty());
	if (s.length() == 1)
	{
		WriteSymbol(s[0]);
		return;
	}
	
	// Here only multicharacter

	int levelDelta = GetSymbolLevelDelta(s);
	if (levelDelta == -1) LevelDec();

	if (m_WasEOL) WriteIndent(m_Level);
	else if (m_SpaceRequired && IsSymbolSpaceBefore(s)) WriteSpace();

	WriteRaw(s);

	if (levelDelta == 1) LevelInc();

	m_SpaceRequired = IsSymbolSpaceAfter(s);
	m_WasEOL = false;
}

void TokenWriter::WriteString( const tstring &s )
{
	if (m_WasEOL) WriteIndent(m_Level);
	else if (m_SpaceRequired) WriteSpace();

	// TODO: This method might be written better, without rewriting whole string to another string before WriteRaw.
	tstring escapedStr;
	Escape(&escapedStr, s, m_EscapeFlags);

	static char delim = _T('"');

	WriteRaw(delim);
	WriteRaw(escapedStr);
	WriteRaw(delim);

	m_SpaceRequired = true;
	m_WasEOL = false;
}

void TokenWriter::WriteIndent( uint level )
{
	for (unsigned i = 0; i < level; i++)
		WriteRaw(m_Indent);
}

void TokenWriter::WriteRaw( tchar ch )
{
	if (m_OutString)
		*m_OutString += ch;
	else if (m_OutStream)
		m_OutStream->WriteEx(ch);
}

void TokenWriter::WriteRaw( const tchar *s )
{
	if (m_OutString)
		*m_OutString += s;
	else if (m_OutStream)
		m_OutStream->WriteStringF(s);
}

void TokenWriter::WriteRaw( const tstring &s )
{
	if (m_OutString)
		*m_OutString += s;
	else if (m_OutStream)
		m_OutStream->WriteStringF(s);
}

void TokenWriter::WriteKeyword( const tstring &s)
{
	int levelDelta = GetKeywordLevelDelta(s);
	if (levelDelta == -1)
		LevelDec();

	if (m_WasEOL) WriteIndent(m_Level);
	else if (m_SpaceRequired) WriteSpace();

	WriteRaw(s);

	if (levelDelta == 1)
		LevelInc();

	m_SpaceRequired = true;
	m_WasEOL = false;
}

void TokenWriter::RegisterSymbol( tchar ch, bool spaceBefore, bool spaceAfter, int levelDelta )
{
	if (spaceBefore)
		m_Symbols_Chars_SpaceBefore += ch;
	if (spaceAfter)
		m_Symbols_Chars_SpaceAfter += ch;

	assert(levelDelta >= -1 && levelDelta <= 1);
	if (levelDelta == -1)
		m_Symbols_Chars_LevelDec += ch;
	else if (levelDelta == 1)
		m_Symbols_Chars_LevelInc += ch;
}

void TokenWriter::RegisterSymbol( const tstring &s, bool spaceBefore, bool spaceAfter, int levelDelta )
{
	if (spaceBefore)
		m_Symbols_Strings_SpaceBefore.push_back(s);
	if (spaceAfter)
		m_Symbols_Strings_SpaceAfter.push_back(s);

	assert(levelDelta >= -1 && levelDelta <= 1);
	if (levelDelta == -1)
		m_Symbols_Strings_LevelDec.push_back(s);
	else if (levelDelta == 1)
		m_Symbols_Strings_LevelInc.push_back(s);
}

void TokenWriter::RegisterKeyword( const tstring &s, int levelDelta )
{
	assert(levelDelta >= -1 && levelDelta <= 1);
	if (levelDelta == -1)
		m_Keywords_LevelDec.push_back(s);
	else if (levelDelta == 1)
		m_Keywords_LevelInc.push_back(s);
}

int TokenWriter::GetKeywordLevelDelta( const tstring &s )
{
	STRING_VECTOR::iterator it = std::find(m_Keywords_LevelInc.begin(), m_Keywords_LevelInc.end(), s);
	if (it != m_Keywords_LevelInc.end())
		return 1;
	it = std::find(m_Keywords_LevelDec.begin(), m_Keywords_LevelDec.end(), s);
	if (it != m_Keywords_LevelDec.end())
		return -1;
	return 0;
}

int TokenWriter::GetSymbolLevelDelta( tchar ch )
{
	size_t pos = m_Symbols_Chars_LevelInc.find(ch);
	if (pos != tstring::npos)
		return 1;
	pos = m_Symbols_Chars_LevelDec.find(ch);
	if (pos != tstring::npos)
		return -1;
	return 0;
}

int TokenWriter::GetSymbolLevelDelta( const tstring &s )
{
	assert(!s.empty());
	if (s.length() == 1)
		return GetSymbolLevelDelta(s[0]);

	STRING_VECTOR::iterator it = std::find(m_Symbols_Strings_LevelInc.begin(), m_Symbols_Strings_LevelInc.end(), s);
	if (it != m_Symbols_Strings_LevelInc.end())
		return 1;
	it = std::find(m_Symbols_Strings_LevelDec.begin(), m_Symbols_Strings_LevelDec.end(), s);
	if (it != m_Symbols_Strings_LevelDec.end())
		return -1;
	return 0;
}

bool TokenWriter::IsSymbolSpaceBefore( tchar ch )
{
	return m_Symbols_Chars_SpaceBefore.find(ch) != tstring::npos;
}

bool TokenWriter::IsSymbolSpaceBefore( const tstring &s )
{
	return std::find(m_Symbols_Strings_SpaceBefore.begin(), m_Symbols_Strings_SpaceBefore.end(), s) != m_Symbols_Strings_SpaceBefore.end();
}

bool TokenWriter::IsSymbolSpaceAfter( tchar ch )
{
	return m_Symbols_Chars_SpaceAfter.find(ch) != tstring::npos;
}

void TokenWriter::WriteUint1( uint8 v, bool hex )
{
	tstring s;
	UintToStr(&s, v, hex ? 16 : 10);
	WriteNumberString(hex ? _T("0x") + s : s);
}

void TokenWriter::WriteUint2( uint16 v, bool hex )
{
	tstring s;
	UintToStr(&s, v, hex ? 16 : 10);
	WriteNumberString(hex ? _T("0x") + s : s);
}

void TokenWriter::WriteUint4( uint32 v, bool hex )
{
	tstring s;
	UintToStr(&s, v, hex ? 16 : 10);
	WriteNumberString(hex ? _T("0x") + s : s);
}

void TokenWriter::WriteUint8( uint64 v, bool hex )
{
	tstring s;
	UintToStr(&s, v, hex ? 16 : 10);
	WriteNumberString(hex ? _T("0x") + s : s);
}

void TokenWriter::WriteInt1( int8 v ) { tstring s; IntToStr(&s, v); WriteNumberString(s); }
void TokenWriter::WriteInt2( int16 v ) { tstring s; IntToStr(&s, v); WriteNumberString(s); }
void TokenWriter::WriteInt4( int32 v ) { tstring s; IntToStr(&s, v); WriteNumberString(s); }
void TokenWriter::WriteInt8( int64 v ) { tstring s; IntToStr(&s, v); WriteNumberString(s); }

void TokenWriter::WriteFloat(  float v, char Mode, int Precision  ) { tstring s; FloatToStr (&s, v, Mode, Precision); WriteNumberString(s); }
void TokenWriter::WriteDouble( double v, char Mode, int Precision ) { tstring s; DoubleToStr(&s, v, Mode, Precision); WriteNumberString(s); }

void TokenWriter::WriteNumberString( const tstring &v )
{
	if (m_WasEOL) WriteIndent(m_Level);
	else if (m_SpaceRequired) WriteSpace();

	WriteRaw(v);

	m_SpaceRequired = true;
	m_WasEOL = false;
}

void TokenWriter::WriteComment( const tstring &s, bool alwaysMultiline /*= false*/ )
{
	if (alwaysMultiline || ContainsEOL(s))
		WriteMultiLineComment(s);
	else
		WriteSingleLineComment(s);
}

void TokenWriter::WriteMultiLineComment( const tstring &s )
{
	assert(s.find(_T("*/")) == tstring::npos);

	if (m_WasEOL) WriteIndent(m_Level);
	else if (m_SpaceRequired) WriteSpace();

	WriteRaw(_T("/*"));
	WriteRaw(s);
	WriteRaw(_T("*/"));

	m_SpaceRequired = true;
	m_WasEOL = false;
}

void TokenWriter::WriteSingleLineComment( const tstring &s )
{
	if (m_WasEOL) WriteIndent(m_Level);
	else if (m_SpaceRequired) WriteSpace();

	WriteRaw(_T("//"));
	WriteRaw(s);
	WriteRaw(m_EOL);

	m_SpaceRequired = false;
	m_WasEOL = true;
}

bool TokenWriter::IsSymbolSpaceAfter( const tstring &s )
{
	return std::find(m_Symbols_Strings_SpaceAfter.begin(), m_Symbols_Strings_SpaceAfter.end(), s) != m_Symbols_Strings_SpaceAfter.end();
}


} // namespace common
