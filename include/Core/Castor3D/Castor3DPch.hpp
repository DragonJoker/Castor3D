/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PRECOMPILED_HEADER_H___
#define ___C3D_PRECOMPILED_HEADER_H___

#include <CastorUtils/config.hpp>

#if CU_UsePCH
#	include "Castor3D/Castor3DPrerequisites.hpp"

#	include "Castor3D/RequiredVersion.hpp"

// STL
#	include <string>
#	include <vector>
#	include <map>
#	include <iostream>
#	include <algorithm>
#	include <list>
#	include <fstream>
#	include <algorithm>
#	include <stack>
#	include <typeinfo>
#	include <locale>
#	include <stdexcept>

// SL
#	include <cstdarg>
#	include <cmath>
#	include <ctime>
#	include <cstdint>

// Utils
#	include <CastorUtils/CastorUtils.hpp>
#	include <CastorUtils/Config/MultiThreadConfig.hpp>
#	include <CastorUtils/Data/BinaryFile.hpp>
#	include <CastorUtils/Data/BinaryLoader.hpp>
#	include <CastorUtils/Data/Path.hpp>
#	include <CastorUtils/Data/TextFile.hpp>
#	include <CastorUtils/Data/TextLoader.hpp>
#	include <CastorUtils/Design/Collection.hpp>
#	include <CastorUtils/Design/Factory.hpp>
#	include <CastorUtils/Design/FlagCombination.hpp>
#	include <CastorUtils/Design/Named.hpp>
#	include <CastorUtils/Design/NonCopyable.hpp>
#	include <CastorUtils/Design/OwnedBy.hpp>
#	include <CastorUtils/Design/Resource.hpp>
#	include <CastorUtils/Design/Templates.hpp>
#	include <CastorUtils/Design/Unique.hpp>
#	include <CastorUtils/Graphics/Colour.hpp>
#	include <CastorUtils/Graphics/BoundingBox.hpp>
#	include <CastorUtils/Graphics/BoundingSphere.hpp>
#	include <CastorUtils/Graphics/Font.hpp>
#	include <CastorUtils/Graphics/Image.hpp>
#	include <CastorUtils/Graphics/Pixel.hpp>
#	include <CastorUtils/Graphics/Rectangle.hpp>
#	include <CastorUtils/Graphics/Size.hpp>
#	include <CastorUtils/Log/Logger.hpp>
#	include <CastorUtils/Math/Angle.hpp>
#	include <CastorUtils/Math/Line2D.hpp>
#	include <CastorUtils/Math/Line3D.hpp>
#	include <CastorUtils/Math/Math.hpp>
#	include <CastorUtils/Math/Matrix.hpp>
#	include <CastorUtils/Math/PlaneEquation.hpp>
#	include <CastorUtils/Math/Point.hpp>
#	include <CastorUtils/Math/Quaternion.hpp>
#	include <CastorUtils/Math/SphericalVertex.hpp>
#	include <CastorUtils/Math/SquareMatrix.hpp>
#	include <CastorUtils/Math/TransformationMatrix.hpp>
#	include <CastorUtils/Miscellaneous/BlockTimer.hpp>
#	include <CastorUtils/Miscellaneous/DynamicLibrary.hpp>
#	include <CastorUtils/Miscellaneous/PreciseTimer.hpp>
#	include <CastorUtils/Miscellaneous/StringUtils.hpp>
#	include <CastorUtils/Miscellaneous/Utils.hpp>

// ShaderWriter
#	include <ShaderWriter/Source.hpp>

// Ashes
#	include <ashespp/Buffer/Buffer.hpp>
#	include <ashespp/Buffer/BufferView.hpp>
#	include <ashespp/Buffer/PushConstantsBuffer.hpp>
#	include <ashespp/Buffer/StagingBuffer.hpp>
#	include <ashespp/Buffer/UniformBuffer.hpp>
#	include <ashespp/Buffer/VertexBuffer.hpp>
#	include <ashespp/Command/CommandBuffer.hpp>
#	include <ashespp/Command/CommandBufferInheritanceInfo.hpp>
#	include <ashespp/Command/CommandPool.hpp>
#	include <ashespp/Command/Queue.hpp>
#	include <ashespp/Core/ApplicationInfo.hpp>
#	include <ashespp/Core/Device.hpp>
#	include <ashespp/Core/DeviceCreateInfo.hpp>
#	include <ashespp/Core/Instance.hpp>
#	include <ashespp/Core/InstanceCreateInfo.hpp>
#	include <ashespp/Core/PhysicalDevice.hpp>
#	include <ashespp/Core/RendererList.hpp>
#	include <ashespp/Core/Surface.hpp>
#	include <ashespp/Core/SwapChain.hpp>
#	include <ashespp/Core/SwapChainCreateInfo.hpp>
#	include <ashespp/Core/WindowHandle.hpp>
#	include <ashespp/Descriptor/DescriptorPool.hpp>
#	include <ashespp/Descriptor/DescriptorSet.hpp>
#	include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#	include <ashespp/Descriptor/DescriptorSetPool.hpp>
#	include <ashespp/Image/Image.hpp>
#	include <ashespp/Image/ImageCreateInfo.hpp>
#	include <ashespp/Image/ImageView.hpp>
#	include <ashespp/Image/ImageViewCreateInfo.hpp>
#	include <ashespp/Image/Sampler.hpp>
#	include <ashespp/Image/SamplerCreateInfo.hpp>
#	include <ashespp/Miscellaneous/DeviceMemory.hpp>
#	include <ashespp/Miscellaneous/QueryPool.hpp>
#	include <ashespp/Pipeline/ComputePipeline.hpp>
#	include <ashespp/Pipeline/ComputePipelineCreateInfo.hpp>
#	include <ashespp/Pipeline/GraphicsPipeline.hpp>
#	include <ashespp/Pipeline/GraphicsPipelineCreateInfo.hpp>
#	include <ashespp/Pipeline/PipelineColorBlendStateCreateInfo.hpp>
#	include <ashespp/Pipeline/PipelineDepthStencilStateCreateInfo.hpp>
#	include <ashespp/Pipeline/PipelineDynamicStateCreateInfo.hpp>
#	include <ashespp/Pipeline/PipelineInputAssemblyStateCreateInfo.hpp>
#	include <ashespp/Pipeline/PipelineLayout.hpp>
#	include <ashespp/Pipeline/PipelineMultisampleStateCreateInfo.hpp>
#	include <ashespp/Pipeline/PipelineRasterizationStateCreateInfo.hpp>
#	include <ashespp/Pipeline/PipelineShaderStageCreateInfo.hpp>
#	include <ashespp/Pipeline/PipelineTessellationStateCreateInfo.hpp>
#	include <ashespp/Pipeline/PipelineVertexInputStateCreateInfo.hpp>
#	include <ashespp/Pipeline/PipelineViewportStateCreateInfo.hpp>
#	include <ashespp/RenderPass/FrameBuffer.hpp>
#	include <ashespp/RenderPass/RenderPass.hpp>
#	include <ashespp/RenderPass/RenderPassCreateInfo.hpp>
#	include <ashespp/RenderPass/SubpassDescription.hpp>
#	include <ashespp/Shader/ShaderModule.hpp>
#	include <ashespp/Sync/Fence.hpp>
#	include <ashespp/Sync/Semaphore.hpp>

#endif

#endif
