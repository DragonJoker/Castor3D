#include "Castor3D/Render/Technique/Transparent/WeightedBlendRendering.hpp"

#include "Castor3D/Render/RenderInfo.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Render/Technique/Transparent/TransparentPassResult.hpp"
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
	WeightedBlendRendering::WeightedBlendRendering( crg::FrameGraph & graph
		, RenderDevice const & device
		, TransparentPass & transparentPass
		, TransparentPassResult const & transparentPassResult
		, ashes::ImageView const & colourView
		, castor::Size const & size
		, Scene const & scene
		, HdrConfigUbo const & hdrConfigUbo
		, GpInfoUbo const & gpInfoUbo
		, LightVolumePassResult const & lpvResult )
		: m_device{ device }
		, m_transparentPass{ transparentPass }
		, m_transparentPassResult{ transparentPassResult }
		, m_size{ size }
		, m_finalCombinePass{ m_device
			, m_size
			, m_transparentPass.getSceneUbo()
			, hdrConfigUbo
			, gpInfoUbo
			, m_transparentPassResult
			, colourView }
	{
	}

	void WeightedBlendRendering::update( CpuUpdater & updater )
	{
		m_transparentPass.update( updater );
	}

	void WeightedBlendRendering::update( GpuUpdater & updater )
	{
		m_transparentPass.update( updater );
	}

	ashes::Semaphore const & WeightedBlendRendering::render( Scene const & scene
		, ashes::Semaphore const & toWait )
	{
		auto * result = &toWait;
		//result = &m_transparentPass.render( *result );
		result = &m_finalCombinePass.render( scene.getFog().getType()
			, *result );
		return *result;
	}

	void WeightedBlendRendering::accept( RenderTechniqueVisitor & visitor )
	{
		visitor.visit( "Transparent Accumulation"
			, m_transparentPassResult[WbTexture::eAccumulation].wholeViewId
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, TextureFactors{}.invert( true ) );
		visitor.visit( "Transparent Revealage"
			, m_transparentPassResult[WbTexture::eRevealage].wholeViewId
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, TextureFactors{}.invert( true ) );
		m_transparentPass.accept( visitor );
		m_finalCombinePass.accept( visitor );
	}
}
