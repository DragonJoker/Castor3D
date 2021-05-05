#include "Castor3D/Render/ShadowMap/ShadowMapPass.hpp"

#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Node/SceneCulledRenderNodes.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Background/Background.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMap.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"

#include <ShaderAST/Shader.hpp>

CU_ImplementCUSmartPtr( castor3d, ShadowMapPass )

using namespace castor;

namespace castor3d
{
	ShadowMapPass::ShadowMapPass( RenderDevice const & device
		, castor::String name
		, MatrixUbo & matrixUbo
		, SceneCuller & culler
		, ShadowMap const & shadowMap
		, ashes::RenderPassPtr renderPass )
		: SceneRenderPass{ device
			, cuT( "ShadowMap" )
			, std::move( name )
			, SceneRenderPassDesc{ shadowMap.getShadowPassResult()[SmTexture::eDepth].getTexture()->getDimensions(), matrixUbo, culler, RenderMode::eBoth, true, false }
			, std::move( renderPass ) }
		, m_shadowMap{ shadowMap }
		, m_shadowMapUbo{ device }
	{
	}

	void ShadowMapPass::doUpdateNodes( SceneCulledRenderNodes & nodes )
	{
		if ( nodes.hasNodes() )
		{
			SceneRenderPass::doUpdate( nodes.instancedStaticNodes.frontCulled );
			SceneRenderPass::doUpdate( nodes.staticNodes.frontCulled );
			SceneRenderPass::doUpdate( nodes.skinnedNodes.frontCulled );
			SceneRenderPass::doUpdate( nodes.instancedSkinnedNodes.frontCulled );
			SceneRenderPass::doUpdate( nodes.morphingNodes.frontCulled );
			SceneRenderPass::doUpdate( nodes.billboardNodes.frontCulled );

			SceneRenderPass::doUpdate( nodes.instancedStaticNodes.backCulled );
			SceneRenderPass::doUpdate( nodes.staticNodes.backCulled );
			SceneRenderPass::doUpdate( nodes.skinnedNodes.backCulled );
			SceneRenderPass::doUpdate( nodes.instancedSkinnedNodes.backCulled );
			SceneRenderPass::doUpdate( nodes.morphingNodes.backCulled );
			SceneRenderPass::doUpdate( nodes.billboardNodes.backCulled );
		}
	}
	
	ashes::VkDescriptorSetLayoutBindingArray ShadowMapPass::doCreateTextureBindings( PipelineFlags const & flags )const
	{
		auto index = 0u;
		ashes::VkDescriptorSetLayoutBindingArray textureBindings;

		if ( !flags.textures.empty() )
		{
			textureBindings.emplace_back( makeDescriptorSetLayoutBinding( index
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT
				, uint32_t( flags.textures.size() ) ) );
			index += uint32_t( flags.textures.size() );
		}

		if ( checkFlag( flags.passFlags, PassFlag::eMetallicRoughness )
			|| checkFlag( flags.passFlags, PassFlag::eSpecularGlossiness ) )
		{
			textureBindings.emplace_back( makeDescriptorSetLayoutBinding( index++
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// c3d_mapIrradiance
			textureBindings.emplace_back( makeDescriptorSetLayoutBinding( index++
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// c3d_mapPrefiltered
			textureBindings.emplace_back( makeDescriptorSetLayoutBinding( index++
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ) );	// c3d_mapBrdf
		}

		return textureBindings;
	}

	namespace
	{
		template< typename DataTypeT, typename InstanceTypeT >
		void fillTexDescriptor( RenderPipeline const & pipeline
			, ashes::DescriptorSetLayout const & layout
			, uint32_t & index
			, ObjectRenderNode< DataTypeT, InstanceTypeT > & node
			, ShadowMapLightTypeArray const & shadowMaps )
		{
			index = 0u;
			ashes::WriteDescriptorSetArray writes;
			auto & flags = pipeline.getFlags();

			if ( !flags.textures.empty() )
			{
				node.passNode.fillDescriptor( layout
					, index
					, writes
					, flags.textures );
			}

			if ( checkFlag( flags.passFlags, PassFlag::eMetallicRoughness )
				|| checkFlag( flags.passFlags, PassFlag::eSpecularGlossiness ) )
			{
				auto & background = *node.sceneNode.getScene()->getBackground();

				if ( background.hasIbl() )
				{
					auto & ibl = background.getIbl();
					bindTexture( ibl.getIrradianceTexture()
						, ibl.getIrradianceSampler()
						, writes
						, index );
					bindTexture( ibl.getPrefilteredEnvironmentTexture()
						, ibl.getPrefilteredEnvironmentSampler()
						, writes
						, index );
					bindTexture( ibl.getPrefilteredBrdfTexture()
						, ibl.getPrefilteredBrdfSampler()
						, writes
						, index );
				}
			}

			node.texDescriptorSet->setBindings( writes );
		}
	}

	void ShadowMapPass::doFillTextureDescriptor( RenderPipeline const & pipeline
		, ashes::DescriptorSetLayout const & layout
		, uint32_t & index
		, BillboardListRenderNode & node
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		fillTexDescriptor( pipeline, layout, index, node, shadowMaps );
	}

	void ShadowMapPass::doFillTextureDescriptor( RenderPipeline const & pipeline
		, ashes::DescriptorSetLayout const & layout
		, uint32_t & index
		, SubmeshRenderNode & node
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		fillTexDescriptor( pipeline, layout, index, node, shadowMaps );
	}

	ShaderPtr ShadowMapPass::doGetGeometryShaderSource( PipelineFlags const & flags )const
	{
		return ShaderPtr{};
	}

	void ShadowMapPass::doUpdatePipeline( RenderPipeline & pipeline )
	{
	}
}
