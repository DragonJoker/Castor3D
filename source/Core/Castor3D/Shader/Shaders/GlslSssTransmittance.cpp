#include "Castor3D/Shader/Shaders/GlslSssTransmittance.hpp"

#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslShadow.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d
{
	namespace shader
	{
		SssTransmittance::SssTransmittance( sdw::ShaderWriter & writer
			, Shadow & shadow
			, Utils & utils
			, ShadowOptions shadowOptions
			, SssProfiles const & sssProfiles )
			: m_writer{ writer }
			, m_shadow{ shadow }
			, m_utils{ utils }
			, m_sssProfiles{ sssProfiles }
			, m_shadowMap{ shadowOptions.type != SceneFlag::eNone }
		{
		}

		void SssTransmittance::declare()
		{
			m_compute = m_writer.implementFunction< sdw::Vec3 >( "computeSssTransmittance"
				, [this]( sdw::Float const & lightSpaceDepth
					, sdw::Float const & shadowDepth
					, sdw::UInt const & sssProfileIndex
					, sdw::Vec3 const & worldNormal
					, sdw::Float const & translucency
					, sdw::Vec3 const & lightVector
					, sdw::Float const & lightFarPlane )
				{
					auto sssProfile = m_writer.declLocale( "sssProfile"
						, m_sssProfiles.getProfile( m_writer.cast< sdw::UInt >( sssProfileIndex ) ) );
					auto factor = m_writer.declLocale( "factor"
						, vec3( 0.0_f ) );

					/**
					* Calculate the scale of the effect.
					*/
					auto scale = m_writer.declLocale( "scale"
						, 8.25_f * translucency / sssProfile.gaussianWidth );

					/**
					* Now we calculate the thickness from the light point of view:
					*/
					auto d = m_writer.declLocale( "d"
						, scale * sdw::abs( ( shadowDepth * lightFarPlane ) - lightSpaceDepth ) );

					/**
					* Armed with the thickness, we can now calculate the color by means of the
					* transmittance profile.
					*/
					auto dd = m_writer.declLocale( "dd"
						, -d * d );
					auto profile = m_writer.declLocale( "profile"
						, vec3( 0.0_f ) );

					FOR( m_writer, sdw::Int, i, 0, i < sssProfile.transmittanceProfileSize, ++i )
					{
						auto profileFactors = m_writer.declLocale( "profileFactors"
							, sssProfile.transmittanceProfile[i] );
						profile += profileFactors.rgb() * exp( dd / profileFactors.a() );
					}
					ROF;
					/**
						* Using the profile, we finally approximate the transmitted lighting from
						* the back of the object:
						*/
					factor = profile
						* translucency
						* clamp( 0.3_f + dot( lightVector, -worldNormal )
							, 0.0_f
							, 1.0_f );
					m_writer.returnStmt( factor );
				}
				, sdw::InFloat{ m_writer, "lightSpaceDepth" }
				, sdw::InFloat{ m_writer, "shadowDepth" }
				, sdw::InUInt{ m_writer, "sssProfileIndex" }
				, sdw::InVec3{ m_writer, "worldNormal" }
				, sdw::InFloat{ m_writer, "transmittance" }
				, sdw::InVec3{ m_writer, "lightVector" }
				, sdw::InFloat{ m_writer, "lightFarPlane" } );
		}
		
		sdw::Vec3 SssTransmittance::compute( LightMaterial const & material
			, DirectionalLight const & light
			, Surface const & surface )
		{
			auto ssstResult = m_writer.declLocale( "ssstResult"
				, vec3( 0.0_f ) );

			if ( m_shadowMap )
			{
				declare();

				IF( m_writer
					, material.sssProfileIndex != 0.0_f )
				{
					auto c3d_mapNormalDepthDirectional = m_writer.getVariable< sdw::CombinedImage2DArrayRgba32 >( Shadow::MapNormalDepthDirectional );

					// We shrink the position inwards the surface to avoid artifacts.
					auto shrinkedPos = m_writer.declLocale( "shrinkedPos"
						, surface.worldPosition - surface.worldNormal * 0.005_f );

					auto lightSpacePosition = m_writer.declLocale( "lightSpacePosition"
						, m_shadow.getLightSpacePosition( light.m_transforms[0_u], shrinkedPos ) );
					lightSpacePosition.xy() /= lightSpacePosition.w();
					lightSpacePosition.xy() = sdw::fma( lightSpacePosition.xy()
						, vec2( 0.5_f )
						, vec2( 0.5_f ) );
					auto shadowDepth = m_writer.declLocale( "shadowDepth"
						, c3d_mapNormalDepthDirectional.sample( vec3( lightSpacePosition.xy(), 0.0_f ) ).w() );
					ssstResult = m_compute( lightSpacePosition.z()
						, shadowDepth
						, m_writer.cast< sdw::UInt >( material.sssProfileIndex )
						, surface.worldNormal
						, material.sssTransmittance
						, light.m_direction
						, light.m_lightBase.m_farPlane );
				}
				FI;
			}

			return ssstResult;
		}

		sdw::Vec3 SssTransmittance::compute( LightMaterial const & material
			, PointLight const & light
			, Surface const & surface )
		{
			auto ssstResult = m_writer.declLocale( "ssstResult"
				, vec3( 0.0_f ) );

			if ( m_shadowMap )
			{
				declare();
				
				IF( m_writer
					, material.sssProfileIndex != 0.0_f )
				{
					auto c3d_mapNormalDepthPoint = m_writer.getVariable< sdw::CombinedImageCubeArrayRgba32 >( Shadow::MapNormalDepthPoint );

					// We shrink the position inwards the surface to avoid artifacts.
					auto shrinkedPos = m_writer.declLocale( "shrinkedPos"
						, surface.worldPosition - surface.worldNormal * 0.005_f );

					auto vertexToLight = m_writer.declLocale( "vertexToLight"
						, shrinkedPos - light.m_position );
					auto shadowDepth = m_writer.declLocale( "shadowDepth"
						, c3d_mapNormalDepthPoint.sample( vec4( vertexToLight, m_writer.cast< sdw::Float >( light.m_lightBase.m_index ) ) ).w() );
					ssstResult = m_compute( ( shrinkedPos - light.m_position ).z()
						, shadowDepth
						, m_writer.cast< sdw::UInt >( material.sssProfileIndex )
						, surface.worldNormal
						, material.sssTransmittance
						, -vertexToLight
						, light.m_lightBase.m_farPlane );
				}
				FI;
			}

			return ssstResult;
		}

		sdw::Vec3 SssTransmittance::compute( LightMaterial const & material
			, SpotLight const & light
			, Surface const & surface )
		{
			auto ssstResult = m_writer.declLocale( "ssstResult"
				, vec3( 0.0_f ) );

			if ( m_shadowMap )
			{
				declare();
				
				IF( m_writer
					, material.sssProfileIndex != 0.0_f )
				{
					auto c3d_mapNormalDepthSpot = m_writer.getVariable< sdw::CombinedImage2DArrayRgba32 >( Shadow::MapNormalDepthSpot );

					// We shrink the position inwards the surface to avoid artifacts.
					auto shrinkedPos = m_writer.declLocale( "shrinkedPos"
						, surface.worldPosition - surface.worldNormal * 0.005_f );

					auto lightSpacePosition = m_writer.declLocale( "lightSpacePosition"
						, light.m_transform * vec4( shrinkedPos, 1.0_f ) );
					lightSpacePosition.xy() /= lightSpacePosition.w();
					lightSpacePosition.xy() = sdw::fma( lightSpacePosition.xy()
						, vec2( 0.5_f )
						, vec2( 0.5_f ) );
					auto shadowDepth = m_writer.declLocale( "shadowDepth"
						, c3d_mapNormalDepthSpot.sample( vec3( lightSpacePosition.xy()
							, m_writer.cast< sdw::Float >( light.m_lightBase.m_index ) ) ).w() );
					ssstResult = m_compute( lightSpacePosition.z()
						, shadowDepth
						, m_writer.cast< sdw::UInt >( material.sssProfileIndex )
						, surface.worldNormal
						, material.sssTransmittance
						, light.m_position - surface.worldPosition
						, light.m_lightBase.m_farPlane );
				}
				FI;
			}

			return ssstResult;
		}
	}
}
