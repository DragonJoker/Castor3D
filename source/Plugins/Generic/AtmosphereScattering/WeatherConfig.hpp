/*
See LICENSE file in root folder
*/
#ifndef ___C3DAS_WeatherConfig_H___
#define ___C3DAS_WeatherConfig_H___

#include "AtmosphereScatteringPrerequisites.hpp"

namespace atmosphere_scattering
{
	template< template< typename DataT > typename WrapperT >
	struct WeatherConfigT
	{
		template< typename ... ParamsT >
		WeatherConfigT( ParamsT & ... params )
			: perlinAmplitude{ params..., 0.5f }
			, perlinFrequency{ params..., 0.8f }
			, perlinScale{ params..., 100.0f }
			, perlinOctaves{ params..., 4u }
		{
		}

		template< template< typename DataU > typename WrapperU >
		WeatherConfigT & operator=( WeatherConfigT< WrapperU > const & rhs )
		{
			perlinAmplitude = rhs.perlinAmplitude;
			perlinFrequency = rhs.perlinFrequency;
			perlinScale = rhs.perlinScale;
			perlinOctaves = rhs.perlinOctaves;

			return *this;
		}

		WrapperT< float > perlinAmplitude;
		WrapperT< float > perlinFrequency;
		WrapperT< float > perlinScale;
		WrapperT< uint32_t > perlinOctaves;
	};
}

#endif
