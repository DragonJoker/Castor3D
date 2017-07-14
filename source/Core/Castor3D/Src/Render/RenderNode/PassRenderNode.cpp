#include "PassRenderNode.hpp"

#include "Material/Pass.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Shader/UniformBuffer.hpp"
#include "Texture/TextureUnit.hpp"

using namespace Castor;

namespace Castor3D
{
	namespace
	{
		void DoGetTexture( Pass const & p_pass
			, ShaderProgram const & p_program
			, TextureChannel p_channel
			, String const & p_name
			, PassRenderNode & p_node )
		{
			TextureUnitSPtr l_unit = p_pass.GetTextureUnit( p_channel );

			if ( l_unit )
			{
				auto l_variable = p_program.FindUniform< UniformType::eSampler >( p_name, ShaderType::ePixel );

				if ( l_variable )
				{
					p_node.m_textures.emplace( l_unit->GetIndex(), *l_variable );
				}
			}
		}
	}

	PassRenderNode::PassRenderNode( Pass & p_pass
		, ShaderProgram const & p_program )
		: m_pass{ p_pass }
	{
		if ( p_pass.GetType() == MaterialType::ePbrMetallicRoughness )
		{
			DoGetTexture( p_pass, p_program, TextureChannel::eAlbedo, ShaderProgram::MapAlbedo, *this );
			DoGetTexture( p_pass, p_program, TextureChannel::eMetallic, ShaderProgram::MapMetallic, *this );
			DoGetTexture( p_pass, p_program, TextureChannel::eRoughness, ShaderProgram::MapRoughness, *this );
			DoGetTexture( p_pass, p_program, TextureChannel::eAmbientOcclusion, ShaderProgram::MapAmbientOcclusion, *this );
		}
		else
		{
			DoGetTexture( p_pass, p_program, TextureChannel::eDiffuse, ShaderProgram::MapDiffuse, *this );
			DoGetTexture( p_pass, p_program, TextureChannel::eSpecular, ShaderProgram::MapSpecular, *this );
			DoGetTexture( p_pass, p_program, TextureChannel::eGloss, ShaderProgram::MapGloss, *this );
		}

		DoGetTexture( p_pass, p_program, TextureChannel::eEmissive, ShaderProgram::MapEmissive, *this );
		DoGetTexture( p_pass, p_program, TextureChannel::eOpacity, ShaderProgram::MapOpacity, *this );
		DoGetTexture( p_pass, p_program, TextureChannel::eHeight, ShaderProgram::MapHeight, *this );
		DoGetTexture( p_pass, p_program, TextureChannel::eNormal, ShaderProgram::MapNormal, *this );
	}
}
