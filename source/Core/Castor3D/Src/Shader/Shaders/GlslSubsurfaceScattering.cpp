#include "GlslSubsurfaceScattering.hpp"

#include "GlslLight.hpp"
#include "GlslPhongLighting.hpp"
#include "GlslMetallicBrdfLighting.hpp"
#include "GlslSpecularBrdfLighting.hpp"
#include "GlslShadow.hpp"
#include "GlslMaterial.hpp"

using namespace castor;
using namespace glsl;

namespace castor3d
{
	namespace shader
	{
		SubsurfaceScattering::SubsurfaceScattering( GlslWriter & writer
			, bool shadowMap )
			: m_writer{ writer }
			, m_shadowMap{ shadowMap }
		{
		}

		void SubsurfaceScattering::declare()
		{
			doDeclareGetTransformedPosition();
			doDeclareComputeDirectionalLightDist();
			doDeclareComputePointLightDist();
			doDeclareComputeSpotLightDist();
		}

		void SubsurfaceScattering::declare( LightType type )
		{
			if ( m_shadowMap )
			{
				doDeclareGetTransformedPosition();
			}

			switch ( type )
			{
			case LightType::eDirectional:
				doDeclareComputeDirectionalLightDist();
				break;

			case LightType::ePoint:
				doDeclareComputeOnePointLightDist();
				break;

			case LightType::eSpot:
				doDeclareComputeOneSpotLightDist();
				break;
			}
		}
		
		Vec3 SubsurfaceScattering::compute( PhongLightingModel const & lighting
			, LegacyMaterial const & material
			, DirectionalLight const & light
			, Vec2 const & uv
			, Vec3 const & position
			, Vec3 const & normal
			, Float const & translucency
			, Vec3 const & eye )const
		{
			auto result = m_writer.declLocale( cuT( "result" )
				, vec3( 0.0_f ) );

			IF( m_writer, material.m_subsurfaceScatteringEnabled() != 0_i )
			{
				result = doComputeLightDist( light
						, uv
						, material.m_distanceBasedTransmission()
						, material.m_backLitCoefficient()
						, position )
					* translucency
					* lighting.computeDirectionalLightBackLit( light
						, eye
						, shader::FragmentInput( position, -normal ) );
			}
			FI;

			return result;
		}

		Vec3 SubsurfaceScattering::compute( PhongLightingModel const & lighting
			, LegacyMaterial const & material
			, PointLight const & light
			, Vec2 const & uv
			, Vec3 const & position
			, Vec3 const & normal
			, Float const & translucency
			, Vec3 const & eye )const
		{
			auto result = m_writer.declLocale( cuT( "result" )
				, vec3( 0.0_f ) );

			IF( m_writer, material.m_subsurfaceScatteringEnabled() != 0_i )
			{
				result = doComputeLightDist( light
						, uv
						, material.m_distanceBasedTransmission()
						, material.m_backLitCoefficient()
						, position )
					* translucency
					* lighting.computePointLightBackLit( light
						, eye
						, shader::FragmentInput( position, -normal ) );
			}
			FI;

			return result;
		}

		Vec3 SubsurfaceScattering::compute( PhongLightingModel const & lighting
			, LegacyMaterial const & material
			, SpotLight const & light
			, Vec2 const & uv
			, Vec3 const & position
			, Vec3 const & normal
			, Float const & translucency
			, Vec3 const & eye )const
		{
			auto result = m_writer.declLocale( cuT( "result" )
				, vec3( 0.0_f ) );

			IF( m_writer, material.m_subsurfaceScatteringEnabled() != 0_i )
			{
				result = doComputeLightDist( light
						, uv
						, material.m_distanceBasedTransmission()
						, material.m_backLitCoefficient()
						, position )
					* translucency
					* lighting.computeSpotLightBackLit( light
						, eye
						, shader::FragmentInput( position, -normal ) );
			}
			FI;

			return result;
		}

		Vec3 SubsurfaceScattering::compute( MetallicBrdfLightingModel const & lighting
			, MetallicRoughnessMaterial const & material
			, DirectionalLight const & light
			, Vec2 const & uv
			, Vec3 const & position
			, Vec3 const & normal
			, Float const & translucency
			, Vec3 const & eye
			, Vec3 const & albedo
			, Float const & metallic )const
		{
			auto result = m_writer.declLocale( cuT( "result" )
				, vec3( 0.0_f ) );

			IF( m_writer, material.m_subsurfaceScatteringEnabled() != 0_i )
			{
				result = doComputeLightDist( light
						, uv
						, material.m_distanceBasedTransmission()
						, material.m_backLitCoefficient()
						, position )
					* translucency
					* lighting.computeDirectionalLightBackLit( light
						, eye
						, albedo
						, metallic
						, shader::FragmentInput( position, -normal ) );
			}
			FI;

			return result;
		}

		Vec3 SubsurfaceScattering::compute( MetallicBrdfLightingModel const & lighting
			, MetallicRoughnessMaterial const & material
			, PointLight const & light
			, Vec2 const & uv
			, Vec3 const & position
			, Vec3 const & normal
			, Float const & translucency
			, Vec3 const & eye
			, Vec3 const & albedo
			, Float const & metallic )const
		{
			auto result = m_writer.declLocale( cuT( "result" )
				, vec3( 0.0_f ) );

			IF( m_writer, material.m_subsurfaceScatteringEnabled() != 0_i )
			{
				result = doComputeLightDist( light
						, uv
						, material.m_distanceBasedTransmission()
						, material.m_backLitCoefficient()
						, position )
					* translucency
					* lighting.computePointLightBackLit( light
						, eye
						, albedo
						, metallic
						, shader::FragmentInput( position, -normal ) );
			}
			FI;

			return result;
		}

		Vec3 SubsurfaceScattering::compute( MetallicBrdfLightingModel const & lighting
			, MetallicRoughnessMaterial const & material
			, SpotLight const & light
			, Vec2 const & uv
			, Vec3 const & position
			, Vec3 const & normal
			, Float const & translucency
			, Vec3 const & eye
			, Vec3 const & albedo
			, Float const & metallic )const
		{
			auto result = m_writer.declLocale( cuT( "result" )
				, vec3( 0.0_f ) );

			IF( m_writer, material.m_subsurfaceScatteringEnabled() != 0_i )
			{
				result = doComputeLightDist( light
						, uv
						, material.m_distanceBasedTransmission()
						, material.m_backLitCoefficient()
						, position )
					* translucency
					* lighting.computeSpotLightBackLit( light
						, eye
						, albedo
						, metallic
						, shader::FragmentInput( position, -normal ) );
			}
			FI;

			return result;
		}

		Vec3 SubsurfaceScattering::compute( SpecularBrdfLightingModel const & lighting
			, SpecularGlossinessMaterial const & material
			, DirectionalLight const & light
			, Vec2 const & uv
			, Vec3 const & position
			, Vec3 const & normal
			, Float const & translucency
			, Vec3 const & eye
			, Vec3 const & specular )const
		{
			auto result = m_writer.declLocale( cuT( "result" )
				, vec3( 0.0_f ) );

			IF( m_writer, material.m_subsurfaceScatteringEnabled() != 0_i )
			{
				result = doComputeLightDist( light
						, uv
						, material.m_distanceBasedTransmission()
						, material.m_backLitCoefficient()
						, position )
					* translucency
					* lighting.computeDirectionalLightBackLit( light
						, eye
						, specular
						, shader::FragmentInput( position, -normal ) );
			}
			FI;

			return result;
		}

		Vec3 SubsurfaceScattering::compute( SpecularBrdfLightingModel const & lighting
			, SpecularGlossinessMaterial const & material
			, PointLight const & light
			, Vec2 const & uv
			, Vec3 const & position
			, Vec3 const & normal
			, Float const & translucency
			, Vec3 const & eye
			, Vec3 const & specular )const
		{
			auto result = m_writer.declLocale( cuT( "result" )
				, vec3( 0.0_f ) );

			IF( m_writer, material.m_subsurfaceScatteringEnabled() != 0_i )
			{
				result = doComputeLightDist( light
						, uv
						, material.m_distanceBasedTransmission()
						, material.m_backLitCoefficient()
						, position )
					* translucency
					* lighting.computePointLightBackLit( light
						, eye
						, specular
						, shader::FragmentInput( position, -normal ) );
			}
			FI;

			return result;
		}

		Vec3 SubsurfaceScattering::compute( SpecularBrdfLightingModel const & lighting
			, SpecularGlossinessMaterial const & material
			, SpotLight const & light
			, Vec2 const & uv
			, Vec3 const & position
			, Vec3 const & normal
			, Float const & translucency
			, Vec3 const & eye
			, Vec3 const & specular )const
		{
			auto result = m_writer.declLocale( cuT( "result" )
				, vec3( 0.0_f ) );

			IF( m_writer, material.m_subsurfaceScatteringEnabled() != 0_i )
			{
				result = doComputeLightDist( light
						, uv
						, material.m_distanceBasedTransmission()
						, material.m_backLitCoefficient()
						, position )
					* translucency
					* lighting.computeSpotLightBackLit( light
						, eye
						, specular
						, shader::FragmentInput( position, -normal ) );
			}
			FI;

			return result;
		}
		
		Vec3 SubsurfaceScattering::computeOne( PhongLightingModel const & lighting
			, LegacyMaterial const & material
			, DirectionalLight const & light
			, Vec2 const & uv
			, Vec3 const & position
			, Vec3 const & normal
			, Float const & translucency
			, Vec3 const & eye )const
		{
			auto result = m_writer.declLocale( cuT( "result" )
				, vec3( 0.0_f ) );

			IF( m_writer, material.m_subsurfaceScatteringEnabled() != 0_i )
			{
				result = doComputeLightDist( light
						, uv
						, material.m_distanceBasedTransmission()
						, material.m_backLitCoefficient()
						, position )
					* translucency
					* lighting.computeDirectionalLightBackLit( light
						, eye
						, shader::FragmentInput( position, -normal ) );
			}
			FI;

			return result;
		}

		Vec3 SubsurfaceScattering::computeOne( PhongLightingModel const & lighting
			, LegacyMaterial const & material
			, PointLight const & light
			, Vec2 const & uv
			, Vec3 const & position
			, Vec3 const & normal
			, Float const & translucency
			, Vec3 const & eye )const
		{
			auto result = m_writer.declLocale( cuT( "result" )
				, vec3( 0.0_f ) );

			IF( m_writer, material.m_subsurfaceScatteringEnabled() != 0_i )
			{
				result = doComputeOneLightDist( light
						, uv
						, material.m_distanceBasedTransmission()
						, material.m_backLitCoefficient()
						, position )
					* translucency
					* lighting.computePointLightBackLit( light
						, eye
						, shader::FragmentInput( position, -normal ) );
			}
			FI;

			return result;
		}

		Vec3 SubsurfaceScattering::computeOne( PhongLightingModel const & lighting
			, LegacyMaterial const & material
			, SpotLight const & light
			, Vec2 const & uv
			, Vec3 const & position
			, Vec3 const & normal
			, Float const & translucency
			, Vec3 const & eye )const
		{
			auto result = m_writer.declLocale( cuT( "result" )
				, vec3( 0.0_f ) );

			IF( m_writer, material.m_subsurfaceScatteringEnabled() != 0_i )
			{
				result = doComputeOneLightDist( light
						, uv
						, material.m_distanceBasedTransmission()
						, material.m_backLitCoefficient()
						, position )
					* translucency
					* lighting.computeSpotLightBackLit( light
						, eye
						, shader::FragmentInput( position, -normal ) );
			}
			FI;

			return result;
		}

		Vec3 SubsurfaceScattering::computeOne( MetallicBrdfLightingModel const & lighting
			, MetallicRoughnessMaterial const & material
			, DirectionalLight const & light
			, Vec2 const & uv
			, Vec3 const & position
			, Vec3 const & normal
			, Float const & translucency
			, Vec3 const & eye
			, Vec3 const & albedo
			, Float const & metallic )const
		{
			auto result = m_writer.declLocale( cuT( "result" )
				, vec3( 0.0_f ) );

			IF( m_writer, material.m_subsurfaceScatteringEnabled() != 0_i )
			{
				result = doComputeLightDist( light
						, uv
						, material.m_distanceBasedTransmission()
						, material.m_backLitCoefficient()
						, position )
					* translucency
					* lighting.computeDirectionalLightBackLit( light
						, eye
						, albedo
						, metallic
						, shader::FragmentInput( position, -normal ) );
			}
			FI;

			return result;
		}

		Vec3 SubsurfaceScattering::computeOne( MetallicBrdfLightingModel const & lighting
			, MetallicRoughnessMaterial const & material
			, PointLight const & light
			, Vec2 const & uv
			, Vec3 const & position
			, Vec3 const & normal
			, Float const & translucency
			, Vec3 const & eye
			, Vec3 const & albedo
			, Float const & metallic )const
		{
			auto result = m_writer.declLocale( cuT( "result" )
				, vec3( 0.0_f ) );

			IF( m_writer, material.m_subsurfaceScatteringEnabled() != 0_i )
			{
				result = doComputeOneLightDist( light
						, uv
						, material.m_distanceBasedTransmission()
						, material.m_backLitCoefficient()
						, position )
					* translucency
					* lighting.computePointLightBackLit( light
						, eye
						, albedo
						, metallic
						, shader::FragmentInput( position, -normal ) );
			}
			FI;

			return result;
		}

		Vec3 SubsurfaceScattering::computeOne( MetallicBrdfLightingModel const & lighting
			, MetallicRoughnessMaterial const & material
			, SpotLight const & light
			, Vec2 const & uv
			, Vec3 const & position
			, Vec3 const & normal
			, Float const & translucency
			, Vec3 const & eye
			, Vec3 const & albedo
			, Float const & metallic )const
		{
			auto result = m_writer.declLocale( cuT( "result" )
				, vec3( 0.0_f ) );

			IF( m_writer, material.m_subsurfaceScatteringEnabled() != 0_i )
			{
				result = doComputeOneLightDist( light
						, uv
						, material.m_distanceBasedTransmission()
						, material.m_backLitCoefficient()
						, position )
					* translucency
					* lighting.computeSpotLightBackLit( light
						, eye
						, albedo
						, metallic
						, shader::FragmentInput( position, -normal ) );
			}
			FI;

			return result;
		}

		Vec3 SubsurfaceScattering::computeOne( SpecularBrdfLightingModel const & lighting
			, SpecularGlossinessMaterial const & material
			, DirectionalLight const & light
			, Vec2 const & uv
			, Vec3 const & position
			, Vec3 const & normal
			, Float const & translucency
			, Vec3 const & eye
			, Vec3 const & specular )const
		{
			auto result = m_writer.declLocale( cuT( "result" )
				, vec3( 0.0_f ) );

			IF( m_writer, material.m_subsurfaceScatteringEnabled() != 0_i )
			{
				result = doComputeLightDist( light
						, uv
						, material.m_distanceBasedTransmission()
						, material.m_backLitCoefficient()
						, position )
					* translucency
					* lighting.computeDirectionalLightBackLit( light
						, eye
						, specular
						, shader::FragmentInput( position, -normal ) );
			}
			FI;

			return result;
		}

		Vec3 SubsurfaceScattering::computeOne( SpecularBrdfLightingModel const & lighting
			, SpecularGlossinessMaterial const & material
			, PointLight const & light
			, Vec2 const & uv
			, Vec3 const & position
			, Vec3 const & normal
			, Float const & translucency
			, Vec3 const & eye
			, Vec3 const & specular )const
		{
			auto result = m_writer.declLocale( cuT( "result" )
				, vec3( 0.0_f ) );

			IF( m_writer, material.m_subsurfaceScatteringEnabled() != 0_i )
			{
				result = doComputeOneLightDist( light
						, uv
						, material.m_distanceBasedTransmission()
						, material.m_backLitCoefficient()
						, position )
					* translucency
					* lighting.computePointLightBackLit( light
						, eye
						, specular
						, shader::FragmentInput( position, -normal ) );
			}
			FI;

			return result;
		}

		Vec3 SubsurfaceScattering::computeOne( SpecularBrdfLightingModel const & lighting
			, SpecularGlossinessMaterial const & material
			, SpotLight const & light
			, Vec2 const & uv
			, Vec3 const & position
			, Vec3 const & normal
			, Float const & translucency
			, Vec3 const & eye
			, Vec3 const & specular )const
		{
			auto result = m_writer.declLocale( cuT( "result" )
				, vec3( 0.0_f ) );

			IF( m_writer, material.m_subsurfaceScatteringEnabled() != 0_i )
			{
				result = doComputeOneLightDist( light
						, uv
						, material.m_distanceBasedTransmission()
						, material.m_backLitCoefficient()
						, position )
					* translucency
					* lighting.computeSpotLightBackLit( light
						, eye
						, specular
						, shader::FragmentInput( position, -normal ) );
			}
			FI;

			return result;
		}

		Vec3 SubsurfaceScattering::doComputeLightDist( DirectionalLight const & light
			, Vec2 const & uv
			, glsl::Int const & distanceBasedTransmission
			, Vec3 const & coefficient
			, Vec3 const & position )const
		{
			return m_computeDirectionalLightDist( light
				, uv
				, distanceBasedTransmission
				, coefficient
				, position );
		}

		Vec3 SubsurfaceScattering::doComputeLightDist( PointLight const & light
			, Vec2 const & uv
			, glsl::Int const & distanceBasedTransmission
			, Vec3 const & coefficient
			, Vec3 const & position )const
		{
			return m_computePointLightDist( light
				, uv
				, distanceBasedTransmission
				, coefficient
				, position );
		}

		Vec3 SubsurfaceScattering::doComputeLightDist( SpotLight const & light
			, Vec2 const & uv
			, glsl::Int const & distanceBasedTransmission
			, Vec3 const & coefficient
			, Vec3 const & position )const
		{
			return m_computeSpotLightDist( light
				, uv
				, distanceBasedTransmission
				, coefficient
				, position );
		}

		Vec3 SubsurfaceScattering::doComputeOneLightDist( PointLight const & light
			, Vec2 const & uv
			, glsl::Int const & distanceBasedTransmission
			, Vec3 const & coefficient
			, Vec3 const & position )const
		{
			return m_computeOnePointLightDist( light
				, uv
				, distanceBasedTransmission
				, coefficient
				, position );
		}

		Vec3 SubsurfaceScattering::doComputeOneLightDist( SpotLight const & light
			, Vec2 const & uv
			, glsl::Int const & distanceBasedTransmission
			, Vec3 const & coefficient
			, Vec3 const & position )const
		{
			return m_computeOneSpotLightDist( light
				, uv
				, distanceBasedTransmission
				, coefficient
				, position );
		}
		
		void SubsurfaceScattering::doDeclareGetTransformedPosition()
		{
			m_getTransformedPosition = m_writer.implementFunction< Vec3 >( cuT( "getTransformedPosition" )
				, [this]( Vec3 const & position
					, Mat4 const & transform )
				{
					auto transformed = m_writer.declLocale( cuT( "transformed" )
						, transform * vec4( position, 1.0_f ) );
					// Perspective divide (result in range [-1,1]).
					transformed.xyz() = transformed.xyz() / transformed.w();
					// Now put the position in range [0,1].
					m_writer.returnStmt( m_writer.paren( transformed.xyz() * 0.5_f ) + vec3( 0.5_f ) );
				}
				, InVec3( &m_writer, cuT( "position" ) )
				, InMat4( &m_writer, cuT( "transform" ) ) );
		}

		void SubsurfaceScattering::doDeclareComputeDirectionalLightDist()
		{
			m_computeDirectionalLightDist = m_writer.implementFunction< Vec3 >( cuT( "computeDirectionalLightDist" )
				, [this]( DirectionalLight const & light
					, Vec2 const & uv
					, Int const & distanceBasedTransmission
					, Vec3 const & coefficient
					, Vec3 const & position )
				{
					auto factor = m_writer.declLocale( cuT( "factor" )
						, coefficient );

					if ( m_shadowMap )
					{
						IF( m_writer, distanceBasedTransmission != 0_i )
						{
							auto c3d_mtxInvViewProj = m_writer.getBuiltin< Mat4 >( cuT( "c3d_mtxInvViewProj" ) );
							auto c3d_mapShadowDirectional = m_writer.getBuiltin< Sampler2D >( Shadow::MapShadowDirectional );

							auto lightSpacePosition = m_writer.declLocale( cuT( "lightSpacePosition" )
								, m_getTransformedPosition( position, light.m_transform() ) );
							auto lightSpaceDepth = m_writer.declLocale( cuT( "lightSpaceDepth" )
								, texture( c3d_mapShadowDirectional, lightSpacePosition.xy() ).r() );
							auto occluder = m_writer.declLocale( cuT( "occluder" )
								, writeFunctionCall< Vec3 >( &m_writer
									, cuT( "calcWSPosition" )
									, uv
									, lightSpaceDepth
									, inverse( light.m_transform() ) ) );
							auto distance = m_writer.declLocale( cuT( "distance" )
								, glsl::distance( occluder, position ) );
							factor = exp( -coefficient * distance );
						}
						FI;
					}

					m_writer.returnStmt( factor );
				}
				, InParam< DirectionalLight >{ &m_writer, cuT( "light" ) }
				, InVec2{ &m_writer, cuT( "uv" ) }
				, InInt{ &m_writer, cuT( "distanceBasedTransmission" ) }
				, InVec3{ &m_writer, cuT( "coefficient" ) }
				, InVec3{ &m_writer, cuT( "position" ) } );
		}

		void SubsurfaceScattering::doDeclareComputePointLightDist()
		{
			m_computePointLightDist = m_writer.implementFunction< Vec3 >( cuT( "computePointLightDist" )
				, [this]( PointLight const & light
					, Vec2 const & uv
					, Int const & distanceBasedTransmission
					, Vec3 const & coefficient
					, Vec3 const & position )
				{
					auto factor = m_writer.declLocale( cuT( "factor" )
						, coefficient );

					if ( m_shadowMap )
					{
						IF( m_writer, distanceBasedTransmission != 0_i )
						{
							auto c3d_mtxInvViewProj = m_writer.getBuiltin< Mat4 >( cuT( "c3d_mtxInvViewProj" ) );
							auto c3d_mapShadowPoint = m_writer.getBuiltin< SamplerCube >( Shadow::MapShadowPoint, PointShadowMapCount );

							auto vertexToLight = m_writer.declLocale( cuT( "vertexToLight" )
								, position - light.m_position() );
							auto direction = m_writer.declLocale( cuT( "direction" )
								, vertexToLight );
							auto lightSpaceDepth = m_writer.declLocale( cuT( "lightSpaceDepth" )
								, texture( c3d_mapShadowPoint[light.m_index()], direction ).r() );
							auto occluder = m_writer.declLocale( cuT( "occluder" )
								, writeFunctionCall< Vec3 >( &m_writer
									, cuT( "calcWSPosition" )
									, uv
									, lightSpaceDepth
									, c3d_mtxInvViewProj ) );
							auto distance = m_writer.declLocale( cuT( "distance" )
								, glsl::distance( occluder, position ) );
							factor = exp( -coefficient * distance );
						}
						FI;
					}

					m_writer.returnStmt( factor );
				}
				, InParam< PointLight >{ &m_writer, cuT( "light" ) }
				, InVec2{ &m_writer, cuT( "uv" ) }
				, InInt{ &m_writer, cuT( "distanceBasedTransmission" ) }
				, InVec3{ &m_writer, cuT( "coefficient" ) }
				, InVec3{ &m_writer, cuT( "position" ) } );
		}

		void SubsurfaceScattering::doDeclareComputeSpotLightDist()
		{
			m_computeSpotLightDist = m_writer.implementFunction< Vec3 >( cuT( "computeSpotLightDist" )
				, [this]( SpotLight const & light
					, Vec2 const & uv
					, Int const & distanceBasedTransmission
					, Vec3 const & coefficient
					, Vec3 const & position )
				{
					auto factor = m_writer.declLocale( cuT( "factor" )
						, coefficient );

					if ( m_shadowMap )
					{
						IF( m_writer, distanceBasedTransmission != 0_i )
						{
							auto c3d_mtxInvViewProj = m_writer.getBuiltin< Mat4 >( cuT( "c3d_mtxInvViewProj" ) );
							auto c3d_mapShadowSpot = m_writer.getBuiltin< Sampler2D >( Shadow::MapShadowSpot, SpotShadowMapCount );

							auto lightSpacePosition = m_writer.declLocale( cuT( "lightSpacePosition" )
								, m_getTransformedPosition( position, light.m_transform() ) );
							auto lightSpaceDepth = m_writer.declLocale( cuT( "lightSpaceDepth" )
								, texture( c3d_mapShadowSpot[light.m_index()], lightSpacePosition.xy() ).r() );
							auto occluder = m_writer.declLocale( cuT( "occluder" )
								, writeFunctionCall< Vec3 >( &m_writer
									, cuT( "calcWSPosition" )
									, uv
									, lightSpaceDepth
									, inverse( light.m_transform() ) ) );
							auto distance = m_writer.declLocale( cuT( "distance" )
								, glsl::distance( occluder, position ) );
							factor = exp( -coefficient * distance );
						}
						FI;
					}

					m_writer.returnStmt( factor );
				}
				, InParam< SpotLight >{ &m_writer, cuT( "light" ) }
				, InVec2{ &m_writer, cuT( "uv" ) }
				, InInt{ &m_writer, cuT( "distanceBasedTransmission" ) }
				, InVec3{ &m_writer, cuT( "coefficient" ) }
				, InVec3{ &m_writer, cuT( "position" ) } );
		}

		void SubsurfaceScattering::doDeclareComputeOnePointLightDist()
		{
			m_computeOnePointLightDist = m_writer.implementFunction< Vec3 >( cuT( "computeOnePointLightDist" )
				, [this]( PointLight const & light
					, Vec2 const & uv
					, Int const & distanceBasedTransmission
					, Vec3 const & coefficient
					, Vec3 const & position )
				{
					auto factor = m_writer.declLocale( cuT( "factor" )
						, coefficient );

					if ( m_shadowMap )
					{
						IF( m_writer, distanceBasedTransmission != 0_i )
						{
							auto c3d_mtxInvViewProj = m_writer.getBuiltin< Mat4 >( cuT( "c3d_mtxInvViewProj" ) );
							auto c3d_mapShadowPoint = m_writer.getBuiltin< SamplerCube >( Shadow::MapShadowPoint );

							auto vertexToLight = m_writer.declLocale( cuT( "vertexToLight" )
								, position - light.m_position() );
							auto direction = m_writer.declLocale( cuT( "direction" )
								, vertexToLight );
							auto lightSpaceDepth = m_writer.declLocale( cuT( "lightSpaceDepth" )
								, texture( c3d_mapShadowPoint, direction ).r() );
							auto occluder = m_writer.declLocale( cuT( "occluder" )
								, writeFunctionCall< Vec3 >( &m_writer
									, cuT( "calcWSPosition" )
									, uv
									, lightSpaceDepth
									, c3d_mtxInvViewProj ) );
							auto distance = m_writer.declLocale( cuT( "distance" )
								, glsl::distance( occluder, position ) );
							factor = exp( -coefficient * distance );
						}
						FI;
					}

					m_writer.returnStmt( factor );
				}
				, InParam< PointLight >{ &m_writer, cuT( "light" ) }
				, InVec2{ &m_writer, cuT( "uv" ) }
				, InInt{ &m_writer, cuT( "distanceBasedTransmission" ) }
				, InVec3{ &m_writer, cuT( "coefficient" ) }
				, InVec3{ &m_writer, cuT( "position" ) } );
		}

		void SubsurfaceScattering::doDeclareComputeOneSpotLightDist()
		{
			m_computeOneSpotLightDist = m_writer.implementFunction< Vec3 >( cuT( "computeOneSpotLightDist" )
				, [this]( SpotLight const & light
					, Vec2 const & uv
					, Int const & distanceBasedTransmission
					, Vec3 const & coefficient
					, Vec3 const & position )
				{
					auto factor = m_writer.declLocale( cuT( "factor" )
						, coefficient );

					if ( m_shadowMap )
					{
						IF( m_writer, distanceBasedTransmission != 0_i )
						{
							auto c3d_mapShadowSpot = m_writer.getBuiltin< Sampler2D >( Shadow::MapShadowSpot );

							auto lightSpacePosition = m_writer.declLocale( cuT( "lightSpacePosition" )
								, m_getTransformedPosition( position, light.m_transform() ) );
							auto lightSpaceDepth = m_writer.declLocale( cuT( "lightSpaceDepth" )
								, texture( c3d_mapShadowSpot, lightSpacePosition.xy() ).r() );
							auto occluder = m_writer.declLocale( cuT( "occluder" )
								, writeFunctionCall< Vec3 >( &m_writer
									, cuT( "calcWSPosition" )
									, uv
									, lightSpaceDepth
									, inverse( light.m_transform() ) ) );
							auto distance = m_writer.declLocale( cuT( "distance" )
								, glsl::distance( occluder, position ) );
							factor = exp( -coefficient * distance );
						}
						FI;
					}

					m_writer.returnStmt( factor );
				}
				, InParam< SpotLight >{ &m_writer, cuT( "light" ) }
				, InVec2{ &m_writer, cuT( "uv" ) }
				, InInt{ &m_writer, cuT( "distanceBasedTransmission" ) }
				, InVec3{ &m_writer, cuT( "coefficient" ) }
				, InVec3{ &m_writer, cuT( "position" ) } );
		}
	}
}
