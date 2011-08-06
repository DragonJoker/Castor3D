#include "GuiCommon/PrecompiledHeader.hpp"

#include "GuiCommon/RendererSelector.hpp"
#include "GuiCommon/ImagesLoader.hpp"

using namespace GuiCommon;
using namespace Castor3D;

wxRendererSelector :: wxRendererSelector( wxWindow * p_pParent, const wxString & p_strTitle)
	:	wxDialog( p_pParent, wxID_ANY, p_strTitle + wxT( " - Select renderer"), wxDefaultPosition, wxSize( 500, 500), wxDEFAULT_DIALOG_STYLE)
	,	m_pBmpCastor( ImagesLoader::GetBitmap( CV_IMG_CASTOR))
{
	wxSize l_size = GetClientSize();
	SetBackgroundColour( wxColour( 128, 128, 128));
	wxStaticText * l_pTitle = new wxStaticText( this, wxID_ANY, p_strTitle, wxPoint( 180, 0), wxDefaultSize, wxALIGN_CENTRE);
	wxFont l_font( 30, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT( "Arial"));
	l_pTitle->SetFont( l_font);
	wxStaticText * l_pStatic = new wxStaticText( this, wxID_ANY, wxT( "Select your renderer in the list below"), wxPoint( 180, 70));
	m_pListRenderers = new wxListBox( this, eID_LIST_RENDERERS, wxPoint( 10, 160), wxSize( l_size.x - 20, 240));
	m_pBtnOk = new wxButton( this, eID_BUTTON_OK, wxT( "OK"), wxPoint( 10, l_size.y - 40), wxSize( 100, 30));
	m_pBtnCancel = new wxButton( this, eID_BUTTON_CANCEL, wxT( "Cancel"), wxPoint( l_size.x - 110, l_size.y - 40), wxSize( 100, 30));
	int l_iCount = 0;

	for (Root::RendererPtrArray::iterator l_it = Root::GetSingleton()->RenderersBegin() ; l_it != Root::GetSingleton()->RenderersEnd() ; ++l_it)
	{
		if ( * l_it)
		{
			m_pListRenderers->Insert( ( * l_it)->GetName(), l_iCount++, ( * l_it).get());
		}
	}

	if (m_pListRenderers->GetCount() > 0)
	{
		m_pListRenderers->Select( 0);
	}

	wxClientDC l_clientDC( this);
	_draw( & l_clientDC);
}

wxRendererSelector :: ~wxRendererSelector()
{
}

void wxRendererSelector :: _draw( wxDC * p_pDC)
{
	p_pDC->DrawBitmap( * m_pBmpCastor, wxPoint( 0, 0), true);
}

BEGIN_EVENT_TABLE( wxRendererSelector, wxDialog)
	EVT_PAINT(								wxRendererSelector::_onPaint)
	EVT_BUTTON(			eID_BUTTON_OK,		wxRendererSelector::_onButtonOk)
	EVT_BUTTON(			eID_BUTTON_CANCEL,	wxRendererSelector::_onButtonCancel)
	EVT_LISTBOX_DCLICK( eID_LIST_RENDERERS,	wxRendererSelector::_onButtonOk)
END_EVENT_TABLE()

void wxRendererSelector :: _onPaint( wxPaintEvent & p_event)
{
	wxPaintDC l_paintDC( this);
	_draw( & l_paintDC);
	p_event.Skip();
}

void wxRendererSelector :: _onButtonOk( wxCommandEvent & p_event)
{
	if (m_pListRenderers->GetCount() > 0)
	{
		unsigned int l_uiReturn = m_pListRenderers->GetSelection();
		EndModal( static_cast<RendererPlugin *>( m_pListRenderers->GetClientData( l_uiReturn))->GetRendererType());
	}
	else
	{
		EndModal( wxID_CANCEL);
	}
}

void wxRendererSelector :: _onButtonCancel( wxCommandEvent & p_event)
{
	EndModal( wxID_CANCEL);
}
