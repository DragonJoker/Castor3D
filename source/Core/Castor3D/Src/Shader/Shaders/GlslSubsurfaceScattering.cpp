#include "GlslSubsurfaceScattering.hpp"

#include "GlslLight.hpp"
#include "GlslShadow.hpp"

using namespace castor;
using namespace glsl;

namespace castor3d
{
	namespace shader
	{
		SubsurfaceScattering::SubsurfaceScattering( GlslWriter & writer )
			: m_writer{ writer }
		{
		}

		void SubsurfaceScattering::declare()
		{
			doDeclareGetTransformedPosition();
			doDeclareComputeDirectionalLightDist();
			doDeclareComputePointLightDist();
			doDeclareComputeSpotLightDist();
		}

		void SubsurfaceScattering::declareDirectional()
		{
			doDeclareGetTransformedPosition();
			doDeclareComputeOneDirectionalLightDist();
		}

		void SubsurfaceScattering::declarePoint()
		{
			doDeclareGetTransformedPosition();
			doDeclareComputeOnePointLightDist();
		}

		void SubsurfaceScattering::declareSpot()
		{
			doDeclareGetTransformedPosition();
			doDeclareComputeOneSpotLightDist();
		}

		Float SubsurfaceScattering::computeLightDist( DirectionalLight const & light
			, Vec3 const & position )const
		{
			return m_computeDirectionalLightDist( light, position );
		}

		Float SubsurfaceScattering::computeLightDist( PointLight const & light
			, Vec3 const & position )const
		{
			return m_computePointLightDist( light, position );
		}

		Float SubsurfaceScattering::computeLightDist( SpotLight const & light
			, Vec3 const & position )const
		{
			return m_computeSpotLightDist( light, position );
		}

		Float SubsurfaceScattering::computeOneLightDist( DirectionalLight const & light
			, Vec3 const & position )const
		{
			return m_computeOneDirectionalLightDist( light, position );
		}

		Float SubsurfaceScattering::computeOneLightDist( PointLight const & light
			, Vec3 const & position )const
		{
			return m_computeOnePointLightDist( light, position );
		}

		Float SubsurfaceScattering::computeOneLightDist( SpotLight const & light
			, Vec3 const & position )const
		{
			return m_computeOneSpotLightDist( light, position );
		}
		
		void SubsurfaceScattering::doDeclareGetTransformedPosition()
		{
			m_getTransformedPosition = m_writer.implementFunction< Vec3 >( cuT( "getLightSpacePosition" )
				, [this]( Vec3 const & position
					, Mat4 const & transform )
				{
					auto transformed = m_writer.declLocale( cuT( "transformedPosition" )
						, transform * vec4( position, 1.0_f ) );
					// Perspective divide (result in range [-1,1]).
					transformed.xyz() = transformed.xyz() / transformed.w();
					// Now put the position in range [0,1].
					transformed.xyz() = m_writer.paren( transformed.xyz() * Float( 0.5 ) ) + Float( 0.5 );
					m_writer.returnStmt( transformed.xyz() );
				}
				, InVec3( &m_writer, cuT( "position" ) )
				, InMat4( &m_writer, cuT( "transform" ) ) );
		}

		void SubsurfaceScattering::doDeclareComputeDirectionalLightDist()
		{
			m_computeDirectionalLightDist = m_writer.implementFunction< Float >( cuT( "computeDirectionalLightDist" )
				, [this]( DirectionalLight const & light
					, Vec3 const & position )
				{
					auto c3d_mapShadowDirectional = m_writer.getBuiltin< Sampler2D >( Shadow::MapShadowDirectional );
					auto lightSpacePosition = m_writer.declLocale( cuT( "lightSpacePosition" )
						, m_getTransformedPosition( position, light.m_transform() ) );
					auto lightSpaceDepth = m_writer.declLocale( cuT( "lightSpaceDepth" )
						, texture( c3d_mapShadowDirectional, lightSpacePosition.xy() ).r() );
					m_writer.returnStmt( 1.0_f / glsl::abs( lightSpaceDepth - position.z() ) );
					//m_writer.returnStmt( glsl::exp( 1.0_f / glsl::abs( lightSpaceDepth - position.z() ) ) );
				}
				, InParam< DirectionalLight >{ &m_writer, cuT( "light" ) }
				, InVec3{ &m_writer, cuT( "position" ) } );
		}

		void SubsurfaceScattering::doDeclareComputePointLightDist()
		{
			m_computePointLightDist = m_writer.implementFunction< Float >( cuT( "computePointLightDist" )
				, [this]( PointLight const & light
					, Vec3 const & position )
				{
					auto c3d_mapShadowPoint = m_writer.getBuiltin< SamplerCube >( Shadow::MapShadowPoint, PointShadowMapCount );
					auto vertexToLight = m_writer.declLocale( cuT( "vertexToLight" )
						, position - light.m_position() );
					auto direction = m_writer.declLocale( cuT( "direction" )
						, vertexToLight );
					auto lightSpaceDepth = m_writer.declLocale( cuT( "lightSpaceDepth" )
						, texture( c3d_mapShadowPoint[light.m_index()], direction ).r() );
					m_writer.returnStmt( glsl::exp( 1.0_f / glsl::abs( lightSpaceDepth - position.z() ) ) );
				}
				, InParam< PointLight >{ &m_writer, cuT( "light" ) }
				, InVec3{ &m_writer, cuT( "position" ) } );
		}

		void SubsurfaceScattering::doDeclareComputeSpotLightDist()
		{
			m_computeSpotLightDist = m_writer.implementFunction< Float >( cuT( "computeSpotLightDist" )
				, [this]( SpotLight const & light
					, Vec3 const & position )
				{
					auto c3d_mapShadowSpot = m_writer.getBuiltin< Sampler2D >( Shadow::MapShadowSpot, SpotShadowMapCount );
					auto lightSpacePosition = m_writer.declLocale( cuT( "lightSpacePosition" )
						, m_getTransformedPosition( position, light.m_transform() ) );
					auto lightSpaceDepth = m_writer.declLocale( cuT( "lightSpaceDepth" )
						, texture( c3d_mapShadowSpot[light.m_index()], lightSpacePosition.xy() ).r() );
					m_writer.returnStmt( glsl::exp( 1.0_f / glsl::abs( lightSpaceDepth - position.z() ) ) );
				}
				, InParam< SpotLight >{ &m_writer, cuT( "light" ) }
				, InVec3{ &m_writer, cuT( "position" ) } );
		}

		void SubsurfaceScattering::doDeclareComputeOneDirectionalLightDist()
		{
			m_computeOneDirectionalLightDist = m_writer.implementFunction< Float >( cuT( "computeOneDirectionalLightDist" )
				, [this]( DirectionalLight const & light
					, Vec3 const & position )
				{
					auto c3d_mapShadowDirectional = m_writer.getBuiltin< Sampler2D >( Shadow::MapShadowDirectional );
					auto lightSpacePosition = m_writer.declLocale( cuT( "lightSpacePosition" )
						, m_getTransformedPosition( position, light.m_transform() ) );
					auto lightSpaceDepth = m_writer.declLocale( cuT( "lightSpaceDepth" )
						, texture( c3d_mapShadowDirectional, lightSpacePosition.xy() ).r() );
					m_writer.returnStmt( 1.0_f / glsl::pow( glsl::abs( lightSpaceDepth - position.z() ), 2.0_f ) );
					//m_writer.returnStmt( glsl::exp( 1.0_f / glsl::abs( lightSpaceDepth - position.z() ) ) );
				}
				, InParam< DirectionalLight >{ &m_writer, cuT( "light" ) }
				, InVec3{ &m_writer, cuT( "position" ) } );
		}

		void SubsurfaceScattering::doDeclareComputeOnePointLightDist()
		{
			m_computeOnePointLightDist = m_writer.implementFunction< Float >( cuT( "computeOnePointLightDist" )
				, [this]( PointLight const & light
					, Vec3 const & position )
				{
					auto c3d_mapShadowPoint = m_writer.getBuiltin< SamplerCube >( Shadow::MapShadowPoint );
					auto vertexToLight = m_writer.declLocale( cuT( "vertexToLight" )
						, position - light.m_position() );
					auto direction = m_writer.declLocale( cuT( "direction" )
						, vertexToLight );
					auto lightSpaceDepth = m_writer.declLocale( cuT( "lightSpaceDepth" )
						, texture( c3d_mapShadowPoint, direction ).r() );
					m_writer.returnStmt( glsl::exp( 1.0_f / glsl::abs( lightSpaceDepth - position.z() ) ) );
				}
				, InParam< PointLight >{ &m_writer, cuT( "light" ) }
				, InVec3{ &m_writer, cuT( "position" ) } );
		}

		void SubsurfaceScattering::doDeclareComputeOneSpotLightDist()
		{
			m_computeOneSpotLightDist = m_writer.implementFunction< Float >( cuT( "computeOneSpotLightDist" )
				, [this]( SpotLight const & light
					, Vec3 const & position )
				{
					auto c3d_mapShadowSpot = m_writer.getBuiltin< Sampler2D >( Shadow::MapShadowSpot );
					auto lightSpacePosition = m_writer.declLocale( cuT( "lightSpacePosition" )
						, m_getTransformedPosition( position, light.m_transform() ) );
					auto lightSpaceDepth = m_writer.declLocale( cuT( "lightSpaceDepth" )
						, texture( c3d_mapShadowSpot, lightSpacePosition.xy() ).r() );
					m_writer.returnStmt( glsl::exp( 1.0_f / glsl::abs( lightSpaceDepth - position.z() ) ) );
				}
				, InParam< SpotLight >{ &m_writer, cuT( "light" ) }
				, InVec3{ &m_writer, cuT( "position" ) } );
		}
	}
}
