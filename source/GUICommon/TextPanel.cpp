#include "GuiCommon/PrecompiledHeader.hpp"

#include "GuiCommon/TextPanel.hpp"
#include "GuiCommon/TextEditor.hpp"
#include "GuiCommon/TextLinesCtrl.hpp"
#include "GuiCommon/SyntaxHighlighter.hpp"

using namespace GuiCommon;
using namespace Castor3D;

const int wxTextPanel::LinesWidth = 40;

wxTextPanel :: wxTextPanel( wxWindow * p_pParent, wxWindowID p_id, const wxPoint & p_ptPos, const wxSize & p_size, long p_style, bool p_showLines)
	:	wxScrolledWindow( p_pParent, p_id, p_ptPos, p_size, p_style)
	,	m_showLines( p_showLines)
	,	m_lines( nullptr)
{
	m_container = new wxPanel( this, wxID_ANY, wxPoint( 0, 0), wxSize( p_size.x * 2, p_size.y));

	wxSize l_sizeEditor( m_container->GetClientSize().x - LinesWidth, m_container->GetClientSize().y);
	long l_lFlags = wxTE_MULTILINE | wxTE_RICH | wxTE_PROCESS_TAB | wxBORDER_NONE;

	if (p_showLines)
	{
		m_text = new wxSyntaxHighlighter<wxTextEditor>( m_container, wxID_ANY, wxEmptyString, wxPoint( LinesWidth, 0), l_sizeEditor, l_lFlags);
	}
	else
	{
		m_text = new wxSyntaxHighlighter<wxTextEditor>( m_container, wxID_ANY, wxEmptyString, wxPoint( 0, 0), wxSize( m_container->GetClientSize().x, l_sizeEditor.y), l_lFlags);
	}

	m_text->SetTextPanel( this);
	m_text->SetFont( wxFont( 10, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT( "Lucida Console")));

	UpdateText();
}

void wxTextPanel :: WriteText( const wxString & p_text)
{
	m_text->WriteText( p_text);
}

void wxTextPanel :: LoadFile( const wxString & p_path, ScriptCompiler * p_pCompiler)
{
	m_strFileName = p_path;
	m_text->LoadFile( m_strFileName);
	UpdateText();
	ParseFile( p_pCompiler);
}

void wxTextPanel :: SaveFile( const wxString & p_path, ScriptCompiler * p_pCompiler)
{
	wxString l_strPath = p_path;

	if (l_strPath.empty())
	{
		wxFileDialog l_dialog( nullptr, wxT( "Choose a file"), wxEmptyString, wxEmptyString, wxT( "GLSL File (*.glsl)|*.glsl"), wxFD_SAVE);

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

void wxTextPanel :: ParseFile( ScriptCompiler * p_pCompiler)
{
	if (p_pCompiler)
	{
//		p_pCompiler->CompileScriptFile( m_strFileName.c_str());
	}
}

void wxTextPanel :: UpdateSize( const wxSize & p_size)
{
	wxSize l_size( m_text->GetPosition().x + p_size.x, p_size.y);
	m_container->SetSize( l_size);
	m_text->SetSize( p_size);
	m_lines->SetSize( wxSize( LinesWidth, p_size.y));

	UpdateScrollBars();
}

void wxTextPanel :: UpdateText()
{
	int x = m_container->GetClientSize().x - LinesWidth;
	int y = m_container->GetClientSize().y;
	int l_iLinesCount = m_text->GetNumberOfLines();

	m_text->SetLinesNumber( l_iLinesCount);
	m_text->SetSize( x * 2, l_iLinesCount * m_text->GetCharHeight());

	if (m_lines == NULL)
	{
		m_lines = new wxTextLinesCtrl( m_container, wxID_ANY, wxPoint( 0, 0), wxSize( LinesWidth, 0), wxLC_LIST | wxLC_SINGLE_SEL | wxLC_ALIGN_LEFT);
		m_text->SetLines( m_lines);
	}

	m_lines->AddLines( l_iLinesCount);

	for (int i = 0 ; i <= l_iLinesCount ; i++)
	{
		m_lines->LineUp();
	}

	UpdateSize( m_text->GetSize());
}

void wxTextPanel :: UpdateScrollBars()
{
	int l_hScrollPos = GetScrollPos( wxHORIZONTAL);
	int l_vScrollPos = GetScrollPos( wxVERTICAL);
	int l_iCharWidth = m_text->GetCharWidth();
	int l_iCharHeight = m_text->GetCharHeight();
	int l_hRange = (m_text->GetPosition().x + m_text->GetSize().x) / m_text->GetCharWidth();
	int l_vRange = m_text->GetNumberOfLines();
	int l_hThumbSize = GetClientSize().x / l_iCharWidth;
	int l_vThumbSize = GetClientSize().y / l_iCharHeight;

	SetScrollbars( l_iCharWidth, l_iCharHeight,
				   m_container->GetSize().x / l_iCharWidth, m_container->GetSize().y / l_iCharHeight,
				   l_hScrollPos, l_vScrollPos);
}

bool wxTextPanel :: IsModified()
{
	return m_text->IsModified();
}

void wxTextPanel :: GoToLine( int p_line)
{
	ScrollLines( p_line - GetScrollPos( wxVERTICAL) - 10);
	m_lines->GoToLine( p_line);
	m_text->GoToLine( p_line);
}

bool wxTextPanel :: Search( const wxString & p_search)
{
	wxString l_txt = m_text->GetValue();

	m_search = p_search;
	m_currentSearchPosition = 0;
	int l_searchIndex = l_txt.Find( m_search);
	vector::deleteAll( m_searchPositions);

	if (l_searchIndex != wxNOT_FOUND)
	{
//		std::cout << "wxTextPanel :: Search - " << l_index << "\n";
		int l_currentLine = 1;
		long l_currentPosition = 0;

		while (l_currentPosition < l_searchIndex)
		{
//			std::cout << "wxTextPanel :: Search - " << (int)l_txt.at( l_currentPosition) << "{" << l_txt.at( l_currentPosition) << "}\n";
			if (l_txt.at( l_currentPosition) == wxT( '\n'))
			{
				l_currentLine++;
			}
			l_currentPosition++;
		}
		l_currentPosition += l_currentLine - 1;
		m_searchPositions.push_back( new SearchInfo( l_currentLine, l_currentPosition, l_searchIndex));

//		std::cout << "wxTextPanel :: Search - Index : " << m_searchIndex << " - Line : " << l_currentLine << " - Cursor : " << l_currentPosition << " To : " << l_currentPosition + p_search.size() << "\n";
		m_text->SetSelection( l_currentPosition, l_currentPosition + p_search.size());
		m_text->SetFocus();

		ScrollLines( l_currentLine - GetScrollPos( wxVERTICAL) - 10);

		return true;
	}
	return false;
}

bool wxTextPanel :: SearchNext()
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
	//		std::cout << "wxTextPanel :: Search - " << l_index << "\n";
			int l_currentLine = l_info->m_line;
			long l_currentPosition = l_info->m_position - l_currentLine + 1;
			int l_searchIndex = l_info->m_index + m_search.size() + l_index;

			while (l_currentPosition < l_searchIndex)
			{
	//			std::cout << "wxTextPanel :: Search - " << (int)l_txt.at( l_currentPosition) << "{" << l_txt.at( l_currentPosition) << "}\n";
				if (l_txt.at( l_currentPosition) == wxT( '\n'))
				{
					l_currentLine++;
				}
				l_currentPosition++;
			}
			l_currentPosition += l_currentLine - 1;
			m_currentSearchPosition = m_searchPositions.size();
			m_searchPositions.push_back( new SearchInfo( l_currentLine, l_currentPosition, l_searchIndex));

	//		std::cout << "wxTextPanel :: Search - Index : " << m_searchIndex << " - Line : " << l_currentLine << " - Cursor : " << l_currentPosition << " To : " << l_currentPosition + p_search.size() << "\n";
			m_text->SetSelection( l_currentPosition, l_currentPosition + m_search.size());
			m_text->SetFocus();

			ScrollLines( l_currentLine - GetScrollPos( wxVERTICAL) - 10);

			return true;
		}
	}
	return false;
}

bool wxTextPanel :: SearchPrevious()
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

void wxTextPanel :: replace( const wxString & p_replacement)
{
	if (m_searchPositions.size() == 0)
	{
		return;
	}

	SearchInfo * l_info = m_searchPositions[m_currentSearchPosition];

	m_text->Replace( l_info->m_position, l_info->m_position + m_search.size(), p_replacement);
}

BEGIN_EVENT_TABLE( wxTextPanel, wxScrolledWindow)
	EVT_SIZE(	wxTextPanel::_onResize)
END_EVENT_TABLE()

void wxTextPanel :: _onResize( wxSizeEvent & p_event)
{
	SetScrollbars( m_text->GetCharWidth(), m_text->GetCharHeight(),
				   m_container->GetSize().x / m_text->GetCharWidth(), m_container->GetSize().y / m_text->GetCharHeight(),
				   GetScrollPos( wxHORIZONTAL), GetScrollPos( wxVERTICAL));
}

void wxTextPanel :: _onSetFocus( wxFocusEvent & p_event)
{
	m_text->SetFocus();
}
