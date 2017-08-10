#include "DistanceRenderNode.hpp"

#include "Scene/BillboardList.hpp"
#include "Scene/Geometry.hpp"

namespace castor3d
{
	namespace details
	{
		SceneNode & getParentNode( Geometry & p_instance )
		{
			return *p_instance.getParent();
		}

		SceneNode & getParentNode( BillboardBase & p_instance )
		{
			return *p_instance.getNode();
		}
	}
}
