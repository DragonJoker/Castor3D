#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/camera/Viewport.h"
#include "Castor3D/main/Pipeline.h"

using namespace Castor3D;

Viewport :: Viewport( unsigned int p_uiWidth, unsigned int p_uiHeight, Viewport::eTYPE p_eType)
	:	m_eType				( p_eType),
		m_uiWindowWidth		( p_uiWidth),
		m_uiWindowHeight	( p_uiHeight),
		m_rLeft				( -2),
		m_rRight			( 2),
		m_rTop				( 2),
		m_rBottom			( -2),
		m_rFarView			( 2000),
		m_rNearView			( -2000),
		m_rFovY				( 45),
		m_rRatio			( 1)
{
	if (m_eType != e2DView)
	{
		m_rNearView = real( 0.1); // not zero or we have a Z fight (?????)
	}

	m_rRatio = 1.0f;
}

Viewport :: ~Viewport()
{
}

void Viewport :: Render( eDRAW_TYPE p_eDisplayMode)
{
	Pipeline::ApplyViewport( m_uiWindowWidth, m_uiWindowHeight);
	Pipeline::MatrixMode( Pipeline::eProjection);
	Pipeline::LoadIdentity();

	if (m_eType == Viewport::e3DView)
	{
		Pipeline::Perspective( m_rFovY, real( m_uiWindowWidth) / real( m_uiWindowHeight), m_rNearView, m_rFarView);
	}
	else if (m_eType == Viewport::e2DView)
	{
		Pipeline::Ortho( m_rLeft, m_rRight, m_rBottom / m_rRatio, m_rTop / m_rRatio, m_rNearView, m_rFarView);
	}

	Pipeline::MatrixMode( Pipeline::eModelView);
	Pipeline::LoadIdentity();
}

Point3r Viewport :: GetDirection( const Point2i & p_ptMouse)
{
	Point4r l_viewport( real( 0), real( 0), real( m_uiWindowWidth), real( m_uiWindowHeight));
	Point3r l_ptReturn;
	Pipeline::UnProject( Point3i( p_ptMouse[0], p_ptMouse[1], 1), l_viewport, l_ptReturn);
	return l_ptReturn;
}

bool Viewport :: Write( Castor::Utils::File & p_file)const
{
	bool l_bReturn = (p_file.Write( m_eType) == sizeof( eTYPE));

	if (l_bReturn)
	{
		if (m_eType == e2DView)
		{
			l_bReturn = p_file.Write( m_rFarView) == sizeof( real);

			if (l_bReturn)
			{
				l_bReturn = p_file.Write( m_rNearView) == sizeof( real);
			}

			if (l_bReturn)
			{
				l_bReturn = p_file.Write( m_rLeft) == sizeof( real);
			}

			if (l_bReturn)
			{
				l_bReturn = p_file.Write( m_rRight) == sizeof( real);
			}

			if (l_bReturn)
			{
				l_bReturn = p_file.Write( m_rTop) == sizeof( real);
			}

			if (l_bReturn)
			{
				l_bReturn = p_file.Write( m_rBottom) == sizeof( real);
			}
		}
		else
		{
			l_bReturn = p_file.Write( m_rFarView) == sizeof( real);

			if (l_bReturn)
			{
				l_bReturn = p_file.Write( m_rNearView) == sizeof( real);
			}

			if (l_bReturn)
			{
				l_bReturn = p_file.Write( m_rFovY) == sizeof( real);
			}
		}
	}

	return l_bReturn;
}

bool Viewport :: Read( Castor::Utils::File & p_file)
{
	bool l_bReturn = (p_file.Read( m_eType) == sizeof( eTYPE));

	if (l_bReturn)
	{
		if (m_eType == e2DView)
		{
			l_bReturn = p_file.Read( m_rFarView) == sizeof( real);

			if (l_bReturn)
			{
				l_bReturn = p_file.Read( m_rNearView) == sizeof( real);
			}

			if (l_bReturn)
			{
				l_bReturn = p_file.Read( m_rLeft) == sizeof( real);
			}

			if (l_bReturn)
			{
				l_bReturn = p_file.Read( m_rRight) == sizeof( real);
			}

			if (l_bReturn)
			{
				l_bReturn = p_file.Read( m_rTop) == sizeof( real);
			}

			if (l_bReturn)
			{
				l_bReturn = p_file.Read( m_rBottom) == sizeof( real);
			}
		}
		else
		{
			l_bReturn = p_file.Read( m_rFarView) == sizeof( real);

			if (l_bReturn)
			{
				l_bReturn = p_file.Read( m_rNearView) == sizeof( real);
			}

			if (l_bReturn)
			{
				l_bReturn = p_file.Read( m_rFovY) == sizeof( real);
			}
		}
	}

	return l_bReturn;
}