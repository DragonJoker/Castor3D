#include "Castor3D/Render/ShadowMap/ShadowMap.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapPass.hpp"

#include <CastorUtils/Graphics/RgbaColour.hpp>

#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Sync/Semaphore.hpp>
#include <ashespp/Image/ImageView.hpp>

using namespace castor;

namespace castor3d
{
	ShadowMap::ShadowMap( Engine & engine
		, castor::String name
		, ShadowMapResult result
		, std::vector< PassData > passes
		, uint32_t count )
		: OwnedBy< Engine >{ engine }
		, m_name{ std::move( name ) }
		, m_result{ std::move( result ) }
		, m_passes{ std::move( passes ) }
		, m_count{ count }
	{
	}

	ShadowMap::~ShadowMap()
	{
	}

	void ShadowMap::accept( PipelineVisitorBase & visitor )
	{
		for ( uint32_t i = 1u; i < uint32_t( SmTexture::eCount ); ++i )
		{
			uint32_t index = 0u;
			m_result[SmTexture( i )].getTexture()->forEachLeafView( [&index, &visitor, this, i]( TextureViewUPtr const & view )
				{
					visitor.visit( m_name + getName( SmTexture( i ) ) + cuT( "L" ) + string::toString( index++ ), view->getSampledView() );
				} );
		}
	}

	bool ShadowMap::initialise()
	{
		bool result = true;

		if ( !m_initialised )
		{
			m_result.initialise();
			auto & device = getCurrentRenderDevice( *this );

			{
				auto cmdBuffer = device.graphicsCommandPool->createCommandBuffer( m_name + "Clear" );
				cmdBuffer->begin( VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT );
				uint32_t index = 0u;

				for ( auto & texture : m_result )
				{
					cmdBuffer->beginDebugBlock(
						{
							m_name + getName( SmTexture( index ) ) + " clear",
							makeFloatArray( getEngine()->getNextRainbowColour() ),
						} );
					texture->getTexture()->forEachLeafView( [&cmdBuffer, index]( TextureViewUPtr const & view )
						{
							cmdBuffer->memoryBarrier( VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT
								, VK_PIPELINE_STAGE_TRANSFER_BIT
								, view->getTargetView().makeTransferDestination( VK_IMAGE_LAYOUT_UNDEFINED ) );

							if ( ashes::isDepthOrStencilFormat( view->getTargetView()->format ) )
							{
								cmdBuffer->clear( view->getTargetView()
									, getClearValue( SmTexture( index ) ).depthStencil );
								cmdBuffer->memoryBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT
									, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT
									, view->getTargetView().makeDepthStencilAttachment( VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ) );
							}
							else
							{
								cmdBuffer->clear( view->getTargetView()
									, getClearValue( SmTexture( index ) ).color );
								cmdBuffer->memoryBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT
									, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
									, view->getTargetView().makeShaderInputResource( VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ) );
							}
						} );
					cmdBuffer->endDebugBlock();
					++index;
				}

				cmdBuffer->end();
				auto fence = device->createFence( m_name + "Clear" );
				device.graphicsQueue->submit( *cmdBuffer, fence.get() );
				fence->wait( ashes::MaxTimeout );
			}

			auto size = m_result[SmTexture::eVariance].getTexture()->getDimensions();

			for ( auto & pass : m_passes )
			{
				pass.matrixUbo->initialise();
				result = result && pass.pass->initialise( { size.width, size.height } );
			}

			if ( result )
			{
				m_finished = device->createSemaphore( m_name );
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

		m_initialised = false;
		doCleanup();
		m_result.cleanup();
	}

	ashes::Semaphore const & ShadowMap::render( ashes::Semaphore const & toWait
		, uint32_t index )
	{
		if ( isUpToDate( index ) )
		{
			return toWait;
		}

		return doRender( toWait, index );
	}

	ashes::VkClearValueArray const & ShadowMap::getClearValues()const
	{
		static ashes::VkClearValueArray const result
		{
			[]()
			{
				ashes::VkClearValueArray result;

				for ( uint32_t i = 0u; i < uint32_t( SmTexture::eCount ); ++i )
				{
					result.push_back( getClearValue( SmTexture( i ) ) );
				}

				return result;
			}()
		};
		return result;
	}

	ashes::Sampler const & ShadowMap::getSampler( SmTexture texture
		, uint32_t index )const
	{
		return m_result[texture].getSampler()->getSampler();
	}

	ashes::ImageView const & ShadowMap::getView( SmTexture texture
		, uint32_t index )const
	{
		return m_result[texture].getTexture()->getDefaultView().getSampledView();
	}
}
