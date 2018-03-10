/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#include "Core/VkPhysicalDevice.hpp"

#include "Core/VkRenderer.hpp"
#include "Core/VkDevice.hpp"

namespace vk_renderer
{
	namespace
	{
		std::string getName( VkPhysicalDeviceType type )
		{
			switch ( type )
			{
#define STR(r) case VK_PHYSICAL_DEVICE_TYPE_ ##r: return #r
				STR( OTHER );
				STR( INTEGRATED_GPU );
				STR( DISCRETE_GPU );
				STR( VIRTUAL_GPU );
#undef STR
			default: return "UNKNOWN_DEVICE_TYPE";
			}
		}
	}

	PhysicalDevice::PhysicalDevice( Renderer & renderer
		, VkPhysicalDevice gpu )
		: renderer::PhysicalDevice{ renderer }
		, m_renderer{ renderer }
		, m_gpu{ gpu }
	{
		initialise();
	}

	void PhysicalDevice::initialise()
	{
		// On récupère les extensions supportées par le GPU.
		uint32_t extensionCount{ 0 };
		auto res = m_renderer.vkEnumerateDeviceExtensionProperties( m_gpu
			, nullptr
			, &extensionCount
			, nullptr );

		if ( !checkError( res ) )
		{
			throw std::runtime_error{ "GPU's extensions enumeration failed: " + getLastError() };
		}

		std::vector< VkExtensionProperties > extensions( extensionCount );
		res = m_renderer.vkEnumerateDeviceExtensionProperties( m_gpu
			, nullptr
			, &extensionCount
			, extensions.data() );

		if ( !checkError( res ) )
		{
			throw std::runtime_error{ "GPU's extensions enumeration failed: " + getLastError() };
		}

		m_renderer.completeLayerNames( m_deviceLayerNames );
		m_deviceExtensionNames.push_back( VK_KHR_SWAPCHAIN_EXTENSION_NAME );
		checkExtensionsAvailability( extensions, m_deviceExtensionNames );

		// Puis les capacités du GPU.
		VkPhysicalDeviceMemoryProperties memoryProperties;
		VkPhysicalDeviceProperties properties;
		VkPhysicalDeviceFeatures features;
		m_renderer.vkGetPhysicalDeviceMemoryProperties( m_gpu, &memoryProperties );
		m_renderer.vkGetPhysicalDeviceProperties( m_gpu, &properties );
		m_renderer.vkGetPhysicalDeviceFeatures( m_gpu, &features );

		for ( uint32_t i = 0u; i < memoryProperties.memoryHeapCount; ++i )
		{
			m_memoryProperties.memoryHeaps.push_back(
			{
				memoryProperties.memoryHeaps[i].size,
				memoryProperties.memoryHeaps[i].flags,
			} );
		}

		for ( uint32_t i = 0u; i < memoryProperties.memoryTypeCount; ++i )
		{
			m_memoryProperties.memoryTypes.push_back(
			{
				memoryProperties.memoryTypes[i].propertyFlags,
				memoryProperties.memoryTypes[i].heapIndex,
			} );
		}

		m_features.robustBufferAccess = features.robustBufferAccess;
		m_features.fullDrawIndexUint32 = features.fullDrawIndexUint32;
		m_features.imageCubeArray = features.imageCubeArray;
		m_features.independentBlend = features.independentBlend;
		m_features.geometryShader = features.geometryShader;
		m_features.tessellationShader = features.tessellationShader;
		m_features.sampleRateShading = features.sampleRateShading;
		m_features.dualSrcBlend = features.dualSrcBlend;
		m_features.logicOp = features.logicOp;
		m_features.multiDrawIndirect = features.multiDrawIndirect;
		m_features.drawIndirectFirstInstance = features.drawIndirectFirstInstance;
		m_features.depthClamp = features.depthClamp;
		m_features.depthBiasClamp = features.depthBiasClamp;
		m_features.fillModeNonSolid = features.fillModeNonSolid;
		m_features.depthBounds = features.depthBounds;
		m_features.wideLines = features.wideLines;
		m_features.largePoints = features.largePoints;
		m_features.alphaToOne = features.alphaToOne;
		m_features.multiViewport = features.multiViewport;
		m_features.samplerAnisotropy = features.samplerAnisotropy;
		m_features.textureCompressionETC2 = features.textureCompressionETC2;
		m_features.textureCompressionASTC_LDR = features.textureCompressionASTC_LDR;
		m_features.textureCompressionBC = features.textureCompressionBC;
		m_features.occlusionQueryPrecise = features.occlusionQueryPrecise;
		m_features.pipelineStatisticsQuery = features.pipelineStatisticsQuery;
		m_features.vertexPipelineStoresAndAtomics = features.vertexPipelineStoresAndAtomics;
		m_features.fragmentStoresAndAtomics = features.fragmentStoresAndAtomics;
		m_features.shaderTessellationAndGeometryPointSize = features.shaderTessellationAndGeometryPointSize;
		m_features.shaderImageGatherExtended = features.shaderImageGatherExtended;
		m_features.shaderStorageImageExtendedFormats = features.shaderStorageImageExtendedFormats;
		m_features.shaderStorageImageMultisample = features.shaderStorageImageMultisample;
		m_features.shaderStorageImageReadWithoutFormat = features.shaderStorageImageReadWithoutFormat;
		m_features.shaderStorageImageWriteWithoutFormat = features.shaderStorageImageWriteWithoutFormat;
		m_features.shaderUniformBufferArrayDynamicIndexing = features.shaderUniformBufferArrayDynamicIndexing;
		m_features.shaderSampledImageArrayDynamicIndexing = features.shaderSampledImageArrayDynamicIndexing;
		m_features.shaderStorageBufferArrayDynamicIndexing = features.shaderStorageBufferArrayDynamicIndexing;
		m_features.shaderStorageImageArrayDynamicIndexing = features.shaderStorageImageArrayDynamicIndexing;
		m_features.shaderClipDistance = features.shaderClipDistance;
		m_features.shaderCullDistance = features.shaderCullDistance;
		m_features.shaderFloat64 = features.shaderFloat64;
		m_features.shaderInt64 = features.shaderInt64;
		m_features.shaderInt16 = features.shaderInt16;
		m_features.shaderResourceResidency = features.shaderResourceResidency;
		m_features.shaderResourceMinLod = features.shaderResourceMinLod;
		m_features.sparseBinding = features.sparseBinding;
		m_features.sparseResidencyBuffer = features.sparseResidencyBuffer;
		m_features.sparseResidencyImage2D = features.sparseResidencyImage2D;
		m_features.sparseResidencyImage3D = features.sparseResidencyImage3D;
		m_features.sparseResidency2Samples = features.sparseResidency2Samples;
		m_features.sparseResidency4Samples = features.sparseResidency4Samples;
		m_features.sparseResidency8Samples = features.sparseResidency8Samples;
		m_features.sparseResidency16Samples = features.sparseResidency16Samples;
		m_features.sparseResidencyAliased = features.sparseResidencyAliased;
		m_features.variableMultisampleRate = features.variableMultisampleRate;
		m_features.inheritedQueries = features.inheritedQueries;

		m_properties.apiVersion = properties.apiVersion;
		m_properties.deviceID = properties.deviceID;
		m_properties.deviceName = properties.deviceName;
		std::memcpy( m_properties.pipelineCacheUUID, properties.pipelineCacheUUID, sizeof( properties.pipelineCacheUUID ) );
		m_properties.vendorID = properties.vendorID;
		m_properties.deviceType = convertPhysicalDeviceType( properties.deviceType );
		m_properties.driverVersion = properties.driverVersion;

		m_properties.limits.maxImageDimension1D = properties.limits.maxImageDimension1D;
		m_properties.limits.maxImageDimension2D = properties.limits.maxImageDimension2D;
		m_properties.limits.maxImageDimension3D = properties.limits.maxImageDimension3D;
		m_properties.limits.maxImageDimensionCube = properties.limits.maxImageDimensionCube;
		m_properties.limits.maxImageArrayLayers = properties.limits.maxImageArrayLayers;
		m_properties.limits.maxTexelBufferElements = properties.limits.maxTexelBufferElements;
		m_properties.limits.maxUniformBufferRange = properties.limits.maxUniformBufferRange;
		m_properties.limits.maxStorageBufferRange = properties.limits.maxStorageBufferRange;
		m_properties.limits.maxPushConstantsSize = properties.limits.maxPushConstantsSize;
		m_properties.limits.maxMemoryAllocationCount = properties.limits.maxMemoryAllocationCount;
		m_properties.limits.maxSamplerAllocationCount = properties.limits.maxSamplerAllocationCount;
		m_properties.limits.bufferImageGranularity = properties.limits.bufferImageGranularity;
		m_properties.limits.sparseAddressSpaceSize = properties.limits.sparseAddressSpaceSize;
		m_properties.limits.maxBoundDescriptorSets = properties.limits.maxBoundDescriptorSets;
		m_properties.limits.maxPerStageDescriptorSamplers = properties.limits.maxPerStageDescriptorSamplers;
		m_properties.limits.maxPerStageDescriptorUniformBuffers = properties.limits.maxPerStageDescriptorUniformBuffers;
		m_properties.limits.maxPerStageDescriptorStorageBuffers = properties.limits.maxPerStageDescriptorStorageBuffers;
		m_properties.limits.maxPerStageDescriptorSampledImages = properties.limits.maxPerStageDescriptorSampledImages;
		m_properties.limits.maxPerStageDescriptorStorageImages = properties.limits.maxPerStageDescriptorStorageImages;
		m_properties.limits.maxPerStageDescriptorInputAttachments = properties.limits.maxPerStageDescriptorInputAttachments;
		m_properties.limits.maxPerStageResources = properties.limits.maxPerStageResources;
		m_properties.limits.maxDescriptorSetSamplers = properties.limits.maxDescriptorSetSamplers;
		m_properties.limits.maxDescriptorSetUniformBuffers = properties.limits.maxDescriptorSetUniformBuffers;
		m_properties.limits.maxDescriptorSetUniformBuffersDynamic = properties.limits.maxDescriptorSetUniformBuffersDynamic;
		m_properties.limits.maxDescriptorSetStorageBuffers = properties.limits.maxDescriptorSetStorageBuffers;
		m_properties.limits.maxDescriptorSetStorageBuffersDynamic = properties.limits.maxDescriptorSetStorageBuffersDynamic;
		m_properties.limits.maxDescriptorSetSampledImages = properties.limits.maxDescriptorSetSampledImages;
		m_properties.limits.maxDescriptorSetStorageImages = properties.limits.maxDescriptorSetStorageImages;
		m_properties.limits.maxDescriptorSetInputAttachments = properties.limits.maxDescriptorSetInputAttachments;
		m_properties.limits.maxVertexInputAttributes = properties.limits.maxVertexInputAttributes;
		m_properties.limits.maxVertexInputBindings = properties.limits.maxVertexInputBindings;
		m_properties.limits.maxVertexInputAttributeOffset = properties.limits.maxVertexInputAttributeOffset;
		m_properties.limits.maxVertexInputBindingStride = properties.limits.maxVertexInputBindingStride;
		m_properties.limits.maxVertexOutputComponents = properties.limits.maxVertexOutputComponents;
		m_properties.limits.maxTessellationGenerationLevel = properties.limits.maxTessellationGenerationLevel;
		m_properties.limits.maxTessellationPatchSize = properties.limits.maxTessellationPatchSize;
		m_properties.limits.maxTessellationControlPerVertexInputComponents = properties.limits.maxTessellationControlPerVertexInputComponents;
		m_properties.limits.maxTessellationControlPerVertexOutputComponents = properties.limits.maxTessellationControlPerVertexOutputComponents;
		m_properties.limits.maxTessellationControlPerPatchOutputComponents = properties.limits.maxTessellationControlPerPatchOutputComponents;
		m_properties.limits.maxTessellationControlTotalOutputComponents = properties.limits.maxTessellationControlTotalOutputComponents;
		m_properties.limits.maxTessellationEvaluationInputComponents = properties.limits.maxTessellationEvaluationInputComponents;
		m_properties.limits.maxTessellationEvaluationOutputComponents = properties.limits.maxTessellationEvaluationOutputComponents;
		m_properties.limits.maxGeometryShaderInvocations = properties.limits.maxGeometryShaderInvocations;
		m_properties.limits.maxGeometryInputComponents = properties.limits.maxGeometryInputComponents;
		m_properties.limits.maxGeometryOutputComponents = properties.limits.maxGeometryOutputComponents;
		m_properties.limits.maxGeometryOutputVertices = properties.limits.maxGeometryOutputVertices;
		m_properties.limits.maxGeometryTotalOutputComponents = properties.limits.maxGeometryTotalOutputComponents;
		m_properties.limits.maxFragmentInputComponents = properties.limits.maxFragmentInputComponents;
		m_properties.limits.maxFragmentOutputAttachments = properties.limits.maxFragmentOutputAttachments;
		m_properties.limits.maxFragmentDualSrcAttachments = properties.limits.maxFragmentDualSrcAttachments;
		m_properties.limits.maxFragmentCombinedOutputResources = properties.limits.maxFragmentCombinedOutputResources;
		m_properties.limits.maxComputeSharedMemorySize = properties.limits.maxComputeSharedMemorySize;
		m_properties.limits.maxComputeWorkGroupCount[0] = properties.limits.maxComputeWorkGroupCount[0];
		m_properties.limits.maxComputeWorkGroupCount[1] = properties.limits.maxComputeWorkGroupCount[1];
		m_properties.limits.maxComputeWorkGroupCount[2] = properties.limits.maxComputeWorkGroupCount[2];
		m_properties.limits.maxComputeWorkGroupInvocations = properties.limits.maxComputeWorkGroupInvocations;
		m_properties.limits.maxComputeWorkGroupSize[0] = properties.limits.maxComputeWorkGroupSize[0];
		m_properties.limits.maxComputeWorkGroupSize[1] = properties.limits.maxComputeWorkGroupSize[1];
		m_properties.limits.maxComputeWorkGroupSize[2] = properties.limits.maxComputeWorkGroupSize[2];
		m_properties.limits.subPixelPrecisionBits = properties.limits.subPixelPrecisionBits;
		m_properties.limits.subTexelPrecisionBits = properties.limits.subTexelPrecisionBits;
		m_properties.limits.mipmapPrecisionBits = properties.limits.mipmapPrecisionBits;
		m_properties.limits.maxDrawIndexedIndexValue = properties.limits.maxDrawIndexedIndexValue;
		m_properties.limits.maxDrawIndirectCount = properties.limits.maxDrawIndirectCount;
		m_properties.limits.maxSamplerLodBias = properties.limits.maxSamplerLodBias;
		m_properties.limits.maxSamplerAnisotropy = properties.limits.maxSamplerAnisotropy;
		m_properties.limits.maxViewports = properties.limits.maxViewports;
		m_properties.limits.maxViewportDimensions[0] = properties.limits.maxViewportDimensions[0];
		m_properties.limits.maxViewportDimensions[1] = properties.limits.maxViewportDimensions[1];
		m_properties.limits.viewportBoundsRange[0] = properties.limits.viewportBoundsRange[0];
		m_properties.limits.viewportBoundsRange[1] = properties.limits.viewportBoundsRange[1];
		m_properties.limits.viewportSubPixelBits = properties.limits.viewportSubPixelBits;
		m_properties.limits.minMemoryMapAlignment = properties.limits.minMemoryMapAlignment;
		m_properties.limits.minTexelBufferOffsetAlignment = properties.limits.minTexelBufferOffsetAlignment;
		m_properties.limits.minUniformBufferOffsetAlignment = properties.limits.minUniformBufferOffsetAlignment;
		m_properties.limits.minStorageBufferOffsetAlignment = properties.limits.minStorageBufferOffsetAlignment;
		m_properties.limits.minTexelOffset = properties.limits.minTexelOffset;
		m_properties.limits.maxTexelOffset = properties.limits.maxTexelOffset;
		m_properties.limits.minTexelGatherOffset = properties.limits.minTexelGatherOffset;
		m_properties.limits.maxTexelGatherOffset = properties.limits.maxTexelGatherOffset;
		m_properties.limits.minInterpolationOffset = properties.limits.minInterpolationOffset;
		m_properties.limits.maxInterpolationOffset = properties.limits.maxInterpolationOffset;
		m_properties.limits.subPixelInterpolationOffsetBits = properties.limits.subPixelInterpolationOffsetBits;
		m_properties.limits.maxFramebufferWidth = properties.limits.maxFramebufferWidth;
		m_properties.limits.maxFramebufferHeight = properties.limits.maxFramebufferHeight;
		m_properties.limits.maxFramebufferLayers = properties.limits.maxFramebufferLayers;
		m_properties.limits.framebufferColorSampleCounts = properties.limits.framebufferColorSampleCounts;
		m_properties.limits.framebufferDepthSampleCounts = properties.limits.framebufferDepthSampleCounts;
		m_properties.limits.framebufferStencilSampleCounts = properties.limits.framebufferStencilSampleCounts;
		m_properties.limits.framebufferNoAttachmentsSampleCounts = properties.limits.framebufferNoAttachmentsSampleCounts;
		m_properties.limits.maxColorAttachments = properties.limits.maxColorAttachments;
		m_properties.limits.sampledImageColorSampleCounts = properties.limits.sampledImageColorSampleCounts;
		m_properties.limits.sampledImageIntegerSampleCounts = properties.limits.sampledImageIntegerSampleCounts;
		m_properties.limits.sampledImageDepthSampleCounts = properties.limits.sampledImageDepthSampleCounts;
		m_properties.limits.sampledImageStencilSampleCounts = properties.limits.sampledImageStencilSampleCounts;
		m_properties.limits.storageImageSampleCounts = properties.limits.storageImageSampleCounts;
		m_properties.limits.maxSampleMaskWords = properties.limits.maxSampleMaskWords;
		m_properties.limits.timestampComputeAndGraphics = properties.limits.timestampComputeAndGraphics;
		m_properties.limits.timestampPeriod = properties.limits.timestampPeriod;
		m_properties.limits.maxClipDistances = properties.limits.maxClipDistances;
		m_properties.limits.maxCullDistances = properties.limits.maxCullDistances;
		m_properties.limits.maxCombinedClipAndCullDistances = properties.limits.maxCombinedClipAndCullDistances;
		m_properties.limits.discreteQueuePriorities = properties.limits.discreteQueuePriorities;
		m_properties.limits.pointSizeRange[0] = properties.limits.pointSizeRange[0];
		m_properties.limits.pointSizeRange[1] = properties.limits.pointSizeRange[1];
		m_properties.limits.lineWidthRange[0] = properties.limits.lineWidthRange[0];
		m_properties.limits.lineWidthRange[1] = properties.limits.lineWidthRange[1];
		m_properties.limits.pointSizeGranularity = properties.limits.pointSizeGranularity;
		m_properties.limits.lineWidthGranularity = properties.limits.lineWidthGranularity;
		m_properties.limits.strictLines = properties.limits.strictLines;
		m_properties.limits.standardSampleLocations = properties.limits.standardSampleLocations;
		m_properties.limits.optimalBufferCopyOffsetAlignment = properties.limits.optimalBufferCopyOffsetAlignment;
		m_properties.limits.optimalBufferCopyRowPitchAlignment = properties.limits.optimalBufferCopyRowPitchAlignment;
		m_properties.limits.nonCoherentAtomSize = properties.limits.nonCoherentAtomSize;

		m_properties.sparseProperties.residencyAlignedMipSize = properties.sparseProperties.residencyAlignedMipSize;
		m_properties.sparseProperties.residencyNonResidentStrict = properties.sparseProperties.residencyNonResidentStrict;
		m_properties.sparseProperties.residencyStandard2DBlockShape = properties.sparseProperties.residencyStandard2DBlockShape;
		m_properties.sparseProperties.residencyStandard2DMultisampleBlockShape = properties.sparseProperties.residencyStandard2DMultisampleBlockShape;
		m_properties.sparseProperties.residencyStandard3DBlockShape = properties.sparseProperties.residencyStandard3DBlockShape;

		// Et enfin les propriétés des familles de files du GPU.
		uint32_t queueCount{ 0 };
		m_renderer.vkGetPhysicalDeviceQueueFamilyProperties( m_gpu, &queueCount, nullptr );
		assert( queueCount >= 1 );

		m_queueProperties.reserve( queueCount );
		std::vector< VkQueueFamilyProperties > queueProperties( queueCount );
		m_renderer.vkGetPhysicalDeviceQueueFamilyProperties( m_gpu, &queueCount, queueProperties.data() );
		assert( queueCount >= 1 );

		for ( auto & props : queueProperties )
		{
			m_queueProperties.push_back(
			{
				convertQueueFlags( props.queueFlags ),
				props.queueCount,
				props.timestampValidBits,
				{
					props.minImageTransferGranularity.width,
					props.minImageTransferGranularity.height,
					props.minImageTransferGranularity.depth,
				}
			} );
		}
	}
}
