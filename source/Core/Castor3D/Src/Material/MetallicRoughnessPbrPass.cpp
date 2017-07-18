#include "MetallicRoughnessPbrPass.hpp"

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

	MetallicRoughnessPbrPass::TextWriter::TextWriter( String const & p_tabs )
		: Castor::TextWriter< MetallicRoughnessPbrPass >{ p_tabs }
	{
	}

	bool MetallicRoughnessPbrPass::TextWriter::operator()( MetallicRoughnessPbrPass const & p_pass, TextFile & p_file )
	{
		Logger::LogInfo( m_tabs + cuT( "Writing MetallicRoughnessPbrPass " ) );
		bool result = p_file.WriteText( cuT( "\n" ) + m_tabs + cuT( "pass\n" ) ) > 0
						&& p_file.WriteText( m_tabs + cuT( "{\n" ) ) > 0;
		
		if ( result )
		{
			result = p_file.Print( 256, cuT( "%s\talbedo " ), m_tabs.c_str() ) > 0
					   && Colour::TextWriter( String() )( p_pass.GetAlbedo(), p_file )
					   && p_file.WriteText( cuT( "\n" ) ) > 0;
			Castor::TextWriter< MetallicRoughnessPbrPass >::CheckError( result, "MetallicRoughnessPbrPass albedo" );
		}

		if ( result )
		{
			result = p_file.WriteText( m_tabs + cuT( "\troughness " )
				+ string::to_string( p_pass.GetRoughness() )
				+ cuT( "\n" ) ) > 0;
			Castor::TextWriter< MetallicRoughnessPbrPass >::CheckError( result, "MetallicRoughnessPbrPass roughness" );
		}

		if ( result )
		{
			result = p_file.WriteText( m_tabs + cuT( "\tmetallic " )
				+ string::to_string( p_pass.GetMetallic() )
				+ cuT( "\n" ) ) > 0;
			Castor::TextWriter< MetallicRoughnessPbrPass >::CheckError( result, "MetallicRoughnessPbrPass reflectance" );
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

	MetallicRoughnessPbrPass::MetallicRoughnessPbrPass( Material & p_parent )
		: Pass{ p_parent }
		, m_albedo{ Colour::from_rgba( 0xFFFFFFFF ) }
	{
	}

	MetallicRoughnessPbrPass::~MetallicRoughnessPbrPass()
	{
	}

	void MetallicRoughnessPbrPass::DoInitialise()
	{
	}

	void MetallicRoughnessPbrPass::DoCleanup()
	{
	}

	void MetallicRoughnessPbrPass::DoUpdate( PassBuffer & p_buffer )const
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
			, GetMetallic()
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

	void MetallicRoughnessPbrPass::DoSetOpacity( float p_value )
	{
		m_albedo.alpha() = p_value;
	}
}
