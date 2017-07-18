#include "MovableObject.hpp"

#include "Animation/Animation.hpp"
#include "Scene/Scene.hpp"

using namespace Castor;

namespace Castor3D
{
	MovableObject::TextWriter::TextWriter( String const & p_tabs )
		: Castor::TextWriter< MovableObject >{ p_tabs }
	{
	}

	bool MovableObject::TextWriter::operator()( MovableObject const & p_object, TextFile & p_file )
	{
		return p_file.WriteText( m_tabs + cuT( "parent \"" ) + p_object.GetParent()->GetName() + cuT( "\"\n" ) ) > 0;
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
		Detach();
		Animable::CleanupAnimations();
	}

	void MovableObject::Detach()
	{
		SceneNodeSPtr node = GetParent();

		if ( node )
		{
			m_notifyIndex.disconnect();
			node->DetachObject( *this );
			m_sceneNode.reset();
		}
	}

	void MovableObject::AttachTo( SceneNodeSPtr p_node )
	{
		m_sceneNode = p_node;

		if ( p_node )
		{
			m_strNodeName = p_node->GetName();
		}
		else
		{
			m_strNodeName.clear();
		}
	}
}
