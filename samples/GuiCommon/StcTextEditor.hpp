/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_STC_TEXT_EDITOR_H___
#define ___GUICOMMON_STC_TEXT_EDITOR_H___

#include <wx/stc/stc.h>
#if wxMAJOR_VERSION >= 3 || ( wxMAJOR_VERSION == 2 && wxMINOR_VERSION >= 9 )
#	include <wx/textcompleter.h>
#endif
#include <set>

#include "GuiCommon/GuiCommonPrerequisites.hpp"
#include "GuiCommon/StcContext.hpp"

namespace GuiCommon
{
	class StcTextEditor
		: public wxStyledTextCtrl
	{
	private:
		typedef enum gcID
		{
			gcID_PROPERTIES = wxID_HIGHEST,
			gcID_INDENTINC,
			gcID_INDENTRED,
			gcID_FINDNEXT,
			gcID_REPLACE,
			gcID_REPLACENEXT,
			gcID_BRACEMATCH,
			gcID_GOTO,
			gcID_PAGEACTIVE,
			gcID_DISPLAYEOL,
			gcID_INDENTGUIDE,
			gcID_LINENUMBER,
			gcID_LONGLINEON,
			gcID_WHITESPACE,
			gcID_FOLDTOGGLE,
			gcID_OVERTYPE,
			gcID_READONLY,
			gcID_WRAPMODEON,
			gcID_CHANGECASE,
			gcID_CHANGELOWER,
			gcID_CHANGEUPPER,
			gcID_HILIGHTLANG,
			gcID_HILIGHTFIRST,
			gcID_HILIGHTLAST = gcID_HILIGHTFIRST + 99,
			gcID_CONVERTEOL,
			gcID_CONVERTCR,
			gcID_CONVERTCRLF,
			gcID_CONVERTLF,
			gcID_USECHARSET,
			gcID_CHARSETANSI,
			gcID_CHARSETMAC,
			gcID_PAGEPREV,
			gcID_PAGENEXT,
			gcID_SELECTLINE,
			gcID_WINDOW_MINIMAL,
			gcID_STATUSBAR,
			gcID_TITLEBAR,
			gcID_ABOUTTIMER,
			gcID_UPDATETIMER,
			gcID_DLG_FIND_TEXT,
			gcID_PREFS_LANGUAGE,
			gcID_PREFS_STYLETYPE,
			gcID_PREFS_KEYWORDS,
		}	gcID;

#if wxMAJOR_VERSION >= 3 || ( wxMAJOR_VERSION == 2 && wxMINOR_VERSION >= 9 )
		class TextAutoCompleter
			: public wxTextCompleter
		{
		public:
			TextAutoCompleter( wxArrayString const & keywords );
			bool Start( wxString const & prefix )override;
			wxString GetNext()override;

		private:
			std::set< wxString > m_keywords;
			std::set< wxString >::iterator m_current;
			wxString m_prefix;
		};
#endif

	public:
		StcTextEditor( StcContext & context
			, wxWindow * parent
			, wxWindowID id = wxID_ANY
			, wxPoint const & pos = wxDefaultPosition
			, wxSize const & size = wxDefaultSize
			, long style = wxVSCROLL );
		virtual ~StcTextEditor();

		bool loadFile();
		bool loadFile( wxString const & filename );
		bool saveFile();
		bool saveFile( wxString const & filename );
		void setText( wxString const & source );
		bool isModified();
		wxString determinePrefs( wxString const & filename );
		bool initializePrefs( wxString const & filename );

		inline wxString getFileName()
		{
			return m_filename;
		}

		inline void setFilename( wxString const & filename )
		{
			m_filename = filename;
		}

		inline LanguageInfoPtr getLanguageInfo()
		{
			return m_language;
		}

	private:
		void doInitialiseBaseColours( wxColour const & bgColour
			, wxColour const & fgColour );

	private:
		DECLARE_EVENT_TABLE()
		void onSize( wxSizeEvent & event );
		void onEditRedo( wxCommandEvent & event );
		void onEditUndo( wxCommandEvent & event );
		void onEditClear( wxCommandEvent & event );
		void onEditCut( wxCommandEvent & event );
		void onEditCopy( wxCommandEvent & event );
		void onEditPaste( wxCommandEvent & event );
		void onFind( wxCommandEvent & event );
		void onFindNext( wxCommandEvent & event );
		void onReplace( wxCommandEvent & event );
		void onReplaceNext( wxCommandEvent & event );
		void onBraceMatch( wxCommandEvent & event );
		void onGoto( wxCommandEvent & event );
		void onEditIndentInc( wxCommandEvent & event );
		void onEditIndentRed( wxCommandEvent & event );
		void onEditSelectAll( wxCommandEvent & event );
		void onEditSelectLine( wxCommandEvent & event );
		void onHighlightLang( wxCommandEvent & event );
		void onDisplayEOL( wxCommandEvent & event );
		void onIndentGuide( wxCommandEvent & event );
		void onLineNumber( wxCommandEvent & event );
		void onLongLineOn( wxCommandEvent & event );
		void onWhiteSpace( wxCommandEvent & event );
		void onFoldToggle( wxCommandEvent & event );
		void onSetOverType( wxCommandEvent & event );
		void onSetReadOnly( wxCommandEvent & event );
		void onWrapmodeOn( wxCommandEvent & event );
		void onUseCharset( wxCommandEvent & event );
		void onChangeCase( wxCommandEvent & event );
		void onConvertEOL( wxCommandEvent & event );
		void onMarginClick( wxStyledTextEvent & event );
		void onCharAdded( wxStyledTextEvent & event );
		void onKey( wxStyledTextEvent & event );

	private:
		wxString m_filename;
		wxString m_currentWord;
		LanguageInfoPtr m_language;
		StcContext & m_context;
		int m_lineNrID;
		int m_lineNrMargin;
		int m_foldingID;
		int m_foldingMargin;
		int m_dividerID;
		int m_tabSpaces;
		bool m_useTabs;
		bool m_tabIndents;
		bool m_backspaceUnindents;
	};
}

#endif
