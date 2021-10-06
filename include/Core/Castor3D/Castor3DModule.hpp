/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Castor3DModule_H___
#define ___C3D_Castor3DModule_H___

#undef RGB

#include <CastorUtils/Multithreading/MultithreadingModule.hpp>
#include <CastorUtils/Design/FlagCombination.hpp>
#include <CastorUtils/Design/OwnedBy.hpp>
#include <CastorUtils/Exception/Assertion.hpp>
#include <CastorUtils/Graphics/GraphicsModule.hpp>
#include <CastorUtils/Graphics/Size.hpp>
#include <CastorUtils/Math/Coords.hpp>
#include <CastorUtils/Math/Point.hpp>

#include <ashespp/AshesPPPrerequisites.hpp>
#include <RenderGraph/FrameGraphPrerequisites.hpp>

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#if !defined( CU_PlatformWindows )
#	define C3D_API
#else
#	ifdef MemoryBarrier
#		undef MemoryBarrier
#	endif
#	if defined( Castor3D_EXPORTS )
#		define C3D_API __declspec( dllexport )
#	else
#		define C3D_API __declspec( dllimport )
#	endif
#endif

namespace castor3d
{
	static uint32_t constexpr InvalidIndex = ~( 0u );
	static VkColorComponentFlags const defaultColorWriteMask{ VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT };

	using castor::xchar;
	using castor::ychar;

	using castor::UInt32StrMap;
	using castor::UInt64StrMap;
	using castor::UInt32Array;

	using crg::FramePassTimer;
	/**
	*\~english
	*\brief		Castor3D engine.
	*\~french
	*\brief		Moteur Castor3D.
	*/
	class Engine;
	/**
	*/
	struct CpuUpdater;
	/**
	*/
	struct GpuUpdater;
	/**
	*\~english
	*	Helper structure to specialise a cache behaviour.
	*\~french
	*	Structure permettant de spécialiser le comportement d'un cache.
	*/
	template< typename ResT, typename KeyT >
	struct PtrCacheTraitsT;
	/**
	*\~english
	*	Helper structure to specialise a cache behaviour.
	*\~french
	*	Structure permettant de spécialiser le comportement d'un cache.
	*/
	template< typename ResT, typename KeyT >
	struct ResourceCacheTraitsT;

	CU_DeclareSmartPtr( Engine );
	CU_DeclareCUSmartPtr( castor3d, FramePassTimer, C3D_API );

	C3D_API castor::LoggerInstance & getLogger( Engine & engine );

	C3D_API castor::Matrix4x4f convert( std::array< float, 16 > const & value );
	C3D_API VkClearColorValue convert( castor::RgbaColour const & value );
	C3D_API castor::RgbaColour convert( VkClearColorValue const & value );
	C3D_API std::array< float, 4u > makeFloatArray( castor::RgbaColour const & value );

	CU_DeclareVector( float, Float );
	CU_DeclareMap( uint32_t, int, IntUInt );
	CU_DeclareMap( castor::String, int, IntStr );
	CU_DeclareMap( int, castor::String, StrInt );

	inline VkDescriptorSetLayoutBinding makeDescriptorSetLayoutBinding( uint32_t binding
		, VkDescriptorType descriptorType
		, VkShaderStageFlags stageFlags
		, uint32_t descriptorCount = 1u
		, VkSampler const * pImmutableSamplers = nullptr )
	{
		return
		{
			binding,
			descriptorType,
			descriptorCount,
			stageFlags,
			pImmutableSamplers,
		};
	}

	inline castor::Size makeSize( VkExtent2D const & size )
	{
		return castor::Size{ size.width, size.height };
	}

	inline castor::Size makeSize( VkExtent3D const & size )
	{
		return castor::Size{ size.width, size.height };
	}

	inline VkExtent2D makeExtent2D( VkExtent3D const & size )
	{
		return VkExtent2D{ size.width, size.height };
	}

	inline VkExtent3D makeExtent3D( VkExtent2D const & size )
	{
		return VkExtent3D{ size.width, size.height, 1u };
	}

	inline VkExtent2D makeExtent2D( castor::Coords2ui const & size )
	{
		return VkExtent2D
		{
			size[0],
			size[1],
		};
	}

	inline VkExtent2D makeExtent2D( castor::Point2ui const & size )
	{
		return VkExtent2D
		{
			size[0],
			size[1],
		};
	}

	inline VkExtent3D makeExtent3D( castor::Coords2ui const & size )
	{
		return VkExtent3D
		{
			size[0],
			size[1],
			1u,
		};
	}

	inline VkExtent3D makeExtent3D( castor::Point2ui const & size )
	{
		return VkExtent3D
		{
			size[0],
			size[1],
			1u,
		};
	}

	inline VkOffset2D makeOffset2D( castor::Coords2i const & pos )
	{
		return VkOffset2D
		{
			pos[0],
			pos[1],
		};
	}

	inline VkOffset2D makeOffset2D( castor::Point2i const & pos )
	{
		return VkOffset2D
		{
			pos[0],
			pos[1],
		};
	}

	inline VkOffset3D makeOffset3D( castor::Coords2i const & pos )
	{
		return VkOffset3D
		{
			pos[0],
			pos[1],
			0u,
		};
	}

	inline VkOffset3D makeOffset3D( castor::Point2i const & pos )
	{
		return VkOffset3D
		{
			pos[0],
			pos[1],
			0u,
		};
	}

	inline VkViewport makeViewport( castor::Coords2ui const & size
		, float zMin = 0.0f
		, float zMax = 1.0f )
	{
		return ashes::makeViewport( {}
			, makeExtent2D( size )
			, zMin
			, zMax );
	}

	inline VkViewport makeViewport( castor::Point2ui const & size
		, float zMin = 0.0f
		, float zMax = 1.0f )
	{
		return ashes::makeViewport( {}
			, makeExtent2D( size )
			, zMin
			, zMax );
	}

	inline VkViewport makeViewport( castor::Coords2i const & pos
		, castor::Coords2ui const & size
		, float zMin = 0.0f
		, float zMax = 1.0f )
	{
		return ashes::makeViewport( makeOffset2D( pos )
			, makeExtent2D( size )
			, zMin
			, zMax );
	}

	inline VkViewport makeViewport( castor::Point2i const & pos
		, castor::Coords2ui const & size
		, float zMin = 0.0f
		, float zMax = 1.0f )
	{
		return ashes::makeViewport( makeOffset2D( pos )
			, makeExtent2D( size )
			, zMin
			, zMax );
	}

	inline VkViewport makeViewport( castor::Coords2i const & pos
		, castor::Point2ui const & size
		, float zMin = 0.0f
		, float zMax = 1.0f )
	{
		return ashes::makeViewport( makeOffset2D( pos )
			, makeExtent2D( size )
			, zMin
			, zMax );
	}

	inline VkViewport makeViewport( castor::Point2i const & pos
		, castor::Point2ui const & size
		, float zMin = 0.0f
		, float zMax = 1.0f )
	{
		return ashes::makeViewport( makeOffset2D( pos )
			, makeExtent2D( size )
			, zMin
			, zMax );
	}

	inline VkRect2D makeScissor( castor::Coords2ui const & size )
	{
		return ashes::makeScissor( {}
		, makeExtent2D( size ) );
	}

	inline VkRect2D makeScissor( castor::Point2ui const & size )
	{
		return ashes::makeScissor( {}
		, makeExtent2D( size ) );
	}

	inline VkRect2D makeScissor( castor::Coords2i const & pos
		, castor::Coords2ui const & size )
	{
		return ashes::makeScissor( makeOffset2D( pos )
			, makeExtent2D( size ) );
	}

	inline VkRect2D makeScissor( castor::Point2i const & pos
		, castor::Coords2ui const & size )
	{
		return ashes::makeScissor( makeOffset2D( pos )
			, makeExtent2D( size ) );
	}

	inline VkRect2D makeScissor( castor::Coords2i const & pos
		, castor::Point2ui const & size )
	{
		return ashes::makeScissor( makeOffset2D( pos )
			, makeExtent2D( size ) );
	}

	inline VkRect2D makeScissor( castor::Point2i const & pos
		, castor::Point2ui const & size )
	{
		return ashes::makeScissor( makeOffset2D( pos )
			, makeExtent2D( size ) );
	}

	inline VkClearValue makeClearValue( float depth, uint32_t stencil = 0u )
	{
		return ashes::makeClearValue( VkClearDepthStencilValue{ depth, stencil } );
	}

	inline VkClearValue makeClearValue( float r, float g, float b, float a = 1.0f )
	{
		return ashes::makeClearValue( { r, g, b, a } );
	}

	template< typename EnumT >
	castor::UInt32StrMap getEnumMapT( EnumT min, EnumT max )
	{
		using ashes::getName;
		castor::UInt32StrMap result;

		for ( uint32_t i = uint32_t( min ); i <= uint32_t( max ); ++i )
		{
			result[getName( EnumT( i ) )] = i;
		}

		return result;
	}

	template< typename EnumT >
	castor::UInt32StrMap getEnumMapT()
	{
		return getEnumMapT( EnumT::eMin, EnumT::eMax );
	}

	static VkClearValue const defaultClearDepthStencil{ makeClearValue( 1.0f, 0u ) };
	static VkClearValue const opaqueBlackClearColor{ makeClearValue( 0.0f, 0.0f, 0.0f, 1.0f ) };
	static VkClearValue const transparentBlackClearColor{ makeClearValue( 0.0f, 0.0f, 0.0f, 0.0f ) };
	static VkClearValue const opaqueWhiteClearColor{ makeClearValue( 1.0f, 1.0f, 1.0f, 1.0f ) };
	static VkClearValue const transparentWhiteClearColor{ makeClearValue( 1.0f, 1.0f, 1.0f, 0.0f ) };

	inline bool operator>( VkImageSubresourceRange const & lhs
		, VkImageSubresourceRange const & rhs )
	{
		return lhs.aspectMask > rhs.aspectMask
			|| ( lhs.aspectMask == rhs.aspectMask
				&& ( lhs.baseArrayLayer > rhs.baseArrayLayer
					|| ( lhs.baseArrayLayer == rhs.baseArrayLayer
						&& ( lhs.layerCount > rhs.layerCount
							|| ( lhs.layerCount == rhs.layerCount
								&& ( lhs.baseMipLevel > rhs.baseMipLevel
									|| ( lhs.baseMipLevel == rhs.baseMipLevel
										&& lhs.levelCount > rhs.levelCount
										)
									)
								)
							)
						)
					)
				);
	}

	inline bool operator<( VkImageSubresourceRange const & lhs
		, VkImageSubresourceRange const & rhs )
	{
		return lhs.aspectMask < rhs.aspectMask
			|| ( lhs.aspectMask == rhs.aspectMask
				&& ( lhs.baseArrayLayer < rhs.baseArrayLayer
					|| ( lhs.baseArrayLayer == rhs.baseArrayLayer
						&& ( lhs.layerCount < rhs.layerCount
							|| ( lhs.layerCount == rhs.layerCount
								&& ( lhs.baseMipLevel < rhs.baseMipLevel
									|| ( lhs.baseMipLevel == rhs.baseMipLevel
										&& lhs.levelCount < rhs.levelCount
										)
									)
								)
							)
						)
					)
				);
	}

	inline bool operator<=( VkImageSubresourceRange const & lhs
		, VkImageSubresourceRange const & rhs )
	{
		return !( lhs > rhs );
	}

	inline bool operator>=( VkImageSubresourceRange const & lhs
		, VkImageSubresourceRange const & rhs )
	{
		return !( lhs < rhs );
	}

	inline bool operator!=( VkImageSubresourceRange const & lhs
		, VkImageSubresourceRange const & rhs )
	{
		return lhs < rhs || lhs > rhs;
	}

	inline bool operator==( VkImageSubresourceRange const & lhs
		, VkImageSubresourceRange const & rhs )
	{
		return lhs >= rhs && lhs <= rhs;
	}
}

CU_DeclareExportedOwnedBy( C3D_API, castor3d::Engine, Engine )

#endif
