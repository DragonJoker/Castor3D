/*
See LICENSE file in root folder
*/
#ifndef ___C3DSDW_ModelData_H___
#define ___C3DSDW_ModelData_H___

#include "Castor3D/Shader/Shaders/GlslSurface.hpp"

#include <ShaderWriter/CompositeTypes/StructInstance.hpp>
#include <ShaderWriter/MatTypes/Mat4.hpp>

namespace castor3d::shader
{
	struct ModelBufferData
		: public sdw::StructInstance
	{
		C3D_API ModelBufferData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled );
		SDW_DeclStructInstance( C3D_API, ModelBufferData );

		C3D_API static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache );

		C3D_API sdw::Mat4 getPrvModelMtx( ProgramFlags programFlags
			, sdw::Mat4 const & curModelMatrix )const;
		C3D_API sdw::Mat3 getNormalMtx( ProgramFlags programFlags
			, sdw::Mat4 const & curModelMatrix )const;
		C3D_API sdw::Vec4 worldToModel( sdw::Vec4 const & pos )const;
		C3D_API sdw::Vec4 modelToWorld( sdw::Vec4 const & pos )const;
		C3D_API sdw::Vec4 modelToCurWorld( sdw::Vec4 const & pos )const;
		C3D_API sdw::Vec4 modelToPrvWorld( sdw::Vec4 const & pos )const;
		C3D_API sdw::Mat4 getCurModelMtx( ProgramFlags programFlags
			, SkinningData const & skinning
			, sdw::IVec4 const & boneIds0
			, sdw::IVec4 const & boneIds1
			, sdw::Vec4 const & boneWeights0
			, sdw::Vec4 const & boneWeights1 )const;

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
				, surface.boneWeights1 );
		}

	public:
		C3D_API static castor::String const BufferName;
		C3D_API static castor::String const DataName;

	private:
		using sdw::StructInstance::getMember;
		using sdw::StructInstance::getMemberArray;

	private:
		sdw::Mat4 m_prvMtxModel;
		sdw::Mat4 m_curMtxModel;
		sdw::Mat4 m_mtxNormal;
		sdw::IVec4 m_shadowEnvMap;
		sdw::Int m_shadowReceiver;
		sdw::Int m_envMapIndex;
	};

	class ModelDatas
	{
	public:
		C3D_API explicit ModelDatas( sdw::ShaderWriter & writer
			, uint32_t binding
			, uint32_t set
			, bool enable = true );
		C3D_API ModelBufferData operator[]( sdw::UInt const & index )const;

	private:
		sdw::ShaderWriter & m_writer;
		std::unique_ptr< sdw::ArraySsboT< ModelBufferData > > m_ssbo;
	};
}

#endif
