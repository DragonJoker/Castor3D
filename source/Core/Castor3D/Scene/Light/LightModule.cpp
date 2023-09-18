#include "Castor3D/Scene/Light/LightModule.hpp"

#include "Castor3D/Cache/ObjectCache.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Scene/Light/Light.hpp"

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

	void RsmConfig::accept( castor::String const & name
		, ConfigurationVisitorBase & visitor )
	{
		visitor.visit( name
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, cuT( "RSM" )
			, cuT( "Intensity" )
			, intensity );
		visitor.visit( name
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, cuT( "RSM" )
			, cuT( "Max Radius" )
			, maxRadius );
		visitor.visit( name
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, cuT( "RSM" )
			, cuT( "Sample Count" )
			, sampleCount );
	}

	float getMaxDistance( LightCategory const & light
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
			float maxChannel = std::max( std::max( light.getColour()[0]
				, light.getColour()[1] )
				, light.getColour()[2] );
			result = 256.0f * maxChannel * light.getDiffuseIntensity();

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
				result = 4000.0f;
			}
		}
		else
		{
			log::error << cuT( "Light's attenuation is set to (0.0, 0.0, 0.0), which results in infinite litten distance, not representable." ) << std::endl;
		}

		return result;
	}

	float getMaxDistance( LightCategory const & light
		, castor::Point3f const & attenuation
		, float max )
	{
		return std::min( max, getMaxDistance( light, attenuation ) );
	}

	float computeRange( castor::Point2f const & intensity
		, float range )
	{
		return range <= 0.0f
			? sqrt( std::max( intensity->x, intensity->y ) ) / 0.00001f
			: range;
	}
}
