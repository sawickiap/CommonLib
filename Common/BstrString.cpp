/** \file
\brief Object-oriented wrapper for Unicode string of type BSTR.
\author Adam Sawicki - sawickiap@poczta.onet.pl - http://asawicki.info/ \n

Part of CommonLib library. \n
Encoding UTF-8, end of line CR+LF \n
License: GNU LGPL. \n
Documentation: \ref Module_BstrString \n
Module components: \ref code_bstrstring
*/
#include "Base.hpp"
#include "BstrString.hpp"

namespace common
{

int BstrString::Cmp(const BstrString &bstr1, const BstrString &bstr2, bool ignoreCase)
{
	HRESULT hr = VarBstrCmp(bstr1.Bstr, bstr2.Bstr, LOCALE_INVARIANT, ignoreCase ? NORM_IGNORECASE : 0);
	switch (hr)
	{
	case VARCMP_LT: return -1;
	case VARCMP_GT: return  1;
	default:        return  0; // case VARCMP_EQ
	}
}

BstrString::BstrString()
: Bstr(NULL)
, TmpStr(NULL)
{
}

BstrString::BstrString(const BstrString &bstr)
: TmpStr(NULL)
{
	if (bstr.length() == 0)
		Bstr = NULL;
	else
		Bstr = SysAllocString(bstr.Bstr);
}

BstrString::BstrString(const char *str)
: TmpStr(NULL)
{
	if (str == NULL) { Bstr = NULL; return; }

	size_t strLen = strlen(str), bufLen;
	bool ok = CharsToUnicode_CalcBufLen(bufLen, str, strLen, CP_ACP);
	if (!ok) { Bstr = NULL; return; }

	Bstr = SysAllocStringLen(NULL, bufLen);
	ConvertCharsToUnicode(Bstr, bufLen, str, strLen, CP_ACP);
}

BstrString::BstrString(const char *str, size_t strLen)
: TmpStr(NULL)
{
	if (str == NULL) { Bstr = NULL; return; }

	size_t bufLen;
	bool ok = CharsToUnicode_CalcBufLen(bufLen, str, strLen, CP_ACP);
	if (!ok) { Bstr = NULL; return; }

	Bstr = SysAllocStringLen(NULL, bufLen);
	ConvertCharsToUnicode(Bstr, bufLen, str, strLen, CP_ACP);
}

BstrString::BstrString(const wchar_t *wcs)
: TmpStr(NULL)
{
	if (wcs == NULL)
		Bstr = NULL;
	else
		Bstr = SysAllocString(wcs);
}

BstrString::BstrString(const wchar_t *wcs, size_t wcsLen)
: TmpStr(NULL)
{
	if (wcs == NULL)
		Bstr = NULL;
	else
		Bstr = SysAllocStringLen(wcs, wcsLen);
}

BstrString::BstrString(const std::string &s)
: TmpStr(NULL)
{
	size_t strLen = s.length(), bufLen;
	bool ok = CharsToUnicode_CalcBufLen(bufLen, s.data(), strLen, CP_ACP);
	if (!ok) { Bstr = NULL; return; }

	Bstr = SysAllocStringLen(NULL, bufLen);
	ConvertCharsToUnicode(Bstr, bufLen, s.data(), strLen, CP_ACP);
}

BstrString::BstrString(const std::wstring &s)
: TmpStr(NULL)
{
	Bstr = SysAllocString(s.c_str());
}

BstrString::BstrString(BSTR bstr)
: Bstr(bstr)
, TmpStr(NULL)
{
}

BstrString::BstrString(size_t length)
: TmpStr(NULL)
{
	Bstr = SysAllocStringLen(NULL, length);
}

BstrString::~BstrString()
{
	delete [] TmpStr;
	SysFreeString(Bstr); // Mo¿na jej podaæ NULL.
}

void BstrString::assign(const BstrString &bstr)
{
	if (&bstr == this) return;

	if (bstr.Bstr == NULL)
	{
		SysFreeString(Bstr);
		Bstr = NULL;
	}
	else
	{
		if (Bstr == NULL)
			Bstr = SysAllocString(bstr.Bstr);
		else
			assert( SysReAllocString(&Bstr, bstr.Bstr) == TRUE );
	}
}

void BstrString::assign(const char *str)
{
	if (str == NULL)
		clear();
	else
	{
		size_t strLen = strlen(str);
		assign(str, strLen);
	}
}

void BstrString::assign(const char *str, size_t strLen)
{
	if (strLen == 0 || str == NULL)
		clear();
	else
	{
		SysFreeString(Bstr);

		size_t bufLen;
		bool ok = CharsToUnicode_CalcBufLen(bufLen, str, strLen, CP_ACP);
		if (ok)
		{
			Bstr = SysAllocStringLen(NULL, bufLen);
			ConvertCharsToUnicode(Bstr, bufLen, str, strLen, CP_ACP);
		}
		else
			Bstr = NULL;
	}
}

void BstrString::assign(const wchar_t *wcs)
{
	if (wcs == NULL)
	{
		SysFreeString(Bstr);
		Bstr = NULL;
	}
	else
	{
		if (Bstr == NULL)
			Bstr = SysAllocString(wcs);
		else
			assert( SysReAllocString(&Bstr, wcs) == TRUE );
	}
}

void BstrString::assign(const wchar_t *wcs, size_t wcsLen)
{
	if (wcs == NULL)
	{
		SysFreeString(Bstr);
		Bstr = NULL;
	}
	else
	{
		if (Bstr == NULL)
			Bstr = SysAllocStringLen(wcs, wcsLen);
		else
			assert( SysReAllocStringLen(&Bstr, wcs, wcsLen) == TRUE );
	}
}

void BstrString::assign(const std::string &s)
{
	assign(s.c_str());
}

void BstrString::assign(const std::wstring &s)
{
	assign(s.c_str());
}

void BstrString::assign(BSTR bstr)
{
	if (bstr == this->Bstr) return;

	SysFreeString(Bstr);
	this->Bstr = bstr;
}

void BstrString::append(const BstrString &bstr)
{
	if (bstr.empty())
		return;
	else if (empty())
		assign(bstr);
	else
	{
		size_t myLength = length(), bstrLength = bstr.length();
		size_t newLength = myLength + bstrLength;
		BSTR newBstr = SysAllocStringLen(Bstr, newLength);
		common_memcpy(newBstr + myLength, bstr.Bstr, bstrLength * sizeof(OLECHAR));
		Bstr = newBstr;
	}
}

void BstrString::append(const char *str)
{
	if (StrIsEmpty(str)) return;
	size_t strLen = strlen(str);
	append(str, strLen);
}

void BstrString::append(const char *str, size_t strLen)
{
	if (strLen == 0 || StrIsEmpty(str)) return;

	size_t appLen;
	bool ok = CharsToUnicode_CalcBufLen(appLen, str, strLen, CP_ACP);
	if (!ok) return;
	size_t oldLen = length();
	resize(oldLen + appLen);
	ConvertCharsToUnicode(Bstr + oldLen, appLen, str, strLen, CP_ACP);
}

void BstrString::append(const wchar_t *wcs)
{
	if (StrIsEmpty(wcs))
		return;
	else if (empty())
		assign(wcs);
	else
	{
		size_t myLen = length(), wcsLen = wcslen(wcs);
		size_t newLength = myLen + wcsLen;
		BSTR newBstr = SysAllocStringLen(Bstr, newLength);
		common_memcpy(newBstr + myLen, wcs, wcsLen * sizeof(OLECHAR));
		Bstr = newBstr;
	}
}

void BstrString::append(const wchar_t *wcs, size_t wcsLen)
{
	if (wcsLen == 0 || StrIsEmpty(wcs))
		return;
	else if (empty())
		assign(wcs);
	else
	{
		size_t myLen = length();
		size_t newLength = myLen + wcsLen;
		BSTR newBstr = SysAllocStringLen(Bstr, newLength);
		common_memcpy(newBstr + myLen, wcs, wcsLen * sizeof(OLECHAR));
		Bstr = newBstr;
	}
}

void BstrString::append(const std::string &s)
{
	append(s.c_str());
}

void BstrString::append(const std::wstring &s)
{
	append(s.c_str());
}

void BstrString::append(BSTR bstr)
{
	size_t bstrLen = (bstr == NULL) ? 0 : SysStringLen(bstr);
	if (bstrLen == 0)
		return;
	else if (empty())
	{
		SysFreeString(Bstr);
		Bstr = SysAllocStringLen(bstr, bstrLen);
	}
	else
	{
		size_t myLen = length();
		size_t newLength = myLen + bstrLen;
		BSTR newBstr = SysAllocStringLen(Bstr, newLength);
		common_memcpy(newBstr + myLen, bstr, bstrLen * sizeof(OLECHAR));
		Bstr = newBstr;
	}
}

BstrString BstrString::operator + (const BstrString &bstr) const
{
	if (empty())
		return BstrString(bstr);
	else if (bstr.empty())
		return BstrString(*this);
	else
	{
		size_t myLen = length(), bstrLen = bstr.length();
		size_t newLen = myLen + bstrLen;
		BSTR newBstr = SysAllocStringLen(Bstr, newLen);
		common_memcpy(newBstr + myLen, bstr.Bstr, bstrLen * sizeof(OLECHAR));
		return BstrString(newBstr);
	}
}

void BstrString::clear()
{
	// Tylko dla optymalizacji.
	if (TmpStr)
	{
		delete [] TmpStr;
		TmpStr = NULL;
	}

	SysFreeString(Bstr); // Mo¿na jej podaæ NULL.
	Bstr = NULL;
}

const char * BstrString::c_str() const
{
	delete [] TmpStr;
	TmpStr = NULL;
	if (empty())
	{
		TmpStr = new char[1];
		TmpStr[0] = '\0';
	}
	else
	{
		size_t strLen = CalcStrLen() + 1;
		TmpStr = new char[strLen];
		ConvertToStr(TmpStr, strLen);
	}
	return TmpStr;
}

size_t BstrString::CalcStrLen() const
{
	if (empty()) return 0;
	size_t len;
	bool ok = UnicodeToChars_CalcBufLen(len, Bstr, CP_ACP);
	return ok ? len : 0;
}

bool BstrString::ConvertToStr(char *outBuf, size_t bufLen) const
{
	if (bufLen == 0 || outBuf == NULL)
		return false;
	if (empty())
	{
		ClearStr(outBuf);
		return true;
	}
	else
	{
		size_t myLen = length();
		bool ok = ConvertUnicodeToChars(outBuf, bufLen, Bstr, myLen, CP_ACP);
		if (bufLen > myLen)
			outBuf[myLen] = '\0';
		return ok;
	}
}

size_t BstrString::length() const
{
	return SysStringLen(Bstr); // Mo¿na jej podaæ NULL.
}

void BstrString::resize(size_t newLength)
{
	if (newLength == length()) return;

	BSTR newBstr = SysAllocStringLen(NULL, newLength);
	size_t lengthToCopy = std::min(newLength, length());
	if (lengthToCopy > 0)
		common_memcpy(newBstr, Bstr, lengthToCopy * sizeof(OLECHAR));
	SysFreeString(Bstr);
	Bstr = newBstr;
}

BstrString BstrString::substr(size_t off, size_t count) const
{
	count = std::min(count, length() - off);

	BSTR newBstr = SysAllocStringLen(Bstr + off, count);
	return BstrString(newBstr);
}


} // namespace common
