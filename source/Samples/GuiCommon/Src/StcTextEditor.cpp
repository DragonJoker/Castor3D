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
				,	wxSTC_C_WORD			//eSTC_TYPE_WORD1
				,	wxSTC_C_WORD2			//eSTC_TYPE_WORD2
				,	wxSTC_C_DEFAULT			//eSTC_TYPE_WORD3
				,	wxSTC_C_GLOBALCLASS		//eSTC_TYPE_WORD4
				,	wxSTC_C_DEFAULT			//eSTC_TYPE_WORD5
				,	wxSTC_C_DEFAULT			//eSTC_TYPE_WORD6
				,	wxSTC_C_COMMENT			//eSTC_TYPE_COMMENT
				,	wxSTC_C_COMMENTDOC		//eSTC_TYPE_COMMENT_DOC
				,	wxSTC_C_COMMENTLINE		//eSTC_TYPE_COMMENT_LINE
				,	wxSTC_C_DEFAULT			//eSTC_TYPE_COMMENT_SPECIAL
				,	wxSTC_C_CHARACTER		//eSTC_TYPE_CHARACTER
				,	wxSTC_C_DEFAULT			//eSTC_TYPE_CHARACTER_EOL
				,	wxSTC_C_STRING			//eSTC_TYPE_STRING
				,	wxSTC_C_STRINGEOL		//eSTC_TYPE_STRING_EOL
				,	wxSTC_C_DEFAULT			//eSTC_TYPE_DELIMITER
				,	wxSTC_C_DEFAULT			//eSTC_TYPE_PUNCTUATION
				,	wxSTC_C_OPERATOR		//eSTC_TYPE_OPERATOR
				,	wxSTC_C_DEFAULT			//eSTC_TYPE_BRACE
				,	wxSTC_C_DEFAULT			//eSTC_TYPE_COMMAND
				,	wxSTC_C_IDENTIFIER		//eSTC_TYPE_IDENTIFIER
				,	wxSTC_C_DEFAULT			//eSTC_TYPE_LABEL
				,	wxSTC_C_NUMBER			//eSTC_TYPE_NUMBER
				,	wxSTC_C_DEFAULT			//eSTC_TYPE_PARAMETER
				,	wxSTC_C_REGEX			//eSTC_TYPE_REGEX
				,	wxSTC_C_UUID			//eSTC_TYPE_UUID
				,	wxSTC_C_DEFAULT			//eSTC_TYPE_VALUE
				,	wxSTC_C_PREPROCESSOR	//eSTC_TYPE_PREPROCESSOR
				,	wxSTC_C_DEFAULT			//eSTC_TYPE_SCRIPT
				,	wxSTC_C_DEFAULT			//eSTC_TYPE_ERROR
				,	wxSTC_C_DEFAULT			//eSTC_TYPE_UNDEFINED
			};
			memcpy( m_iStyles, iStyles,	sizeof( iStyles	) );
		}
	}

	inline int operator []( uint32_t p_uiIndex )
	{
		return m_iStyles[p_uiIndex];
	}
};

//*************************************************************************************************

#if wxMAJOR_VERSION >= 3 || ( wxMAJOR_VERSION == 2 && wxMINOR_VERSION >= 9 )
wxStcTextEditor::wxTextAutoCompleter::wxTextAutoCompleter( wxArrayString const & p_keywords )
{
	for ( wxArrayString::const_iterator l_it = p_keywords.begin(); l_it != p_keywords.end(); ++l_it )
	{
		m_keywords.insert( *l_it );
	}
}

bool wxStcTextEditor::wxTextAutoCompleter::Start( wxString const & p_prefix )
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

wxString wxStcTextEditor::wxTextAutoCompleter::GetNext()
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

wxStcTextEditor::wxStcTextEditor( StcContext * p_pContext, wxWindow * p_pParent, wxWindowID p_id, wxPoint const & p_ptPos, wxSize const & p_size, long p_lStyle )
	:	wxStyledTextCtrl( p_pParent, p_id, p_ptPos, p_size, p_lStyle )
	,	m_pContext( p_pContext )
	,	m_strFilename( wxEmptyString )
	,	m_iLineNrID( 0 )
	,	m_iDividerID( 1 )
	,	m_iFoldingID( 2 )
	,	m_pLanguage()
	,	m_iTabSpaces( 4 )
	,	m_bUseTabs( true )
	,	m_bTabIndents( true )
	,	m_bBackspaceUnindents( true )
{
	wxFont l_font( 10, wxMODERN, wxNORMAL, wxNORMAL );
	SetTabWidth( m_iTabSpaces );
	SetSelAlpha( 127 );
	SetSelBackground( true, wxColour( 51, 153, 255, 127 ) );
	SetViewEOL(	p_pContext->GetDisplayEOLEnable() );
	SetIndentationGuides(	p_pContext->GetIndentGuideEnable() );
	SetEdgeMode(	p_pContext->GetLongLineOnEnable() ? wxSTC_EDGE_LINE : wxSTC_EDGE_NONE );
	SetViewWhiteSpace(	p_pContext->GetWhiteSpaceEnable() ? wxSTC_WS_VISIBLEALWAYS : wxSTC_WS_INVISIBLE );
	SetOvertype(	p_pContext->GetOverTypeInitial() );
	SetReadOnly(	p_pContext->GetReadOnlyInitial() );
	SetWrapMode(	p_pContext->GetWrapModeInitial() ? wxSTC_WRAP_WORD : wxSTC_WRAP_NONE );
	StyleSetFont( wxSTC_STYLE_DEFAULT, l_font );
	InitializePrefs( DEFAULT_LANGUAGE );
	// set visibility
	SetVisiblePolicy(	wxSTC_VISIBLE_STRICT | wxSTC_VISIBLE_SLOP,					1 );
	SetXCaretPolicy(	wxSTC_CARET_EVEN | wxSTC_VISIBLE_STRICT | wxSTC_CARET_SLOP,	1 );
	SetYCaretPolicy(	wxSTC_CARET_EVEN | wxSTC_VISIBLE_STRICT | wxSTC_CARET_SLOP,	1 );
	// markers
	MarkerDefine( wxSTC_MARKNUM_FOLDER,			wxSTC_MARK_DOTDOTDOT,	wxT( "BLACK" ), wxT( "BLACK" ) );
	MarkerDefine( wxSTC_MARKNUM_FOLDEROPEN,		wxSTC_MARK_ARROWDOWN,	wxT( "BLACK" ), wxT( "BLACK" ) );
	MarkerDefine( wxSTC_MARKNUM_FOLDERSUB,		wxSTC_MARK_EMPTY,		wxT( "BLACK" ), wxT( "BLACK" ) );
	MarkerDefine( wxSTC_MARKNUM_FOLDEREND,		wxSTC_MARK_DOTDOTDOT,	wxT( "BLACK" ), wxT( "WHITE" ) );
	MarkerDefine( wxSTC_MARKNUM_FOLDEROPENMID,	wxSTC_MARK_ARROWDOWN,	wxT( "BLACK" ), wxT( "WHITE" ) );
	MarkerDefine( wxSTC_MARKNUM_FOLDERMIDTAIL,	wxSTC_MARK_EMPTY,		wxT( "BLACK" ), wxT( "BLACK" ) );
	MarkerDefine( wxSTC_MARKNUM_FOLDERTAIL,		wxSTC_MARK_EMPTY,		wxT( "BLACK" ), wxT( "BLACK" ) );
	// miscellaneous
	m_iLineNrMargin = TextWidth( wxSTC_STYLE_LINENUMBER, wxT( "_999999" ) );
	m_iFoldingMargin = 16;
	CmdKeyClear( wxSTC_KEY_TAB, 0 ); // this is done by the menu accelerator key
	SetLayoutCache( wxSTC_CACHE_PAGE );
}

wxStcTextEditor::~wxStcTextEditor()
{
}

bool wxStcTextEditor::LoadFile()
{
	bool l_bReturn = false;
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

	l_bReturn = LoadFile( m_strFilename );
#endif
	return l_bReturn;
}

bool wxStcTextEditor::LoadFile( wxString const & p_strFilename )
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

void wxStcTextEditor::SetText( wxString const & p_strSource )
{
	wxStyledTextCtrl::ClearAll();
	wxStyledTextCtrl::SetEOLMode( wxSTC_EOL_LF );
	wxStyledTextCtrl::SetText( p_strSource );
	wxStyledTextCtrl::ConvertEOLs( wxSTC_EOL_LF );
	wxStyledTextCtrl::EmptyUndoBuffer();
}

bool wxStcTextEditor::SaveFile()
{
	bool l_bReturn = false;
#if wxUSE_FILEDLG

	if ( IsModified() )
	{
		if ( !m_strFilename )
		{
			wxFileDialog l_dlg( this, _( "Save file" ), wxEmptyString, wxEmptyString, _( "Any file (*)|*" ), wxFD_SAVE | wxFD_OVERWRITE_PROMPT );

			if ( l_dlg.ShowModal() != wxID_OK )
			{
				l_bReturn = false;
			}
			else
			{
				l_bReturn = true;
				m_strFilename = l_dlg.GetPath();
			}
		}
		else
		{
			l_bReturn = true;
		}

		if ( l_bReturn )
		{
			l_bReturn = SaveFile( m_strFilename );
		}
	}
	else
	{
		l_bReturn = true;
	}

#endif
	return l_bReturn;
}

bool wxStcTextEditor::SaveFile( wxString const & p_strFilename )
{
	bool l_bReturn = true;

	if ( IsModified() )
	{
		l_bReturn = wxStyledTextCtrl::SaveFile( p_strFilename );
	}

	return l_bReturn;
}

bool wxStcTextEditor::IsModified()
{
	return ( GetModify() && ! GetReadOnly() );
}

wxString wxStcTextEditor::DeterminePrefs( wxString const & p_strFilename )
{
	LanguageInfoPtr l_pCurInfo;
	wxString l_strReturn;
	wxString l_strFilepattern;
	wxString l_strCur;

	for ( LanguageInfoPtrArrayConstIt l_it = m_pContext->Begin(); l_it != m_pContext->End() && l_strReturn.empty(); ++l_it )
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

bool wxStcTextEditor::InitializePrefs( wxString const & p_strName )
{
	StyleClearAll();
	LanguageInfoPtr l_pCurInfo;
	bool l_bFound = false;

	for ( LanguageInfoPtrArrayConstIt l_it = m_pContext->Begin(); l_it != m_pContext->End() && ! l_bFound; ++l_it )
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
		if ( m_pContext->GetSyntaxEnable() )
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
					StyleSetForeground(	l_iStyle, l_stCurType->GetForeground() );
				}

				if ( !l_stCurType->GetBackground().empty() )
				{
					StyleSetBackground(	l_iStyle, l_stCurType->GetBackground() );
				}

				StyleSetBold( l_iStyle, ( l_stCurType->GetFontStyle() & eSTC_STYLE_BOLD	) >  0 );
				StyleSetItalic( l_iStyle, ( l_stCurType->GetFontStyle() & eSTC_STYLE_ITALIC	) >  0 );
				StyleSetUnderline( l_iStyle, ( l_stCurType->GetFontStyle() & eSTC_STYLE_UNDERL	) >  0 );
				StyleSetVisible( l_iStyle, ( l_stCurType->GetFontStyle() & eSTC_STYLE_HIDDEN	) == 0 );
				StyleSetCase( l_iStyle, ( l_stCurType->GetLetterCase() ) );

				if ( ! l_pCurInfo->GetWords( l_eType ).empty() )
				{
					Castor::String l_words = l_pCurInfo->GetWords( l_eType );
					SetKeyWords( l_iNbKeywords, l_words.c_str() );
					Castor::StringArray l_array = Castor::str_utils::split( l_words, cuT( " \t\n\r" ), -1, false );

					for ( Castor::StringArray::iterator l_it = l_array.begin(); l_it != l_array.end(); ++l_it )
					{
						l_keywords.push_back( *l_it );
					}

					++l_iNbKeywords;
				}
			}
		}

#if wxMAJOR_VERSION >= 3 || ( wxMAJOR_VERSION == 2 && wxMINOR_VERSION >= 9 )
		AutoComplete( new wxTextAutoCompleter( l_keywords ) );
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

		if ( m_pContext->GetFoldEnable() )
		{
			SetMarginWidth(	m_iFoldingID,	( ( l_pCurInfo->GetFoldFlags() != 0 )						? m_iFoldingMargin : 0 )	);
			SetMarginSensitive(	m_iFoldingID,	( ( l_pCurInfo->GetFoldFlags() != 0 ) ) );
			SetProperty( wxT( "fold"	),	( ( l_pCurInfo->GetFoldFlags() != 0 )						? wxT( "1" ) : wxT( "0" ) )	);
			SetProperty( wxT( "fold.comment"	),	( ( l_pCurInfo->GetFoldFlags() & eSTC_FOLD_COMMENT )	> 0	? wxT( "1" ) : wxT( "0" ) )	);
			SetProperty( wxT( "fold.compact"	),	( ( l_pCurInfo->GetFoldFlags() & eSTC_FOLD_COMPACT )	> 0	? wxT( "1" ) : wxT( "0" ) )	);
			SetProperty( wxT( "fold.preprocessor"	),	( ( l_pCurInfo->GetFoldFlags() & eSTC_FOLD_PREPROC )	> 0	? wxT( "1" ) : wxT( "0" ) )	);
			SetProperty( wxT( "fold.html"	),	( ( l_pCurInfo->GetFoldFlags() & eSTC_FOLD_HTML )		> 0	? wxT( "1" ) : wxT( "0" ) )	);
			SetProperty( wxT( "fold.html.preprocessor"	),	( ( l_pCurInfo->GetFoldFlags() & eSTC_FOLD_HTMLPREP )	> 0	? wxT( "1" ) : wxT( "0" ) )	);
			SetProperty( wxT( "fold.comment.python"	),	( ( l_pCurInfo->GetFoldFlags() & eSTC_FOLD_COMMENTPY )	> 0	? wxT( "1" ) : wxT( "0" ) )	);
			SetProperty( wxT( "fold.quotes.python"	),	( ( l_pCurInfo->GetFoldFlags() & eSTC_FOLD_QUOTESPY )	> 0	? wxT( "1" ) : wxT( "0" ) )	);
		}

		SetFoldFlags( wxSTC_FOLDFLAG_LINEBEFORE_CONTRACTED | wxSTC_FOLDFLAG_LINEAFTER_CONTRACTED );
		// set spaces and indention
		SetTabWidth( m_iTabSpaces );
		SetUseTabs( m_bUseTabs );
		SetTabIndents( m_bTabIndents );
		SetBackSpaceUnIndents( m_bBackspaceUnindents );
		SetIndent( m_pContext->GetIndentEnable() ? m_iTabSpaces : 0 );
		// others
		SetViewEOL( m_pContext->GetDisplayEOLEnable() );
		SetIndentationGuides( m_pContext->GetIndentGuideEnable() );
		SetEdgeColumn( 80 );
		SetEdgeMode( m_pContext->GetLongLineOnEnable() ? wxSTC_EDGE_LINE : wxSTC_EDGE_NONE );
		SetViewWhiteSpace( m_pContext->GetWhiteSpaceEnable() ? wxSTC_WS_VISIBLEALWAYS : wxSTC_WS_INVISIBLE );
		SetOvertype( m_pContext->GetOverTypeInitial() );
		SetReadOnly( m_pContext->GetReadOnlyInitial() );
		SetWrapMode( m_pContext->GetWrapModeInitial() ? wxSTC_WRAP_WORD : wxSTC_WRAP_NONE );
	}

	return l_bFound;
}

BEGIN_EVENT_TABLE( wxStcTextEditor, wxStyledTextCtrl )
	EVT_SIZE(	wxStcTextEditor::OnSize	)
	EVT_MENU(	wxID_CLEAR,						wxStcTextEditor::OnEditClear	)
	EVT_MENU(	wxID_CUT,						wxStcTextEditor::OnEditCut	)
	EVT_MENU(	wxID_COPY,						wxStcTextEditor::OnEditCopy	)
	EVT_MENU(	wxID_PASTE,						wxStcTextEditor::OnEditPaste	)
	EVT_MENU(	gcID_INDENTINC,					wxStcTextEditor::OnEditIndentInc	)
	EVT_MENU(	gcID_INDENTRED,					wxStcTextEditor::OnEditIndentRed	)
	EVT_MENU(	wxID_SELECTALL,					wxStcTextEditor::OnEditSelectAll	)
	EVT_MENU(	gcID_SELECTLINE,				wxStcTextEditor::OnEditSelectLine	)
	EVT_MENU(	wxID_REDO,						wxStcTextEditor::OnEditRedo	)
	EVT_MENU(	wxID_UNDO,						wxStcTextEditor::OnEditUndo	)
	EVT_MENU(	wxID_FIND,						wxStcTextEditor::OnFind	)
	EVT_MENU(	gcID_FINDNEXT,					wxStcTextEditor::OnFindNext	)
	EVT_MENU(	gcID_REPLACE,					wxStcTextEditor::OnReplace	)
	EVT_MENU(	gcID_REPLACENEXT,				wxStcTextEditor::OnReplaceNext	)
	EVT_MENU(	gcID_BRACEMATCH,				wxStcTextEditor::OnBraceMatch	)
	EVT_MENU(	gcID_GOTO,						wxStcTextEditor::OnGoto	)
	EVT_MENU_RANGE(	gcID_HILIGHTFIRST, gcID_HILIGHTLAST,	wxStcTextEditor::OnHilightLang	)
	EVT_MENU(	gcID_DISPLAYEOL,				wxStcTextEditor::OnDisplayEOL	)
	EVT_MENU(	gcID_INDENTGUIDE,				wxStcTextEditor::OnIndentGuide	)
	EVT_MENU(	gcID_LINENUMBER,				wxStcTextEditor::OnLineNumber	)
	EVT_MENU(	gcID_LONGLINEON,				wxStcTextEditor::OnLongLineOn	)
	EVT_MENU(	gcID_WHITESPACE,				wxStcTextEditor::OnWhiteSpace	)
	EVT_MENU(	gcID_FOLDTOGGLE,				wxStcTextEditor::OnFoldToggle	)
	EVT_MENU(	gcID_OVERTYPE,					wxStcTextEditor::OnSetOverType	)
	EVT_MENU(	gcID_READONLY,					wxStcTextEditor::OnSetReadOnly	)
	EVT_MENU(	gcID_WRAPMODEON,				wxStcTextEditor::OnWrapmodeOn	)
	EVT_MENU(	gcID_CHARSETANSI,				wxStcTextEditor::OnUseCharset	)
	EVT_MENU(	gcID_CHARSETMAC,				wxStcTextEditor::OnUseCharset	)
	EVT_MENU(	gcID_CHANGELOWER,				wxStcTextEditor::OnChangeCase	)
	EVT_MENU(	gcID_CHANGEUPPER,				wxStcTextEditor::OnChangeCase	)
	EVT_MENU(	gcID_CONVERTCR,					wxStcTextEditor::OnConvertEOL	)
	EVT_MENU(	gcID_CONVERTCRLF,				wxStcTextEditor::OnConvertEOL	)
	EVT_MENU(	gcID_CONVERTLF,					wxStcTextEditor::OnConvertEOL	)
	EVT_STC_MARGINCLICK(	wxID_ANY,						wxStcTextEditor::OnMarginClick	)
	EVT_STC_CHARADDED(	wxID_ANY,						wxStcTextEditor::OnCharAdded	)
	EVT_STC_KEY(	wxID_ANY ,						wxStcTextEditor::OnKey	)
END_EVENT_TABLE()


void wxStcTextEditor::OnSize( wxSizeEvent & p_event )
{
	int x = GetClientSize().x + ( m_pContext->GetLineNumberEnable() ? m_iLineNrMargin : 0 ) + ( m_pContext->GetFoldEnable() ? m_iFoldingMargin : 0 );

	if ( x > 0 )
	{
		SetScrollWidth( x );
	}

	p_event.Skip();
}

void wxStcTextEditor::OnEditRedo( wxCommandEvent & WXUNUSED( p_event ) )
{
	if ( CanRedo() )
	{
		Redo();
	}
}

void wxStcTextEditor::OnEditUndo( wxCommandEvent & WXUNUSED( p_event ) )
{
	if ( CanUndo() )
	{
		Undo();
	}
}

void wxStcTextEditor::OnEditClear( wxCommandEvent & WXUNUSED( p_event ) )
{
	if ( ! GetReadOnly() )
	{
		Clear();
	}
}

void wxStcTextEditor::OnKey( wxStyledTextEvent & WXUNUSED( p_event ) )
{
	wxMessageBox( wxT( "OnKey" ) );
}

void wxStcTextEditor::OnEditCut( wxCommandEvent & WXUNUSED( p_event ) )
{
	if ( ! GetReadOnly() && ( GetSelectionEnd() - GetSelectionStart() ) > 0 )
	{
		Cut();
	}
}

void wxStcTextEditor::OnEditCopy( wxCommandEvent & WXUNUSED( p_event ) )
{
	if ( GetSelectionEnd() - GetSelectionStart() > 0 )
	{
		Copy();
	}
}

void wxStcTextEditor::OnEditPaste( wxCommandEvent & WXUNUSED( p_event ) )
{
	if ( CanPaste() )
	{
		Paste();
	}
}

void wxStcTextEditor::OnFind( wxCommandEvent & WXUNUSED( p_event ) )
{
}

void wxStcTextEditor::OnFindNext( wxCommandEvent & WXUNUSED( p_event ) )
{
}

void wxStcTextEditor::OnReplace( wxCommandEvent & WXUNUSED( p_event ) )
{
}

void wxStcTextEditor::OnReplaceNext( wxCommandEvent & WXUNUSED( p_event ) )
{
}

void wxStcTextEditor::OnBraceMatch( wxCommandEvent & WXUNUSED( p_event ) )
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

void wxStcTextEditor::OnGoto( wxCommandEvent & WXUNUSED( p_event ) )
{
}

void wxStcTextEditor::OnEditIndentInc( wxCommandEvent & WXUNUSED( p_event ) )
{
	CmdKeyExecute( wxSTC_CMD_TAB );
}

void wxStcTextEditor::OnEditIndentRed( wxCommandEvent & WXUNUSED( p_event ) )
{
	CmdKeyExecute( wxSTC_CMD_DELETEBACK );
}

void wxStcTextEditor::OnEditSelectAll( wxCommandEvent & WXUNUSED( p_event ) )
{
	SetSelection( 0, GetTextLength() );
}

void wxStcTextEditor::OnEditSelectLine( wxCommandEvent & WXUNUSED( p_event ) )
{
	int l_iLineStart = PositionFromLine( GetCurrentLine() );
	int l_iLineEnd = PositionFromLine( GetCurrentLine() + 1 );
	SetSelection( l_iLineStart, l_iLineEnd );
}

void wxStcTextEditor::OnHilightLang( wxCommandEvent & p_event )
{
	InitializePrefs( ( *( m_pContext->Begin() + ( p_event.GetId() - gcID_HILIGHTFIRST ) ) )->GetName() );
}

void wxStcTextEditor::OnDisplayEOL( wxCommandEvent & WXUNUSED( p_event ) )
{
	SetViewEOL( ! GetViewEOL() );
}

void wxStcTextEditor::OnIndentGuide( wxCommandEvent & WXUNUSED( p_event ) )
{
	SetIndentationGuides( ! GetIndentationGuides() );
}

void wxStcTextEditor::OnLineNumber( wxCommandEvent & WXUNUSED( p_event ) )
{
	SetMarginWidth( m_iLineNrID, GetMarginWidth( m_iLineNrID ) == 0 ? m_iLineNrMargin : 0 );
}

void wxStcTextEditor::OnLongLineOn( wxCommandEvent & WXUNUSED( p_event ) )
{
	SetEdgeMode( GetEdgeMode() == 0 ? wxSTC_EDGE_LINE : wxSTC_EDGE_NONE );
}

void wxStcTextEditor::OnWhiteSpace( wxCommandEvent & WXUNUSED( p_event ) )
{
	SetViewWhiteSpace( GetViewWhiteSpace() == 0 ? wxSTC_WS_VISIBLEALWAYS : wxSTC_WS_INVISIBLE );
}

void wxStcTextEditor::OnFoldToggle( wxCommandEvent & WXUNUSED( p_event ) )
{
	ToggleFold( GetFoldParent( GetCurrentLine() ) );
}

void wxStcTextEditor::OnSetOverType( wxCommandEvent & WXUNUSED( p_event ) )
{
	SetOvertype( ! GetOvertype() );
}

void wxStcTextEditor::OnSetReadOnly( wxCommandEvent & WXUNUSED( p_event ) )
{
	SetReadOnly( ! GetReadOnly() );
}

void wxStcTextEditor::OnWrapmodeOn( wxCommandEvent & WXUNUSED( p_event ) )
{
	SetWrapMode( GetWrapMode() == 0 ? wxSTC_WRAP_WORD : wxSTC_WRAP_NONE );
}

void wxStcTextEditor::OnUseCharset( wxCommandEvent & p_event )
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

void wxStcTextEditor::OnChangeCase( wxCommandEvent & p_event )
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

void wxStcTextEditor::OnConvertEOL( wxCommandEvent & p_event )
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

void wxStcTextEditor::OnMarginClick( wxStyledTextEvent & p_event )
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

void wxStcTextEditor::OnCharAdded( wxStyledTextEvent & p_event )
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
