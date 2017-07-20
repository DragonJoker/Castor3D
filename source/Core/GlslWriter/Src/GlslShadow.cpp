#include "GlslShadow.hpp"

using namespace Castor;

namespace GLSL
{
	Castor::String const Shadow::MapShadowDirectional = cuT( "c3d_mapShadowDirectional" );
	Castor::String const Shadow::MapShadowSpot = cuT( "c3d_mapShadowSpot" );
	Castor::String const Shadow::MapShadowPoint = cuT( "c3d_mapShadowPoint" );

	Shadow::Shadow( GlslWriter & p_writer )
		: m_writer{ p_writer }
	{
	}

	void Shadow::Declare( ShadowType p_type )
	{
		auto c3d_mapShadowDirectional = m_writer.DeclUniform< Sampler2DShadow >( MapShadowDirectional );
		auto c3d_mapShadowSpot = m_writer.DeclUniform< Sampler2DArrayShadow >( MapShadowSpot );
		auto c3d_mapShadowPoint = m_writer.DeclUniform< SamplerCubeShadow >( MapShadowPoint, PointShadowMapCount );

		if ( p_type == ShadowType::ePoisson || p_type == ShadowType::eStratifiedPoisson )
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
		DoDeclare_FilterDirectional( p_type );
		DoDeclare_FilterSpot( p_type );
		DoDeclare_FilterPoint( p_type );
		DoDeclare_ComputeDirectionalShadow();
		DoDeclare_ComputeSpotShadow();
		DoDeclare_ComputePointShadow();
	}

	void Shadow::DeclareDirectional( ShadowType p_type )
	{
		auto c3d_mapShadowDirectional = m_writer.DeclUniform< Sampler2DShadow >( MapShadowDirectional );

		if ( p_type == ShadowType::ePoisson || p_type == ShadowType::eStratifiedPoisson )
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
		DoDeclare_FilterDirectional( p_type );
		DoDeclare_ComputeDirectionalShadow();
	}

	void Shadow::DeclarePoint( ShadowType p_type )
	{
		auto c3d_mapShadowPoint = m_writer.DeclUniform< SamplerCubeShadow >( MapShadowPoint );

		if ( p_type == ShadowType::ePoisson || p_type == ShadowType::eStratifiedPoisson )
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
		DoDeclare_FilterOnePoint( p_type );
		DoDeclare_ComputeOnePointShadow();
	}

	void Shadow::DeclareSpot( ShadowType p_type )
	{
		auto c3d_mapShadowSpot = m_writer.DeclUniform< Sampler2DShadow >( MapShadowSpot );

		if ( p_type == ShadowType::ePoisson || p_type == ShadowType::eStratifiedPoisson )
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
		DoDeclare_FilterOneSpot( p_type );
		DoDeclare_ComputeOneSpotShadow();
	}

	Float Shadow::ComputeDirectionalShadow( Mat4 const & p_lightMatrix
		, Vec3 const & p_worldSpacePosition
		, Vec3 const & p_lightDirection
		, Vec3 const & p_normal )
	{
		return m_computeDirectional( p_lightMatrix
			, p_worldSpacePosition
			, p_lightDirection
			, p_normal );
	}

	Float Shadow::ComputeSpotShadow( Mat4 const & p_lightMatrix
		, Vec3 const & p_worldSpacePosition
		, Vec3 const & p_lightDirection
		, Vec3 const & p_normal
		, Int const & p_index )
	{
		return m_computeSpot( p_lightMatrix
			, p_worldSpacePosition
			, p_lightDirection
			, p_normal
			, p_index );
	}

	Float Shadow::ComputePointShadow( Vec3 const & p_worldSpacePosition
		, Vec3 const & p_lightDirection
		, Vec3 const & p_normal
		, Int const & p_index )
	{
		return m_computePoint( p_worldSpacePosition
			, p_lightDirection
			, p_normal
			, p_index );
	}

	Float Shadow::ComputeSpotShadow( Mat4 const & p_lightMatrix
		, Vec3 const & p_worldSpacePosition
		, Vec3 const & p_lightDirection
		, Vec3 const & p_normal )
	{
		return m_computeOneSpot( p_lightMatrix
			, p_worldSpacePosition
			, p_lightDirection
			, p_normal );
	}

	Float Shadow::ComputePointShadow( Vec3 const & p_worldSpacePosition
		, Vec3 const & p_lightDirection
		, Vec3 const & p_normal )
	{
		return m_computeOnePoint( p_worldSpacePosition
			, p_lightDirection
			, p_normal );
	}

	void Shadow::DoDeclare_GetRandom()
	{
		m_getRandom = m_writer.ImplementFunction< Float >( cuT( "GetRandom" )
			, [this]( Vec4 const & p_seed )
			{
				auto p = m_writer.DeclLocale( cuT( "p" ), dot( p_seed, vec4( 12.9898_f, 78.233, 45.164, 94.673 ) ) );
				m_writer.Return( fract( sin( p ) * 43758.5453 ) );
			}
			, InVec4( &m_writer, cuT( "p_seed" ) ) );
	}

	void Shadow::DoDeclare_FilterDirectional( ShadowType p_type )
	{
		switch( p_type )
		{
		case ShadowType::eRaw:
			m_filterDirectional = m_writer.ImplementFunction< Float >( cuT( "FilterDirectional" )
				, [this]( Vec2 const & p_uv
					, Float const & p_depth )
				{
					auto c3d_mapShadowDirectional = m_writer.GetBuiltin< Sampler2DShadow >( Shadow::MapShadowDirectional );
					m_writer.Return( texture( c3d_mapShadowDirectional, vec3( p_uv, p_depth ) ) );
				}
				, InVec2( &m_writer, cuT( "p_uv" ) )
				, InFloat( &m_writer, cuT( "p_depth" ) ) );
			break;

		case ShadowType::ePoisson:
			m_filterDirectional = m_writer.ImplementFunction< Float >( cuT( "FilterDirectional" )
				, [this]( Vec2 const & p_uv
					, Float const & p_depth )
				{
					auto c3d_mapShadowDirectional = m_writer.GetBuiltin< Sampler2DShadow >( Shadow::MapShadowDirectional );
					auto c3d_poissonDisk = m_writer.GetBuiltin< Vec2 >( cuT( "c3d_poissonDisk" ), 4u );
					auto diffusion = m_writer.DeclLocale( cuT( "diffusion" ), Float( 4000.0 ) );
					auto visibility = m_writer.DeclLocale( cuT( "visibility" ), Float( 1 ) );

					for ( int i = 0; i < 4; i++ )
					{
						visibility -= Float( 0.2 ) * m_writer.Paren( Float( 1 ) - texture( c3d_mapShadowDirectional, vec3( p_uv + c3d_poissonDisk[i] / diffusion, p_depth ) ) );
					}

					m_writer.Return( visibility );
				}
				, InVec2( &m_writer, cuT( "p_uv" ) )
				, InFloat( &m_writer, cuT( "p_depth" ) ) );
			break;

		case ShadowType::eStratifiedPoisson:
			m_filterDirectional = m_writer.ImplementFunction< Float >( cuT( "FilterDirectional" )
				, [this]( Vec2 const & p_uv
					, Float const & p_depth )
				{
					auto c3d_mapShadowDirectional = m_writer.GetBuiltin< Sampler2DShadow >( Shadow::MapShadowDirectional );
					auto c3d_poissonDisk = m_writer.GetBuiltin< Vec2 >( cuT( "c3d_poissonDisk" ), 4u );
					auto gl_FragCoord = m_writer.DeclBuiltin< Vec3 >( cuT( "gl_FragCoord" ), 4u );
					auto pindex = m_writer.DeclLocale( cuT( "pindex" ), Int( 0 ) );
					auto diffusion = m_writer.DeclLocale( cuT( "diffusion" ), Float( 2800.0 ) );
					auto visibility = m_writer.DeclLocale( cuT( "visibility" ), Float( 1 ) );

					for ( int i = 0; i < 4; i++ )
					{
						pindex = m_writer.Cast< Int >( 16.0 * m_getRandom( vec4( gl_FragCoord.xy(), gl_FragCoord.y(), i ) ) ) % 16;
						visibility -= Float( 0.2 ) * m_writer.Paren( Float( 1 ) - texture( c3d_mapShadowDirectional, vec3( p_uv + c3d_poissonDisk[pindex] / diffusion, p_depth ) ) );
					}

					m_writer.Return( visibility );
				}
				, InVec2( &m_writer, cuT( "p_uv" ) )
				, InFloat( &m_writer, cuT( "p_depth" ) ) );
			break;

		default:
			FAILURE( "Unknown shadow type." );
			break;
		}
	}

	void Shadow::DoDeclare_FilterSpot( ShadowType p_type )
	{
		switch ( p_type )
		{
		case ShadowType::eRaw:
			m_filterSpot = m_writer.ImplementFunction< Float >( cuT( "FilterSpot" )
				, [this]( Vec2 const & p_uv
					, Float const & p_depth
					, Float const & p_index )
				{
					auto c3d_mapShadowSpot = m_writer.GetBuiltin< Sampler2DArrayShadow >( Shadow::MapShadowSpot );
					m_writer.Return( texture( c3d_mapShadowSpot, vec4( p_uv, p_index, p_depth ) ) );
				}
				, InVec2( &m_writer, cuT( "p_uv" ) )
				, InFloat( &m_writer, cuT( "p_depth" ) )
				, InFloat( &m_writer, cuT( "p_index" ) ) );
			break;

		case ShadowType::ePoisson:
			m_filterSpot = m_writer.ImplementFunction< Float >( cuT( "FilterSpot" )
				, [this]( Vec2 const & p_uv
					, Float const & p_depth
					, Float const & p_index )
				{
					auto c3d_mapShadowSpot = m_writer.GetBuiltin< Sampler2DArrayShadow >( Shadow::MapShadowSpot );
					auto c3d_poissonDisk = m_writer.GetBuiltin< Vec2 >( cuT( "c3d_poissonDisk" ), 4u );
					auto diffusion = m_writer.DeclLocale( cuT( "diffusion" ), 500.0_f );
					auto visibility = m_writer.DeclLocale( cuT( "visibility" ), Float( 1 ) );

					for ( int i = 0; i < 4; i++ )
					{
						visibility -= 0.2_f * m_writer.Paren( 1.0_f - texture( c3d_mapShadowSpot, vec4( p_uv + c3d_poissonDisk[i] / diffusion, p_index, p_depth ) ) );
					}

					m_writer.Return( visibility );
				}
				, InVec2( &m_writer, cuT( "p_uv" ) )
				, InFloat( &m_writer, cuT( "p_depth" ) )
				, InFloat( &m_writer, cuT( "p_index" ) ) );
			break;

		case ShadowType::eStratifiedPoisson:
			m_filterSpot = m_writer.ImplementFunction< Float >( cuT( "FilterSpot" )
				, [this]( Vec2 const & p_uv
					, Float const & p_depth
					, Float const & p_index )
				{
					auto c3d_mapShadowSpot = m_writer.GetBuiltin< Sampler2DArrayShadow >( Shadow::MapShadowSpot );
					auto c3d_poissonDisk = m_writer.GetBuiltin< Vec2 >( cuT( "c3d_poissonDisk" ), 4u );
					auto gl_FragCoord = m_writer.DeclBuiltin< Vec3 >( cuT( "gl_FragCoord" ), 4u );
					auto pindex = m_writer.DeclLocale( cuT( "pindex" ), 0_i );
					auto diffusion = m_writer.DeclLocale( cuT( "diffusion" ), 700.0_f );
					auto visibility = m_writer.DeclLocale( cuT( "visibility" ), Float( 1 ) );

					for ( int i = 0; i < 4; i++ )
					{
						pindex = m_writer.Cast< Int >( 16.0 * m_getRandom( vec4( gl_FragCoord.xy(), gl_FragCoord.y(), i ) ) ) % 16;
						visibility -= 0.2_f * m_writer.Paren( 1.0_f - texture( c3d_mapShadowSpot, vec4( p_uv + c3d_poissonDisk[pindex] / diffusion, p_index, p_depth ) ) );
					}

					m_writer.Return( visibility );
				}
				, InVec2( &m_writer, cuT( "p_uv" ) )
				, InFloat( &m_writer, cuT( "p_depth" ) )
				, InFloat( &m_writer, cuT( "p_index" ) ) );
			break;

		default:
			FAILURE( "Unknown shadow type." );
			break;
		}
	}

	void Shadow::DoDeclare_FilterPoint( ShadowType p_type )
	{
		switch ( p_type )
		{
		case ShadowType::eRaw:
			m_filterPoint = m_writer.ImplementFunction< Float >( cuT( "FilterPoint" )
				, [this]( Vec3 const & p_direction
					, Float const & p_depth
					, Int const & p_index )
				{
					auto c3d_mapShadowPoint = m_writer.GetBuiltin< SamplerCubeShadow >( MapShadowPoint, PointShadowMapCount );
					m_writer.Return( texture( c3d_mapShadowPoint[p_index], vec4( p_direction, p_depth ) ) );
				}
				, InVec3( &m_writer, cuT( "p_direction" ) )
				, InFloat( &m_writer, cuT( "p_depth" ) )
				, InInt( &m_writer, cuT( "p_index" ) ) );
			break;

		case ShadowType::ePoisson:
			m_filterPoint = m_writer.ImplementFunction< Float >( cuT( "FilterPoint" )
				, [this]( Vec3 const & p_direction
					, Float const & p_depth
					, Int const & p_index )
				{
					auto c3d_mapShadowPoint = m_writer.GetBuiltin< SamplerCubeShadow >( MapShadowPoint, PointShadowMapCount );
					auto c3d_poissonDisk = m_writer.GetBuiltin< Vec2 >( cuT( "c3d_poissonDisk" ), 4u );
					auto diffusion = m_writer.DeclLocale( cuT( "diffusion" ), 500.0_f );
					auto visibility = m_writer.DeclLocale( cuT( "visibility" ), Float( 1 ) );

					for ( int i = 0; i < 4; i++ )
					{
						visibility -= 0.2_f * m_writer.Paren( 1.0_f - texture( c3d_mapShadowPoint[p_index], vec4( p_direction.xy() + c3d_poissonDisk[i] / diffusion, p_direction.z(), p_depth ) ) );
					}

					m_writer.Return( visibility );
				}
				, InVec3( &m_writer, cuT( "p_direction" ) )
				, InFloat( &m_writer, cuT( "p_depth" ) )
				, InInt( &m_writer, cuT( "p_index" ) ) );
			break;

		case ShadowType::eStratifiedPoisson:
			m_filterPoint = m_writer.ImplementFunction< Float >( cuT( "FilterPoint" )
				, [this]( Vec3 const & p_direction
					, Float const & p_depth
					, Int const & p_index )
				{
					auto c3d_mapShadowPoint = m_writer.GetBuiltin< SamplerCubeShadow >( MapShadowPoint, PointShadowMapCount );
					auto c3d_poissonDisk = m_writer.GetBuiltin< Vec2 >( cuT( "c3d_poissonDisk" ), 4u );
					auto gl_FragCoord = m_writer.DeclBuiltin< Vec3 >( cuT( "gl_FragCoord" ), 4u );
					auto pindex = m_writer.DeclLocale( cuT( "pindex" ), 0_i );
					auto diffusion = m_writer.DeclLocale( cuT( "diffusion" ), 700.0_f );
					auto visibility = m_writer.DeclLocale( cuT( "visibility" ), Float( 1 ) );

					for ( int i = 0; i < 4; i++ )
					{
						pindex = m_writer.Cast< Int >( 16.0 * m_getRandom( vec4( gl_FragCoord.xy(), gl_FragCoord.y(), i ) ) ) % 16;
						visibility -= 0.2_f * m_writer.Paren( 1.0_f - texture( c3d_mapShadowPoint[p_index], vec4( p_direction.xy() + c3d_poissonDisk[pindex] / diffusion, p_direction.z(), p_depth ) ) );
					}

					m_writer.Return( visibility );
				}
				, InVec3( &m_writer, cuT( "p_direction" ) )
				, InFloat( &m_writer, cuT( "p_depth" ) )
				, InInt( &m_writer, cuT( "p_index" ) ) );
			break;

		default:
			FAILURE( "Unknown shadow type." );
			break;
		}
	}

	void Shadow::DoDeclare_FilterOneSpot( ShadowType p_type )
	{
		switch ( p_type )
		{
		case ShadowType::eRaw:
			m_filterOneSpot = m_writer.ImplementFunction< Float >( cuT( "FilterSpot" )
				, [this]( Vec2 const & p_uv
					, Float const & p_depth )
				{
					auto c3d_mapShadowSpot = m_writer.GetBuiltin< Sampler2DShadow >( Shadow::MapShadowSpot );
					m_writer.Return( texture( c3d_mapShadowSpot, vec3( p_uv, p_depth ) ) );
				}
				, InVec2( &m_writer, cuT( "p_uv" ) )
				, InFloat( &m_writer, cuT( "p_depth" ) ) );
			break;

		case ShadowType::ePoisson:
			m_filterOneSpot = m_writer.ImplementFunction< Float >( cuT( "FilterSpot" )
				, [this]( Vec2 const & p_uv
					, Float const & p_depth )
				{
					auto c3d_mapShadowSpot = m_writer.GetBuiltin< Sampler2DShadow >( Shadow::MapShadowSpot );
					auto c3d_poissonDisk = m_writer.GetBuiltin< Vec2 >( cuT( "c3d_poissonDisk" ), 4u );
					auto diffusion = m_writer.DeclLocale( cuT( "diffusion" ), 500.0_f );
					auto visibility = m_writer.DeclLocale( cuT( "visibility" ), Float( 1 ) );

					for ( int i = 0; i < 4; i++ )
					{
						visibility -= 0.2_f * m_writer.Paren( 1.0_f - texture( c3d_mapShadowSpot, vec3( p_uv + c3d_poissonDisk[i] / diffusion, p_depth ) ) );
					}

					m_writer.Return( visibility );
				}
				, InVec2( &m_writer, cuT( "p_uv" ) )
				, InFloat( &m_writer, cuT( "p_depth" ) ) );
			break;

		case ShadowType::eStratifiedPoisson:
			m_filterOneSpot = m_writer.ImplementFunction< Float >( cuT( "FilterSpot" )
				, [this]( Vec2 const & p_uv
					, Float const & p_depth )
				{
					auto c3d_mapShadowSpot = m_writer.GetBuiltin< Sampler2DShadow >( Shadow::MapShadowSpot );
					auto c3d_poissonDisk = m_writer.GetBuiltin< Vec2 >( cuT( "c3d_poissonDisk" ), 4u );
					auto gl_FragCoord = m_writer.DeclBuiltin< Vec3 >( cuT( "gl_FragCoord" ), 4u );
					auto pindex = m_writer.DeclLocale( cuT( "pindex" ), 0_i );
					auto diffusion = m_writer.DeclLocale( cuT( "diffusion" ), 700.0_f );
					auto visibility = m_writer.DeclLocale( cuT( "visibility" ), Float( 1 ) );

					for ( int i = 0; i < 4; i++ )
					{
						pindex = m_writer.Cast< Int >( 16.0 * m_getRandom( vec4( gl_FragCoord.xy(), gl_FragCoord.y(), i ) ) ) % 16;
						visibility -= 0.2_f * m_writer.Paren( 1.0_f - texture( c3d_mapShadowSpot, vec3( p_uv + c3d_poissonDisk[pindex] / diffusion, p_depth ) ) );
					}

					m_writer.Return( visibility );
				}
				, InVec2( &m_writer, cuT( "p_uv" ) )
				, InFloat( &m_writer, cuT( "p_depth" ) ) );
			break;

		default:
			FAILURE( "Unknown shadow type." );
			break;
		}
	}

	void Shadow::DoDeclare_FilterOnePoint( ShadowType p_type )
	{
		switch ( p_type )
		{
		case ShadowType::eRaw:
			m_filterOnePoint = m_writer.ImplementFunction< Float >( cuT( "FilterPoint" )
				, [this]( Vec3 const & p_direction
					, Float const & p_depth )
				{
					auto c3d_mapShadowPoint = m_writer.GetBuiltin< SamplerCubeShadow >( MapShadowPoint );
					m_writer.Return( texture( c3d_mapShadowPoint, vec4( p_direction, p_depth ) ) );
				}
				, InVec3( &m_writer, cuT( "p_direction" ) )
				, InFloat( &m_writer, cuT( "p_depth" ) ) );
			break;

		case ShadowType::ePoisson:
			m_filterOnePoint = m_writer.ImplementFunction< Float >( cuT( "FilterPoint" )
				, [this]( Vec3 const & p_direction
					, Float const & p_depth )
				{
					auto c3d_mapShadowPoint = m_writer.GetBuiltin< SamplerCubeShadow >( MapShadowPoint );
					auto c3d_poissonDisk = m_writer.GetBuiltin< Vec2 >( cuT( "c3d_poissonDisk" ), 4u );
					auto diffusion = m_writer.DeclLocale( cuT( "diffusion" ), 500.0_f );
					auto visibility = m_writer.DeclLocale( cuT( "visibility" ), Float( 1 ) );

					for ( int i = 0; i < 4; i++ )
					{
						visibility -= 0.2_f * m_writer.Paren( 1.0_f - texture( c3d_mapShadowPoint, vec4( p_direction.xy() + c3d_poissonDisk[i] / diffusion, p_direction.z(), p_depth ) ) );
					}

					m_writer.Return( visibility );
				}
				, InVec3( &m_writer, cuT( "p_direction" ) )
				, InFloat( &m_writer, cuT( "p_depth" ) ) );
			break;

		case ShadowType::eStratifiedPoisson:
			m_filterOnePoint = m_writer.ImplementFunction< Float >( cuT( "FilterPoint" )
				, [this]( Vec3 const & p_direction
					, Float const & p_depth )
				{
					auto c3d_mapShadowPoint = m_writer.GetBuiltin< SamplerCubeShadow >( MapShadowPoint );
					auto c3d_poissonDisk = m_writer.GetBuiltin< Vec2 >( cuT( "c3d_poissonDisk" ), 4u );
					auto gl_FragCoord = m_writer.DeclBuiltin< Vec3 >( cuT( "gl_FragCoord" ), 4u );
					auto pindex = m_writer.DeclLocale( cuT( "pindex" ), 0_i );
					auto diffusion = m_writer.DeclLocale( cuT( "diffusion" ), 700.0_f );
					auto visibility = m_writer.DeclLocale( cuT( "visibility" ), Float( 1 ) );

					for ( int i = 0; i < 4; i++ )
					{
						pindex = m_writer.Cast< Int >( 16.0 * m_getRandom( vec4( gl_FragCoord.xy(), gl_FragCoord.y(), i ) ) ) % 16;
						visibility -= 0.2_f * m_writer.Paren( 1.0_f - texture( c3d_mapShadowPoint, vec4( p_direction.xy() + c3d_poissonDisk[pindex] / diffusion, p_direction.z(), p_depth ) ) );
					}

					m_writer.Return( visibility );
				}
				, InVec3( &m_writer, cuT( "p_direction" ) )
				, InFloat( &m_writer, cuT( "p_depth" ) ) );
			break;

		default:
			FAILURE( "Unknown shadow type." );
			break;
		}
	}

	void Shadow::DoDeclare_GetShadowOffset()
	{
		m_getShadowOffset = m_writer.ImplementFunction< Vec2 >( cuT( "GetShadowOffset" )
			, [this]( Vec3 const & p_normal
				, Vec3 const & p_lightDirection )
			{
				auto cosAlpha = m_writer.DeclLocale( cuT( "cosAlpha" ), clamp( dot( p_normal, p_lightDirection ), 0.0_f, 1.0_f ) );
				auto offsetScaleN = m_writer.DeclLocale( cuT( "offsetScaleN" ), sqrt( 1.0_f - cosAlpha * cosAlpha ) ); // sin( acos( cosAlpha ) )
				auto offsetScaleL = m_writer.DeclLocale( cuT( "offsetScaleL" ), offsetScaleN / cosAlpha ); // tan( acos( cosAlpha ) )
				m_writer.Return( vec2( offsetScaleN, min( 2.0_f, offsetScaleL ) ) );
			}
			, InVec3( &m_writer, cuT( "p_normal" ) )
			, InVec3( &m_writer, cuT( "p_lightDirection" ) ) );
	}

	void Shadow::DoDeclare_GetLightSpacePosition()
	{
		m_getLightSpacePosition = m_writer.ImplementFunction< Vec3 >( cuT( "GetLightSpacePosition" )
			, [this]( Mat4 const & p_lightMatrix
				, Vec3 const & p_worldSpacePosition
				, Vec3 const & p_lightDirection
				, Vec3 const & p_normal )
			{
				// Offset the vertex position along its normal.
				auto offset = m_writer.DeclLocale( cuT( "offset" ), m_getShadowOffset( p_normal, p_lightDirection ) * 2.0_f );
				auto worldSpace = m_writer.DeclLocale( cuT( "worldSpace" ), p_worldSpacePosition + m_writer.Paren( p_normal * offset.x() ) );
				auto lightSpacePosition = m_writer.DeclLocale( cuT( "lightSpacePosition" ), p_lightMatrix * vec4( worldSpace, 1.0_f ) );
				// Perspective divide (result in range [-1,1]).
				lightSpacePosition.xyz() = lightSpacePosition.xyz() / lightSpacePosition.w();
				// Now put the position in range [0,1].
				lightSpacePosition.xyz() = m_writer.Paren( lightSpacePosition.xyz() * Float( 0.5 ) ) + Float( 0.5 );

				m_writer.Return( lightSpacePosition.xyz() );
			}
			, InParam< Mat4 >( &m_writer, cuT( "p_lightMatrix" ) )
			, InVec3( &m_writer, cuT( "p_worldSpacePosition" ) )
			, InVec3( &m_writer, cuT( "p_lightDirection" ) )
			, InVec3( &m_writer, cuT( "p_normal" ) ) );
	}

	void Shadow::DoDeclare_ComputeDirectionalShadow()
	{
		m_computeDirectional = m_writer.ImplementFunction< Float >( cuT( "ComputeDirectionalShadow" )
			, [this]( Mat4 const & p_lightMatrix
				, Vec3 const & p_worldSpacePosition
				, Vec3 const & p_lightDirection
				, Vec3 const & p_normal )
			{
				auto lightSpacePosition = m_writer.DeclLocale( cuT( "lightSpacePosition" ), m_getLightSpacePosition( p_lightMatrix, p_worldSpacePosition, p_lightDirection, p_normal ) );
				auto visibility = m_writer.DeclLocale( cuT( "visibility" ), Float( 1 ) );
				visibility = m_filterDirectional( lightSpacePosition.xy(), lightSpacePosition.z() );
				m_writer.Return( 1.0_f - clamp( visibility, 0.0, 1.0 ) );
			}
			, InParam< Mat4 >( &m_writer, cuT( "p_lightMatrix" ) )
			, InVec3( &m_writer, cuT( "p_worldSpacePosition" ) )
			, InVec3( &m_writer, cuT( "p_lightDirection" ) )
			, InVec3( &m_writer, cuT( "p_normal" ) ) );
	}

	void Shadow::DoDeclare_ComputeSpotShadow()
	{
		m_computeSpot = m_writer.ImplementFunction< Float >( cuT( "ComputeSpotShadow" )
			, [this]( Mat4 const & p_lightMatrix
				, Vec3 const & p_worldSpacePosition
				, Vec3 const & p_lightDirection
				, Vec3 const & p_normal
				, Int const & p_index )
			{
				auto lightSpacePosition = m_writer.DeclLocale( cuT( "lightSpacePosition" ), m_getLightSpacePosition( p_lightMatrix, p_worldSpacePosition, p_lightDirection, p_normal ) );
				auto index = m_writer.DeclLocale( cuT( "index" ), m_writer.Cast< Float >( p_index ) );
				auto visibility = m_writer.DeclLocale( cuT( "visibility" ), 1.0_f );
				visibility = m_filterSpot( lightSpacePosition.xy(), lightSpacePosition.z(), index );
				m_writer.Return( 1.0_f - clamp( visibility, 0.0, 1.0 ) );
			}
			, InParam< Mat4 >( &m_writer, cuT( "p_lightMatrix" ) )
			, InVec3( &m_writer, cuT( "p_worldSpacePosition" ) )
			, InVec3( &m_writer, cuT( "p_lightDirection" ) )
			, InVec3( &m_writer, cuT( "p_normal" ) )
			, InInt( &m_writer, cuT( "p_index" ) ) );
	}

	void Shadow::DoDeclare_ComputePointShadow()
	{
		m_computePoint = m_writer.ImplementFunction< Float >( cuT( "ComputePointShadow" )
			, [this]( Vec3 const & p_worldSpacePosition
			, Vec3 const & p_lightPosition
			, Vec3 const & p_normal
			, Int const & p_index )
			{
				auto vertexToLight = m_writer.DeclLocale( cuT( "vertexToLight" ), normalize( p_worldSpacePosition - p_lightPosition ) );
				auto offset = m_writer.DeclLocale( cuT( "offset" ), m_getShadowOffset( p_normal, p_worldSpacePosition - p_lightPosition ) );
				auto worldSpace = m_writer.DeclLocale( cuT( "worldSpace" ), p_worldSpacePosition + m_writer.Paren( p_normal * offset.x() ) );
				auto visibility = m_writer.DeclLocale( cuT( "visibility" ), 1.0_f );
				vertexToLight = worldSpace - p_lightPosition;
				visibility = m_filterPoint( vertexToLight, length( vertexToLight ) / 4000.0_f, p_index );
				m_writer.Return( 1.0_f - clamp( visibility, 0.0, 1.0 ) );
			}
			, InVec3( &m_writer, cuT( "p_worldSpacePosition" ) )
			, InVec3( &m_writer, cuT( "p_lightPosition" ) )
			, InVec3( &m_writer, cuT( "p_normal" ) )
			, InInt( &m_writer, cuT( "p_index" ) ) );
	}

	void Shadow::DoDeclare_ComputeOneSpotShadow()
	{
		m_computeOneSpot = m_writer.ImplementFunction< Float >( cuT( "ComputeSpotShadow" )
			, [this]( Mat4 const & p_lightMatrix
				, Vec3 const & p_worldSpacePosition
				, Vec3 const & p_lightDirection
				, Vec3 const & p_normal )
			{
				auto lightSpacePosition = m_writer.DeclLocale( cuT( "lightSpacePosition" ), m_getLightSpacePosition( p_lightMatrix, p_worldSpacePosition, p_lightDirection, p_normal ) );
				auto visibility = m_writer.DeclLocale( cuT( "visibility" ), 1.0_f );
				visibility = m_filterOneSpot( lightSpacePosition.xy(), lightSpacePosition.z() );
				m_writer.Return( 1.0_f - clamp( visibility, 0.0, 1.0 ) );
			}
			, InParam< Mat4 >( &m_writer, cuT( "p_lightMatrix" ) )
			, InVec3( &m_writer, cuT( "p_worldSpacePosition" ) )
			, InVec3( &m_writer, cuT( "p_lightDirection" ) )
			, InVec3( &m_writer, cuT( "p_normal" ) ) );
	}

	void Shadow::DoDeclare_ComputeOnePointShadow()
	{
		m_computeOnePoint = m_writer.ImplementFunction< Float >( cuT( "ComputePointShadow" )
			, [this]( Vec3 const & p_worldSpacePosition
			, Vec3 const & p_lightPosition
			, Vec3 const & p_normal )
			{
				auto vertexToLight = m_writer.DeclLocale( cuT( "vertexToLight" ), normalize( p_worldSpacePosition - p_lightPosition ) );
				auto offset = m_writer.DeclLocale( cuT( "offset" ), m_getShadowOffset( p_normal, p_worldSpacePosition - p_lightPosition ) );
				auto worldSpace = m_writer.DeclLocale( cuT( "worldSpace" ), p_worldSpacePosition + m_writer.Paren( p_normal * offset.x() ) );
				auto visibility = m_writer.DeclLocale( cuT( "visibility" ), 1.0_f );
				vertexToLight = worldSpace - p_lightPosition;
				visibility = m_filterOnePoint( vertexToLight, length( vertexToLight ) / 4000.0_f );
				m_writer.Return( 1.0_f - clamp( visibility, 0.0, 1.0 ) );
			}
			, InVec3( &m_writer, cuT( "p_worldSpacePosition" ) )
			, InVec3( &m_writer, cuT( "p_lightPosition" ) )
			, InVec3( &m_writer, cuT( "p_normal" ) ) );
	}
}
