#include "Castor3D/Render/Culling/DummyCuller.hpp"

#include "Castor3D/Render/Node/SceneRenderNodes.hpp"
#include "Castor3D/Scene/Scene.hpp"

namespace castor3d
{
	DummyCuller::DummyCuller( Scene & scene
		, Camera * camera )
		: SceneCuller{ scene, camera }
	{
	}

	bool DummyCuller::isSubmeshVisible( SubmeshRenderNode const & node )const
	{
		return true;
	}

	bool DummyCuller::isBillboardVisible( BillboardRenderNode const & node )const
	{
		return true;
	}
}
