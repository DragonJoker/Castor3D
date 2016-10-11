#include "GlslShadow.hpp"

using namespace Castor;

namespace GLSL
{
	Shadow::Shadow( GlslWriter & p_writer )
		: m_writer{ p_writer }
	{
	}

	void Shadow::Declare( uint8_t p_flags )
	{
		auto l_compute = [this, p_flags]( Vec4 const & p_lightSpacePosition, Sampler2D const & p_map )
		{
			// Perspective divide (result in range [-1,1]).
			auto l_lightSpacePosition = m_writer.GetLocale< Vec3 >( cuT( "l_lightSpacePosition" ), p_lightSpacePosition.xyz() / p_lightSpacePosition.w() );
			// Now put the position in range [0,1].
			l_lightSpacePosition = m_writer.Paren( l_lightSpacePosition * Float( 0.5 ) ) + Float( 0.5 );
			// Sample shadow map to get closest depth from light's point of view.
			auto l_closestDepth = m_writer.GetLocale< Float >( cuT( "l_closestDepth" ), texture( p_map, l_lightSpacePosition.xy() ).r() );
			// And get the current depth of the fragment, from light's point of view.
			auto l_currentDepth = m_writer.GetLocale< Float >( cuT( "l_currentDepth" ), l_lightSpacePosition.z() );
			// Check whether the fragment is in shadow or not.
			m_writer.Return( m_writer.Ternary< Float >( l_currentDepth > l_closestDepth, Float( 1.0f ), Float( 0.0f ) ) );
		};
		m_writer.ImplementFunction< Float >( cuT( "ComputeShadow" )
											 , l_compute
											 , InParam< Vec4 >( &m_writer, cuT( "p_lightSpacePosition" ) )
											 , InParam< Sampler2D >( &m_writer, cuT( "p_map" ) ) );
	}

	Float Shadow::ComputeShadow( Vec4 const & p_lightSpacePosition, Sampler2D const & p_map )
	{
		return WriteFunctionCall< Float >( &m_writer, cuT( "ComputeShadow" ), p_lightSpacePosition, p_map );
	}
}
