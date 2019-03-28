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
