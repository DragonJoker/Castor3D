/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_STC_TEXT_EDITOR_H___
#define ___GUICOMMON_STC_TEXT_EDITOR_H___

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wold-style-cast"

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#include <wx/stc/stc.h>

#if wxMAJOR_VERSION >= 3 || ( wxMAJOR_VERSION == 2 && wxMINOR_VERSION >= 9 )
#	include <wx/textcompleter.h>
#endif
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

#include <set>

#include "GuiCommon/Shader/StcContext.hpp"

namespace GuiCommon
{
	class StcTextEditor
		: public wxStyledTextCtrl
	{
	private:
		enum class gcID
		{
			PROPERTIES = wxID_HIGHEST,
			INDENTINC,
			INDENTRED,
			FINDNEXT,
			REPLACE,
			REPLACENEXT,
			BRACEMATCH,
			GOTO,
			PAGEACTIVE,
			DISPLAYEOL,
			INDENTGUIDE,
			LINENUMBER,
			LONGLINEON,
			WHITESPACE,
			FOLDTOGGLE,
			OVERTYPE,
			READONLY,
			WRAPMODEON,
			CHANGECASE,
			CHANGELOWER,
			CHANGEUPPER,
			HILIGHTLANG,
			HILIGHTFIRST,
			HILIGHTLAST = HILIGHTFIRST + 99,
			CONVERTEOL,
			CONVERTCR,
			CONVERTCRLF,
			CONVERTLF,
			USECHARSET,
			CHARSETANSI,
			CHARSETMAC,
			PAGEPREV,
			PAGENEXT,
			SELECTLINE,
			WINDOW_MINIMAL,
			STATUSBAR,
			TITLEBAR,
			ABOUTTIMER,
			UPDATETIMER,
			DLG_FIND_TEXT,
			PREFS_LANGUAGE,
			PREFS_STYLETYPE,
			PREFS_KEYWORDS,
		};

#if wxMAJOR_VERSION >= 3 || ( wxMAJOR_VERSION == 2 && wxMINOR_VERSION >= 9 )
		class TextAutoCompleter
			: public wxTextCompleter
		{
		public:
			explicit TextAutoCompleter( wxArrayString const & keywords );
			bool Start( wxString const & prefix )override;
			wxString GetNext()override;

		private:
			c3d::Set< wxString > m_keywords;
			c3d::Set< wxString >::iterator m_current;
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

		bool loadFile();
		bool loadFile( wxString const & filename );
		bool saveFile();
		bool saveFile( wxString const & filename );
		void setText( wxString const & source );
		bool isModified()const;
		wxString determinePrefs( wxString const & filename )const;
		bool initializePrefs( wxString const & filename );

		inline wxString const & getFileName()const
		{
			return m_fileName;
		}

		inline void setFilename( wxString const & filename )
		{
			m_fileName = filename;
		}

		inline LanguageInfoRPtr getLanguageInfo()const
		{
			return m_language;
		}

	private:
		void doInitialiseBaseColours( wxColour const & bgColour
			, wxColour const & fgColour );

	private:
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-override"
		DECLARE_EVENT_TABLE()
#pragma clang diagnostic pop
		void onSize( wxSizeEvent & event );
		void onEditRedo( wxCommandEvent & event );
		void onEditUndo( wxCommandEvent & event );
		void onEditClear( wxCommandEvent & event );
		void onEditCut( wxCommandEvent & event );
		void onEditCopy( wxCommandEvent & event );
		void onEditPaste( wxCommandEvent & event );
		void onBraceMatch( wxCommandEvent & event );
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

	private:
		wxString m_fileName;
		wxString m_currentWord;
		LanguageInfoRPtr m_language{};
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

#pragma clang diagnostic pop

#endif
