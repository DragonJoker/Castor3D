#include "Castor3D/Render/RenderNodesPass.hpp"

#include "Castor3D/Config.hpp"
#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/PoolUniformBuffer.hpp"
#include "Castor3D/Cache/AnimatedObjectGroupCache.hpp"
#include "Castor3D/Cache/BillboardCache.hpp"
#include "Castor3D/Cache/GeometryCache.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Cache/ShaderCache.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/MeshletComponent.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/SubmeshComponent.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/SubmeshComponentRegister.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderQueue.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/RenderTechniqueVisitor.hpp"
#include "Castor3D/Render/Clustered/FrustumClusters.hpp"
#include "Castor3D/Render/Culling/SceneCuller.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightVolumePassResult.hpp"
#include "Castor3D/Render/Node/BillboardRenderNode.hpp"
#include "Castor3D/Render/Node/SubmeshRenderNode.hpp"
#include "Castor3D/Render/Node/QueueRenderNodes.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMap.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Animation/AnimatedMesh.hpp"
#include "Castor3D/Scene/Animation/AnimatedSkeleton.hpp"
#include "Castor3D/Scene/Background/Background.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/ShaderBuffers/ShadowBuffer.hpp"
#include "Castor3D/Shader/ShaderBuffers/SssProfileBuffer.hpp"
#include "Castor3D/Shader/ShaderBuffers/TextureAnimationBuffer.hpp"
#include "Castor3D/Shader/ShaderBuffers/TextureConfigurationBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslCullData.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslMeshlet.hpp"
#include <Castor3D/Shader/Shaders/GlslMeshVertex.hpp>
#include "Castor3D/Shader/Shaders/GlslPassShaders.hpp"
#include "Castor3D/Shader/Shaders/GlslSubmeshShaders.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslTaskPayload.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/BillboardUbo.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"
#include "Castor3D/Shader/Ubos/LayeredLpvGridConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/LpvGridConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelDataUbo.hpp"
#include "Castor3D/Shader/Ubos/ObjectIdsUbo.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"
#include "Castor3D/Shader/Ubos/VoxelizerUbo.hpp"

#include <CastorUtils/Miscellaneous/Hash.hpp>

#include <ashespp/Buffer/Buffer.hpp>
#include <ashespp/Buffer/VertexBuffer.hpp>

#include <ShaderWriter/Source.hpp>

#include <RenderGraph/FramePass.hpp>
#include <RenderGraph/FramePassGroup.hpp>
#include <RenderGraph/GraphContext.hpp>

CU_ImplementSmartPtr( castor3d, RenderNodesPass )
CU_ImplementSmartPtr( castor3d, IsRenderPassEnabled )

namespace castor3d
{
	//*********************************************************************************************

	namespace rendndpass
	{
		static const castor::String Suffix = cuT( "/NodesPass" );

		template< typename PipelineContT >
		static auto findPipeline( PipelineFlags const & flags
			, PipelineContT & pipelines )
		{
			return std::find_if( pipelines.begin()
				, pipelines.end()
				, [&flags]( auto & lookup )
				{
					return lookup->getFlags() == flags;
				} );
		}

		static size_t makeHash( PipelineFlags const & flags )
		{
			auto result = size_t( getRenderNodeType( flags.m_programFlags ) );
			castor::hashCombine( result, size_t( flags.m_sceneFlags ) );
			castor::hashCombine( result, flags.pass.hasDeferredDiffuseLightingFlag );

			if ( flags.submeshData )
			{
				castor::hashCombinePtr( result, *flags.submeshData );
			}

			return result;
		}

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

		static crg::ru::Config buildRuConfig( crg::ru::Config const & config
			, crg::ImageViewIdArray const & targetImage
			, crg::ImageViewIdArray const & targetDepth
			, RenderQueue const & queue )
		{
			crg::ru::Config result{ std::max( 1u, uint32_t( std::max( targetImage.size(), targetDepth.size() ) ) )
				, config.resettable
				, config.prePassActions
				, config.postPassActions
				, config.implicitActions };
			queue.fillConfig( result );
			return result;
		}
	}

	//*********************************************************************************************

	RenderNodesPass::RenderNodesPass( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, castor::String const & typeName
		, crg::ImageViewIdArray targetImage
		, crg::ImageViewIdArray targetDepth
		, RenderNodesPassDesc const & desc )
		: castor::OwnedBy< Engine >{ *device.renderSystem.getEngine() }
		, castor::Named{ castor::string::stringCast< castor::xchar >( pass.getFullName() ) }
		, SceneCullerHolder{ &desc.m_culler }
		, RenderQueueHolder{ castor::makeUnique< RenderQueue >( *this, device, desc.m_culler, typeName, desc.m_meshShading, desc.m_ignored ) }
		, crg::RenderPass{ pass
			, context
			, graph
			, { [this]( uint32_t ){ doSubInitialise(); }
				, [this]( crg::RecordContext &, VkCommandBuffer cb, uint32_t ){ doSubRecordInto( cb ); }
				, GetSubpassContentsCallback( [](){ return VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS; } )
				, GetPassIndexCallback( [](){ return 0u; } )
				, IsEnabledCallback( [this](){ return isPassEnabled(); } ) }
			, makeExtent2D( desc.m_size )
			, rendndpass::buildRuConfig( desc.m_ruConfig
				, targetImage
				, targetDepth
				, getRenderQueue() ) }
		, m_device{ device }
		, m_renderSystem{ m_device.renderSystem }
		, m_cameraUbo{ desc.m_cameraUbo }
		, m_targetImage{ std::move( targetImage ) }
		, m_targetDepth{ std::move( targetDepth ) }
		, m_typeName{ typeName }
		, m_typeID{ getEngine()->getRenderPassTypeID( m_typeName ) }
		, m_filters{ desc.m_filters }
		, m_category{ pass.group.getFullName() }
		, m_size{ desc.m_size.width, desc.m_size.height }
		, m_oit{ desc.m_oit }
		, m_forceTwoSided{ desc.m_forceTwoSided }
		, m_safeBand{ desc.m_safeBand }
		, m_meshShading{ desc.m_meshShading && device.hasMeshAndTaskShaders() }
		, m_sceneUbo{ desc.m_sceneUbo }
		, m_index{ desc.m_index }
		, m_handleStatic{ desc.m_handleStatic }
		, m_componentsMask{ desc.m_componentModeFlags }
		, m_allowClusteredLighting{ desc.m_allowClusteredLighting }
		, m_deferredLightingFilter{ desc.m_deferredLightingFilter }
		, m_parallaxOcclusionFilter{ desc.m_parallaxOcclusionFilter }
	{
	}

	RenderNodesPass::~RenderNodesPass()noexcept
	{
		getRenderQueue().cleanup();
		m_backPipelines.clear();
		m_frontPipelines.clear();
	}

	void RenderNodesPass::setIgnoredNode( SceneNode const & node )
	{
		getRenderQueue().setIgnoredNode( node );
	}

	void RenderNodesPass::countNodes( RenderInfo & info )const noexcept
	{
		auto & visible = getVisibleCounts();
		info.visible.objectCount += visible.objectCount;
		info.visible.faceCount += visible.faceCount;
		info.visible.vertexCount += visible.vertexCount;
		info.visible.billboardCount += visible.billboardCount;
		info.drawCalls += getDrawCallsCount();
	}

	void RenderNodesPass::update( CpuUpdater & updater )
	{
		updater.queues->emplace_back( getRenderQueue() );
		doUpdateUbos( updater );
		m_isDirty = false;
	}

	void RenderNodesPass::getSubmeshShaderSource( PipelineFlags const & flags
		, ast::ShaderBuilder & builder )const
	{
		doGetSubmeshShaderSource( flags, builder );
	}

	void RenderNodesPass::getPixelShaderSource( PipelineFlags const & flags
		, ast::ShaderBuilder & builder )const
	{
		doGetPixelShaderSource( flags, builder );
	}

	void RenderNodesPass::forceAdjustFlags( PipelineFlags & flags )const
	{
		doUpdateFlags( flags );
		flags.pass = adjustFlags( flags.pass );
		flags.textures = adjustFlags( flags.textures );
		flags.m_shaderFlags = getShaderFlags();
		flags.isStatic = filtersNonStatic();
	}

	SubmeshComponentCombine RenderNodesPass::adjustFlags( SubmeshComponentCombine const & submeshCombine )const
	{
		auto combine = doAdjustSubmeshComponents( submeshCombine );
		getEngine()->getSubmeshComponentsRegister().registerSubmeshComponentCombine( combine );
		return combine;
	}

	PassComponentCombine RenderNodesPass::adjustFlags( PassComponentCombine const & passCombine )const
	{
		return getEngine()->getPassComponentsRegister().filterComponentFlags( getComponentsMask()
			, passCombine );
	}

	ProgramFlags RenderNodesPass::adjustFlags( ProgramFlags flags )const
	{
		if ( !m_meshShading )
		{
			remFlag( flags, ProgramFlag::eHasMesh );
		}

		return doAdjustProgramFlags( flags );
	}

	SceneFlags RenderNodesPass::adjustFlags( SceneFlags flags )const
	{
		return doAdjustSceneFlags( flags );
	}

	TextureCombine RenderNodesPass::adjustFlags( TextureCombine const & textureCombine )const
	{
		return getEngine()->getPassComponentsRegister().filterTextureFlags( getComponentsMask()
			, textureCombine );
	}

	PipelineFlags RenderNodesPass::createPipelineFlags( PassComponentCombine const & components
		, SubmeshComponentCombine const & submeshComponents
		, BlendMode colourBlendMode
		, BlendMode alphaBlendMode
		, RenderPassTypeID renderPassTypeId
		, LightingModelID lightingModelId
		, BackgroundModelID backgroundModelId
		, VkCompareOp alphaFunc
		, VkCompareOp blendAlphaFunc
		, TextureCombine const & textures
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, VkPrimitiveTopology topology
		, bool isFrontCulled
		, uint32_t passLayerIndex
		, GpuBufferOffsetT< castor::Point4f > const & morphTargets
		, SubmeshRenderData const * submeshData )const noexcept
	{
		auto result = PipelineFlags{ adjustFlags( components )
			, adjustFlags( submeshComponents )
			, lightingModelId
			, backgroundModelId
			, colourBlendMode
			, alphaBlendMode
			, renderPassTypeId
			, programFlags
			, sceneFlags
			, getShaderFlags()
			, topology
			, 3u
			, alphaFunc
			, adjustFlags( textures )
			, submeshComponents.hasPassMaskFlag ? 0u : passLayerIndex
			, morphTargets.getOffset()
			, submeshData
			, filtersNonStatic() };

		if ( isFrontCulled )
		{
			addFlag( result.m_programFlags, ProgramFlag::eFrontCulled );
		}

		doUpdateFlags( result );

		if ( hasAny( result.pass, getEngine()->getPassComponentsRegister().getAlphaBlendingFlag() ) )
		{
			result.alphaFunc = blendAlphaFunc;
		}

		return result;
	}

	PipelineFlags RenderNodesPass::createPipelineFlags( Pass const & pass
		, TextureCombine const & textures
		, SubmeshComponentCombine const & submeshComponents
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, VkPrimitiveTopology topology
		, bool isFrontCulled
		, GpuBufferOffsetT< castor::Point4f > const & morphTargets
		, SubmeshRenderData const * submeshData )const noexcept
	{
		return createPipelineFlags( pass.getPassFlags()
			, submeshComponents
			, pass.getColourBlendMode()
			, pass.getAlphaBlendMode()
			, ( pass.getRenderPassInfo()
				? pass.getRenderPassInfo()->id
				: RenderPassTypeID{} )
			, pass.getLightingModelId()
			, getScene().getBackgroundModelId()
			, pass.getAlphaFunc()
			, pass.getBlendAlphaFunc()
			, textures
			, programFlags
			, sceneFlags
			, topology
			, isFrontCulled
			, pass.getIndex()
			, morphTargets
			, submeshData );
	}

	PipelineAndID RenderNodesPass::prepareBackPipeline( PipelineFlags const & pipelineFlags
		, ashes::PipelineVertexInputStateCreateInfoCRefArray const & vertexLayouts
		, ashes::DescriptorSetLayout const * meshletDescriptorLayout )
	{
		return doPreparePipeline( vertexLayouts
			, meshletDescriptorLayout
			, pipelineFlags
			, VK_CULL_MODE_BACK_BIT );
	}

	PipelineAndID RenderNodesPass::prepareFrontPipeline( PipelineFlags const & pipelineFlags
		, ashes::PipelineVertexInputStateCreateInfoCRefArray const & vertexLayouts
		, ashes::DescriptorSetLayout const * meshletDescriptorLayout )
	{
		return doPreparePipeline( vertexLayouts
			, meshletDescriptorLayout
			, pipelineFlags
			, VK_CULL_MODE_FRONT_BIT );
	}

	void RenderNodesPass::cleanupPipelines()
	{
		for ( auto const & pipeline : m_backPipelines )
		{
			pipeline->cleanup();
		}

		for ( auto const & pipeline : m_frontPipelines )
		{
			pipeline->cleanup();
		}
	}

	ashes::PipelineColorBlendStateCreateInfo RenderNodesPass::createBlendState( BlendMode colourBlendMode
		, BlendMode alphaBlendMode
		, uint32_t attachesCount )
	{
		VkPipelineColorBlendAttachmentState attach{};

		switch ( colourBlendMode )
		{
		case BlendMode::eNoBlend:
			attach.colorBlendOp = VK_BLEND_OP_ADD;
			attach.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
			attach.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
			attach.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
			attach.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
			break;

		case BlendMode::eAdditive:
			attach.blendEnable = VK_TRUE;
			attach.colorBlendOp = VK_BLEND_OP_ADD;
			attach.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
			attach.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
			attach.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
			attach.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
			break;

		case BlendMode::eMultiplicative:
			attach.blendEnable = VK_TRUE;
			attach.colorBlendOp = VK_BLEND_OP_ADD;
			attach.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
			attach.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
			attach.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
			attach.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			break;

		default:
			attach.blendEnable = VK_TRUE;
			attach.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_COLOR;
			attach.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
			attach.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
			attach.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			break;
		}

		if ( colourBlendMode == BlendMode::eNoBlend )
		{
			switch ( alphaBlendMode )
			{
			case BlendMode::eNoBlend:
				attach.alphaBlendOp = VK_BLEND_OP_ADD;
				attach.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
				attach.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
				break;

			case BlendMode::eAdditive:
				attach.blendEnable = VK_TRUE;
				attach.alphaBlendOp = VK_BLEND_OP_ADD;
				attach.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
				attach.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
				break;

			case BlendMode::eMultiplicative:
				attach.blendEnable = VK_TRUE;
				attach.alphaBlendOp = VK_BLEND_OP_ADD;
				attach.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
				attach.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
				attach.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
				attach.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
				break;

			default:
				attach.blendEnable = VK_TRUE;
				attach.alphaBlendOp = VK_BLEND_OP_ADD;
				attach.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
				attach.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
				attach.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
				attach.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
				break;
			}
		}

		attach.colorWriteMask = defaultColorWriteMask;

		return ashes::PipelineColorBlendStateCreateInfo
		{
			0u,
			VK_FALSE,
			VK_LOGIC_OP_COPY,
			ashes::VkPipelineColorBlendAttachmentStateArray{ size_t( attachesCount ), attach },
		};
	}

	void RenderNodesPass::addShadowBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings
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

	void RenderNodesPass::addShadowBindings( SceneFlags const & sceneFlags
		, ashes::VkDescriptorSetLayoutBindingArray & bindings
		, VkShaderStageFlags shaderStages
		, uint32_t & index )
	{
		bool hasShadows{};

		for ( uint32_t j = 0u; j < uint32_t( LightType::eCount ); ++j )
		{
			if ( checkFlag( sceneFlags, SceneFlag( uint8_t( SceneFlag::eShadowBegin ) << j ) ) )
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

	void RenderNodesPass::addBackgroundBindings( SceneBackground const & background
		, ashes::VkDescriptorSetLayoutBindingArray & bindings
		, VkShaderStageFlags shaderStages
		, uint32_t & index )
	{
		background.addBindings( bindings, shaderStages, index );
	}

	void RenderNodesPass::addGIBindings( SceneFlags sceneFlags
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
			else if ( indirectLighting.llpvConfigUbo )
			{
				index += 1u; // LLPV: UBO
				index += 3u; // LLPV: Accum1R + Accum1G + Accum1B.
				index += 3u; // LLPV: Accum2R + Accum2G + Accum2B.
				index += 3u; // LLPV: Accum3R + Accum3G + Accum3B.
			}
		}
	}

	void RenderNodesPass::addClusteredLightingBindings( FrustumClusters const & frustumClusters
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

	void RenderNodesPass::addShadowDescriptor( RenderSystem const & renderSystem
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
				bindTexture( graph
					, result[SmTexture::eLinearDepth].sampledViewId
					, *result[SmTexture::eVariance].sampler
					, descriptorWrites
					, index );
				bindTexture( graph
					, result[SmTexture::eLinearDepth].sampledViewId
					, *result[SmTexture::eLinearDepth].sampler // Compare sampler
					, descriptorWrites
					, index );
				bindTexture( graph
					, result[SmTexture::eVariance].sampledViewId
					, *result[SmTexture::eVariance].sampler
					, descriptorWrites
					, index );
			}
		}

		shadowBuffer.addBinding( descriptorWrites
			, index );
		bindBuffer( renderSystem.getRandomStorage().getBuffer()
			, descriptorWrites
			, index );
#endif
	}

	void RenderNodesPass::addShadowDescriptor( RenderSystem const & renderSystem
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
			if ( checkFlag( sceneFlags, SceneFlag( uint8_t( SceneFlag::eShadowBegin ) << i ) ) )
			{
				for ( auto const & [shadowMapRef, _] : shadowMaps[i] )
				{
					auto const & result = shadowMapRef.get().getShadowPassResult( false );
					bindTexture( graph
						, result[SmTexture::eLinearDepth].sampledViewId
						, *result[SmTexture::eVariance].sampler
						, descriptorWrites
						, index );
					bindTexture( graph
						, result[SmTexture::eLinearDepth].sampledViewId
						, *result[SmTexture::eLinearDepth].sampler // Compare sampler
						, descriptorWrites
						, index );
					bindTexture( graph
						, result[SmTexture::eVariance].sampledViewId
						, *result[SmTexture::eVariance].sampler
						, descriptorWrites
						, index );
					hasShadows = true;
				}
			}
		}

		if ( hasShadows )
		{
			shadowBuffer.addBinding( descriptorWrites
				, index );
			bindBuffer( renderSystem.getRandomStorage().getBuffer()
				, descriptorWrites
				, index );
		}
#endif
	}

	void RenderNodesPass::addBackgroundDescriptor( SceneBackground const & background
		, ashes::WriteDescriptorSetArray & descriptorWrites
		, crg::ImageViewIdArray const & targetImage
		, uint32_t & index )
	{
		background.addDescriptors( descriptorWrites
			, targetImage
			, index );
	}

	void RenderNodesPass::addGIDescriptor( SceneFlags sceneFlags
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
			indirectLighting.vctConfigUbo->addDescriptorWrite( descriptorWrites
				, index );
			bindTexture( indirectLighting.vctFirstBounce->wholeView
				, *indirectLighting.vctFirstBounce->sampler
				, descriptorWrites
				, index );
			bindTexture( indirectLighting.vctSecondaryBounce->wholeView
				, *indirectLighting.vctSecondaryBounce->sampler
				, descriptorWrites
				, index );
		}
		else
		{
			if ( indirectLighting.vctConfigUbo )
			{
				index += 3u; // VCT: UBO + FirstBounce + SecondBounce.
			}

			if ( checkFlag( sceneFlags, SceneFlag::eLpvGI ) )
			{
				CU_Require( indirectLighting.lpvConfigUbo );
				CU_Require( indirectLighting.lpvResult );
				indirectLighting.lpvConfigUbo->addDescriptorWrite( descriptorWrites
					, index );
				auto const & lpv = *indirectLighting.lpvResult;
				bindTexture( lpv[LpvTexture::eR].wholeView
					, *lpv[LpvTexture::eR].sampler
					, descriptorWrites
					, index );
				bindTexture( lpv[LpvTexture::eG].wholeView
					, *lpv[LpvTexture::eG].sampler
					, descriptorWrites
					, index );
				bindTexture( lpv[LpvTexture::eB].wholeView
					, *lpv[LpvTexture::eB].sampler
					, descriptorWrites
					, index );
			}
			else if ( indirectLighting.lpvConfigUbo )
			{
				index += 4u; // LPV: UBO + AccumR + AccumG + AccumB.
			}

			if ( checkFlag( sceneFlags, SceneFlag::eLayeredLpvGI ) )
			{
				CU_Require( indirectLighting.llpvConfigUbo );
				CU_Require( indirectLighting.llpvResult );
				indirectLighting.llpvConfigUbo->addDescriptorWrite( descriptorWrites
					, index );

				for ( auto const & plpv : *indirectLighting.llpvResult )
				{
					auto const & lpv = *plpv;
					bindTexture( lpv[LpvTexture::eR].wholeView
						, *lpv[LpvTexture::eR].sampler
						, descriptorWrites
						, index );
					bindTexture( lpv[LpvTexture::eG].wholeView
						, *lpv[LpvTexture::eG].sampler
						, descriptorWrites
						, index );
					bindTexture( lpv[LpvTexture::eB].wholeView
						, *lpv[LpvTexture::eB].sampler
						, descriptorWrites
						, index );
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

	void RenderNodesPass::addClusteredLightingDescriptor( FrustumClusters const & frustumClusters
		, ashes::WriteDescriptorSetArray & descriptorWrites
		, uint32_t & index )
	{
		frustumClusters.getClustersUbo().addDescriptorWrite( descriptorWrites, index );
		bindBuffer( frustumClusters.getReducedLightsAABBBuffer(), descriptorWrites, index );
		bindBuffer( frustumClusters.getPointLightClusterIndexBuffer(), descriptorWrites, index );
		bindBuffer( frustumClusters.getPointLightClusterGridBuffer(), descriptorWrites, index );
		bindBuffer( frustumClusters.getSpotLightClusterIndexBuffer(), descriptorWrites, index );
		bindBuffer( frustumClusters.getSpotLightClusterGridBuffer(), descriptorWrites, index );
	}

	bool RenderNodesPass::areValidPassFlags( PassComponentCombine const & passFlags )const noexcept
	{
		if ( !passFlags.hasDeferredDiffuseLightingFlag
			&& ( m_deferredLightingFilter == DeferredLightingFilter::eDeferredOnly ) )
		{
			return false;
		}

		if ( ( passFlags.hasParallaxOcclusionMappingOneFlag || passFlags.hasParallaxOcclusionMappingRepeatFlag )
			&& ( m_parallaxOcclusionFilter == ParallaxOcclusionFilter::eDisabled ) )
		{
			return false;
		}

		if ( !( passFlags.hasParallaxOcclusionMappingOneFlag || passFlags.hasParallaxOcclusionMappingRepeatFlag )
			&& ( m_parallaxOcclusionFilter == ParallaxOcclusionFilter::eEnabled ) )
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

	bool RenderNodesPass::isPassEnabled()const noexcept
	{
		return hasNodes();
	}

	ShaderFlags RenderNodesPass::getShaderFlags()const noexcept
	{
		return ShaderFlag::eWorldSpace
			| ShaderFlag::eTangentSpace
			| ShaderFlag::eColour;
	}

	bool RenderNodesPass::areDebugTargetsEnabled()const noexcept
	{
		return getEngine()->areDebugTargetsEnabled();
	}

	bool RenderNodesPass::isValidPass( Pass const & pass )const noexcept
	{
		return doIsValidPass( pass );
	}

	bool RenderNodesPass::isValidRenderable( RenderedObject const & object )const noexcept
	{
		return doIsValidRenderable( object );
	}

	bool RenderNodesPass::isValidNode( SceneNode const & node )const noexcept
	{
		return &node != getIgnoredNode()
			&& ( !handleStatic()
				|| ( filtersStatic() && !node.isStatic() )
				|| ( filtersNonStatic() && node.isStatic() ) );
	}

	bool RenderNodesPass::allowClusteredLighting( ClustersConfig const & config )const noexcept
	{
		return m_allowClusteredLighting && config.enabled;
	}

	bool RenderNodesPass::hasNodes()const noexcept
	{
		return getRenderQueue().hasNodes();
	}

	Scene & RenderNodesPass::getScene()const noexcept
	{
		return getCuller().getScene();
	}

	SceneNode const * RenderNodesPass::getIgnoredNode()const noexcept
	{
		return getRenderQueue().getIgnoredNode();
	}

	bool RenderNodesPass::isMeshShading()const noexcept
	{
#if C3D_UseMeshShaders
		return m_meshShading;
#else
		return false;
#endif
	}

	PipelinesNodesT< SubmeshRenderNode > const & RenderNodesPass::getSubmeshNodes()const
	{
		return getRenderQueue().getRenderNodes().getSubmeshNodes();
	}

	InstantiatedPipelinesNodesT< SubmeshRenderNode > const & RenderNodesPass::getInstancedSubmeshNodes()const
	{
		return getRenderQueue().getRenderNodes().getInstancedSubmeshNodes();
	}

	PipelinesNodesT< BillboardRenderNode > const & RenderNodesPass::getBillboardNodes()const
	{
		return getRenderQueue().getRenderNodes().getBillboardNodes();
	}

	uint32_t RenderNodesPass::getMaxPipelineId()const
	{
		return getRenderQueue().getRenderNodes().getMaxPipelineId();
	}

	PipelineBufferArray const & RenderNodesPass::getPassPipelineNodes()const
	{
		return getRenderQueue().getRenderNodes().getPassPipelineNodes();
	}

	uint32_t RenderNodesPass::getPipelineNodesIndex( PipelineBaseHash const & hash
		, ashes::BufferBase const & buffer )const
	{
		return getRenderQueue().getRenderNodes().getPipelineNodesIndex( hash, buffer );
	}

	uint32_t RenderNodesPass::getDrawCallsCount()const
	{
		return getRenderQueue().getDrawCallsCount();
	}

	RenderCounts const & RenderNodesPass::getVisibleCounts()const
	{
		return getRenderQueue().getVisibleCounts();
	}

	void RenderNodesPass::initialiseAdditionalDescriptor( RenderPipeline & pipeline
		, ShadowMapLightTypeArray const & shadowMaps
		, ShadowBuffer const * shadowBuffer )
	{
		auto descLayoutIt = m_additionalDescriptors.try_emplace( rendndpass::makeHash( pipeline.getFlags() ) ).first;
		auto & descriptors = descLayoutIt->second;

		if ( !descriptors.set )
		{
			auto const & scene = getCuller().getScene();
			descriptors.set = descriptors.pool->createDescriptorSet( getName() + rendndpass::Suffix
				, RenderPipeline::eBuffers );
			auto & descriptorSet = *descriptors.set;
			ashes::WriteDescriptorSetArray descriptorWrites;
			descriptorWrites.push_back( m_cameraUbo.getDescriptorWrite( uint32_t( GlobalBuffersIdx::eCamera ) ) );

			if ( m_sceneUbo )
			{
				descriptorWrites.push_back( m_sceneUbo->getDescriptorWrite( uint32_t( GlobalBuffersIdx::eScene ) ) );
			}

			auto & nodesIds = getRenderQueue().getRenderNodes().getNodesIds();
			auto nodesIdsWrite = ashes::WriteDescriptorSet{ uint32_t( GlobalBuffersIdx::eObjectsNodeID )
				, 0u
				, 1u
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER };
			nodesIdsWrite.bufferInfo.push_back( { nodesIds.getBuffer()
				, 0u
				, nodesIds.getBuffer().getSize() } );
			descriptorWrites.push_back( nodesIdsWrite );

			auto & modelBuffer = scene.getModelBuffer();
			auto modelDataWrite = ashes::WriteDescriptorSet{ uint32_t( GlobalBuffersIdx::eModelsData )
				, 0u
				, 1u
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER };
			modelDataWrite.bufferInfo.push_back( { modelBuffer.getBuffer()
				, 0u
				, modelBuffer.getBuffer().getSize() } );
			descriptorWrites.push_back( modelDataWrite );
			auto const & matCache = getOwner()->getMaterialCache();
			descriptorWrites.push_back( matCache.getPassBuffer().getBinding( uint32_t( GlobalBuffersIdx::eMaterials ) ) );
			descriptorWrites.push_back( matCache.getSssProfileBuffer().getBinding( uint32_t( GlobalBuffersIdx::eSssProfiles ) ) );
			descriptorWrites.push_back( matCache.getTexConfigBuffer().getBinding( uint32_t( GlobalBuffersIdx::eTexConfigs ) ) );
			descriptorWrites.push_back( matCache.getTexAnimBuffer().getBinding( uint32_t( GlobalBuffersIdx::eTexAnims ) ) );

			if ( pipeline.getFlags().isBillboard() )
			{
				auto & billboardDatas = scene.getBillboardsBuffer();
				auto write = ashes::WriteDescriptorSet{ uint32_t( GlobalBuffersIdx::eBillboardsData )
					, 0u
					, 1u
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER };
				write.bufferInfo.push_back( { billboardDatas.getBuffer()
					, 0u
					, billboardDatas.getBuffer().getSize() } );
				descriptorWrites.push_back( write );
			}

			auto index = uint32_t( GlobalBuffersIdx::eCount );

			if ( auto submeshData = pipeline.getFlags().submeshData )
			{
				submeshData->fillDescriptor( pipeline.getFlags(), descriptorWrites, index );
			}

			doFillAdditionalDescriptor( pipeline.getFlags()
				, descriptorWrites
				, shadowMaps
				, shadowBuffer );
			descriptorSet.setBindings( descriptorWrites );
			descriptorSet.update();
		}

		pipeline.setAdditionalDescriptorSet( *descriptors.set );
	}

	void RenderNodesPass::doSubInitialise()
	{
		getRenderQueue().invalidate();
	}

	void RenderNodesPass::doSubRecordInto( VkCommandBuffer commandBuffer )
	{
		VkCommandBuffer secondary = getRenderQueue().initCommandBuffer();
		m_context.vkCmdExecuteCommands( commandBuffer
			, 1u
			, &secondary );
	}

	void RenderNodesPass::doUpdateUbos( CpuUpdater & updater )
	{
	}

	void RenderNodesPass::doAddShadowBindings( Scene const & scene
		, ashes::VkDescriptorSetLayoutBindingArray & bindings
		, uint32_t & index )const
	{
		if ( scene.hasShadows() )
		{
			addShadowBindings( doAdjustSceneFlags( scene.getFlags() )
				, bindings
				, VK_SHADER_STAGE_FRAGMENT_BIT
				, index );
		}
	}

	void RenderNodesPass::doAddBackgroundBindings( Scene const & scene
		, ashes::VkDescriptorSetLayoutBindingArray & bindings
		, uint32_t & index )const
	{
		if ( auto background = scene.getBackground() )
		{
			addBackgroundBindings( *background
				, bindings
				, VK_SHADER_STAGE_FRAGMENT_BIT
				, index );
		}
	}

	void RenderNodesPass::doAddClusteredLightingBindings( RenderTarget const & target
		, ashes::VkDescriptorSetLayoutBindingArray & bindings
		, uint32_t & index )const
	{
		if (target.getFrustumClusters()
			&& allowClusteredLighting( target.getFrustumClusters()->getConfig() ) )
		{
			addClusteredLightingBindings( *target.getFrustumClusters()
				, bindings
				, VK_SHADER_STAGE_FRAGMENT_BIT
				, index );
		}
	}

	void RenderNodesPass::doAddShadowDescriptor( Scene const & scene
		, ashes::WriteDescriptorSetArray & descriptorWrites
		, ShadowMapLightTypeArray const & shadowMaps
		, ShadowBuffer const * shadowBuffer
		, uint32_t & index )const
	{
		if ( shadowBuffer )
		{
			addShadowDescriptor( *getEngine()->getRenderSystem()
				, m_graph
				, doAdjustSceneFlags( scene.getFlags() )
				, descriptorWrites
				, shadowMaps
				, *shadowBuffer
				, index );
		}
	}

	void RenderNodesPass::doAddBackgroundDescriptor( Scene const & scene
		, ashes::WriteDescriptorSetArray & descriptorWrites
		, crg::ImageViewIdArray const & targetImage
		, uint32_t & index )const
	{
		if ( auto background = scene.getBackground() )
		{
			addBackgroundDescriptor( *background
				, descriptorWrites
				, targetImage
				, index );
		}
	}

	void RenderNodesPass::doAddClusteredLightingDescriptor( RenderTarget const & target
		, ashes::WriteDescriptorSetArray & descriptorWrites
		, uint32_t & index )const
	{
		if ( target.getFrustumClusters()
			&& allowClusteredLighting( target.getFrustumClusters()->getConfig() ) )
		{
			addClusteredLightingDescriptor( *target.getFrustumClusters()
				, descriptorWrites
				, index );
		}
	}

	bool RenderNodesPass::doIsValidPass( Pass const & pass )const noexcept
	{
		return ( pass.getRenderPassInfo() == nullptr || pass.getRenderPassInfo()->create == nullptr )
			&& areValidPassFlags( pass.getPassFlags() );
	}

	bool RenderNodesPass::doIsValidRenderable( RenderedObject const & object )const noexcept
	{
		return true;
	}
	
	SubmeshComponentCombine RenderNodesPass::doAdjustSubmeshComponents( SubmeshComponentCombine submeshCombine )const
	{
		if ( !isMeshShading() )
		{
			auto const & components = getEngine()->getSubmeshComponentsRegister();
			remFlags( submeshCombine, components.getMeshletFlag() );
		}

		return submeshCombine;
	}

	ProgramFlags RenderNodesPass::doAdjustProgramFlags( ProgramFlags flags )const
	{
		return flags;
	}

	SceneFlags RenderNodesPass::doAdjustSceneFlags( SceneFlags flags )const
	{
		return flags;
	}

	ShaderProgramRPtr RenderNodesPass::doGetProgram( PipelineFlags const & flags )
	{
		return getEngine()->getShaderProgramCache().getAutomaticProgram( *this, flags );
	}

	void RenderNodesPass::doAccept( castor3d::RenderTechniqueVisitor & visitor )
	{
		if ( visitor.getFlags().renderPassType == m_typeID
			&& visitor.config.allowProgramsVisit )
		{
			auto flags = visitor.getFlags();
			forceAdjustFlags( flags );
			auto shaderProgram = doGetProgram( flags );
			visitor.visit( shaderProgram->getModule() );
		}
	}

	void RenderNodesPass::doUpdateFlags( PipelineFlags & flags )const
	{
		flags.submesh = adjustFlags( flags.submesh );
		flags.m_programFlags = adjustFlags( flags.m_programFlags );
		flags.m_sceneFlags = adjustFlags( flags.m_sceneFlags );
		auto & components = getEngine()->getSubmeshComponentsRegister();

		if ( checkFlag( m_filters, RenderFilter::eAlphaTest ) )
		{
			flags.alphaFunc = VK_COMPARE_OP_ALWAYS;
		}

		if ( checkFlag( m_filters, RenderFilter::eAlphaBlend )
			&& checkFlag( m_filters, RenderFilter::eTransmission ) )
		{
			flags.alphaBlendMode = BlendMode::eNoBlend;
		}

		if ( checkFlag( m_filters, RenderFilter::eAlphaBlend )
			&& checkFlag( m_filters, RenderFilter::eAlphaTest )
			&& checkFlag( m_filters, RenderFilter::eTransmission ) )
		{
			remFlag( flags.m_shaderFlags, ShaderFlag::eOpacity );
		}
		else
		{
			addFlag( flags.m_shaderFlags, ShaderFlag::eOpacity );
		}

		if ( flags.textures.configCount == 0
			&& !flags.forceTexCoords() )
		{
			remFlags( flags.submesh, components.getTexcoord0Flag() );
			remFlags( flags.submesh, components.getTexcoord1Flag() );
			remFlags( flags.submesh, components.getTexcoord2Flag() );
			remFlags( flags.submesh, components.getTexcoord3Flag() );
			flags.submesh.hasTexcoord0Flag = false;
			flags.submesh.hasTexcoord1Flag = false;
			flags.submesh.hasTexcoord2Flag = false;
			flags.submesh.hasTexcoord3Flag = false;
		}

		if ( flags.submesh.hasPassMaskFlag )
		{
			flags.alphaFunc = VK_COMPARE_OP_GREATER;
		}

		if ( !flags.submesh.hasColourFlag )
		{
			remFlag( flags.m_shaderFlags, ShaderFlag::eColour );
		}

		components.registerSubmeshComponentCombine( flags.submesh );
		doAdjustFlags( flags );
	}

	ashes::VkDescriptorSetLayoutBindingArray RenderNodesPass::doCreateAdditionalBindings( PipelineFlags const & flags )const
	{
		VkShaderStageFlags stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		if ( flags.usesMesh() )
		{
			stageFlags |= VK_SHADER_STAGE_MESH_BIT_NV
				| VK_SHADER_STAGE_TASK_BIT_NV;
		}
		else
		{
			stageFlags |= VK_SHADER_STAGE_ALL_GRAPHICS;
		}

		ashes::VkDescriptorSetLayoutBindingArray addBindings;
		addBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( GlobalBuffersIdx::eCamera )
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, stageFlags ) );

		if ( m_sceneUbo )
		{
			addBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( GlobalBuffersIdx::eScene )
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, stageFlags ) );
		}

		addBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( GlobalBuffersIdx::eObjectsNodeID )
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			, stageFlags ) );
		addBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( GlobalBuffersIdx::eModelsData )
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			, stageFlags ) );
		auto const & matCache = getOwner()->getMaterialCache();
		addBindings.emplace_back( matCache.getPassBuffer().createLayoutBinding( uint32_t( GlobalBuffersIdx::eMaterials )
			, stageFlags ) );
		addBindings.emplace_back( matCache.getSssProfileBuffer().createLayoutBinding( uint32_t( GlobalBuffersIdx::eSssProfiles )
			, stageFlags ) );
		addBindings.emplace_back( matCache.getTexConfigBuffer().createLayoutBinding( uint32_t( GlobalBuffersIdx::eTexConfigs )
			, stageFlags ) );
		addBindings.emplace_back( matCache.getTexAnimBuffer().createLayoutBinding( uint32_t( GlobalBuffersIdx::eTexAnims )
			, stageFlags ) );

		if ( flags.isBillboard() )
		{
			addBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( GlobalBuffersIdx::eBillboardsData )
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, stageFlags ) );
		}

		auto index = uint32_t( GlobalBuffersIdx::eCount );

		if ( auto submeshData = flags.submeshData )
		{
			submeshData->fillBindings( flags, addBindings, index );
		}

		doFillAdditionalBindings( flags, addBindings );
		return addBindings;
	}

	std::vector< RenderPipelineUPtr > & RenderNodesPass::doGetFrontPipelines()
	{
		return m_frontPipelines;
	}

	std::vector< RenderPipelineUPtr > & RenderNodesPass::doGetBackPipelines()
	{
		return m_backPipelines;
	}

	std::vector< RenderPipelineUPtr > const & RenderNodesPass::doGetFrontPipelines()const
	{
		return m_frontPipelines;
	}

	std::vector< RenderPipelineUPtr > const & RenderNodesPass::doGetBackPipelines()const
	{
		return m_backPipelines;
	}

	PipelineAndID RenderNodesPass::doPreparePipeline( ashes::PipelineVertexInputStateCreateInfoCRefArray const & vertexLayouts
		, ashes::DescriptorSetLayout const * meshletDescriptorLayout
		, PipelineFlags const & flags
		, VkCullModeFlags cullMode )
	{
		auto & renderSystem = *getEngine()->getRenderSystem();
		auto & device = renderSystem.getRenderDevice();
		RenderPipeline * result{};
		uint16_t id{};
		CU_Require( areValidPassFlags( flags.pass ) );

		if ( !flags.isBillboard()
			|| !flags.writeShadowMap() )
		{
			auto & pipelines = castor::checkFlag( cullMode, VK_CULL_MODE_FRONT_BIT )
				? doGetFrontPipelines()
				: doGetBackPipelines();
			auto it = rendndpass::findPipeline( flags, pipelines );

			if ( it == pipelines.end() )
			{
				auto pipeline = castor::makeUnique< RenderPipeline >( *this
					, renderSystem
					, doCreateDepthStencilState( flags )
					, doCreateRasterizationState( flags, cullMode )
					, doCreateBlendState( flags )
					, doCreateMultisampleState( flags )
					, doGetProgram( flags )
					, flags );
				pipeline->setViewport( makeViewport( m_size ) );

				if ( !flags.writePicking() )
				{
					pipeline->setScissor( makeScissor( m_size ) );
				}

				auto addDescLayoutIt = m_additionalDescriptors.try_emplace( rendndpass::makeHash( flags ) ).first;
				auto & addDescriptors = addDescLayoutIt->second;

				if ( !addDescriptors.layout )
				{
					auto bindings = doCreateAdditionalBindings( flags );
					addDescriptors.layout = device->createDescriptorSetLayout( getName() + rendndpass::Suffix
						, std::move( bindings ) );
					addDescriptors.pool = addDescriptors.layout->createPool( 1u );
				}

				pipeline->setAdditionalDescriptorSetLayout( *addDescriptors.layout );

				if ( flags.usesMesh()
					&& meshletDescriptorLayout )
				{
					pipeline->setMeshletDescriptorSetLayout( *meshletDescriptorLayout );
					pipeline->setPushConstantRanges( { { VK_SHADER_STAGE_MESH_BIT_NV | VK_SHADER_STAGE_TASK_BIT_NV | VK_SHADER_STAGE_FRAGMENT_BIT
						, 0u
						, sizeof( MeshletDrawConstants ) } } );
				}
				else
				{
					pipeline->setVertexLayouts( vertexLayouts );
					pipeline->setPushConstantRanges( { { VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
						, 0u
						, sizeof( DrawConstants ) } } );
				}

				pipeline->initialise( device
					, getRenderPass( 0u ) );
				pipelines.emplace_back( std::move( pipeline ) );
				it = std::next( pipelines.begin()
					, ptrdiff_t( pipelines.size() - 1u ) );
			}
			else
			{
				( *it )->initialise( device, getRenderPass( 0u ) );
			}

			id = uint16_t( std::distance( pipelines.begin(), it ) );
			result = it->get();
		}

		return { result, id };
	}

	ashes::PipelineRasterizationStateCreateInfo RenderNodesPass::doCreateRasterizationState( PipelineFlags const & flags
		, VkCullModeFlags cullMode )const
	{
		return ashes::PipelineRasterizationStateCreateInfo{ 0u, VK_FALSE, VK_FALSE, VK_POLYGON_MODE_FILL, cullMode };
	}

	void RenderNodesPass::doAdjustFlags( PipelineFlags & flags )const
	{
	}

	void RenderNodesPass::doGetSubmeshShaderSource( PipelineFlags const & flags
		, ast::ShaderBuilder & builder )const
	{
		getEngine()->getSubmeshComponentsRegister().getSubmeshRenderShader( flags
			, getComponentsMask()
			, builder );
	}
}
