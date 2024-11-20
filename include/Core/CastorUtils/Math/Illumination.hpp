/*
See LICENSE file in root folder
*/
#ifndef ___CU_Illumination_H___
#define ___CU_Illumination_H___

#include "CastorUtils/Math/Length.hpp"

namespace castor
{
	class Illumination
	{
	public:
		constexpr Illumination()noexcept = default;

		constexpr explicit Illumination( float lux )noexcept
			: m_lux{ lux }
		{
		}

		explicit constexpr Illumination( double lux )noexcept
			: Illumination{ float( lux ) }
		{
		}

		template< typename T >
		Illumination( LuminousIntensity const & intensity, LengthT< T > d )noexcept
			: Illumination{ intensity, d.metres() }
		{
		}

		constexpr float lux()const noexcept
		{
			return m_lux;
		}

	private:
		CU_API Illumination( LuminousIntensity const & intensity, float metres )noexcept;

		float m_lux{};

		friend bool operator==( Illumination const & lhs, Illumination const & rhs )noexcept;
	};
	/**
	 *\~english
	 *\name Comparison operators.
	 *\~french
	 *\name Opérateurs de comparaison.
	 **/
	/**@{*/
	inline bool operator==( Illumination const & lhs, Illumination const & rhs )noexcept
	{
		return std::abs( lhs.lux() - rhs.lux() ) < std::numeric_limits< float >::epsilon();
	}

	inline auto operator<=>( Illumination const & lhs, Illumination const & rhs )noexcept
	{
		return lhs.lux() <=> rhs.lux();
	}
	/**@}*/
}

#endif
