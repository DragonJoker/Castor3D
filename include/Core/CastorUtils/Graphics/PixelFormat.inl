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
	static inline void halfToFloat( float & target, uint16_t const * source )
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

	static inline void floatToHalf( uint16_t * target, float source )
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
						hm = 0u;  // set mantissa to zero
					}
					else
					{
						xm |= 0x00800000u;  // add the hidden leading bit
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

	//!\~english	Specialisation for PixelFormat::eR8_UNORM
	//!\~french		Spécialisation pour PixelFormat::eR8_UNORM
	template<>
	struct PixelComponents< PixelFormat::eR8_UNORM >
	{
		static inline uint8_t L8( uint8_t const * srcBuffer )
		{
			return srcBuffer[0];
		}
		static inline uint8_t R8( uint8_t const * srcBuffer )
		{
			return srcBuffer[0];
		}
		static inline uint8_t G8( uint8_t const * srcBuffer )
		{
			return srcBuffer[0];
		}
		static inline uint8_t B8( uint8_t const * srcBuffer )
		{
			return srcBuffer[0];
		}
		static inline uint8_t A8( uint8_t const * )
		{
			return 0xFF;
		}
		static inline float L32F( uint8_t const * srcBuffer )
		{
			return L8( srcBuffer ) / 255.0f;
		}
		static inline float R32F( uint8_t const * srcBuffer )
		{
			return R8( srcBuffer ) / 255.0f;
		}
		static inline float G32F( uint8_t const * srcBuffer )
		{
			return G8( srcBuffer ) / 255.0f;
		}
		static inline float B32F( uint8_t const * srcBuffer )
		{
			return B8( srcBuffer ) / 255.0f;
		}
		static inline float A32F( uint8_t const * srcBuffer )
		{
			return A8( srcBuffer ) / 255.0f;
		}
		static inline void L8( uint8_t * srcBuffer, uint8_t value )
		{
			srcBuffer[0] = value;
		}
		static inline void R8( uint8_t * srcBuffer, uint8_t value )
		{
			srcBuffer[0] = value;
		}
		static inline void G8( uint8_t * srcBuffer, uint8_t value )
		{
			srcBuffer[0] = value;
		}
		static inline void B8( uint8_t * srcBuffer, uint8_t value )
		{
			srcBuffer[0] = value;
		}
		static inline void A8( uint8_t *, uint8_t )
		{
		}
		static inline void L32F( uint8_t * srcBuffer, float value )
		{
			L8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void R32F( uint8_t * srcBuffer, float value )
		{
			R8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void G32F( uint8_t * srcBuffer, float value )
		{
			G8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void B32F( uint8_t * srcBuffer, float value )
		{
			B8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void A32F( uint8_t *, float )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR32_SFLOAT
	//!\~french		Spécialisation pour PixelFormat::eR32_SFLOAT
	template<>
	struct PixelComponents< PixelFormat::eR32_SFLOAT >
	{
#define src ( *reinterpret_cast< float const * >( srcBuffer ) )
#define dst ( *reinterpret_cast< float * >( srcBuffer ) )

		static inline uint8_t L8( uint8_t const * srcBuffer )
		{
			return uint8_t( L32F( srcBuffer ) * 255 );
		}
		static inline uint8_t R8( uint8_t const * srcBuffer )
		{
			return uint8_t( R32F( srcBuffer ) * 255 );
		}
		static inline uint8_t G8( uint8_t const * srcBuffer )
		{
			return uint8_t( G32F( srcBuffer ) * 255 );
		}
		static inline uint8_t B8( uint8_t const * srcBuffer )
		{
			return uint8_t( B32F( srcBuffer ) * 255 );
		}
		static inline uint8_t A8( uint8_t const * srcBuffer )
		{
			return uint8_t( A32F( srcBuffer ) * 255 );
		}
		static inline float L32F( uint8_t const * srcBuffer )
		{
			return src;
		}
		static inline float R32F( uint8_t const * srcBuffer )
		{
			return src;
		}
		static inline float G32F( uint8_t const * srcBuffer )
		{
			return src;
		}
		static inline float B32F( uint8_t const * srcBuffer )
		{
			return src;
		}
		static inline float A32F( uint8_t const * )
		{
			return 1.0f;
		}
		static inline void L8( uint8_t * srcBuffer, uint8_t value )
		{
			L32F( srcBuffer, float( value ) / 255 );
		}
		static inline void R8( uint8_t * srcBuffer, uint8_t value )
		{
			R32F( srcBuffer, float( value ) / 255 );
		}
		static inline void G8( uint8_t * srcBuffer, uint8_t value )
		{
			G32F( srcBuffer, float( value ) / 255 );
		}
		static inline void B8( uint8_t * srcBuffer, uint8_t value )
		{
			B32F( srcBuffer, float( value ) / 255 );
		}
		static inline void A8( uint8_t *, uint8_t pal )
		{
		}
		static inline void L32F( uint8_t * srcBuffer, float value )
		{
			dst = value;
		}
		static inline void R32F( uint8_t * srcBuffer, float value )
		{
			dst = value;
		}
		static inline void G32F( uint8_t * srcBuffer, float value )
		{
			dst = value;
		}
		static inline void B32F( uint8_t * srcBuffer, float value )
		{
			dst = value;
		}
		static inline void A32F( uint8_t *, float )
		{
		}

#undef src
#undef dst
	};
	//!\~english	Specialisation for PixelFormat::eR8A8_UNORM
	//!\~french		Spécialisation pour PixelFormat::eR8A8_UNORM
	template<>
	struct PixelComponents< PixelFormat::eR8A8_UNORM >
	{
		static inline uint8_t L8( uint8_t const * srcBuffer )
		{
			return srcBuffer[0];
		}
		static inline uint8_t R8( uint8_t const * srcBuffer )
		{
			return srcBuffer[0];
		}
		static inline uint8_t G8( uint8_t const * srcBuffer )
		{
			return srcBuffer[0];
		}
		static inline uint8_t B8( uint8_t const * srcBuffer )
		{
			return srcBuffer[0];
		}
		static inline uint8_t A8( uint8_t const * srcBuffer )
		{
			return srcBuffer[1];
		}
		static inline float L32F( uint8_t const * srcBuffer )
		{
			return L8( srcBuffer ) / 255.0f;
		}
		static inline float R32F( uint8_t const * srcBuffer )
		{
			return R8( srcBuffer ) / 255.0f;
		}
		static inline float G32F( uint8_t const * srcBuffer )
		{
			return G8( srcBuffer ) / 255.0f;
		}
		static inline float B32F( uint8_t const * srcBuffer )
		{
			return B8( srcBuffer ) / 255.0f;
		}
		static inline float A32F( uint8_t const * srcBuffer )
		{
			return A8( srcBuffer ) / 255.0f;
		}
		static inline void L8( uint8_t * srcBuffer, uint8_t value )
		{
			srcBuffer[0] = value;
		}
		static inline void R8( uint8_t * srcBuffer, uint8_t value )
		{
			srcBuffer[0] = value;
		}
		static inline void G8( uint8_t * srcBuffer, uint8_t value )
		{
			srcBuffer[0] = value;
		}
		static inline void B8( uint8_t * srcBuffer, uint8_t value )
		{
			srcBuffer[0] = value;
		}
		static inline void A8( uint8_t * srcBuffer, uint8_t value )
		{
			srcBuffer[1] = value;
		}
		static inline void L32F( uint8_t * srcBuffer, float value )
		{
			L8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void R32F( uint8_t * srcBuffer, float value )
		{
			R8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void G32F( uint8_t * srcBuffer, float value )
		{
			G8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void B32F( uint8_t * srcBuffer, float value )
		{
			B8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void A32F( uint8_t * srcBuffer, float value )
		{
			A8( srcBuffer, uint8_t( value * 255.0 ) );
		}
	};
	//!\~english	Specialisation for PixelFormat::eR32A32_SFLOAT
	//!\~french		Spécialisation pour PixelFormat::eR32A32_SFLOAT
	template<>
	struct PixelComponents< PixelFormat::eR32A32_SFLOAT >
	{
#define src reinterpret_cast< float const * >( srcBuffer )
#define dst reinterpret_cast< float * >( srcBuffer )

		static inline uint8_t L8( uint8_t const * srcBuffer )
		{
			return uint8_t( L32F( srcBuffer ) * 255 );
		}
		static inline uint8_t R8( uint8_t const * srcBuffer )
		{
			return uint8_t( R32F( srcBuffer ) * 255 );
		}
		static inline uint8_t G8( uint8_t const * srcBuffer )
		{
			return uint8_t( G32F( srcBuffer ) * 255 );
		}
		static inline uint8_t B8( uint8_t const * srcBuffer )
		{
			return uint8_t( B32F( srcBuffer ) * 255 );
		}
		static inline uint8_t A8( uint8_t const * srcBuffer )
		{
			return uint8_t( A32F( srcBuffer ) * 255 );
		}
		static inline float L32F( uint8_t const * srcBuffer )
		{
			return src[0];
		}
		static inline float R32F( uint8_t const * srcBuffer )
		{
			return src[0];
		}
		static inline float G32F( uint8_t const * srcBuffer )
		{
			return src[0];
		}
		static inline float B32F( uint8_t const * srcBuffer )
		{
			return src[0];
		}
		static inline float A32F( uint8_t const * srcBuffer )
		{
			return src[1];
		}
		static inline void L8( uint8_t * srcBuffer, uint8_t value )
		{
			L32F( srcBuffer, float( value ) / 255 );
		}
		static inline void R8( uint8_t * srcBuffer, uint8_t value )
		{
			R32F( srcBuffer, float( value ) / 255 );
		}
		static inline void G8( uint8_t * srcBuffer, uint8_t value )
		{
			G32F( srcBuffer, float( value ) / 255 );
		}
		static inline void B8( uint8_t * srcBuffer, uint8_t value )
		{
			B32F( srcBuffer, float( value ) / 255 );
		}
		static inline void A8( uint8_t * srcBuffer, uint8_t value )
		{
			A32F( srcBuffer, float( value ) / 255 );
		}
		static inline void L32F( uint8_t * srcBuffer, float value )
		{
			dst[0] = value;
		}
		static inline void R32F( uint8_t * srcBuffer, float value )
		{
			dst[0] = value;
		}
		static inline void G32F( uint8_t * srcBuffer, float value )
		{
			dst[0] = value;
		}
		static inline void B32F( uint8_t * srcBuffer, float value )
		{
			dst[0] = value;
		}
		static inline void A32F( uint8_t * srcBuffer, float value )
		{
			dst[1] = value;
		}

#undef src
#undef dst
	};
	//!\~english	Specialisation for PixelFormat::eR5G5B5A1_UNORM
	//!\~french		Spécialisation pour PixelFormat::eR5G5B5A1_UNORM
	template<>
	struct PixelComponents< PixelFormat::eR5G5B5A1_UNORM >
	{
#define src ( *reinterpret_cast< uint16_t const * >( srcBuffer ) )
#define dst ( *reinterpret_cast< uint16_t * >( srcBuffer ) )

		static inline uint8_t L8( uint8_t const * srcBuffer )
		{
			return uint8_t( R8( srcBuffer ) * 0.30 + G8( srcBuffer ) * 0.59 + B8( srcBuffer ) * 0.11 );
		}
		static inline uint8_t A8( uint8_t const * srcBuffer )
		{
			return uint8_t( ( src & 0x8000 ) ? 0xFF : 0x00 );
		}
		static inline uint8_t R8( uint8_t const * srcBuffer )
		{
			return uint8_t( ( src & 0x7C00 ) >> 7 );
		}
		static inline uint8_t G8( uint8_t const * srcBuffer )
		{
			return uint8_t( ( src & 0x03E0 ) >> 2 );
		}
		static inline uint8_t B8( uint8_t const * srcBuffer )
		{
			return uint8_t( ( src & 0x001F ) << 3 );
		}
		static inline float L32F( uint8_t const * srcBuffer )
		{
			return L8( srcBuffer ) / 255.0f;
		}
		static inline float R32F( uint8_t const * srcBuffer )
		{
			return R8( srcBuffer ) / 255.0f;
		}
		static inline float G32F( uint8_t const * srcBuffer )
		{
			return G8( srcBuffer ) / 255.0f;
		}
		static inline float B32F( uint8_t const * srcBuffer )
		{
			return B8( srcBuffer ) / 255.0f;
		}
		static inline float A32F( uint8_t const * srcBuffer )
		{
			return A8( srcBuffer ) / 255.0f;
		}
		static inline void L8( uint8_t * srcBuffer, uint8_t value )
		{
			R8( srcBuffer, value );
			G8( srcBuffer, value );
			B8( srcBuffer, value );
		}
		static inline void A8( uint8_t * srcBuffer, uint8_t value )
		{
			dst = ( src & 0x7FFF ) | ( value ? 0x8000 : 0x0000 );
		}
		static inline void R8( uint8_t * srcBuffer, uint8_t value )
		{
			dst = ( src & 0x83FF ) | ( ( uint16_t( value ) << 7 ) & 0x7C00 );
		}
		static inline void G8( uint8_t * srcBuffer, uint8_t value )
		{
			dst = ( src & 0xFC1F ) | ( ( uint16_t( value ) << 2 ) & 0x03E0 );
		}
		static inline void B8( uint8_t * srcBuffer, uint8_t value )
		{
			dst = ( src & 0xFFE0 ) | ( ( uint16_t( value ) >> 3 ) & 0x001F );
		}
		static inline void L32F( uint8_t * srcBuffer, float value )
		{
			L8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void R32F( uint8_t * srcBuffer, float value )
		{
			R8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void G32F( uint8_t * srcBuffer, float value )
		{
			G8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void B32F( uint8_t * srcBuffer, float value )
		{
			B8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void A32F( uint8_t * srcBuffer, float value )
		{
			A8( srcBuffer, uint8_t( value * 255.0 ) );
		}

#undef src
#undef dst
	};
	//!\~english	Specialisation for PixelFormat::eR5G6B5_UNORM
	//!\~french		Spécialisation pour PixelFormat::eR5G6B5_UNORM
	template<>
	struct PixelComponents< PixelFormat::eR5G6B5_UNORM >
	{
#define src	( *reinterpret_cast< uint16_t const * >( srcBuffer ) )
#define dst	( *reinterpret_cast< uint16_t * >( srcBuffer ) )

		static inline uint8_t L8( uint8_t const * srcBuffer )
		{
			return uint8_t( R8( srcBuffer ) * 0.30 + G8( srcBuffer ) * 0.59 + B8( srcBuffer ) * 0.11 );
		}
		static inline uint8_t R8( uint8_t const * srcBuffer )
		{
			return uint8_t( ( src & 0xF800 ) >> 8 );
		}
		static inline uint8_t G8( uint8_t const * srcBuffer )
		{
			return uint8_t( ( src & 0x07E0 ) >> 3 );
		}
		static inline uint8_t B8( uint8_t const * srcBuffer )
		{
			return uint8_t( ( src & 0x001F ) << 3 );
		}
		static inline uint8_t A8( uint8_t const * )
		{
			return 0xFF;
		}
		static inline float L32F( uint8_t const * srcBuffer )
		{
			return L8( srcBuffer ) / 255.0f;
		}
		static inline float R32F( uint8_t const * srcBuffer )
		{
			return R8( srcBuffer ) / 255.0f;
		}
		static inline float G32F( uint8_t const * srcBuffer )
		{
			return G8( srcBuffer ) / 255.0f;
		}
		static inline float B32F( uint8_t const * srcBuffer )
		{
			return B8( srcBuffer ) / 255.0f;
		}
		static inline float A32F( uint8_t const * srcBuffer )
		{
			return A8( srcBuffer ) / 255.0f;
		}
		static inline void L8( uint8_t * srcBuffer, uint8_t value )
		{
			R8( srcBuffer, value );
			G8( srcBuffer, value );
			B8( srcBuffer, value );
		}
		static inline void R8( uint8_t * srcBuffer, uint8_t value )
		{
			dst = ( src & 0x07FF ) | ( ( uint16_t( value ) << 8 ) & 0xF800 );
		}
		static inline void G8( uint8_t * srcBuffer, uint8_t value )
		{
			dst = ( src & 0xF81F ) | ( ( uint16_t( value ) << 3 ) & 0x07E0 );
		}
		static inline void B8( uint8_t * srcBuffer, uint8_t value )
		{
			dst = ( src & 0xFFE0 ) | ( ( uint16_t( value ) >> 3 ) & 0x001F );
		}
		static inline void A8( uint8_t *, uint8_t )
		{
		}
		static inline void L32F( uint8_t * srcBuffer, float value )
		{
			L8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void R32F( uint8_t * srcBuffer, float value )
		{
			R8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void G32F( uint8_t * srcBuffer, float value )
		{
			G8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void B32F( uint8_t * srcBuffer, float value )
		{
			B8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void A32F( uint8_t *, float )
		{
		}

#undef src
#undef dst
	};
	//!\~english	Specialisation for PixelFormat::eR8G8B8_UNORM
	//!\~french		Spécialisation pour PixelFormat::eR8G8B8_UNORM
	template<>
	struct PixelComponents< PixelFormat::eR8G8B8_UNORM >
	{
		struct pixel
		{
			uint8_t r;
			uint8_t g;
			uint8_t b;
		};

#define src	( *reinterpret_cast< pixel const * >( srcBuffer ) )
#define dst	( *reinterpret_cast< pixel * >( srcBuffer ) )

		static inline uint8_t L8( uint8_t const * srcBuffer )
		{
			return uint8_t( R8( srcBuffer ) * 0.30 + G8( srcBuffer ) * 0.59 + B8( srcBuffer ) * 0.11 );
		}
		static inline uint8_t R8( uint8_t const * srcBuffer )
		{
			return src.r;
		}
		static inline uint8_t G8( uint8_t const * srcBuffer )
		{
			return src.g;
		}
		static inline uint8_t B8( uint8_t const * srcBuffer )
		{
			return src.b;
		}
		static inline uint8_t A8( uint8_t const * )
		{
			return 0xFF;
		}
		static inline float L32F( uint8_t const * srcBuffer )
		{
			return L8( srcBuffer ) / 255.0f;
		}
		static inline float R32F( uint8_t const * srcBuffer )
		{
			return R8( srcBuffer ) / 255.0f;
		}
		static inline float G32F( uint8_t const * srcBuffer )
		{
			return G8( srcBuffer ) / 255.0f;
		}
		static inline float B32F( uint8_t const * srcBuffer )
		{
			return B8( srcBuffer ) / 255.0f;
		}
		static inline float A32F( uint8_t const * srcBuffer )
		{
			return A8( srcBuffer ) / 255.0f;
		}
		static inline void L8( uint8_t * srcBuffer, uint8_t value )
		{
			R8( srcBuffer, value );
			G8( srcBuffer, value );
			B8( srcBuffer, value );
		}
		static inline void R8( uint8_t * srcBuffer, uint8_t value )
		{
			dst.r = value;
		}
		static inline void G8( uint8_t * srcBuffer, uint8_t value )
		{
			dst.g = value;
		}
		static inline void B8( uint8_t * srcBuffer, uint8_t value )
		{
			dst.b = value;
		}
		static inline void A8( uint8_t *, uint8_t )
		{
		}
		static inline void L32F( uint8_t * srcBuffer, float value )
		{
			L8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void R32F( uint8_t * srcBuffer, float value )
		{
			R8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void G32F( uint8_t * srcBuffer, float value )
		{
			G8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void B32F( uint8_t * srcBuffer, float value )
		{
			B8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void A32F( uint8_t *, float )
		{
		}

#undef src
#undef dst
	};
	//!\~english	Specialisation for PixelFormat::eB8G8R8_UNORM
	//!\~french		Spécialisation pour PixelFormat::eB8G8R8_UNORM
	template<>
	struct PixelComponents< PixelFormat::eB8G8R8_UNORM >
	{
		struct pixel
		{
			uint8_t b;
			uint8_t g;
			uint8_t r;
		};

#define src	( *reinterpret_cast< pixel const * >( srcBuffer ) )
#define dst	( *reinterpret_cast< pixel * >( srcBuffer ) )

		static inline uint8_t L8( uint8_t const * srcBuffer )
		{
			return uint8_t( R8( srcBuffer ) * 0.30 + G8( srcBuffer ) * 0.59 + B8( srcBuffer ) * 0.11 );
		}
		static inline uint8_t R8( uint8_t const * srcBuffer )
		{
			return src.r;
		}
		static inline uint8_t G8( uint8_t const * srcBuffer )
		{
			return src.g;
		}
		static inline uint8_t B8( uint8_t const * srcBuffer )
		{
			return src.b;
		}
		static inline uint8_t A8( uint8_t const * )
		{
			return 0xFF;
		}
		static inline float L32F( uint8_t const * srcBuffer )
		{
			return L8( srcBuffer ) / 255.0f;
		}
		static inline float R32F( uint8_t const * srcBuffer )
		{
			return R8( srcBuffer ) / 255.0f;
		}
		static inline float G32F( uint8_t const * srcBuffer )
		{
			return G8( srcBuffer ) / 255.0f;
		}
		static inline float B32F( uint8_t const * srcBuffer )
		{
			return B8( srcBuffer ) / 255.0f;
		}
		static inline float A32F( uint8_t const * srcBuffer )
		{
			return A8( srcBuffer ) / 255.0f;
		}
		static inline void L8( uint8_t * srcBuffer, uint8_t value )
		{
			R8( srcBuffer, value );
			G8( srcBuffer, value );
			B8( srcBuffer, value );
		}
		static inline void R8( uint8_t * srcBuffer, uint8_t value )
		{
			dst.r = value;
		}
		static inline void G8( uint8_t * srcBuffer, uint8_t value )
		{
			dst.g = value;
		}
		static inline void B8( uint8_t * srcBuffer, uint8_t value )
		{
			dst.b = value;
		}
		static inline void A8( uint8_t *, uint8_t )
		{
		}
		static inline void L32F( uint8_t * srcBuffer, float value )
		{
			L8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void R32F( uint8_t * srcBuffer, float value )
		{
			R8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void G32F( uint8_t * srcBuffer, float value )
		{
			G8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void B32F( uint8_t * srcBuffer, float value )
		{
			B8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void A32F( uint8_t *, float )
		{
		}

#undef src
#undef dst
	};
	//!\~english	Specialisation for PixelFormat::eR8G8B8_SRGB
	//!\~french		Spécialisation pour PixelFormat::eR8G8B8_SRGB
	template<>
	struct PixelComponents< PixelFormat::eR8G8B8_SRGB >
	{
		struct pixel
		{
			uint8_t r;
			uint8_t g;
			uint8_t b;
		};

#define src	( *reinterpret_cast< pixel const * >( srcBuffer ) )
#define dst	( *reinterpret_cast< pixel * >( srcBuffer ) )

		static inline uint8_t L8( uint8_t const * srcBuffer )
		{
			return uint8_t( R8( srcBuffer ) * 0.30 + G8( srcBuffer ) * 0.59 + B8( srcBuffer ) * 0.11 );
		}
		static inline uint8_t R8( uint8_t const * srcBuffer )
		{
			return src.r;
		}
		static inline uint8_t G8( uint8_t const * srcBuffer )
		{
			return src.g;
		}
		static inline uint8_t B8( uint8_t const * srcBuffer )
		{
			return src.b;
		}
		static inline uint8_t A8( uint8_t const * )
		{
			return 0xFF;
		}
		static inline float L32F( uint8_t const * srcBuffer )
		{
			return L8( srcBuffer ) / 255.0f;
		}
		static inline float R32F( uint8_t const * srcBuffer )
		{
			return R8( srcBuffer ) / 255.0f;
		}
		static inline float G32F( uint8_t const * srcBuffer )
		{
			return G8( srcBuffer ) / 255.0f;
		}
		static inline float B32F( uint8_t const * srcBuffer )
		{
			return B8( srcBuffer ) / 255.0f;
		}
		static inline float A32F( uint8_t const * srcBuffer )
		{
			return A8( srcBuffer ) / 255.0f;
		}
		static inline void L8( uint8_t * srcBuffer, uint8_t value )
		{
			R8( srcBuffer, value );
			G8( srcBuffer, value );
			B8( srcBuffer, value );
		}
		static inline void R8( uint8_t * srcBuffer, uint8_t value )
		{
			dst.r = value;
		}
		static inline void G8( uint8_t * srcBuffer, uint8_t value )
		{
			dst.g = value;
		}
		static inline void B8( uint8_t * srcBuffer, uint8_t value )
		{
			dst.b = value;
		}
		static inline void A8( uint8_t *, uint8_t )
		{
		}
		static inline void L32F( uint8_t * srcBuffer, float value )
		{
			L8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void R32F( uint8_t * srcBuffer, float value )
		{
			R8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void G32F( uint8_t * srcBuffer, float value )
		{
			G8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void B32F( uint8_t * srcBuffer, float value )
		{
			B8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void A32F( uint8_t *, float )
		{
		}

#undef src
#undef dst
	};
	//!\~english	Specialisation for PixelFormat::eB8G8R8_SRGB
	//!\~french		Spécialisation pour PixelFormat::eB8G8R8_SRGB
	template<>
	struct PixelComponents< PixelFormat::eB8G8R8_SRGB >
	{
		struct pixel
		{
			uint8_t b;
			uint8_t g;
			uint8_t r;
		};

#define src	( *reinterpret_cast< pixel const * >( srcBuffer ) )
#define dst	( *reinterpret_cast< pixel * >( srcBuffer ) )

		static inline uint8_t L8( uint8_t const * srcBuffer )
		{
			return uint8_t( R8( srcBuffer ) * 0.30 + G8( srcBuffer ) * 0.59 + B8( srcBuffer ) * 0.11 );
		}
		static inline uint8_t R8( uint8_t const * srcBuffer )
		{
			return src.r;
		}
		static inline uint8_t G8( uint8_t const * srcBuffer )
		{
			return src.g;
		}
		static inline uint8_t B8( uint8_t const * srcBuffer )
		{
			return src.b;
		}
		static inline uint8_t A8( uint8_t const * )
		{
			return 0xFF;
		}
		static inline float L32F( uint8_t const * srcBuffer )
		{
			return L8( srcBuffer ) / 255.0f;
		}
		static inline float R32F( uint8_t const * srcBuffer )
		{
			return R8( srcBuffer ) / 255.0f;
		}
		static inline float G32F( uint8_t const * srcBuffer )
		{
			return G8( srcBuffer ) / 255.0f;
		}
		static inline float B32F( uint8_t const * srcBuffer )
		{
			return B8( srcBuffer ) / 255.0f;
		}
		static inline float A32F( uint8_t const * srcBuffer )
		{
			return A8( srcBuffer ) / 255.0f;
		}
		static inline void L8( uint8_t * srcBuffer, uint8_t value )
		{
			R8( srcBuffer, value );
			G8( srcBuffer, value );
			B8( srcBuffer, value );
		}
		static inline void R8( uint8_t * srcBuffer, uint8_t value )
		{
			dst.r = value;
		}
		static inline void G8( uint8_t * srcBuffer, uint8_t value )
		{
			dst.g = value;
		}
		static inline void B8( uint8_t * srcBuffer, uint8_t value )
		{
			dst.b = value;
		}
		static inline void A8( uint8_t *, uint8_t )
		{
		}
		static inline void L32F( uint8_t * srcBuffer, float value )
		{
			L8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void R32F( uint8_t * srcBuffer, float value )
		{
			R8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void G32F( uint8_t * srcBuffer, float value )
		{
			G8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void B32F( uint8_t * srcBuffer, float value )
		{
			B8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void A32F( uint8_t *, float )
		{
		}

#undef src
#undef dst
	};
	//!\~english	Specialisation for PixelFormat::eR8G8B8A8_UNORM
	//!\~french		Spécialisation pour PixelFormat::eR8G8B8A8_UNORM
	template<>
	struct PixelComponents< PixelFormat::eR8G8B8A8_UNORM >
	{
		struct pixel
		{
			uint8_t r;
			uint8_t g;
			uint8_t b;
			uint8_t a;
		};

#define src	( *reinterpret_cast< pixel const * >( srcBuffer ) )
#define dst	( *reinterpret_cast< pixel * >( srcBuffer ) )

		static inline uint8_t L8( uint8_t const * srcBuffer )
		{
			return uint8_t( R8( srcBuffer ) * 0.30 + G8( srcBuffer ) * 0.59 + B8( srcBuffer ) * 0.11 );
		}
		static inline uint8_t A8( uint8_t const * srcBuffer )
		{
			return src.a;
		}
		static inline uint8_t R8( uint8_t const * srcBuffer )
		{
			return src.r;
		}
		static inline uint8_t G8( uint8_t const * srcBuffer )
		{
			return src.g;
		}
		static inline uint8_t B8( uint8_t const * srcBuffer )
		{
			return src.b;
		}
		static inline float L32F( uint8_t const * srcBuffer )
		{
			return L8( srcBuffer ) / 255.0f;
		}
		static inline float R32F( uint8_t const * srcBuffer )
		{
			return R8( srcBuffer ) / 255.0f;
		}
		static inline float G32F( uint8_t const * srcBuffer )
		{
			return G8( srcBuffer ) / 255.0f;
		}
		static inline float B32F( uint8_t const * srcBuffer )
		{
			return B8( srcBuffer ) / 255.0f;
		}
		static inline float A32F( uint8_t const * srcBuffer )
		{
			return A8( srcBuffer ) / 255.0f;
		}
		static inline void L8( uint8_t * srcBuffer, uint8_t value )
		{
			R8( srcBuffer, value );
			G8( srcBuffer, value );
			B8( srcBuffer, value );
		}
		static inline void A8( uint8_t * srcBuffer, uint8_t value )
		{
			dst.a = value;
		}
		static inline void R8( uint8_t * srcBuffer, uint8_t value )
		{
			dst.r = value;
		}
		static inline void G8( uint8_t * srcBuffer, uint8_t value )
		{
			dst.g = value;
		}
		static inline void B8( uint8_t * srcBuffer, uint8_t value )
		{
			dst.b = value;
		}
		static inline void L32F( uint8_t * srcBuffer, float value )
		{
			L8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void R32F( uint8_t * srcBuffer, float value )
		{
			R8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void G32F( uint8_t * srcBuffer, float value )
		{
			G8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void B32F( uint8_t * srcBuffer, float value )
		{
			B8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void A32F( uint8_t * srcBuffer, float value )
		{
			A8( srcBuffer, uint8_t( value * 255.0 ) );
		}

#undef src
#undef dst
	};
	//!\~english	Specialisation for PixelFormat::eA8B8G8R8_UNORM
	//!\~french		Spécialisation pour PixelFormat::eA8B8G8R8_UNORM
	template<>
	struct PixelComponents< PixelFormat::eA8B8G8R8_UNORM >
	{
		struct pixel
		{
			uint8_t b;
			uint8_t g;
			uint8_t r;
			uint8_t a;
		};

#define src	( *reinterpret_cast< pixel const * >( srcBuffer ) )
#define dst	( *reinterpret_cast< pixel * >( srcBuffer ) )

		static inline uint8_t L8( uint8_t const * srcBuffer )
		{
			return uint8_t( R8( srcBuffer ) * 0.30 + G8( srcBuffer ) * 0.59 + B8( srcBuffer ) * 0.11 );
		}
		static inline uint8_t A8( uint8_t const * srcBuffer )
		{
			return src.a;
		}
		static inline uint8_t R8( uint8_t const * srcBuffer )
		{
			return src.r;
		}
		static inline uint8_t G8( uint8_t const * srcBuffer )
		{
			return src.g;
		}
		static inline uint8_t B8( uint8_t const * srcBuffer )
		{
			return src.b;
		}
		static inline float L32F( uint8_t const * srcBuffer )
		{
			return L8( srcBuffer ) / 255.0f;
		}
		static inline float R32F( uint8_t const * srcBuffer )
		{
			return R8( srcBuffer ) / 255.0f;
		}
		static inline float G32F( uint8_t const * srcBuffer )
		{
			return G8( srcBuffer ) / 255.0f;
		}
		static inline float B32F( uint8_t const * srcBuffer )
		{
			return B8( srcBuffer ) / 255.0f;
		}
		static inline float A32F( uint8_t const * srcBuffer )
		{
			return A8( srcBuffer ) / 255.0f;
		}
		static inline void L8( uint8_t * srcBuffer, uint8_t value )
		{
			R8( srcBuffer, value );
			G8( srcBuffer, value );
			B8( srcBuffer, value );
		}
		static inline void A8( uint8_t * srcBuffer, uint8_t value )
		{
			dst.a = value;
		}
		static inline void R8( uint8_t * srcBuffer, uint8_t value )
		{
			dst.r = value;
		}
		static inline void G8( uint8_t * srcBuffer, uint8_t value )
		{
			dst.g = value;
		}
		static inline void B8( uint8_t * srcBuffer, uint8_t value )
		{
			dst.b = value;
		}
		static inline void L32F( uint8_t * srcBuffer, float value )
		{
			L8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void R32F( uint8_t * srcBuffer, float value )
		{
			R8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void G32F( uint8_t * srcBuffer, float value )
		{
			G8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void B32F( uint8_t * srcBuffer, float value )
		{
			B8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void A32F( uint8_t * srcBuffer, float value )
		{
			A8( srcBuffer, uint8_t( value * 255.0 ) );
		}

#undef src
#undef dst
	};
	//!\~english	Specialisation for PixelFormat::eR8G8B8A8_SRGB
	//!\~french		Spécialisation pour PixelFormat::eR8G8B8A8_SRGB
	template<>
	struct PixelComponents< PixelFormat::eR8G8B8A8_SRGB >
	{
		struct pixel
		{
			uint8_t r;
			uint8_t g;
			uint8_t b;
			uint8_t a;
		};

#define src	( *reinterpret_cast< pixel const * >( srcBuffer ) )
#define dst	( *reinterpret_cast< pixel * >( srcBuffer ) )

		static inline uint8_t L8( uint8_t const * srcBuffer )
		{
			return uint8_t( R8( srcBuffer ) * 0.30 + G8( srcBuffer ) * 0.59 + B8( srcBuffer ) * 0.11 );
		}
		static inline uint8_t A8( uint8_t const * srcBuffer )
		{
			return src.a;
		}
		static inline uint8_t R8( uint8_t const * srcBuffer )
		{
			return src.r;
		}
		static inline uint8_t G8( uint8_t const * srcBuffer )
		{
			return src.g;
		}
		static inline uint8_t B8( uint8_t const * srcBuffer )
		{
			return src.b;
		}
		static inline float L32F( uint8_t const * srcBuffer )
		{
			return L8( srcBuffer ) / 255.0f;
		}
		static inline float R32F( uint8_t const * srcBuffer )
		{
			return R8( srcBuffer ) / 255.0f;
		}
		static inline float G32F( uint8_t const * srcBuffer )
		{
			return G8( srcBuffer ) / 255.0f;
		}
		static inline float B32F( uint8_t const * srcBuffer )
		{
			return B8( srcBuffer ) / 255.0f;
		}
		static inline float A32F( uint8_t const * srcBuffer )
		{
			return A8( srcBuffer ) / 255.0f;
		}
		static inline void L8( uint8_t * srcBuffer, uint8_t value )
		{
			R8( srcBuffer, value );
			G8( srcBuffer, value );
			B8( srcBuffer, value );
		}
		static inline void A8( uint8_t * srcBuffer, uint8_t value )
		{
			dst.a = value;
		}
		static inline void R8( uint8_t * srcBuffer, uint8_t value )
		{
			dst.r = value;
		}
		static inline void G8( uint8_t * srcBuffer, uint8_t value )
		{
			dst.g = value;
		}
		static inline void B8( uint8_t * srcBuffer, uint8_t value )
		{
			dst.b = value;
		}
		static inline void L32F( uint8_t * srcBuffer, float value )
		{
			L8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void R32F( uint8_t * srcBuffer, float value )
		{
			R8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void G32F( uint8_t * srcBuffer, float value )
		{
			G8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void B32F( uint8_t * srcBuffer, float value )
		{
			B8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void A32F( uint8_t * srcBuffer, float value )
		{
			A8( srcBuffer, uint8_t( value * 255.0 ) );
		}

#undef src
#undef dst
	};
	//!\~english	Specialisation for PixelFormat::eA8B8G8R8_SRGB
	//!\~french		Spécialisation pour PixelFormat::eA8B8G8R8_SRGB
	template<>
	struct PixelComponents< PixelFormat::eA8B8G8R8_SRGB >
	{
		struct pixel
		{
			uint8_t b;
			uint8_t g;
			uint8_t r;
			uint8_t a;
		};

#define src	( *reinterpret_cast< pixel const * >( srcBuffer ) )
#define dst	( *reinterpret_cast< pixel * >( srcBuffer ) )

		static inline uint8_t L8( uint8_t const * srcBuffer )
		{
			return uint8_t( R8( srcBuffer ) * 0.30 + G8( srcBuffer ) * 0.59 + B8( srcBuffer ) * 0.11 );
		}
		static inline uint8_t A8( uint8_t const * srcBuffer )
		{
			return src.a;
		}
		static inline uint8_t R8( uint8_t const * srcBuffer )
		{
			return src.r;
		}
		static inline uint8_t G8( uint8_t const * srcBuffer )
		{
			return src.g;
		}
		static inline uint8_t B8( uint8_t const * srcBuffer )
		{
			return src.b;
		}
		static inline float L32F( uint8_t const * srcBuffer )
		{
			return L8( srcBuffer ) / 255.0f;
		}
		static inline float R32F( uint8_t const * srcBuffer )
		{
			return R8( srcBuffer ) / 255.0f;
		}
		static inline float G32F( uint8_t const * srcBuffer )
		{
			return G8( srcBuffer ) / 255.0f;
		}
		static inline float B32F( uint8_t const * srcBuffer )
		{
			return B8( srcBuffer ) / 255.0f;
		}
		static inline float A32F( uint8_t const * srcBuffer )
		{
			return A8( srcBuffer ) / 255.0f;
		}
		static inline void L8( uint8_t * srcBuffer, uint8_t value )
		{
			R8( srcBuffer, value );
			G8( srcBuffer, value );
			B8( srcBuffer, value );
		}
		static inline void A8( uint8_t * srcBuffer, uint8_t value )
		{
			dst.a = value;
		}
		static inline void R8( uint8_t * srcBuffer, uint8_t value )
		{
			dst.r = value;
		}
		static inline void G8( uint8_t * srcBuffer, uint8_t value )
		{
			dst.g = value;
		}
		static inline void B8( uint8_t * srcBuffer, uint8_t value )
		{
			dst.b = value;
		}
		static inline void L32F( uint8_t * srcBuffer, float value )
		{
			L8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void R32F( uint8_t * srcBuffer, float value )
		{
			R8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void G32F( uint8_t * srcBuffer, float value )
		{
			G8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void B32F( uint8_t * srcBuffer, float value )
		{
			B8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void A32F( uint8_t * srcBuffer, float value )
		{
			A8( srcBuffer, uint8_t( value * 255.0 ) );
		}

#undef src
#undef dst
	};
	//!\~english	Specialisation for PixelFormat::eR16G16B16_SFLOAT
	//!\~french		Spécialisation pour PixelFormat::eR16G16B16_SFLOAT
	template<>
	struct PixelComponents< PixelFormat::eR16G16B16_SFLOAT >
	{
		static inline uint8_t L8( uint8_t const * srcBuffer )
		{
			return uint8_t( L32F( srcBuffer ) * 255 );
		}
		static inline uint8_t R8( uint8_t const * srcBuffer )
		{
			return uint8_t( R32F( srcBuffer ) * 255 );
		}
		static inline uint8_t G8( uint8_t const * srcBuffer )
		{
			return uint8_t( G32F( srcBuffer ) * 255 );
		}
		static inline uint8_t B8( uint8_t const * srcBuffer )
		{
			return uint8_t( B32F( srcBuffer ) * 255 );
		}
		static inline uint8_t A8( uint8_t const * srcBuffer )
		{
			return uint8_t( A32F( srcBuffer ) * 255 );
		}
		static inline float L32F( uint8_t const * srcBuffer )
		{
			return float( R32F( srcBuffer ) * 0.30 + G32F( srcBuffer ) * 0.59 + B32F( srcBuffer ) * 0.11 );
		}
		static inline float R32F( uint8_t const * srcBuffer )
		{
			float result{};
			halfToFloat( result, reinterpret_cast< uint16_t const * >( srcBuffer ) + 0 );
			return result;
		}
		static inline float G32F( uint8_t const * srcBuffer )
		{
			float result{};
			halfToFloat( result, reinterpret_cast< uint16_t const * >( srcBuffer ) + 1 );
			return result;
		}
		static inline float B32F( uint8_t const * srcBuffer )
		{
			float result{};
			halfToFloat( result, reinterpret_cast< uint16_t const * >( srcBuffer ) + 2 );
			return result;
		}
		static inline float A32F( uint8_t const * )
		{
			return 1.0f;
		}
		static inline void L8( uint8_t * srcBuffer, uint8_t value )
		{
			L32F( srcBuffer, float( value ) / 255 );
		}
		static inline void R8( uint8_t * srcBuffer, uint8_t value )
		{
			R32F( srcBuffer, float( value ) / 255 );
		}
		static inline void G8( uint8_t * srcBuffer, uint8_t value )
		{
			G32F( srcBuffer, float( value ) / 255 );
		}
		static inline void B8( uint8_t * srcBuffer, uint8_t value )
		{
			B32F( srcBuffer, float( value ) / 255 );
		}
		static inline void A8( uint8_t *, uint8_t )
		{
		}
		static inline void L32F( uint8_t * srcBuffer, float value )
		{
			R32F( srcBuffer, value );
			G32F( srcBuffer, value );
			B32F( srcBuffer, value );
		}
		static inline void R32F( uint8_t * srcBuffer, float value )
		{
			floatToHalf( reinterpret_cast< uint16_t * >( srcBuffer ) + 0, value );
		}
		static inline void G32F( uint8_t * srcBuffer, float value )
		{
			floatToHalf( reinterpret_cast< uint16_t * >( srcBuffer ) + 1, value );
		}
		static inline void B32F( uint8_t * srcBuffer, float value )
		{
			floatToHalf( reinterpret_cast< uint16_t * >( srcBuffer ) + 2, value );
		}
		static inline void A32F( uint8_t *, float )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR32G32B32_SFLOAT
	//!\~french		Spécialisation pour PixelFormat::eR32G32B32_SFLOAT
	template<>
	struct PixelComponents< PixelFormat::eR32G32B32_SFLOAT >
	{
		static inline uint8_t L8( uint8_t const * srcBuffer )
		{
			return uint8_t( L32F( srcBuffer ) * 255 );
		}
		static inline uint8_t R8( uint8_t const * srcBuffer )
		{
			return uint8_t( R32F( srcBuffer ) * 255 );
		}
		static inline uint8_t G8( uint8_t const * srcBuffer )
		{
			return uint8_t( G32F( srcBuffer ) * 255 );
		}
		static inline uint8_t B8( uint8_t const * srcBuffer )
		{
			return uint8_t( B32F( srcBuffer ) * 255 );
		}
		static inline uint8_t A8( uint8_t const * srcBuffer )
		{
			return uint8_t( A32F( srcBuffer ) * 255 );
		}
		static inline float L32F( uint8_t const * srcBuffer )
		{
			return float( R32F( srcBuffer ) * 0.30 + G32F( srcBuffer ) * 0.59 + B32F( srcBuffer ) * 0.11 );
		}
		static inline float R32F( uint8_t const * srcBuffer )
		{
			return reinterpret_cast< float const * >( srcBuffer )[0];
		}
		static inline float G32F( uint8_t const * srcBuffer )
		{
			return reinterpret_cast< float const * >( srcBuffer )[1];
		}
		static inline float B32F( uint8_t const * srcBuffer )
		{
			return reinterpret_cast< float const * >( srcBuffer )[2];
		}
		static inline float A32F( uint8_t const * )
		{
			return 1.0f;
		}
		static inline void L8( uint8_t * srcBuffer, uint8_t value )
		{
			L32F( srcBuffer, float( value ) / 255 );
		}
		static inline void R8( uint8_t * srcBuffer, uint8_t value )
		{
			R32F( srcBuffer, float( value ) / 255 );
		}
		static inline void G8( uint8_t * srcBuffer, uint8_t value )
		{
			G32F( srcBuffer, float( value ) / 255 );
		}
		static inline void B8( uint8_t * srcBuffer, uint8_t value )
		{
			B32F( srcBuffer, float( value ) / 255 );
		}
		static inline void A8( uint8_t *, uint8_t )
		{
		}
		static inline void L32F( uint8_t * srcBuffer, float value )
		{
			R32F( srcBuffer, value );
			G32F( srcBuffer, value );
			B32F( srcBuffer, value );
		}
		static inline void R32F( uint8_t * srcBuffer, float value )
		{
			reinterpret_cast< float * >( srcBuffer )[0] = value;
		}
		static inline void G32F( uint8_t * srcBuffer, float value )
		{
			reinterpret_cast< float * >( srcBuffer )[1] = value;
		}
		static inline void B32F( uint8_t * srcBuffer, float value )
		{
			reinterpret_cast< float * >( srcBuffer )[2] = value;
		}
		static inline void A32F( uint8_t *, float )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR16G16B16A16_SFLOAT
	//!\~french		Spécialisation pour PixelFormat::eR16G16B16A16_SFLOAT
	template<>
	struct PixelComponents< PixelFormat::eR16G16B16A16_SFLOAT >
	{
		static inline uint8_t L8( uint8_t const * srcBuffer )
		{
			return uint8_t( L32F( srcBuffer ) * 255 );
		}
		static inline uint8_t R8( uint8_t const * srcBuffer )
		{
			return uint8_t( R32F( srcBuffer ) * 255 );
		}
		static inline uint8_t G8( uint8_t const * srcBuffer )
		{
			return uint8_t( G32F( srcBuffer ) * 255 );
		}
		static inline uint8_t B8( uint8_t const * srcBuffer )
		{
			return uint8_t( B32F( srcBuffer ) * 255 );
		}
		static inline uint8_t A8( uint8_t const * srcBuffer )
		{
			return uint8_t( A32F( srcBuffer ) * 255 );
		}
		static inline float L32F( uint8_t const * srcBuffer )
		{
			return float( R32F( srcBuffer ) * 0.30 + G32F( srcBuffer ) * 0.59 + B32F( srcBuffer ) * 0.11 );
		}
		static inline float R32F( uint8_t const * srcBuffer )
		{
			float result{};
			halfToFloat( result, reinterpret_cast< uint16_t const * >( srcBuffer ) + 0 );
			return result;
		}
		static inline float G32F( uint8_t const * srcBuffer )
		{
			float result{};
			halfToFloat( result, reinterpret_cast< uint16_t const * >( srcBuffer ) + 1 );
			return result;
		}
		static inline float B32F( uint8_t const * srcBuffer )
		{
			float result{};
			halfToFloat( result, reinterpret_cast< uint16_t const * >( srcBuffer ) + 2 );
			return result;
		}
		static inline float A32F( uint8_t const * srcBuffer )
		{
			float result{};
			halfToFloat( result, reinterpret_cast< uint16_t const * >( srcBuffer ) + 3 );
			return result;
		}
		static inline void L8( uint8_t * srcBuffer, uint8_t value )
		{
			L32F( srcBuffer, float( value ) / 255 );
		}
		static inline void R8( uint8_t * srcBuffer, uint8_t value )
		{
			R32F( srcBuffer, float( value ) / 255 );
		}
		static inline void G8( uint8_t * srcBuffer, uint8_t value )
		{
			G32F( srcBuffer, float( value ) / 255 );
		}
		static inline void B8( uint8_t * srcBuffer, uint8_t value )
		{
			B32F( srcBuffer, float( value ) / 255 );
		}
		static inline void A8( uint8_t * srcBuffer, uint8_t value )
		{
			A32F( srcBuffer, float( value ) / 255 );
		}
		static inline void L32F( uint8_t * srcBuffer, float value )
		{
			R32F( srcBuffer, value );
			G32F( srcBuffer, value );
			B32F( srcBuffer, value );
		}
		static inline void R32F( uint8_t * srcBuffer, float value )
		{
			floatToHalf( reinterpret_cast< uint16_t * >( srcBuffer ) + 0, value );
		}
		static inline void G32F( uint8_t * srcBuffer, float value )
		{
			floatToHalf( reinterpret_cast< uint16_t * >( srcBuffer ) + 1, value );
		}
		static inline void B32F( uint8_t * srcBuffer, float value )
		{
			floatToHalf( reinterpret_cast< uint16_t * >( srcBuffer ) + 2, value );
		}
		static inline void A32F( uint8_t * srcBuffer, float value )
		{
			floatToHalf( reinterpret_cast< uint16_t * >( srcBuffer ) + 3, value );
		}
	};
	//!\~english	Specialisation for PixelFormat::eR32G32B32A32_SFLOAT
	//!\~french		Spécialisation pour PixelFormat::eR32G32B32A32_SFLOAT
	template<>
	struct PixelComponents< PixelFormat::eR32G32B32A32_SFLOAT >
	{
#define src	( reinterpret_cast< float const * >( srcBuffer ) )
#define dst	( reinterpret_cast< float * >( srcBuffer ) )

		static inline uint8_t L8( uint8_t const * srcBuffer )
		{
			return uint8_t( L32F( srcBuffer ) * 255 );
		}
		static inline uint8_t R8( uint8_t const * srcBuffer )
		{
			return uint8_t( R32F( srcBuffer ) * 255 );
		}
		static inline uint8_t G8( uint8_t const * srcBuffer )
		{
			return uint8_t( G32F( srcBuffer ) * 255 );
		}
		static inline uint8_t B8( uint8_t const * srcBuffer )
		{
			return uint8_t( B32F( srcBuffer ) * 255 );
		}
		static inline uint8_t A8( uint8_t const * srcBuffer )
		{
			return uint8_t( A32F( srcBuffer ) * 255 );
		}
		static inline float L32F( uint8_t const * srcBuffer )
		{
			return float( R32F( srcBuffer ) * 0.30 + G32F( srcBuffer ) * 0.59 + B32F( srcBuffer ) * 0.11 );
		}
		static inline float R32F( uint8_t const * srcBuffer )
		{
			return src[0];
		}
		static inline float G32F( uint8_t const * srcBuffer )
		{
			return src[1];
		}
		static inline float B32F( uint8_t const * srcBuffer )
		{
			return src[2];
		}
		static inline float A32F( uint8_t const * srcBuffer )
		{
			return src[3];
		}
		static inline void L8( uint8_t * srcBuffer, uint8_t value )
		{
			L32F( srcBuffer, float( value ) / 255 );
		}
		static inline void R8( uint8_t * srcBuffer, uint8_t value )
		{
			R32F( srcBuffer, float( value ) / 255 );
		}
		static inline void G8( uint8_t * srcBuffer, uint8_t value )
		{
			G32F( srcBuffer, float( value ) / 255 );
		}
		static inline void B8( uint8_t * srcBuffer, uint8_t value )
		{
			B32F( srcBuffer, float( value ) / 255 );
		}
		static inline void A8( uint8_t * srcBuffer, uint8_t value )
		{
			A32F( srcBuffer, float( value ) / 255 );
		}
		static inline void L32F( uint8_t * srcBuffer, float value )
		{
			R32F( srcBuffer, value );
			G32F( srcBuffer, value );
			B32F( srcBuffer, value );
		}
		static inline void R32F( uint8_t * srcBuffer, float value )
		{
			dst[0] = value;
		}
		static inline void G32F( uint8_t * srcBuffer, float value )
		{
			dst[1] = value;
		}
		static inline void B32F( uint8_t * srcBuffer, float value )
		{
			dst[2] = value;
		}
		static inline void A32F( uint8_t * srcBuffer, float value )
		{
			dst[3] = value;
		}

#undef src
#undef dst
	};
	//!\~english	Specialisation for PixelFormat::eD16_UNORM
	//!\~french		Spécialisation pour PixelFormat::eD16_UNORM
	template<>
	struct PixelComponents< PixelFormat::eD16_UNORM >
	{
#define src	( *reinterpret_cast< uint16_t const * >( srcBuffer ) )
#define dst	( *reinterpret_cast< uint16_t * >( srcBuffer ) )

		static inline uint8_t D8( uint8_t const * srcBuffer )
		{
			return uint8_t( src >> 8 );
		}
		static inline uint16_t D16( uint8_t const * srcBuffer )
		{
			return src;
		}
		static inline uint32_t D24( uint8_t const * srcBuffer )
		{
			return ( uint32_t( src ) << 8 );
		}
		static inline uint32_t D32( uint8_t const * srcBuffer )
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
		static inline float D32F( uint8_t const * srcBuffer )
		{
			return float( D32( srcBuffer ) );
		}
		static inline float S32F( uint8_t const * )
		{
			return 0;
		}
		static inline void D8( uint8_t * srcBuffer, uint8_t value )
		{
			dst = ( uint16_t( value ) << 8 );
		}
		static inline void D16( uint8_t * srcBuffer, uint16_t value )
		{
			dst = value;
		}
		static inline void D24( uint8_t * srcBuffer, uint32_t value )
		{
			dst = ( src & 0xFFFF0000 ) | ( 0x0000FFFF & ( value >> 8 ) );
		}
		static inline void D32( uint8_t * srcBuffer, uint32_t value )
		{
			dst = ( src & 0xFFFF0000 ) | ( 0x0000FFFF & ( value >> 16 ) );
		}
		static inline void S1( uint8_t *, uint8_t )
		{
		}
		static inline void S8( uint8_t *, uint8_t )
		{
		}
		static inline void D32F( uint8_t * srcBuffer, float value )
		{
			D32( srcBuffer, uint32_t( value ) );
		}
		static inline void S32F( uint8_t *, float )
		{
		}
		static inline uint8_t L8( uint8_t const * srcBuffer )
		{
			return D8( srcBuffer );
		}
		static inline uint8_t A8( uint8_t const * srcBuffer )
		{
			return 0;
		}
		static inline uint8_t R8( uint8_t const * srcBuffer )
		{
			return L8( srcBuffer );
		}
		static inline uint8_t G8( uint8_t const * srcBuffer )
		{
			return L8( srcBuffer );
		}
		static inline uint8_t B8( uint8_t const * srcBuffer )
		{
			return L8( srcBuffer );
		}
		static inline float L32F( uint8_t const * srcBuffer )
		{
			return L8( srcBuffer ) / 255.0f;
		}
		static inline float R32F( uint8_t const * srcBuffer )
		{
			return R8( srcBuffer ) / 255.0f;
		}
		static inline float G32F( uint8_t const * srcBuffer )
		{
			return G8( srcBuffer ) / 255.0f;
		}
		static inline float B32F( uint8_t const * srcBuffer )
		{
			return B8( srcBuffer ) / 255.0f;
		}
		static inline float A32F( uint8_t const * srcBuffer )
		{
			return A8( srcBuffer ) / 255.0f;
		}
		static inline void L8( uint8_t * srcBuffer, uint8_t value )
		{
			D8( srcBuffer, value );
		}
		static inline void A8( uint8_t * srcBuffer, uint8_t value )
		{
		}
		static inline void R8( uint8_t * srcBuffer, uint8_t value )
		{
			L8( srcBuffer, value );
		}
		static inline void G8( uint8_t * srcBuffer, uint8_t value )
		{
			L8( srcBuffer, value );
		}
		static inline void B8( uint8_t * srcBuffer, uint8_t value )
		{
			L8( srcBuffer, value );
		}
		static inline void L32F( uint8_t * srcBuffer, float value )
		{
			L8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void R32F( uint8_t * srcBuffer, float value )
		{
			R8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void G32F( uint8_t * srcBuffer, float value )
		{
			G8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void B32F( uint8_t * srcBuffer, float value )
		{
			B8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void A32F( uint8_t * srcBuffer, float value )
		{
			A8( srcBuffer, uint8_t( value * 255.0 ) );
		}

#undef src
#undef dst
	};
	//!\~english	Specialisation for PixelFormat::eD24_UNORM_S8_UINT
	//!\~french		Spécialisation pour PixelFormat::eD24_UNORM_S8_UINT
	template<>
	struct PixelComponents< PixelFormat::eD24_UNORM_S8_UINT >
	{
#define src	( *reinterpret_cast< uint32_t const * >( srcBuffer ) )
#define dst	( *reinterpret_cast< uint32_t * >( srcBuffer ) )

		static inline uint8_t	BIT( uint8_t const  p_bySrc )
		{
			return ( ( p_bySrc >> 0 ) & 0x01 ) | ( ( p_bySrc >> 1 ) & 0x01 ) | ( ( p_bySrc >> 2 ) & 0x01 ) | ( ( p_bySrc >> 3 ) & 0x01 ) | ( ( p_bySrc >> 4 ) & 0x01 ) | ( ( p_bySrc >> 5 ) & 0x01 ) | ( ( p_bySrc >> 6 ) & 0x01 ) | ( ( p_bySrc >> 7 ) & 0x01 );
		}
		static inline uint8_t D8( uint8_t const * srcBuffer )
		{
			return uint8_t( ( src & 0xFF000000 ) >> 24 );
		}
		static inline uint16_t D16( uint8_t const * srcBuffer )
		{
			return uint16_t( ( src & 0xFFFF0000 ) >> 16 );
		}
		static inline uint32_t D24( uint8_t const * srcBuffer )
		{
			return uint32_t( ( src & 0xFFFFFF00 ) >> 8 );
		}
		static inline uint32_t D32( uint8_t const * srcBuffer )
		{
			return uint32_t( ( src & 0xFFFFFF00 ) >> 8 );
		}
		static inline uint8_t	S1( uint8_t const * srcBuffer )
		{
			return ( BIT( srcBuffer[3] ) << 0 ) | ( BIT( srcBuffer[7] ) << 1 ) | ( BIT( srcBuffer[11] ) << 2 ) | ( BIT( srcBuffer[15] ) << 3 ) | ( BIT( srcBuffer[19] ) << 4 ) | ( BIT( srcBuffer[23] ) << 5 ) | ( BIT( srcBuffer[27] ) << 6 ) | ( BIT( srcBuffer[31] ) << 7 );
		}
		static inline uint8_t S8( uint8_t const * srcBuffer )
		{
			return uint8_t( src & 0x000000FF );
		}
		static inline float D32F( uint8_t const * srcBuffer )
		{
			return float( D32( srcBuffer ) );
		}
		static inline float S32F( uint8_t const * srcBuffer )
		{
			return float( S8( srcBuffer ) ) / 255.0f;
		}
		static inline void D8( uint8_t * srcBuffer, uint8_t value )
		{
			dst = ( src & 0x000000FF ) | ( ( uint32_t( value ) << 24 ) & 0xFF000000 );
		}
		static inline void D16( uint8_t * srcBuffer, uint16_t value )
		{
			dst = ( src & 0x000000FF ) | ( ( uint32_t( value ) << 16 ) & 0xFFFF0000 );
		}
		static inline void D24( uint8_t * srcBuffer, uint32_t value )
		{
			dst = ( src & 0x000000FF ) | ( ( value << 8 ) & 0xFFFFFF00 );
		}
		static inline void D32( uint8_t * srcBuffer, uint32_t value )
		{
			dst = ( src & 0x000000FF ) | ( value & 0xFFFFFF00 );
		}
		static inline void S1( uint8_t * srcBuffer, uint8_t value )
		{
		}
		static inline void S8( uint8_t * srcBuffer, uint8_t value )
		{
			dst = ( src & 0xFFFFFF00 ) | ( value & 0x000000FF );
		}
		static inline void D32F( uint8_t * srcBuffer, float value )
		{
			D32( srcBuffer, uint32_t( value ) );
		}
		static inline void S32F( uint8_t * srcBuffer, float value )
		{
			S8( srcBuffer, uint8_t( value * 255.0f ) );
		}
		static inline uint8_t L8( uint8_t const * srcBuffer )
		{
			return D8( srcBuffer );
		}
		static inline uint8_t A8( uint8_t const * srcBuffer )
		{
			return S8( srcBuffer );
		}
		static inline uint8_t R8( uint8_t const * srcBuffer )
		{
			return L8( srcBuffer );
		}
		static inline uint8_t G8( uint8_t const * srcBuffer )
		{
			return L8( srcBuffer );
		}
		static inline uint8_t B8( uint8_t const * srcBuffer )
		{
			return L8( srcBuffer );
		}
		static inline float L32F( uint8_t const * srcBuffer )
		{
			return L8( srcBuffer ) / 255.0f;
		}
		static inline float R32F( uint8_t const * srcBuffer )
		{
			return R8( srcBuffer ) / 255.0f;
		}
		static inline float G32F( uint8_t const * srcBuffer )
		{
			return G8( srcBuffer ) / 255.0f;
		}
		static inline float B32F( uint8_t const * srcBuffer )
		{
			return B8( srcBuffer ) / 255.0f;
		}
		static inline float A32F( uint8_t const * srcBuffer )
		{
			return A8( srcBuffer ) / 255.0f;
		}
		static inline void L8( uint8_t * srcBuffer, uint8_t value )
		{
			D8( srcBuffer, value );
		}
		static inline void A8( uint8_t * srcBuffer, uint8_t value )
		{
			S8( srcBuffer, value );
		}
		static inline void R8( uint8_t * srcBuffer, uint8_t value )
		{
			L8( srcBuffer, value );
		}
		static inline void G8( uint8_t * srcBuffer, uint8_t value )
		{
			L8( srcBuffer, value );
		}
		static inline void B8( uint8_t * srcBuffer, uint8_t value )
		{
			L8( srcBuffer, value );
		}
		static inline void L32F( uint8_t * srcBuffer, float value )
		{
			L8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void R32F( uint8_t * srcBuffer, float value )
		{
			R8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void G32F( uint8_t * srcBuffer, float value )
		{
			G8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void B32F( uint8_t * srcBuffer, float value )
		{
			B8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void A32F( uint8_t * srcBuffer, float value )
		{
			A8( srcBuffer, uint8_t( value * 255.0 ) );
		}

#undef src
#undef dst
	};
	//!\~english	Specialisation for PixelFormat::eD32_UNORM
	//!\~french		Spécialisation pour PixelFormat::eD32_UNORM
	template<>
	struct PixelComponents< PixelFormat::eD32_UNORM >
	{
#define src	( *reinterpret_cast< uint32_t const * >( srcBuffer ) )
#define dst	( *reinterpret_cast< uint32_t * >( srcBuffer ) )

		static inline uint8_t D8( uint8_t const * srcBuffer )
		{
			return uint8_t( src >> 24 );
		}
		static inline uint16_t D16( uint8_t const * srcBuffer )
		{
			return uint16_t( src >> 16 );
		}
		static inline uint32_t D24( uint8_t const * srcBuffer )
		{
			return ( src >> 8 );
		}
		static inline uint32_t D32( uint8_t const * srcBuffer )
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
		static inline float D32F( uint8_t const * srcBuffer )
		{
			return float( D32( srcBuffer ) );
		}
		static inline float S32F( uint8_t const * )
		{
			return 0;
		}
		static inline void D8( uint8_t * srcBuffer, uint8_t value )
		{
			dst = ( uint32_t( value ) << 24 );
		}
		static inline void D16( uint8_t * srcBuffer, uint16_t value )
		{
			dst = ( uint32_t( value ) << 16 );
		}
		static inline void D24( uint8_t * srcBuffer, uint32_t value )
		{
			dst = ( value << 8 );
		}
		static inline void D32( uint8_t * srcBuffer, uint32_t value )
		{
			dst = value;
		}
		static inline void S1( uint8_t *, uint8_t )
		{
		}
		static inline void S8( uint8_t *, uint8_t )
		{
		}
		static inline void D32F( uint8_t * srcBuffer, float value )
		{
			D32( srcBuffer, uint32_t( value ) );
		}
		static inline void S32F( uint8_t *, float )
		{
		}
		static inline uint8_t L8( uint8_t const * srcBuffer )
		{
			return D8( srcBuffer );
		}
		static inline uint8_t A8( uint8_t const * srcBuffer )
		{
			return 0xFF;
		}
		static inline uint8_t R8( uint8_t const * srcBuffer )
		{
			return L8( srcBuffer );
		}
		static inline uint8_t G8( uint8_t const * srcBuffer )
		{
			return L8( srcBuffer );
		}
		static inline uint8_t B8( uint8_t const * srcBuffer )
		{
			return L8( srcBuffer );
		}
		static inline float L32F( uint8_t const * srcBuffer )
		{
			return L8( srcBuffer ) / 255.0f;
		}
		static inline float R32F( uint8_t const * srcBuffer )
		{
			return R8( srcBuffer ) / 255.0f;
		}
		static inline float G32F( uint8_t const * srcBuffer )
		{
			return G8( srcBuffer ) / 255.0f;
		}
		static inline float B32F( uint8_t const * srcBuffer )
		{
			return B8( srcBuffer ) / 255.0f;
		}
		static inline float A32F( uint8_t const * srcBuffer )
		{
			return A8( srcBuffer ) / 255.0f;
		}
		static inline void L8( uint8_t * srcBuffer, uint8_t value )
		{
			D8( srcBuffer, value );
		}
		static inline void A8( uint8_t * srcBuffer, uint8_t value )
		{
		}
		static inline void R8( uint8_t * srcBuffer, uint8_t value )
		{
			L8( srcBuffer, value );
		}
		static inline void G8( uint8_t * srcBuffer, uint8_t value )
		{
			L8( srcBuffer, value );
		}
		static inline void B8( uint8_t * srcBuffer, uint8_t value )
		{
			L8( srcBuffer, value );
		}
		static inline void L32F( uint8_t * srcBuffer, float value )
		{
			L8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void R32F( uint8_t * srcBuffer, float value )
		{
			R8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void G32F( uint8_t * srcBuffer, float value )
		{
			G8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void B32F( uint8_t * srcBuffer, float value )
		{
			B8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void A32F( uint8_t * srcBuffer, float value )
		{
			A8( srcBuffer, uint8_t( value * 255.0 ) );
		}

#undef src
#undef dst
	};
	//!\~english	Specialisation for PixelFormat::eD32_SFLOAT
	//!\~french		Spécialisation pour PixelFormat::eD32_SFLOAT
	template<>
	struct PixelComponents< PixelFormat::eD32_SFLOAT >
	{
#define src	( *reinterpret_cast< float const * >( srcBuffer ) )
#define dst	( *reinterpret_cast< float * >( srcBuffer ) )

		static inline float D32F( uint8_t const * srcBuffer )
		{
			return src;
		}
		static inline float S32F( uint8_t const * )
		{
			return 0;
		}
		static inline uint8_t D8( uint8_t const * srcBuffer )
		{
			return uint8_t( src * 256 );
		}
		static inline uint16_t D16( uint8_t const * srcBuffer )
		{
			return uint8_t( src * 65536 );
		}
		static inline uint32_t D24( uint8_t const * srcBuffer )
		{
			return uint32_t( src * 65536 * 256 );
		}
		static inline uint32_t D32( uint8_t const * srcBuffer )
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
		static inline void D32F( uint8_t * srcBuffer, float value )
		{
			dst = value;
		}
		static inline void S32F( uint8_t *, float )
		{
		}
		static inline void D8( uint8_t * srcBuffer, uint8_t value )
		{
			dst = float( value ) / float( std::numeric_limits< uint8_t >::max() );
		}
		static inline void D16( uint8_t * srcBuffer, uint16_t value )
		{
			dst = float( value ) / float( std::numeric_limits< uint16_t >::max() );
		}
		static inline void D24( uint8_t * srcBuffer, uint32_t value )
		{
			dst = float( value ) / float( std::numeric_limits< uint16_t >::max() * std::numeric_limits< uint8_t >::max() );
		}
		static inline void D32( uint8_t * srcBuffer, uint32_t value )
		{
			dst = float( value ) / float( std::numeric_limits< uint32_t >::max() );
		}
		static inline void S1( uint8_t *, uint8_t )
		{
		}
		static inline void S8( uint8_t *, uint8_t )
		{
		}
		static inline uint8_t L8( uint8_t const * srcBuffer )
		{
			return uint8_t( L32F( srcBuffer ) * 255 );
		}
		static inline uint8_t A8( uint8_t const * srcBuffer )
		{
			return 0xFF;
		}
		static inline uint8_t R8( uint8_t const * srcBuffer )
		{
			return L8( srcBuffer );
		}
		static inline uint8_t G8( uint8_t const * srcBuffer )
		{
			return L8( srcBuffer );
		}
		static inline uint8_t B8( uint8_t const * srcBuffer )
		{
			return L8( srcBuffer );
		}
		static inline float L32F( uint8_t const * srcBuffer )
		{
			return src;
		}
		static inline float R32F( uint8_t const * srcBuffer )
		{
			return L32F( srcBuffer );
		}
		static inline float G32F( uint8_t const * srcBuffer )
		{
			return L32F( srcBuffer );
		}
		static inline float B32F( uint8_t const * srcBuffer )
		{
			return L32F( srcBuffer );
		}
		static inline float A32F( uint8_t const * srcBuffer )
		{
			return 1.0f;
		}
		static inline void L8( uint8_t * srcBuffer, uint8_t value )
		{
			D8( srcBuffer, value );
		}
		static inline void A8( uint8_t * srcBuffer, uint8_t value )
		{
		}
		static inline void R8( uint8_t * srcBuffer, uint8_t value )
		{
			L8( srcBuffer, value );
		}
		static inline void G8( uint8_t * srcBuffer, uint8_t value )
		{
			L8( srcBuffer, value );
		}
		static inline void B8( uint8_t * srcBuffer, uint8_t value )
		{
			L8( srcBuffer, value );
		}
		static inline void L32F( uint8_t * srcBuffer, float value )
		{
			dst = value;
		}
		static inline void R32F( uint8_t * srcBuffer, float value )
		{
			L32F( srcBuffer, value );
		}
		static inline void G32F( uint8_t * srcBuffer, float value )
		{
			L32F( srcBuffer, value );
		}
		static inline void B32F( uint8_t * srcBuffer, float value )
		{
			L32F( srcBuffer, value );
		}
		static inline void A32F( uint8_t * srcBuffer, float value )
		{
			L32F( srcBuffer, value );
		}

#undef src
#undef dst
	};
	//!\~english	Specialisation for PixelFormat::eD32_SFLOAT_S8_UINT
	//!\~french		Spécialisation pour PixelFormat::eD32_SFLOAT_S8_UINT
	template<>
	struct PixelComponents< PixelFormat::eD32_SFLOAT_S8_UINT >
	{
#define src	( *reinterpret_cast< float const * >( srcBuffer ) )
#define dst	( *reinterpret_cast< float * >( srcBuffer ) )

		static inline float D32F( uint8_t const * srcBuffer )
		{
			return src;
		}
		static inline float S32F( uint8_t const * )
		{
			return 0;
		}
		static inline uint8_t D8( uint8_t const * srcBuffer )
		{
			return uint8_t( src * 256 );
		}
		static inline uint16_t D16( uint8_t const * srcBuffer )
		{
			return uint8_t( src * 65536 );
		}
		static inline uint32_t D24( uint8_t const * srcBuffer )
		{
			return uint32_t( src * 65536 * 256 );
		}
		static inline uint32_t D32( uint8_t const * srcBuffer )
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
		static inline void D32F( uint8_t * srcBuffer, float value )
		{
			dst = value;
		}
		static inline void S32F( uint8_t *, float )
		{
		}
		static inline void D8( uint8_t * srcBuffer, uint8_t value )
		{
			dst = float( value ) / float( std::numeric_limits< uint8_t >::max() );
		}
		static inline void D16( uint8_t * srcBuffer, uint16_t value )
		{
			dst = float( value ) / float( std::numeric_limits< uint16_t >::max() );
		}
		static inline void D24( uint8_t * srcBuffer, uint32_t value )
		{
			dst = float( value ) / float( std::numeric_limits< uint16_t >::max() * std::numeric_limits< uint8_t >::max() );
		}
		static inline void D32( uint8_t * srcBuffer, uint32_t value )
		{
			dst = float( value ) / float( std::numeric_limits< uint32_t >::max() );
		}
		static inline void S1( uint8_t *, uint8_t )
		{
		}
		static inline void S8( uint8_t *, uint8_t )
		{
		}
		static inline uint8_t L8( uint8_t const * srcBuffer )
		{
			return uint8_t( L32F( srcBuffer ) * 255 );
		}
		static inline uint8_t A8( uint8_t const * srcBuffer )
		{
			return 0xFF;
		}
		static inline uint8_t R8( uint8_t const * srcBuffer )
		{
			return L8( srcBuffer );
		}
		static inline uint8_t G8( uint8_t const * srcBuffer )
		{
			return L8( srcBuffer );
		}
		static inline uint8_t B8( uint8_t const * srcBuffer )
		{
			return L8( srcBuffer );
		}
		static inline float L32F( uint8_t const * srcBuffer )
		{
			return src;
		}
		static inline float R32F( uint8_t const * srcBuffer )
		{
			return L32F( srcBuffer );
		}
		static inline float G32F( uint8_t const * srcBuffer )
		{
			return L32F( srcBuffer );
		}
		static inline float B32F( uint8_t const * srcBuffer )
		{
			return L32F( srcBuffer );
		}
		static inline float A32F( uint8_t const * srcBuffer )
		{
			return 1.0f;
		}
		static inline void L8( uint8_t * srcBuffer, uint8_t value )
		{
			D8( srcBuffer, value );
		}
		static inline void A8( uint8_t * srcBuffer, uint8_t value )
		{
		}
		static inline void R8( uint8_t * srcBuffer, uint8_t value )
		{
			L8( srcBuffer, value );
		}
		static inline void G8( uint8_t * srcBuffer, uint8_t value )
		{
			L8( srcBuffer, value );
		}
		static inline void B8( uint8_t * srcBuffer, uint8_t value )
		{
			L8( srcBuffer, value );
		}
		static inline void L32F( uint8_t * srcBuffer, float value )
		{
			dst = value;
		}
		static inline void R32F( uint8_t * srcBuffer, float value )
		{
			L32F( srcBuffer, value );
		}
		static inline void G32F( uint8_t * srcBuffer, float value )
		{
			L32F( srcBuffer, value );
		}
		static inline void B32F( uint8_t * srcBuffer, float value )
		{
			L32F( srcBuffer, value );
		}
		static inline void A32F( uint8_t * srcBuffer, float value )
		{
			L32F( srcBuffer, value );
		}

#undef src
#undef dst
	};
	//!\~english	Specialisation for PixelFormat::eS8_UINT
	//!\~french		Spécialisation pour PixelFormat::eS8_UINT
	template<>
	struct PixelComponents< PixelFormat::eS8_UINT >
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
		static inline uint8_t S1( uint8_t const * srcBuffer )
		{
			return ( BIT( srcBuffer[3] ) << 0 ) | ( BIT( srcBuffer[7] ) << 1 ) | ( BIT( srcBuffer[11] ) << 2 ) | ( BIT( srcBuffer[15] ) << 3 ) | ( BIT( srcBuffer[19] ) << 4 ) | ( BIT( srcBuffer[23] ) << 5 ) | ( BIT( srcBuffer[27] ) << 6 ) | ( BIT( srcBuffer[31] ) << 7 );
		}
		static inline uint8_t S8( uint8_t const * srcBuffer )
		{
			return *srcBuffer;
		}
		static inline float D32F( uint8_t const * )
		{
			return 0;
		}
		static inline float S32F( uint8_t const * srcBuffer )
		{
			return float( S8( srcBuffer ) ) / 255.0f;
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
		static inline void S1( uint8_t * srcBuffer, uint8_t value )
		{
		}
		static inline void S8( uint8_t * srcBuffer, uint8_t value )
		{
			*srcBuffer = value;
		}
		static inline void D32F( uint8_t *, float )
		{
		}
		static inline void S32F( uint8_t * srcBuffer, float value )
		{
			S8( srcBuffer, uint8_t( value * 255.0f ) );
		}
		static inline uint8_t L8( uint8_t const * srcBuffer )
		{
			return 0;
		}
		static inline uint8_t A8( uint8_t const * srcBuffer )
		{
			return S8( srcBuffer );
		}
		static inline uint8_t R8( uint8_t const * srcBuffer )
		{
			return L8( srcBuffer );
		}
		static inline uint8_t G8( uint8_t const * srcBuffer )
		{
			return L8( srcBuffer );
		}
		static inline uint8_t B8( uint8_t const * srcBuffer )
		{
			return L8( srcBuffer );
		}
		static inline float L32F( uint8_t const * srcBuffer )
		{
			return L8( srcBuffer ) / 255.0f;
		}
		static inline float R32F( uint8_t const * srcBuffer )
		{
			return R8( srcBuffer ) / 255.0f;
		}
		static inline float G32F( uint8_t const * srcBuffer )
		{
			return G8( srcBuffer ) / 255.0f;
		}
		static inline float B32F( uint8_t const * srcBuffer )
		{
			return B8( srcBuffer ) / 255.0f;
		}
		static inline float A32F( uint8_t const * srcBuffer )
		{
			return A8( srcBuffer ) / 255.0f;
		}
		static inline void L8( uint8_t * srcBuffer, uint8_t value )
		{
		}
		static inline void A8( uint8_t * srcBuffer, uint8_t value )
		{
			S8( srcBuffer, value );
		}
		static inline void R8( uint8_t * srcBuffer, uint8_t value )
		{
			L8( srcBuffer, value );
		}
		static inline void G8( uint8_t * srcBuffer, uint8_t value )
		{
			L8( srcBuffer, value );
		}
		static inline void B8( uint8_t * srcBuffer, uint8_t value )
		{
			L8( srcBuffer, value );
		}
		static inline void L32F( uint8_t * srcBuffer, float value )
		{
			L8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void R32F( uint8_t * srcBuffer, float value )
		{
			R8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void G32F( uint8_t * srcBuffer, float value )
		{
			G8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void B32F( uint8_t * srcBuffer, float value )
		{
			B8( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static inline void A32F( uint8_t * srcBuffer, float value )
		{
			A8( srcBuffer, uint8_t( value * 255.0 ) );
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
		template< PixelFormat PFSrc, PixelFormat PFDst, typename Enable = void >
		struct PixelConverter;
		//!\~english	Specialisation when source and destination formats are the same
		//!\~french		Spécialisation quand les formats source et destination sont identiques
		template< PixelFormat PFSrc, PixelFormat PFDst >
		struct PixelConverter< PFSrc, PFDst, typename std::enable_if< PFSrc == PFDst >::type >
		{
			inline void operator()( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
			{
				std::memcpy( dstBuffer, srcBuffer, PixelDefinitions< PFSrc >::Size );
				srcBuffer += PixelDefinitions< PFSrc >::Size;
				dstBuffer += PixelDefinitions< PFDst >::Size;
			}
		};
		//!\~english	Specialisation for converting to PixelFormat::eR8_UNORM
		//!\~french		Spécialisation pour convertir vers PixelFormat::eR8_UNORM
		template< PixelFormat PFSrc, PixelFormat PFDst >
		struct PixelConverter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == PixelFormat::eR8_UNORM >::type >
		{
			inline void operator()( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
			{
				PixelComponents< PFDst >::L8( dstBuffer, PixelComponents< PFSrc >::L8( srcBuffer ) );
				srcBuffer += PixelDefinitions< PFSrc >::Size;
				dstBuffer += PixelDefinitions< PFDst >::Size;
			}
		};
		//!\~english	Specialisation for converting to PixelFormat::eR16_SFLOAT
		//!\~french		Spécialisation pour convertir vers PixelFormat::eR16_SFLOAT
		template< PixelFormat PFSrc, PixelFormat PFDst >
		struct PixelConverter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == PixelFormat::eR16_SFLOAT >::type >
		{
			inline void operator()( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
			{
				srcBuffer += PixelDefinitions< PFSrc >::Size;
				dstBuffer += PixelDefinitions< PFDst >::Size;
			}
		};
		//!\~english	Specialisation for converting to PixelFormat::eR32_SFLOAT
		//!\~french		Spécialisation pour convertir vers PixelFormat::eR32_SFLOAT
		template< PixelFormat PFSrc, PixelFormat PFDst >
		struct PixelConverter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == PixelFormat::eR32_SFLOAT >::type >
		{
			inline void operator()( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
			{
				PixelComponents< PFDst >::L32F( dstBuffer, PixelComponents< PFSrc >::L32F( srcBuffer ) );
				srcBuffer += PixelDefinitions< PFSrc >::Size;
				dstBuffer += PixelDefinitions< PFDst >::Size;
			}
		};
		//!\~english	Specialisation for converting to PixelFormat::eR8A8_UNORM
		//!\~french		Spécialisation pour convertir vers PixelFormat::eR8A8_UNORM
		template< PixelFormat PFSrc, PixelFormat PFDst >
		struct PixelConverter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == PixelFormat::eR8A8_UNORM >::type >
		{
			inline void operator()( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
			{
				PixelComponents< PFDst >::L8( dstBuffer, PixelComponents< PFSrc >::L8( srcBuffer ) );
				PixelComponents< PFDst >::A8( dstBuffer, PixelComponents< PFSrc >::A8( srcBuffer ) );
				srcBuffer += PixelDefinitions< PFSrc >::Size;
				dstBuffer += PixelDefinitions< PFDst >::Size;
			}
		};
		//!\~english	Specialisation for converting to PixelFormat::eR16A16_SFLOAT
		//!\~french		Spécialisation pour convertir vers PixelFormat::eR16A16_SFLOAT
		template< PixelFormat PFSrc, PixelFormat PFDst >
		struct PixelConverter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == PixelFormat::eR16A16_SFLOAT >::type >
		{
			inline void operator()( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
			{
				srcBuffer += PixelDefinitions< PFSrc >::Size;
				dstBuffer += PixelDefinitions< PFDst >::Size;
			}
		};
		//!\~english	Specialisation for converting to PixelFormat::eR32A32_SFLOAT
		//!\~french		Spécialisation pour convertir vers PixelFormat::eR32A32_SFLOAT
		template< PixelFormat PFSrc, PixelFormat PFDst >
		struct PixelConverter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == PixelFormat::eR32A32_SFLOAT >::type >
		{
			inline void operator()( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
			{
				PixelComponents< PFDst >::L32F( dstBuffer, PixelComponents< PFSrc >::L32F( srcBuffer ) );
				PixelComponents< PFDst >::A32F( dstBuffer, PixelComponents< PFSrc >::A32F( srcBuffer ) );
				srcBuffer += PixelDefinitions< PFSrc >::Size;
				dstBuffer += PixelDefinitions< PFDst >::Size;
			}
		};
		//!\~english	Specialisation for converting to PixelFormat::eR5G5B5A1_UNORM
		//!\~french		Spécialisation pour convertir vers PixelFormat::eR5G5B5A1_UNORM
		template< PixelFormat PFSrc, PixelFormat PFDst >
		struct PixelConverter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == PixelFormat::eR5G5B5A1_UNORM >::type >
		{
			inline void operator()( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
			{
				PixelComponents< PFDst >::A8( dstBuffer, PixelComponents< PFSrc >::A8( srcBuffer ) );
				PixelComponents< PFDst >::R8( dstBuffer, PixelComponents< PFSrc >::R8( srcBuffer ) );
				PixelComponents< PFDst >::G8( dstBuffer, PixelComponents< PFSrc >::G8( srcBuffer ) );
				PixelComponents< PFDst >::B8( dstBuffer, PixelComponents< PFSrc >::B8( srcBuffer ) );
				srcBuffer += PixelDefinitions< PFSrc >::Size;
				dstBuffer += PixelDefinitions< PFDst >::Size;
			}
		};
		//!\~english	Specialisation for converting to PixelFormat::eR5G6B5_UNORM
		//!\~french		Spécialisation pour convertir vers PixelFormat::eR5G6B5_UNORM
		template< PixelFormat PFSrc, PixelFormat PFDst >
		struct PixelConverter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == PixelFormat::eR5G6B5_UNORM >::type >
		{
			inline void operator()( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
			{
				PixelComponents< PFDst >::R8( dstBuffer, PixelComponents< PFSrc >::R8( srcBuffer ) );
				PixelComponents< PFDst >::G8( dstBuffer, PixelComponents< PFSrc >::G8( srcBuffer ) );
				PixelComponents< PFDst >::B8( dstBuffer, PixelComponents< PFSrc >::B8( srcBuffer ) );
				srcBuffer += PixelDefinitions< PFSrc >::Size;
				dstBuffer += PixelDefinitions< PFDst >::Size;
			}
		};
		//!\~english	Specialisation for converting to PixelFormat::eR8G8B8_UNORM
		//!\~french		Spécialisation pour convertir vers PixelFormat::eR8G8B8_UNORM
		template< PixelFormat PFSrc, PixelFormat PFDst >
		struct PixelConverter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == PixelFormat::eR8G8B8_UNORM >::type >
		{
			inline void operator()( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
			{
				PixelComponents< PFDst >::R8( dstBuffer, PixelComponents< PFSrc >::R8( srcBuffer ) );
				PixelComponents< PFDst >::G8( dstBuffer, PixelComponents< PFSrc >::G8( srcBuffer ) );
				PixelComponents< PFDst >::B8( dstBuffer, PixelComponents< PFSrc >::B8( srcBuffer ) );
				srcBuffer += PixelDefinitions< PFSrc >::Size;
				dstBuffer += PixelDefinitions< PFDst >::Size;
			}
		};
		//!\~english	Specialisation for converting to PixelFormat::eB8G8R8_UNORM
		//!\~french		Spécialisation pour convertir vers PixelFormat::eB8G8R8_UNORM
		template< PixelFormat PFSrc, PixelFormat PFDst >
		struct PixelConverter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == PixelFormat::eB8G8R8_UNORM >::type >
		{
			inline void operator()( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
			{
				PixelComponents< PFDst >::B8( dstBuffer, PixelComponents< PFSrc >::B8( srcBuffer ) );
				PixelComponents< PFDst >::G8( dstBuffer, PixelComponents< PFSrc >::G8( srcBuffer ) );
				PixelComponents< PFDst >::R8( dstBuffer, PixelComponents< PFSrc >::R8( srcBuffer ) );
				srcBuffer += PixelDefinitions< PFSrc >::Size;
				dstBuffer += PixelDefinitions< PFDst >::Size;
			}
		};
		//!\~english	Specialisation for converting to PixelFormat::eR8G8B8_SRGB
		//!\~french		Spécialisation pour convertir vers PixelFormat::eR8G8B8_SRGB
		template< PixelFormat PFSrc, PixelFormat PFDst >
		struct PixelConverter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == PixelFormat::eR8G8B8_SRGB >::type >
		{
			inline void operator()( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
			{
				PixelComponents< PFDst >::R8( dstBuffer, PixelComponents< PFSrc >::R8( srcBuffer ) );
				PixelComponents< PFDst >::G8( dstBuffer, PixelComponents< PFSrc >::G8( srcBuffer ) );
				PixelComponents< PFDst >::B8( dstBuffer, PixelComponents< PFSrc >::B8( srcBuffer ) );
				srcBuffer += PixelDefinitions< PFSrc >::Size;
				dstBuffer += PixelDefinitions< PFDst >::Size;
			}
		};
		//!\~english	Specialisation for converting to PixelFormat::eB8G8R8_SRGB
		//!\~french		Spécialisation pour convertir vers PixelFormat::eB8G8R8_SRGB
		template< PixelFormat PFSrc, PixelFormat PFDst >
		struct PixelConverter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == PixelFormat::eB8G8R8_SRGB >::type >
		{
			inline void operator()( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
			{
				PixelComponents< PFDst >::B8( dstBuffer, PixelComponents< PFSrc >::B8( srcBuffer ) );
				PixelComponents< PFDst >::G8( dstBuffer, PixelComponents< PFSrc >::G8( srcBuffer ) );
				PixelComponents< PFDst >::R8( dstBuffer, PixelComponents< PFSrc >::R8( srcBuffer ) );
				srcBuffer += PixelDefinitions< PFSrc >::Size;
				dstBuffer += PixelDefinitions< PFDst >::Size;
			}
		};
		//!\~english	Specialisation for converting to PixelFormat::eR8G8B8A8_UNORM
		//!\~french		Spécialisation pour convertir vers PixelFormat::eR8G8B8A8_UNORM
		template< PixelFormat PFSrc, PixelFormat PFDst >
		struct PixelConverter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == PixelFormat::eR8G8B8A8_UNORM >::type >
		{
			inline void operator()( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
			{
				PixelComponents< PFDst >::A8( dstBuffer, PixelComponents< PFSrc >::A8( srcBuffer ) );
				PixelComponents< PFDst >::R8( dstBuffer, PixelComponents< PFSrc >::R8( srcBuffer ) );
				PixelComponents< PFDst >::G8( dstBuffer, PixelComponents< PFSrc >::G8( srcBuffer ) );
				PixelComponents< PFDst >::B8( dstBuffer, PixelComponents< PFSrc >::B8( srcBuffer ) );
				srcBuffer += PixelDefinitions< PFSrc >::Size;
				dstBuffer += PixelDefinitions< PFDst >::Size;
			}
		};
		//!\~english	Specialisation for converting to PixelFormat::eR8G8B8A8_UNORM
		//!\~french		Spécialisation pour convertir vers PixelFormat::eR8G8B8A8_UNORM
		template< PixelFormat PFSrc, PixelFormat PFDst >
		struct PixelConverter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == PixelFormat::eA8B8G8R8_UNORM >::type >
		{
			inline void operator()( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
			{
				PixelComponents< PFDst >::A8( dstBuffer, PixelComponents< PFSrc >::A8( srcBuffer ) );
				PixelComponents< PFDst >::B8( dstBuffer, PixelComponents< PFSrc >::B8( srcBuffer ) );
				PixelComponents< PFDst >::G8( dstBuffer, PixelComponents< PFSrc >::G8( srcBuffer ) );
				PixelComponents< PFDst >::R8( dstBuffer, PixelComponents< PFSrc >::R8( srcBuffer ) );
				srcBuffer += PixelDefinitions< PFSrc >::Size;
				dstBuffer += PixelDefinitions< PFDst >::Size;
			}
		};
		//!\~english	Specialisation for converting to PixelFormat::eR8G8B8A8_SRGB
		//!\~french		Spécialisation pour convertir vers PixelFormat::eR8G8B8A8_SRGB
		template< PixelFormat PFSrc, PixelFormat PFDst >
		struct PixelConverter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == PixelFormat::eR8G8B8A8_SRGB >::type >
		{
			inline void operator()( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
			{
				PixelComponents< PFDst >::A8( dstBuffer, PixelComponents< PFSrc >::A8( srcBuffer ) );
				PixelComponents< PFDst >::R8( dstBuffer, PixelComponents< PFSrc >::R8( srcBuffer ) );
				PixelComponents< PFDst >::G8( dstBuffer, PixelComponents< PFSrc >::G8( srcBuffer ) );
				PixelComponents< PFDst >::B8( dstBuffer, PixelComponents< PFSrc >::B8( srcBuffer ) );
				srcBuffer += PixelDefinitions< PFSrc >::Size;
				dstBuffer += PixelDefinitions< PFDst >::Size;
			}
		};
		//!\~english	Specialisation for converting to PixelFormat::eA8B8G8R8_SRGB
		//!\~french		Spécialisation pour convertir vers PixelFormat::eA8B8G8R8_SRGB
		template< PixelFormat PFSrc, PixelFormat PFDst >
		struct PixelConverter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == PixelFormat::eA8B8G8R8_SRGB >::type >
		{
			inline void operator()( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
			{
				PixelComponents< PFDst >::A8( dstBuffer, PixelComponents< PFSrc >::A8( srcBuffer ) );
				PixelComponents< PFDst >::B8( dstBuffer, PixelComponents< PFSrc >::B8( srcBuffer ) );
				PixelComponents< PFDst >::G8( dstBuffer, PixelComponents< PFSrc >::G8( srcBuffer ) );
				PixelComponents< PFDst >::R8( dstBuffer, PixelComponents< PFSrc >::R8( srcBuffer ) );
				srcBuffer += PixelDefinitions< PFSrc >::Size;
				dstBuffer += PixelDefinitions< PFDst >::Size;
			}
		};
		//!\~english	Specialisation for converting to PixelFormat::eR16G16B16_SFLOAT
		//!\~french		Spécialisation pour convertir vers PixelFormat::eR16G16B16_SFLOAT
		template< PixelFormat PFSrc, PixelFormat PFDst >
		struct PixelConverter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == PixelFormat::eR16G16B16_SFLOAT >::type >
		{
			inline void operator()( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
			{
				PixelComponents< PFDst >::R32F( dstBuffer, PixelComponents< PFSrc >::R32F( srcBuffer ) );
				PixelComponents< PFDst >::G32F( dstBuffer, PixelComponents< PFSrc >::G32F( srcBuffer ) );
				PixelComponents< PFDst >::B32F( dstBuffer, PixelComponents< PFSrc >::B32F( srcBuffer ) );
				srcBuffer += PixelDefinitions< PFSrc >::Size;
				dstBuffer += PixelDefinitions< PFDst >::Size;
			}
		};
		//!\~english	Specialisation for converting to PixelFormat::eR32G32B32_SFLOAT
		//!\~french		Spécialisation pour convertir vers PixelFormat::eR32G32B32_SFLOAT
		template< PixelFormat PFSrc, PixelFormat PFDst >
		struct PixelConverter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == PixelFormat::eR32G32B32_SFLOAT >::type >
		{
			inline void operator()( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
			{
				PixelComponents< PFDst >::R32F( dstBuffer, PixelComponents< PFSrc >::R32F( srcBuffer ) );
				PixelComponents< PFDst >::G32F( dstBuffer, PixelComponents< PFSrc >::G32F( srcBuffer ) );
				PixelComponents< PFDst >::B32F( dstBuffer, PixelComponents< PFSrc >::B32F( srcBuffer ) );
				srcBuffer += PixelDefinitions< PFSrc >::Size;
				dstBuffer += PixelDefinitions< PFDst >::Size;
			}
		};
		//!\~english	Specialisation for converting to PixelFormat::eR16G16B16A16_SFLOAT
		//!\~french		Spécialisation pour convertir vers PixelFormat::eR16G16B16A16_SFLOAT
		template< PixelFormat PFSrc, PixelFormat PFDst >
		struct PixelConverter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == PixelFormat::eR16G16B16A16_SFLOAT >::type >
		{
			inline void operator()( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
			{
				PixelComponents< PFDst >::A32F( dstBuffer, PixelComponents< PFSrc >::A32F( srcBuffer ) );
				PixelComponents< PFDst >::R32F( dstBuffer, PixelComponents< PFSrc >::R32F( srcBuffer ) );
				PixelComponents< PFDst >::G32F( dstBuffer, PixelComponents< PFSrc >::G32F( srcBuffer ) );
				PixelComponents< PFDst >::B32F( dstBuffer, PixelComponents< PFSrc >::B32F( srcBuffer ) );
				srcBuffer += PixelDefinitions< PFSrc >::Size;
				dstBuffer += PixelDefinitions< PFDst >::Size;
			}
		};
		//!\~english	Specialisation for converting to PixelFormat::eR32G32B32A32_SFLOAT
		//!\~french		Spécialisation pour convertir vers PixelFormat::eR32G32B32A32_SFLOAT
		template< PixelFormat PFSrc, PixelFormat PFDst >
		struct PixelConverter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == PixelFormat::eR32G32B32A32_SFLOAT >::type >
		{
			inline void operator()( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
			{
				PixelComponents< PFDst >::A32F( dstBuffer, PixelComponents< PFSrc >::A32F( srcBuffer ) );
				PixelComponents< PFDst >::R32F( dstBuffer, PixelComponents< PFSrc >::R32F( srcBuffer ) );
				PixelComponents< PFDst >::G32F( dstBuffer, PixelComponents< PFSrc >::G32F( srcBuffer ) );
				PixelComponents< PFDst >::B32F( dstBuffer, PixelComponents< PFSrc >::B32F( srcBuffer ) );
				srcBuffer += PixelDefinitions< PFSrc >::Size;
				dstBuffer += PixelDefinitions< PFDst >::Size;
			}
		};
		//!\~english	Specialisation for converting to PixelFormat::eD16_UNORM
		//!\~french		Spécialisation pour convertir vers PixelFormat::eD16_UNORM
		template< PixelFormat PFSrc, PixelFormat PFDst >
		struct PixelConverter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == PixelFormat::eD16_UNORM >::type >
		{
			inline void operator()( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
			{
				PixelComponents< PFDst >::D16( dstBuffer, PixelComponents< PFSrc >::D16( srcBuffer ) );
				srcBuffer += PixelDefinitions< PFSrc >::Size;
				dstBuffer += PixelDefinitions< PFDst >::Size;
			}
		};
		//!\~english	Specialisation for converting to PixelFormat::eD24_UNORM_S8_UINT
		//!\~french		Spécialisation pour convertir vers PixelFormat::eD24_UNORM_S8_UINT
		template< PixelFormat PFSrc, PixelFormat PFDst >
		struct PixelConverter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == PixelFormat::eD24_UNORM_S8_UINT >::type >
		{
			inline void operator()( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
			{
				PixelComponents< PFDst >::D24( dstBuffer, PixelComponents< PFSrc >::D24( srcBuffer ) );
				PixelComponents< PFDst >::S8( dstBuffer, PixelComponents< PFSrc >::S8( srcBuffer ) );
				srcBuffer += PixelDefinitions< PFSrc >::Size;
				dstBuffer += PixelDefinitions< PFDst >::Size;
			}
		};
		//!\~english	Specialisation for converting to PixelFormat::eD32_UNORM
		//!\~french		Spécialisation pour convertir vers PixelFormat::eD32_UNORM
		template< PixelFormat PFSrc, PixelFormat PFDst >
		struct PixelConverter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == PixelFormat::eD32_UNORM >::type >
		{
			inline void operator()( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
			{
				PixelComponents< PFDst >::D32( dstBuffer, PixelComponents< PFSrc >::D32( srcBuffer ) );
				srcBuffer += PixelDefinitions< PFSrc >::Size;
				dstBuffer += PixelDefinitions< PFDst >::Size;
			}
		};
		//!\~english	Specialisation for converting to PixelFormat::eD32_SFLOAT
		//!\~french		Spécialisation pour convertir vers PixelFormat::eD32_SFLOAT
		template< PixelFormat PFSrc, PixelFormat PFDst >
		struct PixelConverter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == PixelFormat::eD32_SFLOAT >::type >
		{
			inline void operator()( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
			{
				PixelComponents< PFDst >::D32F( dstBuffer, PixelComponents< PFSrc >::D32F( srcBuffer ) );
				srcBuffer += PixelDefinitions< PFSrc >::Size;
				dstBuffer += PixelDefinitions< PFDst >::Size;
			}
		};
		//!\~english	Specialisation for converting to PixelFormat::eD32_SFLOAT_S8_UINT
		//!\~french		Spécialisation pour convertir vers PixelFormat::eD32_SFLOAT_S8_UINT
		template< PixelFormat PFSrc, PixelFormat PFDst >
		struct PixelConverter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == PixelFormat::eD32_SFLOAT_S8_UINT >::type >
		{
			inline void operator()( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
			{
				PixelComponents< PFDst >::D32F( dstBuffer, PixelComponents< PFSrc >::D32F( srcBuffer ) );
				PixelComponents< PFDst >::S8( dstBuffer, PixelComponents< PFSrc >::S8( srcBuffer ) );
				srcBuffer += PixelDefinitions< PFSrc >::Size;
				dstBuffer += PixelDefinitions< PFDst >::Size;
			}
		};
		//!\~english	Specialisation for converting to PixelFormat::eS8_UINT
		//!\~french		Spécialisation pour convertir vers PixelFormat::eS8_UINT
		template< PixelFormat PFSrc, PixelFormat PFDst >
		struct PixelConverter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == PixelFormat::eS8_UINT >::type >
		{
			inline void operator()( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
			{
				PixelComponents< PFDst >::S8( dstBuffer, PixelComponents< PFSrc >::S8( srcBuffer ) );
				srcBuffer += PixelDefinitions< PFSrc >::Size;
				dstBuffer += PixelDefinitions< PFDst >::Size;
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
		template< PixelFormat PFSrc, PixelFormat PFDst >
		struct BufferConverter
		{
			inline void operator()( uint8_t const * srcBuffer, uint32_t srcSize, uint8_t * dstBuffer, uint32_t dstSize )
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
				CU_UnsupportedError( "No conversion defined" );
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
				CU_UnsupportedError( "No conversion defined" );
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
				CU_UnsupportedError( "No conversion defined" );
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
		void dynamicColourBufferConversion( uint8_t const * srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t * dstBuffer, uint32_t dstSize )
		{
			switch ( dstFormat )
			{
			case PixelFormat::eR8_UNORM:
				BufferConverter< PF, PixelFormat::eR8_UNORM >()( srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eR16_SFLOAT:
				BufferConverter< PF, PixelFormat::eR16_SFLOAT >()( srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eR32_SFLOAT:
				BufferConverter< PF, PixelFormat::eR32_SFLOAT >()( srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eR8A8_UNORM:
				BufferConverter< PF, PixelFormat::eR8A8_UNORM >()( srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eR16A16_SFLOAT:
				BufferConverter< PF, PixelFormat::eR16A16_SFLOAT >()( srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eR32A32_SFLOAT:
				BufferConverter< PF, PixelFormat::eR32A32_SFLOAT >()( srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eR5G5B5A1_UNORM:
				BufferConverter< PF, PixelFormat::eR5G5B5A1_UNORM >()( srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eR5G6B5_UNORM:
				BufferConverter< PF, PixelFormat::eR5G6B5_UNORM >()( srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eR8G8B8_UNORM:
				BufferConverter< PF, PixelFormat::eR8G8B8_UNORM >()( srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eB8G8R8_UNORM:
				BufferConverter< PF, PixelFormat::eB8G8R8_UNORM >()( srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eR8G8B8_SRGB:
				BufferConverter< PF, PixelFormat::eR8G8B8_SRGB >()( srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eB8G8R8_SRGB:
				BufferConverter< PF, PixelFormat::eB8G8R8_SRGB >()( srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eR8G8B8A8_UNORM:
				BufferConverter< PF, PixelFormat::eR8G8B8A8_UNORM >()( srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eA8B8G8R8_UNORM:
				BufferConverter< PF, PixelFormat::eA8B8G8R8_UNORM >()( srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eR8G8B8A8_SRGB:
				BufferConverter< PF, PixelFormat::eR8G8B8A8_SRGB >()( srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eA8B8G8R8_SRGB:
				BufferConverter< PF, PixelFormat::eA8B8G8R8_SRGB >()( srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eR16G16B16_SFLOAT:
				BufferConverter< PF, PixelFormat::eR16G16B16_SFLOAT >()( srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eR16G16B16A16_SFLOAT:
				BufferConverter< PF, PixelFormat::eR16G16B16A16_SFLOAT >()( srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eR32G32B32_SFLOAT:
				BufferConverter< PF, PixelFormat::eR32G32B32_SFLOAT >()( srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eR32G32B32A32_SFLOAT:
				BufferConverter< PF, PixelFormat::eR32G32B32A32_SFLOAT >()( srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			default:
				CU_UnsupportedError( "No conversion defined" );
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
		void dynamicDepthBufferConversion( uint8_t const * srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t * dstBuffer, uint32_t dstSize )
		{
			switch ( dstFormat )
			{
			case PixelFormat::eD16_UNORM:
				BufferConverter< PF, PixelFormat::eD16_UNORM >()( srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eD24_UNORM_S8_UINT:
				BufferConverter< PF, PixelFormat::eD24_UNORM_S8_UINT >()( srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eD32_UNORM:
				BufferConverter< PF, PixelFormat::eD32_UNORM >()( srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eD32_SFLOAT:
				BufferConverter< PF, PixelFormat::eD32_SFLOAT >()( srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eD32_SFLOAT_S8_UINT:
				BufferConverter< PF, PixelFormat::eD32_SFLOAT_S8_UINT >()( srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eR8G8B8A8_UNORM:
				BufferConverter< PF, PixelFormat::eR8G8B8A8_UNORM >()( srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eR32G32B32_SFLOAT:
				BufferConverter< PF, PixelFormat::eR32G32B32_SFLOAT >()( srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eR8_UNORM:
				BufferConverter< PF, PixelFormat::eR8_UNORM >()( srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eS8_UINT:
				BufferConverter< PF, PixelFormat::eS8_UINT >()( srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			default:
				CU_UnsupportedError( "No conversion defined" );
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
		void dynamicStencilBufferConversion( uint8_t const * srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t * dstBuffer, uint32_t dstSize )
		{
			switch ( dstFormat )
			{
			case PixelFormat::eD24_UNORM_S8_UINT:
				BufferConverter< PF, PixelFormat::eD24_UNORM_S8_UINT >()( srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eS8_UINT:
				BufferConverter< PF, PixelFormat::eS8_UINT >()( srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			default:
				CU_UnsupportedError( "No conversion defined" );
				break;
			}
		}
	}

	//*************************************************************************************************

	namespace PF
	{
		namespace
		{
			template< PixelFormat PF, typename Enable = void > struct PixelComponentTraits;

			template< PixelFormat PF >
			struct PixelComponentTraits< PF, typename std::enable_if< IsColourFormat< PF >::value >::type >
			{
				static float getFloat( Pixel< PF > const & pixel, PixelComponent component )
				{
					float result = 0.0f;

					switch ( component )
					{
					case PixelComponent::eRed:
						result = PixelComponents< PF >::R32F( pixel.constPtr() );
						break;

					case PixelComponent::eGreen:
						result = PixelComponents< PF >::G32F( pixel.constPtr() );
						break;

					case PixelComponent::eBlue:
						result = PixelComponents< PF >::B32F( pixel.constPtr() );
						break;

					case PixelComponent::eAlpha:
						result = PixelComponents< PF >::A32F( pixel.constPtr() );
						break;

					case PixelComponent::eLuminance:
						result = PixelComponents< PF >::L32F( pixel.constPtr() );
						break;

					default:
						result = 0;
						break;
					}

					return result;
				}

				static void setFloat( Pixel< PF > & pixel, PixelComponent component, float value )
				{
					switch ( component )
					{
					case PixelComponent::eRed:
						PixelComponents< PF >::R8( pixel.ptr(), value );
						break;

					case PixelComponent::eGreen:
						PixelComponents< PF >::G8( pixel.ptr(), value );
						break;

					case PixelComponent::eBlue:
						PixelComponents< PF >::B8( pixel.ptr(), value );
						break;

					case PixelComponent::eAlpha:
						PixelComponents< PF >::A8( pixel.ptr(), value );
						break;

					case PixelComponent::eLuminance:
						PixelComponents< PF >::L8( pixel.ptr(), value );
						break;

					default:
						break;
					}
				}

				static uint8_t getByte( Pixel< PF > const & pixel, PixelComponent component )
				{
					uint8_t result = 0;

					switch ( component )
					{
					case PixelComponent::eRed:
						result = PixelComponents< PF >::R8( pixel.constPtr() );
						break;

					case PixelComponent::eGreen:
						result = PixelComponents< PF >::G8( pixel.constPtr() );
						break;

					case PixelComponent::eBlue:
						result = PixelComponents< PF >::B8( pixel.constPtr() );
						break;

					case PixelComponent::eAlpha:
						result = PixelComponents< PF >::A8( pixel.constPtr() );
						break;

					case PixelComponent::eLuminance:
						result = PixelComponents< PF >::L8( pixel.constPtr() );
						break;

					default:
						result = 0;
						break;
					}

					return result;
				}

				static void setByte( Pixel< PF > & pixel, PixelComponent component, uint8_t value )
				{
					switch ( component )
					{
					case PixelComponent::eRed:
						PixelComponents< PF >::R8( pixel.ptr(), value );
						break;

					case PixelComponent::eGreen:
						PixelComponents< PF >::G8( pixel.ptr(), value );
						break;

					case PixelComponent::eBlue:
						PixelComponents< PF >::B8( pixel.ptr(), value );
						break;

					case PixelComponent::eAlpha:
						PixelComponents< PF >::A8( pixel.ptr(), value );
						break;

					case PixelComponent::eLuminance:
						PixelComponents< PF >::L8( pixel.ptr(), value );
						break;

					default:
						break;
					}
				}
			};

			template< PixelFormat PF >
			struct PixelComponentTraits< PF, typename std::enable_if< IsDepthStencilFormat< PF >::value >::type >
			{
				static float getFloat( Pixel< PF > const & pixel, PixelComponent component )
				{
					float result = 0.0f;

					switch ( component )
					{
					case PixelComponent::eDepth:
						result = float( PixelComponents< PF >::D32F( pixel.constPtr() ) );
						break;

					case PixelComponent::eStencil:
						result = float( PixelComponents< PF >::S32F( pixel.constPtr() ) );
						break;

					default:
						result = 0;
						break;
					}

					return result;
				}

				static void setFloat( Pixel< PF > & pixel, PixelComponent component, float value )
				{
					switch ( component )
					{
					case PixelComponent::eDepth:
						PixelComponents< PF >::D32F( pixel.ptr(), value );
						break;

					case PixelComponent::eStencil:
						PixelComponents< PF >::S32F( pixel.ptr(), value );
						break;

					default:
						break;
					}
				}

				static uint8_t getByte( Pixel< PF > const & pixel, PixelComponent component )
				{
					uint8_t result = 0;

					switch ( component )
					{
					case PixelComponent::eDepth:
						result = uint8_t( PixelComponents< PF >::D32( pixel.constPtr() ) >> 24 );
						break;

					case PixelComponent::eStencil:
						result = uint8_t( PixelComponents< PF >::S8( pixel.constPtr() ) );
						break;

					default:
						result = 0;
						break;
					}

					return result;
				}

				static void setByte( Pixel< PF > & pixel, PixelComponent component, uint8_t value )
				{
					switch ( component )
					{
					case PixelComponent::eDepth:
						PixelComponents< PF >::D32( pixel.ptr(), uint32_t( value ) << 24 );
						break;

					case PixelComponent::eStencil:
						PixelComponents< PF >::S8( pixel.ptr(), value );
						break;

					default:
						break;
					}
				}

				static uint16_t getUInt16( Pixel< PF > const & pixel, PixelComponent component )
				{
					uint32_t result = 0;

					switch ( component )
					{
					case PixelComponent::eDepth:
						result = PixelComponents< PF >::D16( pixel.constPtr() );
						break;

					default:
						result = 0;
						break;
					}

					return result;
				}

				static void setUInt16( Pixel< PF > & pixel, PixelComponent component, uint16_t value )
				{
					switch ( component )
					{
					case PixelComponent::eDepth:
						PixelComponents< PF >::D16( pixel.ptr(), value );
						break;

					default:
						break;
					}
				}

				static uint32_t getUInt24( Pixel< PF > const & pixel, PixelComponent component )
				{
					uint32_t result = 0;

					switch ( component )
					{
					case PixelComponent::eDepth:
						result = PixelComponents< PF >::D24( pixel.constPtr() );
						break;

					default:
						result = 0;
						break;
					}

					return result;
				}

				static void setUInt24( Pixel< PF > & pixel, PixelComponent component, uint32_t value )
				{
					switch ( component )
					{
					case PixelComponent::eDepth:
						PixelComponents< PF >::D24( pixel.ptr(), value );
						break;

					default:
						break;
					}
				}

				static uint32_t getUInt32( Pixel< PF > const & pixel, PixelComponent component )
				{
					uint32_t result = 0;

					switch ( component )
					{
					case PixelComponent::eDepth:
						result = PixelComponents< PF >::D32( pixel.constPtr() );
						break;

					default:
						result = 0;
						break;
					}

					return result;
				}

				static void setUInt32( Pixel< PF > & pixel, PixelComponent component, uint32_t value )
				{
					switch ( component )
					{
					case PixelComponent::eDepth:
						PixelComponents< PF >::D32( pixel.ptr(), value );
						break;

					default:
						break;
					}
				}
			};
		}

		template< PixelFormat PF >
		float getFloatComponent( Pixel< PF > const & pixel, PixelComponent component )
		{
			return PixelComponentTraits< PF >::getFloat( pixel, component );
		}

		template< PixelFormat PF >
		void setFloatComponent( Pixel< PF > & pixel, PixelComponent component, float value )
		{
			return PixelComponentTraits< PF >::setFloat( pixel, component, value );
		}

		template< PixelFormat PF >
		uint8_t getByteComponent( Pixel< PF > const & pixel, PixelComponent component )
		{
			return PixelComponentTraits< PF >::getByte( pixel, component );
		}

		template< PixelFormat PF >
		void setByteComponent( Pixel< PF > & pixel, PixelComponent component, uint8_t value )
		{
			return PixelComponentTraits< PF >::setByte( pixel, component, value );
		}

		template< PixelFormat PF >
		uint16_t getUInt16Component( Pixel< PF > const & pixel, PixelComponent component )
		{
			return PixelComponentTraits< PF >::getUInt16( pixel, component );
		}

		template< PixelFormat PF >
		void setUInt16Component( Pixel< PF > & pixel, PixelComponent component, uint16_t value )
		{
			return PixelComponentTraits< PF >::setUInt16( pixel, component, value );
		}

		template< PixelFormat PF >
		uint32_t getUInt24Component( Pixel< PF > const & pixel, PixelComponent component )
		{
			return PixelComponentTraits< PF >::getUInt24( pixel, component );
		}

		template< PixelFormat PF >
		void setUInt24Component( Pixel< PF > & pixel, PixelComponent component, uint32_t value )
		{
			return PixelComponentTraits< PF >::setUInt24( pixel, component, value );
		}

		template< PixelFormat PF >
		uint32_t getUInt32Component( Pixel< PF > const & pixel, PixelComponent component )
		{
			return PixelComponentTraits< PF >::getUInt32( pixel, component );
		}

		template< PixelFormat PF >
		void setUInt32Component( Pixel< PF > & pixel, PixelComponent component, uint32_t value )
		{
			return PixelComponentTraits< PF >::setUInt32( pixel, component, value );
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
		static inline String toString()
		{
			return cuT( "8 bits luminosity" );
		}
		static inline String toStr()
		{
			return cuT( "l8" );
		}
		static inline void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			detail::dynamicColourConversion< PixelFormat::eR8_UNORM >( srcBuffer, dstBuffer, dstFormat );
		}
		static inline void convert( uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			detail::dynamicColourBufferConversion< PixelFormat::eR8_UNORM >( srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF > static inline void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			detail::PixelConverter< PixelFormat::eR8_UNORM, PF >()( srcBuffer, dstBuffer );
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
		static inline String toString()
		{
			return cuT( "Floats luminosity" );
		}
		static inline String toStr()
		{
			return cuT( "l32f" );
		}
		static inline void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			detail::dynamicColourConversion< PixelFormat::eR32_SFLOAT >( srcBuffer, dstBuffer, dstFormat );
		}
		static inline void convert( uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			detail::dynamicColourBufferConversion< PixelFormat::eR32_SFLOAT >( srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF > static inline void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			detail::PixelConverter< PixelFormat::eR32_SFLOAT, PF >()( srcBuffer, dstBuffer );
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
		static inline String toString()
		{
			return cuT( "Half floats luminosity" );
		}
		static inline String toStr()
		{
			return cuT( "l16f" );
		}
		static inline void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
		}
		static inline void convert( uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
		}
		template< PixelFormat PF > static inline void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			detail::PixelConverter< PixelFormat::eR16_SFLOAT, PF >()( srcBuffer, dstBuffer );
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
		static inline String toString()
		{
			return cuT( "16 bits luminosity and alpha" );
		}
		static inline String toStr()
		{
			return cuT( "al16" );
		}
		static inline void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			detail::dynamicColourConversion< PixelFormat::eR8A8_UNORM >( srcBuffer, dstBuffer, dstFormat );
		}
		static inline void convert( uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			detail::dynamicColourBufferConversion< PixelFormat::eR8A8_UNORM >( srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF > static inline void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			detail::PixelConverter< PixelFormat::eR8A8_UNORM, PF >()( srcBuffer, dstBuffer );
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
		static inline String toString()
		{
			return cuT( "Half floats luminosity and alpha" );
		}
		static inline String toStr()
		{
			return cuT( "al16f" );
		}
		static inline void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
		}
		static inline void convert( uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
		}
		template< PixelFormat PF > static inline void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			detail::PixelConverter< PixelFormat::eR16A16_SFLOAT, PF >()( srcBuffer, dstBuffer );
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
		static inline String toString()
		{
			return cuT( "Floats luminosity and alpha" );
		}
		static inline String toStr()
		{
			return cuT( "al32f" );
		}
		static inline void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			detail::dynamicColourConversion< PixelFormat::eR32A32_SFLOAT >( srcBuffer, dstBuffer, dstFormat );
		}
		static inline void convert( uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			detail::dynamicColourBufferConversion< PixelFormat::eR32A32_SFLOAT >( srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF > static inline void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			detail::PixelConverter< PixelFormat::eR32A32_SFLOAT, PF >()( srcBuffer, dstBuffer );
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
		static inline String toString()
		{
			return cuT( "16 bits 5551 ARGB" );
		}
		static inline String toStr()
		{
			return cuT( "argb1555" );
		}
		static inline void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			detail::dynamicColourConversion< PixelFormat::eR5G5B5A1_UNORM >( srcBuffer, dstBuffer, dstFormat );
		}
		static inline void convert( uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			detail::dynamicColourBufferConversion< PixelFormat::eR5G5B5A1_UNORM >( srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF > static inline void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			detail::PixelConverter< PixelFormat::eR5G5B5A1_UNORM, PF >()( srcBuffer, dstBuffer );
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
		static inline String toString()
		{
			return cuT( "16 bits 565 RGB" );
		}
		static inline String toStr()
		{
			return cuT( "rgb565" );
		}
		static inline void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			detail::dynamicColourConversion< PixelFormat::eR5G6B5_UNORM >( srcBuffer, dstBuffer, dstFormat );
		}
		static inline void convert( uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			detail::dynamicColourBufferConversion< PixelFormat::eR5G6B5_UNORM >( srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF > static inline void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			detail::PixelConverter< PixelFormat::eR5G6B5_UNORM, PF >()( srcBuffer, dstBuffer );
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
		static inline String toString()
		{
			return cuT( "24 bits 888 RGB" );
		}
		static inline String toStr()
		{
			return cuT( "rgb24" );
		}
		static inline void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			detail::dynamicColourConversion< PixelFormat::eR8G8B8_UNORM >( srcBuffer, dstBuffer, dstFormat );
		}
		static inline void convert( uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			detail::dynamicColourBufferConversion< PixelFormat::eR8G8B8_UNORM >( srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF > static inline void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			detail::PixelConverter< PixelFormat::eR8G8B8_UNORM, PF >()( srcBuffer, dstBuffer );
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
		static inline String toString()
		{
			return cuT( "24 bits 888 BGR" );
		}
		static inline String toStr()
		{
			return cuT( "bgr24" );
		}
		static inline void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			detail::dynamicColourConversion< PixelFormat::eB8G8R8_UNORM >( srcBuffer, dstBuffer, dstFormat );
		}
		static inline void convert( uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			detail::dynamicColourBufferConversion< PixelFormat::eB8G8R8_UNORM >( srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF > static inline void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			detail::PixelConverter< PixelFormat::eB8G8R8_UNORM, PF >()( srcBuffer, dstBuffer );
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
		static inline String toString()
		{
			return cuT( "24 bits 888 RGB sRGB" );
		}
		static inline String toStr()
		{
			return cuT( "rgb24_srgb" );
		}
		static inline void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			detail::dynamicColourConversion< PixelFormat::eR8G8B8_SRGB >( srcBuffer, dstBuffer, dstFormat );
		}
		static inline void convert( uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			detail::dynamicColourBufferConversion< PixelFormat::eR8G8B8_SRGB >( srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF > static inline void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			detail::PixelConverter< PixelFormat::eR8G8B8_SRGB, PF >()( srcBuffer, dstBuffer );
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
		static inline String toString()
		{
			return cuT( "24 bits 888 BGR sRGB" );
		}
		static inline String toStr()
		{
			return cuT( "bgr24_srgb" );
		}
		static inline void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			detail::dynamicColourConversion< PixelFormat::eB8G8R8_SRGB >( srcBuffer, dstBuffer, dstFormat );
		}
		static inline void convert( uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			detail::dynamicColourBufferConversion< PixelFormat::eB8G8R8_SRGB >( srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF > static inline void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			detail::PixelConverter< PixelFormat::eB8G8R8_SRGB, PF >()( srcBuffer, dstBuffer );
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
		static inline String toString()
		{
			return cuT( "32 bits 8888 ARGB" );
		}
		static inline String toStr()
		{
			return cuT( "argb32" );
		}
		static inline void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			detail::dynamicColourConversion< PixelFormat::eR8G8B8A8_UNORM >( srcBuffer, dstBuffer, dstFormat );
		}
		static inline void convert( uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			detail::dynamicColourBufferConversion< PixelFormat::eR8G8B8A8_UNORM >( srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF > static inline void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			detail::PixelConverter< PixelFormat::eR8G8B8A8_UNORM, PF >()( srcBuffer, dstBuffer );
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
		static inline String toString()
		{
			return cuT( "32 bits 8888 ABGR" );
		}
		static inline String toStr()
		{
			return cuT( "abgr32" );
		}
		static inline void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			detail::dynamicColourConversion< PixelFormat::eA8B8G8R8_UNORM >( srcBuffer, dstBuffer, dstFormat );
		}
		static inline void convert( uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			detail::dynamicColourBufferConversion< PixelFormat::eA8B8G8R8_UNORM >( srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF > static inline void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			detail::PixelConverter< PixelFormat::eA8B8G8R8_UNORM, PF >()( srcBuffer, dstBuffer );
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
		static inline String toString()
		{
			return cuT( "32 bits 8888 ARGB sRGB" );
		}
		static inline String toStr()
		{
			return cuT( "argb32_srgb" );
		}
		static inline void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			detail::dynamicColourConversion< PixelFormat::eR8G8B8A8_SRGB >( srcBuffer, dstBuffer, dstFormat );
		}
		static inline void convert( uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			detail::dynamicColourBufferConversion< PixelFormat::eR8G8B8A8_SRGB >( srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF > static inline void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			detail::PixelConverter< PixelFormat::eR8G8B8A8_SRGB, PF >()( srcBuffer, dstBuffer );
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
		static inline String toString()
		{
			return cuT( "32 bits 8888 ABGR sRGB" );
		}
		static inline String toStr()
		{
			return cuT( "abgr32_srgb" );
		}
		static inline void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			detail::dynamicColourConversion< PixelFormat::eA8B8G8R8_SRGB >( srcBuffer, dstBuffer, dstFormat );
		}
		static inline void convert( uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			detail::dynamicColourBufferConversion< PixelFormat::eA8B8G8R8_SRGB >( srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF > static inline void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			detail::PixelConverter< PixelFormat::eA8B8G8R8_SRGB, PF >()( srcBuffer, dstBuffer );
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
		static inline String toString()
		{
			return cuT( "Half floating point RGB" );
		}
		static inline String toStr()
		{
			return cuT( "rgb16f" );
		}
		static inline void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			detail::dynamicColourConversion< PixelFormat::eR16G16B16_SFLOAT >( srcBuffer, dstBuffer, dstFormat );
		}
		static inline void convert( uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			detail::dynamicColourBufferConversion< PixelFormat::eR16G16B16_SFLOAT >( srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF > static inline void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			detail::PixelConverter< PixelFormat::eR16G16B16_SFLOAT, PF >()( srcBuffer, dstBuffer );
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
		static inline String toString()
		{
			return cuT( "Half floating point ARGB" );
		}
		static inline String toStr()
		{
			return cuT( "argb16f" );
		}
		static inline void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			detail::dynamicColourConversion< PixelFormat::eR16G16B16A16_SFLOAT >( srcBuffer, dstBuffer, dstFormat );
		}
		static inline void convert( uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			detail::dynamicColourBufferConversion< PixelFormat::eR16G16B16A16_SFLOAT >( srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF > static inline void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			detail::PixelConverter< PixelFormat::eR16G16B16A16_SFLOAT, PF >()( srcBuffer, dstBuffer );
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
		static inline String toString()
		{
			return cuT( "Floating point RGB" );
		}
		static inline String toStr()
		{
			return cuT( "rgb32f" );
		}
		static inline void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			detail::dynamicColourConversion< PixelFormat::eR32G32B32_SFLOAT >( srcBuffer, dstBuffer, dstFormat );
		}
		static inline void convert( uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			detail::dynamicColourBufferConversion< PixelFormat::eR32G32B32_SFLOAT >( srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF > static inline void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			detail::PixelConverter< PixelFormat::eR32G32B32_SFLOAT, PF >()( srcBuffer, dstBuffer );
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
		static inline String toString()
		{
			return cuT( "Floating point ARGB" );
		}
		static inline String toStr()
		{
			return cuT( "argb32f" );
		}
		static inline void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			detail::dynamicColourConversion< PixelFormat::eR32G32B32A32_SFLOAT >( srcBuffer, dstBuffer, dstFormat );
		}
		static inline void convert( uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			detail::dynamicColourBufferConversion< PixelFormat::eR32G32B32A32_SFLOAT >( srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF > static inline void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			detail::PixelConverter< PixelFormat::eR32G32B32A32_SFLOAT, PF >()( srcBuffer, dstBuffer );
		}
	};

	//*************************************************************************************************

	//!\~english	Specialisation for PixelFormat::eBC1_RGB_UNORM_BLOCK
	//!\~french		Spécialisation pour PixelFormat::eBC1_RGB_UNORM_BLOCK
	template<>
	struct PixelDefinitions< PixelFormat::eBC1_RGB_UNORM_BLOCK >
	{
		static const uint8_t Size = 1;
		static const uint8_t Count = 3;
		static const bool Alpha = false;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = true;
		static inline String toString()
		{
			return cuT( "DXT1 8 bits compressed format" );
		}
		static inline String toStr()
		{
			return cuT( "dxtc1" );
		}
		static inline void convert( uint8_t const *, uint8_t *, PixelFormat )
		{
			CU_UnsupportedError( "No conversion from " + string::stringCast< char >( toString() ) + " defined" );
		}
		static inline void convert( uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			CU_UnsupportedError( "No conversion from " + string::stringCast< char >( toString() ) + " defined" );
		}
		template< PixelFormat PF > static inline void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			detail::PixelConverter< PixelFormat::eBC1_RGB_UNORM_BLOCK, PF >()( srcBuffer, dstBuffer );
		}
	};

	//*************************************************************************************************

	//!\~english	Specialisation for PixelFormat::eBC3_UNORM_BLOCK
	//!\~french		Spécialisation pour PixelFormat::eBC3_UNORM_BLOCK
	template<>
	struct PixelDefinitions< PixelFormat::eBC3_UNORM_BLOCK >
	{
		static const uint8_t Size = 2;
		static const uint8_t Count = 4;
		static const bool Alpha = true;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = true;
		static const PixelFormat NoAlphaPF = PixelFormat::eBC3_UNORM_BLOCK;
		static inline String toString()
		{
			return cuT( "DXT3 16 bits compressed format" );
		}
		static inline String toStr()
		{
			return cuT( "dxtc3" );
		}
		static inline void convert( uint8_t const *, uint8_t *, PixelFormat )
		{
			CU_UnsupportedError( "No conversion from " + string::stringCast< char >( toString() ) + " defined" );
		}
		static inline void convert( uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			CU_UnsupportedError( "No conversion from " + string::stringCast< char >( toString() ) + " defined" );
		}
		template< PixelFormat PF > static inline void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			detail::PixelConverter< PixelFormat::eBC3_UNORM_BLOCK, PF >()( srcBuffer, dstBuffer );
		}
	};

	//*************************************************************************************************

	//!\~english	Specialisation for PixelFormat::eBC5_UNORM_BLOCK
	//!\~french		Spécialisation pour PixelFormat::eBC5_UNORM_BLOCK
	template<>
	struct PixelDefinitions< PixelFormat::eBC5_UNORM_BLOCK >
	{
		static const uint8_t Size = 2;
		static const uint8_t Count = 4;
		static const bool Alpha = true;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = true;
		static const PixelFormat NoAlphaPF = PixelFormat::eBC5_UNORM_BLOCK;
		static inline String toString()
		{
			return cuT( "DXT5 16 bits compressed format" );
		}
		static inline String toStr()
		{
			return cuT( "dxtc5" );
		}
		static inline void convert( uint8_t const *, uint8_t *, PixelFormat )
		{
			CU_UnsupportedError( "No conversion from " + string::stringCast< char >( toString() ) + " defined" );
		}
		static inline void convert( uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			CU_UnsupportedError( "No conversion from " + string::stringCast< char >( toString() ) + " defined" );
		}
		template< PixelFormat PF > static inline void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			detail::PixelConverter< PixelFormat::eBC5_UNORM_BLOCK, PF >()( srcBuffer, dstBuffer );
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
		static inline String toString()
		{
			return cuT( "16 bits depth" );
		}
		static inline String toStr()
		{
			return cuT( "depth16" );
		}
		static inline void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			detail::dynamicDepthConversion< PixelFormat::eD16_UNORM >( srcBuffer, dstBuffer, dstFormat );
		}
		static inline void convert( uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			detail::dynamicDepthBufferConversion< PixelFormat::eD16_UNORM >( srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF > static inline void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			detail::PixelConverter< PixelFormat::eD16_UNORM, PF >()( srcBuffer, dstBuffer );
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
		static inline String toString()
		{
			return cuT( "24 bits depth, 8 bits stencil" );
		}
		static inline String toStr()
		{
			return cuT( "depth24s8" );
		}
		static inline void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			detail::dynamicDepthConversion< PixelFormat::eD24_UNORM_S8_UINT >( srcBuffer, dstBuffer, dstFormat );
		}
		static inline void convert( uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			detail::dynamicDepthBufferConversion< PixelFormat::eD24_UNORM_S8_UINT >( srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF > static inline void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			detail::PixelConverter<PixelFormat::eD24_UNORM_S8_UINT, PF>()( srcBuffer, dstBuffer );
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
		static inline String toString()
		{
			return cuT( "32 bits depth" );
		}
		static inline String toStr()
		{
			return cuT( "depth32" );
		}
		static inline void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			detail::dynamicDepthConversion< PixelFormat::eD32_UNORM >( srcBuffer, dstBuffer, dstFormat );
		}
		static inline void convert( uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			detail::dynamicDepthBufferConversion< PixelFormat::eD32_UNORM >( srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF > static inline void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			detail::PixelConverter< PixelFormat::eD32_UNORM, PF >()( srcBuffer, dstBuffer );
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
		static inline String toString()
		{
			return cuT( "32 bits floating point depth" );
		}
		static inline String toStr()
		{
			return cuT( "depth32f" );
		}
		static inline void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			detail::dynamicDepthConversion< PixelFormat::eD32_SFLOAT >( srcBuffer, dstBuffer, dstFormat );
		}
		static inline void convert( uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			detail::dynamicDepthBufferConversion< PixelFormat::eD32_SFLOAT >( srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF > static inline void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			detail::PixelConverter< PixelFormat::eD32_SFLOAT, PF >()( srcBuffer, dstBuffer );
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
		static inline String toString()
		{
			return cuT( "32 bits floating point depth, 8 bits stencil" );
		}
		static inline String toStr()
		{
			return cuT( "depth32fs8" );
		}
		static inline void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			detail::dynamicDepthConversion< PixelFormat::eD32_SFLOAT_S8_UINT >( srcBuffer, dstBuffer, dstFormat );
		}
		static inline void convert( uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			detail::dynamicDepthBufferConversion< PixelFormat::eD32_SFLOAT_S8_UINT >( srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF > static inline void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			detail::PixelConverter< PixelFormat::eD32_SFLOAT_S8_UINT, PF >()( srcBuffer, dstBuffer );
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
		static inline String toString()
		{
			return cuT( "8 bits stencil" );
		}
		static inline String toStr()
		{
			return cuT( "stencil8" );
		}
		static inline void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			detail::dynamicStencilConversion< PixelFormat::eS8_UINT >( srcBuffer, dstBuffer, dstFormat );
		}
		static inline void convert( uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			detail::dynamicStencilBufferConversion< PixelFormat::eS8_UINT >( srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF > static inline void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			detail::PixelConverter< PixelFormat::eS8_UINT, PF >()( srcBuffer, dstBuffer );
		}
	};

	//*************************************************************************************************
}
