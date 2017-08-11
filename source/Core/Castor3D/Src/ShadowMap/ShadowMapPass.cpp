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

	void ShadowMapPass::doRenderNodes( SceneRenderNodes & nodes
		, Camera const & camera )
	{
		RenderPass::doRender( nodes.m_instantiatedStaticNodes.m_backCulled, camera );
		RenderPass::doRender( nodes.m_staticNodes.m_backCulled, camera );
		RenderPass::doRender( nodes.m_skinnedNodes.m_backCulled, camera );
		RenderPass::doRender( nodes.m_instantiatedSkinnedNodes.m_backCulled, camera );
		RenderPass::doRender( nodes.m_morphingNodes.m_backCulled, camera );
		RenderPass::doRender( nodes.m_billboardNodes.m_backCulled, camera );
	}

	void ShadowMapPass::doUpdateFlags( TextureChannels & textureFlags
		, ProgramFlags & programFlags
		, SceneFlags & sceneFlags )const
	{
		m_shadowMap.updateFlags( textureFlags
			, programFlags
			, sceneFlags );
	}

	void ShadowMapPass::doUpdatePipeline( RenderPipeline & p_pipeline )const
	{
	}

	void ShadowMapPass::doPrepareFrontPipeline( ShaderProgram & program
		, PipelineFlags const & flags )
	{
	}

	void ShadowMapPass::doPrepareBackPipeline( ShaderProgram & program
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

			getEngine()->postEvent( MakeFunctorEvent( EventType::ePreRender
				, [this, &pipeline, flags]()
				{
					pipeline.addUniformBuffer( m_matrixUbo.getUbo() );
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

	glsl::Shader ShadowMapPass::doGetVertexShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, bool invertNormals )const
	{
		return m_shadowMap.getVertexShaderSource( textureFlags
			, programFlags
			, sceneFlags
			, invertNormals );
	}

	glsl::Shader ShadowMapPass::doGetGeometryShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags )const
	{
		return m_shadowMap.getGeometryShaderSource( textureFlags
			, programFlags
			, sceneFlags );
	}

	glsl::Shader ShadowMapPass::doGetLegacyPixelShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ComparisonFunc alphaFunc )const
	{
		return m_shadowMap.getPixelShaderSource( textureFlags
			, programFlags
			, sceneFlags
			, alphaFunc );
	}

	glsl::Shader ShadowMapPass::doGetPbrMRPixelShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ComparisonFunc alphaFunc )const
	{
		return m_shadowMap.getPixelShaderSource( textureFlags
			, programFlags
			, sceneFlags
			, alphaFunc );
	}

	glsl::Shader ShadowMapPass::doGetPbrSGPixelShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ComparisonFunc alphaFunc )const
	{
		return m_shadowMap.getPixelShaderSource( textureFlags
			, programFlags
			, sceneFlags
			, alphaFunc );
	}
}
