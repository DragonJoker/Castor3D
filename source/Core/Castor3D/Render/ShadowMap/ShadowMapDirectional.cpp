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
#include "Castor3D/Model/Mesh/Submesh/Component/LineMapping.hpp"
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

namespace c3d
{
	namespace shdmapdir
	{
		static String getPassName( uint32_t cascadeIndex
			, bool needsVsm
			, bool needsRsm
			, bool isStatic )
		{
			auto result = cuT( "DirectionalSMC" ) + string::toString( cascadeIndex );

			if ( needsVsm )
			{
				result += cuT( "_VSM" );
			}

			if ( needsRsm )
			{
				result += cuT( "_RSM" );
			}

			if ( isStatic )
			{
				result += cuT( "_Statics" );
			}

			return result;
		}

#if C3D_DebugCascadeFrustum

		static MeshResPtr doCreateFrustumMesh( String const name
			, Scene & scene
			, RgbColour const & colour
			, String const & colourName )
		{

			auto result = scene.addNewMesh( name, scene );
			result->setSerialisable( false );
			auto submesh = result->createSubmesh();
			static InterleavedVertexArray const vertex{ []()
				{
					InterleavedVertexArray result;
					result.resize( 8u );
					result[size_t( Corner::eFarLeftBottom )].position( Point3f{ -1, -1, +1 } );
					result[size_t( Corner::eFarLeftTop )].position( Point3f{ -1, +1, +1 } );
					result[size_t( Corner::eFarRightTop )].position( Point3f{ +1, +1, +1 } );
					result[size_t( Corner::eFarRightBottom )].position( Point3f{ +1, -1, +1 } );
					result[size_t( Corner::eNearLeftBottom )].position( Point3f{ -1, -1, -1 } );
					result[size_t( Corner::eNearLeftTop )].position( Point3f{ -1, +1, -1 } );
					result[size_t( Corner::eNearRightTop )].position( Point3f{ +1, +1, -1 } );
					result[size_t( Corner::eNearRightBottom )].position( Point3f{ +1, -1, -1 } );
					return result;
				}()
			};
			submesh->setTopology( VK_PRIMITIVE_TOPOLOGY_LINE_LIST );
			submesh->addPoints( vertex );
			auto mapping = submesh->createComponent< LineMapping >();
			auto mapping = submesh->createComponent< DefaultRenderComponent >();
			LineIndices lines[]
			{
				LineIndices{ { 0u, 1u } },
				LineIndices{ { 1u, 2u } },
				LineIndices{ { 2u, 3u } },
				LineIndices{ { 3u, 0u } },
				LineIndices{ { 4u, 5u } },
				LineIndices{ { 5u, 6u } },
				LineIndices{ { 6u, 7u } },
				LineIndices{ { 7u, 4u } },
				LineIndices{ { 0u, 4u } },
				LineIndices{ { 1u, 5u } },
				LineIndices{ { 2u, 6u } },
				LineIndices{ { 3u, 7u } },
			};
			mapping->addLineGroup( lines );
			MaterialObs material{};
			String matName = cuT( "Frustum_" ) + colourName;

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
			, ImageCreateFlags::eNone
			, { ShadowMapDirectionalTextureSize, ShadowMapDirectionalTextureSize }
			, scene.getDirectionalShadowCascades()
			, 1u }
		, m_blurIntermediate{ resources.getHandler().createImageId( crg::ImageData{ "DirectionalGB"
			, ImageCreateFlags::eNone
			, ImageType::e2D
			, getFormat( device, SmTexture::eVariance )
			, ( *m_result.begin() )->getExtent()
			, ( ImageUsageFlags::eColorAttachment
				| ImageUsageFlags::eSampled
				| ImageUsageFlags::eTransferSrc ) } ) }
		, m_blurIntermediateView{ resources.getHandler().createViewId( crg::ImageViewData{ m_blurIntermediate.data->name
			, m_blurIntermediate
			, ImageViewCreateFlags::eNone
			, ImageViewType::e2D
			, getFormat( m_blurIntermediate )
			, { ImageAspectFlags::eColor , 0u, 1u, 0u, 1u } } ) }
		, m_cascades{ scene.getDirectionalShadowCascades() }
	{
#if C3D_DebugCascadeFrustum
		Array< RgbColour, 4u > colours{ RgbColour::fromPredefined( PredefinedRgbColour::eRed )
			, RgbColour::fromPredefined( PredefinedRgbColour::eGreen )
			, RgbColour::fromPredefined( PredefinedRgbColour::eBlue )
			, RgbColour::fromComponents( 1.0f, 1.0f, 0.0f ) };
		Array< String, 4u > colourNames{ cuT( "Red" )
			, cuT( "Green" )
			, cuT( "Blue" )
			, cuT( "Yellow" ) };

		for ( uint32_t cascade = 0u; cascade < m_cascades; ++cascade )
		{
			auto name = cuT( "CascadeFrustum" ) + string::toString( cascade );
			auto mesh = shdmapdir::doCreateFrustumMesh( name, scene, colours[cascade], colourNames[cascade] );
			m_frustumMeshes.push_back( mesh );

			if ( !scene.hasGeometry( name ) )
			{
				auto sceneNode = scene.addNewSceneNode( name );
				auto geometry = makeShared< Geometry >( name, scene, *sceneNode, mesh );
				geometry->setShadowCaster( false );
				geometry->setCullable( false );

				for ( auto & submesh : *geometry->getMesh() )
				{
					geometry->setMaterial( *submesh, submesh->getDefaultMaterial() );
				}

				sceneNode->attachTo( *scene.getObjectRootNode() );
				sceneNode->setVisible( false );
				scene.addGeometry( c3d::move( geometry ) );
			}
		}

#endif
		stepProgressBarLocal( progress, cuT( "Creating ShadowMapDirectional" ) );
	}

	void ShadowMapDirectional::doCreatePass( crg::FramePassGroup & graph
		, uint32_t index, bool vsm, bool rsm, bool isStatic
		, Passes & passes )
	{
		Engine const & engine = *m_scene.getEngine();
		auto const width = ShadowMapDirectionalTextureSize;
		auto const height = ShadowMapDirectionalTextureSize;
		auto const w = float( width );
		auto const h = float( height );
		Viewport viewport{ engine };
		viewport.setOrtho( -w / 2, w / 2, -h / 2, h / 2, -5120.0, 5120.0 );
		viewport.update();
		ShadowMapResult & smResult = getShadowPassResult( isStatic );

		doRegisterGraphIO( graph, vsm, rsm, isStatic );

		auto cascadeCount = m_scene.getDirectionalShadowCascades();

		for ( uint32_t cascade = 0u; cascade < cascadeCount; ++cascade )
		{
			auto debugName = toUtf8( shdmapdir::getPassName( cascade, vsm, rsm, isStatic ) );
			auto & group = graph.createPassGroup( debugName );

			if ( m_passes[m_passesIndex].cameras.size() <= cascade )
			{
				m_passes[m_passesIndex].cameraUbos.push_back( makeRawUnique< CameraUbo >( m_device ) );
				m_passes[m_passesIndex].cameras.push_back( m_scene.createCamera( shdmapdir::getPassName( cascade, false, false, false )
					, m_scene
					, *m_scene.getCameraRootNode()
					, viewport
					, true ) );
				CU_Require( m_passes[m_passesIndex].cameras.size() > cascade );
			}

			auto & camera = *m_passes[m_passesIndex].cameras[cascade];
			auto & cameraUbo = *m_passes[m_passesIndex].cameraUbos[cascade];
			passes.passes.emplace_back( makeRawUnique< ShadowMap::PassData >() );
			auto & passData = *passes.passes.back();
			passData.ownCuller = makeUniqueDerived< SceneCuller, DummyCuller >( m_scene, &camera, isStatic );
			passData.culler = passData.ownCuller.get();
			auto & pass = group.createPass( "Nodes"
				, [&passData, this, vsm, rsm, isStatic, &camera, &cameraUbo]( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & runnableGraph )
				{
					auto res = makeRawUnique< ShadowMapPassDirectional >( framePass
						, context
						, runnableGraph
						, m_device
						, cameraUbo
						, *m_renderUbo
						, *passData.culler
						, camera
						, *this
						, vsm
						, rsm
						, isStatic );
					passData.pass = res.get();
					m_device.renderSystem.getEngine()->registerTimer( makeString( framePass.getFullName() )
						, res->getTimer() );
					return res;
				} );

			if ( cascadeCount == 1u )
			{
				if ( isStatic )
				{
					smResult.setLastAttach( SmTexture::eDepth
						, pass.addOutputDepthTarget( smResult.getTargetViewId( SmTexture::eDepth ), getClearValue( SmTexture::eDepth ).depthStencil() ) );
					smResult.setLastAttach( SmTexture::eLinearDepth
						, pass.addOutputColourTarget( smResult.getTargetViewId( SmTexture::eLinearDepth ), getClearValue( SmTexture::eLinearDepth ).color() ) );

					if ( vsm )
					{
						smResult.setLastAttach( SmTexture::eVariance
							, pass.addOutputColourTarget( smResult.getTargetViewId( SmTexture::eVariance ), getClearValue( SmTexture::eVariance ).color() ) );
					}

					if ( rsm )
					{
						smResult.setLastAttach( SmTexture::eNormal
							, pass.addOutputColourTarget( smResult.getTargetViewId( SmTexture::eNormal ), getClearValue( SmTexture::eNormal ).color() ) );
						smResult.setLastAttach( SmTexture::ePosition
							, pass.addOutputColourTarget( smResult.getTargetViewId( SmTexture::ePosition ), getClearValue( SmTexture::ePosition ).color() ) );
						smResult.setLastAttach( SmTexture::eFlux
							, pass.addOutputColourTarget( smResult.getTargetViewId( SmTexture::eFlux ), getClearValue( SmTexture::eFlux ).color() ) );
					}
				}
				else
				{
					smResult.setLastAttach( SmTexture::eDepth
						, pass.addInOutDepthTarget( *smResult.getLastAttach( SmTexture::eDepth ) ) );
					smResult.setLastAttach( SmTexture::eLinearDepth
						, pass.addInOutColourTarget( *smResult.getLastAttach( SmTexture::eLinearDepth) ) );

					if ( vsm )
					{
						smResult.setLastAttach( SmTexture::eVariance
							, pass.addInOutColourTarget( *smResult.getLastAttach( SmTexture::eVariance ) ) );
					}

					if ( rsm )
					{
						smResult.setLastAttach( SmTexture::eNormal
							, pass.addInOutColourTarget( *smResult.getLastAttach( SmTexture::eNormal ) ) );
						smResult.setLastAttach( SmTexture::ePosition
							, pass.addInOutColourTarget( *smResult.getLastAttach( SmTexture::ePosition ) ) );
						smResult.setLastAttach( SmTexture::eFlux
							, pass.addInOutColourTarget( *smResult.getLastAttach( SmTexture::eFlux ) ) );
					}
				}

				if ( isStatic )
				{
					ShadowMapResult & nstSmResult = getShadowPassResult( false );
					auto & copyPass = graph.createPass( "CopyToNonStatic"
						, [this, isStatic, cascade]( crg::FramePass const & framePass
							, crg::GraphContext & context
							, crg::RunnableGraph & runnableGraph )
						{
							auto result = makeRawUnique< crg::ImageCopy >( framePass
								, context
								, runnableGraph
								, getShadowPassResult( isStatic ).getExtent()
								, ImageLayout::eShaderReadOnly
								, crg::ru::Config{}
								, crg::ImageCopy::GetPassIndexCallback( [](){ return 0u; } )
								, crg::ImageCopy::IsEnabledCallback( [this, cascade](){ return doEnableCopyStatic( cascade ); } ) );
							getOwner()->registerTimer( makeString( framePass.getFullName() )
								, result->getTimer() );
							return result;
						} );
					copyPass.addInputTransfer( *smResult.getLastAttach( SmTexture::eDepth ) );
					nstSmResult.setLastAttach( SmTexture::eDepth
						, copyPass.addOutputTransferImage( nstSmResult.getTargetViewId( SmTexture::eDepth ) ) );
					copyPass.addInputTransfer( *smResult.getLastAttach( SmTexture::eLinearDepth ) );
					nstSmResult.setLastAttach( SmTexture::eLinearDepth
						, copyPass.addOutputTransferImage( nstSmResult.getTargetViewId( SmTexture::eLinearDepth ) ) );

					if ( vsm )
					{
						copyPass.addInputTransfer( *smResult.getLastAttach( SmTexture::eVariance ) );
						nstSmResult.setLastAttach( SmTexture::eVariance
							, copyPass.addOutputTransferImage( nstSmResult.getTargetViewId( SmTexture::eVariance ) ) );
					}

					if ( rsm )
					{
						copyPass.addInputTransfer( *smResult.getLastAttach( SmTexture::eNormal ) );
						nstSmResult.setLastAttach( SmTexture::eNormal
							, copyPass.addOutputTransferImage( nstSmResult.getTargetViewId( SmTexture::eNormal ) ) );
						copyPass.addInputTransfer( *smResult.getLastAttach( SmTexture::ePosition ) );
						nstSmResult.setLastAttach( SmTexture::ePosition
							, copyPass.addOutputTransferImage( nstSmResult.getTargetViewId( SmTexture::ePosition ) ) );
						copyPass.addInputTransfer( *smResult.getLastAttach( SmTexture::eFlux ) );
						nstSmResult.setLastAttach( SmTexture::eFlux
							, copyPass.addOutputTransferImage( nstSmResult.getTargetViewId( SmTexture::eFlux ) ) );
					}
				}
				else if ( vsm )
				{
					passes.blurs.push_back( makeUnique< GaussianBlur >( group
						, m_device
						, cuT( "ShadowMapDirectional" )
						, *smResult.getLastAttach( SmTexture::eVariance )
						, 5u ) );
					smResult.setLastAttach( SmTexture::eVariance, &passes.blurs.back()->getResultAttach() );
				}
			}
			else
			{
				if ( isStatic )
				{
					smResult.setLastAttach( SmTexture::eDepth, cascade
						, pass.addOutputDepthTarget( smResult.getTargetViewId( SmTexture::eDepth, cascade ), getClearValue( SmTexture::eDepth ).depthStencil() ) );
					smResult.setLastAttach( SmTexture::eLinearDepth, cascade
						, pass.addOutputColourTarget( smResult.getTargetViewId( SmTexture::eLinearDepth, cascade ), getClearValue( SmTexture::eLinearDepth ).color() ) );

					if ( vsm )
					{
						smResult.setLastAttach( SmTexture::eVariance, cascade
							, pass.addOutputColourTarget( smResult.getTargetViewId( SmTexture::eVariance, cascade ), getClearValue( SmTexture::eVariance ).color() ) );
					}

					if ( rsm )
					{
						smResult.setLastAttach( SmTexture::eNormal, cascade
							, pass.addOutputColourTarget( smResult.getTargetViewId( SmTexture::eNormal, cascade ), getClearValue( SmTexture::eNormal ).color() ) );
						smResult.setLastAttach( SmTexture::ePosition, cascade
							, pass.addOutputColourTarget( smResult.getTargetViewId( SmTexture::ePosition, cascade ), getClearValue( SmTexture::ePosition ).color() ) );
						smResult.setLastAttach( SmTexture::eFlux, cascade
							, pass.addOutputColourTarget( smResult.getTargetViewId( SmTexture::eFlux, cascade ), getClearValue( SmTexture::eFlux ).color() ) );
					}
				}
				else
				{
					smResult.setLastAttach( SmTexture::eDepth, cascade
						, pass.addInOutDepthTarget( *smResult.getLastAttach( SmTexture::eDepth, cascade ) ) );
					smResult.setLastAttach( SmTexture::eLinearDepth, cascade
						, pass.addInOutColourTarget( *smResult.getLastAttach( SmTexture::eLinearDepth, cascade ) ) );

					if ( vsm )
					{
						smResult.setLastAttach( SmTexture::eVariance, cascade
							, pass.addInOutColourTarget( *smResult.getLastAttach( SmTexture::eVariance, cascade ) ) );
					}

					if ( rsm )
					{
						smResult.setLastAttach( SmTexture::eNormal, cascade
							, pass.addInOutColourTarget( *smResult.getLastAttach( SmTexture::eNormal, cascade ) ) );
						smResult.setLastAttach( SmTexture::ePosition, cascade
							, pass.addInOutColourTarget( *smResult.getLastAttach( SmTexture::ePosition, cascade ) ) );
						smResult.setLastAttach( SmTexture::eFlux, cascade
							, pass.addInOutColourTarget( *smResult.getLastAttach( SmTexture::eFlux, cascade ) ) );
					}
				}

				if ( isStatic )
				{
					ShadowMapResult & nstSmResult = getShadowPassResult( false );
					auto & copyPass = group.createPass( "CopyToNonStatic"
						, [this, isStatic, cascade]( crg::FramePass const & framePass
							, crg::GraphContext & context
							, crg::RunnableGraph & runnableGraph )
						{
							auto result = makeRawUnique< crg::ImageCopy >( framePass
								, context
								, runnableGraph
								, getShadowPassResult( isStatic ).getExtent()
								, ImageLayout::eShaderReadOnly
								, crg::ru::Config{}
								, crg::ImageCopy::GetPassIndexCallback( [](){ return 0u; } )
								, crg::ImageCopy::IsEnabledCallback( [this, cascade](){ return doEnableCopyStatic( cascade ); } ) );
							getOwner()->registerTimer( makeString( framePass.getFullName() )
								, result->getTimer() );
							return result;
						} );
					copyPass.addInputTransfer( *smResult.getLastAttach( SmTexture::eDepth, cascade ) );
					nstSmResult.setLastAttach( SmTexture::eDepth, cascade
							, copyPass.addOutputTransferImage( nstSmResult.getTargetViewId( SmTexture::eDepth, cascade ) ) );
					copyPass.addInputTransfer( *smResult.getLastAttach( SmTexture::eLinearDepth, cascade ) );
					nstSmResult.setLastAttach( SmTexture::eLinearDepth, cascade
							, copyPass.addOutputTransferImage( nstSmResult.getTargetViewId( SmTexture::eLinearDepth, cascade ) ) );

					if ( vsm )
					{
						copyPass.addInputTransfer( *smResult.getLastAttach( SmTexture::eVariance, cascade ) );
						nstSmResult.setLastAttach( SmTexture::eVariance, cascade
							, copyPass.addOutputTransferImage( nstSmResult.getTargetViewId( SmTexture::eVariance, cascade ) ) );
					}

					if ( rsm )
					{
						copyPass.addInputTransfer( *smResult.getLastAttach( SmTexture::eNormal, cascade ) );
						nstSmResult.setLastAttach( SmTexture::eNormal, cascade
							, copyPass.addOutputTransferImage( nstSmResult.getTargetViewId( SmTexture::eNormal, cascade ) ) );
						copyPass.addInputTransfer( *smResult.getLastAttach( SmTexture::ePosition, cascade ) );
						nstSmResult.setLastAttach( SmTexture::ePosition, cascade
							, copyPass.addOutputTransferImage( nstSmResult.getTargetViewId( SmTexture::ePosition, cascade ) ) );
						copyPass.addInputTransfer( *smResult.getLastAttach( SmTexture::eFlux, cascade ) );
						nstSmResult.setLastAttach( SmTexture::eFlux, cascade
							, copyPass.addOutputTransferImage( nstSmResult.getTargetViewId( SmTexture::eFlux, cascade ) ) );
					}
				}
				else if ( vsm )
				{
					passes.blurs.push_back( makeUnique< GaussianBlur >( group
						, m_device
						, cuT( "ShadowMapDirectional" )
						, *smResult.getLastAttach( SmTexture::eVariance, cascade )
						, m_blurIntermediateView
						, 5u
						, crg::ImageCopy::IsEnabledCallback( [this, cascade]() { return doEnableBlur( cascade ); } ) ) );
					smResult.setLastAttach( SmTexture::eVariance, cascade, &passes.blurs.back()->getResultAttach() );
				}
			}
		}
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
		for ( auto const & data : makeArrayView( passes.passes.begin()
			, passes.passes.begin() + std::min( m_cascades, uint32_t( passes.passes.size() ) ) ) )
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
		Camera const & camera = *updater.camera;
		DirectionalLightInstance const & directional = static_cast< DirectionalLightInstance & >( light );
		auto node = &light.getNode();
		m_shadowType = light.getShadowType();

		auto shadowModified = light.updateShadow( camera, nullptr, 0 );

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
				auto name = cuT( "CascadeFrustum" ) + string::toString( cascade );
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
