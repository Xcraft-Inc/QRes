// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1995 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#ifndef __QRESTEMPL_H__
#define __QRESTEMPL_H__


#ifndef ALL_WARNINGS
#pragma warning(disable: 4114)
#endif


/////////////////////////////////////////////////////////////////////////////
// global helpers (can be overridden)


template<class TYPE>
inline void __stdcall ConstructElements(TYPE* pElements, int nCount)
{
	// first do bit-wise zero initialization
	memset((void*)pElements, 0, nCount * sizeof(TYPE));

	// then call the constructor(s)
	for (; nCount--; pElements++)
		::new((void*)pElements) TYPE;
}

template<class TYPE>
inline void __stdcall DestructElements(TYPE* pElements, int nCount)
{
	// call the destructor(s)
	for (; nCount--; pElements++)
		pElements->~TYPE();
}

template<class TYPE>
inline void __stdcall CopyElements(TYPE* pDest, const TYPE* pSrc, int nCount)
{
	// default is element-copy using assignment
	while (nCount--)
		*pDest++ = *pSrc;
}


template<class TYPE, class ARG_TYPE>
BOOL __stdcall CompareElements(const TYPE* pElement1, const ARG_TYPE* pElement2)
{
	return *pElement1 == *pElement2;
}

template<class ARG_KEY>
inline UINT __stdcall HashKey(ARG_KEY key)
{
	// default identity hash - works for most primitive values
	return ((UINT)(void*)(DWORD)key) >> 4;
}


/////////////////////////////////////////////////////////////////////////////
// CTypedPtrList<BASE_CLASS, TYPE>

template<class BASE_CLASS, class TYPE>
class CTypedPtrList : public BASE_CLASS
{
public:
// Construction
	CTypedPtrList(int nBlockSize = 10)
		: BASE_CLASS(nBlockSize) { }

	// peek at head or tail
	TYPE& GetHead()
		{ return (TYPE&)BASE_CLASS::GetHead(); }
	TYPE GetHead() const
		{ return (TYPE)BASE_CLASS::GetHead(); }
	TYPE& GetTail()
		{ return (TYPE&)BASE_CLASS::GetTail(); }
	TYPE GetTail() const
		{ return (TYPE)BASE_CLASS::GetTail(); }

	// get head or tail (and remove it) - don't call on empty list!
	TYPE RemoveHead()
		{ return (TYPE)BASE_CLASS::RemoveHead(); }
	TYPE RemoveTail()
		{ return (TYPE)BASE_CLASS::RemoveTail(); }

	// add before head or after tail
	POSITION AddHead(TYPE newElement)
		{ return BASE_CLASS::AddHead(newElement); }
	POSITION AddTail(TYPE newElement)
		{ return BASE_CLASS::AddTail(newElement); }

	// add another list of elements before head or after tail
	void AddHead(CTypedPtrList<BASE_CLASS, TYPE>* pNewList)
		{ BASE_CLASS::AddHead(pNewList); }
	void AddTail(CTypedPtrList<BASE_CLASS, TYPE>* pNewList)
		{ BASE_CLASS::AddTail(pNewList); }

	// iteration
	TYPE& GetNext(POSITION& rPosition)
		{ return (TYPE&)BASE_CLASS::GetNext(rPosition); }
	TYPE GetNext(POSITION& rPosition) const
		{ return (TYPE)BASE_CLASS::GetNext(rPosition); }
	TYPE& GetPrev(POSITION& rPosition)
		{ return (TYPE&)BASE_CLASS::GetPrev(rPosition); }
	TYPE GetPrev(POSITION& rPosition) const
		{ return (TYPE)BASE_CLASS::GetPrev(rPosition); }

	// getting/modifying an element at a given position
	TYPE& GetAt(POSITION position)
		{ return (TYPE&)BASE_CLASS::GetAt(position); }
	TYPE GetAt(POSITION position) const
		{ return (TYPE)BASE_CLASS::GetAt(position); }
	void SetAt(POSITION pos, TYPE newElement)
		{ BASE_CLASS::SetAt(pos, newElement); }
};

/////////////////////////////////////////////////////////////////////////////


#endif //__QRESTEMPL_H__

/////////////////////////////////////////////////////////////////////////////
