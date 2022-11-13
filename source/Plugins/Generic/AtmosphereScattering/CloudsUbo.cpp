#include "AtmosphereScattering/CloudsUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/UniformBufferPool.hpp>
#include <Castor3D/Scene/SceneNode.hpp>

#include <CastorUtils/Graphics/Size.hpp>

#include <ShaderWriter/Source.hpp>

namespace atmosphere_scattering
{
	castor::String const CloudsUbo::Buffer = cuT( "C3D_ATM_Clouds" );
	castor::String const CloudsUbo::Data = cuT( "d" );

	CloudsUbo::CloudsUbo( castor3d::RenderDevice const & device
		, bool & dirty )
		: m_device{ device }
		, m_ubo{ device.uboPool->getBuffer< Configuration >( 0u ) }
		, m_config{ dirty }
	{
	}

	CloudsUbo::~CloudsUbo()
	{
		m_device.uboPool->putBuffer( m_ubo );
	}

	void CloudsUbo::cpuUpdate( Configuration const & config
		, float totalTime )
	{
		m_config = config;
		auto & data = m_ubo.getData();
		data = config;
		data.crispiness *= 10.0f;
		data.time = totalTime;
		data.windDirection = castor::point::getNormalised( data.windDirection );
	}
}
