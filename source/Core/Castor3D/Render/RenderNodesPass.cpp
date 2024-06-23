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
		: NodesPass{ device, pass.group.getFullName(), typeName, pass.getFullName(), targetImage, targetDepth, desc.base() }
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
			, makeExtent2D( desc.base().m_size )
			, rendndpass::buildRuConfig( desc.m_ruConfig
				, targetImage
				, targetDepth
				, getRenderQueue() ) }
		, m_oit{ desc.m_oit }
		, m_forceTwoSided{ desc.m_forceTwoSided }
		, m_meshShading{ desc.m_meshShading && device.hasMeshAndTaskShaders() }
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

	ProgramFlags RenderNodesPass::adjustFlags( ProgramFlags flags )const
	{
		if ( !m_meshShading )
		{
			remFlag( flags, ProgramFlag::eHasMesh );
		}

		return doAdjustProgramFlags( flags );
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
			descriptors.set = descriptors.pool->createDescriptorSet( castor::toUtf8( getName() + rendndpass::Suffix )
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

	void RenderNodesPass::doSubInitialise()const
	{
		getRenderQueue().invalidate();
	}

	void RenderNodesPass::doSubRecordInto( VkCommandBuffer commandBuffer )const
	{
		VkCommandBuffer secondary = getRenderQueue().initCommandBuffer();
		m_context.vkCmdExecuteCommands( commandBuffer
			, 1u
			, &secondary );
	}

	void RenderNodesPass::doUpdate( CpuUpdater & updater )
	{
		updater.queues->emplace_back( getRenderQueue() );
		doUpdateUbos( updater );
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

	ProgramFlags RenderNodesPass::doAdjustProgramFlags( ProgramFlags flags )const
	{
		return flags;
	}

	ShaderProgramRPtr RenderNodesPass::doGetProgram( PipelineFlags const & flags )
	{
		return getEngine()->getShaderProgramCache().getAutomaticProgram( *this, flags );
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

	SceneFlags RenderNodesPass::doAdjustSceneFlags( SceneFlags flags )const
	{
		return flags;
	}

	bool RenderNodesPass::doAreValidPassFlags( PassComponentCombine const & passFlags )const noexcept
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

		return true;
	}

	bool RenderNodesPass::doIsValidNode( SceneNode const & node )const noexcept
	{
		return &node != getIgnoredNode();
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

	castor::Vector< RenderPipelineUPtr > & RenderNodesPass::doGetFrontPipelines()
	{
		return m_frontPipelines;
	}

	castor::Vector< RenderPipelineUPtr > & RenderNodesPass::doGetBackPipelines()
	{
		return m_backPipelines;
	}

	castor::Vector< RenderPipelineUPtr > const & RenderNodesPass::doGetFrontPipelines()const
	{
		return m_frontPipelines;
	}

	castor::Vector< RenderPipelineUPtr > const & RenderNodesPass::doGetBackPipelines()const
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
					addDescriptors.layout = device->createDescriptorSetLayout( castor::toUtf8( getName() + rendndpass::Suffix )
						, castor::move( bindings ) );
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
				pipelines.emplace_back( castor::move( pipeline ) );
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
