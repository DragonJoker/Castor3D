#pragma once

#include "Enemy.hpp"

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

		inline uint32_t GetWave()const
		{
			return m_totalsWaves;
		}

		inline uint32_t GetEnemiesLife()const
		{
			return m_category.m_life;
		}

	private:
		Enemy::Category m_category{ 24.0f, 0u, false, 9u };
		uint32_t m_count{ 0 };
		std::chrono::milliseconds m_timeBetweenTwoSpawns{ 0u };
		std::chrono::milliseconds m_timeSinceLastSpawn{ 0u };
		uint32_t m_totalsWaves{ 0ull };
		uint32_t m_totalSpawned{ 0ull };
	};
}
