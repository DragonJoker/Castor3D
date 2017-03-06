/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
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

		void ParseFile( wxString const & p_strFileName );

		inline uint32_t GetNbLanguages()const
		{
			return uint32_t( m_arrayLanguages.size() );
		}

		inline LanguageInfoPtrArrayIt Begin()
		{
			return m_arrayLanguages.begin();
		}

		inline LanguageInfoPtrArrayConstIt Begin()const
		{
			return m_arrayLanguages.begin();
		}

		inline LanguageInfoPtrArrayConstIt End()const
		{
			return m_arrayLanguages.end();
		}

		inline bool GetWhiteSpaceEnable()const
		{
			return m_bWhiteSpaceEnable;
		}

		inline bool GetLongLineOnEnable()const
		{
			return m_bLongLineOnEnable;
		}

		inline bool GetLineNumberEnable()const
		{
			return m_bLineNumberEnable;
		}

		inline bool GetIndentGuideEnable()const
		{
			return m_bIndentGuideEnable;
		}

		inline bool GetDisplayEOLEnable()const
		{
			return m_bDisplayEOLEnable;
		}

		inline bool GetWrapModeInitial()const
		{
			return m_bWrapModeInitial;
		}

		inline bool GetOverTypeInitial()const
		{
			return m_bOverTypeInitial;
		}

		inline bool GetReadOnlyInitial()const
		{
			return m_bReadOnlyInitial;
		}

		inline bool GetIndentEnable()const
		{
			return m_bIndentEnable;
		}

		inline bool GetFoldEnable()const
		{
			return m_bFoldEnable;
		}
		inline bool GetSyntaxEnable()const
		{
			return m_bSyntaxEnable;
		}

		inline void AddLanguage( LanguageInfoPtr p_pLanguage )
		{
			m_arrayLanguages.push_back( p_pLanguage );
		}

		inline void SetSyntaxEnable( bool val )
		{
			m_bSyntaxEnable = val;
		}

		inline void SetFoldEnable( bool val )
		{
			m_bFoldEnable = val;
		}

		inline void SetIndentEnable( bool val )
		{
			m_bIndentEnable = val;
		}

		inline void SetReadOnlyInitial( bool val )
		{
			m_bReadOnlyInitial = val;
		}

		inline void SetOverTypeInitial( bool val )
		{
			m_bOverTypeInitial = val;
		}

		inline void SetWrapModeInitial( bool val )
		{
			m_bWrapModeInitial = val;
		}

		inline void SetDisplayEOLEnable( bool val )
		{
			m_bDisplayEOLEnable = val;
		}

		inline void SetIndentGuideEnable( bool val )
		{
			m_bIndentGuideEnable = val;
		}

		inline void SetLineNumberEnable( bool val )
		{
			m_bLineNumberEnable = val;
		}

		inline void SetLongLineOnEnable( bool val )
		{
			m_bLongLineOnEnable = val;
		}

		inline void SetWhiteSpaceEnable( bool val )
		{
			m_bWhiteSpaceEnable = val;
		}
	};
}

#endif
