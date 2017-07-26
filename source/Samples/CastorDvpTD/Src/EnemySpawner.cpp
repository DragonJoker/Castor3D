#include "EnemySpawner.hpp"

#include "Game.hpp"

#include <Mesh/Mesh.hpp>
#include <Scene/Geometry.hpp>
#include <Scene/Scene.hpp>
#include <Scene/Light/PointLight.hpp>

using namespace Castor;
using namespace Castor3D;

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
		auto & cell = p_game.GetCell( Point2i{ pathNode.m_x, pathNode.m_y } );
		EnemyPtr result;

		if ( m_enemiesCache.empty() )
		{
			String name = cuT( "EnemyCube_" ) + std::to_string( m_totalSpawned );
			auto baseNode = p_game.GetScene().GetSceneNodeCache().Add( name + cuT( "_Base" ) );
			baseNode->SetPosition( p_game.Convert( Point2i{ cell.m_x, cell.m_y - 1 } ) + Point3r{ 0, p_game.GetCellHeight(), 0 } );
			baseNode->AttachTo( p_game.GetMapNode() );
			auto node = p_game.GetScene().GetSceneNodeCache().Add( name );
			node->SetOrientation( Quaternion::from_axis_angle( Point3r{ 1, 0, 1 }, Angle::from_degrees( 45 ) ) );
			node->AttachTo( baseNode );
			auto geometry = p_game.GetScene().GetGeometryCache().Add( name, node, p_game.GetEnemyMesh() );

			for ( auto submesh : *geometry->GetMesh() )
			{
				geometry->SetMaterial( *submesh, p_game.GetEnemyMaterial() );
			}

			auto light = p_game.GetScene().GetLightCache().Add( name, node, LightType::ePoint );
			light->SetColour( Colour::from_predef( PredefinedColour::eOpaqueRed ) );
			light->SetIntensity( 0.8f, 1.0f );
			light->GetPointLight()->SetAttenuation( Point3f{ 1.0f, 0.1f, 0.0f } );
			result = std::make_shared< Enemy >( *baseNode, p_game, p_path, m_category );
		}
		else
		{
			result = m_enemiesCache.front();
			m_enemiesCache.erase( m_enemiesCache.begin() );
			result->GetNode().SetPosition( p_game.Convert( Point2i{ cell.m_x, cell.m_y - 1 } ) + Point3r{ 0, p_game.GetCellHeight(), 0 } );
			result->Load( p_game );
		}

		return result;
	}

	void EnemySpawner::KillEnemy( Game & p_game, EnemyPtr && p_enemy )
	{
		p_enemy->GetNode().SetPosition( Point3r{ 0, -1000, 0 } );
		m_enemiesCache.push_back( std::move( p_enemy ) );
	}
}
