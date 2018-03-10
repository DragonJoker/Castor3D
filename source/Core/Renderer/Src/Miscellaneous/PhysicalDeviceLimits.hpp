/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_PhysicalDeviceLimits_HPP___
#define ___Renderer_PhysicalDeviceLimits_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

#if defined( max )
#	undef max
#endif

namespace renderer
{
	template< typename T >
	static constexpr T NonAvailable = std::numeric_limits< T >::max();
	/**
	*\~english
	*\brief
	*	Reports implementation-dependent physical device limits.
	*\~french
	*\brief
	*	Rapporte les limites dépendant de l'implémentation pour un périphérique physique.
	*/
	struct PhysicalDeviceLimits
	{
		uint32_t maxImageDimension1D;
		uint32_t maxImageDimension2D;
		uint32_t maxImageDimension3D;
		uint32_t maxImageDimensionCube;
		uint32_t maxImageArrayLayers;
		uint32_t maxTexelBufferElements;
		uint32_t maxUniformBufferRange;
		uint32_t maxStorageBufferRange;
		uint32_t maxPushConstantsSize;
		uint32_t maxMemoryAllocationCount;
		uint32_t maxSamplerAllocationCount;
		uint64_t bufferImageGranularity;
		uint64_t sparseAddressSpaceSize;
		uint32_t maxBoundDescriptorSets;
		uint32_t maxPerStageDescriptorSamplers;
		uint32_t maxPerStageDescriptorUniformBuffers;
		uint32_t maxPerStageDescriptorStorageBuffers;
		uint32_t maxPerStageDescriptorSampledImages;
		uint32_t maxPerStageDescriptorStorageImages;
		uint32_t maxPerStageDescriptorInputAttachments;
		uint32_t maxPerStageResources;
		uint32_t maxDescriptorSetSamplers;
		uint32_t maxDescriptorSetUniformBuffers;
		uint32_t maxDescriptorSetUniformBuffersDynamic;
		uint32_t maxDescriptorSetStorageBuffers;
		uint32_t maxDescriptorSetStorageBuffersDynamic;
		uint32_t maxDescriptorSetSampledImages;
		uint32_t maxDescriptorSetStorageImages;
		uint32_t maxDescriptorSetInputAttachments;
		uint32_t maxVertexInputAttributes;
		uint32_t maxVertexInputBindings;
		uint32_t maxVertexInputAttributeOffset;
		uint32_t maxVertexInputBindingStride;
		uint32_t maxVertexOutputComponents;
		uint32_t maxTessellationGenerationLevel;
		uint32_t maxTessellationPatchSize;
		uint32_t maxTessellationControlPerVertexInputComponents;
		uint32_t maxTessellationControlPerVertexOutputComponents;
		uint32_t maxTessellationControlPerPatchOutputComponents;
		uint32_t maxTessellationControlTotalOutputComponents;
		uint32_t maxTessellationEvaluationInputComponents;
		uint32_t maxTessellationEvaluationOutputComponents;
		uint32_t maxGeometryShaderInvocations;
		uint32_t maxGeometryInputComponents;
		uint32_t maxGeometryOutputComponents;
		uint32_t maxGeometryOutputVertices;
		uint32_t maxGeometryTotalOutputComponents;
		uint32_t maxFragmentInputComponents;
		uint32_t maxFragmentOutputAttachments;
		uint32_t maxFragmentDualSrcAttachments;
		uint32_t maxFragmentCombinedOutputResources;
		uint32_t maxComputeSharedMemorySize;
		uint32_t maxComputeWorkGroupCount[3];
		uint32_t maxComputeWorkGroupInvocations;
		uint32_t maxComputeWorkGroupSize[3];
		uint32_t subPixelPrecisionBits;
		uint32_t subTexelPrecisionBits;
		uint32_t mipmapPrecisionBits;
		uint32_t maxDrawIndexedIndexValue;
		uint32_t maxDrawIndirectCount;
		float maxSamplerLodBias;
		float maxSamplerAnisotropy;
		uint32_t maxViewports;
		uint32_t maxViewportDimensions[2];
		float viewportBoundsRange[2];
		uint32_t viewportSubPixelBits;
		size_t minMemoryMapAlignment;
		uint64_t minTexelBufferOffsetAlignment;
		uint64_t minUniformBufferOffsetAlignment;
		uint64_t minStorageBufferOffsetAlignment;
		int32_t minTexelOffset;
		uint32_t maxTexelOffset;
		int32_t minTexelGatherOffset;
		uint32_t maxTexelGatherOffset;
		float minInterpolationOffset;
		float maxInterpolationOffset;
		uint32_t subPixelInterpolationOffsetBits;
		uint32_t maxFramebufferWidth;
		uint32_t maxFramebufferHeight;
		uint32_t maxFramebufferLayers;
		SampleCountFlags framebufferColorSampleCounts;
		SampleCountFlags framebufferDepthSampleCounts;
		SampleCountFlags framebufferStencilSampleCounts;
		SampleCountFlags framebufferNoAttachmentsSampleCounts;
		uint32_t maxColorAttachments;
		SampleCountFlags sampledImageColorSampleCounts;
		SampleCountFlags sampledImageIntegerSampleCounts;
		SampleCountFlags sampledImageDepthSampleCounts;
		SampleCountFlags sampledImageStencilSampleCounts;
		SampleCountFlags storageImageSampleCounts;
		uint32_t maxSampleMaskWords;
		bool timestampComputeAndGraphics;
		float timestampPeriod;
		uint32_t maxClipDistances;
		uint32_t maxCullDistances;
		uint32_t maxCombinedClipAndCullDistances;
		uint32_t discreteQueuePriorities;
		float pointSizeRange[2];
		float lineWidthRange[2];
		float pointSizeGranularity;
		float lineWidthGranularity;
		bool strictLines;
		bool standardSampleLocations;
		uint64_t optimalBufferCopyOffsetAlignment;
		uint64_t optimalBufferCopyRowPitchAlignment;
		uint64_t nonCoherentAtomSize;
	};
}

#endif
