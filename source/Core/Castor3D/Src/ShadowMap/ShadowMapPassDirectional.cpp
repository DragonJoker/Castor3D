#include "ShadowMapPassDirectional.hpp"

#include "Shader/ShaderProgram.hpp"
#include "Texture/TextureImage.hpp"
#include "Render/RenderPipeline.hpp"
#include "Scene/Light/DirectionalLight.hpp"

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
		, m_shadowConfig{ ShadowMapUbo
			, *engine.getRenderSystem()
			, UboBindingPoint }
		, m_farPlane{ *m_shadowConfig.createUniform< UniformType::eFloat >( FarPlane ) }
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
		light.update( light.getParent()->getDerivedPosition()
			, m_camera->getViewport()
			, index );
		m_camera->attachTo( light.getParent() );
		m_camera->update();
		m_farPlane.setValue( light.getDirectionalLight()->getFarPlane() );
		doUpdate( queues );
	}

	void ShadowMapPassDirectional::render( uint32_t index )
	{
		if ( m_camera && m_initialised )
		{
			m_shadowConfig.update();
			m_shadowConfig.bindTo( UboBindingPoint );
			m_camera->apply();
			m_matrixUbo.update( m_camera->getView()
				, m_camera->getViewport().getProjection() );
			doRenderNodes( m_renderQueue.getRenderNodes(), *m_camera );
		}
	}

	bool ShadowMapPassDirectional::doInitialise( Size const & size )
	{
		Viewport viewport{ *getEngine() };
		real w = real( size.getWidth() );
		real h = real( size.getHeight() );
		viewport.setOrtho( -w / 2, w / 2, h / 2, -h / 2, -5120.0_r, 5120.0_r );
		viewport.update();
		m_camera = std::make_shared< Camera >( cuT( "ShadowMapDirectional" )
			, m_scene
			, m_scene.getCameraRootNode()
			, std::move( viewport ) );
		m_camera->resize( size );

		m_renderQueue.initialise( m_scene, *m_camera );
		return true;
	}

	void ShadowMapPassDirectional::doCleanup()
	{
		m_shadowConfig.cleanup();
		m_camera->detach();
		m_camera.reset();
	}

	void ShadowMapPassDirectional::doUpdate( RenderQueueArray & queues )
	{
		queues.push_back( m_renderQueue );
	}

	void ShadowMapPassDirectional::doPreparePipeline( ShaderProgram & program
		, PipelineFlags const & flags )
	{
		if ( m_backPipelines.find( flags ) == m_backPipelines.end() )
		{
			RasteriserState rsState;
			rsState.setCulledFaces( Culling::eNone );
			DepthStencilState dsState;
			dsState.setDepthTest( true );
			auto & pipeline = *m_backPipelines.emplace( flags
				, getEngine()->getRenderSystem()->createRenderPipeline( std::move( dsState )
					, std::move( rsState )
					, BlendState{}
					, MultisampleState{}
					, program
					, flags ) ).first->second;

			getEngine()->postEvent( makeFunctorEvent( EventType::ePreRender
				, [this, &pipeline, flags]()
			{
				pipeline.addUniformBuffer( m_matrixUbo.getUbo() );
				pipeline.addUniformBuffer( m_modelUbo.getUbo() );
				pipeline.addUniformBuffer( m_modelMatrixUbo.getUbo() );
				pipeline.addUniformBuffer( m_shadowConfig );

				if ( checkFlag( flags.m_programFlags, ProgramFlag::eBillboards ) )
				{
					pipeline.addUniformBuffer( m_billboardUbo.getUbo() );
				}

				if ( checkFlag( flags.m_programFlags, ProgramFlag::eSkinning )
					&& !checkFlag( flags.m_programFlags, ProgramFlag::eInstantiation ) )
				{
					pipeline.addUniformBuffer( m_skinningUbo.getUbo() );
				}

				if ( checkFlag( flags.m_programFlags, ProgramFlag::eMorphing ) )
				{
					pipeline.addUniformBuffer( m_morphingUbo.getUbo() );
				}

				m_initialised = true;
			} ) );
		}
	}
}
