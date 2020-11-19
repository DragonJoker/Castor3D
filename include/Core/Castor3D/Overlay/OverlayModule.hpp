/*
See LICENSE file in root folder
*/
#ifndef ___C3D_OverlayModule_H___
#define ___C3D_OverlayModule_H___

#include "Castor3D/Castor3DModule.hpp"

namespace castor3d
{
	/**@name Overlay */
	//@{

	/**
	*\~english
	*\brief
	*	Overlay types enumeration.
	*\~french
	*\brief
	*	Enumération des types d'overlays.
	*/
	enum class OverlayType
		: uint8_t
	{
		ePanel,
		eBorderPanel,
		eText,
		CU_ScopedEnumBounds( ePanel )
	};
	C3D_API castor::String getName( OverlayType value );
	/**
	*\~english
	*\brief
	*	Text wrapping modes, handles the way text is cut when it overflows the overlay width.
	*\~french
	*\brief
	*	Modes de découpe du texte, gère la manière dont le texte est dessiné quand il dépasse la largeur de l'incrustation.
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
	C3D_API castor::String getName( TextWrappingMode value );
	/**
	*\~english
	*\brief
	*	Text texturing modes, handles the way textures are appied to text overlays.
	*\~french
	*\brief
	*	Modes de texturage du texte, gère la manière dont les textures sont appliquées aux incrustations texte.
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
	C3D_API castor::String getName( TextTexturingMode value );
	/**
	*\~english
	*\brief
	*	Text line spacing modes, handles the way lines are spaced, which each other, inside one TextOverlay.
	*\~french
	*\brief
	*	Modes d'espacement des lignes du texte, gère la manière dont les lignes sont espacées entre elles, dans un TextOverlay.
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
	C3D_API castor::String getName( TextLineSpacingMode value );
	/**
	*\~english
	*\brief
	*	Horizontal alignments for text overlays.
	*\~french
	*\brief
	*	Alignements horizontaux pour les incrutstations texte.
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
	C3D_API castor::String getName( HAlign value );
	/**
	*\~english
	*\brief
	*	Vertical alignments for text overlays.
	*\~french
	*\brief
	*	Alignements verticaux pour les incrutstations texte.
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
	C3D_API castor::String getName( VAlign value );
	/**
	*\~english
	*\brief
	*	Text wrapping modes, handles the way text is cut when it overflows the overlay width.
	*\~french
	*\brief
	*	Modes de découpe du texte, gère la manière dont le texte est dessiné quand il dépasse la largeur de l'incrustation.
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
	C3D_API castor::String getName( BorderPosition value );
	/**
	*\~english
	*\brief
	*	A rectangular overlay with a border.
	*\remarks
	*	Uses a spceific material for the border.
	*\~french
	*\brief
	*	Une incrustation rectangulaire avec un bord.
	*\remarks
	*	Utilise un matériau spécifique pour le bord.
	*/
	class BorderPanelOverlay;
	/**
	*\~english
	*\brief
	*	Debug overlays class.
	*\remarks
	*	Contains all datas and methods to update and display debug overlays.
	*	<br />If not shown, all the methods will be deactivated.
	*\~french
	*\brief
	*	Classe de gestion des incrustations de debogage.
	*\remarks
	*	Contient toutes les données et méthodes pour mettre à jour et afficher les incrustations de débogage.
	*	<br />Si non affichés, toutes les méthodes seront désactivées.
	*/
	class DebugOverlays;
	/**
	*\~english
	*\brief
	*	Contains the font and the texture assiated to this font.
	*\~french
	*\brief
	*	Contient la polica et la texture associée.
	*/
	class FontTexture;
	/**
	*\~english
	*\brief
	*	The overlay class.
	*\remarks
	*	An overlay is a 2D element which is displayed at foreground.
	*\~french
	*\brief
	*	La classe d'incrustation.
	*\remarks
	*	Une incrustation est un élément 2D qui est affiché en premier plan.
	*/
	class Overlay;
	/**
	*\~english
	*\brief
	*	Holds specific members for an overlay category.
	*\~french
	*\brief
	*	Contient les membres spécifiques à une catégorie d'incrustation.
	*/
	class OverlayCategory;
	/**
	*\~english
	*\brief
	*	The mesh factory.
	*\~french
	*\brief
	*	La fabrique de maillages.
	*/
	class OverlayFactory;
	/**
	*\~english
	*\brief
	*	The overlay renderer.
	*\~french
	*\brief
	*	Le renderer d'incrustation.
	*/
	class OverlayRenderer;
	/**
	\~english
	\brief
	*	The overlay visitor base class.
	\~french
	\brief
	*	La classe de base de visiteur d'incrustation.
	*/
	class OverlayVisitor;
	/**
	*\~english
	*\brief
	*	A simple rectangular overlay.
	*\~french
	*\brief
	*	Une simple incrustation rectangulaire.
	*/
	class PanelOverlay;
	/**
	*\~english
	*\brief
	*	An overlay with a text.
	*\~french
	*\brief
	*	Une incrustation avec du texte.
	*/
	class TextOverlay;

	CU_DeclareCUSmartPtr( castor3d, BorderPanelOverlay, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, DebugOverlays, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, FontTexture, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, Overlay, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, OverlayCategory, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, OverlayFactory, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, OverlayRenderer, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, PanelOverlay, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, TextOverlay, C3D_API );

	CU_DeclareVector( OverlaySPtr, OverlayPtr );
	CU_DeclareMap( castor::String, OverlaySPtr, OverlayPtrStr ); 

	//@}
}

#endif
