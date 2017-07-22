#include "GlslShadow.hpp"

using namespace Castor;

namespace GLSL
{
	Castor::String const Shadow::MapShadowDirectional = cuT( "c3d_mapShadowDirectional" );
	Castor::String const Shadow::MapShadowSpot = cuT( "c3d_mapShadowSpot" );
	Castor::String const Shadow::MapShadowPoint = cuT( "c3d_mapShadowPoint" );

	Shadow::Shadow( GlslWriter & writer )
		: m_writer{ writer }
	{
	}

	void Shadow::Declare( ShadowType type )
	{
		auto c3d_mapShadowDirectional = m_writer.DeclUniform< Sampler2DShadow >( MapShadowDirectional );
		auto c3d_mapShadowSpot = m_writer.DeclUniform< Sampler2DArrayShadow >( MapShadowSpot );
		auto c3d_mapShadowPoint = m_writer.DeclUniform< SamplerCubeShadow >( MapShadowPoint, PointShadowMapCount );

		if ( type == ShadowType::ePoisson || type == ShadowType::eStratifiedPoisson )
		{
			auto c3d_poissonDisk = m_writer.DeclUniform< Vec2 >( cuT( "c3d_poissonDisk" ), 4,
			{
				vec2( -0.94201624_f, -0.39906216 ),
				vec2( 0.94558609_f, -0.76890725 ),
				vec2( -0.094184101_f, -0.92938870 ),
				vec2( 0.34495938_f, 0.29387760 )
			} );
		}

		DoDeclare_GetRandom();
		DoDeclare_GetShadowOffset();
		DoDeclare_GetLightSpacePosition();
		DoDeclare_FilterDirectional( type );
		DoDeclare_FilterSpot( type );
		DoDeclare_FilterPoint( type );
		DoDeclare_ComputeDirectionalShadow();
		DoDeclare_ComputeSpotShadow();
		DoDeclare_ComputePointShadow();
	}

	void Shadow::DeclareDirectional( ShadowType type )
	{
		auto c3d_mapShadowDirectional = m_writer.DeclUniform< Sampler2DShadow >( MapShadowDirectional );

		if ( type == ShadowType::ePoisson || type == ShadowType::eStratifiedPoisson )
		{
			auto c3d_poissonDisk = m_writer.DeclUniform< Vec2 >( cuT( "c3d_poissonDisk" ), 4,
			{
				vec2( -0.94201624_f, -0.39906216 ),
				vec2( 0.94558609_f, -0.76890725 ),
				vec2( -0.094184101_f, -0.92938870 ),
				vec2( 0.34495938_f, 0.29387760 )
			} );
		}

		DoDeclare_GetRandom();
		DoDeclare_GetShadowOffset();
		DoDeclare_GetLightSpacePosition();
		DoDeclare_FilterDirectional( type );
		DoDeclare_ComputeDirectionalShadow();
	}

	void Shadow::DeclarePoint( ShadowType type )
	{
		auto c3d_mapShadowPoint = m_writer.DeclUniform< SamplerCubeShadow >( MapShadowPoint );

		if ( type == ShadowType::ePoisson || type == ShadowType::eStratifiedPoisson )
		{
			auto c3d_poissonDisk = m_writer.DeclUniform< Vec2 >( cuT( "c3d_poissonDisk" ), 4,
			{
				vec2( -0.94201624_f, -0.39906216 ),
				vec2( 0.94558609_f, -0.76890725 ),
				vec2( -0.094184101_f, -0.92938870 ),
				vec2( 0.34495938_f, 0.29387760 )
			} );
		}

		DoDeclare_GetRandom();
		DoDeclare_GetShadowOffset();
		DoDeclare_FilterOnePoint( type );
		DoDeclare_ComputeOnePointShadow();
	}

	void Shadow::DeclareSpot( ShadowType type )
	{
		auto c3d_mapShadowSpot = m_writer.DeclUniform< Sampler2DShadow >( MapShadowSpot );

		if ( type == ShadowType::ePoisson || type == ShadowType::eStratifiedPoisson )
		{
			auto c3d_poissonDisk = m_writer.DeclUniform< Vec2 >( cuT( "c3d_poissonDisk" ), 4,
			{
				vec2( -0.94201624_f, -0.39906216 ),
				vec2( 0.94558609_f, -0.76890725 ),
				vec2( -0.094184101_f, -0.92938870 ),
				vec2( 0.34495938_f, 0.29387760 )
			} );
		}

		DoDeclare_GetRandom();
		DoDeclare_GetShadowOffset();
		DoDeclare_GetLightSpacePosition();
		DoDeclare_FilterOneSpot( type );
		DoDeclare_ComputeOneSpotShadow();
	}

	Float Shadow::ComputeDirectionalShadow( Mat4 const & lightMatrix
		, Vec3 const & worldSpacePosition
		, Vec3 const & lightDirection
		, Vec3 const & normal )
	{
		return m_computeDirectional( lightMatrix
			, worldSpacePosition
			, lightDirection
			, normal );
	}

	Float Shadow::ComputeSpotShadow( Mat4 const & lightMatrix
		, Vec3 const & worldSpacePosition
		, Vec3 const & lightDirection
		, Vec3 const & normal
		, Int const & index )
	{
		return m_computeSpot( lightMatrix
			, worldSpacePosition
			, lightDirection
			, normal
			, index );
	}

	Float Shadow::ComputePointShadow( Vec3 const & worldSpacePosition
		, Vec3 const & lightDirection
		, Vec3 const & normal
		, Int const & index )
	{
		return m_computePoint( worldSpacePosition
			, lightDirection
			, normal
			, index );
	}

	Float Shadow::ComputeSpotShadow( Mat4 const & lightMatrix
		, Vec3 const & worldSpacePosition
		, Vec3 const & lightDirection
		, Vec3 const & normal )
	{
		return m_computeOneSpot( lightMatrix
			, worldSpacePosition
			, lightDirection
			, normal );
	}

	Float Shadow::ComputePointShadow( Vec3 const & worldSpacePosition
		, Vec3 const & lightDirection
		, Vec3 const & normal )
	{
		return m_computeOnePoint( worldSpacePosition
			, lightDirection
			, normal );
	}

	void Shadow::DoDeclare_GetRandom()
	{
		m_getRandom = m_writer.ImplementFunction< Float >( cuT( "GetRandom" )
			, [this]( Vec4 const & seed )
			{
				auto p = m_writer.DeclLocale( cuT( "p" )
					, dot( seed, vec4( 12.9898_f, 78.233, 45.164, 94.673 ) ) );
				m_writer.Return( fract( sin( p ) * 43758.5453 ) );
			}
			, InVec4( &m_writer, cuT( "seed" ) ) );
	}

	void Shadow::DoDeclare_FilterDirectional( ShadowType type )
	{
		switch( type )
		{
		case ShadowType::eRaw:
			m_filterDirectional = m_writer.ImplementFunction< Float >( cuT( "FilterDirectional" )
				, [this]( Vec2 const & uv
					, Float const & depth )
				{
					auto c3d_mapShadowDirectional = m_writer.GetBuiltin< Sampler2DShadow >( Shadow::MapShadowDirectional );
					m_writer.Return( texture( c3d_mapShadowDirectional, vec3( uv, depth ) ) );
				}
				, InVec2( &m_writer, cuT( "uv" ) )
				, InFloat( &m_writer, cuT( "depth" ) ) );
			break;

		case ShadowType::ePoisson:
			m_filterDirectional = m_writer.ImplementFunction< Float >( cuT( "FilterDirectional" )
				, [this]( Vec2 const & uv
					, Float const & depth )
				{
					auto c3d_mapShadowDirectional = m_writer.GetBuiltin< Sampler2DShadow >( Shadow::MapShadowDirectional );
					auto c3d_poissonDisk = m_writer.GetBuiltin< Vec2 >( cuT( "c3d_poissonDisk" ), 4u );
					auto diffusion = m_writer.DeclLocale( cuT( "diffusion" )
						, Float( 4000.0 ) );
					auto visibility = m_writer.DeclLocale( cuT( "visibility" )
						, 1.0_f );

					for ( int i = 0; i < 4; i++ )
					{
						visibility -= Float( 0.2 ) * m_writer.Paren( 1.0_f - texture( c3d_mapShadowDirectional, vec3( uv + c3d_poissonDisk[i] / diffusion, depth ) ) );
					}

					m_writer.Return( visibility );
				}
				, InVec2( &m_writer, cuT( "uv" ) )
				, InFloat( &m_writer, cuT( "depth" ) ) );
			break;

		case ShadowType::eStratifiedPoisson:
			m_filterDirectional = m_writer.ImplementFunction< Float >( cuT( "FilterDirectional" )
				, [this]( Vec2 const & uv
					, Float const & depth )
				{
					auto c3d_mapShadowDirectional = m_writer.GetBuiltin< Sampler2DShadow >( Shadow::MapShadowDirectional );
					auto c3d_poissonDisk = m_writer.GetBuiltin< Vec2 >( cuT( "c3d_poissonDisk" ), 4u );
					auto gl_FragCoord = m_writer.DeclBuiltin< Vec3 >( cuT( "gl_FragCoord" ), 4u );
					auto pindex = m_writer.DeclLocale( cuT( "pindex" )
						, Int( 0 ) );
					auto diffusion = m_writer.DeclLocale( cuT( "diffusion" )
						, Float( 2800.0 ) );
					auto visibility = m_writer.DeclLocale( cuT( "visibility" )
						, 1.0_f );

					for ( int i = 0; i < 4; i++ )
					{
						pindex = m_writer.Cast< Int >( 16.0 * m_getRandom( vec4( gl_FragCoord.xy(), gl_FragCoord.y(), i ) ) ) % 16;
						visibility -= Float( 0.2 ) * m_writer.Paren( 1.0_f - texture( c3d_mapShadowDirectional, vec3( uv + c3d_poissonDisk[pindex] / diffusion, depth ) ) );
					}

					m_writer.Return( visibility );
				}
				, InVec2( &m_writer, cuT( "uv" ) )
				, InFloat( &m_writer, cuT( "depth" ) ) );
			break;

		default:
			FAILURE( "Unknown shadow type." );
			break;
		}
	}

	void Shadow::DoDeclare_FilterSpot( ShadowType type )
	{
		switch ( type )
		{
		case ShadowType::eRaw:
			m_filterSpot = m_writer.ImplementFunction< Float >( cuT( "FilterSpot" )
				, [this]( Vec2 const & uv
					, Float const & depth
					, Float const & index )
				{
					auto c3d_mapShadowSpot = m_writer.GetBuiltin< Sampler2DArrayShadow >( Shadow::MapShadowSpot );
					m_writer.Return( texture( c3d_mapShadowSpot, vec4( uv, index, depth ) ) );
				}
				, InVec2( &m_writer, cuT( "uv" ) )
				, InFloat( &m_writer, cuT( "depth" ) )
				, InFloat( &m_writer, cuT( "index" ) ) );
			break;

		case ShadowType::ePoisson:
			m_filterSpot = m_writer.ImplementFunction< Float >( cuT( "FilterSpot" )
				, [this]( Vec2 const & uv
					, Float const & depth
					, Float const & index )
				{
					auto c3d_mapShadowSpot = m_writer.GetBuiltin< Sampler2DArrayShadow >( Shadow::MapShadowSpot );
					auto c3d_poissonDisk = m_writer.GetBuiltin< Vec2 >( cuT( "c3d_poissonDisk" ), 4u );
					auto diffusion = m_writer.DeclLocale( cuT( "diffusion" )
						, 500.0_f );
					auto visibility = m_writer.DeclLocale( cuT( "visibility" )
						, 1.0_f );

					for ( int i = 0; i < 4; i++ )
					{
						visibility -= 0.2_f * m_writer.Paren( 1.0_f - texture( c3d_mapShadowSpot, vec4( uv + c3d_poissonDisk[i] / diffusion, index, depth ) ) );
					}

					m_writer.Return( visibility );
				}
				, InVec2( &m_writer, cuT( "uv" ) )
				, InFloat( &m_writer, cuT( "depth" ) )
				, InFloat( &m_writer, cuT( "index" ) ) );
			break;

		case ShadowType::eStratifiedPoisson:
			m_filterSpot = m_writer.ImplementFunction< Float >( cuT( "FilterSpot" )
				, [this]( Vec2 const & uv
					, Float const & depth
					, Float const & index )
				{
					auto c3d_mapShadowSpot = m_writer.GetBuiltin< Sampler2DArrayShadow >( Shadow::MapShadowSpot );
					auto c3d_poissonDisk = m_writer.GetBuiltin< Vec2 >( cuT( "c3d_poissonDisk" ), 4u );
					auto gl_FragCoord = m_writer.DeclBuiltin< Vec3 >( cuT( "gl_FragCoord" ), 4u );
					auto pindex = m_writer.DeclLocale( cuT( "pindex" )
						, 0_i );
					auto diffusion = m_writer.DeclLocale( cuT( "diffusion" )
						, 700.0_f );
					auto visibility = m_writer.DeclLocale( cuT( "visibility" )
						, 1.0_f );

					for ( int i = 0; i < 4; i++ )
					{
						pindex = m_writer.Cast< Int >( 16.0 * m_getRandom( vec4( gl_FragCoord.xy(), gl_FragCoord.y(), i ) ) ) % 16;
						visibility -= 0.2_f * m_writer.Paren( 1.0_f - texture( c3d_mapShadowSpot, vec4( uv + c3d_poissonDisk[pindex] / diffusion, index, depth ) ) );
					}

					m_writer.Return( visibility );
				}
				, InVec2( &m_writer, cuT( "uv" ) )
				, InFloat( &m_writer, cuT( "depth" ) )
				, InFloat( &m_writer, cuT( "index" ) ) );
			break;

		default:
			FAILURE( "Unknown shadow type." );
			break;
		}
	}

	void Shadow::DoDeclare_FilterPoint( ShadowType type )
	{
		switch ( type )
		{
		case ShadowType::eRaw:
			m_filterPoint = m_writer.ImplementFunction< Float >( cuT( "FilterPoint" )
				, [this]( Vec3 const & direction
					, Float const & depth
					, Int const & index )
				{
					auto c3d_mapShadowPoint = m_writer.GetBuiltin< SamplerCubeShadow >( MapShadowPoint, PointShadowMapCount );
					m_writer.Return( texture( c3d_mapShadowPoint[index], vec4( direction, depth ) ) );
				}
				, InVec3( &m_writer, cuT( "direction" ) )
				, InFloat( &m_writer, cuT( "depth" ) )
				, InInt( &m_writer, cuT( "index" ) ) );
			break;

		case ShadowType::ePoisson:
			m_filterPoint = m_writer.ImplementFunction< Float >( cuT( "FilterPoint" )
				, [this]( Vec3 const & direction
					, Float const & depth
					, Int const & index )
				{
					auto c3d_mapShadowPoint = m_writer.GetBuiltin< SamplerCubeShadow >( MapShadowPoint, PointShadowMapCount );
					auto c3d_poissonDisk = m_writer.GetBuiltin< Vec2 >( cuT( "c3d_poissonDisk" ), 4u );
					auto diffusion = m_writer.DeclLocale( cuT( "diffusion" )
						, 500.0_f );
					auto visibility = m_writer.DeclLocale( cuT( "visibility" )
						, 1.0_f );

					for ( int i = 0; i < 4; i++ )
					{
						visibility -= 0.2_f * m_writer.Paren( 1.0_f - texture( c3d_mapShadowPoint[index], vec4( direction.xy() + c3d_poissonDisk[i] / diffusion, direction.z(), depth ) ) );
					}

					m_writer.Return( visibility );
				}
				, InVec3( &m_writer, cuT( "direction" ) )
				, InFloat( &m_writer, cuT( "depth" ) )
				, InInt( &m_writer, cuT( "index" ) ) );
			break;

		case ShadowType::eStratifiedPoisson:
			m_filterPoint = m_writer.ImplementFunction< Float >( cuT( "FilterPoint" )
				, [this]( Vec3 const & direction
					, Float const & depth
					, Int const & index )
				{
					auto c3d_mapShadowPoint = m_writer.GetBuiltin< SamplerCubeShadow >( MapShadowPoint, PointShadowMapCount );
					auto c3d_poissonDisk = m_writer.GetBuiltin< Vec2 >( cuT( "c3d_poissonDisk" ), 4u );
					auto gl_FragCoord = m_writer.DeclBuiltin< Vec3 >( cuT( "gl_FragCoord" ), 4u );
					auto pindex = m_writer.DeclLocale( cuT( "pindex" )
						, 0_i );
					auto diffusion = m_writer.DeclLocale( cuT( "diffusion" )
						, 700.0_f );
					auto visibility = m_writer.DeclLocale( cuT( "visibility" )
						, 1.0_f );

					for ( int i = 0; i < 4; i++ )
					{
						pindex = m_writer.Cast< Int >( 16.0 * m_getRandom( vec4( gl_FragCoord.xy(), gl_FragCoord.y(), i ) ) ) % 16;
						visibility -= 0.2_f * m_writer.Paren( 1.0_f - texture( c3d_mapShadowPoint[index], vec4( direction.xy() + c3d_poissonDisk[pindex] / diffusion, direction.z(), depth ) ) );
					}

					m_writer.Return( visibility );
				}
				, InVec3( &m_writer, cuT( "direction" ) )
				, InFloat( &m_writer, cuT( "depth" ) )
				, InInt( &m_writer, cuT( "index" ) ) );
			break;

		default:
			FAILURE( "Unknown shadow type." );
			break;
		}
	}

	void Shadow::DoDeclare_FilterOneSpot( ShadowType type )
	{
		switch ( type )
		{
		case ShadowType::eRaw:
			m_filterOneSpot = m_writer.ImplementFunction< Float >( cuT( "FilterSpot" )
				, [this]( Vec2 const & uv
					, Float const & depth )
				{
					auto c3d_mapShadowSpot = m_writer.GetBuiltin< Sampler2DShadow >( Shadow::MapShadowSpot );
					m_writer.Return( texture( c3d_mapShadowSpot, vec3( uv, depth ) ) );
				}
				, InVec2( &m_writer, cuT( "uv" ) )
				, InFloat( &m_writer, cuT( "depth" ) ) );
			break;

		case ShadowType::ePoisson:
			m_filterOneSpot = m_writer.ImplementFunction< Float >( cuT( "FilterSpot" )
				, [this]( Vec2 const & uv
					, Float const & depth )
				{
					auto c3d_mapShadowSpot = m_writer.GetBuiltin< Sampler2DShadow >( Shadow::MapShadowSpot );
					auto c3d_poissonDisk = m_writer.GetBuiltin< Vec2 >( cuT( "c3d_poissonDisk" ), 4u );
					auto diffusion = m_writer.DeclLocale( cuT( "diffusion" )
						, 500.0_f );
					auto visibility = m_writer.DeclLocale( cuT( "visibility" )
						, 1.0_f );

					for ( int i = 0; i < 4; i++ )
					{
						visibility -= 0.2_f * m_writer.Paren( 1.0_f - texture( c3d_mapShadowSpot, vec3( uv + c3d_poissonDisk[i] / diffusion, depth ) ) );
					}

					m_writer.Return( visibility );
				}
				, InVec2( &m_writer, cuT( "uv" ) )
				, InFloat( &m_writer, cuT( "depth" ) ) );
			break;

		case ShadowType::eStratifiedPoisson:
			m_filterOneSpot = m_writer.ImplementFunction< Float >( cuT( "FilterSpot" )
				, [this]( Vec2 const & uv
					, Float const & depth )
				{
					auto c3d_mapShadowSpot = m_writer.GetBuiltin< Sampler2DShadow >( Shadow::MapShadowSpot );
					auto c3d_poissonDisk = m_writer.GetBuiltin< Vec2 >( cuT( "c3d_poissonDisk" ), 4u );
					auto gl_FragCoord = m_writer.DeclBuiltin< Vec3 >( cuT( "gl_FragCoord" ), 4u );
					auto pindex = m_writer.DeclLocale( cuT( "pindex" )
						, 0_i );
					auto diffusion = m_writer.DeclLocale( cuT( "diffusion" )
						, 700.0_f );
					auto visibility = m_writer.DeclLocale( cuT( "visibility" )
						, 1.0_f );

					for ( int i = 0; i < 4; i++ )
					{
						pindex = m_writer.Cast< Int >( 16.0 * m_getRandom( vec4( gl_FragCoord.xy(), gl_FragCoord.y(), i ) ) ) % 16;
						visibility -= 0.2_f * m_writer.Paren( 1.0_f - texture( c3d_mapShadowSpot, vec3( uv + c3d_poissonDisk[pindex] / diffusion, depth ) ) );
					}

					m_writer.Return( visibility );
				}
				, InVec2( &m_writer, cuT( "uv" ) )
				, InFloat( &m_writer, cuT( "depth" ) ) );
			break;

		default:
			FAILURE( "Unknown shadow type." );
			break;
		}
	}

	void Shadow::DoDeclare_FilterOnePoint( ShadowType type )
	{
		switch ( type )
		{
		case ShadowType::eRaw:
			m_filterOnePoint = m_writer.ImplementFunction< Float >( cuT( "FilterPoint" )
				, [this]( Vec3 const & direction
					, Float const & depth )
				{
					auto c3d_mapShadowPoint = m_writer.GetBuiltin< SamplerCubeShadow >( MapShadowPoint );
					m_writer.Return( texture( c3d_mapShadowPoint, vec4( direction, depth ) ) );
				}
				, InVec3( &m_writer, cuT( "direction" ) )
				, InFloat( &m_writer, cuT( "depth" ) ) );
			break;

		case ShadowType::ePoisson:
			m_filterOnePoint = m_writer.ImplementFunction< Float >( cuT( "FilterPoint" )
				, [this]( Vec3 const & direction
					, Float const & depth )
				{
					auto c3d_mapShadowPoint = m_writer.GetBuiltin< SamplerCubeShadow >( MapShadowPoint );
					auto c3d_poissonDisk = m_writer.GetBuiltin< Vec2 >( cuT( "c3d_poissonDisk" ), 4u );
					auto diffusion = m_writer.DeclLocale( cuT( "diffusion" )
						, 500.0_f );
					auto visibility = m_writer.DeclLocale( cuT( "visibility" )
						, 1.0_f );

					for ( int i = 0; i < 4; i++ )
					{
						visibility -= 0.2_f * m_writer.Paren( 1.0_f - texture( c3d_mapShadowPoint, vec4( direction.xy() + c3d_poissonDisk[i] / diffusion, direction.z(), depth ) ) );
					}

					m_writer.Return( visibility );
				}
				, InVec3( &m_writer, cuT( "direction" ) )
				, InFloat( &m_writer, cuT( "depth" ) ) );
			break;

		case ShadowType::eStratifiedPoisson:
			m_filterOnePoint = m_writer.ImplementFunction< Float >( cuT( "FilterPoint" )
				, [this]( Vec3 const & direction
					, Float const & depth )
				{
					auto c3d_mapShadowPoint = m_writer.GetBuiltin< SamplerCubeShadow >( MapShadowPoint );
					auto c3d_poissonDisk = m_writer.GetBuiltin< Vec2 >( cuT( "c3d_poissonDisk" ), 4u );
					auto gl_FragCoord = m_writer.DeclBuiltin< Vec3 >( cuT( "gl_FragCoord" ), 4u );
					auto pindex = m_writer.DeclLocale( cuT( "pindex" )
						, 0_i );
					auto diffusion = m_writer.DeclLocale( cuT( "diffusion" )
						, 700.0_f );
					auto visibility = m_writer.DeclLocale( cuT( "visibility" )
						, 1.0_f );

					for ( int i = 0; i < 4; i++ )
					{
						pindex = m_writer.Cast< Int >( 16.0 * m_getRandom( vec4( gl_FragCoord.xy(), gl_FragCoord.y(), i ) ) ) % 16;
						visibility -= 0.2_f * m_writer.Paren( 1.0_f - texture( c3d_mapShadowPoint, vec4( direction.xy() + c3d_poissonDisk[pindex] / diffusion, direction.z(), depth ) ) );
					}

					m_writer.Return( visibility );
				}
				, InVec3( &m_writer, cuT( "direction" ) )
				, InFloat( &m_writer, cuT( "depth" ) ) );
			break;

		default:
			FAILURE( "Unknown shadow type." );
			break;
		}
	}

	void Shadow::DoDeclare_GetShadowOffset()
	{
		m_getShadowOffset = m_writer.ImplementFunction< Float >( cuT( "GetShadowOffset" )
			, [this]( Vec3 const & normal
				, Vec3 const & lightDirection
				, Float const & minOffset
				, Float const & maxSlopeOffset )
			{
				auto cosAlpha = m_writer.DeclLocale( cuT( "cosAlpha" )
					, clamp( dot( normal, lightDirection ), 0.0_f, 1.0_f ) );
				auto offset = m_writer.DeclLocale( cuT( "offset" )
					, 1.0_f - clamp( dot( normal, normalize( lightDirection ) ), 0.0_f, 1.0_f ) );
				m_writer.Return( minOffset + maxSlopeOffset * offset );
			}
			, InVec3( &m_writer, cuT( "normal" ) )
			, InVec3( &m_writer, cuT( "lightDirection" ) )
			, InFloat( &m_writer, cuT( "minOffset" ) )
			, InFloat( &m_writer, cuT( "maxSlopeOffset" ) ) );
	}

	void Shadow::DoDeclare_GetLightSpacePosition()
	{
		m_getLightSpacePosition = m_writer.ImplementFunction< Vec3 >( cuT( "GetLightSpacePosition" )
			, [this]( Mat4 const & lightMatrix
				, Vec3 const & worldSpacePosition
				, Vec3 const & lightDirection
				, Vec3 const & normal )
			{
				// Offset the vertex position along its normal.
				auto lightSpacePosition = m_writer.DeclLocale( cuT( "lightSpacePosition" )
					, lightMatrix * vec4( worldSpacePosition, 1.0_f ) );
				// Perspective divide (result in range [-1,1]).
				lightSpacePosition.xyz() = lightSpacePosition.xyz() / lightSpacePosition.w();
				// Now put the position in range [0,1].
				lightSpacePosition.xyz() = m_writer.Paren( lightSpacePosition.xyz() * Float( 0.5 ) ) + Float( 0.5 );

				m_writer.Return( lightSpacePosition.xyz() );
			}
			, InParam< Mat4 >( &m_writer, cuT( "lightMatrix" ) )
			, InVec3( &m_writer, cuT( "worldSpacePosition" ) )
			, InVec3( &m_writer, cuT( "lightDirection" ) )
			, InVec3( &m_writer, cuT( "normal" ) ) );
	}

	void Shadow::DoDeclare_ComputeDirectionalShadow()
	{
		m_computeDirectional = m_writer.ImplementFunction< Float >( cuT( "ComputeDirectionalShadow" )
			, [this]( Mat4 const & lightMatrix
				, Vec3 const & worldSpacePosition
				, Vec3 const & lightDirection
				, Vec3 const & normal )
			{
				auto constexpr minOffset = 0.0001f;
				auto constexpr maxSlopeOffset = 0.005f;
				auto offset = m_writer.DeclLocale( cuT( "offset" )
					, m_getShadowOffset( normal, lightDirection, Float( minOffset ), Float( maxSlopeOffset ) ) );
				auto lightSpacePosition = m_writer.DeclLocale( cuT( "lightSpacePosition" )
					, m_getLightSpacePosition( lightMatrix, worldSpacePosition, lightDirection, normal ) );
				auto visibility = m_writer.DeclLocale( cuT( "visibility" )
					, 1.0_f );
				visibility = m_filterDirectional( lightSpacePosition.xy(), lightSpacePosition.z() - offset );
				m_writer.Return( 1.0_f - clamp( visibility, 0.0, 1.0 ) );
			}
			, InParam< Mat4 >( &m_writer, cuT( "lightMatrix" ) )
			, InVec3( &m_writer, cuT( "worldSpacePosition" ) )
			, InVec3( &m_writer, cuT( "lightDirection" ) )
			, InVec3( &m_writer, cuT( "normal" ) ) );
	}

	void Shadow::DoDeclare_ComputeSpotShadow()
	{
		m_computeSpot = m_writer.ImplementFunction< Float >( cuT( "ComputeSpotShadow" )
			, [this]( Mat4 const & lightMatrix
				, Vec3 const & worldSpacePosition
				, Vec3 const & lightDirection
				, Vec3 const & normal
				, Int const & index )
			{
				auto constexpr minOffset = 0.0001f;
				auto constexpr maxSlopeOffset = 0.001f;
				auto offset = m_writer.DeclLocale( cuT( "offset" )
					, m_getShadowOffset( normal, lightDirection, Float( minOffset ), Float( maxSlopeOffset ) ) );
				auto lightSpacePosition = m_writer.DeclLocale( cuT( "lightSpacePosition" )
					, m_getLightSpacePosition( lightMatrix, worldSpacePosition, lightDirection, normal ) );
				auto visibility = m_writer.DeclLocale( cuT( "visibility" )
					, m_filterSpot( lightSpacePosition.xy(), lightSpacePosition.z() - offset, m_writer.Cast< Float >( index ) ) );
				m_writer.Return( 1.0_f - clamp( visibility, 0.0, 1.0 ) );
			}
			, InParam< Mat4 >( &m_writer, cuT( "lightMatrix" ) )
			, InVec3( &m_writer, cuT( "worldSpacePosition" ) )
			, InVec3( &m_writer, cuT( "lightDirection" ) )
			, InVec3( &m_writer, cuT( "normal" ) )
			, InInt( &m_writer, cuT( "index" ) ) );
	}

	void Shadow::DoDeclare_ComputePointShadow()
	{
		m_computePoint = m_writer.ImplementFunction< Float >( cuT( "ComputePointShadow" )
			, [this]( Vec3 const & worldSpacePosition
			, Vec3 const & lightPosition
			, Vec3 const & normal
			, Int const & index )
			{
				auto constexpr minOffset = 0.0001f;
				auto constexpr maxSlopeOffset = 0.002f;
				auto vertexToLight = m_writer.DeclLocale( cuT( "vertexToLight" )
					, worldSpacePosition - lightPosition );
				auto offset = m_writer.DeclLocale( cuT( "offset" )
					, m_getShadowOffset( normal, vertexToLight, Float( minOffset ), Float( maxSlopeOffset ) ) );
				auto visibility = m_writer.DeclLocale( cuT( "visibility" )
					, m_filterPoint( vertexToLight, length( vertexToLight ) / 4000.0_f - offset, index ) );
				m_writer.Return( 1.0_f - clamp( visibility, 0.0, 1.0 ) );
			}
			, InVec3( &m_writer, cuT( "worldSpacePosition" ) )
			, InVec3( &m_writer, cuT( "lightPosition" ) )
			, InVec3( &m_writer, cuT( "normal" ) )
			, InInt( &m_writer, cuT( "index" ) ) );
	}

	void Shadow::DoDeclare_ComputeOneSpotShadow()
	{
		m_computeOneSpot = m_writer.ImplementFunction< Float >( cuT( "ComputeSpotShadow" )
			, [this]( Mat4 const & lightMatrix
				, Vec3 const & worldSpacePosition
				, Vec3 const & lightDirection
				, Vec3 const & normal )
			{
				auto constexpr minOffset = 0.0001f;
				auto constexpr maxSlopeOffset = 0.001f;
				auto offset = m_writer.DeclLocale( cuT( "offset" )
					, m_getShadowOffset( normal, lightDirection, Float( minOffset ), Float( maxSlopeOffset ) ) );
				auto lightSpacePosition = m_writer.DeclLocale( cuT( "lightSpacePosition" )
					, m_getLightSpacePosition( lightMatrix, worldSpacePosition, lightDirection, normal ) );
				auto visibility = m_writer.DeclLocale( cuT( "visibility" )
					, 1.0_f );
				visibility = m_filterOneSpot( lightSpacePosition.xy(), lightSpacePosition.z() - offset );
				m_writer.Return( 1.0_f - clamp( visibility, 0.0, 1.0 ) );
			}
			, InParam< Mat4 >( &m_writer, cuT( "lightMatrix" ) )
			, InVec3( &m_writer, cuT( "worldSpacePosition" ) )
			, InVec3( &m_writer, cuT( "lightDirection" ) )
			, InVec3( &m_writer, cuT( "normal" ) ) );
	}

	void Shadow::DoDeclare_ComputeOnePointShadow()
	{
		m_computeOnePoint = m_writer.ImplementFunction< Float >( cuT( "ComputePointShadow" )
			, [this]( Vec3 const & worldSpacePosition
			, Vec3 const & lightPosition
			, Vec3 const & normal )
			{
				auto constexpr minOffset = 0.0001f;
				auto constexpr maxSlopeOffset = 0.002f;
				auto vertexToLight = m_writer.DeclLocale( cuT( "vertexToLight" )
					, worldSpacePosition - lightPosition );
				auto offset = m_writer.DeclLocale( cuT( "offset" )
					, m_getShadowOffset( normal, vertexToLight, Float( minOffset ), Float( maxSlopeOffset ) ) );
				auto visibility = m_writer.DeclLocale( cuT( "visibility" )
					, m_filterOnePoint( vertexToLight, length( vertexToLight ) / 4000.0_f - offset ) );
				m_writer.Return( 1.0_f - clamp( visibility, 0.0, 1.0 ) );
			}
			, InVec3( &m_writer, cuT( "worldSpacePosition" ) )
			, InVec3( &m_writer, cuT( "lightPosition" ) )
			, InVec3( &m_writer, cuT( "normal" ) ) );
	}
}
