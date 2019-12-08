#pragma once

#include "CastorDvpTDPrerequisites.hpp"

namespace castortd
{
	class Bullet
	{
	public:
		Bullet( float p_speed, uint32_t p_damage, castor3d::SceneNode & p_node, Enemy & p_enemy );
		~Bullet();

		bool accept( Game & p_game );

		void load( float p_speed, uint32_t p_damage, castor::Point3f const & p_node, Enemy & p_enemy );

		inline castor3d::SceneNode & getNode()
		{
			return m_node;
		}

		inline castor3d::SceneNode const & getNode()const
		{
			return m_node;
		}

	private:
		std::reference_wrapper< castor3d::SceneNode > m_node;
		std::reference_wrapper< Enemy > m_target;
		float m_speed{ 0.0f };
		uint32_t m_damage{ 0u };
	};
}
