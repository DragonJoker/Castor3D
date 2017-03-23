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
		Game( Castor3D::Scene & p_scene );
		~Game();

		void Reset();
		void Start();
		void Pause();
		void Resume();
		void Help();
		void Update();
		Cell & GetCell( Castor::Point3r const & p_position );
		Cell & GetCell( Castor::Point2i const & p_position );
		Cell & GetCell( int p_x, int p_y );
		Cell const & GetCell( Castor::Point3r const & p_position )const;
		Cell const & GetCell( Castor::Point2i const & p_position )const;
		Cell const & GetCell( int p_x, int p_y )const;
		Castor::Point3r Convert( Castor::Point2i const & p_position )const;
		Castor::Point2i Convert( Castor::Point3r const & p_position )const;
		void EmitBullet( float p_speed, uint32_t p_damage, Castor::Point3r const & p_origin, Enemy & p_target );
		bool BuildTower( Castor::Point3r const & p_position, Tower::CategoryPtr && p_category );
		void Spend( uint32_t p_value );
		void Gain( uint32_t p_value );
		void LoseLife( uint32_t p_value );

		TowerPtr SelectTower( Cell const & p_cell );
		void UpgradeTowerSpeed( Tower & p_tower );
		void UpgradeTowerRange( Tower & p_tower );
		void UpgradeTowerDamage( Tower & p_tower );
		bool CanAfford( uint32_t p_price );

		inline TowerPtr GetSelectedTower()const
		{
			return m_selectedTower.lock();
		}

		inline float GetCellHeight()const
		{
			return m_cellDimensions[2];
		}

		inline uint32_t GetLives()const
		{
			return m_lives;
		}

		inline uint32_t GetOre()const
		{
			return m_ore;
		}

		inline uint32_t GetWave()const
		{
			return m_spawner.GetWave();
		}

		inline uint32_t GetKills()const
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

		inline bool IsEnded()const
		{
			return m_ended;
		}

		inline bool IsPaused()const
		{
			return m_paused;
		}

		inline Castor3D::Scene & GetScene()const
		{
			return m_scene;
		}

		inline Castor3D::SceneNodeSPtr GetMapNode()const
		{
			return m_mapNode;
		}

		inline Castor3D::MaterialSPtr GetEnemyMaterial()const
		{
			return m_enemyCubeMaterial;
		}

		inline Castor3D::MeshSPtr GetEnemyMesh()const
		{
			return m_enemyCubeMesh;
		}

		inline uint32_t GetEnemiesLife()const
		{
			return m_spawner.GetEnemiesLife();
		}

		inline uint32_t GetEnemiesBounty()const
		{
			return m_spawner.GetEnemiesBounty();
		}

		inline std::chrono::milliseconds GetElapsed()const
		{
			return m_elapsed;
		}

		inline EnemyArray & GetEnemies()
		{
			return m_enemies;
		}

		inline EnemyArray const & GetEnemies()const
		{
			return m_enemies;
		}

	private:
		void DoPrepareGrid();
		void DoAddMapCube( Cell & p_cell );
		void DoAddTarget( Cell & p_cell );
		void DoAddTower( Cell & p_cell, Tower::CategoryPtr && p_category );
		void DoUpdateTowers();
		void DoUpdateEnemies();
		void DoUpdateBullets();
		void DoGameOver();

	private:
		// Persistent data
		Castor3D::Scene & m_scene;
		Hud m_hud;
		Path m_path;
		Castor::Point3r m_cellDimensions;
		Castor3D::SceneNodeSPtr m_mapNode;
		Castor3D::SceneNodeSPtr m_targetNode;
		Castor3D::MeshSPtr m_mapCubeMesh;
		Castor3D::MaterialSPtr m_mapCubeMaterial;
		Castor3D::MeshSPtr m_towerCubeMesh;
		Castor3D::MeshSPtr m_enemyCubeMesh;
		Castor3D::MaterialSPtr m_enemyCubeMaterial;
		Castor3D::MeshSPtr m_bulletMesh;
		Castor3D::MaterialSPtr m_bulletMaterial;
		// Varying data
		Clock::time_point m_saved;
		std::chrono::milliseconds m_elapsed;
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
	};
}
