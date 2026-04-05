#include "Castor3D/Render/EnvironmentMap/EnvironmentMapPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Viewport.hpp"
#include "Castor3D/Render/Culling/FrustumCuller.hpp"
#include "Castor3D/Render/EnvironmentMap/EnvironmentMap.hpp"
#include "Castor3D/Render/Passes/BackgroundRenderer.hpp"
#include "Castor3D/Render/Passes/ForwardRenderTechniquePass.hpp"
#include "Castor3D/Render/Ssao/SsaoConfig.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Background/Background.hpp"

#include <RenderGraph/RunnablePasses/GenerateMipmaps.hpp>
#include <RenderGraph/RunnablePasses/ImageCopy.hpp>
#include <RenderGraph/RunnableGraph.hpp>

CU_ImplementSmartPtr( c3d, EnvironmentMapPass )

namespace c3d
{
	namespace envpass
	{
		static CameraUPtr doCreateCamera( SceneNode & node
			, Extent3D const & extent )
		{
			float const aspect = float( extent.width ) / float( extent.height );
			float const nearZ = 0.1f;
			float const farZ = 1000.0f;
			Viewport viewport{ *node.getScene()->getEngine() };
			viewport.setPerspective( 90.0_degrees
				, aspect
				, nearZ
				, farZ );
			viewport.update();
			auto camera = makeUnique< Camera >( cuT( "EnvironmentMap_" ) + node.getName()
				, *node.getScene()
				, node
				, c3d::move( viewport ) );
			camera->update();
			return camera;
		}
	}

	EnvironmentMapPass::EnvironmentMapPass( RenderDevice const & device
		, EnvironmentMap & environmentMap
		, SceneNodeUPtr faceNode
		, uint32_t index
		, CubeMapFace face
		, SceneBackground & background )
		: OwnedBy< EnvironmentMap >{ environmentMap }
		, Named{ cuT( "Env" ) + environmentMap.getScene().getName() + string::toString( index ) + cuT( "/" ) + c3d::getName( face ) }
		, m_device{ device }
		, m_graph{ c3d::getEngine( device ).getGraphResourceHandler(), toUtf8( getName() ) }
		, m_background{ background }
		, m_node{ c3d::move( faceNode ) }
		, m_index{ index }
		, m_face{ face }
		, m_camera{ envpass::doCreateCamera( *m_node, getOwner()->getSize() ) }
		, m_culler{ makeUniqueDerived< SceneCuller, FrustumCuller >( *m_camera ) }
		, m_cameraUbo{ m_device }
		, m_renderUbo{ m_device }
		, m_sceneUbo{ &environmentMap.getScene().getUbo() }
		, m_colourRender{ device
			, c3d::getEngine( device ).getGraphResourceCache()
			, environmentMap.getTmpImage( m_index, m_face ) }
		, m_colourResult{ device
			, c3d::getEngine( device ).getGraphResourceCache()
			, environmentMap.getColourViewId( m_index, m_face ) }
		, m_depth{ device
			, c3d::getEngine( device ).getGraphResourceCache()
			, getOwner()->getDepthViewId( m_index, m_face ) }
		, m_backgroundRenderer{ makeUnique< BackgroundRenderer >( m_graph.getDefaultGroup()
			, m_device
			, nullptr
			, m_background
			, *m_camera
			, m_renderUbo
			, *m_sceneUbo
			, m_colourRender
			, nullptr
			, nullptr
			, true /*clearColour*/
			, true /*clearDepth*/
			, true /*forceVisible*/
			, &m_depth
			, nullptr ) }
	{
		doCreateOpaquePass();
		doCreateTransparentPass();
		doCreateGenMipmapsPass();
		m_cameraUbo.cpuUpdate( m_camera->getView()
			, m_camera->getRawProjection()
			, m_camera->getFrustum() );
		m_graph.addOutput( m_colourResult.getWholeViewId()
			, makeLayoutState( ImageLayout::eShaderReadOnly ) );
		m_runnable = m_graph.compile( m_device.makeContext() );
		environmentMap.getScene().getEngine()->registerTimer( getName(), m_runnable->getTimer() );
		printGraph( *m_runnable );
		log::trace << "Created EnvironmentMapPass " << getName() << std::endl;
	}

	EnvironmentMapPass::~EnvironmentMapPass()noexcept
	{
		if ( m_runnable )
		{
			getOwner()->getScene().getEngine()->unregisterTimer( getName(), m_runnable->getTimer() );
		}

		m_camera->getParent()->detach( true );
	}

	void EnvironmentMapPass::update( CpuUpdater & updater )
	{
		if ( !m_currentNode )
		{
			return;
		}

		auto position = m_currentNode->getDerivedPosition();
		auto & camera = *m_camera;
		camera.getParent()->setPosition( position );
		camera.getParent()->update();
		camera.update();
		m_culler->update( updater );

		auto oldCamera = updater.camera;
		auto oldSafeBanded = updater.isSafeBanded;
		updater.camera = &camera;
		updater.isSafeBanded = false;

		m_backgroundRenderer->update( updater );
		m_opaquePass->update( updater );
		m_transparentPass->update( updater );
		m_cameraUbo.cpuUpdate( camera );
		m_renderUbo.cpuUpdate( camera.getHdrConfig()
			, makeSize( getOwner()->getSize() ), false
			, updater.debugIndex );

		updater.isSafeBanded = oldSafeBanded;
		updater.camera = oldCamera;
	}

	void EnvironmentMapPass::update( GpuUpdater & updater )
	{
		if ( !m_currentNode )
		{
			return;
		}

		auto & camera = *m_camera;
		auto oldCamera = updater.camera;
		updater.camera = &camera;

		m_backgroundRenderer->update( updater );
		updater.camera = oldCamera;
	}

	void EnvironmentMapPass::record()
	{
		m_runnable->record();
	}

	SemaphoreWaitArray EnvironmentMapPass::render( SemaphoreWaitArray const & toWait
		, ashes::Queue const & queue )
	{
		return m_runnable->run( toWait, queue );
	}

	void EnvironmentMapPass::attachTo( SceneNode & node )
	{
		m_currentNode = &node;

		if ( m_opaquePass )
		{
			m_opaquePass->setIgnoredNode( node );
		}

		if ( m_transparentPass )
		{
			m_transparentPass->setIgnoredNode( node );
		}
	}

	void EnvironmentMapPass::doCreateOpaquePass()
	{
		auto & result = m_graph.createPass( "OpaquePass"
			, [this]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto res = makeRawUnique< ForwardRenderTechniquePass >( nullptr
					, framePass
					, context
					, graph
					, m_device
					, ForwardRenderTechniquePass::Type
					, cuT( "Environment" )
					, m_colourRender
					, m_depth
					, RenderNodesPassDesc{ getOwner()->getSize(), m_cameraUbo, m_renderUbo, *m_sceneUbo, *m_culler }
						.meshShading( true )
						.componentModeFlags( ForwardRenderTechniquePass::DefaultComponentFlags )
					, RenderTechniquePassDesc{ true, SsaoConfig{} }
					, nullptr );
				m_node->getScene()->getEngine()->registerTimer( makeString( framePass.getFullName() )
					, res->getTimer() );
				m_opaquePass = res.get();
				return res;
			} );
		m_depth.setLastAttach( result.addInOutDepthTarget( *m_depth.getLastAttach() ) );
		m_colourRender.setLastAttach( result.addInOutColourTarget( *m_colourRender.getLastAttach() ) );
	}

	void EnvironmentMapPass::doCreateTransparentPass()
	{
		auto & result = m_graph.createPass( "TransparentPass"
			, [this]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto res = makeRawUnique< ForwardRenderTechniquePass >( nullptr
					, framePass
					, context
					, graph
					, m_device
					, ForwardRenderTechniquePass::Type
					, cuT( "Environment" )
					, m_colourRender
					, m_depth
					, RenderNodesPassDesc{ getOwner()->getSize(), m_cameraUbo, m_renderUbo, *m_sceneUbo, *m_culler, false }
						.meshShading( true )
						.componentModeFlags( ForwardRenderTechniquePass::DefaultComponentFlags )
					, RenderTechniquePassDesc{ true, SsaoConfig{} }
					, nullptr );
				m_node->getScene()->getEngine()->registerTimer( makeString( framePass.getFullName() )
					, res->getTimer() );
				m_transparentPass = res.get();
				return res;
			} );
		result.addInputDepthTarget( *m_depth.getLastAttach() );
		m_colourRender.setLastAttach( result.addInOutColourTarget( *m_colourRender.getLastAttach() ) );
	}

	void EnvironmentMapPass::doCreateGenMipmapsPass()
	{
		auto & imgCopy = m_graph.createPass( "CopyRenderToResult"
			, [this]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = makeRawUnique< crg::ImageCopy >( framePass
					, context
					, graph
					, m_colourRender.getExtent() );
				m_node->getScene()->getEngine()->registerTimer( makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} );
		imgCopy.addInputTransfer( *m_colourRender.getLastAttach() );
		m_colourResult.setLastAttach( imgCopy.addOutputTransferImage( m_colourResult.getWholeViewId() ) );

		auto & mipsGen = m_graph.createPass( "GenMips"
			, [this]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = makeRawUnique< crg::GenerateMipmaps >( framePass
					, context
					, graph
					, ImageLayout::eShaderReadOnly );
				m_node->getScene()->getEngine()->registerTimer( makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} );
		mipsGen.addInOutTransfer( *m_colourResult.getLastAttach() );
	}
}
