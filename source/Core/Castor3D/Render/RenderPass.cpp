#include "Castor3D/Render/RenderPass.hpp"

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
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Culling/SceneCuller.hpp"
#include "Castor3D/Render/Node/BillboardRenderNode.hpp"
#include "Castor3D/Render/Node/SubmeshRenderNode.hpp"
#include "Castor3D/Render/Node/QueueRenderNodes.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Animation/AnimatedSkeleton.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/PassBuffer/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/TextureConfigurationBuffer/TextureConfigurationBuffer.hpp"
#include "Castor3D/Shader/Ubos/BillboardUbo.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelInstancesUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelUbo.hpp"
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

	SceneRenderPass::SceneRenderPass( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, String const & category
		, String const & name
		, SceneRenderPassDesc const & desc )
		: OwnedBy< Engine >{ *device.renderSystem.getEngine() }
		, Named{ name }
		, crg::RenderPass{ pass, context, graph, makeExtent2D( desc.m_size ) }
		, m_device{ device }
		, m_renderSystem{ m_device.renderSystem }
		, m_matrixUbo{ desc.m_matrixUbo }
		, m_culler{ desc.m_culler }
		, m_renderQueue{ *this, desc.m_mode, desc.m_ignored }
		, m_category{ category }
		, m_size{ desc.m_size.width, desc.m_size.height }
		, m_oit{ desc.m_oit }
		, m_forceTwoSided{ desc.m_forceTwoSided }
		, m_mode{ desc.m_mode }
		, m_sceneUbo{ m_device }
		, m_index{ desc.m_index }
		, m_instanceMult{ desc.m_instanceMult }
	{
		m_sceneUbo.setWindowSize( m_size );
		m_culler.getScene().getGeometryCache().registerPass( *this );
		m_culler.getScene().getBillboardListCache().registerPass( *this );
	}

	SceneRenderPass::~SceneRenderPass()
	{
		m_renderQueue.cleanup();
		m_backPipelines.clear();
		m_frontPipelines.clear();
	}

	void SceneRenderPass::update( CpuUpdater & updater )
	{
		doUpdate( *updater.queues );
		doUpdateUbos( updater );
		m_isDirty = false;
	}

	ShaderPtr SceneRenderPass::getVertexShaderSource( PipelineFlags const & flags )const
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

	ShaderPtr SceneRenderPass::getGeometryShaderSource( PipelineFlags const & flags )const
	{
		return doGetGeometryShaderSource( flags );
	}

	ShaderPtr SceneRenderPass::getPixelShaderSource( PipelineFlags const & flags )const
	{
		ShaderPtr result;

		if ( checkFlag( flags.passFlags, PassFlag::eMetallicRoughness ) )
		{
			result = doGetPbrMRPixelShaderSource( flags );
		}
		else if ( checkFlag( flags.passFlags, PassFlag::eSpecularGlossiness ) )
		{
			result = doGetPbrSGPixelShaderSource( flags );
		}
		else
		{
			result = doGetPhongPixelShaderSource( flags );
		}

		return result;
	}

	RenderPipeline * SceneRenderPass::prepareBackPipeline( Pass const & pass
		, TextureFlagsArray const & textures
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, VkPrimitiveTopology topology
		, ashes::PipelineVertexInputStateCreateInfoCRefArray const & vertexLayouts
		, ashes::DescriptorSetLayoutCRefArray descriptorLayouts )
	{
		auto flags = PipelineFlags{ pass.getColourBlendMode()
			, pass.getAlphaBlendMode()
			, pass.getPassFlags()
			, pass.getHeightTextureIndex()
			, programFlags
			, sceneFlags
			, topology
			, pass.getAlphaFunc()
			, pass.getBlendAlphaFunc()
			, textures };
		return doPreparePipeline( vertexLayouts
			, std::move( descriptorLayouts )
			, flags
			, VK_CULL_MODE_BACK_BIT );
	}

	RenderPipeline * SceneRenderPass::prepareFrontPipeline( Pass const & pass
		, TextureFlagsArray const & textures
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, VkPrimitiveTopology topology
		, ashes::PipelineVertexInputStateCreateInfoCRefArray const & vertexLayouts
		, ashes::DescriptorSetLayoutCRefArray descriptorLayouts )
	{
		auto flags = PipelineFlags{ pass.getColourBlendMode()
			, pass.getAlphaBlendMode()
			, pass.getPassFlags()
			, pass.getHeightTextureIndex()
			, programFlags
			, sceneFlags
			, topology
			, pass.getAlphaFunc()
			, pass.getBlendAlphaFunc()
			, textures };
		return doPreparePipeline( vertexLayouts
			, std::move( descriptorLayouts )
			, flags
			, VK_CULL_MODE_FRONT_BIT );
	}

	SubmeshRenderNode * SceneRenderPass::createSkinningNode( Pass & pass
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

	SubmeshRenderNode * SceneRenderPass::createMorphingNode( Pass & pass
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

	SubmeshRenderNode * SceneRenderPass::createStaticNode( Pass & pass
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

	BillboardRenderNode * SceneRenderPass::createBillboardNode( Pass & pass
		, RenderPipeline & pipeline
		, BillboardBase & billboard )
	{
		auto & buffers = billboard.getGeometryBuffers();
		auto & scene = billboard.getParentScene();
		auto billboardEntry = scene.getBillboardListCache().getUbos( billboard
			, pass
			, getInstanceMult() );
		auto it = m_modelsInstances.emplace( billboardEntry.hash
			, billboardEntry.modelInstancesUbo ).first;
		it->second = billboardEntry.modelInstancesUbo;
		m_isDirty = true;

		return &m_renderQueue.getAllRenderNodes().createNode( PassRenderNode{ pass }
			, billboardEntry.modelUbo
			, billboardEntry.modelInstancesUbo
			, buffers
			, *billboard.getNode()
			, billboard
			, billboardEntry.billboardUbo );
	}

	void SceneRenderPass::updatePipeline( RenderPipeline & pipeline )
	{
		doUpdatePipeline( pipeline );
	}

	void SceneRenderPass::updateFlags( PipelineFlags & flags
		, VkCullModeFlags cullMode )const
	{
		doUpdateFlags( flags );

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
	}

	FilteredTextureFlags SceneRenderPass::filterTexturesFlags( TextureFlagsArray const & textures )const
	{
		return castor3d::filterTexturesFlags( textures
			, getTexturesMask() );
	}

	ashes::PipelineColorBlendStateCreateInfo SceneRenderPass::createBlendState( BlendMode colourBlendMode
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

	TextureFlags SceneRenderPass::getTexturesMask()const
	{
		return TextureFlags{ TextureFlag::eAll };
	}

	bool SceneRenderPass::isValidPass( Pass const & pass )const
	{
		return doIsValidPass( pass.getPassFlags() );
	}

	void SceneRenderPass::initialiseAdditionalDescriptor( RenderPipeline & pipeline
		, ashes::DescriptorSetPool const & descriptorPool
		, BillboardRenderNode & node
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		auto & layout = descriptorPool.getLayout();
		auto descriptorSet = descriptorPool.createDescriptorSet( getName() + "_BillboardAdd"
			, RenderPipeline::eAdditional );
		ashes::WriteDescriptorSetArray descriptorWrites;
		descriptorWrites.push_back( m_matrixUbo.getDescriptorWrite( uint32_t( PassUboIdx::eMatrix ) ) );
		descriptorWrites.push_back( m_sceneUbo.getDescriptorWrite( uint32_t( PassUboIdx::eScene ) ) );
		doFillAdditionalDescriptor( pipeline
			, descriptorWrites
			, node
			, shadowMaps );
		descriptorSet->setBindings( descriptorWrites );
		descriptorSet->update();
		pipeline.setAdditionalDescriptorSet( node, std::move( descriptorSet ) );
	}

	void SceneRenderPass::initialiseAdditionalDescriptor( RenderPipeline & pipeline
		, ashes::DescriptorSetPool const & descriptorPool
		, SubmeshRenderNode & node
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		auto & layout = descriptorPool.getLayout();
		auto descriptorSet = descriptorPool.createDescriptorSet( getName() + "_" + node.instance.getName() + "Add"
			, RenderPipeline::eAdditional );
		ashes::WriteDescriptorSetArray descriptorWrites;
		descriptorWrites.push_back( m_matrixUbo.getDescriptorWrite( uint32_t( PassUboIdx::eMatrix ) ) );
		descriptorWrites.push_back( m_sceneUbo.getDescriptorWrite( uint32_t( PassUboIdx::eScene ) ) );
		doFillAdditionalDescriptor( pipeline
			, descriptorWrites
			, node
			, shadowMaps );
		descriptorSet->setBindings( descriptorWrites );
		descriptorSet->update();
		pipeline.setAdditionalDescriptorSet( node, std::move( descriptorSet ) );
	}

	void SceneRenderPass::initialiseAdditionalDescriptor( RenderPipeline & pipeline
		, ashes::DescriptorSetPool const & descriptorPool
		, SubmeshRenderNodesByPassMap & nodes
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		for ( auto & passNodes : nodes )
		{
			for ( auto & submeshNodes : passNodes.second )
			{
				initialiseAdditionalDescriptor( pipeline
					, descriptorPool
					, *submeshNodes.second.begin()->second
					, shadowMaps );
			}
		}
	}

	void SceneRenderPass::doSubInitialise( uint32_t index )
	{
		if ( index == 0u )
		{
			m_renderQueue.initialise();
		}
	}

	void SceneRenderPass::doSubRecordInto( VkCommandBuffer commandBuffer
		, uint32_t index )
	{
		VkCommandBuffer secondary = getCommandBuffer();
		m_context.vkCmdExecuteCommands( commandBuffer
			, 1u
			, &secondary );
	}

	uint32_t SceneRenderPass::doCopyNodesMatrices( SubmeshRenderNodePtrArray const & renderNodes
		, std::vector< InstantiationData > & matrixBuffer )const
	{
		auto const count = std::min( uint32_t( matrixBuffer.size() / m_instanceMult )
			, uint32_t( renderNodes.size() ) );
		auto buffer = matrixBuffer.data();
		auto it = renderNodes.begin();
		auto i = 0u;

		while ( i < count )
		{
			auto & node = *it;

			for ( auto inst = 0u; inst < m_instanceMult; ++inst )
			{
				buffer->m_matrix = node->sceneNode.getDerivedTransformationMatrix();
				buffer->m_material = node->passNode.pass.getId();
				++buffer;
			}

			++i;
			++it;
		}

		return count;
	}

	uint32_t SceneRenderPass::doCopyNodesMatrices( SubmeshRenderNodePtrArray const & renderNodes
		, std::vector< InstantiationData > & matrixBuffer
		, RenderInfo & info )const
	{
		auto count = this->doCopyNodesMatrices( renderNodes, matrixBuffer );
		info.m_visibleObjectsCount += count;
		return count;
	}

	uint32_t SceneRenderPass::doCopyNodesBones( SubmeshRenderNodePtrArray const & renderNodes
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

		bonesBuffer.update( 0u, stride * count );
		return count;
	}

	uint32_t SceneRenderPass::doCopyNodesBones( SubmeshRenderNodePtrArray const & renderNodes
		, ShaderBuffer & bonesBuffer
		, RenderInfo & info )const
	{
		auto count = doCopyNodesBones( renderNodes, bonesBuffer );
		info.m_visibleObjectsCount += count;
		return count;
	}

	void SceneRenderPass::doUpdate( SubmeshRenderNodesPtrByPipelineMap & nodes )
	{
		traverseNodes( nodes
			, [this]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, InstantiationComponent & instantiation
				, SubmeshRenderNodePtrArray & renderNodes )
			{
				auto it = instantiation.find( pass.getOwner()->shared_from_this(), m_instanceMult );
				auto index = instantiation.getIndex( m_instanceMult );

				if ( !renderNodes.empty()
					&& it != instantiation.end()
					&& it->second[index].buffer )
				{
					doCopyNodesMatrices( renderNodes
						, it->second[index].data );

					if ( renderNodes.front()->skeleton )
					{
						auto & instantiatedBones = submesh.getInstantiatedBones();

						if ( instantiatedBones.hasInstancedBonesBuffer() )
						{
							doCopyNodesBones( renderNodes, instantiatedBones.getInstancedBonesBuffer() );
						}
					}
				}
			} );
	}

	void SceneRenderPass::doUpdate( SubmeshRenderNodesPtrByPipelineMap & nodes
		, RenderInfo & info )
	{
		traverseNodes( nodes
			, [this, &info]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, InstantiationComponent & instantiation
				, SubmeshRenderNodePtrArray & renderNodes )
			{
				auto it = instantiation.find( pass.getOwner()->shared_from_this(), m_instanceMult );
				auto index = instantiation.getIndex( m_instanceMult );

				if ( !renderNodes.empty()
					&& it != instantiation.end()
					&& it->second[index].buffer )
				{
					uint32_t count1 = doCopyNodesMatrices( renderNodes
						, it->second[index].data );
					info.m_visibleFaceCount += submesh.getFaceCount() * count1;
					info.m_visibleVertexCount += submesh.getPointsCount() * count1;
					++info.m_drawCalls;

					if ( renderNodes.front()->skeleton )
					{
						auto & instantiatedBones = submesh.getInstantiatedBones();

						if ( instantiatedBones.hasInstancedBonesBuffer() )
						{
							uint32_t count2 = doCopyNodesBones( renderNodes, instantiatedBones.getInstancedBonesBuffer() );
							CU_Require( count1 == count2 );
						}
					}
				}
			} );
	}

	namespace
	{
		template< typename NodeT >
		inline void renderNonInstanced( SceneRenderPass & pass
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

	void SceneRenderPass::doUpdate( SubmeshRenderNodePtrByPipelineMap & nodes )
	{
		renderNonInstanced( *this, nodes );
	}

	void SceneRenderPass::doUpdate( SubmeshRenderNodePtrByPipelineMap & nodes
		, RenderInfo & info )
	{
		doUpdate( nodes );
		doUpdateInfos( nodes, info );
	}

	void SceneRenderPass::doUpdate( BillboardRenderNodePtrByPipelineMap & nodes )
	{
		renderNonInstanced( *this
			, nodes );
	}

	void SceneRenderPass::doUpdate( BillboardRenderNodePtrByPipelineMap & nodes
		, RenderInfo & info )
	{
		doUpdate( nodes );
		doUpdateInfos( nodes, info );
	}

	void SceneRenderPass::doUpdate( RenderQueueArray & queues )
	{
		queues.emplace_back( m_renderQueue );
	}

	void SceneRenderPass::doUpdateUbos( CpuUpdater & updater )
	{
		auto & camera = *updater.camera;
		auto jitterProjSpace = updater.jitter * 2.0f;
		jitterProjSpace[0] /= camera.getWidth();
		jitterProjSpace[1] /= camera.getHeight();
		m_matrixUbo.cpuUpdate( camera.getView()
			, camera.getProjection()
			, jitterProjSpace );
		m_sceneUbo.cpuUpdate( *camera.getScene(), &camera );

		if ( getInstanceMult() > 1
			&& !m_modelsInstances.empty() )
		{
			for ( size_t i = 0; i < size_t( RenderMode::eCount ); ++i )
			{
				updateInstancesUbos( m_modelsInstances, getCuller().getCulledSubmeshes( RenderMode( i ) ), getInstanceMult() );
				updateInstancesUbos( m_modelsInstances, getCuller().getCulledBillboards( RenderMode( i ) ), getInstanceMult() );
			}
		}
	}

	bool SceneRenderPass::doIsValidPass( PassFlags const & passFlags )const
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

	ShaderProgramSPtr SceneRenderPass::doGetProgram( PipelineFlags & flags
		, VkCullModeFlags cullMode )
	{
		updateFlags( flags, cullMode );
		return getEngine()->getShaderProgramCache().getAutomaticProgram( *this, flags );
	}

	ashes::VkDescriptorSetLayoutBindingArray SceneRenderPass::doCreateAdditionalBindings( PipelineFlags const & flags )const
	{
		ashes::VkDescriptorSetLayoutBindingArray addBindings;
		addBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( PassUboIdx::eMatrix )
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, ( VK_SHADER_STAGE_FRAGMENT_BIT
				| ( checkFlag( flags.programFlags, ProgramFlag::eHasGeometry )
					? VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_VERTEX_BIT
					: VK_SHADER_STAGE_VERTEX_BIT ) ) ) );
		addBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( PassUboIdx::eScene )
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, ( VK_SHADER_STAGE_FRAGMENT_BIT
				| ( checkFlag( flags.programFlags, ProgramFlag::eHasGeometry )
					? VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_VERTEX_BIT
					: VK_SHADER_STAGE_VERTEX_BIT ) ) ) );
		doFillAdditionalBindings( flags, addBindings );
		return addBindings;
	}

	std::vector< RenderPipelineUPtr > & SceneRenderPass::doGetFrontPipelines()
	{
		return m_frontPipelines;
	}

	std::vector< RenderPipelineUPtr > & SceneRenderPass::doGetBackPipelines()
	{
		return m_backPipelines;
	}

	std::vector< RenderPipelineUPtr > const & SceneRenderPass::doGetFrontPipelines()const
	{
		return m_frontPipelines;
	}

	std::vector< RenderPipelineUPtr > const & SceneRenderPass::doGetBackPipelines()const
	{
		return m_backPipelines;
	}

	RenderPipeline * SceneRenderPass::doPreparePipeline( ashes::PipelineVertexInputStateCreateInfoCRefArray const & vertexLayouts
		, ashes::DescriptorSetLayoutCRefArray descriptorLayouts
		, PipelineFlags & flags
		, VkCullModeFlags cullMode )
	{
		RenderPipeline * result{};
		auto program = doGetProgram( flags, cullMode );

		if ( doIsValidPass( flags.passFlags )
			&& ( !checkFlag( flags.programFlags, ProgramFlag::eBillboards )
				|| !isShadowMapProgram( flags.programFlags ) ) )
		{
			if ( m_mode != RenderMode::eTransparentOnly )
			{
				flags.alphaBlendMode = BlendMode::eNoBlend;
			}

			auto & pipelines = checkFlag( cullMode, VK_CULL_MODE_FRONT_BIT )
				? doGetFrontPipelines()
				: doGetBackPipelines();
			auto it = findPipeline( flags, pipelines );

			if ( it == pipelines.end() )
			{
				auto & renderSystem = *getEngine()->getRenderSystem();
				auto dsState = doCreateDepthStencilState( flags );
				auto bdState = doCreateBlendState( flags );
				auto pipeline = castor::makeUnique< RenderPipeline >( *this
					, renderSystem
					, std::move( dsState )
					, ashes::PipelineRasterizationStateCreateInfo{ 0u, false, false, VK_POLYGON_MODE_FILL, cullMode }
					, std::move( bdState )
					, ashes::PipelineMultisampleStateCreateInfo{}
					, program
					, flags );
				pipeline->setVertexLayouts( vertexLayouts );
				pipeline->setViewport( makeViewport( m_size ) );

				if ( !checkFlag( flags.programFlags, ProgramFlag::ePicking ) )
				{
					pipeline->setScissor( makeScissor( m_size ) );
				}

				auto & device = *renderSystem.getMainRenderDevice();
				auto addBindings = doCreateAdditionalBindings( flags );
				pipeline->setDescriptorSetLayout( device->createDescriptorSetLayout( getName() + "Add"
					, std::move( addBindings ) ) );
				pipeline->initialise( device
					, getRenderPass()
					, std::move( descriptorLayouts ) );
				pipelines.emplace_back( std::move( pipeline ) );
				it = pipelines.begin() + ( pipelines.size() - 1u );
			}

			result = it->get();
		}

		return result;
	}

	SubmeshRenderNode * SceneRenderPass::doCreateSubmeshNode( Pass & pass
		, RenderPipeline & pipeline
		, Submesh & submesh
		, Geometry & primitive
		, AnimatedMesh * mesh
		, AnimatedSkeleton * skeleton )
	{
		auto & buffers = submesh.getGeometryBuffers( getShaderFlags()
			, pass.getOwner()->shared_from_this()
			, getInstanceMult()
			, pass.getTexturesMask() );
		auto & scene = *primitive.getScene();
		auto geometryEntry = scene.getGeometryCache().getUbos( primitive
			, submesh
			, pass
			, getInstanceMult() );
		auto it = m_modelsInstances.emplace( geometryEntry.hash
			, geometryEntry.modelInstancesUbo ).first;
		it->second = geometryEntry.modelInstancesUbo;
		m_isDirty = true;

		auto & result = m_renderQueue.getAllRenderNodes().createNode( PassRenderNode{ pass }
			, geometryEntry.modelUbo
			, geometryEntry.modelInstancesUbo
			, buffers
			, *primitive.getParent()
			, submesh
			, primitive
			, mesh
			, skeleton );

		if ( mesh )
		{
			CU_Require( result.mesh );
			auto animationEntry = scene.getAnimatedObjectGroupCache().getUbos( *mesh );
			result.morphingUbo = animationEntry.morphingUbo;
		}

		if ( skeleton )
		{
			CU_Require( result.skeleton );
			auto animationEntry = scene.getAnimatedObjectGroupCache().getUbos( *skeleton );
			result.skinningUbo = animationEntry.skinningUbo;
		}

		return &result;
	}

	ShaderPtr SceneRenderPass::doGetVertexShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		VertexWriter writer;
		bool hasTextures = !flags.textures.empty();

		// Vertex inputs
		shader::VertexSurface inSurface{ writer
			, flags.programFlags
			, getShaderFlags()
			, hasTextures };
		auto in = writer.getIn();

		UBO_MODEL( writer
			, uint32_t( NodeUboIdx::eModel )
			, RenderPipeline::eBuffers );
		auto skinningData = SkinningUbo::declare( writer
			, uint32_t( NodeUboIdx::eSkinningUbo )
			, uint32_t( NodeUboIdx::eSkinningSsbo )
			, RenderPipeline::eBuffers
			, flags.programFlags );
		UBO_MORPHING( writer
			, uint32_t( NodeUboIdx::eMorphing )
			, RenderPipeline::eBuffers
			, flags.programFlags );

		UBO_MATRIX( writer
			, uint32_t( PassUboIdx::eMatrix )
			, RenderPipeline::eAdditional );
		UBO_SCENE( writer
			, uint32_t( PassUboIdx::eScene )
			, RenderPipeline::eAdditional );

		// Outputs
		shader::OutFragmentSurface outSurface{ writer
			, getShaderFlags()
			, hasTextures };
		auto out = writer.getOut();

		writer.implementFunction< sdw::Void >( "main"
			, [&]()
			{
				auto curPosition = writer.declLocale( "curPosition"
					, inSurface.position );
				auto v4Normal = writer.declLocale( "v4Normal"
					, vec4( inSurface.normal, 0.0_f ) );
				auto v4Tangent = writer.declLocale( "v4Tangent"
					, vec4( inSurface.tangent, 0.0_f ) );
				outSurface.texture = inSurface.texture;
				inSurface.morph( c3d_morphingData
					, curPosition
					, v4Normal
					, v4Tangent
					, outSurface.texture );
				outSurface.material = c3d_modelData.getMaterialIndex( flags.programFlags
					, inSurface.material );
				outSurface.instance = writer.cast< UInt >( in.instanceIndex );

				auto curMtxModel = writer.declLocale< Mat4 >( "curMtxModel"
					, c3d_modelData.getCurModelMtx( flags.programFlags, skinningData, inSurface ) );
				auto prvMtxModel = writer.declLocale< Mat4 >( "prvMtxModel"
					, c3d_modelData.getPrvModelMtx( flags.programFlags, curMtxModel ) );
				auto prvPosition = writer.declLocale( "prvPosition"
					, prvMtxModel * curPosition );
				curPosition = curMtxModel * curPosition;
				outSurface.worldPosition = curPosition.xyz();
				outSurface.computeVelocity( c3d_matrixData
					, curPosition
					, prvPosition );
				out.vtx.position = curPosition;

				auto mtxNormal = writer.declLocale< Mat3 >( "mtxNormal"
					, c3d_modelData.getNormalMtx( flags.programFlags, curMtxModel ) );
				outSurface.computeTangentSpace( flags.programFlags
					, c3d_sceneData.getCameraPosition()
					, mtxNormal
					, v4Normal
					, v4Tangent );

			} );
		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr SceneRenderPass::doGetBillboardShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		VertexWriter writer;
		bool hasTextures = !flags.textures.empty();

		// Shader inputs
		auto position = writer.declInput< Vec4 >( "position", 0u );
		auto uv = writer.declInput< Vec2 >( "uv", 1u, hasTextures );
		auto center = writer.declInput< Vec3 >( "center", 2u );
		auto in = writer.getIn();

		UBO_MODEL( writer
			, uint32_t( NodeUboIdx::eModel )
			, RenderPipeline::eBuffers );
		UBO_BILLBOARD( writer
			, uint32_t( NodeUboIdx::eBillboard )
			, RenderPipeline::eBuffers );

		UBO_MATRIX( writer
			, uint32_t( PassUboIdx::eMatrix )
			, RenderPipeline::eAdditional );
		UBO_SCENE( writer
			, uint32_t( PassUboIdx::eScene )
			, RenderPipeline::eAdditional );

		// Shader outputs
		shader::OutFragmentSurface outSurface{ writer
			, getShaderFlags()
			, hasTextures };
		auto out = writer.getOut();

		writer.implementFunction< Void >( "main"
			, [&]()
			{
				auto curBbcenter = writer.declLocale( "curBbcenter"
					, c3d_modelData.modelToCurWorld( vec4( center, 1.0_f ) ).xyz() );
				auto prvBbcenter = writer.declLocale( "prvBbcenter" 
					, c3d_modelData.modelToPrvWorld( vec4( center, 1.0_f ) ).xyz() );
				auto curToCamera = writer.declLocale( "curToCamera"
					, c3d_sceneData.getPosToCamera( curBbcenter ) );
				curToCamera.y() = 0.0_f;
				curToCamera = normalize( curToCamera );

				auto right = writer.declLocale( "right"
					, c3d_billboardData.getCameraRight( flags.programFlags, c3d_matrixData ) );
				auto up = writer.declLocale( "up"
					, c3d_billboardData.getCameraUp( flags.programFlags, c3d_matrixData ) );
				auto width = writer.declLocale( "width"
					, c3d_billboardData.getWidth( flags.programFlags, c3d_sceneData ) );
				auto height = writer.declLocale( "height"
					, c3d_billboardData.getHeight( flags.programFlags, c3d_sceneData ) );
				outSurface.worldPosition = curBbcenter
					+ right * position.x() * width
					+ up * position.y() * height;

				if ( hasTextures )
				{
					outSurface.texture = vec3( uv, 0.0_f );
				}

				outSurface.material = c3d_modelData.getMaterialIndex();
				outSurface.instance = writer.cast< UInt >( in.instanceIndex );

				auto prvPosition = writer.declLocale( "prvPosition"
					, vec4( prvBbcenter + right * position.x() * width + up * position.y() * height, 1.0_f ) );
				auto curPosition = writer.declLocale( "curPosition"
					, vec4( outSurface.worldPosition, 1.0_f ) );
				outSurface.computeVelocity( c3d_matrixData
					, curPosition
					, prvPosition );
				out.vtx.position = curPosition;

				outSurface.computeTangentSpace( c3d_sceneData.getCameraPosition()
					, curToCamera
					, up
					, right );
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}
}
