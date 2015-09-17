#include "PixelFormat.hpp"
#include "PixelBuffer.hpp"

namespace Castor
{
	namespace detail
	{
		const __m128i buffer_converter< ePIXEL_FORMAT_YUY2, ePIXEL_FORMAT_R8G8B8 >::umask	= _mm_set1_epi32( 0x0000FF00 );
		const __m128i buffer_converter< ePIXEL_FORMAT_YUY2, ePIXEL_FORMAT_R8G8B8 >::vmask	= _mm_set1_epi32( 0xFF000000 );
		const __m128i buffer_converter< ePIXEL_FORMAT_YUY2, ePIXEL_FORMAT_R8G8B8 >::ymask	= _mm_set1_epi32( 0x00FF00FF );
		const __m128i buffer_converter< ePIXEL_FORMAT_YUY2, ePIXEL_FORMAT_R8G8B8 >::uvsub	= _mm_set1_epi32( 0x00800080 );
		const __m128i buffer_converter< ePIXEL_FORMAT_YUY2, ePIXEL_FORMAT_R8G8B8 >::ysub	= _mm_set1_epi32( 0x00100010 );
		const __m128i buffer_converter< ePIXEL_FORMAT_YUY2, ePIXEL_FORMAT_R8G8B8 >::facy	= _mm_set1_epi32( 0x00400040 );// 1 * 64
		const __m128i buffer_converter< ePIXEL_FORMAT_YUY2, ePIXEL_FORMAT_R8G8B8 >::facrv	= _mm_set1_epi32( 0x00710071 );// 1.772 * 64
		const __m128i buffer_converter< ePIXEL_FORMAT_YUY2, ePIXEL_FORMAT_R8G8B8 >::facgu	= _mm_set1_epi32( 0x002D002D );// 0.714 * 64
		const __m128i buffer_converter< ePIXEL_FORMAT_YUY2, ePIXEL_FORMAT_R8G8B8 >::facgv	= _mm_set1_epi32( 0x00160016 );// 0.344 * 64
		const __m128i buffer_converter< ePIXEL_FORMAT_YUY2, ePIXEL_FORMAT_R8G8B8 >::facbu	= _mm_set1_epi32( 0x00590059 );// 1.402 * 64
		const __m128i buffer_converter< ePIXEL_FORMAT_YUY2, ePIXEL_FORMAT_R8G8B8 >::zero	= _mm_set1_epi32( 0x00000000 );

		const __m128i buffer_converter< ePIXEL_FORMAT_YUY2, ePIXEL_FORMAT_A8R8G8B8 >::amask	= _mm_set1_epi32( 0x000000FF );
		const __m128i buffer_converter< ePIXEL_FORMAT_YUY2, ePIXEL_FORMAT_A8R8G8B8 >::umask	= _mm_set1_epi32( 0x0000FF00 );
		const __m128i buffer_converter< ePIXEL_FORMAT_YUY2, ePIXEL_FORMAT_A8R8G8B8 >::vmask	= _mm_set1_epi32( 0xFF000000 );
		const __m128i buffer_converter< ePIXEL_FORMAT_YUY2, ePIXEL_FORMAT_A8R8G8B8 >::ymask	= _mm_set1_epi32( 0x00FF00FF );
		const __m128i buffer_converter< ePIXEL_FORMAT_YUY2, ePIXEL_FORMAT_A8R8G8B8 >::uvsub	= _mm_set1_epi32( 0x00800080 );
		const __m128i buffer_converter< ePIXEL_FORMAT_YUY2, ePIXEL_FORMAT_A8R8G8B8 >::ysub	= _mm_set1_epi32( 0x00100010 );
		const __m128i buffer_converter< ePIXEL_FORMAT_YUY2, ePIXEL_FORMAT_A8R8G8B8 >::facy	= _mm_set1_epi32( 0x00400040 );// 1 * 64
		const __m128i buffer_converter< ePIXEL_FORMAT_YUY2, ePIXEL_FORMAT_A8R8G8B8 >::facrv	= _mm_set1_epi32( 0x00710071 );// 1.772 * 64
		const __m128i buffer_converter< ePIXEL_FORMAT_YUY2, ePIXEL_FORMAT_A8R8G8B8 >::facgu	= _mm_set1_epi32( 0x002D002D );// 0.714 * 64
		const __m128i buffer_converter< ePIXEL_FORMAT_YUY2, ePIXEL_FORMAT_A8R8G8B8 >::facgv	= _mm_set1_epi32( 0x00160016 );// 0.344 * 64
		const __m128i buffer_converter< ePIXEL_FORMAT_YUY2, ePIXEL_FORMAT_A8R8G8B8 >::facbu	= _mm_set1_epi32( 0x00590059 );// 1.402 * 64
		const __m128i buffer_converter< ePIXEL_FORMAT_YUY2, ePIXEL_FORMAT_A8R8G8B8 >::zero	= _mm_set1_epi32( 0x00000000 );
	}

	namespace PF
	{
		ePIXEL_FORMAT GetPFWithoutAlpha( ePIXEL_FORMAT p_ePixelFmt )
		{
			ePIXEL_FORMAT l_eReturn = ePIXEL_FORMAT_COUNT;

			switch ( p_ePixelFmt )
			{
			case ePIXEL_FORMAT_A8L8:
				l_eReturn = pixel_definitions< ePIXEL_FORMAT_A8L8 >::NoAlphaPF;
				break;

			case ePIXEL_FORMAT_AL16F32F:
				l_eReturn = pixel_definitions< ePIXEL_FORMAT_AL16F32F >::NoAlphaPF;
				break;

			case ePIXEL_FORMAT_AL32F:
				l_eReturn = pixel_definitions< ePIXEL_FORMAT_AL32F >::NoAlphaPF;
				break;

			case ePIXEL_FORMAT_A1R5G5B5:
				l_eReturn = pixel_definitions< ePIXEL_FORMAT_A1R5G5B5 >::NoAlphaPF;
				break;

			case ePIXEL_FORMAT_A4R4G4B4:
				l_eReturn = pixel_definitions< ePIXEL_FORMAT_A4R4G4B4 >::NoAlphaPF;
				break;

			case ePIXEL_FORMAT_A8R8G8B8:
				l_eReturn = pixel_definitions< ePIXEL_FORMAT_A8R8G8B8 >::NoAlphaPF;
				break;

			case ePIXEL_FORMAT_A8B8G8R8:
				l_eReturn = pixel_definitions< ePIXEL_FORMAT_A8B8G8R8 >::NoAlphaPF;
				break;

			case ePIXEL_FORMAT_ARGB16F32F:
				l_eReturn = pixel_definitions< ePIXEL_FORMAT_ARGB16F32F >::NoAlphaPF;
				break;

			case ePIXEL_FORMAT_ARGB32F:
				l_eReturn = pixel_definitions< ePIXEL_FORMAT_ARGB32F >::NoAlphaPF;
				break;

			case ePIXEL_FORMAT_DXTC3:
				l_eReturn = pixel_definitions< ePIXEL_FORMAT_DXTC3 >::NoAlphaPF;
				break;

			case ePIXEL_FORMAT_DXTC5:
				l_eReturn = pixel_definitions< ePIXEL_FORMAT_DXTC5 >::NoAlphaPF;
				break;

			default:
				l_eReturn = p_ePixelFmt;
				break;
			}

			return l_eReturn;
		}

		bool HasAlpha( ePIXEL_FORMAT p_ePixelFmt )
		{
			bool l_return = false;

			switch ( p_ePixelFmt )
			{
			case ePIXEL_FORMAT_L8:
				l_return = pixel_definitions< ePIXEL_FORMAT_L8 >::Alpha;
				break;

			case ePIXEL_FORMAT_L16F32F:
				l_return = pixel_definitions< ePIXEL_FORMAT_L16F32F	>::Alpha;
				break;

			case ePIXEL_FORMAT_L32F:
				l_return = pixel_definitions< ePIXEL_FORMAT_L32F >::Alpha;
				break;

			case ePIXEL_FORMAT_A8L8:
				l_return = pixel_definitions< ePIXEL_FORMAT_A8L8 >::Alpha;
				break;

			case ePIXEL_FORMAT_AL16F32F:
				l_return = pixel_definitions< ePIXEL_FORMAT_AL16F32F >::Alpha;
				break;

			case ePIXEL_FORMAT_AL32F:
				l_return = pixel_definitions< ePIXEL_FORMAT_AL32F >::Alpha;
				break;

			case ePIXEL_FORMAT_A1R5G5B5:
				l_return = pixel_definitions< ePIXEL_FORMAT_A1R5G5B5 >::Alpha;
				break;

			case ePIXEL_FORMAT_A4R4G4B4:
				l_return = pixel_definitions< ePIXEL_FORMAT_A4R4G4B4 >::Alpha;
				break;

			case ePIXEL_FORMAT_R5G6B5:
				l_return = pixel_definitions< ePIXEL_FORMAT_R5G6B5 >::Alpha;
				break;

			case ePIXEL_FORMAT_R8G8B8:
				l_return = pixel_definitions< ePIXEL_FORMAT_R8G8B8 >::Alpha;
				break;

			case ePIXEL_FORMAT_B8G8R8:
				l_return = pixel_definitions< ePIXEL_FORMAT_B8G8R8 >::Alpha;
				break;

			case ePIXEL_FORMAT_A8R8G8B8:
				l_return = pixel_definitions< ePIXEL_FORMAT_A8R8G8B8 >::Alpha;
				break;

			case ePIXEL_FORMAT_A8B8G8R8:
				l_return = pixel_definitions< ePIXEL_FORMAT_A8B8G8R8 >::Alpha;
				break;

			case ePIXEL_FORMAT_RGB16F32F:
				l_return = pixel_definitions< ePIXEL_FORMAT_RGB16F32F >::Alpha;
				break;

			case ePIXEL_FORMAT_ARGB16F32F:
				l_return = pixel_definitions< ePIXEL_FORMAT_ARGB16F32F >::Alpha;
				break;

			case ePIXEL_FORMAT_RGB32F:
				l_return = pixel_definitions< ePIXEL_FORMAT_RGB32F >::Alpha;
				break;

			case ePIXEL_FORMAT_ARGB32F:
				l_return = pixel_definitions< ePIXEL_FORMAT_ARGB32F >::Alpha;
				break;

			case ePIXEL_FORMAT_DXTC1:
				l_return = pixel_definitions< ePIXEL_FORMAT_DXTC1 >::Alpha;
				break;

			case ePIXEL_FORMAT_DXTC3:
				l_return = pixel_definitions< ePIXEL_FORMAT_DXTC3 >::Alpha;
				break;

			case ePIXEL_FORMAT_DXTC5:
				l_return = pixel_definitions< ePIXEL_FORMAT_DXTC5 >::Alpha;
				break;

			case ePIXEL_FORMAT_YUY2:
				l_return = pixel_definitions< ePIXEL_FORMAT_YUY2 >::Alpha;
				break;

			case ePIXEL_FORMAT_DEPTH16:
				l_return = pixel_definitions< ePIXEL_FORMAT_DEPTH16 >::Alpha;
				break;

			case ePIXEL_FORMAT_DEPTH24:
				l_return = pixel_definitions< ePIXEL_FORMAT_DEPTH24 >::Alpha;
				break;

			case ePIXEL_FORMAT_DEPTH24S8:
				l_return = pixel_definitions< ePIXEL_FORMAT_DEPTH24S8 >::Alpha;
				break;

			case ePIXEL_FORMAT_DEPTH32:
				l_return = pixel_definitions< ePIXEL_FORMAT_DEPTH32 >::Alpha;
				break;

			case ePIXEL_FORMAT_STENCIL1:
				l_return = pixel_definitions< ePIXEL_FORMAT_STENCIL1 >::Alpha;
				break;

			case ePIXEL_FORMAT_STENCIL8:
				l_return = pixel_definitions< ePIXEL_FORMAT_STENCIL8 >::Alpha;
				break;

			default:
				l_return = false;
				break;
			}

			return l_return;
		}

		bool IsCompressed( ePIXEL_FORMAT p_ePixelFmt )
		{
			bool l_return = false;

			switch ( p_ePixelFmt )
			{
			case ePIXEL_FORMAT_L8:
				l_return = pixel_definitions< ePIXEL_FORMAT_L8 >::Compressed;
				break;

			case ePIXEL_FORMAT_L16F32F:
				l_return = pixel_definitions< ePIXEL_FORMAT_L16F32F >::Compressed;
				break;

			case ePIXEL_FORMAT_L32F:
				l_return = pixel_definitions< ePIXEL_FORMAT_L32F >::Compressed;
				break;

			case ePIXEL_FORMAT_A8L8:
				l_return = pixel_definitions< ePIXEL_FORMAT_A8L8 >::Compressed;
				break;

			case ePIXEL_FORMAT_AL16F32F:
				l_return = pixel_definitions< ePIXEL_FORMAT_AL16F32F >::Compressed;
				break;

			case ePIXEL_FORMAT_AL32F:
				l_return = pixel_definitions< ePIXEL_FORMAT_AL32F >::Compressed;
				break;

			case ePIXEL_FORMAT_A1R5G5B5:
				l_return = pixel_definitions< ePIXEL_FORMAT_A1R5G5B5 >::Compressed;
				break;

			case ePIXEL_FORMAT_A4R4G4B4:
				l_return = pixel_definitions< ePIXEL_FORMAT_A4R4G4B4 >::Compressed;
				break;

			case ePIXEL_FORMAT_R5G6B5:
				l_return = pixel_definitions< ePIXEL_FORMAT_R5G6B5 >::Compressed;
				break;

			case ePIXEL_FORMAT_R8G8B8:
				l_return = pixel_definitions< ePIXEL_FORMAT_R8G8B8 >::Compressed;
				break;

			case ePIXEL_FORMAT_B8G8R8:
				l_return = pixel_definitions< ePIXEL_FORMAT_B8G8R8 >::Compressed;
				break;

			case ePIXEL_FORMAT_A8R8G8B8:
				l_return = pixel_definitions< ePIXEL_FORMAT_A8R8G8B8 >::Compressed;
				break;

			case ePIXEL_FORMAT_A8B8G8R8:
				l_return = pixel_definitions< ePIXEL_FORMAT_A8B8G8R8 >::Compressed;
				break;

			case ePIXEL_FORMAT_RGB16F32F:
				l_return = pixel_definitions< ePIXEL_FORMAT_RGB16F32F >::Compressed;
				break;

			case ePIXEL_FORMAT_ARGB16F32F:
				l_return = pixel_definitions< ePIXEL_FORMAT_ARGB16F32F >::Compressed;
				break;

			case ePIXEL_FORMAT_RGB32F:
				l_return = pixel_definitions< ePIXEL_FORMAT_RGB32F >::Compressed;
				break;

			case ePIXEL_FORMAT_ARGB32F:
				l_return = pixel_definitions< ePIXEL_FORMAT_ARGB32F >::Compressed;
				break;

			case ePIXEL_FORMAT_DXTC1:
				l_return = pixel_definitions< ePIXEL_FORMAT_DXTC1 >::Compressed;
				break;

			case ePIXEL_FORMAT_DXTC3:
				l_return = pixel_definitions< ePIXEL_FORMAT_DXTC3 >::Compressed;
				break;

			case ePIXEL_FORMAT_DXTC5:
				l_return = pixel_definitions< ePIXEL_FORMAT_DXTC5 >::Compressed;
				break;

			case ePIXEL_FORMAT_YUY2:
				l_return = pixel_definitions< ePIXEL_FORMAT_YUY2 >::Compressed;
				break;

			case ePIXEL_FORMAT_DEPTH16:
				l_return = pixel_definitions< ePIXEL_FORMAT_DEPTH16 >::Compressed;
				break;

			case ePIXEL_FORMAT_DEPTH24:
				l_return = pixel_definitions< ePIXEL_FORMAT_DEPTH24 >::Compressed;
				break;

			case ePIXEL_FORMAT_DEPTH24S8:
				l_return = pixel_definitions< ePIXEL_FORMAT_DEPTH24S8 >::Compressed;
				break;

			case ePIXEL_FORMAT_DEPTH32:
				l_return = pixel_definitions< ePIXEL_FORMAT_DEPTH32 >::Compressed;
				break;

			case ePIXEL_FORMAT_STENCIL1:
				l_return = pixel_definitions< ePIXEL_FORMAT_STENCIL1 >::Compressed;
				break;

			case ePIXEL_FORMAT_STENCIL8:
				l_return = pixel_definitions< ePIXEL_FORMAT_STENCIL8 >::Compressed;
				break;

			default:
				l_return = false;
				break;
			}

			return l_return;
		}

		PxBufferBaseSPtr ExtractAlpha( PxBufferBaseSPtr & p_pSrc )
		{
			PxBufferBaseSPtr l_pReturn;

			if ( HasAlpha( p_pSrc->format() ) )
			{
				l_pReturn = PxBufferBase::create( p_pSrc->dimensions(), ePIXEL_FORMAT_A8L8, p_pSrc->const_ptr(), p_pSrc->format() );
				uint8_t * l_pBuffer = l_pReturn->ptr();

				for ( uint32_t i = 0; i < l_pReturn->size(); i += 2 )
				{
					l_pBuffer[0] = l_pBuffer[1];
					l_pBuffer++;
					l_pBuffer++;
				}

				l_pReturn = PxBufferBase::create( p_pSrc->dimensions(), ePIXEL_FORMAT_L8, l_pReturn->const_ptr(), l_pReturn->format() );
				p_pSrc = PxBufferBase::create( p_pSrc->dimensions(), GetPFWithoutAlpha( p_pSrc->format() ), p_pSrc->const_ptr(), p_pSrc->format() );
			}

			return l_pReturn;
		}

		void ReduceToAlpha( PxBufferBaseSPtr & p_pSrc )
		{
			if ( HasAlpha( p_pSrc->format() ) )
			{
				if ( p_pSrc->format() != ePIXEL_FORMAT_A8L8 )
				{
					p_pSrc = PxBufferBase::create( p_pSrc->dimensions(), ePIXEL_FORMAT_A8L8, p_pSrc->const_ptr(), p_pSrc->format() );
				}

				uint8_t * l_pBuffer = p_pSrc->ptr();

				for ( uint32_t i = 0; i < p_pSrc->size(); i += 2 )
				{
					l_pBuffer[0] = l_pBuffer[1];
					l_pBuffer++;
					l_pBuffer++;
				}
			}

			p_pSrc = PxBufferBase::create( p_pSrc->dimensions(), ePIXEL_FORMAT_L8, p_pSrc->const_ptr(), p_pSrc->format() );
		}

		uint8_t GetBytesPerPixel( ePIXEL_FORMAT p_pfFormat )
		{
			uint8_t l_uiReturn = 0;

			switch ( p_pfFormat )
			{
			case ePIXEL_FORMAT_L8:
				l_uiReturn = pixel_definitions< ePIXEL_FORMAT_L8 >::Size;
				break;

			case ePIXEL_FORMAT_L16F32F:
				l_uiReturn = pixel_definitions<	ePIXEL_FORMAT_L16F32F >::Size;
				break;

			case ePIXEL_FORMAT_L32F:
				l_uiReturn = pixel_definitions< ePIXEL_FORMAT_L32F >::Size;
				break;

			case ePIXEL_FORMAT_A8L8:
				l_uiReturn = pixel_definitions< ePIXEL_FORMAT_A8L8 >::Size;
				break;

			case ePIXEL_FORMAT_AL16F32F:
				l_uiReturn = pixel_definitions<	ePIXEL_FORMAT_AL16F32F >::Size;
				break;

			case ePIXEL_FORMAT_AL32F:
				l_uiReturn = pixel_definitions< ePIXEL_FORMAT_AL32F >::Size;
				break;

			case ePIXEL_FORMAT_A1R5G5B5:
				l_uiReturn = pixel_definitions< ePIXEL_FORMAT_A1R5G5B5 >::Size;
				break;

			case ePIXEL_FORMAT_A4R4G4B4:
				l_uiReturn = pixel_definitions< ePIXEL_FORMAT_A4R4G4B4 >::Size;
				break;

			case ePIXEL_FORMAT_R5G6B5:
				l_uiReturn = pixel_definitions< ePIXEL_FORMAT_R5G6B5 >::Size;
				break;

			case ePIXEL_FORMAT_R8G8B8:
				l_uiReturn = pixel_definitions< ePIXEL_FORMAT_R8G8B8 >::Size;
				break;

			case ePIXEL_FORMAT_B8G8R8:
				l_uiReturn = pixel_definitions< ePIXEL_FORMAT_B8G8R8 >::Size;
				break;

			case ePIXEL_FORMAT_A8R8G8B8:
				l_uiReturn = pixel_definitions< ePIXEL_FORMAT_A8R8G8B8 >::Size;
				break;

			case ePIXEL_FORMAT_A8B8G8R8:
				l_uiReturn = pixel_definitions< ePIXEL_FORMAT_A8B8G8R8 >::Size;
				break;

			case ePIXEL_FORMAT_RGB16F32F:
				l_uiReturn = pixel_definitions<	ePIXEL_FORMAT_RGB16F32F >::Size;
				break;

			case ePIXEL_FORMAT_ARGB16F32F:
				l_uiReturn = pixel_definitions<	ePIXEL_FORMAT_ARGB16F32F >::Size;
				break;

			case ePIXEL_FORMAT_RGB32F:
				l_uiReturn = pixel_definitions< ePIXEL_FORMAT_RGB32F >::Size;
				break;

			case ePIXEL_FORMAT_ARGB32F:
				l_uiReturn = pixel_definitions< ePIXEL_FORMAT_ARGB32F >::Size;
				break;

			case ePIXEL_FORMAT_DXTC1:
				l_uiReturn = pixel_definitions<	ePIXEL_FORMAT_DXTC1 >::Size;
				break;

			case ePIXEL_FORMAT_DXTC3:
				l_uiReturn = pixel_definitions<	ePIXEL_FORMAT_DXTC3 >::Size;
				break;

			case ePIXEL_FORMAT_DXTC5:
				l_uiReturn = pixel_definitions<	ePIXEL_FORMAT_DXTC5 >::Size;
				break;

			case ePIXEL_FORMAT_YUY2:
				l_uiReturn = pixel_definitions<	ePIXEL_FORMAT_YUY2 >::Size;
				break;

			case ePIXEL_FORMAT_DEPTH16:
				l_uiReturn = pixel_definitions< ePIXEL_FORMAT_DEPTH16 >::Size;
				break;

			case ePIXEL_FORMAT_DEPTH24:
				l_uiReturn = pixel_definitions< ePIXEL_FORMAT_DEPTH24 >::Size;
				break;

			case ePIXEL_FORMAT_DEPTH24S8:
				l_uiReturn = pixel_definitions< ePIXEL_FORMAT_DEPTH24S8 >::Size;
				break;

			case ePIXEL_FORMAT_DEPTH32:
				l_uiReturn = pixel_definitions< ePIXEL_FORMAT_DEPTH32 >::Size;
				break;

			case ePIXEL_FORMAT_STENCIL1:
				l_uiReturn = pixel_definitions<	ePIXEL_FORMAT_STENCIL1 >::Size;
				break;

			case ePIXEL_FORMAT_STENCIL8:
				l_uiReturn = pixel_definitions< ePIXEL_FORMAT_STENCIL8 >::Size;
				break;
			}

			return l_uiReturn;
		}

		void ConvertPixel( ePIXEL_FORMAT p_eSrcFmt, uint8_t const *& p_pSrc, ePIXEL_FORMAT p_eDestFmt, uint8_t *& p_pDest )
		{
			switch ( p_eSrcFmt )
			{
			case ePIXEL_FORMAT_L8:
				pixel_definitions< ePIXEL_FORMAT_L8 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case ePIXEL_FORMAT_L16F32F:
				pixel_definitions< ePIXEL_FORMAT_L16F32F >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case ePIXEL_FORMAT_L32F:
				pixel_definitions< ePIXEL_FORMAT_L32F >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case ePIXEL_FORMAT_A8L8:
				pixel_definitions< ePIXEL_FORMAT_A8L8 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case ePIXEL_FORMAT_AL16F32F:
				pixel_definitions< ePIXEL_FORMAT_AL16F32F >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case ePIXEL_FORMAT_AL32F:
				pixel_definitions< ePIXEL_FORMAT_AL32F >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case ePIXEL_FORMAT_A1R5G5B5:
				pixel_definitions< ePIXEL_FORMAT_A1R5G5B5 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case ePIXEL_FORMAT_A4R4G4B4:
				pixel_definitions< ePIXEL_FORMAT_A4R4G4B4 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case ePIXEL_FORMAT_R5G6B5:
				pixel_definitions< ePIXEL_FORMAT_R5G6B5 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case ePIXEL_FORMAT_R8G8B8:
				pixel_definitions< ePIXEL_FORMAT_R8G8B8 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case ePIXEL_FORMAT_B8G8R8:
				pixel_definitions< ePIXEL_FORMAT_B8G8R8 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case ePIXEL_FORMAT_A8R8G8B8:
				pixel_definitions< ePIXEL_FORMAT_A8R8G8B8 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case ePIXEL_FORMAT_A8B8G8R8:
				pixel_definitions< ePIXEL_FORMAT_A8B8G8R8 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case ePIXEL_FORMAT_RGB16F32F:
				pixel_definitions< ePIXEL_FORMAT_RGB16F32F >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case ePIXEL_FORMAT_ARGB16F32F:
				pixel_definitions< ePIXEL_FORMAT_ARGB16F32F >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case ePIXEL_FORMAT_RGB32F:
				pixel_definitions< ePIXEL_FORMAT_RGB32F >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case ePIXEL_FORMAT_ARGB32F:
				pixel_definitions< ePIXEL_FORMAT_ARGB32F >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case ePIXEL_FORMAT_DXTC1:
				pixel_definitions< ePIXEL_FORMAT_DXTC1 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case ePIXEL_FORMAT_DXTC3:
				pixel_definitions< ePIXEL_FORMAT_DXTC3 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case ePIXEL_FORMAT_DXTC5:
				pixel_definitions< ePIXEL_FORMAT_DXTC5 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case ePIXEL_FORMAT_YUY2:
				pixel_definitions< ePIXEL_FORMAT_YUY2 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case ePIXEL_FORMAT_DEPTH16:
				pixel_definitions< ePIXEL_FORMAT_DEPTH16 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case ePIXEL_FORMAT_DEPTH24:
				pixel_definitions< ePIXEL_FORMAT_DEPTH24 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case ePIXEL_FORMAT_DEPTH24S8:
				pixel_definitions< ePIXEL_FORMAT_DEPTH24S8 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case ePIXEL_FORMAT_DEPTH32:
				pixel_definitions< ePIXEL_FORMAT_DEPTH32 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case ePIXEL_FORMAT_STENCIL1:
				pixel_definitions< ePIXEL_FORMAT_STENCIL1 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case ePIXEL_FORMAT_STENCIL8:
				pixel_definitions< ePIXEL_FORMAT_STENCIL8 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;
			}
		}

		void ConvertBuffer( ePIXEL_FORMAT p_eSrcFormat, uint8_t const * p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t * p_pDstBuffer, uint32_t p_uiDstSize )
		{
			switch ( p_eSrcFormat )
			{
			case ePIXEL_FORMAT_L8:
				pixel_definitions< ePIXEL_FORMAT_L8 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case ePIXEL_FORMAT_L16F32F:
				pixel_definitions< ePIXEL_FORMAT_L16F32F >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case ePIXEL_FORMAT_L32F:
				pixel_definitions< ePIXEL_FORMAT_L32F >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case ePIXEL_FORMAT_A8L8:
				pixel_definitions< ePIXEL_FORMAT_A8L8 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case ePIXEL_FORMAT_AL16F32F:
				pixel_definitions< ePIXEL_FORMAT_AL16F32F >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case ePIXEL_FORMAT_AL32F:
				pixel_definitions< ePIXEL_FORMAT_AL32F >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case ePIXEL_FORMAT_A1R5G5B5:
				pixel_definitions< ePIXEL_FORMAT_A1R5G5B5 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case ePIXEL_FORMAT_A4R4G4B4:
				pixel_definitions< ePIXEL_FORMAT_A4R4G4B4 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case ePIXEL_FORMAT_R5G6B5:
				pixel_definitions< ePIXEL_FORMAT_R5G6B5 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case ePIXEL_FORMAT_R8G8B8:
				pixel_definitions< ePIXEL_FORMAT_R8G8B8 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case ePIXEL_FORMAT_B8G8R8:
				pixel_definitions< ePIXEL_FORMAT_B8G8R8 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case ePIXEL_FORMAT_A8R8G8B8:
				pixel_definitions< ePIXEL_FORMAT_A8R8G8B8 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case ePIXEL_FORMAT_A8B8G8R8:
				pixel_definitions< ePIXEL_FORMAT_A8B8G8R8 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case ePIXEL_FORMAT_RGB16F32F:
				pixel_definitions<	ePIXEL_FORMAT_RGB16F32F >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case ePIXEL_FORMAT_ARGB16F32F:
				pixel_definitions< ePIXEL_FORMAT_ARGB16F32F >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case ePIXEL_FORMAT_RGB32F:
				pixel_definitions< ePIXEL_FORMAT_RGB32F >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case ePIXEL_FORMAT_ARGB32F:
				pixel_definitions< ePIXEL_FORMAT_ARGB32F >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case ePIXEL_FORMAT_DXTC1:
				pixel_definitions< ePIXEL_FORMAT_DXTC1 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case ePIXEL_FORMAT_DXTC3:
				pixel_definitions< ePIXEL_FORMAT_DXTC3 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case ePIXEL_FORMAT_DXTC5:
				pixel_definitions< ePIXEL_FORMAT_DXTC5 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case ePIXEL_FORMAT_YUY2:
				pixel_definitions< ePIXEL_FORMAT_YUY2 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case ePIXEL_FORMAT_DEPTH16:
				pixel_definitions< ePIXEL_FORMAT_DEPTH16 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case ePIXEL_FORMAT_DEPTH24:
				pixel_definitions< ePIXEL_FORMAT_DEPTH24 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case ePIXEL_FORMAT_DEPTH24S8:
				pixel_definitions< ePIXEL_FORMAT_DEPTH24S8 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case ePIXEL_FORMAT_DEPTH32:
				pixel_definitions< ePIXEL_FORMAT_DEPTH32 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case ePIXEL_FORMAT_STENCIL1:
				pixel_definitions< ePIXEL_FORMAT_STENCIL1 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case ePIXEL_FORMAT_STENCIL8:
				pixel_definitions< ePIXEL_FORMAT_STENCIL8 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;
			}
		}

		ePIXEL_FORMAT GetFormatByName( String const & p_strFormat )
		{
			ePIXEL_FORMAT l_eReturn = ePIXEL_FORMAT_COUNT;

			for ( int i = 0; i < l_eReturn; ++i )
			{
				switch ( i )
				{
				case ePIXEL_FORMAT_L8:
					l_eReturn = ( p_strFormat == pixel_definitions< ePIXEL_FORMAT_L8 >::to_str() ? ePIXEL_FORMAT( i ) : ePIXEL_FORMAT_COUNT );
					break;

				case ePIXEL_FORMAT_L16F32F:
					l_eReturn = ( p_strFormat == pixel_definitions< ePIXEL_FORMAT_L16F32F >::to_str() ? ePIXEL_FORMAT( i ) : ePIXEL_FORMAT_COUNT );
					break;

				case ePIXEL_FORMAT_L32F:
					l_eReturn = ( p_strFormat == pixel_definitions< ePIXEL_FORMAT_L32F >::to_str() ? ePIXEL_FORMAT( i ) : ePIXEL_FORMAT_COUNT );
					break;

				case ePIXEL_FORMAT_A8L8:
					l_eReturn = ( p_strFormat == pixel_definitions< ePIXEL_FORMAT_A8L8 >::to_str() ? ePIXEL_FORMAT( i ) : ePIXEL_FORMAT_COUNT );
					break;

				case ePIXEL_FORMAT_AL16F32F:
					l_eReturn = ( p_strFormat == pixel_definitions< ePIXEL_FORMAT_AL16F32F >::to_str() ? ePIXEL_FORMAT( i ) : ePIXEL_FORMAT_COUNT );
					break;

				case ePIXEL_FORMAT_AL32F:
					l_eReturn = ( p_strFormat == pixel_definitions< ePIXEL_FORMAT_AL32F >::to_str() ? ePIXEL_FORMAT( i ) : ePIXEL_FORMAT_COUNT );
					break;

				case ePIXEL_FORMAT_A1R5G5B5:
					l_eReturn = ( p_strFormat == pixel_definitions< ePIXEL_FORMAT_A1R5G5B5 >::to_str() ? ePIXEL_FORMAT( i ) : ePIXEL_FORMAT_COUNT );
					break;

				case ePIXEL_FORMAT_A4R4G4B4:
					l_eReturn = ( p_strFormat == pixel_definitions< ePIXEL_FORMAT_A4R4G4B4 >::to_str() ? ePIXEL_FORMAT( i ) : ePIXEL_FORMAT_COUNT );
					break;

				case ePIXEL_FORMAT_R5G6B5:
					l_eReturn = ( p_strFormat == pixel_definitions< ePIXEL_FORMAT_R5G6B5 >::to_str() ? ePIXEL_FORMAT( i ) : ePIXEL_FORMAT_COUNT );
					break;

				case ePIXEL_FORMAT_R8G8B8:
					l_eReturn = ( p_strFormat == pixel_definitions< ePIXEL_FORMAT_R8G8B8 >::to_str() ? ePIXEL_FORMAT( i ) : ePIXEL_FORMAT_COUNT );
					break;

				case ePIXEL_FORMAT_B8G8R8:
					l_eReturn = ( p_strFormat == pixel_definitions< ePIXEL_FORMAT_B8G8R8 >::to_str() ? ePIXEL_FORMAT( i ) : ePIXEL_FORMAT_COUNT );
					break;

				case ePIXEL_FORMAT_A8R8G8B8:
					l_eReturn = ( p_strFormat == pixel_definitions< ePIXEL_FORMAT_A8R8G8B8 >::to_str() ? ePIXEL_FORMAT( i ) : ePIXEL_FORMAT_COUNT );
					break;

				case ePIXEL_FORMAT_A8B8G8R8:
					l_eReturn = ( p_strFormat == pixel_definitions< ePIXEL_FORMAT_A8B8G8R8 >::to_str() ? ePIXEL_FORMAT( i ) : ePIXEL_FORMAT_COUNT );
					break;

				case ePIXEL_FORMAT_RGB16F32F	:
					l_eReturn = ( p_strFormat == pixel_definitions< ePIXEL_FORMAT_RGB16F32F		>::to_str() ? ePIXEL_FORMAT( i ) : ePIXEL_FORMAT_COUNT );
					break;

				case ePIXEL_FORMAT_ARGB16F32F	:
					l_eReturn = ( p_strFormat == pixel_definitions< ePIXEL_FORMAT_ARGB16F32F	>::to_str() ? ePIXEL_FORMAT( i ) : ePIXEL_FORMAT_COUNT );
					break;

				case ePIXEL_FORMAT_RGB32F:
					l_eReturn = ( p_strFormat == pixel_definitions< ePIXEL_FORMAT_RGB32F >::to_str() ? ePIXEL_FORMAT( i ) : ePIXEL_FORMAT_COUNT );
					break;

				case ePIXEL_FORMAT_ARGB32F:
					l_eReturn = ( p_strFormat == pixel_definitions< ePIXEL_FORMAT_ARGB32F >::to_str() ? ePIXEL_FORMAT( i ) : ePIXEL_FORMAT_COUNT );
					break;

				case ePIXEL_FORMAT_DXTC1:
					l_eReturn = ( p_strFormat == pixel_definitions< ePIXEL_FORMAT_DXTC1 >::to_str() ? ePIXEL_FORMAT( i ) : ePIXEL_FORMAT_COUNT );
					break;

				case ePIXEL_FORMAT_DXTC3:
					l_eReturn = ( p_strFormat == pixel_definitions< ePIXEL_FORMAT_DXTC3 >::to_str() ? ePIXEL_FORMAT( i ) : ePIXEL_FORMAT_COUNT );
					break;

				case ePIXEL_FORMAT_DXTC5:
					l_eReturn = ( p_strFormat == pixel_definitions< ePIXEL_FORMAT_DXTC5 >::to_str() ? ePIXEL_FORMAT( i ) : ePIXEL_FORMAT_COUNT );
					break;

				case ePIXEL_FORMAT_YUY2:
					l_eReturn = ( p_strFormat == pixel_definitions< ePIXEL_FORMAT_YUY2 >::to_str() ? ePIXEL_FORMAT( i ) : ePIXEL_FORMAT_COUNT );
					break;

				case ePIXEL_FORMAT_DEPTH16:
					l_eReturn = ( p_strFormat == pixel_definitions< ePIXEL_FORMAT_DEPTH16 >::to_str() ? ePIXEL_FORMAT( i ) : ePIXEL_FORMAT_COUNT );
					break;

				case ePIXEL_FORMAT_DEPTH24:
					l_eReturn = ( p_strFormat == pixel_definitions< ePIXEL_FORMAT_DEPTH24 >::to_str() ? ePIXEL_FORMAT( i ) : ePIXEL_FORMAT_COUNT );
					break;

				case ePIXEL_FORMAT_DEPTH24S8:
					l_eReturn = ( p_strFormat == pixel_definitions< ePIXEL_FORMAT_DEPTH24S8 >::to_str() ? ePIXEL_FORMAT( i ) : ePIXEL_FORMAT_COUNT );
					break;

				case ePIXEL_FORMAT_DEPTH32:
					l_eReturn = ( p_strFormat == pixel_definitions< ePIXEL_FORMAT_DEPTH32 >::to_str() ? ePIXEL_FORMAT( i ) : ePIXEL_FORMAT_COUNT );
					break;

				case ePIXEL_FORMAT_STENCIL1:
					l_eReturn = ( p_strFormat == pixel_definitions< ePIXEL_FORMAT_STENCIL1 >::to_str() ? ePIXEL_FORMAT( i ) : ePIXEL_FORMAT_COUNT );
					break;

				case ePIXEL_FORMAT_STENCIL8:
					l_eReturn = ( p_strFormat == pixel_definitions< ePIXEL_FORMAT_STENCIL8 >::to_str() ? ePIXEL_FORMAT( i ) : ePIXEL_FORMAT_COUNT );
					break;
				}
			}

			return l_eReturn;
		}

		String GetFormatName( ePIXEL_FORMAT p_eFormat )
		{
			String l_strReturn;

			switch ( p_eFormat )
			{
			case ePIXEL_FORMAT_L8:
				l_strReturn = pixel_definitions< ePIXEL_FORMAT_L8 >::to_str();
				break;

			case ePIXEL_FORMAT_L16F32F:
				l_strReturn = pixel_definitions< ePIXEL_FORMAT_L16F32F >::to_str();
				break;

			case ePIXEL_FORMAT_L32F:
				l_strReturn = pixel_definitions< ePIXEL_FORMAT_L32F >::to_str();
				break;

			case ePIXEL_FORMAT_A8L8:
				l_strReturn = pixel_definitions< ePIXEL_FORMAT_A8L8 >::to_str();
				break;

			case ePIXEL_FORMAT_AL16F32F:
				l_strReturn = pixel_definitions< ePIXEL_FORMAT_AL16F32F >::to_str();
				break;

			case ePIXEL_FORMAT_AL32F:
				l_strReturn = pixel_definitions< ePIXEL_FORMAT_AL32F >::to_str();
				break;

			case ePIXEL_FORMAT_A1R5G5B5:
				l_strReturn = pixel_definitions< ePIXEL_FORMAT_A1R5G5B5 >::to_str();
				break;

			case ePIXEL_FORMAT_A4R4G4B4:
				l_strReturn = pixel_definitions< ePIXEL_FORMAT_A4R4G4B4 >::to_str();
				break;

			case ePIXEL_FORMAT_R5G6B5:
				l_strReturn = pixel_definitions< ePIXEL_FORMAT_R5G6B5 >::to_str();
				break;

			case ePIXEL_FORMAT_R8G8B8:
				l_strReturn = pixel_definitions< ePIXEL_FORMAT_R8G8B8 >::to_str();
				break;

			case ePIXEL_FORMAT_B8G8R8:
				l_strReturn = pixel_definitions< ePIXEL_FORMAT_B8G8R8 >::to_str();
				break;

			case ePIXEL_FORMAT_A8R8G8B8:
				l_strReturn = pixel_definitions< ePIXEL_FORMAT_A8R8G8B8 >::to_str();
				break;

			case ePIXEL_FORMAT_A8B8G8R8:
				l_strReturn = pixel_definitions< ePIXEL_FORMAT_A8B8G8R8 >::to_str();
				break;

			case ePIXEL_FORMAT_RGB16F32F:
				l_strReturn = pixel_definitions< ePIXEL_FORMAT_RGB16F32F >::to_str();
				break;

			case ePIXEL_FORMAT_ARGB16F32F:
				l_strReturn = pixel_definitions< ePIXEL_FORMAT_ARGB16F32F >::to_str();
				break;

			case ePIXEL_FORMAT_RGB32F:
				l_strReturn = pixel_definitions< ePIXEL_FORMAT_RGB32F >::to_str();
				break;

			case ePIXEL_FORMAT_ARGB32F:
				l_strReturn = pixel_definitions< ePIXEL_FORMAT_ARGB32F >::to_str();
				break;

			case ePIXEL_FORMAT_DXTC1:
				l_strReturn = pixel_definitions< ePIXEL_FORMAT_DXTC1 >::to_str();
				break;

			case ePIXEL_FORMAT_DXTC3:
				l_strReturn = pixel_definitions< ePIXEL_FORMAT_DXTC3 >::to_str();
				break;

			case ePIXEL_FORMAT_DXTC5:
				l_strReturn = pixel_definitions< ePIXEL_FORMAT_DXTC5 >::to_str();
				break;

			case ePIXEL_FORMAT_YUY2:
				l_strReturn = pixel_definitions< ePIXEL_FORMAT_YUY2 >::to_str();
				break;

			case ePIXEL_FORMAT_DEPTH16:
				l_strReturn = pixel_definitions< ePIXEL_FORMAT_DEPTH16 >::to_str();
				break;

			case ePIXEL_FORMAT_DEPTH24:
				l_strReturn = pixel_definitions< ePIXEL_FORMAT_DEPTH24 >::to_str();
				break;

			case ePIXEL_FORMAT_DEPTH24S8:
				l_strReturn = pixel_definitions< ePIXEL_FORMAT_DEPTH24S8 >::to_str();
				break;

			case ePIXEL_FORMAT_DEPTH32:
				l_strReturn = pixel_definitions< ePIXEL_FORMAT_DEPTH32 >::to_str();
				break;

			case ePIXEL_FORMAT_STENCIL1:
				l_strReturn = pixel_definitions< ePIXEL_FORMAT_STENCIL1 >::to_str();
				break;

			case ePIXEL_FORMAT_STENCIL8:
				l_strReturn = pixel_definitions< ePIXEL_FORMAT_STENCIL8 >::to_str();
				break;
			}

			return l_strReturn;
		}
	}
}
