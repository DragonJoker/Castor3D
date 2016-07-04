#include "BillboardManager.hpp"

using namespace Castor;

namespace Castor3D
{
	void testBillboardCache( Scene & p_scene )
	{
		auto cache = MakeObjectCache< BillboardList, Castor::String, BillboardProducer >( p_scene.GetRootNode(), p_scene.GetCameraRootNode(), p_scene.GetObjectRootNode(), SceneGetter{ p_scene }, BillboardProducer{} );
	}
}
