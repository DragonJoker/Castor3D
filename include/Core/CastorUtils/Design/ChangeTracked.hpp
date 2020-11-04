/*
See LICENSE file in root folder
*/
#ifndef ___CU_ChangeTracked_H___
#define ___CU_ChangeTracked_H___

#include "CastorUtils/Design/DesignModule.hpp"

#include <vector>

namespace castor
{
	template< typename T >
	class ChangeTracked
	{
	public:
		ChangeTracked()noexcept
			: m_value{}
			, m_dirty{ true }
		{
		}

		explicit ChangeTracked( T const & rhs )noexcept
			: m_value{ rhs }
			, m_dirty{ true }
		{
		}

		ChangeTracked( ChangeTracked const & rhs )noexcept
			: m_value{ rhs.m_value }
			, m_dirty{ true }
		{
		}

		ChangeTracked & operator=( T const & rhs )noexcept
		{
			m_dirty = m_dirty || ( m_value != rhs );
			m_value = rhs;
			return *this;
		}

		ChangeTracked & operator=( ChangeTracked< T > const & rhs )noexcept
		{
			m_dirty = m_dirty || ( m_value != rhs.m_value );
			m_value = rhs.m_value;
			return *this;
		}

		void reset()noexcept
		{
			m_dirty = false;
		}

		T const & value()const noexcept
		{
			return m_value;
		}

		bool isDirty()const noexcept
		{
			return m_dirty;
		}

		operator T()const noexcept
		{
			return m_value;
		}

		T const * operator->()const noexcept
		{
			return &m_value;
		}

	private:
		T m_value;
		bool m_dirty{ true };
	};

	template< typename T >
	bool operator==( ChangeTracked< T > const & lhs, T const & rhs )
	{
		return lhs.value() == rhs;
	}

	template< typename T >
	bool operator==( T const & lhs, ChangeTracked< T > const & rhs )
	{
		return lhs == rhs.value();
	}

	template< typename T >
	bool operator==( ChangeTracked< T > const & lhs, ChangeTracked< T > const & rhs )
	{
		return lhs.value() == rhs.value();
	}

	template< typename T >
	bool operator!=( ChangeTracked< T > const & lhs, T const & rhs )
	{
		return !operator==( lhs, rhs );
	}

	template< typename T >
	bool operator!=( T const & lhs, ChangeTracked< T > const & rhs )
	{
		return !operator==( lhs, rhs );
	}

	template< typename T >
	bool operator!=( ChangeTracked< T > const & lhs, ChangeTracked< T > const & rhs )
	{
		return !operator==( lhs, rhs );
	}

	template< typename T >
	ChangeTracked< T > makeChangeTracked( T const & value )
	{
		return ChangeTracked< T >{ value };
	}

	template< typename T >
	struct IsChangeTrackedT< ChangeTracked< T > > : std::true_type {};
}

#endif
