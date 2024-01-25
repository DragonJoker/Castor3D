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

#include <CastorUtils/Graphics/RgbaColour.hpp>

#include <RenderGraph/RunnablePasses/GenerateMipmaps.hpp>
#include <RenderGraph/RunnablePasses/ImageCopy.hpp>
#include <RenderGraph/RunnableGraph.hpp>

CU_ImplementSmartPtr( castor3d, EnvironmentMapPass )

namespace castor3d
{
	namespace envpass
	{
		static CameraUPtr doCreateCamera( SceneNode & node
			, VkExtent3D const & size )
		{
			float const aspect = float( size.width ) / float( size.height );
			float const nearZ = 0.1f;
			float const farZ = 1000.0f;
			Viewport viewport{ *node.getScene()->getEngine() };
			viewport.setPerspective( 90.0_degrees
				, aspect
				, nearZ
				, farZ );
			viewport.resize( { size.width, size.height } );
			viewport.update();
			auto camera = castor::makeUnique< Camera >( cuT( "EnvironmentMap_" ) + node.getName()
				, *node.getScene()
				, node
				, castor::move( viewport ) );
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
		, castor::Named{ cuT( "Env" ) + environmentMap.getScene().getName() + castor::string::toString( index ) + cuT( "/" ) + castor3d::getName( face ) }
		, m_device{ device }
		, m_graph{ device.renderSystem.getEngine()->getGraphResourceHandler(), castor::toUtf8( getName() ) }
		, m_background{ background }
		, m_node{ castor::move( faceNode ) }
		, m_index{ index }
		, m_face{ face }
		, m_camera{ envpass::doCreateCamera( *m_node, getOwner()->getSize() ) }
		, m_culler{ castor::makeUniqueDerived< SceneCuller, FrustumCuller >( *m_camera ) }
		, m_cameraUbo{ m_device }
		, m_hdrConfigUbo{ m_device }
		, m_sceneUbo{ m_device }
		, m_colourRenderView{ environmentMap.getTmpImage( m_index, m_face ) }
		, m_colourResultView{ environmentMap.getColourViewId( m_index, m_face ) }
		, m_depthView{ getOwner()->getDepthViewId( m_index, m_face ) }
		, m_backgroundRenderer{ castor::makeUnique< BackgroundRenderer >( m_graph.getDefaultGroup()
			, nullptr
			, m_device
			, nullptr
			, m_background
			, m_hdrConfigUbo
			, m_sceneUbo
			, m_colourRenderView
			, true /*clearColour*/
			, true /*clearDepth*/
			, true /*forceVisible*/
			, m_depthView
			, nullptr ) }
		, m_opaquePassDesc{ &doCreateOpaquePass( &m_backgroundRenderer->getPass() ) }
		, m_transparentPassDesc{ &doCreateTransparentPass( m_opaquePassDesc ) }
	{
		doCreateGenMipmapsPass( m_transparentPassDesc );
		m_cameraUbo.cpuUpdate( getSafeBandedSize( m_camera->getSize() )
			, m_camera->getView()
			, m_camera->getProjection( false )
			, 0u
			, m_camera->getFrustum() );
		m_graph.addOutput( m_colourResultView
			, crg::makeLayoutState( VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ) );
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
		m_cameraUbo.cpuUpdate( camera, updater.debugIndex, false );
		m_hdrConfigUbo.cpuUpdate( camera.getHdrConfig() );
		m_sceneUbo.cpuUpdate( *camera.getScene() );

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

	crg::SemaphoreWaitArray EnvironmentMapPass::render( crg::SemaphoreWaitArray const & toWait
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

	crg::FramePass & EnvironmentMapPass::doCreateOpaquePass( crg::FramePass const * previousPass )
	{
		auto & result = m_graph.createPass( "OpaquePass"
			, [this]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto res = castor::make_unique< ForwardRenderTechniquePass >( nullptr
					, framePass
					, context
					, graph
					, m_device
					, ForwardRenderTechniquePass::Type
					, cuT( "Environment" )
					, crg::ImageViewIdArray{ m_colourRenderView }
					, crg::ImageViewIdArray{ m_depthView }
					, RenderNodesPassDesc{ getOwner()->getSize(), m_cameraUbo, m_sceneUbo, *m_culler }
						.meshShading( true )
						.componentModeFlags( ForwardRenderTechniquePass::DefaultComponentFlags )
					, RenderTechniquePassDesc{ true, SsaoConfig{} }
					, nullptr );
				m_node->getScene()->getEngine()->registerTimer( castor::makeString( framePass.getFullName() )
					, res->getTimer() );
				m_opaquePass = res.get();
				return res;
			} );

		if ( previousPass )
		{
			result.addDependency( *previousPass );
		}

		result.addInOutDepthView( m_depthView );
		result.addInOutColourView( m_colourRenderView );
		return result;
	}

	crg::FramePass & EnvironmentMapPass::doCreateTransparentPass( crg::FramePass const * previousPass )
	{
		auto & result = m_graph.createPass( "TransparentPass"
			, [this]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto res = castor::make_unique< ForwardRenderTechniquePass >( nullptr
					, framePass
					, context
					, graph
					, m_device
					, ForwardRenderTechniquePass::Type
					, cuT( "Environment" )
					, crg::ImageViewIdArray{ m_colourRenderView }
					, crg::ImageViewIdArray{ m_depthView }
					, RenderNodesPassDesc{ getOwner()->getSize(), m_cameraUbo, m_sceneUbo, *m_culler, false }
						.meshShading( true )
						.componentModeFlags( ForwardRenderTechniquePass::DefaultComponentFlags )
					, RenderTechniquePassDesc{ true, SsaoConfig{} }
					, nullptr );
				m_node->getScene()->getEngine()->registerTimer( castor::makeString( framePass.getFullName() )
					, res->getTimer() );
				m_transparentPass = res.get();
				return res;
			} );
		result.addDependency( *previousPass );
		result.addInputDepthView( m_depthView );
		result.addInOutColourView( m_colourRenderView );
		return result;
	}

	void EnvironmentMapPass::doCreateGenMipmapsPass( crg::FramePass const * previousPass )
	{
		auto & imgCopy = m_graph.createPass( "CopyRenderToResult"
			, [this]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = castor::make_unique< crg::ImageCopy >( framePass
					, context
					, graph
					, getExtent( m_colourRenderView ) );
				m_node->getScene()->getEngine()->registerTimer( castor::makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} );
		imgCopy.addDependency( *previousPass );
		imgCopy.addTransferInputView( m_colourRenderView );
		imgCopy.addTransferOutputView( m_colourResultView );

		auto & mipsGen = m_graph.createPass( "GenMips"
			, [this]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = castor::make_unique< crg::GenerateMipmaps >( framePass
					, context
					, graph
					, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
				m_node->getScene()->getEngine()->registerTimer( castor::makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} );
		mipsGen.addDependency( imgCopy );
		mipsGen.addTransferInOutView( m_colourResultView );
	}
}
