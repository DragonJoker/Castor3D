#include <emmintrin.h>
#include <cstring>
#include "UnsupportedFormatException.hpp"
#include "Miscellaneous/StringUtils.hpp"

#if defined( max )
#	undef max
#endif

#if defined( min )
#	undef min
#endif

namespace Castor
{
	static inline void HalfToFloat( float & target, uint16_t const * source )
	{
		uint32_t * xp = ( uint32_t * )&target; // Type pun output as an unsigned 32-bit int
		uint16_t h = *source;

		if ( ( h & 0x7FFFu ) == 0 )
		{
			// Signed zero
			*xp = ( ( uint32_t )h ) << 16;  // Return the signed zero
		}
		else
		{
			// Not zero
			uint16_t hs = h & 0x8000u;  // Pick off sign bit
			uint16_t he = h & 0x7C00u;  // Pick off exponent bits
			uint16_t hm = h & 0x03FFu;  // Pick off mantissa bits

			if ( he == 0 )
			{
				// Denormal will convert to normalized
				int32_t e = -1; // The following loop figures out how much extra to adjust the exponent

				do
				{
					e++;
					hm <<= 1;
				}
				while ( ( hm & 0x0400u ) == 0 ); // Shift until leading bit overflows into exponent bit

				uint32_t xs = ( ( uint32_t )hs ) << 16; // Sign bit
				int32_t xes = ( ( int32_t )( he >> 10 ) ) - 15 + 127 - e; // Exponent unbias the halfp, then bias the single
				uint32_t xe = ( uint32_t )( xes << 23 ); // Exponent
				uint32_t xm = ( ( uint32_t )( hm & 0x03FFu ) ) << 13; // Mantissa
				*xp = ( xs | xe | xm ); // Combine sign bit, exponent bits, and mantissa bits
			}
			else if ( he == 0x7C00u )
			{
				// Inf or NaN (all the exponent bits are set)
				if ( hm == 0 )
				{
					// If mantissa is zero ...
					*xp = ( ( ( uint32_t )hs ) << 16 ) | ( ( uint32_t )0x7F800000u ); // Signed Inf
				}
				else
				{
					*xp = ( uint32_t )0xFFC00000u; // NaN, only 1st mantissa bit set
				}
			}
			else
			{
				// Normalized number
				uint32_t xs = ( ( uint32_t )hs ) << 16; // Sign bit
				int32_t xes = ( ( int32_t )( he >> 10 ) ) - 15 + 127; // Exponent unbias the halfp, then bias the single
				uint32_t xe = ( uint32_t )( xes << 23 ); // Exponent
				uint32_t xm = ( ( uint32_t )hm ) << 13; // Mantissa
				*xp = ( xs | xe | xm ); // Combine sign bit, exponent bits, and mantissa bits
			}
		}
	}

	static inline void FloatToHalf( uint16_t * target, float source )
	{
		uint16_t * hp = target; // Type pun output as an unsigned 16-bit int
		uint32_t x = *reinterpret_cast< uint32_t * >( &source ); // Type pun input as an unsigned 32-bit int

		if ( ( x & 0x7FFFFFFFu ) == 0 )
		{
			// Signed zero
			*hp = uint16_t( x >> 16 );  // Return the signed zero
		}
		else
		{
			// Not zero
			uint32_t xs = x & 0x80000000u;  // Pick off sign bit
			uint32_t xe = x & 0x7F800000u;  // Pick off exponent bits
			uint32_t xm = x & 0x007FFFFFu;  // Pick off mantissa bits

			if ( xe == 0 )
			{
				// Denormal will underflow, return a signed zero
				*hp = uint16_t( xs >> 16 );
			}
			else if ( xe == 0x7F800000u )
			{
				// Inf or NaN (all the exponent bits are set)
				if ( xm == 0 )
				{
					// If mantissa is zero ...
					*hp = uint16_t( ( xs >> 16 ) | 0x7C00u ); // Signed Inf
				}
				else
				{
					*hp = uint16_t( 0xFE00u ); // NaN, only 1st mantissa bit set
				}
			}
			else
			{
				// Normalized number
				uint16_t hs( xs >> 16 ); // Sign bit
				int32_t hes( ( ( int )( xe >> 23 ) ) - 127 + 15 ); // Exponent unbias the single, then bias the halfp
				uint16_t hm{};

				if ( hes >= 0x1F )
				{
					// Overflow
					*hp = uint16_t( ( xs >> 16 ) | 0x7C00u ); // Signed Inf
				}
				else if ( hes <= 0 )
				{
					// Underflow
					if ( ( 14 - hes ) > 24 )
					{
						// Mantissa shifted all the way off & no rounding possibility
						hm = 0u;  // Set mantissa to zero
					}
					else
					{
						xm |= 0x00800000u;  // Add the hidden leading bit
						hm = uint16_t( xm >> ( 14 - hes ) ); // Mantissa

						if ( ( xm >> ( 13 - hes ) ) & 0x00000001u ) // Check for rounding
						{
							hm += ( uint16_t )1u; // Round, might overflow into exp bit, but this is OK
						}
					}

					*hp = ( hs | hm ); // Combine sign bit and mantissa bits, biased exponent is zero
				}
				else
				{
					uint16_t he( hes << 10 ); // Exponent
					uint16_t hm( xm >> 13 ); // Mantissa

					if ( xm & 0x00001000u ) // Check for rounding
					{
						*hp = ( hs | he | hm ) + ( uint16_t )1u; // Round, might overflow to inf, this is OK
					}
					else
					{
						*hp = ( hs | he | hm );  // No rounding
					}
				}
			}
		}
	}

	//!\~english Specialisation for PixelFormat::L8	\~french Spécialisation pour PixelFormat::L8
	template<> struct component< PixelFormat::L8 >
	{
		static inline uint8_t L8( uint8_t const * p_pSrc )
		{
			return p_pSrc[0];
		}
		static inline uint8_t R8( uint8_t const * p_pSrc )
		{
			return p_pSrc[0];
		}
		static inline uint8_t G8( uint8_t const * p_pSrc )
		{
			return p_pSrc[0];
		}
		static inline uint8_t B8( uint8_t const * p_pSrc )
		{
			return p_pSrc[0];
		}
		static inline uint8_t A8( uint8_t const * )
		{
			return 0xFF;
		}
		static inline float L32F( uint8_t const * p_pSrc )
		{
			return L8( p_pSrc ) / 255.0f;
		}
		static inline float R32F( uint8_t const * p_pSrc )
		{
			return R8( p_pSrc ) / 255.0f;
		}
		static inline float G32F( uint8_t const * p_pSrc )
		{
			return G8( p_pSrc ) / 255.0f;
		}
		static inline float B32F( uint8_t const * p_pSrc )
		{
			return B8( p_pSrc ) / 255.0f;
		}
		static inline float A32F( uint8_t const * p_pSrc )
		{
			return A8( p_pSrc ) / 255.0f;
		}
		static inline void L8( uint8_t * p_pSrc, uint8_t p_val )
		{
			p_pSrc[0] = p_val;
		}
		static inline void R8( uint8_t * p_pSrc, uint8_t p_val )
		{
			p_pSrc[0] = p_val;
		}
		static inline void G8( uint8_t * p_pSrc, uint8_t p_val )
		{
			p_pSrc[0] = p_val;
		}
		static inline void B8( uint8_t * p_pSrc, uint8_t p_val )
		{
			p_pSrc[0] = p_val;
		}
		static inline void A8( uint8_t *, uint8_t )
		{
		}
		static inline void L32F( uint8_t * p_pSrc, float p_val )
		{
			L8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}
		static inline void R32F( uint8_t * p_pSrc, float p_val )
		{
			R8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}
		static inline void G32F( uint8_t * p_pSrc, float p_val )
		{
			G8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}
		static inline void B32F( uint8_t * p_pSrc, float p_val )
		{
			B8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}
		static inline void A32F( uint8_t *, float )
		{
		}
	};
	//!\~english Specialisation for PixelFormat::L16F32F	\~french Spécialisation pour PixelFormat::L16F32F
	template<> struct component< PixelFormat::L16F32F >
	{
#define src ( *reinterpret_cast< float const * >( p_pSrc ) )
#define dst ( *reinterpret_cast< float * >( p_pSrc ) )

		static inline uint8_t L8( uint8_t const * p_pSrc )
		{
			return uint8_t( L32F( p_pSrc ) * 255 );
		}
		static inline uint8_t R8( uint8_t const * p_pSrc )
		{
			return uint8_t( R32F( p_pSrc ) * 255 );
		}
		static inline uint8_t G8( uint8_t const * p_pSrc )
		{
			return uint8_t( G32F( p_pSrc ) * 255 );
		}
		static inline uint8_t B8( uint8_t const * p_pSrc )
		{
			return uint8_t( B32F( p_pSrc ) * 255 );
		}
		static inline uint8_t A8( uint8_t const * p_pSrc )
		{
			return uint8_t( A32F( p_pSrc ) * 255 );
		}
		static inline float L32F( uint8_t const * p_pSrc )
		{
			return src;
		}
		static inline float R32F( uint8_t const * p_pSrc )
		{
			return src;
		}
		static inline float G32F( uint8_t const * p_pSrc )
		{
			return src;
		}
		static inline float B32F( uint8_t const * p_pSrc )
		{
			return src;
		}
		static inline float A32F( uint8_t const * )
		{
			return 1.0f;
		}
		static inline void L8( uint8_t * p_pSrc, uint8_t p_val )
		{
			L32F( p_pSrc, float( p_val ) / 255 );
		}
		static inline void R8( uint8_t * p_pSrc, uint8_t p_val )
		{
			R32F( p_pSrc, float( p_val ) / 255 );
		}
		static inline void G8( uint8_t * p_pSrc, uint8_t p_val )
		{
			G32F( p_pSrc, float( p_val ) / 255 );
		}
		static inline void B8( uint8_t * p_pSrc, uint8_t p_val )
		{
			B32F( p_pSrc, float( p_val ) / 255 );
		}
		static inline void A8( uint8_t *, uint8_t pal )
		{
		}
		static inline void L32F( uint8_t * p_pSrc, float p_val )
		{
			dst = p_val;
		}
		static inline void R32F( uint8_t * p_pSrc, float p_val )
		{
			dst = p_val;
		}
		static inline void G32F( uint8_t * p_pSrc, float p_val )
		{
			dst = p_val;
		}
		static inline void B32F( uint8_t * p_pSrc, float p_val )
		{
			dst = p_val;
		}
		static inline void A32F( uint8_t *, float )
		{
		}

#undef src
#undef dst
	};
	//!\~english Specialisation for PixelFormat::L32F	\~french Spécialisation pour PixelFormat::L32F
	template<> struct component< PixelFormat::L32F >
	{
#define src ( *reinterpret_cast< float const * >( p_pSrc ) )
#define dst ( *reinterpret_cast< float * >( p_pSrc ) )

		static inline uint8_t L8( uint8_t const * p_pSrc )
		{
			return uint8_t( L32F( p_pSrc ) * 255 );
		}
		static inline uint8_t R8( uint8_t const * p_pSrc )
		{
			return uint8_t( R32F( p_pSrc ) * 255 );
		}
		static inline uint8_t G8( uint8_t const * p_pSrc )
		{
			return uint8_t( G32F( p_pSrc ) * 255 );
		}
		static inline uint8_t B8( uint8_t const * p_pSrc )
		{
			return uint8_t( B32F( p_pSrc ) * 255 );
		}
		static inline uint8_t A8( uint8_t const * p_pSrc )
		{
			return uint8_t( A32F( p_pSrc ) * 255 );
		}
		static inline float L32F( uint8_t const * p_pSrc )
		{
			return src;
		}
		static inline float R32F( uint8_t const * p_pSrc )
		{
			return src;
		}
		static inline float G32F( uint8_t const * p_pSrc )
		{
			return src;
		}
		static inline float B32F( uint8_t const * p_pSrc )
		{
			return src;
		}
		static inline float A32F( uint8_t const * )
		{
			return 1.0f;
		}
		static inline void L8( uint8_t * p_pSrc, uint8_t p_val )
		{
			L32F( p_pSrc, float( p_val ) / 255 );
		}
		static inline void R8( uint8_t * p_pSrc, uint8_t p_val )
		{
			R32F( p_pSrc, float( p_val ) / 255 );
		}
		static inline void G8( uint8_t * p_pSrc, uint8_t p_val )
		{
			G32F( p_pSrc, float( p_val ) / 255 );
		}
		static inline void B8( uint8_t * p_pSrc, uint8_t p_val )
		{
			B32F( p_pSrc, float( p_val ) / 255 );
		}
		static inline void A8( uint8_t *, uint8_t pal )
		{
		}
		static inline void L32F( uint8_t * p_pSrc, float p_val )
		{
			dst = p_val;
		}
		static inline void R32F( uint8_t * p_pSrc, float p_val )
		{
			dst = p_val;
		}
		static inline void G32F( uint8_t * p_pSrc, float p_val )
		{
			dst = p_val;
		}
		static inline void B32F( uint8_t * p_pSrc, float p_val )
		{
			dst = p_val;
		}
		static inline void A32F( uint8_t *, float )
		{
		}

#undef src
#undef dst
	};
	//!\~english Specialisation for PixelFormat::A8L8	\~french Spécialisation pour PixelFormat::A8L8
	template<> struct component< PixelFormat::A8L8 >
	{
		static inline uint8_t L8( uint8_t const * p_pSrc )
		{
			return p_pSrc[0];
		}
		static inline uint8_t R8( uint8_t const * p_pSrc )
		{
			return p_pSrc[0];
		}
		static inline uint8_t G8( uint8_t const * p_pSrc )
		{
			return p_pSrc[0];
		}
		static inline uint8_t B8( uint8_t const * p_pSrc )
		{
			return p_pSrc[0];
		}
		static inline uint8_t A8( uint8_t const * p_pSrc )
		{
			return p_pSrc[1];
		}
		static inline float L32F( uint8_t const * p_pSrc )
		{
			return L8( p_pSrc ) / 255.0f;
		}
		static inline float R32F( uint8_t const * p_pSrc )
		{
			return R8( p_pSrc ) / 255.0f;
		}
		static inline float G32F( uint8_t const * p_pSrc )
		{
			return G8( p_pSrc ) / 255.0f;
		}
		static inline float B32F( uint8_t const * p_pSrc )
		{
			return B8( p_pSrc ) / 255.0f;
		}
		static inline float A32F( uint8_t const * p_pSrc )
		{
			return A8( p_pSrc ) / 255.0f;
		}
		static inline void L8( uint8_t * p_pSrc, uint8_t p_val )
		{
			p_pSrc[0] = p_val;
		}
		static inline void R8( uint8_t * p_pSrc, uint8_t p_val )
		{
			p_pSrc[0] = p_val;
		}
		static inline void G8( uint8_t * p_pSrc, uint8_t p_val )
		{
			p_pSrc[0] = p_val;
		}
		static inline void B8( uint8_t * p_pSrc, uint8_t p_val )
		{
			p_pSrc[0] = p_val;
		}
		static inline void A8( uint8_t * p_pSrc, uint8_t p_val )
		{
			p_pSrc[1] = p_val;
		}
		static inline void L32F( uint8_t * p_pSrc, float p_val )
		{
			L8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}
		static inline void R32F( uint8_t * p_pSrc, float p_val )
		{
			R8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}
		static inline void G32F( uint8_t * p_pSrc, float p_val )
		{
			G8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}
		static inline void B32F( uint8_t * p_pSrc, float p_val )
		{
			B8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}
		static inline void A32F( uint8_t * p_pSrc, float p_val )
		{
			A8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}
	};
	//!\~english Specialisation for PixelFormat::AL16F32F	\~french Spécialisation pour PixelFormat::AL16F32F
	template<> struct component< PixelFormat::AL16F32F >
	{
#define src reinterpret_cast< float const * >( p_pSrc )
#define dst reinterpret_cast< float * >( p_pSrc )

		static inline uint8_t L8( uint8_t const * p_pSrc )
		{
			return uint8_t( L32F( p_pSrc ) * 255 );
		}
		static inline uint8_t R8( uint8_t const * p_pSrc )
		{
			return uint8_t( R32F( p_pSrc ) * 255 );
		}
		static inline uint8_t G8( uint8_t const * p_pSrc )
		{
			return uint8_t( G32F( p_pSrc ) * 255 );
		}
		static inline uint8_t B8( uint8_t const * p_pSrc )
		{
			return uint8_t( B32F( p_pSrc ) * 255 );
		}
		static inline uint8_t A8( uint8_t const * p_pSrc )
		{
			return uint8_t( A32F( p_pSrc ) * 255 );
		}
		static inline float L32F( uint8_t const * p_pSrc )
		{
			return src[0];
		}
		static inline float R32F( uint8_t const * p_pSrc )
		{
			return src[0];
		}
		static inline float G32F( uint8_t const * p_pSrc )
		{
			return src[0];
		}
		static inline float B32F( uint8_t const * p_pSrc )
		{
			return src[0];
		}
		static inline float A32F( uint8_t const * p_pSrc )
		{
			return src[1];
		}
		static inline void L8( uint8_t * p_pSrc, uint8_t p_val )
		{
			L32F( p_pSrc, float( p_val ) / 255 );
		}
		static inline void R8( uint8_t * p_pSrc, uint8_t p_val )
		{
			R32F( p_pSrc, float( p_val ) / 255 );
		}
		static inline void G8( uint8_t * p_pSrc, uint8_t p_val )
		{
			G32F( p_pSrc, float( p_val ) / 255 );
		}
		static inline void B8( uint8_t * p_pSrc, uint8_t p_val )
		{
			B32F( p_pSrc, float( p_val ) / 255 );
		}
		static inline void A8( uint8_t * p_pSrc, uint8_t p_val )
		{
			A32F( p_pSrc, float( p_val ) / 255 );
		}
		static inline void L32F( uint8_t * p_pSrc, float p_val )
		{
			dst[0] = p_val;
		}
		static inline void R32F( uint8_t * p_pSrc, float p_val )
		{
			dst[0] = p_val;
		}
		static inline void G32F( uint8_t * p_pSrc, float p_val )
		{
			dst[0] = p_val;
		}
		static inline void B32F( uint8_t * p_pSrc, float p_val )
		{
			dst[0] = p_val;
		}
		static inline void A32F( uint8_t * p_pSrc, float p_val )
		{
			dst[1] = p_val;
		}

#undef src
#undef dst
	};
	//!\~english Specialisation for PixelFormat::AL32F	\~french Spécialisation pour PixelFormat::AL32F
	template<> struct component< PixelFormat::AL32F >
	{
#define src reinterpret_cast< float const * >( p_pSrc )
#define dst reinterpret_cast< float * >( p_pSrc )

		static inline uint8_t L8( uint8_t const * p_pSrc )
		{
			return uint8_t( L32F( p_pSrc ) * 255 );
		}
		static inline uint8_t R8( uint8_t const * p_pSrc )
		{
			return uint8_t( R32F( p_pSrc ) * 255 );
		}
		static inline uint8_t G8( uint8_t const * p_pSrc )
		{
			return uint8_t( G32F( p_pSrc ) * 255 );
		}
		static inline uint8_t B8( uint8_t const * p_pSrc )
		{
			return uint8_t( B32F( p_pSrc ) * 255 );
		}
		static inline uint8_t A8( uint8_t const * p_pSrc )
		{
			return uint8_t( A32F( p_pSrc ) * 255 );
		}
		static inline float L32F( uint8_t const * p_pSrc )
		{
			return src[0];
		}
		static inline float R32F( uint8_t const * p_pSrc )
		{
			return src[0];
		}
		static inline float G32F( uint8_t const * p_pSrc )
		{
			return src[0];
		}
		static inline float B32F( uint8_t const * p_pSrc )
		{
			return src[0];
		}
		static inline float A32F( uint8_t const * p_pSrc )
		{
			return src[1];
		}
		static inline void L8( uint8_t * p_pSrc, uint8_t p_val )
		{
			L32F( p_pSrc, float( p_val ) / 255 );
		}
		static inline void R8( uint8_t * p_pSrc, uint8_t p_val )
		{
			R32F( p_pSrc, float( p_val ) / 255 );
		}
		static inline void G8( uint8_t * p_pSrc, uint8_t p_val )
		{
			G32F( p_pSrc, float( p_val ) / 255 );
		}
		static inline void B8( uint8_t * p_pSrc, uint8_t p_val )
		{
			B32F( p_pSrc, float( p_val ) / 255 );
		}
		static inline void A8( uint8_t * p_pSrc, uint8_t p_val )
		{
			A32F( p_pSrc, float( p_val ) / 255 );
		}
		static inline void L32F( uint8_t * p_pSrc, float p_val )
		{
			dst[0] = p_val;
		}
		static inline void R32F( uint8_t * p_pSrc, float p_val )
		{
			dst[0] = p_val;
		}
		static inline void G32F( uint8_t * p_pSrc, float p_val )
		{
			dst[0] = p_val;
		}
		static inline void B32F( uint8_t * p_pSrc, float p_val )
		{
			dst[0] = p_val;
		}
		static inline void A32F( uint8_t * p_pSrc, float p_val )
		{
			dst[1] = p_val;
		}

#undef src
#undef dst
	};
	//!\~english Specialisation for PixelFormat::A1R5G5B5	\~french Spécialisation pour PixelFormat::A1R5G5B5
	template<> struct component< PixelFormat::A1R5G5B5 >
	{
#define src ( *reinterpret_cast< uint16_t const * >( p_pSrc ) )
#define dst ( *reinterpret_cast< uint16_t * >( p_pSrc ) )

		static inline uint8_t L8( uint8_t const * p_pSrc )
		{
			return uint8_t( R8( p_pSrc ) * 0.30 + G8( p_pSrc ) * 0.59 + B8( p_pSrc ) * 0.11 );
		}
		static inline uint8_t A8( uint8_t const * p_pSrc )
		{
			return uint8_t( ( src & 0x8000 ) ? 0xFF : 0x00 );
		}
		static inline uint8_t R8( uint8_t const * p_pSrc )
		{
			return uint8_t( ( src & 0x7C00 ) >> 7 );
		}
		static inline uint8_t G8( uint8_t const * p_pSrc )
		{
			return uint8_t( ( src & 0x03E0 ) >> 2 );
		}
		static inline uint8_t B8( uint8_t const * p_pSrc )
		{
			return uint8_t( ( src & 0x001F ) << 3 );
		}
		static inline float L32F( uint8_t const * p_pSrc )
		{
			return L8( p_pSrc ) / 255.0f;
		}
		static inline float R32F( uint8_t const * p_pSrc )
		{
			return R8( p_pSrc ) / 255.0f;
		}
		static inline float G32F( uint8_t const * p_pSrc )
		{
			return G8( p_pSrc ) / 255.0f;
		}
		static inline float B32F( uint8_t const * p_pSrc )
		{
			return B8( p_pSrc ) / 255.0f;
		}
		static inline float A32F( uint8_t const * p_pSrc )
		{
			return A8( p_pSrc ) / 255.0f;
		}
		static inline void L8( uint8_t * p_pSrc, uint8_t p_val )
		{
			R8( p_pSrc, p_val );
			G8( p_pSrc, p_val );
			B8( p_pSrc, p_val );
		}
		static inline void A8( uint8_t * p_pSrc, uint8_t p_val )
		{
			dst = ( src & 0x7FFF ) | ( p_val ? 0x8000 : 0x0000 );
		}
		static inline void R8( uint8_t * p_pSrc, uint8_t p_val )
		{
			dst = ( src & 0x83FF ) | ( ( uint16_t( p_val ) << 7 ) & 0x7C00 );
		}
		static inline void G8( uint8_t * p_pSrc, uint8_t p_val )
		{
			dst = ( src & 0xFC1F ) | ( ( uint16_t( p_val ) << 2 ) & 0x03E0 );
		}
		static inline void B8( uint8_t * p_pSrc, uint8_t p_val )
		{
			dst = ( src & 0xFFE0 ) | ( ( uint16_t( p_val ) >> 3 ) & 0x001F );
		}
		static inline void L32F( uint8_t * p_pSrc, float p_val )
		{
			L8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}
		static inline void R32F( uint8_t * p_pSrc, float p_val )
		{
			R8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}
		static inline void G32F( uint8_t * p_pSrc, float p_val )
		{
			G8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}
		static inline void B32F( uint8_t * p_pSrc, float p_val )
		{
			B8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}
		static inline void A32F( uint8_t * p_pSrc, float p_val )
		{
			A8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}

#undef src
#undef dst
	};
	//!\~english Specialisation for PixelFormat::A4R4G4B4	\~french Spécialisation pour PixelFormat::A4R4G4B4
	template<> struct component< PixelFormat::A4R4G4B4 >
	{
#define src ( *reinterpret_cast< uint16_t const * >( p_pSrc ) )
#define dst ( *reinterpret_cast< uint16_t * >( p_pSrc ) )

		static inline uint8_t L8( uint8_t const * p_pSrc )
		{
			return uint8_t( R8( p_pSrc ) * 0.30 + G8( p_pSrc ) * 0.59 + B8( p_pSrc ) * 0.11 );
		}
		static inline uint8_t A8( uint8_t const * p_pSrc )
		{
			return uint8_t( ( src >> 8 ) & 0x00F0 );
		}
		static inline uint8_t R8( uint8_t const * p_pSrc )
		{
			return uint8_t( ( src >> 4 ) & 0x00F0 );
		}
		static inline uint8_t G8( uint8_t const * p_pSrc )
		{
			return uint8_t( ( src >> 0 ) & 0x00F0 );
		}
		static inline uint8_t B8( uint8_t const * p_pSrc )
		{
			return uint8_t( ( src << 4 ) & 0x00F0 );
		}
		static inline float L32F( uint8_t const * p_pSrc )
		{
			return L8( p_pSrc ) / 255.0f;
		}
		static inline float R32F( uint8_t const * p_pSrc )
		{
			return R8( p_pSrc ) / 255.0f;
		}
		static inline float G32F( uint8_t const * p_pSrc )
		{
			return G8( p_pSrc ) / 255.0f;
		}
		static inline float B32F( uint8_t const * p_pSrc )
		{
			return B8( p_pSrc ) / 255.0f;
		}
		static inline float A32F( uint8_t const * p_pSrc )
		{
			return A8( p_pSrc ) / 255.0f;
		}
		static inline void L8( uint8_t * p_pSrc, uint8_t p_val )
		{
			R8( p_pSrc, p_val );
			G8( p_pSrc, p_val );
			B8( p_pSrc, p_val );
		}
		static inline void A8( uint8_t * p_pSrc, uint8_t p_val )
		{
			dst = ( dst & 0x0FFF ) | ( ( uint16_t( p_val ) << 8 ) & 0xF000 );
		}
		static inline void R8( uint8_t * p_pSrc, uint8_t p_val )
		{
			dst = ( dst & 0xF0FF ) | ( ( uint16_t( p_val ) << 4 ) & 0x0F00 );
		}
		static inline void G8( uint8_t * p_pSrc, uint8_t p_val )
		{
			dst = ( dst & 0xFF0F ) | ( ( uint16_t( p_val ) << 0 ) & 0x00F0 );
		}
		static inline void B8( uint8_t * p_pSrc, uint8_t p_val )
		{
			dst = ( dst & 0xFFF0 ) | ( ( uint16_t( p_val ) >> 4 ) & 0x000F );
		}
		static inline void L32F( uint8_t * p_pSrc, float p_val )
		{
			L8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}
		static inline void R32F( uint8_t * p_pSrc, float p_val )
		{
			R8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}
		static inline void G32F( uint8_t * p_pSrc, float p_val )
		{
			G8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}
		static inline void B32F( uint8_t * p_pSrc, float p_val )
		{
			B8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}
		static inline void A32F( uint8_t * p_pSrc, float p_val )
		{
			A8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}

#undef src
#undef dst
	};
	//!\~english Specialisation for PixelFormat::R5G6B5	\~french Spécialisation pour PixelFormat::R5G6B5
	template<> struct component< PixelFormat::R5G6B5 >
	{
#define src	( *reinterpret_cast< uint16_t const * >( p_pSrc ) )
#define dst	( *reinterpret_cast< uint16_t * >( p_pSrc ) )

		static inline uint8_t L8( uint8_t const * p_pSrc )
		{
			return uint8_t( R8( p_pSrc ) * 0.30 + G8( p_pSrc ) * 0.59 + B8( p_pSrc ) * 0.11 );
		}
		static inline uint8_t R8( uint8_t const * p_pSrc )
		{
			return uint8_t( ( src & 0xF800 ) >> 8 );
		}
		static inline uint8_t G8( uint8_t const * p_pSrc )
		{
			return uint8_t( ( src & 0x07E0 ) >> 3 );
		}
		static inline uint8_t B8( uint8_t const * p_pSrc )
		{
			return uint8_t( ( src & 0x001F ) << 3 );
		}
		static inline uint8_t A8( uint8_t const * )
		{
			return 0xFF;
		}
		static inline float L32F( uint8_t const * p_pSrc )
		{
			return L8( p_pSrc ) / 255.0f;
		}
		static inline float R32F( uint8_t const * p_pSrc )
		{
			return R8( p_pSrc ) / 255.0f;
		}
		static inline float G32F( uint8_t const * p_pSrc )
		{
			return G8( p_pSrc ) / 255.0f;
		}
		static inline float B32F( uint8_t const * p_pSrc )
		{
			return B8( p_pSrc ) / 255.0f;
		}
		static inline float A32F( uint8_t const * p_pSrc )
		{
			return A8( p_pSrc ) / 255.0f;
		}
		static inline void L8( uint8_t * p_pSrc, uint8_t p_val )
		{
			R8( p_pSrc, p_val );
			G8( p_pSrc, p_val );
			B8( p_pSrc, p_val );
		}
		static inline void R8( uint8_t * p_pSrc, uint8_t p_val )
		{
			dst = ( src & 0x07FF ) | ( ( uint16_t( p_val ) << 8 ) & 0xF800 );
		}
		static inline void G8( uint8_t * p_pSrc, uint8_t p_val )
		{
			dst = ( src & 0xF81F ) | ( ( uint16_t( p_val ) << 3 ) & 0x07E0 );
		}
		static inline void B8( uint8_t * p_pSrc, uint8_t p_val )
		{
			dst = ( src & 0xFFE0 ) | ( ( uint16_t( p_val ) >> 3 ) & 0x001F );
		}
		static inline void A8( uint8_t *, uint8_t )
		{
		}
		static inline void L32F( uint8_t * p_pSrc, float p_val )
		{
			L8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}
		static inline void R32F( uint8_t * p_pSrc, float p_val )
		{
			R8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}
		static inline void G32F( uint8_t * p_pSrc, float p_val )
		{
			G8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}
		static inline void B32F( uint8_t * p_pSrc, float p_val )
		{
			B8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}
		static inline void A32F( uint8_t *, float )
		{
		}

#undef src
#undef dst
	};
	//!\~english Specialisation for PixelFormat::R8G8B8	\~french Spécialisation pour PixelFormat::R8G8B8
	template<> struct component< PixelFormat::R8G8B8 >
	{
#define src	( *reinterpret_cast< uint32_t const * >( p_pSrc ) )
#define dst	( *reinterpret_cast< uint32_t * >( p_pSrc ) )

		static inline uint8_t L8( uint8_t const * p_pSrc )
		{
			return uint8_t( R8( p_pSrc ) * 0.30 + G8( p_pSrc ) * 0.59 + B8( p_pSrc ) * 0.11 );
		}
		static inline uint8_t R8( uint8_t const * p_pSrc )
		{
			return ( ( src & 0x00FF0000 ) >> 16 );
		}
		static inline uint8_t G8( uint8_t const * p_pSrc )
		{
			return ( ( src & 0x0000FF00 ) >> 8 );
		}
		static inline uint8_t B8( uint8_t const * p_pSrc )
		{
			return ( ( src & 0x000000FF ) >> 0 );
		}
		static inline uint8_t A8( uint8_t const * )
		{
			return 0xFF;
		}
		static inline float L32F( uint8_t const * p_pSrc )
		{
			return L8( p_pSrc ) / 255.0f;
		}
		static inline float R32F( uint8_t const * p_pSrc )
		{
			return R8( p_pSrc ) / 255.0f;
		}
		static inline float G32F( uint8_t const * p_pSrc )
		{
			return G8( p_pSrc ) / 255.0f;
		}
		static inline float B32F( uint8_t const * p_pSrc )
		{
			return B8( p_pSrc ) / 255.0f;
		}
		static inline float A32F( uint8_t const * p_pSrc )
		{
			return A8( p_pSrc ) / 255.0f;
		}
		static inline void L8( uint8_t * p_pSrc, uint8_t p_val )
		{
			R8( p_pSrc, p_val );
			G8( p_pSrc, p_val );
			B8( p_pSrc, p_val );
		}
		static inline void R8( uint8_t * p_pSrc, uint8_t p_val )
		{
			dst = ( src & 0xFF00FFFF ) | ( ( uint32_t( p_val ) << 16 ) & 0x00FF0000 );
		}
		static inline void G8( uint8_t * p_pSrc, uint8_t p_val )
		{
			dst = ( src & 0xFFFF00FF ) | ( ( uint32_t( p_val ) << 8 ) & 0x0000FF00 );
		}
		static inline void B8( uint8_t * p_pSrc, uint8_t p_val )
		{
			dst = ( src & 0xFFFFFF00 ) | ( ( uint32_t( p_val ) << 0 ) & 0x000000FF );
		}
		static inline void A8( uint8_t *, uint8_t )
		{
		}
		static inline void L32F( uint8_t * p_pSrc, float p_val )
		{
			L8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}
		static inline void R32F( uint8_t * p_pSrc, float p_val )
		{
			R8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}
		static inline void G32F( uint8_t * p_pSrc, float p_val )
		{
			G8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}
		static inline void B32F( uint8_t * p_pSrc, float p_val )
		{
			B8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}
		static inline void A32F( uint8_t *, float )
		{
		}

#undef src
#undef dst
	};
	//!\~english Specialisation for PixelFormat::B8G8R8	\~french Spécialisation pour PixelFormat::B8G8R8
	template<> struct component< PixelFormat::B8G8R8 >
	{
#define src	( *reinterpret_cast< uint32_t const * >( p_pSrc ) )
#define dst	( *reinterpret_cast< uint32_t * >( p_pSrc ) )

		static inline uint8_t L8( uint8_t const * p_pSrc )
		{
			return uint8_t( R8( p_pSrc ) * 0.30 + G8( p_pSrc ) * 0.59 + B8( p_pSrc ) * 0.11 );
		}
		static inline uint8_t R8( uint8_t const * p_pSrc )
		{
			return ( ( src & 0x000000FF ) >> 0 );
		}
		static inline uint8_t G8( uint8_t const * p_pSrc )
		{
			return ( ( src & 0x0000FF00 ) >> 8 );
		}
		static inline uint8_t B8( uint8_t const * p_pSrc )
		{
			return ( ( src & 0x00FF0000 ) >> 16 );
		}
		static inline uint8_t A8( uint8_t const * )
		{
			return 0xFF;
		}
		static inline float L32F( uint8_t const * p_pSrc )
		{
			return L8( p_pSrc ) / 255.0f;
		}
		static inline float R32F( uint8_t const * p_pSrc )
		{
			return R8( p_pSrc ) / 255.0f;
		}
		static inline float G32F( uint8_t const * p_pSrc )
		{
			return G8( p_pSrc ) / 255.0f;
		}
		static inline float B32F( uint8_t const * p_pSrc )
		{
			return B8( p_pSrc ) / 255.0f;
		}
		static inline float A32F( uint8_t const * p_pSrc )
		{
			return A8( p_pSrc ) / 255.0f;
		}
		static inline void L8( uint8_t * p_pSrc, uint8_t p_val )
		{
			R8( p_pSrc, p_val );
			G8( p_pSrc, p_val );
			B8( p_pSrc, p_val );
		}
		static inline void R8( uint8_t * p_pSrc, uint8_t p_val )
		{
			dst = ( src & 0xFFFFFF00 ) | ( ( uint32_t( p_val ) << 0 ) & 0x000000FF );
		}
		static inline void G8( uint8_t * p_pSrc, uint8_t p_val )
		{
			dst = ( src & 0xFFFF00FF ) | ( ( uint32_t( p_val ) << 8 ) & 0x0000FF00 );
		}
		static inline void B8( uint8_t * p_pSrc, uint8_t p_val )
		{
			dst = ( src & 0xFF00FFFF ) | ( ( uint32_t( p_val ) << 16 ) & 0x00FF0000 );
		}
		static inline void A8( uint8_t *, uint8_t )
		{
		}
		static inline void L32F( uint8_t * p_pSrc, float p_val )
		{
			L8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}
		static inline void R32F( uint8_t * p_pSrc, float p_val )
		{
			R8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}
		static inline void G32F( uint8_t * p_pSrc, float p_val )
		{
			G8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}
		static inline void B32F( uint8_t * p_pSrc, float p_val )
		{
			B8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}
		static inline void A32F( uint8_t *, float )
		{
		}

#undef src
#undef dst
	};
	//!\~english Specialisation for PixelFormat::A8R8G8B8	\~french Spécialisation pour PixelFormat::A8R8G8B8
	template<> struct component< PixelFormat::A8R8G8B8 >
	{
#define src	( *reinterpret_cast< uint32_t const * >( p_pSrc ) )
#define dst	( *reinterpret_cast< uint32_t * >( p_pSrc ) )

		static inline uint8_t L8( uint8_t const * p_pSrc )
		{
			return uint8_t( R8( p_pSrc ) * 0.30 + G8( p_pSrc ) * 0.59 + B8( p_pSrc ) * 0.11 );
		}
		static inline uint8_t A8( uint8_t const * p_pSrc )
		{
			return ( ( src & 0xFF000000 ) >> 24 );
		}
		static inline uint8_t R8( uint8_t const * p_pSrc )
		{
			return ( ( src & 0x00FF0000 ) >> 16 );
		}
		static inline uint8_t G8( uint8_t const * p_pSrc )
		{
			return ( ( src & 0x0000FF00 ) >> 8 );
		}
		static inline uint8_t B8( uint8_t const * p_pSrc )
		{
			return ( ( src & 0x000000FF ) >> 0 );
		}
		static inline float L32F( uint8_t const * p_pSrc )
		{
			return L8( p_pSrc ) / 255.0f;
		}
		static inline float R32F( uint8_t const * p_pSrc )
		{
			return R8( p_pSrc ) / 255.0f;
		}
		static inline float G32F( uint8_t const * p_pSrc )
		{
			return G8( p_pSrc ) / 255.0f;
		}
		static inline float B32F( uint8_t const * p_pSrc )
		{
			return B8( p_pSrc ) / 255.0f;
		}
		static inline float A32F( uint8_t const * p_pSrc )
		{
			return A8( p_pSrc ) / 255.0f;
		}
		static inline void L8( uint8_t * p_pSrc, uint8_t p_val )
		{
			R8( p_pSrc, p_val );
			G8( p_pSrc, p_val );
			B8( p_pSrc, p_val );
		}
		static inline void A8( uint8_t * p_pSrc, uint8_t p_val )
		{
			dst = ( src & 0x00FFFFFF ) | ( ( uint32_t( p_val ) << 24 ) & 0xFF000000 );
		}
		static inline void R8( uint8_t * p_pSrc, uint8_t p_val )
		{
			dst = ( src & 0xFF00FFFF ) | ( ( uint32_t( p_val ) << 16 ) & 0x00FF0000 );
		}
		static inline void G8( uint8_t * p_pSrc, uint8_t p_val )
		{
			dst = ( src & 0xFFFF00FF ) | ( ( uint32_t( p_val ) << 8 ) & 0x0000FF00 );
		}
		static inline void B8( uint8_t * p_pSrc, uint8_t p_val )
		{
			dst = ( src & 0xFFFFFF00 ) | ( ( uint32_t( p_val ) << 0 ) & 0x000000FF );
		}
		static inline void L32F( uint8_t * p_pSrc, float p_val )
		{
			L8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}
		static inline void R32F( uint8_t * p_pSrc, float p_val )
		{
			R8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}
		static inline void G32F( uint8_t * p_pSrc, float p_val )
		{
			G8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}
		static inline void B32F( uint8_t * p_pSrc, float p_val )
		{
			B8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}
		static inline void A32F( uint8_t * p_pSrc, float p_val )
		{
			A8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}

#undef src
#undef dst
	};
	//!\~english Specialisation for PixelFormat::A8B8G8R8	\~french Spécialisation pour PixelFormat::A8B8G8R8
	template<> struct component< PixelFormat::A8B8G8R8 >
	{
#define src	( *reinterpret_cast< uint32_t const * >( p_pSrc ) )
#define dst	( *reinterpret_cast< uint32_t * >( p_pSrc ) )

		static inline uint8_t L8( uint8_t const * p_pSrc )
		{
			return uint8_t( R8( p_pSrc ) * 0.30 + G8( p_pSrc ) * 0.59 + B8( p_pSrc ) * 0.11 );
		}
		static inline uint8_t A8( uint8_t const * p_pSrc )
		{
			return ( ( src & 0xFF000000 ) >> 24 );
		}
		static inline uint8_t R8( uint8_t const * p_pSrc )
		{
			return ( ( src & 0x000000FF ) >> 0 );
		}
		static inline uint8_t G8( uint8_t const * p_pSrc )
		{
			return ( ( src & 0x0000FF00 ) >> 8 );
		}
		static inline uint8_t B8( uint8_t const * p_pSrc )
		{
			return ( ( src & 0x00FF0000 ) >> 16 );
		}
		static inline float L32F( uint8_t const * p_pSrc )
		{
			return L8( p_pSrc ) / 255.0f;
		}
		static inline float R32F( uint8_t const * p_pSrc )
		{
			return R8( p_pSrc ) / 255.0f;
		}
		static inline float G32F( uint8_t const * p_pSrc )
		{
			return G8( p_pSrc ) / 255.0f;
		}
		static inline float B32F( uint8_t const * p_pSrc )
		{
			return B8( p_pSrc ) / 255.0f;
		}
		static inline float A32F( uint8_t const * p_pSrc )
		{
			return A8( p_pSrc ) / 255.0f;
		}
		static inline void L8( uint8_t * p_pSrc, uint8_t p_val )
		{
			R8( p_pSrc, p_val );
			G8( p_pSrc, p_val );
			B8( p_pSrc, p_val );
		}
		static inline void A8( uint8_t * p_pSrc, uint8_t p_val )
		{
			dst = ( src & 0x00FFFFFF ) | ( ( uint32_t( p_val ) << 24 ) & 0xFF000000 );
		}
		static inline void R8( uint8_t * p_pSrc, uint8_t p_val )
		{
			dst = ( src & 0xFFFFFF00 ) | ( ( uint32_t( p_val ) << 0 ) & 0x000000FF );
		}
		static inline void G8( uint8_t * p_pSrc, uint8_t p_val )
		{
			dst = ( src & 0xFFFF00FF ) | ( ( uint32_t( p_val ) << 8 ) & 0x0000FF00 );
		}
		static inline void B8( uint8_t * p_pSrc, uint8_t p_val )
		{
			dst = ( src & 0xFF00FFFF ) | ( ( uint32_t( p_val ) << 16 ) & 0x00FF0000 );
		}
		static inline void L32F( uint8_t * p_pSrc, float p_val )
		{
			L8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}
		static inline void R32F( uint8_t * p_pSrc, float p_val )
		{
			R8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}
		static inline void G32F( uint8_t * p_pSrc, float p_val )
		{
			G8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}
		static inline void B32F( uint8_t * p_pSrc, float p_val )
		{
			B8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}
		static inline void A32F( uint8_t * p_pSrc, float p_val )
		{
			A8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}

#undef src
#undef dst
	};
	//!\~english Specialisation for PixelFormat::RGB16F	\~french Spécialisation pour PixelFormat::RGB16F
	template<> struct component< PixelFormat::RGB16F >
	{
		static inline uint8_t L8( uint8_t const * p_pSrc )
		{
			return uint8_t( L32F( p_pSrc ) * 255 );
		}
		static inline uint8_t R8( uint8_t const * p_pSrc )
		{
			return uint8_t( R32F( p_pSrc ) * 255 );
		}
		static inline uint8_t G8( uint8_t const * p_pSrc )
		{
			return uint8_t( G32F( p_pSrc ) * 255 );
		}
		static inline uint8_t B8( uint8_t const * p_pSrc )
		{
			return uint8_t( B32F( p_pSrc ) * 255 );
		}
		static inline uint8_t A8( uint8_t const * p_pSrc )
		{
			return uint8_t( A32F( p_pSrc ) * 255 );
		}
		static inline float L32F( uint8_t const * p_pSrc )
		{
			return float( R32F( p_pSrc ) * 0.30 + G32F( p_pSrc ) * 0.59 + B32F( p_pSrc ) * 0.11 );
		}
		static inline float R32F( uint8_t const * p_pSrc )
		{
			float l_return{};
			HalfToFloat( l_return, reinterpret_cast< uint16_t const * >( p_pSrc ) + 0 );
			return l_return;
		}
		static inline float G32F( uint8_t const * p_pSrc )
		{
			float l_return{};
			HalfToFloat( l_return, reinterpret_cast< uint16_t const * >( p_pSrc ) + 1 );
			return l_return;
		}
		static inline float B32F( uint8_t const * p_pSrc )
		{
			float l_return{};
			HalfToFloat( l_return, reinterpret_cast< uint16_t const * >( p_pSrc ) + 2 );
			return l_return;
		}
		static inline float A32F( uint8_t const * )
		{
			return 1.0f;
		}
		static inline void L8( uint8_t * p_pSrc, uint8_t p_val )
		{
			L32F( p_pSrc, float( p_val ) / 255 );
		}
		static inline void R8( uint8_t * p_pSrc, uint8_t p_val )
		{
			R32F( p_pSrc, float( p_val ) / 255 );
		}
		static inline void G8( uint8_t * p_pSrc, uint8_t p_val )
		{
			G32F( p_pSrc, float( p_val ) / 255 );
		}
		static inline void B8( uint8_t * p_pSrc, uint8_t p_val )
		{
			B32F( p_pSrc, float( p_val ) / 255 );
		}
		static inline void A8( uint8_t *, uint8_t )
		{
		}
		static inline void L32F( uint8_t * p_pSrc, float p_val )
		{
			R32F( p_pSrc, p_val );
			G32F( p_pSrc, p_val );
			B32F( p_pSrc, p_val );
		}
		static inline void R32F( uint8_t * p_pSrc, float p_val )
		{
			FloatToHalf( reinterpret_cast< uint16_t * >( p_pSrc ) + 0, p_val );
		}
		static inline void G32F( uint8_t * p_pSrc, float p_val )
		{
			FloatToHalf( reinterpret_cast< uint16_t * >( p_pSrc ) + 1, p_val );
		}
		static inline void B32F( uint8_t * p_pSrc, float p_val )
		{
			FloatToHalf( reinterpret_cast< uint16_t * >( p_pSrc ) + 2, p_val );
		}
		static inline void A32F( uint8_t *, float )
		{
		}
	};
	//!\~english Specialisation for PixelFormat::RGB16F32F	\~french Spécialisation pour PixelFormat::RGB16F32F
	template<> struct component< PixelFormat::RGB16F32F >
	{
		static inline uint8_t L8( uint8_t const * p_pSrc )
		{
			return uint8_t( L32F( p_pSrc ) * 255 );
		}
		static inline uint8_t R8( uint8_t const * p_pSrc )
		{
			return uint8_t( R32F( p_pSrc ) * 255 );
		}
		static inline uint8_t G8( uint8_t const * p_pSrc )
		{
			return uint8_t( G32F( p_pSrc ) * 255 );
		}
		static inline uint8_t B8( uint8_t const * p_pSrc )
		{
			return uint8_t( B32F( p_pSrc ) * 255 );
		}
		static inline uint8_t A8( uint8_t const * p_pSrc )
		{
			return uint8_t( A32F( p_pSrc ) * 255 );
		}
		static inline float L32F( uint8_t const * p_pSrc )
		{
			return float( R32F( p_pSrc ) * 0.30 + G32F( p_pSrc ) * 0.59 + B32F( p_pSrc ) * 0.11 );
		}
		static inline float R32F( uint8_t const * p_pSrc )
		{
			return reinterpret_cast< float const * >( p_pSrc )[0];
		}
		static inline float G32F( uint8_t const * p_pSrc )
		{
			return reinterpret_cast< float const * >( p_pSrc )[1];
		}
		static inline float B32F( uint8_t const * p_pSrc )
		{
			return reinterpret_cast< float const * >( p_pSrc )[2];
		}
		static inline float A32F( uint8_t const * )
		{
			return 1.0f;
		}
		static inline void L8( uint8_t * p_pSrc, uint8_t p_val )
		{
			L32F( p_pSrc, float( p_val ) / 255 );
		}
		static inline void R8( uint8_t * p_pSrc, uint8_t p_val )
		{
			R32F( p_pSrc, float( p_val ) / 255 );
		}
		static inline void G8( uint8_t * p_pSrc, uint8_t p_val )
		{
			G32F( p_pSrc, float( p_val ) / 255 );
		}
		static inline void B8( uint8_t * p_pSrc, uint8_t p_val )
		{
			B32F( p_pSrc, float( p_val ) / 255 );
		}
		static inline void A8( uint8_t *, uint8_t )
		{
		}
		static inline void L32F( uint8_t * p_pSrc, float p_val )
		{
			R32F( p_pSrc, p_val );
			G32F( p_pSrc, p_val );
			B32F( p_pSrc, p_val );
		}
		static inline void R32F( uint8_t * p_pSrc, float p_val )
		{
			reinterpret_cast< float * >( p_pSrc )[0] = p_val;
		}
		static inline void G32F( uint8_t * p_pSrc, float p_val )
		{
			reinterpret_cast< float * >( p_pSrc )[1] = p_val;
		}
		static inline void B32F( uint8_t * p_pSrc, float p_val )
		{
			reinterpret_cast< float * >( p_pSrc )[2] = p_val;
		}
		static inline void A32F( uint8_t *, float )
		{
		}
	};
	//!\~english Specialisation for PixelFormat::RGB32F	\~french Spécialisation pour PixelFormat::RGB32F
	template<> struct component< PixelFormat::RGB32F >
	{
		static inline uint8_t L8( uint8_t const * p_pSrc )
		{
			return uint8_t( L32F( p_pSrc ) * 255 );
		}
		static inline uint8_t R8( uint8_t const * p_pSrc )
		{
			return uint8_t( R32F( p_pSrc ) * 255 );
		}
		static inline uint8_t G8( uint8_t const * p_pSrc )
		{
			return uint8_t( G32F( p_pSrc ) * 255 );
		}
		static inline uint8_t B8( uint8_t const * p_pSrc )
		{
			return uint8_t( B32F( p_pSrc ) * 255 );
		}
		static inline uint8_t A8( uint8_t const * p_pSrc )
		{
			return uint8_t( A32F( p_pSrc ) * 255 );
		}
		static inline float L32F( uint8_t const * p_pSrc )
		{
			return float( R32F( p_pSrc ) * 0.30 + G32F( p_pSrc ) * 0.59 + B32F( p_pSrc ) * 0.11 );
		}
		static inline float R32F( uint8_t const * p_pSrc )
		{
			return reinterpret_cast< float const * >( p_pSrc )[0];
		}
		static inline float G32F( uint8_t const * p_pSrc )
		{
			return reinterpret_cast< float const * >( p_pSrc )[1];
		}
		static inline float B32F( uint8_t const * p_pSrc )
		{
			return reinterpret_cast< float const * >( p_pSrc )[2];
		}
		static inline float A32F( uint8_t const * )
		{
			return 1.0f;
		}
		static inline void L8( uint8_t * p_pSrc, uint8_t p_val )
		{
			L32F( p_pSrc, float( p_val ) / 255 );
		}
		static inline void R8( uint8_t * p_pSrc, uint8_t p_val )
		{
			R32F( p_pSrc, float( p_val ) / 255 );
		}
		static inline void G8( uint8_t * p_pSrc, uint8_t p_val )
		{
			G32F( p_pSrc, float( p_val ) / 255 );
		}
		static inline void B8( uint8_t * p_pSrc, uint8_t p_val )
		{
			B32F( p_pSrc, float( p_val ) / 255 );
		}
		static inline void A8( uint8_t *, uint8_t )
		{
		}
		static inline void L32F( uint8_t * p_pSrc, float p_val )
		{
			R32F( p_pSrc, p_val );
			G32F( p_pSrc, p_val );
			B32F( p_pSrc, p_val );
		}
		static inline void R32F( uint8_t * p_pSrc, float p_val )
		{
			reinterpret_cast< float * >( p_pSrc )[0] = p_val;
		}
		static inline void G32F( uint8_t * p_pSrc, float p_val )
		{
			reinterpret_cast< float * >( p_pSrc )[1] = p_val;
		}
		static inline void B32F( uint8_t * p_pSrc, float p_val )
		{
			reinterpret_cast< float * >( p_pSrc )[2] = p_val;
		}
		static inline void A32F( uint8_t *, float )
		{
		}
	};
	//!\~english Specialisation for PixelFormat::RGBA16F	\~french Spécialisation pour PixelFormat::RGBA16F
	template<> struct component< PixelFormat::RGBA16F >
	{
		static inline uint8_t L8( uint8_t const * p_pSrc )
		{
			return uint8_t( L32F( p_pSrc ) * 255 );
		}
		static inline uint8_t R8( uint8_t const * p_pSrc )
		{
			return uint8_t( R32F( p_pSrc ) * 255 );
		}
		static inline uint8_t G8( uint8_t const * p_pSrc )
		{
			return uint8_t( G32F( p_pSrc ) * 255 );
		}
		static inline uint8_t B8( uint8_t const * p_pSrc )
		{
			return uint8_t( B32F( p_pSrc ) * 255 );
		}
		static inline uint8_t A8( uint8_t const * p_pSrc )
		{
			return uint8_t( A32F( p_pSrc ) * 255 );
		}
		static inline float L32F( uint8_t const * p_pSrc )
		{
			return float( R32F( p_pSrc ) * 0.30 + G32F( p_pSrc ) * 0.59 + B32F( p_pSrc ) * 0.11 );
		}
		static inline float R32F( uint8_t const * p_pSrc )
		{
			float l_return{};
			HalfToFloat( l_return, reinterpret_cast< uint16_t const * >( p_pSrc ) + 0 );
			return l_return;
		}
		static inline float G32F( uint8_t const * p_pSrc )
		{
			float l_return{};
			HalfToFloat( l_return, reinterpret_cast< uint16_t const * >( p_pSrc ) + 1 );
			return l_return;
		}
		static inline float B32F( uint8_t const * p_pSrc )
		{
			float l_return{};
			HalfToFloat( l_return, reinterpret_cast< uint16_t const * >( p_pSrc ) + 2 );
			return l_return;
		}
		static inline float A32F( uint8_t const * p_pSrc )
		{
			float l_return{};
			HalfToFloat( l_return, reinterpret_cast< uint16_t const * >( p_pSrc ) + 3 );
			return l_return;
		}
		static inline void L8( uint8_t * p_pSrc, uint8_t p_val )
		{
			L32F( p_pSrc, float( p_val ) / 255 );
		}
		static inline void R8( uint8_t * p_pSrc, uint8_t p_val )
		{
			R32F( p_pSrc, float( p_val ) / 255 );
		}
		static inline void G8( uint8_t * p_pSrc, uint8_t p_val )
		{
			G32F( p_pSrc, float( p_val ) / 255 );
		}
		static inline void B8( uint8_t * p_pSrc, uint8_t p_val )
		{
			B32F( p_pSrc, float( p_val ) / 255 );
		}
		static inline void A8( uint8_t * p_pSrc, uint8_t p_val )
		{
			A32F( p_pSrc, float( p_val ) / 255 );
		}
		static inline void L32F( uint8_t * p_pSrc, float p_val )
		{
			R32F( p_pSrc, p_val );
			G32F( p_pSrc, p_val );
			B32F( p_pSrc, p_val );
		}
		static inline void R32F( uint8_t * p_pSrc, float p_val )
		{
			FloatToHalf( reinterpret_cast< uint16_t * >( p_pSrc ) + 0, p_val );
		}
		static inline void G32F( uint8_t * p_pSrc, float p_val )
		{
			FloatToHalf( reinterpret_cast< uint16_t * >( p_pSrc ) + 1, p_val );
		}
		static inline void B32F( uint8_t * p_pSrc, float p_val )
		{
			FloatToHalf( reinterpret_cast< uint16_t * >( p_pSrc ) + 2, p_val );
		}
		static inline void A32F( uint8_t * p_pSrc, float p_val )
		{
			FloatToHalf( reinterpret_cast< uint16_t * >( p_pSrc ) + 3, p_val );
		}
	};
	//!\~english Specialisation for PixelFormat::RGBA16F32F	\~french Spécialisation pour PixelFormat::RGBA16F32F
	template<> struct component< PixelFormat::RGBA16F32F >
	{
		static inline uint8_t L8( uint8_t const * p_pSrc )
		{
			return uint8_t( L32F( p_pSrc ) * 255 );
		}
		static inline uint8_t R8( uint8_t const * p_pSrc )
		{
			return uint8_t( R32F( p_pSrc ) * 255 );
		}
		static inline uint8_t G8( uint8_t const * p_pSrc )
		{
			return uint8_t( G32F( p_pSrc ) * 255 );
		}
		static inline uint8_t B8( uint8_t const * p_pSrc )
		{
			return uint8_t( B32F( p_pSrc ) * 255 );
		}
		static inline uint8_t A8( uint8_t const * p_pSrc )
		{
			return uint8_t( A32F( p_pSrc ) * 255 );
		}
		static inline float L32F( uint8_t const * p_pSrc )
		{
			return float( R32F( p_pSrc ) * 0.30 + G32F( p_pSrc ) * 0.59 + B32F( p_pSrc ) * 0.11 );
		}
		static inline float R32F( uint8_t const * p_pSrc )
		{
			return reinterpret_cast< float const * >( p_pSrc )[0];
		}
		static inline float G32F( uint8_t const * p_pSrc )
		{
			return reinterpret_cast< float const * >( p_pSrc )[1];
		}
		static inline float B32F( uint8_t const * p_pSrc )
		{
			return reinterpret_cast< float const * >( p_pSrc )[2];
		}
		static inline float A32F( uint8_t const * p_pSrc )
		{
			return reinterpret_cast< float const * >( p_pSrc )[3];
		}
		static inline void L8( uint8_t * p_pSrc, uint8_t p_val )
		{
			L32F( p_pSrc, float( p_val ) / 255 );
		}
		static inline void R8( uint8_t * p_pSrc, uint8_t p_val )
		{
			R32F( p_pSrc, float( p_val ) / 255 );
		}
		static inline void G8( uint8_t * p_pSrc, uint8_t p_val )
		{
			G32F( p_pSrc, float( p_val ) / 255 );
		}
		static inline void B8( uint8_t * p_pSrc, uint8_t p_val )
		{
			B32F( p_pSrc, float( p_val ) / 255 );
		}
		static inline void A8( uint8_t * p_pSrc, uint8_t p_val )
		{
			A32F( p_pSrc, float( p_val ) / 255 );
		}
		static inline void L32F( uint8_t * p_pSrc, float p_val )
		{
			R32F( p_pSrc, p_val );
			G32F( p_pSrc, p_val );
			B32F( p_pSrc, p_val );
		}
		static inline void R32F( uint8_t * p_pSrc, float p_val )
		{
			reinterpret_cast< float * >( p_pSrc )[0] = p_val;
		}
		static inline void G32F( uint8_t * p_pSrc, float p_val )
		{
			reinterpret_cast< float * >( p_pSrc )[1] = p_val;
		}
		static inline void B32F( uint8_t * p_pSrc, float p_val )
		{
			reinterpret_cast< float * >( p_pSrc )[2] = p_val;
		}
		static inline void A32F( uint8_t * p_pSrc, float p_val )
		{
			reinterpret_cast< float * >( p_pSrc )[3] = p_val;
		}
	};
	//!\~english Specialisation for PixelFormat::RGBA32F	\~french Spécialisation pour PixelFormat::RGBA32F
	template<> struct component< PixelFormat::RGBA32F >
	{
#define src	( reinterpret_cast< float const * >( p_pSrc ) )
#define dst	( reinterpret_cast< float * >( p_pSrc ) )

		static inline uint8_t L8( uint8_t const * p_pSrc )
		{
			return uint8_t( L32F( p_pSrc ) * 255 );
		}
		static inline uint8_t R8( uint8_t const * p_pSrc )
		{
			return uint8_t( R32F( p_pSrc ) * 255 );
		}
		static inline uint8_t G8( uint8_t const * p_pSrc )
		{
			return uint8_t( G32F( p_pSrc ) * 255 );
		}
		static inline uint8_t B8( uint8_t const * p_pSrc )
		{
			return uint8_t( B32F( p_pSrc ) * 255 );
		}
		static inline uint8_t A8( uint8_t const * p_pSrc )
		{
			return uint8_t( A32F( p_pSrc ) * 255 );
		}
		static inline float L32F( uint8_t const * p_pSrc )
		{
			return float( R32F( p_pSrc ) * 0.30 + G32F( p_pSrc ) * 0.59 + B32F( p_pSrc ) * 0.11 );
		}
		static inline float R32F( uint8_t const * p_pSrc )
		{
			return src[0];
		}
		static inline float G32F( uint8_t const * p_pSrc )
		{
			return src[1];
		}
		static inline float B32F( uint8_t const * p_pSrc )
		{
			return src[2];
		}
		static inline float A32F( uint8_t const * p_pSrc )
		{
			return src[3];
		}
		static inline void L8( uint8_t * p_pSrc, uint8_t p_val )
		{
			L32F( p_pSrc, float( p_val ) / 255 );
		}
		static inline void R8( uint8_t * p_pSrc, uint8_t p_val )
		{
			R32F( p_pSrc, float( p_val ) / 255 );
		}
		static inline void G8( uint8_t * p_pSrc, uint8_t p_val )
		{
			G32F( p_pSrc, float( p_val ) / 255 );
		}
		static inline void B8( uint8_t * p_pSrc, uint8_t p_val )
		{
			B32F( p_pSrc, float( p_val ) / 255 );
		}
		static inline void A8( uint8_t * p_pSrc, uint8_t p_val )
		{
			A32F( p_pSrc, float( p_val ) / 255 );
		}
		static inline void L32F( uint8_t * p_pSrc, float p_val )
		{
			R32F( p_pSrc, p_val );
			G32F( p_pSrc, p_val );
			B32F( p_pSrc, p_val );
		}
		static inline void R32F( uint8_t * p_pSrc, float p_val )
		{
			dst[0] = p_val;
		}
		static inline void G32F( uint8_t * p_pSrc, float p_val )
		{
			dst[1] = p_val;
		}
		static inline void B32F( uint8_t * p_pSrc, float p_val )
		{
			dst[2] = p_val;
		}
		static inline void A32F( uint8_t * p_pSrc, float p_val )
		{
			dst[3] = p_val;
		}

#undef src
#undef dst
	};
	//!\~english Specialisation for PixelFormat::YUY2	\~french Spécialisation pour PixelFormat::YUY2
	template<> struct component< PixelFormat::YUY2 >
	{
#	define YUV_Y(x)		(((x)[0] & 0xF0) & (((x)[1] & 0xF0) >> 2))
#	define YUV_U(x)		((x)[0] & 0x0F)
#	define YUV_V(x)		((x)[1] & 0x0F)
#	define YUV_C(x)		(YUV_Y(x) -  16)
#	define YUV_D(x)		(YUV_U(x) - 128)
#	define YUV_E(x)		(YUV_V(x) - 128)
#	define YUV_clip(x)	static_cast<uint8_t>( (x) > 255 ? 255 : (x) < 0 ? 0 : (x))
		static inline uint8_t L8( uint8_t const * p_pSrc )
		{
			return uint8_t( R8( p_pSrc ) * 0.30 + G8( p_pSrc ) * 0.59 + B8( p_pSrc ) * 0.11 );
		}
		static inline uint8_t R8( uint8_t const * p_pSrc )
		{
			return uint8_t( YUV_clip( ( 298 * YUV_C( p_pSrc ) + 409 * YUV_E( p_pSrc ) + 128 ) >> 8 ) );
		}
		static inline uint8_t G8( uint8_t const * p_pSrc )
		{
			return uint8_t( YUV_clip( ( 298 * YUV_C( p_pSrc ) - 100 * YUV_D( p_pSrc ) - 208 * YUV_E( p_pSrc ) + 128 ) >> 8 ) );
		}
		static inline uint8_t B8( uint8_t const * p_pSrc )
		{
			return uint8_t( YUV_clip( ( 298 * YUV_C( p_pSrc ) + 516 * YUV_D( p_pSrc ) + 128 ) >> 8 ) );
		}
		static inline uint8_t A8( uint8_t const * )
		{
			return 0xFF;
		}
		static inline float L32F( uint8_t const * p_pSrc )
		{
			return L8( p_pSrc ) / 255.0f;
		}
		static inline float R32F( uint8_t const * p_pSrc )
		{
			return R8( p_pSrc ) / 255.0f;
		}
		static inline float G32F( uint8_t const * p_pSrc )
		{
			return G8( p_pSrc ) / 255.0f;
		}
		static inline float B32F( uint8_t const * p_pSrc )
		{
			return B8( p_pSrc ) / 255.0f;
		}
		static inline float A32F( uint8_t const * p_pSrc )
		{
			return A8( p_pSrc ) / 255.0f;
		}
#	undef YUV_Y
#	undef YUV_U
#	undef YUV_V
#	undef YUV_C
#	undef YUV_D
#	undef YUV_E
#	undef YUV_clip
	};
	//!\~english Specialisation for PixelFormat::D16	\~french Spécialisation pour PixelFormat::D16
	template<> struct component< PixelFormat::D16 >
	{
#define src	( *reinterpret_cast< uint16_t const * >( p_pSrc ) )
#define dst	( *reinterpret_cast< uint16_t * >( p_pSrc ) )

		static inline uint8_t D8( uint8_t const * p_pSrc )
		{
			return uint8_t( src >> 8 );
		}
		static inline uint16_t D16( uint8_t const * p_pSrc )
		{
			return src;
		}
		static inline uint32_t D24( uint8_t const * p_pSrc )
		{
			return ( uint32_t( src ) << 8 );
		}
		static inline uint32_t D32( uint8_t const * p_pSrc )
		{
			return ( uint32_t( src ) << 16 );
		}
		static inline uint8_t S1( uint8_t const * )
		{
			return 0;
		}
		static inline uint8_t S8( uint8_t const * )
		{
			return 0;
		}
		static inline float D32F( uint8_t const * p_pSrc )
		{
			return float( D32( p_pSrc ) );
		}
		static inline float S32F( uint8_t const * )
		{
			return 0;
		}
		static inline void D8( uint8_t * p_pSrc, uint8_t p_val )
		{
			dst = ( uint16_t( p_val ) << 8 );
		}
		static inline void D16( uint8_t * p_pSrc, uint16_t p_val )
		{
			dst = p_val;
		}
		static inline void D24( uint8_t * p_pSrc, uint32_t p_val )
		{
			dst = ( src & 0xFFFF0000 ) | ( 0x0000FFFF & ( p_val >> 8 ) );
		}
		static inline void D32( uint8_t * p_pSrc, uint32_t p_val )
		{
			dst = ( src & 0xFFFF0000 ) | ( 0x0000FFFF & ( p_val >> 16 ) );
		}
		static inline void S1( uint8_t *, uint8_t )
		{
		}
		static inline void S8( uint8_t *, uint8_t )
		{
		}
		static inline void D32F( uint8_t * p_pSrc, float p_val )
		{
			D32( p_pSrc, uint32_t( p_val ) );
		}
		static inline void S32F( uint8_t *, float )
		{
		}
		static inline uint8_t L8( uint8_t const * p_pSrc )
		{
			return D8( p_pSrc );
		}
		static inline uint8_t A8( uint8_t const * p_pSrc )
		{
			return 0;
		}
		static inline uint8_t R8( uint8_t const * p_pSrc )
		{
			return L8( p_pSrc );
		}
		static inline uint8_t G8( uint8_t const * p_pSrc )
		{
			return L8( p_pSrc );
		}
		static inline uint8_t B8( uint8_t const * p_pSrc )
		{
			return L8( p_pSrc );
		}
		static inline float L32F( uint8_t const * p_pSrc )
		{
			return L8( p_pSrc ) / 255.0f;
		}
		static inline float R32F( uint8_t const * p_pSrc )
		{
			return R8( p_pSrc ) / 255.0f;
		}
		static inline float G32F( uint8_t const * p_pSrc )
		{
			return G8( p_pSrc ) / 255.0f;
		}
		static inline float B32F( uint8_t const * p_pSrc )
		{
			return B8( p_pSrc ) / 255.0f;
		}
		static inline float A32F( uint8_t const * p_pSrc )
		{
			return A8( p_pSrc ) / 255.0f;
		}
		static inline void L8( uint8_t * p_pSrc, uint8_t p_val )
		{
			D8( p_pSrc, p_val );
		}
		static inline void A8( uint8_t * p_pSrc, uint8_t p_val )
		{
		}
		static inline void R8( uint8_t * p_pSrc, uint8_t p_val )
		{
			L8( p_pSrc, p_val );
		}
		static inline void G8( uint8_t * p_pSrc, uint8_t p_val )
		{
			L8( p_pSrc, p_val );
		}
		static inline void B8( uint8_t * p_pSrc, uint8_t p_val )
		{
			L8( p_pSrc, p_val );
		}
		static inline void L32F( uint8_t * p_pSrc, float p_val )
		{
			L8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}
		static inline void R32F( uint8_t * p_pSrc, float p_val )
		{
			R8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}
		static inline void G32F( uint8_t * p_pSrc, float p_val )
		{
			G8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}
		static inline void B32F( uint8_t * p_pSrc, float p_val )
		{
			B8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}
		static inline void A32F( uint8_t * p_pSrc, float p_val )
		{
			A8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}

#undef src
#undef dst
	};
	//!\~english Specialisation for PixelFormat::D24	\~french Spécialisation pour PixelFormat::D24
	template<> struct component< PixelFormat::D24 >
	{
#define src	( *reinterpret_cast< uint32_t const * >( p_pSrc ) )
#define dst	( *reinterpret_cast< uint32_t * >( p_pSrc ) )

		static inline uint8_t D8( uint8_t const * p_pSrc )
		{
			return uint8_t( ( src & 0x00FFFFFF ) >> 16 );
		}
		static inline uint16_t D16( uint8_t const * p_pSrc )
		{
			return uint16_t( ( src & 0x00FFFFFF ) >> 8 );
		}
		static inline uint32_t D24( uint8_t const * p_pSrc )
		{
			return uint32_t( src & 0x00FFFFFF );
		}
		static inline uint32_t D32( uint8_t const * p_pSrc )
		{
			return ( uint32_t( src & 0x00FFFFFF ) << 8 );
		}
		static inline uint8_t S8( uint8_t const * )
		{
			return 0;
		}
		static inline float D32F( uint8_t const * p_pSrc )
		{
			return float( D32( p_pSrc ) );
		}
		static inline float S32F( uint8_t const * )
		{
			return 0;
		}
		static inline void D8( uint8_t * p_pSrc, uint8_t p_val )
		{
			dst = ( src & 0xFF000000 ) | ( ( uint32_t( p_val ) << 16 ) & 0x00FFFFFF );
		}
		static inline void D16( uint8_t * p_pSrc, uint16_t p_val )
		{
			dst = ( src & 0xFF000000 ) | ( ( uint32_t( p_val ) << 8 ) & 0x00FFFFFF );
		}
		static inline void D24( uint8_t * p_pSrc, uint32_t p_val )
		{
			dst = ( src & 0xFF000000 ) | ( p_val & 0x00FFFFFF );
		}
		static inline void D32( uint8_t * p_pSrc, uint32_t p_val )
		{
			dst = ( src & 0xFF000000 ) | ( ( p_val >> 8 ) & 0x00FFFFFF );
		}
		static inline void S8( uint8_t *, uint8_t )
		{
		}
		static inline void D32F( uint8_t * p_pSrc, float p_val )
		{
			D32( p_pSrc, uint32_t( p_val ) );
		}
		static inline void S32F( uint8_t *, float )
		{
		}
		static inline uint8_t L8( uint8_t const * p_pSrc )
		{
			return D8( p_pSrc );
		}
		static inline uint8_t A8( uint8_t const * p_pSrc )
		{
			return 0;
		}
		static inline uint8_t R8( uint8_t const * p_pSrc )
		{
			return L8( p_pSrc );
		}
		static inline uint8_t G8( uint8_t const * p_pSrc )
		{
			return L8( p_pSrc );
		}
		static inline uint8_t B8( uint8_t const * p_pSrc )
		{
			return L8( p_pSrc );
		}
		static inline float L32F( uint8_t const * p_pSrc )
		{
			return L8( p_pSrc ) / 255.0f;
		}
		static inline float R32F( uint8_t const * p_pSrc )
		{
			return R8( p_pSrc ) / 255.0f;
		}
		static inline float G32F( uint8_t const * p_pSrc )
		{
			return G8( p_pSrc ) / 255.0f;
		}
		static inline float B32F( uint8_t const * p_pSrc )
		{
			return B8( p_pSrc ) / 255.0f;
		}
		static inline float A32F( uint8_t const * p_pSrc )
		{
			return A8( p_pSrc ) / 255.0f;
		}
		static inline void L8( uint8_t * p_pSrc, uint8_t p_val )
		{
			D8( p_pSrc, p_val );
		}
		static inline void A8( uint8_t * p_pSrc, uint8_t p_val )
		{
		}
		static inline void R8( uint8_t * p_pSrc, uint8_t p_val )
		{
			L8( p_pSrc, p_val );
		}
		static inline void G8( uint8_t * p_pSrc, uint8_t p_val )
		{
			L8( p_pSrc, p_val );
		}
		static inline void B8( uint8_t * p_pSrc, uint8_t p_val )
		{
			L8( p_pSrc, p_val );
		}
		static inline void L32F( uint8_t * p_pSrc, float p_val )
		{
			L8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}
		static inline void R32F( uint8_t * p_pSrc, float p_val )
		{
			R8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}
		static inline void G32F( uint8_t * p_pSrc, float p_val )
		{
			G8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}
		static inline void B32F( uint8_t * p_pSrc, float p_val )
		{
			B8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}
		static inline void A32F( uint8_t * p_pSrc, float p_val )
		{
			A8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}

#undef src
#undef dst
	};
	//!\~english Specialisation for PixelFormat::D24S8	\~french Spécialisation pour PixelFormat::D24S8
	template<> struct component< PixelFormat::D24S8 >
	{
#define src	( *reinterpret_cast< uint32_t const * >( p_pSrc ) )
#define dst	( *reinterpret_cast< uint32_t * >( p_pSrc ) )

		static inline uint8_t	BIT( uint8_t const  p_bySrc )
		{
			return ( ( p_bySrc >> 0 ) & 0x01 ) | ( ( p_bySrc >> 1 ) & 0x01 ) | ( ( p_bySrc >> 2 ) & 0x01 ) | ( ( p_bySrc >> 3 ) & 0x01 ) | ( ( p_bySrc >> 4 ) & 0x01 ) | ( ( p_bySrc >> 5 ) & 0x01 ) | ( ( p_bySrc >> 6 ) & 0x01 ) | ( ( p_bySrc >> 7 ) & 0x01 );
		}
		static inline uint8_t D8( uint8_t const * p_pSrc )
		{
			return uint8_t( ( src & 0xFF000000 ) >> 24 );
		}
		static inline uint16_t D16( uint8_t const * p_pSrc )
		{
			return uint16_t( ( src & 0xFFFF0000 ) >> 16 );
		}
		static inline uint32_t D24( uint8_t const * p_pSrc )
		{
			return uint32_t( ( src & 0xFFFFFF00 ) >> 8 );
		}
		static inline uint32_t D32( uint8_t const * p_pSrc )
		{
			return uint32_t( ( src & 0xFFFFFF00 ) >> 8 );
		}
		static inline uint8_t	S1( uint8_t const * p_pSrc )
		{
			return ( BIT( p_pSrc[3] ) << 0 ) | ( BIT( p_pSrc[7] ) << 1 ) | ( BIT( p_pSrc[11] ) << 2 ) | ( BIT( p_pSrc[15] ) << 3 ) | ( BIT( p_pSrc[19] ) << 4 ) | ( BIT( p_pSrc[23] ) << 5 ) | ( BIT( p_pSrc[27] ) << 6 ) | ( BIT( p_pSrc[31] ) << 7 );
		}
		static inline uint8_t S8( uint8_t const * p_pSrc )
		{
			return uint8_t( src & 0x000000FF );
		}
		static inline float D32F( uint8_t const * p_pSrc )
		{
			return float( D32( p_pSrc ) );
		}
		static inline float S32F( uint8_t const * p_pSrc )
		{
			return float( S8( p_pSrc ) ) / 255.0f;
		}
		static inline void D8( uint8_t * p_pSrc, uint8_t p_val )
		{
			dst = ( src & 0x000000FF ) | ( ( uint32_t( p_val ) << 24 ) & 0xFF000000 );
		}
		static inline void D16( uint8_t * p_pSrc, uint16_t p_val )
		{
			dst = ( src & 0x000000FF ) | ( ( uint32_t( p_val ) << 16 ) & 0xFFFF0000 );
		}
		static inline void D24( uint8_t * p_pSrc, uint32_t p_val )
		{
			dst = ( src & 0x000000FF ) | ( ( p_val << 8 ) & 0xFFFFFF00 );
		}
		static inline void D32( uint8_t * p_pSrc, uint32_t p_val )
		{
			dst = ( src & 0x000000FF ) | ( p_val & 0xFFFFFF00 );
		}
		static inline void S1( uint8_t * p_pSrc, uint8_t p_val )
		{
		}
		static inline void S8( uint8_t * p_pSrc, uint8_t p_val )
		{
			dst = ( src & 0xFFFFFF00 ) | ( p_val & 0x000000FF );
		}
		static inline void D32F( uint8_t * p_pSrc, float p_val )
		{
			D32( p_pSrc, uint32_t( p_val ) );
		}
		static inline void S32F( uint8_t * p_pSrc, float p_val )
		{
			S8( p_pSrc, uint8_t( p_val * 255.0f ) );
		}
		static inline uint8_t L8( uint8_t const * p_pSrc )
		{
			return D8( p_pSrc );
		}
		static inline uint8_t A8( uint8_t const * p_pSrc )
		{
			return S8( p_pSrc );
		}
		static inline uint8_t R8( uint8_t const * p_pSrc )
		{
			return L8( p_pSrc );
		}
		static inline uint8_t G8( uint8_t const * p_pSrc )
		{
			return L8( p_pSrc );
		}
		static inline uint8_t B8( uint8_t const * p_pSrc )
		{
			return L8( p_pSrc );
		}
		static inline float L32F( uint8_t const * p_pSrc )
		{
			return L8( p_pSrc ) / 255.0f;
		}
		static inline float R32F( uint8_t const * p_pSrc )
		{
			return R8( p_pSrc ) / 255.0f;
		}
		static inline float G32F( uint8_t const * p_pSrc )
		{
			return G8( p_pSrc ) / 255.0f;
		}
		static inline float B32F( uint8_t const * p_pSrc )
		{
			return B8( p_pSrc ) / 255.0f;
		}
		static inline float A32F( uint8_t const * p_pSrc )
		{
			return A8( p_pSrc ) / 255.0f;
		}
		static inline void L8( uint8_t * p_pSrc, uint8_t p_val )
		{
			D8( p_pSrc, p_val );
		}
		static inline void A8( uint8_t * p_pSrc, uint8_t p_val )
		{
			S8( p_pSrc, p_val );
		}
		static inline void R8( uint8_t * p_pSrc, uint8_t p_val )
		{
			L8( p_pSrc, p_val );
		}
		static inline void G8( uint8_t * p_pSrc, uint8_t p_val )
		{
			L8( p_pSrc, p_val );
		}
		static inline void B8( uint8_t * p_pSrc, uint8_t p_val )
		{
			L8( p_pSrc, p_val );
		}
		static inline void L32F( uint8_t * p_pSrc, float p_val )
		{
			L8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}
		static inline void R32F( uint8_t * p_pSrc, float p_val )
		{
			R8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}
		static inline void G32F( uint8_t * p_pSrc, float p_val )
		{
			G8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}
		static inline void B32F( uint8_t * p_pSrc, float p_val )
		{
			B8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}
		static inline void A32F( uint8_t * p_pSrc, float p_val )
		{
			A8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}

#undef src
#undef dst
	};
	//!\~english Specialisation for PixelFormat::D32	\~french Spécialisation pour PixelFormat::D32
	template<> struct component< PixelFormat::D32 >
	{
#define src	( *reinterpret_cast< uint32_t const * >( p_pSrc ) )
#define dst	( *reinterpret_cast< uint32_t * >( p_pSrc ) )

		static inline uint8_t D8( uint8_t const * p_pSrc )
		{
			return uint8_t( src >> 24 );
		}
		static inline uint16_t D16( uint8_t const * p_pSrc )
		{
			return uint16_t( src >> 16 );
		}
		static inline uint32_t D24( uint8_t const * p_pSrc )
		{
			return ( src >> 8 );
		}
		static inline uint32_t D32( uint8_t const * p_pSrc )
		{
			return src;
		}
		static inline uint8_t S1( uint8_t const * )
		{
			return 0;
		}
		static inline uint8_t S8( uint8_t const * )
		{
			return 0;
		}
		static inline float D32F( uint8_t const * p_pSrc )
		{
			return float( D32( p_pSrc ) );
		}
		static inline float S32F( uint8_t const * )
		{
			return 0;
		}
		static inline void D8( uint8_t * p_pSrc, uint8_t p_val )
		{
			dst = ( uint32_t( p_val ) << 24 );
		}
		static inline void D16( uint8_t * p_pSrc, uint16_t p_val )
		{
			dst = ( uint32_t( p_val ) << 16 );
		}
		static inline void D24( uint8_t * p_pSrc, uint32_t p_val )
		{
			dst = ( p_val << 8 );
		}
		static inline void D32( uint8_t * p_pSrc, uint32_t p_val )
		{
			dst = p_val;
		}
		static inline void S1( uint8_t *, uint8_t )
		{
		}
		static inline void S8( uint8_t *, uint8_t )
		{
		}
		static inline void D32F( uint8_t * p_pSrc, float p_val )
		{
			D32( p_pSrc, uint32_t( p_val ) );
		}
		static inline void S32F( uint8_t *, float )
		{
		}
		static inline uint8_t L8( uint8_t const * p_pSrc )
		{
			return D8( p_pSrc );
		}
		static inline uint8_t A8( uint8_t const * p_pSrc )
		{
			return 0xFF;
		}
		static inline uint8_t R8( uint8_t const * p_pSrc )
		{
			return L8( p_pSrc );
		}
		static inline uint8_t G8( uint8_t const * p_pSrc )
		{
			return L8( p_pSrc );
		}
		static inline uint8_t B8( uint8_t const * p_pSrc )
		{
			return L8( p_pSrc );
		}
		static inline float L32F( uint8_t const * p_pSrc )
		{
			return L8( p_pSrc ) / 255.0f;
		}
		static inline float R32F( uint8_t const * p_pSrc )
		{
			return R8( p_pSrc ) / 255.0f;
		}
		static inline float G32F( uint8_t const * p_pSrc )
		{
			return G8( p_pSrc ) / 255.0f;
		}
		static inline float B32F( uint8_t const * p_pSrc )
		{
			return B8( p_pSrc ) / 255.0f;
		}
		static inline float A32F( uint8_t const * p_pSrc )
		{
			return A8( p_pSrc ) / 255.0f;
		}
		static inline void L8( uint8_t * p_pSrc, uint8_t p_val )
		{
			D8( p_pSrc, p_val );
		}
		static inline void A8( uint8_t * p_pSrc, uint8_t p_val )
		{
		}
		static inline void R8( uint8_t * p_pSrc, uint8_t p_val )
		{
			L8( p_pSrc, p_val );
		}
		static inline void G8( uint8_t * p_pSrc, uint8_t p_val )
		{
			L8( p_pSrc, p_val );
		}
		static inline void B8( uint8_t * p_pSrc, uint8_t p_val )
		{
			L8( p_pSrc, p_val );
		}
		static inline void L32F( uint8_t * p_pSrc, float p_val )
		{
			L8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}
		static inline void R32F( uint8_t * p_pSrc, float p_val )
		{
			R8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}
		static inline void G32F( uint8_t * p_pSrc, float p_val )
		{
			G8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}
		static inline void B32F( uint8_t * p_pSrc, float p_val )
		{
			B8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}
		static inline void A32F( uint8_t * p_pSrc, float p_val )
		{
			A8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}

#undef src
#undef dst
	};
	//!\~english Specialisation for PixelFormat::D32F	\~french Spécialisation pour PixelFormat::D32F
	template<> struct component< PixelFormat::D32F >
	{
#define src	( *reinterpret_cast< float const * >( p_pSrc ) )
#define dst	( *reinterpret_cast< float * >( p_pSrc ) )

		static inline float D32F( uint8_t const * p_pSrc )
		{
			return src;
		}
		static inline float S32F( uint8_t const * )
		{
			return 0;
		}
		static inline uint8_t D8( uint8_t const * p_pSrc )
		{
			return uint8_t( src * 256 );
		}
		static inline uint16_t D16( uint8_t const * p_pSrc )
		{
			return uint8_t( src * 65536 );
		}
		static inline uint32_t D24( uint8_t const * p_pSrc )
		{
			return uint32_t( src * 65536 * 256 );
		}
		static inline uint32_t D32( uint8_t const * p_pSrc )
		{
			return uint32_t( src * 65536 * 65536 );
		}
		static inline uint8_t S1( uint8_t const * )
		{
			return 0;
		}
		static inline uint8_t S8( uint8_t const * )
		{
			return 0;
		}
		static inline void D32F( uint8_t * p_pSrc, float p_val )
		{
			dst = p_val;
		}
		static inline void S32F( uint8_t *, float )
		{
		}
		static inline void D8( uint8_t * p_pSrc, uint8_t p_val )
		{
			dst = float( p_val ) / float( std::numeric_limits< uint8_t >::max() );
		}
		static inline void D16( uint8_t * p_pSrc, uint16_t p_val )
		{
			dst = float( p_val ) / float( std::numeric_limits< uint16_t >::max() );
		}
		static inline void D24( uint8_t * p_pSrc, uint32_t p_val )
		{
			dst = float( p_val ) / float( std::numeric_limits< uint16_t >::max() * std::numeric_limits< uint8_t >::max() );
		}
		static inline void D32( uint8_t * p_pSrc, uint32_t p_val )
		{
			dst = float( p_val ) / float( std::numeric_limits< uint32_t >::max() );
		}
		static inline void S1( uint8_t *, uint8_t )
		{
		}
		static inline void S8( uint8_t *, uint8_t )
		{
		}
		static inline uint8_t L8( uint8_t const * p_pSrc )
		{
			return uint8_t( L32F( p_pSrc ) * 255 );
		}
		static inline uint8_t A8( uint8_t const * p_pSrc )
		{
			return 0xFF;
		}
		static inline uint8_t R8( uint8_t const * p_pSrc )
		{
			return L8( p_pSrc );
		}
		static inline uint8_t G8( uint8_t const * p_pSrc )
		{
			return L8( p_pSrc );
		}
		static inline uint8_t B8( uint8_t const * p_pSrc )
		{
			return L8( p_pSrc );
		}
		static inline float L32F( uint8_t const * p_pSrc )
		{
			return src;
		}
		static inline float R32F( uint8_t const * p_pSrc )
		{
			return L32F( p_pSrc );
		}
		static inline float G32F( uint8_t const * p_pSrc )
		{
			return L32F( p_pSrc );
		}
		static inline float B32F( uint8_t const * p_pSrc )
		{
			return L32F( p_pSrc );
		}
		static inline float A32F( uint8_t const * p_pSrc )
		{
			return 1.0f;
		}
		static inline void L8( uint8_t * p_pSrc, uint8_t p_val )
		{
			D8( p_pSrc, p_val );
		}
		static inline void A8( uint8_t * p_pSrc, uint8_t p_val )
		{
		}
		static inline void R8( uint8_t * p_pSrc, uint8_t p_val )
		{
			L8( p_pSrc, p_val );
		}
		static inline void G8( uint8_t * p_pSrc, uint8_t p_val )
		{
			L8( p_pSrc, p_val );
		}
		static inline void B8( uint8_t * p_pSrc, uint8_t p_val )
		{
			L8( p_pSrc, p_val );
		}
		static inline void L32F( uint8_t * p_pSrc, float p_val )
		{
			dst = p_val;
		}
		static inline void R32F( uint8_t * p_pSrc, float p_val )
		{
			L32F( p_pSrc, p_val );
		}
		static inline void G32F( uint8_t * p_pSrc, float p_val )
		{
			L32F( p_pSrc, p_val );
		}
		static inline void B32F( uint8_t * p_pSrc, float p_val )
		{
			L32F( p_pSrc, p_val );
		}
		static inline void A32F( uint8_t * p_pSrc, float p_val )
		{
			L32F( p_pSrc, p_val );
		}

#undef src
#undef dst
	};
	//!\~english Specialisation for PixelFormat::S8	\~french Spécialisation pour PixelFormat::S8
	template<> struct component< PixelFormat::S8 >
	{
		static inline uint8_t	BIT( uint8_t const  p_bySrc )
		{
			return ( ( p_bySrc >> 0 ) & 0x01 ) | ( ( p_bySrc >> 1 ) & 0x01 ) | ( ( p_bySrc >> 2 ) & 0x01 ) | ( ( p_bySrc >> 3 ) & 0x01 ) | ( ( p_bySrc >> 4 ) & 0x01 ) | ( ( p_bySrc >> 5 ) & 0x01 ) | ( ( p_bySrc >> 6 ) & 0x01 ) | ( ( p_bySrc >> 7 ) & 0x01 );
		}
		static inline uint16_t D16( uint8_t const * )
		{
			return 0;
		}
		static inline uint32_t D24( uint8_t const * )
		{
			return 0;
		}
		static inline uint32_t D32( uint8_t const * )
		{
			return 0;
		}
		static inline uint8_t S1( uint8_t const * p_pSrc )
		{
			return ( BIT( p_pSrc[3] ) << 0 ) | ( BIT( p_pSrc[7] ) << 1 ) | ( BIT( p_pSrc[11] ) << 2 ) | ( BIT( p_pSrc[15] ) << 3 ) | ( BIT( p_pSrc[19] ) << 4 ) | ( BIT( p_pSrc[23] ) << 5 ) | ( BIT( p_pSrc[27] ) << 6 ) | ( BIT( p_pSrc[31] ) << 7 );
		}
		static inline uint8_t S8( uint8_t const * p_pSrc )
		{
			return *p_pSrc;
		}
		static inline float D32F( uint8_t const * )
		{
			return 0;
		}
		static inline float S32F( uint8_t const * p_pSrc )
		{
			return float( S8( p_pSrc ) ) / 255.0f;
		}
		static inline void D16( uint8_t *, uint16_t )
		{
		}
		static inline void D24( uint8_t *, uint32_t )
		{
		}
		static inline void D32( uint8_t *, uint32_t )
		{
		}
		static inline void S1( uint8_t * p_pSrc, uint8_t p_val )
		{
		}
		static inline void S8( uint8_t * p_pSrc, uint8_t p_val )
		{
			*p_pSrc = p_val;
		}
		static inline void D32F( uint8_t *, float )
		{
		}
		static inline void S32F( uint8_t * p_pSrc, float p_val )
		{
			S8( p_pSrc, uint8_t( p_val * 255.0f ) );
		}
		static inline uint8_t L8( uint8_t const * p_pSrc )
		{
			return 0;
		}
		static inline uint8_t A8( uint8_t const * p_pSrc )
		{
			return S8( p_pSrc );
		}
		static inline uint8_t R8( uint8_t const * p_pSrc )
		{
			return L8( p_pSrc );
		}
		static inline uint8_t G8( uint8_t const * p_pSrc )
		{
			return L8( p_pSrc );
		}
		static inline uint8_t B8( uint8_t const * p_pSrc )
		{
			return L8( p_pSrc );
		}
		static inline float L32F( uint8_t const * p_pSrc )
		{
			return L8( p_pSrc ) / 255.0f;
		}
		static inline float R32F( uint8_t const * p_pSrc )
		{
			return R8( p_pSrc ) / 255.0f;
		}
		static inline float G32F( uint8_t const * p_pSrc )
		{
			return G8( p_pSrc ) / 255.0f;
		}
		static inline float B32F( uint8_t const * p_pSrc )
		{
			return B8( p_pSrc ) / 255.0f;
		}
		static inline float A32F( uint8_t const * p_pSrc )
		{
			return A8( p_pSrc ) / 255.0f;
		}
		static inline void L8( uint8_t * p_pSrc, uint8_t p_val )
		{
		}
		static inline void A8( uint8_t * p_pSrc, uint8_t p_val )
		{
			S8( p_pSrc, p_val );
		}
		static inline void R8( uint8_t * p_pSrc, uint8_t p_val )
		{
			L8( p_pSrc, p_val );
		}
		static inline void G8( uint8_t * p_pSrc, uint8_t p_val )
		{
			L8( p_pSrc, p_val );
		}
		static inline void B8( uint8_t * p_pSrc, uint8_t p_val )
		{
			L8( p_pSrc, p_val );
		}
		static inline void L32F( uint8_t * p_pSrc, float p_val )
		{
			L8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}
		static inline void R32F( uint8_t * p_pSrc, float p_val )
		{
			R8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}
		static inline void G32F( uint8_t * p_pSrc, float p_val )
		{
			G8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}
		static inline void B32F( uint8_t * p_pSrc, float p_val )
		{
			B8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}
		static inline void A32F( uint8_t * p_pSrc, float p_val )
		{
			A8( p_pSrc, uint8_t( p_val * 255.0 ) );
		}
	};
	namespace detail
	{
		/*!
		\author 	Sylvain DOREMUS
		\date 		27/05/2013
		\~english
		\brief		Structure used to convert one pixel from one format to another one
		\~french
		\brief		Structure utilisée pour convertir un pixel d'un format vers un autre
		*/
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst, typename Enable = void > struct pixel_converter;
		//!\~english Specialisation when source and destination formats are the same	\~french Spécialisation quand les formats source et destination sont identiques
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter< PFSrc, PFDst, typename std::enable_if< PFSrc == PFDst >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				std::memcpy( p_pDst, p_pSrc, pixel_definitions< PFSrc >::Size );
				p_pSrc += pixel_definitions< PFSrc >::Size;
				p_pDst += pixel_definitions< PFDst >::Size;
			}
		};
		//!\~english Specialisation for converting to PixelFormat::L8	\~french Spécialisation pour convertir vers PixelFormat::L8
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == PixelFormat::L8 >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				component< PFDst >::L8( p_pDst, component< PFSrc >::L8( p_pSrc ) );
				p_pSrc += pixel_definitions< PFSrc >::Size;
				p_pDst += pixel_definitions< PFDst >::Size;
			}
		};
		//!\~english Specialisation for converting to PixelFormat::L16F32F	\~french Spécialisation pour convertir vers PixelFormat::L16F32F
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == PixelFormat::L16F32F >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				component< PFDst >::L32F( p_pDst, component< PFSrc >::L32F( p_pSrc ) );
				p_pSrc += pixel_definitions< PFSrc >::Size;
				p_pDst += pixel_definitions< PFDst >::Size;
			}
		};
		//!\~english Specialisation for converting to PixelFormat::L32F	\~french Spécialisation pour convertir vers PixelFormat::L32F
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == PixelFormat::L32F >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				component< PFDst >::L32F( p_pDst, component< PFSrc >::L32F( p_pSrc ) );
				p_pSrc += pixel_definitions< PFSrc >::Size;
				p_pDst += pixel_definitions< PFDst >::Size;
			}
		};
		//!\~english Specialisation for converting to PixelFormat::A8L8	\~french Spécialisation pour convertir vers PixelFormat::A8L8
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == PixelFormat::A8L8 >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				component< PFDst >::L8( p_pDst, component< PFSrc >::L8( p_pSrc ) );
				component< PFDst >::A8( p_pDst, component< PFSrc >::A8( p_pSrc ) );
				p_pSrc += pixel_definitions< PFSrc >::Size;
				p_pDst += pixel_definitions< PFDst >::Size;
			}
		};
		//!\~english Specialisation for converting to PixelFormat::AL16F32F	\~french Spécialisation pour convertir vers PixelFormat::AL16F32F
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == PixelFormat::AL16F32F >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				component< PFDst >::L32F( p_pDst, component< PFSrc >::L32F( p_pSrc ) );
				component< PFDst >::A32F( p_pDst, component< PFSrc >::A32F( p_pSrc ) );
				p_pSrc += pixel_definitions< PFSrc >::Size;
				p_pDst += pixel_definitions< PFDst >::Size;
			}
		};
		//!\~english Specialisation for converting to PixelFormat::AL32F	\~french Spécialisation pour convertir vers PixelFormat::AL32F
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == PixelFormat::AL32F >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				component< PFDst >::L32F( p_pDst, component< PFSrc >::L32F( p_pSrc ) );
				component< PFDst >::A32F( p_pDst, component< PFSrc >::A32F( p_pSrc ) );
				p_pSrc += pixel_definitions< PFSrc >::Size;
				p_pDst += pixel_definitions< PFDst >::Size;
			}
		};
		//!\~english Specialisation for converting to PixelFormat::A1R5G5B5	\~french Spécialisation pour convertir vers PixelFormat::A1R5G5B5
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == PixelFormat::A1R5G5B5 >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				component< PFDst >::A8( p_pDst, component< PFSrc >::A8( p_pSrc ) );
				component< PFDst >::R8( p_pDst, component< PFSrc >::R8( p_pSrc ) );
				component< PFDst >::G8( p_pDst, component< PFSrc >::G8( p_pSrc ) );
				component< PFDst >::B8( p_pDst, component< PFSrc >::B8( p_pSrc ) );
				p_pSrc += pixel_definitions< PFSrc >::Size;
				p_pDst += pixel_definitions< PFDst >::Size;
			}
		};
		//!\~english Specialisation for converting to PixelFormat::A4R4G4B4	\~french Spécialisation pour convertir vers PixelFormat::A4R4G4B4
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == PixelFormat::A4R4G4B4 >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				component< PFDst >::A8( p_pDst, component< PFSrc >::A8( p_pSrc ) );
				component< PFDst >::R8( p_pDst, component< PFSrc >::R8( p_pSrc ) );
				component< PFDst >::G8( p_pDst, component< PFSrc >::G8( p_pSrc ) );
				component< PFDst >::B8( p_pDst, component< PFSrc >::B8( p_pSrc ) );
				p_pSrc += pixel_definitions< PFSrc >::Size;
				p_pDst += pixel_definitions< PFDst >::Size;
			}
		};
		//!\~english Specialisation for converting to PixelFormat::R5G6B5	\~french Spécialisation pour convertir vers PixelFormat::R5G6B5
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == PixelFormat::R5G6B5 >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				component< PFDst >::R8( p_pDst, component< PFSrc >::R8( p_pSrc ) );
				component< PFDst >::G8( p_pDst, component< PFSrc >::G8( p_pSrc ) );
				component< PFDst >::B8( p_pDst, component< PFSrc >::B8( p_pSrc ) );
				p_pSrc += pixel_definitions< PFSrc >::Size;
				p_pDst += pixel_definitions< PFDst >::Size;
			}
		};
		//!\~english Specialisation for converting to PixelFormat::R8G8B8	\~french Spécialisation pour convertir vers PixelFormat::R8G8B8
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == PixelFormat::R8G8B8 >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				component< PFDst >::R8( p_pDst, component< PFSrc >::R8( p_pSrc ) );
				component< PFDst >::G8( p_pDst, component< PFSrc >::G8( p_pSrc ) );
				component< PFDst >::B8( p_pDst, component< PFSrc >::B8( p_pSrc ) );
				p_pSrc += pixel_definitions< PFSrc >::Size;
				p_pDst += pixel_definitions< PFDst >::Size;
			}
		};
		//!\~english Specialisation for converting to PixelFormat::B8G8R8	\~french Spécialisation pour convertir vers PixelFormat::B8G8R8
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == PixelFormat::B8G8R8 >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				component< PFDst >::B8( p_pDst, component< PFSrc >::B8( p_pSrc ) );
				component< PFDst >::G8( p_pDst, component< PFSrc >::G8( p_pSrc ) );
				component< PFDst >::R8( p_pDst, component< PFSrc >::R8( p_pSrc ) );
				p_pSrc += pixel_definitions< PFSrc >::Size;
				p_pDst += pixel_definitions< PFDst >::Size;
			}
		};
		//!\~english Specialisation for converting to PixelFormat::A8R8G8B8	\~french Spécialisation pour convertir vers PixelFormat::A8R8G8B8
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == PixelFormat::A8R8G8B8 >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				component< PFDst >::A8( p_pDst, component< PFSrc >::A8( p_pSrc ) );
				component< PFDst >::R8( p_pDst, component< PFSrc >::R8( p_pSrc ) );
				component< PFDst >::G8( p_pDst, component< PFSrc >::G8( p_pSrc ) );
				component< PFDst >::B8( p_pDst, component< PFSrc >::B8( p_pSrc ) );
				p_pSrc += pixel_definitions< PFSrc >::Size;
				p_pDst += pixel_definitions< PFDst >::Size;
			}
		};
		//!\~english Specialisation for converting to PixelFormat::A8R8G8B8	\~french Spécialisation pour convertir vers PixelFormat::A8R8G8B8
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == PixelFormat::A8B8G8R8 >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				component< PFDst >::A8( p_pDst, component< PFSrc >::A8( p_pSrc ) );
				component< PFDst >::B8( p_pDst, component< PFSrc >::B8( p_pSrc ) );
				component< PFDst >::G8( p_pDst, component< PFSrc >::G8( p_pSrc ) );
				component< PFDst >::R8( p_pDst, component< PFSrc >::R8( p_pSrc ) );
				p_pSrc += pixel_definitions< PFSrc >::Size;
				p_pDst += pixel_definitions< PFDst >::Size;
			}
		};
		//!\~english Specialisation for converting to PixelFormat::RGB16F	\~french Spécialisation pour convertir vers PixelFormat::RGB16F
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == PixelFormat::RGB16F >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				component< PFDst >::R32F( p_pDst, component< PFSrc >::R32F( p_pSrc ) );
				component< PFDst >::G32F( p_pDst, component< PFSrc >::G32F( p_pSrc ) );
				component< PFDst >::B32F( p_pDst, component< PFSrc >::B32F( p_pSrc ) );
				p_pSrc += pixel_definitions< PFSrc >::Size;
				p_pDst += pixel_definitions< PFDst >::Size;
			}
		};
		//!\~english Specialisation for converting to PixelFormat::RGB16F32F	\~french Spécialisation pour convertir vers PixelFormat::RGB16F32F
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == PixelFormat::RGB16F32F >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				component< PFDst >::R32F( p_pDst, component< PFSrc >::R32F( p_pSrc ) );
				component< PFDst >::G32F( p_pDst, component< PFSrc >::G32F( p_pSrc ) );
				component< PFDst >::B32F( p_pDst, component< PFSrc >::B32F( p_pSrc ) );
				p_pSrc += pixel_definitions< PFSrc >::Size;
				p_pDst += pixel_definitions< PFDst >::Size;
			}
		};
		//!\~english Specialisation for converting to PixelFormat::RGB32F	\~french Spécialisation pour convertir vers PixelFormat::RGB32F
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == PixelFormat::RGB32F >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				component< PFDst >::R32F( p_pDst, component< PFSrc >::R32F( p_pSrc ) );
				component< PFDst >::G32F( p_pDst, component< PFSrc >::G32F( p_pSrc ) );
				component< PFDst >::B32F( p_pDst, component< PFSrc >::B32F( p_pSrc ) );
				p_pSrc += pixel_definitions< PFSrc >::Size;
				p_pDst += pixel_definitions< PFDst >::Size;
			}
		};
		//!\~english Specialisation for converting to PixelFormat::RGBA16F	\~french Spécialisation pour convertir vers PixelFormat::RGBA16F
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == PixelFormat::RGBA16F >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				component< PFDst >::A32F( p_pDst, component< PFSrc >::A32F( p_pSrc ) );
				component< PFDst >::R32F( p_pDst, component< PFSrc >::R32F( p_pSrc ) );
				component< PFDst >::G32F( p_pDst, component< PFSrc >::G32F( p_pSrc ) );
				component< PFDst >::B32F( p_pDst, component< PFSrc >::B32F( p_pSrc ) );
				p_pSrc += pixel_definitions< PFSrc >::Size;
				p_pDst += pixel_definitions< PFDst >::Size;
			}
		};
		//!\~english Specialisation for converting to PixelFormat::RGBA16F32F	\~french Spécialisation pour convertir vers PixelFormat::RGBA16F32F
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == PixelFormat::RGBA16F32F >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				component< PFDst >::A32F( p_pDst, component< PFSrc >::A32F( p_pSrc ) );
				component< PFDst >::R32F( p_pDst, component< PFSrc >::R32F( p_pSrc ) );
				component< PFDst >::G32F( p_pDst, component< PFSrc >::G32F( p_pSrc ) );
				component< PFDst >::B32F( p_pDst, component< PFSrc >::B32F( p_pSrc ) );
				p_pSrc += pixel_definitions< PFSrc >::Size;
				p_pDst += pixel_definitions< PFDst >::Size;
			}
		};
		//!\~english Specialisation for converting to PixelFormat::RGBA32F	\~french Spécialisation pour convertir vers PixelFormat::RGBA32F
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == PixelFormat::RGBA32F >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				component< PFDst >::A32F( p_pDst, component< PFSrc >::A32F( p_pSrc ) );
				component< PFDst >::R32F( p_pDst, component< PFSrc >::R32F( p_pSrc ) );
				component< PFDst >::G32F( p_pDst, component< PFSrc >::G32F( p_pSrc ) );
				component< PFDst >::B32F( p_pDst, component< PFSrc >::B32F( p_pSrc ) );
				p_pSrc += pixel_definitions< PFSrc >::Size;
				p_pDst += pixel_definitions< PFDst >::Size;
			}
		};
		//!\~english Specialisation for converting to PixelFormat::YUY2	\~french Spécialisation pour convertir vers PixelFormat::YUY2
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == PixelFormat::YUY2 >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				p_pDst[0] = ( ( ( ( ( 66 * component< PFSrc >::R8( p_pSrc ) + 129 * component< PFSrc >::G8( p_pSrc ) +  25 * component< PFSrc >::B8( p_pSrc ) + 128 ) >> 8 ) +  16 ) & 0x0F ) << 4 ) | ( ( ( ( -38 * component< PFSrc >::R8( p_pSrc ) - 74 * component< PFSrc >::G8( p_pSrc ) + 112 * component< PFSrc >::B8( p_pSrc ) + 128 ) >> 8 ) + 128 ) & 0x0F );
				p_pDst[1] = ( ( ( ( ( 66 * component< PFSrc >::R8( p_pSrc ) + 129 * component< PFSrc >::G8( p_pSrc ) +  25 * component< PFSrc >::B8( p_pSrc ) + 128 ) >> 8 ) +  16 ) & 0x0F ) << 4 ) | ( ( ( ( 112 * component< PFSrc >::R8( p_pSrc ) - 94 * component< PFSrc >::G8( p_pSrc ) -  18 * component< PFSrc >::B8( p_pSrc ) + 128 ) >> 8 ) + 128 ) & 0x0F );
				p_pSrc += pixel_definitions< PFSrc >::Size;
				p_pDst += pixel_definitions< PFDst >::Size;
			}
		};
		//!\~english Specialisation for converting to PixelFormat::D16	\~french Spécialisation pour convertir vers PixelFormat::D16
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == PixelFormat::D16 >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				component< PFDst >::D16( p_pDst, component< PFSrc >::D16( p_pSrc ) );
				p_pSrc += pixel_definitions< PFSrc >::Size;
				p_pDst += pixel_definitions< PFDst >::Size;
			}
		};
		//!\~english Specialisation for converting to PixelFormat::D24	\~french Spécialisation pour convertir vers PixelFormat::D24
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == PixelFormat::D24 >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				component< PFDst >::D24( p_pDst, component< PFSrc >::D24( p_pSrc ) );
				p_pSrc += pixel_definitions< PFSrc >::Size;
				p_pDst += pixel_definitions< PFDst >::Size;
			}
		};
		//!\~english Specialisation for converting to PixelFormat::D24S8	\~french Spécialisation pour convertir vers PixelFormat::D24S8
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFSrc != PixelFormat::S1 && PFDst == PixelFormat::D24S8 >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				component< PFDst >::D24( p_pDst, component< PFSrc >::D24( p_pSrc ) );
				component< PFDst >::S8( p_pDst, component< PFSrc >::S8( p_pSrc ) );
				p_pSrc += pixel_definitions< PFSrc >::Size;
				p_pDst += pixel_definitions< PFDst >::Size;
			}
		};
		//!\~english Specialisation for converting to PixelFormat::D32	\~french Spécialisation pour convertir vers PixelFormat::D32
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == PixelFormat::D32 >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				component< PFDst >::D32( p_pDst, component< PFSrc >::D32( p_pSrc ) );
				p_pSrc += pixel_definitions< PFSrc >::Size;
				p_pDst += pixel_definitions< PFDst >::Size;
			}
		};
		//!\~english Specialisation for converting to PixelFormat::D32F	\~french Spécialisation pour convertir vers PixelFormat::D32F
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == PixelFormat::D32F >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				component< PFDst >::D32F( p_pDst, component< PFSrc >::D32F( p_pSrc ) );
				p_pSrc += pixel_definitions< PFSrc >::Size;
				p_pDst += pixel_definitions< PFDst >::Size;
			}
		};
		//!\~english Specialisation for converting to PixelFormat::S1	\~french Spécialisation pour convertir vers PixelFormat::S1
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == PixelFormat::S1 >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				*p_pDst = component< PFSrc >::S1( p_pSrc );
				p_pSrc += pixel_definitions< PFSrc >::Size;
				p_pDst += pixel_definitions< PFDst >::Size;
			}
		};
		//!\~english Specialisation for converting to PixelFormat::S8	\~french Spécialisation pour convertir vers PixelFormat::S8
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFSrc != PixelFormat::S1 && PFDst == PixelFormat::S8 >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				component< PFDst >::S8( p_pDst, component< PFSrc >::S8( p_pSrc ) );
				p_pSrc += pixel_definitions< PFSrc >::Size;
				p_pDst += pixel_definitions< PFDst >::Size;
			}
		};
		//!\~english Specialisation for convertions from PixelFormat::S1 to PixelFormat::S8	\~french Spécialisation pour convertir de PixelFormat::S1 vers PixelFormat::S8
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFSrc == PixelFormat::S1 && PFDst == PixelFormat::S8 >::type >
		{
			inline uint8_t BIT( uint8_t const src, uint8_t uiIndex )
			{
				return ( ( src >> uiIndex ) & 0x01 );
			}

			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				p_pDst[0] = ( BIT( *p_pSrc, 0 ) << 0 ) | ( BIT( *p_pSrc, 0 ) << 1 ) | ( BIT( *p_pSrc, 0 ) << 2 ) | ( BIT( *p_pSrc, 0 ) << 3 ) | ( BIT( *p_pSrc, 0 ) << 4 ) | ( BIT( *p_pSrc, 0 ) << 5 ) | ( BIT( *p_pSrc, 0 ) << 6 ) | ( BIT( *p_pSrc, 0 ) << 7 );
				p_pDst[1] = ( BIT( *p_pSrc, 1 ) << 0 ) | ( BIT( *p_pSrc, 1 ) << 1 ) | ( BIT( *p_pSrc, 1 ) << 2 ) | ( BIT( *p_pSrc, 1 ) << 3 ) | ( BIT( *p_pSrc, 1 ) << 4 ) | ( BIT( *p_pSrc, 1 ) << 5 ) | ( BIT( *p_pSrc, 1 ) << 6 ) | ( BIT( *p_pSrc, 1 ) << 7 );
				p_pDst[2] = ( BIT( *p_pSrc, 2 ) << 0 ) | ( BIT( *p_pSrc, 2 ) << 1 ) | ( BIT( *p_pSrc, 2 ) << 2 ) | ( BIT( *p_pSrc, 2 ) << 3 ) | ( BIT( *p_pSrc, 2 ) << 4 ) | ( BIT( *p_pSrc, 2 ) << 5 ) | ( BIT( *p_pSrc, 2 ) << 6 ) | ( BIT( *p_pSrc, 2 ) << 7 );
				p_pDst[3] = ( BIT( *p_pSrc, 3 ) << 0 ) | ( BIT( *p_pSrc, 3 ) << 1 ) | ( BIT( *p_pSrc, 3 ) << 2 ) | ( BIT( *p_pSrc, 3 ) << 3 ) | ( BIT( *p_pSrc, 3 ) << 4 ) | ( BIT( *p_pSrc, 3 ) << 5 ) | ( BIT( *p_pSrc, 3 ) << 6 ) | ( BIT( *p_pSrc, 3 ) << 7 );
				p_pDst[4] = ( BIT( *p_pSrc, 4 ) << 0 ) | ( BIT( *p_pSrc, 4 ) << 1 ) | ( BIT( *p_pSrc, 4 ) << 2 ) | ( BIT( *p_pSrc, 4 ) << 3 ) | ( BIT( *p_pSrc, 4 ) << 4 ) | ( BIT( *p_pSrc, 4 ) << 5 ) | ( BIT( *p_pSrc, 4 ) << 6 ) | ( BIT( *p_pSrc, 4 ) << 7 );
				p_pDst[5] = ( BIT( *p_pSrc, 5 ) << 0 ) | ( BIT( *p_pSrc, 5 ) << 1 ) | ( BIT( *p_pSrc, 5 ) << 2 ) | ( BIT( *p_pSrc, 5 ) << 3 ) | ( BIT( *p_pSrc, 5 ) << 4 ) | ( BIT( *p_pSrc, 5 ) << 5 ) | ( BIT( *p_pSrc, 5 ) << 6 ) | ( BIT( *p_pSrc, 5 ) << 7 );
				p_pDst[6] = ( BIT( *p_pSrc, 6 ) << 0 ) | ( BIT( *p_pSrc, 6 ) << 1 ) | ( BIT( *p_pSrc, 6 ) << 2 ) | ( BIT( *p_pSrc, 6 ) << 3 ) | ( BIT( *p_pSrc, 6 ) << 4 ) | ( BIT( *p_pSrc, 6 ) << 5 ) | ( BIT( *p_pSrc, 6 ) << 6 ) | ( BIT( *p_pSrc, 6 ) << 7 );
				p_pDst[7] = ( BIT( *p_pSrc, 7 ) << 0 ) | ( BIT( *p_pSrc, 7 ) << 1 ) | ( BIT( *p_pSrc, 7 ) << 2 ) | ( BIT( *p_pSrc, 7 ) << 3 ) | ( BIT( *p_pSrc, 7 ) << 4 ) | ( BIT( *p_pSrc, 7 ) << 5 ) | ( BIT( *p_pSrc, 7 ) << 6 ) | ( BIT( *p_pSrc, 7 ) << 7 );
				p_pSrc += pixel_definitions< PFSrc >::Size;
				p_pDst += pixel_definitions< PFDst >::Size;
			}
		};
		//!\~english Specialisation for convertions from PixelFormat::S1 to PixelFormat::D24S8	\~french Spécialisation pour convertir de PixelFormat::S1 vers PixelFormat::D24S8
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFSrc == PixelFormat::S1 && PFDst == PixelFormat::D24S8 >::type >
		{
			inline uint8_t BIT( uint8_t const src, uint8_t uiIndex )
			{
				return ( ( src >> uiIndex ) & 0x01 );
			}

			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				uint32_t * l_pDst = reinterpret_cast< uint32_t * >( &p_pDst[0] );
				l_pDst[0] = 0xFFFFFF00 + ( ( BIT( *p_pSrc, 0 ) << 0 ) | ( BIT( *p_pSrc, 0 ) << 1 ) | ( BIT( *p_pSrc, 0 ) << 2 ) | ( BIT( *p_pSrc, 0 ) << 3 ) | ( BIT( *p_pSrc, 0 ) << 4 ) | ( BIT( *p_pSrc, 0 ) << 5 ) | ( BIT( *p_pSrc, 0 ) << 6 ) | ( BIT( *p_pSrc, 0 ) << 7 ) );
				l_pDst[1] = 0xFFFFFF00 + ( ( BIT( *p_pSrc, 1 ) << 0 ) | ( BIT( *p_pSrc, 1 ) << 1 ) | ( BIT( *p_pSrc, 1 ) << 2 ) | ( BIT( *p_pSrc, 1 ) << 3 ) | ( BIT( *p_pSrc, 1 ) << 4 ) | ( BIT( *p_pSrc, 1 ) << 5 ) | ( BIT( *p_pSrc, 1 ) << 6 ) | ( BIT( *p_pSrc, 1 ) << 7 ) );
				l_pDst[2] = 0xFFFFFF00 + ( ( BIT( *p_pSrc, 2 ) << 0 ) | ( BIT( *p_pSrc, 2 ) << 1 ) | ( BIT( *p_pSrc, 2 ) << 2 ) | ( BIT( *p_pSrc, 2 ) << 3 ) | ( BIT( *p_pSrc, 2 ) << 4 ) | ( BIT( *p_pSrc, 2 ) << 5 ) | ( BIT( *p_pSrc, 2 ) << 6 ) | ( BIT( *p_pSrc, 2 ) << 7 ) );
				l_pDst[3] = 0xFFFFFF00 + ( ( BIT( *p_pSrc, 3 ) << 0 ) | ( BIT( *p_pSrc, 3 ) << 1 ) | ( BIT( *p_pSrc, 3 ) << 2 ) | ( BIT( *p_pSrc, 3 ) << 3 ) | ( BIT( *p_pSrc, 3 ) << 4 ) | ( BIT( *p_pSrc, 3 ) << 5 ) | ( BIT( *p_pSrc, 3 ) << 6 ) | ( BIT( *p_pSrc, 3 ) << 7 ) );
				l_pDst[4] = 0xFFFFFF00 + ( ( BIT( *p_pSrc, 4 ) << 0 ) | ( BIT( *p_pSrc, 4 ) << 1 ) | ( BIT( *p_pSrc, 4 ) << 2 ) | ( BIT( *p_pSrc, 4 ) << 3 ) | ( BIT( *p_pSrc, 4 ) << 4 ) | ( BIT( *p_pSrc, 4 ) << 5 ) | ( BIT( *p_pSrc, 4 ) << 6 ) | ( BIT( *p_pSrc, 4 ) << 7 ) );
				l_pDst[5] = 0xFFFFFF00 + ( ( BIT( *p_pSrc, 5 ) << 0 ) | ( BIT( *p_pSrc, 5 ) << 1 ) | ( BIT( *p_pSrc, 5 ) << 2 ) | ( BIT( *p_pSrc, 5 ) << 3 ) | ( BIT( *p_pSrc, 5 ) << 4 ) | ( BIT( *p_pSrc, 5 ) << 5 ) | ( BIT( *p_pSrc, 5 ) << 6 ) | ( BIT( *p_pSrc, 5 ) << 7 ) );
				l_pDst[6] = 0xFFFFFF00 + ( ( BIT( *p_pSrc, 6 ) << 0 ) | ( BIT( *p_pSrc, 6 ) << 1 ) | ( BIT( *p_pSrc, 6 ) << 2 ) | ( BIT( *p_pSrc, 6 ) << 3 ) | ( BIT( *p_pSrc, 6 ) << 4 ) | ( BIT( *p_pSrc, 6 ) << 5 ) | ( BIT( *p_pSrc, 6 ) << 6 ) | ( BIT( *p_pSrc, 6 ) << 7 ) );
				l_pDst[7] = 0xFFFFFF00 + ( ( BIT( *p_pSrc, 7 ) << 0 ) | ( BIT( *p_pSrc, 7 ) << 1 ) | ( BIT( *p_pSrc, 7 ) << 2 ) | ( BIT( *p_pSrc, 7 ) << 3 ) | ( BIT( *p_pSrc, 7 ) << 4 ) | ( BIT( *p_pSrc, 7 ) << 5 ) | ( BIT( *p_pSrc, 7 ) << 6 ) | ( BIT( *p_pSrc, 7 ) << 7 ) );
				p_pSrc += pixel_definitions< PFSrc >::Size;
				p_pDst += pixel_definitions< PFDst >::Size;
			}
		};
		/*!
		\author 	Sylvain DOREMUS
		\date 		08/09/2013
		\~english
		\brief		Structure used to convert a buffer from one pixel format to another one
		\~french
		\brief		Structure utilisée pour convertir un buffer d'un format de pixels vers un autre
		*/
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct buffer_converter
		{
			inline void operator()( uint8_t const * p_pSrcBuffer, uint32_t p_uiSrcSize, uint8_t * p_pDstBuffer, uint32_t p_uiDstSize )
			{
				uint8_t const * l_pSrc = &p_pSrcBuffer[0];
				uint8_t * l_pDst = &p_pDstBuffer[0];
				uint32_t l_uiSrcCount = 0;
				uint32_t l_count = p_uiSrcSize / pixel_definitions< PFSrc >::Size;

				for ( uint32_t i = 0; i < l_count; i++ )
				{
					pixel_converter< PFSrc, PFDst >()( l_pSrc, l_pDst );
				}
			}
		};

#if defined( _X64 )

		/*!
		\author 	Sylvain DOREMUS
		\date 		08/09/2013
		\~english
		\brief		Specialisation for YUY2 to A8R8G8B8 conversion
		\~french
		\brief		Spécialisation pour la conversion YUY2 vers A8R8G8B8
		*/
		template<> struct buffer_converter< PixelFormat::YUY2, PixelFormat::A8R8G8B8 >
		{
			static const __m128i amask;
			static const __m128i umask;
			static const __m128i vmask;
			static const __m128i ymask;
			static const __m128i uvsub;
			static const __m128i ysub;
			static const __m128i facy;
			static const __m128i facrv;
			static const __m128i facgu;
			static const __m128i facgv;
			static const __m128i facbu;
			static const __m128i zero;

			inline void operator()( uint8_t const * p_pSrcBuffer, uint32_t p_uiSrcSize, uint8_t * p_pDstBuffer, uint32_t p_uiDstSize )
			{
				__m128i yuv;
				__m128i y;
				__m128i	u;
				__m128i	v;
				__m128i rv0;
				__m128i gu0;
				__m128i gv0;
				__m128i bu0;
				__m128i r0;
				__m128i g0;
				__m128i b0;
				__m128i r1;
				__m128i gb;
				__m128i argb0123;
				__m128i argb4567;
				__m128i const *	l_pBufferIn		= l_pBufferIn = reinterpret_cast< __m128i const * >( p_pSrcBuffer );
				__m128i const *	l_pBufferInEnd	= reinterpret_cast< __m128i const * >( p_pSrcBuffer + p_uiSrcSize );
				__m128i const *	l_pLineIn		= l_pBufferIn;
				__m128i 	*	l_pBufferOut	= reinterpret_cast< __m128i * >( p_pDstBuffer );

				while ( l_pBufferIn != l_pBufferInEnd )
				{
					// On récupère les composantes YUV
					yuv			= _mm_load_si128( l_pLineIn++ );
					// On récupère dans Y les Y uniquement
					y			= _mm_and_si128( yuv, ymask );
					y			= _mm_sub_epi16( y, ysub );
					// dans U les U uniquement
					u			= _mm_and_si128( yuv, umask );
					// que l'on duplique pour en avoir autant que des Y
					u			= _mm_or_si128( _mm_srli_epi32( u, 8 ), _mm_slli_epi32( u, 8 ) );
					u			= _mm_sub_epi16( u, uvsub );
					// dans V les V uniquement
					v			= _mm_and_si128( yuv, vmask );
					// que l'on duplique pour en avoir autant que des Y
					v			= _mm_or_si128( _mm_srli_epi32( v, 8 ), _mm_srli_epi32( v, 24 ) );
					v			= _mm_sub_epi16( v, uvsub );
					// On calcule les facteurs finaux YUV
					y			= _mm_mullo_epi16( facy, y );
					rv0			= _mm_mullo_epi16( facrv, v );
					gu0			= _mm_mullo_epi16( facgu, u );
					gv0			= _mm_mullo_epi16( facgv, v );
					bu0			= _mm_mullo_epi16( facbu, u );
					// Maintenant on calcule les facteurs RGB
					r0			= _mm_srai_epi16( _mm_add_epi16( y, rv0 ), 6 );
					g0			= _mm_srai_epi16( _mm_sub_epi16( y, _mm_add_epi16( gu0, gv0 ) ), 6 );
					b0			= _mm_srai_epi16( _mm_add_epi16( y, bu0 ), 6 );
					// Qu'on sature (clamp entre 0 et 255)
					r0			= _mm_packus_epi16( r0, zero );		// 00000000rrrrrrrr saturated
					g0			= _mm_packus_epi16( g0, zero );		// 00000000gggggggg saturated
					b0			= _mm_packus_epi16( b0, zero );		// 00000000bbbbbbbb saturated
					// Et que l'on ordonne
					r1			= _mm_unpacklo_epi8( zero, b0 );	// b0b0b0b0b0b0b0b0
					gb			= _mm_unpacklo_epi8( g0, r0 );		// rgrgrgrgrgrgrgrg
					argb0123	= _mm_unpacklo_epi16( r1, gb );		// rgb0rgb0rgb0rgb0
					argb4567	= _mm_unpackhi_epi16( r1, gb );		// rgb0rgb0rgb0rgb0
					argb0123	= _mm_or_si128( amask, argb0123 );	// rgbFrgbFrgbFrgbF
					argb4567	= _mm_or_si128( amask, argb4567 );	// rgbFrgbFrgbFrgbF
					// Enfin on fout tout ce bordel dans le buffer
					std::memcpy( l_pBufferOut, &argb0123, sizeof( __m128i ) );
					l_pBufferOut++;
					std::memcpy( l_pBufferOut, &argb4567, sizeof( __m128i ) );
					l_pBufferOut++;
				}
			}
		};
		/*!
		\author 	Sylvain DOREMUS
		\date 		08/09/2013
		\~english
		\brief		Specialisation for YUY2 to R8G8B8 conversion
		\~french
		\brief		Spécialisation pour la conversion YUY2 vers R8G8B8
		*/
		template<> struct buffer_converter< PixelFormat::YUY2, PixelFormat::R8G8B8 >
		{
			static const __m128i umask;
			static const __m128i vmask;
			static const __m128i ymask;
			static const __m128i uvsub;
			static const __m128i ysub;
			static const __m128i facy;
			static const __m128i facrv;
			static const __m128i facgu;
			static const __m128i facgv;
			static const __m128i facbu;
			static const __m128i zero;

			inline void operator()( uint8_t const * p_pSrcBuffer, uint32_t p_uiSrcSize, uint8_t * p_pDstBuffer, uint32_t p_uiDstSize )
			{
				__m128i yuv;
				__m128i y;
				__m128i	u;
				__m128i	v;
				__m128i rv0;
				__m128i gu0;
				__m128i gv0;
				__m128i bu0;
				__m128i r0;
				__m128i g0;
				__m128i b0;
				__m128i r1;
				__m128i gb;
				__m128i argb0123;
				__m128i argb4567;
				__m128i const *	l_pBufferIn		= l_pBufferIn = reinterpret_cast< __m128i const * >( p_pSrcBuffer );
				__m128i const *	l_pBufferInEnd	= reinterpret_cast< __m128i const * >( p_pSrcBuffer + p_uiSrcSize );
				__m128i const *	l_pLineIn		= l_pBufferIn;
				uint8_t 	*	l_pBufferOut	= p_pDstBuffer;

				while ( l_pBufferIn != l_pBufferInEnd )
				{
					// On récupère les composantes YUV
					yuv			= _mm_load_si128( l_pLineIn++ );
					// On récupère dans Y les Y uniquement
					y			= _mm_and_si128( yuv, ymask );
					y			= _mm_sub_epi16( y, ysub );
					// dans U les U uniquement
					u			= _mm_and_si128( yuv, umask );
					// que l'on duplique pour en avoir autant que des Y
					u			= _mm_or_si128( _mm_srli_epi32( u, 8 ), _mm_slli_epi32( u, 8 ) );
					u			= _mm_sub_epi16( u, uvsub );
					// dans V les V uniquement
					v			= _mm_and_si128( yuv, vmask );
					// que l'on duplique pour en avoir autant que des Y
					v			= _mm_or_si128( _mm_srli_epi32( v, 8 ), _mm_srli_epi32( v, 24 ) );
					v			= _mm_sub_epi16( v, uvsub );
					// On calcule les facteurs finaux YUV
					y			= _mm_mullo_epi16( facy, y );
					rv0			= _mm_mullo_epi16( facrv, v );
					gu0			= _mm_mullo_epi16( facgu, u );
					gv0			= _mm_mullo_epi16( facgv, v );
					bu0			= _mm_mullo_epi16( facbu, u );
					// Maintenant on calcule les facteurs RGB
					r0			= _mm_srai_epi16( _mm_add_epi16( y, rv0 ), 6 );
					g0			= _mm_srai_epi16( _mm_sub_epi16( y, _mm_add_epi16( gu0, gv0 ) ), 6 );
					b0			= _mm_srai_epi16( _mm_add_epi16( y, bu0 ), 6 );
					// Qu'on sature (clamp entre 0 et 255)
					r0			= _mm_packus_epi16( r0, zero );		// 00000000rrrrrrrr saturated
					g0			= _mm_packus_epi16( g0, zero );		// 00000000gggggggg saturated
					b0			= _mm_packus_epi16( b0, zero );		// 00000000bbbbbbbb saturated
					// Et que l'on ordonne
					r1			= _mm_unpacklo_epi8( zero, b0 );	// b0b0b0b0b0b0b0b0
					gb			= _mm_unpacklo_epi8( g0, r0 );		// rgrgrgrgrgrgrgrg
					argb0123	= _mm_unpacklo_epi16( r1, gb );		// rgb0rgb0rgb0rgb0
					argb4567	= _mm_unpackhi_epi16( r1, gb );		// rgb0rgb0rgb0rgb0
					// Enfin on fout tout ce bordel dans le buffer
					argb0123 = _mm_srli_si128( argb0123, 1 );
					std::memcpy( l_pBufferOut, &argb0123, 3 );
					l_pBufferOut += 3;
					argb0123 = _mm_srli_si128( argb0123, 4 );
					std::memcpy( l_pBufferOut, &argb0123, 3 );
					l_pBufferOut += 3;
					argb0123 = _mm_srli_si128( argb0123, 4 );
					std::memcpy( l_pBufferOut, &argb0123, 3 );
					l_pBufferOut += 3;
					argb0123 = _mm_srli_si128( argb0123, 4 );
					std::memcpy( l_pBufferOut, &argb0123, 3 );
					l_pBufferOut += 3;
					argb4567 = _mm_srli_si128( argb4567, 1 );
					std::memcpy( l_pBufferOut, &argb4567, 3 );
					l_pBufferOut += 3;
					argb4567 = _mm_srli_si128( argb4567, 4 );
					std::memcpy( l_pBufferOut, &argb4567, 3 );
					l_pBufferOut += 3;
					argb4567 = _mm_srli_si128( argb4567, 4 );
					std::memcpy( l_pBufferOut, &argb4567, 3 );
					l_pBufferOut += 3;
					argb4567 = _mm_srli_si128( argb4567, 4 );
					std::memcpy( l_pBufferOut, &argb4567, 3 );
					l_pBufferOut += 3;
				}
			}
		};

#endif

		/**
		 *\~english
		 *\brief			Conversion from static source format to dynamic destination format
		 *\remarks			Only for colour pixel formats, offsets source and destination by respective pixel format sizes
		 *\param[in,out]	p_pSrc			The source pixel
		 *\param[in,out]	p_pDst			The destination pixel
		 *\param[in]		p_ePixelFmtDst	The destination pixel format
		 *\~french
		 *\brief			Conversion d'une source statique vers une destination dynamique
		 *\remarks			Uniquement pour les formats de pixel couleurs, décale la source et la destination par la taille respective de leur format de pixel
		 *\param[in,out]	p_pSrc			Le pixel source
		 *\param[in,out]	p_pDst			Le pixel destination
		 *\param[in]		p_ePixelFmtDst	Le format du pixel destination
		 */
		template< TPL_PIXEL_FORMAT PF > void DynamicColourConversion( uint8_t const *& p_pSrc, uint8_t *& p_pDst, PixelFormat p_ePixelFmtDst )
		{
			switch ( p_ePixelFmtDst )
			{
			case PixelFormat::L8:
				pixel_converter< PF, PixelFormat::L8 >()( p_pSrc, p_pDst );
				break;

			case PixelFormat::L16F32F:
				pixel_converter< PF, PixelFormat::L16F32F >()( p_pSrc, p_pDst );
				break;

			case PixelFormat::L32F:
				pixel_converter< PF, PixelFormat::L32F >()( p_pSrc, p_pDst );
				break;

			case PixelFormat::A8L8:
				pixel_converter< PF, PixelFormat::A8L8 >()( p_pSrc, p_pDst );
				break;

			case PixelFormat::AL16F32F:
				pixel_converter< PF, PixelFormat::AL16F32F >()( p_pSrc, p_pDst );
				break;

			case PixelFormat::AL32F:
				pixel_converter< PF, PixelFormat::AL32F >()( p_pSrc, p_pDst );
				break;

			case PixelFormat::A1R5G5B5:
				pixel_converter< PF, PixelFormat::A1R5G5B5 >()( p_pSrc, p_pDst );
				break;

			case PixelFormat::A4R4G4B4:
				pixel_converter< PF, PixelFormat::A4R4G4B4 >()( p_pSrc, p_pDst );
				break;

			case PixelFormat::R5G6B5:
				pixel_converter< PF, PixelFormat::R5G6B5 >()( p_pSrc, p_pDst );
				break;

			case PixelFormat::R8G8B8:
				pixel_converter< PF, PixelFormat::R8G8B8 >()( p_pSrc, p_pDst );
				break;

			case PixelFormat::B8G8R8:
				pixel_converter< PF, PixelFormat::B8G8R8 >()( p_pSrc, p_pDst );
				break;

			case PixelFormat::A8R8G8B8:
				pixel_converter< PF, PixelFormat::A8R8G8B8 >()( p_pSrc, p_pDst );
				break;

			case PixelFormat::A8B8G8R8:
				pixel_converter< PF, PixelFormat::A8B8G8R8 >()( p_pSrc, p_pDst );
				break;

			case PixelFormat::RGB16F:
				pixel_converter< PF, PixelFormat::RGB16F >()( p_pSrc, p_pDst );
				break;

			case PixelFormat::RGBA16F:
				pixel_converter< PF, PixelFormat::RGBA16F >()( p_pSrc, p_pDst );
				break;

			case PixelFormat::RGB16F32F:
				pixel_converter< PF, PixelFormat::RGB16F32F >()( p_pSrc, p_pDst );
				break;

			case PixelFormat::RGBA16F32F:
				pixel_converter< PF, PixelFormat::RGBA16F32F >()( p_pSrc, p_pDst );
				break;

			case PixelFormat::RGB32F:
				pixel_converter< PF, PixelFormat::RGB32F >()( p_pSrc, p_pDst );
				break;

			case PixelFormat::RGBA32F:
				pixel_converter< PF, PixelFormat::RGBA32F >()( p_pSrc, p_pDst );
				break;

			case PixelFormat::YUY2:
				pixel_converter< PF, PixelFormat::YUY2 >()( p_pSrc, p_pDst );
				break;

			default:
				UNSUPPORTED_ERROR( "No conversion defined" );
				break;
			}
		}
		/**
		 *\~english
		 *\brief			Conversion from static source format to dynamic destination format
		 *\remarks			Only for depth pixel formats, offsets source and destination by respective pixel format sizes
		 *\param[in,out]	p_pSrc			The source pixel
		 *\param[in,out]	p_pDst			The destination pixel
		 *\param[in]		p_ePixelFmtDst	The destination pixel format
		 *\~french
		 *\brief			Conversion d'une source statique vers une destination dynamique
		 *\remarks			Uniquement pour les formats de pixel profondeurs, décale la source et la destination par la taille respective de leur format de pixel
		 *\param[in,out]	p_pSrc			Le pixel source
		 *\param[in,out]	p_pDst			Le pixel destination
		 *\param[in]		p_ePixelFmtDst	Le format du pixel destination
		 */
		template< TPL_PIXEL_FORMAT PF > void DynamicDepthConversion( uint8_t const *& p_pSrc, uint8_t *& p_pDst, PixelFormat p_ePixelFmtDst )
		{
			switch ( p_ePixelFmtDst )
			{
			case PixelFormat::D16:
				pixel_converter< PF, PixelFormat::D16 >()( p_pSrc, p_pDst );
				break;

			case PixelFormat::D24:
				pixel_converter< PF, PixelFormat::D24 >()( p_pSrc, p_pDst );
				break;

			case PixelFormat::D24S8:
				pixel_converter< PF, PixelFormat::D24S8 >()( p_pSrc, p_pDst );
				break;

			case PixelFormat::D32:
				pixel_converter< PF, PixelFormat::D32 >()( p_pSrc, p_pDst );
				break;

			case PixelFormat::D32F:
				pixel_converter< PF, PixelFormat::D32F >()( p_pSrc, p_pDst );
				break;

			default:
				UNSUPPORTED_ERROR( "No conversion defined" );
				break;
			}
		}
		/**
		 *\~english
		 *\brief			Conversion from static source format to dynamic destination format
		 *\remarks			Only for stencil pixel formats, offsets source and destination by respective pixel format sizes
		 *\param[in,out]	p_pSrc			The source pixel
		 *\param[in,out]	p_pDst			The destination pixel
		 *\param[in]		p_ePixelFmtDst	The destination pixel format
		 *\~french
		 *\brief			Conversion d'une source statique vers une destination dynamique
		 *\remarks			Uniquement pour les formats de pixel stencil, décale la source et la destination par la taille respective de leur format de pixel
		 *\param[in,out]	p_pSrc			Le pixel source
		 *\param[in,out]	p_pDst			Le pixel destination
		 *\param[in]		p_ePixelFmtDst	Le format du pixel destination
		 */
		template< TPL_PIXEL_FORMAT PF > void DynamicStencilConversion( uint8_t const *& p_pSrc, uint8_t *& p_pDst, PixelFormat p_ePixelFmtDst )
		{
			switch ( p_ePixelFmtDst )
			{
			case PixelFormat::D24S8:
				pixel_converter< PF, PixelFormat::D24S8 >()( p_pSrc, p_pDst );
				break;

			case PixelFormat::S1:
				pixel_converter< PF, PixelFormat::S1 >()( p_pSrc, p_pDst );
				break;

			case PixelFormat::S8:
				pixel_converter< PF, PixelFormat::S8 >()( p_pSrc, p_pDst );
				break;

			default:
				UNSUPPORTED_ERROR( "No conversion defined" );
				break;
			}
		}
		/**
		 *\~english
		 *\brief		Buffer conversion
		 *\param[in]	p_pSrcBuffer	The source buffer
		 *\param[in]	p_uiSrcSize		The source buffer size
		 *\param[in]	p_eDstFormat	The destination pixel format
		 *\param[in]	p_pDstBuffer	The destination buffer
		 *\param[in]	p_uiDstSize		The destination buffer size
		 *\~french
		 *\brief		Conversion de buffer
		 *\param[in]	p_pSrcBuffer	Le buffer source
		 *\param[in]	p_uiSrcSize		La taille du buffer source
		 *\param[in]	p_eDstFormat	Le format du buffer destination
		 *\param[in]	p_pDstBuffer	Le buffer destination
		 *\param[in]	p_uiDstSize		La taille du buffer destination
		 */
		template< TPL_PIXEL_FORMAT PF > void DynamicColourBufferConversion( uint8_t const * p_pSrcBuffer, uint32_t p_uiSrcSize, PixelFormat p_eDstFormat, uint8_t * p_pDstBuffer, uint32_t p_uiDstSize )
		{
			switch ( p_eDstFormat )
			{
			case PixelFormat::L8:
				buffer_converter< PF, PixelFormat::L8 >()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::L16F32F:
				buffer_converter< PF, PixelFormat::L16F32F >()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::L32F:
				buffer_converter< PF, PixelFormat::L32F >()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::A8L8:
				buffer_converter< PF, PixelFormat::A8L8 >()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::AL16F32F:
				buffer_converter< PF, PixelFormat::AL16F32F >()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::AL32F:
				buffer_converter< PF, PixelFormat::AL32F >()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::A1R5G5B5:
				buffer_converter< PF, PixelFormat::A1R5G5B5 >()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::A4R4G4B4:
				buffer_converter< PF, PixelFormat::A4R4G4B4 >()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::R5G6B5:
				buffer_converter< PF, PixelFormat::R5G6B5 >()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::R8G8B8:
				buffer_converter< PF, PixelFormat::R8G8B8 >()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::B8G8R8:
				buffer_converter< PF, PixelFormat::B8G8R8 >()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::A8R8G8B8:
				buffer_converter< PF, PixelFormat::A8R8G8B8 >()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::A8B8G8R8:
				buffer_converter< PF, PixelFormat::A8B8G8R8 >()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::RGB16F:
				buffer_converter< PF, PixelFormat::RGB16F >()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::RGBA16F:
				buffer_converter< PF, PixelFormat::RGBA16F >()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::RGB16F32F:
				buffer_converter< PF, PixelFormat::RGB16F32F >()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::RGBA16F32F:
				buffer_converter< PF, PixelFormat::RGBA16F32F >()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::RGB32F:
				buffer_converter< PF, PixelFormat::RGB32F >()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::RGBA32F:
				buffer_converter< PF, PixelFormat::RGBA32F >()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::YUY2:
				buffer_converter< PF, PixelFormat::YUY2 >()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );
				break;

			default:
				UNSUPPORTED_ERROR( "No conversion defined" );
				break;
			}
		}
		/**
		 *\~english
		 *\brief		Buffer conversion
		 *\param[in]	p_pSrcBuffer	The source buffer
		 *\param[in]	p_uiSrcSize		The source buffer size
		 *\param[in]	p_eDstFormat	The destination pixel format
		 *\param[in]	p_pDstBuffer	The destination buffer
		 *\param[in]	p_uiDstSize		The destination buffer size
		 *\~french
		 *\brief		Conversion de buffer
		 *\param[in]	p_pSrcBuffer	Le buffer source
		 *\param[in]	p_uiSrcSize		La taille du buffer source
		 *\param[in]	p_eDstFormat	Le format du buffer destination
		 *\param[in]	p_pDstBuffer	Le buffer destination
		 *\param[in]	p_uiDstSize		La taille du buffer destination
		 */
		template< TPL_PIXEL_FORMAT PF > void DynamicDepthBufferConversion( uint8_t const * p_pSrcBuffer, uint32_t p_uiSrcSize, PixelFormat p_eDstFormat, uint8_t * p_pDstBuffer, uint32_t p_uiDstSize )
		{
			switch ( p_eDstFormat )
			{
			case PixelFormat::D16:
				buffer_converter< PF, PixelFormat::D16 >()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::D24:
				buffer_converter< PF, PixelFormat::D24 >()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::D24S8:
				buffer_converter< PF, PixelFormat::D24S8 >()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::D32:
				buffer_converter< PF, PixelFormat::D32 >()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::D32F:
				buffer_converter< PF, PixelFormat::D32F >()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::A8R8G8B8:
				buffer_converter< PF, PixelFormat::A8R8G8B8 >()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::L8:
				buffer_converter< PF, PixelFormat::L8 >()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );
				break;

			default:
				UNSUPPORTED_ERROR( "No conversion defined" );
				break;
			}
		}
		/**
		 *\~english
		 *\brief		Buffer conversion
		 *\param[in]	p_pSrcBuffer	The source buffer
		 *\param[in]	p_uiSrcSize		The source buffer size
		 *\param[in]	p_eDstFormat	The destination pixel format
		 *\param[in]	p_pDstBuffer	The destination buffer
		 *\param[in]	p_uiDstSize		The destination buffer size
		 *\~french
		 *\brief		Conversion de buffer
		 *\param[in]	p_pSrcBuffer	Le buffer source
		 *\param[in]	p_uiSrcSize		La taille du buffer source
		 *\param[in]	p_eDstFormat	Le format du buffer destination
		 *\param[in]	p_pDstBuffer	Le buffer destination
		 *\param[in]	p_uiDstSize		La taille du buffer destination
		 */
		template< TPL_PIXEL_FORMAT PF > void DynamicStencilBufferConversion( uint8_t const * p_pSrcBuffer, uint32_t p_uiSrcSize, PixelFormat p_eDstFormat, uint8_t * p_pDstBuffer, uint32_t p_uiDstSize )
		{
			switch ( p_eDstFormat )
			{
			case PixelFormat::D24S8:
				buffer_converter< PF, PixelFormat::D24S8 >()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::S1:
				buffer_converter< PF, PixelFormat::S1 >()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );
				break;

			case PixelFormat::S8:
				buffer_converter< PF, PixelFormat::S8 >()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );
				break;

			default:
				UNSUPPORTED_ERROR( "No conversion defined" );
				break;
			}
		}
	}

	//*************************************************************************************************

	namespace PF
	{
		namespace
		{
			template< TPL_PIXEL_FORMAT PF, typename Enable = void > struct PixelComponent;

			template< TPL_PIXEL_FORMAT PF >
			struct PixelComponent< PF, typename std::enable_if< is_colour_format< PF >::value >::type >
			{
				static float GetFloat( Pixel< PF > const & p_pixel, ePIXEL_COMPONENT p_component )
				{
					float l_return = 0.0f;

					switch ( p_component )
					{
					case ePIXEL_COMPONENT_RED:
						l_return = component< PF >::R32F( p_pixel.const_ptr() );
						break;

					case ePIXEL_COMPONENT_GREEN:
						l_return = component< PF >::G32F( p_pixel.const_ptr() );
						break;

					case ePIXEL_COMPONENT_BLUE:
						l_return = component< PF >::B32F( p_pixel.const_ptr() );
						break;

					case ePIXEL_COMPONENT_ALPHA:
						l_return = component< PF >::A32F( p_pixel.const_ptr() );
						break;

					case ePIXEL_COMPONENT_LUMINANCE:
						l_return = component< PF >::L32F( p_pixel.const_ptr() );
						break;

					default:
						l_return = 0;
						break;
					}

					return l_return;
				}

				static void SetFloat( Pixel< PF > & p_pixel, ePIXEL_COMPONENT p_component, float p_value )
				{
					switch ( p_component )
					{
					case ePIXEL_COMPONENT_RED:
						component< PF >::R8( p_pixel.ptr(), p_value );
						break;

					case ePIXEL_COMPONENT_GREEN:
						component< PF >::G8( p_pixel.ptr(), p_value );
						break;

					case ePIXEL_COMPONENT_BLUE:
						component< PF >::B8( p_pixel.ptr(), p_value );
						break;

					case ePIXEL_COMPONENT_ALPHA:
						component< PF >::A8( p_pixel.ptr(), p_value );
						break;

					case ePIXEL_COMPONENT_LUMINANCE:
						component< PF >::L8( p_pixel.ptr(), p_value );
						break;

					default:
						break;
					}
				}

				static uint8_t GetByte( Pixel< PF > const & p_pixel, ePIXEL_COMPONENT p_component )
				{
					uint8_t l_return = 0;

					switch ( p_component )
					{
					case ePIXEL_COMPONENT_RED:
						l_return = component< PF >::R8( p_pixel.const_ptr() );
						break;

					case ePIXEL_COMPONENT_GREEN:
						l_return = component< PF >::G8( p_pixel.const_ptr() );
						break;

					case ePIXEL_COMPONENT_BLUE:
						l_return = component< PF >::B8( p_pixel.const_ptr() );
						break;

					case ePIXEL_COMPONENT_ALPHA:
						l_return = component< PF >::A8( p_pixel.const_ptr() );
						break;

					case ePIXEL_COMPONENT_LUMINANCE:
						l_return = component< PF >::L8( p_pixel.const_ptr() );
						break;

					default:
						l_return = 0;
						break;
					}

					return l_return;
				}

				static void SetByte( Pixel< PF > & p_pixel, ePIXEL_COMPONENT p_component, uint8_t p_value )
				{
					switch ( p_component )
					{
					case ePIXEL_COMPONENT_RED:
						component< PF >::R8( p_pixel.ptr(), p_value );
						break;

					case ePIXEL_COMPONENT_GREEN:
						component< PF >::G8( p_pixel.ptr(), p_value );
						break;

					case ePIXEL_COMPONENT_BLUE:
						component< PF >::B8( p_pixel.ptr(), p_value );
						break;

					case ePIXEL_COMPONENT_ALPHA:
						component< PF >::A8( p_pixel.ptr(), p_value );
						break;

					case ePIXEL_COMPONENT_LUMINANCE:
						component< PF >::L8( p_pixel.ptr(), p_value );
						break;

					default:
						break;
					}
				}
			};

			template< TPL_PIXEL_FORMAT PF >
			struct PixelComponent< PF, typename std::enable_if< is_depth_stencil_format< PF >::value >::type >
			{
				static float GetFloat( Pixel< PF > const & p_pixel, ePIXEL_COMPONENT p_component )
				{
					float l_return = 0.0f;

					switch ( p_component )
					{
					case ePIXEL_COMPONENT_DEPTH:
						l_return = float( component< PF >::D32F( p_pixel.const_ptr() ) );
						break;

					case ePIXEL_COMPONENT_STENCIL:
						l_return = float( component< PF >::S32F( p_pixel.const_ptr() ) );
						break;

					default:
						l_return = 0;
						break;
					}

					return l_return;
				}

				static void SetFloat( Pixel< PF > & p_pixel, ePIXEL_COMPONENT p_component, float p_value )
				{
					switch ( p_component )
					{
					case ePIXEL_COMPONENT_DEPTH:
						component< PF >::D32F( p_pixel.ptr(), p_value );
						break;

					case ePIXEL_COMPONENT_STENCIL:
						component< PF >::S32F( p_pixel.ptr(), p_value );
						break;

					default:
						break;
					}
				}

				static uint8_t GetByte( Pixel< PF > const & p_pixel, ePIXEL_COMPONENT p_component )
				{
					uint8_t l_return = 0;

					switch ( p_component )
					{
					case ePIXEL_COMPONENT_DEPTH:
						l_return = uint8_t( component< PF >::D32( p_pixel.const_ptr() ) >> 24 );
						break;

					case ePIXEL_COMPONENT_STENCIL:
						l_return = uint8_t( component< PF >::S8( p_pixel.const_ptr() ) );
						break;

					default:
						l_return = 0;
						break;
					}

					return l_return;
				}

				static void SetByte( Pixel< PF > & p_pixel, ePIXEL_COMPONENT p_component, uint8_t p_value )
				{
					switch ( p_component )
					{
					case ePIXEL_COMPONENT_DEPTH:
						component< PF >::D32( p_pixel.ptr(), uint32_t( p_value ) << 24 );
						break;

					case ePIXEL_COMPONENT_STENCIL:
						component< PF >::S8( p_pixel.ptr(), p_value );
						break;

					default:
						break;
					}
				}

				static uint16_t GetUInt16( Pixel< PF > const & p_pixel, ePIXEL_COMPONENT p_component )
				{
					uint32_t l_return = 0;

					switch ( p_component )
					{
					case ePIXEL_COMPONENT_DEPTH:
						l_return = component< PF >::D16( p_pixel.const_ptr() );
						break;

					default:
						l_return = 0;
						break;
					}

					return l_return;
				}

				static void SetUInt16( Pixel< PF > & p_pixel, ePIXEL_COMPONENT p_component, uint16_t p_value )
				{
					switch ( p_component )
					{
					case ePIXEL_COMPONENT_DEPTH:
						component< PF >::D16( p_pixel.ptr(), p_value );
						break;

					default:
						break;
					}
				}

				static uint32_t GetUInt24( Pixel< PF > const & p_pixel, ePIXEL_COMPONENT p_component )
				{
					uint32_t l_return = 0;

					switch ( p_component )
					{
					case ePIXEL_COMPONENT_DEPTH:
						l_return = component< PF >::D24( p_pixel.const_ptr() );
						break;

					default:
						l_return = 0;
						break;
					}

					return l_return;
				}

				static void SetUInt24( Pixel< PF > & p_pixel, ePIXEL_COMPONENT p_component, uint32_t p_value )
				{
					switch ( p_component )
					{
					case ePIXEL_COMPONENT_DEPTH:
						component< PF >::D24( p_pixel.ptr(), p_value );
						break;

					default:
						break;
					}
				}

				static uint32_t GetUInt32( Pixel< PF > const & p_pixel, ePIXEL_COMPONENT p_component )
				{
					uint32_t l_return = 0;

					switch ( p_component )
					{
					case ePIXEL_COMPONENT_DEPTH:
						l_return = component< PF >::D32( p_pixel.const_ptr() );
						break;

					default:
						l_return = 0;
						break;
					}

					return l_return;
				}

				static void SetUInt32( Pixel< PF > & p_pixel, ePIXEL_COMPONENT p_component, uint32_t p_value )
				{
					switch ( p_component )
					{
					case ePIXEL_COMPONENT_DEPTH:
						component< PF >::D32( p_pixel.ptr(), p_value );
						break;

					default:
						break;
					}
				}
			};
		}

		template< TPL_PIXEL_FORMAT PF >
		float GetFloatComponent( Pixel< PF > const & p_pixel, ePIXEL_COMPONENT p_component )
		{
			return PixelComponent< PF >::GetFloat( p_pixel, p_component );
		}

		template< TPL_PIXEL_FORMAT PF >
		void SetFloatComponent( Pixel< PF > & p_pixel, ePIXEL_COMPONENT p_component, float p_value )
		{
			return PixelComponent< PF >::SetFloat( p_pixel, p_component, p_value );
		}

		template< TPL_PIXEL_FORMAT PF >
		uint8_t GetByteComponent( Pixel< PF > const & p_pixel, ePIXEL_COMPONENT p_component )
		{
			return PixelComponent< PF >::GetByte( p_pixel, p_component );
		}

		template< TPL_PIXEL_FORMAT PF >
		void SetByteComponent( Pixel< PF > & p_pixel, ePIXEL_COMPONENT p_component, uint8_t p_value )
		{
			return PixelComponent< PF >::SetByte( p_pixel, p_component, p_value );
		}

		template< TPL_PIXEL_FORMAT PF >
		uint16_t GetUInt16Component( Pixel< PF > const & p_pixel, ePIXEL_COMPONENT p_component )
		{
			return PixelComponent< PF >::GetUInt16( p_pixel, p_component );
		}

		template< TPL_PIXEL_FORMAT PF >
		void SetUInt16Component( Pixel< PF > & p_pixel, ePIXEL_COMPONENT p_component, uint16_t p_value )
		{
			return PixelComponent< PF >::SetUInt16( p_pixel, p_component, p_value );
		}

		template< TPL_PIXEL_FORMAT PF >
		uint32_t GetUInt24Component( Pixel< PF > const & p_pixel, ePIXEL_COMPONENT p_component )
		{
			return PixelComponent< PF >::GetUInt24( p_pixel, p_component );
		}

		template< TPL_PIXEL_FORMAT PF >
		void SetUInt24Component( Pixel< PF > & p_pixel, ePIXEL_COMPONENT p_component, uint32_t p_value )
		{
			return PixelComponent< PF >::SetUInt24( p_pixel, p_component, p_value );
		}

		template< TPL_PIXEL_FORMAT PF >
		uint32_t GetUInt32Component( Pixel< PF > const & p_pixel, ePIXEL_COMPONENT p_component )
		{
			return PixelComponent< PF >::GetUInt32( p_pixel, p_component );
		}

		template< TPL_PIXEL_FORMAT PF >
		void SetUInt32Component( Pixel< PF > & p_pixel, ePIXEL_COMPONENT p_component, uint32_t p_value )
		{
			return PixelComponent< PF >::SetUInt32( p_pixel, p_component, p_value );
		}
	}

	//*************************************************************************************************

	//!\~english Specialisation for PixelFormat::L8	\~french Spécialisation pour PixelFormat::L8
	template <> struct pixel_definitions< PixelFormat::L8 >
	{
		static const uint8_t Size = 1;
		static const bool Alpha = false;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static inline String to_string()
		{
			return cuT( "8 bits luminosity" );
		}
		static inline String to_str()
		{
			return cuT( "l8" );
		}
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, PixelFormat p_ePixelFmtDst )
		{
			detail::DynamicColourConversion< PixelFormat::L8 >( p_pSrc, p_pDst, p_ePixelFmtDst );
		}
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, PixelFormat p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize )
		{
			detail::DynamicColourBufferConversion< PixelFormat::L8 >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
		}
		template< TPL_PIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
		{
			detail::pixel_converter< PixelFormat::L8, PF >()( p_pSrc, p_pDst );
		}
	};

	//*************************************************************************************************

	//!\~english Specialisation for PixelFormat::L16F32F	\~french Spécialisation pour PixelFormat::L16F32F
	template <> struct pixel_definitions< PixelFormat::L16F32F >
	{
		static const uint8_t Size = 4;
		static const bool Alpha = false;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static inline String to_string()
		{
			return cuT( "Half floats luminosity" );
		}
		static inline String to_str()
		{
			return cuT( "l16f" );
		}
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, PixelFormat p_ePixelFmtDst )
		{
			detail::DynamicColourConversion< PixelFormat::L16F32F >( p_pSrc, p_pDst, p_ePixelFmtDst );
		}
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, PixelFormat p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize )
		{
			detail::DynamicColourBufferConversion< PixelFormat::L16F32F >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
		}
		template< TPL_PIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
		{
			detail::pixel_converter< PixelFormat::L16F32F, PF >()( p_pSrc, p_pDst );
		}
	};

	//*************************************************************************************************

	//!\~english Specialisation for PixelFormat::L32F	\~french Spécialisation pour PixelFormat::L32F
	template <> struct pixel_definitions< PixelFormat::L32F >
	{
		static const uint8_t Size = 4;
		static const bool Alpha = false;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static inline String to_string()
		{
			return cuT( "Floats luminosity" );
		}
		static inline String to_str()
		{
			return cuT( "l32f" );
		}
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, PixelFormat p_ePixelFmtDst )
		{
			detail::DynamicColourConversion< PixelFormat::L32F >( p_pSrc, p_pDst, p_ePixelFmtDst );
		}
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, PixelFormat p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize )
		{
			detail::DynamicColourBufferConversion< PixelFormat::L32F >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
		}
		template< TPL_PIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
		{
			detail::pixel_converter< PixelFormat::L32F, PF >()( p_pSrc, p_pDst );
		}
	};

	//*************************************************************************************************

	//!\~english Specialisation for PixelFormat::A8L8	\~french Spécialisation pour PixelFormat::A8L8
	template <> struct pixel_definitions< PixelFormat::A8L8 >
	{
		static const uint8_t Size = 2;
		static const bool Alpha = true;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static const PixelFormat NoAlphaPF = PixelFormat::L8;
		static inline String to_string()
		{
			return cuT( "16 bits luminosity and alpha" );
		}
		static inline String to_str()
		{
			return cuT( "al16" );
		}
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, PixelFormat p_ePixelFmtDst )
		{
			detail::DynamicColourConversion< PixelFormat::A8L8 >( p_pSrc, p_pDst, p_ePixelFmtDst );
		}
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, PixelFormat p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize )
		{
			detail::DynamicColourBufferConversion< PixelFormat::A8L8 >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
		}
		template< TPL_PIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
		{
			detail::pixel_converter< PixelFormat::A8L8, PF >()( p_pSrc, p_pDst );
		}
	};

	//*************************************************************************************************

	//!\~english Specialisation for PixelFormat::AL32F	\~french Spécialisation pour PixelFormat::AL32F
	template <> struct pixel_definitions< PixelFormat::AL32F >
	{
		static const uint8_t Size = 8;
		static const bool Alpha = true;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static const PixelFormat NoAlphaPF = PixelFormat::L32F;
		static inline String to_string()
		{
			return cuT( "Floats luminosity and alpha" );
		}
		static inline String to_str()
		{
			return cuT( "al32f" );
		}
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, PixelFormat p_ePixelFmtDst )
		{
			detail::DynamicColourConversion< PixelFormat::AL32F >( p_pSrc, p_pDst, p_ePixelFmtDst );
		}
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, PixelFormat p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize )
		{
			detail::DynamicColourBufferConversion< PixelFormat::AL32F >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
		}
		template< TPL_PIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
		{
			detail::pixel_converter< PixelFormat::AL32F, PF >()( p_pSrc, p_pDst );
		}
	};

	//*************************************************************************************************

	//!\~english Specialisation for PixelFormat::AL16F32F	\~french Spécialisation pour PixelFormat::AL16F32F
	template <> struct pixel_definitions< PixelFormat::AL16F32F >
	{
		static const uint8_t Size = 8;
		static const bool Alpha = true;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static const PixelFormat NoAlphaPF = PixelFormat::L16F32F;
		static inline String to_string()
		{
			return cuT( "Half floats luminosity and alpha" );
		}
		static inline String to_str()
		{
			return cuT( "al16f" );
		}
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, PixelFormat p_ePixelFmtDst )
		{
			detail::DynamicColourConversion< PixelFormat::AL16F32F >( p_pSrc, p_pDst, p_ePixelFmtDst );
		}
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, PixelFormat p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize )
		{
			detail::DynamicColourBufferConversion< PixelFormat::AL16F32F >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
		}
		template< TPL_PIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
		{
			detail::pixel_converter< PixelFormat::AL16F32F, PF >()( p_pSrc, p_pDst );
		}
	};

	//*************************************************************************************************

	//!\~english Specialisation for PixelFormat::A1R5G5B5	\~french Spécialisation pour PixelFormat::A1R5G5B5
	template <> struct pixel_definitions< PixelFormat::A1R5G5B5 >
	{
		static const uint8_t Size = 2;
		static const bool Alpha = true;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static const PixelFormat NoAlphaPF = PixelFormat::R5G6B5;
		static inline String to_string()
		{
			return cuT( "16 bits 5551 ARGB" );
		}
		static inline String to_str()
		{
			return cuT( "argb1555" );
		}
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, PixelFormat p_ePixelFmtDst )
		{
			detail::DynamicColourConversion< PixelFormat::A1R5G5B5 >( p_pSrc, p_pDst, p_ePixelFmtDst );
		}
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, PixelFormat p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize )
		{
			detail::DynamicColourBufferConversion< PixelFormat::A1R5G5B5 >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
		}
		template< TPL_PIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
		{
			detail::pixel_converter< PixelFormat::A1R5G5B5, PF >()( p_pSrc, p_pDst );
		}
	};

	//*************************************************************************************************

	//!\~english Specialisation for PixelFormat::R5G6B5	\~french Spécialisation pour PixelFormat::R5G6B5
	template <> struct pixel_definitions< PixelFormat::R5G6B5 >
	{
		static const uint8_t Size = 2;
		static const bool Alpha = false;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static inline String to_string()
		{
			return cuT( "16 bits 565 RGB" );
		}
		static inline String to_str()
		{
			return cuT( "rgb565" );
		}
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, PixelFormat p_ePixelFmtDst )
		{
			detail::DynamicColourConversion< PixelFormat::R5G6B5 >( p_pSrc, p_pDst, p_ePixelFmtDst );
		}
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, PixelFormat p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize )
		{
			detail::DynamicColourBufferConversion< PixelFormat::R5G6B5 >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
		}
		template< TPL_PIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
		{
			detail::pixel_converter< PixelFormat::R5G6B5, PF >()( p_pSrc, p_pDst );
		}
	};

	//*************************************************************************************************

	//!\~english Specialisation for PixelFormat::A4R4G4B4	\~french Spécialisation pour PixelFormat::A4R4G4B4
	template <> struct pixel_definitions< PixelFormat::A4R4G4B4 >
	{
		static const uint8_t Size = 2;
		static const bool Alpha = true;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static const PixelFormat NoAlphaPF = PixelFormat::R5G6B5;
		static inline String to_string()
		{
			return cuT( "16 bits 4444 ARGB" );
		}
		static inline String to_str()
		{
			return cuT( "argb16" );
		}
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, PixelFormat p_ePixelFmtDst )
		{
			detail::DynamicColourConversion< PixelFormat::A4R4G4B4 >( p_pSrc, p_pDst, p_ePixelFmtDst );
		}
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, PixelFormat p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize )
		{
			detail::DynamicColourBufferConversion< PixelFormat::A4R4G4B4 >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
		}
		template< TPL_PIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
		{
			detail::pixel_converter< PixelFormat::A4R4G4B4, PF >()( p_pSrc, p_pDst );
		}
	};

	//*************************************************************************************************

	//!\~english Specialisation for PixelFormat::R8G8B8	\~french Spécialisation pour PixelFormat::R8G8B8
	template <> struct pixel_definitions< PixelFormat::R8G8B8 >
	{
		static const uint8_t Size = 3;
		static const bool Alpha = false;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static inline String to_string()
		{
			return cuT( "24 bits 888 RGB" );
		}
		static inline String to_str()
		{
			return cuT( "rgb24" );
		}
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, PixelFormat p_ePixelFmtDst )
		{
			detail::DynamicColourConversion< PixelFormat::R8G8B8 >( p_pSrc, p_pDst, p_ePixelFmtDst );
		}
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, PixelFormat p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize )
		{
			detail::DynamicColourBufferConversion< PixelFormat::R8G8B8 >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
		}
		template< TPL_PIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
		{
			detail::pixel_converter< PixelFormat::R8G8B8, PF >()( p_pSrc, p_pDst );
		}
	};

	//*************************************************************************************************

	//!\~english Specialisation for PixelFormat::B8G8R8	\~french Spécialisation pour PixelFormat::B8G8R8
	template <> struct pixel_definitions< PixelFormat::B8G8R8 >
	{
		static const uint8_t Size = 3;
		static const bool Alpha = false;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static inline String to_string()
		{
			return cuT( "24 bits 888 BGR" );
		}
		static inline String to_str()
		{
			return cuT( "bgr24" );
		}
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, PixelFormat p_ePixelFmtDst )
		{
			detail::DynamicColourConversion< PixelFormat::B8G8R8 >( p_pSrc, p_pDst, p_ePixelFmtDst );
		}
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, PixelFormat p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize )
		{
			detail::DynamicColourBufferConversion< PixelFormat::B8G8R8 >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
		}
		template< TPL_PIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
		{
			detail::pixel_converter< PixelFormat::B8G8R8, PF >()( p_pSrc, p_pDst );
		}
	};

	//*************************************************************************************************

	//!\~english Specialisation for PixelFormat::A8R8G8B8	\~french Spécialisation pour PixelFormat::A8R8G8B8
	template <> struct pixel_definitions< PixelFormat::A8R8G8B8 >
	{
		static const uint8_t Size = 4;
		static const bool Alpha = true;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static const PixelFormat NoAlphaPF = PixelFormat::R8G8B8;
		static inline String to_string()
		{
			return cuT( "32 bits 8888 ARGB" );
		}
		static inline String to_str()
		{
			return cuT( "argb32" );
		}
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, PixelFormat p_ePixelFmtDst )
		{
			detail::DynamicColourConversion< PixelFormat::A8R8G8B8 >( p_pSrc, p_pDst, p_ePixelFmtDst );
		}
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, PixelFormat p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize )
		{
			detail::DynamicColourBufferConversion< PixelFormat::A8R8G8B8 >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
		}
		template< TPL_PIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
		{
			detail::pixel_converter< PixelFormat::A8R8G8B8, PF >()( p_pSrc, p_pDst );
		}
	};

	//*************************************************************************************************

	//!\~english Specialisation for PixelFormat::A8B8G8R8	\~french Spécialisation pour PixelFormat::A8B8G8R8
	template <> struct pixel_definitions< PixelFormat::A8B8G8R8 >
	{
		static const uint8_t Size = 4;
		static const bool Alpha = true;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static const PixelFormat NoAlphaPF = PixelFormat::B8G8R8;
		static inline String to_string()
		{
			return cuT( "32 bits 8888 ABGR" );
		}
		static inline String to_str()
		{
			return cuT( "abgr32" );
		}
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, PixelFormat p_ePixelFmtDst )
		{
			detail::DynamicColourConversion< PixelFormat::A8B8G8R8 >( p_pSrc, p_pDst, p_ePixelFmtDst );
		}
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, PixelFormat p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize )
		{
			detail::DynamicColourBufferConversion< PixelFormat::A8B8G8R8 >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
		}
		template< TPL_PIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
		{
			detail::pixel_converter< PixelFormat::A8B8G8R8, PF >()( p_pSrc, p_pDst );
		}
	};

	//*************************************************************************************************

	//!\~english Specialisation for PixelFormat::RGB16F	\~french Spécialisation pour PixelFormat::RGB16F
	template <> struct pixel_definitions< PixelFormat::RGB16F >
	{
		static const uint8_t Size = 6;
		static const bool Alpha = false;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static inline String to_string()
		{
			return cuT( "Half floating point RGB" );
		}
		static inline String to_str()
		{
			return cuT( "rgb16f" );
		}
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, PixelFormat p_ePixelFmtDst )
		{
			detail::DynamicColourConversion< PixelFormat::RGB16F >( p_pSrc, p_pDst, p_ePixelFmtDst );
		}
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, PixelFormat p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize )
		{
			detail::DynamicColourBufferConversion< PixelFormat::RGB16F >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
		}
		template< TPL_PIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
		{
			detail::pixel_converter< PixelFormat::RGB16F, PF >()( p_pSrc, p_pDst );
		}
	};

	//*************************************************************************************************

	//!\~english Specialisation for PixelFormat::RGBA16F	\~french Spécialisation pour PixelFormat::RGBA16F
	template <> struct pixel_definitions< PixelFormat::RGBA16F >
	{
		static const uint8_t Size = 8;
		static const bool Alpha = true;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static const PixelFormat NoAlphaPF = PixelFormat::RGB16F;
		static inline String to_string()
		{
			return cuT( "Half floating point ARGB" );
		}
		static inline String to_str()
		{
			return cuT( "argb16f" );
		}
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, PixelFormat p_ePixelFmtDst )
		{
			detail::DynamicColourConversion< PixelFormat::RGBA16F >( p_pSrc, p_pDst, p_ePixelFmtDst );
		}
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, PixelFormat p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize )
		{
			detail::DynamicColourBufferConversion< PixelFormat::RGBA16F >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
		}
		template< TPL_PIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
		{
			detail::pixel_converter< PixelFormat::RGBA16F, PF >()( p_pSrc, p_pDst );
		}
	};

	//*************************************************************************************************

	//!\~english Specialisation for PixelFormat::RGB16F32F	\~french Spécialisation pour PixelFormat::RGB16F32F
	template <> struct pixel_definitions< PixelFormat::RGB16F32F >
	{
		static const uint8_t Size = 12;
		static const bool Alpha = false;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static inline String to_string()
		{
			return cuT( "GPU half floating point RGB, CPU floating point RGB" );
		}
		static inline String to_str()
		{
			return cuT( "rgb16f32f" );
		}
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, PixelFormat p_ePixelFmtDst )
		{
			detail::DynamicColourConversion< PixelFormat::RGB32F >( p_pSrc, p_pDst, p_ePixelFmtDst );
		}
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, PixelFormat p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize )
		{
			detail::DynamicColourBufferConversion< PixelFormat::RGB32F >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
		}
		template< TPL_PIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
		{
			detail::pixel_converter< PixelFormat::RGB32F, PF >()( p_pSrc, p_pDst );
		}
	};

	//*************************************************************************************************

	//!\~english Specialisation for PixelFormat::RGBA16F32F	\~french Spécialisation pour PixelFormat::RGBA16F32F
	template <> struct pixel_definitions< PixelFormat::RGBA16F32F >
	{
		static const uint8_t Size = 16;
		static const bool Alpha = true;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static const PixelFormat NoAlphaPF = PixelFormat::RGB16F32F;
		static inline String to_string()
		{
			return cuT( "GPU half floating point ARGB, CPU floating point ARGB" );
		}
		static inline String to_str()
		{
			return cuT( "argb16f32f" );
		}
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, PixelFormat p_ePixelFmtDst )
		{
			detail::DynamicColourConversion< PixelFormat::RGBA32F >( p_pSrc, p_pDst, p_ePixelFmtDst );
		}
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, PixelFormat p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize )
		{
			detail::DynamicColourBufferConversion< PixelFormat::RGBA32F >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
		}
		template< TPL_PIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
		{
			detail::pixel_converter< PixelFormat::RGBA32F, PF >()( p_pSrc, p_pDst );
		}
	};

	//*************************************************************************************************

	//!\~english Specialisation for PixelFormat::RGB32F	\~french Spécialisation pour PixelFormat::RGB32F
	template <> struct pixel_definitions< PixelFormat::RGB32F >
	{
		static const uint8_t Size = 12;
		static const bool Alpha = false;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static inline String to_string()
		{
			return cuT( "Floating point RGB" );
		}
		static inline String to_str()
		{
			return cuT( "rgb32f" );
		}
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, PixelFormat p_ePixelFmtDst )
		{
			detail::DynamicColourConversion< PixelFormat::RGB32F >( p_pSrc, p_pDst, p_ePixelFmtDst );
		}
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, PixelFormat p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize )
		{
			detail::DynamicColourBufferConversion< PixelFormat::RGB32F >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
		}
		template< TPL_PIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
		{
			detail::pixel_converter< PixelFormat::RGB32F, PF >()( p_pSrc, p_pDst );
		}
	};

	//*************************************************************************************************

	//!\~english Specialisation for PixelFormat::RGBA32F	\~french Spécialisation pour PixelFormat::RGBA32F
	template <> struct pixel_definitions< PixelFormat::RGBA32F >
	{
		static const uint8_t Size = 16;
		static const bool Alpha = true;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static const PixelFormat NoAlphaPF = PixelFormat::RGB32F;
		static inline String to_string()
		{
			return cuT( "Floating point ARGB" );
		}
		static inline String to_str()
		{
			return cuT( "argb32f" );
		}
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, PixelFormat p_ePixelFmtDst )
		{
			detail::DynamicColourConversion< PixelFormat::RGBA32F >( p_pSrc, p_pDst, p_ePixelFmtDst );
		}
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, PixelFormat p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize )
		{
			detail::DynamicColourBufferConversion< PixelFormat::RGBA32F >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
		}
		template< TPL_PIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
		{
			detail::pixel_converter< PixelFormat::RGBA32F, PF >()( p_pSrc, p_pDst );
		}
	};

	//*************************************************************************************************

	//!\~english Specialisation for PixelFormat::DXTC1	\~french Spécialisation pour PixelFormat::DXTC1
	template <> struct pixel_definitions< PixelFormat::DXTC1 >
	{
		static const uint8_t Size = 1;
		static const bool Alpha = false;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = true;
		static inline String to_string()
		{
			return cuT( "DXT1 8 bits compressed format" );
		}
		static inline String to_str()
		{
			return cuT( "dxtc1" );
		}
		static inline void convert( uint8_t const *, uint8_t *, PixelFormat )
		{
			UNSUPPORTED_ERROR( "No conversion from " + string::string_cast< char >( to_string() ) + " defined" );
		}
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, PixelFormat p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize )
		{
			UNSUPPORTED_ERROR( "No conversion from " + string::string_cast< char >( to_string() ) + " defined" );
		}
		template< TPL_PIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
		{
			detail::pixel_converter< PixelFormat::DXTC1, PF >()( p_pSrc, p_pDst );
		}
	};

	//*************************************************************************************************

	//!\~english Specialisation for PixelFormat::DXTC3	\~french Spécialisation pour PixelFormat::DXTC3
	template <> struct pixel_definitions< PixelFormat::DXTC3 >
	{
		static const uint8_t Size = 2;
		static const bool Alpha = true;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = true;
		static const PixelFormat NoAlphaPF = PixelFormat::DXTC3;
		static inline String to_string()
		{
			return cuT( "DXT3 16 bits compressed format" );
		}
		static inline String to_str()
		{
			return cuT( "dxtc3" );
		}
		static inline void convert( uint8_t const *, uint8_t *, PixelFormat )
		{
			UNSUPPORTED_ERROR( "No conversion from " + string::string_cast< char >( to_string() ) + " defined" );
		}
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, PixelFormat p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize )
		{
			UNSUPPORTED_ERROR( "No conversion from " + string::string_cast< char >( to_string() ) + " defined" );
		}
		template< TPL_PIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
		{
			detail::pixel_converter< PixelFormat::DXTC3, PF >()( p_pSrc, p_pDst );
		}
	};

	//*************************************************************************************************

	//!\~english Specialisation for PixelFormat::DXTC5	\~french Spécialisation pour PixelFormat::DXTC5
	template <> struct pixel_definitions< PixelFormat::DXTC5 >
	{
		static const uint8_t Size = 2;
		static const bool Alpha = true;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = true;
		static const PixelFormat NoAlphaPF = PixelFormat::DXTC5;
		static inline String to_string()
		{
			return cuT( "DXT5 16 bits compressed format" );
		}
		static inline String to_str()
		{
			return cuT( "dxtc5" );
		}
		static inline void convert( uint8_t const *, uint8_t *, PixelFormat )
		{
			UNSUPPORTED_ERROR( "No conversion from " + string::string_cast< char >( to_string() ) + " defined" );
		}
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, PixelFormat p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize )
		{
			UNSUPPORTED_ERROR( "No conversion from " + string::string_cast< char >( to_string() ) + " defined" );
		}
		template< TPL_PIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
		{
			detail::pixel_converter< PixelFormat::DXTC5, PF >()( p_pSrc, p_pDst );
		}
	};

	//*************************************************************************************************

	//!\~english Specialisation for PixelFormat::YUY2	\~french Spécialisation pour PixelFormat::YUY2
	template <> struct pixel_definitions< PixelFormat::YUY2 >
	{
		static const uint8_t Size = 2;
		static const bool Alpha = false;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = true;
		static inline String to_string()
		{
			return cuT( "YUY2(4:2:2) 16 bits compressed format" );
		}
		static inline String to_str()
		{
			return cuT( "yuy2" );
		}
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, PixelFormat p_ePixelFmtDst )
		{
			detail::DynamicColourConversion< PixelFormat::YUY2 >( p_pSrc, p_pDst, p_ePixelFmtDst );
		}
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, PixelFormat p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize )
		{
			detail::DynamicColourBufferConversion< PixelFormat::YUY2 >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
		}
		template< TPL_PIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
		{
			detail::pixel_converter< PixelFormat::YUY2, PF >()( p_pSrc, p_pDst );
		}
	};

	//*************************************************************************************************

	//!\~english Specialisation for PixelFormat::D16	\~french Spécialisation pour PixelFormat::D16
	template <> struct pixel_definitions< PixelFormat::D16 >
	{
		static const uint8_t Size = 2;
		static const bool Alpha = false;
		static const bool Colour = false;
		static const bool Depth = true;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static inline String to_string()
		{
			return cuT( "16 bits depth" );
		}
		static inline String to_str()
		{
			return cuT( "depth16" );
		}
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, PixelFormat p_ePixelFmtDst )
		{
			detail::DynamicDepthConversion< PixelFormat::D16 >( p_pSrc, p_pDst, p_ePixelFmtDst );
		}
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, PixelFormat p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize )
		{
			detail::DynamicDepthBufferConversion< PixelFormat::D16 >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
		}
		template< TPL_PIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
		{
			detail::pixel_converter< PixelFormat::D16, PF >()( p_pSrc, p_pDst );
		}
	};

	//*************************************************************************************************

	//!\~english Specialisation for PixelFormat::D24	\~french Spécialisation pour PixelFormat::D24
	template <> struct pixel_definitions< PixelFormat::D24 >
	{
		static const uint8_t Size = 3;
		static const bool Alpha = false;
		static const bool Colour = false;
		static const bool Depth = true;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static inline String to_string()
		{
			return cuT( "24 bits depth" );
		}
		static inline String to_str()
		{
			return cuT( "depth24" );
		}
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, PixelFormat p_ePixelFmtDst )
		{
			detail::DynamicDepthConversion< PixelFormat::D24 >( p_pSrc, p_pDst, p_ePixelFmtDst );
		}
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, PixelFormat p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize )
		{
			detail::DynamicDepthBufferConversion< PixelFormat::D24 >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
		}
		template< TPL_PIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
		{
			detail::pixel_converter< PixelFormat::D24, PF >()( p_pSrc, p_pDst );
		}
	};

	//*************************************************************************************************

	//!\~english Specialisation for PixelFormat::D24S8	\~french Spécialisation pour PixelFormat::D24S8
	template <> struct pixel_definitions< PixelFormat::D24S8 >
	{
		static const uint8_t Size = 4;
		static const bool Alpha = false;
		static const bool Colour = false;
		static const bool Depth = true;
		static const bool Stencil = true;
		static const bool Compressed = false;
		static inline String to_string()
		{
			return cuT( "24 bits depth, 8 bits stencil" );
		}
		static inline String to_str()
		{
			return cuT( "depth24s8" );
		}
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, PixelFormat p_ePixelFmtDst )
		{
			detail::DynamicDepthConversion< PixelFormat::D24S8 >( p_pSrc, p_pDst, p_ePixelFmtDst );
		}
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, PixelFormat p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize )
		{
			if ( p_eDstFormat == PixelFormat::S8 || p_eDstFormat == PixelFormat::S1 )
			{
				detail::DynamicStencilBufferConversion< PixelFormat::D24S8 >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
			}
			else
			{
				detail::DynamicDepthBufferConversion< PixelFormat::D24S8 >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
			}
		}
		template< TPL_PIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
		{
			detail::pixel_converter<PixelFormat::D24S8, PF>()( p_pSrc, p_pDst );
		}
	};

	//*************************************************************************************************

	//!\~english Specialisation for PixelFormat::D32	\~french Spécialisation pour PixelFormat::D32
	template <> struct pixel_definitions< PixelFormat::D32 >
	{
		static const uint8_t Size = 4;
		static const bool Alpha = false;
		static const bool Colour = false;
		static const bool Depth = true;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static inline String to_string()
		{
			return cuT( "32 bits depth" );
		}
		static inline String to_str()
		{
			return cuT( "depth32" );
		}
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, PixelFormat p_ePixelFmtDst )
		{
			detail::DynamicDepthConversion< PixelFormat::D32 >( p_pSrc, p_pDst, p_ePixelFmtDst );
		}
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, PixelFormat p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize )
		{
			detail::DynamicDepthBufferConversion< PixelFormat::D32 >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
		}
		template< TPL_PIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
		{
			detail::pixel_converter< PixelFormat::D32, PF >()( p_pSrc, p_pDst );
		}
	};

	//*************************************************************************************************

	//!\~english Specialisation for PixelFormat::D32F	\~french Spécialisation pour PixelFormat::D32F
	template <> struct pixel_definitions< PixelFormat::D32F >
	{
		static const uint8_t Size = 4;
		static const bool Alpha = false;
		static const bool Colour = false;
		static const bool Depth = true;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static inline String to_string()
		{
			return cuT( "32 bits floating point depth" );
		}
		static inline String to_str()
		{
			return cuT( "depth32f" );
		}
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, PixelFormat p_ePixelFmtDst )
		{
			detail::DynamicDepthConversion< PixelFormat::D32F >( p_pSrc, p_pDst, p_ePixelFmtDst );
		}
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, PixelFormat p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize )
		{
			detail::DynamicDepthBufferConversion< PixelFormat::D32F >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
		}
		template< TPL_PIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
		{
			detail::pixel_converter< PixelFormat::D32F, PF >()( p_pSrc, p_pDst );
		}
	};

	//*************************************************************************************************

	//!\~english Specialisation for PixelFormat::S1	\~french Spécialisation pour PixelFormat::S1
	template <> struct pixel_definitions< PixelFormat::S1 >
	{
		static const uint8_t Size = 1;
		static const bool Alpha = false;
		static const bool Colour = false;
		static const bool Depth = false;
		static const bool Stencil = true;
		static const bool Compressed = false;
		static inline String to_string()
		{
			return cuT( "1 bit stencil" );
		}
		static inline String to_str()
		{
			return cuT( "stencil1" );
		}
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, PixelFormat p_ePixelFmtDst )
		{
			detail::DynamicStencilConversion< PixelFormat::S1 >( p_pSrc, p_pDst, p_ePixelFmtDst );
		}
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, PixelFormat p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize )
		{
			detail::DynamicStencilBufferConversion< PixelFormat::S1 >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
		}
		template< TPL_PIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
		{
			detail::pixel_converter< PixelFormat::S1, PF >()( p_pSrc, p_pDst );
		}
	};

	//*************************************************************************************************

	//!\~english Specialisation for PixelFormat::S8	\~french Spécialisation pour PixelFormat::S8
	template <> struct pixel_definitions< PixelFormat::S8 >
	{
		static const uint8_t Size = 1;
		static const bool Alpha = false;
		static const bool Colour = false;
		static const bool Depth = false;
		static const bool Stencil = true;
		static const bool Compressed = false;
		static inline String to_string()
		{
			return cuT( "8 bits stencil" );
		}
		static inline String to_str()
		{
			return cuT( "stencil8" );
		}
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, PixelFormat p_ePixelFmtDst )
		{
			detail::DynamicStencilConversion< PixelFormat::S8 >( p_pSrc, p_pDst, p_ePixelFmtDst );
		}
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, PixelFormat p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize )
		{
			detail::DynamicStencilBufferConversion< PixelFormat::S8 >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
		}
		template< TPL_PIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
		{
			detail::pixel_converter< PixelFormat::S8, PF >()( p_pSrc, p_pDst );
		}
	};
}
