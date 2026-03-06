#include "AtmosphereScattering/AtmosphereLightingModel.hpp"

#include "AtmosphereScattering/AtmosphereBackground.hpp"
#include "AtmosphereScattering/AtmosphereBackgroundModel.hpp"

#include <Castor3D/Shader/Shaders/GlslLightSurface.hpp>

#include <ShaderWriter/Source.hpp>

namespace atmosphere_scattering
{
	AtmosphereScatteringModel::AtmosphereScatteringModel( sdw::ShaderWriter & writer )
		: c3ds::ScatteringModel{ writer }
	{
	}

	void AtmosphereScatteringModel::initialiseBackground( c3ds::BackgroundModel & background
		, c3ds::Shadow & shadowModel )
	{
		if ( !atmosphereBackground )
		{
			atmosphereBackground = &static_cast< AtmosphereBackgroundModel & >( background );
			atmosphereBackground->atmosphere.settings.shadowMapEnabled = true;
			atmosphereBackground->atmosphere.shadows = &shadowModel;
		}
	}

	sdw::Vec3 AtmosphereScatteringModel::computeRadiance( c3ds::Light const & light
		, sdw::Vec3 const & lightDirection )const
	{
		return atmosphereBackground->getSunRadiance( lightDirection );
	}

	void AtmosphereScatteringModel::computeScattering( c3ds::LightingModel & lighting
		, c3ds::ShadowData const & shadows
		, sdw::Int const shadowMapIndex
		, sdw::Vec3 const & radiance
		, sdw::Float const & lightIntensity
		, c3ds::BlendComponents const & components
		, c3ds::LightSurface const & lightSurface
		, sdw::Vec3 output )
	{
		auto & writer = sdw::findWriterMandat( lightSurface, output );
		auto targetSize = vec2( sdw::Float{ float( atmosphereBackground->getTargetSize().width ) }
			, float( atmosphereBackground->getTargetSize().height ) );
		auto luminance = writer.declLocale< sdw::Vec4 >( "luminance", sdw::vec4( 0.0_f ) );
		auto transmittance = writer.declLocale< sdw::Vec4 >( "transmittance", sdw::vec4( 0.0_f ) );
		auto ray = writer.declLocale( "ray"
			, atmosphereBackground->getPixelTransLum( lightSurface.clipPosition().xy()
				, targetSize
				, lightSurface.clipPosition().z()
				, transmittance
				, luminance ) );
		output = luminance.xyz() / luminance.a();
		lighting.applyVolumetric( shadows
			, shadowMapIndex
			, lightIntensity
			, lightSurface
			, output
			, true /*multiply*/ );
	}

	c3ds::ScatteringModelPtr AtmosphereScatteringModel::create( sdw::ShaderWriter & writer )
	{
		return c3d::makeUniqueDerived< c3ds::ScatteringModel, AtmosphereScatteringModel >( writer );
	}
}
