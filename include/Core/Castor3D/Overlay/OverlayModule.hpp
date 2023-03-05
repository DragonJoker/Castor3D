/*
See LICENSE file in root folder
*/
#ifndef ___C3D_OverlayModule_H___
#define ___C3D_OverlayModule_H___

#include "Castor3D/Limits.hpp"
#include "Castor3D/Cache/CacheModule.hpp"

#include <CastorUtils/Design/ArrayView.hpp>

namespace castor3d
{
	class Scene;

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
	/**
	\~english
	\brief		A character, along with its size and relative position.
	\~french
	\brief		Un caractère, avec ses dimensions et sa position relative.
	*/
	struct TextChar
	{
		//!\~english	The character dimensions.
		//!\~french		Les dimensions du caractère.
		castor::Point2f size{};
		//!\~english	The character UV position.
		//!\~french		La position de l'UV du caractère.
		castor::Point2f uvPosition{};
		//!\~english	The glyph position relative to cursor.
		//!\~french		La position de la glyphe par rapport au curseur.
		castor::Point2f bearing{};
		//!\~english	The character position, relative to its line.
		//!\~french		La position du caractère, relative à sa ligne.
		float left{};
		//!\~english	The index of the word holding this character.
		//!\~french		L'index du mot contenant ce caractère.
		uint32_t word{};
		//!\~english	The index of the overlay holding this character.
		//!\~french		L'overlay de la ligne contenant ce caractère.
		uint32_t overlay{};
		//!\~english	The index of this character in the overlays chars.
		//!\~french		L'indice de ce caractère dans les caractères de l'incrustation.
		uint32_t index{};
	};
	/**
	\~english
	\brief		A text chars container, along with its size and position.
	\~french
	\brief		Un conteneur de caractères, avec ses dimensions et sa position.
	*/
	struct TextWord
	{
		//!\~english	The y range, relative to bearing.
		//!\~french		L'intervalle y, relatif au bearing.
		castor::Point2f range{};
		//!\~english	The word position.
		//!\~french		La position du mot.
		float left{};
		//!\~english	The word width.
		//!\~french		La longueur du mot.
		float width{};
		//!\~english	The index of the beginning of the word.
		//!\~french		L'indice sur le début du mot.
		uint32_t charBegin{};
		//!\~english	The index of the end of the word.
		//!\~french		L'indice sur la fin du mot.
		uint32_t charEnd{};
		//!\~english	The index of the line holding this word.
		//!\~french		L'index de la ligne contenant ce mot.
		uint32_t line{};
		//!\~english
		//!\~french
		uint32_t pad{};

		auto chars( std::array< TextChar, MaxCharsPerOverlay > & cont )
		{
			return castor::makeArrayView( cont.begin() + charBegin, charEnd - charBegin );
		}
	};
	/**
	\~english
	\brief		Contains the words forming the overlay.
	\~french
	\brief		Contient les mots formant l'incrustation.
	*/
	struct OverlayWords
	{
		uint32_t count{};
		uint32_t pad{};
		std::array< TextWord, MaxTextsContsPerOverlay > elems{};

		auto & getNext()
		{
			return elems[count++];
		}

		auto words()const
		{
			return castor::makeArrayView( elems.data(), elems.data() + count );
		}

		auto words()
		{
			return castor::makeArrayView( elems.begin(), count );
		}
	};
	/**
	\~english
	\brief		A text chars container, along with its size and position.
	\~french
	\brief		Un conteneur de caractères, avec ses dimensions et sa position.
	*/
	struct TextLine
	{
		//!\~english	The line position.
		//!\~french		La position de la ligne.
		castor::Point2f position{};
		//!\~english	The y range, relative to bearing.
		//!\~french		L'intervalle y, relatif au bearing.
		castor::Point2f range{};
		//!\~english	The line width.
		//!\~french		La longueur de la ligne.
		float width{};
		//!\~english	The index of the beginning of the line.
		//!\~french		L'indice sur le début de la ligne.
		uint32_t wordBegin{};
		//!\~english	The index of the end of the line.
		//!\~french		L'indice sur la fin de la ligne.
		uint32_t wordEnd{};
		//!\~english	The index of the beginning of the line.
		//!\~french		L'indice sur le début de la ligne.
		uint32_t charBegin{};
		//!\~english	The index of the end of the line.
		//!\~french		L'indice sur la fin de la ligne.
		uint32_t charEnd{};
		//!\~english
		//!\~french
		uint32_t pad{};

		auto words( OverlayWords & cont )
		{
			return castor::makeArrayView( cont.elems.begin() + wordBegin, wordEnd - wordBegin );
		}

		auto chars( std::array< TextChar, MaxCharsPerOverlay > & cont )
		{
			return castor::makeArrayView( cont.begin() + charBegin, charEnd - charBegin );
		}
	};
	/**
	\~english
	\brief		Contains the lines forming the overlay.
	\~french
	\brief		Contient les lignes formant l'incrustation.
	*/
	struct OverlayLines
	{
		castor::Point2f maxRange{};
		float topOffset{};
		uint32_t count{};
		std::array< TextLine, MaxTextsContsPerOverlay > elems{};

		auto & getNext()
		{
			return elems[count++];
		}

		auto lines()const
		{
			return castor::makeArrayView( elems.data(), elems.data() + count );
		}

		auto lines()
		{
			return castor::makeArrayView( elems.begin(), count );
		}
	};

	CU_DeclareCUSmartPtr( castor3d, BorderPanelOverlay, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, DebugOverlays, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, FontTexture, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, Overlay, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, OverlayCategory, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, OverlayFactory, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, PanelOverlay, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, TextOverlay, C3D_API );
	/**
	*\~english
	*	Helper structure to specialise a cache behaviour.
	*\remarks
	*	Specialisation for Overlay.
	*\~french
	*	Structure permettant de spécialiser le comportement d'un cache.
	*\remarks
	*	Spécialisation pour Overlay.
	*/
	template<>
	struct PtrCacheTraitsT< Overlay, castor::String >
		: PtrCacheTraitsBaseT< Overlay, castor::String >
	{
		using ResT = Overlay;
		using KeyT = castor::String;
		using Base = PtrCacheTraitsBaseT< ResT, KeyT >;
		using ElementT = typename Base::ElementT;
		using ElementPtrT = typename Base::ElementPtrT;
		using ElementObsT = typename Base::ElementObsT;

		C3D_API static const castor::String Name;
	};

	using OverlayCacheTraits = PtrCacheTraitsT< Overlay, castor::String >;
	using OverlayCache = castor::ResourceCacheT< Overlay
		, castor::String
		, OverlayCacheTraits >;

	using OverlayRes = OverlayCacheTraits::ElementPtrT;
	using OverlayResPtr = OverlayCacheTraits::ElementObsT;

	CU_DeclareCUSmartPtr( castor3d, OverlayCache, C3D_API );

	CU_DeclareVector( OverlayRPtr, OverlayPtr );
	CU_DeclareMap( castor::String, OverlayResPtr, OverlayPtrStr );

	//@}
}

namespace castor
{
	template<>
	struct ResourceCacheT< castor3d::Overlay
		, String
		, castor3d::OverlayCacheTraits >;
}

#endif
