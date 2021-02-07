#include "Castor3D/Render/Technique/Opaque/ReflectiveShadowMapGI/RsmConfig.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"

using namespace castor;

namespace castor3d
{
	RsmConfig::TextWriter::TextWriter( String const & tabs )
		: castor::TextWriter< RsmConfig >{ tabs }
	{
	}

	bool RsmConfig::TextWriter::operator()( RsmConfig const & object, TextFile & file )
	{
		bool result{ true };
		log::info << m_tabs << cuT( "Writing RsmConfig" ) << std::endl;
		result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "rsm_config\n" ) + m_tabs + cuT( "{\n" ) ) > 0;

		if ( result )
		{
			result = file.writeText( m_tabs + cuT( "\tintensity " ) + string::toString( object.intensity.value() ) + cuT( "\n" ) ) > 0;
			castor::TextWriter< RsmConfig >::checkError( result, "RsmConfig intensity" );
		}

		if ( result )
		{
			result = file.writeText( m_tabs + cuT( "\tmax_radius " ) + string::toString( object.maxRadius.value() ) + cuT( "\n" ) ) > 0;
			castor::TextWriter< RsmConfig >::checkError( result, "RsmConfig maxRadius" );
		}

		if ( result )
		{
			result = file.writeText( m_tabs + cuT( "\tsample_count " ) + string::toString( object.sampleCount.value().value() ) + cuT( "\n" ) ) > 0;
			castor::TextWriter< RsmConfig >::checkError( result, "RsmConfig sampleCount" );
		}

		file.writeText( m_tabs + cuT( "}\n" ) );
		return result;
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
