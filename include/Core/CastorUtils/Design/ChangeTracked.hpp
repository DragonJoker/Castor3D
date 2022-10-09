/*
See LICENSE file in root folder
*/
#ifndef ___CU_ChangeTracked_H___
#define ___CU_ChangeTracked_H___

#include "CastorUtils/Design/DesignModule.hpp"

#include <vector>

namespace castor
{
	template< typename ValueT, typename ControlT >
	class ChangeTrackedT
	{
	public:
		ChangeTrackedT()noexcept
			: m_value{}
			, m_dirty{ true }
		{
		}

		explicit ChangeTrackedT( ValueT const & rhs )noexcept
			: m_value{ rhs }
			, m_dirty{ true }
		{
		}

		ChangeTrackedT( ChangeTrackedT && rhs )noexcept
			: m_value{ std::move( rhs.m_value ) }
		{
			this->doCopy( m_dirty, rhs.m_dirty );
		}

		ChangeTrackedT( ChangeTrackedT const & rhs )noexcept
			: m_value{ rhs.m_value }
			, m_dirty{ true }
		{
		}

		~ChangeTrackedT() = default;

		ChangeTrackedT & operator=( ValueT const & rhs )noexcept
		{
			this->doCopy( m_dirty, m_dirty || ( m_value != rhs ) );
			m_value = rhs;
			return *this;
		}

		ChangeTrackedT & operator=( ChangeTrackedT const & rhs )noexcept
		{
			this->doCopy( m_dirty, m_dirty || ( m_value != rhs.m_value ) );
			m_value = rhs.m_value;
			return *this;
		}

		ChangeTrackedT & operator=( ChangeTrackedT && rhs )noexcept
		{
			this->doCopy( m_dirty, rhs.m_dirty );
			m_value = std::move( rhs.m_value );
			return *this;
		}

		void reset()noexcept
		{
			m_dirty = false;
		}

		ValueT & naked()noexcept
		{
			return m_value;
		}

		ValueT const & value()const noexcept
		{
			return m_value;
		}

		ControlT & control()noexcept
		{
			return m_dirty;
		}

		bool isDirty()const noexcept
		{
			return m_dirty;
		}

		operator ValueT const &()const noexcept
		{
			return m_value;
		}

		operator ValueT &()noexcept
		{
			return m_value;
		}

		ValueT const & operator*()const noexcept
		{
			return m_value;
		}

		ValueT & operator*()noexcept
		{
			m_dirty = true;
			return m_value;
		}

		ValueT const * operator->()const noexcept
		{
			return &m_value;
		}

		ValueT * operator->()noexcept
		{
			m_dirty = true;
			return &m_value;
		}

	private:
		void doCopy( std::atomic_bool & lhs
			, std::atomic_bool const & rhs )
		{
			lhs = rhs.load();
		}

		void doCopy( bool & lhs
			, bool const & rhs )
		{
			lhs = rhs;
		}

	private:
		ValueT m_value;
		ControlT m_dirty{ true };
	};

	template< typename ValueT, typename ControlT >
	bool operator==( ChangeTrackedT< ValueT, ControlT > const & lhs, ValueT const & rhs )
	{
		return lhs.value() == rhs;
	}

	template< typename ValueT, typename ControlT >
	bool operator==( ValueT const & lhs, ChangeTrackedT< ValueT, ControlT > const & rhs )
	{
		return lhs == rhs.value();
	}

	template< typename ValueT, typename ControlT >
	bool operator==( ChangeTrackedT< ValueT, ControlT > const & lhs, ChangeTrackedT< ValueT, ControlT > const & rhs )
	{
		return lhs.value() == rhs.value();
	}

	template< typename ValueT, typename ControlT >
	bool operator!=( ChangeTrackedT< ValueT, ControlT > const & lhs, ValueT const & rhs )
	{
		return !operator==( lhs, rhs );
	}

	template< typename ValueT, typename ControlT >
	bool operator!=( ValueT const & lhs, ChangeTrackedT< ValueT, ControlT > const & rhs )
	{
		return !operator==( lhs, rhs );
	}

	template< typename ValueT, typename ControlT >
	bool operator!=( ChangeTrackedT< ValueT, ControlT > const & lhs, ChangeTrackedT< ValueT, ControlT > const & rhs )
	{
		return !operator==( lhs, rhs );
	}

	template< typename ControlT, typename ValueT >
	ChangeTrackedT< ValueT, ControlT > makeChangeTrackedT( ValueT const & value )
	{
		return ChangeTrackedT< ValueT, ControlT >{ value };
	}

	template< typename ValueT >
	ChangeTracked< ValueT > makeChangeTracked( ValueT const & value )
	{
		return makeChangeTrackedT< bool, ValueT >( value );
	}

	template< typename ValueT, typename ControlT >
	struct IsChangeTrackedT< ChangeTrackedT< ValueT, ControlT > > : std::true_type {};
}

#endif
