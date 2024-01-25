#pragma once

#include "CastorDvpTDPrerequisites.hpp"

namespace castortd
{
	class Boulder
	{
	public:
		Boulder( float speed
			, uint32_t damage
			, castor3d::SceneNode & node
			, castor::Point3f target );

		bool accept( Game & game );

		void load( float speed
			, uint32_t damage
			, castor::Point3f const & node
			, castor::Point3f target );

		castor3d::SceneNode & getNode()
		{
			return m_node;
		}

		castor3d::SceneNode const & getNode()const
		{
			return m_node;
		}

	private:
		castor::ReferenceWrapper< castor3d::SceneNode > m_node;
		castor::Point3f m_target;
		float m_speed{ 0.0f };
		uint32_t m_damage{ 0u };
	};
}
