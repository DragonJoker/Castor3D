#include "RendererSelector.hpp"
#include "ImagesLoader.hpp"

#include <Engine.hpp>
#include <RendererPlugin.hpp>

using namespace GuiCommon;
using namespace Castor3D;

wxRendererSelector::wxRendererSelector( Engine * p_pEngine, wxWindow * p_pParent, wxString const & p_strTitle )
	:	wxDialog( p_pParent, wxID_ANY, p_strTitle + _( " - Select renderer" ), wxDefaultPosition, wxSize( 500, 500 ), wxDEFAULT_DIALOG_STYLE )
	,	m_pImgCastor( wxImagesLoader::GetBitmap( CV_IMG_CASTOR ) )
	,	m_pEngine( p_pEngine )
{
	wxStaticText *	l_pTitle;
	wxStaticText *	l_pStatic;
	wxSize			l_size			= GetClientSize();
	wxFont			l_font			= wxFont( 30, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT( "Arial" ) );
	int				l_iCount		= 0;
	wxString		l_strSelect		= _( "Select your renderer in the list below" );
	wxString		l_strOk			= _( "OK" );
	wxString		l_strCancel		= _( "Cancel" );
	SetBackgroundColour( wxColour( 128, 128, 128 ) );
	l_pTitle			= new wxStaticText(	this, wxID_ANY,				p_strTitle	);
	l_pStatic			= new wxStaticText(	this, wxID_ANY,				l_strSelect	);
	m_pListRenderers	= new wxListBox(	this, eID_LIST_RENDERERS,	wxDefaultPosition, wxSize( 400, 100 )	);
	m_pBtnOk			= new wxButton(	this, wxID_OK,				l_strOk	);
	m_pBtnCancel		= new wxButton(	this, wxID_CANCEL,			l_strCancel	);
	l_pTitle->SetFont( l_font );

	for ( RendererPtrArrayIt l_it = m_pEngine->RenderersBegin(); l_it != m_pEngine->RenderersEnd(); ++l_it )
	{
		if ( * l_it )
		{
			m_pListRenderers->Insert( ( * l_it )->GetName(), l_iCount++, ( * l_it ).get() );
		}
	}

	if ( m_pListRenderers->GetCount() > 0 )
	{
		m_pListRenderers->Select( 0 );
	}

	wxBoxSizer *	l_pSizer		= new wxBoxSizer( wxVERTICAL	);
	wxBoxSizer *	l_pButtonSizer	= new wxBoxSizer( wxHORIZONTAL	);
	wxBoxSizer *	l_pTitleSizer	= new wxBoxSizer( wxHORIZONTAL	);
	wxBoxSizer *	l_pDescSizer	= new wxBoxSizer( wxHORIZONTAL	);
	l_pTitleSizer->Add(	50, 0,				1	);
	l_pTitleSizer->Add(	l_pTitle,			wxSizerFlags( 0 ).Border( wxRIGHT,	  5 )	);
	l_pDescSizer->Add(	50, 0,				1	);
	l_pDescSizer->Add(	l_pStatic,			wxSizerFlags( 0 ).Border( wxRIGHT,	  5 )	);
	l_pButtonSizer->Add(	m_pBtnOk,			wxSizerFlags( 0 ).Border( wxLEFT,	  5 )	);
	l_pButtonSizer->Add(	200, 0,				1	);
	l_pButtonSizer->Add(	m_pBtnCancel,		wxSizerFlags( 0 ).Border( wxRIGHT,	  5 )	);
	l_pSizer->Add(	l_pTitleSizer,		wxSizerFlags( 0 ).Border( wxALL,	  5 ).Expand()	);
	l_pSizer->Add(	l_pDescSizer,		wxSizerFlags( 0 ).Border( wxALL,	  5 ).Expand()	);
	l_pSizer->Add(	0, 60,				0	);
	l_pSizer->Add(	m_pListRenderers,	wxSizerFlags( 1 ).Border( wxALL,	 10 ).Expand()	);
	l_pSizer->Add(	l_pButtonSizer,		wxSizerFlags( 0 ).Border( wxALL,	  5 ).Expand()	);
	SetSizer( l_pSizer );
	l_pSizer->SetSizeHints( this );
	wxClientDC l_clientDC( this );
	DoDraw( & l_clientDC );
}

wxRendererSelector::~wxRendererSelector()
{
}

eRENDERER_TYPE wxRendererSelector::GetSelectedRenderer()const
{
	eRENDERER_TYPE l_eReturn = eRENDERER_TYPE_UNDEFINED;
	uint32_t l_uiReturn = m_pListRenderers->GetSelection();

	if ( l_uiReturn >= 0 && l_uiReturn < m_pListRenderers->GetCount() )
	{
		l_eReturn = static_cast< RendererPlugin * >( m_pListRenderers->GetClientData( l_uiReturn ) )->GetRendererType();
	}

	return l_eReturn;
}

void wxRendererSelector::DoDraw( wxDC * p_pDC )
{
	p_pDC->DrawBitmap( *m_pImgCastor, wxPoint( 0, 0 ), true );
}

void wxRendererSelector::DoSelect()
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

BEGIN_EVENT_TABLE( wxRendererSelector, wxDialog )
	EVT_PAINT(	wxRendererSelector::OnPaint	)
	EVT_KEY_UP(	wxRendererSelector::OnKeyUp	)
	EVT_BUTTON(	wxID_OK,			wxRendererSelector::OnButtonOk	)
	EVT_BUTTON(	wxID_CANCEL,		wxRendererSelector::OnButtonCancel	)
	EVT_LISTBOX_DCLICK( eID_LIST_RENDERERS,	wxRendererSelector::OnButtonOk	)
END_EVENT_TABLE()

void wxRendererSelector::OnPaint( wxPaintEvent & p_event )
{
	wxPaintDC l_paintDC( this );
	DoDraw( & l_paintDC );
	p_event.Skip();
}

void wxRendererSelector::OnKeyUp( wxKeyEvent & p_event )
{
	switch ( p_event.GetKeyCode() )
	{
	case WXK_RETURN:
		DoSelect();
		break;
	}

	p_event.Skip();
}

void wxRendererSelector::OnButtonOk( wxCommandEvent & p_event )
{
	DoSelect();
	p_event.Skip();
}

void wxRendererSelector::OnButtonCancel( wxCommandEvent & p_event )
{
	EndModal( wxID_CANCEL );
	p_event.Skip();
}
