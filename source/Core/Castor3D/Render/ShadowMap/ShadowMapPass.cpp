#include "Castor3D/Render/ShadowMap/ShadowMapPass.hpp"

#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Node/QueueCulledRenderNodes.hpp"
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
	namespace
	{
		void fillAdditionalDescriptor( RenderPipeline const & pipeline
			, ashes::WriteDescriptorSetArray & descriptorWrites
			, Scene const & scene
			, ShadowMapUbo const & shadowMapUbo )
		{
			auto index = uint32_t( PassUboIdx::eCount );
			auto & flags = pipeline.getFlags();
			descriptorWrites.push_back( scene.getLightCache().getDescriptorWrite( index++ ) );
			descriptorWrites.push_back( shadowMapUbo.getDescriptorWrite( index++ ) );
		}
	}

	ShadowMapPass::ShadowMapPass( crg::FramePass const & pass
		, crg::GraphContext const & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, castor::String name
		, MatrixUbo & matrixUbo
		, SceneCuller & culler
		, ShadowMap const & shadowMap
		, uint32_t instanceMult )
		: SceneRenderPass{ pass
			, context
			, graph
			, device
			, cuT( "ShadowMap" )
			, std::move( name )
			, SceneRenderPassDesc{ shadowMap.getShadowPassResult()[SmTexture::eDepth].image.data->info.extent, matrixUbo, culler, RenderMode::eBoth, true, false }
				.instanceMult( instanceMult ) }
		, m_shadowMap{ shadowMap }
		, m_shadowMapUbo{ device }
	{
	}

	void ShadowMapPass::doUpdateNodes( QueueCulledRenderNodes & nodes )
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

	void ShadowMapPass::doFillAdditionalBindings( PipelineFlags const & flags
		, ashes::VkDescriptorSetLayoutBindingArray & bindings )const
	{
		auto index = uint32_t( PassUboIdx::eCount );
		bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
			, VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT ) );
		m_initialised = true;
	}

	void ShadowMapPass::doFillAdditionalDescriptor( RenderPipeline const & pipeline
		, ashes::WriteDescriptorSetArray & descriptorWrites
		, BillboardRenderNode & node
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		fillAdditionalDescriptor( pipeline
			, descriptorWrites
			, getCuller().getScene()
			, m_shadowMapUbo );
	}

	void ShadowMapPass::doFillAdditionalDescriptor( RenderPipeline const & pipeline
		, ashes::WriteDescriptorSetArray & descriptorWrites
		, SubmeshRenderNode & node
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		fillAdditionalDescriptor( pipeline
			, descriptorWrites
			, getCuller().getScene()
			, m_shadowMapUbo );
	}

	ShaderPtr ShadowMapPass::doGetGeometryShaderSource( PipelineFlags const & flags )const
	{
		return ShaderPtr{};
	}

	void ShadowMapPass::doUpdatePipeline( RenderPipeline & pipeline )
	{
	}
}
