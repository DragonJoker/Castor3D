#pragma once

#include "EnemyCategory.hpp"

namespace castortd
{
	class EnemySpawner
	{
	public:
		EnemySpawner();
		~EnemySpawner();

		void Reset();
		void StartWave( uint32_t m_count );
		bool CanSpawn( std::chrono::milliseconds const & p_elapsed );
		EnemyPtr Spawn( Game const & p_game, Path const & p_path );
		void KillEnemy( Game & p_game, EnemyPtr && p_enemy );

		inline uint32_t GetWave()const
		{
			return m_totalsWaves;
		}

		inline bool IsWaveEnded()const
		{
			return m_count == 0;
		}

		inline uint32_t GetEnemiesLife()const
		{
			return m_category.m_life.GetValue();
		}

		inline uint32_t GetEnemiesBounty()const
		{
			return m_category.m_bounty.GetValue();
		}

	private:
		BaseEnemy m_category;
		uint32_t m_count{ 0 };
		std::chrono::milliseconds m_timeBetweenTwoSpawns{ 0u };
		std::chrono::milliseconds m_timeSinceLastSpawn{ 0u };
		uint32_t m_totalsWaves{ 0ull };
		uint32_t m_totalSpawned{ 0ull };
		EnemyArray m_enemiesCache;
	};
}
