#include "Castor3D/Shader/Ubos/ModelDataUbo.hpp"

#include "Castor3D/Limits.hpp"
#include "Castor3D/Shader/Ubos/SkinningUbo.hpp"

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
		, m_meshletsScale{ getMember< sdw::Vec4 >( "meshletsScale" ) }
		, m_textures{ m_countsIDs.x() }
		, m_materialId{ writer.cast< sdw::UInt >( m_countsIDs.y() ) }
		, m_envMapId{ m_countsIDs.z() }
		, m_shadowReceiver{ m_countsIDs.w() }
		, m_scale{ m_meshletsScale.xyz() }
		, m_meshletCount{ writer.cast< sdw::UInt >( m_meshletsScale.w() ) }
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
			result->declMember( "meshletsScale", ast::type::Kind::eVec4F );
		}

		return result;
	}

	sdw::Mat4 ModelIndices::getPrvModelMtx( ProgramFlags programsFlags
		, sdw::Mat4 const & curModelMatrix )const
	{
		if ( checkFlag( programsFlags, ProgramFlag::eInstantiation ) )
		{
			return curModelMatrix;
		}

		return m_prvMtxModel;
	}

	sdw::Mat3 ModelIndices::getNormalMtx( SubmeshFlags submeshFlags
		, sdw::Mat4 const & curModelMatrix )const
	{
		if ( checkFlag( submeshFlags, SubmeshFlag::eSkin ) )
		{
			return transpose( inverse( mat3( curModelMatrix ) ) );
		}

		return mat3( m_mtxNormal );
	}

	sdw::Mat3 ModelIndices::getNormalMtx( ProgramFlags programsFlags
		, sdw::Mat4 const & curModelMatrix )const
	{
		if ( checkFlag( programsFlags, ProgramFlag::eInstantiation ) )
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

	sdw::Mat4 ModelIndices::getCurModelMtx( ProgramFlags programsFlags
		, sdw::Mat4 const & transform )const
	{
		if ( checkFlag( programsFlags, ProgramFlag::eInstantiation ) )
		{
			return transform;
		}

		return m_curMtxModel;
	}

	sdw::Mat4 ModelIndices::getCurModelMtx( SkinningData const & skinning
		, sdw::UInt const & skinningId
		, sdw::UVec4 const & boneIds0
		, sdw::UVec4 const & boneIds1
		, sdw::Vec4 const & boneWeights0
		, sdw::Vec4 const & boneWeights1 )const
	{
		if ( skinning.transforms )
		{
			return SkinningUbo::computeTransform( skinning
				, m_curMtxModel
				, *getWriter()
				, skinningId
				, boneIds0
				, boneIds1
				, boneWeights0
				, boneWeights1 );
		}

		return m_curMtxModel;
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
}
