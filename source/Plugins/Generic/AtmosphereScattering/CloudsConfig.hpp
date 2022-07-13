/*
See LICENSE file in root folder
*/
#ifndef ___C3DAS_CloudsConfig_H___
#define ___C3DAS_CloudsConfig_H___

#include "AtmosphereScatteringPrerequisites.hpp"

#include <CastorUtils/Math/Angle.hpp>
#include <CastorUtils/Math/SquareMatrix.hpp>

namespace atmosphere_scattering
{
	template< template< typename DataT > typename WrapperT >
	struct CloudsConfigT
	{
		template< typename ... ParamsT >
		explicit CloudsConfigT( ParamsT & ... params )
			: speed{ params..., 10.0f }
			, coverage{ params..., 0.45f }
			, crispiness{ params..., 40.0f }
			, curliness{ params..., 0.1f }
			, density{ params..., 0.2f }
			, absorption{ params..., 0.25f }
			, innerRadius{ params..., 50.0f }
			, outerRadius{ params..., 170.0f }
			, colorTop{ params..., castor::Point3f{ 169.0f, 149.0f, 149.0f } * 1.5f / 255.0f }
			, time{ 0.0f }
			, colorBottom{ params..., castor::Point3f{ 65.0f, 70.0f, 80.0f } * 1.5f / 255.0f }
			, enablePowder{ params..., 1 }
			, windDirection{ params..., castor::Point3f{ 0.5f, 0.0f, 0.1f } }
			, topOffset{ params..., 0.75f }
		{
		}

		template< template< typename DataU > typename WrapperU >
		CloudsConfigT & operator=( CloudsConfigT< WrapperU > const & rhs )
		{
			speed = rhs.speed;
			coverage = rhs.coverage;
			crispiness = rhs.crispiness;
			curliness = rhs.curliness;

			density = rhs.density;
			absorption = rhs.absorption;
			innerRadius = rhs.innerRadius;
			outerRadius = rhs.outerRadius;

			colorTop = rhs.colorTop;
			time = rhs.time;

			colorBottom = rhs.colorBottom;
			enablePowder = rhs.enablePowder;

			windDirection = rhs.windDirection;
			topOffset = rhs.topOffset;

			return *this;
		}

		WrapperT< float > speed;
		WrapperT< float > coverage;
		WrapperT< float > crispiness;
		WrapperT< float > curliness;

		WrapperT< float > density;
		WrapperT< float > absorption;
		WrapperT< float > innerRadius;
		WrapperT< float > outerRadius;

		WrapperT< castor::Point3f > colorTop;
		float time{};

		WrapperT< castor::Point3f > colorBottom;
		WrapperT< int > enablePowder;

		WrapperT< castor::Point3f > windDirection;
		WrapperT< float > topOffset;
	};
}

#endif
