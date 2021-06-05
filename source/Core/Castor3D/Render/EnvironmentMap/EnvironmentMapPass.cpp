#include "Castor3D/Render/EnvironmentMap/EnvironmentMapPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Material/Texture/TextureView.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/Viewport.hpp"
#include "Castor3D/Render/Culling/FrustumCuller.hpp"
#include "Castor3D/Render/EnvironmentMap/EnvironmentMap.hpp"
#include "Castor3D/Render/Passes/BackgroundPass.hpp"
#include "Castor3D/Render/Technique/ForwardRenderTechniquePass.hpp"
#include "Castor3D/Render/Ssao/SsaoConfig.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Background/Background.hpp"

#include <CastorUtils/Graphics/RgbaColour.hpp>

using namespace castor;

namespace castor3d
{
	namespace
	{
		CameraSPtr doCreateCamera( SceneNode & node )
		{
			Viewport viewport{ *node.getScene()->getEngine() };
			auto camera = std::make_shared< Camera >( cuT( "EnvironmentMap_" ) + node.getName()
				, *node.getScene()
				, node
				, std::move( viewport ) );
			camera->update();
			return camera;
		}
	}

	EnvironmentMapPass::EnvironmentMapPass( crg::FrameGraph & graph
		, crg::FramePass const *& previousPass
		, RenderDevice const & device
		, EnvironmentMap & environmentMap
		, SceneNodeSPtr faceNode
		, SceneNode const & objectNode
		, CubeMapFace face
		, RenderPassTimer & timer
		, SceneBackground & background )
		: OwnedBy< EnvironmentMap >{ environmentMap }
		, Named{ "Env" + objectNode.getName() + castor::string::toString( uint32_t( face ) ) }
		, m_device{ device }
		, m_graph{ graph }
		, m_background{ background }
		, m_node{ faceNode }
		, m_face{ face }
		, m_camera{ doCreateCamera( *faceNode ) }
		, m_culler{ std::make_unique< FrustumCuller >( *m_camera->getScene(), *m_camera ) }
		, m_matrixUbo{ m_device }
		, m_modelUbo{ m_device.uboPools->getBuffer< ModelUboConfiguration >( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT ) }
		, m_hdrConfigUbo{ m_device }
		, m_colourView{ environmentMap.getColourViewId( m_face ) }
		, m_backgroundPassDesc{ &doCreateBackgroundPass( previousPass, objectNode ) }
		, m_opaquePassDesc{ &doCreateOpaquePass( previousPass, objectNode ) }
		, m_transparentPassDesc{ &doCreateTransparentPass( previousPass, objectNode ) }
		, m_mtxView{ m_camera->getView() }
	{
		auto size = getOwner()->getSize();
		float const aspect = float( size.width ) / size.height;
		float const nearZ = 0.1f;
		float const farZ = 1000.0f;
		m_camera->getViewport().setPerspective( 90.0_degrees
			, aspect
			, nearZ
			, farZ );
		m_camera->resize( { size.width, size.height } );

		static castor::Matrix4x4f const projection = convert( m_device->perspective( ( 45.0_degrees ).radians()
			, 1.0f
			, 0.0f, 2.0f ) );
		m_matrixUbo.cpuUpdate( m_mtxView, projection );
		log::trace << "Created EnvironmentMapPass " << getName() << std::endl;
	}

	void EnvironmentMapPass::update( CpuUpdater & updater )
	{
		auto & node = *updater.node;
		auto position = node.getDerivedPosition();
		m_camera->getParent()->setPosition( position );
		m_camera->getParent()->update();
		m_camera->update();
		castor::matrix::setTranslate( m_mtxModel, position );
		castor::matrix::scale( m_mtxModel, castor::Point3f{ 1, -1, 1 } );
		m_culler->compute();
		static_cast< RenderTechniquePass & >( *m_opaquePass ).update( updater );
		static_cast< RenderTechniquePass & >( *m_transparentPass ).update( updater );
		m_matrixUbo.cpuUpdate( m_camera->getView()
			, m_camera->getProjection() );
		auto & configuration = m_modelUbo.getData();
		configuration.prvModel = configuration.curModel;
		configuration.curModel = m_mtxModel;
		m_hdrConfigUbo.cpuUpdate( m_camera->getHdrConfig() );
	}

	void EnvironmentMapPass::update( GpuUpdater & updater )
	{
		RenderInfo info;
		m_opaquePass->update( updater );
		m_transparentPass->update( updater );
	}

	crg::FramePass & EnvironmentMapPass::doCreateBackgroundPass( crg::FramePass const *& previousPass
		, SceneNode const & objectNode )
	{
		auto & result = m_graph.createPass( getName() + "BackgroundPass"
			, [this, &objectNode]( crg::FramePass const & pass
				, crg::GraphContext const & context
				, crg::RunnableGraph & graph )
			{
				auto result = std::make_unique< BackgroundPass >( pass
					, context
					, graph
					, m_device
					, m_background
					, makeExtent2D( getExtent( m_colourView ) )
					, false );
				m_backgroundPass = result.get();
				return result;
			} );
		previousPass = &result;
		m_matrixUbo.createPassBinding( result
			, SceneBackground::MtxUboIdx );
		m_modelUbo.createPassBinding( result
			, SceneBackground::MdlMtxUboIdx );
		m_hdrConfigUbo.createPassBinding( result
			, SceneBackground::HdrCfgUboIdx );
		result.addOutputColourView( m_colourView
			, opaqueBlackClearColor );
		return result;
	}

	crg::FramePass & EnvironmentMapPass::doCreateOpaquePass( crg::FramePass const *& previousPass
		, SceneNode const & objectNode )
	{
		auto & result = m_graph.createPass( getName() + "OpaquePass"
			, [this, &objectNode]( crg::FramePass const & pass
				, crg::GraphContext const & context
				, crg::RunnableGraph & graph )
			{
				auto result = std::make_unique< ForwardRenderTechniquePass >( pass
					, context
					, graph
					, m_device
					, cuT( "EnvironmentMap" )
					, getName() + cuT( "Opaque" )
					, SceneRenderPassDesc{ getOwner()->getSize(), m_matrixUbo, *m_culler }
						.ignored( objectNode )
					, RenderTechniquePassDesc{ true, SsaoConfig{} } );
				m_opaquePass = result.get();
				return result;
			} );
		result.addDependency( *previousPass );
		previousPass = &result;
		result.addOutputDepthView( getOwner()->getDepthViewId( m_face )
			, defaultClearDepthStencil );
		result.addInOutColourView( m_colourView );
		return result;
	}

	crg::FramePass & EnvironmentMapPass::doCreateTransparentPass( crg::FramePass const *& previousPass
		, SceneNode const & objectNode )
	{
		auto & result = m_graph.createPass( getName() + "TransparentPass"
			, [this, &objectNode]( crg::FramePass const & pass
				, crg::GraphContext const & context
				, crg::RunnableGraph & graph )
			{
				auto result = std::make_unique< ForwardRenderTechniquePass >( pass
					, context
					, graph
					, m_device
					, cuT( "EnvironmentMap" )
					, getName() + cuT( "Transparent" )
					, SceneRenderPassDesc{ getOwner()->getSize(), m_matrixUbo, *m_culler, false }
						.ignored( objectNode )
					, RenderTechniquePassDesc{ true, SsaoConfig{} } );
				m_transparentPass = result.get();
				return result;
			} );
		result.addDependency( *previousPass );
		previousPass = &result;
		result.addInputDepthView( getOwner()->getDepthViewId( m_face ) );
		result.addInOutColourView( m_colourView );
		return result;
	}
}
