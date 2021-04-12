/*
See LICENSE file in root folder
*/
#ifndef ___CU_PixelFormat___
#define ___CU_PixelFormat___

#include "CastorUtils/Graphics/GraphicsModule.hpp"
#include "CastorUtils/Graphics/PixelComponents.hpp"
#include "CastorUtils/Graphics/PixelDefinitions.hpp"
#include "CastorUtils/Graphics/Size.hpp"

#include "CastorUtils/Exception/Exception.hpp"

#include <vector>
#include <algorithm>
#include <numeric>

namespace castor
{
	template<> struct IsColourFormat< PixelFormat::eD16_UNORM > : public std::false_type {};
	template<> struct IsColourFormat< PixelFormat::eD24_UNORM_S8_UINT > : public std::false_type {};
	template<> struct IsColourFormat< PixelFormat::eD32_SFLOAT > : public std::false_type {};
	template<> struct IsColourFormat< PixelFormat::eD32_SFLOAT_S8_UINT > : public std::false_type {};
	template<> struct IsColourFormat< PixelFormat::eS8_UINT > : public std::false_type {};

	template<> struct IsDepthFormat< PixelFormat::eD16_UNORM > : public std::true_type {};
	template<> struct IsDepthFormat< PixelFormat::eD24_UNORM_S8_UINT > : public std::true_type {};
	template<> struct IsDepthFormat< PixelFormat::eD32_SFLOAT > : public std::true_type {};
	template<> struct IsDepthFormat< PixelFormat::eD32_SFLOAT_S8_UINT > : public std::true_type {};

	template<> struct IsStencilFormat< PixelFormat::eD24_UNORM_S8_UINT > : public std::true_type {};
	template<> struct IsStencilFormat< PixelFormat::eD32_SFLOAT_S8_UINT > : public std::true_type {};
	template<> struct IsStencilFormat< PixelFormat::eS8_UINT > : public std::true_type {};

	template<> struct IsDepthStencilFormat< PixelFormat::eD16_UNORM > : public std::true_type {};
	template<> struct IsDepthStencilFormat< PixelFormat::eD24_UNORM_S8_UINT > : public std::true_type {};
	template<> struct IsDepthStencilFormat< PixelFormat::eD32_SFLOAT > : public std::true_type {};
	template<> struct IsDepthStencilFormat< PixelFormat::eD32_SFLOAT_S8_UINT > : public std::true_type {};
	template<> struct IsDepthStencilFormat< PixelFormat::eS8_UINT > : public std::true_type {};

	namespace PF
	{
		CU_API X8UGetter getR8U( PixelFormat format );
		CU_API X8UGetter getG8U( PixelFormat format );
		CU_API X8UGetter getB8U( PixelFormat format );
		CU_API X8UGetter getA8U( PixelFormat format );
		CU_API X8SGetter getR8S( PixelFormat format );
		CU_API X8SGetter getG8S( PixelFormat format );
		CU_API X8SGetter getB8S( PixelFormat format );
		CU_API X8SGetter getA8S( PixelFormat format );
		CU_API X32FGetter getR32F( PixelFormat format );
		CU_API X32FGetter getG32F( PixelFormat format );
		CU_API X32FGetter getB32F( PixelFormat format );
		CU_API X32FGetter getA32F( PixelFormat format );
		/**
		 *\~english
		 *\brief		Function to retrieve Pixel size without templates
		 *\param[in]	format	The pixel format
		 *\~french
		 *\brief		Fonction de récuperation de la taille d'un pixel sans templates
		 *\param[in]	format	Le format de pixels
		 */
		CU_API uint8_t getBytesPerPixel( PixelFormat format );
		/**
		 *\~english
		 *\brief		Function to retrieve the number of components of a pixel format.
		 *\param[in]	format	The pixel format
		 *\~french
		 *\brief		Fonction de récuperation du nombre de composantes d'un format de pixel.
		 *\param[in]	format	Le format de pixels
		 */
		CU_API uint8_t getComponentsCount( PixelFormat format );
		/**
		 *\~english
		 *\brief		Function to retrieve pixel format from a name
		 *\param[in]	formatName	The pixel format name
		 *\~french
		 *\brief		Fonction de récuperation d'un format de pixel par son nom
		 *\param[in]	formatName	Le nom du format de pixels
		 */
		CU_API PixelFormat getFormatByName( String const & formatName );
		/**
		 *\~english
		 *\brief		Function to retrieve pixel format name
		 *\param[in]	format	The pixel format
		 *\~french
		 *\brief		Fonction de récuperation du nom d'un format de pixel
		 *\param[in]	format	Le format de pixels
		 */
		CU_API String getFormatName( PixelFormat format );
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
		 *\brief		Function to perform convertion without templates
		 *\param[in]	srcDimensions	The source dimensions
		 *\param[in]	dstDimensions	The destination dimensions (used only when block compressing)
		 *\param[in]	srcFormat		The source format
		 *\param[in]	srcBuffer		The source buffer
		 *\param[in]	srcSize			The source size
		 *\param[in]	dstFormat		The destination format
		 *\param[in]	dstBuffer		The destination buffer
		 *\param[in]	dstSize			The destination size
		 *\~french
		 *\brief		Fonction de conversion sans templates
		 *\param[in]	srcDimensions	Les dimensions de la source
		 *\param[in]	dstDimensions	Les dimensions de la destination (utilisé uniquement lors d'une compression par blocs)
		 *\param[in]	srcFormat		Le format de la source
		 *\param[in]	srcBuffer		Le buffer source
		 *\param[in]	srcSize			La taille de la source
		 *\param[in]	dstFormat		Le format de la destination
		 *\param[in]	dstBuffer		Le buffer destination
		 *\param[in]	dstSize			La taille de la destination
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
		 *\brief		Function to perform convertion without templates
		 *\param[in]	srcFormat	The source format
		 *\param[in]	srcBuffer	The source buffer
		 *\param[in]	srcSize		The source size
		 *\param[in]	dstFormat	The destination format
		 *\param[in]	dstBuffer	The destination buffer
		 *\param[in]	dstSize		The destination size
		 *\~french
		 *\brief		Fonction de conversion sans templates
		 *\param[in]	srcFormat	Le format de la source
		 *\param[in]	srcBuffer	Le buffer source
		 *\param[in]	srcSize		La taille de la source
		 *\param[in]	dstFormat	Le format de la destination
		 *\param[in]	dstBuffer	Le buffer destination
		 *\param[in]	dstSize		La taille de la destination
		 */
		static void convertBuffer( Size const & dimensions
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
		 *\param[in]	format		The pixel format.
		 *\param[in]	component	The pixel component.
		 *\return		\p true if the given pixel format contains the wanted pixel component.
		 *\~french
		 *\param[in]	format		Le format de pixels.
		 *\param[in]	component	La composante de pixels.
		 *\return		\p true si le format de pixel donné contient la composante de pixel voulue.
		 */
		CU_API bool hasComponent( PixelFormat format
			, PixelComponent component );
		/**
		 *\see			PixelFormat
		 *\~english
		 *\return		\p false if format is depth, stencil or one without alpha.
		 *\~french
		 *\return		\p false si le format est depth, stencil ou un format sans alpha.
		 */
		CU_API bool hasAlpha( PixelFormat format );
		/**
		 *\see			PixelFormat
		 *\~english
		 *\return		\p true if the given pixel format is a floating point one.
		 *\~french
		 *\return		\p true si le format donné est un format en nombres flottants.
		 */
		CU_API bool isFloatingPoint( PixelFormat format );
		/**
		 *\see			PixelFormat
		 *\~english
		 *\return		\p true if the given pixel format is a compressed one.
		 *\~french
		 *\return		\p true si le format donné est un format compressé.
		 */
		CU_API bool isCompressed( PixelFormat format );
		/**
		 *\~english
		 *\brief		Function to retrieve the compressed pixel format for the given one.
		 *\param[in]	format	The pixel format.
		 *\~french
		 *\brief		Fonction de récuperation du format de pixel compressé correspondant à celui donné
		 *\param[in]	format	Le format de pixels.
		 */
		CU_API PixelFormat getCompressed( PixelFormat format );
		/**
		 *\~english
		 *\brief		Function to perform convertion without templates
		 *\param[in]	srcDimensions	The source dimensions
		 *\param[in]	dstDimensions	The destination dimensions (used only when block compressing)
		 *\param[in]	srcFormat		The source format
		 *\param[in]	srcBuffer		The source buffer
		 *\param[in]	srcSize			The source size
		 *\param[in]	dstFormat		The destination format
		 *\param[in]	dstBuffer		The destination buffer
		 *\param[in]	dstSize			The destination size
		 *\~french
		 *\brief		Fonction de conversion sans templates
		 *\param[in]	srcDimensions	Les dimensions de la source
		 *\param[in]	dstDimensions	Les dimensions de la destination (utilisé uniquement lors d'une compression par blocs)
		 *\param[in]	srcFormat		Le format de la source
		 *\param[in]	srcBuffer		Le buffer source
		 *\param[in]	srcSize			La taille de la source
		 *\param[in]	dstFormat		Le format de la destination
		 *\param[in]	dstBuffer		Le buffer destination
		 *\param[in]	dstSize			La taille de la destination
		 */
		CU_API void compressBuffer( PxBufferConvertOptions const * options
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
		CU_API PxBufferBaseSPtr decompressBuffer( PxBufferBaseSPtr src );
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
		CU_API PxBufferBaseSPtr extractComponent( PxBufferBaseSPtr src
			, PixelComponent component );
		/**
		 *\~english
		 *\brief			Extracts alpha values from a source buffer holding alpha and puts it in a destination buffer.
		 *\remarks			Destination buffer will be a eR8 pixel format buffer and alpha channel from source buffer will be removed.
		 *\param[in,out]	src	The source buffer.
		 *\return			The destination buffer, \p nullptr if source didn't have alpha.
		 *\~french
		 *\brief			Extrait les valeurs alpha d'un tampon source pour les mettre dans un tampon à part.
		 *\remarks			Le tampon contenant les valeurs alpha sera au format eR8 et le canal alpha du tampon source sera supprimé.
		 *\param[in,out]	src	Le tampon source.
		 *\return			Le tampon alpha, \p nullptr si la source n'avait pas d'alpha.
		 */
		CU_API PxBufferBaseSPtr extractAlpha( PxBufferBaseSPtr & src );
		/**
		 *\~english
		 *\brief		Retrieves the pixel format without alpha that is near from the one given
		 *\param[in]	format	The pixel format
		 *\return		The given pixel format if none found
		 *\~french
		 *\brief		Récupère le format de pixel sans alpha le plus proche de celui donné
		 *\param[in]	format	Le format de pixel
		 *\return		Le format de pixels donné si non trouvé
		 */
		CU_API PixelFormat getPFWithoutAlpha( PixelFormat format );
		/**
		 *\~english
		 *\brief			Reduces a buffer to it's alpha channel, stored in a L8 format buffer
		 *\param[in,out]	src	The buffer to reduce
		 *\~french
		 *\brief			Réduit un tampon à son canal alpha stocké dans un tampon au format L8
		 *\param[in,out]	src	Le tampon à réduire
		 */
		CU_API void reduceToAlpha( PxBufferBaseSPtr & src );
	}
}

#endif
