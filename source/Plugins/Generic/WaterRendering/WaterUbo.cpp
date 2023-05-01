#include "WaterRendering/WaterUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/UniformBufferPool.hpp>

#include <CastorUtils/Graphics/Size.hpp>

namespace water
{
	castor::String const WaterUbo::Buffer = cuT( "Water" );
	castor::String const WaterUbo::Data = cuT( "c3d_waterData" );

	WaterUbo::WaterUbo( castor3d::RenderDevice const & device )
		: m_device{ device }
		, m_ubo{ device.uboPool->getBuffer< Configuration >( 0u ) }
	{
	}

	WaterUbo::~WaterUbo()
	{
		m_device.uboPool->putBuffer( m_ubo );
	}

	void WaterUbo::cpuUpdate( WaterUboConfiguration const & config )
	{
		auto & data = m_ubo.getData();
		data = config;
	}
}
