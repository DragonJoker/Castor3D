#include "Castor3D/Render/Ssao/SsaoConfig.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"

using namespace castor;

namespace castor3d
{
	SsaoConfig::TextWriter::TextWriter( String const & tabs )
		: castor::TextWriter< SsaoConfig >{ tabs }
	{
	}

	bool SsaoConfig::TextWriter::operator()( SsaoConfig const & object, TextFile & file )
	{
		bool result{ true };

		if ( object.enabled )
		{
			log::info << m_tabs << cuT( "Writing SsaoConfig" ) << std::endl;
			result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "ssao\n" ) + m_tabs + cuT( "{\n" ) ) > 0;

			if ( result )
			{
				result = file.writeText( m_tabs + cuT( "\tenabled " ) + ( object.enabled ? cuT( "true" ) : cuT( "false" ) ) + cuT( "\n" ) ) > 0;
				castor::TextWriter< SsaoConfig >::checkError( result, "SsaoConfig enabled" );
			}

			if ( result )
			{
				result = file.writeText( m_tabs + cuT( "\thigh_quality " ) + ( object.highQuality ? cuT( "true" ) : cuT( "false" ) ) + cuT( "\n" ) ) > 0;
				castor::TextWriter< SsaoConfig >::checkError( result, "SsaoConfig high quality" );
			}

			if ( result )
			{
				result = file.writeText( m_tabs + cuT( "\tuse_normals_buffer " ) + ( object.useNormalsBuffer ? cuT( "true" ) : cuT( "false" ) ) + cuT( "\n" ) ) > 0;
				castor::TextWriter< SsaoConfig >::checkError( result, "SsaoConfig use normals buffer" );
			}

			if ( result )
			{
				result = file.writeText( m_tabs + cuT( "\tblur_high_quality " ) + ( object.blurHighQuality ? cuT( "true" ) : cuT( "false" ) ) + cuT( "\n" ) ) > 0;
				castor::TextWriter< SsaoConfig >::checkError( result, "SsaoConfig blur high quality" );
			}

			if ( result )
			{
				result = file.writeText( m_tabs + cuT( "\tintensity " ) + string::toString( object.intensity, std::locale{ "C" } ) + cuT( "\n" ) ) > 0;
				castor::TextWriter< SsaoConfig >::checkError( result, "SsaoConfig intensity" );
			}

			if ( result )
			{
				result = file.writeText( m_tabs + cuT( "\tradius " ) + string::toString( object.radius, std::locale{ "C" } ) + cuT( "\n" ) ) > 0;
				castor::TextWriter< SsaoConfig >::checkError( result, "SsaoConfig radius" );
			}

			if ( result )
			{
				result = file.writeText( m_tabs + cuT( "\tbias " ) + string::toString( object.bias, std::locale{ "C" } ) + cuT( "\n" ) ) > 0;
				castor::TextWriter< SsaoConfig >::checkError( result, "SsaoConfig bias" );
			}

			if ( result )
			{
				result = file.writeText( m_tabs + cuT( "\tnum_samples " ) + string::toString( object.numSamples, std::locale{ "C" } ) + cuT( "\n" ) ) > 0;
				castor::TextWriter< SsaoConfig >::checkError( result, "SsaoConfig num samples" );
			}

			if ( result )
			{
				result = file.writeText( m_tabs + cuT( "\tedge_sharpness " ) + string::toString( object.edgeSharpness, std::locale{ "C" } ) + cuT( "\n" ) ) > 0;
				castor::TextWriter< SsaoConfig >::checkError( result, "SsaoConfig edge sharpness" );
			}

			if ( result )
			{
				result = file.writeText( m_tabs + cuT( "\tblur_step_size " ) + string::toString( object.blurStepSize.value().value(), std::locale{ "C" } ) + cuT( "\n" ) ) > 0;
				castor::TextWriter< SsaoConfig >::checkError( result, "SsaoConfig blur step size" );
			}

			if ( result )
			{
				result = file.writeText( m_tabs + cuT( "\tblur_radius " ) + string::toString( object.blurRadius.value().value(), std::locale{ "C" } ) + cuT( "\n" ) ) > 0;
				castor::TextWriter< SsaoConfig >::checkError( result, "SsaoConfig blur radius" );
			}

			if ( result )
			{
				result = file.writeText( m_tabs + cuT( "\tbend_step_count " ) + string::toString( object.bendStepCount.value(), std::locale{ "C" } ) + cuT( "\n" ) ) > 0;
				castor::TextWriter< SsaoConfig >::checkError( result, "SsaoConfig bend steps count" );
			}

			if ( result )
			{
				result = file.writeText( m_tabs + cuT( "\tbend_step_size " ) + string::toString( object.bendStepSize, std::locale{ "C" } ) + cuT( "\n" ) ) > 0;
				castor::TextWriter< SsaoConfig >::checkError( result, "SsaoConfig bend step size" );
			}

			file.writeText( m_tabs + cuT( "}\n" ) );
		}

		return result;
	}

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
