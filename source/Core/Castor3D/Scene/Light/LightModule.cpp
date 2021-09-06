#include "Castor3D/Scene/Light/LightModule.hpp"

#include "Castor3D/Cache/ObjectCache.hpp"
#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"
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
		, PipelineVisitorBase & visitor )
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
}
