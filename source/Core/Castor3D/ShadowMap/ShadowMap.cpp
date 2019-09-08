#include "Castor3D/ShadowMap/ShadowMap.hpp"

#include "Castor3D/Engine.hpp"

#include "Castor3D/Event/Frame/FunctorEvent.hpp"
#include "Castor3D/Mesh/Submesh.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelMatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelUbo.hpp"
#include "Castor3D/Shader/Ubos/MorphingUbo.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"
#include "Castor3D/Shader/Ubos/SkinningUbo.hpp"
#include "Castor3D/ShadowMap/ShadowMapPass.hpp"
#include "Castor3D/Texture/TextureLayout.hpp"
#include "Castor3D/Texture/TextureUnit.hpp"

#include <ashespp/Image/Image.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"

using namespace castor;

namespace castor3d
{
	ShadowMap::ShadowMap( Engine & engine
		, TextureUnit && shadowMap
		, TextureUnit && linearMap
		, std::vector< PassData > && passes
		, uint32_t count )
		: OwnedBy< Engine >{ engine }
		, m_shadowMap{ std::move( shadowMap ) }
		, m_linearMap{ std::move( linearMap ) }
		, m_passes{ std::move( passes ) }
		, m_count{ count }
	{
	}

	ShadowMap::~ShadowMap()
	{
	}

	bool ShadowMap::initialise()
	{
		bool result = true;

		if ( !m_initialised )
		{
			m_shadowMap.initialise();
			m_linearMap.initialise();
			auto & renderSystem = *getEngine()->getRenderSystem();
			auto & device = *renderSystem.getCurrentRenderDevice();

			{
				auto cmdBuffer = device.graphicsCommandPool->createCommandBuffer();
				cmdBuffer->begin( VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT );
				static VkClearColorValue const clearColour{ 1.0f, 1.0f, 1.0f, 1.0f };

				for ( auto & view : *m_shadowMap.getTexture() )
				{
					cmdBuffer->memoryBarrier( VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT
						, VK_PIPELINE_STAGE_TRANSFER_BIT
						, view->getView().makeTransferDestination( VK_IMAGE_LAYOUT_UNDEFINED
							, 0u ) );
					cmdBuffer->clear( view->getView(), clearColour );
					cmdBuffer->memoryBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT
						, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
						, view->getView().makeShaderInputResource( VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
							, 0u ) );
				}

				for ( auto & view : *m_linearMap.getTexture() )
				{
					cmdBuffer->memoryBarrier( VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT
						, VK_PIPELINE_STAGE_TRANSFER_BIT
						, view->getView().makeTransferDestination( VK_IMAGE_LAYOUT_UNDEFINED
							, 0u ) );
					cmdBuffer->clear( view->getView(), clearColour );
					cmdBuffer->memoryBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT
						, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
						, view->getView().makeShaderInputResource( VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
							, 0u ) );
				}

				cmdBuffer->end();
				auto fence = device->createFence();
				device.graphicsQueue->submit( *cmdBuffer, fence.get() );
				fence->wait( ashes::MaxTimeout );
			}

			auto size = m_shadowMap.getTexture()->getDimensions();

			for ( auto & pass : m_passes )
			{
				pass.matrixUbo->initialise();
				result &= pass.pass->initialise( { size.width, size.height } );
			}

			if ( result )
			{
				m_finished = device->createSemaphore();
				doInitialise();
				m_initialised = true;
			}
		}

		return result;
	}

	void ShadowMap::cleanup()
	{
		m_finished.reset();

		for ( auto & pass : m_passes )
		{
			pass.pass->cleanup();
			pass.matrixUbo->cleanup();
		}

		if ( m_initialised )
		{
			m_initialised = false;
			doCleanup();
			m_shadowMap.cleanup();
			m_linearMap.cleanup();
		}
	}

	void ShadowMap::updateFlags( PipelineFlags & flags )const
	{
		remFlag( flags.programFlags, ProgramFlag::eLighting );
		remFlag( flags.passFlags, PassFlag::eAlphaBlending );
		remFlag( flags.textures, TextureFlag::eAllButOpacity );
		flags.texturesCount = checkFlag( flags.textures, TextureFlag::eOpacity )
			? 1u
			: 0u;
		doUpdateFlags( flags );
	}

	ShaderPtr ShadowMap::getVertexShaderSource( PipelineFlags const & flags )const
	{
		return doGetVertexShaderSource( flags );
	}

	ShaderPtr ShadowMap::getGeometryShaderSource( PipelineFlags const & flags )const
	{
		return doGetGeometryShaderSource( flags );
	}

	ShaderPtr ShadowMap::getPixelShaderSource( PipelineFlags const & flags )const
	{
		return doGetPixelShaderSource( flags );
	}

	ashes::Sampler const & ShadowMap::getLinearSampler()const
	{
		return m_linearMap.getSampler()->getSampler();
	}

	ashes::Sampler const & ShadowMap::getVarianceSampler()const
	{
		return m_shadowMap.getSampler()->getSampler();
	}

	ashes::ImageView const & ShadowMap::getLinearView()const
	{
		return m_linearMap.getTexture()->getDefaultView();
	}

	ashes::ImageView const & ShadowMap::getVarianceView()const
	{
		return m_shadowMap.getTexture()->getDefaultView();
	}

	ashes::ImageView const & ShadowMap::getLinearView( uint32_t index )const
	{
		return m_linearMap.getTexture()->getDefaultView();
	}

	ashes::ImageView const & ShadowMap::getVarianceView( uint32_t index )const
	{
		return m_shadowMap.getTexture()->getDefaultView();
	}

	ShaderPtr ShadowMap::doGetGeometryShaderSource( PipelineFlags const & flags )const
	{
		return ShaderPtr{};
	}
}
