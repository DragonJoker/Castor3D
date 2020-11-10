#include "Castor3D/Render/Technique/Opaque/LightVolumeGI/LpvConfig.hpp"

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
		bool result{ true };
		log::info << m_tabs << cuT( "Writing LpvConfig" ) << std::endl;
		result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "lpv_config\n" ) + m_tabs + cuT( "{\n" ) ) > 0;

		if ( result )
		{
			result = file.writeText( m_tabs + cuT( "\tindirect_attenuation" ) + string::toString( object.indirectAttenuation.value() ) ) > 0;
			castor::TextWriter< LpvConfig >::checkError( result, "LpvConfig indirectAttenuation" );
		}

		if ( result )
		{
			result = file.writeText( m_tabs + cuT( "\ttexel_area_modifier" ) + string::toString( object.texelAreaModifier.value() ) ) > 0;
			castor::TextWriter< LpvConfig >::checkError( result, "LpvConfig texelAreaModifier" );
		}

		file.writeText( m_tabs + cuT( "}\n" ) );
		return result;
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
