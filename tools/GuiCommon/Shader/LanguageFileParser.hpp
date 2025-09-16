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
		: c3d::SectionId
	{
		eRoot = c3d::makeSectionName( 'R', 'O', 'O', 'T' ),
		eLanguage = c3d::makeSectionName( 'L', 'A', 'N', 'G' ),
		eKeywords = c3d::makeSectionName( 'K', 'W', 'R', 'D' ),
		eStyle = c3d::makeSectionName( 'S', 'T', 'Y', 'L' ),
	};
	/**
	\~english
	\brief Language file parser
	\~french
	\brief Analyseur de fichiers de langage
	*/
	class LanguageFileParser
		: public c3d::FileParser
	{
	public:
		/**@name Construction / Destruction */
		//@{
		explicit LanguageFileParser( StcContext * stcContext );
		//@}

	private:
		void doCleanupParser( c3d::PreprocessedFile & preprocessed )override;
		void doValidate( c3d::PreprocessedFile & preprocessed )override;
		c3d::String doGetSectionName( c3d::SectionId section )const override;
		c3d::RawUniquePtr< c3d::FileParser > doCreateParser()const override;

	private:
		StcContext * m_stcContext;
	};
}

#endif
