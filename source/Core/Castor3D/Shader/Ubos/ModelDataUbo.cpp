#include "Castor3D/Shader/Ubos/ModelDataUbo.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d::shader
{
	castor::String const ModelIndices::BufferName = cuT( "ModelsIndices" );
	castor::String const ModelIndices::DataName = cuT( "c3d_modelsData" );

	ModelIndices::ModelIndices( sdw::ShaderWriter & writer
		, ast::expr::ExprPtr expr
		, bool enabled )
		: StructInstance{ writer, std::move( expr ), enabled }
		, m_prvMtxModel{ getMember< sdw::Mat4 >( "prvMtxModel" ) }
		, m_curMtxModel{ getMember< sdw::Mat4 >( "curMtxModel" ) }
		, m_mtxNormal{ getMember< sdw::Mat4 >( "mtxNormal" ) }
		, m_textures0{ getMember< sdw::UVec4 >( "textures0" ) }
		, m_textures1{ getMember< sdw::UVec4 >( "textures1" ) }
		, m_countsIDs{ getMember< sdw::IVec4 >( "countsIDs" ) }
		, m_textures{ m_countsIDs.x() }
		, m_materialId{ m_countsIDs.y() }
		, m_envMapId{ m_countsIDs.z() }
		, m_shadowReceiver{ m_countsIDs.w() }
	{
	}

	ast::type::BaseStructPtr ModelIndices::makeType( ast::type::TypesCache & cache )
	{
		auto result = cache.getStruct( ast::type::MemoryLayout::eStd140
			, "C3D_" + BufferName );

		if ( result->empty() )
		{
			result->declMember( "prvMtxModel", ast::type::Kind::eMat4x4F );
			result->declMember( "curMtxModel", ast::type::Kind::eMat4x4F );
			result->declMember( "mtxNormal", ast::type::Kind::eMat4x4F );
			result->declMember( "textures0", ast::type::Kind::eVec4U );
			result->declMember( "textures1", ast::type::Kind::eVec4U );
			result->declMember( "countsIDs", ast::type::Kind::eVec4I );
		}

		return result;
	}

	sdw::Mat4 ModelIndices::getPrvModelMtx( ProgramFlags programFlags
		, sdw::Mat4 const & curModelMatrix )const
	{
		if ( checkFlag( programFlags, ProgramFlag::eSkinning )
			|| checkFlag( programFlags, ProgramFlag::eInstantiation ) )
		{
			return curModelMatrix;
		}

		return m_prvMtxModel;
	}

	sdw::Mat3 ModelIndices::getNormalMtx( ProgramFlags programFlags
		, sdw::Mat4 const & curModelMatrix )const
	{
		if ( checkFlag( programFlags, ProgramFlag::eSkinning )
			|| checkFlag( programFlags, ProgramFlag::eInstantiation ) )
		{
			return transpose( inverse( mat3( curModelMatrix ) ) );
		}

		return mat3( m_mtxNormal );
	}

	sdw::Vec4 ModelIndices::worldToModel( sdw::Vec4 const & pos )const
	{
		return inverse( m_curMtxModel ) * pos;
	}

	sdw::Vec4 ModelIndices::modelToWorld( sdw::Vec4 const & pos )const
	{
		return m_curMtxModel * pos;
	}

	sdw::Vec4 ModelIndices::modelToCurWorld( sdw::Vec4 const & pos )const
	{
		return m_curMtxModel * pos;
	}

	sdw::Vec4 ModelIndices::modelToPrvWorld( sdw::Vec4 const & pos )const
	{
		return m_prvMtxModel * pos;
	}

	sdw::Mat4 ModelIndices::getCurModelMtx( ProgramFlags programFlags
		, SkinningData const & skinning
		, sdw::Mat4 const & transform
		, sdw::UInt const & skinningId
		, sdw::UVec4 const & boneIds0
		, sdw::UVec4 const & boneIds1
		, sdw::Vec4 const & boneWeights0
		, sdw::Vec4 const & boneWeights1 )const
	{
		if ( checkFlag( programFlags, ProgramFlag::eSkinning ) )
		{
			return SkinningUbo::computeTransform( skinning
				, transform
				, *getWriter()
				, programFlags
				, m_curMtxModel
				, skinningId
				, boneIds0
				, boneIds1
				, boneWeights0
				, boneWeights1 );
		}

		if ( checkFlag( programFlags, ProgramFlag::eInstantiation ) )
		{
			return transform;
		}

		return m_curMtxModel;
	}

	sdw::Mat4 ModelIndices::getCurModelMtx( ProgramFlags programFlags
		, SkinningData const & skinning
		, sdw::UInt const & skinningId
		, sdw::UVec4 const & boneIds0
		, sdw::UVec4 const & boneIds1
		, sdw::Vec4 const & boneWeights0
		, sdw::Vec4 const & boneWeights1 )const
	{
		if ( checkFlag( programFlags, ProgramFlag::eSkinning ) )
		{
			return SkinningUbo::computeTransform( skinning
				, m_curMtxModel
				, *getWriter()
				, programFlags
				, m_curMtxModel
				, skinningId
				, boneIds0
				, boneIds1
				, boneWeights0
				, boneWeights1 );
		}

		return m_curMtxModel;
	}

	sdw::UVec4 ModelIndices::getTextures0()const
	{
		return m_textures0;
	}

	sdw::UVec4 ModelIndices::getTextures1()const
	{
		return m_textures1;
	}

	sdw::Int ModelIndices::getTextures()const
	{
		return m_textures;
	}

	sdw::UInt ModelIndices::getMaterialId()const
	{
		return getWriter()->cast< sdw::UInt >( m_materialId );
	}

	sdw::UInt ModelIndices::getTexture( sdw::UVec4 const & textures0
		, sdw::UVec4 const & textures1
		, uint32_t index )
	{
		return ( index < 4u
			? textures0[index]
			: ( index < 8u
				? textures1[index - 4u]
				: 0_u ) );
	}

	//*********************************************************************************************

	castor::String const ObjectsIds::BufferName = cuT( "ObjectIdsBuffer" );
	castor::String const ObjectsIds::DataName = cuT( "c3d_objectIdsData" );
	uint32_t const ObjectsIds::NodesCount = 50'000u;

	ObjectsIds::ObjectsIds( sdw::ShaderWriter & writer
		, ast::expr::ExprPtr expr
		, bool enabled )
		: StructInstance{ writer, std::move( expr ), enabled }
		, m_data{ getMemberArray< sdw::UVec4 >( "data" ) }
	{
	}

	ast::type::BaseStructPtr ObjectsIds::makeType( ast::type::TypesCache & cache )
	{
		auto result = cache.getStruct( ast::type::MemoryLayout::eStd430
			, "C3D_" + BufferName );

		if ( result->empty() )
		{
			result->declMember( "data"
				, ast::type::Kind::eVec4U
				, 10'000u );
		}

		return result;
	}

	//*********************************************************************************************
}
