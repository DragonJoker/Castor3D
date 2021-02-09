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
		log::info << m_tabs << cuT( "Writing RsmConfig" ) << std::endl;
		return beginBlock( cuT( "lpv_config" ), file )
			&& write( cuT( "intensity" ), object.intensity, file )
			&& write( cuT( "max_radius" ), object.maxRadius, file )
			&& write( cuT( "sample_count" ), object.sampleCount, file )
			&& endBlock( file );
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
