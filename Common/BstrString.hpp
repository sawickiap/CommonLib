/** \file
\brief Object-oriented wrapper for Unicode string of type BSTR.
\author Adam Sawicki - sawickiap@poczta.onet.pl - http://asawicki.info/ \n

Part of CommonLib library. \n
Encoding UTF-8, end of line CR+LF \n
License: GNU LGPL. \n
Documentation: \ref Module_BstrString \n
Module components: \ref code_bstrstring
*/
#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif
#ifndef BSTR_STRING_H_
#define BSTR_STRING_H_

#ifndef _WIN32
#error BstrString module supports only _WIN32 platform.
#endif

#include <windows.h> // Must be before Oleauto.h or I get compilation errors.
#include <Oleauto.h>

namespace common
{

/** \addtogroup code_bstrstring BstrString Module
Documentation: \ref Module_BstrString \n
Header: BstrString.hpp */
//@{

/// Wrapper for Unicode string of type BSTR used in Windows OLE Automation.
/** Automatically manages and frees memory of the string. \n
Interface is similar to std::string. \n
Also supports automatic conversion from and to ANSI string char* and std::string. */
class BstrString
{
public:
	/// Returns number less, greater or equal to 0 according to comparison result.
	static int Cmp(const BstrString &bstr1, const BstrString &bstr2, bool ignoreCase = false);

	BstrString();
	BstrString(const BstrString &bstr);
	BstrString(const char *str);
	BstrString(const char *str, size_t strLen);
	BstrString(const wchar_t *wcs);
	BstrString(const wchar_t *wcs, size_t wcsLen);
	BstrString(const std::string &s);
	BstrString(const std::wstring &s);
	BstrString(BSTR bstr); ///< Takes ownership of existing BSTR.
	BstrString(UINT length); ///< Allocates, but doesn't initialize.
	~BstrString();

	void assign(const BstrString &bstr);
	void assign(const char *str);
	void assign(const char *str, size_t strLen);
	void assign(const wchar_t *wcs);
	void assign(const wchar_t *wcs, size_t wcsLen);
	void assign(const std::string &s);
	void assign(const std::wstring &s);
	void assign(BSTR bstr); ///< Takes ownership of existing BSTR.

	BstrString & operator = (const BstrString &bstr) { assign(bstr); return *this; }
	BstrString & operator = (const char *str)        { assign(str);  return *this; }
	BstrString & operator = (const wchar_t *wcs)     { assign(wcs);  return *this; }
	BstrString & operator = (BSTR bstr)              { assign(bstr); return *this; } ///< Takes ownership of existing BSTR.
	BstrString & operator = (const std::string &s)   { assign(s);    return *this; }
	BstrString & operator = (const std::wstring &s)  { assign(s);    return *this; }

	bool operator == (const BstrString &bstr) const { return Cmp(*this, bstr) == 0; }
	bool operator != (const BstrString &bstr) const { return Cmp(*this, bstr) != 0; }
	bool operator <  (const BstrString &bstr) const { return Cmp(*this, bstr) <  0; }
	bool operator >  (const BstrString &bstr) const { return Cmp(*this, bstr) >  0; }
	bool operator <= (const BstrString &bstr) const { return Cmp(*this, bstr) <= 0; }
	bool operator >= (const BstrString &bstr) const { return Cmp(*this, bstr) >= 0; }

	wchar_t       & operator [] (size_t index)       { return ( (wchar_t*)      Bstr )[index]; }
	const wchar_t & operator [] (size_t index) const { return ( (const wchar_t*)Bstr )[index]; }

	void append(const BstrString &bstr);
	void append(const char *str);
	void append(const char *str, size_t strLen);
	void append(const wchar_t *wcs);
	void append(const wchar_t *wcs, size_t wcsLen);
	void append(const std::string &s);
	void append(const std::wstring &s);
	void append(BSTR bstr);

	BstrString & operator += (const BstrString &bstr) { append(bstr); return *this; }
	BstrString & operator += (const char *str)        { append(str);  return *this; }
	BstrString & operator += (const wchar_t *wcs)     { append(wcs);  return *this; }
	BstrString & operator += (const std::string &s)   { append(s);    return *this; }
	BstrString & operator += (const std::wstring &s)  { append(s);    return *this; }
	BstrString & operator += (BSTR bstr)              { append(bstr); return *this; }

	BstrString operator + (const BstrString &bstr) const;

	void clear();

	const char *c_str() const;
	const wchar_t *wc_str() const { return (const wchar_t*)Bstr; }
	BSTR data() { return Bstr; }
	BSTR b_str() const { return Bstr; }

	/// Calculate buffer size necessary to convert the string to ANSI string. Result is without trailing zero.
	size_t CalcStrLen() const;
	/// Converts the string to ANSI string. Appends trailing zero if there is enought room for it.
	bool ConvertToStr(char *outBuf, size_t bufLen) const;

	UINT length() const;
	UINT size() const { return length(); }
	bool empty() const { return length() == 0; }
	void resize(UINT newLength);

	BstrString substr(UINT off = 0, UINT count = UINT_MAX) const;

private:
	BSTR Bstr;
	// May be NULL or may contain outdated string.
	// Updated by c_str.
	mutable char *TmpStr;
};


//@}
// code_bstrstring

} // namespace common

#endif
