#include "PixelFormat.hpp"
#include "PixelBuffer.hpp"

namespace Castor
{
#if defined( _X64 )

	namespace detail
	{
		const __m128i buffer_converter< PixelFormat::YUY2, PixelFormat::R8G8B8 >::umask	= _mm_set1_epi32( 0x0000FF00 );
		const __m128i buffer_converter< PixelFormat::YUY2, PixelFormat::R8G8B8 >::vmask	= _mm_set1_epi32( 0xFF000000 );
		const __m128i buffer_converter< PixelFormat::YUY2, PixelFormat::R8G8B8 >::ymask	= _mm_set1_epi32( 0x00FF00FF );
		const __m128i buffer_converter< PixelFormat::YUY2, PixelFormat::R8G8B8 >::uvsub	= _mm_set1_epi32( 0x00800080 );
		const __m128i buffer_converter< PixelFormat::YUY2, PixelFormat::R8G8B8 >::ysub	= _mm_set1_epi32( 0x00100010 );
		const __m128i buffer_converter< PixelFormat::YUY2, PixelFormat::R8G8B8 >::facy	= _mm_set1_epi32( 0x00400040 );// 1 * 64
		const __m128i buffer_converter< PixelFormat::YUY2, PixelFormat::R8G8B8 >::facrv	= _mm_set1_epi32( 0x00710071 );// 1.772 * 64
		const __m128i buffer_converter< PixelFormat::YUY2, PixelFormat::R8G8B8 >::facgu	= _mm_set1_epi32( 0x002D002D );// 0.714 * 64
		const __m128i buffer_converter< PixelFormat::YUY2, PixelFormat::R8G8B8 >::facgv	= _mm_set1_epi32( 0x00160016 );// 0.344 * 64
		const __m128i buffer_converter< PixelFormat::YUY2, PixelFormat::R8G8B8 >::facbu	= _mm_set1_epi32( 0x00590059 );// 1.402 * 64
		const __m128i buffer_converter< PixelFormat::YUY2, PixelFormat::R8G8B8 >::zero	= _mm_set1_epi32( 0x00000000 );

		const __m128i buffer_converter< PixelFormat::YUY2, PixelFormat::A8R8G8B8 >::amask	= _mm_set1_epi32( 0x000000FF );
		const __m128i buffer_converter< PixelFormat::YUY2, PixelFormat::A8R8G8B8 >::umask	= _mm_set1_epi32( 0x0000FF00 );
		const __m128i buffer_converter< PixelFormat::YUY2, PixelFormat::A8R8G8B8 >::vmask	= _mm_set1_epi32( 0xFF000000 );
		const __m128i buffer_converter< PixelFormat::YUY2, PixelFormat::A8R8G8B8 >::ymask	= _mm_set1_epi32( 0x00FF00FF );
		const __m128i buffer_converter< PixelFormat::YUY2, PixelFormat::A8R8G8B8 >::uvsub	= _mm_set1_epi32( 0x00800080 );
		const __m128i buffer_converter< PixelFormat::YUY2, PixelFormat::A8R8G8B8 >::ysub	= _mm_set1_epi32( 0x00100010 );
		const __m128i buffer_converter< PixelFormat::YUY2, PixelFormat::A8R8G8B8 >::facy	= _mm_set1_epi32( 0x00400040 );// 1 * 64
		const __m128i buffer_converter< PixelFormat::YUY2, PixelFormat::A8R8G8B8 >::facrv	= _mm_set1_epi32( 0x00710071 );// 1.772 * 64
		const __m128i buffer_converter< PixelFormat::YUY2, PixelFormat::A8R8G8B8 >::facgu	= _mm_set1_epi32( 0x002D002D );// 0.714 * 64
		const __m128i buffer_converter< PixelFormat::YUY2, PixelFormat::A8R8G8B8 >::facgv	= _mm_set1_epi32( 0x00160016 );// 0.344 * 64
		const __m128i buffer_converter< PixelFormat::YUY2, PixelFormat::A8R8G8B8 >::facbu	= _mm_set1_epi32( 0x00590059 );// 1.402 * 64
		const __m128i buffer_converter< PixelFormat::YUY2, PixelFormat::A8R8G8B8 >::zero	= _mm_set1_epi32( 0x00000000 );
	}

#endif

	namespace PF
	{
		PixelFormat GetPFWithoutAlpha( PixelFormat p_format )
		{
			PixelFormat l_return = PixelFormat::Count;

			switch ( p_format )
			{
			case PixelFormat::A8L8:
				l_return = pixel_definitions< PixelFormat::A8L8 >::NoAlphaPF;
				break;

			case PixelFormat::AL16F32F:
				l_return = pixel_definitions< PixelFormat::AL16F32F >::NoAlphaPF;
				break;

			case PixelFormat::AL32F:
				l_return = pixel_definitions< PixelFormat::AL32F >::NoAlphaPF;
				break;

			case PixelFormat::A1R5G5B5:
				l_return = pixel_definitions< PixelFormat::A1R5G5B5 >::NoAlphaPF;
				break;

			case PixelFormat::A4R4G4B4:
				l_return = pixel_definitions< PixelFormat::A4R4G4B4 >::NoAlphaPF;
				break;

			case PixelFormat::A8R8G8B8:
				l_return = pixel_definitions< PixelFormat::A8R8G8B8 >::NoAlphaPF;
				break;

			case PixelFormat::A8B8G8R8:
				l_return = pixel_definitions< PixelFormat::A8B8G8R8 >::NoAlphaPF;
				break;

			case PixelFormat::ARGB16F32F:
				l_return = pixel_definitions< PixelFormat::ARGB16F32F >::NoAlphaPF;
				break;

			case PixelFormat::ARGB32F:
				l_return = pixel_definitions< PixelFormat::ARGB32F >::NoAlphaPF;
				break;

			case PixelFormat::DXTC3:
				l_return = pixel_definitions< PixelFormat::DXTC3 >::NoAlphaPF;
				break;

			case PixelFormat::DXTC5:
				l_return = pixel_definitions< PixelFormat::DXTC5 >::NoAlphaPF;
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
			case PixelFormat::L8:
				l_return = pixel_definitions< PixelFormat::L8 >::Alpha;
				break;

			case PixelFormat::L16F32F:
				l_return = pixel_definitions< PixelFormat::L16F32F	>::Alpha;
				break;

			case PixelFormat::L32F:
				l_return = pixel_definitions< PixelFormat::L32F >::Alpha;
				break;

			case PixelFormat::A8L8:
				l_return = pixel_definitions< PixelFormat::A8L8 >::Alpha;
				break;

			case PixelFormat::AL16F32F:
				l_return = pixel_definitions< PixelFormat::AL16F32F >::Alpha;
				break;

			case PixelFormat::AL32F:
				l_return = pixel_definitions< PixelFormat::AL32F >::Alpha;
				break;

			case PixelFormat::A1R5G5B5:
				l_return = pixel_definitions< PixelFormat::A1R5G5B5 >::Alpha;
				break;

			case PixelFormat::A4R4G4B4:
				l_return = pixel_definitions< PixelFormat::A4R4G4B4 >::Alpha;
				break;

			case PixelFormat::R5G6B5:
				l_return = pixel_definitions< PixelFormat::R5G6B5 >::Alpha;
				break;

			case PixelFormat::R8G8B8:
				l_return = pixel_definitions< PixelFormat::R8G8B8 >::Alpha;
				break;

			case PixelFormat::B8G8R8:
				l_return = pixel_definitions< PixelFormat::B8G8R8 >::Alpha;
				break;

			case PixelFormat::A8R8G8B8:
				l_return = pixel_definitions< PixelFormat::A8R8G8B8 >::Alpha;
				break;

			case PixelFormat::A8B8G8R8:
				l_return = pixel_definitions< PixelFormat::A8B8G8R8 >::Alpha;
				break;

			case PixelFormat::RGB16F:
				l_return = pixel_definitions< PixelFormat::RGB16F >::Alpha;
				break;

			case PixelFormat::ARGB16F:
				l_return = pixel_definitions< PixelFormat::ARGB16F >::Alpha;
				break;

			case PixelFormat::RGB16F32F:
				l_return = pixel_definitions< PixelFormat::RGB16F32F >::Alpha;
				break;

			case PixelFormat::ARGB16F32F:
				l_return = pixel_definitions< PixelFormat::ARGB16F32F >::Alpha;
				break;

			case PixelFormat::RGB32F:
				l_return = pixel_definitions< PixelFormat::RGB32F >::Alpha;
				break;

			case PixelFormat::ARGB32F:
				l_return = pixel_definitions< PixelFormat::ARGB32F >::Alpha;
				break;

			case PixelFormat::DXTC1:
				l_return = pixel_definitions< PixelFormat::DXTC1 >::Alpha;
				break;

			case PixelFormat::DXTC3:
				l_return = pixel_definitions< PixelFormat::DXTC3 >::Alpha;
				break;

			case PixelFormat::DXTC5:
				l_return = pixel_definitions< PixelFormat::DXTC5 >::Alpha;
				break;

			case PixelFormat::YUY2:
				l_return = pixel_definitions< PixelFormat::YUY2 >::Alpha;
				break;

			case PixelFormat::D16:
				l_return = pixel_definitions< PixelFormat::D16 >::Alpha;
				break;

			case PixelFormat::D24:
				l_return = pixel_definitions< PixelFormat::D24 >::Alpha;
				break;

			case PixelFormat::D24S8:
				l_return = pixel_definitions< PixelFormat::D24S8 >::Alpha;
				break;

			case PixelFormat::D32:
				l_return = pixel_definitions< PixelFormat::D32 >::Alpha;
				break;

			case PixelFormat::D32F:
				l_return = pixel_definitions< PixelFormat::D32F >::Alpha;
				break;

			case PixelFormat::S1:
				l_return = pixel_definitions< PixelFormat::S1 >::Alpha;
				break;

			case PixelFormat::S8:
				l_return = pixel_definitions< PixelFormat::S8 >::Alpha;
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
			case PixelFormat::L8:
				l_return = pixel_definitions< PixelFormat::L8 >::Compressed;
				break;

			case PixelFormat::L16F32F:
				l_return = pixel_definitions< PixelFormat::L16F32F >::Compressed;
				break;

			case PixelFormat::L32F:
				l_return = pixel_definitions< PixelFormat::L32F >::Compressed;
				break;

			case PixelFormat::A8L8:
				l_return = pixel_definitions< PixelFormat::A8L8 >::Compressed;
				break;

			case PixelFormat::AL16F32F:
				l_return = pixel_definitions< PixelFormat::AL16F32F >::Compressed;
				break;

			case PixelFormat::AL32F:
				l_return = pixel_definitions< PixelFormat::AL32F >::Compressed;
				break;

			case PixelFormat::A1R5G5B5:
				l_return = pixel_definitions< PixelFormat::A1R5G5B5 >::Compressed;
				break;

			case PixelFormat::A4R4G4B4:
				l_return = pixel_definitions< PixelFormat::A4R4G4B4 >::Compressed;
				break;

			case PixelFormat::R5G6B5:
				l_return = pixel_definitions< PixelFormat::R5G6B5 >::Compressed;
				break;

			case PixelFormat::R8G8B8:
				l_return = pixel_definitions< PixelFormat::R8G8B8 >::Compressed;
				break;

			case PixelFormat::B8G8R8:
				l_return = pixel_definitions< PixelFormat::B8G8R8 >::Compressed;
				break;

			case PixelFormat::A8R8G8B8:
				l_return = pixel_definitions< PixelFormat::A8R8G8B8 >::Compressed;
				break;

			case PixelFormat::A8B8G8R8:
				l_return = pixel_definitions< PixelFormat::A8B8G8R8 >::Compressed;
				break;

			case PixelFormat::RGB16F:
				l_return = pixel_definitions< PixelFormat::RGB16F >::Compressed;
				break;

			case PixelFormat::ARGB16F:
				l_return = pixel_definitions< PixelFormat::ARGB16F >::Compressed;
				break;

			case PixelFormat::RGB16F32F:
				l_return = pixel_definitions< PixelFormat::RGB16F32F >::Compressed;
				break;

			case PixelFormat::ARGB16F32F:
				l_return = pixel_definitions< PixelFormat::ARGB16F32F >::Compressed;
				break;

			case PixelFormat::RGB32F:
				l_return = pixel_definitions< PixelFormat::RGB32F >::Compressed;
				break;

			case PixelFormat::ARGB32F:
				l_return = pixel_definitions< PixelFormat::ARGB32F >::Compressed;
				break;

			case PixelFormat::DXTC1:
				l_return = pixel_definitions< PixelFormat::DXTC1 >::Compressed;
				break;

			case PixelFormat::DXTC3:
				l_return = pixel_definitions< PixelFormat::DXTC3 >::Compressed;
				break;

			case PixelFormat::DXTC5:
				l_return = pixel_definitions< PixelFormat::DXTC5 >::Compressed;
				break;

			case PixelFormat::YUY2:
				l_return = pixel_definitions< PixelFormat::YUY2 >::Compressed;
				break;

			case PixelFormat::D16:
				l_return = pixel_definitions< PixelFormat::D16 >::Compressed;
				break;

			case PixelFormat::D24:
				l_return = pixel_definitions< PixelFormat::D24 >::Compressed;
				break;

			case PixelFormat::D24S8:
				l_return = pixel_definitions< PixelFormat::D24S8 >::Compressed;
				break;

			case PixelFormat::D32:
				l_return = pixel_definitions< PixelFormat::D32 >::Compressed;
				break;

			case PixelFormat::D32F:
				l_return = pixel_definitions< PixelFormat::D32F >::Compressed;
				break;

			case PixelFormat::S1:
				l_return = pixel_definitions< PixelFormat::S1 >::Compressed;
				break;

			case PixelFormat::S8:
				l_return = pixel_definitions< PixelFormat::S8 >::Compressed;
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
				l_return = PxBufferBase::create( p_pSrc->dimensions(), PixelFormat::A8L8, p_pSrc->const_ptr(), p_pSrc->format() );
				uint8_t * l_pBuffer = l_return->ptr();

				for ( uint32_t i = 0; i < l_return->size(); i += 2 )
				{
					l_pBuffer[0] = l_pBuffer[1];
					l_pBuffer++;
					l_pBuffer++;
				}

				l_return = PxBufferBase::create( p_pSrc->dimensions(), PixelFormat::L8, l_return->const_ptr(), l_return->format() );
				p_pSrc = PxBufferBase::create( p_pSrc->dimensions(), GetPFWithoutAlpha( p_pSrc->format() ), p_pSrc->const_ptr(), p_pSrc->format() );
			}

			return l_return;
		}

		void ReduceToAlpha( PxBufferBaseSPtr & p_pSrc )
		{
			if ( HasAlpha( p_pSrc->format() ) )
			{
				if ( p_pSrc->format() != PixelFormat::A8L8 )
				{
					p_pSrc = PxBufferBase::create( p_pSrc->dimensions(), PixelFormat::A8L8, p_pSrc->const_ptr(), p_pSrc->format() );
				}

				uint8_t * l_pBuffer = p_pSrc->ptr();

				for ( uint32_t i = 0; i < p_pSrc->size(); i += 2 )
				{
					l_pBuffer[0] = l_pBuffer[1];
					l_pBuffer++;
					l_pBuffer++;
				}
			}

			p_pSrc = PxBufferBase::create( p_pSrc->dimensions(), PixelFormat::L8, p_pSrc->const_ptr(), p_pSrc->format() );
		}

		uint8_t GetBytesPerPixel( PixelFormat p_format )
		{
			uint8_t l_return = 0;

			switch ( p_format )
			{
			case PixelFormat::L8:
				l_return = pixel_definitions< PixelFormat::L8 >::Size;
				break;

			case PixelFormat::L16F32F:
				l_return = pixel_definitions<	PixelFormat::L16F32F >::Size;
				break;

			case PixelFormat::L32F:
				l_return = pixel_definitions< PixelFormat::L32F >::Size;
				break;

			case PixelFormat::A8L8:
				l_return = pixel_definitions< PixelFormat::A8L8 >::Size;
				break;

			case PixelFormat::AL16F32F:
				l_return = pixel_definitions<	PixelFormat::AL16F32F >::Size;
				break;

			case PixelFormat::AL32F:
				l_return = pixel_definitions< PixelFormat::AL32F >::Size;
				break;

			case PixelFormat::A1R5G5B5:
				l_return = pixel_definitions< PixelFormat::A1R5G5B5 >::Size;
				break;

			case PixelFormat::A4R4G4B4:
				l_return = pixel_definitions< PixelFormat::A4R4G4B4 >::Size;
				break;

			case PixelFormat::R5G6B5:
				l_return = pixel_definitions< PixelFormat::R5G6B5 >::Size;
				break;

			case PixelFormat::R8G8B8:
				l_return = pixel_definitions< PixelFormat::R8G8B8 >::Size;
				break;

			case PixelFormat::B8G8R8:
				l_return = pixel_definitions< PixelFormat::B8G8R8 >::Size;
				break;

			case PixelFormat::A8R8G8B8:
				l_return = pixel_definitions< PixelFormat::A8R8G8B8 >::Size;
				break;

			case PixelFormat::A8B8G8R8:
				l_return = pixel_definitions< PixelFormat::A8B8G8R8 >::Size;
				break;

			case PixelFormat::RGB16F:
				l_return = pixel_definitions<	PixelFormat::RGB16F >::Size;
				break;

			case PixelFormat::ARGB16F:
				l_return = pixel_definitions<	PixelFormat::ARGB16F >::Size;
				break;

			case PixelFormat::RGB16F32F:
				l_return = pixel_definitions<	PixelFormat::RGB16F32F >::Size;
				break;

			case PixelFormat::ARGB16F32F:
				l_return = pixel_definitions<	PixelFormat::ARGB16F32F >::Size;
				break;

			case PixelFormat::RGB32F:
				l_return = pixel_definitions< PixelFormat::RGB32F >::Size;
				break;

			case PixelFormat::ARGB32F:
				l_return = pixel_definitions< PixelFormat::ARGB32F >::Size;
				break;

			case PixelFormat::DXTC1:
				l_return = pixel_definitions<	PixelFormat::DXTC1 >::Size;
				break;

			case PixelFormat::DXTC3:
				l_return = pixel_definitions<	PixelFormat::DXTC3 >::Size;
				break;

			case PixelFormat::DXTC5:
				l_return = pixel_definitions<	PixelFormat::DXTC5 >::Size;
				break;

			case PixelFormat::YUY2:
				l_return = pixel_definitions<	PixelFormat::YUY2 >::Size;
				break;

			case PixelFormat::D16:
				l_return = pixel_definitions< PixelFormat::D16 >::Size;
				break;

			case PixelFormat::D24:
				l_return = pixel_definitions< PixelFormat::D24 >::Size;
				break;

			case PixelFormat::D24S8:
				l_return = pixel_definitions< PixelFormat::D24S8 >::Size;
				break;

			case PixelFormat::D32:
				l_return = pixel_definitions< PixelFormat::D32 >::Size;
				break;

			case PixelFormat::D32F:
				l_return = pixel_definitions< PixelFormat::D32F >::Size;
				break;

			case PixelFormat::S1:
				l_return = pixel_definitions<	PixelFormat::S1 >::Size;
				break;

			case PixelFormat::S8:
				l_return = pixel_definitions< PixelFormat::S8 >::Size;
				break;
			}

			return l_return;
		}

		void ConvertPixel( PixelFormat p_eSrcFmt, uint8_t const *& p_pSrc, PixelFormat p_eDestFmt, uint8_t *& p_pDest )
		{
			switch ( p_eSrcFmt )
			{
			case PixelFormat::L8:
				pixel_definitions< PixelFormat::L8 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::L16F32F:
				pixel_definitions< PixelFormat::L16F32F >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::L32F:
				pixel_definitions< PixelFormat::L32F >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::A8L8:
				pixel_definitions< PixelFormat::A8L8 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::AL16F32F:
				pixel_definitions< PixelFormat::AL16F32F >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::AL32F:
				pixel_definitions< PixelFormat::AL32F >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::A1R5G5B5:
				pixel_definitions< PixelFormat::A1R5G5B5 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::A4R4G4B4:
				pixel_definitions< PixelFormat::A4R4G4B4 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::R5G6B5:
				pixel_definitions< PixelFormat::R5G6B5 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::R8G8B8:
				pixel_definitions< PixelFormat::R8G8B8 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::B8G8R8:
				pixel_definitions< PixelFormat::B8G8R8 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::A8R8G8B8:
				pixel_definitions< PixelFormat::A8R8G8B8 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::A8B8G8R8:
				pixel_definitions< PixelFormat::A8B8G8R8 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::RGB16F:
				pixel_definitions< PixelFormat::RGB16F >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::ARGB16F:
				pixel_definitions< PixelFormat::ARGB16F >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::RGB16F32F:
				pixel_definitions< PixelFormat::RGB16F32F >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::ARGB16F32F:
				pixel_definitions< PixelFormat::ARGB16F32F >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::RGB32F:
				pixel_definitions< PixelFormat::RGB32F >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::ARGB32F:
				pixel_definitions< PixelFormat::ARGB32F >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::DXTC1:
				pixel_definitions< PixelFormat::DXTC1 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::DXTC3:
				pixel_definitions< PixelFormat::DXTC3 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::DXTC5:
				pixel_definitions< PixelFormat::DXTC5 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::YUY2:
				pixel_definitions< PixelFormat::YUY2 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::D16:
				pixel_definitions< PixelFormat::D16 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::D24:
				pixel_definitions< PixelFormat::D24 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::D24S8:
				pixel_definitions< PixelFormat::D24S8 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::D32:
				pixel_definitions< PixelFormat::D32 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::D32F:
				pixel_definitions< PixelFormat::D32F >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::S1:
				pixel_definitions< PixelFormat::S1 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;

			case PixelFormat::S8:
				pixel_definitions< PixelFormat::S8 >::convert( p_pSrc, p_pDest, p_eDestFmt );
				break;
			}
		}

		void ConvertBuffer( PixelFormat p_eSrcFormat, uint8_t const * p_pSrcBuffer, uint32_t p_uiSrcSize, PixelFormat p_eDstFormat, uint8_t * p_pDstBuffer, uint32_t p_uiDstSize )
		{
			switch ( p_eSrcFormat )
			{
			case PixelFormat::L8:
				pixel_definitions< PixelFormat::L8 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::L16F32F:
				pixel_definitions< PixelFormat::L16F32F >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::L32F:
				pixel_definitions< PixelFormat::L32F >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::A8L8:
				pixel_definitions< PixelFormat::A8L8 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::AL16F32F:
				pixel_definitions< PixelFormat::AL16F32F >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::AL32F:
				pixel_definitions< PixelFormat::AL32F >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::A1R5G5B5:
				pixel_definitions< PixelFormat::A1R5G5B5 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::A4R4G4B4:
				pixel_definitions< PixelFormat::A4R4G4B4 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::R5G6B5:
				pixel_definitions< PixelFormat::R5G6B5 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::R8G8B8:
				pixel_definitions< PixelFormat::R8G8B8 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::B8G8R8:
				pixel_definitions< PixelFormat::B8G8R8 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::A8R8G8B8:
				pixel_definitions< PixelFormat::A8R8G8B8 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::A8B8G8R8:
				pixel_definitions< PixelFormat::A8B8G8R8 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::RGB16F:
				pixel_definitions< PixelFormat::RGB16F >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::ARGB16F:
				pixel_definitions< PixelFormat::ARGB16F >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::RGB16F32F:
				pixel_definitions< PixelFormat::RGB16F32F >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::ARGB16F32F:
				pixel_definitions< PixelFormat::ARGB16F32F >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::RGB32F:
				pixel_definitions< PixelFormat::RGB32F >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::ARGB32F:
				pixel_definitions< PixelFormat::ARGB32F >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::DXTC1:
				pixel_definitions< PixelFormat::DXTC1 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::DXTC3:
				pixel_definitions< PixelFormat::DXTC3 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::DXTC5:
				pixel_definitions< PixelFormat::DXTC5 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::YUY2:
				pixel_definitions< PixelFormat::YUY2 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::D16:
				pixel_definitions< PixelFormat::D16 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::D24:
				pixel_definitions< PixelFormat::D24 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::D24S8:
				pixel_definitions< PixelFormat::D24S8 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::D32:
				pixel_definitions< PixelFormat::D32 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::D32F:
				pixel_definitions< PixelFormat::D32F >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::S1:
				pixel_definitions< PixelFormat::S1 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::S8:
				pixel_definitions< PixelFormat::S8 >::convert( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
				break;
			}
		}

		PixelFormat GetFormatByName( String const & p_strFormat )
		{
			PixelFormat l_return = PixelFormat::Count;

			for ( int i = 0; i < int( l_return ); ++i )
			{
				switch ( PixelFormat( i ) )
				{
				case PixelFormat::L8:
					l_return = ( p_strFormat == pixel_definitions< PixelFormat::L8 >::to_str() ? PixelFormat( i ) : PixelFormat::Count );
					break;

				case PixelFormat::L16F32F:
					l_return = ( p_strFormat == pixel_definitions< PixelFormat::L16F32F >::to_str() ? PixelFormat( i ) : PixelFormat::Count );
					break;

				case PixelFormat::L32F:
					l_return = ( p_strFormat == pixel_definitions< PixelFormat::L32F >::to_str() ? PixelFormat( i ) : PixelFormat::Count );
					break;

				case PixelFormat::A8L8:
					l_return = ( p_strFormat == pixel_definitions< PixelFormat::A8L8 >::to_str() ? PixelFormat( i ) : PixelFormat::Count );
					break;

				case PixelFormat::AL16F32F:
					l_return = ( p_strFormat == pixel_definitions< PixelFormat::AL16F32F >::to_str() ? PixelFormat( i ) : PixelFormat::Count );
					break;

				case PixelFormat::AL32F:
					l_return = ( p_strFormat == pixel_definitions< PixelFormat::AL32F >::to_str() ? PixelFormat( i ) : PixelFormat::Count );
					break;

				case PixelFormat::A1R5G5B5:
					l_return = ( p_strFormat == pixel_definitions< PixelFormat::A1R5G5B5 >::to_str() ? PixelFormat( i ) : PixelFormat::Count );
					break;

				case PixelFormat::A4R4G4B4:
					l_return = ( p_strFormat == pixel_definitions< PixelFormat::A4R4G4B4 >::to_str() ? PixelFormat( i ) : PixelFormat::Count );
					break;

				case PixelFormat::R5G6B5:
					l_return = ( p_strFormat == pixel_definitions< PixelFormat::R5G6B5 >::to_str() ? PixelFormat( i ) : PixelFormat::Count );
					break;

				case PixelFormat::R8G8B8:
					l_return = ( p_strFormat == pixel_definitions< PixelFormat::R8G8B8 >::to_str() ? PixelFormat( i ) : PixelFormat::Count );
					break;

				case PixelFormat::B8G8R8:
					l_return = ( p_strFormat == pixel_definitions< PixelFormat::B8G8R8 >::to_str() ? PixelFormat( i ) : PixelFormat::Count );
					break;

				case PixelFormat::A8R8G8B8:
					l_return = ( p_strFormat == pixel_definitions< PixelFormat::A8R8G8B8 >::to_str() ? PixelFormat( i ) : PixelFormat::Count );
					break;

				case PixelFormat::A8B8G8R8:
					l_return = ( p_strFormat == pixel_definitions< PixelFormat::A8B8G8R8 >::to_str() ? PixelFormat( i ) : PixelFormat::Count );
					break;

				case PixelFormat::RGB16F:
					l_return = ( p_strFormat == pixel_definitions< PixelFormat::RGB16F >::to_str() ? PixelFormat( i ) : PixelFormat::Count );
					break;

				case PixelFormat::ARGB16F:
					l_return = ( p_strFormat == pixel_definitions< PixelFormat::ARGB16F >::to_str() ? PixelFormat( i ) : PixelFormat::Count );
					break;

				case PixelFormat::RGB16F32F:
					l_return = ( p_strFormat == pixel_definitions< PixelFormat::RGB16F32F >::to_str() ? PixelFormat( i ) : PixelFormat::Count );
					break;

				case PixelFormat::ARGB16F32F:
					l_return = ( p_strFormat == pixel_definitions< PixelFormat::ARGB16F32F >::to_str() ? PixelFormat( i ) : PixelFormat::Count );
					break;

				case PixelFormat::RGB32F:
					l_return = ( p_strFormat == pixel_definitions< PixelFormat::RGB32F >::to_str() ? PixelFormat( i ) : PixelFormat::Count );
					break;

				case PixelFormat::ARGB32F:
					l_return = ( p_strFormat == pixel_definitions< PixelFormat::ARGB32F >::to_str() ? PixelFormat( i ) : PixelFormat::Count );
					break;

				case PixelFormat::DXTC1:
					l_return = ( p_strFormat == pixel_definitions< PixelFormat::DXTC1 >::to_str() ? PixelFormat( i ) : PixelFormat::Count );
					break;

				case PixelFormat::DXTC3:
					l_return = ( p_strFormat == pixel_definitions< PixelFormat::DXTC3 >::to_str() ? PixelFormat( i ) : PixelFormat::Count );
					break;

				case PixelFormat::DXTC5:
					l_return = ( p_strFormat == pixel_definitions< PixelFormat::DXTC5 >::to_str() ? PixelFormat( i ) : PixelFormat::Count );
					break;

				case PixelFormat::YUY2:
					l_return = ( p_strFormat == pixel_definitions< PixelFormat::YUY2 >::to_str() ? PixelFormat( i ) : PixelFormat::Count );
					break;

				case PixelFormat::D16:
					l_return = ( p_strFormat == pixel_definitions< PixelFormat::D16 >::to_str() ? PixelFormat( i ) : PixelFormat::Count );
					break;

				case PixelFormat::D24:
					l_return = ( p_strFormat == pixel_definitions< PixelFormat::D24 >::to_str() ? PixelFormat( i ) : PixelFormat::Count );
					break;

				case PixelFormat::D24S8:
					l_return = ( p_strFormat == pixel_definitions< PixelFormat::D24S8 >::to_str() ? PixelFormat( i ) : PixelFormat::Count );
					break;

				case PixelFormat::D32:
					l_return = ( p_strFormat == pixel_definitions< PixelFormat::D32 >::to_str() ? PixelFormat( i ) : PixelFormat::Count );
					break;

				case PixelFormat::D32F:
					l_return = ( p_strFormat == pixel_definitions< PixelFormat::D32F >::to_str() ? PixelFormat( i ) : PixelFormat::Count );
					break;

				case PixelFormat::S1:
					l_return = ( p_strFormat == pixel_definitions< PixelFormat::S1 >::to_str() ? PixelFormat( i ) : PixelFormat::Count );
					break;

				case PixelFormat::S8:
					l_return = ( p_strFormat == pixel_definitions< PixelFormat::S8 >::to_str() ? PixelFormat( i ) : PixelFormat::Count );
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
			case PixelFormat::L8:
				l_return = pixel_definitions< PixelFormat::L8 >::to_str();
				break;

			case PixelFormat::L16F32F:
				l_return = pixel_definitions< PixelFormat::L16F32F >::to_str();
				break;

			case PixelFormat::L32F:
				l_return = pixel_definitions< PixelFormat::L32F >::to_str();
				break;

			case PixelFormat::A8L8:
				l_return = pixel_definitions< PixelFormat::A8L8 >::to_str();
				break;

			case PixelFormat::AL16F32F:
				l_return = pixel_definitions< PixelFormat::AL16F32F >::to_str();
				break;

			case PixelFormat::AL32F:
				l_return = pixel_definitions< PixelFormat::AL32F >::to_str();
				break;

			case PixelFormat::A1R5G5B5:
				l_return = pixel_definitions< PixelFormat::A1R5G5B5 >::to_str();
				break;

			case PixelFormat::A4R4G4B4:
				l_return = pixel_definitions< PixelFormat::A4R4G4B4 >::to_str();
				break;

			case PixelFormat::R5G6B5:
				l_return = pixel_definitions< PixelFormat::R5G6B5 >::to_str();
				break;

			case PixelFormat::R8G8B8:
				l_return = pixel_definitions< PixelFormat::R8G8B8 >::to_str();
				break;

			case PixelFormat::B8G8R8:
				l_return = pixel_definitions< PixelFormat::B8G8R8 >::to_str();
				break;

			case PixelFormat::A8R8G8B8:
				l_return = pixel_definitions< PixelFormat::A8R8G8B8 >::to_str();
				break;

			case PixelFormat::A8B8G8R8:
				l_return = pixel_definitions< PixelFormat::A8B8G8R8 >::to_str();
				break;

			case PixelFormat::RGB16F:
				l_return = pixel_definitions< PixelFormat::RGB16F >::to_str();
				break;

			case PixelFormat::ARGB16F:
				l_return = pixel_definitions< PixelFormat::ARGB16F >::to_str();
				break;

			case PixelFormat::RGB16F32F:
				l_return = pixel_definitions< PixelFormat::RGB16F32F >::to_str();
				break;

			case PixelFormat::ARGB16F32F:
				l_return = pixel_definitions< PixelFormat::ARGB16F32F >::to_str();
				break;

			case PixelFormat::RGB32F:
				l_return = pixel_definitions< PixelFormat::RGB32F >::to_str();
				break;

			case PixelFormat::ARGB32F:
				l_return = pixel_definitions< PixelFormat::ARGB32F >::to_str();
				break;

			case PixelFormat::DXTC1:
				l_return = pixel_definitions< PixelFormat::DXTC1 >::to_str();
				break;

			case PixelFormat::DXTC3:
				l_return = pixel_definitions< PixelFormat::DXTC3 >::to_str();
				break;

			case PixelFormat::DXTC5:
				l_return = pixel_definitions< PixelFormat::DXTC5 >::to_str();
				break;

			case PixelFormat::YUY2:
				l_return = pixel_definitions< PixelFormat::YUY2 >::to_str();
				break;

			case PixelFormat::D16:
				l_return = pixel_definitions< PixelFormat::D16 >::to_str();
				break;

			case PixelFormat::D24:
				l_return = pixel_definitions< PixelFormat::D24 >::to_str();
				break;

			case PixelFormat::D24S8:
				l_return = pixel_definitions< PixelFormat::D24S8 >::to_str();
				break;

			case PixelFormat::D32:
				l_return = pixel_definitions< PixelFormat::D32 >::to_str();
				break;

			case PixelFormat::D32F:
				l_return = pixel_definitions< PixelFormat::D32F >::to_str();
				break;

			case PixelFormat::S1:
				l_return = pixel_definitions< PixelFormat::S1 >::to_str();
				break;

			case PixelFormat::S8:
				l_return = pixel_definitions< PixelFormat::S8 >::to_str();
				break;
			}

			return l_return;
		}
	}
}
