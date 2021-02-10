#include "Castor3D/Render/Ssao/SsaoConfig.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"

using namespace castor;

namespace castor3d
{
	void SsaoConfig::accept( castor::String const & name
		, PipelineVisitorBase & visitor )
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
}
