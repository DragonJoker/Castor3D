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
		
		sdw::Vec3 SssTransmittance::compute( LightMaterial const & material
			, DirectionalLight const & plight
			, Surface const & psurface )
		{
			if ( !m_shadowMap )
			{
				return vec3( 0.0_f );
			}

			if ( !m_computeDirectional )
			{
				doDeclare();
				m_computeDirectional = m_writer.implementFunction< sdw::Vec3 >( "SDW_computeSssTransmittanceDirectional"
					, [&]( sdw::UInt const & sssProfileIndex
						, sdw::Float const & sssTransmittance
						, DirectionalLight const & light
						, Surface const & surface )
					{
						auto result = m_writer.declLocale( "result"
							, vec3( 0.0_f ) );

						IF( m_writer
							, material.sssProfileIndex != 0.0_f )
						{
							auto c3d_mapNormalDepthDirectional = m_writer.getVariable< sdw::CombinedImage2DArrayRgba32 >( Shadow::MapNormalDepthDirectional );

							// We shrink the position inwards the surface to avoid artifacts.
							auto shrinkedPos = m_writer.declLocale( "shrinkedPos"
								, vec4( surface.worldPosition - surface.worldNormal * 0.005_f, 1.0_f ) );

							auto lightSpacePosition = m_writer.declLocale( "lightSpacePosition"
								, light.transforms[0] * shrinkedPos );
							lightSpacePosition.xy() /= lightSpacePosition.w();
							lightSpacePosition.xy() = sdw::fma( lightSpacePosition.xy()
								, vec2( 0.5_f )
								, vec2( 0.5_f ) );
							auto shadowDepth = m_writer.declLocale( "shadowDepth"
								, c3d_mapNormalDepthDirectional.sample( vec3( lightSpacePosition.xy(), 0.0_f ) ).w() );
							result = m_compute( lightSpacePosition.z()
								, shadowDepth
								, sssProfileIndex
								, surface.worldNormal
								, sssTransmittance
								, light.direction
								, light.base.farPlane );
						}
						FI;

						m_writer.returnStmt( result );
					}
					, sdw::InUInt{ m_writer, "sssProfileIndex" }
					, sdw::InFloat{ m_writer, "sssTransmittance" }
					, InDirectionalLight{ m_writer, "light" }
					, InSurface{ m_writer, "surface" } );
			}

			return m_computeDirectional( m_writer.cast< sdw::UInt >( material.sssProfileIndex )
				, material.sssTransmittance
				, plight
				, psurface );
		}

		sdw::Vec3 SssTransmittance::compute( LightMaterial const & material
			, PointLight const & plight
			, Surface const & psurface )
		{
			if ( !m_shadowMap )
			{
				return vec3( 0.0_f );
			}

			if ( !m_computePoint )
			{
				doDeclare();
				m_computePoint = m_writer.implementFunction< sdw::Vec3 >( "SDW_computeSssTransmittancePoint"
					, [&]( sdw::UInt const & sssProfileIndex
						, sdw::Float const & sssTransmittance
						, PointLight const & light
						, Surface const & surface )
					{
						auto result = m_writer.declLocale( "result"
							, vec3( 0.0_f ) );
				
						IF( m_writer
							, material.sssProfileIndex != 0.0_f )
						{
							auto c3d_mapNormalDepthPoint = m_writer.getVariable< sdw::CombinedImageCubeArrayRgba32 >( Shadow::MapNormalDepthPoint );

							// We shrink the position inwards the surface to avoid artifacts.
							auto shrinkedPos = m_writer.declLocale( "shrinkedPos"
								, surface.worldPosition - surface.worldNormal * 0.005_f );

							auto vertexToLight = m_writer.declLocale( "vertexToLight"
								, shrinkedPos - light.position );
							auto shadowDepth = m_writer.declLocale( "shadowDepth"
								, c3d_mapNormalDepthPoint.sample( vec4( vertexToLight, m_writer.cast< sdw::Float >( light.base.index ) ) ).w() );
							result = m_compute( ( shrinkedPos - light.position ).z()
								, shadowDepth
								, sssProfileIndex
								, surface.worldNormal
								, sssTransmittance
								, -vertexToLight
								, light.base.farPlane );
						}
						FI;

						m_writer.returnStmt( result );
					}
					, sdw::InUInt{ m_writer, "sssProfileIndex" }
					, sdw::InFloat{ m_writer, "sssTransmittance" }
					, InPointLight{ m_writer, "light" }
					, InSurface{ m_writer, "surface" } );
			}

			return m_computePoint( m_writer.cast< sdw::UInt >( material.sssProfileIndex )
				, material.sssTransmittance
				, plight
				, psurface );
		}

		sdw::Vec3 SssTransmittance::compute( LightMaterial const & material
			, SpotLight const & plight
			, Surface const & psurface )
		{
			if ( !m_shadowMap )
			{
				return vec3( 0.0_f );
			}

			if ( !m_computeSpot )
			{
				doDeclare();
				m_computeSpot = m_writer.implementFunction< sdw::Vec3 >( "SDW_computeSssTransmittanceSpot"
					, [&]( sdw::UInt const & sssProfileIndex
						, sdw::Float const & sssTransmittance
						, SpotLight const & light
						, Surface const & surface )
					{
						auto result = m_writer.declLocale( "result"
							, vec3( 0.0_f ) );

						IF( m_writer
							, sssProfileIndex != 0_u )
						{
							auto c3d_mapNormalDepthSpot = m_writer.getVariable< sdw::CombinedImage2DArrayRgba32 >( Shadow::MapNormalDepthSpot );

							// We shrink the position inwards the surface to avoid artifacts.
							auto shrinkedPos = m_writer.declLocale( "shrinkedPos"
								, vec4( surface.worldPosition - surface.worldNormal * 0.005_f, 1.0_f ) );

							auto lightSpacePosition = m_writer.declLocale( "lightSpacePosition"
								, light.transform * shrinkedPos );
							lightSpacePosition.xy() /= lightSpacePosition.w();
							lightSpacePosition.xy() = sdw::fma( lightSpacePosition.xy()
								, vec2( 0.5_f )
								, vec2( 0.5_f ) );
							auto shadowDepth = m_writer.declLocale( "shadowDepth"
								, c3d_mapNormalDepthSpot.sample( vec3( lightSpacePosition.xy()
									, m_writer.cast< sdw::Float >( light.base.index ) ) ).w() );
							result = m_compute( lightSpacePosition.z()
								, shadowDepth
								, sssProfileIndex
								, surface.worldNormal
								, sssTransmittance
								, light.position - surface.worldPosition
								, light.base.farPlane );
							result = vec3( shrinkedPos );
						}
						FI;

						m_writer.returnStmt( result );
					}
					, sdw::InUInt{ m_writer, "sssProfileIndex" }
					, sdw::InFloat{ m_writer, "sssTransmittance" }
					, InSpotLight{ m_writer, "light" }
					, InSurface{ m_writer, "surface" } );
			}

			return m_computeSpot( m_writer.cast< sdw::UInt >( material.sssProfileIndex )
				, material.sssTransmittance
				, plight
				, psurface );
		}

		void SssTransmittance::doDeclare()
		{
			if ( !m_compute )
			{
				m_compute = m_writer.implementFunction< sdw::Vec3 >( "SDW_computeSssTransmittance"
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
		}
	}
}
