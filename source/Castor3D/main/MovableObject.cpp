#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/main/MovableObject.h"
#include "Castor3D/scene/SceneNode.h"
#include "Castor3D/scene/Scene.h"
#include "Castor3D/scene/SceneManager.h"
#include "Castor3D/render_system/RenderSystem.h"

using namespace Castor3D;

MovableObject :: MovableObject( Scene * p_pScene, SceneNode * p_sn, const String & p_name, eTYPE p_eType)
	:	m_strName( p_name)
	,	m_pSceneNode( p_sn)
	,	m_eType( p_eType)
	,	m_pScene( p_pScene)
{
	if (m_pSceneNode != NULL)
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
}

void MovableObject :: Detach()
{
	if (m_pSceneNode != NULL)
	{
		m_pSceneNode->DetachObject( this);
		m_pSceneNode = NULL;
	}
}

bool MovableObject :: Write( File & p_file)const
{
	bool l_bReturn = p_file.WriteLine( "\tparent " + m_pSceneNode->GetName() + "\n") > 0;

	return l_bReturn;
}

bool MovableObject :: Save( File & p_file)const
{
	bool l_bReturn = p_file.Write( m_pSceneNode->GetName());

	return l_bReturn;
}

bool MovableObject :: Load( File & p_file)
{
	String l_strName;
	bool l_bReturn = p_file.Read( l_strName);

	if (l_bReturn)
	{
		m_pSceneNode = m_pScene->GetNode( l_strName).get();
		l_bReturn = m_pSceneNode != NULL;
	}

	return l_bReturn;
}