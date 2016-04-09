#include "NewCubeDialog.hpp"

#include <wx/propgrid/advprops.h>

using namespace Castor3D;

namespace CastorShape
{
	namespace
	{
		static wxString CUBE_WIDTH = _( "Radius" );
		static wxString CUBE_HEIGHT = _( "Height" );
		static wxString CUBE_DEPTH = _( "Face count" );
	}

	NewCubeDialog::NewCubeDialog( Engine * p_engine, wxWindow * parent, wxWindowID p_id )
		: NewGeometryDialog( p_engine, parent, p_id, _( "New cube" ) )
	{
		CUBE_WIDTH = _( "Radius" );
		CUBE_HEIGHT = _( "Height" );
		CUBE_DEPTH = _( "Face count" );
		DoInitialise();
	}

	NewCubeDialog::~NewCubeDialog()
	{
	}

	void NewCubeDialog::DoCreateProperties()
	{
		m_properties->Append( new wxFloatProperty( CUBE_WIDTH ) )->SetValue( 1.0 );
		m_properties->Append( new wxFloatProperty( CUBE_HEIGHT ) )->SetValue( 1.0 );
		m_properties->Append( new wxFloatProperty( CUBE_DEPTH ) )->SetValue( 1.0 );
	}

	real NewCubeDialog::GetWidth()const
	{
		double l_value = m_properties->GetPropertyValue( CUBE_WIDTH );
		return real( l_value );
	}

	real NewCubeDialog::GetHeight()const
	{
		double l_value = m_properties->GetPropertyValue( CUBE_HEIGHT );
		return real( l_value );
	}

	real NewCubeDialog::GetDepth()const
	{
		double l_value = m_properties->GetPropertyValue( CUBE_DEPTH );
		return real( l_value );
	}
}
