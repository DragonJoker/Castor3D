#include "MovableObject.hpp"
#include "SceneNode.hpp"
#include "Scene.hpp"
#include "Animation.hpp"

using namespace Castor;

namespace Castor3D
{
	MovableObject::TextLoader::TextLoader( File::eENCODING_MODE p_encodingMode )
		: Loader< MovableObject, eFILE_TYPE_TEXT, TextFile >( File::eOPEN_MODE_DUMMY, p_encodingMode )
	{
	}

	bool MovableObject::TextLoader::operator()( MovableObject const & p_object, TextFile & p_file )
	{
		bool l_return = p_file.WriteText( cuT( "\t\tparent \"" ) + p_object.GetParent()->GetName() + cuT( "\"\n" ) ) > 0;
		return l_return;
	}

	//*************************************************************************************************

	MovableObject::BinaryParser::BinaryParser( Path const & p_path )
		: Castor3D::BinaryParser< MovableObject >( p_path )
	{
	}

	bool MovableObject::BinaryParser::Fill( MovableObject const & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_return = true;

		if ( l_return )
		{
			l_return = DoFillChunk( p_obj.GetName(), eCHUNK_TYPE_NAME, p_chunk );
		}

		if ( l_return )
		{
			l_return = DoFillChunk( p_obj.GetParent()->GetName(), eCHUNK_TYPE_MOVABLE_NODE, p_chunk );
		}

		return l_return;
	}

	bool MovableObject::BinaryParser::Parse( MovableObject & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_return = true;
		String l_name;

		switch ( p_chunk.GetChunkType() )
		{
		case eCHUNK_TYPE_MOVABLE_NODE:
			l_return = DoParseChunk( l_name, p_chunk );

			if ( l_return )
			{
				SceneNodeSPtr l_pParent = p_obj.GetScene()->GetNode( l_name );

				if ( l_pParent )
				{
					p_obj.AttachTo( l_pParent );
				}
				else
				{
					l_return = false;
				}
			}

			break;
		}

		if ( !l_return )
		{
			p_chunk.EndParse();
		}

		return l_return;
	}

	//*************************************************************************************************

	MovableObject::MovableObject( SceneSPtr p_scene, eMOVABLE_TYPE p_type )
		: Animable( *p_scene->GetOwner() )
		, m_type( p_type )
		, m_pScene( p_scene )
	{
	}

	MovableObject::MovableObject( SceneSPtr p_scene, SceneNodeSPtr p_sn, String const & p_name, eMOVABLE_TYPE p_type )
		: Animable( *p_scene->GetOwner() )
		, m_name( p_name )
		, m_pSceneNode( p_sn )
		, m_type( p_type )
		, m_pScene( p_scene )
	{
	}

	MovableObject::~MovableObject()
	{
		Detach();
	}

	void MovableObject::Cleanup()
	{
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
