#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/camera/Camera.h"

#include "Castor3D/scene/Scene.h"
#include "Castor3D/scene/SceneNode.h"
#include "Castor3D/main/Root.h"
#include "Castor3D/main/Pipeline.h"
#include "Castor3D/render_system/RenderSystem.h"

using namespace Castor3D;

Camera :: Camera( const String & p_strName, unsigned int p_uiWidth, unsigned int p_uiHeight, SceneNodePtr p_pNode, Viewport::eTYPE p_eType)
	:	MovableObject( (SceneNode *)p_pNode.get(), p_strName),
		m_viewport( p_uiWidth, p_uiHeight, p_eType),
		m_uiWindowWidth( p_uiWidth),
		m_uiWindowHeight( p_uiHeight)
{
	m_eType = eCamera;
}

Camera :: ~Camera()
{
}

void Camera :: ResetOrientation()
{
	m_pSceneNode->SetOrientation( Quaternion::Identity);
}

void Camera :: ResetPosition()
{
	m_pSceneNode->SetPosition( Point3r( 0, 0, 0));
}

void Camera :: Render( eDRAW_TYPE p_eDisplayMode)
{
	m_viewport.Render( p_eDisplayMode);
	Pipeline::PushMatrix();
	Pipeline::Translate( m_pSceneNode->GetPosition());
	Pipeline::MultMatrix( m_pSceneNode->GetRotationMatrix());
}

void Camera :: EndRender()
{
	Pipeline::PopMatrix();
}

void Camera :: Resize( unsigned int p_uiWidth, unsigned int p_uiHeight)
{
	m_uiWindowWidth = p_uiWidth;
	m_uiWindowHeight = p_uiHeight;
	m_viewport.SetWindowWidth( p_uiWidth);
	m_viewport.SetWindowHeight( p_uiHeight);
}

bool Camera :: Write( File & p_file)const
{
	return true;
}

bool Camera :: Select( ScenePtr p_pScene, eSELECTION_MODE p_eMode, void ** p_ppFound, int p_iX, int p_iY)
{
	return m_pRenderer->Select( p_pScene, p_eMode, p_ppFound, p_iX, p_iY);
}