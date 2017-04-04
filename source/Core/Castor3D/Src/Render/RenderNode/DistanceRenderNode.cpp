#include "DistanceRenderNode.hpp"

#include "Scene/BillboardList.hpp"
#include "Scene/Geometry.hpp"

namespace Castor3D
{
	namespace details
	{
		SceneNode & GetParentNode( Geometry & p_instance )
		{
			return *p_instance.GetParent();
		}

		SceneNode & GetParentNode( BillboardBase & p_instance )
		{
			return *p_instance.GetNode();
		}
	}
}
