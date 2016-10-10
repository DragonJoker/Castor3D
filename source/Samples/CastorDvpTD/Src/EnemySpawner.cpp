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
		m_timeSinceLastSpawn = std::chrono::milliseconds{};
		m_count = 0u;
	}

	void EnemySpawner::StartWave( uint32_t p_count )
	{
		m_category.Upgrade();
		m_count = p_count;
		++m_totalsWaves;
		m_timeBetweenTwoSpawns = std::chrono::milliseconds( 1000 );
		m_timeSinceLastSpawn = m_timeBetweenTwoSpawns;
	}

	bool EnemySpawner::CanSpawn( std::chrono::milliseconds const & p_elapsed )
	{
		m_timeSinceLastSpawn += p_elapsed;
		return m_count && m_timeSinceLastSpawn >= m_timeBetweenTwoSpawns;
	}

	EnemyPtr EnemySpawner::Spawn( Game const & p_game, Path const & p_path )
	{
		--m_count;
		m_timeSinceLastSpawn = std::chrono::milliseconds{};
		++m_totalSpawned;
		auto & l_pathNode = *p_path.begin();
		auto & l_cell{ p_game.GetCell( Point2i{ l_pathNode.m_x, l_pathNode.m_y } ) };
		EnemyPtr l_return;

		if ( m_enemiesCache.empty() )
		{
			String l_name = cuT( "EnemyCube_" ) + std::to_string( m_totalSpawned );
			auto l_baseNode = p_game.GetScene().GetSceneNodeCache().Add( l_name + cuT( "_Base" ) );
			l_baseNode->SetPosition( p_game.Convert( Point2i{ l_cell.m_x, l_cell.m_y - 1 } ) + Point3r{ 0, p_game.GetCellHeight(), 0 } );
			l_baseNode->AttachTo( p_game.GetMapNode() );
			auto l_node = p_game.GetScene().GetSceneNodeCache().Add( l_name );
			l_node->SetOrientation( Quaternion{ Point3r{ 1, 0, 1 }, Angle::from_degrees( 45 ) } );
			l_node->AttachTo( l_baseNode );
			auto l_geometry = p_game.GetScene().GetGeometryCache().Add( l_name, l_node, p_game.GetEnemyMesh() );

			for ( auto l_submesh : *l_geometry->GetMesh() )
			{
				l_geometry->SetMaterial( l_submesh, p_game.GetEnemyMaterial() );
			}

			auto l_light = p_game.GetScene().GetLightCache().Add( l_name, l_node, LightType::Point );
			l_light->SetColour( Colour::from_predef( Colour::Predefined::OpaqueRed ) );
			l_light->SetIntensity( 0.0f, 0.8f, 1.0f );
			l_light->GetPointLight()->SetAttenuation( Point3f{ 1.0f, 0.1f, 0.0f } );
			l_return = std::make_shared< Enemy >( *l_baseNode, p_game, p_path, m_category );
		}
		else
		{
			l_return = m_enemiesCache.front();
			m_enemiesCache.erase( m_enemiesCache.begin() );
			l_return->GetNode().SetPosition( p_game.Convert( Point2i{ l_cell.m_x, l_cell.m_y - 1 } ) + Point3r{ 0, p_game.GetCellHeight(), 0 } );
			l_return->Load( p_game );
		}

		return l_return;
	}

	void EnemySpawner::KillEnemy( Game & p_game, EnemyPtr && p_enemy )
	{
		p_enemy->GetNode().SetPosition( Point3r{ 0, -1000, 0 } );
		m_enemiesCache.push_back( std::move( p_enemy ) );
	}
}
