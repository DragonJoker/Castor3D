#include "ShadowMapPassDirectional.hpp"

#include "Cache/MaterialCache.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Shader/PassBuffer/PassBuffer.hpp"
#include "Texture/TextureView.hpp"
#include "Render/RenderPipeline.hpp"
#include "Scene/Light/DirectionalLight.hpp"

#include <Buffer/UniformBuffer.hpp>
#include <Descriptor/DescriptorSet.hpp>
#include <Descriptor/DescriptorSetLayout.hpp>

#include <Graphics/Image.hpp>

using namespace castor;

namespace castor3d
{
	String const ShadowMapPassDirectional::ShadowMapUbo = cuT( "ShadowMap" );
	String const ShadowMapPassDirectional::FarPlane = cuT( "c3d_farPlane" );

	ShadowMapPassDirectional::ShadowMapPassDirectional( Engine & engine
		, Scene & scene
		, ShadowMap const & shadowMap )
		: ShadowMapPass{ engine, scene, shadowMap }
	{
	}

	ShadowMapPassDirectional::~ShadowMapPassDirectional()
	{
	}

	void ShadowMapPassDirectional::update( Camera const & camera
		, RenderQueueArray & queues
		, Light & light
		, uint32_t index )
	{
		light.updateShadow( light.getParent()->getDerivedPosition()
			, m_camera->getViewport()
			, index );
		m_camera->attachTo( light.getParent() );
		m_camera->update();

		if ( light.getDirectionalLight()->getFarPlane() != m_farPlane )
		{
			m_farPlane = light.getDirectionalLight()->getFarPlane();
		}

		doUpdate( queues );
	}

	void ShadowMapPassDirectional::render( uint32_t index )
	{
		if ( m_camera && m_initialised )
		{
			auto & config = m_shadowConfig->getData();

			if ( m_farPlane != config.farPlane )
			{
				config.farPlane = m_farPlane;
				m_shadowConfig->upload();
			}

			m_matrixUbo.update( m_camera->getView()
				, m_camera->getViewport().getProjection() );
			doUpdateNodes( m_renderQueue.getRenderNodes(), *m_camera );
		}
	}

	bool ShadowMapPassDirectional::doInitialise( Size const & size )
	{
		Viewport viewport;
		auto w = float( size.getWidth() );
		auto h = float( size.getHeight() );
		viewport.setOrtho( -w / 2, w / 2, h / 2, -h / 2, -5120.0_r, 5120.0_r );
		viewport.update();
		m_camera = std::make_shared< Camera >( cuT( "ShadowMapDirectional" )
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

	void ShadowMapPassDirectional::doCleanup()
	{
		m_shadowConfig.reset();
		m_camera->detach();
		m_camera.reset();
	}

	void ShadowMapPassDirectional::doFillDescriptor( renderer::DescriptorSetLayout const & layout
		, uint32_t & index
		, BillboardListRenderNode & node )
	{
		node.descriptorSet->createBinding( layout.getBinding( ShadowMapPassDirectional::UboBindingPoint )
			, *m_shadowConfig );
	}

	void ShadowMapPassDirectional::doFillDescriptor( renderer::DescriptorSetLayout const & layout
		, uint32_t & index
		, SubmeshRenderNode & node )
	{
		node.descriptorSet->createBinding( layout.getBinding( ShadowMapPassDirectional::UboBindingPoint )
			, *m_shadowConfig );
	}

	void ShadowMapPassDirectional::doUpdate( RenderQueueArray & queues )
	{
		queues.emplace_back( m_renderQueue );
	}

	void ShadowMapPassDirectional::doPreparePipeline( renderer::ShaderStageStateArray & program
		, PipelineFlags const & flags )
	{
		if ( m_backPipelines.find( flags ) == m_backPipelines.end() )
		{
			renderer::RasterisationState rsState;
			rsState.cullMode = renderer::CullModeFlag::eNone;
			renderer::DepthStencilState dsState;
			auto & pipeline = *m_backPipelines.emplace( flags
				, std::make_unique< RenderPipeline >( *getEngine()->getRenderSystem()
					, std::move( dsState )
					, std::move( rsState )
					, renderer::ColourBlendState::createDefault()
					, renderer::MultisampleState{}
					, program
					, flags ) ).first->second;

			auto initialise = [this, &pipeline, flags]()
			{
				auto uboBindings = doCreateUboBindings( flags );
				uboBindings.emplace_back( ShadowMapPassDirectional::UboBindingPoint, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eFragment );
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
