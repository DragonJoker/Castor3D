#ifndef ___Castor_SmartPtrCommon___
#define ___Castor_SmartPtrCommon___

#include "SharedCount.h"

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
		DeleterBase(){}
		~DeleterBase(){}

		virtual void Delete()=0;
	};

	template <class T>
	class DeleterPtr : public DeleterBase
	{
	private:
		T * m_pPointer;

	public:
		DeleterPtr( T * p_pPointer)
			:	m_pPointer( p_pPointer)
		{
		}
		~DeleterPtr()
		{
		}
		virtual void Delete()
		{
			CheckedDelete( m_pPointer);
			m_pPointer = NULL;
		}
	};

	template <class T>
	class DeleterArray : public DeleterBase
	{
	private:
		T * m_pPointer;

	public:
		DeleterArray( T * p_pPointer)
			:	m_pPointer( p_pPointer)
		{
		}
		~DeleterArray()
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