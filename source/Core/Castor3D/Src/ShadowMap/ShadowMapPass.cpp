#include "ShadowMapPass.hpp"

#include "Mesh/Submesh.hpp"
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
		m_timer->start();
	}

	void ShadowMapPass::stopTimer( renderer::CommandBuffer const & commandBuffer )
	{
		m_timer->stop();
	}

	void ShadowMapPass::doUpdateNodes( SceneRenderNodes & nodes
		, Camera const & camera )
	{
		RenderPass::doUpdate( nodes.instancedStaticNodes.backCulled, camera );
		RenderPass::doUpdate( nodes.staticNodes.backCulled, camera );
		RenderPass::doUpdate( nodes.skinnedNodes.backCulled, camera );
		RenderPass::doUpdate( nodes.instancedSkinnedNodes.backCulled, camera );
		RenderPass::doUpdate( nodes.morphingNodes.backCulled, camera );
		RenderPass::doUpdate( nodes.billboardNodes.backCulled, camera );
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

	void ShadowMapPass::doPreparePipeline( renderer::ShaderStageStateArray & program
		, PipelineFlags const & flags )
	{
		if ( m_backPipelines.find( flags ) == m_backPipelines.end() )
		{
			renderer::RasterisationState rsState;
			rsState.cullMode = renderer::CullModeFlag::eNone;
			renderer::DepthStencilState dsState;
			auto blState = renderer::ColourBlendState::createDefault();
			auto & pipeline = *m_backPipelines.emplace( flags
				, std::make_unique< RenderPipeline >( *getEngine()->getRenderSystem()
					, std::move( dsState )
					, std::move( rsState )
					, std::move( blState )
					, renderer::MultisampleState{}
					, program
					, flags ) ).first->second;

			getEngine()->postEvent( makeFunctorEvent( EventType::ePreRender
				, [this, &pipeline, flags]()
				{
					m_initialised = true;
				} ) );
		}
	}

	void ShadowMapPass::doUpdatePipeline( RenderPipeline & pipeline )const
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
