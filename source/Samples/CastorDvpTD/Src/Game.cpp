#include "Game.hpp"

#include <Engine.hpp>
#include <Mesh/Mesh.hpp>
#include <Mesh/Submesh.hpp>
#include <Scene/Geometry.hpp>
#include <Scene/Scene.hpp>
#include <Scene/Light/PointLight.hpp>

using namespace Castor;
using namespace Castor3D;

namespace castortd
{
	namespace
	{
#if defined( NDEBUG )
		constexpr uint32_t InitialLives = 5u;
		constexpr uint32_t InitialOre = 750u;
#else
		constexpr uint32_t InitialLives = 1u;
		constexpr uint32_t InitialOre = 75000u;
#endif

		void DoPrepareGridLine( PathNode const & p_prv, PathNode const & p_cur, Grid & p_grid )
		{
			if ( p_prv.m_x != p_cur.m_x )
			{
				for ( auto x = std::min( p_prv.m_x, p_cur.m_x ); x < std::max( p_prv.m_x, p_cur.m_x ); ++x )
				{
					p_grid( p_prv.m_y - 1, x - 1 ).m_state = Cell::State::Path;
					p_grid( p_prv.m_y - 1, x - 0 ).m_state = Cell::State::Path;
					p_grid( p_prv.m_y - 0, x - 1 ).m_state = Cell::State::Path;
					p_grid( p_prv.m_y - 0, x - 0 ).m_state = Cell::State::Path;
				}
			}
			else
			{
				for ( auto y = std::min( p_prv.m_y, p_cur.m_y ); y <= std::max( p_prv.m_y, p_cur.m_y ); ++y )
				{
					p_grid( y - 1, p_prv.m_x - 1 ).m_state = Cell::State::Path;
					p_grid( y - 1, p_prv.m_x - 0 ).m_state = Cell::State::Path;
					p_grid( y - 0, p_prv.m_x - 1 ).m_state = Cell::State::Path;
					p_grid( y - 0, p_prv.m_x - 0 ).m_state = Cell::State::Path;
				}
			}
		}

		void DoPrepareTarget( PathNode const & p_cur, Scene & p_scene, Grid & p_grid )
		{
			for ( uint32_t x = p_cur.m_x - 2; x <= p_cur.m_x + 2; ++x )
			{
				for ( uint32_t y = p_cur.m_y - 1; y <= p_cur.m_y + 3; ++y )
				{
					p_grid( y, x ).m_state = Cell::State::Path;
				}
			}

			p_grid( p_cur.m_y, p_cur.m_x ).m_state = Cell::State::Target;
		}
	}

	Game::Game( Scene & p_scene )
		: m_scene{ p_scene }
		, m_hud{ *this, p_scene }
		, m_path
		{
			{ 19,  1 },
			{ 19,  3 },
			{ 11,  3 },
			{ 11,  7 },
			{ 23,  7 },
			{ 23, 11 },
			{ 13, 11 },
			{ 13, 15 },
			{ 25, 15 },
			{ 25, 19 },
			{ 15, 19 },
			{ 15, 23 },
			{ 17, 23 },
			{ 17, 27 },
		}
	{
		m_mapNode = m_scene.GetSceneNodeCache().Find( cuT( "MapBase" ) );
		m_mapCubeMesh = m_scene.GetMeshCache().Find( cuT( "MapCube" ) );
		m_mapCubeMaterial = m_scene.GetMaterialView().Find( cuT( "MapCube" ) );
		m_towerCubeMesh = m_scene.GetMeshCache().Find( cuT( "TowerCube" ) );
		m_enemyCubeMesh = m_scene.GetMeshCache().Find( cuT( "EnemyCube" ) );
		m_enemyCubeMaterial = m_scene.GetMaterialView().Find( cuT( "EnemyCube" ) );
		m_bulletMesh = m_scene.GetMeshCache().Find( cuT( "Bullet" ) );
		m_bulletMaterial = m_scene.GetMaterialView().Find( cuT( "Bullet" ) );
		m_targetNode = m_scene.GetSceneNodeCache().Find( cuT( "Target" ) );
		m_cellDimensions[0] = m_mapCubeMesh->GetCollisionBox().GetMax()[0] - m_mapCubeMesh->GetCollisionBox().GetMin()[0];
		m_cellDimensions[1] = m_mapCubeMesh->GetCollisionBox().GetMax()[1] - m_mapCubeMesh->GetCollisionBox().GetMin()[1];
		m_cellDimensions[2] = m_mapCubeMesh->GetCollisionBox().GetMax()[2] - m_mapCubeMesh->GetCollisionBox().GetMin()[2];

		m_hud.Initialise();
		Reset();
	}

	Game::~Game()
	{
	}

	void Game::Reset()
	{
		Grid l_grid;
		std::swap( m_grid, l_grid );

		m_totalBullets = 0ull;
		m_lives = InitialLives;
		m_ore = InitialOre;
		m_kills = 0u;
		m_selectedTower.reset();
		m_paused = false;
		m_ended = false;

		for ( auto & l_bullet : m_bullets )
		{
			l_bullet.GetNode().SetPosition( Point3r{ 0, -10, 0 } );
			m_bulletsCache.push_back( l_bullet );
		}

		m_bullets.clear();

		for ( auto & l_enemy : m_enemies )
		{
			m_spawner.KillEnemy( *this, std::move( l_enemy ) );
		}

		m_enemies.clear();

		for ( auto & l_tower : m_towers )
		{
			l_tower->GetNode().SetPosition( Point3r{ 0, -1000, 0 } );
		}

		m_towers.clear();

		m_spawner.Reset();
	}

	void Game::Start()
	{
		DoPrepareGrid();

		for ( auto & l_cell : m_grid )
		{
			if ( l_cell.m_state == Cell::State::Empty )
			{
				DoAddMapCube( l_cell );
			}
		}

		auto & l_node = *m_path.rbegin();
		DoAddTarget( GetCell( l_node.m_x, l_node.m_y ) );

		m_started = true;
		m_hud.Start();
		m_saved = Clock::now();
	}

	void Game::Pause()
	{
		m_paused = true;
		m_hud.Pause();
	}

	void Game::Resume()
	{
		m_paused = false;
		m_hud.Resume();
		m_saved = Clock::now();
	}

	void Game::Help()
	{
		m_paused = true;
		m_hud.Help();
	}

	void Game::Update()
	{
		if ( m_started && !m_paused )
		{
#if !defined( NDEBUG )
			m_elapsed = std::chrono::milliseconds{ 40 };
#else
			m_elapsed = std::chrono::duration_cast< std::chrono::milliseconds >( Clock::now() - m_saved );
#endif
			DoUpdateBullets();
			DoUpdateTowers();
			DoUpdateEnemies();
			m_hud.Update();
			m_saved = Clock::now();
		}
	}

	Cell & Game::GetCell( int p_x, int p_y )
	{
		return m_grid( p_y, p_x );
	}

	Cell & Game::GetCell( Castor::Point2i const & p_position )
	{
		return GetCell( p_position[0], p_position[1] );
	}

	Cell & Game::GetCell( Point3r const & p_position )
	{
		static Cell l_dummy;
		l_dummy.m_state = Cell::State::Invalid;
		auto l_coords = Convert( p_position );

		if ( l_coords[0] >= 0 && l_coords[0] < int( m_grid.GetWidth() )
			 && l_coords[1] >= 0 && l_coords[1] < int( m_grid.GetHeight() ) )
		{
			return GetCell( l_coords );
		}

		return l_dummy;
	}

	Cell const & Game::GetCell( int p_x, int p_y )const
	{
		return m_grid( p_y, p_x );
	}

	Cell const & Game::GetCell( Castor::Point2i const & p_position )const
	{
		return GetCell( p_position[0], p_position[1] );
	}

	Cell const & Game::GetCell( Point3r const & p_position )const
	{
		static Cell l_dummy;
		l_dummy.m_state = Cell::State::Invalid;
		auto l_coords = Convert( p_position );
		Cell & l_return = l_dummy;

		if ( l_coords[0] >= 0 && l_coords[0] < int( m_grid.GetWidth() )
			 && l_coords[1] >= 0 && l_coords[1] < int( m_grid.GetHeight() ) )
		{
			l_return = GetCell( l_coords );
		}

		return l_dummy;
	}

	bool Game::BuildTower( Castor::Point3r const & p_position, Tower::CategoryPtr && p_category )
	{
		bool l_return = false;

		if ( CanAfford( p_category->GetTowerCost() ) )
		{
			Cell & l_cell = GetCell( p_position );

			if ( l_cell.m_state == Cell::State::Empty )
			{
				l_cell.m_state = Cell::State::Tower;
				Spend( p_category->GetTowerCost() );
				DoAddTower( l_cell, std::move( p_category ) );
				l_return = true;
			}
		}

		return l_return;
	}

	Point3r Game::Convert( Castor::Point2i const & p_position )const
	{
		return Point3r( ( p_position[0] - int( m_grid.GetWidth() ) / 2 ) * m_cellDimensions[0]
						, 0
						, ( p_position[1] - int( m_grid.GetHeight() ) / 2 ) * m_cellDimensions[2] );
	}

	Point2i Game::Convert( Castor::Point3r const & p_position )const
	{
		return Point2i( p_position[0] / m_cellDimensions[0] + m_grid.GetWidth() / 2
						, p_position[2] / m_cellDimensions[2] + m_grid.GetHeight() / 2 );
	}

	void Game::EmitBullet( float p_speed, uint32_t p_damage, Castor::Point3r const & p_origin, Enemy & p_target )
	{
		if ( m_bulletsCache.empty() )
		{
			String l_name = cuT( "Bullet_" ) + std::to_string( ++m_totalBullets );
			auto l_node = m_scene.GetSceneNodeCache().Add( l_name );
			auto l_geometry = m_scene.GetGeometryCache().Add( l_name, l_node, m_bulletMesh );
			l_node->SetPosition( p_origin );
			l_node->AttachTo( m_mapNode );

			for ( auto l_submesh : *l_geometry->GetMesh() )
			{
				l_geometry->SetMaterial( *l_submesh, m_bulletMaterial );
			}

			m_bullets.emplace_back( p_speed, p_damage, *l_node, p_target );
		}
		else
		{
			auto l_bullet = *m_bulletsCache.begin();
			l_bullet.Load( p_speed, p_damage, p_origin, p_target );
			m_bullets.insert( m_bullets.end(), l_bullet );
			m_bulletsCache.erase( m_bulletsCache.begin() );
		}
	}

	void Game::Spend( uint32_t p_value )
	{
		if ( m_ore >= p_value )
		{
			m_ore -= p_value;
		}
		else
		{
			m_ore = 0;
		}
	}

	void Game::Gain( uint32_t p_value )
	{
		m_ore += p_value;
	}

	void Game::LoseLife( uint32_t p_value )
	{
		if ( m_lives > p_value )
		{
			m_lives -= p_value;
		}
		else
		{
			m_lives = 0;
			DoGameOver();
		}
	}

	TowerPtr Game::SelectTower( Cell const & p_cell )
	{
		TowerPtr l_return;

		if ( p_cell.m_state == Cell::State::Tower )
		{
			auto l_it = std::find_if( m_towers.begin(), m_towers.end(), [&p_cell]( TowerPtr p_tower )
			{
				return p_tower->GetCell().m_x == p_cell.m_x
					   && p_tower->GetCell().m_y == p_cell.m_y;
			} );

			if ( l_it != m_towers.end() )
			{
				l_return = *l_it;
				m_selectedTower = l_return;
			}
		}

		return l_return;
	}

	void Game::UpgradeTowerSpeed( Tower & p_tower )
	{
		if ( CanAfford( p_tower.GetSpeedUpgradeCost() )
			 && p_tower.CanUpgradeSpeed() )
		{
			Spend( p_tower.GetSpeedUpgradeCost() );
			p_tower.UpgradeSpeed();
		}
	}

	void Game::UpgradeTowerRange( Tower & p_tower )
	{
		if ( CanAfford( p_tower.GetRangeUpgradeCost() )
			 && p_tower.CanUpgradeRange() )
		{
			Spend( p_tower.GetRangeUpgradeCost() );
			p_tower.UpgradeRange();
		}
	}

	void Game::UpgradeTowerDamage( Tower & p_tower )
	{
		if ( CanAfford( p_tower.GetDamageUpgradeCost() )
			 && p_tower.CanUpgradeDamage() )
		{
			Spend( p_tower.GetDamageUpgradeCost() );
			p_tower.UpgradeDamage();
		}
	}

	bool Game::CanAfford( uint32_t p_price )
	{
		return m_ore >= p_price;
	}

	void Game::DoUpdateTowers()
	{
		for ( auto & l_tower : m_towers )
		{
			l_tower->Accept( *this );
		}
	}

	void Game::DoUpdateEnemies()
	{
		if ( m_enemies.empty() && m_spawner.IsWaveEnded() )
		{
			Gain( m_spawner.GetWave() * 2 );
#if !defined( NDEBUG )
			m_spawner.StartWave( 2u );
#else
			m_spawner.StartWave( 10u );
#endif
		}

		if ( m_spawner.CanSpawn( m_elapsed ) )
		{
			m_enemies.push_back( m_spawner.Spawn( *this, m_path ) );
		}

		Angle const l_angle{ Angle::from_degrees( -m_elapsed.count() * 120 / 1000.0_r ) };
		auto l_it = m_enemies.begin();

		while ( l_it != m_enemies.end() )
		{
			auto l_enemy = *l_it;

			if ( l_enemy->IsAlive() )
			{
				if ( l_enemy->GetState() == Enemy::State::Walking )
				{
					l_enemy->GetNode().Yaw( l_angle );

					if ( l_enemy->Accept( *this ) )
					{
						LoseLife( 1u );
						m_spawner.KillEnemy( *this, std::move( l_enemy ) );
						l_it = m_enemies.erase( l_it );
					}
					else
					{
						++l_it;
					}
				}
				else
				{
					++l_it;
				}

			}
			else if ( l_enemy->GetState() == Enemy::State::Dying )
			{
				l_enemy->Die();
				Gain( l_enemy->GetBounty() );
				m_spawner.KillEnemy( *this, std::move( l_enemy ) );
				l_it = m_enemies.erase( l_it );
				++m_kills;
			}
			else
			{
				// Dead?
				++l_it;
			}
		}
	}

	void Game::DoUpdateBullets()
	{
		auto l_it = m_bullets.begin();

		while ( l_it != m_bullets.end() )
		{
			if ( l_it->Accept( *this ) )
			{
				m_bulletsCache.push_back( *l_it );
				l_it = m_bullets.erase( l_it );
			}
			else
			{
				++l_it;
			}
		}
	}

	void Game::DoPrepareGrid()
	{
		auto l_prv = m_path.begin();

		if ( l_prv != m_path.end() )
		{
			auto l_cur = l_prv + 1;

			while ( l_cur != m_path.end() )
			{
				DoPrepareGridLine( *l_prv, *l_cur, m_grid );

				if ( l_prv == m_path.begin() )
				{
					GetCell( l_prv->m_x, l_prv->m_y ).m_state = Cell::State::Start;
				}

				++l_prv;
				++l_cur;
			}

			DoPrepareTarget( *l_prv, m_scene, m_grid );
		}
	}

	void Game::DoAddMapCube( Cell & p_cell )
	{
		String l_name = cuT( "MapCube_" ) + std::to_string( p_cell.m_x ) + cuT( "x" ) + std::to_string( p_cell.m_y );
		auto l_node = m_scene.GetSceneNodeCache().Add( l_name );
		auto l_geometry = m_scene.GetGeometryCache().Add( l_name, l_node, m_mapCubeMesh );
		l_node->SetPosition( Convert( Point2i{ p_cell.m_x, p_cell.m_y } ) + Point3r{ 0, m_cellDimensions[1] / 2, 0 } );
		l_node->AttachTo( m_mapNode );
		uint32_t l_index{ 0u };

		for ( auto l_submesh : *l_geometry->GetMesh() )
		{
			l_geometry->SetMaterial( *l_submesh, m_mapCubeMaterial );
		}

		p_cell.m_state = Cell::State::Empty;
	}

	void Game::DoAddTarget( Cell & p_cell )
	{
		m_targetNode->SetPosition( Convert( Point2i{ p_cell.m_x, p_cell.m_y + 1 } ) );
		p_cell.m_state = Cell::State::Target;
	}

	void Game::DoAddTower( Cell & p_cell, Tower::CategoryPtr && p_category )
	{
		String l_name = cuT( "TowerCube_" ) + std::to_string( p_cell.m_x ) + cuT( "x" ) + std::to_string( p_cell.m_y );
		auto l_baseNode = m_scene.GetSceneNodeCache().Add( l_name + cuT( "_Base" ) );
		l_baseNode->SetPosition( Convert( Point2i{ p_cell.m_x, p_cell.m_y } ) + Point3r{ 0, m_cellDimensions[1] * 3.0_r / 2, 0 } );
		l_baseNode->AttachTo( m_mapNode );
		auto l_node = m_scene.GetSceneNodeCache().Add( l_name );
		l_node->SetOrientation( Quaternion{ Point3r{ 1, 0, 1 }, Angle::from_degrees( 45 ) } );
		l_node->AttachTo( l_baseNode );

		auto l_geometry = m_scene.GetGeometryCache().Add( l_name, l_node, m_towerCubeMesh );
		auto l_material = m_scene.GetMaterialView().Find( p_category->GetMaterialName() );

		for ( auto l_submesh : *l_geometry->GetMesh() )
		{
			l_geometry->SetMaterial( *l_submesh, l_material );
		}

		auto l_light = m_scene.GetLightCache().Add( l_name, l_node, LightType::ePoint );
		l_light->SetColour( p_category->GetColour() );
		l_light->SetIntensity( 0.0f, 0.8f, 1.0f );
		l_light->GetPointLight()->SetAttenuation( Point3f{ 1.0f, 0.1f, 0.1f } );
		m_towers.push_back( std::make_shared< Tower >( std::move( p_category ), *l_baseNode, p_cell ) );
		p_cell.m_state = Cell::State::Tower;
	}

	void Game::DoGameOver()
	{
		m_started = false;
		m_ended = true;
		m_hud.GameOver();
	}
}
