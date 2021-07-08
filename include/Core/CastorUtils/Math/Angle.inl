#include <cmath>

namespace castor
{
	template< typename Type >
	AngleT< Type >::AngleT()noexcept
		: AngleT< Type >( 0 )
	{
	}

	template< typename Type >
	AngleT< Type >::~AngleT()noexcept
	{
	}

	template< typename Type >
	Type AngleT< Type >::degrees()const noexcept
	{
		return m_radians * RadianToDegree;
	}

	template< typename Type >
	Type AngleT< Type >::radians()const noexcept
	{
		return m_radians;
	}

	template< typename Type >
	Type AngleT< Type >::grads()const noexcept
	{
		return m_radians * RadianToGrad;
	}

	template< typename Type >
	void AngleT< Type >::degrees( double p_rAngle )noexcept
	{
		m_radians = Type( p_rAngle * DegreeToRadian );
	}

	template< typename Type >
	void AngleT< Type >::radians( double p_rAngle )noexcept
	{
		m_radians = Type( p_rAngle );
	}

	template< typename Type >
	void AngleT< Type >::grads( double p_rAngle )noexcept
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
	float AngleT< Type >::cosf()const
	{
		return std::cos( float( m_radians ) );
	}

	template< typename Type >
	float AngleT< Type >::sinf()const
	{
		return std::sin( float( m_radians ) );
	}

	template< typename Type >
	float AngleT< Type >::tanf()const
	{
		return std::tan( float( m_radians ) );
	}

	template< typename Type >
	double AngleT< Type >::cosd()const
	{
		return std::cos( double( m_radians ) );
	}

	template< typename Type >
	double AngleT< Type >::sind()const
	{
		return std::sin( double( m_radians ) );
	}

	template< typename Type >
	double AngleT< Type >::tand()const
	{
		return std::tan( double( m_radians ) );
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
	AngleT< Type > & AngleT< Type >::operator-()noexcept
	{
		m_radians = -m_radians;
		return *this;
	}

	template< typename Type >
	AngleT< Type > & AngleT< Type >::operator+=( AngleT< Type > const & p_rhs )noexcept
	{
		m_radians += p_rhs.m_radians;
		return *this;
	}

	template< typename Type >
	AngleT< Type > & AngleT< Type >::operator-=( AngleT< Type > const & p_rhs )noexcept
	{
		m_radians -= p_rhs.m_radians;
		return *this;
	}

	template< typename Type >
	AngleT< Type > & AngleT< Type >::operator*=( AngleT< Type > const & p_rhs )noexcept
	{
		m_radians *= p_rhs.m_radians;
		return *this;
	}

	template< typename Type >
	AngleT< Type > & AngleT< Type >::operator/=( AngleT< Type > const & p_rhs )noexcept
	{
		m_radians /= p_rhs.m_radians;
		return *this;
	}

	template< typename Type >
	AngleT< Type > & AngleT< Type >::operator*=( double p_rhs )noexcept
	{
		m_radians *= Type( p_rhs );
		return *this;
	}

	template< typename Type >
	AngleT< Type > & AngleT< Type >::operator/=( double p_rhs )noexcept
	{
		m_radians /= Type( p_rhs );
		return *this;
	}

	template< typename Type >
	AngleT< Type > acosT( double value )
	{
		return AngleT< Type >::fromRadians( Type( std::acos( value ) ) );
	}

	template< typename Type >
	AngleT< Type > asinT( double value )
	{
		return AngleT< Type >::fromRadians( Type( std::asin( value ) ) );
	}

	template< typename Type >
	AngleT< Type > atanT( double value )
	{
		return AngleT< Type >::fromRadians( Type( std::atan( value ) ) );
	}

	AngleT< float > acosf( double value )
	{
		return acosT< float >( value );
	}

	AngleT< float > asinf( double value )
	{
		return asinT< float >( value );
	}

	AngleT< float > atanf( double value )
	{
		return atanT< float >( value );
	}

	AngleT< double > acosd( double value )
	{
		return acosT< double >( value );
	}

	AngleT< double > asind( double value )
	{
		return asinT< double >( value );
	}

	AngleT< double > atand( double value )
	{
		return atanT< double >( value );
	}

	template< typename Type >
	bool operator==( AngleT< Type > const & p_lhs, AngleT< Type > const & p_rhs )noexcept
	{
		return std::abs( p_lhs.m_radians - p_rhs.m_radians ) < std::numeric_limits< Type >::epsilon();
	}

	template< typename Type >
	bool operator!=( AngleT< Type > const & p_lhs, AngleT< Type > const & p_rhs )noexcept
	{
		return !( p_lhs == p_rhs );
	}

	template< typename Type >
	inline bool operator<( AngleT< Type > const & p_lhs, AngleT< Type > const & p_rhs )noexcept
	{
		return p_lhs.m_radians < p_rhs.m_radians;
	}

	template< typename Type >
	inline bool operator>=( AngleT< Type > const & p_lhs, AngleT< Type > const & p_rhs )noexcept
	{
		return !( p_lhs < p_rhs );
	}

	template< typename Type >
	inline bool operator>( AngleT< Type > const & p_lhs, AngleT< Type > const & p_rhs )noexcept
	{
		return p_lhs.m_radians > p_rhs.m_radians;
	}

	template< typename Type >
	inline bool operator<=( AngleT< Type > const & p_lhs, AngleT< Type > const & p_rhs )noexcept
	{
		return !( p_lhs > p_rhs );
	}
	template< typename Type >
	AngleT< Type > operator+( AngleT< Type > const & p_lhs, AngleT< Type > const & p_rhs )noexcept
	{
		AngleT< Type > result( p_lhs );
		result += p_rhs;
		return result;
	}

	template< typename Type >
	AngleT< Type > operator-( AngleT< Type > const & p_lhs, AngleT< Type > const & p_rhs )noexcept
	{
		AngleT< Type > result( p_lhs );
		result -= p_rhs;
		return result;
	}

	template< typename Type >
	AngleT< Type > operator*( AngleT< Type > const & p_lhs, AngleT< Type > const & p_rhs )noexcept
	{
		AngleT< Type > result( p_lhs );
		result *= p_rhs;
		return result;
	}

	template< typename Type >
	AngleT< Type > operator/( AngleT< Type > const & p_lhs, AngleT< Type > const & p_rhs )noexcept
	{
		AngleT< Type > result( p_lhs );
		result /= p_rhs;
		return result;
	}

	template< typename Type >
	AngleT< Type > operator*( AngleT< Type > const & p_lhs, double p_rhs )noexcept
	{
		AngleT< Type > result( p_lhs );
		result *= p_rhs;
		return result;
	}

	template< typename Type >
	AngleT< Type > operator/( AngleT< Type > const & p_lhs, double p_rhs )noexcept
	{
		AngleT< Type > result( p_lhs );
		result /= p_rhs;
		return result;
	}
}
