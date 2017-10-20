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
		LanguageInfoPtr pCurrentLanguage;
		StyleInfoPtr pCurrentStyle;
		castor::UIntStrMap mapFoldFlags;
		castor::UIntStrMap mapTypes;
		castor::UIntStrMap mapLexers;
		castor::String strName;
		eSTC_TYPE eStyle;
		eSTC_TYPE eType;
		castor::StringArray arrayWords;

	public:
		LanguageFileContext( castor::TextFile * p_pFile );
	};
}

#endif
