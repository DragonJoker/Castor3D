/*
See LICENSE file in root folder
*/
#ifndef ___CU_PixelFormat___
#define ___CU_PixelFormat___

#include "CastorUtils/Graphics/PixelComponents.hpp"
#include "CastorUtils/Graphics/PixelDefinitions.hpp"
#include "CastorUtils/Graphics/Size.hpp"

#include "CastorUtils/Exception/Exception.hpp"

#include <algorithm>
#include <numeric>
#include <vector>

#include "CastorUtils/Config/BeginExternHeaderGuard.hpp"
#include <atomic>
#include "CastorUtils/Config/EndExternHeaderGuard.hpp"

namespace castor
{
	/**
	 *\~english
	 *\brief		Function to retrieve the number of components of a pixel format.
	 *\param[in]	format	The pixel format
	 *\~french
	 *\brief		Fonction de récuperation du nombre de composantes d'un format de pixel.
	 *\param[in]	format	Le format de pixels
	 */
	constexpr uint8_t getComponentsCount( PixelFormat format );
	/**
	 *\~english
	 *\return		\p false if format is depth, stencil or one without alpha.
	 *\~french
	 *\return		\p false si le format est depth, stencil ou un format sans alpha.
	 */
	constexpr bool hasAlpha( PixelFormat format );
	/**
	 *\~english
	 *\param[in]	format		The pixel format.
	 *\param[in]	component	The pixel component.
	 *\return		\p true if the given pixel format contains the wanted pixel component.
	 *\~french
	 *\param[in]	format		Le format de pixels.
	 *\param[in]	component	La composante de pixels.
	 *\return		\p true si le format de pixel donné contient la composante de pixel voulue.
	 */
	constexpr bool hasComponent( PixelFormat format
		, PixelComponent component );
	/**
	 *\~english
	 *\return		The single component format for given pixel format.
	 *\~french
	 *\return		Le format à composante unique correspondant au format donné.
	 */
	constexpr PixelFormat getSingleComponent( PixelFormat format );
	/**
	 *\~english
	 *\return		\p true if the given pixel format is a int8 one.
	 *\~french
	 *\return		\p true si le format donné est un format en int8.
	 */
	constexpr bool isInt8( PixelFormat format );
	constexpr bool isInt8( VkFormat format )
	{
		return isInt8( PixelFormat( format ) );
	}
	/**
	 *\~english
	 *\return		\p true if the given pixel format is a int16 one.
	 *\~french
	 *\return		\p true si le format donné est un format en int16.
	 */
	constexpr bool isInt16( PixelFormat format );
	constexpr bool isInt16( VkFormat format )
	{
		return isInt16( PixelFormat( format ) );
	}
	/**
	 *\~english
	 *\return		\p true if the given pixel format is a int32 one.
	 *\~french
	 *\return		\p true si le format donné est un format en int32.
	 */
	constexpr bool isInt32( PixelFormat format );
	constexpr bool isInt32( VkFormat format )
	{
		return isInt32( PixelFormat( format ) );
	}
	/**
	 *\~english
	 *\return		\p true if the given pixel format is a floating point one.
	 *\~french
	 *\return		\p true si le format donné est un format en nombres flottants.
	 */
	constexpr bool isFloatingPoint( PixelFormat format );
	constexpr bool isFloatingPoint( VkFormat format )
	{
		return isFloatingPoint( PixelFormat( format ) );
	}
	/**
	 *\~english
	 *\return		\p true if the given pixel format is a compressed one.
	 *\~french
	 *\return		\p true si le format donné est un format compressé.
	 */
	constexpr bool isCompressed( PixelFormat format );
	/**
	 *\~english
	 *\brief		Tells if the given format is an SRGB one.
	 *\param[in]	format	The pixel format
	 *\~french
	 *\brief		Dit si le format donné est un format SRGB.
	 *\param[in]	format	Le format de pixels
	 */
	constexpr bool isSRGBFormat( PixelFormat format );
	/**
	 *\~english
	 *\return		\p true if format is an RG format.
	 *\~french
	 *\return		\p false si le format est en RG.
	 */
	constexpr bool isRGFormat( PixelFormat format );
	/**
	 *\~english
	 *\return		\p true if format is an RGB format.
	 *\~french
	 *\return		\p false si le format est en RGB.
	 */
	constexpr bool isRGBFormat( PixelFormat format );
	/**
	 *\~english
	 *\return		\p true if format is an BGR format.
	 *\~french
	 *\return		\p false si le format est en BGR.
	 */
	constexpr bool isBGRFormat( PixelFormat format );
	/**
	 *\~english
	 *\return		\p true if format is an RGBA format.
	 *\~french
	 *\return		\p false si le format est en RGBA.
	 */
	constexpr bool isRGBAFormat( PixelFormat format );
	/**
	 *\~english
	 *\return		\p true if format is an ARGB format.
	 *\~french
	 *\return		\p false si le format est en ARGB.
	 */
	constexpr bool isARGBFormat( PixelFormat format );
	/**
	 *\~english
	 *\return		\p true if format is an BGRA format.
	 *\~french
	 *\return		\p false si le format est en BGRA.
	 */
	constexpr bool isBGRAFormat( PixelFormat format );
	/**
	 *\~english
	 *\return		\p true if format is an ABGR format.
	 *\~french
	 *\return		\p false si le format est en ABGR.
	 */
	constexpr bool isABGRFormat( PixelFormat format );
	/**
	 *\~english
	 *\return		The SRGB format matching \p format, \p format if not found.
	 *\~french
	 *\return		Le format SRGB correspondant à \p format, \p format si non trouvé.
	 */
	constexpr PixelFormat getSRGBFormat( PixelFormat format );
	/**
	 *\~english
	 *\return		The non SRGB format matching \p format, \p format if not found.
	 *\~french
	 *\return		Le format non SRGB correspondant à \p format, \p format si non trouvé.
	 */
	constexpr PixelFormat getNonSRGBFormat( PixelFormat format );
	/**
	 *\~english
	 *\return		\p true if format is a depth and/or stencil format.
	 *\~french
	 *\return		\p false si le format est un format depth et/ou stencil.
	 */
	constexpr bool isDepthOrStencilFormat( PixelFormat format );
	/**
	 *\~english
	 *\brief		Retrieves the pixel format without alpha that is close to the one given
	 *\param[in]	format	The pixel format
	 *\return		The given pixel format if none found
	 *\~french
	 *\brief		Récupère le format de pixel sans alpha le plus proche de celui donné
	 *\param[in]	format	Le format de pixel
	 *\return		Le format de pixels donné si non trouvé
	 */
	constexpr PixelFormat getPFWithoutAlpha( PixelFormat format );
	/**
	 *\~english
	 *\brief		Retrieves the pixel format with alpha that is close to the one given
	 *\param[in]	format	The pixel format
	 *\return		The given pixel format if none found
	 *\~french
	 *\brief		Récupère le format de pixel avec alpha le plus proche de celui donné
	 *\param[in]	format	Le format de pixel
	 *\return		Le format de pixels donné si non trouvé
	 */
	constexpr PixelFormat getPFWithAlpha( PixelFormat format );
	/**
	 *\~english
	 *\return		The index matching the given PixelComponent.
	 *\~french
	 *\return		L'index correspondant au PixelComponent donné.
	 */
	constexpr uint32_t getComponentIndex( PixelComponent component );
	/**
	 *\~english
	 *\return		The index matching the given PixelComponent and PixelFormat.
	 *\~french
	 *\return		L'index correspondant au PixelComponent et au PixelFormat donnés.
	 */
	constexpr uint32_t getComponentIndex( PixelComponent component
		, PixelFormat format );
	/**
	 *\~english
	 *\return		The PixelComponent matching the given index and PixelFormat.
	 *\~french
	 *\return		Le PixelComponent correspondant à l'index et au PixelFormat donnés.
	 */
	constexpr PixelComponent getIndexComponent( uint32_t index
		, PixelFormat format );
	/**
	 *\~english
	 *\return		The PixelComponents matching the given PixelFormat.
	 *\~french
	 *\return		Le PixelComponents correspondant au PixelFormat donné.
	 */
	constexpr castor::PixelComponents getComponents( castor::PixelFormat format );

	/**
	 *\~english
	 *\brief		Function to retrieve pixel format from a name
	 *\param[in]	formatName	The pixel format name
	 *\~french
	 *\brief		Fonction de récuperation d'un format de pixel par son nom
	 *\param[in]	formatName	Le nom du format de pixels
	 */
	CU_API PixelFormat getFormatByName( StringView formatName );
	/**
	 *\~english
	 *\return		The pixel format matching the given format and components.
	 *\~french
	 *\return		Le format correspondant au format de pixel et aux composantes donnés.
	 */
	CU_API PixelFormat getPixelFormat( PixelFormat format, PixelComponents components );
	/**
	 *\~english
	 *\brief		Function to perform convertion without templates
	 *\param[in]		srcFormat	The source format
	 *\param[in,out]	src			The source pixel
	 *\param[in]		dstFormat	The destination format
	 *\param[in,out]	dst			The destination pixel
	 *\~french
	 *\brief		Fonction de conversion sans templates
	 *\param[in]		srcFormat	Le format de la source
	 *\param[in,out]	src			Le pixel source
	 *\param[in]		dstFormat	Le format de la destination
	 *\param[in,out]	dst			Le pixel destination
	 */
	CU_API void convertPixel( PixelFormat srcFormat
		, uint8_t const *& src
		, PixelFormat dstFormat
		, uint8_t *& dst );
	/**
	 *\~english
	 *\brief		Function to perform convertion without templates.
	 *\param[in]	srcDimensions	The source dimensions.
	 *\param[in]	dstDimensions	The destination dimensions (used only when block compressing).
	 *\param[in]	srcFormat		The source format.
	 *\param[in]	srcBuffer		The source buffer.
	 *\param[in]	srcSize			The source size.
	 *\param[in]	dstFormat		The destination format.
	 *\param[in]	dstBuffer		The destination buffer.
	 *\param[in]	dstSize			The destination size.
	 *\~french
	 *\brief		Fonction de conversion sans templates.
	 *\param[in]	srcDimensions	Les dimensions de la source.
	 *\param[in]	dstDimensions	Les dimensions de la destination (utilisé uniquement lors d'une compression par blocs).
	 *\param[in]	srcFormat		Le format de la source.
	 *\param[in]	srcBuffer		Le buffer source.
	 *\param[in]	srcSize			La taille de la source.
	 *\param[in]	dstFormat		Le format de la destination.
	 *\param[in]	dstBuffer		Le buffer destination.
	 *\param[in]	dstSize			La taille de la destination.
	 */
	CU_API void convertBuffer( Size const & srcDimensions
		, Size const & dstDimensions
		, PixelFormat srcFormat
		, uint8_t const * srcBuffer
		, uint32_t srcSize
		, PixelFormat dstFormat
		, uint8_t * dstBuffer
		, uint32_t dstSize );
	/**
	 *\~english
	 *\brief		Function to perform convertion without templates.
	 *\param[in]	dimensions	The source dimensions.
	 *\param[in]	srcFormat	The source format.
	 *\param[in]	srcBuffer	The source buffer.
	 *\param[in]	srcSize		The source size.
	 *\param[in]	dstFormat	The destination format.
	 *\param[in]	dstBuffer	The destination buffer.
	 *\param[in]	dstSize		The destination size.
	 *\~french
	 *\brief		Fonction de conversion sans templates.
	 *\param[in]	dimensions	Les dimensions de la source.
	 *\param[in]	srcFormat	Le format de la source.
	 *\param[in]	srcBuffer	Le buffer source.
	 *\param[in]	srcSize		La taille de la source.
	 *\param[in]	dstFormat	Le format de la destination.
	 *\param[in]	dstBuffer	Le buffer destination.
	 *\param[in]	dstSize		La taille de la destination.
	 */
	static inline void convertBuffer( Size const & dimensions
		, PixelFormat srcFormat
		, uint8_t const * srcBuffer
		, uint32_t srcSize
		, PixelFormat dstFormat
		, uint8_t * dstBuffer
		, uint32_t dstSize )
	{
		convertBuffer( dimensions
			, dimensions
			, srcFormat
			, srcBuffer
			, srcSize
			, dstFormat
			, dstBuffer
			, dstSize );
	}
	/**
	 *\~english
	 *\brief		Function to perform convertion without templates.
	 *\param[in]	options			The convertion options.
	 *\param[in]	interrupt		Tells if the convertion is to be interrupted.
	 *\param[in]	srcDimensions	The source dimensions.
	 *\param[in]	dstDimensions	The destination dimensions (used only when block compressing).
	 *\param[in]	srcFormat		The source format.
	 *\param[in]	srcBuffer		The source buffer.
	 *\param[in]	srcSize			The source size.
	 *\param[in]	dstFormat		The destination format.
	 *\param[in]	dstBuffer		The destination buffer.
	 *\param[in]	dstSize			The destination size.
	 *\~french
	 *\brief		Fonction de conversion sans templates
	 *\param[in]	options			Les options de conversion.
	 *\param[in]	interrupt		Dit si la conversion est à interrompre.
	 *\param[in]	srcDimensions	Les dimensions de la source.
	 *\param[in]	dstDimensions	Les dimensions de la destination (utilisé uniquement lors d'une compression par blocs).
	 *\param[in]	srcFormat		Le format de la source.
	 *\param[in]	srcBuffer		Le buffer source.
	 *\param[in]	srcSize			La taille de la source.
	 *\param[in]	dstFormat		Le format de la destination.
	 *\param[in]	dstBuffer		Le buffer destination.
	 *\param[in]	dstSize			La taille de la destination.
	 */
	CU_API void compressBuffer( PxBufferConvertOptions const * options
		, std::atomic_bool const * interrupt
		, Size const & srcDimensions
		, Size const & dstDimensions
		, PixelFormat srcFormat
		, uint8_t const * srcBuffer
		, uint32_t srcSize
		, PixelFormat dstFormat
		, uint8_t * dstBuffer
		, uint32_t dstSize );
	/**
	 *\~english
	 *\brief		Decompresses the given compressed pixel buffer.
	 *\param[in]	src	The compressed pixel buffer.
	 *\return		The decompressed pixel buffer.
	 *\~french
	 *\brief		Décompresse le tampopn de pixels donné.
	 *\param[in]	src	Le tampon de pixels compressé.
	 *\return		Le tampon de pixels décompressé.
	 */
	CU_API PxBufferBaseUPtr decompressBuffer( PxBufferBase const & src );
	/**
	 *\~english
	 *\brief		Extracts pixel component values from a source buffer holding alpha and puts it in a destination buffer.
	 *\param[in]	src			The source buffer.
	 *\param[in]	component	The component to extract.
	 *\return		A castor::PixelFormat::eR8 buffer containing the extracted component, \p nullptr if source didn't have the wanted component.
	 *\~french
	 *\brief		Extrait les valeurs d'une composante de pixel d'un tampon source pour les mettre dans un tampon à part.
	 *\param[in]	src			Le tampon source
	 *\param[in]	component	La composante à extraire
	 *\return		Le tampon contenant la composante extraite, au format castor::PixelFormat::eR8, \p nullptr si la source n'avait pas la coposante voulue.
	 */
	CU_API PxBufferBaseUPtr extractComponent( PxBufferBaseRPtr src
		, PixelComponent component );
	/**
	 *\~english
	 *\brief		Extracts pixel component values from a source buffer holding alpha and puts it in a destination buffer.
	 *\param[in]	src			The source buffer.
	 *\param[in]	component	The component to extract.
	 *\return		A castor::PixelFormat::eR8 buffer containing the extracted component, \p nullptr if source didn't have the wanted component.
	 *\~french
	 *\brief		Extrait les valeurs d'une composante de pixel d'un tampon source pour les mettre dans un tampon à part.
	 *\param[in]	src			Le tampon source
	 *\param[in]	component	La composante à extraire
	 *\return		Le tampon contenant la composante extraite, au format castor::PixelFormat::eR8, \p nullptr si la source n'avait pas la coposante voulue.
	 */
	CU_API PxBufferBaseUPtr extractComponents( PxBufferBaseRPtr src
		, PixelComponents component );
	/**
	 *\~english
	 *\brief			Copies some pixel components from a source buffer into pixel components of a destination buffer.
	 *\param[in]		srcComponents	The components to read from the source buffer.
	 *\param[in]		dstComponents	The components to write into the destination buffer.
	 *\param[in]		srcBuffer		The source buffer.
	 *\param[in,out]	dstBuffer		The destination buffer.
	 *\~french
	 *\brief			Copie des composantes de pixel d'un buffer source vers des composantes de pixel d'un buffer destination.
	 *\param[in]		srcComponents	Les composantes à lire depuis le buffer source.
	 *\param[in]		dstComponents	Les composantes à écrire dans le buffer destination.
	 *\param[in]		srcBuffer		Le buffer source.
	 *\param[in,out]	dstBuffer		Le buffer destination.
	 */
	CU_API void copyBufferComponents( PixelComponents srcComponents
		, PixelComponents dstComponents
		, PxBufferBase const & srcBuffer
		, PxBufferBase & dstBuffer );
}

#include "PixelFormat.inl"

#endif
