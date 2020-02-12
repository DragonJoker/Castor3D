#include "Castor3D/Shader/Ubos/HdrConfigUbo.hpp"

#include "Castor3D/Engine.hpp"
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
			m_ubo = makeUniformBuffer< HdrConfig >( *m_engine.getRenderSystem()
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
