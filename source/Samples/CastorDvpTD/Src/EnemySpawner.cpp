#include "EnemySpawner.hpp"

#include "Game.hpp"

#include <Mesh/Mesh.hpp>
#include <Scene/Geometry.hpp>
#include <Scene/Scene.hpp>
#include <Scene/Light/PointLight.hpp>

using namespace castor;
using namespace castor3d;

namespace castortd
{
	EnemySpawner::EnemySpawner()
	{
		Reset();
	}

	EnemySpawner::~EnemySpawner()
	{
	}

	void EnemySpawner::Reset()
	{
		m_category = BaseEnemy{};

		m_totalsWaves = 0u;
		m_totalSpawned = 0u;
		m_timeSinceLastSpawn = Milliseconds{};
		m_count = 0u;
	}

	void EnemySpawner::StartWave( uint32_t p_count )
	{
		m_category.Upgrade();
		m_count = p_count;
		++m_totalsWaves;
		m_timeBetweenTwoSpawns = Milliseconds( 1000 );
		m_timeSinceLastSpawn = m_timeBetweenTwoSpawns;
	}

	bool EnemySpawner::CanSpawn( Milliseconds const & p_elapsed )
	{
		m_timeSinceLastSpawn += p_elapsed;
		return m_count && m_timeSinceLastSpawn >= m_timeBetweenTwoSpawns;
	}

	EnemyPtr EnemySpawner::Spawn( Game const & p_game, Path const & p_path )
	{
		--m_count;
		m_timeSinceLastSpawn = Milliseconds{};
		++m_totalSpawned;
		auto & pathNode = *p_path.begin();
		auto & cell = p_game.getCell( Point2i{ pathNode.m_x, pathNode.m_y } );
		EnemyPtr result;

		if ( m_enemiesCache.empty() )
		{
			String name = cuT( "EnemyCube_" ) + std::to_string( m_totalSpawned );
			auto baseNode = p_game.getScene().getSceneNodeCache().add( name + cuT( "_Base" ) );
			baseNode->setPosition( p_game.convert( Point2i{ cell.m_x, cell.m_y - 1 } ) + Point3r{ 0, p_game.getCellHeight(), 0 } );
			baseNode->attachTo( p_game.getMapNode() );
			auto node = p_game.getScene().getSceneNodeCache().add( name );
			node->setOrientation( Quaternion::fromAxisAngle( Point3r{ 1, 0, 1 }, Angle::fromDegrees( 45 ) ) );
			node->attachTo( baseNode );
			auto geometry = p_game.getScene().getGeometryCache().add( name, node, p_game.getEnemyMesh() );

			for ( auto submesh : *geometry->getMesh() )
			{
				geometry->setMaterial( *submesh, p_game.getEnemyMaterial() );
			}

			auto light = p_game.getScene().getLightCache().add( name, node, LightType::ePoint );
			light->setColour( Colour::fromPredefined( PredefinedColour::eOpaqueRed ) );
			light->setIntensity( 0.8f, 1.0f );
			light->getPointLight()->setAttenuation( Point3f{ 1.0f, 0.1f, 0.0f } );
			result = std::make_shared< Enemy >( *baseNode, p_game, p_path, m_category );
		}
		else
		{
			result = m_enemiesCache.front();
			m_enemiesCache.erase( m_enemiesCache.begin() );
			result->getNode().setPosition( p_game.convert( Point2i{ cell.m_x, cell.m_y - 1 } ) + Point3r{ 0, p_game.getCellHeight(), 0 } );
			result->load( p_game );
		}

		return result;
	}

	void EnemySpawner::KillEnemy( Game & p_game, EnemyPtr && p_enemy )
	{
		p_enemy->getNode().setPosition( Point3r{ 0, -1000, 0 } );
		m_enemiesCache.push_back( std::move( p_enemy ) );
	}
}
