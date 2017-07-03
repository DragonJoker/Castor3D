#include "ShadowMapPassDirectional.hpp"

#include "Shader/ShaderProgram.hpp"
#include "Texture/TextureImage.hpp"
#include "Render/RenderPipeline.hpp"

#include <Graphics/Image.hpp>

using namespace Castor;

namespace Castor3D
{
	ShadowMapPassDirectional::ShadowMapPassDirectional( Engine & p_engine
		, Light & p_light
		, ShadowMap const & p_shadowMap )
		: ShadowMapPass{ p_engine, p_light, p_shadowMap }
	{
	}

	ShadowMapPassDirectional::~ShadowMapPassDirectional()
	{
	}

	bool ShadowMapPassDirectional::DoInitialise( Size const & p_size )
	{
		Viewport l_viewport{ *GetEngine() };
		real l_w = real( p_size.width() );
		real l_h = real( p_size.height() );
		l_viewport.SetOrtho( -l_w / 4, l_w / 4, l_h / 4, -l_h / 4, -512.0_r, 512.0_r );
		l_viewport.Update();
		m_camera = std::make_shared< Camera >( cuT( "ShadowMap_" ) + m_light.GetName()
			, *m_light.GetScene()
			, m_light.GetParent()
			, std::move( l_viewport ) );
		m_camera->Resize( p_size );

		m_renderQueue.Initialise( *m_light.GetScene(), *m_camera );
		return true;
	}

	void ShadowMapPassDirectional::DoCleanup()
	{
		m_camera->Detach();
		m_camera.reset();
	}

	void ShadowMapPassDirectional::DoUpdate( RenderQueueArray & p_queues )
	{
		m_light.Update( m_camera->GetParent()->GetDerivedPosition()
			, m_camera->GetViewport()
			, m_index );
		m_camera->Update();
		p_queues.push_back( m_renderQueue );
	}

	void ShadowMapPassDirectional::DoRender( uint32_t p_face )
	{
		if ( m_camera && m_initialised )
		{
			m_camera->Apply();
			m_matrixUbo.Update( m_camera->GetView()
				, m_camera->GetViewport().GetProjection() );
			DoRenderNodes( m_renderQueue.GetRenderNodes(), *m_camera );
		}
	}

	void ShadowMapPassDirectional::DoPrepareBackPipeline( ShaderProgram & p_program
		, PipelineFlags const & p_flags )
	{
		if ( m_backPipelines.find( p_flags ) == m_backPipelines.end() )
		{
			RasteriserState l_rsState;
			l_rsState.SetCulledFaces( Culling::eNone );
			DepthStencilState l_dsState;
			l_dsState.SetDepthTest( true );
			auto & l_pipeline = *m_backPipelines.emplace( p_flags
				, GetEngine()->GetRenderSystem()->CreateRenderPipeline( std::move( l_dsState )
					, std::move( l_rsState )
					, BlendState{}
					, MultisampleState{}
					, p_program
					, p_flags ) ).first->second;

			GetEngine()->PostEvent( MakeFunctorEvent( EventType::ePreRender
				, [this, &l_pipeline, p_flags]()
				{
					l_pipeline.AddUniformBuffer( m_matrixUbo.GetUbo() );
					l_pipeline.AddUniformBuffer( m_modelMatrixUbo.GetUbo() );

					if ( CheckFlag( p_flags.m_programFlags, ProgramFlag::eBillboards ) )
					{
						l_pipeline.AddUniformBuffer( m_billboardUbo.GetUbo() );
					}

					if ( CheckFlag( p_flags.m_programFlags, ProgramFlag::eSkinning )
						&& !CheckFlag( p_flags.m_programFlags, ProgramFlag::eInstantiation ) )
					{
						l_pipeline.AddUniformBuffer( m_skinningUbo.GetUbo() );
					}

					if ( CheckFlag( p_flags.m_programFlags, ProgramFlag::eMorphing ) )
					{
						l_pipeline.AddUniformBuffer( m_morphingUbo.GetUbo() );
					}

					m_initialised = true;
				} ) );
		}
	}
}
