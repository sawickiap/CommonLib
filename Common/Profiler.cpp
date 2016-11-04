/** \file
\brief Time and performance measurement facility
\author Adam Sawicki - sawickiap@poczta.onet.pl - http://asawicki.info/ \n

Part of CommonLib library. \n
Encoding UTF-8, end of line CR+LF \n
License: GNU LGPL. \n
Documentation: \ref Module_Profiler \n
Module components: \ref code_profiler
*/
#include "Base.hpp"
#include "Profiler.hpp"


namespace common
{

//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Klasa ProfilerItem

ProfilerItem::ProfilerItem(const tstring &Name)
{
	m_Time = GameTime::ZERO;
	m_Count = 0;
	m_strName = Name;
}

ProfilerItem* ProfilerItem::Begin(const tstring &Name)
{
	// Poszukiwanie istniej¹cego elementu
	for (std::vector<ProfilerItem>::iterator it = m_ItemVector.begin(); it != m_ItemVector.end(); ++it)
	{
		if (it->GetName() == Name)
			return &(*it);
	}
	// Nie znaleziono - utworzenie nowego
	m_ItemVector.push_back( ProfilerItem(Name) );
	return &(*(--m_ItemVector.end()));
}

void ProfilerItem::FormatString(tstring *S, unsigned dwLevel, PROFILER_UNITS units)
{
	if (dwLevel > 0)
	{
		DuplicateString(S, _T("  "), dwLevel-1);
		*S += m_strName;
		*S += _T(" : ");

		if (units == PROFILER_UNITS_MILLISECONDS)
		{
			*S += DoubleToStrR(GetAvgTime().ToSeconds_d()*1000.);
			*S += _T(" ms (");
		}
		else // PROFILER_UNITS_SECONDS
		{
			*S += DoubleToStrR(GetAvgTime().ToSeconds_d());
			*S += _T(" s (");
		}
		*S += UintToStrR(GetCount());
		*S += _T(")\n");
	}

	for (uint i = 0; i < GetItemCount(); i++)
		GetItem(i)->FormatString(S, dwLevel+1, units);
}


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Klasa Profiler

Profiler::Profiler() : m_DefaultItem(EMPTY_STRING)
{
	m_ItemStack.push(&m_DefaultItem);
}

Profiler::Profiler(const tstring &Name) : m_DefaultItem(Name)
{
	m_ItemStack.push(&m_DefaultItem);
}

void Profiler::Begin(const tstring &Name)
{
	ProfilerItem* pItem = ( m_ItemStack.top() )->Begin(Name);
	pItem->Start();
	m_ItemStack.push(pItem);
}

void Profiler::End()
{
	if (m_ItemStack.size() > 1)
	{
		ProfilerItem* pItem = m_ItemStack.top();
		pItem->Stop();
		m_ItemStack.pop();
	}
	else
		// B³¹d !!!
		;
}

void Profiler::FormatString(tstring *S, PROFILER_UNITS units)
{
	S->clear();
	GetRootItem()->FormatString(S, 0, units);
}


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Klasa Profile

Profile::Profile(Profiler &profiler, const tstring &Name)
: m_Profiler(profiler)
{
	m_Profiler.Begin(Name);
}

Profile::~Profile()
{
	m_Profiler.End();
}


} // namespace common
