/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "Shader/VkShaderProgram.hpp"

#include "Core/VkDevice.hpp"

# if VKRENDERER_GLSL_TO_SPV
#	include <glslang/Public/ShaderLang.h>
#	include <SPIRV/GlslangToSpv.h>
#endif

namespace vk_renderer
{

#if VKRENDERER_GLSL_TO_SPV

	namespace
	{
		void doInitResources( TBuiltInResource & resources )
		{
			resources.maxLights = 32;
			resources.maxClipPlanes = 6;
			resources.maxTextureUnits = 32;
			resources.maxTextureCoords = 32;
			resources.maxVertexAttribs = 64;
			resources.maxVertexUniformComponents = 4096;
			resources.maxVaryingFloats = 64;
			resources.maxVertexTextureImageUnits = 32;
			resources.maxCombinedTextureImageUnits = 80;
			resources.maxTextureImageUnits = 32;
			resources.maxFragmentUniformComponents = 4096;
			resources.maxDrawBuffers = 32;
			resources.maxVertexUniformVectors = 128;
			resources.maxVaryingVectors = 8;
			resources.maxFragmentUniformVectors = 16;
			resources.maxVertexOutputVectors = 16;
			resources.maxFragmentInputVectors = 15;
			resources.minProgramTexelOffset = -8;
			resources.maxProgramTexelOffset = 7;
			resources.maxClipDistances = 8;
			resources.maxComputeWorkGroupCountX = 65535;
			resources.maxComputeWorkGroupCountY = 65535;
			resources.maxComputeWorkGroupCountZ = 65535;
			resources.maxComputeWorkGroupSizeX = 1024;
			resources.maxComputeWorkGroupSizeY = 1024;
			resources.maxComputeWorkGroupSizeZ = 64;
			resources.maxComputeUniformComponents = 1024;
			resources.maxComputeTextureImageUnits = 16;
			resources.maxComputeImageUniforms = 8;
			resources.maxComputeAtomicCounters = 8;
			resources.maxComputeAtomicCounterBuffers = 1;
			resources.maxVaryingComponents = 60;
			resources.maxVertexOutputComponents = 64;
			resources.maxGeometryInputComponents = 64;
			resources.maxGeometryOutputComponents = 128;
			resources.maxFragmentInputComponents = 128;
			resources.maxImageUnits = 8;
			resources.maxCombinedImageUnitsAndFragmentOutputs = 8;
			resources.maxCombinedShaderOutputResources = 8;
			resources.maxImageSamples = 0;
			resources.maxVertexImageUniforms = 0;
			resources.maxTessControlImageUniforms = 0;
			resources.maxTessEvaluationImageUniforms = 0;
			resources.maxGeometryImageUniforms = 0;
			resources.maxFragmentImageUniforms = 8;
			resources.maxCombinedImageUniforms = 8;
			resources.maxGeometryTextureImageUnits = 16;
			resources.maxGeometryOutputVertices = 256;
			resources.maxGeometryTotalOutputComponents = 1024;
			resources.maxGeometryUniformComponents = 1024;
			resources.maxGeometryVaryingComponents = 64;
			resources.maxTessControlInputComponents = 128;
			resources.maxTessControlOutputComponents = 128;
			resources.maxTessControlTextureImageUnits = 16;
			resources.maxTessControlUniformComponents = 1024;
			resources.maxTessControlTotalOutputComponents = 4096;
			resources.maxTessEvaluationInputComponents = 128;
			resources.maxTessEvaluationOutputComponents = 128;
			resources.maxTessEvaluationTextureImageUnits = 16;
			resources.maxTessEvaluationUniformComponents = 1024;
			resources.maxTessPatchComponents = 120;
			resources.maxPatchVertices = 32;
			resources.maxTessGenLevel = 64;
			resources.maxViewports = 16;
			resources.maxVertexAtomicCounters = 0;
			resources.maxTessControlAtomicCounters = 0;
			resources.maxTessEvaluationAtomicCounters = 0;
			resources.maxGeometryAtomicCounters = 0;
			resources.maxFragmentAtomicCounters = 8;
			resources.maxCombinedAtomicCounters = 8;
			resources.maxAtomicCounterBindings = 1;
			resources.maxVertexAtomicCounterBuffers = 0;
			resources.maxTessControlAtomicCounterBuffers = 0;
			resources.maxTessEvaluationAtomicCounterBuffers = 0;
			resources.maxGeometryAtomicCounterBuffers = 0;
			resources.maxFragmentAtomicCounterBuffers = 1;
			resources.maxCombinedAtomicCounterBuffers = 1;
			resources.maxAtomicCounterBufferSize = 16384;
			resources.maxTransformFeedbackBuffers = 4;
			resources.maxTransformFeedbackInterleavedComponents = 64;
			resources.maxCullDistances = 8;
			resources.maxCombinedClipAndCullDistances = 8;
			resources.maxSamples = 4;
			resources.limits.nonInductiveForLoops = true;
			resources.limits.whileLoops = true;
			resources.limits.doWhileLoops = true;
			resources.limits.generalUniformIndexing = true;
			resources.limits.generalAttributeMatrixVectorIndexing = true;
			resources.limits.generalVaryingIndexing = true;
			resources.limits.generalSamplerIndexing = true;
			resources.limits.generalVariableIndexing = true;
			resources.limits.generalConstantMatrixVectorIndexing = true;
		}

		EShLanguage doGetLanguage( renderer::ShaderStageFlag stage )
		{
			switch ( stage )
			{
			case renderer::ShaderStageFlag::eVertex:
				return EShLangVertex;

			case renderer::ShaderStageFlag::eTessellationControl:
				return EShLangTessControl;

			case renderer::ShaderStageFlag::eTessellationEvaluation:
				return EShLangTessEvaluation;

			case renderer::ShaderStageFlag::eGeometry:
				return EShLangGeometry;

			case renderer::ShaderStageFlag::eFragment:
				return EShLangFragment;

			case renderer::ShaderStageFlag::eCompute:
				return EShLangCompute;

			default:
				assert( false && "Unsupported shader stage." );
				return EShLangVertex;
			}
		}
	}

#endif

	ShaderProgram::ShaderProgram( Device const & device )
		: renderer::ShaderProgram{ device }
		, m_device{ device }
	{
	}

	void ShaderProgram::createModule( std::string const & shader
		, renderer::ShaderStageFlag stage )
	{
#if VKRENDERER_GLSL_TO_SPV

		TBuiltInResource resources;
		doInitResources( resources );

		// Enable SPIR-V and Vulkan rules when parsing GLSL
		auto messages = ( EShMessages )( EShMsgSpvRules | EShMsgVulkanRules );
		auto glstage = doGetLanguage( stage );

		std::string source = shader;

		glslang::TShader glshader{ glstage };
		char const * const str = source.c_str();
		glshader.setStrings( &str, 1 );

		if ( !glshader.parse( &resources, 100, false, messages ) )
		{
			std::cerr << glshader.getInfoLog() << std::endl;
			std::cerr << glshader.getInfoDebugLog() << std::endl;
			throw std::runtime_error{ "Shader compilation failed." };
		}

		glslang::TProgram glprogram;
		glprogram.addShader( &glshader );

		if ( !glprogram.link( messages ) )
		{
			std::cerr << glprogram.getInfoLog() << std::endl;
			std::cerr << glprogram.getInfoDebugLog() << std::endl;
			throw std::runtime_error{ "Shader linkage failed." };
		}

		renderer::UInt32Array spirv;
		glslang::GlslangToSpv( *glprogram.getIntermediate( glstage ), spirv );
		m_modules.emplace_back( m_device, spirv, stage );

#else

		throw std::runtime_error{ "SPIR-V compilation from GLSL is not supported." };

#endif
	}

	void ShaderProgram::createModule( renderer::ByteArray const & fileData
		, renderer::ShaderStageFlag stage )
	{
		m_modules.emplace_back( m_device, fileData, stage );
	}
}
