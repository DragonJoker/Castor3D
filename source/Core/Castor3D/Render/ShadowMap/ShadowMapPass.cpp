#include "Castor3D/Render/ShadowMap/ShadowMapPass.hpp"

#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
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
	ShadowMapPass::ShadowMapPass( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, castor::String const & typeName
		, castor::String name
		, MatrixUbo & matrixUbo
		, SceneCuller & culler
		, ShadowMap const & shadowMap
		, uint32_t instanceMult )
		: RenderNodesPass{ pass
			, context
			, graph
			, device
			, typeName
			, cuT( "ShadowMap" )
			, std::move( name )
			, RenderNodesPassDesc{ getExtent( shadowMap.getShadowPassResult()[SmTexture::eDepth].imageId ), matrixUbo, culler, RenderMode::eBoth, true, false }
				.instanceMult( instanceMult ) }
		, m_shadowMap{ shadowMap }
		, m_shadowMapUbo{ device }
	{
	}

	void ShadowMapPass::doUpdateNodes( QueueCulledRenderNodes & nodes )
	{
		if ( nodes.hasNodes() )
		{
			RenderNodesPass::doUpdate( nodes.instancedStaticNodes.frontCulled );
			RenderNodesPass::doUpdate( nodes.staticNodes.frontCulled );
			RenderNodesPass::doUpdate( nodes.skinnedNodes.frontCulled );
			RenderNodesPass::doUpdate( nodes.instancedSkinnedNodes.frontCulled );
			RenderNodesPass::doUpdate( nodes.morphingNodes.frontCulled );
			RenderNodesPass::doUpdate( nodes.billboardNodes.frontCulled );

			RenderNodesPass::doUpdate( nodes.instancedStaticNodes.backCulled );
			RenderNodesPass::doUpdate( nodes.staticNodes.backCulled );
			RenderNodesPass::doUpdate( nodes.skinnedNodes.backCulled );
			RenderNodesPass::doUpdate( nodes.instancedSkinnedNodes.backCulled );
			RenderNodesPass::doUpdate( nodes.morphingNodes.backCulled );
			RenderNodesPass::doUpdate( nodes.billboardNodes.backCulled );
		}
	}

	void ShadowMapPass::doFillAdditionalBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings )const
	{
		auto index = uint32_t( PassUboIdx::eCount );
		bindings.emplace_back( m_shadowMap.getScene().getLightCache().createLayoutBinding( index++ ) );
		bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT ) );
		m_initialised = true;
	}

	void ShadowMapPass::doFillAdditionalDescriptor( ashes::WriteDescriptorSetArray & descriptorWrites
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		auto index = uint32_t( PassUboIdx::eCount );
		descriptorWrites.push_back( getCuller().getScene().getLightCache().getBinding( index++ ) );
		descriptorWrites.push_back( m_shadowMapUbo.getDescriptorWrite( index++ ) );
	}

	ShaderPtr ShadowMapPass::doGetHullShaderSource( PipelineFlags const & flags )const
	{
		return ShaderPtr{};
	}

	ShaderPtr ShadowMapPass::doGetDomainShaderSource( PipelineFlags const & flags )const
	{
		return ShaderPtr{};
	}

	ShaderPtr ShadowMapPass::doGetGeometryShaderSource( PipelineFlags const & flags )const
	{
		return ShaderPtr{};
	}

	void ShadowMapPass::doUpdatePipeline( RenderPipeline & pipeline )
	{
	}
}
