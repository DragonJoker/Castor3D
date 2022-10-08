/*
See LICENSE file in root folder
*/
#ifndef ___CU_GroupChangeTracked_H___
#define ___CU_GroupChangeTracked_H___

#include "CastorUtils/Design/DesignModule.hpp"

#include <vector>

namespace castor
{
	template< typename ValueT, typename ControlT >
	class GroupChangeTrackedT
	{
	public:
		using Type = ValueT;

	public:
		explicit GroupChangeTrackedT( ControlT & dirty )noexcept
			: m_value{}
			, m_dirty{ dirty }
		{
		}

		GroupChangeTrackedT( ControlT & dirty, ValueT rhs )noexcept
			: m_value{ std::move( rhs ) }
			, m_dirty{ dirty }
		{
		}

		GroupChangeTrackedT & operator=( ValueT const & rhs )noexcept
		{
			m_dirty = m_dirty || ( m_value != rhs );
			m_value = rhs;
			return *this;
		}

		GroupChangeTrackedT & operator=( GroupChangeTrackedT const & rhs )noexcept
		{
			m_dirty = m_dirty || ( m_value != rhs.m_value );
			m_value = rhs.m_value;
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

		ControlT & control()const noexcept
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
			m_dirty = true;
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
		ValueT m_value;
		ControlT & m_dirty;
	};

	template< typename ValueT, typename ControlT >
	bool operator==( GroupChangeTrackedT< ValueT, ControlT > const & lhs
		, ValueT const & rhs )
	{
		return lhs.value() == rhs;
	}

	template< typename ValueT, typename ControlT >
	bool operator==( ValueT const & lhs
		, GroupChangeTrackedT< ValueT, ControlT > const & rhs )
	{
		return lhs == rhs.value();
	}

	template< typename ValueT, typename ControlT >
	bool operator==( GroupChangeTrackedT< ValueT, ControlT > const & lhs
		, GroupChangeTrackedT< ValueT, ControlT > const & rhs )
	{
		return lhs.value() == rhs.value();
	}

	template< typename ValueT, typename ControlT >
	bool operator!=( GroupChangeTrackedT< ValueT, ControlT > const & lhs
		, ValueT const & rhs )
	{
		return !operator==( lhs, rhs );
	}

	template< typename ValueT, typename ControlT >
	bool operator!=( ValueT const & lhs
		, GroupChangeTrackedT< ValueT, ControlT > const & rhs )
	{
		return !operator==( lhs, rhs );
	}

	template< typename ValueT, typename ControlT >
	bool operator!=( GroupChangeTrackedT< ValueT, ControlT > const & lhs
		, GroupChangeTrackedT< ValueT, ControlT > const & rhs )
	{
		return !operator==( lhs, rhs );
	}

	template< typename ValueT, typename ControlT >
	GroupChangeTrackedT< ValueT, ControlT > makeGroupChangeTracked( ControlT & dirty, ValueT const & value )
	{
		return GroupChangeTrackedT< ValueT, ControlT >{ dirty, value };
	}

	template< typename ValueT, typename ControlT >
	struct IsChangeTrackedT< GroupChangeTrackedT< ValueT, ControlT > > : std::true_type
	{
	};

	template< typename ValueT, typename ControlT >
	struct IsGroupChangeTrackedT< GroupChangeTrackedT< ValueT, ControlT > > : std::true_type
	{
	};
}

#endif
