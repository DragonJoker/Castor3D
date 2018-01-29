/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_Prerequisites_HPP___
#define ___Renderer_Prerequisites_HPP___
#pragma once

#include "RendererConfig.hpp"
#include "UtilsMapping.hpp"

#include "Enum/AccessFlag.hpp"
#include "Enum/AttributeFormat.hpp"
#include "Enum/BlendFactor.hpp"
#include "Enum/BlendOp.hpp"
#include "Enum/BorderColour.hpp"
#include "Enum/BufferTarget.hpp"
#include "Enum/ClipDirection.hpp"
#include "Enum/ColourComponentFlag.hpp"
#include "Enum/CommandBufferResetFlag.hpp"
#include "Enum/CommandBufferUsageFlag.hpp"
#include "Enum/CommandPoolCreateFlag.hpp"
#include "Enum/CompareOp.hpp"
#include "Enum/CullModeFlag.hpp"
#include "Enum/DepthStencilStateFlag.hpp"
#include "Enum/DescriptorType.hpp"
#include "Enum/FenceCreateFlag.hpp"
#include "Enum/Filter.hpp"
#include "Enum/FrontFace.hpp"
#include "Enum/ImageAspectFlag.hpp"
#include "Enum/ImageLayout.hpp"
#include "Enum/ImageTiling.hpp"
#include "Enum/ImageUsageFlag.hpp"
#include "Enum/IndexType.hpp"
#include "Enum/LogicOp.hpp"
#include "Enum/MemoryMapFlag.hpp"
#include "Enum/MemoryPropertyFlag.hpp"
#include "Enum/MipmapMode.hpp"
#include "Enum/MultisampleStateFlag.hpp"
#include "Enum/PipelineBindPoint.hpp"
#include "Enum/PipelineStageFlag.hpp"
#include "Enum/PolygonMode.hpp"
#include "Enum/PrimitiveTopology.hpp"
#include "Enum/QueryControlFlag.hpp"
#include "Enum/QueryPipelineStatisticFlag.hpp"
#include "Enum/QueryResultFlag.hpp"
#include "Enum/QueryType.hpp"
#include "Enum/RasterisationStateFlag.hpp"
#include "Enum/SampleCountFlag.hpp"
#include "Enum/ShaderStageFlag.hpp"
#include "Enum/StencilOp.hpp"
#include "Enum/SubpassContents.hpp"
#include "Enum/TessellationStateFlag.hpp"
#include "Enum/TextureType.hpp"
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

namespace renderer
{
	template< typename T >
	class Attribute;
	template< typename T >
	class Buffer;
	template< typename T >
	class UniformBuffer;
	template< typename T >
	class PushConstantsBuffer;
	template< typename T >
	class VertexBuffer;
	template< typename T >
	class ShaderStorageBuffer;

	struct BufferCopy;
	struct BufferImageCopy;
	struct ClearValue;
	struct ImageCopy;
	struct ImageBlit;
	struct PushConstant;
	struct PushConstantRange;

	class AttributeBase;
	class BackBuffer;
	class BufferBase;
	class BufferMemoryBarrier;
	class BufferView;
	class ColourBlendState;
	class ColourBlendStateAttachment;
	class CommandBuffer;
	class CommandPool;
	class Connection;
	class DepthStencilState;
	class DescriptorSet;
	class DescriptorSetBinding;
	class DescriptorSetLayout;
	class DescriptorSetLayoutBinding;
	class DescriptorSetPool;
	class Device;
	class Fence;
	class FrameBuffer;
	class GeometryBuffers;
	class ImageMemoryBarrier;
	class ImageSubresourceRange;
	class IWindowHandle;
	class MultisampleState;
	class Pipeline;
	class PipelineLayout;
	class PushConstantsBufferBase;
	class QueryPool;
	class Queue;
	class RasterisationState;
	class Renderer;
	class RenderingResources;
	class RenderPass;
	class RenderPassState;
	class RenderSubpass;
	class RenderSubpassState;
	class Sampler;
	class Semaphore;
	class Scissor;
	class ShaderProgram;
	class ShaderStorageBufferBase;
	class StagingBuffer;
	class StencilOpState;
	class SwapChain;
	class TessellationState;
	class Texture;
	class TextureAttachment;
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
	using AttributePtr = std::unique_ptr< Attribute< T > >;
	template< typename T >
	using BufferPtr = std::unique_ptr< Buffer< T > >;
	template< typename T >
	using PushConstantsBufferPtr = std::unique_ptr< PushConstantsBuffer< T > >;
	template< typename T >
	using UniformBufferPtr = std::unique_ptr< UniformBuffer< T > >;
	template< typename T >
	using VertexBufferPtr = std::unique_ptr< VertexBuffer< T > >;
	template< typename T >
	using ShaderStorageBufferPtr = std::unique_ptr< ShaderStorageBuffer< T > >;

	using AttributeBasePtr = std::unique_ptr< AttributeBase >;
	using BufferBasePtr = std::unique_ptr< BufferBase >;
	using BufferViewPtr = std::unique_ptr< BufferView >;
	using CommandBufferPtr = std::unique_ptr< CommandBuffer >;
	using CommandPoolPtr = std::unique_ptr< CommandPool >;
	using ConnectionPtr = std::unique_ptr< Connection >;
	using DescriptorSetLayoutPtr = std::unique_ptr< DescriptorSetLayout >;
	using DescriptorSetLayoutBindingPtr = std::unique_ptr< DescriptorSetLayoutBinding >;
	using DescriptorSetPoolPtr = std::unique_ptr< DescriptorSetPool >;
	using DescriptorSetPtr = std::unique_ptr< DescriptorSet >;
	using DevicePtr = std::unique_ptr< Device >;
	using FencePtr = std::unique_ptr< Fence >;
	using TextureAttachmentPtr = std::unique_ptr< TextureAttachment >;
	using GeometryBuffersPtr = std::unique_ptr< GeometryBuffers >;
	using IWindowHandlePtr = std::unique_ptr< IWindowHandle >;
	using PipelineLayoutPtr = std::unique_ptr< PipelineLayout >;
	using QueryPoolPtr = std::unique_ptr< QueryPool >;
	using QueuePtr = std::unique_ptr< Queue >;
	using RendererPtr = std::unique_ptr< Renderer >;
	using RenderingResourcesPtr = std::unique_ptr< RenderingResources >;
	using RenderPassPtr = std::unique_ptr< RenderPass >;
	using RenderSubpassPtr = std::unique_ptr< RenderSubpass >;
	using SemaphorePtr = std::unique_ptr< Semaphore >;
	using ShaderProgramPtr = std::unique_ptr< ShaderProgram >;
	using ShaderStorageBufferBasePtr = std::unique_ptr< ShaderStorageBufferBase >;
	using SwapChainPtr = std::unique_ptr< SwapChain >;
	using VertexBufferBasePtr = std::unique_ptr< VertexBufferBase >;
	using VertexLayoutPtr = std::unique_ptr< VertexLayout >;
	using UniformBufferBasePtr = std::unique_ptr< UniformBufferBase >;

	using DescriptorSetLayoutBindingArray = std::vector< DescriptorSetLayoutBinding >;
	using PushConstantArray = std::vector< PushConstant >;

	using FrameBufferPtr = std::shared_ptr< FrameBuffer >;
	using PipelinePtr = std::shared_ptr< Pipeline >;
	using SamplerPtr = std::shared_ptr< Sampler >;
	using StagingBufferPtr = std::shared_ptr< StagingBuffer >;
	using TexturePtr = std::shared_ptr< Texture >;
	using TextureViewPtr = std::shared_ptr< TextureView >;

	using FrameBufferPtrArray = std::vector< FrameBufferPtr >;
	using TextureAttachmentPtrArray = std::vector< TextureAttachmentPtr >;
	using CommandBufferPtrArray = std::vector< CommandBufferPtr >;
	using RenderSubpassPtrArray = std::vector< RenderSubpassPtr >;

	using ClearValueArray = std::vector< ClearValue >;
	using ColourBlendStateAttachmentArray = std::vector< ColourBlendStateAttachment >;
	using ImageLayoutArray = std::vector< ImageLayout >;
	using PipelineStageFlagsArray = std::vector< PipelineStageFlags >;
	using RenderSubpassArray = std::vector< RenderSubpass >;

	using CommandBufferCRef = std::reference_wrapper< CommandBuffer const >;
	using DescriptorSetLayoutCRef = std::reference_wrapper< DescriptorSetLayout const >;
	using PushConstantRangeCRef = std::reference_wrapper< PushConstantRange const >;
	using SemaphoreCRef = std::reference_wrapper< Semaphore const >;
	using SwapChainCRef = std::reference_wrapper< SwapChain const >;
	using TextureViewCRef = std::reference_wrapper< TextureView const >;
	using VertexLayoutCRef = std::reference_wrapper< VertexLayout const >;
	using VertexBufferCRef = std::reference_wrapper< VertexBufferBase const >;

	using CommandBufferCRefArray = std::vector< CommandBufferCRef >;
	using DescriptorSetLayoutCRefArray = std::vector< DescriptorSetLayoutCRef >;
	using PushConstantRangeCRefArray = std::vector< PushConstantRangeCRef >;
	using SemaphoreCRefArray = std::vector< SemaphoreCRef >;
	using SwapChainCRefArray = std::vector< SwapChainCRef >;
	using TextureViewCRefArray = std::vector< TextureViewCRef >;
	using VertexLayoutCRefArray = std::vector< VertexLayoutCRef >;
	using VertexBufferCRefArray = std::vector< VertexBufferCRef >;
	/**\}*/
	/**
	*\name Typedefs d'attributs de sommets.
	*/
	/**\{*/
	using FloatAttribute = Attribute< float >;
	using Vec2Attribute = Attribute< Vec2 >;
	using Vec3Attribute = Attribute< Vec3 >;
	using Vec4Attribute = Attribute< Vec4 >;
	using FloatAttributePtr = AttributePtr< float >;
	using Vec2AttributePtr = AttributePtr< Vec2 >;
	using Vec3AttributePtr = AttributePtr< Vec3 >;
	using Vec4AttributePtr = AttributePtr< Vec4 >;
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
