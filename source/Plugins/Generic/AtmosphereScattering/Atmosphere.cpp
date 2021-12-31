#include "AtmosphereScattering/Atmosphere.hpp"

#include <ShaderWriter/Source.hpp>

namespace atmosphere_scattering
{
	AtmosphereConfig::AtmosphereConfig( sdw::ShaderWriter & pwriter
		, AtmosphereData const & patmosphereData )
		: writer{ pwriter }
		, atmosphereData{ patmosphereData }
		, betaMEx{ sdw::vec3( atmosphereData.betaMSca / atmosphereData.betaMDiv ) }
	{
	}

	sdw::Vec3 AtmosphereConfig::getTransmittance( sdw::Float const & pr
		, sdw::Float const & pmu
		, sdw::CombinedImage2DRgba32 ptransmittanceMap )
	{
		if ( !m_getTransmittance )
		{
			// transmittance(=transparency) of atmosphere for infinite ray (r,mu)
			// (mu=cos(view zenith angle)), intersections with ground ignored
			m_getTransmittance = writer.implementFunction< sdw::Vec3 >( "getTransmittance"
				, [&]( sdw::Float r
					, sdw::Float mu
					, sdw::CombinedImage2DRgba32 transmittanceMap )
				{
					auto uv = writer.declLocale( "uv"
						, getTransmittanceUV( r, mu ) );
					writer.returnStmt( transmittanceMap.sample( uv ).rgb() );
				}
				, sdw::InFloat{ writer, "r" }
				, sdw::InFloat{ writer, "mu" }
				, sdw::InCombinedImage2DRgba32{ writer, "transmittanceMap" } );
		}

		return m_getTransmittance( pr, pmu, ptransmittanceMap );
	}

	sdw::Float AtmosphereConfig::phaseFunctionR( sdw::Float const & pmu )
	{
		if ( !m_phaseFunctionR )
		{
			// Rayleigh phase function
			m_phaseFunctionR = writer.implementFunction< sdw::Float >( "phaseFunctionR"
				, [&]( sdw::Float mu )
				{
					writer.returnStmt( ( 3.0_f / ( 16.0_f * sdw::Float{ castor::Pi< float > } ) ) * ( 1.0_f + mu * mu ) );
				}
				, sdw::InFloat{ writer, "mu" } );
		}

		return m_phaseFunctionR( pmu );
	}

	sdw::Float AtmosphereConfig::phaseFunctionM( sdw::Float const & pmu )
	{
		if ( !m_phaseFunctionM )
		{
			// Mie phase function
			m_phaseFunctionM = writer.implementFunction< sdw::Float >( "phaseFunctionM"
				, [&]( sdw::Float mu )
				{
					writer.returnStmt( 1.5_f * 1.0_f / ( 4.0_f * sdw::Float{ castor::Pi< float > } ) * ( 1.0_f - atmosphereData.mieG * atmosphereData.mieG ) * pow( 1.0_f + ( atmosphereData.mieG * atmosphereData.mieG ) - 2.0_f * atmosphereData.mieG * mu, -3.0_f / 2.0_f ) * ( 1.0_f + mu * mu ) / ( 2.0_f + atmosphereData.mieG * atmosphereData.mieG ) );
				}
				, sdw::InFloat{ writer, "mu" } );
		}

		return m_phaseFunctionM( pmu );
	}

	sdw::Vec3 AtmosphereConfig::getMie( sdw::Vec4 const & prayMie )
	{
		if ( !m_getMie )
		{
			// approximated single Mie scattering (cf. approximate Cm in paragraph "Angular precision")
			m_getMie = writer.implementFunction< sdw::Vec3 >( "getMie"
				, [&]( sdw::Vec4 rayMie )
				{
					// rayMie.rgb=C*, rayMie.w=Cm,r
					writer.returnStmt( rayMie.rgb() * rayMie.w() / max( rayMie.r(), 1e-4_f ) * ( vec3( atmosphereData.betaR.r() ) / atmosphereData.betaR ) );
				}
				, sdw::InVec4{ writer, "rayMie" } );
		}

		return m_getMie( prayMie );
	}

	sdw::Vec3 AtmosphereConfig::getSkyRadiance( sdw::Vec3 const & pcamera
		, sdw::Vec3 const & pviewdir
		, sdw::Vec3 const & psundir
		, sdw::CombinedImage2DRgba32 const & ptransmittanceMap
		, sdw::CombinedImage3DRgba32 const & pinscatterMap
		, sdw::Vec3 & pextinction )
	{
		if ( !m_getSkyRadiance )
		{
			m_getSkyRadiance = writer.implementFunction< sdw::Vec3 >( "getSkyRadiance"
				, [&]( sdw::Vec3 camera
					, sdw::Vec3 viewdir
					, sdw::Vec3 sundir
					, sdw::CombinedImage2DRgba32 transmittanceMap
					, sdw::CombinedImage3DRgba32 inscatterMap
					, sdw::Vec3 extinction )
				{
					auto result = writer.declLocale< sdw::Vec3 >( "result" );
					auto r = writer.declLocale( "r"
						, length( camera ) );
					auto rMu = writer.declLocale( "rMu"
						, dot( camera, viewdir ) );
					auto mu = writer.declLocale( "mu"
						, rMu / r );
					auto r0 = writer.declLocale( "r0"
						, r );
					auto mu0 = writer.declLocale( "mu0"
						, mu );

					auto deltaSq = writer.declLocale( "deltaSq"
						, sqrt( rMu * rMu - r * r + atmosphereData.Rt * atmosphereData.Rt ) );
					auto din = writer.declLocale( "din"
						, max( -rMu - deltaSq, 0.0_f ) );

					IF( writer, din > 0.0_f )
					{
						camera += din * viewdir;
						rMu += din;
						mu = rMu / atmosphereData.Rt;
						r = atmosphereData.Rt;
					}
					FI;

					IF( writer, r <= atmosphereData.Rt )
					{
						auto nu = writer.declLocale( "nu"
							, dot( viewdir, sundir ) );
						auto muS = writer.declLocale( "muS"
							, dot( camera, sundir ) / r );

						auto inScatter = writer.declLocale( "inScatter"
							, texture4D( inscatterMap, r, rMu / r, muS, nu ) );
						extinction = getTransmittance( r, mu, transmittanceMap );

						auto inScatterM = writer.declLocale( "inScatterM"
							, getMie( inScatter ) );
						auto phase = writer.declLocale( "phase"
							, phaseFunctionR( nu ) );
						auto phaseM = writer.declLocale( "phaseM"
							, phaseFunctionM( nu ) );
						result = inScatter.rgb() * phase + inScatterM * phaseM;
					}
					ELSE
					{
						result = vec3( 0.0_f );
						extinction = vec3( 1.0_f );
					}
					FI;

					writer.returnStmt( result * atmosphereData.sunIntensity );
				}
				, sdw::InVec3{ writer, "camera" }
				, sdw::InVec3{ writer, "viewdir" }
				, sdw::InVec3{ writer, "sundir" }
				, sdw::InCombinedImage2DRgba32{ writer, "transmittanceMap" }
				, sdw::InCombinedImage3DRgba32{ writer, "inscatterMap" }
				, sdw::OutVec3{ writer, "extinction" } );
		}

		return m_getSkyRadiance( pcamera, pviewdir, psundir, ptransmittanceMap, pinscatterMap, pextinction );
	}

	sdw::Vec3 AtmosphereConfig::getIrradiance( sdw::CombinedImage2DRgba32 pirradianceMap
		, sdw::Float pr
		, sdw::Float pmuS )
	{
		if ( !m_getIrradiance )
		{
			m_getIrradiance = writer.implementFunction< sdw::Vec3 >( "getIrradiance"
				, [&]( sdw::CombinedImage2DRgba32 irradianceMap
					, sdw::Float r
					, sdw::Float muS )
				{
					auto uv = writer.declLocale( "uv"
						, getIrradianceUV( r, muS ) );
					writer.returnStmt( irradianceMap.sample( uv ).rgb() );
				}
				, sdw::InCombinedImage2DRgba32{ writer, "irradianceMap" }
				, sdw::InFloat{ writer, "r" }
				, sdw::InFloat{ writer, "mu" } );
		}

		return m_getIrradiance( pirradianceMap, pr, pmuS );
	}

	sdw::Vec3 AtmosphereConfig::getSkyIrradiance( sdw::CombinedImage2DRgba32 pskyIrradiance
		, sdw::Float const & pr
		, sdw::Float const & pmuS )
	{
		if ( !m_getSkyIrradiance )
		{
			m_getSkyIrradiance = writer.implementFunction< sdw::Vec3 >( "getSkyIrradiance"
				, [&]( sdw::CombinedImage2DRgba32 skyIrradiance
					, sdw::Float r
					, sdw::Float muS )
				{
					writer.returnStmt( getIrradiance( skyIrradiance, r, muS ) * atmosphereData.sunIntensity );
				}
				, sdw::InCombinedImage2DRgba32{ writer, "skyIrradiance" }
				, sdw::InFloat{ writer, "r" }
				, sdw::InFloat{ writer, "muS" } );
		}

		return m_getSkyIrradiance( pskyIrradiance, pr, pmuS );
	}

	sdw::Vec2 AtmosphereConfig::getTransmittanceUV( sdw::Float const & pr
		, sdw::Float const & pmu )
	{
		if ( !m_getTransmittanceUV )
		{
			m_getTransmittanceUV = writer.implementFunction< sdw::Vec2 >( "getTransmittanceUV"
				, [&]( sdw::Float r
					, sdw::Float mu )
				{
					auto uR = writer.declLocale< sdw::Float >( "uR" );
					auto uMu = writer.declLocale< sdw::Float >( "uMu" );

					if ( transmittanceNonLinear )
					{
						uR = sqrt( ( r - atmosphereData.Rg ) / ( atmosphereData.Rt - atmosphereData.Rg ) );
						uMu = atan( ( mu + 0.15_f ) / ( 1.0_f + 0.15_f ) * tan( 1.5_f ) ) / 1.5_f;
					}
					else
					{
						uR = ( r - atmosphereData.Rg ) / ( atmosphereData.Rt - atmosphereData.Rg );
						uMu = ( mu + 0.15_f ) / ( 1.0_f + 0.15_f );
					}

					writer.returnStmt( vec2( uMu, uR ) );
				}
				, sdw::InFloat{ writer, "r" }
				, sdw::InFloat{ writer, "mu" } );
		}

		return m_getTransmittanceUV( pr, pmu );
	}

	void AtmosphereConfig::getTransmittanceRMu( sdw::Vec2 const & pfragCoord
		, sdw::Float & pr
		, sdw::Float & pmuS )
	{
		if ( !m_getTransmittanceRMu )
		{
			m_getTransmittanceRMu = writer.implementFunction< sdw::Void >( "getTransmittanceRMu"
				, [&]( sdw::Vec2 fragCoord
					, sdw::Float r
					, sdw::Float muS )
				{
					r = fragCoord.y() / writer.cast< sdw::Float >( atmosphereData.transmittanceH );
					muS = fragCoord.x() / writer.cast< sdw::Float >( atmosphereData.transmittanceW );

					if ( transmittanceNonLinear )
					{
						r = atmosphereData.Rg + ( r * r ) * ( atmosphereData.Rt - atmosphereData.Rg );
						muS = -0.15_f + tan( 1.5_f * muS ) / tan( 1.5_f ) * ( 1.0_f + 0.15_f );
					}
					else
					{
						r = atmosphereData.Rg + r * ( atmosphereData.Rt - atmosphereData.Rg );
						muS = -0.15_f + muS * ( 1.0_f + 0.15_f );
					}
				}
				, sdw::InVec2{ writer, "fragCoord" }
				, sdw::OutFloat{ writer, "r" }
				, sdw::OutFloat{ writer, "muS" } );
		}

		m_getTransmittanceRMu( pfragCoord, pr, pmuS );
	}

	sdw::Vec2 AtmosphereConfig::getIrradianceUV( sdw::Float const & pr
		, sdw::Float const & pmuS )
	{
		if ( !m_getIrradianceUV )
		{
			m_getIrradianceUV = writer.implementFunction< sdw::Vec2 >( "getIrradianceUV"
				, [&]( sdw::Float r
					, sdw::Float muS )
				{
					auto uR = writer.declLocale< sdw::Float >( "uR"
						, ( r - atmosphereData.Rg ) / ( atmosphereData.Rt - atmosphereData.Rg ) );
					auto uMuS = writer.declLocale< sdw::Float >( "uMuS"
						, ( muS + 0.2_f ) / ( 1.0_f + 0.2_f ) );
					writer.returnStmt( vec2( uMuS, uR ) );
				}
				, sdw::InFloat{ writer, "r" }
				, sdw::InFloat{ writer, "muS" } );
		}

		return m_getIrradianceUV( pr, pmuS );
	}

	void AtmosphereConfig::getIrradianceRMuS( sdw::Vec2 const & pfragCoord
		, sdw::Float & pr
		, sdw::Float & pmuS )
	{
		if ( !m_getIrradianceRMuS )
		{
			m_getIrradianceRMuS = writer.implementFunction< sdw::Void >( "getIrradianceRMuS"
				, [&]( sdw::Vec2 fragCoord
					, sdw::Float r
					, sdw::Float muS )
				{
					r = atmosphereData.Rg + ( fragCoord.y() - 0.5_f ) / ( writer.cast< sdw::Float >( atmosphereData.skyH ) - 1.0_f ) * ( atmosphereData.Rt - atmosphereData.Rg );
					muS = -0.2_f + ( fragCoord.x() - 0.5_f ) / ( writer.cast< sdw::Float >( atmosphereData.skyW ) - 1.0_f ) * ( 1.0_f + 0.2_f );
				}
				, sdw::InVec2{ writer, "fragCoord" }
				, sdw::OutFloat{ writer, "r" }
				, sdw::OutFloat{ writer, "muS" } );
		}

		m_getIrradianceRMuS( pfragCoord, pr, pmuS );
	}

	sdw::Vec4 AtmosphereConfig::texture4D( sdw::CombinedImage3DRgba32 ptable
		, sdw::Float pr
		, sdw::Float pmu
		, sdw::Float pmuS
		, sdw::Float pnu )
	{
		if ( !m_texture4D )
		{
			m_texture4D = writer.implementFunction< sdw::Vec4 >( "texture4D"
				, [&]( sdw::CombinedImage3DRgba32 table
					, sdw::Float r
					, sdw::Float mu
					, sdw::Float muS
					, sdw::Float nu )
				{
					auto uR = writer.declLocale< sdw::Float >( "uR" );
					auto uMu = writer.declLocale< sdw::Float >( "uMu" );
					auto uMuS = writer.declLocale< sdw::Float >( "uMuS" );

					auto H = writer.declLocale( "H"
						, sqrt( atmosphereData.Rt * atmosphereData.Rt - atmosphereData.Rg * atmosphereData.Rg ) );
					auto rho = writer.declLocale( "rho"
						, sqrt( r * r - atmosphereData.Rg * atmosphereData.Rg ) );

					if ( inscatterNonLinear )
					{
						auto rmu = writer.declLocale( "rmu"
							, r * mu );
						auto delta = writer.declLocale( "delta"
							, rmu * rmu - r * r + atmosphereData.Rg * atmosphereData.Rg );
						auto cst = writer.declLocale( "cst"
							, writer.ternary( rmu < 0.0_f && delta > 0.0_f
								, vec4( 1.0_f, 0.0_f, 0.0_f, 0.5_f - 0.5_f / writer.cast< sdw::Float >( atmosphereData.resMu ) )
								, vec4( -1.0_f, H * H, H, 0.5_f + 0.5_f / writer.cast< sdw::Float >( atmosphereData.resMu ) ) ) );
						uR = 0.5_f / writer.cast< sdw::Float >( atmosphereData.resR ) + rho / H * ( 1.0_f - 1.0_f / writer.cast< sdw::Float >( atmosphereData.resR ) );
						uMu = cst.w() + ( rmu * cst.x() + sqrt( delta + cst.y() ) ) / ( rho + cst.z() ) * ( 0.5_f - 1.0_f / writer.cast< sdw::Float >( atmosphereData.resMu ) );
						// paper formula
						//uMuS = 0.5_f / writer.cast< sdw::Float >( atmosphereData.resMuS ) + max( ( 1.0_f - exp( -3.0_f * muS - 0.6_f ) ) / ( 1.0_f - exp( -3.6_f ) ), 0.0_f ) * ( 1.0_f - 1.0_f / writer.cast< sdw::Float >( atmosphereData.resMuS ) );
						// better formula
						uMuS = 0.5_f / writer.cast< sdw::Float >( atmosphereData.resMuS ) + ( atan( max( muS, -0.1975_f ) * tan( 1.26_f * 1.1_f ) ) / 1.1_f + ( 1.0_f - 0.26_f ) ) * 0.5_f * ( 1.0_f - 1.0_f / writer.cast< sdw::Float >( atmosphereData.resMuS ) );
					}
					else
					{
						uR = 0.5_f / writer.cast< sdw::Float >( atmosphereData.resR ) + rho / H * ( 1.0_f - 1.0_f / writer.cast< sdw::Float >( atmosphereData.resR ) );
						uMu = 0.5_f / writer.cast< sdw::Float >( atmosphereData.resMu ) + ( mu + 1.0_f ) / 2.0_f * ( 1.0_f - 1.0_f / writer.cast< sdw::Float >( atmosphereData.resMu ) );
						uMuS = 0.5_f / writer.cast< sdw::Float >( atmosphereData.resMuS ) + max( muS + 0.2_f, 0.0_f ) / 1.2_f * ( 1.0_f - 1.0_f / writer.cast< sdw::Float >( atmosphereData.resMuS ) );
					}

					auto lerp = writer.declLocale( "lerp"
						, ( nu + 1.0_f ) / 2.0_f * ( writer.cast< sdw::Float >( atmosphereData.resNu ) - 1.0_f ) );
					auto uNu = writer.declLocale( "uNu"
						, floor( lerp ) );
					lerp = lerp - uNu;
					writer.returnStmt( table.sample( vec3( ( uNu + uMuS ) / writer.cast< sdw::Float >( atmosphereData.resNu ), uMu, uR ) ) * ( 1.0_f - lerp )
						+ table.sample( vec3( ( uNu + uMuS + 1.0_f ) / writer.cast< sdw::Float >( atmosphereData.resNu ), uMu, uR ) ) * lerp );
				}
				, sdw::InCombinedImage3DRgba32{ writer, "table" }
				, sdw::InFloat{ writer, "r" }
				, sdw::InFloat{ writer, "mu" }
				, sdw::InFloat{ writer, "muS" }
				, sdw::InFloat{ writer, "nu" } );
		}

		return m_texture4D( ptable, pr, pmu, pmuS, pnu );
	}

	void AtmosphereConfig::getMuMuSNu( sdw::Float const & pr
		, sdw::Vec4 const & pdhdH
		, sdw::Vec2 const & pfragCoord
		, sdw::Float & pmu
		, sdw::Float & pmuS
		, sdw::Float & pnu )
	{
		if ( !m_getMuMuSNu )
		{
			m_getMuMuSNu = writer.implementFunction< sdw::Void >( "getMuMuSNu"
				, [&]( sdw::Float r
					, sdw::Vec4 dhdH
					, sdw::Vec2 fragCoord
					, sdw::Float mu
					, sdw::Float muS
					, sdw::Float nu )
				{
					auto x = writer.declLocale( "x"
						, fragCoord.x() - 0.5_f );
					auto y = writer.declLocale( "y"
						, fragCoord.y() - 0.5_f );

					if ( inscatterNonLinear )
					{
						IF( writer, y < writer.cast< sdw::Float >( atmosphereData.resMu ) / 2.0_f )
						{
							auto d = writer.declLocale( "d"
								, 1.0_f - y / ( writer.cast< sdw::Float >( atmosphereData.resMu ) / 2.0_f - 1.0_f ) );
							d = min( max( dhdH.z(), d * dhdH.w() ), dhdH.w() * 0.999_f );
							mu = ( atmosphereData.Rg * atmosphereData.Rg - r * r - d * d ) / ( 2.0_f * r * d );
							mu = min( mu, -sqrt( 1.0_f - ( atmosphereData.Rg / r ) * ( atmosphereData.Rg / r ) ) - 0.001_f );
						}
						ELSE
						{
							auto d = writer.declLocale( "d"
								, ( y - writer.cast< sdw::Float >( atmosphereData.resMu ) / 2.0_f ) / ( writer.cast< sdw::Float >( atmosphereData.resMu ) / 2.0_f - 1.0_f ) );
							d = min( max( dhdH.x(), d * dhdH.y() ), dhdH.y() * 0.999_f );
							mu = ( atmosphereData.Rt * atmosphereData.Rt - r * r - d * d ) / ( 2.0_f * r * d );
						}
						FI;

						muS = mod( x, writer.cast< sdw::Float >( atmosphereData.resMuS ) ) / ( writer.cast< sdw::Float >( atmosphereData.resMuS ) - 1.0_f );
						// paper formula
						//muS = -( 0.6 + log( 1.0 - muS * ( 1.0 -  exp( -3.6 ) ) ) ) / 3.0;
						// better formula
						muS = tan( ( 2.0_f * muS - 1.0_f + 0.26_f ) * 1.1_f ) / tan( 1.26_f * 1.1_f );
						nu = -1.0_f + floor( x / writer.cast< sdw::Float >( atmosphereData.resMuS ) ) / ( writer.cast< sdw::Float >( atmosphereData.resNu ) - 1.0_f ) * 2.0_f;
					}
					else
					{
						mu = -1.0_f + 2.0_f * y / ( writer.cast< sdw::Float >( atmosphereData.resMu ) - 1.0_f );
						muS = mod( x, writer.cast< sdw::Float >( atmosphereData.resMuS ) ) / ( writer.cast< sdw::Float >( atmosphereData.resMuS ) - 1.0_f );
						muS = -0.2_f + muS * 1.2_f;
						nu = -1.0_f + floor( x / writer.cast< sdw::Float >( atmosphereData.resMuS ) ) / ( writer.cast< sdw::Float >( atmosphereData.resNu ) - 1.0_f ) * 2.0_f;
					}
				}
				, sdw::InFloat{ writer, "r" }
				, sdw::InVec4{ writer, "dhdH" }
				, sdw::InVec2{ writer, "fragCoord" }
				, sdw::OutFloat{ writer, "mu" }
				, sdw::OutFloat{ writer, "muS" }
				, sdw::OutFloat{ writer, "nu" } );
		}

		m_getMuMuSNu( pr, pdhdH, pfragCoord, pmu, pmuS, pnu );
	}
}
