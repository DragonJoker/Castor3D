#include "FrustumCuller.hpp"

#include "Material/Material.hpp"
#include "Mesh/Mesh.hpp"
#include "Mesh/Submesh.hpp"
#include "Scene/BillboardList.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Geometry.hpp"
#include "Scene/Scene.hpp"
#include "Scene/ParticleSystem/ParticleSystem.hpp"

namespace castor3d
{
	FrustumCuller::FrustumCuller( Scene const & scene
		, Camera const & camera )
		: SceneCuller{ scene, camera }
	{
	}

	void FrustumCuller::compute()
	{
		doCullGeometries();
		doCullBillboards();
		doCullParticles();
	}

	void FrustumCuller::doCullGeometries()
	{
		doClear();
		auto & camera = getCamera();
		auto & scene = getScene();
		auto lock = makeUniqueLock( scene.getGeometryCache() );

		for ( auto primitive : scene.getGeometryCache() )
		{
			if ( primitive.second->getParent()->isVisible()
				&& primitive.second->getMesh() )
			{
				auto mesh = primitive.second->getMesh();

				for ( auto submesh : *mesh )
				{
					auto material = primitive.second->getMaterial( *submesh );

					if ( material )
					{
						auto node = primitive.second->getParent();
						auto visibility = camera.isVisible( submesh->getBoundingSphere()
							, node->getDerivedTransformationMatrix()
							, node->getDerivedScale() )
							&& camera.isVisible( submesh->getBoundingBox()
								, node->getDerivedTransformationMatrix() );

						if ( material->hasAlphaBlending() )
						{
							m_transparentSubmeshes.emplace_back( CulledSubmesh{ *submesh, *node } );
						}
						else
						{
							m_opaqueSubmeshes.emplace_back( CulledSubmesh{ *submesh, *node } );
						}
					}
				}
			}
		}
	}

	void FrustumCuller::doCullBillboards()
	{
		auto & camera = getCamera();
		auto & scene = getScene();
		auto lock = makeUniqueLock( scene.getBillboardListCache() );

		for ( auto billboard : scene.getBillboardListCache() )
		{
			MaterialSPtr material( billboard.second->getMaterial() );

			if ( material )
			{
				if ( material->hasAlphaBlending() )
				{
					m_transparentBillboards.emplace_back( CulledBillboard{ *billboard.second, *billboard.second->getParent() } );
				}
				else
				{
					m_opaqueBillboards.emplace_back( CulledBillboard{ *billboard.second, *billboard.second->getParent() } );
				}
			}
		}
	}

	void FrustumCuller::doCullParticles()
	{
		auto & camera = getCamera();
		auto & scene = getScene();
		auto lock = makeUniqueLock( scene.getParticleSystemCache() );

		for ( auto particleSystem : scene.getParticleSystemCache() )
		{
			MaterialSPtr material( particleSystem.second->getMaterial() );

			if ( material )
			{
				if ( material->hasAlphaBlending() )
				{
					m_transparentBillboards.emplace_back( CulledBillboard{ *particleSystem.second->getBillboards(), *particleSystem.second->getParent() } );
				}
				else
				{
					m_opaqueBillboards.emplace_back( CulledBillboard{ *particleSystem.second->getBillboards(), *particleSystem.second->getParent() } );
				}
			}
		}
	}
}
