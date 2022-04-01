#include "Castor3D/Render/GlobalIllumination/ReflectiveShadowMaps/ReflectiveShadowMaps.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/GlobalIllumination/ReflectiveShadowMaps/RsmGIPass.hpp"
#include "Castor3D/Render/GlobalIllumination/ReflectiveShadowMaps/RsmInterpolatePass.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapResult.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Light/DirectionalLight.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
#include "Castor3D/Scene/Light/SpotLight.hpp"

#include <CastorUtils/Miscellaneous/StringUtils.hpp>

#include <ashespp/Buffer/Buffer.hpp>
#include <ashespp/Buffer/BufferView.hpp>

#include <RenderGraph/GraphContext.hpp>
#include <RenderGraph/RunnableGraph.hpp>
#include <RenderGraph/RunnablePass.hpp>

CU_ImplementSmartPtr( castor3d, ReflectiveShadowMaps )

namespace castor3d
{
	//*********************************************************************************************

	namespace rsm
	{
		class RsmClear
			: public crg::RunnablePass
		{
		public:
			RsmClear( crg::FramePass const & pass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
				: crg::RunnablePass{ pass
					, context
					, graph
					, Callbacks{ []( uint32_t ){}
						, GetPipelineStateCallback( [](){ return crg::getPipelineState( VK_PIPELINE_STAGE_TRANSFER_BIT ); } )
						, [this]( crg::RecordContext & ctx, VkCommandBuffer cb, uint32_t i ){ doRecordInto( ctx, cb, i ); } } }
			{
			}

		protected:
			void doRecordInto( crg::RecordContext & context
				, VkCommandBuffer commandBuffer
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

		static Texture createImage( RenderDevice const & device
			, crg::ResourcesCache & resources
			, castor::String const & name
			, VkFormat format
			, VkExtent3D const & size )
		{
			return Texture{ device
				, resources
				, name
				, 0u
				, VkExtent3D{ size.width / 4u, size.height / 4u, 1u }
				, 1u
				, 1u
				, format
				, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
					| VK_IMAGE_USAGE_SAMPLED_BIT
					| VK_IMAGE_USAGE_TRANSFER_DST_BIT ) };
		}

		static TextureArray createImages( RenderDevice const & device
			, crg::ResourcesCache & resources
			, castor::String const & name
			, VkExtent3D const & size )
		{
			TextureArray result;
			result.emplace_back( createImage( device, resources, name + "GI", VK_FORMAT_R16G16B16A16_SFLOAT, size ) );
			result.emplace_back( createImage( device, resources, name + "Normals", VK_FORMAT_R16G16B16A16_SFLOAT, size ) );
			return result;
		}
	}

	//*********************************************************************************************

	ReflectiveShadowMaps::LightRsm::LightRsm( crg::FrameGraph & graph
		, crg::FramePassArray previousPasses
		, RenderDevice const & device
		, LightCache const & plightCache
		, LightType lightType
		, ShadowBuffer const & shadowBuffer
		, CameraUbo const & cameraUbo
		, crg::ImageViewId const & depthObj
		, crg::ImageViewId const & nmlOcc
		, ShadowMapResult const & smResult
		, TextureArray const & intermediate
		, Texture const & result )
		: lightCache{ plightCache }
		, giPass{ castor::makeUnique< RsmGIPass >( graph
			, std::move( previousPasses )
			, device
			, lightType
			, shadowBuffer
			, intermediate[0].getExtent()
			, cameraUbo
			, depthObj
			, nmlOcc
			, smResult
			, intermediate ) }
		, interpolatePass{ castor::makeUnique< RsmInterpolatePass >( graph
			, giPass->getPass()
			, device
			, lightType
			, shadowBuffer
			, result.getExtent()
			, cameraUbo
			, depthObj
			, nmlOcc
			, smResult
			, giPass->getConfigUbo()
			, giPass->getSamplesSsbo()
			, intermediate[0]
			, intermediate[1]
			, result ) }
		, lastPass{ &interpolatePass->getPass() }
	{
	}

	void ReflectiveShadowMaps::LightRsm::update( CpuUpdater & updater )
	{
		auto & sceneObjs = updater.dirtyScenes[lightCache.getScene()];
		auto & light = *updater.light;
		auto changed = sceneObjs.dirtyLights.end() != std::find( sceneObjs.dirtyLights.begin(), sceneObjs.dirtyLights.end(), &light )
			|| light.getRsmConfig().intensity.isDirty()
			|| light.getRsmConfig().maxRadius.isDirty()
			|| light.getRsmConfig().sampleCount.isDirty();

		if ( changed )
		{
			giPass->update( light );
		}
	}

	//*********************************************************************************************

	ReflectiveShadowMaps::ReflectiveShadowMaps( crg::ResourceHandler & handler
		, Scene const & scene
		, RenderDevice const & device
		, CameraUbo const & cameraUbo
		, ShadowBuffer const & shadowBuffer
		, crg::ImageViewId const & depthObj
		, crg::ImageViewId const & nmlOcc
		, ShadowMapResult const & directionalSmResult
		, ShadowMapResult const & pointSmResult
		, ShadowMapResult const & spotSmResult
		, Texture const & result )
		: castor::Named{ "RSM" }
		, m_scene{ scene }
		, m_device{ device }
		, m_cameraUbo{ cameraUbo }
		, m_shadowBuffer{ shadowBuffer }
		, m_depthObj{ depthObj }
		, m_nmlOcc{ nmlOcc }
		, m_directionalSmResult{ directionalSmResult }
		, m_pointSmResult{ pointSmResult }
		, m_spotSmResult{ spotSmResult }
		, m_graph{ handler, getName() }
		, m_intermediate{ rsm::createImages( device
			, *result.resources
			, "RSMIntermediate"
			, result.getExtent() ) }
		, m_result{ result }
		, m_clearPass{ doCreateClearPass() }
		, m_lastPass{ &m_clearPass }
	{
		for ( auto & value : m_intermediate )
		{
			value.create();
		}
	}

	ReflectiveShadowMaps::~ReflectiveShadowMaps()noexcept
	{
		for ( auto & intermediate : m_intermediate )
		{
			intermediate.destroy();
		}
	}

	void ReflectiveShadowMaps::initialise()
	{
		if ( !m_initialised
			&& m_scene.needsGlobalIllumination( GlobalIlluminationType::eRsm ) )
		{
			m_runnable = m_graph.compile( m_device.makeContext() );
			m_device.renderSystem.getEngine()->postEvent( makeGpuFunctorEvent( GpuEventType::ePreRender
				, [this]( RenderDevice const &
					, QueueData const & )
				{
					m_runnable->record();
				} ) );
			m_initialised = true;
		}
	}

	void ReflectiveShadowMaps::cleanup()
	{
		m_initialised = false;
		m_lightRsms.clear();
	}

	void ReflectiveShadowMaps::registerLight( Light * light )
	{
		if ( auto lit = m_lightRsms.find( light );
			lit == m_lightRsms.end() )
		{
			auto [it, res] = m_lightRsms.emplace( light
				, std::make_unique< LightRsm >( m_graph
					, crg::FramePassArray{ m_lastPass }
					, m_device
					, light->getScene()->getLightCache()
					, light->getLightType()
					, m_shadowBuffer
					, m_cameraUbo
					, m_depthObj
					, m_nmlOcc
					, ( light->getLightType() == LightType::eDirectional
						? m_directionalSmResult
						: (light->getLightType() == LightType::ePoint
							? m_pointSmResult
							: m_spotSmResult ) )
					, m_intermediate
					, m_result ) );
			m_lastPass = it->second->lastPass;
			m_graph.addOutput( m_result.wholeViewId
				, crg::makeLayoutState( VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ) );

			if ( m_runnable )
			{
				m_runnable.reset();
				m_runnable = m_graph.compile( m_device.makeContext() );
				auto runnable = m_runnable.get();
				m_device.renderSystem.getEngine()->postEvent( makeGpuFunctorEvent( GpuEventType::ePreRender
					, [runnable]( RenderDevice const & device
						, QueueData const & queueData )
					{
						runnable->record();
					} ) );
			}
		}
	}

	void ReflectiveShadowMaps::update( CpuUpdater & updater )
	{
		if ( !m_initialised
			|| !m_scene.needsGlobalIllumination( GlobalIlluminationType::eRsm ) )
		{
			return;
		}

		for ( auto & lightRsm : m_lightRsms )
		{
			updater.light = lightRsm.first;
			lightRsm.second->update( updater );
		}
	}

	crg::SemaphoreWaitArray ReflectiveShadowMaps::render( crg::SemaphoreWaitArray const & toWait
		, ashes::Queue const & queue )
	{
		if ( !m_initialised
			|| !m_scene.needsGlobalIllumination( GlobalIlluminationType::eRsm )
			|| m_lightRsms.empty() )
		{
			return toWait;
		}

		return m_runnable->run( toWait, queue );
	}

	void ReflectiveShadowMaps::accept( ConfigurationVisitorBase & visitor )
	{
		if ( m_initialised )
		{
			for ( auto & lightRsm : m_lightRsms )
			{
				lightRsm.second->giPass->accept( visitor );
				lightRsm.second->interpolatePass->accept( visitor );
			}

			visitor.visit( getName() + " GI"
				, m_intermediate[0]
				, m_graph.getFinalLayoutState( m_intermediate[0].wholeViewId ).layout
				, TextureFactors{}.invert( true ) );
			visitor.visit( getName() + " Normal"
				, m_intermediate[1]
				, m_graph.getFinalLayoutState( m_intermediate[1].wholeViewId ).layout
				, TextureFactors{}.invert( true ) );
			visitor.visit( getName() + " Result"
				, m_result
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				, TextureFactors{}.invert( true ) );
		}
	}

	crg::FramePass & ReflectiveShadowMaps::doCreateClearPass()
	{
		auto & result = m_graph.createPass( "RsmClear"
			, []( crg::FramePass const & pass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				return std::make_unique< rsm::RsmClear >( pass
					, context
					, graph );
			} );
		result.addTransferOutputView( m_result.wholeViewId );
		return result;
	}

	//*********************************************************************************************
}
