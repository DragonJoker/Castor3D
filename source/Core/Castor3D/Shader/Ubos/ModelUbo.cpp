#include "Castor3D/Shader/Ubos/ModelUbo.hpp"

#include "Castor3D/Shader/Ubos/SkinningUbo.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"

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
			, m_shadowReceiver{ getMember< sdw::Int >( "shadowReceiver" ) }
			, m_materialIndex{ getMember< sdw::Int >( "materialIndex" ) }
			, m_nodeId{ getMember< sdw::Int >( "nodeId" ) }
			, m_envMapIndex{ getMember< sdw::Int >( "envMapIndex" ) }
		{
		}

		ast::type::StructPtr ModelData::makeType( ast::type::TypesCache & cache )
		{
			auto result = cache.getStruct( ast::type::MemoryLayout::eStd140
				, "C3D_ModelData" );

			if ( result->empty() )
			{
				result->declMember( "prvMtxModel", ast::type::Kind::eMat4x4F );
				result->declMember( "curMtxModel", ast::type::Kind::eMat4x4F );
				result->declMember( "mtxNormal", ast::type::Kind::eMat4x4F );
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

		sdw::Mat4 ModelData::getCurModelMtx( ProgramFlags programFlags
			, SkinningData const & skinning
			, VertexSurface const & surface )const
		{
			if ( checkFlag( programFlags, ProgramFlag::eSkinning ) )
			{
				return SkinningUbo::computeTransform( skinning
					, surface
					, *getWriter()
					, programFlags
					, m_curMtxModel );
			}

			if ( checkFlag( programFlags, ProgramFlag::eInstantiation ) )
			{
				return surface.transform;
			}

			return m_curMtxModel;
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
	}

	//*********************************************************************************************

	castor::String const ModelUbo::BufferModel = cuT( "Model" );
	castor::String const ModelUbo::ModelData = cuT( "c3d_modelData" );

	//*********************************************************************************************
}
