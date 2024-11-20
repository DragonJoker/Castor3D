#include "AtmosphereScattering/AtmosphereLightingModel.hpp"

#include "AtmosphereScattering/AtmosphereBackground.hpp"
#include "AtmosphereScattering/AtmosphereBackgroundModel.hpp"

#include <Castor3D/Shader/Shaders/GlslLightSurface.hpp>

#include <ShaderWriter/Source.hpp>

namespace atmosphere_scattering
{
	AtmosphereScatteringModel::AtmosphereScatteringModel( sdw::ShaderWriter & writer )
		: c3d::ScatteringModel{ writer }
	{
	}

	void AtmosphereScatteringModel::initialiseBackground( c3d::BackgroundModel & background
		, c3d::Shadow & shadowModel )
	{
		if ( !atmosphereBackground )
		{
			atmosphereBackground = &static_cast< AtmosphereBackgroundModel & >( background );
			atmosphereBackground->atmosphere.settings.shadowMapEnabled = true;
			atmosphereBackground->atmosphere.shadows = &shadowModel;
		}
	}

	sdw::Vec3 AtmosphereScatteringModel::computeRadiance( c3d::Light const & light
		, sdw::Vec3 const & lightDirection )const
	{
		return atmosphereBackground->getSunRadiance( lightDirection );
	}

	void AtmosphereScatteringModel::computeScattering( c3d::LightingModel & lighting
		, c3d::ShadowData const & shadows
		, sdw::Int const shadowMapIndex
		, sdw::Vec3 const & radiance
		, sdw::Float const & lightIntensity
		, c3d::BlendComponents const & components
		, c3d::LightSurface const & lightSurface
		, sdw::Vec3 output )
	{
		auto & writer = sdw::findWriterMandat( lightSurface, output );
		auto targetSize = vec2( sdw::Float{ float( atmosphereBackground->getTargetSize().width ) }
			, float( atmosphereBackground->getTargetSize().height ) );
		auto luminance = writer.declLocale< sdw::Vec4 >( "luminance" );
		auto transmittance = writer.declLocale< sdw::Vec4 >( "transmittance" );
		atmosphereBackground->getPixelTransLum( lightSurface.clipPosition().xy()
			, targetSize
			, lightSurface.clipPosition().z()
			, transmittance
			, luminance );
		output = luminance.xyz() / luminance.a();
		lighting.applyVolumetric( shadows
			, shadowMapIndex
			, lightIntensity
			, lightSurface
			, output
			, true /*multiply*/ );
	}

	c3d::ScatteringModelPtr AtmosphereScatteringModel::create( sdw::ShaderWriter & writer )
	{
		return castor::makeUniqueDerived< c3d::ScatteringModel, AtmosphereScatteringModel >( writer );
	}
}
