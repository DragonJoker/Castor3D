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
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderQueue.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
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
#include "Castor3D/Shader/Shaders/GlslPassShaders.hpp"
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

#define SDW_PreferredMeshShadingExtension SDW_MeshShadingNV

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/MeshWriter.hpp>
#include <ShaderWriter/TaskWriter.hpp>

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
			castor::hashCombine( result, flags.components.hasDeferredDiffuseLightingFlag );
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
		, crg::RenderPass{ pass
			, context
			, graph
			, { [this]( uint32_t index ){ doSubInitialise(); }
				, [this]( crg::RecordContext & ctx, VkCommandBuffer cb, uint32_t i ){ doSubRecordInto( ctx, cb, i ); }
				, GetSubpassContentsCallback( [](){ return VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS; } )
				, GetPassIndexCallback( [](){ return 0u; } )
				, IsEnabledCallback( [this](){ return isPassEnabled(); } ) }
			, makeExtent2D( desc.m_size )
			, crg::ru::Config{ std::max( 1u, uint32_t( std::max( targetImage.size(), targetDepth.size() ) ) )
				, desc.m_ruConfig.resettable
				, std::move( desc.m_ruConfig.actions ) } }
		, castor::Named{ castor::string::stringCast< castor::xchar >( pass.getFullName() ) }
		, m_device{ device }
		, m_renderSystem{ m_device.renderSystem }
		, m_cameraUbo{ desc.m_cameraUbo }
		, m_culler{ desc.m_culler }
		, m_targetImage{ std::move( targetImage ) }
		, m_targetDepth{ std::move( targetDepth ) }
		, m_typeName{ typeName }
		, m_typeID{ getEngine()->getRenderPassTypeID( m_typeName ) }
		, m_filters{ desc.m_filters }
		, m_renderQueue{ castor::makeUnique< RenderQueue >( *this, desc.m_ignored ) }
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

	RenderNodesPass::~RenderNodesPass()
	{
		m_renderQueue->cleanup();
		m_backPipelines.clear();
		m_frontPipelines.clear();
	}

	void RenderNodesPass::setIgnoredNode( SceneNode const & node )
	{
		m_renderQueue->setIgnoredNode( node );
	}

	void RenderNodesPass::countNodes( RenderInfo & info )const
	{
		for ( auto & node : getCuller().getSubmeshes() )
		{
			if ( node.visibleOrFrontCulled )
			{
				++info.visibleObjectsCount;
				info.visibleFaceCount += node.node->data.getFaceCount();
				info.visibleVertexCount += node.node->data.getPointsCount();
			}

			++info.totalObjectsCount;
			info.totalFaceCount += node.node->data.getFaceCount();
			info.totalVertexCount += node.node->data.getPointsCount();
		}

		info.drawCalls += getDrawCallsCount();
	}

	void RenderNodesPass::update( CpuUpdater & updater )
	{
		updater.queues->emplace_back( *m_renderQueue );
		doUpdateUbos( updater );
		m_isDirty = false;
	}

	ShaderPtr RenderNodesPass::getTaskShaderSource( PipelineFlags const & flags )const
	{
		return doGetTaskShaderSource( flags );
	}

	ShaderPtr RenderNodesPass::getMeshShaderSource( PipelineFlags const & flags )const
	{
		return doGetMeshShaderSource( flags );
	}

	ShaderPtr RenderNodesPass::getVertexShaderSource( PipelineFlags const & flags )const
	{
		ShaderPtr result;

		if ( flags.isBillboard() )
		{
			result = doGetBillboardShaderSource( flags );
		}
		else
		{
			result = doGetVertexShaderSource( flags );
		}

		return result;
	}

	ShaderPtr RenderNodesPass::getHullShaderSource( PipelineFlags const & flags )const
	{
		return doGetHullShaderSource( flags );
	}

	ShaderPtr RenderNodesPass::getDomainShaderSource( PipelineFlags const & flags )const
	{
		return doGetDomainShaderSource( flags );
	}

	ShaderPtr RenderNodesPass::getGeometryShaderSource( PipelineFlags const & flags )const
	{
		return doGetGeometryShaderSource( flags );
	}

	ShaderPtr RenderNodesPass::getPixelShaderSource( PipelineFlags const & flags )const
	{
		return doGetPixelShaderSource( flags );
	}

	SubmeshFlags RenderNodesPass::adjustFlags( SubmeshFlags flags )const
	{
		return doAdjustSubmeshFlags( flags );
	}

	PassComponentCombine RenderNodesPass::adjustFlags( PassComponentCombine combine )const
	{
		return getEngine()->getPassComponentsRegister().filterComponentFlags( getComponentsMask()
			, combine );
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

	TextureCombine RenderNodesPass::adjustFlags( TextureCombine combine )const
	{
		return getEngine()->getPassComponentsRegister().filterTextureFlags( getComponentsMask()
			, combine );
	}

	PipelineFlags RenderNodesPass::createPipelineFlags( PassComponentCombine components
		, BlendMode colourBlendMode
		, BlendMode alphaBlendMode
		, RenderPassTypeID renderPassTypeId
		, LightingModelID lightingModelId
		, BackgroundModelID backgroundModelId
		, VkCompareOp alphaFunc
		, VkCompareOp blendAlphaFunc
		, TextureCombine const & textures
		, SubmeshFlags const & submeshFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, VkPrimitiveTopology topology
		, bool isFrontCulled
		, uint32_t passLayerIndex
		, GpuBufferOffsetT< castor::Point4f > const & morphTargets )const
	{
		auto result = PipelineFlags{ adjustFlags( components )
			, lightingModelId
			, backgroundModelId
			, colourBlendMode
			, alphaBlendMode
			, renderPassTypeId
			, submeshFlags
			, programFlags
			, sceneFlags
			, getShaderFlags()
			, topology
			, 3u
			, alphaFunc
			, adjustFlags( textures )
			, checkFlag( submeshFlags, SubmeshFlag::ePassMasks ) ? 0u : passLayerIndex
			, morphTargets.getOffset()
			, filtersNonStatic() };

		if ( isFrontCulled )
		{
			addFlag( result.m_programFlags, ProgramFlag::eFrontCulled );
		}

		doUpdateFlags( result );

		if ( hasAny( result.components, getEngine()->getPassComponentsRegister().getAlphaBlendingFlag() ) )
		{
			result.alphaFunc = blendAlphaFunc;
		}

		return result;
	}

	PipelineFlags RenderNodesPass::createPipelineFlags( Pass const & pass
		, TextureCombine const & textures
		, SubmeshFlags const & submeshFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, VkPrimitiveTopology topology
		, bool isFrontCulled
		, GpuBufferOffsetT< castor::Point4f > const & morphTargets )const
	{
		return createPipelineFlags( pass.getPassFlags()
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
			, submeshFlags
			, programFlags
			, sceneFlags
			, topology
			, isFrontCulled
			, pass.getIndex()
			, morphTargets );
	}

	RenderPipeline & RenderNodesPass::prepareBackPipeline( PipelineFlags pipelineFlags
		, ashes::PipelineVertexInputStateCreateInfoCRefArray const & vertexLayouts
		, ashes::DescriptorSetLayout const * meshletDescriptorLayout )
	{
		return doPreparePipeline( vertexLayouts
			, meshletDescriptorLayout
			, std::move( pipelineFlags )
			, VK_CULL_MODE_BACK_BIT );
	}

	RenderPipeline & RenderNodesPass::prepareFrontPipeline( PipelineFlags pipelineFlags
		, ashes::PipelineVertexInputStateCreateInfoCRefArray const & vertexLayouts
		, ashes::DescriptorSetLayout const * meshletDescriptorLayout )
	{
		return doPreparePipeline( vertexLayouts
			, meshletDescriptorLayout
			, std::move( pipelineFlags )
			, VK_CULL_MODE_FRONT_BIT );
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

		case BlendMode::eInterpolative:
			attach.blendEnable = VK_TRUE;
			attach.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_COLOR;
			attach.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
			attach.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
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

			case BlendMode::eInterpolative:
				attach.blendEnable = VK_TRUE;
				attach.alphaBlendOp = VK_BLEND_OP_ADD;
				attach.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
				attach.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
				attach.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
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
			bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, shaderStages ) );
			// Depth Compare
			bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, shaderStages ) );
			// Variance
			bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, shaderStages ) );
		}

		// Shadow Buffer
		bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, shaderStages ) );
		// Random Storage
		bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			, shaderStages ) );
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
				bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
					, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
					, shaderStages ) );
				// Depth Compare
				bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
					, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
					, shaderStages ) );
				// Variance
				bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
					, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
					, shaderStages ) );
				hasShadows = true;
			}
		}

		if ( hasShadows )
		{
			// Shadow Buffer
			bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, shaderStages ) );
			// Random Storage
			bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, shaderStages ) );
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
			bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, shaderStages ) );
			bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, shaderStages ) );	// c3d_mapVoxelsFirstBounce
			bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, shaderStages ) );	// c3d_mapVoxelsSecondaryBounce
		}
		else
		{
			if ( indirectLighting.vctConfigUbo )
			{
				index += 3u; // VCT: UBO + FirstBounce + SecondBounce.
			}

			if ( checkFlag( sceneFlags, SceneFlag::eLpvGI ) )
			{
				bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
					, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
					, shaderStages ) );
				bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
					, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
					, shaderStages ) );	// c3d_lpvAccumulationR
				bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
					, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
					, shaderStages ) );	// c3d_lpvAccumulationG
				bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
					, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
					, shaderStages ) );	// c3d_lpvAccumulationB
			}
			else if ( indirectLighting.lpvConfigUbo )
			{
				index += 4u; // LPV: UBO + AccumR + AccumG + AccumB.
			}

			if ( checkFlag( sceneFlags, SceneFlag::eLayeredLpvGI ) )
			{
				CU_Require( indirectLighting.llpvResult );
				bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
					, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
					, shaderStages ) );

				for ( size_t i = 0u; i < indirectLighting.llpvResult->size(); ++i )
				{
					bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
						, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
						, shaderStages ) );	// c3d_lpvAccumulationRn
					bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
						, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
						, shaderStages ) );	// c3d_lpvAccumulationGn
					bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
						, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
						, shaderStages ) );	// c3d_lpvAccumulationBn
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
		bindings.push_back( frustumClusters.getClustersUbo().createLayoutBinding( index++
			, shaderStages ) );
		bindings.push_back( makeDescriptorSetLayoutBinding( index++ // ReducedLightsAABBBuffer
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			, shaderStages ) );
		bindings.push_back( makeDescriptorSetLayoutBinding( index++ // PointLightIndexBuffer
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			, shaderStages ) );
		bindings.push_back( makeDescriptorSetLayoutBinding( index++ // PointLightClusterBuffer
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			, shaderStages ) );
		bindings.push_back( makeDescriptorSetLayoutBinding( index++ // SpotLightIndexBuffer
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			, shaderStages ) );
		bindings.push_back( makeDescriptorSetLayoutBinding( index++ // SpotLightClusterBuffer
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			, shaderStages ) );
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
			for ( auto & shadowMapRef : shadowMaps[i] )
			{
				auto & result = shadowMapRef.first.get().getShadowPassResult( false );
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

		descriptorWrites.push_back( shadowBuffer.getBinding( index++ ) );
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
				for ( auto & shadowMapRef : shadowMaps[i] )
				{
					auto & result = shadowMapRef.first.get().getShadowPassResult( false );
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
			descriptorWrites.push_back( shadowBuffer.getBinding( index++ ) );
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
			descriptorWrites.push_back( indirectLighting.vctConfigUbo->getDescriptorWrite( index++ ) );
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
				descriptorWrites.push_back( indirectLighting.lpvConfigUbo->getDescriptorWrite( index++ ) );
				auto & lpv = *indirectLighting.lpvResult;
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
				descriptorWrites.push_back( indirectLighting.llpvConfigUbo->getDescriptorWrite( index++ ) );

				for ( auto & plpv : *indirectLighting.llpvResult )
				{
					auto & lpv = *plpv;
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
		descriptorWrites.push_back( frustumClusters.getClustersUbo().getDescriptorWrite( index++ ) );
		bindBuffer( frustumClusters.getReducedLightsAABBBuffer(), descriptorWrites, index );
		bindBuffer( frustumClusters.getPointLightClusterIndexBuffer(), descriptorWrites, index );
		bindBuffer( frustumClusters.getPointLightClusterGridBuffer(), descriptorWrites, index );
		bindBuffer( frustumClusters.getSpotLightClusterIndexBuffer(), descriptorWrites, index );
		bindBuffer( frustumClusters.getSpotLightClusterGridBuffer(), descriptorWrites, index );
	}

	bool RenderNodesPass::areValidPassFlags( PassComponentCombine const & passFlags )const
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

	bool RenderNodesPass::isPassEnabled()const
	{
		return hasNodes();
	}

	ShaderFlags RenderNodesPass::getShaderFlags()const
	{
		return ShaderFlag::eWorldSpace
			| ShaderFlag::eTangentSpace
			| ShaderFlag::eColour;
	}

	bool RenderNodesPass::areDebugTargetsEnabled()const noexcept
	{
		return C3D_EnableDebugTargets != 0;
	}

	bool RenderNodesPass::isValidPass( Pass const & pass )const
	{
		return doIsValidPass( pass );
	}

	bool RenderNodesPass::isValidRenderable( RenderedObject const & object )const
	{
		return doIsValidRenderable( object );
	}

	bool RenderNodesPass::isValidNode( SceneNode const & node )const
	{
		return &node != getIgnoredNode()
			&& ( !handleStatic()
				|| ( filtersStatic() && !node.isStatic() )
				|| ( filtersNonStatic() && node.isStatic() ) );
	}

	bool RenderNodesPass::allowClusteredLighting( ClustersConfig const & config )const
	{
		return m_allowClusteredLighting && config.enabled;
	}

	bool RenderNodesPass::hasNodes()const
	{
		return m_renderQueue->hasNodes();
	}

	Scene & RenderNodesPass::getScene()const
	{
		return getCuller().getScene();
	}

	SceneNode const * RenderNodesPass::getIgnoredNode()const
	{
		return m_renderQueue->getIgnoredNode();
	}

	bool RenderNodesPass::isMeshShading()const
	{
#if C3D_UseMeshShaders
		return m_meshShading;
#else
		return false;
#endif
	}

	NodePtrByPipelineMapT< SubmeshRenderNode > const & RenderNodesPass::getSubmeshNodes()const
	{
		if ( m_renderQueue )
		{
			return m_renderQueue->getRenderNodes().getSubmeshNodes();
		}

		static NodePtrByPipelineMapT< SubmeshRenderNode > dummy;
		return dummy;
	}

	ObjectNodesPtrByPipelineMapT< SubmeshRenderNode > const & RenderNodesPass::getInstancedSubmeshNodes()const
	{
		if ( m_renderQueue )
		{
			return m_renderQueue->getRenderNodes().getInstancedSubmeshNodes();
		}

		static ObjectNodesPtrByPipelineMapT< SubmeshRenderNode > dummy;
		return dummy;
	}

	NodePtrByPipelineMapT< BillboardRenderNode > const & RenderNodesPass::getBillboardNodes()const
	{
		if ( m_renderQueue )
		{
			return m_renderQueue->getRenderNodes().getBillboardNodes();
		}

		static NodePtrByPipelineMapT< BillboardRenderNode > dummy;
		return dummy;
	}

	uint32_t RenderNodesPass::getMaxPipelineId()const
	{
		if ( m_renderQueue )
		{
			return m_renderQueue->getRenderNodes().getMaxPipelineId();
		}

		return {};
	}

	PipelineBufferArray const & RenderNodesPass::getPassPipelineNodes()const
	{
		if ( m_renderQueue )
		{
			return m_renderQueue->getRenderNodes().getPassPipelineNodes();
		}

		static PipelineBufferArray dummy;
		return dummy;
	}

	uint32_t RenderNodesPass::getPipelineNodesIndex( PipelineBaseHash const & hash
		, ashes::BufferBase const & buffer )const
	{
		if ( m_renderQueue )
		{
			return m_renderQueue->getRenderNodes().getPipelineNodesIndex( hash, buffer );
		}

		return {};
	}

	uint32_t RenderNodesPass::getDrawCallsCount()const
	{
		return m_renderQueue->getDrawCallsCount();
	}

	void RenderNodesPass::initialiseAdditionalDescriptor( RenderPipeline & pipeline
		, ShadowMapLightTypeArray const & shadowMaps
		, ShadowBuffer const * shadowBuffer
		, GpuBufferOffsetT< castor::Point4f > const & morphTargets )
	{
		auto descLayoutIt = m_additionalDescriptors.emplace( rendndpass::makeHash( pipeline.getFlags() )
			, PassDescriptors{} ).first;
		auto & descriptors = descLayoutIt->second;

		if ( !descriptors.set )
		{
			auto & scene = getCuller().getScene();
			descriptors.set = descriptors.pool->createDescriptorSet( getName() + rendndpass::Suffix
				, RenderPipeline::eBuffers );
			auto & descriptorSet = *descriptors.set;
			ashes::WriteDescriptorSetArray descriptorWrites;
			descriptorWrites.push_back( m_cameraUbo.getDescriptorWrite( uint32_t( GlobalBuffersIdx::eCamera ) ) );

			if ( m_sceneUbo )
			{
				descriptorWrites.push_back( m_sceneUbo->getDescriptorWrite( uint32_t( GlobalBuffersIdx::eScene ) ) );
			}

			auto & nodesIds = m_renderQueue->getRenderNodes().getNodesIds();
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
			auto & matCache = getOwner()->getMaterialCache();
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
		if ( m_renderQueue->needsInitialise() )
		{
			m_renderQueue->initialise();
		}
		else
		{
			m_renderQueue->invalidate();
		}

		m_renderQueue->getRenderNodes().clear();
		m_backPipelines.clear();
		m_frontPipelines.clear();
	}

	void RenderNodesPass::doSubRecordInto( crg::RecordContext & context
		, VkCommandBuffer commandBuffer
		, uint32_t index )
	{
		VkCommandBuffer secondary = m_renderQueue->initCommandBuffer();
		m_context.vkCmdExecuteCommands( commandBuffer
			, 1u
			, &secondary );
	}

	void RenderNodesPass::doUpdateUbos( CpuUpdater & updater )
	{
	}

	void RenderNodesPass::doAddShadowBindings( Scene const & scene
		, PipelineFlags const & flags
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
		, PipelineFlags const & flags
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
		, PipelineFlags const & flags
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
		, PipelineFlags const & flags
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

	bool RenderNodesPass::doIsValidPass( Pass const & pass )const
	{
		return ( pass.getRenderPassInfo() == nullptr || pass.getRenderPassInfo()->create == nullptr )
			&& areValidPassFlags( pass.getPassFlags() );
	}

	bool RenderNodesPass::doIsValidRenderable( RenderedObject const & object )const
	{
		return true;
	}

	SubmeshFlags RenderNodesPass::doAdjustSubmeshFlags( SubmeshFlags flags )const
	{
		return flags;
	}

	ProgramFlags RenderNodesPass::doAdjustProgramFlags( ProgramFlags flags )const
	{
		return flags;
	}

	SceneFlags RenderNodesPass::doAdjustSceneFlags( SceneFlags flags )const
	{
		return flags;
	}

	ShaderProgramRPtr RenderNodesPass::doGetProgram( PipelineFlags const & flags
		, VkCullModeFlags cullMode )
	{
		return getEngine()->getShaderProgramCache().getAutomaticProgram( *this, flags );
	}

	void RenderNodesPass::doUpdateFlags( PipelineFlags & flags )const
	{
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

		flags.m_submeshFlags = adjustFlags( flags.m_submeshFlags );
		flags.m_programFlags = adjustFlags( flags.m_programFlags );
		flags.m_sceneFlags = adjustFlags( flags.m_sceneFlags );

		if ( flags.textures.configCount == 0
			&& !flags.forceTexCoords() )
		{
			remFlag( flags.m_submeshFlags, SubmeshFlag::eTexcoords );
		}

		if ( checkFlag( flags.m_submeshFlags, SubmeshFlag::ePassMasks ) )
		{
			flags.alphaFunc = VK_COMPARE_OP_GREATER;
		}

		if ( !checkFlag( flags.m_submeshFlags, SubmeshFlag::eColours ) )
		{
			remFlag( flags.m_shaderFlags, ShaderFlag::eColour );
		}

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
			stageFlags |= VK_SHADER_STAGE_VERTEX_BIT;

			if ( flags.usesGeometry() )
			{
				stageFlags |= VK_SHADER_STAGE_GEOMETRY_BIT;
			}

			if ( flags.usesTessellation() )
			{
				stageFlags |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
				stageFlags |= VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
			}
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
		auto & matCache = getOwner()->getMaterialCache();
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

	RenderPipeline & RenderNodesPass::doPreparePipeline( ashes::PipelineVertexInputStateCreateInfoCRefArray const & vertexLayouts
		, ashes::DescriptorSetLayout const * meshletDescriptorLayout
		, PipelineFlags const & flags
		, VkCullModeFlags cullMode )
	{
		auto & renderSystem = *getEngine()->getRenderSystem();
		auto & device = renderSystem.getRenderDevice();
		RenderPipeline * result{};
		CU_Require( areValidPassFlags( flags.components ) );

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
					, doGetProgram( flags, cullMode )
					, flags );
				pipeline->setViewport( makeViewport( m_size ) );

				if ( !flags.writePicking() )
				{
					pipeline->setScissor( makeScissor( m_size ) );
				}

				auto addDescLayoutIt = m_additionalDescriptors.emplace( rendndpass::makeHash( flags )
					, PassDescriptors{} ).first;
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

			result = it->get();
		}

		return *result;
	}

	ashes::PipelineRasterizationStateCreateInfo RenderNodesPass::doCreateRasterizationState( PipelineFlags const & flags
		, VkCullModeFlags cullMode )const
	{
		return ashes::PipelineRasterizationStateCreateInfo{ 0u, VK_FALSE, VK_FALSE, VK_POLYGON_MODE_FILL, cullMode };
	}

	void RenderNodesPass::doAdjustFlags( PipelineFlags & flags )const
	{
	}

	ShaderPtr RenderNodesPass::doGetTaskShaderSource( PipelineFlags const & flags )const
	{
		sdw::TaskWriter writer{ &getEngine()->getShaderAllocator() };
		bool checkCones = flags.isFrontCulled()
			&& flags.hasSubmeshData( SubmeshFlag::eNormals )
			&& !flags.hasWorldPosInputs();

		C3D_Camera( writer
			, GlobalBuffersIdx::eCamera
			, RenderPipeline::eBuffers );
		C3D_ObjectIdsData( writer
			, flags
			, GlobalBuffersIdx::eObjectsNodeID
			, RenderPipeline::eBuffers );
		C3D_ModelsData( writer
			, GlobalBuffersIdx::eModelsData
			, RenderPipeline::eBuffers );
		sdw::StorageBuffer instancesBuffer{ writer
			, std::string{ "c3d_instancesBuffer" }
			, uint32_t( MeshBuffersIdx::eInstances )
			, RenderPipeline::eMeshBuffers
			, ast::type::MemoryLayout::eStd430
			, flags.enableInstantiation() };
		auto c3d_instances = instancesBuffer.declMemberArray< sdw::UVec4 >( "c3d_instances"
			, flags.enableInstantiation() );
		instancesBuffer.end();

		sdw::PushConstantBuffer pcb{ writer, "C3D_DrawData", "c3d_drawData" };
		auto pipelineID = pcb.declMember< sdw::UInt >( "pipelineID" );
		auto drawOffset = pcb.declMember< sdw::UInt >( "drawOffset" );
		auto instanceCount = pcb.declMember< sdw::UInt >( "instanceCount" );
		pcb.end();
		auto c3d_cullData = writer.declArrayStorageBuffer< shader::CullData >( "c3d_cullBuffer"
			, uint32_t( MeshBuffersIdx::eCullData )
			, RenderPipeline::eMeshBuffers );

		auto checkVisible = writer.implementFunction< sdw::Boolean >( "checkVisible"
			, [&]( sdw::UInt nodeId
				, sdw::UInt meshletId )
			{
				auto modelData = writer.declLocale( "modelData"
					, c3d_modelsData[nodeId - 1u] );

				IF( writer, meshletId >= modelData.getMeshletCount() )
				{
					writer.returnStmt( sdw::Boolean{ false } );
				}
				FI;

				auto cullData = writer.declLocale( "cullData"
					, c3d_cullData[meshletId] );

				if ( flags.hasWorldPosInputs() )
				{
					auto sphereCenter = writer.declLocale( "sphereCenter"
						, cullData.sphere.xyz() );
					auto sphereRadius = writer.declLocale( "sphereRadius"
						, cullData.sphere.w() );
					auto coneNormal = writer.declLocale( "coneNormal"
						, flags.isFrontCulled() ? -cullData.cone.xyz() : cullData.cone.xyz()
						, checkCones );
					auto coneCutOff = writer.declLocale( "coneCutOff"
						, cullData.cone.w()
						, checkCones );
				}
				else
				{
					auto curMtxModel = writer.declLocale( "curMtxModel"
						, modelData.getModelMtx() );
					auto meanScale = writer.declLocale( "meanScale"
						, ( modelData.getScale().x() + modelData.getScale().y() + modelData.getScale().z() ) / 3.0f );

					auto sphereCenter = writer.declLocale( "sphereCenter"
						, ( curMtxModel * vec4( cullData.sphere.xyz(), 1.0 ) ).xyz() );
					auto sphereRadius = writer.declLocale( "sphereRadius"
						, cullData.sphere.w() * meanScale );

					auto coneNormal = writer.declLocale( "coneNormal"
						, normalize( ( curMtxModel * vec4( flags.isFrontCulled() ? -cullData.cone.xyz() : cullData.cone.xyz(), 0.0 ) ).xyz() )
						, checkCones );
					auto coneCutOff = writer.declLocale( "coneCutOff"
						, cullData.cone.w()
						, checkCones );
				}

				auto sphereCenter = writer.getVariable< sdw::Vec3 >( "sphereCenter" );
				auto sphereRadius = writer.getVariable< sdw::Float >( "sphereRadius" );

				FOR( writer, sdw::UInt, i, 0u, i < 6u, ++i )
				{
					IF( writer, dot( c3d_cameraData.getFrustumPlane( i ).xyz(), sphereCenter ) + c3d_cameraData.getFrustumPlane( i ).w() <= -sphereRadius )
					{
						writer.returnStmt( sdw::Boolean{ false } );
					}
					FI;
				}
				ROF;

				if ( checkCones )
				{
					auto coneNormal = writer.getVariable< sdw::Vec3 >( "coneNormal" );
					auto coneCutOff = writer.getVariable< sdw::Float >( "coneCutOff" );

					IF( writer, coneCutOff == 1.0_f )
					{
						writer.returnStmt( sdw::Boolean{ true } );
					}
					FI;

					auto posToCamera = writer.declLocale( "posToCamera"
						, c3d_cameraData.position() - sphereCenter );

					IF( writer, dot( posToCamera, coneNormal ) >= ( coneCutOff * length( posToCamera ) + sphereRadius ) )
					{
						writer.returnStmt( sdw::Boolean{ false } );
					}
					FI;
				}

				writer.returnStmt( sdw::Boolean{ true } );
			}
			, sdw::InUInt{ writer, "nodeId" }
			, sdw::InUInt{ writer, "meshletId" } );

		writer.implementMainT< shader::PayloadT >( SDW_TaskLocalSize( 32u, 1u, 1u )
			, sdw::TaskPayloadOutT< shader::PayloadT >{ writer }
			, [&]( sdw::TaskSubgroupIn in
				, sdw::TaskPayloadOutT< shader::PayloadT > payload )
			{
				auto laneId = writer.declLocale( "laneId"
					, in.localInvocationID );
				auto baseId = writer.declLocale( "baseId"
					, in.workGroupID );
				auto meshletId = writer.declLocale( "meshletId"
					, ( baseId * 32u + laneId ) );
				auto drawId = writer.declLocale( "drawId"
					, writer.cast< sdw::UInt >( in.drawID ) + drawOffset );
				auto nodeId = writer.declLocale( "nodeId"
					, shader::getNodeId( c3d_objectIdsData
						, c3d_instances
						, pipelineID
						, drawId
						, flags ) );
				auto render = writer.declLocale( "render"
					, checkVisible( nodeId, meshletId ) );
				auto vote = writer.declLocale( "vote"
					, subgroupBallot( render ) );
				auto tasks = writer.declLocale( "tasks"
					, subgroupBallotBitCount( vote ) );
				auto idxOffset = writer.declLocale( "idxOffset"
					, subgroupBallotExclusiveBitCount( vote ) );

				IF( writer, render )
				{
					payload.meshletIndices[idxOffset] = meshletId;
				}
				FI;

				IF( writer, laneId == 0u )
				{
					payload.dispatchMesh( SDW_TaskLocalSize( tasks, 1_u, 1_u ) );
				}
				FI;

			} );
		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr RenderNodesPass::doGetMeshShaderSource( PipelineFlags const & flags )const
	{
		sdw::MeshWriter writer{ &getEngine()->getShaderAllocator() };

		shader::Utils utils{ writer };
		shader::PassShaders passShaders{ getEngine()->getPassComponentsRegister()
			, flags
			, getComponentsMask()
			, utils };

		C3D_Camera( writer
			, GlobalBuffersIdx::eCamera
			, RenderPipeline::eBuffers );
		C3D_ObjectIdsData( writer
			, flags
			, GlobalBuffersIdx::eObjectsNodeID
			, RenderPipeline::eBuffers );
		C3D_ModelsData( writer
			, GlobalBuffersIdx::eModelsData
			, RenderPipeline::eBuffers );
		shader::Materials materials{ writer
			, passShaders
			, uint32_t( GlobalBuffersIdx::eMaterials )
			, RenderPipeline::eBuffers };

		sdw::PushConstantBuffer pcb{ writer, "C3D_DrawData", "c3d_drawData" };
		auto pipelineID = pcb.declMember< sdw::UInt >( "pipelineID" );
		auto drawOffset = pcb.declMember< sdw::UInt >( "drawOffset" );
		auto instanceCount = pcb.declMember< sdw::UInt >( "instanceCount" );
		pcb.end();

#define DeclareSsbo( Name, Type, Binding, Enable )\
		sdw::StorageBuffer Name##Buffer{ writer\
			, #Name + std::string{ "Buffer" }\
			, uint32_t( Binding )\
			, RenderPipeline::eMeshBuffers\
			, ast::type::MemoryLayout::eStd430\
			, Enable };\
		auto Name = Name##Buffer.declMemberArray< Type >( #Name, Enable );\
		Name##Buffer.end()

		// Inputs
		auto c3d_meshlets = writer.declArrayStorageBuffer< shader::Meshlet >( "c3d_meshlets"
			, uint32_t( MeshBuffersIdx::eMeshlets )
			, RenderPipeline::eMeshBuffers );
		DeclareSsbo( c3d_position
			, sdw::Vec4
			, MeshBuffersIdx::ePosition
			, flags.enablePosition() );
		DeclareSsbo( c3d_normal
			, sdw::Vec4
			, MeshBuffersIdx::eNormal
			, flags.enableNormal() );
		DeclareSsbo( c3d_tangent
			, sdw::Vec4
			, MeshBuffersIdx::eTangent
			, flags.enableTangentSpace() );
		DeclareSsbo( c3d_bitangent
			, sdw::Vec4
			, MeshBuffersIdx::eBitangent
			, flags.enableTangentSpace() );
		DeclareSsbo( c3d_texcoord0
			, sdw::Vec4
			, MeshBuffersIdx::eTexcoord0
			, flags.enableTexcoord0() );
		DeclareSsbo( c3d_texcoord1
			, sdw::Vec4
			, MeshBuffersIdx::eTexcoord1
			, flags.enableTexcoord1() );
		DeclareSsbo( c3d_texcoord2
			, sdw::Vec4
			, MeshBuffersIdx::eTexcoord2
			, flags.enableTexcoord2() );
		DeclareSsbo( c3d_texcoord3
			, sdw::Vec4
			, MeshBuffersIdx::eTexcoord3
			, flags.enableTexcoord3() );
		DeclareSsbo( c3d_colour
			, sdw::Vec4
			, MeshBuffersIdx::eColour
			, flags.enableColours() );
		DeclareSsbo( c3d_passMasks
			, sdw::UVec4
			, MeshBuffersIdx::ePassMasks
			, flags.enablePassMasks() );
		DeclareSsbo( c3d_velocity
			, sdw::Vec4
			, MeshBuffersIdx::eVelocity
			, flags.enableVelocity() );
		DeclareSsbo( c3d_instances
			, sdw::UVec4
			, MeshBuffersIdx::eInstances
			, flags.enableInstantiation() );

#undef DeclareSsbo

		auto meshShader = [&]( sdw::UInt const & meshletIndex
			, sdw::MeshSubgroupIn const & in
			, sdw::MeshVertexListOutT< shader::FragmentSurfaceT > & vtxOut
			, sdw::TrianglesMeshPrimitiveListOutT< sdw::VoidT > & primOut )
		{
			auto laneId = writer.declLocale( "laneId"
				, in.localInvocationID );
			auto drawId = writer.declLocale( "drawId"
				, writer.cast< sdw::UInt >( in.drawID ) + drawOffset );
			auto nodeId = writer.declLocale( "nodeId"
				, shader::getNodeId( c3d_objectIdsData
					, c3d_instances
					, pipelineID
					, drawId
					, flags ) );
			auto meshlet = writer.declLocale( "meshlet"
				, c3d_meshlets[meshletIndex] );
			auto triangleCount = writer.declLocale( "triangleCount"
				, meshlet.triangleCount );
			auto vertexCount = writer.declLocale( "vertexCount"
				, meshlet.vertexCount );

			primOut.setMeshOutputCounts( vertexCount, triangleCount );
			auto indexCount = writer.declLocale( "indexCount"
				, triangleCount * 3u );

			FOR( writer, sdw::UInt, i, laneId, i < indexCount, i += 32u )
			{
				//primOut[i].primitiveID = i;
				primOut[i].primitiveIndex = uvec3( meshlet.indices[i * 3u + 0u]
					, meshlet.indices[i * 3u + 1u]
					, meshlet.indices[i * 3u + 2u] );
			}
			ROF;

			FOR( writer, sdw::UInt, i, laneId, i < vertexCount, i += 32u )
			{
				auto vertexIndex = writer.declLocale( "vertexIndex", meshlet.vertices[i] );

				auto curPosition = writer.declLocale( "curPosition"
					, c3d_position[vertexIndex] );
				auto curNormal = writer.declLocale( "curNormal"
					, c3d_normal[vertexIndex].xyz() );
				auto curTangent = writer.declLocale( "curTangent"
					, c3d_tangent[vertexIndex] );
				auto curBitangent = writer.declLocale( "curBitangent"
					, c3d_bitangent[vertexIndex].xyz() );
				vtxOut[i].texture0 = c3d_texcoord0[vertexIndex].xyz();
				vtxOut[i].texture1 = c3d_texcoord1[vertexIndex].xyz();
				vtxOut[i].texture2 = c3d_texcoord2[vertexIndex].xyz();
				vtxOut[i].texture3 = c3d_texcoord3[vertexIndex].xyz();
				vtxOut[i].colour = c3d_colour[vertexIndex].xyz();
				auto modelData = writer.declLocale( "modelData"
					, c3d_modelsData[nodeId - 1u] );
				vtxOut[i].nodeId = writer.cast< sdw::Int >( nodeId );
				auto material = writer.declLocale( "material"
					, materials.getMaterial( modelData.getMaterialId() ) );
				auto passMultipliers = writer.declLocaleArray( "passMultipliers"
					, 4u
					, std::vector< sdw::Vec4 >{ vec4( 1.0_f, 0.0_f, 0.0_f, 0.0_f )
						, vec4( 0.0_f )
						, vec4( 0.0_f )
						, vec4( 0.0_f ) } );
				material.getPassMultipliers( flags
					, c3d_passMasks[vertexIndex]
					, passMultipliers );
				vtxOut[i].passMultipliers[0] = passMultipliers[0];
				vtxOut[i].passMultipliers[1] = passMultipliers[1];
				vtxOut[i].passMultipliers[2] = passMultipliers[2];
				vtxOut[i].passMultipliers[3] = passMultipliers[3];

				auto curMtxModel = writer.declLocale( "curMtxModel"
					, modelData.getModelMtx() );
				auto prvPosition = writer.declLocale( "prvPosition"
					, curPosition );
				prvPosition.xyz() += c3d_velocity[vertexIndex].xyz();

				if ( flags.hasWorldPosInputs() )
				{
					auto worldPos = writer.declLocale( "worldPos"
						, curPosition );
					vtxOut[i].computeTangentSpace( flags
						, c3d_cameraData.position()
						, worldPos.xyz()
						, curNormal
						, curTangent
						, curBitangent );
				}
				else
				{
					auto prvMtxModel = writer.declLocale( "prvMtxModel"
						, modelData.getPrvModelMtx( flags, curMtxModel ) );
					prvPosition = c3d_cameraData.worldToPrvProj( prvMtxModel * prvPosition );
					auto worldPos = writer.declLocale( "worldPos"
						, curMtxModel * curPosition );
					auto mtxNormal = writer.declLocale( "mtxNormal"
						, modelData.getNormalMtx( flags, curMtxModel ) );
					vtxOut[i].computeTangentSpace( flags
						, c3d_cameraData.position()
						, worldPos.xyz()
						, mtxNormal
						, curNormal
						, curTangent
						, curBitangent );
				}

				auto worldPos = writer.getVariable< sdw::Vec4 >( "worldPos" );
				vtxOut[i].worldPosition = worldPos;
				vtxOut[i].viewPosition = c3d_cameraData.worldToCurView( worldPos );
				curPosition = c3d_cameraData.worldToCurProj( worldPos );
				vtxOut[i].vertexId = vertexIndex;
				vtxOut[i].computeVelocity( c3d_cameraData
					, curPosition
					, prvPosition );
				vtxOut[i].position = curPosition;
			}
			ROF;
		};

		if ( flags.usesTask() )
		{
			writer.implementMainT< shader::PayloadT, shader::FragmentSurfaceT, sdw::VoidT >( SDW_MeshLocalSize( 32u, 1u, 1u )
				, sdw::TaskPayloadInT< shader::PayloadT >{ writer }
				, sdw::MeshVertexListOutT< shader::FragmentSurfaceT >{ writer
					, MaxMeshletVertexCount
					, passShaders
					, flags }
				, sdw::TrianglesMeshPrimitiveListOutT< sdw::VoidT >{ writer
					, MaxMeshletTriangleCount }
				, [&]( sdw::MeshSubgroupIn in
					, sdw::TaskPayloadInT< shader::PayloadT > payload
					, sdw::MeshVertexListOutT< shader::FragmentSurfaceT > vtxOut
					, sdw::TrianglesMeshPrimitiveListOutT< sdw::VoidT > primOut )
				{
					auto baseId = writer.declLocale( "baseId"
						, in.workGroupID );
					meshShader( payload.meshletIndices[baseId], in, vtxOut, primOut );
				} );
		}
		else
		{
			writer.implementMainT< sdw::VoidT, shader::FragmentSurfaceT, sdw::VoidT >( SDW_MeshLocalSize( 32u, 1u, 1u )
				, sdw::TaskPayloadIn{ writer }
				, sdw::MeshVertexListOutT< shader::FragmentSurfaceT >{ writer
					, MaxMeshletVertexCount
					, passShaders
					, flags }
				, sdw::TrianglesMeshPrimitiveListOutT< sdw::VoidT >{ writer
					, MaxMeshletTriangleCount }
				, [&]( sdw::MeshSubgroupIn in
					, sdw::TaskPayloadIn payload
					, sdw::MeshVertexListOutT< shader::FragmentSurfaceT > vtxOut
					, sdw::TrianglesMeshPrimitiveListOutT< sdw::VoidT > primOut )
				{
					auto baseId = writer.declLocale( "baseId"
						, in.workGroupID );
					meshShader( baseId, in, vtxOut, primOut );
				} );
		}

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr RenderNodesPass::doGetVertexShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		VertexWriter writer{ &getEngine()->getShaderAllocator() };

		shader::Utils utils{ writer };
		shader::PassShaders passShaders{ getEngine()->getPassComponentsRegister()
			, flags
			, getComponentsMask()
			, utils };

		C3D_Camera( writer
			, GlobalBuffersIdx::eCamera
			, RenderPipeline::eBuffers );
		C3D_ObjectIdsData( writer
			, flags
			, GlobalBuffersIdx::eObjectsNodeID
			, RenderPipeline::eBuffers );
		C3D_ModelsData( writer
			, GlobalBuffersIdx::eModelsData
			, RenderPipeline::eBuffers );
		shader::Materials materials{ writer
			, passShaders
			, uint32_t( GlobalBuffersIdx::eMaterials )
			, RenderPipeline::eBuffers };

		sdw::PushConstantBuffer pcb{ writer, "C3D_DrawData", "c3d_drawData" };
		auto pipelineID = pcb.declMember< sdw::UInt >( "pipelineID" );
		pcb.end();

		writer.implementMainT< shader::VertexSurfaceT, shader::FragmentSurfaceT >( sdw::VertexInT< shader::VertexSurfaceT >{ writer
				, flags }
			, sdw::VertexOutT< shader::FragmentSurfaceT >{ writer
				, passShaders
				, flags }
			, [&]( VertexInT< shader::VertexSurfaceT > in
				, VertexOutT< shader::FragmentSurfaceT > out )
			{
				auto nodeId = writer.declLocale( "nodeId"
					, shader::getNodeId( c3d_objectIdsData
						, in
						, pipelineID
						, writer.cast< sdw::UInt >( in.drawID )
						, flags ) );
				auto curPosition = writer.declLocale( "curPosition"
					, in.position );
				auto curNormal = writer.declLocale( "curNormal"
					, in.normal );
				auto curTangent = writer.declLocale( "curTangent"
					, in.tangent );
				auto curBitangent = writer.declLocale( "curBitangent"
					, in.bitangent );
				out.texture0 = in.texture0;
				out.texture1 = in.texture1;
				out.texture2 = in.texture2;
				out.texture3 = in.texture3;
				out.colour = in.colour;
				out.nodeId = nodeId;
				auto modelData = writer.declLocale( "modelData"
					, c3d_modelsData[nodeId - 1u] );
				auto material = writer.declLocale( "material"
					, materials.getMaterial( modelData.getMaterialId() ) );
				material.getPassMultipliers( flags
					, in.passMasks
					, out.passMultipliers );

				auto curMtxModel = writer.declLocale< Mat4 >( "curMtxModel"
					, modelData.getModelMtx() );
				auto prvPosition = writer.declLocale( "prvPosition"
					, curPosition );
				prvPosition.xyz() += in.velocity;

				if ( flags.hasWorldPosInputs() )
				{
					auto worldPos = writer.declLocale( "worldPos"
						, curPosition );
					out.computeTangentSpace( flags
						, c3d_cameraData.position()
						, worldPos.xyz()
						, curNormal
						, curTangent
						, curBitangent );
				}
				else
				{
					auto prvMtxModel = writer.declLocale( "prvMtxModel"
						, modelData.getPrvModelMtx( flags, curMtxModel ) );
					prvPosition = c3d_cameraData.worldToPrvProj( prvMtxModel * prvPosition );
					auto worldPos = writer.declLocale( "worldPos"
						, curMtxModel * curPosition );
					auto mtxNormal = writer.declLocale( "mtxNormal"
						, modelData.getNormalMtx( flags, curMtxModel ) );
					out.computeTangentSpace( flags
						, c3d_cameraData.position()
						, worldPos.xyz()
						, mtxNormal
						, curNormal
						, curTangent
						, curBitangent );
				}

				auto worldPos = writer.getVariable< sdw::Vec4 >( "worldPos" );
				out.worldPosition = worldPos;
				out.viewPosition = c3d_cameraData.worldToCurView( worldPos );
				curPosition = c3d_cameraData.worldToCurProj( worldPos );
				out.vertexId = in.vertexIndex - in.baseVertex;
				out.computeVelocity( c3d_cameraData
					, curPosition
					, prvPosition );
				out.vtx.position = curPosition;

			} );
		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr RenderNodesPass::doGetHullShaderSource( PipelineFlags const & flags )const
	{
		return nullptr;
	}

	ShaderPtr RenderNodesPass::doGetDomainShaderSource( PipelineFlags const & flags )const
	{
		return nullptr;
	}

	ShaderPtr RenderNodesPass::doGetGeometryShaderSource( PipelineFlags const & flags )const
	{
		return nullptr;
	}

	ShaderPtr RenderNodesPass::doGetBillboardShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		VertexWriter writer{ &getEngine()->getShaderAllocator() };
		bool enableTexcoords = flags.enableTexcoords();

		shader::Utils utils{ writer };
		shader::PassShaders passShaders{ getEngine()->getPassComponentsRegister()
			, flags
			, getComponentsMask()
			, utils };

		// Shader inputs
		auto position = writer.declInput< Vec4 >( "position", 0u );
		auto uv = writer.declInput< Vec2 >( "uv", 1u, enableTexcoords );
		auto center = writer.declInput< Vec3 >( "center", 2u );

		C3D_Camera( writer
			, GlobalBuffersIdx::eCamera
			, RenderPipeline::eBuffers );
		C3D_ObjectIdsData( writer
			, flags
			, GlobalBuffersIdx::eObjectsNodeID
			, RenderPipeline::eBuffers );
		C3D_ModelsData( writer
			, GlobalBuffersIdx::eModelsData
			, RenderPipeline::eBuffers );
		C3D_Billboard( writer
			, GlobalBuffersIdx::eBillboardsData
			, RenderPipeline::eBuffers );

		sdw::PushConstantBuffer pcb{ writer, "C3D_DrawData", "c3d_drawData" };
		auto pipelineID = pcb.declMember< sdw::UInt >( "pipelineID" );
		pcb.end();

		writer.implementMainT< VoidT, shader::FragmentSurfaceT >( sdw::VertexInT< sdw::VoidT >{ writer }
			, sdw::VertexOutT< shader::FragmentSurfaceT >{ writer
				, passShaders
				, flags }
			, [&]( VertexInT< VoidT > in
				, VertexOutT< shader::FragmentSurfaceT > out )
			{
				auto nodeId = writer.declLocale( "nodeId"
					,  shader::getNodeId( c3d_objectIdsData
						, pipelineID
						, writer.cast< sdw::UInt >( in.drawID ) ) );
				auto modelData = writer.declLocale( "modelData"
					, c3d_modelsData[nodeId - 1u] );
				auto passMultipliers = std::vector< sdw::Vec4 >{ vec4( 1.0_f, 0.0_f, 0.0_f, 0.0_f )
					, vec4( 0.0_f )
					, vec4( 0.0_f )
					, vec4( 0.0_f ) };
				out.passMultipliers[0] = passMultipliers[0];
				out.passMultipliers[1] = passMultipliers[1];
				out.passMultipliers[2] = passMultipliers[2];
				out.passMultipliers[3] = passMultipliers[3];
				out.nodeId = nodeId;

				auto curBbcenter = writer.declLocale( "curBbcenter"
					, modelData.modelToCurWorld( vec4( center, 1.0_f ) ).xyz() );
				auto prvBbcenter = writer.declLocale( "prvBbcenter"
					, modelData.modelToPrvWorld( vec4( center, 1.0_f ) ).xyz() );
				auto curToCamera = writer.declLocale( "curToCamera"
					, c3d_cameraData.getPosToCamera( curBbcenter ) );
				curToCamera.y() = 0.0_f;
				curToCamera = normalize( curToCamera );

				auto billboardData = writer.declLocale( "billboardData"
					, c3d_billboardData[nodeId - 1u] );
				auto right = writer.declLocale( "right"
					, billboardData.getCameraRight( c3d_cameraData ) );
				auto up = writer.declLocale( "up"
					, billboardData.getCameraUp( c3d_cameraData ) );
				auto width = writer.declLocale( "width"
					, billboardData.getWidth( c3d_cameraData ) );
				auto height = writer.declLocale( "height"
					, billboardData.getHeight( c3d_cameraData ) );
				auto scaledRight = writer.declLocale( "scaledRight"
					, right * position.x() * width );
				auto scaledUp = writer.declLocale( "scaledUp"
					, up * position.y() * height );
				auto worldPos = writer.declLocale( "worldPos"
					, vec4( ( curBbcenter + scaledRight + scaledUp ), 1.0_f ) );
				out.worldPosition = worldPos;
				out.texture0 = vec3( uv, 0.0_f );

				auto prvPosition = writer.declLocale( "prvPosition"
					, c3d_cameraData.worldToPrvProj( vec4( prvBbcenter + scaledRight + scaledUp, 1.0_f ) ) );
				auto curPosition = writer.declLocale( "curPosition"
					, c3d_cameraData.worldToCurProj( worldPos ) );
				out.viewPosition = c3d_cameraData.worldToCurView( worldPos );
				out.computeVelocity( c3d_cameraData
					, curPosition
					, prvPosition );
				out.vtx.position = curPosition;
				out.vertexId = in.instanceIndex - in.baseInstance;
				out.computeTangentSpace( flags
					, c3d_cameraData.position()
					, worldPos.xyz()
					, curToCamera
					, vec4( up, 0.0_f )
					, right );
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}
}
