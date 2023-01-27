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
		, crg::ImageViewIdArray const & colour
		, bool clearColour
		, crg::ImageViewId const * depth
		, crg::ImageViewId const * depthObj )
		: m_device{ device }
		, m_colour{ colour }
		, m_matrixUbo{ m_device }
		, m_modelUbo{ m_device.uboPool->getBuffer< ModelBufferConfiguration >( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT ) }
		, m_backgroundPassDesc{ &doCreatePass( graph
			, std::move( previousPasses )
			, name
			, background
			, hdrConfigUbo
			, sceneUbo
			, m_colour
			, clearColour
			, depth
			, depthObj
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
			updater.targetImage = m_colour;
			m_backgroundPass->update( updater );
			updater.targetImage = {};
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
		, crg::ImageViewIdArray const & colour
		, bool clearColour
		, crg::ImageViewId const * depth
		, crg::ImageViewId const * depthObj
		, ProgressBar * progress )
	{
		stepProgressBar( progress, "Creating background pass" );
		auto size = makeExtent2D( getExtent( colour.front() ) );
		auto & result = background.createBackgroundPass( graph
			, m_device
			, progress
			, size
			, colour
			, depth
			, depthObj
			, m_modelUbo
			, m_matrixUbo
			, hdrConfigUbo
			, sceneUbo
			, clearColour
			, m_backgroundPass );
		result.addDependencies( previousPasses );
		return result;
	}

	//*********************************************************************************************
}
