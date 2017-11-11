/** \file
\brief Very fast memory allocator
\author Adam Sawicki - sawickiap@poczta.onet.pl - http://asawicki.info/ \n

Part of CommonLib library. \n
Encoding UTF-8, end of line CR+LF \n
License: GNU LGPL. \n
Documentation: \ref Module_FreeList \n
Module components: \ref code_freelist
*/
#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif
#ifndef COMMON_FREELIST_H_
#define COMMON_FREELIST_H_

#include <new> // dla bad_alloc

namespace common
{

/** \addtogroup code_freelist FreeList Module
Dokumentacja: \ref Module_FreeList \n
Nag³ówek: FreeList.hpp */
//@{

/// Alokator posiadaj¹cy sta³¹ pulê pamiêci
template <typename T>
class FreeList
{
private:
	struct FreeBlock
	{
		FreeBlock *Next;
	};

	char *m_Data;
	FreeBlock *m_FreeBlocks;
	size_t m_Capacity;
	size_t m_FreeCount;

	// Zablokowane
	FreeList(const FreeList &);
	FreeList & operator = (const FreeList &);

	T * PrvTryNew()
	{
		if (m_FreeBlocks == NULL)
			return NULL;
		T *Ptr = (T*)m_FreeBlocks;
		m_FreeBlocks = m_FreeBlocks->Next;
		m_FreeCount--;
		return Ptr;
	}

	T * PrvNew()
	{
		T *R = PrvTryNew();
		if (R == NULL)
			throw std::bad_alloc();
		return R;
	}

public:
	/** \param Capacity to maksymalna liczba elementów */
	FreeList(size_t Capacity) :
		m_Capacity(Capacity),
		m_FreeCount(Capacity)
	{
		assert(Capacity > 0);
		assert(sizeof(T) >= sizeof(FreeBlock) && "FreeList cannot work with such small elements.");

		m_Data = new char[Capacity * sizeof(T)];

		char *data_current = m_Data;
		FreeBlock *fb_prev = NULL, *fb_current;

		for (size_t i = 0; i < Capacity; i++)
		{
			fb_current = (FreeBlock*)(data_current);
			fb_current->Next = fb_prev;

			fb_prev = fb_current;
			data_current += sizeof(T);
		}

		m_FreeBlocks = fb_prev;
	}

	~FreeList()
	{
		//assert(m_FreeCount == m_Capacity && "FreeList deleted before all alocated element freed.");
		delete [] m_Data;
	}

	/// Alokacja z wywo³aniem konstruktora domyœlnego. Typy atomowe pozostaj¹ niezainicjalizowane.
	/** Próbuje zaalokowaæ. Jeœli siê nie da, zwraca NULL. */
	T * TryNew() { T *Ptr = PrvTryNew(); return Ptr ? new (Ptr) T : nullptr; }
	/// Alokacja z wywo³aniem konstruktora domyœlnego. Typy atomowe pozostaj¹ niezainicjalizowane.
	/** Alokuje. Jeœli siê nie da, rzuca wyj¹tek bad_alloc. */
	T * New   () { T *Ptr = PrvNew   (); return new (Ptr) T; }

	/// Wersje do alokacji z jawnym wywo³aniem konstruktora domyœlnego. Dla typów atomowych oznacza to wyzerowanie.
	T * TryNew_ctor() { T *Ptr = PrvTryNew(); return Ptr ? new (Ptr) T() : nullptr; }
	/// Wersje do alokacji z jawnym wywo³aniem konstruktora domyœlnego. Dla typów atomowych oznacza to wyzerowanie.
	T * New_ctor   () { T *Ptr = PrvNew   (); return new (Ptr) T(); }

	/// Wersje do alokacji z wywo³aniem konstruktora posiadaj¹cego 1, 2, 3, 4, 5 parametrów.
	template <typename T1> T * TryNew(const T1 &v1) { T *Ptr = PrvTryNew(); return Ptr ? new (Ptr) T(v1) : nullptr; }
	template <typename T1> T * New   (const T1 &v1) { T *Ptr = PrvNew   (); return new (Ptr) T(v1); }
	template <typename T1, typename T2> T * TryNew(const T1 &v1, const T2 &v2) { T *Ptr = PrvTryNew(); return Ptr ? new (Ptr) T(v1, v2) : nullptr; }
	template <typename T1, typename T2> T * New   (const T1 &v1, const T2 &v2) { T *Ptr = PrvNew   (); return new (Ptr) T(v1, v2); }
	template <typename T1, typename T2, typename T3> T * TryNew(const T1 &v1, const T2 &v2, const T3 &v3) { T *Ptr = PrvTryNew(); return Ptr ? new (Ptr) T(v1, v2, v3) : nullptr; }
	template <typename T1, typename T2, typename T3> T * New   (const T1 &v1, const T2 &v2, const T3 &v3) { T *Ptr = PrvNew   (); return new (Ptr) T(v1, v2, v3); }
	template <typename T1, typename T2, typename T3, typename T4> T * TryNew(const T1 &v1, const T2 &v2, const T3 &v3, const T4 &v4) { T *Ptr = PrvTryNew(); return Ptr ? new (Ptr) T(v1, v2, v3, v4) : nullptr; }
	template <typename T1, typename T2, typename T3, typename T4> T * New   (const T1 &v1, const T2 &v2, const T3 &v3, const T4 &v4) { T *Ptr = PrvNew   (); return new (Ptr) T(v1, v2, v3, v4); }
	template <typename T1, typename T2, typename T3, typename T4, typename T5> T * TryNew(const T1 &v1, const T2 &v2, const T3 &v3, const T4 &v4, const T5 &v5) { T *Ptr = PrvTryNew(); return Ptr ? new (Ptr) T(v1, v2, v3, v4, v5) : nullptr; }
	template <typename T1, typename T2, typename T3, typename T4, typename T5> T * New   (const T1 &v1, const T2 &v2, const T3 &v3, const T4 &v4, const T5 &v5) { T *Ptr = PrvNew   (); return new (Ptr) T(v1, v2, v3, v4, v5); }

	/// Zwalnia komórkê pamiêci zaalokowan¹ wczeœniej z tej listy.
	void Delete(T *x)
	{
		x->~T();
		FreeBlock *new_fb = (FreeBlock*)x;
		new_fb->Next = m_FreeBlocks;
		m_FreeBlocks = new_fb;
		m_FreeCount++;
	}

	/// Zwraca true, jeœli lista jest pusta - nic nie zaalokowane.
	bool IsEmpty() { return m_FreeCount == m_Capacity; }
	/// Zwraca true, jeœli lista jest pe³na - nie ma ju¿ pustego miejsca.
	bool IsFull() { return m_FreeCount == 0; }
	/** \name Statystyki w elementach */
	//@{
	size_t GetUsedCount() { return m_Capacity - m_FreeCount; }
	size_t GetFreeCount() { return m_FreeCount; }
	size_t GetCapacity() { return m_Capacity; }
	//@}
	/** \name Statystyki w bajtach */
	//@{
	size_t GetUsedSize() { return GetUsedCount() * sizeof(T); }
	size_t GetFreeSize() { return GetFreeCount() * sizeof(T); }
	size_t GetAllSize() { return m_Capacity * sizeof(T); }
	//@}

	/// Zwraca true, jeœli podany adres jest zaalokowany z tej listy
	bool BelongsTo(void *p) { return (p >= m_Data) && (p < m_Data + m_Capacity*sizeof(T)); }
};

/// Alokator z samorozszerzeaj¹c¹ siê pul¹ pamiêci
template <typename T>
class DynamicFreeList
{
private:
	size_t m_BlockCapacity;
	// Lista utrzymywana w porz¹dku od najbardziej zajêtych po najbardziej wolne.
	std::vector< FreeList<T> * > m_Blocks;

	// Zablokowane
	DynamicFreeList(const DynamicFreeList &);
	DynamicFreeList & operator = (const DynamicFreeList &);

	FreeList<T> * GetListForNew()
	{
		assert(!m_Blocks.empty());

		size_t LastIndex = m_Blocks.size()-1;
		FreeList<T> *LastList = m_Blocks[LastIndex];
		size_t LastListFreeCount = LastList->GetFreeCount();
		// Wszystko zajête - zaalokuj now¹ ca³kowicie woln¹
		if (LastListFreeCount == 0)
		{
			LastList = new FreeList<T>(m_BlockCapacity);
			m_Blocks.push_back(LastList);
		}
		// Alokacja w ostatniej, najbardziej wolnej
		// (Mo¿e to nienajlepsza opcja pamiêciowo, ale wydajnoœciowo na pewno tak.)
		else
		{
			// Sortowanie
			LastListFreeCount--;
			size_t Index = LastIndex;
			if (Index > 0) // Tylko dla optymalizacji
			{
				while (Index > 0 && m_Blocks[Index-1]->GetFreeCount() > LastListFreeCount)
				{
					m_Blocks[Index] = m_Blocks[Index-1];
					Index--;
				}
				m_Blocks[Index] = LastList;
			}
		}
		return LastList;
	}

public:
	/** \param BlockCapacity to d³ugoœæ jednego bloku, w elementach */
	DynamicFreeList(size_t BlockCapacity) :
		m_BlockCapacity(BlockCapacity)
	{
		assert(BlockCapacity > 0);

		m_Blocks.push_back(new FreeList<T>(BlockCapacity));
	}

	~DynamicFreeList()
	{
		for (size_t i = m_Blocks.size(); i--; )
			delete m_Blocks[i];
	}

	/// Alokacja z wywo³aniem konstruktora domyœlnego. Typy atomowe pozostaj¹ niezainicjalizowane.
	/** Próbuje zaalokowaæ. Jeœli siê nie da, zwraca NULL. */
	T * TryNew() { FreeList<T> *L = GetListForNew(); return L->TryNew(); }
	/// Alokacja z wywo³aniem konstruktora domyœlnego. Typy atomowe pozostaj¹ niezainicjalizowane.
	/** Alokuje. Jeœli siê nie da, rzuca wyj¹tek bad_alloc. */
	T * New   () { FreeList<T> *L = GetListForNew(); return L->New(); }

	/// Wersje do alokacji z jawnym wywo³aniem konstruktora domyœlnego. Dla typów atomowych oznacza to wyzerowanie.
	T * TryNew_ctor() { FreeList<T> *L = GetListForNew(); return L->TryNew_ctor(); }
	/// Wersje do alokacji z jawnym wywo³aniem konstruktora domyœlnego. Dla typów atomowych oznacza to wyzerowanie.
	T * New_ctor   () { FreeList<T> *L = GetListForNew(); return L->New_ctor   (); }

	/// Wersje do alokacji z wywo³aniem konstruktora posiadaj¹cego 1, 2, 3, 4, 5 parametrów.
	template <typename T1> T * TryNew(const T1 &v1) { FreeList<T> *L = GetListForNew(); return L->TryNew(v1); }
	template <typename T1> T * New   (const T1 &v1) { FreeList<T> *L = GetListForNew(); return L->New   (v1); }
	template <typename T1, typename T2> T * TryNew(const T1 &v1, const T2 &v2) { FreeList<T> *L = GetListForNew(); return L->TryNew(v1, v2); }
	template <typename T1, typename T2> T * New   (const T1 &v1, const T2 &v2) { FreeList<T> *L = GetListForNew(); return L->New   (v1, v2); }
	template <typename T1, typename T2, typename T3> T * TryNew(const T1 &v1, const T2 &v2, const T3 &v3) { FreeList<T> *L = GetListForNew(); return L->TryNew(v1, v2, v3); }
	template <typename T1, typename T2, typename T3> T * New   (const T1 &v1, const T2 &v2, const T3 &v3) { FreeList<T> *L = GetListForNew(); return L->New   (v1, v2, v3); }
	template <typename T1, typename T2, typename T3, typename T4> T * TryNew(const T1 &v1, const T2 &v2, const T3 &v3, const T4 &v4) { FreeList<T> *L = GetListForNew(); return L->TryNew(v1, v2, v3, v4); }
	template <typename T1, typename T2, typename T3, typename T4> T * New   (const T1 &v1, const T2 &v2, const T3 &v3, const T4 &v4) { FreeList<T> *L = GetListForNew(); return L->New   (v1, v2, v3, v4); }
	template <typename T1, typename T2, typename T3, typename T4, typename T5> T * TryNew(const T1 &v1, const T2 &v2, const T3 &v3, const T4 &v4, const T5 &v5) { FreeList<T> *L = GetListForNew(); return L->TryNew(v1, v2, v3, v4, v5); }
	template <typename T1, typename T2, typename T3, typename T4, typename T5> T * New   (const T1 &v1, const T2 &v2, const T3 &v3, const T4 &v4, const T5 &v5) { FreeList<T> *L = GetListForNew(); return L->New   (v1, v2, v3, v4, v5); }

	/// Zwalnia komórkê pamiêci zaalokowan¹ wczeœniej z tej listy.
	void Delete(T *x)
	{
		for (size_t i = 0; i < m_Blocks.size(); i++)
		{
			if (m_Blocks[i]->BelongsTo(x))
			{
				FreeList<T> *CurrentBlock = m_Blocks[i];
				size_t MaxIndex = m_Blocks.size()-1;

				CurrentBlock->Delete(x);

				// Sortowanie
				size_t NewFreeCount = CurrentBlock->GetFreeCount();
				if (i < MaxIndex)
				{
					size_t j = i;
					while (j < MaxIndex && NewFreeCount > m_Blocks[j+1]->GetFreeCount())
					{
						m_Blocks[j] = m_Blocks[j+1];
						j++;
					}
					m_Blocks[j] = CurrentBlock;
				}

				// Kasowanie pustej listy
				if (m_Blocks.size() > 1 && m_Blocks[MaxIndex]->IsEmpty() && m_Blocks[MaxIndex-1]->GetFreeCount() >= (m_BlockCapacity >> 2))
				{
					delete m_Blocks[MaxIndex];
					m_Blocks.pop_back();
				}

				return;
			}
		}
		assert(0 && "DynamicFreeList.Delete: Cell doesn't belong to any block from the list.");
	}

	/// Zwraca true, jeœli lista jest pusta - nic nie zaalokowane.
	bool IsEmpty()
	{
		for (size_t i = 0; i < m_Blocks.size(); i++)
			if (!m_Blocks[i]->IsEmpty())
				return false;
		return true;
	}
	/// Zwraca true, jeœli lista jest pe³na - nie ma ju¿ pustego miejsca.
	bool IsFull()
	{
		for (size_t i = 0; i < m_Blocks.size(); i++)
			if (!m_Blocks[i]->IsFull())
				return false;
		return true;
	}
	size_t GetBlockCount() { return m_Blocks.size(); }
	/** \name Statystyki w elementach */
	//@{
	size_t GetBlockCapacity() { return m_BlockCapacity; }
	size_t GetUsedCount()
	{
		size_t R = 0;
		for (size_t i = 0; i < m_Blocks.size(); i++)
			R += m_Blocks[i]->GetUsedCount();
		return R;
	}
	size_t GetFreeCount()
	{
		size_t R = 0;
		for (size_t i = 0; i < m_Blocks.size(); i++)
			R += m_Blocks[i]->GetFreeCount();
		return R;
	}
	size_t GetCapacity() { return m_BlockCapacity * m_Blocks.size(); }
	//@}
	/** \name Statystyki w bajtach */
	//@{
	size_t GetBlockSize() { return GetBlockCapacity() * sizeof(T); }
	size_t GetUsedSize() { return GetUsedCount() * sizeof(T); }
	size_t GetFreeSize() { return GetFreeCount() * sizeof(T); }
	size_t GetAllSize() { return GetCapacity() * sizeof(T); }
	//@}
};

//@}
// code_freelist

} // namespace common

#endif
