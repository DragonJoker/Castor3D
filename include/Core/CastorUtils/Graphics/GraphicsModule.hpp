/*
See LICENSE file in root folder
*/
#ifndef ___CU_GraphicsModule_HPP___
#define ___CU_GraphicsModule_HPP___
#pragma once

#include "CastorUtils/CastorUtils.hpp"

namespace castor
{
	/**@name Graphics */
	//@{
	/**
	\~english
	\brief Pixel format enumeration
	\~french
	\brief Enumération des formats de Pixel
	*/
	enum class PixelFormat
		: uint32_t
	{
#define CUPF_ENUM_VALUE( name, value, components, alpha, colour, depth, stencil, compressed ) e##name = value,
#include "CastorUtils/Graphics/PixelFormat.enum"

		CU_ScopedEnumBounds( eUNDEFINED ),
	};
	/**
	\~english
	\brief		Predefined colours enumeration
	\~french
	\brief		Enumération de couleurs prédéfinies
	*/
	enum class PredefinedRgbaColour
		: uint32_t
	{
		eTransparentBlack = 0x00000000,
		eLowAlphaBlack = 0x00000040,
		eMedAlphaBlack = 0x0000007F,
		eHighAlphaBlack = 0x000000BF,
		eOpaqueBlack = 0x000000FF,
		eTransparentDarkBlue = 0x00007F00,
		eLowAlphaDarkBlue = 0x00007F40,
		eMedAlphaDarkBlue = 0x00007F7F,
		eHighAlphaDarkBlue = 0x00007FBF,
		eOpaqueDarkBlue = 0x00007FFF,
		eTransparentBlue = 0x0000FF00,
		eLowAlphaBlue = 0x0000FF40,
		eMedAlphaBlue = 0x0000FF7F,
		eHighAlphaBlue = 0x0000FFBF,
		eOpaqueBlue = 0x0000FFFF,
		eTransparentDarkGreen = 0x007F0000,
		eLowAlphaDarkGreen = 0x007F0040,
		eMedAlphaDarkGreen = 0x007F007F,
		eHighAlphaDarkGreen = 0x007F00BF,
		eOpaqueDarkGreen = 0x007F00FF,
		eTransparentGreen = 0x00FF0000,
		eLowAlphaGreen = 0x00FF0040,
		eMedAlphaGreen = 0x00FF007F,
		eHighAlphaGreen = 0x00FF00BF,
		eOpaqueGreen = 0x00FF00FF,
		eTransparentDarkRed = 0x7F000000,
		eLowAlphaDarkRed = 0x7F000040,
		eMedAlphaDarkRed = 0x7F00007F,
		eHighAlphaDarkRed = 0x7F0000BF,
		eOpaqueDarkRed = 0x7F0000FF,
		eTransparentRed = 0xFF000000,
		eLowAlphaRed = 0xFF000040,
		eMedAlphaRed = 0xFF00007F,
		eHighAlphaRed = 0xFF0000BF,
		eOpaqueRed = 0xFF0000FF,
		eTransparentLightBlue = 0x7F7FFF00,
		eLowAlphaLightBlue = 0x7F7FFF40,
		eMedAlphaLightBlue = 0x7F7FFF7F,
		eHighAlphaLightBlue = 0x7F7FFFBF,
		eOpaqueLightBlue = 0x7F7FFFFF,
		eTransparentLightGreen = 0x7FFF7F00,
		eLowAlphaLightGreen = 0x7FFF7F40,
		eMedAlphaLightGreen = 0x7FFF7F7F,
		eHighAlphaLightGreen = 0x7FFF7FBF,
		eOpaqueLightGreen = 0x7FFF7FFF,
		eTransparentLightRed = 0xFF7F7F00,
		eLowAlphaLightRed = 0xFF7F7F40,
		eMedAlphaLightRed = 0xFF7F7F7F,
		eHighAlphaLightRed = 0xFF7F7FBF,
		eOpaqueLightRed = 0xFF7F7FFF,
		eTransparentWhite = 0xFFFFFF00,
		eLowAlphaWhite = 0xFFFFFF40,
		eMedAlphaWhite = 0xFFFFFF7F,
		eHighAlphaWhite = 0xFFFFFFBF,
		eOpaqueWhite = 0xFFFFFFFF,
	};
	/**
	\~english
	\brief		Predefined colours enumeration
	\~french
	\brief		Enumération de couleurs prédéfinies
	*/
	enum class PredefinedRgbColour
		: uint32_t
	{
		eBlack = 0x000000FF,
		eDarkBlue = 0x00007FFF,
		eBlue = 0x0000FFFF,
		eDarkGreen = 0x007F00FF,
		eGreen = 0x00FF00FF,
		eDarkRed = 0x7F0000FF,
		eRed = 0xFF0000FF,
		eLightBlue = 0x7F7FFFFF,
		eLightGreen = 0x7FFF7FFF,
		eLightRed = 0xFF7F7FFF,
		eWhite = 0xFFFFFFFF,
	};
	/**
	\~english
	\brief		RgbaColourT components enumeration
	\~french
	\brief		Enumération des composantes d'une couleur
	*/
	enum class RgbaComponent
		: uint8_t
	{
		//!\~english	Red PixelComponents
		//!\~french		Composante rouge
		eRed,
		//!\~english	Green PixelComponents
		//!\~french		Composante verte
		eGreen,
		//!\~english	Blue PixelComponents
		//!\~french		Composante bleue
		eBlue,
		//!\~english	Alpha PixelComponents
		//!\~french		Composante alpha
		eAlpha,
		CU_ScopedEnumBounds( eRed )
	};
	/**
	\~english
	\brief		RgbColourT components enumeration
	\~french
	\brief		Enumération des composantes d'une couleur
	*/
	enum class RgbComponent
		: uint8_t
	{
		//!\~english	Red PixelComponents
		//!\~french		Composante rouge
		eRed,
		//!\~english	Green PixelComponents
		//!\~french		Composante verte
		eGreen,
		//!\~english	Blue PixelComponents
		//!\~french		Composante bleue
		eBlue,
		CU_ScopedEnumBounds( eRed )
	};
	/**
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\~english
	\brief		Intersection types
	\remark		Enumerates the different intersection types.
	\~french
	\brief		Types d'intersection
	\remark		Enumère les différents types d'intersection.
	*/
	enum class Intersection
		: uint8_t
	{
		//!\~english	Completely inside.
		//!\~french		Complètement à l'intérieur.
		eIn,
		//!\~english	Completely outside.
		//!\~french		Complètement à l'extérieur.
		eOut,
		//!\~english	Intersection.
		//!\~french		Intersection
		eIntersect,
		CU_ScopedEnumBounds( eIn )
	};
	/**
	\~english
	\brief		The memory layout for an image.
	\~french
	\brief		Le layout mémoire d'une image.
	*/
	struct ImageLayout;
	/**
	\~english
	\brief		Box container class.
	\remark		A container will be a simple object which surrounds a graphic object (2D or 3D).
				<br />It can be a parallelepiped, a sphere or other.
	\~french
	\brief		Classe de conteneur boîte.
	\remark		Un conteneur boîte est un simple objet encadrant un objet graphique (2D ou 3D).
				<br />Ce peut être un parallélépipède, une sphère ou autre.
	*/
	template< uint8_t Dimension >
	class BoundingContainer;
	/**
	\~english
	\brief		Typedef on a 2 dimensions BoundingContainer.
	\~french
	\brief		Typedef sur une BoundingContainer à 2 dimensions.
	*/
	using BoundingContainer2D = BoundingContainer< 2 >;
	/**
	\~english
	\brief		Typedef on a 3 dimensions BoundingContainer.
	\~french
	\brief		Typedef sur une BoundingContainer à 3 dimensions.
	*/
	using BoundingContainer3D = BoundingContainer< 3 >;
	/**
	\~english
	\brief		Bounding box class.
	\~french
	\brief		Classe de bounding box.
	*/
	class BoundingBox;
	/**
	\~english
	\brief		Sphere container class.
	\~french
	\brief		Classe de conteneur sphérique.
	*/
	class BoundingSphere;
	/**
	\~english
	\brief		Defines a colour PixelComponents (R, G, B or A) to be used in castor::RgbColour or castor::RgbaColour.
	\remark		Holds conversion operators to be converted either into float or uint8_t, with corresponding operations.
				<br />A colour PixelComponents value is a floating number between 0.0 and 1.0.
	\~french
	\brief		Représente une composante de couleur (R, V, B ou A) pour l'utilisation dans castor::RgbColour ou castor::RgbaColour.
	\remark		Définit les opérateurs de conversion en float ou uint8_t, avec les opérations correspondantes.
				<br />La valeur d'une composante de couleur est un nombre flottant compris entre 0.0 et 1.0.
	*/
	class ColourComponent;
	/**
	\~english
	\brief		Defines a HDR colour PixelComponents (R, G, B or A) to be used in castor::HdrRgbColour or castor::HdrRgbaColour.
	\remark		Holds conversion operators to be converted either into float or uint8_t, with corresponding operations
	\~french
	\brief		Représente une composante de couleur HDR (R, V, B ou A) pour l'utilisation dans castor::HdrRgbColour ou castor::HdrRgbaColour.
	\remark		Définit les opérateurs de conversion en float ou uint8_t, avec les opérations correspondantes.
	*/
	class HdrColourComponent;
	/**
	\~english
	\brief		Font resource
	\remark		Representation of a font: face, precision, and others
				<br />The generated font will be put in an image
	\~french
	\brief		Ressource Font
	\remark		Représentation d'une font : face, précision, et autres
				<br />La font chargée sera placée dans une image
	*/
	class Font;
	/**
	\~english
	\brief		Font manager.
	\remark		Holds the loaded fonts, and also the paths to font files.
	\~french
	\brief		Gestionnaire de polices.
	\remark		Détient les polices, et aussi les chemins d'accès aux fichiers des polices.
	*/
	class FontCache;
	/**
	\~english
	\brief		Font character (glyph) representation
	\remark		Holds position, size and data of a character
	\~french
	\brief		Représentation d'un caractère d'une Font (glyphe)
	\remark		Contient la position, taille et pixels d'un caractère
	*/
	class Glyph;
	/**
	\~english
	\brief		Scalable and movable grid.
	\~french
	\brief		Grille redimensionnable et déplaçable.
	*/
	class Grid;
	/**
	\~english
	\brief		Image resource
	\~french
	\brief		Ressource Image
	*/
	class Image;
	/**
	\~english
	\brief		Image manager.
	\remark		Holds the loaded images, and helps their loading.
	\~french
	\brief		Gestionnaire d'images.
	\remark		Détient les images, et simplifie leur création.
	*/
	class ImageCache;
	/**
	\~english
	\brief		An image loader.
	\~french
	\brief		Un loader d'image.
	*/
	class ImageLoaderImpl;
	/**
	\~english
	\brief		Registers the image loaders, per file extension.
	\~french
	\brief		Enregistre les loaders d'image, par extension de fichier.
	*/
	class ImageLoader;
	/**
	\~english
	\brief		An image writer.
	\~french
	\brief		Un writer d'image.
	*/
	class ImageWriterImpl;
	/**
	\~english
	\brief		Registers the image writers, per file extension.
	\~french
	\brief		Enregistre les writers d'image, par extension de fichier.
	*/
	class ImageWriter;
	/**
	\~english
	\brief		Pixel definition.
	\remark		Takes a PixelFormat as a template argument to determine size anf format.
	\~french
	\brief		Définition d'un pixel.
	\remark		L'argument template PixelFormat sert a déterminer la taille et le format du pixel.
	*/
	template< PixelFormat PixelFormatT >
	class Pixel;
	/**
	\author 	Sylvain DOREMUS
	\~english
	\brief		Typedef over a pixel in A8R8G8B8 format
	\~french
	\brief		Typedef sur un pixel au format A8R8G8B8
	*/
	using UbPixel = Pixel< PixelFormat::eR8G8B8A8_UNORM >;
	/**
	\~english
	\brief		Configuration options for Pixel buffer conversion/compression.
	\~french
	\brief		Options de configuration de conversion/compression d'un buffer de Pixel.
	*/
	struct PxBufferConvertOptions;
	/**
	\~english
	\brief		Pixel buffer base definition.
	\~french
	\brief		Définition de la classe de base d'un buffer de Pixel.
	*/
	class PxBufferBase;
	/**
	\~english
	\brief		Pixel buffer class, with pixel format as a template param
	\~french
	\brief		Buffer de pixels avec le format de pixel en argument template
	*/
	template< PixelFormat PixelFormatT >
	class PxBuffer;
	/**
	\~english
	\brief		Pixel buffer iterator.
	\~french
	\brief		Itérateur de buffer de pixels.
	*/
	template< PixelFormat PixelFormatT >
	struct ConstPixelIterator;
	/**
	\~english
	\brief		Pixel buffer iterator.
	\~french
	\brief		Itérateur de buffer de pixels.
	*/
	template< PixelFormat PF >
	struct PixelIterator;
	/**
	\~english
	\brief Pixel PixelComponents enumeration.
	\~french
	\brief Enumération des composantes de Pixel.
	*/
	enum class PixelComponent
		: uint8_t
	{
		//!\~english	Red.
		//!\~french		Rouge.
		eRed,
		//!\~english	Green.
		//!\~french		Vert.
		eGreen,
		//!\~english	Blue.
		//!\~french		Bleu.
		eBlue,
		//!\~english	Alpha.
		//!\~french		Alpha.
		eAlpha,
		//!\~english	Depth.
		//!\~french		Profondeur.
		eDepth,
		//!\~english	Stencil.
		//!\~french		Stencil.
		eStencil,
		CU_ScopedEnumBounds( eRed )
	};
	/**
	\author 	Sylvain DOREMUS
	\date 		27/05/2013
	\~english
	\brief		Holds colour/depth/stencil PixelComponents helper functions
	\~french
	\brief		Contient des fonctions d'aides à la conversion de composantes couleur/profondeur/stencil
	*/
	template< PixelFormat PixelFormatT >
	struct PixelComponents;
	/**
	 *\~english
	 *\brief		Helper struct that holds pixel size, toString and converion functions
	 *\~french
	 *\brief		Structure d'aide contenant la taille d'un pixel ainsi que sa fonction toString et les fonction de conversion vers d'autres formats
	 */
	template< PixelFormat PixelFormatT >
	struct PixelDefinitions;
	/**
	 *\~english
	 *\brief		Helper struct to tell if a pixel format represents a colour
	 *\~french
	 *\brief		Structure d'aide permettant de dire si un format de pixels représente une couleur
	 */
	template< PixelFormat PixelFormatT >
	struct IsColourFormat
		: public std::true_type
	{
	};
	template< PixelFormat PixelFormatT >
	static bool constexpr isColourFormatV = IsColourFormat< PixelFormatT >::value;
	/**
	 *\~english
	 *\brief		Helper struct to tell if a pixel format represents a depth pixel
	 *\~french
	 *\brief		Structure d'aide permettant de dire si un format de pixels représente un pixel de profondeur
	 */
	template< PixelFormat PixelFormatT >
	struct IsDepthFormat
		: public std::false_type
	{
	};
	template< PixelFormat PixelFormatT >
	static bool constexpr isDepthFormatV = IsDepthFormat< PixelFormatT >::value;
	/**
	 *\~english
	 *\brief		Helper struct to tell if a pixel format represents a stencil pixel
	 *\~french
	 *\brief		Structure d'aide permettant de dire si un format de pixels représente un pixel de stencil
	 */
	template< PixelFormat PixelFormatT >
	struct IsStencilFormat
		: public std::false_type
	{
	};
	template< PixelFormat PixelFormatT >
	static bool constexpr isStencilFormatV = IsStencilFormat< PixelFormatT >::value;
	/**
	 *\~english
	 *\brief		Helper struct to tell if a pixel format represents a depth or stencil pixel
	 *\~french
	 *\brief		Structure d'aide permettant de dire si un format de pixels représente un pixel de profondeur ou de stencil
	 */
	template< PixelFormat PixelFormatT >
	struct IsDepthStencilFormat
		: public std::false_type
	{
	};
	template< PixelFormat PixelFormatT >
	static bool constexpr isDepthStencilFormatV = IsDepthStencilFormat< PixelFormatT >::value;
	/**
	 *\~english
	 *\brief		Helper struct to tell if a pixel format is compressed.
	 *\~french
	 *\brief		Structure d'aide permettant de dire si un format de pixels est compressé.
	 */
	template< PixelFormat PixelFormatT >
	struct IsCompressed
		: public std::false_type
	{
	};
	template< PixelFormat PixelFormatT >
	static bool constexpr isCompressedV = IsCompressed< PixelFormatT >::value;
	/**
	 *\~english
	 *\brief		Helper struct to tell if a pixel format is compressed.
	 *\~french
	 *\brief		Structure d'aide permettant de dire si un format de pixels est compressé.
	 */
	template< PixelFormat PixelFormatT >
	struct HasAlpha
		: public std::false_type
	{
	};
	template< PixelFormat PixelFormatT >
	static bool constexpr hasAlphaV = HasAlpha< PixelFormatT >::value;
	/**
	 *\~english
	 *\brief		Helper struct to tell if a pixel format uses unsigned 8 bits components.
	 *\~french
	 *\brief		Structure d'aide permettant de dire si un format de pixels utilise des composantes en 8 bits unsigned.
	 */
	template< PixelFormat PixelFormatT >
	struct Is8UComponents
		: public std::false_type
	{
	};
	template< PixelFormat PixelFormatT >
	static bool constexpr is8UComponentsV = Is8UComponents< PixelFormatT >::value;
	/**
	 *\~english
	 *\brief		Helper struct to tell if a pixel format uses unsigned 16 bits components.
	 *\~french
	 *\brief		Structure d'aide permettant de dire si un format de pixels utilise des composantes en 16 bits unsigned.
	 */
	template< PixelFormat PixelFormatT >
	struct Is16UComponents
		: public std::false_type
	{
	};
	template< PixelFormat PixelFormatT >
	static bool constexpr is16UComponentsV = Is16UComponents< PixelFormatT >::value;
	/**
	 *\~english
	 *\brief		Helper struct to tell if a pixel format uses unsigned 32 bits components.
	 *\~french
	 *\brief		Structure d'aide permettant de dire si un format de pixels utilise des composantes en 32 bits unsigned.
	 */
	template< PixelFormat PixelFormatT >
	struct Is32UComponents
		: public std::false_type
	{
	};
	template< PixelFormat PixelFormatT >
	static bool constexpr is32UComponentsV = Is32UComponents< PixelFormatT >::value;
	/**
	 *\~english
	 *\brief		Helper struct to tell if a pixel format uses unsigned 64 bits components.
	 *\~french
	 *\brief		Structure d'aide permettant de dire si un format de pixels utilise des composantes en 64 bits unsigned.
	 */
	template< PixelFormat PixelFormatT >
	struct Is64UComponents
		: public std::false_type
	{
	};
	template< PixelFormat PixelFormatT >
	static bool constexpr is64UComponentsV = Is64UComponents< PixelFormatT >::value;
	/**
	 *\~english
	 *\brief		Helper struct to tell if a pixel format uses signed 8 bits components.
	 *\~french
	 *\brief		Structure d'aide permettant de dire si un format de pixels utilise des composantes en 8 bits signed.
	 */
	template< PixelFormat PixelFormatT >
	struct Is8SComponents
		: public std::false_type
	{
	};
	template< PixelFormat PixelFormatT >
	static bool constexpr is8SComponentsV = Is8SComponents< PixelFormatT >::value;
	/**
	 *\~english
	 *\brief		Helper struct to tell if a pixel format uses signed 16 bits components.
	 *\~french
	 *\brief		Structure d'aide permettant de dire si un format de pixels utilise des composantes en 16 bits signed.
	 */
	template< PixelFormat PixelFormatT >
	struct Is16SComponents
		: public std::false_type
	{
	};
	template< PixelFormat PixelFormatT >
	static bool constexpr is16SComponentsV = Is16SComponents< PixelFormatT >::value;
	/**
	 *\~english
	 *\brief		Helper struct to tell if a pixel format uses signed 32 bits components.
	 *\~french
	 *\brief		Structure d'aide permettant de dire si un format de pixels utilise des composantes en 32 bits signed.
	 */
	template< PixelFormat PixelFormatT >
	struct Is32SComponents
		: public std::false_type
	{
	};
	template< PixelFormat PixelFormatT >
	static bool constexpr is32SComponentsV = Is32SComponents< PixelFormatT >::value;
	/**
	 *\~english
	 *\brief		Helper struct to tell if a pixel format uses signed 64 bits components.
	 *\~french
	 *\brief		Structure d'aide permettant de dire si un format de pixels utilise des composantes en 64 bits signed.
	 */
	template< PixelFormat PixelFormatT >
	struct Is64SComponents
		: public std::false_type
	{
	};
	template< PixelFormat PixelFormatT >
	static bool constexpr is64SComponentsV = Is64SComponents< PixelFormatT >::value;
	/**
	 *\~english
	 *\brief		Helper struct to tell if a pixel format uses floating point 16 bits components.
	 *\~french
	 *\brief		Structure d'aide permettant de dire si un format de pixels utilise des composantes en 16 bits floating point.
	 */
	template< PixelFormat PixelFormatT >
	struct Is16FComponents
		: public std::false_type
	{
	};
	template< PixelFormat PixelFormatT >
	static bool constexpr is16FComponentsV = Is16FComponents< PixelFormatT >::value;
	/**
	 *\~english
	 *\brief		Helper struct to tell if a pixel format uses floating point 32 bits components.
	 *\~french
	 *\brief		Structure d'aide permettant de dire si un format de pixels utilise des composantes en 32 bits floating point.
	 */
	template< PixelFormat PixelFormatT >
	struct Is32FComponents
		: public std::false_type
	{
	};
	template< PixelFormat PixelFormatT >
	static bool constexpr is32FComponentsV = Is32FComponents< PixelFormatT >::value;
	/**
	 *\~english
	 *\brief		Helper struct to tell if a pixel format uses floating point 64 bits components.
	 *\~french
	 *\brief		Structure d'aide permettant de dire si un format de pixels utilise des composantes en 64 bits floating point.
	 */
	template< PixelFormat PixelFormatT >
	struct Is64FComponents
		: public std::false_type
	{
	};
	template< PixelFormat PixelFormatT >
	static bool constexpr is64FComponentsV = Is64FComponents< PixelFormatT >::value;
	/**
	 *\~english
	 *\brief		Helper struct to tell if a pixel format uses unsigned 16 bits depth component.
	 *\~french
	 *\brief		Structure d'aide permettant de dire si un format de pixels utilise une composante profondeur en 16 bits unsigned.
	 */
	template< PixelFormat PixelFormatT >
	struct IsD16UComponent
		: public std::false_type
	{
	};
	template< PixelFormat PixelFormatT >
	static bool constexpr isD16UComponentV = IsD16UComponent< PixelFormatT >::value;
	/**
	 *\~english
	 *\brief		Helper struct to tell if a pixel format uses unsigned 24 bits depth component.
	 *\~french
	 *\brief		Structure d'aide permettant de dire si un format de pixels utilise une composante profondeur en 24 bits unsigned.
	 */
	template< PixelFormat PixelFormatT >
	struct IsD24UComponent
		: public std::false_type
	{
	};
	template< PixelFormat PixelFormatT >
	static bool constexpr isD24UComponentV = IsD24UComponent< PixelFormatT >::value;
	/**
	 *\~english
	 *\brief		Helper struct to tell if a pixel format uses floating point 32 bits depth component.
	 *\~french
	 *\brief		Structure d'aide permettant de dire si un format de pixels utilise une composante profondeur en 32 bits floating point.
	 */
	template< PixelFormat PixelFormatT >
	struct IsD32FComponent
		: public std::false_type
	{
	};
	template< PixelFormat PixelFormatT >
	static bool constexpr isD32FComponentV = IsD32FComponent< PixelFormatT >::value;
	/**
	 *\~english
	 *\brief		Helper struct to tell if a pixel format uses only an unsigned 8 bits stencil component.
	 *\~french
	 *\brief		Structure d'aide permettant de dire si un format de pixels utilise uniquement une composante stencil en 8 bits stencil.
	 */
	template< PixelFormat PixelFormatT >
	struct IsS8UComponent
		: public std::false_type
	{
	};
	template< PixelFormat PixelFormatT >
	static bool constexpr isS8UComponentV = IsS8UComponent< PixelFormatT >::value;
	/**
	\~english
	\brief		Position class
	\remark		Kind of specialisation of Coords< 2, int32_t >
	\~french
	\brief		Classe de position
	\remark		Sorte de spécialisation de Coords< 2, int32_t >
	*/
	class Position;
	/**
	\~english
	\brief		Rectangle class
	\remark		Rectangle class, inherits from Point, holds the intersection functions and specific accessors
	\~french
	\brief		Classe représentant un rectangle
	\remark		Dérive de Point, gère les intersections entre rectangles et les accesseurs spécifiques
	*/
	class Rectangle;
	/**
	\~english
	\brief		RGBA colour representation
	\remark		Predefines numerous colours
				<br />Allows conversion to different colour formats (RGB, ARGB, BGR, ...) and different data types (long, float, char, ...)
	\~french
	\brief		Représentation d'une couleur RGBA
	\remark		Prédéfinit differentes couleurs
				<br />Permet la conversion entre différents types de couleurs (RGB, ARGB, BGR, ...) et différents types de données (long, float, char, ...)
	*/
	template< typename ComponentType >
	class RgbaColourT;
	/**
	\~english
	\brief		RGB colour representation
	\remark		Predefines numerous colours
				<br />Allows conversion to different colour formats (RGB, ARGB, BGR, ...) and different data types (long, float, char, ...)
	\~french
	\brief		Représentation d'une couleur RGB
	\remark		Prédéfinit differentes couleurs
				<br />Permet la conversion entre différents types de couleurs (RGB, ARGB, BGR, ...) et différents types de données (long, float, char, ...)
	*/
	template< typename ComponentType >
	class RgbColourT;
	/**
	\~english
	\brief		Size class
	\remark		Kind of specialisation of Coords< 2, uint32_t >
	\~french
	\brief		Classe de taille
	\remark		Sorte de spécialisation de Coords< 2, uint32_t >
	*/
	class Size;
	/**
	\~english
	\brief		Unsupported format exception
	\~french
	\brief		Unsupported format exception
	*/
	class UnsupportedFormatException;

	using ImageLoaderPtr = std::unique_ptr< ImageLoaderImpl >;
	using ImageWriterPtr = std::unique_ptr< ImageWriterImpl >;
	using PixelBuffer = PxBuffer< PixelFormat::eR8G8B8A8_UNORM >;

	CU_DeclareSmartPtr( BoundingBox );
	CU_DeclareSmartPtr( BoundingSphere );
	CU_DeclareSmartPtr( Image );
	CU_DeclareSmartPtr( Font );
	CU_DeclareSmartPtr( PxBufferBase );

	using RgbColour = RgbColourT< ColourComponent >;
	using RgbaColour = RgbaColourT< ColourComponent >;
	using HdrRgbColour = RgbColourT< HdrColourComponent >;
	using HdrRgbaColour = RgbaColourT< HdrColourComponent >;
	//@}
}

#endif
