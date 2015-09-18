#include "NewCylinderDialog.hpp"

#include <wx/propgrid/advprops.h>

using namespace Castor3D;
using namespace Castor;

namespace CastorShape
{
	namespace
	{
		static wxString CYLINDER_RADIUS = _( "Radius" );
		static wxString CYLINDER_HEIGHT = _( "Height" );
		static wxString CYLINDER_FACES = _( "Face count" );
	}

	NewCylinderDialog::NewCylinderDialog( Engine * p_pEngine, wxWindow * parent, wxWindowID p_id )
		:	NewGeometryDialog( p_pEngine, parent, p_id, _( "New cylinder" ) )
	{
		CYLINDER_RADIUS = _( "Radius" );
		CYLINDER_HEIGHT = _( "Height" );
		CYLINDER_FACES = _( "Face count" );
		DoInitialise();
	}

	NewCylinderDialog::~NewCylinderDialog()
	{
	}

	void NewCylinderDialog::DoCreateProperties()
	{
		m_properties->Append( new wxFloatProperty( CYLINDER_RADIUS ) )->SetValue( 0.5 );
		m_properties->Append( new wxFloatProperty( CYLINDER_HEIGHT ) )->SetValue( 1.0 );
		m_properties->Append( new wxIntProperty( CYLINDER_FACES ) )->SetValue( 10 );
	}

	real NewCylinderDialog::GetRadius()const
	{
		double l_value = m_properties->GetPropertyValue( CYLINDER_RADIUS );
		return real( l_value );
	}

	real NewCylinderDialog::GetHeight()const
	{
		double l_value = m_properties->GetPropertyValue( CYLINDER_HEIGHT );
		return real( l_value );
	}

	int NewCylinderDialog::GetFacesCount()const
	{
		long l_value = m_properties->GetPropertyValue( CYLINDER_FACES );
		return int( l_value );
	}
}
