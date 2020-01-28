#include "GuiCommon/RendererSelector.hpp"

#include "GuiCommon/ImagesLoader.hpp"
#include "GuiCommon/GradientButton.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/PluginCache.hpp>

using namespace castor3d;

namespace GuiCommon
{
	RendererSelector::RendererSelector( Engine & engine
		, wxWindow * parent
		, wxString const & title )
		: wxDialog{ parent, wxID_ANY, title + _( " - Select renderer" ), wxDefaultPosition, wxSize( 500, 500 ), wxDEFAULT_DIALOG_STYLE }
		, m_castorImg{ ImagesLoader::getBitmap( CV_IMG_CASTOR ) }
		, m_engine{ engine }
	{
		SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		SetForegroundColour( PANEL_FOREGROUND_COLOUR );

		m_renderers = new wxListBox( this, ID_LIST_RENDERERS, wxDefaultPosition, wxSize( 400, 100 ) );
		m_renderers->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_renderers->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_renderers->Bind( wxEVT_KEY_UP, [this]( wxKeyEvent & event ){ OnKeyUp( event ); } );

		int count = 0;

		for ( auto & renderer : m_engine.getRenderersList() )
		{
			m_names.push_back( renderer.name );
		}

		for ( auto & renderer : m_engine.getRenderersList() )
		{
			auto name = make_wxString( renderer.description );
			name.Replace( wxT( " for Ashes" ), wxEmptyString );
			m_renderers->Insert( name, count, &m_names[count] );
			++count;
		}

		if ( !m_renderers->IsEmpty() )
		{
			m_renderers->Select( 0 );
		}

		wxBoxSizer * sizer = new wxBoxSizer( wxVERTICAL );

		wxBoxSizer * titleSizer = new wxBoxSizer( wxHORIZONTAL );
		titleSizer->Add( 50, 0, 1 );
		auto titleTxt = new wxStaticText( this, wxID_ANY, title );
		titleTxt->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		titleTxt->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		titleTxt->SetFont( wxFont{ 30, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT( "Arial" ) } );
		titleSizer->Add( titleTxt, wxSizerFlags( 0 ).Border( wxRIGHT, 5 ) );
		sizer->Add( titleSizer, wxSizerFlags( 0 ).Border( wxALL, 5 ).Expand() );

		wxBoxSizer * descSizer = new wxBoxSizer( wxHORIZONTAL );
		descSizer->Add( 50, 0, 1 );
		auto staticTxt = new wxStaticText( this, wxID_ANY, _( "Select your renderer in the list below" ) );
		staticTxt->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		staticTxt->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		descSizer->Add( staticTxt, wxSizerFlags( 0 ).Border( wxRIGHT, 5 ) );
		sizer->Add( descSizer, wxSizerFlags( 0 ).Border( wxALL, 5 ).Expand() );

		wxBoxSizer * buttonSizer = new wxBoxSizer( wxHORIZONTAL );
		GradientButton * ok = new GradientButton( this, wxID_OK, _( "OK" ) );
		buttonSizer->Add( ok, wxSizerFlags( 0 ).Border( wxLEFT, 5 ) );
		buttonSizer->Add( 200, 0, 1 );
		GradientButton * cancel = new GradientButton( this, wxID_CANCEL, _( "Cancel" ) );
		buttonSizer->Add( cancel, wxSizerFlags( 0 ).Border( wxRIGHT, 5 ) );

		sizer->Add( 0, 60, 0 );
		sizer->Add( m_renderers, wxSizerFlags( 1 ).Border( wxALL, 10 ).Expand() );
		sizer->Add( buttonSizer, wxSizerFlags( 0 ).Border( wxALL,  5 ).Expand() );
		SetSizer( sizer );
		sizer->SetSizeHints( this );
		wxClientDC clientDC( this );
		doDraw( & clientDC );
	}

	RendererSelector::~RendererSelector()
	{
	}

	castor::String RendererSelector::getSelectedRenderer()const
	{
		castor::String result = castor3d::RENDERER_TYPE_UNDEFINED;
		uint32_t selected = m_renderers->GetSelection();

		if ( selected < m_renderers->GetCount() )
		{
			result = *static_cast< castor::String * >( m_renderers->GetClientData( selected ) );
		}

		return result;
	}

	void RendererSelector::doDraw( wxDC * dc )
	{
		dc->DrawBitmap( *m_castorImg, wxPoint( 0, 0 ), true );
	}

	void RendererSelector::doSelect()
	{
		if ( m_renderers->GetCount() > 0 )
		{
			EndModal( wxID_OK );
		}
		else
		{
			EndModal( wxID_CANCEL );
		}
	}

	BEGIN_EVENT_TABLE( RendererSelector, wxDialog )
		EVT_PAINT(	RendererSelector::OnPaint )
		EVT_KEY_UP(	RendererSelector::OnKeyUp )
		EVT_BUTTON(	wxID_OK, RendererSelector::OnButtonOk )
		EVT_BUTTON(	wxID_CANCEL, RendererSelector::OnButtonCancel )
		EVT_LISTBOX_DCLICK( ID_LIST_RENDERERS,	RendererSelector::OnButtonOk )
	END_EVENT_TABLE()

	void RendererSelector::OnPaint( wxPaintEvent & event )
	{
		wxPaintDC paintDC( this );
		doDraw( & paintDC );
		event.Skip();
	}

	void RendererSelector::OnKeyUp( wxKeyEvent & event )
	{
		switch ( event.GetKeyCode() )
		{
		case WXK_RETURN:
			doSelect();
			break;
		}

		event.Skip();
	}

	void RendererSelector::OnButtonOk( wxCommandEvent & event )
	{
		doSelect();
		event.Skip();
	}

	void RendererSelector::OnButtonCancel( wxCommandEvent & event )
	{
		EndModal( wxID_CANCEL );
		event.Skip();
	}
}
