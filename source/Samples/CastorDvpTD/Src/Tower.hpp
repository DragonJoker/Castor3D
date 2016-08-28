#pragma once

#include "CastorDvpTDPrerequisites.hpp"

namespace castortd
{
	class Tower
	{
		friend class Game;

	public:
		struct Category
		{
			uint32_t m_damage{ 3u };
			std::chrono::milliseconds m_cooldown{ 1000 };
			float m_range{ 100.0f };
			uint32_t m_damageCost{ 500u };
			uint32_t m_cooldownCost{ 250u };
			uint32_t m_rangeCost{ 120u };
		};

		enum class State
		{
			Idle,
			Spotted,
		};

	public:
		Tower( Castor3D::SceneNode & p_node, Cell const & p_cell );
		~Tower();

		void Update( std::chrono::milliseconds const & p_elapsed, EnemyArray & p_enemies );

		inline State GetState()const
		{
			return m_state;
		}

		inline Castor3D::SceneNode const & GetNode()const
		{
			return m_node;
		}

		inline Cell const & GetCell()const
		{
			return m_cell;
		}

		inline uint32_t GetDamageUpgradeCost()const
		{
			return m_category.m_damageCost;
		}

		inline uint32_t GetRangeUpgradeCost()const
		{
			return m_category.m_rangeCost;
		}

		inline uint32_t GetSpeedUpgradeCost()const
		{
			return m_category.m_cooldownCost;
		}

		inline uint32_t GetDamage()const
		{
			return m_category.m_damage;
		}

		inline uint32_t GetRange()const
		{
			return uint32_t( m_category.m_range );
		}

		inline uint32_t GetSpeed()const
		{
			return uint32_t( m_category.m_cooldown.count() );
		}

	private:
		bool LookForEnemy( EnemyArray & p_enemies );
		bool CanShoot();
		void Shoot( Game & p_game );

		bool DoIsInRange( Enemy const & p_enemy )const;

		inline Castor3D::SceneNode & GetNode()
		{
			return m_node;
		}

		inline void UpgradeDamage()
		{
			++m_category.m_damage;
			m_category.m_damageCost += 10;
		}

		inline void UpgradeRange()
		{
			m_category.m_range += 20.0f;
			m_category.m_rangeCost += 10;
		}

		inline void UpgradeSpeed()
		{
			m_category.m_cooldown -= std::chrono::milliseconds{ 40 };

			if ( m_category.m_cooldown < std::chrono::milliseconds{ 40 } )
			{
				m_category.m_cooldown = std::chrono::milliseconds{ 40 };
			}

			m_category.m_cooldownCost += 10;
		}

	private:
		Castor3D::SceneNode & m_node;
		Cell const & m_cell;
		State m_state{ State::Idle };
		std::chrono::milliseconds m_remaining{ 0 };
		EnemyPtr m_target{ nullptr };
		Category m_category;
	};
}
