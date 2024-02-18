/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ModelIndexUbo_H___
#define ___C3D_ModelIndexUbo_H___

#include "UbosModule.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"

#include <ShaderWriter/CompositeTypes/StructInstanceHelper.hpp>
#include <ShaderWriter/MatTypes/Mat4.hpp>

namespace castor3d::shader
{
	struct ModelData
		: public sdw::StructInstanceHelperT< "C3D_ModelData"
			, sdw::type::MemoryLayout::eStd140
			, sdw::Mat4Field< "prvMtxModel" >
			, sdw::Mat4Field< "curMtxModel" >
			, sdw::Mat4Field< "mtxNormal" >
			, sdw::UIntField< "materialId" >
			, sdw::UIntField< "shadowReceiver" >
			, sdw::UIntField< "envMapId" >
			, sdw::UIntField< "vertexOffset" >
			, sdw::Vec3Field< "scale" >
			, sdw::UIntField< "meshletCount" >
			, sdw::UIntField< "indexOffset" >
			, sdw::UIntField< "meshletOffset" >
			, sdw::UVec2Field< "pad" > >
	{
		ModelData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, castor::move( expr ), enabled }
		{
		}

		C3D_API sdw::Mat4 getPrvModelMtx( PipelineFlags const & flags
			, sdw::Mat4 const & curModelMatrix )const;
		C3D_API sdw::Mat3 getNormalMtx( bool hasSkin
			, sdw::Mat4 const & curModelMatrix )const;
		C3D_API sdw::Mat3 getNormalMtx( PipelineFlags const & flags
			, sdw::Mat4 const & curModelMatrix )const;
		C3D_API sdw::Vec4 worldToModel( sdw::Vec4 const & pos )const;
		C3D_API DerivVec4 worldToModel( DerivVec4 const & pos )const;
		C3D_API sdw::Vec4 modelToWorld( sdw::Vec4 const & pos )const;
		C3D_API sdw::Vec4 modelToCurWorld( sdw::Vec4 const & pos )const;
		C3D_API sdw::Vec4 modelToPrvWorld( sdw::Vec4 const & pos )const;
		C3D_API sdw::Mat4 getCurModelMtx( PipelineFlags const & flags
			, sdw::Mat4 const & transform )const;
		C3D_API sdw::Mat4 getCurModelMtx( SkinningData const & skinning
			, sdw::UInt const & skinningId
			, sdw::UVec4 const & boneIds0
			, sdw::UVec4 const & boneIds1
			, sdw::Vec4 const & boneWeights0
			, sdw::Vec4 const & boneWeights1 )const;

		sdw::UInt getMaterialId()const
		{
			return getMember< "materialId" >();
		}

		sdw::UInt isShadowReceiver()const
		{
			return getMember< "shadowReceiver" >();
		}

		sdw::UInt getEnvMapIndex()const
		{
			return getMember< "envMapId" >();
		}

		sdw::Mat4 getModelMtx()const
		{
			return getMember< "curMtxModel" >();
		}

		sdw::Vec3 getScale()const
		{
			return getMember< "scale" >();
		}

		sdw::UInt getMeshletCount()const
		{
			return getMember< "meshletCount" >();
		}

		sdw::UInt getVertexOffset()const
		{
			return getMember< "vertexOffset" >();
		}

		sdw::UInt getIndexOffset()const
		{
			return getMember< "indexOffset" >();
		}

	private:
		sdw::Mat4 prvMtxModel()const
		{
			return getMember< "prvMtxModel" >();
		}

		sdw::Mat4 mtxNormal()const
		{
			return getMember< "mtxNormal" >();
		}
	};
}

#define C3D_ModelsData( writer, binding, set )\
	sdw::StorageBuffer c3d_modelsDataBuffer{ writer\
		, "C3D_ModelsDataBuffer"\
		, "c3d_modelsDataBuffer"\
		, uint32_t( binding )\
		, uint32_t( set )\
		, ast::type::MemoryLayout::eStd430\
		, true };\
	auto c3d_modelsData = c3d_modelsDataBuffer.declMemberArray< castor3d::shader::ModelData >( "d" );\
	c3d_modelsDataBuffer.end()

#define C3D_ModelData( writer, binding, set )\
	sdw::UniformBuffer c3d_modelDataBuffer{ writer\
		, "C3D_ModelDataBuffer"\
		, "c3d_modelDataBuffer"\
		, uint32_t( binding )\
		, uint32_t( set )\
		, ast::type::MemoryLayout::eStd140\
		, true };\
	auto c3d_modelData = c3d_modelDataBuffer.declMember< castor3d::shader::ModelData >( "d" );\
	c3d_modelDataBuffer.end()

#endif
