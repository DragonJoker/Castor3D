#pragma once

#include "EnemyCategory.hpp"

namespace castortd
{
	class EnemySpawner
	{
	public:
		EnemySpawner();

		void Reset();
		void StartWave( uint32_t m_count );
		bool CanSpawn( castor::Milliseconds const & p_elapsed );
		EnemyPtr Spawn( Game const & p_game, Path const & p_path );
		void KillEnemy( Game & p_game, EnemyPtr && p_enemy );

		inline uint32_t getWave()const
		{
			return m_totalsWaves;
		}

		inline bool IsWaveEnded()const
		{
			return m_count == 0;
		}

		inline uint32_t getEnemiesLife()const
		{
			return m_category.m_life.getValue();
		}

		inline uint32_t getEnemiesBounty()const
		{
			return m_category.m_bounty.getValue();
		}

	private:
		BaseEnemy m_category;
		uint32_t m_count{ 0 };
		castor::Milliseconds m_timeBetweenTwoSpawns{ 0u };
		castor::Milliseconds m_timeSinceLastSpawn{ 0u };
		uint32_t m_totalsWaves{ 0ull };
		uint32_t m_totalSpawned{ 0ull };
		EnemyArray m_enemiesCache;
	};
}
