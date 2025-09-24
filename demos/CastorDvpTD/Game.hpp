#pragma once

#include "Boulder.hpp"
#include "Bullet.hpp"
#include "EnemySpawner.hpp"
#include "Hud.hpp"
#include "Tower.hpp"

#include <Castor3D/Material/Material.hpp>

namespace castortd
{
	class Game
	{
	public:
		explicit Game( c3d::Scene & scene );

		void reset();
		void start();
		void pause();
		void resume();
		void help();
		void update();
		Cell & getCell( c3d::Point3f const & position );
		Cell & getCell( c3d::Point2i const & position );
		Cell & getCell( int x, int y );
		Cell const & getCell( c3d::Point3f const & position )const;
		Cell const & getCell( c3d::Point2i const & position )const;
		Cell const & getCell( int x, int y )const;
		c3d::Point3f convert( c3d::Point2i const & position )const;
		c3d::Point2i convert( c3d::Point3f const & position )const;
		void emitBullet( float speed, uint32_t damage, c3d::Point3f const & origin, Enemy & target );
		void emitBoulder( float speed, uint32_t damage, c3d::Point3f const & origin, c3d::Point3f const & target );
		bool buildTower( c3d::Point3f const & position, Tower::CategoryPtr && category );
		void spend( uint32_t value );
		void earn( uint32_t value );
		void loseLife( uint32_t value );
		void areaDamage( c3d::Point3f const & position, uint32_t damage )const;

		TowerPtr selectTower( Cell const & cell );
		void upgradeTowerSpeed( Tower & tower );
		void upgradeTowerRange( Tower & tower );
		void upgradeTowerDamage( Tower & tower );
		bool canAfford( uint32_t price )const;

		Tower * getSelectedTower()const
		{
			return m_selectedTower;
		}

		float getCellHeight()const
		{
			return m_cellDimensions[2];
		}

		uint32_t getLives()const
		{
			return m_lives;
		}

		uint32_t getOre()const
		{
			return m_ore;
		}

		uint32_t getWave()const
		{
			return m_spawner.getWave();
		}

		uint32_t getKills()const
		{
			return m_kills;
		}

		bool isStarted()const
		{
			return m_started;
		}

		bool isRunning()const
		{
			return m_started && !m_paused;
		}

		bool isEnded()const
		{
			return m_ended;
		}

		bool isPaused()const
		{
			return m_paused;
		}

		c3d::Scene & getScene()const
		{
			return m_scene;
		}

		c3d::SceneNodeRPtr getMapNode()const
		{
			return m_mapNode;
		}

		c3d::MaterialObs getEnemyMaterial()const
		{
			return m_enemyCubeMaterial;
		}

		c3d::MeshResPtr getEnemyMesh()const
		{
			return m_enemyCubeMesh;
		}

		uint32_t getEnemiesLife()const
		{
			return m_spawner.getEnemiesLife();
		}

		uint32_t getEnemiesBounty()const
		{
			return m_spawner.getEnemiesBounty();
		}

		c3d::Milliseconds getElapsed()const
		{
			return m_elapsed;
		}

		EnemyArray & getEnemies()
		{
			return m_enemies;
		}

		EnemyArray const & getEnemies()const
		{
			return m_enemies;
		}

	private:
		void doPrepareGrid();
		void doAddMapCube( Cell & cell );
		void doAddTarget( Cell & cell );
		c3d::MeshResPtr doSelectMesh( Tower::Category const & category );
		void doAddTower( Cell & cell, Tower::CategoryPtr category );
		void doUpdateTowers();
		void doUpdateEnemies();
		void doUpdateBullets();
		void doUpdateBoulders();
		void doGameOver();

	private:
		// Persistent data
		c3d::Scene & m_scene;
		Hud m_hud;
		Path m_path;
		c3d::Point3f m_cellDimensions;
		c3d::SceneNodeRPtr m_mapNode{};
		c3d::SceneNodeRPtr m_targetNode{};
		c3d::MeshResPtr m_mapCubeMesh{};
		c3d::MaterialObs m_mapCubeMaterial{};
		c3d::MeshResPtr m_shortRangeTowerMesh{};
		c3d::MeshResPtr m_longRangeTowerMesh{};
		c3d::MeshResPtr m_enemyCubeMesh{};
		c3d::MaterialObs m_enemyCubeMaterial{};
		c3d::MeshResPtr m_bulletMesh{};
		c3d::MaterialObs m_bulletMaterial{};
		c3d::MeshResPtr m_boulderMesh{};
		c3d::MaterialObs m_boulderMaterial{};
		c3d::FramePassTimerUPtr m_updateTimer;
		// Varying data
		Clock::time_point m_saved;
		c3d::Milliseconds m_elapsed;
		EnemySpawner m_spawner;
		Grid m_grid;
		TowerArray m_towers;
		EnemyArray m_enemies;
		BulletArray m_bullets;
		BulletArray m_bulletsCache;
		uint64_t m_totalBullets{ 0u };
		BoulderArray m_boulders;
		BoulderArray m_bouldersCache;
		uint64_t m_totalBoulders{ 0u };
		uint32_t m_lives{ 0u };
		uint32_t m_ore{ 0u };
		uint32_t m_kills{ 0u };
		bool m_started{ false };
		bool m_paused{ false };
		bool m_ended{ false };
		TowerRPtr m_selectedTower{};
		c3d::GeometryRPtr m_lastMapCube{};
	};
}
