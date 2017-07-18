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
		bool l_result = p_file.WriteText( cuT( "\n" ) + m_tabs + cuT( "pass\n" ) ) > 0
						&& p_file.WriteText( m_tabs + cuT( "{\n" ) ) > 0;
		
		if ( l_result )
		{
			l_result = p_file.Print( 256, cuT( "%s\talbedo " ), m_tabs.c_str() ) > 0
					   && Colour::TextWriter( String() )( p_pass.GetDiffuse(), p_file )
					   && p_file.WriteText( cuT( "\n" ) ) > 0;
			Castor::TextWriter< SpecularGlossinessPbrPass >::CheckError( l_result, "SpecularGlossinessPbrPass albedo" );
		}

		if ( l_result )
		{
			l_result = p_file.WriteText( m_tabs + cuT( "\tglossiness " )
				+ string::to_string( p_pass.GetGlossiness() )
				+ cuT( "\n" ) ) > 0;
			Castor::TextWriter< SpecularGlossinessPbrPass >::CheckError( l_result, "SpecularGlossinessPbrPass glossiness" );
		}

		if ( l_result )
		{
			l_result = p_file.Print( 256, cuT( "%s\tspecular " ), m_tabs.c_str() ) > 0
				&& Colour::TextWriter( String() )( p_pass.GetDiffuse(), p_file )
				&& p_file.WriteText( cuT( "\n" ) ) > 0;
			Castor::TextWriter< SpecularGlossinessPbrPass >::CheckError( l_result, "SpecularGlossinessPbrPass specular" );
		}

		if ( l_result )
		{
			l_result = Castor::TextWriter< Pass >{ m_tabs }( p_pass, p_file );
		}

		if ( l_result )
		{
			l_result = p_file.WriteText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return l_result;
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

	void SpecularGlossinessPbrPass::DoInitialise()
	{
	}

	void SpecularGlossinessPbrPass::DoCleanup()
	{
	}

	void SpecularGlossinessPbrPass::DoUpdate( PassBuffer & p_buffer )const
	{
		p_buffer.SetComponents( GetId() - 1
			, 0u
			, GetRefractionRatio()
			, CheckFlag( GetTextureFlags(), TextureChannel::eRefraction ) ? 1.0f : 0.0f
			, CheckFlag( GetTextureFlags(), TextureChannel::eReflection ) ? 1.0f : 0.0f
			, GetOpacity() );
		p_buffer.SetComponents( GetId() - 1
			, 1u
			, GetDiffuse()
			, 0.0f );
		p_buffer.SetComponents( GetId() - 1
			, 2u
			, GetSpecular()
			, GetEmissive() );
		p_buffer.SetComponents( GetId() - 1
			, 3u
			, GetGlossiness()
			// TODO: store gamma correction and exposure per pass ? or remove from pass
			, NeedsGammaCorrection() ? 2.2f : 1.0f //gamma correction
			, 1.0f //exposure
			, GetAlphaValue() );
	}

	void SpecularGlossinessPbrPass::DoSetOpacity( float p_value )
	{
		m_diffuse.alpha() = p_value;
	}
}
