#include "SsaoConfig.hpp"

using namespace castor;

namespace castor3d
{
	SsaoConfig::TextWriter::TextWriter( String const & p_tabs )
		: castor::TextWriter< SsaoConfig >{ p_tabs }
	{
	}

	bool SsaoConfig::TextWriter::operator()( SsaoConfig const & p_object, TextFile & p_file )
	{
		bool result{ true };

		if ( p_object.m_enabled )
		{
			Logger::logInfo( m_tabs + cuT( "Writing SsaoConfig" ) );
			result = p_file.writeText( cuT( "\n" ) + m_tabs + cuT( "ssao\n" ) + m_tabs + cuT( "{\n" ) ) > 0;

			if ( result )
			{
				result = p_file.writeText( m_tabs + cuT( "\tenabled " ) + ( p_object.m_enabled ? cuT( "true" ) : cuT( "false" ) ) + cuT( "\n" ) ) > 0;
				castor::TextWriter< SsaoConfig >::checkError( result, "SsaoConfig enabled" );
			}

			if ( result )
			{
				result = p_file.writeText( m_tabs + cuT( "\thigh_quality " ) + ( p_object.m_highQuality ? cuT( "true" ) : cuT( "false" ) ) + cuT( "\n" ) ) > 0;
				castor::TextWriter< SsaoConfig >::checkError( result, "SsaoConfig high quality" );
			}

			//if ( result )
			//{
			//	result = p_file.writeText( m_tabs + cuT( "\tuse_normals_buffer " ) + ( p_object.m_useNormalsBuffer ? cuT( "true" ) : cuT( "false" ) ) + cuT( "\n" ) ) > 0;
			//	castor::TextWriter< SsaoConfig >::checkError( result, "SsaoConfig use normals buffer" );
			//}

			if ( result )
			{
				result = p_file.writeText( m_tabs + cuT( "\tintensity " ) + string::toString( p_object.m_intensity ) + cuT( "\n" ) ) > 0;
				castor::TextWriter< SsaoConfig >::checkError( result, "SsaoConfig intensity" );
			}

			if ( result )
			{
				result = p_file.writeText( m_tabs + cuT( "\tradius " ) + string::toString( p_object.m_radius ) + cuT( "\n" ) ) > 0;
				castor::TextWriter< SsaoConfig >::checkError( result, "SsaoConfig radius" );
			}

			if ( result )
			{
				result = p_file.writeText( m_tabs + cuT( "\tbias " ) + string::toString( p_object.m_bias ) + cuT( "\n" ) ) > 0;
				castor::TextWriter< SsaoConfig >::checkError( result, "SsaoConfig bias" );
			}

			if ( result )
			{
				result = p_file.writeText( m_tabs + cuT( "\tnum_samples " ) + string::toString( p_object.m_numSamples ) + cuT( "\n" ) ) > 0;
				castor::TextWriter< SsaoConfig >::checkError( result, "SsaoConfig num samples" );
			}

			if ( result )
			{
				result = p_file.writeText( m_tabs + cuT( "\tedge_sharpness " ) + string::toString( p_object.m_edgeSharpness ) + cuT( "\n" ) ) > 0;
				castor::TextWriter< SsaoConfig >::checkError( result, "SsaoConfig edge sharpness" );
			}

			//if ( result )
			//{
			//	result = p_file.writeText( m_tabs + cuT( "\tblur_high_quality " ) + ( p_object.m_blurHighQuality ? cuT( "true" ) : cuT( "false" ) ) + cuT( "\n" ) ) > 0;
			//	castor::TextWriter< SsaoConfig >::checkError( result, "SsaoConfig blur high quality" );
			//}

			if ( result )
			{
				result = p_file.writeText( m_tabs + cuT( "\tblur_step_size " ) + string::toString( p_object.m_blurStepSize ) + cuT( "\n" ) ) > 0;
				castor::TextWriter< SsaoConfig >::checkError( result, "SsaoConfig blur step size" );
			}

			if ( result )
			{
				result = p_file.writeText( m_tabs + cuT( "\tblur_radius " ) + string::toString( p_object.m_blurRadius ) + cuT( "\n" ) ) > 0;
				castor::TextWriter< SsaoConfig >::checkError( result, "SsaoConfig blur radius" );
			}

			p_file.writeText( m_tabs + cuT( "}\n" ) );
		}

		return result;
	}
}
