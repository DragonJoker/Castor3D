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

	bool DummyCuller::isSubmeshCulled( SubmeshRenderNode const & node )const
	{
		return false;
	}

	bool DummyCuller::isBillboardCulled( BillboardRenderNode const & node )const
	{
		return false;
	}
}
