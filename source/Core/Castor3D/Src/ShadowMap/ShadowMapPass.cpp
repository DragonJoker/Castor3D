#include "ShadowMapPass.hpp"

#include "Mesh/Submesh.hpp"
#include "Render/RenderPassTimer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Scene/BillboardList.hpp"
#include "Shader/Program.hpp"
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

	void ShadowMapPass::doUpdateNodes( SceneCulledRenderNodes & nodes
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

	void ShadowMapPass::doUpdatePipeline( RenderPipeline & pipeline )const
	{
	}

	void ShadowMapPass::doPrepareFrontPipeline( ShaderProgramSPtr program
		, renderer::VertexLayoutCRefArray const & layouts
		, PipelineFlags const & flags )
	{
		doPreparePipeline( program, layouts, flags );
	}

	void ShadowMapPass::doPrepareBackPipeline( ShaderProgramSPtr program
		, renderer::VertexLayoutCRefArray const & layouts
		, PipelineFlags const & flags )
	{
		doPreparePipeline( program, layouts, flags );
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
