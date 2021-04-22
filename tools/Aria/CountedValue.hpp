/*
See LICENSE file in root folder
*/
#ifndef ___CTP_CountedValue_HPP___
#define ___CTP_CountedValue_HPP___

#include "Prerequisites.hpp"

namespace aria
{
	template< typename ValueT >
	struct CountedValueT
	{
		CountedValueT()
			: m_value{}
		{
		}

		explicit CountedValueT( ValueT value )
			: m_value{ std::move( value ) }
		{
		}

		CountedValueT & operator++()
		{
			++m_value;
#if CTP_UseCountedValue
			onValueChange( *this );
#endif
			return *this;
		}

		CountedValueT & operator--()
		{
			assert( m_value > 0 );
			--m_value;
#if CTP_UseCountedValue
			onValueChange( *this );
#endif
			return *this;
		}

		operator ValueT ()const
		{
			return m_value;
		}

#if CTP_UseCountedValue

	public:
		CountedValueSignalT< ValueT > onValueChange;

#endif

	private:
		ValueT m_value;
	};
}

#endif
