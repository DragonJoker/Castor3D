#include "Castor3D/Shader/Ubos/RsmConfigUbo.hpp"

#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Scene/Scene.hpp"

namespace c3d
{
	//*********************************************************************************************

	RsmConfigUbo::RsmConfigUbo( RenderDevice const & device )
		: m_device{ device }
		, m_ubo{ m_device.uboPool->getBuffer< Configuration >( MemoryPropertyFlags::eNone ) }
	{
	}

	RsmConfigUbo::~RsmConfigUbo()noexcept
	{
		m_device.uboPool->putBuffer( m_ubo );
	}

	void RsmConfigUbo::cpuUpdate( RsmConfig const & rsmConfig
		, uint32_t index )
	{
		CU_Require( m_ubo );
		auto & rsmData = m_ubo.getData();
		rsmData.intensity = *rsmConfig.intensity;
		rsmData.maxRadius = *rsmConfig.maxRadius;
		rsmData.sampleCount = rsmConfig.sampleCount.value().value();
		rsmData.index = int32_t( index );
	}

	//*********************************************************************************************
}
