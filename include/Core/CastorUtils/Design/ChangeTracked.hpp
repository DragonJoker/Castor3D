/*
See LICENSE file in root folder
*/
#ifndef ___CU_ChangeTracked_H___
#define ___CU_ChangeTracked_H___

#include "CastorUtils/Design/DesignModule.hpp"

#include <vector>

namespace c3d
{
	template< typename ValueT, typename ControlT >
	class ChangeTrackedT
	{
	public:
		ChangeTrackedT()noexcept = default;
		~ChangeTrackedT()noexcept = default;

		explicit ChangeTrackedT( c3d::Function< void() > callback )noexcept
			: m_callback{ c3d::move( callback ) }
		{
		}

		explicit ChangeTrackedT( ValueT const & rhs )noexcept
			: m_value{ rhs }
		{
		}

		explicit ChangeTrackedT( ValueT const & rhs
			, c3d::Function< void() > callback )noexcept
			: m_value{ rhs }
			, m_callback{ c3d::move( callback ) }
		{
		}

		ChangeTrackedT( ChangeTrackedT && rhs )noexcept
			: m_value{ c3d::move( rhs.m_value ) }
			, m_callback{ c3d::move( rhs.m_callback ) }
		{
			this->doCopy( m_dirty, rhs.m_dirty );
		}

		ChangeTrackedT( ChangeTrackedT const & rhs )noexcept
			: m_value{ rhs.m_value }
			, m_callback{ rhs.m_callback }
		{
		}

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

		ControlT & control()noexcept
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

			if ( lhs && m_callback )
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

		friend bool operator==( ChangeTrackedT const & lhs, ChangeTrackedT const & rhs )noexcept
		{
			return lhs.value() == rhs.value();
		}

		friend bool operator==( ChangeTrackedT const & lhs, ValueT const & rhs )noexcept
		{
			return lhs.value() == rhs;
		}

		friend bool operator==( ValueT const & lhs, ChangeTrackedT const & rhs )noexcept
		{
			return lhs == rhs.value();
		}

	private:
		ValueT m_value{};
		ControlT m_dirty{ true };
		c3d::Function< void() > m_callback;
	};

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
