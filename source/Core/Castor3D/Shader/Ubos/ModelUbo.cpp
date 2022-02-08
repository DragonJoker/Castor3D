#include "Castor3D/Shader/Ubos/ModelUbo.hpp"

#include "Castor3D/Shader/Ubos/SkinningUbo.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d
{
	//*********************************************************************************************

	namespace shader
	{
		ModelData::ModelData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstance{ writer, std::move( expr ), enabled }
			, m_prvMtxModel{ getMember< sdw::Mat4 >( "prvMtxModel" ) }
			, m_curMtxModel{ getMember< sdw::Mat4 >( "curMtxModel" ) }
			, m_mtxNormal{ getMember< sdw::Mat4 >( "mtxNormal" ) }
			, m_textures0{ getMember< sdw::UVec4 >( "textures0" ) }
			, m_textures1{ getMember< sdw::UVec4 >( "textures1" ) }
			, m_textures{ getMember< sdw::Int >( "textures" ) }
			, m_shadowReceiver{ getMember< sdw::Int >( "shadowReceiver" ) }
			, m_materialIndex{ getMember< sdw::Int >( "materialIndex" ) }
			, m_nodeId{ getMember< sdw::Int >( "nodeId" ) }
			, m_envMapIndex{ getMember< sdw::Int >( "envMapIndex" ) }
		{
		}

		ast::type::BaseStructPtr ModelData::makeType( ast::type::TypesCache & cache )
		{
			auto result = cache.getStruct( ast::type::MemoryLayout::eStd140
				, "C3D_ModelData" );

			if ( result->empty() )
			{
				result->declMember( "prvMtxModel", ast::type::Kind::eMat4x4F );
				result->declMember( "curMtxModel", ast::type::Kind::eMat4x4F );
				result->declMember( "mtxNormal", ast::type::Kind::eMat4x4F );
				result->declMember( "textures0", ast::type::Kind::eVec4U );
				result->declMember( "textures1", ast::type::Kind::eVec4U );
				result->declMember( "textures", ast::type::Kind::eInt );
				result->declMember( "shadowReceiver", ast::type::Kind::eInt );
				result->declMember( "materialIndex", ast::type::Kind::eInt );
				result->declMember( "nodeId", ast::type::Kind::eInt );
				result->declMember( "envMapIndex", ast::type::Kind::eInt );
			}

			return result;
		}

		std::unique_ptr< sdw::Struct > ModelData::declare( sdw::ShaderWriter & writer )
		{
			return std::make_unique< sdw::Struct >( writer
				, makeType( writer.getTypesCache() ) );
		}

		sdw::Mat4 ModelData::getPrvModelMtx( ProgramFlags programFlags
			, sdw::Mat4 const & curModelMatrix )const
		{
			if ( checkFlag( programFlags, ProgramFlag::eSkinning )
				|| checkFlag( programFlags, ProgramFlag::eInstantiation ) )
			{
				return curModelMatrix;
			}

			return m_prvMtxModel;
		}

		sdw::Mat3 ModelData::getNormalMtx( ProgramFlags programFlags
			, sdw::Mat4 const & curModelMatrix )const
		{
			if ( checkFlag( programFlags, ProgramFlag::eSkinning )
				|| checkFlag( programFlags, ProgramFlag::eInstantiation ) )
			{
				return transpose( inverse( mat3( curModelMatrix ) ) );
			}

			return mat3( m_mtxNormal );
		}

		sdw::UVec4 ModelData::getTextures0( ProgramFlags programFlags
			, sdw::UVec4 const & instanceData )const
		{
			if ( checkFlag( programFlags, ProgramFlag::eInstantiation ) )
			{
				return instanceData;
			}

			return getTextures0();
		}

		sdw::UVec4 ModelData::getTextures0()const
		{
			return m_textures0;
		}

		sdw::UVec4 ModelData::getTextures1( ProgramFlags programFlags
			, sdw::UVec4 const & instanceData )const
		{
			if ( checkFlag( programFlags, ProgramFlag::eInstantiation ) )
			{
				return instanceData;
			}

			return getTextures1();
		}

		sdw::UVec4 ModelData::getTextures1()const
		{
			return m_textures1;
		}

		sdw::Int ModelData::getTextures( ProgramFlags programFlags
			, sdw::Int const & instanceData )const
		{
			if ( checkFlag( programFlags, ProgramFlag::eInstantiation ) )
			{
				return instanceData;
			}

			return getTextures();
		}

		sdw::Int ModelData::getTextures()const
		{
			return m_textures;
		}

		sdw::UInt ModelData::getMaterialIndex( ProgramFlags programFlags
			, sdw::Int const & instanceMaterial )const
		{
			if ( checkFlag( programFlags, ProgramFlag::eInstantiation ) )
			{
				return getWriter()->cast< sdw::UInt >( instanceMaterial );
			}

			return getMaterialIndex();
		}

		sdw::UInt ModelData::getMaterialIndex()const
		{
			return getWriter()->cast< sdw::UInt >( m_materialIndex );
		}

		sdw::Int ModelData::getNodeId( ProgramFlags programFlags
			, sdw::Int const & instanceNodeId )const
		{
			if ( checkFlag( programFlags, ProgramFlag::eInstantiation ) )
			{
				return instanceNodeId;
			}

			return getNodeId();
		}

		sdw::Int ModelData::getNodeId()const
		{
			return m_nodeId;
		}

		sdw::Vec4 ModelData::worldToModel( sdw::Vec4 const & pos )const
		{
			return inverse( m_curMtxModel ) * pos;
		}

		sdw::Vec4 ModelData::modelToWorld( sdw::Vec4 const & pos )const
		{
			return m_curMtxModel * pos;
		}

		sdw::Vec4 ModelData::modelToCurWorld( sdw::Vec4 const & pos )const
		{
			return m_curMtxModel * pos;
		}

		sdw::Vec4 ModelData::modelToPrvWorld( sdw::Vec4 const & pos )const
		{
			return m_prvMtxModel * pos;
		}

		sdw::Mat4 ModelData::getCurModelMtx( ProgramFlags programFlags
			, SkinningData const & skinning
			, sdw::IVec4 const & boneIds0
			, sdw::IVec4 const & boneIds1
			, sdw::Vec4 const & boneWeights0
			, sdw::Vec4 const & boneWeights1
			, sdw::Mat4 const & transform )const
		{
			if ( checkFlag( programFlags, ProgramFlag::eSkinning ) )
			{
				return SkinningUbo::computeTransform( skinning
					, boneIds0
					, boneIds1
					, boneWeights0
					, boneWeights1
					, transform
					, *getWriter()
					, programFlags
					, m_curMtxModel );
			}

			if ( checkFlag( programFlags, ProgramFlag::eInstantiation ) )
			{
				return transform;
			}

			return m_curMtxModel;
		}

		sdw::UInt ModelData::getTexture( sdw::UVec4 const & textures0
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

	//*********************************************************************************************

	castor::String const ModelUbo::BufferModel = cuT( "Model" );
	castor::String const ModelUbo::ModelData = cuT( "c3d_modelData" );

	//*********************************************************************************************
}
