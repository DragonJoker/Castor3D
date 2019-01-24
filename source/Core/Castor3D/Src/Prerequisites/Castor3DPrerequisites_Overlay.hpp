/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PREREQUISITES_OVERLAY_H___
#define ___C3D_PREREQUISITES_OVERLAY_H___

namespace castor3d
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
	enum class OverlayType
		: uint8_t
	{
		ePanel,
		eBorderPanel,
		eText,
		CU_ScopedEnumBounds( ePanel )
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		28/11/2014
	\~english
	\brief		Text wrapping modes, handles the way text is cut when it overflows the overlay width
	\~french
	\brief		Modes de découpe du texte, gère la manière dont le texte est dessiné quand il dépasse la largeur de l'incrustation
	*/
	enum class TextWrappingMode
	{
		//!\~english	The text is cut.
		//!\~french		Le texte qui dépasse est découpé.
		eNone,
		//!\~english	The text jumps to next line.
		//!\~french		Le texte passe à la ligne suivante.
		eBreak,
		//!\~english	The text jumps to next line without cutting words.
		//!\~french		Le texte passe à la ligne suivante, sans découper les mots.
		eBreakWords,
		CU_ScopedEnumBounds( eNone )
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		26/03/2016
	\~english
	\brief		Text texturing modes, handles the way textures are appied to text overlays.
	\~french
	\brief		Modes de texturage du texte, gère la manière dont les textures sont appliquées aux incrustations texte.
	*/
	enum class TextTexturingMode
	{
		//!\~english	The texture is repeated on each character of the text.
		//!\~french		La texture est répétée sut chaque caractère du texte.
		eLetter,
		//!\~english	The texture is applied on the whole text.
		//!\~french		La texture est appliquée sur tout le texte.
		eText,
		CU_ScopedEnumBounds( eLetter )
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		16/04/2016
	\~english
	\brief		Text line spacing modes, handles the way lines are spaced, which each other, inside one TextOverlay.
	\~french
	\brief		Modes d'espacement des lignes du texte, gère la manière dont les lignes sont espacées entre elles, dans un TextOverlay.
	*/
	enum class TextLineSpacingMode
	{
		//!\~english	The height of each line is its own height.
		//!\~french		La hauteur d'une ligne est sa propre hauteur.
		eOwnHeight,
		//!\~english	The height of the lines is the maximum height between them.
		//!\~french		La hauteur des lignes est la plus grande hauteur parmi elles.
		eMaxLineHeight,
		//!\~english	The height of the lines is the maximum height from the font's characters.
		//!\~french		La hauteur des lignes est la plus grande hauteur des caractères de la police.
		eMaxFontHeight,
		CU_ScopedEnumBounds( eOwnHeight )
	};
	/*!
	\author		Sylvain DOREMUS
	\date		23/09/2015
	\version	0.8.0
	\~english
	\brief		Horizontal alignments for text overlays.
	\~french
	\brief		Alignemens horizontaux pour les incrutstations texte.
	*/
	enum class HAlign
	{
		//!\~english	Aligned on the left.
		//!\~french		Aligné à gauche.
		eLeft,
		//!\~english	Centered horizontally.
		//!\~french		Centré, horizontalement.
		eCenter,
		//!\~english	Aligned on the right.
		//!\~french		Aligné à droite.
		eRight,
		CU_ScopedEnumBounds( eLeft )
	};
	/*!
	\author		Sylvain DOREMUS
	\date		23/09/2015
	\version	0.8.0
	\~english
	\brief		Vertical alignments for text overlays.
	\~french
	\brief		Alignemens verticaux pour les incrutstations texte.
	*/
	enum class VAlign
	{
		//!\~english	Aligned on the top.
		//!\~french		Aligné en haut.
		eTop,
		//!\~english	Centered vertically.
		//!\~french		Centré, verticalement.
		eCenter,
		//!\~english	Aligned on the bottom.
		//!\~french		Aligné en bas.
		eBottom,
		CU_ScopedEnumBounds( eTop )
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		28/11/2014
	\~english
	\brief		Text wrapping modes, handles the way text is cut when it overflows the overlay width
	\~french
	\brief		Modes de découpe du texte, gère la manière dont le texte est dessiné quand il dépasse la largeur de l'incrustation
	*/
	enum class BorderPosition
	{
		//!\~english	The border is outside the overlay.
		//!\~french		La bordure est à l'intérieur de l'incrustation.
		eInternal,
		//!\~english	The border is middle outside and middle inside the overlay.
		//!\~french		La bordure est à moitié à l'intérieur, à moitié à l'extérieur de l'incrustation.
		eMiddle,
		//!\~english	The border is outside the overlay.
		//!\~french		La bordure est à l'extérieur de l'incrustation.
		eExternal,
		CU_ScopedEnumBounds( eInternal )
	};

	class Overlay;
	class OverlayCategory;
	class PanelOverlay;
	class BorderPanelOverlay;
	class TextOverlay;
	class OverlayFactory;
	class OverlayRenderer;
	class FontTexture;

	CU_DeclareSmartPtr( OverlayFactory );
	CU_DeclareSmartPtr( OverlayRenderer );
	CU_DeclareSmartPtr( FontTexture );

	CU_DeclareSmartPtr( Overlay );
	CU_DeclareSmartPtr( OverlayCategory );
	CU_DeclareSmartPtr( PanelOverlay );
	CU_DeclareSmartPtr( BorderPanelOverlay );
	CU_DeclareSmartPtr( TextOverlay );

	CU_DeclareVector( OverlaySPtr, OverlayPtr );
	CU_DeclareMap( castor::String, OverlaySPtr, OverlayPtrStr );

	//@}
}

#endif
