#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/Camera.hpp"

#include "Castor3D/Scene.hpp"
#include "Castor3D/SceneNode.hpp"
#include "Castor3D/Root.hpp"
#include "Castor3D/Pipeline.hpp"
#include "Castor3D/RenderSystem.hpp"

using namespace Castor3D;

//*************************************************************************************************

bool Loader<Camera> :: Write( const Camera & p_camera, File & p_file)
{
	bool l_bReturn = p_file.WriteLine( cuT( "camera ") + p_camera.GetName() + cuT( "\n{\n")) > 0;

	if (l_bReturn)
	{
		l_bReturn = Loader<MovableObject>::Write( p_camera, p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = Loader<Viewport>::Write( * p_camera.GetViewport(), p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteLine( cuT( "}\n")) > 0;
	}

	return l_bReturn;
}

//*************************************************************************************************

Camera :: Camera( Scene * p_pScene, String const & p_strName, unsigned int p_uiWidth, unsigned int p_uiHeight, SceneNodePtr p_pNode, eVIEWPORT_TYPE p_eType)
	:	MovableObject( p_pScene, p_pNode.get(), p_strName, eMOVABLE_TYPE_CAMERA),
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
	CASTOR_TRACK;
	m_viewport.Render( p_eDisplayMode);
	Pipeline::PushMatrix();
	Pipeline::MultMatrix( m_pSceneNode->GetRotationMatrix());
	Pipeline::Translate( m_pSceneNode->GetPosition());
//	std::cout << m_pSceneNode->GetRotationMatrix() << std::endl;
}

void Camera :: EndRender()
{
	CASTOR_TRACK;
	Pipeline::PopMatrix();
}

void Camera :: Resize( unsigned int p_uiWidth, unsigned int p_uiHeight)
{
	CASTOR_TRACK;
	m_uiWindowWidth = p_uiWidth;
	m_uiWindowHeight = p_uiHeight;
	m_viewport.SetWindowWidth( p_uiWidth);
	m_viewport.SetWindowHeight( p_uiHeight);
	m_pRenderer->Resize( Point2i( p_uiWidth, p_uiHeight));
}

bool Camera :: Select( ScenePtr p_pScene, eSELECTION_MODE p_eMode, void ** p_ppFound, int p_iX, int p_iY)
{
	return m_pRenderer->Select( p_pScene, p_eMode, p_ppFound, p_iX, p_iY);
}

BEGIN_SERIALISE_MAP( Camera, MovableObject)
	ADD_ELEMENT( m_viewport)
END_SERIALISE_MAP()

//*************************************************************************************************