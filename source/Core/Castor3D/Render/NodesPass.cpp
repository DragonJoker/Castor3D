/*
See LICENSE file in root folder
*/
#include "Castor3D/Render/NodesPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/SubmeshComponentRegister.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Clustered/FrustumClusters.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightVolumePassResult.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMap.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Background/Background.hpp"
#include "Castor3D/Shader/ShaderBuffers/ShadowBuffer.hpp"
#include "Castor3D/Shader/Ubos/VoxelizerUbo.hpp"
#include "Castor3D/Shader/Ubos/LayeredLpvGridConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/LpvGridConfigUbo.hpp"

CU_ImplementSmartPtr( c3d, NodesPass )
CU_ImplementSmartPtr( c3d, IsNodesPassEnabled )

namespace c3d
{
	//*********************************************************************************************

	namespace rendndpass
	{
		static const String Suffix = cuT( "/NodesPass" );

		static SceneFlags adjustSceneFlags( SceneFlags sceneFlags
			, IndirectLightingData const & indirectLighting )
		{
			if ( !indirectLighting.vctConfigUbo
				|| !indirectLighting.vctFirstBounce
				|| !indirectLighting.vctSecondaryBounce )
			{
				remFlag( sceneFlags, SceneFlag::eVoxelConeTracing );
			}

			if ( !indirectLighting.lpvConfigUbo || !indirectLighting.lpvResult )
			{
				remFlag( sceneFlags, SceneFlag::eLpvGI );
			}

			if ( !indirectLighting.llpvConfigUbo || !indirectLighting.lpvResult )
			{
				remFlag( sceneFlags, SceneFlag::eLayeredLpvGI );
			}

			return sceneFlags;
		}
	}

	//*********************************************************************************************

	NodesPass::NodesPass( RenderDevice const & device
		, String const & categoryName
		, String const & typeName
		, String const & fullName
		, Texture * targetImage
		, Texture * targetDepth
		, NodesPassDesc const & desc )
		: OwnedBy< Engine >{ c3d::getEngine( device ) }
		, Named{ makeString( fullName ) }
		, m_device{ device }
		, m_renderSystem{ m_device.renderSystem }
		, m_cameraUbo{ desc.m_cameraUbo }
		, m_renderUbo{ desc.m_renderUbo }
		, m_targetImage{ targetImage }
		, m_targetDepth{ targetDepth }
		, m_typeName{ typeName }
		, m_typeID{ getEngine()->getRenderPassTypeID( m_typeName ) }
		, m_filters{ desc.m_filters }
		, m_category{ makeString( categoryName ) }
		, m_size{ desc.m_size.width, desc.m_size.height }
		, m_safeBand{ desc.m_safeBand }
		, m_sceneUbo{ desc.m_sceneUbo }
		, m_index{ desc.m_index }
		, m_handleStatic{ desc.m_handleStatic }
		, m_componentsMask{ desc.m_componentModeFlags }
	{
	}

	void NodesPass::update( CpuUpdater & updater )
	{
		doUpdate( updater );
		m_isDirty = false;
	}

	SubmeshComponentCombine NodesPass::adjustFlags( SubmeshComponentCombine const & submeshCombine )const
	{
		auto combine = doAdjustSubmeshComponents( submeshCombine );
		getEngine()->getSubmeshComponentsRegister().registerSubmeshComponentCombine( combine );
		return combine;
	}

	PassComponentCombine NodesPass::adjustFlags( PassComponentCombine const & passCombine )const
	{
		return getEngine()->getPassComponentsRegister().filterComponentFlags( getComponentsMask()
			, passCombine );
	}

	SceneFlags NodesPass::adjustFlags( SceneFlags flags )const
	{
		return doAdjustSceneFlags( flags );
	}

	TextureCombine NodesPass::adjustFlags( TextureCombine const & textureCombine )const
	{
		return getEngine()->getPassComponentsRegister().filterTextureFlags( getComponentsMask()
			, textureCombine );
	}

	void NodesPass::addShadowLayoutBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings
		, VkShaderStageFlags shaderStages
		, uint32_t & index )
	{
		for ( uint32_t j = 0u; j < uint32_t( LightType::eCount ); ++j )
		{
			// Depth
			addDescriptorSetLayoutBinding( bindings, index
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, shaderStages );
			// Depth Compare
			addDescriptorSetLayoutBinding( bindings, index
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, shaderStages );
			// Variance
			addDescriptorSetLayoutBinding( bindings, index
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, shaderStages );
		}

		// Shadow Buffer
		addDescriptorSetLayoutBinding( bindings, index
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, shaderStages );
		// Random Storage
		addDescriptorSetLayoutBinding( bindings, index
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			, shaderStages );
	}

	void NodesPass::addShadowLayoutBindings( SceneFlags const & sceneFlags
		, ashes::VkDescriptorSetLayoutBindingArray & bindings
		, VkShaderStageFlags shaderStages
		, uint32_t & index )
	{
		bool hasShadows{};

		for ( uint32_t j = 0u; j < uint32_t( LightType::eCount ); ++j )
		{
			if ( checkFlag( sceneFlags, SceneFlag( uint16_t( SceneFlag::eShadowBegin ) << j ) ) )
			{
				// Depth
				addDescriptorSetLayoutBinding( bindings, index
					, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
					, shaderStages );
				// Depth Compare
				addDescriptorSetLayoutBinding( bindings, index
					, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
					, shaderStages );
				// Variance
				addDescriptorSetLayoutBinding( bindings, index
					, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
					, shaderStages );
				hasShadows = true;
			}
		}

		if ( hasShadows )
		{
			// Shadow Buffer
			addDescriptorSetLayoutBinding( bindings, index
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, shaderStages );
			// Random Storage
			addDescriptorSetLayoutBinding( bindings, index
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, shaderStages );
		}
	}

	void NodesPass::addBackgroundLayoutBindings( SceneBackground const & background
		, ashes::VkDescriptorSetLayoutBindingArray & bindings
		, VkShaderStageFlags shaderStages
		, uint32_t & index )
	{
		background.addLayoutBindings( bindings, shaderStages, index );
	}

	void NodesPass::addGILayoutBindings( SceneFlags sceneFlags
		, IndirectLightingData const & indirectLighting
		, ashes::VkDescriptorSetLayoutBindingArray & bindings
		, VkShaderStageFlags shaderStages
		, uint32_t & index )
	{
		sceneFlags = rendndpass::adjustSceneFlags( sceneFlags, indirectLighting );

		if ( checkFlag( sceneFlags, SceneFlag::eVoxelConeTracing ) )
		{
			addDescriptorSetLayoutBinding( bindings, index
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, shaderStages );
			addDescriptorSetLayoutBinding( bindings, index
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, shaderStages );	// c3d_mapVoxelsFirstBounce
			addDescriptorSetLayoutBinding( bindings, index
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, shaderStages );	// c3d_mapVoxelsSecondaryBounce
		}
		else
		{
			if ( indirectLighting.vctConfigUbo )
			{
				index += 3u; // VCT: UBO + FirstBounce + SecondBounce.
			}

			if ( checkFlag( sceneFlags, SceneFlag::eRsmGI ) )
			{
				addDescriptorSetLayoutBinding( bindings, index
					, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
					, shaderStages );
			}
			else if ( indirectLighting.rsmResult )
			{
				++index; // RSM: Result.
			}

			if ( checkFlag( sceneFlags, SceneFlag::eLpvGI ) )
			{
				addDescriptorSetLayoutBinding( bindings, index
					, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
					, shaderStages );
				addDescriptorSetLayoutBinding( bindings, index
					, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
					, shaderStages );	// c3d_lpvAccumulationR
				addDescriptorSetLayoutBinding( bindings, index
					, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
					, shaderStages );	// c3d_lpvAccumulationG
				addDescriptorSetLayoutBinding( bindings, index
					, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
					, shaderStages );	// c3d_lpvAccumulationB
			}
			else if ( indirectLighting.lpvConfigUbo )
			{
				index += 4u; // LPV: UBO + AccumR + AccumG + AccumB.
			}

			if ( checkFlag( sceneFlags, SceneFlag::eLayeredLpvGI ) )
			{
				CU_Require( indirectLighting.llpvResult );
				addDescriptorSetLayoutBinding( bindings, index
					, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
					, shaderStages );

				if ( indirectLighting.llpvResult )
				{
					for ( size_t i = 0u; i < indirectLighting.llpvResult->size(); ++i )
					{
						addDescriptorSetLayoutBinding( bindings, index
							, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
							, shaderStages );	// c3d_lpvAccumulationRn
						addDescriptorSetLayoutBinding( bindings, index
							, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
							, shaderStages );	// c3d_lpvAccumulationGn
						addDescriptorSetLayoutBinding( bindings, index
							, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
							, shaderStages );	// c3d_lpvAccumulationBn
					}
				}
			}
			else if ( indirectLighting.llpvConfigUbo )
			{
				index += 1u; // LLPV: UBO
				index += 3u; // LLPV: Accum1R + Accum1G + Accum1B.
				index += 3u; // LLPV: Accum2R + Accum2G + Accum2B.
				index += 3u; // LLPV: Accum3R + Accum3G + Accum3B.
			}
		}
	}

	void NodesPass::addClusteredLightingLayoutBindings( FrustumClusters const & frustumClusters
		, ashes::VkDescriptorSetLayoutBindingArray & bindings
		, VkShaderStageFlags shaderStages
		, uint32_t & index )
	{
		frustumClusters.getClustersUbo().addLayoutBinding( bindings, index
			, shaderStages );
		addDescriptorSetLayoutBinding( bindings, index // ReducedLightsAABBBuffer
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			, shaderStages );
		addDescriptorSetLayoutBinding( bindings, index // PointLightIndexBuffer
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			, shaderStages );
		addDescriptorSetLayoutBinding( bindings, index // PointLightClusterBuffer
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			, shaderStages );
		addDescriptorSetLayoutBinding( bindings, index // SpotLightIndexBuffer
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			, shaderStages );
		addDescriptorSetLayoutBinding( bindings, index // SpotLightClusterBuffer
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			, shaderStages );
	}

	void NodesPass::addShadowDescriptorWrites( RenderSystem const & renderSystem
		, crg::RunnableGraph & graph
		, ashes::WriteDescriptorSetArray & descriptorWrites
		, ShadowMapLightTypeArray const & shadowMaps
		, ShadowBuffer const & shadowBuffer
		, uint32_t & index )
	{
#if !C3D_DebugDisableShadowMaps
		for ( auto i = 0u; i < uint32_t( LightType::eCount ); ++i )
		{
			for ( auto const & [shadowMapRef, _] : shadowMaps[i] )
			{
				auto const & result = shadowMapRef.get().getShadowPassResult( false );
				result.getTexture( SmTexture::eLinearDepth ).addTextureDescriptorWrite( descriptorWrites, result.getSampler( SmTexture::eVariance ), index );
				result.getTexture( SmTexture::eLinearDepth ).addTextureDescriptorWrite( descriptorWrites, index );// Compare sampler
				result.getTexture( SmTexture::eVariance ).addTextureDescriptorWrite( descriptorWrites, index );
			}
		}

		shadowBuffer.addDescriptorWrite( descriptorWrites, index );
		renderSystem.getRandomStorage().addDescriptorWrite( descriptorWrites, index );
#endif
	}

	void NodesPass::addShadowDescriptorWrites( RenderSystem const & renderSystem
		, crg::RunnableGraph & graph
		, SceneFlags const & sceneFlags
		, ashes::WriteDescriptorSetArray & descriptorWrites
		, ShadowMapLightTypeArray const & shadowMaps
		, ShadowBuffer const & shadowBuffer
		, uint32_t & index )
	{
#if !C3D_DebugDisableShadowMaps
		bool hasShadows{};

		for ( auto i = 0u; i < uint32_t( LightType::eCount ); ++i )
		{
			if ( checkFlag( sceneFlags, SceneFlag( uint16_t( SceneFlag::eShadowBegin ) << i ) ) )
			{
				for ( auto const & [shadowMapRef, _] : shadowMaps[i] )
				{
					auto const & result = shadowMapRef.get().getShadowPassResult( false );
					result.getTexture( SmTexture::eLinearDepth ).addTextureDescriptorWrite( descriptorWrites, result.getSampler( SmTexture::eVariance ), index );
					result.getTexture( SmTexture::eLinearDepth ).addTextureDescriptorWrite( descriptorWrites, index );// Compare sampler
					result.getTexture( SmTexture::eVariance ).addTextureDescriptorWrite( descriptorWrites, index );
					hasShadows = true;
				}
			}
		}

		if ( hasShadows )
		{
			shadowBuffer.addDescriptorWrite( descriptorWrites, index );
			renderSystem.getRandomStorage().addDescriptorWrite( descriptorWrites, index );
		}
#endif
	}

	void NodesPass::addBackgroundDescriptorWrites( SceneBackground const & background
		, ashes::WriteDescriptorSetArray & descriptorWrites
		, Texture * targetImage
		, uint32_t & index )
	{
		background.addDescriptorWrites( descriptorWrites
			, targetImage
			, index );
	}

	void NodesPass::addGIDescriptorWrites( SceneFlags sceneFlags
		, IndirectLightingData const & indirectLighting
		, ashes::WriteDescriptorSetArray & descriptorWrites
		, uint32_t & index )
	{
		sceneFlags = rendndpass::adjustSceneFlags( sceneFlags, indirectLighting );

		if ( checkFlag( sceneFlags, SceneFlag::eVoxelConeTracing ) )
		{
			CU_Require( indirectLighting.vctConfigUbo );
			CU_Require( indirectLighting.vctFirstBounce );
			CU_Require( indirectLighting.vctSecondaryBounce );
			indirectLighting.vctConfigUbo->addDescriptorWrite( descriptorWrites, index );
			indirectLighting.vctFirstBounce->addTextureDescriptorWrite( descriptorWrites, index );
			indirectLighting.vctSecondaryBounce->addTextureDescriptorWrite( descriptorWrites, index );
		}
		else
		{
			if ( indirectLighting.vctConfigUbo )
			{
				index += 3u; // VCT: UBO + FirstBounce + SecondBounce.
			}

			if ( checkFlag( sceneFlags, SceneFlag::eRsmGI ) )
				indirectLighting.rsmResult->addTextureDescriptorWrite( descriptorWrites, index );
			else if ( indirectLighting.rsmResult )
				++index; // RSM: Result.

			if ( checkFlag( sceneFlags, SceneFlag::eLpvGI ) )
			{
				CU_Require( indirectLighting.lpvConfigUbo );
				CU_Require( indirectLighting.lpvResult );
				if ( indirectLighting.lpvConfigUbo )
					indirectLighting.lpvConfigUbo->addDescriptorWrite( descriptorWrites, index );

				if ( indirectLighting.lpvResult )
				{
					auto const & lpv = *indirectLighting.lpvResult;
					lpv.getTexture( LpvTexture::eR ).addTextureDescriptorWrite( descriptorWrites, index );
					lpv.getTexture( LpvTexture::eG ).addTextureDescriptorWrite( descriptorWrites, index );
					lpv.getTexture( LpvTexture::eB ).addTextureDescriptorWrite( descriptorWrites, index );
				}
			}
			else if ( indirectLighting.lpvConfigUbo )
			{
				index += 4u; // LPV: UBO + AccumR + AccumG + AccumB.
			}

			if ( checkFlag( sceneFlags, SceneFlag::eLayeredLpvGI ) )
			{
				CU_Require( indirectLighting.llpvConfigUbo );
				CU_Require( indirectLighting.llpvResult );
				if ( indirectLighting.llpvConfigUbo )
					indirectLighting.llpvConfigUbo->addDescriptorWrite( descriptorWrites, index );

				if ( indirectLighting.llpvResult )
				{
					for ( auto const & plpv : *indirectLighting.llpvResult )
					{
						auto const & lpv = *plpv;
						lpv.getTexture( LpvTexture::eR ).addTextureDescriptorWrite( descriptorWrites, index );
						lpv.getTexture( LpvTexture::eG ).addTextureDescriptorWrite( descriptorWrites, index );
						lpv.getTexture( LpvTexture::eB ).addTextureDescriptorWrite( descriptorWrites, index );
					}
				}
			}
			else if ( indirectLighting.llpvConfigUbo )
			{
				index += 1u; // LLPV: UBO
				index += 3u; // LLPV: Accum1R + Accum1G + Accum1B.
				index += 3u; // LLPV: Accum2R + Accum2G + Accum2B.
				index += 3u; // LLPV: Accum3R + Accum3G + Accum3B.
			}
		}
	}

	void NodesPass::addClusteredLightingDescriptorWrites( FrustumClusters const & frustumClusters
		, ashes::WriteDescriptorSetArray & descriptorWrites
		, uint32_t & index )
	{
		frustumClusters.getClustersUbo().addDescriptorWrite( descriptorWrites, index );
		frustumClusters.getReducedLightsAABBBuffer().addDescriptorWrite( descriptorWrites, index );
		frustumClusters.getPointLightClusterIndexBuffer().addDescriptorWrite( descriptorWrites, index );
		frustumClusters.getPointLightClusterGridBuffer().addDescriptorWrite( descriptorWrites, index );
		frustumClusters.getSpotLightClusterIndexBuffer().addDescriptorWrite( descriptorWrites, index );
		frustumClusters.getSpotLightClusterGridBuffer().addDescriptorWrite( descriptorWrites, index );
	}

	bool NodesPass::areValidPassFlags( PassComponentCombine const & passFlags )const noexcept
	{
		if ( !doAreValidPassFlags( passFlags ) )
		{
			return false;
		}

		if ( passFlags.hasTransmissionFlag )
		{
			return !checkFlag( m_filters, RenderFilter::eTransmission );
		}

		if ( passFlags.hasAlphaTestFlag )
		{
			return !checkFlag( m_filters, RenderFilter::eAlphaTest );
		}

		if ( passFlags.hasAlphaBlendingFlag )
		{
			return !checkFlag( m_filters, RenderFilter::eAlphaBlend );
		}

		return !checkFlag( m_filters, RenderFilter::eOpaque );
	}

	bool NodesPass::isPassEnabled()const noexcept
	{
		return true;
	}

	bool NodesPass::isValidPass( Pass const & pass )const noexcept
	{
		return doIsValidPass( pass );
	}

	bool NodesPass::isValidRenderable( RenderedObject const & object )const noexcept
	{
		return doIsValidRenderable( object );
	}

	bool NodesPass::isValidNode( SceneNode const & node )const noexcept
	{
		return doIsValidNode( node )
			&& ( !handleStatic()
				|| ( filtersStatic() && !node.isStatic() )
				|| ( filtersNonStatic() && node.isStatic() ) );
	}

	bool NodesPass::doIsValidPass( Pass const & pass )const noexcept
	{
		return ( pass.getRenderPassInfo() == nullptr || pass.getRenderPassInfo()->create == nullptr )
			&& areValidPassFlags( pass.getPassFlags() );
	}

	bool NodesPass::doIsValidRenderable( RenderedObject const & object )const noexcept
	{
		return true;
	}
	
	SubmeshComponentCombine NodesPass::doAdjustSubmeshComponents( SubmeshComponentCombine submeshCombine )const
	{
		return submeshCombine;
	}

	SceneFlags NodesPass::doAdjustSceneFlags( SceneFlags flags )const
	{
		return flags;
	}
}
