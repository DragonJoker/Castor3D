#pragma once

#include "Ability.hpp"
#include "Grid.hpp"

#include <Castor3D/Scene/SceneNode.hpp>

namespace castortd
{
	class Enemy
	{
	public:
		struct Category
		{
			inline void Upgrade()
			{
				m_life.Upgrade();
				m_bounty.Upgrade();
			}

			float m_speed;
			bool m_flying;
			Ability< uint32_t > m_life;
			Ability< uint32_t > m_bounty;
		};

		enum class State
		{
			Walking,
			Frozen,
			Dying,
			Dead,
			Arrived,
		};

	public:
		Enemy( castor3d::SceneNode & p_node, Game const & p_game, Path const & p_path, Category const & p_category );
		~Enemy();

		void load( Game const & p_game );

		bool accept( Game const & p_game );

		inline void TakeDamage( uint32_t p_damage )
		{
			if ( m_life > p_damage )
			{
				m_life -= p_damage;
			}
			else
			{
				m_life = 0;
				m_state = State::Dying;
			}
		}

		inline void Die()
		{
			m_state = State::Dead;
			m_node.get().setPosition( castor::Point3f{ 0, -10, 0 } );
		}

		inline bool IsAlive()const
		{
			return m_life > 0;
		}

		inline State getState()const
		{
			return m_state;
		}

		inline uint32_t getBounty()const
		{
			return m_category.get().m_bounty.getValue();
		}

		inline castor3d::SceneNode & getNode()
		{
			return m_node;
		}

		inline castor3d::SceneNode const & getNode()const
		{
			return m_node;
		}

	private:
		std::reference_wrapper< Path const > m_path;
		std::reference_wrapper< castor3d::SceneNode > m_node;
		std::reference_wrapper< Category const > m_category;
		State m_state{ State::Walking };
		float m_speed{ 0.0f };
		uint32_t m_life{ 0 };
		Path::const_iterator m_cur;
		castor::Point3f m_destination;
	};
}
