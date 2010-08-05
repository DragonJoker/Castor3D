#include "PrecompiledHeader.h"

#include "camera/Module_Camera.h"

#include "scene/Module_Scene.h"
#include "geometry/Module_Geometry.h"
#include "material/Module_Material.h"
#include "render_system/Module_Render.h"
#include "main/Module_Main.h"
#include "shader/Module_Shader.h"

#include "camera/Camera.h"
#include "camera/Viewport.h"
#include "scene/SceneNode.h"
#include "geometry/Module_Geometry.h"
#include "main/Root.h"
#include "render_system/RenderSystem.h"
#include "render_system/CameraRenderer.h"

using namespace Castor3D;

Camera :: Camera( CameraRenderer * p_renderer, const String & p_name,
				  int p_ww, int p_wh, ProjectionType p_type)
	:	m_name( p_name),
		m_matrix( new float[16]),
		m_renderer( p_renderer)
{
	m_renderer->SetTarget( this);
	m_viewport = new Viewport( Root::GetRenderSystem()->CreateViewportRenderer(),
							   p_ww, p_wh, p_type);
}

Camera :: ~Camera()
{
	delete [] m_matrix;
	delete m_viewport;
}

void Camera :: Yaw( float p_angle)
{
	Quaternion l_tmp( Vector3f( 0.0, 1.0, 0.0), p_angle);
	m_orientation *= l_tmp;
}

void Camera :: Pitch( float p_angle)
{
	Quaternion l_tmp( Vector3f( 1.0, 0.0, 0.0), p_angle);
	m_orientation *= l_tmp;
}

void Camera :: Roll( float p_angle)
{
	Quaternion l_tmp( Vector3f( 0.0, 0.0, 1.0), p_angle);
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
	m_position = Vector3f( 0, 0, 0);
}

void Camera :: Translate( const Vector3f & p_t)
{
	m_position += p_t;
}

void Camera :: Translate( float x, float y, float z)
{
	m_position.x += x;
	m_position.y += y;
	m_position.z += z;
}

float * Camera :: GetRotationMatrix()
{
	m_orientation.ToRotationMatrix( m_matrix);
	return m_matrix;
}

void Camera :: Apply()
{
	m_viewport->Apply();
	m_orientation.ToRotationMatrix( m_matrix);
	m_renderer->ApplyTransformations( m_position, m_matrix);
}

void Camera :: Remove()
{
	m_renderer->RemoveTransformations();
}

void Camera :: Resize( unsigned int p_width, unsigned int p_height)
{
	m_windowWidth = p_width;
	m_windowHeight = p_height;
	m_viewport->SetWindowWidth( p_width);
	m_viewport->SetWindowHeight( p_height);
}

bool Camera :: Write( File & p_file)const
{
	size_t l_length = m_name.size();
	if (p_file.Write<size_t>( l_length) != sizeof( size_t))
	{
		return false;
	}
	if (p_file.WriteArray<Char>( m_name.c_str(), l_length) != l_length)
	{
		return false;
	}
	if (! WriteVertex( & m_position, p_file))
	{
		return false;
	}
	if (! m_orientation.Write( p_file))
	{
		return false;
	}
	if ( ! m_viewport->Write( p_file))
	{
		return false;
	}
	return true;
}

bool Camera :: Read( File & p_file)
{
	size_t l_nameLength = 0;
	if (p_file.Read<size_t>( l_nameLength) != sizeof( size_t))
	{
		return false;
	}
	Char * l_name = new Char[l_nameLength+1];
	if (p_file.ReadArray<Char>( l_name, l_nameLength) != l_nameLength)
	{
		delete [] l_name;
		return false;
	}
	l_name[l_nameLength] = 0;
	m_name = l_name;
	delete [] l_name;
	if (! ReadVertex( & m_position, p_file))
	{
		return false;
	}
	if (! m_orientation.Read( p_file))
	{
		return false;
	}
	if ( ! m_viewport->Read( p_file))
	{
		return false;
	}
	return true;
}

bool Camera :: Select( Scene * p_scene, SelectionMode p_mode, void ** p_found, int x, int y)
{
	return m_renderer->Select( p_scene, p_mode, p_found, x, y);
}