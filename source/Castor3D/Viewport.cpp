#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/Viewport.hpp"
#include "Castor3D/Pipeline.hpp"

using namespace Castor3D;

//*************************************************************************************************

bool Loader<Viewport> :: Write( const Viewport & p_viewport, File & p_file)
{
	bool l_bReturn = p_file.WriteLine( cuT( "\tviewport\n\t{\n")) > 0;

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteLine( cuT( "\t\ttype ") + Viewport::xstring_type[p_viewport.GetType()] + cuT( "\n")) > 0;
	}

	if (p_viewport.GetType() == eVIEWPORT_TYPE_2D)
	{
		if (l_bReturn)
		{
			l_bReturn = p_file.Print( 256, cuT( "values %f %f %f %f %f %f\n"), p_viewport.GetNearView(), p_viewport.GetFarView(), p_viewport.GetLeft(), p_viewport.GetRight(), p_viewport.GetTop(), p_viewport.GetBottom()) > 0;
		}
	}
	else
	{
		if (l_bReturn)
		{
			l_bReturn = p_file.Print( 256, cuT( "values %f %f %f\n"), p_viewport.GetNearView(), p_viewport.GetFarView(), p_viewport.GetFovY().Degrees()) > 0;
		}
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteLine( cuT( "\t}\n")) > 0;
	}

	return l_bReturn;
}

//*************************************************************************************************

const String Viewport::xstring_type[eVIEWPORT_TYPE_COUNT] = { cuT( "2d_view"), cuT( "3d_view") };

Viewport :: Viewport( unsigned int p_uiWidth, unsigned int p_uiHeight, eVIEWPORT_TYPE p_eType)
	:	m_eType				( p_eType),
		m_uiWindowWidth		( p_uiWidth),
		m_uiWindowHeight	( p_uiHeight),
		m_rLeft				( -2),
		m_rRight			( 2),
		m_rTop				( 2),
		m_rBottom			( -2),
		m_rFarView			( 2000),
		m_rNearView			( -2000),
		m_aFovY				( Angle::FromDegrees( 45.0)),
		m_rRatio			( 1)
{
	if (m_eType != eVIEWPORT_TYPE_2D)
	{
		m_rNearView = real( 0.001); // not zero or we have a Z fight (?????)
	}

	m_rRatio = 1.0f;
}

Viewport :: ~Viewport()
{
}

void Viewport :: Render( ePRIMITIVE_TYPE p_eDisplayMode)
{
	CASTOR_TRACK;
	Pipeline::ApplyViewport( m_uiWindowWidth, m_uiWindowHeight);
	Pipeline::MatrixMode( Pipeline::eMODE_PROJECTION);
	Pipeline::LoadIdentity();

	if (m_eType == eVIEWPORT_TYPE_3D)
	{
		Pipeline::Perspective( m_aFovY, real( m_uiWindowWidth) / real( m_uiWindowHeight), m_rNearView, m_rFarView);
	}
	else if (m_eType == eVIEWPORT_TYPE_2D)
	{
		Pipeline::Ortho( m_rLeft, m_rRight, m_rBottom / m_rRatio, m_rTop / m_rRatio, m_rNearView, m_rFarView);
	}

	Pipeline::MatrixMode( Pipeline::eMODE_MODELVIEW);
	Pipeline::LoadIdentity();
}

void Viewport :: GetDirection( Point2i const & p_ptMouse, Point3r & p_ptResult)
{
	Point4r l_viewport( real( 0), real( 0), real( m_uiWindowWidth), real( m_uiWindowHeight));
	Pipeline::UnProject( Point3i( p_ptMouse[0], p_ptMouse[1], 1), l_viewport, p_ptResult);
}

BEGIN_SERIALISE_MAP( Viewport, Serialisable)
	ADD_ELEMENT( m_eType)
	ADD_ELEMENT( m_rLeft)
	ADD_ELEMENT( m_rRight)
	ADD_ELEMENT( m_rTop)
	ADD_ELEMENT( m_rBottom)
	ADD_ELEMENT( m_rNearView)
	ADD_ELEMENT( m_rFarView)
	ADD_ELEMENT( m_aFovY)
END_SERIALISE_MAP()

//*************************************************************************************************