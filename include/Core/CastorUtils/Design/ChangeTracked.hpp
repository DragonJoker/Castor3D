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
		ChangeTrackedT()noexcept = default;
		~ChangeTrackedT()noexcept = default;

		explicit ChangeTrackedT( castor::Function< void() > callback )noexcept
			: m_callback{ castor::move( callback ) }
		{
		}

		explicit ChangeTrackedT( ValueT const & rhs )noexcept
			: m_value{ rhs }
		{
		}

		explicit ChangeTrackedT( ValueT const & rhs
			, castor::Function< void() > callback )noexcept
			: m_value{ rhs }
			, m_callback{ castor::move( callback ) }
		{
		}

		ChangeTrackedT( ChangeTrackedT && rhs )noexcept
			: m_value{ castor::move( rhs.m_value ) }
			, m_callback{ castor::move( rhs.m_callback ) }
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
			m_value = castor::move( rhs.m_value );
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

		castor::Function< void() > callback()const noexcept
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

	private:
		ValueT m_value{};
		ControlT m_dirty{ true };
		castor::Function< void() > m_callback;
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
