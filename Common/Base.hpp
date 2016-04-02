/** \file
\brief Foundation module
\author Adam Sawicki - sawickiap@poczta.onet.pl - http://regedit.gamedev.pl/ \n

CzÍúÊ biblioteki CommonLib \n
Kodowanie Windows-1250, koniec wiersza CR+LF, test: ZaøÛ≥Ê gÍúlπ jaüÒ \n
Licencja: GNU LGPL. \n
Dokumentacja: \ref Module_Base \n
Elementy modu≥u: \ref code_base
*/
#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif
#ifndef COMMON_BASE_H_
#define COMMON_BASE_H_

/** \addtogroup code_base Base Module
Dokumentacja: \ref Module_Base \n
Nag≥Ûwek: Base.hpp */
//@{

// G≥Ûwne includy
#include <cassert>
#include <string>
#include <vector>

// Niechciane includy :(
#include <algorithm>
#include <limits>
#include <cmath>

#ifdef WIN32
	/// To jest na wypadek w≥πczania gdzieú niøej przez uøytkownika <windows.h>
	#define NOMINMAX
#endif

// String chcÍ mieÊ jak typ wbudowany
using std::string;
using std::wstring;

// Niezaleønie od systemu, sygnatura bieøπcej funkcji bÍdzie w makrze __FUNCSIG__ - konwersja z konwencji GCC na Visual C++.
/* Makro __FUNCTION__ z samπ nazwπ funkcji jest takie samo w Visual C++ jak i w GCC, wiÍc go nie ruszam. */
#ifndef WIN32
	#define __FUNCSIG__ __PRETTY_FUNCTION__
#endif

// Sprawy zwiπzane z Unicode
#ifdef WIN32
	#include <tchar.h>
	#include <wchar.h>
	/// Typ znakowy, zaleønie od makra _UNICODE zdefiniowany jako char lub wchar_t.
	typedef TCHAR tchar;
#else
	#define _T(x) (x)
	typedef char tchar;
#endif

#ifdef _UNICODE
	/// Typ ≥aÒcuchowy, zaleønie od makra _UNICODE zdefiniowany jako std::string lub std::wstring.
	typedef std::wstring tstring;
	/// Konwertuje znak na liczbÍ naturalnπ.
	#define TCHAR_TO_INT(c) ( (int)(c) )
	/// Konwertuje liczbÍ 0..255 na znak.
	#define INT_TO_TCHAR(i) ( (wchar_t)(i) )
#else
	typedef std::string tstring;
	#define TCHAR_TO_INT(c) ( (int)(uint8)(c) )
	#define INT_TO_TCHAR(i) ( (char)(uint8)(i) )
#endif

/// Nazwa bieøπcego pliku ürÛd≥owego jako ≥aÒcuch, zaleønie od makra _UNICODE jest w Unikodzie
#define __TFILE__ _T(__FILE__)
/// Pe≥na sygnatura bieøπcej funkcji w kodzie, zaleønie od makra _UNICODE jest w Unikodzie
#define __TFUNCSIG__ _T(__FUNCSIG__)
/// Sama nazwa bieøπcej funkcji w kodzie, zaleønie od makra _UNICODE jest w Unikodzie
#define __TFUNCTION__ _T(__FUNCTION__)

/** \addtogroup base_main_types Postawowe typy danych */
//@{
#ifdef WIN32
	/// Liczba 32-bitowa bez znaku
	typedef unsigned __int32 uint;
	/// Liczba 8-bitowa ze znakiem
	typedef __int8 int8;
	/// Liczba 8-bitowa bez znaku
	typedef unsigned __int8 uint8;
	/// Liczba 16-bitowa ze znakiem
	typedef __int16 int16;
	/// Liczba 16-bitowa bez znaku
	typedef unsigned __int16 uint16;
	/// Liczba 32-bitowa ze znakiem
	typedef __int32 int32;
	/// Liczba 32-bitowa bez znaku
	typedef unsigned __int32 uint32;
	/// Liczba 64-bitowa ze znakiem
	typedef __int64 int64;
	/// Liczba 64-bitowa bez znaku
	typedef unsigned __int64 uint64;
#else
	typedef unsigned int uint;
	typedef char int8;
	typedef unsigned char uint8;
	typedef short int16;
	typedef unsigned short uint16;
	typedef int int32;
	typedef unsigned int uint32;
	typedef long long int64;
	typedef unsigned long long uint64;
#endif
//@}

#ifdef WIN32
	#define common_memzero(buf,bufSize)   { ZeroMemory(buf,bufSize); }
#else
	#define common_memzero(buf,bufSize)   { memset(buf,0,bufSize); }
#endif

/* These macros define string functions from C standard library they way that
under Visual C++ they resolve to safe _s versions and use Unicode if _UNICODE
defined and under Linux they use standard C functions.

There is a problem with variadic macros (macros with variable number of
arguments) when no argument supplied to the macro. According to language
standard, macro arguments would contain erroneus trailing comma. Visual C++
automatically deals with this. GNU compiler requires using special syntax:
##__VA_ARGS__. */
#ifdef _MSC_VER
	#define common_sprintf(buf,bufSize,fmt,...)   ( _stprintf_s(buf,bufSize,fmt,__VA_ARGS__) )
	#define common_memcpy(dst,src,count)   { memcpy_s(dst,count,src,count); }
	#define common_memmove(dst,src,count)   { memmove_s(dst,count,src,count); }
	#define common_strcat(dst,dstSize,src)   { _tcscat_s(dst,dstSize,src); }
	#define common_strncat(dst,dstSize,src,count)   { _tcsncat_s(dst,dstSize,src,count); }
	#define common_strchr(str,c)   ( _tcschr(str,c) )
	#define common_strrchr(str,c)   ( _tcsrchr(str,c) )
	#define common_strcmp(s1,s2)   ( _tcscmp(s1,s2) )
	#define common_strncmp(s1,s2,count)   ( _tcsncmp(s1,s2,count) )
	#define common_stricmp(s1,s2)   ( _tcsicmp(s1,s2) )
	#define common_strnicmp(s1,s2,count)   ( _tcsnicmp(s1,s2,count) )
	#define common_strcoll(s1,s2)   ( _tcscoll(s) )
	#define common_strcpy(dst,dstSize,src)   { _tcscpy_s(dst,dstSize,src); }
	#define common_strncpy(dst,dstSize,src,count)   { _tcsncpy(dst,dstSize,src,count); }
	#define common_strlen(str)   ( _tcslen(str) )
	#define common_strspn(str,charset)   ( _tcsspn(str,charset) )
	#define common_strcspn(str,charset)   ( _tcscspn(str,charset) )
	#define common_strpbrk(str,charset)   ( _tcspbrk(str,charset) )
	#define common_strstr(str,strSearch)   ( _tcsstr(str,strSearch) )
	#define common_strxfrm(dst,str,count)   ( _tcsxfrm(dst,str,count) )

#else
	#define common_sprintf(buf,bufSize,fmt,...)   ( sprintf(buf,fmt,##__VA_ARGS__) )
	#define common_memcpy(dst,src,count)   { memcpy(dst,src,count); }
	#define common_memmove(dst,src,count)   { memmove(dst,src,count); }
	#define common_strcat(dst,dstSize,src)   { strcat(dst,src); }
	#define common_strncat(dst,dstSize,src,count)   { strncat(dst,src,count); }
	#define common_strchr(str,c)   ( strchr(str,c) )
	#define common_strrchr(str,c)   ( strrchr(str,c) )
	#define common_strcmp(s1,s2)   ( strcmp(s1,s2) )
	#define common_strncmp(s1,s2,count)   ( strncmp(s1,s2,count) )
	#define common_stricmp(s1,s2)   ( stricmp(s1,s2) )
	#define common_strnicmp(s1,s2,count)   ( strnicmp(s1,s2,count) )
	#define common_strcoll(s1,s2)   ( strcoll(s) )
	#define common_strcpy(dst,dstSize,src)   { strcpy(dst,src); }
	#define common_strncpy(dst,dstSize,src,count)   { strncpy(dst,src,count); }
	#define common_strlen(str)   ( strlen(str) )
	#define common_strspn(str,charset)   ( strspn(str,charset) )
	#define common_strcspn(str,charset)   ( strcspn(str,charset) )
	#define common_strpbrk(str,charset)   ( strpbrk(str,charset) )
	#define common_strstr(str,strSearch)   ( strstr(str,strSearch) )
	#define common_strxfrm(dst,str,count)   ( strxfrm(dst,str,count) )

#endif

/// Wektor ≥aÒcuchÛw
typedef std::vector<tstring> STRING_VECTOR;

/// Jeúli wskaünik niezerowy, zwalnia go i zeruje
#define SAFE_DELETE(x) { delete (x); (x) = 0; }
/// Jeúli wskaünik niezerowy, zwalnia go jako tablicÍ i zeruje
#define SAFE_DELARR(x) { delete [] (x); (x) = 0; }
/// Jeúli wskaünik niezerowy, wywo≥uje mu Release() i zeruje
#define SAFE_RELEASE(x) { if (x) { (x)->Release(); (x) = 0; } }

#ifdef WIN32
	/// Asercja, ktÛra ZAWSZE przerywa program kiedy jest niespe≥niona (w debuggerze - robi breakpoint, bez debuggera - wywala program).
	#define	ASSERT_INT3(x) if ((x) == 0) { __asm { int 3 } }
	/// Asercja, ktÛra w kompilacji Debug przerywa program kiedy jest niespe≥niona (w debuggerze - robi breakpoint, bez debuggera - wywala program).
	#ifdef _DEBUG
		#define	ASSERT_INT3_DEBUG(x) if ((x) == 0) { __asm { int 3 } }
	#else
		#define ASSERT_INT3_DEBUG(x) { }
	#endif
#endif

/// Do robienia alignacji co do jednego bajtu dla struktur, czyli bez wype≥niaczy
/** Autor: agent_J. */
#ifdef _MSC_VER
	#define PACKED __declspec(align(1))
#else
	#define PACKED __attribute__((packed))
#endif

/// Makro do umieszczania w klasach, øeby klasa nie mia≥a konstruktora kopiujπcego ani operatora przypisania.
/* UøywaÊ na poczπtku definicji klasy.
Na podstawie biblioteki wxWidgets. */
#define DECLARE_NO_COPY_CLASS(ClassName)        \
	private:                                    \
		ClassName(const ClassName&);            \
		ClassName& operator=(const ClassName&);

/// Asercja w czasie kompilacji
#define STATIC_ASSERT(x) { const char temp[ (((x) == 0) ? 0 : 1) ] = {'\0'}; }

/// Uniwersalny, brakujπcy w C++ operator dos≥ownego rzutowania (reintepretacji bitowej)
template <typename destT, typename srcT>
destT &absolute_cast(srcT &v)
{
	return reinterpret_cast<destT&>(v);
}
template <typename destT, typename srcT>
const destT &absolute_cast(const srcT &v)
{
	return reinterpret_cast<const destT&>(v);
}

#define REF_COUNTED_FIELDS \
	uint m_RefCount;
#define REF_COUNTED_METHODS \
	void AddRef() { m_RefCount++; } \
	void Release() { assert(m_RefCount > 0); if (--m_RefCount == 0) delete this; }
#define REF_COUNTED_CTOR \
	{ m_RefCount = 1; }

#ifdef _DEBUG
	#define NODEFAULT   assert(0)
#else
	#define NODEFAULT   __assume(0)
#endif

//@}
// code_base

/// G≥Ûwna przestrzeÒ nazw biblioteki CommonLib
namespace common
{

/** \addtogroup code_base Base Module */
//@{

//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
/** \addtogroup base_general OgÛlne */
//@{

#ifndef _countof
	/// \internal
	template <typename _CountofType, size_t _SizeOfArray>
	char (*__countof_helper(_CountofType (&_Array)[_SizeOfArray]))[_SizeOfArray];
	/// Makro do pobierania w czasie kompilacji liczby elementÛw statycznej tablicy.
	/** Skopiowane z stdlib.h z Visual C++. */
	#define _countof(_Array) sizeof(*__countof_helper(_Array))
#endif

/// Orientacja, czyli u≥oøenie - pionowa lub pozioma
enum ORIENTATION
{
	O_NONE,
	O_VERT,
	O_HORIZ,
};

/// Alokuje nowπ tablicÍ dynamicznπ 2D
template <typename T>
T **new_2d(size_t cx, size_t cy)
{
	T **a = new T*[cx];
	for (size_t x = 0; x < cx; x++)
		a[x] = new T[cy];
	return a;
}

/// Zwalnia tablicÍ dynamicznπ 2D
template <typename T>
void delete_2d(T **a, size_t cx)
{
	for (size_t x = 0; x < cx; x++)
		delete[] a[x];
	delete[] a;
}

/// Kopiuje string do char* ≥πcznie ze znakami '\\0' (czego nie zapewnia strcpy)
/**
- Na koÒcu do≥πcza '\\0'.
- Oczywiúcie dest musi byÊ dostatecznie pojemne. */
inline void strcpy0(tchar* dest, const tstring &src)
{
	for (size_t i = 0; i < src.length(); i++)
		dest[i] = src[i];
	dest[src.length()] = '\0';
}

/// Wzorzec projektowy Singleton - klasa bazowa
/**
SposÛb uøycia:
\code
class MyClass : public common::Singleton<MyClass>
{
public:
  void Foo();
};

...
MyClass::GetInstance().Foo();
\endcode
*/
template <typename T>
class Singleton
{
public:
	/// Pobiera jedynπ instancjÍ tej klasy. Tworzy jπ przy pierwszym pobraniu.
	static T & GetInstance()
	{
		static T Instance;
		return Instance;
	}
};

/// Algorytm jak te z STL - sortowanie przez wstawianie.
template<class Iterator>
void InsertionSort(Iterator b, Iterator e)
{
	if (b == e) return;
	for (Iterator j = b + 1; j < e; j++)
	{
		typename std::iterator_traits<Iterator>::value_type key;
		key = *j;
		Iterator i = j;
		while (i > b && key < *(i-1))
		{
			*i = *(i-1);
			i--;
		} 
		*i = key;
	}
}

/// Algorytm jak te z STL - sortowanie przez wstawianie, z w≥asnym komparatorem.
template<class Iterator, typename Compare>
void InsertionSort(Iterator b, Iterator e, Compare Comp)
{
	if (b == e) return;
	for (Iterator j = b + 1; j < e; j++)
	{
		typename std::iterator_traits<Iterator>::value_type key;
		key = *j;
		Iterator i = j;
		while (i > b && Comp(key, *(i-1)))
		{
			*i = *(i-1);
			i--;
		} 
		*i = key;
	}
}

template <typename IterT, typename KeyT, typename CmpT>
IterT FirstNotLessIndex(IterT beg, IterT end, const KeyT &key, CmpT cmp)
{
	unsigned down=0, up=(end-beg);
	while(down<up)
	{
		unsigned mid=(down+up)/2;
		int res=cmp(key,*(beg+mid));
		if(res>0)
			down=mid+1;
		else
			up=mid;
	}
	return beg+down;
}

/// An STL-compatible algorithm that performs binary search in sorted container using random-access iterators and return iterator to found element.
/** If key element not found, returns end. \n
This function is useful because standard std::binary_search returns bool and not iterator (what a nonsense!). \n
cmp must be a functor in for of: int cmp(const KeyT &key, const ValT &val), where ValT i the type of *beg and *end.
*/
template <typename IterT, typename KeyT, typename CmpT>
IterT BinarySearch(IterT beg, IterT end, const KeyT &key, CmpT cmp)
{
	IterT it = FirstNotLessIndex<IterT, KeyT, CmpT>(beg, end, key, cmp);
	if (it == end || cmp(key, *it) != 0)
		return end;
	return it;
}

/// Swaps endianess of a 2-byte value.
inline void SwapEndian16(void *p)
{
	uint16 &u = *(uint16*)p;
	u = (u << 8) | (u >> 8);
}
/// Swaps endianess of a 4-byte value.
inline void SwapEndian32(void *p)
{
	uint32 &u = *(uint32*)p;
	u = (u << 24) | ((u & 0x0000ff00u) << 8) | ((u & 0x00ff0000u) >> 8) | (u >> 24);
}
/// Swaps endianess of a 8-byte value.
inline void SwapEndian64(void *p)
{
	uint64 &u = *(uint64*)p;
	u = (u << 56) | (u >> 56) |
		((u & 0x000000000000ff00ull) << 40) | ((u & 0x00ff000000000000ull) >> 40) |
		((u & 0x0000000000ff0000ull) << 24) | ((u & 0x0000ff0000000000ull) >> 24) |
		((u & 0x00000000ff000000ull) <<  8) | ((u & 0x000000ff00000000ull) >>  8);
}

void SwapEndian16_Array(void *p, uint count);
void SwapEndian32_Array(void *p, uint count);
void SwapEndian64_Array(void *p, uint count);

void SwapEndian16_Data(void *p, uint count, int stepBytes);
void SwapEndian32_Data(void *p, uint count, int stepBytes);
void SwapEndian64_Data(void *p, uint count, int stepBytes);

inline void SwapEndian(bool   &v) { }
inline void SwapEndian(uint8  &v) { }
inline void SwapEndian(int8   &v) { }
inline void SwapEndian(uint16  &v) { SwapEndian16(&v); }
inline void SwapEndian(int16   &v) { SwapEndian16(&v); }
inline void SwapEndian(uint32  &v) { SwapEndian32(&v); }
inline void SwapEndian(int32   &v) { SwapEndian32(&v); }
inline void SwapEndian(uint64  &v) { SwapEndian64(&v); }
inline void SwapEndian(int64   &v) { SwapEndian64(&v); }
inline void SwapEndian(float  &v) { SwapEndian32(&v); }
inline void SwapEndian(double &v) { SwapEndian64(&v); }

/// Zatrzymuje bieøπcy wπtek na podanπ liczbÍ milisekund
/** Dzia≥a z jakπútam dok≥adnoúciπ - moøe nie super, ale na pewno wiÍkszπ niø
ca≥a sekunda. */
void Wait(uint32 Miliseconds);

template <typename T>
inline int UniversalCmp(const T &a, const T &b)
{
	if (a < b) return -1;
	if (b < a) return  1;
	return 0;
}

//@}

//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
/** \addtogroup base_smartptr Inteligentne wskaüniki */
//@{

/// Polityka zwalaniania inteligentnych wskaünikÛw, ktÛra robi: <tt>delete p;</tt>
class DeletePolicy
{
public:
	template <typename T>
	static void Destroy(T *p)
	{
		// Sztuczka inspirowana Boost (checked_delete.hpp)
		typedef char type_must_be_complete[ sizeof(T)? 1: -1 ];
		(void) sizeof(type_must_be_complete);
		delete p;
	}
};
/// Polityka zwalaniania inteligentnych wskaünikÛw, ktÛra robi: <tt>delete [] p;</tt>
class DeleteArrayPolicy
{
public:
	template <typename T>
	static void Destroy(T *p)
	{
		// Sztuczka inspirowana Boost (checked_delete.hpp)
		typedef char type_must_be_complete[ sizeof(T)? 1: -1 ];
		(void) sizeof(type_must_be_complete);
		delete [] p;
	}
};
/// Polityka zwalaniania inteligentnych wskaünikÛw, ktÛra robi: <tt>p->Release();</tt>
class ReleasePolicy
{
public:
	template <typename T>
	static void Destroy(T *p)
	{
		if (p) p->Release();
	}
};

/// Inteligentny wskaünik z wy≥πcznym prawem w≥asnoúci.
/**
- Niekopiowalny.
- W destruktorze zwalnia. */
template <typename T, typename PolicyT = DeletePolicy>
class scoped_ptr
{
    template<typename Y, typename PolicyY> friend class scoped_ptr;

private:
	T *m_Ptr;

	scoped_ptr(const scoped_ptr &);
	scoped_ptr & operator = (const scoped_ptr &);

public:
	typedef T value_type;
	typedef T *ptr_type;

	explicit scoped_ptr(T *p = NULL) : m_Ptr(p) { }
	~scoped_ptr() { PolicyT::template Destroy<T>(m_Ptr); }
	
	T & operator * () const { assert(m_Ptr != NULL); return *m_Ptr; }
	T * operator -> () const { assert(m_Ptr != NULL); return m_Ptr; }
	T & operator [] (uint i) const { return m_Ptr[i]; }

	inline friend bool operator == (const scoped_ptr &lhs, const T *rhs) { return lhs.m_Ptr == rhs; }
	inline friend bool operator == (const T *lhs, const scoped_ptr &rhs) { return lhs == rhs.m_Ptr; }
	inline friend bool operator != (const scoped_ptr &lhs, const T *rhs) { return lhs.m_Ptr != rhs; }
	inline friend bool operator != (const T *lhs, const scoped_ptr &rhs) { return lhs != rhs.m_Ptr; }

	T * get() const { return m_Ptr; }
	void swap(scoped_ptr<T, PolicyT> &b) { T *tmp = b.m_Ptr; b.m_Ptr = m_Ptr; m_Ptr = tmp; }
	void reset(T *p = NULL) { assert(p == NULL || p != m_Ptr); scoped_ptr<T, PolicyT>(p).swap(*this); }
	bool is_null() const { return m_Ptr == NULL; }
};

/// Inteligentny wskaünik ze zliczaniem referencji
/**
- Kopiowalny.
- Zwalnia, kiedy zwolniony zostanie ostatni wskazujπcy na obiekt common::shared_ptr. */
template <typename T, typename PolicyT = DeletePolicy>
class shared_ptr
{
    template<typename Y, typename PolicyY> friend class shared_ptr;

private:
	T *m_Ptr;
	unsigned *m_Counter;

public:
	typedef T value_type;
	typedef T *ptr_type;

	explicit shared_ptr(T *p = NULL) : m_Ptr(p), m_Counter(p ? new unsigned(1) : NULL) { }
	~shared_ptr() { if (m_Ptr && --(*m_Counter) == 0) { delete m_Counter; PolicyT::template Destroy<T>(m_Ptr); } }
	
	shared_ptr(const shared_ptr &p) : m_Ptr(p.m_Ptr), m_Counter(p.m_Counter) { if (m_Ptr) (*m_Counter)++; }
	shared_ptr & operator = (const shared_ptr &p) { reset<T>(p); return *this; }
	template <typename U, typename PolicyU> explicit shared_ptr(const shared_ptr<U, PolicyU> &p) : m_Ptr(p.m_Ptr), m_Counter(p.m_Counter) { if (m_Ptr) (*m_Counter)++; }
	template <typename U, typename PolicyU> shared_ptr & operator = (const shared_ptr<U, PolicyU> &p) { reset<U, PolicyU>(p); return *this; }

	T & operator * () const { assert(m_Ptr != NULL); return *m_Ptr; }
	T * operator -> () const { assert(m_Ptr != NULL); return m_Ptr; }
	T & operator [] (uint i) const { return m_Ptr[i]; }

	inline friend bool operator == (const shared_ptr &lhs, const T *rhs) { return lhs.m_Ptr == rhs; }
	inline friend bool operator == (const T *lhs, const shared_ptr &rhs) { return lhs == rhs.m_Ptr; }
	inline friend bool operator != (const shared_ptr &lhs, const T *rhs) { return lhs.m_Ptr != rhs; }
	inline friend bool operator != (const T *lhs, const shared_ptr &rhs) { return lhs != rhs.m_Ptr; }
	template <typename U, typename PolicyU> bool operator == (const shared_ptr<U, PolicyU> &rhs) const { return m_Ptr == rhs.m_Ptr; }
	template <typename U, typename PolicyU> bool operator != (const shared_ptr<U, PolicyU> &rhs) const { return m_Ptr != rhs.m_Ptr; }

	T * get() const { return m_Ptr; }
	void swap(shared_ptr<T, PolicyT> &b) { T *tmp = b.m_Ptr; b.m_Ptr = m_Ptr; m_Ptr = tmp; unsigned *tmpc = b.m_Counter; b.m_Counter = m_Counter; m_Counter = tmpc; }
	void reset(T *p = NULL) { if (p == m_Ptr) return; shared_ptr<T, PolicyT>(p).swap(*this); }
	template <typename U, typename PolicyU> void reset(const shared_ptr<U, PolicyU> &p) { shared_ptr<U, PolicyU>(p).swap(*this); }
	bool unique() const { return m_Counter == NULL || *m_Counter == 1; }
	bool is_null() const { return m_Ptr == NULL; }
};

#ifdef WIN32
	/// Polityka zwalniania uchwytu - robi: <tt>CloseHandle(p);</tt>
	class CloseHandlePolicy  { public: template <typename T> static void Destroy(T p) { if (p != NULL) CloseHandle(p); } };
	/// Polityka zwalniania uchwytu - robi: <tt>DeleteObject(p);</tt>
	class DeleteObjectPolicy { public: template <typename T> static void Destroy(T p) { if (p != NULL) DeleteObject(p); } };
#endif

/// Inteligentny uchwyt - wy≥πczna w≥asnoúÊ
template <typename T, typename PolicyT>
class scoped_handle
{
    template<typename Y, typename PolicyY> friend class scoped_handle;

private:
	T m_Ptr;

	scoped_handle(const scoped_handle &);
	scoped_handle & operator = (const scoped_handle &);

public:
	typedef T value_type;

	explicit scoped_handle(T p) : m_Ptr(p) { }
	~scoped_handle() { PolicyT::template Destroy<T>(m_Ptr); }
	
	inline friend bool operator == (const scoped_handle &lhs, const T &rhs) { return lhs.m_Ptr == rhs; }
	inline friend bool operator == (const T &lhs, const scoped_handle &rhs) { return lhs == rhs.m_Ptr; }
	inline friend bool operator != (const scoped_handle &lhs, const T &rhs) { return lhs.m_Ptr != rhs; }
	inline friend bool operator != (const T &lhs, const scoped_handle &rhs) { return lhs != rhs.m_Ptr; }

	const T & get() const { return m_Ptr; }
	void swap(scoped_handle<T, PolicyT> &b) { T tmp = b.m_Ptr; b.m_Ptr = m_Ptr; m_Ptr = tmp; }
	void reset(T p) { scoped_handle<T, PolicyT>(p).swap(*this); }
};

/// Inteligentny uchwyt ze zliczaniem referencji
template <typename T, typename PolicyT>
class shared_handle
{
    template<typename Y, typename PolicyY> friend class shared_handle;

private:
	T m_Ptr;
	unsigned *m_Counter;

public:
	typedef T value_type;

	explicit shared_handle(T p) : m_Ptr(p), m_Counter(new unsigned(1)) { }
	~shared_handle() { if (--(*m_Counter) == 0) { delete m_Counter; PolicyT::template Destroy<T>(m_Ptr); } }
	
	shared_handle(const shared_handle &p) : m_Ptr(p.m_Ptr), m_Counter(p.m_Counter) { (*m_Counter)++; }
	shared_handle & operator = (const shared_handle &p) { reset<T>(p); return *this; }
	template <typename U, typename PolicyU> explicit shared_handle(const shared_handle<U, PolicyU> &p) : m_Ptr(p.m_Ptr), m_Counter(p.m_Counter) { (*m_Counter)++; }
	template <typename U, typename PolicyU> shared_handle & operator = (const shared_handle<U, PolicyU> &p) { reset<U, PolicyU>(p); return *this; }

	inline friend bool operator == (const shared_handle &lhs, const T &rhs) { return lhs.m_Ptr == rhs; }
	inline friend bool operator == (const T &lhs, const shared_handle &rhs) { return lhs == rhs.m_Ptr; }
	inline friend bool operator != (const shared_handle &lhs, const T &rhs) { return lhs.m_Ptr != rhs; }
	inline friend bool operator != (const T &lhs, const shared_handle &rhs) { return lhs != rhs.m_Ptr; }
	template <typename U, typename PolicyU> bool operator == (const shared_handle<U, PolicyU> &rhs) const { return m_Ptr == rhs.m_Ptr; }
	template <typename U, typename PolicyU> bool operator != (const shared_handle<U, PolicyU> &rhs) const { return m_Ptr != rhs.m_Ptr; }

	const T & get() const { return m_Ptr; }
	void swap(shared_handle<T, PolicyT> &b) { T tmp = b.m_Ptr; b.m_Ptr = m_Ptr; m_Ptr = tmp; unsigned *tmpc = b.m_Counter; b.m_Counter = m_Counter; m_Counter = tmpc; }
	void reset(T p) { if (p == m_Ptr) return; shared_handle<T, PolicyT>(p).swap(*this); }
	template <typename U, typename PolicyU> void reset(const shared_handle<U, PolicyU> &p) { shared_handle<U, PolicyU>(p).swap(*this); }
	bool unique() const { return *m_Counter == 1; }
};

template <typename T, typename PolicyT> void swap(scoped_ptr<T, PolicyT> &a, scoped_ptr<T, PolicyT> &b) { a.swap(b); }
template <typename T, typename PolicyT> void swap(shared_ptr<T, PolicyT> &a, shared_ptr<T, PolicyT> &b) { a.swap(b); }
template <typename T, typename PolicyT> void swap(scoped_handle<T, PolicyT> &a, scoped_handle<T, PolicyT> &b) { a.swap(b); }
template <typename T, typename PolicyT> void swap(shared_handle<T, PolicyT> &a, shared_handle<T, PolicyT> &b) { a.swap(b); }

//@}


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
/** \addtogroup base_math Podstawowa matematyka
Sztuczki z bitami opracowane na podstawie:
Bit Twiddling Hacks, Sean Eron Anderson, http://www-graphics.stanford.edu/~seander/bithacks.html
*/
//@{

/** \addtogroup base_minmax Maksymalne i minimalne wartoúci
Maksymalne i minimalne wartoúci liczb rÛønych typÛw. */
//@{
const int8 MININT8 = std::numeric_limits<int8>::min();
const int8 MAXINT8 = std::numeric_limits<int8>::max();
const int16 MININT16 = std::numeric_limits<int16>::min();
const int16 MAXINT16 = std::numeric_limits<int16>::max();
const int32 MININT32 = std::numeric_limits<int32>::min();
const int32 MAXINT32 = std::numeric_limits<int32>::max();
const int64 MININT64 = std::numeric_limits<int64>::min();
const int64 MAXINT64 = std::numeric_limits<int64>::max();
const uint8 MAXUINT8 = std::numeric_limits<uint8>::max();
const uint16 MAXUINT16 = std::numeric_limits<uint16>::max();
const uint32 MAXUINT32 = std::numeric_limits<uint32>::max();
const uint64 MAXUINT64 = std::numeric_limits<uint64>::max();
const float MINFLOAT = std::numeric_limits<float>::min();
const float MAXFLOAT = std::numeric_limits<float>::max();
//@}

/// Liczba zmiennoprzecinkowa bliska zeru
/** O dziwo to makro zamienione na funkcjÍ inline dzia≥a wolniej - nigdy nie ufaj
optymalizacji kompilatora! */
#define FLOAT_ALMOST_ZERO(F) ((absolute_cast<uint32>(F) & 0x7f800000L) == 0)

/** \addtogroup base_math_consts Sta≥e matematyczne
Bo sta≥e z cmath/math.h nie chcπ dzia≥aÊ mimo zdefiniowania wczeúniej USE_MATH_CONSTANTS :P */
//@{
const float E         = 2.71828182845904523536f;  ///< e
const float LOG2E     = 1.44269504088896340736f;  ///< log2(e)
const float LOG10E    = 0.434294481903251827651f; ///< log10(e)
const float LN2       = 0.693147180559945309417f; ///< ln(2)
const float LN10      = 2.30258509299404568402f;  ///< ln(10)
const float PI        = 3.14159265358979323846264338327950288419716939937510582f;  ///< pi
const float PI_2      = 1.57079632679489661923f;  ///< pi/2
const float PI_4      = 0.785398163397448309616f; ///< pi/4
const float PI_X_2    = 6.28318530717958647692f;  ///< 2*pi
const float _1_PI     = 0.318309886183790671538f; ///< 1/pi
const float _2_PI     = 0.636619772367581343076f; ///< 2/pi
const float _2_SQRTPI = 1.12837916709551257390f;  ///< 2/sqrt(pi)
const float SQRT2     = 1.41421356237309504880f;  ///< sqrt(2)
const float SQRT3     = 1.7320508075688772935274463415059f; ///< sqrt(3)
const float SQRT1_2   = 0.707106781186547524401f; ///< 1/sqrt(2)
//@}

// Windows - potrzebuje float.h
#ifdef WIN32
	/// Zwraca true, jeúli liczba jest niezwyczajna (INF lub NaN)
	bool is_finite(float x);
	/// Zwraca true, jeúli liczba jest niezwyczajna (INF lub NaN)
	bool is_finite(double x);
	/// Zwraca true, jeúli liczba jest NaN
	bool is_nan(float x);
	/// Zwraca true, jeúli liczba jest NaN
	bool is_nan(double x);
// Linux - potrzebuje math.h
#else
	inline bool is_finite(float x)  { return (finitef(x) != 0); }
	inline bool is_finite(double x) { return (finite(x)  != 0); }
	inline bool is_nan(float x)  { return (isnanf(x) != 0); }
	inline bool is_nan(double x) { return (isnan(x)  != 0); }
#endif

/// Dodaje dwie liczby z ograniczeniem zakresu zamiast zawiniÍcia przy przepe≥nieniu
/** Jako T uøywaÊ uint8, uint16, uint32, uint64
(Autor: Tarlandil) */
template <typename T>
T safe_add(T a, T b)
{
	T R = a + b;
	if (R < std::max(a, b)) return std::numeric_limits<T>::max();
	else return R;
}

/// Odejmuje dwie liczby z ograniczeniem zakresu zamiast zawiniÍcia przy przepe≥nieniu
/** Jako T uøywaÊ uint8, uint16, uint32, uint64
(Autor: Tarlandil) */
template <typename T>
T safe_sub(T a, T b)
{
	if (b > a) return T();
	else return a - b;
}

/// Mnoøy dwie liczby z ograniczeniem zakresu zamiast zawiniÍcia przy przepe≥nieniu
/** Jako T uøywaÊ uint8, uint16, uint32, uint64
(Autor: Tarlandil) */
template <typename T>
T safe_mul(T a, T b)
{
	if (b == T()) return T();
	T R = a * b;
	if (R / b != a) return std::numeric_limits<T>::max();
	else return R;
}

/// Bezpieczny arcus cosinus, ogranicza zakres wejúciowy do -1...+1 zwracajπc w razie przekrocznia skrajne wartoúci wyniku.
inline float safe_acos(float x)
{
	if (x <= -1.0f) return PI;
	if (x >= 1.0f) return 0.0f;
	return acosf(x);
}

/// Zaokrπgla liczbÍ, zamiast jπ obcinaÊ
/** Tak matematycznie, czyli do gÛry lub w dÛ≥ zaleønie od czÍúci u≥amkowej. */
inline int round(float x)
{
	return static_cast<int>(floorf(x+0.5f));
}
inline int round(double x)
{
	return static_cast<int>(floor(x+0.5));
}

/// Dzieli 2 liczby ca≥kowite zaokrπglajπc wynik w gÛrÍ
/** Jako typ stosowaÊ int, uint itp.
Dzia≥a tylko dla liczb dodatnich.
UwaøaÊ na przekroczenie zakresu liczby (x+y).
(Author: Tarlandil) */
template <typename T>
inline T ceil_div(T x, T y)
{
	return (x+y-1) / y;
}

/// Aligns given value up to nearest multiply of align value. For example: AlignUp(11, 8) = 16. Use types like uint32, uint64 as T.
template <typename T>
inline T AlignUp(T val, T align)
{
	return (val + align - 1) / align * align;
}
/// Aligns given value down to nearest multiply of align value. For example: AlignUp(19, 8) = 16. Use types like uint32, uint64 as T.
template <typename T>
inline T AlignDown(T val, T align)
{
	return (val / align) * align;
}

/// Dzielenie z zaokrπgleniem matematycznie do najbliøszej.
template <typename T>
inline T round_div(T x, T y)
{
	return (x + (y / 2)) / y;
}

/// Zwraca true, jeúli liczba leøy w epsilonowym otoczeniu zera
inline bool around(float x, float epsilon)
{
	return (fabsf(x) <= epsilon);
}
inline bool around(double x, double epsilon)
{
	return (fabs(x) <= epsilon);
}

/// Zwraca true, jeúli liczba leøy w epsilonowym otoczeniu drugiej liczby
inline bool around(float x, float y, float epsilon)
{
	return (fabsf(x-y) <= epsilon);
}
inline bool around(double x, double y, double epsilon)
{
	return (fabs(x-y) <= epsilon);
}

/// Zwraca true, jeúli dwie liczby zmiennoprzecinkowe sπ praktycznie rÛwne
/** Te funkcje sπ fajne bo nie trzeba znaÊ epsilona, ale za to jest wiÍcej liczenia.
(Autor: Tarlandil) */
inline bool float_equal(float x, float y)
{
	float epsilon = (fabsf(x)+fabsf(y)) * 1e-4f;
	//return around(x, y, epsilon);
	return (fabsf(x-y) <= epsilon);
}
inline bool double_equal(double x, double y)
{
	double epsilon = (fabs(x)+fabs(y)) * 1e-13;
	//return around(x, y, epsilon);
	return (fabs(x-y) <= epsilon);
}

/// Liczy kwadrat liczby
/** Jako T stosowaÊ dowolny typ ktÛry ma operator *.
Ta funkcja przydaje siÍ jeúli w nawiasie jest wyraøenie a nie jedna wartoúÊ. */
template <typename T> inline T sqr(T v) { return v * v; }

/// Zwraca znak podanej liczby ca≥kowitej, czyli -1 0 lub 1
/** StosowaÊ dla typÛw ca≥kowitych ze znakiem, czyli int8, int16, int32, int64. */
template <typename T> inline T sign(T v) { return (v > 0) - (v < 0); }

/// £πczy bity dwÛch liczb wg maski, tzn. wybiera z A te bity, ktÛre w masce majπ 0 i z B te bity, ktÛre w masce majπ 1.
template <typename T> inline T MergeBits(T a, T b, T Mask) { return a ^ ((a ^ b) & Mask); }

/// Zwraca liczbÍ bitÛw, ktÛre sπ jedynkπ
template <typename T> inline uint CountBitsSet(T v) { uint c; for (c = 0; v; c++) v &= v - 1; return c; }

// Specialization based on "Bit Twiddling Hacks" by Sean Eron Anderson
template<> inline uint CountBitsSet<uint>(unsigned v)
{
	uint c; // store the total here
	static const int S[] = {1, 2, 4, 8, 16}; // Magic Binary Numbers
	static const int B[] = {0x55555555, 0x33333333, 0x0F0F0F0F, 0x00FF00FF, 0x0000FFFF};

	c = v - ((v >> 1) & B[0]);
	c = ((c >> S[1]) & B[1]) + (c & B[1]);
	c = ((c >> S[2]) + c) & B[2];
	c = ((c >> S[3]) + c) & B[3];
	c = ((c >> S[4]) + c) & B[4];
	return c;
}

/// Oblicza bit parzystoúci liczby
inline uint CalcParity(uint v) {
	v ^= v >> 1; v ^= v >> 2; v = (v & 0x11111111U) * 0x11111111U; return (v >> 28) & 1;
}
/// Oblicza bit parzystoúci liczby
inline uint64 CalcParity(uint64 v) {
	v ^= v >> 1; v ^= v >> 2; v = (v & 0x1111111111111111ULL) * 0x1111111111111111ULL; return (v >> 60) & 1;
}

/// Zamienia miejscami ciπg bitÛw w liczbie
template <typename T> inline T SwapBitSequences(T v, uint SrcBit, uint DstBit, uint NumBits)
{
	int x = ((v >> SrcBit) ^ (v >> DstBit)) & ((1 << NumBits) - 1);
	return v ^ ((x << SrcBit) | (x << DstBit));
}

/// Zwraca true, jeúli podana liczba jest potÍgπ dwÛjki.
/** T musi byÊ liczbπ ca≥kowitπ bez znaku - uint8, uint16, uint32, uint64, albo liczbπ ze znakiem ale na pewno dodatniπ.
Dla 0 zwraca true. */
template <typename T> inline bool IsPow2(T x) { return (x & (x-1)) == 0; }

/// Zwraca maskÍ bitowπ z ustawionymi na jedynkÍ n najm≥odszymi bitami.
/** n musi byÊ z z zakresu 0..32. */
uint32 GetBitMask(uint32 n);

/// Zwraca logarytm dwÛjkowy z podanej liczby ca≥kowitej bez znaku, tzn numer najstarszego niezerowego bitu.
/** Innymi s≥owy, o ile bitÛw przesunπÊ w lewo jedynkÍ øeby otrzymaÊ najstarszπ
jedynkÍ podanej liczby. WartoúÊ 0 zwraca dla liczb 0 i 1. */
uint log2u(uint v);
/// Zwraca logarytm dziesiÍtny z podanej liczby ca≥kowitej bez znaku, tzn numer najstarszej cyfry dziesiÍtnej.
/** Dla zera niezdefiniowany. */
uint log10u(uint v);

/// Zwraca najmniejszπ potÍgÍ dwÛjki wiÍkszπ lub rÛwnπ podanej liczbie
/** Dla v == 0 zwraca 0.
Dla v > 0x80000000 wynik jest niezdefiniowany. */
inline uint next_pow2(uint v) {
	v--; v |= v >> 1; v |= v >> 2; v |= v >> 4; v |= v >> 8; v |= v >> 16; v++; return v;
}

/// Przeplata m≥odsze 16 bitÛw a (do parzystych) i m≥odsze 16 bitÛw b (do nieparzystych) do liczby wynikowej
uint InterleaveBits(uint a, uint b);
// Rozk≥ada najm≥odsze 10 bitÛw wstawiajπc przed kaødy po dwa zera, tak øe:
// Wejúcie: ----------------------9876543210
// Wyjúcie: ----9--8--7--6--5--4--3--2--1--0
uint Extend10BitsBy2Zeros(uint n);
// Rozk≥ada najm≥odsze 16 bitÛw wstawiajπc przed kaødy po jedno zero, tak øe:
// Wejúcie: ----------------fedcba9876543210
// Wyjúcie: -f-e-d-c-b-a-9-8-7-6-5-4-3-2-1-0
uint Extend16BitsBy1Zero(uint n);

/** As T use types: uint8, uint16, uint32, uint64. */
template <typename T>
inline T ReverseBits(T v)
{
    // Algorithm from: Bit Twiddling Hacks, Sean Eron Anderson,
    // http://graphics.stanford.edu/~seander/bithacks.html#BitReverseObvious
    
    T r = v; // r will be reversed bits of v; first get LSB of v
    int s = sizeof(v) * 8 - 1; // extra shift needed at end

    for (v >>= 1; v; v >>= 1)
    {   
      r <<= 1;
      r |= v & 1;
      s--;
    }
    r <<= s; // shift when v's highest bits are zero
    
    return r;
}

inline uint8 ReverseBits(uint8 v)
{
	return (uint8)( ((v * 0x0802LU & 0x22110LU) | (v * 0x8020LU & 0x88440LU)) * 0x10101LU >> 16 );
}

/// Returns (x + 1) % 3.
/** T: use unsigned integer types.
Source: "Modulo 3", http://www.codercorner.com/Modulo3.htm, algorithm by Christer Ericson. */
template <typename T>
inline T increment_modulo_3(T x)
{
	return ((T)1 << x) & (T)3;
}

/// Liczy potÍge o ca≥kowitym wyk≥adniku (bardzo szybko!)
/** T moøe byÊ dowolnπ liczbπ - int, uint32, float itp.
(Autor: Tarlandil) */
template <typename T>
T powi(T a, uint32 b)
{
	T r = (T)1;
	T p = a;
	while (b)
	{
		if (b & 1)
			r *= p;
		p *= p;
		b >>= 1;
	}
	return r;
}

/// Raises float number to integer exponent.
/** Checked, works great even for negative exponents. */
inline float powi(float x, int N)
{
    float y, w;
    bool ns = false, xs = false;
    // record signs
    if(N < 0) ns = true, N = -N;
    if(x < 0.0f) xs = true, x = -x;
    // setup initial powers
    if(N & 1) y = x;
    else y = 1.0f, xs = false;
    w = x; N >>= 1;
    while(N)
    {
        w = w * w;
        if(N & 1) y *= w;
        N >>= 1;
    }
    // reinstate signs
    if(xs) y = -y;
    if(ns) y = 1.0f / y;
    return y;
}

template <typename T, typename Predicate>
void Sort2(T &v1, T &v2, Predicate comp)
{
	if (comp(v2, v1))
	{
		T tmp = v1;
		v1 = v2;
		v2 = tmp;
	}
}
template <typename T>
void Sort2(T &v1, T &v2)
{
	Sort2(v1, v2, std::less<T>());
}

template <typename T, typename Predicate>
void Sort3(T &v1, T &v2, T &v3, Predicate comp)
{
	Sort2<T, Predicate>(v1, v3, comp);
	Sort2<T, Predicate>(v1, v2, comp);
	Sort2<T, Predicate>(v2, v3, comp);
}
template <typename T>
void Sort3(T &v1, T &v2, T &v3)
{
	Sort2<T>(v1, v3);
	Sort2<T>(v1, v2);
	Sort2<T>(v2, v3);
}

/// Zwraca najwiÍkszπ z podanych 3 liczb
template <typename T>
inline T max3(const T &a, const T &b, const T &c)
{
	return std::max(a, std::max(b, c));
}

/// Zwraca najmniejszπ z podanych 3 liczb
template <typename T>
inline T min3(const T &a, const T &b, const T &c)
{
	return std::min(a, std::min(b, c));
}

/// Zwraca liczbÍ x ograniczonπ do podanego zakresu od a do b
template <typename T>
inline T minmax(const T &a, const T &x, const T &b)
{
	if (x < a) return a; else if (x > b) return b; else return x;
	//return std::min(b, std::max(a, x));
}
inline float saturate(float x) { return minmax<float>(0.f, x, 1.f); }
inline double saturate(double x) { return minmax<double>(0., x, 1.); }

/// Zwraca czÍúÊ ca≥kowitπ liczby wraz ze znakiem
/** (Autor: Tarlandil) */
inline float trunc(float x)
{
	return (x < 0.0f) ? ceilf(x) : floorf(x);
}
inline double trunc(double x)
{
	return (x < 0.0) ? ceil(x) : floor(x);
}

/// Zwraca czÍúÊ u≥amkowπ liczby wraz ze znakiem
/** (Autor: Tarlandil) */
inline float frac(float x)
{
	return x - floorf(x);
}
inline double frac(double x)
{
	return x - floor(x);
}

/// Oblicza sinus i cosinus za jednym zamachem. Moøe byÊ szybsze, niø liczenie osobno.
void sincos(float angle, float *sine, float *cosine);

/// Interpolacja liniowa 1D (t = 0..1)
inline float Lerp(float x, float y, float t)
{
	return x + t*(y-x);
}
inline void Lerp(float *out, float x, float y, float t)
{
	*out = x + t*(y-x);
}
/// Interpolacja liniowa 1D (t jest ograniczane do 0..1)
inline float SafeLerp(float x, float y, float t)
{
	return Lerp(x, y, minmax(0.0f, t, 1.0f));
}

/// Interpolacja liniowa 2D
/** tx tyczy siÍ pierwszego indeksu, ty drugiego. */
inline float Lerp2D(float x11, float x21, float x12, float x22, float tx, float ty)
{
	return Lerp(Lerp(x11, x21, tx), Lerp(x12, x22, tx), ty);
}

/// Normalizuje kπt do przedzia≥u < 0..2PI )
inline float NormalizeAngle(float angle)
{
	angle /= PI_X_2;
	angle = frac(angle);
	if (angle < 0.0f)
		angle += 1.0f;
	else if (angle >= 1.0f)
		angle -= 1.0f;
	return angle * PI_X_2;
}

/// Normalizuje kπt do przedzia≥u -PI..+PI
inline float NormalizeAngle2(float Angle)
{
	Angle += PI;
	Angle -= floorf(Angle * (1.0f/PI_X_2)) * PI_X_2;
	Angle -= PI;
	return Angle;
}

/// Przelicza kπt ze stopni na radiany
inline float DegToRad(float Angle)
{
	// Angle * PI / 180
	return Angle * 0.017453292519943295769222222222222f;
}
/// Przelicza kπt z radianÛw na stopnie
inline float RadToDeg(float Angle)
{
	// Angle * 180 / PI
	return Angle * 57.295779513082320876846364344191f;
}

/// Oblicza rÛønicÍ kπtÛw A-B, gdzie A, B naleøπ do: 0..2*PI
/** UwzglÍdnia przekrÍcanie siÍ kπtÛw, znajduje wiÍc najkrÛtszπ drogÍ miÍdzy nimi. */
inline float AngleDiff(float A, float B)
{
	float R1 = B - A;
	float R2 = B - PI_X_2 - A;
	float R3 = B + PI_X_2 - A;

	float R1a = fabsf(R1);
	float R2a = fabsf(R2);
	float R3a = fabsf(R3);

	if (R1a < R2a && R1a < R3a)
		return R1;
	else if (R2a < R3a)
		return R2;
	else
		return R3;
}

/// Krzywa wyg≥adzona 0..1 -> 0..1: e(x) = 3*x^2 - 2*x^3
inline float EaseCurve(float x)
{
	return x * x * (3.0f - 2.0f * x);
}
// Krzywa wyg≥adzona 0..1 -> 0..1: e(x) = 6*t^5 - 15*t^4 + 10*t^3
/** Lepsza od EaseCurve, bo ma zerowπ drugπ pochodnπ dla x=0 i x=1. */
inline float EaseCurve2(float x)
{
	return x * x * x * (x * (x * 6.0f - 15.0f) + 10.0f);
}

/// Returns 0.0 if x <= min and 1.0 if x >= max and performs smooth Hermite interpolation between 0 and 1 when min < x < max.
/** This is useful in cases where you want a threshold function with a
smooth transition. Results are undefined if min >= max. */
inline float smoothstep(float min, float max, float x)
{
	if(x <= min) return 0.0f;
	if(x >= max) return 1.0f;
	x = (x - min) / (max - min);
	return x * x * (3.0f - 2.0f * x);
}

/// More precise, but slower version of smoothstep function.
inline float smoothstep2(float min, float max, float x)
{
	if(x <= min) return 0.f;
	if(x >= max) return 1.f;
	x = (x - min) / (max - min);
	return 0.5f - cosf(PI * x) * 0.5f;
}

bool is_prime(unsigned n);

/// Funkcja wyg≥adzajπca - Critically Damped Smoothing
/** To jest wyliczane wg rÛwnania rÛøniczkowego na jakπútam sprÍøynÍ, dok≥adnie
analitycznie, wiÍc TimeDelta moøe byÊ dowolnie duøe.
\param[in,out] InOutPos  Pozycja przed i po przeliczeniu tego kroku
\param[in,out] InOutVel  PrÍdkoúÊ przed i po przeliczeniu tego kroku
\param Dest              Pozycja docelowa
\param SmoothTime        WspÛ≥czynnik "bezw≥adnoúci" w jednostkach czasu
\param TimeDelta         Czas kroku (np. czas od poprzedniej klatki)
\param MaxSpeed          Maksymalna dopuszczalna prÍdkoúÊ (maksymalna odleg≥oúÊ na jakπ pozycja moøe nie nadπøaÊ?) */
void SmoothCD(float *InOutPos, float Dest, float *InOutVel, float SmoothTime, float TimeDelta);
void SmoothCD(float *InOutPos, float Dest, float *InOutVel, float SmoothTime, float TimeDelta, float MaxSpeed);

/// Zwraca liczbÍ z przedzia≥u 0..1, zaleønie od wartoúci x wzglÍdem brzegÛw trapezu a, b, c, d.
/**
\verbatim
    x < a => 0
a < x < b => 0..1
b < x < c => 1
c < x < d => 1..0
d < x     => 0
\endverbatim
*/
inline float Trapezoidal(float x, float a, float b, float c, float d)
{
	if (x < a)
		return 0.f;
	else if (x < b)
		return (x-a)/(b-a);
	else if (x < c)
		return 1.f;
	else if (x < d)
		return 1.f - (x-c)/(d-c);
	else
		return 0.f;
}
/// Jak Trapezoidal(), ale zamiast 0..1 zwraca liczbÍ z przedzia≥u min..max.
inline float Trapezoidal(float x, float a, float b, float c, float d, float min, float max)
{
	return Trapezoidal(x, a, b, c, d) * (max-min) + min;
}

/// RÛwnanie kwadratowe a*x^2 + b*x + c = 0
/** Musi byÊ: a != 0.
- Brak pierwiastkÛw - zwraca 0, x1 i x2 niezdefiniowane.
- Jeden pierwiastek - zwraca 1, x1 = x2 = jedyne rozwiπzanie.
- Dwa pierwistki - zwraca 2, x1 to pierwszy pierwiastek, x2 to drugi */
int QuadraticEquation(float a, float b, float c, float *Out_x1, float *Out_x2);

/// Oblicza úredniπ i opcjonalnie wariancjÍ ze zbioru liczb typu float.
/** sqrt(Variance) to ochylenie standardowe.
\param Variance Moøna podaÊ NULL, jeúli nas nie interesuje. */
void CalcMeanAndVariance(const float Numbers[], size_t NumberCount, float *OutMean, float *OutVariance = NULL, bool VarianceBiased = true);
void CalcMeanAndVariance(const void *NumberData, size_t NumberCount, int NumberStride, float *OutMean, float *OutVariance = NULL, bool VarianceBiased = true);

/// MurmurHash 2.0 - podobno bardzo dobra funkcja hashujπca. Dowolne dane binarne. Niestety nie inkrementacyjna.
uint MurmurHash(const void *Data, uint DataLen, uint Seed);

/// Bardzo szybka funkcja hashujπca dane.
/**
Autor: Paul Hsieh, http://www.azillionmonkeys.com/qed/hash.html
Niestety nie inkrementacyjna.
*/
uint SuperFastHash(const void *DataBytes, size_t DataLen);

/// Hash of 32-bit integer number (uint to uint).
/**
Source: "Integer Hash Function", Thomas Wang \n
http://www.concentric.net/~Ttwang/tech/inthash.htm
*/
inline uint WangMix(uint key)
{
	key += ~(key << 15);
	key ^= (key >> 10);
	key += (key << 3);
	key ^= (key >> 6);
	key += ~(key << 11);
	key ^= (key >> 16);
	return key;
}

/// Interpoluje danπ wielkoúÊ miÍdzy wartoúciami kluczowymi dla parametru w zakresie 0..1 z zawiniÍciem.
/**
- Dzia≥a dla typÛw T, dla ktÛrych zdefiniowana jest funkcja interpolujπca (t = 0..1):
  void Lerp(T *Out, const T &v1, const T &v2, float t);
- Jako parametr t, zarÛwno w Items jak i do metody Calc, podawaÊ liczbÍ z zakresu 0..1.
- Wartoúci Items moøna swobodnie zmieniaÊ.
- Dla pustej tablicy Items zwraca zawsze T().
*/
template <typename T>
class RoundInterpolator
{
public:
	struct ITEM
	{
		float t;
		T Value;

		ITEM() { }
		ITEM(float t, const T &Value) : t(t), Value(Value) { }
	};

	std::vector<ITEM> Items;

	RoundInterpolator() : m_LastUsedIndex(0) { }
	void Calc(T *Out, float t) const;

private:
	mutable uint m_LastUsedIndex;
};

template <typename T>
void RoundInterpolator<T>::Calc(T *Out, float t) const
{
	assert(t >= 0.0f && t <= 1.0f);

	if (Items.empty())
	{
		*Out = T();
		return;
	}

	// Znajdü index poprzedniego i nastÍpnego Item

	// Na poczπtek weü zapamiÍtany indeks i sprawdü czy jest poprawny, jeúli nie to wrÛÊ do zerowego.
	uint Index = m_LastUsedIndex;
	// - Indeks poza zakresem
	if (Index >= Items.size())
		Index = 0;
	// - Z≥y czas po lewej
	if (Items[Index].t > t)
		Index = 0;

	// Przechodü dalej
	while (Index < Items.size()-1 && Items[Index+1].t < t)
		Index++;

	// ZapamiÍtaj indeks
	m_LastUsedIndex = Index;

	if (Index < Items.size()-1)
	{
		float my_t = (t - Items[Index].t) / (Items[Index+1].t - Items[Index].t);
		RoundInterpolatorLerp(Out, Items[Index].Value, Items[Index+1].Value, my_t);
	}
	else
	{
		float my_t = (t - Items[Index].t) / (Items[0].t + 1.0f - Items[Index].t);
		RoundInterpolatorLerp(Out, Items[Index].Value, Items[0].Value, my_t);
	}
}

/** \addtogroup base_perlin_noise Szum Perlina
Deterministyczne, pseudolosowe funkcje 1, 2, 3 zmiennych generujπce szum perlina
oraz czÍsto mylony z nim Fractional Brownian Motion (fBm). Szum perlina to
ciπg≥a funkcja pseudolosowa o amplitudzie -1..1 i czÍstotliwoúci 1.
W funkcjach BrownianNoise:
- i - liczba oktaw
- Persistence - utrzymanie amplitudy w kolejnych oktawach; zalecane 0..1*/
//@{
float PerlinNoise1(float x);
float PerlinNoise2(float x, float y);
float PerlinNoise3(float x, float y, float z);
float BrownianNoise1(float x,                   uint i, float Persistence);
float BrownianNoise2(float x, float y,          uint i, float Persistence);
float BrownianNoise3(float x, float y, float z, uint i, float Persistence);
//@}

/** \addtogroup base_math DEC3
Format, ktÛry w liczbie 32-bitowej uint koduje 4 komponenty uint jako W2Z10Y10X10.
Komponenty x, y, z muszπ byÊ w zakresie 10 bitÛw (0..0x03FF, tzn. 0..1023),
a komponent w w zakresie 2 bitÛw (0..0x03, tzn. 0..3).
*/
//@{

inline uint MakeDEC3(uint x, uint y, uint z)
{
	return (x & 0x03FF) | ((y & 0x03FF) << 10) | ((z & 0x03FF) << 20);
}
inline uint MakeDEC3(uint x, uint y, uint z, uint w)
{
	return (x & 0x03FF) | ((y & 0x03FF) << 10) | ((z & 0x03FF) << 20) | ((w & 0x03) << 30);
}
inline uint GetXFromDEC3(uint dec3) { return (dec3      ) & 0x03FF; }
inline uint GetYFromDEC3(uint dec3) { return (dec3 >> 10) & 0x03FF; }
inline uint GetZFromDEC3(uint dec3) { return (dec3 >> 20) & 0x03FF; }
inline uint GetWFromDEC3(uint dec3) { return (dec3 >> 30);          }

//@}

/// Calculates coefficients for linear equation y = A*x + B passing through given points (x1,y1), (x2,y2).
/** \return false if calculation not possible because x1 is equal or too close to x2. */
template <typename T>
bool CalcLinearFactors(
	T &outA, T &outB,
	float x1, float x2,
	const T &y1, const T &y2)
{
	float d = x2-x1;
	if (FLOAT_ALMOST_ZERO(d))
		return false;
	outA = (y2 - y1) / d;
	outB = (y1*x2 - y2*x1) / d;
	return true;
}

/// Calculates coefficients for quadratic equation y = A*x^2 + B*x + C passing through given points (x1,y1), (x2,y2), (x3,y3).
/** \return false if calculation not possible because some x arguments are equal or too close to each other. */
template <typename T>
bool CalcQuadraticFactors(
	T &outA, T &outB, T &outC,
	float x1, float x2, float x3,
	const T &y1, const T &y2, const T &y3)
{
	float x1_sq = x1 * x1;
	float x2_sq = x2 * x2;
	float x3_sq = x3 * x3;
	float w = x1_sq*(x2-x3) + x2_sq*(x3-x1) + x3_sq*(x1+x2);
	if (FLOAT_ALMOST_ZERO(w))
		return false;
	outA = ( y1*(x2-x3) + y2*(x3-x1) + y3*(x1+x2) ) / w;
	outB = ( y1*(x3_sq-x2_sq) + y2*(x1_sq-x3_sq) + y3*(x2_sq-x1_sq) ) / w;
	outC = ( y1*(x2_sq*x3-x3_sq*x2) + y2*(x3_sq*x1-x1_sq*x3) + y3*(x1_sq*x2-x2_sq*x1) ) / w;
	return true;
}

//@}


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
/** \addtogroup base_strings £aÒcuchy */
//@{

/// Rodzaje znakÛw koÒca wiersza
enum EOLMODE
{
	EOL_NONE, ///< Podczas konwersji pozostawienie bez zmian
	EOL_CRLF, ///< 13 i 10 (Windows)
	EOL_LF,   ///< 10      (Unix)
	EOL_CR	  ///< 13      (Mac)
};

/// £aÒcuch pusty - do wykorzystania aby moøna by≥o pobieraÊ do niego referencje itp.
extern const tstring EMPTY_STRING;
/// Standardowy ≥aÒcuch koÒca wiersza zaleøny od systemu
extern const tchar * const EOL;

/// Strona kodowa polskich znakow
enum CHARSET
{
	CHARSET_NONE = 0, ///< Brak polskich liter
	CHARSET_WINDOWS,  ///< Strona kodowa Windows-1250 (uøywana w GUI Windows)
	CHARSET_ISO,      ///< Strona kodowa ISO-8859-2 (Latin-2) (uøywana w Linuksie)
	CHARSET_IBM,      ///< Strona kodowa IBM (CP852) (uøywana na konsoli Windows)
	CHARSET_UTF8,     ///< Strona kodowa UTF-8
};

/// Zwraca true, jesli znak jest alfanumeryczny (litera lub cyfra) wg ustawieÒ systemu
bool CharIsAlphaNumeric(tchar ch);
/// Zwraca true, jesli znak jest litera
bool CharIsAlpha(tchar ch);
/// Zwraca true, jesli znak jest cyfrπ dziesietnπ
inline bool CharIsDigit(tchar ch) { return (ch >= _T('0') && ch <= _T('9')); }
/// Zwraca true, jeúli znak jest cyfrπ szesnastkowπ
/** - Akceptuje ma≥e i duøe litery. */
inline bool CharIsHexDigit(tchar ch) { return (ch >= _T('0') && ch <= _T('9')) || (ch >= _T('A') && ch <= _T('F')) || (ch >= _T('a') && ch <= _T('f')); }
/// Zwraca true, jesli znak jest mala litera
bool CharIsLower(tchar ch);
/// Zwraca true, jesli znak jest duza litera
bool CharIsUpper(tchar ch);
/// Zwraca true, jesli podany znak jest odstepem, czyli bialym znakiem wg systemu
/** Dwie wersje tej funkcji - ANSI i Unicode - sπ potrzebne dla Stream - HexDecoder. */
bool CharIsWhitespace(char ch);
#ifdef WIN32
bool CharIsWhitespace(wchar_t ch);
#endif
/// Zwraca true, jesli podany znak jest odstepem, czyli bialym znakiem
/** Czyli jednym ze znakow:
- <tt>0x09 (9)  "\\t"</tt> - tabulacja
- <tt>0x0A (10) "\\n"</tt> - znak konca wiersza
- <tt>0x0D (13) "\\r"</tt> - znak konca wiersza
- <tt>          "\\v"</tt> - tabulacja pionowa
- <tt>0x20 (32) " "</tt>  - spacja */
inline bool CharIsWhitespace_f(tchar ch)
{
	return (ch == _T(' ') || ch == _T('\n') || ch == _T('\r') || ch == _T('\t') || ch == _T('\v'));
}

/// Szuka w ≥aÒcuchu Str pod≥aÒcucha SubStr, bez rozrÛøniania wielkoúci liter.
/** Zwraca indeks/wskaünik do pierwszego znaku pierwszego znalezionego wystπpienia wewnπtrz str.
Jesli nie znaleziono, zwraca MAXUINT32. */
size_t StrStrI(const tstring &Str, const tstring &SubStr, size_t Count = 0xffffffff);
const tchar * StrStrI(const tchar *Str, const tchar *SubStr, size_t Count = 0xffffffff);
/// Odwraca ≥aÒcuch w miejscu
void ReverseString(tstring *s);
/// Obcina bia≥e znaki z poczπtku i koÒca ≥aÒcucha w miejscu
void Trim(tstring *s);
/// Obcina bia≥e znaki z poczπtku i koÒca ≥aÒcucha
void Trim(tstring *Out, const tstring &s);

// UWAGA! Z nieznanych przyczyn UpperCase i LowerCase üle konwertuje w stronie
// kodowej IBM. Lepiej najpierw zmieniÊ wielkoúÊ liter w innej stronie a potem
// przekonwertowaÊ na IBM - wtedy dzia≥a. Kiedyú moøe naprawiÍ ten b≥ad :/

/// Jeúli to litera, konwertuje na ma≥π
/** - Nie obs≥uguje wielobajtowych stron kodowych (UTF-8). */
tchar CharToLower(tchar ch);
/// Jeúli to litera, konwertuje na duøπ
/** - Nie obs≥uguje wielobajtowych stron kodowych (UTF-8). */
tchar CharToUpper(tchar ch);
/// Konwertuje ≥aÒcuch na ma≥e litery
/** - Nie obs≥uguje wielobajtowych stron kodowych (UTF-8). */
void LowerCase(tstring *s);
inline void LowerCase(tstring *Out, const tstring &s) { *Out = s; LowerCase(Out); }
/// Konwertuje ≥aÒcuch na duøe
/** - Nie obs≥uguje wielobajtowych stron kodowych (UTF-8). */
void UpperCase(tstring *s);
inline void UpperCase(tstring *Out, const tstring &s) { *Out = s; UpperCase(Out); }

/// Zmienia znak specjalny kodowania Windows-1250 na odpowiednik normalny.
/** Jesli podany znak jest jednym ze znakow specjalnych Windows-1250,
zwraca przez a_s jego normalny odpowiednik (moøna podaÊ wskaünik 0) i zwraca true.
Jeúli to nie jest taki znak, zwraca false. */
bool Charset_WindowsSpecialChar(char a_c, string *a_s);
/// Zmienia znak z jednej strony kodowej na druga lub pozostawia bez zmian.
/** Nie obsluguje stron kodowych wielobajtowych, tzn. UTF-8
(wynik jest wtedy niezdefiniowany). */
char Charset_Convert_Char(char a_c, CHARSET a_Charset1, CHARSET a_Charset2);
/// Konwertuje lancuch na podana strone kodowa
/** \param[out] out nie moze byc tym samym lancuchem, co s. */
void Charset_Convert(string *out, const string &s, CHARSET Charset1, CHARSET Charset2);

/// Szyforowanie/deszyfrowanie algorytmem ROT13
void Rot13(tstring *InOut);
inline void Rot13(tstring *Out, const tstring &In) { *Out = In; Rot13(Out); }

/// Rodzaj znaku koÒca wiersza na odpowiadajπcy mu znak (lub znaki)
void EolModeToStr(tstring *Out, EOLMODE EolMode);

/// Zamienia w ≥aÒcuchu wszystkie wystπpienia jedego pod≥aÒcucha na drugi
/** Result musi byÊ oczywiúcie, jak w kaødej takiej funkcji, innym ≥aÒcuchem niø s. */
void Replace(tstring *result, const tstring &s, const tstring &s1, const tstring &s2);
/// Zmienia w ≥aÒcuchu wszystkie wystπpienia znaku Ch1 na Ch2
void Replace(tstring *Out, const tstring &s, tchar Ch1, tchar Ch2);
/// Zmienia w ≥aÒcuchu wszystkie wystπpienia znaku Ch1 na Ch2 w miejscu
void Replace(tstring *InOut, tchar Ch1, tchar Ch2);

/// Zmienia wszelkiego rodzaju znaki koÒca wiersza w poddanym tekúcie na takie same - wg podanego trybu
void ReplaceEOL(tstring *result, const tstring &s, EOLMODE EOLMode);

/// Zamienia wszystkie odstÍpy na pojedyncze spacje
void NormalizeWhitespace(tstring *result, const tstring &s);

/// Zwraca ≥aÒcuch powtÛrzony podanπ liczbÍ razy
void DuplicateString(tstring *Out, const tstring &s, size_t count);

/// Zwraca pod≥aÒcuch z prawej strony
void RightStr(tstring *Out, const tstring &s, size_t Length);

/// Zwraca liczbÍ wystπpieÒ pod≥aÒcucha
size_t SubstringCount(const tstring &str, const tstring &substr);

/// PorÛwnuje dwa ≥aÒcuchy bez rozrÛøniania wielkoúci liter.
/** Zwraca liczbÍ ujemnπ, 0 lub dodatniπ zaleønie od wyniku porÛwnania. */
int StrCmpI(const tstring &s1, const tstring &s2, size_t Count = 0xffffffff);
int StrCmpI(const tchar *s1, const tchar *s2, size_t Count = 0xffffffff);
/// Zwraca true, jeúli podfragmenty podanych ≥aÒcuchÛw sπ identyczne
int SubStrCmp(const tstring &s1, size_t off1, const tstring &s2, size_t off2, size_t length);
/// Zwraca true, jeúli podfragmenty podanych ≥aÒcuchÛw sπ identyczne nie rozrÛøniajπc wielkoúci liter
int SubStrCmpI(const tstring &s1, size_t off1, const tstring &s2, size_t off2, size_t length);

/// Czy ≥aÒcuch zawiera znaki koÒca wiersza?
bool ContainsEOL(const tstring &s);

/// Zwraca true, jesli podany lancuch zaczyna sie od podanego podlancucha
bool StrBegins(const tstring &s, const tstring &sub, bool caseSensitive, size_t Begin = 0);
/// Zwraca true, jeúli podany ≥aÒcuch koÒczy siÍ podanym pod≥aÒcuchem
bool StrEnds(const tstring &s, const tstring &Sub, bool caseSensitive);

/// Kolejne wywo≥ania rozdzielajπ ≥aÒcuch na kolejne elementy oddzielone w jego treúci okreúlonym pod≥aÒcuchem
/** \param[out] out
\param[in,out] index */
bool Split(const tstring &s, const tstring &delimiter, tstring *out, size_t *index);
/// Dzia≥a jak Split(), ale ≥aÒcuchem rozdzielajπcym jest kaødy dowolny znak z delimiters
bool SplitFirstOf(const tstring &s, const tstring &delimiters, tstring *out, size_t *index);
/// Dzieli ≥aÒcuch na czÍúci i zwraca je jako wektor
void Split(STRING_VECTOR *Out, const tstring &s, const tstring &Delimiter);
/// Kolejne wywo≥ania rozdzielajπ ≥aÒcuch na kolejne elementy oddzielone w jego treúci ktÛrπú z sekwencji uznawanπ za koniec wiersza (czyli na kolejne wiersze)
bool SplitEOL(const tstring &s, tstring *out, size_t *index);
/// Kolejne wywo≥ania rozdzielajπ ≥aÒcuch na elementy oddzielone odstÍpami uwzglÍdniajπc ciπgi obiÍte w cudzys≥owy <tt>""</tt> jako ca≥oúÊ
bool SplitQuery(const tstring &s, tstring *out, size_t *index);
/// Sprawdza, czy podany ≥ancuch jest zgodny z podanπ maskπ mogπcπ zawieraÊ znaki wieloznaczne
/** '?' zastÍpuje dowolny jeden znak, '*' zastÍpuje dowolnπ liczbÍ (takøe 0) dowolnych znakÛw. */
bool ValidateWildcard(const tstring &Mask, const tstring &S, bool CaseSensitive = true, size_t MaskOff = 0, size_t SOff = 0);
/// Zwraca zmiennoprzecinkowπ trafnoúÊ wyszukiwania stringa SubStr w stringu Str
/** Zaawansowany algorytm, wymyúlony przeze mnie dawno temu. Bierze pod uwagÍ
rzeczy takie jak:
-# ile razy pod≥aÒcuch wystÍpuje
-# czy pasuje wielkoúÊ liter
-# d≥ugoúÊ ≥aÒcucha i pod≥aÒcucha
-# czy to ca≥e s≥owo
-# czy to ca≥y ≥aÒcuch. */
float FineSearch(const tstring &SubStr, const tstring &Str);
/// Odleg≥oúÊ edycyjna miÍdzy dwoma ≥aÒcuchami.
/** Z≥oøonoúÊ: O( s1.length * s2.length ).
Im mniejsza, tym ≥aÒcuchy bardziej podobne. Jeúli identyczne, zwraca 0. */
size_t LevenshteinDistance(const tstring &s1, const tstring &s2);
/// Odleg≥oúÊ edycyjna miÍdzy dwoma ≥aÒcuchami bez uwzglÍdniania wielkoúci liter.
/** Z≥oøonoúÊ: O( s1.length * s2.length ).
Im mniejsza, tym ≥aÒcuchy bardziej podobne. Jeúli identyczne, zwraca 0. */
size_t LevenshteinDistanceI(const tstring &s1, const tstring &s2);

/** \addtogroup base_natural_sort Sortowanie naturalne
Sortowanie naturalne, czyli takie, w ktÛrym np. "abc2" jest przed "abc120". */
//@{

/// Klasa do porÛwnywania w porzπdku naturalnym.
class StringNaturalCompare
{
private:
	bool m_CaseInsensitive;

	int CompareNumbers(const tstring &s1, size_t i1, const tstring &s2, size_t i2);

public:
	StringNaturalCompare(bool CaseInsensitive) : m_CaseInsensitive(CaseInsensitive) { }
	int Compare(const tstring &s1, const tstring &s2);
};

/// Predykat do sortowania ≥aÒcuchÛw w porzπdku naturalnym
/** Przyk≥ad uøycia:
\code
std::vector<string> v;
std::sort(v.begin(), v.end(), common::StringNaturalLess());
\endcode
*/
class StringNaturalLess : public StringNaturalCompare
{
public:
	StringNaturalLess(bool CaseInsensitive = true) : StringNaturalCompare(CaseInsensitive) { }
	bool operator () (const tstring &s1, const tstring &s2) { return (Compare(s1, s2) < 0); }
};

/// Predykat do sortowania ≥aÒcuchÛw w porzπdku naturalnym
class StringNaturalGreater : public StringNaturalCompare
{
public:
	StringNaturalGreater(bool CaseInsensitive = true) : StringNaturalCompare(CaseInsensitive) { }
	bool operator () (const tstring &s1, const tstring &s2) { return (Compare(s1, s2) > 0); }
};

//@}

/// Predykat do porÛwnywania ≥aÒcuchÛw bez rozrÛøniania wielkoúci liter.
class StrLessI
{
public:
	bool operator () (const tstring &s1, const tstring &s2) { return StrCmpI(s1, s2) < 0; }
};
/// Predykat do porÛwnywania ≥aÒcuchÛw bez rozrÛøniania wielkoúci liter.
class StrGreaterI
{
public:
	bool operator () (const tstring &s1, const tstring &s2) { return StrCmpI(s1, s2) > 0; }
};

/** \addtogroup base_cstr C-style strings */
//@{

/// Zwraca true, jeúli podany ≥aÒcuch jest NULL lub zawiera 0 znakÛw (od razu ma koÒczπce '\0').
inline bool StrIsEmpty(const char *s) { return s == NULL || *s == '\0'; }
/// Czyúci ≥aÒcuch wpisujπc samo koÒczπce '\0'.
inline void ClearStr(char *s) { assert(s); *s = '\0'; }

#ifdef WIN32
	inline bool StrIsEmpty(const wchar_t *s) { return s == NULL || *s == L'\0'; }
	inline void ClearStr(wchar_t *s) { assert(s); *s = L'\0'; }
#endif

/// Szuka w ≥aÒcuchu str pod≥aÒcucha substr, bez rozrÛøniania wielkoúci liter.
/** Zwraca wskaünik do pierwszego znaku pierwszego znalezionego wystπpienia wewnπtrz str.
Jesli nie znaleziono, zwraca NULL. */
const tchar * strnistr(const tchar *str, const tchar *substr, size_t count = MAXUINT32);
// Zwraca true, jeúli ≥aÒcuch str zaczyna siÍ od ≥aÒcucha substr
bool StrBegins(const tchar *str, const tchar *substr, bool caseSensitive);
/// Zwraca true, jeúli ≥aÒcuch str koÒczy siÍ na ≥aÒcuch substr
bool StrEnds(const tchar *str, const tchar *substr, bool caseSensitive);
/// Usuwa podany zakres znakÛw z ≥aÒcucha. Jest bezpieczny na przekroczenie zakresu.
void StrErase(tchar *str, size_t off, size_t count = MAXUINT32);
/// Wstawia podany pod≥aÒcuch do ≥aÒcucha w podane miejsce.
void StrInsert(tchar *str, size_t strCapacity, const tchar *newStr, size_t off, size_t count);
void StrInsert(tchar *str, size_t strCapacity, const tchar *newStr, size_t off);
/// Obcina z poczπtku i koÒca stringa bia≥e znaki (w miejscu)
void Trim(tchar *s);
/// Replaces in string src all "from" chars to "to" chars. Returns number of occurences.
unsigned StrReplace(tchar *str, tchar from, tchar to);

//@}
// base_cstr


/** \addtogroup base_unicode Obs≥uga Unicode
DostÍpne tylko w Windows. */
//@{

#ifdef WIN32

/** \addtogroup base_unicode_convs Konwersje ≥aÒcuchÛw Unicode

Konwersje miÍdzy ≥aÒcuchem Unicode (UTF-16) a ≥aÒcuchem w innym kodowaniu - Single Byte lub Multi Byte.

CodePage to kodowanie znakÛw typu char. Przyk≥adowe wartoúci:
  - Sta≥e: CP_UTF8 (UTF-8), CP_ACP (natywna, 1-bajtowa strona kodowa systemu).
  - Numery takie jak 1250 dla kodowania Windows-1250 (p. MSDN > "Code Page Identifiers").

Funkcje zwracajπ false w przypadku niepowodzenia konwersji. WÛwczas Out jest pusty.
*/
//@{

bool ConvertUnicodeToChars(string *Out, const wstring &S, unsigned CodePage);
bool ConvertUnicodeToChars(string *Out, const wchar_t *S, unsigned NumChars, unsigned CodePage);

bool ConvertCharsToUnicode(wstring *Out, const string &S, unsigned CodePage);
bool ConvertCharsToUnicode(wstring *Out, const char *S, unsigned NumChars, unsigned CodePage);	

/// Oblicza liczbÍ znakÛw (nie bajtÛw!) potrzebnπ przy konwersji miÍdzy Unicode i ANSI.
/** B≥πd: Zwraca false, outLen ustawia na 0. */
bool UnicodeToChars_CalcBufLen(size_t &outLen, const wchar_t *wcs, unsigned CodePage);
bool UnicodeToChars_CalcBufLen(size_t &outLen, const wchar_t *wcs, size_t wcsLen, unsigned CodePage);
bool CharsToUnicode_CalcBufLen(size_t &outLen, const char *str, unsigned CodePage);
bool CharsToUnicode_CalcBufLen(size_t &outLen, const char *str, size_t strLen, unsigned CodePage);

/// Konwertuje ≥aÒcuch miÍdzy Unicode i ANSI. Nie do≥πcza koÒczπcego zera.
/** B≥πd: zwraca false, outBuf ustawia na ≥aÒcuch pusty. */
bool ConvertUnicodeToChars(char *outBuf, size_t bufLen, const wchar_t *wcs, unsigned CodePage);
bool ConvertUnicodeToChars(char *outBuf, size_t bufLen, const wchar_t *wcs, size_t wcsLen, unsigned CodePage);
bool ConvertCharsToUnicode(wchar_t *outBuf, size_t bufLen, const char *str, unsigned CodePage);
bool ConvertCharsToUnicode(wchar_t *outBuf, size_t bufLen, const char *str, size_t strLen, unsigned CodePage);

//@}

/** \addtogroup base_unicode_headers Nag≥Ûwki Unicode
£aÒcuchy zakoÒczone zerem - nag≥Ûwki poszczegÛlnych formatÛw Unicode.
(Standardowo uøywany w Windows jest UTF-16 Little Endian.) */
//@{
extern const char * const BOM_UTF8;
extern const char * const BOM_UTF16_LE;
extern const char * const BOM_UTF16_BE;
//@}

#endif

/** \addtogroup base_unicode_conv_functions Konwersje miÍdzy tstring, string, wstring
W przypadku niepowodzenia zwracajπ false i Out jest wtedy pusty. */
//@{
#ifdef _UNICODE
	bool TstringToString(string *Out, const tstring &S);
	bool StringToTstring(tstring *Out, const string &S);
	inline bool TstringToWstring(wstring *Out, const tstring &S) { *Out = S; return true; }
	inline bool WstringToTstring(tstring *Out, const wstring &S) { *Out = S; return true; }

	inline string TstringToStringR(const tstring &S) { string R;  if (!TstringToString(&R, S)) return string();  return R; }
	inline tstring StringToTstringR(const string &S) { tstring R; if (!StringToTstring(&R, S)) return tstring(); return R; }
	inline wstring TstringToWstring(const tstring &S) { return S; }
	inline tstring WstringToTstring(const wstring &S) { return S; }
#else
	inline bool TstringToString(string *Out, const tstring &S) { *Out = S; return true; }
	inline bool StringToTstring(tstring *Out, const string &S) { *Out = S; return true; }
	inline string TstringToStringR(const tstring &S) { return S; }
	inline tstring StringToTstringR(const string &S) { return S; }
	#ifdef WIN32
		bool TstringToWstring(wstring *Out, const tstring &S);
		bool WstringToTstring(tstring *Out, const wstring &S);
		inline wstring TstringToWstringR(const tstring &S) { wstring R; if (!TstringToWstring(&R, S)) return wstring(); return R; }
		inline tstring WstringToTstringR(const wstring &S) { tstring R; if (!WstringToTstring(&R, S)) return tstring(); return R; }
	#endif
#endif
//@}
// Konwersje miÍdzy tstring, string, wstring

//@}
// Obs≥uga Unicode

//@}
// £aÒcuchy

//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
/** \addtogroup base_paths åcieøki do plikÛw */
//@{

#ifdef WIN32
	/// Separator katalogÛw w úcieøce, zaleøny od platformy.
	const tchar DIR_SEP = _T('\\');
#else
	const tchar DIR_SEP = _T('/');
#endif

/// Zwraca true, jeúli úcieøka jest bezwzglÍdna
bool PathIsAbsolute(const tstring &s);
/// Dodaje do sciezki konczacy '/' lub '\', jesli go nie ma - na miejscu
void IncludeTrailingPathDelimiter(tstring *InOutPath);
/// Dodaje do sciezki konczacy '/' lub '\', jesli go nie ma - do nowego stringa
void IncludeTrailingPathDelimiter(tstring *OutPath, const tstring &InPath);
/// Pozbawia sciezke konczacego '/' lub '\', jesli go ma - na miejscu
void ExcludeTrailingPathDelimiter(tstring *InOutPath);
/// Pozbawia sciezke konczacego '/' lub '\', jesli go ma - do nowego stringa
void ExcludeTrailingPathDelimiter(tstring *OutPath, const tstring &InPath);
/// Zwraca poczπtek úcieøki, w postaci takiej jak "C:\", "\\komputer\udzia≥\" czy "/" albo "\"
/** Jeúli úcieøka jest wzglÍdna, zwraca ≥aÒcuch pusty. */
void ExtractPathPrefix(tstring *OutPrefix, const tstring &s);
/// Zwraca úcieøkÍ do pliku bez nazwy pliku
void ExtractFilePath(tstring *OutPath, const tstring &s);
/// Zwraca samπ nazwÍ pliku bez úcieøki
void ExtractFileName(tstring *OutFileName, const tstring &s);
/// Zwraca rozszerzenie pliku wraz z kropkπ
void ExtractFileExt(tstring *OutExt, const tstring &s);
/// Zmienia w nazwie pliku rozszerzenie na nowe
/** Jesli Ext = "", usuwa rozszerzenie.
Nowe rozszerzenie musi zawieraÊ rozpoczynajπcπ kropkÍ (tzn. nie musi, ale wypada≥oby :) */
void ChangeFileExt(tstring *Out, const tstring &FileName, const tstring &Ext);
/// Pozbawia sciezke artefaktow typu ".\\" czy "..\\"
/** Odpowiednio ja oczywiscie przetwarza tak, ze wyjscie jest logicznie rownowazne wejsciu. */
void NormalizePath(tstring *OutPath, const tstring &s);
/// Przetwarza sciezke wzgledna na bezwzgledna wzgledem okreslonej sciezki
/** Na przyk≥ad Base="C:\\Kat", Path="PodKat\\Plik.txt", Wynik="C:\\Kat\\PodKat\\Plik.txt". */
void RelativeToAbsolutePath(tstring *Out, const tstring &Base, const tstring &Path);
/// Przetwarza sciezke bezwzgledna na wzgledna wzgledem okreslonej sciezki
/** Jeúli siÍ nie da, wychodzi rÛwnieø bezwzglÍdna, rÛwna Target.
Na przyk≥ad Base="C:\\Kat", Tareget="C:\\Kat\\Podkat\\Plik.txt", Wynik="PodKat\\Plik.txt". */
void AbsoluteToRelativePath(tstring *Out, const tstring &Base, const tstring &Target);

//@}
// åcieøki do plikÛw


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
/** \addtogroup base_conversions Konwersje miÍdzy ≥aÒcuchem a rÛønymi typami */
/**
Stara≥em siÍ zagwarantowaÊ w Windows (w Linux jeszcze nie - moøe kiedyú...),
øeby konwersje zawsze dzia≥a≥y tak samo niezaleønie od locale ustawionego w
bibliotece standardowej C (funkcja setlocale), C++ (klasa locale) czy WinAPI
(funkcja SetThreadLocale). SzczegÛlnie konwersje miÍdzy liczbπ
zmiennoprzecinkowπ a ≥aÒcuchem zawsze uøywajπ kropki, a nie przecinka.
*/
//@{

/** \internal */
extern const tchar _DIGITS_L[];
/** \internal */
extern const tchar _DIGITS_U[];

/// Zwraca liczbowy odpowiednik cyfry szesnastkowej.
/** Jeúli b≥πd, zwraca 0xFF.
Akceptuje zarÛwno ma≥e, jak i duøe litery.
(Dwie wersje tej funkcji - osobno dla ANSI i Unicode - sπ potrzebne w Stream - HexDecoder.) */
inline int HexDigitToNumber(char Ch)
{
	if      (Ch >= '0' && Ch <= '9') return (int)(uint8)(Ch -  '0');
	else if (Ch >= 'A' && Ch <= 'F') return (int)(uint8)(Ch - ('A' - 10));
	else if (Ch >= 'a' && Ch <= 'f') return (int)(uint8)(Ch - ('a' - 10));
	else return 0xFF;
}
#ifdef WIN32
inline int HexDigitToNumber(wchar_t Ch)
{
	if      (Ch >= L'0' && Ch <= L'9') return (int)(Ch -  L'0');
	else if (Ch >= L'A' && Ch <= L'F') return (int)(Ch - (L'A' - 10));
	else if (Ch >= L'a' && Ch <= L'f') return (int)(Ch - (L'a' - 10));
	else return 0xFF;
}
#endif

/// Konwersja liczby ca≥kowitej na ≥aÒcuch
/** \param Base musi byÊ z zakresu 2..36 */
template <typename T>
void UintToStr(tstring *Out, T x, uint32 Base = 10, bool UpperCase = true)
{
	if (x == 0)
	{
		Out->assign(_T("0"));
		return;
	}

	Out->clear();
	Out->reserve(8);

	// Podstawa to potÍga dwÛjki - bÍdzie proúciej, bo przesuwanie bitowe zamiast dzielenia
	if (IsPow2<uint32>(Base))
	{
		// O ile bitÛw przesuwaÊ?
		uint32 Bits = log2u(Base);
		// Maska bitowa do &-owania
		uint32 BitMask = GetBitMask(Bits);
		// Jazda!
		if (UpperCase)
		{
			while (x != 0)
			{
				*Out += _DIGITS_U[x & BitMask];
				x = x >> Bits;
			}
		}
		else
		{
			while (x != 0)
			{
				*Out += _DIGITS_L[x & BitMask];
				x = x >> Bits;
			}
		}
	}
	// Podstawa to nie potÍga dwÛjki
	else
	{
		if (UpperCase)
		{
			while (x != 0)
			{
				*Out += _DIGITS_U[x % Base];
				x /= Base;
			}
		}
		else
		{
			while (x != 0)
			{
				*Out += _DIGITS_L[x % Base];
				x /= Base;
			}
		}
	}

	ReverseString(Out);
}

template <typename T>
void IntToStr(tstring *Out, T x, uint32 Base = 10, bool UpperCase = true)
{
	if (x == 0)
	{
		Out->assign(_T("0"));
		return;
	}

	Out->clear();
	Out->reserve(8);

	// Liczba dodatnia - identycznie jak w UintToStr
	if (x > 0)
	{
		// Podstawa to potÍga dwÛjki - bÍdzie proúciej, bo przesuwanie bitowe zamiast dzielenia
		if (IsPow2<uint32>(Base))
		{
			// O ile bitÛw przesuwaÊ?
			uint32 Bits = log2u(Base);
			// Maska bitowa do &-owania
			uint32 BitMask = GetBitMask(Bits);
			// Jazda!
			if (UpperCase)
			{
				while (x != 0)
				{
					*Out += _DIGITS_U[x & BitMask];
					x = x >> Bits;
				}
			}
			else
			{
				while (x != 0)
				{
					*Out += _DIGITS_L[x & BitMask];
					x = x >> Bits;
				}
			}
		}
		// Podstawa to nie potÍga dwÛjki
		else
		{
			if (UpperCase)
			{
				while (x != 0)
				{
					*Out += _DIGITS_U[x % Base];
					x /= Base;
				}
			}
			else
			{
				while (x != 0)
				{
					*Out += _DIGITS_L[x % Base];
					x /= Base;
				}
			}
		}
	}
	// Liczba ujemna - tu bÍdzie musia≥o byÊ inaczej
	// Modulo jest UB, wiÍc uøyjemy obejúcia.
	else
	{
		T x_div_base;
		if (UpperCase)
		{
			while (x != 0)
			{
				x_div_base = x/(T)Base;
				*Out += _DIGITS_U[- (x - x_div_base*(T)Base)];
				x = x_div_base;
			}
		}
		else
		{
			while (x != 0)
			{
				x_div_base = x/(T)Base;
				*Out += _DIGITS_L[- (x - x_div_base*(T)Base)];
				x = x_div_base;
			}
		}
		*Out += '-';
	}

	ReverseString(Out);
}

/// Konwertuje znak cyfry w dowolnym systemie ('0'..'9', 'A'..'Z', 'a'..'z') na liczbÍ, zwraca false jeúli b≥πd.
/** \internal */
template <typename T>
bool _CharToDigit(T *Digit, tchar ch)
{
	if (ch >= _T('0') && ch <= _T('9'))
		*Digit = static_cast<T>(ch-_T('0'));
	else if (ch >= _T('A') && ch <= _T('Z'))
		*Digit = static_cast<T>(ch-_T('A')+10);
	else if (ch >= _T('a') && ch <= _T('z'))
		*Digit = static_cast<T>(ch-_T('a')+10);
	else
		return false;
	return true;
}

/// Konwersja ≥aÒcucha na liczbÍ ca≥kowitπ
/** \return Zwraca:
-  0 - sukces
- -1 - b≥πd sk≥adni ≥aÒcucha
- -2 - przekroczenie zakresu */
template <typename T>
int StrToUint(T *Number, const tstring &str, unsigned Base = 10)
{
	T BaseT = static_cast<T>(Base);
	if (str.empty()) return -1;
	*Number = 0;
	T Digit, LastNumber = 0, Limit = std::numeric_limits<T>::max()/BaseT;
	for (size_t i = 0; i < str.length(); ++i) {
		if (!_CharToDigit(&Digit, str[i])) return -1;
		if (Digit >= BaseT)  return -1;
		if (*Number > Limit) return -2;
		*Number = *Number * BaseT + Digit;
		if (*Number < LastNumber) return -2;
		LastNumber = *Number;
	}
	return 0;
}

template <typename T>
int StrToInt(T *Number, const tstring &str, unsigned Base = 10)
{
	T BaseT = static_cast<T>(Base);
	if (str.empty()) return -1;
	*Number = 0;
	size_t i = 0;
	int Sign = +1;
	if (str[0] == '+') i = 1;
	else if (str[0] == '-') { Sign = -1; i = 1; }
	T Digit, LastNumber = 0, Limit;
	if (Sign > 0) Limit = std::numeric_limits<T>::max()/BaseT;
	else          Limit = std::numeric_limits<T>::min()/BaseT;
	for (; i < str.length(); ++i) {
		if (!_CharToDigit(&Digit, str[i])) return -1;
		if (Digit >= BaseT) return -1;
		if (Sign > 0) { if (*Number > Limit) return -2; }
		else          { if (*Number < Limit) return -2; }
		*Number = *Number * BaseT + Digit * Sign;
		if (Sign > 0) { if (*Number < LastNumber) return -2; }
		else          { if (*Number > LastNumber) return -2; }
		LastNumber = *Number;
	}
	return 0;
}

template <typename T> inline tstring IntToStrR (T x, int base = 10, bool UpperCase = true) { tstring r; IntToStr<T> (&r, x, base, UpperCase); return r; }
template <typename T> inline tstring UintToStrR(T x, int base = 10, bool UpperCase = true) { tstring r; UintToStr<T>(&r, x, base, UpperCase); return r; }

inline void Size_tToStr(tstring *Out, size_t x, size_t Base = 10, bool UpperCase = true) { UintToStr(Out, (uint32)x, Base, UpperCase); }
inline tstring Size_tToStrR(size_t x, size_t Base = 10, bool UpperCase = true) { tstring R; Size_tToStr(&R, x, Base, UpperCase); return R; }

/// Konwersja liczby na ≥aÒcuch o minimalnej podanej d≥ugoúci.
/** Zostanie do tej d≥ugoúci uzupe≥niony zerami. */
template <typename T>
void UintToStr2(tstring *Out, T x, unsigned Length, int base = 10)
{
	tstring Tmp;
	UintToStr(&Tmp, x, base);
	if (Tmp.length() >= Length)
		*Out = Tmp;
	else
	{
		Out->clear();
		Out->reserve(std::max(Tmp.length(), Length));
		for (size_t Left = Length - Tmp.length(); Left > 0; Left--)
			*Out += _T('0');
		*Out += Tmp;
	}
}
template <typename T>
void IntToStr2(tstring *Out, T x, unsigned Length, int base = 10)
{
	tstring Tmp;
	IntToStr<T>(&Tmp, x, base);
	if (Tmp.length() >= Length)
		*Out = Tmp;
	else
	{
		Out->clear();
		Out->reserve(std::max(Tmp.length(), Length));
		assert(!Tmp.empty());
		if (Tmp[0] == _T('-'))
		{
			*Out += _T('-');
			for (size_t Left = Length - Tmp.length(); Left > 0; Left--)
				*Out += _T('0');
			Out->append(Tmp.begin()+1, Tmp.end());
		}
		else
		{
			for (size_t Left = Length - Tmp.length(); Left > 0; Left--)
				*Out += _T('0');
			*Out += Tmp;
		}
	}
}

template <typename T> tstring UintToStr2R(T x, unsigned Length, int Base = 10) { tstring R; UintToStr2<T>(&R, x, Length, Base); return R; }
template <typename T> tstring IntToStr2R (T x, unsigned Length, int Base = 10) { tstring R; IntToStr2<T> (&R, x, Length, Base); return R; }

/// Konwertuje znak na ≥aÒcuch, jako øe automatycznie to siÍ niestety nie odbywa
inline void CharToStr(tstring *Out, tchar ch) { Out->clear(); *Out += ch; }
inline tstring CharToStrR(tchar ch) { tstring s; s += ch; return s; }

/// Konwersja liczb zmiennoprzecinkowych na ≥aÒcuch
/** \param Mode:
- 'e' : -0.12345e-001
- 'E' : -0.12345E-001
- 'f' : -0.12345
- 'g' : optimum ('e', 'f')
- 'G' : optimum ('E', 'f')
\param Precision 0..20 */
void DoubleToStr(tstring *Out, double x, char Mode = 'g', int Precision = 6);
void FloatToStr(tstring *Out, float x, char Mode = 'g', int Precision = 6);

inline tstring DoubleToStrR(double x, char Mode = 'g', int Precision = 6) { tstring R; DoubleToStr(&R, x, Mode, Precision); return R; }
inline tstring FloatToStrR (float x, char Mode = 'g', int Precision = 6) { tstring R;  FloatToStr (&R, x, Mode, Precision); return R; }

/// Konwersja ≥aÒcucha na liczbÍ zmiennoprzecinkowπ.
/**
- W przypadku b≥Ídu sk≥adni zwracajπ wartoúÊ != 0.
- B≥Ídy zakresu nie sπ sprawdzane - wynik niezdefiniowany. */
int StrToDouble(double *out, const tstring &s);
int StrToFloat (float *out,  const tstring &s);

/// Konwertuje wartoúÊ logicznπ na ≥aÒcuch
/** \param mode:
- '0' : 0 / 1
- 'f' : false / true
- 'F' : False / True
- 'U' : FALSE / TRUE
- 'g' : f / t
- 'G' : F / T */
void BoolToStr(tstring *Out, bool x, char mode = 'f');
inline tstring BoolToStrR(bool x, char mode = 'f') { tstring R; BoolToStr(&R, x, mode); return R; }
/// Konwertuje ≥aÒcuch na wartoúÊ logicznπ
/**
- Dozwolone wszelkie wartoúci jak w BoolToStr.
- B≥πd: zwraca false. */
bool StrToBool(bool *result, const tstring &s);

/// Konwertuje wskaünik na ≥aÒcuch 8 znakÛw zapisany szesnastkowo
void PtrToStr(tstring *Out, const void* p);
inline tstring PtrToStrR(const void *p) { tstring R; PtrToStr(&R, p); return R; }

/// Liczba bajtÛw na ≥aÒcuch z rozmiarem np. "1 B", "10.5 kB"
/** Jako T uzywac uint8, uint16, uint32, uint64 lub int ale na pewno wartoúci dodaniej.
\param Space czy miÍdzy liczbÍ a jednostkÍ dawaÊ spacjÍ
\param Precision iloúÊ cyfr po przecinku (moøe byÊ teø 0) */
template <typename T>
void SizeToStr(tstring *str, T size, bool Space, int Precision)
{
	double size2 = (double)size;
	if (size2 >= 1024.0*1024.0*1024.0*1024.0)
	{
		DoubleToStr( str, size2/(1024.0*1024.0*1024.0*1024.0), 'f', Precision );
		str->append(Space ? _T(" TB") : _T("TB"));
	}
	else if (size2 >= 1024.0*1024.0*1024.0)
	{
		DoubleToStr( str, size2/(1024.0*1024.0*1024.0), 'f', Precision );
		str->append(Space ? _T(" GB") : _T("GB"));
	}
	else if (size2 >= 1024.0*1024.0)
	{
		DoubleToStr( str, size2/(1024.0*1024.0), 'f', Precision );
		str->append(Space ? _T(" MB") : _T("MB"));
	}
	else if (size2 >= 1024.0)
	{
		DoubleToStr( str, size2/1024.0, 'f', Precision );
		str->append(Space ? _T(" KB") : _T("KB"));
	}
	else
	{
		UintToStr( str, size );
		str->append(Space ? _T(" B") : _T("B"));
	}
}
template <typename T> tstring SizeToStrR(T Size, bool Space, int Precision) { tstring r; SizeToStr<T>(&r, Size, Space, Precision); return r; }

//@}
// Konwersje miÍdzy ≥aÒcuchem a rÛønymi typami


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
/** \addtogroup base_periodic_funcs Funkcje okresowe */
//@{

/*
Funkcje zwracajπce rÛønego rodzaju przebiegi okresowe.
Domyúlne (bez ustawianych w≥asnych parametrÛw) majπ zakres wyjúciowy 0..1,
czÍstotliwoúÊ 1 i fazÍ 0.
- Base      - dolne ograniczenie
- Amplitude - pe≥na wysokoúÊ
*/

/// Standard sine wave which smoothly changes from low to high and back again.
inline float WaveformSine(float x, float Base, float Amplitude, float Frequency, float Phase)
{
	return Base + (sinf((x + Phase) * Frequency * PI_X_2)+1.0f)*0.5f * Amplitude;
}
inline float WaveformSine(float x)
{
	return (sinf(x * PI_X_2)+1.0f)*0.5f;
}

/// An angular wave with a constant increase / decrease speed with pointed peaks.
inline float WaveformTriangle(float x, float Base, float Amplitude, float Frequency, float Phase)
{
	float Input = frac((x + Phase) * Frequency);
	float Output;
	if (Input < 0.25f)
		Output = Input * 4.0f;
	else if (Input < 0.75f)
		Output = 1.0f - ((Input - 0.25f) * 4.0f);
	else
		Output = ((Input - 0.75f) * 4.0f) - 1.0f;
	return Base + (Output+1.0f)*0.5f * Amplitude;
}
inline float WaveformTriangle(float x)
{
	float Input = frac(x);
	float Output;
	if (Input < 0.25f)
		Output = Input * 4.0f;
	else if (Input < 0.75f)
		Output = 1.0f - ((Input - 0.25f) * 4.0f);
	else
		Output = ((Input - 0.75f) * 4.0f) - 1.0f;
	return (Output+1.0f)*0.5f;
}

/// Half of the time is spent at the min, half at the max with instant transition between.
inline float WaveformSquare(float x, float Base, float Amplitude, float Frequency, float Phase)
{
	return Base + (frac((x + Phase) * Frequency) <= 0.5f ? 1.0f : 0.0f) * Amplitude;
}
inline float WaveformSquare(float x)
{
	return frac(x) <= 0.5f ? 1.0f : 0.0f;
}

/// Gradual steady increase from min to max over the period with an instant return to min at the end.
inline float WaveformSawtooth(float x, float Base, float Amplitude, float Frequency, float Phase)
{
	return Base + frac((x + Phase) * Frequency) * Amplitude;
}
inline float WaveformSawtooth(float x)
{
	return frac(x);
}

/// Gradual steady decrease from max to min over the period, with an instant return to max at the end.
inline float WaveformInvSawtooth(float x, float Base, float Amplitude, float Frequency, float Phase)
{
	return Base + (1.0f - frac((x + Phase) * Frequency)) * Amplitude;
}
inline float WaveformInvSawtooth(float x)
{
	return 1.0f - frac(x);
}

/// Pulse Width Modulation.
/** Works like square, except the high to low transition is controlled by duty
cycle. With a duty cycle of 50% (0.5) will give the same output as square. */
inline float WaveformPWM(float x, float DutyCycle, float Base, float Amplitude, float Frequency, float Phase)
{
	return Base + (frac((x + Phase) * Frequency) <= DutyCycle ? 1.0f : 0.0f) * Amplitude;
}
inline float WaveformPWM(float x, float DutyCycle)
{
	return (frac(x) <= DutyCycle ? 1.0f : 0.0f);
}

//@}
// Funkcje okresowe


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
/** \addtogroup base_random_gen Generator liczb pseudolosowych
*/
//@{

/// Generator liczb pseudolosowych
/** Jest przede wszystkim szybki. Nie jest bardzo dobrze losowy. Moøna go uøywaÊ
jako deterministycznego tworzπc w≥asne obiekty tej klasy i rÍcznie je
naziarniajπc. */
class RandomGenerator
{
private:
	// Current seed
	uint32 m_Seed;
	float m_NextNormalNumber; bool m_NextNormalNumberIs;

public:
	/// Constructor - seed from the clock
	RandomGenerator();
	/// Constructor - seed with given seed
	RandomGenerator(uint32 seed) : m_Seed(seed), m_NextNormalNumberIs(false) { }

	/// Wpisuje nowe ziarno.
	void Seed(uint32 a_Seed)
	{
		m_Seed = a_Seed;
	}

	/// Generuje liczbÍ losowπ ca≥kowitπ bez znaku, pe≥ny zakres
	/** Na podstawie ksiπøki "Pere≥ki programowania gier", tom III, Dante Treglia
	Uwaga! M≥odsze bity sπ s≥abo losowe, powtarzajπ siÍ w bardzo krÛtkim okresie. */
	uint32 RandUint_Fast()
	{
		return ( m_Seed = (m_Seed * 196314165 + 907633515) );
	}
	/// Generuje liczbÍ losowπ, w ktÛrej wszystkie bity sπ doúÊ losowe
	uint32 RandUint()
	{
		return
			(RandUint_Fast() & 0xFF000000)       |
			(RandUint_Fast() & 0xFF000000) >>  8 |
			(RandUint_Fast() & 0xFF000000) >> 16 |
			(RandUint_Fast() & 0xFF000000) >> 24;
	}
	/// Generuje liczbÍ losowπ w zakresie 0 .. max-1
	uint32 RandUint(uint32 max)
	{
		return RandUint() % max;
	}
	/// Generuje liczbÍ losowπ w zakresie min .. max-1
	uint32 RandUint(uint32 min, uint32 max)
	{
		return RandUint() % (max-min) + min;
	}

	/// Generuje losowy bajt
	uint8 RandByte()
	{
		return (RandUint_Fast() & 0xFF000000) >> 24;
	}

	/// Generuje liczbÍ losowπ rzeczywistπ z przedzia≥u 0.0..1.0
	/** Autor tego genialnego algorytmu: Mikael */
	float RandFloat()
	{
		return absolute_cast<float>(RandUint() & 0x007FFFFF | 0x3F800000) - 1.0f;
	}
	/// Generuje liczbÍ losowπ rzeczywistπ z przedzia≥u 0.0..max
	float RandFloat(float max)
	{
		return RandFloat() * max;
	}
	/// Generuje liczbÍ losowπ rzeczywistπ z przedzia≥u min..max
	float RandFloat(float min, float max)
	{
		return RandFloat() * (max - min) + min;
	}
	/// Generates random real number in range -1..1. (not sure whether inclusive).
	/** Author: iq/rgba, http://www.rgba.org/articles/sfrand/sfrand.htm */
	float RandFloat2()
	{
		uint a = (RandUint() & 0x007fffff) | 0x40000000;
		return( *((float*)&a) - 3.0f );
	}

	/// Generuje losowy stan logiczny
	bool RandBool()
	{
		return (RandUint() >= 0x7FFFFFFF);
	}

	/// Generuje liczbÍ losowπ ca≥kowitπ ze znakiem z zakresu Min .. Max-1
	int RandInt(int Min, int Max)
	{
		if (Max > Min)
			return static_cast<int>(RandUint(Max-Min)) + Min;
		else
			return static_cast<int>(RandUint(Min-Max)) + Max;
	}

	/// Wype≥nia podany bufor losowymi danymi binarnymi
	/** \param DataLength w bajtach. */
	void RandData(void *OutData, size_t DataLength);

	/// Generuje liczbÍ losowπ o rozk≥adzie normalnym (Gaussa)
	/** Zgodnie z definicjπ, wylosowana liczba leøy w zakresie z prawdopodobieÒstwem:
	- -1 * sigma .. +1 * sigma : 68%
	- -2 * sigma .. +2 * sigma : 95%
	- -3 * sigma .. +3 * sigma : 99.73% */
	float RandNormal(float sigma);
};

/// Domyúlny generator liczb losowych do uøywania w wπtku g≥Ûwnym i kiedy nie musi byÊ deterministyczny
extern RandomGenerator g_Rand;

/// Wolniejszy w inicjalizacji, generowaniu i wiÍkszy, ale za to tuøo lepszy generator liczb pseudolosowych.
/** èrÛd≥o:
Mersenne Twister Home Page,
http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html
*/
class MersenneTwister
{
public:
	/// Inicjalizuje bieøπcym czasem systemowym
	MersenneTwister();
	/// Inicjalizuje podanym ziarnem
	MersenneTwister(uint Seed);
	/// Inicjalizuje ziarnem w postaci podanej tablicy
	MersenneTwister(uint InitKeys[], int KeyCount);

	/// Generuje liczbÍ uint [0,0xffffffff]
	uint NextUint();
	/// Generuje liczbÍ int [0,0x7fffffff]
	int NextInt31();
	/// Generuje liczbÍ double [0,1]
	double NextReal1();
	/// Generuje liczbÍ double [0,1)
	double NextReal2();
	/// Generuje liczbÍ double (0,1)
	double NextReal3();
	/// Generuje liczbÍ double [0,1) z rozdzielczoúciπ 53 bitÛw
	double NextReal_Res53();

private:
	uint m_State[624];
	int m_Left;
	uint *m_Next;

	void Init(uint Seed);
	void NextState();
};

/// Generator unikatowych identyfikatorÛw
class UniqueGenerator
{
private:
	uint32 m_Next;

public:
	/// Pierwszy bÍdzie mia≥ nr 1.
	UniqueGenerator();
	/// Pierwszy bÍdzie mia≥ podany tutaj numer
	UniqueGenerator(uint32 First);

	/// Zwraca unikatowπ liczbÍ
	uint32 GetUint() { return m_Next++; }
	/// Zwraca unikatowy ≥aÒcuch w formacie "########", gdzie ######## to liczba szesnastkowa.
	void GetString(tstring *Out);
	tstring GetString() { tstring R; GetString(&R); return R; }
	/// Zwraca unikatowy ≥aÒcuch w formacie "<prefix>########", gdzie ######## to liczba szesnastkowa.
	void GetString(tstring *Out, const tstring &Prefix);
	/// \copydoc GetString(tstring*,const tstring&)
	tstring GetString(const tstring &Prefix) { tstring R; GetString(&R, Prefix); return R; }
};

//@}


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Parser wiersza poleceÒ

/// \internal
class CmdLineParser_pimpl;

/// Parser wiersza poleceÒ
class CmdLineParser
{
private:
	scoped_ptr<CmdLineParser_pimpl> pimpl;

public:
	/// Rodzaj pobranej informacji
	enum RESULT
	{
		RESULT_OPT,       ///< Opcja
		RESULT_PARAMETER, ///< Go≥y parametr bez opcji
		RESULT_END,       ///< Koniec parametrÛw
		RESULT_ERROR,     ///< B≥πd sk≥adni
	};

	/// Wersja dla main(int argc, char **argv)
	CmdLineParser(int argc, tchar **argv);
	/// Wersja dla WinMain(HINSTANCE Instance, HINSTANCE, char *CmdLine, int CmdShow)
	CmdLineParser(const tchar *CmdLine);

	~CmdLineParser();

	/// Rejestruje opcjÍ jednoznakowπ.
	/** \param Id podawaÊ liczby wiÍksze od 0. */
	void RegisterOpt(uint Id, tchar Opt, bool Parameter);
	/// Rejestruje opcjÍ wieloznakowπ.
	/** \param Id podawaÊ liczby wiÍksze od 0. */
	void RegisterOpt(uint Id, const tstring &Opt, bool Parameter);

	/// Wczytuje nastÍpnπ informacjÍ, zwraca jej rodzaj
	RESULT ReadNext();

	/// Zwraca dane na temat ostatnio wczytanej informacji - identyfikator opcji.
	/** Jeúli nie zosta≥a wczytana opcja, zwraca 0. */
	uint GetOptId();
	/// Zwraca dane na temat ostatnio wczytanej informacji - treúÊ parametru.
	/** Jeúli nie ma, ≥aÒcuch pusty. */
	const tstring & GetParameter();
};

/// Reprezentuje 128-bitowy globalnie unikatowy identyfikator, zgodnie z RFC 4122
class CommonGUID
{
public:
	static const CommonGUID NIL;

	static int Cmp(const CommonGUID &v1, const CommonGUID &v2);

	/// Tworzy niezainicjalizowany
	CommonGUID() { }
	CommonGUID(uint8 v0, uint8 v1, uint8 v2, uint8 v3, uint8 v4, uint8 v5, uint8 v6, uint8 v7, uint8 v8, uint8 v9, uint8 v10, uint8 v11, uint8 v12, uint8 v13, uint8 v14, uint8 v15);

	const uint8 * AccessData() const { return Data; }
	uint8 * AccessData() { return Data; }

	const uint8 & operator [] (uint i) const { return Data[i]; }
	uint8 & operator [] (uint i) { return Data[i]; }

	bool operator == (const CommonGUID &v) const { return Cmp(*this, v) == 0; }
	bool operator != (const CommonGUID &v) const { return Cmp(*this, v) != 0; }
	bool operator <  (const CommonGUID &v) const { return Cmp(*this, v) <  0; }
	bool operator >  (const CommonGUID &v) const { return Cmp(*this, v) >  0; }
	bool operator <= (const CommonGUID &v) const { return Cmp(*this, v) <= 0; }
	bool operator >= (const CommonGUID &v) const { return Cmp(*this, v) >= 0; }

	void ToString(tchar *out) const; ///< Bufor musi miec dlugosc co najmniej 37 znakow.
	void ToString(tstring &out) const;
	bool FromString(const tchar *s);
	bool FromString(const tstring &s);

	/// Generuje nowy, losowy, unikatowy identyfikator na podstawie algorytmu w wersji 4.
	void GenerateVersion4();

private:
	/// Od najmlodszych do najstarszych, tzn. Data[0] jest najmlodszy.
	uint8 Data[16];
};

//@}
// code_base

} // namespace common


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
/** \addtogroup code_sthtostr Uniwersalne konwersje
Uniwersalny i rozszerzalny mechanizm konwersji na i z ≥aÒcucha */
//@{

template <typename T>
struct SthToStr_obj
{
	void operator () (tstring *Str, const T &Sth)
	{
		// Runtime error
		assert(0 && "SthToStr: Unsupported type.");
		// Compilation error
		// Dzia≥a tylko w Visual C++, g++ pokazuje b≥πd kompilacji nawet kiedy nieuøyte
#ifdef WIN32
		int y = UnsupportedTypeInSthToStr;
#endif
	}
	static inline bool IsSupported() { return false; }
};

template <typename T>
inline void SthToStr(tstring *Str, const T &Sth)
{
	SthToStr_obj<T>()(Str, Sth);
}

template <typename T>
struct StrToSth_obj
{
	bool operator () (const tstring &Str, T *Sth)
	{
		// Runtime error
		assert(0 && "StrToSth: Unsupported type.");
		// Compilation error
		// Dzia≥a tylko w Visual C++, g++ pokazuje b≥πd kompilacji nawet kiedy nieuøyte
#ifdef WIN32
		int y = UnsupportedTypeInSthToStr;
#endif
		return false;
	}
	static inline bool IsSupported() { return false; }
};

template <typename T>
inline bool StrToSth(T *Sth, const tstring &Str)
{
	return StrToSth_obj<T>()(Sth, Str);
}

//@}

namespace common
{

/** \addtogroup code_sthtostr */
//@{

/// \internal
class Format_pimpl
{
	friend class Format;

private:
	tstring m_String;
	tchar m_Sep;
	size_t m_Index;

	Format_pimpl(const tstring &Fmt, tchar Sep) : m_String(Fmt), m_Sep(Sep), m_Index(0) { }
};

/// Klasa do wygodnego sk≥adania stringÛw z rÛønych informacji
/**
Przyk≥ad uøycia:
\code
int i = 10; float f = 3.14f; tstring s = _T("A");
string MyString = Format("i=#, f=#, s=#") % i % f % s;
\endcode
*/
class Format
{
private:
	shared_ptr<Format_pimpl> pimpl;

public:
	Format(const tstring &Fmt, tchar Sep = _T('#')) : pimpl(new Format_pimpl(Fmt, Sep)) { }
	/// Dodaje kolejnπ informacjÍ do formatowanego ≥aÒcucha
	/** \internal
	- To taki jakby dziwny konstuktor kopiujπcy.
	- Uøywam za kaødym razem tworzenia osobnego obiektu, bo nie moøna w g++
	modyfikowaÊ obiektu tymczasowego i nie da≥oby siÍ tego tak wygodnie
	uøywaÊ.
	- Do uøytku wewnÍtrznego - nie uøywaÊ! */
	Format(const Format &f, const tstring &Element);
	/// \internal
	Format(const Format &f, const tchar *Element);

	operator tstring () const { return pimpl->m_String; }
	/// Zwraca finalny string - stosowaÊ tam, gdzie nie zadzia≥a automatyczne rzutowanie na string.
	tstring str() const { return pimpl->m_String; }
};

template <typename T>
const Format operator % (const Format &fmt, const T &x)
{
	tstring s;
	SthToStr<T>(&s, x);
	return Format(fmt, s);
}

inline const Format operator % (const Format &fmt, const tchar *x)
{
	return Format(fmt, x);
}

//@}

} // namespace common


/** \addtogroup code_sthtostr */
//@{

//------------------------------------------------------------------------------
// SthToStr

template <>
struct SthToStr_obj<bool>
{
	void operator () (tstring *Str, const bool &Sth)
	{
		common::BoolToStr(Str, Sth, 'f');
	}
	static inline bool IsSupported() { return true; }
};

template <>
struct SthToStr_obj<tchar>
{
	void operator () (tstring *Str, const tchar &Sth)
	{
		common::CharToStr(Str, Sth);
	}
	static inline bool IsSupported() { return true; }
};

template <>
struct SthToStr_obj<int16>
{
	void operator () (tstring *Str, const int16 &Sth)
	{
		common::IntToStr<int16>(Str, Sth);
	}
	static inline bool IsSupported() { return true; }
};

template <>
struct SthToStr_obj<int32>
{
	void operator () (tstring *Str, const int32 &Sth)
	{
		common::IntToStr<int32>(Str, Sth);
	}
	static inline bool IsSupported() { return true; }
};
template <>
struct SthToStr_obj<long>
{
	void operator () (tstring *Str, const long &Sth)
	{
		common::IntToStr<int32>(Str, Sth);
	}
	static inline bool IsSupported() { return true; }
};

template <>
struct SthToStr_obj<int64>
{
	void operator () (tstring *Str, const int64 &Sth)
	{
		common::IntToStr<int64>(Str, Sth);
	}
	static inline bool IsSupported() { return true; }
};

template <>
struct SthToStr_obj<uint8>
{
	void operator () (tstring *Str, const uint8 &Sth)
	{
		common::UintToStr<uint8>(Str, Sth);
	}
	static inline bool IsSupported() { return true; }
};

template <>
struct SthToStr_obj<uint16>
{
	void operator () (tstring *Str, const uint16 &Sth)
	{
		common::UintToStr<uint16>(Str, Sth);
	}
	static inline bool IsSupported() { return true; }
};

template <>
struct SthToStr_obj<uint32>
{
	void operator () (tstring *Str, const uint32 &Sth)
	{
		common::UintToStr<uint32>(Str, Sth);
	}
	static inline bool IsSupported() { return true; }
};
template <>
struct SthToStr_obj<unsigned long>
{
	void operator () (tstring *Str, const unsigned long &Sth)
	{
		common::UintToStr<uint32>(Str, (uint32)Sth);
	}
	static inline bool IsSupported() { return true; }
};

template <>
struct SthToStr_obj<uint64>
{
	void operator () (tstring *Str, const uint64 &Sth)
	{
		common::UintToStr<uint64>(Str, Sth);
	}
	static inline bool IsSupported() { return true; }
};

template <>
struct SthToStr_obj<float>
{
	void operator () (tstring *Str, const float &Sth)
	{
		common::FloatToStr(Str, Sth);
	}
	static inline bool IsSupported() { return true; }
};

template <>
struct SthToStr_obj<double>
{
	void operator () (tstring *Str, const double &Sth)
	{
		common::DoubleToStr(Str, Sth);
	}
	static inline bool IsSupported() { return true; }
};

template <>
struct SthToStr_obj<tstring>
{
	void operator () (tstring *Str, const tstring &Sth)
	{
		*Str = Sth;
	}
	static inline bool IsSupported() { return true; }
};

template <typename T>
struct SthToStr_obj< std::vector<T> >
{
	void operator () (tstring *Str, const std::vector<T> &Sth)
	{
		Str->clear();
		tstring Element;
		for (typename std::vector<T>::const_iterator it = Sth.begin(); it != Sth.end(); ++it)
		{
			if (!Str->empty())
				*Str += ',';
			SthToStr<T>(&Element, *it);
			*Str += Element;
		}
	}
	static inline bool IsSupported() { return true; }
};

template <>
struct SthToStr_obj<char *>
{
	void operator () (tstring *Str, tchar *Sth)
	{
		*Str = Sth;
	}
	static inline bool IsSupported() { return true; }
};

template <>
struct SthToStr_obj<const tchar *>
{
	void operator () (tstring *Str, const tchar *Sth)
	{
		*Str = Sth;
	}
	static inline bool IsSupported() { return true; }
};

template <>
struct SthToStr_obj<void *>
{
	void operator () (tstring *Str, void *Sth)
	{
		common::PtrToStr(Str, Sth);
	}
	static inline bool IsSupported() { return true; }
};

template <>
struct SthToStr_obj<const void *>
{
	void operator () (tstring *Str, const void *Sth)
	{
		common::PtrToStr(Str, Sth);
	}
	static inline bool IsSupported() { return true; }
};

//------------------------------------------------------------------------------
// SthToStr

template <>
struct StrToSth_obj<bool>
{
	bool operator () (bool *Sth, const tstring &Str)
	{
		return common::StrToBool(Sth, Str);
	}
	static inline bool IsSupported() { return true; }
};

template <>
struct StrToSth_obj<tchar>
{
	bool operator () (tchar *Sth, const tstring &Str)
	{
		if (Str.length() != 1) return false;
		*Sth = Str[0];
		return true;
	}
	static inline bool IsSupported() { return true; }
};

template <>
struct StrToSth_obj<int16>
{
	bool operator () (int16 *Sth, const tstring &Str)
	{
		return common::StrToInt<int16>(Sth, Str) == 0;
	}
	static inline bool IsSupported() { return true; }
};

template <>
struct StrToSth_obj<int32>
{
	bool operator () (int32 *Sth, const tstring &Str)
	{
		return common::StrToInt<int32>(Sth, Str) == 0;
	}
	static inline bool IsSupported() { return true; }
};
template <>
struct StrToSth_obj<long>
{
	bool operator () (long *Sth, const tstring &Str)
	{
		return common::StrToInt<int32>((int32*)Sth, Str) == 0;
	}
	static inline bool IsSupported() { return true; }
};

template <>
struct StrToSth_obj<int64>
{
	bool operator () (int64 *Sth, const tstring &Str)
	{
		return common::StrToInt<int64>(Sth, Str) == 0;
	}
	static inline bool IsSupported() { return true; }
};

template <>
struct StrToSth_obj<uint8>
{
	bool operator () (uint8 *Sth, const tstring &Str)
	{
		return common::StrToUint<uint8>(Sth, Str) == 0;
	}
	static inline bool IsSupported() { return true; }
};

template <>
struct StrToSth_obj<uint16>
{
	bool operator () (uint16 *Sth, const tstring &Str)
	{
		return common::StrToUint<uint16>(Sth, Str) == 0;
	}
	static inline bool IsSupported() { return true; }
};

template <>
struct StrToSth_obj<uint32>
{
	bool operator () (uint32 *Sth, const tstring &Str)
	{
		return common::StrToUint<uint32>(Sth, Str) == 0;
	}
	static inline bool IsSupported() { return true; }
};
template <>
struct StrToSth_obj<unsigned long>
{
	bool operator () (unsigned long *Sth, const tstring &Str)
	{
		return common::StrToUint<uint32>((uint32*)Sth, Str) == 0;
	}
	static inline bool IsSupported() { return true; }
};

template <>
struct StrToSth_obj<uint64>
{
	bool operator () (uint64 *Sth, const tstring &Str)
	{
		return common::StrToUint<uint64>(Sth, Str) == 0;
	}
	static inline bool IsSupported() { return true; }
};

template <>
struct StrToSth_obj<float>
{
	bool operator () (float *Sth, const tstring &Str)
	{
		return common::StrToFloat(Sth, Str) == 0;
	}
	static inline bool IsSupported() { return true; }
};

template <>
struct StrToSth_obj<double>
{
	bool operator () (double *Sth, const tstring &Str)
	{
		return common::StrToDouble(Sth, Str) == 0;
	}
	static inline bool IsSupported() { return true; }
};

template <>
struct StrToSth_obj<tstring>
{
	bool operator () (tstring *Sth, const tstring &Str)
	{
		*Sth = Str;
		return true;
	}
	static inline bool IsSupported() { return true; }
};

template <typename T>
struct StrToSth_obj< std::vector<T> >
{
	bool operator () (std::vector<T> *Sth, const tstring &Str)
	{
		Sth->clear();
		uint32 Index = 0;
		tstring Element;
		T Val;
		while (common::Split(Str, _T(","), &Element, &Index))
		{
			if (!StrToSth<T>(&Val, Element)) return false;
			Sth->push_back(Val);
		}
		return true;
	}
	static inline bool IsSupported() { return true; }
};

//@}
// code_sthtostr

#endif
