#include "AtmosphereScattering/AtmosphereWeatherUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/UniformBufferPool.hpp>
#include <Castor3D/Scene/SceneNode.hpp>

#include <CastorUtils/Graphics/Size.hpp>

#include <ShaderWriter/Source.hpp>

namespace atmosphere_scattering
{
	castor::String const AtmosphereWeatherUbo::Buffer = cuT( "Weather" );
	castor::String const AtmosphereWeatherUbo::Data = cuT( "c3d_weatherData" );

	AtmosphereWeatherUbo::AtmosphereWeatherUbo( castor3d::RenderDevice const & device
		, bool & dirty )
		: m_device{ device }
		, m_ubo{ device.uboPool->getBuffer< Configuration >( 0u ) }
		, m_config{ dirty }
	{
	}

	AtmosphereWeatherUbo::~AtmosphereWeatherUbo()
	{
		m_device.uboPool->putBuffer( m_ubo );
	}

	void AtmosphereWeatherUbo::cpuUpdate( Configuration const & config
		, float totalTime )
	{
		m_config = config;
		auto & data = m_ubo.getData();
		data = config;
		data.time = totalTime;
	}
}
