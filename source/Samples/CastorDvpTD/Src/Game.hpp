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

		void Start();
		void Update();
		Cell & GetCell( Castor::Point3r const & p_position );
		Cell & GetCell( Castor::Point2i const & p_position );
		Cell & GetCell( int p_x, int p_y );
		Cell const & GetCell( Castor::Point3r const & p_position )const;
		Cell const & GetCell( Castor::Point2i const & p_position )const;
		Cell const & GetCell( int p_x, int p_y )const;
		Castor::Point3r Convert( Castor::Point2i const & p_position )const;
		Castor::Point2i Convert( Castor::Point3r const & p_position )const;
		void EmitBullet( uint32_t p_damage, Castor::Point3r const & p_origin, Enemy & p_target );
		bool CanBuildTower();
		bool BuildTower( Castor::Point3r const & p_position );
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
			return m_started || !m_lives;
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

		inline uint32_t GetTowerCost()const
		{
			return m_towerPrice;
		}

		inline uint32_t GetEnemiesLife()const
		{
			return m_spawner.GetEnemiesLife();
		}

	private:
		void DoPrepareGrid();
		void DoAddMapCube( Cell & p_cell );
		void DoAddTarget( Cell & p_cell );
		void DoAddTower( Cell & p_cell );
		void DoUpdateTowers( std::chrono::milliseconds const & p_elapsed );
		void DoUpdateEnemies( std::chrono::milliseconds const & p_elapsed );
		void DoUpdateBullets( std::chrono::milliseconds const & p_elapsed );
		void DoGameOver();

	private:
		Path m_path;
		Grid m_grid;
		TowerArray m_towers;
		EnemyArray m_enemies;
		EnemyArray m_enemiesCache;
		BulletArray m_bullets;
		BulletArray m_bulletsCache;
		Castor3D::Scene & m_scene;
		Castor::Point3r m_cellDimensions;
		Castor3D::SceneNodeSPtr m_mapNode;
		Castor3D::SceneNodeSPtr m_targetNode;
		Castor3D::MeshSPtr m_mapCubeMesh;
		Castor3D::MaterialSPtr m_mapCubeMaterial;
		Castor3D::MaterialSPtr m_mapCubeShadowMaterial;
		Castor3D::MeshSPtr m_towerCubeMesh;
		Castor3D::MaterialSPtr m_towerCubeMaterial;
		Castor3D::MeshSPtr m_enemyCubeMesh;
		Castor3D::MaterialSPtr m_enemyCubeMaterial;
		Castor3D::MeshSPtr m_bulletMesh;
		Castor3D::MaterialSPtr m_bulletMaterial;
		Clock::time_point m_saved;
		std::vector< Enemy::Category > m_enemyCategories;
		EnemySpawner m_spawner;
		uint64_t m_totalBullets{ 0ull };
		uint32_t m_lives{ 50u };
		uint32_t m_ore{ 750u };
		uint32_t m_kills{ 0u };
		Hud m_hud;
		uint32_t m_towerPrice{ 300 };
		bool m_started{ false };
		TowerWPtr m_selectedTower;
	};
}
