#include "SkinningUbo.hpp"

#include "Engine.hpp"
#include "Render/RenderPipeline.hpp"
#include "Scene/Animation/AnimatedSkeleton.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Shader/Ubos/ModelMatrixUbo.hpp"

#include <GlslSource.hpp>

using namespace castor;

namespace castor3d
{
	String const SkinningUbo::BufferSkinning = cuT( "Skinning" );
	String const SkinningUbo::Bones = cuT( "c3d_mtxBones" );

	SkinningUbo::SkinningUbo( Engine & engine )
		: m_ubo{ SkinningUbo::BufferSkinning
			, *engine.getRenderSystem()
			, SkinningUbo::BindingPoint }
		, m_bonesMatrix{ *m_ubo.createUniform< UniformType::eMat4x4f >( SkinningUbo::Bones, 400u ) }
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

	void SkinningUbo::declare( glsl::GlslWriter & p_writer
		, ProgramFlags const & p_flags )
	{
		if ( checkFlag( p_flags, ProgramFlag::eSkinning ) )
		{
			if ( checkFlag( p_flags, ProgramFlag::eInstantiation ) )
			{
				glsl::Ssbo skinning{ p_writer, SkinningUbo::BufferSkinning, SkinningUbo::BindingPoint };
				auto c3d_mtxBones = skinning.declMemberArray< glsl::Mat4 >( SkinningUbo::Bones, checkFlag( p_flags, ProgramFlag::eSkinning ) );
				skinning.end();
			}
			else
			{
				glsl::Ubo skinning{ p_writer, SkinningUbo::BufferSkinning, SkinningUbo::BindingPoint };
				auto c3d_mtxBones = skinning.declMember< glsl::Mat4 >( SkinningUbo::Bones, 400, checkFlag( p_flags, ProgramFlag::eSkinning ) );
				skinning.end();
			}
		}
	}

	glsl::Mat4 SkinningUbo::computeTransform( glsl::GlslWriter & p_writer
		, ProgramFlags const & p_flags )
	{
		using namespace glsl;
		auto bone_ids0 = p_writer.getBuiltin< IVec4 >( ShaderProgram::BoneIds0 );
		auto bone_ids1 = p_writer.getBuiltin< IVec4 >( ShaderProgram::BoneIds1 );
		auto weights0 = p_writer.getBuiltin< Vec4 >( ShaderProgram::Weights0 );
		auto weights1 = p_writer.getBuiltin< Vec4 >( ShaderProgram::Weights1 );
		auto c3d_mtxModel = p_writer.getBuiltin< glsl::Mat4 >( ModelMatrixUbo::MtxModel );
		auto c3d_mtxBones = p_writer.getBuiltinArray< glsl::Mat4 >( SkinningUbo::Bones );
		auto mtxBoneTransform = p_writer.declLocale< Mat4 >( cuT( "mtxBoneTransform" ) );

		if ( checkFlag( p_flags, ProgramFlag::eInstantiation ) )
		{
			auto gl_InstanceID = p_writer.getBuiltin< glsl::Int >( cuT( "gl_InstanceID" ) );
			auto transform = p_writer.getBuiltin< glsl::Mat4 >( cuT( "transform" ) );
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
