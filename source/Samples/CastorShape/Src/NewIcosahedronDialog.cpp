#include "NewIcosahedronDialog.hpp"

#include <wx/propgrid/advprops.h>

using namespace Castor3D;
using namespace Castor;

namespace CastorShape
{
	namespace
	{
		static wxString ICOSAHEDRON_RADIUS = _( "Radius" );
		static wxString ICOSAHEDRON_SUBDIVISIONS = _( "Subdivisions count" );
	}

	NewIcosahedronDialog::NewIcosahedronDialog( Engine * p_engine, wxWindow * parent, wxWindowID p_id )
		: NewGeometryDialog( p_engine, parent, p_id, _( "New icosaedron" ) )
	{
		ICOSAHEDRON_RADIUS = _( "Radius" );
		ICOSAHEDRON_SUBDIVISIONS = _( "Subdivisions count" );
		DoInitialise();
	}

	NewIcosahedronDialog::~NewIcosahedronDialog()
	{
	}

	void NewIcosahedronDialog::DoCreateProperties()
	{
		m_properties->Append( new wxFloatProperty( ICOSAHEDRON_RADIUS ) )->SetValue( 1.0 );
		m_properties->Append( new wxIntProperty( ICOSAHEDRON_SUBDIVISIONS ) )->SetValue( 4 );
	}

	real NewIcosahedronDialog::GetRadius()const
	{
		double l_value = m_properties->GetPropertyValue( ICOSAHEDRON_RADIUS );
		return real( l_value );
	}

	int NewIcosahedronDialog::GetSubdivisionCount()const
	{
		long l_value = m_properties->GetPropertyValue( ICOSAHEDRON_SUBDIVISIONS );
		return int( l_value );
	}
}
