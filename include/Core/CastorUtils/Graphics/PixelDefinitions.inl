#include "CastorUtils/Graphics/PixelComponents.hpp"
#include "CastorUtils/Graphics/UnsupportedFormatException.hpp"
#include "CastorUtils/Miscellaneous/StringUtils.hpp"
#include "CastorUtils/Exception/Assertion.hpp"

#include <ashes/common/Format.hpp>

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
					std::memcpy( dstBuffer, srcBuffer, getBytesPerPixel( PFSrc ) );
				}
				else if constexpr( PFSrc != PixelFormat::eUNDEFINED
					&& PFDst != PixelFormat::eUNDEFINED
					&& !isCompressedV< PFSrc >
					&& !isCompressedV< PFDst > )
				{
					if constexpr ( PFSrc == PFDst )
					{
						std::memcpy( dstBuffer, srcBuffer, getBytesPerPixel( PFSrc ) );
					}
					else if constexpr ( is8UComponentsV< PFDst > )
					{
						PixelComponentsT< PFDst >::R( dstBuffer, getR8U< PFSrc >( srcBuffer ) );
						PixelComponentsT< PFDst >::G( dstBuffer, getG8U< PFSrc >( srcBuffer ) );
						PixelComponentsT< PFDst >::B( dstBuffer, getB8U< PFSrc >( srcBuffer ) );
						PixelComponentsT< PFDst >::A( dstBuffer, getA8U< PFSrc >( srcBuffer ) );
					}
					else if constexpr ( is8SComponentsV< PFDst > )
					{
						PixelComponentsT< PFDst >::R( dstBuffer, getR8S< PFSrc >( srcBuffer ) );
						PixelComponentsT< PFDst >::G( dstBuffer, getG8S< PFSrc >( srcBuffer ) );
						PixelComponentsT< PFDst >::B( dstBuffer, getB8S< PFSrc >( srcBuffer ) );
						PixelComponentsT< PFDst >::A( dstBuffer, getA8S< PFSrc >( srcBuffer ) );
					}
					else if constexpr ( is16UComponentsV< PFDst > )
					{
						PixelComponentsT< PFDst >::R( dstBuffer, getR16U< PFSrc >( srcBuffer ) );
						PixelComponentsT< PFDst >::G( dstBuffer, getG16U< PFSrc >( srcBuffer ) );
						PixelComponentsT< PFDst >::B( dstBuffer, getB16U< PFSrc >( srcBuffer ) );
						PixelComponentsT< PFDst >::A( dstBuffer, getA16U< PFSrc >( srcBuffer ) );
					}
					else if constexpr ( is16SComponentsV< PFDst > )
					{
						PixelComponentsT< PFDst >::R( dstBuffer, getR16S< PFSrc >( srcBuffer ) );
						PixelComponentsT< PFDst >::G( dstBuffer, getG16S< PFSrc >( srcBuffer ) );
						PixelComponentsT< PFDst >::B( dstBuffer, getB16S< PFSrc >( srcBuffer ) );
						PixelComponentsT< PFDst >::A( dstBuffer, getA16S< PFSrc >( srcBuffer ) );
					}
					else if constexpr ( is16FComponentsV< PFDst > )
					{
						PixelComponentsT< PFDst >::R( dstBuffer, getR16F< PFSrc >( srcBuffer ) );
						PixelComponentsT< PFDst >::G( dstBuffer, getG16F< PFSrc >( srcBuffer ) );
						PixelComponentsT< PFDst >::B( dstBuffer, getB16F< PFSrc >( srcBuffer ) );
						PixelComponentsT< PFDst >::A( dstBuffer, getA16F< PFSrc >( srcBuffer ) );
					}
					else if constexpr ( is32UComponentsV< PFDst > )
					{
						PixelComponentsT< PFDst >::R( dstBuffer, getR32U< PFSrc >( srcBuffer ) );
						PixelComponentsT< PFDst >::G( dstBuffer, getG32U< PFSrc >( srcBuffer ) );
						PixelComponentsT< PFDst >::B( dstBuffer, getB32U< PFSrc >( srcBuffer ) );
						PixelComponentsT< PFDst >::A( dstBuffer, getA32U< PFSrc >( srcBuffer ) );
					}
					else if constexpr ( is32SComponentsV< PFDst > )
					{
						PixelComponentsT< PFDst >::R( dstBuffer, getR32S< PFSrc >( srcBuffer ) );
						PixelComponentsT< PFDst >::G( dstBuffer, getG32S< PFSrc >( srcBuffer ) );
						PixelComponentsT< PFDst >::B( dstBuffer, getB32S< PFSrc >( srcBuffer ) );
						PixelComponentsT< PFDst >::A( dstBuffer, getA32S< PFSrc >( srcBuffer ) );
					}
					else if constexpr ( is32FComponentsV< PFDst > )
					{
						PixelComponentsT< PFDst >::R( dstBuffer, getR32F< PFSrc >( srcBuffer ) );
						PixelComponentsT< PFDst >::G( dstBuffer, getG32F< PFSrc >( srcBuffer ) );
						PixelComponentsT< PFDst >::B( dstBuffer, getB32F< PFSrc >( srcBuffer ) );
						PixelComponentsT< PFDst >::A( dstBuffer, getA32F< PFSrc >( srcBuffer ) );
					}
					else if constexpr ( is64UComponentsV< PFDst > )
					{
						PixelComponentsT< PFDst >::R( dstBuffer, getR64U< PFSrc >( srcBuffer ) );
						PixelComponentsT< PFDst >::G( dstBuffer, getG64U< PFSrc >( srcBuffer ) );
						PixelComponentsT< PFDst >::B( dstBuffer, getB64U< PFSrc >( srcBuffer ) );
						PixelComponentsT< PFDst >::A( dstBuffer, getA64U< PFSrc >( srcBuffer ) );
					}
					else if constexpr ( is64SComponentsV< PFDst > )
					{
						PixelComponentsT< PFDst >::R( dstBuffer, getR64S< PFSrc >( srcBuffer ) );
						PixelComponentsT< PFDst >::G( dstBuffer, getG64S< PFSrc >( srcBuffer ) );
						PixelComponentsT< PFDst >::B( dstBuffer, getB64S< PFSrc >( srcBuffer ) );
						PixelComponentsT< PFDst >::A( dstBuffer, getA64S< PFSrc >( srcBuffer ) );
					}
					else if constexpr ( is64FComponentsV< PFDst > )
					{
						PixelComponentsT< PFDst >::R( dstBuffer, getR64F< PFSrc >( srcBuffer ) );
						PixelComponentsT< PFDst >::G( dstBuffer, getG64F< PFSrc >( srcBuffer ) );
						PixelComponentsT< PFDst >::B( dstBuffer, getB64F< PFSrc >( srcBuffer ) );
						PixelComponentsT< PFDst >::A( dstBuffer, getA64F< PFSrc >( srcBuffer ) );
					}
					else if constexpr ( isD16UComponentV< PFDst > )
					{
						PixelComponentsT< PFDst >::D( dstBuffer, getD16U< PFSrc >( srcBuffer ) );
						PixelComponentsT< PFDst >::S( dstBuffer, getS8U< PFSrc >( srcBuffer ) );
					}
					else if constexpr ( isD24UComponentV< PFDst > )
					{
						PixelComponentsT< PFDst >::D( dstBuffer, getD24U< PFSrc >( srcBuffer ) );
						PixelComponentsT< PFDst >::S( dstBuffer, getS8U< PFSrc >( srcBuffer ) );
					}
					else if constexpr ( isD32FComponentV< PFDst > )
					{
						PixelComponentsT< PFDst >::D( dstBuffer, getD32F< PFSrc >( srcBuffer ) );
						PixelComponentsT< PFDst >::S( dstBuffer, getS8U< PFSrc >( srcBuffer ) );
					}
					else if constexpr ( isS8UComponentV< PFDst > )
					{
						PixelComponentsT< PFDst >::S( dstBuffer, getS8U< PFSrc >( srcBuffer ) );
					}
				}

				srcBuffer += getBytesPerPixel( PFSrc );
				dstBuffer += getBytesPerPixel( PFDst );
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
			void operator()( PxBufferConvertOptions const * CU_UnusedParam( options )
				, Size const & srcDimensions
				, Size const & dstDimensions
				, uint8_t const * srcBuffer
				, uint32_t srcSize
				, uint8_t * dstBuffer
				, uint32_t dstSize )
			{
				uint8_t const * src = &srcBuffer[0];
				uint8_t * dst = &dstBuffer[0];
				uint32_t count = uint32_t( srcSize / getBytesPerPixel( PFSrc ) );
				CU_Require( srcSize / getBytesPerPixel( PFSrc ) == dstSize / getBytesPerPixel( PFDst ) );

				if ( srcDimensions == dstDimensions
					&& PFSrc == PFDst )
				{
					CU_Require( srcSize == dstSize );
					std::memcpy( dstBuffer, srcBuffer, srcSize );
				}
				else
				{
					PixelConverter< PFSrc, PFDst > converter;

					for ( uint32_t i = 0; i < count; i++ )
					{
						converter( src, dst );
					}
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
			case PixelFormat::eR8G8_UNORM:
				PixelConverter< PF, PixelFormat::eR8G8_UNORM >()( srcBuffer, dstBuffer );
				break;
			case PixelFormat::eR16G16_SFLOAT:
				PixelConverter< PF, PixelFormat::eR16G16_SFLOAT >()( srcBuffer, dstBuffer );
				break;
			case PixelFormat::eR32G32_SFLOAT:
				PixelConverter< PF, PixelFormat::eR32G32_SFLOAT >()( srcBuffer, dstBuffer );
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
			case PixelFormat::eX8_D24_UNORM:
				PixelConverter< PF, PixelFormat::eX8_D24_UNORM >()( srcBuffer, dstBuffer );
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
			case PixelFormat::eR8G8_UNORM:
				BufferConverter< PF, PixelFormat::eR8G8_UNORM >()( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;
			case PixelFormat::eR16G16_SFLOAT:
				BufferConverter< PF, PixelFormat::eR16G16_SFLOAT >()( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;
			case PixelFormat::eR32G32_SFLOAT:
				BufferConverter< PF, PixelFormat::eR32G32_SFLOAT >()( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
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
			case PixelFormat::eX8_D24_UNORM:
				BufferConverter< PF, PixelFormat::eX8_D24_UNORM >()( options, srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
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

	template< PixelFormat PFT >
	struct PixelDefinitionsT
	{
		static constexpr VkDeviceSize Size = ashes::getMinimalSize( VkFormat( PFT ) );
		static constexpr uint8_t Count = uint8_t( ashes::getCount( VkFormat( PFT ) ) );
		static constexpr bool Alpha = hasAlphaV< PFT >;
		static constexpr bool Colour = isColourFormatV< PFT >;
		static constexpr bool Depth = isDepthFormatV< PFT > || isDepthStencilFormatV< PFT >;
		static constexpr bool Stencil = isStencilFormatV< PFT > || isDepthStencilFormatV< PFT >;
		static constexpr bool Compressed = isCompressedV< PFT >;
		static String const & toStr()
		{
			static String const result = getFormatName( PFT );
			return result;
		}
		static void convert( uint8_t const *& srcBuffer
			, uint8_t *& dstBuffer
			, PixelFormat dstFormat )
		{
			if constexpr ( Depth )
			{
				details::dynamicDepthConversion< PFT >( srcBuffer
					, dstBuffer
					, dstFormat );
			}
			else if constexpr ( Stencil )
			{
				details::dynamicStencilConversion< PFT >( srcBuffer
					, dstBuffer
					, dstFormat );
			}
			else if constexpr ( Compressed )
			{
				if ( dstFormat == PFT )
				{
					details::dynamicColourConversion< PFT >( srcBuffer
						, dstBuffer
						, dstFormat );
				}
			}
			else
			{
				details::dynamicColourConversion< PFT >( srcBuffer
					, dstBuffer
					, dstFormat );
			}
		}
		static void convert( PxBufferConvertOptions const * options
			, castor::Size const & srcDimensions
			, castor::Size const & dstDimensions
			, uint8_t const *& srcBuffer
			, uint32_t srcSize
			, PixelFormat dstFormat
			, uint8_t *& dstBuffer
			, uint32_t dstSize )
		{
			if constexpr ( Depth )
			{
				details::dynamicDepthBufferConversion< PFT >( options
					, srcDimensions
					, dstDimensions
					, srcBuffer
					, srcSize
					, dstFormat
					, dstBuffer
					, dstSize );
			}
			else if constexpr ( Stencil )
			{
				details::dynamicStencilBufferConversion< PFT >( options
					, srcDimensions
					, dstDimensions
					, srcBuffer
					, srcSize
					, dstFormat
					, dstBuffer
					, dstSize );
			}
			else if constexpr ( Compressed )
			{
				if ( dstFormat == PFT )
				{
					details::dynamicColourBufferConversion< PFT >( options
						, srcDimensions
						, dstDimensions
						, srcBuffer
						, srcSize
						, dstFormat
						, dstBuffer
						, dstSize );
				}
			}
			else
			{
				details::dynamicColourBufferConversion< PFT >( options
					, srcDimensions
					, dstDimensions
					, srcBuffer
					, srcSize
					, dstFormat
					, dstBuffer
					, dstSize );
			}
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer
			, uint8_t *& dstBuffer )
		{
			if constexpr ( Compressed )
			{
				if constexpr ( PF == PFT )
				{
					details::PixelConverter< PFT, PF >()( srcBuffer
						, dstBuffer );
				}
			}
			
			else
			{
				details::PixelConverter< PFT, PF >()( srcBuffer
					, dstBuffer );
			}
		}
	};

	//*************************************************************************************************
}
