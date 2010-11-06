#include "PrecompiledHeader.h"

#include "GLSceneNodeRenderer.h"
#include "Module_GL.h"
#include "GLPipeline.h"

using namespace Castor3D;

void GLSceneNodeRenderer :: ApplyTransformations()
{
	Point3r l_position = m_target->GetPosition();
	Point3r l_scale = m_target->GetScale();
	Pipeline::PushMatrix();
	Pipeline::Translate( l_position);
	Pipeline::MultMatrix( m_target->Get4x4RotationMatrix());
	Pipeline::Scale( l_scale);
}

void GLSceneNodeRenderer :: RemoveTransformations()
{
	Pipeline::PopMatrix();
}