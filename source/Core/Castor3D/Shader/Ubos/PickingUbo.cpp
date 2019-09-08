#include "Castor3D/Shader/Ubos/PickingUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBuffer.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

using namespace castor;

namespace castor3d
{
	const uint32_t PickingUbo::BindingPoint = 12u;
	String const PickingUbo::BufferPicking = cuT( "Picking" );
	String const PickingUbo::DrawIndex = cuT( "c3d_drawIndex" );
	String const PickingUbo::NodeIndex = cuT( "c3d_nodeIndex" );

	PickingUbo::PickingUbo( Engine & engine )
		: m_engine{ engine }
	{
		if ( engine.getRenderSystem()->hasCurrentRenderDevice() )
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
			auto & device = getCurrentRenderDevice( m_engine );
			m_ubo = makeUniformBuffer< Configuration >( device
				, 1u
				, VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				, "PickingUbo" );
		}
	}

	void PickingUbo::cleanup()
	{
		m_ubo.reset();
	}

	void PickingUbo::update( uint32_t drawIndex
		, uint32_t nodeIndex )const
	{
		CU_Require( m_ubo );
		auto & configuration = m_ubo->getData( 0u );
		configuration.drawIndex = drawIndex ? 1 : 0;
		configuration.nodeIndex = nodeIndex - 1;
		m_ubo->upload();
	}
}
