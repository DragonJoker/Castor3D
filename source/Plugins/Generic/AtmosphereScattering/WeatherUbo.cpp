#include "AtmosphereScattering/WeatherUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/UniformBufferPool.hpp>
#include <Castor3D/Scene/SceneNode.hpp>

#include <CastorUtils/Graphics/Size.hpp>

#include <ShaderWriter/Source.hpp>

namespace atmosphere_scattering
{
	c3d::MbString const WeatherUbo::Buffer = "Weather";
	c3d::MbString const WeatherUbo::Data = "c3d_weatherData";

	WeatherUbo::WeatherUbo( c3d::RenderDevice const & device
		, bool & dirty )
		: m_device{ device }
		, m_ubo{ device.uboPool->getBuffer< Configuration >( 0u ) }
		, m_config{ dirty }
	{
	}

	WeatherUbo::~WeatherUbo()
	{
		m_device.uboPool->putBuffer( m_ubo );
	}

	void WeatherUbo::cpuUpdate( Configuration const & config )
	{
		m_config = config;
		auto & data = m_ubo.getData();
		data = config;
	}
}
