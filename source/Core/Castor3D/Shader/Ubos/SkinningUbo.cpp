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
					, set
					, true );
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
		auto inBoneIds0 = writer.getVariable< IVec4 >( cuT( "inBoneIds0" ) );
		auto inBoneIds1 = writer.getVariable< IVec4 >( cuT( "inBoneIds1" ) );
		auto inWeights0 = writer.getVariable< Vec4 >( cuT( "inWeights0" ) );
		auto inWeights1 = writer.getVariable< Vec4 >( cuT( "inWeights1" ) );
		auto c3d_curMtxModel = writer.getVariable< Mat4 >( ModelMatrixUbo::CurMtxModel );
		auto mtxBoneTransform = writer.declLocale< Mat4 >( cuT( "mtxBoneTransform" ) );

		if ( checkFlag( flags, ProgramFlag::eInstantiation ) )
		{
			auto gl_InstanceID = writer.getVariable< Int >( cuT( "gl_InstanceIndex" ) );
			auto transform = writer.getVariable< Mat4 >( cuT( "inTransform" ) );
			auto mtxInstanceOffset = writer.declLocale( cuT( "mtxInstanceOffset" )
				, gl_InstanceID * 400_i );

			auto & ssbo = *data.ssbo;
			mtxBoneTransform = ssbo[writer.cast< UInt >( mtxInstanceOffset + inBoneIds0[0_i] )] * inWeights0[0_i];
			mtxBoneTransform += ssbo[writer.cast< UInt >( mtxInstanceOffset + inBoneIds0[1_i] )] * inWeights0[1_i];
			mtxBoneTransform += ssbo[writer.cast< UInt >( mtxInstanceOffset + inBoneIds0[2_i] )] * inWeights0[2_i];
			mtxBoneTransform += ssbo[writer.cast< UInt >( mtxInstanceOffset + inBoneIds0[3_i] )] * inWeights0[3_i];
			mtxBoneTransform += ssbo[writer.cast< UInt >( mtxInstanceOffset + inBoneIds1[0_i] )] * inWeights1[0_i];
			mtxBoneTransform += ssbo[writer.cast< UInt >( mtxInstanceOffset + inBoneIds1[1_i] )] * inWeights1[1_i];
			mtxBoneTransform += ssbo[writer.cast< UInt >( mtxInstanceOffset + inBoneIds1[2_i] )] * inWeights1[2_i];
			mtxBoneTransform += ssbo[writer.cast< UInt >( mtxInstanceOffset + inBoneIds1[3_i] )] * inWeights1[3_i];
			mtxBoneTransform = transform * mtxBoneTransform;
		}
		else
		{
			auto bones = data.ubo->getMemberArray< Mat4 >( SkinningUbo::Bones );
			mtxBoneTransform = bones[inBoneIds0[0]] * inWeights0[0];
			mtxBoneTransform += bones[inBoneIds0[1]] * inWeights0[1];
			mtxBoneTransform += bones[inBoneIds0[2]] * inWeights0[2];
			mtxBoneTransform += bones[inBoneIds0[3]] * inWeights0[3];
			mtxBoneTransform += bones[inBoneIds1[0]] * inWeights1[0];
			mtxBoneTransform += bones[inBoneIds1[1]] * inWeights1[1];
			mtxBoneTransform += bones[inBoneIds1[2]] * inWeights1[2];
			mtxBoneTransform += bones[inBoneIds1[3]] * inWeights1[3];
			mtxBoneTransform = c3d_curMtxModel * mtxBoneTransform;
		}

		return mtxBoneTransform;
	}
}
