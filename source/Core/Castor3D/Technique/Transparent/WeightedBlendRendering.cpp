#include "Castor3D/Technique/Transparent/WeightedBlendRendering.hpp"

#include "Castor3D/Render/RenderInfo.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Shader/PassBuffer/PassBuffer.hpp"
#include "Castor3D/Texture/TextureLayout.hpp"

#include <ashespp/Image/Image.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

using namespace castor;

namespace castor3d
{
	namespace
	{
		ashes::ImagePtr doCreateTexture( Engine & engine
			, Size const & size
			, WbTexture texture )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto & device = getCurrentRenderDevice( renderSystem );
			
			ashes::ImageCreateInfo image
			{
				0u,
				VK_IMAGE_TYPE_2D,
				getTextureFormat( texture ),
				{ size.getWidth(), size.getHeight(), 1u },
				1u,
				1u,
				VK_SAMPLE_COUNT_1_BIT,
				VK_IMAGE_TILING_OPTIMAL,
				( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
					| VK_IMAGE_USAGE_SAMPLED_BIT ),
			};
			auto result = makeImage( device
				, image
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, "AccumulationResult_" + getTextureName( texture ) );
			return result;
		}

		ashes::ImageView doCreateDepthView( Engine & engine
			, ashes::ImageView const & depthView )
		{
			ashes::ImageViewCreateInfo view
			{
				0u,
				depthView->image,
				VK_IMAGE_VIEW_TYPE_2D,
				depthView.image->getFormat(),
				VkComponentMapping{},
				{ VK_IMAGE_ASPECT_DEPTH_BIT, 0u, 1u, 0u, 1u },
			};
			return depthView.image->createView( view );
		}
	}

	WeightedBlendRendering::WeightedBlendRendering( Engine & engine
		, TransparentPass & transparentPass
		, ashes::ImageView const & depthView
		, ashes::ImageView const & colourView
		, TextureLayoutSPtr velocityTexture
		, castor::Size const & size
		, Scene const & scene
		, HdrConfigUbo & hdrConfigUbo )
		: m_engine{ engine }
		, m_transparentPass{ transparentPass }
		, m_size{ size }
		, m_depthView{ doCreateDepthView( engine, depthView ) }
		, m_accumulation{ doCreateTexture( engine, m_size, WbTexture::eAccumulation ) }
		, m_accumulationView{ m_accumulation->createView( VK_IMAGE_VIEW_TYPE_2D, m_accumulation->getFormat() ) }
		, m_revealage{ doCreateTexture( engine, m_size, WbTexture::eRevealage ) }
		, m_revealageView{ m_revealage->createView( VK_IMAGE_VIEW_TYPE_2D, m_revealage->getFormat() ) }
		, m_weightedBlendPassResult{ { m_depthView, m_accumulationView, m_revealageView, velocityTexture->getDefaultView() } }
		, m_finalCombinePass{ engine, m_size, m_transparentPass.getSceneUbo(), hdrConfigUbo, m_weightedBlendPassResult, colourView }
	{
		m_transparentPass.initialiseRenderPass( m_weightedBlendPassResult );
		m_transparentPass.initialise( m_size );
	}

	void WeightedBlendRendering::update( RenderInfo & info
		, Scene const & scene
		, Camera const & camera
		, Point2r const & jitter )
	{
		auto invView = camera.getView().getInverse().getTransposed();
		auto invProj = camera.getProjection().getInverse();
		auto invViewProj = ( camera.getProjection() * camera.getView() ).getInverse();

		m_transparentPass.getSceneUbo().update( scene, &camera );
		m_finalCombinePass.update( camera
			, invViewProj
			, invView
			, invProj );
		m_transparentPass.update( info
			, jitter );
	}

	ashes::Semaphore const & WeightedBlendRendering::render( RenderInfo & info
		, Scene const & scene
		, ashes::Semaphore const & toWait )
	{
		m_engine.setPerObjectLighting( true );
		auto * result = &toWait;
		result = &m_transparentPass.render( *result );
		result = &m_finalCombinePass.render( scene.getFog().getType()
			, *result );
		return *result;
	}

	void WeightedBlendRendering::debugDisplay()
	{
		//auto count = 2;
		//int width = int( m_size.getWidth() ) / 6;
		//int height = int( m_size.getHeight() ) / 6;
		//int left = int( m_size.getWidth() ) - width;
		//int top = int( m_size.getHeight() ) - height;
		//auto size = Size( width, height );
		//auto & context = *m_engine.getRenderSystem()->getCurrentContext();
		//auto index = 0;
		//context.renderDepth( Position{ left, top - height * index++ }, size, *m_weightedBlendPassResult[size_t( WbTexture::eDepth )]->getTexture() );
		//context.renderTexture( Position{ left, top - height * index++ }, size, *m_weightedBlendPassResult[size_t( WbTexture::eRevealage )]->getTexture() );
		//context.renderTexture( Position{ left, top - height * index++ }, size, *m_weightedBlendPassResult[size_t( WbTexture::eAccumulation )]->getTexture() );
	}

	void WeightedBlendRendering::accept( RenderTechniqueVisitor & visitor )
	{
		m_transparentPass.accept( visitor );
		m_finalCombinePass.accept( visitor );
	}
}
