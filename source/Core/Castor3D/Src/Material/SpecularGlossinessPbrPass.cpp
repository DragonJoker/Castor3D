#include "SpecularGlossinessPbrPass.hpp"

#include "Engine.hpp"
#include "Cache/CameraCache.hpp"
#include "Cache/MaterialCache.hpp"
#include "Cache/ShaderCache.hpp"

#include "Render/RenderPipeline.hpp"
#include "Render/RenderNode/RenderNode.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/SceneNode.hpp"
#include "Shader/UniformBuffer.hpp"
#include "Shader/PassBuffer/PassBuffer.hpp"
#include "Shader/ShaderProgram.hpp"
#include "State/BlendState.hpp"
#include "Texture/TextureLayout.hpp"
#include "Texture/TextureUnit.hpp"

#include <Log/Logger.hpp>

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	SpecularGlossinessPbrPass::TextWriter::TextWriter( String const & p_tabs )
		: castor::TextWriter< SpecularGlossinessPbrPass >{ p_tabs }
	{
	}

	bool SpecularGlossinessPbrPass::TextWriter::operator()( SpecularGlossinessPbrPass const & p_pass, TextFile & p_file )
	{
		Logger::logInfo( m_tabs + cuT( "Writing SpecularGlossinessPbrPass " ) );
		bool result = p_file.writeText( cuT( "\n" ) + m_tabs + cuT( "pass\n" ) ) > 0
						&& p_file.writeText( m_tabs + cuT( "{\n" ) ) > 0;
		
		if ( result )
		{
			result = p_file.print( 256, cuT( "%s\talbedo " ), m_tabs.c_str() ) > 0
					   && Colour::TextWriter( String() )( p_pass.getDiffuse(), p_file )
					   && p_file.writeText( cuT( "\n" ) ) > 0;
			castor::TextWriter< SpecularGlossinessPbrPass >::checkError( result, "SpecularGlossinessPbrPass albedo" );
		}

		if ( result )
		{
			result = p_file.writeText( m_tabs + cuT( "\tglossiness " )
				+ string::toString( p_pass.getGlossiness() )
				+ cuT( "\n" ) ) > 0;
			castor::TextWriter< SpecularGlossinessPbrPass >::checkError( result, "SpecularGlossinessPbrPass glossiness" );
		}

		if ( result )
		{
			result = p_file.print( 256, cuT( "%s\tspecular " ), m_tabs.c_str() ) > 0
				&& Colour::TextWriter( String() )( p_pass.getDiffuse(), p_file )
				&& p_file.writeText( cuT( "\n" ) ) > 0;
			castor::TextWriter< SpecularGlossinessPbrPass >::checkError( result, "SpecularGlossinessPbrPass specular" );
		}

		if ( result )
		{
			result = castor::TextWriter< Pass >{ m_tabs }( p_pass, p_file );
		}

		if ( result )
		{
			result = p_file.writeText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return result;
	}

	//*********************************************************************************************

	SpecularGlossinessPbrPass::SpecularGlossinessPbrPass( Material & p_parent )
		: Pass{ p_parent }
		, m_diffuse{ Colour::fromRGBA( 0xFFFFFFFF ) }
		, m_specular{ Colour::fromRGBA( 0xFFFFFFFF ) }
	{
	}

	SpecularGlossinessPbrPass::~SpecularGlossinessPbrPass()
	{
	}

	void SpecularGlossinessPbrPass::accept( PassBuffer & p_buffer )const
	{
		p_buffer.visit( *this );
	}

	void SpecularGlossinessPbrPass::doInitialise()
	{
	}

	void SpecularGlossinessPbrPass::doCleanup()
	{
	}

	void SpecularGlossinessPbrPass::doSetOpacity( float p_value )
	{
		m_diffuse.alpha() = p_value;
	}
}
