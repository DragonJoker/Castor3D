/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_STYLE_INFO_H___
#define ___GUICOMMON_STYLE_INFO_H___

#include "GuiCommon/GuiCommonPrerequisites.hpp"

namespace GuiCommon
{
	/**
	\~english
	\brief Text style enumeration
	\~french
	\brief Enumération des styles de texte
	*/
	enum class eSTC_STYLE
	{
		eBOLD = 0x01,
		eITALIC = 0x02,
		eUNDERL = 0x04,
		eHIDDEN = 0x08,
	};
	/**
	\~english
	\brief Folded types enumeration
	\~french
	\brief Enumération des types de dblocks pouvant àtre ràduits
	*/
	enum class eSTC_FOLD
	{
		eCOMMENT = 0x01,
		eCOMPACT = 0x02,
		ePREPROC = 0x04,
		eHTML = 0x10,
		eHTMLPREP = 0x20,
		eCOMMENTPY = 0x40,
		eQUOTESPY = 0x80,
	};
	/**
	\~english
	\brief Scintilla flag enumeration
	\~french
	\brief Enumération des flags scintilla
	*/
	enum class eSTC_FLAG
	{
		eWRAPMODE = 0x10,
	};
	/**
	\~english
	\brief Defines style informations for a given type of words
	\~french
	\brief Dàfinit les informations de style pour un type de mots
	*/
	class StyleInfo
	{
	public:
		StyleInfo( wxColour const & foreground
			, wxColour const & background
			, int fontstyle
			, int letterCase );

		wxColour foreground;
		wxColour background;
		int fontStyle{ 0 };
		int letterCase{ 0 };
	};
	CU_DeclareMap( int, StyleInfo, StyleInfo );
}

#endif
