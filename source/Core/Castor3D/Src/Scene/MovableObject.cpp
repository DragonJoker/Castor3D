#include "MovableObject.hpp"

#include "SceneNodeManager.hpp"

#include "Animation/Animation.hpp"
#include "Scene/Scene.hpp"

using namespace Castor;

namespace Castor3D
{
	MovableObject::TextLoader::TextLoader( String const & p_tabs )
		: Castor::TextLoader< MovableObject >{ p_tabs }
	{
	}

	bool MovableObject::TextLoader::operator()( MovableObject const & p_object, TextFile & p_file )
	{
		return p_file.WriteText( m_tabs + cuT( "\tparent \"" ) + p_object.GetParent()->GetName() + cuT( "\"\n" ) ) > 0;
	}

	//*************************************************************************************************

	MovableObject::MovableObject( String const & p_name, Scene & p_scene, eMOVABLE_TYPE p_type, SceneNodeSPtr p_sn )
		: OwnedBy< Scene >( p_scene )
		, m_name( p_name )
		, m_type( p_type )
		, m_pSceneNode( p_sn )
	{
	}

	MovableObject::~MovableObject()
	{
		Detach();
		Animable::CleanupAnimations();
	}

	void MovableObject::Detach()
	{
		SceneNodeSPtr l_node = GetParent();

		if ( l_node )
		{
			if ( m_notifyIndex )
			{
				l_node->UnregisterObject( m_notifyIndex );
				m_notifyIndex = 0;
			}

			l_node->DetachObject( shared_from_this() );
			m_pSceneNode.reset();
		}
	}

	void MovableObject::AttachTo( SceneNodeSPtr p_node )
	{
		m_pSceneNode = p_node;

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
