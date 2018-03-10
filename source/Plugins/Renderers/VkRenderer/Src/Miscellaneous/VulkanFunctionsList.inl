/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#if defined( CreateSemaphore )
#	undef CreateSemaphore
#endif

#ifndef VK_LIB_GLOBAL_FUNCTION
#	define VK_LIB_GLOBAL_FUNCTION( x )
#endif

VK_LIB_GLOBAL_FUNCTION( vkCreateInstance )
VK_LIB_GLOBAL_FUNCTION( vkEnumerateInstanceExtensionProperties )
VK_LIB_GLOBAL_FUNCTION( vkEnumerateInstanceLayerProperties )

#undef VK_LIB_GLOBAL_FUNCTION

#ifndef VK_LIB_INSTANCE_FUNCTION
#	define VK_LIB_INSTANCE_FUNCTION( x )
#endif


// Vulkan core
VK_LIB_INSTANCE_FUNCTION( vkCreateDevice )
VK_LIB_INSTANCE_FUNCTION( vkDestroyInstance )
VK_LIB_INSTANCE_FUNCTION( vkEnumeratePhysicalDevices )
VK_LIB_INSTANCE_FUNCTION( vkEnumerateDeviceExtensionProperties )
VK_LIB_INSTANCE_FUNCTION( vkGetDeviceProcAddr )
VK_LIB_INSTANCE_FUNCTION( vkGetPhysicalDeviceFeatures )
VK_LIB_INSTANCE_FUNCTION( vkGetPhysicalDeviceFormatProperties )
VK_LIB_INSTANCE_FUNCTION( vkGetPhysicalDeviceImageFormatProperties )
VK_LIB_INSTANCE_FUNCTION( vkGetPhysicalDeviceMemoryProperties )
VK_LIB_INSTANCE_FUNCTION( vkGetPhysicalDeviceProperties )
VK_LIB_INSTANCE_FUNCTION( vkGetPhysicalDeviceQueueFamilyProperties )

// VK_KHR_display
//VK_LIB_INSTANCE_FUNCTION( vkCreateDisplayPlaneSurfaceKHR )
//VK_LIB_INSTANCE_FUNCTION( vkGetDisplayModePropertiesKHR )
//VK_LIB_INSTANCE_FUNCTION( vkGetDisplayPlaneCapabilitiesKHR )
//VK_LIB_INSTANCE_FUNCTION( vkGetDisplayPlaneSupportedDisplaysKHR )
//VK_LIB_INSTANCE_FUNCTION( vkGetPhysicalDeviceDisplayPlanePropertiesKHR )
//VK_LIB_INSTANCE_FUNCTION( vkGetPhysicalDeviceDisplayPropertiesKHR )

// VK_KHR_surface
VK_LIB_INSTANCE_FUNCTION( vkDestroySurfaceKHR )
VK_LIB_INSTANCE_FUNCTION( vkGetPhysicalDeviceSurfaceCapabilitiesKHR )
VK_LIB_INSTANCE_FUNCTION( vkGetPhysicalDeviceSurfaceFormatsKHR )
VK_LIB_INSTANCE_FUNCTION( vkGetPhysicalDeviceSurfacePresentModesKHR )
VK_LIB_INSTANCE_FUNCTION( vkGetPhysicalDeviceSurfaceSupportKHR )

// VK_EXT_debug_report
VK_LIB_INSTANCE_FUNCTION( vkCreateDebugReportCallbackEXT )
VK_LIB_INSTANCE_FUNCTION( vkDestroyDebugReportCallbackEXT )
VK_LIB_INSTANCE_FUNCTION( vkDebugReportMessageEXT )

#ifdef VK_USE_PLATFORM_ANDROID_KHR
// VK_KHR_android_surface
VK_LIB_INSTANCE_FUNCTION( vkCreateAndroidSurfaceKHR )
#endif

#ifdef VK_USE_PLATFORM_MIR_KHR
// VK_KHR_mir_surface
VK_LIB_INSTANCE_FUNCTION( vkCreateMirSurfaceKHR )
VK_LIB_INSTANCE_FUNCTION( vkGetPhysicalDeviceMirPresentationSupportKHR )
#endif

#ifdef VK_USE_PLATFORM_XCB_KHR
// VK_KHR_xcb_surface
VK_LIB_INSTANCE_FUNCTION( vkCreateXcbSurfaceKHR )
VK_LIB_INSTANCE_FUNCTION( vkGetPhysicalDeviceXcbPresentationSupportKHR )
#endif

#ifdef VK_USE_PLATFORM_XLIB_KHR
// VK_KHR_xlib_surface
VK_LIB_INSTANCE_FUNCTION( vkCreateXlibSurfaceKHR )
VK_LIB_INSTANCE_FUNCTION( vkGetPhysicalDeviceXlibPresentationSupportKHR )
#endif

#ifdef VK_USE_PLATFORM_WAYLAND_KHR
// VK_KHR_wayland_surface
VK_LIB_INSTANCE_FUNCTION( vkCreateWaylandSurfaceKHR )
VK_LIB_INSTANCE_FUNCTION( vkGetPhysicalDeviceWaylandPresentationSupportKHR )
#endif

#ifdef VK_USE_PLATFORM_WIN32_KHR
// VK_KHR_win32_surface
VK_LIB_INSTANCE_FUNCTION( vkCreateWin32SurfaceKHR )
VK_LIB_INSTANCE_FUNCTION( vkGetPhysicalDeviceWin32PresentationSupportKHR )
#endif

#undef VK_LIB_INSTANCE_FUNCTION

#ifndef VK_LIB_DEVICE_FUNCTION
#	define VK_LIB_DEVICE_FUNCTION( x )
#endif

VK_LIB_DEVICE_FUNCTION( vkAcquireNextImageKHR )
VK_LIB_DEVICE_FUNCTION( vkAllocateCommandBuffers )
VK_LIB_DEVICE_FUNCTION( vkAllocateDescriptorSets )
VK_LIB_DEVICE_FUNCTION( vkAllocateMemory )
VK_LIB_DEVICE_FUNCTION( vkBeginCommandBuffer )
VK_LIB_DEVICE_FUNCTION( vkBindBufferMemory )
VK_LIB_DEVICE_FUNCTION( vkBindImageMemory )
VK_LIB_DEVICE_FUNCTION( vkCmdBeginQuery )
VK_LIB_DEVICE_FUNCTION( vkCmdBeginRenderPass )
VK_LIB_DEVICE_FUNCTION( vkCmdBindDescriptorSets )
VK_LIB_DEVICE_FUNCTION( vkCmdBindIndexBuffer )
VK_LIB_DEVICE_FUNCTION( vkCmdBindPipeline )
VK_LIB_DEVICE_FUNCTION( vkCmdBindVertexBuffers )
VK_LIB_DEVICE_FUNCTION( vkCmdBlitImage )
VK_LIB_DEVICE_FUNCTION( vkCmdClearAttachments )
VK_LIB_DEVICE_FUNCTION( vkCmdClearColorImage )
VK_LIB_DEVICE_FUNCTION( vkCmdClearDepthStencilImage )
VK_LIB_DEVICE_FUNCTION( vkCmdCopyBuffer )
VK_LIB_DEVICE_FUNCTION( vkCmdCopyBufferToImage )
VK_LIB_DEVICE_FUNCTION( vkCmdCopyImage )
VK_LIB_DEVICE_FUNCTION( vkCmdDispatch )
VK_LIB_DEVICE_FUNCTION( vkCmdDispatchIndirect )
VK_LIB_DEVICE_FUNCTION( vkCmdDraw )
VK_LIB_DEVICE_FUNCTION( vkCmdDrawIndexed )
VK_LIB_DEVICE_FUNCTION( vkCmdDrawIndirect )
VK_LIB_DEVICE_FUNCTION( vkCmdDrawIndexedIndirect )
VK_LIB_DEVICE_FUNCTION( vkCmdEndQuery )
VK_LIB_DEVICE_FUNCTION( vkCmdEndRenderPass )
VK_LIB_DEVICE_FUNCTION( vkCmdExecuteCommands )
VK_LIB_DEVICE_FUNCTION( vkCmdCopyImageToBuffer )
VK_LIB_DEVICE_FUNCTION( vkCmdNextSubpass )
VK_LIB_DEVICE_FUNCTION( vkCmdPipelineBarrier )
VK_LIB_DEVICE_FUNCTION( vkCmdPushConstants )
VK_LIB_DEVICE_FUNCTION( vkCmdResetQueryPool )
VK_LIB_DEVICE_FUNCTION( vkCmdSetDepthBias )
VK_LIB_DEVICE_FUNCTION( vkCmdSetLineWidth )
VK_LIB_DEVICE_FUNCTION( vkCmdSetScissor )
VK_LIB_DEVICE_FUNCTION( vkCmdSetViewport )
VK_LIB_DEVICE_FUNCTION( vkCmdWriteTimestamp )
VK_LIB_DEVICE_FUNCTION( vkCreateBuffer )
VK_LIB_DEVICE_FUNCTION( vkCreateBufferView )
VK_LIB_DEVICE_FUNCTION( vkCreateCommandPool )
VK_LIB_DEVICE_FUNCTION( vkCreateComputePipelines )
VK_LIB_DEVICE_FUNCTION( vkCreateDescriptorPool )
VK_LIB_DEVICE_FUNCTION( vkCreateDescriptorSetLayout )
VK_LIB_DEVICE_FUNCTION( vkCreateFence )
VK_LIB_DEVICE_FUNCTION( vkCreateFramebuffer )
VK_LIB_DEVICE_FUNCTION( vkCreateGraphicsPipelines )
VK_LIB_DEVICE_FUNCTION( vkCreateImage )
VK_LIB_DEVICE_FUNCTION( vkCreateImageView )
VK_LIB_DEVICE_FUNCTION( vkCreateInstance )
VK_LIB_DEVICE_FUNCTION( vkCreatePipelineLayout )
VK_LIB_DEVICE_FUNCTION( vkCreateRenderPass )
VK_LIB_DEVICE_FUNCTION( vkCreateQueryPool )
VK_LIB_DEVICE_FUNCTION( vkCreateSampler )
VK_LIB_DEVICE_FUNCTION( vkCreateSemaphore )
VK_LIB_DEVICE_FUNCTION( vkCreateShaderModule )
VK_LIB_DEVICE_FUNCTION( vkCreateSwapchainKHR )
VK_LIB_DEVICE_FUNCTION( vkDestroyBuffer )
VK_LIB_DEVICE_FUNCTION( vkDestroyBufferView )
VK_LIB_DEVICE_FUNCTION( vkDestroyCommandPool )
VK_LIB_DEVICE_FUNCTION( vkDestroyDescriptorPool )
VK_LIB_DEVICE_FUNCTION( vkDestroyDescriptorSetLayout )
VK_LIB_DEVICE_FUNCTION( vkDestroyDevice )
VK_LIB_DEVICE_FUNCTION( vkDestroyFence )
VK_LIB_DEVICE_FUNCTION( vkDestroyFramebuffer )
VK_LIB_DEVICE_FUNCTION( vkDestroyImage )
VK_LIB_DEVICE_FUNCTION( vkDestroyImageView )
VK_LIB_DEVICE_FUNCTION( vkDestroyPipeline )
VK_LIB_DEVICE_FUNCTION( vkDestroyPipelineLayout )
VK_LIB_DEVICE_FUNCTION( vkDestroyQueryPool )
VK_LIB_DEVICE_FUNCTION( vkDestroyRenderPass )
VK_LIB_DEVICE_FUNCTION( vkDestroySampler )
VK_LIB_DEVICE_FUNCTION( vkDestroySemaphore )
VK_LIB_DEVICE_FUNCTION( vkDestroyShaderModule )
VK_LIB_DEVICE_FUNCTION( vkDestroySwapchainKHR )
VK_LIB_DEVICE_FUNCTION( vkDeviceWaitIdle )
VK_LIB_DEVICE_FUNCTION( vkEndCommandBuffer )
VK_LIB_DEVICE_FUNCTION( vkFlushMappedMemoryRanges )
VK_LIB_DEVICE_FUNCTION( vkFreeCommandBuffers )
VK_LIB_DEVICE_FUNCTION( vkFreeDescriptorSets )
VK_LIB_DEVICE_FUNCTION( vkFreeMemory )
VK_LIB_DEVICE_FUNCTION( vkGetBufferMemoryRequirements )
VK_LIB_DEVICE_FUNCTION( vkGetDeviceQueue )
VK_LIB_DEVICE_FUNCTION( vkGetImageMemoryRequirements )
VK_LIB_DEVICE_FUNCTION( vkGetImageSubresourceLayout )
VK_LIB_DEVICE_FUNCTION( vkGetQueryPoolResults )
VK_LIB_DEVICE_FUNCTION( vkGetSwapchainImagesKHR )
VK_LIB_DEVICE_FUNCTION( vkInvalidateMappedMemoryRanges )
VK_LIB_DEVICE_FUNCTION( vkMapMemory )
VK_LIB_DEVICE_FUNCTION( vkQueuePresentKHR )
VK_LIB_DEVICE_FUNCTION( vkQueueSubmit )
VK_LIB_DEVICE_FUNCTION( vkQueueWaitIdle )
VK_LIB_DEVICE_FUNCTION( vkResetCommandBuffer )
VK_LIB_DEVICE_FUNCTION( vkResetFences )
VK_LIB_DEVICE_FUNCTION( vkUnmapMemory )
VK_LIB_DEVICE_FUNCTION( vkUpdateDescriptorSets )
VK_LIB_DEVICE_FUNCTION( vkWaitForFences )

#undef VK_LIB_DEVICE_FUNCTION
