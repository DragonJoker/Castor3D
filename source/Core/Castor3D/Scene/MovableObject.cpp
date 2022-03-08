#include "Castor3D/Scene/MovableObject.hpp"

#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"

using namespace castor;

namespace castor3d
{
	MovableObject::MovableObject( String const & name
		, Scene & scene
		, MovableType type
		, SceneNode & node )
		: castor::OwnedBy< Scene >{ scene }
		, Named( name )
		, m_type( type )
		, m_sceneNode( &node )
	{
	}
	
	MovableObject::MovableObject( String const & name
		, Scene & scene
		, MovableType type )
		: MovableObject
		{
			name,
			scene,
			type,
			( type == MovableType::eCamera
				? *scene.getCameraRootNode()
				: *scene.getObjectRootNode() ),
		}
	{
	}

	MovableObject::~MovableObject()
	{
		detach();
	}

	void MovableObject::detach()
	{
		auto node = getParent();

		if ( node )
		{
			m_notifyIndex.disconnect();
			m_sceneNode = nullptr;
			node->detachObject( *this );
		}
	}

	void MovableObject::attachTo( SceneNode & node )
	{
		m_sceneNode = &node;

		if ( m_sceneNode )
		{
			m_strNodeName = m_sceneNode->getName();
		}
		else
		{
			m_strNodeName.clear();
		}
	}

	EngineRPtr MovableObject::getEngine()const
	{
		return getScene()->getEngine();
	}
}
