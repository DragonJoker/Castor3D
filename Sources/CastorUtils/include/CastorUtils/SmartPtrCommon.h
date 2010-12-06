#ifndef ___Castor_SmartPtrCommon___
#define ___Castor_SmartPtrCommon___

#include "SharedCount.h"
#include <iostream>

namespace Castor
{	namespace Templates
{
	struct static_cast_tag {};
	struct const_cast_tag {};
	struct dynamic_cast_tag {};
	struct polymorphic_cast_tag {};

	template<class T> inline void CheckedDelete( T * x)
	{
		typedef char type_must_be_complete[ sizeof( T)? 1: -1 ];
		(void) sizeof( type_must_be_complete);
		delete x;
	}

	template<class T> inline void CheckedArrayDelete(T * x)
	{
		typedef char type_must_be_complete[ sizeof(T)? 1: -1 ];
		(void) sizeof(type_must_be_complete);
		delete [] x;
	}

	class DeleterBase
	{
	public:
		static unsigned long long s_count;

	public:
		DeleterBase()
		{
//			std::cout << "New - " << ++s_count << "\n";
		}
		virtual ~DeleterBase()=0
		{
//			std::cout << "Delete - " << s_count-- << "\n";
		}

		virtual void Delete()=0;
	};

	template <class T>
	class DeleterPtr : public DeleterBase, public MemoryTraced< DeleterPtr<T> >
	{
	private:
		T * m_pPointer;

	public:
		DeleterPtr( T * p_pPointer)
			:	m_pPointer( p_pPointer)
		{
		}
		virtual ~DeleterPtr()
		{
		}
		virtual void Delete()
		{
			CheckedDelete( m_pPointer);
			m_pPointer = NULL;
		}
	};

	template <class T>
	class DeleterArray : public DeleterBase, public MemoryTraced< DeleterArray<T> >
	{
	private:
		T * m_pPointer;

	public:
		DeleterArray( T * p_pPointer)
			:	m_pPointer( p_pPointer)
		{
		}
		virtual ~DeleterArray()
		{
		}
		virtual void Delete()
		{
			CheckedArrayDelete( m_pPointer);
			m_pPointer = NULL;
		}
	};
}
}

#endif