#include "NewConeDialog.hpp"

#include <wx/propgrid/advprops.h>

using namespace Castor3D;
using namespace Castor;

namespace CastorShape
{
	namespace
	{
		static wxString CONE_RADIUS = _( "Radius" );
		static wxString CONE_HEIGHT = _( "Height" );
		static wxString CONE_FACES = _( "Face count" );
	}

	NewConeDialog::NewConeDialog( Scene & p_scene, wxWindow * parent, wxWindowID p_id )
		: NewGeometryDialog( p_scene, parent, p_id, _( "New cone" ) )
	{
		CONE_RADIUS = _( "Radius" );
		CONE_HEIGHT = _( "Height" );
		CONE_FACES = _( "Face count" );
		DoInitialise();
	}

	NewConeDialog::~NewConeDialog()
	{
	}

	void NewConeDialog::DoCreateProperties()
	{
		m_properties->Append( new wxFloatProperty( CONE_RADIUS ) )->SetValue( 0.5 );
		m_properties->Append( new wxFloatProperty( CONE_HEIGHT ) )->SetValue( 1.0 );
		m_properties->Append( new wxIntProperty( CONE_FACES ) )->SetValue( 10 );
	}

	real NewConeDialog::GetRadius()const
	{
		double l_value = m_properties->GetPropertyValue( CONE_RADIUS );
		return real( l_value );
	}

	real NewConeDialog::GetHeight()const
	{
		double l_value = m_properties->GetPropertyValue( CONE_HEIGHT );
		return real( l_value );
	}

	int NewConeDialog::GetFacesCount()const
	{
		long l_value = m_properties->GetPropertyValue( CONE_FACES );
		return int( l_value );
	}
}
