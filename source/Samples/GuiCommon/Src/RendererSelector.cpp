#include "RendererSelector.hpp"

#include "ImagesLoader.hpp"
#include "GradientButton.hpp"

#include <Engine.hpp>
#include <PluginManager.hpp>
#include <Plugin/RendererPlugin.hpp>

using namespace Castor3D;

namespace GuiCommon
{
	RendererSelector::RendererSelector( Engine * p_engine, wxWindow * p_parent, wxString const & p_strTitle )
		: wxDialog( p_parent, wxID_ANY, p_strTitle + _( " - Select renderer" ), wxDefaultPosition, wxSize( 500, 500 ), wxDEFAULT_DIALOG_STYLE )
		, m_pImgCastor( ImagesLoader::GetBitmap( CV_IMG_CASTOR ) )
		, m_engine( p_engine )
	{
		SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		SetForegroundColour( PANEL_FOREGROUND_COLOUR );

		wxStaticText * l_pTitle;
		wxStaticText * l_pStatic;
		wxSize l_size = GetClientSize();
		wxFont l_font = wxFont( 30, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT( "Arial" ) );
		int l_iCount = 0;
		wxString l_strSelect = _( "Select your renderer in the list below" );
		wxString l_strOk = _( "OK" );
		wxString l_strCancel = _( "Cancel" );
		l_pTitle = new wxStaticText( this, wxID_ANY, p_strTitle );
		l_pTitle->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		l_pTitle->SetForegroundColour( PANEL_FOREGROUND_COLOUR );

		l_pStatic = new wxStaticText( this, wxID_ANY, l_strSelect );
		l_pStatic->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		l_pStatic->SetForegroundColour( PANEL_FOREGROUND_COLOUR );

		m_pListRenderers = new wxListBox( this, eID_LIST_RENDERERS, wxDefaultPosition, wxSize( 400, 100 ) );
		m_pListRenderers->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_pListRenderers->SetForegroundColour( PANEL_FOREGROUND_COLOUR );

		GradientButton * l_ok = new GradientButton( this, wxID_OK, l_strOk );
		GradientButton * l_cancel = new GradientButton( this, wxID_CANCEL, l_strCancel );
		l_pTitle->SetFont( l_font );

		for ( auto l_renderer : m_engine->GetPluginManager().GetRenderersList() )
		{
			if ( l_renderer )
			{
				m_pListRenderers->Insert( l_renderer->GetName(), l_iCount++, l_renderer.get() );
			}
		}

		if ( m_pListRenderers->GetCount() > 0 )
		{
			m_pListRenderers->Select( 0 );
		}

		wxBoxSizer * l_pSizer = new wxBoxSizer( wxVERTICAL );
		wxBoxSizer * l_pButtonSizer = new wxBoxSizer( wxHORIZONTAL );
		wxBoxSizer * l_pTitleSizer = new wxBoxSizer( wxHORIZONTAL );
		wxBoxSizer * l_pDescSizer = new wxBoxSizer( wxHORIZONTAL );
		l_pTitleSizer->Add( 50, 0, 1 );
		l_pTitleSizer->Add( l_pTitle, wxSizerFlags( 0 ).Border( wxRIGHT, 5 ) );
		l_pDescSizer->Add( 50, 0, 1 );
		l_pDescSizer->Add( l_pStatic, wxSizerFlags( 0 ).Border( wxRIGHT, 5 ) );
		l_pButtonSizer->Add( l_ok, wxSizerFlags( 0 ).Border( wxLEFT, 5 ) );
		l_pButtonSizer->Add( 200, 0, 1 );
		l_pButtonSizer->Add( l_cancel, wxSizerFlags( 0 ).Border( wxRIGHT, 5 ) );
		l_pSizer->Add( l_pTitleSizer, wxSizerFlags( 0 ).Border( wxALL, 5 ).Expand() );
		l_pSizer->Add( l_pDescSizer, wxSizerFlags( 0 ).Border( wxALL, 5 ).Expand() );
		l_pSizer->Add( 0, 60, 0 );
		l_pSizer->Add( m_pListRenderers, wxSizerFlags( 1 ).Border( wxALL, 10 ).Expand() );
		l_pSizer->Add( l_pButtonSizer, wxSizerFlags( 0 ).Border( wxALL,  5 ).Expand() );
		SetSizer( l_pSizer );
		l_pSizer->SetSizeHints( this );
		wxClientDC l_clientDC( this );
		DoDraw( & l_clientDC );
	}

	RendererSelector::~RendererSelector()
	{
	}

	eRENDERER_TYPE RendererSelector::GetSelectedRenderer()const
	{
		eRENDERER_TYPE l_eReturn = eRENDERER_TYPE_UNDEFINED;
		uint32_t l_uiReturn = m_pListRenderers->GetSelection();

		if ( l_uiReturn >= 0 && l_uiReturn < m_pListRenderers->GetCount() )
		{
			l_eReturn = static_cast< RendererPlugin * >( m_pListRenderers->GetClientData( l_uiReturn ) )->GetRendererType();
		}

		return l_eReturn;
	}

	void RendererSelector::DoDraw( wxDC * p_pDC )
	{
		p_pDC->DrawBitmap( *m_pImgCastor, wxPoint( 0, 0 ), true );
	}

	void RendererSelector::DoSelect()
	{
		if ( m_pListRenderers->GetCount() > 0 )
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
		EVT_LISTBOX_DCLICK( eID_LIST_RENDERERS,	RendererSelector::OnButtonOk )
	END_EVENT_TABLE()

	void RendererSelector::OnPaint( wxPaintEvent & p_event )
	{
		wxPaintDC l_paintDC( this );
		DoDraw( & l_paintDC );
		p_event.Skip();
	}

	void RendererSelector::OnKeyUp( wxKeyEvent & p_event )
	{
		switch ( p_event.GetKeyCode() )
		{
		case WXK_RETURN:
			DoSelect();
			break;
		}

		p_event.Skip();
	}

	void RendererSelector::OnButtonOk( wxCommandEvent & p_event )
	{
		DoSelect();
		p_event.Skip();
	}

	void RendererSelector::OnButtonCancel( wxCommandEvent & p_event )
	{
		EndModal( wxID_CANCEL );
		p_event.Skip();
	}
}
