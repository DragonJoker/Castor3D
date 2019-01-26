#include "PixelFormat.hpp"
#include "PixelBuffer.hpp"

namespace castor
{
	namespace PF
	{
		PixelFormat getPFWithoutAlpha( PixelFormat p_format )
		{
			PixelFormat result = PixelFormat::eCount;

			switch ( p_format )
			{
			case PixelFormat::eR8A8_UNORM:
				result = PixelDefinitions< PixelFormat::eR8A8_UNORM >::NoAlphaPF;
				break;

			case PixelFormat::eR16A16_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR16A16_SFLOAT >::NoAlphaPF;
				break;

			case PixelFormat::eR32A32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR32A32_SFLOAT >::NoAlphaPF;
				break;

			case PixelFormat::eR5G5B5A1_UNORM:
				result = PixelDefinitions< PixelFormat::eR5G5B5A1_UNORM >::NoAlphaPF;
				break;

			case PixelFormat::eR8G8B8A8_UNORM:
				result = PixelDefinitions< PixelFormat::eR8G8B8A8_UNORM >::NoAlphaPF;
				break;

			case PixelFormat::eA8B8G8R8_UNORM:
				result = PixelDefinitions< PixelFormat::eA8B8G8R8_UNORM >::NoAlphaPF;
				break;

			case PixelFormat::eR8G8B8A8_SRGB:
				result = PixelDefinitions< PixelFormat::eR8G8B8A8_UNORM >::NoAlphaPF;
				break;

			case PixelFormat::eA8B8G8R8_SRGB:
				result = PixelDefinitions< PixelFormat::eA8B8G8R8_UNORM >::NoAlphaPF;
				break;

			case PixelFormat::eR32G32B32A32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR32G32B32A32_SFLOAT >::NoAlphaPF;
				break;

			case PixelFormat::eBC3_UNORM_BLOCK:
				result = PixelDefinitions< PixelFormat::eBC3_UNORM_BLOCK >::NoAlphaPF;
				break;

			case PixelFormat::eBC5_UNORM_BLOCK:
				result = PixelDefinitions< PixelFormat::eBC5_UNORM_BLOCK >::NoAlphaPF;
				break;

			default:
				result = p_format;
				break;
			}

			return result;
		}

		bool hasAlpha( PixelFormat p_format )
		{
			bool result = false;

			switch ( p_format )
			{
			case PixelFormat::eR8_UNORM:
				result = PixelDefinitions< PixelFormat::eR8_UNORM >::Alpha;
				break;

			case PixelFormat::eR16_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR16_SFLOAT >::Alpha;
				break;

			case PixelFormat::eR32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR32_SFLOAT >::Alpha;
				break;

			case PixelFormat::eR8A8_UNORM:
				result = PixelDefinitions< PixelFormat::eR8A8_UNORM >::Alpha;
				break;

			case PixelFormat::eR32A32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR32A32_SFLOAT >::Alpha;
				break;

			case PixelFormat::eR5G5B5A1_UNORM:
				result = PixelDefinitions< PixelFormat::eR5G5B5A1_UNORM >::Alpha;
				break;

			case PixelFormat::eR5G6B5_UNORM:
				result = PixelDefinitions< PixelFormat::eR5G6B5_UNORM >::Alpha;
				break;

			case PixelFormat::eR8G8B8_UNORM:
				result = PixelDefinitions< PixelFormat::eR8G8B8_UNORM >::Alpha;
				break;

			case PixelFormat::eB8G8R8_UNORM:
				result = PixelDefinitions< PixelFormat::eB8G8R8_UNORM >::Alpha;
				break;

			case PixelFormat::eR8G8B8_SRGB:
				result = PixelDefinitions< PixelFormat::eR8G8B8_SRGB >::Alpha;
				break;

			case PixelFormat::eB8G8R8_SRGB:
				result = PixelDefinitions< PixelFormat::eB8G8R8_SRGB >::Alpha;
				break;

			case PixelFormat::eR8G8B8A8_UNORM:
				result = PixelDefinitions< PixelFormat::eR8G8B8A8_UNORM >::Alpha;
				break;

			case PixelFormat::eA8B8G8R8_UNORM:
				result = PixelDefinitions< PixelFormat::eA8B8G8R8_UNORM >::Alpha;
				break;

			case PixelFormat::eR8G8B8A8_SRGB:
				result = PixelDefinitions< PixelFormat::eR8G8B8A8_SRGB >::Alpha;
				break;

			case PixelFormat::eA8B8G8R8_SRGB:
				result = PixelDefinitions< PixelFormat::eA8B8G8R8_SRGB >::Alpha;
				break;

			case PixelFormat::eR16G16B16_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR16G16B16_SFLOAT >::Alpha;
				break;

			case PixelFormat::eR16G16B16A16_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR16G16B16A16_SFLOAT >::Alpha;
				break;

			case PixelFormat::eR32G32B32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR32G32B32_SFLOAT >::Alpha;
				break;

			case PixelFormat::eR32G32B32A32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR32G32B32A32_SFLOAT >::Alpha;
				break;

			case PixelFormat::eBC1_RGB_UNORM_BLOCK:
				result = PixelDefinitions< PixelFormat::eBC1_RGB_UNORM_BLOCK >::Alpha;
				break;

			case PixelFormat::eBC3_UNORM_BLOCK:
				result = PixelDefinitions< PixelFormat::eBC3_UNORM_BLOCK >::Alpha;
				break;

			case PixelFormat::eBC5_UNORM_BLOCK:
				result = PixelDefinitions< PixelFormat::eBC5_UNORM_BLOCK >::Alpha;
				break;

			case PixelFormat::eD16_UNORM:
				result = PixelDefinitions< PixelFormat::eD16_UNORM >::Alpha;
				break;

			case PixelFormat::eD24_UNORM_S8_UINT:
				result = PixelDefinitions< PixelFormat::eD24_UNORM_S8_UINT >::Alpha;
				break;

			case PixelFormat::eD32_UNORM:
				result = PixelDefinitions< PixelFormat::eD32_UNORM >::Alpha;
				break;

			case PixelFormat::eD32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eD32_SFLOAT >::Alpha;
				break;

			case PixelFormat::eD32_SFLOAT_S8_UINT:
				result = PixelDefinitions< PixelFormat::eD32_SFLOAT_S8_UINT >::Alpha;
				break;

			case PixelFormat::eS8_UINT:
				result = PixelDefinitions< PixelFormat::eS8_UINT >::Alpha;
				break;

			default:
				result = false;
				break;
			}

			return result;
		}

		bool isCompressed( PixelFormat p_format )
		{
			bool result = false;

			switch ( p_format )
			{
			case PixelFormat::eR8_UNORM:
				result = PixelDefinitions< PixelFormat::eR8_UNORM >::Compressed;
				break;

			case PixelFormat::eR16_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR16_SFLOAT >::Compressed;
				break;

			case PixelFormat::eR32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR32_SFLOAT >::Compressed;
				break;

			case PixelFormat::eR8A8_UNORM:
				result = PixelDefinitions< PixelFormat::eR8A8_UNORM >::Compressed;
				break;

			case PixelFormat::eR32A32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR32A32_SFLOAT >::Compressed;
				break;

			case PixelFormat::eR5G5B5A1_UNORM:
				result = PixelDefinitions< PixelFormat::eR5G5B5A1_UNORM >::Compressed;
				break;

			case PixelFormat::eR5G6B5_UNORM:
				result = PixelDefinitions< PixelFormat::eR5G6B5_UNORM >::Compressed;
				break;

			case PixelFormat::eR8G8B8_UNORM:
				result = PixelDefinitions< PixelFormat::eR8G8B8_UNORM >::Compressed;
				break;

			case PixelFormat::eB8G8R8_UNORM:
				result = PixelDefinitions< PixelFormat::eB8G8R8_UNORM >::Compressed;
				break;

			case PixelFormat::eR8G8B8_SRGB:
				result = PixelDefinitions< PixelFormat::eR8G8B8_SRGB >::Compressed;
				break;

			case PixelFormat::eB8G8R8_SRGB:
				result = PixelDefinitions< PixelFormat::eB8G8R8_SRGB >::Compressed;
				break;

			case PixelFormat::eR8G8B8A8_UNORM:
				result = PixelDefinitions< PixelFormat::eR8G8B8A8_UNORM >::Compressed;
				break;

			case PixelFormat::eA8B8G8R8_UNORM:
				result = PixelDefinitions< PixelFormat::eA8B8G8R8_UNORM >::Compressed;
				break;

			case PixelFormat::eR8G8B8A8_SRGB:
				result = PixelDefinitions< PixelFormat::eR8G8B8A8_SRGB >::Compressed;
				break;

			case PixelFormat::eA8B8G8R8_SRGB:
				result = PixelDefinitions< PixelFormat::eA8B8G8R8_SRGB >::Compressed;
				break;

			case PixelFormat::eR16G16B16_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR16G16B16_SFLOAT >::Compressed;
				break;

			case PixelFormat::eR16G16B16A16_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR16G16B16A16_SFLOAT >::Compressed;
				break;

			case PixelFormat::eR32G32B32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR32G32B32_SFLOAT >::Compressed;
				break;

			case PixelFormat::eR32G32B32A32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR32G32B32A32_SFLOAT >::Compressed;
				break;

			case PixelFormat::eBC1_RGB_UNORM_BLOCK:
				result = PixelDefinitions< PixelFormat::eBC1_RGB_UNORM_BLOCK >::Compressed;
				break;

			case PixelFormat::eBC3_UNORM_BLOCK:
				result = PixelDefinitions< PixelFormat::eBC3_UNORM_BLOCK >::Compressed;
				break;

			case PixelFormat::eBC5_UNORM_BLOCK:
				result = PixelDefinitions< PixelFormat::eBC5_UNORM_BLOCK >::Compressed;
				break;

			case PixelFormat::eD16_UNORM:
				result = PixelDefinitions< PixelFormat::eD16_UNORM >::Compressed;
				break;

			case PixelFormat::eD24_UNORM_S8_UINT:
				result = PixelDefinitions< PixelFormat::eD24_UNORM_S8_UINT >::Compressed;
				break;

			case PixelFormat::eD32_UNORM:
				result = PixelDefinitions< PixelFormat::eD32_UNORM >::Compressed;
				break;

			case PixelFormat::eD32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eD32_SFLOAT >::Compressed;
				break;

			case PixelFormat::eD32_SFLOAT_S8_UINT:
				result = PixelDefinitions< PixelFormat::eD32_SFLOAT_S8_UINT >::Compressed;
				break;

			case PixelFormat::eS8_UINT:
				result = PixelDefinitions< PixelFormat::eS8_UINT >::Compressed;
				break;

			default:
				result = false;
				break;
			}

			return result;
		}

		PxBufferBaseSPtr extractAlpha( PxBufferBaseSPtr & p_pSrc )
		{
			PxBufferBaseSPtr result;

			if ( hasAlpha( p_pSrc->format() ) )
			{
				result = PxBufferBase::create( p_pSrc->dimensions(), PixelFormat::eR8A8_UNORM, p_pSrc->constPtr(), p_pSrc->format() );
				uint8_t * pBuffer = result->ptr();

				for ( uint32_t i = 0; i < result->size(); i += 2 )
				{
					pBuffer[0] = pBuffer[1];
					pBuffer++;
					pBuffer++;
				}

				result = PxBufferBase::create( p_pSrc->dimensions(), PixelFormat::eR8_UNORM, result->constPtr(), result->format() );
				p_pSrc = PxBufferBase::create( p_pSrc->dimensions(), getPFWithoutAlpha( p_pSrc->format() ), p_pSrc->constPtr(), p_pSrc->format() );
			}

			return result;
		}

		void reduceToAlpha( PxBufferBaseSPtr & p_pSrc )
		{
			if ( hasAlpha( p_pSrc->format() ) )
			{
				if ( p_pSrc->format() != PixelFormat::eR8A8_UNORM )
				{
					p_pSrc = PxBufferBase::create( p_pSrc->dimensions(), PixelFormat::eR8A8_UNORM, p_pSrc->constPtr(), p_pSrc->format() );
				}

				uint8_t * pBuffer = p_pSrc->ptr();

				for ( uint32_t i = 0; i < p_pSrc->size(); i += 2 )
				{
					pBuffer[0] = pBuffer[1];
					pBuffer++;
					pBuffer++;
				}
			}

			p_pSrc = PxBufferBase::create( p_pSrc->dimensions(), PixelFormat::eR8_UNORM, p_pSrc->constPtr(), p_pSrc->format() );
		}

		uint8_t getBytesPerPixel( PixelFormat p_format )
		{
			uint8_t result = 0;

			switch ( p_format )
			{
			case PixelFormat::eR8_UNORM:
				result = PixelDefinitions< PixelFormat::eR8_UNORM >::Size;
				break;

			case PixelFormat::eR16_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR16_SFLOAT >::Size;
				break;

			case PixelFormat::eR32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR32_SFLOAT >::Size;
				break;

			case PixelFormat::eR8A8_UNORM:
				result = PixelDefinitions< PixelFormat::eR8A8_UNORM >::Size;
				break;

			case PixelFormat::eR32A32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR32A32_SFLOAT >::Size;
				break;

			case PixelFormat::eR5G5B5A1_UNORM:
				result = PixelDefinitions< PixelFormat::eR5G5B5A1_UNORM >::Size;
				break;

			case PixelFormat::eR5G6B5_UNORM:
				result = PixelDefinitions< PixelFormat::eR5G6B5_UNORM >::Size;
				break;

			case PixelFormat::eR8G8B8_UNORM:
				result = PixelDefinitions< PixelFormat::eR8G8B8_UNORM >::Size;
				break;

			case PixelFormat::eB8G8R8_UNORM:
				result = PixelDefinitions< PixelFormat::eB8G8R8_UNORM >::Size;
				break;

			case PixelFormat::eR8G8B8_SRGB:
				result = PixelDefinitions< PixelFormat::eR8G8B8_SRGB >::Size;
				break;

			case PixelFormat::eB8G8R8_SRGB:
				result = PixelDefinitions< PixelFormat::eB8G8R8_SRGB >::Size;
				break;

			case PixelFormat::eR8G8B8A8_UNORM:
				result = PixelDefinitions< PixelFormat::eR8G8B8A8_UNORM >::Size;
				break;

			case PixelFormat::eA8B8G8R8_UNORM:
				result = PixelDefinitions< PixelFormat::eA8B8G8R8_UNORM >::Size;
				break;

			case PixelFormat::eR8G8B8A8_SRGB:
				result = PixelDefinitions< PixelFormat::eR8G8B8A8_SRGB >::Size;
				break;

			case PixelFormat::eA8B8G8R8_SRGB:
				result = PixelDefinitions< PixelFormat::eA8B8G8R8_SRGB >::Size;
				break;

			case PixelFormat::eR16G16B16_SFLOAT:
				result = PixelDefinitions<	PixelFormat::eR16G16B16_SFLOAT >::Size;
				break;

			case PixelFormat::eR16G16B16A16_SFLOAT:
				result = PixelDefinitions<	PixelFormat::eR16G16B16A16_SFLOAT >::Size;
				break;

			case PixelFormat::eR32G32B32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR32G32B32_SFLOAT >::Size;
				break;

			case PixelFormat::eR32G32B32A32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR32G32B32A32_SFLOAT >::Size;
				break;

			case PixelFormat::eBC1_RGB_UNORM_BLOCK:
				result = PixelDefinitions<	PixelFormat::eBC1_RGB_UNORM_BLOCK >::Size;
				break;

			case PixelFormat::eBC3_UNORM_BLOCK:
				result = PixelDefinitions<	PixelFormat::eBC3_UNORM_BLOCK >::Size;
				break;

			case PixelFormat::eBC5_UNORM_BLOCK:
				result = PixelDefinitions<	PixelFormat::eBC5_UNORM_BLOCK >::Size;
				break;

			case PixelFormat::eD16_UNORM:
				result = PixelDefinitions< PixelFormat::eD16_UNORM >::Size;
				break;

			case PixelFormat::eD24_UNORM_S8_UINT:
				result = PixelDefinitions< PixelFormat::eD24_UNORM_S8_UINT >::Size;
				break;

			case PixelFormat::eD32_UNORM:
				result = PixelDefinitions< PixelFormat::eD32_UNORM >::Size;
				break;

			case PixelFormat::eD32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eD32_SFLOAT >::Size;
				break;

			case PixelFormat::eD32_SFLOAT_S8_UINT:
				result = PixelDefinitions< PixelFormat::eD32_SFLOAT_S8_UINT >::Size;
				break;

			case PixelFormat::eS8_UINT:
				result = PixelDefinitions< PixelFormat::eS8_UINT >::Size;
				break;

			default:
				CU_Failure( "Unsupported pixel format" );
				break;
			}

			return result;
		}

		uint8_t getComponentsCount( PixelFormat p_format )
		{
			uint8_t result = 0;

			switch ( p_format )
			{
			case PixelFormat::eR8_UNORM:
				result = PixelDefinitions< PixelFormat::eR8_UNORM >::Count;
				break;

			case PixelFormat::eR32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR32_SFLOAT >::Count;
				break;

			case PixelFormat::eR16_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR16_SFLOAT >::Count;
				break;

			case PixelFormat::eR8A8_UNORM:
				result = PixelDefinitions< PixelFormat::eR8A8_UNORM >::Count;
				break;

			case PixelFormat::eR32A32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR32A32_SFLOAT >::Count;
				break;

			case PixelFormat::eR5G5B5A1_UNORM:
				result = PixelDefinitions< PixelFormat::eR5G5B5A1_UNORM >::Count;
				break;

			case PixelFormat::eR5G6B5_UNORM:
				result = PixelDefinitions< PixelFormat::eR5G6B5_UNORM >::Count;
				break;

			case PixelFormat::eR8G8B8_UNORM:
				result = PixelDefinitions< PixelFormat::eR8G8B8_UNORM >::Count;
				break;

			case PixelFormat::eB8G8R8_UNORM:
				result = PixelDefinitions< PixelFormat::eB8G8R8_UNORM >::Count;
				break;

			case PixelFormat::eR8G8B8_SRGB:
				result = PixelDefinitions< PixelFormat::eR8G8B8_SRGB >::Count;
				break;

			case PixelFormat::eB8G8R8_SRGB:
				result = PixelDefinitions< PixelFormat::eB8G8R8_SRGB >::Count;
				break;

			case PixelFormat::eR8G8B8A8_UNORM:
				result = PixelDefinitions< PixelFormat::eR8G8B8A8_UNORM >::Count;
				break;

			case PixelFormat::eA8B8G8R8_UNORM:
				result = PixelDefinitions< PixelFormat::eA8B8G8R8_UNORM >::Count;
				break;

			case PixelFormat::eR8G8B8A8_SRGB:
				result = PixelDefinitions< PixelFormat::eR8G8B8A8_SRGB >::Count;
				break;

			case PixelFormat::eA8B8G8R8_SRGB:
				result = PixelDefinitions< PixelFormat::eA8B8G8R8_SRGB >::Count;
				break;

			case PixelFormat::eR16G16B16_SFLOAT:
				result = PixelDefinitions<	PixelFormat::eR16G16B16_SFLOAT >::Count;
				break;

			case PixelFormat::eR16G16B16A16_SFLOAT:
				result = PixelDefinitions<	PixelFormat::eR16G16B16A16_SFLOAT >::Count;
				break;

			case PixelFormat::eR32G32B32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR32G32B32_SFLOAT >::Count;
				break;

			case PixelFormat::eR32G32B32A32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR32G32B32A32_SFLOAT >::Count;
				break;

			case PixelFormat::eBC1_RGB_UNORM_BLOCK:
				result = PixelDefinitions<	PixelFormat::eBC1_RGB_UNORM_BLOCK >::Count;
				break;

			case PixelFormat::eBC3_UNORM_BLOCK:
				result = PixelDefinitions<	PixelFormat::eBC3_UNORM_BLOCK >::Count;
				break;

			case PixelFormat::eBC5_UNORM_BLOCK:
				result = PixelDefinitions<	PixelFormat::eBC5_UNORM_BLOCK >::Count;
				break;

			case PixelFormat::eD16_UNORM:
				result = PixelDefinitions< PixelFormat::eD16_UNORM >::Count;
				break;

			case PixelFormat::eD24_UNORM_S8_UINT:
				result = PixelDefinitions< PixelFormat::eD24_UNORM_S8_UINT >::Count;
				break;

			case PixelFormat::eD32_UNORM:
				result = PixelDefinitions< PixelFormat::eD32_UNORM >::Count;
				break;

			case PixelFormat::eD32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eD32_SFLOAT >::Count;
				break;

			case PixelFormat::eD32_SFLOAT_S8_UINT:
				result = PixelDefinitions< PixelFormat::eD32_SFLOAT_S8_UINT >::Count;
				break;

			case PixelFormat::eS8_UINT:
				result = PixelDefinitions< PixelFormat::eS8_UINT >::Count;
				break;

			default:
				CU_Failure( "Unsupported pixel format" );
				break;
			}

			return result;
		}

		void convertPixel( PixelFormat p_eSrcFmt, uint8_t const *& p_pSrc, PixelFormat p_eDestFmt, uint8_t *& p_pDest )
		{
			switch ( p_eSrcFmt )
			{
			case PixelFormat::eR8_UNORM:
				PixelDefinitions< PixelFormat::eR8_UNORM >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eR16_SFLOAT:
				PixelDefinitions< PixelFormat::eR16_SFLOAT >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eR32_SFLOAT:
				PixelDefinitions< PixelFormat::eR32_SFLOAT >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eR8A8_UNORM:
				PixelDefinitions< PixelFormat::eR8A8_UNORM >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eR32A32_SFLOAT:
				PixelDefinitions< PixelFormat::eR32A32_SFLOAT >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eR5G5B5A1_UNORM:
				PixelDefinitions< PixelFormat::eR5G5B5A1_UNORM >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eR5G6B5_UNORM:
				PixelDefinitions< PixelFormat::eR5G6B5_UNORM >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eR8G8B8_UNORM:
				PixelDefinitions< PixelFormat::eR8G8B8_UNORM >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eB8G8R8_UNORM:
				PixelDefinitions< PixelFormat::eB8G8R8_UNORM >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eR8G8B8_SRGB:
				PixelDefinitions< PixelFormat::eR8G8B8_SRGB >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eB8G8R8_SRGB:
				PixelDefinitions< PixelFormat::eB8G8R8_SRGB >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eR8G8B8A8_UNORM:
				PixelDefinitions< PixelFormat::eR8G8B8A8_UNORM >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eA8B8G8R8_UNORM:
				PixelDefinitions< PixelFormat::eA8B8G8R8_UNORM >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eR8G8B8A8_SRGB:
				PixelDefinitions< PixelFormat::eR8G8B8A8_SRGB >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eA8B8G8R8_SRGB:
				PixelDefinitions< PixelFormat::eA8B8G8R8_SRGB >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eR16G16B16_SFLOAT:
				PixelDefinitions< PixelFormat::eR16G16B16_SFLOAT >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eR16G16B16A16_SFLOAT:
				PixelDefinitions< PixelFormat::eR16G16B16A16_SFLOAT >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eR32G32B32_SFLOAT:
				PixelDefinitions< PixelFormat::eR32G32B32_SFLOAT >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eR32G32B32A32_SFLOAT:
				PixelDefinitions< PixelFormat::eR32G32B32A32_SFLOAT >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eBC1_RGB_UNORM_BLOCK:
				PixelDefinitions< PixelFormat::eBC1_RGB_UNORM_BLOCK >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eBC3_UNORM_BLOCK:
				PixelDefinitions< PixelFormat::eBC3_UNORM_BLOCK >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eBC5_UNORM_BLOCK:
				PixelDefinitions< PixelFormat::eBC5_UNORM_BLOCK >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eD16_UNORM:
				PixelDefinitions< PixelFormat::eD16_UNORM >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eD24_UNORM_S8_UINT:
				PixelDefinitions< PixelFormat::eD24_UNORM_S8_UINT >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eD32_UNORM:
				PixelDefinitions< PixelFormat::eD32_UNORM >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eD32_SFLOAT:
				PixelDefinitions< PixelFormat::eD32_SFLOAT >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eD32_SFLOAT_S8_UINT:
				PixelDefinitions< PixelFormat::eD32_SFLOAT_S8_UINT >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eS8_UINT:
				PixelDefinitions< PixelFormat::eS8_UINT >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			default:
				CU_Failure( "Unsupported pixel format" );
				break;
			}
		}

		void convertBuffer( PixelFormat p_eSrcFormat, uint8_t const * p_pSrcBuffer, uint32_t p_uiSrcSize, PixelFormat p_eDstFormat, uint8_t * p_pDstBuffer, uint32_t p_uiDstSize )
		{
			switch ( p_eSrcFormat )
			{
			case PixelFormat::eR8_UNORM:
				PixelDefinitions< PixelFormat::eR8_UNORM >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eR16_SFLOAT:
				PixelDefinitions< PixelFormat::eR16_SFLOAT >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eR32_SFLOAT:
				PixelDefinitions< PixelFormat::eR32_SFLOAT >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eR8A8_UNORM:
				PixelDefinitions< PixelFormat::eR8A8_UNORM >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eR32A32_SFLOAT:
				PixelDefinitions< PixelFormat::eR32A32_SFLOAT >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eR5G5B5A1_UNORM:
				PixelDefinitions< PixelFormat::eR5G5B5A1_UNORM >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eR5G6B5_UNORM:
				PixelDefinitions< PixelFormat::eR5G6B5_UNORM >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eR8G8B8_UNORM:
				PixelDefinitions< PixelFormat::eR8G8B8_UNORM >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eB8G8R8_UNORM:
				PixelDefinitions< PixelFormat::eB8G8R8_UNORM >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eR8G8B8_SRGB:
				PixelDefinitions< PixelFormat::eR8G8B8_SRGB >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eB8G8R8_SRGB:
				PixelDefinitions< PixelFormat::eB8G8R8_SRGB >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eR8G8B8A8_UNORM:
				PixelDefinitions< PixelFormat::eR8G8B8A8_UNORM >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eA8B8G8R8_UNORM:
				PixelDefinitions< PixelFormat::eA8B8G8R8_UNORM >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eR8G8B8A8_SRGB:
				PixelDefinitions< PixelFormat::eR8G8B8A8_SRGB >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eA8B8G8R8_SRGB:
				PixelDefinitions< PixelFormat::eA8B8G8R8_SRGB >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eR16G16B16_SFLOAT:
				PixelDefinitions< PixelFormat::eR16G16B16_SFLOAT >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eR16G16B16A16_SFLOAT:
				PixelDefinitions< PixelFormat::eR16G16B16A16_SFLOAT >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eR32G32B32_SFLOAT:
				PixelDefinitions< PixelFormat::eR32G32B32_SFLOAT >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eR32G32B32A32_SFLOAT:
				PixelDefinitions< PixelFormat::eR32G32B32A32_SFLOAT >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eBC1_RGB_UNORM_BLOCK:
				PixelDefinitions< PixelFormat::eBC1_RGB_UNORM_BLOCK >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eBC3_UNORM_BLOCK:
				PixelDefinitions< PixelFormat::eBC3_UNORM_BLOCK >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eBC5_UNORM_BLOCK:
				PixelDefinitions< PixelFormat::eBC5_UNORM_BLOCK >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eD16_UNORM:
				PixelDefinitions< PixelFormat::eD16_UNORM >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eD24_UNORM_S8_UINT:
				PixelDefinitions< PixelFormat::eD24_UNORM_S8_UINT >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eD32_UNORM:
				PixelDefinitions< PixelFormat::eD32_UNORM >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eD32_SFLOAT:
				PixelDefinitions< PixelFormat::eD32_SFLOAT >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eD32_SFLOAT_S8_UINT:
				PixelDefinitions< PixelFormat::eD32_SFLOAT_S8_UINT >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eS8_UINT:
				PixelDefinitions< PixelFormat::eS8_UINT >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			default:
				CU_Failure( "Unsupported pixel format" );
				break;
			}
		}

		PixelFormat getFormatByName( String const & p_strFormat )
		{
			PixelFormat result = PixelFormat::eCount;

			for ( int i = 0; i < int( result ); ++i )
			{
				switch ( PixelFormat( i ) )
				{
				case PixelFormat::eR8_UNORM:
					result = ( p_strFormat == PixelDefinitions< PixelFormat::eR8_UNORM >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eR16_SFLOAT:
					result = ( p_strFormat == PixelDefinitions< PixelFormat::eR16_SFLOAT >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eR32_SFLOAT:
					result = ( p_strFormat == PixelDefinitions< PixelFormat::eR32_SFLOAT >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eR8A8_UNORM:
					result = ( p_strFormat == PixelDefinitions< PixelFormat::eR8A8_UNORM >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eR16A16_SFLOAT:
					result = ( p_strFormat == PixelDefinitions< PixelFormat::eR16_SFLOAT >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eR32A32_SFLOAT:
					result = ( p_strFormat == PixelDefinitions< PixelFormat::eR32A32_SFLOAT >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eR5G5B5A1_UNORM:
					result = ( p_strFormat == PixelDefinitions< PixelFormat::eR5G5B5A1_UNORM >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eR5G6B5_UNORM:
					result = ( p_strFormat == PixelDefinitions< PixelFormat::eR5G6B5_UNORM >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eR8G8B8_UNORM:
					result = ( p_strFormat == PixelDefinitions< PixelFormat::eR8G8B8_UNORM >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eB8G8R8_UNORM:
					result = ( p_strFormat == PixelDefinitions< PixelFormat::eB8G8R8_UNORM >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eR8G8B8_SRGB:
					result = ( p_strFormat == PixelDefinitions< PixelFormat::eR8G8B8_SRGB >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eB8G8R8_SRGB:
					result = ( p_strFormat == PixelDefinitions< PixelFormat::eB8G8R8_SRGB >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eR8G8B8A8_UNORM:
					result = ( p_strFormat == PixelDefinitions< PixelFormat::eR8G8B8A8_UNORM >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eA8B8G8R8_UNORM:
					result = ( p_strFormat == PixelDefinitions< PixelFormat::eA8B8G8R8_UNORM >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eR8G8B8A8_SRGB:
					result = ( p_strFormat == PixelDefinitions< PixelFormat::eR8G8B8A8_SRGB >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eA8B8G8R8_SRGB:
					result = ( p_strFormat == PixelDefinitions< PixelFormat::eA8B8G8R8_SRGB >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eR16G16B16_SFLOAT:
					result = ( p_strFormat == PixelDefinitions< PixelFormat::eR16G16B16_SFLOAT >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eR16G16B16A16_SFLOAT:
					result = ( p_strFormat == PixelDefinitions< PixelFormat::eR16G16B16A16_SFLOAT >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eR32G32B32_SFLOAT:
					result = ( p_strFormat == PixelDefinitions< PixelFormat::eR32G32B32_SFLOAT >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eR32G32B32A32_SFLOAT:
					result = ( p_strFormat == PixelDefinitions< PixelFormat::eR32G32B32A32_SFLOAT >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eBC1_RGB_UNORM_BLOCK:
					result = ( p_strFormat == PixelDefinitions< PixelFormat::eBC1_RGB_UNORM_BLOCK >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eBC3_UNORM_BLOCK:
					result = ( p_strFormat == PixelDefinitions< PixelFormat::eBC3_UNORM_BLOCK >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eBC5_UNORM_BLOCK:
					result = ( p_strFormat == PixelDefinitions< PixelFormat::eBC5_UNORM_BLOCK >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eD16_UNORM:
					result = ( p_strFormat == PixelDefinitions< PixelFormat::eD16_UNORM >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eD24_UNORM_S8_UINT:
					result = ( p_strFormat == PixelDefinitions< PixelFormat::eD24_UNORM_S8_UINT >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eD32_UNORM:
					result = ( p_strFormat == PixelDefinitions< PixelFormat::eD32_UNORM >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eD32_SFLOAT:
					result = ( p_strFormat == PixelDefinitions< PixelFormat::eD32_SFLOAT >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eD32_SFLOAT_S8_UINT:
					result = ( p_strFormat == PixelDefinitions< PixelFormat::eD32_SFLOAT_S8_UINT >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eS8_UINT:
					result = ( p_strFormat == PixelDefinitions< PixelFormat::eS8_UINT >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				default:
					CU_Failure( "Unsupported pixel format" );
					break;
				}
			}

			return result;
		}

		String getFormatName( PixelFormat p_format )
		{
			String result;

			switch ( p_format )
			{
			case PixelFormat::eR8_UNORM:
				result = PixelDefinitions< PixelFormat::eR8_UNORM >::toStr();
				break;

			case PixelFormat::eR16_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR32_SFLOAT >::toStr();
				break;

			case PixelFormat::eR32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR32_SFLOAT >::toStr();
				break;

			case PixelFormat::eR8A8_UNORM:
				result = PixelDefinitions< PixelFormat::eR8A8_UNORM >::toStr();
				break;

			case PixelFormat::eR32A32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR32A32_SFLOAT >::toStr();
				break;

			case PixelFormat::eR5G5B5A1_UNORM:
				result = PixelDefinitions< PixelFormat::eR5G5B5A1_UNORM >::toStr();
				break;

			case PixelFormat::eR5G6B5_UNORM:
				result = PixelDefinitions< PixelFormat::eR5G6B5_UNORM >::toStr();
				break;

			case PixelFormat::eR8G8B8_UNORM:
				result = PixelDefinitions< PixelFormat::eR8G8B8_UNORM >::toStr();
				break;

			case PixelFormat::eB8G8R8_UNORM:
				result = PixelDefinitions< PixelFormat::eB8G8R8_UNORM >::toStr();
				break;

			case PixelFormat::eR8G8B8_SRGB:
				result = PixelDefinitions< PixelFormat::eR8G8B8_SRGB >::toStr();
				break;

			case PixelFormat::eB8G8R8_SRGB:
				result = PixelDefinitions< PixelFormat::eB8G8R8_SRGB >::toStr();
				break;

			case PixelFormat::eR8G8B8A8_UNORM:
				result = PixelDefinitions< PixelFormat::eR8G8B8A8_UNORM >::toStr();
				break;

			case PixelFormat::eA8B8G8R8_UNORM:
				result = PixelDefinitions< PixelFormat::eA8B8G8R8_UNORM >::toStr();
				break;

			case PixelFormat::eR8G8B8A8_SRGB:
				result = PixelDefinitions< PixelFormat::eR8G8B8A8_SRGB >::toStr();
				break;

			case PixelFormat::eA8B8G8R8_SRGB:
				result = PixelDefinitions< PixelFormat::eA8B8G8R8_SRGB >::toStr();
				break;

			case PixelFormat::eR16G16B16_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR16G16B16_SFLOAT >::toStr();
				break;

			case PixelFormat::eR16G16B16A16_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR16G16B16A16_SFLOAT >::toStr();
				break;

			case PixelFormat::eR32G32B32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR32G32B32_SFLOAT >::toStr();
				break;

			case PixelFormat::eR32G32B32A32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eR32G32B32A32_SFLOAT >::toStr();
				break;

			case PixelFormat::eBC1_RGB_UNORM_BLOCK:
				result = PixelDefinitions< PixelFormat::eBC1_RGB_UNORM_BLOCK >::toStr();
				break;

			case PixelFormat::eBC3_UNORM_BLOCK:
				result = PixelDefinitions< PixelFormat::eBC3_UNORM_BLOCK >::toStr();
				break;

			case PixelFormat::eBC5_UNORM_BLOCK:
				result = PixelDefinitions< PixelFormat::eBC5_UNORM_BLOCK >::toStr();
				break;

			case PixelFormat::eD16_UNORM:
				result = PixelDefinitions< PixelFormat::eD16_UNORM >::toStr();
				break;

			case PixelFormat::eD24_UNORM_S8_UINT:
				result = PixelDefinitions< PixelFormat::eD24_UNORM_S8_UINT >::toStr();
				break;

			case PixelFormat::eD32_UNORM:
				result = PixelDefinitions< PixelFormat::eD32_UNORM >::toStr();
				break;

			case PixelFormat::eD32_SFLOAT:
				result = PixelDefinitions< PixelFormat::eD32_SFLOAT >::toStr();
				break;

			case PixelFormat::eD32_SFLOAT_S8_UINT:
				result = PixelDefinitions< PixelFormat::eD32_SFLOAT_S8_UINT >::toStr();
				break;

			case PixelFormat::eS8_UINT:
				result = PixelDefinitions< PixelFormat::eS8_UINT >::toStr();
				break;

			default:
				CU_Failure( "Unsupported pixel format" );
				break;
			}

			return result;
		}
	}
}
