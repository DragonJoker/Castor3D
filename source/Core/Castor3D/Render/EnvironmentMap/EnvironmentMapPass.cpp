#include "Castor3D/Render/EnvironmentMap/EnvironmentMapPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Material/Texture/TextureView.hpp"
#include "Castor3D/Render/RenderModule.hpp"
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
#include <RenderGraph/RunnableGraph.hpp>

namespace castor3d
{
	namespace envpass
	{
		static CameraSPtr doCreateCamera( SceneNode & node
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
			auto camera = std::make_shared< Camera >( cuT( "EnvironmentMap_" ) + node.getName()
				, *node.getScene()
				, node
				, std::move( viewport ) );
			camera->update();
			return camera;
		}
	}

	EnvironmentMapPass::EnvironmentMapPass( crg::FrameGraph & graph
		, RenderDevice const & device
		, EnvironmentMap & environmentMap
		, SceneNodeSPtr faceNode
		, uint32_t index
		, CubeMapFace face
		, SceneBackground & background )
		: OwnedBy< EnvironmentMap >{ environmentMap }
		, castor::Named{ castor3d::getName( face ) }
		, m_device{ device }
		, m_graph{ graph.createPassGroup( getName() ) }
		, m_background{ background }
		, m_node{ faceNode }
		, m_index{ index }
		, m_face{ face }
		, m_camera{ envpass::doCreateCamera( *faceNode, getOwner()->getSize() ) }
		, m_culler{ std::make_unique< FrustumCuller >( *m_camera ) }
		, m_matrixUbo{ m_device }
		, m_hdrConfigUbo{ m_device }
		, m_sceneUbo{ m_device }
		, m_colourView{ environmentMap.getColourViewId( m_index, m_face ) }
		, m_opaquePassDesc{ &doCreateOpaquePass( nullptr ) }
		, m_backgroundRenderer{ castor::makeUnique< BackgroundRenderer >( m_graph
			, m_opaquePassDesc
			, m_device
			, getName()
			, m_background
			, m_hdrConfigUbo
			, m_sceneUbo
			, m_colourView
			, false
			, getOwner()->getDepthViewId( m_index, m_face )
			, nullptr ) }
		, m_transparentPassDesc{ &doCreateTransparentPass( &m_backgroundRenderer->getPass() ) }
	{
		doCreateGenMipmapsPass( m_transparentPassDesc );
		m_matrixUbo.cpuUpdate( m_camera->getView()
			, m_camera->getProjection( false )
			, m_camera->getFrustum() );
		m_sceneUbo.setWindowSize( m_camera->getSize() );
		log::trace << "Created EnvironmentMapPass " << m_graph.getName() + "/" + getName() << std::endl;
	}

	EnvironmentMapPass::~EnvironmentMapPass()
	{
		m_camera->getParent()->detach();
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
		m_matrixUbo.cpuUpdate( camera.getView()
			, camera.getProjection( false )
			, camera.getFrustum() );
		m_hdrConfigUbo.cpuUpdate( camera.getHdrConfig() );
		m_sceneUbo.cpuUpdate( *camera.getScene(), &camera );

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
		auto depthView = getOwner()->getDepthViewId( m_index, m_face );
		auto & result = m_graph.createPass( "OpaquePass"
			, [this, depthView]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto res = std::make_unique< ForwardRenderTechniquePass >( nullptr
					, framePass
					, context
					, graph
					, m_device
					, ForwardRenderTechniquePass::Type
					, m_colourView.data->image.data
					, RenderNodesPassDesc{ getOwner()->getSize(), m_matrixUbo, m_sceneUbo, *m_culler }
						.meshShading( true )
						.implicitAction( depthView
							, crg::RecordContext::clearAttachment( depthView, defaultClearDepthStencil ) )
						.implicitAction( m_colourView
							, crg::RecordContext::clearAttachment( m_colourView, transparentBlackClearColor ) )
					, RenderTechniquePassDesc{ true, SsaoConfig{} } );
				m_node->getScene()->getEngine()->registerTimer( framePass.getFullName()
					, res->getTimer() );
				m_opaquePass = res.get();
				return res;
			} );

		if ( previousPass )
		{
			result.addDependency( *previousPass );
		}

		result.addOutputDepthView( depthView
			, defaultClearDepthStencil );
		result.addOutputColourView( m_colourView
			, transparentBlackClearColor );
		return result;
	}

	crg::FramePass & EnvironmentMapPass::doCreateTransparentPass( crg::FramePass const * previousPass )
	{
		auto & result = m_graph.createPass( "TransparentPass"
			, [this]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto res = std::make_unique< ForwardRenderTechniquePass >( nullptr
					, framePass
					, context
					, graph
					, m_device
					, ForwardRenderTechniquePass::Type
					, m_colourView.data->image.data
					, RenderNodesPassDesc{ getOwner()->getSize(), m_matrixUbo, m_sceneUbo, *m_culler, false }
						.meshShading( true )
					, RenderTechniquePassDesc{ true, SsaoConfig{} } );
				m_node->getScene()->getEngine()->registerTimer( framePass.getFullName()
					, res->getTimer() );
				m_transparentPass = res.get();
				return res;
			} );
		result.addDependency( *previousPass );
		result.addInputDepthView( getOwner()->getDepthViewId( m_index, m_face ) );
		result.addInOutColourView( m_colourView );
		return result;
	}

	void EnvironmentMapPass::doCreateGenMipmapsPass( crg::FramePass const * previousPass )
	{
		auto & mipsGen = m_graph.createPass( "GenMips"
			, [this]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = std::make_unique< crg::GenerateMipmaps >( framePass
					, context
					, graph
					, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
				m_node->getScene()->getEngine()->registerTimer( framePass.getFullName()
					, result->getTimer() );
				return result;
			} );
		mipsGen.addDependency( *previousPass );
		mipsGen.addTransferInOutView( m_colourView );
	}
}
