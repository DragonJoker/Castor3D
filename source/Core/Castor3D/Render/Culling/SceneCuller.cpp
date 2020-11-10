#include "Castor3D/Render/Culling/SceneCuller.hpp"

#include "Castor3D/Cache/BillboardCache.hpp"
#include "Castor3D/Cache/GeometryCache.hpp"
#include "Castor3D/Cache/ParticleSystemCache.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/ParticleSystem/ParticleSystem.hpp"

#include <CastorUtils/Miscellaneous/Hash.hpp>

namespace castor3d
{
	//*********************************************************************************************

	size_t hash( CulledSubmesh const & culled )
	{
		return hash( culled.instance, culled.data, *culled.pass );
	}
	
	size_t hash( CulledSubmesh const & culled
		, uint32_t instanceMult )
	{
		return hash( culled.instance, culled.data, *culled.pass, instanceMult );
	}

	bool isVisible( Camera const & camera
		, CulledSubmesh const & node )
	{
		return isVisible( camera.getFrustum(), node );
	}

	bool isVisible( Frustum const & frustum
		, CulledSubmesh const & node )
	{
		return node.sceneNode.isDisplayable()
			&& node.sceneNode.isVisible()
			&& ( node.data.getInstantiation().isInstanced( node.pass->getOwner()->shared_from_this() )
				|| ( frustum.isVisible( node.instance.getBoundingSphere( node.data )
					, node.sceneNode.getDerivedTransformationMatrix()
					, node.sceneNode.getDerivedScale() )
					&& frustum.isVisible( node.instance.getBoundingBox( node.data )
						, node.sceneNode.getDerivedTransformationMatrix() ) ) );
	}

	//*********************************************************************************************

	size_t hash( CulledBillboard const & culled )
	{
		return hash( culled.data, *culled.pass );
	}
	
	size_t hash( CulledBillboard const & culled
		, uint32_t instanceMult )
	{
		return hash( culled.data, *culled.pass, instanceMult );
	}

	bool isVisible( Camera const & camera
		, CulledBillboard const & node )
	{
		return isVisible( camera.getFrustum(), node );
	}

	bool isVisible( Frustum const & frustum
		, CulledBillboard const & node )
	{
		return node.sceneNode.isDisplayable()
			&& node.sceneNode.isVisible();
	}

	//*********************************************************************************************

	SceneCuller::SceneCuller( Scene & scene
		, Camera * camera
		, uint32_t instancesCount )
		: m_scene{ scene }
		, m_camera{ camera }
		, m_instancesCount{ instancesCount }
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
			doClearAll();
			doListGeometries();
			doListBillboards();
			doListParticles();
			m_cameraDirty = true;
		}

		m_culledChanged = m_cameraDirty;

		if ( m_culledChanged )
		{
			doClearCulled();
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

	UInt32Array SceneCuller::getInitialInstances()const
	{
		UInt32Array instances;
		instances.resize( m_instancesCount );
		std::iota( instances.begin(), instances.end(), 0u );;
		return instances;
	}

	void SceneCuller::onSceneChanged( Scene const & scene )
	{
		m_sceneDirty = true;
	}

	void SceneCuller::onCameraChanged( Camera const & camera )
	{
		m_cameraDirty = true;
	}

	void SceneCuller::doClearAll()
	{
		m_allOpaqueSubmeshes.clear();
		m_allTransparentSubmeshes.clear();
		m_allOpaqueBillboards.clear();
		m_allTransparentBillboards.clear();
	}

	void SceneCuller::doClearCulled()
	{
		m_culledOpaqueSubmeshes.clear();
		m_culledTransparentSubmeshes.clear();
		m_culledOpaqueBillboards.clear();
		m_culledTransparentBillboards.clear();
	}

	void SceneCuller::doListGeometries()
	{
		auto & scene = getScene();
		auto lock( castor::makeUniqueLock( scene.getGeometryCache() ) );
		auto instances = getInitialInstances();

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
							for ( auto & pass : *material )
							{
								auto passFlags = pass->getPassFlags();

								if ( checkFlag( passFlags, PassFlag::eAlphaBlending ) )
								{
									m_allTransparentSubmeshes.push_back( CulledSubmesh{ geometry
											, *submesh
											, pass
											, node }
										, instances );
								}
								else
								{
									m_allOpaqueSubmeshes.push_back( CulledSubmesh{ geometry
											, *submesh
											, pass
											, node }
										, instances );
								}
							}
						}
					}

					if ( m_camera )
					{
						auto aabbMin = mesh.getBoundingBox().getMin();
						auto aabbMax = mesh.getBoundingBox().getMax();
						auto & camera = getCamera();
						castor::Point3f corners[8]
						{
							castor::Point3f{ aabbMin[0], aabbMin[1], aabbMin[2] },
							castor::Point3f{ aabbMin[0], aabbMin[1], aabbMax[2] },
							castor::Point3f{ aabbMin[0], aabbMax[1], aabbMin[2] },
							castor::Point3f{ aabbMin[0], aabbMax[1], aabbMax[2] },
							castor::Point3f{ aabbMax[0], aabbMin[1], aabbMin[2] },
							castor::Point3f{ aabbMax[0], aabbMin[1], aabbMax[2] },
							castor::Point3f{ aabbMax[0], aabbMax[1], aabbMin[2] },
							castor::Point3f{ aabbMax[0], aabbMax[1], aabbMax[2] },
						};
						for ( auto & corner : corners )
						{
							m_minCullersZ = std::min( m_minCullersZ
								, ( camera.getView() * node.getDerivedTransformationMatrix() * corner )[2] );
						}
					}
					else
					{
						m_minCullersZ = std::min( m_minCullersZ
							, node.getDerivedPosition()[2] - mesh.getBoundingSphere().getRadius() );
					}
				}
			}
		}
	}

	void SceneCuller::doListBillboards()
	{
		auto & scene = getScene();
		auto lock( castor::makeUniqueLock( scene.getBillboardListCache() ) );
		auto instances = getInitialInstances();

		for ( auto billboard : scene.getBillboardListCache() )
		{
			if ( billboard.second
				&& billboard.second->getParent() )
			{
				auto & billboards = *billboard.second;
				MaterialSPtr material( billboards.getMaterial() );

				if ( material )
				{
					auto & node = *billboards.getParent();

					for ( auto & pass : *material )
					{
						auto passFlags = pass->getPassFlags();

						if ( checkFlag( passFlags, PassFlag::eAlphaBlending ) )
						{
							m_allTransparentBillboards.push_back( CulledBillboard{ billboards
									, billboards
									, pass
									, node }
								, instances );
						}
						else
						{
							m_allOpaqueBillboards.push_back( CulledBillboard{ billboards
									, billboards
									, pass
									, node }
								, instances );
						}
					}
				}
			}
		}
	}

	void SceneCuller::doListParticles()
	{
		auto & scene = getScene();
		auto lock( castor::makeUniqueLock( scene.getParticleSystemCache() ) );
		auto instances = getInitialInstances();

		for ( auto particleSystem : scene.getParticleSystemCache() )
		{
			if ( particleSystem.second
				&& particleSystem.second->getBillboards()
				&& particleSystem.second->getParent() )
			{
				MaterialSPtr material( particleSystem.second->getMaterial() );

				if ( material )
				{
					auto & billboards = *particleSystem.second->getBillboards();
					auto & node = *particleSystem.second->getParent();

					for ( auto & pass : *material )
					{
						if ( material->hasAlphaBlending() )
						{
							m_allTransparentBillboards.push_back( CulledBillboard{ billboards
									, billboards
									, pass
									, node }
								, instances );
						}
						else
						{
							m_allOpaqueBillboards.push_back( CulledBillboard{ billboards
									, billboards
									, pass
									, node }
								, instances );
						}
					}
				}
			}
		}
	}

	//*********************************************************************************************
}
