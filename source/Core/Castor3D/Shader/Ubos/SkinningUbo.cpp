#include "Castor3D/Shader/Ubos/SkinningUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Model/Skeleton/Skeleton.hpp"
#include "Castor3D/Scene/Animation/AnimatedSkeleton.hpp"
#include "Castor3D/Shader/Ubos/ModelMatrixUbo.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d
{
	uint32_t const SkinningUbo::BindingPoint = 8u;
	castor::String const SkinningUbo::BufferSkinning = cuT( "Skinning" );
	castor::String const SkinningUbo::Bones = cuT( "c3d_mtxBones" );

	SkinningData SkinningUbo::declare( sdw::ShaderWriter & writer
		, uint32_t binding
		, uint32_t set
		, ProgramFlags const & flags )
	{
		SkinningData result;

		if ( checkFlag( flags, ProgramFlag::eSkinning ) )
		{
			if ( checkFlag( flags, ProgramFlag::eInstantiation ) )
			{
				result.ssbo = std::make_unique< sdw::ArraySsboT< sdw::Mat4 > >( writer
					, SkinningUbo::BufferSkinning
					, writer.getTypesCache().getMat4x4F()
					, ast::type::MemoryLayout::eStd140
					, binding
					, set );
			}
			else
			{
				result.ubo = std::make_unique< sdw::Ubo >( writer
					, SkinningUbo::BufferSkinning
					, binding
					, set );
				result.ubo->declMember< sdw::Mat4 >( SkinningUbo::Bones
					, 400
					, checkFlag( flags, ProgramFlag::eSkinning ) );
				result.ubo->end();
			}
		}

		return result;
	}

	VkDescriptorSetLayoutBinding SkinningUbo::createLayoutBinding( uint32_t binding
		, ProgramFlags const & flags )
	{
		CU_Require( checkFlag( flags, ProgramFlag::eSkinning ) );

		if ( checkFlag( flags, ProgramFlag::eInstantiation ) )
		{
			return makeDescriptorSetLayoutBinding( binding
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, ( checkFlag( flags, ProgramFlag::eHasGeometry )
					? VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_VERTEX_BIT
					: VK_SHADER_STAGE_VERTEX_BIT ) );
		}

		return makeDescriptorSetLayoutBinding( binding
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, ( checkFlag( flags, ProgramFlag::eHasGeometry )
				? VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_VERTEX_BIT
				: VK_SHADER_STAGE_VERTEX_BIT ) );
	}

	sdw::Mat4 SkinningUbo::computeTransform( SkinningData & data
		, sdw::ShaderWriter & writer
		, ProgramFlags const & flags )
	{
		using namespace sdw;
		auto bone_ids0 = writer.getVariable< IVec4 >( cuT( "bone_ids0" ) );
		auto bone_ids1 = writer.getVariable< IVec4 >( cuT( "bone_ids1" ) );
		auto weights0 = writer.getVariable< Vec4 >( cuT( "weights0" ) );
		auto weights1 = writer.getVariable< Vec4 >( cuT( "weights1" ) );
		auto c3d_curMtxModel = writer.getVariable< Mat4 >( ModelMatrixUbo::CurMtxModel );
		auto mtxBoneTransform = writer.declLocale< Mat4 >( cuT( "mtxBoneTransform" ) );

		if ( checkFlag( flags, ProgramFlag::eInstantiation ) )
		{
			auto gl_InstanceID = writer.getVariable< Int >( cuT( "gl_InstanceIndex" ) );
			auto transform = writer.getVariable< Mat4 >( cuT( "transform" ) );
			auto mtxInstanceOffset = writer.declLocale( cuT( "mtxInstanceOffset" )
				, gl_InstanceID * 400_i );

			auto & ssbo = *data.ssbo;
			mtxBoneTransform = ssbo[writer.cast< UInt >( mtxInstanceOffset + bone_ids0[0_i] )] * weights0[0_i];
			mtxBoneTransform += ssbo[writer.cast< UInt >( mtxInstanceOffset + bone_ids0[1_i] )] * weights0[1_i];
			mtxBoneTransform += ssbo[writer.cast< UInt >( mtxInstanceOffset + bone_ids0[2_i] )] * weights0[2_i];
			mtxBoneTransform += ssbo[writer.cast< UInt >( mtxInstanceOffset + bone_ids0[3_i] )] * weights0[3_i];
			mtxBoneTransform += ssbo[writer.cast< UInt >( mtxInstanceOffset + bone_ids1[0_i] )] * weights1[0_i];
			mtxBoneTransform += ssbo[writer.cast< UInt >( mtxInstanceOffset + bone_ids1[1_i] )] * weights1[1_i];
			mtxBoneTransform += ssbo[writer.cast< UInt >( mtxInstanceOffset + bone_ids1[2_i] )] * weights1[2_i];
			mtxBoneTransform += ssbo[writer.cast< UInt >( mtxInstanceOffset + bone_ids1[3_i] )] * weights1[3_i];
			mtxBoneTransform = transform * mtxBoneTransform;
		}
		else
		{
			auto bones = data.ubo->getMemberArray< Mat4 >( SkinningUbo::Bones );
			mtxBoneTransform = bones[bone_ids0[0]] * weights0[0];
			mtxBoneTransform += bones[bone_ids0[1]] * weights0[1];
			mtxBoneTransform += bones[bone_ids0[2]] * weights0[2];
			mtxBoneTransform += bones[bone_ids0[3]] * weights0[3];
			mtxBoneTransform += bones[bone_ids1[0]] * weights1[0];
			mtxBoneTransform += bones[bone_ids1[1]] * weights1[1];
			mtxBoneTransform += bones[bone_ids1[2]] * weights1[2];
			mtxBoneTransform += bones[bone_ids1[3]] * weights1[3];
			mtxBoneTransform = c3d_curMtxModel * mtxBoneTransform;
		}

		return mtxBoneTransform;
	}
}
