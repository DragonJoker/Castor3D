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
#ifndef ___C3D_PREREQUISITES_OVERLAY_H___
#define ___C3D_PREREQUISITES_OVERLAY_H___

namespace Castor3D
{
	/**@name Overlay */
	//@{

	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Overlay types enumeration
	\~french
	\brief		Enumération des types d'overlays
	*/
	typedef enum eOVERLAY_TYPE
		: uint8_t
	{
		eOVERLAY_TYPE_PANEL,
		eOVERLAY_TYPE_BORDER_PANEL,
		eOVERLAY_TYPE_TEXT,
		CASTOR_ENUM_BOUNDS( eOVERLAY_TYPE, eOVERLAY_TYPE_PANEL )
	}	eOVERLAY_TYPE;
	/*!
	\author 	Sylvain DOREMUS
	\date 		28/11/2014
	\~english
	\brief		Text wrapping modes, handles the way text is cut when it overflows the overlay width
	\~french
	\brief		Modes de découpe du texte, gère la manière dont le texte est dessiné quand il dépasse la largeur de l'incrustation
	*/
	typedef enum eTEXT_WRAPPING_MODE
	{
		//!\~english The text is cut	\~french Le texte qui dépasse est découpé
		eTEXT_WRAPPING_MODE_NONE,
		//!\~english The text jumps to next line	\~french Le texte passe à la ligne suivante
		eTEXT_WRAPPING_MODE_BREAK,
		//!\~english The text jumps to next line without cutting words	\~french Le texte passe à la ligne suivante, sans découper les mots
		eTEXT_WRAPPING_MODE_BREAK_WORDS,
		CASTOR_ENUM_BOUNDS( eTEXT_WRAPPING_MODE, eTEXT_WRAPPING_MODE_NONE )
	}	eTEXT_WRAPPING_MODE;
	/*!
	\author 	Sylvain DOREMUS
	\date 		26/03/2016
	\~english
	\brief		Text texturing modes, handles the way textures are appied to text overlays.
	\~french
	\brief		Modes de texturage du texte, gère la manière dont les textures sont appliquées aux incrustations texte.
	*/
	typedef enum eTEXT_TEXTURING_MODE
	{
		//!\~english The texture is repeated on each character of the text.	\~french La texture est répétée sut chaque caractère du texte.
		eTEXT_TEXTURING_MODE_LETTER,
		//!\~english The texture is applied on the whole text.	\~french La texture est appliquée sur tout le texte.
		eTEXT_TEXTURING_MODE_TEXT,
		CASTOR_ENUM_BOUNDS( eTEXT_TEXTURING_MODE, eTEXT_TEXTURING_MODE_LETTER )
	}	eTEXT_TEXTURING_MODE;
	/*!
	\author 	Sylvain DOREMUS
	\date 		16/04/2016
	\~english
	\brief		Text line spacing modes, handles the way lines are spaced, which each other, inside one TextOverlay.
	\~french
	\brief		Modes d'espacement des lignes du texte, gère la manière dont les lignes sont espacées entre elles, dans un TextOverlay.
	*/
	typedef enum eTEXT_LINE_SPACING_MODE
	{
		//!\~english The height of each line is its own height.	\~french La hauteur d'une ligne est sa propre hauteur.
		eTEXT_LINE_SPACING_MODE_OWN_HEIGHT,
		//!\~english The height of the lines is the maximum height between them.	\~french La hauteur des lignes est la plus grande hauteur parmi elles.
		eTEXT_LINE_SPACING_MODE_MAX_LINE_HEIGHT,
		//!\~english The height of the lines is the maximum height from the font's characters.	\~french La hauteur des lignes est la plus grande hauteur des caractères de la police.
		eTEXT_LINE_SPACING_MODE_MAX_FONT_HEIGHT,
		CASTOR_ENUM_BOUNDS( eTEXT_LINE_SPACING_MODE, eTEXT_LINE_SPACING_MODE_OWN_HEIGHT )
	}	eTEXT_LINE_SPACING_MODE;
	/*!
	\author		Sylvain DOREMUS
	\date		23/09/2015
	\version	0.8.0
	\~english
	\brief		Horizontal alignments for text overlays.
	\~french
	\brief		Alignemens horizontaux pour les incrutstations texte.
	*/
	typedef enum eHALIGN
	{
		//!\~english Aligned on the left.	\~french Aligné à gauche.
		eHALIGN_LEFT,
		//!\~english Centered horizontally.	\~french Centré, horizontalement.
		eHALIGN_CENTER,
		//!\~english Aligned on the right.	\~french Aligné à droite.
		eHALIGN_RIGHT,
		CASTOR_ENUM_BOUNDS( eHALIGN, eHALIGN_LEFT )
	}	eHALIGN;
	/*!
	\author		Sylvain DOREMUS
	\date		23/09/2015
	\version	0.8.0
	\~english
	\brief		Vertical alignments for text overlays.
	\~french
	\brief		Alignemens verticaux pour les incrutstations texte.
	*/
	typedef enum eVALIGN
	{
		//!\~english Aligned on the top.	\~french Aligné en haut.
		eVALIGN_TOP,
		//!\~english Centered vertically.	\~french Centré, verticalement.
		eVALIGN_CENTER,
		//!\~english Aligned on the bottom.	\~french Aligné en bas.
		eVALIGN_BOTTOM,
		CASTOR_ENUM_BOUNDS( eVALIGN, eVALIGN_TOP )
	}	eVALIGN;

	/*!
	\author 	Sylvain DOREMUS
	\date 		28/11/2014
	\~english
	\brief		Text wrapping modes, handles the way text is cut when it overflows the overlay width
	\~french
	\brief		Modes de découpe du texte, gère la manière dont le texte est dessiné quand il dépasse la largeur de l'incrustation
	*/
	typedef enum eBORDER_POSITION
	{
		//!\~english The border is outside the overlay	\~french La bordure est à l'intérieur de l'incrustation
		eBORDER_POSITION_INTERNAL,
		//!\~english The border is middle outside and middle inside the overlay	\~french La bordure est à moitié à l'intérieur, à moitié à l'extérieur de l'incrustation
		eBORDER_POSITION_MIDDLE,
		//!\~english The border is outside the overlay	\~french La bordure est à l'extérieur de l'incrustation
		eBORDER_POSITION_EXTERNAL,
		CASTOR_ENUM_BOUNDS( eBORDER_POSITION, eBORDER_POSITION_INTERNAL )
	}	eBORDER_POSITION;

	class Overlay;
	class OverlayCategory;
	class PanelOverlay;
	class BorderPanelOverlay;
	class TextOverlay;
	class OverlayFactory;
	class OverlayRenderer;
	class FontTexture;

	DECLARE_SMART_PTR( OverlayFactory );
	DECLARE_SMART_PTR( OverlayRenderer );
	DECLARE_SMART_PTR( FontTexture );

	DECLARE_SMART_PTR( Overlay );
	DECLARE_SMART_PTR( OverlayCategory );
	DECLARE_SMART_PTR( PanelOverlay );
	DECLARE_SMART_PTR( BorderPanelOverlay );
	DECLARE_SMART_PTR( TextOverlay );

	DECLARE_VECTOR( OverlaySPtr, OverlayPtr );
	DECLARE_MAP( Castor::String, OverlaySPtr, OverlayPtrStr );

	//@}
}

#endif
