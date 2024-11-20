#include "Castor3D/Scene/Light/LightModule.hpp"

#include "Castor3D/Cache/ObjectCache.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Scene/Light/Light.hpp"

#include <CastorUtils/Math/LuminousIntensity.hpp>

namespace castor3d
{
	castor::String getName( LightType value )
	{
		switch ( value )
		{
		case LightType::eDirectional:
			return cuT( "directional" );
		case LightType::ePoint:
			return cuT( "point" );
		case LightType::eSpot:
			return cuT( "spot" );
		default:
			CU_Failure( "Unsupported LightType" );
			return castor::cuEmptyString;
		}
	}

	float getMaxDistance( castor::Point3f const & colour
		, castor::LuminousIntensity const & intensity
		, castor::Point3f const & attenuation )
	{
		constexpr float threshold = 0.000001f;
		auto constant = std::abs( attenuation[0] );
		auto linear = std::abs( attenuation[1] );
		auto quadratic = std::abs( attenuation[2] );
		float result = std::numeric_limits< float >::max();

		if ( constant >= threshold
			|| linear >= threshold
			|| quadratic >= threshold )
		{
			float maxChannel = std::max( std::max( colour[0]
				, colour[1] )
				, colour[2] );
			result = 256.0f * maxChannel * intensity.candela();

			if ( quadratic >= threshold )
			{
				if ( linear < threshold )
				{
					CU_Require( result >= constant );
					result = sqrtf( ( result - constant ) / quadratic );
				}
				else
				{
					auto delta = linear * linear - 4 * quadratic * ( constant - result );
					CU_Require( delta >= 0 );
					result = ( -linear + sqrtf( delta ) ) / ( 2 * quadratic );
				}
			}
			else if ( linear >= threshold )
			{
				result = ( result - constant ) / linear;
			}
			else
			{
				result = 0.0f;
			}
		}
		else
		{
			result = 0.0f;
		}

		return result;
	}

	float computeRange( castor::LuminousIntensity const & intensity
		, float range )
	{
		return range <= 0.0f
			? float( sqrt( intensity.candela() ) / 0.00001f )
			: range;
	}
}
