#include "Castor3D/Scene/MovableObject.hpp"

#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"

using namespace castor;

namespace castor3d
{
	MovableObject::TextWriter::TextWriter( String const & p_tabs )
		: castor::TextWriter< MovableObject >{ p_tabs }
	{
	}

	bool MovableObject::TextWriter::operator()( MovableObject const & p_object, TextFile & p_file )
	{
		return p_file.writeText( m_tabs + cuT( "parent \"" ) + p_object.getParent()->getName() + cuT( "\"\n" ) ) > 0;
	}

	//*************************************************************************************************

	MovableObject::MovableObject( String const & name
		, Scene & scene
		, MovableType type
		, SceneNode & node )
		: Animable{ *scene.getEngine() }
		, Named( name )
		, m_scene{ &scene }
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
		Animable::cleanupAnimations();
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
}
