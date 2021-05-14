/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_LANGUAGE_INFO_H___
#define ___GUICOMMON_LANGUAGE_INFO_H___

#include "GuiCommon/Shader/StyleInfo.hpp"

#include <wx/stc/stc.h>

namespace GuiCommon
{
	static const wxString DEFAULT_LANGUAGE = wxT( "<default>" );
	/**
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 19/10/2011
	\~english
	\brief Defines language informations (name, file patterns, Scintilla lexer, ...)
	\~french
	\brief Dàfinit les informations d'un langage (nom, extension de fichier, lexer Scintilla, ...)
	*/
	class LanguageInfo
	{
	private:
		typedef std::array< castor::String, 9u > WordArray;

	public:
		LanguageInfo();
		virtual ~LanguageInfo();

		void setKeywords( uint32_t index
			, castor::StringArray const & keywords );

		inline castor::String const & getKeywords( uint32_t index )const
		{
			CU_Require( index < 9 );
			return m_keywords[index];
		}

		inline StyleInfo & getStyle( int type )
		{
			return m_styles.at( type );
		}

		inline StyleInfo const & getStyle( int type )const
		{
			return m_styles.at( type );
		}

		inline StyleInfoMap const & getStyles()const
		{
			return m_styles;
		}

	public:
		castor::String name;
		castor::String filePattern;
		bool isCLike{ true };
		int foldFlags{ 0 };
		int32_t fontSize{ 10 };
		castor::String fontName;

	private:
		WordArray m_keywords;
		StyleInfoMap m_styles;
	};
}

#endif
