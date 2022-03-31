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
	FrustumCuller::FrustumCuller( Scene & scene
		, Camera & camera )
		: SceneCuller{ scene, &camera }
	{
	}

	FrustumCuller::FrustumCuller( Camera & camera )
		: FrustumCuller{ *camera.getScene(), camera }
	{
	}

	FrustumCuller::FrustumCuller(Scene & scene
		, Frustum & frustum )
		: SceneCuller{ scene, nullptr }
		, m_frustum{ &frustum }
	{
	}

	void FrustumCuller::updateFrustum( castor::Matrix4x4f const & projection
		, castor::Matrix4x4f const & view )
	{
		m_frustum->update( projection, view );
	}

	bool FrustumCuller::isSubmeshVisible( SubmeshRenderNode const & node )const
	{
		return !node.instance.isCullable()
			|| isVisible( getCamera().getFrustum(), node );
	}

	bool FrustumCuller::isBillboardVisible( BillboardRenderNode const & node )const
	{
		return !node.instance.isCullable()
			|| isVisible( getCamera().getFrustum(), node );
	}
}
