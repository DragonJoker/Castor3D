#include "RendererSelector.hpp"

#include "ImagesLoader.hpp"
#include "GradientButton.hpp"

#include <Engine.hpp>
#include <Cache/PluginCache.hpp>
#include <Plugin/RendererPlugin.hpp>

using namespace castor3d;

namespace GuiCommon
{
	RendererSelector::RendererSelector( Engine * engine, wxWindow * p_parent, wxString const & p_strTitle )
		: wxDialog( p_parent, wxID_ANY, p_strTitle + _( " - Select renderer" ), wxDefaultPosition, wxSize( 500, 500 ), wxDEFAULT_DIALOG_STYLE )
		, m_pImgCastor( ImagesLoader::getBitmap( CV_IMG_CASTOR ) )
		, m_engine( engine )
	{
		SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		SetForegroundColour( PANEL_FOREGROUND_COLOUR );

		wxStaticText * pTitle;
		wxStaticText * pStatic;
		wxSize size = GetClientSize();
		wxFont font = wxFont( 30, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT( "Arial" ) );
		int iCount = 0;
		wxString strSelect = _( "Select your renderer in the list below" );
		wxString strOk = _( "OK" );
		wxString strCancel = _( "Cancel" );
		pTitle = new wxStaticText( this, wxID_ANY, p_strTitle );
		pTitle->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		pTitle->SetForegroundColour( PANEL_FOREGROUND_COLOUR );

		pStatic = new wxStaticText( this, wxID_ANY, strSelect );
		pStatic->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		pStatic->SetForegroundColour( PANEL_FOREGROUND_COLOUR );

		m_pListRenderers = new wxListBox( this, eID_LIST_RENDERERS, wxDefaultPosition, wxSize( 400, 100 ) );
		m_pListRenderers->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_pListRenderers->SetForegroundColour( PANEL_FOREGROUND_COLOUR );

		GradientButton * ok = new GradientButton( this, wxID_OK, strOk );
		GradientButton * cancel = new GradientButton( this, wxID_CANCEL, strCancel );
		pTitle->SetFont( font );

		for ( auto it : m_engine->getPluginCache().getPlugins( PluginType::eRenderer ) )
		{
			if ( it.second )
			{
				m_pListRenderers->Insert( it.second->getName(), iCount++, it.second.get() );
			}
		}

		if ( m_pListRenderers->GetCount() > 0 )
		{
			m_pListRenderers->Select( 0 );
		}

		wxBoxSizer * pSizer = new wxBoxSizer( wxVERTICAL );
		wxBoxSizer * pButtonSizer = new wxBoxSizer( wxHORIZONTAL );
		wxBoxSizer * pTitleSizer = new wxBoxSizer( wxHORIZONTAL );
		wxBoxSizer * pDescSizer = new wxBoxSizer( wxHORIZONTAL );
		pTitleSizer->Add( 50, 0, 1 );
		pTitleSizer->Add( pTitle, wxSizerFlags( 0 ).Border( wxRIGHT, 5 ) );
		pDescSizer->Add( 50, 0, 1 );
		pDescSizer->Add( pStatic, wxSizerFlags( 0 ).Border( wxRIGHT, 5 ) );
		pButtonSizer->Add( ok, wxSizerFlags( 0 ).Border( wxLEFT, 5 ) );
		pButtonSizer->Add( 200, 0, 1 );
		pButtonSizer->Add( cancel, wxSizerFlags( 0 ).Border( wxRIGHT, 5 ) );
		pSizer->Add( pTitleSizer, wxSizerFlags( 0 ).Border( wxALL, 5 ).Expand() );
		pSizer->Add( pDescSizer, wxSizerFlags( 0 ).Border( wxALL, 5 ).Expand() );
		pSizer->Add( 0, 60, 0 );
		pSizer->Add( m_pListRenderers, wxSizerFlags( 1 ).Border( wxALL, 10 ).Expand() );
		pSizer->Add( pButtonSizer, wxSizerFlags( 0 ).Border( wxALL,  5 ).Expand() );
		SetSizer( pSizer );
		pSizer->SetSizeHints( this );
		wxClientDC clientDC( this );
		doDraw( & clientDC );
	}

	RendererSelector::~RendererSelector()
	{
	}

	castor::String RendererSelector::getSelectedRenderer()const
	{
		castor::String result = RENDERER_TYPE_UNDEFINED;
		uint32_t selected = m_pListRenderers->GetSelection();

		if ( selected < m_pListRenderers->GetCount() )
		{
			result = static_cast< RendererPlugin * >( m_pListRenderers->GetClientData( selected ) )->getRendererType();
		}

		return result;
	}

	void RendererSelector::doDraw( wxDC * p_pDC )
	{
		p_pDC->DrawBitmap( *m_pImgCastor, wxPoint( 0, 0 ), true );
	}

	void RendererSelector::doSelect()
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
		wxPaintDC paintDC( this );
		doDraw( & paintDC );
		p_event.Skip();
	}

	void RendererSelector::OnKeyUp( wxKeyEvent & p_event )
	{
		switch ( p_event.GetKeyCode() )
		{
		case WXK_RETURN:
			doSelect();
			break;
		}

		p_event.Skip();
	}

	void RendererSelector::OnButtonOk( wxCommandEvent & p_event )
	{
		doSelect();
		p_event.Skip();
	}

	void RendererSelector::OnButtonCancel( wxCommandEvent & p_event )
	{
		EndModal( wxID_CANCEL );
		p_event.Skip();
	}
}
