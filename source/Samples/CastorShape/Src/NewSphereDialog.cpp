#include "NewSphereDialog.hpp"

#include <wx/propgrid/advprops.h>

using namespace Castor3D;
using namespace Castor;

namespace CastorShape
{
	namespace
	{
		static wxString SPHERE_RADIUS = _( "Radius" );
		static wxString SPHERE_SUBDIVISIONS = _( "Subdivisions" );
	}

	NewSphereDialog::NewSphereDialog( Engine * p_engine, wxWindow * parent, wxWindowID p_id )
		: NewGeometryDialog( p_engine, parent, p_id, _( "New sphere" ) )
	{
		SPHERE_RADIUS = _( "Radius" );
		SPHERE_SUBDIVISIONS = _( "Subdivisions" );
		DoInitialise();
	}

	NewSphereDialog::~NewSphereDialog()
	{
	}

	void NewSphereDialog::DoCreateProperties()
	{
		m_properties->Append( new wxFloatProperty( SPHERE_RADIUS ) )->SetValue( 1.0 );
		m_properties->Append( new wxIntProperty( SPHERE_SUBDIVISIONS ) )->SetValue( 10 );
	}

	real NewSphereDialog::GetRadius()const
	{
		double l_value = m_properties->GetPropertyValue( SPHERE_RADIUS );
		return real( l_value );
	}

	int NewSphereDialog::GetFacesCount()const
	{
		long l_value = m_properties->GetPropertyValue( SPHERE_SUBDIVISIONS );
		return int( l_value );
	}
}
