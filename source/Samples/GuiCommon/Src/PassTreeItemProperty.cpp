#include "PassTreeItemProperty.hpp"

#include <Pass.hpp>

#include "AdditionalProperties.hpp"
#include <wx/propgrid/advprops.h>

using namespace Castor3D;
using namespace Castor;

namespace GuiCommon
{
	namespace
	{
		static const wxString PROPERTY_CATEGORY_PASS = _( "Pass: " );
		static const wxString PROPERTY_PASS_AMBIENT = _( "Ambient" );
		static const wxString PROPERTY_PASS_DIFFUSE = _( "Diffuse" );
		static const wxString PROPERTY_PASS_SPECULAR = _( "Specular" );
		static const wxString PROPERTY_PASS_EMISSIVE = _( "Emissive" );
		static const wxString PROPERTY_PASS_EXPONENT = _( "Exponent" );
		static const wxString PROPERTY_PASS_TWO_SIDED = _( "Two sided" );
		static const wxString PROPERTY_PASS_OPACITY = _( "Opacity" );
	}

	wxPassTreeItemProperrty::wxPassTreeItemProperrty( Castor3D::PassSPtr p_pass )
		: wxTreeItemProperty( ePROPERTY_DATA_TYPE_PASS )
		, m_pass( p_pass )
	{
	}

	wxPassTreeItemProperrty::~wxPassTreeItemProperrty()
	{
	}

	void wxPassTreeItemProperrty::CreateProperties( wxPropertyGrid * p_grid )
	{
		PassSPtr l_pass = GetPass();

		if ( l_pass )
		{
			p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_PASS + wxString( l_pass->GetParent()->GetName() ) ) );
			p_grid->Append( new wxColourProperty( PROPERTY_PASS_DIFFUSE ) )->SetValue( wxVariant( wxColour( l_pass->GetAmbient().to_bgr() ) ) );
			p_grid->Append( new wxColourProperty( PROPERTY_PASS_AMBIENT ) )->SetValue( wxVariant( wxColour( l_pass->GetDiffuse().to_bgr() ) ) );
			p_grid->Append( new wxColourProperty( PROPERTY_PASS_SPECULAR ) )->SetValue( wxVariant( wxColour( l_pass->GetSpecular().to_bgr() ) ) );
			p_grid->Append( new wxColourProperty( PROPERTY_PASS_EMISSIVE ) )->SetValue( wxVariant( wxColour( l_pass->GetEmissive().to_bgr() ) ) );
			p_grid->Append( new wxFloatProperty( PROPERTY_PASS_EXPONENT ) )->SetValue( l_pass->GetShininess() );
			p_grid->Append( new wxBoolProperty( PROPERTY_PASS_TWO_SIDED, wxPG_BOOL_USE_CHECKBOX ) )->SetValue( l_pass->IsTwoSided() );
			p_grid->Append( new wxFloatProperty( PROPERTY_PASS_OPACITY ) )->SetValue( l_pass->GetAlpha() );
		}
	}

	void wxPassTreeItemProperrty::OnPropertyChange( wxPropertyGridEvent & p_event )
	{
		PassSPtr l_pass = GetPass();
		wxPGProperty * l_property = p_event.GetProperty();

		if ( l_property && l_pass )
		{
		}
	}
}
