/*
See LICENSE file in root folder
*/
#ifndef ___ARIA_StcContext_H___
#define ___ARIA_StcContext_H___

#include "LanguageInfo.hpp"

#include <wx/stc/stc.h>

namespace aria
{
	/**
	\~english
	\brief Holds the general context used by Scintilla
	\~french
	\brief Contient le contexte gànàral utilisà par Scintilla
	*/
	class StcContext
	{
	public:
		StcContext();

		void parseFile( wxString const & fileName );

	public:
		LanguageInfo language;
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
	};
}

#endif
