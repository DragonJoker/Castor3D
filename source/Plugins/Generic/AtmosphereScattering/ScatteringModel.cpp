#include "AtmosphereScattering/ScatteringModel.hpp"

#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Shader/Shaders/GlslLight.hpp>

#include <ShaderWriter/Source.hpp>

namespace atmosphere_scattering
{
	ScatteringModel::ScatteringModel( sdw::ShaderWriter & writer
		, AtmosphereModel & atmosphere
		, Settings settings
		, uint32_t & binding
		, uint32_t set )
		: m_writer{ writer }
		, m_atmosphere{ atmosphere }
		, m_settings{ castor::move( settings ) }
		, transmittanceMap{ writer.declCombinedImg< sdw::CombinedImage2DRgba32 >( "transmittanceMap"
			, binding++
			, set ) }
		, multiScatterMap{ writer.declCombinedImg< sdw::CombinedImage2DRgba32 >( "multiScatterMap"
			, ( settings.needsMultiscatter ? binding++ : 0u )
			, set
			, settings.needsMultiscatter ) }
		, skyViewMap{ writer.declCombinedImg< sdw::CombinedImage2DRgba32 >( "skyViewMap"
			, binding++
			, set ) }
		, volumeMap{ writer.declCombinedImg< sdw::CombinedImage3DRgba32 >( "volumeMap"
			, binding++
			, set ) }
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
					auto sunLuminance = m_writer.declLocale( "sunLuminance"
						, vec3( 0.0_f ) );

					if ( m_settings.bloomSunDisk )
					{
						auto sunMinAngularDiameter = m_writer.declLocale( "sunMinAngularDiameter"
							, sdw::Float{ ( 0.053_degrees ).radians() } );
						auto sunMaxAngularDiameter = m_writer.declLocale( "sunMaxSolidAngle"
							, sunMinAngularDiameter * 10.0_f );
						auto sunAngularDiameter = m_writer.declLocale( "sunAngularDiameter"
							, doGetSunAngle( sunDir, sunMinAngularDiameter, sunMaxAngularDiameter ) );
						auto sunCosTheta = m_writer.declLocale( "sunCosTheta"
							, cos( sunAngularDiameter ) );
						auto cosTheta = m_writer.declLocale( "cosTheta"
							, dot( ray.direction, sunDir ) );
						auto intensity = m_writer.declLocale( "intensity"
							, 1.0_f );
						auto centerToEdge = m_writer.declLocale( "centerToEdge"
							, cosTheta - sunCosTheta );

						IF( m_writer, cosTheta < sunCosTheta )
						{
							auto gaussianBloom = m_writer.declLocale( "gaussianBloom"
								, exp( centerToEdge * 50000.0_f ) * 0.5_f );
							auto invBloom = m_writer.declLocale( "invBloom"
								, 1.0_f / ( 0.02_f - centerToEdge * 300.0_f ) * 0.01_f );
							intensity = gaussianBloom + invBloom;
						}
						FI;

						// Use smoothstep to limit the effect, so it drops off to actual zero.
						intensity = smoothStep( 0.002_f, 1.0_f, intensity );

						IF( m_writer, intensity > 0.0_f )
						{
							IF( m_writer, !m_atmosphere.raySphereIntersectNearest( ray
								, vec3( 0.0_f )
								, m_atmosphere.getPlanetRadius() ).valid() )
							{
								sunLuminance = 2.0_f * getSunRadiance( ray.direction );

								auto u = vec3( 1.0_f, 1.0_f, 1.0_f );
								auto a = vec3( 0.397_f, 0.503_f, 0.652_f );
								centerToEdge = 1.0_f - centerToEdge;
								auto mu = m_writer.declLocale( "mu"
									, sqrt( 1.0_f - centerToEdge * centerToEdge ) );
								auto factor = m_writer.declLocale( "factor"
									, 1.0_f - u * ( 1.0_f - pow( vec3( mu ), a ) ) );

								sunLuminance *= max( vec3( intensity ), factor );
							}
							FI;
						}
						FI;
					}
					else
					{
						auto sunMinAngularDiameter = m_writer.declLocale( "sunMinAngularDiameter"
							, sdw::Float{ ( 0.53_degrees ).radians() } );
						auto sunMaxAngularDiameter = m_writer.declLocale( "sunMaxAngularDiameter"
							, sunMinAngularDiameter * 2.0_f );
						auto sunAngularDiameter = m_writer.declLocale( "sunAngularDiameter"
							, doGetSunAngle( sunDir, sunMinAngularDiameter, sunMaxAngularDiameter ) );
						auto sunCosTheta = m_writer.declLocale( "sunCosTheta"
							, cos( sunAngularDiameter ) );
						auto cosTheta = m_writer.declLocale( "cosTheta"
							, dot( ray.direction, sunDir ) );

						IF( m_writer, cosTheta > sunCosTheta )
						{
							IF( m_writer, !m_atmosphere.raySphereIntersectNearest( ray
								, vec3( 0.0_f )
								, m_atmosphere.getPlanetRadius() ).valid() ) // no intersection
							{
								auto sunSolidAngle = m_writer.declLocale( "sunSolidAngle"
									, castor::PiMult2< float > * ( 1.0_f - cos( sunAngularDiameter * 0.5_f ) ) );
								auto zenithSunLuminance = m_writer.declLocale( "zenithSunLuminance"
									, m_atmosphere.getSunIlluminance() / sunSolidAngle );
								auto outerspaceLuminance = m_writer.declLocale( "outerspaceLuminance"
									, zenithSunLuminance / getSunRadiance( vec3( 0.0_f, 1.0_f, 0.0_f ) ) );

								sunLuminance = outerspaceLuminance * getSunRadiance( ray.direction ) / 1000.0f;

								auto centerToEdge = m_writer.declLocale( "centerToEdge"
									, cosTheta - sunCosTheta );
								auto u = vec3( 1.0_f, 1.0_f, 1.0_f );
								auto a = vec3( 0.397_f, 0.503_f, 0.652_f );
								centerToEdge = 1.0_f - centerToEdge;
								auto mu = m_writer.declLocale( "mu"
									, sqrt( 1.0_f - centerToEdge * centerToEdge ) );
								auto factor = m_writer.declLocale( "factor"
									, 1.0_f - u * ( 1.0_f - pow( vec3( mu ), a ) ) );

								sunLuminance *= factor;
							}
							FI;
						}
						FI;
					}

					m_writer.returnStmt( sunLuminance );
				}
				, InRay{ m_writer, "ray" } );
		}

		return m_getSunLuminance( pray );
	}

	sdw::RetVec3 ScatteringModel::getSunRadiance( sdw::Vec3 const & sunDir )
	{
		return m_atmosphere.getSunRadiance( sunDir, transmittanceMap );
	}

	sdw::Vec4 ScatteringModel::gradSkyView( sdw::Vec2 const & coord
		, sdw::Vec2 const & dPdx
		, sdw::Vec2 const & dPdy )const
	{
		return skyViewMap.grad( coord, dPdx, dPdy );
	}

	sdw::Float ScatteringModel::aerialPerspectiveDepthToSlice( sdw::Float const & depth )
	{
		auto apKmPerSlice = 4.0_f;
		return depth * ( 1.0_f / apKmPerSlice );
	}

	castor3d::shader::RetRay ScatteringModel::getPixelTransLum( sdw::Vec2 const & pfragPos
		, sdw::Vec2 const & pfragSize
		, sdw::Float const & pfragDepth
		, sdw::Vec4 & ptransmittance
		, sdw::Vec4 & pluminance )
	{
		if ( !m_getPixelTransLum )
		{
			m_getPixelTransLum = m_writer.implementFunction< castor3d::shader::Ray >( "scatter_getPixelTransLum"
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
					auto viewHeight = m_writer.declLocale( "viewHeight"
						, length( ray.origin ) );
					doRenderSky( fragSize
						, fragDepth
						, ray
						, viewHeight
						, L
						, luminance );

					IF( m_writer, !doRenderFastAerial( fragPos
						, fragSize
						, fragDepth
						, ray.origin
						, viewHeight
						, L
						, luminance ) )
					{
						// Move to top atmosphere as the starting point for ray marching.
						// This is critical to be after the above to not disrupt above atmosphere tests and voxel selection.
						IF( m_writer, !m_atmosphere.moveToTopAtmosphere( ray ) )
						{
							// Ray is not intersecting the atmosphere
							if ( m_settings.renderSunDisk )
							{
								luminance = vec4( getSunLuminance( ray ), 1.0_f );
							}

							m_writer.returnStmt( ray );
						}
						FI;

						auto ss = m_writer.declLocale( "ss"
							, m_atmosphere.integrateScatteredLuminance( fragPos
								, ray
								, m_atmosphere.getSunDirection()
								, 0.0_f /*sampleCountIni*/
								, fragDepth ) );
						doRegisterOutputs( ss, L, luminance, transmittance );
					}
					FI;

					m_writer.returnStmt( ray );
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

	sdw::RetVec3 ScatteringModel::getSkyRadiance( sdw::Vec3 const & pwsNormal
		, sdw::Vec3 const & pwsPosition
		, sdw::Vec3 const & pV
		, sdw::Vec2 const & pfragSize )
	{
		if ( !m_getSkyRadiance )
		{
			m_getSkyRadiance = m_writer.implementFunction< sdw::Vec3 >( "scatter_getSkyRadiance"
				, [&]( sdw::Vec3 const & wsNormal
					, sdw::Vec3 const & wsPosition
					, sdw::Vec3 const & V
					, sdw::Vec2 const & fragSize )
				{
					auto psPosition = m_writer.declLocale( "psPosition"
						, wsPosition - m_atmosphere.getPlanetPosition() );
					auto ray = m_writer.declLocale( "ray"
						, castor3d::shader::Ray{ m_writer, psPosition, reflect( -V, wsNormal ) } );
					auto viewHeight = m_writer.declLocale( "viewHeight"
						, length( ray.origin ) );
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
							, m_atmosphere.getPlanetRadius() ).valid() );

					auto uv = m_writer.declLocale< sdw::Vec2 >( "uv" );
					m_atmosphere.skyViewLutParamsToUv( intersectGround, viewZenithCosAngle, lightViewCosAngle, viewHeight, uv, fragSize );

					auto result = m_writer.declLocale( "result"
						, skyViewMap.lod( uv, 0.0_f ).rgb() );

					// Rescale
					auto whitePoint = vec3( 1.08241_f, 0.96756_f, 0.95003_f );
					auto exposure = 10.0_f;
					result = vec3( 1.0_f ) - exp( -result / whitePoint * exposure );

					m_writer.returnStmt( result );
				}
				, sdw::InVec3{ m_writer, "wsNormal" }
				, sdw::InVec3{ m_writer, "wsPosition" }
				, sdw::InVec3{ m_writer, "V" }
				, sdw::InVec2{ m_writer, "fragSize" } );
		}

		return m_getSkyRadiance( pwsNormal, pwsPosition, pV, pfragSize );
	}

	sdw::Vec4 ScatteringModel::rescaleLuminance( sdw::Vec4 const & luminance )
	{
		auto hdr = m_writer.declLocale( "hdr"
			, luminance.xyz() / luminance.a() );	// Normalise according to sample count when path tracing

		// Similar setup to the Bruneton demo
		auto whitePoint = vec3( 1.08241_f, 0.96756_f, 0.95003_f );
		auto exposure = 10.0_f;
		hdr = vec3( 1.0_f ) - exp( -hdr / whitePoint * exposure );
		return vec4( m_writer.ternary( luminance.a() == 0.0_f, luminance.xyz(), hdr * luminance.a() ), 1.0_f );
	}

	void ScatteringModel::doRenderSky( sdw::Vec2 const & fragSize
		, sdw::Float const & fragDepth
		, Ray const & ray
		, sdw::Float const & viewHeight
		, sdw::Vec3 & L
		, sdw::Vec4 & luminance )
	{
		if ( m_settings.fastSky )
		{
			IF( m_writer, viewHeight < m_atmosphere.getAtmosphereRadius() && fragDepth == 0.0_f )
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
						, m_atmosphere.getPlanetRadius() ).valid() );

				m_atmosphere.skyViewLutParamsToUv( intersectGround, viewZenithCosAngle, lightViewCosAngle, viewHeight, uv, fragSize );

				luminance = vec4( skyViewMap.lod( uv, 0.0_f ).rgb(), 1.0_f );

				if ( m_settings.renderSunDisk )
				{
					luminance.xyz() += getSunLuminance( ray );
				}

				m_writer.returnStmt( ray );
			}
			FI;
		}
		else if ( m_settings.renderSunDisk )
		{
			IF( m_writer, fragDepth == 0.0_f )
			{
				L += getSunLuminance( ray );
			}
			FI;
		}
	}

	sdw::Boolean ScatteringModel::doRenderFastAerial( sdw::Vec2 const & fragPos
		, sdw::Vec2 const & fragSize
		, sdw::Float const & fragDepth
		, sdw::Vec3 const & worldPos
		, sdw::Float const & viewHeight
		, sdw::Vec3 & L
		, sdw::Vec4 & luminance )
	{
		if ( !m_settings.fastAerialPerspective )
		{
			return 0_b;
		}

		if ( m_settings.colorTransmittance )
		{
			castor3d::log::error << "The fastAerialPerspective path does not support colorTransmittance." << std::endl;
			return 0_b;
		}

		auto isAerial = m_writer.declLocale( "isAerial"
			, viewHeight < m_atmosphere.getAtmosphereRadius() );

		IF( m_writer, isAerial )
		{
			auto apSliceCount = 32.0_f;
			auto depthBufferWorldPos = m_writer.declLocale( "depthBufferWorldPos"
				, m_atmosphere.getWorldPos( fragDepth
					, fragPos
					, fragSize ) );
			auto tDepth = m_writer.declLocale( "tDepth"
				, length( depthBufferWorldPos - worldPos ) );
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
		FI;

		return isAerial;
	}

	void ScatteringModel::doRegisterOutputs( SingleScatteringResult const & ss
		, sdw::Vec3 & L
		, sdw::Vec4 & luminance
		, sdw::Vec4 & transmittance )
	{
		L += ss.luminance();
		auto throughput = m_writer.declLocale( "throughput"
			, ss.transmittance() );

		if ( m_settings.colorTransmittance )
		{
			luminance = vec4( L, 1.0_f );
			transmittance = vec4( throughput, 1.0_f );
		}
		else
		{
			auto trs = m_writer.declLocale( "trs"
				, dot( throughput, sdw::vec3( 1.0 / 3.0 ) ) );
			luminance = vec4( L, 1.0_f - trs );
		}
	}

	sdw::Float ScatteringModel::doGetSunAngle( sdw::Vec3 const & sunDir
		, sdw::Float const & minAngle
		, sdw::Float const & maxAngle )const
	{
		auto sunZenithCosAngle = m_writer.declLocale( "sunZenithCosAngle"
			, dot( sunDir, normalize( m_atmosphere.getCameraPosition() ) ) );
		return minAngle * sunZenithCosAngle
			+ maxAngle * ( 1.0_f - sunZenithCosAngle );
	}
}
