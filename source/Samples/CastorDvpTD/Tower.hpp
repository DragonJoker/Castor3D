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
				, castor::String const & p_attackAnimName )
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
				return m_initialCooldown.count() / float( m_cooldown.getValue().count() );
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

			inline uint32_t getDamageUpgradeCost()const
			{
				return m_damage.getCost();
			}

			inline uint32_t getRangeUpgradeCost()const
			{
				return m_range.getCost();
			}

			inline uint32_t getSpeedUpgradeCost()const
			{
				return m_cooldown.getCost();
			}

			inline uint32_t getTowerCost()const
			{
				return m_towerCost;
			}

			inline uint32_t getDamage()const
			{
				return m_damage.getValue();
			}

			inline float getRange()const
			{
				return m_range.getValue();
			}

			inline castor::Milliseconds getSpeed()const
			{
				return m_cooldown.getValue();
			}

			inline float getBulletSpeed()const
			{
				return m_bulletSpeed;
			}

			inline castor::String const & getMaterialName()const
			{
				return m_material;
			}

			inline castor::RgbColour const & getColour()const
			{
				return m_colour;
			}

			inline castor::String const & getAttackAnimationName()const
			{
				return m_attackAnimName;
			}

			inline castor::Milliseconds const & getAttackAnimationTime()const
			{
				return m_attackAnimTime;
			}

			inline Kind const & getKind()const
			{
				return m_kind;
			}

			inline void setAttackAnimationTime( castor::Milliseconds const & p_time )
			{
				m_attackAnimTime = p_time;
			}

		protected:
			Kind m_kind;
			PaidAbility< uint32_t > m_damage;
			PaidAbility< castor::Milliseconds > m_cooldown;
			castor::Milliseconds m_initialCooldown;
			PaidAbility< float > m_range;
			float m_bulletSpeed{ 0.0f };
			uint32_t m_towerCost{ 0u };
			castor::String m_material;
			castor::RgbColour m_colour;
			castor::String m_attackAnimName;
			castor::Milliseconds m_attackAnimTime;
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
			, castor3d::SceneNode & p_node
			, castor3d::AnimatedObjectGroup & p_anim
			, Cell const & p_cell );
		~Tower();

		void accept( Game & p_game );

		inline State getState()const
		{
			return m_state;
		}

		inline castor3d::SceneNode const & getNode()const
		{
			return m_node;
		}

		inline Cell const & getCell()const
		{
			return m_cell;
		}

		inline uint32_t getDamageUpgradeCost()const
		{
			return m_category->getDamageUpgradeCost();
		}

		inline uint32_t getRangeUpgradeCost()const
		{
			return m_category->getRangeUpgradeCost();
		}

		inline uint32_t getSpeedUpgradeCost()const
		{
			return m_category->getSpeedUpgradeCost();
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

		inline uint32_t getTowerCost()const
		{
			return m_category->getTowerCost();
		}

		inline uint32_t getDamage()const
		{
			return m_category->getDamage();
		}

		inline uint32_t getRange()const
		{
			return uint32_t( m_category->getRange() );
		}

		inline uint32_t getSpeed()const
		{
			return uint32_t( m_category->getSpeed().count() );
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
		bool doLookForEnemy( EnemyArray & p_enemies );
		bool doCanShoot();
		void doStartAttack();
		bool doAnimEnded( EnemyArray & p_enemies );
		void doShoot( Game & p_game );
		void doUpdateTimes( castor::Milliseconds const & p_elapsed );
		bool doIsInRange( Enemy const & p_enemy )const;
		void doTurnToTarget();

		inline castor3d::SceneNode & getNode()
		{
			return m_node;
		}

	private:
		castor3d::SceneNode & m_node;
		castor3d::AnimatedObjectGroup & m_anim;
		Cell const & m_cell;
		State m_state{ State::Idle };
		castor::Milliseconds m_remaining{ 0 };
		castor::Milliseconds m_animRemain{ 0 };
		EnemyPtr m_target{ nullptr };
		CategoryPtr m_category;
		float m_animScale{ 1.0f };
	};
}
