#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LayeredLightPropagationVolumes.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"
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

namespace castor3d
{
	//*********************************************************************************************

	namespace
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
					, { [](){}
						, GetSemaphoreWaitFlagsCallback( [this](){ return VK_PIPELINE_STAGE_TRANSFER_BIT; } )
						, [this]( VkCommandBuffer cb, uint32_t i ){ doRecordInto( cb, i ); } } }
			{
			}

		private:
			void doRecordInto( VkCommandBuffer commandBuffer
				, uint32_t index )
			{
				auto clearValue = transparentBlackClearColor.color;

				for ( auto & attach : m_pass.images )
				{
					auto view = attach.view();
					auto image = m_graph.createImage( view.data->image );
					assert( attach.isTransferOutputView() );
					m_context.vkCmdClearColorImage( commandBuffer
						, image
						, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
						, &clearValue
						, 1u
						, &view.data->info.subresourceRange );
				}
			}
		};

		std::vector< LightVolumePassResult > createInjection( crg::ResourceHandler & handler
			, RenderDevice const & device
			, castor::String const & name
			, uint32_t lpvGridSize
			, uint32_t cascadeCount )
		{
			std::vector< LightVolumePassResult > result;

			for ( uint32_t cascade = 0u; cascade < cascadeCount; ++cascade )
			{
				result.emplace_back( handler
					, device
					, name + "Injection" + castor::string::toString( cascade )
					, lpvGridSize );
			}

			return result;
		}

		TextureArray createGeometry( crg::ResourceHandler & handler
			, RenderDevice const & device
			, castor::String const & name
			, uint32_t lpvGridSize
			, uint32_t cascadeCount
			, bool enabled )
		{
			TextureArray result;

			for ( uint32_t cascade = 0u; cascade < cascadeCount; ++cascade )
			{
				result.push_back( enabled
					? GeometryInjectionPass::createResult( handler
						, device
						, name
						, cascade
						, lpvGridSize )
					: Texture{} );
			}

			return result;
		}

		std::vector< std::array< LightVolumePassResult, 2u > > createPropagation( crg::ResourceHandler & handler
			, RenderDevice const & device
			, castor::String const & name
			, uint32_t lpvGridSize
			, uint32_t cascadeCount )
		{
			std::vector< std::array< LightVolumePassResult, 2u > > result;

			for ( uint32_t cascade = 0u; cascade < cascadeCount; ++cascade )
			{
				result.push_back( { LightVolumePassResult{ handler, device, name + "Propagate" + std::to_string( cascade ) + "0", lpvGridSize }
					, LightVolumePassResult{ handler, device, name + "Propagate" + std::to_string( cascade ) + "1", lpvGridSize } } );
			}

			return result;
		}
	}

	//*********************************************************************************************

	LayeredLightPropagationVolumesBase::LightLpv::LightLpv( crg::FrameGraph & graph
		, crg::FramePass const & previousPass
		, RenderDevice const & device
		, castor::String const & name
		, LightCache const & lightCache
		, LightType lightType
		, ShadowMapResult const & smResult
		, LpvGridConfigUboArray const & lpvGridConfigUbos
		, std::vector< LightVolumePassResult > const & injection
		, TextureArray const * geometry )
		: lastLightPass{ &previousPass }
		, lastGeomPass{ &previousPass }
	{
		auto & engine = *device.renderSystem.getEngine();

		for ( uint32_t cascade = 0u; cascade < CascadeCount; ++cascade )
		{
			lpvLightConfigUbos.emplace_back( device );
			lightInjectionPassesDesc.push_back( &doCreateInjectionPass( graph
				, device
				, name
				, lightCache
				, lightType
				, smResult
				, lpvGridConfigUbos
				, injection
				, cascade ) );

			if ( geometry )
			{
				geometryInjectionPassesDesc.push_back( &doCreateGeometryPass( graph
					, device
					, name
					, lightCache
					, lightType
					, smResult
					, lpvGridConfigUbos
					, *geometry
					, cascade ) );
			}
		}
	}

	bool LayeredLightPropagationVolumesBase::LightLpv::update( CpuUpdater & updater
		, std::vector< float > const & lpvCellSizes )
	{
		auto & light = *updater.light;
		auto changed = light.hasChanged()
			|| light.getLpvConfig().indirectAttenuation.isDirty()
			|| light.getLpvConfig().texelAreaModifier.isDirty();

		if ( changed )
		{
			uint32_t index = 0;

			for ( auto & lpvLightConfigUbo : lpvLightConfigUbos )
			{
				lpvLightConfigUbo.cpuUpdate( light, lpvCellSizes[index++], 0u );
			}
		}

		return changed;
	}

	crg::FramePass & LayeredLightPropagationVolumesBase::LightLpv::doCreateInjectionPass( crg::FrameGraph & graph
		, RenderDevice const & device
		, castor::String const & name
		, LightCache const & lightCache
		, LightType lightType
		, ShadowMapResult const & smResult
		, LpvGridConfigUboArray const & lpvGridConfigUbos
		, std::vector< LightVolumePassResult > const & injection
		, uint32_t cascade )
	{
		auto rsmSize = smResult[SmTexture::eDepth].getExtent().width;
		auto & result = graph.createPass( name + "LightInjection" + std::to_string( cascade )
			, [this, &device, name, lightType, rsmSize]( crg::FramePass const & pass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = std::make_unique< LightInjectionPass >( pass
					, context
					, graph
					, device
					, lightType
					, device.renderSystem.getEngine()->getLpvGridSize()
					, rsmSize );
				lightInjectionPasses.push_back( result.get() );
				device.renderSystem.getEngine()->registerTimer( "LLPV"
					, result->getTimer() );
				return result;
			} );
		result.addDependency( *lastLightPass );
		result.addUniformBufferView( { lightCache.getBuffer().getBuffer(), "LightCache" }
			, lightCache.getView()
			, LightInjectionPass::LightsIdx
			, lightCache.getView().getOffset()
			, lightCache.getView().getRange() );
		result.addSampledView( smResult[SmTexture::eNormalLinear].sampledViewId
			, LightInjectionPass::RsmNormalsIdx
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		result.addSampledView( smResult[SmTexture::ePosition].sampledViewId
			, LightInjectionPass::RsmPositionIdx
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		result.addSampledView( smResult[SmTexture::eFlux].sampledViewId
			, LightInjectionPass::RsmFluxIdx
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		lpvGridConfigUbos[cascade].createPassBinding( result
			, LightInjectionPass::LpvGridUboIdx );
		lpvLightConfigUbos[cascade].createPassBinding( result
			, LightInjectionPass::LpvLightUboIdx );

		result.addInOutColourView( injection[cascade][LpvTexture::eR].targetViewId );
		result.addInOutColourView( injection[cascade][LpvTexture::eG].targetViewId );
		result.addInOutColourView( injection[cascade][LpvTexture::eB].targetViewId );
		lastLightPass = &result;
		return result;
	}

	crg::FramePass & LayeredLightPropagationVolumesBase::LightLpv::doCreateGeometryPass( crg::FrameGraph & graph
		, RenderDevice const & device
		, castor::String const & name
		, LightCache const & lightCache
		, LightType lightType
		, ShadowMapResult const & smResult
		, LpvGridConfigUboArray const & lpvGridConfigUbos
		, TextureArray const & geometry
		, uint32_t cascade )
	{
		auto rsmSize = smResult[SmTexture::eDepth].getExtent().width;
		auto & result = graph.createPass( name + "GeomInjection" + std::to_string( cascade )
			, [this, &device, name, lightType, rsmSize, cascade]( crg::FramePass const & pass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = std::make_unique< GeometryInjectionPass >( pass
					, context
					, graph
					, device
					, lightType
					, device.renderSystem.getEngine()->getLpvGridSize()
					, rsmSize );
				geometryInjectionPasses.push_back( result.get() );
				device.renderSystem.getEngine()->registerTimer( "LLPV"
					, result->getTimer() );
				return result;
			} );
		result.addDependency( *lastGeomPass );
		result.addUniformBufferView( { lightCache.getBuffer().getBuffer(), "LightCache" }
			, lightCache.getView()
			, GeometryInjectionPass::LightsIdx
			, lightCache.getView().getOffset()
			, lightCache.getView().getRange() );
		result.addSampledView( smResult[SmTexture::eNormalLinear].sampledViewId
			, GeometryInjectionPass::RsmNormalsIdx
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		result.addSampledView( smResult[SmTexture::ePosition].sampledViewId
			, GeometryInjectionPass::RsmPositionIdx
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		lpvGridConfigUbos[cascade].createPassBinding( result
			, GeometryInjectionPass::LpvGridUboIdx );
		lpvLightConfigUbos[cascade].createPassBinding( result
			, GeometryInjectionPass::LpvLightUboIdx );

		result.addInOutColourView( geometry[cascade].targetViewId );
		lastGeomPass = &result;
		return result;
	}

	//*********************************************************************************************

	LayeredLightPropagationVolumesBase::LayeredLightPropagationVolumesBase( crg::ResourceHandler & handler
		, Scene const & scene
		, LightType lightType
		, RenderDevice const & device
		, ShadowMapResult const & smResult
		, LightVolumePassResultArray const & lpvResult
		, LayeredLpvGridConfigUbo & lpvGridConfigUbo
		, bool geometryVolumes )
		: castor::Named{ "LLPV" + ( geometryVolumes ? castor::String( "G" ) : castor::String( "" ) ) }
		, m_scene{ scene }
		, m_device{ device }
		, m_smResult{ smResult }
		, m_lpvResult{ lpvResult }
		, m_lightType{ lightType }
		, m_lpvGridConfigUbo{ lpvGridConfigUbo }
		, m_geometryVolumes{ geometryVolumes }
		, m_graph{ handler, getName() }
		, m_injection{ createInjection( handler, m_device, getName(), m_scene.getLpvGridSize(), CascadeCount ) }
		, m_geometry{ createGeometry( handler, m_device, getName(), m_scene.getLpvGridSize(), CascadeCount, m_geometryVolumes ) }
		, m_propagate{ createPropagation( handler, m_device, getName(), m_scene.getLpvGridSize(), CascadeCount ) }
		, m_clearInjectionPass{ doCreateClearInjectionPass() }
	{
		for ( uint32_t cascade = 0u; cascade < CascadeCount; ++cascade )
		{
			m_lpvGridConfigUbos.emplace_back( m_device );
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
			m_lightPropagationPassesDesc = doCreatePropagationPasses();
			m_runnable = m_graph.compile( m_device.makeContext() );
			m_runnable->record();
			m_initialised = true;
		}
	}

	void LayeredLightPropagationVolumesBase::cleanup()
	{
		m_initialised = false;
		m_lightPropagationPasses = {};
		m_lightLpvs.clear();
	}

	void LayeredLightPropagationVolumesBase::registerLight( Light * light )
	{
		auto it = m_lightLpvs.find( light );

		if ( it == m_lightLpvs.end() )
		{
			auto ires = m_lightLpvs.emplace( light
				, LightLpv{ m_graph
					, m_clearInjectionPass
					, m_device
					, this->getName() + light->getName()
					, light->getScene()->getLightCache()
					, m_lightType
					, m_smResult
					, m_lpvGridConfigUbos
					, m_injection
					, ( m_geometryVolumes
						? &m_geometry
						: nullptr ) } );

			for ( uint32_t cascade = 0u; cascade < CascadeCount; ++cascade )
			{
				auto index = cascade * MaxPropagationSteps;
				m_lightPropagationPassesDesc[index]->addDependency( *ires.first->second.lightInjectionPassesDesc[cascade] );

				if ( m_geometryVolumes )
				{
					m_lightPropagationPassesDesc[index + 1u]->addDependency( *ires.first->second.geometryInjectionPassesDesc[cascade] );
				}
			}

			if ( m_runnable )
			{
				m_runnable.reset();
				m_runnable = m_graph.compile( m_device.makeContext() );
				m_runnable->record();
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

		auto & camera = *updater.camera;
		auto aabb = m_scene.getBoundingBox();
		auto camPos = camera.getParent()->getDerivedPosition();
		castor::Point3f camDir{ 0, 0, 1 };
		camera.getParent()->getDerivedOrientation().transform( camDir, camDir );
		auto changed = m_aabb != aabb
			|| m_cameraPos != camPos
			|| m_cameraDir != camDir;

		for ( auto & lightLpv : m_lightLpvs )
		{
			updater.light = lightLpv.first;
			changed = lightLpv.second.update( updater
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
				, m_aabb.getDimensions()->z ) / m_scene.getLpvGridSize();
			castor::Grid grid{ m_scene.getLpvGridSize(), cellSize, m_aabb.getMax(), m_aabb.getMin(), 1.0f, 0 };
			std::array< float, CascadeCount > const scales{ 1.0f, 0.65f, 0.4f };

			for ( auto i = 0u; i < CascadeCount; ++i )
			{
				m_grids[i] = &m_lpvGridConfigUbos[i].cpuUpdate( i
					, scales[i]
					, grid
					, m_aabb
					, m_cameraPos
					, m_cameraDir
					, m_scene.getLpvIndirectAttenuation() );
				m_gridsSizes[i] = castor::Point4f{ m_grids[i]->getCenter()->x
					, m_grids[i]->getCenter()->y
					, m_grids[i]->getCenter()->z
					, m_grids[i]->getCellSize() };
			}

			m_lpvGridConfigUbo.cpuUpdate( m_grids
				, m_scene.getLpvIndirectAttenuation() );
		}
	}

	crg::SemaphoreWait LayeredLightPropagationVolumesBase::render( crg::SemaphoreWait const & toWait
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

	void LayeredLightPropagationVolumesBase::accept( PipelineVisitorBase & visitor )
	{
		if ( m_initialised )
		{
			for ( auto & lightLpv : m_lightLpvs )
			{
				for ( auto & lightInjectionPass : lightLpv.second.lightInjectionPasses )
				{
					if ( lightInjectionPass )
					{
						lightInjectionPass->accept( visitor );
					}
				}

				if ( m_geometryVolumes )
				{
					for ( auto & geometryInjectionPass : lightLpv.second.geometryInjectionPasses )
					{
						if ( geometryInjectionPass )
						{
							geometryInjectionPass->accept( visitor );
						}
					}
				}
			}

			for ( auto & pass : m_lightPropagationPasses )
			{
				if ( pass )
				{
					pass->accept( visitor );
				}
			}

			uint32_t layer = 0u;

			for ( auto & injection : m_injection )
			{
				for ( auto i = 0u; i < uint32_t( LpvTexture::eCount ); ++i )
				{
					auto tex = LpvTexture( i );
					visitor.visit( "Layered LPV Injection" + std::to_string( layer ) + " " + castor3d::getName( tex )
						, injection[tex]
						, m_graph.getFinalLayout( injection[tex].wholeViewId ).layout
						, TextureFactors::tex3D( &m_gridsSizes[i] ) );
				}

				++layer;
			}

			layer = 0u;

			for ( auto & geometry : m_geometry )
			{
				visitor.visit( "Layered LPV Geometry" + std::to_string( layer )
					, geometry
					, m_graph.getFinalLayout( geometry.wholeViewId ).layout
					, TextureFactors::tex3D( &m_gridsSizes[layer] ) );
				++layer;
			}

			uint32_t level = 0u;

			for ( auto & propagates : m_propagate )
			{
				layer = 0u;

				for ( auto & propagate : propagates )
				{
					for ( auto i = 0u; i < uint32_t( LpvTexture::eCount ); ++i )
					{
						auto tex = LpvTexture( i );
						visitor.visit( "Layered LPV Propagation" + std::to_string( level ) + "_" + std::to_string( layer ) + " " + castor3d::getName( tex )
							, propagate[tex]
							, m_graph.getFinalLayout( propagate[tex].wholeViewId ).layout
							, TextureFactors::tex3D( &m_gridsSizes[i] ) );
					}

					++layer;
				}

				++level;
			}
		}
	}

	crg::FramePass & LayeredLightPropagationVolumesBase::doCreateClearInjectionPass()
	{
		auto & result = m_graph.createPass( "LLpvClearInjection"
			, [this]( crg::FramePass const & pass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				return std::make_unique< LpvClear >( pass
					, context
					, graph );
			} );

		for ( auto & injection : m_injection )
		{
			for ( auto & texture : injection )
			{
				result.addTransferOutputView( texture.wholeViewId
					, VK_IMAGE_LAYOUT_UNDEFINED );
			}
		}

		if ( m_geometryVolumes )
		{
			for ( auto & texture : m_geometry )
			{
				result.addTransferOutputView( texture.wholeViewId
					, VK_IMAGE_LAYOUT_UNDEFINED );
			}
		}

		return result;
	}

	crg::FramePass & LayeredLightPropagationVolumesBase::doCreatePropagationPass( std::vector< crg::FramePass const * > previousPasses
		, std::string const & name
		, LightVolumePassResult const & injection
		, LightVolumePassResult const & lpvResult
		, LightVolumePassResult const & propagation
		, uint32_t cascade
		, uint32_t index )
	{
		auto & result = m_graph.createPass( getName() + name + std::to_string( cascade )
			, [this, index]( crg::FramePass const & pass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = std::make_unique< LightPropagationPass >( pass
					, context
					, graph
					, m_device
					, m_geometryVolumes && index > 0u
					, m_scene.getLpvGridSize()
					, ( index == 0u ? BlendMode::eNoBlend : BlendMode::eAdditive ) );
				m_lightPropagationPasses.push_back( result.get() );
				m_device.renderSystem.getEngine()->registerTimer( "LLPV"
					, result->getTimer() );
				return result;
			} );

		for ( auto & previousPass : previousPasses )
		{
			result.addDependency( *previousPass );
		}

		m_lpvGridConfigUbos[cascade].createPassBinding( result
			, LightPropagationPass::LpvGridUboIdx );
		result.addSampledView( injection[LpvTexture::eR].sampledViewId
			, LightPropagationPass::RLpvGridIdx
			, VK_IMAGE_LAYOUT_UNDEFINED );
		result.addSampledView( injection[LpvTexture::eG].sampledViewId
			, LightPropagationPass::GLpvGridIdx
			, VK_IMAGE_LAYOUT_UNDEFINED );
		result.addSampledView( injection[LpvTexture::eB].sampledViewId
			, LightPropagationPass::BLpvGridIdx
			, VK_IMAGE_LAYOUT_UNDEFINED );

		if ( index > 0u && m_geometryVolumes )
		{
			result.addSampledView( m_geometry[cascade].sampledViewId
				, LightPropagationPass::GpGridIdx
				, VK_IMAGE_LAYOUT_UNDEFINED );
		}

		if ( index == 0u )
		{
			result.addOutputColourView( lpvResult[LpvTexture::eR].targetViewId );
			result.addOutputColourView( lpvResult[LpvTexture::eG].targetViewId );
			result.addOutputColourView( lpvResult[LpvTexture::eB].targetViewId );
		}
		else if ( index == MaxPropagationSteps - 1u )
		{
			result.addInOutColourView( lpvResult[LpvTexture::eR].targetViewId
				, {}
				, VK_IMAGE_LAYOUT_UNDEFINED
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
			result.addInOutColourView( lpvResult[LpvTexture::eG].targetViewId
				, {}
				, VK_IMAGE_LAYOUT_UNDEFINED
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
			result.addInOutColourView( lpvResult[LpvTexture::eB].targetViewId
				, {}
				, VK_IMAGE_LAYOUT_UNDEFINED
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		}
		else
		{
			result.addInOutColourView( lpvResult[LpvTexture::eR].targetViewId );
			result.addInOutColourView( lpvResult[LpvTexture::eG].targetViewId );
			result.addInOutColourView( lpvResult[LpvTexture::eB].targetViewId );
		}

		if ( index <= 1u )
		{
			result.addOutputColourView( propagation[LpvTexture::eR].targetViewId );
			result.addOutputColourView( propagation[LpvTexture::eG].targetViewId );
			result.addOutputColourView( propagation[LpvTexture::eB].targetViewId );
		}
		else
		{
			result.addInOutColourView( propagation[LpvTexture::eR].targetViewId );
			result.addInOutColourView( propagation[LpvTexture::eG].targetViewId );
			result.addInOutColourView( propagation[LpvTexture::eB].targetViewId );
		}

		return result;
	}

	std::vector< crg::FramePass * > LayeredLightPropagationVolumesBase::doCreatePropagationPasses()
	{
		std::vector< crg::FramePass * > result;
		std::vector< crg::FramePass const * > previousPasses;

		for ( uint32_t cascade = 0u; cascade < CascadeCount; ++cascade )
		{
			auto & propagate = m_propagate[cascade];
			auto & lpvResult = *m_lpvResult[cascade];
			auto & injection = m_injection[cascade];
			uint32_t propIndex = 0u;
			result.push_back( &doCreatePropagationPass( previousPasses
				, "Propagation" + std::to_string( cascade ) + "NoOccNoBlend"
				, injection
				, lpvResult
				, propagate[propIndex]
				, cascade
				, 0u ) );
			auto previous = result.back();
			previousPasses.clear();

			if ( m_geometryVolumes )
			{
				for ( auto & injectionPasses : m_lightLpvs )
				{
					previousPasses.push_back( injectionPasses.second.geometryInjectionPassesDesc[cascade] );
				}
			}

			for ( uint32_t i = 1u; i < MaxPropagationSteps; ++i )
			{
				auto & input = propagate[propIndex];
				propIndex = 1u - propIndex;
				auto & output = propagate[propIndex];
				std::string name = ( m_geometryVolumes
					? castor::String{ cuT( "OccBlend" ) }
					: castor::String{ cuT( "NoOccBlend" ) } );
				previousPasses.push_back( previous );
				result.push_back( &doCreatePropagationPass( previousPasses
					, "Propagation" + std::to_string( cascade ) + name + std::to_string( i )
					, input
					, lpvResult
					, output
					, cascade
					, i ) );
				previousPasses.clear();
				previous = result.back();
			}

			previousPasses.push_back( result.back() );
		}

		return result;
	}
}