/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ModelUbo_H___
#define ___C3D_ModelUbo_H___

#include "SkinningUbo.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"

#include <ShaderWriter/CompositeTypes/StructInstance.hpp>
#include <ShaderWriter/MatTypes/Mat4.hpp>

namespace castor3d
{
	namespace shader
	{
		struct ModelData
			: public sdw::StructInstance
		{
			C3D_API ModelData( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled );
			SDW_DeclStructInstance( C3D_API, ModelData );

			C3D_API static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache );
			C3D_API static std::unique_ptr< sdw::Struct > declare( sdw::ShaderWriter & writer );

			C3D_API sdw::Mat4 getPrvModelMtx( ProgramFlags programFlags
				, sdw::Mat4 const & curModelMatrix )const;
			C3D_API sdw::Mat3 getNormalMtx( ProgramFlags programFlags
				, sdw::Mat4 const & curModelMatrix )const;
			C3D_API sdw::UInt getMaterialIndex( ProgramFlags programFlags
				, sdw::Int const & instanceMaterial )const;
			C3D_API sdw::UInt getMaterialIndex()const;
			C3D_API sdw::Int getNodeId( ProgramFlags programFlags
				, sdw::Int const & instanceNodeId )const;
			C3D_API sdw::Int getNodeId()const;
			C3D_API sdw::Vec4 worldToModel( sdw::Vec4 const & pos )const;
			C3D_API sdw::Vec4 modelToWorld( sdw::Vec4 const & pos )const;
			C3D_API sdw::Vec4 modelToCurWorld( sdw::Vec4 const & pos )const;
			C3D_API sdw::Vec4 modelToPrvWorld( sdw::Vec4 const & pos )const;
			C3D_API sdw::Mat4 getCurModelMtx( ProgramFlags programFlags
				, SkinningData const & skinning
				, sdw::IVec4 const & boneIds0
				, sdw::IVec4 const & boneIds1
				, sdw::Vec4 const & boneWeights0
				, sdw::Vec4 const & boneWeights1
				, sdw::Mat4 const & transform )const;

			sdw::Int const & isShadowReceiver()const
			{
				return m_shadowReceiver;
			}

			sdw::Int const & getEnvMapIndex()const
			{
				return m_envMapIndex;
			}

			sdw::Mat4 const & getModelMtx()const
			{
				return m_curMtxModel;
			}

			template< ast::var::Flag FlagT >
			sdw::Mat4 getCurModelMtx( ProgramFlags programFlags
				, SkinningData const & skinning
				, VertexSurfaceT< FlagT > const & surface )const
			{
				return getCurModelMtx( programFlags
					, skinning
					, surface.boneIds0
					, surface.boneIds1
					, surface.boneWeights0
					, surface.boneWeights1
					, surface.transform );
			}

		private:
			using sdw::StructInstance::getMember;
			using sdw::StructInstance::getMemberArray;

		private:
			sdw::Mat4 m_prvMtxModel;
			sdw::Mat4 m_curMtxModel;
			sdw::Mat4 m_mtxNormal;
			sdw::Int m_shadowReceiver;
			sdw::Int m_materialIndex;
			sdw::Int m_nodeId;
			sdw::Int m_envMapIndex;
		};
	}

	class ModelUbo
	{
	public:
		using Configuration = ModelUboConfiguration;

	public:
		C3D_API static castor::String const BufferModel;
		C3D_API static castor::String const ModelData;
	};
}

#define UBO_MODEL( writer, binding, set )\
	sdw::Ubo model{ writer\
		, castor3d::ModelUbo::BufferModel\
		, binding\
		, set\
		, ast::type::MemoryLayout::eStd140\
		, true };\
	auto c3d_modelData = model.declMember< castor3d::shader::ModelData >( castor3d::ModelUbo::ModelData );\
	model.end()

#endif
