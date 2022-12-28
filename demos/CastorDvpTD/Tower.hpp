#pragma once

#include "Ability.hpp"

#include <CastorUtils/Graphics/RgbColour.hpp>

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

			Category( Kind kind
				, castor::String const & attackAnimName )
				: m_kind{ kind }
				, m_attackAnimName{ attackAnimName }
				, m_attackAnimTime{}
			{
			}

			void upgradeDamage()
			{
				m_damage.upgrade();
			}

			void upgradeRange()
			{
				m_range.upgrade();
			}

			void upgradeSpeed()
			{
				m_speed.upgrade();
			}

			bool canUpgradeDamage()
			{
				return m_damage.canUpgrade();
			}

			bool canUpgradeRange()
			{
				return m_range.canUpgrade();
			}

			bool canUpgradeSpeed()
			{
				return m_speed.canUpgrade();
			}

			uint32_t getDamageUpgradeCost()const
			{
				return m_damage.getCost();
			}

			uint32_t getRangeUpgradeCost()const
			{
				return m_range.getCost();
			}

			uint32_t getSpeedUpgradeCost()const
			{
				return m_speed.getCost();
			}

			uint32_t getTowerCost()const
			{
				return m_towerCost;
			}

			uint32_t getDamage()const
			{
				return m_damage.getValue();
			}

			float getRange()const
			{
				return m_range.getValue();
			}

			float getSpeed()const
			{
				return m_speed.getValue();
			}

			float getBulletSpeed()const
			{
				return m_bulletSpeed;
			}

			castor::String const & getAttackAnimationName()const
			{
				return m_attackAnimName;
			}

			castor::Milliseconds const & getAttackAnimationTime()const
			{
				return m_attackAnimTime;
			}

			Kind const & getKind()const
			{
				return m_kind;
			}

			void setAttackAnimationTime( castor::Milliseconds const & time )
			{
				m_attackAnimTime = time;
			}

		protected:
			Kind m_kind;
			PaidAbility< uint32_t > m_damage;
			PaidAbility< float > m_speed;
			castor::Milliseconds m_initialCooldown{};
			PaidAbility< float > m_range;
			float m_bulletSpeed{ 0.0f };
			uint32_t m_towerCost{ 0u };
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
		Tower( CategoryPtr && category
			, castor3d::SceneNode & node
			, castor3d::AnimatedObjectGroup & anim
			, Cell const & cell );

		void accept( Game & game );

		State getState()const
		{
			return m_state;
		}

		castor3d::SceneNode const & getNode()const
		{
			return m_node;
		}

		Cell const & getCell()const
		{
			return m_cell;
		}

		uint32_t getDamageUpgradeCost()const
		{
			return m_category->getDamageUpgradeCost();
		}

		uint32_t getRangeUpgradeCost()const
		{
			return m_category->getRangeUpgradeCost();
		}

		uint32_t getSpeedUpgradeCost()const
		{
			return m_category->getSpeedUpgradeCost();
		}

		bool canUpgradeDamage()
		{
			return m_category->canUpgradeDamage();
		}

		bool canUpgradeRange()
		{
			return m_category->canUpgradeRange();
		}

		bool canUpgradeSpeed()
		{
			return m_category->canUpgradeSpeed();
		}

		uint32_t getTowerCost()const
		{
			return m_category->getTowerCost();
		}

		uint32_t getDamage()const
		{
			return m_category->getDamage();
		}

		uint32_t getRange()const
		{
			return uint32_t( m_category->getRange() );
		}

		float getSpeed()const
		{
			return m_category->getSpeed();
		}

		void upgradeDamage()
		{
			m_category->upgradeDamage();
		}

		void upgradeRange()
		{
			m_category->upgradeRange();
		}

		void upgradeSpeed()
		{
			m_category->upgradeSpeed();
			m_animScale = getSpeed();
		}

	private:
		bool doLookForEnemy( EnemyArray & enemies );
		bool doCanShoot();
		void doStartAttack();
		bool doAnimEnded( EnemyArray & enemies );
		void doShoot( Game & game );
		void doUpdateTimes( castor::Milliseconds const & elapsed );
		bool doIsInRange( Enemy const & enemy )const;
		void doTurnToTarget();

		castor3d::SceneNode & getNode()
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
