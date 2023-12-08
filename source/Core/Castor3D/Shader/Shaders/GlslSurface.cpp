#include "Castor3D/Shader/Shaders/GlslSurface.hpp"

#include "Castor3D/Render/RenderNodesPass.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslPassShaders.hpp"
#include "Castor3D/Shader/Shaders/GlslSubmeshShaders.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"
#include "Castor3D/Shader/Ubos/MorphingUbo.hpp"

#include <ShaderWriter/Writer.hpp>
#include <ShaderWriter/Intrinsics/Intrinsics.hpp>

namespace castor3d::shader
{
	//*****************************************************************************************

	SurfaceBase::SurfaceBase( sdw::ShaderWriter & writer
		, ast::expr::ExprPtr expr
		, bool enabled )
		: StructInstance{ writer, std::move( expr ), enabled }
		, clipPosition{ this->getMember< sdw::Vec3 >( "clipPosition", true ) }
		, viewPosition{ this->getMember< sdw::Vec4 >( "viewPosition", true ) }
		, worldPosition{ this->getMember< sdw::Vec4 >( "worldPosition", true ) }
		, normal{ this->getMember< sdw::Vec3 >( "normal", true ) }
	{
	}

	void SurfaceBase::fillType( sdw::type::BaseStruct & type )
	{
		type.declMember( "clipPosition", ast::type::Kind::eVec3F );
		type.declMember( "viewPosition", ast::type::Kind::eVec4F );
		type.declMember( "worldPosition", ast::type::Kind::eVec4F );
		type.declMember( "normal", ast::type::Kind::eVec3F );
	}

	void SurfaceBase::fillIOType( sdw::type::IOStruct & type
		, PipelineFlags const & flags
		, uint32_t & index )
	{
		type.declMember( "viewPosition"
			, ast::type::Kind::eVec4F
			, ast::type::NotArray
			, flags.usesViewSpace() ? index++ : 0u
			, flags.usesViewSpace() );
		type.declMember( "worldPosition"
			, ast::type::Kind::eVec4F
			, ast::type::NotArray
			, flags.usesWorldSpace() ? index++ : 0u
			, flags.usesWorldSpace() );
		type.declMember( "normal"
			, ast::type::Kind::eVec3F
			, ast::type::NotArray
			, flags.enableNormal() ? index++ : 0u
			, flags.enableNormal() );
	}

	void SurfaceBase::fillType( sdw::type::BaseStruct & type
		, PipelineFlags const & flags )
	{
		type.declMember( "clipPosition"
			, ast::type::Kind::eVec3F );
		type.declMember( "viewPosition"
			, ast::type::Kind::eVec4F
			, ast::type::NotArray
			, flags.usesViewSpace() );
		type.declMember( "worldPosition"
			, ast::type::Kind::eVec4F
			, ast::type::NotArray
			, flags.usesWorldSpace() );
		type.declMember( "normal"
			, ast::type::Kind::eVec3F
			, ast::type::NotArray
			, flags.enableNormal() );
	}

	void SurfaceBase::fillInit( sdw::expr::ExprList & init
		, sdw::Vec3 clip
		, sdw::Vec4 view
		, sdw::Vec4 world
		, sdw::Vec3 normal )
	{
		init.emplace_back( makeExpr( clip.isEnabled() ? clip : vec3( 0.0_f ) ) );
		init.emplace_back( makeExpr( view.isEnabled() ? view : vec4( 0.0_f ) ) );
		init.emplace_back( makeExpr( world.isEnabled() ? world : vec4( 0.0_f ) ) );
		init.emplace_back( makeExpr( normal.isEnabled() ? normal : vec3( 0.0_f ) ) );
	}

	void SurfaceBase::fillInit( sdw::expr::ExprList & init
		, PipelineFlags const & flags
		, sdw::Vec3 clip
		, sdw::Vec4 view
		, sdw::Vec4 world
		, sdw::Vec3 normal )
	{
		init.emplace_back( makeExpr( clip ) );

		if ( flags.usesViewSpace() )
		{
			init.emplace_back( makeExpr( vec4( view.xyz(), 1.0_f ) ) );
		}

		if ( flags.usesWorldSpace() )
		{
			init.emplace_back( makeExpr( vec4( world.xyz(), 1.0_f ) ) );
		}

		if ( flags.enableNormal() )
		{
			init.emplace_back( makeExpr( normal ) );
		}
	}

	//*****************************************************************************************

	Surface::Surface( sdw::ShaderWriter & writer
		, ast::expr::ExprPtr expr
		, bool enabled )
		: SurfaceBase{ writer, std::move( expr ), enabled }
		, texCoord{ this->getMember< sdw::Vec3 >( "texture0" ) }
	{
	}

	Surface::Surface( sdw::Vec3 clip
		, sdw::Vec4 view
		, sdw::Vec4 world
		, sdw::Vec3 normal
		, sdw::Vec3 coord )
		: Surface{ findWriterMandat( clip, view, world, normal, coord )
			, sdw::makeAggrInit( makeType( findTypesCache( clip, view, world, normal, coord ) )
				, [&clip, &view, &world, &normal, &coord]()
				{
					sdw::expr::ExprList result;
					fillInit( result, clip, view, world, normal, coord );
					return result;
				}() )
			, true }
	{
	}

	Surface::Surface( sdw::Vec3 clip
		, sdw::Vec3 view
		, sdw::Vec3 world
		, sdw::Vec3 normal
		, sdw::Vec3 coord )
		: Surface{ std::move( clip )
			, vec4( view, 1.0_f )
			, vec4( world, 1.0_f )
			, std::move( normal )
			, vec3( 0.0_f ) }
	{
	}

	Surface::Surface( sdw::Vec3 clip
		, sdw::Vec4 view
		, sdw::Vec4 world
		, sdw::Vec3 normal )
		: Surface{ std::move( clip )
			, std::move( view )
			, std::move( world )
			, std::move( normal )
			, vec3( 0.0_f ) }
	{
	}

	Surface::Surface( sdw::Vec3 clip
		, sdw::Vec3 view
		, sdw::Vec3 world
		, sdw::Vec3 normal )
		: Surface{ std::move( clip )
			, std::move( view )
			, std::move( world )
			, std::move( normal )
			, vec3( 0.0_f ) }
	{
	}

	Surface::Surface( sdw::Vec3 world
		, sdw::Vec3 normal )
		: Surface{ vec3( 0.0_f )
			, vec4( 0.0_f )
			, vec4( world, 1.0_f )
			, std::move( normal )
			, vec3( 0.0_f ) }
	{
	}

	ast::type::BaseStructPtr Surface::makeType( ast::type::TypesCache & cache )
	{
		auto result = cache.getStruct( ast::type::MemoryLayout::eStd430
			, "C3D_TexSurface" );

		if ( result->empty() )
		{
			fillType( *result );
		}

		return result;
	}

	void Surface::fillType( sdw::type::BaseStruct & type )
	{
		SurfaceBase::fillType( type );
		type.declMember( "texture0"
			, ast::type::Kind::eVec3F );
	}

	void Surface::fillType( sdw::type::BaseStruct & type
		, PipelineFlags const & flags )
	{
		SurfaceBase::fillType( type, flags );
		type.declMember( "texture0"
			, ast::type::Kind::eVec3F
			, flags.enableTexcoord0() );
	}

	void Surface::fillInit( sdw::expr::ExprList & init
		, sdw::Vec3 clip
		, sdw::Vec4 view
		, sdw::Vec4 world
		, sdw::Vec3 normal
		, sdw::Vec3 texCoord )
	{
		SurfaceBase::fillInit( init, clip, view, world, normal );
		init.emplace_back( makeExpr( texCoord ) );
	}

	void Surface::fillInit( sdw::expr::ExprList & init
		, PipelineFlags const & flags
		, sdw::Vec3 clip
		, sdw::Vec4 view
		, sdw::Vec4 world
		, sdw::Vec3 normal
		, sdw::Vec3 texCoord )
	{
		SurfaceBase::fillInit( init, flags, clip, view, world, normal );

		if ( flags.enableTexcoord0() )
		{
			init.emplace_back( makeExpr( texCoord ) );
		}
	}

	//*****************************************************************************************

	RasterizerSurfaceBase::RasterizerSurfaceBase( sdw::ShaderWriter & writer
		, sdw::expr::ExprPtr expr
		, bool enabled )
		: SurfaceBase{ writer, std::move( expr ), enabled }
		, curPosition{ this->getMember< sdw::Vec4 >( "curPosition", true ) }
		, prvPosition{ this->getMember< sdw::Vec4 >( "prvPosition", true ) }
		, tangentSpaceFragPosition{ this->getMember< sdw::Vec3 >( "tangentSpaceFragPosition", true ) }
		, tangentSpaceViewPosition{ this->getMember< sdw::Vec3 >( "tangentSpaceViewPosition", true ) }
		, tangent{ this->getMember< sdw::Vec4 >( "tangent", true ) }
		, bitangent{ this->getMember< sdw::Vec3 >( "bitangent", true ) }
		, colour{ this->getMember< sdw::Vec3 >( "colour", true ) }
		, passMultipliers{ this->getMemberArray< sdw::Vec4 >( "passMultipliers", true ) }
		, nodeId{ this->getMember< sdw::UInt >( "nodeId", true ) }
		, vertexId{ this->getMember< sdw::UInt >( "vertexId", true ) }
		, meshletId{ this->getMember< sdw::UInt >( "meshletId", true ) }
	{
	}

	void RasterizerSurfaceBase::computeVelocity( CameraData const & cameraData
		, sdw::Vec4 & csCurPos
		, sdw::Vec4 & csPrvPos )
	{
		// Convert the jitter from non-homogeneous coordinates to homogeneous
		// coordinates and add it:
		// (note that for providing the jitter in non-homogeneous projection space,
		//  pixel coordinates (screen space) need to multiplied by two in the C++
		//  code)
		cameraData.jitter( csCurPos );
		cameraData.jitter( csPrvPos );

		curPosition = vec4( csCurPos.xyw(), 1.0_f );
		prvPosition = vec4( csPrvPos.xyw(), 1.0_f );

		// Positions in projection space are in [-1, 1] range, while texture
		// coordinates are in [0, 1] range. So, we divide by 2 to get velocities in
		// the scale (and flip the y axis):
		curPosition.xy() *= vec2( 0.5_f, -0.5_f );
		prvPosition.xy() *= vec2( 0.5_f, -0.5_f );
	}

	void RasterizerSurfaceBase::computeTangentSpace( PipelineFlags const & flags
		, sdw::Vec3 const & cameraPosition
		, sdw::Vec3 const & worldPos
		, sdw::Vec3 const & nml
		, sdw::Vec4 const & tan )
	{
		if ( flags.enableTangentSpace() )
		{
			normal = nml;
			tangent = tan;
			bitangent = tangent.w() * cross( normal, tangent.xyz() );

			if ( flags.isFrontCulled() )
			{
				normal = -normal;
				tangent.xyz() = -tangent.xyz();
				bitangent = -bitangent;
			}

			if ( !tangentSpaceFragPosition.getExpr()->isDummy() )
			{
				CU_Require( !worldPos.getExpr()->isDummy() );
				CU_Require( !tangentSpaceViewPosition.getExpr()->isDummy() );
				auto tbn = getWriter()->declLocale( "tbn"
					, transpose( mat3( tangent.xyz(), bitangent, normal ) ) );
				tangentSpaceFragPosition = tbn * worldPos;
				tangentSpaceViewPosition = tbn * cameraPosition.xyz();
			}
		}
		else if ( normal.isEnabled() )
		{
			normal = nml;

			if ( flags.isFrontCulled() )
			{
				normal = -normal;
			}
		}
	}

	void RasterizerSurfaceBase::computeTangentSpace( PipelineFlags const & flags
		, sdw::Vec3 const & cameraPosition
		, sdw::Vec3 const & worldPos
		, sdw::Mat3 const & mtx
		, sdw::Vec3 const & nml
		, sdw::Vec4 const & tan )
	{
		return computeTangentSpace( flags
			, cameraPosition
			, worldPos
			, normalize( mtx * nml )
			, vec4( normalize( mtx * tan.xyz() ), tan.w() ) );
	}

	void RasterizerSurfaceBase::computeTangentSpace( PipelineFlags const & flags
		, sdw::Vec3 const & cameraPosition
		, sdw::Vec3 const & worldPos
		, sdw::Mat3 const & mtx
		, sdw::Vec3 const & nml
		, sdw::Vec4 const & tan
		, sdw::Vec3 const & bin )
	{
		if ( bin.isEnabled() )
		{
			return computeTangentSpace( flags
				, cameraPosition
				, worldPos
				, normalize( mtx * nml )
				, vec4( normalize( mtx * tan.xyz() ), tan.w() )
				, vec3( normalize( mtx * bin ) ) );
		}

		return computeTangentSpace( flags
			, cameraPosition
			, worldPos
			, normalize( mtx * nml )
			, vec4( normalize( mtx * tan.xyz() ), tan.w() ) );
	}

	void RasterizerSurfaceBase::computeTangentSpace( PipelineFlags const & flags
		, sdw::Vec3 const & cameraPosition
		, sdw::Vec3 const & worldPos
		, sdw::Vec3 const & nml
		, sdw::Vec4 const & tan
		, sdw::Vec3 const & bin )
	{
		if ( bin.isEnabled() )
		{
			if ( flags.enableTangentSpace() )
			{
				normal = nml;
				tangent = tan;
				bitangent = bin;

				if ( flags.isFrontCulled() )
				{
					normal = -normal;
					tangent.xyz() = -tangent.xyz();
					bitangent = -bitangent;
				}

				if ( !tangentSpaceFragPosition.getExpr()->isDummy() )
				{
					CU_Require( !worldPos.getExpr()->isDummy() );
					CU_Require( !tangentSpaceViewPosition.getExpr()->isDummy() );
					auto tbn = getWriter()->declLocale( "tbn"
						, transpose( mat3( tangent.xyz(), bitangent, normal ) ) );
					tangentSpaceFragPosition = tbn * worldPos;
					tangentSpaceViewPosition = tbn * cameraPosition.xyz();
				}
			}
			else
			{
				normal = nml;

				if ( flags.isFrontCulled() )
				{
					normal = -normal;
				}
			}
		}
		else
		{
			computeTangentSpace( flags, cameraPosition, worldPos, nml, tan );
		}
	}

	sdw::Vec2 RasterizerSurfaceBase::getVelocity()const
	{
		return ( curPosition.xy() / curPosition.z() ) - ( prvPosition.xy() / prvPosition.z() );
	}

	void RasterizerSurfaceBase::fillIOType( sdw::type::IOStruct & type
		, SubmeshShaders const & submeshShaders
		, PassShaders const & shaders
		, PipelineFlags const & flags
		, uint32_t & index )
	{
		SurfaceBase::fillIOType( type, flags, index );
		type.declMember( "nodeId", ast::type::Kind::eUInt
			, ast::type::NotArray
			, index++ );
		type.declMember( "vertexId", ast::type::Kind::eUInt
			, ast::type::NotArray
			, ( flags.enableVertexID() ? index++ : 0 )
			, flags.enableVertexID() );
		type.declMember( "meshletId", ast::type::Kind::eUInt
			, ast::type::NotArray
			, ( flags.enableMeshletID() ? index++ : 0 )
			, flags.enableMeshletID() );
		type.declMember( "curPosition", ast::type::Kind::eVec4F
			, ast::type::NotArray
			, ( flags.writeVelocity() ? index++ : 0 )
			, flags.writeVelocity() );
		type.declMember( "prvPosition", ast::type::Kind::eVec4F
			, ast::type::NotArray
			, ( flags.writeVelocity() ? index++ : 0 )
			, flags.writeVelocity() );
		type.declMember( "tangentSpaceFragPosition"
			, ast::type::Kind::eVec3F
			, ast::type::NotArray
			, ( shaders.enableParallaxOcclusionMapping( flags ) ? index++ : 0 )
			, shaders.enableParallaxOcclusionMapping( flags ) );
		type.declMember( "tangentSpaceViewPosition"
			, ast::type::Kind::eVec3F
			, ast::type::NotArray
			, ( shaders.enableParallaxOcclusionMapping( flags) ? index++ : 0 )
			, shaders.enableParallaxOcclusionMapping( flags ) );
		type.declMember( "tangent", ast::type::Kind::eVec4F
			, ast::type::NotArray
			, ( flags.enableTangentSpace() ? index++ : 0 )
			, flags.enableTangentSpace() );
		type.declMember( "bitangent", ast::type::Kind::eVec3F
			, ast::type::NotArray
			, ( ( flags.enableBitangent() || flags.enableTangentSpace() ) ? index++ : 0 )
			, ( flags.enableBitangent() || flags.enableTangentSpace() ) );
		type.declMember( "colour", ast::type::Kind::eVec3F
			, ast::type::NotArray
			, ( flags.enableColours() ? index++ : 0 )
			, flags.enableColours() );
		type.declMember( "passMultipliers", ast::type::Kind::eVec4F
			, 4u
			, ( flags.enablePassMasks() ? index : 0 )
			, flags.enablePassMasks() );

		if ( flags.enablePassMasks() )
		{
			index += 4u;
		}
		
		submeshShaders.fillRasterSurface( type, index );
	}

	void RasterizerSurfaceBase::fillType( sdw::type::BaseStruct & type
		, SubmeshShaders const & submeshShaders
		, PassShaders const & shaders
		, PipelineFlags const & flags )
	{
		SurfaceBase::fillType( type, flags );
		type.declMember( "nodeId", ast::type::Kind::eUInt
			, ast::type::NotArray );
		type.declMember( "vertexId", ast::type::Kind::eUInt
			, ast::type::NotArray
			, flags.enableVertexID() );
		type.declMember( "meshletId", ast::type::Kind::eUInt
			, ast::type::NotArray
			, flags.enableMeshletID() );
		type.declMember( "curPosition", ast::type::Kind::eVec4F
			, ast::type::NotArray
			, flags.writeVelocity() );
		type.declMember( "prvPosition", ast::type::Kind::eVec4F
			, ast::type::NotArray
			, flags.writeVelocity() );
		type.declMember( "tangentSpaceFragPosition"
			, ast::type::Kind::eVec3F
			, ast::type::NotArray
			, shaders.enableParallaxOcclusionMapping( flags ) );
		type.declMember( "tangentSpaceViewPosition"
			, ast::type::Kind::eVec3F
			, ast::type::NotArray
			, shaders.enableParallaxOcclusionMapping( flags ) );
		type.declMember( "tangent", ast::type::Kind::eVec4F
			, ast::type::NotArray
			, flags.enableTangentSpace() );
		type.declMember( "bitangent", ast::type::Kind::eVec3F
			, ast::type::NotArray
			, flags.enableBitangent() || flags.enableTangentSpace() );
		type.declMember( "colour", ast::type::Kind::eVec3F
			, ast::type::NotArray
			, flags.enableColours() );
		type.declMember( "passMultipliers", ast::type::Kind::eVec4F
			, 4u
			, flags.enablePassMasks() );
		submeshShaders.fillRasterSurface( type );
	}

	void RasterizerSurfaceBase::fillType( sdw::type::BaseStruct & type
		, SubmeshShaders const & submeshShaders )
	{
		SurfaceBase::fillType( type );
		type.declMember( "nodeId", ast::type::Kind::eUInt
			, ast::type::NotArray );
		type.declMember( "vertexId", ast::type::Kind::eUInt
			, ast::type::NotArray );
		type.declMember( "meshletId", ast::type::Kind::eUInt
			, ast::type::NotArray );
		type.declMember( "curPosition", ast::type::Kind::eVec4F
			, ast::type::NotArray );
		type.declMember( "prvPosition", ast::type::Kind::eVec4F
			, ast::type::NotArray );
		type.declMember( "tangentSpaceFragPosition", ast::type::Kind::eVec3F
			, ast::type::NotArray );
		type.declMember( "tangentSpaceViewPosition", ast::type::Kind::eVec3F
			, ast::type::NotArray );
		type.declMember( "tangent", ast::type::Kind::eVec4F
			, ast::type::NotArray );
		type.declMember( "bitangent", ast::type::Kind::eVec3F
			, ast::type::NotArray );
		type.declMember( "colour", ast::type::Kind::eVec3F
			, ast::type::NotArray );
		type.declMember( "passMultipliers", ast::type::Kind::eVec4F
			, 4u );
		submeshShaders.fillRasterSurface( type );
	}

	//*****************************************************************************************
}
