#include "Castor3D/Render/ShadowMap/ShadowMap.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapPass.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include <CastorUtils/Graphics/RgbaColour.hpp>

#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Sync/Semaphore.hpp>
#include <ashespp/Image/ImageView.hpp>

CU_ImplementCUSmartPtr( castor3d, ShadowMap )

namespace castor3d
{
	ShadowMap::ShadowMap( RenderDevice const & device
		, Scene & scene
		, LightType lightType
		, ShadowMapResult result
		, uint32_t count )
		: OwnedBy< Engine >{ *scene.getEngine() }
		, m_device{ device }
		, m_scene{ scene }
		, m_lightType{ lightType }
		, m_name{ cuT( "ShadowMap" ) + castor::string::snakeToCamelCase( getName( lightType ) ) }
		, m_result{ std::move( result ) }
		, m_count{ count }
		, m_finished{ m_device->createSemaphore( m_name ) }
	{
	}

	ShadowMap::~ShadowMap()
	{
		m_finished.reset();
		m_passes.clear();
		m_initialised = false;
		m_result.cleanup();
	}

	void ShadowMap::accept( PipelineVisitorBase & visitor )
	{
		if ( m_initialised )
		{
			for ( uint32_t i = 1u; i < uint32_t( SmTexture::eCount ); ++i )
			{
				uint32_t index = 0u;

				if ( visitor.config.forceMiplevelsVisit )
				{
					m_result[SmTexture( i )].getTexture()->forEachLeafView( [&index, &visitor, this, i]( TextureViewUPtr const & view )
						{
							visitor.visit( m_name + getName( SmTexture( i ) ) + cuT( "L" ) + castor::string::toString( index++ )
								, view->getSampledView()
								, ( ashes::isDepthOrStencilFormat( view->getTargetView()->format )
									? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
									: VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ) );
						} );
				}
				else
				{
					m_result[SmTexture( i )].getTexture()->forEachView( [&index, &visitor, this, i]( TextureViewUPtr const & view )
						{
							visitor.visit( m_name + getName( SmTexture( i ) ) + cuT( "L" ) + castor::string::toString( index++ )
								, view->getSampledView()
								, ( ashes::isDepthOrStencilFormat( view->getTargetView()->format )
									? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
									: VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ) );
						} );
				}
			}
		}
	}

	bool ShadowMap::initialise()
	{
		if ( !m_initialised
			&& m_scene.hasShadows( m_lightType ) )
		{
			m_result.initialise( m_device );
			{
				auto cmdBuffer = m_device.graphicsCommandPool->createCommandBuffer( m_name + "Clear" );
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
				auto fence = m_device->createFence( m_name + "Clear" );
				m_device.graphicsQueue->submit( *cmdBuffer, fence.get() );
				fence->wait( ashes::MaxTimeout );
			}

			doInitialise( m_device );
			m_initialised = true;
		}

		return m_initialised;
	}

	ashes::Semaphore const & ShadowMap::render( RenderDevice const & device
		, ashes::Semaphore const & toWait
		, uint32_t index )
	{
#if !C3D_MeasureShadowMapImpact
		if ( isUpToDate( index ) )
		{
			return toWait;
		}
#endif

		return doRender( device, toWait, index );
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
