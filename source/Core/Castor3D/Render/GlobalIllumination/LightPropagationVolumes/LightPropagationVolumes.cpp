#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightPropagationVolumes.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
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
#include "Castor3D/Shader/Ubos/LpvGridConfigUbo.hpp"

#include <CastorUtils/Miscellaneous/StringUtils.hpp>

#include <ashespp/Buffer/Buffer.hpp>
#include <ashespp/Buffer/BufferView.hpp>

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

		protected:
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

		crg::ImageViewId createArrayView( crg::FrameGraph & graph
			, crg::ImageViewId cubeArrayView )
		{
			auto data = *cubeArrayView.data;
			data.info.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
			return graph.createView( data );
		}

		std::vector< LpvLightConfigUbo > createUbos( RenderDevice const & device
			, LightType lightType )
		{
			std::vector< LpvLightConfigUbo > result;

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

	LightPropagationVolumesBase::LightLpv::LightLpv( crg::FrameGraph & graph
		, crg::FramePass const & previousPass
		, RenderDevice const & device
		, castor::String const & name
		, LightCache const & lightCache
		, LightType lightType
		, ShadowMapResult const & smResult
		, LpvGridConfigUbo const & lpvGridConfigUbo
		, LightVolumePassResult const & injection
		, Texture * geometry )
		: lpvLightConfigUbos{ createUbos( device, lightType ) }
		, lastPass{ &previousPass }
		, lightInjectionPassDescs{ doCreateInjectionPasses( graph
			, device
			, name
			, lightCache
			, lightType
			, smResult
			, lpvGridConfigUbo
			, injection ) }
		, geometryInjectionPassDescs{ ( geometry
			? doCreateGeometryPasses( graph
				, device
				, name
				, lightCache
				, lightType
				, smResult
				, lpvGridConfigUbo
				, *geometry )
			: crg::FramePassArray{} ) }
	{
	}

	bool LightPropagationVolumesBase::LightLpv::update( CpuUpdater & updater
		, float lpvCellSize )
	{
		auto & light = *updater.light;
		auto changed = light.hasChanged()
			|| light.getLpvConfig().indirectAttenuation.isDirty()
			|| light.getLpvConfig().texelAreaModifier.isDirty();

		if ( changed )
		{
			uint32_t index = 0u;

			for ( auto & lpvLightConfigUbo : lpvLightConfigUbos )
			{
				lpvLightConfigUbo.cpuUpdate( light, lpvCellSize, index++ );
			}
		}

		return changed;
	}

	crg::FramePass const & LightPropagationVolumesBase::LightLpv::doCreateInjectionPass( crg::FrameGraph & graph
		, RenderDevice const & device
		, castor::String const & name
		, LightCache const & lightCache
		, LightType lightType
		, ShadowMapResult const & smResult
		, LpvGridConfigUbo const & lpvGridConfigUbo
		, LightVolumePassResult const & injection )
	{
		auto rsmSize = smResult[SmTexture::eDepth].getExtent().width;
		auto & pass = graph.createPass( name + "LightInjection"
			, [this, &device, lightType, rsmSize]( crg::FramePass const & pass
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
				device.renderSystem.getEngine()->registerTimer( graph.getName() + "/LPV"
					, result->getTimer() );
				return result;
			} );
		pass.addDependency( *lastPass );
		pass.addUniformBufferView( { lightCache.getBuffer().getBuffer(), "LightCache" }
			, lightCache.getView()
			, LightInjectionPass::LightsIdx
			, lightCache.getView().getOffset()
			, lightCache.getView().getRange() );
		pass.addSampledView( smResult[SmTexture::eNormalLinear].sampledViewId
			, LightInjectionPass::RsmNormalsIdx
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		pass.addSampledView( smResult[SmTexture::ePosition].sampledViewId
			, LightInjectionPass::RsmPositionIdx
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		pass.addSampledView( smResult[SmTexture::eFlux].sampledViewId
			, LightInjectionPass::RsmFluxIdx
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		lpvGridConfigUbo.createPassBinding( pass
			, LightInjectionPass::LpvGridUboIdx );
		lpvLightConfigUbos[0].createPassBinding( pass
			, LightInjectionPass::LpvLightUboIdx );

		pass.addInOutColourView( injection[LpvTexture::eR].targetViewId );
		pass.addInOutColourView( injection[LpvTexture::eG].targetViewId );
		pass.addInOutColourView( injection[LpvTexture::eB].targetViewId );

		return pass;
	}

	crg::FramePass const & LightPropagationVolumesBase::LightLpv::doCreateInjectionPass( crg::FrameGraph & graph
		, RenderDevice const & device
		, castor::String const & name
		, LightCache const & lightCache
		, std::vector< crg::ImageViewId > const & arrayViews
		, CubeMapFace face
		, ShadowMapResult const & smResult
		, LpvGridConfigUbo const & lpvGridConfigUbo
		, LightVolumePassResult const & injection )
	{
		auto rsmSize = smResult[SmTexture::eDepth].getExtent().width;
		auto & pass = graph.createPass( name + castor3d::getName( face ) + "LightInjection"
			, [this, &device, face, rsmSize]( crg::FramePass const & pass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = std::make_unique< LightInjectionPass >( pass
					, context
					, graph
					, device
					, face
					, device.renderSystem.getEngine()->getLpvGridSize()
					, rsmSize );
				lightInjectionPasses.push_back( result.get() );
				device.renderSystem.getEngine()->registerTimer( graph.getName() + "/LPV"
					, result->getTimer() );
				return result;
			} );
		pass.addDependency( *lastPass );
		pass.addUniformBufferView( { lightCache.getBuffer().getBuffer(), "LightCache" }
			, lightCache.getView()
			, LightInjectionPass::LightsIdx
			, lightCache.getView().getOffset()
			, lightCache.getView().getRange() );
		pass.addSampledView( arrayViews[0u]
			, LightInjectionPass::RsmNormalsIdx
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		pass.addSampledView( arrayViews[1u]
			, LightInjectionPass::RsmPositionIdx
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		pass.addSampledView( arrayViews[2u]
			, LightInjectionPass::RsmFluxIdx
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		lpvGridConfigUbo.createPassBinding( pass
			, LightInjectionPass::LpvGridUboIdx );
		lpvLightConfigUbos[uint32_t( face )].createPassBinding( pass
			, LightInjectionPass::LpvLightUboIdx );
		pass.addInOutColourView( injection[LpvTexture::eR].targetViewId );
		pass.addInOutColourView( injection[LpvTexture::eG].targetViewId );
		pass.addInOutColourView( injection[LpvTexture::eB].targetViewId );

		return pass;
	}

	crg::FramePassArray LightPropagationVolumesBase::LightLpv::doCreateInjectionPasses( crg::FrameGraph & graph
		, RenderDevice const & device
		, castor::String const & name
		, LightCache const & lightCache
		, LightType lightType
		, ShadowMapResult const & smResult
		, LpvGridConfigUbo const & lpvGridConfigUbo
		, LightVolumePassResult const & injection )
	{
		crg::FramePassArray result;

		if ( lightType != LightType::ePoint )
		{
			result.push_back( &doCreateInjectionPass( graph
				, device
				, name
				, lightCache
				, lightType
				, smResult
				, lpvGridConfigUbo
				, injection ) );
			lastPass = result.back();
		}
		else
		{
			std::vector< crg::ImageViewId > arrayViews;
			arrayViews.push_back( createArrayView( graph, smResult[SmTexture::eNormalLinear].sampledViewId ) );
			arrayViews.push_back( createArrayView( graph, smResult[SmTexture::ePosition].sampledViewId ) );
			arrayViews.push_back( createArrayView( graph, smResult[SmTexture::eFlux].sampledViewId ) );

			for ( uint32_t faceIndex = 0u; faceIndex < 6u; ++faceIndex )
			{
				result.push_back( &doCreateInjectionPass( graph
					, device
					, name
					, lightCache
					, arrayViews
					, CubeMapFace( faceIndex )
					, smResult
					, lpvGridConfigUbo
					, injection ) );
				lastPass = result.back();
			}
		}

		return result;
	}

	crg::FramePass const & LightPropagationVolumesBase::LightLpv::doCreateGeometryPass( crg::FrameGraph & graph
		, RenderDevice const & device
		, castor::String const & name
		, LightCache const & lightCache
		, LightType lightType
		, ShadowMapResult const & smResult
		, LpvGridConfigUbo const & lpvGridConfigUbo
		, Texture & geometry )
	{
		auto rsmSize = smResult[SmTexture::eDepth].getExtent().width;
		auto & pass = graph.createPass( name + "GeomInjection"
			, [this, &device, lightType, rsmSize]( crg::FramePass const & pass
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
				device.renderSystem.getEngine()->registerTimer( graph.getName() + "/LPV"
					, result->getTimer() );
				return result;
			} );
		pass.addDependency( *lastPass );
		pass.addUniformBufferView( { lightCache.getBuffer().getBuffer(), "LightCache" }
			, lightCache.getView()
			, GeometryInjectionPass::LightsIdx
			, lightCache.getView().getOffset()
			, lightCache.getView().getRange() );
		pass.addSampledView( smResult[SmTexture::eNormalLinear].sampledViewId
			, GeometryInjectionPass::RsmNormalsIdx
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		pass.addSampledView( smResult[SmTexture::ePosition].sampledViewId
			, GeometryInjectionPass::RsmPositionIdx
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		lpvGridConfigUbo.createPassBinding( pass
			, GeometryInjectionPass::LpvGridUboIdx );
		lpvLightConfigUbos[0].createPassBinding( pass
			, GeometryInjectionPass::LpvLightUboIdx );

		pass.addInOutColourView( geometry.targetViewId );

		return pass;
	}

	crg::FramePass const & LightPropagationVolumesBase::LightLpv::doCreateGeometryPass( crg::FrameGraph & graph
		, RenderDevice const & device
		, castor::String const & name
		, LightCache const & lightCache
		, std::vector< crg::ImageViewId > const & arrayViews
		, CubeMapFace face
		, ShadowMapResult const & smResult
		, LpvGridConfigUbo const & lpvGridConfigUbo
		, Texture & geometry )
	{
		auto rsmSize = smResult[SmTexture::eDepth].getExtent().width;
		auto & pass = graph.createPass( name + castor3d::getName( face ) + "GeomInjection"
			, [this, &device, face, rsmSize]( crg::FramePass const & pass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = std::make_unique< GeometryInjectionPass >( pass
					, context
					, graph
					, device
					, face
					, device.renderSystem.getEngine()->getLpvGridSize()
					, rsmSize );
				geometryInjectionPasses.push_back( result.get() );
				device.renderSystem.getEngine()->registerTimer( graph.getName() + "/LPV"
					, result->getTimer() );
				return result;
			} );
		pass.addDependency( *lastPass );
		pass.addUniformBufferView( { lightCache.getBuffer().getBuffer(), "LightCache" }
			, lightCache.getView()
			, GeometryInjectionPass::LightsIdx
			, lightCache.getView().getOffset()
			, lightCache.getView().getRange() );
		pass.addSampledView( arrayViews[0u]
			, LightInjectionPass::RsmNormalsIdx
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		pass.addSampledView( arrayViews[1u]
			, LightInjectionPass::RsmPositionIdx
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		lpvGridConfigUbo.createPassBinding( pass
			, GeometryInjectionPass::LpvGridUboIdx );
		lpvLightConfigUbos[uint32_t( face )].createPassBinding( pass
			, GeometryInjectionPass::LpvLightUboIdx );

		pass.addInOutColourView( geometry.targetViewId );

		return pass;
	}

	crg::FramePassArray LightPropagationVolumesBase::LightLpv::doCreateGeometryPasses( crg::FrameGraph & graph
		, RenderDevice const & device
		, castor::String const & name
		, LightCache const & lightCache
		, LightType lightType
		, ShadowMapResult const & smResult
		, LpvGridConfigUbo const & lpvGridConfigUbo
		, Texture & geometry )
	{
		crg::FramePassArray result;

		if ( lightType != LightType::ePoint )
		{
			result.push_back( &doCreateGeometryPass( graph
				, device
				, name
				, lightCache
				, lightType
				, smResult
				, lpvGridConfigUbo
				, geometry ) );
			lastPass = result.back();
		}
		else
		{
			std::vector< crg::ImageViewId > arrayViews;
			arrayViews.push_back( createArrayView( graph, smResult[SmTexture::eNormalLinear].sampledViewId ) );
			arrayViews.push_back( createArrayView( graph, smResult[SmTexture::ePosition].sampledViewId ) );

			for ( uint32_t faceIndex = 0u; faceIndex < 6u; ++faceIndex )
			{
				result.push_back( &doCreateGeometryPass( graph
					, device
					, name
					, lightCache
					, arrayViews
					, CubeMapFace( faceIndex )
					, smResult
					, lpvGridConfigUbo
					, geometry ) );
				lastPass = result.back();
			}
		}

		return result;
	}

	//*********************************************************************************************

	LightPropagationVolumesBase::LightPropagationVolumesBase( crg::ResourceHandler & handler
		, Scene const & scene
		, LightType lightType
		, RenderDevice const & device
		, ShadowMapResult const & smResult
		, LightVolumePassResult const & lpvResult
		, LpvGridConfigUbo & lpvGridConfigUbo
		, bool geometryVolumes )
		: castor::Named{ "LPV" + ( geometryVolumes ? castor::String( "G" ) : castor::String( "" ) ) }
		, m_scene{ scene }
		, m_device{ device }
		, m_smResult{ smResult }
		, m_lpvResult{ lpvResult }
		, m_lightType{ lightType }
		, m_lpvGridConfigUbo{ lpvGridConfigUbo }
		, m_geometryVolumes{ geometryVolumes }
		, m_graph{ handler, getName() }
		, m_injection{ handler
			, m_device
			, getName() + "LightInjection0"
			, m_scene.getLpvGridSize() }
		, m_geometry{ ( geometryVolumes
			? GeometryInjectionPass::createResult( handler
				, m_device
				, getName()
				, 0u
				, m_scene.getLpvGridSize() )
			: Texture{} ) }
		, m_propagate{ LightVolumePassResult{ handler
				, m_device
				, getName() + "Propagate0"
				, m_scene.getLpvGridSize() }
			, LightVolumePassResult{ handler
				, m_device
				, getName() + "Propagate1"
				, m_scene.getLpvGridSize() } }
		, m_clearPass{ doCreateClearPass() }
	{
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
			m_lightPropagationPassesDesc = doCreatePropagationPasses();
			m_runnable = m_graph.compile( m_device.makeContext() );
			auto runnable = m_runnable.get();
			m_device.renderSystem.getEngine()->postEvent( makeGpuFunctorEvent( EventType::ePreRender
				, [runnable]( RenderDevice const & device
					, QueueData const & queueData )
				{
					runnable->record();
				} ) );
			m_initialised = true;
		}
	}

	void LightPropagationVolumesBase::cleanup()
	{
		m_initialised = false;
		m_lightLpvs.clear();
		m_lightPropagationPasses = {};
	}

	void LightPropagationVolumesBase::registerLight( Light * light )
	{
		auto it = m_lightLpvs.find( light );

		if ( it == m_lightLpvs.end() )
		{
			auto ires = m_lightLpvs.emplace( light
				, std::make_unique< LightLpv >( m_graph
					, m_clearPass
					, m_device
					, light->getName()
					, light->getScene()->getLightCache()
					, m_lightType
					, m_smResult
					, m_lpvGridConfigUbo
					, m_injection
					, ( m_geometryVolumes
						? &m_geometry
						: nullptr ) ) );
			for ( auto & lightInjectionPassDesc : ires.first->second->lightInjectionPassDescs )
			{
				m_lightPropagationPassesDesc.front()->addDependency( *lightInjectionPassDesc );
			}

			if ( m_geometryVolumes )
			{
				for ( auto index = 1u; index < m_lightPropagationPassesDesc.size(); ++index )
				{
					for ( auto & geometryInjectionPassDesc : ires.first->second->geometryInjectionPassDescs )
					{
						m_lightPropagationPassesDesc[index]->addDependency( *geometryInjectionPassDesc );
					}
				}
			}

			if ( m_runnable )
			{
				m_runnable.reset();
				m_runnable = m_graph.compile( m_device.makeContext() );
				auto runnable = m_runnable.get();
				m_device.renderSystem.getEngine()->postEvent( makeGpuFunctorEvent( EventType::ePreRender
					, [runnable]( RenderDevice const & device
						, QueueData const & queueData )
					{
						runnable->record();
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
			changed = lightLpv.second->update( updater
				, std::max( std::max( aabb.getDimensions()->x
					, aabb.getDimensions()->y )
					, aabb.getDimensions()->z ) / m_scene.getLpvGridSize() )
				|| changed;
		}

		if ( changed )
		{
			m_aabb = aabb;
			m_cameraPos = camPos;
			m_cameraDir = camDir;
			auto & grid = m_lpvGridConfigUbo.cpuUpdate( m_aabb
				, m_cameraPos
				, m_cameraDir
				, m_scene.getLpvGridSize()
				, m_scene.getLpvIndirectAttenuation() );
			m_gridsSize = castor::Point4f{ grid.getCenter()->x
				, grid.getCenter()->y
				, grid.getCenter()->z
				, grid.getCellSize() };
		}
	}

	crg::SemaphoreWait LightPropagationVolumesBase::render( crg::SemaphoreWait const & toWait
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

	void LightPropagationVolumesBase::accept( PipelineVisitorBase & visitor )
	{
		if ( m_initialised )
		{
			for ( auto & lightLpv : m_lightLpvs )
			{
				for ( auto & lightInjectionPass : lightLpv.second->lightInjectionPasses )
				{
					lightInjectionPass->accept( visitor );
				}

				if ( m_geometryVolumes )
				{
					for ( auto & geometryInjectionPass : lightLpv.second->geometryInjectionPasses )
					{
						geometryInjectionPass->accept( visitor );
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

			for ( auto i = 0u; i < uint32_t( LpvTexture::eCount ); ++i )
			{
				auto tex = LpvTexture( i );
				visitor.visit( "LPV Injection " + castor3d::getName( tex )
					, m_injection[tex]
					, m_graph.getFinalLayout( m_injection[tex].wholeViewId ).layout
					, TextureFactors::tex3D( &m_gridsSize ) );
			}

			if ( m_geometryVolumes )
			{
				visitor.visit( "LPV Geometry"
					, m_geometry
					, m_graph.getFinalLayout( m_geometry.wholeViewId ).layout
					, TextureFactors::tex3D( &m_gridsSize ) );
			}

			uint32_t level = 0u;

			for ( auto & propagate : m_propagate )
			{
				for ( auto i = 0u; i < uint32_t( LpvTexture::eCount ); ++i )
				{
					auto tex = LpvTexture( i );
					visitor.visit( "Layered LPV Propagation" + std::to_string( level ) + " " + castor3d::getName( tex )
						, propagate[tex]
						, m_graph.getFinalLayout( propagate[tex].wholeViewId ).layout
						, TextureFactors::tex3D( &m_gridsSize ) );
				}

				++level;
			}
		}
	}

	crg::FramePass & LightPropagationVolumesBase::doCreateClearPass()
	{
		auto & result = m_graph.createPass( "LpvClear"
			, [this]( crg::FramePass const & pass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				return std::make_unique< LpvClear >( pass
					, context
					, graph );
			} );

		for ( auto & texture : m_injection )
		{
			result.addTransferOutputView( texture.wholeViewId
				, VK_IMAGE_LAYOUT_UNDEFINED );
		}

		if ( m_geometryVolumes )
		{
			result.addTransferOutputView( m_geometry.wholeViewId
				, VK_IMAGE_LAYOUT_UNDEFINED );
		}

		return result;
	}

	crg::FramePass & LightPropagationVolumesBase::doCreatePropagationPass( std::vector< crg::FramePass const * > previousPasses
		, std::string const & name
		, LightVolumePassResult const & injection
		, LightVolumePassResult const & lpvResult
		, LightVolumePassResult const & propagation
		, uint32_t index )
	{
		auto & result = m_graph.createPass( getName() + name
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
				m_device.renderSystem.getEngine()->registerTimer( graph.getName() + "/LPV"
					, result->getTimer() );
				return result;
			} );

		for ( auto & previousPass : previousPasses )
		{
			result.addDependency( *previousPass );
		}

		m_lpvGridConfigUbo.createPassBinding( result
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
			result.addSampledView( m_geometry.sampledViewId
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

	std::vector< crg::FramePass * > LightPropagationVolumesBase::doCreatePropagationPasses()
	{
		uint32_t propIndex = 0u;
		std::vector< crg::FramePass * > result;
		std::vector< crg::FramePass const * > previousPasses;

		for ( auto & lightLpv : m_lightLpvs )
		{
			for ( auto & lightInjectionPassDesc : lightLpv.second->lightInjectionPassDescs )
			{
				previousPasses.push_back( lightInjectionPassDesc );
			}
		}

		result.push_back( &doCreatePropagationPass( previousPasses
			, "PropagationNoOccNoBlend"
			, m_injection
			, m_lpvResult
			, m_propagate[propIndex]
			, 0u ) );
		auto previous = result.back();
		previousPasses.clear();

		if ( m_geometryVolumes )
		{
			for ( auto & lightLpv : m_lightLpvs )
			{
				for ( auto & geometryInjectionPassDesc : lightLpv.second->geometryInjectionPassDescs )
				{
					previousPasses.push_back( geometryInjectionPassDesc );
				}
			}
		}

		for ( uint32_t i = 1u; i < MaxPropagationSteps; ++i )
		{
			auto & input = m_propagate[propIndex];
			propIndex = 1u - propIndex;
			auto & output = m_propagate[propIndex];
			std::string name = ( m_geometryVolumes
				? castor::String{ cuT( "OccBlend" ) }
				: castor::String{ cuT( "NoOccBlend" ) } );
			previousPasses.push_back( previous );
			result.push_back( &doCreatePropagationPass( previousPasses
				, "Propagation" + name + std::to_string( i )
				, input
				, m_lpvResult
				, output
				, i ) );
			previousPasses.clear();
			previous = result.back();
		}

		return result;
	}

	//*********************************************************************************************
}