#include "LegacyPass.hpp"

#include "Engine.hpp"

#include "Material/Material.hpp"
#include "Render/RenderNode/RenderNode.hpp"
#include "Scene/Scene.hpp"
#include "Shader/PassBuffer/PassBuffer.hpp"
#include "Texture/TextureLayout.hpp"

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	LegacyPass::TextWriter::TextWriter( String const & p_tabs )
		: castor::TextWriter< LegacyPass >{ p_tabs }
	{
	}

	bool LegacyPass::TextWriter::operator()( LegacyPass const & p_pass, TextFile & p_file )
	{
		Logger::logInfo( m_tabs + cuT( "Writing LegacyPass " ) );
		bool result = p_file.writeText( cuT( "\n" ) + m_tabs + cuT( "pass\n" ) ) > 0
						&& p_file.writeText( m_tabs + cuT( "{\n" ) ) > 0;

		if ( result )
		{
			result = p_file.print( 256, cuT( "%s\tdiffuse " ), m_tabs.c_str() ) > 0
				&& RgbColour::TextWriter( String() )( p_pass.getDiffuse(), p_file )
				&& p_file.writeText( cuT( "\n" ) ) > 0;
			castor::TextWriter< LegacyPass >::checkError( result, "LegacyPass diffuse colour" );
		}

		if ( result )
		{
			result = p_file.print( 256, cuT( "%s\tspecular " ), m_tabs.c_str() ) > 0
				&& RgbColour::TextWriter( String() )( p_pass.getSpecular(), p_file )
				&& p_file.writeText( cuT( "\n" ) ) > 0;
			castor::TextWriter< LegacyPass >::checkError( result, "LegacyPass specular colour" );
		}

		if ( result )
		{
			result = p_file.writeText( m_tabs + cuT( "\tambient " ) + string::toString( p_pass.getAmbient() ) + cuT( "\n" ) ) > 0;
			castor::TextWriter< LegacyPass >::checkError( result, "LegacyPass ambient" );
		}

		if ( result )
		{
			result = p_file.writeText( m_tabs + cuT( "\tshininess " ) + string::toString( p_pass.getShininess() ) + cuT( "\n" ) ) > 0;
			castor::TextWriter< LegacyPass >::checkError( result, "LegacyPass shininess" );
		}

		if ( result )
		{
			result = Pass::TextWriter{ m_tabs }( p_pass, p_file );
		}

		if ( result )
		{
			result = p_file.writeText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return result;
	}

	//*********************************************************************************************

	LegacyPass::LegacyPass( Material & p_parent )
		: Pass{ p_parent }
		, m_diffuse{ RgbColour::fromRGBA( 0xFFFFFFFF ) }
		, m_specular{ RgbColour::fromRGBA( 0xFFFFFFFF ) }
	{
	}

	LegacyPass::~LegacyPass()
	{
	}

	void LegacyPass::accept( PassBuffer & p_buffer )const
	{
		p_buffer.visit( *this );
	}

	void LegacyPass::doInitialise()
	{
	}

	void LegacyPass::doCleanup()
	{
	}

	void LegacyPass::doSetOpacity( float p_value )
	{
	}
}
