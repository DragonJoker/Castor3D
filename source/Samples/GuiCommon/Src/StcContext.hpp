/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_STC_CONTEXT_H___
#define ___GUICOMMON_STC_CONTEXT_H___

#include "GuiCommonPrerequisites.hpp"

#include <wx/stc/stc.h>

namespace GuiCommon
{
	/*!
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 19/10/2011
	\~english
	\brief Holds the general context used by Scintilla
	\~french
	\brief Contient le contexte gànàral utilisà par Scintilla
	*/
	class StcContext
	{
	private:
		bool m_bSyntaxEnable;
		bool m_bFoldEnable;
		bool m_bIndentEnable;
		bool m_bReadOnlyInitial;
		bool m_bOverTypeInitial;
		bool m_bWrapModeInitial;
		bool m_bDisplayEOLEnable;
		bool m_bIndentGuideEnable;
		bool m_bLineNumberEnable;
		bool m_bLongLineOnEnable;
		bool m_bWhiteSpaceEnable;
		LanguageInfoPtrArray m_arrayLanguages;

	public:
		StcContext();
		virtual ~StcContext();

		void parseFile( wxString const & p_strFileName );

		inline uint32_t getNbLanguages()const
		{
			return uint32_t( m_arrayLanguages.size() );
		}

		inline LanguageInfoPtrArrayIt begin()
		{
			return m_arrayLanguages.begin();
		}

		inline LanguageInfoPtrArrayConstIt begin()const
		{
			return m_arrayLanguages.begin();
		}

		inline LanguageInfoPtrArrayConstIt end()const
		{
			return m_arrayLanguages.end();
		}

		inline bool getWhiteSpaceEnable()const
		{
			return m_bWhiteSpaceEnable;
		}

		inline bool getLongLineOnEnable()const
		{
			return m_bLongLineOnEnable;
		}

		inline bool getLineNumberEnable()const
		{
			return m_bLineNumberEnable;
		}

		inline bool getIndentGuideEnable()const
		{
			return m_bIndentGuideEnable;
		}

		inline bool getDisplayEOLEnable()const
		{
			return m_bDisplayEOLEnable;
		}

		inline bool getWrapModeInitial()const
		{
			return m_bWrapModeInitial;
		}

		inline bool getOverTypeInitial()const
		{
			return m_bOverTypeInitial;
		}

		inline bool getReadOnlyInitial()const
		{
			return m_bReadOnlyInitial;
		}

		inline bool getIndentEnable()const
		{
			return m_bIndentEnable;
		}

		inline bool getFoldEnable()const
		{
			return m_bFoldEnable;
		}
		inline bool getSyntaxEnable()const
		{
			return m_bSyntaxEnable;
		}

		inline void addLanguage( LanguageInfoPtr p_pLanguage )
		{
			m_arrayLanguages.push_back( p_pLanguage );
		}

		inline void setSyntaxEnable( bool val )
		{
			m_bSyntaxEnable = val;
		}

		inline void setFoldEnable( bool val )
		{
			m_bFoldEnable = val;
		}

		inline void setIndentEnable( bool val )
		{
			m_bIndentEnable = val;
		}

		inline void setReadOnlyInitial( bool val )
		{
			m_bReadOnlyInitial = val;
		}

		inline void setOverTypeInitial( bool val )
		{
			m_bOverTypeInitial = val;
		}

		inline void setWrapModeInitial( bool val )
		{
			m_bWrapModeInitial = val;
		}

		inline void setDisplayEOLEnable( bool val )
		{
			m_bDisplayEOLEnable = val;
		}

		inline void setIndentGuideEnable( bool val )
		{
			m_bIndentGuideEnable = val;
		}

		inline void setLineNumberEnable( bool val )
		{
			m_bLineNumberEnable = val;
		}

		inline void setLongLineOnEnable( bool val )
		{
			m_bLongLineOnEnable = val;
		}

		inline void setWhiteSpaceEnable( bool val )
		{
			m_bWhiteSpaceEnable = val;
		}
	};
}

#endif
