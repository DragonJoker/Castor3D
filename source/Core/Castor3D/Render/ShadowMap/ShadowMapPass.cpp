#include "Castor3D/Render/ShadowMap/ShadowMapPass.hpp"

#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderQueue.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Node/QueueRenderNodes.hpp"
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
		, bool needsVsm
		, bool needsRsm )
		: RenderNodesPass{ pass
			, context
			, graph
			, device
			, typeName
			, cuT( "ShadowMap" )
			, std::move( name )
			, nullptr
			, RenderNodesPassDesc{ getExtent( shadowMap.getShadowPassResult()[SmTexture::eDepth].imageId )
				, matrixUbo
				, culler } }
		, m_shadowMap{ shadowMap }
		, m_needsVsm{ needsVsm }
		, m_needsRsm{ needsRsm }
		, m_shadowMapUbo{ device }
	{
	}

	bool ShadowMapPass::doIsValidRenderable( RenderedObject const & object )const
	{
		return object.isShadowCaster();
	}

	SubmeshFlags ShadowMapPass::doAdjustSubmeshFlags( SubmeshFlags flags )const
	{
		if ( !m_needsRsm )
		{
			remFlag( flags, SubmeshFlag::eNormals );
			remFlag( flags, SubmeshFlag::eTangents );
		}

		return flags;
	}

	void ShadowMapPass::doFillAdditionalBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings )const
	{
		auto index = uint32_t( GlobalBuffersIdx::eCount );
		bindings.emplace_back( m_shadowMap.getScene().getLightCache().createLayoutBinding( index++ ) );
		bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT ) );
		m_initialised = true;
	}

	void ShadowMapPass::doFillAdditionalDescriptor( ashes::WriteDescriptorSetArray & descriptorWrites
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		auto index = uint32_t( GlobalBuffersIdx::eCount );
		descriptorWrites.push_back( getCuller().getScene().getLightCache().getBinding( index++ ) );
		descriptorWrites.push_back( m_shadowMapUbo.getDescriptorWrite( index++ ) );
	}
}
