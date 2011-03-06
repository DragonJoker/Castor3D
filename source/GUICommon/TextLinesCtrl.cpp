#include "GuiCommon/PrecompiledHeader.h"

#include "GuiCommon/TextLinesCtrl.h"

using namespace GuiCommon;
using namespace Castor3D;

TextLinesCtrl :: TextLinesCtrl( wxWindow * parent, wxWindowID id, const wxPoint& pos,
							    const wxSize& size, long style)
	:	wxPanel( parent, id, pos, size, style),
		m_nbLines( 0)
{
	m_lines = new wxListBox( this, wxID_ANY, wxPoint( 0, 0), GetClientSize(), 0, NULL,
							 wxBORDER_NONE | wxLB_SINGLE | wxLB_NEEDED_SB);
	m_lines->SetFont( wxFont( 10, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, CU_T( "Lucida Console")));
}

void TextLinesCtrl :: AddLines( unsigned int p_nbLines)
{
	wxString l_caption;
	int l_deltaY = 0;
	for (unsigned int i = 0 ; i < p_nbLines ; i++)
	{
		l_caption << ++m_nbLines;
		m_lines->InsertItems( 1, & l_caption, m_lines->GetCount());
		l_caption.clear();
		l_deltaY += GetCharHeight();
	}
	SetSize( GetSize() + wxSize( 0, l_deltaY));
	m_lines->SetSize( GetClientSize());
}

void TextLinesCtrl :: RemoveLine()
{
	if (m_nbLines > 0)
	{
		m_nbLines--;
		m_lines->Delete( m_lines->GetCount() - 1);
		SetSize( GetSize() - wxSize( 0, GetCharHeight()));
		m_lines->SetSize( GetClientSize());
	}
}

void TextLinesCtrl :: GoToLine( int p_line)
{
	if (p_line <= static_cast <int>( m_lines->GetCount()))
	{
		m_lines->SetSelection( p_line - 1);
	}
}

BEGIN_EVENT_TABLE( TextLinesCtrl, wxPanel)
	EVT_LEFT_DCLICK(	TextLinesCtrl::_onDoubleClick)
END_EVENT_TABLE()

void TextLinesCtrl :: _onDoubleClick( wxMouseEvent & p_event)
{
	p_event.Skip();
}
