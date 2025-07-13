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
#include <ShaderAST/ShaderStlTypes.hpp>

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

#define C3D_Stringify( x ) C3DX_Stringify(x)
#define C3DX_Stringify( x ) #x
#define C3D_Join2Strings( l, r ) l cuT( "." ) r
#define C3D_Join3Strings( l, m, r ) C3D_Join2Strings(l, C3D_Join2Strings(m, r))
#define C3D_Join4Strings( l, ml, mr, r ) C3D_Join3Strings(l, ml, C3D_Join2Strings(mr, r))

namespace castor3d
{
	static uint32_t constexpr InvalidIndex = ~0u;
	static crg::ColorComponentFlags const defaultColorWriteMask{ crg::ColorComponentFlags::eR | crg::ColorComponentFlags::eG | crg::ColorComponentFlags::eB | crg::ColorComponentFlags::eA };

	using crg::AccessFlags;
	using crg::AttachmentLoadOp;
	using crg::AttachmentStoreOp;
	using crg::BlendFactor;
	using crg::BlendOp;
	using crg::ColorComponentFlags;
	using crg::FilterMode;
	using crg::ImageAspectFlags;
	using crg::ImageCreateFlags;
	using crg::ImageLayout;
	using crg::ImageTiling;
	using crg::ImageType;
	using crg::ImageUsageFlags;
	using crg::ImageViewCreateFlags;
	using crg::ImageViewType;
	using crg::MipmapMode;
	using crg::PipelineStageFlags;
	using crg::SampleCount;
	using crg::WrapMode;

	using crg::AccessState;
	using crg::ClearColorValue;
	using crg::ClearDepthStencilValue;
	using crg::ClearValue;
	using crg::Extent2D;
	using crg::Extent3D;
	using crg::ImageCreateInfo;
	using crg::ImageSubresourceRange;
	using crg::ImageViewCreateInfo;
	using crg::LayoutState;
	using crg::Offset2D;
	using crg::Offset3D;
	using crg::PipelineState;
	using crg::Rect2D;
	using crg::SemaphoreWait;
	using crg::SemaphoreWaitArray;

	static constexpr AccessState VertexAttributeInputState{ AccessFlags::eVertexAttributeRead, PipelineStageFlags::eVertexInput };
	static constexpr AccessState VertexIndexInputState{ AccessFlags::eIndexRead, PipelineStageFlags::eVertexInput };
	static constexpr AccessState FragmentShaderReadState{ AccessFlags::eShaderRead, PipelineStageFlags::eFragmentShader };
	static constexpr AccessState VertexShaderReadState{ AccessFlags::eShaderRead, PipelineStageFlags::eVertexShader };
	static constexpr AccessState ComputeShaderReadState{ AccessFlags::eShaderRead, PipelineStageFlags::eComputeShader };
	static constexpr AccessState ComputeShaderReadWriteState{ AccessFlags::eShaderRead | AccessFlags::eShaderWrite, PipelineStageFlags::eComputeShader };
	static constexpr AccessState ComputeShaderWriteState{ AccessFlags::eShaderWrite, PipelineStageFlags::eComputeShader };
	static constexpr AccessState MeshShaderReadState{ AccessFlags::eShaderRead, PipelineStageFlags::eMeshShader };
	static constexpr AccessState TaskShaderReadState{ AccessFlags::eShaderRead, PipelineStageFlags::eTaskShader };
	static constexpr AccessState FragmentUniformReadState{ AccessFlags::eUniformRead, PipelineStageFlags::eFragmentShader };
	static constexpr AccessState VertexUniformReadState{ AccessFlags::eUniformRead, PipelineStageFlags::eVertexShader };
	static constexpr AccessState ComputeUniformReadState{ AccessFlags::eUniformRead, PipelineStageFlags::eComputeShader };
	static constexpr AccessState MeshUniformReadState{ AccessFlags::eUniformRead, PipelineStageFlags::eMeshShader };
	static constexpr AccessState TaskUniformReadState{ AccessFlags::eUniformRead, PipelineStageFlags::eTaskShader };
	static constexpr AccessState ComputeIndirectCommandReadState{ AccessFlags::eIndirectCommandRead, PipelineStageFlags::eComputeShader };
	static constexpr AccessState DrawIndirectCommandState{ AccessFlags::eIndirectCommandRead, PipelineStageFlags::eDrawIndirect };
	static constexpr AccessState TransferReadState{ AccessFlags::eTransferRead, PipelineStageFlags::eTransfer };
	static constexpr AccessState TransferWriteState{ AccessFlags::eTransferWrite, PipelineStageFlags::eTransfer };
	static constexpr AccessState HostWriteState{ AccessFlags::eHostWrite, PipelineStageFlags::eHost };

	using castor::StringHash;
	using castor::xchar;

	using castor::s8;
	using castor::u8;
	using castor::byte;
	using castor::s16;
	using castor::u16;
	using castor::s32;
	using castor::u32;
	using castor::s64;
	using castor::u64;
	using castor::usize;
	using castor::f32;
	using castor::f64;

	using castor::UInt32StrMap;
	using castor::UInt64StrMap;
	using castor::UInt32Array;

	using crg::FramePassTimer;

	using BytePtr = uint8_t *;
	using ByteCPtr = uint8_t const *;
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
	*\brief
	*	Base class for external file import
	*\~french
	*\brief
	*	Classe de base pour l'import de fichiers externes
	*/
	class ImporterFile;
	/**
	*\~english
	*\brief
	*	Base class for external file import
	*\~french
	*\brief
	*	Classe de base pour l'import de fichiers externes
	*/
	class ImporterFileFactory;
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

	/** @cond !Doxygen */
	CU_DeclareSmartPtr( castor3d, Engine, C3D_API );
	CU_DeclareSmartPtr( castor3d, ImporterFile, C3D_API );
	CU_DeclareSmartPtr( castor3d, FramePassTimer, C3D_API );
	CU_DeclareSmartPtr( castor3d, ImporterFileFactory, C3D_API );

	CU_DeclareVector( float, Float );
	CU_DeclareMap( uint32_t, int, IntUInt );
	CU_DeclareMap( castor::String, int, IntStr );
	CU_DeclareMap( int, castor::String, StrInt );
	/** @endcond */

	C3D_API castor::LoggerInstance & getLogger( Engine const & engine );

	C3D_API castor::Matrix4x4f convert( castor::Array< float, 16 > const & value );
	C3D_API VkClearColorValue convert( castor::RgbaColour const & value );
	C3D_API castor::RgbaColour convert( VkClearColorValue const & value );
	C3D_API castor::Array< float, 4u > makeFloatArray( castor::RgbaColour const & value );

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

	inline void addDescriptorSetLayoutBinding( ashes::VkDescriptorSetLayoutBindingArray & bindings
		, uint32_t & binding
		, VkDescriptorType descriptorType
		, VkShaderStageFlags stageFlags
		, uint32_t descriptorCount = 1u
		, VkSampler const * pImmutableSamplers = nullptr )
	{
		bindings.push_back( makeDescriptorSetLayoutBinding( binding
			, descriptorType
			, stageFlags
			, descriptorCount
			, pImmutableSamplers ) );
		++binding;
	}

	inline castor::Size makeSize( VkExtent2D const & size )
	{
		return castor::Size{ size.width, size.height };
	}

	inline castor::Size makeSize( Extent2D const & size )
	{
		return castor::Size{ size.width, size.height };
	}

	inline castor::Size makeSize( VkExtent3D const & size )
	{
		return castor::Size{ size.width, size.height };
	}

	inline castor::Size makeSize( Extent3D const & size )
	{
		return castor::Size{ size.width, size.height };
	}

	inline VkExtent2D makeExtent2D( VkExtent3D const & size )
	{
		return VkExtent2D{ size.width, size.height };
	}

	inline Extent2D makeExtent2D( Extent3D const & size )
	{
		return Extent2D{ size.width, size.height };
	}

	inline VkExtent3D makeExtent3D( VkExtent2D const & size )
	{
		return VkExtent3D{ size.width, size.height, 1u };
	}

	inline Extent3D makeExtent3D( Extent2D const & size )
	{
		return Extent3D{ size.width, size.height, 1u };
	}

	inline VkExtent3D makeVkExtent3D( Extent2D const & size )
	{
		return VkExtent3D{ size.width, size.height, 1u };
	}

	inline Extent2D makeExtent2D( castor::Coords2ui const & size )
	{
		return Extent2D
		{
			size[0],
			size[1],
		};
	}

	inline VkExtent2D makeVkExtent2D( castor::Coords2ui const & size )
	{
		return VkExtent2D
		{
			size[0],
			size[1],
		};
	}

	inline Extent2D makeExtent2D( castor::Point2ui const & size )
	{
		return Extent2D
		{
			size[0],
			size[1],
		};
	}

	inline Extent3D makeExtent3D( castor::Coords2ui const & size )
	{
		return Extent3D
		{
			size[0],
			size[1],
			1u,
		};
	}

	inline VkExtent3D makeVkExtent3D( castor::Coords2ui const & size )
	{
		return VkExtent3D
		{
			size[0],
			size[1],
			1u,
		};
	}

	inline Extent3D makeExtent3D( castor::Point2ui const & size )
	{
		return Extent3D
		{
			size[0],
			size[1],
			1u,
		};
	}

	inline Offset2D makeOffset2D( castor::Coords2i const & pos )
	{
		return Offset2D
		{
			pos[0],
			pos[1],
		};
	}

	inline Offset2D makeOffset2D( castor::Point2i const & pos )
	{
		return Offset2D
		{
			pos[0],
			pos[1],
		};
	}

	inline Offset3D makeOffset3D( castor::Coords2i const & pos )
	{
		return Offset3D
		{
			pos[0],
			pos[1],
			0u,
		};
	}

	inline Offset3D makeOffset3D( castor::Point2i const & pos )
	{
		return Offset3D
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
			, convert( makeExtent2D( size ) )
			, zMin
			, zMax );
	}

	inline VkViewport makeViewport( castor::Point2ui const & size
		, float zMin = 0.0f
		, float zMax = 1.0f )
	{
		return ashes::makeViewport( {}
			, convert( makeExtent2D( size ) )
			, zMin
			, zMax );
	}

	inline VkViewport makeViewport( castor::Coords2i const & pos
		, castor::Coords2ui const & size
		, float zMin = 0.0f
		, float zMax = 1.0f )
	{
		return ashes::makeViewport( convert( makeOffset2D( pos ) )
			, convert( makeExtent2D( size ) )
			, zMin
			, zMax );
	}

	inline VkViewport makeViewport( castor::Point2i const & pos
		, castor::Coords2ui const & size
		, float zMin = 0.0f
		, float zMax = 1.0f )
	{
		return ashes::makeViewport( convert( makeOffset2D( pos ) )
			, convert( makeExtent2D( size ) )
			, zMin
			, zMax );
	}

	inline VkViewport makeViewport( castor::Coords2i const & pos
		, castor::Point2ui const & size
		, float zMin = 0.0f
		, float zMax = 1.0f )
	{
		return ashes::makeViewport( convert( makeOffset2D( pos ) )
			, convert( makeExtent2D( size ) )
			, zMin
			, zMax );
	}

	inline VkViewport makeViewport( castor::Point2i const & pos
		, castor::Point2ui const & size
		, float zMin = 0.0f
		, float zMax = 1.0f )
	{
		return ashes::makeViewport( convert( makeOffset2D( pos ) )
			, convert( makeExtent2D( size ) )
			, zMin
			, zMax );
	}

	inline VkRect2D makeScissor( castor::Coords2ui const & size )
	{
		return ashes::makeScissor( {}
		, convert( makeExtent2D( size ) ) );
	}

	inline VkRect2D makeScissor( castor::Point2ui const & size )
	{
		return ashes::makeScissor( {}
		, convert( makeExtent2D( size ) ) );
	}

	inline VkRect2D makeScissor( castor::Coords2i const & pos
		, castor::Coords2ui const & size )
	{
		return ashes::makeScissor( convert( makeOffset2D( pos ) )
			, convert( makeExtent2D( size ) ) );
	}

	inline VkRect2D makeScissor( castor::Point2i const & pos
		, castor::Coords2ui const & size )
	{
		return ashes::makeScissor( convert( makeOffset2D( pos ) )
			, convert( makeExtent2D( size ) ) );
	}

	inline VkRect2D makeScissor( castor::Coords2i const & pos
		, castor::Point2ui const & size )
	{
		return ashes::makeScissor( convert( makeOffset2D( pos ) )
			, convert( makeExtent2D( size ) ) );
	}

	inline VkRect2D makeScissor( castor::Point2i const & pos
		, castor::Point2ui const & size )
	{
		return ashes::makeScissor( convert( makeOffset2D( pos ) )
			, convert( makeExtent2D( size ) ) );
	}

	template< typename EnumT >
	castor::UInt32StrMap getEnumMapT( EnumT min, EnumT max )
	{
		using ashes::getName;
		castor::UInt32StrMap result;

		for ( uint32_t i = uint32_t( min ); i <= uint32_t( max ); ++i )
		{
			result[castor::makeString( getName( EnumT( i ) ) )] = i;
		}

		return result;
	}

	template< typename EnumT >
	castor::UInt32StrMap getEnumMapT()
	{
		return getEnumMapT( EnumT::eMin, EnumT::eMax );
	}

	static ClearDepthStencilValue const defaultClearDepthStencil{ 0.0f, 0u };
	static ClearColorValue const opaqueBlackClearColor{ 0.0f, 0.0f, 0.0f, 1.0f };
	static ClearColorValue const transparentBlackClearColor{ 0.0f, 0.0f, 0.0f, 0.0f };
	static ClearColorValue const opaqueWhiteClearColor{ 1.0f, 1.0f, 1.0f, 1.0f };
	static ClearColorValue const transparentWhiteClearColor{ 1.0f, 1.0f, 1.0f, 0.0f };

	inline bool operator>( ImageSubresourceRange const & lhs
		, ImageSubresourceRange const & rhs )
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

	inline bool operator<( ImageSubresourceRange const & lhs
		, ImageSubresourceRange const & rhs )
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

	inline bool operator<=( ImageSubresourceRange const & lhs
		, ImageSubresourceRange const & rhs )
	{
		return !( lhs > rhs );
	}

	inline bool operator>=( ImageSubresourceRange const & lhs
		, ImageSubresourceRange const & rhs )
	{
		return !( lhs < rhs );
	}

	inline bool operator!=( ImageSubresourceRange const & lhs
		, ImageSubresourceRange const & rhs )
	{
		return lhs < rhs || lhs > rhs;
	}
}

CU_DeclareExportedOwnedBy( C3D_API, castor3d::Engine, Engine )

#endif
