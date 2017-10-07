#include "PassRenderNode.hpp"

#include "Material/Pass.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Shader/UniformBuffer.hpp"
#include "Texture/TextureUnit.hpp"

using namespace castor;

namespace castor3d
{
	namespace
	{
		void doGetTexture( Pass const & p_pass
			, ShaderProgram const & p_program
			, TextureChannel p_channel
			, String const & p_name
			, PassRenderNode & p_node )
		{
			TextureUnitSPtr unit = p_pass.getTextureUnit( p_channel );

			if ( unit )
			{
				auto variable = p_program.findUniform< UniformType::eSampler >( p_name, ShaderType::ePixel );

				if ( variable )
				{
					p_node.m_textures.emplace( unit->getIndex(), *variable );
				}
			}
		}
	}

	PassRenderNode::PassRenderNode( Pass & p_pass
		, ShaderProgram const & p_program )
		: m_pass{ p_pass }
	{
		switch ( p_pass.getType() )
		{
		case MaterialType::eLegacy:
			doGetTexture( p_pass, p_program, TextureChannel::eDiffuse, ShaderProgram::MapDiffuse, *this );
			doGetTexture( p_pass, p_program, TextureChannel::eSpecular, ShaderProgram::MapSpecular, *this );
			doGetTexture( p_pass, p_program, TextureChannel::eGloss, ShaderProgram::MapGloss, *this );
			break;

		case MaterialType::ePbrMetallicRoughness:
			doGetTexture( p_pass, p_program, TextureChannel::eAlbedo, ShaderProgram::MapAlbedo, *this );
			doGetTexture( p_pass, p_program, TextureChannel::eMetallic, ShaderProgram::MapMetallic, *this );
			doGetTexture( p_pass, p_program, TextureChannel::eRoughness, ShaderProgram::MapRoughness, *this );
			break;

		case MaterialType::ePbrSpecularGlossiness:
			doGetTexture( p_pass, p_program, TextureChannel::eAlbedo, ShaderProgram::MapDiffuse, *this );
			doGetTexture( p_pass, p_program, TextureChannel::eSpecular, ShaderProgram::MapSpecular, *this );
			doGetTexture( p_pass, p_program, TextureChannel::eGloss, ShaderProgram::MapGloss, *this );
			break;

		default:
			FAILURE( "Unsupported material type" );
		}

		doGetTexture( p_pass, p_program, TextureChannel::eAmbientOcclusion, ShaderProgram::MapAmbientOcclusion, *this );
		doGetTexture( p_pass, p_program, TextureChannel::eEmissive, ShaderProgram::MapEmissive, *this );
		doGetTexture( p_pass, p_program, TextureChannel::eOpacity, ShaderProgram::MapOpacity, *this );
		doGetTexture( p_pass, p_program, TextureChannel::eHeight, ShaderProgram::MapHeight, *this );
		doGetTexture( p_pass, p_program, TextureChannel::eNormal, ShaderProgram::MapNormal, *this );
		doGetTexture( p_pass, p_program, TextureChannel::eTransmittance, ShaderProgram::MapTransmittance, *this );
	}
}
