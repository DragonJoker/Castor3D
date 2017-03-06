#include "NewTorusDialog.hpp"

#include <wx/propgrid/advprops.h>

using namespace Castor3D;
using namespace Castor;

namespace CastorShape
{
	namespace
	{
		static wxString TORUS_INTERNAL_RADIUS = _( "Internal radius" );
		static wxString TORUS_EXTERNAL_RADIUS = _( "External radius" );
		static wxString TORUS_INTERNAL_WIDTH_SUBDIVISIONS = _( "Internal subdivisions" );
		static wxString TORUS_EXTERNAL_DEPTH_SUBDIVISIONS = _( "External subdivisions" );
	}

	NewTorusDialog::NewTorusDialog(Scene & p_scene, wxWindow * parent, wxWindowID p_id )
		: NewGeometryDialog( p_scene, parent, p_id, cuT( "New torus" ) )
	{
		TORUS_INTERNAL_RADIUS = _( "Internal radius" );
		TORUS_EXTERNAL_RADIUS = _( "External radius" );
		TORUS_INTERNAL_WIDTH_SUBDIVISIONS = _( "Internal subdivisions" );
		TORUS_EXTERNAL_DEPTH_SUBDIVISIONS = _( "External subdivisions" );
		DoInitialise();
	}

	NewTorusDialog::~NewTorusDialog()
	{
	}

	void NewTorusDialog::DoCreateProperties()
	{
		m_properties->Append( new wxFloatProperty( TORUS_INTERNAL_RADIUS ) )->SetValue( 1.0 );
		m_properties->Append( new wxFloatProperty( TORUS_EXTERNAL_RADIUS ) )->SetValue( 0.5 );
		m_properties->Append( new wxIntProperty( TORUS_INTERNAL_WIDTH_SUBDIVISIONS ) )->SetValue( 20 );
		m_properties->Append( new wxIntProperty( TORUS_EXTERNAL_DEPTH_SUBDIVISIONS ) )->SetValue( 20 );
	}

	real NewTorusDialog::GetInternalRadius()const
	{
		double l_value = m_properties->GetPropertyValue( TORUS_INTERNAL_RADIUS );
		return real( l_value );
	}

	real NewTorusDialog::GetExternalRadius()const
	{
		double l_value = m_properties->GetPropertyValue( TORUS_EXTERNAL_RADIUS );
		return real( l_value );
	}

	int NewTorusDialog::GetInternalFacesCount()const
	{
		long l_value = m_properties->GetPropertyValue( TORUS_INTERNAL_WIDTH_SUBDIVISIONS );
		return int( l_value );
	}

	int NewTorusDialog::GetExternalFacesCount()const
	{
		long l_value = m_properties->GetPropertyValue( TORUS_EXTERNAL_DEPTH_SUBDIVISIONS );
		return int( l_value );
	}
}
