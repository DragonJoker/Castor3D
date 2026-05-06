#include "Castor3D/Shader/Ubos/SkinningUbo.hpp"

#include "Castor3D/Limits.hpp"

#include <ShaderWriter/Source.hpp>

namespace c3d
{
	//*********************************************************************************************

	MbString const SkinningUbo::BufferSkinning = "C3D_Skinning";

	shader::SkinningData SkinningUbo::declare( sdw::ShaderWriter & writer
		, uint32_t transformsBinding
		, uint32_t transformsSet
		, bool hasSkin )
	{
		return shader::SkinningData{ writer.declArrayStorageBuffer< sdw::Mat4x4 >( SkinningUbo::BufferSkinning
			, transformsBinding
			, transformsSet
			, ast::type::MemoryLayout::eStd430
			, hasSkin ) };
	}

	sdw::Mat4 SkinningUbo::computeTransform( shader::SkinningData const & data
		, sdw::Mat4 const & transform
		, sdw::ShaderWriter & writer
		, sdw::UInt const & skinningId
		, sdw::UVec4 const & boneIds0
		, sdw::UVec4 const & boneIds1
		, sdw::Vec4 const & boneWeights0
		, sdw::Vec4 const & boneWeights1 )
	{
		auto mtxBoneTransform = writer.declLocale< sdw::Mat4 >( "mtxBoneTransform" );
		auto mtxInstanceOffset = writer.declLocale( "mtxInstanceOffset", skinningId * MaxBonesCount );

		auto const & ssbo = data.transforms;
		mtxBoneTransform = ssbo[mtxInstanceOffset + writer.cast< sdw::UInt >( boneIds0[0_i] )] * boneWeights0[0_i];
		mtxBoneTransform += ssbo[mtxInstanceOffset + writer.cast< sdw::UInt >( boneIds0[1_i] )] * boneWeights0[1_i];
		mtxBoneTransform += ssbo[mtxInstanceOffset + writer.cast< sdw::UInt >( boneIds0[2_i] )] * boneWeights0[2_i];
		mtxBoneTransform += ssbo[mtxInstanceOffset + writer.cast< sdw::UInt >( boneIds0[3_i] )] * boneWeights0[3_i];
		mtxBoneTransform += ssbo[mtxInstanceOffset + writer.cast< sdw::UInt >( boneIds1[0_i] )] * boneWeights1[0_i];
		mtxBoneTransform += ssbo[mtxInstanceOffset + writer.cast< sdw::UInt >( boneIds1[1_i] )] * boneWeights1[1_i];
		mtxBoneTransform += ssbo[mtxInstanceOffset + writer.cast< sdw::UInt >( boneIds1[2_i] )] * boneWeights1[2_i];
		mtxBoneTransform += ssbo[mtxInstanceOffset + writer.cast< sdw::UInt >( boneIds1[3_i] )] * boneWeights1[3_i];
		mtxBoneTransform = transform * mtxBoneTransform;

		return mtxBoneTransform;
	}
}
