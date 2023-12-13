#include "Castor3D/Render/ShadowMap/ShadowMapDirectional.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Limits.hpp"
#include "Castor3D/Event/Frame/CpuFunctorEvent.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/DefaultRenderComponent.hpp"
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
#include <RenderGraph/RunnablePasses/ImageCopy.hpp>

#define C3D_DebugCascadeFrustum 0

namespace castor3d
{
	namespace shdmapdir
	{
		static castor::String getPassName( uint32_t cascadeIndex
			, bool needsVsm
			, bool needsRsm
			, bool isStatic )
		{
			auto result = cuT( "DirectionalSMC" ) + castor::string::toString( cascadeIndex );

			if ( needsVsm )
			{
				result += "_VSM";
			}

			if ( needsRsm )
			{
				result += "_RSM";
			}

			if ( isStatic )
			{
				result += "_Statics";
			}

			return result;
		}

#if C3D_DebugCascadeFrustum

		static MeshResPtr doCreateFrustumMesh( castor::String const name
			, Scene & scene
			, castor::RgbColour const & colour
			, castor::String const & colourName )
		{

			auto result = scene.addNewMesh( name, scene );
			result->setSerialisable( false );
			auto submesh = result->createSubmesh();
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
			auto mapping = submesh->createComponent< LinesMapping >();
			auto mapping = submesh->createComponent< DefaultRenderComponent >();
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
			MaterialObs material{};
			castor::String matName = cuT( "Frustum_" ) + colourName;

			if ( !scene.getEngine()->hasMaterial( matName ) )
			{
				material = scene.getEngine()->addNewMaterial( matName
					, *scene.getEngine()
					, scene.getDefaultLightingModel() );
				auto pass = material->createPass();
				pass->enableLighting( false );
				pass->enablePicking( false );
				pass->setColour( colour );
			}
			else
			{
				material = scene.getEngine()->findMaterial( matName );
			}

			submesh->setDefaultMaterial( material );
			result->computeContainers();
			scene.getListener().postEvent( makeGpuInitialiseEvent( *submesh ) );
			return result;
		}

#endif
	}

	ShadowMapDirectional::ShadowMapDirectional( crg::ResourcesCache & resources
		, RenderDevice const & device
		, Scene & scene
		, ProgressBar * progress )
		: ShadowMap{ resources
			, device
			, scene
			, LightType::eDirectional
			, 0u
			, { ShadowMapDirectionalTextureSize, ShadowMapDirectionalTextureSize }
			, scene.getDirectionalShadowCascades()
			, 1u }
		, m_blurIntermediate{ resources.getHandler().createImageId( crg::ImageData{ "DirectionalGB"
			, 0u
			, VK_IMAGE_TYPE_2D
			, getFormat( device, SmTexture::eVariance )
			, ( *m_result.begin() )->getExtent()
			, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_TRANSFER_SRC_BIT ) } ) }
		, m_blurIntermediateView{ resources.getHandler().createViewId( crg::ImageViewData{ m_blurIntermediate.data->name
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
				auto sceneNode = scene.addNewSceneNode( name );
				auto geometry = std::make_shared< Geometry >( name, scene, *sceneNode, mesh );
				geometry->setShadowCaster( false );
				geometry->setCullable( false );

				for ( auto & submesh : *geometry->getMesh() )
				{
					geometry->setMaterial( *submesh, submesh->getDefaultMaterial() );
				}

				sceneNode->attachTo( *scene.getObjectRootNode() );
				sceneNode->setVisible( false );
				scene.addGeometry( std::move( geometry ) );
			}
		}

#endif
		stepProgressBarLocal( progress, "Creating ShadowMapDirectional" );
	}

	crg::FramePassArray ShadowMapDirectional::doCreatePass( crg::FrameGraph & graph
		, crg::FramePassArray const & previousPasses
		, uint32_t index
		, bool vsm
		, bool rsm
		, bool isStatic
		, Passes & passes )
	{
		auto & engine = *m_scene.getEngine();
		auto const width = ShadowMapDirectionalTextureSize;
		auto const height = ShadowMapDirectionalTextureSize;
		auto const w = float( width );
		auto const h = float( height );
		Viewport viewport{ engine };
		viewport.setOrtho( -w / 2, w / 2, -h / 2, h / 2, -5120.0, 5120.0 );
		viewport.resize( { width, height } );
		viewport.update();
		auto & smResult = getShadowPassResult( isStatic );
		auto & depth = smResult[SmTexture::eDepth];
		auto & linear = smResult[SmTexture::eLinearDepth];
		auto & variance = smResult[SmTexture::eVariance];
		auto & normal = smResult[SmTexture::eNormal];
		auto & position = smResult[SmTexture::ePosition];
		auto & flux = smResult[SmTexture::eFlux];

		doRegisterGraphIO( graph, vsm, rsm, isStatic );

		crg::FramePass const * previousPass{};
		auto cascadeCount = m_scene.getDirectionalShadowCascades();
		crg::FramePassArray resultPasses;

		for ( uint32_t cascade = 0u; cascade < cascadeCount; ++cascade )
		{
			std::string debugName = shdmapdir::getPassName( cascade, vsm, rsm, isStatic );
			auto & group = graph.createPassGroup( debugName );

			if ( m_passes[m_passesIndex].cameras.size() <= cascade )
			{
				m_passes[m_passesIndex].cameraUbos.push_back( std::make_unique< CameraUbo >( m_device ) );
				m_passes[m_passesIndex].cameras.push_back( m_scene.createCamera( shdmapdir::getPassName( cascade, false, false, false )
					, m_scene
					, *m_scene.getCameraRootNode()
					, viewport
					, true ) );
				CU_Require( m_passes[m_passesIndex].cameras.size() > cascade );
			}

			auto & camera = *m_passes[m_passesIndex].cameras[cascade];
			auto & cameraUbo = *m_passes[m_passesIndex].cameraUbos[cascade];
			passes.passes.emplace_back( std::make_unique< ShadowMap::PassData >() );
			auto & passData = *passes.passes.back();
			passData.ownCuller = castor::makeUniqueDerived< SceneCuller, DummyCuller >( m_scene, &camera, isStatic );
			passData.culler = passData.ownCuller.get();
			auto & pass = group.createPass( "Nodes"
				, [&passData, this, cascade, vsm, rsm, isStatic, &camera, &cameraUbo]( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & runnableGraph )
				{
					auto res = std::make_unique< ShadowMapPassDirectional >( framePass
						, context
						, runnableGraph
						, m_device
						, cameraUbo
						, *passData.culler
						, camera
						, *this
						, vsm
						, rsm
						, isStatic
						, cascade );
					passData.pass = res.get();
					m_device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
						, res->getTimer() );
					return res;
				} );

			if ( !isStatic )
			{
				pass.addDependency( *previousPasses[cascade] );
			}

			if ( previousPass )
			{
				pass.addDependency( *previousPass );
			}

			previousPass = &pass;

			if ( cascadeCount == 1u )
			{
				if ( isStatic )
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
				}
				else
				{
					pass.addInOutDepthView( depth.targetViewId );
					pass.addInOutColourView( linear.targetViewId );

					if ( vsm )
					{
						pass.addInOutColourView( variance.targetViewId );
					}

					if ( rsm )
					{
						pass.addInOutColourView( normal.targetViewId );
						pass.addInOutColourView( position.targetViewId );
						pass.addInOutColourView( flux.targetViewId );
					}
				}
				if ( isStatic )
				{
					auto & nstSmResult = getShadowPassResult( false );
					auto & copyPass = graph.createPass( "CopyToNonStatic"
						, [this, isStatic, cascade]( crg::FramePass const & framePass
							, crg::GraphContext & context
							, crg::RunnableGraph & runnableGraph )
						{
							auto result = std::make_unique< crg::ImageCopy >( framePass
								, context
								, runnableGraph
								, getShadowPassResult( isStatic )[SmTexture::eDepth].getExtent()
								, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
								, crg::ru::Config{}
								, crg::ImageCopy::GetPassIndexCallback( [](){ return 0u; } )
								, crg::ImageCopy::IsEnabledCallback( [this, cascade](){ return doEnableCopyStatic( cascade ); } ) );
							getOwner()->registerTimer( framePass.getFullName()
								, result->getTimer() );
							return result;
						} );
					pass.addDependency( pass );
					copyPass.addTransferInputView( depth.targetViewId );
					copyPass.addTransferOutputView( nstSmResult[SmTexture::eDepth].targetViewId );
					copyPass.addTransferInputView( linear.targetViewId );
					copyPass.addTransferOutputView( nstSmResult[SmTexture::eLinearDepth].targetViewId );

					if ( vsm )
					{
						copyPass.addTransferInputView( variance.targetViewId );
						copyPass.addTransferOutputView( nstSmResult[SmTexture::eVariance].targetViewId );
					}

					if ( rsm )
					{
						copyPass.addTransferInputView( normal.targetViewId );
						copyPass.addTransferOutputView( nstSmResult[SmTexture::eNormal].targetViewId );
						copyPass.addTransferInputView( position.targetViewId );
						copyPass.addTransferOutputView( nstSmResult[SmTexture::ePosition].targetViewId );
						copyPass.addTransferInputView( flux.targetViewId );
						copyPass.addTransferOutputView( nstSmResult[SmTexture::eFlux].targetViewId );
					}

					resultPasses.push_back( &copyPass );
				}
				else if ( vsm )
				{
					passes.blurs.push_back( castor::makeUnique< GaussianBlur >( group
						, *previousPass
						, m_device
						, cuT( "ShadowMapDirectional" )
						, debugName
						, variance.wholeViewId
						, 5u ) );
					resultPasses.push_back( &passes.blurs.back()->getLastPass() );
				}
				else
				{
					resultPasses.push_back( &pass );
				}
			}
			else
			{
				if ( isStatic )
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
				}
				else
				{
					pass.addInOutDepthView( depth.subViewsId[cascade] );
					pass.addInOutColourView( linear.subViewsId[cascade] );

					if ( vsm )
					{
						pass.addInOutColourView( variance.subViewsId[cascade] );
					}

					if ( rsm )
					{
						pass.addInOutColourView( normal.subViewsId[cascade] );
						pass.addInOutColourView( position.subViewsId[cascade] );
						pass.addInOutColourView( flux.subViewsId[cascade] );
					}
				}

				if ( isStatic )
				{
					auto & nstSmResult = getShadowPassResult( false );
					auto & copyPass = group.createPass( "CopyToNonStatic"
						, [this, isStatic, cascade]( crg::FramePass const & framePass
							, crg::GraphContext & context
							, crg::RunnableGraph & runnableGraph )
						{
							auto result = std::make_unique< crg::ImageCopy >( framePass
								, context
								, runnableGraph
								, getShadowPassResult( isStatic )[SmTexture::eDepth].getExtent()
								, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
								, crg::ru::Config{}
								, crg::ImageCopy::GetPassIndexCallback( [](){ return 0u; } )
								, crg::ImageCopy::IsEnabledCallback( [this, cascade](){ return doEnableCopyStatic( cascade ); } ) );
							getOwner()->registerTimer( framePass.getFullName()
								, result->getTimer() );
							return result;
						} );
					copyPass.addDependency( *previousPass );
					copyPass.addTransferInputView( depth.subViewsId[cascade] );
					copyPass.addTransferOutputView( nstSmResult[SmTexture::eDepth].subViewsId[cascade] );
					copyPass.addTransferInputView( linear.subViewsId[cascade] );
					copyPass.addTransferOutputView( nstSmResult[SmTexture::eLinearDepth].subViewsId[cascade] );

					if ( vsm )
					{
						copyPass.addTransferInputView( variance.subViewsId[cascade] );
						copyPass.addTransferOutputView( nstSmResult[SmTexture::eVariance].subViewsId[cascade] );
					}

					if ( rsm )
					{
						copyPass.addTransferInputView( normal.subViewsId[cascade] );
						copyPass.addTransferOutputView( nstSmResult[SmTexture::eNormal].subViewsId[cascade] );
						copyPass.addTransferInputView( position.subViewsId[cascade] );
						copyPass.addTransferOutputView( nstSmResult[SmTexture::ePosition].subViewsId[cascade] );
						copyPass.addTransferInputView( flux.subViewsId[cascade] );
						copyPass.addTransferOutputView( nstSmResult[SmTexture::eFlux].subViewsId[cascade] );
					}

					previousPass = &copyPass;
					resultPasses.push_back( previousPass );
				}
				else if ( vsm )
				{
					passes.blurs.push_back( castor::makeUnique< GaussianBlur >( group
						, *previousPass
						, m_device
						, cuT( "ShadowMapDirectional" )
						, debugName
						, variance.subViewsId[cascade]
						, m_blurIntermediateView
						, 5u
						, crg::ImageCopy::IsEnabledCallback( [this, cascade]() { return doEnableBlur( cascade ); } ) ) );
					resultPasses.push_back( &passes.blurs.back()->getLastPass() );
				}
				else
				{
					resultPasses.push_back( &pass );
				}
			}
		}

		return resultPasses;
	}

	bool ShadowMapDirectional::doIsUpToDate( uint32_t index
		, ShadowMap::Passes const & passes )const
	{
		return std::all_of( passes.passes.begin()
			, passes.passes.begin() + std::min( m_cascades, uint32_t( passes.passes.size() ) )
			, []( ShadowMap::PassDataPtr const & data )
			{
				return data->pass->isUpToDate();
			} );
	}

	void ShadowMapDirectional::doSetUpToDate( uint32_t index
		, ShadowMap::Passes & passes )
	{
		for ( auto & data : castor::makeArrayView( passes.passes.begin(), passes.passes.begin() + std::min( m_cascades, uint32_t( passes.passes.size() ) ) ) )
		{
			data->pass->setUpToDate();
		}
	}

	void ShadowMapDirectional::doUpdate( CpuUpdater & updater
		, ShadowMap::Passes & passes )
	{
		auto save = updater.index;

		for ( uint32_t cascade = 0u; cascade < m_cascades; ++cascade )
		{
			updater.index = cascade;
			passes.passes[cascade]->pass->update( updater );
		}

		updater.index = save;
	}

	void ShadowMapDirectional::doUpdate( GpuUpdater & updater
		, ShadowMapDirectional::Passes & passes )
	{
		auto save = updater.index;
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
				auto & lightCamera = *m_passes[m_passesIndex].cameras[cascade];
				lightCamera.attachTo( *node );
				lightCamera.setView( directional.getViewMatrix( cascade ) );
				lightCamera.setProjection( directional.getProjMatrix( cascade ) );
				lightCamera.updateFrustum();

#if C3D_DebugCascadeFrustum
				auto name = "CascadeFrustum" + std::to_string( cascade );
				auto & scene = *light.getScene();
				auto sceneNode = scene.tryFindGeometry( name );
				sceneNode->setVisible( true );
				auto & frustum = lightCamera.getFrustum();
				auto mesh = m_frustumMeshes[cascade];
				auto submesh = mesh->getSubmesh( 0u );
				auto & points = submesh->getPoints();

				for ( auto i = 0u; i < points.size(); ++i )
				{
					points[i] = frustum.getPoints()[i];
				}

				submesh->needsUpdate();
				submesh->computeContainers();
				scene.getEngine()->postEvent( makeGpuFunctorEvent( GpuEventType::ePreUpload
					, [submesh]( RenderDevice const & device
						, QueueData const & queueData )
					{
						submesh->update();
					} ) );
#endif

			}

			updater.index = cascade;
		}

		updater.index = save;
	}
}
