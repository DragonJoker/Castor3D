/*
See LICENSE file in root folder
*/
#ifndef ___C3D_makeVkType_H___
#define ___C3D_makeVkType_H___

#include "MiscellaneousModule.hpp"

namespace castor3d
{
	template< typename AshesType >
	struct VkTypeTraits;
	
	template<> struct VkTypeTraits< VkApplicationInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_APPLICATION_INFO; };
	template<> struct VkTypeTraits< VkInstanceCreateInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO; };
	template<> struct VkTypeTraits< VkDeviceQueueCreateInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO; };
	template<> struct VkTypeTraits< VkDeviceCreateInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO; };
	template<> struct VkTypeTraits< VkSubmitInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SUBMIT_INFO; };
	template<> struct VkTypeTraits< VkMemoryAllocateInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO; };
	template<> struct VkTypeTraits< VkMappedMemoryRange>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE; };
	template<> struct VkTypeTraits< VkBindSparseInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_BIND_SPARSE_INFO; };
	template<> struct VkTypeTraits< VkFenceCreateInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO; };
	template<> struct VkTypeTraits< VkSemaphoreCreateInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO; };
	template<> struct VkTypeTraits< VkEventCreateInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_EVENT_CREATE_INFO; };
	template<> struct VkTypeTraits< VkQueryPoolCreateInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO; };
	template<> struct VkTypeTraits< VkBufferCreateInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO; };
	template<> struct VkTypeTraits< VkBufferViewCreateInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO; };
	template<> struct VkTypeTraits< VkImageCreateInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO; };
	template<> struct VkTypeTraits< VkImageViewCreateInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO; };
	template<> struct VkTypeTraits< VkShaderModuleCreateInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO; };
	template<> struct VkTypeTraits< VkPipelineCacheCreateInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO; };
	template<> struct VkTypeTraits< VkPipelineShaderStageCreateInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO; };
	template<> struct VkTypeTraits< VkPipelineVertexInputStateCreateInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO; };
	template<> struct VkTypeTraits< VkPipelineInputAssemblyStateCreateInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO; };
	template<> struct VkTypeTraits< VkPipelineTessellationStateCreateInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO; };
	template<> struct VkTypeTraits< VkPipelineViewportStateCreateInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO; };
	template<> struct VkTypeTraits< VkPipelineRasterizationStateCreateInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO; };
	template<> struct VkTypeTraits< VkPipelineMultisampleStateCreateInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO; };
	template<> struct VkTypeTraits< VkPipelineDepthStencilStateCreateInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO; };
	template<> struct VkTypeTraits< VkPipelineColorBlendStateCreateInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO; };
	template<> struct VkTypeTraits< VkPipelineDynamicStateCreateInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO; };
	template<> struct VkTypeTraits< VkGraphicsPipelineCreateInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO; };
	template<> struct VkTypeTraits< VkComputePipelineCreateInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO; };
	template<> struct VkTypeTraits< VkPipelineLayoutCreateInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO; };
	template<> struct VkTypeTraits< VkSamplerCreateInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO; };
	template<> struct VkTypeTraits< VkDescriptorSetLayoutCreateInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO; };
	template<> struct VkTypeTraits< VkDescriptorPoolCreateInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO; };
	template<> struct VkTypeTraits< VkDescriptorSetAllocateInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO; };
	template<> struct VkTypeTraits< VkWriteDescriptorSet>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET; };
	template<> struct VkTypeTraits< VkCopyDescriptorSet>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET; };
	template<> struct VkTypeTraits< VkFramebufferCreateInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO; };
	template<> struct VkTypeTraits< VkRenderPassCreateInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO; };
	template<> struct VkTypeTraits< VkCommandPoolCreateInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO; };
	template<> struct VkTypeTraits< VkCommandBufferAllocateInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO; };
	template<> struct VkTypeTraits< VkCommandBufferInheritanceInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO; };
	template<> struct VkTypeTraits< VkCommandBufferBeginInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO; };
	template<> struct VkTypeTraits< VkRenderPassBeginInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO; };
	template<> struct VkTypeTraits< VkBufferMemoryBarrier>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER; };
	template<> struct VkTypeTraits< VkImageMemoryBarrier>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER; };
	template<> struct VkTypeTraits< VkMemoryBarrier>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_MEMORY_BARRIER; };
	template<> struct VkTypeTraits< VkPhysicalDeviceSubgroupProperties>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_PROPERTIES; };
	template<> struct VkTypeTraits< VkBindBufferMemoryInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_BIND_BUFFER_MEMORY_INFO; };
	template<> struct VkTypeTraits< VkBindImageMemoryInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_INFO; };
	template<> struct VkTypeTraits< VkPhysicalDevice16BitStorageFeatures>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES; };
	template<> struct VkTypeTraits< VkMemoryDedicatedRequirements>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_MEMORY_DEDICATED_REQUIREMENTS; };
	template<> struct VkTypeTraits< VkMemoryDedicatedAllocateInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_MEMORY_DEDICATED_ALLOCATE_INFO; };
	template<> struct VkTypeTraits< VkMemoryAllocateFlagsInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO; };
	template<> struct VkTypeTraits< VkDeviceGroupRenderPassBeginInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEVICE_GROUP_RENDER_PASS_BEGIN_INFO; };
	template<> struct VkTypeTraits< VkDeviceGroupCommandBufferBeginInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEVICE_GROUP_COMMAND_BUFFER_BEGIN_INFO; };
	template<> struct VkTypeTraits< VkDeviceGroupSubmitInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEVICE_GROUP_SUBMIT_INFO; };
	template<> struct VkTypeTraits< VkDeviceGroupBindSparseInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEVICE_GROUP_BIND_SPARSE_INFO; };
	template<> struct VkTypeTraits< VkBindBufferMemoryDeviceGroupInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_BIND_BUFFER_MEMORY_DEVICE_GROUP_INFO; };
	template<> struct VkTypeTraits< VkBindImageMemoryDeviceGroupInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_DEVICE_GROUP_INFO; };
	template<> struct VkTypeTraits< VkPhysicalDeviceGroupProperties>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES; };
	template<> struct VkTypeTraits< VkDeviceGroupDeviceCreateInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEVICE_GROUP_DEVICE_CREATE_INFO; };
	template<> struct VkTypeTraits< VkBufferMemoryRequirementsInfo2>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_BUFFER_MEMORY_REQUIREMENTS_INFO_2; };
	template<> struct VkTypeTraits< VkImageMemoryRequirementsInfo2>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IMAGE_MEMORY_REQUIREMENTS_INFO_2; };
	template<> struct VkTypeTraits< VkImageSparseMemoryRequirementsInfo2>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IMAGE_SPARSE_MEMORY_REQUIREMENTS_INFO_2; };
	template<> struct VkTypeTraits< VkMemoryRequirements2>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2; };
	template<> struct VkTypeTraits< VkSparseImageMemoryRequirements2>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SPARSE_IMAGE_MEMORY_REQUIREMENTS_2; };
	template<> struct VkTypeTraits< VkPhysicalDeviceFeatures2>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2; };
	template<> struct VkTypeTraits< VkPhysicalDeviceProperties2>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2; };
	template<> struct VkTypeTraits< VkFormatProperties2>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2; };
	template<> struct VkTypeTraits< VkImageFormatProperties2>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IMAGE_FORMAT_PROPERTIES_2; };
	template<> struct VkTypeTraits< VkPhysicalDeviceImageFormatInfo2>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_FORMAT_INFO_2; };
	template<> struct VkTypeTraits< VkQueueFamilyProperties2>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2; };
	template<> struct VkTypeTraits< VkPhysicalDeviceMemoryProperties2>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2; };
	template<> struct VkTypeTraits< VkSparseImageFormatProperties2>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SPARSE_IMAGE_FORMAT_PROPERTIES_2; };
	template<> struct VkTypeTraits< VkPhysicalDeviceSparseImageFormatInfo2>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SPARSE_IMAGE_FORMAT_INFO_2; };
	template<> struct VkTypeTraits< VkPhysicalDevicePointClippingProperties>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_POINT_CLIPPING_PROPERTIES; };
	template<> struct VkTypeTraits< VkRenderPassInputAttachmentAspectCreateInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_RENDER_PASS_INPUT_ATTACHMENT_ASPECT_CREATE_INFO; };
	template<> struct VkTypeTraits< VkImageViewUsageCreateInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IMAGE_VIEW_USAGE_CREATE_INFO; };
	template<> struct VkTypeTraits< VkPipelineTessellationDomainOriginStateCreateInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_DOMAIN_ORIGIN_STATE_CREATE_INFO; };
	template<> struct VkTypeTraits< VkRenderPassMultiviewCreateInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_RENDER_PASS_MULTIVIEW_CREATE_INFO; };
	template<> struct VkTypeTraits< VkPhysicalDeviceMultiviewFeatures>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES; };
	template<> struct VkTypeTraits< VkPhysicalDeviceMultiviewProperties>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PROPERTIES; };
	template<> struct VkTypeTraits< VkPhysicalDeviceVariablePointerFeatures>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTER_FEATURES; };
	template<> struct VkTypeTraits< VkProtectedSubmitInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PROTECTED_SUBMIT_INFO; };
	template<> struct VkTypeTraits< VkPhysicalDeviceProtectedMemoryFeatures>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROTECTED_MEMORY_FEATURES; };
	template<> struct VkTypeTraits< VkPhysicalDeviceProtectedMemoryProperties>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROTECTED_MEMORY_PROPERTIES; };
	template<> struct VkTypeTraits< VkDeviceQueueInfo2>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEVICE_QUEUE_INFO_2; };
	template<> struct VkTypeTraits< VkSamplerYcbcrConversionCreateInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_CREATE_INFO; };
	template<> struct VkTypeTraits< VkSamplerYcbcrConversionInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_INFO; };
	template<> struct VkTypeTraits< VkBindImagePlaneMemoryInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_BIND_IMAGE_PLANE_MEMORY_INFO; };
	template<> struct VkTypeTraits< VkImagePlaneMemoryRequirementsInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IMAGE_PLANE_MEMORY_REQUIREMENTS_INFO; };
	template<> struct VkTypeTraits< VkPhysicalDeviceSamplerYcbcrConversionFeatures>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES; };
	template<> struct VkTypeTraits< VkSamplerYcbcrConversionImageFormatProperties>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_IMAGE_FORMAT_PROPERTIES; };
	template<> struct VkTypeTraits< VkDescriptorUpdateTemplateCreateInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DESCRIPTOR_UPDATE_TEMPLATE_CREATE_INFO; };
	template<> struct VkTypeTraits< VkPhysicalDeviceExternalImageFormatInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_IMAGE_FORMAT_INFO; };
	template<> struct VkTypeTraits< VkExternalImageFormatProperties>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_EXTERNAL_IMAGE_FORMAT_PROPERTIES; };
	template<> struct VkTypeTraits< VkPhysicalDeviceExternalBufferInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_BUFFER_INFO; };
	template<> struct VkTypeTraits< VkExternalBufferProperties>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_EXTERNAL_BUFFER_PROPERTIES; };
	template<> struct VkTypeTraits< VkPhysicalDeviceIDProperties>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ID_PROPERTIES; };
	template<> struct VkTypeTraits< VkExternalMemoryBufferCreateInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_BUFFER_CREATE_INFO; };
	template<> struct VkTypeTraits< VkExternalMemoryImageCreateInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMAGE_CREATE_INFO; };
	template<> struct VkTypeTraits< VkExportMemoryAllocateInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_EXPORT_MEMORY_ALLOCATE_INFO; };
	template<> struct VkTypeTraits< VkPhysicalDeviceExternalFenceInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_FENCE_INFO; };
	template<> struct VkTypeTraits< VkExternalFenceProperties>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_EXTERNAL_FENCE_PROPERTIES; };
	template<> struct VkTypeTraits< VkExportFenceCreateInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_EXPORT_FENCE_CREATE_INFO; };
	template<> struct VkTypeTraits< VkExportSemaphoreCreateInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_EXPORT_SEMAPHORE_CREATE_INFO; };
	template<> struct VkTypeTraits< VkPhysicalDeviceExternalSemaphoreInfo>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_SEMAPHORE_INFO; };
	template<> struct VkTypeTraits< VkExternalSemaphoreProperties>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_EXTERNAL_SEMAPHORE_PROPERTIES; };
	template<> struct VkTypeTraits< VkPhysicalDeviceMaintenance3Properties>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_3_PROPERTIES; };
	template<> struct VkTypeTraits< VkDescriptorSetLayoutSupport>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_SUPPORT; };
	template<> struct VkTypeTraits< VkPhysicalDeviceShaderDrawParameterFeatures>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETER_FEATURES; };
	template<> struct VkTypeTraits< VkSwapchainCreateInfoKHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR; };
	template<> struct VkTypeTraits< VkPresentInfoKHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR; };
	template<> struct VkTypeTraits< VkDeviceGroupPresentCapabilitiesKHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEVICE_GROUP_PRESENT_CAPABILITIES_KHR; };
	template<> struct VkTypeTraits< VkImageSwapchainCreateInfoKHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IMAGE_SWAPCHAIN_CREATE_INFO_KHR; };
	template<> struct VkTypeTraits< VkBindImageMemorySwapchainInfoKHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_SWAPCHAIN_INFO_KHR; };
	template<> struct VkTypeTraits< VkAcquireNextImageInfoKHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_ACQUIRE_NEXT_IMAGE_INFO_KHR; };
	template<> struct VkTypeTraits< VkDeviceGroupPresentInfoKHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEVICE_GROUP_PRESENT_INFO_KHR; };
	template<> struct VkTypeTraits< VkDeviceGroupSwapchainCreateInfoKHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEVICE_GROUP_SWAPCHAIN_CREATE_INFO_KHR; };
	template<> struct VkTypeTraits< VkDisplayModeCreateInfoKHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DISPLAY_MODE_CREATE_INFO_KHR; };
	template<> struct VkTypeTraits< VkDisplaySurfaceCreateInfoKHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DISPLAY_SURFACE_CREATE_INFO_KHR; };
	template<> struct VkTypeTraits< VkDisplayPresentInfoKHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DISPLAY_PRESENT_INFO_KHR; };

#ifdef VK_USE_PLATFORM_ANDROID_KHR
	template<> struct VkTypeTraits< VkAndroidSurfaceCreateInfoKHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR; };
	template<> struct VkTypeTraits< VkAndroidHardwareBufferUsageAndroid>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_ANDROID_HARDWARE_BUFFER_USAGE_ANDROID; };
	template<> struct VkTypeTraits< VkAndroidHardwareBufferPropertiesAndroid>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_ANDROID_HARDWARE_BUFFER_PROPERTIES_ANDROID; };
	template<> struct VkTypeTraits< VkAndroidHardwareBufferFormatPropertiesAndroid>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_ANDROID_HARDWARE_BUFFER_FORMAT_PROPERTIES_ANDROID; };
	template<> struct VkTypeTraits< VkImportAndroidHardwareBufferInfoAndroid>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IMPORT_ANDROID_HARDWARE_BUFFER_INFO_ANDROID; };
	template<> struct VkTypeTraits< VkMemoryGetAndroidHardwareBufferInfoAndroid>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_MEMORY_GET_ANDROID_HARDWARE_BUFFER_INFO_ANDROID; };
	template<> struct VkTypeTraits< VkExternalFormatAndroid>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_EXTERNAL_FORMAT_ANDROID; };
#endif
#ifdef VK_USE_PLATFORM_FUCHSIA
	template<> struct VkTypeTraits< VkImagePipeSurfaceCreateInfoFuchsia>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IMAGEPIPE_SURFACE_CREATE_INFO_FUCHSIA; };
#endif
#ifdef VK_USE_PLATFORM_IOS_MVK
	template<> struct VkTypeTraits< VkIOSSurfaceCreateInfoMVK>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IOS_SURFACE_CREATE_INFO_MVK; };
#endif
#ifdef VK_USE_PLATFORM_MACOS_MVK
	template<> struct VkTypeTraits< VkMacOSSurfaceCreateInfoMVK>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK; };
#endif
#ifdef VK_USE_PLATFORM_VI_NN
	template<> struct VkTypeTraits< VkViSurfaceCreateInfoNN>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_VI_SURFACE_CREATE_INFO_NN; };
#endif
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
	template<> struct VkTypeTraits< VkWaylandSurfaceCreateInfoKHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR; };
#endif
#ifdef VK_USE_PLATFORM_WIN32_KHR
	template<> struct VkTypeTraits< VkWin32SurfaceCreateInfoKHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR; };
	template<> struct VkTypeTraits< VkImportMemoryWin32HandleInfoNV>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IMPORT_MEMORY_WIN32_HANDLE_INFO_NV; };
	template<> struct VkTypeTraits< VkExportMemoryWin32HandleInfoNV>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_EXPORT_MEMORY_WIN32_HANDLE_INFO_NV; };
#endif
#if VK_KHR_external_memory_win32
	template<> struct VkTypeTraits< VkImportMemoryWin32HandleInfoKHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IMPORT_MEMORY_WIN32_HANDLE_INFO_KHR; };
	template<> struct VkTypeTraits< VkExportMemoryWin32HandleInfoKHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_EXPORT_MEMORY_WIN32_HANDLE_INFO_KHR; };
	template<> struct VkTypeTraits< VkMemoryWin32HandlePropertiesKHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_MEMORY_WIN32_HANDLE_PROPERTIES_KHR; };
	template<> struct VkTypeTraits< VkMemoryGetWin32HandleInfoKHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_MEMORY_GET_WIN32_HANDLE_INFO_KHR; };
#endif
#if VK_KHR_win32_keyed_mutex
	template<> struct VkTypeTraits< VkWin32KeyedMutexAcquireReleaseInfoKHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_WIN32_KEYED_MUTEX_ACQUIRE_RELEASE_INFO_KHR; };
#endif
#if VK_NV_win32_keyed_mutex
	template<> struct VkTypeTraits< VkWin32KeyedMutexAcquireReleaseInfoNV>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_WIN32_KEYED_MUTEX_ACQUIRE_RELEASE_INFO_NV; };
#endif
#if VK_KHR_external_semaphore_win32
	template<> struct VkTypeTraits< VkImportSemaphoreWin32HandleInfoKHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IMPORT_SEMAPHORE_WIN32_HANDLE_INFO_KHR; };
	template<> struct VkTypeTraits< VkExportSemaphoreWin32HandleInfoKHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_EXPORT_SEMAPHORE_WIN32_HANDLE_INFO_KHR; };
	template<> struct VkTypeTraits< VkD3D12FenceSubmitInfoKHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_D3D12_FENCE_SUBMIT_INFO_KHR; };
	template<> struct VkTypeTraits< VkSemaphoreGetWin32HandleInfoKHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SEMAPHORE_GET_WIN32_HANDLE_INFO_KHR; };
#endif
#if VK_KHR_external_fence_win32
	template<> struct VkTypeTraits< VkImportFenceWin32HandleInfoKHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IMPORT_FENCE_WIN32_HANDLE_INFO_KHR; };
	template<> struct VkTypeTraits< VkExportFenceWin32HandleInfoKHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_EXPORT_FENCE_WIN32_HANDLE_INFO_KHR; };
	template<> struct VkTypeTraits< VkFenceGetWin32HandleInfoKHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_FENCE_GET_WIN32_HANDLE_INFO_KHR; };
#endif
#ifdef VK_USE_PLATFORM_XCB_KHR
	template<> struct VkTypeTraits< VkXcbSurfaceCreateInfoKHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR; };
#endif
#ifdef VK_USE_PLATFORM_XLIB_KHR
	template<> struct VkTypeTraits< VkXlibSurfaceCreateInfoKHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR; };
#endif
#ifdef VK_USE_PLATFORM_XLIB_XRANDR_EXT
	template<> struct VkTypeTraits< VkXlibSurfaceCreateInfoKHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR; };
#endif

	template<> struct VkTypeTraits< VkDebugReportCallbackCreateInfoEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT; };
	template<> struct VkTypeTraits< VkPipelineRasterizationStateRasterizationOrderAMD>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_RASTERIZATION_ORDER_AMD; };
	template<> struct VkTypeTraits< VkDebugMarkerObjectNameInfoEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEBUG_MARKER_OBJECT_NAME_INFO_EXT; };
	template<> struct VkTypeTraits< VkDebugMarkerObjectTagInfoEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEBUG_MARKER_OBJECT_TAG_INFO_EXT; };
	template<> struct VkTypeTraits< VkDebugMarkerMarkerInfoEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT; };
	template<> struct VkTypeTraits< VkDedicatedAllocationImageCreateInfoNV>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEDICATED_ALLOCATION_IMAGE_CREATE_INFO_NV; };
	template<> struct VkTypeTraits< VkDedicatedAllocationBufferCreateInfoNV>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEDICATED_ALLOCATION_BUFFER_CREATE_INFO_NV; };
	template<> struct VkTypeTraits< VkDedicatedAllocationMemoryAllocateInfoNV>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEDICATED_ALLOCATION_MEMORY_ALLOCATE_INFO_NV; };
	template<> struct VkTypeTraits< VkPhysicalDeviceTransformFeedbackFeaturesEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TRANSFORM_FEEDBACK_FEATURES_EXT; };
	template<> struct VkTypeTraits< VkPhysicalDeviceTransformFeedbackPropertiesEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TRANSFORM_FEEDBACK_PROPERTIES_EXT; };
	template<> struct VkTypeTraits< VkPipelineRasterizationStateStreamCreateInfoEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_STREAM_CREATE_INFO_EXT; };
	template<> struct VkTypeTraits< VkTextureLODGatherFormatPropertiesAMD>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_TEXTURE_LOD_GATHER_FORMAT_PROPERTIES_AMD; };
	template<> struct VkTypeTraits< VkPhysicalDeviceCornerSampledImageFeaturesNV>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CORNER_SAMPLED_IMAGE_FEATURES_NV; };
	template<> struct VkTypeTraits< VkExternalMemoryImageCreateInfoNV>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMAGE_CREATE_INFO_NV; };
	template<> struct VkTypeTraits< VkExportMemoryAllocateInfoNV>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_EXPORT_MEMORY_ALLOCATE_INFO_NV; };
	template<> struct VkTypeTraits< VkValidationFlagsEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_VALIDATION_FLAGS_EXT; };
	template<> struct VkTypeTraits< VkImageViewASTCDecodeModeEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IMAGE_VIEW_ASTC_DECODE_MODE_EXT; };
	template<> struct VkTypeTraits< VkPhysicalDeviceASTCDecodeFeaturesEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ASTC_DECODE_FEATURES_EXT; };
	template<> struct VkTypeTraits< VkImportMemoryFdInfoKHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IMPORT_MEMORY_FD_INFO_KHR; };
	template<> struct VkTypeTraits< VkMemoryFdPropertiesKHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_MEMORY_FD_PROPERTIES_KHR; };
	template<> struct VkTypeTraits< VkMemoryGetFdInfoKHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_MEMORY_GET_FD_INFO_KHR; };
	template<> struct VkTypeTraits< VkImportSemaphoreFdInfoKHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IMPORT_SEMAPHORE_FD_INFO_KHR; };
	template<> struct VkTypeTraits< VkSemaphoreGetFdInfoKHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SEMAPHORE_GET_FD_INFO_KHR; };
	template<> struct VkTypeTraits< VkPhysicalDevicePushDescriptorPropertiesKHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PUSH_DESCRIPTOR_PROPERTIES_KHR; };
	template<> struct VkTypeTraits< VkCommandBufferInheritanceConditionalRenderingInfoEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_CONDITIONAL_RENDERING_INFO_EXT; };
	template<> struct VkTypeTraits< VkPhysicalDeviceConditionalRenderingFeaturesEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CONDITIONAL_RENDERING_FEATURES_EXT; };
	template<> struct VkTypeTraits< VkConditionalRenderingBeginInfoEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_CONDITIONAL_RENDERING_BEGIN_INFO_EXT; };
	template<> struct VkTypeTraits< VkPhysicalDeviceFloat16Int8FeaturesKHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FLOAT16_INT8_FEATURES_KHR; };
	template<> struct VkTypeTraits< VkPresentRegionsKHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PRESENT_REGIONS_KHR; };
	template<> struct VkTypeTraits< VkObjectTableCreateInfoNVX>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_OBJECT_TABLE_CREATE_INFO_NVX; };
	template<> struct VkTypeTraits< VkIndirectCommandsLayoutCreateInfoNVX>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_INDIRECT_COMMANDS_LAYOUT_CREATE_INFO_NVX; };
	template<> struct VkTypeTraits< VkCmdProcessCommandsInfoNVX>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_CMD_PROCESS_COMMANDS_INFO_NVX; };
	template<> struct VkTypeTraits< VkCmdReserveSpaceForCommandsInfoNVX>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_CMD_RESERVE_SPACE_FOR_COMMANDS_INFO_NVX; };
	template<> struct VkTypeTraits< VkDeviceGeneratedCommandsLimitsNVX>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEVICE_GENERATED_COMMANDS_LIMITS_NVX; };
	template<> struct VkTypeTraits< VkDeviceGeneratedCommandsFeaturesNVX>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEVICE_GENERATED_COMMANDS_FEATURES_NVX; };
	template<> struct VkTypeTraits< VkPipelineViewportWScalingStateCreateInfoNV>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_W_SCALING_STATE_CREATE_INFO_NV; };
	template<> struct VkTypeTraits< VkSurfaceCapabilities2EXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SURFACE_CAPABILITIES_2_EXT; };
	template<> struct VkTypeTraits< VkDisplayPowerInfoEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DISPLAY_POWER_INFO_EXT; };
	template<> struct VkTypeTraits< VkDeviceEventInfoEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEVICE_EVENT_INFO_EXT; };
	template<> struct VkTypeTraits< VkDisplayEventInfoEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DISPLAY_EVENT_INFO_EXT; };
	template<> struct VkTypeTraits< VkSwapchainCounterCreateInfoEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SWAPCHAIN_COUNTER_CREATE_INFO_EXT; };
	template<> struct VkTypeTraits< VkPresentTimesInfoGOOGLE>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PRESENT_TIMES_INFO_GOOGLE; };
	template<> struct VkTypeTraits< VkPhysicalDeviceMultiviewPerViewAttributesPropertiesNVX>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PER_VIEW_ATTRIBUTES_PROPERTIES_NVX; };
	template<> struct VkTypeTraits< VkPipelineViewportSwizzleStateCreateInfoNV>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_SWIZZLE_STATE_CREATE_INFO_NV; };
	template<> struct VkTypeTraits< VkPhysicalDeviceDiscardRectanglePropertiesEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DISCARD_RECTANGLE_PROPERTIES_EXT; };
	template<> struct VkTypeTraits< VkPipelineDiscardRectangleStateCreateInfoEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_DISCARD_RECTANGLE_STATE_CREATE_INFO_EXT; };
	template<> struct VkTypeTraits< VkPhysicalDeviceConservativeRasterizationPropertiesEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CONSERVATIVE_RASTERIZATION_PROPERTIES_EXT; };
	template<> struct VkTypeTraits< VkPipelineRasterizationConservativeStateCreateInfoEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_CONSERVATIVE_STATE_CREATE_INFO_EXT; };
	template<> struct VkTypeTraits< VkPhysicalDeviceDepthClipEnableFeaturesEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLIP_ENABLE_FEATURES_EXT; };
	template<> struct VkTypeTraits< VkPipelineRasterizationDepthClipStateCreateInfoEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_DEPTH_CLIP_STATE_CREATE_INFO_EXT; };
	template<> struct VkTypeTraits< VkHdrMetadataEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_HDR_METADATA_EXT; };
	template<> struct VkTypeTraits< VkAttachmentDescription2KHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2_KHR; };
	template<> struct VkTypeTraits< VkAttachmentReference2KHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2_KHR; };
	template<> struct VkTypeTraits< VkSubpassDescription2KHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2_KHR; };
	template<> struct VkTypeTraits< VkSubpassDependency2KHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2_KHR; };
	template<> struct VkTypeTraits< VkRenderPassCreateInfo2KHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2_KHR; };
	template<> struct VkTypeTraits< VkSubpassBeginInfoKHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SUBPASS_BEGIN_INFO_KHR; };
	template<> struct VkTypeTraits< VkSubpassEndInfoKHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SUBPASS_END_INFO_KHR; };
	template<> struct VkTypeTraits< VkSharedPresentSurfaceCapabilitiesKHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SHARED_PRESENT_SURFACE_CAPABILITIES_KHR; };
	template<> struct VkTypeTraits< VkImportFenceFdInfoKHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IMPORT_FENCE_FD_INFO_KHR; };
	template<> struct VkTypeTraits< VkFenceGetFdInfoKHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_FENCE_GET_FD_INFO_KHR; };
	template<> struct VkTypeTraits< VkPhysicalDeviceSurfaceInfo2KHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SURFACE_INFO_2_KHR; };
	template<> struct VkTypeTraits< VkSurfaceCapabilities2KHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SURFACE_CAPABILITIES_2_KHR; };
	template<> struct VkTypeTraits< VkSurfaceFormat2KHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SURFACE_FORMAT_2_KHR; };
	template<> struct VkTypeTraits< VkDisplayProperties2KHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DISPLAY_PROPERTIES_2_KHR; };
	template<> struct VkTypeTraits< VkDisplayPlaneProperties2KHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DISPLAY_PLANE_PROPERTIES_2_KHR; };
	template<> struct VkTypeTraits< VkDisplayModeProperties2KHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DISPLAY_MODE_PROPERTIES_2_KHR; };
	template<> struct VkTypeTraits< VkDisplayPlaneInfo2KHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DISPLAY_PLANE_INFO_2_KHR; };
	template<> struct VkTypeTraits< VkDisplayPlaneCapabilities2KHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DISPLAY_PLANE_CAPABILITIES_2_KHR; };
	template<> struct VkTypeTraits< VkDebugUtilsObjectNameInfoEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT; };
	template<> struct VkTypeTraits< VkDebugUtilsObjectTagInfoEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_TAG_INFO_EXT; };
	template<> struct VkTypeTraits< VkDebugUtilsLabelEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT; };
	template<> struct VkTypeTraits< VkDebugUtilsMessengerCallbackDataEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CALLBACK_DATA_EXT; };
	template<> struct VkTypeTraits< VkDebugUtilsMessengerCreateInfoEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT; };
	template<> struct VkTypeTraits< VkPhysicalDeviceSamplerFilterMinmaxPropertiesEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_FILTER_MINMAX_PROPERTIES_EXT; };
	template<> struct VkTypeTraits< VkSamplerReductionModeCreateInfoEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SAMPLER_REDUCTION_MODE_CREATE_INFO_EXT; };
	template<> struct VkTypeTraits< VkPhysicalDeviceInlineUniformBlockFeaturesEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_FEATURES_EXT; };
	template<> struct VkTypeTraits< VkPhysicalDeviceInlineUniformBlockPropertiesEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_PROPERTIES_EXT; };
	template<> struct VkTypeTraits< VkWriteDescriptorSetInlineUniformBlockEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_INLINE_UNIFORM_BLOCK_EXT; };
	template<> struct VkTypeTraits< VkDescriptorPoolInlineUniformBlockCreateInfoEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_INLINE_UNIFORM_BLOCK_CREATE_INFO_EXT; };
	template<> struct VkTypeTraits< VkSampleLocationsInfoEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SAMPLE_LOCATIONS_INFO_EXT; };
	template<> struct VkTypeTraits< VkRenderPassSampleLocationsBeginInfoEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_RENDER_PASS_SAMPLE_LOCATIONS_BEGIN_INFO_EXT; };
	template<> struct VkTypeTraits< VkPipelineSampleLocationsStateCreateInfoEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_SAMPLE_LOCATIONS_STATE_CREATE_INFO_EXT; };
	template<> struct VkTypeTraits< VkPhysicalDeviceSampleLocationsPropertiesEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLE_LOCATIONS_PROPERTIES_EXT; };
	template<> struct VkTypeTraits< VkMultisamplePropertiesEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_MULTISAMPLE_PROPERTIES_EXT; };
	template<> struct VkTypeTraits< VkImageFormatListCreateInfoKHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IMAGE_FORMAT_LIST_CREATE_INFO_KHR; };
	template<> struct VkTypeTraits< VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BLEND_OPERATION_ADVANCED_FEATURES_EXT; };
	template<> struct VkTypeTraits< VkPhysicalDeviceBlendOperationAdvancedPropertiesEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BLEND_OPERATION_ADVANCED_PROPERTIES_EXT; };
	template<> struct VkTypeTraits< VkPipelineColorBlendAdvancedStateCreateInfoEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_ADVANCED_STATE_CREATE_INFO_EXT; };
	template<> struct VkTypeTraits< VkPipelineCoverageToColorStateCreateInfoNV>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_COVERAGE_TO_COLOR_STATE_CREATE_INFO_NV; };
	template<> struct VkTypeTraits< VkPipelineCoverageModulationStateCreateInfoNV>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_COVERAGE_MODULATION_STATE_CREATE_INFO_NV; };
	template<> struct VkTypeTraits< VkDrmFormatModifierPropertiesListEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DRM_FORMAT_MODIFIER_PROPERTIES_LIST_EXT; };
	template<> struct VkTypeTraits< VkDrmFormatModifierPropertiesEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DRM_FORMAT_MODIFIER_PROPERTIES_EXT; };
	template<> struct VkTypeTraits< VkPhysicalDeviceImageDrmFormatModifierInfoEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_DRM_FORMAT_MODIFIER_INFO_EXT; };
	template<> struct VkTypeTraits< VkImageDrmFormatModifierListCreateInfoEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IMAGE_DRM_FORMAT_MODIFIER_LIST_CREATE_INFO_EXT; };
	template<> struct VkTypeTraits< VkImageDrmFormatModifierExplicitCreateInfoEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IMAGE_DRM_FORMAT_MODIFIER_EXPLICIT_CREATE_INFO_EXT; };
	template<> struct VkTypeTraits< VkImageDrmFormatModifierPropertiesEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IMAGE_DRM_FORMAT_MODIFIER_PROPERTIES_EXT; };
	template<> struct VkTypeTraits< VkValidationCacheCreateInfoEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_VALIDATION_CACHE_CREATE_INFO_EXT; };
	template<> struct VkTypeTraits< VkShaderModuleValidationCacheCreateInfoEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SHADER_MODULE_VALIDATION_CACHE_CREATE_INFO_EXT; };
	template<> struct VkTypeTraits< VkDescriptorSetLayoutBindingFlagsCreateInfoEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT; };
	template<> struct VkTypeTraits< VkPhysicalDeviceDescriptorIndexingFeaturesEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT; };
	template<> struct VkTypeTraits< VkPhysicalDeviceDescriptorIndexingPropertiesEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES_EXT; };
	template<> struct VkTypeTraits< VkDescriptorSetVariableDescriptorCountAllocateInfoEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO_EXT; };
	template<> struct VkTypeTraits< VkDescriptorSetVariableDescriptorCountLayoutSupportEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_LAYOUT_SUPPORT_EXT; };
	template<> struct VkTypeTraits< VkPipelineViewportShadingRateImageStateCreateInfoNV>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_SHADING_RATE_IMAGE_STATE_CREATE_INFO_NV; };
	template<> struct VkTypeTraits< VkPhysicalDeviceShadingRateImageFeaturesNV>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADING_RATE_IMAGE_FEATURES_NV; };
	template<> struct VkTypeTraits< VkPhysicalDeviceShadingRateImagePropertiesNV>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADING_RATE_IMAGE_PROPERTIES_NV; };
	template<> struct VkTypeTraits< VkPipelineViewportCoarseSampleOrderStateCreateInfoNV>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_COARSE_SAMPLE_ORDER_STATE_CREATE_INFO_NV; };
	template<> struct VkTypeTraits< VkRayTracingPipelineCreateInfoNV>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_NV; };
	template<> struct VkTypeTraits< VkAccelerationStructureCreateInfoNV>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_NV; };
	template<> struct VkTypeTraits< VkGeometryNV>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_GEOMETRY_NV; };
	template<> struct VkTypeTraits< VkGeometryTrianglesNV >{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_GEOMETRY_TRIANGLES_NV; };
	template<> struct VkTypeTraits< VkGeometryAABBNV>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_GEOMETRY_AABB_NV; };
	template<> struct VkTypeTraits< VkBindAccelerationStructureMemoryInfoNV>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_BIND_ACCELERATION_STRUCTURE_MEMORY_INFO_NV; };
	template<> struct VkTypeTraits< VkWriteDescriptorSetAccelerationStructureNV>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_NV; };
	template<> struct VkTypeTraits< VkAccelerationStructureMemoryRequirementsInfoNV>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_INFO_NV; };
	template<> struct VkTypeTraits< VkPhysicalDeviceRayTracingPropertiesNV>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PROPERTIES_NV; };
	template<> struct VkTypeTraits< VkRayTracingShaderGroupCreateInfoNV>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_NV; };
	template<> struct VkTypeTraits< VkAccelerationStructureInfoNV>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_INFO_NV; };
	template<> struct VkTypeTraits< VkPhysicalDeviceRepresentativeFragmentTestFeaturesNV>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_REPRESENTATIVE_FRAGMENT_TEST_FEATURES_NV; };
	template<> struct VkTypeTraits< VkPipelineRepresentativeFragmentTestStateCreateInfoNV>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_REPRESENTATIVE_FRAGMENT_TEST_STATE_CREATE_INFO_NV; };
	template<> struct VkTypeTraits< VkPhysicalDeviceImageViewImageFormatInfoEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_VIEW_IMAGE_FORMAT_INFO_EXT; };
	template<> struct VkTypeTraits< VkFilterCubicImageViewImageFormatPropertiesEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_FILTER_CUBIC_IMAGE_VIEW_IMAGE_FORMAT_PROPERTIES_EXT; };
	template<> struct VkTypeTraits< VkDeviceQueueGlobalPriorityCreateInfoEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEVICE_QUEUE_GLOBAL_PRIORITY_CREATE_INFO_EXT; };
	template<> struct VkTypeTraits< VkPhysicalDevice8BitStorageFeaturesKHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES_KHR; };
	template<> struct VkTypeTraits< VkImportMemoryHostPointerInfoEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IMPORT_MEMORY_HOST_POINTER_INFO_EXT; };
	template<> struct VkTypeTraits< VkMemoryHostPointerPropertiesEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_MEMORY_HOST_POINTER_PROPERTIES_EXT; };
	template<> struct VkTypeTraits< VkPhysicalDeviceExternalMemoryHostPropertiesEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_MEMORY_HOST_PROPERTIES_EXT; };
	template<> struct VkTypeTraits< VkPhysicalDeviceShaderAtomicInt64FeaturesKHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_INT64_FEATURES_KHR; };
	template<> struct VkTypeTraits< VkCalibratedTimestampInfoEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_CALIBRATED_TIMESTAMP_INFO_EXT; };
	template<> struct VkTypeTraits< VkPhysicalDeviceShaderCorePropertiesAMD>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CORE_PROPERTIES_AMD; };
	template<> struct VkTypeTraits< VkDeviceMemoryOverallocationCreateInfoAMD>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_DEVICE_MEMORY_OVERALLOCATION_CREATE_INFO_AMD; };
	template<> struct VkTypeTraits< VkPhysicalDeviceVertexAttributeDivisorPropertiesEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_PROPERTIES_EXT; };
	template<> struct VkTypeTraits< VkPipelineVertexInputDivisorStateCreateInfoEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_DIVISOR_STATE_CREATE_INFO_EXT; };
	template<> struct VkTypeTraits< VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_FEATURES_EXT; };
	template<> struct VkTypeTraits< VkPhysicalDeviceDriverPropertiesKHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES_KHR; };
	template<> struct VkTypeTraits< VkPhysicalDeviceFloatControlsPropertiesKHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FLOAT_CONTROLS_PROPERTIES_KHR; };
	template<> struct VkTypeTraits< VkPhysicalDeviceDepthStencilResolvePropertiesKHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_STENCIL_RESOLVE_PROPERTIES_KHR; };
	template<> struct VkTypeTraits< VkSubpassDescriptionDepthStencilResolveKHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_DEPTH_STENCIL_RESOLVE_KHR; };
	template<> struct VkTypeTraits< VkPhysicalDeviceComputeShaderDerivativesFeaturesNV>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COMPUTE_SHADER_DERIVATIVES_FEATURES_NV; };
	template<> struct VkTypeTraits< VkPhysicalDeviceMeshShaderFeaturesNV>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_NV; };
	template<> struct VkTypeTraits< VkPhysicalDeviceMeshShaderPropertiesNV>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_PROPERTIES_NV; };
	template<> struct VkTypeTraits< VkPhysicalDeviceFragmentShaderBarycentricFeaturesNV>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADER_BARYCENTRIC_FEATURES_NV; };
	template<> struct VkTypeTraits< VkPhysicalDeviceShaderImageFootprintFeaturesNV>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_IMAGE_FOOTPRINT_FEATURES_NV; };
	template<> struct VkTypeTraits< VkPipelineViewportExclusiveScissorStateCreateInfoNV>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_EXCLUSIVE_SCISSOR_STATE_CREATE_INFO_NV; };
	template<> struct VkTypeTraits< VkPhysicalDeviceExclusiveScissorFeaturesNV>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXCLUSIVE_SCISSOR_FEATURES_NV; };
	template<> struct VkTypeTraits< VkCheckpointDataNV>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_CHECKPOINT_DATA_NV; };
	template<> struct VkTypeTraits< VkQueueFamilyCheckpointPropertiesNV>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_QUEUE_FAMILY_CHECKPOINT_PROPERTIES_NV; };
	template<> struct VkTypeTraits< VkPhysicalDeviceVulkanMemoryModelFeaturesKHR>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_MEMORY_MODEL_FEATURES_KHR; };
	template<> struct VkTypeTraits< VkPhysicalDevicePCIBusInfoPropertiesEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PCI_BUS_INFO_PROPERTIES_EXT; };
	template<> struct VkTypeTraits< VkPhysicalDeviceFragmentDensityMapFeaturesEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_FEATURES_EXT; };
	template<> struct VkTypeTraits< VkPhysicalDeviceFragmentDensityMapPropertiesEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_PROPERTIES_EXT; };
	template<> struct VkTypeTraits< VkRenderPassFragmentDensityMapCreateInfoEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_RENDER_PASS_FRAGMENT_DENSITY_MAP_CREATE_INFO_EXT; };
	template<> struct VkTypeTraits< VkPhysicalDeviceScalarBlockLayoutFeaturesEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SCALAR_BLOCK_LAYOUT_FEATURES_EXT; };
	template<> struct VkTypeTraits< VkPhysicalDeviceMemoryBudgetPropertiesEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_BUDGET_PROPERTIES_EXT; };
	template<> struct VkTypeTraits< VkPhysicalDeviceMemoryPriorityFeaturesEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PRIORITY_FEATURES_EXT; };
	template<> struct VkTypeTraits< VkMemoryPriorityAllocateInfoEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_MEMORY_PRIORITY_ALLOCATE_INFO_EXT; };
	template<> struct VkTypeTraits< VkPhysicalDeviceDedicatedAllocationImageAliasingFeaturesNV>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEDICATED_ALLOCATION_IMAGE_ALIASING_FEATURES_NV; };
	template<> struct VkTypeTraits< VkPhysicalDeviceBufferAddressFeaturesEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_ADDRESS_FEATURES_EXT; };
	template<> struct VkTypeTraits< VkBufferDeviceAddressInfoEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO_EXT; };
	template<> struct VkTypeTraits< VkBufferDeviceAddressCreateInfoEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_CREATE_INFO_EXT; };
	template<> struct VkTypeTraits< VkImageStencilUsageCreateInfoEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_IMAGE_STENCIL_USAGE_CREATE_INFO_EXT; };
	template<> struct VkTypeTraits< VkValidationFeaturesEXT>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT; };
	template<> struct VkTypeTraits< VkPhysicalDeviceCooperativeMatrixFeaturesNV>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_FEATURES_NV; };
	template<> struct VkTypeTraits< VkCooperativeMatrixPropertiesNV>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_COOPERATIVE_MATRIX_PROPERTIES_NV; };
	template<> struct VkTypeTraits< VkPhysicalDeviceCooperativeMatrixPropertiesNV>{ static VkStructureType constexpr value = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_PROPERTIES_NV; };

	template< typename VkType, typename ... Params >
	inline VkType makeVkType( Params && ... params )
	{
		return VkType
		{
			VkTypeTraits< VkType >::value,
			nullptr,
			std::forward< Params >( params )...
		};
	}
}

#endif
