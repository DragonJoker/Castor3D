#pragma once

#include "Ability.hpp"

namespace castortd
{
	class Tower
	{
		friend class Game;

	public:
		struct Category
		{
			inline void UpgradeDamage()
			{
				m_damage.Upgrade();
			}

			inline void UpgradeRange()
			{
				m_range.Upgrade();
			}

			inline void UpgradeSpeed()
			{
				m_cooldown.Upgrade();
			}

			inline bool CanUpgradeDamage()
			{
				return m_damage.CanUpgrade();
			}

			inline bool CanUpgradeRange()
			{
				return m_range.CanUpgrade();
			}

			inline bool CanUpgradeSpeed()
			{
				return m_cooldown.CanUpgrade();
			}

			inline uint32_t GetDamageUpgradeCost()const
			{
				return m_damage.GetCost();
			}

			inline uint32_t GetRangeUpgradeCost()const
			{
				return m_range.GetCost();
			}

			inline uint32_t GetSpeedUpgradeCost()const
			{
				return m_cooldown.GetCost();
			}

			inline uint32_t GetTowerCost()const
			{
				return m_towerCost;
			}

			inline uint32_t GetDamage()const
			{
				return m_damage.GetValue();
			}

			inline float GetRange()const
			{
				return m_range.GetValue();
			}

			inline std::chrono::milliseconds GetSpeed()const
			{
				return m_cooldown.GetValue();
			}

			inline float GetBulletSpeed()const
			{
				return m_bulletSpeed;
			}

			inline Castor::String const & GetMaterialName()const
			{
				return m_material;
			}

			inline Castor::Colour const & GetColour()const
			{
				return m_colour;
			}

		protected:
			PaidAbility< uint32_t > m_damage;
			PaidAbility< std::chrono::milliseconds > m_cooldown;
			PaidAbility< float > m_range;
			float m_bulletSpeed{ 0.0f };
			uint32_t m_towerCost{ 0u };
			Castor::String m_material;
			Castor::Colour m_colour;
		};

		using CategoryPtr = std::unique_ptr< Category >;

		enum class State
		{
			Idle,
			Spotted,
		};

	public:
		Tower( CategoryPtr && p_category, Castor3D::SceneNode & p_node, Cell const & p_cell );
		~Tower();

		void Accept( Game & p_game );

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
			return m_category->GetDamageUpgradeCost();
		}

		inline uint32_t GetRangeUpgradeCost()const
		{
			return m_category->GetRangeUpgradeCost();
		}

		inline uint32_t GetSpeedUpgradeCost()const
		{
			return m_category->GetSpeedUpgradeCost();
		}

		inline bool CanUpgradeDamage()
		{
			return m_category->CanUpgradeDamage();
		}

		inline bool CanUpgradeRange()
		{
			return m_category->CanUpgradeRange();
		}

		inline bool CanUpgradeSpeed()
		{
			return m_category->CanUpgradeSpeed();
		}

		inline uint32_t GetTowerCost()const
		{
			return m_category->GetTowerCost();
		}

		inline uint32_t GetDamage()const
		{
			return m_category->GetDamage();
		}

		inline uint32_t GetRange()const
		{
			return uint32_t( m_category->GetRange() );
		}

		inline uint32_t GetSpeed()const
		{
			return uint32_t( m_category->GetSpeed().count() );
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
			m_category->UpgradeDamage();
		}

		inline void UpgradeRange()
		{
			m_category->UpgradeRange();
		}

		inline void UpgradeSpeed()
		{
			m_category->UpgradeSpeed();
		}

	private:
		Castor3D::SceneNode & m_node;
		Cell const & m_cell;
		State m_state{ State::Idle };
		std::chrono::milliseconds m_remaining{ 0 };
		EnemyPtr m_target{ nullptr };
		CategoryPtr m_category;
	};
}
