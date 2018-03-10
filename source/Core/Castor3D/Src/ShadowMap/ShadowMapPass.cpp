#include "ShadowMapPass.hpp"

#include "Mesh/Submesh.hpp"
#include "Buffer/GeometryBuffers.hpp"
#include "Render/RenderPassTimer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Scene/BillboardList.hpp"
#include "Shader/ShaderProgram.hpp"
#include "ShadowMap/ShadowMap.hpp"
#include "Texture/TextureLayout.hpp"

#include <GlslShader.hpp>

using namespace castor;

namespace castor3d
{
	ShadowMapPass::ShadowMapPass( Engine & engine
		, Scene & scene
		, ShadowMap const & shadowMap )
		: RenderPass{ cuT( "ShadowMap" ), engine, nullptr }
		, m_scene{ scene }
		, m_shadowMap{ shadowMap }
	{
	}

	ShadowMapPass::~ShadowMapPass()
	{
	}

	void ShadowMapPass::startTimer( renderer::CommandBuffer const & commandBuffer )
	{
		m_timer->start( commandBuffer );
	}

	void ShadowMapPass::stopTimer( renderer::CommandBuffer const & commandBuffer )
	{
		m_timer->stop( commandBuffer );
	}

	void ShadowMapPass::doRenderNodes( SceneRenderNodes & nodes
		, Camera const & camera )
	{
		RenderPass::doUpdate( nodes.m_instantiatedStaticNodes.m_backCulled, camera );
		RenderPass::doUpdate( nodes.m_staticNodes.m_backCulled, camera );
		RenderPass::doUpdate( nodes.m_skinnedNodes.m_backCulled, camera );
		RenderPass::doUpdate( nodes.m_instantiatedSkinnedNodes.m_backCulled, camera );
		RenderPass::doUpdate( nodes.m_morphingNodes.m_backCulled, camera );
		RenderPass::doUpdate( nodes.m_billboardNodes.m_backCulled, camera );
	}

	void ShadowMapPass::doUpdateFlags( PassFlags & passFlags
		, TextureChannels & textureFlags
		, ProgramFlags & programFlags
		, SceneFlags & sceneFlags )const
	{
		m_shadowMap.updateFlags( passFlags
			, textureFlags
			, programFlags
			, sceneFlags );
	}

	void ShadowMapPass::doPreparePipeline( ShaderProgram & program
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

	void ShadowMapPass::doUpdatePipeline( RenderPipeline & p_pipeline )const
	{
	}

	void ShadowMapPass::doPrepareFrontPipeline( renderer::ShaderStageStateArray & program
		, PipelineFlags const & flags )
	{
		doPreparePipeline( program, flags );
	}

	void ShadowMapPass::doPrepareBackPipeline( renderer::ShaderStageStateArray & program
		, PipelineFlags const & flags )
	{
		doPreparePipeline( program, flags );
	}

	glsl::Shader ShadowMapPass::doGetVertexShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, bool invertNormals )const
	{
		return m_shadowMap.getVertexShaderSource( passFlags
			, textureFlags
			, programFlags
			, sceneFlags
			, invertNormals );
	}

	glsl::Shader ShadowMapPass::doGetGeometryShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags )const
	{
		return m_shadowMap.getGeometryShaderSource( passFlags
			, textureFlags
			, programFlags
			, sceneFlags );
	}

	glsl::Shader ShadowMapPass::doGetLegacyPixelShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, renderer::CompareOp alphaFunc )const
	{
		return m_shadowMap.getPixelShaderSource( passFlags
			, textureFlags
			, programFlags
			, sceneFlags
			, alphaFunc );
	}

	glsl::Shader ShadowMapPass::doGetPbrMRPixelShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, renderer::CompareOp alphaFunc )const
	{
		return m_shadowMap.getPixelShaderSource( passFlags
			, textureFlags
			, programFlags
			, sceneFlags
			, alphaFunc );
	}

	glsl::Shader ShadowMapPass::doGetPbrSGPixelShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, renderer::CompareOp alphaFunc )const
	{
		return m_shadowMap.getPixelShaderSource( passFlags
			, textureFlags
			, programFlags
			, sceneFlags
			, alphaFunc );
	}
}
