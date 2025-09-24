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

CU_ImplementSmartPtr( c3d, ReflectiveShadowMaps )

namespace c3d
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
						, GetPipelineStateCallback( [](){ return crg::getPipelineState( PipelineStageFlags::eTransfer ); } )
						, [this]( crg::RecordContext const & ctx, VkCommandBuffer cb, uint32_t i ){ doRecordInto( ctx, cb, i ); } } }
			{
			}

		protected:
			void doRecordInto( [[maybe_unused]] crg::RecordContext const & context
				, VkCommandBuffer commandBuffer
				, [[maybe_unused]] uint32_t index )
			{
				auto clearValue = crg::convert( transparentBlackClearColor );

				for ( auto & [binding, attach] : getPass().getOutputs() )
				{
					auto view = attach->view();
					auto image = getGraph().createImage( view.data->image );
					auto subresourceRange = convert( view.data->info.subresourceRange );
					assert( attach->isTransferInputImageView() );
					context->vkCmdClearColorImage( commandBuffer
						, image
						, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
						, &clearValue
						, 1u
						, &subresourceRange );
				}
			}
		};

		static Texture createImage( RenderDevice const & device
			, crg::ResourcesCache & resources
			, String const & name
			, PixelFormat format
			, Extent3D const & size )
		{
			return Texture{ device
				, resources
				, name
				, { ImageCreateFlags::eNone
					, Extent3D{ size.width / 4u, size.height / 4u, 1u }, 1u, 1u
					, format
					, ( ImageUsageFlags::eColorAttachment
						| ImageUsageFlags::eSampled
						| ImageUsageFlags::eTransferDst ) }
				, {} };
		}

		static TextureArray createImages( RenderDevice const & device
			, crg::ResourcesCache & resources
			, String const & name
			, Extent3D const & size )
		{
			TextureArray result;
			result.emplace_back( createImage( device, resources, name + "GI", PixelFormat::eR16G16B16A16_SFLOAT, size ) );
			result.emplace_back( createImage( device, resources, name + "Normals", PixelFormat::eR16G16B16A16_SFLOAT, size ) );
			return result;
		}
	}

	//*********************************************************************************************

	ReflectiveShadowMaps::LightRsm::LightRsm( crg::FrameGraph & graph
		, RenderDevice const & device
		, LightCache const & plightCache
		, LightType lightType
		, ShadowBuffer const & shadowBuffer
		, CameraUbo const & cameraUbo
		, Texture const & depthObj
		, Texture const & nmlOcc
		, ShadowMapResult const & smResult
		, TextureArray & intermediate
		, Texture & result )
		: lightCache{ plightCache }
		, giPass{ makeUnique< RsmGIPass >( graph
			, device
			, lightType
			, shadowBuffer
			, intermediate[0].getExtent()
			, cameraUbo
			, depthObj
			, nmlOcc
			, smResult
			, intermediate[0]
			, intermediate[1] ) }
		, interpolatePass{ makeUnique< RsmInterpolatePass >( graph
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

	ReflectiveShadowMaps::ReflectiveShadowMaps( crg::ResourcesCache & resources
		, Scene const & scene
		, RenderDevice const & device
		, CameraUbo const & cameraUbo
		, ShadowBuffer const & shadowBuffer
		, Texture const & depthObj
		, Texture const & nmlOcc
		, ShadowMapResult const & directionalSmResult
		, ShadowMapResult const & pointSmResult
		, ShadowMapResult const & spotSmResult
		, Texture & result )
		: Named{ "RSM" }
		, m_scene{ scene }
		, m_device{ device }
		, m_cameraUbo{ cameraUbo }
		, m_shadowBuffer{ shadowBuffer }
		, m_depthObj{ depthObj }
		, m_nmlOcc{ nmlOcc }
		, m_directionalSmResult{ directionalSmResult }
		, m_pointSmResult{ pointSmResult }
		, m_spotSmResult{ spotSmResult }
		, m_graph{ resources.getHandler(), toUtf8( getName() ) }
		, m_intermediate{ rsm::createImages( device
			, resources
			, "RSMIntermediate"
			, result.getExtent() ) }
		, m_result{ result }
	{
		doCreateClearPass();
		for ( auto & value : m_intermediate )
		{
			value.create();
		}

		m_graph.addInput( m_directionalSmResult.getTargetViewId( SmTexture::eNormal )
			, makeLayoutState( ImageLayout::eShaderReadOnly ) );
		m_graph.addInput( m_directionalSmResult.getTargetViewId( SmTexture::ePosition )
			, makeLayoutState( ImageLayout::eShaderReadOnly ) );
		m_graph.addInput( m_directionalSmResult.getTargetViewId( SmTexture::eFlux )
			, makeLayoutState( ImageLayout::eShaderReadOnly ) );
		m_graph.addInput( m_pointSmResult.getTargetViewId( SmTexture::eNormal )
			, makeLayoutState( ImageLayout::eShaderReadOnly ) );
		m_graph.addInput( m_pointSmResult.getTargetViewId( SmTexture::ePosition )
			, makeLayoutState( ImageLayout::eShaderReadOnly ) );
		m_graph.addInput( m_pointSmResult.getTargetViewId( SmTexture::eFlux )
			, makeLayoutState( ImageLayout::eShaderReadOnly ) );
		m_graph.addInput( m_spotSmResult.getTargetViewId( SmTexture::eNormal )
			, makeLayoutState( ImageLayout::eShaderReadOnly ) );
		m_graph.addInput( m_spotSmResult.getTargetViewId( SmTexture::ePosition )
			, makeLayoutState( ImageLayout::eShaderReadOnly ) );
		m_graph.addInput( m_spotSmResult.getTargetViewId( SmTexture::eFlux )
			, makeLayoutState( ImageLayout::eShaderReadOnly ) );
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
			m_scene.getEngine()->registerTimer( makeString( m_runnable->getName() ) + cuT( "/Graph" )
				, m_runnable->getTimer() );
			printGraph( *m_runnable );
			m_recordEvent = m_device.renderSystem.getEngine()->postEvent( makeGpuFunctorEvent( GpuEventType::ePreRender
				, [this]( RenderDevice const &
					, QueueData const & )
				{
					m_runnable->record();
					m_recordEvent = nullptr;
				} ) );
			m_initialised = true;
		}
	}

	void ReflectiveShadowMaps::cleanup()
	{
		if ( m_recordEvent )
			m_recordEvent->skip();
		m_initialised = false;
		m_lightRsms.clear();
	}

	void ReflectiveShadowMaps::registerLight( LightInstance * light )
	{
		if ( auto [it, res] = m_lightRsms.try_emplace( light );
			res )
		{
			it->second = makeRawUnique< LightRsm >( m_graph
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
				, m_result );
			if ( m_lightRsms.size() == 1u )
				m_graph.addOutput( m_result.getWholeViewId()
					, makeLayoutState( ImageLayout::eShaderReadOnly ) );

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
				m_recordEvent = m_device.renderSystem.getEngine()->postEvent( makeGpuFunctorEvent( GpuEventType::ePreRender
					, [this]( RenderDevice const &
						, QueueData const & )
					{
						m_runnable->record();
						m_recordEvent = nullptr;
					} ) );
			}
		}
	}

	void ReflectiveShadowMaps::update( CpuUpdater & updater )const
	{
		if ( !m_initialised
			|| !m_scene.needsGlobalIllumination( GlobalIlluminationType::eRsm ) )
		{
			return;
		}

		for ( auto const & [light, rsm] : m_lightRsms )
		{
			updater.light = light;
			rsm->update( updater );
		}
	}

	SemaphoreWaitArray ReflectiveShadowMaps::render( SemaphoreWaitArray const & toWait
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
			for ( auto const & [_, rsm] : m_lightRsms )
			{
				rsm->giPass->accept( visitor );
				rsm->interpolatePass->accept( visitor );
			}

			visitor.visit( getName() + " GI"
				, m_intermediate[0]
				, m_graph.getFinalLayoutState( m_intermediate[0].getWholeViewId() ).layout
				, TextureFactors{}.invert( true ) );
			visitor.visit( getName() + " Normal"
				, m_intermediate[1]
				, m_graph.getFinalLayoutState( m_intermediate[1].getWholeViewId() ).layout
				, TextureFactors{}.invert( true ) );
			visitor.visit( getName() + " Result"
				, m_result
				, ImageLayout::eShaderReadOnly
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
				return makeRawUnique< rsm::RsmClear >( pass
					, context
					, graph );
			} );
		m_result.setLastAttach( result.addOutputTransferImage( m_result.getWholeViewId() ) );
		return result;
	}

	//*********************************************************************************************
}
