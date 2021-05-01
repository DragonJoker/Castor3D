/*
See LICENSE file in root folder
*/
#ifndef ___ARIA_LanguageFileParser_H___
#define ___ARIA_LanguageFileParser_H___

#include "StcContext.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>

namespace aria
{
	/**
	\~english
	\brief Language file sections enumeration
	\~french
	\brief Enumération des sections de fichier de langage
	*/
	enum class LANGSection
	{
		eRoot = CU_MakeSectionName( 'R', 'O', 'O', 'T' ),
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
		explicit LanguageFileParser( StcContext & stcContext );
		~LanguageFileParser();

	private:
		void doInitialiseParser( castor::Path const & path )override;
		void doCleanupParser()override;
		bool doDiscardParser( castor::String const & line )override;
		bool doDelegateParser( castor::String const & CU_UnusedParam( line ) )override
		{
			return false;
		}
		void doValidate()override;
		castor::String doGetSectionName( uint32_t section )override;

	private:
		StcContext & m_stcContext;
	};

	CU_DeclareAttributeParser( Language_Pattern )
	CU_DeclareAttributeParser( Language_Lexer )
	CU_DeclareAttributeParser( Language_FoldFlags )
	CU_DeclareAttributeParser( Language_Keywords )
	CU_DeclareAttributeParser( Language_Style )
	CU_DeclareAttributeParser( Language_FontName )
	CU_DeclareAttributeParser( Language_FontSize )
	CU_DeclareAttributeParser( Language_CLike )
	CU_DeclareAttributeParser( Style_Type )
	CU_DeclareAttributeParser( Style_FgColour )
	CU_DeclareAttributeParser( Style_BgColour )
	CU_DeclareAttributeParser( Style_FontStyle )
	CU_DeclareAttributeParser( Keywords_End )
}

#endif
