#include "Castor3D/Render/Passes/BackgroundRenderer.hpp"

#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Miscellaneous/ProgressBar.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Background/Background.hpp"
#include "Castor3D/Shader/Ubos/HdrConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelDataUbo.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"

#include <ashes/ashes.hpp>

#include <RenderGraph/FramePassGroup.hpp>

CU_ImplementCUSmartPtr( castor3d, BackgroundRenderer )

namespace castor3d
{
	//*********************************************************************************************
	
	BackgroundRenderer::BackgroundRenderer( crg::FramePassGroup & graph
		, crg::FramePassArray previousPasses
		, RenderDevice const & device
		, ProgressBar * progress
		, castor::String const & name
		, SceneBackground & background
		, HdrConfigUbo const & hdrConfigUbo
		, SceneUbo const & sceneUbo
		, crg::ImageViewId const & colour
		, bool clearColour
		, crg::ImageViewId const * depth )
		: m_device{ device }
		, m_matrixUbo{ m_device }
		, m_modelUbo{ m_device.uboPool->getBuffer< ModelBufferConfiguration >( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT ) }
		, m_backgroundPassDesc{ &doCreatePass( graph
			, std::move( previousPasses )
			, name
			, background
			, hdrConfigUbo
			, sceneUbo
			, colour
			, clearColour
			, depth
			, progress ) }
	{
	}

	BackgroundRenderer::~BackgroundRenderer()
	{
		m_device.uboPool->putBuffer( m_modelUbo );
	}

	void BackgroundRenderer::update( CpuUpdater & updater )
	{
		if ( m_backgroundPass )
		{
			m_backgroundPass->update( updater );
		}

		m_matrixUbo.cpuUpdate( updater.bgMtxView
			, updater.bgMtxProj
			, updater.camera->getFrustum() );
		auto & configuration = m_modelUbo.getData();
		configuration.prvModel = configuration.curModel;
		configuration.curModel = updater.bgMtxModl;
	}

	void BackgroundRenderer::update( GpuUpdater & updater )
	{
		if ( m_backgroundPass )
		{
			m_backgroundPass->update( updater );
		}
	}

	crg::FramePass const & BackgroundRenderer::doCreatePass( crg::FramePassGroup & graph
		, crg::FramePassArray previousPasses
		, castor::String const & name
		, SceneBackground & background
		, HdrConfigUbo const & hdrConfigUbo
		, SceneUbo const & sceneUbo
		, crg::ImageViewId const & colour
		, bool clearColour
		, crg::ImageViewId const * depth
		, ProgressBar * progress )
	{
		stepProgressBar( progress, "Creating background pass" );
		auto size = makeExtent2D( getExtent( colour ) );
		auto & result = graph.createPass( "Background"
			, [this, &background, progress, size, depth]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				stepProgressBar( progress, "Initialising background pass" );
				auto res = background.createBackgroundPass( framePass
					, context
					, runnableGraph
					, m_device
					, size
					, depth != nullptr );
				m_backgroundPass = res.get();
				m_device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
					, res->getTimer() );
				return res;
			} );
		result.addDependencies( previousPasses );
		m_matrixUbo.createPassBinding( result
			, SceneBackground::MtxUboIdx );
		m_modelUbo.createPassBinding( result
			, "Model"
			, SceneBackground::MdlMtxUboIdx );
		hdrConfigUbo.createPassBinding( result
			, SceneBackground::HdrCfgUboIdx );
		sceneUbo.createPassBinding( result
			, SceneBackground::SceneUboIdx );
		result.addSampledView( background.getTextureId().sampledViewId
			, SceneBackground::SkyBoxImgIdx
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );

		if ( depth )
		{
			result.addInOutDepthStencilView( *depth );
		}

		if ( clearColour )
		{
			result.addOutputColourView( colour
				, transparentBlackClearColor );
		}
		else
		{
			result.addInOutColourView( colour );
		}

		return result;
	}

	//*********************************************************************************************
}
