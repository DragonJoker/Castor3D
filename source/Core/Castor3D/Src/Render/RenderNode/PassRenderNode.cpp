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
			, PassRenderNodeUniforms & p_node )
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
	PassRenderNodeUniforms::PassRenderNodeUniforms( UniformBuffer & p_passUbo )
		: m_passUbo{ p_passUbo }
		, m_diffuse{ *p_passUbo.GetUniform< UniformType::eVec4f >( ShaderProgram::MatDiffuse ) }
		, m_specular{ *p_passUbo.GetUniform< UniformType::eVec4f >( ShaderProgram::MatSpecular ) }
		, m_emissive{ *p_passUbo.GetUniform< UniformType::eVec4f >( ShaderProgram::MatEmissive ) }
		, m_shininess{ *p_passUbo.GetUniform< UniformType::eFloat >( ShaderProgram::MatShininess ) }
		, m_opacity{ *p_passUbo.GetUniform< UniformType::eFloat >( ShaderProgram::MatOpacity ) }
		, m_environmentIndex{ *p_passUbo.GetUniform< UniformType::eFloat >( ShaderProgram::MatEnvironmentIndex ) }
		, m_refractionRatio{ *p_passUbo.GetUniform< UniformType::eFloat >( ShaderProgram::MatRefractionRatio ) }
	{
	}

	PassRenderNode::PassRenderNode( Pass & p_pass
		, ShaderProgram const & p_program
		, UniformBuffer & p_passUbo )
		: PassRenderNodeUniforms{ p_passUbo }
		, m_pass{ p_pass }
	{
		DoGetTexture( p_pass, p_program, TextureChannel::eDiffuse, ShaderProgram::MapDiffuse, *this );
		DoGetTexture( p_pass, p_program, TextureChannel::eSpecular, ShaderProgram::MapSpecular, *this );
		DoGetTexture( p_pass, p_program, TextureChannel::eEmissive, ShaderProgram::MapEmissive, *this );
		DoGetTexture( p_pass, p_program, TextureChannel::eOpacity, ShaderProgram::MapOpacity, *this );
		DoGetTexture( p_pass, p_program, TextureChannel::eGloss, ShaderProgram::MapGloss, *this );
		DoGetTexture( p_pass, p_program, TextureChannel::eHeight, ShaderProgram::MapHeight, *this );
		DoGetTexture( p_pass, p_program, TextureChannel::eNormal, ShaderProgram::MapNormal, *this );
	}
}
