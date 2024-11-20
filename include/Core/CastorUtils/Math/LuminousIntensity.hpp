/*
See LICENSE file in root folder
*/
#ifndef ___CU_LuminousIntensity_H___
#define ___CU_LuminousIntensity_H___

#include "CastorUtils/Math/Length.hpp"

namespace castor
{
	class LuminousIntensity
	{
	public:
		constexpr LuminousIntensity()noexcept = default;

		constexpr explicit LuminousIntensity( float candela )noexcept
			: m_candela{ candela }
		{
		}

		explicit constexpr LuminousIntensity( double candela )noexcept
			: LuminousIntensity{ float( candela ) }
		{
		}

		template< typename T >
		LuminousIntensity( Illumination const & illumination, LengthT< T > d )noexcept
			: LuminousIntensity{ illumination, d.metres() }
		{
		}

		constexpr float candela()const noexcept
		{
			return m_candela;
		}

	private:
		CU_API LuminousIntensity( Illumination const & illumination, float metres )noexcept;

		float m_candela{};

		friend bool operator==( LuminousIntensity const & lhs, LuminousIntensity const & rhs )noexcept;
	};
	/**
	 *\~english
	 *\name Comparison operators.
	 *\~french
	 *\name Opérateurs de comparaison.
	 **/
	/**@{*/
	inline bool operator==( LuminousIntensity const & lhs, LuminousIntensity const & rhs )noexcept
	{
		return std::abs( lhs.candela() - rhs.candela() ) < std::numeric_limits< float >::epsilon();
	}

	inline auto operator<=>( LuminousIntensity const & lhs, LuminousIntensity const & rhs )noexcept
	{
		return lhs.candela() <=> rhs.candela();
	}
	/**@}*/
}

#endif
