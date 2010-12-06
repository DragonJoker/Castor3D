#include "PrecompiledHeader.h"

#include "camera/Camera.h"

#include "scene/Scene.h"
#include "scene/NodeBase.h"
#include "main/Root.h"
#include "main/Pipeline.h"
#include "render_system/RenderSystem.h"

using namespace Castor3D;

Camera :: Camera( const String & p_name, int p_ww, int p_wh, CameraNodePtr p_pNode, Viewport::eTYPE p_type)
	:	MovableObject( (NodeBase *)p_pNode.get(), p_name),
		m_viewport( new Viewport( p_ww, p_wh, p_type))
{
}

Camera :: ~Camera()
{
}

void Camera :: ResetOrientation()
{
	m_pSceneNode->SetOrientation( Quaternion::Quat_Identity);
}

void Camera :: ResetPosition()
{
	m_pSceneNode->SetPosition( Point3r( 0, 0, 0));
}

void Camera :: Render( eDRAW_TYPE p_displayMode)
{
	m_viewport->Render( p_displayMode);
	Pipeline::PushMatrix();
	Pipeline::Translate( m_pSceneNode->GetPosition());
	Pipeline::MultMatrix( m_pSceneNode->GetRotationMatrix());
//	m_pRenderer->Render( p_displayMode);
}

void Camera :: EndRender()
{
	Pipeline::PopMatrix();
}

void Camera :: Resize( unsigned int p_width, unsigned int p_height)
{
	m_windowWidth = p_width;
	m_windowHeight = p_height;
	m_viewport->SetWindowWidth( p_width);
	m_viewport->SetWindowHeight( p_height);
}

bool Camera :: Write( File & p_pFile)const
{
	return true;
}

bool Camera :: Select( ScenePtr p_scene, eSELECTION_MODE p_mode, void ** p_found, int x, int y)
{
	return m_pRenderer->Select( p_scene, p_mode, p_found, x, y);
}