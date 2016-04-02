/** \file
\brief Przyrz¹d do pomiarów czasu i wydajnoœci
\author Adam Sawicki - sawickiap@poczta.onet.pl - http://regedit.gamedev.pl/ \n

Czêœæ biblioteki CommonLib \n
Kodowanie Windows-1250, koniec wiersza CR+LF, test: Za¿ó³æ gêœl¹ jaŸñ \n
Licencja: GNU LGPL. \n
Dokumentacja: \ref Module_Profiler \n
Elementy modu³u: \ref code_profiler
*/
#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif
#ifndef COMMON_PROFILER_H_
#define COMMON_PROFILER_H_

#include <stack> // :(
#include <map> // :(
#include "DateTime.hpp"
#include "Threads.hpp"

namespace common
{

/** \addtogroup code_profiler Profile module
Documentation: \ref Module_Profiler \n
Header: Profiler.hpp */
//@{

class Profiler;

enum PROFILER_UNITS {
	PROFILER_UNITS_MILLISECONDS,
	PROFILER_UNITS_SECONDS,
};

/// Pozycja danych profilera
class ProfilerItem
{
private:
	// Sumaryczny czas wykonania
	GameTime m_Time;
	// Liczba przebiegów
	uint m_Count;
	// Czas rozpoczêcia bie¿¹cego przebiegu [s]
	GameTime m_StartTime;
	tstring m_strName; // Nazwa elementu
	std::vector<ProfilerItem> m_ItemVector; // Podelementy

	friend class Profiler;
	ProfilerItem * Begin(const tstring &Name);
	void Start() { m_StartTime = GetCurrentGameTime(); }
	void Stop() { m_Time += GetCurrentGameTime() - m_StartTime; m_Count++; }

public:
	ProfilerItem(const tstring &Name);

	const tstring &GetName() { return m_strName; }
	bool Empty() { return m_Count == 0; }
	uint GetCount() { return m_Count; }
	GameTime GetAvgTime() { return Empty() ? GameTime::ZERO : (m_Time / (int64)m_Count); }
	size_t GetItemCount() { return m_ItemVector.size(); }
	ProfilerItem* GetItem(size_t index) { return &m_ItemVector[index]; }
	void FormatString(tstring *S, unsigned dwLevel, PROFILER_UNITS units);
};

/// Profiler
class Profiler
{
public:
	Profiler();
	Profiler(const tstring &Name);
	void Begin(const tstring &Name);
	void End();
	ProfilerItem* GetRootItem() { return &m_DefaultItem; }
	/// Zapisuje ca³e drzewo profilu do ³añcucha - ka¿da pozycja w osobnym wierszu
	/** Wciêcia to dwie spacje <tt>"  "</tt>
	Koñce wiersza to <tt>\\n</tt>
	Jednostka to milisekundy. */
	void FormatString(tstring *S, PROFILER_UNITS units);

private:
	std::stack<ProfilerItem*> m_ItemStack;
	ProfilerItem m_DefaultItem;
};

/// Klasa, której obiekt mo¿esz dla wygody utworzyæ zamiast wywo³ywaæ Begin i End profilera.
/** Tworzenie tego obiektu wywo³uje Profiler::Begin, a usuwanie - Profiler::End.
Obiekt automatyczny utworzony na stosie sam siê usunie przy wyjœciu
z funkcji. Dlatego warto tworzyæ obiekt tej klasy jako zmienn¹ lokaln¹ o byle
jakiej nazwie i nieu¿ywan¹ nigdzie dalej na pocz¹tku ka¿dej profilowanej
funkcji. */
class Profile
{
private:
	Profiler &m_Profiler;
public:
	Profile(Profiler &profiler, const tstring &Name);
	~Profile();
};

/// Class to measure duration of some code in a flat manner, without any hierarchy.
/** Samples are aggregated by key and for each key average time is calculated. \n
KeyT and KeyTraits are like in std::set or std::map. KeyT must be comparable
using KeyTraits functor. KeyT must also be convertable to string with
SthToStr<KeyT> function call. \n
Class is thread-safe. */
template < typename KeyT, typename KeyTraits=std::less<KeyT> >
class FlatProfiler
{
public:
	FlatProfiler() : m_Mutex(0) { }
	/// Clears all the remembered results.
	void Clear();
	/// Registers new sample collected by custom time measurement.
	void AddSample(const KeyT &key, GameTime timeInterval);
	/// Returns multiline string with all results in unsorted order.
	void FormatString(tstring *out, PROFILER_UNITS units);

private:
	struct ENTRY
	{
		GameTime SumTime;
		uint Count;
		ENTRY() : SumTime(GameTime::ZERO), Count(0) { }
		GameTime GetAvgTime() const { return SumTime / (int64)Count; }
	};
	typedef std::map<KeyT, ENTRY, KeyTraits> MapType;

	Mutex m_Mutex;
	MapType m_Entries;
};

/// Support RIAA class to collect time sample for FlatProfiler.
/** Remembers start time in constructor and registers duration of the object
existence to given FlatProfiler in destructor. */
template < typename KeyT, typename KeyTraits=std::less<KeyT> >
class FlatProfilerSampler
{
public:
	FlatProfilerSampler(FlatProfiler<KeyT, KeyTraits> &profiler, const KeyT &key) : m_Profiler(profiler), m_Key(key), m_StartTime(GetCurrentGameTime()) { }
	~FlatProfilerSampler() { m_Profiler.AddSample(m_Key, GetCurrentGameTime() - m_StartTime); }

private:
	FlatProfiler<KeyT, KeyTraits> &m_Profiler;
	KeyT m_Key;
	GameTime m_StartTime;
};

template <typename KeyT, typename KeyTraits>
void FlatProfiler<KeyT, KeyTraits>::Clear()
{
	MutexLock lock(m_Mutex);
	m_Entries.clear();
}

template <typename KeyT, typename KeyTraits>
void FlatProfiler<KeyT, KeyTraits>::AddSample(const KeyT &key, GameTime timeInterval)
{
	MutexLock lock(m_Mutex);
	ENTRY &entry = m_Entries[key];
	entry.SumTime += timeInterval;
	entry.Count++;
}

template <typename KeyT, typename KeyTraits>
void FlatProfiler<KeyT, KeyTraits>::FormatString(tstring *out, PROFILER_UNITS units)
{
	MutexLock lock(m_Mutex);

	tstring keyStr;
	for (MapType::iterator it = m_Entries.begin(); it != m_Entries.end(); ++it)
	{
		const ENTRY &entry = it->second;

		SthToStr<KeyT>(&keyStr, it->first);
		*out += keyStr;
		*out += _T(" : ");

		if (units == PROFILER_UNITS_MILLISECONDS)
		{
			*out += DoubleToStrR(entry.GetAvgTime().ToSeconds_d()*1000.);
			*out += _T(" ms (");
		}
		else // PROFILER_UNITS_SECONDS
		{
			*out += DoubleToStrR(entry.GetAvgTime().ToSeconds_d());
			*out += _T(" s (");
		}
		*out += UintToStrR(entry.Count);
		*out += _T(")\n");
	}
}

//@}
// code_profiler

} // namespace common

/** \addtogroup code_profiler */
//@{
/** Dla jeszcze wiêszej wygody, zamiast tworzyæ obiekt klasy Profile wystarczy
na pocz¹tku guardowanej do profilowania funkcji czy dowolnego bloku { }
postawiæ to makro. */
#define PROFILE_GUARD(profiler,Name)   common::Profile __profile_guard_object(profiler, Name);
//@}

#endif
