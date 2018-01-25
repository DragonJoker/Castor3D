#include "GlRenderSystem.hpp"

#include <Log/Logger.hpp>

#include <Engine.hpp>

#include <Core/GlDevice.hpp>
#include <Core/GlRenderer.hpp>

using namespace castor3d;
using namespace castor;

namespace GlRender
{
	String GlRenderSystem::Name = cuT( "OpenGL Renderer" );
	String GlRenderSystem::Type = cuT( "opengl" );

	GlRenderSystem::GlRenderSystem( Engine & engine )
		: RenderSystem( engine, Name )
	{
		m_renderer = std::make_unique< gl_renderer::Renderer >();
	}

	GlRenderSystem::~GlRenderSystem()
	{
	}

	RenderSystemUPtr GlRenderSystem::create( Engine & engine )
	{
		return std::make_unique< GlRenderSystem >( engine );
	}

	void GlRenderSystem::doInitialise()
	{
		if ( !m_initialised )
		{
			auto & device = static_cast< gl_renderer::Device const & >( getMainDevice() );
			m_gpuInformations.setVendor( device.getVendor() );
			m_gpuInformations.setRenderer( device.getRenderer() );
			m_gpuInformations.setVersion( device.getVersionString() );
			m_gpuInformations.setShaderLanguageVersion( device.getGlslVersion() );
			m_gpuInformations.updateFeature( GpuFeature::eConstantsBuffers, true );
			m_gpuInformations.updateFeature( GpuFeature::eTextureBuffers, true );
			m_gpuInformations.updateFeature( GpuFeature::eInstancing, true );
			m_gpuInformations.updateFeature( GpuFeature::eAccumulationBuffer, true );
			m_gpuInformations.updateFeature( GpuFeature::eNonPowerOfTwoTextures, true );
			m_gpuInformations.updateFeature( GpuFeature::eAtomicCounterBuffers, true );
			m_gpuInformations.updateFeature( GpuFeature::eImmutableTextureStorage, true );
			m_gpuInformations.updateFeature( GpuFeature::eShaderStorageBuffers, true );
			m_gpuInformations.updateFeature( GpuFeature::eTransformFeedback, true );

			m_gpuInformations.useShaderType( renderer::ShaderStageFlag::eCompute, true );
			m_gpuInformations.useShaderType( renderer::ShaderStageFlag::eTessellationControl, true );
			m_gpuInformations.useShaderType( renderer::ShaderStageFlag::eTessellationEvaluation, true );
			m_gpuInformations.useShaderType( renderer::ShaderStageFlag::eGeometry, true );
			m_gpuInformations.useShaderType( renderer::ShaderStageFlag::eFragment, true );
			m_gpuInformations.useShaderType( renderer::ShaderStageFlag::eVertex, true );
			m_initialised = true;
		}
	}

	void GlRenderSystem::doCleanup()
	{
		m_initialised = false;
	}
}
