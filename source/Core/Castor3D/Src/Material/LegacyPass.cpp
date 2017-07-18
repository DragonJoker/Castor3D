#include "LegacyPass.hpp"

#include "Engine.hpp"

#include "Material/Material.hpp"
#include "Render/RenderNode/RenderNode.hpp"
#include "Scene/Scene.hpp"
#include "Shader/PassBuffer.hpp"
#include "Texture/TextureLayout.hpp"

using namespace Castor;

namespace Castor3D
{
	//*********************************************************************************************

	LegacyPass::TextWriter::TextWriter( String const & p_tabs )
		: Castor::TextWriter< LegacyPass >{ p_tabs }
	{
	}

	bool LegacyPass::TextWriter::operator()( LegacyPass const & p_pass, TextFile & p_file )
	{
		Logger::LogInfo( m_tabs + cuT( "Writing LegacyPass " ) );
		bool result = p_file.WriteText( cuT( "\n" ) + m_tabs + cuT( "pass\n" ) ) > 0
						&& p_file.WriteText( m_tabs + cuT( "{\n" ) ) > 0;

		if ( result )
		{
			result = p_file.Print( 256, cuT( "%s\tdiffuse " ), m_tabs.c_str() ) > 0
				&& Colour::TextWriter( String() )( p_pass.GetDiffuse(), p_file )
				&& p_file.WriteText( cuT( "\n" ) ) > 0;
			Castor::TextWriter< LegacyPass >::CheckError( result, "LegacyPass diffuse colour" );
		}

		if ( result )
		{
			result = p_file.Print( 256, cuT( "%s\tspecular " ), m_tabs.c_str() ) > 0
					   && Colour::TextWriter( String() )( p_pass.GetSpecular(), p_file )
				&& p_file.WriteText( cuT( "\n" ) ) > 0;
			Castor::TextWriter< LegacyPass >::CheckError( result, "LegacyPass specular colour" );
		}

		if ( result )
		{
			result = p_file.WriteText( m_tabs + cuT( "\tambient " ) + string::to_string( p_pass.GetAmbient() ) + cuT( "\n" ) ) > 0;
			Castor::TextWriter< LegacyPass >::CheckError( result, "LegacyPass ambient" );
		}

		if ( result )
		{
			result = p_file.WriteText( m_tabs + cuT( "\tshininess " ) + string::to_string( p_pass.GetShininess() ) + cuT( "\n" ) ) > 0;
			Castor::TextWriter< LegacyPass >::CheckError( result, "LegacyPass shininess" );
		}

		if ( result )
		{
			result = Pass::TextWriter{ m_tabs }( p_pass, p_file );
		}

		if ( result )
		{
			result = p_file.WriteText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return result;
	}

	//*********************************************************************************************

	LegacyPass::LegacyPass( Material & p_parent )
		: Pass{ p_parent }
		, m_diffuse{ Colour::from_rgba( 0xFFFFFFFF ) }
		, m_specular{ Colour::from_rgba( 0xFFFFFFFF ) }
	{
	}

	LegacyPass::~LegacyPass()
	{
	}

	void LegacyPass::DoInitialise()
	{
	}

	void LegacyPass::DoCleanup()
	{
	}

	void LegacyPass::DoUpdate( PassBuffer & p_buffer )const
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
			, GetAmbient() );
		p_buffer.SetComponents( GetId() - 1
			, 2u
			, GetSpecular()
			, GetEmissive() );
		p_buffer.SetComponents( GetId() - 1
			, 3u
			, GetShininess()
			// TODO: store gamma correction and exposure per pass ? or remove from pass
			, NeedsGammaCorrection() ? 2.2f : 1.0f //gamma correction
			, 1.0f //exposure
			, GetAlphaValue() );
	}

	void LegacyPass::DoSetOpacity( float p_value )
	{
		m_diffuse.alpha() = p_value;
		m_specular.alpha() = p_value;
	}
}
