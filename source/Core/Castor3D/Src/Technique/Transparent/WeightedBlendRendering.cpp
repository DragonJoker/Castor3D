#include "WeightedBlendRendering.hpp"

#include "FrameBuffer/DepthStencilRenderBuffer.hpp"
#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "Render/RenderInfo.hpp"
#include "Texture/TextureLayout.hpp"
#include "Shader/PassBuffer/PassBuffer.hpp"

using namespace castor;

namespace castor3d
{
	WeightedBlendRendering::WeightedBlendRendering( Engine & engine
		, TransparentPass & transparentPass
		, FrameBuffer & frameBuffer
		, TextureAttachment & depthAttach
		, castor::Size const & size
		, Scene const & scene )
		: m_engine{ engine }
		, m_transparentPass{ transparentPass }
		, m_frameBuffer{ frameBuffer }
		, m_size{ size }
	{
		auto & renderSystem = *engine.getRenderSystem();
		m_weightedBlendPassFrameBuffer = renderSystem.createFrameBuffer();
		m_weightedBlendPassFrameBuffer->setClearColour( Colour::fromPredefined( PredefinedColour::eOpaqueBlack ) );
		bool result = m_weightedBlendPassFrameBuffer->initialise();

		if ( result )
		{
			m_weightedBlendPassResult[0] = std::make_unique< TextureUnit >( m_engine );
			m_weightedBlendPassResult[0]->setIndex( MinTextureIndex );
			m_weightedBlendPassResult[0]->setTexture( depthAttach.getTexture() );
			m_weightedBlendPassResult[0]->setSampler( m_engine.getLightsSampler() );
			m_weightedBlendPassResult[0]->initialise();
			m_weightedBlendPassTexAttachs[0] = m_weightedBlendPassFrameBuffer->createAttachment( depthAttach.getTexture() );

			for ( uint32_t i = uint32_t( WbTexture::eAccumulation ); i < uint32_t( WbTexture::eCount ); i++ )
			{
				auto texture = renderSystem.createTexture( TextureType::eTwoDimensions
					, AccessType::eNone
					, AccessType::eRead | AccessType::eWrite
					, getTextureFormat( WbTexture( i ) )
					, m_size );
				texture->getImage().initialiseSource();

				m_weightedBlendPassResult[i] = std::make_unique< TextureUnit >( m_engine );
				m_weightedBlendPassResult[i]->setIndex( MinTextureIndex + i );
				m_weightedBlendPassResult[i]->setTexture( texture );
				m_weightedBlendPassResult[i]->setSampler( m_engine.getLightsSampler() );
				m_weightedBlendPassResult[i]->initialise();

				m_weightedBlendPassTexAttachs[i] = m_weightedBlendPassFrameBuffer->createAttachment( texture );
			}

			m_weightedBlendPassFrameBuffer->bind();

			for ( int i = 0; i < size_t( WbTexture::eCount ) && result; i++ )
			{
				m_weightedBlendPassFrameBuffer->attach( getTextureAttachmentPoint( WbTexture( i ) )
					, getTextureAttachmentIndex( WbTexture( i ) )
					, m_weightedBlendPassTexAttachs[i]
					, m_weightedBlendPassResult[i]->getType() );
			}

			ENSURE( m_weightedBlendPassFrameBuffer->isComplete() );
			m_weightedBlendPassFrameBuffer->setDrawBuffers();
			m_weightedBlendPassFrameBuffer->unbind();
		}

		if ( result )
		{
			m_finalCombinePass = std::make_unique< FinalCombinePass >( *renderSystem.getEngine()
				, m_size );
		}

		ENSURE( result );
	}

	WeightedBlendRendering::~WeightedBlendRendering()
	{
		m_weightedBlendPassFrameBuffer->bind();
		m_weightedBlendPassFrameBuffer->detachAll();
		m_weightedBlendPassFrameBuffer->unbind();
		m_weightedBlendPassFrameBuffer->cleanup();

		for ( auto & attach : m_weightedBlendPassTexAttachs )
		{
			attach.reset();
		}

		m_weightedBlendPassResult[0].reset();

		for ( uint32_t i = uint32_t( WbTexture::eAccumulation ); i < uint32_t( WbTexture::eCount ); i++ )
		{
			m_weightedBlendPassResult[i]->cleanup();
			m_weightedBlendPassResult[i].reset();
		}

		m_weightedBlendPassFrameBuffer.reset();
	}

	void WeightedBlendRendering::render( RenderInfo & info
		, Scene const & scene
		, Camera const & camera
		, ShadowMapLightTypeArray & shadowMaps
		, Point2r const & jitter
		, TextureUnit const & velocity )
	{
		static Colour accumClear = Colour::fromPredefined( PredefinedColour::eTransparentBlack );
		static Colour revealClear = Colour::fromPredefined( PredefinedColour::eOpaqueWhite );
		auto invView = camera.getView().getInverse().getTransposed();
		auto invProj = camera.getViewport().getProjection().getInverse();
		auto invViewProj = ( camera.getViewport().getProjection() * camera.getView() ).getInverse();

		m_engine.setPerObjectLighting( true );
		camera.apply();

		// Accumulate blend.
		m_weightedBlendPassFrameBuffer->bind( FrameBufferTarget::eDraw );
		m_weightedBlendPassTexAttachs[size_t( WbTexture::eAccumulation )]->clear( accumClear );
		m_weightedBlendPassTexAttachs[size_t( WbTexture::eRevealage )]->clear( revealClear );
		m_transparentPass.render( info
			, shadowMaps
			, jitter );
		m_weightedBlendPassFrameBuffer->unbind();

		m_finalCombinePass->render( m_weightedBlendPassResult
			, m_frameBuffer
			, camera
			, invViewProj
			, invView
			, invProj
			, scene.getFog() );
	}

	void WeightedBlendRendering::debugDisplay()
	{
		auto count = 2;
		int width = int( m_size.getWidth() ) / 6;
		int height = int( m_size.getHeight() ) / 6;
		int left = int( m_size.getWidth() ) - width;
		int top = int( m_size.getHeight() ) - height;
		auto size = Size( width, height );
		auto & context = *m_engine.getRenderSystem()->getCurrentContext();
		auto index = 0;
		context.renderDepth( Position{ left, top - height * index++ }, size, *m_weightedBlendPassResult[size_t( WbTexture::eDepth )]->getTexture() );
		context.renderTexture( Position{ left, top - height * index++ }, size, *m_weightedBlendPassResult[size_t( WbTexture::eRevealage )]->getTexture() );
		context.renderTexture( Position{ left, top - height * index++ }, size, *m_weightedBlendPassResult[size_t( WbTexture::eAccumulation )]->getTexture() );
	}
}
