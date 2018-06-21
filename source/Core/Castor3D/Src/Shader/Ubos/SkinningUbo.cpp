#include "SkinningUbo.hpp"

#include "Engine.hpp"
#include "Render/RenderPipeline.hpp"
#include "Scene/Animation/AnimatedSkeleton.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Shader/Ubos/ModelMatrixUbo.hpp"

#include <Descriptor/DescriptorSetLayoutBinding.hpp>

#include <GlslSource.hpp>

using namespace castor;

namespace castor3d
{
	uint32_t const SkinningUbo::BindingPoint = 6u;
	String const SkinningUbo::BufferSkinning = cuT( "Skinning" );
	String const SkinningUbo::Bones = cuT( "c3d_mtxBones" );

	SkinningUbo::SkinningUbo( Engine & engine )
		: m_engine{ engine }
	{
		if ( engine.getRenderSystem()->hasCurrentDevice() )
		{
			initialise();
		}
	}

	SkinningUbo::~SkinningUbo()
	{
	}

	void SkinningUbo::initialise()
	{
		if ( !m_ubo )
		{
			auto & device = getCurrentDevice( m_engine );
			m_ubo = renderer::makeUniformBuffer< Configuration >( device
				, 1u
				, renderer::BufferTarget::eTransferDst
				, renderer::MemoryPropertyFlag::eHostVisible );
		}
	}

	void SkinningUbo::cleanup()
	{
		m_ubo.reset();
	}

	void SkinningUbo::update( AnimatedSkeleton const & skeleton )const
	{
		REQUIRE( m_ubo );
		skeleton.fillShader( m_ubo->getData( 0u ).bonesMatrix );
		m_ubo->upload();
	}

	void SkinningUbo::declare( glsl::GlslWriter & writer
		, uint32_t binding
		, uint32_t set
		, ProgramFlags const & flags )
	{
		if ( checkFlag( flags, ProgramFlag::eSkinning ) )
		{
			if ( checkFlag( flags, ProgramFlag::eInstantiation ) )
			{
				glsl::Ssbo skinning{ writer, SkinningUbo::BufferSkinning, binding, set };
				auto c3d_mtxBones = skinning.declMemberArray< glsl::Mat4 >( SkinningUbo::Bones, checkFlag( flags, ProgramFlag::eSkinning ) );
				skinning.end();
			}
			else
			{
				glsl::Ubo skinning{ writer, SkinningUbo::BufferSkinning, binding, set };
				auto c3d_mtxBones = skinning.declMember< glsl::Mat4 >( SkinningUbo::Bones, 400, checkFlag( flags, ProgramFlag::eSkinning ) );
				skinning.end();
			}
		}
	}

	renderer::DescriptorSetLayoutBinding SkinningUbo::createLayoutBinding( uint32_t binding
		, ProgramFlags const & flags )
	{
		REQUIRE( checkFlag( flags, ProgramFlag::eSkinning ) );

		if ( checkFlag( flags, ProgramFlag::eInstantiation ) )
		{
			return renderer::DescriptorSetLayoutBinding{ binding, renderer::DescriptorType::eStorageBuffer, renderer::ShaderStageFlag::eVertex };
		}

		return renderer::DescriptorSetLayoutBinding{ binding, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eVertex };
	}

	glsl::Mat4 SkinningUbo::computeTransform( glsl::GlslWriter & writer
		, ProgramFlags const & flags )
	{
		using namespace glsl;
		auto bone_ids0 = writer.getBuiltin< IVec4 >( cuT( "bone_ids0" ) );
		auto bone_ids1 = writer.getBuiltin< IVec4 >( cuT( "bone_ids1" ) );
		auto weights0 = writer.getBuiltin< Vec4 >( cuT( "weights0" ) );
		auto weights1 = writer.getBuiltin< Vec4 >( cuT( "weights1" ) );
		auto c3d_curMtxModel = writer.getBuiltin< glsl::Mat4 >( ModelMatrixUbo::CurMtxModel );
		auto c3d_mtxBones = writer.getBuiltinArray< glsl::Mat4 >( SkinningUbo::Bones );
		auto mtxBoneTransform = writer.declLocale< Mat4 >( cuT( "mtxBoneTransform" ) );

		if ( checkFlag( flags, ProgramFlag::eInstantiation ) )
		{
			auto gl_InstanceID = writer.getBuiltin< glsl::Int >( writer.getInstanceID() );
			auto transform = writer.getBuiltin< glsl::Mat4 >( cuT( "transform" ) );
			auto mtxInstanceOffset = writer.declLocale< Int >( cuT( "mtxInstanceOffset" )
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
			mtxBoneTransform = c3d_curMtxModel * mtxBoneTransform;
		}

		return mtxBoneTransform;
	}
}
