#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LayeredLightPropagationVolumes.hpp"

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
#include "Castor3D/Shader/Ubos/LayeredLpvGridConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/LpvGridConfigUbo.hpp"

#include <RenderGraph/GraphContext.hpp>
#include <RenderGraph/RunnablePass.hpp>
#include <RenderGraph/RunnablePasses/ImageBlit.hpp>

namespace c3d
{
	//*********************************************************************************************

	namespace llpvpropvol
	{
		class LpvClear
			: public crg::RunnablePass
		{
		public:
			LpvClear( crg::FramePass const & pass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
				: crg::RunnablePass{ pass
					, context
					, graph
					, { crg::defaultV< InitialiseCallback >
						, GetPipelineStateCallback( [](){ return crg::getPipelineState( PipelineStageFlags::eTransfer ); } )
						, RecordCallback( [this]( crg::RecordContext &, VkCommandBuffer cb, uint32_t i ){ doRecordInto( cb, i ); } ) } }
			{
			}

		private:
			void doRecordInto( VkCommandBuffer commandBuffer
				, uint32_t index )
			{
				auto clearValue = convert( transparentBlackClearColor );

				for ( auto & [_, attach] : m_pass.outputs )
				{
					auto view = attach->view( index );
					auto image = m_graph.createImage( view.data->image );
					auto subresourceRange = convert( view.data->info.subresourceRange );
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

		static Vector< LightVolumePassResult > createInjection( crg::ResourcesCache & resources
			, RenderDevice const & device
			, String const & name
			, uint32_t lpvGridSize
			, uint32_t cascadeCount )
		{
			Vector< LightVolumePassResult > result;

			for ( uint32_t cascade = 0u; cascade < cascadeCount; ++cascade )
			{
				result.emplace_back( resources
					, device
					, name + cuT( "Injection" ) + string::toString( cascade )
					, lpvGridSize );
			}

			return result;
		}

		static TextureArray createGeometry( crg::ResourcesCache & resources
			, RenderDevice const & device
			, String const & name
			, uint32_t lpvGridSize
			, uint32_t cascadeCount
			, bool enabled )
		{
			TextureArray result;

			for ( uint32_t cascade = 0u; cascade < cascadeCount; ++cascade )
			{
				result.push_back( enabled
					? GeometryInjectionPass::createResult( resources
						, device
						, name
						, cascade
						, lpvGridSize )
					: Texture{} );
			}

			return result;
		}

		static Vector< Array< LightVolumePassResult, 2u > > createPropagation( crg::ResourcesCache & resources
			, RenderDevice const & device
			, String const & name
			, uint32_t lpvGridSize
			, uint32_t cascadeCount )
		{
			Vector< Array< LightVolumePassResult, 2u > > result;

			for ( uint32_t cascade = 0u; cascade < cascadeCount; ++cascade )
			{
				result.push_back( { LightVolumePassResult{ resources, device, name + cuT( "Propagate" ) + string::toString( cascade ) + cuT( "0" ), lpvGridSize }
					, LightVolumePassResult{ resources, device, name + cuT( "Propagate" ) + string::toString( cascade ) + cuT( "1" ), lpvGridSize } } );
			}

			return result;
		}
	}

	//*********************************************************************************************

	LayeredLightPropagationVolumesBase::LightLpv::LightLpv( crg::FramePassGroup & graph
		, RenderDevice const & device
		, String const & name
		, LightCache const & plightCache
		, LightType lightType
		, ShadowMapResult const & smResult
		, LpvGridConfigUboArray const & lpvGridConfigUbos
		, Vector< LightVolumePassResult > & injection
		, TextureArray * geometry )
		: lightCache{ plightCache }
	{
		for ( uint32_t cascade = 0u; cascade < LpvMaxCascadesCount; ++cascade )
		{
			lpvLightConfigUbos.emplace_back( device );
			doCreateInjectionPass( graph, device, name, lightType
				, smResult, lpvGridConfigUbos, injection, cascade );
			if ( geometry )
				doCreateGeometryPass( graph, device, name, lightType
					, smResult, lpvGridConfigUbos, *geometry, cascade );
		}
	}

	bool LayeredLightPropagationVolumesBase::LightLpv::update( CpuUpdater & updater
		, Vector< float > const & lpvCellSizes )
	{
		auto & sceneObjs = updater.dirtyScenes[lightCache.getScene()];
		auto & light = *updater.light;
		auto changed = sceneObjs.dirtyLights.end() != std::find( sceneObjs.dirtyLights.begin(), sceneObjs.dirtyLights.end(), &light )
			|| light.getLpvConfig().indirectAttenuation.isDirty()
			|| light.getLpvConfig().texelAreaModifier.isDirty();

		if ( changed )
		{
			uint32_t index = 0;

			for ( auto & lpvLightConfigUbo : lpvLightConfigUbos )
			{
				lpvLightConfigUbo.cpuUpdate( light, lpvCellSizes[index], 0u );
				++index;
			}
		}

		return changed;
	}

	void LayeredLightPropagationVolumesBase::LightLpv::doCreateInjectionPass( crg::FramePassGroup & graph
		, RenderDevice const & device
		, String const & name
		, LightType lightType
		, ShadowMapResult const & smResult
		, LpvGridConfigUboArray const & lpvGridConfigUbos
		, Vector< LightVolumePassResult > & injection
		, uint32_t cascade )
	{
		auto rsmSize = smResult.getExtent().width;
		auto & pass = graph.createPass( toUtf8( name ) + "LightInjection" + string::toMbString( cascade )
			, [this, &device, lightType, rsmSize]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				auto res = makeRawUnique< LightInjectionPass >( framePass, context, runnableGraph
					, device, lightType, lightCache.getScene()->getLpvGridSize(), rsmSize );
				lightInjectionPasses.emplace_back( res.get() );
				device.renderSystem.getEngine()->registerTimer( makeString( framePass.getFullName() )
					, res->getTimer() );
				return res;
			} );
		lightCache.createPassBinding( pass, LightInjectionPass::LightsIdx );
		pass.addInputSampledImage( smResult.getSampledViewId( SmTexture::eNormal ), LightInjectionPass::RsmNormalsIdx );
		pass.addInputSampledImage( smResult.getSampledViewId( SmTexture::ePosition ), LightInjectionPass::RsmPositionIdx );
		pass.addInputSampledImage( smResult.getSampledViewId( SmTexture::eFlux ), LightInjectionPass::RsmFluxIdx );
		lpvGridConfigUbos[cascade].createPassBinding( pass, LightInjectionPass::LpvGridUboIdx );
		lpvLightConfigUbos[cascade].createPassBinding( pass, LightInjectionPass::LpvLightUboIdx );

		injection[cascade].setLastAttach( LpvTexture::eR, pass.addInOutColourTarget( *injection[cascade].getLastAttach( LpvTexture::eR ) ) );
		injection[cascade].setLastAttach( LpvTexture::eG, pass.addInOutColourTarget( *injection[cascade].getLastAttach( LpvTexture::eG ) ) );
		injection[cascade].setLastAttach( LpvTexture::eB, pass.addInOutColourTarget( *injection[cascade].getLastAttach( LpvTexture::eB ) ) );
	}

	void LayeredLightPropagationVolumesBase::LightLpv::doCreateGeometryPass( crg::FramePassGroup & graph
		, RenderDevice const & device
		, String const & name
		, LightType lightType
		, ShadowMapResult const & smResult
		, LpvGridConfigUboArray const & lpvGridConfigUbos
		, TextureArray & geometry
		, uint32_t cascade )
	{
		auto rsmSize = smResult.getExtent().width;
		auto & pass = graph.createPass( toUtf8( name ) + "GeomInjection" + string::toMbString( cascade )
			, [this, &device, lightType, rsmSize]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				auto res = makeRawUnique< GeometryInjectionPass >( framePass, context, runnableGraph
					, device, lightType, lightCache.getScene()->getLpvGridSize(), rsmSize );
				geometryInjectionPasses.emplace_back( res.get() );
				device.renderSystem.getEngine()->registerTimer( makeString( framePass.getFullName() )
					, res->getTimer() );
				return res;
			} );
		lightCache.createPassBinding( pass, GeometryInjectionPass::LightsIdx );
		pass.addInputSampledImage( smResult.getSampledViewId( SmTexture::eNormal ), GeometryInjectionPass::RsmNormalsIdx );
		pass.addInputSampledImage( smResult.getSampledViewId( SmTexture::ePosition ), GeometryInjectionPass::RsmPositionIdx );
		lpvGridConfigUbos[cascade].createPassBinding( pass, GeometryInjectionPass::LpvGridUboIdx );
		lpvLightConfigUbos[cascade].createPassBinding( pass, GeometryInjectionPass::LpvLightUboIdx );

		geometry[cascade].setLastAttach( pass.addInOutColourTarget( *geometry[cascade].getLastAttach() ) );
	}

	//*********************************************************************************************

	LayeredLightPropagationVolumesBase::LayeredLightPropagationVolumesBase( crg::ResourcesCache & resources
		, Scene const & scene
		, LightType lightType
		, RenderDevice const & device
		, ShadowMapResult const & smResult
		, LightVolumePassResultArray const & lpvResult
		, LayeredLpvGridConfigUbo & lpvGridConfigUbo
		, bool geometryVolumes )
		: Named{ cuT( "LLPV" ) + ( geometryVolumes ? String( cuT( "G" ) ) : String( cuT( "" ) ) ) }
		, m_scene{ scene }
		, m_device{ device }
		, m_sourceSmResult{ smResult }
		, m_downsampledSmResult{ resources
			, device
			, cuT( "LPV" )
			, ( ( lightType == LightType::ePoint ) ? ImageCreateFlags::eCubeCompatible : ImageCreateFlags::eNone )
			, Size{ 512u, 512u }
			, smResult.getArrayLayers() }
		, m_lpvResult{ lpvResult }
		, m_lpvGridConfigUbo{ lpvGridConfigUbo }
		, m_geometryVolumes{ geometryVolumes }
		, m_graph{ resources.getHandler(), toUtf8( getName() ) }
		, m_lightType{ lightType }
		, m_injection{ llpvpropvol::createInjection( resources, m_device, getName(), m_scene.getLpvGridSize(), LpvMaxCascadesCount ) }
		, m_geometry{ llpvpropvol::createGeometry( resources, m_device, getName(), m_scene.getLpvGridSize(), LpvMaxCascadesCount, m_geometryVolumes ) }
		, m_propagate{ llpvpropvol::createPropagation( resources, m_device, getName(), m_scene.getLpvGridSize(), LpvMaxCascadesCount ) }
		, m_lightPropagationFirstPasses{ LpvMaxCascadesCount, nullptr }
	{
		doCreateClearPass();
		doCreateDownsamplePass();

		for ( auto const & value : m_injection )
		{
			value.create();
		}

		for ( auto & value : m_geometry )
		{
			value.create();
		}

		for ( auto const & values : m_propagate )
		{
			for ( auto const & value : values )
			{
				value.create();
			}
		}

		m_graph.addInput( m_sourceSmResult.getTargetViewId( SmTexture::eNormal )
			, makeLayoutState( ImageLayout::eShaderReadOnly ) );
		m_graph.addInput( m_sourceSmResult.getTargetViewId( SmTexture::ePosition )
			, makeLayoutState( ImageLayout::eShaderReadOnly ) );
		m_graph.addInput( m_sourceSmResult.getTargetViewId( SmTexture::eFlux )
			, makeLayoutState( ImageLayout::eShaderReadOnly ) );

		for ( uint32_t cascade = 0u; cascade < LpvMaxCascadesCount; ++cascade )
		{
			m_lpvGridConfigUbos.emplace_back( m_device );
			auto const & result = *lpvResult[cascade];
			m_graph.addOutput( result.getTargetViewId( LpvTexture::eR )
				, makeLayoutState( ImageLayout::eShaderReadOnly ) );
			m_graph.addOutput( result.getTargetViewId( LpvTexture::eG )
				, makeLayoutState( ImageLayout::eShaderReadOnly ) );
			m_graph.addOutput( result.getTargetViewId( LpvTexture::eB )
				, makeLayoutState( ImageLayout::eShaderReadOnly ) );
		}
	}

	void LayeredLightPropagationVolumesBase::initialise()
	{
		if ( !m_initialised
			&& m_scene.needsGlobalIllumination( m_lightType
				, ( m_geometryVolumes
					? GlobalIlluminationType::eLayeredLpvG
					: GlobalIlluminationType::eLayeredLpv ) ) )
		{
			m_aabb = m_scene.getBoundingBox();
			doCreatePropagationPasses();
			m_runnable = m_graph.compile( m_device.makeContext() );
			m_scene.getEngine()->registerTimer( makeString( m_runnable->getName() ) + cuT( "/Graph" )
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

	void LayeredLightPropagationVolumesBase::cleanup()noexcept
	{
		if ( m_recordEvent )
			m_recordEvent->skip();
		m_initialised = false;
		m_lightPropagationPasses = {};
		m_lightLpvs.clear();
	}

	void LayeredLightPropagationVolumesBase::registerLight( LightInstance * light )
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
				, m_downsampledSmResult
				, m_lpvGridConfigUbos
				, m_injection
				, ( m_geometryVolumes ? &m_geometry : nullptr ) );

			if ( !m_lightPropagationFirstPasses.empty() )
			{
				for ( uint32_t cascade = 0u; cascade < LpvMaxCascadesCount; ++cascade )
				{
					m_lightPropagationFirstPasses[cascade]->addImplicit( *m_injection[cascade].getLastAttach( LpvTexture::eR ), ImageLayout::eShaderReadOnly );
					m_lightPropagationFirstPasses[cascade]->addImplicit( *m_injection[cascade].getLastAttach( LpvTexture::eG ), ImageLayout::eShaderReadOnly );
					m_lightPropagationFirstPasses[cascade]->addImplicit( *m_injection[cascade].getLastAttach( LpvTexture::eB ), ImageLayout::eShaderReadOnly );
				}

				m_lightPropagationFirstPasses.clear();
			}

			if ( m_runnable )
			{
				m_scene.getEngine()->unregisterTimer( makeString( m_runnable->getName() ) + cuT( "/Graph" )
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

	void LayeredLightPropagationVolumesBase::update( CpuUpdater & updater )
	{
		if ( !m_initialised
			|| !m_scene.needsGlobalIllumination( m_lightType
				, ( m_geometryVolumes
					? GlobalIlluminationType::eLayeredLpvG
					: GlobalIlluminationType::eLayeredLpv ) ) )
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
				, { m_lpvGridConfigUbo.getUbo().getData().allMinVolumeCorners[0]->w
					, m_lpvGridConfigUbo.getUbo().getData().allMinVolumeCorners[1]->w
					, m_lpvGridConfigUbo.getUbo().getData().allMinVolumeCorners[2]->w } )
				|| changed;
		}

		if ( changed )
		{
			m_aabb = aabb;
			m_cameraPos = camPos;
			m_cameraDir = camDir;
			auto cellSize = std::max( std::max( m_aabb.getDimensions()->x
				, m_aabb.getDimensions()->y )
				, m_aabb.getDimensions()->z ) / float( m_scene.getLpvGridSize() );
			Grid grid{ m_scene.getLpvGridSize(), cellSize, m_aabb.getMax(), m_aabb.getMin(), 1.0f };
			Array< float, LpvMaxCascadesCount > const scales{ 1.0f, 0.65f, 0.4f };

			for ( auto i = 0u; i < LpvMaxCascadesCount; ++i )
			{
				m_grids[i] = &m_lpvGridConfigUbos[i].cpuUpdate( scales[i]
					, grid
					, m_cameraPos
					, m_cameraDir
					, m_scene.getLpvIndirectAttenuation() );
				m_gridsSizes[i] = Point4f{ m_grids[i]->getCenter()->x
					, m_grids[i]->getCenter()->y
					, m_grids[i]->getCenter()->z
					, m_grids[i]->getCellSize() };
			}

			m_lpvGridConfigUbo.cpuUpdate( m_grids
				, m_scene.getLpvIndirectAttenuation() );
		}
	}

	SemaphoreWaitArray LayeredLightPropagationVolumesBase::render( SemaphoreWaitArray const & toWait
		, ashes::Queue const & queue )
	{
		if ( !m_initialised
			|| !m_scene.needsGlobalIllumination( m_lightType
				, ( m_geometryVolumes
					? GlobalIlluminationType::eLayeredLpvG
					: GlobalIlluminationType::eLayeredLpv ) )
			|| m_lightLpvs.empty() )
		{
			return toWait;
		}

		return m_runnable->run( toWait, queue );
	}

	void LayeredLightPropagationVolumesBase::accept( ConfigurationVisitorBase & visitor )
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
					for ( auto & geometryInjectionPass : lpv->geometryInjectionPasses )
					{
						if ( geometryInjectionPass )
							geometryInjectionPass->accept( visitor );
					}
				}
			}

			for ( auto & pass : m_lightPropagationPasses )
			{
				if ( pass )
					pass->accept( visitor );
			}

			uint32_t layer = 0u;

			for ( auto const & injection : m_injection )
			{
				for ( auto i = 0u; i < uint32_t( LpvTexture::eCount ); ++i )
				{
					auto tex = LpvTexture( i );
					visitor.visit( cuT( "Layered LPV Injection" ) + string::toString( layer ) + cuT( " " ) + getTexName( tex )
						, injection.getTexture( tex )
						, m_graph.getFinalLayoutState( injection.getWholeViewId( tex ) ).layout
						, TextureFactors::tex3D( &m_gridsSizes[i] ) );
				}

				++layer;
			}

			if ( m_geometryVolumes )
			{
				layer = 0u;

				for ( auto const & geometry : m_geometry )
				{
					visitor.visit( cuT( "Layered LPV Geometry" ) + string::toString( layer )
						, geometry
						, m_graph.getFinalLayoutState( geometry.getWholeViewId() ).layout
						, TextureFactors::tex3D( &m_gridsSizes[layer] ) );
					++layer;
				}
			}

			uint32_t level = 0u;

			for ( auto const & propagates : m_propagate )
			{
				layer = 0u;

				for ( auto const & propagate : propagates )
				{
					for ( auto i = 0u; i < uint32_t( LpvTexture::eCount ); ++i )
					{
						auto tex = LpvTexture( i );
						visitor.visit( cuT( "Layered LPV Propagation" ) + string::toString( level ) + cuT( "_" ) + string::toString( layer ) + cuT( " " ) + getTexName( tex )
							, propagate.getTexture( tex )
							, m_graph.getFinalLayoutState( propagate.getWholeViewId( tex ) ).layout
							, TextureFactors::tex3D( &m_gridsSizes[i] ) );
					}

					++layer;
				}

				++level;
			}
		}
	}

	void LayeredLightPropagationVolumesBase::doCreateClearPass()
	{
		auto & result = m_graph.createPass( "LLpvClearInjection"
			, []( crg::FramePass const & pass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				return makeRawUnique< llpvpropvol::LpvClear >( pass
					, context
					, graph );
			} );

		for ( auto const & injection : m_injection )
		{
			for ( auto & texture : injection )
				texture->setLastAttach( result.addOutputTransferImage( texture->getTargetViewId() ) );
		}

		if ( m_geometryVolumes )
		{
			for ( auto & texture : m_geometry )
				texture.setLastAttach( result.addOutputTransferImage( texture.getTargetViewId() ) );
		}
	}

	void LayeredLightPropagationVolumesBase::doCreateDownsamplePass()
	{
		auto extent = m_sourceSmResult.getExtent();

		for ( auto i = uint32_t( SmTexture::eNormal ); i < uint32_t( SmTexture::eCount ); ++i )
		{
			auto smTexture = SmTexture( i );
			auto & pass = m_graph.createPass( "LLpvDownsampleShadowMap/" + toUtf8( getTexName( smTexture ) )
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
			m_downsampledSmResult.setLastAttach( smTexture, pass.addOutputTransferImage( m_downsampledSmResult.getWholeViewId( smTexture ) ) );
		}
	}

	crg::FramePass & LayeredLightPropagationVolumesBase::doCreatePropagationPass( String const & name
		, LightVolumePassResult const & injection
		, LightVolumePassResult & lpvResult
		, LightVolumePassResult & propagation
		, uint32_t cascade
		, uint32_t index )
	{
		auto & result = m_graph.createPass( toUtf8( name ) + string::toMbString( cascade )
			, [this, index]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				auto res = makeRawUnique< LightPropagationPass >( framePass, context, runnableGraph
					, m_device, m_geometryVolumes && index > 0u, m_scene.getLpvGridSize()
					, ( index == 0u ? BlendMode::eNoBlend : BlendMode::eAdditive ) );
				m_lightPropagationPasses.emplace_back( res.get() );
				m_device.renderSystem.getEngine()->registerTimer( makeString( framePass.getFullName() )
					, res->getTimer() );
				return res;
			} );
		m_lpvGridConfigUbos[cascade].createPassBinding( result, LightPropagationPass::LpvGridUboIdx );
		result.addInputSampled( *injection.getSampledLastAttach( LpvTexture::eR ), LightPropagationPass::RLpvGridIdx
			, crg::SamplerDesc{ FilterMode::eLinear, FilterMode::eLinear, MipmapMode::eLinear } );
		result.addInputSampled( *injection.getSampledLastAttach( LpvTexture::eG ), LightPropagationPass::GLpvGridIdx
			, crg::SamplerDesc{ FilterMode::eLinear, FilterMode::eLinear, MipmapMode::eLinear } );
		result.addInputSampled( *injection.getSampledLastAttach( LpvTexture::eB ), LightPropagationPass::BLpvGridIdx
			, crg::SamplerDesc{ FilterMode::eLinear, FilterMode::eLinear, MipmapMode::eLinear } );

		if ( index > 0u && m_geometryVolumes )
		{
			result.addInputSampled( *m_geometry[cascade].getSampledLastAttach(), LightPropagationPass::GpGridIdx
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

	void LayeredLightPropagationVolumesBase::doCreatePropagationPasses()
	{
		for ( uint32_t cascade = 0u; cascade < LpvMaxCascadesCount; ++cascade )
		{
			auto & propagate = m_propagate[cascade];
			auto const & injection = m_injection[cascade];
			auto & lpvResult = *m_lpvResult[cascade];
			uint32_t propIndex = 0u;
			auto const * input = &injection;
			auto * output = &propagate[propIndex];
			m_lightPropagationFirstPasses[cascade] = &doCreatePropagationPass( cuT( "Propagation" ) + string::toString( cascade ) + cuT( "NoOccNoBlend" )
				, *input
				, lpvResult
				, *output
				, cascade
				, 0u );

			for ( uint32_t i = 1u; i < LpvMaxPropagationSteps; ++i )
			{
				input = &propagate[propIndex];
				propIndex = 1u - propIndex;
				output = &propagate[propIndex];
				String name = ( m_geometryVolumes
					? String{ cuT( "OccBlend" ) }
					: String{ cuT( "NoOccBlend" ) } );
				doCreatePropagationPass( cuT( "Propagation" ) + string::toString( cascade ) + name + string::toString( i )
					, *input
					, lpvResult
					, *output
					, cascade
					, i );
			}
		}
	}
}
