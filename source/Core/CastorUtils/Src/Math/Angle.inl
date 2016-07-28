#include <cmath>

namespace Castor
{
	template< typename Type >
	AngleT< Type >::AngleT()
		: AngleT< Type >( 0 )
	{
	}

	template< typename Type >
	AngleT< Type >::AngleT( AngleT< Type > const & p_angle )
		: AngleT< Type >( p_angle.m_radians )
	{
	}

	template< typename Type >
	AngleT< Type >::AngleT( AngleT< Type > && p_angle )
		: m_radians( 0 )
	{
		m_radians = std::move( p_angle.m_radians );
		p_angle.m_radians = 0;
	}

	template< typename Type >
	AngleT< Type >::~AngleT()
	{
	}

	template< typename Type >
	Type AngleT< Type >::degrees()const
	{
		return m_radians * RadianToDegree;
	}

	template< typename Type >
	Type AngleT< Type >::radians()const
	{
		return m_radians;
	}

	template< typename Type >
	Type AngleT< Type >::grads()const
	{
		return m_radians * RadianToGrad;
	}

	template< typename Type >
	void AngleT< Type >::degrees( double p_rAngle )
	{
		m_radians = Type( p_rAngle * DegreeToRadian );
	}

	template< typename Type >
	void AngleT< Type >::radians( double p_rAngle )
	{
		m_radians = Type (p_rAngle );
	}

	template< typename Type >
	void AngleT< Type >::grads( double p_rAngle )
	{
		m_radians = Type( p_rAngle * GradToRadian );
	}

	template< typename Type >
	Type AngleT< Type >::cos()const
	{
		return std::cos( m_radians );
	}

	template< typename Type >
	Type AngleT< Type >::sin()const
	{
		return std::sin( m_radians );
	}

	template< typename Type >
	Type AngleT< Type >::tan()const
	{
		return std::tan( m_radians );
	}

	template< typename Type >
	Type AngleT< Type >::cosh()const
	{
		return std::cosh( m_radians );
	}

	template< typename Type >
	Type AngleT< Type >::sinh()const
	{
		return std::sinh( m_radians );
	}

	template< typename Type >
	Type AngleT< Type >::tanh()const
	{
		return std::tanh( m_radians );
	}

	template< typename Type >
	void AngleT< Type >::acos( double p_rValue )
	{
		m_radians = Type( std::acos( p_rValue ) );
	}

	template< typename Type >
	void AngleT< Type >::asin( double p_rValue )
	{
		m_radians = Type( std::asin( p_rValue ) );
	}

	template< typename Type >
	void AngleT< Type >::atan( double p_rValue )
	{
		m_radians = Type( std::atan( p_rValue ) );
	}

	template< typename Type >
	AngleT< Type > & AngleT< Type >::operator=( AngleT< Type > const & p_rhs )
	{
		m_radians = p_rhs.m_radians;
		return *this;
	}

	template< typename Type >
	AngleT< Type > & AngleT< Type >::operator=( AngleT< Type > && p_rhs )
	{
		if ( this != &p_rhs )
		{
			m_radians = p_rhs.m_radians;
			p_rhs.m_radians = 0;
		}

		return *this;
	}

	template< typename Type >
	AngleT< Type > & AngleT< Type >::operator+=( AngleT< Type > const & p_rhs )
	{
		m_radians += p_rhs.m_radians;
		return *this;
	}

	template< typename Type >
	AngleT< Type > & AngleT< Type >::operator-=( AngleT< Type > const & p_rhs )
	{
		m_radians -= p_rhs.m_radians;
		return *this;
	}

	template< typename Type >
	AngleT< Type > & AngleT< Type >::operator*=( AngleT< Type > const & p_rhs )
	{
		m_radians *= p_rhs.m_radians;
		return *this;
	}

	template< typename Type >
	AngleT< Type > & AngleT< Type >::operator/=( AngleT< Type > const & p_rhs )
	{
		m_radians /= p_rhs.m_radians;
		return *this;
	}

	template< typename Type >
	AngleT< Type > & AngleT< Type >::operator*=( double p_rhs )
	{
		m_radians *= Type( p_rhs );
		return *this;
	}

	template< typename Type >
	AngleT< Type > & AngleT< Type >::operator/=( double p_rhs )
	{
		m_radians /= Type( p_rhs );
		return *this;
	}

	template< typename Type >
	bool operator==( AngleT< Type > const & p_lhs, AngleT< Type > const & p_rhs )
	{
		return std::abs( p_lhs.m_radians - p_rhs.m_radians ) < std::numeric_limits< Type >::epsilon();
	}

	template< typename Type >
	bool operator!=( AngleT< Type > const & p_lhs, AngleT< Type > const & p_rhs )
	{
		return !( p_lhs == p_rhs );
	}

	template< typename Type >
	AngleT< Type > operator+( AngleT< Type > const & p_lhs, AngleT< Type > const & p_rhs )
	{
		AngleT< Type > l_return( p_lhs );
		l_return += p_rhs;
		return l_return;
	}

	template< typename Type >
	AngleT< Type > operator-( AngleT< Type > const & p_lhs, AngleT< Type > const & p_rhs )
	{
		AngleT< Type > l_return( p_lhs );
		l_return -= p_rhs;
		return l_return;
	}

	template< typename Type >
	AngleT< Type > operator*( AngleT< Type > const & p_lhs, AngleT< Type > const & p_rhs )
	{
		AngleT< Type > l_return( p_lhs );
		l_return *= p_rhs;
		return l_return;
	}

	template< typename Type >
	AngleT< Type > operator/( AngleT< Type > const & p_lhs, AngleT< Type > const & p_rhs )
	{
		AngleT< Type > l_return( p_lhs );
		l_return /= p_rhs;
		return l_return;
	}

	template< typename Type >
	AngleT< Type > operator*( AngleT< Type > const & p_lhs, double p_rhs )
	{
		AngleT< Type > l_return( p_lhs );
		l_return *= p_rhs;
		return l_return;
	}

	template< typename Type >
	AngleT< Type > operator/( AngleT< Type > const & p_lhs, double p_rhs )
	{
		AngleT< Type > l_return( p_lhs );
		l_return /= p_rhs;
		return l_return;
	}
}
