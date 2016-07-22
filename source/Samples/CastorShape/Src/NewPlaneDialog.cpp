#include "NewPlaneDialog.hpp"

#include <wx/propgrid/advprops.h>

using namespace Castor3D;
using namespace Castor;

namespace CastorShape
{
	namespace
	{
		static wxString PLANE_WIDTH = _( "Width" );
		static wxString PLANE_DEPTH = _( "Depth" );
		static wxString PLANE_WIDTH_SUBDIVISIONS = _( "Width subdivisions" );
		static wxString PLANE_DEPTH_SUBDIVISIONS = _( "Depth subdivisions" );
	}

	NewPlaneDialog::NewPlaneDialog( Scene & p_scene, wxWindow * parent, wxWindowID p_id )
		: NewGeometryDialog( p_scene, parent, p_id, _( "New plane" ) )
	{
		PLANE_WIDTH = _( "Width" );
		PLANE_DEPTH = _( "Depth" );
		PLANE_WIDTH_SUBDIVISIONS = _( "Width subdivisions" );
		PLANE_DEPTH_SUBDIVISIONS = _( "Depth subdivisions" );
		DoInitialise();
	}

	NewPlaneDialog::~NewPlaneDialog()
	{
	}

	void NewPlaneDialog::DoCreateProperties()
	{
		m_properties->Append( new wxFloatProperty( PLANE_WIDTH ) )->SetValue( 1.0 );
		m_properties->Append( new wxFloatProperty( PLANE_DEPTH ) )->SetValue( 1.0 );
		m_properties->Append( new wxIntProperty( PLANE_WIDTH_SUBDIVISIONS ) )->SetValue( 10 );
		m_properties->Append( new wxIntProperty( PLANE_DEPTH_SUBDIVISIONS ) )->SetValue( 10 );
	}

	real NewPlaneDialog::GetWidth()const
	{
		double l_value = m_properties->GetPropertyValue( PLANE_WIDTH );
		return real( l_value );
	}

	real NewPlaneDialog::GetDepth()const
	{
		double l_value = m_properties->GetPropertyValue( PLANE_DEPTH );
		return real( l_value );
	}

	int NewPlaneDialog::GetWidthSubdivisionCount()const
	{
		long l_value = m_properties->GetPropertyValue( PLANE_WIDTH_SUBDIVISIONS );
		return int( l_value );
	}

	int NewPlaneDialog::GetDepthSubdivisionCount()const
	{
		long l_value = m_properties->GetPropertyValue( PLANE_DEPTH_SUBDIVISIONS );
		return int( l_value );
	}
}
