#include "Castor3D/Miscellaneous/LoadingScreen.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Cache/ObjectCache.hpp"
#include "Castor3D/Cache/OverlayCache.hpp"
#include "Castor3D/Cache/CacheView.hpp"
#include "Castor3D/Event/Frame/CpuFunctorEvent.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Miscellaneous/ProgressBar.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Overlay/TextOverlay.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/Culling/FrustumCuller.hpp"
#include "Castor3D/Render/Overlays/OverlayPass.hpp"
#include "Castor3D/Render/Passes/BackgroundRenderer.hpp"
#include "Castor3D/Render/Passes/ForwardRenderTechniquePass.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Background/Background.hpp"
#include "Castor3D/Shader/Program.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>

#include <RenderGraph/RunnableGraph.hpp>

#include <ShaderWriter/Source.hpp>

CU_ImplementSmartPtr( castor3d, LoadingScreen )

namespace castor3d
{
	namespace loadscreen
	{
		static Texture createTexture( RenderDevice const & device
			, crg::ResourcesCache & resources
			, std::string const & name
			, castor::Size const & size
			, VkFormat format
			, VkImageUsageFlags usage )
		{
			auto result = Texture{ device
				, resources
				, name
				, 0u
				, makeExtent3D( size )
				, 1u
				, 1u
				, format
				, usage
				, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK };
			result.create();
			return result;
		}

		static Texture createColour( RenderDevice const & device
			, crg::ResourcesCache & resources
			, std::string const & name
			, castor::Size const & size )
		{
			return createTexture( device
				, resources
				, name + "Col"
				, size
				, VK_FORMAT_R8G8B8A8_UNORM
				, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
					| VK_IMAGE_USAGE_SAMPLED_BIT ) );
		}

		static Texture createDepth( RenderDevice const & device
			, crg::ResourcesCache & resources
			, std::string const & name
			, castor::Size const & size )
		{
			return createTexture( device
				, resources
				, name + "Dpt"
				, size
				, device.selectSuitableDepthFormat( VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT )
				, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT );
		}

		static CameraRPtr createCamera( Scene & scene
			, castor::Size const & size )
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
				viewport.resize( size );
				viewport.update();
				auto camera = castor::makeUnique< Camera >( LoadingScreen::SceneName
					, scene
					, *scene.getCameraRootNode()
					, std::move( viewport ) );
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

		static SceneUbo createSceneUbo( RenderDevice const & device
			, castor::Size const & size )
		{
			return SceneUbo{ device };
		}

		static ShaderPtr getVertexProgram( Engine & engine )
		{
			using namespace sdw;
			VertexWriter writer{ &engine.getShaderAllocator() };

			// Shader inputs
			auto position = writer.declInput< Vec2 >( "position", 0u );

			writer.implementMainT< VoidT, VoidT >( [&]( VertexIn in
					, VertexOut out )
				{
					out.vtx.position = vec4( position, 0.0_f, 1.0_f );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		static ShaderPtr getPixelProgram( Engine & engine )
		{
			using namespace sdw;
			FragmentWriter writer{ &engine.getShaderAllocator() };

			// Shader inputs
			auto c3d_source = writer.declCombinedImg< FImg2DRgba32 >( "c3d_source", 0u, 0u );

			// Shader outputs
			auto fragColor = writer.declOutput< Vec4 >( "fragColor", 0 );

			writer.implementMainT< VoidT, VoidT >( [&]( FragmentIn in
				, FragmentOut out )
				{
					fragColor = c3d_source.fetch( ivec2( in.fragCoord.xy() ), 0_i );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
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

	LoadingScreen::WindowPass::WindowPass( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, VkRenderPass renderPass
		, VkExtent2D const & renderSize )
		: crg::RunnablePass{ pass
			, context
			, graph
			, { [this]( uint32_t index ) { doInitialise(); }
				, crg::getDefaultV< crg::RunnablePass::GetPipelineStateCallback >()
				, [this]( crg::RecordContext & context, VkCommandBuffer cmd, uint32_t i ){ doRecordInto( context, cmd, i ); } }
			, { 1u, true } }
		, m_renderSize{ renderSize }
		, m_renderPass{ renderPass }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, SceneName, loadscreen::getVertexProgram( *device.renderSystem.getEngine() ) }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, SceneName, loadscreen::getPixelProgram( *device.renderSystem.getEngine() ) }
		, m_stages{ makeShaderState( device, m_vertexShader )
			, makeShaderState( device, m_pixelShader ) }
		, m_renderQuad{ pass
			, context
			, graph
			, crg::rq::Config{}
				.program( crg::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) )
			, 1u }
	{
	}

	void LoadingScreen::WindowPass::setRenderPass( VkRenderPass renderPass
		, VkExtent2D const & renderSize )
	{
		m_renderSize = renderSize;
		m_renderPass = renderPass;
		m_framebuffer = VkFramebuffer{};

		if ( m_renderQuad.isInitialised() )
		{
			m_renderQuad.resetRenderPass( m_renderSize
				, m_renderPass
				, RenderNodesPass::createBlendState( BlendMode::eNoBlend, BlendMode::eNoBlend, 1u )
				, 0u );
			reRecordCurrent();
		}
	}

	void LoadingScreen::WindowPass::setTarget( ashes::FrameBuffer const & framebuffer
		, std::vector< VkClearValue > clearValues )
	{
		m_framebuffer = framebuffer;
		m_renderSize = framebuffer.getDimensions();
		m_clearValues = std::move( clearValues );
	}

	void LoadingScreen::WindowPass::doInitialise()
	{
	}

	void LoadingScreen::WindowPass::doRecordInto( crg::RecordContext & context
		, VkCommandBuffer commandBuffer
		, uint32_t index )
	{
		if ( m_renderPass && m_framebuffer )
		{
			if ( !m_renderQuad.isInitialised() )
			{
				m_renderQuad.initialise( *this
					, m_renderSize
					, m_renderPass
					, RenderNodesPass::createBlendState( BlendMode::eNoBlend, BlendMode::eNoBlend, 1u )
					, index );
			}

			auto beginInfo = makeVkStruct< VkRenderPassBeginInfo >( m_renderPass
				, m_framebuffer
				, VkRect2D{ {}, m_renderSize }
				, uint32_t( m_clearValues.size() )
				, m_clearValues.data() );
			m_context.vkCmdBeginRenderPass( commandBuffer
				, &beginInfo
				, VK_SUBPASS_CONTENTS_INLINE );
			m_renderQuad.record( context, commandBuffer, index );
			m_context.vkCmdEndRenderPass( commandBuffer );
		}
	}

	//*********************************************************************************************

	castor::String const LoadingScreen::SceneName = cuT( "C3D_LoadingScreen" );

	LoadingScreen::LoadingScreen( ProgressBar & progressBar
		, RenderDevice const & device
		, crg::ResourcesCache & resources
		, SceneRPtr scene
		, VkRenderPass renderPass
		, castor::Size const & size )
		: m_device{ device }
		, m_progressBar{ progressBar }
		, m_graph{ resources.getHandler(), SceneName }
		, m_scene{ std::move( scene ) }
		, m_background{ *m_scene->getBackground() }
		, m_renderPass{ renderPass }
		, m_renderSize{ size }
		, m_camera{ loadscreen::createCamera( *m_scene, m_renderSize ) }
		, m_culler{ castor::makeUniqueDerived< SceneCuller, FrustumCuller >( *m_camera ) }
		, m_colour{ loadscreen::createColour( m_device, resources, SceneName, m_renderSize ) }
		, m_depth{ loadscreen::createDepth( m_device, resources, SceneName, m_renderSize ) }
		, m_cameraUbo{ m_device }
		, m_hdrConfigUbo{ m_device }
		, m_sceneUbo{ loadscreen::createSceneUbo( m_device, m_renderSize ) }
		, m_backgroundRenderer{ castor::makeUnique< BackgroundRenderer >( m_graph.getDefaultGroup()
			, nullptr
			, m_device
			, nullptr
			, SceneName
			, *m_scene->getBackground()
			, m_hdrConfigUbo
			, m_sceneUbo
			, m_colour.targetViewId
			, true /*clearColour*/ ) }
		, m_opaquePassDesc{ &doCreateOpaquePass( &m_backgroundRenderer->getPass() ) }
		, m_transparentPassDesc{ &doCreateTransparentPass( m_opaquePassDesc ) }
		, m_overlayPassDesc{ &doCreateOverlayPass( m_transparentPassDesc ) }
		, m_windowPassDesc{ &doCreateWindowPass( m_overlayPassDesc ) }
		, m_runnable{ loadscreen::createRunnableGraph( m_graph, m_device ) }
	{
	}

	LoadingScreen::~LoadingScreen()
	{
		m_runnable.reset();
		m_backgroundRenderer.reset();
		m_culler.reset();
		m_camera = {};
		m_depth.destroy();
		m_colour.destroy();
	}

	void LoadingScreen::enable()
	{
		m_enabled = true;
	}

	void LoadingScreen::disable()
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
			updater.camera = m_camera;
			updater.scene = m_scene;
			updater.isSafeBanded = false;

			m_scene->update( updater );
			m_camera->update();

			m_culler->update( updater );
			m_cameraUbo.cpuUpdate( *m_camera, 0u, false );
			m_hdrConfigUbo.cpuUpdate( m_camera->getHdrConfig() );
			m_sceneUbo.cpuUpdate( *m_scene );

			m_backgroundRenderer->update( updater );
			m_opaquePass->update( updater );
			m_transparentPass->update( updater );
			m_overlayPass->update( updater );

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
			updater.camera = m_camera;
			updater.scene = m_scene;

			m_scene->update( updater );
			m_backgroundRenderer->update( updater );
			m_overlayPass->update( updater );

			updater.camera = oldCamera;
			updater.scene = oldScene;
		}
	}

	void LoadingScreen::upload( UploadData & uploader )
	{
		m_overlayPass->upload( uploader );
	}

	void LoadingScreen::setRenderPass( VkRenderPass renderPass
		, castor::Size const & renderSize )
	{
		m_renderPass = renderPass;
		m_renderSize = renderSize;
		m_camera->getViewport().resize( m_renderSize );

		if ( m_windowPass )
		{
			m_windowPass->setRenderPass( renderPass
				,  makeExtent2D( m_renderSize ) );
		}
	}

	crg::SemaphoreWaitArray LoadingScreen::render( ashes::Queue const & queue
		, ashes::FrameBuffer const & framebuffer
		, crg::SemaphoreWaitArray const & toWait
		, crg::Fence *& fence )
	{
		auto result = toWait;

		if ( m_enabled )
		{
			m_windowPass->resetCommandBuffer( m_windowPass->getIndex() );
			m_windowPass->setTarget( framebuffer
				, { transparentBlackClearColor } );
			m_windowPass->reRecordCurrent();
			result = { m_runnable->run( result, queue ) };
			fence = &m_windowPass->getFence();
		}

		return result;
	}

	crg::FramePass & LoadingScreen::doCreateOpaquePass( crg::FramePass const * previousPass )
	{
		auto & result = m_graph.getDefaultGroup().createPass( "Opaque"
			, [this]( crg::FramePass const & pass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = std::make_unique< ForwardRenderTechniquePass >( nullptr
					, pass
					, context
					, graph
					, m_device
					, ForwardRenderTechniquePass::Type
					, cuT( "LoadingScreen" )
					, crg::ImageViewIdArray{ m_colour.targetViewId }
					, crg::ImageViewIdArray{ m_depth.targetViewId }
					, RenderNodesPassDesc{ makeExtent3D( m_camera->getSize() ), m_cameraUbo, m_sceneUbo, *m_culler }
						.meshShading( true )
						.componentModeFlags( ForwardRenderTechniquePass::DefaultComponentFlags )
					, RenderTechniquePassDesc{ true, SsaoConfig{} } );
				m_opaquePass = result.get();
				return result;
			} );
		result.addDependency( *previousPass );
		result.addOutputDepthView( m_depth.targetViewId
			, defaultClearDepthStencil );
		result.addInOutColourView( m_colour.targetViewId );
		return result;
	}

	crg::FramePass & LoadingScreen::doCreateTransparentPass( crg::FramePass const * previousPass )
	{
		auto & result = m_graph.getDefaultGroup().createPass( "Transparent"
			, [this]( crg::FramePass const & pass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = std::make_unique< ForwardRenderTechniquePass >( nullptr
					, pass
					, context
					, graph
					, m_device
					, ForwardRenderTechniquePass::Type
					, cuT( "LoadingScreen" )
					, crg::ImageViewIdArray{ m_colour.targetViewId }
					, crg::ImageViewIdArray{ m_depth.targetViewId }
					, RenderNodesPassDesc{ makeExtent3D( m_camera->getSize() ), m_cameraUbo, m_sceneUbo, *m_culler, false }
						.meshShading( true )
						.componentModeFlags( ForwardRenderTechniquePass::DefaultComponentFlags )
					, RenderTechniquePassDesc{ true, SsaoConfig{} } );
				m_transparentPass = result.get();
				return result;
			} );
		result.addDependency( *previousPass );
		result.addInputDepthView( m_depth.targetViewId );
		result.addInOutColourView( m_colour.targetViewId );
		return result;
	}

	crg::FramePass & LoadingScreen::doCreateOverlayPass( crg::FramePass const * previousPass )
	{
		auto & result = m_graph.getDefaultGroup().createPass( "Overlay"
			, [this]( crg::FramePass const & pass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = std::make_unique< OverlayPass >( pass
					, context
					, graph
					, m_device
					, *m_scene
					, makeExtent2D( m_colour.getExtent() )
					, m_colour );
				m_overlayPass = result.get();
				return result;
			} );
		result.addDependency( *previousPass );
		result.addInOutColourView( m_colour.targetViewId );
		return result;
	}

	crg::FramePass & LoadingScreen::doCreateWindowPass( crg::FramePass const * previousPass )
	{
		auto & result = m_graph.getDefaultGroup().createPass( "Window"
			, [this]( crg::FramePass const & pass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = std::make_unique< WindowPass >( pass
					, context
					, graph
					, m_device
					, m_renderPass
					, makeExtent2D( m_renderSize ) );
				m_windowPass = result.get();
				return result;
			} );
		result.addDependency( *previousPass );
		result.addSampledView( m_colour.sampledViewId, 0u );
		return result;
	}
}
