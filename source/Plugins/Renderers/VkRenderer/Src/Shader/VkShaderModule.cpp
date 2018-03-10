/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#include "Shader/VkShaderModule.hpp"

#include "Core/VkDevice.hpp"

# if VKRENDERER_GLSL_TO_SPV
#	include <glslang/Public/ShaderLang.h>
#	include <SPIRV/GlslangToSpv.h>
#endif

#include <regex>

namespace vk_renderer
{
#if VKRENDERER_GLSL_TO_SPV

	namespace
	{
		void doInitResources( Device const & device
			, TBuiltInResource & resources )
		{
			auto & limits = device.getPhysicalDevice().getProperties().limits;

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
			resources.maxTextureImageUnits = 32;
			resources.maxTextureUnits = 32;
			resources.maxTransformFeedbackBuffers = 4;
			resources.maxTransformFeedbackInterleavedComponents = 64;
			resources.maxVaryingComponents = 60;
			resources.maxVaryingFloats = 64;
			resources.maxVaryingVectors = 8;
			resources.maxVertexAtomicCounterBuffers = 0;
			resources.maxVertexAtomicCounters = 0;
			resources.maxVertexAttribs = 64;
			resources.maxVertexImageUniforms = 0;
			resources.maxVertexOutputComponents = 64;
			resources.maxVertexOutputVectors = 16;
			resources.maxVertexTextureImageUnits = 32;
			resources.maxVertexUniformComponents = 4096;
			resources.maxVertexUniformVectors = 128;
			resources.maxViewports = int( limits.maxViewports );
			resources.minProgramTexelOffset = -8;
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

	ShaderModule::ShaderModule( Device const & device
		, renderer::ShaderStageFlag stage )
		: renderer::ShaderModule{ stage }
		, m_device{ device }
		, m_stage{ convert( stage ) }
	{
	}

	ShaderModule::~ShaderModule()
	{
		if ( m_shader != VK_NULL_HANDLE )
		{
			m_device.vkDestroyShaderModule( m_device, m_shader, nullptr );
		}
	}

	void ShaderModule::loadShader( std::string const & shader )
	{
#if VKRENDERER_GLSL_TO_SPV

		TBuiltInResource resources;
		doInitResources( m_device, resources );

		// Enable SPIR-V and Vulkan rules when parsing GLSL
		auto messages = ( EShMessages )( EShMsgSpvRules | EShMsgVulkanRules );
		auto glstage = doGetLanguage( renderer::ShaderModule::getStage() );

		std::string source = shader;

		glslang::TShader glshader{ glstage };
		std::regex regex{ R"(\#version \d*)" };
		source = std::regex_replace( source.data()
			, regex
			, "$&\n#define VULKAN 100\n" );
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
		doLoadShader( spirv.data(), uint32_t( spirv.size() * sizeof( uint32_t ) ) );

#else

		throw std::runtime_error{ "SPIR-V compilation from GLSL is not supported." };

#endif
	}

	void ShaderModule::loadShader( renderer::UInt32Array const & shader )
	{
		doLoadShader( shader.data(), uint32_t( shader.size() * sizeof( uint32_t ) ) );
	}

	void ShaderModule::loadShader( renderer::ByteArray const & shader )
	{
		doLoadShader( reinterpret_cast< uint32_t const * >( shader.data() ), uint32_t( shader.size() ) );
	}

	void ShaderModule::doLoadShader( uint32_t const * const shaderCode
		, uint32_t codeSize )
	{
		VkShaderModuleCreateInfo createInfo
		{
			VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			nullptr,
			0,
			codeSize,
			shaderCode
		};
		DEBUG_DUMP( createInfo );
		auto res = m_device.vkCreateShaderModule( m_device
			, &createInfo
			, nullptr
			, &m_shader );

		if ( !checkError( res ) )
		{
			throw std::runtime_error{ "Shader module creation failed: " + getLastError() };
		}
	}
}
