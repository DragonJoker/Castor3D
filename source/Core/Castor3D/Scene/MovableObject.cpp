#include "Castor3D/Scene/MovableObject.hpp"

#include "Castor3D/Animation/Animation.hpp"
#include "Castor3D/Scene/Scene.hpp"

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

	MovableObject::MovableObject( String const & p_name, Scene & p_scene, MovableType p_type, SceneNodeSPtr p_sn )
		: Animable{ p_scene }
		, Named( p_name )
		, m_type( p_type )
		, m_sceneNode( p_sn )
	{
	}

	MovableObject::~MovableObject()
	{
		detach();
		Animable::cleanupAnimations();
	}

	void MovableObject::detach()
	{
		SceneNodeSPtr node = getParent();

		if ( node )
		{
			m_notifyIndex.disconnect();
			node->detachObject( *this );
			m_sceneNode.reset();
		}
	}

	void MovableObject::attachTo( SceneNodeSPtr p_node )
	{
		m_sceneNode = p_node;

		if ( p_node )
		{
			m_strNodeName = p_node->getName();
		}
		else
		{
			m_strNodeName.clear();
		}
	}
}
