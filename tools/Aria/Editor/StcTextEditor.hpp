/*
See LICENSE file in root folder
*/
#ifndef ___ARIA_StcTextEditor_H___
#define ___ARIA_StcTextEditor_H___

#include <wx/stc/stc.h>
#if wxMAJOR_VERSION >= 3 || ( wxMAJOR_VERSION == 2 && wxMINOR_VERSION >= 9 )
#	include <wx/textcompleter.h>
#endif
#include <set>

#include "StcContext.hpp"

namespace aria
{
	class StcTextEditor
		: public wxStyledTextCtrl
	{
	private:
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

		bool loadFile( wxString const & filename );
		bool saveFile();
		bool isModified();

		wxString getFileName()
		{
			return m_filename;
		}

		void setFilename( wxString const & filename )
		{
			m_filename = filename;
		}

		LanguageInfo & getLanguageInfo()
		{
			return m_context.language;
		}

	private:
		void doInitializePrefs();
		void doInitialiseBaseColours( wxColour const & bgColour
			, wxColour const & fgColour );

	private:
		DECLARE_EVENT_TABLE()
		void onSize( wxSizeEvent & event );
		void onFind( wxCommandEvent & event );
		void onFindNext( wxCommandEvent & event );
		void onReplace( wxCommandEvent & event );
		void onReplaceNext( wxCommandEvent & event );
		void onBraceMatch( wxCommandEvent & event );
		void onGoto( wxCommandEvent & event );
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
		void onConvertEOL( wxCommandEvent & event );
		void onMarginClick( wxStyledTextEvent & event );

	private:
		wxString m_filename;
		wxString m_currentWord;
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
