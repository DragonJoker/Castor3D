#include "PrecompiledHeader.h"

#include "camera/Camera.h"

#include "scene/Scene.h"
#include "main/Root.h"
#include "render_system/RenderSystem.h"

using namespace Castor3D;

Camera :: Camera( const String & p_name,
				 int p_ww, int p_wh, Viewport::eTYPE p_type)
	:	m_name( p_name),
		m_matrix( new real[16])
{
	m_viewport = new Viewport( p_ww, p_wh, p_type);
}

Camera :: ~Camera()
{
	delete [] m_matrix;
//	delete m_viewport;
}

void Camera :: Yaw( const Angle & p_angle)
{
	Quaternion l_tmp( Point3r( 0.0, 1.0, 0.0), p_angle);
	m_orientation *= l_tmp;
}

void Camera :: Pitch( const Angle & p_angle)
{
	Quaternion l_tmp( Point3r( 1.0, 0.0, 0.0), p_angle);
	m_orientation *= l_tmp;
}

void Camera :: Roll( const Angle & p_angle)
{
	Quaternion l_tmp( Point3r( 0.0, 0.0, 1.0), p_angle);
	m_orientation *= l_tmp;
}

void Camera :: Rotate( const Quaternion & p_quat)
{
	m_orientation *= p_quat;
}

void Camera :: ResetOrientation()
{
	Quaternion l_tmp;
	m_orientation = l_tmp;
}

void Camera :: ResetPosition()
{
	m_position = Point3r( 0, 0, 0);
}

void Camera :: Translate( const Point3r & p_t)
{
	m_position += p_t;
}

void Camera :: Translate( real x, real y, real z)
{
	m_position += Point3r( x, y, z);
}

real * Camera :: GetRotationMatrix()
{
	m_orientation.ToRotationMatrix( m_matrix);
	return m_matrix;
}

void Camera :: Apply( eDRAW_TYPE p_displayMode)
{
	m_viewport->Apply( p_displayMode);
	m_orientation.ToRotationMatrix( m_matrix);
	m_pRenderer->ApplyTransformations( m_position, m_matrix);
}

void Camera :: Remove()
{
	m_pRenderer->RemoveTransformations();
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