#include "HdrConfigUbo.hpp"

#include "Engine.hpp"
#include "HDR/HdrConfig.hpp"
#include "Render/RenderSystem.hpp"

#include <Buffer/UniformBuffer.hpp>

using namespace castor;

namespace castor3d
{
	String const HdrConfigUbo::BufferHdrConfig = cuT( "HdrConfig" );
	String const HdrConfigUbo::Exposure = cuT( "c3d_exposure" );
	String const HdrConfigUbo::Gamma = cuT( "c3d_gamma" );

	HdrConfigUbo::HdrConfigUbo( Engine & engine )
		: m_engine{ engine }
	{
		if ( engine.getRenderSystem()->hasCurrentDevice() )
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
			auto & device = *m_engine.getRenderSystem()->getCurrentDevice();
			m_ubo = renderer::makeUniformBuffer< HdrConfig >( device
				, 1u
				, renderer::BufferTarget::eTransferDst
				, renderer::MemoryPropertyFlag::eHostVisible );
			m_ubo->upload();
		}
	}

	void HdrConfigUbo::cleanup()
	{
		m_ubo.reset();
	}

	void HdrConfigUbo::update( HdrConfig const & config )const
	{
		REQUIRE( m_ubo );
		m_ubo->getData( 0u ).setExposure( config.getExposure() );
		m_ubo->getData( 0u ).setGamma( config.getGamma() );
		m_ubo->upload();
	}
}
