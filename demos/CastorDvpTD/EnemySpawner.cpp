#include "EnemySpawner.hpp"

#include "Game.hpp"

#include <Castor3D/Cache/GeometryCache.hpp>
#include <Castor3D/Cache/LightCache.hpp>
#include <Castor3D/Cache/ObjectCache.hpp>
#include <Castor3D/Model/Mesh/Mesh.hpp>
#include <Castor3D/Scene/Geometry.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/SceneNode.hpp>
#include <Castor3D/Scene/Light/Light.hpp>
#include <Castor3D/Scene/Light/PointLight.hpp>

namespace castortd
{
	EnemySpawner::EnemySpawner()
	{
		reset();
	}

	void EnemySpawner::reset()
	{
		m_category = BaseEnemy{};

		m_totalsWaves = 0u;
		m_totalSpawned = 0u;
		m_timeSinceLastSpawn = c3d::Milliseconds{};
		m_count = 0u;
	}

	void EnemySpawner::startWave( uint32_t count )
	{
		m_category.upgrade();
		m_count = count;
		++m_totalsWaves;
		m_timeBetweenTwoSpawns = c3d::Milliseconds( 600 );
		m_timeSinceLastSpawn = m_timeBetweenTwoSpawns;
	}

	bool EnemySpawner::canSpawn( c3d::Milliseconds const & elapsed )
	{
		m_timeSinceLastSpawn += elapsed;
		return m_count && m_timeSinceLastSpawn >= m_timeBetweenTwoSpawns;
	}

	EnemyPtr EnemySpawner::spawn( Game const & game, Path const & path )
	{
		--m_count;
		m_timeSinceLastSpawn = c3d::Milliseconds{};
		++m_totalSpawned;
		auto & pathNode = *path.begin();
		auto & cell = game.getCell( c3d::Point2i{ pathNode.m_x, pathNode.m_y } );
		EnemyPtr result;

		if ( m_enemiesCache.empty() )
		{
			c3d::String name = cuT( "EnemyCube_" ) + c3d::string::toString( m_totalSpawned );
			auto baseNode = game.getScene().addNewSceneNode( name + cuT( "_Base" ) );
			baseNode->setPosition( game.convert( c3d::Point2i{ cell.m_x, cell.m_y - 1 } ) + c3d::Point3f{ 0, game.getCellHeight(), 0 } );
			baseNode->attachTo( *game.getMapNode() );
			auto node = game.getScene().addNewSceneNode( name );
			node->setOrientation( c3d::Quaternion::fromAxisAngle( c3d::Point3f{ 1, 0, 1 }, 45.0_degrees ) );
			node->attachTo( *baseNode );
			auto geometry = game.getScene().createGeometry( name
				, game.getScene()
				, *node
				, game.getEnemyMesh() );

			for ( auto const & submesh : *geometry->getMesh() )
			{
				geometry->setMaterial( *submesh, game.getEnemyMaterial() );
			}

			game.getScene().addGeometry( c3d::move( geometry ) );

			auto light = game.getScene().createLight( name
				, game.getScene()
				, *node
				, game.getScene().getLightsFactory()
				, c3d::LightType::ePoint );
			light->setColour( c3d::RgbColour::fromPredefined( c3d::PredefinedRgbColour::eRed ) );
			light->getPointLight()->setIntensity( c3d::LuminousIntensity{ 0.8f } );
			light->getPointLight()->setAttenuation( c3d::Point3f{ 1.0f, 0.1f, 0.0f } );
			game.getScene().addLight( name, light );
			result = c3d::makeShared< Enemy >( *baseNode, game, path, m_category );
		}
		else
		{
			result = m_enemiesCache.front();
			m_enemiesCache.erase( m_enemiesCache.begin() );
			result->getNode().setPosition( game.convert( c3d::Point2i{ cell.m_x, cell.m_y - 1 } ) + c3d::Point3f{ 0, game.getCellHeight(), 0 } );
			result->load( game );
		}

		return result;
	}

	void EnemySpawner::killEnemy( EnemyPtr enemy )
	{
		enemy->getNode().setPosition( c3d::Point3f{ 0, -1000, 0 } );
		m_enemiesCache.push_back( c3d::move( enemy ) );
	}
}
