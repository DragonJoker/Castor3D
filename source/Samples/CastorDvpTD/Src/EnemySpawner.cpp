#include "EnemySpawner.hpp"

#include "Game.hpp"

using namespace Castor;
using namespace Castor3D;

namespace castortd
{
	EnemySpawner::EnemySpawner()
	{
	}

	EnemySpawner::~EnemySpawner()
	{
	}

	void EnemySpawner::Reset()
	{
		m_category = Enemy::Category
		{
			24.0f,
			0u,
			false,
			5u
		};

		m_totalsWaves = 0u;
		m_totalSpawned = 0u;
		m_timeSinceLastSpawn = std::chrono::milliseconds{};
		m_count = 0u;
	}

	void EnemySpawner::StartWave( uint32_t p_count )
	{
		m_category = Enemy::Category
		{
			m_category.m_speed,
			m_category.m_life + 5,
			m_category.m_flying,
			m_category.m_bounty + 2u
		};

		m_count = p_count;
		++m_totalsWaves;
		m_timeBetweenTwoSpawns = std::chrono::milliseconds( 1000 );// All enemies spawned in 1 second.
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

		return std::make_shared< Enemy >( *l_baseNode, p_game, p_path, m_category );
	}
}
