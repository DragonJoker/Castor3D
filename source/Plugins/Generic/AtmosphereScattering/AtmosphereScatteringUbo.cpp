#include "AtmosphereScattering/AtmosphereScatteringUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/SceneNode.hpp>

#include <CastorUtils/Graphics/Size.hpp>

#include <ShaderWriter/Source.hpp>

namespace atmosphere_scattering
{
	//*********************************************************************************************

	c3d::MbString const AtmosphereScatteringUbo::Buffer = "C3D_ATM_Atmosphere";
	c3d::MbString const AtmosphereScatteringUbo::Data = "d";

	AtmosphereScatteringUbo::AtmosphereScatteringUbo( c3d::RenderDevice const & device )
		: UboT{ device }
	{
	}

	std::tuple< c3d::Point3f, c3d::Vector3f, c3d::Point3f > AtmosphereScatteringUbo::cpuUpdate( Configuration const & config
		, c3d::SceneNode const & sunNode
		, c3d::SceneNode const & planetNode )
	{
		auto const & engine = *sunNode.getScene()->getEngine();

		auto sunDirection = c3d::Point3f{ 0, 0, 1 };
		sunNode.getDerivedOrientation().transform( sunDirection, sunDirection );
		sunDirection = -c3d::point::getNormalised( sunDirection );

		auto planetPosition = c3d::Vector3f::fromUnit( planetNode.getDerivedPosition(), engine.getLengthUnit() );
		auto planetPositionKm = planetPosition.kilometres();

		auto mieAbsorption = config.mieExtinction - config.mieScattering;
		mieAbsorption->x = std::max( 0.0f, mieAbsorption->x );
		mieAbsorption->y = std::max( 0.0f, mieAbsorption->y );
		mieAbsorption->z = std::max( 0.0f, mieAbsorption->z );

		auto & data = getNCData();
		data = config;
		data.sunIlluminance *= data.sunIlluminanceScale;
		data.sunDirection->x = sunDirection->x;
		data.sunDirection->y = sunDirection->y;
		data.sunDirection->z = sunDirection->z;

		data.planetPosition->x = planetPositionKm->x;
		data.planetPosition->y = planetPositionKm->y;
		data.planetPosition->z = planetPositionKm->z;

		data.mieAbsorption->x = std::max( 0.0f, mieAbsorption->x );
		data.mieAbsorption->y = std::max( 0.0f, mieAbsorption->y );
		data.mieAbsorption->z = std::max( 0.0f, mieAbsorption->z );

		return { sunDirection, planetPosition, mieAbsorption };
	}

	//************************************************************************************************
}
