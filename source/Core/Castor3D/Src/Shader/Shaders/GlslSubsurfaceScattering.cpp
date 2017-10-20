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
		castor::String const SubsurfaceScattering::TransmittanceProfile = cuT( "c3d_transmittanceProfile" );
		castor::String const SubsurfaceScattering::ProfileFactorsCount = cuT( "c3d_profileFactorsCount" );

		SubsurfaceScattering::SubsurfaceScattering( GlslWriter & writer
			, bool shadowMap )
			: m_writer{ writer }
			, m_shadowMap{ shadowMap }
		{
		}

		void SubsurfaceScattering::declare( LightType type )
		{
			if ( m_shadowMap )
			{
				auto c3d_transmittanceProfile = m_writer.declUniform< Vec4 >( SubsurfaceScattering::TransmittanceProfile, 10u );
				auto c3d_profileFactorsCount = m_writer.declUniform< Int >( SubsurfaceScattering::ProfileFactorsCount );
				doDeclareGetTransformedPosition();
			}

			switch ( type )
			{
			case LightType::eDirectional:
			{
				doDeclareComputeDirectionalTransmittance();
				break;
			}

			case LightType::ePoint:
			{
				doDeclareComputePointTransmittance();
				break;
			}

			case LightType::eSpot:
			{
				doDeclareComputeSpotTransmittance();
				break;
			}

			default:
				CASTOR_EXCEPTION( "Unsupported light type for transmittance processing" );
			}
		}
		
		Vec3 SubsurfaceScattering::compute( BaseMaterial const & material
			, DirectionalLight const & light
			, Vec2 const & uv
			, Vec3 const & position
			, Vec3 const & normal
			, Float const & translucency )const
		{
			auto result = m_writer.declLocale( cuT( "result" )
				, vec3( 0.0_f ) );

			IF( m_writer, material.m_subsurfaceScatteringEnabled() != 0_i )
			{
				result = doComputeTransmittance( light
					, uv
					, material.m_transmittanceProfileSize()
					, material.m_transmittanceProfile()
					, material.m_gaussianWidth()
					, position
					, normal
					, translucency );
			}
			FI;

			return result;
		}

		Vec3 SubsurfaceScattering::compute( BaseMaterial const & material
			, PointLight const & light
			, Vec2 const & uv
			, Vec3 const & position
			, Vec3 const & normal
			, Float const & translucency )const
		{
			auto result = m_writer.declLocale( cuT( "result" )
				, vec3( 0.0_f ) );

			IF( m_writer, material.m_subsurfaceScatteringEnabled() != 0_i )
			{
				result = doComputeTransmittance( light
					, uv
					, material.m_transmittanceProfileSize()
					, material.m_transmittanceProfile()
					, material.m_gaussianWidth()
					, position
					, normal
					, translucency );
			}
			FI;

			return result;
		}

		Vec3 SubsurfaceScattering::compute( BaseMaterial const & material
			, SpotLight const & light
			, Vec2 const & uv
			, Vec3 const & position
			, Vec3 const & normal
			, Float const & translucency )const
		{
			auto result = m_writer.declLocale( cuT( "result" )
				, vec3( 0.0_f ) );

			IF( m_writer, material.m_subsurfaceScatteringEnabled() != 0_i )
			{
				result = doComputeTransmittance( light
					, uv
					, material.m_transmittanceProfileSize()
					, material.m_transmittanceProfile()
					, material.m_gaussianWidth()
					, position
					, normal
					, translucency );
			}
			FI;

			return result;
		}

		Vec3 SubsurfaceScattering::doComputeTransmittance( DirectionalLight const & light
			, Vec2 const & uv
			, glsl::Int const & transmittanceProfileSize
			, glsl::Array< glsl::Vec4 > const & transmittanceProfile
			, Float const & width
			, Vec3 const & position
			, Vec3 const & normal
			, Float const & translucency )const
		{
			return m_computeDirectionalTransmittance( light
				, uv
				, transmittanceProfileSize
				, transmittanceProfile
				, width
				, position
				, normal
				, translucency );
		}

		Vec3 SubsurfaceScattering::doComputeTransmittance( PointLight const & light
			, Vec2 const & uv
			, glsl::Int const & transmittanceProfileSize
			, glsl::Array< glsl::Vec4 > const & transmittanceProfile
			, Float const & width
			, Vec3 const & position
			, Vec3 const & normal
			, Float const & translucency )const
		{
			return m_computePointTransmittance( light
				, uv
				, transmittanceProfileSize
				, transmittanceProfile
				, width
				, position
				, normal
				, translucency );
		}

		Vec3 SubsurfaceScattering::doComputeTransmittance( SpotLight const & light
			, Vec2 const & uv
			, glsl::Int const & transmittanceProfileSize
			, glsl::Array< glsl::Vec4 > const & transmittanceProfile
			, Float const & width
			, Vec3 const & position
			, Vec3 const & normal
			, Float const & translucency )const
		{
			return m_computeSpotTransmittance( light
				, uv
				, transmittanceProfileSize
				, transmittanceProfile
				, width
				, position
				, normal
				, translucency );
		}
		
		void SubsurfaceScattering::doDeclareGetTransformedPosition()
		{
			m_getTransformedPosition = m_writer.implementFunction< Vec3 >( cuT( "getTransformedPosition" )
				, [this]( Vec3 const & position
					, Mat4 const & transform )
				{
					auto transformed = m_writer.declLocale( cuT( "transformed" )
						, transform * vec4( position, 1.0_f ) );
					// Perspective divide (result in range [0,1]).
					m_writer.returnStmt( transformed.xyz() / transformed.w() );
				}
				, InVec3( &m_writer, cuT( "position" ) )
				, InMat4( &m_writer, cuT( "transform" ) ) );
		}

		void SubsurfaceScattering::doDeclareComputeDirectionalTransmittance()
		{
			m_computeDirectionalTransmittance = m_writer.implementFunction< Vec3 >( cuT( "computeDirectionalLightDist" )
				, [this]( DirectionalLight const & light
					, Vec2 const & uv
					, glsl::Int const & transmittanceProfileSize
					, glsl::Array< glsl::Vec4 > const & transmittanceProfile
					, Float const & width
					, Vec3 const & position
					, Vec3 const & normal
					, Float const & translucency )
				{
					auto factor = m_writer.declLocale( cuT( "factor" )
						, vec3( 0.0_f ) );

					if ( m_shadowMap )
					{
						IF( m_writer, transmittanceProfileSize > 0_i )
						{
							auto c3d_mapDepthDirectional = m_writer.getBuiltin< Sampler2D >( Shadow::MapDepthDirectional );

							auto lightSpacePosition = m_writer.declLocale( cuT( "lightSpacePosition" )
								, m_getTransformedPosition( position, light.m_transform() ) );
							auto lightSpaceDepth = m_writer.declLocale( cuT( "lightSpaceDepth" )
								, texture( c3d_mapDepthDirectional, lightSpacePosition.xy() ).r() );
							auto occluder = m_writer.declLocale( cuT( "occluder" )
								, writeFunctionCall< Vec3 >( &m_writer
									, cuT( "calcWSPosition" )
									, uv
									, lightSpaceDepth
									, inverse( light.m_transform() ) ) );
							//auto distance = m_writer.declLocale( cuT( "distance" )
							//	, glsl::distance( viewMatrix * occluder, position ) );
							//factor = coefficient * exp( -distance );
						}
						FI;
					}

					m_writer.returnStmt( factor );
				}
				, InParam< DirectionalLight >{ &m_writer, cuT( "light" ) }
				, InVec2{ &m_writer, cuT( "uv" ) }
				, InInt{ &m_writer, cuT( "transmittanceProfileSize" ) }
				, InArrayParam< Vec4 >{ &m_writer, cuT( "transmittanceProfile" ), 10u }
				, InFloat{ &m_writer, cuT( "width" ) }
				, InVec3{ &m_writer, cuT( "position" ) }
				, InVec3{ &m_writer, cuT( "normal" ) }
				, InFloat{ &m_writer, cuT( "translucency" ) } );
		}

		void SubsurfaceScattering::doDeclareComputePointTransmittance()
		{
			m_computePointTransmittance = m_writer.implementFunction< Vec3 >( cuT( "computePointLightDist" )
				, [this]( PointLight const & light
					, Vec2 const & uv
					, glsl::Int const & transmittanceProfileSize
					, glsl::Array< glsl::Vec4 > const & transmittanceProfile
					, Float const & width
					, Vec3 const & position
					, Vec3 const & normal
					, Float const & translucency )
				{
					auto factor = m_writer.declLocale( cuT( "factor" )
						, vec3( 0.0_f ) );

					if ( m_shadowMap )
					{
						IF( m_writer, transmittanceProfileSize > 0_i )
						{
							auto c3d_mtxInvViewProj = m_writer.getBuiltin< Mat4 >( cuT( "c3d_mtxInvViewProj" ) );
							auto c3d_mapDepthPoint = m_writer.getBuiltin< SamplerCube >( Shadow::MapDepthPoint );

							auto vertexToLight = m_writer.declLocale( cuT( "vertexToLight" )
								, position - light.m_position() );
							auto direction = m_writer.declLocale( cuT( "direction" )
								, vertexToLight );
							auto lightSpaceDepth = m_writer.declLocale( cuT( "lightSpaceDepth" )
								, texture( c3d_mapDepthPoint, direction ).r() );
							auto occluder = m_writer.declLocale( cuT( "occluder" )
								, writeFunctionCall< Vec3 >( &m_writer
									, cuT( "calcWSPosition" )
									, uv
									, lightSpaceDepth
									, c3d_mtxInvViewProj ) );
							//auto distance = m_writer.declLocale( cuT( "distance" )
							//	, glsl::distance( vec3( viewMatrix * vec4( occluder, 1.0_f ) ), position ) );
							//factor = coefficient * exp( -distance );
						}
						FI;
					}

					m_writer.returnStmt( factor );
				}
				, InParam< PointLight >{ &m_writer, cuT( "light" ) }
				, InVec2{ &m_writer, cuT( "uv" ) }
				, InInt{ &m_writer, cuT( "transmittanceProfileSize" ) }
				, InArrayParam< Vec4 >{ &m_writer, cuT( "transmittanceProfile" ), 10u }
				, InFloat{ &m_writer, cuT( "width" ) }
				, InVec3{ &m_writer, cuT( "position" ) }
				, InVec3{ &m_writer, cuT( "normal" ) }
				, InFloat{ &m_writer, cuT( "translucency" ) } );
		}

		void SubsurfaceScattering::doDeclareComputeSpotTransmittance()
		{
			m_computeSpotTransmittance = m_writer.implementFunction< Vec3 >( cuT( "computeSpotLightDist" )
				, [this]( SpotLight const & light
					, Vec2 const & uv
					, glsl::Int const & transmittanceProfileSize
					, glsl::Array< glsl::Vec4 > const & transmittanceProfile
					, Float const & width
					, Vec3 const & position
					, Vec3 const & normal
					, Float const & translucency )
				{
					auto factor = m_writer.declLocale( cuT( "factor" )
						, vec3( 0.0_f ) );

					if ( m_shadowMap )
					{
						IF( m_writer, transmittanceProfileSize > 0_i )
						{
							auto c3d_mapDepthSpot = m_writer.getBuiltin< Sampler2D >( Shadow::MapDepthSpot );
							/**
							 * Calculate the scale of the effect.
							 */
							auto scale = m_writer.declLocale( cuT( "scale" )
								, 8.25_f * translucency / width );

							/**
							 * First we shrink the position inwards the surface to avoid artifacts:
							 * (Note that this can be done once for all the lights)
							 */
							auto shrinkedPos = m_writer.declLocale( cuT( "shrinkedPos" )
								, position - normal * 0.005 );

							/**
							 * Now we calculate the thickness from the light point of view:
							 */
							auto lightSpacePosition = m_writer.declLocale( cuT( "lightSpacePosition" )
								, m_getTransformedPosition( shrinkedPos, light.m_transform() ) );
							auto lightSpaceDepth = m_writer.declLocale( cuT( "d1" )
								, texture( c3d_mapDepthSpot, lightSpacePosition.xy() ).r() );
							auto distance = m_writer.declLocale( cuT( "distance" )
								, scale * glsl::abs( lightSpaceDepth - lightSpacePosition.z() ) * light.m_lightBase().m_farPlane() );

#if C3D_DEBUG_SSS_TRANSMITTANCE

							factor = vec3( -distance * distance );
							//distance = -distance * distance;
							//factor = vec3( 0.233_f, 0.455, 0.649 ) * exp( distance / 0.0064 )
							//	+ vec3( 0.1_f, 0.336, 0.344 ) * exp( distance / 0.0484 )
							//	+ vec3( 0.118_f, 0.198, 0.0 ) * exp( distance / 0.187 )
							//	+ vec3( 0.113_f, 0.007, 0.007 ) * exp( distance / 0.567 )
							//	+ vec3( 0.358_f, 0.004, 0.0 ) * exp( distance / 1.99 )
							//	+ vec3( 0.078_f, 0.0, 0.0 ) * exp( distance / 7.41 );
							//factor = factor * clamp( 0.3_f + dot( light.m_direction(), normal )
							//	, 0.0_f
							//	, 1.0_f );

#else

							/**
							 * Armed with the thickness, we can now calculate the color by means of the
							 * transmittance profile.
							 */
							distance = -distance * distance;

							FOR( m_writer, Int, i, 0, "i < transmittanceProfileSize", "++i" )
							{
								auto profileFactor = m_writer.declLocale( cuT( "profileFactor" )
									, transmittanceProfile[i] );
								factor += profileFactor.rgb() * exp( distance / profileFactor.a() );
							}
							ROF;

							/**
							 * Using the profile, we finally approximate the transmitted lighting from
							 * the back of the object:
							 */
							factor = factor * clamp( 0.3_f + dot( light.m_direction(), -normal )
								, 0.0_f
								, 1.0_f );

#endif
						}
						FI;
					}

					m_writer.returnStmt( factor );
				}
				, InParam< SpotLight >{ &m_writer, cuT( "light" ) }
				, InVec2{ &m_writer, cuT( "uv" ) }
				, InInt{ &m_writer, cuT( "transmittanceProfileSize" ) }
				, InArrayParam< Vec4 >{ &m_writer, cuT( "transmittanceProfile" ), 10u }
				, InFloat{ &m_writer, cuT( "width" ) }
				, InVec3{ &m_writer, cuT( "position" ) }
				, InVec3{ &m_writer, cuT( "normal" ) }
				, InFloat{ &m_writer, cuT( "translucency" ) } );
		}
	}
}
