#include "Castor3D/Render/Passes/BackgroundRenderer.hpp"

#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Miscellaneous/ProgressBar.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Background/Background.hpp"
#include "Castor3D/Shader/Ubos/RenderUbo.hpp"

#include <RenderGraph/FramePassGroup.hpp>

CU_ImplementSmartPtr( c3d, BackgroundRenderer )

namespace c3d
{
	//*********************************************************************************************
	
	BackgroundRenderer::BackgroundRenderer( crg::FramePassGroup & graph
		, RenderDevice const & device
		, ProgressBar * progress
		, SceneBackground & background
		, RenderUbo const & renderUbo
		, SceneUbo const & sceneUbo
		, Texture & colour
		, bool clearColour
		, bool clearDepth
		, bool forceVisible
		, Texture * depth
		, Texture const * depthObj )
		: m_device{ device }
		, m_colour{ colour }
		, m_cameraUbo{ m_device }
		, m_modelUbo{ m_device.uboPool->getBuffer< ModelBufferConfiguration >( MemoryPropertyFlags::eDeviceLocal ) }
	{
		doCreatePass( graph, background
			, renderUbo, sceneUbo, m_colour
			, clearColour, clearDepth, forceVisible
			, depth, depthObj
			, progress );
	}

	BackgroundRenderer::~BackgroundRenderer()noexcept
	{
		m_device.uboPool->putBuffer( m_modelUbo );
	}

	void BackgroundRenderer::update( CpuUpdater & updater )
	{
		if ( m_backgroundPass )
		{
			updater.targetImage = &m_colour;
			m_backgroundPass->update( updater );
			updater.targetImage = {};
		}

		m_cameraUbo.cpuUpdate( *updater.camera
			, updater.bgMtxView
			, updater.bgMtxProj );
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

	void BackgroundRenderer::doCreatePass( crg::FramePassGroup & graph
		, SceneBackground & background
		, RenderUbo const & renderUbo
		, SceneUbo const & sceneUbo
		, Texture & colour
		, bool clearColour
		, bool clearDepth
		, bool forceVisible
		, Texture * depth
		, Texture const * depthObj
		, ProgressBar * progress )
	{
		stepProgressBarLocal( progress, cuT( "Creating background pass" ) );
		auto size = makeExtent2D( colour.getExtent() );
		background.createBackgroundPass( graph
			, m_device
			, progress
			, size
			, colour
			, depth
			, depthObj
			, m_modelUbo
			, m_cameraUbo
			, renderUbo
			, sceneUbo
			, clearColour
			, clearDepth
			, forceVisible
			, m_backgroundPass );
	}

	//*********************************************************************************************
}
