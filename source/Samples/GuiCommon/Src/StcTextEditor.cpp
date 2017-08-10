#include "StcTextEditor.hpp"
#include "LanguageInfo.hpp"
#include "StyleInfo.hpp"

#include <wx/filename.h>

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
				wxSTC_C_DEFAULT,		//eSTC_TYPE_DEFAULT
				wxSTC_C_WORD,			//eSTC_TYPE_WORD1
				wxSTC_C_WORD2,			//eSTC_TYPE_WORD2
				wxSTC_C_DEFAULT,		//eSTC_TYPE_WORD3
				wxSTC_C_GLOBALCLASS,	//eSTC_TYPE_WORD4
				wxSTC_C_DEFAULT,		//eSTC_TYPE_WORD5
				wxSTC_C_DEFAULT,		//eSTC_TYPE_WORD6
				wxSTC_C_COMMENT,		//eSTC_TYPE_COMMENT
				wxSTC_C_COMMENTDOC,		//eSTC_TYPE_COMMENT_DOC
				wxSTC_C_COMMENTLINE,	//eSTC_TYPE_COMMENT_LINE
				wxSTC_C_DEFAULT,		//eSTC_TYPE_COMMENT_SPECIAL
				wxSTC_C_CHARACTER,		//eSTC_TYPE_CHARACTER
				wxSTC_C_DEFAULT,		//eSTC_TYPE_CHARACTER_EOL
				wxSTC_C_STRING,			//eSTC_TYPE_STRING
				wxSTC_C_STRINGEOL,		//eSTC_TYPE_STRING_EOL
				wxSTC_C_DEFAULT,		//eSTC_TYPE_DELIMITER
				wxSTC_C_DEFAULT,		//eSTC_TYPE_PUNCTUATION
				wxSTC_C_OPERATOR,		//eSTC_TYPE_OPERATOR
				wxSTC_C_DEFAULT,		//eSTC_TYPE_BRACE
				wxSTC_C_DEFAULT,		//eSTC_TYPE_COMMAND
				wxSTC_C_IDENTIFIER,		//eSTC_TYPE_IDENTIFIER
				wxSTC_C_DEFAULT,		//eSTC_TYPE_LABEL
				wxSTC_C_NUMBER,			//eSTC_TYPE_NUMBER
				wxSTC_C_DEFAULT,		//eSTC_TYPE_PARAMETER
				wxSTC_C_REGEX,			//eSTC_TYPE_REGEX
				wxSTC_C_UUID,			//eSTC_TYPE_UUID
				wxSTC_C_DEFAULT,		//eSTC_TYPE_VALUE
				wxSTC_C_PREPROCESSOR,	//eSTC_TYPE_PREPROCESSOR
				wxSTC_C_DEFAULT,		//eSTC_TYPE_SCRIPT
				wxSTC_C_DEFAULT,		//eSTC_TYPE_ERROR
				wxSTC_C_DEFAULT,		//eSTC_TYPE_UNDEFINED
			};
			memcpy( m_iStyles, iStyles, sizeof( iStyles ) );
		}
	}

	inline int operator []( uint32_t p_index )
	{
		return m_iStyles[p_index];
	}
};

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

StcTextEditor::StcTextEditor( StcContext & p_context, wxWindow * p_parent, wxWindowID p_id, wxPoint const & p_ptPos, wxSize const & p_size, long p_lStyle )
	:	wxStyledTextCtrl( p_parent, p_id, p_ptPos, p_size, p_lStyle )
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
	wxFont font( 10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL );
	SetTabWidth( m_iTabSpaces );
	SetSelAlpha( 127 );
	SetSelBackground( true, wxColour( 51, 153, 255, 127 ) );
	SetViewEOL( m_context.getDisplayEOLEnable() );
	SetIndentationGuides( m_context.getIndentGuideEnable() );
	SetEdgeMode( m_context.getLongLineOnEnable() ? wxSTC_EDGE_LINE : wxSTC_EDGE_NONE );
	SetViewWhiteSpace( m_context.getWhiteSpaceEnable() ? wxSTC_WS_VISIBLEALWAYS : wxSTC_WS_INVISIBLE );
	SetOvertype( m_context.getOverTypeInitial() );
	SetReadOnly( m_context.getReadOnlyInitial() );
	SetWrapMode( m_context.getWrapModeInitial() ? wxSTC_WRAP_WORD : wxSTC_WRAP_NONE );
	StyleSetFont( wxSTC_STYLE_DEFAULT, font );
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
	bool result = false;
#if wxUSE_FILEDLG

	if ( ! m_strFilename )
	{
		wxFileDialog dlg( this, _( "open file" ), wxEmptyString, wxEmptyString, _( "Any file (*)|*" ), wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_CHANGE_DIR );

		if ( dlg.ShowModal() != wxID_OK )
		{
			return false;
		}

		m_strFilename = dlg.GetPath();
	}

	result = LoadFile( m_strFilename );
#endif
	return result;
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
	wxFileName fileName( m_strFilename );
	InitializePrefs( DeterminePrefs( fileName.GetFullName() ) );
	return true;
}

void StcTextEditor::setText( wxString const & p_strSource )
{
	wxStyledTextCtrl::ClearAll();
	wxStyledTextCtrl::SetEOLMode( wxSTC_EOL_LF );
	wxStyledTextCtrl::SetText( p_strSource );
	wxStyledTextCtrl::ConvertEOLs( wxSTC_EOL_LF );
	wxStyledTextCtrl::EmptyUndoBuffer();
}

bool StcTextEditor::SaveFile()
{
	bool result = false;
#if wxUSE_FILEDLG

	if ( isModified() )
	{
		if ( !m_strFilename )
		{
			wxFileDialog dlg( this, _( "Save file" ), wxEmptyString, wxEmptyString, _( "Any file (*)|*" ), wxFD_SAVE | wxFD_OVERWRITE_PROMPT );

			if ( dlg.ShowModal() != wxID_OK )
			{
				result = false;
			}
			else
			{
				result = true;
				m_strFilename = dlg.GetPath();
			}
		}
		else
		{
			result = true;
		}

		if ( result )
		{
			result = SaveFile( m_strFilename );
		}
	}
	else
	{
		result = true;
	}

#endif
	return result;
}

bool StcTextEditor::SaveFile( wxString const & p_strFilename )
{
	bool result = true;

	if ( isModified() )
	{
		result = wxStyledTextCtrl::SaveFile( p_strFilename );
	}

	return result;
}

bool StcTextEditor::isModified()
{
	return ( GetModify() && !GetReadOnly() );
}

wxString StcTextEditor::DeterminePrefs( wxString const & p_strFilename )
{
	LanguageInfoPtr pCurInfo;
	wxString strReturn;
	wxString strFilepattern;
	wxString strCur;

	for ( LanguageInfoPtrArrayConstIt it = m_context.begin(); it != m_context.end() && strReturn.empty(); ++it )
	{
		pCurInfo = * it;
		strFilepattern = pCurInfo->getFilePattern();
		strFilepattern.Lower();

		while ( ! strFilepattern.empty() && strReturn.empty() )
		{
			strCur = strFilepattern.BeforeFirst( ';' );

			if ( ( strCur == p_strFilename ) || ( strCur == ( p_strFilename.BeforeLast( '.' ) + wxT( ".*" ) ) ) || ( strCur == ( wxT( "*." ) + p_strFilename.AfterLast( '.' ) ) ) )
			{
				strReturn = pCurInfo->getName();
			}
			else
			{
				strFilepattern = strFilepattern.AfterFirst( ';' );
			}
		}
	}

	return strReturn;
}

bool StcTextEditor::InitializePrefs( wxString const & p_name )
{
	StyleClearAll();
	LanguageInfoPtr pCurInfo;
	bool bFound = false;

	for ( LanguageInfoPtrArrayConstIt it = m_context.begin(); it != m_context.end() && ! bFound; ++it )
	{
		pCurInfo = * it;

		if ( pCurInfo->getName().c_str() == p_name )
		{
			bFound = true;
		}
	}

	if ( bFound )
	{
		wxString defaultFgColour = pCurInfo->getStyle( eSTC_TYPE_DEFAULT )->getForeground();
		wxString defaultBgColour = pCurInfo->getStyle( eSTC_TYPE_DEFAULT )->getBackground();
		// set lexer and language
		SetLexer( pCurInfo->getLexerID() );
		m_pLanguage = pCurInfo;
		// set margin for line numbers
		SetMarginType( m_iLineNrID, wxSTC_MARGIN_NUMBER );
		SetMarginWidth( m_iLineNrID, 50 );

		// default fonts for all styles!
		for ( int i = 0; i < wxSTC_STYLE_LASTPREDEFINED; i++ )
		{
			wxFont font( 10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL );
			StyleSetFont( i, font );
		}

		// set common styles
		StyleSetForeground( wxSTC_STYLE_LINENUMBER, defaultFgColour );
		StyleSetBackground( wxSTC_STYLE_LINENUMBER, defaultBgColour );
		StyleSetForeground( wxSTC_STYLE_DEFAULT, defaultFgColour );
		StyleSetBackground( wxSTC_STYLE_DEFAULT, defaultBgColour );
		StyleSetForeground( wxSTC_STYLE_CONTROLCHAR, defaultFgColour );
		StyleSetBackground( wxSTC_STYLE_CONTROLCHAR, defaultBgColour );
		StyleSetForeground( wxSTC_STYLE_BRACELIGHT, wxColour( wxT( "DARK GREEN" ) ) );
		StyleSetBackground( wxSTC_STYLE_BRACELIGHT, defaultBgColour );
		StyleSetForeground( wxSTC_STYLE_BRACEBAD, wxColour( wxT( "DARK RED" ) ) );
		StyleSetBackground( wxSTC_STYLE_BRACEBAD, defaultBgColour );
		StyleSetBackground( wxSTC_STYLE_INDENTGUIDE, defaultBgColour );
		StyleSetBackground( wxSTC_STYLE_CALLTIP, defaultBgColour );
		StyleSetBackground( wxSTC_STYLE_LASTPREDEFINED, defaultBgColour );
		MarkerSetBackground( wxSTC_MARKNUM_FOLDER, defaultBgColour );
		MarkerSetForeground( wxSTC_MARKNUM_FOLDER, defaultFgColour );
		MarkerSetBackground( wxSTC_MARKNUM_FOLDEROPEN, defaultBgColour );
		MarkerSetForeground( wxSTC_MARKNUM_FOLDEROPEN, defaultFgColour );
		MarkerSetBackground( wxSTC_MARKNUM_FOLDERSUB, defaultBgColour );
		MarkerSetForeground( wxSTC_MARKNUM_FOLDERSUB, defaultFgColour );
		MarkerSetBackground( wxSTC_MARKNUM_FOLDEREND, defaultBgColour );
		MarkerSetForeground( wxSTC_MARKNUM_FOLDEREND, defaultFgColour );
		MarkerSetBackground( wxSTC_MARKNUM_FOLDEROPENMID, defaultBgColour );
		MarkerSetForeground( wxSTC_MARKNUM_FOLDEROPENMID, defaultFgColour );
		MarkerSetBackground( wxSTC_MARKNUM_FOLDERMIDTAIL, defaultBgColour );
		MarkerSetForeground( wxSTC_MARKNUM_FOLDERMIDTAIL, defaultFgColour );
		MarkerSetBackground( wxSTC_MARKNUM_FOLDERTAIL, defaultBgColour );
		MarkerSetForeground( wxSTC_MARKNUM_FOLDERTAIL, defaultFgColour );
		wxArrayString keywords;

		// initialize settings
		if ( m_context.getSyntaxEnable() )
		{
			int iNbKeywords = 0;
			LexerStylesAssoc lexerAssoc( pCurInfo->getLexerID() );

			for ( int j = eSTC_TYPE_DEFAULT; j < eSTC_TYPE_COUNT; ++j )
			{
				eSTC_TYPE type = eSTC_TYPE( j );
				int iStyle = lexerAssoc[type];
				StyleInfoPtr const & stCurType = pCurInfo->getStyle( type );
				wxFont font( stCurType->getFontSize(), wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, stCurType->getFontName() );
				StyleSetFont( iStyle, font );

				if ( !stCurType->getForeground().empty() )
				{
					StyleSetForeground( iStyle, stCurType->getForeground() );
				}

				if ( !stCurType->getBackground().empty() )
				{
					StyleSetBackground( iStyle, stCurType->getBackground() );
				}

				StyleSetBold( iStyle, ( stCurType->getFontStyle() & eSTC_STYLE_BOLD ) >  0 );
				StyleSetItalic( iStyle, ( stCurType->getFontStyle() & eSTC_STYLE_ITALIC ) >  0 );
				StyleSetUnderline( iStyle, ( stCurType->getFontStyle() & eSTC_STYLE_UNDERL ) >  0 );
				StyleSetVisible( iStyle, ( stCurType->getFontStyle() & eSTC_STYLE_HIDDEN ) == 0 );
				StyleSetCase( iStyle, ( stCurType->getLetterCase() ) );

				if ( ! pCurInfo->getWords( type ).empty() )
				{
					castor::String words = pCurInfo->getWords( type );
					SetKeyWords( iNbKeywords, words.c_str() );
					castor::StringArray array = castor::string::split( words, cuT( " \t\n\r" ), -1, false );

					for ( castor::StringArray::iterator it = array.begin(); it != array.end(); ++it )
					{
						keywords.push_back( *it );
					}

					++iNbKeywords;
				}
			}
		}

#if wxMAJOR_VERSION >= 3 || ( wxMAJOR_VERSION == 2 && wxMINOR_VERSION >= 9 )
		AutoComplete( new TextAutoCompleter( keywords ) );
#endif
		// set margin as unused
		SetMarginType( m_iDividerID, wxSTC_MARGIN_SYMBOL );
		SetMarginWidth( m_iDividerID, 0 );
		SetMarginSensitive( m_iDividerID, false );
		// folding
		SetMarginType( m_iFoldingID, wxSTC_MARGIN_SYMBOL );
		SetMarginMask( m_iFoldingID, wxSTC_MASK_FOLDERS );
		StyleSetBackground( m_iFoldingID, defaultBgColour );
		SetMarginWidth( m_iFoldingID, 0 );
		SetMarginSensitive( m_iFoldingID, false );

		if ( m_context.getFoldEnable() )
		{
			SetMarginWidth( m_iFoldingID, ( ( pCurInfo->getFoldFlags() != 0 ) ? m_iFoldingMargin : 0 ) );
			SetMarginSensitive( m_iFoldingID, ( ( pCurInfo->getFoldFlags() != 0 ) ) );
			SetProperty( wxT( "fold" ), ( ( pCurInfo->getFoldFlags() != 0 ) ? wxT( "1" ) : wxT( "0" ) ) );
			SetProperty( wxT( "fold.comment" ), ( ( pCurInfo->getFoldFlags() & eSTC_FOLD_COMMENT ) > 0 ? wxT( "1" ) : wxT( "0" ) ) );
			SetProperty( wxT( "fold.compact" ), ( ( pCurInfo->getFoldFlags() & eSTC_FOLD_COMPACT ) > 0 ? wxT( "1" ) : wxT( "0" ) ) );
			SetProperty( wxT( "fold.preprocessor" ), ( ( pCurInfo->getFoldFlags() & eSTC_FOLD_PREPROC ) > 0	? wxT( "1" ) : wxT( "0" ) ) );
			SetProperty( wxT( "fold.html" ), ( ( pCurInfo->getFoldFlags() & eSTC_FOLD_HTML ) > 0	? wxT( "1" ) : wxT( "0" ) ) );
			SetProperty( wxT( "fold.html.preprocessor" ), ( ( pCurInfo->getFoldFlags() & eSTC_FOLD_HTMLPREP ) > 0	? wxT( "1" ) : wxT( "0" ) ) );
			SetProperty( wxT( "fold.comment.python" ), ( ( pCurInfo->getFoldFlags() & eSTC_FOLD_COMMENTPY ) > 0	? wxT( "1" ) : wxT( "0" ) ) );
			SetProperty( wxT( "fold.quotes.python" ), ( ( pCurInfo->getFoldFlags() & eSTC_FOLD_QUOTESPY ) > 0	? wxT( "1" ) : wxT( "0" ) ) );
		}

		SetFoldFlags( wxSTC_FOLDFLAG_LINEBEFORE_CONTRACTED | wxSTC_FOLDFLAG_LINEAFTER_CONTRACTED );
		// set spaces and indention
		SetTabWidth( m_iTabSpaces );
		SetUseTabs( m_bUseTabs );
		SetTabIndents( m_bTabIndents );
		SetBackSpaceUnIndents( m_bBackspaceUnindents );
		SetIndent( m_context.getIndentEnable() ? m_iTabSpaces : 0 );
		// others
		SetViewEOL( m_context.getDisplayEOLEnable() );
		SetIndentationGuides( m_context.getIndentGuideEnable() );
		SetEdgeColumn( 80 );
		SetEdgeMode( m_context.getLongLineOnEnable() ? wxSTC_EDGE_LINE : wxSTC_EDGE_NONE );
		SetViewWhiteSpace( m_context.getWhiteSpaceEnable() ? wxSTC_WS_VISIBLEALWAYS : wxSTC_WS_INVISIBLE );
		SetOvertype( m_context.getOverTypeInitial() );
		SetReadOnly( m_context.getReadOnlyInitial() );
		SetWrapMode( m_context.getWrapModeInitial() ? wxSTC_WRAP_WORD : wxSTC_WRAP_NONE );
	}

	return bFound;
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
	EVT_MENU( gcID_OVERTYPE, StcTextEditor::OnsetOverType )
	EVT_MENU( gcID_READONLY, StcTextEditor::OnsetReadOnly )
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
	int x = GetClientSize().x + ( m_context.getLineNumberEnable() ? m_iLineNrMargin : 0 ) + ( m_context.getFoldEnable() ? m_iFoldingMargin : 0 );

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
	if ( !GetReadOnly() )
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
	if ( !GetReadOnly() && ( GetSelectionEnd() - GetSelectionStart() ) > 0 )
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
	int iMin = GetCurrentPos();
	int iMax = BraceMatch( iMin );

	if ( iMax > ( iMin + 1 ) )
	{
		BraceHighlight( iMin + 1, iMax );
		SetSelection( iMin + 1, iMax );
	}
	else
	{
		BraceBadLight( iMin );
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
	int iLineStart = PositionFromLine( GetCurrentLine() );
	int iLineEnd = PositionFromLine( GetCurrentLine() + 1 );
	SetSelection( iLineStart, iLineEnd );
}

void StcTextEditor::OnHilightLang( wxCommandEvent & p_event )
{
	InitializePrefs( ( *( m_context.begin() + ( p_event.GetId() - gcID_HILIGHTFIRST ) ) )->getName() );
}

void StcTextEditor::OnDisplayEOL( wxCommandEvent & WXUNUSED( p_event ) )
{
	SetViewEOL( !GetViewEOL() );
}

void StcTextEditor::OnIndentGuide( wxCommandEvent & WXUNUSED( p_event ) )
{
	SetIndentationGuides( !GetIndentationGuides() );
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

void StcTextEditor::OnsetOverType( wxCommandEvent & WXUNUSED( p_event ) )
{
	SetOvertype( !GetOvertype() );
}

void StcTextEditor::OnsetReadOnly( wxCommandEvent & WXUNUSED( p_event ) )
{
	SetReadOnly( !GetReadOnly() );
}

void StcTextEditor::OnWrapmodeOn( wxCommandEvent & WXUNUSED( p_event ) )
{
	SetWrapMode( GetWrapMode() == 0 ? wxSTC_WRAP_WORD : wxSTC_WRAP_NONE );
}

void StcTextEditor::OnUseCharset( wxCommandEvent & p_event )
{
	int iCharset = GetCodePage();

	switch ( p_event.GetId() )
	{
	case gcID_CHARSETANSI:
		iCharset = wxSTC_CHARSET_ANSI;
		break;

	case gcID_CHARSETMAC:
		iCharset = wxSTC_CHARSET_ANSI;
		break;
	}

	for ( int i = 0; i < wxSTC_STYLE_LASTPREDEFINED; ++i )
	{
		StyleSetCharacterSet( i, iCharset );
	}

	SetCodePage( iCharset );
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
	int iEolMode = GetEOLMode();

	switch ( p_event.GetId() )
	{
	case gcID_CONVERTCR:
		iEolMode = wxSTC_EOL_CR;
		break;

	case gcID_CONVERTCRLF:
		iEolMode = wxSTC_EOL_CRLF;
		break;

	case gcID_CONVERTLF:
		iEolMode = wxSTC_EOL_LF;
		break;
	}

	ConvertEOLs( iEolMode );
	SetEOLMode( iEolMode );
}

void StcTextEditor::OnMarginClick( wxStyledTextEvent & p_event )
{
	if ( p_event.GetMargin() == 2 )
	{
		int iLineClick = LineFromPosition( p_event.GetPosition() );
		int iLevelClick = GetFoldLevel( iLineClick );

		if ( ( iLevelClick & wxSTC_FOLDLEVELHEADERFLAG ) > 0 )
		{
			ToggleFold( iLineClick );
		}
	}
}

void StcTextEditor::OnCharAdded( wxStyledTextEvent & p_event )
{
	char chr = ( char )p_event.GetKey();
	int iCurrentLine = GetCurrentLine();

	if ( chr == '\n' )
	{
		int iLineInd = 0;

		if ( iCurrentLine > 0 )
		{
			iLineInd = GetLineIndentation( iCurrentLine - 1 );
		}

		if ( iLineInd != 0 )
		{
			SetLineIndentation( iCurrentLine, iLineInd );
			GotoPos( PositionFromLine( iCurrentLine ) + iLineInd );
		}
	}
}

//*************************************************************************************************
