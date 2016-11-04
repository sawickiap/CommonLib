/** \file
\brief Macros for creating doubly-linked list of objects.
\author Adam Sawicki - sawickiap@poczta.onet.pl - http://asawicki.info/ \n

Part of CommonLib library. \n
Encoding UTF-8, end of line CR+LF \n
License: GNU LGPL. \n
Documentation: \ref Module_ObjList \n
Module components: \ref code_objlist
*/
#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif
#ifndef OBJ_LIST_H_
#define OBJ_LIST_H_

namespace common
{

/** \addtogroup code_objlist ObjList Module
Documentation: \ref Module_ObjList \n
Header: ObjList.hpp */
//@{

/** \addtogroup objlist_normal Normal object list
*/
//@{

/// Declares list item fields. Use in item class definition in chosen section (private recommended).
#define OBJLIST_ITEM_FIELDS(ListT, ItemT, Name)  \
	friend class ListT;  \
	ItemT *m_##Name##_Prev, *m_##Name##_Next;  \
	ListT *m_##Name##_List;

/// Defines inline list item methods. Use in item class definition in chosen section (public recommended).
#define OBJLIST_ITEM_METHODS(ListT, ItemT, Name)  \
	bool Name##_IsOnList() { return m_##Name##_List != NULL; }  \
	ListT * Name##_GetList() { return m_##Name##_List; }  \
	ItemT * Name##_GetPrev() { return m_##Name##_Prev; }  \
	ItemT * Name##_GetNext() { return m_##Name##_Next; }

/// Initializes list item fields. Use in body of item class constructor.
#define OBJLIST_ITEM_CTOR(ListT, ItemT, Name)  \
	{ m_##Name##_Prev = m_##Name##_Next = NULL; m_##Name##_List = NULL; }

/// Removes itself from the list if present on the list. Use in body of item class destructor.
#define OBJLIST_ITEM_DTOR(ListT, ItemT, Name)  \
	{ if (m_##Name##_List) m_##Name##_List->Name##_Remove(this); }

/// Declares list class static fields. Use in list class definition in chosen section (private recommended).
#define OBJLIST_LIST_FIELDS(ListT, ItemT, Name)  \
	ItemT *m_##Name##_First, *m_##Name##_Last;  \
	uint m_##Name##_ItemCount;

/// Declares list class methods. Use in list class definition in chosen section (public recommended).
#define OBJLIST_LIST_METHODS(ListT, ItemT, Name)  \
	ItemT * Name##_GetFirst() { return m_##Name##_First; }  \
	ItemT * Name##_GetLast() { return m_##Name##_Last; }  \
	uint Name##_GetItemCount() { return m_##Name##_ItemCount; }  \
	bool Name##_IsEmpty() { return m_##Name##_ItemCount == 0; }  \
	void Name##_InsertBack(ItemT *v);  \
	void Name##_InsertFront(ItemT *v);  \
	void Name##_InsertAfter(ItemT *v, ItemT *existingItem);  \
	void Name##_InsertBefore(ItemT *v, ItemT *existingItem);  \
	void Name##_Remove(ItemT *v);  \
	void Name##_Clear();

/// Initializes list class fields. Use in body of list class constructor.
#define OBJLIST_LIST_CTOR(ListT, ItemT, Name)  \
	{ m_##Name##_First = m_##Name##_Last = NULL; m_##Name##_ItemCount = 0; }

/// Removes all items from the list. Use in body of list class destructor.
#define OBJLIST_LIST_DTOR(ListT, ItemT, Name)  \
	{  \
		ItemT *v, *vNext;  \
		for (v = m_##Name##_First; v != NULL; v = vNext)  \
		{  \
			assert(v->m_##Name##_List == this);  \
			vNext = v->m_##Name##_Next;  \
			v->m_##Name##_Prev = v->m_##Name##_Next = NULL;  \
			v->m_##Name##_List = NULL;  \
		}  \
	}

/// Defines list class methods implementation. Use in CPP file with implementation of list class.
#define OBJLIST_LIST_IMPL(ListT, ItemT, Name) \
	void ListT::Name##_InsertBack(ItemT *v)  \
	{  \
		assert(v->m_##Name##_List == NULL && v->m_##Name##_Prev == NULL && v->m_##Name##_Next == NULL);  \
		m_##Name##_ItemCount++;  \
		v->m_##Name##_List = this;  \
		if (m_##Name##_Last == NULL)  \
			m_##Name##_First = m_##Name##_Last = v;  \
		else  \
		{  \
			v->m_##Name##_Prev = m_##Name##_Last;  \
			m_##Name##_Last->m_##Name##_Next = v;  \
			m_##Name##_Last = v;  \
		}  \
	}  \
	void ListT::Name##_InsertFront(ItemT *v)  \
	{  \
		assert(v->m_##Name##_List == NULL && v->m_##Name##_Prev == NULL && v->m_##Name##_Next == NULL);  \
		m_##Name##_ItemCount++;  \
		v->m_##Name##_List = this;  \
		if (m_##Name##_First == NULL)  \
			m_##Name##_First = m_##Name##_Last = v;  \
		else  \
		{  \
			v->m_##Name##_Next = m_##Name##_First;  \
			m_##Name##_First->m_##Name##_Prev = v;  \
			m_##Name##_First = v;  \
		}  \
	}  \
	void ListT::Name##_InsertAfter(ItemT *v, ItemT *existingItem)  \
	{  \
		assert(existingItem->m_##Name##_List == this);  \
		if (existingItem->m_##Name##_Next == NULL)  \
			Name##_InsertBack(v);  \
		else  \
		{  \
			assert(v->m_##Name##_List == NULL && v->m_##Name##_Prev == NULL && v->m_##Name##_Next == NULL);  \
			m_##Name##_ItemCount++;  \
			v->m_##Name##_List = this;  \
			ItemT *nextItem = existingItem->m_##Name##_Next;  \
			existingItem->m_##Name##_Next = v;  \
			v->m_##Name##_Prev = existingItem;  \
			v->m_##Name##_Next = nextItem;  \
			nextItem->m_##Name##_Prev = v;  \
		}  \
	}  \
	void ListT::Name##_InsertBefore(ItemT *v, ItemT *existingItem)  \
	{  \
		assert(existingItem->m_##Name##_List == this);  \
		if (existingItem->m_##Name##_Prev == NULL)  \
			Name##_InsertFront(v);  \
		else  \
		{  \
			assert(v->m_##Name##_List == NULL && v->m_##Name##_Prev == NULL && v->m_##Name##_Next == NULL);  \
			m_##Name##_ItemCount++;  \
			v->m_##Name##_List = this;  \
			ItemT *prev_v = existingItem->m_##Name##_Prev;  \
			prev_v->m_##Name##_Next = v;  \
			v->m_##Name##_Prev = prev_v;  \
			v->m_##Name##_Next = existingItem;  \
			existingItem->m_##Name##_Prev = v;  \
		}  \
	}  \
	void ListT::Name##_Remove(ItemT *v)  \
	{  \
		assert(v->##Name##_GetList() == this);  \
		if (v->m_##Name##_Prev == NULL)  \
		{  \
			assert(m_##Name##_First == v);  \
			m_##Name##_First = v->m_##Name##_Next;  \
		}  \
		else  \
		{  \
			assert(v->m_##Name##_Prev->m_##Name##_Next == v);  \
			v->m_##Name##_Prev->m_##Name##_Next = v->m_##Name##_Next;  \
		}  \
		if (v->m_##Name##_Next == NULL)  \
		{  \
			assert(m_##Name##_Last == v);  \
			m_##Name##_Last = v->m_##Name##_Prev;  \
		}  \
		else  \
		{  \
			assert(v->m_##Name##_Next->m_##Name##_Prev == v);  \
			v->m_##Name##_Next->m_##Name##_Prev = v->m_##Name##_Prev;  \
		}  \
		v->m_##Name##_Next = v->m_##Name##_Prev = NULL;  \
		v->m_##Name##_List = NULL;  \
		assert(m_##Name##_ItemCount > 0);  \
		m_##Name##_ItemCount--;  \
	}  \
	void ListT::Name##_Clear()  \
	{  \
		ItemT *v, *vNext;  \
		for (v = m_##Name##_First; v != NULL; v = vNext)  \
		{  \
			vNext = v->m_##Name##_Next;  \
			v->m_##Name##_Prev = v->m_##Name##_Next = NULL;  \
			assert(v->m_##Name##_List == this);  \
			v->m_##Name##_List = NULL;  \
		}  \
		m_##Name##_First = m_##Name##_Last = NULL;  \
		m_##Name##_ItemCount = 0;  \
	}

//@}
// objlist_normal


/** \addtogroup objlist_static Static object list
*/
//@{

/// Declares list item fields. Use in item class definition in chosen section (private recommended).
#define STATIC_OBJLIST_ITEM_FIELDS(ListT, ItemT, Name)  \
	friend class ListT;  \
	ItemT *m_##Name##_Prev, *m_##Name##_Next;  \
	bool m_##NAME##_IsOnList;

/// Defines inline list item methods. Use in item class definition in chosen section (public recommended).
#define STATIC_OBJLIST_ITEM_METHODS(ListT, ItemT, Name)  \
	bool Name##_IsOnList() { return m_##Name##_IsOnList; }  \
	ItemT * Name##_GetPrev() { return m_##Name##_Prev; }  \
	ItemT * Name##_GetNext() { return m_##Name##_Next; }

/// Initializes list item fields. Use in body of item class constructor.
#define STATIC_OBJLIST_ITEM_CTOR(ListT, ItemT, Name)  \
	{ m_##Name##_Prev = m_##Name##_Next = NULL; m_##Name##_IsOnList = false; }

/// Removes itself from the list if present on the list. Use in body of item class destructor.
#define STATIC_OBJLIST_ITEM_DTOR(ListT, ItemT, Name)  \
	{ if (m_##Name##_IsOnList) ListT::##Name##_Remove(this); }

/// Declares list class static fields. Use in list class definition in chosen section (private recommended).
#define STATIC_OBJLIST_LIST_FIELDS(ListT, ItemT, Name)  \
	static ItemT *m_##Name##_First, *m_##Name##_Last;  \
	static uint m_##Name##_ItemCount;

/// Declares list class static methods. Use in list class definition in chosen section (public recommended).
#define STATIC_OBJLIST_LIST_METHODS(ListT, ItemT, Name)  \
	static ItemT * Name##_GetFirst() { return m_##Name##_First; }  \
	static ItemT * Name##_GetLast() { return m_##Name##_Last; }  \
	static uint Name##_GetItemCount() { return m_##Name##_ItemCount; }  \
	static bool Name##_IsEmpty() { return m_##Name##_ItemCount == 0; } \
	static void Name##_InsertBack(ItemT *v);  \
	static void Name##_InsertFront(ItemT *v);  \
	static void Name##_InsertAfter(ItemT *v, ItemT *existingItem);  \
	static void Name##_InsertBefore(ItemT *v, ItemT *existingItem);  \
	static void Name##_Remove(ItemT *v);  \
	static void Name##_Clear();

/// Defines list class static fields and methods implementation. Use in CPP file with implementation of list class.
#define STATIC_OBJLIST_LIST_IMPL(ListT, ItemT, Name) \
	ItemT * ListT::m_##Name##_First = NULL;  \
	ItemT * ListT::m_##Name##_Last  = NULL;  \
	uint ListT::m_##Name##_ItemCount = 0;  \
	void ListT::Name##_InsertBack(ItemT *v)  \
	{  \
		assert(!v->m_##Name##_IsOnList && v->m_##Name##_Prev == NULL && v->m_##Name##_Next == NULL);  \
		m_##Name##_ItemCount++;  \
		v->m_##Name##_IsOnList = true;  \
		if (m_##Name##_Last == NULL)  \
			m_##Name##_First = m_##Name##_Last = v;  \
		else  \
		{  \
			v->m_##Name##_Prev = m_##Name##_Last;  \
			m_##Name##_Last->m_##Name##_Next = v;  \
			m_##Name##_Last = v;  \
		}  \
	}  \
	void ListT::Name##_InsertFront(ItemT *v)  \
	{  \
		assert(!v->m_##Name##_IsOnList && v->m_##Name##_Prev == NULL && v->m_##Name##_Next == NULL);  \
		m_##Name##_ItemCount++;  \
		v->m_##Name##_IsOnList = true;  \
		if (m_##Name##_First == NULL)  \
			m_##Name##_First = m_##Name##_Last = v;  \
		else  \
		{  \
			v->m_##Name##_Next = m_##Name##_First;  \
			m_##Name##_First->m_##Name##_Prev = v;  \
			m_##Name##_First = v;  \
		}  \
	}  \
	void ListT::Name##_InsertAfter(ItemT *v, ItemT *existingItem)  \
	{  \
		assert(existingItem->m_##Name##_IsOnList);  \
		if (existingItem->m_##Name##_Next == NULL)  \
			Name##_InsertBack(v);  \
		else  \
		{  \
			assert(!v->m_##Name##_IsOnList && v->m_##Name##_Prev == NULL && v->m_##Name##_Next == NULL);  \
			m_##Name##_ItemCount++;  \
			v->m_##Name##_IsOnList = true;  \
			ItemT *nextItem = existingItem->m_##Name##_Next;  \
			existingItem->m_##Name##_Next = v;  \
			v->m_##Name##_Prev = existingItem;  \
			v->m_##Name##_Next = nextItem;  \
			nextItem->m_##Name##_Prev = v;  \
		}  \
	}  \
	void ListT::Name##_InsertBefore(ItemT *v, ItemT *existingItem)  \
	{  \
		assert(existingItem->m_##Name##_IsOnList);  \
		if (existingItem->m_##Name##_Prev == NULL)  \
			Name##_InsertFront(v);  \
		else  \
		{  \
			assert(!v->m_##Name##_IsOnList && v->m_##Name##_Prev == NULL && v->m_##Name##_Next == NULL);  \
			m_##Name##_ItemCount++;  \
			v->m_##Name##_IsOnList = true;  \
			ItemT *prev_v = existingItem->m_##Name##_Prev;  \
			prev_v->m_##Name##_Next = v;  \
			v->m_##Name##_Prev = prev_v;  \
			v->m_##Name##_Next = existingItem;  \
			existingItem->m_##Name##_Prev = v;  \
		}  \
	}  \
	void ListT::Name##_Remove(ItemT *v)  \
	{  \
		assert(v->m_##Name##_IsOnList);  \
		if (v->m_##Name##_Prev == NULL)  \
		{  \
			assert(m_##Name##_First == v);  \
			m_##Name##_First = v->m_##Name##_Next;  \
		}  \
		else  \
		{  \
			assert(v->m_##Name##_Prev->m_##Name##_Next == v);  \
			v->m_##Name##_Prev->m_##Name##_Next = v->m_##Name##_Next;  \
		}  \
		if (v->m_##Name##_Next == NULL)  \
		{  \
			assert(m_##Name##_Last == v);  \
			m_##Name##_Last = v->m_##Name##_Prev;  \
		}  \
		else  \
		{  \
			assert(v->m_##Name##_Next->m_##Name##_Prev == v);  \
			v->m_##Name##_Next->m_##Name##_Prev = v->m_##Name##_Prev;  \
		}  \
		v->m_##Name##_Next = v->m_##Name##_Prev = NULL;  \
		v->m_##Name##_IsOnList = false;  \
		assert(m_##Name##_ItemCount > 0);  \
		m_##Name##_ItemCount--;  \
	}  \
	void ListT::Name##_Clear()  \
	{  \
		ItemT *v, *vNext;  \
		for (v = m_##Name##_First; v != NULL; v = vNext)  \
		{  \
			vNext = v->m_##Name##_Next;  \
			v->m_##Name##_Prev = v->m_##Name##_Next = NULL;  \
			assert(v->m_##Name##_IsOnList);  \
			v->m_##Name##_IsOnList = false;  \
		}  \
		m_##Name##_First = m_##Name##_Last = NULL;  \
		m_##Name##_ItemCount = 0;  \
	}

//@}
// objlist_static

//@}
// code_objlist

} // namespace common

#endif
