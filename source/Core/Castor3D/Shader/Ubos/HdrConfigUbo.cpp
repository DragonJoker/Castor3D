#include "Castor3D/Shader/Ubos/HdrConfigUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBuffer.hpp"
#include "Castor3D/HDR/HdrConfig.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

using namespace castor;

namespace castor3d
{
	uint32_t const HdrConfigUbo::BindingPoint = 10u;
	String const HdrConfigUbo::BufferHdrConfig = cuT( "HdrConfig" );
	String const HdrConfigUbo::Exposure = cuT( "c3d_exposure" );
	String const HdrConfigUbo::Gamma = cuT( "c3d_gamma" );

	HdrConfigUbo::HdrConfigUbo( Engine & engine )
		: m_engine{ engine }
	{
		if ( engine.getRenderSystem()->getCurrentRenderDevice() )
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
			m_ubo = makeUniformBuffer< HdrConfig >( device
				, 1u
				, VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				, "HdrConfigUbo" );
			m_ubo->upload();
		}
	}

	void HdrConfigUbo::cleanup()
	{
		m_ubo.reset();
	}

	void HdrConfigUbo::update( HdrConfig const & config )const
	{
		CU_Require( m_ubo );
		m_ubo->getData( 0u ).setExposure( config.getExposure() );
		m_ubo->getData( 0u ).setGamma( config.getGamma() );
		m_ubo->upload();
	}
}
