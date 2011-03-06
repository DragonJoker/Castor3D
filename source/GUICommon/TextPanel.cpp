#include "GuiCommon/PrecompiledHeader.h"

#include "GuiCommon/TextPanel.h"
#include "GuiCommon/TextCtrl.h"
#include "GuiCommon/TextLinesCtrl.h"

using namespace GuiCommon;
using namespace Castor3D;

TextPanel :: TextPanel( wxWindow * parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
					    long style, bool p_showLines)
	:	wxScrolledWindow( parent, id, pos, size, style),
		m_showLines( p_showLines)
{
	m_container = new wxPanel( this, wxID_ANY, wxPoint( 0, 0), wxSize( size.x * 2, size.y));

	int x = m_container->GetClientSize().x - 40;
	int y = m_container->GetClientSize().y;

	if (p_showLines)
	{
		m_text = new TextCtrl( m_container, wxID_ANY, wxEmptyString, wxPoint( 40, 0), wxSize( x, y),
							   wxTE_MULTILINE | wxTE_RICH | wxTE_PROCESS_TAB);
	}
	else
	{
		m_text = new TextCtrl( m_container, wxID_ANY, wxEmptyString, wxPoint( 0, 0), wxSize( m_container->GetClientSize().x, y),
							   wxTE_MULTILINE | wxTE_RICH | wxTE_PROCESS_TAB);
	}

	m_text->SetFont( wxFont( 10, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, CU_T( "Lucida Console")));

	UpdateText();
}

void TextPanel :: WriteText( const wxString & p_text)
{
	m_text->WriteText( p_text);
}

void TextPanel :: LoadFile( const wxString & p_path, ScriptCompiler * p_pCompiler)
{
	m_strFileName = p_path;
	m_text->LoadFile( m_strFileName);
	UpdateText();
	ParseFile( p_pCompiler);
}

void TextPanel :: SaveFile( const wxString & p_path, ScriptCompiler * p_pCompiler)
{
	wxString l_strPath = p_path;

	if (l_strPath.empty())
	{
		wxFileDialog l_dialog( NULL, CU_T( "Choisissez un fichier"), wxEmptyString, wxEmptyString, CU_T( "GLSL File (*.glsl)|*.glsl"), wxFD_SAVE);

		if (l_dialog.ShowModal() == wxOK)
		{
			l_strPath = l_dialog.GetPath();
		}
		else
		{
			return;
		}
	}

	m_strFileName = l_strPath;
	m_text->SaveFile( m_strFileName);
	ParseFile( p_pCompiler);
}

void TextPanel :: ParseFile( ScriptCompiler * p_pCompiler)
{
	if (p_pCompiler != NULL)
	{
//		p_pCompiler->CompileScriptFile( m_strFileName.c_str());
	}
}

void TextPanel :: UpdateSize()
{
	wxSize l_size( m_text->GetPosition().x + m_text->GetSize().x,
				  m_text->GetNumberOfLines() * m_text->GetCharHeight());
	m_container->SetSize( l_size);

	UpdateScrollBars();
}

void TextPanel :: UpdateText()
{
	int x = m_container->GetClientSize().x - 40;
	int y = m_container->GetClientSize().y;

	m_text->SetLinesNumber( m_text->GetNumberOfLines());

	m_text->SetSize( x * 2, (m_text->GetNumberOfLines()) * m_text->GetCharHeight());

	m_lines = new TextLinesCtrl( m_container, wxID_ANY, wxPoint( 0, 0), wxSize( 40, 0),
								 wxLC_LIST | wxLC_SINGLE_SEL | wxLC_ALIGN_LEFT);
//	m_lines->Enable( false);
	m_lines->AddLines( m_text->GetNumberOfLines());

	for (int i = 0 ; i <= m_text->GetNumberOfLines() ; i++)
	{
		m_lines->LineUp();
	}

	m_text->SetLines( m_lines);

	UpdateSize();
}

void TextPanel :: UpdateScrollBars()
{
	int l_hScrollPos = GetScrollPos( wxHORIZONTAL);
	int l_vScrollPos = GetScrollPos( wxVERTICAL);
	int l_hRange = (m_text->GetPosition().x + m_text->GetSize().x) / m_text->GetCharWidth();
	int l_vRange = m_text->GetNumberOfLines();
	int l_hThumbSize = GetClientSize().x / m_text->GetCharWidth();
	int l_vThumbSize = GetClientSize().y / m_text->GetCharHeight();

	SetScrollbars( m_text->GetCharWidth(), m_text->GetCharHeight(),
				   m_container->GetSize().x / m_text->GetCharWidth(), m_container->GetSize().y / m_text->GetCharHeight(),
				   GetScrollPos( wxHORIZONTAL), GetScrollPos( wxVERTICAL));
}

bool TextPanel :: IsModified()
{
	return m_text->IsModified();
}

void TextPanel :: GoToLine( int p_line)
{
	ScrollLines( p_line - GetScrollPos( wxVERTICAL) - 10);
	m_lines->GoToLine( p_line);
	m_text->GoToLine( p_line);
}

bool TextPanel :: Search( const wxString & p_search)
{
	wxString l_txt = m_text->GetValue();

	m_search = p_search;
	m_currentSearchPosition = 0;
	int l_searchIndex = l_txt.Find( m_search);
	vector::deleteAll( m_searchPositions);

	if (l_searchIndex != wxNOT_FOUND)
	{
//		std::cout << "TextPanel :: Search - " << l_index << "\n";
		int l_currentLine = 1;
		long l_currentPosition = 0;

		while (l_currentPosition < l_searchIndex)
		{
//			std::cout << "TextPanel :: Search - " << (int)l_txt.at( l_currentPosition) << "{" << l_txt.at( l_currentPosition) << "}\n";
			if (l_txt.at( l_currentPosition) == '\n')
			{
				l_currentLine++;
			}
			l_currentPosition++;
		}
		l_currentPosition += l_currentLine - 1;
		m_searchPositions.push_back( new SearchInfo( l_currentLine, l_currentPosition, l_searchIndex));

//		std::cout << "TextPanel :: Search - Index : " << m_searchIndex << " - Line : " << l_currentLine << " - Cursor : " << l_currentPosition << " To : " << l_currentPosition + p_search.size() << "\n";
		m_text->SetSelection( l_currentPosition, l_currentPosition + p_search.size());
		m_text->SetFocus();

		ScrollLines( l_currentLine - GetScrollPos( wxVERTICAL) - 10);

		return true;
	}
	return false;
}

bool TextPanel :: SearchNext()
{
	if (m_searchPositions.size() > 0)
	{
		SearchInfo * l_info = m_searchPositions[m_currentSearchPosition];

		wxString l_txt = m_text->GetValue();
		l_txt = l_txt.Mid( l_info->m_index + m_search.size());
		int l_index = l_txt.Find( m_search);
		l_txt = m_text->GetValue();

		if (l_index != wxNOT_FOUND)
		{
	//		std::cout << "TextPanel :: Search - " << l_index << "\n";
			int l_currentLine = l_info->m_line;
			long l_currentPosition = l_info->m_position - l_currentLine + 1;
			int l_searchIndex = l_info->m_index + m_search.size() + l_index;

			while (l_currentPosition < l_searchIndex)
			{
	//			std::cout << "TextPanel :: Search - " << (int)l_txt.at( l_currentPosition) << "{" << l_txt.at( l_currentPosition) << "}\n";
				if (l_txt.at( l_currentPosition) == '\n')
				{
					l_currentLine++;
				}
				l_currentPosition++;
			}
			l_currentPosition += l_currentLine - 1;
			m_currentSearchPosition = m_searchPositions.size();
			m_searchPositions.push_back( new SearchInfo( l_currentLine, l_currentPosition, l_searchIndex));

	//		std::cout << "TextPanel :: Search - Index : " << m_searchIndex << " - Line : " << l_currentLine << " - Cursor : " << l_currentPosition << " To : " << l_currentPosition + p_search.size() << "\n";
			m_text->SetSelection( l_currentPosition, l_currentPosition + m_search.size());
			m_text->SetFocus();

			ScrollLines( l_currentLine - GetScrollPos( wxVERTICAL) - 10);

			return true;
		}
	}
	return false;
}

bool TextPanel :: SearchPrevious()
{
	if (m_searchPositions.size() > 0 && m_currentSearchPosition > 0)
	{
		delete m_searchPositions[m_currentSearchPosition];
		vector::eraseValue( m_searchPositions, m_searchPositions[m_currentSearchPosition]);
		m_currentSearchPosition--;

		SearchInfo * l_info = m_searchPositions[m_currentSearchPosition];

		m_text->SetSelection( l_info->m_position, l_info->m_position + m_search.size());
		m_text->SetFocus();

		ScrollLines( l_info->m_line - GetScrollPos( wxVERTICAL) - 10);

		return true;
	}
	return false;
}

void TextPanel :: replace( const wxString & p_replacement)
{
	if (m_searchPositions.size() == 0)
	{
		return;
	}

	SearchInfo * l_info = m_searchPositions[m_currentSearchPosition];

	m_text->Replace( l_info->m_position, l_info->m_position + m_search.size(), p_replacement);
}

BEGIN_EVENT_TABLE( TextPanel, wxScrolledWindow)
	EVT_SIZE(	TextPanel::_onResize)
END_EVENT_TABLE()

void TextPanel :: _onResize( wxSizeEvent & p_event)
{
	SetScrollbars( m_text->GetCharWidth(), m_text->GetCharHeight(),
				   m_container->GetSize().x / m_text->GetCharWidth(), m_container->GetSize().y / m_text->GetCharHeight(),
				   GetScrollPos( wxHORIZONTAL), GetScrollPos( wxVERTICAL));
}

void TextPanel :: _onSetFocus( wxFocusEvent & p_event)
{
	m_text->SetFocus();
}
