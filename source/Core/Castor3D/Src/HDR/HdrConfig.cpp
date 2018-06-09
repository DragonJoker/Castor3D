#include "HdrConfig.hpp"

using namespace castor;

namespace castor3d
{
	HdrConfig::TextWriter::TextWriter( String const & tabs )
		: castor::TextWriter< HdrConfig >{ tabs }
	{
	}

	bool HdrConfig::TextWriter::operator()( HdrConfig const & obj, TextFile & file )
	{
		bool result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "hdr_config\n" ) ) > 0
			&& file.writeText( m_tabs + cuT( "{\n" ) ) > 0;

		if ( result )
		{
			result = file.writeText( m_tabs + cuT( "\texposure " ) + string::toString( obj.getExposure(), std::locale{ "C" } ) + cuT( "\n" ) ) > 0;
			castor::TextWriter< HdrConfig >::checkError( result, "HdrConfig exposure." );
		}

		if ( result )
		{
			result = file.writeText( m_tabs + cuT( "\tgamma " ) + string::toString( obj.getGamma(), std::locale{ "C" } ) + cuT( "\n" ) ) > 0;
			castor::TextWriter< HdrConfig >::checkError( result, "HdrConfig gamma." );
		}

		if ( result )
		{
			result = file.writeText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return result;
	}
}
