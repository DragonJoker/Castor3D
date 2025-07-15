#include "AtmosphereScattering/AtmosphereBackgroundModel.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Shader/Shaders/GlslBlendComponents.hpp>
#include <Castor3D/Shader/Shaders/GlslDebugOutput.hpp>

#include <ShaderWriter/Writer.hpp>
#include <ShaderWriter/BaseTypes/Float.hpp>
#include <ShaderWriter/Intrinsics/Intrinsics.hpp>

namespace atmosphere_scattering
{
	namespace model
	{
		template< typename DataT >
		DataT getData( sdw::UniformBuffer & ubo
			, c3d::MbString const & name
			, bool enabled = true )
		{
			auto result = ubo.declMember< DataT >( name, enabled );
			ubo.end();
			return result;
		}
	}

	c3d::String const AtmosphereBackgroundModel::Name = cuT( "c3d.atmosphere" );
	c3d::String const AtmosphereBackgroundModel::PluginType = cuT( "atmospheric_scattering" );
	c3d::MbString const AtmosphereBackgroundModel::PluginName = "Atmospheric Scattering";

	AtmosphereBackgroundModel::AtmosphereBackgroundModel( c3d::Engine const & engine
		, sdw::ShaderWriter & writer
		, c3d::shader::Utils & utils
		, c3d::Extent2D targetSize
		, bool needsForeground
		, uint32_t & binding
		, uint32_t set )
		: c3d::shader::BackgroundModel{ writer, utils, c3d::move( targetSize ), true, false, false }
		, cameraBuffer{ writer.declUniformBuffer<>( CameraUbo::Buffer
			, binding++
			, set ) }
		, cameraData{ model::getData< CameraData >( cameraBuffer, CameraUbo::Data ) }
		, atmosphereBuffer{ writer.declUniformBuffer<>( AtmosphereScatteringUbo::Buffer
			, binding++
			, set ) }
		, atmosphereData{ model::getData< AtmosphereData >( atmosphereBuffer, AtmosphereScatteringUbo::Data ) }
		, cloudsBuffer{ writer.declUniformBuffer<>( CloudsUbo::Buffer
			, binding++
			, set
			, sdw::type::MemoryLayout::eStd140
			, needsForeground ) }
		, cloudsData{ model::getData< CloudsData >( cloudsBuffer, CloudsUbo::Data, needsForeground ) }
		, atmosphere{ m_writer
			, atmosphereData
			, AtmosphereModel::Settings{ c3d::Length::fromUnit( 1.0f, engine.getLengthUnit() ) }
				.setCameraData( &cameraData )
				.setVariableSampleCount( true )
				.setMieRayPhase( true )
				.setMultiScatApprox( true )
			, { 256u, 64u } }
		, scattering{ m_writer
			, atmosphere
			, ScatteringModel::Settings{}
				.setNeedsMultiscatter( true )
				.setFastAerialPerspective( true )
			, binding
			, set }
		, cloudsResult{ writer.declCombinedImg< sdw::CombinedImage2DRgba32 >( "c3d_atmbg_cloudsResult"
			, binding++
			, set
			, needsForeground ) }
	{
	}

	c3d::shader::BackgroundModelPtr AtmosphereBackgroundModel::create( c3d::Engine const & engine
		, sdw::ShaderWriter & writer
		, c3d::shader::Utils & utils
		, c3d::Extent2D targetSize
		, bool needsForeground
		, uint32_t & binding
		, uint32_t set )
	{
		return c3d::makeRawUnique< AtmosphereBackgroundModel >( engine
			, writer
			, utils
			, c3d::move( targetSize )
			, needsForeground
			, binding
			, set );
	}

	sdw::RetVec3 AtmosphereBackgroundModel::computeSpecularReflection( sdw::Vec3 const & pwsNormal
		, sdw::Vec3 const & pwsPosition
		, sdw::Vec3 const & pV
		, sdw::Float const & proughness )
	{
		if ( !m_computeSpecularReflection )
		{
			m_computeSpecularReflection = m_writer.implementFunction< sdw::Vec3 >( "c3d_atmbg_computeSpecularReflection"
				, [this]( sdw::Vec3 const & wsNormal
					, sdw::Vec3 const & wsPosition
					, sdw::Vec3 const & wsEyeDir )
				{
					auto fragSize = vec2( sdw::Float{ float( getTargetSize().width ) }
						, float( getTargetSize().height ) );
					auto skyContrib = m_writer.declLocale( "skyContrib"
						, max( vec3( 0.0_f )
							, scattering.getSkyRadiance( wsNormal
								, wsPosition
								, wsEyeDir
								, fragSize ) ) );
					m_writer.returnStmt( skyContrib );
				}
				, sdw::InVec3{ m_writer, "wsNormal" }
				, sdw::InVec3{ m_writer, "wsPosition" }
				, sdw::InVec3{ m_writer, "wsEyeDir" } );
		}
		return m_computeSpecularReflection( pwsNormal
			, pwsPosition
			, pV );
	}

	void AtmosphereBackgroundModel::applyVolume( sdw::Vec2 const pfragCoord
		, sdw::Float const plinearDepth
		, sdw::Vec2 const ptargetSize
		, sdw::Vec2 const pcameraPlanes
		, sdw::Vec4 & poutput )
	{
		if ( !cloudsBuffer.isEnabled() )
		{
			return;
		}

		if ( !m_computeVolume )
		{
			m_computeVolume = m_writer.implementFunction< sdw::Void >( "c3d_atmbg_computeVolume"
				, [&]( sdw::Vec2 const & fragCoord
					, sdw::Float linearDepth
					, sdw::Vec2 const & targetSize
					, sdw::Vec4 output )
				{
					auto uv = m_writer.declLocale( "uv"
						, fragCoord / targetSize );
					auto ray = m_writer.declLocale( "ray"
						, atmosphere.castRay( vec2( uv.x(), 1.0_f - uv.y() ) ) );
					linearDepth = linearDepth * atmosphere.settings.length.kilometres();

					auto clampOuter = m_writer.declLocale( "clampOuter"
						, 0_b );
					auto interGround = m_writer.declLocale( "interGround"
						, atmosphere.raySphereIntersectNearest( ray
							, atmosphere.getPlanetRadius() ) );
					auto wasHittingGround = m_writer.declLocale( "wasHittingGround"
						, interGround.valid() );

					sdwIF( m_writer, ( ( !interGround.valid() ) || linearDepth < interGround.t() ) )
					{
						interGround.t() = linearDepth;
						interGround.point() = ray.step( linearDepth );
						interGround.valid() = 1_b;
						clampOuter = 1_b;
					}
					sdwFI

					auto interInnerN = m_writer.declLocale( "interInnerN", Intersection{ m_writer } );
					auto interInnerF = m_writer.declLocale( "interInnerF", Intersection{ m_writer } );
					auto interInnerCount = m_writer.declLocale( "interInnerCount"
						, atmosphere.raySphereIntersect( ray
							, cloudsData.innerRadius() + atmosphere.getPlanetRadius()
							, interGround
							, 0_b
							, interInnerN
							, interInnerF ) );
					auto interOuterN = m_writer.declLocale( "interOuterN", Intersection{ m_writer } );
					auto interOuterF = m_writer.declLocale( "interOuterF", Intersection{ m_writer } );
					auto interOuterCount = m_writer.declLocale( "interOuterCount"
						, atmosphere.raySphereIntersect( ray
							, cloudsData.outerRadius() + atmosphere.getPlanetRadius()
							, interGround
							, clampOuter
							, interOuterN
							, interOuterF ) );
					auto crossesInner = m_writer.declLocale( "crossesInner"
						, interInnerN.valid() && linearDepth > interInnerN.t() );
					auto crossesOuter = m_writer.declLocale( "crossesOuter"
						, interOuterN.valid() && linearDepth >= interOuterN.t() );

					sdwIF( m_writer, crossesInner || crossesOuter )
					{
						auto clouds = m_writer.declLocale( "clouds"
							, cloudsResult.lod( uv, 0.0_f ) );
						output = mix( output, clouds, vec4( clouds.a() ) );
					}
					sdwFI
				}
				, sdw::InVec2{ m_writer, "fragCoord" }
				, sdw::InFloat{ m_writer, "linearDepth" }
				, sdw::InVec2{ m_writer, "targetSize" }
				, sdw::InOutVec4{ m_writer, "output" } );
		}

		sdwIF( m_writer, cloudsData.coverage() > 0.0_f
			&& plinearDepth < pcameraPlanes.y() )
		{
			m_computeVolume( pfragCoord
				, plinearDepth
				, ptargetSize
				, poutput );
		}
		sdwFI
	}

	sdw::Vec3 AtmosphereBackgroundModel::getSunRadiance( sdw::Vec3 const & psunDir )
	{
		return scattering.getSunRadiance( psunDir );
	}
}
