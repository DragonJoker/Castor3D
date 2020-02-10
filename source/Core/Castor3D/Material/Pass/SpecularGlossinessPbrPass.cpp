#include "Castor3D/Material/SpecularGlossinessPbrPass.hpp"

#include "Castor3D/Shader/PassBuffer/PassBuffer.hpp"

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	SpecularGlossinessPbrPass::TextWriter::TextWriter( String const & tabs )
		: castor::TextWriter< SpecularGlossinessPbrPass >{ tabs }
	{
	}

	bool SpecularGlossinessPbrPass::TextWriter::operator()( SpecularGlossinessPbrPass const & pass, TextFile & file )
	{
		Logger::logInfo( m_tabs + cuT( "Writing SpecularGlossinessPbrPass " ) );
		bool result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "pass\n" ) ) > 0
						&& file.writeText( m_tabs + cuT( "{\n" ) ) > 0;
		
		if ( result )
		{
			result = file.print( 256, cuT( "%s\tdiffuse " ), m_tabs.c_str() ) > 0
				&& RgbColour::TextWriter( String() )( pass.getDiffuse(), file )
				&& file.writeText( cuT( "\n" ) ) > 0;
			castor::TextWriter< SpecularGlossinessPbrPass >::checkError( result, "SpecularGlossinessPbrPass albedo" );
		}

		if ( result )
		{
			result = file.print( 256, cuT( "%s\tspecular " ), m_tabs.c_str() ) > 0
				&& RgbColour::TextWriter( String() )( pass.getSpecular(), file )
				&& file.writeText( cuT( "\n" ) ) > 0;
			castor::TextWriter< SpecularGlossinessPbrPass >::checkError( result, "SpecularGlossinessPbrPass specular" );
		}

		if ( result )
		{
			result = file.writeText( m_tabs + cuT( "\tglossiness " )
				+ string::toString( pass.getGlossiness(), std::locale{ "C" } )
				+ cuT( "\n" ) ) > 0;
			castor::TextWriter< SpecularGlossinessPbrPass >::checkError( result, "SpecularGlossinessPbrPass glossiness" );
		}

		if ( result )
		{
			result = Pass::TextWriter{ m_tabs }( pass, file );
		}

		if ( result )
		{
			result = file.writeText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return result;
	}

	//*********************************************************************************************

	SpecularGlossinessPbrPass::SpecularGlossinessPbrPass( Material & parent )
		: Pass{ parent }
		, m_diffuse{ RgbColour::fromRGBA( 0xFFFFFFFF ) }
		, m_specular{ RgbColour::fromRGBA( 0xFFFFFFFF ) }
	{
	}

	SpecularGlossinessPbrPass::~SpecularGlossinessPbrPass()
	{
	}

	void SpecularGlossinessPbrPass::accept( PassBuffer & buffer )const
	{
		buffer.visit( *this );
	}

	void SpecularGlossinessPbrPass::doInitialise()
	{
	}

	void SpecularGlossinessPbrPass::doCleanup()
	{
	}

	void SpecularGlossinessPbrPass::doSetOpacity( float value )
	{
	}
}
