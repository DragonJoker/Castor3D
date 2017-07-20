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

	MetallicRoughnessPbrPass::TextWriter::TextWriter( String const & tabs )
		: Castor::TextWriter< MetallicRoughnessPbrPass >{ tabs }
	{
	}

	bool MetallicRoughnessPbrPass::TextWriter::operator()( MetallicRoughnessPbrPass const & pass, TextFile & file )
	{
		Logger::LogInfo( m_tabs + cuT( "Writing MetallicRoughnessPbrPass " ) );
		bool result = file.WriteText( cuT( "\n" ) + m_tabs + cuT( "pass\n" ) ) > 0
						&& file.WriteText( m_tabs + cuT( "{\n" ) ) > 0;
		
		if ( result )
		{
			result = file.Print( 256, cuT( "%s\talbedo " ), m_tabs.c_str() ) > 0
					   && Colour::TextWriter( String() )( pass.GetAlbedo(), file )
					   && file.WriteText( cuT( "\n" ) ) > 0;
			Castor::TextWriter< MetallicRoughnessPbrPass >::CheckError( result, "MetallicRoughnessPbrPass albedo" );
		}

		if ( result && pass.GetRoughness() != 1 )
		{
			result = file.WriteText( m_tabs + cuT( "\troughness " )
				+ string::to_string( pass.GetRoughness() )
				+ cuT( "\n" ) ) > 0;
			Castor::TextWriter< MetallicRoughnessPbrPass >::CheckError( result, "MetallicRoughnessPbrPass roughness" );
		}

		if ( result && pass.GetMetallic() != 0 )
		{
			result = file.WriteText( m_tabs + cuT( "\tmetallic " )
				+ string::to_string( pass.GetMetallic() )
				+ cuT( "\n" ) ) > 0;
			Castor::TextWriter< MetallicRoughnessPbrPass >::CheckError( result, "MetallicRoughnessPbrPass reflectance" );
		}

		if ( result )
		{
			result = Pass::TextWriter{ m_tabs }( pass, file );
		}

		if ( result )
		{
			result = file.WriteText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return result;
	}

	//*********************************************************************************************

	MetallicRoughnessPbrPass::MetallicRoughnessPbrPass( Material & parent )
		: Pass{ parent }
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

	void MetallicRoughnessPbrPass::DoUpdate( PassBuffer & buffer )const
	{
		buffer.SetComponents( GetId() - 1
			, 0u
			, GetRefractionRatio()
			, CheckFlag( GetTextureFlags(), TextureChannel::eRefraction ) ? 1.0f : 0.0f
			, CheckFlag( GetTextureFlags(), TextureChannel::eReflection ) ? 1.0f : 0.0f
			, GetOpacity() );
		buffer.SetComponents( GetId() - 1
			, 1u
			, GetAlbedo()
			, GetRoughness() );
		buffer.SetComponents( GetId() - 1
			, 2u
			, GetMetallic()
			, GetEmissive()
			// TODO: store gamma correction and exposure per pass ? or remove from pass
			, NeedsGammaCorrection() ? 2.2f : 1.0f //gamma correction
			, 1.0f );//exposure
		buffer.SetComponents( GetId() - 1
			, 3u
			, GetAlphaValue()
			, 0.0f
			, 0.0f
			, 0.0f );
	}

	void MetallicRoughnessPbrPass::DoSetOpacity( float value )
	{
		m_albedo.alpha() = value;
	}
}
