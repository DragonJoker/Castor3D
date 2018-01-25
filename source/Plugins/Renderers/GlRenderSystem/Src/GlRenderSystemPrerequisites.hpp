/*
See LICENSE file in root folder
*/
#ifndef ___GL_RENDER_SYSTEM_PREREQUISITES_H___
#define ___GL_RENDER_SYSTEM_PREREQUISITES_H___

#include <CastorUtilsPrerequisites.hpp>
#include <Castor3DPrerequisites.hpp>
#include <GlRendererPrerequisites.hpp>

#ifdef CASTOR_PLATFORM_WINDOWS
#	ifdef GlRenderSystem_EXPORTS
#		define C3D_Gl_API __declspec( dllexport )
#	else
#		define C3D_Gl_API __declspec( dllimport )
#	endif
#else
#	define C3D_Gl_API
#endif

namespace GlRender
{
	enum class GlMin
		: uint32_t
	{
		eProgramTexelOffset = 0x8904,
		eMapBufferAlignment = 0x90BC,
	};

	enum class GlMax
		: uint32_t
	{
		eComputeShaderStorageBlocks = 0x90DB,
		eCombinedShaderStorageBlocks = 0x90DC,
		eComputeUniformBlocks = 0x91BB,
		eComputeTextureImageUnits = 0x91BC,
		eComputeUniformComponents = 0x8263,
		eComputeAtomicCounters = 0x8265,
		eComputeAtomicCounterBuffers = 0x8264,
		eCombinedComputeUniformComponents = 0x8266,
		eComputeWorkGroupInvocations = 0x90EB,
		eComputeWorkGroupCount = 0x91BE,
		eComputeWorkGroupSize = 0x91BF,
		eDebugGroupStackDepth = 0x826C,
		eTexture3DSize = 0x8073,
		eTextureArrayLayers = 0x88FF,
		eClipDistances = 0x0D32,
		eColorTextureSamples = 0x910E,
		eCombinedAtomicCounters = 0x92D7,
		eCombinedFragmentUniformComponents = 0x8A33,
		eCombinedGeometryUniformComponents = 0x8A32,
		eCombinedTextureImageUnits = 0x8B4D,
		eCombinedUniformBlocks = 0x8A2E,
		eCombinedVertexUniformComponents = 0x8A31,
		eTextureCubeSize = 0x851C,
		eDepthTextureSamples = 0x910F,
		eDrawBuffers = 0x8824,
		eDualSourceDrawBuffers = 0x88FC,
		eElementsIndices = 0x80E9,
		eElementsVertices = 0x80E8,
		eFragmentAtomicCounters = 0x92D6,
		eFragmentShaderStorageBlocks = 0x90DA,
		eFragmentInputComponents = 0x9125,
		eFragmentUniformComponents = 0x8B49,
		eFragmentUniformVectors = 0x8DFD,
		eFragmentUniformBlocks = 0x8A2D,
		eFramebufferWidth = 0x9315,
		eFramebufferHeight = 0x9316,
		eFramebufferLayers = 0x9317,
		eFramebufferSamples = 0x9318,
		eGeometryAtomicCounters = 0x92D5,
		eGeometryShaderStorageBlocks = 0x90D7,
		eGeometryInputComponents = 0x9123,
		eGeometryOutputComponents = 0x9124,
		eGeometryTextureImageUnits = 0x8C29,
		eGeometryUniformBlocks = 0x8A2C,
		eGeometryUniformComponents = 0x8DDF,
		eIntegerSamples = 0x9110,
		eLabelLength = 0x82E8,
		eProgramTexelOffset = 0x8905,
		eRectangleTextureSize = 0x84F8,
		eRenderbufferSize = 0x84E8,
		eSampleMaskWords = 0x8E59,
		eSamples = 0x8D57,
		eServerWaitTimeout = 0x9111,
		eShaderStorageBufferBindings = 0x90DD,
		eTessControlAtomicCounters = 0x92D3,
		eTessEvaluationAtomicCounters = 0x92D4,
		eTessControlShaderStorageBlocks = 0x90D8,
		eTessEvaluationShaderStorageBlocks = 0x90D9,
		eTextureBufferSize = 0x8C2B,
		eTextureImageUnits = 0x8872,
		eTextureLODBias = 0x84FD,
		eTextureSize = 0x0D33,
		eUniformBufferBindings = 0x8A2F,
		eUniformBlockSize = 0x8A30,
		eUniformLocations = 0x826E,
		eVaryingComponents = 0x8B4B,
		eVaryingVectors = 0x8DFC,
		eVaryingFloats = 0x8B4B,
		eVertexAtomicCounters = 0x92D2,
		eVertexAttribs = 0x8869,
		eVertexShaderStorageBlocks = 0x90D6,
		eVertexTextureImageUnits = 0x8B4C,
		eVertexUniformComponents = 0x8B4A,
		eVertexUniformVectors = 0x8DFB,
		eVertexOutputComponents = 0x9122,
		eVertexUniformBlocks = 0x8A2B,
		eViewportDims = 0x0D3A,
		eViewports = 0x825B,
	};

	enum class GlGpuInfo
	{
		eTotalAvailableMemNVX = 0x9048,
		eVBOFreeMemoryATI = 0x87FB,
		eTextureFreeMemoryATI = 0x87FC,
		eRenderbufferFreeMemoryATI = 0x87FD,
	};

	class GlRenderSystem;
}

#endif
