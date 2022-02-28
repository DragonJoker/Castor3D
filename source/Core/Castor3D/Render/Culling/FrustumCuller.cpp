#include "Castor3D/Render/Culling/FrustumCuller.hpp"

#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/Node/BillboardRenderNode.hpp"
#include "Castor3D/Render/Node/SceneRenderNodes.hpp"
#include "Castor3D/Render/Node/SubmeshRenderNode.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"

namespace castor3d
{
	namespace
	{
		template< typename NodeT >
		void cullNodes( Camera const & camera
			, SceneRenderNodes::DescriptorNodesPoolsT< NodeT > const & all
			, SceneCuller::NodeArrayT< NodeT > & culled )
		{
			for ( auto & itPass : all )
			{
				for ( auto & itNode : itPass.second )
				{
					auto & node = *itNode.second;

					if ( !isCulled( node )
						|| isVisible( camera, node ) )
					{
						culled.push_back( &node );
					}
				}
			}
		}
	}

	FrustumCuller::FrustumCuller( Scene & scene
		, Camera & camera )
		: SceneCuller{ scene, &camera }
	{
	}

	FrustumCuller::FrustumCuller( Camera & camera )
		: FrustumCuller{ *camera.getScene(), camera }
	{
	}

	void FrustumCuller::doCullGeometries()
	{
		cullNodes( getCamera()
			, getScene().getRenderNodes().getSubmeshNodes()
			, m_culledSubmeshes );
	}

	void FrustumCuller::doCullBillboards()
	{
		cullNodes( getCamera()
			, getScene().getRenderNodes().getBillboardNodes()
			, m_culledBillboards );
	}
}
