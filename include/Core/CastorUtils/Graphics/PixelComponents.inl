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

	//!\~english	Specialisation for PixelFormat::eR4G4_UNORM
	//!\~french		Spécialisation pour PixelFormat::eR4G4_UNORM
	template<>
	struct PixelComponents< PixelFormat::eR4G4_UNORM >
	{
		using Type = uint8_t;
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
		static Type B( uint8_t const * buffer )
		{
			return 0x00u;
		}
		static Type A( uint8_t const * buffer )
		{
			return std::numeric_limits< Type >::max();
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value >> 4;
		}
		static void G( uint8_t * buffer, Type value )
		{
			getDst( buffer ).g = value >> 4;
		}
		static void B( uint8_t * buffer, Type value )
		{
		}
		static void A( uint8_t * buffer, Type value )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR4G4B4A4_UNORM
	//!\~french		Spécialisation pour PixelFormat::eR4G4B4A4_UNORM
	template<>
	struct PixelComponents< PixelFormat::eR4G4B4A4_UNORM >
	{
		using Type = uint8_t;
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
			getDst( buffer ).r = value >> 4;
		}
		static void G( uint8_t * buffer, Type value )
		{
			getDst( buffer ).g = value >> 4;
		}
		static void B( uint8_t * buffer, Type value )
		{
			getDst( buffer ).b = value >> 4;
		}
		static void A( uint8_t * buffer, Type value )
		{
			getDst( buffer ).a = value >> 4;
		}
	};
	//!\~english	Specialisation for PixelFormat::eB4G4R4A4_UNORM
	//!\~french		Spécialisation pour PixelFormat::eB4G4R4A4_UNORM
	template<>
	struct PixelComponents< PixelFormat::eB4G4R4A4_UNORM >
	{
		using Type = uint8_t;
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
			getDst( buffer ).r = value >> 4;
		}
		static void G( uint8_t * buffer, Type value )
		{
			getDst( buffer ).g = value >> 4;
		}
		static void B( uint8_t * buffer, Type value )
		{
			getDst( buffer ).b = value >> 4;
		}
		static void A( uint8_t * buffer, Type value )
		{
			getDst( buffer ).a = value >> 4;
		}
	};
	//!\~english	Specialisation for PixelFormat::eR5G6B5_UNORM
	//!\~french		Spécialisation pour PixelFormat::eR5G6B5_UNORM
	template<>
	struct PixelComponents< PixelFormat::eR5G6B5_UNORM >
	{
		using Type = uint8_t;
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
		static Type A( uint8_t const * buffer )
		{
			return std::numeric_limits< Type >::max();
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value >> 3;
		}
		static void G( uint8_t * buffer, Type value )
		{
			getDst( buffer ).g = value >> 2;
		}
		static void B( uint8_t * buffer, Type value )
		{
			getDst( buffer ).b = value >> 3;
		}
		static void A( uint8_t * buffer, Type value )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eB5G6R5_UNORM
	//!\~french		Spécialisation pour PixelFormat::eB5G6R5_UNORM
	template<>
	struct PixelComponents< PixelFormat::eB5G6R5_UNORM >
	{
		using Type = uint8_t;
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
		static Type A( uint8_t const * buffer )
		{
			return 0xFF;
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value >> 3;
		}
		static void G( uint8_t * buffer, Type value )
		{
			getDst( buffer ).g = value >> 2;
		}
		static void B( uint8_t * buffer, Type value )
		{
			getDst( buffer ).b = value >> 3;
		}
		static void A( uint8_t * buffer, Type value )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR5G5B5A1_UNORM
	//!\~french		Spécialisation pour PixelFormat::eR5G5B5A1_UNORM
	template<>
	struct PixelComponents< PixelFormat::eR5G5B5A1_UNORM >
	{
		using Type = uint8_t;
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
			return getSrc( buffer ).a << 7;
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value >> 3;
		}
		static void G( uint8_t * buffer, Type value )
		{
			getDst( buffer ).g = value >> 3;
		}
		static void B( uint8_t * buffer, Type value )
		{
			getDst( buffer ).b = value >> 3;
		}
		static void A( uint8_t * buffer, Type value )
		{
			getDst( buffer ).a = ( value != 0 ) ? 0x01 : 0x00;
		}
	};
	//!\~english	Specialisation for PixelFormat::eB5G5R5A1_UNORM
	//!\~french		Spécialisation pour PixelFormat::eB5G5R5A1_UNORM
	template<>
	struct PixelComponents< PixelFormat::eB5G5R5A1_UNORM >
	{
		using Type = uint8_t;
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
			getDst( buffer ).r = value >> 3;
		}
		static void G( uint8_t * buffer, Type value )
		{
			getDst( buffer ).g = value >> 3;
		}
		static void B( uint8_t * buffer, Type value )
		{
			getDst( buffer ).b = value >> 3;
		}
		static void A( uint8_t * buffer, Type value )
		{
			getDst( buffer ).a = ( value != 0 ) ? 0x01 : 0x00;
		}
	};
	//!\~english	Specialisation for PixelFormat::eA1R5G5B5_UNORM
	//!\~french		Spécialisation pour PixelFormat::eA1R5G5B5_UNORM
	template<>
	struct PixelComponents< PixelFormat::eA1R5G5B5_UNORM >
	{
		using Type = uint8_t;
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
			getDst( buffer ).r = value >> 3;
		}
		static void G( uint8_t * buffer, Type value )
		{
			getDst( buffer ).g = value >> 3;
		}
		static void B( uint8_t * buffer, Type value )
		{
			getDst( buffer ).b = value >> 3;
		}
		static void A( uint8_t * buffer, Type value )
		{
			getDst( buffer ).a = ( value != 0 ) ? 0x01 : 0x00;
		}
	};
	//!\~english	Specialisation for PixelFormat::eR8_UNORM
	//!\~french		Spécialisation pour PixelFormat::eR8_UNORM
	template<>
	struct PixelComponents< PixelFormat::eR8_UNORM >
	{
		using Type = uint8_t;
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
		static Type G( uint8_t const * buffer )
		{
			return 0x00;
		}
		static Type B( uint8_t const * buffer )
		{
			return 0x00;
		}
		static Type A( uint8_t const * buffer )
		{
			return std::numeric_limits< Type >::max();
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value;
		}
		static void G( uint8_t * buffer, Type value )
		{
		}
		static void B( uint8_t * buffer, Type value )
		{
		}
		static void A( uint8_t * buffer, Type value )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR8_SNORM
	//!\~french		Spécialisation pour PixelFormat::eR8_SNORM
	template<>
	struct PixelComponents< PixelFormat::eR8_SNORM >
	{
		using Type = int8_t;
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
		static Type G( uint8_t const * buffer )
		{
			return 0x00;
		}
		static Type B( uint8_t const * buffer )
		{
			return 0x00;
		}
		static Type A( uint8_t const * buffer )
		{
			return std::numeric_limits< Type >::max();
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value;
		}
		static void G( uint8_t * buffer, Type value )
		{
		}
		static void B( uint8_t * buffer, Type value )
		{
		}
		static void A( uint8_t * buffer, Type value )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR8_USCALED
	//!\~french		Spécialisation pour PixelFormat::eR8_USCALED
	template<>
	struct PixelComponents< PixelFormat::eR8_USCALED >
		: public PixelComponents< PixelFormat::eR8_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR8_SSCALED
	//!\~french		Spécialisation pour PixelFormat::eR8_SSCALED
	template<>
	struct PixelComponents< PixelFormat::eR8_SSCALED >
		: public PixelComponents< PixelFormat::eR8_SNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR8_UINT
	//!\~french		Spécialisation pour PixelFormat::eR8_UINT
	template<>
	struct PixelComponents< PixelFormat::eR8_UINT >
		: public PixelComponents< PixelFormat::eR8_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR8_SINT
	//!\~french		Spécialisation pour PixelFormat::eR8_SINT
	template<>
	struct PixelComponents< PixelFormat::eR8_SINT >
		: public PixelComponents< PixelFormat::eR8_SNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR8_SRGB
	//!\~french		Spécialisation pour PixelFormat::eR8_SRGB
	template<>
	struct PixelComponents< PixelFormat::eR8_SRGB >
		: public PixelComponents< PixelFormat::eR8_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR8G8_UNORM
	//!\~french		Spécialisation pour PixelFormat::eR8G8_UNORM
	template<>
	struct PixelComponents< PixelFormat::eR8G8_UNORM >
	{
		using Type = uint8_t;
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
		static Type B( uint8_t const * buffer )
		{
			return 0x00;
		}
		static Type A( uint8_t const * buffer )
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
		}
		static void A( uint8_t * buffer, Type value )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR8G8_SNORM
	//!\~french		Spécialisation pour PixelFormat::eR8G8_SNORM
	template<>
	struct PixelComponents< PixelFormat::eR8G8_SNORM >
	{
		using Type = int8_t;
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
		static Type B( uint8_t const * buffer )
		{
			return 0x00;
		}
		static Type A( uint8_t const * buffer )
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
		}
		static void A( uint8_t * buffer, Type value )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR8G8_USCALED
	//!\~french		Spécialisation pour PixelFormat::eR8G8_USCALED
	template<>
	struct PixelComponents< PixelFormat::eR8G8_USCALED >
		: public PixelComponents< PixelFormat::eR8G8_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR8G8_SSCALED
	//!\~french		Spécialisation pour PixelFormat::eR8G8_SSCALED
	template<>
	struct PixelComponents< PixelFormat::eR8G8_SSCALED >
		: public PixelComponents< PixelFormat::eR8G8_SNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR8G8_UINT
	//!\~french		Spécialisation pour PixelFormat::eR8G8_UINT
	template<>
	struct PixelComponents< PixelFormat::eR8G8_UINT >
		: public PixelComponents< PixelFormat::eR8G8_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR8G8_SINT
	//!\~french		Spécialisation pour PixelFormat::eR8G8_SINT
	template<>
	struct PixelComponents< PixelFormat::eR8G8_SINT >
		: public PixelComponents< PixelFormat::eR8G8_SNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR8G8_SRGB
	//!\~french		Spécialisation pour PixelFormat::eR8G8_SRGB
	template<>
	struct PixelComponents< PixelFormat::eR8G8_SRGB >
		: public PixelComponents< PixelFormat::eR8G8_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR8G8B8_UNORM
	//!\~french		Spécialisation pour PixelFormat::eR8G8B8_UNORM
	template<>
	struct PixelComponents< PixelFormat::eR8G8B8_UNORM >
	{
		using Type = uint8_t;
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
		static Type A( uint8_t const * buffer )
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
		static void A( uint8_t * buffer, Type value )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR8G8B8_SNORM
	//!\~french		Spécialisation pour PixelFormat::eR8G8B8_SNORM
	template<>
	struct PixelComponents< PixelFormat::eR8G8B8_SNORM >
	{
		using Type = int8_t;
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
		static Type A( uint8_t const * buffer )
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
		static void A( uint8_t * buffer, Type value )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR8G8B8_USCALED
	//!\~french		Spécialisation pour PixelFormat::eR8G8B8_USCALED
	template<>
	struct PixelComponents< PixelFormat::eR8G8B8_USCALED >
		: public PixelComponents< PixelFormat::eR8G8B8_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR8G8B8_SSCALED
	//!\~french		Spécialisation pour PixelFormat::eR8G8B8_SSCALED
	template<>
	struct PixelComponents< PixelFormat::eR8G8B8_SSCALED >
		: public PixelComponents< PixelFormat::eR8G8B8_SNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR8G8B8_UINT
	//!\~french		Spécialisation pour PixelFormat::eR8G8B8_UINT
	template<>
	struct PixelComponents< PixelFormat::eR8G8B8_UINT >
		: public PixelComponents< PixelFormat::eR8G8B8_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR8G8B8_SINT
	//!\~french		Spécialisation pour PixelFormat::eR8G8B8_SINT
	template<>
	struct PixelComponents< PixelFormat::eR8G8B8_SINT >
		: public PixelComponents< PixelFormat::eR8G8B8_SNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR8G8B8_SRGB
	//!\~french		Spécialisation pour PixelFormat::eR8G8B8_SRGB
	template<>
	struct PixelComponents< PixelFormat::eR8G8B8_SRGB >
		: public PixelComponents< PixelFormat::eR8G8B8_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eB8G8R8_UNORM
	//!\~french		Spécialisation pour PixelFormat::eB8G8R8_UNORM
	template<>
	struct PixelComponents< PixelFormat::eB8G8R8_UNORM >
	{
		using Type = uint8_t;
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
		static Type A( uint8_t const * buffer )
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
		static void A( uint8_t * buffer, Type value )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eB8G8R8_SNORM
	//!\~french		Spécialisation pour PixelFormat::eB8G8R8_SNORM
	template<>
	struct PixelComponents< PixelFormat::eB8G8R8_SNORM >
	{
		using Type = int8_t;
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
		static Type A( uint8_t const * buffer )
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
		static void A( uint8_t * buffer, Type value )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eB8G8R8_USCALED
	//!\~french		Spécialisation pour PixelFormat::eB8G8R8_USCALED
	template<>
	struct PixelComponents< PixelFormat::eB8G8R8_USCALED >
		: public PixelComponents< PixelFormat::eB8G8R8_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eB8G8R8_SSCALED
	//!\~french		Spécialisation pour PixelFormat::eB8G8R8_SSCALED
	template<>
	struct PixelComponents< PixelFormat::eB8G8R8_SSCALED >
		: public PixelComponents< PixelFormat::eB8G8R8_SNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eB8G8R8_UINT
	//!\~french		Spécialisation pour PixelFormat::eB8G8R8_UINT
	template<>
	struct PixelComponents< PixelFormat::eB8G8R8_UINT >
		: public PixelComponents< PixelFormat::eB8G8R8_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eB8G8R8_SINT
	//!\~french		Spécialisation pour PixelFormat::eB8G8R8_SINT
	template<>
	struct PixelComponents< PixelFormat::eB8G8R8_SINT >
		: public PixelComponents< PixelFormat::eB8G8R8_SNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eB8G8R8_SRGB
	//!\~french		Spécialisation pour PixelFormat::eB8G8R8_SRGB
	template<>
	struct PixelComponents< PixelFormat::eB8G8R8_SRGB >
		: public PixelComponents< PixelFormat::eB8G8R8_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR8G8B8A8_UNORM
	//!\~french		Spécialisation pour PixelFormat::eR8G8B8A8_UNORM
	template<>
	struct PixelComponents< PixelFormat::eR8G8B8A8_UNORM >
	{
		using Type = uint8_t;
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
	struct PixelComponents< PixelFormat::eR8G8B8A8_SNORM >
	{
		using Type = int8_t;
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
	struct PixelComponents< PixelFormat::eR8G8B8A8_USCALED >
		: public PixelComponents< PixelFormat::eR8G8B8A8_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR8G8B8A8_SSCALED
	//!\~french		Spécialisation pour PixelFormat::eR8G8B8A8_SSCALED
	template<>
	struct PixelComponents< PixelFormat::eR8G8B8A8_SSCALED >
		: public PixelComponents< PixelFormat::eR8G8B8A8_SNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR8G8B8A8_UINT
	//!\~french		Spécialisation pour PixelFormat::eR8G8B8A8_UINT
	template<>
	struct PixelComponents< PixelFormat::eR8G8B8A8_UINT >
		: public PixelComponents< PixelFormat::eR8G8B8A8_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR8G8B8A8_SINT
	//!\~french		Spécialisation pour PixelFormat::eR8G8B8A8_SINT
	template<>
	struct PixelComponents< PixelFormat::eR8G8B8A8_SINT >
		: public PixelComponents< PixelFormat::eR8G8B8A8_SNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR8G8B8A8_SRGB
	//!\~french		Spécialisation pour PixelFormat::eR8G8B8A8_SRGB
	template<>
	struct PixelComponents< PixelFormat::eR8G8B8A8_SRGB >
		: public PixelComponents< PixelFormat::eR8G8B8A8_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eB8G8R8A8_UNORM
	//!\~french		Spécialisation pour PixelFormat::eB8G8R8A8_UNORM
	template<>
	struct PixelComponents< PixelFormat::eB8G8R8A8_UNORM >
	{
		using Type = uint8_t;
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
	struct PixelComponents< PixelFormat::eB8G8R8A8_SNORM >
	{
		using Type = int8_t;
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
	struct PixelComponents< PixelFormat::eB8G8R8A8_USCALED >
		: public PixelComponents< PixelFormat::eB8G8R8A8_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eB8G8R8A8_SSCALED
	//!\~french		Spécialisation pour PixelFormat::eB8G8R8A8_SSCALED
	template<>
	struct PixelComponents< PixelFormat::eB8G8R8A8_SSCALED >
		: public PixelComponents< PixelFormat::eB8G8R8A8_SNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eB8G8R8A8_UINT
	//!\~french		Spécialisation pour PixelFormat::eB8G8R8A8_UINT
	template<>
	struct PixelComponents< PixelFormat::eB8G8R8A8_UINT >
		: public PixelComponents< PixelFormat::eB8G8R8A8_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eB8G8R8A8_SINT
	//!\~french		Spécialisation pour PixelFormat::eB8G8R8A8_SINT
	template<>
	struct PixelComponents< PixelFormat::eB8G8R8A8_SINT >
		: public PixelComponents< PixelFormat::eB8G8R8A8_SNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eB8G8R8A8_SRGB
	//!\~french		Spécialisation pour PixelFormat::eB8G8R8A8_SRGB
	template<>
	struct PixelComponents< PixelFormat::eB8G8R8A8_SRGB >
		: public PixelComponents< PixelFormat::eB8G8R8A8_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eA8B8G8R8_UNORM
	//!\~french		Spécialisation pour PixelFormat::eA8B8G8R8_UNORM
	template<>
	struct PixelComponents< PixelFormat::eA8B8G8R8_UNORM >
	{
		using Type = uint8_t;
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
	struct PixelComponents< PixelFormat::eA8B8G8R8_SNORM >
	{
		using Type = int8_t;
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
	struct PixelComponents< PixelFormat::eA8B8G8R8_USCALED >
		: public PixelComponents< PixelFormat::eA8B8G8R8_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eA8B8G8R8_SSCALED
	//!\~french		Spécialisation pour PixelFormat::eA8B8G8R8_SSCALED
	template<>
	struct PixelComponents< PixelFormat::eA8B8G8R8_SSCALED >
		: public PixelComponents< PixelFormat::eA8B8G8R8_SNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eA8B8G8R8_UINT
	//!\~french		Spécialisation pour PixelFormat::eA8B8G8R8_UINT
	template<>
	struct PixelComponents< PixelFormat::eA8B8G8R8_UINT >
		: public PixelComponents< PixelFormat::eA8B8G8R8_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eA8B8G8R8_SINT
	//!\~french		Spécialisation pour PixelFormat::eA8B8G8R8_SINT
	template<>
	struct PixelComponents< PixelFormat::eA8B8G8R8_SINT >
		: public PixelComponents< PixelFormat::eA8B8G8R8_SNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eA8B8G8R8_SRGB
	//!\~french		Spécialisation pour PixelFormat::eA8B8G8R8_SRGB
	template<>
	struct PixelComponents< PixelFormat::eA8B8G8R8_SRGB >
		: public PixelComponents< PixelFormat::eA8B8G8R8_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eA2R10G10B10_UNORM
	//!\~french		Spécialisation pour PixelFormat::eA2R10G10B10_UNORM
	template<>
	struct PixelComponents< PixelFormat::eA2R10G10B10_UNORM >
	{
		using Type = uint16_t;
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
	//!\~english	Specialisation for PixelFormat::eA2R10G10B10_SNORM
	//!\~french		Spécialisation pour PixelFormat::eA2R10G10B10_SNORM
	template<>
	struct PixelComponents< PixelFormat::eA2R10G10B10_SNORM >
	{
		using Type = int16_t;
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
	struct PixelComponents< PixelFormat::eA2R10G10B10_USCALED >
		: public PixelComponents< PixelFormat::eA2R10G10B10_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eA2R10G10B10_SSCALED
	//!\~french		Spécialisation pour PixelFormat::eA2R10G10B10_SSCALED
	template<>
	struct PixelComponents< PixelFormat::eA2R10G10B10_SSCALED >
		: public PixelComponents< PixelFormat::eA2R10G10B10_SNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eA2R10G10B10_UINT
	//!\~french		Spécialisation pour PixelFormat::eA2R10G10B10_UINT
	template<>
	struct PixelComponents< PixelFormat::eA2R10G10B10_UINT >
		: public PixelComponents< PixelFormat::eA2R10G10B10_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eA2R10G10B10_SINT
	//!\~french		Spécialisation pour PixelFormat::eA2R10G10B10_SINT
	template<>
	struct PixelComponents< PixelFormat::eA2R10G10B10_SINT >
		: public PixelComponents< PixelFormat::eA2R10G10B10_SNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eA2B10G10R10_UNORM
	//!\~french		Spécialisation pour PixelFormat::eA2B10G10R10_UNORM
	template<>
	struct PixelComponents< PixelFormat::eA2B10G10R10_UNORM >
	{
		using Type = uint16_t;
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
	//!\~english	Specialisation for PixelFormat::eA2B10G10R10_SNORM
	//!\~french		Spécialisation pour PixelFormat::eA2B10G10R10_SNORM
	template<>
	struct PixelComponents< PixelFormat::eA2B10G10R10_SNORM >
	{
		using Type = int16_t;
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
	struct PixelComponents< PixelFormat::eA2B10G10R10_USCALED >
		: public PixelComponents< PixelFormat::eA2B10G10R10_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eA2B10G10R10_SSCALED
	//!\~french		Spécialisation pour PixelFormat::eA2B10G10R10_SSCALED
	template<>
	struct PixelComponents< PixelFormat::eA2B10G10R10_SSCALED >
		: public PixelComponents< PixelFormat::eA2B10G10R10_SNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eA2B10G10R10_UINT
	//!\~french		Spécialisation pour PixelFormat::eA2B10G10R10_UINT
	template<>
	struct PixelComponents< PixelFormat::eA2B10G10R10_UINT >
		: public PixelComponents< PixelFormat::eA2B10G10R10_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eA2B10G10R10_SINT
	//!\~french		Spécialisation pour PixelFormat::eA2B10G10R10_SINT
	template<>
	struct PixelComponents< PixelFormat::eA2B10G10R10_SINT >
		: public PixelComponents< PixelFormat::eA2B10G10R10_SNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR16_UNORM
	//!\~french		Spécialisation pour PixelFormat::eR16_UNORM
	template<>
	struct PixelComponents< PixelFormat::eR16_UNORM >
	{
		using Type = uint16_t;
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
		static Type G( uint8_t const * buffer )
		{
			return 0;
		}
		static Type B( uint8_t const * buffer )
		{
			return 0;
		}
		static Type A( uint8_t const * buffer )
		{
			return std::numeric_limits< Type >::max();
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value;
		}
		static void G( uint8_t * buffer, Type value )
		{
		}
		static void B( uint8_t * buffer, Type value )
		{
		}
		static void A( uint8_t * buffer, Type value )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR16_SNORM
	//!\~french		Spécialisation pour PixelFormat::eR16_SNORM
	template<>
	struct PixelComponents< PixelFormat::eR16_SNORM >
	{
		using Type = int16_t;
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
		static Type G( uint8_t const * buffer )
		{
			return 0;
		}
		static Type B( uint8_t const * buffer )
		{
			return 0;
		}
		static Type A( uint8_t const * buffer )
		{
			return std::numeric_limits< Type >::max();
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value;
		}
		static void G( uint8_t * buffer, Type value )
		{
		}
		static void B( uint8_t * buffer, Type value )
		{
		}
		static void A( uint8_t * buffer, Type value )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR16_USCALED
	//!\~french		Spécialisation pour PixelFormat::eR16_USCALED
	template<>
	struct PixelComponents< PixelFormat::eR16_USCALED >
		: public PixelComponents< PixelFormat::eR16_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR16_SSCALED
	//!\~french		Spécialisation pour PixelFormat::eR16_SSCALED
	template<>
	struct PixelComponents< PixelFormat::eR16_SSCALED >
		: public PixelComponents< PixelFormat::eR16_SNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR16_UINT
	//!\~french		Spécialisation pour PixelFormat::eR16_UINT
	template<>
	struct PixelComponents< PixelFormat::eR16_UINT >
		: public PixelComponents< PixelFormat::eR16_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR16_SINT
	//!\~french		Spécialisation pour PixelFormat::eR16_SINT
	template<>
	struct PixelComponents< PixelFormat::eR16_SINT >
		: public PixelComponents< PixelFormat::eR16_SNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR16_SFLOAT
	//!\~french		Spécialisation pour PixelFormat::eR16_SFLOAT
	template<>
	struct PixelComponents< PixelFormat::eR16_SFLOAT >
		: public PixelComponents< PixelFormat::eR16_SNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR16G16_UNORM
	//!\~french		Spécialisation pour PixelFormat::eR16G16_UNORM
	template<>
	struct PixelComponents< PixelFormat::eR16G16_UNORM >
	{
		using Type = uint16_t;
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
		static Type B( uint8_t const * buffer )
		{
			return 0;
		}
		static Type A( uint8_t const * buffer )
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
		}
		static void A( uint8_t * buffer, Type value )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR16G16_SNORM
	//!\~french		Spécialisation pour PixelFormat::eR16G16_SNORM
	template<>
	struct PixelComponents< PixelFormat::eR16G16_SNORM >
	{
		using Type = int16_t;
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
		static Type B( uint8_t const * buffer )
		{
			return 0;
		}
		static Type A( uint8_t const * buffer )
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
		}
		static void A( uint8_t * buffer, Type value )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR16G16_USCALED
	//!\~french		Spécialisation pour PixelFormat::eR16G16_USCALED
	template<>
	struct PixelComponents< PixelFormat::eR16G16_USCALED >
		: public PixelComponents< PixelFormat::eR16G16_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR16G16_SSCALED
	//!\~french		Spécialisation pour PixelFormat::eR16G16_SSCALED
	template<>
	struct PixelComponents< PixelFormat::eR16G16_SSCALED >
		: public PixelComponents< PixelFormat::eR16G16_SNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR16G16_UINT
	//!\~french		Spécialisation pour PixelFormat::eR16G16_UINT
	template<>
	struct PixelComponents< PixelFormat::eR16G16_UINT >
		: public PixelComponents< PixelFormat::eR16G16_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR16G16_SINT
	//!\~french		Spécialisation pour PixelFormat::eR16G16_SINT
	template<>
	struct PixelComponents< PixelFormat::eR16G16_SINT >
		: public PixelComponents< PixelFormat::eR16G16_SNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR16G16_SFLOAT
	//!\~french		Spécialisation pour PixelFormat::eR16G16_SFLOAT
	template<>
	struct PixelComponents< PixelFormat::eR16G16_SFLOAT >
		: public PixelComponents< PixelFormat::eR16G16_SNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR16G16B16_UNORM
	//!\~french		Spécialisation pour PixelFormat::eR16G16B16_UNORM
	template<>
	struct PixelComponents< PixelFormat::eR16G16B16_UNORM >
	{
		using Type = uint16_t;
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
		static Type A( uint8_t const * buffer )
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
		static void A( uint8_t * buffer, Type value )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR16G16B16_SNORM
	//!\~french		Spécialisation pour PixelFormat::eR16G16B16_SNORM
	template<>
	struct PixelComponents< PixelFormat::eR16G16B16_SNORM >
	{
		using Type = int16_t;
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
		static Type A( uint8_t const * buffer )
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
		static void A( uint8_t * buffer, Type value )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR16G16B16_USCALED
	//!\~french		Spécialisation pour PixelFormat::eR16G16B16_USCALED
	template<>
	struct PixelComponents< PixelFormat::eR16G16B16_USCALED >
		: public PixelComponents< PixelFormat::eR16G16B16_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR16G16B16_SSCALED
	//!\~french		Spécialisation pour PixelFormat::eR16G16B16_SSCALED
	template<>
	struct PixelComponents< PixelFormat::eR16G16B16_SSCALED >
		: public PixelComponents< PixelFormat::eR16G16B16_SNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR16G16B16_UINT
	//!\~french		Spécialisation pour PixelFormat::eR16G16B16_UINT
	template<>
	struct PixelComponents< PixelFormat::eR16G16B16_UINT >
		: public PixelComponents< PixelFormat::eR16G16B16_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR16G16B16_SINT
	//!\~french		Spécialisation pour PixelFormat::eR16G16B16_SINT
	template<>
	struct PixelComponents< PixelFormat::eR16G16B16_SINT >
		: public PixelComponents< PixelFormat::eR16G16B16_SNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR16G16B16_SFLOAT
	//!\~french		Spécialisation pour PixelFormat::eR16G16B16_SFLOAT
	template<>
	struct PixelComponents< PixelFormat::eR16G16B16_SFLOAT >
		: public PixelComponents< PixelFormat::eR16G16B16_SNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR16G16B16A16_UNORM
	//!\~french		Spécialisation pour PixelFormat::eR16G16B16A16_UNORM
	template<>
	struct PixelComponents< PixelFormat::eR16G16B16A16_UNORM >
	{
		using Type = uint16_t;
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
	struct PixelComponents< PixelFormat::eR16G16B16A16_SNORM >
	{
		using Type = int16_t;
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
	struct PixelComponents< PixelFormat::eR16G16B16A16_USCALED >
		: public PixelComponents< PixelFormat::eR16G16B16A16_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR16G16B16A16_SSCALED
	//!\~french		Spécialisation pour PixelFormat::eR16G16B16A16_SSCALED
	template<>
	struct PixelComponents< PixelFormat::eR16G16B16A16_SSCALED >
		: public PixelComponents< PixelFormat::eR16G16B16A16_SNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR16G16B16A16_UINT
	//!\~french		Spécialisation pour PixelFormat::eR16G16B16A16_UINT
	template<>
	struct PixelComponents< PixelFormat::eR16G16B16A16_UINT >
		: public PixelComponents< PixelFormat::eR16G16B16A16_UNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR16G16B16A16_SINT
	//!\~french		Spécialisation pour PixelFormat::eR16G16B16A16_SINT
	template<>
	struct PixelComponents< PixelFormat::eR16G16B16A16_SINT >
		: public PixelComponents< PixelFormat::eR16G16B16A16_SNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR16G16B16A16_SFLOAT
	//!\~french		Spécialisation pour PixelFormat::eR16G16B16A16_SFLOAT
	template<>
	struct PixelComponents< PixelFormat::eR16G16B16A16_SFLOAT >
		: public PixelComponents< PixelFormat::eR16G16B16A16_SNORM >
	{
	};
	//!\~english	Specialisation for PixelFormat::eR32_UINT
	//!\~french		Spécialisation pour PixelFormat::eR32_UINT
	template<>
	struct PixelComponents< PixelFormat::eR32_UINT >
	{
		using Type = uint32_t;
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
		static Type G( uint8_t const * buffer )
		{
			return 0;
		}
		static Type B( uint8_t const * buffer )
		{
			return 0;
		}
		static Type A( uint8_t const * buffer )
		{
			return std::numeric_limits< Type >::max();
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value;
		}
		static void G( uint8_t * buffer, Type value )
		{
		}
		static void B( uint8_t * buffer, Type value )
		{
		}
		static void A( uint8_t * buffer, Type value )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR32_SINT
	//!\~french		Spécialisation pour PixelFormat::eR32_SINT
	template<>
	struct PixelComponents< PixelFormat::eR32_SINT >
	{
		using Type = int32_t;
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
		static Type G( uint8_t const * buffer )
		{
			return 0;
		}
		static Type B( uint8_t const * buffer )
		{
			return 0;
		}
		static Type A( uint8_t const * buffer )
		{
			return std::numeric_limits< Type >::max();
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value;
		}
		static void G( uint8_t * buffer, Type value )
		{
		}
		static void B( uint8_t * buffer, Type value )
		{
		}
		static void A( uint8_t * buffer, Type value )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR32_SFLOAT
	//!\~french		Spécialisation pour PixelFormat::eR32_SFLOAT
	template<>
	struct PixelComponents< PixelFormat::eR32_SFLOAT >
	{
		using Type = float;
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
		static Type G( uint8_t const * buffer )
		{
			return 0;
		}
		static Type B( uint8_t const * buffer )
		{
			return 0;
		}
		static Type A( uint8_t const * buffer )
		{
			return std::numeric_limits< Type >::max();
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value;
		}
		static void G( uint8_t * buffer, Type value )
		{
		}
		static void B( uint8_t * buffer, Type value )
		{
		}
		static void A( uint8_t * buffer, Type value )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR32G32_UINT
	//!\~french		Spécialisation pour PixelFormat::eR32G32_UINT
	template<>
	struct PixelComponents< PixelFormat::eR32G32_UINT >
	{
		using Type = uint32_t;
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
		static Type B( uint8_t const * buffer )
		{
			return 0;
		}
		static Type A( uint8_t const * buffer )
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
		}
		static void A( uint8_t * buffer, Type value )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR32G32_SINT
	//!\~french		Spécialisation pour PixelFormat::eR32G32_SINT
	template<>
	struct PixelComponents< PixelFormat::eR32G32_SINT >
	{
		using Type = int32_t;
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
		static Type B( uint8_t const * buffer )
		{
			return 0;
		}
		static Type A( uint8_t const * buffer )
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
		}
		static void A( uint8_t * buffer, Type value )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR32G32_SFLOAT
	//!\~french		Spécialisation pour PixelFormat::eR32G32_SFLOAT
	template<>
	struct PixelComponents< PixelFormat::eR32G32_SFLOAT >
	{
		using Type = float;
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
		static Type B( uint8_t const * buffer )
		{
			return 0;
		}
		static Type A( uint8_t const * buffer )
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
		}
		static void A( uint8_t * buffer, Type value )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR32G32B32_UINT
	//!\~french		Spécialisation pour PixelFormat::eR32G32B32_UINT
	template<>
	struct PixelComponents< PixelFormat::eR32G32B32_UINT >
	{
		using Type = uint32_t;
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
		static Type A( uint8_t const * buffer )
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
		static void A( uint8_t * buffer, Type value )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR32G32B32_SINT
	//!\~french		Spécialisation pour PixelFormat::eR32G32B32_SINT
	template<>
	struct PixelComponents< PixelFormat::eR32G32B32_SINT >
	{
		using Type = int32_t;
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
		static Type A( uint8_t const * buffer )
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
		static void A( uint8_t * buffer, Type value )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR32G32B32_SFLOAT
	//!\~french		Spécialisation pour PixelFormat::eR32G32B32_SFLOAT
	template<>
	struct PixelComponents< PixelFormat::eR32G32B32_SFLOAT >
	{
		using Type = float;
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
		static Type A( uint8_t const * buffer )
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
		static void A( uint8_t * buffer, Type value )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR32G32B32A32_UINT
	//!\~french		Spécialisation pour PixelFormat::eR32G32B32A32_UINT
	template<>
	struct PixelComponents< PixelFormat::eR32G32B32A32_UINT >
	{
		using Type = uint32_t;
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
	struct PixelComponents< PixelFormat::eR32G32B32A32_SINT >
	{
		using Type = int32_t;
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
	struct PixelComponents< PixelFormat::eR32G32B32A32_SFLOAT >
	{
		using Type = float;
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
	struct PixelComponents< PixelFormat::eR64_UINT >
	{
		using Type = uint64_t;
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
		static Type G( uint8_t const * buffer )
		{
			return 0;
		}
		static Type B( uint8_t const * buffer )
		{
			return 0;
		}
		static Type A( uint8_t const * buffer )
		{
			return std::numeric_limits< Type >::max();
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value;
		}
		static void G( uint8_t * buffer, Type value )
		{
		}
		static void B( uint8_t * buffer, Type value )
		{
		}
		static void A( uint8_t * buffer, Type value )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR64_SINT
	//!\~french		Spécialisation pour PixelFormat::eR64_SINT
	template<>
	struct PixelComponents< PixelFormat::eR64_SINT >
	{
		using Type = int64_t;
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
		static Type G( uint8_t const * buffer )
		{
			return 0;
		}
		static Type B( uint8_t const * buffer )
		{
			return 0;
		}
		static Type A( uint8_t const * buffer )
		{
			return std::numeric_limits< Type >::max();
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value;
		}
		static void G( uint8_t * buffer, Type value )
		{
		}
		static void B( uint8_t * buffer, Type value )
		{
		}
		static void A( uint8_t * buffer, Type value )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR64_SFLOAT
	//!\~french		Spécialisation pour PixelFormat::eR64_SFLOAT
	template<>
	struct PixelComponents< PixelFormat::eR64_SFLOAT >
	{
		using Type = double;
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
		static Type G( uint8_t const * buffer )
		{
			return 0;
		}
		static Type B( uint8_t const * buffer )
		{
			return 0;
		}
		static Type A( uint8_t const * buffer )
		{
			return std::numeric_limits< Type >::max();
		}
		static void R( uint8_t * buffer, Type value )
		{
			getDst( buffer ).r = value;
		}
		static void G( uint8_t * buffer, Type value )
		{
		}
		static void B( uint8_t * buffer, Type value )
		{
		}
		static void A( uint8_t * buffer, Type value )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR64G64_UINT
	//!\~french		Spécialisation pour PixelFormat::eR64G64_UINT
	template<>
	struct PixelComponents< PixelFormat::eR64G64_UINT >
	{
		using Type = uint64_t;
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
		static Type B( uint8_t const * buffer )
		{
			return 0;
		}
		static Type A( uint8_t const * buffer )
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
		}
		static void A( uint8_t * buffer, Type value )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR64G64_SINT
	//!\~french		Spécialisation pour PixelFormat::eR64G64_SINT
	template<>
	struct PixelComponents< PixelFormat::eR64G64_SINT >
	{
		using Type = int64_t;
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
		static Type B( uint8_t const * buffer )
		{
			return 0;
		}
		static Type A( uint8_t const * buffer )
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
		}
		static void A( uint8_t * buffer, Type value )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR64G64_SFLOAT
	//!\~french		Spécialisation pour PixelFormat::eR64G64_SFLOAT
	template<>
	struct PixelComponents< PixelFormat::eR64G64_SFLOAT >
	{
		using Type = double;
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
		static Type B( uint8_t const * buffer )
		{
			return 0;
		}
		static Type A( uint8_t const * buffer )
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
		}
		static void A( uint8_t * buffer, Type value )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR64G64B64_UINT
	//!\~french		Spécialisation pour PixelFormat::eR64G64B64_UINT
	template<>
	struct PixelComponents< PixelFormat::eR64G64B64_UINT >
	{
		using Type = uint64_t;
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
		static Type A( uint8_t const * buffer )
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
		static void A( uint8_t * buffer, Type value )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR64G64B64_SINT
	//!\~french		Spécialisation pour PixelFormat::eR64G64B64_SINT
	template<>
	struct PixelComponents< PixelFormat::eR64G64B64_SINT >
	{
		using Type = int64_t;
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
		static Type A( uint8_t const * buffer )
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
		static void A( uint8_t * buffer, Type value )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR64G64B64_SFLOAT
	//!\~french		Spécialisation pour PixelFormat::eR64G64B64_SFLOAT
	template<>
	struct PixelComponents< PixelFormat::eR64G64B64_SFLOAT >
	{
		using Type = double;
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
		static Type A( uint8_t const * buffer )
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
		static void A( uint8_t * buffer, Type value )
		{
		}
	};
	//!\~english	Specialisation for PixelFormat::eR64G64B64A64_UINT
	//!\~french		Spécialisation pour PixelFormat::eR64G64B64A64_UINT
	template<>
	struct PixelComponents< PixelFormat::eR64G64B64A64_UINT >
	{
		using Type = uint64_t;
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
	struct PixelComponents< PixelFormat::eR64G64B64A64_SINT >
	{
		using Type = int64_t;
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
	struct PixelComponents< PixelFormat::eR64G64B64A64_SFLOAT >
	{
		using Type = double;
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
	//!\~english	Specialisation for PixelFormat::eD16_UNORM
	//!\~french		Spécialisation pour PixelFormat::eD16_UNORM
	template<>
	struct PixelComponents< PixelFormat::eD16_UNORM >
	{
		using Type = uint16_t;
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
	struct PixelComponents< PixelFormat::eX8_D24_UNORM >
	{
		using Type = uint32_t;
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
	struct PixelComponents< PixelFormat::eD32_SFLOAT >
	{
		using Type = float;
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
	struct PixelComponents< PixelFormat::eS8_UINT >
	{
		using Type = uint8_t;
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
	struct PixelComponents< PixelFormat::eD16_UNORM_S8_UINT >
	{
		using Type = uint16_t;
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
	struct PixelComponents< PixelFormat::eD24_UNORM_S8_UINT >
	{
		using Type = uint32_t;
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
	struct PixelComponents< PixelFormat::eD32_SFLOAT_S8_UINT >
	{
		using Type = float;
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
				return Type( value );
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
				return Type( value );
			}
			static Type cast( double value )
			{
				return Type( value );
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
				return Type( value );
			}
			static Type cast( double value )
			{
				return Type( value );
			}
		};

		template<>
		struct ConvCastT< uint16_t >
		{
			using Type = uint16_t;

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
				return Type( value );
			}
			static Type cast( double value )
			{
				return Type( value );
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
				return Type( value );
			}
			static Type cast( double value )
			{
				return Type( value );
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
				return Type( value );
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
				return Type( value );
			}
			static Type cast( double value )
			{
				return Type( value );
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
				return Type( value );
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
				return Type( value );
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

	template< PixelFormat PFT >
	uint8_t getR8U( uint8_t const * buffer )
	{
		return details::PxComponentGetter< uint8_t >::get( buffer, PixelComponents< PFT >::R );
	}

	template< PixelFormat PFT >
	int8_t getR8S( uint8_t const * buffer )
	{
		return details::PxComponentGetter< int8_t >::get( buffer, PixelComponents< PFT >::R );
	}

	template< PixelFormat PFT >
	uint16_t getR16U( uint8_t * buffer )
	{
		return details::PxComponentGetter< uint16_t >::get( buffer, PixelComponents< PFT >::R );
	}

	template< PixelFormat PFT >
	int16_t getR16S( uint8_t const * buffer )
	{
		return details::PxComponentGetter< int16_t >::get( buffer, PixelComponents< PFT >::R );
	}

	template< PixelFormat PFT >
	int16_t getR16F( uint8_t const * buffer )
	{
		return details::PxComponentGetter< int16_t >::get( buffer, PixelComponents< PFT >::R );
	}

	template< PixelFormat PFT >
	uint32_t getR32S( uint8_t const * buffer )
	{
		return details::PxComponentGetter< uint32_t >::get( buffer, PixelComponents< PFT >::R );
	}

	template< PixelFormat PFT >
	int32_t getR32U( uint8_t const * buffer )
	{
		return details::PxComponentGetter< int32_t >::get( buffer, PixelComponents< PFT >::R );
	}

	template< PixelFormat PFT >
	float getR32F( uint8_t const * buffer )
	{
		return details::PxComponentGetter< float >::get( buffer, PixelComponents< PFT >::R );
	}

	template< PixelFormat PFT >
	uint64_t getR64U( uint8_t const * buffer )
	{
		return details::PxComponentGetter< uint64_t >::get( buffer, PixelComponents< PFT >::R );
	}

	template< PixelFormat PFT >
	int64_t getR64S( uint8_t const * buffer )
	{
		return details::PxComponentGetter< int64_t >::get( buffer, PixelComponents< PFT >::R );
	}

	template< PixelFormat PFT >
	double getR64F( uint8_t const * buffer )
	{
		return details::PxComponentGetter< double >::get( buffer, PixelComponents< PFT >::R );
	}

	//*************************************************************************************************

	template< PixelFormat PFT >
	uint8_t getG8U( uint8_t const * buffer )
	{
		return details::PxComponentGetter< uint8_t >::get( buffer, PixelComponents< PFT >::G );
	}

	template< PixelFormat PFT >
	int8_t getG8S( uint8_t const * buffer )
	{
		return details::PxComponentGetter< int8_t >::get( buffer, PixelComponents< PFT >::G );
	}

	template< PixelFormat PFT >
	uint16_t getG16U( uint8_t const * buffer )
	{
		return details::PxComponentGetter< uint16_t >::get( buffer, PixelComponents< PFT >::G );
	}

	template< PixelFormat PFT >
	int16_t getG16S( uint8_t const * buffer )
	{
		return details::PxComponentGetter< int16_t >::get( buffer, PixelComponents< PFT >::G );
	}

	template< PixelFormat PFT >
	int16_t getG16F( uint8_t const * buffer )
	{
		return details::PxComponentGetter< int16_t >::get( buffer, PixelComponents< PFT >::G );
	}

	template< PixelFormat PFT >
	uint32_t getG32U( uint8_t const * buffer )
	{
		return details::PxComponentGetter< uint32_t >::get( buffer, PixelComponents< PFT >::G );
	}

	template< PixelFormat PFT >
	int32_t getG32S( uint8_t const * buffer )
	{
		return details::PxComponentGetter< int32_t >::get( buffer, PixelComponents< PFT >::G );
	}

	template< PixelFormat PFT >
	float getG32F( uint8_t const * buffer )
	{
		return details::PxComponentGetter< float >::get( buffer, PixelComponents< PFT >::G );
	}

	template< PixelFormat PFT >
	uint64_t getG64U( uint8_t const * buffer )
	{
		return details::PxComponentGetter< uint64_t >::get( buffer, PixelComponents< PFT >::G );
	}

	template< PixelFormat PFT >
	int64_t getG64S( uint8_t const * buffer )
	{
		return details::PxComponentGetter< int64_t >::get( buffer, PixelComponents< PFT >::G );
	}

	template< PixelFormat PFT >
	double getG64F( uint8_t const * buffer )
	{
		return details::PxComponentGetter< double >::get( buffer, PixelComponents< PFT >::G );
	}

	//*************************************************************************************************

	template< PixelFormat PFT >
	uint8_t getB8U( uint8_t const * buffer )
	{
		return details::PxComponentGetter< uint8_t >::get( buffer, PixelComponents< PFT >::B );
	}

	template< PixelFormat PFT >
	int8_t getB8S( uint8_t const * buffer )
	{
		return details::PxComponentGetter< int8_t >::get( buffer, PixelComponents< PFT >::B );
	}

	template< PixelFormat PFT >
	uint16_t getB16U( uint8_t const * buffer )
	{
		return details::PxComponentGetter< uint16_t >::get( buffer, PixelComponents< PFT >::B );
	}

	template< PixelFormat PFT >
	int16_t getB16S( uint8_t const * buffer )
	{
		return details::PxComponentGetter< int16_t >::get( buffer, PixelComponents< PFT >::B );
	}

	template< PixelFormat PFT >
	int16_t getB16F( uint8_t const * buffer )
	{
		return details::PxComponentGetter< int16_t >::get( buffer, PixelComponents< PFT >::B );
	}

	template< PixelFormat PFT >
	uint32_t getB32U( uint8_t const * buffer )
	{
		return details::PxComponentGetter< uint32_t >::get( buffer, PixelComponents< PFT >::B );
	}

	template< PixelFormat PFT >
	int32_t getB32S( uint8_t const * buffer )
	{
		return details::PxComponentGetter< int32_t >::get( buffer, PixelComponents< PFT >::B );
	}

	template< PixelFormat PFT >
	float getB32F( uint8_t const * buffer )
	{
		return details::PxComponentGetter< float >::get( buffer, PixelComponents< PFT >::B );
	}

	template< PixelFormat PFT >
	uint64_t getB64U( uint8_t const * buffer )
	{
		return details::PxComponentGetter< uint64_t >::get( buffer, PixelComponents< PFT >::B );
	}

	template< PixelFormat PFT >
	int64_t getB64S( uint8_t const * buffer )
	{
		return details::PxComponentGetter< int64_t >::get( buffer, PixelComponents< PFT >::B );
	}

	template< PixelFormat PFT >
	double getB64F( uint8_t const * buffer )
	{
		return details::PxComponentGetter< double >::get( buffer, PixelComponents< PFT >::B );
	}

	//*************************************************************************************************

	template< PixelFormat PFT >
	uint8_t getA8U( uint8_t const * buffer )
	{
		return details::PxComponentGetter< uint8_t >::get( buffer, PixelComponents< PFT >::A );
	}

	template< PixelFormat PFT >
	int8_t getA8S( uint8_t const * buffer )
	{
		return details::PxComponentGetter< int8_t >::get( buffer, PixelComponents< PFT >::A );
	}

	template< PixelFormat PFT >
	uint16_t getA16U( uint8_t const * buffer )
	{
		return details::PxComponentGetter< uint16_t >::get( buffer, PixelComponents< PFT >::A );
	}

	template< PixelFormat PFT >
	int16_t getA16S( uint8_t const * buffer )
	{
		return details::PxComponentGetter< int16_t >::get( buffer, PixelComponents< PFT >::A );
	}

	template< PixelFormat PFT >
	int16_t getA16F( uint8_t const * buffer )
	{
		return details::PxComponentGetter< int16_t >::get( buffer, PixelComponents< PFT >::A );
	}

	template< PixelFormat PFT >
	uint32_t getA32U( uint8_t const * buffer )
	{
		return details::PxComponentGetter< uint32_t >::get( buffer, PixelComponents< PFT >::A );
	}

	template< PixelFormat PFT >
	int32_t getA32S( uint8_t const * buffer )
	{
		return details::PxComponentGetter< int32_t >::get( buffer, PixelComponents< PFT >::A );
	}

	template< PixelFormat PFT >
	float getA32F( uint8_t const * buffer )
	{
		return details::PxComponentGetter< float >::get( buffer, PixelComponents< PFT >::A );
	}

	template< PixelFormat PFT >
	uint64_t getA64U( uint8_t const * buffer )
	{
		return details::PxComponentGetter< uint64_t >::get( buffer, PixelComponents< PFT >::A );
	}

	template< PixelFormat PFT >
	int64_t getA64S( uint8_t const * buffer )
	{
		return details::PxComponentGetter< int64_t >::get( buffer, PixelComponents< PFT >::A );
	}

	template< PixelFormat PFT >
	double getA64F( uint8_t const * buffer )
	{
		return details::PxComponentGetter< double >::get( buffer, PixelComponents< PFT >::A );
	}

	//*************************************************************************************************

	template< PixelFormat PFT >
	uint16_t getD16U( uint8_t const * buffer )
	{
		return details::PxComponentGetter< uint16_t >::get( buffer, PixelComponents< PFT >::D );
	}

	template< PixelFormat PFT >
	uint32_t getD24U( uint8_t const * buffer )
	{
		return details::PxComponentGetter< uint32_t >::get( buffer, PixelComponents< PFT >::D );
	}

	template< PixelFormat PFT >
	float getD32F( uint8_t const * buffer )
	{
		return details::PxComponentGetter< float >::get( buffer, PixelComponents< PFT >::D );
	}

	//*************************************************************************************************

	template< PixelFormat PFT >
	uint8_t getS8U( uint8_t const * buffer )
	{
		return details::PxComponentGetter< uint8_t >::get( buffer, PixelComponents< PFT >::S );
	}

	//*************************************************************************************************

	template< PixelFormat PFT >
	void setR8U( uint8_t * buffer, uint8_t value )
	{
		details::PxComponentSetter< uint8_t >::set( buffer, value, PixelComponents< PFT >::R );
	}

	template< PixelFormat PFT >
	void setR8S( uint8_t * buffer, int8_t value )
	{
		details::PxComponentSetter< int8_t >::set( buffer, value, PixelComponents< PFT >::R );
	}

	template< PixelFormat PFT >
	void setR16U( uint8_t * buffer, uint16_t value )
	{
		details::PxComponentSetter< uint16_t >::set( buffer, value, PixelComponents< PFT >::R );
	}

	template< PixelFormat PFT >
	void setR16S( uint8_t * buffer, int16_t value )
	{
		details::PxComponentSetter< int16_t >::set( buffer, value, PixelComponents< PFT >::R );
	}

	template< PixelFormat PFT >
	void setR16F( uint8_t * buffer, int16_t value )
	{
		details::PxComponentSetter< int16_t >::set( buffer, value, PixelComponents< PFT >::R );
	}

	template< PixelFormat PFT >
	void setR32S( uint8_t * buffer, uint32_t value )
	{
		details::PxComponentSetter< uint32_t >::set( buffer, value, PixelComponents< PFT >::R );
	}

	template< PixelFormat PFT >
	void setR32U( uint8_t * buffer, int32_t value )
	{
		details::PxComponentSetter< int32_t >::set( buffer, value, PixelComponents< PFT >::R );
	}

	template< PixelFormat PFT >
	void setR32F( uint8_t * buffer, float value )
	{
		details::PxComponentSetter< float >::set( buffer, value, PixelComponents< PFT >::R );
	}

	template< PixelFormat PFT >
	void setR64U( uint8_t * buffer, uint64_t value )
	{
		details::PxComponentSetter< uint64_t >::set( buffer, value, PixelComponents< PFT >::R );
	}

	template< PixelFormat PFT >
	void setR64S( uint8_t * buffer, int64_t value )
	{
		details::PxComponentSetter< int64_t >::set( buffer, value, PixelComponents< PFT >::R );
	}

	template< PixelFormat PFT >
	void setR64F( uint8_t * buffer, double value )
	{
		details::PxComponentSetter< double >::set( buffer, value, PixelComponents< PFT >::R );
	}

	//*************************************************************************************************

	template< PixelFormat PFT >
	void setG8U( uint8_t * buffer, uint8_t value )
	{
		details::PxComponentSetter< uint8_t >::set( buffer, value, PixelComponents< PFT >::G );
	}

	template< PixelFormat PFT >
	void setG8S( uint8_t * buffer, int8_t value )
	{
		details::PxComponentSetter< int8_t >::set( buffer, value, PixelComponents< PFT >::G );
	}

	template< PixelFormat PFT >
	void setG16U( uint8_t * buffer, uint16_t value )
	{
		details::PxComponentSetter< uint16_t >::set( buffer, value, PixelComponents< PFT >::G );
	}

	template< PixelFormat PFT >
	void setG16S( uint8_t * buffer, int16_t value )
	{
		details::PxComponentSetter< int16_t >::set( buffer, value, PixelComponents< PFT >::G );
	}

	template< PixelFormat PFT >
	void setG16F( uint8_t * buffer, int16_t value )
	{
		details::PxComponentSetter< int16_t >::set( buffer, value, PixelComponents< PFT >::G );
	}

	template< PixelFormat PFT >
	void setG32S( uint8_t * buffer, uint32_t value )
	{
		details::PxComponentSetter< uint32_t >::set( buffer, value, PixelComponents< PFT >::G );
	}

	template< PixelFormat PFT >
	void setG32U( uint8_t * buffer, int32_t value )
	{
		details::PxComponentSetter< int32_t >::set( buffer, value, PixelComponents< PFT >::G );
	}

	template< PixelFormat PFT >
	void setG32F( uint8_t * buffer, float value )
	{
		details::PxComponentSetter< float >::set( buffer, value, PixelComponents< PFT >::G );
	}

	template< PixelFormat PFT >
	void setG64U( uint8_t * buffer, uint64_t value )
	{
		details::PxComponentSetter< uint64_t >::set( buffer, value, PixelComponents< PFT >::G );
	}

	template< PixelFormat PFT >
	void setG64S( uint8_t * buffer, int64_t value )
	{
		details::PxComponentSetter< int64_t >::set( buffer, value, PixelComponents< PFT >::G );
	}

	template< PixelFormat PFT >
	void setG64F( uint8_t * buffer, double value )
	{
		details::PxComponentSetter< double >::set( buffer, value, PixelComponents< PFT >::G );
	}

	//*************************************************************************************************

	template< PixelFormat PFT >
	void setB8U( uint8_t * buffer, uint8_t value )
	{
		details::PxComponentSetter< uint8_t >::set( buffer, value, PixelComponents< PFT >::B );
	}

	template< PixelFormat PFT >
	void setB8S( uint8_t * buffer, int8_t value )
	{
		details::PxComponentSetter< int8_t >::set( buffer, value, PixelComponents< PFT >::B );
	}

	template< PixelFormat PFT >
	void setB16U( uint8_t * buffer, uint16_t value )
	{
		details::PxComponentSetter< uint16_t >::set( buffer, value, PixelComponents< PFT >::B );
	}

	template< PixelFormat PFT >
	void setB16S( uint8_t * buffer, int16_t value )
	{
		details::PxComponentSetter< int16_t >::set( buffer, value, PixelComponents< PFT >::B );
	}

	template< PixelFormat PFT >
	void setB16F( uint8_t * buffer, int16_t value )
	{
		details::PxComponentSetter< int16_t >::set( buffer, value, PixelComponents< PFT >::B );
	}

	template< PixelFormat PFT >
	void setB32S( uint8_t * buffer, uint32_t value )
	{
		details::PxComponentSetter< uint32_t >::set( buffer, value, PixelComponents< PFT >::B );
	}

	template< PixelFormat PFT >
	void setB32U( uint8_t * buffer, int32_t value )
	{
		details::PxComponentSetter< int32_t >::set( buffer, value, PixelComponents< PFT >::B );
	}

	template< PixelFormat PFT >
	void setB32F( uint8_t * buffer, float value )
	{
		details::PxComponentSetter< float >::set( buffer, value, PixelComponents< PFT >::B );
	}

	template< PixelFormat PFT >
	void setB64U( uint8_t * buffer, uint64_t value )
	{
		details::PxComponentSetter< uint64_t >::set( buffer, value, PixelComponents< PFT >::B );
	}

	template< PixelFormat PFT >
	void setB64S( uint8_t * buffer, int64_t value )
	{
		details::PxComponentSetter< int64_t >::set( buffer, value, PixelComponents< PFT >::B );
	}

	template< PixelFormat PFT >
	void setB64F( uint8_t * buffer, double value )
	{
		details::PxComponentSetter< double >::set( buffer, value, PixelComponents< PFT >::B );
	}

	//*************************************************************************************************

	template< PixelFormat PFT >
	void setA8U( uint8_t * buffer, uint8_t value )
	{
		details::PxComponentSetter< uint8_t >::set( buffer, value, PixelComponents< PFT >::A );
	}

	template< PixelFormat PFT >
	void setA8S( uint8_t * buffer, int8_t value )
	{
		details::PxComponentSetter< int8_t >::set( buffer, value, PixelComponents< PFT >::A );
	}

	template< PixelFormat PFT >
	void setA16U( uint8_t * buffer, uint16_t value )
	{
		details::PxComponentSetter< uint16_t >::set( buffer, value, PixelComponents< PFT >::A );
	}

	template< PixelFormat PFT >
	void setA16S( uint8_t * buffer, int16_t value )
	{
		details::PxComponentSetter< int16_t >::set( buffer, value, PixelComponents< PFT >::A );
	}

	template< PixelFormat PFT >
	void setA16F( uint8_t * buffer, int16_t value )
	{
		details::PxComponentSetter< int16_t >::set( buffer, value, PixelComponents< PFT >::A );
	}

	template< PixelFormat PFT >
	void setA32S( uint8_t * buffer, uint32_t value )
	{
		details::PxComponentSetter< uint32_t >::set( buffer, value, PixelComponents< PFT >::A );
	}

	template< PixelFormat PFT >
	void setA32U( uint8_t * buffer, int32_t value )
	{
		details::PxComponentSetter< int32_t >::set( buffer, value, PixelComponents< PFT >::A );
	}

	template< PixelFormat PFT >
	void setA32F( uint8_t * buffer, float value )
	{
		details::PxComponentSetter< float >::set( buffer, value, PixelComponents< PFT >::A );
	}

	template< PixelFormat PFT >
	void setA64U( uint8_t * buffer, uint64_t value )
	{
		details::PxComponentSetter< uint64_t >::set( buffer, value, PixelComponents< PFT >::A );
	}

	template< PixelFormat PFT >
	void setA64S( uint8_t * buffer, int64_t value )
	{
		details::PxComponentSetter< int64_t >::set( buffer, value, PixelComponents< PFT >::A );
	}

	template< PixelFormat PFT >
	void setA64F( uint8_t * buffer, double value )
	{
		details::PxComponentSetter< double >::set( buffer, value, PixelComponents< PFT >::A );
	}

	//*************************************************************************************************

	template< PixelFormat PFT >
	void setD16U( uint8_t * buffer, uint16_t value )
	{
		details::PxComponentSetter< uint16_t >::set( buffer, value, PixelComponents< PFT >::D );
	}

	template< PixelFormat PFT >
	void setD24U( uint8_t * buffer, uint32_t value )
	{
		details::PxComponentSetter< uint32_t >::set( buffer, value, PixelComponents< PFT >::D );
	}

	template< PixelFormat PFT >
	void setD32F( uint8_t * buffer, float value )
	{
		details::PxComponentSetter< float >::set( buffer, value, PixelComponents< PFT >::D );
	}

	//*************************************************************************************************

	template< PixelFormat PFT >
	void setS8U( uint8_t * buffer, uint8_t value )
	{
		details::PxComponentSetter< uint8_t >::set( buffer, value, PixelComponents< PFT >::S );
	}

	//*************************************************************************************************
}
