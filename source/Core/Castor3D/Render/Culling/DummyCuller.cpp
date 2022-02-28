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

	void DummyCuller::doCullGeometries()
	{
		auto & nodes = getScene().getRenderNodes().getSubmeshNodes();
		auto & culled = m_culledSubmeshes;

		for ( auto & itPass : nodes )
		{
			for ( auto & itNode : itPass.second )
			{
				culled.push_back( itNode.second.get() );
			}
		}
	}

	void DummyCuller::doCullBillboards()
	{
		auto & nodes = getScene().getRenderNodes().getBillboardNodes();
		auto & culled = m_culledBillboards;

		for ( auto & itPass : nodes )
		{
			for ( auto & itNode : itPass.second )
			{
				culled.push_back( itNode.second.get() );
			}
		}
	}
}
