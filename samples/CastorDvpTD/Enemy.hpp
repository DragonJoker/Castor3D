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
			void upgrade()
			{
				m_life.upgrade();
				m_bounty.upgrade();
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
		Enemy( castor3d::SceneNode & node
			, Game const & game
			, Path const & path
			, Category const & category );

		void load( Game const & game );

		bool accept( Game const & game );

		void takeDamage( uint32_t damage )
		{
			if ( m_life > damage )
			{
				m_life -= damage;
			}
			else
			{
				m_life = 0;
				m_state = State::Dying;
			}
		}

		void die()
		{
			m_state = State::Dead;
			m_node.get().setPosition( castor::Point3f{ 0, -10, 0 } );
		}

		bool isAlive()const
		{
			return m_life > 0;
		}

		State getState()const
		{
			return m_state;
		}

		uint32_t getBounty()const
		{
			return m_category.get().m_bounty.getValue();
		}

		castor3d::SceneNode & getNode()
		{
			return m_node;
		}

		castor3d::SceneNode const & getNode()const
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
