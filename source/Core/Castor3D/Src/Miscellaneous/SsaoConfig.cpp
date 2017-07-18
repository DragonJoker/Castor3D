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
		bool result{ true };

		if ( p_object.m_enabled )
		{
			Logger::LogInfo( m_tabs + cuT( "Writing SsaoConfig" ) );
			result = p_file.WriteText( cuT( "\n" ) + m_tabs + cuT( "ssao\n" ) + m_tabs + cuT( "{\n" ) ) > 0;

			if ( result )
			{
				result = p_file.WriteText( m_tabs + cuT( "\tenabled " ) + ( p_object.m_enabled ? cuT( "true" ) : cuT( "false" ) ) + cuT( "\n" ) ) > 0;
				Castor::TextWriter< SsaoConfig >::CheckError( result, "SsaoConfig enabled" );
			}

			if ( result )
			{
				result = p_file.WriteText( m_tabs + cuT( "\tradius " ) + string::to_string( p_object.m_radius ) + cuT( "\n" ) ) > 0;
				Castor::TextWriter< SsaoConfig >::CheckError( result, "SsaoConfig radius" );
			}

			if ( result )
			{
				result = p_file.WriteText( m_tabs + cuT( "\tbias " ) + string::to_string( p_object.m_bias ) + cuT( "\n" ) ) > 0;
				Castor::TextWriter< SsaoConfig >::CheckError( result, "SsaoConfig bias" );
			}

			p_file.WriteText( m_tabs + cuT( "}\n" ) );
		}

		return result;
	}
}
