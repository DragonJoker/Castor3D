#include "StcTextEditor.hpp"
#include "LanguageInfo.hpp"
#include "StyleInfo.hpp"

#	include <wx/filename.h>

using namespace GuiCommon;

//*************************************************************************************************

struct LexerStylesAssoc
{
	int m_iStyles[eSTC_TYPE_COUNT];

	LexerStylesAssoc( eSTC_LEX p_eLex )
	{
		memset( m_iStyles, 0, sizeof( m_iStyles ) );

		if ( p_eLex == eSTC_LEX_CPP )
		{
			int iStyles[eSTC_TYPE_COUNT] =
			{
				wxSTC_C_DEFAULT			//eSTC_TYPE_DEFAULT
				, wxSTC_C_WORD			//eSTC_TYPE_WORD1
				, wxSTC_C_WORD2			//eSTC_TYPE_WORD2
				, wxSTC_C_DEFAULT			//eSTC_TYPE_WORD3
				, wxSTC_C_GLOBALCLASS		//eSTC_TYPE_WORD4
				, wxSTC_C_DEFAULT			//eSTC_TYPE_WORD5
				, wxSTC_C_DEFAULT			//eSTC_TYPE_WORD6
				, wxSTC_C_COMMENT			//eSTC_TYPE_COMMENT
				, wxSTC_C_COMMENTDOC		//eSTC_TYPE_COMMENT_DOC
				, wxSTC_C_COMMENTLINE		//eSTC_TYPE_COMMENT_LINE
				, wxSTC_C_DEFAULT			//eSTC_TYPE_COMMENT_SPECIAL
				, wxSTC_C_CHARACTER		//eSTC_TYPE_CHARACTER
				, wxSTC_C_DEFAULT			//eSTC_TYPE_CHARACTER_EOL
				, wxSTC_C_STRING			//eSTC_TYPE_STRING
				, wxSTC_C_STRINGEOL		//eSTC_TYPE_STRING_EOL
				, wxSTC_C_DEFAULT			//eSTC_TYPE_DELIMITER
				, wxSTC_C_DEFAULT			//eSTC_TYPE_PUNCTUATION
				, wxSTC_C_OPERATOR		//eSTC_TYPE_OPERATOR
				, wxSTC_C_DEFAULT			//eSTC_TYPE_BRACE
				, wxSTC_C_DEFAULT			//eSTC_TYPE_COMMAND
				, wxSTC_C_IDENTIFIER		//eSTC_TYPE_IDENTIFIER
				, wxSTC_C_DEFAULT			//eSTC_TYPE_LABEL
				, wxSTC_C_NUMBER			//eSTC_TYPE_NUMBER
				, wxSTC_C_DEFAULT			//eSTC_TYPE_PARAMETER
				, wxSTC_C_REGEX			//eSTC_TYPE_REGEX
				, wxSTC_C_UUID			//eSTC_TYPE_UUID
				, wxSTC_C_DEFAULT			//eSTC_TYPE_VALUE
				, wxSTC_C_PREPROCESSOR	//eSTC_TYPE_PREPROCESSOR
				, wxSTC_C_DEFAULT			//eSTC_TYPE_SCRIPT
				, wxSTC_C_DEFAULT			//eSTC_TYPE_ERROR
				, wxSTC_C_DEFAULT			//eSTC_TYPE_UNDEFINED
			};
			memcpy( m_iStyles, iStyles, sizeof( iStyles ) );
		}
	}

	inline int operator []( uint32_t p_uiIndex )
	{
		return m_iStyles[p_uiIndex];
	}
};

//*************************************************************************************************

#if wxMAJOR_VERSION >= 3 || ( wxMAJOR_VERSION == 2 && wxMINOR_VERSION >= 9 )
StcTextEditor::TextAutoCompleter::TextAutoCompleter( wxArrayString const & p_keywords )
{
	for ( wxArrayString::const_iterator l_it = p_keywords.begin(); l_it != p_keywords.end(); ++l_it )
	{
		m_keywords.insert( *l_it );
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
	wxString l_return;

	if ( m_current != m_keywords.end() )
	{
		++m_current;

		if ( m_current != m_keywords.end() )
		{
			if ( m_current->find( m_prefix ) == 0 )
			{
				l_return = *m_current;
			}
		}
	}

	return l_return;
}
#endif
//*************************************************************************************************

StcTextEditor::StcTextEditor( StcContext & p_context, wxWindow * p_pParent, wxWindowID p_id, wxPoint const & p_ptPos, wxSize const & p_size, long p_lStyle )
	:	wxStyledTextCtrl( p_pParent, p_id, p_ptPos, p_size, p_lStyle )
	, m_context( p_context )
	, m_strFilename( wxEmptyString )
	, m_iLineNrID( 0 )
	, m_iDividerID( 1 )
	, m_iFoldingID( 2 )
	, m_pLanguage()
	, m_iTabSpaces( 4 )
	, m_bUseTabs( true )
	, m_bTabIndents( true )
	, m_bBackspaceUnindents( true )
{
	wxFont l_font( 10, wxMODERN, wxNORMAL, wxNORMAL );
	SetTabWidth( m_iTabSpaces );
	SetSelAlpha( 127 );
	SetSelBackground( true, wxColour( 51, 153, 255, 127 ) );
	SetViewEOL( m_context.GetDisplayEOLEnable() );
	SetIndentationGuides( m_context.GetIndentGuideEnable() );
	SetEdgeMode( m_context.GetLongLineOnEnable() ? wxSTC_EDGE_LINE : wxSTC_EDGE_NONE );
	SetViewWhiteSpace( m_context.GetWhiteSpaceEnable() ? wxSTC_WS_VISIBLEALWAYS : wxSTC_WS_INVISIBLE );
	SetOvertype( m_context.GetOverTypeInitial() );
	SetReadOnly( m_context.GetReadOnlyInitial() );
	SetWrapMode( m_context.GetWrapModeInitial() ? wxSTC_WRAP_WORD : wxSTC_WRAP_NONE );
	StyleSetFont( wxSTC_STYLE_DEFAULT, l_font );
	InitializePrefs( DEFAULT_LANGUAGE );
	// set visibility
	SetVisiblePolicy( wxSTC_VISIBLE_STRICT | wxSTC_VISIBLE_SLOP, 1 );
	SetXCaretPolicy( wxSTC_CARET_EVEN | wxSTC_VISIBLE_STRICT | wxSTC_CARET_SLOP, 1 );
	SetYCaretPolicy( wxSTC_CARET_EVEN | wxSTC_VISIBLE_STRICT | wxSTC_CARET_SLOP, 1 );
	// markers
	MarkerDefine( wxSTC_MARKNUM_FOLDER, wxSTC_MARK_DOTDOTDOT, wxT( "BLACK" ), wxT( "BLACK" ) );
	MarkerDefine( wxSTC_MARKNUM_FOLDEROPEN, wxSTC_MARK_ARROWDOWN, wxT( "BLACK" ), wxT( "BLACK" ) );
	MarkerDefine( wxSTC_MARKNUM_FOLDERSUB, wxSTC_MARK_EMPTY, wxT( "BLACK" ), wxT( "BLACK" ) );
	MarkerDefine( wxSTC_MARKNUM_FOLDEREND, wxSTC_MARK_DOTDOTDOT, wxT( "BLACK" ), wxT( "WHITE" ) );
	MarkerDefine( wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_ARROWDOWN, wxT( "BLACK" ), wxT( "WHITE" ) );
	MarkerDefine( wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_EMPTY, wxT( "BLACK" ), wxT( "BLACK" ) );
	MarkerDefine( wxSTC_MARKNUM_FOLDERTAIL, wxSTC_MARK_EMPTY, wxT( "BLACK" ), wxT( "BLACK" ) );
	// miscellaneous
	m_iLineNrMargin = TextWidth( wxSTC_STYLE_LINENUMBER, wxT( "_999999" ) );
	m_iFoldingMargin = 16;
	CmdKeyClear( wxSTC_KEY_TAB, 0 ); // this is done by the menu accelerator key
	SetLayoutCache( wxSTC_CACHE_PAGE );
}

StcTextEditor::~StcTextEditor()
{
}

bool StcTextEditor::LoadFile()
{
	bool l_return = false;
#if wxUSE_FILEDLG

	if ( ! m_strFilename )
	{
		wxFileDialog l_dlg( this, _( "Open file" ), wxEmptyString, wxEmptyString, _( "Any file (*)|*" ), wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_CHANGE_DIR );

		if ( l_dlg.ShowModal() != wxID_OK )
		{
			return false;
		}

		m_strFilename = l_dlg.GetPath();
	}

	l_return = LoadFile( m_strFilename );
#endif
	return l_return;
}

bool StcTextEditor::LoadFile( wxString const & p_strFilename )
{
	if ( ! p_strFilename.empty() )
	{
		m_strFilename = p_strFilename;
	}

	wxStyledTextCtrl::ClearAll();
	wxStyledTextCtrl::SetEOLMode( wxSTC_EOL_LF );
	wxStyledTextCtrl::LoadFile( m_strFilename );
	wxStyledTextCtrl::ConvertEOLs( wxSTC_EOL_LF );
	wxStyledTextCtrl::EmptyUndoBuffer();
	wxFileName l_fileName( m_strFilename );
	InitializePrefs( DeterminePrefs( l_fileName.GetFullName() ) );
	return true;
}

void StcTextEditor::SetText( wxString const & p_strSource )
{
	wxStyledTextCtrl::ClearAll();
	wxStyledTextCtrl::SetEOLMode( wxSTC_EOL_LF );
	wxStyledTextCtrl::SetText( p_strSource );
	wxStyledTextCtrl::ConvertEOLs( wxSTC_EOL_LF );
	wxStyledTextCtrl::EmptyUndoBuffer();
}

bool StcTextEditor::SaveFile()
{
	bool l_return = false;
#if wxUSE_FILEDLG

	if ( IsModified() )
	{
		if ( !m_strFilename )
		{
			wxFileDialog l_dlg( this, _( "Save file" ), wxEmptyString, wxEmptyString, _( "Any file (*)|*" ), wxFD_SAVE | wxFD_OVERWRITE_PROMPT );

			if ( l_dlg.ShowModal() != wxID_OK )
			{
				l_return = false;
			}
			else
			{
				l_return = true;
				m_strFilename = l_dlg.GetPath();
			}
		}
		else
		{
			l_return = true;
		}

		if ( l_return )
		{
			l_return = SaveFile( m_strFilename );
		}
	}
	else
	{
		l_return = true;
	}

#endif
	return l_return;
}

bool StcTextEditor::SaveFile( wxString const & p_strFilename )
{
	bool l_return = true;

	if ( IsModified() )
	{
		l_return = wxStyledTextCtrl::SaveFile( p_strFilename );
	}

	return l_return;
}

bool StcTextEditor::IsModified()
{
	return ( GetModify() && ! GetReadOnly() );
}

wxString StcTextEditor::DeterminePrefs( wxString const & p_strFilename )
{
	LanguageInfoPtr l_pCurInfo;
	wxString l_strReturn;
	wxString l_strFilepattern;
	wxString l_strCur;

	for ( LanguageInfoPtrArrayConstIt l_it = m_context.Begin(); l_it != m_context.End() && l_strReturn.empty(); ++l_it )
	{
		l_pCurInfo = * l_it;
		l_strFilepattern = l_pCurInfo->GetFilePattern();
		l_strFilepattern.Lower();

		while ( ! l_strFilepattern.empty() && l_strReturn.empty() )
		{
			l_strCur = l_strFilepattern.BeforeFirst( ';' );

			if ( ( l_strCur == p_strFilename ) || ( l_strCur == ( p_strFilename.BeforeLast( '.' ) + wxT( ".*" ) ) ) || ( l_strCur == ( wxT( "*." ) + p_strFilename.AfterLast( '.' ) ) ) )
			{
				l_strReturn = l_pCurInfo->GetName();
			}
			else
			{
				l_strFilepattern = l_strFilepattern.AfterFirst( ';' );
			}
		}
	}

	return l_strReturn;
}

bool StcTextEditor::InitializePrefs( wxString const & p_strName )
{
	StyleClearAll();
	LanguageInfoPtr l_pCurInfo;
	bool l_bFound = false;

	for ( LanguageInfoPtrArrayConstIt l_it = m_context.Begin(); l_it != m_context.End() && ! l_bFound; ++l_it )
	{
		l_pCurInfo = * l_it;

		if ( l_pCurInfo->GetName().c_str() == p_strName )
		{
			l_bFound = true;
		}
	}

	if ( l_bFound )
	{
		wxString l_defaultFgColour = l_pCurInfo->GetStyle( eSTC_TYPE_DEFAULT )->GetForeground();
		wxString l_defaultBgColour = l_pCurInfo->GetStyle( eSTC_TYPE_DEFAULT )->GetBackground();
		// set lexer and language
		SetLexer( l_pCurInfo->GetLexerID() );
		m_pLanguage = l_pCurInfo;
		// set margin for line numbers
		SetMarginType( m_iLineNrID, wxSTC_MARGIN_NUMBER );
		SetMarginWidth( m_iLineNrID, 50 );

		// default fonts for all styles!
		for ( int i = 0; i < wxSTC_STYLE_LASTPREDEFINED; i++ )
		{
			wxFont l_font( 10, wxMODERN, wxNORMAL, wxNORMAL );
			StyleSetFont( i, l_font );
		}

		// set common styles
		StyleSetForeground( wxSTC_STYLE_LINENUMBER, l_defaultFgColour );
		StyleSetBackground( wxSTC_STYLE_LINENUMBER, l_defaultBgColour );
		StyleSetForeground( wxSTC_STYLE_DEFAULT, l_defaultFgColour );
		StyleSetBackground( wxSTC_STYLE_DEFAULT, l_defaultBgColour );
		StyleSetForeground( wxSTC_STYLE_CONTROLCHAR, l_defaultFgColour );
		StyleSetBackground( wxSTC_STYLE_CONTROLCHAR, l_defaultBgColour );
		StyleSetForeground( wxSTC_STYLE_BRACELIGHT, wxColour( wxT( "DARK GREEN" ) ) );
		StyleSetBackground( wxSTC_STYLE_BRACELIGHT, l_defaultBgColour );
		StyleSetForeground( wxSTC_STYLE_BRACEBAD, wxColour( wxT( "DARK RED" ) ) );
		StyleSetBackground( wxSTC_STYLE_BRACEBAD, l_defaultBgColour );
		StyleSetBackground( wxSTC_STYLE_INDENTGUIDE, l_defaultBgColour );
		StyleSetBackground( wxSTC_STYLE_CALLTIP, l_defaultBgColour );
		StyleSetBackground( wxSTC_STYLE_LASTPREDEFINED, l_defaultBgColour );
		wxArrayString l_keywords;

		// initialize settings
		if ( m_context.GetSyntaxEnable() )
		{
			int l_iNbKeywords = 0;
			LexerStylesAssoc l_lexerAssoc( l_pCurInfo->GetLexerID() );

			for ( int j = eSTC_TYPE_DEFAULT; j < eSTC_TYPE_COUNT; ++j )
			{
				eSTC_TYPE l_eType = eSTC_TYPE( j );
				int l_iStyle = l_lexerAssoc[l_eType];
				StyleInfoPtr const & l_stCurType = l_pCurInfo->GetStyle( l_eType );
				wxFont l_font( l_stCurType->GetFontSize(), wxMODERN, wxNORMAL, wxNORMAL, false, l_stCurType->GetFontName() );
				StyleSetFont( l_iStyle, l_font );

				if ( !l_stCurType->GetForeground().empty() )
				{
					StyleSetForeground( l_iStyle, l_stCurType->GetForeground() );
				}

				if ( !l_stCurType->GetBackground().empty() )
				{
					StyleSetBackground( l_iStyle, l_stCurType->GetBackground() );
				}

				StyleSetBold( l_iStyle, ( l_stCurType->GetFontStyle() & eSTC_STYLE_BOLD ) >  0 );
				StyleSetItalic( l_iStyle, ( l_stCurType->GetFontStyle() & eSTC_STYLE_ITALIC ) >  0 );
				StyleSetUnderline( l_iStyle, ( l_stCurType->GetFontStyle() & eSTC_STYLE_UNDERL ) >  0 );
				StyleSetVisible( l_iStyle, ( l_stCurType->GetFontStyle() & eSTC_STYLE_HIDDEN ) == 0 );
				StyleSetCase( l_iStyle, ( l_stCurType->GetLetterCase() ) );

				if ( ! l_pCurInfo->GetWords( l_eType ).empty() )
				{
					Castor::String l_words = l_pCurInfo->GetWords( l_eType );
					SetKeyWords( l_iNbKeywords, l_words.c_str() );
					Castor::StringArray l_array = Castor::string::split( l_words, cuT( " \t\n\r" ), -1, false );

					for ( Castor::StringArray::iterator l_it = l_array.begin(); l_it != l_array.end(); ++l_it )
					{
						l_keywords.push_back( *l_it );
					}

					++l_iNbKeywords;
				}
			}
		}

#if wxMAJOR_VERSION >= 3 || ( wxMAJOR_VERSION == 2 && wxMINOR_VERSION >= 9 )
		AutoComplete( new TextAutoCompleter( l_keywords ) );
#endif
		// set margin as unused
		SetMarginType( m_iDividerID, wxSTC_MARGIN_SYMBOL );
		SetMarginWidth( m_iDividerID, 0 );
		SetMarginSensitive( m_iDividerID, false );
		// folding
		SetMarginType( m_iFoldingID, wxSTC_MARGIN_SYMBOL );
		SetMarginMask( m_iFoldingID, wxSTC_MASK_FOLDERS );
		StyleSetBackground( m_iFoldingID, l_defaultBgColour );
		SetMarginWidth( m_iFoldingID, 0 );
		SetMarginSensitive( m_iFoldingID, false );

		if ( m_context.GetFoldEnable() )
		{
			SetMarginWidth( m_iFoldingID, ( ( l_pCurInfo->GetFoldFlags() != 0 ) ? m_iFoldingMargin : 0 ) );
			SetMarginSensitive( m_iFoldingID, ( ( l_pCurInfo->GetFoldFlags() != 0 ) ) );
			SetProperty( wxT( "fold" ), ( ( l_pCurInfo->GetFoldFlags() != 0 ) ? wxT( "1" ) : wxT( "0" ) ) );
			SetProperty( wxT( "fold.comment" ), ( ( l_pCurInfo->GetFoldFlags() & eSTC_FOLD_COMMENT ) > 0 ? wxT( "1" ) : wxT( "0" ) ) );
			SetProperty( wxT( "fold.compact" ), ( ( l_pCurInfo->GetFoldFlags() & eSTC_FOLD_COMPACT ) > 0 ? wxT( "1" ) : wxT( "0" ) ) );
			SetProperty( wxT( "fold.preprocessor" ), ( ( l_pCurInfo->GetFoldFlags() & eSTC_FOLD_PREPROC ) > 0	? wxT( "1" ) : wxT( "0" ) ) );
			SetProperty( wxT( "fold.html" ), ( ( l_pCurInfo->GetFoldFlags() & eSTC_FOLD_HTML ) > 0	? wxT( "1" ) : wxT( "0" ) ) );
			SetProperty( wxT( "fold.html.preprocessor" ), ( ( l_pCurInfo->GetFoldFlags() & eSTC_FOLD_HTMLPREP ) > 0	? wxT( "1" ) : wxT( "0" ) ) );
			SetProperty( wxT( "fold.comment.python" ), ( ( l_pCurInfo->GetFoldFlags() & eSTC_FOLD_COMMENTPY ) > 0	? wxT( "1" ) : wxT( "0" ) ) );
			SetProperty( wxT( "fold.quotes.python" ), ( ( l_pCurInfo->GetFoldFlags() & eSTC_FOLD_QUOTESPY ) > 0	? wxT( "1" ) : wxT( "0" ) ) );
		}

		SetFoldFlags( wxSTC_FOLDFLAG_LINEBEFORE_CONTRACTED | wxSTC_FOLDFLAG_LINEAFTER_CONTRACTED );
		// set spaces and indention
		SetTabWidth( m_iTabSpaces );
		SetUseTabs( m_bUseTabs );
		SetTabIndents( m_bTabIndents );
		SetBackSpaceUnIndents( m_bBackspaceUnindents );
		SetIndent( m_context.GetIndentEnable() ? m_iTabSpaces : 0 );
		// others
		SetViewEOL( m_context.GetDisplayEOLEnable() );
		SetIndentationGuides( m_context.GetIndentGuideEnable() );
		SetEdgeColumn( 80 );
		SetEdgeMode( m_context.GetLongLineOnEnable() ? wxSTC_EDGE_LINE : wxSTC_EDGE_NONE );
		SetViewWhiteSpace( m_context.GetWhiteSpaceEnable() ? wxSTC_WS_VISIBLEALWAYS : wxSTC_WS_INVISIBLE );
		SetOvertype( m_context.GetOverTypeInitial() );
		SetReadOnly( m_context.GetReadOnlyInitial() );
		SetWrapMode( m_context.GetWrapModeInitial() ? wxSTC_WRAP_WORD : wxSTC_WRAP_NONE );
	}

	return l_bFound;
}

BEGIN_EVENT_TABLE( StcTextEditor, wxStyledTextCtrl )
	EVT_SIZE( StcTextEditor::OnSize )
	EVT_MENU( wxID_CLEAR, StcTextEditor::OnEditClear )
	EVT_MENU( wxID_CUT, StcTextEditor::OnEditCut )
	EVT_MENU( wxID_COPY, StcTextEditor::OnEditCopy )
	EVT_MENU( wxID_PASTE, StcTextEditor::OnEditPaste )
	EVT_MENU( gcID_INDENTINC, StcTextEditor::OnEditIndentInc )
	EVT_MENU( gcID_INDENTRED, StcTextEditor::OnEditIndentRed )
	EVT_MENU( wxID_SELECTALL, StcTextEditor::OnEditSelectAll )
	EVT_MENU( gcID_SELECTLINE, StcTextEditor::OnEditSelectLine )
	EVT_MENU( wxID_REDO, StcTextEditor::OnEditRedo )
	EVT_MENU( wxID_UNDO, StcTextEditor::OnEditUndo )
	EVT_MENU( wxID_FIND, StcTextEditor::OnFind )
	EVT_MENU( gcID_FINDNEXT, StcTextEditor::OnFindNext )
	EVT_MENU( gcID_REPLACE, StcTextEditor::OnReplace )
	EVT_MENU( gcID_REPLACENEXT, StcTextEditor::OnReplaceNext )
	EVT_MENU( gcID_BRACEMATCH, StcTextEditor::OnBraceMatch )
	EVT_MENU( gcID_GOTO, StcTextEditor::OnGoto )
	EVT_MENU_RANGE( gcID_HILIGHTFIRST, gcID_HILIGHTLAST, StcTextEditor::OnHilightLang )
	EVT_MENU( gcID_DISPLAYEOL, StcTextEditor::OnDisplayEOL )
	EVT_MENU( gcID_INDENTGUIDE, StcTextEditor::OnIndentGuide )
	EVT_MENU( gcID_LINENUMBER, StcTextEditor::OnLineNumber )
	EVT_MENU( gcID_LONGLINEON, StcTextEditor::OnLongLineOn )
	EVT_MENU( gcID_WHITESPACE, StcTextEditor::OnWhiteSpace )
	EVT_MENU( gcID_FOLDTOGGLE, StcTextEditor::OnFoldToggle )
	EVT_MENU( gcID_OVERTYPE, StcTextEditor::OnSetOverType )
	EVT_MENU( gcID_READONLY, StcTextEditor::OnSetReadOnly )
	EVT_MENU( gcID_WRAPMODEON, StcTextEditor::OnWrapmodeOn )
	EVT_MENU( gcID_CHARSETANSI, StcTextEditor::OnUseCharset )
	EVT_MENU( gcID_CHARSETMAC, StcTextEditor::OnUseCharset )
	EVT_MENU( gcID_CHANGELOWER, StcTextEditor::OnChangeCase )
	EVT_MENU( gcID_CHANGEUPPER, StcTextEditor::OnChangeCase )
	EVT_MENU( gcID_CONVERTCR, StcTextEditor::OnConvertEOL )
	EVT_MENU( gcID_CONVERTCRLF, StcTextEditor::OnConvertEOL )
	EVT_MENU( gcID_CONVERTLF, StcTextEditor::OnConvertEOL )
	EVT_STC_MARGINCLICK( wxID_ANY, StcTextEditor::OnMarginClick )
	EVT_STC_CHARADDED( wxID_ANY, StcTextEditor::OnCharAdded )
	EVT_STC_KEY( wxID_ANY , StcTextEditor::OnKey )
END_EVENT_TABLE()


void StcTextEditor::OnSize( wxSizeEvent & p_event )
{
	int x = GetClientSize().x + ( m_context.GetLineNumberEnable() ? m_iLineNrMargin : 0 ) + ( m_context.GetFoldEnable() ? m_iFoldingMargin : 0 );

	if ( x > 0 )
	{
		SetScrollWidth( x );
	}

	p_event.Skip();
}

void StcTextEditor::OnEditRedo( wxCommandEvent & WXUNUSED( p_event ) )
{
	if ( CanRedo() )
	{
		Redo();
	}
}

void StcTextEditor::OnEditUndo( wxCommandEvent & WXUNUSED( p_event ) )
{
	if ( CanUndo() )
	{
		Undo();
	}
}

void StcTextEditor::OnEditClear( wxCommandEvent & WXUNUSED( p_event ) )
{
	if ( ! GetReadOnly() )
	{
		Clear();
	}
}

void StcTextEditor::OnKey( wxStyledTextEvent & WXUNUSED( p_event ) )
{
	wxMessageBox( wxT( "OnKey" ) );
}

void StcTextEditor::OnEditCut( wxCommandEvent & WXUNUSED( p_event ) )
{
	if ( ! GetReadOnly() && ( GetSelectionEnd() - GetSelectionStart() ) > 0 )
	{
		Cut();
	}
}

void StcTextEditor::OnEditCopy( wxCommandEvent & WXUNUSED( p_event ) )
{
	if ( GetSelectionEnd() - GetSelectionStart() > 0 )
	{
		Copy();
	}
}

void StcTextEditor::OnEditPaste( wxCommandEvent & WXUNUSED( p_event ) )
{
	if ( CanPaste() )
	{
		Paste();
	}
}

void StcTextEditor::OnFind( wxCommandEvent & WXUNUSED( p_event ) )
{
}

void StcTextEditor::OnFindNext( wxCommandEvent & WXUNUSED( p_event ) )
{
}

void StcTextEditor::OnReplace( wxCommandEvent & WXUNUSED( p_event ) )
{
}

void StcTextEditor::OnReplaceNext( wxCommandEvent & WXUNUSED( p_event ) )
{
}

void StcTextEditor::OnBraceMatch( wxCommandEvent & WXUNUSED( p_event ) )
{
	int l_iMin = GetCurrentPos();
	int l_iMax = BraceMatch( l_iMin );

	if ( l_iMax > ( l_iMin + 1 ) )
	{
		BraceHighlight( l_iMin + 1, l_iMax );
		SetSelection( l_iMin + 1, l_iMax );
	}
	else
	{
		BraceBadLight( l_iMin );
	}
}

void StcTextEditor::OnGoto( wxCommandEvent & WXUNUSED( p_event ) )
{
}

void StcTextEditor::OnEditIndentInc( wxCommandEvent & WXUNUSED( p_event ) )
{
	CmdKeyExecute( wxSTC_CMD_TAB );
}

void StcTextEditor::OnEditIndentRed( wxCommandEvent & WXUNUSED( p_event ) )
{
	CmdKeyExecute( wxSTC_CMD_DELETEBACK );
}

void StcTextEditor::OnEditSelectAll( wxCommandEvent & WXUNUSED( p_event ) )
{
	SetSelection( 0, GetTextLength() );
}

void StcTextEditor::OnEditSelectLine( wxCommandEvent & WXUNUSED( p_event ) )
{
	int l_iLineStart = PositionFromLine( GetCurrentLine() );
	int l_iLineEnd = PositionFromLine( GetCurrentLine() + 1 );
	SetSelection( l_iLineStart, l_iLineEnd );
}

void StcTextEditor::OnHilightLang( wxCommandEvent & p_event )
{
	InitializePrefs( ( *( m_context.Begin() + ( p_event.GetId() - gcID_HILIGHTFIRST ) ) )->GetName() );
}

void StcTextEditor::OnDisplayEOL( wxCommandEvent & WXUNUSED( p_event ) )
{
	SetViewEOL( ! GetViewEOL() );
}

void StcTextEditor::OnIndentGuide( wxCommandEvent & WXUNUSED( p_event ) )
{
	SetIndentationGuides( ! GetIndentationGuides() );
}

void StcTextEditor::OnLineNumber( wxCommandEvent & WXUNUSED( p_event ) )
{
	SetMarginWidth( m_iLineNrID, GetMarginWidth( m_iLineNrID ) == 0 ? m_iLineNrMargin : 0 );
}

void StcTextEditor::OnLongLineOn( wxCommandEvent & WXUNUSED( p_event ) )
{
	SetEdgeMode( GetEdgeMode() == 0 ? wxSTC_EDGE_LINE : wxSTC_EDGE_NONE );
}

void StcTextEditor::OnWhiteSpace( wxCommandEvent & WXUNUSED( p_event ) )
{
	SetViewWhiteSpace( GetViewWhiteSpace() == 0 ? wxSTC_WS_VISIBLEALWAYS : wxSTC_WS_INVISIBLE );
}

void StcTextEditor::OnFoldToggle( wxCommandEvent & WXUNUSED( p_event ) )
{
	ToggleFold( GetFoldParent( GetCurrentLine() ) );
}

void StcTextEditor::OnSetOverType( wxCommandEvent & WXUNUSED( p_event ) )
{
	SetOvertype( ! GetOvertype() );
}

void StcTextEditor::OnSetReadOnly( wxCommandEvent & WXUNUSED( p_event ) )
{
	SetReadOnly( ! GetReadOnly() );
}

void StcTextEditor::OnWrapmodeOn( wxCommandEvent & WXUNUSED( p_event ) )
{
	SetWrapMode( GetWrapMode() == 0 ? wxSTC_WRAP_WORD : wxSTC_WRAP_NONE );
}

void StcTextEditor::OnUseCharset( wxCommandEvent & p_event )
{
	int l_iCharset = GetCodePage();

	switch ( p_event.GetId() )
	{
	case gcID_CHARSETANSI:
		l_iCharset = wxSTC_CHARSET_ANSI;
		break;

	case gcID_CHARSETMAC:
		l_iCharset = wxSTC_CHARSET_ANSI;
		break;
	}

	for ( int i = 0; i < wxSTC_STYLE_LASTPREDEFINED; ++i )
	{
		StyleSetCharacterSet( i, l_iCharset );
	}

	SetCodePage( l_iCharset );
}

void StcTextEditor::OnChangeCase( wxCommandEvent & p_event )
{
	switch ( p_event.GetId() )
	{
	case gcID_CHANGELOWER:
		CmdKeyExecute( wxSTC_CMD_LOWERCASE );
		break;

	case gcID_CHANGEUPPER:
		CmdKeyExecute( wxSTC_CMD_UPPERCASE );
		break;
	}
}

void StcTextEditor::OnConvertEOL( wxCommandEvent & p_event )
{
	int l_iEolMode = GetEOLMode();

	switch ( p_event.GetId() )
	{
	case gcID_CONVERTCR:
		l_iEolMode = wxSTC_EOL_CR;
		break;

	case gcID_CONVERTCRLF:
		l_iEolMode = wxSTC_EOL_CRLF;
		break;

	case gcID_CONVERTLF:
		l_iEolMode = wxSTC_EOL_LF;
		break;
	}

	ConvertEOLs( l_iEolMode );
	SetEOLMode( l_iEolMode );
}

void StcTextEditor::OnMarginClick( wxStyledTextEvent & p_event )
{
	if ( p_event.GetMargin() == 2 )
	{
		int l_iLineClick = LineFromPosition( p_event.GetPosition() );
		int l_iLevelClick = GetFoldLevel( l_iLineClick );

		if ( ( l_iLevelClick & wxSTC_FOLDLEVELHEADERFLAG ) > 0 )
		{
			ToggleFold( l_iLineClick );
		}
	}
}

void StcTextEditor::OnCharAdded( wxStyledTextEvent & p_event )
{
	char l_chr = ( char )p_event.GetKey();
	int l_iCurrentLine = GetCurrentLine();

	if ( l_chr == '\n' )
	{
		int l_iLineInd = 0;

		if ( l_iCurrentLine > 0 )
		{
			l_iLineInd = GetLineIndentation( l_iCurrentLine - 1 );
		}

		if ( l_iLineInd != 0 )
		{
			SetLineIndentation( l_iCurrentLine, l_iLineInd );
			GotoPos( PositionFromLine( l_iCurrentLine ) + l_iLineInd );
		}
	}
}

//*************************************************************************************************
