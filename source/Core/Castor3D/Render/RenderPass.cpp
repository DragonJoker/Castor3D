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
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Node/RenderNode_Render.hpp"
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

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
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
	}

	//*********************************************************************************************

	SceneRenderPass::SceneRenderPass( RenderDevice const & device
		, String const & category
		, String const & name
		, SceneRenderPassDesc const & desc
		, ashes::RenderPassPtr renderPass )
		: OwnedBy< Engine >{ *device.renderSystem.getEngine() }
		, Named{ name }
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
		, m_renderPass{ std::move( renderPass ) }
		, m_ownTimer{ ( desc.m_timer
			? nullptr
			: std::make_shared< RenderPassTimer >( m_device, m_category, name ) ) }
		, m_timer{ ( desc.m_timer
			? desc.m_timer
			: m_ownTimer.get() ) }
		, m_index{ desc.m_index }
		, m_instanceMult{ desc.m_instanceMult }
		, m_shaderCache{ makeCache( *getEngine() ) }
	{
		m_sceneUbo.setWindowSize( m_size );
		m_culler.getScene().getGeometryCache().registerPass( *this );
		m_culler.getScene().getBillboardListCache().registerPass( *this );
	}

	SceneRenderPass::~SceneRenderPass()
	{
		m_renderPass.reset();
		m_renderQueue.cleanup();
		m_ownTimer.reset();
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

	void SceneRenderPass::prepareBackPipeline( PipelineFlags & flags
		, ashes::PipelineVertexInputStateCreateInfoCRefArray const & layouts )
	{
		doUpdateFlags( flags );
		remFlag( flags.programFlags, ProgramFlag::eInvertNormals );

		if ( doIsValidPass( flags.passFlags )
			&& ( !checkFlag( flags.programFlags, ProgramFlag::eBillboards )
				|| !isShadowMapProgram( flags.programFlags ) ) )
		{
			if ( m_mode != RenderMode::eTransparentOnly )
			{
				flags.alphaBlendMode = BlendMode::eNoBlend;
			}

			doPrepareBackPipeline( getShaderProgramCache().getAutomaticProgram( *this, flags )
				, layouts
				, flags );
		}
	}

	PipelineFlags SceneRenderPass::prepareBackPipeline( BlendMode colourBlendMode
		, BlendMode alphaBlendMode
		, VkCompareOp alphaFunc
		, VkCompareOp blendAlphaFunc
		, PassFlags const & passFlags
		, TextureFlagsArray const & textures
		, uint32_t heightMapIndex
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, VkPrimitiveTopology topology
		, ashes::PipelineVertexInputStateCreateInfoCRefArray const & layouts )
	{
		auto flags = PipelineFlags{ colourBlendMode
			, alphaBlendMode
			, passFlags
			, heightMapIndex
			, programFlags
			, sceneFlags
			, topology
			, alphaFunc
			, blendAlphaFunc
			, textures };
		prepareBackPipeline( flags, layouts );
		return flags;
	}

	void SceneRenderPass::prepareFrontPipeline( PipelineFlags & flags
		, ashes::PipelineVertexInputStateCreateInfoCRefArray const & layouts )
	{
		doUpdateFlags( flags );
		addFlag( flags.programFlags, ProgramFlag::eInvertNormals );

		if ( doIsValidPass( flags.passFlags )
			&& ( !checkFlag( flags.programFlags, ProgramFlag::eBillboards )
				|| !isShadowMapProgram( flags.programFlags ) ) )
		{
			if ( m_mode != RenderMode::eTransparentOnly )
			{
				flags.alphaBlendMode = BlendMode::eNoBlend;
			}

			doPrepareFrontPipeline( getShaderProgramCache().getAutomaticProgram( *this, flags )
				, layouts
				, flags );
		}
	}

	PipelineFlags SceneRenderPass::prepareFrontPipeline( BlendMode colourBlendMode
		, BlendMode alphaBlendMode
		, VkCompareOp alphaFunc
		, VkCompareOp blendAlphaFunc
		, PassFlags const & passFlags
		, TextureFlagsArray const & textures
		, uint32_t heightMapIndex
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, VkPrimitiveTopology topology
		, ashes::PipelineVertexInputStateCreateInfoCRefArray const & layouts )
	{
		auto flags = PipelineFlags{ colourBlendMode
			, alphaBlendMode
			, passFlags
			, heightMapIndex
			, programFlags
			, sceneFlags
			, topology
			, alphaFunc
			, blendAlphaFunc
			, textures };
		prepareFrontPipeline( flags, layouts );
		return flags;
	}

	RenderPipeline * SceneRenderPass::getPipelineFront( PipelineFlags flags )const
	{
		if ( m_mode != RenderMode::eTransparentOnly )
		{
			flags.alphaBlendMode = BlendMode::eNoBlend;
		}

		auto & pipelines = doGetFrontPipelines();
		auto it = pipelines.find( flags );
		RenderPipeline * result{ nullptr };

		if ( it != pipelines.end() )
		{
			result = it->second.get();
		}

		return result;
	}

	RenderPipeline * SceneRenderPass::getPipelineBack( PipelineFlags flags )const
	{
		if ( m_mode != RenderMode::eTransparentOnly )
		{
			flags.alphaBlendMode = BlendMode::eNoBlend;
		}

		auto & pipelines = doGetBackPipelines();
		auto it = pipelines.find( flags );
		RenderPipeline * result{ nullptr };

		if ( it != pipelines.end() )
		{
			result = it->second.get();
		}

		return result;
	}

	SkinningRenderNode SceneRenderPass::createSkinningNode( Pass & pass
		, RenderPipeline & pipeline
		, Submesh & submesh
		, Geometry & primitive
		, AnimatedSkeleton & skeleton )
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
		auto animationEntry = scene.getAnimatedObjectGroupCache().getUbos( skeleton );
		auto it = m_modelsInstances.emplace( geometryEntry.hash
			, geometryEntry.modelInstancesUbo ).first;
		it->second = geometryEntry.modelInstancesUbo;
		m_isDirty = true;

		return SkinningRenderNode{ doCreatePassRenderNode( pass, pipeline )
			, geometryEntry.modelUbo
			, geometryEntry.modelInstancesUbo
			, buffers
			, *primitive.getParent()
			, submesh
			, primitive
			, skeleton
			, animationEntry.skinningUbo };
	}

	MorphingRenderNode SceneRenderPass::createMorphingNode( Pass & pass
		, RenderPipeline & pipeline
		, Submesh & submesh
		, Geometry & primitive
		, AnimatedMesh & mesh )
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
		auto animationEntry = scene.getAnimatedObjectGroupCache().getUbos( mesh );
		auto it = m_modelsInstances.emplace( geometryEntry.hash
			, geometryEntry.modelInstancesUbo ).first;
		it->second = geometryEntry.modelInstancesUbo;
		m_isDirty = true;

		return MorphingRenderNode{ doCreatePassRenderNode( pass, pipeline )
			, geometryEntry.modelUbo
			, geometryEntry.modelInstancesUbo
			, buffers
			, *primitive.getParent()
			, submesh
			, primitive
			, mesh
			, animationEntry.morphingUbo };
	}

	StaticRenderNode SceneRenderPass::createStaticNode( Pass & pass
		, RenderPipeline & pipeline
		, Submesh & submesh
		, Geometry & primitive )
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

		return StaticRenderNode{ doCreatePassRenderNode( pass, pipeline )
			, geometryEntry.modelUbo
			, geometryEntry.modelInstancesUbo
			, buffers
			, *primitive.getParent()
			, submesh
			, primitive };
	}

	BillboardRenderNode SceneRenderPass::createBillboardNode( Pass & pass
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

		return BillboardRenderNode{ doCreatePassRenderNode( pass, pipeline )
			, billboardEntry.modelUbo
			, billboardEntry.billboardUbo
			, billboardEntry.modelInstancesUbo
			, buffers
			, *billboard.getNode()
			, billboard };
	}

	void SceneRenderPass::updatePipeline( RenderPipeline & pipeline )
	{
		doUpdatePipeline( pipeline );
	}

	void SceneRenderPass::updateFlags( PipelineFlags & flags )const
	{
		doUpdateFlags( flags );
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

	namespace
	{
		template< typename RenderNodeT >
		void initialiseCommonUboDescriptor( Engine & engine
			, RenderPipeline const & pipeline
			, ashes::DescriptorSetLayout const & layout
			, RenderNodeT & node
			, ashes::DescriptorSet & descriptorSet
			, MatrixUbo & matrixUbo
			, SceneUbo const & sceneUbo )
		{
			engine.getMaterialCache().getPassBuffer().createBinding( descriptorSet
				, layout.getBinding( uint32_t( NodeUboIdx::eMaterials ) ) );

			if ( !pipeline.getFlags().textures.empty() )
			{
				engine.getMaterialCache().getTextureBuffer().createBinding( descriptorSet
					, layout.getBinding( uint32_t( NodeUboIdx::eTextures ) ) );
			}

			if ( checkFlag( pipeline.getFlags().programFlags, ProgramFlag::eInstanceMult ) )
			{
				CU_Require( node.modelInstancesUbo );
				node.modelInstancesUbo.createSizedBinding( descriptorSet
					, layout.getBinding( uint32_t( NodeUboIdx::eModelInstances ) ) );
			}

			node.modelUbo.createSizedBinding( descriptorSet
				, layout.getBinding( uint32_t( NodeUboIdx::eModel ) ) );
		}
	}

	void SceneRenderPass::initialiseUboDescriptor( RenderPipeline const & pipeline
		, ashes::DescriptorSetPool const & descriptorPool
		, BillboardRenderNode & node )
	{
		auto & layout = descriptorPool.getLayout();
		node.uboDescriptorSet = descriptorPool.createDescriptorSet( getName() + "_BillboardUbo"
			, RenderPipeline::eBuffers );
		auto & descriptorSet = *node.uboDescriptorSet;
		initialiseCommonUboDescriptor( *getEngine()
			, pipeline
			, layout
			, node
			, descriptorSet
			, m_matrixUbo
			, m_sceneUbo );
		node.billboardUbo.createSizedBinding( descriptorSet
			, layout.getBinding( uint32_t( NodeUboIdx::eBillboard ) ) );
		descriptorSet.update();
	}

	void SceneRenderPass::initialiseUboDescriptor( RenderPipeline const & pipeline
		, ashes::DescriptorSetPool const & descriptorPool
		, MorphingRenderNode & node )
	{
		auto & layout = descriptorPool.getLayout();
		node.uboDescriptorSet = descriptorPool.createDescriptorSet( getName() + "_" + node.instance.getName() + "Ubo"
			, RenderPipeline::eBuffers );
		auto & descriptorSet = *node.uboDescriptorSet;
		initialiseCommonUboDescriptor( *getEngine()
			, pipeline
			, layout
			, node
			, descriptorSet
			, m_matrixUbo
			, m_sceneUbo );
		node.morphingUbo.createSizedBinding( descriptorSet
			, layout.getBinding( uint32_t( NodeUboIdx::eMorphing ) ) );
		descriptorSet.update();
	}

	void SceneRenderPass::initialiseUboDescriptor( RenderPipeline const & pipeline
		, ashes::DescriptorSetPool const & descriptorPool
		, SkinningRenderNode & node )
	{
		auto & layout = descriptorPool.getLayout();
		node.uboDescriptorSet = descriptorPool.createDescriptorSet( getName() + "_" + node.instance.getName() + "Ubo"
			, RenderPipeline::eBuffers );
		auto & descriptorSet = *node.uboDescriptorSet;
		initialiseCommonUboDescriptor( *getEngine()
			, pipeline
			, layout
			, node
			, descriptorSet
			, m_matrixUbo
			, m_sceneUbo );

		if ( checkFlag( pipeline.getFlags().programFlags, ProgramFlag::eInstantiation ) )
		{
			node.data.getInstantiatedBones().getInstancedBonesBuffer().createBinding( descriptorSet
				, layout.getBinding( uint32_t( NodeUboIdx::eSkinning ) ) );
		}
		else
		{
			node.skinningUbo.createSizedBinding( descriptorSet
				, layout.getBinding( uint32_t( NodeUboIdx::eSkinning ) ) );
		}

		descriptorSet.update();
	}

	void SceneRenderPass::initialiseUboDescriptor( RenderPipeline const & pipeline
		, ashes::DescriptorSetPool const & descriptorPool
		, StaticRenderNode & node )
	{
		auto & layout = descriptorPool.getLayout();
		node.uboDescriptorSet = descriptorPool.createDescriptorSet( getName() + "_" + node.instance.getName() + "Ubo"
			, RenderPipeline::eBuffers );
		auto & descriptorSet = *node.uboDescriptorSet;
		initialiseCommonUboDescriptor( *getEngine()
			, pipeline
			, layout
			, node
			, descriptorSet
			, m_matrixUbo
			, m_sceneUbo );
		descriptorSet.update();
	}

	void SceneRenderPass::initialiseUboDescriptor( RenderPipeline const & pipeline
		, ashes::DescriptorSetPool const & descriptorPool
		, SubmeshSkinninRenderNodesByPassMap & nodes )
	{
		for ( auto & passNodes : nodes )
		{
			for ( auto & submeshNodes : passNodes.second )
			{
				initialiseUboDescriptor( pipeline
					, descriptorPool
					, submeshNodes.second.begin()->second );
			}
		}
	}

	void SceneRenderPass::initialiseUboDescriptor( RenderPipeline const & pipeline
		, ashes::DescriptorSetPool const & descriptorPool
		, SubmeshStaticRenderNodesByPassMap & nodes )
	{
		for ( auto & passNodes : nodes )
		{
			for ( auto & submeshNodes : passNodes.second )
			{
				initialiseUboDescriptor( pipeline
					, descriptorPool
					, submeshNodes.second.begin()->second );
			}
		}
	}

	void SceneRenderPass::initialiseTextureDescriptor( RenderPipeline const & pipeline
		, ashes::DescriptorSetPool const & descriptorPool
		, BillboardRenderNode & node )
	{
		uint32_t index = 0u;
		node.texDescriptorSet = descriptorPool.createDescriptorSet( getName() + "_BillboardTex"
			, RenderPipeline::eTextures );
		auto & descriptorSet = *node.texDescriptorSet;

		if ( !pipeline.getFlags().textures.empty() )
		{
			node.passNode.fillDescriptor( descriptorSet.getLayout()
				, index
				, descriptorSet
				, pipeline.getFlags().textures );
		}

		descriptorSet.update();
	}

	void SceneRenderPass::initialiseTextureDescriptor( RenderPipeline const & pipeline
		, ashes::DescriptorSetPool const & descriptorPool
		, MorphingRenderNode & node )
	{
		uint32_t index = 0u;
		node.texDescriptorSet = descriptorPool.createDescriptorSet( getName() + "_" + node.instance.getName() + "Tex"
			, RenderPipeline::eTextures );
		auto & descriptorSet = *node.texDescriptorSet;

		if ( !pipeline.getFlags().textures.empty() )
		{
			node.passNode.fillDescriptor( descriptorSet.getLayout()
				, index
				, descriptorSet
				, pipeline.getFlags().textures );
		}

		descriptorSet.update();
	}

	void SceneRenderPass::initialiseTextureDescriptor( RenderPipeline const & pipeline
		, ashes::DescriptorSetPool const & descriptorPool
		, SkinningRenderNode & node )
	{
		uint32_t index = 0u;
		node.texDescriptorSet = descriptorPool.createDescriptorSet( getName() + "_" + node.instance.getName() + "Tex"
			, RenderPipeline::eTextures );
		auto & descriptorSet = *node.texDescriptorSet;

		if ( !pipeline.getFlags().textures.empty() )
		{
			node.passNode.fillDescriptor( descriptorSet.getLayout()
				, index
				, descriptorSet
				, pipeline.getFlags().textures );
		}

		descriptorSet.update();
	}

	void SceneRenderPass::initialiseTextureDescriptor( RenderPipeline const & pipeline
		, ashes::DescriptorSetPool const & descriptorPool
		, StaticRenderNode & node )
	{
		uint32_t index = 0u;
		node.texDescriptorSet = descriptorPool.createDescriptorSet( getName() + "_" + node.instance.getName() + "Tex"
			, RenderPipeline::eTextures );
		auto & descriptorSet = *node.texDescriptorSet;

		if ( !pipeline.getFlags().textures.empty() )
		{
			node.passNode.fillDescriptor( descriptorSet.getLayout()
				, index
				, descriptorSet
				, pipeline.getFlags().textures );
		}

		descriptorSet.update();
	}

	void SceneRenderPass::initialiseTextureDescriptor( RenderPipeline const & pipeline
		, ashes::DescriptorSetPool const & descriptorPool
		, SubmeshSkinninRenderNodesByPassMap & nodes )
	{
		for ( auto & passNodes : nodes )
		{
			for ( auto & submeshNodes : passNodes.second )
			{
				if ( pipeline.hasDescriptorPool( RenderPipeline::eTextures ) )
				{
					initialiseTextureDescriptor( pipeline
						, descriptorPool
						, submeshNodes.second.begin()->second );
				}
			}
		}
	}

	void SceneRenderPass::initialiseTextureDescriptor( RenderPipeline const & pipeline
		, ashes::DescriptorSetPool const & descriptorPool
		, SubmeshStaticRenderNodesByPassMap & nodes )
	{
		for ( auto & passNodes : nodes )
		{
			for ( auto & submeshNodes : passNodes.second )
			{
				if ( pipeline.hasDescriptorPool( RenderPipeline::eTextures ) )
				{
					initialiseTextureDescriptor( pipeline
						, descriptorPool
						, submeshNodes.second.begin()->second );
				}
			}
		}
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
		, MorphingRenderNode & node
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
		, SkinningRenderNode & node
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
		, StaticRenderNode & node
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
		, SubmeshSkinninRenderNodesByPassMap & nodes
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		for ( auto & passNodes : nodes )
		{
			for ( auto & submeshNodes : passNodes.second )
			{
				initialiseAdditionalDescriptor( pipeline
					, descriptorPool
					, submeshNodes.second.begin()->second
					, shadowMaps );
			}
		}
	}

	void SceneRenderPass::initialiseAdditionalDescriptor( RenderPipeline & pipeline
		, ashes::DescriptorSetPool const & descriptorPool
		, SubmeshStaticRenderNodesByPassMap & nodes
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		for ( auto & passNodes : nodes )
		{
			for ( auto & submeshNodes : passNodes.second )
			{
				initialiseAdditionalDescriptor( pipeline
					, descriptorPool
					, submeshNodes.second.begin()->second
					, shadowMaps );
			}
		}
	}

	PassRenderNode SceneRenderPass::doCreatePassRenderNode( Pass & pass
		, RenderPipeline & pipeline )
	{
		return PassRenderNode
		{
			pass,
		};
	}

	SceneRenderNode SceneRenderPass::doCreateSceneRenderNode( Scene & scene
		, RenderPipeline & pipeline )
	{
		return SceneRenderNode{};
	}

	namespace
	{
		template< typename NodeT >
		uint32_t copyNodesMatrices( std::vector< NodeT * > const & renderNodes
			, std::vector< InstantiationData > & matrixBuffer
			, uint32_t instanceMult )
		{
			auto const count = std::min( uint32_t( matrixBuffer.size() / instanceMult )
				, uint32_t( renderNodes.size() ) );
			auto buffer = matrixBuffer.data();
			auto it = renderNodes.begin();
			auto i = 0u;

			while ( i < count )
			{
				auto & node = *it;

				for ( auto inst = 0u; inst < instanceMult; ++inst )
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
	}

	uint32_t SceneRenderPass::doCopyNodesMatrices( StaticRenderNodePtrArray const & renderNodes
		, std::vector< InstantiationData > & matrixBuffer )const
	{
		return copyNodesMatrices( renderNodes, matrixBuffer, m_instanceMult );
	}

	uint32_t SceneRenderPass::doCopyNodesMatrices( StaticRenderNodePtrArray const & renderNodes
		, std::vector< InstantiationData > & matrixBuffer
		, RenderInfo & info )const
	{
		auto count = this->doCopyNodesMatrices( renderNodes, matrixBuffer );
		info.m_visibleObjectsCount += count;
		return count;
	}

	uint32_t SceneRenderPass::doCopyNodesMatrices( SkinningRenderNodePtrArray const & renderNodes
		, std::vector< InstantiationData > & matrixBuffer )const
	{
		return copyNodesMatrices( renderNodes, matrixBuffer, m_instanceMult );
	}

	uint32_t SceneRenderPass::doCopyNodesMatrices( SkinningRenderNodePtrArray const & renderNodes
		, std::vector< InstantiationData > & matrixBuffer
		, RenderInfo & info )const
	{
		auto count = doCopyNodesMatrices( renderNodes, matrixBuffer );
		info.m_visibleObjectsCount += count;
		return count;
	}

	uint32_t SceneRenderPass::doCopyNodesBones( SkinningRenderNodePtrArray const & renderNodes
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
				node->skeleton.fillBuffer( buffer );
				buffer += stride;
			}

			++i;
			++it;
		}

		bonesBuffer.update( 0u, stride * count );
		return count;
	}

	uint32_t SceneRenderPass::doCopyNodesBones( SkinningRenderNodePtrArray const & renderNodes
		, ShaderBuffer & bonesBuffer
		, RenderInfo & info )const
	{
		auto count = doCopyNodesBones( renderNodes, bonesBuffer );
		info.m_visibleObjectsCount += count;
		return count;
	}

	void SceneRenderPass::doUpdate( SubmeshStaticRenderNodesPtrByPipelineMap & nodes )
	{
		traverseNodes( nodes
			, [this]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, InstantiationComponent & instantiation
				, StaticRenderNodePtrArray & renderNodes )
			{
				auto it = instantiation.find( pass.getOwner()->shared_from_this(), m_instanceMult );
				auto index = instantiation.getIndex( m_instanceMult );

				if ( !renderNodes.empty()
					&& it != instantiation.end()
					&& it->second[index].buffer )
				{
					doCopyNodesMatrices( renderNodes
						, it->second[index].data );
				}
			} );
	}

	void SceneRenderPass::doUpdate( SubmeshStaticRenderNodesPtrByPipelineMap & nodes
		, RenderInfo & info )
	{
		traverseNodes( nodes
			, [this, &info]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, InstantiationComponent & instantiation
				, StaticRenderNodePtrArray & renderNodes )
			{
				auto it = instantiation.find( pass.getOwner()->shared_from_this(), m_instanceMult );
				auto index = instantiation.getIndex( m_instanceMult );

				if ( !renderNodes.empty()
					&& it != instantiation.end()
					&& it->second[index].buffer )
				{
					uint32_t count = doCopyNodesMatrices( renderNodes
						, it->second[index].data
						, info );
					info.m_visibleFaceCount += submesh.getFaceCount() * count;
					info.m_visibleVertexCount += submesh.getPointsCount() * count;
					++info.m_drawCalls;
				}
			} );
	}

	namespace
	{
		template< typename MapType >
		inline void renderNonInstanced( SceneRenderPass & pass
			, MapType & nodes )
		{
			for ( auto & itPipelines : nodes )
			{
				pass.updatePipeline( *itPipelines.first );
			}
		}

		template< typename MapType >
		inline void renderNonInstanced( SceneRenderPass & pass
			, MapType & nodes
			, RenderInfo & info )
		{
			for ( auto & itPipelines : nodes )
			{
				pass.updatePipeline( *itPipelines.first );

				for ( auto & renderNode : itPipelines.second )
				{
					info.m_visibleFaceCount += details::getPrimitiveCount( renderNode->data );
					info.m_visibleVertexCount += details::getVertexCount( renderNode->data );
					++info.m_drawCalls;
					++info.m_visibleObjectsCount;
				}
			}
		}
	}

	void SceneRenderPass::doUpdate( StaticRenderNodePtrByPipelineMap & nodes )
	{
		renderNonInstanced( *this
			, nodes );
	}

	void SceneRenderPass::doUpdate( StaticRenderNodePtrByPipelineMap & nodes
		, RenderInfo & info )
	{
		renderNonInstanced( *this
			, nodes
			, info );
	}

	void SceneRenderPass::doUpdate( SkinningRenderNodePtrByPipelineMap & nodes )
	{
		renderNonInstanced( *this
			, nodes );
	}

	void SceneRenderPass::doUpdate( SkinningRenderNodePtrByPipelineMap & nodes
		, RenderInfo & info )
	{
		renderNonInstanced( *this
			, nodes
			, info );
	}

	void SceneRenderPass::doUpdate( SubmeshSkinningRenderNodesPtrByPipelineMap & nodes )
	{
		traverseNodes( nodes
			, [this]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, InstantiationComponent & instantiation
				, SkinningRenderNodePtrArray & renderNodes )
			{
				auto & instantiatedBones = submesh.getInstantiatedBones();
				auto it = instantiation.find( pass.getOwner()->shared_from_this(), m_instanceMult );
				auto index = instantiation.getIndex( m_instanceMult );

				if ( !renderNodes.empty()
					&& it != instantiation.end()
					&& it->second[index].buffer
					&& instantiatedBones.hasInstancedBonesBuffer() )
				{
					uint32_t count1 = doCopyNodesMatrices( renderNodes, it->second[index].data );
					uint32_t count2 = doCopyNodesBones( renderNodes, instantiatedBones.getInstancedBonesBuffer() );
					CU_Require( count1 == count2 );
				}
			} );
	}

	void SceneRenderPass::doUpdate( SubmeshSkinningRenderNodesPtrByPipelineMap & nodes
		, RenderInfo & info )
	{
		traverseNodes( nodes
			, [this, &info]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, InstantiationComponent & instantiation
				, SkinningRenderNodePtrArray & renderNodes )
			{
				auto & instantiatedBones = submesh.getInstantiatedBones();
				auto it = instantiation.find( pass.getOwner()->shared_from_this(), m_instanceMult );
				auto index = instantiation.getIndex( m_instanceMult );

				if ( !renderNodes.empty()
					&& it != instantiation.end()
					&& it->second[index].buffer
					&& instantiatedBones.hasInstancedBonesBuffer() )
				{
					uint32_t count1 = doCopyNodesMatrices( renderNodes, it->second[index].data, info );
					uint32_t count2 = doCopyNodesBones( renderNodes, instantiatedBones.getInstancedBonesBuffer(), info );
					CU_Require( count1 == count2 );
					info.m_visibleFaceCount += submesh.getFaceCount() * count1;
					info.m_visibleVertexCount += submesh.getPointsCount() * count1;
					++info.m_drawCalls;
				}
			} );
	}

	void SceneRenderPass::doUpdate( MorphingRenderNodePtrByPipelineMap & nodes )
	{
		renderNonInstanced( *this
			, nodes );
	}

	void SceneRenderPass::doUpdate( MorphingRenderNodePtrByPipelineMap & nodes
		, RenderInfo & info )
	{
		renderNonInstanced( *this
			, nodes
			, info );
	}

	void SceneRenderPass::doUpdate( BillboardRenderNodePtrByPipelineMap & nodes )
	{
		renderNonInstanced( *this
			, nodes );
	}

	void SceneRenderPass::doUpdate( BillboardRenderNodePtrByPipelineMap & nodes
		, RenderInfo & info )
	{
		renderNonInstanced( *this
			, nodes
			, info );
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

	ashes::VkDescriptorSetLayoutBindingArray SceneRenderPass::doCreateUboBindings( PipelineFlags const & flags )const
	{
		ashes::VkDescriptorSetLayoutBindingArray uboBindings;
		uboBindings.emplace_back( getEngine()->getMaterialCache().getPassBuffer().createLayoutBinding( uint32_t( NodeUboIdx::eMaterials ) ) );

		if ( !flags.textures.empty() )
		{
			uboBindings.emplace_back( getEngine()->getMaterialCache().getTextureBuffer().createLayoutBinding( uint32_t( NodeUboIdx::eTextures ) ) );
		}

		uboBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( NodeUboIdx::eModel )
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, ( VK_SHADER_STAGE_FRAGMENT_BIT
				| ( checkFlag( flags.programFlags, ProgramFlag::eHasGeometry )
					? VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_VERTEX_BIT
					: VK_SHADER_STAGE_VERTEX_BIT ) ) ) );

		if ( checkFlag( flags.programFlags, ProgramFlag::eBillboards ) )
		{
			uboBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( NodeUboIdx::eBillboard )
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, ( checkFlag( flags.programFlags, ProgramFlag::eHasGeometry )
					? VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_VERTEX_BIT
					: VK_SHADER_STAGE_VERTEX_BIT ) ) );
		}

		if ( checkFlag( flags.programFlags, ProgramFlag::eSkinning ) )
		{
			uboBindings.push_back( SkinningUbo::createLayoutBinding( uint32_t( NodeUboIdx::eSkinning )
				, flags.programFlags ) );
		}

		if ( checkFlag( flags.programFlags, ProgramFlag::eMorphing ) )
		{
			uboBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( NodeUboIdx::eMorphing )
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, ( checkFlag( flags.programFlags, ProgramFlag::eHasGeometry )
					? VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_VERTEX_BIT
					: VK_SHADER_STAGE_VERTEX_BIT ) ) );
		}

		if ( checkFlag( flags.programFlags, ProgramFlag::eInstanceMult ) )
		{
			uboBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( NodeUboIdx::eModelInstances )
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, VK_SHADER_STAGE_VERTEX_BIT ) );
		}

		return uboBindings;
	}

	ashes::VkDescriptorSetLayoutBindingArray SceneRenderPass::doCreateTextureBindings( PipelineFlags const & flags )const
	{
		ashes::VkDescriptorSetLayoutBindingArray texBindings;

		if ( !flags.textures.empty() )
		{
			texBindings.emplace_back( makeDescriptorSetLayoutBinding( 0u
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT
				, uint32_t( flags.textures.size() ) ) );
		}

		return texBindings;
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

	std::map< PipelineFlags, RenderPipelineUPtr > & SceneRenderPass::doGetFrontPipelines()
	{
		return m_frontPipelines;
	}

	std::map< PipelineFlags, RenderPipelineUPtr > & SceneRenderPass::doGetBackPipelines()
	{
		return m_backPipelines;
	}

	std::map< PipelineFlags, RenderPipelineUPtr > const & SceneRenderPass::doGetFrontPipelines()const
	{
		return m_frontPipelines;
	}

	std::map< PipelineFlags, RenderPipelineUPtr > const & SceneRenderPass::doGetBackPipelines()const
	{
		return m_backPipelines;
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

	void SceneRenderPass::doInitialisePipeline( RenderDevice const & device
		, RenderPipeline & pipeline )const
	{
		auto & flags = pipeline.getFlags();
		auto uboBindings = doCreateUboBindings( flags );
		auto texBindings = doCreateTextureBindings( flags );
		auto addBindings = doCreateAdditionalBindings( flags );
		auto uboLayout = device->createDescriptorSetLayout( getName() + "Ubo"
			, std::move( uboBindings ) );
		auto texLayout = device->createDescriptorSetLayout( getName() + "Tex"
			, std::move( texBindings ) );
		auto addLayout = device->createDescriptorSetLayout( getName() + "Add"
			, std::move( addBindings ) );
		std::vector< ashes::DescriptorSetLayoutPtr > dsLayouts;
		dsLayouts.emplace_back( std::move( uboLayout ) );
		dsLayouts.emplace_back( std::move( texLayout ) );
		dsLayouts.emplace_back( std::move( addLayout ) );
		pipeline.setDescriptorSetLayouts( std::move( dsLayouts ) );
		pipeline.initialise( device, getRenderPass() );
	}

	void SceneRenderPass::doPrepareFrontPipeline( ShaderProgramSPtr program
		, ashes::PipelineVertexInputStateCreateInfoCRefArray const & layouts
		, PipelineFlags const & flags )
	{
		auto & pipelines = doGetFrontPipelines();

		if ( pipelines.find( flags ) == pipelines.end() )
		{
			auto dsState = doCreateDepthStencilState( flags );
			auto bdState = doCreateBlendState( flags );
			auto & pipeline = *pipelines.emplace( flags
				, castor::makeUnique< RenderPipeline >( *this
					, *getEngine()->getRenderSystem()
					, std::move( dsState )
					, ashes::PipelineRasterizationStateCreateInfo{ 0u, false, false, VK_POLYGON_MODE_FILL, VK_CULL_MODE_FRONT_BIT }
					, std::move( bdState )
					, ashes::PipelineMultisampleStateCreateInfo{}
					, program
					, flags ) ).first->second;
			pipeline.setVertexLayouts( layouts );
			pipeline.setViewport( makeViewport( m_size ) );

			if ( !checkFlag( flags.programFlags, ProgramFlag::ePicking ) )
			{
				pipeline.setScissor( makeScissor( m_size ) );
			}

			getEngine()->sendEvent( makeGpuFunctorEvent( EventType::ePreRender
				, [this, &pipeline]( RenderDevice const & device )
				{
					doInitialisePipeline( device, pipeline );
				} ) );
		}
	}

	void SceneRenderPass::doPrepareBackPipeline( ShaderProgramSPtr program
		, ashes::PipelineVertexInputStateCreateInfoCRefArray const & layouts
		, PipelineFlags const & flags )
	{
		auto & pipelines = doGetBackPipelines();

		if ( pipelines.find( flags ) == pipelines.end() )
		{
			auto dsState = doCreateDepthStencilState( flags );
			auto bdState = doCreateBlendState( flags );
			auto & pipeline = *pipelines.emplace( flags
				, castor::makeUnique< RenderPipeline >( *this
					, *getEngine()->getRenderSystem()
					, std::move( dsState )
					, ashes::PipelineRasterizationStateCreateInfo{ 0u, false, false, VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT }
					, std::move( bdState )
					, ashes::PipelineMultisampleStateCreateInfo{}
					, program
					, flags ) ).first->second;
			pipeline.setVertexLayouts( layouts );
			pipeline.setViewport( makeViewport( m_size ) );

			if ( !checkFlag( flags.programFlags, ProgramFlag::ePicking ) )
			{
				pipeline.setScissor( makeScissor( m_size ) );
			}

			getEngine()->sendEvent( makeGpuFunctorEvent( EventType::ePreRender
				, [this, &pipeline]( RenderDevice const & device )
				{
					doInitialisePipeline( device, pipeline );
				} ) );
		}
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
			, uint32_t( NodeUboIdx::eSkinning )
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
