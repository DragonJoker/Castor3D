#include "AtmosphereScattering/ScatteringModel.hpp"

#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Shader/Shaders/GlslLight.hpp>

#include <ShaderWriter/Source.hpp>

namespace atmosphere_scattering
{
	ScatteringModel::ScatteringModel( sdw::ShaderWriter & writer
		, AtmosphereModel & atmosphere
		, bool colorTransmittance
		, bool fastSky
		, bool fastAerialPerspective
		, bool renderSunDisk
		, bool bloomSunDisk
		, uint32_t binding
		, bool needsMultiscatter )
		: m_writer{ writer }
		, m_atmosphere{ atmosphere }
		, m_colorTransmittance{ colorTransmittance }
		, m_fastSky{ fastSky }
		, m_fastAerialPerspective{ fastAerialPerspective }
		, m_renderSunDisk{ renderSunDisk }
		, m_bloomSunDisk{ bloomSunDisk }
		, transmittanceMap{ writer.declCombinedImg< sdw::CombinedImage2DRgba32 >( "transmittanceMap"
			, binding++
			, 0u ) }
		, multiScatterMap{ writer.declCombinedImg< sdw::CombinedImage2DRgba32 >( "multiScatterMap"
			, ( needsMultiscatter ? binding++ : 0u )
			, 0u
			, needsMultiscatter ) }
		, skyViewMap{ writer.declCombinedImg< sdw::CombinedImage2DRgba32 >( "skyViewMap"
			, binding++
			, 0u ) }
		, volumeMap{ writer.declCombinedImg< sdw::CombinedImage3DRgba32 >( "volumeMap"
			, binding++
			, 0u ) }
	{
		m_atmosphere.setTransmittanceMap( transmittanceMap );
		m_atmosphere.setMultiscatterMap( multiScatterMap );
	}

	sdw::RetVec3 ScatteringModel::getSunLuminance( Ray const & pray )
	{
		if ( !m_getSunLuminance )
		{
			m_getSunLuminance = m_writer.implementFunction< sdw::Vec3 >( "scatter_getSunLuminance"
				, [&]( Ray const & ray )
				{
					auto sunDir = m_writer.declLocale( "sunDir"
						, m_atmosphere.getSunDirection() );

					if ( m_bloomSunDisk )
					{
						auto sunMinSolidAngle = m_writer.declLocale( "sunMinSolidAngle"
							, 0.053_f * sdw::Float{ castor::Pi< float > } / 180.0_f );
						auto sunMaxSolidAngle = m_writer.declLocale( "sunMaxSolidAngle"
							, sunMinSolidAngle * 10.0_f );
						auto sunSolidAngle = m_writer.declLocale( "sunSolidAngle"
							, doGetSunAngle( sunDir, sunMinSolidAngle, sunMaxSolidAngle ) );
						auto minSunCosTheta = m_writer.declLocale( "minSunCosTheta"
							, cos( sunSolidAngle ) );
						auto cosTheta = m_writer.declLocale( "cosTheta"
							, dot( ray.direction, sunDir ) );
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
							IF( m_writer, m_atmosphere.raySphereIntersectNearest( ray, vec3( 0.0_f ), m_atmosphere.getEarthRadius() ).valid() )
							{
								sunLuminance *= vec3( 0.0_f );
							}
							ELSE
							{
								// If the sun value is applied to this pixel, we need to calculate the transmittance to obscure it.
								sunLuminance *= vec3( 2.0_f ) * getSunRadiance( sunDir );
							}
							FI;
						}
						FI;

						m_writer.returnStmt( sunLuminance );
					}
					else
					{
						auto sunMinSolidAngle = m_writer.declLocale( "sunMinSolidAngle"
							, 0.53_f * sdw::Float{ castor::Pi< float > } / 180.0_f );
						auto sunMaxSolidAngle = m_writer.declLocale( "sunMaxSolidAngle"
							, sunMinSolidAngle * 2.0_f );
						auto sunSolidAngle = m_writer.declLocale( "sunSolidAngle"
							, doGetSunAngle( sunDir, sunMinSolidAngle, sunMaxSolidAngle ) );
						auto minSunCosTheta = m_writer.declLocale( "minSunCosTheta"
							, cos( sunSolidAngle ) );
						auto cosTheta = m_writer.declLocale( "cosTheta"
							, dot( ray.direction, sunDir ) );
						auto sunLuminance = m_writer.declLocale( "sunLuminance"
							, vec3( 0.0_f ) );

						IF( m_writer, cosTheta > minSunCosTheta )
						{
							IF( m_writer, !m_atmosphere.raySphereIntersectNearest( ray
								, vec3( 0.0_f )
								, m_atmosphere.getEarthRadius() ).valid() ) // no intersection
							{
								sunLuminance = vec3( 2.0_f ) * getSunRadiance( sunDir );
							}
							FI;
						}
						FI;

						m_writer.returnStmt( sunLuminance );
					}
				}
				, InRay{ m_writer, "ray" } );
		}

		return m_getSunLuminance( pray );
	}

	sdw::RetVec3 ScatteringModel::getSunRadiance( sdw::Vec3 const & sunDir )
	{
		return m_atmosphere.getSunRadiance( m_atmosphere.getCameraPosition()
			, sunDir
			, transmittanceMap );
	}

	sdw::Float ScatteringModel::aerialPerspectiveDepthToSlice( sdw::Float const & depth )
	{
		auto apKmPerSlice = 4.0_f;
		return depth * ( 1.0_f / apKmPerSlice );
	}

	sdw::Void ScatteringModel::getPixelTransLum( sdw::Vec2 const & pfragPos
		, sdw::Vec2 const & pfragSize
		, sdw::Float const & pfragDepth
		, sdw::Vec4 & ptransmittance
		, sdw::Vec4 & pluminance )
	{
		if ( !m_getPixelTransLum )
		{
			m_getPixelTransLum = m_writer.implementFunction< sdw::Void >( "scatter_getPixelTransLum"
				, [&]( sdw::Vec2 const & fragPos
					, sdw::Vec2 const & fragSize
					, sdw::Float const & fragDepth
					, sdw::Vec4 transmittance
					, sdw::Vec4 luminance )
				{
					auto ray = m_writer.declLocale( "ray"
						, m_atmosphere.castRay( fragPos, fragSize ) );
					auto L = m_writer.declLocale( "L"
						, vec3( 0.0_f ) );
					doRenderSky( fragSize
						, fragDepth
						, ray
						, L
						, luminance );

					if ( m_fastAerialPerspective )
					{
						doRenderFastAerial( fragPos
							, fragSize
							, fragDepth
							, ray.origin
							, L
							, luminance );
					}
					else
					{
						// Move to top atmosphere as the starting point for ray marching.
						// This is critical to be after the above to not disrupt above atmosphere tests and voxel selection.
						IF( m_writer, !m_atmosphere.moveToTopAtmosphere( ray ) )
						{
							// Ray is not intersecting the atmosphere
							if ( m_renderSunDisk )
							{
								luminance = vec4( getSunLuminance( ray ), 1.0_f );
							}

							m_writer.returnStmt();
						}
						FI;

						auto sampleCountIni = 0.0_f;
						auto ss = m_writer.declLocale( "ss"
							, m_atmosphere.integrateScatteredLuminanceNoShadow( fragPos
								, ray
								, m_atmosphere.getSunDirection()
								, sampleCountIni
								, fragDepth ) );
						doRegisterOutputs( ss, L, luminance, transmittance );
					}
				}
				, sdw::InVec2{ m_writer, "fragPos" }
				, sdw::InVec2{ m_writer, "fragSize" }
				, sdw::InFloat{ m_writer, "fragDepth" }
				, sdw::OutVec4{ m_writer, "transmittance" }
				, sdw::OutVec4{ m_writer, "luminance" } );
		}

		return m_getPixelTransLum( pfragPos
			, pfragSize
			, pfragDepth
			, ptransmittance
			, pluminance );
	}

	sdw::Void ScatteringModel::getPixelTransLum( sdw::Vec2 const & pfragPos
		, sdw::Vec2 const & pfragSize
		, sdw::Float const & pfragDepth
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
			m_getPixelTransLumShadow = m_writer.implementFunction< sdw::Void >( "scatter_getPixelTransLumShadow"
				, [&]( sdw::Vec2 const & fragPos
					, sdw::Vec2 const & fragSize
					, sdw::Float const & fragDepth
					, castor3d::shader::Light const & light
					, sdw::Vec3 const & surfaceWorldNormal
					, sdw::Mat4 const & lightMatrix
					, sdw::UInt const & cascadeIndex
					, sdw::UInt const & maxCascade
					, sdw::Vec4 transmittance
					, sdw::Vec4 luminance )
				{
					auto ray = m_writer.declLocale( "ray"
						, m_atmosphere.castRay( fragPos, fragSize ) );
					auto L = m_writer.declLocale( "L"
						, vec3( 0.0_f ) );
					doRenderSky( fragSize
						, fragDepth
						, ray
						, L
						, luminance );

					if ( m_fastAerialPerspective )
					{
						doRenderFastAerial( fragPos
							, fragSize
							, fragDepth
							, ray.origin
							, L
							, luminance );
					}
					else
					{
						// Move to top atmosphere as the starting point for ray marching.
						// This is critical to be after the above to not disrupt above atmosphere tests and voxel selection.
						IF( m_writer, !m_atmosphere.moveToTopAtmosphere( ray ) )
						{
							// Ray is not intersecting the atmosphere	
							if ( m_renderSunDisk )
							{
								luminance = vec4( getSunLuminance( ray ), 1.0_f );
							}

							m_writer.returnStmt();
						}
						FI;

						auto sampleCountIni = 0.0_f;
						auto ss = m_writer.declLocale( "ss"
							, m_atmosphere.integrateScatteredLuminance( fragPos
								, ray
								, m_atmosphere.getSunDirection()
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
			, plight
			, psurfaceWorldNormal
			, plightMatrix
			, pcascadeIndex
			, pmaxCascade
			, ptransmittance
			, pluminance );
	}

	sdw::Vec4 ScatteringModel::rescaleLuminance( sdw::Vec4 const & luminance )
	{
		luminance.xyz() /= luminance.a();	// Normalise according to sample count when path tracing

		// Similar setup to the Bruneton demo
		auto whitePoint = vec3( 1.08241_f, 0.96756_f, 0.95003_f );
		auto exposure = 10.0_f;
		auto hdr = vec3( 1.0_f ) - exp( -luminance.rgb() / whitePoint * exposure );
		return vec4( hdr, luminance.a() );
	}

	void ScatteringModel::doRenderSky( sdw::Vec2 const & fragSize
		, sdw::Float const & fragDepth
		, Ray const & ray
		, sdw::Vec3 & L
		, sdw::Vec4 & luminance )
	{
		if ( m_fastSky )
		{
			auto viewHeight = m_writer.declLocale( "viewHeight"
				, length( ray.origin ) );

			IF( m_writer, viewHeight < m_atmosphere.getAtmosphereRadius() && fragDepth >= 1.0_f )
			{
				auto uv = m_writer.declLocale< sdw::Vec2 >( "uv" );
				auto upVector = m_writer.declLocale( "upVector"
					, normalize( ray.origin ) );
				auto viewZenithCosAngle = m_writer.declLocale( "viewZenithCosAngle"
					, dot( ray.direction, upVector ) );

				auto sideVector = m_writer.declLocale( "sideVector"
					, normalize( cross( upVector, ray.direction ) ) );		// assumes non parallel vectors
				auto forwardVector = m_writer.declLocale( "forwardVector"
					, normalize( cross( sideVector, upVector ) ) );	// aligns toward the sun light but perpendicular to up vector
				auto lightOnPlane = m_writer.declLocale( "lightOnPlane"
					, vec2( dot( m_atmosphere.getSunDirection(), forwardVector )
						, dot( m_atmosphere.getSunDirection(), sideVector ) ) );
				lightOnPlane = normalize( lightOnPlane );
				auto lightViewCosAngle = m_writer.declLocale( "lightViewCosAngle"
					, lightOnPlane.x() );

				auto intersectGround = m_writer.declLocale( "intersectGround"
					, m_atmosphere.raySphereIntersectNearest( ray
						, vec3( 0.0_f )
						, m_atmosphere.getEarthRadius() ).valid() );

				m_atmosphere.skyViewLutParamsToUv( intersectGround, viewZenithCosAngle, lightViewCosAngle, viewHeight, uv, fragSize );

				luminance = vec4( skyViewMap.lod( uv, 0.0_f ).rgb(), 1.0_f );

				if ( m_renderSunDisk )
				{
					luminance.xyz() += getSunLuminance( ray );
				}

				m_writer.returnStmt();
			}
			FI;
		}
		else if ( m_renderSunDisk )
		{
			IF( m_writer, fragDepth >= 1.0_f )
			{
				L += getSunLuminance( ray );
			}
			FI;
		}
	}

	void ScatteringModel::doRenderFastAerial( sdw::Vec2 const & fragPos
		, sdw::Vec2 const & fragSize
		, sdw::Float const & fragDepth
		, sdw::Vec3 const & worldPos
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
				, m_atmosphere.getWorldPos( fragDepth
					, fragPos
					, fragSize ) );
			auto tDepth = m_writer.declLocale( "tDepth"
				, length( depthBufferWorldPos - ( worldPos + vec3( 0.0_f, -m_atmosphere.getEarthRadius(), 0.0_f ) ) ) );
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

	void ScatteringModel::doRegisterOutputs( SingleScatteringResult const & ss
		, sdw::Vec3 & L
		, sdw::Vec4 & luminance
		, sdw::Vec4 & transmittance )
	{
		L += ss.luminance();
		auto throughput = m_writer.declLocale( "throughput"
			, ss.transmittance() );

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

	sdw::Float ScatteringModel::doGetSunAngle( sdw::Vec3 const & sunDir
		, sdw::Float const & minAngle
		, sdw::Float const & maxAngle )const
	{
		auto sunZenithCosAngle = m_writer.declLocale( "sunZenithCosAngle"
			, dot( sunDir, normalize( m_atmosphere.getCameraPositionFromEarth() ) ) );
		return minAngle * sunZenithCosAngle
			+ maxAngle * ( 1.0_f - sunZenithCosAngle );
	}
}
