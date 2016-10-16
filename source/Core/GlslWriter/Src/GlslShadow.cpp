#include "GlslShadow.hpp"

using namespace Castor;

namespace GLSL
{
	Castor::String const Shadow::MapShadow2D = cuT( "c3d_mapShadow2D" );
	Castor::String const Shadow::MapShadowCube = cuT( "c3d_mapShadowCube" );

	Shadow::Shadow( GlslWriter & p_writer )
		: m_writer{ p_writer }
	{
	}

	void Shadow::Declare( ShadowType p_type )
	{
		auto c3d_mapShadow2D = m_writer.GetUniform< Sampler2DShadow >( MapShadow2D, 10u );
		auto c3d_mapShadowCube = m_writer.GetUniform< SamplerCube >( MapShadowCube, 10u );

		if ( p_type == ShadowType::Poisson || p_type == ShadowType::StratifiedPoisson )
		{
			auto c3d_poissonDisk = m_writer.GetUniform< Vec2 >( cuT( "c3d_poissonDisk" ), 4,
			{
				vec2( Float( -0.94201624 ), -0.39906216 ),
				vec2( Float( 0.94558609 ), -0.76890725 ),
				vec2( Float( -0.094184101 ), -0.92938870 ),
				vec2( Float( 0.34495938 ), 0.29387760 )
			} );
		}

		DoDeclare_GetRandom();
		DoDeclare_Compute2DShadow( p_type );
		DoDeclare_ComputeCubeShadow( p_type );
	}

	Float Shadow::ComputeShadow( Vec4 const & p_lightSpacePosition, Vec3 const & p_lightDir, Vec3 const & p_normal, Sampler2DShadow const & p_map )
	{
		return WriteFunctionCall< Float >( &m_writer, cuT( "Compute2DShadow" ), p_lightSpacePosition, p_lightDir, p_normal, p_map );
	}

	Float Shadow::ComputeShadow( Vec3 const & p_lightDirection, Vec3 const & p_normal, SamplerCube const & p_map )
	{
		return WriteFunctionCall< Float >( &m_writer, cuT( "ComputeCubeShadow" ), p_lightDirection, p_normal, p_map );
	}

	void Shadow::DoDeclare_Compute2DShadow( ShadowType p_type )
	{
		auto l_compute = [this, &p_type]( Vec4 const & p_lightSpacePosition, Vec3 const & p_lightDirection, Vec3 const & p_normal, Sampler2DShadow const & p_map )
		{
			auto c3d_poissonDisk = m_writer.GetBuiltin< Vec2 >( cuT( "c3d_poissonDisk" ), 4u );
			// Perspective divide (result in range [-1,1]).
			auto l_lightSpacePosition = m_writer.GetLocale< Vec3 >( cuT( "l_lightSpacePosition" ), p_lightSpacePosition.xyz() / p_lightSpacePosition.w() );
			// Now put the position in range [0,1].
			l_lightSpacePosition = m_writer.Paren( l_lightSpacePosition * Float( 0.5 ) ) + Float( 0.5 );
			auto l_visibility = m_writer.GetLocale< Float >( cuT( "l_visibility" ), Float( 1 ) );

			if ( p_type == ShadowType::Raw )
			{
				l_visibility = texture( p_map, l_lightSpacePosition );
			}
			else if ( p_type == ShadowType::Poisson )
			{
				auto c3d_poissonDisk = m_writer.GetBuiltin< Vec2 >( cuT( "c3d_poissonDisk" ), 4u );

				for ( int i = 0; i < 4; i++ )
				{
					l_visibility -= Float( 0.2 ) * m_writer.Paren( Float( 1 ) - texture( p_map, vec3( l_lightSpacePosition.xy() + c3d_poissonDisk[i] / 700.0, l_lightSpacePosition.z() ) ) );
				}
			}
			else if ( p_type == ShadowType::StratifiedPoisson )
			{
				auto c3d_poissonDisk = m_writer.GetBuiltin< Vec2 >( cuT( "c3d_poissonDisk" ), 4u );
				auto gl_FragCoord = m_writer.GetBuiltin< Vec3 >( cuT( "gl_FragCoord" ), 4u );
				auto l_index = m_writer.GetLocale< Int >( cuT( "l_index" ) );

				for ( int i = 0; i < 4; i++ )
				{
					l_index = m_writer.Cast< Int >( 16.0 * WriteFunctionCall< Float >( &m_writer, cuT( "GetRandom" ), vec4( gl_FragCoord.xy(), gl_FragCoord.y(), i ) ) ) % 16;
					l_visibility -= Float( 0.2 ) * m_writer.Paren( Float( 1 ) - texture( p_map, vec3( l_lightSpacePosition.xy() + c3d_poissonDisk[l_index] / 700.0, l_lightSpacePosition.z() ) ) );
				}
			}

			m_writer.Return( clamp( l_visibility + Float( 0.5 ), 0.0, 1.0 ) );
		};
		m_writer.ImplementFunction< Float >( cuT( "Compute2DShadow" )
											 , l_compute
											 , InParam< Vec4 >( &m_writer, cuT( "p_lightSpacePosition" ) )
											 , InParam< Vec3 >( &m_writer, cuT( "p_lightDirection" ) )
											 , InParam< Vec3 >( &m_writer, cuT( "p_normal" ) )
											 , InParam< Sampler2DShadow >( &m_writer, cuT( "p_map" ) ) );
	}

	void Shadow::DoDeclare_ComputeCubeShadow( ShadowType p_type )
	{
		auto l_compute = [this, &p_type]( Vec3 const & p_lightDirection, Vec3 const & p_normal, SamplerCube const & p_map )
		{
			auto c3d_poissonDisk = m_writer.GetBuiltin< Vec2 >( cuT( "c3d_poissonDisk" ), 4u );
			auto l_visibility = m_writer.GetLocale< Float >( cuT( "l_visibility" ), Float( 1 ) );

			//if ( p_type == ShadowType::Raw )
			{
				auto l_direction = m_writer.GetLocale< Vec3 >( cuT( "l_direction" ), texture( p_map, p_lightDirection.xyz() ).rgb() );

				IF( m_writer, length( l_direction ) < length( p_lightDirection ) )
				{
					l_visibility = 0;
				}
				FI;
			}
			//else if ( p_type == ShadowType::Poisson )
			//{
			//	auto c3d_poissonDisk = m_writer.GetBuiltin< Vec2 >( cuT( "c3d_poissonDisk" ), 4u );

			//	for ( int i = 0; i < 4; i++ )
			//	{
			//		l_visibility -= Float( 0.2 ) * m_writer.Paren( Float( 1 ) - texture( p_map, vec4( p_lightDirection.xy() + c3d_poissonDisk[i] / 700.0, p_lightDirection.z(), length( p_lightDirection ) ) ) );
			//	}
			//}
			//else if ( p_type == ShadowType::StratifiedPoisson )
			//{
			//	auto c3d_poissonDisk = m_writer.GetBuiltin< Vec2 >( cuT( "c3d_poissonDisk" ), 4u );
			//	auto gl_FragCoord = m_writer.GetBuiltin< Vec3 >( cuT( "gl_FragCoord" ), 4u );
			//	auto l_index = m_writer.GetLocale< Int >( cuT( "l_index" ) );

			//	for ( int i = 0; i < 4; i++ )
			//	{
			//		l_index = m_writer.Cast< Int >( 16.0 * WriteFunctionCall< Float >( &m_writer, cuT( "GetRandom" ), vec4( gl_FragCoord.xy(), gl_FragCoord.y(), i ) ) ) % 16;
			//		l_visibility -= Float( 0.2 ) * m_writer.Paren( Float( 1 ) - texture( p_map, vec4( p_lightDirection.xy() + c3d_poissonDisk[l_index] / 700.0, p_lightDirection.z(), length( p_lightDirection ) ) ) );
			//	}
			//}

			m_writer.Return( clamp( l_visibility + Float( 0.5 ), 0.0, 1.0 ) );
		};
		m_writer.ImplementFunction< Float >( cuT( "ComputeCubeShadow" )
											 , l_compute
											 , InParam< Vec3 >( &m_writer, cuT( "p_lightDirection" ) )
											 , InParam< Vec3 >( &m_writer, cuT( "p_normal" ) )
											 , InParam< SamplerCube >( &m_writer, cuT( "p_map" ) ) );
	}

	void Shadow::DoDeclare_GetRandom()
	{
		auto l_random = [this]( Vec4 const & p_seed )
		{
			auto l_dot = m_writer.GetLocale< Float >( cuT( "l_dot" ), dot( p_seed, vec4( Float( 12.9898 ), 78.233, 45.164, 94.673 ) ) );
			m_writer.Return( fract( sin( l_dot ) * 43758.5453 ) );
		};
		m_writer.ImplementFunction< Float >( cuT( "GetRandom" )
											 , l_random
											 , InParam< Vec4 >( &m_writer, cuT( "p_seed" ) ) );
	}
}
