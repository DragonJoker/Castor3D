#include "Castor3D/Shader/Shaders/GlslSurface.hpp"

#include "Castor3D/Render/RenderNodesPass.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"
#include "Castor3D/Shader/Ubos/MorphingUbo.hpp"

#include <ShaderWriter/Intrinsics/Intrinsics.hpp>

namespace castor3d::shader
{
	//*****************************************************************************************

	template< ast::var::Flag FlagT >
	BillboardSurfaceT< FlagT >::BillboardSurfaceT( sdw::ShaderWriter & writer
		, sdw::expr::ExprPtr expr
		, bool enabled )
		: StructInstance{ writer, castor::move( expr ), enabled }
		, position{ this->getMember< sdw::Vec4 >( "position" ) }
		, texture0{ this->getMember< sdw::Vec2 >( "texture0", true ) }
		, center{ this->getMember< sdw::Vec3 >( "center" ) }
	{
	}

	template< ast::var::Flag FlagT >
	ast::type::IOStructPtr BillboardSurfaceT< FlagT >::makeIOType( ast::type::TypesCache & cache
		, sdw::EntryPoint entryPoint
		, PipelineFlags const & flags )
	{
		auto result = cache.getIOStruct( "C3D_BillboardSurface"
			, entryPoint
			, FlagT );

		if ( result->empty() )
		{
			uint32_t index = 0u;
			result->declMember( "position", ast::type::Kind::eVec4F
				, ast::type::NotArray
				, index++ );
			result->declMember( "texture0", ast::type::Kind::eVec2F
				, ast::type::NotArray
				, ( flags.enableTexcoords() ? index++ : 0 )
				, flags.enableTexcoords() );
			result->declMember( "center", ast::type::Kind::eVec3F
				, ast::type::NotArray
				, index++ );
		}

		return result;
	}

	template< ast::var::Flag FlagT >
	ast::type::BaseStructPtr BillboardSurfaceT< FlagT >::makeType( ast::type::TypesCache & cache )
	{
		auto result = cache.getStruct( ast::type::MemoryLayout::eC, "C3D_BillboardSurface" );

		if ( result->empty() )
		{
			result->declMember( "position", ast::type::Kind::eVec4F
				, ast::type::NotArray );
			result->declMember( "texture0", ast::type::Kind::eVec2F
				, ast::type::NotArray );
			result->declMember( "center", ast::type::Kind::eVec3F
				, ast::type::NotArray );
		}

		return result;
	}

	//*****************************************************************************************

	template< typename TexcoordT, ast::var::Flag FlagT >
	RasterizerSurfaceT< TexcoordT, FlagT >::RasterizerSurfaceT( sdw::ShaderWriter & writer
		, sdw::expr::ExprPtr expr
		, bool enabled )
		: RasterizerSurfaceBase{ writer, castor::move( expr ), enabled }
		, texture0{ this->getMember< TexcoordT >( "texture0", true ) }
		, texture1{ this->getMember< TexcoordT >( "texture1", true ) }
		, texture2{ this->getMember< TexcoordT >( "texture2", true ) }
		, texture3{ this->getMember< TexcoordT >( "texture3", true ) }
	{
	}

	template< typename TexcoordT, ast::var::Flag FlagT >
	RasterizerSurfaceT< TexcoordT, FlagT >::RasterizerSurfaceT( sdw::Vec3 clip
		, sdw::Vec3 view
		, sdw::Vec3 world
		, sdw::Vec3 normal
		, TexcoordT coord )
		: RasterizerSurfaceT{ findWriterMandat( clip, view, world, normal, coord )
			, sdw::makeAggrInit( makeType( findTypesCache( clip, view, world, normal, coord ) )
				, [&clip, &view, &world, &normal, &coord]()
				{
					sdw::expr::ExprList result;
					SurfaceBase::fillInit( result, clip, vec4( view, 1.0_f ), vec4( world, 1.0_f ), normal );
					result.emplace_back( makeExpr( coord ) );
					return result;
				}() )
			, true }
	{
	}

	template< typename TexcoordT, ast::var::Flag FlagT >
	template< ast::var::Flag FlagU >
	RasterizerSurfaceT< TexcoordT, FlagT >::RasterizerSurfaceT( RasterizerSurfaceT< TexcoordT, FlagU > const & rhs )
		: RasterizerSurfaceT{ *rhs.getWriter()
			, sdw::makeAggrInit( rhs.getType(), rhs.makeAggrInit() )
			, rhs.isEnabled() }
	{
	}

	template< typename TexcoordT, ast::var::Flag FlagT >
	ast::type::IOStructPtr RasterizerSurfaceT< TexcoordT, FlagT >::makeIOType( ast::type::TypesCache & cache
		, sdw::EntryPoint entryPoint
		, SubmeshShaders const & submeshShaders
		, PassShaders const & passShaders
		, PipelineFlags const & flags )
	{
		auto result = cache.getIOStruct( "C3D_RasterizerSurface"
			, entryPoint
			, FlagT );

		if ( result->empty() )
		{
			auto texType = TexcoordT::makeType( cache );
			uint32_t index = 0u;
			RasterizerSurfaceBase::fillIOType( *result, submeshShaders, passShaders, flags, index );
			result->declMember( "texture0", texType
				, ast::type::NotArray
				, ( flags.enableTexcoord0() ? index++ : 0 )
				, flags.enableTexcoord0() );
			result->declMember( "texture1", texType
				, ast::type::NotArray
				, ( flags.enableTexcoord1() ? index++ : 0 )
				, flags.enableTexcoord1() );
			result->declMember( "texture2", texType
				, ast::type::NotArray
				, ( flags.enableTexcoord2() ? index++ : 0 )
				, flags.enableTexcoord2() );
			result->declMember( "texture3", texType
				, ast::type::NotArray
				, ( flags.enableTexcoord3() ? index++ : 0 )
				, flags.enableTexcoord3() );
		}

		return result;
	}

	template< typename TexcoordT, ast::var::Flag FlagT >
	ast::type::BaseStructPtr RasterizerSurfaceT< TexcoordT, FlagT >::makeType( ast::type::TypesCache & cache
		, SubmeshShaders const & submeshShaders
		, PassShaders const & passShaders
		, PipelineFlags const & flags )
	{
		auto result = cache.getStruct( ast::type::MemoryLayout::eC
			, "C3D_RasterizerSurface" );

		if ( result->empty() )
		{
			auto texType = TexcoordT::makeType( cache );
			RasterizerSurfaceBase::fillType( *result, submeshShaders, passShaders, flags );
			result->declMember( "texture0", texType
				, ast::type::NotArray
				, flags.enableTexcoord0() );
			result->declMember( "texture1", texType
				, ast::type::NotArray
				, flags.enableTexcoord1() );
			result->declMember( "texture2", texType
				, ast::type::NotArray
				, flags.enableTexcoord2() );
			result->declMember( "texture3", texType
				, ast::type::NotArray
				, flags.enableTexcoord3() );
		}

		return result;
	}

	template< typename TexcoordT, ast::var::Flag FlagT >
	ast::type::BaseStructPtr RasterizerSurfaceT< TexcoordT, FlagT >::makeType( ast::type::TypesCache & cache
		, SubmeshShaders const & submeshShaders )
	{
		auto result = cache.getStruct( ast::type::MemoryLayout::eC, "C3D_RasterSurface" );

		if ( result->empty() )
		{
			auto texType = TexcoordT::makeType( cache );
			RasterizerSurfaceBase::fillType( *result, submeshShaders );
			result->declMember( "texture0", texType
				, ast::type::NotArray );
			result->declMember( "texture1", texType
				, ast::type::NotArray );
			result->declMember( "texture2", texType
				, ast::type::NotArray );
			result->declMember( "texture3", texType
				, ast::type::NotArray );
		}

		return result;
	}

	//*****************************************************************************************

	template< sdw::var::Flag FlagT >
	VoxelSurfaceT< FlagT >::VoxelSurfaceT( sdw::ShaderWriter & writer
		, sdw::expr::ExprPtr expr
		, bool enabled )
		: SurfaceBase{ writer, castor::move( expr ), enabled }
		, texture0{ getMember< sdw::Vec3 >( "texture0", true ) }
		, texture1{ getMember< sdw::Vec3 >( "texture1", true ) }
		, texture2{ getMember< sdw::Vec3 >( "texture2", true ) }
		, texture3{ getMember< sdw::Vec3 >( "texture3", true ) }
		, colour{ getMember< sdw::Vec3 >( "colour", true ) }
		, nodeId{ getMember< sdw::UInt >( "nodeId" ) }
		, passMultipliers{ getMemberArray< sdw::Vec4 >( "passMultipliers", true ) }
	{
	}

	template< sdw::var::Flag FlagT >
	sdw::type::IOStructPtr VoxelSurfaceT< FlagT >::makeIOType( sdw::type::TypesCache & cache
		, sdw::EntryPoint entryPoint
		, PipelineFlags const & flags )
	{
		auto result = cache.getIOStruct( "C3D_VoxelSurface"
			, entryPoint
			, FlagT );

		if ( result->empty() )
		{
			uint32_t index = 0u;
			SurfaceBase::fillIOType( *result, flags, index );
			result->declMember( "texture0"
				, sdw::type::Kind::eVec3F
				, sdw::type::NotArray
				, ( flags.enableTexcoord0() ? index++ : 0 )
				, flags.enableTexcoord0() );
			result->declMember( "texture1"
				, sdw::type::Kind::eVec3F
				, sdw::type::NotArray
				, ( flags.enableTexcoord1() ? index++ : 0 )
				, flags.enableTexcoord1() );
			result->declMember( "texture2"
				, sdw::type::Kind::eVec3F
				, sdw::type::NotArray
				, ( flags.enableTexcoord2() ? index++ : 0 )
				, flags.enableTexcoord2() );
			result->declMember( "texture3"
				, sdw::type::Kind::eVec3F
				, sdw::type::NotArray
				, ( flags.enableTexcoord3() ? index++ : 0 )
				, flags.enableTexcoord3() );
			result->declMember( "colour"
				, sdw::type::Kind::eVec3F
				, sdw::type::NotArray
				, ( flags.enableColours() ? index++ : 0 )
				, flags.enableColours() );
			result->declMember( "nodeId"
				, sdw::type::Kind::eUInt
				, sdw::type::NotArray
				, index++ );
			result->declMember( "passMultipliers"
				, sdw::type::Kind::eVec4F
				, 4u
				, ( flags.enablePassMasks() ? index : 0 )
				, flags.enablePassMasks() );
		}

		return result;
	}

	template< sdw::var::Flag FlagT >
	sdw::type::BaseStructPtr VoxelSurfaceT< FlagT >::makeType( sdw::type::TypesCache & cache
		, PipelineFlags const & flags )
	{
		auto result = cache.getStruct( sdw::type::MemoryLayout::eStd430
			, "C3D_VoxelSurfaceT" );

		if ( result->empty() )
		{
			SurfaceBase::fillType( *result, flags );
			result->declMember( "texture0"
				, sdw::type::Kind::eVec3F
				, sdw::type::NotArray
				, flags.enableTexcoord0() );
			result->declMember( "texture1"
				, sdw::type::Kind::eVec3F
				, sdw::type::NotArray
				, flags.enableTexcoord1() );
			result->declMember( "texture2"
				, sdw::type::Kind::eVec3F
				, sdw::type::NotArray
				, flags.enableTexcoord2() );
			result->declMember( "texture3"
				, sdw::type::Kind::eVec3F
				, sdw::type::NotArray
				, flags.enableTexcoord3() );
			result->declMember( "colour"
				, sdw::type::Kind::eVec3F
				, sdw::type::NotArray
				, flags.enableColours() );
			result->declMember( "nodeId"
				, sdw::type::Kind::eUInt
				, sdw::type::NotArray );
			result->declMember( "passMultipliers"
				, sdw::type::Kind::eVec4F
				, 4u
				, flags.enablePassMasks() );
		}

		return result;
	}

	template< sdw::var::Flag FlagT >
	sdw::type::BaseStructPtr VoxelSurfaceT< FlagT >::makeType( sdw::type::TypesCache & cache )
	{
		auto result = cache.getStruct( sdw::type::MemoryLayout::eStd430
			, "C3D_VoxelSurface" );

		if ( result->empty() )
		{
			SurfaceBase::fillType( *result );
			result->declMember( "texture0"
				, sdw::type::Kind::eVec3F
				, sdw::type::NotArray );
			result->declMember( "texture1"
				, sdw::type::Kind::eVec3F
				, sdw::type::NotArray );
			result->declMember( "texture2"
				, sdw::type::Kind::eVec3F
				, sdw::type::NotArray );
			result->declMember( "texture3"
				, sdw::type::Kind::eVec3F
				, sdw::type::NotArray );
			result->declMember( "colour"
				, sdw::type::Kind::eVec3F
				, sdw::type::NotArray );
			result->declMember( "nodeId"
				, sdw::type::Kind::eUInt
				, sdw::type::NotArray );
			result->declMember( "passMultipliers"
				, sdw::type::Kind::eVec4F
				, 4u );
		}

		return result;
	}

	//*****************************************************************************************
}
