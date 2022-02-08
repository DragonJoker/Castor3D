#include "Castor3D/Shader/Shaders/GlslSurface.hpp"

#include "Castor3D/Render/RenderPass.hpp"
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
		, clipPosition{ getMember< sdw::Vec2 >( "clipPosition" ) }
		, viewPosition{ getMember< sdw::Vec3 >( "viewPosition" ) }
		, worldPosition{ getMember< sdw::Vec3 >( "worldPosition" ) }
		, worldNormal{ getMember< sdw::Vec3 >( "worldNormal" ) }
		, texCoord{ getMember< sdw::Vec3 >( "texCoord" ) }
	{
	}

	template< ast::var::Flag FlagT >
	void SurfaceT< FlagT >::create( sdw::Vec2 clip
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
	void SurfaceT< FlagT >::create( sdw::Vec2 clip
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
		create( sdw::vec2( 0.0_f )
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
			result->declMember( "clipPosition", ast::type::Kind::eVec2F );
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
		, position{ this->getMember< sdw::Vec4 >( "position", true ) }
		, normal{ this->getMember< sdw::Vec3 >( "normal", true ) }
		, tangent{ this->getMember< sdw::Vec3 >( "tangent", true ) }
		, texture0{ this->getMember< sdw::Vec3 >( "texcoord0", true ) }
		, texture1{ this->getMember< sdw::Vec3 >( "texcoord1", true ) }
		, boneIds0{ this->getMember< sdw::IVec4 >( "boneIds0", true ) }
		, boneIds1{ this->getMember< sdw::IVec4 >( "boneIds1", true ) }
		, boneWeights0{ this->getMember< sdw::Vec4 >( "boneWeights0", true ) }
		, boneWeights1{ this->getMember< sdw::Vec4 >( "boneWeights1", true ) }
		, transform{ this->getMember< sdw::Mat4 >( "transform", true ) }
		, textures0{ this->getMember< sdw::UVec4 >( "textures0", true ) }
		, textures1{ this->getMember< sdw::UVec4 >( "textures1", true ) }
		, textures{ this->getMember< sdw::Int >( "textures", true ) }
		, material{ this->getMember< sdw::Int >( "material", true ) }
		, nodeId{ this->getMember< sdw::Int >( "nodeId", true ) }
		, position2{ this->getMember< sdw::Vec4 >( "position2", true ) }
		, normal2{ this->getMember< sdw::Vec3 >( "normal2", true ) }
		, tangent2{ this->getMember< sdw::Vec3 >( "tangent2", true ) }
		, texture2{ this->getMember< sdw::Vec3 >( "texcoord2", true ) }
	{
	}

	template< ast::var::Flag FlagT >
	ast::type::IOStructPtr VertexSurfaceT< FlagT >::makeIOType( ast::type::TypesCache & cache
		, ProgramFlags programFlags
		, ShaderFlags shaderFlags
		, FilteredTextureFlags textureFlags
		, PassFlags passFlags
		, bool hasTextures )
	{
		hasTextures |= checkFlag( programFlags, ProgramFlag::eForceTexCoords );
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
				, ( checkFlag( shaderFlags, ShaderFlag::eNormal ) ? index++ : 0 )
				, checkFlag( shaderFlags, ShaderFlag::eNormal ) );
			result->declMember( "tangent", ast::type::Kind::eVec3F
				, ast::type::NotArray
				, ( checkFlag( shaderFlags, ShaderFlag::eTangentSpace ) ? index++ : 0 )
				, checkFlag( shaderFlags, ShaderFlag::eTangentSpace ) );
			result->declMember( "texcoord0", ast::type::Kind::eVec3F
				, ast::type::NotArray
				, ( hasTextures ? index++ : 0 )
				, hasTextures );
			//@}
			/**
			*	Morphing
			*/
			//@{
			result->declMember( "position2", ast::type::Kind::eVec4F
				, ast::type::NotArray
				, ( checkFlag( programFlags, ProgramFlag::eMorphing ) ? index++ : 0 )
				, checkFlag( programFlags, ProgramFlag::eMorphing ) );
			result->declMember( "normal2", ast::type::Kind::eVec3F
				, ast::type::NotArray
				, ( ( checkFlag( programFlags, ProgramFlag::eMorphing ) && checkFlag( shaderFlags, ShaderFlag::eNormal ) ) ? index++ : 0 )
				, checkFlag( programFlags, ProgramFlag::eMorphing ) && checkFlag( shaderFlags, ShaderFlag::eNormal ) );
			result->declMember( "tangent2", ast::type::Kind::eVec3F
				, ast::type::NotArray
				, ( ( checkFlag( programFlags, ProgramFlag::eMorphing ) && checkFlag( shaderFlags, ShaderFlag::eTangentSpace ) ) ? index++ : 0 )
				, checkFlag( programFlags, ProgramFlag::eMorphing ) && checkFlag( shaderFlags, ShaderFlag::eTangentSpace ) );
			result->declMember( "texcoord2", ast::type::Kind::eVec3F
				, ast::type::NotArray
				, ( ( checkFlag( programFlags, ProgramFlag::eMorphing ) && hasTextures ) ? index++ : 0 )
				, checkFlag( programFlags, ProgramFlag::eMorphing ) && hasTextures );
			//@}
			/**
			*	Instantiation
			*/
			//@{
			result->declMember( "transform", ast::type::Kind::eMat4x4F
				, ast::type::NotArray
				, ( checkFlag( programFlags, ProgramFlag::eInstantiation ) ? index : 0 )
				, checkFlag( programFlags, ProgramFlag::eInstantiation ) );
			index = checkFlag( programFlags, ProgramFlag::eInstantiation )
				? index + 4u
				: index;
			result->declMember( "textures0", ast::type::Kind::eVec4U
				, ast::type::NotArray
				, ( checkFlag( programFlags, ProgramFlag::eInstantiation ) ? index++ : 0 )
				, checkFlag( programFlags, ProgramFlag::eInstantiation ) );
			result->declMember( "textures1", ast::type::Kind::eVec4U
				, ast::type::NotArray
				, ( checkFlag( programFlags, ProgramFlag::eInstantiation ) ? index++ : 0 )
				, checkFlag( programFlags, ProgramFlag::eInstantiation ) );
			result->declMember( "textures", ast::type::Kind::eInt
				, ast::type::NotArray
				, ( checkFlag( programFlags, ProgramFlag::eInstantiation ) ? index++ : 0 )
				, checkFlag( programFlags, ProgramFlag::eInstantiation ) );
			result->declMember( "material", ast::type::Kind::eInt
				, ast::type::NotArray
				, ( checkFlag( programFlags, ProgramFlag::eInstantiation ) ? index++ : 0 )
				, checkFlag( programFlags, ProgramFlag::eInstantiation ) );
			result->declMember( "nodeId", ast::type::Kind::eInt
				, ast::type::NotArray
				, ( ( checkFlag( programFlags, ProgramFlag::eInstantiation ) && checkFlag( shaderFlags, ShaderFlag::eNodeId ) ) ? index++ : 0 )
				, ( checkFlag( programFlags, ProgramFlag::eInstantiation ) && checkFlag( shaderFlags, ShaderFlag::eNodeId ) ) );
			//@}
			/**
			*	Skinning
			*/
			//@{
			result->declMember( "boneIds0", ast::type::Kind::eVec4I
				, ast::type::NotArray
				, ( checkFlag( programFlags, ProgramFlag::eSkinning ) ? index++ : 0 )
				, checkFlag( programFlags, ProgramFlag::eSkinning ) );
			result->declMember( "boneIds1", ast::type::Kind::eVec4I
				, ast::type::NotArray
				, ( checkFlag( programFlags, ProgramFlag::eSkinning ) ? index++ : 0 )
				, checkFlag( programFlags, ProgramFlag::eSkinning ) );
			result->declMember( "boneWeights0", ast::type::Kind::eVec4F
				, ast::type::NotArray
				, ( checkFlag( programFlags, ProgramFlag::eSkinning ) ? index++ : 0 )
				, checkFlag( programFlags, ProgramFlag::eSkinning ) );
			result->declMember( "boneWeights1", ast::type::Kind::eVec4F
				, ast::type::NotArray
				, ( checkFlag( programFlags, ProgramFlag::eSkinning ) ? index++ : 0 )
				, checkFlag( programFlags, ProgramFlag::eSkinning ) );
			//@}
			/**
			*	Secondary UV
			*/
			//@{
			result->declMember( "texcoord1", ast::type::Kind::eVec3F
				, ast::type::NotArray
				, ( ( checkFlag( programFlags, ProgramFlag::eSecondaryUV ) && hasTextures ) ? index++ : 0 )
				, checkFlag( programFlags, ProgramFlag::eSecondaryUV ) && hasTextures );
			//@}
		}

		return result;
	}

	template< ast::var::Flag FlagT >
	void VertexSurfaceT< FlagT >::morph( MorphingData const & morphing
		, sdw::Vec4 & pos
		, sdw::Vec3 & uvw )const
	{
		morphing.morph( pos, position2
			, uvw, texture2 );
	}

	template< ast::var::Flag FlagT >
	void VertexSurfaceT< FlagT >::morph( MorphingData const & morphing
		, sdw::Vec4 & pos
		, sdw::Vec4 & nml
		, sdw::Vec3 & uvw )const
	{
		morphing.morph( pos, position2
			, nml, normal2
			, uvw, texture2 );
	}

	template< ast::var::Flag FlagT >
	void VertexSurfaceT< FlagT >::morph( MorphingData const & morphing
		, sdw::Vec4 & pos
		, sdw::Vec4 & nml
		, sdw::Vec4 & tan
		, sdw::Vec3 & uvw )const
	{
		morphing.morph( pos, position2
			, nml, normal2
			, tan, tangent2
			, uvw, texture2 );
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
		, instance{ this->getMember< sdw::UInt >( "instance", true ) }
		, textures0{ this->getMember< sdw::UVec4 >( "textures0", true ) }
		, textures1{ this->getMember< sdw::UVec4 >( "textures1", true ) }
		, textures{ this->getMember< sdw::Int >( "textures", true ) }
		, material{ this->getMember< sdw::UInt >( "material", true ) }
		, nodeId{ this->getMember< sdw::Int >( "nodeId", true ) }
	{
	}

	template< ast::var::Flag FlagT >
	ast::type::IOStructPtr FragmentSurfaceT< FlagT >::makeIOType( ast::type::TypesCache & cache
		, ProgramFlags programFlags
		, ShaderFlags shaderFlags
		, FilteredTextureFlags textureFlags
		, PassFlags passFlags
		, bool hasTextures )
	{
		hasTextures |= checkFlag( programFlags, ProgramFlag::eForceTexCoords );
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
				, ( ( checkFlag( shaderFlags, ShaderFlag::eTangentSpace ) && hasParallaxOcclusionMapping ) ? index++ : 0 )
				, ( checkFlag( shaderFlags, ShaderFlag::eTangentSpace ) && hasParallaxOcclusionMapping ) );
			result->declMember( "tangentSpaceViewPosition"
				, ast::type::Kind::eVec3F
				, ast::type::NotArray
				, ( ( checkFlag(shaderFlags, ShaderFlag::eTangentSpace ) && hasParallaxOcclusionMapping ) ? index++ : 0 )
				, ( checkFlag( shaderFlags, ShaderFlag::eTangentSpace ) && hasParallaxOcclusionMapping ) );
			result->declMember( "normal"
				, ast::type::Kind::eVec3F
				, ast::type::NotArray
				, ( checkFlag( shaderFlags, ShaderFlag::eNormal ) ? index++ : 0 )
				, checkFlag( shaderFlags, ShaderFlag::eNormal ) );
			result->declMember( "tangent", ast::type::Kind::eVec3F
				, ast::type::NotArray
				, ( checkFlag( shaderFlags, ShaderFlag::eTangentSpace ) ? index++ : 0 )
				, checkFlag( shaderFlags, ShaderFlag::eTangentSpace ) );
			result->declMember( "bitangent", ast::type::Kind::eVec3F
				, ast::type::NotArray
				, ( checkFlag( shaderFlags, ShaderFlag::eTangentSpace ) ? index++ : 0 )
				, checkFlag( shaderFlags, ShaderFlag::eTangentSpace ) );
			result->declMember( "texcoord0", ast::type::Kind::eVec3F
				, ast::type::NotArray
				, ( hasTextures ? index++ : 0 )
				, hasTextures );
			result->declMember( "texcoord1", ast::type::Kind::eVec3F
				, ast::type::NotArray
				, ( ( checkFlag( programFlags, ProgramFlag::eSecondaryUV ) && hasTextures ) ? index++ : 0 )
				, checkFlag( programFlags, ProgramFlag::eSecondaryUV ) && hasTextures );
			result->declMember( "instance"
				, ast::type::Kind::eUInt
				, ast::type::NotArray
				, ( checkFlag( programFlags, ProgramFlag::ePicking ) ? index++ : 0 )
				, checkFlag( programFlags, ProgramFlag::ePicking ) );
			result->declMember( "textures0", ast::type::Kind::eVec4U
				, ast::type::NotArray
				, index++ );
			result->declMember( "textures1", ast::type::Kind::eVec4U
				, ast::type::NotArray
				, index++ );
			result->declMember( "textures", ast::type::Kind::eInt
				, ast::type::NotArray
				, index++ );
			result->declMember( "material", ast::type::Kind::eUInt
				, ast::type::NotArray
				, index++ );
			result->declMember( "nodeId"
				, ast::type::Kind::eInt
				, ast::type::NotArray
				, ( checkFlag( shaderFlags, ShaderFlag::eNodeId ) ? index++ : 0 )
				, checkFlag( shaderFlags, ShaderFlag::eNodeId ) );
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
	void FragmentSurfaceT< FlagT >::computeTangentSpace( ProgramFlags programFlags
		, sdw::Vec3 const & cameraPosition
		, sdw::Vec3 const & worldPos
		, sdw::Mat3 const & mtx
		, sdw::Vec4 const & nml
		, sdw::Vec4 const & tan )
	{
		CU_Require( !normal.getExpr()->isDummy() );
		CU_Require( !tangent.getExpr()->isDummy() );
		CU_Require( !bitangent.getExpr()->isDummy() );
		normal = normalize( mtx * nml.xyz() );
		tangent = normalize( mtx * tan.xyz() );
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

	template< ast::var::Flag FlagT >
	void FragmentSurfaceT< FlagT >::computeTangentSpace( sdw::Vec3 const & cameraPosition
		, sdw::Vec3 const & worldPos
		, sdw::Vec3 const & nml
		, sdw::Vec3 const & tan
		, sdw::Vec3 const & bin )
	{
		CU_Require( !normal.getExpr()->isDummy() );
		CU_Require( !tangent.getExpr()->isDummy() );
		CU_Require( !bitangent.getExpr()->isDummy() );
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

	template< ast::var::Flag FlagT >
	sdw::Vec2 FragmentSurfaceT< FlagT >::getVelocity()const
	{
		return ( curPosition.xy() / curPosition.z() ) - ( prvPosition.xy() / prvPosition.z() );
	}

	//*****************************************************************************************
}
