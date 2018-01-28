/**
*\file
*	RenderLibPrerequisites.h
*\author
*	Sylvain Doremus
*/
#pragma once

#include <RendererPrerequisites.hpp>

#include "Miscellaneous/OpenGLFunctions.hpp"
#include "GlPixelFormat.hpp"

#include "Enum/GlAccessFlag.hpp"
#include "Enum/GlAttachmentPoint.hpp"
#include "Enum/GlAttachmentType.hpp"
#include "Enum/GlAttributeFormat.hpp"
#include "Enum/GlBaseType.hpp"
#include "Enum/GlBlendFactor.hpp"
#include "Enum/GlBlendOp.hpp"
#include "Enum/GlBorderColour.hpp"
#include "Enum/GlBufferTarget.hpp"
#include "Enum/GlClearTarget.hpp"
#include "Enum/GlClipInfo.hpp"
#include "Enum/GlColourComponentFlag.hpp"
#include "Enum/GlCompareOp.hpp"
#include "Enum/GlCullModeFlag.hpp"
#include "Enum/GlFenceWaitFlag.hpp"
#include "Enum/GlFilter.hpp"
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
#include "Enum/GlShaderInfo.hpp"
#include "Enum/GlShaderStageFlag.hpp"
#include "Enum/GlStencilOp.hpp"
#include "Enum/GlTextureType.hpp"
#include "Enum/GlTweak.hpp"
#include "Enum/GlWrapMode.hpp"

#include "Miscellaneous/GlCallLogger.hpp"

namespace gl_renderer
{
	class Buffer;
	class BufferView;
	class CommandBase;
	class Context;
	class DescriptorSet;
	class Device;
	class FrameBuffer;
	class GeometryBuffers;
	class Pipeline;
	class PipelineLayout;
	class QueryPool;
	class RenderPass;
	class ShaderProgram;
	class Texture;
	class TextureView;

	using ContextPtr = std::unique_ptr< Context >;
	using CommandPtr = std::unique_ptr< CommandBase >;

	using CommandArray = std::vector< CommandPtr >;
}
