/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_LANGUAGE_FILE_CONTEXT_H___
#define ___GUICOMMON_LANGUAGE_FILE_CONTEXT_H___

#include "GuiCommon/Shader/StcContext.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>
#include <CastorUtils/FileParser/FileParserContext.hpp>

namespace GuiCommon
{
	/**
	\~english
	\brief File parsing context for language files
	\~french
	\brief Contexte d'analyse pour les fichiers de langage
	*/
	class LanguageFileContext
	{
	public:
		explicit LanguageFileContext();

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
