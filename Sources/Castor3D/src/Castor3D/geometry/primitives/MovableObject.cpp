#include "PrecompiledHeader.h"

#include "geometry/Module_Geometry.h"

#include "geometry/primitives/MovableObject.h"
#include "scene/SceneNode.h"
#include "scene/Scene.h"

#include "Log.h"

using namespace Castor3D;

MovableObject :: MovableObject( SceneNode * p_sn, const String & p_name)
	:	m_name( p_name),
		m_matrix( new float[16]),
		m_sceneNode( p_sn)
{
	if (p_sn != NULL)
	{
		p_sn->AttachGeometry( this);
	}
}

MovableObject :: ~MovableObject()
{
	delete [] m_matrix;
}

void MovableObject :: Cleanup()
{
}

bool MovableObject :: Write( FileIO * p_pFile)const
{
	bool l_bReturn = p_pFile->WriteLine( "\tparent " + m_sceneNode->GetName() + "\n");

	return true;
}

float * MovableObject :: Get4x4RotationMatrix()
{
	m_orientation.ToRotationMatrix( m_matrix);
	return m_matrix;
}

void MovableObject :: Detach()
{
	if (m_sceneNode != NULL)
	{
		m_sceneNode->DetachGeometry( this);
		m_sceneNode = NULL;
	}
}