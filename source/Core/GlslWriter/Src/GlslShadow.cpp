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
		auto c3d_mapShadow2D = m_writer.GetUniform< Sampler2DArrayShadow >( MapShadowSpot );
		auto c3d_mapShadowCube = m_writer.GetUniform< SamplerCube >( MapShadowPoint );

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
		DoDeclare_ComputeDirectionalShadow( p_type );
		DoDeclare_ComputeSpotShadow( p_type );
		DoDeclare_ComputePointShadow( p_type );
	}

	Float Shadow::ComputeDirectionalShadow( Vec4 const & p_lightSpacePosition, Vec3 const & p_lightDir, Vec3 const & p_normal )
	{
		return WriteFunctionCall< Float >( &m_writer, cuT( "ComputeDirectionalShadow" ), p_lightSpacePosition, p_lightDir, p_normal );
	}

	Float Shadow::ComputeSpotShadow( Vec4 const & p_lightSpacePosition, Vec3 const & p_lightDir, Vec3 const & p_normal, Int const & p_index )
	{
		return WriteFunctionCall< Float >( &m_writer, cuT( "ComputeSpotShadow" ), p_lightSpacePosition, p_lightDir, p_normal, p_index );
	}

	Float Shadow::ComputePointShadow( Vec3 const & p_lightDirection, Vec3 const & p_normal, Int const & p_index )
	{
		return WriteFunctionCall< Float >( &m_writer, cuT( "ComputePointShadow" ), p_lightDirection, p_normal, p_index );
	}

	void Shadow::DoDeclare_ComputeDirectionalShadow( ShadowType p_type )
	{
		auto l_compute = [this, &p_type]( Vec4 const & p_lightSpacePosition, Vec3 const & p_lightDirection, Vec3 const & p_normal )
		{
			auto c3d_mapShadow2D = m_writer.GetBuiltin< Sampler2DShadow >( Shadow::MapShadowDirectional );
			auto c3d_poissonDisk = m_writer.GetBuiltin< Vec2 >( cuT( "c3d_poissonDisk" ), 4u );
			// Perspective divide (result in range [-1,1]).
			auto l_lightSpacePosition = m_writer.GetLocale( cuT( "l_lightSpacePosition" ), p_lightSpacePosition.xyz() / p_lightSpacePosition.w() );
			// Now put the position in range [0,1].
			l_lightSpacePosition = m_writer.Paren( l_lightSpacePosition * Float( 0.5 ) ) + Float( 0.5 );
			// Take care of shadow acne by subtracting a bias based on the surface's slope.
			auto l_z = m_writer.GetLocale( cuT( "l_z" ), l_lightSpacePosition.z() );
			auto l_cosTheta = m_writer.GetLocale( cuT( "l_cosTheta" ), clamp( dot( p_normal, p_lightDirection ), 0, 1 ) );
			auto l_bias = m_writer.GetLocale( cuT( "l_bias" ), Float( 0.00008 ) * tan( acos( l_cosTheta ) ) );
			l_bias = clamp( l_bias, 0.0, 0.0002 );
			l_z -= l_bias;

			auto l_visibility = m_writer.GetLocale( cuT( "l_visibility" ), Float( 1 ) );

			if ( p_type == ShadowType::eRaw )
			{
				l_visibility = texture( c3d_mapShadow2D, vec3( l_lightSpacePosition.xy(), l_z ) );
			}
			else if ( p_type == ShadowType::ePoisson )
			{
				auto c3d_poissonDisk = m_writer.GetBuiltin< Vec2 >( cuT( "c3d_poissonDisk" ), 4u );
				auto l_diffusion = m_writer.GetLocale( cuT( "l_diffusion" ), Float( 500.0 ) );

				for ( int i = 0; i < 4; i++ )
				{
					l_visibility -= Float( 0.2 ) * m_writer.Paren( Float( 1 ) - texture( c3d_mapShadow2D, vec3( l_lightSpacePosition.xy() + c3d_poissonDisk[i] / l_diffusion, l_z ) ) );
				}
			}
			else if ( p_type == ShadowType::eStratifiedPoisson )
			{
				auto c3d_poissonDisk = m_writer.GetBuiltin< Vec2 >( cuT( "c3d_poissonDisk" ), 4u );
				auto gl_FragCoord = m_writer.GetBuiltin< Vec3 >( cuT( "gl_FragCoord" ), 4u );
				auto l_index = m_writer.GetLocale( cuT( "l_index" ), Int( 0 ) );
				auto l_diffusion = m_writer.GetLocale( cuT( "l_diffusion" ), Float( 700.0 ) );

				for ( int i = 0; i < 4; i++ )
				{
					l_index = m_writer.Cast< Int >( 16.0 * WriteFunctionCall< Float >( &m_writer, cuT( "GetRandom" ), vec4( gl_FragCoord.xy(), gl_FragCoord.y(), i ) ) ) % 16;
					l_visibility -= Float( 0.2 ) * m_writer.Paren( Float( 1 ) - texture( c3d_mapShadow2D, vec3( l_lightSpacePosition.xy() + c3d_poissonDisk[l_index] / l_diffusion, l_z ) ) );
				}
			}

			m_writer.Return( clamp( l_visibility, 0.0, 1.0 ) );
		};
		m_writer.ImplementFunction< Float >( cuT( "ComputeDirectionalShadow" )
											 , l_compute
											 , InParam< Vec4 >( &m_writer, cuT( "p_lightSpacePosition" ) )
											 , InParam< Vec3 >( &m_writer, cuT( "p_lightDirection" ) )
											 , InParam< Vec3 >( &m_writer, cuT( "p_normal" ) ) );
	}

	void Shadow::DoDeclare_ComputeSpotShadow( ShadowType p_type )
	{
		auto l_compute = [this, &p_type]( Vec4 const & p_lightSpacePosition, Vec3 const & p_lightDirection, Vec3 const & p_normal, Int const & p_index )
		{
			auto c3d_mapShadow2D = m_writer.GetBuiltin< Sampler2DArrayShadow >( Shadow::MapShadowSpot );
			auto c3d_poissonDisk = m_writer.GetBuiltin< Vec2 >( cuT( "c3d_poissonDisk" ), 4u );
			auto l_index = m_writer.GetLocale( cuT( "l_index" ), m_writer.Cast< Float >( p_index ) / SpotShadowMapCount );
			// Perspective divide (result in range [-1,1]).
			auto l_lightSpacePosition = m_writer.GetLocale( cuT( "l_lightSpacePosition" ), p_lightSpacePosition.xyz() / p_lightSpacePosition.w() );
			// Now put the position in range [0,1].
			l_lightSpacePosition = m_writer.Paren( l_lightSpacePosition * 0.5_f ) + 0.5_f;
			// Take care of shadow acne by subtracting a bias based on the surface's slope.
			auto l_z = m_writer.GetLocale( cuT( "l_z" ), l_lightSpacePosition.z() );
			auto l_cosTheta = m_writer.GetLocale( cuT( "l_cosTheta" ), clamp( dot( p_normal, p_lightDirection ), 0, 1 ) );
			auto l_bias = m_writer.GetLocale( cuT( "l_bias" ), 0.001_f * tan( acos( l_cosTheta ) ) );
			l_bias = clamp( l_bias, 0.0001, 0.001 );
			l_z -= l_bias;

			auto l_visibility = m_writer.GetLocale( cuT( "l_visibility" ), 1.0_f );

			if ( p_type == ShadowType::eRaw )
			{
				l_visibility = texture( c3d_mapShadow2D, vec4( l_lightSpacePosition.xy(), l_index, l_z ) );
			}
			else if ( p_type == ShadowType::ePoisson )
			{
				auto c3d_poissonDisk = m_writer.GetBuiltin< Vec2 >( cuT( "c3d_poissonDisk" ), 4u );
				auto l_diffusion = m_writer.GetLocale( cuT( "l_diffusion" ), 500.0_f );

				for ( int i = 0; i < 4; i++ )
				{
					l_visibility -= 0.2_f * m_writer.Paren( 1.0_f - texture( c3d_mapShadow2D, vec4( l_lightSpacePosition.xy() + c3d_poissonDisk[i] / l_diffusion, l_index, l_z ) ) );
				}
			}
			else if ( p_type == ShadowType::eStratifiedPoisson )
			{
				auto c3d_poissonDisk = m_writer.GetBuiltin< Vec2 >( cuT( "c3d_poissonDisk" ), 4u );
				auto gl_FragCoord = m_writer.GetBuiltin< Vec3 >( cuT( "gl_FragCoord" ), 4u );
				auto l_index = m_writer.GetLocale( cuT( "l_index" ), 0_i );
				auto l_diffusion = m_writer.GetLocale( cuT( "l_diffusion" ), 700.0_f );

				for ( int i = 0; i < 4; i++ )
				{
					l_index = m_writer.Cast< Int >( 16.0 * WriteFunctionCall< Float >( &m_writer, cuT( "GetRandom" ), vec4( gl_FragCoord.xy(), gl_FragCoord.y(), i ) ) ) % 16;
					l_visibility -= 0.2_f * m_writer.Paren( 1.0_f - texture( c3d_mapShadow2D, vec4( l_lightSpacePosition.xy() + c3d_poissonDisk[l_index] / l_diffusion, l_index, l_z ) ) );
				}
			}

			m_writer.Return( clamp( l_visibility, 0.0, 1.0 ) );
		};
		m_writer.ImplementFunction< Float >( cuT( "ComputeSpotShadow" )
											 , l_compute
											 , InParam< Vec4 >( &m_writer, cuT( "p_lightSpacePosition" ) )
											 , InParam< Vec3 >( &m_writer, cuT( "p_lightDirection" ) )
											 , InParam< Vec3 >( &m_writer, cuT( "p_normal" ) )
											 , InParam< Int >( &m_writer, cuT( "p_index" ) ) );
	}

	void Shadow::DoDeclare_ComputePointShadow( ShadowType p_type )
	{
		auto l_compute = [this, &p_type]( Vec3 const & p_vertexPosition, Vec3 const & p_lightPosition, Int const & p_index )
		{
			auto c3d_mapShadowCube = m_writer.GetBuiltin< SamplerCube >( Shadow::MapShadowPoint );
			auto c3d_poissonDisk = m_writer.GetBuiltin< Vec2 >( cuT( "c3d_poissonDisk" ), 4u );
			auto l_index = m_writer.GetLocale( cuT( "l_index" ), m_writer.Cast< Float >( p_index ) / PointShadowMapCount );
			auto l_visibility = m_writer.GetLocale( cuT( "l_visibility" ), 1.0_f );

			//if ( p_type == ShadowType::eRaw )
			//{
				auto l_vertexToLight = m_writer.GetLocale( cuT( "l_vertexToLight" ), p_vertexPosition - p_lightPosition );
				auto l_closest = m_writer.GetLocale( cuT( "l_closest" ), texture( c3d_mapShadowCube, l_vertexToLight ).r() );
				l_closest *= 2000.0_f;
				auto l_current = m_writer.GetLocale( cuT( "l_current" ), length( l_vertexToLight ) );
				auto l_bias = m_writer.GetLocale( cuT( "l_bias" ), 0.05_f );
				l_current -= l_bias;

				IF( m_writer, l_closest < l_current )
				{
					l_visibility = 0;
				}
				FI;
			//}
			//else if ( p_type == ShadowType::ePoisson )
			//{
			//	auto c3d_poissonDisk = m_writer.GetBuiltin< Vec2 >( cuT( "c3d_poissonDisk" ), 4u );

			//	for ( int i = 0; i < 4; i++ )
			//	{
			//		l_visibility -= 0.2_f * m_writer.Paren( 1.0_f - texture( p_map, vec4( p_lightDirection.xy() + c3d_poissonDisk[i] / 700.0, p_lightDirection.z(), length( p_lightDirection ) ) ) );
			//	}
			//}
			//else if ( p_type == ShadowType::eStratifiedPoisson )
			//{
			//	auto c3d_poissonDisk = m_writer.GetBuiltin< Vec2 >( cuT( "c3d_poissonDisk" ), 4u );
			//	auto gl_FragCoord = m_writer.GetBuiltin< Vec3 >( cuT( "gl_FragCoord" ), 4u );
			//	auto l_index = m_writer.GetLocale( cuT( "l_index" ) );

			//	for ( int i = 0; i < 4; i++ )
			//	{
			//		l_index = m_writer.Cast< Int >( 16.0 * WriteFunctionCall< Float >( &m_writer, cuT( "GetRandom" ), vec4( gl_FragCoord.xy(), gl_FragCoord.y(), i ) ) ) % 16;
			//		l_visibility -= 0.2_f * m_writer.Paren( 1.0_f - texture( p_map, vec4( p_lightDirection.xy() + c3d_poissonDisk[l_index] / 700.0, p_lightDirection.z(), length( p_lightDirection ) ) ) );
			//	}
			//}

			m_writer.Return( clamp( l_visibility + 0.5_f, 0.0, 1.0 ) );
		};
		m_writer.ImplementFunction< Float >( cuT( "ComputePointShadow" )
											 , l_compute
											 , InParam< Vec3 >( &m_writer, cuT( "p_vertexPosition" ) )
											 , InParam< Vec3 >( &m_writer, cuT( "p_lightPosition" ) )
											 , InParam< Int >( &m_writer, cuT( "p_index" ) ) );
	}

	void Shadow::DoDeclare_GetRandom()
	{
		auto l_random = [this]( Vec4 const & p_seed )
		{
			auto l_dot = m_writer.GetLocale( cuT( "l_dot" ), dot( p_seed, vec4( 12.9898_f, 78.233, 45.164, 94.673 ) ) );
			m_writer.Return( fract( sin( l_dot ) * 43758.5453 ) );
		};
		m_writer.ImplementFunction< Float >( cuT( "GetRandom" )
											 , l_random
											 , InParam< Vec4 >( &m_writer, cuT( "p_seed" ) ) );
	}
}
