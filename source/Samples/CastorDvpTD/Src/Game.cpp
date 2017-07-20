#include "Game.hpp"

#include <Cache/AnimatedObjectGroupCache.hpp>
#include <Engine.hpp>
#include <Animation/Animation.hpp>
#include <Mesh/Mesh.hpp>
#include <Mesh/Submesh.hpp>
#include <Mesh/Skeleton/Skeleton.hpp>
#include <Scene/Geometry.hpp>
#include <Scene/Animation/AnimatedObject.hpp>
#include <Scene/Animation/AnimatedObjectGroup.hpp>
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
		constexpr uint32_t InitialOre = 75000u;
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

		void DoUpdateMaterials( Geometry & p_geometry
			, Tower::Category::Kind p_kind
			, CacheView< Material, MaterialCache, EventType::ePreRender > const & p_materials )
		{
			auto & mesh = *p_geometry.GetMesh();

			switch ( p_kind )
			{
			case Tower::Category::Kind::eLongRange:
				p_geometry.SetMaterial( *mesh.GetSubmesh( 0u ), p_materials.Find( cuT( "splash_accessories" ) ) );
				p_geometry.SetMaterial( *mesh.GetSubmesh( 1u ), p_materials.Find( cuT( "splash_accessories" ) ) );
				p_geometry.SetMaterial( *mesh.GetSubmesh( 2u ), p_materials.Find( cuT( "splash_body" ) ) );
				break;

			case Tower::Category::Kind::eShortRange:
				p_geometry.SetMaterial( *mesh.GetSubmesh( 0u ), p_materials.Find( cuT( "short_range_accessories" ) ) );
				p_geometry.SetMaterial( *mesh.GetSubmesh( 1u ), p_materials.Find( cuT( "short_range_accessories" ) ) );
				p_geometry.SetMaterial( *mesh.GetSubmesh( 2u ), p_materials.Find( cuT( "short_range_body" ) ) );
				break;
			}
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
		m_shortRangeTowerMesh = m_scene.GetMeshCache().Find( cuT( "ShortRange" ) );
		m_longRangeTowerMesh = m_scene.GetMeshCache().Find( cuT( "HeavySplash" ) );
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
		Grid grid;
		std::swap( m_grid, grid );

		m_totalBullets = 0ull;
		m_lives = InitialLives;
		m_ore = InitialOre;
		m_kills = 0u;
		m_selectedTower.reset();
		m_paused = false;
		m_ended = false;

		for ( auto & bullet : m_bullets )
		{
			bullet.GetNode().SetPosition( Point3r{ 0, -10, 0 } );
			m_bulletsCache.push_back( bullet );
		}

		m_bullets.clear();

		for ( auto & enemy : m_enemies )
		{
			m_spawner.KillEnemy( *this, std::move( enemy ) );
		}

		m_enemies.clear();

		for ( auto & tower : m_towers )
		{
			tower->GetNode().SetPosition( Point3r{ 0, -1000, 0 } );
		}

		m_towers.clear();

		m_spawner.Reset();
	}

	void Game::Start()
	{
		DoPrepareGrid();

		for ( auto & cell : m_grid )
		{
			if ( cell.m_state == Cell::State::Empty )
			{
				DoAddMapCube( cell );
			}
		}

		for ( auto submesh : *m_lastMapCube->GetMesh() )
		{
			// We need to update the render nodes (since the submesh's geometry buffers are now invalid).
			m_scene.SetChanged();
			m_scene.GetListener().PostEvent( MakeFunctorEvent( EventType::eQueueRender
				, [this, submesh]()
			{
				// TODO: Find a better way, since this forbids the suppression of RAM storage of the VBO data.
				submesh->ResetGpuBuffers();
			} ) );
		}

		auto & node = *m_path.rbegin();
		DoAddTarget( GetCell( node.m_x, node.m_y ) );

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
		static Cell dummy;
		dummy.m_state = Cell::State::Invalid;
		auto coords = Convert( p_position );

		if ( coords[0] >= 0 && coords[0] < int( m_grid.GetWidth() )
			 && coords[1] >= 0 && coords[1] < int( m_grid.GetHeight() ) )
		{
			return GetCell( coords );
		}

		return dummy;
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
		static Cell dummy;
		dummy.m_state = Cell::State::Invalid;
		auto coords = Convert( p_position );
		Cell & result = dummy;

		if ( coords[0] >= 0 && coords[0] < int( m_grid.GetWidth() )
			 && coords[1] >= 0 && coords[1] < int( m_grid.GetHeight() ) )
		{
			result = GetCell( coords );
		}

		return dummy;
	}

	bool Game::BuildTower( Castor::Point3r const & p_position, Tower::CategoryPtr && p_category )
	{
		bool result = false;

		if ( CanAfford( p_category->GetTowerCost() ) )
		{
			Cell & cell = GetCell( p_position );

			if ( cell.m_state == Cell::State::Empty )
			{
				cell.m_state = Cell::State::Tower;
				Spend( p_category->GetTowerCost() );
				DoAddTower( cell, std::move( p_category ) );
				result = true;
			}
		}

		return result;
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
			String name = cuT( "Bullet_" ) + std::to_string( ++m_totalBullets );
			auto node = m_scene.GetSceneNodeCache().Add( name );
			auto geometry = m_scene.GetGeometryCache().Add( name, node, m_bulletMesh );
			node->SetPosition( p_origin );
			node->AttachTo( m_mapNode );

			for ( auto submesh : *geometry->GetMesh() )
			{
				geometry->SetMaterial( *submesh, m_bulletMaterial );
			}

			m_bullets.emplace_back( p_speed, p_damage, *node, p_target );
		}
		else
		{
			auto bullet = *m_bulletsCache.begin();
			bullet.Load( p_speed, p_damage, p_origin, p_target );
			m_bullets.insert( m_bullets.end(), bullet );
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
		TowerPtr result;

		if ( p_cell.m_state == Cell::State::Tower )
		{
			auto it = std::find_if( m_towers.begin()
				, m_towers.end()
				, [&p_cell]( TowerPtr p_tower )
				{
					return p_tower->GetCell().m_x == p_cell.m_x
						&& p_tower->GetCell().m_y == p_cell.m_y;
				} );

			if ( it != m_towers.end() )
			{
				result = *it;
				m_selectedTower = result;
			}
		}

		return result;
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
		for ( auto & tower : m_towers )
		{
			tower->Accept( *this );
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

		Angle const angle{ Angle::from_degrees( -m_elapsed.count() * 120 / 1000.0_r ) };
		auto it = m_enemies.begin();

		while ( it != m_enemies.end() )
		{
			auto enemy = *it;

			if ( enemy->IsAlive() )
			{
				if ( enemy->GetState() == Enemy::State::Walking )
				{
					enemy->GetNode().Yaw( angle );

					if ( enemy->Accept( *this ) )
					{
						LoseLife( 1u );
						m_spawner.KillEnemy( *this, std::move( enemy ) );
						it = m_enemies.erase( it );
					}
					else
					{
						++it;
					}
				}
				else
				{
					++it;
				}

			}
			else if ( enemy->GetState() == Enemy::State::Dying )
			{
				enemy->Die();
				Gain( enemy->GetBounty() );
				m_spawner.KillEnemy( *this, std::move( enemy ) );
				it = m_enemies.erase( it );
				++m_kills;
			}
			else
			{
				// Dead?
				++it;
			}
		}
	}

	void Game::DoUpdateBullets()
	{
		auto it = m_bullets.begin();

		while ( it != m_bullets.end() )
		{
			if ( it->Accept( *this ) )
			{
				m_bulletsCache.push_back( *it );
				it = m_bullets.erase( it );
			}
			else
			{
				++it;
			}
		}
	}

	void Game::DoPrepareGrid()
	{
		auto prv = m_path.begin();

		if ( prv != m_path.end() )
		{
			auto cur = prv + 1;

			while ( cur != m_path.end() )
			{
				DoPrepareGridLine( *prv, *cur, m_grid );

				if ( prv == m_path.begin() )
				{
					GetCell( prv->m_x, prv->m_y ).m_state = Cell::State::Start;
				}

				++prv;
				++cur;
			}

			DoPrepareTarget( *prv, m_scene, m_grid );
		}
	}

	void Game::DoAddMapCube( Cell & p_cell )
	{
		String name = cuT( "MapCube_" ) + std::to_string( p_cell.m_x ) + cuT( "x" ) + std::to_string( p_cell.m_y );
		auto node = m_scene.GetSceneNodeCache().Add( name );
		auto geometry = m_scene.GetGeometryCache().Add( name, node, m_mapCubeMesh );
		node->SetPosition( Convert( Point2i{ p_cell.m_x, p_cell.m_y } ) + Point3r{ 0, m_cellDimensions[1] / 2, 0 } );
		node->AttachTo( m_mapNode );
		uint32_t index{ 0u };

		for ( auto submesh : *geometry->GetMesh() )
		{
			geometry->SetMaterial( *submesh, m_mapCubeMaterial, false );
		}

		m_lastMapCube = geometry;
		p_cell.m_state = Cell::State::Empty;
	}

	void Game::DoAddTarget( Cell & p_cell )
	{
		m_targetNode->SetPosition( Convert( Point2i{ p_cell.m_x, p_cell.m_y + 1 } ) );
		p_cell.m_state = Cell::State::Target;
	}

	MeshSPtr Game::DoSelectMesh( Tower::Category & p_category )
	{
		MeshSPtr result;

		switch ( p_category.GetKind() )
		{
		case Tower::Category::Kind::eLongRange:
			result = m_longRangeTowerMesh;
			break;

		case Tower::Category::Kind::eShortRange:
			result = m_shortRangeTowerMesh;
			break;
		}

		return result;
	}

	void Game::DoAddTower( Cell & p_cell, Tower::CategoryPtr && p_category )
	{
		String name = cuT( "Tower_" ) + std::to_string( p_cell.m_x ) + cuT( "x" ) + std::to_string( p_cell.m_y );
		auto node = m_scene.GetSceneNodeCache().Add( name );
		node->SetPosition( Convert( Point2i{ p_cell.m_x, p_cell.m_y } ) + Point3r{ 0, m_cellDimensions[1], 0 } );
		node->AttachTo( m_mapNode );
		MeshSPtr mesh = DoSelectMesh( *p_category );
		auto tower = m_scene.GetGeometryCache().Add( name, node, mesh );
		auto animGroup = m_scene.GetAnimatedObjectGroupCache().Add( name );
		std::chrono::milliseconds time{ 0 };

		if ( !tower->GetAnimations().empty() )
		{
			auto object = animGroup->AddObject( *tower, tower->GetName() + cuT( "_Movable" ) );
		}

		if ( tower->GetMesh() )
		{
			auto mesh = tower->GetMesh();

			if ( !mesh->GetAnimations().empty() )
			{
				auto object = animGroup->AddObject( *mesh, tower->GetName() + cuT( "_Mesh" ) );
				time = std::max( time
					, mesh->GetAnimation( p_category->GetAttackAnimationName() ).GetLength() );
			}

			auto skeleton = mesh->GetSkeleton();

			if ( skeleton )
			{
				if ( !skeleton->GetAnimations().empty() )
				{
					auto object = animGroup->AddObject( *skeleton, tower->GetName() + cuT( "_Skeleton" ) );
					time = std::max( time
						, skeleton->GetAnimation( p_category->GetAttackAnimationName() ).GetLength() );
				}
			}
		}

		animGroup->AddAnimation( p_category->GetAttackAnimationName() );
		animGroup->SetAnimationLooped( p_category->GetAttackAnimationName(), false );
		DoUpdateMaterials( *tower
			, p_category->GetKind()
			, m_scene.GetMaterialView() );
		p_cell.m_state = Cell::State::Tower;
		p_category->SetAttackAnimationTime( time );
		animGroup->StartAnimation( p_category->GetAttackAnimationName() );
		animGroup->PauseAnimation( p_category->GetAttackAnimationName() );
		node->SetScale( Point3r{ 0.15, 0.15, 0.15 } );
		std::clog << "Animation time: " << time.count() << std::endl;
		m_towers.push_back( std::make_shared< Tower >( std::move( p_category ), *node, *animGroup, p_cell ) );
	}

	void Game::DoGameOver()
	{
		m_started = false;
		m_ended = true;
		m_hud.GameOver();
	}
}
