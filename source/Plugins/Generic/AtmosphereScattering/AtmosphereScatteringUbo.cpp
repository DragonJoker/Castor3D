#include "AtmosphereScattering/AtmosphereScatteringUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/UniformBufferPool.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/SceneNode.hpp>

#include <CastorUtils/Graphics/Size.hpp>

#include <ShaderWriter/Source.hpp>

namespace atmosphere_scattering
{
	//*********************************************************************************************

	castor::MbString const AtmosphereScatteringUbo::Buffer = "C3D_ATM_Atmosphere";
	castor::MbString const AtmosphereScatteringUbo::Data = "d";

	AtmosphereScatteringUbo::AtmosphereScatteringUbo( castor3d::RenderDevice const & device
		, bool & dirty )
		: m_device{ device }
		, m_ubo{ device.uboPool->getBuffer< Configuration >( 0u ) }
		, m_dirty{ dirty }
		, m_config{ m_dirty }
		, m_sunDirection{ m_dirty }
		, m_planetPosition{ m_dirty }
		, m_mieAbsorption{ m_dirty }
	{
	}

	AtmosphereScatteringUbo::~AtmosphereScatteringUbo()
	{
		m_device.uboPool->putBuffer( m_ubo );
	}

	castor::Pair< castor::Point3f, castor::Vector3f > AtmosphereScatteringUbo::cpuUpdate( Configuration const & config
		, castor3d::SceneNode const & sunNode
		, castor3d::SceneNode const & planetNode )
	{
		auto & engine = *sunNode.getScene()->getEngine();

		auto sunDirection = castor::Point3f{ 0, 0, 1 };
		sunNode.getDerivedOrientation().transform( sunDirection, sunDirection );
		sunDirection = -castor::point::getNormalised( sunDirection );
		m_sunDirection = { sunDirection };

		auto planetPosition = castor::Vector3f::fromUnit( planetNode.getDerivedPosition(), engine.getLengthUnit() );
		m_planetPosition = planetPosition.kilometres();

		auto mieAbsorption = config.mieExtinction - config.mieScattering;
		m_mieAbsorption = castor::Point3f{ std::max( 0.0f, mieAbsorption->x )
			, std::max( 0.0f, mieAbsorption->y )
			, std::max( 0.0f, mieAbsorption->z ) };

		m_config = config;

		if ( m_dirty )
		{
			auto & data = m_ubo.getData();
			data = config;
			data.sunIlluminance *= data.sunIlluminanceScale;
			data.sunDirection->x = ( *m_sunDirection )->x;
			data.sunDirection->y = ( *m_sunDirection )->y;
			data.sunDirection->z = ( *m_sunDirection )->z;

			data.planetPosition->x = ( *m_planetPosition )->x;
			data.planetPosition->y = ( *m_planetPosition )->y;
			data.planetPosition->z = ( *m_planetPosition )->z;

			data.mieAbsorption->x = std::max( 0.0f, ( *m_mieAbsorption )->x );
			data.mieAbsorption->y = std::max( 0.0f, ( *m_mieAbsorption )->y );
			data.mieAbsorption->z = std::max( 0.0f, ( *m_mieAbsorption )->z );
		}

		return { sunDirection, planetPosition };
	}

	//************************************************************************************************
}
