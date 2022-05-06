#include "Castor3D/Render/ShadowMap/ShadowMapDirectional.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Limits.hpp"
#include "Castor3D/Event/Frame/CpuFunctorEvent.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureView.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/LinesMapping.hpp"
#include "Castor3D/Miscellaneous/ProgressBar.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Culling/DummyCuller.hpp"
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
	namespace shdmapdir
	{
#if C3D_DebugCascadeFrustum

		static MeshResPtr doCreateFrustumMesh( castor::String const name
			, Scene & scene
			, castor::RgbColour const & colour
			, castor::String const & colourName )
		{

			auto result = scene.addNewMesh( name, scene );
			result.lock()->setSerialisable( false );
			auto submesh = result.lock()->createSubmesh();
			static castor3d::InterleavedVertexArray const vertex{ []()
				{
					castor3d::InterleavedVertexArray result;
					result.resize( 8u );
					result[size_t( FrustumCorner::eFarLeftBottom )].position( castor::Point3f{ -1, -1, +1 } );
					result[size_t( FrustumCorner::eFarLeftTop )].position( castor::Point3f{ -1, +1, +1 } );
					result[size_t( FrustumCorner::eFarRightTop )].position( castor::Point3f{ +1, +1, +1 } );
					result[size_t( FrustumCorner::eFarRightBottom )].position( castor::Point3f{ +1, -1, +1 } );
					result[size_t( FrustumCorner::eNearLeftBottom )].position( castor::Point3f{ -1, -1, -1 } );
					result[size_t( FrustumCorner::eNearLeftTop )].position( castor::Point3f{ -1, +1, -1 } );
					result[size_t( FrustumCorner::eNearRightTop )].position( castor::Point3f{ +1, +1, -1 } );
					result[size_t( FrustumCorner::eNearRightBottom )].position( castor::Point3f{ +1, -1, -1 } );
					return result;
				}()
			};
			submesh->setTopology( VK_PRIMITIVE_TOPOLOGY_LINE_LIST );
			submesh->addPoints( vertex );
			auto mapping = std::make_shared< castor3d::LinesMapping >( *submesh );
			castor3d::LineIndices lines[]
			{
				castor3d::LineIndices{ { 0u, 1u } },
				castor3d::LineIndices{ { 1u, 2u } },
				castor3d::LineIndices{ { 2u, 3u } },
				castor3d::LineIndices{ { 3u, 0u } },
				castor3d::LineIndices{ { 4u, 5u } },
				castor3d::LineIndices{ { 5u, 6u } },
				castor3d::LineIndices{ { 6u, 7u } },
				castor3d::LineIndices{ { 7u, 4u } },
				castor3d::LineIndices{ { 0u, 4u } },
				castor3d::LineIndices{ { 1u, 5u } },
				castor3d::LineIndices{ { 2u, 6u } },
				castor3d::LineIndices{ { 3u, 7u } },
			};
			mapping->addLineGroup( lines );
			submesh->setIndexMapping( mapping );
			MaterialResPtr material;
			castor::String matName = cuT( "Frustum_" ) + colourName;

			if ( !scene.getEngine()->hasMaterial( matName ) )
			{
				material = scene.getEngine()->addNewMaterial( matName
					, *scene.getEngine()
					, scene.getPassesType() );
				auto pass = material.lock()->createPass();
				pass->enableLighting( false );
				pass->enablePicking( false );
				pass->setColour( colour );
			}
			else
			{
				material = scene.getEngine()->findMaterial( matName );
			}

			submesh->setDefaultMaterial( material.lock().get() );
			result.lock()->computeContainers();
			scene.getListener().postEvent( makeGpuInitialiseEvent( *submesh ) );
			return result;
		}

#endif

		static std::vector< ShadowMap::PassDataPtr > createPasses( crg::ResourceHandler & handler
			, std::vector< std::unique_ptr< crg::FrameGraph > > & graphs
			, std::vector< crg::RunnableGraphPtr > & runnables
			, std::vector< GaussianBlurUPtr > & blurs
			, crg::ImageViewId intermediate
			, RenderDevice const & device
			, Scene & scene
			, ShadowMap & shadowMap
			, uint32_t cascadeCount
			, bool vsm
			, bool rsm )
		{
			auto & engine = *scene.getEngine();
			std::vector< ShadowMap::PassDataPtr > result;
			auto const width = ShadowMapDirectionalTextureSize;
			auto const height = ShadowMapDirectionalTextureSize;
			auto const w = float( width );
			auto const h = float( height );
			Viewport viewport{ engine };
			viewport.setOrtho( -w / 2, w / 2, -h / 2, h / 2, -5120.0, 5120.0 );
			viewport.resize( { width, height } );
			viewport.update();
			auto & smResult = shadowMap.getShadowPassResult();
			auto & depth = smResult[SmTexture::eDepth];
			auto & linear = smResult[SmTexture::eLinearDepth];
			auto & variance = smResult[SmTexture::eVariance];
			auto & normal = smResult[SmTexture::eNormal];
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
				passData.culler = std::make_unique< DummyCuller >( scene, passData.camera.get() );
				auto & pass = graph.createPass( debugName
					, [&passData, &device, &shadowMap, cascade, vsm, rsm]( crg::FramePass const & framePass
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
							, vsm
							, rsm
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
					pass.addOutputColourView( linear.targetViewId, getClearValue( SmTexture::eLinearDepth ) );

					if ( vsm )
					{
						pass.addOutputColourView( variance.targetViewId, getClearValue( SmTexture::eVariance ) );
					}

					if ( rsm )
					{
						pass.addOutputColourView( normal.targetViewId, getClearValue( SmTexture::eNormal ) );
						pass.addOutputColourView( position.targetViewId, getClearValue( SmTexture::ePosition ) );
						pass.addOutputColourView( flux.targetViewId, getClearValue( SmTexture::eFlux ) );
					}

					if ( vsm )
					{
						blurs.push_back( castor::makeUnique< GaussianBlur >( graph
							, *previousPass
							, device
							, cuT( "ShadowMapDirectional" )
							, debugName
							, variance.wholeViewId
							, 5u ) );
					}
				}
				else
				{
					pass.addOutputDepthView( depth.subViewsId[cascade], getClearValue( SmTexture::eDepth ) );
					pass.addOutputColourView( linear.subViewsId[cascade], getClearValue( SmTexture::eLinearDepth ) );

					if ( vsm )
					{
						pass.addOutputColourView( variance.subViewsId[cascade], getClearValue( SmTexture::eVariance ) );
					}

					if ( rsm )
					{
						pass.addOutputColourView( normal.subViewsId[cascade], getClearValue( SmTexture::eNormal ) );
						pass.addOutputColourView( position.subViewsId[cascade], getClearValue( SmTexture::ePosition ) );
						pass.addOutputColourView( flux.subViewsId[cascade], getClearValue( SmTexture::eFlux ) );
					}

					if ( vsm )
					{
						blurs.push_back( castor::makeUnique< GaussianBlur >( graph
							, *previousPass
							, device
							, cuT( "ShadowMapDirectional" )
							, debugName
							, variance.subViewsId[cascade]
							, intermediate
							, 5u ) );
					}
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
			, { ShadowMapDirectionalTextureSize, ShadowMapDirectionalTextureSize }
			, scene.getDirectionalShadowCascades()
			, 1u }
		, m_blurIntermediate{ handler.createImageId( crg::ImageData{ "DirectionalGB"
			, 0u
			, VK_IMAGE_TYPE_2D
			, getFormat( device, SmTexture::eVariance )
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
			auto mesh = shdmapdir::doCreateFrustumMesh( name, scene, colours[cascade], colourNames[cascade] );
			m_frustumMeshes.push_back( mesh );

			if ( !scene.hasGeometry( name ) )
			{
				auto sceneNode = scene.addNewSceneNode( name ).lock();
				auto geometry = std::make_shared< Geometry >( name, scene, *sceneNode, mesh );
				geometry->setShadowCaster( false );
				geometry->setCullable( false );

				for ( auto & submesh : *geometry->getMesh().lock() )
				{
					geometry->setMaterial( *submesh, submesh->getDefaultMaterial() );
				}

				sceneNode->attachTo( *scene.getObjectRootNode() );
				sceneNode->setVisible( false );
				scene.addGeometry( std::move( geometry ) );
			}
		}

#endif
		stepProgressBar( progress, "Creating ShadowMapDirectional" );
	}

	void ShadowMapDirectional::update( GpuUpdater & updater )
	{
		auto & myPasses = m_passes[m_passesIndex];

		if ( myPasses.runnables[updater.index] )
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
					auto & lightCamera = *myPasses.passes[cascade]->camera;
					lightCamera.attachTo( *node );
					lightCamera.setView( directional.getViewMatrix( cascade ) );
					lightCamera.setProjection( directional.getProjMatrix( cascade ) );
					lightCamera.updateFrustum();

#if C3D_DebugCascadeFrustum
					auto name = "CascadeFrustum" + std::to_string( cascade );
					auto & scene = *light.getScene();
					auto sceneNode = scene.tryFindGeometry( name ).lock();
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
			}
		}
	}

	std::vector< ShadowMap::PassDataPtr > ShadowMapDirectional::doCreatePass( uint32_t index
		, bool vsm
		, bool rsm )
	{
		auto & myPasses = m_passes[m_passesIndex];
		return shdmapdir::createPasses( m_handler
			, myPasses.graphs
			, myPasses.runnables
			, myPasses.blurs
			, m_blurIntermediateView
			, m_device
			, m_scene
			, *this
			, m_scene.getDirectionalShadowCascades()
			, vsm
			, rsm );
	}

	bool ShadowMapDirectional::doIsUpToDate( uint32_t index )const
	{
		auto & myPasses = m_passes[m_passesIndex];
		return std::all_of( myPasses.passes.begin()
			, myPasses.passes.begin() + std::min( m_cascades, uint32_t( myPasses.passes.size() ) )
			, []( ShadowMap::PassDataPtr const & data )
			{
				return data->pass->isUpToDate();
			} );
	}

	void ShadowMapDirectional::doSetUpToDate( uint32_t index )
	{
		auto & myPasses = m_passes[m_passesIndex];

		for ( auto & data : castor::makeArrayView( myPasses.passes.begin(), myPasses.passes.begin() + std::min( m_cascades, uint32_t( myPasses.passes.size() ) ) ) )
		{
			data->pass->setUpToDate();
		}
	}

	void ShadowMapDirectional::doUpdate( CpuUpdater & updater )
	{
		auto & myPasses = m_passes[m_passesIndex];

		if ( myPasses.runnables[updater.index] )
		{
			for ( uint32_t cascade = 0u; cascade < m_cascades; ++cascade )
			{
				updater.index = cascade;
				myPasses.passes[cascade]->pass->update( updater );
			}
		}
	}
}
