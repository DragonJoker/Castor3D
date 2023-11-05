/*
See LICENSE file in root folder
*/
namespace castor3d::shader
{
	//*********************************************************************************************

	template< sdw::var::Flag FlagT >
	PrepassOutputT< FlagT >::PrepassOutputT( sdw::ShaderWriter & writer
		, sdw::expr::ExprPtr expr
		, bool enabled )
		: sdw::StructInstance{ writer, std::move( expr ), enabled }
		, depthObj{ this->getMember< sdw::Vec4 >( "depthObj" ) }
		, visibility{ this->getMember< sdw::UVec2 >( "visibility", true ) }
		, velocity{ this->getMember< sdw::Vec2 >( "velocity", true ) }
		, nmlOcc{ this->getMember< sdw::Vec4 >( "nmlOcc", true ) }
	{
	}

	template< sdw::var::Flag FlagT >
	sdw::type::IOStructPtr PrepassOutputT< FlagT >::makeIOType( sdw::type::TypesCache & cache
		, sdw::EntryPoint entryPoint
		, PipelineFlags const & flags )
	{
		auto result = cache.getIOStruct( "C3D_PrepassOutput"
			, entryPoint
			, FlagT );

		if ( result->empty() )
		{
			uint32_t index = 0u;
			result->declMember( "depthObj", ast::type::Kind::eVec4F
				, ast::type::NotArray
				, index++ );
			result->declMember( "visibility", ast::type::Kind::eVec2U32
				, ast::type::NotArray
				, ( flags.writeVisibility() ? index++ : 0 )
				, flags.writeVisibility() );
			result->declMember( "velocity", ast::type::Kind::eVec2F
				, ast::type::NotArray
				, ( flags.writeVelocity() ? index++ : 0 )
				, flags.writeVelocity() );
			result->declMember( "nmlOcc", ast::type::Kind::eVec4F
				, ast::type::NotArray
				, index++ );
		}

		return result;
	}

	template< sdw::var::Flag FlagT >
	sdw::type::BaseStructPtr PrepassOutputT< FlagT >::makeType( sdw::type::TypesCache & cache )
	{
		auto result = cache.getStruct( ast::type::MemoryLayout::eC, "C3D_PrepassOutput" );

		if ( result->empty() )
		{
			result->declMember( "depthObj", ast::type::Kind::eVec4F
				, ast::type::NotArray );
			result->declMember( "visibility", ast::type::Kind::eVec2U32
				, ast::type::NotArray );
			result->declMember( "velocity", ast::type::Kind::eVec2F
				, ast::type::NotArray );
			result->declMember( "nmlOcc", ast::type::Kind::eVec4F
				, ast::type::NotArray );
		}

		return result;
	}

	//*********************************************************************************************

	template< sdw::var::Flag FlagT >
	ShadowsOutputT< FlagT >::ShadowsOutputT( sdw::ShaderWriter & writer
		, sdw::expr::ExprPtr expr
		, bool enabled )
		: sdw::StructInstance{ writer, std::move( expr ), enabled }
		, linear{ this->getMember< sdw::Float >( "linear" ) }
		, variance{ this->getMember< sdw::Vec2 >( "variance", true ) }
		, normal{ this->getMember< sdw::Vec4 >( "normal", true ) }
		, position{ this->getMember< sdw::Vec4 >( "position", true ) }
		, flux{ this->getMember< sdw::Vec4 >( "flux", true ) }
	{
	}

	template< sdw::var::Flag FlagT >
	sdw::type::IOStructPtr ShadowsOutputT< FlagT >::makeIOType( sdw::type::TypesCache & cache
		, sdw::EntryPoint entryPoint
		, bool needsVsm
		, bool needsRsm )
	{
		auto result = cache.getIOStruct( "C3D_ShadowsOutput"
			, entryPoint
			, FlagT );

		if ( result->empty() )
		{
			uint32_t index = 0u;
			result->declMember( "linear", ast::type::Kind::eFloat
				, ast::type::NotArray
				, index++ );
			result->declMember( "variance", ast::type::Kind::eVec2F
				, ast::type::NotArray
				, ( needsVsm ? index++ : 0 )
				, needsVsm );
			result->declMember( "normal", ast::type::Kind::eVec4F
				, ast::type::NotArray
				, ( needsRsm ? index++ : 0 )
				, needsRsm );
			result->declMember( "position", ast::type::Kind::eVec4F
				, ast::type::NotArray
				, ( needsRsm ? index++ : 0 )
				, needsRsm );
			result->declMember( "flux", ast::type::Kind::eVec4F
				, ast::type::NotArray
				, ( needsRsm ? index++ : 0 )
				, needsRsm );
		}

		return result;
	}

	template< sdw::var::Flag FlagT >
	sdw::type::BaseStructPtr ShadowsOutputT< FlagT >::makeType( sdw::type::TypesCache & cache )
	{
		auto result = cache.getStruct( ast::type::MemoryLayout::eC, "C3D_ShadowsOutput" );

		if ( result->empty() )
		{
			result->declMember( "linear", ast::type::Kind::eFloat
				, ast::type::NotArray );
			result->declMember( "variance", ast::type::Kind::eVec2F
				, ast::type::NotArray );
			result->declMember( "normal", ast::type::Kind::eVec4F
				, ast::type::NotArray );
			result->declMember( "position", ast::type::Kind::eVec4F
				, ast::type::NotArray );
			result->declMember( "flux", ast::type::Kind::eVec4F
				, ast::type::NotArray );
		}

		return result;
	}

	//*********************************************************************************************
}
