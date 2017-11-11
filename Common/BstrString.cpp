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

    assert(bufLen < (size_t)UINT_MAX);
	Bstr = SysAllocStringLen(NULL, (UINT)bufLen);
	ConvertCharsToUnicode(Bstr, bufLen, str, strLen, CP_ACP);
}

BstrString::BstrString(const char *str, size_t strLen)
: TmpStr(NULL)
{
	if (str == NULL) { Bstr = NULL; return; }

	size_t bufLen;
	bool ok = CharsToUnicode_CalcBufLen(bufLen, str, strLen, CP_ACP);
	if (!ok) { Bstr = NULL; return; }

    assert(bufLen < (size_t)UINT_MAX);
	Bstr = SysAllocStringLen(NULL, (UINT)bufLen);
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
    {
        assert(wcsLen < (size_t)UINT_MAX);
		Bstr = SysAllocStringLen(wcs, (UINT)wcsLen);
    }
}

BstrString::BstrString(const std::string &s)
: TmpStr(NULL)
{
	size_t strLen = s.length(), bufLen;
	bool ok = CharsToUnicode_CalcBufLen(bufLen, s.data(), strLen, CP_ACP);
	if (!ok) { Bstr = NULL; return; }

    assert(bufLen < (size_t)UINT_MAX);
	Bstr = SysAllocStringLen(NULL, (UINT)bufLen);
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

BstrString::BstrString(UINT length)
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
        {
            INT r = SysReAllocString(&Bstr, bstr.Bstr);
			assert(r == TRUE);
        }
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
            assert(bufLen < (size_t)UINT_MAX);
			Bstr = SysAllocStringLen(NULL, (UINT)bufLen);
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
        {
            INT r = SysReAllocString(&Bstr, wcs);
			assert(r == TRUE);
        }
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
        assert(wcsLen < (size_t)UINT_MAX);
		if (Bstr == NULL)
			Bstr = SysAllocStringLen(wcs, (UINT)wcsLen);
		else
        {
            INT r = SysReAllocStringLen(&Bstr, wcs, (UINT)wcsLen);
			assert(r == TRUE);
        }
	}
}

void BstrString::assign(const std::string &s)
{
	assign(s.c_str(), s.length());
}

void BstrString::assign(const std::wstring &s)
{
	assign(s.c_str(), s.length());
}

void BstrString::assignBstr(BSTR bstr)
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
		UINT myLength = length(), bstrLength = bstr.length();
		UINT newLength = myLength + bstrLength;
		BSTR newBstr = SysAllocStringLen(Bstr, newLength);
		common_memcpy(newBstr + (size_t)myLength, bstr.Bstr, (size_t)bstrLength * sizeof(OLECHAR));
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
	size_t oldLen = (size_t)length();
    size_t newLen = oldLen + appLen;
    assert(newLen <= (size_t)UINT_MAX);
	resize((UINT)newLen);
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
		UINT myLen = length();
        size_t wcsLen = wcslen(wcs);
        assert(wcsLen + (size_t)myLen <= (size_t)UINT_MAX);
       	UINT newLength = myLen + (UINT)wcsLen;
		BSTR newBstr = SysAllocStringLen(Bstr, newLength);
		common_memcpy(newBstr + (size_t)myLen, wcs, wcsLen * sizeof(OLECHAR));
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
		UINT myLen = length();
        assert((size_t)myLen + wcsLen <= (size_t)UINT_MAX);
		UINT newLength = myLen + (UINT)wcsLen;
		BSTR newBstr = SysAllocStringLen(Bstr, newLength);
		common_memcpy(newBstr + (size_t)myLen, wcs, wcsLen * sizeof(OLECHAR));
		Bstr = newBstr;
	}
}

void BstrString::append(const std::string &s)
{
	append(s.c_str(), s.length());
}

void BstrString::append(const std::wstring &s)
{
	append(s.c_str(), s.length());
}

BstrString BstrString::operator + (const BstrString &bstr) const
{
	if (empty())
		return BstrString(bstr);
	else if (bstr.empty())
		return BstrString(*this);
	else
	{
		UINT myLen = length(), bstrLen = bstr.length();
		UINT newLen = myLen + bstrLen;
		BSTR newBstr = SysAllocStringLen(Bstr, newLen);
		common_memcpy(newBstr + (size_t)myLen, bstr.Bstr, (size_t)bstrLen * sizeof(OLECHAR));
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
		size_t myLen = (size_t)length();
		bool ok = ConvertUnicodeToChars(outBuf, bufLen, Bstr, myLen, CP_ACP);
		if (bufLen > myLen)
			outBuf[myLen] = '\0';
		return ok;
	}
}

UINT BstrString::length() const
{
	return SysStringLen(Bstr); // Mo¿na jej podaæ NULL.
}

void BstrString::resize(UINT newLength)
{
	if (newLength == length()) return;

	BSTR newBstr = SysAllocStringLen(NULL, newLength);
	size_t lengthToCopy = (size_t)std::min(newLength, length());
	if (lengthToCopy > 0)
		common_memcpy(newBstr, Bstr, lengthToCopy * sizeof(OLECHAR));
	SysFreeString(Bstr);
	Bstr = newBstr;
}

BstrString BstrString::substr(UINT off, UINT count) const
{
	count = std::min(count, length() - off);

	BSTR newBstr = SysAllocStringLen(Bstr + (size_t)off, count);
	return BstrString(newBstr);
}


} // namespace common
