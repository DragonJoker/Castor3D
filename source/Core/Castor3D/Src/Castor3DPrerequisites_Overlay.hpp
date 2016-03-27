/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

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
	CASTOR_TYPE( uint8_t )
	{
		eOVERLAY_TYPE_PANEL,
		eOVERLAY_TYPE_BORDER_PANEL,
		eOVERLAY_TYPE_TEXT,
		eOVERLAY_TYPE_COUNT,
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
	}	eTEXT_TEXTURING_MODE;
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
