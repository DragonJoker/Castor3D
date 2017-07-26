#include "SkinningUbo.hpp"

#include "Engine.hpp"
#include "Render/RenderPipeline.hpp"
#include "Scene/Animation/AnimatedSkeleton.hpp"
#include "Shader/ShaderProgram.hpp"

#include <GlslSource.hpp>

using namespace Castor;

namespace Castor3D
{
	SkinningUbo::SkinningUbo( Engine & engine )
		: m_ubo{ ShaderProgram::BufferSkinning
			, *engine.GetRenderSystem()
			, SkinningUbo::BindingPoint }
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
		m_ubo.BindTo( SkinningUbo::BindingPoint );
	}

	void SkinningUbo::Declare( GLSL::GlslWriter & p_writer
		, ProgramFlags const & p_flags )
	{
		if ( CheckFlag( p_flags, ProgramFlag::eSkinning ) )
		{
			if ( CheckFlag( p_flags, ProgramFlag::eInstantiation ) )
			{
				GLSL::Ssbo skinning{ p_writer, ShaderProgram::BufferSkinning, SkinningUbo::BindingPoint };
				auto c3d_mtxBones = skinning.DeclMemberArray< GLSL::Mat4 >( ShaderProgram::Bones, CheckFlag( p_flags, ProgramFlag::eSkinning ) );
				skinning.End();
			}
			else
			{
				GLSL::Ubo skinning{ p_writer, ShaderProgram::BufferSkinning, SkinningUbo::BindingPoint };
				auto c3d_mtxBones = skinning.DeclMember< GLSL::Mat4 >( ShaderProgram::Bones, 400, CheckFlag( p_flags, ProgramFlag::eSkinning ) );
				skinning.End();
			}
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
		auto mtxBoneTransform = p_writer.DeclLocale< Mat4 >( cuT( "mtxBoneTransform" ) );

		if ( CheckFlag( p_flags, ProgramFlag::eInstantiation ) )
		{
			auto gl_InstanceID = p_writer.GetBuiltin< GLSL::Int >( cuT( "gl_InstanceID" ) );
			auto transform = p_writer.GetBuiltin< GLSL::Mat4 >( cuT( "transform" ) );
			auto mtxInstanceOffset = p_writer.DeclLocale< Int >( cuT( "mtxInstanceOffset" )
				, gl_InstanceID * 400_i );
			mtxBoneTransform = c3d_mtxBones[mtxInstanceOffset + bone_ids0[0_i]] * weights0[0_i];
			mtxBoneTransform += c3d_mtxBones[mtxInstanceOffset + bone_ids0[1_i]] * weights0[1_i];
			mtxBoneTransform += c3d_mtxBones[mtxInstanceOffset + bone_ids0[2_i]] * weights0[2_i];
			mtxBoneTransform += c3d_mtxBones[mtxInstanceOffset + bone_ids0[3_i]] * weights0[3_i];
			mtxBoneTransform += c3d_mtxBones[mtxInstanceOffset + bone_ids1[0_i]] * weights1[0_i];
			mtxBoneTransform += c3d_mtxBones[mtxInstanceOffset + bone_ids1[1_i]] * weights1[1_i];
			mtxBoneTransform += c3d_mtxBones[mtxInstanceOffset + bone_ids1[2_i]] * weights1[2_i];
			mtxBoneTransform += c3d_mtxBones[mtxInstanceOffset + bone_ids1[3_i]] * weights1[3_i];
			mtxBoneTransform = transform * mtxBoneTransform;
		}
		else
		{
			mtxBoneTransform = c3d_mtxBones[bone_ids0[0_i]] * weights0[0_i];
			mtxBoneTransform += c3d_mtxBones[bone_ids0[1_i]] * weights0[1_i];
			mtxBoneTransform += c3d_mtxBones[bone_ids0[2_i]] * weights0[2_i];
			mtxBoneTransform += c3d_mtxBones[bone_ids0[3_i]] * weights0[3_i];
			mtxBoneTransform += c3d_mtxBones[bone_ids1[0_i]] * weights1[0_i];
			mtxBoneTransform += c3d_mtxBones[bone_ids1[1_i]] * weights1[1_i];
			mtxBoneTransform += c3d_mtxBones[bone_ids1[2_i]] * weights1[2_i];
			mtxBoneTransform += c3d_mtxBones[bone_ids1[3_i]] * weights1[3_i];
			mtxBoneTransform = c3d_mtxModel * mtxBoneTransform;
		}

		return mtxBoneTransform;
	}
}
