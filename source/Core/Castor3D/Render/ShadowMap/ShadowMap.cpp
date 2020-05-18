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
		, ShadowMapPassResult result
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
		uint32_t index = 0u;
		m_result[SmTexture::eVariance].getTexture()->forEachView( [&index, &visitor, this]( TextureViewUPtr const & view )
			{
				visitor.visit( m_name + "Variance" + string::toString( index++ ), view->getView() );
			} );
		index = 0u;
		m_result[SmTexture::eLinearNormal].getTexture()->forEachView( [&index, &visitor, this]( TextureViewUPtr const & view )
			{
				visitor.visit( m_name + "Linear" + string::toString( index++ ), view->getView() );
			} );
		index = 0u;
		m_result[SmTexture::ePosition].getTexture()->forEachView( [&index, &visitor, this]( TextureViewUPtr const & view )
			{
				visitor.visit( m_name + "Position" + string::toString( index++ ), view->getView() );
			} );
		index = 0u;
		m_result[SmTexture::eFlux].getTexture()->forEachView( [&index, &visitor, this]( TextureViewUPtr const & view )
			{
				visitor.visit( m_name + "Flux" + string::toString( index++ ), view->getView() );
			} );
	}

	bool ShadowMap::initialise()
	{
		bool result = true;

		if ( !m_initialised )
		{
			for ( auto & map : m_result )
			{
				map.initialise();
			}

			auto & device = getCurrentRenderDevice( *this );

			{
				auto cmdBuffer = device.graphicsCommandPool->createCommandBuffer( m_name + "Clear" );
				cmdBuffer->begin( VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT );
				static VkClearColorValue const clearColour{ 1.0f, 1.0f, 1.0f, 1.0f };
				uint32_t index = 0u;

				for ( auto & texture : m_result )
				{
					cmdBuffer->beginDebugBlock(
						{
							m_name + getName( SmTexture( index ) ) + " clear",
							makeFloatArray( getEngine()->getNextRainbowColour() ),
						} );
					texture.getTexture()->forEachLeafView( [&cmdBuffer]( TextureViewUPtr const & view )
						{
							cmdBuffer->memoryBarrier( VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT
								, VK_PIPELINE_STAGE_TRANSFER_BIT
								, view->getView().makeTransferDestination( VK_IMAGE_LAYOUT_UNDEFINED ) );
							cmdBuffer->clear( view->getView(), clearColour );
							cmdBuffer->memoryBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT
								, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
								, view->getView().makeShaderInputResource( VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ) );
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

		for ( auto & map : m_result )
		{
			map.initialise();
		}
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

	void ShadowMap::updateFlags( PipelineFlags & flags )const
	{
		addFlag( flags.programFlags, ProgramFlag::eLighting );
		remFlag( flags.programFlags, ProgramFlag::eInvertNormals );
		remFlag( flags.passFlags, PassFlag::eAlphaBlending );
		assert( ( flags.textures & ShadowMap::TextureFlags ) == flags.textures );
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
		if ( checkFlag( flags.passFlags, PassFlag::eMetallicRoughness ) )
		{
			return doGetPbrMrPixelShaderSource( flags );
		}

		if ( checkFlag( flags.passFlags, PassFlag::eSpecularGlossiness ) )
		{
			return doGetPbrSgPixelShaderSource( flags );
		}

		return doGetPhongPixelShaderSource( flags );
	}

	ashes::Sampler const & ShadowMap::getLinearSampler( uint32_t index )const
	{
		return m_result[SmTexture::eLinearNormal].getSampler()->getSampler();
	}

	ashes::Sampler const & ShadowMap::getVarianceSampler( uint32_t index )const
	{
		return m_result[SmTexture::eVariance].getSampler()->getSampler();
	}

	ashes::ImageView const & ShadowMap::getLinearView( uint32_t index )const
	{
		return m_result[SmTexture::eLinearNormal].getTexture()->getDefaultView().getView();
	}

	ashes::ImageView const & ShadowMap::getVarianceView( uint32_t index )const
	{
		return m_result[SmTexture::eVariance].getTexture()->getDefaultView().getView();
	}

	ShaderPtr ShadowMap::doGetGeometryShaderSource( PipelineFlags const & flags )const
	{
		return ShaderPtr{};
	}
}
