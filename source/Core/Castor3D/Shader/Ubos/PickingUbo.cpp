#include "Castor3D/Shader/Ubos/PickingUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

namespace castor3d
{
	const uint32_t PickingUbo::BindingPoint = 12u;
	castor::String const PickingUbo::BufferPicking = cuT( "Picking" );
	castor::String const PickingUbo::DrawIndex = cuT( "c3d_drawIndex" );
	castor::String const PickingUbo::NodeIndex = cuT( "c3d_nodeIndex" );

	PickingUbo::PickingUbo( Engine & engine )
		: m_engine{ engine }
	{
		if ( engine.getRenderSystem()->hasCurrentRenderDevice() )
		{
			initialise();
		}
	}

	void PickingUbo::initialise()
	{
		if ( !m_ubo )
		{
			m_ubo = makeUniformBuffer< Configuration >( *m_engine.getRenderSystem()
				, 1u
				, VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				, "PickingUbo" );
		}
	}

	void PickingUbo::update( Configuration & configuration
		, uint32_t drawIndex
		, uint32_t nodeIndex )
	{
		configuration.drawIndex = drawIndex;
		configuration.nodeIndex = nodeIndex;
	}
}
