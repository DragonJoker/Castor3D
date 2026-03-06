#include "AtmosphereScattering/CloudsVolumePlugin.hpp"

#include <Castor3D/Shader/Shaders/GlslVolumeShaders.hpp>

namespace atmosphere_scattering
{
	//************************************************************************************************

	namespace details
	{
		static uint32_t getNextBinding( uint32_t & binding )
		{
			auto result = binding;
			++binding;
			return result;
		}

		template< typename DataT, typename BufferT >
		static DataT createUniformBufferData( sdw::ShaderWriter & writer, uint32_t binding )
		{
			auto buffer = writer.declUniformBuffer<>( BufferT::Buffer, binding, 0u );
			auto result = buffer.template declMember< DataT >( BufferT::Data );
			buffer.end();
			return result;
		}
	}

	//************************************************************************************************

	AtmosphereVolumeTraversal::AtmosphereVolumeTraversal( sdw::ShaderWriter & writer
		, c3ds::VolumeShaders const & volumeShaders
		, AtmosphereModel & atmosphere )
		: m_writer{ writer }
		, m_volumeShaders{ volumeShaders }
		, m_atmosphere{ atmosphere }
	{
	}

	void AtmosphereVolumeTraversal::fillType( sdw::type::BaseStruct & type )const
	{
		type.declMember( "atmosphereData", AtmosphereTraverseData::makeType( type.getTypesCache() ) );
	}

	void AtmosphereVolumeTraversal::fillInit( sdw::type::BaseStruct & type
		, sdw::expr::ExprList & inits )const
	{
		inits.emplace_back( sdw::makeAggrInit( AtmosphereTraverseData::makeType( type.getTypesCache() )
			, AtmosphereTraverseData::getZeroInit() ) );
	}

	void AtmosphereVolumeTraversal::registerVolumes( c3ds::Volumes & volumes )
	{
		m_volumeType = volumes.registerVolumeType( getTraversalFunc(), getStepFunc() );
	}

	void AtmosphereVolumeTraversal::initialise( sdw::Vec2 const & pixelCoord, c3ds::Ray & ray
		, c3ds::Volumes & volumes, c3ds::VolumesTraversalResult & result
		, sdw::Vec4 const & depthObj )
	{
		auto atmosphereTraversal = result.getMember< AtmosphereTraverseData >( "atmosphereData" );
		auto range = m_writer.declLocale( "range"
			, listVolumes( ray, depthObj.b(), depthObj.g(), pixelCoord, volumes ) );
		atmosphereTraversal.initialise( m_atmosphere.hasVariableSampleCount(), m_atmosphere.atmosphereData, ray, range );
	}

	void AtmosphereVolumeTraversal::finalise( c3ds::Ray const & ray, c3ds::VolumesTraversalResult & result )
	{
		auto traverseData = result.getMember< AtmosphereTraverseData >( "atmosphereData" );

		auto const globalLuminance = m_writer.declLocale< sdw::Vec3 >( "globalLuminance"
			, m_atmosphere.settings.illuminanceIsOne
				// When building the scattering factor, we assume light illuminance is 1 to compute a transfert function relative to identity illuminance of 1.
				// This make the scattering factor independent of the light. It is now only linked to the atmosphere properties.
				? vec3( 1.0_f )
				: m_atmosphere.getSunIlluminance() );

		// Mie and Raylegh phases
		auto wi = m_writer.declLocale( "wi", traverseData.sunDirection );
		auto wo = m_writer.declLocale( "wo", ray.direction );
		auto cosTheta = m_writer.declLocale( "cosTheta", dot( wi, wo ) );
		// negate cosTheta because due to worldDir being a "in" direction. 
		auto miePhaseValue = m_writer.declLocale< sdw::Float >( "miePhaseValue"
			, m_atmosphere.hgPhase( m_atmosphere.getMiePhaseFunctionG(), -cosTheta ) );
		auto rayleighPhaseValue = m_writer.declLocale< sdw::Float >( "rayleighPhaseValue"
			, m_atmosphere.rayleighPhase( cosTheta ) );

		result.inscatter += traverseData.computeScattering( rayleighPhaseValue, miePhaseValue, globalLuminance );
	}

	c3ds::VolumeTraversalFunc const & AtmosphereVolumeTraversal::getTraversalFunc()
	{
		if ( !m_traverse )
		{
			m_traverse = m_writer.implementFunction< sdw::Void >( "atm_traverseVolume"
				, [this]( c3ds::Volume const & volume
					, Ray const & ray
					, sdw::Float const & sample
					, sdw::Float const & t
					, sdw::Float const & dt
					, sdw::Vec3 const & pos
					, c3ds::VolumesTraversalResult result )
				{
					auto traverseData = result.getMember< AtmosphereTraverseData >( "atmosphereData" );

					auto rayToSun = m_writer.declLocale( "rayToSun", Ray{ pos, traverseData.sunDirection } );
					auto medium = m_writer.declLocale( "medium", m_atmosphere.sampleMediumRGB( rayToSun.origin ) );
					auto sampleOpticalDepth = m_writer.declLocale( "sampleOpticalDepth", medium.extinction() * dt );
					auto sampleTransmittance = m_writer.declLocale( "sampleTransmittance", exp( -sampleOpticalDepth ) );

					auto pHeight = m_writer.declLocale( "pHeight", length( rayToSun.origin ) );
					auto upVector = m_writer.declLocale( "upVector", rayToSun.origin / pHeight );
					auto sunZenithCosAngle = m_writer.declLocale( "sunZenithCosAngle", dot( rayToSun.direction, upVector ) );
					auto trUv = m_writer.declLocale( "trUv"
						, m_atmosphere.lutTransmittanceParamsToUv( pHeight, sunZenithCosAngle ) );
					auto transmittanceToSun = m_writer.declLocale( "transmittanceToSun"
						, ( m_atmosphere.transmittanceTexture
							? m_atmosphere.transmittanceTexture->lod( trUv, 0.0_f ).rgb()
							: vec3( 0.0_f ) ) );

					auto planetO = m_writer.declLocale( "planetO", vec3( 0.0_f, 0.0f, 0.0f ) );
					auto planetShadow = m_writer.declLocale( "planetShadow", m_atmosphere.getPlanetShadow( rayToSun, planetO, upVector ) );
					// Dual scattering for multi scattering
					auto multiScatteredLuminance = m_writer.declLocale( "multiScatteredLuminance"
						, ( m_atmosphere.hasMultiscattering()
							? m_atmosphere.getMultipleScattering( pHeight, sunZenithCosAngle )
							: vec3( 0.0_f ) ) );

					// See slide 28 at http://www.frostbite.com/2015/08/physically-based-unified-volumetric-rendering-in-frostbite/
					traverseData.rayleighSingleScatter += planetShadow * m_atmosphere.integrateInscatter( transmittanceToSun * medium.scatteringRay()
						, result.transmittance, sampleTransmittance, medium.extinction() );
					traverseData.mieSingleScatter += planetShadow * m_atmosphere.integrateInscatter( transmittanceToSun * medium.scatteringMie()
						, result.transmittance, sampleTransmittance, medium.extinction() );
					traverseData.rayMieMultiScatter += ( m_atmosphere.hasMultiscattering()
						? m_atmosphere.integrateInscatter( multiScatteredLuminance * medium.scattering()
							, result.transmittance, sampleTransmittance, medium.extinction() )
						: vec3( 0.0_f ) );
					result.transmittance *= sampleTransmittance;
				}
				, c3ds::InVolume{ m_writer, "volume" }
				, c3ds::InRay{ m_writer, "ray" }
				, sdw::InFloat{ m_writer, "sample" }
				, sdw::InFloat{ m_writer, "t" }
				, sdw::InFloat{ m_writer, "dt" }
				, sdw::InVec3{ m_writer, "pos" }
				, c3ds::InOutVolumesTraversalResult{ m_writer, "result", m_volumeShaders } );
		}

		return m_traverse;
	}

	c3ds::VolumeStepFunc const & AtmosphereVolumeTraversal::getStepFunc()
	{
		if ( !m_step )
		{
			m_step = m_writer.implementFunction< sdw::Void >( "atm_stepInVolume"
				, [this]( c3ds::Volume const & volume
					, c3ds::Ray const & ray
					, sdw::Float const & sample
					, sdw::Float t
					, sdw::Float dt
					, c3ds::VolumesTraversalResult const & traversal )
				{
					auto sampleSegmentT = 0.3_f;
					auto result = m_writer.declLocale( "result", 0.0_f );
					auto traverseData = traversal.getMember< AtmosphereTraverseData >( "atmosphereData" );

					if ( m_atmosphere.hasVariableSampleCount() )
					{
						// More expensive but artifact free
						auto t0 = m_writer.declLocale( "t0", ( sample ) / traverseData.sampleCountFloor );
						auto t1 = m_writer.declLocale( "t1", ( sample + 1.0_f ) / traverseData.sampleCountFloor );
						// Non linear distribution of sample within the range.
						t0 = t0 * t0;
						t1 = t1 * t1;
						// Make t0 and t1 world space distances.
						t0 = traverseData.tMaxFloor * t0;

						sdwIF( m_writer, t1 > 1.0_f )
						{
							dt = volume.end - t0;
							//	t1 = tMaxFloor;	// this reveal depth slices
						}
						sdwELSE
						{
							dt = sdw::fma( traverseData.tMaxFloor, t1, -t0 );
						}
						sdwFI

						//t = t0 + (t1 - t0) * (whangHashNoise(pixPos.x, pixPos.y, gFrameId * 1920 * 1080)); // With dithering required to hide some sampling artefact relying on TAA later? This may even allow volumetric shadow?
						t = sdw::fma( dt, sampleSegmentT, t0 );
					}
					else
					{
						// Exact difference, important for accuracy of multiple scattering
						auto newT = m_writer.declLocale( "newT", volume.end * ( sample + sampleSegmentT ) / traverseData.sampleCount );
						dt = newT - t;
						t = newT;
					}
				}
				, c3ds::InVolume{ m_writer, "volume" }
				, c3ds::InOutRay{ m_writer, "ray" }
				, sdw::InFloat{ m_writer, "sample" }
				, sdw::InOutFloat{ m_writer, "t" }
				, sdw::InOutFloat{ m_writer, "dt" }
				, c3ds::InOutVolumesTraversalResult{ m_writer, "traversal", m_volumeShaders } );
		}

		return m_step;
	}

	sdw::RetFloat AtmosphereVolumeTraversal::getSamplesCount( sdw::Float const & range )const
	{
		return mix( m_atmosphere.getRayMarchMinSPP()
			, m_atmosphere.getRayMarchMaxSPP()
			, clamp( range * 0.01_f, 0.0_f, 1.0_f ) );
	}

	sdw::RetFloat AtmosphereVolumeTraversal::listVolumes( c3ds::Ray const & pray
		, sdw::Float const & pobjectId
		, sdw::Float const & plinearDepth
		, sdw::Vec2 const & ppixPos
		, c3ds::Volumes & pvolumes )
	{
		if ( !m_listVolumes )
		{
			m_listVolumes = m_writer.implementFunction< sdw::Float >( "atm_listVolumes"
				, [this]( Ray const & ray
					, sdw::Float const & objectId
					, sdw::Float const & linearDepth
					, sdw::Vec2 const & pixPos
					, sdw::Array< c3ds::Volume > volumes
					, sdw::UInt volumeCount )
				{
					auto tMax = m_writer.declLocale( "tMax", 0.0_f );

					sdwIF( m_writer, m_atmosphere.moveToTopAtmosphere( ray ) && m_atmosphere.moveToBottomAtmosphere( ray ) )
					{
						// Compute next intersection with atmosphere or ground
						auto planetO = m_writer.declLocale( "planetO", vec3( 0.0_f, 0.0f, 0.0f ) );
						auto tBottom = m_writer.declLocale( "tBottom", m_atmosphere.raySphereIntersectNearest( ray, planetO, m_atmosphere.getPlanetRadius() ) );
						auto tTop = m_writer.declLocale( "tTop", m_atmosphere.raySphereIntersectNearest( ray, planetO, m_atmosphere.getAtmosphereRadius() ) );

						sdwIF( m_writer, tBottom.valid() || tTop.valid() )
						{
							sdwIF( m_writer, !tBottom.valid() )
							{
								tMax = tTop.t();
							}
							sdwELSE
							{
								sdwIF( m_writer, tTop.t() > 0.0_f )
								{
									tMax = min( tTop.t(), tBottom.t() );
								}
								sdwFI
							}
							sdwFI

							sdwIF( m_writer, linearDepth > 0.0f )
							{
								auto targetExtent = sdw::vec2( float( m_atmosphere.transmittanceExtent.width ), float( m_atmosphere.transmittanceExtent.height ) );
								auto depthBufferWorldPos = m_writer.declLocale( "depthBufferWorldPos"
									, m_atmosphere.getWorldPos( linearDepth, pixPos, targetExtent ) );
								auto tDepth = m_writer.declLocale( "tDepth"
									, length( depthBufferWorldPos - ray.origin ) ); // apply planet offset to go back to origin as top of planet mode. 

								sdwIF( m_writer, tDepth < tMax )
								{
									tMax = tDepth;
								}
								sdwFI
							}
							sdwFI

							tMax = min( tMax, 9000000.0_f );
							auto samples = m_writer.declLocale( "samples", getSamplesCount( tMax ) );
							volumes[volumeCount] = c3ds::Volume{ volumeCount, sdw::UInt{ getVolumeType() }
								, 0.0_f, tMax
								, tMax / m_writer.cast< sdw::Float >( samples )
								, samples };
							tMax = volumes[volumeCount++].end;
						}
						sdwFI
					}
					sdwFI

					m_writer.returnStmt( tMax );
				}
				, c3ds::InOutRay{ m_writer, "ray" }
				, sdw::InFloat{ m_writer, "objectId" }
				, sdw::InFloat{ m_writer, "linearDepth" }
				, sdw::InVec2{ m_writer, "pixPos" }
				, sdw::InOutParam< sdw::Array< c3ds::Volume > >{ m_writer, "volumes", c3ds::MaxVolumeCount }
				, sdw::InOutUInt{ m_writer, "volumeCount" } );

		}

		m_listVolumes( pray, pobjectId, plinearDepth, ppixCoord, pvolumes.getVolumes(), pvolumes.getCount() );
	}

	//************************************************************************************************

	c3d::String const CloudsVolumePlugin::TypeName = cuT( "c3d.atmosphere" );

	CloudsVolumePlugin::Shader::Shader( sdw::ShaderWriter & writer
		, c3ds::VolumeShaders const & volumeShaders
		, c3d::Extent2D targetExtent
		, bool hasDepth
		, uint32_t & binding )
		: VolumeComponentShader{ writer, volumeShaders }
		, m_depthMap{ m_writer.declCombinedImg< sdw::CombinedImage2DRgba32 >( "depthMap", details::getNextBinding( binding ), 0u, hasDepth ) }
		, m_atmosphereData{ details::createUniformBufferData< AtmosphereData, AtmosphereScatteringUbo >( m_writer, details::getNextBinding( binding ) ) }
		, m_cameraData{ details::createUniformBufferData< CameraData, CameraUbo >( m_writer, details::getNextBinding( binding ) ) }
		, m_atmosphere{ m_writer, m_atmosphereData
			, AtmosphereModel::Settings{ c3d::Length::fromUnit( 1.0f, volumeShaders.getEngine().getLengthUnit() ) }
				.setCameraData( &m_cameraData )
				.setVariableSampleCount( true )
				.setMieRayPhase( true )
				.setMultiScatApprox( true )
			, c3d::move( targetExtent ) }
		, m_scattering{ m_writer, m_atmosphere
			, ScatteringModel::Settings{}
				.setNeedsMultiscatter( true )
				.setBloomSunDisk( true )
			, binding, 0u }
		, m_atmosphereTraversal{ m_writer, m_volumeShaders, m_atmosphere }
	{
	}

	void CloudsVolumePlugin::Shader::fillType( sdw::type::BaseStruct & type )const
	{
		m_atmosphereTraversal.fillType( type );
	}

	void CloudsVolumePlugin::Shader::fillInit( sdw::type::BaseStruct & type
		, sdw::expr::ExprList & inits )const
	{
		m_atmosphereTraversal.fillInit( type, inits );
	}

	void CloudsVolumePlugin::Shader::registerVolumes( c3ds::Volumes & volumes )
	{
		m_atmosphereTraversal.registerVolumes( volumes );
	}

	void CloudsVolumePlugin::Shader::initialise( sdw::Vec2 const & pixelCoord, c3ds::Ray & ray
		, c3ds::Volumes & volumes, c3ds::VolumesTraversalResult & result )
	{
		auto sceneUv = m_writer.declLocale( "sceneUv"
			, pixelCoord / result.renderSize );
		auto depthObj = m_writer.declLocale( "depthObj"
			, m_depthMap.isEnabled() ? m_depthMap.lod( vec2( sceneUv.x(), 1.0_f - sceneUv.y() ), 0.0_f ) : vec4( -1.0_f, -1.0_f, 0.0_f, 0.0_f ) );
		if ( m_depthMap.isEnabled() && m_atmosphere.settings.length.lengthUnit() != c3d::LengthUnit::eKilometre )
			depthObj.g() *= m_atmosphere.settings.length.kilometres();

		m_atmosphereTraversal.initialise( pixelCoord, ray, volumes, result, depthObj );
	}

	void CloudsVolumePlugin::Shader::finalise( c3ds::Ray const & ray, c3ds::VolumesTraversalResult & result )
	{
		m_atmosphereTraversal.finalise( ray, result );
	}

	//************************************************************************************************
}
