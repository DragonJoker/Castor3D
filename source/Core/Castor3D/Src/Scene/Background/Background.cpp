#include "Scene/Background/Background.hpp"

#include "Engine.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/Scene.hpp"
#include "Texture/Sampler.hpp"

#include <Command/CommandBufferInheritanceInfo.hpp>
#include <Core/Device.hpp>

namespace castor3d
{
	SceneBackground::SceneBackground( Engine & engine
		, Scene & scene
		, BackgroundType type )
		: OwnedBy< Engine >{ engine }
		, m_scene{ scene }
		, m_type{ type }
		, m_configUbo{ engine }
	{
	}

	SceneBackground::~SceneBackground()
	{
	}

	bool SceneBackground::initialise( renderer::RenderPass const & renderPass )
	{
		m_semaphore = getEngine()->getRenderSystem()->getCurrentDevice()->createSemaphore();
		m_configUbo.initialise();

		castor::String const name = cuT( "Skybox" );

		if ( getEngine()->getSamplerCache().has( name ) )
		{
			m_sampler = getEngine()->getSamplerCache().find( name );
		}
		else
		{
			auto sampler = getEngine()->getSamplerCache().add( name );
			sampler->setMinFilter( renderer::Filter::eLinear );
			sampler->setMagFilter( renderer::Filter::eLinear );
			sampler->setWrapS( renderer::WrapMode::eClampToEdge );
			sampler->setWrapT( renderer::WrapMode::eClampToEdge );
			sampler->setWrapR( renderer::WrapMode::eClampToEdge );
			m_sampler = sampler;
		}

		m_sampler.lock()->initialise();
		auto result = doInitialise( renderPass );

		if ( result
			&& m_scene.getMaterialsType() != MaterialType::eLegacy
			&& m_texture->getLayersCount() == 6u )
		{
			m_ibl = std::make_unique< IblTextures >( m_scene, m_texture->getTexture() );
			m_ibl->update();
		}

		return result;
	}

	void SceneBackground::cleanup()
	{
		doCleanup();

		m_configUbo.cleanup();
		m_pipeline.reset();
		m_vertexBuffer.reset();
		m_descriptorSet.reset();
		m_pipelineLayout.reset();
		m_descriptorPool.reset();
		m_descriptorLayout.reset();
		m_semaphore.reset();

		if ( m_texture )
		{
			m_texture->cleanup();
			m_texture.reset();
		}

		m_ibl.reset();
	}

	void SceneBackground::update( Camera const & camera )
	{
		if ( !m_hdr )
		{
			m_configUbo.update( m_scene.getHdrConfig() );
		}

		doUpdate( camera );
	}

	bool SceneBackground::prepareFrame( renderer::CommandBuffer & commandBuffer
		, renderer::RenderPass const & renderPass
		, renderer::FrameBuffer const & frameBuffer )
	{
		renderer::ClearColorValue colour;
		renderer::DepthStencilClearValue depth;
		auto result = commandBuffer.begin();

		if ( result )
		{
			commandBuffer.beginRenderPass( renderPass
				, frameBuffer
				, { depth, colour }
			, renderer::SubpassContents::eInline );
			commandBuffer.bindPipeline( *m_pipeline );
			commandBuffer.setViewport( { m_size.getWidth(), m_size.getHeight(), 0, 0 } );
			commandBuffer.setScissor( { 0, 0, m_size.getWidth(), m_size.getHeight() } );
			commandBuffer.bindDescriptorSet( *m_descriptorSet, *m_pipelineLayout );
			commandBuffer.bindVertexBuffer( 0u, m_vertexBuffer->getBuffer(), 0u );
			commandBuffer.draw( 36u );
			commandBuffer.endRenderPass();
			result = commandBuffer.end();
		}

		return result;
	}

	bool SceneBackground::prepareFrame( renderer::CommandBuffer & commandBuffer
		, renderer::RenderPass const & renderPass )
	{
		auto result = commandBuffer.begin(  renderer::CommandBufferUsageFlag::eRenderPassContinue
			, renderer::CommandBufferInheritanceInfo
			{
				&renderPass,
				0u,
				nullptr,
				false,
				0u,
				0u
			} );

		if ( result )
		{
			commandBuffer.bindPipeline( *m_pipeline );
			commandBuffer.setViewport( { m_size.getWidth(), m_size.getHeight(), 0, 0 } );
			commandBuffer.setScissor( { 0, 0, m_size.getWidth(), m_size.getHeight() } );
			commandBuffer.bindDescriptorSet( *m_descriptorSet, *m_pipelineLayout );
			commandBuffer.bindVertexBuffer( 0u, m_vertexBuffer->getBuffer(), 0u );
			commandBuffer.draw( 36u );
			result = commandBuffer.end();
		}

		return result;
	}
}
