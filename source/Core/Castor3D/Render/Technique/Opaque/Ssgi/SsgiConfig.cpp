#include "Castor3D/Render/Technique/Opaque/Ssgi/SsgiConfig.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"

using namespace castor;

namespace castor3d
{
	SsgiConfig::TextWriter::TextWriter( String const & tabs )
		: castor::TextWriter< SsgiConfig >{ tabs }
	{
	}

	bool SsgiConfig::TextWriter::operator()( SsgiConfig const & object, TextFile & file )
	{
		bool result{ true };

		if ( object.enabled )
		{
			log::info << m_tabs << cuT( "Writing SsgiConfig" ) << std::endl;
			result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "ssgi\n" ) + m_tabs + cuT( "{\n" ) ) > 0;

			if ( result )
			{
				result = file.writeText( m_tabs + cuT( "\tenabled " ) + ( object.enabled ? cuT( "true" ) : cuT( "false" ) ) + cuT( "\n" ) ) > 0;
				castor::TextWriter< SsgiConfig >::checkError( result, "SsgiConfig enabled" );
			}

			if ( result )
			{
				result = file.writeText( m_tabs + cuT( "\tblurSize " ) + ( object.blurSize.value().value() ? cuT( "true" ) : cuT( "false" ) ) + cuT( "\n" ) ) > 0;
				castor::TextWriter< SsgiConfig >::checkError( result, "SsgiConfig blur size" );
			}

			file.writeText( m_tabs + cuT( "}\n" ) );
		}

		return result;
	}

	void SsgiConfig::accept( castor::String const & name
		, RenderTechniqueVisitor & visitor )
	{
		visitor.visit( name
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, cuT( "SSGI" )
			, cuT( "BlurSize" )
			, blurSize );
	}
}
