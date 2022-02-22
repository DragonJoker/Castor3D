#include "Castor3D/Shader/Ubos/SkinningUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Model/Skeleton/Skeleton.hpp"
#include "Castor3D/Scene/Animation/AnimatedSkeleton.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d
{
	//*********************************************************************************************

	castor::String const SkinningUbo::BufferSkinning = cuT( "C3D_Skinning" );

	shader::SkinningData SkinningUbo::declare( sdw::ShaderWriter & writer
		, uint32_t transformsBinding
		, uint32_t transformsSet
		, ProgramFlags const & flags )
	{
		shader::SkinningData result;

		if ( checkFlag( flags, ProgramFlag::eSkinning ) )
		{
			result.transforms = std::make_unique< sdw::ArraySsboT< sdw::Mat4 > >( writer
				, SkinningUbo::BufferSkinning
				, writer.getTypesCache().getMat4x4F()
				, ast::type::MemoryLayout::eStd430
				, transformsBinding
				, transformsSet
				, true );
		}

		return result;
	}

	sdw::Mat4 SkinningUbo::computeTransform( shader::SkinningData const & data
		, sdw::Mat4 transform
		, sdw::ShaderWriter & writer
		, ProgramFlags const & flags
		, sdw::Mat4 const & curMtxModel
		, sdw::UInt const & skinningId
		, sdw::UVec4 const & boneIds0
		, sdw::UVec4 const & boneIds1
		, sdw::Vec4 const & boneWeights0
		, sdw::Vec4 const & boneWeights1 )
	{
		using namespace sdw;
		auto mtxBoneTransform = writer.declLocale< Mat4 >( "mtxBoneTransform" );
		auto mtxInstanceOffset = writer.declLocale( "mtxInstanceOffset", skinningId * 400_u );

		auto & ssbo = *data.transforms;
		mtxBoneTransform = ssbo[mtxInstanceOffset + writer.cast< UInt >( boneIds0[0_i] )] * boneWeights0[0_i];
		mtxBoneTransform += ssbo[mtxInstanceOffset + writer.cast< UInt >( boneIds0[1_i] )] * boneWeights0[1_i];
		mtxBoneTransform += ssbo[mtxInstanceOffset + writer.cast< UInt >( boneIds0[2_i] )] * boneWeights0[2_i];
		mtxBoneTransform += ssbo[mtxInstanceOffset + writer.cast< UInt >( boneIds0[3_i] )] * boneWeights0[3_i];
		mtxBoneTransform += ssbo[mtxInstanceOffset + writer.cast< UInt >( boneIds1[0_i] )] * boneWeights1[0_i];
		mtxBoneTransform += ssbo[mtxInstanceOffset + writer.cast< UInt >( boneIds1[1_i] )] * boneWeights1[1_i];
		mtxBoneTransform += ssbo[mtxInstanceOffset + writer.cast< UInt >( boneIds1[2_i] )] * boneWeights1[2_i];
		mtxBoneTransform += ssbo[mtxInstanceOffset + writer.cast< UInt >( boneIds1[3_i] )] * boneWeights1[3_i];
		mtxBoneTransform = transform * mtxBoneTransform;

		return mtxBoneTransform;
	}
}
