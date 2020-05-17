#include "Castor3D/Render/ShadowMap/ShadowMapPass.hpp"

#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/Node/SceneCulledRenderNodes.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMap.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"

#include <ShaderAST/Shader.hpp>

using namespace castor;

namespace castor3d
{
	ShadowMapPass::ShadowMapPass( castor::String name
		, Engine & engine
		, MatrixUbo & matrixUbo
		, SceneCuller & culler
		, ShadowMap const & shadowMap )
		: RenderPass{ cuT( "ShadowMap" ), std::move( name ), engine, matrixUbo, culler }
		, m_shadowMap{ shadowMap }
		, m_shadowMapUbo{ engine }
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
	
	ashes::VkDescriptorSetLayoutBindingArray ShadowMapPass::doCreateTextureBindings( PipelineFlags const & flags )const
	{
		auto index = getMinTextureIndex();
		ashes::VkDescriptorSetLayoutBindingArray textureBindings;

		if ( flags.texturesCount )
		{
			textureBindings.emplace_back( makeDescriptorSetLayoutBinding( index
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT
				, flags.texturesCount ) );
			index += flags.texturesCount;
		}

		return textureBindings;
	}

	void ShadowMapPass::doUpdateFlags( PipelineFlags & flags )const
	{
		m_shadowMap.updateFlags( flags );
	}

	void ShadowMapPass::doFillTextureDescriptor( ashes::DescriptorSetLayout const & layout
		, uint32_t & index
		, BillboardListRenderNode & node
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		node.passNode.fillDescriptor( layout
			, index
			, *node.texDescriptorSet
			, ShadowMap::TextureFlags );
	}

	void ShadowMapPass::doFillTextureDescriptor( ashes::DescriptorSetLayout const & layout
		, uint32_t & index
		, SubmeshRenderNode & node
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		node.passNode.fillDescriptor( layout
			, index
			, *node.texDescriptorSet
			, ShadowMap::TextureFlags );
	}

	void ShadowMapPass::doUpdatePipeline( RenderPipeline & pipeline )const
	{
	}

	ShaderPtr ShadowMapPass::doGetVertexShaderSource( PipelineFlags const & flags )const
	{
		return m_shadowMap.getVertexShaderSource( flags );
	}

	ShaderPtr ShadowMapPass::doGetGeometryShaderSource( PipelineFlags const & flags )const
	{
		return m_shadowMap.getGeometryShaderSource( flags );
	}

	ShaderPtr ShadowMapPass::doGetPhongPixelShaderSource( PipelineFlags const & flags )const
	{
		return m_shadowMap.getPixelShaderSource( flags );
	}

	ShaderPtr ShadowMapPass::doGetPbrMRPixelShaderSource( PipelineFlags const & flags )const
	{
		return m_shadowMap.getPixelShaderSource( flags );
	}

	ShaderPtr ShadowMapPass::doGetPbrSGPixelShaderSource( PipelineFlags const & flags )const
	{
		return m_shadowMap.getPixelShaderSource( flags );
	}
}
