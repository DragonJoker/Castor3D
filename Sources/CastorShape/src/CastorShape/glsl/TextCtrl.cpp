#include "PrecompiledHeaders.h"

#include "glsl/TextCtrl.h"
#include "glsl/TextPanel.h"
#include "glsl/TextLinesCtrl.h"

using namespace CastorShape;
using namespace Castor3D;

TextCtrl :: TextCtrl( wxWindow * parent, wxWindowID id, const wxString& value, 
					  const wxPoint& pos, const wxSize& size, long style)
	:	wxTextCtrl( parent, id, value, pos, size, style),
		m_posDeb		( 0),
		m_posFin		( 0),
		m_linesNumber	( 0),
		m_scrollByWheel	( false),
		m_difference	( 0),
		m_precPosition	( 0),
		m_modified		( false)
{
	m_panel = reinterpret_cast <TextPanel *>( GetParent()->GetParent());
}

TextCtrl :: ~TextCtrl()
{
//	std::cout << "TextCtrl :: ~TextCtrl - " << m_file << " - Modified : " << m_modified << " - Saved : " << m_file->Saved << "\n";
	if (m_modified)
	{
		if (wxMessageBox( CU_T( "Enregistrer les modifications?"), m_strFileName, wxYES_NO, this) == wxYES)
		{
			if (m_strFileName.empty())
			{
				wxFileDialog l_dialog( NULL, CU_T( "Choisissez un fichier"), wxEmptyString, wxEmptyString, CU_T( "GLSL File (*.glsl)|*.glsl"), wxFD_SAVE);

				if (l_dialog.ShowModal() == wxOK)
				{
					m_strFileName = l_dialog.GetPath();
				}
				else
				{
					return;
				}
			}

			wxTextCtrl::SaveFile( m_strFileName);
		}
	}
}

void TextCtrl :: AutoWord()
{
}

void TextCtrl :: _onMouseWheel( wxMouseEvent & p_event)
{
	m_scrollByWheel = true;

	if (p_event.GetWheelRotation() < 0)
	{
		reinterpret_cast <TextPanel *>( GetParent()->GetParent())->ScrollLines( 3);
	}
	else
	{
		reinterpret_cast <TextPanel *>( GetParent()->GetParent())->ScrollLines( -3);
	}

	m_scrollByWheel = false;
}

void TextCtrl :: SaveFile( const wxString & p_path, int p_fileType)
{
	m_modified = false;
	m_strFileName = p_path;
	wxTextCtrl::SaveFile( p_path, p_fileType);
}

bool TextCtrl :: LoadFile( const wxString & p_filename, int p_fileType)
{
	m_modified = false;
	m_strFileName = p_filename;
	return wxTextCtrl::LoadFile( m_strFileName, p_fileType);
}

void TextCtrl :: GoToLine( int p_line)
{
	int l_currentLine = 1;
	SetInsertionPoint( 0);
	wxString l_txt = GetValue();
	long l_currentPosition = 0;
	while (l_currentLine < p_line)
	{
		while (l_currentPosition < GetLastPosition() && l_txt.at( l_currentPosition) != '\n')
		{
			l_currentPosition++;
		}
		if (l_currentPosition < GetLastPosition())
		{
			l_currentLine++;
		}
		else
		{
			break;
		}
	}
	if (l_currentPosition < GetLastPosition())
	{
//		std::cout << "TextCtrl :: GoToLine - Wanted : " << p_line << " - Retrieved : " << l_currentLine << " - Cursor : " << l_currentPosition << "\n";
		SetInsertionPoint( l_currentPosition);
	}
}

void TextCtrl :: CompleteCode( int p_keyCode)
{
}

BEGIN_EVENT_TABLE( TextCtrl, wxTextCtrl)
	EVT_KEY_UP(					TextCtrl::_onKeyUp)
	EVT_KEY_DOWN(				TextCtrl::_onKeyDown)
//	EVT_CHAR(					TextCtrl::_onEventText)
	EVT_MOUSEWHEEL(				TextCtrl::_onMouseWheel)
	EVT_SCROLLWIN_TOP(			TextCtrl::_onScrollToTop)
	EVT_SCROLLWIN_BOTTOM(		TextCtrl::_onScrollToBottom)
	EVT_SCROLLWIN_LINEUP(		TextCtrl::_onScrollLineUp)
	EVT_SCROLLWIN_LINEDOWN(		TextCtrl::_onScrollLineDown)
	EVT_SCROLLWIN_PAGEUP(		TextCtrl::_onScrollPageUp)
	EVT_SCROLLWIN_PAGEDOWN(		TextCtrl::_onScrollPageDown)
	EVT_SCROLLWIN_THUMBTRACK(	TextCtrl::_onScrollThumbTrack)
	EVT_SCROLLWIN_THUMBRELEASE(	TextCtrl::_onScrollThumbRelease)
END_EVENT_TABLE()

void TextCtrl :: _onKeyDown( wxKeyEvent& p_event)
{
	switch (p_event.GetKeyCode())
	{
		case WXK_PAGEUP :
			p_event.Skip();
			PageUp();
			m_lines->PageUp();
			m_panel->PageUp();
		break;
		case WXK_PAGEDOWN :
			p_event.Skip();
			PageDown();
			m_lines->PageDown();
			m_panel->PageDown();
		break;
		case WXK_UP :
			p_event.Skip();
			LineUp();
			m_lines->LineUp();
			m_panel->LineUp();
		break;
		case WXK_DOWN :
			p_event.Skip();
			LineDown();
			m_lines->LineDown();
			m_panel->LineDown();
		break;
		case WXK_F1 :
		case WXK_F2 :
		case WXK_F3 :
		case WXK_F4 :
		case WXK_F5 :
		case WXK_F6 :
		case WXK_F7 :
		case WXK_F8 :
		case WXK_F9 :
		case WXK_F10 :
		case WXK_F11 :
		case WXK_F12 :
		case WXK_F13 :
		case WXK_F14 :
		case WXK_F15 :
		case WXK_F16 :
		case WXK_F17 :
		case WXK_F18 :
		case WXK_F19 :
		case WXK_F20 :
		case WXK_F21 :
		case WXK_F22 :
		case WXK_F23 :
		case WXK_F24 :
		case WXK_NUMLOCK :
		case WXK_SCROLL :
		case WXK_SHIFT:
		case WXK_ALT:
		case WXK_CONTROL:
		case WXK_MENU:
		case WXK_PAUSE:
		case WXK_CAPITAL:
		case WXK_END:
		case WXK_HOME:
		case WXK_LEFT:
		case WXK_RIGHT:
		case WXK_SELECT:
		case WXK_PRINT:
		case WXK_EXECUTE:
		case WXK_SNAPSHOT:
		case WXK_INSERT:
			p_event.Skip();
		break;
		case WXK_TAB:
			long l_begin, l_end;
			GetSelection( & l_begin, & l_end);
			if (l_begin == l_end)
			{
				p_event.Skip();
			}
			else
			{
				long l_column, l_lineBegin, l_lineEnd;

				PositionToXY( l_begin, & l_column, & l_lineBegin);
				PositionToXY( l_end, & l_column, & l_lineEnd);

				wxString l_lineTxt;
				long l_position;
				bool l_beginModified = false;
				for (long i = l_lineBegin ; i <= l_lineEnd ; i++)
				{
					l_lineTxt = GetLineText( i);
					if ( ! p_event.ShiftDown())
					{
						l_lineTxt = CU_T( "\t") + l_lineTxt;
						l_end++;
						if ( ! l_beginModified)
						{
							l_begin++;
							l_beginModified = true;
						}
					}
					else
					{
						if (l_lineTxt.at( 0) == '\t')
						{
							l_lineTxt = l_lineTxt.Mid( 1);
							l_end--;
							if ( ! l_beginModified)
							{
								l_begin--;
								l_beginModified = true;
							}
						}
					}
					l_position = XYToPosition( 0, i);
					Replace( l_position, l_position + GetLineLength( i), l_lineTxt);
				}
				SetSelection( l_begin, l_end);
			}
			m_modified = true;
		break;
		default:
			p_event.Skip();
			m_modified = true;
	}
}

void TextCtrl :: _onKeyUp( wxKeyEvent & p_event)
{
	if ((p_event.GetKeyCode() == 's' || p_event.GetKeyCode() == 'S') && p_event.ControlDown())
	{
		if ( ! m_strFileName.empty())
		{
			SaveFile( m_strFileName);
		}
		else
		{
			wxString l_strPath;
			wxFileDialog l_dialog( NULL, CU_T( "Choisissez un fichier"), wxEmptyString, wxEmptyString, CU_T( "GLSL File (*.glsl)|*.glsl"), wxFD_SAVE);

			if (l_dialog.ShowModal() == wxOK)
			{
				m_strFileName = l_dialog.GetPath();
			}

			SaveFile( m_strFileName);
		}
	}

	p_event.Skip();

	CompleteCode( p_event.GetKeyCode());

	if (m_linesNumber != GetNumberOfLines())
	{
		SetSize( wxSize( GetSize().x, GetNumberOfLines() * GetCharHeight()));
		m_panel->UpdateSize();
		if (m_linesNumber < GetNumberOfLines())
		{
			m_lines->AddLines( GetNumberOfLines() - m_linesNumber);
			do
			{
				++m_linesNumber;
				m_panel->LineDown();
			}
			while (m_linesNumber < GetNumberOfLines());
		}
		else if (m_linesNumber > GetNumberOfLines())
		{
			do
			{
				--m_linesNumber;
				m_lines->RemoveLine();
			}
			while (m_linesNumber > GetNumberOfLines());
		}
	}
}

void TextCtrl :: _onScrollToTop( wxScrollWinEvent & p_event)
{
	p_event.Skip();

	if (m_scrollByWheel)
	{
		return;
	}

//	std::cout << "TextCtrl :: _onScrollToTop\n";
	m_lines->ScrollLines( -m_lines->GetScrollPos( wxVERTICAL));
	m_panel->ScrollLines( -m_panel->GetScrollPos( wxVERTICAL));
	m_precPosition = 0;
}

void TextCtrl :: _onScrollToBottom( wxScrollWinEvent & p_event)
{
	p_event.Skip();

	if (m_scrollByWheel)
	{
		return;
	}

//	std::cout << "TextCtrl :: _onScrollToBottom\n";
	m_lines->ScrollLines( m_lines->GetScrollRange( wxVERTICAL));
	m_panel->ScrollLines( m_panel->GetScrollRange( wxVERTICAL));
	m_precPosition = GetScrollRange( wxVERTICAL);
}

void TextCtrl :: _onScrollLineUp( wxScrollWinEvent & p_event)
{
	p_event.Skip();

	if (m_scrollByWheel)
	{
		return;
	}

//	std::cout << "TextCtrl :: _onScrollLineUp\n";
	m_lines->LineUp();
	m_precPosition = GetScrollPos( wxVERTICAL);
}

void TextCtrl :: _onScrollLineDown( wxScrollWinEvent & p_event)
{
	p_event.Skip();

	if (m_scrollByWheel)
	{
		return;
	}

//	std::cout << "TextCtrl :: _onScrollLineDown\n";
	m_lines->LineDown();
	m_precPosition = GetScrollPos( wxVERTICAL);
}

void TextCtrl :: _onScrollPageUp( wxScrollWinEvent & p_event)
{
	p_event.Skip();

	if (m_scrollByWheel)
	{
		return;
	}

//	std::cout << "TextCtrl :: _onScrollPageUp\n";
	m_lines->PageUp();
	m_precPosition = GetScrollPos( wxVERTICAL);
}

void TextCtrl :: _onScrollPageDown( wxScrollWinEvent & p_event)
{
	p_event.Skip();

	if (m_scrollByWheel)
	{
		return;
	}

//	std::cout << "TextCtrl :: _onScrollPageDown\n";
	m_lines->PageDown();
	m_precPosition = GetScrollPos( wxVERTICAL);
}

void TextCtrl :: _onScrollThumbTrack( wxScrollWinEvent & p_event)
{
	p_event.Skip();

	if (m_scrollByWheel)
	{
		return;
	}

	int l_currentPosition = p_event.GetPosition();
	m_difference += (l_currentPosition - m_precPosition);
	m_precPosition = l_currentPosition;
	real l_ratio = ((real)(GetScrollRange( wxVERTICAL) - GetScrollThumb( wxVERTICAL))) / ((real)GetNumberOfLines());
//	std::cout << "TextCtrl :: _onScrollThumbTrack - Difference : " << m_difference << "\n";
	while (abs( m_difference) >= l_ratio)
	{
		if (m_difference < 0)
		{
			m_lines->ScrollLines( -1);
			m_difference += l_ratio;
		}
		else
		{
			m_lines->ScrollLines( 1);
			m_difference -= l_ratio;
		}
	}
}

void TextCtrl :: _onScrollThumbRelease( wxScrollWinEvent & p_event)
{
	p_event.Skip();

	if (m_scrollByWheel)
	{
		return;
	}
}