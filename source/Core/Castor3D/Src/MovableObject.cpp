#include "MovableObject.hpp"
#include "SceneNode.hpp"
#include "Scene.hpp"
#include "Animation.hpp"

using namespace Castor;

namespace Castor3D
{
	MovableObject::TextLoader::TextLoader( File::eENCODING_MODE p_eEncodingMode )
		:	Loader< MovableObject, eFILE_TYPE_TEXT, TextFile >( File::eOPEN_MODE_DUMMY, p_eEncodingMode )
	{
	}

	bool MovableObject::TextLoader::operator()( MovableObject const & p_object, TextFile & p_file )
	{
		bool l_bReturn = p_file.WriteText( cuT( "\t\tparent \"" ) + p_object.GetParent()->GetName() + cuT( "\"\n" ) ) > 0;
		return l_bReturn;
	}

	//*************************************************************************************************

	MovableObject::BinaryParser::BinaryParser( Path const & p_path )
		:	Castor3D::BinaryParser< MovableObject >( p_path )
	{
	}

	bool MovableObject::BinaryParser::Fill( MovableObject const & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_bReturn = true;

		if ( l_bReturn )
		{
			l_bReturn = DoFillChunk( p_obj.GetName(), eCHUNK_TYPE_NAME, p_chunk );
		}

		if ( l_bReturn )
		{
			l_bReturn = DoFillChunk( p_obj.GetParent()->GetName(), eCHUNK_TYPE_MOVABLE_NODE, p_chunk );
		}

		return l_bReturn;
	}

	bool MovableObject::BinaryParser::Parse( MovableObject & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_bReturn = true;
		String l_name;

		switch ( p_chunk.GetChunkType() )
		{
		case eCHUNK_TYPE_MOVABLE_NODE:
			l_bReturn = DoParseChunk( l_name, p_chunk );

			if ( l_bReturn )
			{
				SceneNodeSPtr l_pParent = p_obj.GetScene()->GetNode( l_name );

				if ( l_pParent )
				{
					p_obj.AttachTo( l_pParent );
				}
				else
				{
					l_bReturn = false;
				}
			}

			break;
		}

		if ( !l_bReturn )
		{
			p_chunk.EndParse();
		}

		return l_bReturn;
	}

	//*************************************************************************************************

	MovableObject::MovableObject( SceneSPtr p_pScene, eMOVABLE_TYPE p_eType )
		:	m_eType( p_eType )
		,	m_pScene( p_pScene )
	{
	}

	MovableObject::MovableObject( SceneSPtr p_pScene, SceneNodeSPtr p_sn, String const & p_name, eMOVABLE_TYPE p_eType )
		:	m_strName( p_name )
		,	m_pSceneNode( p_sn )
		,	m_eType( p_eType )
		,	m_pScene( p_pScene )
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
