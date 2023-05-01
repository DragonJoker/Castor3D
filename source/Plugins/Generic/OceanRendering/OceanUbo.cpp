#include "OceanRendering/OceanUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/UniformBufferPool.hpp>

#include <CastorUtils/Graphics/Size.hpp>

namespace ocean
{
	//************************************************************************************************

	OceanUbo::OceanUbo( castor3d::RenderDevice const & device )
		: m_device{ device }
		, m_ubo{ device.uboPool->getBuffer< Configuration >( 0u ) }
	{
	}

	OceanUbo::~OceanUbo()
	{
		m_device.uboPool->putBuffer( m_ubo );
	}

	void OceanUbo::cpuUpdate( OceanUboConfiguration const & config )
	{
		auto & data = m_ubo.getData();
		data = config;
	}

	//************************************************************************************************
}
