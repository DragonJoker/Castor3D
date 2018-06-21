#include "FrustumCuller.hpp"

#include "Mesh/Submesh.hpp"
#include "Scene/Camera.hpp"
#include "Scene/SceneNode.hpp"

namespace castor3d
{
	FrustumCuller::FrustumCuller( Scene const & scene
		, Camera & camera )
		: SceneCuller{ scene, &camera }
	{
	}

	void FrustumCuller::doCullGeometries()
	{
		auto & camera = getCamera();

		for ( auto & node : m_allOpaqueSubmeshes )
		{
			auto visible = node.sceneNode.isDisplayable()
				&& node.sceneNode.isVisible()
				&& ( node.data.getInstantiation().isInstanced( node.material )
					|| camera.isVisible( node.instance, node.data ) );

			if ( visible )
			{
				m_culledOpaqueSubmeshes.push_back( &node );
			}
		}

		for ( auto & node : m_allTransparentSubmeshes )
		{
			auto visible = node.sceneNode.isDisplayable()
				&& node.sceneNode.isVisible()
				&& ( node.data.getInstantiation().isInstanced( node.material )
				|| camera.isVisible( node.instance, node.data ) );

			if ( visible )
			{
				m_culledTransparentSubmeshes.push_back( &node );
			}
		}
	}

	void FrustumCuller::doCullBillboards()
	{
		auto & camera = getCamera();

		for ( auto & node : m_allOpaqueBillboards )
		{
			auto visible = node.sceneNode.isDisplayable()
				&& node.sceneNode.isVisible();

			if ( visible )
			{
				m_culledOpaqueBillboards.push_back( &node );
			}
		}

		for ( auto & node : m_allTransparentBillboards )
		{
			auto visible = node.sceneNode.isDisplayable()
				&& node.sceneNode.isVisible();

			if ( visible )
			{
				m_culledTransparentBillboards.push_back( &node );
			}
		}
	}
}
