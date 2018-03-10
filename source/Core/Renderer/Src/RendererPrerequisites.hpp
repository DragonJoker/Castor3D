/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_Prerequisites_HPP___
#define ___Renderer_Prerequisites_HPP___
#pragma once

#include "RendererConfig.hpp"
#include "UtilsMapping.hpp"

#include "Enum/AccessFlag.hpp"
#include "Enum/AttachmentLoadOp.hpp"
#include "Enum/AttachmentStoreOp.hpp"
#include "Enum/BlendFactor.hpp"
#include "Enum/BlendOp.hpp"
#include "Enum/BorderColour.hpp"
#include "Enum/BufferTarget.hpp"
#include "Enum/ClipDirection.hpp"
#include "Enum/ComponentSwizzle.hpp"
#include "Enum/ConstantFormat.hpp"
#include "Enum/ColourComponentFlag.hpp"
#include "Enum/CommandBufferResetFlag.hpp"
#include "Enum/CommandBufferUsageFlag.hpp"
#include "Enum/CommandPoolCreateFlag.hpp"
#include "Enum/CompareOp.hpp"
#include "Enum/CullModeFlag.hpp"
#include "Enum/DependencyFlag.hpp"
#include "Enum/DepthStencilStateFlag.hpp"
#include "Enum/DescriptorPoolCreateFlag.hpp"
#include "Enum/DescriptorType.hpp"
#include "Enum/DynamicState.hpp"
#include "Enum/FenceCreateFlag.hpp"
#include "Enum/Filter.hpp"
#include "Enum/Format.hpp"
#include "Enum/FormatFeatureFlag.hpp"
#include "Enum/FrontFace.hpp"
#include "Enum/ImageAspectFlag.hpp"
#include "Enum/ImageCreateFlag.hpp"
#include "Enum/ImageLayout.hpp"
#include "Enum/ImageTiling.hpp"
#include "Enum/ImageUsageFlag.hpp"
#include "Enum/IndexType.hpp"
#include "Enum/LogicOp.hpp"
#include "Enum/MemoryHeapFlag.hpp"
#include "Enum/MemoryMapFlag.hpp"
#include "Enum/MemoryPropertyFlag.hpp"
#include "Enum/MipmapMode.hpp"
#include "Enum/MultisampleStateFlag.hpp"
#include "Enum/PhysicalDeviceType.hpp"
#include "Enum/PipelineBindPoint.hpp"
#include "Enum/PipelineStageFlag.hpp"
#include "Enum/PolygonMode.hpp"
#include "Enum/PrimitiveTopology.hpp"
#include "Enum/QueryControlFlag.hpp"
#include "Enum/QueryPipelineStatisticFlag.hpp"
#include "Enum/QueryResultFlag.hpp"
#include "Enum/QueryType.hpp"
#include "Enum/QueueFlag.hpp"
#include "Enum/RasterisationStateFlag.hpp"
#include "Enum/RenderPassCreateFlag.hpp"
#include "Enum/SampleCountFlag.hpp"
#include "Enum/ShaderStageFlag.hpp"
#include "Enum/SharingMode.hpp"
#include "Enum/StencilOp.hpp"
#include "Enum/SubpassContents.hpp"
#include "Enum/SubpassDescriptionFlag.hpp"
#include "Enum/TessellationStateFlag.hpp"
#include "Enum/TextureType.hpp"
#include "Enum/TextureViewType.hpp"
#include "Enum/VertexInputRate.hpp"
#include "Enum/WrapMode.hpp"

#include <cassert>
#include <cstring>
#include <ctime>
#include <functional>
#include <iomanip>
#include <map>
#include <memory>
#include <sstream>
#include <vector>

#if defined( _MSC_VER )
#	include <optional>
#else
#	include <experimental/optional>
namespace std
{
	using experimental::optional;
	using experimental::nullopt;
}
#endif

namespace renderer
{
	using ByteArray = std::vector< uint8_t >;
	using UInt16Array = std::vector< uint16_t >;
	using UInt32Array = std::vector< uint32_t >;
	using UInt64Array = std::vector< uint64_t >;
	using FloatArray = std::vector< float >;
	using StringArray = std::vector< std::string >;

	template< typename T >
	class Buffer;
	template< typename T >
	class UniformBuffer;
	template< typename T >
	class PushConstantsBuffer;
	template< typename T >
	class SpecialisationInfo;
	template< typename T >
	class VertexBuffer;

	struct AttachmentDescription;
	struct AttachmentReference;
	struct BufferCopy;
	struct BufferImageCopy;
	struct ClearAttachment;
	struct ClearRect;
	struct ClearValue;
	struct ColourBlendState;
	struct ColourBlendStateAttachment;
	struct CommandBufferInheritanceInfo;
	struct DepthStencilState;
	struct DescriptorBufferInfo;
	struct DescriptorImageInfo;
	struct DescriptorPoolSize;
	struct Extent2D;
	struct Extent3D;
	struct FormatProperties;
	struct GraphicsPipelineCreateInfo;
	struct ImageBlit;
	struct ImageCopy;
	struct ImageCreateInfo;
	struct ImageFormatProperties;
	struct ImageSubresource;
	struct ImageSubresourceRange;
	struct InputAssemblyState;
	struct MemoryHeap;
	struct MemoryRequirements;
	struct MemoryType;
	struct MultisampleState;
	struct Offset2D;
	struct Offset3D;
	struct PhysicalDeviceFeatures;
	struct PhysicalDeviceLimits;
	struct PhysicalDeviceMemoryProperties;
	struct PhysicalDeviceProperties;
	struct PhysicalDeviceSparseProperties;
	struct PushConstant;
	struct PushConstantRange;
	struct QueueFamilyProperties;
	struct RasterisationState;
	struct RenderPassCreateInfo;
	struct RenderSubpassState;
	struct ShaderStageState;
	struct SpecialisationMapEntry;
	struct StencilOpState;
	struct SubpassDependency;
	struct SubpassDescription;
	struct SubresourceLayout;
	struct TessellationState;
	struct VertexInputAttributeDescription;
	struct VertexInputBindingDescription;
	struct VertexInputState;
	struct WriteDescriptorSet;

	class Attribute;
	class BackBuffer;
	class BufferBase;
	class BufferMemoryBarrier;
	class BufferView;
	class CommandBuffer;
	class CommandPool;
	class ComputePipeline;
	class Connection;
	class DescriptorPool;
	class DescriptorSet;
	class DescriptorSetLayout;
	class DescriptorSetLayoutBinding;
	class DescriptorSetPool;
	class Device;
	class DeviceMemory;
	class Fence;
	class FrameBuffer;
	class ImageMemoryBarrier;
	class IWindowHandle;
	class PhysicalDevice;
	class Pipeline;
	class PipelineLayout;
	class PushConstantsBufferBase;
	class QueryPool;
	class Queue;
	class Renderer;
	class RenderingResources;
	class RenderPass;
	class RenderSubpass;
	class Sampler;
	class Semaphore;
	class Scissor;
	class ShaderModule;
	class ShaderProgram;
	class ShaderStorageBufferBase;
	class SpecialisationInfoBase;
	class StagingBuffer;
	class SwapChain;
	class Texture;
	class FrameBufferAttachment;
	class TextureView;
	class UniformBufferBase;
	class VertexBufferBase;
	class VertexLayout;
	class Viewport;
	class WindowHandle;

	/**
	*\~french
	*\brief
	*	Temps en nanosecondes à attendre pour qu'un tampon de commandes soit exécuté.
	*\~english
	*\brief
	*	Nanoseconds time to wait for a command buffer to be executed.
	*/
	static const uint32_t FenceTimeout = 100000000;

	/**
	*\name Typedefs généralistes.
	*/
	/**\{*/
	template< typename T >
	using BufferPtr = std::unique_ptr< Buffer< T > >;
	template< typename T >
	using PushConstantsBufferPtr = std::unique_ptr< PushConstantsBuffer< T > >;
	template< typename T >
	using UniformBufferPtr = std::unique_ptr< UniformBuffer< T > >;
	template< typename T >
	using VertexBufferPtr = std::unique_ptr< VertexBuffer< T > >;
	template< typename T >
	using PushConstantsBufferPtr = std::unique_ptr< PushConstantsBuffer< T > >;
	template< typename T >
	using SpecialisationInfoPtr = std::unique_ptr< SpecialisationInfo< T > >;

	using AttributeBasePtr = std::unique_ptr< Attribute >;
	using BackBufferPtr = std::unique_ptr< BackBuffer >;
	using BufferBasePtr = std::unique_ptr< BufferBase >;
	using BufferViewPtr = std::unique_ptr< BufferView >;
	using CommandBufferPtr = std::unique_ptr< CommandBuffer >;
	using CommandPoolPtr = std::unique_ptr< CommandPool >;
	using ComputePipelinePtr = std::unique_ptr< ComputePipeline >;
	using ConnectionPtr = std::unique_ptr< Connection >;
	using DescriptorPoolPtr = std::unique_ptr< DescriptorPool >;
	using DescriptorSetLayoutPtr = std::unique_ptr< DescriptorSetLayout >;
	using DescriptorSetLayoutBindingPtr = std::unique_ptr< DescriptorSetLayoutBinding >;
	using DescriptorSetPoolPtr = std::unique_ptr< DescriptorSetPool >;
	using DescriptorSetPtr = std::unique_ptr< DescriptorSet >;
	using DevicePtr = std::unique_ptr< Device >;
	using FencePtr = std::unique_ptr< Fence >;
	using FrameBufferPtr = std::unique_ptr< FrameBuffer >;
	using IWindowHandlePtr = std::unique_ptr< IWindowHandle >;
	using PhysicalDevicePtr = std::unique_ptr< PhysicalDevice >;
	using PipelinePtr = std::unique_ptr< Pipeline >;
	using PipelineLayoutPtr = std::unique_ptr< PipelineLayout >;
	using QueryPoolPtr = std::unique_ptr< QueryPool >;
	using QueuePtr = std::unique_ptr< Queue >;
	using RendererPtr = std::unique_ptr< Renderer >;
	using RenderingResourcesPtr = std::unique_ptr< RenderingResources >;
	using RenderPassPtr = std::unique_ptr< RenderPass >;
	using RenderSubpassPtr = std::unique_ptr< RenderSubpass >;
	using SamplerPtr = std::unique_ptr< Sampler >;
	using SemaphorePtr = std::unique_ptr< Semaphore >;
	using ShaderProgramPtr = std::unique_ptr< ShaderProgram >;
	using ShaderStorageBufferBasePtr = std::unique_ptr< ShaderStorageBufferBase >;
	using StagingBufferPtr = std::unique_ptr< StagingBuffer >;
	using SwapChainPtr = std::unique_ptr< SwapChain >;
	using TexturePtr = std::unique_ptr< Texture >;
	using TextureViewPtr = std::unique_ptr< TextureView >;
	using VertexBufferBasePtr = std::unique_ptr< VertexBufferBase >;
	using VertexLayoutPtr = std::unique_ptr< VertexLayout >;
	using UniformBufferBasePtr = std::unique_ptr< UniformBufferBase >;

	using ShaderModulePtr = std::shared_ptr< ShaderModule >;
	using SpecialisationInfoBasePtr = std::shared_ptr< SpecialisationInfoBase >;
	using DeviceMemoryPtr = std::shared_ptr< DeviceMemory >;

	using AttachmentDescriptionArray = std::vector< AttachmentDescription >;
	using AttachmentReferenceArray = std::vector< AttachmentReference >;
	using AttributeArray = std::vector< Attribute >;
	using BufferImageCopyArray = std::vector< BufferImageCopy >;
	using ClearAttachmentArray = std::vector< ClearAttachment >;
	using ClearRectArray = std::vector< ClearRect >;
	using ClearValueArray = std::vector< ClearValue >;
	using ColourBlendStateAttachmentArray = std::vector< ColourBlendStateAttachment >;
	using DescriptorPoolSizeArray = std::vector< DescriptorPoolSize >;
	using DescriptorSetLayoutBindingArray = std::vector< DescriptorSetLayoutBinding >;
	using FrameBufferAttachmentArray = std::vector< FrameBufferAttachment >;
	using ImageLayoutArray = std::vector< ImageLayout >;
	using PipelineStageFlagsArray = std::vector< PipelineStageFlags >;
	using PushConstantArray = std::vector< PushConstant >;
	using RenderSubpassArray = std::vector< RenderSubpass >;
	using ShaderStageStateArray = std::vector< ShaderStageState >;
	using SpecialisationMapEntryArray = std::vector< SpecialisationMapEntry >;
	using SubpassDescriptionArray = std::vector< SubpassDescription >;
	using SubpassDependencyArray = std::vector< SubpassDependency >;
	using VertexInputAttributeDescriptionArray = std::vector< VertexInputAttributeDescription >;
	using VertexInputBindingDescriptionArray = std::vector< VertexInputBindingDescription >;
	using WriteDescriptorSetArray = std::vector< WriteDescriptorSet >;

	using FrameBufferPtrArray = std::vector< FrameBufferPtr >;
	using BackBufferPtrArray = std::vector< BackBufferPtr >;
	using CommandBufferPtrArray = std::vector< CommandBufferPtr >;
	using RenderSubpassPtrArray = std::vector< RenderSubpassPtr >;

	using BufferCRef = std::reference_wrapper< BufferBase const >;
	using BufferViewCRef = std::reference_wrapper< BufferView const >;
	using CommandBufferCRef = std::reference_wrapper< CommandBuffer const >;
	using DescriptorSetCRef = std::reference_wrapper< DescriptorSet const >;
	using DescriptorSetLayoutCRef = std::reference_wrapper< DescriptorSetLayout const >;
	using PushConstantRangeCRef = std::reference_wrapper< PushConstantRange const >;
	using SamplerCRef = std::reference_wrapper< Sampler const >;
	using SemaphoreCRef = std::reference_wrapper< Semaphore const >;
	using SwapChainCRef = std::reference_wrapper< SwapChain const >;
	using TextureViewCRef = std::reference_wrapper< TextureView const >;
	using VertexLayoutCRef = std::reference_wrapper< VertexLayout const >;
	using VertexBufferCRef = std::reference_wrapper< VertexBufferBase const >;

	using BufferCRefArray = std::vector< BufferCRef >;
	using CommandBufferCRefArray = std::vector< CommandBufferCRef >;
	using DescriptorSetCRefArray = std::vector< DescriptorSetCRef >;
	using DescriptorSetLayoutCRefArray = std::vector< DescriptorSetLayoutCRef >;
	using PushConstantRangeCRefArray = std::vector< PushConstantRangeCRef >;
	using SemaphoreCRefArray = std::vector< SemaphoreCRef >;
	using SwapChainCRefArray = std::vector< SwapChainCRef >;
	using TextureViewCRefArray = std::vector< TextureViewCRef >;
	using VertexLayoutCRefArray = std::vector< VertexLayoutCRef >;
	using VertexBufferCRefArray = std::vector< VertexBufferCRef >;
	/**\}*/
	/**
	*\name Signaux.
	*/
	/**\{*/
	using DeviceEnabledFunction = std::function< void( Device const & ) >;
	using DeviceDisabledFunction = std::function< void( Device const & ) >;

	using DeviceEnabledSignal = Signal< DeviceEnabledFunction >;
	using DeviceDisabledSignal = Signal< DeviceDisabledFunction >;

	using DeviceEnabledConnection = SignalConnection< DeviceEnabledSignal >;
	using DeviceDisabledConnection = SignalConnection< DeviceDisabledSignal >;
	/**\}*/
}

#endif
