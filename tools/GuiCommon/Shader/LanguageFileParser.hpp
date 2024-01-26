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
		void doCleanupParser( castor::PreprocessedFile & preprocessed )override;
		void doValidate( castor::PreprocessedFile & preprocessed )override;
		castor::String doGetSectionName( castor::SectionId section )const override;
		castor::RawUniquePtr< castor::FileParser > doCreateParser()const override;

	private:
		StcContext * m_stcContext;
	};
}

#endif
