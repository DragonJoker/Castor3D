/*
See LICENSE file in root folder
*/
#ifndef ___C3D_makeVkType_H___
#define ___C3D_makeVkType_H___

#include "MiscellaneousModule.hpp"

namespace castor3d
{
	template< typename AshesType >
	struct VkStructTraits;
	
	template<> struct VkStructTraits< VkApplicationInfo >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_APPLICATION_INFO; };
	template<> struct VkStructTraits< VkInstanceCreateInfo >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO; };
	template<> struct VkStructTraits< VkDeviceQueueCreateInfo >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO; };
	template<> struct VkStructTraits< VkDeviceCreateInfo >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO; };
	template<> struct VkStructTraits< VkSubmitInfo >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SUBMIT_INFO; };
	template<> struct VkStructTraits< VkMemoryAllocateInfo >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO; };
	template<> struct VkStructTraits< VkMappedMemoryRange >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE; };
	template<> struct VkStructTraits< VkBindSparseInfo >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_BIND_SPARSE_INFO; };
	template<> struct VkStructTraits< VkFenceCreateInfo >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO; };
	template<> struct VkStructTraits< VkSemaphoreCreateInfo >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO; };
	template<> struct VkStructTraits< VkEventCreateInfo >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_EVENT_CREATE_INFO; };
	template<> struct VkStructTraits< VkQueryPoolCreateInfo >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO; };
	template<> struct VkStructTraits< VkBufferCreateInfo >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO; };
	template<> struct VkStructTraits< VkBufferViewCreateInfo >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO; };
	template<> struct VkStructTraits< VkImageCreateInfo >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO; };
	template<> struct VkStructTraits< VkImageViewCreateInfo >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO; };
	template<> struct VkStructTraits< VkShaderModuleCreateInfo >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO; };
	template<> struct VkStructTraits< VkPipelineCacheCreateInfo >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO; };
	template<> struct VkStructTraits< VkPipelineShaderStageCreateInfo >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO; };
	template<> struct VkStructTraits< VkPipelineVertexInputStateCreateInfo >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO; };
	template<> struct VkStructTraits< VkPipelineInputAssemblyStateCreateInfo >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO; };
	template<> struct VkStructTraits< VkPipelineTessellationStateCreateInfo >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO; };
	template<> struct VkStructTraits< VkPipelineViewportStateCreateInfo >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO; };
	template<> struct VkStructTraits< VkPipelineRasterizationStateCreateInfo >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO; };
	template<> struct VkStructTraits< VkPipelineMultisampleStateCreateInfo >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO; };
	template<> struct VkStructTraits< VkPipelineDepthStencilStateCreateInfo >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO; };
	template<> struct VkStructTraits< VkPipelineColorBlendStateCreateInfo >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO; };
	template<> struct VkStructTraits< VkPipelineDynamicStateCreateInfo >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO; };
	template<> struct VkStructTraits< VkGraphicsPipelineCreateInfo >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO; };
	template<> struct VkStructTraits< VkComputePipelineCreateInfo >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO; };
	template<> struct VkStructTraits< VkPipelineLayoutCreateInfo >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO; };
	template<> struct VkStructTraits< VkSamplerCreateInfo >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO; };
	template<> struct VkStructTraits< VkDescriptorSetLayoutCreateInfo >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO; };
	template<> struct VkStructTraits< VkDescriptorPoolCreateInfo >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO; };
	template<> struct VkStructTraits< VkDescriptorSetAllocateInfo >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO; };
	template<> struct VkStructTraits< VkWriteDescriptorSet >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET; };
	template<> struct VkStructTraits< VkCopyDescriptorSet >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET; };
	template<> struct VkStructTraits< VkFramebufferCreateInfo >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO; };
	template<> struct VkStructTraits< VkRenderPassCreateInfo >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO; };
	template<> struct VkStructTraits< VkCommandPoolCreateInfo >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO; };
	template<> struct VkStructTraits< VkCommandBufferAllocateInfo >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO; };
	template<> struct VkStructTraits< VkCommandBufferInheritanceInfo >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO; };
	template<> struct VkStructTraits< VkCommandBufferBeginInfo >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO; };
	template<> struct VkStructTraits< VkRenderPassBeginInfo >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO; };
	template<> struct VkStructTraits< VkBufferMemoryBarrier >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER; };
	template<> struct VkStructTraits< VkImageMemoryBarrier >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER; };
	template<> struct VkStructTraits< VkMemoryBarrier >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_MEMORY_BARRIER; };
	template<> struct VkStructTraits< VkPhysicalDeviceSubgroupProperties >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_PROPERTIES; };
	template<> struct VkStructTraits< VkBindBufferMemoryInfo >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_BIND_BUFFER_MEMORY_INFO; };
	template<> struct VkStructTraits< VkBindImageMemoryInfo >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_INFO; };
	template<> struct VkStructTraits< VkPhysicalDevice16BitStorageFeatures >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES; };
	template<> struct VkStructTraits< VkMemoryDedicatedRequirements >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_MEMORY_DEDICATED_REQUIREMENTS; };
	template<> struct VkStructTraits< VkMemoryDedicatedAllocateInfo >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_MEMORY_DEDICATED_ALLOCATE_INFO; };
	template<> struct VkStructTraits< VkMemoryAllocateFlagsInfo >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO; };
	template<> struct VkStructTraits< VkDeviceGroupRenderPassBeginInfo >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEVICE_GROUP_RENDER_PASS_BEGIN_INFO; };
	template<> struct VkStructTraits< VkDeviceGroupCommandBufferBeginInfo >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEVICE_GROUP_COMMAND_BUFFER_BEGIN_INFO; };
	template<> struct VkStructTraits< VkDeviceGroupSubmitInfo >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEVICE_GROUP_SUBMIT_INFO; };
	template<> struct VkStructTraits< VkDeviceGroupBindSparseInfo >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEVICE_GROUP_BIND_SPARSE_INFO; };
	template<> struct VkStructTraits< VkBindBufferMemoryDeviceGroupInfo >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_BIND_BUFFER_MEMORY_DEVICE_GROUP_INFO; };
	template<> struct VkStructTraits< VkBindImageMemoryDeviceGroupInfo >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_DEVICE_GROUP_INFO; };
	template<> struct VkStructTraits< VkPhysicalDeviceGroupProperties >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES; };
	template<> struct VkStructTraits< VkDeviceGroupDeviceCreateInfo >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEVICE_GROUP_DEVICE_CREATE_INFO; };
#if VK_VERSION_1_1
	template<> struct VkStructTraits< VkProtectedSubmitInfo >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PROTECTED_SUBMIT_INFO; };
	template<> struct VkStructTraits< VkPhysicalDeviceProtectedMemoryFeatures >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROTECTED_MEMORY_FEATURES; };
	template<> struct VkStructTraits< VkPhysicalDeviceProtectedMemoryProperties >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROTECTED_MEMORY_PROPERTIES; };
	template<> struct VkStructTraits< VkDeviceQueueInfo2 >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEVICE_QUEUE_INFO_2; };
	template<> struct VkStructTraits< VkPhysicalDeviceShaderDrawParameterFeatures >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETER_FEATURES; };
#endif
#if VK_VERSION_1_2
	template<> struct VkStructTraits< VkPhysicalDeviceVulkan11Features >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES; };
	template<> struct VkStructTraits< VkPhysicalDeviceVulkan11Properties >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES; };
	template<> struct VkStructTraits< VkPhysicalDeviceVulkan12Features >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES; };
	template<> struct VkStructTraits< VkPhysicalDeviceVulkan12Properties >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES; };
#endif
#if VK_KHR_driver_properties
#endif
#ifdef VK_USE_PLATFORM_ANDROID_KHR
	template<> struct VkStructTraits< VkAndroidSurfaceCreateInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR; };
	template<> struct VkStructTraits< VkAndroidHardwareBufferUsageAndroid >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_ANDROID_HARDWARE_BUFFER_USAGE_ANDROID; };
	template<> struct VkStructTraits< VkAndroidHardwareBufferPropertiesAndroid >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_ANDROID_HARDWARE_BUFFER_PROPERTIES_ANDROID; };
	template<> struct VkStructTraits< VkAndroidHardwareBufferFormatPropertiesAndroid >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_ANDROID_HARDWARE_BUFFER_FORMAT_PROPERTIES_ANDROID; };
	template<> struct VkStructTraits< VkImportAndroidHardwareBufferInfoAndroid >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IMPORT_ANDROID_HARDWARE_BUFFER_INFO_ANDROID; };
	template<> struct VkStructTraits< VkMemoryGetAndroidHardwareBufferInfoAndroid >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_MEMORY_GET_ANDROID_HARDWARE_BUFFER_INFO_ANDROID; };
	template<> struct VkStructTraits< VkExternalFormatAndroid >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_EXTERNAL_FORMAT_ANDROID; };
#endif
#ifdef VK_USE_PLATFORM_FUCHSIA
	template<> struct VkStructTraits< VkImagePipeSurfaceCreateInfoFuchsia >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IMAGEPIPE_SURFACE_CREATE_INFO_FUCHSIA; };
#endif
#ifdef VK_USE_PLATFORM_IOS_MVK
	template<> struct VkStructTraits< VkIOSSurfaceCreateInfoMVK >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IOS_SURFACE_CREATE_INFO_MVK; };
#endif
#ifdef VK_USE_PLATFORM_MACOS_MVK
	template<> struct VkStructTraits< VkMacOSSurfaceCreateInfoMVK >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK; };
#endif
#ifdef VK_USE_PLATFORM_VI_NN
	template<> struct VkStructTraits< VkViSurfaceCreateInfoNN >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_VI_SURFACE_CREATE_INFO_NN; };
#endif
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
	template<> struct VkStructTraits< VkWaylandSurfaceCreateInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR; };
#endif
#ifdef VK_USE_PLATFORM_WIN32_KHR
	template<> struct VkStructTraits< VkWin32SurfaceCreateInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR; };
	template<> struct VkStructTraits< VkImportMemoryWin32HandleInfoNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IMPORT_MEMORY_WIN32_HANDLE_INFO_NV; };
	template<> struct VkStructTraits< VkExportMemoryWin32HandleInfoNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_EXPORT_MEMORY_WIN32_HANDLE_INFO_NV; };
#endif
#ifdef VK_USE_PLATFORM_XCB_KHR
	template<> struct VkStructTraits< VkXcbSurfaceCreateInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR; };
#endif
#ifdef VK_USE_PLATFORM_XLIB_KHR
	template<> struct VkStructTraits< VkXlibSurfaceCreateInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR; };
#endif
#ifdef VK_USE_PLATFORM_XLIB_XRANDR_EXT
	template<> struct VkStructTraits< VkXlibSurfaceCreateInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR; };
#endif
#if VK_KHR_external_memory_win32
	template<> struct VkStructTraits< VkImportMemoryWin32HandleInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IMPORT_MEMORY_WIN32_HANDLE_INFO_KHR; };
	template<> struct VkStructTraits< VkExportMemoryWin32HandleInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_EXPORT_MEMORY_WIN32_HANDLE_INFO_KHR; };
	template<> struct VkStructTraits< VkMemoryWin32HandlePropertiesKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_MEMORY_WIN32_HANDLE_PROPERTIES_KHR; };
	template<> struct VkStructTraits< VkMemoryGetWin32HandleInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_MEMORY_GET_WIN32_HANDLE_INFO_KHR; };
#endif
#if VK_KHR_win32_keyed_mutex
	template<> struct VkStructTraits< VkWin32KeyedMutexAcquireReleaseInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_WIN32_KEYED_MUTEX_ACQUIRE_RELEASE_INFO_KHR; };
#endif
#if VK_NV_win32_keyed_mutex
	template<> struct VkStructTraits< VkWin32KeyedMutexAcquireReleaseInfoNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_WIN32_KEYED_MUTEX_ACQUIRE_RELEASE_INFO_NV; };
#endif
#if VK_KHR_external_semaphore_win32
	template<> struct VkStructTraits< VkImportSemaphoreWin32HandleInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IMPORT_SEMAPHORE_WIN32_HANDLE_INFO_KHR; };
	template<> struct VkStructTraits< VkExportSemaphoreWin32HandleInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_EXPORT_SEMAPHORE_WIN32_HANDLE_INFO_KHR; };
	template<> struct VkStructTraits< VkD3D12FenceSubmitInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_D3D12_FENCE_SUBMIT_INFO_KHR; };
	template<> struct VkStructTraits< VkSemaphoreGetWin32HandleInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SEMAPHORE_GET_WIN32_HANDLE_INFO_KHR; };
#endif
#if VK_KHR_external_fence_win32
	template<> struct VkStructTraits< VkImportFenceWin32HandleInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IMPORT_FENCE_WIN32_HANDLE_INFO_KHR; };
	template<> struct VkStructTraits< VkExportFenceWin32HandleInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_EXPORT_FENCE_WIN32_HANDLE_INFO_KHR; };
	template<> struct VkStructTraits< VkFenceGetWin32HandleInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_FENCE_GET_WIN32_HANDLE_INFO_KHR; };
#endif
#if VK_KHR_get_memory_requirements2
	template<> struct VkStructTraits< VkBufferMemoryRequirementsInfo2KHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_BUFFER_MEMORY_REQUIREMENTS_INFO_2_KHR; };
	template<> struct VkStructTraits< VkImageMemoryRequirementsInfo2KHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IMAGE_MEMORY_REQUIREMENTS_INFO_2_KHR; };
	template<> struct VkStructTraits< VkImageSparseMemoryRequirementsInfo2KHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IMAGE_SPARSE_MEMORY_REQUIREMENTS_INFO_2_KHR; };
	template<> struct VkStructTraits< VkMemoryRequirements2KHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2_KHR; };
	template<> struct VkStructTraits< VkSparseImageMemoryRequirements2KHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SPARSE_IMAGE_MEMORY_REQUIREMENTS_2_KHR; };
#endif
#if VK_KHR_get_physical_device_properties2
	template<> struct VkStructTraits< VkPhysicalDeviceFeatures2KHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2; };
	template<> struct VkStructTraits< VkPhysicalDeviceProperties2KHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2; };
	template<> struct VkStructTraits< VkFormatProperties2KHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2; };
	template<> struct VkStructTraits< VkImageFormatProperties2KHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IMAGE_FORMAT_PROPERTIES_2_KHR; };
	template<> struct VkStructTraits< VkPhysicalDeviceImageFormatInfo2KHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_FORMAT_INFO_2_KHR; };
	template<> struct VkStructTraits< VkQueueFamilyProperties2KHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2_KHR; };
	template<> struct VkStructTraits< VkPhysicalDeviceMemoryProperties2KHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2_KHR; };
	template<> struct VkStructTraits< VkSparseImageFormatProperties2KHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SPARSE_IMAGE_FORMAT_PROPERTIES_2_KHR; };
	template<> struct VkStructTraits< VkPhysicalDeviceSparseImageFormatInfo2KHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SPARSE_IMAGE_FORMAT_INFO_2_KHR; };
#endif
#if VK_KHR_maintenance2
	template<> struct VkStructTraits< VkPhysicalDevicePointClippingPropertiesKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_POINT_CLIPPING_PROPERTIES_KHR; };
	template<> struct VkStructTraits< VkRenderPassInputAttachmentAspectCreateInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_RENDER_PASS_INPUT_ATTACHMENT_ASPECT_CREATE_INFO_KHR; };
	template<> struct VkStructTraits< VkImageViewUsageCreateInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IMAGE_VIEW_USAGE_CREATE_INFO_KHR; };
	template<> struct VkStructTraits< VkPipelineTessellationDomainOriginStateCreateInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_DOMAIN_ORIGIN_STATE_CREATE_INFO_KHR; };
#endif
#if VK_KHR_multiview
	template<> struct VkStructTraits< VkRenderPassMultiviewCreateInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_RENDER_PASS_MULTIVIEW_CREATE_INFO_KHR; };
	template<> struct VkStructTraits< VkPhysicalDeviceMultiviewFeaturesKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES_KHR; };
	template<> struct VkStructTraits< VkPhysicalDeviceMultiviewPropertiesKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PROPERTIES_KHR; };
#endif
#if VK_KHR_variable_pointers
	template<> struct VkStructTraits< VkPhysicalDeviceVariablePointerFeaturesKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTER_FEATURES_KHR; };
#	if !VK_VERSION_1_1
	template<> struct VkStructTraits< VkPhysicalDeviceVariablePointersFeaturesKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTERS_FEATURES_KHR; };
#	endif
#endif
#if VK_KHR_sampler_ycbcr_conversion
	template<> struct VkStructTraits< VkSamplerYcbcrConversionCreateInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_CREATE_INFO_KHR; };
	template<> struct VkStructTraits< VkSamplerYcbcrConversionInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_INFO_KHR; };
	template<> struct VkStructTraits< VkBindImagePlaneMemoryInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_BIND_IMAGE_PLANE_MEMORY_INFO_KHR; };
	template<> struct VkStructTraits< VkImagePlaneMemoryRequirementsInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IMAGE_PLANE_MEMORY_REQUIREMENTS_INFO_KHR; };
	template<> struct VkStructTraits< VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES_KHR; };
	template<> struct VkStructTraits< VkSamplerYcbcrConversionImageFormatPropertiesKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_IMAGE_FORMAT_PROPERTIES_KHR; };
#endif
#if VK_KHR_descriptor_update_template
	template<> struct VkStructTraits< VkDescriptorUpdateTemplateCreateInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DESCRIPTOR_UPDATE_TEMPLATE_CREATE_INFO_KHR; };
#endif
#if VK_KHR_external_memory_capabilities
	template<> struct VkStructTraits< VkPhysicalDeviceExternalImageFormatInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_IMAGE_FORMAT_INFO_KHR; };
	template<> struct VkStructTraits< VkExternalImageFormatPropertiesKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_EXTERNAL_IMAGE_FORMAT_PROPERTIES_KHR; };
	template<> struct VkStructTraits< VkPhysicalDeviceExternalBufferInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_BUFFER_INFO_KHR; };
	template<> struct VkStructTraits< VkExternalBufferPropertiesKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_EXTERNAL_BUFFER_PROPERTIES_KHR; };
	template<> struct VkStructTraits< VkPhysicalDeviceIDPropertiesKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ID_PROPERTIES_KHR; };
#endif
#if VK_KHR_external_memory
	template<> struct VkStructTraits< VkExternalMemoryBufferCreateInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_BUFFER_CREATE_INFO_KHR; };
	template<> struct VkStructTraits< VkExternalMemoryImageCreateInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMAGE_CREATE_INFO_KHR; };
	template<> struct VkStructTraits< VkExportMemoryAllocateInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_EXPORT_MEMORY_ALLOCATE_INFO_KHR; };
#endif
#if VK_KHR_external_fence_capabilities
	template<> struct VkStructTraits< VkPhysicalDeviceExternalFenceInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_FENCE_INFO_KHR; };
	template<> struct VkStructTraits< VkExternalFencePropertiesKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_EXTERNAL_FENCE_PROPERTIES_KHR; };
	template<> struct VkStructTraits< VkExportFenceCreateInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_EXPORT_FENCE_CREATE_INFO_KHR; };
#endif
#if VK_KHR_external_semaphore
	template<> struct VkStructTraits< VkExportSemaphoreCreateInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_EXPORT_SEMAPHORE_CREATE_INFO_KHR; };
#endif
#if VK_KHR_external_semaphore_capabilities
	template<> struct VkStructTraits< VkPhysicalDeviceExternalSemaphoreInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_SEMAPHORE_INFO_KHR; };
	template<> struct VkStructTraits< VkExternalSemaphorePropertiesKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_EXTERNAL_SEMAPHORE_PROPERTIES_KHR; };
#endif
#if VK_KHR_maintenance3
	template<> struct VkStructTraits< VkPhysicalDeviceMaintenance3PropertiesKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_3_PROPERTIES_KHR; };
	template<> struct VkStructTraits< VkDescriptorSetLayoutSupportKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_SUPPORT_KHR; };
#endif
#if VK_KHR_swapchain
	template<> struct VkStructTraits< VkSwapchainCreateInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR; };
	template<> struct VkStructTraits< VkPresentInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR; };
	template<> struct VkStructTraits< VkDeviceGroupPresentCapabilitiesKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEVICE_GROUP_PRESENT_CAPABILITIES_KHR; };
	template<> struct VkStructTraits< VkImageSwapchainCreateInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IMAGE_SWAPCHAIN_CREATE_INFO_KHR; };
	template<> struct VkStructTraits< VkBindImageMemorySwapchainInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_SWAPCHAIN_INFO_KHR; };
	template<> struct VkStructTraits< VkAcquireNextImageInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_ACQUIRE_NEXT_IMAGE_INFO_KHR; };
	template<> struct VkStructTraits< VkDeviceGroupPresentInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEVICE_GROUP_PRESENT_INFO_KHR; };
	template<> struct VkStructTraits< VkDeviceGroupSwapchainCreateInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEVICE_GROUP_SWAPCHAIN_CREATE_INFO_KHR; };
#endif
#if VK_KHR_display
	template<> struct VkStructTraits< VkDisplayModeCreateInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DISPLAY_MODE_CREATE_INFO_KHR; };
	template<> struct VkStructTraits< VkDisplaySurfaceCreateInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DISPLAY_SURFACE_CREATE_INFO_KHR; };
	template<> struct VkStructTraits< VkDisplayPresentInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DISPLAY_PRESENT_INFO_KHR; };
#endif
#if VK_EXT_debug_report
	template<> struct VkStructTraits< VkDebugReportCallbackCreateInfoEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT; };
#endif
#if VK_AMD_rasterization_order
	template<> struct VkStructTraits< VkPipelineRasterizationStateRasterizationOrderAMD >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_RASTERIZATION_ORDER_AMD; };
#endif
#if VK_EXT_debug_marker
	template<> struct VkStructTraits< VkDebugMarkerObjectNameInfoEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEBUG_MARKER_OBJECT_NAME_INFO_EXT; };
	template<> struct VkStructTraits< VkDebugMarkerObjectTagInfoEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEBUG_MARKER_OBJECT_TAG_INFO_EXT; };
	template<> struct VkStructTraits< VkDebugMarkerMarkerInfoEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT; };
#endif
#if VK_NV_dedicated_allocation
	template<> struct VkStructTraits< VkDedicatedAllocationImageCreateInfoNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEDICATED_ALLOCATION_IMAGE_CREATE_INFO_NV; };
	template<> struct VkStructTraits< VkDedicatedAllocationBufferCreateInfoNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEDICATED_ALLOCATION_BUFFER_CREATE_INFO_NV; };
	template<> struct VkStructTraits< VkDedicatedAllocationMemoryAllocateInfoNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEDICATED_ALLOCATION_MEMORY_ALLOCATE_INFO_NV; };
#endif
#if VK_EXT_transform_feedback
	template<> struct VkStructTraits< VkPhysicalDeviceTransformFeedbackFeaturesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TRANSFORM_FEEDBACK_FEATURES_EXT; };
	template<> struct VkStructTraits< VkPhysicalDeviceTransformFeedbackPropertiesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TRANSFORM_FEEDBACK_PROPERTIES_EXT; };
	template<> struct VkStructTraits< VkPipelineRasterizationStateStreamCreateInfoEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_STREAM_CREATE_INFO_EXT; };
#endif
#if VK_AMD_texture_gather_bias_lod
	template<> struct VkStructTraits< VkTextureLODGatherFormatPropertiesAMD >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_TEXTURE_LOD_GATHER_FORMAT_PROPERTIES_AMD; };
#endif
#if VK_NV_corner_sampled_image
	template<> struct VkStructTraits< VkPhysicalDeviceCornerSampledImageFeaturesNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CORNER_SAMPLED_IMAGE_FEATURES_NV; };
#endif
#if VK_NV_external_memory
	template<> struct VkStructTraits< VkExternalMemoryImageCreateInfoNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMAGE_CREATE_INFO_NV; };
	template<> struct VkStructTraits< VkExportMemoryAllocateInfoNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_EXPORT_MEMORY_ALLOCATE_INFO_NV; };
#endif
#if VK_EXT_validation_flags
	template<> struct VkStructTraits< VkValidationFlagsEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_VALIDATION_FLAGS_EXT; };
#endif
#if VK_EXT_astc_decode_mode
	template<> struct VkStructTraits< VkImageViewASTCDecodeModeEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IMAGE_VIEW_ASTC_DECODE_MODE_EXT; };
	template<> struct VkStructTraits< VkPhysicalDeviceASTCDecodeFeaturesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ASTC_DECODE_FEATURES_EXT; };
#endif
#if VK_KHR_external_memory_fd
	template<> struct VkStructTraits< VkImportMemoryFdInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IMPORT_MEMORY_FD_INFO_KHR; };
	template<> struct VkStructTraits< VkMemoryFdPropertiesKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_MEMORY_FD_PROPERTIES_KHR; };
	template<> struct VkStructTraits< VkMemoryGetFdInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_MEMORY_GET_FD_INFO_KHR; };
#endif
#if VK_KHR_external_semaphore_fd
	template<> struct VkStructTraits< VkImportSemaphoreFdInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IMPORT_SEMAPHORE_FD_INFO_KHR; };
	template<> struct VkStructTraits< VkSemaphoreGetFdInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SEMAPHORE_GET_FD_INFO_KHR; };
#endif
#if VK_KHR_push_descriptor
	template<> struct VkStructTraits< VkPhysicalDevicePushDescriptorPropertiesKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PUSH_DESCRIPTOR_PROPERTIES_KHR; };
#endif
#if VK_EXT_conditional_rendering
	template<> struct VkStructTraits< VkCommandBufferInheritanceConditionalRenderingInfoEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_CONDITIONAL_RENDERING_INFO_EXT; };
	template<> struct VkStructTraits< VkPhysicalDeviceConditionalRenderingFeaturesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CONDITIONAL_RENDERING_FEATURES_EXT; };
	template<> struct VkStructTraits< VkConditionalRenderingBeginInfoEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_CONDITIONAL_RENDERING_BEGIN_INFO_EXT; };
#endif
#if VK_KHR_shader_float16_int8
	template<> struct VkStructTraits< VkPhysicalDeviceFloat16Int8FeaturesKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FLOAT16_INT8_FEATURES_KHR; };
#endif
#if VK_KHR_incremental_present
	template<> struct VkStructTraits< VkPresentRegionsKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PRESENT_REGIONS_KHR; };
#endif
#if VK_NVX_device_generated_commands
	template<> struct VkStructTraits< VkObjectTableCreateInfoNVX >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_OBJECT_TABLE_CREATE_INFO_NVX; };
	template<> struct VkStructTraits< VkIndirectCommandsLayoutCreateInfoNVX >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_INDIRECT_COMMANDS_LAYOUT_CREATE_INFO_NVX; };
	template<> struct VkStructTraits< VkCmdProcessCommandsInfoNVX >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_CMD_PROCESS_COMMANDS_INFO_NVX; };
	template<> struct VkStructTraits< VkCmdReserveSpaceForCommandsInfoNVX >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_CMD_RESERVE_SPACE_FOR_COMMANDS_INFO_NVX; };
	template<> struct VkStructTraits< VkDeviceGeneratedCommandsLimitsNVX >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEVICE_GENERATED_COMMANDS_LIMITS_NVX; };
	template<> struct VkStructTraits< VkDeviceGeneratedCommandsFeaturesNVX >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEVICE_GENERATED_COMMANDS_FEATURES_NVX; };
#endif
#if VK_NV_clip_space_w_scaling
	template<> struct VkStructTraits< VkPipelineViewportWScalingStateCreateInfoNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_W_SCALING_STATE_CREATE_INFO_NV; };
#endif
#if VK_EXT_display_surface_counter
	template<> struct VkStructTraits< VkSurfaceCapabilities2EXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SURFACE_CAPABILITIES_2_EXT; };
#endif
#if VK_EXT_display_control
	template<> struct VkStructTraits< VkDisplayPowerInfoEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DISPLAY_POWER_INFO_EXT; };
	template<> struct VkStructTraits< VkDeviceEventInfoEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEVICE_EVENT_INFO_EXT; };
	template<> struct VkStructTraits< VkDisplayEventInfoEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DISPLAY_EVENT_INFO_EXT; };
	template<> struct VkStructTraits< VkSwapchainCounterCreateInfoEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SWAPCHAIN_COUNTER_CREATE_INFO_EXT; };
#endif
#if VK_GOOGLE_display_timing
	template<> struct VkStructTraits< VkPresentTimesInfoGOOGLE >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PRESENT_TIMES_INFO_GOOGLE; };
#endif
#if VK_NVX_multiview_per_view_attributes
	template<> struct VkStructTraits< VkPhysicalDeviceMultiviewPerViewAttributesPropertiesNVX >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PER_VIEW_ATTRIBUTES_PROPERTIES_NVX; };
#endif
#if VK_NV_viewport_swizzle
	template<> struct VkStructTraits< VkPipelineViewportSwizzleStateCreateInfoNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_SWIZZLE_STATE_CREATE_INFO_NV; };
#endif
#if VK_EXT_discard_rectangles
	template<> struct VkStructTraits< VkPhysicalDeviceDiscardRectanglePropertiesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DISCARD_RECTANGLE_PROPERTIES_EXT; };
	template<> struct VkStructTraits< VkPipelineDiscardRectangleStateCreateInfoEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_DISCARD_RECTANGLE_STATE_CREATE_INFO_EXT; };
#endif
#if VK_EXT_conservative_rasterization
	template<> struct VkStructTraits< VkPhysicalDeviceConservativeRasterizationPropertiesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CONSERVATIVE_RASTERIZATION_PROPERTIES_EXT; };
	template<> struct VkStructTraits< VkPipelineRasterizationConservativeStateCreateInfoEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_CONSERVATIVE_STATE_CREATE_INFO_EXT; };
#endif
#if VK_EXT_depth_clip_enable
	template<> struct VkStructTraits< VkPhysicalDeviceDepthClipEnableFeaturesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLIP_ENABLE_FEATURES_EXT; };
	template<> struct VkStructTraits< VkPipelineRasterizationDepthClipStateCreateInfoEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_DEPTH_CLIP_STATE_CREATE_INFO_EXT; };
#endif
#if VK_EXT_hdr_metadata
	template<> struct VkStructTraits< VkHdrMetadataEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_HDR_METADATA_EXT; };
#endif
#if VK_KHR_create_renderpass2
	template<> struct VkStructTraits< VkAttachmentDescription2KHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2_KHR; };
	template<> struct VkStructTraits< VkAttachmentReference2KHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2_KHR; };
	template<> struct VkStructTraits< VkSubpassDescription2KHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2_KHR; };
	template<> struct VkStructTraits< VkSubpassDependency2KHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2_KHR; };
	template<> struct VkStructTraits< VkRenderPassCreateInfo2KHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2_KHR; };
	template<> struct VkStructTraits< VkSubpassBeginInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SUBPASS_BEGIN_INFO_KHR; };
	template<> struct VkStructTraits< VkSubpassEndInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SUBPASS_END_INFO_KHR; };
#endif
#if VK_KHR_shared_presentable_image
	template<> struct VkStructTraits< VkSharedPresentSurfaceCapabilitiesKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SHARED_PRESENT_SURFACE_CAPABILITIES_KHR; };
#endif
#if VK_KHR_external_fence_fd
	template<> struct VkStructTraits< VkImportFenceFdInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IMPORT_FENCE_FD_INFO_KHR; };
	template<> struct VkStructTraits< VkFenceGetFdInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_FENCE_GET_FD_INFO_KHR; };
#endif
#if VK_KHR_get_surface_capabilities2
	template<> struct VkStructTraits< VkPhysicalDeviceSurfaceInfo2KHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SURFACE_INFO_2_KHR; };
	template<> struct VkStructTraits< VkSurfaceCapabilities2KHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SURFACE_CAPABILITIES_2_KHR; };
	template<> struct VkStructTraits< VkSurfaceFormat2KHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SURFACE_FORMAT_2_KHR; };
#endif
#if VK_KHR_get_display_properties2
	template<> struct VkStructTraits< VkDisplayProperties2KHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DISPLAY_PROPERTIES_2_KHR; };
	template<> struct VkStructTraits< VkDisplayPlaneProperties2KHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DISPLAY_PLANE_PROPERTIES_2_KHR; };
	template<> struct VkStructTraits< VkDisplayModeProperties2KHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DISPLAY_MODE_PROPERTIES_2_KHR; };
	template<> struct VkStructTraits< VkDisplayPlaneInfo2KHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DISPLAY_PLANE_INFO_2_KHR; };
	template<> struct VkStructTraits< VkDisplayPlaneCapabilities2KHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DISPLAY_PLANE_CAPABILITIES_2_KHR; };
#endif
#if VK_EXT_debug_utils
	template<> struct VkStructTraits< VkDebugUtilsObjectNameInfoEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT; };
	template<> struct VkStructTraits< VkDebugUtilsObjectTagInfoEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_TAG_INFO_EXT; };
	template<> struct VkStructTraits< VkDebugUtilsLabelEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT; };
	template<> struct VkStructTraits< VkDebugUtilsMessengerCallbackDataEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CALLBACK_DATA_EXT; };
	template<> struct VkStructTraits< VkDebugUtilsMessengerCreateInfoEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT; };
#endif
#if VK_EXT_sampler_filter_minmax
	template<> struct VkStructTraits< VkPhysicalDeviceSamplerFilterMinmaxPropertiesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_FILTER_MINMAX_PROPERTIES_EXT; };
	template<> struct VkStructTraits< VkSamplerReductionModeCreateInfoEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SAMPLER_REDUCTION_MODE_CREATE_INFO_EXT; };
#endif
#if VK_EXT_inline_uniform_block
	template<> struct VkStructTraits< VkPhysicalDeviceInlineUniformBlockFeaturesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_FEATURES_EXT; };
	template<> struct VkStructTraits< VkPhysicalDeviceInlineUniformBlockPropertiesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_PROPERTIES_EXT; };
	template<> struct VkStructTraits< VkWriteDescriptorSetInlineUniformBlockEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_INLINE_UNIFORM_BLOCK_EXT; };
	template<> struct VkStructTraits< VkDescriptorPoolInlineUniformBlockCreateInfoEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_INLINE_UNIFORM_BLOCK_CREATE_INFO_EXT; };
#endif
#if VK_EXT_sample_locations
	template<> struct VkStructTraits< VkSampleLocationsInfoEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SAMPLE_LOCATIONS_INFO_EXT; };
	template<> struct VkStructTraits< VkRenderPassSampleLocationsBeginInfoEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_RENDER_PASS_SAMPLE_LOCATIONS_BEGIN_INFO_EXT; };
	template<> struct VkStructTraits< VkPipelineSampleLocationsStateCreateInfoEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_SAMPLE_LOCATIONS_STATE_CREATE_INFO_EXT; };
	template<> struct VkStructTraits< VkPhysicalDeviceSampleLocationsPropertiesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLE_LOCATIONS_PROPERTIES_EXT; };
	template<> struct VkStructTraits< VkMultisamplePropertiesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_MULTISAMPLE_PROPERTIES_EXT; };
#endif
#if VK_KHR_separate_depth_stencil_layouts
	template<> struct VkStructTraits< VkPhysicalDeviceSeparateDepthStencilLayoutsFeaturesKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SEPARATE_DEPTH_STENCIL_LAYOUTS_FEATURES_KHR; };
	template<> struct VkStructTraits< VkAttachmentReferenceStencilLayout >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_STENCIL_LAYOUT_KHR; };
	template<> struct VkStructTraits< VkAttachmentDescriptionStencilLayout >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_STENCIL_LAYOUT_KHR; };
#endif
#if VK_KHR_timeline_semaphore
	template<> struct VkStructTraits< VkPhysicalDeviceTimelineSemaphoreFeaturesKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES_KHR; };
	template<> struct VkStructTraits< VkPhysicalDeviceTimelineSemaphorePropertiesKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_PROPERTIES_KHR; };
	template<> struct VkStructTraits< VkSemaphoreTypeCreateInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO_KHR; };
	template<> struct VkStructTraits< VkTimelineSemaphoreSubmitInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO_KHR; };
	template<> struct VkStructTraits< VkSemaphoreWaitInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO_KHR; };
	template<> struct VkStructTraits< VkSemaphoreSignalInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SEMAPHORE_SIGNAL_INFO_KHR; };
#endif
#if VK_EXT_host_query_reset
	template<> struct VkStructTraits< VkPhysicalDeviceHostQueryResetFeaturesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES_EXT; };
#endif
#if VK_KHR_uniform_buffer_standard_layout
	template<> struct VkStructTraits< VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_UNIFORM_BUFFER_STANDARD_LAYOUT_FEATURES_KHR; };
#endif
#if VK_KHR_shader_subgroup_extended_types
	template<> struct VkStructTraits< VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_EXTENDED_TYPES_FEATURES_KHR; };
#endif
#if VK_KHR_buffer_device_address
	template<> struct VkStructTraits< VkPhysicalDeviceBufferDeviceAddressFeaturesKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES_KHR; };
	template<> struct VkStructTraits< VkBufferDeviceAddressInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO_KHR; };
	template<> struct VkStructTraits< VkBufferOpaqueCaptureAddressCreateInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_BUFFER_OPAQUE_CAPTURE_ADDRESS_CREATE_INFO_KHR; };
	template<> struct VkStructTraits< VkMemoryOpaqueCaptureAddressAllocateInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_MEMORY_OPAQUE_CAPTURE_ADDRESS_ALLOCATE_INFO_KHR; };
	template<> struct VkStructTraits< VkDeviceMemoryOpaqueCaptureAddressInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEVICE_MEMORY_OPAQUE_CAPTURE_ADDRESS_INFO_KHR; };
#	if VK_EXT_buffer_device_address
	template<> struct VkStructTraits< VkPhysicalDeviceBufferDeviceAddressFeaturesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES_EXT; };
	template<> struct VkStructTraits< VkBufferDeviceAddressCreateInfoEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_CREATE_INFO_EXT; };
#	endif
#elif VK_EXT_buffer_device_address
	template<> struct VkStructTraits< VkPhysicalDeviceBufferDeviceAddressFeaturesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES_EXT; };
	template<> struct VkStructTraits< VkBufferDeviceAddressInfoEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO_EXT; };
	template<> struct VkStructTraits< VkBufferDeviceAddressCreateInfoEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_CREATE_INFO_EXT; };
#endif
#if VK_KHR_image_format_list
	template<> struct VkStructTraits< VkImageFormatListCreateInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IMAGE_FORMAT_LIST_CREATE_INFO_KHR; };
#endif
#if VK_KHR_imageless_framebuffer
	template<> struct VkStructTraits< VkPhysicalDeviceImagelessFramebufferFeaturesKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IMAGE_FORMAT_LIST_CREATE_INFO_KHR; };
	template<> struct VkStructTraits< VkFramebufferAttachmentsCreateInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_FRAMEBUFFER_ATTACHMENTS_CREATE_INFO_KHR; };
	template<> struct VkStructTraits< VkFramebufferAttachmentImageInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_FRAMEBUFFER_ATTACHMENT_IMAGE_INFO_KHR; };
	template<> struct VkStructTraits< VkRenderPassAttachmentBeginInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_RENDER_PASS_ATTACHMENT_BEGIN_INFO_KHR; };
#endif
#if VK_EXT_blend_operation_advanced
	template<> struct VkStructTraits< VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BLEND_OPERATION_ADVANCED_FEATURES_EXT; };
	template<> struct VkStructTraits< VkPhysicalDeviceBlendOperationAdvancedPropertiesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BLEND_OPERATION_ADVANCED_PROPERTIES_EXT; };
	template<> struct VkStructTraits< VkPipelineColorBlendAdvancedStateCreateInfoEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_ADVANCED_STATE_CREATE_INFO_EXT; };
#endif
#if VK_NV_fragment_coverage_to_color
	template<> struct VkStructTraits< VkPipelineCoverageToColorStateCreateInfoNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_COVERAGE_TO_COLOR_STATE_CREATE_INFO_NV; };
#endif
#if VK_NV_framebuffer_mixed_samples
	template<> struct VkStructTraits< VkPipelineCoverageModulationStateCreateInfoNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_COVERAGE_MODULATION_STATE_CREATE_INFO_NV; };
#endif
#if VK_EXT_image_drm_format_modifier
	template<> struct VkStructTraits< VkDrmFormatModifierPropertiesListEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DRM_FORMAT_MODIFIER_PROPERTIES_LIST_EXT; };
	template<> struct VkStructTraits< VkPhysicalDeviceImageDrmFormatModifierInfoEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_DRM_FORMAT_MODIFIER_INFO_EXT; };
	template<> struct VkStructTraits< VkImageDrmFormatModifierListCreateInfoEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IMAGE_DRM_FORMAT_MODIFIER_LIST_CREATE_INFO_EXT; };
	template<> struct VkStructTraits< VkImageDrmFormatModifierExplicitCreateInfoEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IMAGE_DRM_FORMAT_MODIFIER_EXPLICIT_CREATE_INFO_EXT; };
	template<> struct VkStructTraits< VkImageDrmFormatModifierPropertiesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IMAGE_DRM_FORMAT_MODIFIER_PROPERTIES_EXT; };
#endif
#if VK_EXT_validation_cache
	template<> struct VkStructTraits< VkValidationCacheCreateInfoEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_VALIDATION_CACHE_CREATE_INFO_EXT; };
	template<> struct VkStructTraits< VkShaderModuleValidationCacheCreateInfoEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SHADER_MODULE_VALIDATION_CACHE_CREATE_INFO_EXT; };
#endif
#if VK_EXT_descriptor_indexing
	template<> struct VkStructTraits< VkDescriptorSetLayoutBindingFlagsCreateInfoEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT; };
	template<> struct VkStructTraits< VkPhysicalDeviceDescriptorIndexingFeaturesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT; };
	template<> struct VkStructTraits< VkPhysicalDeviceDescriptorIndexingPropertiesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES_EXT; };
	template<> struct VkStructTraits< VkDescriptorSetVariableDescriptorCountAllocateInfoEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO_EXT; };
	template<> struct VkStructTraits< VkDescriptorSetVariableDescriptorCountLayoutSupportEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_LAYOUT_SUPPORT_EXT; };
#endif
#if VK_NV_shading_rate_image
	template<> struct VkStructTraits< VkPipelineViewportShadingRateImageStateCreateInfoNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_SHADING_RATE_IMAGE_STATE_CREATE_INFO_NV; };
	template<> struct VkStructTraits< VkPhysicalDeviceShadingRateImageFeaturesNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADING_RATE_IMAGE_FEATURES_NV; };
	template<> struct VkStructTraits< VkPhysicalDeviceShadingRateImagePropertiesNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADING_RATE_IMAGE_PROPERTIES_NV; };
	template<> struct VkStructTraits< VkPipelineViewportCoarseSampleOrderStateCreateInfoNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_COARSE_SAMPLE_ORDER_STATE_CREATE_INFO_NV; };
#endif
#if VK_NV_ray_tracing
	template<> struct VkStructTraits< VkRayTracingPipelineCreateInfoNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_NV; };
	template<> struct VkStructTraits< VkAccelerationStructureCreateInfoNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_NV; };
	template<> struct VkStructTraits< VkGeometryNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_GEOMETRY_NV; };
	template<> struct VkStructTraits< VkGeometryTrianglesNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_GEOMETRY_TRIANGLES_NV; };
	template<> struct VkStructTraits< VkGeometryAABBNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_GEOMETRY_AABB_NV; };
	template<> struct VkStructTraits< VkBindAccelerationStructureMemoryInfoNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_BIND_ACCELERATION_STRUCTURE_MEMORY_INFO_NV; };
	template<> struct VkStructTraits< VkWriteDescriptorSetAccelerationStructureNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_NV; };
	template<> struct VkStructTraits< VkAccelerationStructureMemoryRequirementsInfoNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_INFO_NV; };
	template<> struct VkStructTraits< VkPhysicalDeviceRayTracingPropertiesNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PROPERTIES_NV; };
	template<> struct VkStructTraits< VkRayTracingShaderGroupCreateInfoNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_NV; };
	template<> struct VkStructTraits< VkAccelerationStructureInfoNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_INFO_NV; };
#endif
#if VK_NV_representative_fragment_test
	template<> struct VkStructTraits< VkPhysicalDeviceRepresentativeFragmentTestFeaturesNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_REPRESENTATIVE_FRAGMENT_TEST_FEATURES_NV; };
	template<> struct VkStructTraits< VkPipelineRepresentativeFragmentTestStateCreateInfoNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_REPRESENTATIVE_FRAGMENT_TEST_STATE_CREATE_INFO_NV; };
#endif
#if VK_EXT_filter_cubic
	template<> struct VkStructTraits< VkPhysicalDeviceImageViewImageFormatInfoEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_VIEW_IMAGE_FORMAT_INFO_EXT; };
	template<> struct VkStructTraits< VkFilterCubicImageViewImageFormatPropertiesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_FILTER_CUBIC_IMAGE_VIEW_IMAGE_FORMAT_PROPERTIES_EXT; };
#endif
#if VK_EXT_global_priority
	template<> struct VkStructTraits< VkDeviceQueueGlobalPriorityCreateInfoEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEVICE_QUEUE_GLOBAL_PRIORITY_CREATE_INFO_EXT; };
#endif
#if VK_KHR_8bit_storage
	template<> struct VkStructTraits< VkPhysicalDevice8BitStorageFeaturesKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES_KHR; };
#endif
#if VK_EXT_external_memory_host
	template<> struct VkStructTraits< VkImportMemoryHostPointerInfoEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IMPORT_MEMORY_HOST_POINTER_INFO_EXT; };
	template<> struct VkStructTraits< VkMemoryHostPointerPropertiesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_MEMORY_HOST_POINTER_PROPERTIES_EXT; };
	template<> struct VkStructTraits< VkPhysicalDeviceExternalMemoryHostPropertiesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_MEMORY_HOST_PROPERTIES_EXT; };
#endif
#if VK_KHR_shader_atomic_int64
	template<> struct VkStructTraits< VkPhysicalDeviceShaderAtomicInt64FeaturesKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_INT64_FEATURES_KHR; };
#endif
#if VK_EXT_calibrated_timestamps
	template<> struct VkStructTraits< VkCalibratedTimestampInfoEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_CALIBRATED_TIMESTAMP_INFO_EXT; };
#endif
#if VK_AMD_shader_core_properties
	template<> struct VkStructTraits< VkPhysicalDeviceShaderCorePropertiesAMD >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CORE_PROPERTIES_AMD; };
#endif
#if VK_AMD_memory_overallocation_behavior
	template<> struct VkStructTraits< VkDeviceMemoryOverallocationCreateInfoAMD >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEVICE_MEMORY_OVERALLOCATION_CREATE_INFO_AMD; };
#endif
#if VK_EXT_vertex_attribute_divisor
	template<> struct VkStructTraits< VkPhysicalDeviceVertexAttributeDivisorPropertiesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_PROPERTIES_EXT; };
	template<> struct VkStructTraits< VkPipelineVertexInputDivisorStateCreateInfoEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_DIVISOR_STATE_CREATE_INFO_EXT; };
	template<> struct VkStructTraits< VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_FEATURES_EXT; };
#endif
#if VK_KHR_driver_properties
	template<> struct VkStructTraits< VkPhysicalDeviceDriverPropertiesKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES_KHR; };
#endif
#if VK_KHR_shader_float_controls
	template<> struct VkStructTraits< VkPhysicalDeviceFloatControlsPropertiesKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FLOAT_CONTROLS_PROPERTIES_KHR; };
#endif
#if VK_KHR_depth_stencil_resolve
	template<> struct VkStructTraits< VkPhysicalDeviceDepthStencilResolvePropertiesKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_STENCIL_RESOLVE_PROPERTIES_KHR; };
	template<> struct VkStructTraits< VkSubpassDescriptionDepthStencilResolveKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_DEPTH_STENCIL_RESOLVE_KHR; };
#endif
#if VK_NV_compute_shader_derivatives
	template<> struct VkStructTraits< VkPhysicalDeviceComputeShaderDerivativesFeaturesNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COMPUTE_SHADER_DERIVATIVES_FEATURES_NV; };
#endif
#if VK_NV_mesh_shader
	template<> struct VkStructTraits< VkPhysicalDeviceMeshShaderFeaturesNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_NV; };
	template<> struct VkStructTraits< VkPhysicalDeviceMeshShaderPropertiesNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_PROPERTIES_NV; };
#endif
#if VK_NV_fragment_shader_barycentric
	template<> struct VkStructTraits< VkPhysicalDeviceFragmentShaderBarycentricFeaturesNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADER_BARYCENTRIC_FEATURES_NV; };
#endif
#if VK_NV_shader_image_footprint
	template<> struct VkStructTraits< VkPhysicalDeviceShaderImageFootprintFeaturesNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_IMAGE_FOOTPRINT_FEATURES_NV; };
#endif
#if VK_NV_scissor_exclusive
	template<> struct VkStructTraits< VkPipelineViewportExclusiveScissorStateCreateInfoNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_EXCLUSIVE_SCISSOR_STATE_CREATE_INFO_NV; };
	template<> struct VkStructTraits< VkPhysicalDeviceExclusiveScissorFeaturesNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXCLUSIVE_SCISSOR_FEATURES_NV; };
#endif
#if VK_NV_device_diagnostic_checkpoints
	template<> struct VkStructTraits< VkCheckpointDataNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_CHECKPOINT_DATA_NV; };
	template<> struct VkStructTraits< VkQueueFamilyCheckpointPropertiesNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_QUEUE_FAMILY_CHECKPOINT_PROPERTIES_NV; };
#endif
#if VK_KHR_vulkan_memory_model
	template<> struct VkStructTraits< VkPhysicalDeviceVulkanMemoryModelFeaturesKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_MEMORY_MODEL_FEATURES_KHR; };
#endif
#if VK_KHR_shader_terminate_invocation
	template<> struct VkStructTraits< VkPhysicalDeviceShaderTerminateInvocationFeaturesKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_TERMINATE_INVOCATION_FEATURES_KHR; };
#endif
#if VK_EXT_pci_bus_info
	template<> struct VkStructTraits< VkPhysicalDevicePCIBusInfoPropertiesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PCI_BUS_INFO_PROPERTIES_EXT; };
#endif
#if VK_EXT_fragment_density_map
	template<> struct VkStructTraits< VkPhysicalDeviceFragmentDensityMapFeaturesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_FEATURES_EXT; };
	template<> struct VkStructTraits< VkPhysicalDeviceFragmentDensityMapPropertiesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_PROPERTIES_EXT; };
	template<> struct VkStructTraits< VkRenderPassFragmentDensityMapCreateInfoEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_RENDER_PASS_FRAGMENT_DENSITY_MAP_CREATE_INFO_EXT; };
#endif
#if VK_EXT_scalar_block_layout
	template<> struct VkStructTraits< VkPhysicalDeviceScalarBlockLayoutFeaturesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SCALAR_BLOCK_LAYOUT_FEATURES_EXT; };
#endif
#if VK_EXT_memory_budget
	template<> struct VkStructTraits< VkPhysicalDeviceMemoryBudgetPropertiesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_BUDGET_PROPERTIES_EXT; };
#endif
#if VK_EXT_memory_priority
	template<> struct VkStructTraits< VkPhysicalDeviceMemoryPriorityFeaturesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PRIORITY_FEATURES_EXT; };
	template<> struct VkStructTraits< VkMemoryPriorityAllocateInfoEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_MEMORY_PRIORITY_ALLOCATE_INFO_EXT; };
#endif
#if VK_NV_dedicated_allocation_image_aliasing
	template<> struct VkStructTraits< VkPhysicalDeviceDedicatedAllocationImageAliasingFeaturesNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEDICATED_ALLOCATION_IMAGE_ALIASING_FEATURES_NV; };
#endif
#if VK_EXT_separate_stencil_usage
	template<> struct VkStructTraits< VkImageStencilUsageCreateInfoEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IMAGE_STENCIL_USAGE_CREATE_INFO_EXT; };
#endif
#if VK_EXT_validation_features
	template<> struct VkStructTraits< VkValidationFeaturesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT; };
#endif
#if VK_NV_cooperative_matrix
	template<> struct VkStructTraits< VkPhysicalDeviceCooperativeMatrixFeaturesNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_FEATURES_NV; };
	template<> struct VkStructTraits< VkCooperativeMatrixPropertiesNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_COOPERATIVE_MATRIX_PROPERTIES_NV; };
	template<> struct VkStructTraits< VkPhysicalDeviceCooperativeMatrixPropertiesNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_PROPERTIES_NV; };
#endif
#if VK_NV_coverage_reduction_mode
	template<> struct VkStructTraits< VkPipelineCoverageReductionStateCreateInfoNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_COVERAGE_REDUCTION_STATE_CREATE_INFO_NV; };
	template<> struct VkStructTraits< VkFramebufferMixedSamplesCombinationNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_FRAMEBUFFER_MIXED_SAMPLES_COMBINATION_NV; };
#endif
#if VK_EXT_fragment_shader_interlock
	template<> struct VkStructTraits< VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADER_INTERLOCK_FEATURES_EXT; };
#endif
#if VK_EXT_ycbcr_image_arrays
	template<> struct VkStructTraits< VkPhysicalDeviceYcbcrImageArraysFeaturesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_YCBCR_IMAGE_ARRAYS_FEATURES_EXT; };
#endif
#if VK_EXT_provoking_vertex
	template<> struct VkStructTraits< VkPhysicalDeviceProvokingVertexFeaturesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROVOKING_VERTEX_FEATURES_EXT; };
	template<> struct VkStructTraits< VkPipelineRasterizationProvokingVertexStateCreateInfoEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_PROVOKING_VERTEX_STATE_CREATE_INFO_EXT; };
	template<> struct VkStructTraits< VkPhysicalDeviceProvokingVertexPropertiesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROVOKING_VERTEX_PROPERTIES_EXT; };
#endif
#if VK_EXT_full_screen_exclusive
	template<> struct VkStructTraits< VkSurfaceFullScreenExclusiveInfoEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SURFACE_FULL_SCREEN_EXCLUSIVE_INFO_EXT; };
	template<> struct VkStructTraits< VkSurfaceCapabilitiesFullScreenExclusiveEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SURFACE_CAPABILITIES_FULL_SCREEN_EXCLUSIVE_EXT; };
	template<> struct VkStructTraits< VkSurfaceFullScreenExclusiveWin32InfoEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SURFACE_FULL_SCREEN_EXCLUSIVE_WIN32_INFO_EXT; };
#endif
#if VK_EXT_headless_surface
	template<> struct VkStructTraits< VkHeadlessSurfaceCreateInfoEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_HEADLESS_SURFACE_CREATE_INFO_EXT; };
#endif
#if VK_EXT_line_rasterization
	template<> struct VkStructTraits< VkPhysicalDeviceLineRasterizationFeaturesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINE_RASTERIZATION_FEATURES_EXT; };
	template<> struct VkStructTraits< VkPipelineRasterizationLineStateCreateInfoEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_LINE_STATE_CREATE_INFO_EXT; };
	template<> struct VkStructTraits< VkPhysicalDeviceLineRasterizationPropertiesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINE_RASTERIZATION_PROPERTIES_EXT; };
#endif
#if VK_EXT_shader_atomic_float
	template<> struct VkStructTraits< VkPhysicalDeviceShaderAtomicFloatFeaturesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT_FEATURES_EXT; };
#endif
#if VK_EXT_index_type_uint8
	template<> struct VkStructTraits< VkPhysicalDeviceIndexTypeUint8FeaturesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INDEX_TYPE_UINT8_FEATURES_EXT; };
#endif
#if VK_EXT_extended_dynamic_state
	template<> struct VkStructTraits< VkPhysicalDeviceExtendedDynamicStateFeaturesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT; };
#endif
#if VK_KHR_pipeline_executable_properties
	template<> struct VkStructTraits< VkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_EXECUTABLE_PROPERTIES_FEATURES_KHR; };
	template<> struct VkStructTraits< VkPipelineInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_INFO_KHR; };
	template<> struct VkStructTraits< VkPipelineExecutablePropertiesKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_EXECUTABLE_PROPERTIES_KHR; };
	template<> struct VkStructTraits< VkPipelineExecutableInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_EXECUTABLE_INFO_KHR; };
	template<> struct VkStructTraits< VkPipelineExecutableStatisticKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_EXECUTABLE_STATISTIC_KHR; };
	template<> struct VkStructTraits< VkPipelineExecutableInternalRepresentationKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_EXECUTABLE_INTERNAL_REPRESENTATION_KHR; };
#endif
#if VK_EXT_shader_atomic_float2
	template<> struct VkStructTraits< VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT_2_FEATURES_EXT; };
#endif
#if VK_EXT_shader_demote_to_helper_invocation
	template<> struct VkStructTraits< VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DEMOTE_TO_HELPER_INVOCATION_FEATURES_EXT; };
#endif
#if VK_NV_device_generated_commands
	template<> struct VkStructTraits< VkPhysicalDeviceDeviceGeneratedCommandsPropertiesNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_GENERATED_COMMANDS_PROPERTIES_NV; };
	template<> struct VkStructTraits< VkGraphicsShaderGroupCreateInfoNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_GRAPHICS_SHADER_GROUP_CREATE_INFO_NV; };
	template<> struct VkStructTraits< VkGraphicsPipelineShaderGroupsCreateInfoNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_SHADER_GROUPS_CREATE_INFO_NV; };
	template<> struct VkStructTraits< VkIndirectCommandsLayoutTokenNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_INDIRECT_COMMANDS_LAYOUT_TOKEN_NV; };
	template<> struct VkStructTraits< VkIndirectCommandsLayoutCreateInfoNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_INDIRECT_COMMANDS_LAYOUT_CREATE_INFO_NV; };
	template<> struct VkStructTraits< VkGeneratedCommandsInfoNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_GENERATED_COMMANDS_INFO_NV; };
	template<> struct VkStructTraits< VkGeneratedCommandsMemoryRequirementsInfoNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_GENERATED_COMMANDS_MEMORY_REQUIREMENTS_INFO_NV; };
	template<> struct VkStructTraits< VkPhysicalDeviceDeviceGeneratedCommandsFeaturesNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_GENERATED_COMMANDS_FEATURES_NV; };
#endif
#if VK_NV_inherited_viewport_scissor
	template<> struct VkStructTraits< VkPhysicalDeviceInheritedViewportScissorFeaturesNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INHERITED_VIEWPORT_SCISSOR_FEATURES_NV; };
	template<> struct VkStructTraits< VkCommandBufferInheritanceViewportScissorInfoNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_VIEWPORT_SCISSOR_INFO_NV; };
#endif
#if VK_KHR_shader_integer_dot_product
	template<> struct VkStructTraits< VkPhysicalDeviceShaderIntegerDotProductFeaturesKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_INTEGER_DOT_PRODUCT_FEATURES_KHR; };
	template<> struct VkStructTraits< VkPhysicalDeviceShaderIntegerDotProductPropertiesKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_INTEGER_DOT_PRODUCT_PROPERTIES_KHR; };
#endif
#if VK_EXT_texel_buffer_alignment
	template<> struct VkStructTraits< VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXEL_BUFFER_ALIGNMENT_FEATURES_EXT; };
	template<> struct VkStructTraits< VkPhysicalDeviceTexelBufferAlignmentPropertiesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXEL_BUFFER_ALIGNMENT_PROPERTIES_EXT; };
#endif
#if VK_QCOM_render_pass_transform
	template<> struct VkStructTraits< VkCommandBufferInheritanceRenderPassTransformInfoQCOM >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_RENDER_PASS_TRANSFORM_INFO_QCOM; };
	template<> struct VkStructTraits< VkRenderPassTransformBeginInfoQCOM >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_RENDER_PASS_TRANSFORM_BEGIN_INFO_QCOM; };
#endif
#if VK_EXT_device_memory_report
	template<> struct VkStructTraits< VkPhysicalDeviceDeviceMemoryReportFeaturesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEVICE_MEMORY_REPORT_FEATURES_EXT; };
	template<> struct VkStructTraits< VkDeviceDeviceMemoryReportCreateInfoEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEVICE_DEVICE_MEMORY_REPORT_CREATE_INFO_EXT; };
	template<> struct VkStructTraits< VkDeviceMemoryReportCallbackDataEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEVICE_MEMORY_REPORT_CALLBACK_DATA_EXT; };
#endif
#if VK_EXT_robustness2
	template<> struct VkStructTraits< VkPhysicalDeviceRobustness2FeaturesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_FEATURES_EXT; };
	template<> struct VkStructTraits< VkPhysicalDeviceRobustness2PropertiesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_PROPERTIES_EXT; };
#endif
#if VK_EXT_custom_border_color
	template<> struct VkStructTraits< VkSamplerCustomBorderColorCreateInfoEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SAMPLER_CUSTOM_BORDER_COLOR_CREATE_INFO_EXT; };
	template<> struct VkStructTraits< VkPhysicalDeviceCustomBorderColorPropertiesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CUSTOM_BORDER_COLOR_PROPERTIES_EXT; };
	template<> struct VkStructTraits< VkPhysicalDeviceCustomBorderColorFeaturesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CUSTOM_BORDER_COLOR_FEATURES_EXT; };
#endif
#if VK_KHR_pipeline_library
	template<> struct VkStructTraits< VkPipelineLibraryCreateInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_LIBRARY_CREATE_INFO_KHR; };
#endif
#if VK_KHR_present_id
	template<> struct VkStructTraits< VkPresentIdKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PRESENT_ID_KHR; };
	template<> struct VkStructTraits< VkPhysicalDevicePresentIdFeaturesKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_ID_FEATURES_KHR; };
#endif
#if VK_EXT_private_data
	template<> struct VkStructTraits< VkPhysicalDevicePrivateDataFeaturesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRIVATE_DATA_FEATURES_EXT; };
	template<> struct VkStructTraits< VkDevicePrivateDataCreateInfoEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEVICE_PRIVATE_DATA_CREATE_INFO_EXT; };
	template<> struct VkStructTraits< VkPrivateDataSlotCreateInfoEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PRIVATE_DATA_SLOT_CREATE_INFO_EXT; };
#endif
#if VK_EXT_pipeline_creation_cache_control
	template<> struct VkStructTraits< VkPhysicalDevicePipelineCreationCacheControlFeaturesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_CREATION_CACHE_CONTROL_FEATURES_EXT; };
#endif
#if VK_NV_device_diagnostics_config
	template<> struct VkStructTraits< VkPhysicalDeviceDiagnosticsConfigFeaturesNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DIAGNOSTICS_CONFIG_FEATURES_NV; };
	template<> struct VkStructTraits< VkDeviceDiagnosticsConfigCreateInfoNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEVICE_DIAGNOSTICS_CONFIG_CREATE_INFO_NV; };
#endif
#if VK_KHR_synchronization2
	template<> struct VkStructTraits< VkMemoryBarrier2KHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2_KHR; };
	template<> struct VkStructTraits< VkBufferMemoryBarrier2KHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2_KHR; };
	template<> struct VkStructTraits< VkImageMemoryBarrier2KHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2_KHR; };
	template<> struct VkStructTraits< VkDependencyInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEPENDENCY_INFO_KHR; };
	template<> struct VkStructTraits< VkSubmitInfo2KHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SUBMIT_INFO_2_KHR; };
	template<> struct VkStructTraits< VkSemaphoreSubmitInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO_KHR; };
	template<> struct VkStructTraits< VkCommandBufferSubmitInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO_KHR; };
	template<> struct VkStructTraits< VkPhysicalDeviceSynchronization2FeaturesKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES_KHR; };
	template<> struct VkStructTraits< VkQueueFamilyCheckpointProperties2NV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_QUEUE_FAMILY_CHECKPOINT_PROPERTIES_2_NV; };
	template<> struct VkStructTraits< VkCheckpointData2NV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_CHECKPOINT_DATA_2_NV; };
#endif
#if VK_KHR_shader_subgroup_uniform_control_flow
	template<> struct VkStructTraits< VkPhysicalDeviceShaderSubgroupUniformControlFlowFeaturesKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_UNIFORM_CONTROL_FLOW_FEATURES_KHR; };
#endif
#if VK_KHR_zero_initialize_workgroup_memory
	template<> struct VkStructTraits< VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeaturesKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ZERO_INITIALIZE_WORKGROUP_MEMORY_FEATURES_KHR; };
#endif
#if VK_NV_fragment_shading_rate_enums
	template<> struct VkStructTraits< VkPhysicalDeviceFragmentShadingRateEnumsPropertiesNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_ENUMS_PROPERTIES_NV; };
	template<> struct VkStructTraits< VkPhysicalDeviceFragmentShadingRateEnumsFeaturesNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_ENUMS_FEATURES_NV; };
	template<> struct VkStructTraits< VkPipelineFragmentShadingRateEnumStateCreateInfoNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_FRAGMENT_SHADING_RATE_ENUM_STATE_CREATE_INFO_NV; };
#endif
#if VK_NV_ray_tracing_motion_blur
	template<> struct VkStructTraits< VkAccelerationStructureGeometryMotionTrianglesDataNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_MOTION_TRIANGLES_DATA_NV; };
	template<> struct VkStructTraits< VkPhysicalDeviceRayTracingMotionBlurFeaturesNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_MOTION_BLUR_FEATURES_NV; };
	template<> struct VkStructTraits< VkAccelerationStructureMotionInfoNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_MOTION_INFO_NV; };
#endif
#if VK_EXT_ycbcr_2plane_444_formats
	template<> struct VkStructTraits< VkPhysicalDeviceYcbcr2Plane444FormatsFeaturesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_YCBCR_2_PLANE_444_FORMATS_FEATURES_EXT; };
#endif
#if VK_EXT_fragment_density_map2
	template<> struct VkStructTraits< VkPhysicalDeviceFragmentDensityMap2FeaturesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_2_FEATURES_EXT; };
	template<> struct VkStructTraits< VkPhysicalDeviceFragmentDensityMap2PropertiesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_2_PROPERTIES_EXT; };
#endif
#if VK_QCOM_rotated_copy_commands
	template<> struct VkStructTraits< VkCopyCommandTransformInfoQCOM >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_COPY_COMMAND_TRANSFORM_INFO_QCOM; };
#endif
#if VK_EXT_image_robustness
	template<> struct VkStructTraits< VkPhysicalDeviceImageRobustnessFeaturesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_ROBUSTNESS_FEATURES_EXT; };
#endif
#if VK_KHR_workgroup_memory_explicit_layout
	template<> struct VkStructTraits< VkPhysicalDeviceWorkgroupMemoryExplicitLayoutFeaturesKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_WORKGROUP_MEMORY_EXPLICIT_LAYOUT_FEATURES_KHR; };
#endif
#if VK_KHR_copy_commands2
	template<> struct VkStructTraits< VkCopyBufferInfo2KHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2_KHR; };
	template<> struct VkStructTraits< VkCopyImageInfo2KHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_COPY_IMAGE_INFO_2_KHR; };
	template<> struct VkStructTraits< VkCopyBufferToImageInfo2KHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_COPY_BUFFER_TO_IMAGE_INFO_2_KHR; };
	template<> struct VkStructTraits< VkCopyImageToBufferInfo2KHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_COPY_IMAGE_TO_BUFFER_INFO_2_KHR; };
	template<> struct VkStructTraits< VkBlitImageInfo2KHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2_KHR; };
	template<> struct VkStructTraits< VkResolveImageInfo2KHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_RESOLVE_IMAGE_INFO_2_KHR; };
	template<> struct VkStructTraits< VkBufferCopy2KHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_BUFFER_COPY_2_KHR; };
	template<> struct VkStructTraits< VkImageCopy2KHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IMAGE_COPY_2_KHR; };
	template<> struct VkStructTraits< VkImageBlit2KHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IMAGE_BLIT_2_KHR; };
	template<> struct VkStructTraits< VkBufferImageCopy2KHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_BUFFER_IMAGE_COPY_2_KHR; };
	template<> struct VkStructTraits< VkImageResolve2KHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IMAGE_RESOLVE_2_KHR; };
#endif
#if VK_EXT_4444_formats
	template<> struct VkStructTraits< VkPhysicalDevice4444FormatsFeaturesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_4444_FORMATS_FEATURES_EXT; };
#endif
#if VK_EXT_rgba10x6_formats
	template<> struct VkStructTraits< VkPhysicalDeviceRGBA10X6FormatsFeaturesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RGBA10X6_FORMATS_FEATURES_EXT; };
#endif
#if VK_EXT_directfb_surface
	template<> struct VkStructTraits< VkDirectFBSurfaceCreateInfoEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DIRECTFB_SURFACE_CREATE_INFO_EXT; };
#endif
#if VK_VALVE_mutable_descriptor_type
	template<> struct VkStructTraits< VkPhysicalDeviceMutableDescriptorTypeFeaturesVALVE >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MUTABLE_DESCRIPTOR_TYPE_FEATURES_VALVE; };
	template<> struct VkStructTraits< VkMutableDescriptorTypeCreateInfoVALVE >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_MUTABLE_DESCRIPTOR_TYPE_CREATE_INFO_VALVE; };
#endif
#if VK_EXT_vertex_input_dynamic_state
	template<> struct VkStructTraits< VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_INPUT_DYNAMIC_STATE_FEATURES_EXT; };
	template<> struct VkStructTraits< VkVertexInputBindingDescription2EXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_VERTEX_INPUT_BINDING_DESCRIPTION_2_EXT; };
	template<> struct VkStructTraits< VkVertexInputAttributeDescription2EXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_2_EXT; };
#endif
#if VK_EXT_physical_device_drm
	template<> struct VkStructTraits< VkPhysicalDeviceDrmPropertiesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRM_PROPERTIES_EXT; };
#endif
#if VK_EXT_primitive_topology_list_restart
	template<> struct VkStructTraits< VkPhysicalDevicePrimitiveTopologyListRestartFeaturesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRIMITIVE_TOPOLOGY_LIST_RESTART_FEATURES_EXT; };
#endif
#if VK_KHR_format_feature_flags2
	template<> struct VkStructTraits< VkFormatProperties3KHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_3_KHR; };
#endif
#if VK_FUCHSIA_external_memory
	template<> struct VkStructTraits< VkImportMemoryZirconHandleInfoFUCHSIA >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IMPORT_MEMORY_ZIRCON_HANDLE_INFO_FUCHSIA; };
	template<> struct VkStructTraits< VkMemoryZirconHandlePropertiesFUCHSIA >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_MEMORY_ZIRCON_HANDLE_PROPERTIES_FUCHSIA; };
	template<> struct VkStructTraits< VkMemoryGetZirconHandleInfoFUCHSIA >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_MEMORY_GET_ZIRCON_HANDLE_INFO_FUCHSIA; };
#endif
#if VK_FUCHSIA_external_semaphore
	template<> struct VkStructTraits< VkImportSemaphoreZirconHandleInfoFUCHSIA >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IMPORT_SEMAPHORE_ZIRCON_HANDLE_INFO_FUCHSIA; };
	template<> struct VkStructTraits< VkSemaphoreGetZirconHandleInfoFUCHSIA >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SEMAPHORE_GET_ZIRCON_HANDLE_INFO_FUCHSIA; };
#endif
#if VK_FUCHSIA_external_semaphore
	template<> struct VkStructTraits< VkBufferCollectionCreateInfoFUCHSIA >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_BUFFER_COLLECTION_CREATE_INFO_FUCHSIA; };
	template<> struct VkStructTraits< VkImportMemoryBufferCollectionFUCHSIA >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IMPORT_MEMORY_BUFFER_COLLECTION_FUCHSIA; };
	template<> struct VkStructTraits< VkBufferCollectionImageCreateInfoFUCHSIA >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_BUFFER_COLLECTION_IMAGE_CREATE_INFO_FUCHSIA; };
	template<> struct VkStructTraits< VkBufferCollectionPropertiesFUCHSIA >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_BUFFER_COLLECTION_PROPERTIES_FUCHSIA; };
	template<> struct VkStructTraits< VkBufferConstraintsInfoFUCHSIA >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_BUFFER_CONSTRAINTS_INFO_FUCHSIA; };
	template<> struct VkStructTraits< VkBufferCollectionBufferCreateInfoFUCHSIA >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_BUFFER_COLLECTION_BUFFER_CREATE_INFO_FUCHSIA; };
	template<> struct VkStructTraits< VkImageConstraintsInfoFUCHSIA >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IMAGE_CONSTRAINTS_INFO_FUCHSIA; };
	template<> struct VkStructTraits< VkImageFormatConstraintsInfoFUCHSIA >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IMAGE_FORMAT_CONSTRAINTS_INFO_FUCHSIA; };
	template<> struct VkStructTraits< VkSysmemColorSpaceFUCHSIA >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SYSMEM_COLOR_SPACE_FUCHSIA; };
	template<> struct VkStructTraits< VkBufferCollectionConstraintsInfoFUCHSIA >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_BUFFER_COLLECTION_CONSTRAINTS_INFO_FUCHSIA; };
#endif
#if VK_HUAWEI_subpass_shading
	template<> struct VkStructTraits< VkSubpassShadingPipelineCreateInfoHUAWEI >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SUBPASS_SHADING_PIPELINE_CREATE_INFO_HUAWEI; };
	template<> struct VkStructTraits< VkPhysicalDeviceSubpassShadingFeaturesHUAWEI >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBPASS_SHADING_FEATURES_HUAWEI; };
	template<> struct VkStructTraits< VkPhysicalDeviceSubpassShadingPropertiesHUAWEI >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBPASS_SHADING_PROPERTIES_HUAWEI; };
#endif
#if VK_HUAWEI_invocation_mask
	template<> struct VkStructTraits< VkPhysicalDeviceInvocationMaskFeaturesHUAWEI >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INVOCATION_MASK_FEATURES_HUAWEI; };
#endif
#if VK_NV_external_memory_rdma
	template<> struct VkStructTraits< VkMemoryGetRemoteAddressInfoNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_MEMORY_GET_REMOTE_ADDRESS_INFO_NV; };
	template<> struct VkStructTraits< VkPhysicalDeviceExternalMemoryRDMAFeaturesNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_MEMORY_RDMA_FEATURES_NV; };
#endif
#if VK_EXT_extended_dynamic_state2
	template<> struct VkStructTraits< VkPhysicalDeviceExtendedDynamicState2FeaturesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_2_FEATURES_EXT; };
#endif
#if VK_QNX_screen_surface
	template<> struct VkStructTraits< VkScreenSurfaceCreateInfoQNX >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SCREEN_SURFACE_CREATE_INFO_QNX; };
#endif
#if VK_EXT_color_write_enable
	template<> struct VkStructTraits< VkPhysicalDeviceColorWriteEnableFeaturesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COLOR_WRITE_ENABLE_FEATURES_EXT; };
	template<> struct VkStructTraits< VkPipelineColorWriteCreateInfoEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_COLOR_WRITE_CREATE_INFO_EXT; };
#endif
#if VK_EXT_global_priority_query
	template<> struct VkStructTraits< VkPhysicalDeviceGlobalPriorityQueryFeaturesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GLOBAL_PRIORITY_QUERY_FEATURES_EXT; };
	template<> struct VkStructTraits< VkQueueFamilyGlobalPriorityPropertiesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_QUEUE_FAMILY_GLOBAL_PRIORITY_PROPERTIES_EXT; };
#endif
#if VK_EXT_multi_draw
	template<> struct VkStructTraits< VkPhysicalDeviceMultiDrawFeaturesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTI_DRAW_FEATURES_EXT; };
	template<> struct VkStructTraits< VkPhysicalDeviceMultiDrawPropertiesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTI_DRAW_PROPERTIES_EXT; };
#endif
#if VK_EXT_border_color_swizzle
	template<> struct VkStructTraits< VkPhysicalDeviceBorderColorSwizzleFeaturesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BORDER_COLOR_SWIZZLE_FEATURES_EXT; };
	template<> struct VkStructTraits< VkSamplerBorderColorComponentMappingCreateInfoEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SAMPLER_BORDER_COLOR_COMPONENT_MAPPING_CREATE_INFO_EXT; };
#endif
#if VK_KHR_maintenance4
	template<> struct VkStructTraits< VkPhysicalDeviceMaintenance4FeaturesKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_4_FEATURES_KHR; };
	template<> struct VkStructTraits< VkPhysicalDeviceMaintenance4PropertiesKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_4_PROPERTIES_KHR; };
	template<> struct VkStructTraits< VkDeviceBufferMemoryRequirementsKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEVICE_BUFFER_MEMORY_REQUIREMENTS_KHR; };
	template<> struct VkStructTraits< VkDeviceImageMemoryRequirementsKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEVICE_IMAGE_MEMORY_REQUIREMENTS_KHR; };
#endif
#if VK_EXT_pageable_device_local_memory
	template<> struct VkStructTraits< VkPhysicalDevicePageableDeviceLocalMemoryFeaturesEXT >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PAGEABLE_DEVICE_LOCAL_MEMORY_FEATURES_EXT; };
#endif
#if VK_KHR_acceleration_structure
	template<> struct VkStructTraits< VkAccelerationStructureGeometryTrianglesDataKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR; };
	template<> struct VkStructTraits< VkAccelerationStructureGeometryAabbsDataKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_AABBS_DATA_KHR; };
	template<> struct VkStructTraits< VkAccelerationStructureGeometryInstancesDataKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR; };
	template<> struct VkStructTraits< VkAccelerationStructureGeometryKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR; };
	template<> struct VkStructTraits< VkAccelerationStructureBuildGeometryInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR; };
	template<> struct VkStructTraits< VkAccelerationStructureCreateInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR; };
	template<> struct VkStructTraits< VkWriteDescriptorSetAccelerationStructureKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR; };
	template<> struct VkStructTraits< VkPhysicalDeviceAccelerationStructureFeaturesKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR; };
	template<> struct VkStructTraits< VkPhysicalDeviceAccelerationStructurePropertiesKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_PROPERTIES_KHR; };
	template<> struct VkStructTraits< VkAccelerationStructureDeviceAddressInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR; };
	template<> struct VkStructTraits< VkAccelerationStructureVersionInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_VERSION_INFO_KHR; };
	template<> struct VkStructTraits< VkCopyAccelerationStructureToMemoryInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_COPY_ACCELERATION_STRUCTURE_TO_MEMORY_INFO_KHR; };
	template<> struct VkStructTraits< VkCopyMemoryToAccelerationStructureInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_COPY_MEMORY_TO_ACCELERATION_STRUCTURE_INFO_KHR; };
	template<> struct VkStructTraits< VkCopyAccelerationStructureInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_COPY_ACCELERATION_STRUCTURE_INFO_KHR; };
	template<> struct VkStructTraits< VkAccelerationStructureBuildSizesInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR; };
#endif
#if VK_KHR_ray_tracing_pipeline
	template<> struct VkStructTraits< VkRayTracingShaderGroupCreateInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR; };
	template<> struct VkStructTraits< VkRayTracingPipelineInterfaceCreateInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_INTERFACE_CREATE_INFO_KHR; };
	template<> struct VkStructTraits< VkRayTracingPipelineCreateInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR; };
	template<> struct VkStructTraits< VkPhysicalDeviceRayTracingPipelineFeaturesKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR; };
	template<> struct VkStructTraits< VkPhysicalDeviceRayTracingPipelinePropertiesKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR; };
#endif
#if VK_KHR_ray_query
	template<> struct VkStructTraits< VkPhysicalDeviceRayQueryFeaturesKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR; };
#endif
#if VK_KHR_video_encode_queue
	template<> struct VkStructTraits< VkVideoEncodeInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_VIDEO_ENCODE_INFO_KHR; };
	template<> struct VkStructTraits< VkVideoEncodeRateControlInfoKHR >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_VIDEO_ENCODE_RATE_CONTROL_INFO_KHR; };
#endif

	template< typename VkStructT, typename ... ParamsT >
	inline VkStructT makeVkStructPNext( void * next
		, ParamsT && ... params )
	{
		return VkStructT{ VkStructTraits< VkStructT >::value
			, next
			, std::forward< ParamsT >( params )... };
	}

	template< typename VkStructT, typename ... ParamsT >
	inline VkStructT makeVkStruct( ParamsT && ... params )
	{
		return VkStructT{ VkStructTraits< VkStructT >::value
			, nullptr
			, std::forward< ParamsT >( params )... };
	}
}

#endif
