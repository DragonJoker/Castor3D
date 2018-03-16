#include "VkRenderSystem.hpp"

#include <Log/Logger.hpp>

#include <Engine.hpp>

#include <Core/VkDevice.hpp>
#include <Core/VkRenderer.hpp>

using namespace castor;

namespace VkRender
{
	String RenderSystem::Name = cuT( "Vulkan Renderer" );
	String RenderSystem::Type = cuT( "vulkan" );

	RenderSystem::RenderSystem( castor3d::Engine & engine )
		: castor3d::RenderSystem( engine, Name )
	{
		m_renderer = std::make_unique< vk_renderer::Renderer >( renderer::Renderer::Configuration
		{
			"Castor3D",
			"Castor3D",
#ifdef NDEBUG
			false,
#else
			true,
#endif
		} );
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
			auto & device = static_cast< vk_renderer::Device const & >( getMainDevice() );
			StringStream stream;
			stream << ( device.getProperties().apiVersion >> 22 ) << cuT( "." ) << ( ( device.getProperties().apiVersion >> 12 ) & 0x0FFF );

			m_gpuInformations.setRenderer( device.getProperties().deviceName );
			m_gpuInformations.setVersion( stream.str() );
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
			m_gpuInformations.useShaderType( renderer::ShaderStageFlag::eTessellationControl, device.getFeatures().tessellationShader );
			m_gpuInformations.useShaderType( renderer::ShaderStageFlag::eTessellationEvaluation, device.getFeatures().tessellationShader );
			m_gpuInformations.useShaderType( renderer::ShaderStageFlag::eGeometry, device.getFeatures().geometryShader );
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
