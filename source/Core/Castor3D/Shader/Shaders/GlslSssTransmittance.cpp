#include "Castor3D/Shader/Shaders/GlslSssTransmittance.hpp"

#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslPhongLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMetallicBrdfLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslSpecularBrdfLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslShadow.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <ShaderWriter/Source.hpp>

using namespace castor;
using namespace sdw;

namespace castor3d
{
	namespace shader
	{
		SssTransmittance::SssTransmittance( ShaderWriter & writer
			, Shadow const & shadow
			, Utils const & utils
			, bool shadowMap )
			: m_writer{ writer }
			, m_shadow{ shadow }
			, m_utils{ utils }
			, m_shadowMap{ shadowMap }
		{
		}

		void SssTransmittance::declare( LightType type )
		{
			m_compute = m_writer.implementFunction< Vec3 >( "computeSssTransmittance"
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
					auto factor = m_writer.declLocale( "factor"
						, vec3( 0.0_f ) );

					if ( m_shadowMap )
					{
						IF( m_writer, transmittanceProfileSize > 0_i )
						{
							/**
							 * Calculate the scale of the effect.
							 */
							auto scale = m_writer.declLocale( "scale"
								, 8.25_f * translucency / sssWidth );

							/**
							 * Now we calculate the thickness from the light point of view:
							 */
							auto d = m_writer.declLocale( "d"
								, sdw::abs( shadowDepth - lightSpacePosition.z() ) * lightFarPlane );

#if C3D_DebugSSSTransmittance

							auto dd = m_writer.declLocale( "dd"
								, -d * d );

							FOR( m_writer, Int, i, 0, i < transmittanceProfileSize, ++i )
							{
								auto profileFactor = m_writer.declLocale( "profileFactor"
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
							auto dd = m_writer.declLocale( "dd"
								, -d * d );

							FOR( m_writer, Int, i, 0, i < transmittanceProfileSize, ++i )
							{
								auto profileFactor = m_writer.declLocale( "profileFactor"
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
				, InFloat{ m_writer, "shadowDepth" }
				, InInt{ m_writer, "transmittanceProfileSize" }
				, InVec4Array{ m_writer, "transmittanceProfile", 10u }
				, InFloat{ m_writer, "sssWidth" }
				, InVec3{ m_writer, "lightSpacePosition" }
				, InVec3{ m_writer, "worldNormal" }
				, InFloat{ m_writer, "transmittance" }
				, InVec3{ m_writer, "lightVector" }
				, InFloat{ m_writer, "lightFarPlane" }
				, InVec3{ m_writer, "lightAttenuation" } );
		}
		
		Vec3 SssTransmittance::compute( BaseMaterial const & material
			, DirectionalLight const & light
			, Vec2 const & uv
			, Vec3 const & position
			, Vec3 const & normal
			, Float const & translucency )const
		{
			auto ssstResult = m_writer.declLocale( "ssstResult"
				, vec3( 0.0_f ) );

			if ( m_shadowMap )
			{
				IF( m_writer, material.m_subsurfaceScatteringEnabled != 0_i )
				{
					auto c3d_mapDepthDirectional = m_writer.getVariable< SampledImage2DRgba32 >( Shadow::MapDepthNormalDirectional );
					/**
					* First we shrink the position inwards the surface to avoid artifacts:
					* (Note that this can be done once for all the lights)
					*/
					auto shrinkedPos = m_writer.declLocale( "shrinkedPos"
						, position - normal * 0.005_f );
					auto lightSpacePosition = m_writer.declLocale( "lightSpacePosition"
						, m_shadow.getLightSpacePosition( light.m_transforms[0_u]
							, shrinkedPos ) );
					auto shadowDepth = m_writer.declLocale( "d1"
						, texture( c3d_mapDepthDirectional, lightSpacePosition.xy() ).r() );
					ssstResult = m_compute( shadowDepth
						, material.m_transmittanceProfileSize
						, material.m_transmittanceProfile
						, material.m_gaussianWidth
						, lightSpacePosition.xyz()
						, normal
						, translucency
						, light.m_direction
						, light.m_lightBase.m_farPlane
						, vec3( 1.0_f, 0.0_f, 0.0_f ) );
				}
				FI;
			}

			return ssstResult;
		}

		Vec3 SssTransmittance::compute( BaseMaterial const & material
			, PointLight const & light
			, Vec2 const & uv
			, Vec3 const & position
			, Vec3 const & normal
			, Float const & translucency )const
		{
			auto ssstResult = m_writer.declLocale( "ssstResult"
				, vec3( 0.0_f ) );

			if ( m_shadowMap )
			{
				IF( m_writer, material.m_subsurfaceScatteringEnabled != 0_i )
				{
					auto c3d_mtxInvViewProj = m_writer.getVariable< Mat4 >( "c3d_mtxInvViewProj" );
					auto c3d_mapDepthPoint = m_writer.getVariable< SampledImageCubeRgba32 >( Shadow::MapDepthNormalPoint );
					/**
					* First we shrink the position inwards the surface to avoid artifacts:
					* (Note that this can be done once for all the lights)
					*/
					auto shrinkedPos = m_writer.declLocale( "shrinkedPos"
						, position - normal * 0.005_f );

					auto vertexToLight = m_writer.declLocale( "vertexToLight"
						, shrinkedPos - light.m_position );
					auto direction = m_writer.declLocale( "direction"
						, vertexToLight );
					auto shadowDepth = m_writer.declLocale( "shadowDepth"
						, texture( c3d_mapDepthPoint, direction ).r() );
					auto lightSpacePosition = m_writer.declLocale( "lightSpacePosition"
						, m_utils.calcWSPosition( uv
							, shadowDepth
							, c3d_mtxInvViewProj ) );
					ssstResult = m_compute( shadowDepth
						, material.m_transmittanceProfileSize
						, material.m_transmittanceProfile
						, material.m_gaussianWidth
						, lightSpacePosition
						, normal
						, translucency
						, normalize( -vertexToLight )
						, light.m_lightBase.m_farPlane
						, light.m_attenuation );
				}
				FI;
			}

			return ssstResult;
		}

		Vec3 SssTransmittance::compute( BaseMaterial const & material
			, SpotLight const & light
			, Vec2 const & uv
			, Vec3 const & position
			, Vec3 const & normal
			, Float const & translucency )const
		{
			auto ssstResult = m_writer.declLocale( "ssstResult"
				, vec3( 0.0_f ) );

			if ( m_shadowMap )
			{
				IF( m_writer, material.m_subsurfaceScatteringEnabled != 0_i )
				{
					auto c3d_mapDepthSpot = m_writer.getVariable< SampledImage2DRgba32 >( Shadow::MapShadowSpot );
					// We shrink the position inwards the surface to avoid artifacts.
					auto shrinkedPos = m_writer.declLocale( "shrinkedPos"
						, position - normal * 0.005_f );
					auto lightSpacePosition = m_writer.declLocale( "lightSpacePosition"
						, m_shadow.getLightSpacePosition( light.m_transform
							, shrinkedPos ) );
					auto shadowDepth = m_writer.declLocale( "shadowDepth"
						, texture( c3d_mapDepthSpot, lightSpacePosition.xy() ).r() );
					ssstResult = m_compute( shadowDepth
						, material.m_transmittanceProfileSize
						, material.m_transmittanceProfile
						, material.m_gaussianWidth
						, lightSpacePosition.xyz()
						, normal
						, translucency
						, normalize( light.m_position - position )
						, light.m_lightBase.m_farPlane
						, light.m_attenuation );
				}
				FI;
			}

			return ssstResult;
		}
	}
}
