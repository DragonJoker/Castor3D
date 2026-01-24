#include "Castor3D/Render/ShadowMap/ShadowMapPass.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/SubmeshComponentRegister.hpp"
#include "Castor3D/Render/RenderQueue.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMap.hpp"

CU_ImplementSmartPtr( c3d, ShadowMapPass )

namespace c3d
{
	namespace shdmappass
	{
		static RenderNodesPassDesc buildDesc( ShadowMap const & shadowMap
			, CameraUbo const & cameraUbo
			, RenderUbo const & renderUbo
			, SceneCuller & culler
			, bool needsVsm
			, bool needsRsm
			, bool isStatic )
		{
			RenderNodesPassDesc result{ shadowMap.getShadowPassResult( false ).getExtent()
				, cameraUbo
				, renderUbo
				, culler };
			result.isStatic( isStatic );

			if ( !isStatic )
			{
				auto & smResult = shadowMap.getShadowPassResult( true );
				result.implicitAction( smResult.getSampledViewId( SmTexture::eLinearDepth )
					, crg::RecordContext::clearAttachment( smResult.getSampledViewId( SmTexture::eLinearDepth )
						, getClearValue( SmTexture::eLinearDepth ).color()
						, ImageLayout::eShaderReadOnly ) );

				if ( needsVsm )
				{
					result.implicitAction( smResult.getSampledViewId( SmTexture::eVariance )
						, crg::RecordContext::clearAttachment( smResult.getSampledViewId( SmTexture::eVariance )
							, getClearValue( SmTexture::eVariance ).color()
							, ImageLayout::eShaderReadOnly ) );
				}

				if ( needsRsm )
				{
					result.implicitAction( smResult.getSampledViewId( SmTexture::eNormal )
						, crg::RecordContext::clearAttachment( smResult.getSampledViewId( SmTexture::eNormal )
							, getClearValue( SmTexture::eNormal ).color()
							, ImageLayout::eShaderReadOnly ) );
					result.implicitAction( smResult.getSampledViewId( SmTexture::ePosition )
						, crg::RecordContext::clearAttachment( smResult.getSampledViewId( SmTexture::ePosition )
							, getClearValue( SmTexture::ePosition ).color()
							, ImageLayout::eShaderReadOnly ) );
					result.implicitAction( smResult.getSampledViewId( SmTexture::eFlux )
						, crg::RecordContext::clearAttachment( smResult.getSampledViewId( SmTexture::eFlux )
							, getClearValue( SmTexture::eFlux ).color()
							, ImageLayout::eShaderReadOnly ) );
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
		, String const & typeName
		, CameraUbo const & cameraUbo
		, RenderUbo const & renderUbo
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
				, renderUbo
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

	bool ShadowMapPass::isPassEnabled()const noexcept
	{
#if !C3D_MeasureShadowMapImpact
		if ( getEngine()->areUpdateOptimisationsEnabled() )
		{
			return RenderNodesPass::isPassEnabled()
				&& ( getRenderQueue().isOutOfDate() || m_outOfDate );
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
		m_outOfDate = getRenderQueue().isOutOfDate();
	}

	bool ShadowMapPass::doIsValidRenderable( RenderedObject const & object )const noexcept
	{
		return object.isShadowCaster();
	}
	
	SubmeshComponentCombine ShadowMapPass::doAdjustSubmeshComponents( SubmeshComponentCombine submeshCombine )const
	{
		if ( !m_needsRsm )
		{
			auto const & components = getEngine()->getSubmeshComponentsRegister();
			remFlags( submeshCombine, components.getNormalFlag() );
			remFlags( submeshCombine, components.getTangentFlag() );
			remFlags( submeshCombine, components.getBitangentFlag() );
			remFlags( submeshCombine, components.getColourFlag() );
			submeshCombine.hasNormalFlag = false;
			submeshCombine.hasTangentFlag = false;
			submeshCombine.hasBitangentFlag = false;
			submeshCombine.hasColourFlag = false;
		}

		return submeshCombine;
	}

	void ShadowMapPass::doFillAdditionalBindings( PipelineFlags const & flags
		, ashes::VkDescriptorSetLayoutBindingArray & bindings )const
	{
		auto index = uint32_t( GlobalBuffersIdx::eCount ) + flags.submeshDataBindings;
		m_shadowMap.getScene().getLightCache().addLayoutBinding( bindings
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, index );
		addDescriptorSetLayoutBinding( bindings
			, index
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT );	// ShadowMapUbo
		m_initialised = true;
	}

	void ShadowMapPass::doFillAdditionalDescriptor( PipelineFlags const & flags
		, ashes::WriteDescriptorSetArray & descriptorWrites
		, ShadowMapLightTypeArray const & shadowMaps
		, ShadowBuffer const * shadowBuffer )
	{
		auto index = uint32_t( GlobalBuffersIdx::eCount ) + flags.submeshDataBindings;
		getCuller().getScene().getLightCache().addBinding( descriptorWrites, index );
		m_shadowMapUbo.addDescriptorWrite( descriptorWrites, index );
	}
}
