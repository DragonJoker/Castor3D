#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/MovableObject.hpp"
#include "Castor3D/SceneNode.hpp"
#include "Castor3D/Scene.hpp"
#include "Castor3D/RenderSystem.hpp"

using namespace Castor3D;

//*************************************************************************************************

bool Loader<MovableObject> :: Write( const MovableObject & p_object, File & p_file)
{
	bool l_bReturn = p_file.WriteLine( "\tparent " + p_object.GetParent()->GetName() + "\n") > 0;

	return l_bReturn;
}

//*************************************************************************************************

MovableObject :: MovableObject( eMOVABLE_TYPE p_eType)
	:	m_pSceneNode( NULL)
	,	m_eType( p_eType)
	,	m_pScene( NULL)
{
}

MovableObject :: MovableObject( Scene * p_pScene, SceneNode * p_sn, String const & p_name, eMOVABLE_TYPE p_eType)
	:	m_strName( p_name)
	,	m_pSceneNode( p_sn)
	,	m_eType( p_eType)
	,	m_pScene( p_pScene)
{
	if (m_pSceneNode)
	{
		m_pSceneNode->AttachObject( this);
	}
}

MovableObject :: ~MovableObject()
{
	Detach();
}

void MovableObject :: Cleanup()
{
	CASTOR_TRACK;
}

void MovableObject :: Detach()
{
	if (m_pSceneNode)
	{
		m_pSceneNode->DetachObject( this);
		m_pSceneNode = nullptr;
	}
}

void MovableObject :: AttachTo( SceneNode * p_node)
{
	m_pSceneNode = p_node;

	if (m_pSceneNode != NULL)
	{
		m_strNodeName = m_pSceneNode->GetName();
	}
	else
	{
		m_strNodeName = cuEmptyString;
	}
}

BEGIN_SERIALISE_MAP( MovableObject, Serialisable)
	ADD_ELEMENT( m_strName)
	ADD_ELEMENT( m_strNodeName)
END_SERIALISE_MAP()

BEGIN_POST_UNSERIALISE( MovableObject, Serialisable)
	m_pScene = static_cast<Scene *>( m_pParentSerialisable);
	m_pSceneNode = m_pScene->GetNode( m_strNodeName).get();
END_POST_UNSERIALISE()

//*************************************************************************************************