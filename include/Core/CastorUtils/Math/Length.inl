#include <cmath>

namespace castor
{
	//*********************************************************************************************

	namespace details
	{
		template< LengthUnit FromT >
		struct LengthUnitConvertFactors;

		template<>
		struct LengthUnitConvertFactors< LengthUnit::eKilometre >
		{
			static constexpr Array< double, size_t( LengthUnit::eCount ) > factors{ double( 1.0 )
				, double( 1000.0 )
				, double( 100000.0 )
				, double( 1000000.0 )
				, double( 1.0 / 0.0009144 )
				, double( 1.0 / 0.0003048 )
				, double( 1.0 / 0.0000254 ) };
		};

		template<>
		struct LengthUnitConvertFactors< LengthUnit::eMetre >
		{
			static constexpr Array< double, size_t( LengthUnit::eCount ) > factors{ double( 1.0 / 1000.0 )
				, double( 1.0 )
				, double( 100.0 )
				, double( 1000.0 )
				, double( 1.0 / 0.9144 )
				, double( 1.0 / 0.3048 )
				, double( 1.0 / 0.0254 ) };
		};

		template<>
		struct LengthUnitConvertFactors< LengthUnit::eCentimetre >
		{
			static constexpr Array< double, size_t( LengthUnit::eCount ) > factors{ double( 1.0 / 100000.0 )
				, double( 1.0 / 100.0 )
				, double( 1.0 )
				, double( 10.0 )
				, double( 1.0 / 91.44 )
				, double( 1.0 / 30.48 )
				, double( 1.0 / 2.54 ) };
		};

		template<>
		struct LengthUnitConvertFactors< LengthUnit::eMillimetre >
		{
			static constexpr Array< double, size_t( LengthUnit::eCount ) > factors{ double( 1.0 / 1000000.0 )
				, double( 1.0 / 1000.0 )
				, double( 1.0 / 10.0 )
				, double( 1.0 )
				, double( 1.0 / 914.4 )
				, double( 1.0 / 304.8 )
				, double( 1.0 / 25.4 ) };
		};

		template<>
		struct LengthUnitConvertFactors< LengthUnit::eYard >
		{
			static constexpr Array< double, size_t( LengthUnit::eCount ) > factors{ double( 0.0009144 )
				, double( 0.9144 )
				, double( 91.44 )
				, double( 914.4 )
				, double( 1.0 )
				, double( 3.0 )
				, double( 36.0 ) };
		};

		template<>
		struct LengthUnitConvertFactors< LengthUnit::eFoot >
		{
			static constexpr Array< double, size_t( LengthUnit::eCount ) > factors{ double( 0.0003048 )
				, double( 0.3048 )
				, double( 30.48 )
				, double( 304.8 )
				, double( 1.0 / 3.0 )
				, double( 1.0 )
				, double( 12.0 ) };
		};

		template<>
		struct LengthUnitConvertFactors< LengthUnit::eInch >
		{
			static constexpr Array< double, size_t( LengthUnit::eCount ) > factors{ double( 0.0000254 )
				, double( 0.0254 )
				, double( 2.54 )
				, double( 25.4 )
				, double( 1.0 / 36.0 )
				, double( 1.0 / 12.0 )
				, double( 1.0 ) };
		};

		template< LengthUnit FromT, typename TypeT >
		static inline TypeT convertTo( TypeT const & value
			, LengthUnit to )
		{
			using ConvertFactors = LengthUnitConvertFactors< FromT >;

			switch ( to )
			{
			case LengthUnit::eKilometre:
			case LengthUnit::eMetre:
			case LengthUnit::eCentimetre:
			case LengthUnit::eMillimetre:
			case LengthUnit::eYard:
			case LengthUnit::eFoot:
			case LengthUnit::eInch:
				return TypeT( pointCast< double >( value ) * ConvertFactors::factors[size_t( to )] );
			default:
				CU_Failure( "Unsupported length unit for conversion" );
				return value;
			}
		}
	}

	//*********************************************************************************************

	template< typename TypeT >
	inline LengthT< TypeT >::LengthT( LengthUnit unit )noexcept
		: LengthT< TypeT >{ TypeT{}, unit }
	{
	}

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
	inline LengthT< TypeT > & LengthT< TypeT >::operator-()noexcept
	{
		m_value = -m_value;
		return *this;
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

	//*********************************************************************************************

	template< typename TypeT >
	static inline TypeT convert( TypeT const & value
		, LengthUnit from
		, LengthUnit to )
	{
		switch ( from )
		{
		case LengthUnit::eKilometre:
			return details::convertTo< LengthUnit::eKilometre >( value, to );
		case LengthUnit::eMetre:
			return details::convertTo< LengthUnit::eMetre >( value, to );
		case LengthUnit::eCentimetre:
			return details::convertTo< LengthUnit::eCentimetre >( value, to );
		case LengthUnit::eMillimetre:
			return details::convertTo< LengthUnit::eMillimetre >( value, to );
		case LengthUnit::eYard:
			return details::convertTo< LengthUnit::eYard >( value, to );
		case LengthUnit::eFoot:
			return details::convertTo< LengthUnit::eFoot >( value, to );
		case LengthUnit::eInch:
			return details::convertTo< LengthUnit::eInch >( value, to );
		default:
			CU_Failure( "Unsupported length unit for conversion" );
			return value;
		}
	}

	//*********************************************************************************************

	template< typename TypeT >
	bool operator==( LengthT< TypeT > const & lhs, LengthT< TypeT > const & rhs )noexcept
	{
		return std::abs( lhs.m_value - convert( rhs.m_value, rhs.m_unit, lhs.m_unit ) ) < std::numeric_limits< TypeT >::epsilon();
	}

	template< typename TypeT >
	bool operator!=( LengthT< TypeT > const & lhs, LengthT< TypeT > const & rhs )noexcept
	{
		return !( lhs == rhs );
	}

	template< typename TypeT >
	inline bool operator<( LengthT< TypeT > const & lhs, LengthT< TypeT > const & rhs )noexcept
	{
		return lhs.m_value < convert( rhs.m_value, rhs.m_unit, lhs.m_unit );
	}

	template< typename TypeT >
	inline bool operator>=( LengthT< TypeT > const & lhs, LengthT< TypeT > const & rhs )noexcept
	{
		return !( lhs < rhs );
	}

	template< typename TypeT >
	inline bool operator>( LengthT< TypeT > const & lhs, LengthT< TypeT > const & rhs )noexcept
	{
		return lhs.m_value > convert( rhs.m_value, rhs.m_unit, lhs.m_unit );
	}

	template< typename TypeT >
	inline bool operator<=( LengthT< TypeT > const & lhs, LengthT< TypeT > const & rhs )noexcept
	{
		return !( lhs > rhs );
	}

	//*********************************************************************************************

	template< typename TypeT >
	LengthT< TypeT > operator+( LengthT< TypeT > const & lhs, LengthT< TypeT > const & rhs )noexcept
	{
		LengthT< TypeT > result{ lhs };
		result += rhs;
		return result;
	}

	template< typename TypeT >
	LengthT< TypeT > operator-( LengthT< TypeT > const & lhs, LengthT< TypeT > const & rhs )noexcept
	{
		LengthT< TypeT > result{ lhs };
		result -= rhs;
		return result;
	}

	template< typename TypeT >
	LengthT< TypeT > operator+( TypeT const & lhs, LengthT< TypeT > const & rhs )noexcept
	{
		return LengthT< TypeT >::fromUnit( lhs, rhs.lengthUnit() ) + rhs;
	}

	template< typename TypeT >
	LengthT< TypeT > operator-( TypeT const & lhs, LengthT< TypeT > const & rhs )noexcept
	{
		return LengthT< TypeT >::fromUnit( lhs, rhs.lengthUnit() ) - rhs;
	}

	template< typename TypeT >
	LengthT< TypeT > operator+( LengthT< TypeT > const & lhs, TypeT const & rhs )noexcept
	{
		LengthT< TypeT > result{ lhs };
		result += rhs;
		return result;
	}

	template< typename TypeT >
	LengthT< TypeT > operator-( LengthT< TypeT > const & lhs, TypeT const & rhs )noexcept
	{
		LengthT< TypeT > result{ lhs };
		result -= rhs;
		return result;
	}

	template< typename TypeT >
	LengthT< TypeT > operator*( LengthT< TypeT > const & lhs, double rhs )noexcept
	{
		LengthT< TypeT > result{ lhs };
		result *= rhs;
		return result;
	}

	template< typename TypeT >
	LengthT< TypeT > operator/( LengthT< TypeT > const & lhs, double rhs )noexcept
	{
		LengthT< TypeT > result{ lhs };
		result /= rhs;
		return result;
	}

	//*********************************************************************************************
}
