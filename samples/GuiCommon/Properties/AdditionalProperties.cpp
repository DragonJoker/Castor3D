#include "GuiCommon/Properties/AdditionalProperties.hpp"

#include "GuiCommon/System/GradientButton.hpp"

#include <wx/propgrid/advprops.h>

using namespace castor;

namespace GuiCommon
{
	//************************************************************************************************

	ButtonData::ButtonData( ButtonEventMethod method )
		: m_method( method )
	{
	}

	void ButtonData::Call( wxVariant const & var )
	{
		m_method( var );
	}

	//************************************************************************************************

	wxPGWindowList ButtonEventEditor::CreateControls( wxPropertyGrid * p_propgrid, wxPGProperty * p_property, wxPoint const & p_pos, wxSize const & p_size )const
	{
		// create and return a single button to be used as editor
		// size and pos represent the entire value cell: use that to position the button
		return wxPGWindowList( new GradientButton( p_propgrid, wxID_ANY, _( "View" ), p_pos, p_size ) );
	}

	// since the editor does not need to change the primary control (the button)
	// to reflect changes, UpdateControl is just a no-op
	void ButtonEventEditor::UpdateControl( wxPGProperty * property, wxWindow * ctrl ) const
	{
	}

	bool ButtonEventEditor::OnEvent( wxPropertyGrid * p_propgrid, wxPGProperty * p_property, wxWindow * p_wnd_primary, wxEvent & p_event )const
	{
		// handle the button event
		if ( p_event.GetEventType() == wxEVT_COMMAND_BUTTON_CLICKED )
		{
			// extract the client data from the property
			if ( ButtonData * btn = dynamic_cast< ButtonData * >( p_property->GetClientObject() ) )
			{
				// call the method
				btn->Call( p_property->GetValue() );
				return false;
			}
		}

		return false;
	}
	//************************************************************************************************

	wxFloatProperty * CreateProperty( wxString const & p_name, double const & p_value )
	{
		return new wxFloatProperty( p_name, wxPG_LABEL, p_value );
	}

	wxFloatProperty * CreateProperty( wxString const & p_name, float const & p_value )
	{
		return new wxFloatProperty( p_name, wxPG_LABEL, p_value );
	}

	wxIntProperty * CreateProperty( wxString const & p_name, int const & p_value )
	{
		return new wxIntProperty( p_name, wxPG_LABEL, p_value );
	}

	wxUIntProperty * CreateProperty( wxString const & p_name, uint32_t const & p_value )
	{
		return new wxUIntProperty( p_name, wxPG_LABEL, p_value );
	}

	wxBoolProperty * CreateProperty( wxString const & p_name, bool const & p_value, bool p_checkbox )
	{
		wxBoolProperty * result = new wxBoolProperty( p_name, wxPG_LABEL );
		result->SetAttribute( wxT( "UseCheckbox" ), p_checkbox );
		return result;
	}

	wxStringProperty * CreateProperty( wxString const & p_name, wxString const & p_value )
	{
		return new wxStringProperty( p_name, wxPG_LABEL, p_value );
	}

	wxPGProperty * addAttributes( wxPGProperty * prop )
	{
		prop->SetEditor( wxPGEditor_SpinCtrl );
		prop->SetAttribute( wxPG_ATTR_SPINCTRL_WRAP, WXVARIANT( true ) );
		prop->SetAttribute( wxPG_ATTR_SPINCTRL_MOTION, WXVARIANT( true ) );
		return prop;
	}

	//************************************************************************************************
}
