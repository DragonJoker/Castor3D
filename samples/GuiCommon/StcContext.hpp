/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_STC_CONTEXT_H___
#define ___GUICOMMON_STC_CONTEXT_H___

#include "GuiCommon/GuiCommonPrerequisites.hpp"

#include <wx/stc/stc.h>

namespace GuiCommon
{
	/**
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 19/10/2011
	\~english
	\brief Holds the general context used by Scintilla
	\~french
	\brief Contient le contexte gànàral utilisà par Scintilla
	*/
	class StcContext
	{
	public:
		StcContext();
		virtual ~StcContext();

		void parseFile( wxString const & fileName );

		inline size_t size()const
		{
			return m_languages.size();
		}

		inline LanguageInfoPtrArrayIt begin()
		{
			return m_languages.begin();
		}

		inline LanguageInfoPtrArrayConstIt begin()const
		{
			return m_languages.begin();
		}

		inline LanguageInfoPtrArrayIt end()
		{
			return m_languages.end();
		}

		inline LanguageInfoPtrArrayConstIt end()const
		{
			return m_languages.end();
		}

		inline void push_back( LanguageInfoPtr language )
		{
			m_languages.push_back( language );
		}

	public:
		bool syntaxEnable{ true };
		bool foldEnable{ true };
		bool indentEnable{ true };
		bool readOnlyInitial{ false };
		bool overTypeInitial{ false };
		bool wrapModeInitial{ false };
		bool displayEOLEnable{ false };
		bool indentGuideEnable{ false };
		bool lineNumberEnable{ true };
		bool longLineOnEnable{ true };
		bool whiteSpaceEnable{ true };
		int fontSize;
		wxString fontName;

	private:
		LanguageInfoPtrArray m_languages;
	};
}

#endif
