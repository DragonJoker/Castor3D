#include "Castor3D/Shader/Shaders/GlslSurface.hpp"

#include "Castor3D/Render/RenderNodesPass.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/MorphingUbo.hpp"

#include <ShaderWriter/Writer.hpp>
#include <ShaderWriter/Intrinsics/Intrinsics.hpp>

namespace castor3d::shader
{
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
		, SubmeshFlags submeshFlags
		, ProgramFlags programFlags
		, ShaderFlags shaderFlags
		, FilteredTextureFlags textureFlags
		, PassFlags passFlags
		, bool hasTextures )
	{
		hasTextures = hasTextures || checkFlag( programFlags, ProgramFlag::eForceTexCoords );
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
				, ( ( checkFlag( submeshFlags, SubmeshFlag::eNormals ) && checkFlag( shaderFlags, ShaderFlag::eNormal ) ) ? index++ : 0 )
				, ( checkFlag( submeshFlags, SubmeshFlag::eNormals ) && checkFlag( shaderFlags, ShaderFlag::eNormal ) ) );
			result->declMember( "tangent", ast::type::Kind::eVec3F
				, ast::type::NotArray
				, ( ( checkFlag( submeshFlags, SubmeshFlag::eTangents ) && checkFlag( shaderFlags, ShaderFlag::eTangentSpace ) ) ? index++ : 0 )
				, ( checkFlag( submeshFlags, SubmeshFlag::eTangents ) && checkFlag( shaderFlags, ShaderFlag::eTangentSpace ) ) );
			result->declMember( "texcoord0", ast::type::Kind::eVec3F
				, ast::type::NotArray
				, ( ( checkFlag( submeshFlags, SubmeshFlag::eTexcoords0 ) && hasTextures ) ? index++ : 0 )
				, checkFlag( submeshFlags, SubmeshFlag::eTexcoords0 ) && hasTextures );
			result->declMember( "texcoord1", ast::type::Kind::eVec3F
				, ast::type::NotArray
				, ( ( checkFlag( submeshFlags, SubmeshFlag::eTexcoords1 ) && hasTextures ) ? index++ : 0 )
				, checkFlag( submeshFlags, SubmeshFlag::eTexcoords1 ) && hasTextures );
			result->declMember( "texcoord2", ast::type::Kind::eVec3F
				, ast::type::NotArray
				, ( ( checkFlag( submeshFlags, SubmeshFlag::eTexcoords2 ) && hasTextures ) ? index++ : 0 )
				, checkFlag( submeshFlags, SubmeshFlag::eTexcoords2 ) && hasTextures );
			result->declMember( "texcoord3", ast::type::Kind::eVec3F
				, ast::type::NotArray
				, ( ( checkFlag( submeshFlags, SubmeshFlag::eTexcoords3 ) && hasTextures ) ? index++ : 0 )
				, checkFlag( submeshFlags, SubmeshFlag::eTexcoords3 ) && hasTextures );
			result->declMember( "colour", ast::type::Kind::eVec3F
				, ast::type::NotArray
				, ( checkFlag( submeshFlags, SubmeshFlag::eColours ) ? index++ : 0 )
				, checkFlag( submeshFlags, SubmeshFlag::eColours ) );
			result->declMember( "passMasks", ast::type::Kind::eVec4U
				, ast::type::NotArray
				, ( checkFlag( submeshFlags, SubmeshFlag::ePassMasks ) ? index++ : 0 )
				, checkFlag( submeshFlags, SubmeshFlag::ePassMasks ) );
			//@}
			/**
			*	Velocity
			*/
			//@{
			result->declMember( "velocity", ast::type::Kind::eVec3F
				, ast::type::NotArray
				, ( ( checkFlag( submeshFlags, SubmeshFlag::eVelocity ) && checkFlag( shaderFlags, ShaderFlag::eVelocity ) ) ? index++ : 0 )
				, checkFlag( submeshFlags, SubmeshFlag::eVelocity ) && checkFlag( shaderFlags, ShaderFlag::eVelocity ) );
			//@}
			/**
			*	Instantiation
			*/
			//@{
			result->declMember( "objectIds", ast::type::Kind::eVec4U
				, ast::type::NotArray
				, ( checkFlag( programFlags, ProgramFlag::eInstantiation ) ? index++ : 0 )
				, checkFlag( programFlags, ProgramFlag::eInstantiation ) );
			//@}
		}

		return result;
	}

	//*****************************************************************************************

	template< ast::var::Flag FlagT >
	FragmentSurfaceT< FlagT >::FragmentSurfaceT( sdw::ShaderWriter & writer
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
		, texture0{ this->getMember< sdw::Vec3 >( "texcoord0", true ) }
		, texture1{ this->getMember< sdw::Vec3 >( "texcoord1", true ) }
		, texture2{ this->getMember< sdw::Vec3 >( "texcoord2", true ) }
		, texture3{ this->getMember< sdw::Vec3 >( "texcoord3", true ) }
		, colour{ this->getMember< sdw::Vec3 >( "colour", true ) }
		, passMultipliers{ this->getMemberArray< sdw::Vec4 >( "passMultipliers", true ) }
		, nodeId{ this->getMember< sdw::Int >( "nodeId", true ) }
		, vertexId{ this->getMember< sdw::Int >( "vertexId", true ) }
	{
	}

	template< ast::var::Flag FlagT >
	ast::type::IOStructPtr FragmentSurfaceT< FlagT >::makeIOType( ast::type::TypesCache & cache
		, SubmeshFlags submeshFlags
		, ProgramFlags programFlags
		, ShaderFlags shaderFlags
		, FilteredTextureFlags textureFlags
		, PassFlags passFlags
		, bool hasTextures )
	{
		hasTextures = hasTextures || checkFlag( programFlags, ProgramFlag::eForceTexCoords );
		auto result = cache.getIOStruct( ast::type::MemoryLayout::eC
			, "C3D_" + ( FlagT == sdw::var::Flag::eShaderOutput
				? std::string{ "Output" }
				: std::string{ "Input" } ) + "FragSurface"
			, FlagT );

		if ( result->empty() )
		{
			bool hasParallaxOcclusionMapping = ( checkFlags( textureFlags, TextureFlag::eHeight ) != textureFlags.end() )
				&& ( checkFlag( passFlags, PassFlag::eParallaxOcclusionMappingOne )
					|| checkFlag( passFlags, PassFlag::eParallaxOcclusionMappingRepeat ) );
			uint32_t index = 0u;
			result->declMember( "worldPosition", ast::type::Kind::eVec4F
				, ast::type::NotArray
				, ( checkFlag( shaderFlags, ShaderFlag::eWorldSpace ) ? index++ : 0 )
				, checkFlag( shaderFlags, ShaderFlag::eWorldSpace ) );
			result->declMember( "viewPosition", ast::type::Kind::eVec4F
				, ast::type::NotArray
				, ( checkFlag( shaderFlags, ShaderFlag::eViewSpace ) ? index++ : 0 )
				, checkFlag( shaderFlags, ShaderFlag::eViewSpace ) );
			result->declMember( "curPosition", ast::type::Kind::eVec4F
				, ast::type::NotArray
				, ( checkFlag( shaderFlags, ShaderFlag::eVelocity ) ? index++ : 0 )
				, checkFlag( shaderFlags, ShaderFlag::eVelocity ) );
			result->declMember( "prvPosition", ast::type::Kind::eVec4F
				, ast::type::NotArray
				, ( checkFlag( shaderFlags, ShaderFlag::eVelocity ) ? index++ : 0 )
				, checkFlag( shaderFlags, ShaderFlag::eVelocity ) );
			result->declMember( "tangentSpaceFragPosition"
				, ast::type::Kind::eVec3F
				, ast::type::NotArray
				, ( ( checkFlag( submeshFlags, SubmeshFlag::eTangents ) && checkFlag( shaderFlags, ShaderFlag::eTangentSpace ) && hasParallaxOcclusionMapping ) ? index++ : 0 )
				, ( checkFlag( submeshFlags, SubmeshFlag::eTangents ) && checkFlag( shaderFlags, ShaderFlag::eTangentSpace ) && hasParallaxOcclusionMapping ) );
			result->declMember( "tangentSpaceViewPosition"
				, ast::type::Kind::eVec3F
				, ast::type::NotArray
				, ( ( checkFlag( submeshFlags, SubmeshFlag::eTangents ) && checkFlag(shaderFlags, ShaderFlag::eTangentSpace ) && hasParallaxOcclusionMapping ) ? index++ : 0 )
				, ( checkFlag( submeshFlags, SubmeshFlag::eTangents ) && checkFlag( shaderFlags, ShaderFlag::eTangentSpace ) && hasParallaxOcclusionMapping ) );
			result->declMember( "normal"
				, ast::type::Kind::eVec3F
				, ast::type::NotArray
				, ( ( checkFlag( submeshFlags, SubmeshFlag::eNormals ) && checkFlag( shaderFlags, ShaderFlag::eNormal ) ) ? index++ : 0 )
				, ( checkFlag( submeshFlags, SubmeshFlag::eNormals ) && checkFlag( shaderFlags, ShaderFlag::eNormal ) ) );
			result->declMember( "tangent", ast::type::Kind::eVec3F
				, ast::type::NotArray
				, ( ( checkFlag( submeshFlags, SubmeshFlag::eTangents ) && checkFlag( shaderFlags, ShaderFlag::eTangentSpace ) ) ? index++ : 0 )
				, ( checkFlag( submeshFlags, SubmeshFlag::eTangents ) && checkFlag( shaderFlags, ShaderFlag::eTangentSpace ) ) );
			result->declMember( "bitangent", ast::type::Kind::eVec3F
				, ast::type::NotArray
				, ( ( checkFlag( submeshFlags, SubmeshFlag::eTangents ) && checkFlag( shaderFlags, ShaderFlag::eTangentSpace ) ) ? index++ : 0 )
				, ( checkFlag( submeshFlags, SubmeshFlag::eTangents ) && checkFlag( shaderFlags, ShaderFlag::eTangentSpace ) ) );
			result->declMember( "texcoord0", ast::type::Kind::eVec3F
				, ast::type::NotArray
				, ( ( checkFlag( submeshFlags, SubmeshFlag::eTexcoords0 ) && hasTextures ) ? index++ : 0 )
				, checkFlag( submeshFlags, SubmeshFlag::eTexcoords0 ) && hasTextures );
			result->declMember( "texcoord1", ast::type::Kind::eVec3F
				, ast::type::NotArray
				, ( ( checkFlag( submeshFlags, SubmeshFlag::eTexcoords1 ) && hasTextures ) ? index++ : 0 )
				, checkFlag( submeshFlags, SubmeshFlag::eTexcoords1 ) && hasTextures );
			result->declMember( "texcoord2", ast::type::Kind::eVec3F
				, ast::type::NotArray
				, ( ( checkFlag( submeshFlags, SubmeshFlag::eTexcoords2 ) && hasTextures ) ? index++ : 0 )
				, checkFlag( submeshFlags, SubmeshFlag::eTexcoords2 ) && hasTextures );
			result->declMember( "texcoord3", ast::type::Kind::eVec3F
				, ast::type::NotArray
				, ( ( checkFlag( submeshFlags, SubmeshFlag::eTexcoords3 ) && hasTextures ) ? index++ : 0 )
				, checkFlag( submeshFlags, SubmeshFlag::eTexcoords3 ) && hasTextures );
			result->declMember( "colour", ast::type::Kind::eVec3F
				, ast::type::NotArray
				, ( checkFlag( submeshFlags, SubmeshFlag::eColours ) ? index++ : 0 )
				, checkFlag( submeshFlags, SubmeshFlag::eColours ) );
			result->declMember( "passMultipliers", ast::type::Kind::eVec4F
				, 4u
				, ( checkFlag( submeshFlags, SubmeshFlag::ePassMasks ) ? index : 0 )
				, checkFlag( submeshFlags, SubmeshFlag::ePassMasks ) );
			if ( checkFlag( submeshFlags, SubmeshFlag::ePassMasks ) )
			{
				index += 4u;
			}
			result->declMember( "nodeId", ast::type::Kind::eInt
				, ast::type::NotArray
				, index++ );
			result->declMember( "vertexId", ast::type::Kind::eInt
				, ast::type::NotArray
				, ( checkFlag( programFlags, ProgramFlag::eDepthPass ) ? index++ : 0 )
				, checkFlag( programFlags, ProgramFlag::eDepthPass ) );
		}

		return result;
	}

	template< ast::var::Flag FlagT >
	ast::type::BaseStructPtr FragmentSurfaceT< FlagT >::makeType( ast::type::TypesCache & cache )
	{
		auto result = cache.getStruct( ast::type::MemoryLayout::eC, "C3D_FragSurface" );

		if ( result->empty() )
		{
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
			result->declMember( "passMultipliers", ast::type::Kind::eVec4F
				, 4u );
			result->declMember( "nodeId", ast::type::Kind::eInt
				, ast::type::NotArray );
			result->declMember( "vertexId", ast::type::Kind::eInt
				, ast::type::NotArray );
		}

		return result;
	}

	template< ast::var::Flag FlagT >
	void FragmentSurfaceT< FlagT >::computeVelocity( MatrixData const & matrixData
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

	template< ast::var::Flag FlagT >
	void FragmentSurfaceT< FlagT >::computeTangentSpace( SubmeshFlags submeshFlags
		, ProgramFlags programFlags
		, sdw::Vec3 const & cameraPosition
		, sdw::Vec3 const & worldPos
		, sdw::Vec3 const & nml
		, sdw::Vec3 const & tan )
	{
		if ( checkFlag( submeshFlags, SubmeshFlag::eTangents ) )
		{
			normal = nml;
			tangent = tan;
			tangent = normalize( sdw::fma( -normal, vec3( dot( tangent, normal ) ), tangent ) );
			bitangent = cross( normal, tangent );

			if ( checkFlag( programFlags, ProgramFlag::eInvertNormals ) )
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
			normal = nml.xyz();

			if ( checkFlag( programFlags, ProgramFlag::eInvertNormals ) )
			{
				normal = -normal;
			}
		}
	}

	template< ast::var::Flag FlagT >
	void FragmentSurfaceT< FlagT >::computeTangentSpace( SubmeshFlags submeshFlags
		, ProgramFlags programFlags
		, sdw::Vec3 const & cameraPosition
		, sdw::Vec3 const & worldPos
		, sdw::Mat3 const & mtx
		, sdw::Vec3 const & nml
		, sdw::Vec3 const & tan )
	{
		return computeTangentSpace( submeshFlags
			, programFlags
			, cameraPosition
			, worldPos
			, normalize( mtx * nml )
			, normalize( mtx * tan ) );
	}

	template< ast::var::Flag FlagT >
	void FragmentSurfaceT< FlagT >::computeTangentSpace( SubmeshFlags submeshFlags
		, sdw::Vec3 const & cameraPosition
		, sdw::Vec3 const & worldPos
		, sdw::Vec3 const & nml
		, sdw::Vec3 const & tan
		, sdw::Vec3 const & bin )
	{
		if ( checkFlag( submeshFlags, SubmeshFlag::eTangents ) )
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

	template< ast::var::Flag FlagT >
	sdw::Vec2 FragmentSurfaceT< FlagT >::getVelocity()const
	{
		return ( curPosition.xy() / curPosition.z() ) - ( prvPosition.xy() / prvPosition.z() );
	}

	//*****************************************************************************************
}
