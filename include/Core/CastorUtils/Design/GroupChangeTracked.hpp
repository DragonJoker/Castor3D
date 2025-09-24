/*
See LICENSE file in root folder
*/
#ifndef ___CU_GroupChangeTracked_H___
#define ___CU_GroupChangeTracked_H___

#include "CastorUtils/Design/DesignModule.hpp"

#include <vector>

namespace c3d
{
	template< typename ValueT, typename ControlT >
	class GroupChangeTrackedT
	{
	public:
		using Type = ValueT;

	public:
		explicit GroupChangeTrackedT( ControlT & dirty
			, c3d::Function< void() > callback = {} )noexcept
			: m_value{}
			, m_dirty{ dirty }
			, m_callback{ c3d::move( callback ) }
		{
		}

		GroupChangeTrackedT( GroupChangeTrackedT && rhs )noexcept
			: m_value{ c3d::move( rhs.m_value ) }
			, m_dirty{ rhs.m_dirty }
			, m_callback{ c3d::move( rhs.m_callback ) }
		{
		}

		GroupChangeTrackedT( GroupChangeTrackedT const & rhs )noexcept
			: m_value{ rhs.m_value }
			, m_dirty{ rhs.m_dirty }
			, m_callback{ rhs.m_callback }
		{
		}

		GroupChangeTrackedT( ControlT & dirty
			, ValueT rhs
			, c3d::Function< void() > callback = {} )noexcept
			: m_value{ c3d::move( rhs ) }
			, m_dirty{ dirty }
			, m_callback{ c3d::move( callback ) }
		{
		}

		~GroupChangeTrackedT()noexcept = default;

		GroupChangeTrackedT & operator=( ValueT const & rhs )noexcept
		{
			this->doCopy( m_dirty, m_dirty || ( m_value != rhs ) );
			m_value = rhs;
			return *this;
		}

		GroupChangeTrackedT & operator=( GroupChangeTrackedT const & rhs )noexcept
		{
			this->doCopy( m_dirty, m_dirty || ( m_value != rhs.m_value ) );
			m_value = rhs.m_value;
			return *this;
		}

		GroupChangeTrackedT & operator=( GroupChangeTrackedT && rhs )noexcept
		{
			this->doCopy( m_dirty, rhs.m_dirty );
			m_value = c3d::move( rhs.m_value );
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

		c3d::Function< void() > callback()const noexcept
		{
			return m_callback;
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
			doMakeDirty();
			return m_value;
		}

		ValueT const & operator*()const noexcept
		{
			return m_value;
		}

		ValueT & operator*()noexcept
		{
			doMakeDirty();
			return m_value;
		}

		ValueT const * operator->()const noexcept
		{
			return &m_value;
		}

		ValueT * operator->()noexcept
		{
			doMakeDirty();
			return &m_value;
		}

	private:
		void doCopy( std::atomic_bool & lhs
			, std::atomic_bool const & rhs )const
		{
			lhs = rhs.load();

			if (lhs && m_callback )
			{
				m_callback();
			}
		}

		void doCopy( bool & lhs
			, bool const & rhs )const
		{
			lhs = rhs;

			if ( lhs && m_callback )
			{
				m_callback();
			}
		}

		void doMakeDirty()
		{
			m_dirty = true;

			if ( m_callback )
			{
				m_callback();
			}
		}

		friend bool operator==( GroupChangeTrackedT const & lhs, ValueT const & rhs )noexcept
		{
			return lhs.value() == rhs;
		}

		friend bool operator==( ValueT const & lhs, GroupChangeTrackedT const & rhs )noexcept
		{
			return lhs == rhs.value();
		}

		friend bool operator==( GroupChangeTrackedT const & lhs, GroupChangeTrackedT const & rhs )noexcept
		{
			return lhs.value() == rhs.value();
		}

	private:
		ValueT m_value;
		ControlT & m_dirty;
		c3d::Function< void() > m_callback;
	};

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
