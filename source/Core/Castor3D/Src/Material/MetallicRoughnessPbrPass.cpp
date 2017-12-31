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

	MetallicRoughnessPbrPass::TextWriter::TextWriter( String const & tabs )
		: castor::TextWriter< MetallicRoughnessPbrPass >{ tabs }
	{
	}

	bool MetallicRoughnessPbrPass::TextWriter::operator()( MetallicRoughnessPbrPass const & pass, TextFile & file )
	{
		Logger::logInfo( m_tabs + cuT( "Writing MetallicRoughnessPbrPass " ) );
		bool result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "pass\n" ) ) > 0
						&& file.writeText( m_tabs + cuT( "{\n" ) ) > 0;
		
		if ( result )
		{
			result = file.print( 256, cuT( "%s\talbedo " ), m_tabs.c_str() ) > 0
				&& RgbColour::TextWriter( String() )( pass.getAlbedo(), file )
				&& file.writeText( cuT( "\n" ) ) > 0;
			castor::TextWriter< MetallicRoughnessPbrPass >::checkError( result, "MetallicRoughnessPbrPass albedo" );
		}

		if ( result && pass.getRoughness() != 1 )
		{
			result = file.writeText( m_tabs + cuT( "\troughness " )
				+ string::toString( pass.getRoughness() )
				+ cuT( "\n" ) ) > 0;
			castor::TextWriter< MetallicRoughnessPbrPass >::checkError( result, "MetallicRoughnessPbrPass roughness" );
		}

		if ( result && pass.getMetallic() != 0 )
		{
			result = file.writeText( m_tabs + cuT( "\tmetallic " )
				+ string::toString( pass.getMetallic() )
				+ cuT( "\n" ) ) > 0;
			castor::TextWriter< MetallicRoughnessPbrPass >::checkError( result, "MetallicRoughnessPbrPass reflectance" );
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

	MetallicRoughnessPbrPass::MetallicRoughnessPbrPass( Material & parent )
		: Pass{ parent }
		, m_albedo{ RgbColour::fromRGBA( 0xFFFFFFFF ) }
	{
	}

	MetallicRoughnessPbrPass::~MetallicRoughnessPbrPass()
	{
	}

	void MetallicRoughnessPbrPass::accept( PassBuffer & p_buffer )const
	{
		p_buffer.visit( *this );
	}

	void MetallicRoughnessPbrPass::doInitialise()
	{
	}

	void MetallicRoughnessPbrPass::doCleanup()
	{
	}

	void MetallicRoughnessPbrPass::doSetOpacity( float value )
	{
	}
}
