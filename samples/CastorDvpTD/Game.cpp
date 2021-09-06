#include "Game.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Animation/Animation.hpp>
#include <Castor3D/Cache/AnimatedObjectGroupCache.hpp>
#include <Castor3D/Cache/CacheView.hpp>
#include <Castor3D/Cache/GeometryCache.hpp>
#include <Castor3D/Cache/MaterialCache.hpp>
#include <Castor3D/Cache/ObjectCache.hpp>
#include <Castor3D/Event/Frame/FrameListener.hpp>
#include <Castor3D/Model/Mesh/Mesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>
#include <Castor3D/Model/Skeleton/Skeleton.hpp>
#include <Castor3D/Scene/Geometry.hpp>
#include <Castor3D/Scene/Animation/AnimatedObject.hpp>
#include <Castor3D/Scene/Animation/AnimatedObjectGroup.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/Light/PointLight.hpp>

#include <CastorUtils/Design/ResourceCache.hpp>

using namespace castor;
using namespace castor3d;

namespace castortd
{
	namespace
	{
#if defined( NDEBUG )
		constexpr uint32_t InitialLives = 50000u;
		constexpr uint32_t InitialOre = 75000u;
#else
		constexpr uint32_t InitialLives = 1u;
		constexpr uint32_t InitialOre = 75000u;
#endif

		void doPrepareGridLine( PathNode const & p_prv, PathNode const & p_cur, Grid & p_grid )
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

		void doPrepareTarget( PathNode const & p_cur, Scene & p_scene, Grid & p_grid )
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

		void doUpdateMaterials( Geometry & p_geometry
			, Tower::Category::Kind p_kind
			, CacheViewT< MaterialCache, EventType::ePreRender > const & p_materials )
		{
			auto & mesh = *p_geometry.getMesh().lock();

			switch ( p_kind )
			{
			case Tower::Category::Kind::eLongRange:
				p_geometry.setMaterial( *mesh.getSubmesh( 0u ), p_materials.find( cuT( "splash_accessories" ) ).lock().get() );
				p_geometry.setMaterial( *mesh.getSubmesh( 1u ), p_materials.find( cuT( "splash_accessories" ) ).lock().get() );
				p_geometry.setMaterial( *mesh.getSubmesh( 2u ), p_materials.find( cuT( "splash_body" ) ).lock().get() );
				break;

			case Tower::Category::Kind::eShortRange:
				p_geometry.setMaterial( *mesh.getSubmesh( 0u ), p_materials.find( cuT( "short_range_accessories" ) ).lock().get() );
				p_geometry.setMaterial( *mesh.getSubmesh( 1u ), p_materials.find( cuT( "short_range_accessories" ) ).lock().get() );
				p_geometry.setMaterial( *mesh.getSubmesh( 2u ), p_materials.find( cuT( "short_range_body" ) ).lock().get() );
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
		m_mapNode = m_scene.getSceneNodeCache().find( cuT( "MapBase" ) ).lock();
		m_mapCubeMesh = m_scene.getMeshCache().find( cuT( "MapCube" ) );
		m_mapCubeMaterial = m_scene.getMaterialView().find( cuT( "MapCube" ) );
		m_shortRangeTowerMesh = m_scene.getMeshCache().find( cuT( "ShortRange" ) );
		m_longRangeTowerMesh = m_scene.getMeshCache().find( cuT( "HeavySplash" ) );
		m_enemyCubeMesh = m_scene.getMeshCache().find( cuT( "EnemyCube" ) );
		m_enemyCubeMaterial = m_scene.getMaterialView().find( cuT( "EnemyCube" ) );
		m_bulletMesh = m_scene.getMeshCache().find( cuT( "Bullet" ) );
		m_bulletMaterial = m_scene.getMaterialView().find( cuT( "Bullet" ) );
		m_targetNode = m_scene.getSceneNodeCache().find( cuT( "Target" ) ).lock();
		m_cellDimensions[0] = m_mapCubeMesh.lock()->getBoundingBox().getMax()[0] - m_mapCubeMesh.lock()->getBoundingBox().getMin()[0];
		m_cellDimensions[1] = m_mapCubeMesh.lock()->getBoundingBox().getMax()[1] - m_mapCubeMesh.lock()->getBoundingBox().getMin()[1];
		m_cellDimensions[2] = m_mapCubeMesh.lock()->getBoundingBox().getMax()[2] - m_mapCubeMesh.lock()->getBoundingBox().getMin()[2];

		m_hud.initialise();
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
			bullet.getNode().setPosition( castor::Point3f{ 0, -10, 0 } );
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
			tower->getNode().setPosition( castor::Point3f{ 0, -1000, 0 } );
		}

		m_towers.clear();

		m_spawner.Reset();
	}

	void Game::start()
	{
		doPrepareGrid();

		for ( auto & cell : m_grid )
		{
			if ( cell.m_state == Cell::State::Empty )
			{
				doAddMapCube( cell );
			}
		}

		m_scene.setChanged();

		auto & node = *m_path.rbegin();
		doAddTarget( getCell( node.m_x, node.m_y ) );

		m_started = true;
		m_hud.start();
		m_saved = Clock::now();
	}

	void Game::pause()
	{
		m_paused = true;
		m_hud.pause();
	}

	void Game::resume()
	{
		m_paused = false;
		m_hud.resume();
		m_saved = Clock::now();
	}

	void Game::Help()
	{
		m_paused = true;
		m_hud.Help();
	}

	void Game::update()
	{
		if ( m_started && !m_paused )
		{
#if !defined( NDEBUG )
			m_elapsed = Milliseconds{ 40 };
#else
			m_elapsed += std::chrono::duration_cast< Milliseconds >( Clock::now() - m_saved );
#endif
			doUpdateBullets();
			doUpdateTowers();
			doUpdateEnemies();
			m_hud.update();
			m_saved = Clock::now();
		}
	}

	Cell & Game::getCell( int p_x, int p_y )
	{
		return m_grid( p_y, p_x );
	}

	Cell & Game::getCell( castor::Point2i const & p_position )
	{
		return getCell( p_position[0], p_position[1] );
	}

	Cell & Game::getCell( castor::Point3f const & p_position )
	{
		static Cell dummy;
		dummy.m_state = Cell::State::Invalid;
		auto coords = convert( p_position );

		if ( coords[0] >= 0 && coords[0] < int( m_grid.getWidth() )
			 && coords[1] >= 0 && coords[1] < int( m_grid.getHeight() ) )
		{
			return getCell( coords );
		}

		return dummy;
	}

	Cell const & Game::getCell( int p_x, int p_y )const
	{
		return m_grid( p_y, p_x );
	}

	Cell const & Game::getCell( castor::Point2i const & p_position )const
	{
		return getCell( p_position[0], p_position[1] );
	}

	Cell const & Game::getCell( castor::Point3f const & p_position )const
	{
		static Cell dummy;
		dummy.m_state = Cell::State::Invalid;
		auto coords = convert( p_position );
		Cell const * result = &dummy;

		if ( coords[0] >= 0 && coords[0] < int( m_grid.getWidth() )
			 && coords[1] >= 0 && coords[1] < int( m_grid.getHeight() ) )
		{
			result = &getCell( coords );
		}

		return *result;
	}

	bool Game::BuildTower( castor::Point3f const & p_position, Tower::CategoryPtr && p_category )
	{
		bool result = false;

		if ( CanAfford( p_category->getTowerCost() ) )
		{
			Cell & cell = getCell( p_position );

			if ( cell.m_state == Cell::State::Empty )
			{
				cell.m_state = Cell::State::Tower;
				Spend( p_category->getTowerCost() );
				doAddTower( cell, std::move( p_category ) );
				result = true;
			}
		}

		return result;
	}

	castor::Point3f Game::convert( castor::Point2i const & p_position )const
	{
		return castor::Point3f( ( p_position[0] - int( m_grid.getWidth() ) / 2 ) * m_cellDimensions[0]
			, 0
			, ( p_position[1] - int( m_grid.getHeight() ) / 2 ) * m_cellDimensions[2] );
	}

	Point2i Game::convert( castor::Point3f const & p_position )const
	{
		return Point2i( p_position[0] / m_cellDimensions[0] + m_grid.getWidth() / 2
			, p_position[2] / m_cellDimensions[2] + m_grid.getHeight() / 2 );
	}

	void Game::EmitBullet( float p_speed, uint32_t p_damage, castor::Point3f const & p_origin, Enemy & p_target )
	{
		if ( m_bulletsCache.empty() )
		{
			String name = cuT( "Bullet_" ) + std::to_string( ++m_totalBullets );
			auto node = m_scene.getSceneNodeCache().add( name
				, *m_scene.getObjectRootNode()
				, m_scene ).lock();
			auto geometry = m_scene.getGeometryCache().add( name
				, m_scene
				, *node
				, m_bulletMesh ).lock();
			node->setPosition( p_origin );
			node->attachTo( *m_mapNode );

			for ( auto submesh : *geometry->getMesh().lock() )
			{
				geometry->setMaterial( *submesh, m_bulletMaterial.lock().get() );
			}

			m_bullets.emplace_back( p_speed, p_damage, *node, p_target );
		}
		else
		{
			auto bullet = *m_bulletsCache.begin();
			bullet.load( p_speed, p_damage, p_origin, p_target );
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
			doGameOver();
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
					return p_tower->getCell().m_x == p_cell.m_x
						&& p_tower->getCell().m_y == p_cell.m_y;
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
		if ( CanAfford( p_tower.getSpeedUpgradeCost() )
			 && p_tower.CanUpgradeSpeed() )
		{
			Spend( p_tower.getSpeedUpgradeCost() );
			p_tower.UpgradeSpeed();
		}
	}

	void Game::UpgradeTowerRange( Tower & p_tower )
	{
		if ( CanAfford( p_tower.getRangeUpgradeCost() )
			 && p_tower.CanUpgradeRange() )
		{
			Spend( p_tower.getRangeUpgradeCost() );
			p_tower.UpgradeRange();
		}
	}

	void Game::UpgradeTowerDamage( Tower & p_tower )
	{
		if ( CanAfford( p_tower.getDamageUpgradeCost() )
			 && p_tower.CanUpgradeDamage() )
		{
			Spend( p_tower.getDamageUpgradeCost() );
			p_tower.UpgradeDamage();
		}
	}

	bool Game::CanAfford( uint32_t p_price )
	{
		return m_ore >= p_price;
	}

	void Game::doUpdateTowers()
	{
		for ( auto & tower : m_towers )
		{
			tower->accept( *this );
		}
	}

	void Game::doUpdateEnemies()
	{
		if ( m_enemies.empty() && m_spawner.IsWaveEnded() )
		{
			Gain( m_spawner.getWave() * 2 );
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

		Angle const angle{ Angle::fromDegrees( -m_elapsed.count() * 120 / 1000.0f ) };
		auto it = m_enemies.begin();

		while ( it != m_enemies.end() )
		{
			auto enemy = *it;

			if ( enemy->IsAlive() )
			{
				if ( enemy->getState() == Enemy::State::Walking )
				{
					enemy->getNode().yaw( angle );

					if ( enemy->accept( *this ) )
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
			else if ( enemy->getState() == Enemy::State::Dying )
			{
				enemy->Die();
				Gain( enemy->getBounty() );
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

	void Game::doUpdateBullets()
	{
		auto it = m_bullets.begin();

		while ( it != m_bullets.end() )
		{
			if ( it->accept( *this ) )
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

	void Game::doPrepareGrid()
	{
		auto prv = m_path.begin();

		if ( prv != m_path.end() )
		{
			auto cur = prv + 1;

			while ( cur != m_path.end() )
			{
				doPrepareGridLine( *prv, *cur, m_grid );

				if ( prv == m_path.begin() )
				{
					getCell( prv->m_x, prv->m_y ).m_state = Cell::State::Start;
				}

				++prv;
				++cur;
			}

			doPrepareTarget( *prv, m_scene, m_grid );
		}
	}

	void Game::doAddMapCube( Cell & p_cell )
	{
		String name = cuT( "MapCube_" ) + std::to_string( p_cell.m_x ) + cuT( "x" ) + std::to_string( p_cell.m_y );
		auto node = m_scene.getSceneNodeCache().add( name
			, *m_scene.getObjectRootNode()
			, m_scene ).lock();
		auto geometry = m_scene.getGeometryCache().add( name
			, m_scene
			, *node, m_mapCubeMesh ).lock();
		node->setPosition( convert( Point2i{ p_cell.m_x, p_cell.m_y } ) + castor::Point3f{ 0, m_cellDimensions[1] / 2, 0 } );
		node->attachTo( *m_mapNode );

		for ( auto submesh : *geometry->getMesh().lock() )
		{
			geometry->setMaterial( *submesh, m_mapCubeMaterial.lock().get(), false );
		}

		m_lastMapCube = geometry;
		p_cell.m_state = Cell::State::Empty;
	}

	void Game::doAddTarget( Cell & p_cell )
	{
		m_targetNode->setPosition( convert( Point2i{ p_cell.m_x, p_cell.m_y + 1 } ) );
		p_cell.m_state = Cell::State::Target;
	}

	MeshResPtr Game::doSelectMesh( Tower::Category & p_category )
	{
		MeshResPtr result;

		switch ( p_category.getKind() )
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

	void Game::doAddTower( Cell & p_cell, Tower::CategoryPtr && p_category )
	{
		String name = cuT( "Tower_" ) + std::to_string( p_cell.m_x ) + cuT( "x" ) + std::to_string( p_cell.m_y );
		auto node = m_scene.getSceneNodeCache().add( name
			, *m_scene.getObjectRootNode()
			, m_scene ).lock();
		node->setPosition( convert( Point2i{ p_cell.m_x, p_cell.m_y } ) + castor::Point3f{ 0, m_cellDimensions[1], 0 } );
		node->attachTo( *m_mapNode );
		auto mesh = doSelectMesh( *p_category );
		auto tower = m_scene.getGeometryCache().add( name
			, m_scene
			, *node, mesh ).lock();
		auto animGroup = m_scene.getAnimatedObjectGroupCache().add( name
			, m_scene ).lock();
		Milliseconds time{ 0 };

		if ( !tower->getAnimations().empty() )
		{
			animGroup->addObject( *tower, tower->getName() + cuT( "_Movable" ) );
		}

		if ( tower->getMesh().lock() )
		{
			auto mesh = tower->getMesh().lock();

			if ( !mesh->getAnimations().empty() )
			{
			  animGroup->addObject( *mesh, *tower, tower->getName() + cuT( "_Mesh" ) );
				time = std::max( time
					, mesh->getAnimation( p_category->getAttackAnimationName() ).getLength() );
			}

			auto skeleton = mesh->getSkeleton();

			if ( skeleton )
			{
				if ( !skeleton->getAnimations().empty() )
				{
					animGroup->addObject( *skeleton, *mesh, *tower, tower->getName() + cuT( "_Skeleton" ) );
					time = std::max( time
						, skeleton->getAnimation( p_category->getAttackAnimationName() ).getLength() );
				}
			}
		}

		animGroup->addAnimation( p_category->getAttackAnimationName() );
		animGroup->setAnimationLooped( p_category->getAttackAnimationName(), false );
		doUpdateMaterials( *tower
			, p_category->getKind()
			, m_scene.getMaterialView() );
		p_cell.m_state = Cell::State::Tower;
		p_category->setAttackAnimationTime( time );
		animGroup->startAnimation( p_category->getAttackAnimationName() );
		animGroup->pauseAnimation( p_category->getAttackAnimationName() );
		node->setScale( castor::Point3f{ 0.15, 0.15, 0.15 } );
		std::clog << "Animation time: " << time.count() << std::endl;
		m_towers.push_back( std::make_shared< Tower >( std::move( p_category ), *node, *animGroup, p_cell ) );
	}

	void Game::doGameOver()
	{
		m_started = false;
		m_ended = true;
		m_hud.GameOver();
	}
}
