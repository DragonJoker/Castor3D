/*
See LICENSE file in root folder
*/
#ifndef ___ARIA_LanguageFileContext_H___
#define ___ARIA_LanguageFileContext_H___

#include "StcContext.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>
#include <CastorUtils/FileParser/FileParserContext.hpp>

namespace aria
{
	class LanguageFileContext
		: public castor::FileParserContext
	{
	public:
		explicit LanguageFileContext( castor::Path const & path
			, LanguageInfo & currentLanguage );

	public:
		LanguageInfo & currentLanguage;
		StyleInfo * currentStyle{ nullptr };
		castor::UInt32StrMap mapFoldFlags;
		castor::UInt32StrMap mapTypes;
		castor::String strName;
		wxArrayString keywords;
		uint32_t index;
	};
}

#endif
