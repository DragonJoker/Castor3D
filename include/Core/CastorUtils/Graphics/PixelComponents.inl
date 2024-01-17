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

#pragma warning( push )
#pragma warning( disable: 4756 )

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"

namespace castor
{
	//*************************************************************************************************

	//!\~english	Specialisation for PixelFormat::eR4G4_UNORM
	//!\~french		Spécialisation pour PixelFormat::eR4G4_UNORM
	template<>
	struct PixelComponentsT< PixelFormat::eR4G4_UNORM >
	{
		using Type = uint8_t;
		static PixelComponents constexpr Components = ( PixelComponent::eRed
			| PixelComponent::eGreen );
		struct Pixel
		{
			uint8_t r : 4;
			uint8_t g : 4;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type R( uint8_t const * buffer )
		{
			return Type( getSrc( buffer ).r << 4 );
		}
		static Type G( uint8_t const * buffer )
		{
			return Type( getSrc( buffer ).g << 4 );
		}
		static Type B( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return 0x00u;
		}
		static Type A( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return std::numeric_limits< Type >::max();
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = uint8_t( value >> 4 );
		}
		static void G( uint8_t * buffer, Type value )
		{
			getDst( buffer ).g = uint8_t( value >> 4 );
		}
		static void B( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
		static void A( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR4G4B4A4_UNORM
	//!\~french		Spécialisation pour PixelFormat::eR4G4B4A4_UNORM
	template<>
	struct PixelComponentsT< PixelFormat::eR4G4B4A4_UNORM >
	{
		using Type = uint8_t;
		static PixelComponents constexpr Components = ( PixelComponent::eRed
			| PixelComponent::eGreen
			| PixelComponent::eBlue
			| PixelComponent::eAlpha );
		struct Pixel
		{
			uint16_t r : 4;
			uint16_t g : 4;
			uint16_t b : 4;
			uint16_t a : 4;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type R( uint8_t const * buffer )
		{
			return Type( getSrc( buffer ).r << 4 );
		}
		static Type G( uint8_t const * buffer )
		{
			return Type( getSrc( buffer ).g << 4 );
		}
		static Type B( uint8_t const * buffer )
		{
			return Type( getSrc( buffer ).b << 4 );
		}
		static Type A( uint8_t const * buffer )
		{
			return Type( getSrc( buffer ).a << 4 );
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = uint16_t( value >> 4 );
		}
		static void G( uint8_t * buffer, Type value )
		{
			getDst( buffer ).g = uint16_t( value >> 4 );
		}
		static void B( uint8_t * buffer, Type value )
		{
			getDst( buffer ).b = uint16_t( value >> 4 );
		}
		static void A( uint8_t * buffer, Type value )
		{
			getDst( buffer ).a = uint16_t( value >> 4 );
		}
	};
	//!\~english	Specialisation for PixelFormat::eB4G4R4A4_UNORM
	//!\~french		Spécialisation pour PixelFormat::eB4G4R4A4_UNORM
	template<>
	struct PixelComponentsT< PixelFormat::eB4G4R4A4_UNORM >
	{
		using Type = uint8_t;
		static PixelComponents constexpr Components = ( PixelComponent::eRed
			| PixelComponent::eGreen
			| PixelComponent::eBlue
			| PixelComponent::eAlpha );
		struct Pixel
		{
			uint16_t b : 4;
			uint16_t g : 4;
			uint16_t r : 4;
			uint16_t a : 4;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type R( uint8_t const * buffer )
		{
			return Type( getSrc( buffer ).r << 4 );
		}
		static Type G( uint8_t const * buffer )
		{
			return Type( getSrc( buffer ).g << 4 );
		}
		static Type B( uint8_t const * buffer )
		{
			return Type( getSrc( buffer ).b << 4 );
		}
		static Type A( uint8_t const * buffer )
		{
			return Type( getSrc( buffer ).a << 4 );
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = uint16_t( value >> 4 );
		}
		static void G( uint8_t * buffer, Type value )
		{
			getDst( buffer ).g = uint16_t( value >> 4 );
		}
		static void B( uint8_t * buffer, Type value )
		{
			getDst( buffer ).b = uint16_t( value >> 4 );
		}
		static void A( uint8_t * buffer, Type value )
		{
			getDst( buffer ).a = uint16_t( value >> 4 );
		}
	};
	//!\~english	Specialisation for PixelFormat::eR5G6B5_UNORM
	//!\~french		Spécialisation pour PixelFormat::eR5G6B5_UNORM
	template<>
	struct PixelComponentsT< PixelFormat::eR5G6B5_UNORM >
	{
		using Type = uint8_t;
		static PixelComponents constexpr Components = ( PixelComponent::eRed
			| PixelComponent::eGreen
			| PixelComponent::eBlue );
		struct Pixel
		{
			uint16_t r : 5;
			uint16_t g : 6;
			uint16_t b : 5;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type R( uint8_t const * buffer )
		{
			return Type( getSrc( buffer ).r << 3 );
		}
		static Type G( uint8_t const * buffer )
		{
			return Type( getSrc( buffer ).g << 2 );
		}
		static Type B( uint8_t const * buffer )
		{
			return Type( getSrc( buffer ).b << 3 );
		}
		static Type A( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return std::numeric_limits< Type >::max();
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = uint16_t( value >> 3 );
		}
		static void G( uint8_t * buffer, Type value )
		{
			getDst( buffer ).g = uint16_t( value >> 2 );
		}
		static void B( uint8_t * buffer, Type value )
		{
			getDst( buffer ).b = uint16_t( value >> 3 );
		}
		static void A( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eB5G6R5_UNORM
	//!\~french		Spécialisation pour PixelFormat::eB5G6R5_UNORM
	template<>
	struct PixelComponentsT< PixelFormat::eB5G6R5_UNORM >
	{
		using Type = uint8_t;
		static PixelComponents constexpr Components = ( PixelComponent::eRed
			| PixelComponent::eGreen
			| PixelComponent::eBlue );
		struct Pixel
		{
			uint16_t b : 5;
			uint16_t g : 6;
			uint16_t r : 5;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type R( uint8_t const * buffer )
		{
			return Type( getSrc( buffer ).r << 3 );
		}
		static Type G( uint8_t const * buffer )
		{
			return Type( getSrc( buffer ).g << 2 );
		}
		static Type B( uint8_t const * buffer )
		{
			return Type( getSrc( buffer ).b << 3 );
		}
		static Type A( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return 0xFF;
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = uint16_t( value >> 3 );
		}
		static void G( uint8_t * buffer, Type value )
		{
			getDst( buffer ).g = uint16_t( value >> 2 );
		}
		static void B( uint8_t * buffer, Type value )
		{
			getDst( buffer ).b = uint16_t( value >> 3 );
		}
		static void A( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR5G5B5A1_UNORM
	//!\~french		Spécialisation pour PixelFormat::eR5G5B5A1_UNORM
	template<>
	struct PixelComponentsT< PixelFormat::eR5G5B5A1_UNORM >
	{
		using Type = uint8_t;
		static PixelComponents constexpr Components = ( PixelComponent::eRed
			| PixelComponent::eGreen
			| PixelComponent::eBlue
			| PixelComponent::eAlpha );
		struct Pixel
		{
			uint16_t r : 5;
			uint16_t g : 5;
			uint16_t b : 5;
			uint16_t a : 1;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type R( uint8_t const * buffer )
		{
			return Type( getSrc( buffer ).r << 3 );
		}
		static Type G( uint8_t const * buffer )
		{
			return Type( getSrc( buffer ).g << 3 );
		}
		static Type B( uint8_t const * buffer )
		{
			return Type( getSrc( buffer ).b << 3 );
		}
		static Type A( uint8_t const * buffer )
		{
			return Type( getSrc( buffer ).a << 7 );
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = uint16_t( value >> 3 );
		}
		static void G( uint8_t * buffer, Type value )
		{
			getDst( buffer ).g = uint16_t( value >> 3 );
		}
		static void B( uint8_t * buffer, Type value )
		{
			getDst( buffer ).b = uint16_t( value >> 3 );
		}
		static void A( uint8_t * buffer, Type value )
		{
			getDst( buffer ).a = uint16_t( ( value != 0 ) ? 0x01 : 0x00 );
		}
	};
	//!\~english	Specialisation for PixelFormat::eB5G5R5A1_UNORM
	//!\~french		Spécialisation pour PixelFormat::eB5G5R5A1_UNORM
	template<>
	struct PixelComponentsT< PixelFormat::eB5G5R5A1_UNORM >
	{
		using Type = uint8_t;
		static PixelComponents constexpr Components = ( PixelComponent::eRed
			| PixelComponent::eGreen
			| PixelComponent::eBlue
			| PixelComponent::eAlpha );
		struct Pixel
		{
			uint16_t b : 5;
			uint16_t g : 5;
			uint16_t r : 5;
			uint16_t a : 1;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type R( uint8_t const * buffer )
		{
			return Type( getSrc( buffer ).r << 3 );
		}
		static Type G( uint8_t const * buffer )
		{
			return Type( getSrc( buffer ).g << 3 );
		}
		static Type B( uint8_t const * buffer )
		{
			return Type( getSrc( buffer ).b << 3 );
		}
		static Type A( uint8_t const * buffer )
		{
			return Type( getSrc( buffer ).a << 7 );
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = uint16_t( value >> 3 );
		}
		static void G( uint8_t * buffer, Type value )
		{
			getDst( buffer ).g = uint16_t( value >> 3 );
		}
		static void B( uint8_t * buffer, Type value )
		{
			getDst( buffer ).b = uint16_t( value >> 3 );
		}
		static void A( uint8_t * buffer, Type value )
		{
			getDst( buffer ).a = uint16_t( ( value != 0 ) ? 0x01 : 0x00 );
		}
	};
	//!\~english	Specialisation for PixelFormat::eA1R5G5B5_UNORM
	//!\~french		Spécialisation pour PixelFormat::eA1R5G5B5_UNORM
	template<>
	struct PixelComponentsT< PixelFormat::eA1R5G5B5_UNORM >
	{
		using Type = uint8_t;
		static PixelComponents constexpr Components = ( PixelComponent::eRed
			| PixelComponent::eGreen
			| PixelComponent::eBlue
			| PixelComponent::eAlpha );
		struct Pixel
		{
			uint16_t a : 1;
			uint16_t r : 5;
			uint16_t g : 5;
			uint16_t b : 5;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type R( uint8_t const * buffer )
		{
			return Type( getSrc( buffer ).r << 3 );
		}
		static Type G( uint8_t const * buffer )
		{
			return Type( getSrc( buffer ).g << 3 );
		}
		static Type B( uint8_t const * buffer )
		{
			return Type( getSrc( buffer ).b << 3 );
		}
		static Type A( uint8_t const * buffer )
		{
			return Type( getSrc( buffer ).a << 7 );
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = uint16_t( value >> 3 );
		}
		static void G( uint8_t * buffer, Type value )
		{
			getDst( buffer ).g = uint16_t( value >> 3 );
		}
		static void B( uint8_t * buffer, Type value )
		{
			getDst( buffer ).b = uint16_t( value >> 3 );
		}
		static void A( uint8_t * buffer, Type value )
		{
			getDst( buffer ).a = uint16_t( ( value != 0 ) ? 0x01 : 0x00 );
		}
	};
	//!\~english	Specialisation for PixelFormat::eR8_UNORM
	//!\~french		Spécialisation pour PixelFormat::eR8_UNORM
	template<>
	struct PixelComponentsT< PixelFormat::eR8_UNORM >
	{
		using Type = uint8_t;
		static PixelComponents constexpr Components = ( PixelComponent::eRed );
		struct Pixel
		{
			int8_t r;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type R( uint8_t const * buffer )
		{
			return Type( getSrc( buffer ).r );
		}
		static Type G( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return 0x00;
		}
		static Type B( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return 0x00;
		}
		static Type A( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return std::numeric_limits< Type >::max();
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = int8_t( value );
		}
		static void G( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
		static void B( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
		static void A( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR8_SNORM
	//!\~french		Spécialisation pour PixelFormat::eR8_SNORM
	template<>
	struct PixelComponentsT< PixelFormat::eR8_SNORM >
	{
		using Type = int8_t;
		static PixelComponents constexpr Components = ( PixelComponent::eRed );
		struct Pixel
		{
			int8_t r;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type R( uint8_t const * buffer )
		{
			return getSrc( buffer ).r;
		}
		static Type G( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return 0x00;
		}
		static Type B( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return 0x00;
		}
		static Type A( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return std::numeric_limits< Type >::max();
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value;
		}
		static void G( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
		static void B( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
		static void A( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR8_USCALED
	//!\~french		Spécialisation pour PixelFormat::eR8_USCALED
	template<>
	struct PixelComponentsT< PixelFormat::eR8_USCALED >
		: public PixelComponentsT< PixelFormat::eR8_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR8_SSCALED
	//!\~french		Spécialisation pour PixelFormat::eR8_SSCALED
	template<>
	struct PixelComponentsT< PixelFormat::eR8_SSCALED >
		: public PixelComponentsT< PixelFormat::eR8_SNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR8_UINT
	//!\~french		Spécialisation pour PixelFormat::eR8_UINT
	template<>
	struct PixelComponentsT< PixelFormat::eR8_UINT >
		: public PixelComponentsT< PixelFormat::eR8_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR8_SINT
	//!\~french		Spécialisation pour PixelFormat::eR8_SINT
	template<>
	struct PixelComponentsT< PixelFormat::eR8_SINT >
		: public PixelComponentsT< PixelFormat::eR8_SNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR8_SRGB
	//!\~french		Spécialisation pour PixelFormat::eR8_SRGB
	template<>
	struct PixelComponentsT< PixelFormat::eR8_SRGB >
		: public PixelComponentsT< PixelFormat::eR8_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR8G8_UNORM
	//!\~french		Spécialisation pour PixelFormat::eR8G8_UNORM
	template<>
	struct PixelComponentsT< PixelFormat::eR8G8_UNORM >
	{
		using Type = uint8_t;
		static PixelComponents constexpr Components = ( PixelComponent::eRed
			| PixelComponent::eGreen );
		struct Pixel
		{
			uint8_t r;
			uint8_t g;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type R( uint8_t const * buffer )
		{
			return getSrc( buffer ).r;
		}
		static Type G( uint8_t const * buffer )
		{
			return getSrc( buffer ).g;
		}
		static Type B( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return 0x00;
		}
		static Type A( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return std::numeric_limits< Type >::max();
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value;
		}
		static void G( uint8_t * buffer, Type value )
		{
			getDst( buffer ).g = value;
		}
		static void B( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
		static void A( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR8G8_SNORM
	//!\~french		Spécialisation pour PixelFormat::eR8G8_SNORM
	template<>
	struct PixelComponentsT< PixelFormat::eR8G8_SNORM >
	{
		using Type = int8_t;
		static PixelComponents constexpr Components = ( PixelComponent::eRed
			| PixelComponent::eGreen );
		struct Pixel
		{
			int8_t r;
			int8_t g;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type R( uint8_t const * buffer )
		{
			return getSrc( buffer ).r;
		}
		static Type G( uint8_t const * buffer )
		{
			return getSrc( buffer ).g;
		}
		static Type B( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return 0x00;
		}
		static Type A( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return std::numeric_limits< Type >::max();
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value;
		}
		static void G( uint8_t * buffer, Type value )
		{
			getDst( buffer ).g = value;
		}
		static void B( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
		static void A( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR8G8_USCALED
	//!\~french		Spécialisation pour PixelFormat::eR8G8_USCALED
	template<>
	struct PixelComponentsT< PixelFormat::eR8G8_USCALED >
		: public PixelComponentsT< PixelFormat::eR8G8_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR8G8_SSCALED
	//!\~french		Spécialisation pour PixelFormat::eR8G8_SSCALED
	template<>
	struct PixelComponentsT< PixelFormat::eR8G8_SSCALED >
		: public PixelComponentsT< PixelFormat::eR8G8_SNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR8G8_UINT
	//!\~french		Spécialisation pour PixelFormat::eR8G8_UINT
	template<>
	struct PixelComponentsT< PixelFormat::eR8G8_UINT >
		: public PixelComponentsT< PixelFormat::eR8G8_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR8G8_SINT
	//!\~french		Spécialisation pour PixelFormat::eR8G8_SINT
	template<>
	struct PixelComponentsT< PixelFormat::eR8G8_SINT >
		: public PixelComponentsT< PixelFormat::eR8G8_SNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR8G8_SRGB
	//!\~french		Spécialisation pour PixelFormat::eR8G8_SRGB
	template<>
	struct PixelComponentsT< PixelFormat::eR8G8_SRGB >
		: public PixelComponentsT< PixelFormat::eR8G8_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR8G8B8_UNORM
	//!\~french		Spécialisation pour PixelFormat::eR8G8B8_UNORM
	template<>
	struct PixelComponentsT< PixelFormat::eR8G8B8_UNORM >
	{
		using Type = uint8_t;
		static PixelComponents constexpr Components = ( PixelComponent::eRed
			| PixelComponent::eGreen
			| PixelComponent::eBlue );
		struct Pixel
		{
			uint8_t r;
			uint8_t g;
			uint8_t b;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type R( uint8_t const * buffer )
		{
			return getSrc( buffer ).r;
		}
		static Type G( uint8_t const * buffer )
		{
			return getSrc( buffer ).g;
		}
		static Type B( uint8_t const * buffer )
		{
			return getSrc( buffer ).b;
		}
		static Type A( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return std::numeric_limits< Type >::max();
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value;
		}
		static void G( uint8_t * buffer, Type value )
		{
			getDst( buffer ).g = value;
		}
		static void B( uint8_t * buffer, Type value )
		{
			getDst( buffer ).b = value;
		}
		static void A( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR8G8B8_SNORM
	//!\~french		Spécialisation pour PixelFormat::eR8G8B8_SNORM
	template<>
	struct PixelComponentsT< PixelFormat::eR8G8B8_SNORM >
	{
		using Type = int8_t;
		static PixelComponents constexpr Components = ( PixelComponent::eRed
			| PixelComponent::eGreen
			| PixelComponent::eBlue );
		struct Pixel
		{
			int8_t r;
			int8_t g;
			int8_t b;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type R( uint8_t const * buffer )
		{
			return getSrc( buffer ).r;
		}
		static Type G( uint8_t const * buffer )
		{
			return getSrc( buffer ).g;
		}
		static Type B( uint8_t const * buffer )
		{
			return getSrc( buffer ).b;
		}
		static Type A( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return std::numeric_limits< Type >::max();
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value;
		}
		static void G( uint8_t * buffer, Type value )
		{
			getDst( buffer ).g = value;
		}
		static void B( uint8_t * buffer, Type value )
		{
			getDst( buffer ).b = value;
		}
		static void A( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR8G8B8_USCALED
	//!\~french		Spécialisation pour PixelFormat::eR8G8B8_USCALED
	template<>
	struct PixelComponentsT< PixelFormat::eR8G8B8_USCALED >
		: public PixelComponentsT< PixelFormat::eR8G8B8_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR8G8B8_SSCALED
	//!\~french		Spécialisation pour PixelFormat::eR8G8B8_SSCALED
	template<>
	struct PixelComponentsT< PixelFormat::eR8G8B8_SSCALED >
		: public PixelComponentsT< PixelFormat::eR8G8B8_SNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR8G8B8_UINT
	//!\~french		Spécialisation pour PixelFormat::eR8G8B8_UINT
	template<>
	struct PixelComponentsT< PixelFormat::eR8G8B8_UINT >
		: public PixelComponentsT< PixelFormat::eR8G8B8_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR8G8B8_SINT
	//!\~french		Spécialisation pour PixelFormat::eR8G8B8_SINT
	template<>
	struct PixelComponentsT< PixelFormat::eR8G8B8_SINT >
		: public PixelComponentsT< PixelFormat::eR8G8B8_SNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR8G8B8_SRGB
	//!\~french		Spécialisation pour PixelFormat::eR8G8B8_SRGB
	template<>
	struct PixelComponentsT< PixelFormat::eR8G8B8_SRGB >
		: public PixelComponentsT< PixelFormat::eR8G8B8_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eB8G8R8_UNORM
	//!\~french		Spécialisation pour PixelFormat::eB8G8R8_UNORM
	template<>
	struct PixelComponentsT< PixelFormat::eB8G8R8_UNORM >
	{
		using Type = uint8_t;
		static PixelComponents constexpr Components = ( PixelComponent::eRed
			| PixelComponent::eGreen
			| PixelComponent::eBlue );
		struct Pixel
		{
			uint8_t b;
			uint8_t g;
			uint8_t r;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type R( uint8_t const * buffer )
		{
			return getSrc( buffer ).r;
		}
		static Type G( uint8_t const * buffer )
		{
			return getSrc( buffer ).g;
		}
		static Type B( uint8_t const * buffer )
		{
			return getSrc( buffer ).b;
		}
		static Type A( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return std::numeric_limits< Type >::max();
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value;
		}
		static void G( uint8_t * buffer, Type value )
		{
			getDst( buffer ).g = value;
		}
		static void B( uint8_t * buffer, Type value )
		{
			getDst( buffer ).b = value;
		}
		static void A( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eB8G8R8_SNORM
	//!\~french		Spécialisation pour PixelFormat::eB8G8R8_SNORM
	template<>
	struct PixelComponentsT< PixelFormat::eB8G8R8_SNORM >
	{
		using Type = int8_t;
		static PixelComponents constexpr Components = ( PixelComponent::eRed
			| PixelComponent::eGreen
			| PixelComponent::eBlue );
		struct Pixel
		{
			int8_t b;
			int8_t g;
			int8_t r;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type R( uint8_t const * buffer )
		{
			return getSrc( buffer ).r;
		}
		static Type G( uint8_t const * buffer )
		{
			return getSrc( buffer ).g;
		}
		static Type B( uint8_t const * buffer )
		{
			return getSrc( buffer ).b;
		}
		static Type A( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return std::numeric_limits< Type >::max();
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value;
		}
		static void G( uint8_t * buffer, Type value )
		{
			getDst( buffer ).g = value;
		}
		static void B( uint8_t * buffer, Type value )
		{
			getDst( buffer ).b = value;
		}
		static void A( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eB8G8R8_USCALED
	//!\~french		Spécialisation pour PixelFormat::eB8G8R8_USCALED
	template<>
	struct PixelComponentsT< PixelFormat::eB8G8R8_USCALED >
		: public PixelComponentsT< PixelFormat::eB8G8R8_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eB8G8R8_SSCALED
	//!\~french		Spécialisation pour PixelFormat::eB8G8R8_SSCALED
	template<>
	struct PixelComponentsT< PixelFormat::eB8G8R8_SSCALED >
		: public PixelComponentsT< PixelFormat::eB8G8R8_SNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eB8G8R8_UINT
	//!\~french		Spécialisation pour PixelFormat::eB8G8R8_UINT
	template<>
	struct PixelComponentsT< PixelFormat::eB8G8R8_UINT >
		: public PixelComponentsT< PixelFormat::eB8G8R8_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eB8G8R8_SINT
	//!\~french		Spécialisation pour PixelFormat::eB8G8R8_SINT
	template<>
	struct PixelComponentsT< PixelFormat::eB8G8R8_SINT >
		: public PixelComponentsT< PixelFormat::eB8G8R8_SNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eB8G8R8_SRGB
	//!\~french		Spécialisation pour PixelFormat::eB8G8R8_SRGB
	template<>
	struct PixelComponentsT< PixelFormat::eB8G8R8_SRGB >
		: public PixelComponentsT< PixelFormat::eB8G8R8_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR8G8B8A8_UNORM
	//!\~french		Spécialisation pour PixelFormat::eR8G8B8A8_UNORM
	template<>
	struct PixelComponentsT< PixelFormat::eR8G8B8A8_UNORM >
	{
		using Type = uint8_t;
		static PixelComponents constexpr Components = ( PixelComponent::eRed
			| PixelComponent::eGreen
			| PixelComponent::eBlue
			| PixelComponent::eAlpha );
		struct Pixel
		{
			uint8_t r;
			uint8_t g;
			uint8_t b;
			uint8_t a;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type R( uint8_t const * buffer )
		{
			return getSrc( buffer ).r;
		}
		static Type G( uint8_t const * buffer )
		{
			return getSrc( buffer ).g;
		}
		static Type B( uint8_t const * buffer )
		{
			return getSrc( buffer ).b;
		}
		static Type A( uint8_t const * buffer )
		{
			return getSrc( buffer ).a;
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value;
		}
		static void G( uint8_t * buffer, Type value )
		{
			getDst( buffer ).g = value;
		}
		static void B( uint8_t * buffer, Type value )
		{
			getDst( buffer ).b = value;
		}
		static void A( uint8_t * buffer, Type value )
		{
			getDst( buffer ).a = value;
		}
	};
	//!\~english	Specialisation for PixelFormat::eR8G8B8A8_SNORM
	//!\~french		Spécialisation pour PixelFormat::eR8G8B8A8_SNORM
	template<>
	struct PixelComponentsT< PixelFormat::eR8G8B8A8_SNORM >
	{
		using Type = int8_t;
		static PixelComponents constexpr Components = ( PixelComponent::eRed
			| PixelComponent::eGreen
			| PixelComponent::eBlue
			| PixelComponent::eAlpha );
		struct Pixel
		{
			int8_t r;
			int8_t g;
			int8_t b;
			int8_t a;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type R( uint8_t const * buffer )
		{
			return getSrc( buffer ).r;
		}
		static Type G( uint8_t const * buffer )
		{
			return getSrc( buffer ).g;
		}
		static Type B( uint8_t const * buffer )
		{
			return getSrc( buffer ).b;
		}
		static Type A( uint8_t const * buffer )
		{
			return getSrc( buffer ).a;
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value;
		}
		static void G( uint8_t * buffer, Type value )
		{
			getDst( buffer ).g = value;
		}
		static void B( uint8_t * buffer, Type value )
		{
			getDst( buffer ).b = value;
		}
		static void A( uint8_t * buffer, Type value )
		{
			getDst( buffer ).a = value;
		}
	};
	//!\~english	Specialisation for PixelFormat::eR8G8B8A8_USCALED
	//!\~french		Spécialisation pour PixelFormat::eR8G8B8A8_USCALED
	template<>
	struct PixelComponentsT< PixelFormat::eR8G8B8A8_USCALED >
		: public PixelComponentsT< PixelFormat::eR8G8B8A8_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR8G8B8A8_SSCALED
	//!\~french		Spécialisation pour PixelFormat::eR8G8B8A8_SSCALED
	template<>
	struct PixelComponentsT< PixelFormat::eR8G8B8A8_SSCALED >
		: public PixelComponentsT< PixelFormat::eR8G8B8A8_SNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR8G8B8A8_UINT
	//!\~french		Spécialisation pour PixelFormat::eR8G8B8A8_UINT
	template<>
	struct PixelComponentsT< PixelFormat::eR8G8B8A8_UINT >
		: public PixelComponentsT< PixelFormat::eR8G8B8A8_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR8G8B8A8_SINT
	//!\~french		Spécialisation pour PixelFormat::eR8G8B8A8_SINT
	template<>
	struct PixelComponentsT< PixelFormat::eR8G8B8A8_SINT >
		: public PixelComponentsT< PixelFormat::eR8G8B8A8_SNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR8G8B8A8_SRGB
	//!\~french		Spécialisation pour PixelFormat::eR8G8B8A8_SRGB
	template<>
	struct PixelComponentsT< PixelFormat::eR8G8B8A8_SRGB >
		: public PixelComponentsT< PixelFormat::eR8G8B8A8_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eB8G8R8A8_UNORM
	//!\~french		Spécialisation pour PixelFormat::eB8G8R8A8_UNORM
	template<>
	struct PixelComponentsT< PixelFormat::eB8G8R8A8_UNORM >
	{
		using Type = uint8_t;
		static PixelComponents constexpr Components = ( PixelComponent::eRed
			| PixelComponent::eGreen
			| PixelComponent::eBlue
			| PixelComponent::eAlpha );
		struct Pixel
		{
			uint8_t b;
			uint8_t g;
			uint8_t r;
			uint8_t a;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type R( uint8_t const * buffer )
		{
			return getSrc( buffer ).r;
		}
		static Type G( uint8_t const * buffer )
		{
			return getSrc( buffer ).g;
		}
		static Type B( uint8_t const * buffer )
		{
			return getSrc( buffer ).b;
		}
		static Type A( uint8_t const * buffer )
		{
			return getSrc( buffer ).a;
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value;
		}
		static void G( uint8_t * buffer, Type value )
		{
			getDst( buffer ).g = value;
		}
		static void B( uint8_t * buffer, Type value )
		{
			getDst( buffer ).b = value;
		}
		static void A( uint8_t * buffer, Type value )
		{
			getDst( buffer ).a = value;
		}
	};
	//!\~english	Specialisation for PixelFormat::eB8G8R8A8_SNORM
	//!\~french		Spécialisation pour PixelFormat::eB8G8R8A8_SNORM
	template<>
	struct PixelComponentsT< PixelFormat::eB8G8R8A8_SNORM >
	{
		using Type = int8_t;
		static PixelComponents constexpr Components = ( PixelComponent::eRed
			| PixelComponent::eGreen
			| PixelComponent::eBlue
			| PixelComponent::eAlpha );
		struct Pixel
		{
			int8_t b;
			int8_t g;
			int8_t r;
			int8_t a;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type R( uint8_t const * buffer )
		{
			return getSrc( buffer ).r;
		}
		static Type G( uint8_t const * buffer )
		{
			return getSrc( buffer ).g;
		}
		static Type B( uint8_t const * buffer )
		{
			return getSrc( buffer ).b;
		}
		static Type A( uint8_t const * buffer )
		{
			return getSrc( buffer ).a;
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value;
		}
		static void G( uint8_t * buffer, Type value )
		{
			getDst( buffer ).g = value;
		}
		static void B( uint8_t * buffer, Type value )
		{
			getDst( buffer ).b = value;
		}
		static void A( uint8_t * buffer, Type value )
		{
			getDst( buffer ).a = value;
		}
	};
	//!\~english	Specialisation for PixelFormat::eB8G8R8A8_USCALED
	//!\~french		Spécialisation pour PixelFormat::eB8G8R8A8_USCALED
	template<>
	struct PixelComponentsT< PixelFormat::eB8G8R8A8_USCALED >
		: public PixelComponentsT< PixelFormat::eB8G8R8A8_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eB8G8R8A8_SSCALED
	//!\~french		Spécialisation pour PixelFormat::eB8G8R8A8_SSCALED
	template<>
	struct PixelComponentsT< PixelFormat::eB8G8R8A8_SSCALED >
		: public PixelComponentsT< PixelFormat::eB8G8R8A8_SNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eB8G8R8A8_UINT
	//!\~french		Spécialisation pour PixelFormat::eB8G8R8A8_UINT
	template<>
	struct PixelComponentsT< PixelFormat::eB8G8R8A8_UINT >
		: public PixelComponentsT< PixelFormat::eB8G8R8A8_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eB8G8R8A8_SINT
	//!\~french		Spécialisation pour PixelFormat::eB8G8R8A8_SINT
	template<>
	struct PixelComponentsT< PixelFormat::eB8G8R8A8_SINT >
		: public PixelComponentsT< PixelFormat::eB8G8R8A8_SNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eB8G8R8A8_SRGB
	//!\~french		Spécialisation pour PixelFormat::eB8G8R8A8_SRGB
	template<>
	struct PixelComponentsT< PixelFormat::eB8G8R8A8_SRGB >
		: public PixelComponentsT< PixelFormat::eB8G8R8A8_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eA8B8G8R8_UNORM
	//!\~french		Spécialisation pour PixelFormat::eA8B8G8R8_UNORM
	template<>
	struct PixelComponentsT< PixelFormat::eA8B8G8R8_UNORM >
	{
		using Type = uint8_t;
		static PixelComponents constexpr Components = ( PixelComponent::eRed
			| PixelComponent::eGreen
			| PixelComponent::eBlue
			| PixelComponent::eAlpha );
		struct Pixel
		{
			uint8_t a;
			uint8_t b;
			uint8_t g;
			uint8_t r;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type R( uint8_t const * buffer )
		{
			return getSrc( buffer ).r;
		}
		static Type G( uint8_t const * buffer )
		{
			return getSrc( buffer ).g;
		}
		static Type B( uint8_t const * buffer )
		{
			return getSrc( buffer ).b;
		}
		static Type A( uint8_t const * buffer )
		{
			return getSrc( buffer ).a;
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value;
		}
		static void G( uint8_t * buffer, Type value )
		{
			getDst( buffer ).g = value;
		}
		static void B( uint8_t * buffer, Type value )
		{
			getDst( buffer ).b = value;
		}
		static void A( uint8_t * buffer, Type value )
		{
			getDst( buffer ).a = value;
		}
	};
	//!\~english	Specialisation for PixelFormat::eA8B8G8R8_SNORM
	//!\~french		Spécialisation pour PixelFormat::eA8B8G8R8_SNORM
	template<>
	struct PixelComponentsT< PixelFormat::eA8B8G8R8_SNORM >
	{
		using Type = int8_t;
		static PixelComponents constexpr Components = ( PixelComponent::eRed
			| PixelComponent::eGreen
			| PixelComponent::eBlue
			| PixelComponent::eAlpha );
		struct Pixel
		{
			int8_t a;
			int8_t b;
			int8_t g;
			int8_t r;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type R( uint8_t const * buffer )
		{
			return getSrc( buffer ).r;
		}
		static Type G( uint8_t const * buffer )
		{
			return getSrc( buffer ).g;
		}
		static Type B( uint8_t const * buffer )
		{
			return getSrc( buffer ).b;
		}
		static Type A( uint8_t const * buffer )
		{
			return getSrc( buffer ).a;
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value;
		}
		static void G( uint8_t * buffer, Type value )
		{
			getDst( buffer ).g = value;
		}
		static void B( uint8_t * buffer, Type value )
		{
			getDst( buffer ).b = value;
		}
		static void A( uint8_t * buffer, Type value )
		{
			getDst( buffer ).a = value;
		}
	};
	//!\~english	Specialisation for PixelFormat::eA8B8G8R8_USCALED
	//!\~french		Spécialisation pour PixelFormat::eA8B8G8R8_USCALED
	template<>
	struct PixelComponentsT< PixelFormat::eA8B8G8R8_USCALED >
		: public PixelComponentsT< PixelFormat::eA8B8G8R8_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eA8B8G8R8_SSCALED
	//!\~french		Spécialisation pour PixelFormat::eA8B8G8R8_SSCALED
	template<>
	struct PixelComponentsT< PixelFormat::eA8B8G8R8_SSCALED >
		: public PixelComponentsT< PixelFormat::eA8B8G8R8_SNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eA8B8G8R8_UINT
	//!\~french		Spécialisation pour PixelFormat::eA8B8G8R8_UINT
	template<>
	struct PixelComponentsT< PixelFormat::eA8B8G8R8_UINT >
		: public PixelComponentsT< PixelFormat::eA8B8G8R8_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eA8B8G8R8_SINT
	//!\~french		Spécialisation pour PixelFormat::eA8B8G8R8_SINT
	template<>
	struct PixelComponentsT< PixelFormat::eA8B8G8R8_SINT >
		: public PixelComponentsT< PixelFormat::eA8B8G8R8_SNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eA8B8G8R8_SRGB
	//!\~french		Spécialisation pour PixelFormat::eA8B8G8R8_SRGB
	template<>
	struct PixelComponentsT< PixelFormat::eA8B8G8R8_SRGB >
		: public PixelComponentsT< PixelFormat::eA8B8G8R8_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eA2R10G10B10_UNORM
	//!\~french		Spécialisation pour PixelFormat::eA2R10G10B10_UNORM
	template<>
	struct PixelComponentsT< PixelFormat::eA2R10G10B10_UNORM >
	{
		using Type = uint16_t;
		static PixelComponents constexpr Components = ( PixelComponent::eRed
			| PixelComponent::eGreen
			| PixelComponent::eBlue
			| PixelComponent::eAlpha );
		struct Pixel
		{
			uint32_t a : 2;
			uint32_t r : 10;
			uint32_t g : 10;
			uint32_t b : 10;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type R( uint8_t const * buffer )
		{
			return Type( getSrc( buffer ).r << 6 );
		}
		static Type G( uint8_t const * buffer )
		{
			return Type( getSrc( buffer ).g << 6 );
		}
		static Type B( uint8_t const * buffer )
		{
			return Type( getSrc( buffer ).b << 6 );
		}
		static Type A( uint8_t const * buffer )
		{
			return Type( getSrc( buffer ).a << 14 );
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = uint32_t( value >> 6 );
		}
		static void G( uint8_t * buffer, Type value )
		{
			getDst( buffer ).g = uint32_t( value >> 6 );
		}
		static void B( uint8_t * buffer, Type value )
		{
			getDst( buffer ).b = uint32_t( value >> 6 );
		}
		static void A( uint8_t * buffer, Type value )
		{
			getDst( buffer ).a = uint32_t( value >> 14 );
		}
	};
	//!\~english	Specialisation for PixelFormat::eA2R10G10B10_SNORM
	//!\~french		Spécialisation pour PixelFormat::eA2R10G10B10_SNORM
	template<>
	struct PixelComponentsT< PixelFormat::eA2R10G10B10_SNORM >
	{
		using Type = int16_t;
		static PixelComponents constexpr Components = ( PixelComponent::eRed
			| PixelComponent::eGreen
			| PixelComponent::eBlue
			| PixelComponent::eAlpha );
		struct Pixel
		{
			int32_t a : 2;
			int32_t r : 10;
			int32_t g : 10;
			int32_t b : 10;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type R( uint8_t const * buffer )
		{
			return Type( getSrc( buffer ).r << 6 );
		}
		static Type G( uint8_t const * buffer )
		{
			return Type( getSrc( buffer ).g << 6 );
		}
		static Type B( uint8_t const * buffer )
		{
			return Type( getSrc( buffer ).b << 6 );
		}
		static Type A( uint8_t const * buffer )
		{
			return Type( getSrc( buffer ).a << 14 );
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value >> 6;
		}
		static void G( uint8_t * buffer, Type value )
		{
			getDst( buffer ).g = value >> 6;
		}
		static void B( uint8_t * buffer, Type value )
		{
			getDst( buffer ).b = value >> 6;
		}
		static void A( uint8_t * buffer, Type value )
		{
			getDst( buffer ).a = value >> 14;
		}
	};
	//!\~english	Specialisation for PixelFormat::eA2R10G10B10_USCALED
	//!\~french		Spécialisation pour PixelFormat::eA2R10G10B10_USCALED
	template<>
	struct PixelComponentsT< PixelFormat::eA2R10G10B10_USCALED >
		: public PixelComponentsT< PixelFormat::eA2R10G10B10_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eA2R10G10B10_SSCALED
	//!\~french		Spécialisation pour PixelFormat::eA2R10G10B10_SSCALED
	template<>
	struct PixelComponentsT< PixelFormat::eA2R10G10B10_SSCALED >
		: public PixelComponentsT< PixelFormat::eA2R10G10B10_SNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eA2R10G10B10_UINT
	//!\~french		Spécialisation pour PixelFormat::eA2R10G10B10_UINT
	template<>
	struct PixelComponentsT< PixelFormat::eA2R10G10B10_UINT >
		: public PixelComponentsT< PixelFormat::eA2R10G10B10_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eA2R10G10B10_SINT
	//!\~french		Spécialisation pour PixelFormat::eA2R10G10B10_SINT
	template<>
	struct PixelComponentsT< PixelFormat::eA2R10G10B10_SINT >
		: public PixelComponentsT< PixelFormat::eA2R10G10B10_SNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eA2B10G10R10_UNORM
	//!\~french		Spécialisation pour PixelFormat::eA2B10G10R10_UNORM
	template<>
	struct PixelComponentsT< PixelFormat::eA2B10G10R10_UNORM >
	{
		using Type = uint16_t;
		static PixelComponents constexpr Components = ( PixelComponent::eRed
			| PixelComponent::eGreen
			| PixelComponent::eBlue
			| PixelComponent::eAlpha );
		struct Pixel
		{
			uint32_t a : 2;
			uint32_t b : 10;
			uint32_t g : 10;
			uint32_t r : 10;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type R( uint8_t const * buffer )
		{
			return Type( getSrc( buffer ).r << 6 );
		}
		static Type G( uint8_t const * buffer )
		{
			return Type( getSrc( buffer ).g << 6 );
		}
		static Type B( uint8_t const * buffer )
		{
			return Type( getSrc( buffer ).b << 6 );
		}
		static Type A( uint8_t const * buffer )
		{
			return Type( getSrc( buffer ).a << 14 );
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = uint32_t( value >> 6 );
		}
		static void G( uint8_t * buffer, Type value )
		{
			getDst( buffer ).g = uint32_t( value >> 6 );
		}
		static void B( uint8_t * buffer, Type value )
		{
			getDst( buffer ).b = uint32_t( value >> 6 );
		}
		static void A( uint8_t * buffer, Type value )
		{
			getDst( buffer ).a = uint32_t( value >> 14 );
		}
	};
	//!\~english	Specialisation for PixelFormat::eA2B10G10R10_SNORM
	//!\~french		Spécialisation pour PixelFormat::eA2B10G10R10_SNORM
	template<>
	struct PixelComponentsT< PixelFormat::eA2B10G10R10_SNORM >
	{
		using Type = int16_t;
		static PixelComponents constexpr Components = ( PixelComponent::eRed
			| PixelComponent::eGreen
			| PixelComponent::eBlue
			| PixelComponent::eAlpha );
		struct Pixel
		{
			int32_t a : 2;
			int32_t b : 10;
			int32_t g : 10;
			int32_t r : 10;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type R( uint8_t const * buffer )
		{
			return Type( getSrc( buffer ).r << 6 );
		}
		static Type G( uint8_t const * buffer )
		{
			return Type( getSrc( buffer ).g << 6 );
		}
		static Type B( uint8_t const * buffer )
		{
			return Type( getSrc( buffer ).b << 6 );
		}
		static Type A( uint8_t const * buffer )
		{
			return Type( getSrc( buffer ).a << 14 );
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value >> 6;
		}
		static void G( uint8_t * buffer, Type value )
		{
			getDst( buffer ).g = value >> 6;
		}
		static void B( uint8_t * buffer, Type value )
		{
			getDst( buffer ).b = value >> 6;
		}
		static void A( uint8_t * buffer, Type value )
		{
			getDst( buffer ).a = value >> 14;
		}
	};
	//!\~english	Specialisation for PixelFormat::eA2B10G10R10_USCALED
	//!\~french		Spécialisation pour PixelFormat::eA2B10G10R10_USCALED
	template<>
	struct PixelComponentsT< PixelFormat::eA2B10G10R10_USCALED >
		: public PixelComponentsT< PixelFormat::eA2B10G10R10_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eA2B10G10R10_SSCALED
	//!\~french		Spécialisation pour PixelFormat::eA2B10G10R10_SSCALED
	template<>
	struct PixelComponentsT< PixelFormat::eA2B10G10R10_SSCALED >
		: public PixelComponentsT< PixelFormat::eA2B10G10R10_SNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eA2B10G10R10_UINT
	//!\~french		Spécialisation pour PixelFormat::eA2B10G10R10_UINT
	template<>
	struct PixelComponentsT< PixelFormat::eA2B10G10R10_UINT >
		: public PixelComponentsT< PixelFormat::eA2B10G10R10_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eA2B10G10R10_SINT
	//!\~french		Spécialisation pour PixelFormat::eA2B10G10R10_SINT
	template<>
	struct PixelComponentsT< PixelFormat::eA2B10G10R10_SINT >
		: public PixelComponentsT< PixelFormat::eA2B10G10R10_SNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR16_UNORM
	//!\~french		Spécialisation pour PixelFormat::eR16_UNORM
	template<>
	struct PixelComponentsT< PixelFormat::eR16_UNORM >
	{
		using Type = uint16_t;
		static PixelComponents constexpr Components = ( PixelComponent::eRed );
		struct Pixel
		{
			uint16_t r;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type R( uint8_t const * buffer )
		{
			return getSrc( buffer ).r;
		}
		static Type G( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return 0;
		}
		static Type B( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return 0;
		}
		static Type A( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return std::numeric_limits< Type >::max();
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value;
		}
		static void G( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
		static void B( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
		static void A( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR16_SNORM
	//!\~french		Spécialisation pour PixelFormat::eR16_SNORM
	template<>
	struct PixelComponentsT< PixelFormat::eR16_SNORM >
	{
		using Type = int16_t;
		static PixelComponents constexpr Components = ( PixelComponent::eRed );
		struct Pixel
		{
			int16_t r;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type R( uint8_t const * buffer )
		{
			return getSrc( buffer ).r;
		}
		static Type G( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return 0;
		}
		static Type B( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return 0;
		}
		static Type A( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return std::numeric_limits< Type >::max();
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value;
		}
		static void G( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
		static void B( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
		static void A( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR16_USCALED
	//!\~french		Spécialisation pour PixelFormat::eR16_USCALED
	template<>
	struct PixelComponentsT< PixelFormat::eR16_USCALED >
		: public PixelComponentsT< PixelFormat::eR16_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR16_SSCALED
	//!\~french		Spécialisation pour PixelFormat::eR16_SSCALED
	template<>
	struct PixelComponentsT< PixelFormat::eR16_SSCALED >
		: public PixelComponentsT< PixelFormat::eR16_SNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR16_UINT
	//!\~french		Spécialisation pour PixelFormat::eR16_UINT
	template<>
	struct PixelComponentsT< PixelFormat::eR16_UINT >
		: public PixelComponentsT< PixelFormat::eR16_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR16_SINT
	//!\~french		Spécialisation pour PixelFormat::eR16_SINT
	template<>
	struct PixelComponentsT< PixelFormat::eR16_SINT >
		: public PixelComponentsT< PixelFormat::eR16_SNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR16_SFLOAT
	//!\~french		Spécialisation pour PixelFormat::eR16_SFLOAT
	template<>
	struct PixelComponentsT< PixelFormat::eR16_SFLOAT >
		: public PixelComponentsT< PixelFormat::eR16_SNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR16G16_UNORM
	//!\~french		Spécialisation pour PixelFormat::eR16G16_UNORM
	template<>
	struct PixelComponentsT< PixelFormat::eR16G16_UNORM >
	{
		using Type = uint16_t;
		static PixelComponents constexpr Components = ( PixelComponent::eRed
			| PixelComponent::eGreen );
		struct Pixel
		{
			uint16_t r;
			uint16_t g;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type R( uint8_t const * buffer )
		{
			return getSrc( buffer ).r;
		}
		static Type G( uint8_t const * buffer )
		{
			return getSrc( buffer ).g;
		}
		static Type B( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return 0;
		}
		static Type A( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return std::numeric_limits< Type >::max();
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value;
		}
		static void G( uint8_t * buffer, Type value )
		{
			getDst( buffer ).g = value;
		}
		static void B( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
		static void A( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR16G16_SNORM
	//!\~french		Spécialisation pour PixelFormat::eR16G16_SNORM
	template<>
	struct PixelComponentsT< PixelFormat::eR16G16_SNORM >
	{
		using Type = int16_t;
		static PixelComponents constexpr Components = ( PixelComponent::eRed
			| PixelComponent::eGreen );
		struct Pixel
		{
			int16_t r;
			int16_t g;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type R( uint8_t const * buffer )
		{
			return getSrc( buffer ).r;
		}
		static Type G( uint8_t const * buffer )
		{
			return getSrc( buffer ).g;
		}
		static Type B( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return 0;
		}
		static Type A( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return std::numeric_limits< Type >::max();
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value;
		}
		static void G( uint8_t * buffer, Type value )
		{
			getDst( buffer ).g = value;
		}
		static void B( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
		static void A( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR16G16_USCALED
	//!\~french		Spécialisation pour PixelFormat::eR16G16_USCALED
	template<>
	struct PixelComponentsT< PixelFormat::eR16G16_USCALED >
		: public PixelComponentsT< PixelFormat::eR16G16_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR16G16_SSCALED
	//!\~french		Spécialisation pour PixelFormat::eR16G16_SSCALED
	template<>
	struct PixelComponentsT< PixelFormat::eR16G16_SSCALED >
		: public PixelComponentsT< PixelFormat::eR16G16_SNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR16G16_UINT
	//!\~french		Spécialisation pour PixelFormat::eR16G16_UINT
	template<>
	struct PixelComponentsT< PixelFormat::eR16G16_UINT >
		: public PixelComponentsT< PixelFormat::eR16G16_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR16G16_SINT
	//!\~french		Spécialisation pour PixelFormat::eR16G16_SINT
	template<>
	struct PixelComponentsT< PixelFormat::eR16G16_SINT >
		: public PixelComponentsT< PixelFormat::eR16G16_SNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR16G16_SFLOAT
	//!\~french		Spécialisation pour PixelFormat::eR16G16_SFLOAT
	template<>
	struct PixelComponentsT< PixelFormat::eR16G16_SFLOAT >
		: public PixelComponentsT< PixelFormat::eR16G16_SNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR16G16B16_UNORM
	//!\~french		Spécialisation pour PixelFormat::eR16G16B16_UNORM
	template<>
	struct PixelComponentsT< PixelFormat::eR16G16B16_UNORM >
	{
		using Type = uint16_t;
		static PixelComponents constexpr Components = ( PixelComponent::eRed
			| PixelComponent::eGreen
			| PixelComponent::eBlue );
		struct Pixel
		{
			uint16_t r;
			uint16_t g;
			uint16_t b;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type R( uint8_t const * buffer )
		{
			return getSrc( buffer ).r;
		}
		static Type G( uint8_t const * buffer )
		{
			return getSrc( buffer ).g;
		}
		static Type B( uint8_t const * buffer )
		{
			return getSrc( buffer ).b;
		}
		static Type A( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return std::numeric_limits< Type >::max();
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value;
		}
		static void G( uint8_t * buffer, Type value )
		{
			getDst( buffer ).g = value;
		}
		static void B( uint8_t * buffer, Type value )
		{
			getDst( buffer ).b = value;
		}
		static void A( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR16G16B16_SNORM
	//!\~french		Spécialisation pour PixelFormat::eR16G16B16_SNORM
	template<>
	struct PixelComponentsT< PixelFormat::eR16G16B16_SNORM >
	{
		using Type = int16_t;
		static PixelComponents constexpr Components = ( PixelComponent::eRed
			| PixelComponent::eGreen
			| PixelComponent::eBlue );
		struct Pixel
		{
			int16_t r;
			int16_t g;
			int16_t b;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type R( uint8_t const * buffer )
		{
			return getSrc( buffer ).r;
		}
		static Type G( uint8_t const * buffer )
		{
			return getSrc( buffer ).g;
		}
		static Type B( uint8_t const * buffer )
		{
			return getSrc( buffer ).b;
		}
		static Type A( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return std::numeric_limits< Type >::max();
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value;
		}
		static void G( uint8_t * buffer, Type value )
		{
			getDst( buffer ).g = value;
		}
		static void B( uint8_t * buffer, Type value )
		{
			getDst( buffer ).b = value;
		}
		static void A( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR16G16B16_USCALED
	//!\~french		Spécialisation pour PixelFormat::eR16G16B16_USCALED
	template<>
	struct PixelComponentsT< PixelFormat::eR16G16B16_USCALED >
		: public PixelComponentsT< PixelFormat::eR16G16B16_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR16G16B16_SSCALED
	//!\~french		Spécialisation pour PixelFormat::eR16G16B16_SSCALED
	template<>
	struct PixelComponentsT< PixelFormat::eR16G16B16_SSCALED >
		: public PixelComponentsT< PixelFormat::eR16G16B16_SNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR16G16B16_UINT
	//!\~french		Spécialisation pour PixelFormat::eR16G16B16_UINT
	template<>
	struct PixelComponentsT< PixelFormat::eR16G16B16_UINT >
		: public PixelComponentsT< PixelFormat::eR16G16B16_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR16G16B16_SINT
	//!\~french		Spécialisation pour PixelFormat::eR16G16B16_SINT
	template<>
	struct PixelComponentsT< PixelFormat::eR16G16B16_SINT >
		: public PixelComponentsT< PixelFormat::eR16G16B16_SNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR16G16B16_SFLOAT
	//!\~french		Spécialisation pour PixelFormat::eR16G16B16_SFLOAT
	template<>
	struct PixelComponentsT< PixelFormat::eR16G16B16_SFLOAT >
		: public PixelComponentsT< PixelFormat::eR16G16B16_SNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR16G16B16A16_UNORM
	//!\~french		Spécialisation pour PixelFormat::eR16G16B16A16_UNORM
	template<>
	struct PixelComponentsT< PixelFormat::eR16G16B16A16_UNORM >
	{
		using Type = uint16_t;
		static PixelComponents constexpr Components = ( PixelComponent::eRed
			| PixelComponent::eGreen
			| PixelComponent::eBlue
			| PixelComponent::eAlpha );
		struct Pixel
		{
			uint16_t r;
			uint16_t g;
			uint16_t b;
			uint16_t a;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type R( uint8_t const * buffer )
		{
			return getSrc( buffer ).r;
		}
		static Type G( uint8_t const * buffer )
		{
			return getSrc( buffer ).g;
		}
		static Type B( uint8_t const * buffer )
		{
			return getSrc( buffer ).b;
		}
		static Type A( uint8_t const * buffer )
		{
			return getSrc( buffer ).a;
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value;
		}
		static void G( uint8_t * buffer, Type value )
		{
			getDst( buffer ).g = value;
		}
		static void B( uint8_t * buffer, Type value )
		{
			getDst( buffer ).b = value;
		}
		static void A( uint8_t * buffer, Type value )
		{
			getDst( buffer ).a = value;
		}
	};
	//!\~english	Specialisation for PixelFormat::eR16G16B16A16_SNORM
	//!\~french		Spécialisation pour PixelFormat::eR16G16B16A16_SNORM
	template<>
	struct PixelComponentsT< PixelFormat::eR16G16B16A16_SNORM >
	{
		using Type = int16_t;
		static PixelComponents constexpr Components = ( PixelComponent::eRed
			| PixelComponent::eGreen
			| PixelComponent::eBlue
			| PixelComponent::eAlpha );
		struct Pixel
		{
			int16_t r;
			int16_t g;
			int16_t b;
			int16_t a;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type R( uint8_t const * buffer )
		{
			return getSrc( buffer ).r;
		}
		static Type G( uint8_t const * buffer )
		{
			return getSrc( buffer ).g;
		}
		static Type B( uint8_t const * buffer )
		{
			return getSrc( buffer ).b;
		}
		static Type A( uint8_t const * buffer )
		{
			return getSrc( buffer ).a;
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value;
		}
		static void G( uint8_t * buffer, Type value )
		{
			getDst( buffer ).g = value;
		}
		static void B( uint8_t * buffer, Type value )
		{
			getDst( buffer ).b = value;
		}
		static void A( uint8_t * buffer, Type value )
		{
			getDst( buffer ).a = value;
		}
	};
	//!\~english	Specialisation for PixelFormat::eR16G16B16A16_USCALED
	//!\~french		Spécialisation pour PixelFormat::eR16G16B16A16_USCALED
	template<>
	struct PixelComponentsT< PixelFormat::eR16G16B16A16_USCALED >
		: public PixelComponentsT< PixelFormat::eR16G16B16A16_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR16G16B16A16_SSCALED
	//!\~french		Spécialisation pour PixelFormat::eR16G16B16A16_SSCALED
	template<>
	struct PixelComponentsT< PixelFormat::eR16G16B16A16_SSCALED >
		: public PixelComponentsT< PixelFormat::eR16G16B16A16_SNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR16G16B16A16_UINT
	//!\~french		Spécialisation pour PixelFormat::eR16G16B16A16_UINT
	template<>
	struct PixelComponentsT< PixelFormat::eR16G16B16A16_UINT >
		: public PixelComponentsT< PixelFormat::eR16G16B16A16_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR16G16B16A16_SINT
	//!\~french		Spécialisation pour PixelFormat::eR16G16B16A16_SINT
	template<>
	struct PixelComponentsT< PixelFormat::eR16G16B16A16_SINT >
		: public PixelComponentsT< PixelFormat::eR16G16B16A16_SNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR16G16B16A16_SFLOAT
	//!\~french		Spécialisation pour PixelFormat::eR16G16B16A16_SFLOAT
	template<>
	struct PixelComponentsT< PixelFormat::eR16G16B16A16_SFLOAT >
		: public PixelComponentsT< PixelFormat::eR16G16B16A16_SNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR32_UINT
	//!\~french		Spécialisation pour PixelFormat::eR32_UINT
	template<>
	struct PixelComponentsT< PixelFormat::eR32_UINT >
	{
		using Type = uint32_t;
		static PixelComponents constexpr Components = ( PixelComponent::eRed );
		struct Pixel
		{
			uint32_t r;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type R( uint8_t const * buffer )
		{
			return getSrc( buffer ).r;
		}
		static Type G( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return 0;
		}
		static Type B( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return 0;
		}
		static Type A( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return std::numeric_limits< Type >::max();
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value;
		}
		static void G( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
		static void B( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
		static void A( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR32_SINT
	//!\~french		Spécialisation pour PixelFormat::eR32_SINT
	template<>
	struct PixelComponentsT< PixelFormat::eR32_SINT >
	{
		using Type = int32_t;
		static PixelComponents constexpr Components = ( PixelComponent::eRed );
		struct Pixel
		{
			int32_t r;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type R( uint8_t const * buffer )
		{
			return getSrc( buffer ).r;
		}
		static Type G( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return 0;
		}
		static Type B( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return 0;
		}
		static Type A( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return std::numeric_limits< Type >::max();
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value;
		}
		static void G( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
		static void B( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
		static void A( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR32_SFLOAT
	//!\~french		Spécialisation pour PixelFormat::eR32_SFLOAT
	template<>
	struct PixelComponentsT< PixelFormat::eR32_SFLOAT >
	{
		using Type = float;
		static PixelComponents constexpr Components = ( PixelComponent::eRed );
		struct Pixel
		{
			float r;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type R( uint8_t const * buffer )
		{
			return getSrc( buffer ).r;
		}
		static Type G( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return 0;
		}
		static Type B( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return 0;
		}
		static Type A( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return std::numeric_limits< Type >::max();
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value;
		}
		static void G( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
		static void B( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
		static void A( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR32G32_UINT
	//!\~french		Spécialisation pour PixelFormat::eR32G32_UINT
	template<>
	struct PixelComponentsT< PixelFormat::eR32G32_UINT >
	{
		using Type = uint32_t;
		static PixelComponents constexpr Components = ( PixelComponent::eRed
			| PixelComponent::eGreen );
		struct Pixel
		{
			uint32_t r;
			uint32_t g;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type R( uint8_t const * buffer )
		{
			return getSrc( buffer ).r;
		}
		static Type G( uint8_t const * buffer )
		{
			return getSrc( buffer ).g;
		}
		static Type B( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return 0;
		}
		static Type A( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return std::numeric_limits< Type >::max();
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value;
		}
		static void G( uint8_t * buffer, Type value )
		{
			getDst( buffer ).g = value;
		}
		static void B( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
		static void A( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR32G32_SINT
	//!\~french		Spécialisation pour PixelFormat::eR32G32_SINT
	template<>
	struct PixelComponentsT< PixelFormat::eR32G32_SINT >
	{
		using Type = int32_t;
		static PixelComponents constexpr Components = ( PixelComponent::eRed
			| PixelComponent::eGreen );
		struct Pixel
		{
			int32_t r;
			int32_t g;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type R( uint8_t const * buffer )
		{
			return getSrc( buffer ).r;
		}
		static Type G( uint8_t const * buffer )
		{
			return getSrc( buffer ).g;
		}
		static Type B( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return 0;
		}
		static Type A( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return std::numeric_limits< Type >::max();
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value;
		}
		static void G( uint8_t * buffer, Type value )
		{
			getDst( buffer ).g = value;
		}
		static void B( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
		static void A( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR32G32_SFLOAT
	//!\~french		Spécialisation pour PixelFormat::eR32G32_SFLOAT
	template<>
	struct PixelComponentsT< PixelFormat::eR32G32_SFLOAT >
	{
		using Type = float;
		static PixelComponents constexpr Components = ( PixelComponent::eRed
			| PixelComponent::eGreen );
		struct Pixel
		{
			float r;
			float g;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type R( uint8_t const * buffer )
		{
			return getSrc( buffer ).r;
		}
		static Type G( uint8_t const * buffer )
		{
			return getSrc( buffer ).g;
		}
		static Type B( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return 0;
		}
		static Type A( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return std::numeric_limits< Type >::max();
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value;
		}
		static void G( uint8_t * buffer, Type value )
		{
			getDst( buffer ).g = value;
		}
		static void B( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
		static void A( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR32G32B32_UINT
	//!\~french		Spécialisation pour PixelFormat::eR32G32B32_UINT
	template<>
	struct PixelComponentsT< PixelFormat::eR32G32B32_UINT >
	{
		using Type = uint32_t;
		static PixelComponents constexpr Components = ( PixelComponent::eRed
			| PixelComponent::eGreen
			| PixelComponent::eBlue );
		struct Pixel
		{
			uint32_t r;
			uint32_t g;
			uint32_t b;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type R( uint8_t const * buffer )
		{
			return getSrc( buffer ).r;
		}
		static Type G( uint8_t const * buffer )
		{
			return getSrc( buffer ).g;
		}
		static Type B( uint8_t const * buffer )
		{
			return getSrc( buffer ).b;
		}
		static Type A( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return std::numeric_limits< Type >::max();
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value;
		}
		static void G( uint8_t * buffer, Type value )
		{
			getDst( buffer ).g = value;
		}
		static void B( uint8_t * buffer, Type value )
		{
			getDst( buffer ).b = value;
		}
		static void A( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR32G32B32_SINT
	//!\~french		Spécialisation pour PixelFormat::eR32G32B32_SINT
	template<>
	struct PixelComponentsT< PixelFormat::eR32G32B32_SINT >
	{
		using Type = int32_t;
		static PixelComponents constexpr Components = ( PixelComponent::eRed
			| PixelComponent::eGreen
			| PixelComponent::eBlue );
		struct Pixel
		{
			int32_t r;
			int32_t g;
			int32_t b;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type R( uint8_t const * buffer )
		{
			return getSrc( buffer ).r;
		}
		static Type G( uint8_t const * buffer )
		{
			return getSrc( buffer ).g;
		}
		static Type B( uint8_t const * buffer )
		{
			return getSrc( buffer ).b;
		}
		static Type A( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return std::numeric_limits< Type >::max();
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value;
		}
		static void G( uint8_t * buffer, Type value )
		{
			getDst( buffer ).g = value;
		}
		static void B( uint8_t * buffer, Type value )
		{
			getDst( buffer ).b = value;
		}
		static void A( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR32G32B32_SFLOAT
	//!\~french		Spécialisation pour PixelFormat::eR32G32B32_SFLOAT
	template<>
	struct PixelComponentsT< PixelFormat::eR32G32B32_SFLOAT >
	{
		using Type = float;
		static PixelComponents constexpr Components = ( PixelComponent::eRed
			| PixelComponent::eGreen
			| PixelComponent::eBlue );
		struct Pixel
		{
			float r;
			float g;
			float b;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type R( uint8_t const * buffer )
		{
			return getSrc( buffer ).r;
		}
		static Type G( uint8_t const * buffer )
		{
			return getSrc( buffer ).g;
		}
		static Type B( uint8_t const * buffer )
		{
			return getSrc( buffer ).b;
		}
		static Type A( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return std::numeric_limits< Type >::max();
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value;
		}
		static void G( uint8_t * buffer, Type value )
		{
			getDst( buffer ).g = value;
		}
		static void B( uint8_t * buffer, Type value )
		{
			getDst( buffer ).b = value;
		}
		static void A( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR32G32B32A32_UINT
	//!\~french		Spécialisation pour PixelFormat::eR32G32B32A32_UINT
	template<>
	struct PixelComponentsT< PixelFormat::eR32G32B32A32_UINT >
	{
		using Type = uint32_t;
		static PixelComponents constexpr Components = ( PixelComponent::eRed
			| PixelComponent::eGreen
			| PixelComponent::eBlue
			| PixelComponent::eAlpha );
		struct Pixel
		{
			uint32_t r;
			uint32_t g;
			uint32_t b;
			uint32_t a;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type R( uint8_t const * buffer )
		{
			return getSrc( buffer ).r;
		}
		static Type G( uint8_t const * buffer )
		{
			return getSrc( buffer ).g;
		}
		static Type B( uint8_t const * buffer )
		{
			return getSrc( buffer ).b;
		}
		static Type A( uint8_t const * buffer )
		{
			return getSrc( buffer ).a;
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value;
		}
		static void G( uint8_t * buffer, Type value )
		{
			getDst( buffer ).g = value;
		}
		static void B( uint8_t * buffer, Type value )
		{
			getDst( buffer ).b = value;
		}
		static void A( uint8_t * buffer, Type value )
		{
			getDst( buffer ).a = value;
		}
	};
	//!\~english	Specialisation for PixelFormat::eR32G32B32A32_SINT
	//!\~french		Spécialisation pour PixelFormat::eR32G32B32A32_SINT
	template<>
	struct PixelComponentsT< PixelFormat::eR32G32B32A32_SINT >
	{
		using Type = int32_t;
		static PixelComponents constexpr Components = ( PixelComponent::eRed
			| PixelComponent::eGreen
			| PixelComponent::eBlue
			| PixelComponent::eAlpha );
		struct Pixel
		{
			int32_t r;
			int32_t g;
			int32_t b;
			int32_t a;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type R( uint8_t const * buffer )
		{
			return getSrc( buffer ).r;
		}
		static Type G( uint8_t const * buffer )
		{
			return getSrc( buffer ).g;
		}
		static Type B( uint8_t const * buffer )
		{
			return getSrc( buffer ).b;
		}
		static Type A( uint8_t const * buffer )
		{
			return getSrc( buffer ).a;
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value;
		}
		static void G( uint8_t * buffer, Type value )
		{
			getDst( buffer ).g = value;
		}
		static void B( uint8_t * buffer, Type value )
		{
			getDst( buffer ).b = value;
		}
		static void A( uint8_t * buffer, Type value )
		{
			getDst( buffer ).a = value;
		}
	};
	//!\~english	Specialisation for PixelFormat::eR32G32B32A32_SFLOAT
	//!\~french		Spécialisation pour PixelFormat::eR32G32B32A32_SFLOAT
	template<>
	struct PixelComponentsT< PixelFormat::eR32G32B32A32_SFLOAT >
	{
		using Type = float;
		static PixelComponents constexpr Components = ( PixelComponent::eRed
			| PixelComponent::eGreen
			| PixelComponent::eBlue
			| PixelComponent::eAlpha );
		struct Pixel
		{
			float r;
			float g;
			float b;
			float a;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type R( uint8_t const * buffer )
		{
			return getSrc( buffer ).r;
		}
		static Type G( uint8_t const * buffer )
		{
			return getSrc( buffer ).g;
		}
		static Type B( uint8_t const * buffer )
		{
			return getSrc( buffer ).b;
		}
		static Type A( uint8_t const * buffer )
		{
			return getSrc( buffer ).a;
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value;
		}
		static void G( uint8_t * buffer, Type value )
		{
			getDst( buffer ).g = value;
		}
		static void B( uint8_t * buffer, Type value )
		{
			getDst( buffer ).b = value;
		}
		static void A( uint8_t * buffer, Type value )
		{
			getDst( buffer ).a = value;
		}
	};
	//!\~english	Specialisation for PixelFormat::eR64_UINT
	//!\~french		Spécialisation pour PixelFormat::eR64_UINT
	template<>
	struct PixelComponentsT< PixelFormat::eR64_UINT >
	{
		using Type = uint64_t;
		static PixelComponents constexpr Components = ( PixelComponent::eRed );
		struct Pixel
		{
			uint64_t r;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type R( uint8_t const * buffer )
		{
			return getSrc( buffer ).r;
		}
		static Type G( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return 0;
		}
		static Type B( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return 0;
		}
		static Type A( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return std::numeric_limits< Type >::max();
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value;
		}
		static void G( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
		static void B( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
		static void A( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR64_SINT
	//!\~french		Spécialisation pour PixelFormat::eR64_SINT
	template<>
	struct PixelComponentsT< PixelFormat::eR64_SINT >
	{
		using Type = int64_t;
		static PixelComponents constexpr Components = ( PixelComponent::eRed );
		struct Pixel
		{
			int64_t r;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type R( uint8_t const * buffer )
		{
			return getSrc( buffer ).r;
		}
		static Type G( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return 0;
		}
		static Type B( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return 0;
		}
		static Type A( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return std::numeric_limits< Type >::max();
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value;
		}
		static void G( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
		static void B( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
		static void A( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR64_SFLOAT
	//!\~french		Spécialisation pour PixelFormat::eR64_SFLOAT
	template<>
	struct PixelComponentsT< PixelFormat::eR64_SFLOAT >
	{
		using Type = double;
		static PixelComponents constexpr Components = ( PixelComponent::eRed );
		struct Pixel
		{
			double r;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type R( uint8_t const * buffer )
		{
			return getSrc( buffer ).r;
		}
		static Type G( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return 0;
		}
		static Type B( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return 0;
		}
		static Type A( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return std::numeric_limits< Type >::max();
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value;
		}
		static void G( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
		static void B( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
		static void A( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR64G64_UINT
	//!\~french		Spécialisation pour PixelFormat::eR64G64_UINT
	template<>
	struct PixelComponentsT< PixelFormat::eR64G64_UINT >
	{
		using Type = uint64_t;
		static PixelComponents constexpr Components = ( PixelComponent::eRed
			| PixelComponent::eGreen );
		struct Pixel
		{
			uint64_t r;
			uint64_t g;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type R( uint8_t const * buffer )
		{
			return getSrc( buffer ).r;
		}
		static Type G( uint8_t const * buffer )
		{
			return getSrc( buffer ).g;
		}
		static Type B( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return 0;
		}
		static Type A( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return std::numeric_limits< Type >::max();
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value;
		}
		static void G( uint8_t * buffer, Type value )
		{
			getDst( buffer ).g = value;
		}
		static void B( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
		static void A( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR64G64_SINT
	//!\~french		Spécialisation pour PixelFormat::eR64G64_SINT
	template<>
	struct PixelComponentsT< PixelFormat::eR64G64_SINT >
	{
		using Type = int64_t;
		static PixelComponents constexpr Components = ( PixelComponent::eRed
			| PixelComponent::eGreen );
		struct Pixel
		{
			int64_t r;
			int64_t g;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type R( uint8_t const * buffer )
		{
			return getSrc( buffer ).r;
		}
		static Type G( uint8_t const * buffer )
		{
			return getSrc( buffer ).g;
		}
		static Type B( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return 0;
		}
		static Type A( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return std::numeric_limits< Type >::max();
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value;
		}
		static void G( uint8_t * buffer, Type value )
		{
			getDst( buffer ).g = value;
		}
		static void B( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
		static void A( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR64G64_SFLOAT
	//!\~french		Spécialisation pour PixelFormat::eR64G64_SFLOAT
	template<>
	struct PixelComponentsT< PixelFormat::eR64G64_SFLOAT >
	{
		using Type = double;
		static PixelComponents constexpr Components = ( PixelComponent::eRed
			| PixelComponent::eGreen );
		struct Pixel
		{
			double r;
			double g;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type R( uint8_t const * buffer )
		{
			return getSrc( buffer ).r;
		}
		static Type G( uint8_t const * buffer )
		{
			return getSrc( buffer ).g;
		}
		static Type B( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return 0;
		}
		static Type A( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return std::numeric_limits< Type >::max();
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value;
		}
		static void G( uint8_t * buffer, Type value )
		{
			getDst( buffer ).g = value;
		}
		static void B( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
		static void A( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR64G64B64_UINT
	//!\~french		Spécialisation pour PixelFormat::eR64G64B64_UINT
	template<>
	struct PixelComponentsT< PixelFormat::eR64G64B64_UINT >
	{
		using Type = uint64_t;
		static PixelComponents constexpr Components = ( PixelComponent::eRed
			| PixelComponent::eGreen
			| PixelComponent::eBlue );
		struct Pixel
		{
			uint64_t r;
			uint64_t g;
			uint64_t b;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type R( uint8_t const * buffer )
		{
			return getSrc( buffer ).r;
		}
		static Type G( uint8_t const * buffer )
		{
			return getSrc( buffer ).g;
		}
		static Type B( uint8_t const * buffer )
		{
			return getSrc( buffer ).b;
		}
		static Type A( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return std::numeric_limits< Type >::max();
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value;
		}
		static void G( uint8_t * buffer, Type value )
		{
			getDst( buffer ).g = value;
		}
		static void B( uint8_t * buffer, Type value )
		{
			getDst( buffer ).b = value;
		}
		static void A( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR64G64B64_SINT
	//!\~french		Spécialisation pour PixelFormat::eR64G64B64_SINT
	template<>
	struct PixelComponentsT< PixelFormat::eR64G64B64_SINT >
	{
		using Type = int64_t;
		static PixelComponents constexpr Components = ( PixelComponent::eRed
			| PixelComponent::eGreen
			| PixelComponent::eBlue );
		struct Pixel
		{
			int64_t r;
			int64_t g;
			int64_t b;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type R( uint8_t const * buffer )
		{
			return getSrc( buffer ).r;
		}
		static Type G( uint8_t const * buffer )
		{
			return getSrc( buffer ).g;
		}
		static Type B( uint8_t const * buffer )
		{
			return getSrc( buffer ).b;
		}
		static Type A( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return std::numeric_limits< Type >::max();
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value;
		}
		static void G( uint8_t * buffer, Type value )
		{
			getDst( buffer ).g = value;
		}
		static void B( uint8_t * buffer, Type value )
		{
			getDst( buffer ).b = value;
		}
		static void A( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR64G64B64_SFLOAT
	//!\~french		Spécialisation pour PixelFormat::eR64G64B64_SFLOAT
	template<>
	struct PixelComponentsT< PixelFormat::eR64G64B64_SFLOAT >
	{
		using Type = double;
		static PixelComponents constexpr Components = ( PixelComponent::eRed
			| PixelComponent::eGreen
			| PixelComponent::eBlue );
		struct Pixel
		{
			double r;
			double g;
			double b;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type R( uint8_t const * buffer )
		{
			return getSrc( buffer ).r;
		}
		static Type G( uint8_t const * buffer )
		{
			return getSrc( buffer ).g;
		}
		static Type B( uint8_t const * buffer )
		{
			return getSrc( buffer ).b;
		}
		static Type A( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return std::numeric_limits< Type >::max();
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value;
		}
		static void G( uint8_t * buffer, Type value )
		{
			getDst( buffer ).g = value;
		}
		static void B( uint8_t * buffer, Type value )
		{
			getDst( buffer ).b = value;
		}
		static void A( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR64G64B64A64_UINT
	//!\~french		Spécialisation pour PixelFormat::eR64G64B64A64_UINT
	template<>
	struct PixelComponentsT< PixelFormat::eR64G64B64A64_UINT >
	{
		using Type = uint64_t;
		static PixelComponents constexpr Components = ( PixelComponent::eRed
			| PixelComponent::eGreen
			| PixelComponent::eBlue
			| PixelComponent::eAlpha );
		struct Pixel
		{
			uint64_t r;
			uint64_t g;
			uint64_t b;
			uint64_t a;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type R( uint8_t const * buffer )
		{
			return getSrc( buffer ).r;
		}
		static Type G( uint8_t const * buffer )
		{
			return getSrc( buffer ).g;
		}
		static Type B( uint8_t const * buffer )
		{
			return getSrc( buffer ).b;
		}
		static Type A( uint8_t const * buffer )
		{
			return getSrc( buffer ).a;
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value;
		}
		static void G( uint8_t * buffer, Type value )
		{
			getDst( buffer ).g = value;
		}
		static void B( uint8_t * buffer, Type value )
		{
			getDst( buffer ).b = value;
		}
		static void A( uint8_t * buffer, Type value )
		{
			getDst( buffer ).a = value;
		}
	};
	//!\~english	Specialisation for PixelFormat::eR64G64B64A64_SINT
	//!\~french		Spécialisation pour PixelFormat::eR64G64B64A64_SINT
	template<>
	struct PixelComponentsT< PixelFormat::eR64G64B64A64_SINT >
	{
		using Type = int64_t;
		static PixelComponents constexpr Components = ( PixelComponent::eRed
			| PixelComponent::eGreen
			| PixelComponent::eBlue
			| PixelComponent::eAlpha );
		struct Pixel
		{
			int64_t r;
			int64_t g;
			int64_t b;
			int64_t a;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type R( uint8_t const * buffer )
		{
			return getSrc( buffer ).r;
		}
		static Type G( uint8_t const * buffer )
		{
			return getSrc( buffer ).g;
		}
		static Type B( uint8_t const * buffer )
		{
			return getSrc( buffer ).b;
		}
		static Type A( uint8_t const * buffer )
		{
			return getSrc( buffer ).a;
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value;
		}
		static void G( uint8_t * buffer, Type value )
		{
			getDst( buffer ).g = value;
		}
		static void B( uint8_t * buffer, Type value )
		{
			getDst( buffer ).b = value;
		}
		static void A( uint8_t * buffer, Type value )
		{
			getDst( buffer ).a = value;
		}
	};
	//!\~english	Specialisation for PixelFormat::eR64G64B64A64_SFLOAT
	//!\~french		Spécialisation pour PixelFormat::eR64G64B64A64_SFLOAT
	template<>
	struct PixelComponentsT< PixelFormat::eR64G64B64A64_SFLOAT >
	{
		using Type = double;
		static PixelComponents constexpr Components = ( PixelComponent::eRed
			| PixelComponent::eGreen
			| PixelComponent::eBlue
			| PixelComponent::eAlpha );
		struct Pixel
		{
			double r;
			double g;
			double b;
			double a;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type R( uint8_t const * buffer )
		{
			return getSrc( buffer ).r;
		}
		static Type G( uint8_t const * buffer )
		{
			return getSrc( buffer ).g;
		}
		static Type B( uint8_t const * buffer )
		{
			return getSrc( buffer ).b;
		}
		static Type A( uint8_t const * buffer )
		{
			return getSrc( buffer ).a;
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value;
		}
		static void G( uint8_t * buffer, Type value )
		{
			getDst( buffer ).g = value;
		}
		static void B( uint8_t * buffer, Type value )
		{
			getDst( buffer ).b = value;
		}
		static void A( uint8_t * buffer, Type value )
		{
			getDst( buffer ).a = value;
		}
	};
	//!\~english	Specialisation for PixelFormat::eB10G11R11_UFLOAT
	//!\~french		Spécialisation pour PixelFormat::eB10G11R11_UFLOAT
	template<>
	struct PixelComponentsT< PixelFormat::eB10G11R11_UFLOAT >
	{
		using Type = float;
		static PixelComponents constexpr Components = ( PixelComponent::eRed
			| PixelComponent::eGreen
			| PixelComponent::eBlue );
		struct Pixel
		{
			uint32_t b : 10;
			uint32_t g : 11;
			uint32_t r : 11;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type R( uint8_t const * buffer )
		{
			return Type( getSrc( buffer ).r << 21 );
		}
		static Type G( uint8_t const * buffer )
		{
			return Type( getSrc( buffer ).g << 21 );
		}
		static Type B( uint8_t const * buffer )
		{
			return Type( getSrc( buffer ).b << 22 );
		}
		static Type A( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return std::numeric_limits< float >::max();
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = ( uint32_t( value ) >> 21 );
		}
		static void G( uint8_t * buffer, Type value )
		{
			getDst( buffer ).g = ( uint32_t( value ) >> 21 );
		}
		static void B( uint8_t * buffer, Type value )
		{
			getDst( buffer ).b = ( uint32_t( value ) >> 21 );
		}
		static void A( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eE5B9G9R9_UFLOAT
	//!\~french		Spécialisation pour PixelFormat::eE5B9G9R9_UFLOAT
	template<>
	struct PixelComponentsT< PixelFormat::eE5B9G9R9_UFLOAT >
	{
		using Type = float;
		static PixelComponents constexpr Components = ( PixelComponent::eRed
			| PixelComponent::eGreen
			| PixelComponent::eBlue
			| PixelComponent::eAlpha );
		struct Pixel
		{
			uint32_t e : 5;
			uint32_t b : 9;
			uint32_t g : 9;
			uint32_t r : 9;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type R( uint8_t const * buffer )
		{
			return Type( getSrc( buffer ).r << 23 );
		}
		static Type G( uint8_t const * buffer )
		{
			return Type( getSrc( buffer ).g << 23 );
		}
		static Type B( uint8_t const * buffer )
		{
			return Type( getSrc( buffer ).b << 23 );
		}
		static Type A( CU_UnusedParam( uint8_t const *, buffer ) )
		{
			return std::numeric_limits< float >::max();
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = ( uint32_t( value ) >> 23 );
		}
		static void G( uint8_t * buffer, Type value )
		{
			getDst( buffer ).g = ( uint32_t( value ) >> 23 );
		}
		static void B( uint8_t * buffer, Type value )
		{
			getDst( buffer ).b = ( uint32_t( value ) >> 23 );
		}
		static void A( CU_UnusedParam( uint8_t *, buffer ), CU_UnusedParam( Type, value ) )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eD16_UNORM
	//!\~french		Spécialisation pour PixelFormat::eD16_UNORM
	template<>
	struct PixelComponentsT< PixelFormat::eD16_UNORM >
	{
		using Type = uint16_t;
		static PixelComponents constexpr Components = ( PixelComponent::eDepth );
		struct Pixel
		{
			uint16_t d;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type D( uint8_t const * buffer )
		{
			return getSrc( buffer ).d;
		}
		static uint8_t S( uint8_t const * )
		{
			return 0;
		}
		static void D( uint8_t * buffer, Type value )
		{
			getDst( buffer ).d = value;
		}
		static void S( uint8_t *, uint8_t )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eX8_D24_UNORM
	//!\~french		Spécialisation pour PixelFormat::eX8_D24_UNORM
	template<>
	struct PixelComponentsT< PixelFormat::eX8_D24_UNORM >
	{
		using Type = uint32_t;
		static PixelComponents constexpr Components = ( PixelComponent::eDepth );
		struct Pixel
		{
			uint32_t d : 24, : 8;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type D( uint8_t const * buffer )
		{
			return getSrc( buffer ).d;
		}
		static uint8_t S( uint8_t const * )
		{
			return 0;
		}
		static void D( uint8_t * buffer, Type value )
		{
			getDst( buffer ).d = value;
		}
		static void S( uint8_t *, uint8_t )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eD32_SFLOAT
	//!\~french		Spécialisation pour PixelFormat::eD32_SFLOAT
	template<>
	struct PixelComponentsT< PixelFormat::eD32_SFLOAT >
	{
		using Type = float;
		static PixelComponents constexpr Components = ( PixelComponent::eDepth );
		struct Pixel
		{
			float d;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type D( uint8_t const * buffer )
		{
			return getSrc( buffer ).d;
		}
		static uint8_t S( uint8_t const * )
		{
			return 0;
		}
		static void D( uint8_t * buffer, Type value )
		{
			getDst( buffer ).d = value;
		}
		static void S( uint8_t *, uint8_t )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eS8_UINT
	//!\~french		Spécialisation pour PixelFormat::eS8_UINT
	template<>
	struct PixelComponentsT< PixelFormat::eS8_UINT >
	{
		using Type = uint8_t;
		static PixelComponents constexpr Components = ( PixelComponent::eStencil );
		struct Pixel
		{
			uint8_t s;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type D( uint8_t const * )
		{
			return 0;
		}
		static uint8_t S( uint8_t const * buffer )
		{
			return getSrc( buffer ).s;
		}
		static void D( uint8_t *, Type )
		{
		}
		static void S( uint8_t * buffer, uint8_t value )
		{
			getDst( buffer ).s = value;
		}
	};
	//!\~english	Specialisation for PixelFormat::eD16_UNORM_S8_UINT
	//!\~french		Spécialisation pour PixelFormat::eD16_UNORM_S8_UINT
	template<>
	struct PixelComponentsT< PixelFormat::eD16_UNORM_S8_UINT >
	{
		using Type = uint16_t;
		static PixelComponents constexpr Components = ( PixelComponent::eDepth
			| PixelComponent::eStencil );
		struct Pixel
		{
			uint16_t d;
			uint8_t s;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type D( uint8_t const * buffer )
		{
			return getSrc( buffer ).d;
		}
		static uint8_t S( uint8_t const * buffer )
		{
			return getSrc( buffer ).s;
		}
		static void D( uint8_t * buffer, Type value )
		{
			getDst( buffer ).d = value;
		}
		static void S( uint8_t * buffer, uint8_t value )
		{
			getDst( buffer ).s = value;
		}
	};
	//!\~english	Specialisation for PixelFormat::eD24_UNORM_S8_UINT
	//!\~french		Spécialisation pour PixelFormat::eD24_UNORM_S8_UINT
	template<>
	struct PixelComponentsT< PixelFormat::eD24_UNORM_S8_UINT >
	{
		using Type = uint32_t;
		static PixelComponents constexpr Components = ( PixelComponent::eDepth
			| PixelComponent::eStencil );
		struct Pixel
		{
			uint32_t d : 24;
			uint32_t s : 8;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type D( uint8_t const * buffer )
		{
			return getSrc( buffer ).d;
		}
		static uint8_t S( uint8_t const * buffer )
		{
			return getSrc( buffer ).s;
		}
		static void D( uint8_t * buffer, Type value )
		{
			getDst( buffer ).d = value;
		}
		static void S( uint8_t * buffer, uint8_t value )
		{
			getDst( buffer ).s = value;
		}
	};
	//!\~english	Specialisation for PixelFormat::eD24_UNORM_S8_UINT
	//!\~french		Spécialisation pour PixelFormat::eD24_UNORM_S8_UINT
	template<>
	struct PixelComponentsT< PixelFormat::eD32_SFLOAT_S8_UINT >
	{
		using Type = float;
		static PixelComponents constexpr Components = ( PixelComponent::eDepth
			| PixelComponent::eStencil );
		struct Pixel
		{
			uint32_t d : 24;
			uint32_t s : 8;
		};
		static Pixel const & getSrc( void const * buffer )
		{
			return *reinterpret_cast< Pixel const * >( buffer );
		}
		static Pixel & getDst( void * buffer )
		{
			return *reinterpret_cast< Pixel * >( buffer );
		}
		static Type D( uint8_t const * buffer )
		{
			return float( getSrc( buffer ).d );
		}
		static uint8_t S( uint8_t const * buffer )
		{
			return getSrc( buffer ).s;
		}
		static void D( uint8_t * buffer, Type value )
		{
			getDst( buffer ).d = uint32_t( value );
		}
		static void S( uint8_t * buffer, uint8_t value )
		{
			getDst( buffer ).s = value;
		}
	};

	//*************************************************************************************************

	namespace details
	{
		template< typename ReturnT >
		struct ConvCastT;

		template<>
		struct ConvCastT< uint8_t >
		{
			using Type = uint8_t;

			static Type cast( uint8_t value )
			{
				return value;
			}
			static Type cast( int8_t value )
			{
				return Type( value );
			}
			static Type cast( uint16_t value )
			{
				return Type( value >> 8 );
			}
			static Type cast( int16_t value )
			{
				return Type( value >> 8 );
			}
			static Type cast( uint32_t value )
			{
				return Type( value >> 24 );
			}
			static Type cast( int32_t value )
			{
				return Type( value >> 24 );
			}
			static Type cast( uint64_t value )
			{
				return Type( value >> 56 );
			}
			static Type cast( int64_t value )
			{
				return Type( value >> 56 );
			}
			static Type cast( float value )
			{
				return Type( value * 255.0f );
			}
			static Type cast( double value )
			{
				return Type( value * 255.0 );
			}
		};

		template<>
		struct ConvCastT< int8_t >
		{
			using Type = int8_t;

			static Type cast( uint8_t value )
			{
				return Type( value );
			}
			static Type cast( int8_t value )
			{
				return value;
			}
			static Type cast( uint16_t value )
			{
				return Type( value >> 8 );
			}
			static Type cast( int16_t value )
			{
				return Type( value >> 8 );
			}
			static Type cast( uint32_t value )
			{
				return Type( value >> 24 );
			}
			static Type cast( int32_t value )
			{
				return Type( value >> 24 );
			}
			static Type cast( uint64_t value )
			{
				return Type( value >> 56 );
			}
			static Type cast( int64_t value )
			{
				return Type( value >> 56 );
			}
			static Type cast( float value )
			{
				return Type( value * 128.0f );
			}
			static Type cast( double value )
			{
				return Type( value * 128.0 );
			}
		};

		template<>
		struct ConvCastT< uint16_t >
		{
			using Type = uint16_t;

			static Type cast( uint8_t value )
			{
				return Type( Type( value ) << 8 );
			}
			static Type cast( int8_t value )
			{
				return Type( Type( value ) << 8 );
			}
			static Type cast( uint16_t value )
			{
				return value;
			}
			static Type cast( int16_t value )
			{
				return Type( value );
			}
			static Type cast( uint32_t value )
			{
				return Type( value >> 16 );
			}
			static Type cast( int32_t value )
			{
				return Type( value >> 16 );
			}
			static Type cast( uint64_t value )
			{
				return Type( value >> 48 );
			}
			static Type cast( int64_t value )
			{
				return Type( value >> 48 );
			}
			static Type cast( float value )
			{
				return Type( value * 65536.0f );
			}
			static Type cast( double value )
			{
				return Type( value * 65536.0 );
			}
		};

		template<>
		struct ConvCastT< int16_t >
		{
			using Type = int16_t;

			static Type cast( uint8_t value )
			{
				return Type( value ) << 8;
			}
			static Type cast( int8_t value )
			{
				return Type( value ) << 8;
			}
			static Type cast( uint16_t value )
			{
				return Type( value );
			}
			static Type cast( int16_t value )
			{
				return value;
			}
			static Type cast( uint32_t value )
			{
				return Type( value >> 16 );
			}
			static Type cast( int32_t value )
			{
				return Type( value >> 16 );
			}
			static Type cast( uint64_t value )
			{
				return Type( value >> 48 );
			}
			static Type cast( int64_t value )
			{
				return Type( value >> 48 );
			}
			static Type cast( float value )
			{
				return Type( value * 32768.0f );
			}
			static Type cast( double value )
			{
				return Type( value * 32765.0 );
			}
		};

		template<>
		struct ConvCastT< uint32_t >
		{
			using Type = uint32_t;

			static Type cast( uint8_t value )
			{
				return Type( value ) << 24;
			}
			static Type cast( int8_t value )
			{
				return Type( value ) << 24;
			}
			static Type cast( uint16_t value )
			{
				return Type( value ) << 8;
			}
			static Type cast( int16_t value )
			{
				return Type( value ) << 8;
			}
			static Type cast( uint32_t value )
			{
				return value;
			}
			static Type cast( int32_t value )
			{
				return Type( value );
			}
			static Type cast( uint64_t value )
			{
				return Type( value >> 32 );
			}
			static Type cast( int64_t value )
			{
				return Type( value >> 32 );
			}
			static Type cast( float value )
			{
				return Type( value * float( 65536.0 * 65536.0 ) );
			}
			static Type cast( double value )
			{
				return Type( value * ( 65536.0 * 65536.0 ) );
			}
		};

		template<>
		struct ConvCastT< int32_t >
		{
			using Type = int32_t;

			static Type cast( uint8_t value )
			{
				return Type( value ) << 24;
			}
			static Type cast( int8_t value )
			{
				return Type( value ) << 24;
			}
			static Type cast( uint16_t value )
			{
				return Type( value ) << 8;
			}
			static Type cast( int16_t value )
			{
				return Type( value ) << 8;
			}
			static Type cast( uint32_t value )
			{
				return Type( value );
			}
			static Type cast( int32_t value )
			{
				return value;
			}
			static Type cast( uint64_t value )
			{
				return Type( value >> 32 );
			}
			static Type cast( int64_t value )
			{
				return Type( value >> 32 );
			}
			static Type cast( float value )
			{
				return Type( value );
			}
			static Type cast( double value )
			{
				return Type( value );
			}
		};

		template<>
		struct ConvCastT< uint64_t >
		{
			using Type = uint64_t;

			static Type cast( uint8_t value )
			{
				return Type( value ) << 56;
			}
			static Type cast( int8_t value )
			{
				return Type( value ) << 56;
			}
			static Type cast( uint16_t value )
			{
				return Type( value ) << 48;
			}
			static Type cast( int16_t value )
			{
				return Type( value ) << 48;
			}
			static Type cast( uint32_t value )
			{
				return Type( value ) << 32;
			}
			static Type cast( int32_t value )
			{
				return Type( value ) << 32;
			}
			static Type cast( uint64_t value )
			{
				return value;
			}
			static Type cast( int64_t value )
			{
				return Type( value );
			}
			static Type cast( float value )
			{
				return Type( value );
			}
			static Type cast( double value )
			{
				return Type( value );
			}
		};

		template<>
		struct ConvCastT< int64_t >
		{
			using Type = int64_t;

			static Type cast( uint8_t value )
			{
				return Type( value ) << 56;
			}
			static Type cast( int8_t value )
			{
				return Type( value ) << 56;
			}
			static Type cast( uint16_t value )
			{
				return Type( value ) << 48;
			}
			static Type cast( int16_t value )
			{
				return Type( value ) << 48;
			}
			static Type cast( uint32_t value )
			{
				return Type( value ) << 32;
			}
			static Type cast( int32_t value )
			{
				return Type( value ) << 32;
			}
			static Type cast( uint64_t value )
			{
				return Type( value );
			}
			static Type cast( int64_t value )
			{
				return value;
			}
			static Type cast( float value )
			{
				return Type( value );
			}
			static Type cast( double value )
			{
				return Type( value );
			}
		};

		template<>
		struct ConvCastT< float >
		{
			using Type = float;

			static Type cast( uint8_t value )
			{
				return Type( value );
			}
			static Type cast( int8_t value )
			{
				return Type( value );
			}
			static Type cast( uint16_t value )
			{
				return Type( value );
			}
			static Type cast( int16_t value )
			{
				return Type( value );
			}
			static Type cast( uint32_t value )
			{
				return Type( value );
			}
			static Type cast( int32_t value )
			{
				return Type( value );
			}
			static Type cast( uint64_t value )
			{
				return Type( value );
			}
			static Type cast( int64_t value )
			{
				return Type( value );
			}
			static Type cast( float value )
			{
				return value;
			}
			static Type cast( double value )
			{
				return Type( value );
			}
		};

		template<>
		struct ConvCastT< double >
		{
			using Type = double;

			static Type cast( uint8_t value )
			{
				return Type( value );
			}
			static Type cast( int8_t value )
			{
				return Type( value );
			}
			static Type cast( uint16_t value )
			{
				return Type( value );
			}
			static Type cast( int16_t value )
			{
				return Type( value );
			}
			static Type cast( uint32_t value )
			{
				return Type( value );
			}
			static Type cast( int32_t value )
			{
				return Type( value );
			}
			static Type cast( uint64_t value )
			{
				return Type( value );
			}
			static Type cast( int64_t value )
			{
				return Type( value );
			}
			static Type cast( float value )
			{
				return Type( value );
			}
			static Type cast( double value )
			{
				return value;
			}
		};

		template< typename ReturnT, typename ParamT >
		ReturnT componentCast( ParamT value )
		{
			return ConvCastT< ReturnT >::cast( value );
		}

		template< typename TypeT >
		struct PxComponentGetter
		{
			template< typename ParamT >
			static TypeT get( uint8_t const * buffer
				, ParamT( *getX )( uint8_t const * ) )
			{
				return componentCast< TypeT >( getX( buffer ) );
			}
		};

		template< typename TypeT >
		struct PxComponentSetter
		{
			template< typename ParamT >
			static void set( uint8_t * buffer
				, TypeT value
				, void( *setX )( uint8_t *, ParamT ) )
			{
				setX( buffer, componentCast< ParamT >( value ) );
			}
		};
	}

	//*************************************************************************************************

	template< PixelFormat PFT, PixelComponent PCT >
	uint8_t getX8U( uint8_t const * buffer )
	{
		if constexpr ( PCT == PixelComponent::eRed )
		{
			return getR8U< PFT >( buffer );
		}
		else if constexpr ( PCT == PixelComponent::eGreen )
		{
			return getG8U< PFT >( buffer );
		}
		else if constexpr ( PCT == PixelComponent::eBlue )
		{
			return getB8U< PFT >( buffer );
		}
		else if constexpr ( PCT == PixelComponent::eAlpha )
		{
			return getA8U< PFT >( buffer );
		}
		else if constexpr ( PCT == PixelComponent::eStencil )
		{
			return getS8U< PFT >( buffer );
		}
		else
		{
			return 0;
		}
	}

	template< PixelFormat PFT, PixelComponent PCT >
	int8_t getX8S( uint8_t const * buffer )
	{
		if constexpr ( PCT == PixelComponent::eRed )
		{
			return getR8S< PFT >( buffer );
		}
		else if constexpr ( PCT == PixelComponent::eGreen )
		{
			return getG8S< PFT >( buffer );
		}
		else if constexpr ( PCT == PixelComponent::eBlue )
		{
			return getB8S< PFT >( buffer );
		}
		else if constexpr ( PCT == PixelComponent::eAlpha )
		{
			return getA8S< PFT >( buffer );
		}
		else
		{
			return 0;
		}
	}

	template< PixelFormat PFT, PixelComponent PCT >
	uint16_t getX16U( uint8_t const * buffer )
	{
		if constexpr ( PCT == PixelComponent::eRed )
		{
			return getR16U< PFT >( buffer );
		}
		else if constexpr ( PCT == PixelComponent::eGreen )
		{
			return getG16U< PFT >( buffer );
		}
		else if constexpr ( PCT == PixelComponent::eBlue )
		{
			return getB16U< PFT >( buffer );
		}
		else if constexpr ( PCT == PixelComponent::eAlpha )
		{
			return getA16U< PFT >( buffer );
		}
		else if constexpr ( PCT == PixelComponent::eDepth )
		{
			return getD16U< PFT >( buffer );
		}
		else
		{
			return 0;
		}
	}

	template< PixelFormat PFT, PixelComponent PCT >
	int16_t getX16S( uint8_t const * buffer )
	{
		if constexpr ( PCT == PixelComponent::eRed )
		{
			return getR16S< PFT >( buffer );
		}
		else if constexpr ( PCT == PixelComponent::eGreen )
		{
			return getG16S< PFT >( buffer );
		}
		else if constexpr ( PCT == PixelComponent::eBlue )
		{
			return getB16S< PFT >( buffer );
		}
		else if constexpr ( PCT == PixelComponent::eAlpha )
		{
			return getA16S< PFT >( buffer );
		}
		else
		{
			return 0;
		}
	}

	template< PixelFormat PFT, PixelComponent PCT >
	int16_t getX16F( uint8_t const * buffer )
	{
		if constexpr ( PCT == PixelComponent::eRed )
		{
			return getR16F< PFT >( buffer );
		}
		else if constexpr ( PCT == PixelComponent::eGreen )
		{
			return getG16F< PFT >( buffer );
		}
		else if constexpr ( PCT == PixelComponent::eBlue )
		{
			return getB16F< PFT >( buffer );
		}
		else if constexpr ( PCT == PixelComponent::eAlpha )
		{
			return getA16F< PFT >( buffer );
		}
		else
		{
			return 0;
		}
	}

	template< PixelFormat PFT, PixelComponent PCT >
	uint32_t getX32U( uint8_t const * buffer )
	{
		if constexpr ( PCT == PixelComponent::eRed )
		{
			return getR32U< PFT >( buffer );
		}
		else if constexpr ( PCT == PixelComponent::eGreen )
		{
			return getG32U< PFT >( buffer );
		}
		else if constexpr ( PCT == PixelComponent::eBlue )
		{
			return getB32U< PFT >( buffer );
		}
		else if constexpr ( PCT == PixelComponent::eAlpha )
		{
			return getA32U< PFT >( buffer );
		}
		else if constexpr ( PCT == PixelComponent::eDepth )
		{
			return getD24U< PFT >( buffer );
		}
		else
		{
			return 0;
		}
	}

	template< PixelFormat PFT, PixelComponent PCT >
	int32_t getX32S( uint8_t const * buffer )
	{
		if constexpr ( PCT == PixelComponent::eRed )
		{
			return getR32S< PFT >( buffer );
		}
		else if constexpr ( PCT == PixelComponent::eGreen )
		{
			return getG32S< PFT >( buffer );
		}
		else if constexpr ( PCT == PixelComponent::eBlue )
		{
			return getB32S< PFT >( buffer );
		}
		else if constexpr ( PCT == PixelComponent::eAlpha )
		{
			return getA32S< PFT >( buffer );
		}
		else
		{
			return 0;
		}
	}

	template< PixelFormat PFT, PixelComponent PCT >
	float getX32F( uint8_t const * buffer )
	{
		if constexpr ( PCT == PixelComponent::eRed )
		{
			return getR32F< PFT >( buffer );
		}
		else if constexpr ( PCT == PixelComponent::eGreen )
		{
			return getG32F< PFT >( buffer );
		}
		else if constexpr ( PCT == PixelComponent::eBlue )
		{
			return getB32F< PFT >( buffer );
		}
		else if constexpr ( PCT == PixelComponent::eAlpha )
		{
			return getA32F< PFT >( buffer );
		}
		else if constexpr ( PCT == PixelComponent::eDepth )
		{
			return getD32F< PFT >( buffer );
		}
		else
		{
			return 0;
		}
	}

	template< PixelFormat PFT, PixelComponent PCT >
	uint64_t getX64U( uint8_t const * buffer )
	{
		if constexpr ( PCT == PixelComponent::eRed )
		{
			return getR64U< PFT >( buffer );
		}
		else if constexpr ( PCT == PixelComponent::eGreen )
		{
			return getG64U< PFT >( buffer );
		}
		else if constexpr ( PCT == PixelComponent::eBlue )
		{
			return getB64U< PFT >( buffer );
		}
		else if constexpr ( PCT == PixelComponent::eAlpha )
		{
			return getA64U< PFT >( buffer );
		}
		else
		{
			return 0;
		}
	}

	template< PixelFormat PFT, PixelComponent PCT >
	int64_t getX64S( uint8_t const * buffer )
	{
		if constexpr ( PCT == PixelComponent::eRed )
		{
			return getR64S< PFT >( buffer );
		}
		else if constexpr ( PCT == PixelComponent::eGreen )
		{
			return getG64S< PFT >( buffer );
		}
		else if constexpr ( PCT == PixelComponent::eBlue )
		{
			return getB64S< PFT >( buffer );
		}
		else if constexpr ( PCT == PixelComponent::eAlpha )
		{
			return getA64S< PFT >( buffer );
		}
		else
		{
			return 0;
		}
	}

	template< PixelFormat PFT, PixelComponent PCT >
	double getX64F( uint8_t const * buffer )
	{
		if constexpr ( PCT == PixelComponent::eRed )
		{
			return getR64F< PFT >( buffer );
		}
		else if constexpr ( PCT == PixelComponent::eGreen )
		{
			return getG64F< PFT >( buffer );
		}
		else if constexpr ( PCT == PixelComponent::eBlue )
		{
			return getB64F< PFT >( buffer );
		}
		else if constexpr ( PCT == PixelComponent::eAlpha )
		{
			return getA64F< PFT >( buffer );
		}
		else
		{
			return 0;
		}
	}

	//*************************************************************************************************

	template< PixelFormat PFT, PixelComponent PCT >
	void setX8U( uint8_t * buffer, uint8_t value )
	{
		if constexpr ( PCT == PixelComponent::eRed )
		{
			return setR8U< PFT >( buffer, value );
		}
		else if constexpr ( PCT == PixelComponent::eGreen )
		{
			return setG8U< PFT >( buffer, value );
		}
		else if constexpr ( PCT == PixelComponent::eBlue )
		{
			return setB8U< PFT >( buffer, value );
		}
		else if constexpr ( PCT == PixelComponent::eAlpha )
		{
			return setA8U< PFT >( buffer, value );
		}
		else if constexpr ( PCT == PixelComponent::eStencil )
		{
			return setS8U< PFT >( buffer, value );
		}
	}

	template< PixelFormat PFT, PixelComponent PCT >
	void setX8S( uint8_t * buffer, int8_t value )
	{
		if constexpr ( PCT == PixelComponent::eRed )
		{
			return setR8S< PFT >( buffer, value );
		}
		else if constexpr ( PCT == PixelComponent::eGreen )
		{
			return setG8S< PFT >( buffer, value );
		}
		else if constexpr ( PCT == PixelComponent::eBlue )
		{
			return setB8S< PFT >( buffer, value );
		}
		else if constexpr ( PCT == PixelComponent::eAlpha )
		{
			return setA8S< PFT >( buffer, value );
		}
	}

	template< PixelFormat PFT, PixelComponent PCT >
	void setX16U( uint8_t * buffer, uint16_t value )
	{
		if constexpr ( PCT == PixelComponent::eRed )
		{
			return setR16U< PFT >( buffer, value );
		}
		else if constexpr ( PCT == PixelComponent::eGreen )
		{
			return setG16U< PFT >( buffer, value );
		}
		else if constexpr ( PCT == PixelComponent::eBlue )
		{
			return setB16U< PFT >( buffer, value );
		}
		else if constexpr ( PCT == PixelComponent::eAlpha )
		{
			return setA16U< PFT >( buffer, value );
		}
		else if constexpr ( PCT == PixelComponent::eDepth )
		{
			return setD16U< PFT >( buffer, value );
		}
	}

	template< PixelFormat PFT, PixelComponent PCT >
	void setX16S( uint8_t * buffer, int16_t value )
	{
		if constexpr ( PCT == PixelComponent::eRed )
		{
			return setR16S< PFT >( buffer, value );
		}
		else if constexpr ( PCT == PixelComponent::eGreen )
		{
			return setG16S< PFT >( buffer, value );
		}
		else if constexpr ( PCT == PixelComponent::eBlue )
		{
			return setB16S< PFT >( buffer, value );
		}
		else if constexpr ( PCT == PixelComponent::eAlpha )
		{
			return setA16S< PFT >( buffer, value );
		}
	}

	template< PixelFormat PFT, PixelComponent PCT >
	void setX16F( uint8_t * buffer, int16_t value )
	{
		if constexpr ( PCT == PixelComponent::eRed )
		{
			return setR16F< PFT >( buffer, value );
		}
		else if constexpr ( PCT == PixelComponent::eGreen )
		{
			return setG16F< PFT >( buffer, value );
		}
		else if constexpr ( PCT == PixelComponent::eBlue )
		{
			return setB16F< PFT >( buffer, value );
		}
		else if constexpr ( PCT == PixelComponent::eAlpha )
		{
			return setA16F< PFT >( buffer, value );
		}
	}

	template< PixelFormat PFT, PixelComponent PCT >
	void setX32U( uint8_t * buffer, uint32_t value )
	{
		if constexpr ( PCT == PixelComponent::eRed )
		{
			return setR32U< PFT >( buffer, value );
		}
		else if constexpr ( PCT == PixelComponent::eGreen )
		{
			return setG32U< PFT >( buffer, value );
		}
		else if constexpr ( PCT == PixelComponent::eBlue )
		{
			return setB32U< PFT >( buffer, value );
		}
		else if constexpr ( PCT == PixelComponent::eAlpha )
		{
			return setA32U< PFT >( buffer, value );
		}
		else if constexpr ( PCT == PixelComponent::eDepth )
		{
			return setD24U< PFT >( buffer, value );
		}
	}

	template< PixelFormat PFT, PixelComponent PCT >
	void setX32S( uint8_t * buffer, int32_t value )
	{
		if constexpr ( PCT == PixelComponent::eRed )
		{
			return setR32S< PFT >( buffer, value );
		}
		else if constexpr ( PCT == PixelComponent::eGreen )
		{
			return setG32S< PFT >( buffer, value );
		}
		else if constexpr ( PCT == PixelComponent::eBlue )
		{
			return setB32S< PFT >( buffer, value );
		}
		else if constexpr ( PCT == PixelComponent::eAlpha )
		{
			return setA32S< PFT >( buffer, value );
		}
	}

	template< PixelFormat PFT, PixelComponent PCT >
	void setX32F( uint8_t * buffer, float value )
	{
		if constexpr ( PCT == PixelComponent::eRed )
		{
			return setR32F< PFT >( buffer, value );
		}
		else if constexpr ( PCT == PixelComponent::eGreen )
		{
			return setG32F< PFT >( buffer, value );
		}
		else if constexpr ( PCT == PixelComponent::eBlue )
		{
			return setB32F< PFT >( buffer, value );
		}
		else if constexpr ( PCT == PixelComponent::eAlpha )
		{
			return setA32F< PFT >( buffer, value );
		}
		else if constexpr ( PCT == PixelComponent::eDepth )
		{
			return setD32F< PFT >( buffer, value );
		}
	}

	template< PixelFormat PFT, PixelComponent PCT >
	void setX64U( uint8_t * buffer, uint64_t value )
	{
		if constexpr ( PCT == PixelComponent::eRed )
		{
			return setR64U< PFT >( buffer, value );
		}
		else if constexpr ( PCT == PixelComponent::eGreen )
		{
			return setG64U< PFT >( buffer, value );
		}
		else if constexpr ( PCT == PixelComponent::eBlue )
		{
			return setB64U< PFT >( buffer, value );
		}
		else if constexpr ( PCT == PixelComponent::eAlpha )
		{
			return setA64U< PFT >( buffer, value );
		}
	}

	template< PixelFormat PFT, PixelComponent PCT >
	void setX64S( uint8_t * buffer, int64_t value )
	{
		if constexpr ( PCT == PixelComponent::eRed )
		{
			return setR64S< PFT >( buffer, value );
		}
		else if constexpr ( PCT == PixelComponent::eGreen )
		{
			return setG64S< PFT >( buffer, value );
		}
		else if constexpr ( PCT == PixelComponent::eBlue )
		{
			return setB64S< PFT >( buffer, value );
		}
		else if constexpr ( PCT == PixelComponent::eAlpha )
		{
			return setA64S< PFT >( buffer, value );
		}
	}

	template< PixelFormat PFT, PixelComponent PCT >
	void setX64F( uint8_t * buffer, double value )
	{
		if constexpr ( PCT == PixelComponent::eRed )
		{
			return setR64F< PFT >( buffer, value );
		}
		else if constexpr ( PCT == PixelComponent::eGreen )
		{
			return setG64F< PFT >( buffer, value );
		}
		else if constexpr ( PCT == PixelComponent::eBlue )
		{
			return setB64F< PFT >( buffer, value );
		}
		else if constexpr ( PCT == PixelComponent::eAlpha )
		{
			return setA64F< PFT >( buffer, value );
		}
	}

	//*************************************************************************************************

	template< PixelFormat PFT >
	uint8_t getR8U( uint8_t const * buffer )
	{
		return details::PxComponentGetter< uint8_t >::get( buffer, PixelComponentsT< PFT >::R );
	}

	template< PixelFormat PFT >
	int8_t getR8S( uint8_t const * buffer )
	{
		return details::PxComponentGetter< int8_t >::get( buffer, PixelComponentsT< PFT >::R );
	}

	template< PixelFormat PFT >
	uint16_t getR16U( uint8_t const * buffer )
	{
		return details::PxComponentGetter< uint16_t >::get( buffer, PixelComponentsT< PFT >::R );
	}

	template< PixelFormat PFT >
	int16_t getR16S( uint8_t const * buffer )
	{
		return details::PxComponentGetter< int16_t >::get( buffer, PixelComponentsT< PFT >::R );
	}

	template< PixelFormat PFT >
	int16_t getR16F( uint8_t const * buffer )
	{
		return details::PxComponentGetter< int16_t >::get( buffer, PixelComponentsT< PFT >::R );
	}

	template< PixelFormat PFT >
	uint32_t getR32U( uint8_t const * buffer )
	{
		return details::PxComponentGetter< uint32_t >::get( buffer, PixelComponentsT< PFT >::R );
	}

	template< PixelFormat PFT >
	int32_t getR32S( uint8_t const * buffer )
	{
		return details::PxComponentGetter< int32_t >::get( buffer, PixelComponentsT< PFT >::R );
	}

	template< PixelFormat PFT >
	float getR32F( uint8_t const * buffer )
	{
		return details::PxComponentGetter< float >::get( buffer, PixelComponentsT< PFT >::R );
	}

	template< PixelFormat PFT >
	uint64_t getR64U( uint8_t const * buffer )
	{
		return details::PxComponentGetter< uint64_t >::get( buffer, PixelComponentsT< PFT >::R );
	}

	template< PixelFormat PFT >
	int64_t getR64S( uint8_t const * buffer )
	{
		return details::PxComponentGetter< int64_t >::get( buffer, PixelComponentsT< PFT >::R );
	}

	template< PixelFormat PFT >
	double getR64F( uint8_t const * buffer )
	{
		return details::PxComponentGetter< double >::get( buffer, PixelComponentsT< PFT >::R );
	}

	//*************************************************************************************************

	template< PixelFormat PFT >
	uint8_t getG8U( uint8_t const * buffer )
	{
		return details::PxComponentGetter< uint8_t >::get( buffer, PixelComponentsT< PFT >::G );
	}

	template< PixelFormat PFT >
	int8_t getG8S( uint8_t const * buffer )
	{
		return details::PxComponentGetter< int8_t >::get( buffer, PixelComponentsT< PFT >::G );
	}

	template< PixelFormat PFT >
	uint16_t getG16U( uint8_t const * buffer )
	{
		return details::PxComponentGetter< uint16_t >::get( buffer, PixelComponentsT< PFT >::G );
	}

	template< PixelFormat PFT >
	int16_t getG16S( uint8_t const * buffer )
	{
		return details::PxComponentGetter< int16_t >::get( buffer, PixelComponentsT< PFT >::G );
	}

	template< PixelFormat PFT >
	int16_t getG16F( uint8_t const * buffer )
	{
		return details::PxComponentGetter< int16_t >::get( buffer, PixelComponentsT< PFT >::G );
	}

	template< PixelFormat PFT >
	uint32_t getG32U( uint8_t const * buffer )
	{
		return details::PxComponentGetter< uint32_t >::get( buffer, PixelComponentsT< PFT >::G );
	}

	template< PixelFormat PFT >
	int32_t getG32S( uint8_t const * buffer )
	{
		return details::PxComponentGetter< int32_t >::get( buffer, PixelComponentsT< PFT >::G );
	}

	template< PixelFormat PFT >
	float getG32F( uint8_t const * buffer )
	{
		return details::PxComponentGetter< float >::get( buffer, PixelComponentsT< PFT >::G );
	}

	template< PixelFormat PFT >
	uint64_t getG64U( uint8_t const * buffer )
	{
		return details::PxComponentGetter< uint64_t >::get( buffer, PixelComponentsT< PFT >::G );
	}

	template< PixelFormat PFT >
	int64_t getG64S( uint8_t const * buffer )
	{
		return details::PxComponentGetter< int64_t >::get( buffer, PixelComponentsT< PFT >::G );
	}

	template< PixelFormat PFT >
	double getG64F( uint8_t const * buffer )
	{
		return details::PxComponentGetter< double >::get( buffer, PixelComponentsT< PFT >::G );
	}

	//*************************************************************************************************

	template< PixelFormat PFT >
	uint8_t getB8U( uint8_t const * buffer )
	{
		return details::PxComponentGetter< uint8_t >::get( buffer, PixelComponentsT< PFT >::B );
	}

	template< PixelFormat PFT >
	int8_t getB8S( uint8_t const * buffer )
	{
		return details::PxComponentGetter< int8_t >::get( buffer, PixelComponentsT< PFT >::B );
	}

	template< PixelFormat PFT >
	uint16_t getB16U( uint8_t const * buffer )
	{
		return details::PxComponentGetter< uint16_t >::get( buffer, PixelComponentsT< PFT >::B );
	}

	template< PixelFormat PFT >
	int16_t getB16S( uint8_t const * buffer )
	{
		return details::PxComponentGetter< int16_t >::get( buffer, PixelComponentsT< PFT >::B );
	}

	template< PixelFormat PFT >
	int16_t getB16F( uint8_t const * buffer )
	{
		return details::PxComponentGetter< int16_t >::get( buffer, PixelComponentsT< PFT >::B );
	}

	template< PixelFormat PFT >
	uint32_t getB32U( uint8_t const * buffer )
	{
		return details::PxComponentGetter< uint32_t >::get( buffer, PixelComponentsT< PFT >::B );
	}

	template< PixelFormat PFT >
	int32_t getB32S( uint8_t const * buffer )
	{
		return details::PxComponentGetter< int32_t >::get( buffer, PixelComponentsT< PFT >::B );
	}

	template< PixelFormat PFT >
	float getB32F( uint8_t const * buffer )
	{
		return details::PxComponentGetter< float >::get( buffer, PixelComponentsT< PFT >::B );
	}

	template< PixelFormat PFT >
	uint64_t getB64U( uint8_t const * buffer )
	{
		return details::PxComponentGetter< uint64_t >::get( buffer, PixelComponentsT< PFT >::B );
	}

	template< PixelFormat PFT >
	int64_t getB64S( uint8_t const * buffer )
	{
		return details::PxComponentGetter< int64_t >::get( buffer, PixelComponentsT< PFT >::B );
	}

	template< PixelFormat PFT >
	double getB64F( uint8_t const * buffer )
	{
		return details::PxComponentGetter< double >::get( buffer, PixelComponentsT< PFT >::B );
	}

	//*************************************************************************************************

	template< PixelFormat PFT >
	uint8_t getA8U( uint8_t const * buffer )
	{
		return details::PxComponentGetter< uint8_t >::get( buffer, PixelComponentsT< PFT >::A );
	}

	template< PixelFormat PFT >
	int8_t getA8S( uint8_t const * buffer )
	{
		return details::PxComponentGetter< int8_t >::get( buffer, PixelComponentsT< PFT >::A );
	}

	template< PixelFormat PFT >
	uint16_t getA16U( uint8_t const * buffer )
	{
		return details::PxComponentGetter< uint16_t >::get( buffer, PixelComponentsT< PFT >::A );
	}

	template< PixelFormat PFT >
	int16_t getA16S( uint8_t const * buffer )
	{
		return details::PxComponentGetter< int16_t >::get( buffer, PixelComponentsT< PFT >::A );
	}

	template< PixelFormat PFT >
	int16_t getA16F( uint8_t const * buffer )
	{
		return details::PxComponentGetter< int16_t >::get( buffer, PixelComponentsT< PFT >::A );
	}

	template< PixelFormat PFT >
	uint32_t getA32U( uint8_t const * buffer )
	{
		return details::PxComponentGetter< uint32_t >::get( buffer, PixelComponentsT< PFT >::A );
	}

	template< PixelFormat PFT >
	int32_t getA32S( uint8_t const * buffer )
	{
		return details::PxComponentGetter< int32_t >::get( buffer, PixelComponentsT< PFT >::A );
	}

	template< PixelFormat PFT >
	float getA32F( uint8_t const * buffer )
	{
		return details::PxComponentGetter< float >::get( buffer, PixelComponentsT< PFT >::A );
	}

	template< PixelFormat PFT >
	uint64_t getA64U( uint8_t const * buffer )
	{
		return details::PxComponentGetter< uint64_t >::get( buffer, PixelComponentsT< PFT >::A );
	}

	template< PixelFormat PFT >
	int64_t getA64S( uint8_t const * buffer )
	{
		return details::PxComponentGetter< int64_t >::get( buffer, PixelComponentsT< PFT >::A );
	}

	template< PixelFormat PFT >
	double getA64F( uint8_t const * buffer )
	{
		return details::PxComponentGetter< double >::get( buffer, PixelComponentsT< PFT >::A );
	}

	//*************************************************************************************************

	template< PixelFormat PFT >
	uint16_t getD16U( uint8_t const * buffer )
	{
		return details::PxComponentGetter< uint16_t >::get( buffer, PixelComponentsT< PFT >::D );
	}

	template< PixelFormat PFT >
	uint32_t getD24U( uint8_t const * buffer )
	{
		return details::PxComponentGetter< uint32_t >::get( buffer, PixelComponentsT< PFT >::D );
	}

	template< PixelFormat PFT >
	float getD32F( uint8_t const * buffer )
	{
		return details::PxComponentGetter< float >::get( buffer, PixelComponentsT< PFT >::D );
	}

	//*************************************************************************************************

	template< PixelFormat PFT >
	uint8_t getS8U( uint8_t const * buffer )
	{
		return details::PxComponentGetter< uint8_t >::get( buffer, PixelComponentsT< PFT >::S );
	}

	//*************************************************************************************************

	template< PixelFormat PFT >
	void setR8U( uint8_t * buffer, uint8_t value )
	{
		details::PxComponentSetter< uint8_t >::set( buffer, value, PixelComponentsT< PFT >::R );
	}

	template< PixelFormat PFT >
	void setR8S( uint8_t * buffer, int8_t value )
	{
		details::PxComponentSetter< int8_t >::set( buffer, value, PixelComponentsT< PFT >::R );
	}

	template< PixelFormat PFT >
	void setR16U( uint8_t * buffer, uint16_t value )
	{
		details::PxComponentSetter< uint16_t >::set( buffer, value, PixelComponentsT< PFT >::R );
	}

	template< PixelFormat PFT >
	void setR16S( uint8_t * buffer, int16_t value )
	{
		details::PxComponentSetter< int16_t >::set( buffer, value, PixelComponentsT< PFT >::R );
	}

	template< PixelFormat PFT >
	void setR16F( uint8_t * buffer, int16_t value )
	{
		details::PxComponentSetter< int16_t >::set( buffer, value, PixelComponentsT< PFT >::R );
	}

	template< PixelFormat PFT >
	void setR32S( uint8_t * buffer, uint32_t value )
	{
		details::PxComponentSetter< uint32_t >::set( buffer, value, PixelComponentsT< PFT >::R );
	}

	template< PixelFormat PFT >
	void setR32U( uint8_t * buffer, int32_t value )
	{
		details::PxComponentSetter< int32_t >::set( buffer, value, PixelComponentsT< PFT >::R );
	}

	template< PixelFormat PFT >
	void setR32F( uint8_t * buffer, float value )
	{
		details::PxComponentSetter< float >::set( buffer, value, PixelComponentsT< PFT >::R );
	}

	template< PixelFormat PFT >
	void setR64U( uint8_t * buffer, uint64_t value )
	{
		details::PxComponentSetter< uint64_t >::set( buffer, value, PixelComponentsT< PFT >::R );
	}

	template< PixelFormat PFT >
	void setR64S( uint8_t * buffer, int64_t value )
	{
		details::PxComponentSetter< int64_t >::set( buffer, value, PixelComponentsT< PFT >::R );
	}

	template< PixelFormat PFT >
	void setR64F( uint8_t * buffer, double value )
	{
		details::PxComponentSetter< double >::set( buffer, value, PixelComponentsT< PFT >::R );
	}

	//*************************************************************************************************

	template< PixelFormat PFT >
	void setG8U( uint8_t * buffer, uint8_t value )
	{
		details::PxComponentSetter< uint8_t >::set( buffer, value, PixelComponentsT< PFT >::G );
	}

	template< PixelFormat PFT >
	void setG8S( uint8_t * buffer, int8_t value )
	{
		details::PxComponentSetter< int8_t >::set( buffer, value, PixelComponentsT< PFT >::G );
	}

	template< PixelFormat PFT >
	void setG16U( uint8_t * buffer, uint16_t value )
	{
		details::PxComponentSetter< uint16_t >::set( buffer, value, PixelComponentsT< PFT >::G );
	}

	template< PixelFormat PFT >
	void setG16S( uint8_t * buffer, int16_t value )
	{
		details::PxComponentSetter< int16_t >::set( buffer, value, PixelComponentsT< PFT >::G );
	}

	template< PixelFormat PFT >
	void setG16F( uint8_t * buffer, int16_t value )
	{
		details::PxComponentSetter< int16_t >::set( buffer, value, PixelComponentsT< PFT >::G );
	}

	template< PixelFormat PFT >
	void setG32U( uint8_t * buffer, uint32_t value )
	{
		details::PxComponentSetter< uint32_t >::set( buffer, value, PixelComponentsT< PFT >::G );
	}

	template< PixelFormat PFT >
	void setG32S( uint8_t * buffer, int32_t value )
	{
		details::PxComponentSetter< int32_t >::set( buffer, value, PixelComponentsT< PFT >::G );
	}

	template< PixelFormat PFT >
	void setG32F( uint8_t * buffer, float value )
	{
		details::PxComponentSetter< float >::set( buffer, value, PixelComponentsT< PFT >::G );
	}

	template< PixelFormat PFT >
	void setG64U( uint8_t * buffer, uint64_t value )
	{
		details::PxComponentSetter< uint64_t >::set( buffer, value, PixelComponentsT< PFT >::G );
	}

	template< PixelFormat PFT >
	void setG64S( uint8_t * buffer, int64_t value )
	{
		details::PxComponentSetter< int64_t >::set( buffer, value, PixelComponentsT< PFT >::G );
	}

	template< PixelFormat PFT >
	void setG64F( uint8_t * buffer, double value )
	{
		details::PxComponentSetter< double >::set( buffer, value, PixelComponentsT< PFT >::G );
	}

	//*************************************************************************************************

	template< PixelFormat PFT >
	void setB8U( uint8_t * buffer, uint8_t value )
	{
		details::PxComponentSetter< uint8_t >::set( buffer, value, PixelComponentsT< PFT >::B );
	}

	template< PixelFormat PFT >
	void setB8S( uint8_t * buffer, int8_t value )
	{
		details::PxComponentSetter< int8_t >::set( buffer, value, PixelComponentsT< PFT >::B );
	}

	template< PixelFormat PFT >
	void setB16U( uint8_t * buffer, uint16_t value )
	{
		details::PxComponentSetter< uint16_t >::set( buffer, value, PixelComponentsT< PFT >::B );
	}

	template< PixelFormat PFT >
	void setB16S( uint8_t * buffer, int16_t value )
	{
		details::PxComponentSetter< int16_t >::set( buffer, value, PixelComponentsT< PFT >::B );
	}

	template< PixelFormat PFT >
	void setB16F( uint8_t * buffer, int16_t value )
	{
		details::PxComponentSetter< int16_t >::set( buffer, value, PixelComponentsT< PFT >::B );
	}

	template< PixelFormat PFT >
	void setB32U( uint8_t * buffer, uint32_t value )
	{
		details::PxComponentSetter< uint32_t >::set( buffer, value, PixelComponentsT< PFT >::B );
	}

	template< PixelFormat PFT >
	void setB32S( uint8_t * buffer, int32_t value )
	{
		details::PxComponentSetter< int32_t >::set( buffer, value, PixelComponentsT< PFT >::B );
	}

	template< PixelFormat PFT >
	void setB32F( uint8_t * buffer, float value )
	{
		details::PxComponentSetter< float >::set( buffer, value, PixelComponentsT< PFT >::B );
	}

	template< PixelFormat PFT >
	void setB64U( uint8_t * buffer, uint64_t value )
	{
		details::PxComponentSetter< uint64_t >::set( buffer, value, PixelComponentsT< PFT >::B );
	}

	template< PixelFormat PFT >
	void setB64S( uint8_t * buffer, int64_t value )
	{
		details::PxComponentSetter< int64_t >::set( buffer, value, PixelComponentsT< PFT >::B );
	}

	template< PixelFormat PFT >
	void setB64F( uint8_t * buffer, double value )
	{
		details::PxComponentSetter< double >::set( buffer, value, PixelComponentsT< PFT >::B );
	}

	//*************************************************************************************************

	template< PixelFormat PFT >
	void setA8U( uint8_t * buffer, uint8_t value )
	{
		details::PxComponentSetter< uint8_t >::set( buffer, value, PixelComponentsT< PFT >::A );
	}

	template< PixelFormat PFT >
	void setA8S( uint8_t * buffer, int8_t value )
	{
		details::PxComponentSetter< int8_t >::set( buffer, value, PixelComponentsT< PFT >::A );
	}

	template< PixelFormat PFT >
	void setA16U( uint8_t * buffer, uint16_t value )
	{
		details::PxComponentSetter< uint16_t >::set( buffer, value, PixelComponentsT< PFT >::A );
	}

	template< PixelFormat PFT >
	void setA16S( uint8_t * buffer, int16_t value )
	{
		details::PxComponentSetter< int16_t >::set( buffer, value, PixelComponentsT< PFT >::A );
	}

	template< PixelFormat PFT >
	void setA16F( uint8_t * buffer, int16_t value )
	{
		details::PxComponentSetter< int16_t >::set( buffer, value, PixelComponentsT< PFT >::A );
	}

	template< PixelFormat PFT >
	void setA32U( uint8_t * buffer, uint32_t value )
	{
		details::PxComponentSetter< uint32_t >::set( buffer, value, PixelComponentsT< PFT >::A );
	}

	template< PixelFormat PFT >
	void setA32S( uint8_t * buffer, int32_t value )
	{
		details::PxComponentSetter< int32_t >::set( buffer, value, PixelComponentsT< PFT >::A );
	}

	template< PixelFormat PFT >
	void setA32F( uint8_t * buffer, float value )
	{
		details::PxComponentSetter< float >::set( buffer, value, PixelComponentsT< PFT >::A );
	}

	template< PixelFormat PFT >
	void setA64U( uint8_t * buffer, uint64_t value )
	{
		details::PxComponentSetter< uint64_t >::set( buffer, value, PixelComponentsT< PFT >::A );
	}

	template< PixelFormat PFT >
	void setA64S( uint8_t * buffer, int64_t value )
	{
		details::PxComponentSetter< int64_t >::set( buffer, value, PixelComponentsT< PFT >::A );
	}

	template< PixelFormat PFT >
	void setA64F( uint8_t * buffer, double value )
	{
		details::PxComponentSetter< double >::set( buffer, value, PixelComponentsT< PFT >::A );
	}

	//*************************************************************************************************

	template< PixelFormat PFT >
	void setD16U( uint8_t * buffer, uint16_t value )
	{
		details::PxComponentSetter< uint16_t >::set( buffer, value, PixelComponentsT< PFT >::D );
	}

	template< PixelFormat PFT >
	void setD24U( uint8_t * buffer, uint32_t value )
	{
		details::PxComponentSetter< uint32_t >::set( buffer, value, PixelComponentsT< PFT >::D );
	}

	template< PixelFormat PFT >
	void setD32F( uint8_t * buffer, float value )
	{
		details::PxComponentSetter< float >::set( buffer, value, PixelComponentsT< PFT >::D );
	}

	//*************************************************************************************************

	template< PixelFormat PFT >
	void setS8U( uint8_t * buffer, uint8_t value )
	{
		details::PxComponentSetter< uint8_t >::set( buffer, value, PixelComponentsT< PFT >::S );
	}

	//*************************************************************************************************
}

#pragma GCC diagnostic pop

#pragma warning( pop )
