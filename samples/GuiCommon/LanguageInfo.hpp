/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_LANGUAGE_INFO_H___
#define ___GUICOMMON_LANGUAGE_INFO_H___

#include "GuiCommon/GuiCommonPrerequisites.hpp"
#include "GuiCommon/StyleInfo.hpp"

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

		inline int getFoldFlags()const
		{
			return m_foldFlags;
		}

		inline int32_t getFontSize()const
		{
			return m_fontSize;
		}

		inline castor::String const & getKeywords( uint32_t index )const
		{
			CU_Require( index < 9 );
			return m_keywords[index];
		}

		inline castor::String const & getFilePattern()const
		{
			return m_filePattern;
		}

		inline castor::String const & getName()const
		{
			return m_name;
		}

		inline castor::String const & getFontName()const
		{
			return m_fontName;
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

		inline void setName( castor::String const & name )
		{
			m_name = name;
		}

		inline void setFontName( castor::String const & name )
		{
			m_fontName = name;
		}

		inline void setFilePattern( castor::String const & pattern )
		{
			m_filePattern = pattern;
		}

		inline void setFoldFlags( int flags )
		{
			m_foldFlags = flags;
		}

		inline void setFontSize( int32_t size )
		{
			m_fontSize = size;
		}

	private:
		castor::String m_name;
		castor::String m_filePattern;
		WordArray m_keywords;
		int m_foldFlags{ 0 };
		int32_t m_fontSize{ 10 };
		castor::String m_fontName;
		StyleInfoMap m_styles;
	};
}

#endif
