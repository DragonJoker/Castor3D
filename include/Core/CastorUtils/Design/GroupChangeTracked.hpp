/*
See LICENSE file in root folder
*/
#ifndef ___CU_GroupChangeTracked_H___
#define ___CU_GroupChangeTracked_H___

#include "CastorUtils/CastorUtilsPrerequisites.hpp"

#include <vector>

namespace castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Templated class that provide std::array style buffer view.
	\~french
	\brief		Classe template qui fournit une vue sur un tampon, à la manière d'un std::array.
	*/
	template< typename T >
	class GroupChangeTracked
	{
	public:
		explicit GroupChangeTracked( bool & dirty )noexcept
			: m_value{}
			, m_dirty{ dirty }
		{
		}

		GroupChangeTracked( bool & dirty, T const & rhs )noexcept
			: m_value{ rhs }
			, m_dirty{ dirty }
		{
		}

		GroupChangeTracked & operator=( T const & rhs )noexcept
		{
			m_dirty |= m_value != rhs;
			m_value = rhs;
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
		bool & m_dirty;
	};

	template< typename T >
	bool operator==( GroupChangeTracked< T > const & lhs
		, T const & rhs )
	{
		return lhs.value() == rhs;
	}

	template< typename T >
	bool operator==( T const & lhs
		, GroupChangeTracked< T > const & rhs )
	{
		return lhs == rhs.value();
	}

	template< typename T >
	bool operator==( GroupChangeTracked< T > const & lhs
		, GroupChangeTracked< T > const & rhs )
	{
		return lhs.value() == rhs.value();
	}

	template< typename T >
	bool operator!=( GroupChangeTracked< T > const & lhs
		, T const & rhs )
	{
		return !operator==( lhs, rhs );
	}

	template< typename T >
	bool operator!=( T const & lhs
		, GroupChangeTracked< T > const & rhs )
	{
		return !operator==( lhs, rhs );
	}

	template< typename T >
	bool operator!=( GroupChangeTracked< T > const & lhs
		, GroupChangeTracked< T > const & rhs )
	{
		return !operator==( lhs, rhs );
	}
}

#endif
