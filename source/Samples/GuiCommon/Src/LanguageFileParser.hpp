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
		LanguageFileParser( StcContext * p_pStcContext );
		virtual ~LanguageFileParser();
		//@}

	private:
		virtual void doInitialiseParser( castor::TextFile & p_file );
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
