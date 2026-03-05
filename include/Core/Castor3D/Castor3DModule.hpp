/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Castor3DModule_H___
#define ___C3D_Castor3DModule_H___

#ifdef RGB
#	undef RGB
#endif

#include <CastorUtils/Multithreading/MultithreadingModule.hpp>
#include <CastorUtils/Design/FlagCombination.hpp>
#include <CastorUtils/Design/OwnedBy.hpp>
#include <CastorUtils/Exception/Assertion.hpp>
#include <CastorUtils/Graphics/GraphicsModule.hpp>
#include <CastorUtils/Graphics/Size.hpp>
#include <CastorUtils/Math/Point.hpp>
#include <CastorUtils/Math/PointView.hpp>

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
#		if defined( __MINGW32__ )
#			define C3D_INL_API
#		else
#			define C3D_INL_API __declspec( dllexport )
#		endif
#	else
#		define C3D_API __declspec( dllimport )
#		if defined( __MINGW32__ )
#			define C3D_INL_API
#		else
#			define C3D_INL_API __declspec( dllimport )
#		endif
#	endif
#endif

#define C3D_Stringify( x ) C3DX_Stringify(x)
#define C3DX_Stringify( x ) #x
#define C3D_Join2Strings( l, r ) l cuT( "." ) r
#define C3D_Join3Strings( l, m, r ) C3D_Join2Strings(l, C3D_Join2Strings(m, r))
#define C3D_Join4Strings( l, ml, mr, r ) C3D_Join3Strings(l, ml, C3D_Join2Strings(mr, r))

namespace c3d
{
	static uint32_t constexpr InvalidIndex = ~0u;
	static crg::ColorComponentFlags const defaultColorWriteMask{ crg::ColorComponentFlags::eR | crg::ColorComponentFlags::eG | crg::ColorComponentFlags::eB | crg::ColorComponentFlags::eA };

	using crg::AccessFlags;
	using crg::AttachmentLoadOp;
	using crg::AttachmentStoreOp;
	using crg::DeviceSize;
	using crg::BlendFactor;
	using crg::BlendOp;
	using crg::BufferCreateFlags;
	using crg::BufferUsageFlags;
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
	using crg::MemoryPropertyFlags;
	using crg::MipmapMode;
	using crg::PipelineStageFlags;
	using crg::SampleCount;
	using crg::WrapMode;

	using crg::AccessState;
	using crg::BufferCreateInfo;
	using crg::BufferSubresourceRange;
	using crg::BufferViewCreateInfo;
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
	using crg::Rect3D;
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
	CU_DeclareSmartPtr( c3d, Engine, C3D_API );
	CU_DeclareSmartPtr( c3d, ImporterFile, C3D_API );
	CU_DeclareSmartPtr( c3d, FramePassTimer, C3D_API );
	CU_DeclareSmartPtr( c3d, ImporterFileFactory, C3D_API );

	CU_DeclareVector( float, Float );
	CU_DeclareMap( uint32_t, int, IntUInt );
	CU_DeclareMap( String, int, IntStr );
	CU_DeclareMap( int, String, StrInt );
	/** @endcond */

	C3D_API LoggerInstance & getLogger( Engine const & engine );

	C3D_API Matrix4x4f convert( Array< float, 16 > const & value );
	C3D_API VkClearColorValue convert( RgbaColour const & value );
	C3D_API RgbaColour convert( VkClearColorValue const & value );
	C3D_API Array< float, 4u > makeFloatArray( RgbaColour const & value );

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

	template< typename EnumT >
	inline VkDescriptorSetLayoutBinding makeDescriptorSetLayoutBindingT( EnumT binding
		, VkDescriptorType descriptorType
		, VkShaderStageFlags stageFlags
		, uint32_t descriptorCount = 1u
		, VkSampler const * pImmutableSamplers = nullptr )
	{
		return makeDescriptorSetLayoutBinding( uint32_t( binding ), descriptorType, stageFlags, descriptorCount, pImmutableSamplers );
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

	template< typename EnumT >
	inline void addDescriptorSetLayoutBindingT( ashes::VkDescriptorSetLayoutBindingArray & bindings
		, EnumT binding
		, VkDescriptorType descriptorType
		, VkShaderStageFlags stageFlags
		, uint32_t descriptorCount = 1u
		, VkSampler const * pImmutableSamplers = nullptr )
	{
		bindings.push_back( makeDescriptorSetLayoutBindingT( binding
			, descriptorType
			, stageFlags
			, descriptorCount
			, pImmutableSamplers ) );
	}

	inline Size makeSize( VkExtent2D const & size )
	{
		return Size{ size.width, size.height };
	}

	inline Size makeSize( Extent2D const & size )
	{
		return Size{ size.width, size.height };
	}

	inline Size makeSize( VkExtent3D const & size )
	{
		return Size{ size.width, size.height };
	}

	inline Size makeSize( Extent3D const & size )
	{
		return Size{ size.width, size.height };
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

	inline Extent2D makeExtent2D( PointView2ui const & size )
	{
		return Extent2D
		{
			size[0],
			size[1],
		};
	}

	inline VkExtent2D makeVkExtent2D( PointView2ui const & size )
	{
		return VkExtent2D
		{
			size[0],
			size[1],
		};
	}

	inline Extent2D makeExtent2D( Point2ui const & size )
	{
		return Extent2D
		{
			size[0],
			size[1],
		};
	}

	inline Extent3D makeExtent3D( PointView2ui const & size )
	{
		return Extent3D
		{
			size[0],
			size[1],
			1u,
		};
	}

	inline VkExtent3D makeVkExtent3D( PointView2ui const & size )
	{
		return VkExtent3D
		{
			size[0],
			size[1],
			1u,
		};
	}

	inline Extent3D makeExtent3D( Point2ui const & size )
	{
		return Extent3D
		{
			size[0],
			size[1],
			1u,
		};
	}

	inline Offset2D makeOffset2D( PointView2i const & pos )
	{
		return Offset2D
		{
			pos[0],
			pos[1],
		};
	}

	inline Offset2D makeOffset2D( Point2i const & pos )
	{
		return Offset2D
		{
			pos[0],
			pos[1],
		};
	}

	inline Offset3D makeOffset3D( PointView2i const & pos )
	{
		return Offset3D
		{
			pos[0],
			pos[1],
			0u,
		};
	}

	inline Offset3D makeOffset3D( Point2i const & pos )
	{
		return Offset3D
		{
			pos[0],
			pos[1],
			0u,
		};
	}

	inline VkViewport makeViewport( PointView2ui const & size
		, float zMin = 0.0f
		, float zMax = 1.0f )
	{
		return ashes::makeViewport( {}
			, convert( makeExtent2D( size ) )
			, zMin
			, zMax );
	}

	inline VkViewport makeViewport( Point2ui const & size
		, float zMin = 0.0f
		, float zMax = 1.0f )
	{
		return ashes::makeViewport( {}
			, convert( makeExtent2D( size ) )
			, zMin
			, zMax );
	}

	inline VkViewport makeViewport( PointView2i const & pos
		, PointView2ui const & size
		, float zMin = 0.0f
		, float zMax = 1.0f )
	{
		return ashes::makeViewport( convert( makeOffset2D( pos ) )
			, convert( makeExtent2D( size ) )
			, zMin
			, zMax );
	}

	inline VkViewport makeViewport( Point2i const & pos
		, PointView2ui const & size
		, float zMin = 0.0f
		, float zMax = 1.0f )
	{
		return ashes::makeViewport( convert( makeOffset2D( pos ) )
			, convert( makeExtent2D( size ) )
			, zMin
			, zMax );
	}

	inline VkViewport makeViewport( PointView2i const & pos
		, Point2ui const & size
		, float zMin = 0.0f
		, float zMax = 1.0f )
	{
		return ashes::makeViewport( convert( makeOffset2D( pos ) )
			, convert( makeExtent2D( size ) )
			, zMin
			, zMax );
	}

	inline VkViewport makeViewport( Point2i const & pos
		, Point2ui const & size
		, float zMin = 0.0f
		, float zMax = 1.0f )
	{
		return ashes::makeViewport( convert( makeOffset2D( pos ) )
			, convert( makeExtent2D( size ) )
			, zMin
			, zMax );
	}

	inline VkRect2D makeScissor( PointView2ui const & size )
	{
		return ashes::makeScissor( {}
		, convert( makeExtent2D( size ) ) );
	}

	inline VkRect2D makeScissor( Point2ui const & size )
	{
		return ashes::makeScissor( {}
		, convert( makeExtent2D( size ) ) );
	}

	inline VkRect2D makeScissor( PointView2i const & pos
		, PointView2ui const & size )
	{
		return ashes::makeScissor( convert( makeOffset2D( pos ) )
			, convert( makeExtent2D( size ) ) );
	}

	inline VkRect2D makeScissor( Point2i const & pos
		, PointView2ui const & size )
	{
		return ashes::makeScissor( convert( makeOffset2D( pos ) )
			, convert( makeExtent2D( size ) ) );
	}

	inline VkRect2D makeScissor( PointView2i const & pos
		, Point2ui const & size )
	{
		return ashes::makeScissor( convert( makeOffset2D( pos ) )
			, convert( makeExtent2D( size ) ) );
	}

	inline VkRect2D makeScissor( Point2i const & pos
		, Point2ui const & size )
	{
		return ashes::makeScissor( convert( makeOffset2D( pos ) )
			, convert( makeExtent2D( size ) ) );
	}

	template< typename EnumT >
	UInt32StrMap getEnumMapT( EnumT min, EnumT max )
	{
		using ashes::getName;
		UInt32StrMap result;

		for ( uint32_t i = uint32_t( min ); i <= uint32_t( max ); ++i )
		{
			result[makeString( getName( EnumT( i ) ) )] = i;
		}

		return result;
	}

	template< typename EnumT >
	UInt32StrMap getEnumMapT()
	{
		return getEnumMapT( EnumT::eMin, EnumT::eMax );
	}

	static ClearDepthStencilValue const defaultClearDepthStencil{ 0.0f, 0u };
	static ClearColorValue const opaqueBlackClearColor{ 0.0f, 0.0f, 0.0f, 1.0f };
	static ClearColorValue const transparentBlackClearColor{ 0.0f, 0.0f, 0.0f, 0.0f };
	static ClearColorValue const opaqueWhiteClearColor{ 1.0f, 1.0f, 1.0f, 1.0f };
	static ClearColorValue const transparentWhiteClearColor{ 1.0f, 1.0f, 1.0f, 0.0f };
}

CU_DeclareExportedOwnedBy( C3D_API, Engine, Engine )

#endif
