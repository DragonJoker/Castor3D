#pragma once

#include "EnemyCategory.hpp"

namespace castortd
{
	class EnemySpawner
	{
	public:
		EnemySpawner();

		void reset();
		void startWave( uint32_t count );
		bool canSpawn( castor::Milliseconds const & elapsed );
		EnemyPtr spawn( Game const & game, Path const & path );
		void killEnemy( Game & game, EnemyPtr && enemy );

		uint32_t getWave()const
		{
			return m_totalsWaves;
		}

		bool isWaveEnded()const
		{
			return m_count == 0;
		}

		uint32_t getEnemiesLife()const
		{
			return m_category.m_life.getValue();
		}

		uint32_t getEnemiesBounty()const
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
