#include <cmath>

namespace castor
{
	template< typename Type >
	inline AngleT< Type >::AngleT()noexcept
		: AngleT< Type >{ Type{} }
	{
	}

	template< typename Type >
	inline Type AngleT< Type >::turns()const noexcept
	{
		return m_radians * RadianToTurn;
	}
	
	template< typename Type >
	inline Type AngleT< Type >::degrees()const noexcept
	{
		return m_radians * RadianToDegree;
	}

	template< typename Type >
	inline Type AngleT< Type >::radians()const noexcept
	{
		return m_radians;
	}

	template< typename Type >
	inline Type AngleT< Type >::grads()const noexcept
	{
		return m_radians * RadianToGrad;
	}

	template< typename Type >
	inline void AngleT< Type >::turns( double value )noexcept
	{
		m_radians = Type( value * TurnToRadian );
	}
	
	template< typename Type >
	inline void AngleT< Type >::degrees( double value )noexcept
	{
		m_radians = Type( value * DegreeToRadian );
	}

	template< typename Type >
	inline void AngleT< Type >::radians( double value )noexcept
	{
		m_radians = Type( value );
	}

	template< typename Type >
	inline void AngleT< Type >::grads( double value )noexcept
	{
		m_radians = Type( value * GradToRadian );
	}

	template< typename Type >
	inline Type AngleT< Type >::cos()const
	{
		return std::cos( m_radians );
	}

	template< typename Type >
	inline Type AngleT< Type >::sin()const
	{
		return std::sin( m_radians );
	}

	template< typename Type >
	inline Type AngleT< Type >::tan()const
	{
		return std::tan( m_radians );
	}

	template< typename Type >
	inline float AngleT< Type >::cosf()const
	{
		return std::cos( float( m_radians ) );
	}

	template< typename Type >
	inline float AngleT< Type >::sinf()const
	{
		return std::sin( float( m_radians ) );
	}

	template< typename Type >
	inline float AngleT< Type >::tanf()const
	{
		return std::tan( float( m_radians ) );
	}

	template< typename Type >
	inline double AngleT< Type >::cosd()const
	{
		return std::cos( double( m_radians ) );
	}

	template< typename Type >
	inline double AngleT< Type >::sind()const
	{
		return std::sin( double( m_radians ) );
	}

	template< typename Type >
	inline double AngleT< Type >::tand()const
	{
		return std::tan( double( m_radians ) );
	}

	template< typename Type >
	inline Type AngleT< Type >::cosh()const
	{
		return std::cosh( m_radians );
	}

	template< typename Type >
	inline Type AngleT< Type >::sinh()const
	{
		return std::sinh( m_radians );
	}

	template< typename Type >
	inline Type AngleT< Type >::tanh()const
	{
		return std::tanh( m_radians );
	}

	template< typename Type >
	inline AngleT< Type > AngleT< Type >::operator-()const noexcept
	{
		return AngleT{ -m_radians };
	}

	template< typename Type >
	inline AngleT< Type > & AngleT< Type >::operator+=( AngleT< Type > const & rhs )noexcept
	{
		m_radians += rhs.m_radians;
		return *this;
	}

	template< typename Type >
	inline AngleT< Type > & AngleT< Type >::operator-=( AngleT< Type > const & rhs )noexcept
	{
		m_radians -= rhs.m_radians;
		return *this;
	}

	template< typename Type >
	inline AngleT< Type > & AngleT< Type >::operator*=( AngleT< Type > const & rhs )noexcept
	{
		m_radians *= rhs.m_radians;
		return *this;
	}

	template< typename Type >
	inline AngleT< Type > & AngleT< Type >::operator/=( AngleT< Type > const & rhs )noexcept
	{
		m_radians /= rhs.m_radians;
		return *this;
	}

	template< typename Type >
	inline AngleT< Type > & AngleT< Type >::operator*=( double rhs )noexcept
	{
		m_radians *= Type( rhs );
		return *this;
	}

	template< typename Type >
	inline AngleT< Type > & AngleT< Type >::operator/=( double rhs )noexcept
	{
		m_radians /= Type( rhs );
		return *this;
	}

	template< typename Type >
	inline AngleT< Type > acosT( double value )
	{
		return AngleT< Type >::fromRadians( Type( std::acos( value ) ) );
	}

	template< typename Type >
	inline AngleT< Type > asinT( double value )
	{
		return AngleT< Type >::fromRadians( Type( std::asin( value ) ) );
	}

	template< typename Type >
	inline AngleT< Type > atanT( double value )
	{
		return AngleT< Type >::fromRadians( Type( std::atan( value ) ) );
	}

	template< typename Type >
	inline AngleT< Type > atan2T( double y, double x )
	{
		return AngleT< Type >::fromRadians( Type( std::atan2( y, x ) ) );
	}

	inline AngleT< float > acosf( double value )
	{
		return acosT< float >( value );
	}

	inline AngleT< float > asinf( double value )
	{
		return asinT< float >( value );
	}

	inline AngleT< float > atanf( double value )
	{
		return atanT< float >( value );
	}

	inline AngleT< float > atan2f( double y, double x )
	{
		return atan2T< float >( y, x );
	}

	inline AngleT< double > acosd( double value )
	{
		return acosT< double >( value );
	}

	inline AngleT< double > asind( double value )
	{
		return asinT< double >( value );
	}

	inline AngleT< double > atand( double value )
	{
		return atanT< double >( value );
	}

	inline AngleT< double > atan2d( double y, double x )
	{
		return atan2T< double >( y, x );
	}

	template< typename Type >
	AngleT< Type > operator+( AngleT< Type > const & lhs, AngleT< Type > const & rhs )noexcept
	{
		AngleT< Type > result( lhs );
		result += rhs;
		return result;
	}

	template< typename Type >
	AngleT< Type > operator-( AngleT< Type > const & lhs, AngleT< Type > const & rhs )noexcept
	{
		AngleT< Type > result( lhs );
		result -= rhs;
		return result;
	}

	template< typename Type >
	AngleT< Type > operator*( AngleT< Type > const & lhs, AngleT< Type > const & rhs )noexcept
	{
		AngleT< Type > result( lhs );
		result *= rhs;
		return result;
	}

	template< typename Type >
	AngleT< Type > operator/( AngleT< Type > const & lhs, AngleT< Type > const & rhs )noexcept
	{
		AngleT< Type > result( lhs );
		result /= rhs;
		return result;
	}

	template< typename Type >
	AngleT< Type > operator*( AngleT< Type > const & lhs, double rhs )noexcept
	{
		AngleT< Type > result( lhs );
		result *= rhs;
		return result;
	}

	template< typename Type >
	AngleT< Type > operator/( AngleT< Type > const & lhs, double rhs )noexcept
	{
		AngleT< Type > result( lhs );
		result /= rhs;
		return result;
	}
}
