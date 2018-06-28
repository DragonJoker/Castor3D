#include "SceneCuller.hpp"

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
	SceneCuller::SceneCuller( Scene const & scene
		, Camera * camera )
		: m_scene{ scene }
		, m_camera{ camera }
	{
		m_sceneChanged = m_scene.onChanged.connect( [this]( Scene const & scene )
			{
				onSceneChanged( m_scene );
			} );

		if ( m_camera )
		{
			m_cameraChanged = m_camera->onChanged.connect( [this]( Camera const & camera )
				{
					onCameraChanged( camera );
				} );
		}
	}

	void SceneCuller::compute()
	{
		m_allChanged = m_sceneDirty;

		if ( m_allChanged )
		{
			m_minCullersZ = std::numeric_limits< float >::max();
			m_allOpaqueSubmeshes.clear();
			m_allTransparentSubmeshes.clear();
			m_allOpaqueBillboards.clear();
			m_allTransparentBillboards.clear();
			doListGeometries();
			doListBillboards();
			doListParticles();
			m_cameraDirty = true;
		}

		m_culledChanged = m_cameraDirty;

		if ( m_culledChanged )
		{
			m_culledOpaqueSubmeshes.clear();
			m_culledTransparentSubmeshes.clear();
			m_culledOpaqueBillboards.clear();
			m_culledTransparentBillboards.clear();
			doCullGeometries();
			doCullBillboards();
		}

		m_sceneDirty = false;
		m_cameraDirty = false;

		if ( m_allChanged || m_culledChanged )
		{
			onCompute( *this );
		}
	}

	void SceneCuller::onSceneChanged( Scene const & scene )
	{
		m_sceneDirty = true;
	}

	void SceneCuller::onCameraChanged( Camera const & camera )
	{
		m_cameraDirty = true;
	}

	void SceneCuller::doListGeometries()
	{
		auto & scene = getScene();
		auto lock = makeUniqueLock( scene.getGeometryCache() );

		for ( auto primitive : scene.getGeometryCache() )
		{
			if ( primitive.second
				&& primitive.second->getParent() )
			{
				auto & geometry = *primitive.second;
				auto & node = *geometry.getParent();

				if ( primitive.second->getMesh() )
				{
					auto & mesh = *geometry.getMesh();

					for ( auto submesh : mesh )
					{
						auto material = geometry.getMaterial( *submesh );

						if ( material )
						{
							if ( material->hasAlphaBlending() )
							{
								m_allTransparentSubmeshes.emplace_back( CulledSubmesh{ geometry
									, *submesh
									, material
									, node } );
							}
							else
							{
								m_allOpaqueSubmeshes.emplace_back( CulledSubmesh{ geometry
									, *submesh
									, material
									, node } );
							}
						}
					}

					if ( m_camera )
					{
						auto aabbMin = mesh.getBoundingBox().getMin();
						auto aabbMax = mesh.getBoundingBox().getMax();
						auto & camera = getCamera();
						castor::Point3r corners[8]
						{
							castor::Point3r{ aabbMin[0], aabbMin[1], aabbMin[2] },
							castor::Point3r{ aabbMin[0], aabbMin[1], aabbMax[2] },
							castor::Point3r{ aabbMin[0], aabbMax[1], aabbMin[2] },
							castor::Point3r{ aabbMin[0], aabbMax[1], aabbMax[2] },
							castor::Point3r{ aabbMax[0], aabbMin[1], aabbMin[2] },
							castor::Point3r{ aabbMax[0], aabbMin[1], aabbMax[2] },
							castor::Point3r{ aabbMax[0], aabbMax[1], aabbMin[2] },
							castor::Point3r{ aabbMax[0], aabbMax[1], aabbMax[2] },
						};
						for ( auto & corner : corners )
						{
							m_minCullersZ = std::min( m_minCullersZ, ( camera.getView() * node.getDerivedTransformationMatrix() * corner )[2] );
						}
					}
					else
					{
						m_minCullersZ = std::min( m_minCullersZ, -mesh.getBoundingSphere().getRadius() );
					}
				}
			}
		}
	}

	void SceneCuller::doListBillboards()
	{
		auto & scene = getScene();
		auto lock = makeUniqueLock( scene.getBillboardListCache() );

		for ( auto billboard : scene.getBillboardListCache() )
		{
			if ( billboard.second
				&& billboard.second->getParent() )
			{
				auto & billboards = *billboard.second;
				auto & node = *billboards.getParent();

				MaterialSPtr material( billboards.getMaterial() );

				if ( material )
				{
					if ( material->hasAlphaBlending() )
					{
						m_allTransparentBillboards.emplace_back( CulledBillboard{ billboards
							, billboards
							, material
							, node } );
					}
					else
					{
						m_allOpaqueBillboards.emplace_back( CulledBillboard{ billboards
							, billboards
							, material
							, node } );
					}
				}
			}
		}
	}

	void SceneCuller::doListParticles()
	{
		auto & scene = getScene();
		auto lock = makeUniqueLock( scene.getParticleSystemCache() );

		for ( auto particleSystem : scene.getParticleSystemCache() )
		{
			if ( particleSystem.second
				&& particleSystem.second->getBillboards()
				&& particleSystem.second->getParent() )
			{
				auto & billboards = *particleSystem.second->getBillboards();
				auto & node = *particleSystem.second->getParent();

				MaterialSPtr material( particleSystem.second->getMaterial() );

				if ( material )
				{
					if ( material->hasAlphaBlending() )
					{
						m_allTransparentBillboards.emplace_back( CulledBillboard{ billboards
							, billboards
							, material
							, node } );
					}
					else
					{
						m_allOpaqueBillboards.emplace_back( CulledBillboard{ billboards
							, billboards
							, material
							, node } );
					}
				}
			}
		}
	}
}
