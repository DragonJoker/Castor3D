#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LpvConfig.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"

using namespace castor;

namespace castor3d
{
	LpvConfig::TextWriter::TextWriter( String const & tabs )
		: castor::TextWriter< LpvConfig >{ tabs }
	{
	}

	bool LpvConfig::TextWriter::operator()( LpvConfig const & object, TextFile & file )
	{
		log::info << m_tabs << cuT( "Writing LpvConfig" ) << std::endl;
		return beginBlock( cuT( "lpv_config" ), file )
			&& write( cuT( "texel_area_modifier" ), object.texelAreaModifier, file )
			&& endBlock( file );
	}

	void LpvConfig::accept( castor::String const & name
		, PipelineVisitorBase & visitor )
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
