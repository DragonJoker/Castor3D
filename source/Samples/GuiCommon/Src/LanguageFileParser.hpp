/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_LANGUAGE_FILE_PARSER_H___
#define ___GUICOMMON_LANGUAGE_FILE_PARSER_H___

#include "StcContext.hpp"

#include <FileParser/FileParser.hpp>

namespace GuiCommon
{
	/*!
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
		eRoot = MAKE_SECTION_NAME( 'R', 'O', 'O', 'T' ),
		eLanguage = MAKE_SECTION_NAME( 'L', 'A', 'N', 'G' ),
		eSection = MAKE_SECTION_NAME( 'S', 'E', 'C', 'T' ),
		eStyle = MAKE_SECTION_NAME( 'S', 'T', 'Y', 'L' ),
		eList = MAKE_SECTION_NAME( 'L', 'I', 'S', 'T' ),
	};
	/*!
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
		virtual bool doDelegateParser( castor::String const & CU_PARAM_UNUSED( p_line ) )
		{
			return false;
		}
		virtual void doValidate();
		virtual castor::String doGetSectionName( uint32_t p_section );
	};

	DECLARE_ATTRIBUTE_PARSER( Root_Language )
	DECLARE_ATTRIBUTE_PARSER( Language_Pattern )
	DECLARE_ATTRIBUTE_PARSER( Language_Lexer )
	DECLARE_ATTRIBUTE_PARSER( Language_FoldFlags )
	DECLARE_ATTRIBUTE_PARSER( Language_Section )
	DECLARE_ATTRIBUTE_PARSER( Language_Style )
	DECLARE_ATTRIBUTE_PARSER( Style_Type )
	DECLARE_ATTRIBUTE_PARSER( Style_FgColour )
	DECLARE_ATTRIBUTE_PARSER( Style_BgColour )
	DECLARE_ATTRIBUTE_PARSER( Style_FontName )
	DECLARE_ATTRIBUTE_PARSER( Style_FontStyle )
	DECLARE_ATTRIBUTE_PARSER( Style_FontSize )
	DECLARE_ATTRIBUTE_PARSER( Section_Type )
	DECLARE_ATTRIBUTE_PARSER( Section_List )
	DECLARE_ATTRIBUTE_PARSER( Section_End )
}

#endif
