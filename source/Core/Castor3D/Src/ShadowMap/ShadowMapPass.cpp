#include "ShadowMapPass.hpp"

#include "Mesh/Submesh.hpp"
#include "Mesh/Buffer/GeometryBuffers.hpp"
#include "Render/RenderPipeline.hpp"
#include "Scene/BillboardList.hpp"
#include "Shader/ShaderProgram.hpp"
#include "ShadowMap/ShadowMap.hpp"
#include "Texture/TextureLayout.hpp"

#include <GlslShader.hpp>

using namespace Castor;

namespace Castor3D
{
	ShadowMapPass::ShadowMapPass( Engine & p_engine
		, Light & p_light
		, ShadowMap const & p_shadowMap )
		: RenderPass{ cuT( "ShadowMap" ), p_engine, nullptr }
		, m_shadowMap{ p_shadowMap }
		, m_light{ p_light }
	{
	}

	ShadowMapPass::~ShadowMapPass()
	{
	}

	void ShadowMapPass::Render( uint32_t p_face )
	{
		DoRender( p_face );
	}

	void ShadowMapPass::Update( RenderQueueArray & p_queues
		, int32_t p_index )
	{
		m_index = p_index;
		RenderPass::Update( p_queues );
	}

	void ShadowMapPass::DoRenderNodes( SceneRenderNodes & p_nodes
		, Camera const & p_camera )
	{
		RenderPass::DoRender( p_nodes.m_instancedNodes.m_backCulled, p_camera );
		RenderPass::DoRender( p_nodes.m_staticNodes.m_backCulled, p_camera );
		RenderPass::DoRender( p_nodes.m_skinningNodes.m_backCulled, p_camera );
		RenderPass::DoRender( p_nodes.m_instancedSkinningNodes.m_backCulled, p_camera );
		RenderPass::DoRender( p_nodes.m_morphingNodes.m_backCulled, p_camera );
		RenderPass::DoRender( p_nodes.m_billboardNodes.m_backCulled, p_camera );
	}

	void ShadowMapPass::DoUpdateFlags( TextureChannels & p_textureFlags
		, ProgramFlags & p_programFlags
		, SceneFlags & p_sceneFlags )const
	{
		m_shadowMap.UpdateFlags( p_textureFlags
			, p_programFlags
			, p_sceneFlags );
	}

	void ShadowMapPass::DoUpdatePipeline( RenderPipeline & p_pipeline )const
	{
	}

	void ShadowMapPass::DoPrepareFrontPipeline( ShaderProgram & p_program
		, PipelineFlags const & p_flags )
	{
	}

	void ShadowMapPass::DoPrepareBackPipeline( ShaderProgram & p_program
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

	GLSL::Shader ShadowMapPass::DoGetVertexShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, SceneFlags const & p_sceneFlags
		, bool p_invertNormals )const
	{
		return m_shadowMap.GetVertexShaderSource( p_textureFlags
			, p_programFlags
			, p_sceneFlags
			, p_invertNormals );
	}

	GLSL::Shader ShadowMapPass::DoGetGeometryShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, SceneFlags const & p_sceneFlags )const
	{
		return m_shadowMap.GetGeometryShaderSource( p_textureFlags
			, p_programFlags
			, p_sceneFlags );
	}

	GLSL::Shader ShadowMapPass::DoGetPixelShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, SceneFlags const & p_sceneFlags
		, ComparisonFunc p_alphaFunc )const
	{
		return m_shadowMap.GetPixelShaderSource( p_textureFlags
			, p_programFlags
			, p_sceneFlags
			, p_alphaFunc );
	}
}
