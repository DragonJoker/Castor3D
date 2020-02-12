/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_LANGUAGE_FILE_PARSER_H___
#define ___GUICOMMON_LANGUAGE_FILE_PARSER_H___

#include "GuiCommon/StcContext.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>

namespace GuiCommon
{
	/**
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 19/10/2011
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
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 19/10/2011
	\~english
	\brief Language file parser
	\~french
	\brief Analyseur de fichiers de langage
	*/
	class LanguageFileParser
		: public castor::FileParser
	{
	private:
		StcContext * m_pStcContext;

	public:
		/**@name Construction / Destruction */
		//@{
		explicit LanguageFileParser( StcContext * p_pStcContext );
		virtual ~LanguageFileParser();
		//@}

	private:
		virtual void doInitialiseParser( castor::Path const & path );
		virtual void doCleanupParser();
		virtual bool doDiscardParser( castor::String const & p_line );
		virtual bool doDelegateParser( castor::String const & CU_UnusedParam( p_line ) )
		{
			return false;
		}
		virtual void doValidate();
		virtual castor::String doGetSectionName( uint32_t p_section );
	};

	CU_DeclareAttributeParser( Root_Language )
	CU_DeclareAttributeParser( Language_Pattern )
	CU_DeclareAttributeParser( Language_Lexer )
	CU_DeclareAttributeParser( Language_FoldFlags )
	CU_DeclareAttributeParser( Language_Keywords )
	CU_DeclareAttributeParser( Language_Style )
	CU_DeclareAttributeParser( Language_FontName )
	CU_DeclareAttributeParser( Language_FontSize )
	CU_DeclareAttributeParser( Style_Type )
	CU_DeclareAttributeParser( Style_FgColour )
	CU_DeclareAttributeParser( Style_BgColour )
	CU_DeclareAttributeParser( Style_FontStyle )
	CU_DeclareAttributeParser( Keywords_End )
}

#endif
