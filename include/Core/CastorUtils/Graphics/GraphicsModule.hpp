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
	using X8UGetter = uint8_t( * )( uint8_t const * );
	using X8SGetter = int8_t( * )( uint8_t const * );
	using X32FGetter = float( * )( uint8_t const * );
	/**
	\~english
	\brief Pixel format enumeration
	\~french
	\brief Enumération des formats de Pixel
	*/
	enum class PixelFormat
		: uint32_t
	{
		eUNDEFINED = 0,

		eR4G4_UNORM = 1,
		eR4G4B4A4_UNORM = 2,
		eB4G4R4A4_UNORM = 3,
		eR5G6B5_UNORM = 4,
		eB5G6R5_UNORM = 5,
		eR5G5B5A1_UNORM = 6,
		eB5G5R5A1_UNORM = 7,
		eA1R5G5B5_UNORM = 8,

		eR8_UNORM = 9,
		eR8_SNORM = 10,
		eR8_USCALED = 11,
		eR8_SSCALED = 12,
		eR8_UINT = 13,
		eR8_SINT = 14,
		eR8_SRGB = 15,

		eR8G8_UNORM = 16,
		eR8G8_SNORM = 17,
		eR8G8_USCALED = 18,
		eR8G8_SSCALED = 19,
		eR8G8_UINT = 20,
		eR8G8_SINT = 21,
		eR8G8_SRGB = 22,

		eR8G8B8_UNORM = 23,
		eR8G8B8_SNORM = 24,
		eR8G8B8_USCALED = 25,
		eR8G8B8_SSCALED = 26,
		eR8G8B8_UINT = 27,
		eR8G8B8_SINT = 28,
		eR8G8B8_SRGB = 29,

		eB8G8R8_UNORM = 30,
		eB8G8R8_SNORM = 31,
		eB8G8R8_USCALED = 32,
		eB8G8R8_SSCALED = 33,
		eB8G8R8_UINT = 34,
		eB8G8R8_SINT = 35,
		eB8G8R8_SRGB = 36,

		eR8G8B8A8_UNORM = 37,
		eR8G8B8A8_SNORM = 38,
		eR8G8B8A8_USCALED = 39,
		eR8G8B8A8_SSCALED = 40,
		eR8G8B8A8_UINT = 41,
		eR8G8B8A8_SINT = 42,
		eR8G8B8A8_SRGB = 43,

		eB8G8R8A8_UNORM = 44,
		eB8G8R8A8_SNORM = 45,
		eB8G8R8A8_USCALED = 46,
		eB8G8R8A8_SSCALED = 47,
		eB8G8R8A8_UINT = 48,
		eB8G8R8A8_SINT = 49,
		eB8G8R8A8_SRGB = 50,

		eA8B8G8R8_UNORM = 51,
		eA8B8G8R8_SNORM = 52,
		eA8B8G8R8_USCALED = 53,
		eA8B8G8R8_SSCALED = 54,
		eA8B8G8R8_UINT = 55,
		eA8B8G8R8_SINT = 56,
		eA8B8G8R8_SRGB = 57,

		eA2R10G10B10_UNORM = 58,
		eA2R10G10B10_SNORM = 59,
		eA2R10G10B10_USCALED = 60,
		eA2R10G10B10_SSCALED = 61,
		eA2R10G10B10_UINT = 62,
		eA2R10G10B10_SINT = 63,

		eA2B10G10R10_UNORM = 64,
		eA2B10G10R10_SNORM = 65,
		eA2B10G10R10_USCALED = 66,
		eA2B10G10R10_SSCALED = 67,
		eA2B10G10R10_UINT = 68,
		eA2B10G10R10_SINT = 69,

		eR16_UNORM = 70,
		eR16_SNORM = 71,
		eR16_USCALED = 72,
		eR16_SSCALED = 73,
		eR16_UINT = 74,
		eR16_SINT = 75,
		eR16_SFLOAT = 76,

		eR16G16_UNORM = 77,
		eR16G16_SNORM = 78,
		eR16G16_USCALED = 79,
		eR16G16_SSCALED = 80,
		eR16G16_UINT = 81,
		eR16G16_SINT = 82,
		eR16G16_SFLOAT = 83,

		eR16G16B16_UNORM = 84,
		eR16G16B16_SNORM = 85,
		eR16G16B16_USCALED = 86,
		eR16G16B16_SSCALED = 87,
		eR16G16B16_UINT = 88,
		eR16G16B16_SINT = 89,
		eR16G16B16_SFLOAT = 90,

		eR16G16B16A16_UNORM = 91,
		eR16G16B16A16_SNORM = 92,
		eR16G16B16A16_USCALED = 93,
		eR16G16B16A16_SSCALED = 94,
		eR16G16B16A16_UINT = 95,
		eR16G16B16A16_SINT = 96,
		eR16G16B16A16_SFLOAT = 97,

		eR32_UINT = 98,
		eR32_SINT = 99,
		eR32_SFLOAT = 100,

		eR32G32_UINT = 101,
		eR32G32_SINT = 102,
		eR32G32_SFLOAT = 103,

		eR32G32B32_UINT = 104,
		eR32G32B32_SINT = 105,
		eR32G32B32_SFLOAT = 106,

		eR32G32B32A32_UINT = 107,
		eR32G32B32A32_SINT = 108,
		eR32G32B32A32_SFLOAT = 109,

		eR64_UINT = 110,
		eR64_SINT = 111,
		eR64_SFLOAT = 112,

		eR64G64_UINT = 113,
		eR64G64_SINT = 114,
		eR64G64_SFLOAT = 115,

		eR64G64B64_UINT = 116,
		eR64G64B64_SINT = 117,
		eR64G64B64_SFLOAT = 118,

		eR64G64B64A64_UINT = 119,
		eR64G64B64A64_SINT = 120,
		eR64G64B64A64_SFLOAT = 121,

		eB10G11R11_UFLOAT = 122,
		eE5B9G9R9_UFLOAT = 123,

		eD16_UNORM = 124,
		eX8_D24_UNORM = 125,
		eD32_SFLOAT = 126,
		eS8_UINT = 127,
		eD16_UNORM_S8_UINT = 128,
		eD24_UNORM_S8_UINT = 129,
		eD32_SFLOAT_S8_UINT = 130,

		eBC1_RGB_UNORM_BLOCK = 131, // RGB_DXT1
		eBC1_RGB_SRGB_BLOCK = 132, // RGB_DXT1
		eBC1_RGBA_UNORM_BLOCK = 133, // RGBA_DXT1
		eBC1_RGBA_SRGB_BLOCK = 134, // RGBA_DXT1
		eBC2_UNORM_BLOCK = 135, // RGBA_DXT3
		eBC2_SRGB_BLOCK = 136, // RGBA_DXT3
		eBC3_UNORM_BLOCK = 137, // RGBA_DXT5
		eBC3_SRGB_BLOCK = 138, // RGBA_DXT5
		eBC4_UNORM_BLOCK = 139, // R_ATI1N
		eBC4_SNORM_BLOCK = 140, // R_ATI1N
		eBC5_UNORM_BLOCK = 141, // RG_ATI2N
		eBC5_SNORM_BLOCK = 142, // RG_ATI2N
		eBC6H_UFLOAT_BLOCK = 143, // RGB_BP
		eBC6H_SFLOAT_BLOCK = 144, // RGB_BP
		eBC7_UNORM_BLOCK = 145, // RGBA_BP
		eBC7_SRGB_BLOCK = 146, // RGBA_BP

		eETC2_R8G8B8_UNORM_BLOCK = 147,
		eETC2_R8G8B8_SRGB_BLOCK = 148,
		eETC2_R8G8B8A1_UNORM_BLOCK = 149,
		eETC2_R8G8B8A1_SRGB_BLOCK = 150,
		eETC2_R8G8B8A8_UNORM_BLOCK = 151,
		eETC2_R8G8B8A8_SRGB_BLOCK = 152,
		eEAC_R11_UNORM_BLOCK = 153,
		eEAC_R11_SNORM_BLOCK = 154,
		eEAC_R11G11_UNORM_BLOCK = 155,
		eEAC_R11G11_SNORM_BLOCK = 156,

		eASTC_4x4_UNORM_BLOCK = 157,
		eASTC_4x4_SRGB_BLOCK = 158,
		eASTC_5x4_UNORM_BLOCK = 159,
		eASTC_5x4_SRGB_BLOCK = 160,
		eASTC_5x5_UNORM_BLOCK = 161,
		eASTC_5x5_SRGB_BLOCK = 162,
		eASTC_6x5_UNORM_BLOCK = 163,
		eASTC_6x5_SRGB_BLOCK = 164,
		eASTC_6x6_UNORM_BLOCK = 165,
		eASTC_6x6_SRGB_BLOCK = 166,
		eASTC_8x5_UNORM_BLOCK = 167,
		eASTC_8x5_SRGB_BLOCK = 168,
		eASTC_8x6_UNORM_BLOCK = 169,
		eASTC_8x6_SRGB_BLOCK = 170,
		eASTC_8x8_UNORM_BLOCK = 171,
		eASTC_8x8_SRGB_BLOCK = 172,
		eASTC_10x5_UNORM_BLOCK = 173,
		eASTC_10x5_SRGB_BLOCK = 174,
		eASTC_10x6_UNORM_BLOCK = 175,
		eASTC_10x6_SRGB_BLOCK = 176,
		eASTC_10x8_UNORM_BLOCK = 177,
		eASTC_10x8_SRGB_BLOCK = 178,
		eASTC_10x10_UNORM_BLOCK = 179,
		eASTC_10x10_SRGB_BLOCK = 180,
		eASTC_12x10_UNORM_BLOCK = 181,
		eASTC_12x10_SRGB_BLOCK = 182,
		eASTC_12x12_UNORM_BLOCK = 183,
		eASTC_12x12_SRGB_BLOCK = 184,
		CU_ScopedEnumBounds( eUNDEFINED ),

		eR8A8_UNORM = eR8G8_UNORM,
		eR8A8_SNORM = eR8G8_SNORM,
		eR8A8_USCALED = eR8G8_USCALED,
		eR8A8_SSCALED = eR8G8_SSCALED,
		eR8A8_UINT = eR8G8_UINT,
		eR8A8_SINT = eR8G8_SINT,
		eR8A8_SRGB = eR8G8_SRGB,

		eR16A16_UNORM = eR16G16_UNORM,
		eR16A16_SNORM = eR16G16_SNORM,
		eR16A16_USCALED = eR16G16_USCALED,
		eR16A16_SSCALED = eR16G16_SSCALED,
		eR16A16_UINT = eR16G16_UINT,
		eR16A16_SINT = eR16G16_SINT,
		eR16A16_SFLOAT = eR16G16_SFLOAT,

		eR32A32_UINT = eR32G32_UINT,
		eR32A32_SINT = eR32G32_SINT,
		eR32A32_SFLOAT = eR32G32_SFLOAT,

		eR64A64_UINT = eR64G64_UINT,
		eR64A64_SINT = eR64G64_SINT,
		eR64A64_SFLOAT = eR64G64_SFLOAT,

		eD32_UNORM = eX8_D24_UNORM,
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
		//!\~english	Luminance.
		//!\~french		Luminance.
		eLuminance,
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
