#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LpvConfig.hpp"

#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"

namespace castor3d
{
	void LpvConfig::accept( castor::String const & name
		, ConfigurationVisitorBase & visitor )
	{
		visitor.visit( name
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, cuT( "LPV" )
			, cuT( "Indirect Attenuation" )
			, indirectAttenuation );
		visitor.visit( name
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, cuT( "LPV" )
			, cuT( "Texel Area Modifier" )
			, texelAreaModifier );
	}
}
