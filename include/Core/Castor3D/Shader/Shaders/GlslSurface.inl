#include "Castor3D/Shader/Shaders/GlslSurface.hpp"

#include "Castor3D/Render/RenderNodesPass.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/MorphingUbo.hpp"

#include <ShaderWriter/Writer.hpp>
#include <ShaderWriter/Intrinsics/Intrinsics.hpp>

namespace castor3d::shader
{
	//*****************************************************************************************

	namespace surface
	{
		template< typename TypeT >
		sdw::Array< TypeT > getMemberArray( sdw::StorageBuffer const & buffer
			, std::string const & name
			, bool enable )
		{
			auto result = buffer.declMemberArray< TypeT >( name, enable );
			buffer.end();
			return result;
		}
	}

	//*****************************************************************************************

	template< ast::var::Flag FlagT >
	SurfaceT< FlagT >::SurfaceT( sdw::ShaderWriter & writer
		, ast::expr::ExprPtr expr
		, bool enabled )
		: StructInstance{ writer, std::move( expr ), enabled }
		, clipPosition{ getMember< sdw::Vec3 >( "clipPosition" ) }
		, viewPosition{ getMember< sdw::Vec3 >( "viewPosition" ) }
		, worldPosition{ getMember< sdw::Vec3 >( "worldPosition" ) }
		, worldNormal{ getMember< sdw::Vec3 >( "worldNormal" ) }
		, texCoord{ getMember< sdw::Vec3 >( "texCoord" ) }
	{
	}

	template< ast::var::Flag FlagT >
	void SurfaceT< FlagT >::create( sdw::Vec3 clip
		, sdw::Vec3 view
		, sdw::Vec3 world
		, sdw::Vec3 normal )
	{
		clipPosition = clip;
		viewPosition = view;
		worldPosition = world;
		worldNormal = normal;
	}

	template< ast::var::Flag FlagT >
	void SurfaceT< FlagT >::create( sdw::Vec3 clip
		, sdw::Vec3 view
		, sdw::Vec3 world
		, sdw::Vec3 normal
		, sdw::Vec3 coord )
	{
		clipPosition = clip;
		viewPosition = view;
		worldPosition = world;
		worldNormal = normal;
		texCoord = coord;
	}

	template< ast::var::Flag FlagT >
	void SurfaceT< FlagT >::create( sdw::Vec3 world
		, sdw::Vec3 normal )
	{
		create( sdw::vec3( 0.0_f )
			, sdw::vec3( 0.0_f )
			, world
			, normal
			, sdw::vec3( 0.0_f ) );
	}

	template< ast::var::Flag FlagT >
	ast::type::BaseStructPtr SurfaceT< FlagT >::makeType( ast::type::TypesCache & cache )
	{
		auto result = cache.getStruct( ast::type::MemoryLayout::eStd430
			, "C3D_Surface" );

		if ( result->empty() )
		{
			result->declMember( "clipPosition", ast::type::Kind::eVec3F );
			result->declMember( "viewPosition", ast::type::Kind::eVec3F );
			result->declMember( "worldPosition", ast::type::Kind::eVec3F );
			result->declMember( "worldNormal", ast::type::Kind::eVec3F );
			result->declMember( "texCoord", ast::type::Kind::eVec3F );
		}

		return result;
	}

	template< ast::var::Flag FlagT >
	std::unique_ptr< sdw::Struct > SurfaceT< FlagT >::declare( sdw::ShaderWriter & writer )
	{
		return std::make_unique< sdw::Struct >( writer
			, makeType( writer.getTypesCache() ) );
	}

	//*****************************************************************************************

	template< ast::var::Flag FlagT >
	VertexSurfaceT< FlagT >::VertexSurfaceT( sdw::ShaderWriter & writer
		, sdw::expr::ExprPtr expr
		, bool enabled )
		: StructInstance{ writer, std::move( expr ), enabled }
		// Base
		, position{ this->getMember< sdw::Vec4 >( "position", true ) }
		, normal{ this->getMember< sdw::Vec3 >( "normal", true ) }
		, tangent{ this->getMember< sdw::Vec3 >( "tangent", true ) }
		, texture0{ this->getMember< sdw::Vec3 >( "texcoord0", true ) }
		, texture1{ this->getMember< sdw::Vec3 >( "texcoord1", true ) }
		, texture2{ this->getMember< sdw::Vec3 >( "texcoord2", true ) }
		, texture3{ this->getMember< sdw::Vec3 >( "texcoord3", true ) }
		, colour{ this->getMember< sdw::Vec3 >( "colour", true ) }
		, passMasks{ this->getMember< sdw::UVec4 >( "passMasks", true ) }
		// Velocity
		, velocity{ this->getMember< sdw::Vec3 >( "velocity", true ) }
		// Instantiation
		, objectIds{ this->getMember< sdw::UVec4 >( "objectIds", true ) }
	{
	}

	template< ast::var::Flag FlagT >
	ast::type::IOStructPtr VertexSurfaceT< FlagT >::makeIOType( ast::type::TypesCache & cache
		, PipelineFlags const & flags )
	{
		auto result = cache.getIOStruct( ast::type::MemoryLayout::eC
			, "C3D_" + ( FlagT == sdw::var::Flag::eShaderOutput
				? std::string{ "Output" }
				: std::string{ "Input" } ) + "Surface"
			, FlagT );

		if ( result->empty() )
		{
			uint32_t index = 0u;
			/**
			*	Base
			*/
			//@{
			result->declMember( "position", ast::type::Kind::eVec4F
				, ast::type::NotArray
				, index++ );
			result->declMember( "normal", ast::type::Kind::eVec3F
				, ast::type::NotArray
				, ( flags.enableNormal() ? index++ : 0 )
				, flags.enableNormal() );
			result->declMember( "tangent", ast::type::Kind::eVec3F
				, ast::type::NotArray
				, ( flags.enableTangentSpace() ? index++ : 0 )
				, flags.enableTangentSpace() );
			result->declMember( "texcoord0", ast::type::Kind::eVec3F
				, ast::type::NotArray
				, ( flags.enableTexcoord0() ? index++ : 0 )
				, flags.enableTexcoord0() );
			result->declMember( "texcoord1", ast::type::Kind::eVec3F
				, ast::type::NotArray
				, ( flags.enableTexcoord1() ? index++ : 0 )
				, flags.enableTexcoord1() );
			result->declMember( "texcoord2", ast::type::Kind::eVec3F
				, ast::type::NotArray
				, ( flags.enableTexcoord2() ? index++ : 0 )
				, flags.enableTexcoord2() );
			result->declMember( "texcoord3", ast::type::Kind::eVec3F
				, ast::type::NotArray
				, ( flags.enableTexcoord3() ? index++ : 0 )
				, flags.enableTexcoord3() );
			result->declMember( "colour", ast::type::Kind::eVec3F
				, ast::type::NotArray
				, ( flags.enableColours() ? index++ : 0 )
				, flags.enableColours() );
			result->declMember( "passMasks", ast::type::Kind::eVec4U
				, ast::type::NotArray
				, ( flags.enablePassMasks() ? index++ : 0 )
				, flags.enablePassMasks() );
			//@}
			/**
			*	Velocity
			*/
			//@{
			result->declMember( "velocity", ast::type::Kind::eVec3F
				, ast::type::NotArray
				, ( flags.enableVelocity() ? index++ : 0 )
				, flags.enableVelocity() );
			//@}
			/**
			*	Instantiation
			*/
			//@{
			result->declMember( "objectIds", ast::type::Kind::eVec4U
				, ast::type::NotArray
				, ( flags.enableInstantiation() ? index++ : 0 )
				, flags.enableInstantiation() );
			//@}
		}

		return result;
	}

	template< ast::var::Flag FlagT >
	ast::type::BaseStructPtr VertexSurfaceT< FlagT >::makeType( ast::type::TypesCache & cache )
	{
		auto result = cache.getStruct( ast::type::MemoryLayout::eC, "C3D_VertSurface" );

		if ( result->empty() )
		{
			/**
			*	Base
			*/
			//@{
			result->declMember( "position", ast::type::Kind::eVec4F
				, ast::type::NotArray );
			result->declMember( "normal", ast::type::Kind::eVec3F
				, ast::type::NotArray );
			result->declMember( "tangent", ast::type::Kind::eVec3F
				, ast::type::NotArray );
			result->declMember( "texcoord0", ast::type::Kind::eVec3F
				, ast::type::NotArray );
			result->declMember( "texcoord1", ast::type::Kind::eVec3F
				, ast::type::NotArray );
			result->declMember( "texcoord2", ast::type::Kind::eVec3F
				, ast::type::NotArray );
			result->declMember( "texcoord3", ast::type::Kind::eVec3F
				, ast::type::NotArray );
			result->declMember( "colour", ast::type::Kind::eVec3F
				, ast::type::NotArray );
			result->declMember( "passMasks", ast::type::Kind::eVec4U
				, ast::type::NotArray );
			//@}
			/**
			*	Velocity
			*/
			//@{
			result->declMember( "velocity", ast::type::Kind::eVec3F
				, ast::type::NotArray );
			//@}
			/**
			*	Instantiation
			*/
			//@{
			result->declMember( "objectIds", ast::type::Kind::eVec4U
				, ast::type::NotArray );
			//@}
		}

		return result;
	}

	//*****************************************************************************************

	template< typename TexcoordT, ast::var::Flag FlagT >
	RasterizerSurfaceT< TexcoordT, FlagT >::RasterizerSurfaceT( sdw::ShaderWriter & writer
		, sdw::expr::ExprPtr expr
		, bool enabled )
		: StructInstance{ writer, std::move( expr ), enabled }
		, worldPosition{ this->getMember< sdw::Vec4 >( "worldPosition", true ) }
		, viewPosition{ this->getMember< sdw::Vec4 >( "viewPosition", true ) }
		, curPosition{ this->getMember< sdw::Vec4 >( "curPosition", true ) }
		, prvPosition{ this->getMember< sdw::Vec4 >( "prvPosition", true ) }
		, tangentSpaceFragPosition{ this->getMember< sdw::Vec3 >( "tangentSpaceFragPosition", true ) }
		, tangentSpaceViewPosition{ this->getMember< sdw::Vec3 >( "tangentSpaceViewPosition", true ) }
		, normal{ this->getMember< sdw::Vec3 >( "normal", true ) }
		, tangent{ this->getMember< sdw::Vec3 >( "tangent", true ) }
		, bitangent{ this->getMember< sdw::Vec3 >( "bitangent", true ) }
		, texture0{ this->getMember< TexcoordT >( "texcoord0", true ) }
		, texture1{ this->getMember< TexcoordT >( "texcoord1", true ) }
		, texture2{ this->getMember< TexcoordT >( "texcoord2", true ) }
		, texture3{ this->getMember< TexcoordT >( "texcoord3", true ) }
		, colour{ this->getMember< sdw::Vec3 >( "colour", true ) }
		, passMultipliers{ this->getMemberArray< sdw::Vec4 >( "passMultipliers", true ) }
		, nodeId{ this->getMember< sdw::UInt >( "nodeId", true ) }
		, vertexId{ this->getMember< sdw::UInt >( "vertexId", true ) }
	{
	}

	template< typename TexcoordT, ast::var::Flag FlagT >
	ast::type::IOStructPtr RasterizerSurfaceT< TexcoordT, FlagT >::makeIOType( ast::type::TypesCache & cache
		, PipelineFlags const & flags )
	{
		auto result = cache.getIOStruct( ast::type::MemoryLayout::eC
			, "C3D_" + ( FlagT == sdw::var::Flag::eShaderOutput
				? std::string{ "Output" }
				: std::string{ "Input" } ) + "RasterSurface"
			, FlagT );

		if ( result->empty() )
		{
			auto texType = TexcoordT::makeType( cache );
			uint32_t index = 0u;
			result->declMember( "worldPosition", ast::type::Kind::eVec4F
				, ast::type::NotArray
				, ( flags.usesWorldSpace() ? index++ : 0 )
				, flags.usesWorldSpace() );
			result->declMember( "viewPosition", ast::type::Kind::eVec4F
				, ast::type::NotArray
				, ( flags.usesViewSpace() ? index++ : 0 )
				, flags.usesViewSpace() );
			result->declMember( "curPosition", ast::type::Kind::eVec4F
				, ast::type::NotArray
				, ( flags.writeVelocity() ? index++ : 0 )
				, flags.writeVelocity() );
			result->declMember( "prvPosition", ast::type::Kind::eVec4F
				, ast::type::NotArray
				, ( flags.writeVelocity() ? index++ : 0 )
				, flags.writeVelocity() );
			result->declMember( "tangentSpaceFragPosition"
				, ast::type::Kind::eVec3F
				, ast::type::NotArray
				, ( flags.enableParallaxOcclusionMapping() ? index++ : 0 )
				, flags.enableParallaxOcclusionMapping() );
			result->declMember( "tangentSpaceViewPosition"
				, ast::type::Kind::eVec3F
				, ast::type::NotArray
				, ( flags.enableParallaxOcclusionMapping() ? index++ : 0 )
				, flags.enableParallaxOcclusionMapping() );
			result->declMember( "normal"
				, ast::type::Kind::eVec3F
				, ast::type::NotArray
				, ( flags.enableNormal() ? index++ : 0 )
				, flags.enableNormal() );
			result->declMember( "tangent", ast::type::Kind::eVec3F
				, ast::type::NotArray
				, ( flags.enableTangentSpace() ? index++ : 0 )
				, flags.enableTangentSpace() );
			result->declMember( "bitangent", ast::type::Kind::eVec3F
				, ast::type::NotArray
				, ( flags.enableTangentSpace() ? index++ : 0 )
				, flags.enableTangentSpace() );
			result->declMember( "texcoord0", texType
				, ast::type::NotArray
				, ( flags.enableTexcoord0() ? index++ : 0 )
				, flags.enableTexcoord0() );
			result->declMember( "texcoord1", texType
				, ast::type::NotArray
				, ( flags.enableTexcoord1() ? index++ : 0 )
				, flags.enableTexcoord1() );
			result->declMember( "texcoord2", texType
				, ast::type::NotArray
				, ( flags.enableTexcoord2() ? index++ : 0 )
				, flags.enableTexcoord2() );
			result->declMember( "texcoord3", texType
				, ast::type::NotArray
				, ( flags.enableTexcoord3() ? index++ : 0 )
				, flags.enableTexcoord3() );
			result->declMember( "colour", ast::type::Kind::eVec3F
				, ast::type::NotArray
				, ( flags.enableColours() ? index++ : 0 )
				, flags.enableColours() );
			result->declMember( "passMultipliers", ast::type::Kind::eVec4F
				, 4u
				, ( flags.enablePassMasks() ? index : 0 )
				, flags.enablePassMasks() );

			if ( flags.enablePassMasks() )
			{
				index += 4u;
			}

			result->declMember( "nodeId", ast::type::Kind::eUInt
				, ast::type::NotArray
				, index++ );
			result->declMember( "vertexId", ast::type::Kind::eUInt
				, ast::type::NotArray
				, ( flags.enableVertexID() ? index : 0 )
				, flags.enableVertexID() );
		}

		return result;
	}

	template< typename TexcoordT, ast::var::Flag FlagT >
	ast::type::BaseStructPtr RasterizerSurfaceT< TexcoordT, FlagT >::makeType( ast::type::TypesCache & cache )
	{
		auto result = cache.getStruct( ast::type::MemoryLayout::eC, "C3D_FragSurface" );

		if ( result->empty() )
		{
			auto texType = TexcoordT::makeType( cache );
			result->declMember( "worldPosition", ast::type::Kind::eVec4F
				, ast::type::NotArray );
			result->declMember( "viewPosition", ast::type::Kind::eVec4F
				, ast::type::NotArray );
			result->declMember( "curPosition", ast::type::Kind::eVec4F
				, ast::type::NotArray );
			result->declMember( "prvPosition", ast::type::Kind::eVec4F
				, ast::type::NotArray );
			result->declMember( "tangentSpaceFragPosition", ast::type::Kind::eVec3F
				, ast::type::NotArray );
			result->declMember( "tangentSpaceViewPosition", ast::type::Kind::eVec3F
				, ast::type::NotArray );
			result->declMember( "normal", ast::type::Kind::eVec3F
				, ast::type::NotArray );
			result->declMember( "tangent", ast::type::Kind::eVec3F
				, ast::type::NotArray );
			result->declMember( "bitangent", ast::type::Kind::eVec3F
				, ast::type::NotArray );
			result->declMember( "texcoord0", texType
				, ast::type::NotArray );
			result->declMember( "texcoord1", texType
				, ast::type::NotArray );
			result->declMember( "texcoord2", texType
				, ast::type::NotArray );
			result->declMember( "texcoord3", texType
				, ast::type::NotArray );
			result->declMember( "colour", ast::type::Kind::eVec3F
				, ast::type::NotArray );
			result->declMember( "passMultipliers", ast::type::Kind::eVec4F
				, 4u );
			result->declMember( "nodeId", ast::type::Kind::eInt
				, ast::type::NotArray );
			result->declMember( "vertexId", ast::type::Kind::eInt
				, ast::type::NotArray );
		}

		return result;
	}

	template< typename TexcoordT, ast::var::Flag FlagT >
	void RasterizerSurfaceT< TexcoordT, FlagT >::computeVelocity( MatrixData const & matrixData
		, sdw::Vec4 & csCurPos
		, sdw::Vec4 & csPrvPos )
	{
		// Convert the jitter from non-homogeneous coordinates to homogeneous
		// coordinates and add it:
		// (note that for providing the jitter in non-homogeneous projection space,
		//  pixel coordinates (screen space) need to multiplied by two in the C++
		//  code)
		matrixData.jitter( csCurPos );
		matrixData.jitter( csPrvPos );

		curPosition = vec4( csCurPos.xyw(), 1.0_f );
		prvPosition = vec4( csPrvPos.xyw(), 1.0_f );

		// Positions in projection space are in [-1, 1] range, while texture
		// coordinates are in [0, 1] range. So, we divide by 2 to get velocities in
		// the scale (and flip the y axis):
		curPosition.xy() *= vec2( 0.5_f, -0.5_f );
		prvPosition.xy() *= vec2( 0.5_f, -0.5_f );
	}

	template< typename TexcoordT, ast::var::Flag FlagT >
	void RasterizerSurfaceT< TexcoordT, FlagT >::computeTangentSpace( PipelineFlags const & flags
		, sdw::Vec3 const & cameraPosition
		, sdw::Vec3 const & worldPos
		, sdw::Vec3 const & nml
		, sdw::Vec3 const & tan )
	{
		if ( flags.enableTangentSpace() )
		{
			normal = nml;
			tangent = tan;
			tangent = normalize( sdw::fma( -normal, vec3( dot( tangent, normal ) ), tangent ) );
			bitangent = cross( normal, tangent );

			if ( flags.hasInvertNormals() )
			{
				normal = -normal;
				tangent = -tangent;
				bitangent = -bitangent;
			}

			if ( !tangentSpaceFragPosition.getExpr()->isDummy() )
			{
				CU_Require( !worldPos.getExpr()->isDummy() );
				CU_Require( !tangentSpaceViewPosition.getExpr()->isDummy() );
				auto tbn = getWriter()->declLocale( "tbn"
					, transpose( mat3( tangent, bitangent, normal ) ) );
				tangentSpaceFragPosition = tbn * worldPos;
				tangentSpaceViewPosition = tbn * cameraPosition.xyz();
			}
		}
		else if ( normal.isEnabled() )
		{
			normal = nml;

			if ( flags.hasInvertNormals() )
			{
				normal = -normal;
			}
		}
	}

	template< typename TexcoordT, ast::var::Flag FlagT >
	void RasterizerSurfaceT< TexcoordT, FlagT >::computeTangentSpace( PipelineFlags const & flags
		, sdw::Vec3 const & cameraPosition
		, sdw::Vec3 const & worldPos
		, sdw::Mat3 const & mtx
		, sdw::Vec3 const & nml
		, sdw::Vec3 const & tan )
	{
		return computeTangentSpace( flags
			, cameraPosition
			, worldPos
			, normalize( mtx * nml )
			, normalize( mtx * tan ) );
	}

	template< typename TexcoordT, ast::var::Flag FlagT >
	void RasterizerSurfaceT< TexcoordT, FlagT >::computeTangentSpace( PipelineFlags const & flags
		, sdw::Vec3 const & cameraPosition
		, sdw::Vec3 const & worldPos
		, sdw::Vec3 const & nml
		, sdw::Vec3 const & tan
		, sdw::Vec3 const & bin )
	{
		if ( flags.enableTangentSpace() )
		{
			normal = nml;
			tangent = tan;
			bitangent = bin;

			if ( !tangentSpaceFragPosition.getExpr()->isDummy() )
			{
				CU_Require( !worldPos.getExpr()->isDummy() );
				CU_Require( !tangentSpaceViewPosition.getExpr()->isDummy() );
				auto tbn = getWriter()->declLocale( "tbn"
					, transpose( mat3( tangent, bitangent, normal ) ) );
				tangentSpaceFragPosition = tbn * worldPos;
				tangentSpaceViewPosition = tbn * cameraPosition.xyz();
			}
		}
		else
		{
			normal = nml;
		}
	}

	template< typename TexcoordT, ast::var::Flag FlagT >
	sdw::Vec2 RasterizerSurfaceT< TexcoordT, FlagT >::getVelocity()const
	{
		return ( curPosition.xy() / curPosition.z() ) - ( prvPosition.xy() / prvPosition.z() );
	}

	//*****************************************************************************************

#define DefineSsbo( Name, Type, Enable )\
		m_##Name##Buffer{ writer\
			, #Name + std::string{ "Buffer" }\
			, binding++\
			, set\
			, ast::type::MemoryLayout::eStd430\
			, Enable }\
		, ##Name{ getMemberArray< Type >( m_##Name##Buffer, #Name, Enable ) }

	template< typename PositionT >
	ComputeSurfaceT< PositionT >::ComputeSurfaceT( sdw::ShaderWriter & writer
		, PipelineFlags const & flags
		, uint32_t stride
		, uint32_t binding
		, uint32_t set )
		: DefineSsbo( indices, sdw::UInt, flags.enableIndices() )
		, DefineSsbo( positions, PositionT, flags.enablePosition() )
		, DefineSsbo( normals, sdw::Vec4, flags.enableNormal() )
		, DefineSsbo( tangents, sdw::Vec4, flags.enableTangentSpace() )
		, DefineSsbo( texcoords0, sdw::Vec4, flags.enableTexcoord0() && ( stride == 0u ) )
		, DefineSsbo( texcoords1, sdw::Vec4, flags.enableTexcoord1() )
		, DefineSsbo( texcoords2, sdw::Vec4, flags.enableTexcoord2() )
		, DefineSsbo( texcoords3, sdw::Vec4, flags.enableTexcoord3() )
		, DefineSsbo( colours, sdw::Vec4, flags.enableColours() )
		, DefineSsbo( velocities, sdw::Vec4, flags.enableVelocity() )
		, DefineSsbo( passMasks, sdw::UVec4, flags.enablePassMasks() )
	{
	}

#undef DefineSsbo

	//*****************************************************************************************
}
