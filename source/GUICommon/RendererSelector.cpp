#include "GuiCommon/PrecompiledHeader.h"

#include "GuiCommon/RendererSelector.h"
#include "GuiCommon/ImagesLoader.h"

using namespace GuiCommon;
using namespace Castor3D;

RendererSelector :: RendererSelector( wxWindow * p_pParent, const wxString & p_strTitle)
	:	wxDialog( p_pParent, wxID_ANY, p_strTitle + wxT( " - Select renderer"), wxDefaultPosition, wxSize( 500, 500))
	,	m_pBmpCastor( ImagesLoader::GetBitmap( CV_IMG_CASTOR))
{
	wxArrayString l_arrayStrings;
#ifdef _WIN32
	l_arrayStrings.push_back( wxT( "Direct3D 9 Renderer"));
#endif
	l_arrayStrings.push_back( wxT( "OpenGL 3.x Compatible Renderer"));
	l_arrayStrings.push_back( wxT( "OpenGL 2.x Compatible Renderer"));

	wxSize l_size = GetClientSize();
	m_pPanel = new wxPanel( this, wxID_ANY, wxPoint( 0, 0), l_size);
	m_pPanel->SetBackgroundColour( wxColour( 128, 128, 128));
	wxStaticText * l_pTitle = new wxStaticText( m_pPanel, wxID_ANY, p_strTitle, wxPoint( 180, 0), wxDefaultSize, wxALIGN_CENTRE);
	wxFont l_font( 30, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, "Arial");
	l_pTitle->SetFont( l_font);
	wxStaticText * l_pStatic = new wxStaticText( m_pPanel, wxID_ANY, wxT( "Select your renderer in the list below"), wxPoint( 180, 70));
	m_pListRenderers = new wxListBox( m_pPanel, eList, wxPoint( 10, 160), wxSize( l_size.x - 20, l_size.y - 210), l_arrayStrings);
	m_pBtnOk = new wxButton( m_pPanel, eOk, wxT( "OK"), wxPoint( 10, 435), wxSize( 100, 30));
	m_pBtnCancel = new wxButton( m_pPanel, eCancel, wxT( "Cancel"), wxPoint( l_size.x - 110, 435), wxSize( 100, 30));
	m_pListRenderers->Select( 0);
	wxClientDC l_clientDC( m_pPanel);
	_draw( & l_clientDC);
}

RendererSelector :: ~RendererSelector()
{
}

void RendererSelector :: _draw( wxDC * p_pDC)
{
	p_pDC->DrawBitmap( * m_pBmpCastor, wxPoint( 0, 0), true);
}

BEGIN_EVENT_TABLE( RendererSelector, wxDialog)
	EVT_PAINT(				RendererSelector::_onPaint)
	EVT_BUTTON( eOk,		RendererSelector::_onButtonOk)
	EVT_BUTTON( eCancel,	RendererSelector::_onButtonCancel)
END_EVENT_TABLE()

void RendererSelector :: _onPaint( wxPaintEvent & p_event)
{
	wxPaintDC l_paintDC( this);
	wxClientDC l_clientDC( m_pPanel);
	_draw( & l_clientDC);
	p_event.Skip();
}

void RendererSelector :: _onButtonOk( wxCommandEvent & p_event)
{
	int l_iReturn = m_pListRenderers->GetSelection();

#ifdef _WIN32
	switch (l_iReturn)
	{
	case 0:
		EndModal( RendererDriver::eDirect3D9);
		break;

	case 1:
		EndModal( RendererDriver::eOpenGl3);
		break;

	case 2:
		EndModal( RendererDriver::eOpenGl2);
		break;
	}
#else
	switch (l_iReturn)
	{
	case 0:
		EndModal( RendererDriver::eOpenGl3);
		break;

	case 1:
		EndModal( RendererDriver::eOpenGl2);
		break;
	}
#endif
}

void RendererSelector :: _onButtonCancel( wxCommandEvent & p_event)
{
	EndModal( wxID_CANCEL);
}