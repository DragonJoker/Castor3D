#include <cmath>

#include "CastorUtils/Math/Point.hpp"

namespace c3d
{
	template< typename TypeT >
	inline TypeT LengthT< TypeT >::unit( LengthUnit u )const noexcept
	{
		return convert( m_value, m_unit, u );
	}

	template< typename TypeT >
	inline TypeT LengthT< TypeT >::kilometres()const noexcept
	{
		return convert( m_value, m_unit, LengthUnit::eKilometre );
	}

	template< typename TypeT >
	inline TypeT LengthT< TypeT >::metres()const noexcept
	{
		return convert( m_value, m_unit, LengthUnit::eMetre );
	}

	template< typename TypeT >
	inline TypeT LengthT< TypeT >::centimetres()const noexcept
	{
		return convert( m_value, m_unit, LengthUnit::eCentimetre );
	}

	template< typename TypeT >
	inline TypeT LengthT< TypeT >::millimetres()const noexcept
	{
		return convert( m_value, m_unit, LengthUnit::eMillimetre );
	}

	template< typename TypeT >
	inline TypeT LengthT< TypeT >::yards()const noexcept
	{
		return convert( m_value, m_unit, LengthUnit::eYard );
	}

	template< typename TypeT >
	inline TypeT LengthT< TypeT >::feet()const noexcept
	{
		return convert( m_value, m_unit, LengthUnit::eFoot );
	}

	template< typename TypeT >
	inline TypeT LengthT< TypeT >::inches()const noexcept
	{
		return convert( m_value, m_unit, LengthUnit::eInch );
	}

	template< typename TypeT >
	inline void LengthT< TypeT >::kilometres( double value )noexcept
	{
		m_value = convert( value, LengthUnit::eKilometre, m_unit );
	}

	template< typename TypeT >
	inline void LengthT< TypeT >::metres( double value )noexcept
	{
		m_value = convert( value, LengthUnit::eMetre, m_unit );
	}

	template< typename TypeT >
	inline void LengthT< TypeT >::centimetres( double value )noexcept
	{
		m_value = convert( value, LengthUnit::eCentimetre, m_unit );
	}

	template< typename TypeT >
	inline void LengthT< TypeT >::millimetres( double value )noexcept
	{
		m_value = convert( value, LengthUnit::eMillimetre, m_unit );
	}

	template< typename TypeT >
	inline void LengthT< TypeT >::yards( double value )noexcept
	{
		m_value = convert( value, LengthUnit::eYard, m_unit );
	}

	template< typename TypeT >
	inline void LengthT< TypeT >::feet( double value )noexcept
	{
		m_value = convert( value, LengthUnit::eFoot, m_unit );
	}

	template< typename TypeT >
	inline void LengthT< TypeT >::inches( double value )noexcept
	{
		m_value = convert( value, LengthUnit::eInch, m_unit );
	}

	template< typename TypeT >
	inline LengthT< TypeT > & LengthT< TypeT >::operator=( TypeT const & rhs )noexcept
	{
		m_value = rhs;
		return *this;
	}

	template< typename TypeT >
	inline LengthT< TypeT > LengthT< TypeT >::operator-()const noexcept
	{
		return LengthT{ -m_value, m_unit };
	}

	template< typename TypeT >
	inline LengthT< TypeT > & LengthT< TypeT >::operator+=( LengthT< TypeT > const & rhs )noexcept
	{
		m_value += convert( rhs.m_value, rhs.m_unit, m_unit );
		return *this;
	}

	template< typename TypeT >
	inline LengthT< TypeT > & LengthT< TypeT >::operator-=( LengthT< TypeT > const & rhs )noexcept
	{
		m_value -= convert( rhs.m_value, rhs.m_unit, m_unit );
		return *this;
	}

	template< typename TypeT >
	inline LengthT< TypeT > & LengthT< TypeT >::operator+=( TypeT const & rhs )noexcept
	{
		m_value += rhs;
		return *this;
	}

	template< typename TypeT >
	inline LengthT< TypeT > & LengthT< TypeT >::operator-=( TypeT const & rhs )noexcept
	{
		m_value -= rhs;
		return *this;
	}

	template< typename TypeT >
	inline LengthT< TypeT > & LengthT< TypeT >::operator*=( double rhs )noexcept
	{
		m_value *= TypeT( rhs );
		return *this;
	}

	template< typename TypeT >
	inline LengthT< TypeT > & LengthT< TypeT >::operator/=( double rhs )noexcept
	{
		m_value /= TypeT( rhs );
		return *this;
	}
}
