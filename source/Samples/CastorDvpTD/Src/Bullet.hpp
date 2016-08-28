#pragma once

#include "CastorDvpTDPrerequisites.hpp"

namespace castortd
{
	class Bullet
	{
	public:
		Bullet( uint32_t p_damage, Castor3D::SceneNode & p_node, Enemy & p_enemy );
		~Bullet();

		bool Move( std::chrono::milliseconds const & p_elapsed );

		void Load( uint32_t p_damage, Castor::Point3r const & p_node, Enemy & p_enemy );

	private:
		std::reference_wrapper< Castor3D::SceneNode > m_node;
		std::reference_wrapper< Enemy > m_target;
		float m_speed{ 48.0f };
		uint32_t m_damage{ 0u };
	};
}
