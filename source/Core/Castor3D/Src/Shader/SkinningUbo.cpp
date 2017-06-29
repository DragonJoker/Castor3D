#include "SkinningUbo.hpp"

#include "Engine.hpp"
#include "Render/RenderPipeline.hpp"
#include "Scene/Animation/AnimatedSkeleton.hpp"
#include "Shader/ShaderProgram.hpp"

#include <GlslSource.hpp>

using namespace Castor;

namespace Castor3D
{
	SkinningUbo::SkinningUbo( Engine & p_engine )
		: m_ubo{ ShaderProgram::BufferSkinning, *p_engine.GetRenderSystem() }
		, m_bonesMatrix{ *m_ubo.CreateUniform< UniformType::eMat4x4f >( ShaderProgram::Bones, 400u ) }
	{
	}

	SkinningUbo::~SkinningUbo()
	{
	}

	void SkinningUbo::Update( AnimatedSkeleton const & p_skeleton )const
	{
		p_skeleton.FillShader( m_bonesMatrix );
		m_ubo.Update();
	}

	void SkinningUbo::Declare( GLSL::GlslWriter & p_writer
		, ProgramFlags const & p_flags )
	{
		if ( CheckFlag( p_flags, ProgramFlag::eInstantiation ) )
		{
			GLSL::Ssbo l_skinning{ p_writer, ShaderProgram::BufferSkinning };
			auto c3d_mtxBones = l_skinning.GetUniform< GLSL::Mat4 >( ShaderProgram::Bones, 400, CheckFlag( p_flags, ProgramFlag::eSkinning ) );
			l_skinning.End();
		}
		else
		{
			GLSL::Ubo l_skinning{ p_writer, ShaderProgram::BufferSkinning };
			auto c3d_mtxBones = l_skinning.GetUniform< GLSL::Mat4 >( ShaderProgram::Bones, 400, CheckFlag( p_flags, ProgramFlag::eSkinning ) );
			l_skinning.End();
		}
	}

	GLSL::Mat4 SkinningUbo::ComputeTransform( GLSL::GlslWriter & p_writer
		, ProgramFlags const & p_flags )
	{
		using namespace GLSL;
		auto l_mtxBoneTransform = p_writer.DeclLocale< Mat4 >( cuT( "l_mtxBoneTransform" ) );
		l_mtxBoneTransform = c3d_mtxBones[bone_ids0[0_i]] * weights0[0_i];
		l_mtxBoneTransform += c3d_mtxBones[bone_ids0[1_i]] * weights0[1_i];
		l_mtxBoneTransform += c3d_mtxBones[bone_ids0[2_i]] * weights0[2_i];
		l_mtxBoneTransform += c3d_mtxBones[bone_ids0[3_i]] * weights0[3_i];
		l_mtxBoneTransform += c3d_mtxBones[bone_ids1[0_i]] * weights1[0_i];
		l_mtxBoneTransform += c3d_mtxBones[bone_ids1[1_i]] * weights1[1_i];
		l_mtxBoneTransform += c3d_mtxBones[bone_ids1[2_i]] * weights1[2_i];
		l_mtxBoneTransform += c3d_mtxBones[bone_ids1[3_i]] * weights1[3_i];
		l_mtxModel = c3d_mtxModel * l_mtxBoneTransform;
	}
}
