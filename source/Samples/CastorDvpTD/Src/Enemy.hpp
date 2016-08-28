#pragma once

#include "Grid.hpp"

namespace castortd
{
	class Enemy
	{
	public:
		struct Category
		{
			float m_speed;
			uint32_t m_life;
			bool m_flying;
			uint32_t m_bounty;
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
		Enemy( Castor3D::SceneNode & p_node, Game const & p_game, Path const & p_path, Category const & p_category );
		~Enemy();

		void Load( Game const & p_game, Category const & p_category );

		bool Walk( Game const & p_game, std::chrono::milliseconds const & p_elapsed );

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
			m_node.get().SetPosition( Castor::Point3r{ 0, -10, 0 } );
		}

		inline bool IsAlive()const
		{
			return m_life > 0;
		}

		inline State GetState()const
		{
			return m_state;
		}

		inline uint32_t GetBounty()const
		{
			return m_category.get().m_bounty;
		}

		inline Castor3D::SceneNode & GetNode()
		{
			return m_node;
		}

		inline Castor3D::SceneNode const & GetNode()const
		{
			return m_node;
		}

	private:
		std::reference_wrapper< Path const > m_path;
		std::reference_wrapper< Castor3D::SceneNode > m_node;
		std::reference_wrapper< Category const > m_category;
		State m_state{ State::Walking };
		float m_speed{ 0.0f };
		uint32_t m_life{ 0 };
		Path::const_iterator m_cur;
		Castor::Point3r m_destination;
	};
}
