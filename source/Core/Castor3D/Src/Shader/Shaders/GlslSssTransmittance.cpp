#include "GlslSssTransmittance.hpp"

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
		castor::String const SssTransmittance::TransmittanceProfile = cuT( "c3d_transmittanceProfile" );
		castor::String const SssTransmittance::ProfileFactorsCount = cuT( "c3d_profileFactorsCount" );

		SssTransmittance::SssTransmittance( GlslWriter & writer
			, bool shadowMap )
			: m_writer{ writer }
			, m_shadowMap{ shadowMap }
		{
		}

		void SssTransmittance::declare( LightType type )
		{
			if ( m_shadowMap )
			{
				auto c3d_transmittanceProfile = m_writer.declUniform< Vec4 >( SssTransmittance::TransmittanceProfile, 10u );
				auto c3d_profileFactorsCount = m_writer.declUniform< Int >( SssTransmittance::ProfileFactorsCount );
			}

			doDeclareComputeTransmittance();
		}
		
		Vec3 SssTransmittance::compute( BaseMaterial const & material
			, DirectionalLight const & light
			, Vec2 const & uv
			, Vec3 const & position
			, Vec3 const & normal
			, Float const & translucency )const
		{
			auto result = m_writer.declLocale( cuT( "result" )
				, vec3( 0.0_f ) );

			if ( m_shadowMap )
			{
				IF( m_writer, material.m_subsurfaceScatteringEnabled() != 0_i )
				{
					auto c3d_mapDepthDirectional = m_writer.getBuiltin< Sampler2D >( Shadow::MapDepthDirectional );
					/**
					* First we shrink the position inwards the surface to avoid artifacts:
					* (Note that this can be done once for all the lights)
					*/
					auto shrinkedPos = m_writer.declLocale( cuT( "shrinkedPos" )
						, position - normal * 0.005 );
					auto lightSpacePosition = m_writer.declLocale( cuT( "lightSpacePosition" )
						, writeFunctionCall< Vec3 >( &m_writer, cuT( "getLightSpacePosition" )
							, light.m_transform()
							, shrinkedPos ) );
					auto shadowDepth = m_writer.declLocale( cuT( "d1" )
						, texture( c3d_mapDepthDirectional, lightSpacePosition.xy() ).r() );
					result = doCompute( shadowDepth
						, material.m_transmittanceProfileSize()
						, material.m_transmittanceProfile()
						, material.m_gaussianWidth()
						, lightSpacePosition
						, normal
						, translucency
						, light.m_direction()
						, light.m_lightBase().m_farPlane()
						, vec3( 1.0_f, 0.0_f, 0.0_f ) );
				}
				FI;
			}

			return result;
		}

		Vec3 SssTransmittance::compute( BaseMaterial const & material
			, PointLight const & light
			, Vec2 const & uv
			, Vec3 const & position
			, Vec3 const & normal
			, Float const & translucency )const
		{
			auto result = m_writer.declLocale( cuT( "result" )
				, vec3( 0.0_f ) );

			if ( m_shadowMap )
			{
				IF( m_writer, material.m_subsurfaceScatteringEnabled() != 0_i )
				{
					auto c3d_mtxInvViewProj = m_writer.getBuiltin< Mat4 >( cuT( "c3d_mtxInvViewProj" ) );
					auto c3d_mapDepthPoint = m_writer.getBuiltin< SamplerCube >( Shadow::MapDepthPoint );
					/**
					* First we shrink the position inwards the surface to avoid artifacts:
					* (Note that this can be done once for all the lights)
					*/
					auto shrinkedPos = m_writer.declLocale( cuT( "shrinkedPos" )
						, position - normal * 0.005 );

					auto vertexToLight = m_writer.declLocale( cuT( "vertexToLight" )
						, shrinkedPos - light.m_position() );
					auto direction = m_writer.declLocale( cuT( "direction" )
						, vertexToLight );
					auto shadowDepth = m_writer.declLocale( cuT( "shadowDepth" )
						, texture( c3d_mapDepthPoint, direction ).r() );
					auto lightSpacePosition = m_writer.declLocale( cuT( "lightSpacePosition" )
						, writeFunctionCall< Vec3 >( &m_writer
							, cuT( "calcWSPosition" )
							, uv
							, shadowDepth
							, c3d_mtxInvViewProj ) );
					result = doCompute( shadowDepth
						, material.m_transmittanceProfileSize()
						, material.m_transmittanceProfile()
						, material.m_gaussianWidth()
						, lightSpacePosition
						, normal
						, translucency
						, normalize( -vertexToLight )
						, light.m_lightBase().m_farPlane()
						, light.m_attenuation() );
				}
				FI;
			}

			return result;
		}

		Vec3 SssTransmittance::compute( BaseMaterial const & material
			, SpotLight const & light
			, Vec2 const & uv
			, Vec3 const & position
			, Vec3 const & normal
			, Float const & translucency )const
		{
			auto result = m_writer.declLocale( cuT( "result" )
				, vec3( 0.0_f ) );

			if ( m_shadowMap )
			{
				IF( m_writer, material.m_subsurfaceScatteringEnabled() != 0_i )
				{
					auto c3d_mapDepthSpot = m_writer.getBuiltin< Sampler2D >( Shadow::MapShadowSpot );
					// We shrink the position inwards the surface to avoid artifacts.
					auto shrinkedPos = m_writer.declLocale( cuT( "shrinkedPos" )
						, position - normal * 0.005 );
					auto lightSpacePosition = m_writer.declLocale( cuT( "lightSpacePosition" )
						, writeFunctionCall< Vec3 >( &m_writer, cuT( "getLightSpacePosition" )
							, light.m_transform()
							, shrinkedPos ) );
					auto shadowDepth = m_writer.declLocale( cuT( "shadowDepth" )
						, texture( c3d_mapDepthSpot, lightSpacePosition.xy() ).r() );
					result = doCompute( shadowDepth
						, material.m_transmittanceProfileSize()
						, material.m_transmittanceProfile()
						, material.m_gaussianWidth()
						, lightSpacePosition
						, normal
						, translucency
						, normalize( light.m_position() - position )
						, light.m_lightBase().m_farPlane()
						, light.m_attenuation() );
				}
				FI;
			}

			return result;
		}

		Vec3 SssTransmittance::doCompute( Float const & shadowDepth
			, Int const & transmittanceProfileSize
			, Array< Vec4 > const & transmittanceProfile
			, Float const & sssWidth
			, Vec3 const & worldPosition
			, Vec3 const & worldNormal
			, Float const & transmittance
			, Vec3 const & lightVector
			, Float const & lightFarPlane
			, Vec3 const & lightAttenuation )const
		{
			return m_compute( shadowDepth
				, transmittanceProfileSize
				, transmittanceProfile
				, sssWidth
				, worldPosition
				, worldNormal
				, transmittance
				, lightVector
				, lightFarPlane
				, lightAttenuation );
		}

		void SssTransmittance::doDeclareComputeTransmittance()
		{
			m_compute = m_writer.implementFunction< Vec3 >( cuT( "sssTransmittance" )
				, [this]( Float const & shadowDepth
					, Int const & transmittanceProfileSize
					, Array< Vec4 > const & transmittanceProfile
					, Float const & sssWidth
					, Vec3 const & lightSpacePosition
					, Vec3 const & worldNormal
					, Float const & translucency
					, Vec3 const & lightVector
					, Float const & lightFarPlane
					, Vec3 const & lightAttenuation )
				{
					auto factor = m_writer.declLocale( cuT( "factor" )
						, vec3( 0.0_f ) );

					if ( m_shadowMap )
					{
						IF( m_writer, transmittanceProfileSize > 0_i )
						{
							/**
							 * Calculate the scale of the effect.
							 */
							auto scale = m_writer.declLocale( cuT( "scale" )
								, 8.25_f * translucency / sssWidth );

							/**
							 * Now we calculate the thickness from the light point of view:
							 */
							auto d = m_writer.declLocale( cuT( "d" )
								, glsl::abs( shadowDepth - lightSpacePosition.z() ) * lightFarPlane );

#if C3D_DEBUG_SSS_TRANSMITTANCE

							auto dd = m_writer.declLocale( cuT( "dd" )
								, -d * d );

							FOR( m_writer, Int, i, 0, "i < transmittanceProfileSize", "++i" )
							{
								auto profileFactor = m_writer.declLocale( cuT( "profileFactor" )
									, transmittanceProfile[i] );
								factor += profileFactor.rgb() * exp( dd / profileFactor.a() );
							}
							ROF;

							factor = factor
								* scale
								* clamp( 0.3_f + dot( lightVector, -worldNormal )
									, 0.0_f
									, 1.0_f );
							//factor = vec3( lightSpacePosition.z() );

#else

							/**
							 * Armed with the thickness, we can now calculate the color by means of the
							 * transmittance profile.
							 */
							auto dd = m_writer.declLocale( cuT( "dd" )
								, -d * d );

							FOR( m_writer, Int, i, 0, "i < transmittanceProfileSize", "++i" )
							{
								auto profileFactor = m_writer.declLocale( cuT( "profileFactor" )
									, transmittanceProfile[i] );
								factor += profileFactor.rgb() * exp( dd / profileFactor.a() );
							}
							ROF;
							/**
							 * Using the profile, we finally approximate the transmitted lighting from
							 * the back of the object:
							 */
							factor = factor
								* scale
								* clamp( 0.3_f + dot( lightVector, -worldNormal )
									, 0.0_f
									, 1.0_f );

#endif
						}
						FI;
					}

					m_writer.returnStmt( factor );
				}
				, InFloat{ &m_writer, cuT( "shadowDepth" ) }
				, InInt{ &m_writer, cuT( "transmittanceProfileSize" ) }
				, InArrayParam< Vec4 >{ &m_writer, cuT( "transmittanceProfile" ), 10u }
				, InFloat{ &m_writer, cuT( "sssWidth" ) }
				, InVec3{ &m_writer, cuT( "lightSpacePosition" ) }
				, InVec3{ &m_writer, cuT( "worldNormal" ) }
				, InFloat{ &m_writer, cuT( "transmittance" ) }
				, InVec3{ &m_writer, cuT( "lightVector" ) }
				, InFloat{ &m_writer, cuT( "lightFarPlane" ) }
				, InVec3{ &m_writer, cuT( "lightAttenuation" ) } );
		}
	}
}
