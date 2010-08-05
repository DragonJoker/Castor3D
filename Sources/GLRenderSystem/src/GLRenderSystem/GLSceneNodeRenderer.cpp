//******************************************************************************
#include "PrecompiledHeader.h"

#include "../../include/GLRenderSystem/GLSceneNodeRenderer.h"
#include "../../include/GLRenderSystem/Module_GL.h"
//******************************************************************************
using namespace Castor3D;
//******************************************************************************

void GLSceneNodeRenderer :: ApplyTransformations()
{
	Vector3f l_position = m_target->GetPosition();
	Vector3f l_scale = m_target->GetScale();
	glPushMatrix();
	glTranslatef( l_position.x, l_position.y, l_position.z);
	glMultMatrixf( m_target->Get4x4RotationMatrix());
	glScalef( l_scale.x, l_scale.y, l_scale.z);
}

//******************************************************************************

void GLSceneNodeRenderer :: RemoveTransformations()
{
	glPopMatrix();
}

//******************************************************************************
