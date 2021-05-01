/*
See LICENSE file in root folder
*/
#ifndef ___ARIA_LanguageInfo_H___
#define ___ARIA_LanguageInfo_H___

#include "StyleInfo.hpp"

#include <CastorUtils/Exception/Assertion.hpp>
#include <CastorUtils/Miscellaneous/StringUtils.hpp>

#include <wx/stc/stc.h>

namespace aria
{
	/**
	\~english
	\brief Defines language informations (name, file patterns, Scintilla lexer, ...)
	\~french
	\brief Dàfinit les informations d'un langage (nom, extension de fichier, lexer Scintilla, ...)
	*/
	class LanguageInfo
	{
	private:
		typedef std::array< wxString, 9u > WordArray;

	public:
		LanguageInfo();
		virtual ~LanguageInfo();

		void setKeywords( uint32_t index
			, wxArrayString const & keywords );

		wxString const & getKeywords( uint32_t index )const
		{
			CU_Require( index < 9 );
			return m_keywords[index];
		}

		StyleInfo & getStyle( int type )
		{
			return m_styles.at( type );
		}

		StyleInfo const & getStyle( int type )const
		{
			return m_styles.at( type );
		}

		StyleInfoMap const & getStyles()const
		{
			return m_styles;
		}

	public:
		wxString name;
		wxString filePattern;
		bool isCLike{ true };
		int foldFlags{ 0 };
		int32_t fontSize{ 10 };
		wxString fontName;

	private:
		WordArray m_keywords;
		StyleInfoMap m_styles;
	};
}

#endif
