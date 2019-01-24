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
#	include <Buffer/Buffer.hpp>
#	include <Buffer/BufferView.hpp>
#	include <Buffer/PushConstantsBuffer.hpp>
#	include <Buffer/StagingBuffer.hpp>
#	include <Buffer/UniformBuffer.hpp>
#	include <Buffer/VertexBuffer.hpp>
#	include <Command/CommandBuffer.hpp>
#	include <Command/CommandBufferInheritanceInfo.hpp>
#	include <Command/CommandPool.hpp>
#	include <Command/Queue.hpp>
#	include <Core/BackBuffer.hpp>
#	include <Core/Connection.hpp>
#	include <Core/PhysicalDevice.hpp>
#	include <Core/Renderer.hpp>
#	include <Core/SwapChain.hpp>
#	include <Core/WindowHandle.hpp>
#	include <Descriptor/DescriptorSet.hpp>
#	include <Descriptor/DescriptorSetLayout.hpp>
#	include <Descriptor/DescriptorSetPool.hpp>
#	include <Image/Sampler.hpp>
#	include <Image/Texture.hpp>
#	include <Image/TextureView.hpp>
#	include <Pipeline/ColourBlendState.hpp>
#	include <Pipeline/ComputePipeline.hpp>
#	include <Pipeline/DepthStencilState.hpp>
#	include <Pipeline/InputAssemblyState.hpp>
#	include <Pipeline/MultisampleState.hpp>
#	include <Pipeline/Pipeline.hpp>
#	include <Pipeline/PipelineLayout.hpp>
#	include <Pipeline/RasterisationState.hpp>
#	include <Pipeline/Scissor.hpp>
#	include <Pipeline/ShaderStageState.hpp>
#	include <Pipeline/SpecialisationInfo.hpp>
#	include <Pipeline/StencilOpState.hpp>
#	include <Pipeline/TessellationState.hpp>
#	include <Pipeline/VertexInputState.hpp>
#	include <Pipeline/VertexLayout.hpp>
#	include <Pipeline/Viewport.hpp>
#	include <RenderPass/FrameBuffer.hpp>
#	include <RenderPass/RenderPass.hpp>
#	include <RenderPass/RenderPassCreateInfo.hpp>
#	include <RenderPass/RenderSubpass.hpp>
#	include <Shader/ShaderModule.hpp>
#	include <Sync/BufferMemoryBarrier.hpp>
#	include <Sync/Fence.hpp>
#	include <Sync/ImageMemoryBarrier.hpp>
#	include <Sync/Semaphore.hpp>

#endif

#endif
