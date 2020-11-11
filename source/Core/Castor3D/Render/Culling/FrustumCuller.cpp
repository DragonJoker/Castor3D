#include "Castor3D/Render/Culling/FrustumCuller.hpp"

#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/SceneNode.hpp"

namespace castor3d
{
	namespace
	{
		template< typename CulledT >
		void cullNodes( Camera const & camera
			, SceneCuller::CulledInstancesT< CulledT > & all
			, SceneCuller::CulledInstancesPtrT< CulledT > & culled )
		{
			auto objectIt = all.objects.begin();
			auto instanceIt = all.instances.begin();

			while ( objectIt != all.objects.end() )
			{
				CulledT & node = *objectIt;
				UInt32Array & instances = *instanceIt;

				if ( isVisible( camera, node ) )
				{
					culled.push_back( &node, &instances );
				}

				++objectIt;
				++instanceIt;
			}
		}

		template< typename CulledT >
		void cullNodes( Camera const & camera
			, SceneCuller::CulledInstanceArrayT< CulledT > & all
			, SceneCuller::CulledInstancePtrArrayT< CulledT > & culled )
		{
			for ( size_t i = 0; i < size_t( RenderMode::eCount ); ++i )
			{
				cullNodes( camera, all[i], culled[i] );
			}
		}
	}

	FrustumCuller::FrustumCuller( Scene & scene
		, Camera & camera )
		: SceneCuller{ scene, &camera, 1u }
	{
	}

	void FrustumCuller::doCullGeometries()
	{
		cullNodes( getCamera(), m_allSubmeshes, m_culledSubmeshes );
	}

	void FrustumCuller::doCullBillboards()
	{
		cullNodes( getCamera(), m_allBillboards, m_culledBillboards );
	}
}
