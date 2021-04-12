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
	//*************************************************************************************************

	namespace PF
	{
		namespace details
		{
			template< PixelFormat PF, typename Enable = void >
			struct PixelComponentTraits;

			template< PixelFormat PF >
			struct PixelComponentTraits< PF
				, typename std::enable_if< IsColourFormat< PF >::value >::type >
			{
				static float getFloat( Pixel< PF > const & pixel
					, PixelComponent component )
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

				static void setFloat( Pixel< PF > & pixel
					, PixelComponent component
					, float value )
				{
					switch ( component )
					{
					case PixelComponent::eRed:
						PixelComponents< PF >::R8U( pixel.ptr(), value );
						break;

					case PixelComponent::eGreen:
						PixelComponents< PF >::G8U( pixel.ptr(), value );
						break;

					case PixelComponent::eBlue:
						PixelComponents< PF >::B8U( pixel.ptr(), value );
						break;

					case PixelComponent::eAlpha:
						PixelComponents< PF >::A8U( pixel.ptr(), value );
						break;

					case PixelComponent::eLuminance:
						PixelComponents< PF >::L8U( pixel.ptr(), value );
						break;

					default:
						break;
					}
				}

				static uint8_t getByte( Pixel< PF > const & pixel
					, PixelComponent component )
				{
					uint8_t result = 0;

					switch ( component )
					{
					case PixelComponent::eRed:
						result = PixelComponents< PF >::R8U( pixel.constPtr() );
						break;

					case PixelComponent::eGreen:
						result = PixelComponents< PF >::G8U( pixel.constPtr() );
						break;

					case PixelComponent::eBlue:
						result = PixelComponents< PF >::B8U( pixel.constPtr() );
						break;

					case PixelComponent::eAlpha:
						result = PixelComponents< PF >::A8U( pixel.constPtr() );
						break;

					case PixelComponent::eLuminance:
						result = PixelComponents< PF >::L8U( pixel.constPtr() );
						break;

					default:
						result = 0;
						break;
					}

					return result;
				}

				static void setByte( Pixel< PF > & pixel
					, PixelComponent component
					, uint8_t value )
				{
					switch ( component )
					{
					case PixelComponent::eRed:
						PixelComponents< PF >::R8U( pixel.ptr(), value );
						break;

					case PixelComponent::eGreen:
						PixelComponents< PF >::G8U( pixel.ptr(), value );
						break;

					case PixelComponent::eBlue:
						PixelComponents< PF >::B8U( pixel.ptr(), value );
						break;

					case PixelComponent::eAlpha:
						PixelComponents< PF >::A8U( pixel.ptr(), value );
						break;

					case PixelComponent::eLuminance:
						PixelComponents< PF >::L8U( pixel.ptr(), value );
						break;

					default:
						break;
					}
				}
			};

			template< PixelFormat PF >
			struct PixelComponentTraits< PF
				, typename std::enable_if< IsDepthStencilFormat< PF >::value >::type >
			{
				static float getFloat( Pixel< PF > const & pixel
					, PixelComponent component )
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

				static void setFloat( Pixel< PF > & pixel
					, PixelComponent component, float value )
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

				static uint8_t getByte( Pixel< PF > const & pixel
					, PixelComponent component )
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

				static void setByte( Pixel< PF > & pixel
					, PixelComponent component
					, uint8_t value )
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

				static uint16_t getUInt16( Pixel< PF > const & pixel
					, PixelComponent component )
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

				static void setUInt16( Pixel< PF > & pixel
					, PixelComponent component
					, uint16_t value )
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

				static uint32_t getUInt24( Pixel< PF > const & pixel
					, PixelComponent component )
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

				static void setUInt24( Pixel< PF > & pixel
					, PixelComponent component
					, uint32_t value )
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

				static uint32_t getUInt32( Pixel< PF > const & pixel
					, PixelComponent component )
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

				static void setUInt32( Pixel< PF > & pixel
					, PixelComponent component
					, uint32_t value )
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

		//*************************************************************************************************

		template< PixelFormat PF >
		float getFloatComponent( Pixel< PF > const & pixel
			, PixelComponent component )
		{
			return details::PixelComponentTraits< PF >::getFloat( pixel
				, component );
		}

		template< PixelFormat PF >
		void setFloatComponent( Pixel< PF > & pixel
			, PixelComponent component
			, float value )
		{
			return details::PixelComponentTraits< PF >::setFloat( pixel
				, component
				, value );
		}

		template< PixelFormat PF >
		uint8_t getByteComponent( Pixel< PF > const & pixel
			, PixelComponent component )
		{
			return details::PixelComponentTraits< PF >::getByte( pixel
				, component );
		}

		template< PixelFormat PF >
		void setByteComponent( Pixel< PF > & pixel
			, PixelComponent component
			, uint8_t value )
		{
			return details::PixelComponentTraits< PF >::setByte( pixel
				, component
				, value );
		}

		template< PixelFormat PF >
		uint16_t getUInt16Component( Pixel< PF > const & pixel
			, PixelComponent component )
		{
			return details::PixelComponentTraits< PF >::getUInt16( pixel
				, component );
		}

		template< PixelFormat PF >
		void setUInt16Component( Pixel< PF > & pixel
			, PixelComponent component
			, uint16_t value )
		{
			return details::PixelComponentTraits< PF >::setUInt16( pixel
				, component
				, value );
		}

		template< PixelFormat PF >
		uint32_t getUInt24Component( Pixel< PF > const & pixel
			, PixelComponent component )
		{
			return details::PixelComponentTraits< PF >::getUInt24( pixel
				, component );
		}

		template< PixelFormat PF >
		void setUInt24Component( Pixel< PF > & pixel
			, PixelComponent component
			, uint32_t value )
		{
			return details::PixelComponentTraits< PF >::setUInt24( pixel
				, component
				, value );
		}

		template< PixelFormat PF >
		uint32_t getUInt32Component( Pixel< PF > const & pixel
			, PixelComponent component )
		{
			return details::PixelComponentTraits< PF >::getUInt32( pixel
				, component );
		}

		template< PixelFormat PF >
		void setUInt32Component( Pixel< PF > & pixel
			, PixelComponent component
			, uint32_t value )
		{
			return details::PixelComponentTraits< PF >::setUInt32( pixel
				, component
				, value );
		}
	}

	//*************************************************************************************************

	namespace details
	{
		static void halfToFloat( float & target
			, uint16_t const * source )
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

		static void floatToHalf( uint16_t * target
			, float source )
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
	}

	//*************************************************************************************************

	//!\~english	Specialisation for PixelFormat::eR8_UNORM
	//!\~french		Spécialisation pour PixelFormat::eR8_UNORM
	template<>
	struct PixelComponents< PixelFormat::eR8_UNORM >
	{
		static uint8_t L8U( uint8_t const * srcBuffer )
		{
			return srcBuffer[0];
		}
		static uint8_t R8U( uint8_t const * srcBuffer )
		{
			return srcBuffer[0];
		}
		static uint8_t G8U( uint8_t const * srcBuffer )
		{
			return srcBuffer[0];
		}
		static uint8_t B8U( uint8_t const * srcBuffer )
		{
			return srcBuffer[0];
		}
		static uint8_t A8U( uint8_t const * )
		{
			return 0xFF;
		}
		static int8_t L8S( uint8_t const * srcBuffer )
		{
			return int8_t( L8U( srcBuffer ) );
		}
		static int8_t R8S( uint8_t const * srcBuffer )
		{
			return int8_t( R8U( srcBuffer ) );
		}
		static int8_t G8S( uint8_t const * srcBuffer )
		{
			return int8_t( G8U( srcBuffer ) );
		}
		static int8_t B8S( uint8_t const * srcBuffer )
		{
			return int8_t( B8U( srcBuffer ) );
		}
		static int8_t A8S( uint8_t const * srcBuffer )
		{
			return int8_t( A8U( srcBuffer ) );
		}
		static float L32F( uint8_t const * srcBuffer )
		{
			return L8U( srcBuffer ) / 255.0f;
		}
		static float R32F( uint8_t const * srcBuffer )
		{
			return R8U( srcBuffer ) / 255.0f;
		}
		static float G32F( uint8_t const * srcBuffer )
		{
			return G8U( srcBuffer ) / 255.0f;
		}
		static float B32F( uint8_t const * srcBuffer )
		{
			return B8U( srcBuffer ) / 255.0f;
		}
		static float A32F( uint8_t const * srcBuffer )
		{
			return A8U( srcBuffer ) / 255.0f;
		}
		static void L8U( uint8_t * srcBuffer, uint8_t value )
		{
			srcBuffer[0] = value;
		}
		static void R8U( uint8_t * srcBuffer, uint8_t value )
		{
			srcBuffer[0] = value;
		}
		static void G8U( uint8_t * srcBuffer, uint8_t value )
		{
			srcBuffer[0] = value;
		}
		static void B8U( uint8_t * srcBuffer, uint8_t value )
		{
			srcBuffer[0] = value;
		}
		static void A8U( uint8_t *, uint8_t )
		{
		}
		static void L8S( uint8_t * srcBuffer, int8_t value )
		{
			L8U( srcBuffer, uint8_t( value ) );
		}
		static void R8S( uint8_t * srcBuffer, int8_t value )
		{
			R8U( srcBuffer, uint8_t( value ) );
		}
		static void G8S( uint8_t * srcBuffer, int8_t value )
		{
			G8U( srcBuffer, uint8_t( value ) );
		}
		static void B8S( uint8_t * srcBuffer, int8_t value )
		{
			B8U( srcBuffer, uint8_t( value ) );
		}
		static void A8S( uint8_t * srcBuffer, uint8_t value )
		{
			A8U( srcBuffer, uint8_t( value ) );
		}
		static void L32F( uint8_t * srcBuffer, float value )
		{
			L8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void R32F( uint8_t * srcBuffer, float value )
		{
			R8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void G32F( uint8_t * srcBuffer, float value )
		{
			G8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void B32F( uint8_t * srcBuffer, float value )
		{
			B8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void A32F( uint8_t *, float )
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

		static uint8_t L8U( uint8_t const * srcBuffer )
		{
			return uint8_t( L8S( srcBuffer ) );
		}
		static uint8_t R8U( uint8_t const * srcBuffer )
		{
			return uint8_t( R8S( srcBuffer ) );
		}
		static uint8_t G8U( uint8_t const * srcBuffer )
		{
			return uint8_t( G8S( srcBuffer ) );
		}
		static uint8_t B8U( uint8_t const * srcBuffer )
		{
			return uint8_t( B8S( srcBuffer ) );
		}
		static uint8_t A8U( uint8_t const * srcBuffer )
		{
			return uint8_t( A8S( srcBuffer ) );
		}
		static int8_t L8S( uint8_t const * srcBuffer )
		{
			return int8_t( L32F( srcBuffer ) * 255 );
		}
		static int8_t R8S( uint8_t const * srcBuffer )
		{
			return int8_t( R32F( srcBuffer ) * 255 );
		}
		static int8_t G8S( uint8_t const * srcBuffer )
		{
			return int8_t( G32F( srcBuffer ) * 255 );
		}
		static int8_t B8S( uint8_t const * srcBuffer )
		{
			return int8_t( B32F( srcBuffer ) * 255 );
		}
		static int8_t A8S( uint8_t const * srcBuffer )
		{
			return int8_t( A32F( srcBuffer ) * 255 );
		}
		static float L32F( uint8_t const * srcBuffer )
		{
			return src;
		}
		static float R32F( uint8_t const * srcBuffer )
		{
			return src;
		}
		static float G32F( uint8_t const * srcBuffer )
		{
			return src;
		}
		static float B32F( uint8_t const * srcBuffer )
		{
			return src;
		}
		static float A32F( uint8_t const * )
		{
			return 1.0f;
		}
		static void L8U( uint8_t * srcBuffer, uint8_t value )
		{
			L8S( srcBuffer, int8_t( value ) );
		}
		static void R8U( uint8_t * srcBuffer, uint8_t value )
		{
			R8S( srcBuffer, int8_t( value ) );
		}
		static void G8U( uint8_t * srcBuffer, uint8_t value )
		{
			G8S( srcBuffer, int8_t( value ) );
		}
		static void B8U( uint8_t * srcBuffer, uint8_t value )
		{
			B8S( srcBuffer, int8_t( value ) );
		}
		static void A8U( uint8_t * srcBuffer, uint8_t value )
		{
			A8S( srcBuffer, int8_t( value ) );
		}
		static void L8S( uint8_t * srcBuffer, int8_t value )
		{
			L32F( srcBuffer, float( value ) / 255 );
		}
		static void R8S( uint8_t * srcBuffer, int8_t value )
		{
			R32F( srcBuffer, float( value ) / 255 );
		}
		static void G8S( uint8_t * srcBuffer, int8_t value )
		{
			G32F( srcBuffer, float( value ) / 255 );
		}
		static void B8S( uint8_t * srcBuffer, int8_t value )
		{
			B32F( srcBuffer, float( value ) / 255 );
		}
		static void A8S( uint8_t * srcBuffer, int8_t value )
		{
			A32F( srcBuffer, float( value ) / 255 );
		}
		static void L32F( uint8_t * srcBuffer, float value )
		{
			dst = value;
		}
		static void R32F( uint8_t * srcBuffer, float value )
		{
			dst = value;
		}
		static void G32F( uint8_t * srcBuffer, float value )
		{
			dst = value;
		}
		static void B32F( uint8_t * srcBuffer, float value )
		{
			dst = value;
		}
		static void A32F( uint8_t *, float )
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
		static uint8_t L8U( uint8_t const * srcBuffer )
		{
			return srcBuffer[0];
		}
		static uint8_t R8U( uint8_t const * srcBuffer )
		{
			return srcBuffer[0];
		}
		static uint8_t G8U( uint8_t const * srcBuffer )
		{
			return srcBuffer[0];
		}
		static uint8_t B8U( uint8_t const * srcBuffer )
		{
			return srcBuffer[0];
		}
		static uint8_t A8U( uint8_t const * srcBuffer )
		{
			return srcBuffer[1];
		}
		static int8_t L8S( uint8_t const * srcBuffer )
		{
			return int8_t( L8U( srcBuffer ) );
		}
		static int8_t R8S( uint8_t const * srcBuffer )
		{
			return int8_t( R8U( srcBuffer ) );
		}
		static int8_t G8S( uint8_t const * srcBuffer )
		{
			return int8_t( G8U( srcBuffer ) );
		}
		static int8_t B8S( uint8_t const * srcBuffer )
		{
			return int8_t( B8U( srcBuffer ) );
		}
		static int8_t A8S( uint8_t const * srcBuffer )
		{
			return int8_t( A8U( srcBuffer ) );
		}
		static float L32F( uint8_t const * srcBuffer )
		{
			return L8U( srcBuffer ) / 255.0f;
		}
		static float R32F( uint8_t const * srcBuffer )
		{
			return R8U( srcBuffer ) / 255.0f;
		}
		static float G32F( uint8_t const * srcBuffer )
		{
			return G8U( srcBuffer ) / 255.0f;
		}
		static float B32F( uint8_t const * srcBuffer )
		{
			return B8U( srcBuffer ) / 255.0f;
		}
		static float A32F( uint8_t const * srcBuffer )
		{
			return A8U( srcBuffer ) / 255.0f;
		}
		static void L8U( uint8_t * srcBuffer, uint8_t value )
		{
			srcBuffer[0] = value;
		}
		static void R8U( uint8_t * srcBuffer, uint8_t value )
		{
			srcBuffer[0] = value;
		}
		static void G8U( uint8_t * srcBuffer, uint8_t value )
		{
			srcBuffer[0] = value;
		}
		static void B8U( uint8_t * srcBuffer, uint8_t value )
		{
			srcBuffer[0] = value;
		}
		static void A8U( uint8_t * srcBuffer, uint8_t value )
		{
			srcBuffer[1] = value;
		}
		static void L8S( uint8_t * srcBuffer, int8_t value )
		{
			L8U( srcBuffer, uint8_t( value ) );
		}
		static void R8S( uint8_t * srcBuffer, int8_t value )
		{
			R8U( srcBuffer, uint8_t( value ) );
		}
		static void G8S( uint8_t * srcBuffer, int8_t value )
		{
			G8U( srcBuffer, uint8_t( value ) );
		}
		static void B8S( uint8_t * srcBuffer, int8_t value )
		{
			B8U( srcBuffer, uint8_t( value ) );
		}
		static void A8S( uint8_t * srcBuffer, uint8_t value )
		{
			A8U( srcBuffer, uint8_t( value ) );
		}
		static void L32F( uint8_t * srcBuffer, float value )
		{
			L8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void R32F( uint8_t * srcBuffer, float value )
		{
			R8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void G32F( uint8_t * srcBuffer, float value )
		{
			G8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void B32F( uint8_t * srcBuffer, float value )
		{
			B8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void A32F( uint8_t * srcBuffer, float value )
		{
			A8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
	};
	//!\~english	Specialisation for PixelFormat::eR32A32_SFLOAT
	//!\~french		Spécialisation pour PixelFormat::eR32A32_SFLOAT
	template<>
	struct PixelComponents< PixelFormat::eR32A32_SFLOAT >
	{
#define src reinterpret_cast< float const * >( srcBuffer )
#define dst reinterpret_cast< float * >( srcBuffer )

		static uint8_t L8U( uint8_t const * srcBuffer )
		{
			return uint8_t( L8S( srcBuffer ) );
		}
		static uint8_t R8U( uint8_t const * srcBuffer )
		{
			return uint8_t( R8S( srcBuffer ) );
		}
		static uint8_t G8U( uint8_t const * srcBuffer )
		{
			return uint8_t( G8S( srcBuffer ) );
		}
		static uint8_t B8U( uint8_t const * srcBuffer )
		{
			return uint8_t( B8S( srcBuffer ) );
		}
		static uint8_t A8U( uint8_t const * srcBuffer )
		{
			return uint8_t( A8S( srcBuffer ) );
		}
		static int8_t L8S( uint8_t const * srcBuffer )
		{
			return int8_t( L32F( srcBuffer ) * 255 );
		}
		static int8_t R8S( uint8_t const * srcBuffer )
		{
			return int8_t( R32F( srcBuffer ) * 255 );
		}
		static int8_t G8S( uint8_t const * srcBuffer )
		{
			return int8_t( G32F( srcBuffer ) * 255 );
		}
		static int8_t B8S( uint8_t const * srcBuffer )
		{
			return int8_t( B32F( srcBuffer ) * 255 );
		}
		static int8_t A8S( uint8_t const * srcBuffer )
		{
			return int8_t( A32F( srcBuffer ) * 255 );
		}
		static float L32F( uint8_t const * srcBuffer )
		{
			return src[0];
		}
		static float R32F( uint8_t const * srcBuffer )
		{
			return src[0];
		}
		static float G32F( uint8_t const * srcBuffer )
		{
			return src[0];
		}
		static float B32F( uint8_t const * srcBuffer )
		{
			return src[0];
		}
		static float A32F( uint8_t const * srcBuffer )
		{
			return src[1];
		}
		static void L8U( uint8_t * srcBuffer, uint8_t value )
		{
			L8S( srcBuffer, int8_t( value ) );
		}
		static void R8U( uint8_t * srcBuffer, uint8_t value )
		{
			R8S( srcBuffer, int8_t( value ) );
		}
		static void G8U( uint8_t * srcBuffer, uint8_t value )
		{
			G8S( srcBuffer, int8_t( value ) );
		}
		static void B8U( uint8_t * srcBuffer, uint8_t value )
		{
			B8S( srcBuffer, int8_t( value ) );
		}
		static void A8U( uint8_t * srcBuffer, uint8_t value )
		{
			A8S( srcBuffer, int8_t( value ) );
		}
		static void L8S( uint8_t * srcBuffer, int8_t value )
		{
			L32F( srcBuffer, float( value ) / 255 );
		}
		static void R8S( uint8_t * srcBuffer, int8_t value )
		{
			R32F( srcBuffer, float( value ) / 255 );
		}
		static void G8S( uint8_t * srcBuffer, int8_t value )
		{
			G32F( srcBuffer, float( value ) / 255 );
		}
		static void B8S( uint8_t * srcBuffer, int8_t value )
		{
			B32F( srcBuffer, float( value ) / 255 );
		}
		static void A8S( uint8_t * srcBuffer, int8_t value )
		{
			A32F( srcBuffer, float( value ) / 255 );
		}
		static void L32F( uint8_t * srcBuffer, float value )
		{
			dst[0] = value;
		}
		static void R32F( uint8_t * srcBuffer, float value )
		{
			dst[0] = value;
		}
		static void G32F( uint8_t * srcBuffer, float value )
		{
			dst[0] = value;
		}
		static void B32F( uint8_t * srcBuffer, float value )
		{
			dst[0] = value;
		}
		static void A32F( uint8_t * srcBuffer, float value )
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

		static uint8_t L8U( uint8_t const * srcBuffer )
		{
			return uint8_t( R8U( srcBuffer ) * 0.30 + G8U( srcBuffer ) * 0.59 + B8U( srcBuffer ) * 0.11 );
		}
		static uint8_t A8U( uint8_t const * srcBuffer )
		{
			return uint8_t( ( src & 0x8000 ) ? 0xFF : 0x00 );
		}
		static uint8_t R8U( uint8_t const * srcBuffer )
		{
			return uint8_t( ( src & 0x7C00 ) >> 7 );
		}
		static uint8_t G8U( uint8_t const * srcBuffer )
		{
			return uint8_t( ( src & 0x03E0 ) >> 2 );
		}
		static uint8_t B8U( uint8_t const * srcBuffer )
		{
			return uint8_t( ( src & 0x001F ) << 3 );
		}
		static int8_t L8S( uint8_t const * srcBuffer )
		{
			return int8_t( L8U( srcBuffer ) );
		}
		static int8_t R8S( uint8_t const * srcBuffer )
		{
			return int8_t( R8U( srcBuffer ) );
		}
		static int8_t G8S( uint8_t const * srcBuffer )
		{
			return int8_t( G8U( srcBuffer ) );
		}
		static int8_t B8S( uint8_t const * srcBuffer )
		{
			return int8_t( B8U( srcBuffer ) );
		}
		static int8_t A8S( uint8_t const * srcBuffer )
		{
			return int8_t( A8U( srcBuffer ) );
		}
		static float L32F( uint8_t const * srcBuffer )
		{
			return L8U( srcBuffer ) / 255.0f;
		}
		static float R32F( uint8_t const * srcBuffer )
		{
			return R8U( srcBuffer ) / 255.0f;
		}
		static float G32F( uint8_t const * srcBuffer )
		{
			return G8U( srcBuffer ) / 255.0f;
		}
		static float B32F( uint8_t const * srcBuffer )
		{
			return B8U( srcBuffer ) / 255.0f;
		}
		static float A32F( uint8_t const * srcBuffer )
		{
			return A8U( srcBuffer ) / 255.0f;
		}
		static void L8U( uint8_t * srcBuffer, uint8_t value )
		{
			R8U( srcBuffer, value );
			G8U( srcBuffer, value );
			B8U( srcBuffer, value );
		}
		static void A8U( uint8_t * srcBuffer, uint8_t value )
		{
			dst = ( src & 0x7FFF ) | ( value ? 0x8000 : 0x0000 );
		}
		static void R8U( uint8_t * srcBuffer, uint8_t value )
		{
			dst = ( src & 0x83FF ) | ( ( uint16_t( value ) << 7 ) & 0x7C00 );
		}
		static void G8U( uint8_t * srcBuffer, uint8_t value )
		{
			dst = ( src & 0xFC1F ) | ( ( uint16_t( value ) << 2 ) & 0x03E0 );
		}
		static void B8U( uint8_t * srcBuffer, uint8_t value )
		{
			dst = ( src & 0xFFE0 ) | ( ( uint16_t( value ) >> 3 ) & 0x001F );
		}
		static void L8S( uint8_t * srcBuffer, int8_t value )
		{
			L8U( srcBuffer, uint8_t( value ) );
		}
		static void R8S( uint8_t * srcBuffer, int8_t value )
		{
			R8U( srcBuffer, uint8_t( value ) );
		}
		static void G8S( uint8_t * srcBuffer, int8_t value )
		{
			G8U( srcBuffer, uint8_t( value ) );
		}
		static void B8S( uint8_t * srcBuffer, int8_t value )
		{
			B8U( srcBuffer, uint8_t( value ) );
		}
		static void A8S( uint8_t * srcBuffer, uint8_t value )
		{
			A8U( srcBuffer, uint8_t( value ) );
		}
		static void L32F( uint8_t * srcBuffer, float value )
		{
			L8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void R32F( uint8_t * srcBuffer, float value )
		{
			R8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void G32F( uint8_t * srcBuffer, float value )
		{
			G8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void B32F( uint8_t * srcBuffer, float value )
		{
			B8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void A32F( uint8_t * srcBuffer, float value )
		{
			A8U( srcBuffer, uint8_t( value * 255.0 ) );
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

		static uint8_t L8U( uint8_t const * srcBuffer )
		{
			return uint8_t( R8U( srcBuffer ) * 0.30 + G8U( srcBuffer ) * 0.59 + B8U( srcBuffer ) * 0.11 );
		}
		static uint8_t R8U( uint8_t const * srcBuffer )
		{
			return uint8_t( ( src & 0xF800 ) >> 8 );
		}
		static uint8_t G8U( uint8_t const * srcBuffer )
		{
			return uint8_t( ( src & 0x07E0 ) >> 3 );
		}
		static uint8_t B8U( uint8_t const * srcBuffer )
		{
			return uint8_t( ( src & 0x001F ) << 3 );
		}
		static uint8_t A8U( uint8_t const * )
		{
			return 0xFF;
		}
		static int8_t L8S( uint8_t const * srcBuffer )
		{
			return int8_t( L8U( srcBuffer ) );
		}
		static int8_t R8S( uint8_t const * srcBuffer )
		{
			return int8_t( R8U( srcBuffer ) );
		}
		static int8_t G8S( uint8_t const * srcBuffer )
		{
			return int8_t( G8U( srcBuffer ) );
		}
		static int8_t B8S( uint8_t const * srcBuffer )
		{
			return int8_t( B8U( srcBuffer ) );
		}
		static int8_t A8S( uint8_t const * srcBuffer )
		{
			return int8_t( A8U( srcBuffer ) );
		}
		static float L32F( uint8_t const * srcBuffer )
		{
			return L8U( srcBuffer ) / 255.0f;
		}
		static float R32F( uint8_t const * srcBuffer )
		{
			return R8U( srcBuffer ) / 255.0f;
		}
		static float G32F( uint8_t const * srcBuffer )
		{
			return G8U( srcBuffer ) / 255.0f;
		}
		static float B32F( uint8_t const * srcBuffer )
		{
			return B8U( srcBuffer ) / 255.0f;
		}
		static float A32F( uint8_t const * srcBuffer )
		{
			return A8U( srcBuffer ) / 255.0f;
		}
		static void L8U( uint8_t * srcBuffer, uint8_t value )
		{
			R8U( srcBuffer, value );
			G8U( srcBuffer, value );
			B8U( srcBuffer, value );
		}
		static void R8U( uint8_t * srcBuffer, uint8_t value )
		{
			dst = ( src & 0x07FF ) | ( ( uint16_t( value ) << 8 ) & 0xF800 );
		}
		static void G8U( uint8_t * srcBuffer, uint8_t value )
		{
			dst = ( src & 0xF81F ) | ( ( uint16_t( value ) << 3 ) & 0x07E0 );
		}
		static void B8U( uint8_t * srcBuffer, uint8_t value )
		{
			dst = ( src & 0xFFE0 ) | ( ( uint16_t( value ) >> 3 ) & 0x001F );
		}
		static void A8U( uint8_t *, uint8_t )
		{
		}
		static void L8S( uint8_t * srcBuffer, int8_t value )
		{
			L8U( srcBuffer, uint8_t( value ) );
		}
		static void R8S( uint8_t * srcBuffer, int8_t value )
		{
			R8U( srcBuffer, uint8_t( value ) );
		}
		static void G8S( uint8_t * srcBuffer, int8_t value )
		{
			G8U( srcBuffer, uint8_t( value ) );
		}
		static void B8S( uint8_t * srcBuffer, int8_t value )
		{
			B8U( srcBuffer, uint8_t( value ) );
		}
		static void A8S( uint8_t * srcBuffer, uint8_t value )
		{
			A8U( srcBuffer, uint8_t( value ) );
		}
		static void L32F( uint8_t * srcBuffer, float value )
		{
			L8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void R32F( uint8_t * srcBuffer, float value )
		{
			R8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void G32F( uint8_t * srcBuffer, float value )
		{
			G8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void B32F( uint8_t * srcBuffer, float value )
		{
			B8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void A32F( uint8_t *, float )
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

		static uint8_t L8U( uint8_t const * srcBuffer )
		{
			return uint8_t( R8U( srcBuffer ) * 0.30 + G8U( srcBuffer ) * 0.59 + B8U( srcBuffer ) * 0.11 );
		}
		static uint8_t R8U( uint8_t const * srcBuffer )
		{
			return src.r;
		}
		static uint8_t G8U( uint8_t const * srcBuffer )
		{
			return src.g;
		}
		static uint8_t B8U( uint8_t const * srcBuffer )
		{
			return src.b;
		}
		static uint8_t A8U( uint8_t const * )
		{
			return 0xFF;
		}
		static int8_t L8S( uint8_t const * srcBuffer )
		{
			return int8_t( L8U( srcBuffer ) );
		}
		static int8_t R8S( uint8_t const * srcBuffer )
		{
			return int8_t( R8U( srcBuffer ) );
		}
		static int8_t G8S( uint8_t const * srcBuffer )
		{
			return int8_t( G8U( srcBuffer ) );
		}
		static int8_t B8S( uint8_t const * srcBuffer )
		{
			return int8_t( B8U( srcBuffer ) );
		}
		static int8_t A8S( uint8_t const * srcBuffer )
		{
			return int8_t( A8U( srcBuffer ) );
		}
		static float L32F( uint8_t const * srcBuffer )
		{
			return L8U( srcBuffer ) / 255.0f;
		}
		static float R32F( uint8_t const * srcBuffer )
		{
			return R8U( srcBuffer ) / 255.0f;
		}
		static float G32F( uint8_t const * srcBuffer )
		{
			return G8U( srcBuffer ) / 255.0f;
		}
		static float B32F( uint8_t const * srcBuffer )
		{
			return B8U( srcBuffer ) / 255.0f;
		}
		static float A32F( uint8_t const * srcBuffer )
		{
			return A8U( srcBuffer ) / 255.0f;
		}
		static void L8U( uint8_t * srcBuffer, uint8_t value )
		{
			R8U( srcBuffer, value );
			G8U( srcBuffer, value );
			B8U( srcBuffer, value );
		}
		static void R8U( uint8_t * srcBuffer, uint8_t value )
		{
			dst.r = value;
		}
		static void G8U( uint8_t * srcBuffer, uint8_t value )
		{
			dst.g = value;
		}
		static void B8U( uint8_t * srcBuffer, uint8_t value )
		{
			dst.b = value;
		}
		static void A8U( uint8_t *, uint8_t )
		{
		}
		static void L8S( uint8_t * srcBuffer, int8_t value )
		{
			L8U( srcBuffer, uint8_t( value ) );
		}
		static void R8S( uint8_t * srcBuffer, int8_t value )
		{
			R8U( srcBuffer, uint8_t( value ) );
		}
		static void G8S( uint8_t * srcBuffer, int8_t value )
		{
			G8U( srcBuffer, uint8_t( value ) );
		}
		static void B8S( uint8_t * srcBuffer, int8_t value )
		{
			B8U( srcBuffer, uint8_t( value ) );
		}
		static void A8S( uint8_t * srcBuffer, uint8_t value )
		{
			A8U( srcBuffer, uint8_t( value ) );
		}
		static void L32F( uint8_t * srcBuffer, float value )
		{
			L8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void R32F( uint8_t * srcBuffer, float value )
		{
			R8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void G32F( uint8_t * srcBuffer, float value )
		{
			G8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void B32F( uint8_t * srcBuffer, float value )
		{
			B8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void A32F( uint8_t *, float )
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

		static uint8_t L8U( uint8_t const * srcBuffer )
		{
			return uint8_t( R8U( srcBuffer ) * 0.30 + G8U( srcBuffer ) * 0.59 + B8U( srcBuffer ) * 0.11 );
		}
		static uint8_t R8U( uint8_t const * srcBuffer )
		{
			return src.r;
		}
		static uint8_t G8U( uint8_t const * srcBuffer )
		{
			return src.g;
		}
		static uint8_t B8U( uint8_t const * srcBuffer )
		{
			return src.b;
		}
		static uint8_t A8U( uint8_t const * )
		{
			return 0xFF;
		}
		static int8_t L8S( uint8_t const * srcBuffer )
		{
			return int8_t( L8U( srcBuffer ) );
		}
		static int8_t R8S( uint8_t const * srcBuffer )
		{
			return int8_t( R8U( srcBuffer ) );
		}
		static int8_t G8S( uint8_t const * srcBuffer )
		{
			return int8_t( G8U( srcBuffer ) );
		}
		static int8_t B8S( uint8_t const * srcBuffer )
		{
			return int8_t( B8U( srcBuffer ) );
		}
		static int8_t A8S( uint8_t const * srcBuffer )
		{
			return int8_t( A8U( srcBuffer ) );
		}
		static float L32F( uint8_t const * srcBuffer )
		{
			return L8U( srcBuffer ) / 255.0f;
		}
		static float R32F( uint8_t const * srcBuffer )
		{
			return R8U( srcBuffer ) / 255.0f;
		}
		static float G32F( uint8_t const * srcBuffer )
		{
			return G8U( srcBuffer ) / 255.0f;
		}
		static float B32F( uint8_t const * srcBuffer )
		{
			return B8U( srcBuffer ) / 255.0f;
		}
		static float A32F( uint8_t const * srcBuffer )
		{
			return A8U( srcBuffer ) / 255.0f;
		}
		static void L8U( uint8_t * srcBuffer, uint8_t value )
		{
			R8U( srcBuffer, value );
			G8U( srcBuffer, value );
			B8U( srcBuffer, value );
		}
		static void R8U( uint8_t * srcBuffer, uint8_t value )
		{
			dst.r = value;
		}
		static void G8U( uint8_t * srcBuffer, uint8_t value )
		{
			dst.g = value;
		}
		static void B8U( uint8_t * srcBuffer, uint8_t value )
		{
			dst.b = value;
		}
		static void A8U( uint8_t *, uint8_t )
		{
		}
		static void L8S( uint8_t * srcBuffer, int8_t value )
		{
			L8U( srcBuffer, uint8_t( value ) );
		}
		static void R8S( uint8_t * srcBuffer, int8_t value )
		{
			R8U( srcBuffer, uint8_t( value ) );
		}
		static void G8S( uint8_t * srcBuffer, int8_t value )
		{
			G8U( srcBuffer, uint8_t( value ) );
		}
		static void B8S( uint8_t * srcBuffer, int8_t value )
		{
			B8U( srcBuffer, uint8_t( value ) );
		}
		static void A8S( uint8_t * srcBuffer, uint8_t value )
		{
			A8U( srcBuffer, uint8_t( value ) );
		}
		static void L32F( uint8_t * srcBuffer, float value )
		{
			L8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void R32F( uint8_t * srcBuffer, float value )
		{
			R8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void G32F( uint8_t * srcBuffer, float value )
		{
			G8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void B32F( uint8_t * srcBuffer, float value )
		{
			B8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void A32F( uint8_t *, float )
		{
		}

#undef src
#undef dst
	};
	//!\~english	Specialisation for PixelFormat::eB8G8R8A8_UNORM
	//!\~french		Spécialisation pour PixelFormat::eB8G8R8A8_UNORM
	template<>
	struct PixelComponents< PixelFormat::eB8G8R8A8_UNORM >
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

		static uint8_t L8U( uint8_t const * srcBuffer )
		{
			return uint8_t( R8U( srcBuffer ) * 0.30 + G8U( srcBuffer ) * 0.59 + B8U( srcBuffer ) * 0.11 );
		}
		static uint8_t A8U( uint8_t const * srcBuffer )
		{
			return src.a;
		}
		static uint8_t R8U( uint8_t const * srcBuffer )
		{
			return src.r;
		}
		static uint8_t G8U( uint8_t const * srcBuffer )
		{
			return src.g;
		}
		static uint8_t B8U( uint8_t const * srcBuffer )
		{
			return src.b;
		}
		static int8_t L8S( uint8_t const * srcBuffer )
		{
			return int8_t( L8U( srcBuffer ) );
		}
		static int8_t R8S( uint8_t const * srcBuffer )
		{
			return int8_t( R8U( srcBuffer ) );
		}
		static int8_t G8S( uint8_t const * srcBuffer )
		{
			return int8_t( G8U( srcBuffer ) );
		}
		static int8_t B8S( uint8_t const * srcBuffer )
		{
			return int8_t( B8U( srcBuffer ) );
		}
		static int8_t A8S( uint8_t const * srcBuffer )
		{
			return int8_t( A8U( srcBuffer ) );
		}
		static float L32F( uint8_t const * srcBuffer )
		{
			return L8U( srcBuffer ) / 255.0f;
		}
		static float R32F( uint8_t const * srcBuffer )
		{
			return R8U( srcBuffer ) / 255.0f;
		}
		static float G32F( uint8_t const * srcBuffer )
		{
			return G8U( srcBuffer ) / 255.0f;
		}
		static float B32F( uint8_t const * srcBuffer )
		{
			return B8U( srcBuffer ) / 255.0f;
		}
		static float A32F( uint8_t const * srcBuffer )
		{
			return A8U( srcBuffer ) / 255.0f;
		}
		static void L8U( uint8_t * srcBuffer, uint8_t value )
		{
			R8U( srcBuffer, value );
			G8U( srcBuffer, value );
			B8U( srcBuffer, value );
		}
		static void A8U( uint8_t * srcBuffer, uint8_t value )
		{
			dst.a = value;
		}
		static void R8U( uint8_t * srcBuffer, uint8_t value )
		{
			dst.r = value;
		}
		static void G8U( uint8_t * srcBuffer, uint8_t value )
		{
			dst.g = value;
		}
		static void B8U( uint8_t * srcBuffer, uint8_t value )
		{
			dst.b = value;
		}
		static void L8S( uint8_t * srcBuffer, int8_t value )
		{
			L8U( srcBuffer, uint8_t( value ) );
		}
		static void R8S( uint8_t * srcBuffer, int8_t value )
		{
			R8U( srcBuffer, uint8_t( value ) );
		}
		static void G8S( uint8_t * srcBuffer, int8_t value )
		{
			G8U( srcBuffer, uint8_t( value ) );
		}
		static void B8S( uint8_t * srcBuffer, int8_t value )
		{
			B8U( srcBuffer, uint8_t( value ) );
		}
		static void A8S( uint8_t * srcBuffer, uint8_t value )
		{
			A8U( srcBuffer, uint8_t( value ) );
		}
		static void L32F( uint8_t * srcBuffer, float value )
		{
			L8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void R32F( uint8_t * srcBuffer, float value )
		{
			R8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void G32F( uint8_t * srcBuffer, float value )
		{
			G8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void B32F( uint8_t * srcBuffer, float value )
		{
			B8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void A32F( uint8_t * srcBuffer, float value )
		{
			A8U( srcBuffer, uint8_t( value * 255.0 ) );
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

		static uint8_t L8U( uint8_t const * srcBuffer )
		{
			return uint8_t( R8U( srcBuffer ) * 0.30 + G8U( srcBuffer ) * 0.59 + B8U( srcBuffer ) * 0.11 );
		}
		static uint8_t R8U( uint8_t const * srcBuffer )
		{
			return src.r;
		}
		static uint8_t G8U( uint8_t const * srcBuffer )
		{
			return src.g;
		}
		static uint8_t B8U( uint8_t const * srcBuffer )
		{
			return src.b;
		}
		static uint8_t A8U( uint8_t const * )
		{
			return 0xFF;
		}
		static int8_t L8S( uint8_t const * srcBuffer )
		{
			return int8_t( L8U( srcBuffer ) );
		}
		static int8_t R8S( uint8_t const * srcBuffer )
		{
			return int8_t( R8U( srcBuffer ) );
		}
		static int8_t G8S( uint8_t const * srcBuffer )
		{
			return int8_t( G8U( srcBuffer ) );
		}
		static int8_t B8S( uint8_t const * srcBuffer )
		{
			return int8_t( B8U( srcBuffer ) );
		}
		static int8_t A8S( uint8_t const * srcBuffer )
		{
			return int8_t( A8U( srcBuffer ) );
		}
		static float L32F( uint8_t const * srcBuffer )
		{
			return L8U( srcBuffer ) / 255.0f;
		}
		static float R32F( uint8_t const * srcBuffer )
		{
			return R8U( srcBuffer ) / 255.0f;
		}
		static float G32F( uint8_t const * srcBuffer )
		{
			return G8U( srcBuffer ) / 255.0f;
		}
		static float B32F( uint8_t const * srcBuffer )
		{
			return B8U( srcBuffer ) / 255.0f;
		}
		static float A32F( uint8_t const * srcBuffer )
		{
			return A8U( srcBuffer ) / 255.0f;
		}
		static void L8U( uint8_t * srcBuffer, uint8_t value )
		{
			R8U( srcBuffer, value );
			G8U( srcBuffer, value );
			B8U( srcBuffer, value );
		}
		static void R8U( uint8_t * srcBuffer, uint8_t value )
		{
			dst.r = value;
		}
		static void G8U( uint8_t * srcBuffer, uint8_t value )
		{
			dst.g = value;
		}
		static void B8U( uint8_t * srcBuffer, uint8_t value )
		{
			dst.b = value;
		}
		static void A8U( uint8_t *, uint8_t )
		{
		}
		static void L8S( uint8_t * srcBuffer, int8_t value )
		{
			L8U( srcBuffer, uint8_t( value ) );
		}
		static void R8S( uint8_t * srcBuffer, int8_t value )
		{
			R8U( srcBuffer, uint8_t( value ) );
		}
		static void G8S( uint8_t * srcBuffer, int8_t value )
		{
			G8U( srcBuffer, uint8_t( value ) );
		}
		static void B8S( uint8_t * srcBuffer, int8_t value )
		{
			B8U( srcBuffer, uint8_t( value ) );
		}
		static void A8S( uint8_t * srcBuffer, uint8_t value )
		{
			A8U( srcBuffer, uint8_t( value ) );
		}
		static void L32F( uint8_t * srcBuffer, float value )
		{
			L8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void R32F( uint8_t * srcBuffer, float value )
		{
			R8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void G32F( uint8_t * srcBuffer, float value )
		{
			G8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void B32F( uint8_t * srcBuffer, float value )
		{
			B8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void A32F( uint8_t *, float )
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

		static uint8_t L8U( uint8_t const * srcBuffer )
		{
			return uint8_t( R8U( srcBuffer ) * 0.30 + G8U( srcBuffer ) * 0.59 + B8U( srcBuffer ) * 0.11 );
		}
		static uint8_t R8U( uint8_t const * srcBuffer )
		{
			return src.r;
		}
		static uint8_t G8U( uint8_t const * srcBuffer )
		{
			return src.g;
		}
		static uint8_t B8U( uint8_t const * srcBuffer )
		{
			return src.b;
		}
		static uint8_t A8U( uint8_t const * )
		{
			return 0xFF;
		}
		static int8_t L8S( uint8_t const * srcBuffer )
		{
			return int8_t( L8U( srcBuffer ) );
		}
		static int8_t R8S( uint8_t const * srcBuffer )
		{
			return int8_t( R8U( srcBuffer ) );
		}
		static int8_t G8S( uint8_t const * srcBuffer )
		{
			return int8_t( G8U( srcBuffer ) );
		}
		static int8_t B8S( uint8_t const * srcBuffer )
		{
			return int8_t( B8U( srcBuffer ) );
		}
		static int8_t A8S( uint8_t const * srcBuffer )
		{
			return int8_t( A8U( srcBuffer ) );
		}
		static float L32F( uint8_t const * srcBuffer )
		{
			return L8U( srcBuffer ) / 255.0f;
		}
		static float R32F( uint8_t const * srcBuffer )
		{
			return R8U( srcBuffer ) / 255.0f;
		}
		static float G32F( uint8_t const * srcBuffer )
		{
			return G8U( srcBuffer ) / 255.0f;
		}
		static float B32F( uint8_t const * srcBuffer )
		{
			return B8U( srcBuffer ) / 255.0f;
		}
		static float A32F( uint8_t const * srcBuffer )
		{
			return A8U( srcBuffer ) / 255.0f;
		}
		static void L8U( uint8_t * srcBuffer, uint8_t value )
		{
			R8U( srcBuffer, value );
			G8U( srcBuffer, value );
			B8U( srcBuffer, value );
		}
		static void R8U( uint8_t * srcBuffer, uint8_t value )
		{
			dst.r = value;
		}
		static void G8U( uint8_t * srcBuffer, uint8_t value )
		{
			dst.g = value;
		}
		static void B8U( uint8_t * srcBuffer, uint8_t value )
		{
			dst.b = value;
		}
		static void A8U( uint8_t *, uint8_t )
		{
		}
		static void L8S( uint8_t * srcBuffer, int8_t value )
		{
			L8U( srcBuffer, uint8_t( value ) );
		}
		static void R8S( uint8_t * srcBuffer, int8_t value )
		{
			R8U( srcBuffer, uint8_t( value ) );
		}
		static void G8S( uint8_t * srcBuffer, int8_t value )
		{
			G8U( srcBuffer, uint8_t( value ) );
		}
		static void B8S( uint8_t * srcBuffer, int8_t value )
		{
			B8U( srcBuffer, uint8_t( value ) );
		}
		static void A8S( uint8_t * srcBuffer, uint8_t value )
		{
			A8U( srcBuffer, uint8_t( value ) );
		}
		static void L32F( uint8_t * srcBuffer, float value )
		{
			L8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void R32F( uint8_t * srcBuffer, float value )
		{
			R8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void G32F( uint8_t * srcBuffer, float value )
		{
			G8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void B32F( uint8_t * srcBuffer, float value )
		{
			B8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void A32F( uint8_t *, float )
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

		static uint8_t L8U( uint8_t const * srcBuffer )
		{
			return uint8_t( R8U( srcBuffer ) * 0.30 + G8U( srcBuffer ) * 0.59 + B8U( srcBuffer ) * 0.11 );
		}
		static uint8_t A8U( uint8_t const * srcBuffer )
		{
			return src.a;
		}
		static uint8_t R8U( uint8_t const * srcBuffer )
		{
			return src.r;
		}
		static uint8_t G8U( uint8_t const * srcBuffer )
		{
			return src.g;
		}
		static uint8_t B8U( uint8_t const * srcBuffer )
		{
			return src.b;
		}
		static int8_t L8S( uint8_t const * srcBuffer )
		{
			return int8_t( L8U( srcBuffer ) );
		}
		static int8_t R8S( uint8_t const * srcBuffer )
		{
			return int8_t( R8U( srcBuffer ) );
		}
		static int8_t G8S( uint8_t const * srcBuffer )
		{
			return int8_t( G8U( srcBuffer ) );
		}
		static int8_t B8S( uint8_t const * srcBuffer )
		{
			return int8_t( B8U( srcBuffer ) );
		}
		static int8_t A8S( uint8_t const * srcBuffer )
		{
			return int8_t( A8U( srcBuffer ) );
		}
		static float L32F( uint8_t const * srcBuffer )
		{
			return L8U( srcBuffer ) / 255.0f;
		}
		static float R32F( uint8_t const * srcBuffer )
		{
			return R8U( srcBuffer ) / 255.0f;
		}
		static float G32F( uint8_t const * srcBuffer )
		{
			return G8U( srcBuffer ) / 255.0f;
		}
		static float B32F( uint8_t const * srcBuffer )
		{
			return B8U( srcBuffer ) / 255.0f;
		}
		static float A32F( uint8_t const * srcBuffer )
		{
			return A8U( srcBuffer ) / 255.0f;
		}
		static void L8U( uint8_t * srcBuffer, uint8_t value )
		{
			R8U( srcBuffer, value );
			G8U( srcBuffer, value );
			B8U( srcBuffer, value );
		}
		static void A8U( uint8_t * srcBuffer, uint8_t value )
		{
			dst.a = value;
		}
		static void R8U( uint8_t * srcBuffer, uint8_t value )
		{
			dst.r = value;
		}
		static void G8U( uint8_t * srcBuffer, uint8_t value )
		{
			dst.g = value;
		}
		static void B8U( uint8_t * srcBuffer, uint8_t value )
		{
			dst.b = value;
		}
		static void L8S( uint8_t * srcBuffer, int8_t value )
		{
			L8U( srcBuffer, uint8_t( value ) );
		}
		static void R8S( uint8_t * srcBuffer, int8_t value )
		{
			R8U( srcBuffer, uint8_t( value ) );
		}
		static void G8S( uint8_t * srcBuffer, int8_t value )
		{
			G8U( srcBuffer, uint8_t( value ) );
		}
		static void B8S( uint8_t * srcBuffer, int8_t value )
		{
			B8U( srcBuffer, uint8_t( value ) );
		}
		static void A8S( uint8_t * srcBuffer, uint8_t value )
		{
			A8U( srcBuffer, uint8_t( value ) );
		}
		static void L32F( uint8_t * srcBuffer, float value )
		{
			L8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void R32F( uint8_t * srcBuffer, float value )
		{
			R8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void G32F( uint8_t * srcBuffer, float value )
		{
			G8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void B32F( uint8_t * srcBuffer, float value )
		{
			B8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void A32F( uint8_t * srcBuffer, float value )
		{
			A8U( srcBuffer, uint8_t( value * 255.0 ) );
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

		static uint8_t L8U( uint8_t const * srcBuffer )
		{
			return uint8_t( R8U( srcBuffer ) * 0.30 + G8U( srcBuffer ) * 0.59 + B8U( srcBuffer ) * 0.11 );
		}
		static uint8_t A8U( uint8_t const * srcBuffer )
		{
			return src.a;
		}
		static uint8_t R8U( uint8_t const * srcBuffer )
		{
			return src.r;
		}
		static uint8_t G8U( uint8_t const * srcBuffer )
		{
			return src.g;
		}
		static uint8_t B8U( uint8_t const * srcBuffer )
		{
			return src.b;
		}
		static int8_t L8S( uint8_t const * srcBuffer )
		{
			return int8_t( L8U( srcBuffer ) );
		}
		static int8_t R8S( uint8_t const * srcBuffer )
		{
			return int8_t( R8U( srcBuffer ) );
		}
		static int8_t G8S( uint8_t const * srcBuffer )
		{
			return int8_t( G8U( srcBuffer ) );
		}
		static int8_t B8S( uint8_t const * srcBuffer )
		{
			return int8_t( B8U( srcBuffer ) );
		}
		static int8_t A8S( uint8_t const * srcBuffer )
		{
			return int8_t( A8U( srcBuffer ) );
		}
		static float L32F( uint8_t const * srcBuffer )
		{
			return L8U( srcBuffer ) / 255.0f;
		}
		static float R32F( uint8_t const * srcBuffer )
		{
			return R8U( srcBuffer ) / 255.0f;
		}
		static float G32F( uint8_t const * srcBuffer )
		{
			return G8U( srcBuffer ) / 255.0f;
		}
		static float B32F( uint8_t const * srcBuffer )
		{
			return B8U( srcBuffer ) / 255.0f;
		}
		static float A32F( uint8_t const * srcBuffer )
		{
			return A8U( srcBuffer ) / 255.0f;
		}
		static void L8U( uint8_t * srcBuffer, uint8_t value )
		{
			R8U( srcBuffer, value );
			G8U( srcBuffer, value );
			B8U( srcBuffer, value );
		}
		static void A8U( uint8_t * srcBuffer, uint8_t value )
		{
			dst.a = value;
		}
		static void R8U( uint8_t * srcBuffer, uint8_t value )
		{
			dst.r = value;
		}
		static void G8U( uint8_t * srcBuffer, uint8_t value )
		{
			dst.g = value;
		}
		static void B8U( uint8_t * srcBuffer, uint8_t value )
		{
			dst.b = value;
		}
		static void L8S( uint8_t * srcBuffer, int8_t value )
		{
			L8U( srcBuffer, uint8_t( value ) );
		}
		static void R8S( uint8_t * srcBuffer, int8_t value )
		{
			R8U( srcBuffer, uint8_t( value ) );
		}
		static void G8S( uint8_t * srcBuffer, int8_t value )
		{
			G8U( srcBuffer, uint8_t( value ) );
		}
		static void B8S( uint8_t * srcBuffer, int8_t value )
		{
			B8U( srcBuffer, uint8_t( value ) );
		}
		static void A8S( uint8_t * srcBuffer, uint8_t value )
		{
			A8U( srcBuffer, uint8_t( value ) );
		}
		static void L32F( uint8_t * srcBuffer, float value )
		{
			L8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void R32F( uint8_t * srcBuffer, float value )
		{
			R8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void G32F( uint8_t * srcBuffer, float value )
		{
			G8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void B32F( uint8_t * srcBuffer, float value )
		{
			B8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void A32F( uint8_t * srcBuffer, float value )
		{
			A8U( srcBuffer, uint8_t( value * 255.0 ) );
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

		static uint8_t L8U( uint8_t const * srcBuffer )
		{
			return uint8_t( R8U( srcBuffer ) * 0.30 + G8U( srcBuffer ) * 0.59 + B8U( srcBuffer ) * 0.11 );
		}
		static uint8_t A8U( uint8_t const * srcBuffer )
		{
			return src.a;
		}
		static uint8_t R8U( uint8_t const * srcBuffer )
		{
			return src.r;
		}
		static uint8_t G8U( uint8_t const * srcBuffer )
		{
			return src.g;
		}
		static uint8_t B8U( uint8_t const * srcBuffer )
		{
			return src.b;
		}
		static int8_t L8S( uint8_t const * srcBuffer )
		{
			return int8_t( L8U( srcBuffer ) );
		}
		static int8_t R8S( uint8_t const * srcBuffer )
		{
			return int8_t( R8U( srcBuffer ) );
		}
		static int8_t G8S( uint8_t const * srcBuffer )
		{
			return int8_t( G8U( srcBuffer ) );
		}
		static int8_t B8S( uint8_t const * srcBuffer )
		{
			return int8_t( B8U( srcBuffer ) );
		}
		static int8_t A8S( uint8_t const * srcBuffer )
		{
			return int8_t( A8U( srcBuffer ) );
		}
		static float L32F( uint8_t const * srcBuffer )
		{
			return L8U( srcBuffer ) / 255.0f;
		}
		static float R32F( uint8_t const * srcBuffer )
		{
			return R8U( srcBuffer ) / 255.0f;
		}
		static float G32F( uint8_t const * srcBuffer )
		{
			return G8U( srcBuffer ) / 255.0f;
		}
		static float B32F( uint8_t const * srcBuffer )
		{
			return B8U( srcBuffer ) / 255.0f;
		}
		static float A32F( uint8_t const * srcBuffer )
		{
			return A8U( srcBuffer ) / 255.0f;
		}
		static void L8U( uint8_t * srcBuffer, uint8_t value )
		{
			R8U( srcBuffer, value );
			G8U( srcBuffer, value );
			B8U( srcBuffer, value );
		}
		static void A8U( uint8_t * srcBuffer, uint8_t value )
		{
			dst.a = value;
		}
		static void R8U( uint8_t * srcBuffer, uint8_t value )
		{
			dst.r = value;
		}
		static void G8U( uint8_t * srcBuffer, uint8_t value )
		{
			dst.g = value;
		}
		static void B8U( uint8_t * srcBuffer, uint8_t value )
		{
			dst.b = value;
		}
		static void L8S( uint8_t * srcBuffer, int8_t value )
		{
			L8U( srcBuffer, uint8_t( value ) );
		}
		static void R8S( uint8_t * srcBuffer, int8_t value )
		{
			R8U( srcBuffer, uint8_t( value ) );
		}
		static void G8S( uint8_t * srcBuffer, int8_t value )
		{
			G8U( srcBuffer, uint8_t( value ) );
		}
		static void B8S( uint8_t * srcBuffer, int8_t value )
		{
			B8U( srcBuffer, uint8_t( value ) );
		}
		static void A8S( uint8_t * srcBuffer, uint8_t value )
		{
			A8U( srcBuffer, uint8_t( value ) );
		}
		static void L32F( uint8_t * srcBuffer, float value )
		{
			L8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void R32F( uint8_t * srcBuffer, float value )
		{
			R8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void G32F( uint8_t * srcBuffer, float value )
		{
			G8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void B32F( uint8_t * srcBuffer, float value )
		{
			B8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void A32F( uint8_t * srcBuffer, float value )
		{
			A8U( srcBuffer, uint8_t( value * 255.0 ) );
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

		static uint8_t L8U( uint8_t const * srcBuffer )
		{
			return uint8_t( R8U( srcBuffer ) * 0.30 + G8U( srcBuffer ) * 0.59 + B8U( srcBuffer ) * 0.11 );
		}
		static uint8_t A8U( uint8_t const * srcBuffer )
		{
			return src.a;
		}
		static uint8_t R8U( uint8_t const * srcBuffer )
		{
			return src.r;
		}
		static uint8_t G8U( uint8_t const * srcBuffer )
		{
			return src.g;
		}
		static uint8_t B8U( uint8_t const * srcBuffer )
		{
			return src.b;
		}
		static int8_t L8S( uint8_t const * srcBuffer )
		{
			return int8_t( L8U( srcBuffer ) );
		}
		static int8_t R8S( uint8_t const * srcBuffer )
		{
			return int8_t( R8U( srcBuffer ) );
		}
		static int8_t G8S( uint8_t const * srcBuffer )
		{
			return int8_t( G8U( srcBuffer ) );
		}
		static int8_t B8S( uint8_t const * srcBuffer )
		{
			return int8_t( B8U( srcBuffer ) );
		}
		static int8_t A8S( uint8_t const * srcBuffer )
		{
			return int8_t( A8U( srcBuffer ) );
		}
		static float L32F( uint8_t const * srcBuffer )
		{
			return L8U( srcBuffer ) / 255.0f;
		}
		static float R32F( uint8_t const * srcBuffer )
		{
			return R8U( srcBuffer ) / 255.0f;
		}
		static float G32F( uint8_t const * srcBuffer )
		{
			return G8U( srcBuffer ) / 255.0f;
		}
		static float B32F( uint8_t const * srcBuffer )
		{
			return B8U( srcBuffer ) / 255.0f;
		}
		static float A32F( uint8_t const * srcBuffer )
		{
			return A8U( srcBuffer ) / 255.0f;
		}
		static void L8U( uint8_t * srcBuffer, uint8_t value )
		{
			R8U( srcBuffer, value );
			G8U( srcBuffer, value );
			B8U( srcBuffer, value );
		}
		static void A8U( uint8_t * srcBuffer, uint8_t value )
		{
			dst.a = value;
		}
		static void R8U( uint8_t * srcBuffer, uint8_t value )
		{
			dst.r = value;
		}
		static void G8U( uint8_t * srcBuffer, uint8_t value )
		{
			dst.g = value;
		}
		static void B8U( uint8_t * srcBuffer, uint8_t value )
		{
			dst.b = value;
		}
		static void L8S( uint8_t * srcBuffer, int8_t value )
		{
			L8U( srcBuffer, uint8_t( value ) );
		}
		static void R8S( uint8_t * srcBuffer, int8_t value )
		{
			R8U( srcBuffer, uint8_t( value ) );
		}
		static void G8S( uint8_t * srcBuffer, int8_t value )
		{
			G8U( srcBuffer, uint8_t( value ) );
		}
		static void B8S( uint8_t * srcBuffer, int8_t value )
		{
			B8U( srcBuffer, uint8_t( value ) );
		}
		static void A8S( uint8_t * srcBuffer, uint8_t value )
		{
			A8U( srcBuffer, uint8_t( value ) );
		}
		static void L32F( uint8_t * srcBuffer, float value )
		{
			L8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void R32F( uint8_t * srcBuffer, float value )
		{
			R8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void G32F( uint8_t * srcBuffer, float value )
		{
			G8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void B32F( uint8_t * srcBuffer, float value )
		{
			B8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void A32F( uint8_t * srcBuffer, float value )
		{
			A8U( srcBuffer, uint8_t( value * 255.0 ) );
		}

#undef src
#undef dst
	};
	//!\~english	Specialisation for PixelFormat::eR16G16B16_SFLOAT
	//!\~french		Spécialisation pour PixelFormat::eR16G16B16_SFLOAT
	template<>
	struct PixelComponents< PixelFormat::eR16G16B16_SFLOAT >
	{

		static uint8_t L8U( uint8_t const * srcBuffer )
		{
			return uint8_t( L8S( srcBuffer ) );
		}
		static uint8_t R8U( uint8_t const * srcBuffer )
		{
			return uint8_t( R8S( srcBuffer ) );
		}
		static uint8_t G8U( uint8_t const * srcBuffer )
		{
			return uint8_t( G8S( srcBuffer ) );
		}
		static uint8_t B8U( uint8_t const * srcBuffer )
		{
			return uint8_t( B8S( srcBuffer ) );
		}
		static uint8_t A8U( uint8_t const * srcBuffer )
		{
			return uint8_t( A8S( srcBuffer ) );
		}
		static int8_t L8S( uint8_t const * srcBuffer )
		{
			return int8_t( L32F( srcBuffer ) * 255 );
		}
		static int8_t R8S( uint8_t const * srcBuffer )
		{
			return int8_t( R32F( srcBuffer ) * 255 );
		}
		static int8_t G8S( uint8_t const * srcBuffer )
		{
			return int8_t( G32F( srcBuffer ) * 255 );
		}
		static int8_t B8S( uint8_t const * srcBuffer )
		{
			return int8_t( B32F( srcBuffer ) * 255 );
		}
		static int8_t A8S( uint8_t const * srcBuffer )
		{
			return int8_t( A32F( srcBuffer ) * 255 );
		}
		static float L32F( uint8_t const * srcBuffer )
		{
			return float( R32F( srcBuffer ) * 0.30 + G32F( srcBuffer ) * 0.59 + B32F( srcBuffer ) * 0.11 );
		}
		static float R32F( uint8_t const * srcBuffer )
		{
			float result{};
			details::halfToFloat( result, reinterpret_cast< uint16_t const * >( srcBuffer ) + 0 );
			return result;
		}
		static float G32F( uint8_t const * srcBuffer )
		{
			float result{};
			details::halfToFloat( result, reinterpret_cast< uint16_t const * >( srcBuffer ) + 1 );
			return result;
		}
		static float B32F( uint8_t const * srcBuffer )
		{
			float result{};
			details::halfToFloat( result, reinterpret_cast< uint16_t const * >( srcBuffer ) + 2 );
			return result;
		}
		static float A32F( uint8_t const * )
		{
			return 1.0f;
		}
		static void L8U( uint8_t * srcBuffer, uint8_t value )
		{
			L8S( srcBuffer, int8_t( value ) );
		}
		static void R8U( uint8_t * srcBuffer, uint8_t value )
		{
			R8S( srcBuffer, int8_t( value ) );
		}
		static void G8U( uint8_t * srcBuffer, uint8_t value )
		{
			G8S( srcBuffer, int8_t( value ) );
		}
		static void B8U( uint8_t * srcBuffer, uint8_t value )
		{
			B8S( srcBuffer, int8_t( value ) );
		}
		static void A8U( uint8_t * srcBuffer, uint8_t value )
		{
			A8S( srcBuffer, int8_t( value ) );
		}
		static void L8S( uint8_t * srcBuffer, int8_t value )
		{
			L32F( srcBuffer, float( value ) / 255 );
		}
		static void R8S( uint8_t * srcBuffer, int8_t value )
		{
			R32F( srcBuffer, float( value ) / 255 );
		}
		static void G8S( uint8_t * srcBuffer, int8_t value )
		{
			G32F( srcBuffer, float( value ) / 255 );
		}
		static void B8S( uint8_t * srcBuffer, int8_t value )
		{
			B32F( srcBuffer, float( value ) / 255 );
		}
		static void A8S( uint8_t * srcBuffer, int8_t value )
		{
			A32F( srcBuffer, float( value ) / 255 );
		}
		static void L32F( uint8_t * srcBuffer, float value )
		{
			R32F( srcBuffer, value );
			G32F( srcBuffer, value );
			B32F( srcBuffer, value );
		}
		static void R32F( uint8_t * srcBuffer, float value )
		{
			details::floatToHalf( reinterpret_cast< uint16_t * >( srcBuffer ) + 0, value );
		}
		static void G32F( uint8_t * srcBuffer, float value )
		{
			details::floatToHalf( reinterpret_cast< uint16_t * >( srcBuffer ) + 1, value );
		}
		static void B32F( uint8_t * srcBuffer, float value )
		{
			details::floatToHalf( reinterpret_cast< uint16_t * >( srcBuffer ) + 2, value );
		}
		static void A32F( uint8_t *, float )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR32G32B32_SFLOAT
	//!\~french		Spécialisation pour PixelFormat::eR32G32B32_SFLOAT
	template<>
	struct PixelComponents< PixelFormat::eR32G32B32_SFLOAT >
	{

		static uint8_t L8U( uint8_t const * srcBuffer )
		{
			return uint8_t( L8S( srcBuffer ) );
		}
		static uint8_t R8U( uint8_t const * srcBuffer )
		{
			return uint8_t( R8S( srcBuffer ) );
		}
		static uint8_t G8U( uint8_t const * srcBuffer )
		{
			return uint8_t( G8S( srcBuffer ) );
		}
		static uint8_t B8U( uint8_t const * srcBuffer )
		{
			return uint8_t( B8S( srcBuffer ) );
		}
		static uint8_t A8U( uint8_t const * srcBuffer )
		{
			return uint8_t( A8S( srcBuffer ) );
		}
		static int8_t L8S( uint8_t const * srcBuffer )
		{
			return int8_t( L32F( srcBuffer ) * 255 );
		}
		static int8_t R8S( uint8_t const * srcBuffer )
		{
			return int8_t( R32F( srcBuffer ) * 255 );
		}
		static int8_t G8S( uint8_t const * srcBuffer )
		{
			return int8_t( G32F( srcBuffer ) * 255 );
		}
		static int8_t B8S( uint8_t const * srcBuffer )
		{
			return int8_t( B32F( srcBuffer ) * 255 );
		}
		static int8_t A8S( uint8_t const * srcBuffer )
		{
			return int8_t( A32F( srcBuffer ) * 255 );
		}
		static float L32F( uint8_t const * srcBuffer )
		{
			return float( R32F( srcBuffer ) * 0.30 + G32F( srcBuffer ) * 0.59 + B32F( srcBuffer ) * 0.11 );
		}
		static float R32F( uint8_t const * srcBuffer )
		{
			return reinterpret_cast< float const * >( srcBuffer )[0];
		}
		static float G32F( uint8_t const * srcBuffer )
		{
			return reinterpret_cast< float const * >( srcBuffer )[1];
		}
		static float B32F( uint8_t const * srcBuffer )
		{
			return reinterpret_cast< float const * >( srcBuffer )[2];
		}
		static float A32F( uint8_t const * )
		{
			return 1.0f;
		}
		static void L8U( uint8_t * srcBuffer, uint8_t value )
		{
			L8S( srcBuffer, int8_t( value ) );
		}
		static void R8U( uint8_t * srcBuffer, uint8_t value )
		{
			R8S( srcBuffer, int8_t( value ) );
		}
		static void G8U( uint8_t * srcBuffer, uint8_t value )
		{
			G8S( srcBuffer, int8_t( value ) );
		}
		static void B8U( uint8_t * srcBuffer, uint8_t value )
		{
			B8S( srcBuffer, int8_t( value ) );
		}
		static void A8U( uint8_t * srcBuffer, uint8_t value )
		{
			A8S( srcBuffer, int8_t( value ) );
		}
		static void L8S( uint8_t * srcBuffer, int8_t value )
		{
			L32F( srcBuffer, float( value ) / 255 );
		}
		static void R8S( uint8_t * srcBuffer, int8_t value )
		{
			R32F( srcBuffer, float( value ) / 255 );
		}
		static void G8S( uint8_t * srcBuffer, int8_t value )
		{
			G32F( srcBuffer, float( value ) / 255 );
		}
		static void B8S( uint8_t * srcBuffer, int8_t value )
		{
			B32F( srcBuffer, float( value ) / 255 );
		}
		static void A8S( uint8_t * srcBuffer, int8_t value )
		{
			A32F( srcBuffer, float( value ) / 255 );
		}
		static void L32F( uint8_t * srcBuffer, float value )
		{
			R32F( srcBuffer, value );
			G32F( srcBuffer, value );
			B32F( srcBuffer, value );
		}
		static void R32F( uint8_t * srcBuffer, float value )
		{
			reinterpret_cast< float * >( srcBuffer )[0] = value;
		}
		static void G32F( uint8_t * srcBuffer, float value )
		{
			reinterpret_cast< float * >( srcBuffer )[1] = value;
		}
		static void B32F( uint8_t * srcBuffer, float value )
		{
			reinterpret_cast< float * >( srcBuffer )[2] = value;
		}
		static void A32F( uint8_t *, float )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR16G16B16A16_SFLOAT
	//!\~french		Spécialisation pour PixelFormat::eR16G16B16A16_SFLOAT
	template<>
	struct PixelComponents< PixelFormat::eR16G16B16A16_SFLOAT >
	{
		static uint8_t L8U( uint8_t const * srcBuffer )
		{
			return uint8_t( L8S( srcBuffer ) );
		}
		static uint8_t R8U( uint8_t const * srcBuffer )
		{
			return uint8_t( R8S( srcBuffer ) );
		}
		static uint8_t G8U( uint8_t const * srcBuffer )
		{
			return uint8_t( G8S( srcBuffer ) );
		}
		static uint8_t B8U( uint8_t const * srcBuffer )
		{
			return uint8_t( B8S( srcBuffer ) );
		}
		static uint8_t A8U( uint8_t const * srcBuffer )
		{
			return uint8_t( A8S( srcBuffer ) );
		}
		static int8_t L8S( uint8_t const * srcBuffer )
		{
			return int8_t( L32F( srcBuffer ) * 255 );
		}
		static int8_t R8S( uint8_t const * srcBuffer )
		{
			return int8_t( R32F( srcBuffer ) * 255 );
		}
		static int8_t G8S( uint8_t const * srcBuffer )
		{
			return int8_t( G32F( srcBuffer ) * 255 );
		}
		static int8_t B8S( uint8_t const * srcBuffer )
		{
			return int8_t( B32F( srcBuffer ) * 255 );
		}
		static int8_t A8S( uint8_t const * srcBuffer )
		{
			return int8_t( A32F( srcBuffer ) * 255 );
		}
		static float L32F( uint8_t const * srcBuffer )
		{
			return float( R32F( srcBuffer ) * 0.30 + G32F( srcBuffer ) * 0.59 + B32F( srcBuffer ) * 0.11 );
		}
		static float R32F( uint8_t const * srcBuffer )
		{
			float result{};
			details::halfToFloat( result, reinterpret_cast< uint16_t const * >( srcBuffer ) + 0 );
			return result;
		}
		static float G32F( uint8_t const * srcBuffer )
		{
			float result{};
			details::halfToFloat( result, reinterpret_cast< uint16_t const * >( srcBuffer ) + 1 );
			return result;
		}
		static float B32F( uint8_t const * srcBuffer )
		{
			float result{};
			details::halfToFloat( result, reinterpret_cast< uint16_t const * >( srcBuffer ) + 2 );
			return result;
		}
		static float A32F( uint8_t const * srcBuffer )
		{
			float result{};
			details::halfToFloat( result, reinterpret_cast< uint16_t const * >( srcBuffer ) + 3 );
			return result;
		}
		static void L8U( uint8_t * srcBuffer, uint8_t value )
		{
			L8S( srcBuffer, int8_t( value ) );
		}
		static void R8U( uint8_t * srcBuffer, uint8_t value )
		{
			R8S( srcBuffer, int8_t( value ) );
		}
		static void G8U( uint8_t * srcBuffer, uint8_t value )
		{
			G8S( srcBuffer, int8_t( value ) );
		}
		static void B8U( uint8_t * srcBuffer, uint8_t value )
		{
			B8S( srcBuffer, int8_t( value ) );
		}
		static void A8U( uint8_t * srcBuffer, uint8_t value )
		{
			A8S( srcBuffer, int8_t( value ) );
		}
		static void L8S( uint8_t * srcBuffer, int8_t value )
		{
			L32F( srcBuffer, float( value ) / 255 );
		}
		static void R8S( uint8_t * srcBuffer, int8_t value )
		{
			R32F( srcBuffer, float( value ) / 255 );
		}
		static void G8S( uint8_t * srcBuffer, int8_t value )
		{
			G32F( srcBuffer, float( value ) / 255 );
		}
		static void B8S( uint8_t * srcBuffer, int8_t value )
		{
			B32F( srcBuffer, float( value ) / 255 );
		}
		static void A8S( uint8_t * srcBuffer, int8_t value )
		{
			A32F( srcBuffer, float( value ) / 255 );
		}
		static void L32F( uint8_t * srcBuffer, float value )
		{
			R32F( srcBuffer, value );
			G32F( srcBuffer, value );
			B32F( srcBuffer, value );
		}
		static void R32F( uint8_t * srcBuffer, float value )
		{
			details::floatToHalf( reinterpret_cast< uint16_t * >( srcBuffer ) + 0, value );
		}
		static void G32F( uint8_t * srcBuffer, float value )
		{
			details::floatToHalf( reinterpret_cast< uint16_t * >( srcBuffer ) + 1, value );
		}
		static void B32F( uint8_t * srcBuffer, float value )
		{
			details::floatToHalf( reinterpret_cast< uint16_t * >( srcBuffer ) + 2, value );
		}
		static void A32F( uint8_t * srcBuffer, float value )
		{
			details::floatToHalf( reinterpret_cast< uint16_t * >( srcBuffer ) + 3, value );
		}
	};
	//!\~english	Specialisation for PixelFormat::eR32G32B32A32_SFLOAT
	//!\~french		Spécialisation pour PixelFormat::eR32G32B32A32_SFLOAT
	template<>
	struct PixelComponents< PixelFormat::eR32G32B32A32_SFLOAT >
	{
#define src	( reinterpret_cast< float const * >( srcBuffer ) )
#define dst	( reinterpret_cast< float * >( srcBuffer ) )

		static uint8_t L8U( uint8_t const * srcBuffer )
		{
			return uint8_t( L8S( srcBuffer ) );
		}
		static uint8_t R8U( uint8_t const * srcBuffer )
		{
			return uint8_t( R8S( srcBuffer ) );
		}
		static uint8_t G8U( uint8_t const * srcBuffer )
		{
			return uint8_t( G8S( srcBuffer ) );
		}
		static uint8_t B8U( uint8_t const * srcBuffer )
		{
			return uint8_t( B8S( srcBuffer ) );
		}
		static uint8_t A8U( uint8_t const * srcBuffer )
		{
			return uint8_t( A8S( srcBuffer ) );
		}
		static int8_t L8S( uint8_t const * srcBuffer )
		{
			return int8_t( L32F( srcBuffer ) * 255 );
		}
		static int8_t R8S( uint8_t const * srcBuffer )
		{
			return int8_t( R32F( srcBuffer ) * 255 );
		}
		static int8_t G8S( uint8_t const * srcBuffer )
		{
			return int8_t( G32F( srcBuffer ) * 255 );
		}
		static int8_t B8S( uint8_t const * srcBuffer )
		{
			return int8_t( B32F( srcBuffer ) * 255 );
		}
		static int8_t A8S( uint8_t const * srcBuffer )
		{
			return int8_t( A32F( srcBuffer ) * 255 );
		}
		static float L32F( uint8_t const * srcBuffer )
		{
			return float( R32F( srcBuffer ) * 0.30 + G32F( srcBuffer ) * 0.59 + B32F( srcBuffer ) * 0.11 );
		}
		static float R32F( uint8_t const * srcBuffer )
		{
			return src[0];
		}
		static float G32F( uint8_t const * srcBuffer )
		{
			return src[1];
		}
		static float B32F( uint8_t const * srcBuffer )
		{
			return src[2];
		}
		static float A32F( uint8_t const * srcBuffer )
		{
			return src[3];
		}
		static void L8U( uint8_t * srcBuffer, uint8_t value )
		{
			L8S( srcBuffer, int8_t( value ) );
		}
		static void R8U( uint8_t * srcBuffer, uint8_t value )
		{
			R8S( srcBuffer, int8_t( value ) );
		}
		static void G8U( uint8_t * srcBuffer, uint8_t value )
		{
			G8S( srcBuffer, int8_t( value ) );
		}
		static void B8U( uint8_t * srcBuffer, uint8_t value )
		{
			B8S( srcBuffer, int8_t( value ) );
		}
		static void A8U( uint8_t * srcBuffer, uint8_t value )
		{
			A8S( srcBuffer, int8_t( value ) );
		}
		static void L8S( uint8_t * srcBuffer, int8_t value )
		{
			L32F( srcBuffer, float( value ) / 255 );
		}
		static void R8S( uint8_t * srcBuffer, int8_t value )
		{
			R32F( srcBuffer, float( value ) / 255 );
		}
		static void G8S( uint8_t * srcBuffer, int8_t value )
		{
			G32F( srcBuffer, float( value ) / 255 );
		}
		static void B8S( uint8_t * srcBuffer, int8_t value )
		{
			B32F( srcBuffer, float( value ) / 255 );
		}
		static void A8S( uint8_t * srcBuffer, int8_t value )
		{
			A32F( srcBuffer, float( value ) / 255 );
		}
		static void L32F( uint8_t * srcBuffer, float value )
		{
			R32F( srcBuffer, value );
			G32F( srcBuffer, value );
			B32F( srcBuffer, value );
		}
		static void R32F( uint8_t * srcBuffer, float value )
		{
			dst[0] = value;
		}
		static void G32F( uint8_t * srcBuffer, float value )
		{
			dst[1] = value;
		}
		static void B32F( uint8_t * srcBuffer, float value )
		{
			dst[2] = value;
		}
		static void A32F( uint8_t * srcBuffer, float value )
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

		static uint8_t D8( uint8_t const * srcBuffer )
		{
			return uint8_t( src >> 8 );
		}
		static uint16_t D16( uint8_t const * srcBuffer )
		{
			return src;
		}
		static uint32_t D24( uint8_t const * srcBuffer )
		{
			return ( uint32_t( src ) << 8 );
		}
		static uint32_t D32( uint8_t const * srcBuffer )
		{
			return ( uint32_t( src ) << 16 );
		}
		static uint8_t S1( uint8_t const * )
		{
			return 0;
		}
		static uint8_t S8( uint8_t const * )
		{
			return 0;
		}
		static float D32F( uint8_t const * srcBuffer )
		{
			return float( D32( srcBuffer ) );
		}
		static float S32F( uint8_t const * )
		{
			return 0;
		}
		static void D8( uint8_t * srcBuffer, uint8_t value )
		{
			dst = ( uint16_t( value ) << 8 );
		}
		static void D16( uint8_t * srcBuffer, uint16_t value )
		{
			dst = value;
		}
		static void D24( uint8_t * srcBuffer, uint32_t value )
		{
			dst = ( src & 0xFFFF0000 ) | ( 0x0000FFFF & ( value >> 8 ) );
		}
		static void D32( uint8_t * srcBuffer, uint32_t value )
		{
			dst = ( src & 0xFFFF0000 ) | ( 0x0000FFFF & ( value >> 16 ) );
		}
		static void S1( uint8_t *, uint8_t )
		{
		}
		static void S8( uint8_t *, uint8_t )
		{
		}
		static void D32F( uint8_t * srcBuffer, float value )
		{
			D32( srcBuffer, uint32_t( value ) );
		}
		static void S32F( uint8_t *, float )
		{
		}
		static uint8_t L8U( uint8_t const * srcBuffer )
		{
			return D8( srcBuffer );
		}
		static uint8_t A8U( uint8_t const * srcBuffer )
		{
			return 0;
		}
		static uint8_t R8U( uint8_t const * srcBuffer )
		{
			return L8U( srcBuffer );
		}
		static uint8_t G8U( uint8_t const * srcBuffer )
		{
			return L8U( srcBuffer );
		}
		static uint8_t B8U( uint8_t const * srcBuffer )
		{
			return L8U( srcBuffer );
		}
		static int8_t L8S( uint8_t const * srcBuffer )
		{
			return int8_t( L8U( srcBuffer ) );
		}
		static int8_t R8S( uint8_t const * srcBuffer )
		{
			return int8_t( R8U( srcBuffer ) );
		}
		static int8_t G8S( uint8_t const * srcBuffer )
		{
			return int8_t( G8U( srcBuffer ) );
		}
		static int8_t B8S( uint8_t const * srcBuffer )
		{
			return int8_t( B8U( srcBuffer ) );
		}
		static int8_t A8S( uint8_t const * srcBuffer )
		{
			return int8_t( A8U( srcBuffer ) );
		}
		static float L32F( uint8_t const * srcBuffer )
		{
			return L8U( srcBuffer ) / 255.0f;
		}
		static float R32F( uint8_t const * srcBuffer )
		{
			return R8U( srcBuffer ) / 255.0f;
		}
		static float G32F( uint8_t const * srcBuffer )
		{
			return G8U( srcBuffer ) / 255.0f;
		}
		static float B32F( uint8_t const * srcBuffer )
		{
			return B8U( srcBuffer ) / 255.0f;
		}
		static float A32F( uint8_t const * srcBuffer )
		{
			return A8U( srcBuffer ) / 255.0f;
		}
		static void L8U( uint8_t * srcBuffer, uint8_t value )
		{
			D8( srcBuffer, value );
		}
		static void A8U( uint8_t * srcBuffer, uint8_t value )
		{
		}
		static void R8U( uint8_t * srcBuffer, uint8_t value )
		{
			L8U( srcBuffer, value );
		}
		static void G8U( uint8_t * srcBuffer, uint8_t value )
		{
			L8U( srcBuffer, value );
		}
		static void B8U( uint8_t * srcBuffer, uint8_t value )
		{
			L8U( srcBuffer, value );
		}
		static void L8S( uint8_t * srcBuffer, int8_t value )
		{
			L8U( srcBuffer, uint8_t( value ) );
		}
		static void R8S( uint8_t * srcBuffer, int8_t value )
		{
			R8U( srcBuffer, uint8_t( value ) );
		}
		static void G8S( uint8_t * srcBuffer, int8_t value )
		{
			G8U( srcBuffer, uint8_t( value ) );
		}
		static void B8S( uint8_t * srcBuffer, int8_t value )
		{
			B8U( srcBuffer, uint8_t( value ) );
		}
		static void A8S( uint8_t * srcBuffer, uint8_t value )
		{
			A8U( srcBuffer, uint8_t( value ) );
		}
		static void L32F( uint8_t * srcBuffer, float value )
		{
			L8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void R32F( uint8_t * srcBuffer, float value )
		{
			R8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void G32F( uint8_t * srcBuffer, float value )
		{
			G8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void B32F( uint8_t * srcBuffer, float value )
		{
			B8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void A32F( uint8_t * srcBuffer, float value )
		{
			A8U( srcBuffer, uint8_t( value * 255.0 ) );
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

		static uint8_t	BIT( uint8_t const  p_bySrc )
		{
			return ( ( p_bySrc >> 0 ) & 0x01 ) | ( ( p_bySrc >> 1 ) & 0x01 ) | ( ( p_bySrc >> 2 ) & 0x01 ) | ( ( p_bySrc >> 3 ) & 0x01 ) | ( ( p_bySrc >> 4 ) & 0x01 ) | ( ( p_bySrc >> 5 ) & 0x01 ) | ( ( p_bySrc >> 6 ) & 0x01 ) | ( ( p_bySrc >> 7 ) & 0x01 );
		}
		static uint8_t D8( uint8_t const * srcBuffer )
		{
			return uint8_t( ( src & 0xFF000000 ) >> 24 );
		}
		static uint16_t D16( uint8_t const * srcBuffer )
		{
			return uint16_t( ( src & 0xFFFF0000 ) >> 16 );
		}
		static uint32_t D24( uint8_t const * srcBuffer )
		{
			return uint32_t( ( src & 0xFFFFFF00 ) >> 8 );
		}
		static uint32_t D32( uint8_t const * srcBuffer )
		{
			return uint32_t( ( src & 0xFFFFFF00 ) >> 8 );
		}
		static uint8_t	S1( uint8_t const * srcBuffer )
		{
			return ( BIT( srcBuffer[3] ) << 0 ) | ( BIT( srcBuffer[7] ) << 1 ) | ( BIT( srcBuffer[11] ) << 2 ) | ( BIT( srcBuffer[15] ) << 3 ) | ( BIT( srcBuffer[19] ) << 4 ) | ( BIT( srcBuffer[23] ) << 5 ) | ( BIT( srcBuffer[27] ) << 6 ) | ( BIT( srcBuffer[31] ) << 7 );
		}
		static uint8_t S8( uint8_t const * srcBuffer )
		{
			return uint8_t( src & 0x000000FF );
		}
		static float D32F( uint8_t const * srcBuffer )
		{
			return float( D32( srcBuffer ) );
		}
		static float S32F( uint8_t const * srcBuffer )
		{
			return float( S8( srcBuffer ) ) / 255.0f;
		}
		static void D8( uint8_t * srcBuffer, uint8_t value )
		{
			dst = ( src & 0x000000FF ) | ( ( uint32_t( value ) << 24 ) & 0xFF000000 );
		}
		static void D16( uint8_t * srcBuffer, uint16_t value )
		{
			dst = ( src & 0x000000FF ) | ( ( uint32_t( value ) << 16 ) & 0xFFFF0000 );
		}
		static void D24( uint8_t * srcBuffer, uint32_t value )
		{
			dst = ( src & 0x000000FF ) | ( ( value << 8 ) & 0xFFFFFF00 );
		}
		static void D32( uint8_t * srcBuffer, uint32_t value )
		{
			dst = ( src & 0x000000FF ) | ( value & 0xFFFFFF00 );
		}
		static void S1( uint8_t * srcBuffer, uint8_t value )
		{
		}
		static void S8( uint8_t * srcBuffer, uint8_t value )
		{
			dst = ( src & 0xFFFFFF00 ) | ( value & 0x000000FF );
		}
		static void D32F( uint8_t * srcBuffer, float value )
		{
			D32( srcBuffer, uint32_t( value ) );
		}
		static void S32F( uint8_t * srcBuffer, float value )
		{
			S8( srcBuffer, uint8_t( value * 255.0f ) );
		}
		static uint8_t L8U( uint8_t const * srcBuffer )
		{
			return D8( srcBuffer );
		}
		static uint8_t A8U( uint8_t const * srcBuffer )
		{
			return S8( srcBuffer );
		}
		static uint8_t R8U( uint8_t const * srcBuffer )
		{
			return L8U( srcBuffer );
		}
		static uint8_t G8U( uint8_t const * srcBuffer )
		{
			return L8U( srcBuffer );
		}
		static uint8_t B8U( uint8_t const * srcBuffer )
		{
			return L8U( srcBuffer );
		}
		static int8_t L8S( uint8_t const * srcBuffer )
		{
			return int8_t( L8U( srcBuffer ) );
		}
		static int8_t R8S( uint8_t const * srcBuffer )
		{
			return int8_t( R8U( srcBuffer ) );
		}
		static int8_t G8S( uint8_t const * srcBuffer )
		{
			return int8_t( G8U( srcBuffer ) );
		}
		static int8_t B8S( uint8_t const * srcBuffer )
		{
			return int8_t( B8U( srcBuffer ) );
		}
		static int8_t A8S( uint8_t const * srcBuffer )
		{
			return int8_t( A8U( srcBuffer ) );
		}
		static float L32F( uint8_t const * srcBuffer )
		{
			return L8U( srcBuffer ) / 255.0f;
		}
		static float R32F( uint8_t const * srcBuffer )
		{
			return R8U( srcBuffer ) / 255.0f;
		}
		static float G32F( uint8_t const * srcBuffer )
		{
			return G8U( srcBuffer ) / 255.0f;
		}
		static float B32F( uint8_t const * srcBuffer )
		{
			return B8U( srcBuffer ) / 255.0f;
		}
		static float A32F( uint8_t const * srcBuffer )
		{
			return A8U( srcBuffer ) / 255.0f;
		}
		static void L8U( uint8_t * srcBuffer, uint8_t value )
		{
			D8( srcBuffer, value );
		}
		static void A8U( uint8_t * srcBuffer, uint8_t value )
		{
			S8( srcBuffer, value );
		}
		static void R8U( uint8_t * srcBuffer, uint8_t value )
		{
			L8U( srcBuffer, value );
		}
		static void G8U( uint8_t * srcBuffer, uint8_t value )
		{
			L8U( srcBuffer, value );
		}
		static void B8U( uint8_t * srcBuffer, uint8_t value )
		{
			L8U( srcBuffer, value );
		}
		static void L8S( uint8_t * srcBuffer, int8_t value )
		{
			L8U( srcBuffer, uint8_t( value ) );
		}
		static void R8S( uint8_t * srcBuffer, int8_t value )
		{
			R8U( srcBuffer, uint8_t( value ) );
		}
		static void G8S( uint8_t * srcBuffer, int8_t value )
		{
			G8U( srcBuffer, uint8_t( value ) );
		}
		static void B8S( uint8_t * srcBuffer, int8_t value )
		{
			B8U( srcBuffer, uint8_t( value ) );
		}
		static void A8S( uint8_t * srcBuffer, uint8_t value )
		{
			A8U( srcBuffer, uint8_t( value ) );
		}
		static void L32F( uint8_t * srcBuffer, float value )
		{
			L8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void R32F( uint8_t * srcBuffer, float value )
		{
			R8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void G32F( uint8_t * srcBuffer, float value )
		{
			G8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void B32F( uint8_t * srcBuffer, float value )
		{
			B8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void A32F( uint8_t * srcBuffer, float value )
		{
			A8U( srcBuffer, uint8_t( value * 255.0 ) );
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

		static uint8_t D8( uint8_t const * srcBuffer )
		{
			return uint8_t( src >> 24 );
		}
		static uint16_t D16( uint8_t const * srcBuffer )
		{
			return uint16_t( src >> 16 );
		}
		static uint32_t D24( uint8_t const * srcBuffer )
		{
			return ( src >> 8 );
		}
		static uint32_t D32( uint8_t const * srcBuffer )
		{
			return src;
		}
		static uint8_t S1( uint8_t const * )
		{
			return 0;
		}
		static uint8_t S8( uint8_t const * )
		{
			return 0;
		}
		static float D32F( uint8_t const * srcBuffer )
		{
			return float( D32( srcBuffer ) );
		}
		static float S32F( uint8_t const * )
		{
			return 0;
		}
		static void D8( uint8_t * srcBuffer, uint8_t value )
		{
			dst = ( uint32_t( value ) << 24 );
		}
		static void D16( uint8_t * srcBuffer, uint16_t value )
		{
			dst = ( uint32_t( value ) << 16 );
		}
		static void D24( uint8_t * srcBuffer, uint32_t value )
		{
			dst = ( value << 8 );
		}
		static void D32( uint8_t * srcBuffer, uint32_t value )
		{
			dst = value;
		}
		static void S1( uint8_t *, uint8_t )
		{
		}
		static void S8( uint8_t *, uint8_t )
		{
		}
		static void D32F( uint8_t * srcBuffer, float value )
		{
			D32( srcBuffer, uint32_t( value ) );
		}
		static void S32F( uint8_t *, float )
		{
		}
		static uint8_t L8U( uint8_t const * srcBuffer )
		{
			return D8( srcBuffer );
		}
		static uint8_t A8U( uint8_t const * srcBuffer )
		{
			return 0xFF;
		}
		static uint8_t R8U( uint8_t const * srcBuffer )
		{
			return L8U( srcBuffer );
		}
		static uint8_t G8U( uint8_t const * srcBuffer )
		{
			return L8U( srcBuffer );
		}
		static uint8_t B8U( uint8_t const * srcBuffer )
		{
			return L8U( srcBuffer );
		}
		static int8_t L8S( uint8_t const * srcBuffer )
		{
			return int8_t( L8U( srcBuffer ) );
		}
		static int8_t R8S( uint8_t const * srcBuffer )
		{
			return int8_t( R8U( srcBuffer ) );
		}
		static int8_t G8S( uint8_t const * srcBuffer )
		{
			return int8_t( G8U( srcBuffer ) );
		}
		static int8_t B8S( uint8_t const * srcBuffer )
		{
			return int8_t( B8U( srcBuffer ) );
		}
		static int8_t A8S( uint8_t const * srcBuffer )
		{
			return int8_t( A8U( srcBuffer ) );
		}
		static float L32F( uint8_t const * srcBuffer )
		{
			return L8U( srcBuffer ) / 255.0f;
		}
		static float R32F( uint8_t const * srcBuffer )
		{
			return R8U( srcBuffer ) / 255.0f;
		}
		static float G32F( uint8_t const * srcBuffer )
		{
			return G8U( srcBuffer ) / 255.0f;
		}
		static float B32F( uint8_t const * srcBuffer )
		{
			return B8U( srcBuffer ) / 255.0f;
		}
		static float A32F( uint8_t const * srcBuffer )
		{
			return A8U( srcBuffer ) / 255.0f;
		}
		static void L8U( uint8_t * srcBuffer, uint8_t value )
		{
			D8( srcBuffer, value );
		}
		static void A8U( uint8_t * srcBuffer, uint8_t value )
		{
		}
		static void R8U( uint8_t * srcBuffer, uint8_t value )
		{
			L8U( srcBuffer, value );
		}
		static void G8U( uint8_t * srcBuffer, uint8_t value )
		{
			L8U( srcBuffer, value );
		}
		static void B8U( uint8_t * srcBuffer, uint8_t value )
		{
			L8U( srcBuffer, value );
		}
		static void L8S( uint8_t * srcBuffer, int8_t value )
		{
			L8U( srcBuffer, uint8_t( value ) );
		}
		static void R8S( uint8_t * srcBuffer, int8_t value )
		{
			R8U( srcBuffer, uint8_t( value ) );
		}
		static void G8S( uint8_t * srcBuffer, int8_t value )
		{
			G8U( srcBuffer, uint8_t( value ) );
		}
		static void B8S( uint8_t * srcBuffer, int8_t value )
		{
			B8U( srcBuffer, uint8_t( value ) );
		}
		static void A8S( uint8_t * srcBuffer, uint8_t value )
		{
			A8U( srcBuffer, uint8_t( value ) );
		}
		static void L32F( uint8_t * srcBuffer, float value )
		{
			L8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void R32F( uint8_t * srcBuffer, float value )
		{
			R8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void G32F( uint8_t * srcBuffer, float value )
		{
			G8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void B32F( uint8_t * srcBuffer, float value )
		{
			B8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void A32F( uint8_t * srcBuffer, float value )
		{
			A8U( srcBuffer, uint8_t( value * 255.0 ) );
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

		static float D32F( uint8_t const * srcBuffer )
		{
			return src;
		}
		static float S32F( uint8_t const * )
		{
			return 0;
		}
		static uint8_t D8( uint8_t const * srcBuffer )
		{
			return uint8_t( src * 256 );
		}
		static uint16_t D16( uint8_t const * srcBuffer )
		{
			return uint8_t( src * 65536 );
		}
		static uint32_t D24( uint8_t const * srcBuffer )
		{
			return uint32_t( src * 65536 * 256 );
		}
		static uint32_t D32( uint8_t const * srcBuffer )
		{
			return uint32_t( src * 65536 * 65536 );
		}
		static uint8_t S1( uint8_t const * )
		{
			return 0;
		}
		static uint8_t S8( uint8_t const * )
		{
			return 0;
		}
		static void D32F( uint8_t * srcBuffer, float value )
		{
			dst = value;
		}
		static void S32F( uint8_t *, float )
		{
		}
		static void D8( uint8_t * srcBuffer, uint8_t value )
		{
			dst = float( value ) / float( std::numeric_limits< uint8_t >::max() );
		}
		static void D16( uint8_t * srcBuffer, uint16_t value )
		{
			dst = float( value ) / float( std::numeric_limits< uint16_t >::max() );
		}
		static void D24( uint8_t * srcBuffer, uint32_t value )
		{
			dst = float( value ) / float( std::numeric_limits< uint16_t >::max() * std::numeric_limits< uint8_t >::max() );
		}
		static void D32( uint8_t * srcBuffer, uint32_t value )
		{
			dst = float( value ) / float( std::numeric_limits< uint32_t >::max() );
		}
		static void S1( uint8_t *, uint8_t )
		{
		}
		static void S8( uint8_t *, uint8_t )
		{
		}
		static uint8_t L8U( uint8_t const * srcBuffer )
		{
			return uint8_t( L32F( srcBuffer ) * 255 );
		}
		static uint8_t A8U( uint8_t const * srcBuffer )
		{
			return 0xFF;
		}
		static uint8_t R8U( uint8_t const * srcBuffer )
		{
			return L8U( srcBuffer );
		}
		static uint8_t G8U( uint8_t const * srcBuffer )
		{
			return L8U( srcBuffer );
		}
		static uint8_t B8U( uint8_t const * srcBuffer )
		{
			return L8U( srcBuffer );
		}
		static int8_t L8S( uint8_t const * srcBuffer )
		{
			return int8_t( L8U( srcBuffer ) );
		}
		static int8_t R8S( uint8_t const * srcBuffer )
		{
			return int8_t( R8U( srcBuffer ) );
		}
		static int8_t G8S( uint8_t const * srcBuffer )
		{
			return int8_t( G8U( srcBuffer ) );
		}
		static int8_t B8S( uint8_t const * srcBuffer )
		{
			return int8_t( B8U( srcBuffer ) );
		}
		static int8_t A8S( uint8_t const * srcBuffer )
		{
			return int8_t( A8U( srcBuffer ) );
		}
		static float L32F( uint8_t const * srcBuffer )
		{
			return src;
		}
		static float R32F( uint8_t const * srcBuffer )
		{
			return L32F( srcBuffer );
		}
		static float G32F( uint8_t const * srcBuffer )
		{
			return L32F( srcBuffer );
		}
		static float B32F( uint8_t const * srcBuffer )
		{
			return L32F( srcBuffer );
		}
		static float A32F( uint8_t const * srcBuffer )
		{
			return 1.0f;
		}
		static void L8U( uint8_t * srcBuffer, uint8_t value )
		{
			D8( srcBuffer, value );
		}
		static void A8U( uint8_t * srcBuffer, uint8_t value )
		{
		}
		static void R8U( uint8_t * srcBuffer, uint8_t value )
		{
			L8U( srcBuffer, value );
		}
		static void G8U( uint8_t * srcBuffer, uint8_t value )
		{
			L8U( srcBuffer, value );
		}
		static void B8U( uint8_t * srcBuffer, uint8_t value )
		{
			L8U( srcBuffer, value );
		}
		static void L8S( uint8_t * srcBuffer, int8_t value )
		{
			L8U( srcBuffer, uint8_t( value ) );
		}
		static void R8S( uint8_t * srcBuffer, int8_t value )
		{
			R8U( srcBuffer, uint8_t( value ) );
		}
		static void G8S( uint8_t * srcBuffer, int8_t value )
		{
			G8U( srcBuffer, uint8_t( value ) );
		}
		static void B8S( uint8_t * srcBuffer, int8_t value )
		{
			B8U( srcBuffer, uint8_t( value ) );
		}
		static void A8S( uint8_t * srcBuffer, uint8_t value )
		{
			A8U( srcBuffer, uint8_t( value ) );
		}
		static void L32F( uint8_t * srcBuffer, float value )
		{
			dst = value;
		}
		static void R32F( uint8_t * srcBuffer, float value )
		{
			L32F( srcBuffer, value );
		}
		static void G32F( uint8_t * srcBuffer, float value )
		{
			L32F( srcBuffer, value );
		}
		static void B32F( uint8_t * srcBuffer, float value )
		{
			L32F( srcBuffer, value );
		}
		static void A32F( uint8_t * srcBuffer, float value )
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

		static float D32F( uint8_t const * srcBuffer )
		{
			return src;
		}
		static float S32F( uint8_t const * )
		{
			return 0;
		}
		static uint8_t D8( uint8_t const * srcBuffer )
		{
			return uint8_t( src * 256 );
		}
		static uint16_t D16( uint8_t const * srcBuffer )
		{
			return uint8_t( src * 65536 );
		}
		static uint32_t D24( uint8_t const * srcBuffer )
		{
			return uint32_t( src * 65536 * 256 );
		}
		static uint32_t D32( uint8_t const * srcBuffer )
		{
			return uint32_t( src * 65536 * 65536 );
		}
		static uint8_t S1( uint8_t const * )
		{
			return 0;
		}
		static uint8_t S8( uint8_t const * )
		{
			return 0;
		}
		static void D32F( uint8_t * srcBuffer, float value )
		{
			dst = value;
		}
		static void S32F( uint8_t *, float )
		{
		}
		static void D8( uint8_t * srcBuffer, uint8_t value )
		{
			dst = float( value ) / float( std::numeric_limits< uint8_t >::max() );
		}
		static void D16( uint8_t * srcBuffer, uint16_t value )
		{
			dst = float( value ) / float( std::numeric_limits< uint16_t >::max() );
		}
		static void D24( uint8_t * srcBuffer, uint32_t value )
		{
			dst = float( value ) / float( std::numeric_limits< uint16_t >::max() * std::numeric_limits< uint8_t >::max() );
		}
		static void D32( uint8_t * srcBuffer, uint32_t value )
		{
			dst = float( value ) / float( std::numeric_limits< uint32_t >::max() );
		}
		static void S1( uint8_t *, uint8_t )
		{
		}
		static void S8( uint8_t *, uint8_t )
		{
		}
		static uint8_t L8U( uint8_t const * srcBuffer )
		{
			return uint8_t( L32F( srcBuffer ) * 255 );
		}
		static uint8_t A8U( uint8_t const * srcBuffer )
		{
			return 0xFF;
		}
		static uint8_t R8U( uint8_t const * srcBuffer )
		{
			return L8U( srcBuffer );
		}
		static uint8_t G8U( uint8_t const * srcBuffer )
		{
			return L8U( srcBuffer );
		}
		static uint8_t B8U( uint8_t const * srcBuffer )
		{
			return L8U( srcBuffer );
		}
		static int8_t L8S( uint8_t const * srcBuffer )
		{
			return int8_t( L8U( srcBuffer ) );
		}
		static int8_t R8S( uint8_t const * srcBuffer )
		{
			return int8_t( R8U( srcBuffer ) );
		}
		static int8_t G8S( uint8_t const * srcBuffer )
		{
			return int8_t( G8U( srcBuffer ) );
		}
		static int8_t B8S( uint8_t const * srcBuffer )
		{
			return int8_t( B8U( srcBuffer ) );
		}
		static int8_t A8S( uint8_t const * srcBuffer )
		{
			return int8_t( A8U( srcBuffer ) );
		}
		static float L32F( uint8_t const * srcBuffer )
		{
			return src;
		}
		static float R32F( uint8_t const * srcBuffer )
		{
			return L32F( srcBuffer );
		}
		static float G32F( uint8_t const * srcBuffer )
		{
			return L32F( srcBuffer );
		}
		static float B32F( uint8_t const * srcBuffer )
		{
			return L32F( srcBuffer );
		}
		static float A32F( uint8_t const * srcBuffer )
		{
			return 1.0f;
		}
		static void L8U( uint8_t * srcBuffer, uint8_t value )
		{
			D8( srcBuffer, value );
		}
		static void A8U( uint8_t * srcBuffer, uint8_t value )
		{
		}
		static void R8U( uint8_t * srcBuffer, uint8_t value )
		{
			L8U( srcBuffer, value );
		}
		static void G8U( uint8_t * srcBuffer, uint8_t value )
		{
			L8U( srcBuffer, value );
		}
		static void B8U( uint8_t * srcBuffer, uint8_t value )
		{
			L8U( srcBuffer, value );
		}
		static void L8S( uint8_t * srcBuffer, int8_t value )
		{
			L8U( srcBuffer, uint8_t( value ) );
		}
		static void R8S( uint8_t * srcBuffer, int8_t value )
		{
			R8U( srcBuffer, uint8_t( value ) );
		}
		static void G8S( uint8_t * srcBuffer, int8_t value )
		{
			G8U( srcBuffer, uint8_t( value ) );
		}
		static void B8S( uint8_t * srcBuffer, int8_t value )
		{
			B8U( srcBuffer, uint8_t( value ) );
		}
		static void A8S( uint8_t * srcBuffer, uint8_t value )
		{
			A8U( srcBuffer, uint8_t( value ) );
		}
		static void L32F( uint8_t * srcBuffer, float value )
		{
			dst = value;
		}
		static void R32F( uint8_t * srcBuffer, float value )
		{
			L32F( srcBuffer, value );
		}
		static void G32F( uint8_t * srcBuffer, float value )
		{
			L32F( srcBuffer, value );
		}
		static void B32F( uint8_t * srcBuffer, float value )
		{
			L32F( srcBuffer, value );
		}
		static void A32F( uint8_t * srcBuffer, float value )
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
		static uint8_t	BIT( uint8_t const  p_bySrc )
		{
			return ( ( p_bySrc >> 0 ) & 0x01 ) | ( ( p_bySrc >> 1 ) & 0x01 ) | ( ( p_bySrc >> 2 ) & 0x01 ) | ( ( p_bySrc >> 3 ) & 0x01 ) | ( ( p_bySrc >> 4 ) & 0x01 ) | ( ( p_bySrc >> 5 ) & 0x01 ) | ( ( p_bySrc >> 6 ) & 0x01 ) | ( ( p_bySrc >> 7 ) & 0x01 );
		}
		static uint16_t D16( uint8_t const * )
		{
			return 0;
		}
		static uint32_t D24( uint8_t const * )
		{
			return 0;
		}
		static uint32_t D32( uint8_t const * )
		{
			return 0;
		}
		static uint8_t S1( uint8_t const * srcBuffer )
		{
			return ( BIT( srcBuffer[3] ) << 0 ) | ( BIT( srcBuffer[7] ) << 1 ) | ( BIT( srcBuffer[11] ) << 2 ) | ( BIT( srcBuffer[15] ) << 3 ) | ( BIT( srcBuffer[19] ) << 4 ) | ( BIT( srcBuffer[23] ) << 5 ) | ( BIT( srcBuffer[27] ) << 6 ) | ( BIT( srcBuffer[31] ) << 7 );
		}
		static uint8_t S8( uint8_t const * srcBuffer )
		{
			return *srcBuffer;
		}
		static float D32F( uint8_t const * )
		{
			return 0;
		}
		static float S32F( uint8_t const * srcBuffer )
		{
			return float( S8( srcBuffer ) ) / 255.0f;
		}
		static void D16( uint8_t *, uint16_t )
		{
		}
		static void D24( uint8_t *, uint32_t )
		{
		}
		static void D32( uint8_t *, uint32_t )
		{
		}
		static void S1( uint8_t * srcBuffer, uint8_t value )
		{
		}
		static void S8( uint8_t * srcBuffer, uint8_t value )
		{
			*srcBuffer = value;
		}
		static void D32F( uint8_t *, float )
		{
		}
		static void S32F( uint8_t * srcBuffer, float value )
		{
			S8( srcBuffer, uint8_t( value * 255.0f ) );
		}
		static uint8_t L8U( uint8_t const * srcBuffer )
		{
			return 0;
		}
		static uint8_t A8U( uint8_t const * srcBuffer )
		{
			return S8( srcBuffer );
		}
		static uint8_t R8U( uint8_t const * srcBuffer )
		{
			return L8U( srcBuffer );
		}
		static uint8_t G8U( uint8_t const * srcBuffer )
		{
			return L8U( srcBuffer );
		}
		static uint8_t B8U( uint8_t const * srcBuffer )
		{
			return L8U( srcBuffer );
		}
		static int8_t L8S( uint8_t const * srcBuffer )
		{
			return int8_t( L8U( srcBuffer ) );
		}
		static int8_t R8S( uint8_t const * srcBuffer )
		{
			return int8_t( R8U( srcBuffer ) );
		}
		static int8_t G8S( uint8_t const * srcBuffer )
		{
			return int8_t( G8U( srcBuffer ) );
		}
		static int8_t B8S( uint8_t const * srcBuffer )
		{
			return int8_t( B8U( srcBuffer ) );
		}
		static int8_t A8S( uint8_t const * srcBuffer )
		{
			return int8_t( A8U( srcBuffer ) );
		}
		static float L32F( uint8_t const * srcBuffer )
		{
			return L8U( srcBuffer ) / 255.0f;
		}
		static float R32F( uint8_t const * srcBuffer )
		{
			return R8U( srcBuffer ) / 255.0f;
		}
		static float G32F( uint8_t const * srcBuffer )
		{
			return G8U( srcBuffer ) / 255.0f;
		}
		static float B32F( uint8_t const * srcBuffer )
		{
			return B8U( srcBuffer ) / 255.0f;
		}
		static float A32F( uint8_t const * srcBuffer )
		{
			return A8U( srcBuffer ) / 255.0f;
		}
		static void L8U( uint8_t * srcBuffer, uint8_t value )
		{
		}
		static void A8U( uint8_t * srcBuffer, uint8_t value )
		{
			S8( srcBuffer, value );
		}
		static void R8U( uint8_t * srcBuffer, uint8_t value )
		{
			L8U( srcBuffer, value );
		}
		static void G8U( uint8_t * srcBuffer, uint8_t value )
		{
			L8U( srcBuffer, value );
		}
		static void B8U( uint8_t * srcBuffer, uint8_t value )
		{
			L8U( srcBuffer, value );
		}
		static void L8S( uint8_t * srcBuffer, int8_t value )
		{
			L8U( srcBuffer, uint8_t( value ) );
		}
		static void R8S( uint8_t * srcBuffer, int8_t value )
		{
			R8U( srcBuffer, uint8_t( value ) );
		}
		static void G8S( uint8_t * srcBuffer, int8_t value )
		{
			G8U( srcBuffer, uint8_t( value ) );
		}
		static void B8S( uint8_t * srcBuffer, int8_t value )
		{
			B8U( srcBuffer, uint8_t( value ) );
		}
		static void A8S( uint8_t * srcBuffer, uint8_t value )
		{
			A8U( srcBuffer, uint8_t( value ) );
		}
		static void L32F( uint8_t * srcBuffer, float value )
		{
			L8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void R32F( uint8_t * srcBuffer, float value )
		{
			R8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void G32F( uint8_t * srcBuffer, float value )
		{
			G8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void B32F( uint8_t * srcBuffer, float value )
		{
			B8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
		static void A32F( uint8_t * srcBuffer, float value )
		{
			A8U( srcBuffer, uint8_t( value * 255.0 ) );
		}
	};

	//*************************************************************************************************
}
