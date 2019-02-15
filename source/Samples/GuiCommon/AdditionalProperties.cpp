#include "AdditionalProperties.hpp"

#include "GradientButton.hpp"

#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>

using namespace castor;

namespace GuiCommon
{
	//************************************************************************************************

	ButtonData::ButtonData( ButtonEventMethod p_method, wxEvtHandler * p_handler )
		: m_method( p_method )
		, m_handler( p_handler )
	{
	}

	bool ButtonData::Call( wxPGProperty * property )
	{
		return ( *m_handler.*m_method )( property );
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
				return btn->Call( p_property );
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

	wxStringProperty * CreateProperty( wxString const & p_name, wxString const & p_value, ButtonEventMethod p_method, wxEvtHandler * p_handler, wxPGEditor * p_editor )
	{
		wxStringProperty * result = new wxStringProperty( p_name, wxPG_LABEL, p_value );
		result->SetEditor( p_editor );
		result->SetClientObject( new ButtonData( p_method, p_handler ) );
		return result;
	}

	//************************************************************************************************
}
