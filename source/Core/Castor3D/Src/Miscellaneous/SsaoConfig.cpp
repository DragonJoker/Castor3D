#include "SsaoConfig.hpp"

using namespace Castor;

namespace Castor3D
{
	SsaoConfig::TextWriter::TextWriter( String const & p_tabs )
		: Castor::TextWriter< SsaoConfig >{ p_tabs }
	{
	}

	bool SsaoConfig::TextWriter::operator()( SsaoConfig const & p_object, TextFile & p_file )
	{
		bool l_result{ true };

		if ( p_object.m_enabled )
		{
			Logger::LogInfo( m_tabs + cuT( "Writing SsaoConfig" ) );
			l_result = p_file.WriteText( cuT( "\n" ) + m_tabs + cuT( "ssao\n" ) + m_tabs + cuT( "{\n" ) ) > 0;

			if ( l_result )
			{
				l_result = p_file.WriteText( m_tabs + cuT( "\tenabled " ) + ( p_object.m_enabled ? cuT( "true" ) : cuT( "false" ) ) + cuT( "\n" ) ) > 0;
				Castor::TextWriter< SsaoConfig >::CheckError( l_result, "SsaoConfig enabled" );
			}

			if ( l_result )
			{
				l_result = p_file.WriteText( m_tabs + cuT( "\tradius " ) + string::to_string( p_object.m_radius ) + cuT( "\n" ) ) > 0;
				Castor::TextWriter< SsaoConfig >::CheckError( l_result, "SsaoConfig radius" );
			}

			if ( l_result )
			{
				l_result = p_file.WriteText( m_tabs + cuT( "\tbias " ) + string::to_string( p_object.m_bias ) + cuT( "\n" ) ) > 0;
				Castor::TextWriter< SsaoConfig >::CheckError( l_result, "SsaoConfig bias" );
			}

			p_file.WriteText( m_tabs + cuT( "}\n" ) );
		}

		return l_result;
	}
}
