#include "Castor3D/Shader/Shaders/GlslSurface.hpp"

#include "Castor3D/Render/RenderNodesPass.hpp"
#include "Castor3D/Shader/Shaders/GlslPassShaders.hpp"
#include "Castor3D/Shader/Shaders/GlslSubmeshShaders.hpp"
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

	template< typename Position3T, typename Position4T, typename NormalT >
	SurfaceBaseT< Position3T, Position4T, NormalT >::SurfaceBaseT( sdw::ShaderWriter & writer
		, ast::expr::ExprPtr expr
		, bool enabled )
		: StructInstance{ writer, castor::move( expr ), enabled }
		, clipPosition{ this->template getMember< sdw::Vec3 >( "clipPosition", true ) }
		, viewPosition{ this->template getMember< Position4T >( "viewPosition", true ) }
		, worldPosition{ this->template getMember< Position4T >( "worldPosition", true ) }
		, normal{ this->template getMember< NormalT >( "normal", true ) }
	{
	}

	template< typename Position3T, typename Position4T, typename NormalT >
	void SurfaceBaseT< Position3T, Position4T, NormalT >::fillType( sdw::type::BaseStruct & type )
	{
		type.declMember( "clipPosition", sdw::Vec3::makeType( type.getTypesCache() ) );
		type.declMember( "viewPosition", Position4T::makeType( type.getTypesCache() ) );
		type.declMember( "worldPosition", Position4T::makeType( type.getTypesCache() ) );
		type.declMember( "normal", NormalT::makeType( type.getTypesCache() ) );
	}

	template< typename Position3T, typename Position4T, typename NormalT >
	void SurfaceBaseT< Position3T, Position4T, NormalT >::fillIOType( sdw::type::IOStruct & type
		, PipelineFlags const & flags
		, uint32_t & index )
	{
		type.declMember( "viewPosition"
			, Position4T::makeType( type.getTypesCache() )
			, ast::type::NotArray
			, flags.usesViewSpace() ? index++ : 0u
			, flags.usesViewSpace() );
		type.declMember( "worldPosition"
			, Position4T::makeType( type.getTypesCache() )
			, ast::type::NotArray
			, flags.usesWorldSpace() ? index++ : 0u
			, flags.usesWorldSpace() );
		type.declMember( "normal"
			, NormalT::makeType( type.getTypesCache() )
			, ast::type::NotArray
			, flags.enableNormal() ? index++ : 0u
			, flags.enableNormal() );
	}

	template< typename Position3T, typename Position4T, typename NormalT >
	void SurfaceBaseT< Position3T, Position4T, NormalT >::fillType( sdw::type::BaseStruct & type
		, PipelineFlags const & flags )
	{
		type.declMember( "clipPosition"
			, sdw::Vec3::makeType( type.getTypesCache() ) );
		type.declMember( "viewPosition"
			, Position4T::makeType( type.getTypesCache() )
			, ast::type::NotArray
			, flags.usesViewSpace() );
		type.declMember( "worldPosition"
			, Position4T::makeType( type.getTypesCache() )
			, ast::type::NotArray
			, flags.usesWorldSpace() );
		type.declMember( "normal"
			, NormalT::makeType( type.getTypesCache() )
			, ast::type::NotArray
			, flags.enableNormal() );
	}

	template< typename Position3T, typename Position4T, typename NormalT >
	void SurfaceBaseT< Position3T, Position4T, NormalT >::fillInit( sdw::expr::ExprList & init
		, sdw::Vec3 clip
		, Position4T view
		, Position4T world
		, NormalT normal )
	{
		init.emplace_back( makeExpr( clip.isEnabled() ? clip : vec3( 0.0_f ) ) );
		init.emplace_back( makeExpr( view.isEnabled() ? view : SurfaceBaseT::makePosition4( 0.0_f ) ) );
		init.emplace_back( makeExpr( world.isEnabled() ? world : SurfaceBaseT::makePosition4( 0.0_f ) ) );
		init.emplace_back( makeExpr( normal.isEnabled() ? normal : SurfaceBaseT::makeNormal( 0.0_f ) ) );
	}

	template< typename Position3T, typename Position4T, typename NormalT >
	void SurfaceBaseT< Position3T, Position4T, NormalT >::fillInit( sdw::expr::ExprList & init
		, PipelineFlags const & flags
		, sdw::Vec3 clip
		, Position4T view
		, Position4T world
		, NormalT normal )
	{
		init.emplace_back( makeExpr( clip ) );

		if ( flags.usesViewSpace() )
		{
			init.emplace_back( makeExpr( SurfaceBaseT::makePosition4( shader::getXYZ( view ) ) ) );
		}

		if ( flags.usesWorldSpace() )
		{
			init.emplace_back( makeExpr( SurfaceBaseT::makePosition4( shader::getXYZ( world ) ) ) );
		}

		if ( flags.enableNormal() )
		{
			init.emplace_back( makeExpr( normal ) );
		}
	}

	//*****************************************************************************************

	template< typename Position3T, typename Position4T, typename NormalT >
	SurfaceT< Position3T, Position4T, NormalT >::SurfaceT( sdw::ShaderWriter & writer
		, ast::expr::ExprPtr expr
		, bool enabled )
		: SurfaceBaseT< Position3T, Position4T, NormalT >{ writer, castor::move( expr ), enabled }
		, texCoord{ this->template getMember< sdw::Vec3 >( "texture0" ) }
	{
	}

	template< typename Position3T, typename Position4T, typename NormalT >
	SurfaceT< Position3T, Position4T, NormalT >::SurfaceT( sdw::Vec3 clip
		, Position4T view
		, Position4T world
		, NormalT normal
		, sdw::Vec3 coord )
		: SurfaceT{ findWriterMandat( clip, view, world, normal, coord )
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

	template< typename Position3T, typename Position4T, typename NormalT >
	SurfaceT< Position3T, Position4T, NormalT >::SurfaceT( sdw::Vec3 clip
		, Position3T view
		, Position3T world
		, NormalT normal
		, sdw::Vec3 coord )
		: SurfaceT{ castor::move( clip )
			, SurfaceBaseT< Position3T, Position4T, NormalT >::makePosition4( view )
			, SurfaceBaseT< Position3T, Position4T, NormalT >::makePosition4( world )
			, castor::move( normal )
			, coord }
	{
	}

	template< typename Position3T, typename Position4T, typename NormalT >
	SurfaceT< Position3T, Position4T, NormalT >::SurfaceT( sdw::Vec3 clip
		, Position4T view
		, Position4T world
		, NormalT normal )
		: SurfaceT{ castor::move( clip )
			, castor::move( view )
			, castor::move( world )
			, castor::move( normal )
			, vec3( 0.0_f ) }
	{
	}

	template< typename Position3T, typename Position4T, typename NormalT >
	SurfaceT< Position3T, Position4T, NormalT >::SurfaceT( sdw::Vec3 clip
		, Position3T view
		, Position3T world
		, NormalT normal )
		: SurfaceT{ castor::move( clip )
			, castor::move( view )
			, castor::move( world )
			, castor::move( normal )
			, vec3( 0.0_f ) }
	{
	}

	template< typename Position3T, typename Position4T, typename NormalT >
	SurfaceT< Position3T, Position4T, NormalT >::SurfaceT( NormalT world
		, NormalT normal )
		: SurfaceT{ vec3( 0.0_f )
			, SurfaceBaseT< Position3T, Position4T, NormalT >::makePosition4( 0.0_f )
			, SurfaceBaseT< Position3T, Position4T, NormalT >::makePosition4( world )
			, castor::move( normal )
			, vec3( 0.0_f ) }
	{
	}

	template< typename Position3T, typename Position4T, typename NormalT >
	ast::type::BaseStructPtr SurfaceT< Position3T, Position4T, NormalT >::makeType( ast::type::TypesCache & cache )
	{
		auto result = cache.getStruct( ast::type::MemoryLayout::eStd430
			, "C3D_TexSurface" );

		if ( result->empty() )
		{
			fillType( *result );
		}

		return result;
	}

	template< typename Position3T, typename Position4T, typename NormalT >
	void SurfaceT< Position3T, Position4T, NormalT >::fillType( sdw::type::BaseStruct & type )
	{
		SurfaceBaseT< Position3T, Position4T, NormalT >::fillType( type );
		type.declMember( "texture0"
			, ast::type::Kind::eVec3F );
	}

	template< typename Position3T, typename Position4T, typename NormalT >
	void SurfaceT< Position3T, Position4T, NormalT >::fillType( sdw::type::BaseStruct & type
		, PipelineFlags const & flags )
	{
		SurfaceBaseT< Position3T, Position4T, NormalT >::fillType( type, flags );
		type.declMember( "texture0"
			, ast::type::Kind::eVec3F
			, flags.enableTexcoord0() );
	}

	template< typename Position3T, typename Position4T, typename NormalT >
	void SurfaceT< Position3T, Position4T, NormalT >::fillInit( sdw::expr::ExprList & init
		, sdw::Vec3 clip
		, Position4T view
		, Position4T world
		, NormalT normal
		, sdw::Vec3 texCoord )
	{
		SurfaceBaseT< Position3T, Position4T, NormalT >::fillInit( init, clip, view, world, normal );
		init.emplace_back( makeExpr( texCoord ) );
	}

	template< typename Position3T, typename Position4T, typename NormalT >
	void SurfaceT< Position3T, Position4T, NormalT >::fillInit( sdw::expr::ExprList & init
		, PipelineFlags const & flags
		, sdw::Vec3 clip
		, Position4T view
		, Position4T world
		, NormalT normal
		, sdw::Vec3 texCoord )
	{
		SurfaceBaseT< Position3T, Position4T, NormalT >::fillInit( init, flags, clip, view, world, normal );

		if ( flags.enableTexcoord0() )
		{
			init.emplace_back( makeExpr( texCoord ) );
		}
	}

	//*****************************************************************************************

	template< typename Position3T, typename Position4T, typename Normal3T, typename Normal4T >
	RasterizerSurfaceBaseT< Position3T, Position4T, Normal3T, Normal4T >::RasterizerSurfaceBaseT( sdw::ShaderWriter & writer
		, sdw::expr::ExprPtr expr
		, bool enabled )
		: SurfaceBaseT< Position3T, Position4T, Normal3T >{ writer, castor::move( expr ), enabled }
		, curPosition{ this->template getMember< Position4T >( "curPosition", true ) }
		, prvPosition{ this->template getMember< Position4T >( "prvPosition", true ) }
		, tangentSpaceFragPosition{ this->template getMember< Position3T >( "tangentSpaceFragPosition", true ) }
		, tangentSpaceViewPosition{ this->template getMember< sdw::Vec3 >( "tangentSpaceViewPosition", true ) }
		, tangent{ this->template getMember< Normal4T >( "tangent", true ) }
		, bitangent{ this->template getMember< Normal3T >( "bitangent", true ) }
		, colour{ this->template getMember< sdw::Vec3 >( "colour", true ) }
		, passMultipliers{ this->template getMemberArray< sdw::Vec4 >( "passMultipliers", true ) }
		, nodeId{ this->template getMember< sdw::UInt >( "nodeId", true ) }
		, vertexId{ this->template getMember< sdw::UInt >( "vertexId", true ) }
		, meshletId{ this->template getMember< sdw::UInt >( "meshletId", true ) }
	{
	}

	template< typename Position3T, typename Position4T, typename Normal3T, typename Normal4T >
	void RasterizerSurfaceBaseT< Position3T, Position4T, Normal3T, Normal4T >::computeVelocity( CameraData const & cameraData
		, Position4T & csCurPos
		, Position4T & csPrvPos )
	{
		// Convert the jitter from non-homogeneous coordinates to homogeneous
		// coordinates and add it:
		// (note that for providing the jitter in non-homogeneous projection space,
		//  pixel coordinates (screen space) need to multiplied by two in the C++
		//  code)
		cameraData.jitter( csCurPos );
		cameraData.jitter( csPrvPos );

		curPosition = SurfaceBaseT< Position3T, Position4T, Normal3T >::makePosition4( shader::getXYW( csCurPos ) );
		prvPosition = SurfaceBaseT< Position3T, Position4T, Normal3T >::makePosition4( shader::getXYW( csPrvPos ) );

		// Positions in projection space are in [-1, 1] range, while texture
		// coordinates are in [0, 1] range. So, we divide by 2 to get velocities in
		// the scale (and flip the y axis):
		shader::mulXY( curPosition, vec2( 0.5_f, -0.5_f ) );
		shader::mulXY( prvPosition, vec2( 0.5_f, -0.5_f ) );
	}

	template< typename Position3T, typename Position4T, typename Normal3T, typename Normal4T >
	void RasterizerSurfaceBaseT< Position3T, Position4T, Normal3T, Normal4T >::computeTangentSpace( PipelineFlags const & flags
		, sdw::Vec3 const & cameraPosition
		, Position3T const & worldPos
		, Normal3T const & nml
		, Normal4T const & tan )
	{
		if ( flags.enableTangentSpace() )
		{
			this->normal = nml;
			tangent = tan;
			bitangent = cross( this->normal, shader::getXYZ( tangent ) ) * shader::getW( tangent );

			if ( flags.isFrontCulled() )
			{
				this->normal = -this->normal;
				shader::negateXYZ( tangent );
				bitangent = -bitangent;
			}

			if ( !tangentSpaceFragPosition.getExpr()->isDummy() )
			{
				CU_Require( !worldPos.getExpr()->isDummy() );
				CU_Require( !tangentSpaceViewPosition.getExpr()->isDummy() );
				auto tbn = this->getWriter()->declLocale( "tbn"
					, transpose( mat3( shader::getRawXYZ( tangent ), shader::getRawXYZ( bitangent ), shader::getRawXYZ( this->normal ) ) ) );
				tangentSpaceFragPosition = tbn * worldPos;
				tangentSpaceViewPosition = tbn * cameraPosition.xyz();
			}
		}
		else if ( this->normal.isEnabled() )
		{
			this->normal = nml;

			if ( flags.isFrontCulled() )
			{
				this->normal = -this->normal;
			}
		}
	}

	template< typename Position3T, typename Position4T, typename Normal3T, typename Normal4T >
	void RasterizerSurfaceBaseT< Position3T, Position4T, Normal3T, Normal4T >::computeTangentSpace( PipelineFlags const & flags
		, sdw::Vec3 const & cameraPosition
		, Position3T const & worldPos
		, sdw::Mat3 const & mtx
		, Normal3T const & nml
		, Normal4T const & tan )
	{
		return computeTangentSpace( flags
			, cameraPosition
			, worldPos
			, normalize( mtx * nml )
			, SurfaceBaseT< Position3T, Position4T, Normal3T >::makePosition4( normalize( mtx * shader::getXYZ( tan ) ), shader::getW( tan ) ) );
	}

	template< typename Position3T, typename Position4T, typename Normal3T, typename Normal4T >
	void RasterizerSurfaceBaseT< Position3T, Position4T, Normal3T, Normal4T >::computeTangentSpace( PipelineFlags const & flags
		, sdw::Vec3 const & cameraPosition
		, Position3T const & worldPos
		, sdw::Mat3 const & mtx
		, Normal3T const & nml
		, Normal4T const & tan
		, Normal3T const & bin )
	{
		if ( bin.isEnabled() )
		{
			return computeTangentSpace( flags
				, cameraPosition
				, worldPos
				, normalize( mtx * nml )
				, SurfaceBaseT< Position3T, Position4T, Normal3T >::makePosition4( normalize( mtx * shader::getXYZ( tan ) ), shader::getW( tan ) )
				, normalize( mtx * bin ) );
		}

		return computeTangentSpace( flags
			, cameraPosition
			, worldPos
			, normalize( mtx * nml )
			, SurfaceBaseT< Position3T, Position4T, Normal3T >::makePosition4( normalize( mtx * shader::getXYZ( tan ) ), shader::getW( tan ) ) );
	}

	template< typename Position3T, typename Position4T, typename Normal3T, typename Normal4T >
	void RasterizerSurfaceBaseT< Position3T, Position4T, Normal3T, Normal4T >::computeTangentSpace( PipelineFlags const & flags
		, sdw::Vec3 const & cameraPosition
		, Position3T const & worldPos
		, Normal3T const & nml
		, Normal4T const & tan
		, Normal3T const & bin )
	{
		if ( bin.isEnabled() )
		{
			if ( flags.enableTangentSpace() )
			{
				this->normal = nml;
				tangent = tan;
				bitangent = bin;

				if ( flags.isFrontCulled() )
				{
					this->normal = -this->normal;
					shader::negateXYZ( tangent );
					bitangent = -bitangent;
				}

				if ( !tangentSpaceFragPosition.getExpr()->isDummy() )
				{
					CU_Require( !worldPos.getExpr()->isDummy() );
					CU_Require( !tangentSpaceViewPosition.getExpr()->isDummy() );
					auto tbn = this->getWriter()->declLocale( "tbn"
						, transpose( mat3( shader::getRawXYZ( tangent ), shader::getRawXYZ( bitangent ), shader::getRawXYZ( this->normal ) ) ) );
					tangentSpaceFragPosition = tbn * worldPos;
					tangentSpaceViewPosition = tbn * cameraPosition.xyz();
				}
			}
			else
			{
				this->normal = nml;

				if ( flags.isFrontCulled() )
				{
					this->normal = -this->normal;
				}
			}
		}
		else
		{
			computeTangentSpace( flags, cameraPosition, worldPos, nml, tan );
		}
	}

	template< typename Position3T, typename Position4T, typename Normal3T, typename Normal4T >
	sdw::Vec2 RasterizerSurfaceBaseT< Position3T, Position4T, Normal3T, Normal4T >::getVelocity()const
	{
		return ( curPosition.xy() / curPosition.z() ) - ( prvPosition.xy() / prvPosition.z() );
	}

	template< typename Position3T, typename Position4T, typename Normal3T, typename Normal4T >
	void RasterizerSurfaceBaseT< Position3T, Position4T, Normal3T, Normal4T >::fillIOType( sdw::type::IOStruct & type
		, SubmeshShaders const & submeshShaders
		, PassShaders const & shaders
		, PipelineFlags const & flags
		, uint32_t & index )
	{
		SurfaceBaseT< Position3T, Position4T, Normal3T >::fillIOType( type, flags, index );
		type.declMember( "nodeId", ast::type::Kind::eUInt
			, ast::type::NotArray
			, index );
		++index;
		type.declMember( "vertexId"
			, ast::type::Kind::eUInt
			, ast::type::NotArray
			, ( flags.enableVertexID() ? index++ : 0 )
			, flags.enableVertexID() );
		type.declMember( "meshletId"
			, ast::type::Kind::eUInt
			, ast::type::NotArray
			, ( flags.enableMeshletID() ? index++ : 0 )
			, flags.enableMeshletID() );
		type.declMember( "curPosition"
			, Position4T::makeType( type.getTypesCache() )
			, ast::type::NotArray
			, ( flags.writeVelocity() ? index++ : 0 )
			, flags.writeVelocity() );
		type.declMember( "prvPosition"
			, Position4T::makeType( type.getTypesCache() )
			, ast::type::NotArray
			, ( flags.writeVelocity() ? index++ : 0 )
			, flags.writeVelocity() );
		type.declMember( "tangentSpaceFragPosition"
			, Position3T::makeType( type.getTypesCache() )
			, ast::type::NotArray
			, ( shaders.enableParallaxOcclusionMapping( flags ) ? index++ : 0 )
			, shaders.enableParallaxOcclusionMapping( flags ) );
		type.declMember( "tangentSpaceViewPosition"
			, ast::type::Kind::eVec3F
			, ast::type::NotArray
			, ( shaders.enableParallaxOcclusionMapping( flags) ? index++ : 0 )
			, shaders.enableParallaxOcclusionMapping( flags ) );
		type.declMember( "tangent"
			, Normal4T::makeType( type.getTypesCache() )
			, ( flags.enableTangentSpace() ? index++ : 0 )
			, flags.enableTangentSpace() );
		type.declMember( "bitangent"
			, Normal3T::makeType( type.getTypesCache() )
			, ast::type::NotArray
			, ( ( flags.enableBitangent() || flags.enableTangentSpace() ) ? index++ : 0 )
			, ( flags.enableBitangent() || flags.enableTangentSpace() ) );
		type.declMember( "colour", ast::type::Kind::eVec3F
			, ast::type::NotArray
			, ( flags.enableColours() ? index++ : 0 )
			, flags.enableColours() );
		type.declMember( "passMultipliers"
			, ast::type::Kind::eVec4F
			, 4u
			, ( flags.enablePassMasks() ? index : 0 )
			, flags.enablePassMasks() );

		if ( flags.enablePassMasks() )
		{
			index += 4u;
		}
		
		submeshShaders.fillRasterSurface( type, index );
	}

	template< typename Position3T, typename Position4T, typename Normal3T, typename Normal4T >
	void RasterizerSurfaceBaseT< Position3T, Position4T, Normal3T, Normal4T >::fillType( sdw::type::BaseStruct & type
		, SubmeshShaders const & submeshShaders
		, PassShaders const & shaders
		, PipelineFlags const & flags )
	{
		SurfaceBaseT< Position3T, Position4T, Normal3T >::fillType( type, flags );
		type.declMember( "nodeId"
			, ast::type::Kind::eUInt
			, ast::type::NotArray );
		type.declMember( "vertexId"
			, ast::type::Kind::eUInt
			, ast::type::NotArray
			, flags.enableVertexID() );
		type.declMember( "meshletId"
			, ast::type::Kind::eUInt
			, ast::type::NotArray
			, flags.enableMeshletID() );
		type.declMember( "curPosition"
			, Position4T::makeType( type.getTypesCache() )
			, ast::type::NotArray
			, flags.writeVelocity() );
		type.declMember( "prvPosition"
			, Position4T::makeType( type.getTypesCache() )
			, ast::type::NotArray
			, flags.writeVelocity() );
		type.declMember( "tangentSpaceFragPosition"
			, Position3T::makeType( type.getTypesCache() )
			, ast::type::NotArray
			, shaders.enableParallaxOcclusionMapping( flags ) );
		type.declMember( "tangentSpaceViewPosition"
			, ast::type::Kind::eVec3F
			, ast::type::NotArray
			, shaders.enableParallaxOcclusionMapping( flags ) );
		type.declMember( "tangent"
			, Normal4T::makeType( type.getTypesCache() )
			, ast::type::NotArray
			, flags.enableTangentSpace() );
		type.declMember( "bitangent"
			, Normal3T::makeType( type.getTypesCache() )
			, ast::type::NotArray
			, flags.enableBitangent() || flags.enableTangentSpace() );
		type.declMember( "colour"
			, ast::type::Kind::eVec3F
			, ast::type::NotArray
			, flags.enableColours() );
		type.declMember( "passMultipliers"
			, ast::type::Kind::eVec4F
			, 4u
			, flags.enablePassMasks() );
		submeshShaders.fillRasterSurface( type );
	}

	template< typename Position3T, typename Position4T, typename Normal3T, typename Normal4T >
	void RasterizerSurfaceBaseT< Position3T, Position4T, Normal3T, Normal4T >::fillType( sdw::type::BaseStruct & type
		, SubmeshShaders const & submeshShaders )
	{
		SurfaceBaseT< Position3T, Position4T, Normal3T >::fillType( type );
		type.declMember( "nodeId", ast::type::Kind::eUInt
			, ast::type::NotArray );
		type.declMember( "vertexId", ast::type::Kind::eUInt
			, ast::type::NotArray );
		type.declMember( "meshletId", ast::type::Kind::eUInt
			, ast::type::NotArray );
		type.declMember( "curPosition", Position4T::makeType( type.getTypesCache() )
			, ast::type::NotArray );
		type.declMember( "prvPosition", Position4T::makeType( type.getTypesCache() )
			, ast::type::NotArray );
		type.declMember( "tangentSpaceFragPosition", Position3T::makeType( type.getTypesCache() )
			, ast::type::NotArray );
		type.declMember( "tangentSpaceViewPosition", ast::type::Kind::eVec3F
			, ast::type::NotArray );
		type.declMember( "tangent", Normal4T::makeType( type.getTypesCache() )
			, ast::type::NotArray );
		type.declMember( "bitangent", Normal3T::makeType( type.getTypesCache() )
			, ast::type::NotArray );
		type.declMember( "colour", ast::type::Kind::eVec3F
			, ast::type::NotArray );
		type.declMember( "passMultipliers", ast::type::Kind::eVec4F
			, 4u );
		submeshShaders.fillRasterSurface( type );
	}

	//*****************************************************************************************

	template< typename Position3T, typename Position4T, typename Normal3T, typename Normal4T, typename TexcoordT, ast::var::Flag FlagT >
	RasterizerSurfaceT< Position3T, Position4T, Normal3T, Normal4T, TexcoordT, FlagT >::RasterizerSurfaceT( sdw::ShaderWriter & writer
		, sdw::expr::ExprPtr expr
		, bool enabled )
		: RasterizerSurfaceBaseT< Position3T, Position4T, Normal3T, Normal4T >{ writer, castor::move( expr ), enabled }
		, texture0{ this->template getMember< TexcoordT >( "texture0", true ) }
		, texture1{ this->template getMember< TexcoordT >( "texture1", true ) }
		, texture2{ this->template getMember< TexcoordT >( "texture2", true ) }
		, texture3{ this->template getMember< TexcoordT >( "texture3", true ) }
	{
	}

	template< typename Position3T, typename Position4T, typename Normal3T, typename Normal4T, typename TexcoordT, ast::var::Flag FlagT >
	RasterizerSurfaceT< Position3T, Position4T, Normal3T, Normal4T, TexcoordT, FlagT >::RasterizerSurfaceT( sdw::Vec3 clip
		, Position3T view
		, Position3T world
		, Normal3T normal
		, TexcoordT coord )
		: RasterizerSurfaceT{ findWriterMandat( clip, view, world, normal, coord )
			, sdw::makeAggrInit( makeType( findTypesCache( clip, view, world, normal, coord ) )
				, [&clip, &view, &world, &normal, &coord]()
				{
					sdw::expr::ExprList result;
					SurfaceBaseT< Position3T, Position4T, Normal3T >::fillInit( result
						, clip
						, SurfaceBaseT< Position3T, Position4T, Normal3T >::makePosition4( view )
						, SurfaceBaseT< Position3T, Position4T, Normal3T >::makePosition4( world )
						, normal );
					result.emplace_back( makeExpr( coord ) );
					return result;
				}() )
			, true }
	{
	}

	template< typename Position3T, typename Position4T, typename Normal3T, typename Normal4T, typename TexcoordT, ast::var::Flag FlagT >
	template< ast::var::Flag FlagU >
	RasterizerSurfaceT< Position3T, Position4T, Normal3T, Normal4T, TexcoordT, FlagT >::RasterizerSurfaceT( RasterizerSurfaceT< Position3T, Position4T, Normal3T, Normal4T, TexcoordT, FlagU > const & rhs )
		: RasterizerSurfaceT{ *rhs.getWriter()
			, sdw::makeAggrInit( rhs.getType(), rhs.makeAggrInit() )
			, rhs.isEnabled() }
	{
	}

	template< typename Position3T, typename Position4T, typename Normal3T, typename Normal4T, typename TexcoordT, ast::var::Flag FlagT >
	ast::type::IOStructPtr RasterizerSurfaceT< Position3T, Position4T, Normal3T, Normal4T, TexcoordT, FlagT >::makeIOType( ast::type::TypesCache & cache
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
			RasterizerSurfaceBaseT< Position3T, Position4T, Normal3T, Normal4T >::fillIOType( *result, submeshShaders, passShaders, flags, index );
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

	template< typename Position3T, typename Position4T, typename Normal3T, typename Normal4T, typename TexcoordT, ast::var::Flag FlagT >
	ast::type::BaseStructPtr RasterizerSurfaceT< Position3T, Position4T, Normal3T, Normal4T, TexcoordT, FlagT >::makeType( ast::type::TypesCache & cache
		, SubmeshShaders const & submeshShaders
		, PassShaders const & passShaders
		, PipelineFlags const & flags )
	{
		auto result = cache.getStruct( ast::type::MemoryLayout::eC
			, "C3D_RasterizerSurface" );

		if ( result->empty() )
		{
			auto texType = TexcoordT::makeType( cache );
			RasterizerSurfaceBaseT< Position3T, Position4T, Normal3T, Normal4T >::fillType( *result, submeshShaders, passShaders, flags );
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

	template< typename Position3T, typename Position4T, typename Normal3T, typename Normal4T, typename TexcoordT, ast::var::Flag FlagT >
	ast::type::BaseStructPtr RasterizerSurfaceT< Position3T, Position4T, Normal3T, Normal4T, TexcoordT, FlagT >::makeType( ast::type::TypesCache & cache
		, SubmeshShaders const & submeshShaders )
	{
		auto result = cache.getStruct( ast::type::MemoryLayout::eC, "C3D_RasterSurface" );

		if ( result->empty() )
		{
			auto texType = TexcoordT::makeType( cache );
			RasterizerSurfaceBaseT< Position3T, Position4T, Normal3T, Normal4T >::fillType( *result, submeshShaders );
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
