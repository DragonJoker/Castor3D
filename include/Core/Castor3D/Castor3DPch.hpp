/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Castor3DPch_H___
#define ___C3D_Castor3DPch_H___

#include <CastorUtils/Data/BinaryFile.hpp>
#include <CastorUtils/Data/Endianness.hpp>
#include <CastorUtils/Data/Loader.hpp>
#include <CastorUtils/Data/Path.hpp>
#include <CastorUtils/Data/TextWriter.hpp>
#include <CastorUtils/Data/ZipArchive.hpp>
#include <CastorUtils/Data/Text/TextFont.hpp>
#include <CastorUtils/Data/Text/TextPoint.hpp>
#include <CastorUtils/Data/Text/TextPosition.hpp>
#include <CastorUtils/Data/Text/TextQuaternion.hpp>
#include <CastorUtils/Data/Text/TextRgbColour.hpp>
#include <CastorUtils/Data/Text/TextSize.hpp>
#include <CastorUtils/Design/ArrayView.hpp>
#include <CastorUtils/Design/BlockGuard.hpp>
#include <CastorUtils/Design/ChangeTracked.hpp>
#include <CastorUtils/Design/Collection.hpp>
#include <CastorUtils/Design/DelayedInitialiser.hpp>
#include <CastorUtils/Design/Factory.hpp>
#include <CastorUtils/Design/FlagCombination.hpp>
#include <CastorUtils/Design/GroupChangeTracked.hpp>
#include <CastorUtils/Design/Named.hpp>
#include <CastorUtils/Design/OwnedBy.hpp>
#include <CastorUtils/Design/Resource.hpp>
#include <CastorUtils/Design/ScopeGuard.hpp>
#include <CastorUtils/Design/Signal.hpp>
#include <CastorUtils/Design/Unique.hpp>
#include <CastorUtils/Exception/Assertion.hpp>
#include <CastorUtils/Exception/Exception.hpp>
#include <CastorUtils/FileParser/AttributeParsersBySection.hpp>
#include <CastorUtils/FileParser/FileParser.hpp>
#include <CastorUtils/FileParser/FileParserContext.hpp>
#include <CastorUtils/FileParser/ParserParameter.hpp>
#include <CastorUtils/Graphics/BoundingBox.hpp>
#include <CastorUtils/Graphics/BoundingSphere.hpp>
#include <CastorUtils/Graphics/Colour.hpp>
#include <CastorUtils/Graphics/ExrImageLoader.hpp>
#include <CastorUtils/Graphics/Font.hpp>
#include <CastorUtils/Graphics/FontCache.hpp>
#include <CastorUtils/Graphics/FreeImageLoader.hpp>
#include <CastorUtils/Graphics/GliImageLoader.hpp>
#include <CastorUtils/Graphics/GliImageWriter.hpp>
#include <CastorUtils/Graphics/Grid.hpp>
#include <CastorUtils/Graphics/HdrColourComponent.hpp>
#include <CastorUtils/Graphics/HeightMapToNormalMap.hpp>
#include <CastorUtils/Graphics/Image.hpp>
#include <CastorUtils/Graphics/ImageCache.hpp>
#include <CastorUtils/Graphics/ImageLayout.hpp>
#include <CastorUtils/Graphics/ImageLoader.hpp>
#include <CastorUtils/Graphics/ImageWriter.hpp>
#include <CastorUtils/Graphics/PixelBuffer.hpp>
#include <CastorUtils/Graphics/PixelBufferBase.hpp>
#include <CastorUtils/Graphics/PixelFormat.hpp>
#include <CastorUtils/Graphics/Position.hpp>
#include <CastorUtils/Graphics/Rectangle.hpp>
#include <CastorUtils/Graphics/RgbaColour.hpp>
#include <CastorUtils/Graphics/RgbColour.hpp>
#include <CastorUtils/Graphics/Size.hpp>
#include <CastorUtils/Graphics/StbImageLoader.hpp>
#include <CastorUtils/Graphics/StbImageWriter.hpp>
#include <CastorUtils/Graphics/XpmImageLoader.hpp>
#include <CastorUtils/Log/Logger.hpp>
#include <CastorUtils/Log/LoggerImpl.hpp>
#include <CastorUtils/Log/LoggerInstance.hpp>
#include <CastorUtils/Log/LoggerStream.hpp>
#include <CastorUtils/Math/Angle.hpp>
#include <CastorUtils/Math/Coords.hpp>
#include <CastorUtils/Math/Matrix.hpp>
#include <CastorUtils/Math/PlaneEquation.hpp>
#include <CastorUtils/Math/Point.hpp>
#include <CastorUtils/Math/Quaternion.hpp>
#include <CastorUtils/Math/RangedValue.hpp>
#include <CastorUtils/Math/Speed.hpp>
#include <CastorUtils/Math/SphericalVertex.hpp>
#include <CastorUtils/Math/SquareMatrix.hpp>
#include <CastorUtils/Math/TransformationMatrix.hpp>
#include <CastorUtils/Miscellaneous/BitSize.hpp>
#include <CastorUtils/Miscellaneous/BlockTracker.hpp>
#include <CastorUtils/Miscellaneous/CpuInformations.hpp>
#include <CastorUtils/Miscellaneous/Debug.hpp>
#include <CastorUtils/Miscellaneous/DynamicLibrary.hpp>
#include <CastorUtils/Miscellaneous/Hash.hpp>
#include <CastorUtils/Miscellaneous/PreciseTimer.hpp>
#include <CastorUtils/Miscellaneous/StringUtils.hpp>
#include <CastorUtils/Miscellaneous/Utils.hpp>
#include <CastorUtils/Multithreading/AsyncJobQueue.hpp>
#include <CastorUtils/Multithreading/MultithreadingModule.hpp>
#include <CastorUtils/Multithreading/ThreadPool.hpp>
#include <CastorUtils/Pool/BuddyAllocator.hpp>
#include <CastorUtils/Pool/UniqueObjectPool.hpp>
#include <CastorUtils/Stream/StreamPrefixManipulators.hpp>

#include <ShaderAST/Shader.hpp>
#include <ShaderAST/Expr/ExprComma.hpp>

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/Writer.hpp>
#include <ShaderWriter/BaseTypes/Array.hpp>
#include <ShaderWriter/BaseTypes/Bool.hpp>
#include <ShaderWriter/BaseTypes/Double.hpp>
#include <ShaderWriter/BaseTypes/Float.hpp>
#include <ShaderWriter/BaseTypes/Image.hpp>
#include <ShaderWriter/BaseTypes/Int.hpp>
#include <ShaderWriter/BaseTypes/SampledImage.hpp>
#include <ShaderWriter/BaseTypes/Sampler.hpp>
#include <ShaderWriter/BaseTypes/UInt.hpp>
#include <ShaderWriter/CompositeTypes/ArraySsbo.hpp>
#include <ShaderWriter/CompositeTypes/Function.hpp>
#include <ShaderWriter/CompositeTypes/Struct.hpp>
#include <ShaderWriter/CompositeTypes/StructInstance.hpp>
#include <ShaderWriter/Intrinsics/Intrinsics.hpp>
#include <ShaderWriter/MatTypes/Mat4.hpp>
#include <ShaderWriter/VecTypes/Vec4.hpp>

#include <CompilerSpirV/compileSpirV.hpp>

#include <ashes/ashes.hpp>

#include <ashes/common/Optional.hpp>

#include <ashespp/Buffer/Buffer.hpp>
#include <ashespp/Buffer/BufferView.hpp>
#include <ashespp/Buffer/PushConstantsBuffer.hpp>
#include <ashespp/Buffer/StagingBuffer.hpp>
#include <ashespp/Buffer/UniformBuffer.hpp>
#include <ashespp/Buffer/VertexBuffer.hpp>
#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Command/CommandBufferInheritanceInfo.hpp>
#include <ashespp/Command/CommandPool.hpp>
#include <ashespp/Core/Device.hpp>
#include <ashespp/Core/Instance.hpp>
#include <ashespp/Core/RendererList.hpp>
#include <ashespp/Core/Surface.hpp>
#include <ashespp/Core/SwapChain.hpp>
#include <ashespp/Core/SwapChainCreateInfo.hpp>
#include <ashespp/Core/WindowHandle.hpp>
#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Descriptor/DescriptorSetPool.hpp>
#include <ashespp/Image/Image.hpp>
#include <ashespp/Image/ImageCreateInfo.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/Image/Sampler.hpp>
#include <ashespp/Image/SamplerCreateInfo.hpp>
#include <ashespp/Image/StagingTexture.hpp>
#include <ashespp/Miscellaneous/DeviceMemory.hpp>
#include <ashespp/Miscellaneous/QueryPool.hpp>
#include <ashespp/Miscellaneous/RendererFeatures.hpp>
#include <ashespp/Pipeline/ComputePipeline.hpp>
#include <ashespp/Pipeline/ComputePipelineCreateInfo.hpp>
#include <ashespp/Pipeline/GraphicsPipeline.hpp>
#include <ashespp/Pipeline/GraphicsPipelineCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineLayout.hpp>
#include <ashespp/Pipeline/PipelineColorBlendStateCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineDepthStencilStateCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineInputAssemblyStateCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineMultisampleStateCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineRasterizationStateCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineShaderStageCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineVertexInputStateCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineViewportStateCreateInfo.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>
#include <ashespp/RenderPass/SubpassDescription.hpp>
#include <ashespp/Sync/Fence.hpp>
#include <ashespp/Sync/Queue.hpp>
#include <ashespp/Sync/Semaphore.hpp>

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <deque>
#include <iomanip>
#include <list>
#include <locale>
#include <map>
#include <memory>
#include <mutex>
#include <numeric>
#include <random>
#include <regex>
#include <set>
#include <stack>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>


#endif
