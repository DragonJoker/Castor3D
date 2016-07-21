/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
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
		: public Castor::FileParserContext
	{
	public:
		LanguageInfoPtr pCurrentLanguage;
		StyleInfoPtr pCurrentStyle;
		Castor::UIntStrMap mapFoldFlags;
		Castor::UIntStrMap mapTypes;
		Castor::UIntStrMap mapLexers;
		Castor::String strName;
		eSTC_TYPE eStyle;
		eSTC_TYPE eType;
		Castor::StringArray arrayWords;

	public:
		LanguageFileContext( Castor::TextFile * p_pFile );
	};
}

#endif
