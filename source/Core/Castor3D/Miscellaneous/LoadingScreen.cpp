#include "Castor3D/Miscellaneous/LoadingScreen.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/OverlayCache.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Gui/ControlsManager.hpp"
#include "Castor3D/Miscellaneous/ProgressBar.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Culling/FrustumCuller.hpp"
#include "Castor3D/Render/Overlays/OverlayPass.hpp"
#include "Castor3D/Render/Passes/BackgroundRenderer.hpp"
#include "Castor3D/Render/Passes/ForwardRenderTechniquePass.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>

#include <RenderGraph/RunnableGraph.hpp>

CU_ImplementSmartPtr( c3d, LoadingScreen )

namespace c3d
{
	namespace loadscreen
	{
		static Texture createTexture( RenderDevice const & device
			, crg::ResourcesCache & resources
			, String const & name
			, Size const & size
			, PixelFormat format
			, ImageUsageFlags usage )
		{
			TextureSamplerCreateInfo samplerInfo{ BorderColour::eFloatOpaqueBlack };
			TextureCreateInfo createInfo{ ImageCreateFlags::eNone
				, makeExtent3D( size ), 1u, 1u
				, format, usage };
			Texture result{ device
				, resources
				, name
				, createInfo
				, TextureSamplerInfo{ samplerInfo } };
			result.create();
			return result;
		}

		static Texture createColour( RenderDevice const & device
			, crg::ResourcesCache & resources
			, String const & name
			, Size const & size
			, PixelFormat format )
		{
			return createTexture( device
				, resources
				, name + cuT( "Col" )
				, size
				, format
				, ( ImageUsageFlags::eColorAttachment
					| ImageUsageFlags::eSampled ) );
		}

		static Texture createDepth( RenderDevice const & device
			, crg::ResourcesCache & resources
			, String const & name
			, Size const & size )
		{
			return createTexture( device
				, resources
				, name + cuT( "Dpt" )
				, size
				, device.selectSuitableDepthFormat( VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT )
				, ImageUsageFlags::eDepthStencilAttachment );
		}

		static CameraRPtr createCamera( Scene & scene
			, Size const & size )
		{
			CameraRPtr result{};

			if ( scene.getCameraCache().isEmpty() )
			{
				float const aspect = float( size.getWidth() ) / float( size.getHeight() );
				float const nearZ = 0.1f;
				float const farZ = 1000.0f;
				Viewport viewport{ *scene.getEngine() };
				viewport.setPerspective( 90.0_degrees
					, aspect
					, nearZ
					, farZ );
				viewport.update();
				auto camera = makeUnique< Camera >( LoadingScreen::SceneName
					, scene
					, *scene.getCameraRootNode()
					, c3d::move( viewport ) );
				result = scene.addCamera( LoadingScreen::SceneName
					, camera );
				result->update();
			}
			else
			{
				result = scene.getCameraCache().begin()->second.get();
			}

			return result;
		}

		static crg::RunnableGraphPtr createRunnableGraph( crg::FrameGraph & graph
			, RenderDevice const & device )
		{
			auto result = graph.compile( device.makeContext() );
			auto runnable = result.get();
			printGraph( *result );
			device.renderSystem.getEngine()->postEvent( makeGpuFunctorEvent( GpuEventType::ePreUpload
				, [runnable]( RenderDevice const &
					, QueueData const & )
				{
					runnable->record();
				} ) );
			return result;
		}
	}

	//*********************************************************************************************

	String const LoadingScreen::SceneName = cuT( "C3D_LoadingScreen" );

	LoadingScreen::LoadingScreen( ProgressBar & progressBar
		, RenderDevice const & device
		, crg::ResourcesCache & resources
		, SceneRPtr scene
		, VkRenderPass renderPass
		, Size const & size )
		: m_device{ device }
		, m_progressBar{ progressBar }
		, m_graph{ makeRawUnique< crg::FrameGraph >( resources.getHandler(), toUtf8( SceneName ) ) }
		, m_scene{ c3d::move( scene ) }
		, m_background{ *m_scene->getBackground() }
		, m_renderPass{ renderPass }
		, m_initialRenderSize{ size }
		, m_renderSize{ size }
		, m_camera{ loadscreen::createCamera( *m_scene, m_renderSize ) }
		, m_culler{ makeUniqueDerived< SceneCuller, FrustumCuller >( *m_camera ) }
		, m_colour{ loadscreen::createColour( m_device, resources, SceneName, m_initialRenderSize, m_swapchainFormat ) }
		, m_depth{ loadscreen::createDepth( m_device, resources, SceneName, m_initialRenderSize ) }
		, m_cameraUbo{ m_device }
		, m_renderUbo{ m_device }
		, m_sceneUbo{ &scene->getUbo() }
		, m_backgroundRenderer{ makeUnique< BackgroundRenderer >( m_graph->getDefaultGroup()
			, m_device
			, nullptr
			, *m_scene->getBackground()
			, m_renderUbo
			, *m_sceneUbo
			, m_colour
			, true /*clearColour*/ ) }
	{
		doCreateOpaquePass();
		doCreateTransparentPass();
		doCreateOverlayPass();
		m_runnable = loadscreen::createRunnableGraph( *m_graph, m_device );
		m_device.renderSystem.getEngine()->getControlsManager()->setSize( m_renderSize );
	}

	LoadingScreen::~LoadingScreen()noexcept
	{
		m_runnable.reset();
		m_backgroundRenderer.reset();
		m_culler.reset();
		m_camera = {};
		m_depth.destroy();
		m_colour.destroy();
	}

	void LoadingScreen::enable()noexcept
	{
		m_enabled = true;
	}

	void LoadingScreen::disable()noexcept
	{
		m_enabled = false;
	}

	void LoadingScreen::update( CpuUpdater & updater )
	{
		if ( m_enabled )
		{
			auto oldCamera = updater.camera;
			auto oldScene = updater.scene;
			auto oldSafeBanded = updater.isSafeBanded;
			auto oldRenderSize = updater.renderSize;
			updater.renderSize = m_renderSize;
			updater.camera = m_camera;
			updater.scene = m_scene;
			updater.isSafeBanded = false;

			m_scene->update( updater );
			m_camera->update();

			m_culler->update( updater );
			m_cameraUbo.cpuUpdate( *m_camera );
			m_renderUbo.cpuUpdate( m_camera->getHdrConfig(), m_renderSize, false, 0u );

			m_backgroundRenderer->update( updater );
			m_opaquePass->update( updater );
			m_transparentPass->update( updater );
			m_overlayPass->update( updater );

			updater.renderSize = oldRenderSize;
			updater.isSafeBanded = oldSafeBanded;
			updater.camera = oldCamera;
			updater.scene = oldScene;
		}
	}

	void LoadingScreen::update( GpuUpdater & updater )
	{
		if ( m_enabled )
		{
			auto oldCamera = updater.camera;
			auto oldScene = updater.scene;
			auto oldRenderSize = updater.renderSize;
			updater.renderSize = m_renderSize;
			updater.camera = m_camera;
			updater.scene = m_scene;

			m_scene->update( updater );
			m_backgroundRenderer->update( updater );
			m_overlayPass->update( updater );

			updater.renderSize = oldRenderSize;
			updater.camera = oldCamera;
			updater.scene = oldScene;
		}
	}

	void LoadingScreen::upload( UploadData & uploader )
	{
		m_overlayPass->upload( uploader );
	}

	void LoadingScreen::setRenderPass( VkRenderPass renderPass
		, Size const & renderSize
		, PixelFormat swapchainFormat )
	{
		m_renderPass = renderPass;
		m_renderSize = renderSize;
		m_device.renderSystem.getEngine()->getControlsManager()->setSize( m_renderSize );

		if ( m_swapchainFormat != swapchainFormat )
		{
			m_swapchainFormat = swapchainFormat;
			m_needsRecreate = true;
		}
	}

	void LoadingScreen::record()
	{
		if ( m_enabled
			&& m_needsRecreate.exchange( false ) )
		{
			auto & resources = *m_colour.resources;

			m_runnable.reset();
			m_backgroundRenderer.reset();
			m_colour.destroy();
			m_graph.reset();

			m_graph = makeRawUnique< crg::FrameGraph >( resources.getHandler(), toUtf8( SceneName ) );
			m_colour = loadscreen::createColour( m_device, resources, SceneName, m_initialRenderSize, m_swapchainFormat );
			m_colour.create();
			m_backgroundRenderer = makeUnique< BackgroundRenderer >( m_graph->getDefaultGroup()
				, m_device
				, nullptr
				, *m_scene->getBackground()
				, m_renderUbo
				, *m_sceneUbo
				, m_colour
				, true /*clearColour*/ );
			doCreateOpaquePass();
			doCreateTransparentPass();
			doCreateOverlayPass();
			m_runnable = loadscreen::createRunnableGraph( *m_graph, m_device );
		}
	}

	SemaphoreWaitArray LoadingScreen::render( ashes::Queue const & queue
		, SemaphoreWaitArray const & toWait )
	{
		auto result = toWait;
		if ( m_enabled )
			result = m_runnable->run( result, queue );
		return result;
	}

	void LoadingScreen::doCreateOpaquePass()
	{
		auto & pass = m_graph->getDefaultGroup().createPass( "Opaque"
			, [this]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = makeRawUnique< ForwardRenderTechniquePass >( nullptr
					, framePass
					, context
					, graph
					, m_device
					, ForwardRenderTechniquePass::Type
					, cuT( "LoadingScreen" )
					, m_colour
					, m_depth
					, RenderNodesPassDesc{ makeExtent3D( m_renderSize ), m_cameraUbo, m_renderUbo, *m_sceneUbo, *m_culler }
						.meshShading( true )
						.componentModeFlags( ForwardRenderTechniquePass::DefaultComponentFlags )
					, RenderTechniquePassDesc{ true, SsaoConfig{} } );
				m_opaquePass = result.get();
				return result;
			} );
		m_depth.setLastAttach( pass.addOutputDepthTarget( m_depth.getTargetViewId()
			, defaultClearDepthStencil ) );
		m_colour.setLastAttach( pass.addInOutColourTarget( *m_colour.getLastAttach() ) );
	}

	void LoadingScreen::doCreateTransparentPass()
	{
		auto & pass = m_graph->getDefaultGroup().createPass( "Transparent"
			, [this]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = makeRawUnique< ForwardRenderTechniquePass >( nullptr
					, framePass
					, context
					, graph
					, m_device
					, ForwardRenderTechniquePass::Type
					, cuT( "LoadingScreen" )
					, m_colour
					, m_depth
					, RenderNodesPassDesc{ makeExtent3D( m_renderSize ), m_cameraUbo, m_renderUbo, *m_sceneUbo, *m_culler, false }
						.meshShading( true )
						.componentModeFlags( ForwardRenderTechniquePass::DefaultComponentFlags )
					, RenderTechniquePassDesc{ true, SsaoConfig{} } );
				m_transparentPass = result.get();
				return result;
			} );
		pass.addInputDepthTarget( *m_depth.getLastAttach() );
		m_colour.setLastAttach( pass.addInOutColourTarget( *m_colour.getLastAttach() ) );
	}

	void LoadingScreen::doCreateOverlayPass()
	{
		auto & pass = m_graph->getDefaultGroup().createPass( "Overlay"
			, [this]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = makeRawUnique< OverlayPass >( framePass
					, context
					, graph
					, m_device
					, *m_scene
					, makeExtent2D( m_colour.getExtent() )
					, m_colour
					, m_renderUbo );
				m_overlayPass = result.get();
				return result;
			} );
		m_colour.setLastAttach( pass.addInOutColourTarget( *m_colour.getLastAttach() ) );
	}
}
