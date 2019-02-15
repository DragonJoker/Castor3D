/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_LANGUAGE_FILE_CONTEXT_H___
#define ___GUICOMMON_LANGUAGE_FILE_CONTEXT_H___

#include "StcContext.hpp"

#include <FileParser/FileParser.hpp>
#include <FileParser/FileParserContext.hpp>

namespace GuiCommon
{
	/*!
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 19/10/2011
	\~english
	\brief File parsing context for language files
	\~french
	\brief Contexte d'analyse pour les fichiers de langage
	*/
	class LanguageFileContext
		: public castor::FileParserContext
	{
	public:
		explicit LanguageFileContext( castor::Path const & path );

	public:
		LanguageInfoPtr currentLanguage;
		StyleInfo * currentStyle{ nullptr };
		castor::UInt32StrMap mapFoldFlags;
		castor::UInt32StrMap mapTypes;
		castor::String strName;
		castor::StringArray keywords;
		uint32_t index;
	};
}

#endif
