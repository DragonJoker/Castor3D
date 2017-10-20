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

#include "GuiCommonPrerequisites.hpp"
#include "StcContext.hpp"

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
			TextAutoCompleter( wxArrayString const & p_keywords );
			virtual bool Start( wxString const & p_prefix );
			virtual wxString GetNext();

		private:
			std::set< wxString > m_keywords;
			std::set< wxString >::iterator m_current;
			wxString m_prefix;
		};
#endif

	public:
		StcTextEditor( StcContext & p_context, wxWindow * p_parent, wxWindowID p_id = wxID_ANY, wxPoint const & p_ptPos = wxDefaultPosition, wxSize const & p_size = wxDefaultSize, long p_lStyle = wxVSCROLL );
		virtual ~StcTextEditor();

		bool LoadFile();
		bool LoadFile( wxString const & p_strFilename );
		bool SaveFile();
		bool SaveFile( wxString const & p_strFilename );
		void setText( wxString const & p_strSource );
		bool isModified();
		wxString DeterminePrefs( wxString const & p_strFilename );
		bool InitializePrefs( wxString const & p_strFilename );

		inline wxString getFileName()
		{
			return m_strFilename;
		}

		inline void setFilename( wxString const & p_strFilename )
		{
			m_strFilename = p_strFilename;
		}

		inline LanguageInfoPtr getLanguageInfo()
		{
			return m_pLanguage;
		}

	private:
		bool Usersettings( wxString const & p_strFilename );

	private:
		DECLARE_EVENT_TABLE()
		void OnSize( wxSizeEvent & p_event );
		void OnEditRedo( wxCommandEvent & p_event );
		void OnEditUndo( wxCommandEvent & p_event );
		void OnEditClear( wxCommandEvent & p_event );
		void OnEditCut( wxCommandEvent & p_event );
		void OnEditCopy( wxCommandEvent & p_event );
		void OnEditPaste( wxCommandEvent & p_event );
		void OnFind( wxCommandEvent & p_event );
		void OnFindNext( wxCommandEvent & p_event );
		void OnReplace( wxCommandEvent & p_event );
		void OnReplaceNext( wxCommandEvent & p_event );
		void OnBraceMatch( wxCommandEvent & p_event );
		void OnGoto( wxCommandEvent & p_event );
		void OnEditIndentInc( wxCommandEvent & p_event );
		void OnEditIndentRed( wxCommandEvent & p_event );
		void OnEditSelectAll( wxCommandEvent & p_event );
		void OnEditSelectLine( wxCommandEvent & p_event );
		void OnHilightLang( wxCommandEvent & p_event );
		void OnDisplayEOL( wxCommandEvent & p_event );
		void OnIndentGuide( wxCommandEvent & p_event );
		void OnLineNumber( wxCommandEvent & p_event );
		void OnLongLineOn( wxCommandEvent & p_event );
		void OnWhiteSpace( wxCommandEvent & p_event );
		void OnFoldToggle( wxCommandEvent & p_event );
		void OnsetOverType( wxCommandEvent & p_event );
		void OnsetReadOnly( wxCommandEvent & p_event );
		void OnWrapmodeOn( wxCommandEvent & p_event );
		void OnUseCharset( wxCommandEvent & p_event );
		void OnChangeCase( wxCommandEvent & p_event );
		void OnConvertEOL( wxCommandEvent & p_event );
		void OnMarginClick( wxStyledTextEvent & p_event );
		void OnCharAdded( wxStyledTextEvent & p_event );
		void OnKey( wxStyledTextEvent & p_event );

	private:
		wxString m_strFilename;
		wxString m_strCurrentWord;
		LanguageInfoPtr m_pLanguage;
		int m_iLineNrID;
		int m_iLineNrMargin;
		int m_iFoldingID;
		int m_iFoldingMargin;
		int m_iDividerID;
		StcContext & m_context;
		int m_iTabSpaces;
		bool m_bUseTabs;
		bool m_bTabIndents;
		bool m_bBackspaceUnindents;
	};
}

#endif
