#include "ShadowMapPass.hpp"

#include "Mesh/Submesh.hpp"
#include "Mesh/Buffer/GeometryBuffers.hpp"
#include "Render/RenderPassTimer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Scene/BillboardList.hpp"
#include "Shader/ShaderProgram.hpp"
#include "ShadowMap/ShadowMap.hpp"
#include "Texture/TextureLayout.hpp"

#include <GlslShader.hpp>

using namespace Castor;

namespace Castor3D
{
	ShadowMapPass::ShadowMapPass( Engine & engine
		, Light & p_light
		, ShadowMap const & p_shadowMap )
		: RenderPass{ cuT( "ShadowMap" ), engine, nullptr }
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
		RenderPass::DoRender( p_nodes.m_instantiatedStaticNodes.m_backCulled, p_camera );
		RenderPass::DoRender( p_nodes.m_staticNodes.m_backCulled, p_camera );
		RenderPass::DoRender( p_nodes.m_skinnedNodes.m_backCulled, p_camera );
		RenderPass::DoRender( p_nodes.m_instantiatedSkinnedNodes.m_backCulled, p_camera );
		RenderPass::DoRender( p_nodes.m_morphingNodes.m_backCulled, p_camera );
		RenderPass::DoRender( p_nodes.m_billboardNodes.m_backCulled, p_camera );
	}

	void ShadowMapPass::DoUpdateFlags( TextureChannels & textureFlags
		, ProgramFlags & programFlags
		, SceneFlags & sceneFlags )const
	{
		m_shadowMap.UpdateFlags( textureFlags
			, programFlags
			, sceneFlags );
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
			RasteriserState rsState;
			rsState.SetCulledFaces( Culling::eNone );
			DepthStencilState dsState;
			dsState.SetDepthTest( true );
			auto & pipeline = *m_backPipelines.emplace( p_flags
				, GetEngine()->GetRenderSystem()->CreateRenderPipeline( std::move( dsState )
					, std::move( rsState )
					, BlendState{}
					, MultisampleState{}
					, p_program
					, p_flags ) ).first->second;

			GetEngine()->PostEvent( MakeFunctorEvent( EventType::ePreRender
				, [this, &pipeline, p_flags]()
				{
					pipeline.AddUniformBuffer( m_matrixUbo.GetUbo() );
					pipeline.AddUniformBuffer( m_modelMatrixUbo.GetUbo() );

					if ( CheckFlag( p_flags.m_programFlags, ProgramFlag::eBillboards ) )
					{
						pipeline.AddUniformBuffer( m_billboardUbo.GetUbo() );
					}

					if ( CheckFlag( p_flags.m_programFlags, ProgramFlag::eSkinning )
						&& !CheckFlag( p_flags.m_programFlags, ProgramFlag::eInstantiation ) )
					{
						pipeline.AddUniformBuffer( m_skinningUbo.GetUbo() );
					}

					if ( CheckFlag( p_flags.m_programFlags, ProgramFlag::eMorphing ) )
					{
						pipeline.AddUniformBuffer( m_morphingUbo.GetUbo() );
					}

					m_initialised = true;
				} ) );
		}
	}

	GLSL::Shader ShadowMapPass::DoGetVertexShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, bool invertNormals )const
	{
		return m_shadowMap.GetVertexShaderSource( textureFlags
			, programFlags
			, sceneFlags
			, invertNormals );
	}

	GLSL::Shader ShadowMapPass::DoGetGeometryShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags )const
	{
		return m_shadowMap.GetGeometryShaderSource( textureFlags
			, programFlags
			, sceneFlags );
	}

	GLSL::Shader ShadowMapPass::DoGetLegacyPixelShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ComparisonFunc alphaFunc )const
	{
		return m_shadowMap.GetPixelShaderSource( textureFlags
			, programFlags
			, sceneFlags
			, alphaFunc );
	}

	GLSL::Shader ShadowMapPass::DoGetPbrMRPixelShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ComparisonFunc alphaFunc )const
	{
		return m_shadowMap.GetPixelShaderSource( textureFlags
			, programFlags
			, sceneFlags
			, alphaFunc );
	}

	GLSL::Shader ShadowMapPass::DoGetPbrSGPixelShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ComparisonFunc alphaFunc )const
	{
		return m_shadowMap.GetPixelShaderSource( textureFlags
			, programFlags
			, sceneFlags
			, alphaFunc );
	}
}
