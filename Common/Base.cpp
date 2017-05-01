/** \file
\brief Foundation module
\author Adam Sawicki - sawickiap@poczta.onet.pl - http://asawicki.info/ \n

Part of CommonLib library. \n
Encoding UTF-8, end of line CR+LF \n
License: GNU LGPL. \n
Documentation: \ref Module_Base \n
Module components: \ref code_base
*/
#include "Base.hpp"
#include <cstdio> // dla sprintf, sscanf
#include <cctype> // dla tolower, isalnum itp.
#include <ctime> // dla time potrzebnego w RandomGenerator
#include <memory.h>
#ifdef WIN32
	#include <windows.h>
	#include <float.h> // dla _finite i _isnan
    #include <intrin.h> // for _debugbreak
#else
	#include <sys/time.h> // dla gettimeofday
#endif


namespace common
{

namespace Internal
{

#ifdef WIN32

void DebugBreak()
{
    __debugbreak();
}

#endif // #ifdef WIN32

} // namespace Internal

//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Ogólne

void SwapEndian16_Array(void *p, size_t count)
{
	uint16 *u = (uint16*)p;
	for (size_t i = 0; i < count; i++)
		SwapEndian16(&u[i]);
}
void SwapEndian32_Array(void *p, size_t count)
{
	uint32 *u = (uint32*)p;
	for (size_t i = 0; i < count; i++)
		SwapEndian32(&u[i]);
}
void SwapEndian64_Array(void *p, size_t count)
{
	uint64 *u = (uint64*)p;
	for (size_t i = 0; i < count; i++)
		SwapEndian64(&u[i]);
}

void SwapEndian16_Data(void *p, size_t count, ptrdiff_t stepBytes)
{
	char *bytes = (char*)p;
	for (size_t i = 0; i < count; i++, bytes += stepBytes)
		SwapEndian16(bytes);
}
void SwapEndian32_Data(void *p, size_t count, ptrdiff_t stepBytes)
{
	char *bytes = (char*)p;
	for (size_t i = 0; i < count; i++, bytes += stepBytes)
		SwapEndian32(bytes);
}
void SwapEndian64_Data(void *p, size_t count, ptrdiff_t stepBytes)
{
	char *bytes = (char*)p;
	for (size_t i = 0; i < count; i++, bytes += stepBytes)
		SwapEndian64(bytes);
}

void Wait(uint32 Miliseconds)
{
#ifdef WIN32
	Sleep(Miliseconds);
#else
	timeval t;
	t.tv_sec = Miliseconds / 1000;
	t.tv_usec = (Miliseconds % 1000) * 1000;
	select(0, 0, 0, 0, &t);
#endif
}


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// £añcuchy

#ifdef WIN32
	const tchar * const EOL = _T("\r\n");
#else
	const tchar * const EOL = _T("\n");
#endif

const tstring EMPTY_STRING = tstring();

// Liczba obslugiwanych stron kodowych
static const int CHARSET_COUNT = 5;
// Kody znakow poszczegolnych stron kodowych
static const string CHARSET_CHARS[CHARSET_COUNT][18] = {
	{ "\x61", "\x63", "\x65", "\x6C", "\x6E", "\x6F", "\x73", "\x7A", "\x7A",    // brak, male
	  "\x41", "\x43", "\x45", "\x4C", "\x4E", "\x4F", "\x53", "\x5A", "\x5A", }, // brak, duze
	{ "\xB9", "\xE6", "\xEA", "\xB3", "\xF1", "\xF3", "\x9C", "\x9F", "\xBF",    // Windows, male
	  "\xA5", "\xC6", "\xCA", "\xA3", "\xD1", "\xD3", "\x8C", "\x8F", "\xAF", }, // Windows, duze
	{ "\xB1", "\xE6", "\xEA", "\xB3", "\xF1", "\xF3", "\xB6", "\xBC", "\xBF",    // ISO, male
	  "\xA1", "\xC6", "\xCA", "\xA3", "\xD1", "\xD3", "\xA6", "\xAC", "\xAF", }, // ISO, duze
	{ "\xA5", "\x86", "\xA9", "\x88", "\xE4", "\xA2", "\x98", "\xAB", "\xBE",    // IBM, male
	  "\xA4", "\x8F", "\xA8", "\x9D", "\xE3", "\xE0", "\x97", "\x8D", "\xBD", }, // IBM, duze
	{ "\xC4\x85", "\xC4\x87", "\xC4\x99", "\xC5\x82", "\xC5\x84", "\xC3\xB3", "\xC5\x9B", "\xC5\xBA", "\xC5\xBC",    // UTF-8, male
	  "\xC4\x84", "\xC4\x86", "\xC4\x98", "\xC5\x81", "\xC5\x83", "\xC3\x93", "\xC5\x9A", "\xC5\xB9", "\xC5\xBB", }, // UTF-8, duze
};

bool CharIsAlphaNumeric(tchar ch)
{
#ifdef WIN32
	return (IsCharAlphaNumeric(ch) != 0);
#else
	return (isalnum(static_cast<unsigned char>(ch)) != 0);
#endif
}

bool CharIsAlpha(tchar ch)
{
#ifdef WIN32
	return (IsCharAlpha(ch) != 0);
#else
	return (isalpha(static_cast<unsigned char>(ch)) != 0);
#endif
}

bool CharIsLower(tchar ch)
{
#ifdef WIN32
	return (IsCharLower(ch) != 0);
#else
	return (islower(static_cast<unsigned char>(ch)) != 0);
#endif
}

bool CharIsUpper(tchar ch)
{
#ifdef WIN32
	return (IsCharUpper(ch) != 0);
#else
	return (isupper(static_cast<unsigned char>(ch)) != 0);
#endif
}

bool CharIsWhitespace(char ch)
{
	return (isspace(static_cast<unsigned char>(ch)) != 0);
}

#ifdef WIN32
bool CharIsWhitespace(wchar_t ch)
{
	return (iswspace(ch) != 0);
	// Z funkcj¹ IsCharSpace z WinAPI s¹ jakieœ cyrki - patrz MSDN.
}
#endif

size_t StrStrI(const tstring &Str, const tstring &SubStr, size_t Count)
{
	const tchar *StrSz    = Str.c_str();
	const tchar *SubStrSz = SubStr.c_str();
	const tchar *R = StrStrI(StrSz, SubStrSz, Count);
	return (R == NULL) ? MAXUINT32 : (R - StrSz);
}

const tchar * StrStrI(const tchar *Str, const tchar *SubStr, size_t Count)
{
	size_t nLen, nOffset, nMaxOffset, nStringLenInt;
	const tchar *pPos;
	assert(Str && SubStr);
#ifdef WIN32
	nLen = _tcslen(SubStr);
#else
	nLen = strlen(SubStr);
#endif
	if (nLen == 0)
		return Str;
#ifdef WIN32
	size_t nStringToBeSearched = _tcslen(Str);
#else
	size_t nStringToBeSearched = strlen(Str);
#endif
	if (nLen > nStringToBeSearched)
		return NULL;
	if (Count > nStringToBeSearched)
		nStringLenInt = nStringToBeSearched;
	else
		nStringLenInt = Count;
	nMaxOffset = nStringLenInt - nLen;
	pPos = Str;
	for (nOffset = 0; nOffset <= nMaxOffset; nOffset++)
	{
		if (StrCmpI(pPos, SubStr, nLen) == 0)
			return pPos;
		pPos++;
	}
	return NULL;
}

void ReverseString(tstring *s)
{
	size_t i = 0;
	size_t j = s->length()-1;
	while (i < j)
		std::swap( (*s)[i++], (*s)[j--] );
}

void Trim(tstring *s)
{
	if (s->empty()) return;
	// pocz¹tek
	size_t i = 0;
	while (CharIsWhitespace((*s)[i]))
		if (++i == s->size())
		{
			s->clear();
			return;
		}
	s->erase(0, i);
	// koniec
	i = s->size()-1;
	while (CharIsWhitespace((*s)[i]))
		i--;
	s->erase(i+1);
}

void Trim(tstring *Out, const tstring &s)
{
	if (s.empty())
	{
		Out->clear();
		return;
	}
	// pocz¹tek
	size_t start = 0;
	while (CharIsWhitespace(s[start]))
		if (++start == s.size())
		{
			Out->clear();
			return;
		}
	// koniec
	size_t count = s.size()-start;
	while (CharIsWhitespace(s[start+count-1]))
		count--;
	// zwrócenie
	*Out = s.substr(start, count);
}

/* UWAGA!
Funkcje CharUpper() i CharLower() z Win32API powoduja blad ochorony, kiedy
uruchamia sie je w trybie konwersji pojedynczego znaku, a tym znakiem jest
polska litera!
W sumie to chyba daloby sie napisac lepiej, ten blad wynika przeciez z tego,
¿e podaje liczbe ze znakiem (char jest signed, polskie litery sa na minusie).
Gdyby to przekonwertowac... No ale to wymaga dosc zaawansowanego rzutowania,
wiec niech tak sobie juz zostanie :PPP
*/

tchar CharToLower(tchar ch)
{
#ifdef WIN32
	tchar s[2];
	s[0] = ch;
	s[1] = '\0';
	CharLower(s);
	return s[0];
#else
	return static_cast<char>( tolower(static_cast<unsigned char>(ch)) );
#endif
}

tchar CharToUpper(tchar ch)
{
#ifdef WIN32
	tchar s[2];
	s[0] = ch;
	s[1] = '\0';
	CharUpper(s);
	return s[0];
#else
	return static_cast<char>( toupper(static_cast<unsigned char>(ch)) );
#endif
}

void LowerCase(tstring *s)
{
	for (size_t i = 0; i < s->size(); i++)
		(*s)[i] = CharToLower((*s)[i]);
}

void UpperCase(tstring *s)
{
	for (size_t i = 0; i < s->size(); i++)
		(*s)[i] = CharToUpper((*s)[i]);
}

bool Charset_WindowsSpecialChar(char a_c, string *a_s)
{
	switch (a_c)
	{
	case '\x84':
	case '\x94':
	case '\x93':
		if (a_s) *a_s = "\""; return true;
	case '\x85':
		if (a_s) *a_s = "..."; return true;
	case '\x96':
	case '\x97':
		if (a_s) *a_s = "-"; return true;
	case '\x92':
		if (a_s) *a_s = "'"; return true;
	default:
		return false;
	}
}

char Charset_Convert_Char(char a_c, CHARSET a_Charset1, CHARSET a_Charset2)
{
	if (a_Charset1 == a_Charset2) return a_c;
	for (int i = 0; i < 18; ++i)
		if (CHARSET_CHARS[a_Charset1][i][0] == a_c)
			return CHARSET_CHARS[a_Charset2][i][0];
	return a_c;
}

// Funkcja wewnêtrzna, potrzebna tylko dla Charset_Convert.
static bool _StrBeginsA(const string &s, const string &sub, size_t Begin)
{
	if (Begin+sub.size() > s.size())
		return false;
	for (size_t i = 0; i < sub.size(); i++)
		if (s[Begin+i] != sub[i])
			return false;
	return true;
}

void Charset_Convert(string *out, const string &s, CHARSET Charset1, CHARSET Charset2)
{
	if (Charset1 == Charset2) {
		*out = s;
		return;
	}

	size_t i, index = 0; string tmp; bool converted;
	out->clear();
	while (index < s.size()) {
		if (Charset1 == CHARSET_WINDOWS && Charset_WindowsSpecialChar(s[index], &tmp)) {
			*out += tmp;
			index++;
		}
		else {
			converted = false;
			for (i = 0; i < 18; i++) {
				if (_StrBeginsA(s, CHARSET_CHARS[Charset1][i], index)) {
					*out += CHARSET_CHARS[Charset2][i];
					index += CHARSET_CHARS[Charset1][i].size();
					converted = true;
					break;
				}
			}
			if (!converted) {
				*out += s[index];
				index++;
			}
		}
	}
}

void EolModeToStr(tstring *Out, EOLMODE EolMode)
{
	switch (EolMode)
	{
	case EOL_CRLF:
		*Out = _T("\r\n");
		return;
	case EOL_LF:
		*Out = _T("\n");
		return;
	case EOL_CR:
		*Out = _T("\r");
		return;
	default:
		Out->clear();
		return;
	}
}

void Replace(tstring *result, const tstring &s, const tstring &s1, const tstring &s2)
{
	if (s1.empty())
	{
		*result = s;
		return;
	}
	result->clear();
	size_t index1 = 0;
	size_t index2 = 0;
	while ( (index2 = s.find(s1, index1)) != tstring::npos )
	{
		*result += s.substr(index1, index2-index1);
		*result += s2;
		index1 = index2 + s1.size();
	}
	*result += s.substr(index1);
}

void Replace(tstring *Out, const tstring &s, tchar Ch1, tchar Ch2)
{
	size_t Length = s.length();
	Out->resize(Length);
	tchar Ch;
	for (size_t i = 0; i < Length; i++)
	{
		Ch = s[i];
		if (Ch == Ch1)
			(*Out)[i] = Ch2;
		else
			(*Out)[i] = Ch;
	}
}

void Replace(tstring *InOut, tchar Ch1, tchar Ch2)
{
	size_t Length = InOut->length();
	for (size_t i = 0; i < Length; i++)
	{
		if ((*InOut)[i] == Ch1)
			(*InOut)[i] = Ch2;
	}
}

void ReplaceEOL(tstring *result, const tstring &s, EOLMODE EOLMode)
{
	if (EOLMode == EOL_NONE)
	{
		*result = s;
		return;
	}

	result->clear();
	tstring Tmp;
	bool ItWasCR = false;
	for (size_t i = 0; i < s.size(); i++)
	{
		if (s[i] == _T('\r'))
			ItWasCR = true;
		else if (s[i] == _T('\n'))
		{
			EolModeToStr(&Tmp, EOLMode);
			*result += Tmp;
			ItWasCR = false;
		}
		else
		{
			if (ItWasCR)
			{
				EolModeToStr(&Tmp, EOLMode);
				*result += Tmp;
				ItWasCR = false;
			}
			*result += s[i];
		}
	}
	if (ItWasCR)
	{
		EolModeToStr(&Tmp, EOLMode);
		*result += Tmp;
	}
}

void NormalizeWhitespace(tstring *result, const tstring &s)
{
	bool inside = false;
	for (size_t i = 0; i < s.size(); i++)
	{
		if (CharIsWhitespace(s[i]))
			inside = true;
		else
		{
			if (inside)
			{
				*result += _T(' ');
				inside = false;
			}
			*result += s[i];
		}
	}
	if (inside)
		*result += ' ';
}

void DuplicateString(tstring *Out, const tstring &s, size_t count)
{
	Out->clear();
	for (size_t i = 0; i < count; i++)
		*Out += s;
}

void RightStr(tstring *Out, const tstring &s, size_t Length)
{
	Length = std::min(Length, s.length());
	*Out = s.substr(s.length() - Length);
}

size_t SubstringCount(const tstring &str, const tstring &substr)
{
	if (substr.empty())
		return 0;
	size_t result = 0;
	size_t index = 0;
	size_t i;
	for (;;)
	{
		i = str.find(substr, index);
		if (i == tstring::npos)
			break;
		++result;
		index = i + substr.size();
	}
	return result;
}

int StrCmpI(const tstring &s1, const tstring &s2, size_t Count)
{
	return StrCmpI(s1.c_str(), s2.c_str(), Count);
}

int StrCmpI(const tchar *s1, const tchar *s2, size_t Count)
{
#ifdef WIN32
	if (Count == MAXUINT32)
		return _tcsicmp(s1, s2);
	else
		return _tcsnicmp(s1, s2, Count);
#else
	if (Count == MAXUINT32)
		return stricmp(s1, s2);
	else
		return strnicmp(s1, s2, Count);
#endif
}

int SubStrCmp(const tstring &s1, size_t off1, const tstring &s2, size_t off2, size_t length)
{
#ifdef _UNICODE
	if (length == MAXUINT32)
		return wcscmp(s1.c_str()+off1, s1.c_str()+off2);
	else
		return wcsncmp(s1.c_str()+off1, s1.c_str()+off2, length);
#else
	if (length == MAXUINT32)
		return strcmp(s1.c_str()+off1, s1.c_str()+off2);
	else
		return strncmp(s1.c_str()+off1, s2.c_str()+off2, length);
#endif
}

int SubStrCmpI(const tstring &s1, size_t off1, const tstring &s2, size_t off2, size_t length)
{
#ifdef WIN32
	if (length == MAXUINT32)
		return _tcsicmp(s1.c_str()+off1, s2.c_str()+off2);
	else
		return _tcsncicmp(s1.c_str()+off1, s2.c_str()+off2, length);
#else
	size_t i1 = off1, i2 = off2;
	bool s1_end, s2_end;
	char ch1, ch2;
	for (;;)
	{
		if (length == 0) return 0; // Char count limit reached
		s1_end = i1 >= s1.length();
		s2_end = i2 >= s2.length();
		if (s1_end && s2_end) return 0; // Both strings ended
		if (s1_end) return -1; // Only first string ended - first is before
		if (s2_end) return  1; // Only second string ended - first is after

		ch1 = s1[i1];
		ch2 = s2[i2];

		if (ch1 != ch2 && CharToLower(ch1) != CharToLower(ch2))
			return TCHAR_TO_INT(ch1) - TCHAR_TO_INT(ch2);

		i1++; i2++; length--;
	}
#endif
}

bool ContainsEOL(const tstring &s)
{
	for (size_t i = 0; i < s.size(); ++i)
		if (s[i] == _T('\r') || s[i] == _T('\n'))
			return true;
	return false;
}

bool StrBegins(const tstring &s, const tstring &sub, bool caseSensitive, size_t Begin)
{
	if (Begin+sub.size() > s.size())
		return false;
	
	if (caseSensitive)
	{
		for (size_t i = 0; i < sub.size(); i++)
			if (s[Begin+i] != sub[i])
				return false;
	}
	else
	{
		for (size_t i = 0; i < sub.size(); i++)
			if (CharToLower(s[Begin+i]) != CharToLower(sub[i]))
				return false;
	}

	return true;
}

bool StrEnds(const tstring &s, const tstring &Sub, bool caseSensitive)
{
	if (Sub.length() > s.length())
		return false;
	size_t si = s.length() - Sub.length();
	size_t subi = 0;

	if (caseSensitive)
	{
		for ( ; si < s.length(); si++, subi++)
		{
			if (s[si] != Sub[subi])
				return false;
		}
	}
	else
	{
		for ( ; si < s.length(); si++, subi++)
		{
			if (CharToLower(s[si]) != CharToLower(Sub[subi]))
				return false;
		}
	}

	return true;
}

bool Split(const tstring &s, const tstring &delimiter, tstring *out, size_t *index)
{
	if (delimiter.empty())
		return false;
	if (*index >= s.size())
		return false;

	size_t pos = s.find(delimiter, *index);
	if (pos == tstring::npos)
	{
		*out = s.substr(*index);
		*index = s.size();
		return true;
	}

	*out = s.substr(*index, pos-*index);
	*index = pos + delimiter.size();
	return true;
}

bool SplitFirstOf(const tstring &s, const tstring &delimiters, tstring *out, size_t *index)
{
	if (delimiters.empty())
		return false;
	if (*index >= s.size())
		return false;

	size_t pos = s.find_first_of(delimiters, *index);
	if (pos == tstring::npos)
	{
		*out = s.substr(*index);
		*index = s.size();
		return true;
	}

	*out = s.substr(*index, pos-*index);
	*index = pos + 1;
	return true;
}

void Split(STRING_VECTOR *Out, const tstring &s, const tstring &Delimiter)
{
	size_t Index = 0;
	tstring Tmp;
	Out->clear();
	while (Split(s, Delimiter, &Tmp, &Index))
		Out->push_back(Tmp);
}

bool SplitEOL(const tstring &s, tstring *out, size_t *index)
{
	if (*index >= s.size())
		return false;

	size_t posr = s.find('\r', *index);
	size_t posn = s.find('\n', *index);
	if (posr == tstring::npos && posn == tstring::npos)
	{
		*out = s.substr(*index);
		*index = s.size();
		return true;
	}

	if (posn == tstring::npos || posr < posn)
	{
		*out = s.substr(*index, posr-*index);
		if (*index < s.size()-1 && s[posr+1] == _T('\n'))
			*index = posr + 2;
		else
			*index = posr + 1;
		return true;
	}

	*out = s.substr(*index, posn-*index);
	*index = posn + 1;
	return true;
}

bool SplitQuery(const tstring &s, tstring *out, size_t *index)
{
	if (*index >= s.size())
		return false;

	size_t i;
	size_t l = s.size();

	while (CharIsWhitespace(s[*index]))
		if (++(*index) >= l)
			return false;

    if (s[*index] == _T('"'))
	{
		(*index)++;
		i = s.find(_T('"'), *index);
	}
	else
	{
		i = tstring::npos;
		for (size_t j = *index; j < s.size(); j++)
			if (CharIsWhitespace(s[j]))
			{
				i = j;
				break;
			}
	}

	if (i == tstring::npos)
	{
		*out = s.substr(*index);
		*index = l+1;
	}
	else
	{
		*out = s.substr(*index, i-*index);
		*index = i+1;
	}
	return true;
}

bool ValidateWildcard_Asterisk(const tstring &Mask, const tstring &S, size_t *Mask_i, size_t *S_i, bool CaseSensitive)
{
	// Warning: uses multiple returns.
	bool Fit = true;

	// Erase the leading asterisk
	(*Mask_i)++;
	while (*S_i < S.length() && (Mask[*Mask_i] == _T('?') || Mask[*Mask_i] == _T('*')))
	{
		if (Mask[*Mask_i] == _T('?'))
			(*S_i)++;
		(*Mask_i)++;
	}
	// Now it could be that test is empty and wildcard contains aterisks. Then we delete them to get a proper state
	while (Mask[*Mask_i] == _T('*'))
		(*Mask_i)++;
	if (*S_i == S.length() && *Mask_i < Mask.length())
		return !Fit;
	if (*S_i == S.length() && *Mask_i < Mask.length())
		return Fit;
	// Neither test nor wildcard are empty! the first character of wildcard isn't in [*?]
	if (!ValidateWildcard(Mask, S, CaseSensitive, *Mask_i, *S_i))
	{
		do
		{
			(*S_i)++;
			// skip as much characters as possible in the teststring; stop if a character match occurs
			if (CaseSensitive)
			{
				while (*S_i < S.length() && Mask[*Mask_i] != S[*S_i])
					(*S_i)++;
			}
			else
			{
				while (*S_i < S.length() && CharToLower(Mask[*Mask_i]) != CharToLower(S[*S_i]))
					(*S_i)++;
			}
		}
		while ( (*S_i < S.length()) ? !ValidateWildcard(Mask, S, CaseSensitive, *Mask_i, *S_i) : ((Fit = 0) == true) );
	}
	if (*S_i == S.length() && *Mask_i == Mask.length())
		Fit = true;
	return Fit;
}

bool ValidateWildcard(const tstring &Mask, const tstring &S, bool CaseSensitive, size_t MaskOff, size_t SOff)
{
	// Algoryt na podstawie:
	// Wildcards Pattern Matching Algorithm, Florian Schintke
	// http://user.cs.tu-berlin.de/~schintke/references/wildcards/

	bool Fit = true;

	for ( ; MaskOff < Mask.length() && Fit && SOff < S.length(); MaskOff++)
	{
		switch (Mask[MaskOff])
		{
		case _T('?'):
			SOff++;
			break;
		case _T('*'):
			Fit = ValidateWildcard_Asterisk(Mask, S, &MaskOff, &SOff, CaseSensitive);
			MaskOff--;
			break;
		default:
			if (CaseSensitive)
				Fit = (S[SOff] == Mask[MaskOff]);
			else
				Fit = (CharToLower(S[SOff]) == CharToLower(Mask[MaskOff]));
			SOff++;
		}
	}

	while (Mask[MaskOff] == _T('*') && Fit)
		MaskOff++;

	return (Fit && SOff == S.length() && MaskOff == Mask.length());
}

// Funkcja do u¿ytku wewnêtrznego dla FineSearch
// Zwraca liczbê zale¿n¹ od okolicznoœci, w jakich wystêpuje znaleziony string: 1.0 .. 4.0
// Mno¿ona jest przez ni¹ obliczana trafnoœæ.
static float _MnoznikOkolicznosci(const tstring &str, size_t start, size_t length)
{
	bool l = (start == 0);
	bool r = (start+length == str.size());
	// Ca³e pole
	if (l && r)
		return 4.f;
	// Pocz¹tek pola
	else if (l)
		return 3.f;
	else
	{
//			if (!l)
			if (CharIsAlphaNumeric(str[start-1]))
				l = true;
		if (!r)
			if (CharIsAlphaNumeric(str[start+length]))
				r = true;
		// Ca³y ci¹g
		if (l && r)
			return 2.f;
		// Nic ciekawego
		else
			return 1.f;
	}
}

float FineSearch(const tstring &SubStr, const tstring &Str)
{
	tstring usubstr; LowerCase(&usubstr, SubStr);
	tstring ustr; LowerCase(&ustr, Str);
	float result = 0.f;

	size_t i = 0;
	size_t p; float x;

	while (true)
	{
		p = ustr.find(usubstr, i);
		// Koniec szukania
		if (p == tstring::npos)
			break;
		x = std::max( usubstr.size()/5.f - Str.size()/100.f + 1.f, 1.f);
		// Jeœli zgadza siê wielkoœæ liter
		if (SubStrCmp(Str, p, SubStr, 0, SubStr.size()) == 0)
			x *= 1.5;
		// Mno¿nik okolicznoœci - jeœli to ca³e pole, pocz¹tek pola lub ca³y ci¹g
		x *= _MnoznikOkolicznosci(Str, p+Str.size()-Str.size(), SubStr.size());
		// Dodajemy wyliczon¹ trafnoœæ
		result += x;
		i = p + SubStr.length();
	}

	return result;
}

size_t LevenshteinDistance(const tstring &s1, const tstring &s2)
{
	size_t L1 = s1.length();
	size_t L2 = s2.length();
	size_t i, j, cost;
	tchar c1, c2;

	if (L1 == 0) return L2;
	if (L2 == 0) return L1;

	size_t *D_PrevRow = new size_t[L2+1];
	size_t *D_CurrRow = new size_t[L2+1];

	for (j = 0; j <= L2; j++)
		D_PrevRow[j] = j;

	for (i = 1; i <= L1; i++)
	{
		D_CurrRow[0] = i;
		c1 = s1[i-1];
		for (j = 1; j <= L2; j++)
		{
			c2 = s2[j-1];
			cost = (c1 == c2 ? 0 : 1);
			D_CurrRow[j] = min3(D_PrevRow[j] + 1, D_CurrRow[j-1] + 1, D_PrevRow[j-1] + cost);
		}
		std::swap(D_PrevRow, D_CurrRow);
	}

	size_t R = D_PrevRow[L2]; // Prev, bo zosta³o na koniec ostatniego przebiegu pêtli zamienione z Curr
	delete [] D_CurrRow;
	delete [] D_PrevRow;
	return R;
}

size_t LevenshteinDistanceI(const tstring &s1, const tstring &s2)
{
	size_t L1 = s1.length();
	size_t L2 = s2.length();
	size_t i, j, cost;
	tchar c1, c2;

	if (L1 == 0) return L2;
	if (L2 == 0) return L1;

	size_t *D_PrevRow = new size_t[L2+1];
	size_t *D_CurrRow = new size_t[L2+1];

	for (j = 0; j <= L2; j++)
		D_PrevRow[j] = j;

	for (i = 1; i <= L1; i++)
	{
		D_CurrRow[0] = i;
		c1 = CharToLower(s1[i-1]);
		for (j = 1; j <= L2; j++)
		{
			c2 = CharToLower(s2[j-1]);
			cost = (c1 == c2 ? 0 : 1);
			D_CurrRow[j] = min3(D_PrevRow[j] + 1, D_CurrRow[j-1] + 1, D_PrevRow[j-1] + cost);
		}
		std::swap(D_PrevRow, D_CurrRow);
	}

	size_t R = D_PrevRow[L2]; // Prev, bo zosta³o na koniec ostatniego przebiegu pêtli zamienione z Curr
	delete [] D_CurrRow;
	delete [] D_PrevRow;
	return R;
}


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// C-style strings

const tchar * strnistr(const tchar *str, const tchar *substr, size_t count)
{
	size_t nLen, nOffset, nMaxOffset, nStringLenInt;
	const tchar *pPos;
	nLen = common_strlen(substr);
	if (nLen == 0)
		return str;
	size_t nStringToBeSearched = common_strlen(str);
	if (nLen > nStringToBeSearched)
		return NULL;
	if (count > nStringToBeSearched)
		nStringLenInt = nStringToBeSearched;
	else
		nStringLenInt = count;
	nMaxOffset = nStringLenInt - nLen;
	pPos = str;
	for ( nOffset = 0; nOffset <= nMaxOffset; nOffset++ )
	{
		if ( common_strnicmp(pPos, substr, nLen) == 0 )
			return pPos;
		pPos++;
	}
	return NULL;
}

bool StrBegins(const tchar *str, const tchar *substr, bool caseSensitive)
{
	size_t strLen = common_strlen(str);
	size_t substrLen = common_strlen(substr);
	if (substrLen > strLen) return false;

	if (caseSensitive)
		return common_strncmp(str, substr, substrLen) == 0;
	else
		return common_strnicmp(str, substr, substrLen) == 0;
}

bool StrEnds(const tchar *str, const tchar *substr, bool caseSensitive)
{
	size_t strLen = common_strlen(str);
	size_t substrLen = common_strlen(substr);
	if (substrLen > strLen) return false;

	if (caseSensitive)
		return common_strncmp(str + (strLen - substrLen), substr, substrLen) == 0;
	else
		return common_strnicmp(str + (strLen - substrLen), substr, substrLen) == 0;
}

void StrErase(tchar *str, size_t off, size_t count)
{
	if (count == 0) return;
	size_t strLen = common_strlen(str);
	if (off >= strLen) return;
	if (strLen == 0) return;
	// obetnij
	if (count >= strLen - off)
		str[off] = _T('\0');
	// przesuñ dalsze znaki
	else
	{
		size_t c = strLen - off - count;
		size_t dstIndex = off;
		size_t srcIndex = off + count;
		while (c > 0 && dstIndex < strLen)
		{
			str[dstIndex] = str[srcIndex];
			srcIndex++; dstIndex++; c--;
		}
		str[dstIndex] =  _T('\0');
	}
}

void StrInsert(tchar *str, size_t strCapacity, const tchar *newStr, size_t off, size_t count)
{
	if (count == 0) return;
	size_t strLen = common_strlen(str);
	assert(off <= strLen);
	assert(strLen + count < strCapacity);
	// Zrób miejsce
	size_t c = strLen - off;
	size_t srcIndex = off + c - 1;
	size_t dstIndex = srcIndex + count;
	str[dstIndex + 1] =  _T('\0');
	while (c > 0)
	{
		str[dstIndex] = str[srcIndex];
		srcIndex--; dstIndex--; c--;
	}
	// Wpisz nowy ³añcuch
	srcIndex = 0; dstIndex = off;
	while (count > 0)
	{
		str[dstIndex] = newStr[srcIndex];
		srcIndex++; dstIndex++; count--;
	}
}

void StrInsert(tchar *str, size_t strCapacity, const tchar *newStr, size_t off)
{
	StrInsert(str, strCapacity, newStr, off, common_strlen(newStr));
}

void Trim(tchar *s)
{
	size_t sLen = common_strlen(s);

	size_t begPos = 0;
	while (begPos < sLen && CharIsWhitespace(s[begPos]))
		begPos++;
	
	// Ca³y string to bia³e znaki
	if (begPos == sLen)
	{
		ClearStr(s);
		return;
	}

	size_t endPos = sLen;
	while (CharIsWhitespace(s[endPos-1]))
		endPos--;

	// Obetnij koniec, a potem pocz¹tek
	StrErase(s, endPos);
	StrErase(s, 0, begPos);
}

unsigned StrReplace(tchar *str, tchar from, tchar to)
{
	unsigned R = 0;
	while (*str != _T('\0'))
	{
		if (*str == from)
		{
			*str = to;
			R++;
		}
		str++;
	}
	return R;
}


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Obs³uga Unicode (tylko Windows)
#ifdef WIN32

bool ConvertUnicodeToChars(string *Out, const wstring &S, unsigned CodePage)
{
	if (S.empty())
	{
		Out->clear(); return true;
	}

	// Phase 1 - Get buffer size.
	int Size = WideCharToMultiByte(CodePage, 0, S.data(), (int)S.length(), NULL, 0, NULL, NULL);
	if (Size == 0)
	{
		Out->clear(); return false;
	}

	// Phase 2 - Do conversion.
	scoped_ptr<char, DeleteArrayPolicy> Buf(new char[Size]);
	int R = WideCharToMultiByte(CodePage, 0, S.data(), (int)S.length(), Buf.get(), Size, NULL, NULL);
	if (R == 0)
	{
		Out->clear(); return false;
	}
	
	Out->assign(Buf.get(), Size);
	return true;
}

bool ConvertUnicodeToChars(string *Out, const wchar_t *S, size_t NumChars, unsigned CodePage)
{
	if (NumChars == 0)
	{
		Out->clear(); return true;
	}

    assert(NumChars <= (size_t)INT_MAX);

	// Phase 1 - Get buffer size.
	int Size = WideCharToMultiByte(CodePage, 0, S, (int)NumChars, NULL, 0, NULL, NULL);
	if (Size == 0)
	{
		Out->clear(); return false;
	}

	// Phase 2 - Do conversion.
	scoped_ptr<char, DeleteArrayPolicy> Buf(new char[Size]);
	int R = WideCharToMultiByte(CodePage, 0, S, (int)NumChars, Buf.get(), Size, NULL, NULL);
	if (R == 0)
	{
		Out->clear(); return false;
	}
	
	Out->assign(Buf.get(), Size);
	return true;
}

bool ConvertCharsToUnicode(wstring *Out, const string &S, unsigned CodePage)
{
	if (S.empty())
	{
		Out->clear(); return true;
	}

	// Phase 1 - Get buffer size.
	int Size = MultiByteToWideChar(CodePage, 0, S.data(), (int)S.length(), NULL, 0);
	if (Size == 0)
	{
		Out->clear(); return false;
	}

	// Phase 2 - Do conversion.
	scoped_ptr<wchar_t, DeleteArrayPolicy> Buf(new wchar_t[Size]);
	int R = MultiByteToWideChar(CodePage, 0, S.data(), (int)S.length(), Buf.get(), Size);
	if (R == 0)
	{
		Out->clear(); return false;
	}

	Out->assign(Buf.get(), Size);
	return true;
}

bool ConvertCharsToUnicode(wstring *Out, const char *S, size_t NumChars, unsigned CodePage)
{
	if (NumChars == 0)
	{
		Out->clear(); return true;
	}

    assert(NumChars <= (size_t)INT_MAX);

	// Phase 1 - Get buffer size.
	int Size = MultiByteToWideChar(CodePage, 0, S, (int)NumChars, NULL, 0);
	if (Size == 0)
	{
		Out->clear(); return false;
	}

	// Phase 2 - Do conversion.
	scoped_ptr<wchar_t, DeleteArrayPolicy> Buf(new wchar_t[Size]);
	int R = MultiByteToWideChar(CodePage, 0, S, (int)NumChars, Buf.get(), Size);
	if (R == 0)
	{
		Out->clear(); return false;
	}

	Out->assign(Buf.get(), Size);
	return true;
}

bool UnicodeToChars_CalcBufLen(size_t &outLen, const wchar_t *wcs, unsigned CodePage)
{
	return UnicodeToChars_CalcBufLen(outLen, wcs, wcslen(wcs), CodePage);
}

bool UnicodeToChars_CalcBufLen(size_t &outLen, const wchar_t *wcs, size_t wcsLen, unsigned CodePage)
{
	if (wcsLen == 0 || wcs == NULL)
	{
		outLen = 0;
		return true;
	}

	int len = WideCharToMultiByte(CodePage, 0, wcs, (int)wcsLen, NULL, 0, NULL, NULL);
	outLen = (size_t)len;
	return len > 0;
}

bool CharsToUnicode_CalcBufLen(size_t &outLen, const char *str, unsigned CodePage)
{
	return CharsToUnicode_CalcBufLen(outLen, str, strlen(str), CodePage);
}

bool CharsToUnicode_CalcBufLen(size_t &outLen, const char *str, size_t strLen, unsigned CodePage)
{
	if (strLen == 0 || str == NULL)
	{
		outLen = 0;
		return false;
	}
	int len = MultiByteToWideChar(CodePage, 0, str, (int)strLen, NULL, 0);
	outLen = (size_t)len;
	return len > 0;
}

bool ConvertUnicodeToChars(char *outBuf, size_t bufLen, const wchar_t *wcs, unsigned CodePage)
{
	return ConvertUnicodeToChars(outBuf, bufLen, wcs, wcslen(wcs), CodePage);
}

bool ConvertUnicodeToChars(char *outBuf, size_t bufLen, const wchar_t *wcs, size_t wcsLen, unsigned CodePage)
{
	assert(outBuf != NULL && bufLen > 0);

	if (wcsLen == 0 || StrIsEmpty(wcs))
	{
		ClearStr(outBuf);
		return true;
	}

	int R = WideCharToMultiByte(CodePage, 0, wcs, (int)wcsLen, outBuf, (int)bufLen, NULL, NULL);
	return R > 0;
}

bool ConvertCharsToUnicode(wchar_t *outBuf, size_t bufLen, const char *str, unsigned CodePage)
{
	return ConvertCharsToUnicode(outBuf, bufLen, str, strlen(str), CodePage);
}

bool ConvertCharsToUnicode(wchar_t *outBuf, size_t bufLen, const char *str, size_t strLen, unsigned CodePage)
{
	assert(outBuf != NULL && bufLen > 0);
	
	if (strLen == 0 || StrIsEmpty(str))
	{
		ClearStr(outBuf);
		return true;
	}

	int R = MultiByteToWideChar(CodePage, 0, str, (int)strLen, outBuf, (int)bufLen);
	return R > 0;
}


const char * const BOM_UTF8     = "\xEF\xBB\xBF";
const char * const BOM_UTF16_LE = "\xFF\xFE";
const char * const BOM_UTF16_BE = "\xFE\xFF";

#endif

#ifdef _UNICODE
	bool TstringToString(string *Out, const tstring &S) { return ConvertUnicodeToChars(Out, S, CP_ACP); }
	bool StringToTstring(tstring *Out, const string &S) { return ConvertCharsToUnicode(Out, S, CP_ACP); }
#else
	#ifdef WIN32
		bool TstringToWstring(wstring *Out, const tstring &S) { return ConvertCharsToUnicode(Out, S, CP_ACP); }
		bool WstringToTstring(tstring *Out, const wstring &S) { return ConvertUnicodeToChars(Out, S, CP_ACP); }
	#endif
#endif

/*
Sortowanie naturalne
Napisane od pocz¹tku, ale na podstawie kodu:
  strnatcmp.c -- Perform 'natural order' comparisons of strings in C.
  Copyright (C) 2000, 2004 by Martin Pool <mbp sourcefrog net>
  http://sourcefrog.net/projects/natsort/
Mój algorytm dzia³a jednak trochê inaczej i daje inne wyniki (przyda³oby siê
kiedyœ go poprawiæ).
*/

int StringNaturalCompare::CompareNumbers(const tstring &s1, size_t i1, const tstring &s2, size_t i2)
{
	// pomiñ pocz¹tkowe zera
	while (i1 < s1.length() && s1[i1] == _T('0'))
		i1++;
	while (i2 < s2.length() && s2[i2] == _T('0'))
		i2++;

	// The longest run of digits wins.  That aside, the greatest
	// value wins, but we can't know that it will until we've scanned
	// both numbers to know that they have the same magnitude, so we
	// remember it in BIAS.
	int bias = 0;

	for ( ; ; i1++, i2++)
	{
		// koniec lancucha
		if (i1 == s1.length() || !CharIsDigit(s1[i1]))
		{
			if (i2 == s2.length() || !CharIsDigit(s2[i2]))
				return bias;
			else
				return -1;
		}
		else if (i2 == s2.length() || !CharIsDigit(s2[i2]))
			return +1;

		// porównanie znaków
		if (s1[i1] < s2[i2])
		{
			if (bias == 0)
				bias = -1;
		}
		else if (s1[i1] > s2[i2])
		{
			if (bias == 0)
				bias = +1;
		}
	}
	return 0;
}

int StringNaturalCompare::Compare(const tstring &s1, const tstring &s2)
{
	size_t i1 = 0, i2 = 0;
	tchar c1, c2;
	int result;

	for (;;)
	{
		// koniec ³añcucha
		if (i1 == s1.length())
		{
			if (i2 == s2.length())
				return 0;
			else
				return -1;
		}
		else if (i2 == s2.length())
			return +1;

		// pobierz znaki
		c1 = s1[i1];
		c2 = s2[i2];

		// pomiñ bia³e znaki
		if (CharIsWhitespace(c1))
		{
			i1++;
			continue;
		}
		if (CharIsWhitespace(c2))
		{
			i2++;
			continue;
		}

		// obydwa to cyfry - porównujemy liczby
		if (CharIsDigit(c1) && CharIsDigit(c2))
		{
			if ((result = CompareNumbers(s1, i1, s2, i2)) != 0)
				return result;
		}

		// nieuwzglêdnianie wielkoœci liter
		if (m_CaseInsensitive)
		{
			c1 = CharToLower(c1);
			c2 = CharToLower(c2);
		}

		// porównanie znaków
		if (c1 < c2)
			return -1;
		if (c1 > c2)
			return +1;

		// nastêpne znaki
		i1++;
		i2++;
	}
}


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Œcie¿ki do plików

// [Wewnêtrzna]
// Zwraca true, jeœli znak jest jakimœ separatorem katalogów
inline bool IsPathSlash(tchar Ch)
{
	return (Ch == _T('\\')) || (Ch == _T('/'));
}

// [Wewnêtrzna]
// Porównuje dwie nazwy plików lub katalogów
// Z rozró¿nianiem lub bez rozró¿niania wielkoœci znaków, zale¿nie od systemu
bool PathCmp(const tstring &s1, const tstring &s2)
{
#ifdef WIN32
	return StrCmpI(s1, s2) == 0;
#else
	return (s1 == s2);
#endif
}

// [Wewnêtrzna]
// Rozk³ada œcie¿kê na czêœci
void DecomposePath(const tstring &s, tstring *OutPrefix, tstring *OutPath, tstring *OutTrailingPathDelimiter)
{
	OutPrefix->clear();
	OutPath->clear();
	OutTrailingPathDelimiter->clear();
	size_t Beg = 0, End = s.length();

	// Prefix
	if (!s.empty())
	{
		// Zaczyna siê od <slash>
		if (IsPathSlash(s[0]))
		{
			// <slash> <slash> <serwer> <slash> <udzia³> <slash>
			if (s.length() > 1 && IsPathSlash(s[1]))
			{
				size_t i = s.find_first_of(_T("/\\"), 2);
				if (i == tstring::npos) { *OutPrefix = s; return; }
				i = s.find_first_of(_T("/\\"), i+1);
				if (i == tstring::npos) { *OutPrefix = s; return; }
				*OutPrefix = s.substr(0, i+1);
				Beg = i+1;
			}
			// Nie - po prostu <slash>
			else
			{
				CharToStr(OutPrefix, s[0]);
				Beg = 1;
			}
		}
		// <dysk> : <slash>
		else
		{
			size_t i = s.find(':');
			if (i != tstring::npos)
			{
				size_t j = s.find_first_of(_T("/\\"));
				if (j == tstring::npos || j > i+1)
				{
					*OutPrefix = s.substr(0, i+1);
					Beg = i+1;
				}
				else if (j > i)
				{
					*OutPrefix = s.substr(0, j+1);
					Beg = j+1;
				}
			}
		}
	}

	// TrailingPathDelimiter
	if (End > Beg)
	{
		if (IsPathSlash(s[End-1]))
		{
			CharToStr(OutTrailingPathDelimiter, s[End-1]);
			End--;
		}
	}

	assert(End >= Beg);
	*OutPath = s.substr(Beg, End-Beg);
}

bool PathIsAbsolute(const tstring &s)
{
	// Przepisane z czêœci Prefix funkcji DecomposePath
	if (s.empty())
		return false;
	// Zaczyna siê od <slash>
	if (IsPathSlash(s[0]))
		return true;
	// <dysk> : <slash>
	size_t i = s.find(_T(':'));
	if (i == tstring::npos)
		return false;
	size_t j = s.find_first_of(_T("/\\"));
	if (j == tstring::npos || j > i)
		return true;
	return false;
}

void IncludeTrailingPathDelimiter(tstring *InOutPath)
{
	if (InOutPath->empty())
		CharToStr(InOutPath, DIR_SEP);
	else
	{
		if (!IsPathSlash((*InOutPath)[InOutPath->length()-1]))
			*InOutPath += DIR_SEP;
	}
}

void IncludeTrailingPathDelimiter(tstring *OutPath, const tstring &InPath)
{
	if (InPath.empty())
		CharToStr(OutPath, DIR_SEP);
	else
	{
		if (!IsPathSlash(InPath[InPath.length()-1]))
			*OutPath = InPath + CharToStrR(DIR_SEP);
		else
			*OutPath = InPath;
	}
}

void ExcludeTrailingPathDelimiter(tstring *InOutPath)
{
	if (InOutPath->empty())
		return;
	if (IsPathSlash((*InOutPath)[InOutPath->length()-1]))
		InOutPath->erase(InOutPath->length()-1, 1);
}

void ExcludeTrailingPathDelimiter(tstring *OutPath, const tstring &InPath)
{
	if (InPath.empty())
		OutPath->clear();
	else
	{
		if (IsPathSlash(InPath[InPath.length()-1]))
			*OutPath = InPath.substr(0, InPath.length()-1);
		else
			*OutPath = InPath;
	}
}

void ExtractPathPrefix(tstring *OutPrefix, const tstring &s)
{
	OutPrefix->clear();
	size_t Beg = 0;

	// Skopiowana czêœæ Prefix z funkcji DecomposePath
	if (!s.empty())
	{
		// Zaczyna siê od <slash>
		if (IsPathSlash(s[0]))
		{
			// <slash> <slash> <serwer> <slash> <udzia³> <slash>
			if (s.length() > 1 && IsPathSlash(s[1]))
			{
				size_t i = s.find_first_of(_T("/\\"), 2);
				if (i == tstring::npos) { *OutPrefix = s; return; }
				i = s.find_first_of(_T("/\\"), i+1);
				if (i == tstring::npos) { *OutPrefix = s; return; }
				*OutPrefix = s.substr(0, i+1);
				Beg = i+1;
			}
			// Nie - po prostu <slash>
			else
			{
				CharToStr(OutPrefix, s[0]);
				Beg = 1;
			}
		}
		// <dysk> : <slash>
		else
		{
			size_t i = s.find(_T(':'));
			if (i != tstring::npos)
			{
				size_t j = s.find_first_of(_T("/\\"));
				if (j == tstring::npos || j > i+1)
				{
					*OutPrefix = s.substr(0, i+1);
					Beg = i+1;
				}
				else if (j > i)
				{
					*OutPrefix = s.substr(0, j+1);
					Beg = j+1;
				}
			}
		}
	}

	if (Beg > 0)
		*OutPrefix = s.substr(0, Beg+1);
}

void ExtractFilePath(tstring *OutPath, const tstring &s)
{
	size_t i = s.find_last_of(_T("\\/:"));
	if (i == tstring::npos)
		OutPath->clear();
	else
		*OutPath = s.substr(0, i + 1);
}

void ExtractFileName(tstring *OutFileName, const tstring &s)
{
	size_t i = s.find_last_of(_T("\\/:"));
	if (i == tstring::npos)
		*OutFileName = s;
	else
		*OutFileName = s.substr(i + 1);
}

void ExtractFileExt(tstring *OutExt, const tstring &s)
{
	size_t i = s.find_last_of(_T(".\\/:"));
	if ( (i != tstring::npos) && (i > 0) && (s[i] == _T('.')) )
		*OutExt = s.substr(i);
	else
		OutExt->clear();
}

void ChangeFileExt(tstring *Out, const tstring &FileName, const tstring &Ext)
{
	size_t i = FileName.find_last_of(_T(".\\/:"));
	if ( (i == tstring::npos) || (FileName[i] != _T('.')) )
		*Out = FileName + Ext;
	else
		*Out = FileName.substr(0, i) + Ext;
}

void NormalizePath(tstring *OutPath, const tstring &s)
{
	const tstring Delimiters = _T("/\\");
	tstring Path, Trailing, Tmp;
	DecomposePath(s, OutPath, &Path, &Trailing);
	STRING_VECTOR Dirs;
	size_t Index = 0;
	while (SplitFirstOf(Path, Delimiters, &Tmp, &Index))
	{
		if (Tmp.empty())
		{
			*OutPath = s;
			return;
		}
		if (Tmp[0] == _T('.'))
		{
			// ".."
			if (Tmp.length() == 2 && Tmp[1] == _T('.'))
			{
				if (Dirs.empty())
					Dirs.push_back(Tmp);
				else
					Dirs.pop_back();
			}
			// "." - nie rób nic
		}
		// Zwyk³y katalog
		else
			Dirs.push_back(Tmp);
	}

	for (size_t i = 0; i < Dirs.size(); i++)
	{
		if (!OutPath->empty())
			IncludeTrailingPathDelimiter(OutPath);
		*OutPath += Dirs[i];
	}
	OutPath->append(Trailing);
}

void RelativeToAbsolutePath(tstring *Out, const tstring &Base, const tstring &Path)
{
	if (PathIsAbsolute(Path))
		*Out = Path;
	else
	{
		tstring Tmp = Base;
		IncludeTrailingPathDelimiter(&Tmp);
		Tmp.append(Path);
		NormalizePath(Out, Tmp);
	}
}

void AbsoluteToRelativePath(tstring *Out, const tstring &Base, const tstring &Target)
{
	tstring BasePrefix, BasePath, BaseTrailing;
	tstring TargetPrefix, TargetPath, TargetTrailing;
	DecomposePath(Base, &BasePrefix, &BasePath, &BaseTrailing);
	DecomposePath(Target, &TargetPrefix, &TargetPath, &TargetTrailing);

	if (!PathCmp(BasePrefix, TargetPrefix))
	{
		*Out = Target;
		return;
	}

	// Spisz katalogi Base i Target
	const tstring DELIMITER = _T("/\\");
	STRING_VECTOR BaseDirs, TargetDirs;
	size_t i = 0;
	tstring Tmp;
	while (SplitFirstOf(BasePath, DELIMITER, &Tmp, &i))
		BaseDirs.push_back(Tmp);
	i = 0;
	while (SplitFirstOf(TargetPath, DELIMITER, &Tmp, &i))
		TargetDirs.push_back(Tmp);

	tstring R;
	// Wyeliminuj wspólne katalogi z pocz¹tku œcie¿ek
	size_t DirsDifferenceIndex = 0;
	while (DirsDifferenceIndex < BaseDirs.size() && DirsDifferenceIndex < TargetDirs.size() && PathCmp(BaseDirs[DirsDifferenceIndex], TargetDirs[DirsDifferenceIndex]))
		DirsDifferenceIndex++;

	// Przepisz na wyjœcie "/.." lub "\.."
	for (size_t i = DirsDifferenceIndex; i < BaseDirs.size(); i++)
	{
		if (!R.empty())
			IncludeTrailingPathDelimiter(&R);
		R.append(_T(".."));
	}
	// Przepisz na wyjœcie to co w Target
	for (size_t i = DirsDifferenceIndex; i < TargetDirs.size(); i++)
	{
		if (!R.empty())
			IncludeTrailingPathDelimiter(&R);
		R.append(TargetDirs[i]);
	}
	// Opcjonalny koñcz¹cy '/' lub '\'
	bool TrailingPathDelimiter = (!Target.empty() && Target[Target.length()-1] == DIR_SEP);
	if (TrailingPathDelimiter)
		IncludeTrailingPathDelimiter(&R);
	NormalizePath(Out, R);
}


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// KONWERSJE

/*
Locale dla funkcji konwertuj¹cych, ¿eby nie uwzglêdnia³y aktualnie ustawionego,
tylko zawsze u¿ywa³y standardowego, ¿eby np. przy konwersji float i double
u¿ywana by³a zawsze kropka, a nie przecinek jeœli ustawiony jest polski locale.
Niestety nie zwalniam tego obiektu na koniec, choæ powinienem wywo³aæ
_free_locale.
*/
#ifdef WIN32
	static _locale_t g_MainLocale = _create_locale(LC_ALL, "C");
#endif

static const tchar _DIGITS_L[] = _T("0123456789abcdefghijklmnopqrstuvwxyz");
static const tchar _DIGITS_U[] = _T("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ");

// Zale¿nie od precyzji
static const tchar * const DOUBLE_TO_STR_FORMAT[] = {
	_T("%.0"), _T("%.1"), _T("%.2"), _T("%.3"), _T("%.4"), _T("%.5"), _T("%.6"), _T("%.7"), _T("%.8"), _T("%.9"), _T("%.10"), _T("%.11"), _T("%.12"), _T("%.13"), _T("%.14"), _T("%.15"), _T("%.16"), _T("%.17"), _T("%.18"), _T("%.19"), _T("%.20"),
};

void DoubleToStr(tstring *Out, double x, char mode, int precision)
{
	tchar sz[256], Format[6];

#ifdef WIN32
	#ifdef _UNICODE
		wcscpy_s(Format, _countof(Format), DOUBLE_TO_STR_FORMAT[precision]);
		size_t l = _tcslen(Format);
		Format[l] = mode;
		Format[l+1] = L'\0';
		_stprintf_s_l(sz, _countof(sz), Format, g_MainLocale, x);
	#else
		#ifdef _MSC_VER
			strcpy_s(Format, _countof(Format), DOUBLE_TO_STR_FORMAT[precision]);
		#else
			strcpy(Format, DOUBLE_TO_STR_FORMAT[precision]);
		#endif
		size_t l = strlen(Format);
		Format[l] = mode;
		Format[l+1] = '\0';
		_sprintf_s_l(sz, _countof(sz), Format, g_MainLocale, x);
	#endif
#else
	strcpy(Format, DOUBLE_TO_STR_FORMAT[precision]);
	size_t l = strlen(Format);
	Format[l] = mode;
	Format[l+1] = '\0';
	sprintf(sz, Format, x);
#endif

	*Out = sz;
}

void FloatToStr(tstring *Out, float x, char mode, int precision)
{
	DoubleToStr(Out, static_cast<double>(x), mode, precision);
}

int StrToDouble(double *out, const tstring &s)
{
	////// Sprawdzenie sk³adni
	size_t pos = 0;
	// Whitespace
	for (;;)
	{
		if (pos == s.length())
			return -1;
		if (s[pos] == ' ' || s[pos] == _T('\t'))
			pos++;
		else
			break;
	}
	// Sign
	if (pos == s.length())
		return -1;
	if (s[pos] == _T('+') || s[pos] == _T('-'))
		pos++;
	// Digits
	if (pos == s.length())
		return -1;
	bool DigitsBeforePoint = false;
	while (pos < s.length() && (s[pos] >= _T('0') && s[pos] <= _T('9')))
	{
		DigitsBeforePoint = true;
		pos++;
	}
	// .Digits
	bool DigitsAfterPoint = false;
	if (pos == s.length() && !DigitsBeforePoint)
		return -1;
	if (pos < s.length() && s[pos] == _T('.'))
	{
		pos++;
		while (pos < s.length())
		{
			if (s[pos] >= _T('0') && s[pos] <= _T('9'))
			{
				pos++;
				DigitsAfterPoint = true;
			}
			else
				break;
		}
	}
	if (!DigitsBeforePoint && !DigitsAfterPoint)
		return -1;
	// {DdEe}
	if (pos < s.length())
	{
		if (!(s[pos] == _T('d') || s[pos] == _T('D') || s[pos] == _T('e') || s[pos] == _T('E')))
			return -1;
		pos++;
		// Sign
		if (pos <= s.length() && (s[pos] == _T('+') || s[pos] == _T('-')))
			pos++;
		// Digits
		if (pos == s.length())
			return -1;
		while (pos < s.length() && (s[pos] >= _T('0') && s[pos] <= _T('9')))
			pos++;
		if (pos < s.length())
			return -1;
	}

	////// Konwersja w³aœciwa
	#ifdef WIN32
		#ifdef _UNICODE
			*out = _wtof_l(s.c_str(), g_MainLocale);
		#else
			*out = _atof_l(s.c_str(), g_MainLocale);
		#endif
	#else
		*out = atof(s.c_str());
	#endif

	return 0;
}

int StrToFloat(float *out, const tstring &s)
{
	double d;
	int r = StrToDouble(&d, s);
	if (r != 0)
		return r;
	*out = (float)d;
	return 0;
}

void BoolToStr(tstring *Out, bool x, char mode)
{
	switch (mode)
	{
	case '0':
		*Out = (x ? _T("1") : _T("0"));
		return;
	case 'F':
		*Out = (x ? _T("True") : _T("False"));
		return;
	case 'U':
		*Out = (x ? _T("TRUE") : _T("FALSE"));
		return;
	case 'g':
		*Out = (x ? _T("t") : _T("f"));
		return;
	case 'G':
		*Out = (x ? _T("T") : _T("F"));
		return;
	default: // 'f'
		*Out = (x ? _T("true") : _T("false"));
		return;
	}
}

bool StrToBool(bool *result, const tstring &s)
{
	if (s == _T("0") || s == _T("f") || s == _T("F") || s == _T("false") || s == _T("False") || s == _T("FALSE"))
		*result = false;
	else if (s == _T("1") || s == _T("t") || s == _T("T") || s == _T("true") || s == _T("True") || s == _T("TRUE"))
		*result = true;
	else
		return false;

	return true;
}

void PtrToStr(tstring *Out, const void* p)
{
	tchar sz[9];
#ifdef _UNICODE
	_stprintf_s(sz, _countof(sz), L"%p", p);
#else
	#ifdef _MSC_VER
		sprintf_s(sz, _countof(sz), "%p", p);
	#else
		sprintf(sz, "%p", p);
	#endif
#endif
	*Out = sz;
}

static uint8 ROT13_TABLE[] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,78,79,80,81,82,83,84,85,86,87,88,89,90,65,66,67,68,69,70,71,72,73,74,75,76,77,91,92,93,94,95,96,110,111,112,113,114,115,116,117,118,119,120,121,122,97,98,99,100,101,102,103,104,105,106,107,108,109,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255 };

void Rot13(tstring *InOut)
{
#ifdef _UNICODE
	int ci;
	for (size_t i = 0; i < InOut->length(); i++)
	{
		ci = TCHAR_TO_INT((*InOut)[i]);
		if (ci < 256)
			(*InOut)[i] = INT_TO_TCHAR(ROT13_TABLE[ci]);
	}
#else
	for (size_t i = 0; i < InOut->length(); i++)
		(*InOut)[i] = (char)ROT13_TABLE[(uint8)(*InOut)[i]];
#endif
}


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Matematyka

#ifdef WIN32
	bool is_finite(float x)  { return (_finite(x) != 0); }
	bool is_finite(double x) { return (_finite(x) != 0); }
	bool is_nan(float x)  { return (_isnan(x) != 0); }
	bool is_nan(double x) { return (_isnan(x) != 0); }
#endif

static const uint BIT_MASKS[33] = {
	0x00000000, 0x00000001, 0x00000003, 0x00000007,
	0x0000000F, 0x0000001F, 0x0000003F, 0x0000007F,
	0x000000FF, 0x000001FF, 0x000003FF, 0x000007FF,
	0x00000FFF, 0x00001FFF, 0x00003FFF, 0x00007FFF,
	0x0000FFFF, 0x0001FFFF, 0x0003FFFF, 0x0007FFFF,
	0x000FFFFF, 0x001FFFFF, 0x003FFFFF, 0x007FFFFF,
	0x00FFFFFF, 0x01FFFFFF, 0x03FFFFFF, 0x07FFFFFF,
	0x0FFFFFFF, 0x1FFFFFFF, 0x3FFFFFFF, 0x7FFFFFFF,
	0xFFFFFFFF,
};

uint32 GetBitMask(uint32 n)
{
	assert(n <= 32);
	return BIT_MASKS[n];
}

/// \internal
static const int MultiplyDeBruijnBitPosition[32] = {
	0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8,
	31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9,
};

uint log2u(uint v)
{
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v = (v >> 1) + 1;
	return MultiplyDeBruijnBitPosition[(v * 0x077CB531UL) >> 27];
}

/// \internal
static unsigned int const PowersOf10[] = {
	1, 10, 100, 1000, 10000, 100000,
	1000000, 10000000, 100000000, 1000000000,
};

uint log10u(uint v)
{
	uint t = (log2u(v) + 1) * 1233 >> 12;
	return t - (v < PowersOf10[t]);
}

/// \internal
static const unsigned int InterleaveBits_B[] = {0x55555555, 0x33333333, 0x0F0F0F0F, 0x00FF00FF};
/// \internal
static const unsigned int InterleaveBits_S[] = {1, 2, 4, 8};

uint InterleaveBits(uint a, uint b)
{
	a = (a | (a << InterleaveBits_S[3])) & InterleaveBits_B[3];
	a = (a | (a << InterleaveBits_S[2])) & InterleaveBits_B[2];
	a = (a | (a << InterleaveBits_S[1])) & InterleaveBits_B[1];
	a = (a | (a << InterleaveBits_S[0])) & InterleaveBits_B[0];

	b = (b | (b << InterleaveBits_S[3])) & InterleaveBits_B[3];
	b = (b | (b << InterleaveBits_S[2])) & InterleaveBits_B[2];
	b = (b | (b << InterleaveBits_S[1])) & InterleaveBits_B[1];
	b = (b | (b << InterleaveBits_S[0])) & InterleaveBits_B[0];

	return a | (b << 1);
}

uint Extend10BitsBy2Zeros(uint n)
{
	// Na podstawie ksi¹¿ki: Real-Time Collision Detection, Christer Ericson

    // n = ----------------------9876543210 : Bits initially
    // n = ------98----------------76543210 : After (1)
    // n = ------98--------7654--------3210 : After (2)
    // n = ------98----76----54----32----10 : After (3)
    // n = ----9--8--7--6--5--4--3--2--1--0 : After (4)
    n = (n ^ (n << 16)) & 0xff0000ff; // (1)
    n = (n ^ (n <<  8)) & 0x0300f00f; // (2)
    n = (n ^ (n <<  4)) & 0x030c30c3; // (3)
    n = (n ^ (n <<  2)) & 0x09249249; // (4)
    return n;
}

uint Extend16BitsBy1Zero(uint n)
{
	// Na podstawie ksi¹¿ki: Real-Time Collision Detection, Christer Ericson

    // n = ----------------fedcba9876543210 : Bits initially
    // n = --------fedcba98--------76543210 : After (1)
    // n = ----fedc----ba98----7654----3210 : After (2)
    // n = --fe--dc--ba--98--76--54--32--10 : After (3)
    // n = -f-e-d-c-b-a-9-8-7-6-5-4-3-2-1-0 : After (4)
    n = (n ^ (n <<  8)) & 0x00ff00ff; // (1)
    n = (n ^ (n <<  4)) & 0x0f0f0f0f; // (2)
    n = (n ^ (n <<  2)) & 0x33333333; // (3)
    n = (n ^ (n <<  1)) & 0x55555555; // (4)
    return n;
}

void sincos(float angle, float *sine, float *cosine)
{
	#if defined(WIN32) && !defined(_WIN64)
		// Procedurê mam od: Karol Kuczmarski "Xion"
		__asm
		{
			mov eax, sine
			mov edx, cosine
			fld     angle
			fsincos
			fstp    dword ptr [edx]
			fstp    dword ptr [eax]
		}

	#else
		*sine = sinf(angle);
		*cosine = cosf(angle);

	#endif
}

bool is_prime(unsigned n)
{
	if (n == 1) return false;
	if (n < 4) return true; // 2 and 3 are prime
	if (n % 2 == 0) return false;
	if (n < 9) return true; // we have already excluded 4, 6 and 8.
	if (n % 3 == 0) return false;
	unsigned r = (unsigned)sqrt((double)n);
	unsigned f = 5;
	while (f <= r) {
		if (n % f == 0) return false;
		if (n % (f+2) == 0) return false;
		f += 6;
	}
	return true;
}

// Funkcje SmoothCD s¹ napisane na podstawie ksi¹¿ki:
// "Game Programming Gems", tom 4, rozdz. 1.10, str. 95.

void SmoothCD(float *InOutPos, float Dest, float *InOutVel, float SmoothTime, float TimeDelta)
{
	float Omega = 2.f / SmoothTime;
	float x = Omega * TimeDelta;
	// Przybli¿enie funkcji 1 / exp(x) doœæ dok³adne dla wartoœci 0..1
	float Exp = 1.f / (1.f + x + 0.48f * x * x + 0.235f * x * x * x);
	float Change = *InOutPos - Dest;
	float Temp = (*InOutVel + Omega * Change) * TimeDelta;
	*InOutVel = (*InOutVel - Omega * Temp) * Exp;
	*InOutPos = Dest + (Change + Temp) * Exp;
}

void SmoothCD(float *InOutPos, float Dest, float *InOutVel, float SmoothTime, float TimeDelta, float MaxSpeed)
{
	float Omega = 2.f / SmoothTime;
	float x = Omega * TimeDelta;
	// Przybli¿enie funkcji 1 / exp(x) doœæ dok³adne dla wartoœci 0..1
	float Exp = 1.f / (1.f + x + 0.48f * x * x + 0.235f * x * x * x);
	float Change = *InOutPos - Dest;
	float MaxChange = MaxSpeed*SmoothTime;
	Change = minmax(-MaxChange, Change, MaxChange);
	float Temp = (*InOutVel + Omega * Change) * TimeDelta;
	*InOutVel = (*InOutVel - Omega * Temp) * Exp;
	*InOutPos = Dest + (Change + Temp) * Exp;
}

int QuadraticEquation(float a, float b, float c, float *Out_x1, float *Out_x2)
{
	float delta = (b * b) - (4.0f * a * c);
	if (delta < 0.f)
		return 0;
	else if (FLOAT_ALMOST_ZERO(delta))
	{
		*Out_x1 = *Out_x2 = - b / (2.0f * a);
		return 1;
	}
	else
	{
		float denominator = 1.0f / (2.0f * a);
		float delta_sqr = sqrtf(delta);
		*Out_x1 = (- b - delta_sqr) * denominator;
		*Out_x2 = (- b + delta_sqr) * denominator;
		return 2;
	}
}

void CalcMeanAndVariance(const float Numbers[], size_t NumberCount, float *OutMean, float *OutVariance, bool VarianceBiased)
{
	CalcMeanAndVariance(Numbers, NumberCount, sizeof(float), OutMean, OutVariance, VarianceBiased);
}

void CalcMeanAndVariance(const void *NumberData, size_t NumberCount, int NumberStride, float *OutMean, float *OutVariance, bool VarianceBiased)
{
	assert(NumberCount > 0);
	const char *NumberBytes = (const char*)NumberData;
	*OutMean = *(const float*)NumberBytes;
	NumberBytes += NumberStride;

	for (size_t i = 1; i < NumberCount; i++)
	{
		*OutMean += *(const float*)NumberBytes;
		NumberBytes += NumberStride;
	}

	float NumberCountRcp = 1.0f / (float)NumberCount;
	*OutMean *= NumberCountRcp;

	if (OutVariance)
	{
		NumberBytes = (const char*)NumberData;
		*OutVariance = (*(const float*)NumberBytes - *OutMean) * (*(const float*)NumberBytes - *OutMean);
		NumberBytes += NumberStride;
		float Tmp;

		for (size_t i = 1; i < NumberCount; i++)
		{
			Tmp = *(const float*)NumberBytes - *OutMean;
			*OutVariance += Tmp * Tmp;
			NumberBytes += NumberStride;
		}

		if (VarianceBiased)
			*OutVariance /= (float)(NumberCount - 1);
		else
			*OutVariance *= NumberCountRcp;
	}
}

uint MurmurHash(const void *Data, uint DataLen, uint Seed)
{
	// Autor: Austin Appleby, http://murmurhash.googlepages.com/

	// 'm' and 'r' are mixing constants generated offline.
	// They're not really 'magic', they just happen to work well.
	const unsigned int m = 0x5bd1e995;
	const int r = 24;

	// Initialize the hash to a 'random' value
	unsigned int h = Seed ^ DataLen;

	// Mix 4 bytes at a time into the hash
	const unsigned char * data = (const unsigned char *)Data;

	while(DataLen >= 4)
	{
		unsigned int k = *(unsigned int *)data;
		k *= m; 
		k ^= k >> r; 
		k *= m; 
		h *= m; 
		h ^= k;
		data += 4;
		DataLen -= 4;
	}

	// Handle the last few bytes of the input array
	switch(DataLen)
	{
	case 3: h ^= data[2] << 16;
	case 2: h ^= data[1] << 8;
	case 1: h ^= data[0];
	h *= m;
	};

	// Do a few final mixes of the hash to ensure the last few
	// bytes are well-incorporated.
	h ^= h >> 13;
	h *= m;
	h ^= h >> 15;

	return h;
}

#define get16bits(d) ((((uint)(((const uint8 *)(d))[1])) << 8)\
                       +(uint)(((const uint8 *)(d))[0]) )

uint SuperFastHash(const void *DataBytes, size_t DataLen)
{
	const char *data = (const char *)DataBytes;
    assert(DataLen <= UINT_MAX);
	uint hash = (uint)DataLen, tmp;
	int rem;

	assert(data != NULL && DataLen > 0);

    rem = DataLen & 3;
    DataLen >>= 2;

    // Main loop
    for (;DataLen > 0; DataLen--) {
        hash  += get16bits (data);
        tmp    = (get16bits (data+2) << 11) ^ hash;
        hash   = (hash << 16) ^ tmp;
        data  += 2*sizeof (uint16);
        hash  += hash >> 11;
    }

    // Handle end cases
    switch (rem) {
        case 3: hash += get16bits (data);
                hash ^= hash << 16;
                hash ^= data[sizeof (uint16)] << 18;
                hash += hash >> 11;
                break;
        case 2: hash += get16bits (data);
                hash ^= hash << 11;
                hash += hash >> 17;
                break;
        case 1: hash += *data;
                hash ^= hash << 10;
                hash += hash >> 1;
    }

    // Force "avalanching" of final 127 bits
    hash ^= hash << 3;
    hash += hash >> 5;
    hash ^= hash << 4;
    hash += hash >> 17;
    hash ^= hash << 25;
    hash += hash >> 6;

    return hash;
}

#undef get16bits

/*
Szum Perlina (Perlin Noise) - algorytm na podstawie implementacji referencyjnej
w jêzyku Java - "Improved Noise reference implementation", Ken Perlin, 2002,
http://mrl.nyu.edu/~perlin/noise/
*/

inline float Perlin_Fade(float t)
{
	return t * t * t * (t * (t * 6 - 15) + 10);
}

inline float Perlin_Lerp(float t, float a, float b)
{
	return a + t * (b - a);
}

inline float Perlin_Grad1(int hash, float x)
{
	// Convert lo 4 bits of hash code into 12 gradient directions
	int h = hash & 15;
	float u = h<8 ? x : 0.f, v = h<4 ? 0.f : h==12||h==14 ? x : 0.f;
	return ((h&1) == 0 ? u : -u) + ((h&2) == 0 ? v : -v);
}

inline float Perlin_Grad2(int hash, float x, float y)
{
	// Convert lo 4 bits of hash code into 12 gradient directions
	int h = hash & 15;
	float u = h<8 ? x : y, v = h<4 ? y : h==12||h==14 ? x : 0.f;
	return ((h&1) == 0 ? u : -u) + ((h&2) == 0 ? v : -v);
}

inline float Perlin_Grad3(int hash, float x, float y, float z)
{
	// Convert lo 4 bits of hash code into 12 gradient directions
	int h = hash & 15;
	float u = h<8 ? x : y, v = h<4 ? y : h==12||h==14 ? x : z;
	return ((h&1) == 0 ? u : -u) + ((h&2) == 0 ? v : -v);
}

int Perlin_P[] = {
	151,160,137,91,90,15,
	131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
	190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
	88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
	77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
	102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
	135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
	5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
	223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
	129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
	251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
	49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
	138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
	,
	151,160,137,91,90,15,
	131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
	190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
	88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
	77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
	102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
	135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
	5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
	223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
	129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
	251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
	49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
	138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
};

float PerlinNoise1(float x)
{
	// Find unit cube that contains point
	int X = (int)x & 255;
	// Find relative x,y,z of point in cube
	x -= floorf(x);
	// Compute Perlin_Fade curves for each x,y,z
	float u = Perlin_Fade(x);
	// Hash coordinates of the 8 cube corners
	int A = Perlin_P[X  ], AA = Perlin_P[A];
	int B = Perlin_P[X+1], BA = Perlin_P[B];

	// and add blended results from 8 curners of cube
	return Perlin_Lerp(u,
		Perlin_Grad1(Perlin_P[AA  ], x   ),
		Perlin_Grad1(Perlin_P[BA  ], x-1 ));
}

float PerlinNoise2(float x, float y)
{
	// Find unit cube that contains point
	int X = (int)x & 255;
	int Y = (int)y & 255;
	// Find relative x,y,z of point in cube
	x -= floorf(x);
	y -= floorf(y);
	// Compute Perlin_Fade curves for each x,y,z
	float u = Perlin_Fade(x);
	float v = Perlin_Fade(y);
	// Hash coordinates of the 8 cube corners
	int A = Perlin_P[X  ]+Y, AA = Perlin_P[A], AB = Perlin_P[A+1];
	int B = Perlin_P[X+1]+Y, BA = Perlin_P[B], BB = Perlin_P[B+1];

	// and add blended results from 8 curners of cube
	return Perlin_Lerp(v,
		Perlin_Lerp(u,
			Perlin_Grad2(Perlin_P[AA  ], x  , y   ),
			Perlin_Grad2(Perlin_P[BA  ], x-1, y   )),
		Perlin_Lerp(u,
			Perlin_Grad2(Perlin_P[AB  ], x  , y-1 ),
			Perlin_Grad2(Perlin_P[BB  ], x-1, y-1 )));
}

float PerlinNoise3(float x, float y, float z)
{
	// Find unit cube that contains point
	int X = (int)x & 255;
	int Y = (int)y & 255;
	int Z = (int)z & 255;
	// Find relative x,y,z of point in cube
	x -= floorf(x);
	y -= floorf(y);
	z -= floorf(z);
	// Compute Perlin_Fade curves for each x,y,z
	float u = Perlin_Fade(x);
	float v = Perlin_Fade(y);
	float w = Perlin_Fade(z);
	// Hash coordinates of the 8 cube corners
	int A = Perlin_P[X  ]+Y, AA = Perlin_P[A]+Z, AB = Perlin_P[A+1]+Z;
	int B = Perlin_P[X+1]+Y, BA = Perlin_P[B]+Z, BB = Perlin_P[B+1]+Z;

	// and add blended results from 8 curners of cube
	return Perlin_Lerp(w,
		Perlin_Lerp(v,
			Perlin_Lerp(u,
				Perlin_Grad3(Perlin_P[AA  ], x  , y  , z   ),
				Perlin_Grad3(Perlin_P[BA  ], x-1, y  , z   )),
			Perlin_Lerp(u,
				Perlin_Grad3(Perlin_P[AB  ], x  , y-1, z   ),
				Perlin_Grad3(Perlin_P[BB  ], x-1, y-1, z   ))),
		Perlin_Lerp(v,
			Perlin_Lerp(u,
				Perlin_Grad3(Perlin_P[AA+1], x  , y  , z-1 ),
				Perlin_Grad3(Perlin_P[BA+1], x-1, y  , z-1 )),
			Perlin_Lerp(u,
				Perlin_Grad3(Perlin_P[AB+1], x  , y-1, z-1 ),
				Perlin_Grad3(Perlin_P[BB+1], x-1, y-1, z-1 ))));
}

float BrownianNoise1(float x, uint i, float Persistence)
{
	float R = 0.f;
	float Frequency = 1.f, Amplitude = 1.f;
	for (uint j = 0; j < i; j++)
	{
		R += PerlinNoise1(x * Frequency) * Amplitude;
		Frequency *= 2.f;
		Amplitude *= Persistence;
	}
	return R;
}

float BrownianNoise2(float x, float y, uint i, float Persistence)
{
	float R = 0.f;
	float Frequency = 1.f, Amplitude = 1.f;
	for (uint j = 0; j < i; j++)
	{
		R += PerlinNoise2(x * Frequency, y * Frequency) * Amplitude;
		Frequency *= 2.f;
		Amplitude *= Persistence;
	}
	return R;
}

float BrownianNoise3(float x, float y, float z, uint i, float Persistence)
{
	float R = 0.f;
	float Frequency = 1.f, Amplitude = 1.f;
	for (uint j = 0; j < i; j++)
	{
		R += PerlinNoise3(x * Frequency, y * Frequency, z * Frequency) * Amplitude;
		Frequency *= 2.f;
		Amplitude *= Persistence;
	}
	return R;
}


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// GENERATOR LICZB PSEUDOLOSOWYCH

RandomGenerator::RandomGenerator() :
	m_NextNormalNumberIs(false)
{
	m_Seed = (uint32)time(0);
}

void RandomGenerator::RandData(void *OutData, size_t DataLength)
{
	size_t WordCount = DataLength >> 2;
	size_t RemainingBytes = DataLength & 0x3;

	uint *OutWords = (uint*)OutData;
	while (WordCount > 0)
	{
		*OutWords = RandUint();
		OutWords++;
		WordCount--;
	}

	if (RemainingBytes > 0)
	{
		uint x = RandUint();
		uint8 *OutBytes = (uint8*)OutWords;
		switch (RemainingBytes)
		{
		case 3:
			*OutBytes = (x >> 23) & 0xFF; // Te przesuniêcia bitowe s¹ przypadkowe
			OutBytes++;
		case 2:
			*OutBytes = (x >> 9) & 0xFF;
			OutBytes++;
		case 1:
			*OutBytes = x & 0xFF;
		}
	}
}

float RandomGenerator::RandNormal(float sigma)
{
	// Nowa implementacja
	// The polar form of the Box-Muller transformation
	// ród³o: http://www.taygeta.com/random/gaussian.html
	// Ten algorytm generuje dwie liczby na raz, wiêc jedn¹ zwraca od razu, a drug¹
	// zapamiêtuje do nastêpnego wywo³ania tej metody.
	if (m_NextNormalNumberIs)
	{
		m_NextNormalNumberIs = false;
		return m_NextNormalNumber * sigma;
	}
	else
	{
		float x1, x2, w;
		do {
			x1 = 2.0f * RandFloat() - 1.0f;
			x2 = 2.0f * RandFloat() - 1.0f;
			w = x1 * x1 + x2 * x2;
		} while (w >= 1.0f);
		w = sqrtf((-2.0f * logf(w)) / w);
		m_NextNormalNumber = x2 * w; // Druga wygenerowana liczba
		m_NextNormalNumberIs = true;
		return x1 * w * sigma; // Pierwsza wygenerowana liczba
	}

	// Stara implementacja
/*	float u1 = RandFloat(), u2 = RandFloat();
	float z = sqrtf(-2.0f*log(u1)) * cosf(2*PI*u2);
	if (RandBool())
		return z * sigma;
	else
		return -z * sigma;*/
}

RandomGenerator g_Rand;


#define MERSENNE_N 624
#define MERSENNE_M 397
#define MERSENNE_MATRIX_A 0x9908b0dfUL   /* constant vector a */
#define MERSENNE_UMASK 0x80000000UL /* most significant w-r bits */
#define MERSENNE_LMASK 0x7fffffffUL /* least significant r bits */
#define MERSENNE_MIXBITS(u,v) ( ((u) & MERSENNE_UMASK) | ((v) & MERSENNE_LMASK) )
#define MERSENNE_TWIST(u,v) ((MERSENNE_MIXBITS(u,v) >> 1) ^ ((v)&1U ? MERSENNE_MATRIX_A : 0U))

MersenneTwister::MersenneTwister()
{
	Init((uint32)time(0));
}

MersenneTwister::MersenneTwister(uint Seed)
{
	Init(Seed);
}

MersenneTwister::MersenneTwister(uint InitKeys[], int KeyCount)
{
	int i, j, k;
	Init(19650218U);
	i=1; j=0;
	k = (MERSENNE_N>KeyCount ? MERSENNE_N : KeyCount);
	for (; k; k--)
	{
		m_State[i] = (m_State[i] ^ ((m_State[i-1] ^ (m_State[i-1] >> 30)) * 1664525U)) + InitKeys[j] + j;
		m_State[i] &= 0xffffffffU;
		i++; j++;
		if (i>=MERSENNE_N) { m_State[0] = m_State[MERSENNE_N-1]; i=1; }
		if (j>=KeyCount) j=0;
	}
	for (k=MERSENNE_N-1; k; k--)
	{
		m_State[i] = (m_State[i] ^ ((m_State[i-1] ^ (m_State[i-1] >> 30)) * 1566083941U)) - i;
		//m_State[i] &= 0xffffffffU; /* for WORDSIZE > 32 machines */
		i++;
		if (i>=MERSENNE_N) { m_State[0] = m_State[MERSENNE_N-1]; i=1; }
	}

	m_State[0] = 0x80000000UL; /* MSB is 1; assuring non-zero initial array */ 
	m_Left = 1;
}

void MersenneTwister::Init(uint Seed)
{
	int j;
	m_State[0] = Seed & 0xffffffffU;
	for (j=1; j<MERSENNE_N; j++)
	{
		m_State[j] = (1812433253U * (m_State[j-1] ^ (m_State[j-1] >> 30)) + j); 
		m_State[j] &= 0xffffffffU;
	}
	m_Left = 1;
}

void MersenneTwister::NextState()
{
	unsigned *p=m_State;
	int j;

	m_Left = MERSENNE_N;
	m_Next = m_State;

	for (j=MERSENNE_N-MERSENNE_M+1; --j; p++) 
		*p = p[MERSENNE_M] ^ MERSENNE_TWIST(p[0], p[1]);

	for (j=MERSENNE_M; --j; p++) 
		*p = p[MERSENNE_M-MERSENNE_N] ^ MERSENNE_TWIST(p[0], p[1]);

	*p = p[MERSENNE_M-MERSENNE_N] ^ MERSENNE_TWIST(p[0], m_State[0]);
}

uint MersenneTwister::NextUint()
{
	unsigned y;

	if (--m_Left == 0) NextState();
	y = *m_Next++;

	y ^= (y >> 11);
	y ^= (y << 7) & 0x9d2c5680U;
	y ^= (y << 15) & 0xefc60000U;
	y ^= (y >> 18);

	return y;
}

int MersenneTwister::NextInt31()
{
	unsigned y;

	if (--m_Left == 0) NextState();
	y = *m_Next++;

	y ^= (y >> 11);
	y ^= (y << 7) & 0x9d2c5680U;
	y ^= (y << 15) & 0xefc60000U;
	y ^= (y >> 18);

	return (int)(y>>1);
}

double MersenneTwister::NextReal1()
{
	unsigned y;

	if (--m_Left == 0) NextState();
	y = *m_Next++;

	y ^= (y >> 11);
	y ^= (y << 7) & 0x9d2c5680U;
	y ^= (y << 15) & 0xefc60000U;
	y ^= (y >> 18);

	return (double)y * (1.0/4294967295.0); 
	// divided by 2^32-1
}

double MersenneTwister::NextReal2()
{
	unsigned y;

	if (--m_Left == 0) NextState();
	y = *m_Next++;

	y ^= (y >> 11);
	y ^= (y << 7) & 0x9d2c5680U;
	y ^= (y << 15) & 0xefc60000U;
	y ^= (y >> 18);

	return (double)y * (1.0/4294967296.0); 
	// divided by 2^32
}

double MersenneTwister::NextReal3()
{
	unsigned y;

	if (--m_Left == 0) NextState();
	y = *m_Next++;

	y ^= (y >> 11);
	y ^= (y << 7) & 0x9d2c5680U;
	y ^= (y << 15) & 0xefc60000U;
	y ^= (y >> 18);

	return ((double)y + 0.5) * (1.0/4294967296.0); 
	// divided by 2^32
}

double MersenneTwister::NextReal_Res53()
{
	unsigned a=NextUint()>>5, b=NextUint()>>6; 
	return(a*67108864.0+b)*(1.0/9007199254740992.0); 
}


UniqueGenerator::UniqueGenerator() :
	m_Next(1)
{
}

UniqueGenerator::UniqueGenerator(uint32 First) :
	m_Next(First)
{
}

void UniqueGenerator::GetString(tstring *Out)
{
	UintToStr2(Out, m_Next, 8, 16);
	m_Next++;
}

void UniqueGenerator::GetString(tstring *Out, const tstring &Prefix)
{
	tstring s;
	UintToStr2(&s, m_Next, 8, 16);

	*Out = Prefix;
	*Out += s;

	m_Next++;
}


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Parser wiersza poleceñ

class CmdLineParser_pimpl
{
private:
	struct SHORT_OPT
	{
		uint Id;
		tchar Opt;
		bool Parameter;

		SHORT_OPT(uint Id, tchar Opt, bool Parameter) : Id(Id), Opt(Opt), Parameter(Parameter) { }
	};

	struct LONG_OPT
	{
		uint Id;
		tstring Opt;
		bool Parameter;

		LONG_OPT(uint Id, tstring Opt, bool Parameter) : Id(Id), Opt(Opt), Parameter(Parameter) { }
	};

	// ==== Warstwa I ====

	// Zawsze niezerowe jest albo m_argv, albo m_CmdLine.
	tchar **m_argv;
	const tchar *m_CmdLine;
	// Wa¿ne tylko, jeœli m_argv != NULL
	int m_argc;
	// Wa¿ne tylko jeœli m_CmdLine != NULL
	size_t m_CmdLineLength;
	// Jeœli m_argv != NULL, przechowuje indeks nastêpnego argumentu w m_argv.
	// Jeœli m_CmdLine != NULL, przechowuje indeks nastêpnego znaku w m_CmdLine.
	size_t m_ArgIndex;

	// Zwraca nastêpny argumet
	// Zwraca false, jeœli nie odczytano nastêpnego argumentu, bo ju¿ koniec.
	bool ReadNextArg(tstring *OutArg);

	// ==== Zarejestrowane argumenty ====

	std::vector<SHORT_OPT> m_ShortOpts;
	std::vector<LONG_OPT> m_LongOpts;

	// Zwraca wskaŸnk do struktury ze znalezion¹ opcj¹ lub NULL jeœli nie znalaz³
	SHORT_OPT * FindShortOpt(tchar Opt);
	LONG_OPT * FindLongOpt(const tstring &Opt);

	// ==== Warstwa II ====

	// Stan - czy jesteœmy wewn¹rz parsowania ci¹gu po³¹czonych opcji jednoznakowych
	bool m_InsideMultioption;
	// Zapamiêtany ostatnio pobrany argument - tylko jeœli m_InsideMultioption == true
	tstring m_LastArg;
	// Indeks nastêpnego znaku do sparsowania w tym argumencie - tylko jeœli m_InsideMultioption == true
	size_t m_LastArgIndex;
	// Dane do zwracania w zwi¹zku z ostatnio odczytan¹ informacj¹
	uint m_LastOptId;
	tstring m_LastParameter;

public:
	CmdLineParser_pimpl(int argc, tchar **argv);
	CmdLineParser_pimpl(const tchar *CmdLine);

	void RegisterOpt(uint Id, tchar Opt, bool Parameter);
	void RegisterOpt(uint Id, const tstring &Opt, bool Parameter);

	CmdLineParser::RESULT ReadNext();
	uint GetOptId();
	const tstring & GetParameter();
};

bool CmdLineParser_pimpl::ReadNextArg(tstring *OutArg)
{
	if (m_argv != NULL)
	{
		if (m_ArgIndex >= (size_t)m_argc) return false;

		*OutArg = m_argv[m_ArgIndex];
		m_ArgIndex++;
		return true;
	}
	else
	{
		if (m_ArgIndex >= m_CmdLineLength) return false;
		
		// Algorytm parsowania opracowany na podstawie dog³êbnych badañ zachowania wiersza poleceñ Windows
		// z ró¿nymi dziwnymi kombinacjami znaków w parametrach i sposobu ich przetwarzania na argc i argv
		// przekazywane przez system do main().
		OutArg->clear();
		bool InsideQuotes = false;
		while (m_ArgIndex < m_CmdLineLength)
		{
			tchar Ch = m_CmdLine[m_ArgIndex];
			if (Ch == _T('\\'))
			{
				// Analiza dalszego ci¹gu ³añcucha
				bool FollowedByQuote = false; // wynik analizy 1
				size_t BackslashCount = 1; // wynik analizy 2
				size_t TmpIndex = m_ArgIndex + 1;
				while (TmpIndex < m_CmdLineLength)
				{
					tchar TmpCh = m_CmdLine[TmpIndex];
					if (TmpCh == _T('\\'))
					{
						BackslashCount++;
						TmpIndex++;
					}
					else if (TmpCh == _T('"'))
					{
						FollowedByQuote = true;
						break;
					}
					else
						break;
				}

				// Stosowne do wyników analizy dalsze postêpowanie
				if (FollowedByQuote)
				{
					// Parzysta liczba '\'
					if (BackslashCount % 2 == 0)
					{
						for (size_t i = 0; i < BackslashCount / 2; i++)
							*OutArg += _T('\\');
						m_ArgIndex += BackslashCount + 1;
						InsideQuotes = !InsideQuotes;
					}
					// Nieparzysta liczba '\'
					else
					{
						for (size_t i = 0; i < BackslashCount / 2; i++)
							*OutArg += _T('\\');
						*OutArg += _T('"');
						m_ArgIndex += BackslashCount + 1;
					}
				}
				else
				{
					for (size_t i = 0; i < BackslashCount; i++)
						*OutArg += _T('\\');
					m_ArgIndex += BackslashCount;
				}
			}
			else if (Ch == _T('"'))
			{
				InsideQuotes = !InsideQuotes;
				m_ArgIndex++;
			}
			else if (CharIsWhitespace(Ch))
			{
				if (InsideQuotes)
				{
					*OutArg += Ch;
					m_ArgIndex++;
				}
				else
				{
					m_ArgIndex++;
					break;
				}
			}
			else
			{
				*OutArg += Ch;
				m_ArgIndex++;
			}
		}

		// Pomiñ dodatkowe odstêpy
		while (m_ArgIndex < m_CmdLineLength && CharIsWhitespace(m_CmdLine[m_ArgIndex]))
			m_ArgIndex++;

		return true;
	}
}

CmdLineParser_pimpl::SHORT_OPT * CmdLineParser_pimpl::FindShortOpt(tchar Opt)
{
	for (size_t i = 0; i < m_ShortOpts.size(); i++)
		if (m_ShortOpts[i].Opt == Opt)
			return &m_ShortOpts[i];
	return NULL;
}

CmdLineParser_pimpl::LONG_OPT * CmdLineParser_pimpl::FindLongOpt(const tstring &Opt)
{
	for (size_t i = 0; i < m_LongOpts.size(); i++)
		if (m_LongOpts[i].Opt == Opt)
			return &m_LongOpts[i];
	return NULL;
}

CmdLineParser_pimpl::CmdLineParser_pimpl(int argc, tchar **argv) :
	m_argv(argv),
	m_CmdLine(NULL),
	m_argc(argc),
	m_CmdLineLength(0),
	m_ArgIndex(1),
	m_InsideMultioption(false),
	m_LastArgIndex(0),
	m_LastOptId(0)
{
	assert(argc > 0);
	assert(argv != NULL);
}

CmdLineParser_pimpl::CmdLineParser_pimpl(const tchar *CmdLine) :
	m_argv(NULL),
	m_CmdLine(CmdLine),
	m_argc(0),
	m_ArgIndex(0),
	m_InsideMultioption(false),
	m_LastArgIndex(0),
	m_LastOptId(0)
{
	assert(CmdLine != NULL);

#ifdef _UNICODE
	m_CmdLineLength = _tcslen(m_CmdLine);
#else
	m_CmdLineLength = strlen(m_CmdLine);
#endif

	// Pomiñ pocz¹tkowe odstêpy
	while (m_ArgIndex < m_CmdLineLength && CharIsWhitespace(m_CmdLine[m_ArgIndex]))
		m_ArgIndex++;
}

void CmdLineParser_pimpl::RegisterOpt(uint Id, tchar Opt, bool Parameter)
{
	assert(Opt != _T('\0'));

	m_ShortOpts.push_back(SHORT_OPT(Id, Opt, Parameter));
}

void CmdLineParser_pimpl::RegisterOpt(uint Id, const tstring &Opt, bool Parameter)
{
	assert(!Opt.empty());
	
	m_LongOpts.push_back(LONG_OPT(Id, Opt, Parameter));
}

CmdLineParser::RESULT CmdLineParser_pimpl::ReadNext()
{
	if (m_InsideMultioption)
	{
		// £añcuch rozpoczynaj¹cy siê od m_LastArg[m_LastArgIndex] jest krótk¹ opcj¹ - jeden znak, dalej mog¹ byæ ró¿ne rzeczy.
		assert(m_LastArgIndex < m_LastArg.length());
		SHORT_OPT *so = FindShortOpt(m_LastArg[m_LastArgIndex]);
		// Nie ma takiej opcji - b³¹d
		if (so == NULL)
		{
			m_LastOptId = 0;
			m_LastParameter.clear();
			return CmdLineParser::RESULT_ERROR;
		}
		// Ta opcja powinna mieæ parametr
		if (so->Parameter)
		{
			// Co jest dalej?
			// Dalej nie ma nic
			if (m_LastArg.length() == m_LastArgIndex+1)
			{
				// Nastêpny argument to parameter - wczytaj go prosto tam gdzie trzeba
				if (!ReadNextArg(&m_LastParameter))
				{
					// Nie ma nastêpnego argumentu, a oczekiwany jest parametr - b³¹d
					m_LastOptId = 0;
					m_LastParameter.clear();
					return CmdLineParser::RESULT_ERROR;
				}
				m_InsideMultioption = false;
				m_LastOptId = so->Id;
				return CmdLineParser::RESULT_OPT;
			}
			// Dalej jest '='
			else if (m_LastArg[m_LastArgIndex+1] == '=')
			{
				m_InsideMultioption = false;
				m_LastParameter = m_LastArg.substr(m_LastArgIndex+2);
				m_LastOptId = so->Id;
				return CmdLineParser::RESULT_OPT;
			}
			// Dalej jest coœ innego - to bezpoœrednio parametr
			else
			{
				m_InsideMultioption = false;
				m_LastParameter = m_LastArg.substr(m_LastArgIndex+1);
				m_LastOptId = so->Id;
				return CmdLineParser::RESULT_OPT;
			}
		}
		// Ta opcja nie powinna mieæ parametru
		else
		{
			// Koniec
			if (m_LastArg.length() == m_LastArgIndex+1)
			{
				m_InsideMultioption = false;
				m_LastParameter.clear();
				m_LastOptId = so->Id;
				return CmdLineParser::RESULT_OPT;
			}
			// Nie koniec - nastêpne parametry w tej samej komendzie - dalej ten specjalny stan!
			else
			{
				// m_InsideMultioption = true; - pozostaje
				// m_LastArg pozostaje wype³niony
				m_LastArgIndex++;

				m_LastParameter.clear();
				m_LastOptId = so->Id;
				return CmdLineParser::RESULT_OPT;
			}
		}
	}
	else
	{
		// Pobierz nastêpny argument
		// U¿yjemy tu sobie m_LastArg, bo i czemu nie - dopóki m_InsideMultioption == false, i tak jest nieistotny.
		if (!ReadNextArg(&m_LastArg))
		{
			// Koniec
			m_LastParameter.clear();
			m_LastOptId = 0;
			return CmdLineParser::RESULT_END;
		}
		
		// SprawdŸ pierwszy znak
		// - Zaczyna siê od '-'
		if (!m_LastArg.empty() && m_LastArg[0] == _T('-'))
		{
			// Zaczyna siê od '--'
			if (m_LastArg.length() > 1 && m_LastArg[1] == _T('-'))
			{
				size_t EqualIndex = m_LastArg.find(_T('='), 2);
				// Jest znak '='
				if (EqualIndex != tstring::npos)
				{
					// To ma byæ d³uga opcja "--nazwa=param"
					LONG_OPT *lo = FindLongOpt(m_LastArg.substr(2, EqualIndex-2));
					// Jeœli nie ma takiej d³ugiej opcji - b³¹d
					// Ta opcja musi oczekiwaæ parametru skoro jest '=' - jeœli nie oczekuje, to b³¹d
					if (lo == NULL || lo->Parameter == false)
					{
						m_LastOptId = 0;
						m_LastParameter.clear();
						return CmdLineParser::RESULT_ERROR;
					}
					m_LastParameter = m_LastArg.substr(EqualIndex+1);
					m_LastOptId = lo->Id;
					return CmdLineParser::RESULT_OPT;
				}
				// Nie ma znaku '='
				else
				{
					// To ma byæ d³uga opcja "--nazwa" lub "--nazwa param"
					LONG_OPT *lo = FindLongOpt(m_LastArg.substr(2));
					// Jeœli nie ma takiej d³ugiej opcji - b³¹d
					if (lo == NULL)
					{
						m_LastOptId = 0;
						m_LastParameter.clear();
						return CmdLineParser::RESULT_ERROR;
					}
					// Jeœli ta opcja oczekuje parametru
					if (lo->Parameter)
					{
						// Nastêpny argument to parameter - wczytaj go prosto tam gdzie trzeba
						if (!ReadNextArg(&m_LastParameter))
						{
							// Nie ma nastêpnego argumentu, a oczekiwany jest parametr - b³¹d
							m_LastOptId = 0;
							m_LastParameter.clear();
							return CmdLineParser::RESULT_ERROR;
						}
					}
					// Nie oczekuje parametru
					else
						m_LastParameter.clear();
					// Koniec - sukces
					m_LastOptId = lo->Id;
					return CmdLineParser::RESULT_OPT;
				}
			}
			// Zaczyna siê od samego '-'
			else
			{
				// Krótka opcja
				// To jest opcja jednoznakowa. Dalej mog¹ byæ ró¿ne rzeczy
				// Samo '-' - b³¹d
				if (m_LastArg.length() < 2)
				{
					m_LastOptId = 0;
					m_LastParameter.clear();
					return CmdLineParser::RESULT_ERROR;
				}
				SHORT_OPT *so = FindShortOpt(m_LastArg[1]);
				// Nie ma takiej opcji - b³¹d
				if (so == NULL)
				{
					m_LastOptId = 0;
					m_LastParameter.clear();
					return CmdLineParser::RESULT_ERROR;
				}
				// Ta opcja powinna mieæ parametr
				if (so->Parameter)
				{
					// Co jest dalej?
					// Dalej nie ma nic
					if (m_LastArg.length() == 2)
					{
						// Nastêpny argument to parameter - wczytaj go prosto tam gdzie trzeba
						if (!ReadNextArg(&m_LastParameter))
						{
							// Nie ma nastêpnego argumentu, a oczekiwany jest parametr - b³¹d
							m_LastOptId = 0;
							m_LastParameter.clear();
							return CmdLineParser::RESULT_ERROR;
						}
						m_LastOptId = so->Id;
						return CmdLineParser::RESULT_OPT;
					}
					// Dalej jest '='
					else if (m_LastArg[2] == _T('='))
					{
						m_LastParameter = m_LastArg.substr(3);
						m_LastOptId = so->Id;
						return CmdLineParser::RESULT_OPT;
					}
					// Dalej jest coœ innego - to bezpoœrednio parametr
					else
					{
						m_LastParameter = m_LastArg.substr(2);
						m_LastOptId = so->Id;
						return CmdLineParser::RESULT_OPT;
					}
				}
				// Ta opcja nie powinna mieæ parametru
				else
				{
					// Koniec
					if (m_LastArg.length() == 2)
					{
						m_LastParameter.clear();
						m_LastOptId = so->Id;
						return CmdLineParser::RESULT_OPT;
					}
					// Nie koniec - nastêpne parametry w tej samej komendzie - specjalny stan!
					else
					{
						m_InsideMultioption = true;
						// m_LastArg ju¿ wype³niony
						m_LastArgIndex = 2;

						m_LastParameter.clear();
						m_LastOptId = so->Id;
						return CmdLineParser::RESULT_OPT;
					}
				}
			}
		}
		// - Zaczyna siê od '/'
		else if (!m_LastArg.empty() && m_LastArg[0] == _T('/'))
		{
			size_t EqualIndex = m_LastArg.find(_T('='), 1);
			// Jest znak '='
			if (EqualIndex != tstring::npos)
			{
				// To ma byæ krótka lub d³uga opcja "/nazwa=param"
				// To mo¿e byæ krótka
				if (EqualIndex == 2)
				{
					SHORT_OPT *so = FindShortOpt(m_LastArg[1]);
					// Znaleziono krótk¹ opcjê
					if (so != NULL)
					{
						// Ta opcja musi oczekiwaæ parametru skoro jest '=' - jeœli nie oczekuje, to b³¹d
						if (so->Parameter == false)	
						{
							m_LastOptId = 0;
							m_LastParameter.clear();
							return CmdLineParser::RESULT_ERROR;
						}
						m_LastParameter = m_LastArg.substr(EqualIndex+1);
						m_LastOptId = so->Id;
						return CmdLineParser::RESULT_OPT;
					}
				}
				// To nie jest krótka opcja bo nazwa za d³uga lub bo nie znaleziono takiej krótkiej - to musi byæ d³uga opcja
				LONG_OPT *lo = FindLongOpt(m_LastArg.substr(1, EqualIndex-1));
				// Jeœli nie ma takiej d³ugiej opcji - b³¹d
				// Ta opcja musi oczekiwaæ parametru skoro jest '=' - jeœli nie oczekuje, to b³¹d
				if (lo == NULL || lo->Parameter == false)
				{
					m_LastOptId = 0;
					m_LastParameter.clear();
					return CmdLineParser::RESULT_ERROR;
				}
				m_LastParameter = m_LastArg.substr(EqualIndex+1);
				m_LastOptId = lo->Id;
				return CmdLineParser::RESULT_OPT;
			}
			// Nie ma znaku '='
			else
			{
				// To ma byæ krótka lub d³uga opcja "/nazwa" lub "/nazwa param"
				// To mo¿e byæ krótka opcja
				if (m_LastArg.length() == 2)
				{
					SHORT_OPT *so = FindShortOpt(m_LastArg[1]);
					// Znaleziono krótk¹ opcjê
					if (so != NULL)
					{
						// Jeœli ta opcja oczekuje parametru
						if (so->Parameter)
						{
							// Nastêpny argument to parameter - wczytaj go prosto tam gdzie trzeba
							if (!ReadNextArg(&m_LastParameter))
							{
								// Nie ma nastêpnego argumentu, a oczekiwany jest parametr - b³¹d
								m_LastOptId = 0;
								m_LastParameter.clear();
								return CmdLineParser::RESULT_ERROR;
							}
						}
						// Nie ma parametru
						else
							m_LastParameter.clear();
						// Koniec - sukces
						m_LastOptId = so->Id;
						return CmdLineParser::RESULT_OPT;
					}
				}
				// To nie jest krótka opcja bo nazwa za d³uga lub bo nie znaleziono takiej krótkiej - to musi byæ d³uga opcja
				LONG_OPT *lo = FindLongOpt(m_LastArg.substr(1));
				// Jeœli nie ma takiej d³ugiej opcji - b³¹d
				if (lo == NULL)
				{
					m_LastOptId = 0;
					m_LastParameter.clear();
					return CmdLineParser::RESULT_ERROR;
				}
				// Jeœli ta opcja oczekuje parametru
				if (lo->Parameter)
				{
					// Nastêpny argument to parameter - wczytaj go prosto tam gdzie trzeba
					if (!ReadNextArg(&m_LastParameter))
					{
						// Nie ma nastêpnego argumentu, a oczekiwany jest parametr - b³¹d
						m_LastOptId = 0;
						m_LastParameter.clear();
						return CmdLineParser::RESULT_ERROR;
					}
				}
				// Nie ma parametru
				else
					m_LastParameter.clear();
				// Koniec - sukces
				m_LastOptId = lo->Id;
				return CmdLineParser::RESULT_OPT;
			}
		}
		// - Zaczyna siê od innego znaku lub jest pusty
		else
		{
			// To jest go³y parametr bez opcji
			m_LastOptId = 0;
			m_LastParameter = m_LastArg;
			return CmdLineParser::RESULT_PARAMETER;
		}
	}
}

uint CmdLineParser_pimpl::GetOptId()
{
	return m_LastOptId;
}

const tstring & CmdLineParser_pimpl::GetParameter()
{
	return m_LastParameter;
}

CmdLineParser::CmdLineParser(int argc, tchar **argv) :
	pimpl(new CmdLineParser_pimpl(argc, argv))
{
}

CmdLineParser::CmdLineParser(const tchar *CmdLine) :
	pimpl(new CmdLineParser_pimpl(CmdLine))
{
}

CmdLineParser::~CmdLineParser()
{
	pimpl.reset();
}

void CmdLineParser::RegisterOpt(uint Id, tchar Opt, bool Parameter)
{
	pimpl->RegisterOpt(Id, Opt, Parameter);
}

void CmdLineParser::RegisterOpt(uint Id, const tstring &Opt, bool Parameter)
{
	pimpl->RegisterOpt(Id, Opt, Parameter);
}

CmdLineParser::RESULT CmdLineParser::ReadNext()
{
	return pimpl->ReadNext();
}

uint CmdLineParser::GetOptId()
{
	return pimpl->GetOptId();
}

const tstring & CmdLineParser::GetParameter()
{
	return pimpl->GetParameter();
}

const CommonGUID CommonGUID::NIL = CommonGUID(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

int CommonGUID::Cmp(const CommonGUID &v1, const CommonGUID &v2)
{
	int v;
	for (uint i = 16; i--; )
	{
		v = (int)v1.Data[i] - (int)v2.Data[i];
		if (v != 0) return v;
	}
	return 0;
}

CommonGUID::CommonGUID(uint8 v0, uint8 v1, uint8 v2, uint8 v3, uint8 v4, uint8 v5, uint8 v6, uint8 v7, uint8 v8, uint8 v9, uint8 v10, uint8 v11, uint8 v12, uint8 v13, uint8 v14, uint8 v15)
{
	Data[ 0] = v0;  Data[ 1] = v1;  Data[ 2] =  v2; Data[ 3] = v3;
	Data[ 4] = v4;  Data[ 5] = v5;  Data[ 6] =  v6; Data[ 7] = v7;
	Data[ 8] = v8;  Data[ 9] = v9;  Data[10] = v10; Data[11] = v11;
	Data[12] = v12; Data[13] = v13; Data[14] = v14; Data[15] = v15;
}

void CommonGUID::ToString(tchar *out) const
{
#ifdef _UNICODE
	swprintf_s(out, 37, L"%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X",
		(int)Data[15], (int)Data[14], (int)Data[13], (int)Data[12],
		(int)Data[11], (int)Data[10], (int)Data[ 9], (int)Data[ 8],
		(int)Data[ 7], (int)Data[ 6], (int)Data[ 5], (int)Data[ 4],
		(int)Data[ 3], (int)Data[ 2], (int)Data[ 1], (int)Data[ 0] );
#elif defined(_MSC_VER)
	sprintf_s(out, 37, "%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X",
		(int)Data[15], (int)Data[14], (int)Data[13], (int)Data[12],
		(int)Data[11], (int)Data[10], (int)Data[ 9], (int)Data[ 8],
		(int)Data[ 7], (int)Data[ 6], (int)Data[ 5], (int)Data[ 4],
		(int)Data[ 3], (int)Data[ 2], (int)Data[ 1], (int)Data[ 0] );
#else
	sprintf(out, "%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X",
		(int)Data[15], (int)Data[14], (int)Data[13], (int)Data[12],
		(int)Data[11], (int)Data[10], (int)Data[ 9], (int)Data[ 8],
		(int)Data[ 7], (int)Data[ 6], (int)Data[ 5], (int)Data[ 4],
		(int)Data[ 3], (int)Data[ 2], (int)Data[ 1], (int)Data[ 0] );
#endif
}

void CommonGUID::ToString(tstring &out) const
{
	tchar buf[37];
	ToString(buf);
	out = buf;
}

void CommonGUID::GenerateVersion4()
{
	for (uint i = 0; i < 16; i++)
		Data[i] = g_Rand.RandByte();

	Data[7] = Data[7] & 0x3F | 0x80;
	Data[9] = Data[9] & 0x0F | 0x40;
}

bool CommonGUID::FromString(const tchar *s)
{
	int ints[16];
#ifdef _UNICODE
	int R = swscanf_s(s, L"%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X",
		&ints[15], &ints[14], &ints[13], &ints[12],
		&ints[11], &ints[10], &ints[ 9], &ints[ 8],
		&ints[ 7], &ints[ 6], &ints[ 5], &ints[ 4],
		&ints[ 3], &ints[ 2], &ints[ 1], &ints[ 0] );
#elif defined(_MSC_VER)
	int R = sscanf_s(s, "%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X",
		&ints[15], &ints[14], &ints[13], &ints[12],
		&ints[11], &ints[10], &ints[ 9], &ints[ 8],
		&ints[ 7], &ints[ 6], &ints[ 5], &ints[ 4],
		&ints[ 3], &ints[ 2], &ints[ 1], &ints[ 0] );
#else
	int R = sscanf(s, "%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X",
		&ints[15], &ints[14], &ints[13], &ints[12],
		&ints[11], &ints[10], &ints[ 9], &ints[ 8],
		&ints[ 7], &ints[ 6], &ints[ 5], &ints[ 4],
		&ints[ 3], &ints[ 2], &ints[ 1], &ints[ 0] );
#endif
	if (R != 16) return false;
	for (uint i = 0; i < 16; i++)
		Data[i] = (uint8)ints[i];
	return true;
}

bool CommonGUID::FromString(const tstring &s)
{
	return FromString(s.c_str());
}

Format::Format(const Format &f, const tstring &Element) :
	pimpl(f.pimpl)
{
	size_t index = pimpl->m_String.find(pimpl->m_Sep, pimpl->m_Index);

	// nie ma ju¿ gdzie dodaæ - nie zmieniamy
	if (index == tstring::npos) return;

	pimpl->m_String.erase(index, 1);
	pimpl->m_String.insert(index, Element);
	pimpl->m_Index = index + Element.length();
}

Format::Format(const Format &f, const tchar *Element) :
	pimpl(f.pimpl)
{
	size_t index = pimpl->m_String.find(pimpl->m_Sep, pimpl->m_Index);

	// nie ma ju¿ gdzie dodaæ - nie zmieniamy
	if (index == tstring::npos) return;

	pimpl->m_String.erase(index, 1);
	pimpl->m_String.insert(index, Element);
#ifdef _UNICODE
	pimpl->m_Index = index + _tcslen(Element);
#else
	pimpl->m_Index = index + strlen(Element);
#endif
}

} // namespace common
