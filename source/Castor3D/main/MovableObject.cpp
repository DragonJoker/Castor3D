#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/main/MovableObject.h"
#include "Castor3D/scene/SceneNode.h"
#include "Castor3D/scene/Scene.h"

using namespace Castor3D;

MovableObject :: MovableObject( SceneNode * p_sn, const String & p_name)
	:	m_strName( p_name),
		m_pSceneNode( p_sn)
{
	if (m_pSceneNode != NULL)
	{
		m_pSceneNode->AttachObject( this);
	}
}

MovableObject :: ~MovableObject()
{
}

void MovableObject :: Cleanup()
{
}

bool MovableObject :: Write( File & p_pFile)const
{
	bool l_bReturn = p_pFile.WriteLine( "\tparent " + m_pSceneNode->GetName() + "\n");

	return true;
}

void MovableObject :: Detach()
{
	if (m_pSceneNode != NULL)
	{
		m_pSceneNode->DetachObject( this);
		m_pSceneNode = NULL;
	}
}