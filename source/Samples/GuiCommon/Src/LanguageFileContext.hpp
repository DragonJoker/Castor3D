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
#ifndef ___GUICOMMON_LANGUAGE_FILE_CONTEXT_H___
#define ___GUICOMMON_LANGUAGE_FILE_CONTEXT_H___

#include "StcContext.hpp"

#include <FileParser/FileParser.hpp>
#include <FileParser/FileParserContext.hpp>

namespace GuiCommon
{
	/*!
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 19/10/2011
	\~english
	\brief File parsing context for language files
	\~french
	\brief Contexte d'analyse pour les fichiers de langage
	*/
	class LanguageFileContext
		: public castor::FileParserContext
	{
	public:
		LanguageInfoPtr pCurrentLanguage;
		StyleInfoPtr pCurrentStyle;
		castor::UIntStrMap mapFoldFlags;
		castor::UIntStrMap mapTypes;
		castor::UIntStrMap mapLexers;
		castor::String strName;
		eSTC_TYPE eStyle;
		eSTC_TYPE eType;
		castor::StringArray arrayWords;

	public:
		LanguageFileContext( castor::TextFile * p_pFile );
	};
}

#endif
