#include "WavesUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/UniformBufferPool.hpp>

namespace waves
{
	WavesUbo::WavesUbo( castor3d::RenderDevice const & device )
		: m_device{ device }
		, m_ubo{ device.uboPool->getBuffer< Configuration >( 0u ) }
	{
	}

	WavesUbo::~WavesUbo()
	{
		m_device.uboPool->putBuffer( m_ubo );
	}

	void WavesUbo::cpuUpdate( WavesConfiguration const & config )
	{
		auto & data = m_ubo.getData();
		data = config;
	}
}
