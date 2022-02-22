#include "Castor3D/Shader/Ubos/SkinningUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Model/Skeleton/Skeleton.hpp"
#include "Castor3D/Scene/Animation/AnimatedSkeleton.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d
{
	//*********************************************************************************************

	castor::String const SkinningBonesData::BufferName = cuT( "C3D_BonesWeightsData" );

	SkinningBonesData::SkinningBonesData( sdw::ShaderWriter & writer
		, ast::expr::ExprPtr expr
		, bool enabled )
		: StructInstance{ writer, std::move( expr ), enabled }
		, boneIds0{ getMember< sdw::IVec4 >( "boneIds0" ) }
		, boneIds1{ getMember< sdw::IVec4 >( "boneIds1" ) }
		, boneWeights0{ getMember< sdw::Vec4 >( "boneWeights0" ) }
		, boneWeights1{ getMember< sdw::Vec4 >( "boneWeights1" ) }
	{
	}

	ast::type::BaseStructPtr SkinningBonesData::makeType( ast::type::TypesCache & cache )
	{
		auto result = cache.getStruct( ast::type::MemoryLayout::eStd430
			, cuT( "C3D_Bones" ) );

		if ( result->empty() )
		{
			result->declMember( "boneIds0", ast::type::Kind::eVec4I );
			result->declMember( "boneIds1", ast::type::Kind::eVec4I );
			result->declMember( "boneWeights0", ast::type::Kind::eVec4F );
			result->declMember( "boneWeights1", ast::type::Kind::eVec4F );
		}

		return result;
	}

	//*********************************************************************************************

	castor::String const SkinningUbo::BufferSkinning = cuT( "C3D_Skinning" );
	castor::String const SkinningUbo::Bones = cuT( "c3d_mtxBones" );

	shader::SkinningData SkinningUbo::declare( sdw::ShaderWriter & writer
		, uint32_t transformsBinding
		, uint32_t bonesBinding
		, uint32_t transformsSet
		, uint32_t bonesSet
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
			result.bones = std::make_unique< sdw::ArraySsboT< SkinningBonesData > >( writer
				, SkinningBonesData::BufferName
				, bonesBinding
				, bonesSet
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
		, sdw::Int const & vertexIndex )
	{
		using namespace sdw;
		auto mtxBoneTransform = writer.declLocale< Mat4 >( "mtxBoneTransform" );
		auto & bonesData = *data.bones;
		auto boneIds0 = writer.declLocale( "boneIds0", bonesData[writer.cast< UInt >( vertexIndex )].boneIds0 );
		auto boneIds1 = writer.declLocale( "boneIds1", bonesData[writer.cast< UInt >( vertexIndex )].boneIds1 );
		auto boneWeights0 = writer.declLocale( "boneWeights0", bonesData[writer.cast< UInt >( vertexIndex )].boneWeights0 );
		auto boneWeights1 = writer.declLocale( "boneWeights1", bonesData[writer.cast< UInt >( vertexIndex )].boneWeights1 );
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
