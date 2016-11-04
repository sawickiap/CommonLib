/** \file
\brief Date and time
\author Adam Sawicki - sawickiap@poczta.onet.pl - http://asawicki.info/ \n

Part of CommonLib library. \n
Encoding UTF-8, end of line CR+LF \n
License: GNU LGPL. \n
Documentation: \ref Module_DateTime
*/
#include "Base.hpp"
#include "DateTime.hpp"
#ifdef WIN32
	#include <windows.h>
#else
	#include <sys/time.h> // dla gettimeofday
#endif

namespace common
{

// this is the integral part of JDN of the midnight of Jan 1, 1970 (i.e. JDN(Jan 1, 1970) = 2440587.5)
const int EPOCH_JDN = 2440587;
const int MONTHS_IN_YEAR = 12;
const int SEC_PER_MIN = 60;
const int MIN_PER_HOUR = 60;
const int HOURS_PER_DAY = 24;
const int SECONDS_PER_DAY = 86400;
const int DAYS_PER_WEEK = 7;
const int MILLISECONDS_PER_DAY = 86400000;
const uint SECONDS_PER_HOUR = 3600;
// this constant is used to transform a time_t value to the internal
// representation, as time_t is in seconds and we use milliseconds it's
// fixed to 1000
const int TIME_T_FACTOR = 1000;

// the date of JDN -0.5 (as we don't work with fractional parts, this is the
// reference date for us) is Nov 24, 4714BC
const int JDN_0_YEAR = -4713;
const int JDN_0_MONTH = NOV;
const int JDN_0_DAY = 24;

// the constants used for JDN calculations
const int JDN_OFFSET         = 32046;
const int DAYS_PER_5_MONTHS  = 153;
const int DAYS_PER_4_YEARS   = 1461;
const int DAYS_PER_400_YEARS = 146097;

const tchar * const DEFAULT_FORMAT = _T("Y-N-D H:M:S");

const tchar * const WEEKDAY_NAMES_SHORT_LOWERCASE[] = {
	_T("ndz"), _T("pn"), _T("wt"), _T("œr"), _T("czw"), _T("pt"), _T("sob"), _T("err")
};
const tchar * const WEEKDAY_NAMES_SHORT_FIRST_UPPERCASE[] = {
	_T("Ndz"), _T("Pn"), _T("Wt"), _T("Œr"), _T("Czw"), _T("Pt"), _T("Sob"), _T("Err")
};
const tchar * const WEEKDAY_NAMES_SHORT_UPPERCASE[] = {
	_T("NDZ"), _T("PN"), _T("WT"), _T("ŒR"), _T("CZW"), _T("PT"), _T("SOB"), _T("ERR")
};
const tchar * const WEEKDAY_NAMES_LONG_LOWERCASE[] = {
	_T("niedziela"), _T("poniedzia³ek"), _T("wtorek"), _T("œroda"), _T("czwartek"), _T("pi¹tek"), _T("sobota"), _T("b³¹d")
};
const tchar * const WEEKDAY_NAMES_LONG_FIRST_UPPERCASE[] = {
	_T("Niedziela"), _T("Poniedzia³ek"), _T("Wtorek"), _T("Œroda"), _T("Czwartek"), _T("Pi¹tek"), _T("Sobota"), _T("B³¹d")
};
const tchar * const WEEKDAY_NAMES_LONG_UPPERCASE[] = {
	_T("NIEDZIELA"), _T("PONIEDZIA£EK"), _T("WTOREK"), _T("ŒRODA"), _T("CZWARTEK"), _T("PI¥TEK"), _T("SOBOTA"), _T("B£¥D")
};

const tchar * const MONTH_NAMES_SHORT_LOWERCASE[] = {
	_T("sty"), _T("lut"), _T("mar"), _T("kwi"), _T("maj"), _T("cze"), _T("lip"), _T("sie"), _T("wrz"), _T("paz"), _T("lis"), _T("gru"), _T("err")
};
const tchar * const MONTH_NAMES_SHORT_FIRST_UPPERCASE[] = {
	_T("Sty"), _T("Lut"), _T("Mar"), _T("Kwi"), _T("Maj"), _T("Cze"), _T("Lip"), _T("Sie"), _T("Wrz"), _T("Paz"), _T("Lis"), _T("Gru"), _T("err")
};
const tchar * const MONTH_NAMES_SHORT_UPPERCASE[] = {
	_T("STY"), _T("LUT"), _T("MAR"), _T("KWI"), _T("MAJ"), _T("CZE"), _T("LIP"), _T("SIE"), _T("WRZ"), _T("PAZ"), _T("LIS"), _T("GRU"), _T("ERR")
};
const tchar * const MONTH_NAMES_LONG_LOWERCASE[] = {
	_T("styczeñ"), _T("luty"), _T("marzec"), _T("kwiecieñ"), _T("maj"), _T("czerwiec"), _T("lipiec"), _T("sierpieñ"), _T("wrzesieñ"), _T("paŸdziernik"), _T("listopad"), _T("grudzieñ"), _T("b³¹d")
};
const tchar * const MONTH_NAMES_LONG_FIRST_UPPERCASE[] = {
	_T("Styczeñ"), _T("Luty"), _T("Marzec"), _T("Kwiecieñ"), _T("Maj"), _T("Czerwiec"), _T("Lipiec"), _T("Sierpieñ"), _T("Wrzesieñ"), _T("PaŸdziernik"), _T("Listopad"), _T("Grudzieñ"), _T("B³¹d")
};
const tchar * const MONTH_NAMES_LONG_UPPERCASE[] = {
	_T("STYCZEÑ"), _T("LUTY"), _T("MARZEC"), _T("KWIECIEÑ"), _T("MAJ"), _T("CZERWIEC"), _T("LIPIEC"), _T("SIERPIEÑ"), _T("WRZESIEÑ"), _T("PADZIERNIK"), _T("LISTOPAD"), _T("GRUDZIEÑ"), _T("B£¥D")
};

// Liczba dni w poszczególnych miesi¹cach lat 1. zwyk³ych 2. przestêpnych
const uint DAYS_IN_MONTH[2][MONTHS_IN_YEAR] = {
	{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
	{ 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
};

const uint16 CUMULATIVE_DAYS[2][MONTHS_IN_YEAR] = {
    { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 },
    { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335 }
};

// [wewnêtrzna]
// return the integral part of the JDN for the midnight of the given date (to
// get the real JDN you need to add 0.5, this is, in fact, JDN of the
// noon of the previous day)
int GetTruncatedJDN(uint16 day, MONTH mon, int year)
{
    // CREDIT: code below is by Scott E. Lee (but bugs are mine)

    // check the date validity
    assert(
      ((year > JDN_0_YEAR) ||
      ((year == JDN_0_YEAR) && (mon > JDN_0_MONTH)) ||
      ((year == JDN_0_YEAR) && (mon == JDN_0_MONTH) && (day >= JDN_0_DAY))) &&
      "date out of range - can't convert to JDN");

    // make the year positive to avoid problems with negative numbers division
    year += 4800;

    // months are counted from March here
    int month;
    if ( mon >= MAR )
    {
        month = mon - 2;
    }
    else
    {
        month = mon + 10;
        year--;
    }

    // now we can simply add all the contributions together
    return ((year / 100) * DAYS_PER_400_YEARS) / 4
            + ((year % 100) * DAYS_PER_4_YEARS) / 4
            + (month * DAYS_PER_5_MONTHS + 2) / 5
            + day
            - JDN_OFFSET;
}

// [wewnêtrzna]
// Napisana tak ¿eby by³a bezpieczna w¹tkowo, nie to co standardowa localtime.
void MyLocalTime(struct tm *OutTm, time_t Time)
{
#ifdef WIN32
	int R = localtime_s(OutTm, &Time);
	assert( R == 0 );

#else
	localtime_r(&Time, OutTm);

#endif
}


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// TIMESPAN

void TIMESPAN::Set(int Hours, int Minutes, int64 Seconds, int64 Milliseconds)
{
	// assign first to avoid precision loss
	m_Diff = Hours;
	m_Diff *= 60;
	m_Diff += Minutes;
	m_Diff *= 60;
	m_Diff += Seconds;
	m_Diff *= 1000;
	m_Diff += Milliseconds;
}

void TIMESPAN::ToString(tstring *Out, bool ShowMilliseconds) const
{
	Out->clear();

	if (ShowMilliseconds)
		Out->reserve(13);
	else
		Out->reserve(8);

	uint64 Milliseconds;

	if (IsNegative())
	{
		*Out += _T('-');
		Milliseconds = (uint64)(-GetMilliseconds());
	}
	else
		Milliseconds = (uint64)GetMilliseconds();

	uint64 Seconds = Milliseconds / 1000;
	Milliseconds -= Seconds * 1000;
	uint Minutes = (uint)(Seconds / 60);
	Seconds -= (uint64)Minutes * 60;
	uint Hours = Minutes / 60;
	Minutes -= Hours * 60;

	tstring s;
	UintToStr(&s, Hours);
	Out->append(s);
	*Out += _T(':');
	UintToStr2(&s, Minutes, 2);
	Out->append(s);
	*Out += _T(':');
	UintToStr2(&s, Seconds, 2);
	Out->append(s);
	if (ShowMilliseconds)
	{
		*Out += _T(':');
		UintToStr2(&s, Milliseconds, 3);
		Out->append(s);
	}
}

void TIMESPAN::ToString_Nice(tstring *Out, bool ShowMilliseconds) const
{
	uint64 Seconds;

	if (IsNegative())
	{
		*Out = _T('-');
		Seconds = (uint64)(-GetSeconds());
	}
	else
	{
		Out->clear();
		Seconds = (uint64)GetSeconds();
	}

	tstring s;
	// "# s"
	if (Seconds < 60)
	{
		UintToStr(&s, Seconds);
		Out->append(s);
		Out->append(_T(" s"));
	}
	// "#:## min"
	else if (Seconds < SECONDS_PER_HOUR)
	{
		uint Minutes = (uint)(Seconds / 60);
		Seconds -= (uint64)Minutes * 60;
		UintToStr(&s, Minutes);
		*Out = s;
		*Out += _T(':');
		UintToStr2(&s, Seconds, 2);
		Out->append(s);
		Out->append(_T(" min"));
	}
	// "#:## h"
	else
	{
		uint Minutes = (uint)(Seconds / 60);
		uint Hours = Minutes / 60;
		Minutes -= Hours * 60;
		UintToStr(&s, Hours);
		*Out = s;
		*Out += _T(':');
		UintToStr2(&s, Minutes, 2);
		Out->append(s);
		Out->append(_T(" h"));
	}

	if (ShowMilliseconds)
	{
		int64 Milliseconds = IsNegative() ? -m_Diff : m_Diff;
		Milliseconds %= 1000;
		UintToStr(&s, (uint)Milliseconds);
		*Out += ' ';
		*Out += s;
		*Out += _T(" ms");
	}
}


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// TMSTRUCT

void TMSTRUCT::ComputeWeekDay() const
{
	// compute the week day from day/month/year: we use the dumbest algorithm
	// possible: just compute our JDN and then use the (simple to derive)
	// formula: weekday = (JDN + 1.5) % 7
	int d = (GetTruncatedJDN(mday, mon, year) + 2) % 7;
	wday = (WEEKDAY)d;
}

void TMSTRUCT::Set(uint16 Day, MONTH Month, int Year, uint16 Hour, uint16 Minute, uint16 Second, uint16 Millisec)
{
	assert(Month < INV_MONTH);
	assert(Day > 0 && Day <= GetNumOfDaysInMonth(Year, Month));
	assert(Hour < 64);
	assert(Minute < 60);
	assert(Second < 62);
	assert(Millisec < 1000);

    static const int YEAR_MIN_IN_RANGE = 1970;
    static const int YEAR_MAX_IN_RANGE = 2037;

	// Jeœli mieœci siê w granicach czasu Uniksa, u¿yj jego funkcji - szybsza
	if (Year >= YEAR_MIN_IN_RANGE && Year <= YEAR_MAX_IN_RANGE)
	{
		struct tm tm;
		tm.tm_year = Year - 1900;
		tm.tm_mon = Month;
		tm.tm_mday = Day;
		tm.tm_hour = Hour;
		tm.tm_min = Minute;
		tm.tm_sec = Second;
		tm.tm_isdst = -1; // na pewno? w wx by³o tutaj "mktime() will guess it"

		Set(tm);
		SetMillisecond(Millisec);
		wday = INV_WEEKDAY;
	}
	else
	{
		assert("Date out of Unix range.");

/*		// get the JDN for the midnight of this day
		m_Time = GetTruncatedJDN(Day, Month, Year);
		m_Time -= EPOCH_JDN;
		m_Time *= SECONDS_PER_DAY * TIME_T_FACTOR;

		// JDN corresponds to GMT, we take localtime
//        Add(wxTimeSpan(hour, minute, second + GetTimeZone(), millisec));
		Add(TIMESPAN(Hour, Minute, Second, Millisec));*/
	}
}

void TMSTRUCT::Set(const DATETIME &dt)
{
	time_t time = dt.GetTicks();
	assert(time != (time_t)-1);

	struct tm tm2;
	MyLocalTime(&tm2, time);

	sec = (uint16)tm2.tm_sec;
	min = (uint16)tm2.tm_min;
	hour = (uint16)tm2.tm_hour;
	mday = (uint16)tm2.tm_mday;
	mon = (MONTH)tm2.tm_mon;
	year = tm2.tm_year + 1900;
	wday = (WEEKDAY)tm2.tm_wday;

	long TimeOnly = (long)(dt.m_Time % MILLISECONDS_PER_DAY);
	msec = (uint16)(TimeOnly % 1000);
}

void TMSTRUCT::Set(const struct tm &tm)
{
	msec = 0;
	sec = (uint16)tm.tm_sec;
	min = (uint16)tm.tm_min;
	hour = (uint16)tm.tm_hour;
	mday = (uint16)tm.tm_mday;
	mon = (MONTH)tm.tm_mon;
	year = tm.tm_year + 1900;
	wday = (WEEKDAY)tm.tm_wday;
}

bool TMSTRUCT::IsValid() const
{
	// we allow for the leap seconds, although we don't use them (yet)
	return (mon < INV_MONTH) &&
		(mday <= GetNumOfDaysInMonth(year, mon)) &&
		(hour < 24) && (min < 60) && (sec < 62) && (msec < 1000);
}

bool TMSTRUCT::IsSameDate(const TMSTRUCT &tm) const
{
	return (year == tm.year && mon == tm.mon && mday == tm.mday);
}

bool TMSTRUCT::IsSameTime(const TMSTRUCT &tm) const
{
	return (hour == tm.hour && min == tm.min && sec == tm.sec && msec == tm.msec);
}

void TMSTRUCT::AddMonths(int monDiff)
{
	if (monDiff == 0) return;

	// normalize the months field
	while ( monDiff < -mon )
	{
		year--;
		monDiff += MONTHS_IN_YEAR;
	}

	while ( monDiff + mon >= MONTHS_IN_YEAR )
	{
		year++;
		monDiff -= MONTHS_IN_YEAR;
	}

	mon = (MONTH)(mon + monDiff);
	assert(mon >= 0 && mon < MONTHS_IN_YEAR);

	wday = INV_WEEKDAY;
}

void TMSTRUCT::AddDays(int dayDiff)
{
	if (dayDiff == 0) return;

	// normalize the days field
	while ( dayDiff + mday < 1 )
	{
		AddMonths(-1);
		dayDiff += GetNumOfDaysInMonth(year, mon);
	}

	mday = (uint16)( mday + dayDiff );
	while ( mday > GetNumOfDaysInMonth(year, mon) )
	{
		mday -= GetNumOfDaysInMonth(year, mon);
		AddMonths(1);
	}

	assert( mday > 0 && mday <= GetNumOfDaysInMonth(year, mon));

	wday = INV_WEEKDAY;
}

void TMSTRUCT::GetTm(struct tm *Out) const
{
	Out->tm_year = year - 1900;
	Out->tm_mon = mon;
	Out->tm_mday = mday;
	Out->tm_hour = hour;
	Out->tm_min = min;
	Out->tm_sec = sec;
	Out->tm_isdst = -1; // na pewno? w wx by³o tutaj "mktime() will guess it"
}

void TMSTRUCT::Add(const DATESPAN &d)
{
	year += d.GetYears();
	AddMonths(d.GetMonths());
	// Jeœli dzieñ poza zakresem, ogranicz do ostatniego dnia danego miesi¹ca
	if (mday > GetNumOfDaysInMonth(year, mon))
		mday = GetNumOfDaysInMonth(year, mon);
	AddDays(d.GetTotalDays());
}


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// DATETIME

bool DATETIME::IsInStdRange() const
{
	return m_Time >= 0l && (m_Time / TIME_T_FACTOR) < LONG_MAX;
}


void DATETIME::Set(time_t Time)
{
	//m_Time = Time - TIME_BASE_OFFSET;
	m_Time = Time; // Bo na naszych systemach jest liczone te¿ od 1970, czyli OFFSET=0
	m_Time *= TIME_T_FACTOR;
}

void DATETIME::Set(const struct tm &Time)
{
	struct tm tm2(Time);
	time_t timet = mktime(&tm2);
	if (timet == (time_t)-1)
		assert(0 && "mktime() failed.");
	Set(timet);
}

void DATETIME::Set(const TMSTRUCT &Time)
{
	struct tm tm;
	Time.GetTm(&tm);
	Set(tm);
}

void DATETIME::SetJDN(double JDN)
{
	// so that m_time will be 0 for the midnight of Jan 1, 1970 which is jdn EPOCH_JDN + 0.5
	JDN -= EPOCH_JDN + 0.5;
	m_Time = (int64)(JDN * MILLISECONDS_PER_DAY);
}

bool DATETIME::IsEqualUpTo(const DATETIME &dt, const TIMESPAN &ts) const
{
	DATETIME dt1 = dt - ts, dt2 = dt + ts;
	return IsBetween(dt1, dt2);
}

time_t DATETIME::GetTicks() const
{
	if (!IsInStdRange())
		return (time_t)-1;
//	return (time_t)(m_Time / TIME_T_FACTOR) + TIME_BASE_OFFSET;
	return (time_t)(m_Time / TIME_T_FACTOR);
}


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Funkcje globalne

const tchar * GetWeekDayName(WEEKDAY WeekDay, NAME_FORM Form)
{
	if (Form & NAME_LONG)
	{
		if (Form & NAME_UPPERCASE)
		{
			if (WeekDay >= INV_WEEKDAY)
				return WEEKDAY_NAMES_LONG_UPPERCASE[INV_WEEKDAY];
			return WEEKDAY_NAMES_LONG_UPPERCASE[WeekDay];
		}
		else if (Form & NAME_FIRST_UPPERCASE)
		{
			if (WeekDay >= INV_WEEKDAY)
				return WEEKDAY_NAMES_LONG_FIRST_UPPERCASE[INV_WEEKDAY];
			return WEEKDAY_NAMES_LONG_FIRST_UPPERCASE[WeekDay];
		}
		else
		{
			if (WeekDay >= INV_WEEKDAY)
				return WEEKDAY_NAMES_LONG_LOWERCASE[INV_WEEKDAY];
			return WEEKDAY_NAMES_LONG_LOWERCASE[WeekDay];
		}
	}
	else
	{
		if (Form & NAME_UPPERCASE)
		{
			if (WeekDay >= INV_WEEKDAY)
				return WEEKDAY_NAMES_SHORT_UPPERCASE[INV_WEEKDAY];
			return WEEKDAY_NAMES_SHORT_UPPERCASE[WeekDay];
		}
		else if (Form & NAME_FIRST_UPPERCASE)
		{
			if (WeekDay >= INV_WEEKDAY)
				return WEEKDAY_NAMES_SHORT_FIRST_UPPERCASE[INV_WEEKDAY];
			return WEEKDAY_NAMES_SHORT_FIRST_UPPERCASE[WeekDay];
		}
		else
		{
			if (WeekDay >= INV_WEEKDAY)
				return WEEKDAY_NAMES_SHORT_LOWERCASE[INV_WEEKDAY];
			return WEEKDAY_NAMES_SHORT_LOWERCASE[WeekDay];
		}
	}
}

const tchar * GetMonthName(MONTH Month, NAME_FORM Form)
{
	if (Form & NAME_LONG)
	{
		if (Form & NAME_UPPERCASE)
		{
			if (Month >= INV_MONTH)
				return MONTH_NAMES_LONG_UPPERCASE[INV_MONTH];
			return MONTH_NAMES_LONG_UPPERCASE[Month];
		}
		else if (Form & NAME_FIRST_UPPERCASE)
		{
			if (Month >= INV_MONTH)
				return MONTH_NAMES_LONG_FIRST_UPPERCASE[INV_MONTH];
			return MONTH_NAMES_LONG_FIRST_UPPERCASE[Month];
		}
		else
		{
			if (Month >= INV_MONTH)
				return MONTH_NAMES_LONG_LOWERCASE[INV_MONTH];
			return MONTH_NAMES_LONG_LOWERCASE[Month];
		}
	}
	else
	{
		if (Form & NAME_UPPERCASE)
		{
			if (Month >= INV_MONTH)
				return MONTH_NAMES_SHORT_UPPERCASE[INV_MONTH];
			return MONTH_NAMES_SHORT_UPPERCASE[Month];
		}
		else if (Form & NAME_FIRST_UPPERCASE)
		{
			if (Month >= INV_MONTH)
				return MONTH_NAMES_SHORT_FIRST_UPPERCASE[INV_MONTH];
			return MONTH_NAMES_SHORT_FIRST_UPPERCASE[Month];
		}
		else
		{
			if (Month >= INV_MONTH)
				return MONTH_NAMES_SHORT_LOWERCASE[INV_MONTH];
			return MONTH_NAMES_SHORT_LOWERCASE[Month];
		}
	}
}

uint GetNumOfDaysInMonth(int Year, uint Month)
{
	return DAYS_IN_MONTH[IsLeapYear(Year)][Month];
}

DATETIME Now()
{
	return DATETIME(GetTimeNow());
}

DATETIME UNow()
{
	DATETIME R;

#ifdef WIN32
	SYSTEMTIME thenst = { 1970, 1, 4, 1, 0, 0, 0, 0 }; // 00:00:00 Jan 1st 1970
	FILETIME thenft;
	SystemTimeToFileTime(&thenst, &thenft);
	int64 then = (int64)thenft.dwHighDateTime * 0x100000000 + thenft.dwLowDateTime; // time in 100 nanoseconds

	SYSTEMTIME nowst;
	GetSystemTime(&nowst);
	FILETIME nowft;
	SystemTimeToFileTime(&nowst, &nowft);
	int64 now = (int64)nowft.dwHighDateTime * 0x100000000 + nowft.dwLowDateTime; // time in 100 nanoseconds

	R.m_Time = (now - then) / 10000;  // time from 00:00:00 Jan 1st 1970 to now in milliseconds

#else
	struct timeval tp;
	int gtod_r = gettimeofday(&tp, NULL);
	assert(gtod_r == 0 && "gettimeofday failed.");
	R.m_Time = (int64)tp.tv_sec * 1000;
	R.m_Time = (R.m_Time + (tp.tv_usec / 1000));

#endif

	return R;
}

time_t GetTimeNow()
{
	time_t R;
	time(&R);
	return R;
}

void GetTmNow(struct tm *Out)
{
	time_t Time = GetTimeNow();
	struct tm tmstruct;
	MyLocalTime(&tmstruct, Time);
	*Out = tmstruct;
}

void DateToStr(tstring *Out, const TMSTRUCT &tm, const tstring &Format)
{
	Out->clear();
	Out->reserve(Format.length()*2);
	size_t fi = 0;
	tstring s;
	while (fi < Format.length())
	{
		switch (Format[fi])
		{
		case _T('h'):
			UintToStr(&s, tm.GetHour());
			Out->append(s);
			fi++;
			break;
		case _T('H'):
			UintToStr2(&s, tm.GetHour(), 2);
			Out->append(s);
			fi++;
			break;
		case _T('m'):
			UintToStr(&s, tm.GetMinute());
			Out->append(s);
			fi++;
			break;
		case _T('M'):
			UintToStr2(&s, tm.GetMinute(), 2);
			Out->append(s);
			fi++;
			break;
		case _T('s'):
			UintToStr(&s, tm.GetSecond());
			Out->append(s);
			fi++;
			break;
		case _T('S'):
			UintToStr2(&s, tm.GetSecond(), 2);
			Out->append(s);
			fi++;
			break;
		case _T('d'):
			UintToStr(&s, tm.GetDay());
			Out->append(s);
			fi++;
			break;
		case _T('D'):
			UintToStr2(&s, tm.GetDay(), 2);
			Out->append(s);
			fi++;
			break;
		case _T('y'):
			IntToStr2(&s, (tm.GetYear() > 99 ? tm.GetYear() % 100 : tm.GetYear()), 2);
			Out->append(s);
			fi++;
			break;
		case _T('Y'):
			IntToStr2(&s, tm.GetYear(), 4);
			Out->append(s);
			fi++;
			break;
		case _T('i'):
			IntToStr(&s, tm.GetMillisecond());
			Out->append(s);
			fi++;
			break;
		case _T('I'):
			IntToStr2(&s, tm.GetMillisecond(), 3);
			Out->append(s);
			fi++;
			break;

		case _T('w'):
		case _T('W'):
			{
				bool Textual = false;
				uint NameForm = (Format[fi] == _T('W') ? NAME_LONG : NAME_SHORT);
				fi++;
				if (fi < Format.length())
				{
					if (Format[fi] == _T('l'))
					{
						NameForm |= NAME_LOWERCASE;
						Textual = true;
						fi++;
					}
					else if (Format[fi] == _T('f'))
					{
						NameForm |= NAME_FIRST_UPPERCASE;
						Textual = true;
						fi++;
					}
					else if (Format[fi] == _T('u'))
					{
						NameForm |= NAME_UPPERCASE;
						Textual = true;
						fi++;
					}
				}

				if (Textual)
					Out->append(GetWeekDayName(tm.GetWeekDay(), (NAME_FORM)NameForm));
				else
				{
					WEEKDAY WeekDay = tm.GetWeekDay();
					UintToStr(&s, (WeekDay == SUN ? 7 : (uint)WeekDay));
					Out->append(s);
				}
			}
			break;

		case _T('n'):
		case _T('N'):
			{
				bool Textual = false;
				uint NameForm = (Format[fi] == _T('N') ? NAME_LONG : NAME_SHORT);
				fi++;
				if (fi < Format.length())
				{
					if (Format[fi] == _T('l'))
					{
						NameForm |= NAME_LOWERCASE;
						Textual = true;
						fi++;
					}
					else if (Format[fi] == _T('f'))
					{
						NameForm |= NAME_FIRST_UPPERCASE;
						Textual = true;
						fi++;
					}
					else if (Format[fi] == _T('u'))
					{
						NameForm |= NAME_UPPERCASE;
						Textual = true;
						fi++;
					}
				}

				if (Textual)
					Out->append(GetMonthName(tm.GetMonth(), (NAME_FORM)NameForm));
				else
				{
					MONTH Month = tm.GetMonth();
					if (NameForm == NAME_LONG)
						UintToStr2(&s, (uint)Month+1, 2);
					else
						UintToStr(&s, (uint)Month+1);
					Out->append(s);
				}
			}
			break;

		default:
			*Out += Format[fi];
			fi++;
		}
	}
}

WEEKDAY StrToWeekDay(const tstring &s)
{
	for (uint i = 0; i < 7; i++)
	{
		if (s == WEEKDAY_NAMES_SHORT_LOWERCASE[i] ||
			s == WEEKDAY_NAMES_SHORT_FIRST_UPPERCASE[i] ||
			s == WEEKDAY_NAMES_SHORT_UPPERCASE[i] ||
			s == WEEKDAY_NAMES_LONG_LOWERCASE[i] ||
			s == WEEKDAY_NAMES_LONG_FIRST_UPPERCASE[i] ||
			s == WEEKDAY_NAMES_LONG_UPPERCASE[i])
		{
			return (WEEKDAY)i;
		}
	}
	return INV_WEEKDAY;
}

MONTH StrToMonth(const tstring &s)
{
	for (uint i = 0; i < 12; i++)
	{
		if (s == MONTH_NAMES_SHORT_LOWERCASE[i] ||
			s == MONTH_NAMES_SHORT_FIRST_UPPERCASE[i] ||
			s == MONTH_NAMES_SHORT_UPPERCASE[i] ||
			s == MONTH_NAMES_LONG_LOWERCASE[i] ||
			s == MONTH_NAMES_LONG_FIRST_UPPERCASE[i] ||
			s == MONTH_NAMES_LONG_UPPERCASE[i])
		{
			return (MONTH)i;
		}
	}
	return INV_MONTH;
}

// [wewnêtrzna]
void ParseNumber(tstring *Out, const tstring &s, size_t *InOutPos)
{
	Out->clear();
	while (*InOutPos < s.length() && CharIsDigit(s[*InOutPos]))
	{
		*Out += s[*InOutPos];
		(*InOutPos)++;
	}
}

// [wewnêtrzna]
bool CharIsAlphaNumeric_f(tchar ch)
{
	return
		(ch >= _T('0') && ch <= _T('9')) ||
		(ch >= _T('A') && ch <= _T('Z')) ||
		(ch >= _T('a') && ch <= _T('z')) ||
		(ch == _T('¹')) || (ch == _T('æ')) || (ch == _T('ê')) || (ch == _T('³')) || (ch == _T('ñ')) || (ch == _T('ó')) || (ch == _T('œ')) || (ch == _T('¿')) || (ch == _T('Ÿ')) ||
		(ch == _T('¥')) || (ch == _T('Æ')) || (ch == _T('Ê')) || (ch == _T('£')) || (ch == _T('Ñ')) || (ch == _T('Ó')) || (ch == _T('Œ')) || (ch == _T('¯')) || (ch == _T(''));
}

// [wewnêtrzna]
void ParseName(tstring *Out, const tstring &s, size_t *InOutPos)
{
	Out->clear();
	while (*InOutPos < s.length() && CharIsAlphaNumeric_f(s[*InOutPos]))
	{
		*Out += s[*InOutPos];
		(*InOutPos)++;
	}
}

bool StrToDate(TMSTRUCT *Out, const tstring &s, const tstring &Format)
{
	int Year = 0;
	MONTH Month = JAN;
	uint16 Day = 0;
	uint16 Hour = 0;
	uint16 Minute = 0;
	uint16 Second = 0;
	uint16 Millisecond = 0;

	size_t fi = 0, si = 0;
	tstring Tmp;
	while (fi < Format.length())
	{
		switch (Format[fi])
		{
		case _T('h'):
		case _T('H'):
			if (Hour != 0) return false;
			ParseNumber(&Tmp, s, &si);
			fi++;
			if (StrToUint(&Hour, Tmp) != 0) return false;
			break;
		case _T('m'):
		case _T('M'):
			if (Minute != 0) return false;
			ParseNumber(&Tmp, s, &si);
			fi++;
			if (StrToUint(&Minute, Tmp) != 0) return false;
			break;
		case _T('s'):
		case _T('S'):
			if (Second != 0) return false;
			ParseNumber(&Tmp, s, &si);
			fi++;
			if (StrToUint(&Second, Tmp) != 0) return false;
			break;
		case _T('d'):
		case _T('D'):
			if (Day != 0) return false;
			ParseNumber(&Tmp, s, &si);
			fi++;
			if (StrToUint(&Day, Tmp) != 0) return false;
			break;
		case _T('i'):
		case _T('I'):
			if (Millisecond != 0) return false;
			ParseNumber(&Tmp, s, &si);
			fi++;
			if (StrToUint(&Millisecond, Tmp) != 0) return false;
			break;
		case _T('y'):
		case _T('Y'):
			{
				bool Short = (Format[fi] == _T('y'));
				if (Year != 0) return false;
				ParseNumber(&Tmp, s, &si);
				fi++;
				if (StrToInt(&Year, Tmp) != 0) return false;
				if (Short)
				{
					if (Year >= 70)
						Year += 1900;
					else
						Year += 2000;
				}
			}
			break;

		case _T('w'):
		case _T('W'):
			fi++;
			// Tekstowe
			if (fi < Format.length() && (Format[fi] == _T('l') || Format[fi] == _T('f') || Format[fi] == _T('u')))
			{
				fi++;
				ParseName(&Tmp, s, &si);
			}
			// Liczbowe
			else
				ParseNumber(&Tmp, s, &si);
			break;

		case _T('n'):
		case _T('N'):
			fi++;
			// Tekstowe
			if (fi < Format.length() && (Format[fi] == _T('l') || Format[fi] == _T('f') || Format[fi] == _T('u')))
			{
				fi++;
				ParseName(&Tmp, s, &si);
				Month = StrToMonth(Tmp);
				if (Month == INV_MONTH) return false;
			}
			// Liczbowe
			else
			{
				ParseNumber(&Tmp, s, &si);
				uint MonthNumber;
				if (StrToUint(&MonthNumber, Tmp) != 0) return false;
				Month = (MONTH)MonthNumber;
			}
			break;

		default:
			if (si >= s.length()) return false;
			if (s[si] != Format[fi]) return false;
			si++;
			fi++;
		}
	}

	*Out = TMSTRUCT(Day, Month, Year, Hour, Minute, Second, Millisecond);
	return true;
}

//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// class GameTime

#ifdef WIN32

const GameTime GameTime::ZERO = GameTime(0ll);
const GameTime GameTime::MIN_VALUE = GameTime(MININT64);
const GameTime GameTime::MAX_VALUE = GameTime(MAXINT64);
bool GameTime::s_Initialized = false;
int64 GameTime::s_PerfFreq = 0;
int64 GameTime::s_StartPerfCount = 0;

GameTime GetCurrentGameTime()
{
	ASSERT_INT3_DEBUG(GameTime::s_Initialized);

	int64 v;

#ifdef WIN32
	QueryPerformanceCounter((LARGE_INTEGER*)&v);
#else
	timeval tv;
	gettimeofday(&tv, 0);
	v = (int64)tv.tv_sec * 1000000ll + (int64)tv.tv_usec;
#endif

	return GameTime(v - GameTime::s_StartPerfCount);
}

GameTime MillisecondsToGameTime(int Milliseconds)
{
	return MillisecondsToGameTime((int64)Milliseconds);
}

GameTime MillisecondsToGameTime(int64 Milliseconds)
{
	ASSERT_INT3_DEBUG(GameTime::s_Initialized);

	return GameTime(Milliseconds * GameTime::s_PerfFreq / 1000);
}

GameTime SecondsToGameTime(float Seconds)
{
	ASSERT_INT3_DEBUG(GameTime::s_Initialized);

	return SecondsToGameTime((double)Seconds);
}

GameTime SecondsToGameTime(double Seconds)
{
	ASSERT_INT3_DEBUG(GameTime::s_Initialized);

	return GameTime((int64)(Seconds * (double)GameTime::s_PerfFreq));
}

int64 GameTime::ToMilliseconds() const
{
	return round_div(m_PerfCount * 1000ll, s_PerfFreq);
}

void GameTime::Initialize()
{
	ASSERT_INT3_DEBUG(!s_Initialized);

#ifdef WIN32
	QueryPerformanceFrequency((LARGE_INTEGER*)&s_PerfFreq);
	QueryPerformanceCounter((LARGE_INTEGER*)&s_StartPerfCount);
#else
	timeval tv;
	gettimeofday(&tv, 0);
	s_StartPerfCount = (int64)tv.tv_sec * 1000000ll + (int64)tv.tv_usec;
	s_PerfFreq = 1000000ll; // Microseconds
#endif

	s_Initialized = true;
}

#endif

} // namespace common
