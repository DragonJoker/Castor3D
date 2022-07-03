/*
See LICENSE file in root folder
*/
#ifndef ___C3DAS_AtmosphereWeatherConfig_H___
#define ___C3DAS_AtmosphereWeatherConfig_H___

#include "AtmosphereScatteringPrerequisites.hpp"

#include <CastorUtils/Math/Angle.hpp>
#include <CastorUtils/Math/SquareMatrix.hpp>

namespace atmosphere_scattering
{
	template< template< typename DataT > typename WrapperT >
	struct AtmosphereWeatherConfigT
	{
		template< typename ... ParamsT >
		AtmosphereWeatherConfigT( ParamsT & ... params )
			: cloudSpeed{ params..., 45.0f }
			, coverage{ params..., 0.45f }
			, crispiness{ params..., 40.0f }
			, curliness{ params..., 0.1f }
			, density{ params..., 0.02f }
			, absorption{ params..., 0.35f }
			, sphereInnerRadius{ params..., 50.0f }
			, sphereOuterRadius{ params..., 170.0f }
			, perlinAmplitude{ params..., 0.5f }
			, perlinFrequency{ params..., 0.8f }
			, perlinScale{ params..., 100.0f }
			, perlinOctaves{ params..., 4u }
			, cloudColorTop{ params..., castor::Point3f{ 169.0f, 149.0f, 149.0f } * 1.5f / 255.0f }
			, time{ 0.0f }
			, cloudColorBottom{ params..., castor::Point3f{ 65.0f, 70.0f, 80.0f } * 1.5f / 255.0f }
			, enablePowder{ params..., 0 }
			, seed{ params..., castor::Point3f{} }
			, windDirection{ params..., castor::Point3f{ 0.5f, 0.0f, 0.1f } }
		{
		}

		template< template< typename DataU > typename WrapperU >
		AtmosphereWeatherConfigT & operator=( AtmosphereWeatherConfigT< WrapperU > const & rhs )
		{
			cloudSpeed = rhs.cloudSpeed;
			coverage = rhs.coverage;
			crispiness = rhs.crispiness;
			curliness = rhs.curliness;

			density = rhs.density;
			absorption = rhs.absorption;
			sphereInnerRadius = rhs.sphereInnerRadius;
			sphereOuterRadius = rhs.sphereOuterRadius;

			perlinAmplitude = rhs.perlinAmplitude;
			perlinFrequency = rhs.perlinFrequency;
			perlinScale = rhs.perlinScale;
			perlinOctaves = rhs.perlinOctaves;

			cloudColorTop = rhs.cloudColorTop;
			time = rhs.time;

			cloudColorBottom = rhs.cloudColorBottom;
			enablePowder = rhs.enablePowder;

			seed = rhs.seed;

			windDirection = rhs.windDirection;

			return *this;
		}

		WrapperT< float > cloudSpeed;
		WrapperT< float > coverage;
		WrapperT< float > crispiness;
		WrapperT< float > curliness;

		WrapperT< float > density;
		WrapperT< float > absorption;
		WrapperT< float > sphereInnerRadius;
		WrapperT< float > sphereOuterRadius;

		WrapperT< float > perlinAmplitude;
		WrapperT< float > perlinFrequency;
		WrapperT< float > perlinScale;
		WrapperT< uint32_t > perlinOctaves;

		WrapperT< castor::Point3f > cloudColorTop;
		float time{};

		WrapperT< castor::Point3f > cloudColorBottom;
		WrapperT< int > enablePowder;

		WrapperT< castor::Point3f > seed;
		float pad0{};

		WrapperT< castor::Point3f > windDirection;
		float pad1{};
	};
}

#endif
