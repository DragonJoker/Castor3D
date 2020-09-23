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
			initialise();
		}
	}

	HdrConfigUbo::~HdrConfigUbo()
	{
	}

	void HdrConfigUbo::initialise()
	{
		if ( !m_ubo )
		{
			auto & device = getCurrentRenderDevice( m_engine );
			m_ubo = device.uboPools->getBuffer< Configuration >( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
		}
	}

	void HdrConfigUbo::cleanup()
	{
		if ( m_ubo )
		{
			auto & device = getCurrentRenderDevice( m_engine );
			device.uboPools->putBuffer( m_ubo );
		}
	}

	void HdrConfigUbo::cpuUpdate( HdrConfig const & config )
	{
		CU_Require( m_ubo );
		m_ubo.getData().setExposure( config.getExposure() );
		m_ubo.getData().setGamma( config.getGamma() );
	}
}
