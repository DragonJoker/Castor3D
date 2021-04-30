#include "Castor3D/Shader/Ubos/SkinningUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Model/Skeleton/Skeleton.hpp"
#include "Castor3D/Scene/Animation/AnimatedSkeleton.hpp"
#include "Castor3D/Shader/Ubos/ModelUbo.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d
{
	castor::String const SkinningUbo::BufferSkinning = cuT( "Skinning" );
	castor::String const SkinningUbo::Bones = cuT( "c3d_mtxBones" );

	shader::SkinningData SkinningUbo::declare( sdw::ShaderWriter & writer
		, uint32_t binding
		, uint32_t set
		, ProgramFlags const & flags )
	{
		shader::SkinningData result;

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

	sdw::Mat4 SkinningUbo::computeTransform( shader::SkinningData const & data
		, shader::VertexSurface const & surface
		, sdw::ShaderWriter & writer
		, ProgramFlags const & flags
		, sdw::Mat4 const & curMtxModel )
	{
		using namespace sdw;
		auto mtxBoneTransform = writer.declLocale< Mat4 >( cuT( "mtxBoneTransform" ) );

		if ( checkFlag( flags, ProgramFlag::eInstantiation ) )
		{
			auto gl_InstanceID = writer.getVariable< Int >( cuT( "gl_InstanceIndex" ) );
			auto mtxInstanceOffset = writer.declLocale( cuT( "mtxInstanceOffset" )
				, gl_InstanceID * 400_i );

			auto & ssbo = *data.ssbo;
			mtxBoneTransform = ssbo[writer.cast< UInt >( mtxInstanceOffset + surface.boneIds0[0_i] )] * surface.boneWeights0[0_i];
			mtxBoneTransform += ssbo[writer.cast< UInt >( mtxInstanceOffset + surface.boneIds0[1_i] )] * surface.boneWeights0[1_i];
			mtxBoneTransform += ssbo[writer.cast< UInt >( mtxInstanceOffset + surface.boneIds0[2_i] )] * surface.boneWeights0[2_i];
			mtxBoneTransform += ssbo[writer.cast< UInt >( mtxInstanceOffset + surface.boneIds0[3_i] )] * surface.boneWeights0[3_i];
			mtxBoneTransform += ssbo[writer.cast< UInt >( mtxInstanceOffset + surface.boneIds1[0_i] )] * surface.boneWeights1[0_i];
			mtxBoneTransform += ssbo[writer.cast< UInt >( mtxInstanceOffset + surface.boneIds1[1_i] )] * surface.boneWeights1[1_i];
			mtxBoneTransform += ssbo[writer.cast< UInt >( mtxInstanceOffset + surface.boneIds1[2_i] )] * surface.boneWeights1[2_i];
			mtxBoneTransform += ssbo[writer.cast< UInt >( mtxInstanceOffset + surface.boneIds1[3_i] )] * surface.boneWeights1[3_i];
			mtxBoneTransform = surface.transform * mtxBoneTransform;
		}
		else
		{
			auto bones = data.ubo->getMemberArray< Mat4 >( SkinningUbo::Bones );
			mtxBoneTransform = bones[surface.boneIds0[0]] * surface.boneWeights0[0];
			mtxBoneTransform += bones[surface.boneIds0[1]] * surface.boneWeights0[1];
			mtxBoneTransform += bones[surface.boneIds0[2]] * surface.boneWeights0[2];
			mtxBoneTransform += bones[surface.boneIds0[3]] * surface.boneWeights0[3];
			mtxBoneTransform += bones[surface.boneIds1[0]] * surface.boneWeights1[0];
			mtxBoneTransform += bones[surface.boneIds1[1]] * surface.boneWeights1[1];
			mtxBoneTransform += bones[surface.boneIds1[2]] * surface.boneWeights1[2];
			mtxBoneTransform += bones[surface.boneIds1[3]] * surface.boneWeights1[3];
			mtxBoneTransform = curMtxModel * mtxBoneTransform;
		}

		return mtxBoneTransform;
	}
}
