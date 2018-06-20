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
		, Camera & camera )
		: SceneCuller{ scene, &camera }
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
			if ( primitive.second )
			{
				auto & geometry = *primitive.second;
				auto & node = *geometry.getParent();

				if ( node.isDisplayable()
					&& node.isVisible()
					&& primitive.second->getMesh() )
				{
					auto & mesh = *geometry.getMesh();

					for ( auto submesh : mesh )
					{
						auto material = geometry.getMaterial( *submesh );

						if ( material )
						{
							auto visible = submesh->getInstantiation().isInstanced( material )
								|| camera.isVisible( geometry, *submesh );

							if ( visible )
							{
								if ( material->hasAlphaBlending() )
								{
									m_transparentSubmeshes.emplace_back( CulledSubmesh{ geometry
										, *submesh
										, material
										, node } );
								}
								else
								{
									m_opaqueSubmeshes.emplace_back( CulledSubmesh{ geometry
										, *submesh
										, material
										, node } );
								}
							}
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
			if ( billboard.second )
			{
				auto & billboards = *billboard.second;
				auto & node = *billboards.getParent();

				if ( node.isDisplayable() && node.isVisible() )
				{
					MaterialSPtr material( billboards.getMaterial() );

					if ( material )
					{
						if ( material->hasAlphaBlending() )
						{
							m_transparentBillboards.emplace_back( CulledBillboard{ billboards
								, billboards
								, material
								, node } );
						}
						else
						{
							m_opaqueBillboards.emplace_back( CulledBillboard{ billboards
								, billboards
								, material
								, node } );
						}
					}
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
			if ( particleSystem.second
				&& particleSystem.second->getBillboards() )
			{
				auto & billboards = *particleSystem.second->getBillboards();
				auto & node = *particleSystem.second->getParent();

				if ( node.isDisplayable() && node.isVisible() )
				{
					MaterialSPtr material( particleSystem.second->getMaterial() );

					if ( material )
					{
						if ( material->hasAlphaBlending() )
						{
							m_transparentBillboards.emplace_back( CulledBillboard{ billboards
								, billboards
								, material
								, node } );
						}
						else
						{
							m_opaqueBillboards.emplace_back( CulledBillboard{ billboards
								, billboards
								, material
								, node } );
						}
					}
				}
			}
		}
	}
}
