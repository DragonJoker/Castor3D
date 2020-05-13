#include "Castor3D/Render/Technique/Transparent/WeightedBlendRendering.hpp"

#include "Castor3D/Render/RenderInfo.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Fog.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Shader/PassBuffer/PassBuffer.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"

#include <ashespp/Image/Image.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

using namespace castor;

namespace castor3d
{
	WeightedBlendRendering::WeightedBlendRendering( Engine & engine
		, TransparentPass & transparentPass
		, ashes::ImageView const & depthView
		, ashes::ImageView const & colourView
		, TextureLayoutSPtr velocityTexture
		, castor::Size const & size
		, Scene const & scene
		, HdrConfigUbo & hdrConfigUbo
		, GpInfoUbo const & gpInfoUbo )
		: m_engine{ engine }
		, m_transparentPass{ transparentPass }
		, m_size{ size }
		, m_transparentPassResult{ engine, *depthView.image, *velocityTexture->getDefaultView().image }
		, m_finalCombinePass{ engine, m_size, m_transparentPass.getSceneUbo(), hdrConfigUbo, gpInfoUbo, m_transparentPassResult, colourView }
	{
		m_transparentPass.initialiseRenderPass( m_transparentPassResult );
		m_transparentPass.initialise( m_size );
	}

	void WeightedBlendRendering::update( RenderInfo & info
		, Scene const & scene
		, Camera const & camera
		, castor::Point2f const & jitter )
	{
		auto invView = camera.getView().getInverse().getTransposed();
		auto invProj = camera.getProjection().getInverse();
		auto invViewProj = ( camera.getProjection() * camera.getView() ).getInverse();

		m_transparentPass.getSceneUbo().update( scene, &camera );
		m_transparentPass.update( info
			, jitter );
	}

	ashes::Semaphore const & WeightedBlendRendering::render( Scene const & scene
		, ashes::Semaphore const & toWait )
	{
		m_engine.setPerObjectLighting( true );
		auto * result = &toWait;
		result = &m_transparentPass.render( *result );
		result = &m_finalCombinePass.render( scene.getFog().getType()
			, *result );
		return *result;
	}

	void WeightedBlendRendering::accept( RenderTechniqueVisitor & visitor )
	{
		visitor.visit( "Transparent Accumulation", m_transparentPassResult.getViews()[uint32_t( WbTexture::eAccumulation )] );
		visitor.visit( "Transparent Revealage", m_transparentPassResult.getViews()[uint32_t( WbTexture::eRevealage )] );
		m_transparentPass.accept( visitor );
		m_finalCombinePass.accept( visitor );
	}
}
