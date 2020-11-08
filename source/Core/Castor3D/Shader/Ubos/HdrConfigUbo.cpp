#include "Castor3D/Shader/Ubos/HdrConfigUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/ToneMapping/HdrConfig.hpp"

namespace castor3d
{
	uint32_t const HdrConfigUbo::BindingPoint = 10u;
	castor::String const HdrConfigUbo::BufferHdrConfig = cuT( "HdrConfig" );
	castor::String const HdrConfigUbo::Exposure = cuT( "c3d_exposure" );
	castor::String const HdrConfigUbo::Gamma = cuT( "c3d_gamma" );

	HdrConfigUbo::HdrConfigUbo( Engine & engine )
		: m_engine{ engine }
	{
		if ( engine.getRenderSystem()->hasCurrentRenderDevice() )
		{
			initialise( engine.getRenderSystem()->getCurrentRenderDevice() );
		}
	}

	HdrConfigUbo::~HdrConfigUbo()
	{
	}

	void HdrConfigUbo::initialise( RenderDevice const & device )
	{
		if ( !m_ubo )
		{
			m_ubo = device.uboPools->getBuffer< Configuration >( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
		}
	}

	void HdrConfigUbo::cleanup( RenderDevice const & device )
	{
		if ( m_ubo )
		{
			device.uboPools->putBuffer( m_ubo );
		}
	}

	void HdrConfigUbo::cpuUpdate( HdrConfig const & config )
	{
		CU_Require( m_ubo );
		m_ubo.getData().exposure = config.exposure;
		m_ubo.getData().gamma = config.gamma;
	}
}
