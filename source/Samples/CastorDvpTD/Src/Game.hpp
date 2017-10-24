#pragma once

#include "Bullet.hpp"
#include "EnemySpawner.hpp"
#include "Hud.hpp"
#include "Tower.hpp"

namespace castortd
{
	class Game
	{
	public:
		explicit Game( castor3d::Scene & p_scene );
		~Game();

		void Reset();
		void start();
		void pause();
		void resume();
		void Help();
		void update();
		Cell & getCell( castor::Point3r const & p_position );
		Cell & getCell( castor::Point2i const & p_position );
		Cell & getCell( int p_x, int p_y );
		Cell const & getCell( castor::Point3r const & p_position )const;
		Cell const & getCell( castor::Point2i const & p_position )const;
		Cell const & getCell( int p_x, int p_y )const;
		castor::Point3r convert( castor::Point2i const & p_position )const;
		castor::Point2i convert( castor::Point3r const & p_position )const;
		void EmitBullet( float p_speed, uint32_t p_damage, castor::Point3r const & p_origin, Enemy & p_target );
		bool BuildTower( castor::Point3r const & p_position, Tower::CategoryPtr && p_category );
		void Spend( uint32_t p_value );
		void Gain( uint32_t p_value );
		void LoseLife( uint32_t p_value );

		TowerPtr SelectTower( Cell const & p_cell );
		void UpgradeTowerSpeed( Tower & p_tower );
		void UpgradeTowerRange( Tower & p_tower );
		void UpgradeTowerDamage( Tower & p_tower );
		bool CanAfford( uint32_t p_price );

		inline TowerPtr getSelectedTower()const
		{
			return m_selectedTower.lock();
		}

		inline float getCellHeight()const
		{
			return m_cellDimensions[2];
		}

		inline uint32_t getLives()const
		{
			return m_lives;
		}

		inline uint32_t getOre()const
		{
			return m_ore;
		}

		inline uint32_t getWave()const
		{
			return m_spawner.getWave();
		}

		inline uint32_t getKills()const
		{
			return m_kills;
		}

		inline bool IsStarted()const
		{
			return m_started;
		}

		inline bool IsRunning()const
		{
			return m_started && !m_paused;
		}

		inline bool isEnded()const
		{
			return m_ended;
		}

		inline bool isPaused()const
		{
			return m_paused;
		}

		inline castor3d::Scene & getScene()const
		{
			return m_scene;
		}

		inline castor3d::SceneNodeSPtr getMapNode()const
		{
			return m_mapNode;
		}

		inline castor3d::MaterialSPtr getEnemyMaterial()const
		{
			return m_enemyCubeMaterial;
		}

		inline castor3d::MeshSPtr getEnemyMesh()const
		{
			return m_enemyCubeMesh;
		}

		inline uint32_t getEnemiesLife()const
		{
			return m_spawner.getEnemiesLife();
		}

		inline uint32_t getEnemiesBounty()const
		{
			return m_spawner.getEnemiesBounty();
		}

		inline castor::Milliseconds getElapsed()const
		{
			return m_elapsed;
		}

		inline EnemyArray & getEnemies()
		{
			return m_enemies;
		}

		inline EnemyArray const & getEnemies()const
		{
			return m_enemies;
		}

	private:
		void doPrepareGrid();
		void doAddMapCube( Cell & p_cell );
		void doAddTarget( Cell & p_cell );
		castor3d::MeshSPtr doSelectMesh( Tower::Category & p_category );
		void doAddTower( Cell & p_cell, Tower::CategoryPtr && p_category );
		void doUpdateTowers();
		void doUpdateEnemies();
		void doUpdateBullets();
		void doGameOver();

	private:
		// Persistent data
		castor3d::Scene & m_scene;
		Hud m_hud;
		Path m_path;
		castor::Point3r m_cellDimensions;
		castor3d::SceneNodeSPtr m_mapNode;
		castor3d::SceneNodeSPtr m_targetNode;
		castor3d::MeshSPtr m_mapCubeMesh;
		castor3d::MaterialSPtr m_mapCubeMaterial;
		castor3d::MeshSPtr m_shortRangeTowerMesh;
		castor3d::MeshSPtr m_longRangeTowerMesh;
		castor3d::MeshSPtr m_enemyCubeMesh;
		castor3d::MaterialSPtr m_enemyCubeMaterial;
		castor3d::MeshSPtr m_bulletMesh;
		castor3d::MaterialSPtr m_bulletMaterial;
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
		uint32_t m_lives{ 0u };
		uint32_t m_ore{ 0u };
		uint32_t m_kills{ 0u };
		bool m_started{ false };
		bool m_paused{ false };
		bool m_ended{ false };
		TowerWPtr m_selectedTower;
		castor3d::GeometrySPtr m_lastMapCube;
	};
}
