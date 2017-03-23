#pragma once

#include "CastorDvpTDPrerequisites.hpp"

namespace castortd
{
	class Bullet
	{
	public:
		Bullet( float p_speed, uint32_t p_damage, Castor3D::SceneNode & p_node, Enemy & p_enemy );
		~Bullet();

		bool Accept( Game & p_game );

		void Load( float p_speed, uint32_t p_damage, Castor::Point3r const & p_node, Enemy & p_enemy );

		inline Castor3D::SceneNode & GetNode()
		{
			return m_node;
		}

		inline Castor3D::SceneNode const & GetNode()const
		{
			return m_node;
		}

	private:
		std::reference_wrapper< Castor3D::SceneNode > m_node;
		std::reference_wrapper< Enemy > m_target;
		float m_speed{ 0.0f };
		uint32_t m_damage{ 0u };
	};
}
