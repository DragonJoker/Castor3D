#include "AtmosphereScattering/AtmosphereBackgroundModel.hpp"

#include <ShaderWriter/BaseTypes/Float.hpp>
#include <ShaderWriter/Intrinsics/Intrinsics.hpp>

namespace atmosphere_scattering
{
	castor::String const AtmosphereBackgroundModel::Name = cuT( "c3d.atmosphere" );

	AtmosphereBackgroundModel::AtmosphereBackgroundModel( sdw::ShaderWriter & writer
		, castor3d::shader::Utils & utils
		, uint32_t & binding
		, uint32_t set )
		: castor3d::shader::BackgroundModel{ writer, utils }
	{
	}

	castor3d::shader::BackgroundModelPtr AtmosphereBackgroundModel::create( sdw::ShaderWriter & writer
		, castor3d::shader::Utils & utils
		, uint32_t & binding
		, uint32_t set )
	{
		return std::make_unique< AtmosphereBackgroundModel >( writer, utils, binding, set );
	}

	sdw::Vec3 AtmosphereBackgroundModel::computeReflections( sdw::Vec3 const & wsIncident
		, sdw::Vec3 const & wsNormal
		, castor3d::shader::LightMaterial const & material
		, sdw::CombinedImage2DRg32 const & brdf )
	{
		return vec3( 0.0_f );
	}

	sdw::Vec3 AtmosphereBackgroundModel::computeRefractions( sdw::Vec3 const & wsIncident
		, sdw::Vec3 const & wsNormal
		, sdw::Float const & refractionRatio
		, sdw::Vec3 const & transmission
		, castor3d::shader::LightMaterial const & material )
	{
		return vec3( 0.0_f );
	}

	sdw::Void AtmosphereBackgroundModel::mergeReflRefr( sdw::Vec3 const & wsIncident
		, sdw::Vec3 const & wsNormal
		, sdw::Float const & refractionRatio
		, sdw::Vec3 const & transmission
		, castor3d::shader::LightMaterial const & material
		, sdw::Vec3 & reflection
		, sdw::Vec3 & refraction )
	{
		return sdw::Void{};
	}
}
