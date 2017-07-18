#include "PixelFormat.hpp"
#include "PixelBuffer.hpp"

namespace Castor
{
#if defined( _X64 ) && CASTOR_USE_SSE2

	namespace detail
	{
		const __m128i buffer_converter< PixelFormat::eYUY2, PixelFormat::eR8G8B8 >::umask	= _mm_set1_epi32( 0x0000FF00 );
		const __m128i buffer_converter< PixelFormat::eYUY2, PixelFormat::eR8G8B8 >::vmask	= _mm_set1_epi32( 0xFF000000 );
		const __m128i buffer_converter< PixelFormat::eYUY2, PixelFormat::eR8G8B8 >::ymask	= _mm_set1_epi32( 0x00FF00FF );
		const __m128i buffer_converter< PixelFormat::eYUY2, PixelFormat::eR8G8B8 >::uvsub	= _mm_set1_epi32( 0x00800080 );
		const __m128i buffer_converter< PixelFormat::eYUY2, PixelFormat::eR8G8B8 >::ysub	= _mm_set1_epi32( 0x00100010 );
		const __m128i buffer_converter< PixelFormat::eYUY2, PixelFormat::eR8G8B8 >::facy	= _mm_set1_epi32( 0x00400040 );// 1 * 64
		const __m128i buffer_converter< PixelFormat::eYUY2, PixelFormat::eR8G8B8 >::facrv	= _mm_set1_epi32( 0x00710071 );// 1.772 * 64
		const __m128i buffer_converter< PixelFormat::eYUY2, PixelFormat::eR8G8B8 >::facgu	= _mm_set1_epi32( 0x002D002D );// 0.714 * 64
		const __m128i buffer_converter< PixelFormat::eYUY2, PixelFormat::eR8G8B8 >::facgv	= _mm_set1_epi32( 0x00160016 );// 0.344 * 64
		const __m128i buffer_converter< PixelFormat::eYUY2, PixelFormat::eR8G8B8 >::facbu	= _mm_set1_epi32( 0x00590059 );// 1.402 * 64
		const __m128i buffer_converter< PixelFormat::eYUY2, PixelFormat::eR8G8B8 >::zero	= _mm_set1_epi32( 0x00000000 );

		const __m128i buffer_converter< PixelFormat::eYUY2, PixelFormat::eA8R8G8B8 >::amask	= _mm_set1_epi32( 0x000000FF );
		const __m128i buffer_converter< PixelFormat::eYUY2, PixelFormat::eA8R8G8B8 >::umask	= _mm_set1_epi32( 0x0000FF00 );
		const __m128i buffer_converter< PixelFormat::eYUY2, PixelFormat::eA8R8G8B8 >::vmask	= _mm_set1_epi32( 0xFF000000 );
		const __m128i buffer_converter< PixelFormat::eYUY2, PixelFormat::eA8R8G8B8 >::ymask	= _mm_set1_epi32( 0x00FF00FF );
		const __m128i buffer_converter< PixelFormat::eYUY2, PixelFormat::eA8R8G8B8 >::uvsub	= _mm_set1_epi32( 0x00800080 );
		const __m128i buffer_converter< PixelFormat::eYUY2, PixelFormat::eA8R8G8B8 >::ysub	= _mm_set1_epi32( 0x00100010 );
		const __m128i buffer_converter< PixelFormat::eYUY2, PixelFormat::eA8R8G8B8 >::facy	= _mm_set1_epi32( 0x00400040 );// 1 * 64
		const __m128i buffer_converter< PixelFormat::eYUY2, PixelFormat::eA8R8G8B8 >::facrv	= _mm_set1_epi32( 0x00710071 );// 1.772 * 64
		const __m128i buffer_converter< PixelFormat::eYUY2, PixelFormat::eA8R8G8B8 >::facgu	= _mm_set1_epi32( 0x002D002D );// 0.714 * 64
		const __m128i buffer_converter< PixelFormat::eYUY2, PixelFormat::eA8R8G8B8 >::facgv	= _mm_set1_epi32( 0x00160016 );// 0.344 * 64
		const __m128i buffer_converter< PixelFormat::eYUY2, PixelFormat::eA8R8G8B8 >::facbu	= _mm_set1_epi32( 0x00590059 );// 1.402 * 64
		const __m128i buffer_converter< PixelFormat::eYUY2, PixelFormat::eA8R8G8B8 >::zero	= _mm_set1_epi32( 0x00000000 );
	}

#endif

	namespace PF
	{
		PixelFormat GetPFWithoutAlpha( PixelFormat p_format )
		{
			PixelFormat result = PixelFormat::eCount;

			switch ( p_format )
			{
			case PixelFormat::eA8L8:
				result = pixel_definitions< PixelFormat::eA8L8 >::NoAlphaPF;
				break;

			case PixelFormat::eAL16F32F:
				result = pixel_definitions< PixelFormat::eAL16F32F >::NoAlphaPF;
				break;

			case PixelFormat::eAL32F:
				result = pixel_definitions< PixelFormat::eAL32F >::NoAlphaPF;
				break;

			case PixelFormat::eA1R5G5B5:
				result = pixel_definitions< PixelFormat::eA1R5G5B5 >::NoAlphaPF;
				break;

			case PixelFormat::eA4R4G4B4:
				result = pixel_definitions< PixelFormat::eA4R4G4B4 >::NoAlphaPF;
				break;

			case PixelFormat::eA8R8G8B8:
				result = pixel_definitions< PixelFormat::eA8R8G8B8 >::NoAlphaPF;
				break;

			case PixelFormat::eA8B8G8R8:
				result = pixel_definitions< PixelFormat::eA8B8G8R8 >::NoAlphaPF;
				break;

			case PixelFormat::eRGBA16F32F:
				result = pixel_definitions< PixelFormat::eRGBA16F32F >::NoAlphaPF;
				break;

			case PixelFormat::eRGBA32F:
				result = pixel_definitions< PixelFormat::eRGBA32F >::NoAlphaPF;
				break;

			case PixelFormat::eDXTC3:
				result = pixel_definitions< PixelFormat::eDXTC3 >::NoAlphaPF;
				break;

			case PixelFormat::eDXTC5:
				result = pixel_definitions< PixelFormat::eDXTC5 >::NoAlphaPF;
				break;

			default:
				result = p_format;
				break;
			}

			return result;
		}

		bool HasAlpha( PixelFormat p_format )
		{
			bool result = false;

			switch ( p_format )
			{
			case PixelFormat::eL8:
				result = pixel_definitions< PixelFormat::eL8 >::Alpha;
				break;

			case PixelFormat::eL16F32F:
				result = pixel_definitions< PixelFormat::eL16F32F	>::Alpha;
				break;

			case PixelFormat::eL32F:
				result = pixel_definitions< PixelFormat::eL32F >::Alpha;
				break;

			case PixelFormat::eA8L8:
				result = pixel_definitions< PixelFormat::eA8L8 >::Alpha;
				break;

			case PixelFormat::eAL16F32F:
				result = pixel_definitions< PixelFormat::eAL16F32F >::Alpha;
				break;

			case PixelFormat::eAL32F:
				result = pixel_definitions< PixelFormat::eAL32F >::Alpha;
				break;

			case PixelFormat::eA1R5G5B5:
				result = pixel_definitions< PixelFormat::eA1R5G5B5 >::Alpha;
				break;

			case PixelFormat::eA4R4G4B4:
				result = pixel_definitions< PixelFormat::eA4R4G4B4 >::Alpha;
				break;

			case PixelFormat::eR5G6B5:
				result = pixel_definitions< PixelFormat::eR5G6B5 >::Alpha;
				break;

			case PixelFormat::eR8G8B8:
				result = pixel_definitions< PixelFormat::eR8G8B8 >::Alpha;
				break;

			case PixelFormat::eB8G8R8:
				result = pixel_definitions< PixelFormat::eB8G8R8 >::Alpha;
				break;

			case PixelFormat::eA8R8G8B8:
				result = pixel_definitions< PixelFormat::eA8R8G8B8 >::Alpha;
				break;

			case PixelFormat::eA8B8G8R8:
				result = pixel_definitions< PixelFormat::eA8B8G8R8 >::Alpha;
				break;

			case PixelFormat::eRGB16F:
				result = pixel_definitions< PixelFormat::eRGB16F >::Alpha;
				break;

			case PixelFormat::eRGBA16F:
				result = pixel_definitions< PixelFormat::eRGBA16F >::Alpha;
				break;

			case PixelFormat::eRGB16F32F:
				result = pixel_definitions< PixelFormat::eRGB16F32F >::Alpha;
				break;

			case PixelFormat::eRGBA16F32F:
				result = pixel_definitions< PixelFormat::eRGBA16F32F >::Alpha;
				break;

			case PixelFormat::eRGB32F:
				result = pixel_definitions< PixelFormat::eRGB32F >::Alpha;
				break;

			case PixelFormat::eRGBA32F:
				result = pixel_definitions< PixelFormat::eRGBA32F >::Alpha;
				break;

			case PixelFormat::eDXTC1:
				result = pixel_definitions< PixelFormat::eDXTC1 >::Alpha;
				break;

			case PixelFormat::eDXTC3:
				result = pixel_definitions< PixelFormat::eDXTC3 >::Alpha;
				break;

			case PixelFormat::eDXTC5:
				result = pixel_definitions< PixelFormat::eDXTC5 >::Alpha;
				break;

			case PixelFormat::eYUY2:
				result = pixel_definitions< PixelFormat::eYUY2 >::Alpha;
				break;

			case PixelFormat::eD16:
				result = pixel_definitions< PixelFormat::eD16 >::Alpha;
				break;

			case PixelFormat::eD24:
				result = pixel_definitions< PixelFormat::eD24 >::Alpha;
				break;

			case PixelFormat::eD24S8:
				result = pixel_definitions< PixelFormat::eD24S8 >::Alpha;
				break;

			case PixelFormat::eD32:
				result = pixel_definitions< PixelFormat::eD32 >::Alpha;
				break;

			case PixelFormat::eD32F:
				result = pixel_definitions< PixelFormat::eD32F >::Alpha;
				break;

			case PixelFormat::eD32FS8:
				result = pixel_definitions< PixelFormat::eD32FS8 >::Alpha;
				break;

			case PixelFormat::eS1:
				result = pixel_definitions< PixelFormat::eS1 >::Alpha;
				break;

			case PixelFormat::eS8:
				result = pixel_definitions< PixelFormat::eS8 >::Alpha;
				break;

			default:
				result = false;
				break;
			}

			return result;
		}

		bool IsCompressed( PixelFormat p_format )
		{
			bool result = false;

			switch ( p_format )
			{
			case PixelFormat::eL8:
				result = pixel_definitions< PixelFormat::eL8 >::Compressed;
				break;

			case PixelFormat::eL16F32F:
				result = pixel_definitions< PixelFormat::eL16F32F >::Compressed;
				break;

			case PixelFormat::eL32F:
				result = pixel_definitions< PixelFormat::eL32F >::Compressed;
				break;

			case PixelFormat::eA8L8:
				result = pixel_definitions< PixelFormat::eA8L8 >::Compressed;
				break;

			case PixelFormat::eAL16F32F:
				result = pixel_definitions< PixelFormat::eAL16F32F >::Compressed;
				break;

			case PixelFormat::eAL32F:
				result = pixel_definitions< PixelFormat::eAL32F >::Compressed;
				break;

			case PixelFormat::eA1R5G5B5:
				result = pixel_definitions< PixelFormat::eA1R5G5B5 >::Compressed;
				break;

			case PixelFormat::eA4R4G4B4:
				result = pixel_definitions< PixelFormat::eA4R4G4B4 >::Compressed;
				break;

			case PixelFormat::eR5G6B5:
				result = pixel_definitions< PixelFormat::eR5G6B5 >::Compressed;
				break;

			case PixelFormat::eR8G8B8:
				result = pixel_definitions< PixelFormat::eR8G8B8 >::Compressed;
				break;

			case PixelFormat::eB8G8R8:
				result = pixel_definitions< PixelFormat::eB8G8R8 >::Compressed;
				break;

			case PixelFormat::eA8R8G8B8:
				result = pixel_definitions< PixelFormat::eA8R8G8B8 >::Compressed;
				break;

			case PixelFormat::eA8B8G8R8:
				result = pixel_definitions< PixelFormat::eA8B8G8R8 >::Compressed;
				break;

			case PixelFormat::eRGB16F:
				result = pixel_definitions< PixelFormat::eRGB16F >::Compressed;
				break;

			case PixelFormat::eRGBA16F:
				result = pixel_definitions< PixelFormat::eRGBA16F >::Compressed;
				break;

			case PixelFormat::eRGB16F32F:
				result = pixel_definitions< PixelFormat::eRGB16F32F >::Compressed;
				break;

			case PixelFormat::eRGBA16F32F:
				result = pixel_definitions< PixelFormat::eRGBA16F32F >::Compressed;
				break;

			case PixelFormat::eRGB32F:
				result = pixel_definitions< PixelFormat::eRGB32F >::Compressed;
				break;

			case PixelFormat::eRGBA32F:
				result = pixel_definitions< PixelFormat::eRGBA32F >::Compressed;
				break;

			case PixelFormat::eDXTC1:
				result = pixel_definitions< PixelFormat::eDXTC1 >::Compressed;
				break;

			case PixelFormat::eDXTC3:
				result = pixel_definitions< PixelFormat::eDXTC3 >::Compressed;
				break;

			case PixelFormat::eDXTC5:
				result = pixel_definitions< PixelFormat::eDXTC5 >::Compressed;
				break;

			case PixelFormat::eYUY2:
				result = pixel_definitions< PixelFormat::eYUY2 >::Compressed;
				break;

			case PixelFormat::eD16:
				result = pixel_definitions< PixelFormat::eD16 >::Compressed;
				break;

			case PixelFormat::eD24:
				result = pixel_definitions< PixelFormat::eD24 >::Compressed;
				break;

			case PixelFormat::eD24S8:
				result = pixel_definitions< PixelFormat::eD24S8 >::Compressed;
				break;

			case PixelFormat::eD32:
				result = pixel_definitions< PixelFormat::eD32 >::Compressed;
				break;

			case PixelFormat::eD32F:
				result = pixel_definitions< PixelFormat::eD32F >::Compressed;
				break;

			case PixelFormat::eD32FS8:
				result = pixel_definitions< PixelFormat::eD32FS8 >::Compressed;
				break;

			case PixelFormat::eS1:
				result = pixel_definitions< PixelFormat::eS1 >::Compressed;
				break;

			case PixelFormat::eS8:
				result = pixel_definitions< PixelFormat::eS8 >::Compressed;
				break;

			default:
				result = false;
				break;
			}

			return result;
		}

		PxBufferBaseSPtr ExtractAlpha( PxBufferBaseSPtr & p_pSrc )
		{
			PxBufferBaseSPtr result;

			if ( HasAlpha( p_pSrc->format() ) )
			{
				result = PxBufferBase::create( p_pSrc->dimensions(), PixelFormat::eA8L8, p_pSrc->const_ptr(), p_pSrc->format() );
				uint8_t * pBuffer = result->ptr();

				for ( uint32_t i = 0; i < result->size(); i += 2 )
				{
					pBuffer[0] = pBuffer[1];
					pBuffer++;
					pBuffer++;
				}

				result = PxBufferBase::create( p_pSrc->dimensions(), PixelFormat::eL8, result->const_ptr(), result->format() );
				p_pSrc = PxBufferBase::create( p_pSrc->dimensions(), GetPFWithoutAlpha( p_pSrc->format() ), p_pSrc->const_ptr(), p_pSrc->format() );
			}

			return result;
		}

		void ReduceToAlpha( PxBufferBaseSPtr & p_pSrc )
		{
			if ( HasAlpha( p_pSrc->format() ) )
			{
				if ( p_pSrc->format() != PixelFormat::eA8L8 )
				{
					p_pSrc = PxBufferBase::create( p_pSrc->dimensions(), PixelFormat::eA8L8, p_pSrc->const_ptr(), p_pSrc->format() );
				}

				uint8_t * pBuffer = p_pSrc->ptr();

				for ( uint32_t i = 0; i < p_pSrc->size(); i += 2 )
				{
					pBuffer[0] = pBuffer[1];
					pBuffer++;
					pBuffer++;
				}
			}

			p_pSrc = PxBufferBase::create( p_pSrc->dimensions(), PixelFormat::eL8, p_pSrc->const_ptr(), p_pSrc->format() );
		}

		uint8_t GetBytesPerPixel( PixelFormat p_format )
		{
			uint8_t result = 0;

			switch ( p_format )
			{
			case PixelFormat::eL8:
				result = pixel_definitions< PixelFormat::eL8 >::Size;
				break;

			case PixelFormat::eL16F32F:
				result = pixel_definitions<	PixelFormat::eL16F32F >::Size;
				break;

			case PixelFormat::eL32F:
				result = pixel_definitions< PixelFormat::eL32F >::Size;
				break;

			case PixelFormat::eA8L8:
				result = pixel_definitions< PixelFormat::eA8L8 >::Size;
				break;

			case PixelFormat::eAL16F32F:
				result = pixel_definitions<	PixelFormat::eAL16F32F >::Size;
				break;

			case PixelFormat::eAL32F:
				result = pixel_definitions< PixelFormat::eAL32F >::Size;
				break;

			case PixelFormat::eA1R5G5B5:
				result = pixel_definitions< PixelFormat::eA1R5G5B5 >::Size;
				break;

			case PixelFormat::eA4R4G4B4:
				result = pixel_definitions< PixelFormat::eA4R4G4B4 >::Size;
				break;

			case PixelFormat::eR5G6B5:
				result = pixel_definitions< PixelFormat::eR5G6B5 >::Size;
				break;

			case PixelFormat::eR8G8B8:
				result = pixel_definitions< PixelFormat::eR8G8B8 >::Size;
				break;

			case PixelFormat::eB8G8R8:
				result = pixel_definitions< PixelFormat::eB8G8R8 >::Size;
				break;

			case PixelFormat::eA8R8G8B8:
				result = pixel_definitions< PixelFormat::eA8R8G8B8 >::Size;
				break;

			case PixelFormat::eA8B8G8R8:
				result = pixel_definitions< PixelFormat::eA8B8G8R8 >::Size;
				break;

			case PixelFormat::eRGB16F:
				result = pixel_definitions<	PixelFormat::eRGB16F >::Size;
				break;

			case PixelFormat::eRGBA16F:
				result = pixel_definitions<	PixelFormat::eRGBA16F >::Size;
				break;

			case PixelFormat::eRGB16F32F:
				result = pixel_definitions<	PixelFormat::eRGB16F32F >::Size;
				break;

			case PixelFormat::eRGBA16F32F:
				result = pixel_definitions<	PixelFormat::eRGBA16F32F >::Size;
				break;

			case PixelFormat::eRGB32F:
				result = pixel_definitions< PixelFormat::eRGB32F >::Size;
				break;

			case PixelFormat::eRGBA32F:
				result = pixel_definitions< PixelFormat::eRGBA32F >::Size;
				break;

			case PixelFormat::eDXTC1:
				result = pixel_definitions<	PixelFormat::eDXTC1 >::Size;
				break;

			case PixelFormat::eDXTC3:
				result = pixel_definitions<	PixelFormat::eDXTC3 >::Size;
				break;

			case PixelFormat::eDXTC5:
				result = pixel_definitions<	PixelFormat::eDXTC5 >::Size;
				break;

			case PixelFormat::eYUY2:
				result = pixel_definitions<	PixelFormat::eYUY2 >::Size;
				break;

			case PixelFormat::eD16:
				result = pixel_definitions< PixelFormat::eD16 >::Size;
				break;

			case PixelFormat::eD24:
				result = pixel_definitions< PixelFormat::eD24 >::Size;
				break;

			case PixelFormat::eD24S8:
				result = pixel_definitions< PixelFormat::eD24S8 >::Size;
				break;

			case PixelFormat::eD32:
				result = pixel_definitions< PixelFormat::eD32 >::Size;
				break;

			case PixelFormat::eD32F:
				result = pixel_definitions< PixelFormat::eD32F >::Size;
				break;

			case PixelFormat::eD32FS8:
				result = pixel_definitions< PixelFormat::eD32FS8 >::Size;
				break;

			case PixelFormat::eS1:
				result = pixel_definitions<	PixelFormat::eS1 >::Size;
				break;

			case PixelFormat::eS8:
				result = pixel_definitions< PixelFormat::eS8 >::Size;
				break;

			default:
				FAILURE( "Unsupported pixel format" );
				break;
			}

			return result;
		}

		uint8_t GetComponentsCount( PixelFormat p_format )
		{
			uint8_t result = 0;

			switch ( p_format )
			{
			case PixelFormat::eL8:
				result = pixel_definitions< PixelFormat::eL8 >::Count;
				break;

			case PixelFormat::eL16F32F:
				result = pixel_definitions<	PixelFormat::eL16F32F >::Count;
				break;

			case PixelFormat::eL32F:
				result = pixel_definitions< PixelFormat::eL32F >::Count;
				break;

			case PixelFormat::eA8L8:
				result = pixel_definitions< PixelFormat::eA8L8 >::Count;
				break;

			case PixelFormat::eAL16F32F:
				result = pixel_definitions<	PixelFormat::eAL16F32F >::Count;
				break;

			case PixelFormat::eAL32F:
				result = pixel_definitions< PixelFormat::eAL32F >::Count;
				break;

			case PixelFormat::eA1R5G5B5:
				result = pixel_definitions< PixelFormat::eA1R5G5B5 >::Count;
				break;

			case PixelFormat::eA4R4G4B4:
				result = pixel_definitions< PixelFormat::eA4R4G4B4 >::Count;
				break;

			case PixelFormat::eR5G6B5:
				result = pixel_definitions< PixelFormat::eR5G6B5 >::Count;
				break;

			case PixelFormat::eR8G8B8:
				result = pixel_definitions< PixelFormat::eR8G8B8 >::Count;
				break;

			case PixelFormat::eB8G8R8:
				result = pixel_definitions< PixelFormat::eB8G8R8 >::Count;
				break;

			case PixelFormat::eA8R8G8B8:
				result = pixel_definitions< PixelFormat::eA8R8G8B8 >::Count;
				break;

			case PixelFormat::eA8B8G8R8:
				result = pixel_definitions< PixelFormat::eA8B8G8R8 >::Count;
				break;

			case PixelFormat::eRGB16F:
				result = pixel_definitions<	PixelFormat::eRGB16F >::Count;
				break;

			case PixelFormat::eRGBA16F:
				result = pixel_definitions<	PixelFormat::eRGBA16F >::Count;
				break;

			case PixelFormat::eRGB16F32F:
				result = pixel_definitions<	PixelFormat::eRGB16F32F >::Count;
				break;

			case PixelFormat::eRGBA16F32F:
				result = pixel_definitions<	PixelFormat::eRGBA16F32F >::Count;
				break;

			case PixelFormat::eRGB32F:
				result = pixel_definitions< PixelFormat::eRGB32F >::Count;
				break;

			case PixelFormat::eRGBA32F:
				result = pixel_definitions< PixelFormat::eRGBA32F >::Count;
				break;

			case PixelFormat::eDXTC1:
				result = pixel_definitions<	PixelFormat::eDXTC1 >::Count;
				break;

			case PixelFormat::eDXTC3:
				result = pixel_definitions<	PixelFormat::eDXTC3 >::Count;
				break;

			case PixelFormat::eDXTC5:
				result = pixel_definitions<	PixelFormat::eDXTC5 >::Count;
				break;

			case PixelFormat::eYUY2:
				result = pixel_definitions<	PixelFormat::eYUY2 >::Count;
				break;

			case PixelFormat::eD16:
				result = pixel_definitions< PixelFormat::eD16 >::Count;
				break;

			case PixelFormat::eD24:
				result = pixel_definitions< PixelFormat::eD24 >::Count;
				break;

			case PixelFormat::eD24S8:
				result = pixel_definitions< PixelFormat::eD24S8 >::Count;
				break;

			case PixelFormat::eD32:
				result = pixel_definitions< PixelFormat::eD32 >::Count;
				break;

			case PixelFormat::eD32F:
				result = pixel_definitions< PixelFormat::eD32F >::Count;
				break;

			case PixelFormat::eD32FS8:
				result = pixel_definitions< PixelFormat::eD32FS8 >::Count;
				break;

			case PixelFormat::eS1:
				result = pixel_definitions<	PixelFormat::eS1 >::Count;
				break;

			case PixelFormat::eS8:
				result = pixel_definitions< PixelFormat::eS8 >::Count;
				break;

			default:
				FAILURE( "Unsupported pixel format" );
				break;
			}

			return result;
		}

		void ConvertPixel( PixelFormat p_eSrcFmt, uint8_t const *& p_pSrc, PixelFormat p_eDestFmt, uint8_t *& p_pDest )
		{
			switch ( p_eSrcFmt )
			{
			case PixelFormat::eL8:
				pixel_definitions< PixelFormat::eL8 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eL16F32F:
				pixel_definitions< PixelFormat::eL16F32F >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eL32F:
				pixel_definitions< PixelFormat::eL32F >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eA8L8:
				pixel_definitions< PixelFormat::eA8L8 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eAL16F32F:
				pixel_definitions< PixelFormat::eAL16F32F >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eAL32F:
				pixel_definitions< PixelFormat::eAL32F >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eA1R5G5B5:
				pixel_definitions< PixelFormat::eA1R5G5B5 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eA4R4G4B4:
				pixel_definitions< PixelFormat::eA4R4G4B4 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eR5G6B5:
				pixel_definitions< PixelFormat::eR5G6B5 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eR8G8B8:
				pixel_definitions< PixelFormat::eR8G8B8 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eB8G8R8:
				pixel_definitions< PixelFormat::eB8G8R8 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eA8R8G8B8:
				pixel_definitions< PixelFormat::eA8R8G8B8 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eA8B8G8R8:
				pixel_definitions< PixelFormat::eA8B8G8R8 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eRGB16F:
				pixel_definitions< PixelFormat::eRGB16F >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eRGBA16F:
				pixel_definitions< PixelFormat::eRGBA16F >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eRGB16F32F:
				pixel_definitions< PixelFormat::eRGB16F32F >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eRGBA16F32F:
				pixel_definitions< PixelFormat::eRGBA16F32F >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eRGB32F:
				pixel_definitions< PixelFormat::eRGB32F >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eRGBA32F:
				pixel_definitions< PixelFormat::eRGBA32F >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eDXTC1:
				pixel_definitions< PixelFormat::eDXTC1 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eDXTC3:
				pixel_definitions< PixelFormat::eDXTC3 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eDXTC5:
				pixel_definitions< PixelFormat::eDXTC5 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eYUY2:
				pixel_definitions< PixelFormat::eYUY2 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eD16:
				pixel_definitions< PixelFormat::eD16 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eD24:
				pixel_definitions< PixelFormat::eD24 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eD24S8:
				pixel_definitions< PixelFormat::eD24S8 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eD32:
				pixel_definitions< PixelFormat::eD32 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eD32F:
				pixel_definitions< PixelFormat::eD32F >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eD32FS8:
				pixel_definitions< PixelFormat::eD32FS8 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eS1:
				pixel_definitions< PixelFormat::eS1 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eS8:
				pixel_definitions< PixelFormat::eS8 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			default:
				FAILURE( "Unsupported pixel format" );
				break;
			}
		}

		void ConvertBuffer( PixelFormat p_eSrcFormat, uint8_t const * p_pSrcBuffer, uint32_t p_uiSrcSize, PixelFormat p_eDstFormat, uint8_t * p_pDstBuffer, uint32_t p_uiDstSize )
		{
			switch ( p_eSrcFormat )
			{
			case PixelFormat::eL8:
				pixel_definitions< PixelFormat::eL8 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eL16F32F:
				pixel_definitions< PixelFormat::eL16F32F >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eL32F:
				pixel_definitions< PixelFormat::eL32F >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eA8L8:
				pixel_definitions< PixelFormat::eA8L8 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eAL16F32F:
				pixel_definitions< PixelFormat::eAL16F32F >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eAL32F:
				pixel_definitions< PixelFormat::eAL32F >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eA1R5G5B5:
				pixel_definitions< PixelFormat::eA1R5G5B5 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eA4R4G4B4:
				pixel_definitions< PixelFormat::eA4R4G4B4 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eR5G6B5:
				pixel_definitions< PixelFormat::eR5G6B5 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eR8G8B8:
				pixel_definitions< PixelFormat::eR8G8B8 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eB8G8R8:
				pixel_definitions< PixelFormat::eB8G8R8 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eA8R8G8B8:
				pixel_definitions< PixelFormat::eA8R8G8B8 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eA8B8G8R8:
				pixel_definitions< PixelFormat::eA8B8G8R8 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eRGB16F:
				pixel_definitions< PixelFormat::eRGB16F >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eRGBA16F:
				pixel_definitions< PixelFormat::eRGBA16F >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eRGB16F32F:
				pixel_definitions< PixelFormat::eRGB16F32F >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eRGBA16F32F:
				pixel_definitions< PixelFormat::eRGBA16F32F >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eRGB32F:
				pixel_definitions< PixelFormat::eRGB32F >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eRGBA32F:
				pixel_definitions< PixelFormat::eRGBA32F >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eDXTC1:
				pixel_definitions< PixelFormat::eDXTC1 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eDXTC3:
				pixel_definitions< PixelFormat::eDXTC3 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eDXTC5:
				pixel_definitions< PixelFormat::eDXTC5 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eYUY2:
				pixel_definitions< PixelFormat::eYUY2 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eD16:
				pixel_definitions< PixelFormat::eD16 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eD24:
				pixel_definitions< PixelFormat::eD24 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eD24S8:
				pixel_definitions< PixelFormat::eD24S8 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eD32:
				pixel_definitions< PixelFormat::eD32 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eD32F:
				pixel_definitions< PixelFormat::eD32F >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eD32FS8:
				pixel_definitions< PixelFormat::eD32FS8 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eS1:
				pixel_definitions< PixelFormat::eS1 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eS8:
				pixel_definitions< PixelFormat::eS8 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			default:
				FAILURE( "Unsupported pixel format" );
				break;
			}
		}

		PixelFormat GetFormatByName( String const & p_strFormat )
		{
			PixelFormat result = PixelFormat::eCount;

			for ( int i = 0; i < int( result ); ++i )
			{
				switch ( PixelFormat( i ) )
				{
				case PixelFormat::eL8:
					result = ( p_strFormat == pixel_definitions< PixelFormat::eL8 >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eL16F32F:
					result = ( p_strFormat == pixel_definitions< PixelFormat::eL16F32F >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eL32F:
					result = ( p_strFormat == pixel_definitions< PixelFormat::eL32F >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eA8L8:
					result = ( p_strFormat == pixel_definitions< PixelFormat::eA8L8 >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eAL16F32F:
					result = ( p_strFormat == pixel_definitions< PixelFormat::eAL16F32F >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eAL32F:
					result = ( p_strFormat == pixel_definitions< PixelFormat::eAL32F >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eA1R5G5B5:
					result = ( p_strFormat == pixel_definitions< PixelFormat::eA1R5G5B5 >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eA4R4G4B4:
					result = ( p_strFormat == pixel_definitions< PixelFormat::eA4R4G4B4 >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eR5G6B5:
					result = ( p_strFormat == pixel_definitions< PixelFormat::eR5G6B5 >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eR8G8B8:
					result = ( p_strFormat == pixel_definitions< PixelFormat::eR8G8B8 >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eB8G8R8:
					result = ( p_strFormat == pixel_definitions< PixelFormat::eB8G8R8 >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eA8R8G8B8:
					result = ( p_strFormat == pixel_definitions< PixelFormat::eA8R8G8B8 >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eA8B8G8R8:
					result = ( p_strFormat == pixel_definitions< PixelFormat::eA8B8G8R8 >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eRGB16F:
					result = ( p_strFormat == pixel_definitions< PixelFormat::eRGB16F >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eRGBA16F:
					result = ( p_strFormat == pixel_definitions< PixelFormat::eRGBA16F >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eRGB16F32F:
					result = ( p_strFormat == pixel_definitions< PixelFormat::eRGB16F32F >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eRGBA16F32F:
					result = ( p_strFormat == pixel_definitions< PixelFormat::eRGBA16F32F >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eRGB32F:
					result = ( p_strFormat == pixel_definitions< PixelFormat::eRGB32F >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eRGBA32F:
					result = ( p_strFormat == pixel_definitions< PixelFormat::eRGBA32F >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eDXTC1:
					result = ( p_strFormat == pixel_definitions< PixelFormat::eDXTC1 >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eDXTC3:
					result = ( p_strFormat == pixel_definitions< PixelFormat::eDXTC3 >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eDXTC5:
					result = ( p_strFormat == pixel_definitions< PixelFormat::eDXTC5 >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eYUY2:
					result = ( p_strFormat == pixel_definitions< PixelFormat::eYUY2 >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eD16:
					result = ( p_strFormat == pixel_definitions< PixelFormat::eD16 >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eD24:
					result = ( p_strFormat == pixel_definitions< PixelFormat::eD24 >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eD24S8:
					result = ( p_strFormat == pixel_definitions< PixelFormat::eD24S8 >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eD32:
					result = ( p_strFormat == pixel_definitions< PixelFormat::eD32 >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eD32F:
					result = ( p_strFormat == pixel_definitions< PixelFormat::eD32F >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eD32FS8:
					result = ( p_strFormat == pixel_definitions< PixelFormat::eD32FS8 >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eS1:
					result = ( p_strFormat == pixel_definitions< PixelFormat::eS1 >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eS8:
					result = ( p_strFormat == pixel_definitions< PixelFormat::eS8 >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				default:
					FAILURE( "Unsupported pixel format" );
					break;
				}
			}

			return result;
		}

		String GetFormatName( PixelFormat p_format )
		{
			String result;

			switch ( p_format )
			{
			case PixelFormat::eL8:
				result = pixel_definitions< PixelFormat::eL8 >::to_str();
				break;

			case PixelFormat::eL16F32F:
				result = pixel_definitions< PixelFormat::eL16F32F >::to_str();
				break;

			case PixelFormat::eL32F:
				result = pixel_definitions< PixelFormat::eL32F >::to_str();
				break;

			case PixelFormat::eA8L8:
				result = pixel_definitions< PixelFormat::eA8L8 >::to_str();
				break;

			case PixelFormat::eAL16F32F:
				result = pixel_definitions< PixelFormat::eAL16F32F >::to_str();
				break;

			case PixelFormat::eAL32F:
				result = pixel_definitions< PixelFormat::eAL32F >::to_str();
				break;

			case PixelFormat::eA1R5G5B5:
				result = pixel_definitions< PixelFormat::eA1R5G5B5 >::to_str();
				break;

			case PixelFormat::eA4R4G4B4:
				result = pixel_definitions< PixelFormat::eA4R4G4B4 >::to_str();
				break;

			case PixelFormat::eR5G6B5:
				result = pixel_definitions< PixelFormat::eR5G6B5 >::to_str();
				break;

			case PixelFormat::eR8G8B8:
				result = pixel_definitions< PixelFormat::eR8G8B8 >::to_str();
				break;

			case PixelFormat::eB8G8R8:
				result = pixel_definitions< PixelFormat::eB8G8R8 >::to_str();
				break;

			case PixelFormat::eA8R8G8B8:
				result = pixel_definitions< PixelFormat::eA8R8G8B8 >::to_str();
				break;

			case PixelFormat::eA8B8G8R8:
				result = pixel_definitions< PixelFormat::eA8B8G8R8 >::to_str();
				break;

			case PixelFormat::eRGB16F:
				result = pixel_definitions< PixelFormat::eRGB16F >::to_str();
				break;

			case PixelFormat::eRGBA16F:
				result = pixel_definitions< PixelFormat::eRGBA16F >::to_str();
				break;

			case PixelFormat::eRGB16F32F:
				result = pixel_definitions< PixelFormat::eRGB16F32F >::to_str();
				break;

			case PixelFormat::eRGBA16F32F:
				result = pixel_definitions< PixelFormat::eRGBA16F32F >::to_str();
				break;

			case PixelFormat::eRGB32F:
				result = pixel_definitions< PixelFormat::eRGB32F >::to_str();
				break;

			case PixelFormat::eRGBA32F:
				result = pixel_definitions< PixelFormat::eRGBA32F >::to_str();
				break;

			case PixelFormat::eDXTC1:
				result = pixel_definitions< PixelFormat::eDXTC1 >::to_str();
				break;

			case PixelFormat::eDXTC3:
				result = pixel_definitions< PixelFormat::eDXTC3 >::to_str();
				break;

			case PixelFormat::eDXTC5:
				result = pixel_definitions< PixelFormat::eDXTC5 >::to_str();
				break;

			case PixelFormat::eYUY2:
				result = pixel_definitions< PixelFormat::eYUY2 >::to_str();
				break;

			case PixelFormat::eD16:
				result = pixel_definitions< PixelFormat::eD16 >::to_str();
				break;

			case PixelFormat::eD24:
				result = pixel_definitions< PixelFormat::eD24 >::to_str();
				break;

			case PixelFormat::eD24S8:
				result = pixel_definitions< PixelFormat::eD24S8 >::to_str();
				break;

			case PixelFormat::eD32:
				result = pixel_definitions< PixelFormat::eD32 >::to_str();
				break;

			case PixelFormat::eD32F:
				result = pixel_definitions< PixelFormat::eD32F >::to_str();
				break;

			case PixelFormat::eD32FS8:
				result = pixel_definitions< PixelFormat::eD32FS8 >::to_str();
				break;

			case PixelFormat::eS1:
				result = pixel_definitions< PixelFormat::eS1 >::to_str();
				break;

			case PixelFormat::eS8:
				result = pixel_definitions< PixelFormat::eS8 >::to_str();
				break;

			default:
				FAILURE( "Unsupported pixel format" );
				break;
			}

			return result;
		}
	}
}
