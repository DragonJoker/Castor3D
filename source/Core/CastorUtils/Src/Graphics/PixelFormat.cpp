#include "PixelFormat.hpp"
#include "PixelBuffer.hpp"

namespace Castor
{
#if defined( _X64 )

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
			PixelFormat l_return = PixelFormat::eCount;

			switch ( p_format )
			{
			case PixelFormat::eA8L8:
				l_return = pixel_definitions< PixelFormat::eA8L8 >::NoAlphaPF;
				break;

			case PixelFormat::eAL16F32F:
				l_return = pixel_definitions< PixelFormat::eAL16F32F >::NoAlphaPF;
				break;

			case PixelFormat::eAL32F:
				l_return = pixel_definitions< PixelFormat::eAL32F >::NoAlphaPF;
				break;

			case PixelFormat::eA1R5G5B5:
				l_return = pixel_definitions< PixelFormat::eA1R5G5B5 >::NoAlphaPF;
				break;

			case PixelFormat::eA4R4G4B4:
				l_return = pixel_definitions< PixelFormat::eA4R4G4B4 >::NoAlphaPF;
				break;

			case PixelFormat::eA8R8G8B8:
				l_return = pixel_definitions< PixelFormat::eA8R8G8B8 >::NoAlphaPF;
				break;

			case PixelFormat::eA8B8G8R8:
				l_return = pixel_definitions< PixelFormat::eA8B8G8R8 >::NoAlphaPF;
				break;

			case PixelFormat::eRGBA16F32F:
				l_return = pixel_definitions< PixelFormat::eRGBA16F32F >::NoAlphaPF;
				break;

			case PixelFormat::eRGBA32F:
				l_return = pixel_definitions< PixelFormat::eRGBA32F >::NoAlphaPF;
				break;

			case PixelFormat::eDXTC3:
				l_return = pixel_definitions< PixelFormat::eDXTC3 >::NoAlphaPF;
				break;

			case PixelFormat::eDXTC5:
				l_return = pixel_definitions< PixelFormat::eDXTC5 >::NoAlphaPF;
				break;

			default:
				l_return = p_format;
				break;
			}

			return l_return;
		}

		bool HasAlpha( PixelFormat p_format )
		{
			bool l_return = false;

			switch ( p_format )
			{
			case PixelFormat::eL8:
				l_return = pixel_definitions< PixelFormat::eL8 >::Alpha;
				break;

			case PixelFormat::eL16F32F:
				l_return = pixel_definitions< PixelFormat::eL16F32F	>::Alpha;
				break;

			case PixelFormat::eL32F:
				l_return = pixel_definitions< PixelFormat::eL32F >::Alpha;
				break;

			case PixelFormat::eA8L8:
				l_return = pixel_definitions< PixelFormat::eA8L8 >::Alpha;
				break;

			case PixelFormat::eAL16F32F:
				l_return = pixel_definitions< PixelFormat::eAL16F32F >::Alpha;
				break;

			case PixelFormat::eAL32F:
				l_return = pixel_definitions< PixelFormat::eAL32F >::Alpha;
				break;

			case PixelFormat::eA1R5G5B5:
				l_return = pixel_definitions< PixelFormat::eA1R5G5B5 >::Alpha;
				break;

			case PixelFormat::eA4R4G4B4:
				l_return = pixel_definitions< PixelFormat::eA4R4G4B4 >::Alpha;
				break;

			case PixelFormat::eR5G6B5:
				l_return = pixel_definitions< PixelFormat::eR5G6B5 >::Alpha;
				break;

			case PixelFormat::eR8G8B8:
				l_return = pixel_definitions< PixelFormat::eR8G8B8 >::Alpha;
				break;

			case PixelFormat::eB8G8R8:
				l_return = pixel_definitions< PixelFormat::eB8G8R8 >::Alpha;
				break;

			case PixelFormat::eA8R8G8B8:
				l_return = pixel_definitions< PixelFormat::eA8R8G8B8 >::Alpha;
				break;

			case PixelFormat::eA8B8G8R8:
				l_return = pixel_definitions< PixelFormat::eA8B8G8R8 >::Alpha;
				break;

			case PixelFormat::eRGB16F:
				l_return = pixel_definitions< PixelFormat::eRGB16F >::Alpha;
				break;

			case PixelFormat::eRGBA16F:
				l_return = pixel_definitions< PixelFormat::eRGBA16F >::Alpha;
				break;

			case PixelFormat::eRGB16F32F:
				l_return = pixel_definitions< PixelFormat::eRGB16F32F >::Alpha;
				break;

			case PixelFormat::eRGBA16F32F:
				l_return = pixel_definitions< PixelFormat::eRGBA16F32F >::Alpha;
				break;

			case PixelFormat::eRGB32F:
				l_return = pixel_definitions< PixelFormat::eRGB32F >::Alpha;
				break;

			case PixelFormat::eRGBA32F:
				l_return = pixel_definitions< PixelFormat::eRGBA32F >::Alpha;
				break;

			case PixelFormat::eDXTC1:
				l_return = pixel_definitions< PixelFormat::eDXTC1 >::Alpha;
				break;

			case PixelFormat::eDXTC3:
				l_return = pixel_definitions< PixelFormat::eDXTC3 >::Alpha;
				break;

			case PixelFormat::eDXTC5:
				l_return = pixel_definitions< PixelFormat::eDXTC5 >::Alpha;
				break;

			case PixelFormat::eYUY2:
				l_return = pixel_definitions< PixelFormat::eYUY2 >::Alpha;
				break;

			case PixelFormat::eD16:
				l_return = pixel_definitions< PixelFormat::eD16 >::Alpha;
				break;

			case PixelFormat::eD24:
				l_return = pixel_definitions< PixelFormat::eD24 >::Alpha;
				break;

			case PixelFormat::eD24S8:
				l_return = pixel_definitions< PixelFormat::eD24S8 >::Alpha;
				break;

			case PixelFormat::eD32:
				l_return = pixel_definitions< PixelFormat::eD32 >::Alpha;
				break;

			case PixelFormat::eD32F:
				l_return = pixel_definitions< PixelFormat::eD32F >::Alpha;
				break;

			case PixelFormat::eS1:
				l_return = pixel_definitions< PixelFormat::eS1 >::Alpha;
				break;

			case PixelFormat::eS8:
				l_return = pixel_definitions< PixelFormat::eS8 >::Alpha;
				break;

			default:
				l_return = false;
				break;
			}

			return l_return;
		}

		bool IsCompressed( PixelFormat p_format )
		{
			bool l_return = false;

			switch ( p_format )
			{
			case PixelFormat::eL8:
				l_return = pixel_definitions< PixelFormat::eL8 >::Compressed;
				break;

			case PixelFormat::eL16F32F:
				l_return = pixel_definitions< PixelFormat::eL16F32F >::Compressed;
				break;

			case PixelFormat::eL32F:
				l_return = pixel_definitions< PixelFormat::eL32F >::Compressed;
				break;

			case PixelFormat::eA8L8:
				l_return = pixel_definitions< PixelFormat::eA8L8 >::Compressed;
				break;

			case PixelFormat::eAL16F32F:
				l_return = pixel_definitions< PixelFormat::eAL16F32F >::Compressed;
				break;

			case PixelFormat::eAL32F:
				l_return = pixel_definitions< PixelFormat::eAL32F >::Compressed;
				break;

			case PixelFormat::eA1R5G5B5:
				l_return = pixel_definitions< PixelFormat::eA1R5G5B5 >::Compressed;
				break;

			case PixelFormat::eA4R4G4B4:
				l_return = pixel_definitions< PixelFormat::eA4R4G4B4 >::Compressed;
				break;

			case PixelFormat::eR5G6B5:
				l_return = pixel_definitions< PixelFormat::eR5G6B5 >::Compressed;
				break;

			case PixelFormat::eR8G8B8:
				l_return = pixel_definitions< PixelFormat::eR8G8B8 >::Compressed;
				break;

			case PixelFormat::eB8G8R8:
				l_return = pixel_definitions< PixelFormat::eB8G8R8 >::Compressed;
				break;

			case PixelFormat::eA8R8G8B8:
				l_return = pixel_definitions< PixelFormat::eA8R8G8B8 >::Compressed;
				break;

			case PixelFormat::eA8B8G8R8:
				l_return = pixel_definitions< PixelFormat::eA8B8G8R8 >::Compressed;
				break;

			case PixelFormat::eRGB16F:
				l_return = pixel_definitions< PixelFormat::eRGB16F >::Compressed;
				break;

			case PixelFormat::eRGBA16F:
				l_return = pixel_definitions< PixelFormat::eRGBA16F >::Compressed;
				break;

			case PixelFormat::eRGB16F32F:
				l_return = pixel_definitions< PixelFormat::eRGB16F32F >::Compressed;
				break;

			case PixelFormat::eRGBA16F32F:
				l_return = pixel_definitions< PixelFormat::eRGBA16F32F >::Compressed;
				break;

			case PixelFormat::eRGB32F:
				l_return = pixel_definitions< PixelFormat::eRGB32F >::Compressed;
				break;

			case PixelFormat::eRGBA32F:
				l_return = pixel_definitions< PixelFormat::eRGBA32F >::Compressed;
				break;

			case PixelFormat::eDXTC1:
				l_return = pixel_definitions< PixelFormat::eDXTC1 >::Compressed;
				break;

			case PixelFormat::eDXTC3:
				l_return = pixel_definitions< PixelFormat::eDXTC3 >::Compressed;
				break;

			case PixelFormat::eDXTC5:
				l_return = pixel_definitions< PixelFormat::eDXTC5 >::Compressed;
				break;

			case PixelFormat::eYUY2:
				l_return = pixel_definitions< PixelFormat::eYUY2 >::Compressed;
				break;

			case PixelFormat::eD16:
				l_return = pixel_definitions< PixelFormat::eD16 >::Compressed;
				break;

			case PixelFormat::eD24:
				l_return = pixel_definitions< PixelFormat::eD24 >::Compressed;
				break;

			case PixelFormat::eD24S8:
				l_return = pixel_definitions< PixelFormat::eD24S8 >::Compressed;
				break;

			case PixelFormat::eD32:
				l_return = pixel_definitions< PixelFormat::eD32 >::Compressed;
				break;

			case PixelFormat::eD32F:
				l_return = pixel_definitions< PixelFormat::eD32F >::Compressed;
				break;

			case PixelFormat::eS1:
				l_return = pixel_definitions< PixelFormat::eS1 >::Compressed;
				break;

			case PixelFormat::eS8:
				l_return = pixel_definitions< PixelFormat::eS8 >::Compressed;
				break;

			default:
				l_return = false;
				break;
			}

			return l_return;
		}

		PxBufferBaseSPtr ExtractAlpha( PxBufferBaseSPtr & p_pSrc )
		{
			PxBufferBaseSPtr l_return;

			if ( HasAlpha( p_pSrc->format() ) )
			{
				l_return = PxBufferBase::create( p_pSrc->dimensions(), PixelFormat::eA8L8, p_pSrc->const_ptr(), p_pSrc->format() );
				uint8_t * l_pBuffer = l_return->ptr();

				for ( uint32_t i = 0; i < l_return->size(); i += 2 )
				{
					l_pBuffer[0] = l_pBuffer[1];
					l_pBuffer++;
					l_pBuffer++;
				}

				l_return = PxBufferBase::create( p_pSrc->dimensions(), PixelFormat::eL8, l_return->const_ptr(), l_return->format() );
				p_pSrc = PxBufferBase::create( p_pSrc->dimensions(), GetPFWithoutAlpha( p_pSrc->format() ), p_pSrc->const_ptr(), p_pSrc->format() );
			}

			return l_return;
		}

		void ReduceToAlpha( PxBufferBaseSPtr & p_pSrc )
		{
			if ( HasAlpha( p_pSrc->format() ) )
			{
				if ( p_pSrc->format() != PixelFormat::eA8L8 )
				{
					p_pSrc = PxBufferBase::create( p_pSrc->dimensions(), PixelFormat::eA8L8, p_pSrc->const_ptr(), p_pSrc->format() );
				}

				uint8_t * l_pBuffer = p_pSrc->ptr();

				for ( uint32_t i = 0; i < p_pSrc->size(); i += 2 )
				{
					l_pBuffer[0] = l_pBuffer[1];
					l_pBuffer++;
					l_pBuffer++;
				}
			}

			p_pSrc = PxBufferBase::create( p_pSrc->dimensions(), PixelFormat::eL8, p_pSrc->const_ptr(), p_pSrc->format() );
		}

		uint8_t GetBytesPerPixel( PixelFormat p_format )
		{
			uint8_t l_return = 0;

			switch ( p_format )
			{
			case PixelFormat::eL8:
				l_return = pixel_definitions< PixelFormat::eL8 >::Size;
				break;

			case PixelFormat::eL16F32F:
				l_return = pixel_definitions<	PixelFormat::eL16F32F >::Size;
				break;

			case PixelFormat::eL32F:
				l_return = pixel_definitions< PixelFormat::eL32F >::Size;
				break;

			case PixelFormat::eA8L8:
				l_return = pixel_definitions< PixelFormat::eA8L8 >::Size;
				break;

			case PixelFormat::eAL16F32F:
				l_return = pixel_definitions<	PixelFormat::eAL16F32F >::Size;
				break;

			case PixelFormat::eAL32F:
				l_return = pixel_definitions< PixelFormat::eAL32F >::Size;
				break;

			case PixelFormat::eA1R5G5B5:
				l_return = pixel_definitions< PixelFormat::eA1R5G5B5 >::Size;
				break;

			case PixelFormat::eA4R4G4B4:
				l_return = pixel_definitions< PixelFormat::eA4R4G4B4 >::Size;
				break;

			case PixelFormat::eR5G6B5:
				l_return = pixel_definitions< PixelFormat::eR5G6B5 >::Size;
				break;

			case PixelFormat::eR8G8B8:
				l_return = pixel_definitions< PixelFormat::eR8G8B8 >::Size;
				break;

			case PixelFormat::eB8G8R8:
				l_return = pixel_definitions< PixelFormat::eB8G8R8 >::Size;
				break;

			case PixelFormat::eA8R8G8B8:
				l_return = pixel_definitions< PixelFormat::eA8R8G8B8 >::Size;
				break;

			case PixelFormat::eA8B8G8R8:
				l_return = pixel_definitions< PixelFormat::eA8B8G8R8 >::Size;
				break;

			case PixelFormat::eRGB16F:
				l_return = pixel_definitions<	PixelFormat::eRGB16F >::Size;
				break;

			case PixelFormat::eRGBA16F:
				l_return = pixel_definitions<	PixelFormat::eRGBA16F >::Size;
				break;

			case PixelFormat::eRGB16F32F:
				l_return = pixel_definitions<	PixelFormat::eRGB16F32F >::Size;
				break;

			case PixelFormat::eRGBA16F32F:
				l_return = pixel_definitions<	PixelFormat::eRGBA16F32F >::Size;
				break;

			case PixelFormat::eRGB32F:
				l_return = pixel_definitions< PixelFormat::eRGB32F >::Size;
				break;

			case PixelFormat::eRGBA32F:
				l_return = pixel_definitions< PixelFormat::eRGBA32F >::Size;
				break;

			case PixelFormat::eDXTC1:
				l_return = pixel_definitions<	PixelFormat::eDXTC1 >::Size;
				break;

			case PixelFormat::eDXTC3:
				l_return = pixel_definitions<	PixelFormat::eDXTC3 >::Size;
				break;

			case PixelFormat::eDXTC5:
				l_return = pixel_definitions<	PixelFormat::eDXTC5 >::Size;
				break;

			case PixelFormat::eYUY2:
				l_return = pixel_definitions<	PixelFormat::eYUY2 >::Size;
				break;

			case PixelFormat::eD16:
				l_return = pixel_definitions< PixelFormat::eD16 >::Size;
				break;

			case PixelFormat::eD24:
				l_return = pixel_definitions< PixelFormat::eD24 >::Size;
				break;

			case PixelFormat::eD24S8:
				l_return = pixel_definitions< PixelFormat::eD24S8 >::Size;
				break;

			case PixelFormat::eD32:
				l_return = pixel_definitions< PixelFormat::eD32 >::Size;
				break;

			case PixelFormat::eD32F:
				l_return = pixel_definitions< PixelFormat::eD32F >::Size;
				break;

			case PixelFormat::eS1:
				l_return = pixel_definitions<	PixelFormat::eS1 >::Size;
				break;

			case PixelFormat::eS8:
				l_return = pixel_definitions< PixelFormat::eS8 >::Size;
				break;
			}

			return l_return;
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

			case PixelFormat::eS1:
				pixel_definitions< PixelFormat::eS1 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::eS8:
				pixel_definitions< PixelFormat::eS8 >::convert( p_pSrc, p_pDest, p_eDestFmt );
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

			case PixelFormat::eS1:
				pixel_definitions< PixelFormat::eS1 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::eS8:
				pixel_definitions< PixelFormat::eS8 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;
			}
		}

		PixelFormat GetFormatByName( String const & p_strFormat )
		{
			PixelFormat l_return = PixelFormat::eCount;

			for ( int i = 0; i < int( l_return ); ++i )
			{
				switch ( PixelFormat( i ) )
				{
				case PixelFormat::eL8:
					l_return = ( p_strFormat == pixel_definitions< PixelFormat::eL8 >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eL16F32F:
					l_return = ( p_strFormat == pixel_definitions< PixelFormat::eL16F32F >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eL32F:
					l_return = ( p_strFormat == pixel_definitions< PixelFormat::eL32F >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eA8L8:
					l_return = ( p_strFormat == pixel_definitions< PixelFormat::eA8L8 >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eAL16F32F:
					l_return = ( p_strFormat == pixel_definitions< PixelFormat::eAL16F32F >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eAL32F:
					l_return = ( p_strFormat == pixel_definitions< PixelFormat::eAL32F >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eA1R5G5B5:
					l_return = ( p_strFormat == pixel_definitions< PixelFormat::eA1R5G5B5 >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eA4R4G4B4:
					l_return = ( p_strFormat == pixel_definitions< PixelFormat::eA4R4G4B4 >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eR5G6B5:
					l_return = ( p_strFormat == pixel_definitions< PixelFormat::eR5G6B5 >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eR8G8B8:
					l_return = ( p_strFormat == pixel_definitions< PixelFormat::eR8G8B8 >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eB8G8R8:
					l_return = ( p_strFormat == pixel_definitions< PixelFormat::eB8G8R8 >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eA8R8G8B8:
					l_return = ( p_strFormat == pixel_definitions< PixelFormat::eA8R8G8B8 >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eA8B8G8R8:
					l_return = ( p_strFormat == pixel_definitions< PixelFormat::eA8B8G8R8 >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eRGB16F:
					l_return = ( p_strFormat == pixel_definitions< PixelFormat::eRGB16F >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eRGBA16F:
					l_return = ( p_strFormat == pixel_definitions< PixelFormat::eRGBA16F >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eRGB16F32F:
					l_return = ( p_strFormat == pixel_definitions< PixelFormat::eRGB16F32F >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eRGBA16F32F:
					l_return = ( p_strFormat == pixel_definitions< PixelFormat::eRGBA16F32F >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eRGB32F:
					l_return = ( p_strFormat == pixel_definitions< PixelFormat::eRGB32F >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eRGBA32F:
					l_return = ( p_strFormat == pixel_definitions< PixelFormat::eRGBA32F >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eDXTC1:
					l_return = ( p_strFormat == pixel_definitions< PixelFormat::eDXTC1 >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eDXTC3:
					l_return = ( p_strFormat == pixel_definitions< PixelFormat::eDXTC3 >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eDXTC5:
					l_return = ( p_strFormat == pixel_definitions< PixelFormat::eDXTC5 >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eYUY2:
					l_return = ( p_strFormat == pixel_definitions< PixelFormat::eYUY2 >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eD16:
					l_return = ( p_strFormat == pixel_definitions< PixelFormat::eD16 >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eD24:
					l_return = ( p_strFormat == pixel_definitions< PixelFormat::eD24 >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eD24S8:
					l_return = ( p_strFormat == pixel_definitions< PixelFormat::eD24S8 >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eD32:
					l_return = ( p_strFormat == pixel_definitions< PixelFormat::eD32 >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eD32F:
					l_return = ( p_strFormat == pixel_definitions< PixelFormat::eD32F >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eS1:
					l_return = ( p_strFormat == pixel_definitions< PixelFormat::eS1 >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;

				case PixelFormat::eS8:
					l_return = ( p_strFormat == pixel_definitions< PixelFormat::eS8 >::to_str() ? PixelFormat( i ) : PixelFormat::eCount );
					break;
				}
			}

			return l_return;
		}

		String GetFormatName( PixelFormat p_format )
		{
			String l_return;

			switch ( p_format )
			{
			case PixelFormat::eL8:
				l_return = pixel_definitions< PixelFormat::eL8 >::to_str();
				break;

			case PixelFormat::eL16F32F:
				l_return = pixel_definitions< PixelFormat::eL16F32F >::to_str();
				break;

			case PixelFormat::eL32F:
				l_return = pixel_definitions< PixelFormat::eL32F >::to_str();
				break;

			case PixelFormat::eA8L8:
				l_return = pixel_definitions< PixelFormat::eA8L8 >::to_str();
				break;

			case PixelFormat::eAL16F32F:
				l_return = pixel_definitions< PixelFormat::eAL16F32F >::to_str();
				break;

			case PixelFormat::eAL32F:
				l_return = pixel_definitions< PixelFormat::eAL32F >::to_str();
				break;

			case PixelFormat::eA1R5G5B5:
				l_return = pixel_definitions< PixelFormat::eA1R5G5B5 >::to_str();
				break;

			case PixelFormat::eA4R4G4B4:
				l_return = pixel_definitions< PixelFormat::eA4R4G4B4 >::to_str();
				break;

			case PixelFormat::eR5G6B5:
				l_return = pixel_definitions< PixelFormat::eR5G6B5 >::to_str();
				break;

			case PixelFormat::eR8G8B8:
				l_return = pixel_definitions< PixelFormat::eR8G8B8 >::to_str();
				break;

			case PixelFormat::eB8G8R8:
				l_return = pixel_definitions< PixelFormat::eB8G8R8 >::to_str();
				break;

			case PixelFormat::eA8R8G8B8:
				l_return = pixel_definitions< PixelFormat::eA8R8G8B8 >::to_str();
				break;

			case PixelFormat::eA8B8G8R8:
				l_return = pixel_definitions< PixelFormat::eA8B8G8R8 >::to_str();
				break;

			case PixelFormat::eRGB16F:
				l_return = pixel_definitions< PixelFormat::eRGB16F >::to_str();
				break;

			case PixelFormat::eRGBA16F:
				l_return = pixel_definitions< PixelFormat::eRGBA16F >::to_str();
				break;

			case PixelFormat::eRGB16F32F:
				l_return = pixel_definitions< PixelFormat::eRGB16F32F >::to_str();
				break;

			case PixelFormat::eRGBA16F32F:
				l_return = pixel_definitions< PixelFormat::eRGBA16F32F >::to_str();
				break;

			case PixelFormat::eRGB32F:
				l_return = pixel_definitions< PixelFormat::eRGB32F >::to_str();
				break;

			case PixelFormat::eRGBA32F:
				l_return = pixel_definitions< PixelFormat::eRGBA32F >::to_str();
				break;

			case PixelFormat::eDXTC1:
				l_return = pixel_definitions< PixelFormat::eDXTC1 >::to_str();
				break;

			case PixelFormat::eDXTC3:
				l_return = pixel_definitions< PixelFormat::eDXTC3 >::to_str();
				break;

			case PixelFormat::eDXTC5:
				l_return = pixel_definitions< PixelFormat::eDXTC5 >::to_str();
				break;

			case PixelFormat::eYUY2:
				l_return = pixel_definitions< PixelFormat::eYUY2 >::to_str();
				break;

			case PixelFormat::eD16:
				l_return = pixel_definitions< PixelFormat::eD16 >::to_str();
				break;

			case PixelFormat::eD24:
				l_return = pixel_definitions< PixelFormat::eD24 >::to_str();
				break;

			case PixelFormat::eD24S8:
				l_return = pixel_definitions< PixelFormat::eD24S8 >::to_str();
				break;

			case PixelFormat::eD32:
				l_return = pixel_definitions< PixelFormat::eD32 >::to_str();
				break;

			case PixelFormat::eD32F:
				l_return = pixel_definitions< PixelFormat::eD32F >::to_str();
				break;

			case PixelFormat::eS1:
				l_return = pixel_definitions< PixelFormat::eS1 >::to_str();
				break;

			case PixelFormat::eS8:
				l_return = pixel_definitions< PixelFormat::eS8 >::to_str();
				break;
			}

			return l_return;
		}
	}
}
