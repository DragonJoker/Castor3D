#include <emmintrin.h>
#include <cstring>
#include "UnsupportedFormatException.hpp"
#include "StringUtils.hpp"

namespace Castor
{
	//!\~english Specialisation for ePIXEL_FORMAT_L8	\~french Spécialisation pour ePIXEL_FORMAT_L8
	template<> struct component< ePIXEL_FORMAT_L8 >
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
	//!\~english Specialisation for ePIXEL_FORMAT_L16F32F	\~french Spécialisation pour ePIXEL_FORMAT_L16F32F
	template<> struct component< ePIXEL_FORMAT_L16F32F >
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
	//!\~english Specialisation for ePIXEL_FORMAT_L32F	\~french Spécialisation pour ePIXEL_FORMAT_L32F
	template<> struct component< ePIXEL_FORMAT_L32F >
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
	//!\~english Specialisation for ePIXEL_FORMAT_A8L8	\~french Spécialisation pour ePIXEL_FORMAT_A8L8
	template<> struct component< ePIXEL_FORMAT_A8L8 >
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
	//!\~english Specialisation for ePIXEL_FORMAT_AL16F32F	\~french Spécialisation pour ePIXEL_FORMAT_AL16F32F
	template<> struct component< ePIXEL_FORMAT_AL16F32F >
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
	//!\~english Specialisation for ePIXEL_FORMAT_AL32F	\~french Spécialisation pour ePIXEL_FORMAT_AL32F
	template<> struct component< ePIXEL_FORMAT_AL32F >
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
	//!\~english Specialisation for ePIXEL_FORMAT_A1R5G5B5	\~french Spécialisation pour ePIXEL_FORMAT_A1R5G5B5
	template<> struct component< ePIXEL_FORMAT_A1R5G5B5 >
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
	//!\~english Specialisation for ePIXEL_FORMAT_A4R4G4B4	\~french Spécialisation pour ePIXEL_FORMAT_A4R4G4B4
	template<> struct component< ePIXEL_FORMAT_A4R4G4B4 >
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
	//!\~english Specialisation for ePIXEL_FORMAT_R5G6B5	\~french Spécialisation pour ePIXEL_FORMAT_R5G6B5
	template<> struct component< ePIXEL_FORMAT_R5G6B5 >
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
	//!\~english Specialisation for ePIXEL_FORMAT_R8G8B8	\~french Spécialisation pour ePIXEL_FORMAT_R8G8B8
	template<> struct component< ePIXEL_FORMAT_R8G8B8 >
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
	//!\~english Specialisation for ePIXEL_FORMAT_B8G8R8	\~french Spécialisation pour ePIXEL_FORMAT_B8G8R8
	template<> struct component< ePIXEL_FORMAT_B8G8R8 >
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
	//!\~english Specialisation for ePIXEL_FORMAT_A8R8G8B8	\~french Spécialisation pour ePIXEL_FORMAT_A8R8G8B8
	template<> struct component< ePIXEL_FORMAT_A8R8G8B8 >
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
	//!\~english Specialisation for ePIXEL_FORMAT_A8B8G8R8	\~french Spécialisation pour ePIXEL_FORMAT_A8B8G8R8
	template<> struct component< ePIXEL_FORMAT_A8B8G8R8 >
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
	//!\~english Specialisation for ePIXEL_FORMAT_RGB16F32F	\~french Spécialisation pour ePIXEL_FORMAT_RGB16F32F
	template<> struct component< ePIXEL_FORMAT_RGB16F32F >
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
	//!\~english Specialisation for ePIXEL_FORMAT_RGB32F	\~french Spécialisation pour ePIXEL_FORMAT_RGB32F
	template<> struct component< ePIXEL_FORMAT_RGB32F >
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
	//!\~english Specialisation for ePIXEL_FORMAT_ARGB16F32F	\~french Spécialisation pour ePIXEL_FORMAT_ARGB16F32F
	template<> struct component< ePIXEL_FORMAT_ARGB16F32F >
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
		static inline float A32F( uint8_t const * p_pSrc )
		{
			return reinterpret_cast< float const * >( p_pSrc )[0];
		}
		static inline float R32F( uint8_t const * p_pSrc )
		{
			return reinterpret_cast< float const * >( p_pSrc )[1];
		}
		static inline float G32F( uint8_t const * p_pSrc )
		{
			return reinterpret_cast< float const * >( p_pSrc )[2];
		}
		static inline float B32F( uint8_t const * p_pSrc )
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
		static inline void A32F( uint8_t * p_pSrc, float p_val )
		{
			reinterpret_cast< float * >( p_pSrc )[0] = p_val;
		}
		static inline void R32F( uint8_t * p_pSrc, float p_val )
		{
			reinterpret_cast< float * >( p_pSrc )[1] = p_val;
		}
		static inline void G32F( uint8_t * p_pSrc, float p_val )
		{
			reinterpret_cast< float * >( p_pSrc )[2] = p_val;
		}
		static inline void B32F( uint8_t * p_pSrc, float p_val )
		{
			reinterpret_cast< float * >( p_pSrc )[3] = p_val;
		}
	};
	//!\~english Specialisation for ePIXEL_FORMAT_ARGB32F	\~french Spécialisation pour ePIXEL_FORMAT_ARGB32F
	template<> struct component< ePIXEL_FORMAT_ARGB32F >
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
		static inline float A32F( uint8_t const * p_pSrc )
		{
			return reinterpret_cast< float const * >( p_pSrc )[0];
		}
		static inline float R32F( uint8_t const * p_pSrc )
		{
			return reinterpret_cast< float const * >( p_pSrc )[1];
		}
		static inline float G32F( uint8_t const * p_pSrc )
		{
			return reinterpret_cast< float const * >( p_pSrc )[2];
		}
		static inline float B32F( uint8_t const * p_pSrc )
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
		static inline void A32F( uint8_t * p_pSrc, float p_val )
		{
			reinterpret_cast< float * >( p_pSrc )[0] = p_val;
		}
		static inline void R32F( uint8_t * p_pSrc, float p_val )
		{
			reinterpret_cast< float * >( p_pSrc )[1] = p_val;
		}
		static inline void G32F( uint8_t * p_pSrc, float p_val )
		{
			reinterpret_cast< float * >( p_pSrc )[2] = p_val;
		}
		static inline void B32F( uint8_t * p_pSrc, float p_val )
		{
			reinterpret_cast< float * >( p_pSrc )[3] = p_val;
		}
	};
	//!\~english Specialisation for ePIXEL_FORMAT_YUY2	\~french Spécialisation pour ePIXEL_FORMAT_YUY2
	template<> struct component< ePIXEL_FORMAT_YUY2 >
	{
#	define YUV_Y(x)		(((x)[0] & 0xF0) & (((x)[1] & 0xF0) >> 2))
#	define YUV_U(x)		((x)[0] & 0x0F)
#	define YUV_V(x)		((x)[1] & 0x0F)
#	define YUV_C(x)		(YUV_Y(x) -  16)
#	define YUV_D(x)		(YUV_U(x) - 128)
#	define YUV_E(x)		(YUV_V(x) - 128)
#	define YUV_clip(x)	static_cast<uint8_t>( (x) > 255 ? 255 : (x) < 0 ? 0 : (x))
		static inline uint8_t L8( uint8_t const * p_pSrc	)
		{
			return uint8_t( R8( p_pSrc ) * 0.30 + G8( p_pSrc ) * 0.59 + B8( p_pSrc ) * 0.11 );
		}
		static inline uint8_t R8( uint8_t const * p_pSrc	)
		{
			return uint8_t( YUV_clip( ( 298 * YUV_C( p_pSrc ) + 409 * YUV_E( p_pSrc ) + 128 ) >> 8 ) );
		}
		static inline uint8_t G8( uint8_t const * p_pSrc	)
		{
			return uint8_t( YUV_clip( ( 298 * YUV_C( p_pSrc ) - 100 * YUV_D( p_pSrc ) - 208 * YUV_E( p_pSrc ) + 128 ) >> 8 ) );
		}
		static inline uint8_t B8( uint8_t const * p_pSrc	)
		{
			return uint8_t( YUV_clip( ( 298 * YUV_C( p_pSrc ) + 516 * YUV_D( p_pSrc ) + 128 ) >> 8 ) );
		}
		static inline uint8_t A8( uint8_t const * )
		{
			return 0xFF;
		}
		static inline float L32F( uint8_t const * p_pSrc	)
		{
			return L8( p_pSrc ) / 255.0f;
		}
		static inline float R32F( uint8_t const * p_pSrc	)
		{
			return R8( p_pSrc ) / 255.0f;
		}
		static inline float G32F( uint8_t const * p_pSrc	)
		{
			return G8( p_pSrc ) / 255.0f;
		}
		static inline float B32F( uint8_t const * p_pSrc	)
		{
			return B8( p_pSrc ) / 255.0f;
		}
		static inline float A32F( uint8_t const * p_pSrc	)
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
	//!\~english Specialisation for ePIXEL_FORMAT_DEPTH16	\~french Spécialisation pour ePIXEL_FORMAT_DEPTH16
	template<> struct component< ePIXEL_FORMAT_DEPTH16 >
	{
#define src	( *reinterpret_cast< uint16_t const * >( p_pSrc ) )
#define dst	( *reinterpret_cast< uint16_t * >( p_pSrc ) )

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

#undef src
#undef dst
	};
	//!\~english Specialisation for ePIXEL_FORMAT_DEPTH24	\~french Spécialisation pour ePIXEL_FORMAT_DEPTH24
	template<> struct component< ePIXEL_FORMAT_DEPTH24 >
	{
#define src	( *reinterpret_cast< uint32_t const * >( p_pSrc ) )
#define dst	( *reinterpret_cast< uint32_t * >( p_pSrc ) )

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

#undef src
#undef dst
	};
	//!\~english Specialisation for ePIXEL_FORMAT_DEPTH24S8	\~french Spécialisation pour ePIXEL_FORMAT_DEPTH24S8
	template<> struct component< ePIXEL_FORMAT_DEPTH24S8 >
	{
#define src	( *reinterpret_cast< uint32_t const * >( p_pSrc ) )
#define dst	( *reinterpret_cast< uint32_t * >( p_pSrc ) )

		static inline uint8_t	BIT( uint8_t const  p_bySrc )
		{
			return ( ( p_bySrc >> 0 ) & 0x01 ) | ( ( p_bySrc >> 1 ) & 0x01 ) | ( ( p_bySrc >> 2 ) & 0x01 ) | ( ( p_bySrc >> 3 ) & 0x01 ) | ( ( p_bySrc >> 4 ) & 0x01 ) | ( ( p_bySrc >> 5 ) & 0x01 ) | ( ( p_bySrc >> 6 ) & 0x01 ) | ( ( p_bySrc >> 7 ) & 0x01 );
		}
		static inline uint16_t D16( uint8_t const * p_pSrc )
		{
			return uint16_t( ( src & 0xFFFFFF00 ) >> 16 );
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
		static inline void D16( uint8_t * p_pSrc, uint16_t p_val )
		{
			dst = ( src & 0x000000FF ) | ( ( uint32_t( p_val ) << 16 ) & 0xFFFFFF00 );
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

#undef src
#undef dst
	};
	//!\~english Specialisation for ePIXEL_FORMAT_DEPTH32	\~french Spécialisation pour ePIXEL_FORMAT_DEPTH32
	template<> struct component< ePIXEL_FORMAT_DEPTH32 >
	{
#define src	( *reinterpret_cast< uint32_t const * >( p_pSrc ) )
#define dst	( *reinterpret_cast< uint32_t * >( p_pSrc ) )

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

#undef src
#undef dst
	};
	//!\~english Specialisation for ePIXEL_FORMAT_STENCIL8	\~french Spécialisation pour ePIXEL_FORMAT_STENCIL8
	template<> struct component< ePIXEL_FORMAT_STENCIL8 >
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
		//!\~english Specialisation for converting to ePIXEL_FORMAT_L8	\~french Spécialisation pour convertir vers ePIXEL_FORMAT_L8
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == ePIXEL_FORMAT_L8 >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				component< PFDst >::L8( p_pDst, component< PFSrc >::L8( p_pSrc ) );
				p_pSrc += pixel_definitions< PFSrc >::Size;
				p_pDst += pixel_definitions< PFDst >::Size;
			}
		};
		//!\~english Specialisation for converting to ePIXEL_FORMAT_L16F32F	\~french Spécialisation pour convertir vers ePIXEL_FORMAT_L16F32F
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == ePIXEL_FORMAT_L16F32F >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				component< PFDst >::L32F( p_pDst, component< PFSrc >::L32F( p_pSrc ) );
				p_pSrc += pixel_definitions< PFSrc >::Size;
				p_pDst += pixel_definitions< PFDst >::Size;
			}
		};
		//!\~english Specialisation for converting to ePIXEL_FORMAT_L32F	\~french Spécialisation pour convertir vers ePIXEL_FORMAT_L32F
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == ePIXEL_FORMAT_L32F >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				component< PFDst >::L32F( p_pDst, component< PFSrc >::L32F( p_pSrc ) );
				p_pSrc += pixel_definitions< PFSrc >::Size;
				p_pDst += pixel_definitions< PFDst >::Size;
			}
		};
		//!\~english Specialisation for converting to ePIXEL_FORMAT_A8L8	\~french Spécialisation pour convertir vers ePIXEL_FORMAT_A8L8
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == ePIXEL_FORMAT_A8L8 >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				component< PFDst >::L8( p_pDst, component< PFSrc >::L8( p_pSrc ) );
				component< PFDst >::A8( p_pDst, component< PFSrc >::A8( p_pSrc ) );
				p_pSrc += pixel_definitions< PFSrc >::Size;
				p_pDst += pixel_definitions< PFDst >::Size;
			}
		};
		//!\~english Specialisation for converting to ePIXEL_FORMAT_AL16F32F	\~french Spécialisation pour convertir vers ePIXEL_FORMAT_AL16F32F
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == ePIXEL_FORMAT_AL16F32F >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				component< PFDst >::L32F( p_pDst, component< PFSrc >::L32F( p_pSrc ) );
				component< PFDst >::A32F( p_pDst, component< PFSrc >::A32F( p_pSrc ) );
				p_pSrc += pixel_definitions< PFSrc >::Size;
				p_pDst += pixel_definitions< PFDst >::Size;
			}
		};
		//!\~english Specialisation for converting to ePIXEL_FORMAT_AL32F	\~french Spécialisation pour convertir vers ePIXEL_FORMAT_AL32F
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == ePIXEL_FORMAT_AL32F >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				component< PFDst >::L32F( p_pDst, component< PFSrc >::L32F( p_pSrc ) );
				component< PFDst >::A32F( p_pDst, component< PFSrc >::A32F( p_pSrc ) );
				p_pSrc += pixel_definitions< PFSrc >::Size;
				p_pDst += pixel_definitions< PFDst >::Size;
			}
		};
		//!\~english Specialisation for converting to ePIXEL_FORMAT_A1R5G5B5	\~french Spécialisation pour convertir vers ePIXEL_FORMAT_A1R5G5B5
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == ePIXEL_FORMAT_A1R5G5B5 >::type >
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
		//!\~english Specialisation for converting to ePIXEL_FORMAT_A4R4G4B4	\~french Spécialisation pour convertir vers ePIXEL_FORMAT_A4R4G4B4
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == ePIXEL_FORMAT_A4R4G4B4 >::type >
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
		//!\~english Specialisation for converting to ePIXEL_FORMAT_R5G6B5	\~french Spécialisation pour convertir vers ePIXEL_FORMAT_R5G6B5
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == ePIXEL_FORMAT_R5G6B5 >::type >
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
		//!\~english Specialisation for converting to ePIXEL_FORMAT_R8G8B8	\~french Spécialisation pour convertir vers ePIXEL_FORMAT_R8G8B8
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == ePIXEL_FORMAT_R8G8B8 >::type >
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
		//!\~english Specialisation for converting to ePIXEL_FORMAT_B8G8R8	\~french Spécialisation pour convertir vers ePIXEL_FORMAT_B8G8R8
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == ePIXEL_FORMAT_B8G8R8 >::type >
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
		//!\~english Specialisation for converting to ePIXEL_FORMAT_A8R8G8B8	\~french Spécialisation pour convertir vers ePIXEL_FORMAT_A8R8G8B8
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == ePIXEL_FORMAT_A8R8G8B8 >::type >
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
		//!\~english Specialisation for converting to ePIXEL_FORMAT_A8R8G8B8	\~french Spécialisation pour convertir vers ePIXEL_FORMAT_A8R8G8B8
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == ePIXEL_FORMAT_A8B8G8R8 >::type >
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
		//!\~english Specialisation for converting to ePIXEL_FORMAT_RGB16F32F	\~french Spécialisation pour convertir vers ePIXEL_FORMAT_RGB16F32F
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == ePIXEL_FORMAT_RGB16F32F >::type >
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
		//!\~english Specialisation for converting to ePIXEL_FORMAT_RGB32F	\~french Spécialisation pour convertir vers ePIXEL_FORMAT_RGB32F
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == ePIXEL_FORMAT_RGB32F >::type >
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
		//!\~english Specialisation for converting to ePIXEL_FORMAT_ARGB16F32F	\~french Spécialisation pour convertir vers ePIXEL_FORMAT_ARGB16F32F
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == ePIXEL_FORMAT_ARGB16F32F >::type >
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
		//!\~english Specialisation for converting to ePIXEL_FORMAT_ARGB32F	\~french Spécialisation pour convertir vers ePIXEL_FORMAT_ARGB32F
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == ePIXEL_FORMAT_ARGB32F >::type >
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
		//!\~english Specialisation for converting to ePIXEL_FORMAT_YUY2	\~french Spécialisation pour convertir vers ePIXEL_FORMAT_YUY2
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == ePIXEL_FORMAT_YUY2 >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				p_pDst[0] = ( ( ( ( ( 66 * component< PFSrc >::R8( p_pSrc ) + 129 * component< PFSrc >::G8( p_pSrc ) +  25 * component< PFSrc >::B8( p_pSrc ) + 128 ) >> 8 ) +  16 ) & 0x0F ) << 4 ) | ( ( ( ( -38 * component< PFSrc >::R8( p_pSrc ) - 74 * component< PFSrc >::G8( p_pSrc ) + 112 * component< PFSrc >::B8( p_pSrc ) + 128 ) >> 8 ) + 128 ) & 0x0F );
				p_pDst[1] = ( ( ( ( ( 66 * component< PFSrc >::R8( p_pSrc ) + 129 * component< PFSrc >::G8( p_pSrc ) +  25 * component< PFSrc >::B8( p_pSrc ) + 128 ) >> 8 ) +  16 ) & 0x0F ) << 4 ) | ( ( ( ( 112 * component< PFSrc >::R8( p_pSrc ) - 94 * component< PFSrc >::G8( p_pSrc ) -  18 * component< PFSrc >::B8( p_pSrc ) + 128 ) >> 8 ) + 128 ) & 0x0F );
				p_pSrc += pixel_definitions< PFSrc >::Size;
				p_pDst += pixel_definitions< PFDst >::Size;
			}
		};
		//!\~english Specialisation for converting to ePIXEL_FORMAT_DEPTH16	\~french Spécialisation pour convertir vers ePIXEL_FORMAT_DEPTH16
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == ePIXEL_FORMAT_DEPTH16 >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				component< PFDst >::D16( p_pDst, component< PFSrc >::D16( p_pSrc ) );
				p_pSrc += pixel_definitions< PFSrc >::Size;
				p_pDst += pixel_definitions< PFDst >::Size;
			}
		};
		//!\~english Specialisation for converting to ePIXEL_FORMAT_DEPTH24	\~french Spécialisation pour convertir vers ePIXEL_FORMAT_DEPTH24
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == ePIXEL_FORMAT_DEPTH24 >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				component< PFDst >::D24( p_pDst, component< PFSrc >::D24( p_pSrc ) );
				p_pSrc += pixel_definitions< PFSrc >::Size;
				p_pDst += pixel_definitions< PFDst >::Size;
			}
		};
		//!\~english Specialisation for converting to ePIXEL_FORMAT_DEPTH24S8	\~french Spécialisation pour convertir vers ePIXEL_FORMAT_DEPTH24S8
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFSrc != ePIXEL_FORMAT_STENCIL1 && PFDst == ePIXEL_FORMAT_DEPTH24S8 >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				component< PFDst >::D24( p_pDst, component< PFSrc >::D24( p_pSrc ) );
				component< PFDst >::S8( p_pDst, component< PFSrc >::S8( p_pSrc ) );
				p_pSrc += pixel_definitions< PFSrc >::Size;
				p_pDst += pixel_definitions< PFDst >::Size;
			}
		};
		//!\~english Specialisation for converting to ePIXEL_FORMAT_DEPTH32	\~french Spécialisation pour convertir vers ePIXEL_FORMAT_DEPTH32
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == ePIXEL_FORMAT_DEPTH32 >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				component< PFDst >::D32( p_pDst, component< PFSrc >::D32( p_pSrc ) );
				p_pSrc += pixel_definitions< PFSrc >::Size;
				p_pDst += pixel_definitions< PFDst >::Size;
			}
		};
		//!\~english Specialisation for converting to ePIXEL_FORMAT_STENCIL1	\~french Spécialisation pour convertir vers ePIXEL_FORMAT_STENCIL1
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == ePIXEL_FORMAT_STENCIL1 >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				*p_pDst = component< PFSrc >::S1( p_pSrc );
				p_pSrc += pixel_definitions< PFSrc >::Size;
				p_pDst += pixel_definitions< PFDst >::Size;
			}
		};
		//!\~english Specialisation for converting to ePIXEL_FORMAT_STENCIL8	\~french Spécialisation pour convertir vers ePIXEL_FORMAT_STENCIL8
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFSrc != ePIXEL_FORMAT_STENCIL1 && PFDst == ePIXEL_FORMAT_STENCIL8 >::type >
		{
			inline void operator()( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
			{
				component< PFDst >::S8( p_pDst, component< PFSrc >::S8( p_pSrc ) );
				p_pSrc += pixel_definitions< PFSrc >::Size;
				p_pDst += pixel_definitions< PFDst >::Size;
			}
		};
		//!\~english Specialisation for convertions from ePIXEL_FORMAT_STENCIL1 to ePIXEL_FORMAT_STENCIL8	\~french Spécialisation pour convertir de ePIXEL_FORMAT_STENCIL1 vers ePIXEL_FORMAT_STENCIL8
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFSrc == ePIXEL_FORMAT_STENCIL1 && PFDst == ePIXEL_FORMAT_STENCIL8 >::type >
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
		//!\~english Specialisation for convertions from ePIXEL_FORMAT_STENCIL1 to ePIXEL_FORMAT_DEPTH24S8	\~french Spécialisation pour convertir de ePIXEL_FORMAT_STENCIL1 vers ePIXEL_FORMAT_DEPTH24S8
		template< TPL_PIXEL_FORMAT PFSrc, TPL_PIXEL_FORMAT PFDst > struct pixel_converter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFSrc == ePIXEL_FORMAT_STENCIL1 && PFDst == ePIXEL_FORMAT_DEPTH24S8 >::type >
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
				uint32_t l_uiCount = p_uiSrcSize / pixel_definitions< PFSrc >::Size;

				for ( uint32_t i = 0; i < l_uiCount; i++ )
				{
					pixel_converter< PFSrc, PFDst >()( l_pSrc, l_pDst );
				}
			}
		};
		/*!
		\author 	Sylvain DOREMUS
		\date 		08/09/2013
		\~english
		\brief		Specialisation for YUY2 to A8R8G8B8 conversion
		\~french
		\brief		Spécialisation pour la conversion YUY2 vers A8R8G8B8
		*/
		template<> struct buffer_converter< ePIXEL_FORMAT_YUY2, ePIXEL_FORMAT_A8R8G8B8 >
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
		template<> struct buffer_converter< ePIXEL_FORMAT_YUY2, ePIXEL_FORMAT_R8G8B8 >
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
		/**
		 *\~english
		 *\brief			Conversion from static source format to dynamic destination format
		 *\remark			Only for colour pixel formats, offsets source and destination by respective pixel format sizes
		 *\param[in,out]	p_pSrc			The source pixel
		 *\param[in,out]	p_pDst			The destination pixel
		 *\param[in]		p_ePixelFmtDst	The destination pixel format
		 *\~french
		 *\brief			Conversion d'une source statique vers une destination dynamique
		 *\remark			Uniquement pour les formats de pixel couleurs, décale la source et la destination par la taille respective de leur format de pixel
		 *\param[in,out]	p_pSrc			Le pixel source
		 *\param[in,out]	p_pDst			Le pixel destination
		 *\param[in]		p_ePixelFmtDst	Le format du pixel destination
		 */
		template< TPL_PIXEL_FORMAT PF > void DynamicColourConversion( uint8_t const *& p_pSrc, uint8_t *& p_pDst, ePIXEL_FORMAT p_ePixelFmtDst )
		{
			switch ( p_ePixelFmtDst )
			{
			case ePIXEL_FORMAT_L8:
				pixel_converter< PF, ePIXEL_FORMAT_L8 >()( p_pSrc, p_pDst );
				break;

			case ePIXEL_FORMAT_L16F32F:
				pixel_converter< PF, ePIXEL_FORMAT_L16F32F >()( p_pSrc, p_pDst );
				break;

			case ePIXEL_FORMAT_L32F:
				pixel_converter< PF, ePIXEL_FORMAT_L32F >()( p_pSrc, p_pDst );
				break;

			case ePIXEL_FORMAT_A8L8:
				pixel_converter< PF, ePIXEL_FORMAT_A8L8 >()( p_pSrc, p_pDst );
				break;

			case ePIXEL_FORMAT_AL16F32F:
				pixel_converter< PF, ePIXEL_FORMAT_AL16F32F >()( p_pSrc, p_pDst );
				break;

			case ePIXEL_FORMAT_AL32F:
				pixel_converter< PF, ePIXEL_FORMAT_AL32F >()( p_pSrc, p_pDst );
				break;

			case ePIXEL_FORMAT_A1R5G5B5:
				pixel_converter< PF, ePIXEL_FORMAT_A1R5G5B5 >()( p_pSrc, p_pDst );
				break;

			case ePIXEL_FORMAT_A4R4G4B4:
				pixel_converter< PF, ePIXEL_FORMAT_A4R4G4B4 >()( p_pSrc, p_pDst );
				break;

			case ePIXEL_FORMAT_R5G6B5:
				pixel_converter< PF, ePIXEL_FORMAT_R5G6B5 >()( p_pSrc, p_pDst );
				break;

			case ePIXEL_FORMAT_R8G8B8:
				pixel_converter< PF, ePIXEL_FORMAT_R8G8B8 >()( p_pSrc, p_pDst );
				break;

			case ePIXEL_FORMAT_B8G8R8:
				pixel_converter< PF, ePIXEL_FORMAT_B8G8R8 >()( p_pSrc, p_pDst );
				break;

			case ePIXEL_FORMAT_A8R8G8B8:
				pixel_converter< PF, ePIXEL_FORMAT_A8R8G8B8 >()( p_pSrc, p_pDst );
				break;

			case ePIXEL_FORMAT_A8B8G8R8:
				pixel_converter< PF, ePIXEL_FORMAT_A8B8G8R8 >()( p_pSrc, p_pDst );
				break;

			case ePIXEL_FORMAT_RGB16F32F:
				pixel_converter< PF, ePIXEL_FORMAT_RGB16F32F >()( p_pSrc, p_pDst );
				break;

			case ePIXEL_FORMAT_ARGB16F32F:
				pixel_converter< PF, ePIXEL_FORMAT_ARGB16F32F >()( p_pSrc, p_pDst );
				break;

			case ePIXEL_FORMAT_RGB32F:
				pixel_converter< PF, ePIXEL_FORMAT_RGB32F >()( p_pSrc, p_pDst );
				break;

			case ePIXEL_FORMAT_ARGB32F:
				pixel_converter< PF, ePIXEL_FORMAT_ARGB32F >()( p_pSrc, p_pDst );
				break;

			case ePIXEL_FORMAT_YUY2:
				pixel_converter< PF, ePIXEL_FORMAT_YUY2 >()( p_pSrc, p_pDst );
				break;

			default:
				UNSUPPORTED_ERROR( "No conversion defined" );
				break;
			}
		}
		/**
		 *\~english
		 *\brief			Conversion from static source format to dynamic destination format
		 *\remark			Only for depth pixel formats, offsets source and destination by respective pixel format sizes
		 *\param[in,out]	p_pSrc			The source pixel
		 *\param[in,out]	p_pDst			The destination pixel
		 *\param[in]		p_ePixelFmtDst	The destination pixel format
		 *\~french
		 *\brief			Conversion d'une source statique vers une destination dynamique
		 *\remark			Uniquement pour les formats de pixel profondeurs, décale la source et la destination par la taille respective de leur format de pixel
		 *\param[in,out]	p_pSrc			Le pixel source
		 *\param[in,out]	p_pDst			Le pixel destination
		 *\param[in]		p_ePixelFmtDst	Le format du pixel destination
		 */
		template< TPL_PIXEL_FORMAT PF > void DynamicDepthConversion( uint8_t const *& p_pSrc, uint8_t *& p_pDst, ePIXEL_FORMAT p_ePixelFmtDst )
		{
			switch ( p_ePixelFmtDst )
			{
			case ePIXEL_FORMAT_DEPTH16:
				pixel_converter< PF, ePIXEL_FORMAT_DEPTH16 >()( p_pSrc, p_pDst );
				break;

			case ePIXEL_FORMAT_DEPTH24:
				pixel_converter< PF, ePIXEL_FORMAT_DEPTH24 >()( p_pSrc, p_pDst );
				break;

			case ePIXEL_FORMAT_DEPTH24S8:
				pixel_converter< PF, ePIXEL_FORMAT_DEPTH24S8 >()( p_pSrc, p_pDst );
				break;

			case ePIXEL_FORMAT_DEPTH32:
				pixel_converter< PF, ePIXEL_FORMAT_DEPTH32 >()( p_pSrc, p_pDst );
				break;

			default:
				UNSUPPORTED_ERROR( "No conversion defined" );
				break;
			}
		}
		/**
		 *\~english
		 *\brief			Conversion from static source format to dynamic destination format
		 *\remark			Only for stencil pixel formats, offsets source and destination by respective pixel format sizes
		 *\param[in,out]	p_pSrc			The source pixel
		 *\param[in,out]	p_pDst			The destination pixel
		 *\param[in]		p_ePixelFmtDst	The destination pixel format
		 *\~french
		 *\brief			Conversion d'une source statique vers une destination dynamique
		 *\remark			Uniquement pour les formats de pixel stencil, décale la source et la destination par la taille respective de leur format de pixel
		 *\param[in,out]	p_pSrc			Le pixel source
		 *\param[in,out]	p_pDst			Le pixel destination
		 *\param[in]		p_ePixelFmtDst	Le format du pixel destination
		 */
		template< TPL_PIXEL_FORMAT PF > void DynamicStencilConversion( uint8_t const *& p_pSrc, uint8_t *& p_pDst, ePIXEL_FORMAT p_ePixelFmtDst )
		{
			switch ( p_ePixelFmtDst )
			{
			case ePIXEL_FORMAT_DEPTH24S8:
				pixel_converter< PF, ePIXEL_FORMAT_DEPTH24S8 >()( p_pSrc, p_pDst );
				break;

			case ePIXEL_FORMAT_STENCIL1:
				pixel_converter< PF, ePIXEL_FORMAT_STENCIL1 >()( p_pSrc, p_pDst );
				break;

			case ePIXEL_FORMAT_STENCIL8:
				pixel_converter< PF, ePIXEL_FORMAT_STENCIL8 >()( p_pSrc, p_pDst );
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
		template< TPL_PIXEL_FORMAT PF > void DynamicColourBufferConversion( uint8_t const * p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t * p_pDstBuffer, uint32_t p_uiDstSize )
		{
			switch ( p_eDstFormat )
			{
			case ePIXEL_FORMAT_L8:
				buffer_converter< PF, ePIXEL_FORMAT_L8 >()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );
				break;

			case ePIXEL_FORMAT_L16F32F:
				buffer_converter< PF, ePIXEL_FORMAT_L16F32F >()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );
				break;

			case ePIXEL_FORMAT_L32F:
				buffer_converter< PF, ePIXEL_FORMAT_L32F >()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );
				break;

			case ePIXEL_FORMAT_A8L8:
				buffer_converter< PF, ePIXEL_FORMAT_A8L8 >()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );
				break;

			case ePIXEL_FORMAT_AL16F32F:
				buffer_converter< PF, ePIXEL_FORMAT_AL16F32F >()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );
				break;

			case ePIXEL_FORMAT_AL32F:
				buffer_converter< PF, ePIXEL_FORMAT_AL32F >()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );
				break;

			case ePIXEL_FORMAT_A1R5G5B5:
				buffer_converter< PF, ePIXEL_FORMAT_A1R5G5B5 >()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );
				break;

			case ePIXEL_FORMAT_A4R4G4B4:
				buffer_converter< PF, ePIXEL_FORMAT_A4R4G4B4 >()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );
				break;

			case ePIXEL_FORMAT_R5G6B5:
				buffer_converter< PF, ePIXEL_FORMAT_R5G6B5 >()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );
				break;

			case ePIXEL_FORMAT_R8G8B8:
				buffer_converter< PF, ePIXEL_FORMAT_R8G8B8 >()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );
				break;

			case ePIXEL_FORMAT_B8G8R8:
				buffer_converter< PF, ePIXEL_FORMAT_B8G8R8 >()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );
				break;

			case ePIXEL_FORMAT_A8R8G8B8:
				buffer_converter< PF, ePIXEL_FORMAT_A8R8G8B8 >()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );
				break;

			case ePIXEL_FORMAT_A8B8G8R8:
				buffer_converter< PF, ePIXEL_FORMAT_A8B8G8R8 >()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );
				break;

			case ePIXEL_FORMAT_RGB16F32F:
				buffer_converter< PF, ePIXEL_FORMAT_RGB16F32F >()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );
				break;

			case ePIXEL_FORMAT_ARGB16F32F:
				buffer_converter< PF, ePIXEL_FORMAT_ARGB16F32F >()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );
				break;

			case ePIXEL_FORMAT_RGB32F:
				buffer_converter< PF, ePIXEL_FORMAT_RGB32F >()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );
				break;

			case ePIXEL_FORMAT_ARGB32F:
				buffer_converter< PF, ePIXEL_FORMAT_ARGB32F >()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );
				break;

			case ePIXEL_FORMAT_YUY2:
				buffer_converter< PF, ePIXEL_FORMAT_YUY2 >()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );
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
		template< TPL_PIXEL_FORMAT PF > void DynamicDepthBufferConversion( uint8_t const * p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t * p_pDstBuffer, uint32_t p_uiDstSize )
		{
			switch ( p_eDstFormat )
			{
			case ePIXEL_FORMAT_DEPTH16:
				buffer_converter< PF, ePIXEL_FORMAT_DEPTH16 >()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );
				break;

			case ePIXEL_FORMAT_DEPTH24:
				buffer_converter< PF, ePIXEL_FORMAT_DEPTH24 >()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );
				break;

			case ePIXEL_FORMAT_DEPTH24S8:
				buffer_converter< PF, ePIXEL_FORMAT_DEPTH24S8 >()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );
				break;

			case ePIXEL_FORMAT_DEPTH32:
				buffer_converter< PF, ePIXEL_FORMAT_DEPTH32 >()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );
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
		template< TPL_PIXEL_FORMAT PF > void DynamicStencilBufferConversion( uint8_t const * p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t * p_pDstBuffer, uint32_t p_uiDstSize )
		{
			switch ( p_eDstFormat )
			{
			case ePIXEL_FORMAT_DEPTH24S8:
				buffer_converter< PF, ePIXEL_FORMAT_DEPTH24S8 >()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );
				break;

			case ePIXEL_FORMAT_STENCIL1:
				buffer_converter< PF, ePIXEL_FORMAT_STENCIL1 >()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );
				break;

			case ePIXEL_FORMAT_STENCIL8:
				buffer_converter< PF, ePIXEL_FORMAT_STENCIL8 >()( p_pSrcBuffer, p_uiSrcSize, p_pDstBuffer, p_uiDstSize );
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

	//!\~english Specialisation for ePIXEL_FORMAT_L8	\~french Spécialisation pour ePIXEL_FORMAT_L8
	template <> struct pixel_definitions< ePIXEL_FORMAT_L8 >
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
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, ePIXEL_FORMAT p_ePixelFmtDst )
		{
			detail::DynamicColourConversion< ePIXEL_FORMAT_L8 >( p_pSrc, p_pDst, p_ePixelFmtDst );
		}
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize )
		{
			detail::DynamicColourBufferConversion< ePIXEL_FORMAT_L8 >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
		}
		template< TPL_PIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
		{
			detail::pixel_converter< ePIXEL_FORMAT_L8, PF >()( p_pSrc, p_pDst );
		}
	};

	//*************************************************************************************************

	//!\~english Specialisation for ePIXEL_FORMAT_L16F32F	\~french Spécialisation pour ePIXEL_FORMAT_L16F32F
	template <> struct pixel_definitions< ePIXEL_FORMAT_L16F32F >
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
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, ePIXEL_FORMAT p_ePixelFmtDst )
		{
			detail::DynamicColourConversion< ePIXEL_FORMAT_L16F32F >( p_pSrc, p_pDst, p_ePixelFmtDst );
		}
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize )
		{
			detail::DynamicColourBufferConversion< ePIXEL_FORMAT_L16F32F >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
		}
		template< TPL_PIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
		{
			detail::pixel_converter< ePIXEL_FORMAT_L16F32F, PF >()( p_pSrc, p_pDst );
		}
	};

	//*************************************************************************************************

	//!\~english Specialisation for ePIXEL_FORMAT_L32F	\~french Spécialisation pour ePIXEL_FORMAT_L32F
	template <> struct pixel_definitions< ePIXEL_FORMAT_L32F >
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
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, ePIXEL_FORMAT p_ePixelFmtDst )
		{
			detail::DynamicColourConversion< ePIXEL_FORMAT_L32F >( p_pSrc, p_pDst, p_ePixelFmtDst );
		}
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize )
		{
			detail::DynamicColourBufferConversion< ePIXEL_FORMAT_L32F >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
		}
		template< TPL_PIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
		{
			detail::pixel_converter< ePIXEL_FORMAT_L32F, PF >()( p_pSrc, p_pDst );
		}
	};

	//*************************************************************************************************

	//!\~english Specialisation for ePIXEL_FORMAT_A8L8	\~french Spécialisation pour ePIXEL_FORMAT_A8L8
	template <> struct pixel_definitions< ePIXEL_FORMAT_A8L8 >
	{
		static const uint8_t Size = 2;
		static const bool Alpha = true;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static const ePIXEL_FORMAT NoAlphaPF = ePIXEL_FORMAT_L8;
		static inline String to_string()
		{
			return cuT( "16 bits luminosity and alpha" );
		}
		static inline String to_str()
		{
			return cuT( "al16" );
		}
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, ePIXEL_FORMAT p_ePixelFmtDst )
		{
			detail::DynamicColourConversion< ePIXEL_FORMAT_A8L8 >( p_pSrc, p_pDst, p_ePixelFmtDst );
		}
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize )
		{
			detail::DynamicColourBufferConversion< ePIXEL_FORMAT_A8L8 >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
		}
		template< TPL_PIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
		{
			detail::pixel_converter< ePIXEL_FORMAT_A8L8, PF >()( p_pSrc, p_pDst );
		}
	};

	//*************************************************************************************************

	//!\~english Specialisation for ePIXEL_FORMAT_AL32F	\~french Spécialisation pour ePIXEL_FORMAT_AL32F
	template <> struct pixel_definitions< ePIXEL_FORMAT_AL32F >
	{
		static const uint8_t Size = 8;
		static const bool Alpha = true;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static const ePIXEL_FORMAT NoAlphaPF = ePIXEL_FORMAT_L32F;
		static inline String to_string()
		{
			return cuT( "Floats luminosity and alpha" );
		}
		static inline String to_str()
		{
			return cuT( "al32f" );
		}
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, ePIXEL_FORMAT p_ePixelFmtDst )
		{
			detail::DynamicColourConversion< ePIXEL_FORMAT_AL32F >( p_pSrc, p_pDst, p_ePixelFmtDst );
		}
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize )
		{
			detail::DynamicColourBufferConversion< ePIXEL_FORMAT_AL32F >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
		}
		template< TPL_PIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
		{
			detail::pixel_converter< ePIXEL_FORMAT_AL32F, PF >()( p_pSrc, p_pDst );
		}
	};

	//*************************************************************************************************

	//!\~english Specialisation for ePIXEL_FORMAT_AL16F32F	\~french Spécialisation pour ePIXEL_FORMAT_AL16F32F
	template <> struct pixel_definitions< ePIXEL_FORMAT_AL16F32F >
	{
		static const uint8_t Size = 8;
		static const bool Alpha = true;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static const ePIXEL_FORMAT NoAlphaPF = ePIXEL_FORMAT_L16F32F;
		static inline String to_string()
		{
			return cuT( "Half floats luminosity and alpha" );
		}
		static inline String to_str()
		{
			return cuT( "al16f" );
		}
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, ePIXEL_FORMAT p_ePixelFmtDst )
		{
			detail::DynamicColourConversion< ePIXEL_FORMAT_AL16F32F >( p_pSrc, p_pDst, p_ePixelFmtDst );
		}
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize )
		{
			detail::DynamicColourBufferConversion< ePIXEL_FORMAT_AL16F32F >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
		}
		template< TPL_PIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
		{
			detail::pixel_converter< ePIXEL_FORMAT_AL16F32F, PF >()( p_pSrc, p_pDst );
		}
	};

	//*************************************************************************************************

	//!\~english Specialisation for ePIXEL_FORMAT_A1R5G5B5	\~french Spécialisation pour ePIXEL_FORMAT_A1R5G5B5
	template <> struct pixel_definitions< ePIXEL_FORMAT_A1R5G5B5 >
	{
		static const uint8_t Size = 2;
		static const bool Alpha = true;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static const ePIXEL_FORMAT NoAlphaPF = ePIXEL_FORMAT_R5G6B5;
		static inline String to_string()
		{
			return cuT( "16 bits 5551 ARGB" );
		}
		static inline String to_str()
		{
			return cuT( "argb1555" );
		}
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, ePIXEL_FORMAT p_ePixelFmtDst )
		{
			detail::DynamicColourConversion< ePIXEL_FORMAT_A1R5G5B5 >( p_pSrc, p_pDst, p_ePixelFmtDst );
		}
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize )
		{
			detail::DynamicColourBufferConversion< ePIXEL_FORMAT_A1R5G5B5 >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
		}
		template< TPL_PIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
		{
			detail::pixel_converter< ePIXEL_FORMAT_A1R5G5B5, PF >()( p_pSrc, p_pDst );
		}
	};

	//*************************************************************************************************

	//!\~english Specialisation for ePIXEL_FORMAT_R5G6B5	\~french Spécialisation pour ePIXEL_FORMAT_R5G6B5
	template <> struct pixel_definitions< ePIXEL_FORMAT_R5G6B5 >
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
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, ePIXEL_FORMAT p_ePixelFmtDst )
		{
			detail::DynamicColourConversion< ePIXEL_FORMAT_R5G6B5 >( p_pSrc, p_pDst, p_ePixelFmtDst );
		}
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize )
		{
			detail::DynamicColourBufferConversion< ePIXEL_FORMAT_R5G6B5 >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
		}
		template< TPL_PIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
		{
			detail::pixel_converter< ePIXEL_FORMAT_R5G6B5, PF >()( p_pSrc, p_pDst );
		}
	};

	//*************************************************************************************************

	//!\~english Specialisation for ePIXEL_FORMAT_A4R4G4B4	\~french Spécialisation pour ePIXEL_FORMAT_A4R4G4B4
	template <> struct pixel_definitions< ePIXEL_FORMAT_A4R4G4B4 >
	{
		static const uint8_t Size = 2;
		static const bool Alpha = true;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static const ePIXEL_FORMAT NoAlphaPF = ePIXEL_FORMAT_R5G6B5;
		static inline String to_string()
		{
			return cuT( "16 bits 4444 ARGB" );
		}
		static inline String to_str()
		{
			return cuT( "argb16" );
		}
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, ePIXEL_FORMAT p_ePixelFmtDst )
		{
			detail::DynamicColourConversion< ePIXEL_FORMAT_A4R4G4B4 >( p_pSrc, p_pDst, p_ePixelFmtDst );
		}
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize )
		{
			detail::DynamicColourBufferConversion< ePIXEL_FORMAT_A4R4G4B4 >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
		}
		template< TPL_PIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
		{
			detail::pixel_converter< ePIXEL_FORMAT_A4R4G4B4, PF >()( p_pSrc, p_pDst );
		}
	};

	//*************************************************************************************************

	//!\~english Specialisation for ePIXEL_FORMAT_R8G8B8	\~french Spécialisation pour ePIXEL_FORMAT_R8G8B8
	template <> struct pixel_definitions< ePIXEL_FORMAT_R8G8B8 >
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
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, ePIXEL_FORMAT p_ePixelFmtDst )
		{
			detail::DynamicColourConversion< ePIXEL_FORMAT_R8G8B8 >( p_pSrc, p_pDst, p_ePixelFmtDst );
		}
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize )
		{
			detail::DynamicColourBufferConversion< ePIXEL_FORMAT_R8G8B8 >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
		}
		template< TPL_PIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
		{
			detail::pixel_converter< ePIXEL_FORMAT_R8G8B8, PF >()( p_pSrc, p_pDst );
		}
	};

	//*************************************************************************************************

	//!\~english Specialisation for ePIXEL_FORMAT_B8G8R8	\~french Spécialisation pour ePIXEL_FORMAT_B8G8R8
	template <> struct pixel_definitions< ePIXEL_FORMAT_B8G8R8 >
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
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, ePIXEL_FORMAT p_ePixelFmtDst )
		{
			detail::DynamicColourConversion< ePIXEL_FORMAT_B8G8R8 >( p_pSrc, p_pDst, p_ePixelFmtDst );
		}
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize )
		{
			detail::DynamicColourBufferConversion< ePIXEL_FORMAT_B8G8R8 >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
		}
		template< TPL_PIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
		{
			detail::pixel_converter< ePIXEL_FORMAT_B8G8R8, PF >()( p_pSrc, p_pDst );
		}
	};

	//*************************************************************************************************

	//!\~english Specialisation for ePIXEL_FORMAT_A8R8G8B8	\~french Spécialisation pour ePIXEL_FORMAT_A8R8G8B8
	template <> struct pixel_definitions< ePIXEL_FORMAT_A8R8G8B8 >
	{
		static const uint8_t Size = 4;
		static const bool Alpha = true;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static const ePIXEL_FORMAT NoAlphaPF = ePIXEL_FORMAT_R8G8B8;
		static inline String to_string()
		{
			return cuT( "32 bits 8888 ARGB" );
		}
		static inline String to_str()
		{
			return cuT( "argb32" );
		}
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, ePIXEL_FORMAT p_ePixelFmtDst )
		{
			detail::DynamicColourConversion< ePIXEL_FORMAT_A8R8G8B8 >( p_pSrc, p_pDst, p_ePixelFmtDst );
		}
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize )
		{
			detail::DynamicColourBufferConversion< ePIXEL_FORMAT_A8R8G8B8 >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
		}
		template< TPL_PIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
		{
			detail::pixel_converter< ePIXEL_FORMAT_A8R8G8B8, PF >()( p_pSrc, p_pDst );
		}
	};

	//*************************************************************************************************

	//!\~english Specialisation for ePIXEL_FORMAT_A8B8G8R8	\~french Spécialisation pour ePIXEL_FORMAT_A8B8G8R8
	template <> struct pixel_definitions< ePIXEL_FORMAT_A8B8G8R8 >
	{
		static const uint8_t Size = 4;
		static const bool Alpha = true;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static const ePIXEL_FORMAT NoAlphaPF = ePIXEL_FORMAT_B8G8R8;
		static inline String to_string()
		{
			return cuT( "32 bits 8888 ABGR" );
		}
		static inline String to_str()
		{
			return cuT( "abgr32" );
		}
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, ePIXEL_FORMAT p_ePixelFmtDst )
		{
			detail::DynamicColourConversion< ePIXEL_FORMAT_A8B8G8R8 >( p_pSrc, p_pDst, p_ePixelFmtDst );
		}
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize )
		{
			detail::DynamicColourBufferConversion< ePIXEL_FORMAT_A8B8G8R8 >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
		}
		template< TPL_PIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
		{
			detail::pixel_converter< ePIXEL_FORMAT_A8B8G8R8, PF >()( p_pSrc, p_pDst );
		}
	};

	//*************************************************************************************************

	//!\~english Specialisation for ePIXEL_FORMAT_RGB16F32F	\~french Spécialisation pour ePIXEL_FORMAT_RGB16F32F
	template <> struct pixel_definitions< ePIXEL_FORMAT_RGB16F32F >
	{
		static const uint8_t Size = 12;
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
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, ePIXEL_FORMAT p_ePixelFmtDst )
		{
			detail::DynamicColourConversion< ePIXEL_FORMAT_RGB32F >( p_pSrc, p_pDst, p_ePixelFmtDst );
		}
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize )
		{
			detail::DynamicColourBufferConversion< ePIXEL_FORMAT_RGB32F >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
		}
		template< TPL_PIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
		{
			detail::pixel_converter< ePIXEL_FORMAT_RGB32F, PF >()( p_pSrc, p_pDst );
		}
	};

	//*************************************************************************************************

	//!\~english Specialisation for ePIXEL_FORMAT_ARGB16F32F	\~french Spécialisation pour ePIXEL_FORMAT_ARGB16F32F
	template <> struct pixel_definitions< ePIXEL_FORMAT_ARGB16F32F >
	{
		static const uint8_t Size = 16;
		static const bool Alpha = true;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static const ePIXEL_FORMAT NoAlphaPF = ePIXEL_FORMAT_RGB16F32F;
		static inline String to_string()
		{
			return cuT( "Half floating point ARGB" );
		}
		static inline String to_str()
		{
			return cuT( "argb16f" );
		}
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, ePIXEL_FORMAT p_ePixelFmtDst )
		{
			detail::DynamicColourConversion< ePIXEL_FORMAT_ARGB32F >( p_pSrc, p_pDst, p_ePixelFmtDst );
		}
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize )
		{
			detail::DynamicColourBufferConversion< ePIXEL_FORMAT_ARGB32F >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
		}
		template< TPL_PIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
		{
			detail::pixel_converter< ePIXEL_FORMAT_ARGB32F, PF >()( p_pSrc, p_pDst );
		}
	};

	//*************************************************************************************************

	//!\~english Specialisation for ePIXEL_FORMAT_RGB32F	\~french Spécialisation pour ePIXEL_FORMAT_RGB32F
	template <> struct pixel_definitions< ePIXEL_FORMAT_RGB32F >
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
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, ePIXEL_FORMAT p_ePixelFmtDst )
		{
			detail::DynamicColourConversion< ePIXEL_FORMAT_RGB32F >( p_pSrc, p_pDst, p_ePixelFmtDst );
		}
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize )
		{
			detail::DynamicColourBufferConversion< ePIXEL_FORMAT_RGB32F >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
		}
		template< TPL_PIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
		{
			detail::pixel_converter< ePIXEL_FORMAT_RGB32F, PF >()( p_pSrc, p_pDst );
		}
	};

	//*************************************************************************************************

	//!\~english Specialisation for ePIXEL_FORMAT_ARGB32F	\~french Spécialisation pour ePIXEL_FORMAT_ARGB32F
	template <> struct pixel_definitions< ePIXEL_FORMAT_ARGB32F >
	{
		static const uint8_t Size = 16;
		static const bool Alpha = true;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static const ePIXEL_FORMAT NoAlphaPF = ePIXEL_FORMAT_RGB32F;
		static inline String to_string()
		{
			return cuT( "Floating point ARGB" );
		}
		static inline String to_str()
		{
			return cuT( "argb32f" );
		}
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, ePIXEL_FORMAT p_ePixelFmtDst )
		{
			detail::DynamicColourConversion< ePIXEL_FORMAT_ARGB32F >( p_pSrc, p_pDst, p_ePixelFmtDst );
		}
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize )
		{
			detail::DynamicColourBufferConversion< ePIXEL_FORMAT_ARGB32F >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
		}
		template< TPL_PIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
		{
			detail::pixel_converter< ePIXEL_FORMAT_ARGB32F, PF >()( p_pSrc, p_pDst );
		}
	};

	//*************************************************************************************************

	//!\~english Specialisation for ePIXEL_FORMAT_DXTC1	\~french Spécialisation pour ePIXEL_FORMAT_DXTC1
	template <> struct pixel_definitions< ePIXEL_FORMAT_DXTC1 >
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
		static inline void convert( uint8_t const *, uint8_t *, ePIXEL_FORMAT )
		{
			UNSUPPORTED_ERROR( "No conversion from " + str_utils::to_str( to_string() ) + " defined" );
		}
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize )
		{
			UNSUPPORTED_ERROR( "No conversion from " + str_utils::to_str( to_string() ) + " defined" );
		}
		template< TPL_PIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
		{
			detail::pixel_converter< ePIXEL_FORMAT_DXTC1, PF >()( p_pSrc, p_pDst );
		}
	};

	//*************************************************************************************************

	//!\~english Specialisation for ePIXEL_FORMAT_DXTC3	\~french Spécialisation pour ePIXEL_FORMAT_DXTC3
	template <> struct pixel_definitions< ePIXEL_FORMAT_DXTC3 >
	{
		static const uint8_t Size = 2;
		static const bool Alpha = true;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = true;
		static const ePIXEL_FORMAT NoAlphaPF = ePIXEL_FORMAT_DXTC3;
		static inline String to_string()
		{
			return cuT( "DXT3 16 bits compressed format" );
		}
		static inline String to_str()
		{
			return cuT( "dxtc3" );
		}
		static inline void convert( uint8_t const *, uint8_t *, ePIXEL_FORMAT )
		{
			UNSUPPORTED_ERROR( "No conversion from " + str_utils::to_str( to_string() ) + " defined" );
		}
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize )
		{
			UNSUPPORTED_ERROR( "No conversion from " + str_utils::to_str( to_string() ) + " defined" );
		}
		template< TPL_PIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
		{
			detail::pixel_converter< ePIXEL_FORMAT_DXTC3, PF >()( p_pSrc, p_pDst );
		}
	};

	//*************************************************************************************************

	//!\~english Specialisation for ePIXEL_FORMAT_DXTC5	\~french Spécialisation pour ePIXEL_FORMAT_DXTC5
	template <> struct pixel_definitions< ePIXEL_FORMAT_DXTC5 >
	{
		static const uint8_t Size = 2;
		static const bool Alpha = true;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = true;
		static const ePIXEL_FORMAT NoAlphaPF = ePIXEL_FORMAT_DXTC5;
		static inline String to_string()
		{
			return cuT( "DXT5 16 bits compressed format" );
		}
		static inline String to_str()
		{
			return cuT( "dxtc5" );
		}
		static inline void convert( uint8_t const *, uint8_t *, ePIXEL_FORMAT )
		{
			UNSUPPORTED_ERROR( "No conversion from " + str_utils::to_str( to_string() ) + " defined" );
		}
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize )
		{
			UNSUPPORTED_ERROR( "No conversion from " + str_utils::to_str( to_string() ) + " defined" );
		}
		template< TPL_PIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
		{
			detail::pixel_converter< ePIXEL_FORMAT_DXTC5, PF >()( p_pSrc, p_pDst );
		}
	};

	//*************************************************************************************************

	//!\~english Specialisation for ePIXEL_FORMAT_YUY2	\~french Spécialisation pour ePIXEL_FORMAT_YUY2
	template <> struct pixel_definitions< ePIXEL_FORMAT_YUY2 >
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
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, ePIXEL_FORMAT p_ePixelFmtDst )
		{
			detail::DynamicColourConversion< ePIXEL_FORMAT_YUY2 >( p_pSrc, p_pDst, p_ePixelFmtDst );
		}
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize )
		{
			detail::DynamicColourBufferConversion< ePIXEL_FORMAT_YUY2 >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
		}
		template< TPL_PIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
		{
			detail::pixel_converter< ePIXEL_FORMAT_YUY2, PF >()( p_pSrc, p_pDst );
		}
	};

	//*************************************************************************************************

	//!\~english Specialisation for ePIXEL_FORMAT_DEPTH16	\~french Spécialisation pour ePIXEL_FORMAT_DEPTH16
	template <> struct pixel_definitions< ePIXEL_FORMAT_DEPTH16 >
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
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, ePIXEL_FORMAT p_ePixelFmtDst )
		{
			detail::DynamicDepthConversion< ePIXEL_FORMAT_DEPTH16 >( p_pSrc, p_pDst, p_ePixelFmtDst );
		}
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize )
		{
			detail::DynamicDepthBufferConversion< ePIXEL_FORMAT_DEPTH16 >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
		}
		template< TPL_PIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
		{
			detail::pixel_converter< ePIXEL_FORMAT_DEPTH16, PF >()( p_pSrc, p_pDst );
		}
	};

	//*************************************************************************************************

	//!\~english Specialisation for ePIXEL_FORMAT_DEPTH24	\~french Spécialisation pour ePIXEL_FORMAT_DEPTH24
	template <> struct pixel_definitions< ePIXEL_FORMAT_DEPTH24 >
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
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, ePIXEL_FORMAT p_ePixelFmtDst )
		{
			detail::DynamicDepthConversion< ePIXEL_FORMAT_DEPTH24 >( p_pSrc, p_pDst, p_ePixelFmtDst );
		}
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize )
		{
			detail::DynamicDepthBufferConversion< ePIXEL_FORMAT_DEPTH24 >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
		}
		template< TPL_PIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
		{
			detail::pixel_converter< ePIXEL_FORMAT_DEPTH24, PF >()( p_pSrc, p_pDst );
		}
	};

	//*************************************************************************************************

	//!\~english Specialisation for ePIXEL_FORMAT_DEPTH24S8	\~french Spécialisation pour ePIXEL_FORMAT_DEPTH24S8
	template <> struct pixel_definitions< ePIXEL_FORMAT_DEPTH24S8 >
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
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, ePIXEL_FORMAT p_ePixelFmtDst )
		{
			detail::DynamicDepthConversion< ePIXEL_FORMAT_DEPTH24S8 >( p_pSrc, p_pDst, p_ePixelFmtDst );
		}
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize )
		{
			if ( p_eDstFormat == ePIXEL_FORMAT_STENCIL8 || p_eDstFormat == ePIXEL_FORMAT_STENCIL1 )
			{
				detail::DynamicStencilBufferConversion< ePIXEL_FORMAT_DEPTH24S8 >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
			}
			else
			{
				detail::DynamicDepthBufferConversion< ePIXEL_FORMAT_DEPTH24S8 >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
			}
		}
		template< TPL_PIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
		{
			detail::pixel_converter<ePIXEL_FORMAT_DEPTH24S8, PF>()( p_pSrc, p_pDst );
		}
	};

	//*************************************************************************************************

	//!\~english Specialisation for ePIXEL_FORMAT_DEPTH32	\~french Spécialisation pour ePIXEL_FORMAT_DEPTH32
	template <> struct pixel_definitions< ePIXEL_FORMAT_DEPTH32 >
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
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, ePIXEL_FORMAT p_ePixelFmtDst )
		{
			detail::DynamicDepthConversion< ePIXEL_FORMAT_DEPTH32 >( p_pSrc, p_pDst, p_ePixelFmtDst );
		}
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize )
		{
			detail::DynamicDepthBufferConversion< ePIXEL_FORMAT_DEPTH32 >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
		}
		template< TPL_PIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
		{
			detail::pixel_converter< ePIXEL_FORMAT_DEPTH32, PF >()( p_pSrc, p_pDst );
		}
	};

	//*************************************************************************************************

	//!\~english Specialisation for ePIXEL_FORMAT_STENCIL1	\~french Spécialisation pour ePIXEL_FORMAT_STENCIL1
	template <> struct pixel_definitions< ePIXEL_FORMAT_STENCIL1 >
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
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, ePIXEL_FORMAT p_ePixelFmtDst )
		{
			detail::DynamicStencilConversion< ePIXEL_FORMAT_STENCIL1 >( p_pSrc, p_pDst, p_ePixelFmtDst );
		}
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize )
		{
			detail::DynamicStencilBufferConversion< ePIXEL_FORMAT_STENCIL1 >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
		}
		template< TPL_PIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
		{
			detail::pixel_converter< ePIXEL_FORMAT_STENCIL1, PF >()( p_pSrc, p_pDst );
		}
	};

	//*************************************************************************************************

	//!\~english Specialisation for ePIXEL_FORMAT_STENCIL8	\~french Spécialisation pour ePIXEL_FORMAT_STENCIL8
	template <> struct pixel_definitions< ePIXEL_FORMAT_STENCIL8 >
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
		static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst, ePIXEL_FORMAT p_ePixelFmtDst )
		{
			detail::DynamicStencilConversion< ePIXEL_FORMAT_STENCIL8 >( p_pSrc, p_pDst, p_ePixelFmtDst );
		}
		static inline void convert( uint8_t const *& p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t *& p_pDstBuffer, uint32_t p_uiDstSize )
		{
			detail::DynamicStencilBufferConversion< ePIXEL_FORMAT_STENCIL8 >( p_pSrcBuffer, p_uiSrcSize, p_eDstFormat, p_pDstBuffer, p_uiDstSize );
		}
		template< TPL_PIXEL_FORMAT PF > static inline void convert( uint8_t const *& p_pSrc, uint8_t *& p_pDst )
		{
			detail::pixel_converter< ePIXEL_FORMAT_STENCIL8, PF >()( p_pSrc, p_pDst );
		}
	};
}
