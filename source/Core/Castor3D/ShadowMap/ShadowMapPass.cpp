#include "Castor3D/ShadowMap/ShadowMapPass.hpp"

#include "Castor3D/Mesh/Submesh.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/ShadowMap/ShadowMap.hpp"
#include "Castor3D/Texture/TextureLayout.hpp"

#include <ShaderWriter/Shader.hpp>

using namespace castor;

namespace castor3d
{
	ShadowMapPass::ShadowMapPass( Engine & engine
		, MatrixUbo const & matrixUbo
		, SceneCuller & culler
		, ShadowMap const & shadowMap )
		: RenderPass{ cuT( "ShadowMap" ), cuT( "ShadowMap" ), engine, matrixUbo, culler }
		, m_shadowMap{ shadowMap }
	{
	}

	ShadowMapPass::~ShadowMapPass()
	{
	}

	void ShadowMapPass::doUpdateNodes( SceneCulledRenderNodes & nodes )
	{
		if ( nodes.hasNodes() )
		{
			RenderPass::doUpdate( nodes.instancedStaticNodes.frontCulled );
			RenderPass::doUpdate( nodes.staticNodes.frontCulled );
			RenderPass::doUpdate( nodes.skinnedNodes.frontCulled );
			RenderPass::doUpdate( nodes.instancedSkinnedNodes.frontCulled );
			RenderPass::doUpdate( nodes.morphingNodes.frontCulled );
			RenderPass::doUpdate( nodes.billboardNodes.frontCulled );

			RenderPass::doUpdate( nodes.instancedStaticNodes.backCulled );
			RenderPass::doUpdate( nodes.staticNodes.backCulled );
			RenderPass::doUpdate( nodes.skinnedNodes.backCulled );
			RenderPass::doUpdate( nodes.instancedSkinnedNodes.backCulled );
			RenderPass::doUpdate( nodes.morphingNodes.backCulled );
			RenderPass::doUpdate( nodes.billboardNodes.backCulled );
		}
	}

	ashes::DescriptorSetLayoutBindingArray ShadowMapPass::doCreateTextureBindings( PipelineFlags const & flags )const
	{
		auto index = MinBufferIndex;
		ashes::DescriptorSetLayoutBindingArray textureBindings;

		if ( checkFlag( flags.textureFlags, TextureChannel::eOpacity ) )
		{
			textureBindings.emplace_back( index++, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment );
		}

		return textureBindings;
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

	void ShadowMapPass::doFillTextureDescriptor( ashes::DescriptorSetLayout const & layout
		, uint32_t & index
		, BillboardListRenderNode & node
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		node.passNode.fillDescriptor( layout
			, index
			, *node.texDescriptorSet
			, true );
	}

	void ShadowMapPass::doFillTextureDescriptor( ashes::DescriptorSetLayout const & layout
		, uint32_t & index
		, SubmeshRenderNode & node
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		node.passNode.fillDescriptor( layout
			, index
			, *node.texDescriptorSet
			, true );
	}

	void ShadowMapPass::doUpdatePipeline( RenderPipeline & pipeline )const
	{
	}

	ShaderPtr ShadowMapPass::doGetVertexShaderSource( PassFlags const & passFlags
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

	ShaderPtr ShadowMapPass::doGetGeometryShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags )const
	{
		return m_shadowMap.getGeometryShaderSource( passFlags
			, textureFlags
			, programFlags
			, sceneFlags );
	}

	ShaderPtr ShadowMapPass::doGetLegacyPixelShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ashes::CompareOp alphaFunc )const
	{
		return m_shadowMap.getPixelShaderSource( passFlags
			, textureFlags
			, programFlags
			, sceneFlags
			, alphaFunc );
	}

	ShaderPtr ShadowMapPass::doGetPbrMRPixelShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ashes::CompareOp alphaFunc )const
	{
		return m_shadowMap.getPixelShaderSource( passFlags
			, textureFlags
			, programFlags
			, sceneFlags
			, alphaFunc );
	}

	ShaderPtr ShadowMapPass::doGetPbrSGPixelShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ashes::CompareOp alphaFunc )const
	{
		return m_shadowMap.getPixelShaderSource( passFlags
			, textureFlags
			, programFlags
			, sceneFlags
			, alphaFunc );
	}
}
