#include "PickingUbo.hpp"

#include "Engine.hpp"
#include "Render/RenderSystem.hpp"

#include <Buffer/UniformBuffer.hpp>

using namespace castor;

namespace castor3d
{
	String const PickingUbo::BufferPicking = cuT( "Picking" );
	String const PickingUbo::DrawIndex = cuT( "c3d_drawIndex" );
	String const PickingUbo::NodeIndex = cuT( "c3d_nodeIndex" );

	PickingUbo::PickingUbo( Engine & engine )
		: m_engine{ engine }
	{
		if ( engine.getRenderSystem()->hasCurrentDevice() )
		{
			initialise();
		}
	}

	PickingUbo::~PickingUbo()
	{
	}

	void PickingUbo::initialise()
	{
		if ( !m_ubo )
		{
			auto & device = getCurrentDevice( m_engine );
			m_ubo = renderer::makeUniformBuffer< Configuration >( device
				, 1u
				, renderer::BufferTarget::eTransferDst
				, renderer::MemoryPropertyFlag::eHostVisible );
		}
	}

	void PickingUbo::cleanup()
	{
		m_ubo.reset();
	}

	void PickingUbo::update( uint32_t drawIndex
		, uint32_t nodeIndex )const
	{
		REQUIRE( m_ubo );
		auto & configuration = m_ubo->getData( 0u );
		configuration.drawIndex = drawIndex ? 1 : 0;
		configuration.nodeIndex = nodeIndex - 1;
		m_ubo->upload();
	}
}
