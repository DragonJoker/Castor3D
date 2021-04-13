#include "CastorUtils/Graphics/PixelComponents.hpp"
#include "CastorUtils/Graphics/UnsupportedFormatException.hpp"
#include "CastorUtils/Miscellaneous/StringUtils.hpp"
#include "CastorUtils/Exception/Assertion.hpp"

#include <cstring>

#if CU_UseSSE2
#	include <emmintrin.h>
#endif

#if defined( max )
#	undef max
#endif

#if defined( min )
#	undef min
#endif

namespace castor
{
	namespace details
	{
		/**
		\~english
		\brief		Structure used to convert one pixel from one format to another one
		\~french
		\brief		Structure utilisée pour convertir un pixel d'un format vers un autre
		*/
		template< PixelFormat PFSrc, PixelFormat PFDst >
		struct PixelConverter
		{
			void operator()( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
			{
				if constexpr ( PFSrc == PFDst )
				{
					std::memcpy( dstBuffer, srcBuffer, PixelDefinitions< PFSrc >::Size );
				}
				else if constexpr ( is8UComponentsV< PFDst > )
				{
					PixelComponents< PFDst >::R( dstBuffer, getR8U< PFSrc >( srcBuffer ) );
					PixelComponents< PFDst >::G( dstBuffer, getG8U< PFSrc >( srcBuffer ) );
					PixelComponents< PFDst >::B( dstBuffer, getB8U< PFSrc >( srcBuffer ) );
					PixelComponents< PFDst >::A( dstBuffer, getA8U< PFSrc >( srcBuffer ) );
				}
				else if constexpr ( is8SComponentsV< PFDst > )
				{
					PixelComponents< PFDst >::R( dstBuffer, getR8S< PFSrc >( srcBuffer ) );
					PixelComponents< PFDst >::G( dstBuffer, getG8S< PFSrc >( srcBuffer ) );
					PixelComponents< PFDst >::B( dstBuffer, getB8S< PFSrc >( srcBuffer ) );
					PixelComponents< PFDst >::A( dstBuffer, getA8S< PFSrc >( srcBuffer ) );
				}
				else if constexpr ( is16UComponentsV< PFDst > )
				{
					PixelComponents< PFDst >::R( dstBuffer, getR16U< PFSrc >( srcBuffer ) );
					PixelComponents< PFDst >::G( dstBuffer, getG16U< PFSrc >( srcBuffer ) );
					PixelComponents< PFDst >::B( dstBuffer, getB16U< PFSrc >( srcBuffer ) );
					PixelComponents< PFDst >::A( dstBuffer, getA16U< PFSrc >( srcBuffer ) );
				}
				else if constexpr ( is16SComponentsV< PFDst > )
				{
					PixelComponents< PFDst >::R( dstBuffer, getR16S< PFSrc >( srcBuffer ) );
					PixelComponents< PFDst >::G( dstBuffer, getG16S< PFSrc >( srcBuffer ) );
					PixelComponents< PFDst >::B( dstBuffer, getB16S< PFSrc >( srcBuffer ) );
					PixelComponents< PFDst >::A( dstBuffer, getA16S< PFSrc >( srcBuffer ) );
				}
				else if constexpr ( is16FComponentsV< PFDst > )
				{
					PixelComponents< PFDst >::R( dstBuffer, getR16F< PFSrc >( srcBuffer ) );
					PixelComponents< PFDst >::G( dstBuffer, getG16F< PFSrc >( srcBuffer ) );
					PixelComponents< PFDst >::B( dstBuffer, getB16F< PFSrc >( srcBuffer ) );
					PixelComponents< PFDst >::A( dstBuffer, getA16F< PFSrc >( srcBuffer ) );
				}
				else if constexpr ( is32UComponentsV< PFDst > )
				{
					PixelComponents< PFDst >::R( dstBuffer, getR32U< PFSrc >( srcBuffer ) );
					PixelComponents< PFDst >::G( dstBuffer, getG32U< PFSrc >( srcBuffer ) );
					PixelComponents< PFDst >::B( dstBuffer, getB32U< PFSrc >( srcBuffer ) );
					PixelComponents< PFDst >::A( dstBuffer, getA32U< PFSrc >( srcBuffer ) );
				}
				else if constexpr ( is32SComponentsV< PFDst > )
				{
					PixelComponents< PFDst >::R( dstBuffer, getR32S< PFSrc >( srcBuffer ) );
					PixelComponents< PFDst >::G( dstBuffer, getG32S< PFSrc >( srcBuffer ) );
					PixelComponents< PFDst >::B( dstBuffer, getB32S< PFSrc >( srcBuffer ) );
					PixelComponents< PFDst >::A( dstBuffer, getA32S< PFSrc >( srcBuffer ) );
				}
				else if constexpr ( is32FComponentsV< PFDst > )
				{
					PixelComponents< PFDst >::R( dstBuffer, getR32F< PFSrc >( srcBuffer ) );
					PixelComponents< PFDst >::G( dstBuffer, getG32F< PFSrc >( srcBuffer ) );
					PixelComponents< PFDst >::B( dstBuffer, getB32F< PFSrc >( srcBuffer ) );
					PixelComponents< PFDst >::A( dstBuffer, getA32F< PFSrc >( srcBuffer ) );
				}
				else if constexpr ( is64UComponentsV< PFDst > )
				{
					PixelComponents< PFDst >::R( dstBuffer, getR64U< PFSrc >( srcBuffer ) );
					PixelComponents< PFDst >::G( dstBuffer, getG64U< PFSrc >( srcBuffer ) );
					PixelComponents< PFDst >::B( dstBuffer, getB64U< PFSrc >( srcBuffer ) );
					PixelComponents< PFDst >::A( dstBuffer, getA64U< PFSrc >( srcBuffer ) );
				}
				else if constexpr ( is64SComponentsV< PFDst > )
				{
					PixelComponents< PFDst >::R( dstBuffer, getR64S< PFSrc >( srcBuffer ) );
					PixelComponents< PFDst >::G( dstBuffer, getG64S< PFSrc >( srcBuffer ) );
					PixelComponents< PFDst >::B( dstBuffer, getB64S< PFSrc >( srcBuffer ) );
					PixelComponents< PFDst >::A( dstBuffer, getA64S< PFSrc >( srcBuffer ) );
				}
				else if constexpr ( is64FComponentsV< PFDst > )
				{
					PixelComponents< PFDst >::R( dstBuffer, getR64F< PFSrc >( srcBuffer ) );
					PixelComponents< PFDst >::G( dstBuffer, getG64F< PFSrc >( srcBuffer ) );
					PixelComponents< PFDst >::B( dstBuffer, getB64F< PFSrc >( srcBuffer ) );
					PixelComponents< PFDst >::A( dstBuffer, getA64F< PFSrc >( srcBuffer ) );
				}
				else if constexpr ( isD16UComponentV< PFDst > )
				{
					PixelComponents< PFDst >::D( dstBuffer, getD16U< PFSrc >( srcBuffer ) );
					PixelComponents< PFDst >::S( dstBuffer, getS8U< PFSrc >( srcBuffer ) );
				}
				else if constexpr ( isD24UComponentV< PFDst > )
				{
					PixelComponents< PFDst >::D( dstBuffer, getD24U< PFSrc >( srcBuffer ) );
					PixelComponents< PFDst >::S( dstBuffer, getS8U< PFSrc >( srcBuffer ) );
				}
				else if constexpr ( isD32FComponentV< PFDst > )
				{
					PixelComponents< PFDst >::D( dstBuffer, getD32F< PFSrc >( srcBuffer ) );
					PixelComponents< PFDst >::S( dstBuffer, getS8U< PFSrc >( srcBuffer ) );
				}
				else if constexpr ( isS8UComponentV< PFDst > )
				{
					PixelComponents< PFDst >::S( dstBuffer, getS8U< PFSrc >( srcBuffer ) );
				}

				srcBuffer += PixelDefinitions< PFSrc >::Size;
				dstBuffer += PixelDefinitions< PFDst >::Size;
			}
		};
		/**
		\~english
		\brief		Structure used to convert a buffer from one pixel format to another one
		\~french
		\brief		Structure utilisée pour convertir un buffer d'un format de pixels vers un autre
		*/
		template< PixelFormat PFSrc, PixelFormat PFDst, typename EnableT = void >
		struct BufferConverter
		{
			void operator()( PxBufferConvertOptions const * options
				, Size const & srcDimensions
				, Size const & dstDimensions
				, uint8_t const * srcBuffer
				, uint32_t srcSize
				, uint8_t * dstBuffer
				, uint32_t dstSize )
			{
				uint8_t const * src = &srcBuffer[0];
				uint8_t * dst = &dstBuffer[0];
				uint32_t count = srcSize / PixelDefinitions< PFSrc >::Size;
				CU_Require( srcSize / PixelDefinitions< PFSrc >::Size == dstSize / PixelDefinitions< PFDst >::Size );
				PixelConverter< PFSrc, PFDst > converter;

				for ( uint32_t i = 0; i < count; i++ )
				{
					converter( src, dst );
				}
			}
		};

		/**
		 *\~english
		 *\brief			Conversion from static source format to dynamic destination format
		 *\remarks			Only for colour pixel formats, offsets source and destination by respective pixel format sizes
		 *\param[in,out]	srcBuffer	The source pixel
		 *\param[in,out]	dstBuffer	The destination pixel
		 *\param[in]		dstFormat	The destination pixel format
		 *\~french
		 *\brief			Conversion d'une source statique vers une destination dynamique
		 *\remarks			Uniquement pour les formats de pixel couleurs, décale la source et la destination par la taille respective de leur format de pixel
		 *\param[in,out]	srcBuffer	Le pixel source
		 *\param[in,out]	dstBuffer	Le pixel destination
		 *\param[in]		dstFormat	Le format du pixel destination
		 */
		template< PixelFormat PF >
		void dynamicColourConversion( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			switch ( dstFormat )
			{
			case PixelFormat::eR8_UNORM:
				PixelConverter< PF, PixelFormat::eR8_UNORM >()( srcBuffer, dstBuffer );
				break;
			case PixelFormat::eR16_SFLOAT:
				PixelConverter< PF, PixelFormat::eR16_SFLOAT >()( srcBuffer, dstBuffer );
				break;
			case PixelFormat::eR32_SFLOAT:
				PixelConverter< PF, PixelFormat::eR32_SFLOAT >()( srcBuffer, dstBuffer );
				break;
			case PixelFormat::eR8A8_UNORM:
				PixelConverter< PF, PixelFormat::eR8A8_UNORM >()( srcBuffer, dstBuffer );
				break;
			case PixelFormat::eR16A16_SFLOAT:
				PixelConverter< PF, PixelFormat::eR16A16_SFLOAT >()( srcBuffer, dstBuffer );
				break;
			case PixelFormat::eR32A32_SFLOAT:
				PixelConverter< PF, PixelFormat::eR32A32_SFLOAT >()( srcBuffer, dstBuffer );
				break;
			case PixelFormat::eR5G5B5A1_UNORM:
				PixelConverter< PF, PixelFormat::eR5G5B5A1_UNORM >()( srcBuffer, dstBuffer );
				break;
			case PixelFormat::eR5G6B5_UNORM:
				PixelConverter< PF, PixelFormat::eR5G6B5_UNORM >()( srcBuffer, dstBuffer );
				break;
			case PixelFormat::eR8G8B8_UNORM:
				PixelConverter< PF, PixelFormat::eR8G8B8_UNORM >()( srcBuffer, dstBuffer );
				break;
			case PixelFormat::eB8G8R8_UNORM:
				PixelConverter< PF, PixelFormat::eB8G8R8_UNORM >()( srcBuffer, dstBuffer );
				break;
			case PixelFormat::eR8G8B8_SRGB:
				PixelConverter< PF, PixelFormat::eR8G8B8_SRGB >()( srcBuffer, dstBuffer );
				break;
			case PixelFormat::eB8G8R8_SRGB:
				PixelConverter< PF, PixelFormat::eB8G8R8_SRGB >()( srcBuffer, dstBuffer );
				break;
			case PixelFormat::eR8G8B8A8_UNORM:
				PixelConverter< PF, PixelFormat::eR8G8B8A8_UNORM >()( srcBuffer, dstBuffer );
				break;
			case PixelFormat::eA8B8G8R8_UNORM:
				PixelConverter< PF, PixelFormat::eA8B8G8R8_UNORM >()( srcBuffer, dstBuffer );
				break;
			case PixelFormat::eR8G8B8A8_SRGB:
				PixelConverter< PF, PixelFormat::eR8G8B8A8_SRGB >()( srcBuffer, dstBuffer );
				break;
			case PixelFormat::eA8B8G8R8_SRGB:
				PixelConverter< PF, PixelFormat::eA8B8G8R8_SRGB >()( srcBuffer, dstBuffer );
				break;
			case PixelFormat::eR16G16B16_SFLOAT:
				PixelConverter< PF, PixelFormat::eR16G16B16_SFLOAT >()( srcBuffer, dstBuffer );
				break;
			case PixelFormat::eR16G16B16A16_SFLOAT:
				PixelConverter< PF, PixelFormat::eR16G16B16A16_SFLOAT >()( srcBuffer, dstBuffer );
				break;
			case PixelFormat::eR32G32B32_SFLOAT:
				PixelConverter< PF, PixelFormat::eR32G32B32_SFLOAT >()( srcBuffer, dstBuffer );
				break;
			case PixelFormat::eR32G32B32A32_SFLOAT:
				PixelConverter< PF, PixelFormat::eR32G32B32A32_SFLOAT >()( srcBuffer, dstBuffer );
				break;
			default:
				if ( PF == dstFormat )
				{
					PixelConverter< PF, PF >()( srcBuffer, dstBuffer );
				}
				else
				{
					CU_UnsupportedError( "No conversion defined" );
				}
				break;
			}
		}
		/**
		 *\~english
		 *\brief			Conversion from static source format to dynamic destination format
		 *\remarks			Only for depth pixel formats, offsets source and destination by respective pixel format sizes
		 *\param[in,out]	srcBuffer	The source pixel
		 *\param[in,out]	dstBuffer	The destination pixel
		 *\param[in]		dstFormat	The destination pixel format
		 *\~french
		 *\brief			Conversion d'une source statique vers une destination dynamique
		 *\remarks			Uniquement pour les formats de pixel profondeurs, décale la source et la destination par la taille respective de leur format de pixel
		 *\param[in,out]	srcBuffer	Le pixel source
		 *\param[in,out]	dstBuffer	Le pixel destination
		 *\param[in]		dstFormat	Le format du pixel destination
		 */
		template< PixelFormat PF >
		void dynamicDepthConversion( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			switch ( dstFormat )
			{
			case PixelFormat::eD16_UNORM:
				PixelConverter< PF, PixelFormat::eD16_UNORM >()( srcBuffer, dstBuffer );
				break;
			case PixelFormat::eD24_UNORM_S8_UINT:
				PixelConverter< PF, PixelFormat::eD24_UNORM_S8_UINT >()( srcBuffer, dstBuffer );
				break;
			case PixelFormat::eD32_UNORM:
				PixelConverter< PF, PixelFormat::eD32_UNORM >()( srcBuffer, dstBuffer );
				break;
			case PixelFormat::eD32_SFLOAT:
				PixelConverter< PF, PixelFormat::eD32_SFLOAT >()( srcBuffer, dstBuffer );
				break;
			case PixelFormat::eD32_SFLOAT_S8_UINT:
				PixelConverter< PF, PixelFormat::eD32_SFLOAT_S8_UINT >()( srcBuffer, dstBuffer );
				break;
			default:
				if ( PF == dstFormat )
				{
					PixelConverter< PF, PF >()( srcBuffer, dstBuffer );
				}
				else
				{
					CU_UnsupportedError( "No conversion defined" );
				}
				break;
			}
		}
		/**
		 *\~english
		 *\brief			Conversion from static source format to dynamic destination format
		 *\remarks			Only for stencil pixel formats, offsets source and destination by respective pixel format sizes
		 *\param[in,out]	srcBuffer	The source pixel
		 *\param[in,out]	dstBuffer	The destination pixel
		 *\param[in]		dstFormat	The destination pixel format
		 *\~french
		 *\brief			Conversion d'une source statique vers une destination dynamique
		 *\remarks			Uniquement pour les formats de pixel stencil, décale la source et la destination par la taille respective de leur format de pixel
		 *\param[in,out]	srcBuffer	Le pixel source
		 *\param[in,out]	dstBuffer	Le pixel destination
		 *\param[in]		dstFormat	Le format du pixel destination
		 */
		template< PixelFormat PF >
		void dynamicStencilConversion( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			switch ( dstFormat )
			{
			case PixelFormat::eD24_UNORM_S8_UINT:
				PixelConverter< PF, PixelFormat::eD24_UNORM_S8_UINT >()( srcBuffer, dstBuffer );
				break;
			case PixelFormat::eS8_UINT:
				PixelConverter< PF, PixelFormat::eS8_UINT >()( srcBuffer, dstBuffer );
				break;
			default:
				if ( PF == dstFormat )
				{
					PixelConverter< PF, PF >()( srcBuffer, dstBuffer );
				}
				else
				{
					CU_UnsupportedError( "No conversion defined" );
				}
				break;
			}
		}
		/**
		 *\~english
		 *\brief		Buffer conversion
		 *\param[in]	srcBuffer	The source buffer
		 *\param[in]	srcSize		The source buffer size
		 *\param[in]	dstFormat	The destination pixel format
		 *\param[in]	dstBuffer	The destination buffer
		 *\param[in]	dstSize		The destination buffer size
		 *\~french
		 *\brief		Conversion de buffer
		 *\param[in]	srcBuffer	Le buffer source
		 *\param[in]	srcSize		La taille du buffer source
		 *\param[in]	dstFormat	Le format du buffer destination
		 *\param[in]	dstBuffer	Le buffer destination
		 *\param[in]	dstSize		La taille du buffer destination
		 */
		template< PixelFormat PF >
		void dynamicColourBufferConversion( PxBufferConvertOptions const * options
			, Size const & srcDimensions
			, Size const & dstDimensions
			, uint8_t const * srcBuffer
			, uint32_t srcSize
			, PixelFormat dstFormat
			, uint8_t * dstBuffer
			, uint32_t dstSize )
		{
			switch ( dstFormat )
			{
			case PixelFormat::eR8_UNORM:
				BufferConverter< PF, PixelFormat::eR8_UNORM >()( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;
			case PixelFormat::eR16_SFLOAT:
				BufferConverter< PF, PixelFormat::eR16_SFLOAT >()( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;
			case PixelFormat::eR32_SFLOAT:
				BufferConverter< PF, PixelFormat::eR32_SFLOAT >()( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;
			case PixelFormat::eR8A8_UNORM:
				BufferConverter< PF, PixelFormat::eR8A8_UNORM >()( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;
			case PixelFormat::eR16A16_SFLOAT:
				BufferConverter< PF, PixelFormat::eR16A16_SFLOAT >()( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;
			case PixelFormat::eR32A32_SFLOAT:
				BufferConverter< PF, PixelFormat::eR32A32_SFLOAT >()( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;
			case PixelFormat::eR5G5B5A1_UNORM:
				BufferConverter< PF, PixelFormat::eR5G5B5A1_UNORM >()( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;
			case PixelFormat::eR5G6B5_UNORM:
				BufferConverter< PF, PixelFormat::eR5G6B5_UNORM >()( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;
			case PixelFormat::eR8G8B8_UNORM:
				BufferConverter< PF, PixelFormat::eR8G8B8_UNORM >()( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;
			case PixelFormat::eB8G8R8_UNORM:
				BufferConverter< PF, PixelFormat::eB8G8R8_UNORM >()( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;
			case PixelFormat::eB8G8R8A8_UNORM:
				BufferConverter< PF, PixelFormat::eB8G8R8A8_UNORM >()( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;
			case PixelFormat::eR8G8B8_SRGB:
				BufferConverter< PF, PixelFormat::eR8G8B8_SRGB >()( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;
			case PixelFormat::eB8G8R8_SRGB:
				BufferConverter< PF, PixelFormat::eB8G8R8_SRGB >()( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;
			case PixelFormat::eR8G8B8A8_UNORM:
				BufferConverter< PF, PixelFormat::eR8G8B8A8_UNORM >()( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;
			case PixelFormat::eA8B8G8R8_UNORM:
				BufferConverter< PF, PixelFormat::eA8B8G8R8_UNORM >()( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;
			case PixelFormat::eR8G8B8A8_SRGB:
				BufferConverter< PF, PixelFormat::eR8G8B8A8_SRGB >()( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;
			case PixelFormat::eA8B8G8R8_SRGB:
				BufferConverter< PF, PixelFormat::eA8B8G8R8_SRGB >()( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;
			case PixelFormat::eR16G16B16_SFLOAT:
				BufferConverter< PF, PixelFormat::eR16G16B16_SFLOAT >()( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;
			case PixelFormat::eR16G16B16A16_SFLOAT:
				BufferConverter< PF, PixelFormat::eR16G16B16A16_SFLOAT >()( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;
			case PixelFormat::eR32G32B32_SFLOAT:
				BufferConverter< PF, PixelFormat::eR32G32B32_SFLOAT >()( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;
			case PixelFormat::eR32G32B32A32_SFLOAT:
				BufferConverter< PF, PixelFormat::eR32G32B32A32_SFLOAT >()( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;
			default:
				if ( PF == dstFormat )
				{
					BufferConverter< PF, PF >()( nullptr, srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				}
				else
				{
					CU_UnsupportedError( "No conversion defined" );
				}
				break;
			}
		}
		/**
		 *\~english
		 *\brief		Buffer conversion
		 *\param[in]	srcBuffer	The source buffer
		 *\param[in]	srcSize		The source buffer size
		 *\param[in]	dstFormat	The destination pixel format
		 *\param[in]	dstBuffer	The destination buffer
		 *\param[in]	dstSize		The destination buffer size
		 *\~french
		 *\brief		Conversion de buffer
		 *\param[in]	srcBuffer	Le buffer source
		 *\param[in]	srcSize		La taille du buffer source
		 *\param[in]	dstFormat	Le format du buffer destination
		 *\param[in]	dstBuffer	Le buffer destination
		 *\param[in]	dstSize		La taille du buffer destination
		 */
		template< PixelFormat PF >
		void dynamicDepthBufferConversion( PxBufferConvertOptions const * options
			, Size const & srcDimensions
			, Size const & dstDimensions
			, uint8_t const * srcBuffer
			, uint32_t srcSize
			, PixelFormat dstFormat
			, uint8_t * dstBuffer
			, uint32_t dstSize )
		{
			switch ( dstFormat )
			{
			case PixelFormat::eD16_UNORM:
				BufferConverter< PF, PixelFormat::eD16_UNORM >()( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;
			case PixelFormat::eD24_UNORM_S8_UINT:
				BufferConverter< PF, PixelFormat::eD24_UNORM_S8_UINT >()( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;
			case PixelFormat::eD32_UNORM:
				BufferConverter< PF, PixelFormat::eD32_UNORM >()( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;
			case PixelFormat::eD32_SFLOAT:
				BufferConverter< PF, PixelFormat::eD32_SFLOAT >()( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;
			case PixelFormat::eD32_SFLOAT_S8_UINT:
				BufferConverter< PF, PixelFormat::eD32_SFLOAT_S8_UINT >()( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;
			case PixelFormat::eS8_UINT:
				BufferConverter< PF, PixelFormat::eS8_UINT >()( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;
			default:
				if ( PF == dstFormat )
				{
					BufferConverter< PF, PF >()( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				}
				else
				{
					CU_UnsupportedError( "No conversion defined" );
				}
				break;
			}
		}
		/**
		 *\~english
		 *\brief		Buffer conversion
		 *\param[in]	srcBuffer	The source buffer
		 *\param[in]	srcSize		The source buffer size
		 *\param[in]	dstFormat	The destination pixel format
		 *\param[in]	dstBuffer	The destination buffer
		 *\param[in]	dstSize		The destination buffer size
		 *\~french
		 *\brief		Conversion de buffer
		 *\param[in]	srcBuffer	Le buffer source
		 *\param[in]	srcSize		La taille du buffer source
		 *\param[in]	dstFormat	Le format du buffer destination
		 *\param[in]	dstBuffer	Le buffer destination
		 *\param[in]	dstSize		La taille du buffer destination
		 */
		template< PixelFormat PF >
		void dynamicStencilBufferConversion( PxBufferConvertOptions const * options
			, Size const & srcDimensions
			, Size const & dstDimensions
			, uint8_t const * srcBuffer
			, uint32_t srcSize
			, PixelFormat dstFormat
			, uint8_t * dstBuffer
			, uint32_t dstSize )
		{
			switch ( dstFormat )
			{
			case PixelFormat::eD24_UNORM_S8_UINT:
				BufferConverter< PF, PixelFormat::eD24_UNORM_S8_UINT >()( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;
			case PixelFormat::eS8_UINT:
				BufferConverter< PF, PixelFormat::eS8_UINT >()( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;
			default:
				if ( PF == dstFormat )
				{
					BufferConverter< PF, PF >()( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				}
				else
				{
					CU_UnsupportedError( "No conversion defined" );
				}
				break;
			}
		}
	}

	//*************************************************************************************************

	//!\~english	Specialisation for PixelFormat::eR8_UNORM
	//!\~french		Spécialisation pour PixelFormat::eR8_UNORM
	template<>
	struct PixelDefinitions< PixelFormat::eR8_UNORM >
	{
		static const uint8_t Size = 1;
		static const uint8_t Count = 1;
		static const bool Alpha = false;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static String toString()
		{
			return cuT( "8 bits luminosity" );
		}
		static String toStr()
		{
			return cuT( "l8" );
		}
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			details::dynamicColourConversion< PixelFormat::eR8_UNORM >( srcBuffer, dstBuffer, dstFormat );
		}
		static void convert( PxBufferConvertOptions const * options, castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			details::dynamicColourBufferConversion< PixelFormat::eR8_UNORM >( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eR8_UNORM, PF >()( srcBuffer, dstBuffer );
		}
	};

	//*************************************************************************************************

	//!\~english	Specialisation for PixelFormat::eR32_SFLOAT
	//!\~french		Spécialisation pour PixelFormat::eR32_SFLOAT
	template<>
	struct PixelDefinitions< PixelFormat::eR32_SFLOAT >
	{
		static const uint8_t Size = 4;
		static const uint8_t Count = 1;
		static const bool Alpha = false;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static String toString()
		{
			return cuT( "Floats luminosity" );
		}
		static String toStr()
		{
			return cuT( "l32f" );
		}
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			details::dynamicColourConversion< PixelFormat::eR32_SFLOAT >( srcBuffer, dstBuffer, dstFormat );
		}
		static void convert( PxBufferConvertOptions const * options, castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			details::dynamicColourBufferConversion< PixelFormat::eR32_SFLOAT >( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eR32_SFLOAT, PF >()( srcBuffer, dstBuffer );
		}
	};

	//*************************************************************************************************

	//!\~english	Specialisation for PixelFormat::eR16_SFLOAT
	//!\~french		Spécialisation pour PixelFormat::eR16_SFLOAT
	template<>
	struct PixelDefinitions< PixelFormat::eR16_SFLOAT >
	{
		static const uint8_t Size = 2;
		static const uint8_t Count = 1;
		static const bool Alpha = false;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static String toString()
		{
			return cuT( "Half floats luminosity" );
		}
		static String toStr()
		{
			return cuT( "l16f" );
		}
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
		}
		static void convert( PxBufferConvertOptions const * options, castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eR16_SFLOAT, PF >()( srcBuffer, dstBuffer );
		}
	};

	//*************************************************************************************************

	//!\~english	Specialisation for PixelFormat::eR8A8_UNORM
	//!\~french		Spécialisation pour PixelFormat::eR8A8_UNORM
	template<>
	struct PixelDefinitions< PixelFormat::eR8A8_UNORM >
	{
		static const uint8_t Size = 2;
		static const uint8_t Count = 2;
		static const bool Alpha = true;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static const PixelFormat NoAlphaPF = PixelFormat::eR8_UNORM;
		static String toString()
		{
			return cuT( "16 bits luminosity and alpha" );
		}
		static String toStr()
		{
			return cuT( "al16" );
		}
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			details::dynamicColourConversion< PixelFormat::eR8A8_UNORM >( srcBuffer, dstBuffer, dstFormat );
		}
		static void convert( PxBufferConvertOptions const * options, castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			details::dynamicColourBufferConversion< PixelFormat::eR8A8_UNORM >( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eR8A8_UNORM, PF >()( srcBuffer, dstBuffer );
		}
	};

	//*************************************************************************************************

	//!\~english	Specialisation for PixelFormat::eR16A16_SFLOAT
	//!\~french		Spécialisation pour PixelFormat::eR16A16_SFLOAT
	template<>
	struct PixelDefinitions< PixelFormat::eR16A16_SFLOAT >
	{
		static const uint8_t Size = 4;
		static const uint8_t Count = 2;
		static const bool Alpha = true;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static const PixelFormat NoAlphaPF = PixelFormat::eR16_SFLOAT;
		static String toString()
		{
			return cuT( "Half floats luminosity and alpha" );
		}
		static String toStr()
		{
			return cuT( "al16f" );
		}
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
		}
		static void convert( PxBufferConvertOptions const * options, castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eR16A16_SFLOAT, PF >()( srcBuffer, dstBuffer );
		}
	};

	//*************************************************************************************************

	//!\~english	Specialisation for PixelFormat::eR32A32_SFLOAT
	//!\~french		Spécialisation pour PixelFormat::eR32A32_SFLOAT
	template<>
	struct PixelDefinitions< PixelFormat::eR32A32_SFLOAT >
	{
		static const uint8_t Size = 8;
		static const uint8_t Count = 2;
		static const bool Alpha = true;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static const PixelFormat NoAlphaPF = PixelFormat::eR32_SFLOAT;
		static String toString()
		{
			return cuT( "Floats luminosity and alpha" );
		}
		static String toStr()
		{
			return cuT( "al32f" );
		}
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			details::dynamicColourConversion< PixelFormat::eR32A32_SFLOAT >( srcBuffer, dstBuffer, dstFormat );
		}
		static void convert( PxBufferConvertOptions const * options, castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			details::dynamicColourBufferConversion< PixelFormat::eR32A32_SFLOAT >( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eR32A32_SFLOAT, PF >()( srcBuffer, dstBuffer );
		}
	};

	//*************************************************************************************************

	//!\~english	Specialisation for PixelFormat::eR5G5B5A1_UNORM
	//!\~french		Spécialisation pour PixelFormat::eR5G5B5A1_UNORM
	template<>
	struct PixelDefinitions< PixelFormat::eR5G5B5A1_UNORM >
	{
		static const uint8_t Size = 2;
		static const uint8_t Count = 4;
		static const bool Alpha = true;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static const PixelFormat NoAlphaPF = PixelFormat::eR5G6B5_UNORM;
		static String toString()
		{
			return cuT( "16 bits 5551 ARGB" );
		}
		static String toStr()
		{
			return cuT( "argb1555" );
		}
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			details::dynamicColourConversion< PixelFormat::eR5G5B5A1_UNORM >( srcBuffer, dstBuffer, dstFormat );
		}
		static void convert( PxBufferConvertOptions const * options, castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			details::dynamicColourBufferConversion< PixelFormat::eR5G5B5A1_UNORM >( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eR5G5B5A1_UNORM, PF >()( srcBuffer, dstBuffer );
		}
	};

	//*************************************************************************************************

	//!\~english	Specialisation for PixelFormat::eR5G6B5_UNORM
	//!\~french		Spécialisation pour PixelFormat::eR5G6B5_UNORM
	template<>
	struct PixelDefinitions< PixelFormat::eR5G6B5_UNORM >
	{
		static const uint8_t Size = 2;
		static const uint8_t Count = 3;
		static const bool Alpha = false;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static String toString()
		{
			return cuT( "16 bits 565 RGB" );
		}
		static String toStr()
		{
			return cuT( "rgb565" );
		}
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			details::dynamicColourConversion< PixelFormat::eR5G6B5_UNORM >( srcBuffer, dstBuffer, dstFormat );
		}
		static void convert( PxBufferConvertOptions const * options, castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			details::dynamicColourBufferConversion< PixelFormat::eR5G6B5_UNORM >( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eR5G6B5_UNORM, PF >()( srcBuffer, dstBuffer );
		}
	};

	//*************************************************************************************************

	//!\~english	Specialisation for PixelFormat::eR8G8B8_UNORM
	//!\~french		Spécialisation pour PixelFormat::eR8G8B8_UNORM
	template<>
	struct PixelDefinitions< PixelFormat::eR8G8B8_UNORM >
	{
		static const uint8_t Size = 3;
		static const uint8_t Count = 3;
		static const bool Alpha = false;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static String toString()
		{
			return cuT( "24 bits 888 RGB" );
		}
		static String toStr()
		{
			return cuT( "rgb24" );
		}
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			details::dynamicColourConversion< PixelFormat::eR8G8B8_UNORM >( srcBuffer, dstBuffer, dstFormat );
		}
		static void convert( PxBufferConvertOptions const * options, castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			details::dynamicColourBufferConversion< PixelFormat::eR8G8B8_UNORM >( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eR8G8B8_UNORM, PF >()( srcBuffer, dstBuffer );
		}
	};

	//*************************************************************************************************

	//!\~english	Specialisation for PixelFormat::eB8G8R8_UNORM
	//!\~french		Spécialisation pour PixelFormat::eB8G8R8_UNORM
	template<>
	struct PixelDefinitions< PixelFormat::eB8G8R8_UNORM >
	{
		static const uint8_t Size = 3;
		static const uint8_t Count = 3;
		static const bool Alpha = false;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static String toString()
		{
			return cuT( "24 bits 888 BGR" );
		}
		static String toStr()
		{
			return cuT( "bgr24" );
		}
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			details::dynamicColourConversion< PixelFormat::eB8G8R8_UNORM >( srcBuffer, dstBuffer, dstFormat );
		}
		static void convert( PxBufferConvertOptions const * options, castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			details::dynamicColourBufferConversion< PixelFormat::eB8G8R8_UNORM >( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eB8G8R8_UNORM, PF >()( srcBuffer, dstBuffer );
		}
	};

	//*************************************************************************************************

	//!\~english	Specialisation for PixelFormat::eB8G8R8A8_UNORM
	//!\~french		Spécialisation pour PixelFormat::eB8G8R8A8_UNORM
	template<>
	struct PixelDefinitions< PixelFormat::eB8G8R8A8_UNORM >
	{
		static const uint8_t Size = 4;
		static const uint8_t Count = 4;
		static const bool Alpha = true;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static const PixelFormat NoAlphaPF = PixelFormat::eB8G8R8_UNORM;
		static String toString()
		{
			return cuT( "32 bits 8888 BGRA" );
		}
		static String toStr()
		{
			return cuT( "bgra32" );
		}
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			details::dynamicColourConversion< PixelFormat::eB8G8R8A8_UNORM >( srcBuffer, dstBuffer, dstFormat );
		}
		static void convert( PxBufferConvertOptions const * options, castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			details::dynamicColourBufferConversion< PixelFormat::eB8G8R8A8_UNORM >( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eB8G8R8A8_UNORM, PF >()( srcBuffer, dstBuffer );
		}
	};

	//*************************************************************************************************

	//!\~english	Specialisation for PixelFormat::eR8G8B8_SRGB
	//!\~french		Spécialisation pour PixelFormat::eR8G8B8_SRGB
	template<>
	struct PixelDefinitions< PixelFormat::eR8G8B8_SRGB >
	{
		static const uint8_t Size = 3;
		static const uint8_t Count = 3;
		static const bool Alpha = false;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static String toString()
		{
			return cuT( "24 bits 888 RGB sRGB" );
		}
		static String toStr()
		{
			return cuT( "rgb24_srgb" );
		}
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			details::dynamicColourConversion< PixelFormat::eR8G8B8_SRGB >( srcBuffer, dstBuffer, dstFormat );
		}
		static void convert( PxBufferConvertOptions const * options, castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			details::dynamicColourBufferConversion< PixelFormat::eR8G8B8_SRGB >( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eR8G8B8_SRGB, PF >()( srcBuffer, dstBuffer );
		}
	};

	//*************************************************************************************************

	//!\~english	Specialisation for PixelFormat::eB8G8R8_SRGB
	//!\~french		Spécialisation pour PixelFormat::eB8G8R8_SRGB
	template<>
	struct PixelDefinitions< PixelFormat::eB8G8R8_SRGB >
	{
		static const uint8_t Size = 3;
		static const uint8_t Count = 3;
		static const bool Alpha = false;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static String toString()
		{
			return cuT( "24 bits 888 BGR sRGB" );
		}
		static String toStr()
		{
			return cuT( "bgr24_srgb" );
		}
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			details::dynamicColourConversion< PixelFormat::eB8G8R8_SRGB >( srcBuffer, dstBuffer, dstFormat );
		}
		static void convert( PxBufferConvertOptions const * options, castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			details::dynamicColourBufferConversion< PixelFormat::eB8G8R8_SRGB >( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eB8G8R8_SRGB, PF >()( srcBuffer, dstBuffer );
		}
	};

	//*************************************************************************************************

	//!\~english	Specialisation for PixelFormat::eR8G8B8A8_UNORM
	//!\~french		Spécialisation pour PixelFormat::eR8G8B8A8_UNORM
	template<>
	struct PixelDefinitions< PixelFormat::eR8G8B8A8_UNORM >
	{
		static const uint8_t Size = 4;
		static const uint8_t Count = 4;
		static const bool Alpha = true;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static const PixelFormat NoAlphaPF = PixelFormat::eR8G8B8_UNORM;
		static String toString()
		{
			return cuT( "32 bits 8888 ARGB" );
		}
		static String toStr()
		{
			return cuT( "argb32" );
		}
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			details::dynamicColourConversion< PixelFormat::eR8G8B8A8_UNORM >( srcBuffer, dstBuffer, dstFormat );
		}
		static void convert( PxBufferConvertOptions const * options, castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			details::dynamicColourBufferConversion< PixelFormat::eR8G8B8A8_UNORM >( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eR8G8B8A8_UNORM, PF >()( srcBuffer, dstBuffer );
		}
	};

	//*************************************************************************************************

	//!\~english	Specialisation for PixelFormat::eA8B8G8R8_UNORM
	//!\~french		Spécialisation pour PixelFormat::eA8B8G8R8_UNORM
	template<>
	struct PixelDefinitions< PixelFormat::eA8B8G8R8_UNORM >
	{
		static const uint8_t Size = 4;
		static const uint8_t Count = 4;
		static const bool Alpha = true;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static const PixelFormat NoAlphaPF = PixelFormat::eB8G8R8_UNORM;
		static String toString()
		{
			return cuT( "32 bits 8888 ABGR" );
		}
		static String toStr()
		{
			return cuT( "abgr32" );
		}
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			details::dynamicColourConversion< PixelFormat::eA8B8G8R8_UNORM >( srcBuffer, dstBuffer, dstFormat );
		}
		static void convert( PxBufferConvertOptions const * options, castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			details::dynamicColourBufferConversion< PixelFormat::eA8B8G8R8_UNORM >( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eA8B8G8R8_UNORM, PF >()( srcBuffer, dstBuffer );
		}
	};

	//*************************************************************************************************

	//!\~english	Specialisation for PixelFormat::eR8G8B8A8_SRGB
	//!\~french		Spécialisation pour PixelFormat::eR8G8B8A8_SRGB
	template<>
	struct PixelDefinitions< PixelFormat::eR8G8B8A8_SRGB >
	{
		static const uint8_t Size = 4;
		static const uint8_t Count = 4;
		static const bool Alpha = true;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static const PixelFormat NoAlphaPF = PixelFormat::eR8G8B8_SRGB;
		static String toString()
		{
			return cuT( "32 bits 8888 ARGB sRGB" );
		}
		static String toStr()
		{
			return cuT( "argb32_srgb" );
		}
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			details::dynamicColourConversion< PixelFormat::eR8G8B8A8_SRGB >( srcBuffer, dstBuffer, dstFormat );
		}
		static void convert( PxBufferConvertOptions const * options, castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			details::dynamicColourBufferConversion< PixelFormat::eR8G8B8A8_SRGB >( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eR8G8B8A8_SRGB, PF >()( srcBuffer, dstBuffer );
		}
	};

	//*************************************************************************************************

	//!\~english	Specialisation for PixelFormat::eA8B8G8R8_SRGB
	//!\~french		Spécialisation pour PixelFormat::eA8B8G8R8_SRGB
	template<>
	struct PixelDefinitions< PixelFormat::eA8B8G8R8_SRGB >
	{
		static const uint8_t Size = 4;
		static const uint8_t Count = 4;
		static const bool Alpha = true;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static const PixelFormat NoAlphaPF = PixelFormat::eB8G8R8_SRGB;
		static String toString()
		{
			return cuT( "32 bits 8888 ABGR sRGB" );
		}
		static String toStr()
		{
			return cuT( "abgr32_srgb" );
		}
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			details::dynamicColourConversion< PixelFormat::eA8B8G8R8_SRGB >( srcBuffer, dstBuffer, dstFormat );
		}
		static void convert( PxBufferConvertOptions const * options, castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			details::dynamicColourBufferConversion< PixelFormat::eA8B8G8R8_SRGB >( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eA8B8G8R8_SRGB, PF >()( srcBuffer, dstBuffer );
		}
	};

	//*************************************************************************************************

	//!\~english	Specialisation for PixelFormat::eR16G16B16_SFLOAT
	//!\~french		Spécialisation pour PixelFormat::eR16G16B16_SFLOAT
	template<>
	struct PixelDefinitions< PixelFormat::eR16G16B16_SFLOAT >
	{
		static const uint8_t Size = 6;
		static const uint8_t Count = 3;
		static const bool Alpha = false;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static String toString()
		{
			return cuT( "Half floating point RGB" );
		}
		static String toStr()
		{
			return cuT( "rgb16f" );
		}
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			details::dynamicColourConversion< PixelFormat::eR16G16B16_SFLOAT >( srcBuffer, dstBuffer, dstFormat );
		}
		static void convert( PxBufferConvertOptions const * options, castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			details::dynamicColourBufferConversion< PixelFormat::eR16G16B16_SFLOAT >( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eR16G16B16_SFLOAT, PF >()( srcBuffer, dstBuffer );
		}
	};

	//*************************************************************************************************

	//!\~english	Specialisation for PixelFormat::eR16G16B16A16_SFLOAT
	//!\~french		Spécialisation pour PixelFormat::eR16G16B16A16_SFLOAT
	template<>
	struct PixelDefinitions< PixelFormat::eR16G16B16A16_SFLOAT >
	{
		static const uint8_t Size = 8;
		static const uint8_t Count = 4;
		static const bool Alpha = true;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static const PixelFormat NoAlphaPF = PixelFormat::eR16G16B16_SFLOAT;
		static String toString()
		{
			return cuT( "Half floating point ARGB" );
		}
		static String toStr()
		{
			return cuT( "argb16f" );
		}
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			details::dynamicColourConversion< PixelFormat::eR16G16B16A16_SFLOAT >( srcBuffer, dstBuffer, dstFormat );
		}
		static void convert( PxBufferConvertOptions const * options, castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			details::dynamicColourBufferConversion< PixelFormat::eR16G16B16A16_SFLOAT >( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eR16G16B16A16_SFLOAT, PF >()( srcBuffer, dstBuffer );
		}
	};

	//*************************************************************************************************

	//!\~english	Specialisation for PixelFormat::eR32G32B32_SFLOAT
	//!\~french		Spécialisation pour PixelFormat::eR32G32B32_SFLOAT
	template<>
	struct PixelDefinitions< PixelFormat::eR32G32B32_SFLOAT >
	{
		static const uint8_t Size = 12;
		static const uint8_t Count = 3;
		static const bool Alpha = false;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static String toString()
		{
			return cuT( "Floating point RGB" );
		}
		static String toStr()
		{
			return cuT( "rgb32f" );
		}
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			details::dynamicColourConversion< PixelFormat::eR32G32B32_SFLOAT >( srcBuffer, dstBuffer, dstFormat );
		}
		static void convert( PxBufferConvertOptions const * options, castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			details::dynamicColourBufferConversion< PixelFormat::eR32G32B32_SFLOAT >( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eR32G32B32_SFLOAT, PF >()( srcBuffer, dstBuffer );
		}
	};

	//*************************************************************************************************

	//!\~english	Specialisation for PixelFormat::eR32G32B32A32_SFLOAT
	//!\~french		Spécialisation pour PixelFormat::eR32G32B32A32_SFLOAT
	template<>
	struct PixelDefinitions< PixelFormat::eR32G32B32A32_SFLOAT >
	{
		static const uint8_t Size = 16;
		static const uint8_t Count = 4;
		static const bool Alpha = true;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static const PixelFormat NoAlphaPF = PixelFormat::eR32G32B32_SFLOAT;
		static String toString()
		{
			return cuT( "Floating point ARGB" );
		}
		static String toStr()
		{
			return cuT( "argb32f" );
		}
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			details::dynamicColourConversion< PixelFormat::eR32G32B32A32_SFLOAT >( srcBuffer, dstBuffer, dstFormat );
		}
		static void convert( PxBufferConvertOptions const * options, castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			details::dynamicColourBufferConversion< PixelFormat::eR32G32B32A32_SFLOAT >( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eR32G32B32A32_SFLOAT, PF >()( srcBuffer, dstBuffer );
		}
	};

	//*************************************************************************************************

	//!\~english	Specialisation for PixelFormat::eBC1_RGB_UNORM_BLOCK
	//!\~french		Spécialisation pour PixelFormat::eBC1_RGB_UNORM_BLOCK
	template<>
	struct PixelDefinitions< PixelFormat::eBC1_RGB_UNORM_BLOCK >
	{
		static const uint8_t Size = 8u; // Bit count for a 4x4 pixels block
		static const uint8_t Count = 3;
		static const bool Alpha = false;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = true;
		static String toString()
		{
			return cuT( "BC1 8 bits compressed format" );
		}
		static String toStr()
		{
			return cuT( "bc1_unorm" );
		}
		static void convert( uint8_t const * srcBuffer, uint8_t * dstBuffer, PixelFormat dstFormat )
		{
			if ( dstFormat == PixelFormat::eBC1_RGB_UNORM_BLOCK )
			{
				details::PixelConverter< PixelFormat::eBC1_RGB_UNORM_BLOCK, PixelFormat::eBC1_RGB_UNORM_BLOCK >()( srcBuffer, dstBuffer );
			}
		}
		static void convert( PxBufferConvertOptions const * options, castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			if ( dstFormat == PixelFormat::eBC1_RGB_UNORM_BLOCK )
			{
				details::BufferConverter< PixelFormat::eBC1_RGB_UNORM_BLOCK, PixelFormat::eBC1_RGB_UNORM_BLOCK >()( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
			}
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eBC1_RGB_UNORM_BLOCK, PF >()( srcBuffer, dstBuffer );
		}
	};

	//*************************************************************************************************

	//!\~english	Specialisation for PixelFormat::eBC1_RGB_SRGB_BLOCK
	//!\~french		Spécialisation pour PixelFormat::eBC1_RGB_SRGB_BLOCK
	template<>
	struct PixelDefinitions< PixelFormat::eBC1_RGB_SRGB_BLOCK >
	{
		static const uint8_t Size = 8u; // Bit count for a 4x4 pixels block
		static const uint8_t Count = 3;
		static const bool Alpha = false;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = true;
		static String toString()
		{
			return cuT( "BC1 SRGB 8 bits compressed format" );
		}
		static String toStr()
		{
			return cuT( "bc1_srgb" );
		}
		static void convert( uint8_t const * srcBuffer, uint8_t * dstBuffer, PixelFormat dstFormat )
		{
			if ( dstFormat == PixelFormat::eBC1_RGB_SRGB_BLOCK )
			{
				details::PixelConverter< PixelFormat::eBC1_RGB_SRGB_BLOCK, PixelFormat::eBC1_RGB_SRGB_BLOCK >()( srcBuffer, dstBuffer );
			}
		}
		static void convert( PxBufferConvertOptions const * options, castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			if ( dstFormat == PixelFormat::eBC1_RGB_SRGB_BLOCK )
			{
				details::BufferConverter< PixelFormat::eBC1_RGB_SRGB_BLOCK, PixelFormat::eBC1_RGB_SRGB_BLOCK >()( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
			}
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eBC1_RGB_SRGB_BLOCK, PF >()( srcBuffer, dstBuffer );
		}
	};

	//*************************************************************************************************

	//!\~english	Specialisation for PixelFormat::eBC1_RGBA_UNORM_BLOCK
	//!\~french		Spécialisation pour PixelFormat::eBC1_RGBA_UNORM_BLOCK
	template<>
	struct PixelDefinitions< PixelFormat::eBC1_RGBA_UNORM_BLOCK >
	{
		static const uint8_t Size = 8u; // Bit count for a 4x4 pixels block
		static const uint8_t Count = 4;
		static const bool Alpha = true;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = true;
		static String toString()
		{
			return cuT( "BC1 8 bits compressed format" );
		}
		static String toStr()
		{
			return cuT( "bc1_rgba_unorm" );
		}
		static void convert( uint8_t const * srcBuffer, uint8_t * dstBuffer, PixelFormat dstFormat )
		{
			if ( dstFormat == PixelFormat::eBC1_RGBA_UNORM_BLOCK )
			{
				details::PixelConverter< PixelFormat::eBC1_RGBA_UNORM_BLOCK, PixelFormat::eBC1_RGBA_UNORM_BLOCK >()( srcBuffer, dstBuffer );
			}
		}
		static void convert( PxBufferConvertOptions const * options, castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			if ( dstFormat == PixelFormat::eBC1_RGBA_UNORM_BLOCK )
			{
				details::BufferConverter< PixelFormat::eBC1_RGBA_UNORM_BLOCK, PixelFormat::eBC1_RGBA_UNORM_BLOCK >()( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
			}
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eBC1_RGBA_UNORM_BLOCK, PF >()( srcBuffer, dstBuffer );
		}
	};

	//*************************************************************************************************

	//!\~english	Specialisation for PixelFormat::eBC1_RGBA_SRGB_BLOCK
	//!\~french		Spécialisation pour PixelFormat::eBC1_RGBA_SRGB_BLOCK
	template<>
	struct PixelDefinitions< PixelFormat::eBC1_RGBA_SRGB_BLOCK >
	{
		static const uint8_t Size = 8u; // Bit count for a 4x4 pixels block
		static const uint8_t Count = 4;
		static const bool Alpha = true;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = true;
		static String toString()
		{
			return cuT( "BC1 SRGB 8 bits compressed format" );
		}
		static String toStr()
		{
			return cuT( "bc1_rgba_srgb" );
		}
		static void convert( uint8_t const * srcBuffer, uint8_t * dstBuffer, PixelFormat dstFormat )
		{
			if ( dstFormat == PixelFormat::eBC1_RGBA_SRGB_BLOCK )
			{
				details::PixelConverter< PixelFormat::eBC1_RGBA_SRGB_BLOCK, PixelFormat::eBC1_RGBA_SRGB_BLOCK >()( srcBuffer, dstBuffer );
			}
		}
		static void convert( PxBufferConvertOptions const * options, castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			if ( dstFormat == PixelFormat::eBC1_RGBA_SRGB_BLOCK )
			{
				details::BufferConverter< PixelFormat::eBC1_RGBA_SRGB_BLOCK, PixelFormat::eBC1_RGBA_SRGB_BLOCK >()( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
			}
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eBC1_RGBA_SRGB_BLOCK, PF >()( srcBuffer, dstBuffer );
		}
	};

	//*************************************************************************************************

	//!\~english	Specialisation for PixelFormat::eBC2_UNORM_BLOCK
	//!\~french		Spécialisation pour PixelFormat::eBC2_UNORM_BLOCK
	template<>
	struct PixelDefinitions< PixelFormat::eBC2_UNORM_BLOCK >
	{
		static const uint8_t Size = 16u; // Bit count for a 4x4 pixels block
		static const uint8_t Count = 4;
		static const bool Alpha = true;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = true;
		static const PixelFormat NoAlphaPF = PixelFormat::eBC2_UNORM_BLOCK;
		static String toString()
		{
			return cuT( "BC2 RGBA 16 bits compressed format" );
		}
		static String toStr()
		{
			return cuT( "bc2_unorm" );
		}
		static void convert( uint8_t const * srcBuffer, uint8_t * dstBuffer, PixelFormat dstFormat )
		{
			if ( dstFormat == PixelFormat::eBC2_UNORM_BLOCK )
			{
				details::PixelConverter< PixelFormat::eBC2_UNORM_BLOCK, PixelFormat::eBC2_UNORM_BLOCK >()( srcBuffer, dstBuffer );
			}
		}
		static void convert( PxBufferConvertOptions const * options, castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			if ( dstFormat == PixelFormat::eBC2_UNORM_BLOCK )
			{
				details::BufferConverter< PixelFormat::eBC2_UNORM_BLOCK, PixelFormat::eBC2_UNORM_BLOCK >()( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
			}
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eBC2_UNORM_BLOCK, PF >()( srcBuffer, dstBuffer );
		}
	};

	//*************************************************************************************************

	//!\~english	Specialisation for PixelFormat::eBC2_SRGB_BLOCK
	//!\~french		Spécialisation pour PixelFormat::eBC2_SRGB_BLOCK
	template<>
	struct PixelDefinitions< PixelFormat::eBC2_SRGB_BLOCK >
	{
		static const uint8_t Size = 16u; // Bit count for a 4x4 pixels block
		static const uint8_t Count = 4;
		static const bool Alpha = true;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = true;
		static const PixelFormat NoAlphaPF = PixelFormat::eBC2_SRGB_BLOCK;
		static String toString()
		{
			return cuT( "BC2 RGBA SRGB 16 bits compressed format" );
		}
		static String toStr()
		{
			return cuT( "bc2_srgb" );
		}
		static void convert( uint8_t const * srcBuffer, uint8_t * dstBuffer, PixelFormat dstFormat )
		{
			if ( dstFormat == PixelFormat::eBC2_UNORM_BLOCK )
			{
				details::PixelConverter< PixelFormat::eBC2_SRGB_BLOCK, PixelFormat::eBC2_SRGB_BLOCK >()( srcBuffer, dstBuffer );
			}
		}
		static void convert( PxBufferConvertOptions const * options, castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			if ( dstFormat == PixelFormat::eBC2_SRGB_BLOCK )
			{
				details::BufferConverter< PixelFormat::eBC2_SRGB_BLOCK, PixelFormat::eBC2_SRGB_BLOCK >()( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
			}
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eBC2_SRGB_BLOCK, PF >()( srcBuffer, dstBuffer );
		}
	};

	//*************************************************************************************************

	//!\~english	Specialisation for PixelFormat::eBC3_UNORM_BLOCK
	//!\~french		Spécialisation pour PixelFormat::eBC3_UNORM_BLOCK
	template<>
	struct PixelDefinitions< PixelFormat::eBC3_UNORM_BLOCK >
	{
		static const uint8_t Size = 16u; // Bit count for a 4x4 pixels block
		static const uint8_t Count = 4;
		static const bool Alpha = true;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = true;
		static const PixelFormat NoAlphaPF = PixelFormat::eBC3_UNORM_BLOCK;
		static String toString()
		{
			return cuT( "BC3 16 bits compressed format" );
		}
		static String toStr()
		{
			return cuT( "bc3_unorm" );
		}
		static void convert( uint8_t const * srcBuffer, uint8_t * dstBuffer, PixelFormat dstFormat )
		{
			if ( dstFormat == PixelFormat::eBC3_UNORM_BLOCK )
			{
				details::PixelConverter< PixelFormat::eBC3_UNORM_BLOCK, PixelFormat::eBC3_UNORM_BLOCK >()( srcBuffer, dstBuffer );
			}
		}
		static void convert( PxBufferConvertOptions const * options, castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			if ( dstFormat == PixelFormat::eBC3_UNORM_BLOCK )
			{
				details::BufferConverter< PixelFormat::eBC3_UNORM_BLOCK, PixelFormat::eBC3_UNORM_BLOCK >()( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
			}
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eBC3_UNORM_BLOCK, PF >()( srcBuffer, dstBuffer );
		}
	};

	//*************************************************************************************************

	//!\~english	Specialisation for PixelFormat::eBC3_SRGB_BLOCK
	//!\~french		Spécialisation pour PixelFormat::eBC3_SRGB_BLOCK
	template<>
	struct PixelDefinitions< PixelFormat::eBC3_SRGB_BLOCK >
	{
		static const uint8_t Size = 16u; // Bit count for a 4x4 pixels block
		static const uint8_t Count = 4;
		static const bool Alpha = true;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = true;
		static const PixelFormat NoAlphaPF = PixelFormat::eBC3_SRGB_BLOCK;
		static String toString()
		{
			return cuT( "BC3 SRGB 16 bits compressed format" );
		}
		static String toStr()
		{
			return cuT( "bc3_srgb" );
		}
		static void convert( uint8_t const * srcBuffer, uint8_t * dstBuffer, PixelFormat dstFormat )
		{
			if ( dstFormat == PixelFormat::eBC3_SRGB_BLOCK )
			{
				details::PixelConverter< PixelFormat::eBC3_SRGB_BLOCK, PixelFormat::eBC3_SRGB_BLOCK >()( srcBuffer, dstBuffer );
			}
		}
		static void convert( PxBufferConvertOptions const * options, castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			if ( dstFormat == PixelFormat::eBC3_SRGB_BLOCK )
			{
				details::BufferConverter< PixelFormat::eBC3_SRGB_BLOCK, PixelFormat::eBC3_SRGB_BLOCK >()( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
			}
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eBC3_SRGB_BLOCK, PF >()( srcBuffer, dstBuffer );
		}
	};

	//*************************************************************************************************

	//!\~english	Specialisation for PixelFormat::eBC4_UNORM_BLOCK
	//!\~french		Spécialisation pour PixelFormat::eBC4_UNORM_BLOCK
	template<>
	struct PixelDefinitions< PixelFormat::eBC4_UNORM_BLOCK >
	{
		static const uint8_t Size = 8u; // Bit count for a 4x4 pixels block
		static const uint8_t Count = 1;
		static const bool Alpha = false;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = true;
		static const PixelFormat NoAlphaPF = PixelFormat::eBC4_UNORM_BLOCK;
		static String toString()
		{
			return cuT( "BC4 8 bits compressed format" );
		}
		static String toStr()
		{
			return cuT( "bc4_unorm" );
		}
		static void convert( uint8_t const * srcBuffer, uint8_t * dstBuffer, PixelFormat dstFormat )
		{
			if ( dstFormat == PixelFormat::eBC4_UNORM_BLOCK )
			{
				details::PixelConverter< PixelFormat::eBC4_UNORM_BLOCK, PixelFormat::eBC4_UNORM_BLOCK >()( srcBuffer, dstBuffer );
			}
		}
		static void convert( PxBufferConvertOptions const * options, castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			if ( dstFormat == PixelFormat::eBC4_UNORM_BLOCK )
			{
				details::BufferConverter< PixelFormat::eBC4_UNORM_BLOCK, PixelFormat::eBC4_UNORM_BLOCK >()( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
			}
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eBC4_UNORM_BLOCK, PF >()( srcBuffer, dstBuffer );
		}
	};

	//*************************************************************************************************

	//!\~english	Specialisation for PixelFormat::eBC4_SNORM_BLOCK
	//!\~french		Spécialisation pour PixelFormat::eBC4_SNORM_BLOCK
	template<>
	struct PixelDefinitions< PixelFormat::eBC4_SNORM_BLOCK >
	{
		static const uint8_t Size = 8u; // Bit count for a 4x4 pixels block
		static const uint8_t Count = 1;
		static const bool Alpha = false;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = true;
		static const PixelFormat NoAlphaPF = PixelFormat::eBC4_SNORM_BLOCK;
		static String toString()
		{
			return cuT( "BC4 8 bits compressed format" );
		}
		static String toStr()
		{
			return cuT( "bc4_snorm" );
		}
		static void convert( uint8_t const * srcBuffer, uint8_t * dstBuffer, PixelFormat dstFormat )
		{
			if ( dstFormat == PixelFormat::eBC4_SNORM_BLOCK )
			{
				details::PixelConverter< PixelFormat::eBC4_SNORM_BLOCK, PixelFormat::eBC4_SNORM_BLOCK >()( srcBuffer, dstBuffer );
			}
		}
		static void convert( PxBufferConvertOptions const * options, castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			if ( dstFormat == PixelFormat::eBC4_SNORM_BLOCK )
			{
				details::BufferConverter< PixelFormat::eBC4_SNORM_BLOCK, PixelFormat::eBC4_SNORM_BLOCK >()( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
			}
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eBC4_SNORM_BLOCK, PF >()( srcBuffer, dstBuffer );
		}
	};

	//*************************************************************************************************

	//!\~english	Specialisation for PixelFormat::eBC5_UNORM_BLOCK
	//!\~french		Spécialisation pour PixelFormat::eBC5_UNORM_BLOCK
	template<>
	struct PixelDefinitions< PixelFormat::eBC5_UNORM_BLOCK >
	{
		static const uint8_t Size = 16u; // Bit count for a 4x4 pixels block
		static const uint8_t Count = 2;
		static const bool Alpha = false;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = true;
		static const PixelFormat NoAlphaPF = PixelFormat::eBC5_UNORM_BLOCK;
		static String toString()
		{
			return cuT( "BC5 16 bits compressed format" );
		}
		static String toStr()
		{
			return cuT( "bc5_unorm" );
		}
		static void convert( uint8_t const * srcBuffer, uint8_t * dstBuffer, PixelFormat dstFormat )
		{
			if ( dstFormat == PixelFormat::eBC5_UNORM_BLOCK )
			{
				details::PixelConverter< PixelFormat::eBC5_UNORM_BLOCK, PixelFormat::eBC5_UNORM_BLOCK >()( srcBuffer, dstBuffer );
			}
		}
		static void convert( PxBufferConvertOptions const * options, castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			if ( dstFormat == PixelFormat::eBC5_UNORM_BLOCK )
			{
				details::BufferConverter< PixelFormat::eBC5_UNORM_BLOCK, PixelFormat::eBC5_UNORM_BLOCK >()( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
			}
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eBC5_UNORM_BLOCK, PF >()( srcBuffer, dstBuffer );
		}
	};

	//*************************************************************************************************

	//!\~english	Specialisation for PixelFormat::eBC5_SNORM_BLOCK
	//!\~french		Spécialisation pour PixelFormat::eBC5_SNORM_BLOCK
	template<>
	struct PixelDefinitions< PixelFormat::eBC5_SNORM_BLOCK >
	{
		static const uint8_t Size = 16u; // Bit count for a 4x4 pixels block
		static const uint8_t Count = 2;
		static const bool Alpha = false;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = true;
		static const PixelFormat NoAlphaPF = PixelFormat::eBC5_SNORM_BLOCK;
		static String toString()
		{
			return cuT( "BC5 16 bits compressed format" );
		}
		static String toStr()
		{
			return cuT( "bc5_snorm" );
		}
		static void convert( uint8_t const * srcBuffer, uint8_t * dstBuffer, PixelFormat dstFormat )
		{
			if ( dstFormat == PixelFormat::eBC5_SNORM_BLOCK )
			{
				details::PixelConverter< PixelFormat::eBC5_SNORM_BLOCK, PixelFormat::eBC5_SNORM_BLOCK >()( srcBuffer, dstBuffer );
			}
		}
		static void convert( PxBufferConvertOptions const * options, castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			if ( dstFormat == PixelFormat::eBC5_SNORM_BLOCK )
			{
				details::BufferConverter< PixelFormat::eBC5_SNORM_BLOCK, PixelFormat::eBC5_SNORM_BLOCK >()( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
			}
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eBC5_SNORM_BLOCK, PF >()( srcBuffer, dstBuffer );
		}
	};

	//*************************************************************************************************

	//!\~english	Specialisation for PixelFormat::eBC5_SNORM_BLOCK
	//!\~french		Spécialisation pour PixelFormat::eBC5_SNORM_BLOCK
	template<>
	struct PixelDefinitions< PixelFormat::eBC6H_UFLOAT_BLOCK >
	{
		static const uint8_t Size = 16u; // Bit count for a 4x4 pixels block
		static const uint8_t Count = 3;
		static const bool Alpha = false;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = true;
		static const PixelFormat NoAlphaPF = PixelFormat::eBC6H_UFLOAT_BLOCK;
		static String toString()
		{
			return cuT( "BC6H 16 bits compressed format" );
		}
		static String toStr()
		{
			return cuT( "bc6_ufloat" );
		}
		static void convert( uint8_t const * srcBuffer, uint8_t * dstBuffer, PixelFormat dstFormat )
		{
			if ( dstFormat == PixelFormat::eBC6H_UFLOAT_BLOCK )
			{
				details::PixelConverter< PixelFormat::eBC6H_UFLOAT_BLOCK, PixelFormat::eBC6H_UFLOAT_BLOCK >()( srcBuffer, dstBuffer );
			}
		}
		static void convert( PxBufferConvertOptions const * options, castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			if ( dstFormat == PixelFormat::eBC6H_UFLOAT_BLOCK )
			{
				details::BufferConverter< PixelFormat::eBC6H_UFLOAT_BLOCK, PixelFormat::eBC6H_UFLOAT_BLOCK >()( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
			}
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eBC6H_UFLOAT_BLOCK, PF >()( srcBuffer, dstBuffer );
		}
	};

	//*************************************************************************************************

	//!\~english	Specialisation for PixelFormat::eBC6H_SFLOAT_BLOCK
	//!\~french		Spécialisation pour PixelFormat::eBC6H_SFLOAT_BLOCK
	template<>
	struct PixelDefinitions< PixelFormat::eBC6H_SFLOAT_BLOCK >
	{
		static const uint8_t Size = 16u; // Bit count for a 4x4 pixels block
		static const uint8_t Count = 3;
		static const bool Alpha = false;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = true;
		static const PixelFormat NoAlphaPF = PixelFormat::eBC6H_SFLOAT_BLOCK;
		static String toString()
		{
			return cuT( "BC6H 16 bits compressed format" );
		}
		static String toStr()
		{
			return cuT( "bc6_sfloat" );
		}
		static void convert( uint8_t const * srcBuffer, uint8_t * dstBuffer, PixelFormat dstFormat )
		{
			if ( dstFormat == PixelFormat::eBC6H_SFLOAT_BLOCK )
			{
				details::PixelConverter< PixelFormat::eBC6H_SFLOAT_BLOCK, PixelFormat::eBC6H_SFLOAT_BLOCK >()( srcBuffer, dstBuffer );
			}
		}
		static void convert( PxBufferConvertOptions const * options, castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			if ( dstFormat == PixelFormat::eBC6H_SFLOAT_BLOCK )
			{
				details::BufferConverter< PixelFormat::eBC6H_SFLOAT_BLOCK, PixelFormat::eBC6H_SFLOAT_BLOCK >()( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
			}
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eBC6H_SFLOAT_BLOCK, PF >()( srcBuffer, dstBuffer );
		}
	};

	//*************************************************************************************************

	//!\~english	Specialisation for PixelFormat::eBC7_UNORM_BLOCK
	//!\~french		Spécialisation pour PixelFormat::eBC7_UNORM_BLOCK
	template<>
	struct PixelDefinitions< PixelFormat::eBC7_UNORM_BLOCK >
	{
		static const uint8_t Size = 16u; // Bit count for a 4x4 pixels block
		static const uint8_t Count = 4;
		static const bool Alpha = true;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = true;
		static const PixelFormat NoAlphaPF = PixelFormat::eBC7_UNORM_BLOCK;
		static String toString()
		{
			return cuT( "BC7 16 bits compressed format" );
		}
		static String toStr()
		{
			return cuT( "bc7_unorm" );
		}
		static void convert( uint8_t const * srcBuffer, uint8_t * dstBuffer, PixelFormat dstFormat )
		{
			if ( dstFormat == PixelFormat::eBC7_UNORM_BLOCK )
			{
				details::PixelConverter< PixelFormat::eBC7_UNORM_BLOCK, PixelFormat::eBC7_UNORM_BLOCK >()( srcBuffer, dstBuffer );
			}
		}
		static void convert( PxBufferConvertOptions const * options, castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			if ( dstFormat == PixelFormat::eBC7_UNORM_BLOCK )
			{
				details::BufferConverter< PixelFormat::eBC7_UNORM_BLOCK, PixelFormat::eBC7_UNORM_BLOCK >()( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
			}
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eBC7_UNORM_BLOCK, PF >()( srcBuffer, dstBuffer );
		}
	};

	//*************************************************************************************************

	//!\~english	Specialisation for PixelFormat::eBC7_SRGB_BLOCK
	//!\~french		Spécialisation pour PixelFormat::eBC7_SRGB_BLOCK
	template<>
	struct PixelDefinitions< PixelFormat::eBC7_SRGB_BLOCK >
	{
		static const uint8_t Size = 16u; // Bit count for a 4x4 pixels block
		static const uint8_t Count = 4;
		static const bool Alpha = true;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = true;
		static const PixelFormat NoAlphaPF = PixelFormat::eBC7_SRGB_BLOCK;
		static String toString()
		{
			return cuT( "BC7 16 bits compressed format" );
		}
		static String toStr()
		{
			return cuT( "bc7_srgb" );
		}
		static void convert( uint8_t const * srcBuffer, uint8_t * dstBuffer, PixelFormat dstFormat )
		{
			if ( dstFormat == PixelFormat::eBC7_SRGB_BLOCK )
			{
				details::PixelConverter< PixelFormat::eBC7_SRGB_BLOCK, PixelFormat::eBC7_SRGB_BLOCK >()( srcBuffer, dstBuffer );
			}
		}
		static void convert( PxBufferConvertOptions const * options, castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			if ( dstFormat == PixelFormat::eBC7_SRGB_BLOCK )
			{
				details::BufferConverter< PixelFormat::eBC7_SRGB_BLOCK, PixelFormat::eBC7_SRGB_BLOCK >()( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
			}
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eBC7_SRGB_BLOCK, PF >()( srcBuffer, dstBuffer );
		}
	};

	//*************************************************************************************************

	//!\~english	Specialisation for PixelFormat::eD16_UNORM
	//!\~french		Spécialisation pour PixelFormat::eD16_UNORM
	template<>
	struct PixelDefinitions< PixelFormat::eD16_UNORM >
	{
		static const uint8_t Size = 2;
		static const uint8_t Count = 1;
		static const bool Alpha = false;
		static const bool Colour = false;
		static const bool Depth = true;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static String toString()
		{
			return cuT( "16 bits depth" );
		}
		static String toStr()
		{
			return cuT( "depth16" );
		}
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			details::dynamicDepthConversion< PixelFormat::eD16_UNORM >( srcBuffer, dstBuffer, dstFormat );
		}
		static void convert( PxBufferConvertOptions const * options, castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			details::dynamicDepthBufferConversion< PixelFormat::eD16_UNORM >( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eD16_UNORM, PF >()( srcBuffer, dstBuffer );
		}
	};

	//*************************************************************************************************

	//!\~english	Specialisation for PixelFormat::eD24_UNORM_S8_UINT
	//!\~french		Spécialisation pour PixelFormat::eD24_UNORM_S8_UINT
	template<>
	struct PixelDefinitions< PixelFormat::eD24_UNORM_S8_UINT >
	{
		static const uint8_t Size = 4;
		static const uint8_t Count = 2;
		static const bool Alpha = false;
		static const bool Colour = false;
		static const bool Depth = true;
		static const bool Stencil = true;
		static const bool Compressed = false;
		static String toString()
		{
			return cuT( "24 bits depth, 8 bits stencil" );
		}
		static String toStr()
		{
			return cuT( "depth24s8" );
		}
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			details::dynamicDepthConversion< PixelFormat::eD24_UNORM_S8_UINT >( srcBuffer, dstBuffer, dstFormat );
		}
		static void convert( PxBufferConvertOptions const * options, castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			details::dynamicDepthBufferConversion< PixelFormat::eD24_UNORM_S8_UINT >( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter<PixelFormat::eD24_UNORM_S8_UINT, PF>()( srcBuffer, dstBuffer );
		}
	};

	//*************************************************************************************************

	//!\~english	Specialisation for PixelFormat::eD32_UNORM
	//!\~french		Spécialisation pour PixelFormat::eD32_UNORM
	template<>
	struct PixelDefinitions< PixelFormat::eD32_UNORM >
	{
		static const uint8_t Size = 4;
		static const uint8_t Count = 1;
		static const bool Alpha = false;
		static const bool Colour = false;
		static const bool Depth = true;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static String toString()
		{
			return cuT( "32 bits depth" );
		}
		static String toStr()
		{
			return cuT( "depth32" );
		}
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			details::dynamicDepthConversion< PixelFormat::eD32_UNORM >( srcBuffer, dstBuffer, dstFormat );
		}
		static void convert( PxBufferConvertOptions const * options, castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			details::dynamicDepthBufferConversion< PixelFormat::eD32_UNORM >( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eD32_UNORM, PF >()( srcBuffer, dstBuffer );
		}
	};

	//*************************************************************************************************

	//!\~english	Specialisation for PixelFormat::eD32_SFLOAT
	//!\~french		Spécialisation pour PixelFormat::eD32_SFLOAT
	template<>
	struct PixelDefinitions< PixelFormat::eD32_SFLOAT >
	{
		static const uint8_t Size = 4;
		static const uint8_t Count = 1;
		static const bool Alpha = false;
		static const bool Colour = false;
		static const bool Depth = true;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static String toString()
		{
			return cuT( "32 bits floating point depth" );
		}
		static String toStr()
		{
			return cuT( "depth32f" );
		}
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			details::dynamicDepthConversion< PixelFormat::eD32_SFLOAT >( srcBuffer, dstBuffer, dstFormat );
		}
		static void convert( PxBufferConvertOptions const * options, castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			details::dynamicDepthBufferConversion< PixelFormat::eD32_SFLOAT >( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eD32_SFLOAT, PF >()( srcBuffer, dstBuffer );
		}
	};

	//*************************************************************************************************

	//!\~english	Specialisation for PixelFormat::eD32_SFLOAT_S8_UINT
	//!\~french		Spécialisation pour PixelFormat::eD32_SFLOAT_S8_UINT
	template<>
	struct PixelDefinitions< PixelFormat::eD32_SFLOAT_S8_UINT >
	{
		static const uint8_t Size = 4;
		static const uint8_t Count = 2;
		static const bool Alpha = false;
		static const bool Colour = false;
		static const bool Depth = true;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static String toString()
		{
			return cuT( "32 bits floating point depth, 8 bits stencil" );
		}
		static String toStr()
		{
			return cuT( "depth32fs8" );
		}
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			details::dynamicDepthConversion< PixelFormat::eD32_SFLOAT_S8_UINT >( srcBuffer, dstBuffer, dstFormat );
		}
		static void convert( PxBufferConvertOptions const * options, castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			details::dynamicDepthBufferConversion< PixelFormat::eD32_SFLOAT_S8_UINT >( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eD32_SFLOAT_S8_UINT, PF >()( srcBuffer, dstBuffer );
		}
	};

	//*************************************************************************************************

	//!\~english	Specialisation for PixelFormat::eS8_UINT
	//!\~french		Spécialisation pour PixelFormat::eS8_UINT
	template<>
	struct PixelDefinitions< PixelFormat::eS8_UINT >
	{
		static const uint8_t Size = 1;
		static const uint8_t Count = 1;
		static const bool Alpha = false;
		static const bool Colour = false;
		static const bool Depth = false;
		static const bool Stencil = true;
		static const bool Compressed = false;
		static String toString()
		{
			return cuT( "8 bits stencil" );
		}
		static String toStr()
		{
			return cuT( "stencil8" );
		}
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			details::dynamicStencilConversion< PixelFormat::eS8_UINT >( srcBuffer, dstBuffer, dstFormat );
		}
		static void convert( PxBufferConvertOptions const * options, castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			details::dynamicStencilBufferConversion< PixelFormat::eS8_UINT >( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eS8_UINT, PF >()( srcBuffer, dstBuffer );
		}
	};

	//*************************************************************************************************
}
