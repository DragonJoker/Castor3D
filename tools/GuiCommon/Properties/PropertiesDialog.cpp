#include "GuiCommon/Properties/PropertiesDialog.hpp"
#include "GuiCommon/System/GradientButton.hpp"

namespace GuiCommon
{
	PropertiesDialog::PropertiesDialog( wxWindow * parent
		, const wxString & title
		, TreeItemPropertyUPtr properties
		, const wxPoint & pos
		, const wxSize & wndSize )
		: wxDialog{ parent, wxID_ANY, title, pos, wndSize }
		, m_properties{ std::move( properties ) }
	{
		auto size = GetClientSize();
		SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		SetForegroundColour( PANEL_FOREGROUND_COLOUR );

		wxBoxSizer * sizerAll = new wxBoxSizer{ wxVERTICAL };
		m_holder = new PropertiesContainer{ true, this };
		m_holder->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_holder->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_holder->setPropertyData( m_properties.get() );
		m_holder->SetSize( size - wxSize{ 0, 100 } );
		sizerAll->Add( m_holder, wxSizerFlags( 1 ).Expand().Border( wxTOP | wxBOTTOM | wxLEFT | wxRIGHT, 10 ) );

		wxBoxSizer * sizerButtons = new wxBoxSizer{ wxHORIZONTAL };
		sizerButtons->Add( new GradientButton{ this, wxID_OK, _( "Validate" ), wxDefaultPosition, wxSize{ 75, 25 }, wxBORDER_SIMPLE }, wxSizerFlags( 1 ).Border( wxLEFT, 10 ) );
		sizerButtons->AddStretchSpacer();
		sizerButtons->Add( new GradientButton{ this, wxID_CANCEL, _( "Cancel" ), wxDefaultPosition, wxSize( 75, 25 ), wxBORDER_SIMPLE }, wxSizerFlags( 1 ).Border( wxRIGHT, 10 ) );
		sizerAll->Add( sizerButtons, wxSizerFlags().Expand().Border( wxBOTTOM, 10 ) );

		sizerAll->SetMinSize( size );
		sizerAll->SetSizeHints( this );
		SetSizer( sizerAll );
	}
}
