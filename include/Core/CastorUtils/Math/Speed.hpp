/*
See LICENSE file in root folder
*/
#ifndef ___CU_Speed_HPP___
#define ___CU_Speed_HPP___

#include "Angle.hpp"
#include "Point.hpp"

namespace castor
{
	template< typename ValueT >
	struct SpeedTraitsT
	{
		static ValueT convert( double d )
		{
			return ValueT( d );
		}
	};

	template< typename ValueT >
	struct SpeedTraitsT< Point< ValueT, 2 > >
	{
		static Point< ValueT, 2 > convert( double d )
		{
			return Point< ValueT, 2 >{ d, d };
		}
	};

	template< typename ValueT >
	struct SpeedTraitsT< Point< ValueT, 3 > >
	{
		static Point< ValueT, 3 > convert( double d )
		{
			return Point< ValueT, 3 >{ d, d, d };
		}
	};

	template< typename ValueT >
	struct SpeedTraitsT< Point< ValueT, 4 > >
	{
		static Point< ValueT, 4 > convert( double d )
		{
			return Point< ValueT, 4 >{ d, d, d, d };
		}
	};

	template< typename ValueT >
	struct SpeedTraitsT< AngleT< ValueT > >
	{
		static AngleT< ValueT > convert( double d )
		{
			return AngleT< ValueT >::fromRadians( d );
		}
	};

	template< typename ValueT, typename DurationT, typename TraitsT >
	class SpeedT
	{
		template< typename ValueU, typename DurationU, typename TraitsU >
		friend class SpeedT;

	public:
		using duration_type = DurationT;
		using value_type = ValueT;

	public:
		SpeedT()
			: m_value{}
		{
		}

		explicit SpeedT( ValueT const & value )
			: m_value{ value }
		{
		}

		SpeedT( SpeedT< ValueT, DurationT > const & rhs )
			: m_value{ rhs.m_value }
		{
		}

		SpeedT & operator=( SpeedT< ValueT, DurationT > const & rhs )
		{
			m_value = rhs.m_value;
			return *this;
		}

		template< typename ValueU, typename DurationU >
		explicit SpeedT( SpeedT < ValueU, DurationU > const & rhs )
			: m_value{ rhs.getDistance( Unit ) }
		{
		}

		template< typename ValueU, typename DurationU >
		SpeedT & operator=( SpeedT< ValueU, DurationU > const & rhs )
		{
			m_value = rhs.getDistance( Unit );
			return *this;
		}

		template< typename DurationU >
		ValueT getDistance( DurationU const & duration )const
		{
			return m_value * TraitsT::convert( getDurationRatioTo< DurationU >() * double( duration.count() ) );
		}

		ValueT const & getValue()const
		{
			return m_value;
		}

		template< typename DurationU >
		static double getDurationRatioFrom()
		{
			static auto constexpr uZero = DurationU{ 0u };
			static auto constexpr tZero = Zero;
			static auto constexpr uUnit = std::chrono::duration_cast< DurationU >( Unit );
			static auto constexpr tUnit = std::chrono::duration_cast< DurationT >( DurationU{ 1u } );

			if constexpr ( tUnit == tZero )
			{
				// Given duration is more precise than my duration.
				return double( uUnit.count() );
			}
			else if constexpr ( uUnit == uZero )
			{
				// Given duration is less precise than my duration.
				return 1.0 / tUnit.count();
			}
			else
			{
				// Same precision.
				return 1.0;
			}
		}

		template< typename DurationU >
		static double getDurationRatioTo()
		{
			static auto constexpr uZero = DurationU{ 0u };
			static auto constexpr tZero = Zero;
			static auto constexpr uUnit = std::chrono::duration_cast< DurationU >( Unit );
			static auto constexpr tUnit = std::chrono::duration_cast< DurationT >( DurationU{ 1u } );

			if constexpr ( tUnit == tZero )
			{
				// Given duration is more precise than my duration.
				return 1.0 / double( uUnit.count() );
			}
			else if constexpr ( uUnit == uZero )
			{
				// Given duration is less precise than my duration.
				return double( tUnit.count() );
			}
			else
			{
				// Same precision.
				return 1.0;
			}
		}

	public:
		static DurationT constexpr Zero{ 0u };
		static DurationT constexpr Unit{ 1u };

	private:
		ValueT m_value;

		template< typename Value, typename Duration, typename Traits >
		friend bool operator==( SpeedT< Value, Duration, Traits > const &, SpeedT< Value, Duration, Traits > const & );
		template< typename Value, typename Duration, typename Traits >
		friend bool operator<( SpeedT< Value, Duration > const &, SpeedT< Value, Duration, Traits > const & );
		template< typename Value, typename Duration, typename Traits >
		friend bool operator>( SpeedT< Value, Duration, Traits > const &, SpeedT< Value, Duration, Traits > const & );
	};
	/**
	*\~english
	*\name	Comparison.
	*\~english
	*\name	Comparaison.
	**/
	/**@{*/
	template< typename ValueT, typename DurationT, typename TraitsT >
	bool operator==( SpeedT< ValueT, DurationT, TraitsT > const & lhs
		, SpeedT< ValueT, DurationT, TraitsT > const & rhs )
	{
		return lhs.m_value == rhs.m_value;
	}

	template< typename ValueT, typename DurationT, typename TraitsT >
	bool operator!=( SpeedT< ValueT, DurationT, TraitsT > const & lhs
		, SpeedT< ValueT, DurationT, TraitsT > const & rhs )
	{
		return !( lhs == rhs.m_value );
	}

	template< typename ValueT, typename DurationT, typename TraitsT >
	bool operator<( SpeedT< ValueT, DurationT, TraitsT > const & lhs
		, SpeedT< ValueT, DurationT, TraitsT > const & rhs )
	{
		return lhs.m_value < rhs.m_value;
	}

	template< typename ValueT, typename DurationT, typename TraitsT >
	bool operator>=( SpeedT< ValueT, DurationT, TraitsT > const & lhs
		, SpeedT< ValueT, DurationT, TraitsT > const & rhs )
	{
		return !( lhs < rhs.m_value );
	}

	template< typename ValueT, typename DurationT, typename TraitsT >
	bool operator>( SpeedT< ValueT, DurationT, TraitsT > const & lhs
		, SpeedT< ValueT, DurationT, TraitsT > const & rhs )
	{
		return lhs.m_value < rhs.m_value;
	}

	template< typename ValueT, typename DurationT, typename TraitsT >
	bool operator<=( SpeedT< ValueT, DurationT, TraitsT > const & lhs
		, SpeedT< ValueT, DurationT, TraitsT > const & rhs )
	{
		return !( lhs > rhs.m_value );
	}
	/**@}*/

	template< typename DurationT, typename ValueT >
	SpeedT< ValueT, DurationT > makeSpeed( ValueT const & value )
	{
		return SpeedT< ValueT, DurationT >{ value };
	}

	template< typename DurationT, typename ValueT >
	SpeedT< ValueT, DurationT > makeSpeed( ValueT const & value 
		, DurationT const & unit )
	{
		return makeSpeed< DurationT >( value );
	}

	template< typename ValueT >
	struct IsSpeedT : std::false_type{};

	template< typename ValueT >
	struct IsSpeedT< SpeedT< ValueT, castor::Seconds > > : std::true_type{};

	template< typename ValueT >
	struct IsSpeedT< SpeedT< ValueT, castor::Milliseconds > > : std::true_type{};

	template< typename ValueT >
	struct IsSpeedT< SpeedT< ValueT, castor::Microseconds > > : std::true_type{};

	template< typename ValueT >
	struct IsSpeedT< SpeedT< ValueT, castor::Nanoseconds > > : std::true_type{};

	template< typename ValueT >
	static bool constexpr isSpeedT = IsSpeedT< ValueT >::value;
}

#endif
