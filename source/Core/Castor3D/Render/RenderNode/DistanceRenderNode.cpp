#include "DistanceRenderNode.hpp"

#include "Mesh/Mesh.hpp"
#include "Mesh/Submesh.hpp"
#include "Scene/BillboardList.hpp"
#include "Scene/Geometry.hpp"

namespace castor3d
{
	namespace details
	{
		uint32_t getPrimitiveCount( Submesh const & submesh )
		{
			return submesh.getFaceCount();
		}

		uint32_t getPrimitiveCount( BillboardBase const & instance )
		{
			return instance.getCount() * 2u;
		}

		uint32_t getVertexCount( Submesh const & submesh )
		{
			return submesh.getPointsCount();
		}

		uint32_t getVertexCount( BillboardBase const & instance )
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
