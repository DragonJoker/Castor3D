#include "Castor3D/Scene/MovableObject.hpp"

#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"

CU_ImplementSmartPtr( castor3d, MovableObject )

namespace castor3d
{
	MovableObject::MovableObject( castor::String const & name
		, Scene & scene
		, MovableType type
		, SceneNode & node )
		: castor::OwnedBy< Scene >{ scene }
		, castor::Named( name )
		, m_type{ type }
		, m_sceneNode{ nullptr }
	{
		node.attachObject( *this );
	}
	
	MovableObject::MovableObject( castor::String const & name
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
		markDirty();
	}

	MovableObject::~MovableObject()
	{
		auto node = getParent();

		if ( node )
		{
			m_notifyIndex.disconnect();
			m_sceneNode = nullptr;
			node->detachObject( *this );
		}
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
		if ( m_sceneNode != &node )
		{
			m_sceneNode = &node;
			markDirty();
		}
	}

	void MovableObject::markDirty()
	{
		getScene()->markDirty( *this );
	}

	EngineRPtr MovableObject::getEngine()const
	{
		return getScene()->getEngine();
	}
}
