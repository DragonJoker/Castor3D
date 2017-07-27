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
#include "Shader/PassBuffer.hpp"
#include "Shader/ShaderProgram.hpp"
#include "State/BlendState.hpp"
#include "Texture/TextureLayout.hpp"
#include "Texture/TextureUnit.hpp"

#include <Log/Logger.hpp>

using namespace Castor;

namespace Castor3D
{
	//*********************************************************************************************

	SpecularGlossinessPbrPass::TextWriter::TextWriter( String const & p_tabs )
		: Castor::TextWriter< SpecularGlossinessPbrPass >{ p_tabs }
	{
	}

	bool SpecularGlossinessPbrPass::TextWriter::operator()( SpecularGlossinessPbrPass const & p_pass, TextFile & p_file )
	{
		Logger::LogInfo( m_tabs + cuT( "Writing SpecularGlossinessPbrPass " ) );
		bool result = p_file.WriteText( cuT( "\n" ) + m_tabs + cuT( "pass\n" ) ) > 0
						&& p_file.WriteText( m_tabs + cuT( "{\n" ) ) > 0;
		
		if ( result )
		{
			result = p_file.Print( 256, cuT( "%s\talbedo " ), m_tabs.c_str() ) > 0
					   && Colour::TextWriter( String() )( p_pass.GetDiffuse(), p_file )
					   && p_file.WriteText( cuT( "\n" ) ) > 0;
			Castor::TextWriter< SpecularGlossinessPbrPass >::CheckError( result, "SpecularGlossinessPbrPass albedo" );
		}

		if ( result )
		{
			result = p_file.WriteText( m_tabs + cuT( "\tglossiness " )
				+ string::to_string( p_pass.GetGlossiness() )
				+ cuT( "\n" ) ) > 0;
			Castor::TextWriter< SpecularGlossinessPbrPass >::CheckError( result, "SpecularGlossinessPbrPass glossiness" );
		}

		if ( result )
		{
			result = p_file.Print( 256, cuT( "%s\tspecular " ), m_tabs.c_str() ) > 0
				&& Colour::TextWriter( String() )( p_pass.GetDiffuse(), p_file )
				&& p_file.WriteText( cuT( "\n" ) ) > 0;
			Castor::TextWriter< SpecularGlossinessPbrPass >::CheckError( result, "SpecularGlossinessPbrPass specular" );
		}

		if ( result )
		{
			result = Castor::TextWriter< Pass >{ m_tabs }( p_pass, p_file );
		}

		if ( result )
		{
			result = p_file.WriteText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return result;
	}

	//*********************************************************************************************

	SpecularGlossinessPbrPass::SpecularGlossinessPbrPass( Material & p_parent )
		: Pass{ p_parent }
		, m_diffuse{ Colour::from_rgba( 0xFFFFFFFF ) }
		, m_specular{ Colour::from_rgba( 0xFFFFFFFF ) }
	{
	}

	SpecularGlossinessPbrPass::~SpecularGlossinessPbrPass()
	{
	}

	void SpecularGlossinessPbrPass::Accept( PassBuffer & p_buffer )const
	{
		p_buffer.Visit( *this );
	}

	void SpecularGlossinessPbrPass::DoInitialise()
	{
	}

	void SpecularGlossinessPbrPass::DoCleanup()
	{
	}

	void SpecularGlossinessPbrPass::DoSetOpacity( float p_value )
	{
		m_diffuse.alpha() = p_value;
	}
}
