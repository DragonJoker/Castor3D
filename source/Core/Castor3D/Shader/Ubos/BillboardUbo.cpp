#include "Castor3D/Shader/Ubos/BillboardUbo.hpp"

#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d
{
	//*********************************************************************************************

	namespace shader
	{
		BillboardData::BillboardData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstance{ writer, std::move( expr ), enabled }
			, m_dimensions{ getMember< sdw::Vec2 >( "dimensions" ) }
		{
		}

		ast::type::BaseStructPtr BillboardData::makeType( ast::type::TypesCache & cache )
		{
			auto result = cache.getStruct( ast::type::MemoryLayout::eStd140
				, "C3D_BillboardData" );

			if ( result->empty() )
			{
				result->declMember( "dimensions", ast::type::Kind::eVec2F );
			}

			return result;
		}

		std::unique_ptr< sdw::Struct > BillboardData::declare( sdw::ShaderWriter & writer )
		{
			return std::make_unique< sdw::Struct >( writer
				, makeType( writer.getTypesCache() ) );
		}

		sdw::Vec3 BillboardData::getCameraRight( ProgramFlags programFlags
			, MatrixData const & matrixData )const
		{
			if ( checkFlag( programFlags, ProgramFlag::eSpherical ) )
			{
				return normalize( matrixData.getCurViewRight() );
			}

			return normalize( vec3( matrixData.m_curView[0][0], 0.0_f, matrixData.m_curView[2][0] ) );
		}

		sdw::Vec3 BillboardData::getCameraUp( ProgramFlags programFlags
			, MatrixData const & matrixData )const
		{
			if ( checkFlag( programFlags, ProgramFlag::eSpherical ) )
			{
				return normalize( matrixData.getCurViewUp() );
			}

			return vec3( 0.0_f, 1.0_f, 0.0_f );
		}

		sdw::Float BillboardData::getWidth( ProgramFlags programFlags
			, SceneData const & sceneData )const
		{
			return ( checkFlag( programFlags, ProgramFlag::eFixedSize )
				? m_dimensions.x() / sceneData.m_clipInfo.x()
				: m_dimensions.x() );
		}

		sdw::Float BillboardData::getHeight( ProgramFlags programFlags
			, SceneData const & sceneData )const
		{
			return ( checkFlag( programFlags, ProgramFlag::eFixedSize )
				? m_dimensions.y() / sceneData.m_clipInfo.y()
				: m_dimensions.y() );
		}
	}

	//*********************************************************************************************

	castor::String const BillboardUbo::BufferBillboard = cuT( "Billboard" );
	castor::String const BillboardUbo::BillboardData = cuT( "c3d_billboardData" );

	//*********************************************************************************************
}
