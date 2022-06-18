#include "AtmosphereScattering/Scattering.hpp"

#include <Castor3D/Miscellaneous/Logger.hpp>

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
		, bool renderSunDisk )
		: m_writer{ writer }
		, m_atmosphereConfig{ atmosphereConfig }
		, m_cameraData{ cameraData }
		, m_atmosphereData{ atmosphereData }
		, m_colorTransmittance{ colorTransmittance }
		, m_fastSky{ fastSky }
		, m_fastAerialPerspective{ fastAerialPerspective }
		, m_renderSunDisk{ renderSunDisk }
	{
	}

	sdw::Vec3 ScatteringConfig::getSunLuminance( sdw::Vec3 const & pworldPos
		, sdw::Vec3 const & pworldDir
		, sdw::CombinedImage2DRgba32 const & ptransmittanceMap )
	{
		if ( !m_getPixelTransLum )
		{
			m_getValFromTLUT = m_writer.implementFunction< sdw::Vec3 >( "getValFromTLUT"
				, [&]( sdw::Vec3 const & pos
					, sdw::Vec3 const & sunDir
					, sdw::CombinedImage2DRgba32 const & transmittanceMap )
				{
					auto height = m_writer.declLocale( "height"
						, length( pos ) );
					auto up = m_writer.declLocale( "up"
						, pos / height );
					auto sunCosZenithAngle = m_writer.declLocale( "sunCosZenithAngle"
						, dot( sunDir, up ) );
					auto uv = m_writer.declLocale( "uv"
						, vec2( clamp( 0.5_f + 0.5_f * sunCosZenithAngle, 0.0_f, 1.0_f )
							, max( 0.0_f, min( 1.0_f, ( height - m_atmosphereData.bottomRadius ) / ( m_atmosphereData.topRadius - m_atmosphereData.bottomRadius ) ) ) ) );
					m_writer.returnStmt( transmittanceMap.lod( uv, 0.0_f ).rgb() );
				}
				, sdw::InVec3{ m_writer, "pos" }
				, sdw::InVec3{ m_writer, "sunDir" }
				, sdw::InCombinedImage2DRgba32{ m_writer, "transmittanceMap" } );

			m_getSunLuminance = m_writer.implementFunction< sdw::Vec3 >( "getSunLuminance"
				, [&]( sdw::Vec3 const & worldPos
					, sdw::Vec3 const & worldDir
					, sdw::CombinedImage2DRgba32 const & transmittanceMap )
				{
					if ( m_renderSunDisk )
					{
						auto sunSolidAngle = m_writer.declLocale( "sunSolidAngle"
							, 0.53_f * sdw::Float{ castor::Pi< float > } / 180.0_f );
						auto minSunCosTheta = m_writer.declLocale( "minSunCosTheta"
							, cos( sunSolidAngle ) );
						auto cosTheta = m_writer.declLocale( "cosTheta"
							, dot( worldDir, m_atmosphereData.sunDirection ) );
						auto sunLuminance = m_writer.declLocale( "sunLuminance"
							, vec3( 1000000.0_f ) ); // arbitrary. But fine, not use when comparing the models

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
								sunLuminance *= m_getValFromTLUT( worldPos, m_atmosphereData.sunDirection, transmittanceMap );
							}
							FI;
						}
						FI;

						m_writer.returnStmt( sunLuminance );
					}

					m_writer.returnStmt( vec3( 0.0_f ) );
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
		, sdw::CombinedImage3DRgba32 const & pvolumMap
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
					auto sampleCountIni = 0.0_f;	// Can go a low as 10 sample but energy lost starts to be visible.
					auto planetRadiusOffset = 0.01_f;
					auto apSliceCount = 32.0_f;

					auto clipSpace = m_writer.declLocale( "clipSpace"
						, vec3( ( fragPos / fragSize ) * vec2( 2.0_f, 2.0_f ) - vec2( 1.0_f, 1.0_f ), 1.0_f ) );

					auto hPos = m_writer.declLocale( "hPos"
						, m_cameraData.camProjToWorld( vec4( clipSpace, 1.0_f ) ) );

					auto worldDir = m_writer.declLocale( "worldDir"
						, normalize( hPos.xyz() / hPos.w() - m_cameraData.position ) );
					auto worldPos = m_writer.declLocale( "worldPos"
						, m_cameraData.position + vec3( 0.0_f, m_atmosphereData.bottomRadius, 0.0_f ) );

					auto viewHeight = m_writer.declLocale( "viewHeight"
						, length( worldPos ) );
					auto L = m_writer.declLocale( "L"
						, vec3( 0.0_f ) );

					if ( m_fastSky )
					{
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

							luminance = vec4( skyViewMap.lod( uv, 0.0_f ).rgb() + getSunLuminance( worldPos, worldDir, transmittanceMap ), 1.0_f );
							m_writer.returnStmt();
						}
						FI;
					}
					else
					{
						IF( m_writer, fragDepth >= 1.0_f )
						{
							L += getSunLuminance( worldPos, worldDir, transmittanceMap );
						}
						FI;
					}

					if ( m_fastAerialPerspective )
					{
						if ( m_colorTransmittance )
						{
							castor3d::log::error << "The fastAerialPerspective path does not support colorTransmittance." << std::endl;
						}
						else
						{
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
							L.rgb() += AP.rgb();
							auto opacity = m_writer.declLocale( "opacity"
								, AP.a() );

							luminance = vec4( L, opacity );
						}
					}
					else
					{
						// Move to top atmosphere as the starting point for ray marching.
						// This is critical to be after the above to not disrupt above atmosphere tests and voxel selection.
						IF( m_writer, !m_atmosphereConfig.moveToTopAtmosphere( worldPos, worldDir ) )
						{
							// Ray is not intersecting the atmosphere		
							luminance = vec4( getSunLuminance( worldPos, worldDir, transmittanceMap ), 1.0_f );
							m_writer.returnStmt();
						}
						FI;

						SingleScatteringResult ss = m_writer.declLocale( "ss"
							, m_atmosphereConfig.integrateScatteredLuminance( fragPos
								, worldPos
								, worldDir
								, m_atmosphereData.sunDirection
								, sampleCountIni
								, fragDepth ) );

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
			, pvolumMap
			, ptransmittance
			, pluminance );
	}
}
