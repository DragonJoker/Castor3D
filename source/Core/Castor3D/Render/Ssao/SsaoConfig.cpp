#include "Castor3D/Render/Ssao/SsaoConfig.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"

namespace castor3d
{
	void SsaoConfig::accept( castor::String const & name
		, ConfigurationVisitorBase & visitor )
	{
		visitor.visit( name
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, cuT( "SSAO" )
			, cuT( "Radius" )
			, radius );
		visitor.visit( name
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, cuT( "SSAO" )
			, cuT( "Bias" )
			, bias );
		visitor.visit( name
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, cuT( "SSAO" )
			, cuT( "Intensity" )
			, intensity );
		visitor.visit( name
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, cuT( "SSAO" )
			, cuT( "Samples" )
			, numSamples );
		visitor.visit( name
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, cuT( "SSAO" )
			, cuT( "Edge Sharpness" )
			, edgeSharpness );
		visitor.visit( name
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, cuT( "SSAO" )
			, cuT( "Blur Step Size" )
			, blurStepSize );
		visitor.visit( name
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, cuT( "SSAO" )
			, cuT( "Blur Radius" )
			, blurRadius );
		visitor.visit( name
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, cuT( "SSAO" )
			, cuT( "Bend Step Count" )
			, bendStepCount );
		visitor.visit( name
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, cuT( "SSAO" )
			, cuT( "Bend Step Size" )
			, bendStepSize );
	}

	void SsaoConfig::accept( ConfigurationVisitorBase & visitor )
	{
		visitor.visit( cuT( "SSAO" ) );
		visitor.visit( cuT( "Enable SSAO" ), enabled );
		visitor.visit( cuT( "High Quality" ), highQuality );
		visitor.visit( cuT( "Normals Buffer" ), useNormalsBuffer );
		visitor.visit( cuT( "Radius" ), radius );
		visitor.visit( cuT( "Bias" ), bias );
		visitor.visit( cuT( "Intensity" ), intensity );
		visitor.visit( cuT( "Samples" ), numSamples );
		visitor.visit( cuT( "Edge Sharpness" ), edgeSharpness );
		visitor.visit( cuT( "Blur High Quality" ), blurHighQuality );
		visitor.visit( cuT( "Blur Step Size" ), blurStepSize );
		visitor.visit( cuT( "Blur Radius" ), blurRadius );
		visitor.visit( cuT( "Bend Step Count" ), bendStepCount );
		visitor.visit( cuT( "Bend Step Size" ), bendStepSize );
	}
}
