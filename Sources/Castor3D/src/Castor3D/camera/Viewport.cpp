
#include "PrecompiledHeader.h"

#include "camera/Module_Camera.h"


#include "scene/Module_Scene.h"
#include "geometry/Module_Geometry.h"
#include "material/Module_Material.h"
#include "render_system/Module_Render.h"
#include "main/Module_Main.h"
#include "shader/Module_Shader.h"

#include "camera/Viewport.h"
#include "render_system/CameraRenderer.h"


using namespace Castor3D;

Viewport :: Viewport( ViewportRenderer * p_renderer, int p_ww, int p_wh,
					  ProjectionType p_type)
	:	m_type( p_type),
		m_windowWidth( p_ww),
		m_windowHeight( p_wh),
		m_renderer( p_renderer)
{
	m_renderer->SetTarget( this);
	if (m_type == pt2DView)
	{
		m_farView = 2000.0f;
		m_nearView = -2000.0f;
		m_left = -2.0f;
		m_right = 2.0f;
		m_top = 2.0f;
		m_bottom = -2.0f;
	}
	else
	{
		m_nearView = 0.1f; // not zero or we have a Z fight (?????)
		m_farView = 20000.0f;
		m_fovY = 45.0f;
	}
	m_ratio = 1.0f;
}

Viewport :: ~Viewport()
{
}

void Viewport :: Apply()
{
	m_renderer->Apply( m_type);
}

Vector3f Viewport :: GetDirection( const Point2D<int> & p_mouse)
{
	return m_renderer->GetDirection( p_mouse);
}

bool Viewport :: Write( General::Utils::File & p_file)const
{
	if ( ! p_file.Write<int>( m_type))
	{
		return false;
	}
	if (m_type == pt2DView)
	{
		if ( ! p_file.Write<float>( m_farView))
		{
			return false;
		}
		if ( ! p_file.Write<float>( m_nearView))
		{
			return false;
		}
		if ( ! p_file.Write<float>( m_left))
		{
			return false;
		}
		if ( ! p_file.Write<float>( m_right))
		{
			return false;
		}
		if ( ! p_file.Write<float>( m_top))
		{
			return false;
		}
		if ( ! p_file.Write<float>( m_bottom))
		{
			return false;
		}
	}
	else
	{
		if ( ! p_file.Write<float>( m_farView))
		{
			return false;
		}
		if ( ! p_file.Write<float>( m_nearView))
		{
			return false;
		}
		if ( ! p_file.Write<float>( m_fovY))
		{
			return false;
		}
	}
	return true;
}

bool Viewport :: Read( General::Utils::File & p_file)
{
	if ( ! p_file.Read<ProjectionType>( m_type))
	{
		return false;
	}
	if (m_type == pt2DView)
	{
		if ( ! p_file.Read<float>( m_farView))
		{
			return false;
		}
		if ( ! p_file.Read<float>( m_nearView))
		{
			return false;
		}
		if ( ! p_file.Read<float>( m_left))
		{
			return false;
		}
		if ( ! p_file.Read<float>( m_right))
		{
			return false;
		}
		if ( ! p_file.Read<float>( m_top))
		{
			return false;
		}
		if ( ! p_file.Read<float>( m_bottom))
		{
			return false;
		}
	}
	else
	{
		if ( ! p_file.Read<float>( m_farView))
		{
			return false;
		}
		if ( ! p_file.Read<float>( m_nearView))
		{
			return false;
		}
		if ( ! p_file.Read<float>( m_fovY))
		{
			return false;
		}
	}
	return true;
}