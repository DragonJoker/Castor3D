#include "VkRenderSystem.hpp"

#include <Log/Logger.hpp>
#include <GL/gl.h>

namespace VkRender
{
	castor::String RenderSystem::Name = cuT( "Vulkan Renderer" );
	castor::String RenderSystem::Type = cuT( "vulkan" );

	RenderSystem::RenderSystem( castor3d::Engine & engine )
		: castor3d::RenderSystem( engine, Name )
	{
	}

	RenderSystem::~RenderSystem()
	{
	}

	castor3d::RenderSystemUPtr RenderSystem::create( castor3d::Engine & engine )
	{
		return std::make_unique< RenderSystem >( engine );
	}

	void RenderSystem::doInitialise()
	{
		if ( !m_initialised )
		{
			m_gpuInformations.setShaderLanguageVersion( 460 );
			m_gpuInformations.updateFeature( castor3d::GpuFeature::eConstantsBuffers, true );
			m_gpuInformations.updateFeature( castor3d::GpuFeature::eTextureBuffers, true );
			m_gpuInformations.updateFeature( castor3d::GpuFeature::eInstancing, true );
			m_gpuInformations.updateFeature( castor3d::GpuFeature::eAccumulationBuffer, true );
			m_gpuInformations.updateFeature( castor3d::GpuFeature::eNonPowerOfTwoTextures, true );
			m_gpuInformations.updateFeature( castor3d::GpuFeature::eAtomicCounterBuffers, true );
			m_gpuInformations.updateFeature( castor3d::GpuFeature::eImmutableTextureStorage, true );
			m_gpuInformations.updateFeature( castor3d::GpuFeature::eShaderStorageBuffers, true );
			m_gpuInformations.updateFeature( castor3d::GpuFeature::eTransformFeedback, true );

			m_gpuInformations.useShaderType( renderer::ShaderStageFlag::eCompute, true );
			m_gpuInformations.useShaderType( renderer::ShaderStageFlag::eTessellationControl, true );
			m_gpuInformations.useShaderType( renderer::ShaderStageFlag::eTessellationEvaluation, true );
			m_gpuInformations.useShaderType( renderer::ShaderStageFlag::eGeometry, true );
			m_gpuInformations.useShaderType( renderer::ShaderStageFlag::eFragment, true );
			m_gpuInformations.useShaderType( renderer::ShaderStageFlag::eVertex, true );

			m_initialised = true;
		}
	}

	void RenderSystem::doCleanup()
	{
		m_initialised = false;
	}
}
