#include "AtmosphereScattering/Scattering.hpp"

#include "AtmosphereScattering/AtmosphereWeatherUbo.hpp"

#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Shader/Shaders/GlslLight.hpp>

#include <ShaderWriter/Source.hpp>

namespace atmosphere_scattering
{
	ScatteringConfig::ScatteringConfig( sdw::ShaderWriter & writer
		, AtmosphereConfig & atmosphereConfig
		, CameraData const & cameraData
		, AtmosphereData const & atmosphereData
		, bool colorTransmittance
		, bool fastSky
		, bool fastAerialPerspective
		, bool renderSunDisk
		, bool bloomSunDisk )
		: m_writer{ writer }
		, m_atmosphereConfig{ atmosphereConfig }
		, m_cameraData{ cameraData }
		, m_atmosphereData{ atmosphereData }
		, m_colorTransmittance{ colorTransmittance }
		, m_fastSky{ fastSky }
		, m_fastAerialPerspective{ fastAerialPerspective }
		, m_renderSunDisk{ renderSunDisk }
		, m_bloomSunDisk{ bloomSunDisk }
	{
	}

	ScatteringConfig::ScatteringConfig( sdw::ShaderWriter & writer
		, AtmosphereConfig & atmosphereConfig
		, CameraData const & cameraData
		, AtmosphereData const & atmosphereData
		, WeatherData const & weatherData
		, bool colorTransmittance
		, bool fastSky
		, bool fastAerialPerspective
		, bool renderSunDisk
		, bool bloomSunDisk )
		: m_writer{ writer }
		, m_atmosphereConfig{ atmosphereConfig }
		, m_cameraData{ cameraData }
		, m_atmosphereData{ atmosphereData }
		, m_weatherData{ &weatherData }
		, m_colorTransmittance{ colorTransmittance }
		, m_fastSky{ fastSky }
		, m_fastAerialPerspective{ fastAerialPerspective }
		, m_renderSunDisk{ renderSunDisk }
		, m_bloomSunDisk{ bloomSunDisk }
	{
	}

	sdw::Vec3 ScatteringConfig::getSunLuminance( sdw::Vec3 const & pworldPos
		, sdw::Vec3 const & pworldDir
		, sdw::CombinedImage2DRgba32 const & ptransmittanceMap )
	{
		if ( !m_getSunLuminance )
		{
			m_getSunLuminance = m_writer.implementFunction< sdw::Vec3 >( "getSunLuminance"
				, [&]( sdw::Vec3 const & worldPos
					, sdw::Vec3 const & worldDir
					, sdw::CombinedImage2DRgba32 const & transmittanceMap )
				{
					if ( m_bloomSunDisk )
					{
						auto sunSolidAngle = m_writer.declLocale( "sunSolidAngle"
							, 0.053_f * sdw::Float{ castor::Pi< float > } / 180.0_f );
						auto minSunCosTheta = m_writer.declLocale( "minSunCosTheta"
							, cos( sunSolidAngle ) );
						auto cosTheta = m_writer.declLocale( "cosTheta"
							, dot( worldDir, m_atmosphereData.sunDirection ) );
						auto sunLuminance = m_writer.declLocale( "sunLuminance"
							, vec3( 1.0_f ) );

						IF( m_writer, cosTheta < minSunCosTheta )
						{
							auto offset = m_writer.declLocale( "offset"
								, minSunCosTheta - cosTheta );
							auto gaussianBloom = m_writer.declLocale( "gaussianBloom"
								, exp( -offset * 50000.0_f ) * 0.5_f );
							auto invBloom = m_writer.declLocale( "invBloom"
								, 1.0_f / ( 0.02_f + offset * 300.0_f ) * 0.01_f );
							sunLuminance = vec3( gaussianBloom + invBloom );
						}
						FI;

						// Use smoothstep to limit the effect, so it drops off to actual zero.
						sunLuminance = smoothStep( vec3( 0.002_f ), vec3( 1.0_f ), sunLuminance );

						IF( m_writer, length( sunLuminance ) > 0.0_f )
						{
							IF( m_writer, m_atmosphereConfig.raySphereIntersectNearest( worldPos, worldDir, vec3( 0.0_f ), m_atmosphereData.bottomRadius ) >= 0.0_f )
							{
								sunLuminance *= vec3( 0.0_f );
							}
							ELSE
							{
								// If the sun value is applied to this pixel, we need to calculate the transmittance to obscure it.
								sunLuminance *= vec3( 2.0_f ) * m_atmosphereConfig.getSunRadiance( m_cameraData.position()
									, m_atmosphereData.sunDirection
									, transmittanceMap );
							}
							FI;
						}
						FI;

						m_writer.returnStmt( sunLuminance );
					}
					else
					{
						auto sunSolidAngle = m_writer.declLocale( "sunSolidAngle"
							, 0.5_f * 0.505_f * sdw::Float{ castor::Pi< float > } / 180.0_f );
						auto minSunCosTheta = m_writer.declLocale( "minSunCosTheta"
							, cos( sunSolidAngle ) );
						auto cosTheta = m_writer.declLocale( "cosTheta"
							, dot( worldDir, m_atmosphereData.sunDirection ) );
						auto sunLuminance = m_writer.declLocale( "sunLuminance"
							, vec3( 0.0_f ) );

						IF( m_writer, cosTheta > minSunCosTheta )
						{
							IF( m_writer, m_atmosphereConfig.raySphereIntersectNearest( worldPos, worldDir, vec3( 0.0_f ), m_atmosphereData.bottomRadius ) < 0.0_f ) // no intersection
							{
								sunLuminance = vec3( 2.0_f ) * m_atmosphereConfig.getSunRadiance( m_cameraData.position()
									, m_atmosphereData.sunDirection
									, transmittanceMap );
							}
							FI;
						}
						FI;

						m_writer.returnStmt( sunLuminance );
					}
				}
				, sdw::InVec3{ m_writer, "worldPos" }
				, sdw::InVec3{ m_writer, "worldDir" }
				, sdw::InCombinedImage2DRgba32{ m_writer, "transmittanceMap" } );
		}

		return m_getSunLuminance( pworldPos
			, pworldDir
			, ptransmittanceMap );
	}

	sdw::Float ScatteringConfig::aerialPerspectiveDepthToSlice( sdw::Float const & pdepth )
	{
		if ( !m_aerialPerspectiveDepthToSlice )
		{
			m_aerialPerspectiveDepthToSlice = m_writer.implementFunction< sdw::Float >( "aerialPerspectiveDepthToSlice"
				, [&]( sdw::Float const & depth )
				{
					auto apKmPerSlice = m_writer.declConstant( "apKmPerSlice"
						, 4.0_f );
					m_writer.returnStmt( depth * ( 1.0_f / apKmPerSlice ) );
				}
				, sdw::InFloat{ m_writer, "depth" } );
		}

		return m_aerialPerspectiveDepthToSlice( pdepth );
	}

	sdw::Void ScatteringConfig::getPixelTransLum( sdw::Vec2 const & pfragPos
		, sdw::Vec2 const & pfragSize
		, sdw::Float const & pfragDepth
		, sdw::CombinedImage2DRgba32 const & ptransmittanceMap
		, sdw::CombinedImage2DRgba32 const & pskyViewMap
		, sdw::CombinedImage3DRgba32 const & pvolumeMap
		, castor3d::shader::Light const & plight
		, sdw::Vec3 const & psurfaceWorldNormal
		, sdw::Mat4 const & plightMatrix
		, sdw::UInt const & pcascadeIndex
		, sdw::UInt const & pmaxCascade
		, sdw::Vec4 & ptransmittance
		, sdw::Vec4 & pluminance )
	{
		if ( !m_getPixelTransLumShadow )
		{
			m_getPixelTransLumShadow = m_writer.implementFunction< sdw::Void >( "getPixelTransLumShadow"
				, [&]( sdw::Vec2 const & fragPos
					, sdw::Vec2 const & fragSize
					, sdw::Float const & fragDepth
					, sdw::CombinedImage2DRgba32 const & transmittanceMap
					, sdw::CombinedImage2DRgba32 const & skyViewMap
					, sdw::CombinedImage3DRgba32 const & volumeMap
					, castor3d::shader::Light const & light
					, sdw::Vec3 const & surfaceWorldNormal
					, sdw::Mat4 const & lightMatrix
					, sdw::UInt const & cascadeIndex
					, sdw::UInt const & maxCascade
					, sdw::Vec4 transmittance
					, sdw::Vec4 luminance )
				{
					auto clipSpace = m_writer.declLocale( "clipSpace"
						, m_atmosphereConfig.getClipSpace( fragPos, fragSize, 1.0_f ) );
					auto hPos = m_writer.declLocale( "hPos"
						, m_cameraData.camProjToWorld( vec4( clipSpace, 1.0_f ) ) );
					auto worldDir = m_writer.declLocale( "worldDir"
						, normalize( hPos.xyz() / hPos.w() - m_cameraData.position() ) );
					auto worldPos = m_writer.declLocale( "worldPos"
						, m_cameraData.position() + vec3( 0.0_f, m_atmosphereData.bottomRadius, 0.0_f ) );
					auto L = m_writer.declLocale( "L"
						, vec3( 0.0_f ) );
					doRenderSky( fragSize
						, fragDepth
						, worldPos
						, worldDir
						, transmittanceMap
						, skyViewMap
						, L
						, luminance );

					if ( m_fastAerialPerspective )
					{
						doRenderFastAerial( fragPos
							, fragSize
							, fragDepth
							, worldPos
							, volumeMap
							, L
							, luminance );
					}
					else
					{
						// Move to top atmosphere as the starting point for ray marching.
						// This is critical to be after the above to not disrupt above atmosphere tests and voxel selection.
						IF( m_writer, !m_atmosphereConfig.moveToTopAtmosphere( worldPos, worldDir ) )
						{
							// Ray is not intersecting the atmosphere	
							if ( m_renderSunDisk )
							{
								luminance = vec4( getSunLuminance( worldPos, worldDir, transmittanceMap ), 1.0_f );
							}

							m_writer.returnStmt();
						}
						FI;

						auto sampleCountIni = 0.0_f;
						auto ss = m_writer.declLocale( "ss"
							, m_atmosphereConfig.integrateScatteredLuminance( fragPos
								, worldPos
								, worldDir
								, m_atmosphereData.sunDirection
								, sampleCountIni
								, fragDepth
								, light
								, surfaceWorldNormal
								, lightMatrix
								, cascadeIndex
								, maxCascade ) );
						doRegisterOutputs( ss, L, luminance, transmittance );
					}
				}
				, sdw::InVec2{ m_writer, "fragPos" }
				, sdw::InVec2{ m_writer, "fragSize" }
				, sdw::InFloat{ m_writer, "fragDepth" }
				, sdw::InCombinedImage2DRgba32{ m_writer, "transmittanceMap" }
				, sdw::InCombinedImage2DRgba32{ m_writer, "skyViewMap" }
				, sdw::InCombinedImage3DRgba32{ m_writer, "volumeMap" }
				, castor3d::shader::InLight{ m_writer, "light" }
				, sdw::InVec3{ m_writer, "surfaceWorldNormal" }
				, sdw::InMat4{ m_writer, "lightMatrix" }
				, sdw::InUInt{ m_writer, "cascadeIndex" }
				, sdw::InUInt{ m_writer, "maxCascade" }
				, sdw::OutVec4{ m_writer, "transmittance" }
				, sdw::OutVec4{ m_writer, "luminance" } );
		}

		return m_getPixelTransLumShadow( pfragPos
			, pfragSize
			, pfragDepth
			, ptransmittanceMap
			, pskyViewMap
			, pvolumeMap
			, plight
			, psurfaceWorldNormal
			, plightMatrix
			, pcascadeIndex
			, pmaxCascade
			, ptransmittance
			, pluminance );
	}

	sdw::Void ScatteringConfig::getPixelTransLum( sdw::Vec2 const & pfragPos
		, sdw::Vec2 const & pfragSize
		, sdw::Float const & pfragDepth
		, sdw::CombinedImage2DRgba32 const & ptransmittanceMap
		, sdw::CombinedImage2DRgba32 const & pskyViewMap
		, sdw::CombinedImage3DRgba32 const & pvolumeMap
		, sdw::Vec4 & ptransmittance
		, sdw::Vec4 & pluminance )
	{
		if ( !m_getPixelTransLum )
		{
			m_getPixelTransLum = m_writer.implementFunction< sdw::Void >( "getPixelTransLum"
				, [&]( sdw::Vec2 const & fragPos
					, sdw::Vec2 const & fragSize
					, sdw::Float const & fragDepth
					, sdw::CombinedImage2DRgba32 const & transmittanceMap
					, sdw::CombinedImage2DRgba32 const & skyViewMap
					, sdw::CombinedImage3DRgba32 const & volumeMap
					, sdw::Vec4 transmittance
					, sdw::Vec4 luminance )
				{
					auto clipSpace = m_writer.declLocale( "clipSpace"
						, m_atmosphereConfig.getClipSpace( fragPos, fragSize, 1.0_f ) );
					auto hPos = m_writer.declLocale( "hPos"
						, m_cameraData.camProjToWorld( vec4( clipSpace, 1.0_f ) ) );
					auto worldDir = m_writer.declLocale( "worldDir"
						, normalize( hPos.xyz() / hPos.w() - m_cameraData.position() ) );
					auto worldPos = m_writer.declLocale( "worldPos"
						, m_cameraData.position() + vec3( 0.0_f, m_atmosphereData.bottomRadius, 0.0_f ) );
					auto L = m_writer.declLocale( "L"
						, vec3( 0.0_f ) );
					doRenderSky( fragSize
						, fragDepth
						, worldPos
						, worldDir
						, transmittanceMap
						, skyViewMap
						, L
						, luminance );

					if ( m_fastAerialPerspective )
					{
						doRenderFastAerial( fragPos
							, fragSize
							, fragDepth
							, worldPos
							, volumeMap
							, L
							, luminance );
					}
					else
					{
						// Move to top atmosphere as the starting point for ray marching.
						// This is critical to be after the above to not disrupt above atmosphere tests and voxel selection.
						IF( m_writer, !m_atmosphereConfig.moveToTopAtmosphere( worldPos, worldDir ) )
						{
							// Ray is not intersecting the atmosphere		
							if ( m_renderSunDisk )
							{
								luminance = vec4( getSunLuminance( worldPos, worldDir, transmittanceMap ), 1.0_f );
							}

							m_writer.returnStmt();
						}
						FI;

						auto sampleCountIni = 0.0_f;
						auto ss = m_writer.declLocale( "ss"
							, m_atmosphereConfig.integrateScatteredLuminanceNoShadow( fragPos
								, worldPos
								, worldDir
								, m_atmosphereData.sunDirection
								, sampleCountIni
								, fragDepth ) );
						doRegisterOutputs( ss, L, luminance, transmittance );
					}
				}
				, sdw::InVec2{ m_writer, "fragPos" }
				, sdw::InVec2{ m_writer, "fragSize" }
				, sdw::InFloat{ m_writer, "fragDepth" }
				, sdw::InCombinedImage2DRgba32{ m_writer, "transmittanceMap" }
				, sdw::InCombinedImage2DRgba32{ m_writer, "skyViewMap" }
				, sdw::InCombinedImage3DRgba32{ m_writer, "volumeMap" }
				, sdw::OutVec4{ m_writer, "transmittance" }
				, sdw::OutVec4{ m_writer, "luminance" } );
		}

		return m_getPixelTransLum( pfragPos
			, pfragSize
			, pfragDepth
			, ptransmittanceMap
			, pskyViewMap
			, pvolumeMap
			, ptransmittance
			, pluminance );
	}

	sdw::Vec4 ScatteringConfig::rescaleLuminance( sdw::Vec4 const & luminance )
	{
		luminance.xyz() /= luminance.a();	// Normalise according to sample count when path tracing

		// Similar setup to the Bruneton demo
		auto whitePoint = vec3( 1.08241_f, 0.96756_f, 0.95003_f );
		auto exposure = 10.0_f;
		auto hdr = vec3( 1.0_f ) - exp( -luminance.rgb() / whitePoint * exposure );
		return vec4( hdr, luminance.a() );
	}

	void ScatteringConfig::doRenderSky( sdw::Vec2 const & fragSize
		, sdw::Float const & fragDepth
		, sdw::Vec3 const & worldPos
		, sdw::Vec3 const & worldDir
		, sdw::CombinedImage2DRgba32 const & transmittanceMap
		, sdw::CombinedImage2DRgba32 const & skyViewMap
		, sdw::Vec3 & L
		, sdw::Vec4 & luminance )
	{
		if ( m_fastSky )
		{
			auto viewHeight = m_writer.declLocale( "viewHeight"
				, length( worldPos ) );

			IF( m_writer, viewHeight < m_atmosphereData.topRadius && fragDepth >= 1.0_f )
			{
				auto uv = m_writer.declLocale< sdw::Vec2 >( "uv" );
				auto upVector = m_writer.declLocale( "upVector"
					, normalize( worldPos ) );
				auto viewZenithCosAngle = m_writer.declLocale( "viewZenithCosAngle"
					, dot( worldDir, upVector ) );

				auto sideVector = m_writer.declLocale( "sideVector"
					, normalize( cross( upVector, worldDir ) ) );		// assumes non parallel vectors
				auto forwardVector = m_writer.declLocale( "forwardVector"
					, normalize( cross( sideVector, upVector ) ) );	// aligns toward the sun light but perpendicular to up vector
				auto lightOnPlane = m_writer.declLocale( "lightOnPlane"
					, vec2( dot( m_atmosphereData.sunDirection, forwardVector )
						, dot( m_atmosphereData.sunDirection, sideVector ) ) );
				lightOnPlane = normalize( lightOnPlane );
				auto lightViewCosAngle = m_writer.declLocale( "lightViewCosAngle"
					, lightOnPlane.x() );

				auto intersectGround = m_writer.declLocale( "intersectGround"
					, m_atmosphereConfig.raySphereIntersectNearest( worldPos
						, worldDir
						, vec3( 0.0_f )
						, m_atmosphereData.bottomRadius ) >= 0.0_f );

				m_atmosphereConfig.skyViewLutParamsToUv( intersectGround, viewZenithCosAngle, lightViewCosAngle, viewHeight, uv, fragSize );

				luminance = vec4( skyViewMap.lod( uv, 0.0_f ).rgb(), 1.0_f );

				if ( m_renderSunDisk )
				{
					luminance.xyz() += getSunLuminance( worldPos, worldDir, transmittanceMap );
				}

				m_writer.returnStmt();
			}
			FI;
		}
		else if ( m_renderSunDisk )
		{
			IF( m_writer, fragDepth >= 1.0_f )
			{
				L += getSunLuminance( worldPos, worldDir, transmittanceMap );
			}
			FI;
		}
	}

	void ScatteringConfig::doRenderFastAerial( sdw::Vec2 const & fragPos
		, sdw::Vec2 const & fragSize
		, sdw::Float const & fragDepth
		, sdw::Vec3 const & worldPos
		, sdw::CombinedImage3DRgba32 const & volumeMap
		, sdw::Vec3 & L
		, sdw::Vec4 & luminance )
	{
		if ( m_colorTransmittance )
		{
			castor3d::log::error << "The fastAerialPerspective path does not support colorTransmittance." << std::endl;
		}
		else
		{
			auto apSliceCount = 32.0_f;
			auto depthBufferWorldPos = m_writer.declLocale( "depthBufferWorldPos"
				, m_atmosphereConfig.getWorldPos( fragDepth
					, fragPos
					, fragSize ) );
			auto tDepth = m_writer.declLocale( "tDepth"
				, length( depthBufferWorldPos - ( worldPos + vec3( 0.0_f, -m_atmosphereData.bottomRadius, 0.0_f ) ) ) );
			auto slice = m_writer.declLocale( "slice"
				, aerialPerspectiveDepthToSlice( tDepth ) );
			auto weight = m_writer.declLocale( "weight"
				, 1.0_f );

			IF( m_writer, slice < 0.5_f )
			{
				// We multiply by weight to fade to 0 at depth 0. That works for luminance and opacity.
				weight = clamp( slice * 2.0_f, 0.0_f, 1.0_f );
				slice = 0.5_f;
			}
			FI;

			auto w = m_writer.declLocale( "w"
				, sqrt( slice / apSliceCount ) );	// squared distribution

			auto AP = m_writer.declLocale( "AP"
				, weight * volumeMap.lod( vec3( fragPos / fragSize, w ), 0.0_f ) );
			L += AP.rgb();
			auto opacity = m_writer.declLocale( "opacity"
				, AP.a() );

			luminance = vec4( L, opacity );
		}
	}

	void ScatteringConfig::doRegisterOutputs( SingleScatteringResult const & ss
		, sdw::Vec3 & L
		, sdw::Vec4 & luminance
		, sdw::Vec4 & transmittance )
	{
		L += ss.luminance;
		auto throughput = m_writer.declLocale( "throughput"
			, ss.transmittance );

		if ( m_colorTransmittance )
		{
			luminance = vec4( L, 1.0_f );
			transmittance = vec4( throughput, 1.0_f );
		}
		else
		{
			auto t = m_writer.declLocale( "t"
				, dot( throughput, vec3( 1.0_f / 3.0_f ) ) );
			luminance = vec4( L, 1.0_f - t );
		}
	}
}
