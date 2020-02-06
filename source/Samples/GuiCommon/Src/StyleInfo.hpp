/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_STYLE_INFO_H___
#define ___GUICOMMON_STYLE_INFO_H___

#include "GuiCommonPrerequisites.hpp"

namespace GuiCommon
{
	/*!
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 19/10/2011
	\~english
	\brief Text style enumeration
	\~french
	\brief Enumération des styles de texte
	*/
	typedef enum eSTC_STYLE
	{
		eSTC_STYLE_BOLD		= 0x01,
		eSTC_STYLE_ITALIC	= 0x02,
		eSTC_STYLE_UNDERL	= 0x04,
		eSTC_STYLE_HIDDEN	= 0x08,
	}	eSTC_STYLE;
	/*!
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 19/10/2011
	\~english Folded types enumeration
	\brief
	\~french
	\brief Enumération des types de dblocks pouvant àtre ràduits
	*/
	typedef enum eSTC_FOLD
	{
		eSTC_FOLD_COMMENT	= 0x01,
		eSTC_FOLD_COMPACT	= 0x02,
		eSTC_FOLD_PREPROC	= 0x04,
		eSTC_FOLD_HTML		= 0x10,
		eSTC_FOLD_HTMLPREP	= 0x20,
		eSTC_FOLD_COMMENTPY	= 0x40,
		eSTC_FOLD_QUOTESPY	= 0x80,
	}	eSTC_FOLD;
	/*!
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 19/10/2011
	\~english
	\brief Scintilla flag enumeration
	\~french
	\brief Enumération des flags scintilla
	*/
	typedef enum eSTC_FLAG
	{
		eSTC_FLAG_WRAPMODE	= 0x10,
	}	eSTC_FLAG;
	/*!
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 19/10/2011
	\~english
	\brief Defines style informations for a given type of words
	\~french
	\brief Dàfinit les informations de style pour un type de mots
	*/
	class StyleInfo
	{
	private:
		wxString m_name;
		wxString m_strForeground;
		wxString m_strBackground;
		wxString m_strFontName;
		int m_iFontSize;
		int m_iFontStyle;
		int m_iLetterCase;

	public:
		StyleInfo( wxString const & p_name, wxString const & p_strForeground, wxString const & p_strBackground, wxString const & p_strFontName, int p_iFontsize, int p_iFontstyle, int p_iLetterCase );
		StyleInfo( StyleInfo const & p_infos );
		StyleInfo & operator =( StyleInfo const & p_infos );
		virtual ~StyleInfo();

		inline int getLetterCase()const
		{
			return m_iLetterCase;
		}

		inline int getFontStyle()const
		{
			return m_iFontStyle;
		}

		inline int getFontSize()const
		{
			return m_iFontSize;
		}

		inline wxString	const & getFontName()const
		{
			return m_strFontName;
		}

		inline wxString	const & getBackground()const
		{
			return m_strBackground;
		}

		inline wxString	const & getForeground()const
		{
			return m_strForeground;
		}

		inline wxString	const & getName()const
		{
			return m_name;
		}

		inline void setName( wxString const & val )
		{
			m_name = val;
		}

		inline void setForeground( wxString const & val )
		{
			m_strForeground = val;
		}

		inline void setBackground( wxString const & val )
		{
			m_strBackground = val;
		}

		inline void setFontName( wxString const & val )
		{
			m_strFontName = val;
		}

		inline void setFontSize( int val )
		{
			m_iFontSize = val;
		}

		inline void setFontStyle( int val )
		{
			m_iFontStyle = val;
		}

		inline void setLetterCase( int val )
		{
			m_iLetterCase = val;
		}
	};
}

#endif
