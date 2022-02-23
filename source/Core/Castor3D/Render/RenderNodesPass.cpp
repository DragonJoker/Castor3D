#include "Castor3D/Render/RenderNodesPass.hpp"

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
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderQueue.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Culling/SceneCuller.hpp"
#include "Castor3D/Render/Node/BillboardRenderNode.hpp"
#include "Castor3D/Render/Node/SubmeshRenderNode.hpp"
#include "Castor3D/Render/Node/QueueCulledRenderNodes.hpp"
#include "Castor3D/Render/Node/QueueRenderNodes.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Animation/AnimatedMesh.hpp"
#include "Castor3D/Scene/Animation/AnimatedSkeleton.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/ShaderBuffers/TextureAnimationBuffer.hpp"
#include "Castor3D/Shader/ShaderBuffers/TextureConfigurationBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Ubos/BillboardUbo.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelInstancesUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelDataUbo.hpp"
#include "Castor3D/Shader/Ubos/MorphingUbo.hpp"
#include "Castor3D/Shader/Ubos/PickingUbo.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"
#include "Castor3D/Shader/Ubos/SkinningUbo.hpp"

#include <ashespp/Buffer/Buffer.hpp>
#include <ashespp/Buffer/VertexBuffer.hpp>

#include <ShaderWriter/Source.hpp>

#include <RenderGraph/GraphContext.hpp>

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		uint32_t getPrimitiveCount( Submesh const & submesh )
		{
			return submesh.getFaceCount();
		}

		uint32_t getPrimitiveCount( BillboardBase const & instance )
		{
			return instance.getCount() * 2u;
		}

		uint32_t getVertexCount( Submesh const & submesh )
		{
			return submesh.getPointsCount();
		}

		uint32_t getVertexCount( BillboardBase const & instance )
		{
			return instance.getCount();
		}

		template< typename MapType, typename SubFuncType >
		inline void subTraverseNodes( MapType & nodes
			, SubFuncType subFunction )
		{
			for ( auto & itPipelines : nodes )
			{
				for ( auto & itPass : itPipelines.second )
				{
					subFunction( *itPipelines.first, itPass );
				}
			}
		}

		template< typename MapType, typename FuncType >
		inline void traverseNodes( MapType & nodes
			, FuncType function )
		{
			subTraverseNodes( nodes
				, [&function]( auto & first
					, auto & itPass )
				{
					for ( auto & itSubmeshes : itPass.second )
					{
						function( first
							, *itPass.first
							, *itSubmeshes.first
							, itSubmeshes.first->getInstantiation()
							, itSubmeshes.second );
					}
				} );
		}

		template< typename CulledT >
		void updateInstancesUbos( std::map< size_t, UniformBufferOffsetT< ModelInstancesUboConfiguration > > & buffers
			, SceneCuller::CulledInstancesPtrT< CulledT > const & nodes
			, uint32_t instanceMult )
		{
			auto instancesIt = nodes.instances.begin();

			for ( auto & node : nodes.objects )
			{
				auto instances = *instancesIt;
				auto it = buffers.find( hash( *node, instanceMult ) );
				CU_Require( it != buffers.end() );
				cpuUpdate( it->second, *instances );
				++instancesIt;
			}
		}

		template< typename PipelineContT >
		auto findPipeline( PipelineFlags const & flags
			, PipelineContT & pipelines )
		{
			return std::find_if( pipelines.begin()
				, pipelines.end()
				, [&flags]( auto & lookup )
				{
					return lookup->getFlags() == flags;
				} );
		}
	}

	//*********************************************************************************************

	RenderNodesPass::RenderNodesPass( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, String const & typeName
		, String const & category
		, String const & name
		, RenderNodesPassDesc const & desc )
		: OwnedBy< Engine >{ *device.renderSystem.getEngine() }
		, Named{ name }
		, crg::RenderPass{ pass
			, context
			, graph
			, { [this](){ doSubInitialise(); }
				, [this]( crg::RecordContext & context, VkCommandBuffer cb, uint32_t i ){ doSubRecordInto( context, cb, i ); }
				, GetSubpassContentsCallback( [](){ return VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS; } )
				, GetPassIndexCallback( [](){ return 0u; } )
				, IsEnabledCallback( [this](){ return isPassEnabled(); } ) }
			, makeExtent2D( desc.m_size )
			, desc.m_ruConfig }
		, m_device{ device }
		, m_renderSystem{ m_device.renderSystem }
		, m_matrixUbo{ desc.m_matrixUbo }
		, m_culler{ desc.m_culler }
		, m_typeName{ typeName }
		, m_typeID{ getEngine()->getRenderPassTypeID( m_typeName ) }
		, m_renderQueue{ castor::makeUnique< RenderQueue >( *this, desc.m_mode, desc.m_ignored ) }
		, m_category{ category }
		, m_size{ desc.m_size.width, desc.m_size.height }
		, m_mode{ desc.m_mode }
		, m_oit{ desc.m_oit }
		, m_forceTwoSided{ desc.m_forceTwoSided }
		, m_safeBand{ desc.m_safeBand }
		, m_sceneUbo{ m_device }
		, m_index{ desc.m_index }
		, m_instanceMult{ desc.m_instanceMult }
	{
		m_sceneUbo.setWindowSize( m_size );
		m_culler.getScene().getGeometryCache().registerPass( *this );
		m_culler.getScene().getBillboardListCache().registerPass( *this );
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

	void RenderNodesPass::update( CpuUpdater & updater )
	{
		doUpdate( *updater.queues );
		doUpdateUbos( updater );
		m_isDirty = false;
	}

	void RenderNodesPass::update( GpuUpdater & updater )
	{
	}

	ShaderPtr RenderNodesPass::getVertexShaderSource( PipelineFlags const & flags )const
	{
		ShaderPtr result;

		if ( checkFlag( flags.programFlags, ProgramFlag::eBillboards ) )
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

	PipelineFlags RenderNodesPass::createPipelineFlags( BlendMode colourBlendMode
		, BlendMode alphaBlendMode
		, PassFlags passFlags
		, RenderPassTypeID renderPassTypeID
		, PassTypeID passTypeID
		, uint32_t heightTextureIndex
		, VkCompareOp alphaFunc
		, VkCompareOp blendAlphaFunc
		, TextureFlagsArray const & textures
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, VkPrimitiveTopology topology )
	{
		auto result = PipelineFlags{ colourBlendMode
			, alphaBlendMode
			, passFlags
			, renderPassTypeID
			, passTypeID
			, heightTextureIndex
			, programFlags
			, sceneFlags
			, topology
			, 3u
			, alphaFunc
			, blendAlphaFunc
			, textures };
		updateFlags( result );
		return result;
	}

	PipelineFlags RenderNodesPass::createPipelineFlags( Pass const & pass
		, TextureFlagsArray const & textures
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, VkPrimitiveTopology topology )
	{
		return createPipelineFlags( pass.getColourBlendMode()
			, pass.getAlphaBlendMode()
			, pass.getPassFlags()
			, ( pass.getRenderPassInfo()
				? pass.getRenderPassInfo()->id
				: RenderPassTypeID{} )
			, pass.getTypeID()
			, pass.getHeightTextureIndex()
			, pass.getAlphaFunc()
			, pass.getBlendAlphaFunc()
			, textures
			, programFlags
			, sceneFlags
			, topology );
	}

	RenderPipeline & RenderNodesPass::prepareBackPipeline( PipelineFlags pipelineFlags
		, ashes::PipelineVertexInputStateCreateInfoCRefArray const & vertexLayouts
		, ashes::DescriptorSetLayoutCRefArray descriptorLayouts )
	{
		return doPreparePipeline( vertexLayouts
			, std::move( descriptorLayouts )
			, std::move( pipelineFlags )
			, VK_CULL_MODE_BACK_BIT );
	}

	RenderPipeline & RenderNodesPass::prepareFrontPipeline( PipelineFlags pipelineFlags
		, ashes::PipelineVertexInputStateCreateInfoCRefArray const & vertexLayouts
		, ashes::DescriptorSetLayoutCRefArray descriptorLayouts )
	{
		return doPreparePipeline( vertexLayouts
			, std::move( descriptorLayouts )
			, std::move( pipelineFlags )
			, VK_CULL_MODE_FRONT_BIT );
	}

	SubmeshRenderNode * RenderNodesPass::createSkinningNode( Pass & pass
		, RenderPipeline & pipeline
		, Submesh & submesh
		, Geometry & primitive
		, AnimatedSkeleton & skeleton )
	{
		return doCreateSubmeshNode( pass
			, pipeline
			, submesh
			, primitive
			, nullptr
			, &skeleton );
	}

	SubmeshRenderNode * RenderNodesPass::createMorphingNode( Pass & pass
		, RenderPipeline & pipeline
		, Submesh & submesh
		, Geometry & primitive
		, AnimatedMesh & mesh )
	{
		return doCreateSubmeshNode( pass
			, pipeline
			, submesh
			, primitive
			, &mesh
			, nullptr );
	}

	SubmeshRenderNode * RenderNodesPass::createStaticNode( Pass & pass
		, RenderPipeline & pipeline
		, Submesh & submesh
		, Geometry & primitive )
	{
		return doCreateSubmeshNode( pass
			, pipeline
			, submesh
			, primitive
			, nullptr
			, nullptr );
	}

	BillboardRenderNode * RenderNodesPass::createBillboardNode( Pass & pass
		, RenderPipeline & pipeline
		, BillboardBase & billboard )
	{
		auto & buffers = billboard.getGeometryBuffers();
		auto & scene = billboard.getParentScene();
		auto billboardEntry = scene.getBillboardListCache().getUbos( billboard
			, pass
			, getInstanceMult() );
		m_isDirty = true;

		return &m_renderQueue->getAllRenderNodes().createNode( PassRenderNode{ pass }
			, buffers
			, *billboard.getNode()
			, billboard );
	}

	void RenderNodesPass::updatePipeline( RenderPipeline & pipeline )
	{
		doUpdatePipeline( pipeline );
	}

	void RenderNodesPass::updateFlags( PipelineFlags & flags )const
	{
		doUpdateFlags( flags );
	}

	FilteredTextureFlags RenderNodesPass::filterTexturesFlags( TextureFlagsArray const & textures )const
	{
		return castor3d::filterTexturesFlags( textures
			, getTexturesMask() );
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
			break;

		case BlendMode::eAdditive:
			attach.blendEnable = VK_TRUE;
			attach.colorBlendOp = VK_BLEND_OP_ADD;
			attach.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
			attach.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
			break;

		case BlendMode::eMultiplicative:
			attach.blendEnable = VK_TRUE;
			attach.colorBlendOp = VK_BLEND_OP_ADD;
			attach.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
			attach.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
			break;

		case BlendMode::eInterpolative:
			attach.blendEnable = VK_TRUE;
			attach.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_COLOR;
			attach.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
			break;

		default:
			attach.blendEnable = VK_TRUE;
			attach.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_COLOR;
			attach.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
			break;
		}

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

		attach.colorWriteMask = defaultColorWriteMask;

		return ashes::PipelineColorBlendStateCreateInfo
		{
			0u,
			VK_FALSE,
			VK_LOGIC_OP_COPY,
			ashes::VkPipelineColorBlendAttachmentStateArray{ size_t( attachesCount ), attach },
		};
	}

	TextureFlags RenderNodesPass::getTexturesMask()const
	{
		return TextureFlags{ TextureFlag::eAll };
	}

	bool RenderNodesPass::isValidPass( Pass const & pass )const
	{
		return doIsValidPass( pass );
	}

	bool RenderNodesPass::hasNodes()const
	{
		return m_renderQueue->hasNodes();
	}

	bool RenderNodesPass::isPassEnabled()const
	{
		return hasNodes();
	}

	void RenderNodesPass::initialiseAdditionalDescriptor( RenderPipeline & pipeline
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		auto flags = pipeline.getFlags();
		doUpdateFlags( flags );
		auto nodeType = getRenderNodeType( flags.programFlags );
		auto & descriptors = m_additionalDescriptors[size_t( nodeType )];

		if ( !descriptors.set )
		{
			auto & scene = getCuller().getScene();
			descriptors.set = descriptors.pool->createDescriptorSet( getName() + "_Add"
				, RenderPipeline::ePass );
			ashes::WriteDescriptorSetArray descriptorWrites;
			descriptorWrites.push_back( m_matrixUbo.getDescriptorWrite( uint32_t( GlobalBuffersIdx::eMatrix ) ) );
			descriptorWrites.push_back( m_sceneUbo.getDescriptorWrite( uint32_t( GlobalBuffersIdx::eScene ) ) );

			auto & nodesIds = m_renderQueue->getCulledRenderNodes().getNodesIds();
			auto nodesIdsWrite = ashes::WriteDescriptorSet{ uint32_t( GlobalBuffersIdx::eObjectsNodeID )
				, 0u
				, 1u
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER };
			nodesIdsWrite.bufferInfo.push_back( { nodesIds.getBuffer()
				, 0u
				, nodesIds.getBuffer().getSize() } );
			descriptorWrites.push_back( nodesIdsWrite );

			auto & modelBuffer = checkFlag( flags.programFlags, ProgramFlag::eBillboards )
				? scene.getBillboardListCache().getModelBuffer()
				: scene.getGeometryCache().getModelBuffer();
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
			descriptorWrites.push_back( matCache.getTexConfigBuffer().getBinding( uint32_t( GlobalBuffersIdx::eTexConfigs ) ) );
			descriptorWrites.push_back( matCache.getTexAnimBuffer().getBinding( uint32_t( GlobalBuffersIdx::eTexAnims ) ) );

			if ( checkFlag( flags.programFlags, ProgramFlag::eBillboards ) )
			{
				auto & billboardDatas = scene.getBillboardListCache().getBillboardsBuffer();
				auto write = ashes::WriteDescriptorSet{ uint32_t( GlobalBuffersIdx::eBillboardsData )
					, 0u
					, 1u
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER };
				write.bufferInfo.push_back( { billboardDatas.getBuffer()
					, 0u
					, billboardDatas.getBuffer().getSize() } );
				descriptorWrites.push_back( write );
			}

			if ( checkFlag( flags.programFlags, ProgramFlag::eMorphing ) )
			{
				auto & morphingDatas = scene.getAnimatedObjectGroupCache().getMorphingBuffer();
				auto write = ashes::WriteDescriptorSet{ uint32_t( GlobalBuffersIdx::eMorphingData )
					, 0u
					, 1u
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER };
				write.bufferInfo.push_back( { morphingDatas.getBuffer()
					, 0u
					, morphingDatas.getBuffer().getSize() } );
				descriptorWrites.push_back( write );
			}

			if ( checkFlag( flags.programFlags, ProgramFlag::eSkinning ) )
			{
				auto & transformsDatas = scene.getAnimatedObjectGroupCache().getSkinningTransformsBuffer();
				auto transformsWrite = ashes::WriteDescriptorSet{ uint32_t( GlobalBuffersIdx::eSkinningTransformData )
					, 0u
					, 1u
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER };
				transformsWrite.bufferInfo.push_back( { transformsDatas.getBuffer()
					, 0u
					, transformsDatas.getBuffer().getSize() } );
				descriptorWrites.push_back( transformsWrite );
			}

			doFillAdditionalDescriptor( descriptorWrites
				, shadowMaps );
			descriptors.set->setBindings( descriptorWrites );
			descriptors.set->update();
		}

		pipeline.setAdditionalDescriptorSet( *descriptors.set );
	}

	void RenderNodesPass::doSubInitialise()
	{
		m_renderQueue->initialise();
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

	uint32_t RenderNodesPass::doCopyNodesIds( SubmeshRenderNodePtrArray const & renderNodes
		, std::vector< InstantiationData > & instanceBuffer )const
	{
		auto const count = std::min( uint32_t( instanceBuffer.size() / m_instanceMult )
			, uint32_t( renderNodes.size() ) );
		auto buffer = instanceBuffer.data();
		auto it = renderNodes.begin();
		auto i = 0u;

		while ( i < count )
		{
			auto & node = *it;

			for ( auto inst = 0u; inst < m_instanceMult; ++inst )
			{
				CU_Require( node->getId() > 0u );
				buffer->m_objectIDs->x = node->getId() - 1u;

				if ( node->mesh )
				{
					CU_Require( node->mesh->getId() > 0u );
					buffer->m_objectIDs->y = node->mesh->getId() - 1u;
				}

				if ( node->skeleton )
				{
					CU_Require( node->skeleton->getId() > 0u );
					buffer->m_objectIDs->z = node->skeleton->getId() - 1u;
				}

				++buffer;
			}

			++i;
			++it;
		}

		return count;
	}

	uint32_t RenderNodesPass::doCopyNodesIds( SubmeshRenderNodePtrArray const & renderNodes
		, std::vector< InstantiationData > & instanceBuffer
		, RenderInfo & info )const
	{
		auto count = doCopyNodesIds( renderNodes, instanceBuffer );
		info.m_visibleObjectsCount += count;
		return count;
	}

	uint32_t RenderNodesPass::doCopyNodesBones( SubmeshRenderNodePtrArray const & renderNodes
		, ShaderBuffer & bonesBuffer )const
	{
		uint32_t const mtxSize = sizeof( float ) * 16;
		VkDeviceSize const stride = mtxSize * 400u;
		auto const count = std::min( uint32_t( bonesBuffer.getSize() / stride ), uint32_t( renderNodes.size() ) );
		CU_Require( count <= renderNodes.size() );
		auto buffer = bonesBuffer.getPtr();
		auto it = renderNodes.begin();
		auto i = 0u;

		while ( i < count )
		{
			auto & node = *it;

			for ( auto inst = 0u; inst < m_instanceMult; ++inst )
			{
				node->skeleton->fillBuffer( buffer );
				buffer += stride;
			}

			++i;
			++it;
		}

		return count;
	}

	uint32_t RenderNodesPass::doCopyNodesBones( SubmeshRenderNodePtrArray const & renderNodes
		, ShaderBuffer & bonesBuffer
		, RenderInfo & info )const
	{
		auto count = doCopyNodesBones( renderNodes, bonesBuffer );
		info.m_visibleObjectsCount += count;
		return count;
	}

	void RenderNodesPass::doUpdate( SubmeshRenderNodesPtrByPipelineMap & nodes )
	{
		traverseNodes( nodes
			, [this]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, InstantiationComponent & instantiation
				, SubmeshRenderNodePtrArray & renderNodes )
			{
				auto it = instantiation.find( pass.getOwner(), m_instanceMult );
				auto index = instantiation.getIndex( m_instanceMult );

				if ( !renderNodes.empty()
					&& it != instantiation.end()
					&& it->second[index].buffer )
				{
					doCopyNodesIds( renderNodes
						, it->second[index].data );
				}
			} );
	}

	void RenderNodesPass::doUpdate( SubmeshRenderNodesPtrByPipelineMap & nodes
		, RenderInfo & info )
	{
		traverseNodes( nodes
			, [this, &info]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, InstantiationComponent & instantiation
				, SubmeshRenderNodePtrArray & renderNodes )
			{
				auto it = instantiation.find( pass.getOwner(), m_instanceMult );
				auto index = instantiation.getIndex( m_instanceMult );

				if ( !renderNodes.empty()
					&& it != instantiation.end()
					&& it->second[index].buffer )
				{
					uint32_t count1 = doCopyNodesIds( renderNodes
						, it->second[index].data );
					info.m_visibleFaceCount += submesh.getFaceCount() * count1;
					info.m_visibleVertexCount += submesh.getPointsCount() * count1;
					++info.m_drawCalls;
				}
			} );
	}

	namespace
	{
		template< typename NodeT >
		inline void renderNonInstanced( RenderNodesPass & pass
			, NodePtrByPipelineMapT< NodeT > & nodes )
		{
			for ( auto & itPipelines : nodes )
			{
				pass.updatePipeline( *itPipelines.first );
			}
		}

		template< typename NodeT >
		inline void doUpdateInfos( NodePtrByPipelineMapT< NodeT > & nodes
			, RenderInfo & info )
		{
			for ( auto & itPipelines : nodes )
			{
				for ( auto & renderNode : itPipelines.second )
				{
					info.m_visibleFaceCount += getPrimitiveCount( renderNode->data );
					info.m_visibleVertexCount += getVertexCount( renderNode->data );
					++info.m_drawCalls;
					++info.m_visibleObjectsCount;
				}
			}
		}
	}

	void RenderNodesPass::doUpdate( SubmeshRenderNodePtrByPipelineMap & nodes )
	{
		renderNonInstanced( *this, nodes );
	}

	void RenderNodesPass::doUpdate( SubmeshRenderNodePtrByPipelineMap & nodes
		, RenderInfo & info )
	{
		doUpdate( nodes );
		doUpdateInfos( nodes, info );
	}

	void RenderNodesPass::doUpdate( BillboardRenderNodePtrByPipelineMap & nodes )
	{
		renderNonInstanced( *this
			, nodes );
	}

	void RenderNodesPass::doUpdate( BillboardRenderNodePtrByPipelineMap & nodes
		, RenderInfo & info )
	{
		doUpdate( nodes );
		doUpdateInfos( nodes, info );
	}

	void RenderNodesPass::doUpdate( RenderQueueArray & queues )
	{
		queues.emplace_back( *m_renderQueue );
	}

	void RenderNodesPass::doUpdateUbos( CpuUpdater & updater )
	{
		auto & camera = *updater.camera;
		auto jitterProjSpace = updater.jitter * 2.0f;
		jitterProjSpace[0] /= float( camera.getWidth() );
		jitterProjSpace[1] /= float( camera.getHeight() );
		m_matrixUbo.cpuUpdate( camera.getView()
			, camera.getProjection( m_safeBand != 0u )
			, jitterProjSpace );
		m_sceneUbo.cpuUpdate( *camera.getScene(), &camera );
	}

	bool RenderNodesPass::doIsValidPass( Pass const & pass )const
	{
		return ( pass.getRenderPassInfo() == nullptr || pass.getRenderPassInfo()->create == nullptr )
			&& doAreValidPassFlags( pass.getPassFlags() );
	}

	SceneFlags RenderNodesPass::doAdjustFlags( SceneFlags flags )const
	{
		return flags;
	}

	bool RenderNodesPass::doAreValidPassFlags( PassFlags const & passFlags )const
	{
		if ( checkFlag( passFlags, PassFlag::eAlphaBlending ) )
		{
			if ( checkFlag( passFlags, PassFlag::eAlphaTest ) )
			{
				return true;
			}

			return m_mode == RenderMode::eBoth
				|| m_mode == RenderMode::eTransparentOnly;
		}

		return m_mode == RenderMode::eBoth
			|| m_mode == RenderMode::eOpaqueOnly;
	}

	ShaderProgramSPtr RenderNodesPass::doGetProgram( PipelineFlags const & flags
		, VkCullModeFlags cullMode )
	{
		return getEngine()->getShaderProgramCache().getAutomaticProgram( *this, flags );
	}

	ashes::VkDescriptorSetLayoutBindingArray RenderNodesPass::doCreateAdditionalBindings( PipelineFlags const & flags )const
	{
		VkShaderStageFlags stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT;

		if ( checkFlag( flags.programFlags, ProgramFlag::eHasGeometry ) )
		{
			stageFlags |= VK_SHADER_STAGE_GEOMETRY_BIT;
		}

		if ( checkFlag( flags.programFlags, ProgramFlag::eHasTessellation ) )
		{
			stageFlags |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
			stageFlags |= VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
		}

		ashes::VkDescriptorSetLayoutBindingArray addBindings;
		addBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( GlobalBuffersIdx::eMatrix )
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, stageFlags ) );
		addBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( GlobalBuffersIdx::eScene )
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, stageFlags ) );
		addBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( GlobalBuffersIdx::eObjectsNodeID )
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			, stageFlags ) );
		addBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( GlobalBuffersIdx::eModelsData )
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			, stageFlags ) );
		auto & matCache = getOwner()->getMaterialCache();
		addBindings.emplace_back( matCache.getPassBuffer().createLayoutBinding( uint32_t( GlobalBuffersIdx::eMaterials ) ) );
		addBindings.emplace_back( matCache.getTexConfigBuffer().createLayoutBinding( uint32_t( GlobalBuffersIdx::eTexConfigs ) ) );
		addBindings.emplace_back( matCache.getTexAnimBuffer().createLayoutBinding( uint32_t( GlobalBuffersIdx::eTexAnims ) ) );

		if ( checkFlag( flags.programFlags, ProgramFlag::eBillboards ) )
		{
			addBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( GlobalBuffersIdx::eBillboardsData )
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, stageFlags ) );
		}

		if ( checkFlag( flags.programFlags, ProgramFlag::eMorphing ) )
		{
			addBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( GlobalBuffersIdx::eMorphingData )
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, stageFlags ) );
		}

		if ( checkFlag( flags.programFlags, ProgramFlag::eSkinning ) )
		{
			addBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( GlobalBuffersIdx::eSkinningTransformData )
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, stageFlags ) );
		}

		doFillAdditionalBindings( addBindings );
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
		, ashes::DescriptorSetLayoutCRefArray descriptorLayouts
		, PipelineFlags flags
		, VkCullModeFlags cullMode )
	{
		auto & renderSystem = *getEngine()->getRenderSystem();
		auto & device = renderSystem.getRenderDevice();
		RenderPipeline * result{};

		if ( m_mode != RenderMode::eTransparentOnly )
		{
			flags.alphaBlendMode = BlendMode::eNoBlend;
		}

		if ( checkFlag( cullMode, VK_CULL_MODE_FRONT_BIT ) )
		{
			addFlag( flags.programFlags, ProgramFlag::eInvertNormals );
		}
		else
		{
			remFlag( flags.programFlags, ProgramFlag::eInvertNormals );
		}

		auto & descriptors = m_additionalDescriptors[size_t( getRenderNodeType( flags.programFlags ) )];

		if ( !descriptors.layout )
		{
			auto bindings = doCreateAdditionalBindings( flags );
			descriptors.layout = device->createDescriptorSetLayout( getName() + "Add"
				, std::move( bindings ) );
			descriptors.pool = descriptors.layout->createPool( 1u );
		}

		auto program = doGetProgram( flags, cullMode );

		if ( doAreValidPassFlags( flags.passFlags )
			&& ( !checkFlag( flags.programFlags, ProgramFlag::eBillboards )
				|| !isShadowMapProgram( flags.programFlags ) ) )
		{
			auto & pipelines = checkFlag( cullMode, VK_CULL_MODE_FRONT_BIT )
				? doGetFrontPipelines()
				: doGetBackPipelines();
			auto it = findPipeline( flags, pipelines );

			if ( it == pipelines.end() )
			{
				auto pipeline = castor::makeUnique< RenderPipeline >( *this
					, renderSystem
					, doCreateDepthStencilState( flags )
					, ashes::PipelineRasterizationStateCreateInfo{ 0u, false, false, VK_POLYGON_MODE_FILL, cullMode }
					, doCreateBlendState( flags )
					, ashes::PipelineMultisampleStateCreateInfo{}
					, program
					, flags );
				pipeline->setVertexLayouts( vertexLayouts );
				pipeline->setViewport( makeViewport( m_size ) );

				if ( !checkFlag( flags.programFlags, ProgramFlag::ePicking ) )
				{
					pipeline->setScissor( makeScissor( m_size ) );
				}

				pipeline->setDescriptorSetLayout( *descriptors.layout );
				pipeline->setPushConstantRanges( { { VK_SHADER_STAGE_VERTEX_BIT, 0u, 8u } } );
				pipeline->initialise( device
					, getRenderPass()
					, std::move( descriptorLayouts ) );
				pipelines.emplace_back( std::move( pipeline ) );
				it = std::next( pipelines.begin()
					, ptrdiff_t( pipelines.size() - 1u ) );
			}

			result = it->get();
		}

		return *result;
	}

	SubmeshRenderNode * RenderNodesPass::doCreateSubmeshNode( Pass & pass
		, RenderPipeline & pipeline
		, Submesh & submesh
		, Geometry & primitive
		, AnimatedMesh * mesh
		, AnimatedSkeleton * skeleton )
	{
		auto & buffers = submesh.getGeometryBuffers( getShaderFlags()
			, pipeline.getFlags().programFlags
			, pass.getOwner()
			, getInstanceMult()
			, pass.getTexturesMask()
			, checkFlag( pipeline.getFlags().programFlags, ProgramFlag::eForceTexCoords ) );
		auto & scene = *primitive.getScene();
		auto geometryEntry = scene.getGeometryCache().getUbos( primitive
			, submesh
			, pass
			, getInstanceMult() );
		m_isDirty = true;

		auto & result = m_renderQueue->getAllRenderNodes().createNode( PassRenderNode{ pass }
			, buffers
			, *primitive.getParent()
			, submesh
			, primitive
			, mesh
			, skeleton );
		CU_Require( !mesh || result.mesh );
		CU_Require( !skeleton || result.skeleton );
		return &result;
	}

	ShaderPtr RenderNodesPass::doGetVertexShaderSource( PipelineFlags const & flags )const
	{
		// Since their vertex attribute locations overlap, we must not have both set at the same time.
		CU_Require( ( checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) ? 1 : 0 )
			+ ( checkFlag( flags.programFlags, ProgramFlag::eMorphing ) ? 1 : 0 ) < 2
			&& "Can't have both instantiation and morphing yet." );
		using namespace sdw;
		VertexWriter writer;
		auto textureFlags = filterTexturesFlags( flags.textures );
		bool hasTextures = !flags.textures.empty();

		C3D_Matrix( writer
			, GlobalBuffersIdx::eMatrix
			, RenderPipeline::ePass );
		C3D_Scene( writer
			, GlobalBuffersIdx::eScene
			, RenderPipeline::ePass );
		C3D_ObjectIdsData( writer
			, GlobalBuffersIdx::eObjectsNodeID
			, RenderPipeline::ePass );
		C3D_ModelsData( writer
			, GlobalBuffersIdx::eModelsData
			, RenderPipeline::ePass );
		C3D_Morphing( writer
			, GlobalBuffersIdx::eMorphingData
			, RenderPipeline::ePass
			, flags.programFlags );
		auto skinningData = SkinningUbo::declare( writer
			, uint32_t( GlobalBuffersIdx::eSkinningTransformData )
			, RenderPipeline::ePass
			, flags.programFlags );

		sdw::Pcb pcb{ writer, "DrawData" };
		auto pipelineID = pcb.declMember< sdw::UInt >( "pipelineID" );
		auto customDrawID = pcb.declMember< sdw::UInt >( "customDrawID" );
		pcb.end();

		writer.implementMainT< shader::VertexSurfaceT, shader::FragmentSurfaceT >( sdw::VertexInT< shader::VertexSurfaceT >{ writer
				, flags.programFlags
				, getShaderFlags()
				, textureFlags
				, flags.passFlags
				, hasTextures }
			, sdw::VertexOutT< shader::FragmentSurfaceT >{ writer
				, flags.programFlags
				, getShaderFlags()
				, textureFlags
				, flags.passFlags
				, hasTextures }
			, [&]( VertexInT< shader::VertexSurfaceT > in
				, VertexOutT< shader::FragmentSurfaceT > out )
			{
				auto ids = shader::getIds( c3d_objectIdsData
					, in
					, pipelineID
					, customDrawID
					, flags.programFlags );
				auto curPosition = writer.declLocale( "curPosition"
					, in.position );
				auto v4Normal = writer.declLocale( "v4Normal"
					, vec4( in.normal, 0.0_f ) );
				auto v4Tangent = writer.declLocale( "v4Tangent"
					, vec4( in.tangent, 0.0_f ) );
				out.texture0 = in.texture0;
				auto morphingData = writer.declLocale( "morphingData"
					, c3d_morphingData[ids.morphingId]
					, checkFlag( flags.programFlags, ProgramFlag::eMorphing ) );
				in.morph( morphingData
					, curPosition
					, v4Normal
					, v4Tangent
					, out.texture0 );
				auto modelData = writer.declLocale( "modelData"
					, c3d_modelsData[ids.nodeId] );
				out.textures0 = modelData.getTextures0();
				out.textures1 = modelData.getTextures1();
				out.textures = modelData.getTextures();
				out.material = modelData.getMaterialId();
				out.nodeId = writer.cast< Int >( ids.nodeId );
				out.instanceId = writer.cast< UInt >( in.instanceIndex );

				auto curMtxModel = writer.declLocale< Mat4 >( "curMtxModel"
					, modelData.getCurModelMtx( flags.programFlags
						, skinningData
						, ids.skinningId
						, in.boneIds0
						, in.boneIds1
						, in.boneWeights0
						, in.boneWeights1 ) );
				auto prvMtxModel = writer.declLocale< Mat4 >( "prvMtxModel"
					, modelData.getPrvModelMtx( flags.programFlags, curMtxModel ) );
				auto prvPosition = writer.declLocale( "prvPosition"
					, c3d_matrixData.worldToPrvProj( prvMtxModel * curPosition ) );
				auto worldPos = writer.declLocale( "worldPos"
					, curMtxModel * curPosition );
				out.worldPosition = worldPos;
				out.viewPosition = c3d_matrixData.worldToCurView( worldPos );
				curPosition = c3d_matrixData.worldToCurProj( worldPos );
				out.computeVelocity( c3d_matrixData
					, curPosition
					, prvPosition );
				out.vtx.position = curPosition;

				auto mtxNormal = writer.declLocale< Mat3 >( "mtxNormal"
					, modelData.getNormalMtx( flags.programFlags, curMtxModel ) );
				out.computeTangentSpace( flags.programFlags
					, c3d_sceneData.cameraPosition
					, worldPos.xyz()
					, mtxNormal
					, v4Normal
					, v4Tangent );

			} );
		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr RenderNodesPass::doGetBillboardShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		VertexWriter writer;
		auto textureFlags = filterTexturesFlags( flags.textures );
		bool hasTextures = !flags.textures.empty();

		// Shader inputs
		auto position = writer.declInput< Vec4 >( "position", 0u );
		auto uv = writer.declInput< Vec2 >( "uv", 1u, hasTextures );
		auto center = writer.declInput< Vec3 >( "center", 2u );

		C3D_Matrix( writer
			, GlobalBuffersIdx::eMatrix
			, RenderPipeline::ePass );
		C3D_Scene( writer
			, GlobalBuffersIdx::eScene
			, RenderPipeline::ePass );
		C3D_ObjectIdsData( writer
			, GlobalBuffersIdx::eObjectsNodeID
			, RenderPipeline::ePass );
		C3D_ModelsData( writer
			, GlobalBuffersIdx::eModelsData
			, RenderPipeline::ePass );
		C3D_Billboard( writer
			, GlobalBuffersIdx::eBillboardsData
			, RenderPipeline::ePass );

		sdw::Pcb pcb{ writer, "DrawData" };
		auto pipelineID = pcb.declMember< sdw::UInt >( "pipelineID" );
		auto customDrawID = pcb.declMember< sdw::UInt >( "customDrawID" );
		pcb.end();

		writer.implementMainT< VoidT, shader::FragmentSurfaceT >( sdw::VertexInT< sdw::VoidT >{ writer }
			, sdw::VertexOutT< shader::FragmentSurfaceT >{ writer
				, flags.programFlags
				, getShaderFlags()
				, textureFlags
				, flags.passFlags
				, hasTextures }
			, [&]( VertexInT< VoidT > in
				, VertexOutT< shader::FragmentSurfaceT > out )
			{
				auto nodeId = writer.declLocale( "nodeId"
					,  shader::getNodeId( c3d_objectIdsData
						, pipelineID
						, customDrawID ) );
				auto modelData = writer.declLocale( "modelData"
					, c3d_modelsData[nodeId] );
				out.textures0 = modelData.getTextures0();
				out.textures1 = modelData.getTextures1();
				out.textures = modelData.getTextures();
				out.material = modelData.getMaterialId();
				out.nodeId = writer.cast< sdw::Int >( nodeId );
				out.instanceId = writer.cast< UInt >( in.instanceIndex );

				auto curBbcenter = writer.declLocale( "curBbcenter"
					, modelData.modelToCurWorld( vec4( center, 1.0_f ) ).xyz() );
				auto prvBbcenter = writer.declLocale( "prvBbcenter"
					, modelData.modelToPrvWorld( vec4( center, 1.0_f ) ).xyz() );
				auto curToCamera = writer.declLocale( "curToCamera"
					, c3d_sceneData.getPosToCamera( curBbcenter ) );
				curToCamera.y() = 0.0_f;
				curToCamera = normalize( curToCamera );

				auto billboardData = writer.declLocale( "billboardData"
					, c3d_billboardData[nodeId] );
				auto right = writer.declLocale( "right"
					, billboardData.getCameraRight( flags.programFlags, c3d_matrixData ) );
				auto up = writer.declLocale( "up"
					, billboardData.getCameraUp( flags.programFlags, c3d_matrixData ) );
				auto width = writer.declLocale( "width"
					, billboardData.getWidth( flags.programFlags, c3d_sceneData ) );
				auto height = writer.declLocale( "height"
					, billboardData.getHeight( flags.programFlags, c3d_sceneData ) );
				auto scaledRight = writer.declLocale( "scaledRight"
					, right * position.x() * width );
				auto scaledUp = writer.declLocale( "scaledUp"
					, up * position.y() * height );
				auto worldPos = writer.declLocale( "worldPos"
					, vec4( ( curBbcenter + scaledRight + scaledUp ), 1.0_f ) );
				out.worldPosition = worldPos;

				if ( hasTextures )
				{
					out.texture0 = vec3( uv, 0.0_f );
				}

				auto prvPosition = writer.declLocale( "prvPosition"
					, c3d_matrixData.worldToPrvProj( vec4( prvBbcenter + scaledRight + scaledUp, 1.0_f ) ) );
				auto curPosition = writer.declLocale( "curPosition"
					, c3d_matrixData.worldToCurProj( worldPos ) );
				out.viewPosition = c3d_matrixData.worldToCurView( worldPos );
				out.computeVelocity( c3d_matrixData
					, curPosition
					, prvPosition );
				out.vtx.position = curPosition;

				out.computeTangentSpace( c3d_sceneData.cameraPosition
					, worldPos.xyz()
					, curToCamera
					, up
					, right );
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}
}
