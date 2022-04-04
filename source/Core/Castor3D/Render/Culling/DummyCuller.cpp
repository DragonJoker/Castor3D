#include "Castor3D/Render/Culling/DummyCuller.hpp"

#include "Castor3D/Render/Node/BillboardRenderNode.hpp"
#include "Castor3D/Render/Node/SubmeshRenderNode.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/SceneNode.hpp"

namespace castor3d
{
	DummyCuller::DummyCuller( Scene & scene
		, Camera * camera )
		: SceneCuller{ scene, camera }
	{
	}

	bool DummyCuller::isSubmeshVisible( SubmeshRenderNode const & node )const
	{
		return node.instance.getParent()->isVisible();
	}

	bool DummyCuller::isBillboardVisible( BillboardRenderNode const & node )const
	{
		return node.instance.getNode()->isVisible();
	}
}
