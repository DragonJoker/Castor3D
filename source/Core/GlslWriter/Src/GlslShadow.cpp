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
		auto c3d_mapShadowDirectional = m_writer.GetUniform< Sampler2DShadow >( MapShadowDirectional );
		auto c3d_mapShadowSpot = m_writer.GetUniform< Sampler2DArrayShadow >( MapShadowSpot );
		auto c3d_mapShadowPoint = m_writer.GetUniform< SamplerCubeShadow >( MapShadowPoint );

		if ( p_type == ShadowType::ePoisson || p_type == ShadowType::eStratifiedPoisson )
		{
			auto c3d_poissonDisk = m_writer.GetUniform< Vec2 >( cuT( "c3d_poissonDisk" ), 4,
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
		DoDeclare_FilterRaw();
		DoDeclare_FilterPoisson();
		DoDeclare_FilterStratifiedPoisson();
		DoDeclare_ComputeDirectionalShadow( p_type );
		DoDeclare_ComputeSpotShadow( p_type );
		DoDeclare_ComputePointShadow( p_type );
	}

	Float Shadow::ComputeDirectionalShadow( Mat4 const & p_lightMatrix
		, Vec3 const & p_worldSpacePosition
		, Vec3 const & p_lightDirection
		, Vec3 const & p_normal )
	{
		return WriteFunctionCall< Float >( &m_writer
			, cuT( "ComputeDirectionalShadow" )
			, p_lightMatrix
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
		return WriteFunctionCall< Float >( &m_writer
			, cuT( "ComputeSpotShadow" )
			, p_lightMatrix
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
		return WriteFunctionCall< Float >( &m_writer
			, cuT( "ComputePointShadow" )
			, p_worldSpacePosition
			, p_lightDirection
			, p_normal
			, p_index );
	}

	Float Shadow::GetRandom( Vec4 const & p_seed )
	{
		return WriteFunctionCall< Float >( &m_writer
			, cuT( "GetRandom" )
			, p_seed );
	}

	Vec2 Shadow::GetShadowOffset( Vec3 const & p_normal
		, Vec3 const & p_lightDirection )
	{
		return WriteFunctionCall< Vec2 >( &m_writer
			, cuT( "GetShadowOffset" )
			, p_normal
			, p_lightDirection );
	}

	Float Shadow::FilterRaw( Vec2 const & p_uv
		, Float const & p_depth )
	{
		return WriteFunctionCall< Vec2 >( &m_writer
			, cuT( "FilterRawDirectional" )
			, p_uv
			, p_depth );
	}

	Float Shadow::FilterRaw( Vec2 const & p_uv
		, Float const & p_depth
		, Float const & p_index )
	{
		return WriteFunctionCall< Vec2 >( &m_writer
			, cuT( "FilterRawSpot" )
			, p_uv
			, p_depth
			, p_index );
	}

	Float Shadow::FilterRaw( Vec3 const & p_direction
		, Float const & p_depth
		, Float const & p_index )
	{
		return WriteFunctionCall< Vec2 >( &m_writer
			, cuT( "FilterRawPoint" )
			, p_direction
			, p_depth
			, p_index );
	}

	Float Shadow::FilterPoisson( Vec2 const & p_uv
		, Float const & p_depth )
	{
		return WriteFunctionCall< Vec2 >( &m_writer
			, cuT( "FilterPoissonDirectional" )
			, p_uv
			, p_depth );
	}

	Float Shadow::FilterPoisson( Vec2 const & p_uv
		, Float const & p_depth
		, Float const & p_index )
	{
		return WriteFunctionCall< Vec2 >( &m_writer
			, cuT( "FilterPoissonSpot" )
			, p_uv
			, p_depth
			, p_index );
	}

	Float Shadow::FilterPoisson( Vec3 const & p_direction
		, Float const & p_depth
		, Float const & p_index )
	{
		return WriteFunctionCall< Vec2 >( &m_writer
			, cuT( "FilterPoissonPoint" )
			, p_direction
			, p_depth
			, p_index );
	}

	Float Shadow::FilterStratifiedPoisson( Vec2 const & p_uv
		, Float const & p_depth )
	{
		return WriteFunctionCall< Vec2 >( &m_writer
			, cuT( "FilterStratifiedPoissonDirectional" )
			, p_uv
			, p_depth );
	}

	Float Shadow::FilterStratifiedPoisson( Vec2 const & p_uv
		, Float const & p_depth
		, Float const & p_index )
	{
		return WriteFunctionCall< Vec2 >( &m_writer
			, cuT( "FilterStratifiedPoissonSpot" )
			, p_uv
			, p_depth
			, p_index );
	}

	Float Shadow::FilterStratifiedPoisson( Vec3 const & p_direction
		, Float const & p_depth
		, Float const & p_index )
	{
		return WriteFunctionCall< Vec2 >( &m_writer
			, cuT( "FilterStratifiedPoissonPoint" )
			, p_direction
			, p_depth
			, p_index );
	}

	Vec3 Shadow::GetLightSpacePosition( Mat4 const & p_lightMatrix
		, Vec3 const & p_worldSpacePosition
		, Vec3 const & p_lightDirection
		, Vec3 const & p_normal )
	{
		return WriteFunctionCall< Vec3 >( &m_writer
			, cuT( "GetLightSpacePosition" )
			, p_lightMatrix
			, p_worldSpacePosition
			, p_lightDirection
			, p_normal );
	}

	void Shadow::DoDeclare_GetRandom()
	{
		m_writer.ImplementFunction< Float >( cuT( "GetRandom" )
			, [this]( Vec4 const & p_seed )
			{
				auto l_dot = m_writer.GetLocale( cuT( "l_dot" ), dot( p_seed, vec4( 12.9898_f, 78.233, 45.164, 94.673 ) ) );
				m_writer.Return( fract( sin( l_dot ) * 43758.5453 ) );
			}
			, InParam< Vec4 >( &m_writer, cuT( "p_seed" ) ) );
	}

	void Shadow::DoDeclare_FilterRaw()
	{
		m_writer.ImplementFunction< Float >( cuT( "FilterRawDirectional" )
			, [this]( Vec2 const & p_uv
				, Float const & p_depth )
			{
				auto c3d_mapShadowDirectional = m_writer.GetBuiltin< Sampler2DShadow >( Shadow::MapShadowDirectional );
				m_writer.Return( texture( c3d_mapShadowDirectional, vec3( p_uv, p_depth ) ) );
			}
			, InParam< Vec2 >( &m_writer, cuT( "p_uv" ) )
			, InParam< Float >( &m_writer, cuT( "p_depth" ) ) );

		m_writer.ImplementFunction< Float >( cuT( "FilterRawSpot" )
			, [this]( Vec2 const & p_uv
				, Float const & p_depth
				, Float const & p_index )
			{
				auto c3d_mapShadowSpot = m_writer.GetBuiltin< Sampler2DArrayShadow >( Shadow::MapShadowSpot );
				m_writer.Return( texture( c3d_mapShadowSpot, vec4( p_uv, p_index, p_depth ) ) );
			}
			, InParam< Vec2 >( &m_writer, cuT( "p_uv" ) )
			, InParam< Float >( &m_writer, cuT( "p_depth" ) )
			, InParam< Float >( &m_writer, cuT( "p_index" ) ) );

		m_writer.ImplementFunction< Float >( cuT( "FilterRawPoint" )
			, [this]( Vec3 const & p_direction
				, Float const & p_depth
				, Float const & p_index )
			{
				auto c3d_mapShadowPoint = m_writer.GetBuiltin< SamplerCubeShadow >( Shadow::MapShadowPoint );
				m_writer.Return( texture( c3d_mapShadowPoint, vec4( p_direction, p_depth ) ) );
			}
			, InParam< Vec3 >( &m_writer, cuT( "p_direction" ) )
			, InParam< Float >( &m_writer, cuT( "p_depth" ) )
			, InParam< Float >( &m_writer, cuT( "p_index" ) ) );
	}

	void Shadow::DoDeclare_FilterPoisson()
	{
		m_writer.ImplementFunction< Float >( cuT( "FilterPoissonDirectional" )
			, [this]( Vec2 const & p_uv
				, Float const & p_depth )
			{
				auto c3d_mapShadowDirectional = m_writer.GetBuiltin< Sampler2DShadow >( Shadow::MapShadowDirectional );
				auto c3d_poissonDisk = m_writer.GetBuiltin< Vec2 >( cuT( "c3d_poissonDisk" ), 4u );
				auto l_diffusion = m_writer.GetLocale( cuT( "l_diffusion" ), Float( 4000.0 ) );
				auto l_visibility = m_writer.GetLocale( cuT( "l_visibility" ), Float( 1 ) );

				for ( int i = 0; i < 4; i++ )
				{
					l_visibility -= Float( 0.2 ) * m_writer.Paren( Float( 1 ) - texture( c3d_mapShadowDirectional, vec3( p_uv + c3d_poissonDisk[i] / l_diffusion, p_depth ) ) );
				}

				m_writer.Return( l_visibility );
			}
			, InParam< Vec2 >( &m_writer, cuT( "p_uv" ) )
			, InParam< Float >( &m_writer, cuT( "p_depth" ) ) );

		m_writer.ImplementFunction< Float >( cuT( "FilterPoissonSpot" )
			, [this]( Vec2 const & p_uv
				, Float const & p_depth
				, Float const & p_index )
			{
				auto c3d_mapShadowSpot = m_writer.GetBuiltin< Sampler2DArrayShadow >( Shadow::MapShadowSpot );
				auto c3d_poissonDisk = m_writer.GetBuiltin< Vec2 >( cuT( "c3d_poissonDisk" ), 4u );
				auto l_diffusion = m_writer.GetLocale( cuT( "l_diffusion" ), 500.0_f );
				auto l_visibility = m_writer.GetLocale( cuT( "l_visibility" ), Float( 1 ) );

				for ( int i = 0; i < 4; i++ )
				{
					l_visibility -= 0.2_f * m_writer.Paren( 1.0_f - texture( c3d_mapShadowSpot, vec4( p_uv + c3d_poissonDisk[i] / l_diffusion, p_index, p_depth ) ) );
				}

				m_writer.Return( l_visibility );
			}
			, InParam< Vec2 >( &m_writer, cuT( "p_uv" ) )
			, InParam< Float >( &m_writer, cuT( "p_depth" ) )
			, InParam< Float >( &m_writer, cuT( "p_index" ) ) );

		m_writer.ImplementFunction< Float >( cuT( "FilterPoissonPoint" )
			, [this]( Vec3 const & p_direction
				, Float const & p_depth
				, Float const & p_index )
			{
				auto c3d_mapShadowPoint = m_writer.GetBuiltin< SamplerCubeShadow >( Shadow::MapShadowPoint );
				auto c3d_poissonDisk = m_writer.GetBuiltin< Vec2 >( cuT( "c3d_poissonDisk" ), 4u );
				auto l_diffusion = m_writer.GetLocale( cuT( "l_diffusion" ), 500.0_f );
				auto l_visibility = m_writer.GetLocale( cuT( "l_visibility" ), Float( 1 ) );

				for ( int i = 0; i < 4; i++ )
				{
					l_visibility -= 0.2_f * m_writer.Paren( 1.0_f - texture( c3d_mapShadowPoint, vec4( p_direction.xy() + c3d_poissonDisk[i] / l_diffusion, p_direction.z(), p_depth ) ) );
				}

				m_writer.Return( l_visibility );
			}
			, InParam< Vec3 >( &m_writer, cuT( "p_direction" ) )
			, InParam< Float >( &m_writer, cuT( "p_depth" ) )
			, InParam< Float >( &m_writer, cuT( "p_index" ) ) );
	}

	void Shadow::DoDeclare_FilterStratifiedPoisson()
	{
		m_writer.ImplementFunction< Float >( cuT( "FilterStratifiedPoissonDirectional" )
			, [this]( Vec2 const & p_uv
				, Float const & p_depth )
			{
				auto c3d_mapShadowDirectional = m_writer.GetBuiltin< Sampler2DShadow >( Shadow::MapShadowDirectional );
				auto c3d_poissonDisk = m_writer.GetBuiltin< Vec2 >( cuT( "c3d_poissonDisk" ), 4u );
				auto gl_FragCoord = m_writer.GetBuiltin< Vec3 >( cuT( "gl_FragCoord" ), 4u );
				auto l_pindex = m_writer.GetLocale( cuT( "l_pindex" ), Int( 0 ) );
				auto l_diffusion = m_writer.GetLocale( cuT( "l_diffusion" ), Float( 2800.0 ) );
				auto l_visibility = m_writer.GetLocale( cuT( "l_visibility" ), Float( 1 ) );

				for ( int i = 0; i < 4; i++ )
				{
					l_pindex = m_writer.Cast< Int >( 16.0 * GetRandom( vec4( gl_FragCoord.xy(), gl_FragCoord.y(), i ) ) ) % 16;
					l_visibility -= Float( 0.2 ) * m_writer.Paren( Float( 1 ) - texture( c3d_mapShadowDirectional, vec3( p_uv + c3d_poissonDisk[l_pindex] / l_diffusion, p_depth ) ) );
				}

				m_writer.Return( l_visibility );
			}
			, InParam< Vec2 >( &m_writer, cuT( "p_uv" ) )
			, InParam< Float >( &m_writer, cuT( "p_depth" ) ) );

		m_writer.ImplementFunction< Float >( cuT( "FilterStratifiedPoissonSpot" )
			, [this]( Vec2 const & p_uv
				, Float const & p_depth
				, Float const & p_index )
			{
				auto c3d_mapShadowSpot = m_writer.GetBuiltin< Sampler2DArrayShadow >( Shadow::MapShadowSpot );
				auto c3d_poissonDisk = m_writer.GetBuiltin< Vec2 >( cuT( "c3d_poissonDisk" ), 4u );
				auto gl_FragCoord = m_writer.GetBuiltin< Vec3 >( cuT( "gl_FragCoord" ), 4u );
				auto l_pindex = m_writer.GetLocale( cuT( "l_pindex" ), 0_i );
				auto l_diffusion = m_writer.GetLocale( cuT( "l_diffusion" ), 700.0_f );
				auto l_visibility = m_writer.GetLocale( cuT( "l_visibility" ), Float( 1 ) );

				for ( int i = 0; i < 4; i++ )
				{
					l_pindex = m_writer.Cast< Int >( 16.0 * GetRandom( vec4( gl_FragCoord.xy(), gl_FragCoord.y(), i ) ) ) % 16;
					l_visibility -= 0.2_f * m_writer.Paren( 1.0_f - texture( c3d_mapShadowSpot, vec4( p_uv + c3d_poissonDisk[l_pindex] / l_diffusion, p_index, p_depth ) ) );
				}

				m_writer.Return( l_visibility );
			}
			, InParam< Vec2 >( &m_writer, cuT( "p_uv" ) )
			, InParam< Float >( &m_writer, cuT( "p_depth" ) )
			, InParam< Float >( &m_writer, cuT( "p_index" ) ) );

		m_writer.ImplementFunction< Float >( cuT( "FilterStratifiedPoissonPoint" )
			, [this]( Vec3 const & p_direction
				, Float const & p_depth
				, Float const & p_index )
			{
				auto c3d_mapShadowPoint = m_writer.GetBuiltin< SamplerCubeShadow >( Shadow::MapShadowPoint );
				auto c3d_poissonDisk = m_writer.GetBuiltin< Vec2 >( cuT( "c3d_poissonDisk" ), 4u );
				auto gl_FragCoord = m_writer.GetBuiltin< Vec3 >( cuT( "gl_FragCoord" ), 4u );
				auto l_pindex = m_writer.GetLocale( cuT( "l_pindex" ), 0_i );
				auto l_diffusion = m_writer.GetLocale( cuT( "l_diffusion" ), 700.0_f );
				auto l_visibility = m_writer.GetLocale( cuT( "l_visibility" ), Float( 1 ) );

				for ( int i = 0; i < 4; i++ )
				{
					l_pindex = m_writer.Cast< Int >( 16.0 * GetRandom( vec4( gl_FragCoord.xy(), gl_FragCoord.y(), i ) ) ) % 16;
					l_visibility -= 0.2_f * m_writer.Paren( 1.0_f - texture( c3d_mapShadowPoint, vec4( p_direction.xy() + c3d_poissonDisk[l_pindex] / l_diffusion, p_direction.z(), p_depth ) ) );
				}

				m_writer.Return( l_visibility );
			}
			, InParam< Vec3 >( &m_writer, cuT( "p_direction" ) )
			, InParam< Float >( &m_writer, cuT( "p_depth" ) )
			, InParam< Float >( &m_writer, cuT( "p_index" ) ) );
	}

	void Shadow::DoDeclare_GetShadowOffset()
	{
		m_writer.ImplementFunction< Vec2 >( cuT( "GetShadowOffset" )
			, [this]( Vec3 const & p_normal
				, Vec3 const & p_lightDirection )
			{
				auto l_cosAlpha = m_writer.GetLocale( cuT( "l_cosAlpha" ), clamp( dot( p_normal, p_lightDirection ), 0.0_f, 1.0_f ) );
				auto l_offsetScaleN = m_writer.GetLocale( cuT( "l_offsetScaleN" ), sqrt( 1.0_f - l_cosAlpha * l_cosAlpha ) ); // sin( acos( l_cosAlpha ) )
				auto l_offsetScaleL = m_writer.GetLocale( cuT( "l_offsetScaleL" ), l_offsetScaleN / l_cosAlpha ); // tan( acos( l_cosAlpha ) )
				m_writer.Return( vec2( l_offsetScaleN, min( 2.0_f, l_offsetScaleL ) ) );
			}
			, InParam< Vec3 >( &m_writer, cuT( "p_normal" ) )
			, InParam< Vec3 >( &m_writer, cuT( "p_lightDirection" ) ) );
	}

	void Shadow::DoDeclare_GetLightSpacePosition()
	{
		m_writer.ImplementFunction< Vec3 >( cuT( "GetLightSpacePosition" )
			, [this]( Mat4 const & p_lightMatrix
				, Vec3 const & p_worldSpacePosition
				, Vec3 const & p_lightDirection
				, Vec3 const & p_normal )
			{
				// Offset the vertex position along its normal.
				auto l_offset = m_writer.GetLocale( cuT( "l_offset" ), GetShadowOffset( p_normal, p_lightDirection ) * 2.0_f );
				auto l_worldSpace = m_writer.GetLocale( cuT( "l_worldSpace" ), p_worldSpacePosition + m_writer.Paren( p_normal * l_offset.x() ) );
				auto l_lightSpacePosition = m_writer.GetLocale( cuT( "l_lightSpacePosition" ), p_lightMatrix * vec4( l_worldSpace, 1.0_f ) );
				// Perspective divide (result in range [-1,1]).
				l_lightSpacePosition.xyz() = l_lightSpacePosition.xyz() / l_lightSpacePosition.w();
				// Now put the position in range [0,1].
				l_lightSpacePosition.xyz() = m_writer.Paren( l_lightSpacePosition.xyz() * Float( 0.5 ) ) + Float( 0.5 );

				m_writer.Return( l_lightSpacePosition.xyz() );
			}
			, InParam< Mat4 >( &m_writer, cuT( "p_lightMatrix" ) )
			, InParam< Vec3 >( &m_writer, cuT( "p_worldSpacePosition" ) )
			, InParam< Vec3 >( &m_writer, cuT( "p_lightDirection" ) )
			, InParam< Vec3 >( &m_writer, cuT( "p_normal" ) ) );
	}

	void Shadow::DoDeclare_ComputeDirectionalShadow( ShadowType p_type )
	{
		m_writer.ImplementFunction< Float >( cuT( "ComputeDirectionalShadow" )
			, [this, &p_type]( Mat4 const & p_lightMatrix
				, Vec3 const & p_worldSpacePosition
				, Vec3 const & p_lightDirection
				, Vec3 const & p_normal )
			{
				auto l_lightSpacePosition = m_writer.GetLocale( cuT( "l_lightSpacePosition" ), GetLightSpacePosition( p_lightMatrix, p_worldSpacePosition, p_lightDirection, p_normal ) );
				auto l_visibility = m_writer.GetLocale( cuT( "l_visibility" ), Float( 1 ) );

				if ( p_type == ShadowType::eRaw )
				{
					l_visibility = FilterRaw( l_lightSpacePosition.xy(), l_lightSpacePosition.z() );
				}
				else if ( p_type == ShadowType::ePoisson )
				{
					l_visibility = FilterPoisson( l_lightSpacePosition.xy(), l_lightSpacePosition.z() );
				}
				else if ( p_type == ShadowType::eStratifiedPoisson )
				{
					l_visibility = FilterStratifiedPoisson( l_lightSpacePosition.xy(), l_lightSpacePosition.z() );
				}

				m_writer.Return( clamp( l_visibility, 0.0, 1.0 ) );
			}
			, InParam< Mat4 >( &m_writer, cuT( "p_lightMatrix" ) )
			, InParam< Vec3 >( &m_writer, cuT( "p_worldSpacePosition" ) )
			, InParam< Vec3 >( &m_writer, cuT( "p_lightDirection" ) )
			, InParam< Vec3 >( &m_writer, cuT( "p_normal" ) ) );
	}

	void Shadow::DoDeclare_ComputeSpotShadow( ShadowType p_type )
	{
		m_writer.ImplementFunction< Float >( cuT( "ComputeSpotShadow" )
			, [this, &p_type]( Mat4 const & p_lightMatrix
				, Vec3 const & p_worldSpacePosition
				, Vec3 const & p_lightDirection
				, Vec3 const & p_normal
				, Int const & p_index )
			{
				auto l_lightSpacePosition = m_writer.GetLocale( cuT( "l_lightSpacePosition" ), GetLightSpacePosition( p_lightMatrix, p_worldSpacePosition, p_lightDirection, p_normal ) );
				auto l_index = m_writer.GetLocale( cuT( "l_index" ), m_writer.Cast< Float >( p_index ) / SpotShadowMapCount );
				auto l_visibility = m_writer.GetLocale( cuT( "l_visibility" ), 1.0_f );

				if ( p_type == ShadowType::eRaw )
				{
					l_visibility = FilterRaw( l_lightSpacePosition.xy(), l_lightSpacePosition.z(), l_index );
				}
				else if ( p_type == ShadowType::ePoisson )
				{
					l_visibility = FilterPoisson( l_lightSpacePosition.xy(), l_lightSpacePosition.z(), l_index );
				}
				else if ( p_type == ShadowType::eStratifiedPoisson )
				{
					l_visibility = FilterStratifiedPoisson( l_lightSpacePosition.xy(), l_lightSpacePosition.z(), l_index );
				}

				m_writer.Return( clamp( l_visibility, 0.0, 1.0 ) );
			}
			, InParam< Mat4 >( &m_writer, cuT( "p_lightMatrix" ) )
			, InParam< Vec3 >( &m_writer, cuT( "p_worldSpacePosition" ) )
			, InParam< Vec3 >( &m_writer, cuT( "p_lightDirection" ) )
			, InParam< Vec3 >( &m_writer, cuT( "p_normal" ) )
			, InParam< Int >( &m_writer, cuT( "p_index" ) ) );
	}

	void Shadow::DoDeclare_ComputePointShadow( ShadowType p_type )
	{
		m_writer.ImplementFunction< Float >( cuT( "ComputePointShadow" )
			, [this, &p_type]( Vec3 const & p_worldSpacePosition
			, Vec3 const & p_lightPosition
			, Vec3 const & p_normal
			, Int const & p_index )
			{
				auto l_vertexToLight = m_writer.GetLocale( cuT( "l_vertexToLight" ), normalize( p_worldSpacePosition - p_lightPosition ) );
				auto l_offset = m_writer.GetLocale( cuT( "l_offset" ), GetShadowOffset( p_normal, p_worldSpacePosition - p_lightPosition ) );
				auto l_worldSpace = m_writer.GetLocale( cuT( "l_worldSpace" ), p_worldSpacePosition + m_writer.Paren( p_normal * l_offset.x() ) );
				auto l_index = m_writer.GetLocale( cuT( "l_index" ), m_writer.Cast< Float >( p_index ) / SpotShadowMapCount );
				auto l_visibility = m_writer.GetLocale( cuT( "l_visibility" ), 1.0_f );
				l_vertexToLight = l_worldSpace - p_lightPosition;

				if ( p_type == ShadowType::eRaw )
				{
					l_visibility = FilterRaw( l_vertexToLight, length( l_vertexToLight ) / 4000.0_f, l_index );
				}
				else if ( p_type == ShadowType::ePoisson )
				{
					l_visibility = FilterPoisson( l_vertexToLight, length( l_vertexToLight ) / 4000.0_f, l_index );
				}
				else if ( p_type == ShadowType::eStratifiedPoisson )
				{
					l_visibility = FilterStratifiedPoisson( l_vertexToLight, length( l_vertexToLight ) / 4000.0_f, l_index );
				}

				m_writer.Return( clamp( l_visibility, 0.0, 1.0 ) );
			}
			, InParam< Vec3 >( &m_writer, cuT( "p_worldSpacePosition" ) )
			, InParam< Vec3 >( &m_writer, cuT( "p_lightPosition" ) )
			, InParam< Vec3 >( &m_writer, cuT( "p_normal" ) )
			, InParam< Int >( &m_writer, cuT( "p_index" ) ) );
	}
}
