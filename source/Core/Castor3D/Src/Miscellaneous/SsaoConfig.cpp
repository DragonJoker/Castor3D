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
				result = p_file.writeText( m_tabs + cuT( "\tkernel_size " ) + string::toString( p_object.m_kernelSize.value() ) + cuT( "\n" ) ) > 0;
				castor::TextWriter< SsaoConfig >::checkError( result, "SsaoConfig kernel size" );
			}

			p_file.writeText( m_tabs + cuT( "}\n" ) );
		}

		return result;
	}
}
