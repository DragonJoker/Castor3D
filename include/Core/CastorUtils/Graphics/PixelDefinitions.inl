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
	namespace details
	{
		/**
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
			void operator()( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
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
			void operator()( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
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
			void operator()( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
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
			void operator()( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
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
			void operator()( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
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
			void operator()( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
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
			void operator()( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
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
			void operator()( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
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
			void operator()( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
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
			void operator()( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
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
			void operator()( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
			{
				PixelComponents< PFDst >::B8( dstBuffer, PixelComponents< PFSrc >::B8( srcBuffer ) );
				PixelComponents< PFDst >::G8( dstBuffer, PixelComponents< PFSrc >::G8( srcBuffer ) );
				PixelComponents< PFDst >::R8( dstBuffer, PixelComponents< PFSrc >::R8( srcBuffer ) );
				srcBuffer += PixelDefinitions< PFSrc >::Size;
				dstBuffer += PixelDefinitions< PFDst >::Size;
			}
		};
		//!\~english	Specialisation for converting to PixelFormat::eB8G8R8A8_UNORM
		//!\~french		Spécialisation pour convertir vers PixelFormat::eB8G8R8A8_UNORM
		template< PixelFormat PFSrc, PixelFormat PFDst >
		struct PixelConverter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == PixelFormat::eB8G8R8A8_UNORM >::type >
		{
			void operator()( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
			{
				PixelComponents< PFDst >::B8( dstBuffer, PixelComponents< PFSrc >::B8( srcBuffer ) );
				PixelComponents< PFDst >::G8( dstBuffer, PixelComponents< PFSrc >::G8( srcBuffer ) );
				PixelComponents< PFDst >::R8( dstBuffer, PixelComponents< PFSrc >::R8( srcBuffer ) );
				PixelComponents< PFDst >::A8( dstBuffer, PixelComponents< PFSrc >::A8( srcBuffer ) );
				srcBuffer += PixelDefinitions< PFSrc >::Size;
				dstBuffer += PixelDefinitions< PFDst >::Size;
			}
		};
		//!\~english	Specialisation for converting to PixelFormat::eR8G8B8_SRGB
		//!\~french		Spécialisation pour convertir vers PixelFormat::eR8G8B8_SRGB
		template< PixelFormat PFSrc, PixelFormat PFDst >
		struct PixelConverter < PFSrc, PFDst, typename std::enable_if < PFSrc != PFDst && PFDst == PixelFormat::eR8G8B8_SRGB >::type >
		{
			void operator()( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
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
			void operator()( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
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
			void operator()( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
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
			void operator()( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
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
			void operator()( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
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
			void operator()( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
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
			void operator()( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
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
			void operator()( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
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
			void operator()( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
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
			void operator()( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
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
			void operator()( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
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
			void operator()( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
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
			void operator()( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
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
			void operator()( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
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
			void operator()( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
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
			void operator()( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
			{
				PixelComponents< PFDst >::S8( dstBuffer, PixelComponents< PFSrc >::S8( srcBuffer ) );
				srcBuffer += PixelDefinitions< PFSrc >::Size;
				dstBuffer += PixelDefinitions< PFDst >::Size;
			}
		};
		/**
		\~english
		\brief		Structure used to convert a buffer from one pixel format to another one
		\~french
		\brief		Structure utilisée pour convertir un buffer d'un format de pixels vers un autre
		*/
		template< PixelFormat PFSrc, PixelFormat PFDst, typename EnableT = void >
		struct BufferConverter;

		template< PixelFormat PFSrc
			, PixelFormat PFDst >
		struct BufferConverter< PFSrc, PFDst
			, std::enable_if_t< ( PFSrc == PFDst
				|| ( ( PFDst != PixelFormat::eBC1_RGB_UNORM_BLOCK )
					&& ( PFDst != PixelFormat::eBC1_RGB_SRGB_BLOCK )
					&& ( PFDst != PixelFormat::eBC3_UNORM_BLOCK )
					&& ( PFDst != PixelFormat::eBC3_SRGB_BLOCK ) ) ) > >
		{
			void operator()( Size const & srcDimensions
				, Size const & dstDimensions
				, uint8_t const * srcBuffer
				, uint32_t srcSize
				, uint8_t * dstBuffer
				, uint32_t dstSize )
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
		\~english
		\brief		Structure used to convert a buffer from one pixel format to another one
		\~french
		\brief		Structure utilisée pour convertir un buffer d'un format de pixels vers un autre
		*/
		template< PixelFormat PFSrc
			, PixelFormat PFDst >
			struct BufferConverter< PFSrc, PFDst
			, std::enable_if_t< ( PFSrc != PFDst
				&& ( ( PFDst == PixelFormat::eBC1_RGB_UNORM_BLOCK )
					|| ( PFDst == PixelFormat::eBC1_RGB_SRGB_BLOCK ) ) ) > >
		{
			void operator()( Size const & srcDimensions
				, Size const & dstDimensions
				, uint8_t const * srcBuffer
				, uint32_t srcSize
				, uint8_t * dstBuffer
				, uint32_t dstSize )
			{
				BC1Compressor compressor{ PixelDefinitions< PFSrc >::Size
					, PixelComponents< PFSrc >::R8
					, PixelComponents< PFSrc >::G8
					, PixelComponents< PFSrc >::B8
					, PixelComponents< PFSrc >::A8 };
				compressor.compress( srcDimensions
					, dstDimensions
					, srcBuffer
					, srcSize
					, dstBuffer
					, dstSize );
			}
		};
		/**
		\~english
		\brief		Structure used to convert a buffer from one pixel format to another one
		\~french
		\brief		Structure utilisée pour convertir un buffer d'un format de pixels vers un autre
		*/
		template< PixelFormat PFSrc
			, PixelFormat PFDst >
			struct BufferConverter< PFSrc, PFDst
			, std::enable_if_t< ( PFSrc != PFDst
				&& ( ( PFDst == PixelFormat::eBC3_UNORM_BLOCK )
					|| ( PFDst == PixelFormat::eBC3_SRGB_BLOCK ) ) ) > >
		{
			void operator()( Size const & srcDimensions
				, Size const & dstDimensions
				, uint8_t const * srcBuffer
				, uint32_t srcSize
				, uint8_t * dstBuffer
				, uint32_t dstSize )
			{
				BC3Compressor compressor{ PixelDefinitions< PFSrc >::Size
					, PixelComponents< PFSrc >::R8
					, PixelComponents< PFSrc >::G8
					, PixelComponents< PFSrc >::B8
					, PixelComponents< PFSrc >::A8 };
				compressor.compress( srcDimensions
					, dstDimensions
					, srcBuffer
					, srcSize
					, dstBuffer
					, dstSize );
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
				if ( PF == dstFormat )
				{
					PixelConverter< PF, PF >()( srcBuffer, dstBuffer );
				}
				else
				{
					CU_UnsupportedError( "No conversion defined" );
				}
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
				if ( PF == dstFormat )
				{
					PixelConverter< PF, PF >()( srcBuffer, dstBuffer );
				}
				else
				{
					CU_UnsupportedError( "No conversion defined" );
				}
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
				if ( PF == dstFormat )
				{
					PixelConverter< PF, PF >()( srcBuffer, dstBuffer );
				}
				else
				{
					CU_UnsupportedError( "No conversion defined" );
				}
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
		void dynamicColourBufferConversion( Size const & srcDimensions
			, Size const & dstDimensions
			, uint8_t const * srcBuffer
			, uint32_t srcSize
			, PixelFormat dstFormat
			, uint8_t * dstBuffer
			, uint32_t dstSize )
		{
			switch ( dstFormat )
			{
			case PixelFormat::eR8_UNORM:
				BufferConverter< PF, PixelFormat::eR8_UNORM >()( srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eR16_SFLOAT:
				BufferConverter< PF, PixelFormat::eR16_SFLOAT >()( srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eR32_SFLOAT:
				BufferConverter< PF, PixelFormat::eR32_SFLOAT >()( srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eR8A8_UNORM:
				BufferConverter< PF, PixelFormat::eR8A8_UNORM >()( srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eR16A16_SFLOAT:
				BufferConverter< PF, PixelFormat::eR16A16_SFLOAT >()( srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eR32A32_SFLOAT:
				BufferConverter< PF, PixelFormat::eR32A32_SFLOAT >()( srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eR5G5B5A1_UNORM:
				BufferConverter< PF, PixelFormat::eR5G5B5A1_UNORM >()( srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eR5G6B5_UNORM:
				BufferConverter< PF, PixelFormat::eR5G6B5_UNORM >()( srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eR8G8B8_UNORM:
				BufferConverter< PF, PixelFormat::eR8G8B8_UNORM >()( srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eB8G8R8_UNORM:
				BufferConverter< PF, PixelFormat::eB8G8R8_UNORM >()( srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eB8G8R8A8_UNORM:
				BufferConverter< PF, PixelFormat::eB8G8R8A8_UNORM >()( srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eR8G8B8_SRGB:
				BufferConverter< PF, PixelFormat::eR8G8B8_SRGB >()( srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eB8G8R8_SRGB:
				BufferConverter< PF, PixelFormat::eB8G8R8_SRGB >()( srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eR8G8B8A8_UNORM:
				BufferConverter< PF, PixelFormat::eR8G8B8A8_UNORM >()( srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eA8B8G8R8_UNORM:
				BufferConverter< PF, PixelFormat::eA8B8G8R8_UNORM >()( srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eR8G8B8A8_SRGB:
				BufferConverter< PF, PixelFormat::eR8G8B8A8_SRGB >()( srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eA8B8G8R8_SRGB:
				BufferConverter< PF, PixelFormat::eA8B8G8R8_SRGB >()( srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eR16G16B16_SFLOAT:
				BufferConverter< PF, PixelFormat::eR16G16B16_SFLOAT >()( srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eR16G16B16A16_SFLOAT:
				BufferConverter< PF, PixelFormat::eR16G16B16A16_SFLOAT >()( srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eR32G32B32_SFLOAT:
				BufferConverter< PF, PixelFormat::eR32G32B32_SFLOAT >()( srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eR32G32B32A32_SFLOAT:
				BufferConverter< PF, PixelFormat::eR32G32B32A32_SFLOAT >()( srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eBC1_RGB_UNORM_BLOCK:
				BufferConverter< PF, PixelFormat::eBC1_RGB_UNORM_BLOCK >()( srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eBC1_RGB_SRGB_BLOCK:
				BufferConverter< PF, PixelFormat::eBC1_RGB_SRGB_BLOCK >()( srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eBC3_UNORM_BLOCK:
				BufferConverter< PF, PixelFormat::eBC3_UNORM_BLOCK >()( srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eBC3_SRGB_BLOCK:
				BufferConverter< PF, PixelFormat::eBC3_SRGB_BLOCK >()( srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			default:
				if ( PF == dstFormat )
				{
					BufferConverter< PF, PF >()( srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				}
				else
				{
					CU_UnsupportedError( "No conversion defined" );
				}
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
		void dynamicDepthBufferConversion( Size const & srcDimensions
			, Size const & dstDimensions
			, uint8_t const * srcBuffer
			, uint32_t srcSize
			, PixelFormat dstFormat
			, uint8_t * dstBuffer
			, uint32_t dstSize )
		{
			switch ( dstFormat )
			{
			case PixelFormat::eD16_UNORM:
				BufferConverter< PF, PixelFormat::eD16_UNORM >()( srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eD24_UNORM_S8_UINT:
				BufferConverter< PF, PixelFormat::eD24_UNORM_S8_UINT >()( srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eD32_UNORM:
				BufferConverter< PF, PixelFormat::eD32_UNORM >()( srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eD32_SFLOAT:
				BufferConverter< PF, PixelFormat::eD32_SFLOAT >()( srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eD32_SFLOAT_S8_UINT:
				BufferConverter< PF, PixelFormat::eD32_SFLOAT_S8_UINT >()( srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eR8G8B8A8_UNORM:
				BufferConverter< PF, PixelFormat::eR8G8B8A8_UNORM >()( srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eR32G32B32_SFLOAT:
				BufferConverter< PF, PixelFormat::eR32G32B32_SFLOAT >()( srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eR8_UNORM:
				BufferConverter< PF, PixelFormat::eR8_UNORM >()( srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eS8_UINT:
				BufferConverter< PF, PixelFormat::eS8_UINT >()( srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			default:
				if ( PF == dstFormat )
				{
					BufferConverter< PF, PF >()( srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				}
				else
				{
					CU_UnsupportedError( "No conversion defined" );
				}
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
		void dynamicStencilBufferConversion( Size const & srcDimensions
			, Size const & dstDimensions
			, uint8_t const * srcBuffer
			, uint32_t srcSize
			, PixelFormat dstFormat
			, uint8_t * dstBuffer
			, uint32_t dstSize )
		{
			switch ( dstFormat )
			{
			case PixelFormat::eD24_UNORM_S8_UINT:
				BufferConverter< PF, PixelFormat::eD24_UNORM_S8_UINT >()( srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			case PixelFormat::eS8_UINT:
				BufferConverter< PF, PixelFormat::eS8_UINT >()( srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				break;

			default:
				if ( PF == dstFormat )
				{
					BufferConverter< PF, PF >()( srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
				}
				else
				{
					CU_UnsupportedError( "No conversion defined" );
				}
				break;
			}
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
		static String toString()
		{
			return cuT( "8 bits luminosity" );
		}
		static String toStr()
		{
			return cuT( "l8" );
		}
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			details::dynamicColourConversion< PixelFormat::eR8_UNORM >( srcBuffer, dstBuffer, dstFormat );
		}
		static void convert( castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			details::dynamicColourBufferConversion< PixelFormat::eR8_UNORM >( srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eR8_UNORM, PF >()( srcBuffer, dstBuffer );
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
		static String toString()
		{
			return cuT( "Floats luminosity" );
		}
		static String toStr()
		{
			return cuT( "l32f" );
		}
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			details::dynamicColourConversion< PixelFormat::eR32_SFLOAT >( srcBuffer, dstBuffer, dstFormat );
		}
		static void convert( castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			details::dynamicColourBufferConversion< PixelFormat::eR32_SFLOAT >( srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eR32_SFLOAT, PF >()( srcBuffer, dstBuffer );
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
		static String toString()
		{
			return cuT( "Half floats luminosity" );
		}
		static String toStr()
		{
			return cuT( "l16f" );
		}
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
		}
		static void convert( castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eR16_SFLOAT, PF >()( srcBuffer, dstBuffer );
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
		static String toString()
		{
			return cuT( "16 bits luminosity and alpha" );
		}
		static String toStr()
		{
			return cuT( "al16" );
		}
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			details::dynamicColourConversion< PixelFormat::eR8A8_UNORM >( srcBuffer, dstBuffer, dstFormat );
		}
		static void convert( castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			details::dynamicColourBufferConversion< PixelFormat::eR8A8_UNORM >( srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eR8A8_UNORM, PF >()( srcBuffer, dstBuffer );
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
		static String toString()
		{
			return cuT( "Half floats luminosity and alpha" );
		}
		static String toStr()
		{
			return cuT( "al16f" );
		}
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
		}
		static void convert( castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eR16A16_SFLOAT, PF >()( srcBuffer, dstBuffer );
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
		static String toString()
		{
			return cuT( "Floats luminosity and alpha" );
		}
		static String toStr()
		{
			return cuT( "al32f" );
		}
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			details::dynamicColourConversion< PixelFormat::eR32A32_SFLOAT >( srcBuffer, dstBuffer, dstFormat );
		}
		static void convert( castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			details::dynamicColourBufferConversion< PixelFormat::eR32A32_SFLOAT >( srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eR32A32_SFLOAT, PF >()( srcBuffer, dstBuffer );
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
		static String toString()
		{
			return cuT( "16 bits 5551 ARGB" );
		}
		static String toStr()
		{
			return cuT( "argb1555" );
		}
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			details::dynamicColourConversion< PixelFormat::eR5G5B5A1_UNORM >( srcBuffer, dstBuffer, dstFormat );
		}
		static void convert( castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			details::dynamicColourBufferConversion< PixelFormat::eR5G5B5A1_UNORM >( srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eR5G5B5A1_UNORM, PF >()( srcBuffer, dstBuffer );
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
		static String toString()
		{
			return cuT( "16 bits 565 RGB" );
		}
		static String toStr()
		{
			return cuT( "rgb565" );
		}
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			details::dynamicColourConversion< PixelFormat::eR5G6B5_UNORM >( srcBuffer, dstBuffer, dstFormat );
		}
		static void convert( castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			details::dynamicColourBufferConversion< PixelFormat::eR5G6B5_UNORM >( srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eR5G6B5_UNORM, PF >()( srcBuffer, dstBuffer );
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
		static String toString()
		{
			return cuT( "24 bits 888 RGB" );
		}
		static String toStr()
		{
			return cuT( "rgb24" );
		}
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			details::dynamicColourConversion< PixelFormat::eR8G8B8_UNORM >( srcBuffer, dstBuffer, dstFormat );
		}
		static void convert( castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			details::dynamicColourBufferConversion< PixelFormat::eR8G8B8_UNORM >( srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eR8G8B8_UNORM, PF >()( srcBuffer, dstBuffer );
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
		static String toString()
		{
			return cuT( "24 bits 888 BGR" );
		}
		static String toStr()
		{
			return cuT( "bgr24" );
		}
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			details::dynamicColourConversion< PixelFormat::eB8G8R8_UNORM >( srcBuffer, dstBuffer, dstFormat );
		}
		static void convert( castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			details::dynamicColourBufferConversion< PixelFormat::eB8G8R8_UNORM >( srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eB8G8R8_UNORM, PF >()( srcBuffer, dstBuffer );
		}
	};

	//*************************************************************************************************

	//!\~english	Specialisation for PixelFormat::eB8G8R8A8_UNORM
	//!\~french		Spécialisation pour PixelFormat::eB8G8R8A8_UNORM
	template<>
	struct PixelDefinitions< PixelFormat::eB8G8R8A8_UNORM >
	{
		static const uint8_t Size = 4;
		static const uint8_t Count = 4;
		static const bool Alpha = true;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = false;
		static const PixelFormat NoAlphaPF = PixelFormat::eB8G8R8_UNORM;
		static String toString()
		{
			return cuT( "32 bits 8888 BGRA" );
		}
		static String toStr()
		{
			return cuT( "bgra32" );
		}
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			details::dynamicColourConversion< PixelFormat::eB8G8R8A8_UNORM >( srcBuffer, dstBuffer, dstFormat );
		}
		static void convert( castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			details::dynamicColourBufferConversion< PixelFormat::eB8G8R8A8_UNORM >( srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eB8G8R8A8_UNORM, PF >()( srcBuffer, dstBuffer );
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
		static String toString()
		{
			return cuT( "24 bits 888 RGB sRGB" );
		}
		static String toStr()
		{
			return cuT( "rgb24_srgb" );
		}
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			details::dynamicColourConversion< PixelFormat::eR8G8B8_SRGB >( srcBuffer, dstBuffer, dstFormat );
		}
		static void convert( castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			details::dynamicColourBufferConversion< PixelFormat::eR8G8B8_SRGB >( srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eR8G8B8_SRGB, PF >()( srcBuffer, dstBuffer );
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
		static String toString()
		{
			return cuT( "24 bits 888 BGR sRGB" );
		}
		static String toStr()
		{
			return cuT( "bgr24_srgb" );
		}
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			details::dynamicColourConversion< PixelFormat::eB8G8R8_SRGB >( srcBuffer, dstBuffer, dstFormat );
		}
		static void convert( castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			details::dynamicColourBufferConversion< PixelFormat::eB8G8R8_SRGB >( srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eB8G8R8_SRGB, PF >()( srcBuffer, dstBuffer );
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
		static String toString()
		{
			return cuT( "32 bits 8888 ARGB" );
		}
		static String toStr()
		{
			return cuT( "argb32" );
		}
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			details::dynamicColourConversion< PixelFormat::eR8G8B8A8_UNORM >( srcBuffer, dstBuffer, dstFormat );
		}
		static void convert( castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			details::dynamicColourBufferConversion< PixelFormat::eR8G8B8A8_UNORM >( srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eR8G8B8A8_UNORM, PF >()( srcBuffer, dstBuffer );
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
		static String toString()
		{
			return cuT( "32 bits 8888 ABGR" );
		}
		static String toStr()
		{
			return cuT( "abgr32" );
		}
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			details::dynamicColourConversion< PixelFormat::eA8B8G8R8_UNORM >( srcBuffer, dstBuffer, dstFormat );
		}
		static void convert( castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			details::dynamicColourBufferConversion< PixelFormat::eA8B8G8R8_UNORM >( srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eA8B8G8R8_UNORM, PF >()( srcBuffer, dstBuffer );
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
		static String toString()
		{
			return cuT( "32 bits 8888 ARGB sRGB" );
		}
		static String toStr()
		{
			return cuT( "argb32_srgb" );
		}
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			details::dynamicColourConversion< PixelFormat::eR8G8B8A8_SRGB >( srcBuffer, dstBuffer, dstFormat );
		}
		static void convert( castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			details::dynamicColourBufferConversion< PixelFormat::eR8G8B8A8_SRGB >( srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eR8G8B8A8_SRGB, PF >()( srcBuffer, dstBuffer );
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
		static String toString()
		{
			return cuT( "32 bits 8888 ABGR sRGB" );
		}
		static String toStr()
		{
			return cuT( "abgr32_srgb" );
		}
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			details::dynamicColourConversion< PixelFormat::eA8B8G8R8_SRGB >( srcBuffer, dstBuffer, dstFormat );
		}
		static void convert( castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			details::dynamicColourBufferConversion< PixelFormat::eA8B8G8R8_SRGB >( srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eA8B8G8R8_SRGB, PF >()( srcBuffer, dstBuffer );
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
		static String toString()
		{
			return cuT( "Half floating point RGB" );
		}
		static String toStr()
		{
			return cuT( "rgb16f" );
		}
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			details::dynamicColourConversion< PixelFormat::eR16G16B16_SFLOAT >( srcBuffer, dstBuffer, dstFormat );
		}
		static void convert( castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			details::dynamicColourBufferConversion< PixelFormat::eR16G16B16_SFLOAT >( srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eR16G16B16_SFLOAT, PF >()( srcBuffer, dstBuffer );
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
		static String toString()
		{
			return cuT( "Half floating point ARGB" );
		}
		static String toStr()
		{
			return cuT( "argb16f" );
		}
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			details::dynamicColourConversion< PixelFormat::eR16G16B16A16_SFLOAT >( srcBuffer, dstBuffer, dstFormat );
		}
		static void convert( castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			details::dynamicColourBufferConversion< PixelFormat::eR16G16B16A16_SFLOAT >( srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eR16G16B16A16_SFLOAT, PF >()( srcBuffer, dstBuffer );
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
		static String toString()
		{
			return cuT( "Floating point RGB" );
		}
		static String toStr()
		{
			return cuT( "rgb32f" );
		}
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			details::dynamicColourConversion< PixelFormat::eR32G32B32_SFLOAT >( srcBuffer, dstBuffer, dstFormat );
		}
		static void convert( castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			details::dynamicColourBufferConversion< PixelFormat::eR32G32B32_SFLOAT >( srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eR32G32B32_SFLOAT, PF >()( srcBuffer, dstBuffer );
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
		static String toString()
		{
			return cuT( "Floating point ARGB" );
		}
		static String toStr()
		{
			return cuT( "argb32f" );
		}
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			details::dynamicColourConversion< PixelFormat::eR32G32B32A32_SFLOAT >( srcBuffer, dstBuffer, dstFormat );
		}
		static void convert( castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			details::dynamicColourBufferConversion< PixelFormat::eR32G32B32A32_SFLOAT >( srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eR32G32B32A32_SFLOAT, PF >()( srcBuffer, dstBuffer );
		}
	};

	//*************************************************************************************************

	//!\~english	Specialisation for PixelFormat::eBC1_RGB_UNORM_BLOCK
	//!\~french		Spécialisation pour PixelFormat::eBC1_RGB_UNORM_BLOCK
	template<>
	struct PixelDefinitions< PixelFormat::eBC1_RGB_UNORM_BLOCK >
	{
		static const uint8_t Size = 8u; // Bit count for a 4x4 pixels block
		static const uint8_t Count = 3;
		static const bool Alpha = false;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = true;
		static String toString()
		{
			return cuT( "DXT1 8 bits compressed format" );
		}
		static String toStr()
		{
			return cuT( "dxtc1" );
		}
		static void convert( uint8_t const * srcBuffer, uint8_t * dstBuffer, PixelFormat dstFormat )
		{
			if ( dstFormat == PixelFormat::eBC1_RGB_UNORM_BLOCK )
			{
				details::PixelConverter< PixelFormat::eBC1_RGB_UNORM_BLOCK, PixelFormat::eBC1_RGB_UNORM_BLOCK >()( srcBuffer, dstBuffer );
			}
		}
		static void convert( castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			if ( dstFormat == PixelFormat::eBC1_RGB_UNORM_BLOCK )
			{
				details::BufferConverter< PixelFormat::eBC1_RGB_UNORM_BLOCK, PixelFormat::eBC1_RGB_UNORM_BLOCK >()( srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
			}
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eBC1_RGB_UNORM_BLOCK, PF >()( srcBuffer, dstBuffer );
		}
	};

	//*************************************************************************************************

	//!\~english	Specialisation for PixelFormat::eBC1_RGBA_UNORM_BLOCK
	//!\~french		Spécialisation pour PixelFormat::eBC1_RGBA_UNORM_BLOCK
	template<>
	struct PixelDefinitions< PixelFormat::eBC1_RGBA_UNORM_BLOCK >
	{
		static const uint8_t Size = 8u; // Bit count for a 4x4 pixels block
		static const uint8_t Count = 4;
		static const bool Alpha = true;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = true;
		static String toString()
		{
			return cuT( "BC1 8 bits compressed format" );
		}
		static String toStr()
		{
			return cuT( "bc1" );
		}
		static void convert( uint8_t const * srcBuffer, uint8_t * dstBuffer, PixelFormat dstFormat )
		{
			if ( dstFormat == PixelFormat::eBC1_RGBA_UNORM_BLOCK )
			{
				details::PixelConverter< PixelFormat::eBC1_RGBA_UNORM_BLOCK, PixelFormat::eBC1_RGBA_UNORM_BLOCK >()( srcBuffer, dstBuffer );
			}
		}
		static void convert( castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			if ( dstFormat == PixelFormat::eBC1_RGBA_UNORM_BLOCK )
			{
				details::BufferConverter< PixelFormat::eBC1_RGBA_UNORM_BLOCK, PixelFormat::eBC1_RGBA_UNORM_BLOCK >()( srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
			}
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eBC1_RGBA_UNORM_BLOCK, PF >()( srcBuffer, dstBuffer );
		}
	};

	//*************************************************************************************************

	//!\~english	Specialisation for PixelFormat::eBC3_UNORM_BLOCK
	//!\~french		Spécialisation pour PixelFormat::eBC3_UNORM_BLOCK
	template<>
	struct PixelDefinitions< PixelFormat::eBC3_UNORM_BLOCK >
	{
		static const uint8_t Size = 16u; // Bit count for a 4x4 pixels block
		static const uint8_t Count = 4;
		static const bool Alpha = true;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = true;
		static const PixelFormat NoAlphaPF = PixelFormat::eBC3_UNORM_BLOCK;
		static String toString()
		{
			return cuT( "DXT3 16 bits compressed format" );
		}
		static String toStr()
		{
			return cuT( "dxtc3" );
		}
		static void convert( uint8_t const * srcBuffer, uint8_t * dstBuffer, PixelFormat dstFormat )
		{
			if ( dstFormat == PixelFormat::eBC3_UNORM_BLOCK )
			{
				details::PixelConverter< PixelFormat::eBC3_UNORM_BLOCK, PixelFormat::eBC3_UNORM_BLOCK >()( srcBuffer, dstBuffer );
			}
		}
		static void convert( castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			if ( dstFormat == PixelFormat::eBC3_UNORM_BLOCK )
			{
				details::BufferConverter< PixelFormat::eBC3_UNORM_BLOCK, PixelFormat::eBC3_UNORM_BLOCK >()( srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
			}
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eBC3_UNORM_BLOCK, PF >()( srcBuffer, dstBuffer );
		}
	};

	//*************************************************************************************************

	//!\~english	Specialisation for PixelFormat::eBC5_UNORM_BLOCK
	//!\~french		Spécialisation pour PixelFormat::eBC5_UNORM_BLOCK
	template<>
	struct PixelDefinitions< PixelFormat::eBC5_UNORM_BLOCK >
	{
		static const uint8_t Size = 16u; // Bit count for a 4x4 pixels block
		static const uint8_t Count = 4;
		static const bool Alpha = true;
		static const bool Colour = true;
		static const bool Depth = false;
		static const bool Stencil = false;
		static const bool Compressed = true;
		static const PixelFormat NoAlphaPF = PixelFormat::eBC5_UNORM_BLOCK;
		static String toString()
		{
			return cuT( "DXT5 16 bits compressed format" );
		}
		static String toStr()
		{
			return cuT( "dxtc5" );
		}
		static void convert( uint8_t const * srcBuffer, uint8_t * dstBuffer, PixelFormat dstFormat )
		{
			if ( dstFormat == PixelFormat::eBC5_UNORM_BLOCK )
			{
				details::PixelConverter< PixelFormat::eBC5_UNORM_BLOCK, PixelFormat::eBC5_UNORM_BLOCK >()( srcBuffer, dstBuffer );
			}
		}
		static void convert( castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			if ( dstFormat == PixelFormat::eBC5_UNORM_BLOCK )
			{
				details::BufferConverter< PixelFormat::eBC5_UNORM_BLOCK, PixelFormat::eBC5_UNORM_BLOCK >()( srcDimensions, dstDimensions, srcBuffer, srcSize, dstBuffer, dstSize );
			}
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eBC5_UNORM_BLOCK, PF >()( srcBuffer, dstBuffer );
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
		static String toString()
		{
			return cuT( "16 bits depth" );
		}
		static String toStr()
		{
			return cuT( "depth16" );
		}
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			details::dynamicDepthConversion< PixelFormat::eD16_UNORM >( srcBuffer, dstBuffer, dstFormat );
		}
		static void convert( castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			details::dynamicDepthBufferConversion< PixelFormat::eD16_UNORM >( srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eD16_UNORM, PF >()( srcBuffer, dstBuffer );
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
		static String toString()
		{
			return cuT( "24 bits depth, 8 bits stencil" );
		}
		static String toStr()
		{
			return cuT( "depth24s8" );
		}
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			details::dynamicDepthConversion< PixelFormat::eD24_UNORM_S8_UINT >( srcBuffer, dstBuffer, dstFormat );
		}
		static void convert( castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			details::dynamicDepthBufferConversion< PixelFormat::eD24_UNORM_S8_UINT >( srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter<PixelFormat::eD24_UNORM_S8_UINT, PF>()( srcBuffer, dstBuffer );
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
		static String toString()
		{
			return cuT( "32 bits depth" );
		}
		static String toStr()
		{
			return cuT( "depth32" );
		}
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			details::dynamicDepthConversion< PixelFormat::eD32_UNORM >( srcBuffer, dstBuffer, dstFormat );
		}
		static void convert( castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			details::dynamicDepthBufferConversion< PixelFormat::eD32_UNORM >( srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eD32_UNORM, PF >()( srcBuffer, dstBuffer );
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
		static String toString()
		{
			return cuT( "32 bits floating point depth" );
		}
		static String toStr()
		{
			return cuT( "depth32f" );
		}
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			details::dynamicDepthConversion< PixelFormat::eD32_SFLOAT >( srcBuffer, dstBuffer, dstFormat );
		}
		static void convert( castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			details::dynamicDepthBufferConversion< PixelFormat::eD32_SFLOAT >( srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eD32_SFLOAT, PF >()( srcBuffer, dstBuffer );
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
		static String toString()
		{
			return cuT( "32 bits floating point depth, 8 bits stencil" );
		}
		static String toStr()
		{
			return cuT( "depth32fs8" );
		}
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			details::dynamicDepthConversion< PixelFormat::eD32_SFLOAT_S8_UINT >( srcBuffer, dstBuffer, dstFormat );
		}
		static void convert( castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			details::dynamicDepthBufferConversion< PixelFormat::eD32_SFLOAT_S8_UINT >( srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eD32_SFLOAT_S8_UINT, PF >()( srcBuffer, dstBuffer );
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
		static String toString()
		{
			return cuT( "8 bits stencil" );
		}
		static String toStr()
		{
			return cuT( "stencil8" );
		}
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer, PixelFormat dstFormat )
		{
			details::dynamicStencilConversion< PixelFormat::eS8_UINT >( srcBuffer, dstBuffer, dstFormat );
		}
		static void convert( castor::Size const & srcDimensions, castor::Size const & dstDimensions, uint8_t const *& srcBuffer, uint32_t srcSize, PixelFormat dstFormat, uint8_t *& dstBuffer, uint32_t dstSize )
		{
			details::dynamicStencilBufferConversion< PixelFormat::eS8_UINT >( srcDimensions, dstDimensions, srcBuffer, srcSize, dstFormat, dstBuffer, dstSize );
		}
		template< PixelFormat PF >
		static void convert( uint8_t const *& srcBuffer, uint8_t *& dstBuffer )
		{
			details::PixelConverter< PixelFormat::eS8_UINT, PF >()( srcBuffer, dstBuffer );
		}
	};

	//*************************************************************************************************
}
