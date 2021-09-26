/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_LANGUAGE_FILE_PARSER_H___
#define ___GUICOMMON_LANGUAGE_FILE_PARSER_H___

#include "GuiCommon/Shader/StcContext.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>

namespace GuiCommon
{
	/**
	\~english
	\brief Language file sections enumeration
	\~french
	\brief Enumàration des sections de fichier de langage
	*/
	enum class LANGSection
	{
		eRoot = CU_MakeSectionName( 'R', 'O', 'O', 'T' ),
		eLanguage = CU_MakeSectionName( 'L', 'A', 'N', 'G' ),
		eKeywords = CU_MakeSectionName( 'K', 'W', 'R', 'D' ),
		eStyle = CU_MakeSectionName( 'S', 'T', 'Y', 'L' ),
	};
	/**
	\~english
	\brief Language file parser
	\~french
	\brief Analyseur de fichiers de langage
	*/
	class LanguageFileParser
		: public castor::FileParser
	{
	public:
		/**@name Construction / Destruction */
		//@{
		explicit LanguageFileParser( StcContext * stcContext );
		//@}

	private:
		castor::FileParserContextSPtr doInitialiseParser( castor::Path const & path )override;
		void doCleanupParser( castor::PreprocessedFile & preprocessed )override;
		bool doDiscardParser( castor::PreprocessedFile & preprocessed
			, castor::String const & line )override;
		void doValidate()override;
		castor::String doGetSectionName( castor::SectionId section )const override;
		std::unique_ptr< castor::FileParser > doCreateParser()const override;

	private:
		StcContext * m_stcContext;
	};
}

#endif
