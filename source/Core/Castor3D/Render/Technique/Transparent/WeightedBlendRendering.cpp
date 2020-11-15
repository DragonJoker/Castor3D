#include "Castor3D/Render/Technique/Transparent/WeightedBlendRendering.hpp"

#include "Castor3D/Render/RenderInfo.hpp"
#include "Castor3D/Render/RenderLoop.hpp"
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

CU_ImplementCUSmartPtr( castor3d, WeightedBlendRendering )

using namespace castor;

namespace castor3d
{
	WeightedBlendRendering::WeightedBlendRendering( Engine & engine
		, RenderDevice const & device
		, TransparentPass & transparentPass
		, TextureUnit const & depthView
		, ashes::ImageView const & colourView
		, TextureUnit const & velocityTexture
		, castor::Size const & size
		, Scene const & scene
		, HdrConfigUbo const & hdrConfigUbo
		, GpInfoUbo const & gpInfoUbo )
		: m_engine{ engine }
		, m_transparentPass{ transparentPass }
		, m_size{ size }
		, m_transparentPassResult{ engine, device, depthView, velocityTexture }
		, m_finalCombinePass{ engine, device, m_size, m_transparentPass.getSceneUbo(), hdrConfigUbo, gpInfoUbo, m_transparentPassResult, colourView }
	{
		m_transparentPass.initialiseRenderPass( device, m_transparentPassResult );
		m_transparentPass.initialise( device, m_size );
	}

	void WeightedBlendRendering::update( CpuUpdater & updater )
	{
		m_transparentPass.update( updater );
	}

	void WeightedBlendRendering::update( GpuUpdater & updater )
	{
		m_transparentPass.update( updater );
	}

	ashes::Semaphore const & WeightedBlendRendering::render( RenderDevice const & device
		, Scene const & scene
		, ashes::Semaphore const & toWait )
	{
		auto * result = &toWait;
		result = &m_transparentPass.render( device, *result );
		result = &m_finalCombinePass.render( device
			, scene.getFog().getType()
			, *result );
		return *result;
	}

	void WeightedBlendRendering::accept( RenderTechniqueVisitor & visitor )
	{
		visitor.visit( "Transparent Accumulation"
			, m_transparentPassResult[WbTexture::eAccumulation].getTexture()->getDefaultView().getSampledView()
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		visitor.visit( "Transparent Revealage"
			, m_transparentPassResult[WbTexture::eRevealage].getTexture()->getDefaultView().getSampledView()
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		m_transparentPass.accept( visitor );
		m_finalCombinePass.accept( visitor );
	}
}
