#include "Castor3D/Render/ShadowMap/ShadowMapPass.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
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

#include <ShaderAST/Shader.hpp>

CU_ImplementSmartPtr( castor3d, ShadowMapPass )

namespace castor3d
{
	namespace shdmappass
	{
		static RenderNodesPassDesc buildDesc( ShadowMap const & shadowMap
			, CameraUbo const & cameraUbo
			, SceneCuller & culler
			, bool needsVsm
			, bool needsRsm
			, bool isStatic )
		{
			RenderNodesPassDesc result{ getExtent( shadowMap.getShadowPassResult( false )[SmTexture::eDepth].imageId )
				, cameraUbo
				, culler };
			result.isStatic( isStatic );

			if ( !isStatic )
			{
				auto & smResult = shadowMap.getShadowPassResult( true );
				result.implicitAction( smResult[SmTexture::eLinearDepth].sampledViewId
					, crg::RecordContext::clearAttachment( smResult[SmTexture::eLinearDepth].sampledViewId
						, getClearValue( SmTexture::eLinearDepth )
						, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ) );

				if ( needsVsm )
				{
					result.implicitAction( smResult[SmTexture::eVariance].sampledViewId
						, crg::RecordContext::clearAttachment( smResult[SmTexture::eVariance].sampledViewId
							, getClearValue( SmTexture::eVariance )
							, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ) );
				}

				if ( needsRsm )
				{
					result.implicitAction( smResult[SmTexture::eNormal].sampledViewId
						, crg::RecordContext::clearAttachment( smResult[SmTexture::eNormal].sampledViewId
							, getClearValue( SmTexture::eNormal )
							, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ) );
					result.implicitAction( smResult[SmTexture::ePosition].sampledViewId
						, crg::RecordContext::clearAttachment( smResult[SmTexture::ePosition].sampledViewId
							, getClearValue( SmTexture::ePosition )
							, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ) );
					result.implicitAction( smResult[SmTexture::eFlux].sampledViewId
						, crg::RecordContext::clearAttachment( smResult[SmTexture::eFlux].sampledViewId
							, getClearValue( SmTexture::eFlux )
							, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ) );
				}
			}

			result.componentModeFlags( ComponentModeFlag::eOpacity
				| ComponentModeFlag::eNormals
				| ComponentModeFlag::eGeometry
				| ( needsRsm ? ComponentModeFlag::eColour : ComponentModeFlag::eNone ) );

			return result;
		}
	}

	ShadowMapPass::ShadowMapPass( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, castor::String const & typeName
		, CameraUbo const & cameraUbo
		, SceneCuller & culler
		, ShadowMap const & shadowMap
		, bool needsVsm
		, bool needsRsm
		, bool isStatic )
		: RenderNodesPass{ pass
			, context
			, graph
			, device
			, typeName
			, {}
			, {}
			, shdmappass::buildDesc( shadowMap
				, cameraUbo
				, culler
				, needsVsm
				, needsRsm
				, isStatic ) }
		, m_shadowMap{ shadowMap }
		, m_shadowMapUbo{ device }
		, m_needsVsm{ needsVsm }
		, m_needsRsm{ needsRsm }
	{
	}

	bool ShadowMapPass::isPassEnabled()const
	{
#if !C3D_MeasureShadowMapImpact
		if ( getEngine()->areUpdateOptimisationsEnabled() )
		{
			return RenderNodesPass::isPassEnabled()
				&& ( m_renderQueue->isOutOfDate() || m_outOfDate );
		}
#endif
		return RenderNodesPass::isPassEnabled();
	}

	bool ShadowMapPass::isUpToDate()const
	{
		return !m_outOfDate;
	}

	void ShadowMapPass::setUpToDate()
	{
		m_outOfDate = m_renderQueue->isOutOfDate();
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

	void ShadowMapPass::doFillAdditionalBindings( PipelineFlags const & flags
		, ashes::VkDescriptorSetLayoutBindingArray & bindings )const
	{
		auto index = uint32_t( GlobalBuffersIdx::eCount );
		bindings.emplace_back( m_shadowMap.getScene().getLightCache().createLayoutBinding( index++ ) );
		bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT ) );	// ShadowMapUbo
		m_initialised = true;
	}

	void ShadowMapPass::doFillAdditionalDescriptor( PipelineFlags const & flags
		, ashes::WriteDescriptorSetArray & descriptorWrites
		, ShadowMapLightTypeArray const & shadowMaps
		, ShadowBuffer const * shadowBuffer )
	{
		auto index = uint32_t( GlobalBuffersIdx::eCount );
		descriptorWrites.push_back( getCuller().getScene().getLightCache().getBinding( index++ ) );
		descriptorWrites.push_back( m_shadowMapUbo.getDescriptorWrite( index++ ) );
	}
}
