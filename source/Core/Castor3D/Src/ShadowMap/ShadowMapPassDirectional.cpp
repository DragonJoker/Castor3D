#include "ShadowMapPassDirectional.hpp"

#include "Shader/ShaderProgram.hpp"
#include "Texture/TextureImage.hpp"
#include "Render/RenderPipeline.hpp"

#include <Graphics/Image.hpp>

using namespace castor;

namespace castor3d
{
	ShadowMapPassDirectional::ShadowMapPassDirectional( Engine & engine
		, Light & p_light
		, ShadowMap const & p_shadowMap )
		: ShadowMapPass{ engine, p_light, p_shadowMap }
	{
	}

	ShadowMapPassDirectional::~ShadowMapPassDirectional()
	{
	}

	bool ShadowMapPassDirectional::doInitialise( Size const & p_size )
	{
		Viewport viewport{ *getEngine() };
		real w = real( p_size.getWidth() );
		real h = real( p_size.getHeight() );
		viewport.setOrtho( -w / 2, w / 2, h / 2, -h / 2, -5120.0_r, 5120.0_r );
		viewport.update();
		m_camera = std::make_shared< Camera >( cuT( "ShadowMap_" ) + m_light.getName()
			, *m_light.getScene()
			, m_light.getParent()
			, std::move( viewport ) );
		m_camera->resize( p_size );

		m_renderQueue.initialise( *m_light.getScene(), *m_camera );
		return true;
	}

	void ShadowMapPassDirectional::doCleanup()
	{
		m_camera->detach();
		m_camera.reset();
	}

	void ShadowMapPassDirectional::doUpdate( RenderQueueArray & p_queues )
	{
		m_light.update( m_camera->getParent()->getDerivedPosition()
			, m_camera->getViewport()
			, m_index );
		m_camera->update();
		p_queues.push_back( m_renderQueue );
	}

	void ShadowMapPassDirectional::doRender( uint32_t p_face )
	{
		if ( m_camera && m_initialised )
		{
			m_camera->apply();
			m_matrixUbo.update( m_camera->getView()
				, m_camera->getViewport().getProjection() );
			doRenderNodes( m_renderQueue.getRenderNodes(), *m_camera );
		}
	}

	void ShadowMapPassDirectional::doPrepareBackPipeline( ShaderProgram & p_program
		, PipelineFlags const & p_flags )
	{
		if ( m_backPipelines.find( p_flags ) == m_backPipelines.end() )
		{
			RasteriserState rsState;
			rsState.setCulledFaces( Culling::eNone );
			DepthStencilState dsState;
			dsState.setDepthTest( true );
			auto & pipeline = *m_backPipelines.emplace( p_flags
				, getEngine()->getRenderSystem()->createRenderPipeline( std::move( dsState )
					, std::move( rsState )
					, BlendState{}
					, MultisampleState{}
					, p_program
					, p_flags ) ).first->second;

			getEngine()->postEvent( MakeFunctorEvent( EventType::ePreRender
				, [this, &pipeline, p_flags]()
				{
					pipeline.addUniformBuffer( m_matrixUbo.getUbo() );
					pipeline.addUniformBuffer( m_modelMatrixUbo.getUbo() );

					if ( checkFlag( p_flags.m_programFlags, ProgramFlag::eBillboards ) )
					{
						pipeline.addUniformBuffer( m_billboardUbo.getUbo() );
					}

					if ( checkFlag( p_flags.m_programFlags, ProgramFlag::eSkinning )
						&& !checkFlag( p_flags.m_programFlags, ProgramFlag::eInstantiation ) )
					{
						pipeline.addUniformBuffer( m_skinningUbo.getUbo() );
					}

					if ( checkFlag( p_flags.m_programFlags, ProgramFlag::eMorphing ) )
					{
						pipeline.addUniformBuffer( m_morphingUbo.getUbo() );
					}

					m_initialised = true;
				} ) );
		}
	}
}
