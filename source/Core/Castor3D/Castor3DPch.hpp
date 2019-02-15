/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PRECOMPILED_HEADER_H___
#define ___C3D_PRECOMPILED_HEADER_H___

#include <config.hpp>

#if CU_UsePCH
#	include "Castor3DPrerequisites.hpp"

#	include <RequiredVersion.hpp>

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
#	include <CastorUtils.hpp>
#	include <Config/MultiThreadConfig.hpp>
#	include <Data/BinaryFile.hpp>
#	include <Data/BinaryLoader.hpp>
#	include <Data/Path.hpp>
#	include <Data/TextFile.hpp>
#	include <Data/TextLoader.hpp>
#	include <Design/Collection.hpp>
#	include <Design/Factory.hpp>
#	include <Design/FlagCombination.hpp>
#	include <Design/Named.hpp>
#	include <Design/NonCopyable.hpp>
#	include <Design/OwnedBy.hpp>
#	include <Design/Resource.hpp>
#	include <Design/Templates.hpp>
#	include <Design/Unique.hpp>
#	include <Graphics/Colour.hpp>
#	include <Graphics/BoundingBox.hpp>
#	include <Graphics/BoundingSphere.hpp>
#	include <Graphics/Font.hpp>
#	include <Graphics/Image.hpp>
#	include <Graphics/Pixel.hpp>
#	include <Graphics/Rectangle.hpp>
#	include <Graphics/Size.hpp>
#	include <Log/Logger.hpp>
#	include <Math/Angle.hpp>
#	include <Math/Line2D.hpp>
#	include <Math/Line3D.hpp>
#	include <Math/Math.hpp>
#	include <Math/Matrix.hpp>
#	include <Math/PlaneEquation.hpp>
#	include <Math/Point.hpp>
#	include <Math/Quaternion.hpp>
#	include <Math/SphericalVertex.hpp>
#	include <Math/SquareMatrix.hpp>
#	include <Math/TransformationMatrix.hpp>
#	include <Miscellaneous/BlockTimer.hpp>
#	include <Miscellaneous/DynamicLibrary.hpp>
#	include <Miscellaneous/PreciseTimer.hpp>
#	include <Miscellaneous/StringUtils.hpp>
#	include <Miscellaneous/Utils.hpp>

// ShaderWriter
#	include <ShaderWriter/Source.hpp>

// RendererLib
#	include <Ashes/Buffer/Buffer.hpp>
#	include <Ashes/Buffer/BufferView.hpp>
#	include <Ashes/Buffer/PushConstantsBuffer.hpp>
#	include <Ashes/Buffer/StagingBuffer.hpp>
#	include <Ashes/Buffer/UniformBuffer.hpp>
#	include <Ashes/Buffer/VertexBuffer.hpp>
#	include <Ashes/Command/CommandBuffer.hpp>
#	include <Ashes/Command/CommandBufferInheritanceInfo.hpp>
#	include <Ashes/Command/CommandPool.hpp>
#	include <Ashes/Command/Queue.hpp>
#	include <Ashes/Core/BackBuffer.hpp>
#	include <Ashes/Core/Connection.hpp>
#	include <Ashes/Core/PhysicalDevice.hpp>
#	include <Ashes/Core/Renderer.hpp>
#	include <Ashes/Core/SwapChain.hpp>
#	include <Ashes/Core/WindowHandle.hpp>
#	include <Ashes/Descriptor/DescriptorSet.hpp>
#	include <Ashes/Descriptor/DescriptorSetLayout.hpp>
#	include <Ashes/Descriptor/DescriptorSetPool.hpp>
#	include <Ashes/Image/Sampler.hpp>
#	include <Ashes/Image/Texture.hpp>
#	include <Ashes/Image/TextureView.hpp>
#	include <Ashes/Pipeline/ColourBlendState.hpp>
#	include <Ashes/Pipeline/ComputePipeline.hpp>
#	include <Ashes/Pipeline/DepthStencilState.hpp>
#	include <Ashes/Pipeline/InputAssemblyState.hpp>
#	include <Ashes/Pipeline/MultisampleState.hpp>
#	include <Ashes/Pipeline/Pipeline.hpp>
#	include <Ashes/Pipeline/PipelineLayout.hpp>
#	include <Ashes/Pipeline/RasterisationState.hpp>
#	include <Ashes/Pipeline/Scissor.hpp>
#	include <Ashes/Pipeline/ShaderStageState.hpp>
#	include <Ashes/Pipeline/SpecialisationInfo.hpp>
#	include <Ashes/Pipeline/StencilOpState.hpp>
#	include <Ashes/Pipeline/TessellationState.hpp>
#	include <Ashes/Pipeline/VertexInputState.hpp>
#	include <Ashes/Pipeline/VertexLayout.hpp>
#	include <Ashes/Pipeline/Viewport.hpp>
#	include <Ashes/RenderPass/FrameBuffer.hpp>
#	include <Ashes/RenderPass/RenderPass.hpp>
#	include <Ashes/RenderPass/RenderPassCreateInfo.hpp>
#	include <Ashes/RenderPass/RenderSubpass.hpp>
#	include <Ashes/Shader/ShaderModule.hpp>
#	include <Ashes/Sync/BufferMemoryBarrier.hpp>
#	include <Ashes/Sync/Fence.hpp>
#	include <Ashes/Sync/ImageMemoryBarrier.hpp>
#	include <Ashes/Sync/Semaphore.hpp>

#endif

#endif
