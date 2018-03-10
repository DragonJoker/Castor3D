/**
*\file
*	RenderLibPrerequisites.h
*\author
*	Sylvain Doremus
*/
#pragma once

#include <RendererPrerequisites.hpp>

#include "Miscellaneous/OpenGLFunctions.hpp"

#include "Enum/GlAccessFlag.hpp"
#include "Enum/GlAttachmentPoint.hpp"
#include "Enum/GlAttachmentType.hpp"
#include "Enum/GlBaseType.hpp"
#include "Enum/GlBlendFactor.hpp"
#include "Enum/GlBlendOp.hpp"
#include "Enum/GlBorderColour.hpp"
#include "Enum/GlBufferTarget.hpp"
#include "Enum/GlClearTarget.hpp"
#include "Enum/GlClipInfo.hpp"
#include "Enum/GlColourComponentFlag.hpp"
#include "Enum/GlCompareOp.hpp"
#include "Enum/GlComponentSwizzle.hpp"
#include "Enum/GlConstantFormat.hpp"
#include "Enum/GlCullModeFlag.hpp"
#include "Enum/GlFenceWaitFlag.hpp"
#include "Enum/GlFilter.hpp"
#include "Enum/GlFormat.hpp"
#include "Enum/GlFrameBufferTarget.hpp"
#include "Enum/GlFrontFace.hpp"
#include "Enum/GlGetParameter.hpp"
#include "Enum/GlImageAspectFlag.hpp"
#include "Enum/GlImageLayout.hpp"
#include "Enum/GlImageTiling.hpp"
#include "Enum/GlIndexType.hpp"
#include "Enum/GlLogicOp.hpp"
#include "Enum/GlMemoryBarrierFlag.hpp"
#include "Enum/GlMemoryMapFlag.hpp"
#include "Enum/GlMemoryPropertyFlag.hpp"
#include "Enum/GlMipmapMode.hpp"
#include "Enum/GlPolygonMode.hpp"
#include "Enum/GlPrimitiveTopology.hpp"
#include "Enum/GlQueryResultFlag.hpp"
#include "Enum/GlQueryType.hpp"
#include "Enum/GlSampleCountFlag.hpp"
#include "Enum/GlSamplerParameter.hpp"
#include "Enum/GlShaderBinaryFormat.hpp"
#include "Enum/GlShaderInfo.hpp"
#include "Enum/GlShaderStageFlag.hpp"
#include "Enum/GlStencilOp.hpp"
#include "Enum/GlTexLevelParameter.hpp"
#include "Enum/GlTexParameter.hpp"
#include "Enum/GlTextureType.hpp"
#include "Enum/GlTextureViewType.hpp"
#include "Enum/GlTextureUnit.hpp"
#include "Enum/GlTweak.hpp"
#include "Enum/GlWrapMode.hpp"

#include "Miscellaneous/GlCallLogger.hpp"
#include "Pipeline/GlSpecialisationInfo.hpp"

#include "Miscellaneous/GlDebug.hpp"

#define BufferOffset( n ) ( ( uint8_t * )nullptr + ( n ) )

namespace gl_renderer
{
	class Buffer;
	class BufferView;
	class CommandBase;
	class ComputePipeline;
	class Context;
	class DescriptorSet;
	class Device;
	class FrameBuffer;
	class GeometryBuffers;
	class PhysicalDevice;
	class Pipeline;
	class PipelineLayout;
	class QueryPool;
	class Renderer;
	class RenderPass;
	class ShaderModule;
	class ShaderProgram;
	class Texture;
	class TextureView;

	using ContextPtr = std::unique_ptr< Context >;
	using CommandPtr = std::unique_ptr< CommandBase >;
	using GeometryBuffersPtr = std::unique_ptr< GeometryBuffers >;
	using TextureViewPtr = std::unique_ptr< TextureView >;

	using GeometryBuffersRef = std::reference_wrapper< GeometryBuffers >;

	using ShaderModuleCRef = std::reference_wrapper< ShaderModule const >;

	using GeometryBuffersRefArray = std::vector< GeometryBuffersRef >;

	using ShaderModuleCRefArray = std::vector< ShaderModuleCRef >;

	using CommandArray = std::vector< CommandPtr >;

	struct BufferObjectBinding
	{
		GLuint bo;
		uint64_t offset;
		Buffer const * buffer;
	};
	using VboBindings = std::map< uint32_t, BufferObjectBinding >;
	using IboBinding = std::optional< BufferObjectBinding >;

	using BufferDestroyFunc = std::function< void( GLuint ) >;
	using BufferDestroySignal = renderer::Signal< BufferDestroyFunc >;
	using BufferDestroyConnection = renderer::SignalConnection< BufferDestroySignal >;
}
