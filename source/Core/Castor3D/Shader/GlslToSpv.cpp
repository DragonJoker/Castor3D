/*
This file belongs to Ashes.
See LICENSE file in root folder.
*/
#include "Castor3D/Shader/GlslToSpv.hpp"
#include "Castor3D/RequiredVersion.hpp"

#if C3D_HasGlslang
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <ashespp/Core/Device.hpp>

#include <glslang/Public/ShaderLang.h>
#include <SPIRV/GlslangToSpv.h>

#include <locale>
#include <regex>

namespace castor3d
{
	namespace
	{
		struct BlockLocale
		{
			BlockLocale()
				: m_prvLoc{ std::locale( "" ) }
			{
				if ( m_prvLoc.name() != "C" )
				{
					std::locale::global( std::locale{ "C" } );
				}
			}

			~BlockLocale()
			{
				if ( m_prvLoc.name() != "C" )
				{
					std::locale::global( m_prvLoc );
				}
			}

		private:
			std::locale m_prvLoc;
		};

		void doInitResources( ashes::Device const & device
			, TBuiltInResource & resources )
		{
			auto limits = device.getPhysicalDevice().getProperties().limits;

			resources.limits.doWhileLoops = true;
			resources.limits.generalAttributeMatrixVectorIndexing = true;
			resources.limits.generalConstantMatrixVectorIndexing = true;
			resources.limits.generalSamplerIndexing = true;
			resources.limits.generalUniformIndexing = true;
			resources.limits.generalVariableIndexing = true;
			resources.limits.generalVaryingIndexing = true;
			resources.limits.nonInductiveForLoops = true;
			resources.limits.whileLoops = true;
			resources.maxAtomicCounterBindings = 1;
			resources.maxAtomicCounterBufferSize = 16384;
			resources.maxClipDistances = int( limits.maxClipDistances );
			resources.maxClipPlanes = 6;
			resources.maxCombinedAtomicCounterBuffers = 1;
			resources.maxCombinedAtomicCounters = 8;
			resources.maxCombinedClipAndCullDistances = int( limits.maxCombinedClipAndCullDistances );
			resources.maxCombinedImageUniforms = 8;
			resources.maxCombinedImageUnitsAndFragmentOutputs = 8;
			resources.maxCombinedShaderOutputResources = 8;
			resources.maxCombinedTextureImageUnits = 80;
			resources.maxComputeAtomicCounterBuffers = 1;
			resources.maxComputeAtomicCounters = 8;
			resources.maxComputeImageUniforms = 8;
			resources.maxComputeTextureImageUnits = 16;
			resources.maxComputeUniformComponents = 1024;
			resources.maxComputeWorkGroupCountX = int( limits.maxComputeWorkGroupCount[0] );
			resources.maxComputeWorkGroupCountY = int( limits.maxComputeWorkGroupCount[1] );
			resources.maxComputeWorkGroupCountZ = int( limits.maxComputeWorkGroupCount[2] );
			resources.maxComputeWorkGroupSizeX = int( limits.maxComputeWorkGroupSize[0] );
			resources.maxComputeWorkGroupSizeY = int( limits.maxComputeWorkGroupSize[1] );
			resources.maxComputeWorkGroupSizeZ = int( limits.maxComputeWorkGroupSize[2] );
			resources.maxCullDistances = int( limits.maxCullDistances );
			resources.maxDrawBuffers = int( limits.maxColorAttachments );
			resources.maxFragmentAtomicCounterBuffers = 1;
			resources.maxFragmentAtomicCounters = 8;
			resources.maxFragmentImageUniforms = 8;
			resources.maxFragmentInputComponents = int( limits.maxFragmentInputComponents );
			resources.maxFragmentInputVectors = 15;
			resources.maxFragmentUniformComponents = 4096;
			resources.maxFragmentUniformVectors = 16;
			resources.maxGeometryAtomicCounterBuffers = 0;
			resources.maxGeometryAtomicCounters = 0;
			resources.maxGeometryImageUniforms = 0;
			resources.maxGeometryInputComponents = int( limits.maxGeometryInputComponents );
			resources.maxGeometryOutputComponents = int( limits.maxGeometryOutputComponents );
			resources.maxGeometryOutputVertices = 256;
			resources.maxGeometryTextureImageUnits = 16;
			resources.maxGeometryTotalOutputComponents = int( limits.maxGeometryTotalOutputComponents );
			resources.maxGeometryUniformComponents = 1024;
			resources.maxGeometryVaryingComponents = int( limits.maxGeometryOutputVertices );
			resources.maxImageSamples = 0;
			resources.maxImageUnits = 8;
			resources.maxLights = 32;
			resources.maxPatchVertices = int( limits.maxTessellationPatchSize );
			resources.maxProgramTexelOffset = int( limits.maxTexelOffset );
			resources.maxSamples = 4;
			resources.maxTessControlAtomicCounterBuffers = 0;
			resources.maxTessControlAtomicCounters = 0;
			resources.maxTessControlImageUniforms = 0;
			resources.maxTessControlInputComponents = int( limits.maxTessellationControlPerVertexInputComponents );
			resources.maxTessControlOutputComponents = 128;
			resources.maxTessControlTextureImageUnits = 16;
			resources.maxTessControlTotalOutputComponents = int( limits.maxTessellationControlTotalOutputComponents );
			resources.maxTessControlUniformComponents = 1024;
			resources.maxTessEvaluationAtomicCounterBuffers = 0;
			resources.maxTessEvaluationAtomicCounters = 0;
			resources.maxTessEvaluationImageUniforms = 0;
			resources.maxTessEvaluationInputComponents = int( limits.maxTessellationEvaluationInputComponents );
			resources.maxTessEvaluationOutputComponents = int( limits.maxTessellationEvaluationOutputComponents );
			resources.maxTessEvaluationTextureImageUnits = 16;
			resources.maxTessEvaluationUniformComponents = 1024;
			resources.maxTessGenLevel = int( limits.maxTessellationGenerationLevel );
			resources.maxTessPatchComponents = int( limits.maxTessellationControlPerPatchOutputComponents );
			resources.maxTextureCoords = 32;
			resources.maxTextureImageUnits = int( limits.maxDescriptorSetStorageImages );
			resources.maxTextureUnits = int( limits.maxDescriptorSetSampledImages );
			resources.maxTransformFeedbackBuffers = 4;
			resources.maxTransformFeedbackInterleavedComponents = 64;
			resources.maxVaryingComponents = int( limits.maxVertexOutputComponents );
			resources.maxVaryingFloats = int( limits.maxVertexOutputComponents );
			resources.maxVaryingVectors = 8;
			resources.maxVertexAtomicCounterBuffers = 0;
			resources.maxVertexAtomicCounters = 0;
			resources.maxVertexAttribs = 64;
			resources.maxVertexImageUniforms = 0;
			resources.maxVertexOutputComponents = int( limits.maxVertexOutputComponents );
			resources.maxVertexOutputVectors = 16;
			resources.maxVertexTextureImageUnits = 32;
			resources.maxVertexUniformComponents = 4096;
			resources.maxVertexUniformVectors = 128;
			resources.maxViewports = int( limits.maxViewports );
			resources.minProgramTexelOffset = -8;
		}

		EShLanguage doGetLanguage( VkShaderStageFlagBits stage )
		{
			switch ( stage )
			{
			case VK_SHADER_STAGE_VERTEX_BIT:
				return EShLangVertex;

			case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
				return EShLangTessControl;

			case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
				return EShLangTessEvaluation;

			case VK_SHADER_STAGE_GEOMETRY_BIT:
				return EShLangGeometry;

			case VK_SHADER_STAGE_FRAGMENT_BIT:
				return EShLangFragment;

			case VK_SHADER_STAGE_COMPUTE_BIT:
				return EShLangCompute;

			default:
				assert( false && "Unsupported shader stage." );
				return EShLangVertex;
			}
		}
	}

	void initialiseGlslang()
	{
		glslang::InitializeProcess();
	}

	void cleanupGlslang()
	{
		glslang::FinalizeProcess();
	}

	UInt32Array compileGlslToSpv( RenderDevice const & device
		, VkShaderStageFlagBits stage
		, std::string const & shader )
	{
		BlockLocale guard;
		TBuiltInResource resources;
		doInitResources( *device.device, resources );

		// Enable SPIR-V and Vulkan rules when parsing GLSL
		auto messages = ( EShMessages )( EShMsgSpvRules | EShMsgVulkanRules );
		auto glstage = doGetLanguage( stage );

		std::string source = shader;
		glslang::TShader glshader{ glstage };
		char const * const str = source.c_str();
		glshader.setStrings( &str, 1 );

		if ( !glshader.parse( &resources, 100, false, messages ) )
		{
			log::error << glshader.getInfoLog() << std::endl;
			log::error << glshader.getInfoDebugLog() << std::endl;
			log::error << source << std::endl;
			CU_Exception( "Shader compilation failed." );
		}

		glslang::TProgram glprogram;
		glprogram.addShader( &glshader );

		if ( !glprogram.link( messages ) )
		{
			log::error << glprogram.getInfoLog() << std::endl;
			log::error << glprogram.getInfoDebugLog() << std::endl;
			log::error << source << std::endl;
			CU_Exception( "Shader linkage failed." );
		}

		ashes::UInt32Array spirv;
		glslang::GlslangToSpv( *glprogram.getIntermediate( glstage ), spirv );

		return spirv;
	}
}

#else

#include <CastorUtils/Exception/Exception.hpp>

namespace castor3d
{
	void initialiseGlslang()
	{
	}

	void cleanupGlslang()
	{
	}

	UInt32Array compileGlslToSpv( RenderDevice const & device
		, VkShaderStageFlagBits stage
		, std::string const & shader )
	{
		CU_Exception( "glslang is unavailable." );
	}
}

#endif
