#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/camera/Camera.h"

#include "Castor3D/scene/Scene.h"
#include "Castor3D/scene/SceneNode.h"
#include "Castor3D/main/Root.h"
#include "Castor3D/main/Pipeline.h"
#include "Castor3D/render_system/RenderSystem.h"

using namespace Castor3D;

Camera :: Camera( Scene * p_pScene, const String & p_strName, unsigned int p_uiWidth, unsigned int p_uiHeight, SceneNodePtr p_pNode, Viewport::eTYPE p_eType)
	:	MovableObject( p_pScene, (SceneNode *)p_pNode.get(), p_strName, eCamera),
		m_viewport( p_uiWidth, p_uiHeight, p_eType),
		m_uiWindowWidth( p_uiWidth),
		m_uiWindowHeight( p_uiHeight)
{
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

void Camera :: Render( ePRIMITIVE_TYPE p_eDisplayMode)
{
	m_viewport.Render( p_eDisplayMode);
	Pipeline::PushMatrix();
	Pipeline::MultMatrix( m_pSceneNode->GetRotationMatrix());
	Pipeline::Translate( m_pSceneNode->GetPosition());
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

bool Camera :: Select( ScenePtr p_pScene, eSELECTION_MODE p_eMode, void ** p_ppFound, int p_iX, int p_iY)
{
	return m_pRenderer->Select( p_pScene, p_eMode, p_ppFound, p_iX, p_iY);
}

bool Camera :: Write( File & p_file)const
{
	bool l_bReturn = p_file.WriteLine( CU_T( "camera ") + m_strName + CU_T( "\n{\n")) > 0;

	if (l_bReturn)
	{
		l_bReturn = MovableObject::Write( p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = m_viewport.Write( p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteLine( CU_T( "}\n")) > 0;
	}

	return l_bReturn;
}

bool Camera :: Save( File & p_file)const
{
	bool l_bReturn = p_file.Write( m_strName);

	if (l_bReturn)
	{
		MovableObject::Save( p_file);
	}

	if (l_bReturn)
	{
		m_viewport.Save( p_file);
	}

	return l_bReturn;
}

bool Camera :: Load( File & p_file)
{
	bool l_bReturn = p_file.Read( m_strName);

	if (l_bReturn)
	{
		MovableObject::Load( p_file);
	}

	if (l_bReturn)
	{
		m_viewport.Load( p_file);
	}

	return l_bReturn;
}