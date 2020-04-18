#include "Castor3D/Shader/Ubos/PickingUbo.hpp"

namespace castor3d
{
	const uint32_t PickingUbo::BindingPoint = 12u;
	castor::String const PickingUbo::BufferPicking = cuT( "Picking" );
	castor::String const PickingUbo::DrawIndex = cuT( "c3d_drawIndex" );
	castor::String const PickingUbo::NodeIndex = cuT( "c3d_nodeIndex" );

	void PickingUbo::update( Configuration & configuration
		, uint32_t drawIndex
		, uint32_t nodeIndex )
	{
		configuration.drawIndex = drawIndex;
		configuration.nodeIndex = nodeIndex;
	}
}
