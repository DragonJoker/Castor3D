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
			enum class Kind
			{
				eShortRange,
				eLongRange
			};

			Category( Kind p_kind
				, Castor::String const & p_attackAnimName )
				: m_kind{ p_kind }
				, m_attackAnimName{ p_attackAnimName }
				, m_attackAnimTime{}
			{
			}

			inline void UpgradeDamage()
			{
				m_damage.Upgrade();
			}

			inline void UpgradeRange()
			{
				m_range.Upgrade();
			}

			inline float UpgradeSpeed()
			{
				m_cooldown.Upgrade();
				return m_initialCooldown.count() / float( m_cooldown.GetValue().count() );
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

			inline Castor::Milliseconds GetSpeed()const
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

			inline Castor::String const & GetAttackAnimationName()const
			{
				return m_attackAnimName;
			}

			inline Castor::Milliseconds const & GetAttackAnimationTime()const
			{
				return m_attackAnimTime;
			}

			inline Kind const & GetKind()const
			{
				return m_kind;
			}

			inline void SetAttackAnimationTime( Castor::Milliseconds const & p_time )
			{
				m_attackAnimTime = p_time;
			}

		protected:
			Kind m_kind;
			PaidAbility< uint32_t > m_damage;
			PaidAbility< Castor::Milliseconds > m_cooldown;
			Castor::Milliseconds m_initialCooldown;
			PaidAbility< float > m_range;
			float m_bulletSpeed{ 0.0f };
			uint32_t m_towerCost{ 0u };
			Castor::String m_material;
			Castor::Colour m_colour;
			Castor::String m_attackAnimName;
			Castor::Milliseconds m_attackAnimTime;
		};

		using CategoryPtr = std::unique_ptr< Category >;

		enum class State
		{
			Idle,
			Spotted,
			Shooting,
		};

	public:
		Tower( CategoryPtr && p_category
			, Castor3D::SceneNode & p_node
			, Castor3D::AnimatedObjectGroup & p_anim
			, Cell const & p_cell );
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
			m_animScale = m_category->UpgradeSpeed();
		}

	private:
		bool DoLookForEnemy( EnemyArray & p_enemies );
		bool DoCanShoot();
		void DoStartAttack();
		bool DoAnimEnded( EnemyArray & p_enemies );
		void DoShoot( Game & p_game );
		void DoUpdateTimes( Castor::Milliseconds const & p_elapsed );
		bool DoIsInRange( Enemy const & p_enemy )const;
		void DoTurnToTarget();

		inline Castor3D::SceneNode & GetNode()
		{
			return m_node;
		}

	private:
		Castor3D::SceneNode & m_node;
		Castor3D::AnimatedObjectGroup & m_anim;
		Cell const & m_cell;
		State m_state{ State::Idle };
		Castor::Milliseconds m_remaining{ 0 };
		Castor::Milliseconds m_animRemain{ 0 };
		EnemyPtr m_target{ nullptr };
		CategoryPtr m_category;
		float m_animScale{ 1.0f };
	};
}
