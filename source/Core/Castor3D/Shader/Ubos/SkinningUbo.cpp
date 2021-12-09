#include "Castor3D/Shader/Ubos/SkinningUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Model/Skeleton/Skeleton.hpp"
#include "Castor3D/Scene/Animation/AnimatedSkeleton.hpp"
#include "Castor3D/Shader/Ubos/ModelUbo.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d
{
	castor::String const SkinningUbo::BufferSkinning = cuT( "Skinning" );
	castor::String const SkinningUbo::Bones = cuT( "c3d_mtxBones" );

	shader::SkinningData SkinningUbo::declare( sdw::ShaderWriter & writer
		, uint32_t uboBinding
		, uint32_t sboBinding
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
					, sboBinding
					, set
					, true );
			}
			else
			{
				result.ubo = std::make_unique< sdw::Ubo >( writer
					, SkinningUbo::BufferSkinning
					, uboBinding
					, set );
				result.ubo->declMember< sdw::Mat4 >( SkinningUbo::Bones
					, 400
					, checkFlag( flags, ProgramFlag::eSkinning ) );
				result.ubo->end();
			}
		}

		return result;
	}

	sdw::Mat4 SkinningUbo::computeTransform( shader::SkinningData const & data
		, sdw::IVec4 boneIds0
		, sdw::IVec4 boneIds1
		, sdw::Vec4 boneWeights0
		, sdw::Vec4 boneWeights1
		, sdw::Mat4 transform
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
			mtxBoneTransform = ssbo[writer.cast< UInt >( mtxInstanceOffset + boneIds0[0_i] )] * boneWeights0[0_i];
			mtxBoneTransform += ssbo[writer.cast< UInt >( mtxInstanceOffset + boneIds0[1_i] )] * boneWeights0[1_i];
			mtxBoneTransform += ssbo[writer.cast< UInt >( mtxInstanceOffset + boneIds0[2_i] )] * boneWeights0[2_i];
			mtxBoneTransform += ssbo[writer.cast< UInt >( mtxInstanceOffset + boneIds0[3_i] )] * boneWeights0[3_i];
			mtxBoneTransform += ssbo[writer.cast< UInt >( mtxInstanceOffset + boneIds1[0_i] )] * boneWeights1[0_i];
			mtxBoneTransform += ssbo[writer.cast< UInt >( mtxInstanceOffset + boneIds1[1_i] )] * boneWeights1[1_i];
			mtxBoneTransform += ssbo[writer.cast< UInt >( mtxInstanceOffset + boneIds1[2_i] )] * boneWeights1[2_i];
			mtxBoneTransform += ssbo[writer.cast< UInt >( mtxInstanceOffset + boneIds1[3_i] )] * boneWeights1[3_i];
			mtxBoneTransform = transform * mtxBoneTransform;
		}
		else
		{
			auto bones = data.ubo->getMemberArray< Mat4 >( SkinningUbo::Bones );
			mtxBoneTransform = bones[boneIds0[0]] * boneWeights0[0];
			mtxBoneTransform += bones[boneIds0[1]] * boneWeights0[1];
			mtxBoneTransform += bones[boneIds0[2]] * boneWeights0[2];
			mtxBoneTransform += bones[boneIds0[3]] * boneWeights0[3];
			mtxBoneTransform += bones[boneIds1[0]] * boneWeights1[0];
			mtxBoneTransform += bones[boneIds1[1]] * boneWeights1[1];
			mtxBoneTransform += bones[boneIds1[2]] * boneWeights1[2];
			mtxBoneTransform += bones[boneIds1[3]] * boneWeights1[3];
			mtxBoneTransform = curMtxModel * mtxBoneTransform;
		}

		return mtxBoneTransform;
	}
}
