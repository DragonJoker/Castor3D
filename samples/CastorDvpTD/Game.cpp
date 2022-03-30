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
#if defined( NDEBUG )
#	if Cheat
		constexpr uint32_t InitialLives = 500u;
		constexpr uint32_t InitialOre = 75000u;
#	else
		constexpr uint32_t InitialLives = 5u;
		constexpr uint32_t InitialOre = 750u;
#	endif
#else
		constexpr uint32_t InitialLives = 1u;
		constexpr uint32_t InitialOre = 75000u;
#endif

		static void doPrepareGridLine( PathNode const & p_prv, PathNode const & p_cur, Grid & p_grid )
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

		static void doPrepareTarget( PathNode const & p_cur, castor3d::Scene & p_scene, Grid & p_grid )
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

		static void doUpdateMaterials( castor3d::Geometry & p_geometry
			, Tower::Category::Kind p_kind
			, castor3d::CacheViewT< castor3d::MaterialCache, castor3d::EventType::ePreRender > const & p_materials )
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
				p_geometry.setMaterial( *mesh.getSubmesh( 0u ), p_materials.find( cuT( "short_range_body" ) ).lock().get() );
				p_geometry.setMaterial( *mesh.getSubmesh( 1u ), p_materials.find( cuT( "short_range_accessories" ) ).lock().get() );
				break;
			}
		}
	}

	Game::Game( castor3d::Scene & p_scene )
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
		m_boulderMesh = m_scene.getMeshCache().find( cuT( "Boulder" ) );
		m_boulderMaterial = m_scene.getMaterialView().find( cuT( "Boulder" ) );
		m_targetNode = m_scene.getSceneNodeCache().find( cuT( "Target" ) ).lock();
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
		m_selectedTower.reset();
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

	Cell & Game::getCell( int p_x, int p_y )
	{
		return m_grid( uint32_t( p_y ), uint32_t( p_x ) );
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
		return m_grid( uint32_t( p_y )
			, uint32_t( p_x ) );
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

	bool Game::buildTower( castor::Point3f const & p_position, Tower::CategoryPtr && p_category )
	{
		bool result = false;

		if ( canAfford( p_category->getTowerCost() ) )
		{
			Cell & cell = getCell( p_position );

			if ( cell.m_state == Cell::State::Empty )
			{
				cell.m_state = Cell::State::Tower;
				spend( p_category->getTowerCost() );
				doAddTower( cell, std::move( p_category ) );
				result = true;
			}
		}

		return result;
	}

	castor::Point3f Game::convert( castor::Point2i const & p_position )const
	{
		return castor::Point3f( ( float( p_position[0] ) - float( m_grid.getWidth() ) / 2 ) * m_cellDimensions[0]
			, 0
			, ( float( p_position[1] ) - float( m_grid.getHeight() ) / 2 ) * m_cellDimensions[2] );
	}

	castor::Point2i Game::convert( castor::Point3f const & p_position )const
	{
		return castor::Point2i( int( p_position[0] / m_cellDimensions[0] + float( m_grid.getWidth() / 2 ) )
			, int( p_position[2] / m_cellDimensions[2] + float( m_grid.getHeight() / 2 ) ) );
	}

	void Game::emitBullet( float p_speed, uint32_t p_damage, castor::Point3f const & p_origin, Enemy & p_target )
	{
		if ( m_bulletsCache.empty() )
		{
			castor::String name = cuT( "Bullet_" ) + castor::string::toString( ++m_totalBullets );
			auto node = m_scene.getSceneNodeCache().add( name ).lock();
			auto geometry = m_scene.getGeometryCache().create( name
				, m_scene
				, *node
				, m_bulletMesh );
			node->setPosition( p_origin );
			node->attachTo( *m_mapNode );

			for ( auto submesh : *geometry->getMesh().lock() )
			{
				geometry->setMaterial( *submesh, m_bulletMaterial.lock().get() );
			}

			m_scene.getGeometryCache().add( geometry );
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

	void Game::emitBoulder( float speed
		, uint32_t damage
		, castor::Point3f const & origin
		, castor::Point3f const & target )
	{
		if ( m_bouldersCache.empty() )
		{
			castor::String name = cuT( "Boulder_" ) + castor::string::toString( ++m_totalBoulders );
			auto node = m_scene.getSceneNodeCache().add( name ).lock();
			auto geometry = m_scene.getGeometryCache().create( name
				, m_scene
				, *node
				, m_boulderMesh );
			node->setPosition( origin );
			node->attachTo( *m_mapNode );

			for ( auto submesh : *geometry->getMesh().lock() )
			{
				geometry->setMaterial( *submesh, m_boulderMaterial.lock().get() );
			}

			m_scene.getGeometryCache().add( geometry );
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

	void Game::spend( uint32_t p_value )
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

	void Game::earn( uint32_t p_value )
	{
		m_ore += p_value;
	}

	void Game::loseLife( uint32_t p_value )
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

	TowerPtr Game::selectTower( Cell const & p_cell )
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

	void Game::upgradeTowerSpeed( Tower & p_tower )
	{
		if ( canAfford( p_tower.getSpeedUpgradeCost() )
			 && p_tower.canUpgradeSpeed() )
		{
			spend( p_tower.getSpeedUpgradeCost() );
			p_tower.upgradeSpeed();
		}
	}

	void Game::upgradeTowerRange( Tower & p_tower )
	{
		if ( canAfford( p_tower.getRangeUpgradeCost() )
			 && p_tower.canUpgradeRange() )
		{
			spend( p_tower.getRangeUpgradeCost() );
			p_tower.upgradeRange();
		}
	}

	void Game::upgradeTowerDamage( Tower & p_tower )
	{
		if ( canAfford( p_tower.getDamageUpgradeCost() )
			 && p_tower.canUpgradeDamage() )
		{
			spend( p_tower.getDamageUpgradeCost() );
			p_tower.upgradeDamage();
		}
	}

	bool Game::canAfford( uint32_t p_price )
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

	void Game::doAddMapCube( Cell & p_cell )
	{
		castor::String name = cuT( "MapCube_" ) + std::to_string( p_cell.m_x ) + cuT( "x" ) + std::to_string( p_cell.m_y );
		auto node = m_scene.getSceneNodeCache().add( name ).lock();
		auto geometry = m_scene.getGeometryCache().create( name
			, m_scene
			, *node
			, m_mapCubeMesh );
		node->setPosition( convert( castor::Point2i{ p_cell.m_x, p_cell.m_y } ) + castor::Point3f{ 0, m_cellDimensions[1] / 2, 0 } );
		node->attachTo( *m_mapNode );

		for ( auto submesh : *geometry->getMesh().lock() )
		{
			geometry->setMaterial( *submesh, m_mapCubeMaterial.lock().get() );
		}

		m_scene.getGeometryCache().add( geometry );
		m_lastMapCube = geometry;
		p_cell.m_state = Cell::State::Empty;
	}

	void Game::doAddTarget( Cell & p_cell )
	{
		m_targetNode->setPosition( convert( castor::Point2i{ p_cell.m_x, p_cell.m_y + 1 } ) );
		p_cell.m_state = Cell::State::Target;
	}

	castor3d::MeshResPtr Game::doSelectMesh( Tower::Category & p_category )
	{
		castor3d::MeshResPtr result;

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
		castor::String name = cuT( "Tower_" ) + std::to_string( p_cell.m_x ) + cuT( "x" ) + std::to_string( p_cell.m_y );
		auto node = m_scene.getSceneNodeCache().add( name ).lock();
		node->setPosition( convert( castor::Point2i{ p_cell.m_x, p_cell.m_y } ) + castor::Point3f{ 0, m_cellDimensions[1], 0 } );
		node->attachTo( *m_mapNode );
		auto mesh = doSelectMesh( *p_category );
		auto tower = m_scene.getGeometryCache().create( name
			, m_scene
			, *node
			, mesh );
		auto animGroup = m_scene.getAnimatedObjectGroupCache().add( name
			, m_scene ).lock();
		castor::Milliseconds time{ 0 };

		if ( node->hasAnimation() )
		{
			animGroup->addObject( *node, tower->getName() + cuT( "_Node" ) );
		}

		if ( tower->getMesh().lock() )
		{
			auto tmesh = tower->getMesh().lock();

			if ( tmesh->hasAnimation() )
			{
			  animGroup->addObject( *tmesh, *tower, tower->getName() + cuT( "_Mesh" ) );
				time = std::max( time
					, tmesh->getAnimation( p_category->getAttackAnimationName() ).getLength() );
			}

			auto skeleton = tmesh->getSkeleton();

			if ( skeleton )
			{
				if ( skeleton->hasAnimation() )
				{
					animGroup->addObject( *skeleton, *tmesh, *tower, tower->getName() + cuT( "_Skeleton" ) );
					time = std::max( time
						, skeleton->getAnimation( p_category->getAttackAnimationName() ).getLength() );
				}
			}
		}

		animGroup->addAnimation( p_category->getAttackAnimationName() );
		animGroup->setAnimationLooped( p_category->getAttackAnimationName(), false );
		game::doUpdateMaterials( *tower
			, p_category->getKind()
			, m_scene.getMaterialView() );
		m_scene.getGeometryCache().add( tower );
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
