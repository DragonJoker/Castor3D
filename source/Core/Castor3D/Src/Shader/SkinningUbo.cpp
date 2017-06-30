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
			auto c3d_mtxBones = l_skinning.DeclMemberArray< GLSL::Mat4 >( ShaderProgram::Bones, CheckFlag( p_flags, ProgramFlag::eSkinning ) );
			l_skinning.End();
		}
		else
		{
			GLSL::Ubo l_skinning{ p_writer, ShaderProgram::BufferSkinning };
			auto c3d_mtxBones = l_skinning.DeclMember< GLSL::Mat4 >( ShaderProgram::Bones, 400, CheckFlag( p_flags, ProgramFlag::eSkinning ) );
			l_skinning.End();
		}
	}

	GLSL::Mat4 SkinningUbo::ComputeTransform( GLSL::GlslWriter & p_writer
		, ProgramFlags const & p_flags )
	{
		using namespace GLSL;
		auto bone_ids0 = p_writer.GetBuiltin< IVec4 >( ShaderProgram::BoneIds0 );
		auto bone_ids1 = p_writer.GetBuiltin< IVec4 >( ShaderProgram::BoneIds1 );
		auto weights0 = p_writer.GetBuiltin< Vec4 >( ShaderProgram::Weights0 );
		auto weights1 = p_writer.GetBuiltin< Vec4 >( ShaderProgram::Weights1 );
		auto c3d_mtxModel = p_writer.GetBuiltin< GLSL::Mat4 >( RenderPipeline::MtxModel );
		auto c3d_mtxBones = p_writer.GetBuiltinArray< GLSL::Mat4 >( ShaderProgram::Bones );
		auto l_mtxBoneTransform = p_writer.DeclLocale< Mat4 >( cuT( "l_mtxBoneTransform" ) );

		if ( CheckFlag( p_flags, ProgramFlag::eInstantiation ) )
		{
			auto gl_InstanceID = p_writer.GetBuiltin< GLSL::Int >( cuT( "gl_InstanceID" ) );
			auto l_mtxInstanceOffset = p_writer.DeclLocale< Int >( cuT( "l_mtxInstanceOffset" )
				, gl_InstanceID * 400_i );
			l_mtxBoneTransform = c3d_mtxBones[l_mtxInstanceOffset + bone_ids0[0_i]] * weights0[0_i];
			l_mtxBoneTransform += c3d_mtxBones[l_mtxInstanceOffset + bone_ids0[1_i]] * weights0[1_i];
			l_mtxBoneTransform += c3d_mtxBones[l_mtxInstanceOffset + bone_ids0[2_i]] * weights0[2_i];
			l_mtxBoneTransform += c3d_mtxBones[l_mtxInstanceOffset + bone_ids0[3_i]] * weights0[3_i];
			l_mtxBoneTransform += c3d_mtxBones[l_mtxInstanceOffset + bone_ids1[0_i]] * weights1[0_i];
			l_mtxBoneTransform += c3d_mtxBones[l_mtxInstanceOffset + bone_ids1[1_i]] * weights1[1_i];
			l_mtxBoneTransform += c3d_mtxBones[l_mtxInstanceOffset + bone_ids1[2_i]] * weights1[2_i];
			l_mtxBoneTransform += c3d_mtxBones[l_mtxInstanceOffset + bone_ids1[3_i]] * weights1[3_i];
		}
		else
		{
			l_mtxBoneTransform = c3d_mtxBones[bone_ids0[0_i]] * weights0[0_i];
			l_mtxBoneTransform += c3d_mtxBones[bone_ids0[1_i]] * weights0[1_i];
			l_mtxBoneTransform += c3d_mtxBones[bone_ids0[2_i]] * weights0[2_i];
			l_mtxBoneTransform += c3d_mtxBones[bone_ids0[3_i]] * weights0[3_i];
			l_mtxBoneTransform += c3d_mtxBones[bone_ids1[0_i]] * weights1[0_i];
			l_mtxBoneTransform += c3d_mtxBones[bone_ids1[1_i]] * weights1[1_i];
			l_mtxBoneTransform += c3d_mtxBones[bone_ids1[2_i]] * weights1[2_i];
			l_mtxBoneTransform += c3d_mtxBones[bone_ids1[3_i]] * weights1[3_i];
		}

		return c3d_mtxModel * l_mtxBoneTransform;
	}
}
