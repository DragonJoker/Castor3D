#pragma once

#include "CastorDvpTDPrerequisites.hpp"

namespace castortd
{
	class Boulder
	{
	public:
		Boulder( float speed
			, uint32_t damage
			, c3d::SceneNode & node
			, c3d::Point3f target );

		bool accept( Game & game );

		void load( float speed
			, uint32_t damage
			, c3d::Point3f const & node
			, c3d::Point3f target );

		c3d::SceneNode & getNode()
		{
			return m_node;
		}

		c3d::SceneNode const & getNode()const
		{
			return m_node;
		}

	private:
		c3d::ReferenceWrapper< c3d::SceneNode > m_node;
		c3d::Point3f m_target;
		float m_speed{ 0.0f };
		uint32_t m_damage{ 0u };
	};
}
