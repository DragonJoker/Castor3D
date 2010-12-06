#include "PrecompiledHeader.h"

#include "camera/Viewport.h"
#include "main/Pipeline.h"

using namespace Castor3D;

Viewport :: Viewport( int p_ww, int p_wh, Viewport::eTYPE p_type)
	:	m_type			( p_type),
		m_windowWidth	( p_ww),
		m_windowHeight	( p_wh),
		m_left			( -2),
		m_right			( 2),
		m_top			( 2),
		m_bottom		( -2),
		m_farView		( 2000),
		m_nearView		( -2000),
		m_fovY			( 45),
		m_ratio			( 1)
{
	if (m_type != e2DView)
	{
		m_nearView = real( 0.1); // not zero or we have a Z fight (?????)
	}

	m_ratio = 1.0f;
}

Viewport :: ~Viewport()
{
}

void Viewport :: Render( eDRAW_TYPE p_displayMode)
{
	Pipeline::ApplyViewport( m_windowWidth, m_windowHeight);
	Pipeline::MatrixMode( Pipeline::eProjection);
	Pipeline::LoadIdentity();

	if (m_type == Viewport::e3DView)
	{
		Pipeline::Perspective( m_fovY, real( m_windowWidth) / real( m_windowHeight), m_nearView, m_farView);
	}
	else if (m_type == Viewport::e2DView)
	{
		Pipeline::Ortho( m_left, m_right, m_bottom / m_ratio, m_top / m_ratio, m_nearView, m_farView);
	}

	Pipeline::MatrixMode( Pipeline::eModelView);
	Pipeline::LoadIdentity();
}

Point3r Viewport :: GetDirection( const Point<int, 2> & p_mouse)
{
	Point4r l_viewport( real( 0), real( 0), real( m_windowWidth), real( m_windowHeight));
	Point3r l_ptReturn;
	Pipeline::UnProject( Point3i( p_mouse[0], p_mouse[1], 1), l_viewport, l_ptReturn);
	return l_ptReturn;
}

bool Viewport :: Write( Castor::Utils::File & p_file)const
{
	bool l_bReturn = (p_file.Write( m_type) == sizeof( eTYPE));

	if (l_bReturn)
	{
		if (m_type == e2DView)
		{
			l_bReturn = p_file.Write( m_farView) == sizeof( real);

			if (l_bReturn)
			{
				l_bReturn = p_file.Write( m_nearView) == sizeof( real);
			}

			if (l_bReturn)
			{
				l_bReturn = p_file.Write( m_left) == sizeof( real);
			}

			if (l_bReturn)
			{
				l_bReturn = p_file.Write( m_right) == sizeof( real);
			}

			if (l_bReturn)
			{
				l_bReturn = p_file.Write( m_top) == sizeof( real);
			}

			if (l_bReturn)
			{
				l_bReturn = p_file.Write( m_bottom) == sizeof( real);
			}
		}
		else
		{
			l_bReturn = p_file.Write( m_farView) == sizeof( real);

			if (l_bReturn)
			{
				l_bReturn = p_file.Write( m_nearView) == sizeof( real);
			}

			if (l_bReturn)
			{
				l_bReturn = p_file.Write( m_fovY) == sizeof( real);
			}
		}
	}

	return l_bReturn;
}

bool Viewport :: Read( Castor::Utils::File & p_file)
{
	bool l_bReturn = (p_file.Read( m_type) == sizeof( eTYPE));

	if (l_bReturn)
	{
		if (m_type == e2DView)
		{
			l_bReturn = p_file.Read( m_farView) == sizeof( real);

			if (l_bReturn)
			{
				l_bReturn = p_file.Read( m_nearView) == sizeof( real);
			}

			if (l_bReturn)
			{
				l_bReturn = p_file.Read( m_left) == sizeof( real);
			}

			if (l_bReturn)
			{
				l_bReturn = p_file.Read( m_right) == sizeof( real);
			}

			if (l_bReturn)
			{
				l_bReturn = p_file.Read( m_top) == sizeof( real);
			}

			if (l_bReturn)
			{
				l_bReturn = p_file.Read( m_bottom) == sizeof( real);
			}
		}
		else
		{
			l_bReturn = p_file.Read( m_farView) == sizeof( real);

			if (l_bReturn)
			{
				l_bReturn = p_file.Read( m_nearView) == sizeof( real);
			}

			if (l_bReturn)
			{
				l_bReturn = p_file.Read( m_fovY) == sizeof( real);
			}
		}
	}

	return l_bReturn;
}