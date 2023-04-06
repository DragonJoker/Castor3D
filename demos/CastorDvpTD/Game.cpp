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
#include <Castor3D/Render/RenderDevice.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Scene/Geometry.hpp>
#include <Castor3D/Scene/Animation/AnimatedObject.hpp>
#include <Castor3D/Scene/Animation/AnimatedObjectGroup.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/Light/PointLight.hpp>

#include <RenderGraph/FramePassTimer.hpp>

#include <CastorUtils/Design/ResourceCache.hpp>

#define Cheat 0

namespace castortd
{
	namespace game
	{
#if !defined( NDEBUG ) || Cheat
		constexpr uint32_t InitialLives = 500u;
		constexpr uint32_t InitialOre = 75000u;
#	else
		constexpr uint32_t InitialLives = 5u;
		constexpr uint32_t InitialOre = 750u;
#	endif

		static void doPrepareGridLine( PathNode const & prv, PathNode const & cur, Grid & grid )
		{
			if ( prv.m_x != cur.m_x )
			{
				for ( auto x = std::min( prv.m_x, cur.m_x ); x < std::max( prv.m_x, cur.m_x ); ++x )
				{
					grid( prv.m_y - 1, x - 1 ).m_state = Cell::State::Path;
					grid( prv.m_y - 1, x - 0 ).m_state = Cell::State::Path;
					grid( prv.m_y - 0, x - 1 ).m_state = Cell::State::Path;
					grid( prv.m_y - 0, x - 0 ).m_state = Cell::State::Path;
				}
			}
			else
			{
				for ( auto y = std::min( prv.m_y, cur.m_y ); y <= std::max( prv.m_y, cur.m_y ); ++y )
				{
					grid( y - 1, prv.m_x - 1 ).m_state = Cell::State::Path;
					grid( y - 1, prv.m_x - 0 ).m_state = Cell::State::Path;
					grid( y - 0, prv.m_x - 1 ).m_state = Cell::State::Path;
					grid( y - 0, prv.m_x - 0 ).m_state = Cell::State::Path;
				}
			}
		}

		static void doPrepareTarget( PathNode const & cur, castor3d::Scene & scene, Grid & grid )
		{
			for ( uint32_t x = cur.m_x - 2; x <= cur.m_x + 2; ++x )
			{
				for ( uint32_t y = cur.m_y - 1; y <= cur.m_y + 3; ++y )
				{
					grid( y, x ).m_state = Cell::State::Path;
				}
			}

			grid( cur.m_y, cur.m_x ).m_state = Cell::State::Target;
		}

		static void doUpdateMaterials( castor3d::Geometry & geometry
			, Tower::Category::Kind kind
			, castor3d::CacheViewT< castor3d::MaterialCache, castor3d::EventType::ePreRender > const & materials )
		{
			if ( auto mesh = geometry.getMesh().lock() )
			{
				switch ( kind )
				{
				case Tower::Category::Kind::eLongRange:
					geometry.setMaterial( *mesh->getSubmesh( 0u ), materials.find( cuT( "splash_accessories" ) ).lock().get() );
					geometry.setMaterial( *mesh->getSubmesh( 1u ), materials.find( cuT( "splash_accessories" ) ).lock().get() );
					geometry.setMaterial( *mesh->getSubmesh( 2u ), materials.find( cuT( "splash_body" ) ).lock().get() );
					break;

				case Tower::Category::Kind::eShortRange:
					geometry.setMaterial( *mesh->getSubmesh( 0u ), materials.find( cuT( "short_range_body" ) ).lock().get() );
					geometry.setMaterial( *mesh->getSubmesh( 1u ), materials.find( cuT( "short_range_accessories" ) ).lock().get() );
					break;
				}
			}
		}
	}

	Game::Game( castor3d::Scene & scene )
		: m_scene{ scene }
		, m_hud{ *this, m_scene }
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
		m_mapNode = m_scene.findSceneNode( cuT( "MapBase" ) );
		m_mapCubeMesh = m_scene.findMesh( cuT( "MapCube" ) );
		m_mapCubeMaterial = m_scene.findMaterial( cuT( "MapCube" ) );
		m_shortRangeTowerMesh = m_scene.findMesh( cuT( "ShortRange" ) );
		m_longRangeTowerMesh = m_scene.findMesh( cuT( "HeavySplash" ) );
		m_enemyCubeMesh = m_scene.findMesh( cuT( "EnemyCube" ) );
		m_enemyCubeMaterial = m_scene.findMaterial( cuT( "EnemyCube" ) );
		m_bulletMesh = m_scene.findMesh( cuT( "Bullet" ) );
		m_bulletMaterial = m_scene.findMaterial( cuT( "Bullet" ) );
		m_boulderMesh = m_scene.findMesh( cuT( "Boulder" ) );
		m_boulderMaterial = m_scene.findMaterial( cuT( "Boulder" ) );
		m_targetNode = m_scene.findSceneNode( cuT( "Target" ) );
		m_updateTimer = castor::makeUnique< castor3d::FramePassTimer >( m_scene.getEngine()->getRenderSystem()->getRenderDevice().makeContext()
			, "CastorDvpTD/Update" );
		m_scene.getEngine()->registerTimer( "CastorDvpTD/Update", *m_updateTimer );
		m_cellDimensions[0] = m_mapCubeMesh.lock()->getBoundingBox().getMax()[0] - m_mapCubeMesh.lock()->getBoundingBox().getMin()[0];
		m_cellDimensions[1] = m_mapCubeMesh.lock()->getBoundingBox().getMax()[1] - m_mapCubeMesh.lock()->getBoundingBox().getMin()[1];
		m_cellDimensions[2] = m_mapCubeMesh.lock()->getBoundingBox().getMax()[2] - m_mapCubeMesh.lock()->getBoundingBox().getMin()[2];

		m_hud.initialise();
		reset();
	}

	Game::~Game()
	{
	}

	void Game::reset()
	{
		Grid grid;
		std::swap( m_grid, grid );

		m_totalBullets = 0ull;
		m_totalBoulders = 0ull;
		m_lives = game::InitialLives;
		m_ore = game::InitialOre;
		m_kills = 0u;
		m_selectedTower = {};
		m_paused = false;
		m_ended = false;

		for ( auto & bullet : m_bullets )
		{
			bullet.getNode().setPosition( castor::Point3f{ 0, -10, 0 } );
			m_bulletsCache.push_back( bullet );
		}

		m_bullets.clear();

		for ( auto & boulder : m_boulders )
		{
			boulder.getNode().setPosition( castor::Point3f{ 0, -10, 0 } );
			m_bouldersCache.push_back( boulder );
		}

		m_boulders.clear();

		for ( auto & enemy : m_enemies )
		{
			m_spawner.killEnemy( *this, std::move( enemy ) );
		}

		m_enemies.clear();

		for ( auto & tower : m_towers )
		{
			tower->getNode().setPosition( castor::Point3f{ 0, -1000, 0 } );
		}

		m_towers.clear();

		m_spawner.reset();
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

		auto & node = *m_path.rbegin();
		doAddTarget( getCell( int( node.m_x ), int( node.m_y ) ) );

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

	void Game::help()
	{
		m_paused = true;
		m_hud.Help();
	}

	void Game::update()
	{
		if ( m_started && !m_paused )
		{
			auto block = m_updateTimer->start();
			m_elapsed = std::min( 40_ms
				, std::chrono::duration_cast< castor::Milliseconds >( Clock::now() - m_saved ) );
			doUpdateBullets();
			doUpdateBoulders();
			doUpdateTowers();
			doUpdateEnemies();
			m_hud.update();
			m_saved = Clock::now();
		}
	}

	Cell & Game::getCell( int x, int y )
	{
		return m_grid( uint32_t( y ), uint32_t( x ) );
	}

	Cell & Game::getCell( castor::Point2i const & position )
	{
		return getCell( position[0], position[1] );
	}

	Cell & Game::getCell( castor::Point3f const & position )
	{
		static Cell dummy;
		dummy.m_state = Cell::State::Invalid;
		auto coords = convert( position );

		if ( coords[0] >= 0 && coords[0] < int( m_grid.getWidth() )
			 && coords[1] >= 0 && coords[1] < int( m_grid.getHeight() ) )
		{
			return getCell( coords );
		}

		return dummy;
	}

	Cell const & Game::getCell( int x, int y )const
	{
		return m_grid( uint32_t( y )
			, uint32_t( x ) );
	}

	Cell const & Game::getCell( castor::Point2i const & position )const
	{
		return getCell( position[0], position[1] );
	}

	Cell const & Game::getCell( castor::Point3f const & position )const
	{
		static Cell dummy;
		dummy.m_state = Cell::State::Invalid;
		auto coords = convert( position );
		Cell const * result = &dummy;

		if ( coords[0] >= 0 && coords[0] < int( m_grid.getWidth() )
			 && coords[1] >= 0 && coords[1] < int( m_grid.getHeight() ) )
		{
			result = &getCell( coords );
		}

		return *result;
	}

	bool Game::buildTower( castor::Point3f const & position, Tower::CategoryPtr && category )
	{
		bool result = false;

		if ( canAfford( category->getTowerCost() ) )
		{
			Cell & cell = getCell( position );

			if ( cell.m_state == Cell::State::Empty )
			{
				cell.m_state = Cell::State::Tower;
				spend( category->getTowerCost() );
				doAddTower( cell, std::move( category ) );
				result = true;
			}
		}

		return result;
	}

	castor::Point3f Game::convert( castor::Point2i const & position )const
	{
		return castor::Point3f( ( float( position[0] ) - float( m_grid.getWidth() ) / 2 ) * m_cellDimensions[0]
			, 0
			, ( float( position[1] ) - float( m_grid.getHeight() ) / 2 ) * m_cellDimensions[2] );
	}

	castor::Point2i Game::convert( castor::Point3f const & position )const
	{
		return castor::Point2i( int( position[0] / m_cellDimensions[0] + float( m_grid.getWidth() / 2 ) )
			, int( position[2] / m_cellDimensions[2] + float( m_grid.getHeight() / 2 ) ) );
	}

	void Game::emitBullet( float speed, uint32_t damage, castor::Point3f const & origin, Enemy & target )
	{
		if ( m_bulletsCache.empty() )
		{
			castor::String name = cuT( "Bullet_" ) + castor::string::toString( ++m_totalBullets );
			auto node = m_scene.addNewSceneNode( name );
			auto geometry = m_scene.createGeometry( name
				, m_scene
				, *node
				, m_bulletMesh );
			node->setPosition( origin );
			node->attachTo( *m_mapNode );

			for ( auto submesh : *geometry->getMesh().lock() )
			{
				geometry->setMaterial( *submesh, m_bulletMaterial.lock().get() );
			}

			m_scene.addGeometry( std::move( geometry ) );
			m_bullets.emplace_back( speed, damage, *node, target );
		}
		else
		{
			auto bullet = *m_bulletsCache.begin();
			bullet.load( speed, damage, origin, target );
			m_bullets.insert( m_bullets.end(), bullet );
			m_bulletsCache.erase( m_bulletsCache.begin() );
		}
	}

	void Game::emitBoulder( float speed
		, uint32_t damage
		, castor::Point3f const & origin
		, castor::Point3f const & target )
	{
		if ( m_bouldersCache.empty() )
		{
			castor::String name = cuT( "Boulder_" ) + castor::string::toString( ++m_totalBoulders );
			auto node = m_scene.addNewSceneNode( name );
			auto geometry = m_scene.createGeometry( name
				, m_scene
				, *node
				, m_boulderMesh );
			node->setPosition( origin );
			node->attachTo( *m_mapNode );

			for ( auto submesh : *geometry->getMesh().lock() )
			{
				geometry->setMaterial( *submesh, m_boulderMaterial.lock().get() );
			}

			m_scene.addGeometry( std::move( geometry ) );
			m_boulders.emplace_back( speed, damage, *node, target );
		}
		else
		{
			auto boulder = *m_bouldersCache.begin();
			boulder.load( speed, damage, origin, target );
			m_boulders.insert( m_boulders.end(), boulder );
			m_bouldersCache.erase( m_bouldersCache.begin() );
		}
	}

	void Game::spend( uint32_t value )
	{
		if ( m_ore >= value )
		{
			m_ore -= value;
		}
		else
		{
			m_ore = 0;
		}
	}

	void Game::earn( uint32_t value )
	{
		m_ore += value;
	}

	void Game::loseLife( uint32_t value )
	{
		if ( m_lives > value )
		{
			m_lives -= value;
		}
		else
		{
			m_lives = 0;
			doGameOver();
		}
	}

	void Game::areaDamage( castor::Point3f const & position, uint32_t damage )
	{
		auto area = 32.0f;

		for ( auto & enemy : m_enemies )
		{
			auto distance = castor::point::distance( enemy->getNode().getDerivedPosition(), position );
			auto enemyRatio = distance / area;

			if ( enemyRatio <= 1.0 )
			{
				enemy->takeDamage( uint32_t( float( damage ) * ( 1.0f - enemyRatio ) ) );
			}
		}
	}

	TowerPtr Game::selectTower( Cell const & cell )
	{
		TowerPtr result;

		if ( cell.m_state == Cell::State::Tower )
		{
			auto it = std::find_if( m_towers.begin()
				, m_towers.end()
				, [&cell]( TowerPtr tower )
				{
					return tower->getCell().m_x == cell.m_x
						&& tower->getCell().m_y == cell.m_y;
				} );

			if ( it != m_towers.end() )
			{
				result = *it;
				m_selectedTower = result.get();
			}
		}

		return result;
	}

	void Game::upgradeTowerSpeed( Tower & tower )
	{
		if ( canAfford( tower.getSpeedUpgradeCost() )
			 && tower.canUpgradeSpeed() )
		{
			spend( tower.getSpeedUpgradeCost() );
			tower.upgradeSpeed();
		}
	}

	void Game::upgradeTowerRange( Tower & tower )
	{
		if ( canAfford( tower.getRangeUpgradeCost() )
			 && tower.canUpgradeRange() )
		{
			spend( tower.getRangeUpgradeCost() );
			tower.upgradeRange();
		}
	}

	void Game::upgradeTowerDamage( Tower & tower )
	{
		if ( canAfford( tower.getDamageUpgradeCost() )
			 && tower.canUpgradeDamage() )
		{
			spend( tower.getDamageUpgradeCost() );
			tower.upgradeDamage();
		}
	}

	bool Game::canAfford( uint32_t price )
	{
		return m_ore >= price;
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
		if ( m_enemies.empty() && m_spawner.isWaveEnded() )
		{
			earn( m_spawner.getWave() * 10 );
#if !defined( NDEBUG )
			m_spawner.startWave( 2u );
#else
			m_spawner.startWave( 10u );
#endif
		}

		if ( m_spawner.canSpawn( m_elapsed ) )
		{
			m_enemies.push_back( m_spawner.spawn( *this, m_path ) );
		}

		castor::Angle const angle{ castor::Angle::fromDegrees( float( -m_elapsed.count() ) * 120 / 1000.0f ) };
		auto it = m_enemies.begin();

		while ( it != m_enemies.end() )
		{
			auto enemy = *it;

			if ( enemy->isAlive() )
			{
				if ( enemy->getState() == Enemy::State::Walking )
				{
					enemy->getNode().yaw( angle );

					if ( enemy->accept( *this ) )
					{
						loseLife( 1u );
						m_spawner.killEnemy( *this, std::move( enemy ) );
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
				enemy->die();
				earn( enemy->getBounty() );
				m_spawner.killEnemy( *this, std::move( enemy ) );
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

	void Game::doUpdateBoulders()
	{
		auto it = m_boulders.begin();

		while ( it != m_boulders.end() )
		{
			if ( it->accept( *this ) )
			{
				m_bouldersCache.push_back( *it );
				it = m_boulders.erase( it );
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
				game::doPrepareGridLine( *prv, *cur, m_grid );

				if ( prv == m_path.begin() )
				{
					getCell( int( prv->m_x ), int( prv->m_y ) ).m_state = Cell::State::Start;
				}

				++prv;
				++cur;
			}

			game::doPrepareTarget( *prv, m_scene, m_grid );
		}
	}

	void Game::doAddMapCube( Cell & cell )
	{
		castor::String name = cuT( "MapCube_" ) + std::to_string( cell.m_x ) + cuT( "x" ) + std::to_string( cell.m_y );
		auto node = m_scene.addNewSceneNode( name );
		auto geometry = m_scene.createGeometry( name
			, m_scene
			, *node
			, m_mapCubeMesh );
		node->setPosition( convert( castor::Point2i{ cell.m_x, cell.m_y } ) + castor::Point3f{ 0, m_cellDimensions[1] / 2, 0 } );
		node->attachTo( *m_mapNode );

		for ( auto submesh : *geometry->getMesh().lock() )
		{
			geometry->setMaterial( *submesh, m_mapCubeMaterial.lock().get() );
		}

		m_lastMapCube = geometry.get();
		m_scene.addGeometry( std::move( geometry ) );
		cell.m_state = Cell::State::Empty;
	}

	void Game::doAddTarget( Cell & cell )
	{
		m_targetNode->setPosition( convert( castor::Point2i{ cell.m_x, cell.m_y + 1 } ) );
		cell.m_state = Cell::State::Target;
	}

	castor3d::MeshResPtr Game::doSelectMesh( Tower::Category & category )
	{
		castor3d::MeshResPtr result;

		switch ( category.getKind() )
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

	void Game::doAddTower( Cell & cell, Tower::CategoryPtr && category )
	{
		castor::String name = cuT( "Tower_" ) + std::to_string( cell.m_x ) + cuT( "x" ) + std::to_string( cell.m_y );
		auto node = m_scene.addNewSceneNode( name );
		node->setPosition( convert( castor::Point2i{ cell.m_x, cell.m_y } ) + castor::Point3f{ 0, m_cellDimensions[1], 0 } );
		node->attachTo( *m_mapNode );
		auto mesh = doSelectMesh( *category );
		auto tower = m_scene.createGeometry( name
			, m_scene
			, *node
			, mesh );
		auto animGroup = m_scene.addNewAnimatedObjectGroup( name
			, m_scene ).lock();
		castor::Milliseconds time{ 0 };

		if ( node->hasAnimation() )
		{
			animGroup->addObject( *node, tower->getName() );
		}

		if ( tower->getMesh().lock() )
		{
			auto tmesh = tower->getMesh().lock();

			if ( tmesh->hasAnimation() )
			{
			  animGroup->addObject( *tmesh, *tower, tower->getName() );
				time = std::max( time
					, tmesh->getAnimation( category->getAttackAnimationName() ).getLength() );
			}

			auto skeleton = tmesh->getSkeleton();

			if ( skeleton )
			{
				if ( skeleton->hasAnimation() )
				{
					animGroup->addObject( *skeleton, *tmesh, *tower, tower->getName() );
					time = std::max( time
						, skeleton->getAnimation( category->getAttackAnimationName() ).getLength() );
				}
			}
		}

		animGroup->addAnimation( category->getAttackAnimationName() );
		animGroup->setAnimationLooped( category->getAttackAnimationName(), false );
		game::doUpdateMaterials( *tower
			, category->getKind()
			, m_scene.getMaterialView() );
		m_scene.addGeometry( std::move( tower ) );
		cell.m_state = Cell::State::Tower;
		category->setAttackAnimationTime( time );
		animGroup->startAnimation( category->getAttackAnimationName() );
		animGroup->pauseAnimation( category->getAttackAnimationName() );
		node->setScale( castor::Point3f{ 0.15, 0.15, 0.15 } );
		std::clog << "Animation time: " << time.count() << std::endl;
		m_towers.push_back( std::make_shared< Tower >( std::move( category ), *node, *animGroup, cell ) );
	}

	void Game::doGameOver()
	{
		m_started = false;
		m_ended = true;
		m_hud.GameOver();
	}
}
