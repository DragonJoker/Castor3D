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
			doGetTexture( p_pass, p_program, TextureChannel::eDiffuse, cuT( "c3d_mapDiffuse" ), *this );
			doGetTexture( p_pass, p_program, TextureChannel::eSpecular, cuT( "c3d_mapSpecular" ), *this );
			doGetTexture( p_pass, p_program, TextureChannel::eGloss, cuT( "c3d_mapGloss" ), *this );
			break;

		case MaterialType::ePbrMetallicRoughness:
			doGetTexture( p_pass, p_program, TextureChannel::eAlbedo, cuT( "c3d_mapAlbedo" ), *this );
			doGetTexture( p_pass, p_program, TextureChannel::eMetallic, cuT( "c3d_mapMetallic" ), *this );
			doGetTexture( p_pass, p_program, TextureChannel::eRoughness, cuT( "c3d_mapRoughness" ), *this );
			break;

		case MaterialType::ePbrSpecularGlossiness:
			doGetTexture( p_pass, p_program, TextureChannel::eAlbedo, cuT( "c3d_mapDiffuse" ), *this );
			doGetTexture( p_pass, p_program, TextureChannel::eSpecular, cuT( "c3d_mapSpecular" ), *this );
			doGetTexture( p_pass, p_program, TextureChannel::eGloss, cuT( "c3d_mapGloss" ), *this );
			break;

		default:
			FAILURE( "Unsupported material type" );
		}

		doGetTexture( p_pass, p_program, TextureChannel::eAmbientOcclusion, ShaderProgram::MapAmbientOcclusion, *this );
		doGetTexture( p_pass, p_program, TextureChannel::eEmissive, cuT( "c3d_mapEmissive" ), *this );
		doGetTexture( p_pass, p_program, TextureChannel::eOpacity, cuT( "c3d_mapOpacity" ), *this );
		doGetTexture( p_pass, p_program, TextureChannel::eHeight, cuT( "c3d_mapHeight" ), *this );
		doGetTexture( p_pass, p_program, TextureChannel::eNormal, cuT( "c3d_mapNormal" ), *this );
		doGetTexture( p_pass, p_program, TextureChannel::eTransmittance, ShaderProgram::MapTransmittance, *this );
	}
}
