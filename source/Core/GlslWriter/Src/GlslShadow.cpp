#include "GlslShadow.hpp"

using namespace Castor;

namespace GLSL
{
	Shadow::Shadow( GlslWriter & p_writer )
		: m_writer{ p_writer }
	{
	}

	void Shadow::Declare()
	{
		auto c3d_mapShadow = m_writer.GetUniform< Sampler2DShadow >( cuT( "c3d_mapShadow" ), 10u );

		auto l_compute = [this]( Vec4 const & p_lightSpacePosition, Vec3 const & p_lightDir, Vec3 const & p_normal, Sampler2DShadow const & p_map )
		{
			// Perspective divide (result in range [-1,1]).
			auto l_lightSpacePosition = m_writer.GetLocale< Vec3 >( cuT( "l_lightSpacePosition" ), p_lightSpacePosition.xyz() / p_lightSpacePosition.w() );
			// Now put the position in range [0,1].
			l_lightSpacePosition = m_writer.Paren( l_lightSpacePosition * Float( 0.5 ) ) + Float( 0.5 );
			// Sample shadow map to get closest depth from light's point of view.
			auto l_visibility = m_writer.GetLocale< Float >( cuT( "l_closestDepth" ), texture( p_map, l_lightSpacePosition ) );

			//// Take care of shadow acne
			////auto l_bias = m_writer.GetLocale< Float >( cuT( "l_bias" ), max( Float( 0.05 ) * m_writer.Paren( Float( 1.0 ) - dot( p_normal, p_lightDir ) ), Float( 0.005 ) ) );
			//auto l_bias = m_writer.GetLocale< Float >( cuT( "l_bias" ), Float( 0.005 ) );
			////auto l_bias = m_writer.GetLocale< Float >( cuT( "l_bias" ), Float( 0.005 ) * tan( acos( clamp( dot( p_normal, p_lightDir ), 0, 1 ) ) ) );
			////l_bias = clamp( l_bias, 0.0, 0.01 );
			//// Check whether the fragment is in shadow or not.
			//auto l_visibility = m_writer.GetLocale< Float >( cuT( "l_visibility" ), Float( 1.0f ) );

			////IF( m_writer, m_writer.Paren( l_lightSpacePosition.z() - l_bias ) < l_closestDepth )
			//IF( m_writer, l_lightSpacePosition.z() < l_closestDepth )
			//{
			//	l_visibility = Float( 0.5f );
			//}
			//FI;

			m_writer.Return( clamp( l_visibility + Float( 0.5 ), 0.0, 1.0 ) );

			//auto l_visibility = m_writer.GetLocale< Float >( cuT( "l_visibility" ), m_writer.Ternary( l_lightSpacePosition.z() > l_closestDepth, Float( 1.0f ), Float( 0.0f ) ) );
			//m_writer.Return( l_visibility );
		};
		m_writer.ImplementFunction< Float >( cuT( "ComputeShadow" )
											 , l_compute
											 , InParam< Vec4 >( &m_writer, cuT( "p_lightSpacePosition" ) )
											 , InParam< Vec3 >( &m_writer, cuT( "p_lightDir" ) )
											 , InParam< Vec3 >( &m_writer, cuT( "p_normal" ) )
											 , InParam< Sampler2DShadow >( &m_writer, cuT( "p_map" ) ) );
	}

	Float Shadow::ComputeShadow( Vec4 const & p_lightSpacePosition, Vec3 const & p_lightDir, Vec3 const & p_normal, Sampler2DShadow const & p_map )
	{
		return WriteFunctionCall< Float >( &m_writer, cuT( "ComputeShadow" ), p_lightSpacePosition, p_lightDir, p_normal, p_map );
	}
}
