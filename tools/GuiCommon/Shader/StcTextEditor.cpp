#include "GuiCommon/Shader/StcTextEditor.hpp"

#include "GuiCommon/Shader/LanguageInfo.hpp"
#include "GuiCommon/Shader/StyleInfo.hpp"

#include <wx/filename.h>

#include <algorithm>

using namespace GuiCommon;

//*************************************************************************************************

#if wxMAJOR_VERSION >= 3 || ( wxMAJOR_VERSION == 2 && wxMINOR_VERSION >= 9 )
StcTextEditor::TextAutoCompleter::TextAutoCompleter( wxArrayString const & p_keywords )
{
	for ( wxArrayString::const_iterator it = p_keywords.begin(); it != p_keywords.end(); ++it )
	{
		m_keywords.insert( *it );
	}
}

bool StcTextEditor::TextAutoCompleter::Start( wxString const & p_prefix )
{
	m_prefix = p_prefix;

	if ( m_prefix.size() >= 3 )
	{
		m_current = std::find_if( m_keywords.begin(), m_keywords.end(), [&]( wxString const & p_keyword )
		{
			return p_keyword.find( m_prefix ) == 0;
		} );
	}
	else
	{
		m_current = m_keywords.end();
	}

	return m_current != m_keywords.end();
}

wxString StcTextEditor::TextAutoCompleter::GetNext()
{
	wxString result;

	if ( m_current != m_keywords.end() )
	{
		++m_current;

		if ( m_current != m_keywords.end() )
		{
			if ( m_current->find( m_prefix ) == 0 )
			{
				result = *m_current;
			}
		}
	}

	return result;
}
#endif
//*************************************************************************************************

StcTextEditor::StcTextEditor( StcContext & context
	, wxWindow * parent
	, wxWindowID id
	, wxPoint const & pos
	, wxSize const & size
	, long style )
	: wxStyledTextCtrl( parent, id, pos, size, style )
	, m_context( context )
	, m_filename( wxEmptyString )
	, m_lineNrID( 0 )
	, m_dividerID( 1 )
	, m_foldingID( 2 )
	, m_language()
	, m_tabSpaces( 4 )
	, m_useTabs( true )
	, m_tabIndents( true )
	, m_backspaceUnindents( true )
{
	SetTabWidth( m_tabSpaces );

	SetViewEOL( m_context.displayEOLEnable );
	SetIndentationGuides( m_context.indentGuideEnable );
	SetEdgeMode( m_context.longLineOnEnable ? wxSTC_EDGE_LINE : wxSTC_EDGE_NONE );
	static const wxColour WHITESPACE_COLOUR = wxColour( 25, 76, 127, 255 );
	SetWhitespaceForeground( true, WHITESPACE_COLOUR );
	SetViewWhiteSpace( wxSTC_WS_VISIBLEALWAYS );
	SetOvertype( m_context.overTypeInitial );
	SetReadOnly( m_context.readOnlyInitial );
	SetWrapMode( m_context.wrapModeInitial ? wxSTC_WRAP_WORD : wxSTC_WRAP_NONE );
	// set visibility
	SetVisiblePolicy( wxSTC_VISIBLE_STRICT | wxSTC_VISIBLE_SLOP, 1 );
	SetXCaretPolicy( wxSTC_CARET_EVEN | wxSTC_VISIBLE_STRICT | wxSTC_CARET_SLOP, 1 );
	SetYCaretPolicy( wxSTC_CARET_EVEN | wxSTC_VISIBLE_STRICT | wxSTC_CARET_SLOP, 1 );
	// miscellaneous
	m_lineNrMargin = TextWidth( wxSTC_STYLE_LINENUMBER, wxT( "_999999" ) );
	m_foldingMargin = 16;
	CmdKeyClear( wxSTC_KEY_TAB, 0 ); // this is done by the menu accelerator key
	SetLayoutCache( wxSTC_CACHE_PAGE );

	initializePrefs( DEFAULT_LANGUAGE );
}

StcTextEditor::~StcTextEditor()
{
}

bool StcTextEditor::loadFile()
{
	bool result = false;
#if wxUSE_FILEDLG

	if ( ! m_filename )
	{
		wxFileDialog dlg( this
			, _( "Open file" )
			, wxEmptyString
			, wxEmptyString
			, _( "Any file (*)|*" )
			, wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_CHANGE_DIR );

		if ( dlg.ShowModal() != wxID_OK )
		{
			return false;
		}

		m_filename = dlg.GetPath();
	}

	result = LoadFile( m_filename );
#endif
	return result;
}

bool StcTextEditor::loadFile( wxString const & filename )
{
	if ( !filename.empty() )
	{
		m_filename = filename;
	}

	wxStyledTextCtrl::ClearAll();
	wxStyledTextCtrl::SetEOLMode( wxSTC_EOL_LF );
	wxStyledTextCtrl::LoadFile( m_filename );
	wxStyledTextCtrl::ConvertEOLs( wxSTC_EOL_LF );
	wxStyledTextCtrl::EmptyUndoBuffer();
	wxFileName filePath( m_filename );
	initializePrefs( determinePrefs( filePath.GetFullName() ) );
	return true;
}

void StcTextEditor::setText( wxString const & source )
{
	wxStyledTextCtrl::ClearAll();
	wxStyledTextCtrl::SetEOLMode( wxSTC_EOL_LF );
	wxStyledTextCtrl::SetText( source );
	wxStyledTextCtrl::ConvertEOLs( wxSTC_EOL_LF );
	wxStyledTextCtrl::EmptyUndoBuffer();
}

bool StcTextEditor::saveFile()
{
	bool result = false;
#if wxUSE_FILEDLG

	if ( isModified() )
	{
		if ( !m_filename )
		{
			wxFileDialog dlg( this
				, _( "Save file" )
				, wxEmptyString
				, wxEmptyString
				, _( "Any file (*)|*" )
				, wxFD_SAVE | wxFD_OVERWRITE_PROMPT );

			if ( dlg.ShowModal() != wxID_OK )
			{
				result = false;
			}
			else
			{
				result = true;
				m_filename = dlg.GetPath();
			}
		}
		else
		{
			result = true;
		}

		if ( result )
		{
			result = SaveFile( m_filename );
		}
	}
	else
	{
		result = true;
	}

#endif
	return result;
}

bool StcTextEditor::saveFile( wxString const & filename )
{
	bool result = true;

	if ( isModified() )
	{
		result = wxStyledTextCtrl::SaveFile( filename );
	}

	return result;
}

bool StcTextEditor::isModified()
{
	return ( GetModify() && !GetReadOnly() );
}

wxString StcTextEditor::determinePrefs( wxString const & filename )
{
	wxString result;

	for ( auto currInfo : m_context )
	{
		if ( result.empty() )
		{
			wxString filepattern = make_wxString( currInfo->filePattern );
			filepattern.Lower();

			while ( !filepattern.empty() && result.empty() )
			{
				auto strCur = filepattern.BeforeFirst( ';' );

				if ( ( strCur == filename )
					|| ( strCur == ( filename.BeforeLast( '.' ) + wxT( ".*" ) ) )
					|| ( strCur == ( wxT( "*." ) + filename.AfterLast( '.' ) ) ) )
				{
					result = currInfo->name;
				}
				else
				{
					filepattern = filepattern.AfterFirst( ';' );
				}
			}
		}
	}

	return result;
}

bool StcTextEditor::initializePrefs( wxString const & name )
{
	// Set margin for line numbers
	SetMarginType( m_lineNrID, wxSTC_MARGIN_NUMBER );
	SetMarginWidth( m_lineNrID, 50 );

	auto it = std::find_if( m_context.begin()
		, m_context.end()
		, [&name]( LanguageInfoPtr lookup )
		{
			return lookup->name.c_str() == name;
		} );

	if ( it == m_context.end() )
	{
		wxFont font( 10
			, wxFONTFAMILY_MODERN
			, wxFONTSTYLE_NORMAL
			, wxFONTWEIGHT_NORMAL
			, false );
		StyleSetFont( wxSTC_STYLE_DEFAULT, font );
		doInitialiseBaseColours( PANEL_BACKGROUND_COLOUR, PANEL_FOREGROUND_COLOUR );
	}
	else
	{
		m_language = *it;

		if ( m_language->isCLike )
		{
			SetLexer( wxSTC_LEX_CPP );
		}
		else
		{
			SetLexer( wxSTC_LEX_ASM );
		}

		auto defaultFgColour = m_language->getStyle( wxSTC_C_DEFAULT ).foreground;
		auto defaultBgColour = m_language->getStyle( wxSTC_C_DEFAULT ).background;
		wxFont font( m_language->fontSize
			, wxFONTFAMILY_MODERN
			, wxFONTSTYLE_NORMAL
			, wxFONTWEIGHT_NORMAL
			, false
			, make_wxString( m_language->fontName ) );
		StyleSetFont( wxSTC_STYLE_DEFAULT, font );
		doInitialiseBaseColours( defaultBgColour, defaultFgColour );

		wxArrayString keywords;

		// initialize settings
		if ( m_context.syntaxEnable )
		{
			for ( auto & styleIt : m_language->getStyles() )
			{
				StyleInfo const & styleInfo = styleIt.second;
				int style = styleIt.first;

				if ( styleInfo.foreground.IsOk() )
				{
					StyleSetForeground( style, styleInfo.foreground );
				}

				if ( styleInfo.background.IsOk() )
				{
					StyleSetBackground( style, styleInfo.background );
				}

				StyleSetBold( style, ( styleInfo.fontStyle & eSTC_STYLE_BOLD ) > 0 );
				StyleSetItalic( style, ( styleInfo.fontStyle & eSTC_STYLE_ITALIC ) > 0 );
				StyleSetUnderline( style, ( styleInfo.fontStyle & eSTC_STYLE_UNDERL ) > 0 );
				StyleSetVisible( style, ( styleInfo.fontStyle & eSTC_STYLE_HIDDEN ) == 0 );
				StyleSetCase( style, ( styleInfo.letterCase ) );
			}

			for ( auto index = 0; index < 9; ++index )
			{
				castor::String words = m_language->getKeywords( index );

				if ( !words.empty() )
				{
					SetKeyWords( index, words.c_str() );
					castor::StringArray array = castor::string::split( words, cuT( " \t\n\r" ), -1, false );

					for ( auto keyword : array )
					{
						keywords.push_back( keyword );
					}
				}
			}
		}

#if wxMAJOR_VERSION >= 3 || ( wxMAJOR_VERSION == 2 && wxMINOR_VERSION >= 9 )
		AutoComplete( new TextAutoCompleter( keywords ) );
#endif
		// set margin as unused
		SetMarginType( m_dividerID, wxSTC_MARGIN_SYMBOL );
		SetMarginWidth( m_dividerID, 0 );
		SetMarginSensitive( m_dividerID, false );
		// folding
		SetMarginType( m_foldingID, wxSTC_MARGIN_SYMBOL );
		SetMarginMask( m_foldingID, wxSTC_MASK_FOLDERS );
		StyleSetBackground( m_foldingID, defaultBgColour );
		SetMarginWidth( m_foldingID, 0 );
		SetMarginSensitive( m_foldingID, false );

		if ( m_context.foldEnable )
		{
			SetMarginWidth( m_foldingID, ( ( m_language->foldFlags != 0 ) ? m_foldingMargin : 0 ) );
			SetMarginSensitive( m_foldingID, ( ( m_language->foldFlags != 0 ) ) );
			SetProperty( wxT( "fold" ), ( ( m_language->foldFlags != 0 ) ? wxT( "1" ) : wxT( "0" ) ) );
			SetProperty( wxT( "fold.comment" ), ( ( m_language->foldFlags & eSTC_FOLD_COMMENT ) > 0 ? wxT( "1" ) : wxT( "0" ) ) );
			SetProperty( wxT( "fold.compact" ), ( ( m_language->foldFlags & eSTC_FOLD_COMPACT ) > 0 ? wxT( "1" ) : wxT( "0" ) ) );
			SetProperty( wxT( "fold.preprocessor" ), ( ( m_language->foldFlags & eSTC_FOLD_PREPROC ) > 0	? wxT( "1" ) : wxT( "0" ) ) );
			SetProperty( wxT( "fold.html" ), ( ( m_language->foldFlags & eSTC_FOLD_HTML ) > 0	? wxT( "1" ) : wxT( "0" ) ) );
			SetProperty( wxT( "fold.html.preprocessor" ), ( ( m_language->foldFlags & eSTC_FOLD_HTMLPREP ) > 0	? wxT( "1" ) : wxT( "0" ) ) );
			SetProperty( wxT( "fold.comment.python" ), ( ( m_language->foldFlags & eSTC_FOLD_COMMENTPY ) > 0	? wxT( "1" ) : wxT( "0" ) ) );
			SetProperty( wxT( "fold.quotes.python" ), ( ( m_language->foldFlags & eSTC_FOLD_QUOTESPY ) > 0	? wxT( "1" ) : wxT( "0" ) ) );
		}

		SetFoldFlags( wxSTC_FOLDFLAG_LINEBEFORE_CONTRACTED | wxSTC_FOLDFLAG_LINEAFTER_CONTRACTED );
		// set spaces and indention
		SetTabWidth( m_tabSpaces );
		SetUseTabs( m_useTabs );
		SetTabIndents( m_tabIndents );
		SetBackSpaceUnIndents( m_backspaceUnindents );
		SetIndent( m_context.indentEnable ? m_tabSpaces : 0 );
		// others
		SetViewEOL( m_context.displayEOLEnable );
		SetIndentationGuides( m_context.indentGuideEnable );
		SetEdgeColumn( 80 );
		SetEdgeMode( m_context.longLineOnEnable ? wxSTC_EDGE_LINE : wxSTC_EDGE_NONE );
		SetViewWhiteSpace( m_context.whiteSpaceEnable ? wxSTC_WS_VISIBLEALWAYS : wxSTC_WS_INVISIBLE );
		SetOvertype( m_context.overTypeInitial );
		SetReadOnly( m_context.readOnlyInitial );
		SetWrapMode( m_context.wrapModeInitial ? wxSTC_WRAP_WORD : wxSTC_WRAP_NONE );
	}

	return it != m_context.end();
}

void StcTextEditor::doInitialiseBaseColours( wxColour const & bgColour
	, wxColour const & fgColour )
{
	SetBackgroundColour( bgColour );
	SetForegroundColour( fgColour );
	SetSelAlpha( 127 );
	SetSelBackground( true, wxColour( 51, 153, 255, 127 ) );
	SetFoldMarginColour( true, bgColour );
	SetFoldMarginHiColour( true, bgColour );
	StyleSetBackground( wxSTC_STYLE_DEFAULT, bgColour );
	StyleSetForeground( wxSTC_STYLE_DEFAULT, fgColour );
	StyleClearAll();

	SetCaretForeground( fgColour );

	MarkerDefine( wxSTC_MARKNUM_FOLDER, wxSTC_MARK_DOTDOTDOT, fgColour, bgColour );
	MarkerDefine( wxSTC_MARKNUM_FOLDEROPEN, wxSTC_MARK_ARROWDOWN, fgColour, bgColour );
	MarkerDefine( wxSTC_MARKNUM_FOLDERSUB, wxSTC_MARK_EMPTY, fgColour, bgColour );
	MarkerDefine( wxSTC_MARKNUM_FOLDEREND, wxSTC_MARK_DOTDOTDOT, fgColour, bgColour );
	MarkerDefine( wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_ARROWDOWN, fgColour, bgColour );
	MarkerDefine( wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_EMPTY, fgColour, bgColour );
	MarkerDefine( wxSTC_MARKNUM_FOLDERTAIL, wxSTC_MARK_EMPTY, fgColour, bgColour );

	MarkerSetBackground( wxSTC_MARKNUM_FOLDER, bgColour );
	MarkerSetForeground( wxSTC_MARKNUM_FOLDER, fgColour );
	MarkerSetBackground( wxSTC_MARKNUM_FOLDEROPEN, bgColour );
	MarkerSetForeground( wxSTC_MARKNUM_FOLDEROPEN, fgColour );
	MarkerSetBackground( wxSTC_MARKNUM_FOLDERSUB, bgColour );
	MarkerSetForeground( wxSTC_MARKNUM_FOLDERSUB, fgColour );
	MarkerSetBackground( wxSTC_MARKNUM_FOLDEREND, bgColour );
	MarkerSetForeground( wxSTC_MARKNUM_FOLDEREND, fgColour );
	MarkerSetBackground( wxSTC_MARKNUM_FOLDEROPENMID, bgColour );
	MarkerSetForeground( wxSTC_MARKNUM_FOLDEROPENMID, fgColour );
	MarkerSetBackground( wxSTC_MARKNUM_FOLDERMIDTAIL, bgColour );
	MarkerSetForeground( wxSTC_MARKNUM_FOLDERMIDTAIL, fgColour );
	MarkerSetBackground( wxSTC_MARKNUM_FOLDERTAIL, bgColour );
	MarkerSetForeground( wxSTC_MARKNUM_FOLDERTAIL, fgColour );

	// set common styles
	StyleSetBackground( wxSTC_STYLE_LINENUMBER, bgColour );
	StyleSetForeground( wxSTC_STYLE_LINENUMBER, fgColour );
	StyleSetBackground( wxSTC_STYLE_CONTROLCHAR, bgColour );
	StyleSetForeground( wxSTC_STYLE_CONTROLCHAR, fgColour );
	StyleSetBackground( wxSTC_STYLE_BRACELIGHT, bgColour );
	StyleSetForeground( wxSTC_STYLE_BRACELIGHT, wxColour( wxT( "DARK GREEN" ) ) );
	StyleSetBackground( wxSTC_STYLE_BRACEBAD, bgColour );
	StyleSetForeground( wxSTC_STYLE_BRACEBAD, wxColour( wxT( "DARK RED" ) ) );
	StyleSetBackground( wxSTC_STYLE_INDENTGUIDE, bgColour );
	StyleSetBackground( wxSTC_STYLE_CALLTIP, bgColour );
	StyleSetBackground( wxSTC_STYLE_LASTPREDEFINED, bgColour );

	StyleSetBackground( wxSTC_C_DEFAULT, bgColour );
	StyleSetForeground( wxSTC_C_DEFAULT, fgColour );
	StyleSetBackground( wxSTC_C_COMMENT, bgColour );
	StyleSetBackground( wxSTC_C_COMMENTLINE, bgColour );
	StyleSetBackground( wxSTC_C_COMMENTDOC, bgColour );
	StyleSetBackground( wxSTC_C_COMMENTLINEDOC, bgColour );
	StyleSetBackground( wxSTC_C_COMMENTDOCKEYWORD, bgColour );
	StyleSetBackground( wxSTC_C_COMMENTDOCKEYWORDERROR, bgColour );
	StyleSetBackground( wxSTC_C_PREPROCESSORCOMMENT, bgColour );
#if wxCHECK_VERSION( 3, 1, 0 )
	StyleSetBackground( wxSTC_C_PREPROCESSORCOMMENTDOC, bgColour );
#endif
}

BEGIN_EVENT_TABLE( StcTextEditor, wxStyledTextCtrl )
	EVT_SIZE( StcTextEditor::onSize )
	EVT_MENU( wxID_CLEAR, StcTextEditor::onEditClear )
	EVT_MENU( wxID_CUT, StcTextEditor::onEditCut )
	EVT_MENU( wxID_COPY, StcTextEditor::onEditCopy )
	EVT_MENU( wxID_PASTE, StcTextEditor::onEditPaste )
	EVT_MENU( gcID_INDENTINC, StcTextEditor::onEditIndentInc )
	EVT_MENU( gcID_INDENTRED, StcTextEditor::onEditIndentRed )
	EVT_MENU( wxID_SELECTALL, StcTextEditor::onEditSelectAll )
	EVT_MENU( gcID_SELECTLINE, StcTextEditor::onEditSelectLine )
	EVT_MENU( wxID_REDO, StcTextEditor::onEditRedo )
	EVT_MENU( wxID_UNDO, StcTextEditor::onEditUndo )
	EVT_MENU( wxID_FIND, StcTextEditor::onFind )
	EVT_MENU( gcID_FINDNEXT, StcTextEditor::onFindNext )
	EVT_MENU( gcID_REPLACE, StcTextEditor::onReplace )
	EVT_MENU( gcID_REPLACENEXT, StcTextEditor::onReplaceNext )
	EVT_MENU( gcID_BRACEMATCH, StcTextEditor::onBraceMatch )
	EVT_MENU( gcID_GOTO, StcTextEditor::onGoto )
	EVT_MENU_RANGE( gcID_HILIGHTFIRST, gcID_HILIGHTLAST, StcTextEditor::onHighlightLang )
	EVT_MENU( gcID_DISPLAYEOL, StcTextEditor::onDisplayEOL )
	EVT_MENU( gcID_INDENTGUIDE, StcTextEditor::onIndentGuide )
	EVT_MENU( gcID_LINENUMBER, StcTextEditor::onLineNumber )
	EVT_MENU( gcID_LONGLINEON, StcTextEditor::onLongLineOn )
	EVT_MENU( gcID_WHITESPACE, StcTextEditor::onWhiteSpace )
	EVT_MENU( gcID_FOLDTOGGLE, StcTextEditor::onFoldToggle )
	EVT_MENU( gcID_OVERTYPE, StcTextEditor::onSetOverType )
	EVT_MENU( gcID_READONLY, StcTextEditor::onSetReadOnly )
	EVT_MENU( gcID_WRAPMODEON, StcTextEditor::onWrapmodeOn )
	EVT_MENU( gcID_CHARSETANSI, StcTextEditor::onUseCharset )
	EVT_MENU( gcID_CHARSETMAC, StcTextEditor::onUseCharset )
	EVT_MENU( gcID_CHANGELOWER, StcTextEditor::onChangeCase )
	EVT_MENU( gcID_CHANGEUPPER, StcTextEditor::onChangeCase )
	EVT_MENU( gcID_CONVERTCR, StcTextEditor::onConvertEOL )
	EVT_MENU( gcID_CONVERTCRLF, StcTextEditor::onConvertEOL )
	EVT_MENU( gcID_CONVERTLF, StcTextEditor::onConvertEOL )
	EVT_STC_MARGINCLICK( wxID_ANY, StcTextEditor::onMarginClick )
	EVT_STC_CHARADDED( wxID_ANY, StcTextEditor::onCharAdded )
END_EVENT_TABLE()


void StcTextEditor::onSize( wxSizeEvent & event )
{
	int x = GetClientSize().x
		+ ( m_context.lineNumberEnable ? m_lineNrMargin : 0 )
		+ ( m_context.foldEnable ? m_foldingMargin : 0 );

	if ( x > 0 )
	{
		SetScrollWidth( x );
	}

	event.Skip();
}

void StcTextEditor::onEditRedo( wxCommandEvent & WXUNUSED( event ) )
{
	if ( CanRedo() )
	{
		Redo();
	}
}

void StcTextEditor::onEditUndo( wxCommandEvent & WXUNUSED( event ) )
{
	if ( CanUndo() )
	{
		Undo();
	}
}

void StcTextEditor::onEditClear( wxCommandEvent & WXUNUSED( event ) )
{
	if ( !GetReadOnly() )
	{
		Clear();
	}
}

void StcTextEditor::onKey( wxStyledTextEvent & WXUNUSED( event ) )
{
	wxMessageBox( wxT( "OnKey" ) );
}

void StcTextEditor::onEditCut( wxCommandEvent & WXUNUSED( event ) )
{
	if ( !GetReadOnly() && ( GetSelectionEnd() - GetSelectionStart() ) > 0 )
	{
		Cut();
	}
}

void StcTextEditor::onEditCopy( wxCommandEvent & WXUNUSED( event ) )
{
	if ( GetSelectionEnd() - GetSelectionStart() > 0 )
	{
		Copy();
	}
}

void StcTextEditor::onEditPaste( wxCommandEvent & WXUNUSED( event ) )
{
	if ( CanPaste() )
	{
		Paste();
	}
}

void StcTextEditor::onFind( wxCommandEvent & WXUNUSED( event ) )
{
}

void StcTextEditor::onFindNext( wxCommandEvent & WXUNUSED( event ) )
{
}

void StcTextEditor::onReplace( wxCommandEvent & WXUNUSED( event ) )
{
}

void StcTextEditor::onReplaceNext( wxCommandEvent & WXUNUSED( event ) )
{
}

void StcTextEditor::onBraceMatch( wxCommandEvent & WXUNUSED( event ) )
{
	int min = GetCurrentPos();
	int max = BraceMatch( min );

	if ( max > ( min + 1 ) )
	{
		BraceHighlight( min + 1, max );
		SetSelection( min + 1, max );
	}
	else
	{
		BraceBadLight( min );
	}
}

void StcTextEditor::onGoto( wxCommandEvent & WXUNUSED( event ) )
{
}

void StcTextEditor::onEditIndentInc( wxCommandEvent & WXUNUSED( event ) )
{
	CmdKeyExecute( wxSTC_CMD_TAB );
}

void StcTextEditor::onEditIndentRed( wxCommandEvent & WXUNUSED( event ) )
{
	CmdKeyExecute( wxSTC_CMD_DELETEBACK );
}

void StcTextEditor::onEditSelectAll( wxCommandEvent & WXUNUSED( event ) )
{
	SetSelection( 0, GetTextLength() );
}

void StcTextEditor::onEditSelectLine( wxCommandEvent & WXUNUSED( event ) )
{
	int lineStart = PositionFromLine( GetCurrentLine() );
	int lineEnd = PositionFromLine( GetCurrentLine() + 1 );
	SetSelection( lineStart, lineEnd );
}

void StcTextEditor::onHighlightLang( wxCommandEvent & event )
{
	initializePrefs( ( *( m_context.begin() + ( event.GetId() - gcID_HILIGHTFIRST ) ) )->name );
}

void StcTextEditor::onDisplayEOL( wxCommandEvent & WXUNUSED( event ) )
{
	SetViewEOL( !GetViewEOL() );
}

void StcTextEditor::onIndentGuide( wxCommandEvent & WXUNUSED( event ) )
{
	SetIndentationGuides( !GetIndentationGuides() );
}

void StcTextEditor::onLineNumber( wxCommandEvent & WXUNUSED( event ) )
{
	SetMarginWidth( m_lineNrID, GetMarginWidth( m_lineNrID ) == 0 ? m_lineNrMargin : 0 );
}

void StcTextEditor::onLongLineOn( wxCommandEvent & WXUNUSED( event ) )
{
	SetEdgeMode( GetEdgeMode() == 0 ? wxSTC_EDGE_LINE : wxSTC_EDGE_NONE );
}

void StcTextEditor::onWhiteSpace( wxCommandEvent & WXUNUSED( event ) )
{
	SetViewWhiteSpace( GetViewWhiteSpace() == 0 ? wxSTC_WS_VISIBLEALWAYS : wxSTC_WS_INVISIBLE );
}

void StcTextEditor::onFoldToggle( wxCommandEvent & WXUNUSED( event ) )
{
	ToggleFold( GetFoldParent( GetCurrentLine() ) );
}

void StcTextEditor::onSetOverType( wxCommandEvent & WXUNUSED( event ) )
{
	SetOvertype( !GetOvertype() );
}

void StcTextEditor::onSetReadOnly( wxCommandEvent & WXUNUSED( event ) )
{
	SetReadOnly( !GetReadOnly() );
}

void StcTextEditor::onWrapmodeOn( wxCommandEvent & WXUNUSED( event ) )
{
	SetWrapMode( GetWrapMode() == 0 ? wxSTC_WRAP_WORD : wxSTC_WRAP_NONE );
}

void StcTextEditor::onUseCharset( wxCommandEvent & event )
{
	int charset = GetCodePage();

	switch ( event.GetId() )
	{
	case gcID_CHARSETANSI:
		charset = wxSTC_CHARSET_ANSI;
		break;

	case gcID_CHARSETMAC:
		charset = wxSTC_CHARSET_ANSI;
		break;
	}

	for ( int i = 0; i < wxSTC_STYLE_LASTPREDEFINED; ++i )
	{
		StyleSetCharacterSet( i, charset );
	}

	SetCodePage( charset );
}

void StcTextEditor::onChangeCase( wxCommandEvent & event )
{
	switch ( event.GetId() )
	{
	case gcID_CHANGELOWER:
		CmdKeyExecute( wxSTC_CMD_LOWERCASE );
		break;

	case gcID_CHANGEUPPER:
		CmdKeyExecute( wxSTC_CMD_UPPERCASE );
		break;
	}
}

void StcTextEditor::onConvertEOL( wxCommandEvent & event )
{
	int eolMode = GetEOLMode();

	switch ( event.GetId() )
	{
	case gcID_CONVERTCR:
		eolMode = wxSTC_EOL_CR;
		break;

	case gcID_CONVERTCRLF:
		eolMode = wxSTC_EOL_CRLF;
		break;

	case gcID_CONVERTLF:
		eolMode = wxSTC_EOL_LF;
		break;
	}

	ConvertEOLs( eolMode );
	SetEOLMode( eolMode );
}

void StcTextEditor::onMarginClick( wxStyledTextEvent & event )
{
	if ( event.GetMargin() == 2 )
	{
		int lineClick = LineFromPosition( event.GetPosition() );
		int levelClick = GetFoldLevel( lineClick );

		if ( ( levelClick & wxSTC_FOLDLEVELHEADERFLAG ) > 0 )
		{
			ToggleFold( lineClick );
		}
	}
}

void StcTextEditor::onCharAdded( wxStyledTextEvent & event )
{
	char chr = ( char )event.GetKey();
	int currentLine = GetCurrentLine();

	if ( chr == '\n' )
	{
		int lineInd = 0;

		if ( currentLine > 0 )
		{
			lineInd = GetLineIndentation( currentLine - 1 );
		}

		if ( lineInd != 0 )
		{
			SetLineIndentation( currentLine, lineInd );
			GotoPos( PositionFromLine( currentLine ) + lineInd );
		}
	}
}

//*************************************************************************************************
