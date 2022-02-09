#include "Castor3D/Shader/Shaders/GlslModelData.hpp"

#include "Castor3D/Shader/Ubos/SkinningUbo.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d::shader
{
	//*********************************************************************************************

	castor::String const ModelBufferData::BufferName = cuT( "ModelData" );
	castor::String const ModelBufferData::DataName = cuT( "c3d_modelData" );

	ModelBufferData::ModelBufferData( sdw::ShaderWriter & writer
		, ast::expr::ExprPtr expr
		, bool enabled )
		: StructInstance{ writer, std::move( expr ), enabled }
		, m_prvMtxModel{ getMember< sdw::Mat4 >( "prvMtxModel" ) }
		, m_curMtxModel{ getMember< sdw::Mat4 >( "curMtxModel" ) }
		, m_mtxNormal{ getMember< sdw::Mat4 >( "mtxNormal" ) }
		, m_shadowEnvMap{ getMember< sdw::IVec4 >( "shadowEnvMap" ) }
		, m_shadowReceiver{ m_shadowEnvMap.x() }
		, m_envMapIndex{ m_shadowEnvMap.y() }
	{
	}

	ast::type::BaseStructPtr ModelBufferData::makeType( ast::type::TypesCache & cache )
	{
		auto result = cache.getStruct( ast::type::MemoryLayout::eStd430
			, "C3D_" + BufferName );

		if ( result->empty() )
		{
			result->declMember( "prvMtxModel", ast::type::Kind::eMat4x4F );
			result->declMember( "curMtxModel", ast::type::Kind::eMat4x4F );
			result->declMember( "mtxNormal", ast::type::Kind::eMat4x4F );
			result->declMember( "shadowEnvMap", ast::type::Kind::eVec4I );
		}

		return result;
	}

	sdw::Mat4 ModelBufferData::getPrvModelMtx( ProgramFlags programFlags
		, sdw::Mat4 const & curModelMatrix )const
	{
		if ( checkFlag( programFlags, ProgramFlag::eSkinning )
			|| checkFlag( programFlags, ProgramFlag::eInstantiation ) )
		{
			return curModelMatrix;
		}

		return m_prvMtxModel;
	}

	sdw::Mat3 ModelBufferData::getNormalMtx( ProgramFlags programFlags
		, sdw::Mat4 const & curModelMatrix )const
	{
		if ( checkFlag( programFlags, ProgramFlag::eSkinning )
			|| checkFlag( programFlags, ProgramFlag::eInstantiation ) )
		{
			return transpose( inverse( mat3( curModelMatrix ) ) );
		}

		return mat3( m_mtxNormal );
	}

	sdw::Vec4 ModelBufferData::worldToModel( sdw::Vec4 const & pos )const
	{
		return inverse( m_curMtxModel ) * pos;
	}

	sdw::Vec4 ModelBufferData::modelToWorld( sdw::Vec4 const & pos )const
	{
		return m_curMtxModel * pos;
	}

	sdw::Vec4 ModelBufferData::modelToCurWorld( sdw::Vec4 const & pos )const
	{
		return m_curMtxModel * pos;
	}

	sdw::Vec4 ModelBufferData::modelToPrvWorld( sdw::Vec4 const & pos )const
	{
		return m_prvMtxModel * pos;
	}

	sdw::Mat4 ModelBufferData::getCurModelMtx( ProgramFlags programFlags
		, SkinningData const & skinning
		, sdw::IVec4 const & boneIds0
		, sdw::IVec4 const & boneIds1
		, sdw::Vec4 const & boneWeights0
		, sdw::Vec4 const & boneWeights1 )const
	{
		if ( checkFlag( programFlags, ProgramFlag::eSkinning ) )
		{
			return SkinningUbo::computeTransform( skinning
				, boneIds0
				, boneIds1
				, boneWeights0
				, boneWeights1
				, m_curMtxModel
				, *getWriter()
				, programFlags
				, m_curMtxModel );
		}

		return m_curMtxModel;
	}

	//*********************************************************************************************

	ModelDatas::ModelDatas( sdw::ShaderWriter & writer
		, uint32_t binding
		, uint32_t set
		, bool enable )
		: m_writer{ writer }
	{
		if ( enable )
		{
			m_ssbo = std::make_unique< sdw::ArraySsboT< ModelBufferData > >( m_writer
				, ModelBufferData::DataName
				, binding
				, set
				, true );
		}
	}

	ModelBufferData ModelDatas::operator[]( sdw::UInt const & index )const
	{
		return ( *m_ssbo )[index - 1_u];
	}

	//*********************************************************************************************
}
