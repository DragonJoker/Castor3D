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
			case PixelFormat::eA8L8:
				result = PixelDefinitions< PixelFormat::eA8L8 >::NoAlphaPF;
				break;

			case PixelFormat::eAL16F:
				result = PixelDefinitions< PixelFormat::eAL16F >::NoAlphaPF;
				break;

			case PixelFormat::eAL32F:
				result = PixelDefinitions< PixelFormat::eAL32F >::NoAlphaPF;
				break;

			case PixelFormat::eA1R5G5B5:
				result = PixelDefinitions< PixelFormat::eA1R5G5B5 >::NoAlphaPF;
				break;

			case PixelFormat::eA8R8G8B8:
				result = PixelDefinitions< PixelFormat::eA8R8G8B8 >::NoAlphaPF;
				break;

			case PixelFormat::eA8B8G8R8:
				result = PixelDefinitions< PixelFormat::eA8B8G8R8 >::NoAlphaPF;
				break;

			case PixelFormat::eA8R8G8B8_SRGB:
				result = PixelDefinitions< PixelFormat::eA8R8G8B8 >::NoAlphaPF;
				break;

			case PixelFormat::eA8B8G8R8_SRGB:
				result = PixelDefinitions< PixelFormat::eA8B8G8R8 >::NoAlphaPF;
				break;

			case PixelFormat::eRGBA32F:
				result = PixelDefinitions< PixelFormat::eRGBA32F >::NoAlphaPF;
				break;

			case PixelFormat::eDXTC3:
				result = PixelDefinitions< PixelFormat::eDXTC3 >::NoAlphaPF;
				break;

			case PixelFormat::eDXTC5:
				result = PixelDefinitions< PixelFormat::eDXTC5 >::NoAlphaPF;
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
			case PixelFormat::eL8:
				result = PixelDefinitions< PixelFormat::eL8 >::Alpha;
				break;

			case PixelFormat::eL16F:
				result = PixelDefinitions< PixelFormat::eL16F >::Alpha;
				break;

			case PixelFormat::eL32F:
				result = PixelDefinitions< PixelFormat::eL32F >::Alpha;
				break;

			case PixelFormat::eA8L8:
				result = PixelDefinitions< PixelFormat::eA8L8 >::Alpha;
				break;

			case PixelFormat::eAL32F:
				result = PixelDefinitions< PixelFormat::eAL32F >::Alpha;
				break;

			case PixelFormat::eA1R5G5B5:
				result = PixelDefinitions< PixelFormat::eA1R5G5B5 >::Alpha;
				break;

			case PixelFormat::eR5G6B5:
				result = PixelDefinitions< PixelFormat::eR5G6B5 >::Alpha;
				break;

			case PixelFormat::eR8G8B8:
				result = PixelDefinitions< PixelFormat::eR8G8B8 >::Alpha;
				break;

			case PixelFormat::eB8G8R8:
				result = PixelDefinitions< PixelFormat::eB8G8R8 >::Alpha;
				break;

			case PixelFormat::eR8G8B8_SRGB:
				result = PixelDefinitions< PixelFormat::eR8G8B8_SRGB >::Alpha;
				break;

			case PixelFormat::eB8G8R8_SRGB:
				result = PixelDefinitions< PixelFormat::eB8G8R8_SRGB >::Alpha;
				break;

			case PixelFormat::eA8R8G8B8:
				result = PixelDefinitions< PixelFormat::eA8R8G8B8 >::Alpha;
				break;

			case PixelFormat::eA8B8G8R8:
				result = PixelDefinitions< PixelFormat::eA8B8G8R8 >::Alpha;
				break;

			case PixelFormat::eA8R8G8B8_SRGB:
				result = PixelDefinitions< PixelFormat::eA8R8G8B8_SRGB >::Alpha;
				break;

			case PixelFormat::eA8B8G8R8_SRGB:
				result = PixelDefinitions< PixelFormat::eA8B8G8R8_SRGB >::Alpha;
				break;

			case PixelFormat::eRGB16F:
				result = PixelDefinitions< PixelFormat::eRGB16F >::Alpha;
				break;

			case PixelFormat::eRGBA16F:
				result = PixelDefinitions< PixelFormat::eRGBA16F >::Alpha;
				break;

			case PixelFormat::eRGB32F:
				result = PixelDefinitions< PixelFormat::eRGB32F >::Alpha;
				break;

			case PixelFormat::eRGBA32F:
				result = PixelDefinitions< PixelFormat::eRGBA32F >::Alpha;
				break;

			case PixelFormat::eDXTC1:
				result = PixelDefinitions< PixelFormat::eDXTC1 >::Alpha;
				break;

			case PixelFormat::eDXTC3:
				result = PixelDefinitions< PixelFormat::eDXTC3 >::Alpha;
				break;

			case PixelFormat::eDXTC5:
				result = PixelDefinitions< PixelFormat::eDXTC5 >::Alpha;
				break;

			case PixelFormat::eD16:
				result = PixelDefinitions< PixelFormat::eD16 >::Alpha;
				break;

			case PixelFormat::eD24S8:
				result = PixelDefinitions< PixelFormat::eD24S8 >::Alpha;
				break;

			case PixelFormat::eD32:
				result = PixelDefinitions< PixelFormat::eD32 >::Alpha;
				break;

			case PixelFormat::eD32F:
				result = PixelDefinitions< PixelFormat::eD32F >::Alpha;
				break;

			case PixelFormat::eD32FS8:
				result = PixelDefinitions< PixelFormat::eD32FS8 >::Alpha;
				break;

			case PixelFormat::eS8:
				result = PixelDefinitions< PixelFormat::eS8 >::Alpha;
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
			case PixelFormat::eL8:
				result = PixelDefinitions< PixelFormat::eL8 >::Compressed;
				break;

			case PixelFormat::eL16F:
				result = PixelDefinitions< PixelFormat::eL16F >::Compressed;
				break;

			case PixelFormat::eL32F:
				result = PixelDefinitions< PixelFormat::eL32F >::Compressed;
				break;

			case PixelFormat::eA8L8:
				result = PixelDefinitions< PixelFormat::eA8L8 >::Compressed;
				break;

			case PixelFormat::eAL32F:
				result = PixelDefinitions< PixelFormat::eAL32F >::Compressed;
				break;

			case PixelFormat::eA1R5G5B5:
				result = PixelDefinitions< PixelFormat::eA1R5G5B5 >::Compressed;
				break;

			case PixelFormat::eR5G6B5:
				result = PixelDefinitions< PixelFormat::eR5G6B5 >::Compressed;
				break;

			case PixelFormat::eR8G8B8:
				result = PixelDefinitions< PixelFormat::eR8G8B8 >::Compressed;
				break;

			case PixelFormat::eB8G8R8:
				result = PixelDefinitions< PixelFormat::eB8G8R8 >::Compressed;
				break;

			case PixelFormat::eR8G8B8_SRGB:
				result = PixelDefinitions< PixelFormat::eR8G8B8_SRGB >::Compressed;
				break;

			case PixelFormat::eB8G8R8_SRGB:
				result = PixelDefinitions< PixelFormat::eB8G8R8_SRGB >::Compressed;
				break;

			case PixelFormat::eA8R8G8B8:
				result = PixelDefinitions< PixelFormat::eA8R8G8B8 >::Compressed;
				break;

			case PixelFormat::eA8B8G8R8:
				result = PixelDefinitions< PixelFormat::eA8B8G8R8 >::Compressed;
				break;

			case PixelFormat::eA8R8G8B8_SRGB:
				result = PixelDefinitions< PixelFormat::eA8R8G8B8_SRGB >::Compressed;
				break;

			case PixelFormat::eA8B8G8R8_SRGB:
				result = PixelDefinitions< PixelFormat::eA8B8G8R8_SRGB >::Compressed;
				break;

			case PixelFormat::eRGB16F:
				result = PixelDefinitions< PixelFormat::eRGB16F >::Compressed;
				break;

			case PixelFormat::eRGBA16F:
				result = PixelDefinitions< PixelFormat::eRGBA16F >::Compressed;
				break;

			case PixelFormat::eRGB32F:
				result = PixelDefinitions< PixelFormat::eRGB32F >::Compressed;
				break;

			case PixelFormat::eRGBA32F:
				result = PixelDefinitions< PixelFormat::eRGBA32F >::Compressed;
				break;

			case PixelFormat::eDXTC1:
				result = PixelDefinitions< PixelFormat::eDXTC1 >::Compressed;
				break;

			case PixelFormat::eDXTC3:
				result = PixelDefinitions< PixelFormat::eDXTC3 >::Compressed;
				break;

			case PixelFormat::eDXTC5:
				result = PixelDefinitions< PixelFormat::eDXTC5 >::Compressed;
				break;

			case PixelFormat::eD16:
				result = PixelDefinitions< PixelFormat::eD16 >::Compressed;
				break;

			case PixelFormat::eD24S8:
				result = PixelDefinitions< PixelFormat::eD24S8 >::Compressed;
				break;

			case PixelFormat::eD32:
				result = PixelDefinitions< PixelFormat::eD32 >::Compressed;
				break;

			case PixelFormat::eD32F:
				result = PixelDefinitions< PixelFormat::eD32F >::Compressed;
				break;

			case PixelFormat::eD32FS8:
				result = PixelDefinitions< PixelFormat::eD32FS8 >::Compressed;
				break;

			case PixelFormat::eS8:
				result = PixelDefinitions< PixelFormat::eS8 >::Compressed;
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
				result = PxBufferBase::create( p_pSrc->dimensions(), PixelFormat::eA8L8, p_pSrc->constPtr(), p_pSrc->format() );
				uint8_t * pBuffer = result->ptr();

				for ( uint32_t i = 0; i < result->size(); i += 2 )
				{
					pBuffer[0] = pBuffer[1];
					pBuffer++;
					pBuffer++;
				}

				result = PxBufferBase::create( p_pSrc->dimensions(), PixelFormat::eL8, result->constPtr(), result->format() );
				p_pSrc = PxBufferBase::create( p_pSrc->dimensions(), getPFWithoutAlpha( p_pSrc->format() ), p_pSrc->constPtr(), p_pSrc->format() );
			}

			return result;
		}

		void reduceToAlpha( PxBufferBaseSPtr & p_pSrc )
		{
			if ( hasAlpha( p_pSrc->format() ) )
			{
				if ( p_pSrc->format() != PixelFormat::eA8L8 )
				{
					p_pSrc = PxBufferBase::create( p_pSrc->dimensions(), PixelFormat::eA8L8, p_pSrc->constPtr(), p_pSrc->format() );
				}

				uint8_t * pBuffer = p_pSrc->ptr();

				for ( uint32_t i = 0; i < p_pSrc->size(); i += 2 )
				{
					pBuffer[0] = pBuffer[1];
					pBuffer++;
					pBuffer++;
				}
			}

			p_pSrc = PxBufferBase::create( p_pSrc->dimensions(), PixelFormat::eL8, p_pSrc->constPtr(), p_pSrc->format() );
		}

		uint8_t getBytesPerPixel( PixelFormat p_format )
		{
			uint8_t result = 0;

			switch ( p_format )
			{
			case PixelFormat::eL8:
				result = PixelDefinitions< PixelFormat::eL8 >::Size;
				break;

			case PixelFormat::eL16F:
				result = PixelDefinitions< PixelFormat::eL16F >::Size;
				break;

			case PixelFormat::eL32F:
				result = PixelDefinitions< PixelFormat::eL32F >::Size;
				break;

			case PixelFormat::eA8L8:
				result = PixelDefinitions< PixelFormat::eA8L8 >::Size;
				break;

			case PixelFormat::eAL32F:
				result = PixelDefinitions< PixelFormat::eAL32F >::Size;
				break;

			case PixelFormat::eA1R5G5B5:
				result = PixelDefinitions< PixelFormat::eA1R5G5B5 >::Size;
				break;

			case PixelFormat::eR5G6B5:
				result = PixelDefinitions< PixelFormat::eR5G6B5 >::Size;
				break;

			case PixelFormat::eR8G8B8:
				result = PixelDefinitions< PixelFormat::eR8G8B8 >::Size;
				break;

			case PixelFormat::eB8G8R8:
				result = PixelDefinitions< PixelFormat::eB8G8R8 >::Size;
				break;

			case PixelFormat::eR8G8B8_SRGB:
				result = PixelDefinitions< PixelFormat::eR8G8B8_SRGB >::Size;
				break;

			case PixelFormat::eB8G8R8_SRGB:
				result = PixelDefinitions< PixelFormat::eB8G8R8_SRGB >::Size;
				break;

			case PixelFormat::eA8R8G8B8:
				result = PixelDefinitions< PixelFormat::eA8R8G8B8 >::Size;
				break;

			case PixelFormat::eA8B8G8R8:
				result = PixelDefinitions< PixelFormat::eA8B8G8R8 >::Size;
				break;

			case PixelFormat::eA8R8G8B8_SRGB:
				result = PixelDefinitions< PixelFormat::eA8R8G8B8_SRGB >::Size;
				break;

			case PixelFormat::eA8B8G8R8_SRGB:
				result = PixelDefinitions< PixelFormat::eA8B8G8R8_SRGB >::Size;
				break;

			case PixelFormat::eRGB16F:
				result = PixelDefinitions<	PixelFormat::eRGB16F >::Size;
				break;

			case PixelFormat::eRGBA16F:
				result = PixelDefinitions<	PixelFormat::eRGBA16F >::Size;
				break;

			case PixelFormat::eRGB32F:
				result = PixelDefinitions< PixelFormat::eRGB32F >::Size;
				break;

			case PixelFormat::eRGBA32F:
				result = PixelDefinitions< PixelFormat::eRGBA32F >::Size;
				break;

			case PixelFormat::eDXTC1:
				result = PixelDefinitions<	PixelFormat::eDXTC1 >::Size;
				break;

			case PixelFormat::eDXTC3:
				result = PixelDefinitions<	PixelFormat::eDXTC3 >::Size;
				break;

			case PixelFormat::eDXTC5:
				result = PixelDefinitions<	PixelFormat::eDXTC5 >::Size;
				break;

			case PixelFormat::eD16:
				result = PixelDefinitions< PixelFormat::eD16 >::Size;
				break;

			case PixelFormat::eD24S8:
				result = PixelDefinitions< PixelFormat::eD24S8 >::Size;
				break;

			case PixelFormat::eD32:
				result = PixelDefinitions< PixelFormat::eD32 >::Size;
				break;

			case PixelFormat::eD32F:
				result = PixelDefinitions< PixelFormat::eD32F >::Size;
				break;

			case PixelFormat::eD32FS8:
				result = PixelDefinitions< PixelFormat::eD32FS8 >::Size;
				break;

			case PixelFormat::eS8:
				result = PixelDefinitions< PixelFormat::eS8 >::Size;
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
			case PixelFormat::eL8:
				result = PixelDefinitions< PixelFormat::eL8 >::Count;
				break;

			case PixelFormat::eL32F:
				result = PixelDefinitions< PixelFormat::eL32F >::Count;
				break;

			case PixelFormat::eL16F:
				result = PixelDefinitions< PixelFormat::eL16F >::Count;
				break;

			case PixelFormat::eA8L8:
				result = PixelDefinitions< PixelFormat::eA8L8 >::Count;
				break;

			case PixelFormat::eAL32F:
				result = PixelDefinitions< PixelFormat::eAL32F >::Count;
				break;

			case PixelFormat::eA1R5G5B5:
				result = PixelDefinitions< PixelFormat::eA1R5G5B5 >::Count;
				break;

			case PixelFormat::eR5G6B5:
				result = PixelDefinitions< PixelFormat::eR5G6B5 >::Count;
				break;

			case PixelFormat::eR8G8B8:
				result = PixelDefinitions< PixelFormat::eR8G8B8 >::Count;
				break;

			case PixelFormat::eB8G8R8:
				result = PixelDefinitions< PixelFormat::eB8G8R8 >::Count;
				break;

			case PixelFormat::eR8G8B8_SRGB:
				result = PixelDefinitions< PixelFormat::eR8G8B8_SRGB >::Count;
				break;

			case PixelFormat::eB8G8R8_SRGB:
				result = PixelDefinitions< PixelFormat::eB8G8R8_SRGB >::Count;
				break;

			case PixelFormat::eA8R8G8B8:
				result = PixelDefinitions< PixelFormat::eA8R8G8B8 >::Count;
				break;

			case PixelFormat::eA8B8G8R8:
				result = PixelDefinitions< PixelFormat::eA8B8G8R8 >::Count;
				break;

			case PixelFormat::eA8R8G8B8_SRGB:
				result = PixelDefinitions< PixelFormat::eA8R8G8B8_SRGB >::Count;
				break;

			case PixelFormat::eA8B8G8R8_SRGB:
				result = PixelDefinitions< PixelFormat::eA8B8G8R8_SRGB >::Count;
				break;

			case PixelFormat::eRGB16F:
				result = PixelDefinitions<	PixelFormat::eRGB16F >::Count;
				break;

			case PixelFormat::eRGBA16F:
				result = PixelDefinitions<	PixelFormat::eRGBA16F >::Count;
				break;

			case PixelFormat::eRGB32F:
				result = PixelDefinitions< PixelFormat::eRGB32F >::Count;
				break;

			case PixelFormat::eRGBA32F:
				result = PixelDefinitions< PixelFormat::eRGBA32F >::Count;
				break;

			case PixelFormat::eDXTC1:
				result = PixelDefinitions<	PixelFormat::eDXTC1 >::Count;
				break;

			case PixelFormat::eDXTC3:
				result = PixelDefinitions<	PixelFormat::eDXTC3 >::Count;
				break;

			case PixelFormat::eDXTC5:
				result = PixelDefinitions<	PixelFormat::eDXTC5 >::Count;
				break;

			case PixelFormat::eD16:
				result = PixelDefinitions< PixelFormat::eD16 >::Count;
				break;

			case PixelFormat::eD24S8:
				result = PixelDefinitions< PixelFormat::eD24S8 >::Count;
				break;

			case PixelFormat::eD32:
				result = PixelDefinitions< PixelFormat::eD32 >::Count;
				break;

			case PixelFormat::eD32F:
				result = PixelDefinitions< PixelFormat::eD32F >::Count;
				break;

			case PixelFormat::eD32FS8:
				result = PixelDefinitions< PixelFormat::eD32FS8 >::Count;
				break;

			case PixelFormat::eS8:
				result = PixelDefinitions< PixelFormat::eS8 >::Count;
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
			case PixelFormat::eL8:
				PixelDefinitions< PixelFormat::eL8 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eL16F:
				PixelDefinitions< PixelFormat::eL16F >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eL32F:
				PixelDefinitions< PixelFormat::eL32F >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eA8L8:
				PixelDefinitions< PixelFormat::eA8L8 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eAL32F:
				PixelDefinitions< PixelFormat::eAL32F >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eA1R5G5B5:
				PixelDefinitions< PixelFormat::eA1R5G5B5 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eR5G6B5:
				PixelDefinitions< PixelFormat::eR5G6B5 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eR8G8B8:
				PixelDefinitions< PixelFormat::eR8G8B8 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eB8G8R8:
				PixelDefinitions< PixelFormat::eB8G8R8 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eR8G8B8_SRGB:
				PixelDefinitions< PixelFormat::eR8G8B8_SRGB >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eB8G8R8_SRGB:
				PixelDefinitions< PixelFormat::eB8G8R8_SRGB >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eA8R8G8B8:
				PixelDefinitions< PixelFormat::eA8R8G8B8 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eA8B8G8R8:
				PixelDefinitions< PixelFormat::eA8B8G8R8 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eA8R8G8B8_SRGB:
				PixelDefinitions< PixelFormat::eA8R8G8B8_SRGB >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eA8B8G8R8_SRGB:
				PixelDefinitions< PixelFormat::eA8B8G8R8_SRGB >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eRGB16F:
				PixelDefinitions< PixelFormat::eRGB16F >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eRGBA16F:
				PixelDefinitions< PixelFormat::eRGBA16F >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eRGB32F:
				PixelDefinitions< PixelFormat::eRGB32F >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eRGBA32F:
				PixelDefinitions< PixelFormat::eRGBA32F >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eDXTC1:
				PixelDefinitions< PixelFormat::eDXTC1 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eDXTC3:
				PixelDefinitions< PixelFormat::eDXTC3 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eDXTC5:
				PixelDefinitions< PixelFormat::eDXTC5 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eD16:
				PixelDefinitions< PixelFormat::eD16 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eD24S8:
				PixelDefinitions< PixelFormat::eD24S8 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eD32:
				PixelDefinitions< PixelFormat::eD32 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eD32F:
				PixelDefinitions< PixelFormat::eD32F >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eD32FS8:
				PixelDefinitions< PixelFormat::eD32FS8 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eS8:
				PixelDefinitions< PixelFormat::eS8 >::convert( p_pSrc, p_pDest, p_eDestFmt );
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
			case PixelFormat::eL8:
				PixelDefinitions< PixelFormat::eL8 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eL16F:
				PixelDefinitions< PixelFormat::eL16F >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eL32F:
				PixelDefinitions< PixelFormat::eL32F >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eA8L8:
				PixelDefinitions< PixelFormat::eA8L8 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eAL32F:
				PixelDefinitions< PixelFormat::eAL32F >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eA1R5G5B5:
				PixelDefinitions< PixelFormat::eA1R5G5B5 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eR5G6B5:
				PixelDefinitions< PixelFormat::eR5G6B5 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eR8G8B8:
				PixelDefinitions< PixelFormat::eR8G8B8 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eB8G8R8:
				PixelDefinitions< PixelFormat::eB8G8R8 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eR8G8B8_SRGB:
				PixelDefinitions< PixelFormat::eR8G8B8_SRGB >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eB8G8R8_SRGB:
				PixelDefinitions< PixelFormat::eB8G8R8_SRGB >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eA8R8G8B8:
				PixelDefinitions< PixelFormat::eA8R8G8B8 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eA8B8G8R8:
				PixelDefinitions< PixelFormat::eA8B8G8R8 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eA8R8G8B8_SRGB:
				PixelDefinitions< PixelFormat::eA8R8G8B8_SRGB >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eA8B8G8R8_SRGB:
				PixelDefinitions< PixelFormat::eA8B8G8R8_SRGB >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eRGB16F:
				PixelDefinitions< PixelFormat::eRGB16F >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eRGBA16F:
				PixelDefinitions< PixelFormat::eRGBA16F >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eRGB32F:
				PixelDefinitions< PixelFormat::eRGB32F >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eRGBA32F:
				PixelDefinitions< PixelFormat::eRGBA32F >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eDXTC1:
				PixelDefinitions< PixelFormat::eDXTC1 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eDXTC3:
				PixelDefinitions< PixelFormat::eDXTC3 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eDXTC5:
				PixelDefinitions< PixelFormat::eDXTC5 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eD16:
				PixelDefinitions< PixelFormat::eD16 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eD24S8:
				PixelDefinitions< PixelFormat::eD24S8 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eD32:
				PixelDefinitions< PixelFormat::eD32 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eD32F:
				PixelDefinitions< PixelFormat::eD32F >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eD32FS8:
				PixelDefinitions< PixelFormat::eD32FS8 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eS8:
				PixelDefinitions< PixelFormat::eS8 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
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
				case PixelFormat::eL8:
					result = ( p_strFormat == PixelDefinitions< PixelFormat::eL8 >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eL16F:
					result = ( p_strFormat == PixelDefinitions< PixelFormat::eL16F >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eL32F:
					result = ( p_strFormat == PixelDefinitions< PixelFormat::eL32F >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eA8L8:
					result = ( p_strFormat == PixelDefinitions< PixelFormat::eA8L8 >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eAL16F:
					result = ( p_strFormat == PixelDefinitions< PixelFormat::eL16F >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eAL32F:
					result = ( p_strFormat == PixelDefinitions< PixelFormat::eAL32F >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eA1R5G5B5:
					result = ( p_strFormat == PixelDefinitions< PixelFormat::eA1R5G5B5 >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eR5G6B5:
					result = ( p_strFormat == PixelDefinitions< PixelFormat::eR5G6B5 >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eR8G8B8:
					result = ( p_strFormat == PixelDefinitions< PixelFormat::eR8G8B8 >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eB8G8R8:
					result = ( p_strFormat == PixelDefinitions< PixelFormat::eB8G8R8 >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eR8G8B8_SRGB:
					result = ( p_strFormat == PixelDefinitions< PixelFormat::eR8G8B8_SRGB >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eB8G8R8_SRGB:
					result = ( p_strFormat == PixelDefinitions< PixelFormat::eB8G8R8_SRGB >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eA8R8G8B8:
					result = ( p_strFormat == PixelDefinitions< PixelFormat::eA8R8G8B8 >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eA8B8G8R8:
					result = ( p_strFormat == PixelDefinitions< PixelFormat::eA8B8G8R8 >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eA8R8G8B8_SRGB:
					result = ( p_strFormat == PixelDefinitions< PixelFormat::eA8R8G8B8_SRGB >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eA8B8G8R8_SRGB:
					result = ( p_strFormat == PixelDefinitions< PixelFormat::eA8B8G8R8_SRGB >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eRGB16F:
					result = ( p_strFormat == PixelDefinitions< PixelFormat::eRGB16F >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eRGBA16F:
					result = ( p_strFormat == PixelDefinitions< PixelFormat::eRGBA16F >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eRGB32F:
					result = ( p_strFormat == PixelDefinitions< PixelFormat::eRGB32F >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eRGBA32F:
					result = ( p_strFormat == PixelDefinitions< PixelFormat::eRGBA32F >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eDXTC1:
					result = ( p_strFormat == PixelDefinitions< PixelFormat::eDXTC1 >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eDXTC3:
					result = ( p_strFormat == PixelDefinitions< PixelFormat::eDXTC3 >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eDXTC5:
					result = ( p_strFormat == PixelDefinitions< PixelFormat::eDXTC5 >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eD16:
					result = ( p_strFormat == PixelDefinitions< PixelFormat::eD16 >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eD24S8:
					result = ( p_strFormat == PixelDefinitions< PixelFormat::eD24S8 >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eD32:
					result = ( p_strFormat == PixelDefinitions< PixelFormat::eD32 >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eD32F:
					result = ( p_strFormat == PixelDefinitions< PixelFormat::eD32F >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eD32FS8:
					result = ( p_strFormat == PixelDefinitions< PixelFormat::eD32FS8 >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eS8:
					result = ( p_strFormat == PixelDefinitions< PixelFormat::eS8 >::toStr() ? PixelFormat( i ) : PixelFormat::eCount );
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
			case PixelFormat::eL8:
				result = PixelDefinitions< PixelFormat::eL8 >::toStr();
				break;

			case PixelFormat::eL16F:
				result = PixelDefinitions< PixelFormat::eL32F >::toStr();
				break;

			case PixelFormat::eL32F:
				result = PixelDefinitions< PixelFormat::eL32F >::toStr();
				break;

			case PixelFormat::eA8L8:
				result = PixelDefinitions< PixelFormat::eA8L8 >::toStr();
				break;

			case PixelFormat::eAL32F:
				result = PixelDefinitions< PixelFormat::eAL32F >::toStr();
				break;

			case PixelFormat::eA1R5G5B5:
				result = PixelDefinitions< PixelFormat::eA1R5G5B5 >::toStr();
				break;

			case PixelFormat::eR5G6B5:
				result = PixelDefinitions< PixelFormat::eR5G6B5 >::toStr();
				break;

			case PixelFormat::eR8G8B8:
				result = PixelDefinitions< PixelFormat::eR8G8B8 >::toStr();
				break;

			case PixelFormat::eB8G8R8:
				result = PixelDefinitions< PixelFormat::eB8G8R8 >::toStr();
				break;

			case PixelFormat::eR8G8B8_SRGB:
				result = PixelDefinitions< PixelFormat::eR8G8B8_SRGB >::toStr();
				break;

			case PixelFormat::eB8G8R8_SRGB:
				result = PixelDefinitions< PixelFormat::eB8G8R8_SRGB >::toStr();
				break;

			case PixelFormat::eA8R8G8B8:
				result = PixelDefinitions< PixelFormat::eA8R8G8B8 >::toStr();
				break;

			case PixelFormat::eA8B8G8R8:
				result = PixelDefinitions< PixelFormat::eA8B8G8R8 >::toStr();
				break;

			case PixelFormat::eA8R8G8B8_SRGB:
				result = PixelDefinitions< PixelFormat::eA8R8G8B8_SRGB >::toStr();
				break;

			case PixelFormat::eA8B8G8R8_SRGB:
				result = PixelDefinitions< PixelFormat::eA8B8G8R8_SRGB >::toStr();
				break;

			case PixelFormat::eRGB16F:
				result = PixelDefinitions< PixelFormat::eRGB16F >::toStr();
				break;

			case PixelFormat::eRGBA16F:
				result = PixelDefinitions< PixelFormat::eRGBA16F >::toStr();
				break;

			case PixelFormat::eRGB32F:
				result = PixelDefinitions< PixelFormat::eRGB32F >::toStr();
				break;

			case PixelFormat::eRGBA32F:
				result = PixelDefinitions< PixelFormat::eRGBA32F >::toStr();
				break;

			case PixelFormat::eDXTC1:
				result = PixelDefinitions< PixelFormat::eDXTC1 >::toStr();
				break;

			case PixelFormat::eDXTC3:
				result = PixelDefinitions< PixelFormat::eDXTC3 >::toStr();
				break;

			case PixelFormat::eDXTC5:
				result = PixelDefinitions< PixelFormat::eDXTC5 >::toStr();
				break;

			case PixelFormat::eD16:
				result = PixelDefinitions< PixelFormat::eD16 >::toStr();
				break;

			case PixelFormat::eD24S8:
				result = PixelDefinitions< PixelFormat::eD24S8 >::toStr();
				break;

			case PixelFormat::eD32:
				result = PixelDefinitions< PixelFormat::eD32 >::toStr();
				break;

			case PixelFormat::eD32F:
				result = PixelDefinitions< PixelFormat::eD32F >::toStr();
				break;

			case PixelFormat::eD32FS8:
				result = PixelDefinitions< PixelFormat::eD32FS8 >::toStr();
				break;

			case PixelFormat::eS8:
				result = PixelDefinitions< PixelFormat::eS8 >::toStr();
				break;

			default:
				CU_Failure( "Unsupported pixel format" );
				break;
			}

			return result;
		}
	}
}
