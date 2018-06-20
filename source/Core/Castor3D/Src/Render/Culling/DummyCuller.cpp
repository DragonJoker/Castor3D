#include "DummyCuller.hpp"

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
	DummyCuller::DummyCuller( Scene const & scene )
		: SceneCuller{ scene, nullptr }
	{
	}

	void DummyCuller::compute()
	{
		doCullGeometries();
		doCullBillboards();
		doCullParticles();
	}

	void DummyCuller::doCullGeometries()
	{
		doClear();
		auto & scene = getScene();
		auto lock = makeUniqueLock( scene.getGeometryCache() );

		for ( auto primitive : scene.getGeometryCache() )
		{
			if ( primitive.second->getParent()->isDisplayable()
				&& primitive.second->getParent()->isVisible()
				&& primitive.second->getMesh() )
			{
				auto mesh = primitive.second->getMesh();

				for ( auto submesh : *mesh )
				{
					auto material = primitive.second->getMaterial( *submesh );

					if ( material )
					{
						auto node = primitive.second->getParent();

						if ( material->hasAlphaBlending() )
						{
							m_transparentSubmeshes.emplace_back( CulledSubmesh{ *primitive.second
								, *submesh
								, material
								, *node } );
						}
						else
						{
							m_opaqueSubmeshes.emplace_back( CulledSubmesh{ *primitive.second
								, *submesh
								, material
								, *node } );
						}
					}
				}
			}
		}
	}

	void DummyCuller::doCullBillboards()
	{
		auto & scene = getScene();
		auto lock = makeUniqueLock( scene.getBillboardListCache() );

		for ( auto billboard : scene.getBillboardListCache() )
		{
			if ( billboard.second->getParent()->isDisplayable()
				&& billboard.second->getParent()->isVisible()
				&& billboard.second )
			{
				MaterialSPtr material( billboard.second->getMaterial() );

				if ( material )
				{
					if ( material->hasAlphaBlending() )
					{
						m_transparentBillboards.emplace_back( CulledBillboard{ *billboard.second
							, *billboard.second
							, material
							, *billboard.second->getParent() } );
					}
					else
					{
						m_opaqueBillboards.emplace_back( CulledBillboard{ *billboard.second
							, *billboard.second
							, material
							, *billboard.second->getParent() } );
					}
				}
			}
		}
	}

	void DummyCuller::doCullParticles()
	{
		auto & scene = getScene();
		auto lock = makeUniqueLock( scene.getParticleSystemCache() );

		for ( auto particleSystem : scene.getParticleSystemCache() )
		{
			if ( particleSystem.second->getParent()->isDisplayable()
				&& particleSystem.second->getParent()->isVisible()
				&& particleSystem.second->getBillboards() )
			{
				MaterialSPtr material( particleSystem.second->getMaterial() );

				if ( material )
				{
					if ( material->hasAlphaBlending() )
					{
						m_transparentBillboards.emplace_back( CulledBillboard{ *particleSystem.second->getBillboards()
							, *particleSystem.second->getBillboards()
							, material
							, *particleSystem.second->getParent() } );
					}
					else
					{
						m_opaqueBillboards.emplace_back( CulledBillboard{ *particleSystem.second->getBillboards()
							, *particleSystem.second->getBillboards()
							, material
							, *particleSystem.second->getParent() } );
					}
				}
			}
		}
	}
}
