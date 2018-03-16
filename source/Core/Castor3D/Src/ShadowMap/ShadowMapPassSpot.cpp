#include "ShadowMapPassSpot.hpp"

#include "Render/RenderPipeline.hpp"
#include "Scene/Light/SpotLight.hpp"

using namespace castor;

namespace castor3d
{
	String const ShadowMapPassSpot::ShadowMapUbo = cuT( "ShadowMap" );
	String const ShadowMapPassSpot::FarPlane = cuT( "c3d_farPlane" );

	ShadowMapPassSpot::ShadowMapPassSpot( Engine & engine
		, Scene & scene
		, ShadowMap const & shadowMap )
		: ShadowMapPass{ engine, scene, shadowMap }
	{
	}

	ShadowMapPassSpot::~ShadowMapPassSpot()
	{
	}

	void ShadowMapPassSpot::update( Camera const & camera
		, RenderQueueArray & queues
		, Light & light
		, uint32_t index )
	{
		light.updateShadow( light.getParent()->getDerivedPosition()
			, m_camera->getViewport()
			, index );
		m_camera->attachTo( light.getParent() );
		m_camera->update();
		auto & data = m_shadowConfig->getData();
		data.farPlane = light.getSpotLight()->getFarPlane();
		queues.emplace_back( m_renderQueue );
	}

	void ShadowMapPassSpot::updateDeviceDependent( uint32_t index )
	{
		if ( m_camera && m_initialised )
		{
			m_shadowConfig->upload();
			m_matrixUbo.update( m_camera->getView()
				, m_camera->getViewport().getProjection() );
			doUpdateNodes( m_renderQueue.getCulledRenderNodes(), *m_camera );
		}
	}

	bool ShadowMapPassSpot::doInitialise( Size const & size )
	{
		Viewport viewport;
		m_camera = std::make_shared< Camera >( cuT( "ShadowMapSpot" )
			, m_scene
			, m_scene.getCameraRootNode()
			, std::move( viewport ) );
		m_camera->resize( size );

		m_shadowConfig = renderer::makeUniformBuffer< Configuration >( *getEngine()->getRenderSystem()->getCurrentDevice()
			, 1u
			, 0u
			, renderer::MemoryPropertyFlag::eHostVisible | renderer::MemoryPropertyFlag::eHostCoherent );

		m_renderQueue.initialise( m_scene, *m_camera );
		return true;
	}

	void ShadowMapPassSpot::doCleanup()
	{
		m_shadowConfig.reset();
		m_camera->detach();
		m_camera.reset();
	}

	void ShadowMapPassSpot::doFillDescriptor( renderer::DescriptorSetLayout const & layout
		, uint32_t & index
		, BillboardListRenderNode & node )
	{
		node.descriptorSet->createBinding( layout.getBinding( ShadowMapPassSpot::UboBindingPoint )
			, *m_shadowConfig );
	}

	void ShadowMapPassSpot::doFillDescriptor( renderer::DescriptorSetLayout const & layout
		, uint32_t & index
		, SubmeshRenderNode & node )
	{
		node.descriptorSet->createBinding( layout.getBinding( ShadowMapPassSpot::UboBindingPoint )
			, *m_shadowConfig );
	}

	void ShadowMapPassSpot::doUpdate( RenderQueueArray & queues )
	{
		queues.emplace_back( m_renderQueue );
	}

	void ShadowMapPassSpot::doPreparePipeline( renderer::ShaderStageStateArray & program
		, PipelineFlags const & flags )
	{
		if ( m_backPipelines.find( flags ) == m_backPipelines.end() )
		{
			renderer::RasterisationState rsState;
			rsState.cullMode = renderer::CullModeFlag::eNone;
			renderer::DepthStencilState dsState;
			auto bdState = renderer::ColourBlendState::createDefault();
			auto & pipeline = *m_backPipelines.emplace( flags
				, std::make_unique< RenderPipeline >( *getEngine()->getRenderSystem()
					, std::move( dsState )
					, std::move( rsState )
					, std::move( bdState )
					, renderer::MultisampleState{}
					, program
					, flags ) ).first->second;

			auto initialise = [this, &pipeline, flags]()
			{
				auto uboBindings = doCreateUboBindings( flags );
				uboBindings.emplace_back( ShadowMapPassSpot::UboBindingPoint, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eFragment );
				auto layout = getEngine()->getRenderSystem()->getCurrentDevice()->createDescriptorSetLayout( std::move( uboBindings ) );
				std::vector< renderer::DescriptorSetLayoutPtr > layouts;
				layouts.emplace_back( std::move( layout ) );
				pipeline.setDescriptorSetLayouts( std::move( layouts ) );
				m_initialised = true;
			};

			if ( getEngine()->getRenderSystem()->hasCurrentDevice() )
			{
				initialise();
			}
			else
			{
				getEngine()->postEvent( makeFunctorEvent( EventType::ePreRender, initialise ) );
			}
		}
	}
}
