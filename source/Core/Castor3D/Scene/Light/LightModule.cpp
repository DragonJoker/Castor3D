#include "Castor3D/Scene/Light/LightModule.hpp"

#include <CastorUtils/Math/LuminousIntensity.hpp>

namespace c3d
{
	String getName( LightType value )
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
			return cuEmptyString;
		}
	}

	float getMaxDistance( Point3f const & colour
		, LuminousIntensity const & intensity
		, Point3f const & attenuation )
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

	float computeRange( LuminousIntensity const & intensity
		, float range )
	{
		return range <= 0.0f
			? float( sqrt( intensity.candela() ) / 0.00001f )
			: range;
	}
}
