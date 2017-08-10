#include "SkinningUbo.hpp"

#include "Engine.hpp"
#include "Render/RenderPipeline.hpp"
#include "Scene/Animation/AnimatedSkeleton.hpp"
#include "Shader/ShaderProgram.hpp"

#include <GlslSource.hpp>

using namespace castor;

namespace castor3d
{
	SkinningUbo::SkinningUbo( Engine & engine )
		: m_ubo{ ShaderProgram::BufferSkinning
			, *engine.getRenderSystem()
			, SkinningUbo::BindingPoint }
		, m_bonesMatrix{ *m_ubo.createUniform< UniformType::eMat4x4f >( ShaderProgram::Bones, 400u ) }
	{
	}

	SkinningUbo::~SkinningUbo()
	{
	}

	void SkinningUbo::update( AnimatedSkeleton const & p_skeleton )const
	{
		p_skeleton.fillShader( m_bonesMatrix );
		m_ubo.update();
		m_ubo.bindTo( SkinningUbo::BindingPoint );
	}

	void SkinningUbo::declare( GLSL::GlslWriter & p_writer
		, ProgramFlags const & p_flags )
	{
		if ( checkFlag( p_flags, ProgramFlag::eSkinning ) )
		{
			if ( checkFlag( p_flags, ProgramFlag::eInstantiation ) )
			{
				GLSL::Ssbo skinning{ p_writer, ShaderProgram::BufferSkinning, SkinningUbo::BindingPoint };
				auto c3d_mtxBones = skinning.declMemberArray< GLSL::Mat4 >( ShaderProgram::Bones, checkFlag( p_flags, ProgramFlag::eSkinning ) );
				skinning.end();
			}
			else
			{
				GLSL::Ubo skinning{ p_writer, ShaderProgram::BufferSkinning, SkinningUbo::BindingPoint };
				auto c3d_mtxBones = skinning.declMember< GLSL::Mat4 >( ShaderProgram::Bones, 400, checkFlag( p_flags, ProgramFlag::eSkinning ) );
				skinning.end();
			}
		}
	}

	GLSL::Mat4 SkinningUbo::computeTransform( GLSL::GlslWriter & p_writer
		, ProgramFlags const & p_flags )
	{
		using namespace GLSL;
		auto bone_ids0 = p_writer.getBuiltin< IVec4 >( ShaderProgram::BoneIds0 );
		auto bone_ids1 = p_writer.getBuiltin< IVec4 >( ShaderProgram::BoneIds1 );
		auto weights0 = p_writer.getBuiltin< Vec4 >( ShaderProgram::Weights0 );
		auto weights1 = p_writer.getBuiltin< Vec4 >( ShaderProgram::Weights1 );
		auto c3d_mtxModel = p_writer.getBuiltin< GLSL::Mat4 >( RenderPipeline::MtxModel );
		auto c3d_mtxBones = p_writer.getBuiltinArray< GLSL::Mat4 >( ShaderProgram::Bones );
		auto mtxBoneTransform = p_writer.declLocale< Mat4 >( cuT( "mtxBoneTransform" ) );

		if ( checkFlag( p_flags, ProgramFlag::eInstantiation ) )
		{
			auto gl_InstanceID = p_writer.getBuiltin< GLSL::Int >( cuT( "gl_InstanceID" ) );
			auto transform = p_writer.getBuiltin< GLSL::Mat4 >( cuT( "transform" ) );
			auto mtxInstanceOffset = p_writer.declLocale< Int >( cuT( "mtxInstanceOffset" )
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
