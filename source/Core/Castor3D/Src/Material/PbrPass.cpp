#include "PbrPass.hpp"

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

	PbrPass::TextWriter::TextWriter( String const & p_tabs )
		: Castor::TextWriter< PbrPass >{ p_tabs }
	{
	}

	bool PbrPass::TextWriter::operator()( PbrPass const & p_pass, TextFile & p_file )
	{
		Logger::LogInfo( m_tabs + cuT( "Writing PbrPass " ) );
		bool l_return = p_file.WriteText( cuT( "\n" ) + m_tabs + cuT( "pass\n" ) ) > 0
						&& p_file.WriteText( m_tabs + cuT( "{\n" ) ) > 0;
		
		if ( l_return )
		{
			l_return = p_file.Print( 256, cuT( "%s\talbedo " ), m_tabs.c_str() ) > 0
					   && HdrColour::TextWriter( String() )( p_pass.GetAlbedo(), p_file )
					   && p_file.WriteText( cuT( "\n" ) ) > 0;
			Castor::TextWriter< PbrPass >::CheckError( l_return, "PbrPass albedo" );
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "\troughness " )
				+ string::to_string( p_pass.GetRoughness() )
				+ cuT( "\n" ) ) > 0;
			Castor::TextWriter< PbrPass >::CheckError( l_return, "PbrPass roughness" );
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "\treflectance " )
				+ string::to_string( p_pass.GetReflectance() )
				+ cuT( "\n" ) ) > 0;
			Castor::TextWriter< PbrPass >::CheckError( l_return, "PbrPass reflectance" );
		}

		if ( l_return )
		{
			l_return = Castor::TextWriter< Pass >{ m_tabs }( p_pass, p_file );
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return l_return;
	}

	//*********************************************************************************************

	PbrPass::PbrPass( Material & p_parent )
		: Pass{ p_parent }
		, m_albedo{ HdrColour::from_rgba( 0xFFFFFFFF ) }
	{
	}

	PbrPass::~PbrPass()
	{
	}

	void PbrPass::DoInitialise()
	{
	}

	void PbrPass::DoCleanup()
	{
	}

	void PbrPass::DoUpdate( PassBuffer & p_buffer )const
	{
		p_buffer.SetComponents( GetId() - 1
			, 0u
			, GetRefractionRatio()
			, CheckFlag( GetTextureFlags(), TextureChannel::eRefraction ) ? 1.0f : 0.0f
			, CheckFlag( GetTextureFlags(), TextureChannel::eReflection ) ? 1.0f : 0.0f
			, GetOpacity() );
		p_buffer.SetComponents( GetId() - 1
			, 1u
			, GetAlbedo()
			, GetRoughness() );
		p_buffer.SetComponents( GetId() - 1
			, 2u
			, GetReflectance()
			, GetEmissive()
			// TODO: store gamma correction and exposure per pass ? or remove from pass
			, NeedsGammaCorrection() ? 2.2f : 1.0f //gamma correction
			, 1.0f );//exposure
		p_buffer.SetComponents( GetId() - 1
			, 3u
			, GetAlphaValue()
			, 0.0f
			, 0.0f
			, 0.0f );
	}

	void PbrPass::DoSetOpacity( float p_value )
	{
		m_albedo.alpha() = p_value;
	}
}
