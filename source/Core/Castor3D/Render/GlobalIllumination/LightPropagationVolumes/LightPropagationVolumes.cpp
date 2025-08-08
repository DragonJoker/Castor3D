#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightPropagationVolumes.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Limits.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/GeometryInjectionPass.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightInjectionPass.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightPropagationPass.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LpvConfig.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapResult.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Light/DirectionalLight.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
#include "Castor3D/Scene/Light/SpotLight.hpp"
#include "Castor3D/Shader/Ubos/LpvGridConfigUbo.hpp"

#include <CastorUtils/Miscellaneous/StringUtils.hpp>

#include <ashespp/Buffer/Buffer.hpp>
#include <ashespp/Buffer/BufferView.hpp>

#include <RenderGraph/GraphContext.hpp>
#include <RenderGraph/RunnablePass.hpp>
#include <RenderGraph/RunnablePasses/ImageBlit.hpp>

namespace c3d
{
	//*********************************************************************************************

	namespace lpvpropvol
	{
		class LpvClear
			: public crg::RunnablePass
		{
		public:
			LpvClear( crg::FramePass const & pass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
				: crg::RunnablePass{ pass, context, graph
					, { crg::defaultV< InitialiseCallback >
						, GetPipelineStateCallback( [](){ return crg::getPipelineState( PipelineStageFlags::eTransfer ); } )
						, RecordCallback( [this]( crg::RecordContext const &, VkCommandBuffer cb, uint32_t ){ doRecordInto( cb ); } ) } }
			{
			}

		protected:
			void doRecordInto( VkCommandBuffer commandBuffer )
			{
				auto clearValue = convert( transparentBlackClearColor );

				for ( auto & [binding, attach] : m_pass.outputs )
				{
					auto view = attach->view();
					auto image = m_graph.createImage( view.data->image );
					auto subresourceRange = convert( view.data->info.subresourceRange );
					subresourceRange.layerCount = view.data->image.data->info.arrayLayers;
					assert( attach->isTransferOutputView() );
					m_context.vkCmdClearColorImage( commandBuffer
						, image
						, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
						, &clearValue
						, 1u
						, &subresourceRange );
				}
			}
		};

		static crg::ImageViewId createArrayView( crg::FramePassGroup const & graph
			, crg::ImageViewId cubeArrayView )
		{
			auto data = *cubeArrayView.data;
			data.info.viewType = ImageViewType::e2DArray;
			return graph.createView( data );
		}

		static Vector< LpvLightConfigUbo > createUbos( RenderDevice const & device
			, LightType lightType )
		{
			Vector< LpvLightConfigUbo > result;

			if ( lightType == LightType::ePoint )
			{
				for ( uint32_t i = 0u; i < 6u; ++i )
				{
					result.emplace_back( device );
				}
			}
			else
			{
				result.emplace_back( device );
			}

			return result;
		}
	}

	//*********************************************************************************************

	LightPropagationVolumesBase::LightLpv::LightLpv( crg::FramePassGroup & graph
		, RenderDevice const & device
		, String const & name
		, LightCache const & plightCache
		, LightType lightType
		, ShadowMapResult const & smResult
		, LpvGridConfigUbo const & lpvGridConfigUbo
		, LightVolumePassResult & injection
		, Texture * geometry )
		: lightCache{ plightCache }
		, lpvLightConfigUbos{ lpvpropvol::createUbos( device, lightType ) }
	{
		doCreateInjectionPasses( graph, device, name, lightType
			, smResult, lpvGridConfigUbo, injection );
		if ( geometry )
			doCreateGeometryPasses( graph, device, name, lightType
				, smResult, lpvGridConfigUbo, *geometry );
	}

	bool LightPropagationVolumesBase::LightLpv::update( CpuUpdater & updater
		, float lpvCellSize )
	{
		auto & sceneObjs = updater.dirtyScenes[lightCache.getScene()];
		auto & light = *updater.light;
		auto changed = sceneObjs.dirtyLights.end() != std::find( sceneObjs.dirtyLights.begin(), sceneObjs.dirtyLights.end(), &light )
			|| light.getLpvConfig().indirectAttenuation.isDirty()
			|| light.getLpvConfig().texelAreaModifier.isDirty();

		if ( changed )
		{
			uint32_t index = 0u;

			for ( auto & lpvLightConfigUbo : lpvLightConfigUbos )
			{
				lpvLightConfigUbo.cpuUpdate( light, lpvCellSize, index );
				++index;
			}
		}

		return changed;
	}

	void LightPropagationVolumesBase::LightLpv::doCreateInjectionPass( crg::FramePassGroup & graph
		, RenderDevice const & device
		, String const & name
		, LightType lightType
		, ShadowMapResult const & smResult
		, LpvGridConfigUbo const & lpvGridConfigUbo
		, LightVolumePassResult & injection )
	{
		auto rsmSize = smResult.getExtent().width;
		auto & pass = graph.createPass( toUtf8( name ) + "LightInjection"
			, [this, &device, lightType, rsmSize]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				auto result = makeRawUnique< LightInjectionPass >( framePass, context, runnableGraph
					, device, lightType, lightCache.getScene()->getLpvGridSize(), rsmSize );
				lightInjectionPasses.push_back( result.get() );
				device.renderSystem.getEngine()->registerTimer( makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} );
		lightCache.createPassBinding( pass, LightInjectionPass::LightsIdx );
		pass.addInputSampledImage( smResult.getSampledViewId( SmTexture::eNormal ), LightInjectionPass::RsmNormalsIdx );
		pass.addInputSampledImage( smResult.getSampledViewId( SmTexture::ePosition ), LightInjectionPass::RsmPositionIdx );
		pass.addInputSampledImage( smResult.getSampledViewId( SmTexture::eFlux ), LightInjectionPass::RsmFluxIdx );
		lpvGridConfigUbo.createPassBinding( pass, LightInjectionPass::LpvGridUboIdx );
		lpvLightConfigUbos[0].createPassBinding( pass, LightInjectionPass::LpvLightUboIdx );

		injection.setLastAttach( LpvTexture::eR, pass.addInOutColourTarget( *injection.getLastAttach( LpvTexture::eR ) ) );
		injection.setLastAttach( LpvTexture::eG, pass.addInOutColourTarget( *injection.getLastAttach( LpvTexture::eG ) ) );
		injection.setLastAttach( LpvTexture::eB, pass.addInOutColourTarget( *injection.getLastAttach( LpvTexture::eB ) ) );
	}

	void LightPropagationVolumesBase::LightLpv::doCreateInjectionPass( crg::FramePassGroup & graph
		, RenderDevice const & device
		, String const & name
		, Vector< crg::ImageViewId > const & arrayViews
		, CubeMapFace face
		, ShadowMapResult const & smResult
		, LpvGridConfigUbo const & lpvGridConfigUbo
		, LightVolumePassResult & injection )
	{
		auto rsmSize = smResult.getExtent().width;
		auto & pass = graph.createPass( toUtf8( name + c3d::getName( face ) ) + "LightInjection"
			, [this, &device, face, rsmSize]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				auto result = makeRawUnique< LightInjectionPass >( framePass, context, runnableGraph
					, device, face, lightCache.getScene()->getLpvGridSize(), rsmSize );
				lightInjectionPasses.push_back( result.get() );
				device.renderSystem.getEngine()->registerTimer( makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} );
		lightCache.createPassBinding( pass, LightInjectionPass::LightsIdx );
		pass.addInputSampledImage( arrayViews[0u], LightInjectionPass::RsmNormalsIdx );
		pass.addInputSampledImage( arrayViews[1u], LightInjectionPass::RsmPositionIdx );
		pass.addInputSampledImage( arrayViews[2u], LightInjectionPass::RsmFluxIdx );
		lpvGridConfigUbo.createPassBinding( pass, LightInjectionPass::LpvGridUboIdx );
		lpvLightConfigUbos[uint32_t( face )].createPassBinding( pass, LightInjectionPass::LpvLightUboIdx );

		injection.setLastAttach( LpvTexture::eR, pass.addInOutColourTarget( *injection.getLastAttach( LpvTexture::eR ) ) );
		injection.setLastAttach( LpvTexture::eG, pass.addInOutColourTarget( *injection.getLastAttach( LpvTexture::eG ) ) );
		injection.setLastAttach( LpvTexture::eB, pass.addInOutColourTarget( *injection.getLastAttach( LpvTexture::eB ) ) );
	}

	void LightPropagationVolumesBase::LightLpv::doCreateInjectionPasses( crg::FramePassGroup & graph
		, RenderDevice const & device
		, String const & name
		, LightType lightType
		, ShadowMapResult const & smResult
		, LpvGridConfigUbo const & lpvGridConfigUbo
		, LightVolumePassResult & injection )
	{
		if ( lightType != LightType::ePoint )
		{
			doCreateInjectionPass( graph, device, name, lightType
				, smResult, lpvGridConfigUbo, injection );
		}
		else
		{
			Vector< crg::ImageViewId > arrayViews;
			arrayViews.push_back( lpvpropvol::createArrayView( graph, smResult.getSampledViewId( SmTexture::eNormal ) ) );
			arrayViews.push_back( lpvpropvol::createArrayView( graph, smResult.getSampledViewId( SmTexture::ePosition ) ) );
			arrayViews.push_back( lpvpropvol::createArrayView( graph, smResult.getSampledViewId( SmTexture::eFlux ) ) );

			for ( uint32_t faceIndex = 0u; faceIndex < 6u; ++faceIndex )
			{
				doCreateInjectionPass( graph, device, name
					, arrayViews, CubeMapFace( faceIndex )
					, smResult, lpvGridConfigUbo, injection );
			}
		}
	}

	void LightPropagationVolumesBase::LightLpv::doCreateGeometryPass( crg::FramePassGroup & graph
		, RenderDevice const & device
		, String const & name
		, LightType lightType
		, ShadowMapResult const & smResult
		, LpvGridConfigUbo const & lpvGridConfigUbo
		, Texture & geometry )
	{
		auto rsmSize = smResult.getExtent().width;
		auto & pass = graph.createPass( toUtf8( name ) + "GeomInjection"
			, [this, &device, lightType, rsmSize]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				auto result = makeRawUnique< GeometryInjectionPass >( framePass, context, runnableGraph
					, device, lightType, lightCache.getScene()->getLpvGridSize(), rsmSize );
				geometryInjectionPasses.push_back( result.get() );
				device.renderSystem.getEngine()->registerTimer( makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} );
		lightCache.createPassBinding( pass, GeometryInjectionPass::LightsIdx );
		pass.addInputSampledImage( smResult.getSampledViewId( SmTexture::eNormal ), GeometryInjectionPass::RsmNormalsIdx );
		pass.addInputSampledImage( smResult.getSampledViewId( SmTexture::ePosition ), GeometryInjectionPass::RsmPositionIdx );
		lpvGridConfigUbo.createPassBinding( pass, GeometryInjectionPass::LpvGridUboIdx );
		lpvLightConfigUbos[0].createPassBinding( pass, GeometryInjectionPass::LpvLightUboIdx );

		geometry.setLastAttach( pass.addInOutColourTarget( *geometry.getLastAttach() ) );
	}

	void LightPropagationVolumesBase::LightLpv::doCreateGeometryPass( crg::FramePassGroup & graph
		, RenderDevice const & device
		, String const & name
		, Vector< crg::ImageViewId > const & arrayViews
		, CubeMapFace face
		, ShadowMapResult const & smResult
		, LpvGridConfigUbo const & lpvGridConfigUbo
		, Texture & geometry )
	{
		auto rsmSize = smResult.getExtent().width;
		auto & pass = graph.createPass( toUtf8( name + c3d::getName( face ) ) + "GeomInjection"
			, [this, &device, face, rsmSize]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				auto result = makeRawUnique< GeometryInjectionPass >( framePass, context, runnableGraph
					, device, face, lightCache.getScene()->getLpvGridSize(), rsmSize );
				geometryInjectionPasses.push_back( result.get() );
				device.renderSystem.getEngine()->registerTimer( makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} );
		lightCache.createPassBinding( pass, GeometryInjectionPass::LightsIdx );
		pass.addInputSampledImage( arrayViews[0u], GeometryInjectionPass::RsmNormalsIdx );
		pass.addInputSampledImage( arrayViews[1u], GeometryInjectionPass::RsmPositionIdx );
		lpvGridConfigUbo.createPassBinding( pass, GeometryInjectionPass::LpvGridUboIdx );
		lpvLightConfigUbos[uint32_t( face )].createPassBinding( pass, GeometryInjectionPass::LpvLightUboIdx );

		geometry.setLastAttach( pass.addInOutColourTarget( *geometry.getLastAttach() ) );
	}

	void LightPropagationVolumesBase::LightLpv::doCreateGeometryPasses( crg::FramePassGroup & graph
		, RenderDevice const & device
		, String const & name
		, LightType lightType
		, ShadowMapResult const & smResult
		, LpvGridConfigUbo const & lpvGridConfigUbo
		, Texture & geometry )
	{
		if ( lightType != LightType::ePoint )
		{
			doCreateGeometryPass( graph, device, name, lightType
				, smResult, lpvGridConfigUbo, geometry );
		}
		else
		{
			Vector< crg::ImageViewId > arrayViews;
			arrayViews.push_back( lpvpropvol::createArrayView( graph, smResult.getSampledViewId( SmTexture::eNormal ) ) );
			arrayViews.push_back( lpvpropvol::createArrayView( graph, smResult.getSampledViewId( SmTexture::ePosition ) ) );

			for ( uint32_t faceIndex = 0u; faceIndex < 6u; ++faceIndex )
			{
				doCreateGeometryPass( graph, device, name
					, arrayViews, CubeMapFace( faceIndex )
					, smResult, lpvGridConfigUbo, geometry );
			}
		}
	}

	//*********************************************************************************************

	LightPropagationVolumesBase::LightPropagationVolumesBase( crg::ResourcesCache & resources
		, Scene const & scene
		, LightType lightType
		, RenderDevice const & device
		, ShadowMapResult const & smResult
		, LightVolumePassResult & lpvResult
		, LpvGridConfigUbo & lpvGridConfigUbo
		, bool geometryVolumes )
		: Named{ cuT( "LPV" ) + ( geometryVolumes ? String( cuT( "G" ) ) : String( cuT( "" ) ) ) }
		, m_scene{ scene }
		, m_device{ device }
		, m_sourceSmResult{ smResult }
		, m_downsampledSmResult{ ( lightType == LightType::eDirectional
			? makeRawUnique< ShadowMapResult >( resources
				, device
				, cuT( "LPV" )
				, ( ( lightType == LightType::ePoint ) ? ImageCreateFlags::eCubeCompatible : ImageCreateFlags::eNone )
				, Size{ 512u, 512u }
				, smResult.getArrayLayers() )
			: nullptr ) }
		, m_usedSmResult{ m_downsampledSmResult ? m_downsampledSmResult.get() : &m_sourceSmResult }
		, m_lpvResult{ lpvResult }
		, m_lpvGridConfigUbo{ lpvGridConfigUbo }
		, m_geometryVolumes{ geometryVolumes }
		, m_graph{ resources.getHandler(), toUtf8( getName() ) }
		, m_lightType{ lightType }
		, m_injection{ resources
			, m_device
			, getName() + cuT( "LightInjection0" )
			, m_scene.getLpvGridSize() }
		, m_geometry{ ( geometryVolumes
			? GeometryInjectionPass::createResult( resources
				, m_device
				, getName()
				, 0u
				, m_scene.getLpvGridSize() )
			: Texture{} ) }
		, m_propagate{ LightVolumePassResult{ resources
				, m_device
				, getName() + cuT( "Propagate0" )
				, m_scene.getLpvGridSize() }
			, LightVolumePassResult{ resources
				, m_device
				, getName() + cuT( "Propagate1" )
				, m_scene.getLpvGridSize() } }
	{
		doCreateClearPass();
		if ( m_downsampledSmResult )
			doCreateDownsamplePass();

		m_injection.create();
		m_geometry.create();

		for ( auto const & value : m_propagate )
		{
			value.create();
		}

		m_graph.addInput( m_sourceSmResult.getTargetViewId( SmTexture::eNormal )
			, makeLayoutState( ImageLayout::eShaderReadOnly ) );
		m_graph.addInput( m_sourceSmResult.getTargetViewId( SmTexture::ePosition )
			, makeLayoutState( ImageLayout::eShaderReadOnly ) );
		m_graph.addInput( m_sourceSmResult.getTargetViewId( SmTexture::eFlux )
			, makeLayoutState( ImageLayout::eShaderReadOnly ) );

		m_graph.addOutput( lpvResult.getTargetViewId( LpvTexture::eR )
			, makeLayoutState( ImageLayout::eShaderReadOnly ) );
		m_graph.addOutput( lpvResult.getTargetViewId( LpvTexture::eG )
			, makeLayoutState( ImageLayout::eShaderReadOnly ) );
		m_graph.addOutput( lpvResult.getTargetViewId( LpvTexture::eB )
			, makeLayoutState( ImageLayout::eShaderReadOnly ) );
	}

	void LightPropagationVolumesBase::initialise()
	{
		if ( !m_initialised
			&& m_scene.needsGlobalIllumination( m_lightType
				, ( m_geometryVolumes
					? GlobalIlluminationType::eLpvG
					: GlobalIlluminationType::eLpv ) ) )
		{
			m_aabb = m_scene.getBoundingBox();
			doCreatePropagationPasses();
			m_runnable = m_graph.compile( m_device.makeContext() );
			m_scene.getEngine()->registerTimer( makeString( m_runnable->getName() + "/Graph" )
				, m_runnable->getTimer() );
			printGraph( *m_runnable );
			m_recordEvent = m_device.renderSystem.getEngine()->postEvent( makeGpuFunctorEvent( GpuEventType::ePreUpload
				, [this]( RenderDevice const &
					, QueueData const & )
				{
					m_runnable->record();
					m_recordEvent = nullptr;
				} ) );
			m_initialised = true;
		}
	}

	void LightPropagationVolumesBase::cleanup()noexcept
	{
		if ( m_recordEvent )
			m_recordEvent->skip();
		m_initialised = false;
		m_lightLpvs.clear();
		m_lightPropagationPasses = {};
	}

	void LightPropagationVolumesBase::registerLight( LightInstance * light )
	{
		if ( auto [it, res] = m_lightLpvs.try_emplace( light );
			res )
		{
			auto & group = m_graph.createPassGroup( toUtf8( light->getName() ) );
			it->second = makeRawUnique< LightLpv >( group
				, m_device
				, getName() + light->getName()
				, light->getScene()->getLightCache()
				, m_lightType
				, *m_usedSmResult
				, m_lpvGridConfigUbo
				, m_injection
				, ( m_geometryVolumes? &m_geometry: nullptr ) );

			if ( m_lightPropagationFirstPass )
			{
				m_lightPropagationFirstPass->addImplicit( *m_injection.getLastAttach( LpvTexture::eR ), ImageLayout::eShaderReadOnly );
				m_lightPropagationFirstPass->addImplicit( *m_injection.getLastAttach( LpvTexture::eG ), ImageLayout::eShaderReadOnly );
				m_lightPropagationFirstPass->addImplicit( *m_injection.getLastAttach( LpvTexture::eB ), ImageLayout::eShaderReadOnly );
				m_lightPropagationFirstPass = nullptr;
			}

			if ( m_runnable )
			{
				m_scene.getEngine()->unregisterTimer( makeString( m_runnable->getName() + "/Graph" )
					, m_runnable->getTimer() );
				m_runnable.reset();
				m_runnable = m_graph.compile( m_device.makeContext() );
				m_scene.getEngine()->registerTimer( makeString( m_runnable->getName() ) + cuT( "/Graph" )
					, m_runnable->getTimer() );
				printGraph( *m_runnable );

				if ( m_recordEvent )
					m_recordEvent->skip();
				m_recordEvent = m_device.renderSystem.getEngine()->postEvent( makeGpuFunctorEvent( GpuEventType::ePreUpload
					, [this]( RenderDevice const &
						, QueueData const & )
					{
						m_runnable->record();
						m_recordEvent = nullptr;
					} ) );
			}
		}
	}

	void LightPropagationVolumesBase::update( CpuUpdater & updater )
	{
		if ( !m_initialised
			|| !m_scene.needsGlobalIllumination( m_lightType
				, ( m_geometryVolumes
					? GlobalIlluminationType::eLpvG
					: GlobalIlluminationType::eLpv ) ) )
		{
			return;
		}

		auto const & camera = *updater.camera;
		auto aabb = m_scene.getBoundingBox();
		auto camPos = camera.getParent()->getDerivedPosition();
		Point3f camDir{ 0, 0, 1 };
		camera.getParent()->getDerivedOrientation().transform( camDir, camDir );
		auto changed = m_aabb != aabb
			|| m_cameraPos != camPos
			|| m_cameraDir != camDir;

		for ( auto const & [light, lpv] : m_lightLpvs )
		{
			updater.light = light;
			changed = lpv->update( updater
				, std::max( std::max( aabb.getDimensions()->x
					, aabb.getDimensions()->y )
					, aabb.getDimensions()->z ) / float( m_scene.getLpvGridSize() ) )
				|| changed;
		}

		if ( changed )
		{
			m_aabb = aabb;
			m_cameraPos = camPos;
			m_cameraDir = camDir;
			auto & grid = m_lpvGridConfigUbo.cpuUpdate( m_aabb
				, m_cameraPos
				, m_scene.getLpvGridSize()
				, m_scene.getLpvIndirectAttenuation() );
			m_gridsSize = Point4f{ grid.getCenter()->x
				, grid.getCenter()->y
				, grid.getCenter()->z
				, grid.getCellSize() };
		}
	}

	SemaphoreWaitArray LightPropagationVolumesBase::render( SemaphoreWaitArray const & toWait
		, ashes::Queue const & queue )
	{
		if ( !m_initialised
			|| !m_scene.needsGlobalIllumination( m_lightType
				, ( m_geometryVolumes
					? GlobalIlluminationType::eLpvG
					: GlobalIlluminationType::eLpv ) )
			|| m_lightLpvs.empty() )
		{
			return toWait;
		}

		return m_runnable->run( toWait, queue );
	}

	void LightPropagationVolumesBase::accept( ConfigurationVisitorBase & visitor )const
	{
		if ( m_initialised )
		{
			for ( auto const & [light, lpv] : m_lightLpvs )
			{
				for ( auto & lightInjectionPass : lpv->lightInjectionPasses )
				{
					if ( lightInjectionPass )
						lightInjectionPass->accept( visitor );
				}

				if ( m_geometryVolumes )
				{
					for ( auto const & geometryInjectionPass : lpv->geometryInjectionPasses )
					{
						if ( geometryInjectionPass )
							geometryInjectionPass->accept( visitor );
					}
				}
			}

			for ( auto const & pass : m_lightPropagationPasses )
			{
				if ( pass )
					pass->accept( visitor );
			}

			for ( auto i = 0u; i < uint32_t( LpvTexture::eCount ); ++i )
			{
				auto tex = LpvTexture( i );
				visitor.visit( cuT( "LPV Injection " ) + getTexName( tex )
					, m_injection.getTexture( tex )
					, m_graph.getFinalLayoutState( m_injection.getWholeViewId( tex ) ).layout
					, TextureFactors::tex3D( &m_gridsSize ) );
			}

			if ( m_geometryVolumes )
			{
				visitor.visit( cuT( "LPV Geometry" )
					, m_geometry
					, m_graph.getFinalLayoutState( m_geometry.getWholeViewId() ).layout
					, TextureFactors::tex3D( &m_gridsSize ) );
			}

			uint32_t level = 0u;

			for ( auto const & propagate : m_propagate )
			{
				for ( auto i = 0u; i < uint32_t( LpvTexture::eCount ); ++i )
				{
					auto tex = LpvTexture( i );
					visitor.visit( cuT( "LPV Propagation" ) + string::toString( level ) + cuT( " " ) + getTexName( tex )
						, propagate.getTexture( tex )
						, m_graph.getFinalLayoutState( propagate.getWholeViewId( tex ) ).layout
						, TextureFactors::tex3D( &m_gridsSize ) );
				}

				++level;
			}
		}
	}

	void LightPropagationVolumesBase::doCreateClearPass()
	{
		auto & result = m_graph.createPass( "LpvClearInjection"
			, []( crg::FramePass const & pass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				return makeRawUnique< lpvpropvol::LpvClear >( pass
					, context
					, graph );
			} );

		for ( auto & texture : m_injection )
			texture->setLastAttach( result.addOutputTransferImage( texture->getTargetViewId() ) );
		if ( m_geometryVolumes )
			m_geometry.setLastAttach( result.addOutputTransferImage( m_geometry.getTargetViewId() ) );
	}

	void LightPropagationVolumesBase::doCreateDownsamplePass()
	{
		auto extent = m_sourceSmResult.getExtent();

		for ( auto i = uint32_t( SmTexture::eNormal ); i < uint32_t( SmTexture::eCount ); ++i )
		{
			auto smTexture = SmTexture( i );
			auto & pass = m_graph.createPass( "LpvDownsampleShadowMap/" + toUtf8( getTexName( smTexture ) )
				, [extent]( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & graph )
				{
					return makeRawUnique< crg::ImageBlit >( framePass, context, graph
						, Rect3D{ Offset3D{}, extent }
						, Rect3D{ Offset3D{}, Extent3D{ 512u, 512u, 1u } }
						, FilterMode::eLinear );
				} );
			pass.addInputTransferImage( m_sourceSmResult.getSampledViewId( smTexture ) );
			m_downsampledSmResult->setLastAttach( smTexture, pass.addOutputTransferImage( m_downsampledSmResult->getWholeViewId( smTexture ) ) );
		}
	}

	crg::FramePass & LightPropagationVolumesBase::doCreatePropagationPass( String const & name
		, LightVolumePassResult const & injection
		, LightVolumePassResult & lpvResult
		, LightVolumePassResult & propagation
		, uint32_t index )
	{
		auto & result = m_graph.createPass( toUtf8( name )
			, [this, index]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				auto res = makeRawUnique< LightPropagationPass >( framePass, context, runnableGraph, m_device
					, m_geometryVolumes && index > 0u, m_scene.getLpvGridSize()
					, ( index == 0u ? BlendMode::eNoBlend : BlendMode::eAdditive ) );
				m_lightPropagationPasses.push_back( res.get() );
				m_device.renderSystem.getEngine()->registerTimer( makeString( framePass.getFullName() )
					, res->getTimer() );
				return res;
			} );
		m_lpvGridConfigUbo.createPassBinding( result, LightPropagationPass::LpvGridUboIdx );
		result.addInputSampled( *injection.getSampledLastAttach( LpvTexture::eR ), LightPropagationPass::RLpvGridIdx
			, crg::SamplerDesc{ FilterMode::eLinear, FilterMode::eLinear, MipmapMode::eLinear } );
		result.addInputSampled( *injection.getSampledLastAttach( LpvTexture::eG ), LightPropagationPass::GLpvGridIdx
			, crg::SamplerDesc{ FilterMode::eLinear, FilterMode::eLinear, MipmapMode::eLinear } );
		result.addInputSampled( *injection.getSampledLastAttach( LpvTexture::eB ), LightPropagationPass::BLpvGridIdx
			, crg::SamplerDesc{ FilterMode::eLinear, FilterMode::eLinear, MipmapMode::eLinear } );

		if ( index > 0u && m_geometryVolumes )
		{
			result.addInputSampled( *m_geometry.getSampledLastAttach(), LightPropagationPass::GpGridIdx
				, crg::SamplerDesc{ FilterMode::eLinear, FilterMode::eLinear, MipmapMode::eLinear } );
		}

		if ( index == 0u )
		{
			lpvResult.setLastAttach( LpvTexture::eR, result.addOutputColourTarget( lpvResult.getTargetViewId( LpvTexture::eR ) ) );
			lpvResult.setLastAttach( LpvTexture::eG, result.addOutputColourTarget( lpvResult.getTargetViewId( LpvTexture::eG ) ) );
			lpvResult.setLastAttach( LpvTexture::eB, result.addOutputColourTarget( lpvResult.getTargetViewId( LpvTexture::eB ) ) );
		}
		else
		{
			lpvResult.setLastAttach( LpvTexture::eR, result.addInOutColourTarget( *lpvResult.getLastAttach( LpvTexture::eR ) ) );
			lpvResult.setLastAttach( LpvTexture::eG, result.addInOutColourTarget( *lpvResult.getLastAttach( LpvTexture::eG ) ) );
			lpvResult.setLastAttach( LpvTexture::eB, result.addInOutColourTarget( *lpvResult.getLastAttach( LpvTexture::eB ) ) );
		}

		if ( index <= 1u )
		{
			propagation.setLastAttach( LpvTexture::eR, result.addOutputColourTarget( propagation.getTargetViewId( LpvTexture::eR ) ) );
			propagation.setLastAttach( LpvTexture::eG, result.addOutputColourTarget( propagation.getTargetViewId( LpvTexture::eG ) ) );
			propagation.setLastAttach( LpvTexture::eB, result.addOutputColourTarget( propagation.getTargetViewId( LpvTexture::eB ) ) );
		}
		else
		{
			propagation.setLastAttach( LpvTexture::eR, result.addInOutColourTarget( *propagation.getLastAttach( LpvTexture::eR ) ) );
			propagation.setLastAttach( LpvTexture::eG, result.addInOutColourTarget( *propagation.getLastAttach( LpvTexture::eG ) ) );
			propagation.setLastAttach( LpvTexture::eB, result.addInOutColourTarget( *propagation.getLastAttach( LpvTexture::eB ) ) );
		}

		return result;
	}

	void LightPropagationVolumesBase::doCreatePropagationPasses()
	{
		uint32_t propIndex = 0u;
		auto const * input = &m_injection;
		auto * output = &m_propagate[propIndex];
		m_lightPropagationFirstPass = &doCreatePropagationPass( cuT( "PropagationNoOccNoBlend" )
			, *input
			, m_lpvResult
			, *output
			, 0u );
		String name = ( m_geometryVolumes
			? String{ cuT( "OccBlend" ) }
			: String{ cuT( "NoOccBlend" ) } );

		for ( uint32_t i = 1u; i < LpvMaxPropagationSteps; ++i )
		{
			input = &m_propagate[propIndex];
			propIndex = 1u - propIndex;
			output = &m_propagate[propIndex];
			doCreatePropagationPass( cuT( "Propagation" ) + name + string::toString( i )
				, *input
				, m_lpvResult
				, *output
				, i );
		}
	}

	//*********************************************************************************************
}
