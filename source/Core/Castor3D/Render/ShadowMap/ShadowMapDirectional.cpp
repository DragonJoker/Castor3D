#include "Castor3D/Render/ShadowMap/ShadowMapDirectional.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Event/Frame/CpuFunctorEvent.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureView.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/TriFaceMapping.hpp"
#include "Castor3D/Miscellaneous/ProgressBar.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Culling/FrustumCuller.hpp"
#include "Castor3D/Render/Passes/GaussianBlur.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapPassDirectional.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/DirectionalLight.hpp"
#include "Castor3D/Shader/Ubos/ShadowMapUbo.hpp"

#include <ashespp/Image/Image.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>
#include <ashespp/Sync/Fence.hpp>

#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/Graphics/Image.hpp>
#include <CastorUtils/Miscellaneous/BlockTracker.hpp>

#include <RenderGraph/FrameGraph.hpp>
#include <RenderGraph/RunnableGraph.hpp>

#define C3D_DebugCascadeFrustum 0

namespace castor3d
{
	namespace
	{
#if C3D_DebugCascadeFrustum

		MeshResPtr doCreateFrustumMesh( castor::String const name
			, Scene & scene
			, castor::RgbColour const & colour
			, castor::String const & colourName )
		{
			auto result = scene.getMeshCache().add( name, scene );
			result.lock()->setSerialisable( false );
			auto submesh = result.lock()->createSubmesh();
			InterleavedVertexArray vertex{
				InterleavedVertex{}.position( { -1, -1, -1 } ),
				InterleavedVertex{}.position( { -1, +1, -1 } ),
				InterleavedVertex{}.position( { +1, +1, -1 } ),
				InterleavedVertex{}.position( { +1, -1, -1 } ),
				InterleavedVertex{}.position( { -1, -1, +1 } ),
				InterleavedVertex{}.position( { -1, +1, +1 } ),
				InterleavedVertex{}.position( { +1, +1, +1 } ),
				InterleavedVertex{}.position( { +1, -1, +1 } ),
				InterleavedVertex{}.position( { -1, -1, -1 } ),
				InterleavedVertex{}.position( { -1, +1, -1 } ),
				InterleavedVertex{}.position( { +1, +1, -1 } ),
				InterleavedVertex{}.position( { +1, -1, -1 } ),
				InterleavedVertex{}.position( { -1, -1, +1 } ),
				InterleavedVertex{}.position( { -1, +1, +1 } ),
				InterleavedVertex{}.position( { +1, +1, +1 } ),
				InterleavedVertex{}.position( { +1, -1, +1 } ),
				InterleavedVertex{}.position( { -1, -1, -1 } ),
				InterleavedVertex{}.position( { -1, +1, -1 } ),
				InterleavedVertex{}.position( { +1, +1, -1 } ),
				InterleavedVertex{}.position( { +1, -1, -1 } ),
				InterleavedVertex{}.position( { -1, -1, +1 } ),
				InterleavedVertex{}.position( { -1, +1, +1 } ),
				InterleavedVertex{}.position( { +1, +1, +1 } ),
				InterleavedVertex{}.position( { +1, -1, +1 } ), };
			submesh->setTopology( VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST );
			submesh->addPoints( vertex );
			auto mapping = std::make_shared< TriFaceMapping >( *submesh );
			mapping->addQuadFace( 0u, 1u, 2u, 3u );
			mapping->addQuadFace( 4u, 5u, 6u, 7u );
			mapping->addQuadFace( 8u, 9u, 10u, 11u );
			mapping->addQuadFace( 12u, 13u, 14u, 15u );
			mapping->addQuadFace( 16u, 17u, 18u, 19u );
			mapping->addQuadFace( 20u, 21u, 22u, 23u );
			submesh->setIndexMapping( mapping );
			MaterialResPtr material;
			castor::String matName = cuT( "Frustum_" ) + colourName;

			if ( !scene.getEngine()->getMaterialCache().has( matName ) )
			{
				material = scene.getEngine()->getMaterialCache().add( matName
					, *scene.getEngine()
					, scene.getPassesType() );
				auto pass = material.lock()->createPass();
				pass->enableLighting( false );
				pass->enablePicking( false );
				pass->setColour( colour );
				pass->setOpacity( 0.2f );
			}
			else
			{
				material = scene.getEngine()->getMaterialCache().find( matName );
			}

			submesh->setDefaultMaterial( material.lock().get() );
			result.lock()->computeContainers();
			scene.getListener().postEvent( makeGpuInitialiseEvent( *submesh ) );
			return result;
		}

#endif

		std::vector< ShadowMap::PassDataPtr > createPasses( crg::ResourceHandler & handler
			, std::vector< std::unique_ptr< crg::FrameGraph > > & graphs
			, std::vector< crg::RunnableGraphPtr > & runnables
			, std::vector< std::unique_ptr< GaussianBlur > > & blurs
			, crg::ImageViewId intermediate
			, RenderDevice const & device
			, Scene & scene
			, ShadowMap & shadowMap
			, uint32_t cascadeCount )
		{
			auto & engine = *scene.getEngine();
			std::vector< ShadowMap::PassDataPtr > result;
			auto const width = ShadowMapPassDirectional::TileSize;
			auto const height = ShadowMapPassDirectional::TileSize;
			auto const w = float( width );
			auto const h = float( height );
			Viewport viewport{ engine };
			viewport.setOrtho( -w / 2, w / 2, -h / 2, h / 2, -5120.0, 5120.0 );
			viewport.resize( { width, height } );
			viewport.update();
			auto & smResult = shadowMap.getShadowPassResult();
			auto & depth = smResult[SmTexture::eDepth];
			auto & linear = smResult[SmTexture::eNormalLinear];
			auto & variance = smResult[SmTexture::eVariance];
			auto & position = smResult[SmTexture::ePosition];
			auto & flux = smResult[SmTexture::eFlux];

			graphs.push_back( std::make_unique< crg::FrameGraph >( handler,  "DirectionalSMC" ) );
			auto & graph = graphs.back()->getDefaultGroup();
			crg::FramePass const * previousPass{};

			for ( uint32_t cascade = 0u; cascade < cascadeCount; ++cascade )
			{
				std::string debugName = "DirectionalSMC" + std::to_string( cascade + 1u );
				result.emplace_back( std::make_unique< ShadowMap::PassData >( std::make_unique< MatrixUbo >( device )
					, std::make_shared< Camera >( debugName
						, scene
						, *scene.getCameraRootNode()
						, viewport
						, true )
					, nullptr ) );
				auto & passData = *result.back();
				passData.culler = std::make_unique< FrustumCuller >( scene, *passData.camera );
				auto & pass = graph.createPass( debugName
					, [&passData, &device, &shadowMap, cascade]( crg::FramePass const & framePass
						, crg::GraphContext & context
						, crg::RunnableGraph & runnableGraph )
					{
						auto res = std::make_unique< ShadowMapPassDirectional >( framePass
							, context
							, runnableGraph
							, device
							, *passData.matrixUbo
							, *passData.culler
							, shadowMap
							, cascade );
						passData.pass = res.get();
						device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
							, res->getTimer() );
						return res;
					} );

				if ( previousPass )
				{
					pass.addDependency( *previousPass );
				}

				previousPass = &pass;

				if ( cascadeCount == 1u )
				{
					pass.addOutputDepthView( depth.targetViewId, getClearValue( SmTexture::eDepth ) );
					pass.addOutputColourView( linear.targetViewId, getClearValue( SmTexture::eNormalLinear ) );
					pass.addOutputColourView( variance.targetViewId, getClearValue( SmTexture::eVariance ) );
					pass.addOutputColourView( position.targetViewId, getClearValue( SmTexture::ePosition ) );
					pass.addOutputColourView( flux.targetViewId, getClearValue( SmTexture::eFlux ) );

					blurs.push_back( std::make_unique< GaussianBlur >( graph
						, *previousPass
						, device
						, cuT( "ShadowMapDirectional" )
						, debugName
						, variance.wholeViewId
						, 5u ) );
				}
				else
				{
					pass.addOutputDepthView( depth.subViewsId[cascade], getClearValue( SmTexture::eDepth ) );
					pass.addOutputColourView( linear.subViewsId[cascade], getClearValue( SmTexture::eNormalLinear ) );
					pass.addOutputColourView( variance.subViewsId[cascade], getClearValue( SmTexture::eVariance ) );
					pass.addOutputColourView( position.subViewsId[cascade], getClearValue( SmTexture::ePosition ) );
					pass.addOutputColourView( flux.subViewsId[cascade], getClearValue( SmTexture::eFlux ) );

					blurs.push_back( std::make_unique< GaussianBlur >( graph
						, *previousPass
						, device
						, cuT( "ShadowMapDirectional" )
						, debugName
						, variance.subViewsId[cascade]
						, intermediate
						, 5u ) );
				}
			}

			return result;
		}
	}

	ShadowMapDirectional::ShadowMapDirectional( crg::ResourceHandler & handler
		, RenderDevice const & device
		, Scene & scene
		, ProgressBar * progress )
		: ShadowMap{ handler
			, device
			, scene
			, LightType::eDirectional
			, 0u
			, { ShadowMapPassDirectional::TileSize, ShadowMapPassDirectional::TileSize }
			, scene.getDirectionalShadowCascades()
			, 1u }
		, m_blurIntermediate{ handler.createImageId( crg::ImageData{ "DirectionalGB"
			, 0u
			, VK_IMAGE_TYPE_2D
			, getFormat( SmTexture::eVariance )
			, ( *m_result.begin() )->getExtent()
			, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_TRANSFER_SRC_BIT ) } ) }
		, m_blurIntermediateView{ handler.createViewId( crg::ImageViewData{ m_blurIntermediate.data->name
			, m_blurIntermediate
			, 0u
			, VK_IMAGE_VIEW_TYPE_2D
			, getFormat( m_blurIntermediate )
			, { VK_IMAGE_ASPECT_COLOR_BIT, 0u, 1u, 0u, 1u } } ) }
		, m_cascades{ scene.getDirectionalShadowCascades() }
	{
#if C3D_DebugCascadeFrustum
		std::array< castor::RgbColour, 4u > colours{ castor::RgbColour::fromPredefined( castor::PredefinedRgbColour::eRed )
			, castor::RgbColour::fromPredefined( castor::PredefinedRgbColour::eGreen )
			, castor::RgbColour::fromPredefined( castor::PredefinedRgbColour::eBlue )
			, castor::RgbColour::fromComponents( 1.0f, 1.0f, 0.0f ) };
		std::array< castor::String, 4u > colourNames{ cuT( "Red" )
			, cuT( "Green" )
			, cuT( "Blue" )
			, cuT( "Yellow" ) };

		for ( uint32_t cascade = 0u; cascade < m_cascades; ++cascade )
		{
			auto name = "CascadeFrustum" + std::to_string( cascade );
			auto mesh = doCreateFrustumMesh( name, scene, colours[cascade], colourNames[cascade] );
			m_frustumMeshes.push_back( mesh );

			if ( !scene.getGeometryCache().has( name ) )
			{
				auto sceneNode = scene.getSceneNodeCache().add( name, scene ).lock();
				auto geometry = std::make_shared< Geometry >( name, scene, *sceneNode, mesh );
				geometry->setShadowCaster( false );
				geometry->setCulled( false );

				for ( auto & submesh : *geometry->getMesh().lock() )
				{
					geometry->setMaterial( *submesh, submesh->getDefaultMaterial() );
				}

				sceneNode->attachTo( *scene.getObjectRootNode() );
				sceneNode->setVisible( false );
				scene.getGeometryCache().add( std::move( geometry ) );
			}
		}

#endif
		stepProgressBar( progress, "Creating ShadowMapDirectional" );
	}

	void ShadowMapDirectional::update( GpuUpdater & updater )
	{
		if ( m_runnables[updater.index] )
		{
			auto & light = *updater.light;
			auto & camera = *updater.camera;
			auto & directional = *light.getDirectionalLight();
			auto node = light.getParent();
			node->update();
			m_shadowType = light.getShadowType();

			auto shadowModified = directional.updateShadow( camera );

			for ( uint32_t cascade = 0u; cascade < m_cascades; ++cascade )
			{
				if ( shadowModified )
				{
					auto & culler = m_passes[cascade]->pass->getCuller();
					auto & lightCamera = culler.getCamera();
					lightCamera.attachTo( *node );
					lightCamera.setProjection( directional.getProjMatrix( cascade ) );
					lightCamera.setView( directional.getViewMatrix( cascade ) );
					lightCamera.updateFrustum();

#if C3D_DebugCascadeFrustum
					auto name = "CascadeFrustum" + std::to_string( cascade );
					auto & scene = *light.getScene();
					auto sceneNode = scene.getGeometryCache().tryFind( name ).lock();
					sceneNode->setVisible( true );
					auto & frustum = lightCamera.getFrustum();
					auto mesh = m_frustumMeshes[cascade].lock();
					auto submesh = mesh->getSubmesh( 0u );
					auto & points = submesh->getPoints();

					for ( auto i = 0u; i < points.size(); ++i )
					{
						points[i] = frustum.getPoints()[i];
					}

					submesh->needsUpdate();
					submesh->computeContainers();
					scene.getEngine()->postEvent( makeGpuFunctorEvent( EventType::ePreRender
						, [submesh]( RenderDevice const & device
							, QueueData const & queueData )
						{
							submesh->update();
						} ) );
#endif

				}

				updater.index = cascade;
				m_passes[cascade]->pass->update( updater );
			}
		}
	}

	std::vector< ShadowMap::PassDataPtr > ShadowMapDirectional::doCreatePass( uint32_t index )
	{
		return createPasses( m_handler
			, m_graphs
			, m_runnables
			, m_blurs
			, m_blurIntermediateView
			, m_device
			, m_scene
			, *this
			, m_scene.getDirectionalShadowCascades() );
	}

	bool ShadowMapDirectional::doIsUpToDate( uint32_t index )const
	{
		return std::all_of( m_passes.begin()
			, m_passes.begin() + std::min( m_cascades, uint32_t( m_passes.size() ) )
			, []( ShadowMap::PassDataPtr const & data )
			{
				return data->pass->isUpToDate();
			} );
	}

	void ShadowMapDirectional::doSetUpToDate( uint32_t index )
	{
		for ( auto & data : castor::makeArrayView( m_passes.begin(), m_passes.begin() + std::min( m_cascades, uint32_t( m_passes.size() ) ) ) )
		{
			data->pass->setUpToDate();
		}
	}

	void ShadowMapDirectional::doUpdate( CpuUpdater & updater )
	{
		if ( m_runnables[updater.index] )
		{
			for ( uint32_t cascade = 0u; cascade < m_cascades; ++cascade )
			{
				updater.index = cascade;
				m_passes[cascade]->pass->update( updater );
			}
		}
	}
}
