#include "DistanceRenderNode.hpp"

#include "Mesh/Mesh.hpp"
#include "Scene/BillboardList.hpp"
#include "Scene/Geometry.hpp"

namespace castor3d
{
	namespace details
	{
		uint32_t getPrimitiveCount( Geometry & instance )
		{
			return instance.getMesh()->getFaceCount();
		}

		uint32_t getPrimitiveCount( BillboardBase & instance )
		{
			return instance.getCount() * 2u;
		}

		uint32_t getVertexCount( Geometry & instance )
		{
			return instance.getMesh()->getVertexCount();
		}

		uint32_t getVertexCount( BillboardBase & instance )
		{
			return instance.getCount();
		}

		SceneNode & getParentNode( Geometry & instance )
		{
			return *instance.getParent();
		}

		SceneNode & getParentNode( BillboardBase & instance )
		{
			return *instance.getNode();
		}
	}
}
