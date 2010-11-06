#include "PrecompiledHeader.h"

#include "camera/Viewport.h"

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
	if (m_type != pt2DView)
	{
		m_nearView = real( 0.1); // not zero or we have a Z fight (?????)
	}

	m_ratio = 1.0f;
}

Viewport :: ~Viewport()
{
}

void Viewport :: Apply( eDRAW_TYPE p_displayMode)
{
	m_pRenderer->Apply( m_type);
}

Point3r Viewport :: GetDirection( const Point<int, 2> & p_mouse)
{
	return m_pRenderer->GetDirection( p_mouse);
}

bool Viewport :: Write( Castor::Utils::File & p_file)const
{
	bool l_bReturn = (p_file.Write( m_type) == sizeof( eTYPE));

	if (l_bReturn)
	{
		if (m_type == pt2DView)
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
		if (m_type == pt2DView)
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