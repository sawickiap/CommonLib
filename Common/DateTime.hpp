/** \file
\brief Date and time
\author Adam Sawicki - sawickiap@poczta.onet.pl - http://asawicki.info/ \n

Part of CommonLib library. \n
Encoding UTF-8, end of line CR+LF \n
License: GNU LGPL. \n
Documentation: \ref Module_DateTime \n
Module components: \ref code_datetime
*/
#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif
#ifndef COMMON_DATETIME_H_
#define COMMON_DATETIME_H_

#include <time.h>

namespace common
{

/** \addtogroup code_datetime DateTime Module
Dokumentacja: \ref Module_DateTime \n
Nag³ówek: DateTime.hpp */
//@{

/// Dzieñ tygodnia
enum WEEKDAY
{
	SUN, MON, TUE, WED, THU, FRI, SAT, INV_WEEKDAY
};

/// Miesi¹c roku
enum MONTH
{
	JAN, FEB, MAR, APR, MAY, JUN, JUL, AUG, SEP, OCT, NOV, DEC, INV_MONTH
};

/// Flagi bitowe - nale¿y po³¹czyæ razem flagê SHORT/LONG z flag¹ LOWERCASE/FIRST_UPPERCASE/UPPERCASE lub u¿yæ jednej z ju¿ po³¹czonych.
enum NAME_FORM
{
	NAME_SHORT = 0x00,
	NAME_LONG  = 0x01,
	NAME_LOWERCASE       = 0x00,
	NAME_FIRST_UPPERCASE = 0x10,
	NAME_UPPERCASE       = 0x20,
	NAME_SHORT_LOWERCASE       = 0x00,
	NAME_SHORT_FIRST_UPPERCASE = 0x10,
	NAME_SHORT_UPPERCASE       = 0x20,
	NAME_LONG_LOWERCASE        = 0x01,
	NAME_LONG_FIRST_UPPERCASE  = 0x11,
	NAME_LONG_UPPERCASE        = 0x21,
};

/// Domyœlny format dla StrToDate() i DateToStr()
extern const tchar * const DEFAULT_FORMAT;

struct DATESPAN;
struct TIMESPAN;
struct TMSTRUCT;
struct DATETIME;

/// Reprezentuje odcinek czasu.
/** Przechowuje osobne liczby ca³kowite dla: lat, miesiêcy, tygodni i dni.
Mo¿na za jego pomoc¹ przesuwaæ TMSTRUCT. */
struct DATESPAN
{
private:
	int m_Years;
	int m_Months;
	int m_Weeks;
	int m_Days;

public:
	/// Zwraca DATESPAN reprezentuj¹cy n dni
	static DATESPAN Days(int n) { return DATESPAN(0, 0, 0, n); }
	/// Zwraca DATESPAN reprezentuj¹cy 1 dzieñ
	static DATESPAN Day() { return Days(1); }
	/// Zwraca DATESPAN reprezentuj¹cy n tygodni
	static DATESPAN Weeks(int n) { return DATESPAN(0, 0, n, 0); }
	/// Zwraca DATESPAN reprezentuj¹cy 1 tydzieñ
	static DATESPAN Week() { return Weeks(1); }
	/// Zwraca DATESPAN reprezentuj¹cy n miesiêcy
	static DATESPAN Months(int n) { return DATESPAN(0, n, 0, 0); }
	/// Zwraca DATESPAN reprezentuj¹cy 1 miesi¹c
	static DATESPAN Month() { return Months(1); }
	/// Zwraca DATESPAN reprezentuj¹cy n lat
	static DATESPAN Years(int n) { return DATESPAN(n, 0, 0, 0); }
	/// Zwraca DATESPAN reprezentuj¹cy 1 rok
	static DATESPAN Year() { return Years(1); }

	DATESPAN(int Years, int Months, int Weeks, int Days) : m_Years(Years), m_Months(Months), m_Weeks(Weeks), m_Days(Days) { }

	DATESPAN & operator += (const DATESPAN &ds) { m_Years += ds.m_Years; m_Months += ds.m_Months; m_Weeks += ds.m_Weeks; m_Days += ds.m_Days; return *this; }
	DATESPAN & operator -= (const DATESPAN &ds) { m_Years -= ds.m_Years; m_Months -= ds.m_Months; m_Weeks -= ds.m_Weeks; m_Days -= ds.m_Days; return *this; }
	DATESPAN & operator *= (int n) { m_Years *= n; m_Months *= n; m_Weeks *= n; m_Days *= n; return *this; }
	DATESPAN operator - () const { return DATESPAN(-m_Years, -m_Months, -m_Weeks, -m_Days); }
	bool operator == (const DATESPAN &ds) const { return m_Years == ds.m_Years && m_Months == ds.m_Months && m_Weeks == ds.m_Weeks && m_Days == ds.m_Days; }
	bool operator != (const DATESPAN &ds) const { return m_Years != ds.m_Years || m_Months != ds.m_Months || m_Weeks != ds.m_Weeks || m_Days != ds.m_Days; }

	/// Zwraca liczbê dni (bez tygodni)
	int GetDays() const { return m_Days; }
	/// Zwraca liczbê tygodni
	int GetWeeks() const { return m_Weeks; }
	/// Zwraca liczbê miesiêcy (bez lat)
	int GetMonths() const { return m_Months; }
	/// Zwraca liczbê lat
	int GetYears() const { return m_Years; }
	/// Zwraca liczbê dni wliczaj¹c tygodnie (ale nie miesi¹ce ani lata)
	int GetTotalDays() const { return m_Weeks * 7 + m_Days; }

	/** \name Ustawianie poszczególnych pól */
	//@{
	void Set(int Years, int Months, int Weeks, int Days) { m_Years = Years; m_Months = Months; m_Weeks = Weeks; m_Days = Days; }
	void SetDays(int n) { m_Days = n; }
	void SetWeeks(int n) { m_Weeks = n; }
	void SetMonths(int n) { m_Months = n; }
	void SetYears(int n) { m_Years = n; }
	//@}
};

inline DATESPAN operator + (const DATESPAN &ds1, const DATESPAN &ds2) { return DATESPAN(ds1.GetYears()+ds2.GetYears(), ds1.GetMonths()+ds2.GetMonths(), ds1.GetWeeks()+ds2.GetWeeks(), ds1.GetDays()+ds2.GetDays()); }
inline DATESPAN operator - (const DATESPAN &ds1, const DATESPAN &ds2) { return DATESPAN(ds1.GetYears()-ds2.GetYears(), ds1.GetMonths()-ds2.GetMonths(), ds1.GetWeeks()-ds2.GetWeeks(), ds1.GetDays()-ds2.GetDays()); }
inline DATESPAN operator * (const DATESPAN &ds, int n) { return DATESPAN(ds.GetYears()*n, ds.GetMonths()*n, ds.GetWeeks()*n, ds.GetDays()*n); }
inline DATESPAN operator * (int n, const DATESPAN &ds) { return DATESPAN(ds.GetYears()*n, ds.GetMonths()*n, ds.GetWeeks()*n, ds.GetDays()*n); }


/// Reprezentuje odcinek czasu.
/*** Przechowuje liczbê ca³kowit¹ ze znakiem w milisekundach.
Mo¿na za jego pomoc¹ przesuwaæ DATETIME. */
struct TIMESPAN
{
public:
	/** \name TIMESPAN dla podanego odcinka czasu */
	//@{
	static TIMESPAN Milliseconds(int64 Milliseconds) { return TIMESPAN(0, 0, 0, Milliseconds); }
	static TIMESPAN Millisecond() { return Milliseconds(1); }
	static TIMESPAN Seconds(int64 Seconds) { return TIMESPAN(0, 0, Seconds); }
	static TIMESPAN Second() { return Seconds(1); }
	static TIMESPAN Minutes(int Minutes) { return TIMESPAN(0, Minutes, 0); }
	static TIMESPAN Minutes() { return Minutes(1); }
	static TIMESPAN Hours(int Hours) { return TIMESPAN(Hours, 0, 0); }
	static TIMESPAN Hour() { return Hours(1); }
	static TIMESPAN Days(int Days) { return Hours(Days * 24); }
	static TIMESPAN Day() { return Days(1); }
	static TIMESPAN Weeks(int Weeks) { return Days(Weeks * 7); }
	static TIMESPAN Week() { return Weeks(1); }
	//@}

	/// W milisekundach
	int64 m_Diff;

	/// Tworzy niezainicjalizowan¹
	TIMESPAN() { }
	/// Tworzy i inicjalizuje liczb¹ milisekund
	explicit TIMESPAN(int64 Diff) { Set(Diff); }
	/// Tworzy i inicjalizuje podanymi wartoœciami (zakres dowolny, niekoniecznie 0..59 ani nic takiego)
	TIMESPAN(int Hours, int Minutes, int64 Seconds, int64 Milliseconds = 0) { Set(Hours, Minutes, Seconds, Milliseconds); }

	void Set(int64 Diff) { m_Diff = Diff; }
	void Set(int Hours, int Minutes, int64 Seconds, int64 Milliseconds = 0);

	bool operator == (const TIMESPAN &t) const { return m_Diff == t.m_Diff; }
	bool operator != (const TIMESPAN &t) const { return m_Diff != t.m_Diff; }
	bool operator <= (const TIMESPAN &t) const { return m_Diff <= t.m_Diff; }
	bool operator >= (const TIMESPAN &t) const { return m_Diff >= t.m_Diff; }
	bool operator <  (const TIMESPAN &t) const { return m_Diff <  t.m_Diff; }
	bool operator >  (const TIMESPAN &t) const { return m_Diff >  t.m_Diff; }
	TIMESPAN operator - () const { return TIMESPAN(-m_Diff); }
	TIMESPAN & operator *= (int n) { m_Diff *= n; return *this; }
	TIMESPAN & operator += (const TIMESPAN &t) { m_Diff += t.m_Diff; return *this; }
	TIMESPAN & operator -= (const TIMESPAN &t) { m_Diff -= t.m_Diff; return *this; }

	/// Porównywanie wartoœci bezwzglêdnej
	bool IsShorterThan(const TIMESPAN &t) const { return Abs() < t.Abs(); }
	/// Porównywanie wartoœci bezwzglêdnej
	bool IsLongerThan(const TIMESPAN &t) const { return Abs() > t.Abs(); }

	bool IsNull() const { return m_Diff == 0; }
	bool IsPositive() const { return m_Diff > 0; }
	bool IsNegative() const { return m_Diff < 0; }

	int GetWeeks() const { return GetDays() / 7; }
	int GetDays() const { return GetHours() / 24; }
	int GetHours() const { return GetMinutes() / 60; }
	int GetMinutes() const { return (int)(GetSeconds() / 60); }
	int64 GetSeconds() const { return m_Diff / 1000; }
	int64 GetMilliseconds() const { return m_Diff; }

	/// Zwraca nowy TIMESPAN - wartoœæ bezwzglêdna
	TIMESPAN Abs() const { return TIMESPAN(m_Diff < 0 ? -m_Diff : m_Diff); }

	/// Przetwarza ten odcinek czasu na ³añcuch w postaci formalnej: "0:00:00" (H:MM:SS) lub z milisekundami: "0:00:00:0000" (H:MM:SS:IIII)
	void ToString(tstring *Out, bool ShowMilliseconds = false) const;
	/// Przetwarza ten odcinek czasu na ³añcuch w postaci przyjaznej, np. "2:55 h", "3:10 min", "20 s"
	void ToString_Nice(tstring *Out, bool ShowMilliseconds = false) const;
};

inline TIMESPAN operator * (const TIMESPAN &t, int n) { return TIMESPAN(t.m_Diff * n); }
inline TIMESPAN operator * (int n, const TIMESPAN &t) { return TIMESPAN(t.m_Diff * n); }
inline TIMESPAN operator + (const TIMESPAN &t1, const TIMESPAN &t2) { return TIMESPAN(t1.m_Diff + t2.m_Diff); }
inline TIMESPAN operator - (const TIMESPAN &t1, const TIMESPAN &t2) { return TIMESPAN(t1.m_Diff - t2.m_Diff); }

/// Reprezentuje moment czasu.
/** Przechowuje osobne pola: dzien, miesi¹c, rok, godzina, minuta, sekunda, milisekunda.
Pamiêta tak¿e dzieñ tygodnia, który wyliczany jest przy pierwszym odczytaniu (pole mutable).
*/
struct TMSTRUCT
{
private:
	uint16 msec, sec, min, hour, mday;
	MONTH mon;
	int year;

	mutable WEEKDAY wday;

	/// Wylicza dzieñ tygodnia na podstawie pozosta³ych pól
	void ComputeWeekDay() const;

public:

	/// Tworzy niezainicjalizowany
	TMSTRUCT() : wday(INV_WEEKDAY) { }
	/// Tworzy na podstawie podanych danych
	explicit TMSTRUCT(const DATETIME &dt) { Set(dt); }
	explicit TMSTRUCT(const struct tm &tm) { Set(tm); }
	TMSTRUCT(uint16 Day, MONTH Month, int Year, uint16 Hour, uint16 Minute, uint16 Second, uint16 Millisec = 0) { Set(Day, Month, Year, Hour, Minute, Second, Millisec); }
	TMSTRUCT(uint16 Day, MONTH Month, int Year) { Set(Day, Month, Year, 0, 0, 0); }

	void Set(const DATETIME &dt);
	void Set(const struct tm &tm);

	int GetYear() const { return year; }
	MONTH GetMonth() const { return mon; }
	uint16 GetDay() const { return mday; }
	uint16 GetHour() const { return hour; }
	uint16 GetMinute() const { return min; }
	uint16 GetSecond() const { return sec; }
	uint16 GetMillisecond() const { return msec; }

	/// W razie potrzeby wylicza i zwraca dzieñ tygodnia
	WEEKDAY GetWeekDay() const
	{
		if (wday == INV_WEEKDAY)
			ComputeWeekDay();
		return (WEEKDAY)wday;
	}

	TMSTRUCT & operator += (const DATESPAN &d) { Add(d);      return *this; }
	TMSTRUCT & operator -= (const DATESPAN &d) { Subtract(d); return *this; }

	void Add(const DATESPAN &d);
	void Subtract(const DATESPAN &d) { Add(-d); }

	/// Sprawdza poprawnoœæ
	bool IsValid() const;
	bool IsSameDate(const TMSTRUCT &tm) const;
	bool IsSameTime(const TMSTRUCT &tm) const;

	/// Dodaje miesi¹ce pozostawiaj¹c miesi¹c i rok znormalizowane
	/** Mo¿e wyjœæ data poza zakresem np. 31 lutego. */
	void AddMonths(int MonDiff);
	/// Dodaje dni pozostawiaj¹c datê znormalizowan¹
	void AddDays(int DayDiff);

	/// Ustawia poszczególne pola
	void Set(uint16 Day, MONTH Month, int Year, uint16 Hour, uint16 Minute, uint16 Second, uint16 Millisec = 0);
	void Set(uint16 Day, MONTH Month, int Year) { Set(Day, Month, Year, 0, 0, 0); }
	void SetYear(int Year) { year = Year; wday = INV_WEEKDAY; }
	void SetMonth(MONTH Month) { mon = Month; wday = INV_WEEKDAY; }
	void SetDay(uint16 Day) { mday = Day; wday = INV_WEEKDAY; }
	void SetHour(uint16 Hour) { hour = Hour; }
	void SetMinute(uint16 Minute) { min = Minute; }
	void SetSecond(uint16 Second) { sec = Second; }
	void SetMillisecond(uint16 Millisecond) { msec = Millisecond; }

	/// Zeruje czas, zostawia tylko datê. Czas bêdzie wynosi³ 00:00:00:0
	void ResetTime() { hour = min = sec = msec = 0; }
	
	/// Przetwarza na struct tm
	void GetTm(struct tm *Out) const;
};

inline TMSTRUCT operator + (const TMSTRUCT &tm, const DATESPAN &ds) { TMSTRUCT R = tm; R += ds; return R; }
inline TMSTRUCT operator - (const TMSTRUCT &tm, const DATESPAN &ds) { TMSTRUCT R = tm; R -= ds; return R; }

/// Reprezentuje moment czasu.
/** Przechowuje liczbê milisekund od epoki Uniksowej, czyli 1 stycznia 1970. */
struct DATETIME
{
private:
	bool IsInStdRange() const;

public:
	int64 m_Time;

	/// Tworzy nowy, niezainicjalizowany
	DATETIME() { }
	/// Tworzy nowy na podstawie podanych danych
	explicit DATETIME(time_t Time) { Set(Time); }
	explicit DATETIME(const struct tm &Time) { Set(Time); }
	explicit DATETIME(const TMSTRUCT &Time) { Set(Time); }

	bool operator == (const DATETIME &t) const { return m_Time == t.m_Time; }
	bool operator != (const DATETIME &t) const { return m_Time != t.m_Time; }
	bool operator <= (const DATETIME &t) const { return m_Time <= t.m_Time; }
	bool operator >= (const DATETIME &t) const { return m_Time >= t.m_Time; }
	bool operator <  (const DATETIME &t) const { return m_Time <  t.m_Time; }
	bool operator >  (const DATETIME &t) const { return m_Time >  t.m_Time; }

	DATETIME & operator += (const TIMESPAN &t) { Add(t);      return *this; }
	DATETIME & operator -= (const TIMESPAN &t) { Subtract(t); return *this; }

	/** \name Ustawia ca³¹ wartoœæ na podan¹ datê i czas */
	//@{
	void SetValue(int64 Value) { m_Time = Value; }
	void Set(time_t Time);
	void Set(const struct tm &Time);
	void Set(const TMSTRUCT &Time);
	void SetJDN(double JDN);
	//@}
	/** \name Ustawia poszczególne pola */
	//@{
	void SetMillisecond(uint Millisecond) { m_Time -= m_Time % 1000; m_Time += Millisecond; }
	/// Zwiêksza wartoœæ
	void Add(int64 Milliseconds) { m_Time += Milliseconds; }
	/// Zwiêksza wartoœæ
	void Add(const TIMESPAN &t) { m_Time += t.m_Diff; }
	/// Zmniejsza wartoœæ
	void Subtract(int64 Milliseconds) { m_Time -= Milliseconds; }
	/// Zmniejsza wartoœæ
	void Subtract(const TIMESPAN &t) { m_Time -= t.m_Diff; }
	//@}

	bool IsStrictlyBetween(const DATETIME &dt1, const DATETIME &dt2) const { return (m_Time > dt1.m_Time) && (m_Time < dt2.m_Time); }
	bool IsBetween(const DATETIME &dt1, const DATETIME &dt2) const { return (m_Time >= dt1.m_Time) && (m_Time <= dt2.m_Time); }
	bool IsEqualUpTo(const DATETIME &dt, const TIMESPAN &ts) const;

	/// Przerabia na time_t
	/** Zwraca (time_t)-1 jeœli poza zakresem */
	time_t GetTicks() const;
};

inline TIMESPAN operator - (const DATETIME &dt1, const DATETIME &dt2) { return TIMESPAN(dt1.m_Time - dt2.m_Time); }
inline DATETIME operator + (const DATETIME &dt, const TIMESPAN &ts) { DATETIME R = dt; R += ts; return R; }
inline DATETIME operator - (const DATETIME &dt, const TIMESPAN &ts) { DATETIME R = dt; R -= ts; return R; }

/// Zwraca true, jeœli podany rok jest przestêpny
inline bool IsLeapYear(int Year) { return (Year % 4 == 0) && ((Year % 100 != 0) || (Year % 400 == 0)); }
/// Zwraca liczbê dni w podanym roku (365 lub 366)
inline uint GetNumOfDaysInYear(int Year) { return IsLeapYear(Year) ? 366 : 365; }
/// Zwraca liczbê dni w podanym miesi¹cu podanego roku
uint GetNumOfDaysInMonth(int Year, uint Month);
/// Zwraca wiek podanego roku
inline int GetCentury(int Year) { return (Year > 0) ? (Year / 100) : (Year / 100 - 1); }

/// Zwraca bie¿¹c¹ datê i czas jako time_t
time_t GetTimeNow();
/// Zwraca bie¿¹c¹ datê i czas jako struct tm
void GetTmNow(struct tm *Out);
/// Zwraca bie¿¹c¹ datê i czas, dok³adnoœæ co do sekund
DATETIME Now();
/// Zwraca bie¿¹c¹ datê i czas, dok³adnoœæ co do milisekund
DATETIME UNow();

/// Zwraca nazwê dnia tygodnia w podanej formie (W jezyku polskim!!!)
const tchar * GetWeekDayName(WEEKDAY WeekDay, NAME_FORM Form);
/// Zwraca nazwê miesi¹ca w podanej formie (W jezyku polskim!!!)
const tchar * GetMonthName(MONTH Month, NAME_FORM Form);
/// Konwertuje datê na ³añcuch
void DateToStr(tstring *Out, const TMSTRUCT &tm, const tstring &Format);

/// Konwertuje ³añcuch na dzieñ tygodnia. £añcuch mo¿e byæ w dowolnej formie.
/** Jeœli siê nie uda, zwraca INV_WEEKDAY. */
WEEKDAY StrToWeekDay(const tstring &s);
/// Konwertuje ³añcuch na miesi¹c. £añcuch mo¿e byæ w dowolnej formie.
/** Jeœli siê nie uda, zwraca INV_WEEKDAY. */
MONTH StrToMonth(const tstring &s);
/// Konwertuje ³añcuch na datê
/** Jeœli siê nie uda, zwraca false.
¯eby to mia³o sens, musz¹ siê znaleŸæ tam co najmniej rok, miesi¹c i dzieñ.*/
bool StrToDate(TMSTRUCT *Out, const tstring &s, const tstring &Format);

#ifdef WIN32

/// Reprezentuje b¹dŸ to bezwzglêdny czas systemowy, b¹dŸ te¿ odcinek (róznicê) czasu, jeœli odjête dwa GameTime.
/** Wartoœæ bezwzglêdna jest mierzona od chwili startu programu. */
struct GameTime
{
public:
	static const GameTime ZERO;
	static const GameTime MIN_VALUE;
	static const GameTime MAX_VALUE;
	/// Trzeba wywo³aæ na pocz¹tku programu, ¿eby poprawnie dzia³a³a klasa GameTime.
	static void Initialize();

	GameTime() { }
	explicit GameTime(int64 v) : m_PerfCount(v) { }

	void SetInt8(int64 v) { m_PerfCount = v; }
	int64 GetInt8() const { return m_PerfCount; }

	GameTime operator + () const { return GameTime( m_PerfCount); }
	GameTime operator - () const { return GameTime(-m_PerfCount); }
	GameTime operator * (int64 v) const { return GameTime(m_PerfCount * v); }
	GameTime operator / (int64 v) const { return GameTime(m_PerfCount / v); }
	GameTime operator % (int64 v) const { return GameTime(m_PerfCount % v); }
	int64 operator / (GameTime v) const { return m_PerfCount / v.m_PerfCount; }
	int64 operator % (GameTime v) const { return m_PerfCount % v.m_PerfCount; }
	GameTime operator + (GameTime v) const { return GameTime(m_PerfCount + v.m_PerfCount); }
	GameTime operator - (GameTime v) const { return GameTime(m_PerfCount - v.m_PerfCount); }
	GameTime & operator = (GameTime v) { m_PerfCount = v.m_PerfCount; return *this; }
	GameTime & operator += (GameTime v) { m_PerfCount += v.m_PerfCount; return *this; }
	GameTime & operator -= (GameTime v) { m_PerfCount -= v.m_PerfCount; return *this; }
	GameTime & operator *= (int64 v) { m_PerfCount *= v; return *this; }
	GameTime & operator /= (int64 v) { m_PerfCount /= v; return *this; }
	GameTime & operator %= (int64 v) { m_PerfCount %= v; return *this; }
	bool operator == (GameTime v) const { return m_PerfCount == v.m_PerfCount; }
	bool operator != (GameTime v) const { return m_PerfCount != v.m_PerfCount; }
	bool operator <  (GameTime v) const { return m_PerfCount <  v.m_PerfCount; }
	bool operator >  (GameTime v) const { return m_PerfCount >  v.m_PerfCount; }
	bool operator <= (GameTime v) const { return m_PerfCount <= v.m_PerfCount; }
	bool operator >= (GameTime v) const { return m_PerfCount >= v.m_PerfCount; }

	float  ToSeconds_f() const { return (float) m_PerfCount / (float) s_PerfFreq; }
	double ToSeconds_d() const { return (double)m_PerfCount / (double)s_PerfFreq; }
	int64 ToMilliseconds() const;
	int  ToMilliseconds_i() const { return (int)ToMilliseconds(); }
	TIMESPAN ToTimespan() const { return TIMESPAN(ToMilliseconds()); }

private:
	friend GameTime GetCurrentGameTime();
	friend GameTime MillisecondsToGameTime(int Milliseconds);
	friend GameTime MillisecondsToGameTime(int64 Milliseconds);
	friend GameTime SecondsToGameTime(float Seconds);
	friend GameTime SecondsToGameTime(double Seconds);

	int64 m_PerfCount;

	static bool s_Initialized;
	static int64 s_PerfFreq;
	static int64 s_StartPerfCount;
};

inline GameTime operator * (int64 v, const GameTime &t) { return GameTime(t.GetInt8() * v); }

GameTime GetCurrentGameTime();
GameTime MillisecondsToGameTime(int Milliseconds);
GameTime MillisecondsToGameTime(int64 Milliseconds);
GameTime SecondsToGameTime(float Seconds);
GameTime SecondsToGameTime(double Seconds);

#endif // WIN32

//@}
// code_datetime

} // namespace common

/** \addtogroup code_sthtostr */
//@{

template <>
struct SthToStr_obj<common::TMSTRUCT>
{
	void operator () (tstring *Str, const common::TMSTRUCT &Sth)
	{
		common::DateToStr(Str, Sth, common::DEFAULT_FORMAT);
	}
	static inline bool IsSupported() { return true; }
};

template <>
struct StrToSth_obj<common::TMSTRUCT>
{
	bool operator () (common::TMSTRUCT *Sth, const tstring &Str)
	{
		return common::StrToDate(Sth, Str, common::DEFAULT_FORMAT);
	}
	static inline bool IsSupported() { return true; }
};

template <>
struct SthToStr_obj<common::DATETIME>
{
	void operator () (tstring *Str, const common::DATETIME &Sth)
	{
		SthToStr(Str, Sth.m_Time);
	}
	static inline bool IsSupported() { return true; }
};

template <>
struct SthToStr_obj<common::TIMESPAN>
{
	void operator () (tstring *Str, const common::TIMESPAN &Sth)
	{
		SthToStr(Str, Sth.m_Diff);
	}
	static inline bool IsSupported() { return true; }
};

//@}
// code_sthtostr

#endif
