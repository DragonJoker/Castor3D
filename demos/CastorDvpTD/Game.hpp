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
		explicit Game( castor3d::Scene & scene );
		~Game();

		void reset();
		void start();
		void pause();
		void resume();
		void help();
		void update();
		Cell & getCell( castor::Point3f const & position );
		Cell & getCell( castor::Point2i const & position );
		Cell & getCell( int x, int y );
		Cell const & getCell( castor::Point3f const & position )const;
		Cell const & getCell( castor::Point2i const & position )const;
		Cell const & getCell( int x, int y )const;
		castor::Point3f convert( castor::Point2i const & position )const;
		castor::Point2i convert( castor::Point3f const & position )const;
		void emitBullet( float speed, uint32_t damage, castor::Point3f const & origin, Enemy & target );
		void emitBoulder( float speed, uint32_t damage, castor::Point3f const & origin, castor::Point3f const & target );
		bool buildTower( castor::Point3f const & position, Tower::CategoryPtr && category );
		void spend( uint32_t value );
		void earn( uint32_t value );
		void loseLife( uint32_t value );
		void areaDamage( castor::Point3f const & position, uint32_t damage );

		TowerPtr selectTower( Cell const & cell );
		void upgradeTowerSpeed( Tower & tower );
		void upgradeTowerRange( Tower & tower );
		void upgradeTowerDamage( Tower & tower );
		bool canAfford( uint32_t price );

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

		castor3d::Scene & getScene()const
		{
			return m_scene;
		}

		castor3d::SceneNodeRPtr getMapNode()const
		{
			return m_mapNode;
		}

		castor3d::MaterialObs getEnemyMaterial()const
		{
			return m_enemyCubeMaterial;
		}

		castor3d::MeshResPtr getEnemyMesh()const
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

		castor::Milliseconds getElapsed()const
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
		castor3d::MeshResPtr doSelectMesh( Tower::Category & category );
		void doAddTower( Cell & cell, Tower::CategoryPtr && category );
		void doUpdateTowers();
		void doUpdateEnemies();
		void doUpdateBullets();
		void doUpdateBoulders();
		void doGameOver();

	private:
		// Persistent data
		castor3d::Scene & m_scene;
		Hud m_hud;
		Path m_path;
		castor::Point3f m_cellDimensions;
		castor3d::SceneNodeRPtr m_mapNode{};
		castor3d::SceneNodeRPtr m_targetNode{};
		castor3d::MeshResPtr m_mapCubeMesh{};
		castor3d::MaterialObs m_mapCubeMaterial{};
		castor3d::MeshResPtr m_shortRangeTowerMesh{};
		castor3d::MeshResPtr m_longRangeTowerMesh{};
		castor3d::MeshResPtr m_enemyCubeMesh{};
		castor3d::MaterialObs m_enemyCubeMaterial{};
		castor3d::MeshResPtr m_bulletMesh{};
		castor3d::MaterialObs m_bulletMaterial{};
		castor3d::MeshResPtr m_boulderMesh{};
		castor3d::MaterialObs m_boulderMaterial{};
		castor3d::FramePassTimerUPtr m_updateTimer;
		// Varying data
		Clock::time_point m_saved;
		castor::Milliseconds m_elapsed;
		EnemySpawner m_spawner;
		Grid m_grid;
		TowerArray m_towers;
		EnemyArray m_enemies;
		BulletArray m_bullets;
		BulletArray m_bulletsCache;
		uint64_t m_totalBullets{ 0ull };
		BoulderArray m_boulders;
		BoulderArray m_bouldersCache;
		uint64_t m_totalBoulders{ 0ull };
		uint32_t m_lives{ 0u };
		uint32_t m_ore{ 0u };
		uint32_t m_kills{ 0u };
		bool m_started{ false };
		bool m_paused{ false };
		bool m_ended{ false };
		TowerRPtr m_selectedTower{};
		castor3d::GeometryRPtr m_lastMapCube{};
	};
}
