#include "Castor3D/Render/Culling/InstantiatedFrustumCuller.hpp"

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
		void cullNodes( std::vector< Frustum > const & frustums
			, SceneCuller::CulledInstancesT< CulledT > & all
			, SceneCuller::CulledInstancesPtrT< CulledT > & culled )
		{
			uint32_t frustumIndex = 0u;
			std::vector< uint32_t > curIndex;
			curIndex.resize( all.objects.size(), 0u );
			CU_Require( all.objects.size() == all.instances.size() );

			for ( auto & frustum : frustums )
			{
				auto indexIt = curIndex.begin();
				auto objectIt = all.objects.begin();
				auto instanceIt = all.instances.begin();

				while ( objectIt != all.objects.end() )
				{
					CulledT & node = *objectIt;

					if ( isVisible( frustum, node ) )
					{
						UInt32Array & instances = *instanceIt;
						instances[*indexIt] = frustumIndex;
						++( *indexIt );
					}

					++indexIt;
					++objectIt;
					++instanceIt;
				}

				++frustumIndex;
			}

			auto indexIt = curIndex.begin();
			auto objectIt = all.objects.begin();
			auto instanceIt = all.instances.begin();

			while ( objectIt != all.objects.end() )
			{
				if ( *indexIt )
				{
					CulledT & node = *objectIt;
					UInt32Array & instances = *instanceIt;
					instances.resize( *indexIt );
					culled.push_back( &node, &instances );
				}

				++indexIt;
				++objectIt;
				++instanceIt;
			}
		}
	}

	InstantiatedFrustumCuller::InstantiatedFrustumCuller( Scene & scene
		, Camera & camera
		, uint32_t instanceCount )
		: SceneCuller{ scene, &camera, instanceCount }
	{
	}

	void InstantiatedFrustumCuller::doCullGeometries()
	{
		cullNodes( m_frustums, m_allOpaqueSubmeshes, m_culledOpaqueSubmeshes );
		cullNodes( m_frustums, m_allTransparentSubmeshes, m_culledTransparentSubmeshes );
	}

	void InstantiatedFrustumCuller::doCullBillboards()
	{
		cullNodes( m_frustums, m_allOpaqueBillboards, m_culledOpaqueBillboards );
		cullNodes( m_frustums, m_allTransparentBillboards, m_culledTransparentBillboards );
	}
}
