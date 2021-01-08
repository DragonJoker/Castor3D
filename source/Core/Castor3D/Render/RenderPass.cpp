#include "Castor3D/Render/RenderPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/PoolUniformBuffer.hpp"
#include "Castor3D/Cache/AnimatedObjectGroupCache.hpp"
#include "Castor3D/Cache/BillboardCache.hpp"
#include "Castor3D/Cache/BillboardUboPools.hpp"
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
#include "Castor3D/Shader/TextureConfigurationBuffer/TextureConfigurationBuffer.hpp"
#include "Castor3D/Shader/Ubos/BillboardUbo.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelInstancesUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelMatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelUbo.hpp"
#include "Castor3D/Shader/Ubos/MorphingUbo.hpp"
#include "Castor3D/Shader/Ubos/PickingUbo.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"
#include "Castor3D/Shader/Ubos/SkinningUbo.hpp"
#include "Castor3D/Shader/Ubos/TexturesUbo.hpp"

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

	RenderPass::RenderPass( String const & category
		, String const & name
		, Engine & engine
		, MatrixUbo & matrixUbo
		, SceneCuller & culler
		, RenderMode mode
		, bool oit
		, SceneNode const * ignored
		, uint32_t instanceMult )
		: OwnedBy< Engine >{ engine }
		, Named{ name }
		, m_renderSystem{ *engine.getRenderSystem() }
		, m_matrixUbo{ matrixUbo }
		, m_culler{ culler }
		, m_renderQueue{ *this, mode, ignored }
		, m_category{ category }
		, m_oit{ oit }
		, m_mode{ mode }
		, m_sceneUbo{ engine }
		, m_instanceMult{ instanceMult }
	{
		m_culler.getScene().getGeometryCache().registerPass( *this );
		m_culler.getScene().getBillboardListCache().registerPass( *this );
	}

	RenderPass::RenderPass( String const & category
		, String const & name
		, Engine & engine
		, MatrixUbo & matrixUbo
		, SceneCuller & culler
		, uint32_t instanceMult )
		: RenderPass{ category, name, engine, matrixUbo, culler, RenderMode::eOpaqueOnly, true, nullptr, instanceMult }
	{
	}

	RenderPass::RenderPass( String const & category
		, String const & name
		, Engine & engine
		, MatrixUbo & matrixUbo
		, SceneCuller & culler
		, bool oit
		, uint32_t instanceMult )
		: RenderPass{ category, name, engine, matrixUbo, culler, RenderMode::eTransparentOnly, oit, nullptr, instanceMult }
	{
	}

	RenderPass::RenderPass( String const & category
		, String const & name
		, Engine & engine
		, MatrixUbo & matrixUbo
		, SceneCuller & culler
		, SceneNode const * ignored
		, uint32_t instanceMult )
		: RenderPass{ category, name, engine, matrixUbo, culler, RenderMode::eOpaqueOnly, true, ignored, instanceMult }
	{
	}

	RenderPass::RenderPass( String const & category
		, String const & name
		, Engine & engine
		, MatrixUbo & matrixUbo
		, SceneCuller & culler
		, bool oit
		, SceneNode const * ignored
		, uint32_t instanceMult )
		: RenderPass{ category, name, engine, matrixUbo, culler, RenderMode::eTransparentOnly, oit, ignored, instanceMult }
	{
	}

	RenderPass::~RenderPass()
	{
	}

	bool RenderPass::initialise( RenderDevice const & device
		, Size const & size )
	{
		m_timer = std::make_shared< RenderPassTimer >( *getEngine(), device, m_category, getName() );
		m_sceneUbo.initialise( device );
		m_sceneUbo.setWindowSize( size );
		m_size = size;
		return doInitialise( device, size );
	}

	void RenderPass::cleanup( RenderDevice const & device )
	{
		m_sceneUbo.cleanup( device );
		m_renderPass.reset();
		doCleanup( device );
		m_timer.reset();
		m_backPipelines.clear();
		m_frontPipelines.clear();
	}

	void RenderPass::update( CpuUpdater & updater )
	{
		doUpdate( *updater.queues );
		doUpdateUbos( updater );
		m_isDirty = false;
	}

	ShaderPtr RenderPass::getVertexShaderSource( PipelineFlags const & flags )const
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

	ShaderPtr RenderPass::getGeometryShaderSource( PipelineFlags const & flags )const
	{
		return doGetGeometryShaderSource( flags );
	}

	ShaderPtr RenderPass::getPixelShaderSource( PipelineFlags const & flags )const
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

	void RenderPass::prepareBackPipeline( PipelineFlags & flags
		, ashes::PipelineVertexInputStateCreateInfoCRefArray const & layouts )
	{
		doUpdateFlags( flags );
		remFlag( flags.programFlags, ProgramFlag::eInvertNormals );

		if ( isValidNodeForPass( flags.passFlags, m_mode )
			&& ( !checkFlag( flags.programFlags, ProgramFlag::eBillboards )
				|| !isShadowMapProgram( flags.programFlags ) ) )
		{
			if ( m_mode != RenderMode::eTransparentOnly )
			{
				flags.alphaBlendMode = BlendMode::eNoBlend;
			}

			auto program = doGetProgram( flags );
			doPrepareBackPipeline( program, layouts, flags );
		}
	}

	PipelineFlags RenderPass::prepareBackPipeline( BlendMode colourBlendMode
		, BlendMode alphaBlendMode
		, VkCompareOp alphaFunc
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
			, textures };
		prepareBackPipeline( flags, layouts );
		return flags;
	}

	void RenderPass::prepareFrontPipeline( PipelineFlags & flags
		, ashes::PipelineVertexInputStateCreateInfoCRefArray const & layouts )
	{
		doUpdateFlags( flags );
		addFlag( flags.programFlags, ProgramFlag::eInvertNormals );

		if ( isValidNodeForPass( flags.passFlags, m_mode )
			&& ( !checkFlag( flags.programFlags, ProgramFlag::eBillboards )
				|| !isShadowMapProgram( flags.programFlags ) ) )
		{
			if ( m_mode != RenderMode::eTransparentOnly )
			{
				flags.alphaBlendMode = BlendMode::eNoBlend;
			}

			auto program = doGetProgram( flags );
			doPrepareFrontPipeline( program, layouts, flags );
		}
	}

	PipelineFlags RenderPass::prepareFrontPipeline( BlendMode colourBlendMode
		, BlendMode alphaBlendMode
		, VkCompareOp alphaFunc
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
			, textures };
		prepareFrontPipeline( flags, layouts );
		return flags;
	}

	RenderPipeline * RenderPass::getPipelineFront( PipelineFlags flags )const
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

	RenderPipeline * RenderPass::getPipelineBack( PipelineFlags flags )const
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

	SkinningRenderNode RenderPass::createSkinningNode( Pass & pass
		, RenderPipeline & pipeline
		, Submesh & submesh
		, Geometry & primitive
		, AnimatedSkeleton & skeleton )
	{
		auto & buffers = submesh.getGeometryBuffers( pass.getOwner()->shared_from_this()
			, getInstanceMult()
			, pass.getTextures( getTexturesMask() ) );
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

		return SkinningRenderNode
		{
			pipeline,
			doCreatePassRenderNode( pass, pipeline ),
			geometryEntry.modelMatrixUbo,
			geometryEntry.modelUbo,
			geometryEntry.pickingUbo,
			geometryEntry.texturesUbo,
			geometryEntry.modelInstancesUbo,
			buffers,
			*primitive.getParent(),
			submesh,
			primitive,
			skeleton,
			animationEntry.skinningUbo
		};
	}

	MorphingRenderNode RenderPass::createMorphingNode( Pass & pass
		, RenderPipeline & pipeline
		, Submesh & submesh
		, Geometry & primitive
		, AnimatedMesh & mesh )
	{
		auto & buffers = submesh.getGeometryBuffers( pass.getOwner()->shared_from_this()
			, getInstanceMult()
			, pass.getTextures( getTexturesMask() ) );
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

		return MorphingRenderNode
		{
			pipeline,
			doCreatePassRenderNode( pass, pipeline ),
			geometryEntry.modelMatrixUbo,
			geometryEntry.modelUbo,
			geometryEntry.pickingUbo,
			geometryEntry.texturesUbo,
			geometryEntry.modelInstancesUbo,
			buffers,
			*primitive.getParent(),
			submesh,
			primitive,
			mesh,
			animationEntry.morphingUbo
		};
	}

	StaticRenderNode RenderPass::createStaticNode( Pass & pass
		, RenderPipeline & pipeline
		, Submesh & submesh
		, Geometry & primitive )
	{
		auto & buffers = submesh.getGeometryBuffers( pass.getOwner()->shared_from_this()
			, getInstanceMult()
			, pass.getTextures( getTexturesMask() ) );
		auto & scene = *primitive.getScene();
		auto geometryEntry = scene.getGeometryCache().getUbos( primitive
			, submesh
			, pass
			, getInstanceMult() );
		auto it = m_modelsInstances.emplace( geometryEntry.hash
			, geometryEntry.modelInstancesUbo ).first;
		it->second = geometryEntry.modelInstancesUbo;
		m_isDirty = true;

		return StaticRenderNode
		{
			pipeline,
			doCreatePassRenderNode( pass, pipeline ),
			geometryEntry.modelMatrixUbo,
			geometryEntry.modelUbo,
			geometryEntry.pickingUbo,
			geometryEntry.texturesUbo,
			geometryEntry.modelInstancesUbo,
			buffers,
			*primitive.getParent(),
			submesh,
			primitive,
		};
	}

	BillboardRenderNode RenderPass::createBillboardNode( Pass & pass
		, RenderPipeline & pipeline
		, BillboardBase & billboard )
	{
		auto & buffers = billboard.getGeometryBuffers();
		auto & scene = billboard.getParentScene();
		auto billboardEntry = scene.getBillboardListCache().getUboPools().getUbos( billboard
			, pass
			, getInstanceMult() );
		auto it = m_modelsInstances.emplace( billboardEntry.hash
			, billboardEntry.modelInstancesUbo ).first;
		it->second = billboardEntry.modelInstancesUbo;
		m_isDirty = true;

		return BillboardRenderNode
		{
			pipeline,
			doCreatePassRenderNode( pass, pipeline ),
			billboardEntry.modelMatrixUbo,
			billboardEntry.modelUbo,
			billboardEntry.pickingUbo,
			billboardEntry.billboardUbo,
			billboardEntry.texturesUbo,
			billboardEntry.modelInstancesUbo,
			buffers,
			*billboard.getNode(),
			billboard
		};
	}

	void RenderPass::updatePipeline( RenderPipeline & pipeline )
	{
		doUpdatePipeline( pipeline );
	}

	void RenderPass::updateFlags( PipelineFlags & flags )const
	{
		doUpdateFlags( flags );
	}

	ashes::PipelineColorBlendStateCreateInfo RenderPass::createBlendState( BlendMode colourBlendMode
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

	TextureFlags RenderPass::getTexturesMask()const
	{
		return TextureFlags{ TextureFlag::eAll };
	}

	namespace
	{
		template< typename RenderNodeT >
		void initialiseCommonUboDescriptor( Engine & engine
			, ashes::DescriptorSetLayout const & layout
			, RenderNodeT & node
			, MatrixUbo & matrixUbo
			, SceneUbo const & sceneUbo )
		{
			ashes::DescriptorSet & uboDescriptorSet = *node.uboDescriptorSet;
			engine.getMaterialCache().getPassBuffer().createBinding( uboDescriptorSet
				, layout.getBinding( getPassBufferIndex() ) );

			if ( !node.pipeline.getFlags().textures.empty() )
			{
				engine.getMaterialCache().getTextureBuffer().createBinding( uboDescriptorSet
					, layout.getBinding( getTexturesBufferIndex() ) );
			}

			if ( checkFlag( node.pipeline.getFlags().programFlags, ProgramFlag::eLighting ) )
			{
				uboDescriptorSet.createBinding( layout.getBinding( getLightBufferIndex() )
					, node.sceneNode.getScene()->getLightCache().getBuffer()
					, node.sceneNode.getScene()->getLightCache().getView() );
			}

			matrixUbo.createSizedBinding( uboDescriptorSet
				, layout.getBinding( MatrixUbo::BindingPoint ) );
			sceneUbo.createSizedBinding( uboDescriptorSet
				, layout.getBinding( SceneUbo::BindingPoint ) );

			if ( !checkFlag( node.pipeline.getFlags().programFlags, ProgramFlag::eInstantiation ) )
			{
				node.modelMatrixUbo.createSizedBinding( uboDescriptorSet
					, layout.getBinding( ModelMatrixUbo::BindingPoint ) );
			}

			if ( checkFlag( node.pipeline.getFlags().programFlags, ProgramFlag::eInstanceMult ) )
			{
				CU_Require( node.modelInstancesUbo );
				node.modelInstancesUbo.createSizedBinding( uboDescriptorSet
					, layout.getBinding( ModelInstancesUbo::BindingPoint ) );
			}

			if ( !node.pipeline.getFlags().textures.empty() )
			{
				node.texturesUbo.createSizedBinding( uboDescriptorSet
					, layout.getBinding( TexturesUbo::BindingPoint ) );
			}

			node.modelUbo.createSizedBinding( uboDescriptorSet
				, layout.getBinding( ModelUbo::BindingPoint ) );
		}
	}

	void RenderPass::initialiseUboDescriptor( ashes::DescriptorSetPool const & descriptorPool
		, BillboardRenderNode & node )
	{
		auto & layout = descriptorPool.getLayout();
		node.uboDescriptorSet = descriptorPool.createDescriptorSet( getName() + "_BillboardUbo"
			, 0u );
		initialiseCommonUboDescriptor( *getEngine()
			, layout
			, node
			, m_matrixUbo
			, m_sceneUbo );
		node.billboardUbo.createSizedBinding( *node.uboDescriptorSet
			, layout.getBinding( BillboardUbo::BindingPoint ) );
		doFillUboDescriptor( layout, node );
		node.uboDescriptorSet->update();
	}

	void RenderPass::initialiseUboDescriptor( ashes::DescriptorSetPool const & descriptorPool
		, MorphingRenderNode & node )
	{
		auto & layout = descriptorPool.getLayout();
		node.uboDescriptorSet = descriptorPool.createDescriptorSet( getName() + "_" + node.instance.getName() + "Ubo"
			, 0u );
		initialiseCommonUboDescriptor( *getEngine()
			, layout
			, node
			, m_matrixUbo
			, m_sceneUbo );
		node.morphingUbo.createSizedBinding( *node.uboDescriptorSet
			, layout.getBinding( MorphingUbo::BindingPoint ) );
		doFillUboDescriptor( layout, node );
		node.uboDescriptorSet->update();
	}

	void RenderPass::initialiseUboDescriptor( ashes::DescriptorSetPool const & descriptorPool
		, SkinningRenderNode & node )
	{
		auto & layout = descriptorPool.getLayout();
		node.uboDescriptorSet = descriptorPool.createDescriptorSet( getName() + "_" + node.instance.getName() + "Ubo"
			, 0u );
		initialiseCommonUboDescriptor( *getEngine()
			, layout
			, node
			, m_matrixUbo
			, m_sceneUbo );

		if ( checkFlag( node.pipeline.getFlags().programFlags, ProgramFlag::eInstantiation ) )
		{
			node.data.getInstantiatedBones().getInstancedBonesBuffer().createBinding( *node.uboDescriptorSet
				, layout.getBinding( SkinningUbo::BindingPoint ) );
		}
		else
		{
			node.skinningUbo.createSizedBinding( *node.uboDescriptorSet
				, layout.getBinding( SkinningUbo::BindingPoint ) );
		}

		doFillUboDescriptor( layout, node );
		node.uboDescriptorSet->update();
	}

	void RenderPass::initialiseUboDescriptor( ashes::DescriptorSetPool const & descriptorPool
		, StaticRenderNode & node )
	{
		auto & layout = descriptorPool.getLayout();
		node.uboDescriptorSet = descriptorPool.createDescriptorSet( getName() + "_" + node.instance.getName() + "Ubo"
			, 0u );
		initialiseCommonUboDescriptor( *getEngine()
			, layout
			, node
			, m_matrixUbo
			, m_sceneUbo );
		doFillUboDescriptor( layout, node );
		node.uboDescriptorSet->update();
	}

	void RenderPass::initialiseUboDescriptor( ashes::DescriptorSetPool const & descriptorPool
		, SubmeshSkinninRenderNodesByPassMap & nodes )
	{
		for ( auto & passNodes : nodes )
		{
			for ( auto & submeshNodes : passNodes.second )
			{
				initialiseUboDescriptor( descriptorPool, submeshNodes.second.begin()->second );
			}
		}
	}

	void RenderPass::initialiseUboDescriptor( ashes::DescriptorSetPool const & descriptorPool
		, SubmeshStaticRenderNodesByPassMap & nodes )
	{
		for ( auto & passNodes : nodes )
		{
			for ( auto & submeshNodes : passNodes.second )
			{
				initialiseUboDescriptor( descriptorPool, submeshNodes.second.begin()->second );
			}
		}
	}

	void RenderPass::initialiseTextureDescriptor( ashes::DescriptorSetPool const & descriptorPool
		, BillboardRenderNode & node
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		auto & layout = descriptorPool.getLayout();
		uint32_t index = getMinTextureIndex();
		node.texDescriptorSet = descriptorPool.createDescriptorSet( getName() + "_BillboardTex"
			, 1u );
		doFillTextureDescriptor( layout, index, node, shadowMaps );
		node.texDescriptorSet->update();
	}

	void RenderPass::initialiseTextureDescriptor( ashes::DescriptorSetPool const & descriptorPool
		, MorphingRenderNode & node
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		auto & layout = descriptorPool.getLayout();
		uint32_t index = getMinTextureIndex();
		node.texDescriptorSet = descriptorPool.createDescriptorSet( getName() + "_" + node.instance.getName() + "Tex"
			, 1u );
		doFillTextureDescriptor( layout, index, node, shadowMaps );
		node.texDescriptorSet->update();
	}

	void RenderPass::initialiseTextureDescriptor( ashes::DescriptorSetPool const & descriptorPool
		, SkinningRenderNode & node
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		auto & layout = descriptorPool.getLayout();
		uint32_t index = getMinTextureIndex();
		node.texDescriptorSet = descriptorPool.createDescriptorSet( getName() + "_" + node.instance.getName() + "Tex"
			, 1u );
		doFillTextureDescriptor( layout, index, node, shadowMaps );
		node.texDescriptorSet->update();
	}

	void RenderPass::initialiseTextureDescriptor( ashes::DescriptorSetPool const & descriptorPool
		, StaticRenderNode & node
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		auto & layout = descriptorPool.getLayout();
		uint32_t index = getMinTextureIndex();
		node.texDescriptorSet = descriptorPool.createDescriptorSet( getName() + "_" + node.instance.getName() + "Tex"
			, 1u );
		doFillTextureDescriptor( layout, index, node, shadowMaps );
		node.texDescriptorSet->update();
	}

	namespace
	{
		template< typename MapType >
		void initialiseTextureDescriptors( RenderPass & renderPass
			, ashes::DescriptorSetPool const & descriptorPool
			, MapType & nodes
			, ShadowMapLightTypeArray const & shadowMaps )
		{
			subTraverseNodes( nodes
				, [&renderPass, &descriptorPool, &shadowMaps]( Pass & pass1
					, auto & itPass )
				{
					Pass & pass = itPass.second.begin()->second.passNode.pass;

					if ( itPass.second.begin()->second.pipeline.hasDescriptorPool( 1u ) )
					{
						renderPass.initialiseTextureDescriptor( descriptorPool
							, itPass.second.begin()->second
							, shadowMaps );
					}
				} );
		}
	}

	void RenderPass::initialiseTextureDescriptor( ashes::DescriptorSetPool const & descriptorPool
		, SubmeshSkinninRenderNodesByPassMap & nodes
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		initialiseTextureDescriptors( *this
			, descriptorPool
			, nodes
			, shadowMaps );
	}

	void RenderPass::initialiseTextureDescriptor( ashes::DescriptorSetPool const & descriptorPool
		, SubmeshStaticRenderNodesByPassMap & nodes
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		initialiseTextureDescriptors( *this
			, descriptorPool
			, nodes
			, shadowMaps );
	}

	PassRenderNode RenderPass::doCreatePassRenderNode( Pass & pass
		, RenderPipeline & pipeline )
	{
		return PassRenderNode
		{
			pass,
		};
	}

	SceneRenderNode RenderPass::doCreateSceneRenderNode( Scene & scene
		, RenderPipeline & pipeline )
	{
		return SceneRenderNode{};
	}

	ShaderProgramSPtr RenderPass::doGetProgram( PipelineFlags const & flags )const
	{
		return getEngine()->getShaderProgramCache().getAutomaticProgram( *this
			, flags );
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

	uint32_t RenderPass::doCopyNodesMatrices( StaticRenderNodePtrArray const & renderNodes
		, std::vector< InstantiationData > & matrixBuffer )const
	{
		return copyNodesMatrices( renderNodes, matrixBuffer, m_instanceMult );
	}

	uint32_t RenderPass::doCopyNodesMatrices( StaticRenderNodePtrArray const & renderNodes
		, std::vector< InstantiationData > & matrixBuffer
		, RenderInfo & info )const
	{
		auto count = this->doCopyNodesMatrices( renderNodes, matrixBuffer );
		info.m_visibleObjectsCount += count;
		return count;
	}

	uint32_t RenderPass::doCopyNodesMatrices( SkinningRenderNodePtrArray const & renderNodes
		, std::vector< InstantiationData > & matrixBuffer )const
	{
		return copyNodesMatrices( renderNodes, matrixBuffer, m_instanceMult );
	}

	uint32_t RenderPass::doCopyNodesMatrices( SkinningRenderNodePtrArray const & renderNodes
		, std::vector< InstantiationData > & matrixBuffer
		, RenderInfo & info )const
	{
		auto count = doCopyNodesMatrices( renderNodes, matrixBuffer );
		info.m_visibleObjectsCount += count;
		return count;
	}

	uint32_t RenderPass::doCopyNodesBones( SkinningRenderNodePtrArray const & renderNodes
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

	uint32_t RenderPass::doCopyNodesBones( SkinningRenderNodePtrArray const & renderNodes
		, ShaderBuffer & bonesBuffer
		, RenderInfo & info )const
	{
		auto count = doCopyNodesBones( renderNodes, bonesBuffer );
		info.m_visibleObjectsCount += count;
		return count;
	}

	void RenderPass::doUpdate( SubmeshStaticRenderNodesPtrByPipelineMap & nodes )
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

	void RenderPass::doUpdate( SubmeshStaticRenderNodesPtrByPipelineMap & nodes
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
		inline void renderNonInstanced( RenderPass & pass
			, MapType & nodes )
		{
			for ( auto & itPipelines : nodes )
			{
				pass.updatePipeline( *itPipelines.first );
			}
		}

		template< typename MapType >
		inline void renderNonInstanced( RenderPass & pass
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

	void RenderPass::doUpdate( StaticRenderNodesPtrByPipelineMap & nodes )
	{
		renderNonInstanced( *this
			, nodes );
	}

	void RenderPass::doUpdate( StaticRenderNodesPtrByPipelineMap & nodes
		, RenderInfo & info )
	{
		renderNonInstanced( *this
			, nodes
			, info );
	}

	void RenderPass::doUpdate( SkinningRenderNodesPtrByPipelineMap & nodes )
	{
		renderNonInstanced( *this
			, nodes );
	}

	void RenderPass::doUpdate( SkinningRenderNodesPtrByPipelineMap & nodes
		, RenderInfo & info )
	{
		renderNonInstanced( *this
			, nodes
			, info );
	}

	void RenderPass::doUpdate( SubmeshSkinningRenderNodesPtrByPipelineMap & nodes )
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

	void RenderPass::doUpdate( SubmeshSkinningRenderNodesPtrByPipelineMap & nodes
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

	void RenderPass::doUpdate( MorphingRenderNodesPtrByPipelineMap & nodes )
	{
		renderNonInstanced( *this
			, nodes );
	}

	void RenderPass::doUpdate( MorphingRenderNodesPtrByPipelineMap & nodes
		, RenderInfo & info )
	{
		renderNonInstanced( *this
			, nodes
			, info );
	}

	void RenderPass::doUpdate( BillboardRenderNodesPtrByPipelineMap & nodes )
	{
		renderNonInstanced( *this
			, nodes );
	}

	void RenderPass::doUpdate( BillboardRenderNodesPtrByPipelineMap & nodes
		, RenderInfo & info )
	{
		renderNonInstanced( *this
			, nodes
			, info );
	}

	void RenderPass::doUpdateUbos( CpuUpdater & updater )
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

	std::map< PipelineFlags, RenderPipelineUPtr > & RenderPass::doGetFrontPipelines()
	{
		return m_frontPipelines;
	}

	std::map< PipelineFlags, RenderPipelineUPtr > & RenderPass::doGetBackPipelines()
	{
		return m_backPipelines;
	}

	std::map< PipelineFlags, RenderPipelineUPtr > const & RenderPass::doGetFrontPipelines()const
	{
		return m_frontPipelines;
	}

	std::map< PipelineFlags, RenderPipelineUPtr > const & RenderPass::doGetBackPipelines()const
	{
		return m_backPipelines;
	}

	void RenderPass::doPrepareFrontPipeline( ShaderProgramSPtr program
		, ashes::PipelineVertexInputStateCreateInfoCRefArray const & layouts
		, PipelineFlags const & flags )
	{
		auto & pipelines = doGetFrontPipelines();

		if ( pipelines.find( flags ) == pipelines.end() )
		{
			auto dsState = doCreateDepthStencilState( flags );
			auto bdState = doCreateBlendState( flags );
			auto & pipeline = *pipelines.emplace( flags
				, std::make_unique< RenderPipeline >( *this
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
				, [this, &pipeline, flags]( RenderDevice const & device )
				{
					auto uboBindings = doCreateUboBindings( flags );
					auto texBindings = doCreateTextureBindings( flags );
					auto uboLayout = device->createDescriptorSetLayout( getName()
						, std::move( uboBindings ) );
					auto texLayout = device->createDescriptorSetLayout( getName()
						, std::move( texBindings ) );
					std::vector< ashes::DescriptorSetLayoutPtr > dsLayouts;
					dsLayouts.emplace_back( std::move( uboLayout ) );
					dsLayouts.emplace_back( std::move( texLayout ) );
					pipeline.setDescriptorSetLayouts( std::move( dsLayouts ) );
					pipeline.initialise( device, getRenderPass() );
				} ) );
		}
	}

	void RenderPass::doPrepareBackPipeline( ShaderProgramSPtr program
		, ashes::PipelineVertexInputStateCreateInfoCRefArray const & layouts
		, PipelineFlags const & flags )
	{
		auto & pipelines = doGetBackPipelines();

		if ( pipelines.find( flags ) == pipelines.end() )
		{
			auto dsState = doCreateDepthStencilState( flags );
			auto bdState = doCreateBlendState( flags );
			auto & pipeline = *pipelines.emplace( flags
				, std::make_unique< RenderPipeline >( *this
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
				, [this, &pipeline, flags]( RenderDevice const & device )
				{
					auto uboBindings = doCreateUboBindings( flags );
					auto texBindings = doCreateTextureBindings( flags );
					auto uboLayout = device->createDescriptorSetLayout( std::move( uboBindings ) );
					auto texLayout = device->createDescriptorSetLayout( std::move( texBindings ) );
					std::vector< ashes::DescriptorSetLayoutPtr > dsLayouts;
					dsLayouts.emplace_back( std::move( uboLayout ) );
					dsLayouts.emplace_back( std::move( texLayout ) );
					pipeline.setDescriptorSetLayouts( std::move( dsLayouts ) );
					pipeline.initialise( device, getRenderPass() );
				} ) );
		}
	}

	ashes::VkDescriptorSetLayoutBindingArray RenderPass::doCreateUboBindings( PipelineFlags const & flags )const
	{
		ashes::VkDescriptorSetLayoutBindingArray uboBindings;
		uboBindings.emplace_back( getEngine()->getMaterialCache().getPassBuffer().createLayoutBinding() );

		if ( !flags.textures.empty() )
		{
			uboBindings.emplace_back( getEngine()->getMaterialCache().getTextureBuffer().createLayoutBinding() );
		}

		if ( checkFlag( flags.programFlags, ProgramFlag::eLighting ) )
		{
			uboBindings.emplace_back( makeDescriptorSetLayoutBinding( getLightBufferIndex()
				, VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER
				, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		}

		uboBindings.emplace_back( makeDescriptorSetLayoutBinding( MatrixUbo::BindingPoint//3
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, ( checkFlag( flags.programFlags, ProgramFlag::eHasGeometry )
				? VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_VERTEX_BIT
				: VK_SHADER_STAGE_VERTEX_BIT ) ) );
		uboBindings.emplace_back( makeDescriptorSetLayoutBinding( SceneUbo::BindingPoint//4
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, ( VK_SHADER_STAGE_FRAGMENT_BIT
				| ( checkFlag( flags.programFlags, ProgramFlag::eHasGeometry )
					? VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_VERTEX_BIT
					: VK_SHADER_STAGE_VERTEX_BIT ) ) ) );

		if ( !checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) )
		{
			uboBindings.emplace_back( makeDescriptorSetLayoutBinding( ModelMatrixUbo::BindingPoint//5
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, ( checkFlag( flags.programFlags, ProgramFlag::eHasGeometry )
					? VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_VERTEX_BIT
					: VK_SHADER_STAGE_VERTEX_BIT ) ) );
		}

		uboBindings.emplace_back( makeDescriptorSetLayoutBinding( TexturesUbo::BindingPoint//6
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		uboBindings.emplace_back( makeDescriptorSetLayoutBinding( ModelUbo::BindingPoint//7
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, ( VK_SHADER_STAGE_FRAGMENT_BIT
				| ( checkFlag( flags.programFlags, ProgramFlag::eHasGeometry )
					? VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_VERTEX_BIT
					: VK_SHADER_STAGE_VERTEX_BIT ) ) ) );

		if ( checkFlag( flags.programFlags, ProgramFlag::eSkinning ) )
		{
			uboBindings.push_back( SkinningUbo::createLayoutBinding( SkinningUbo::BindingPoint//8
				, flags.programFlags ) );
		}

		if ( checkFlag( flags.programFlags, ProgramFlag::eMorphing ) )
		{
			uboBindings.emplace_back( makeDescriptorSetLayoutBinding( MorphingUbo::BindingPoint//9
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, ( checkFlag( flags.programFlags, ProgramFlag::eHasGeometry )
					? VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_VERTEX_BIT
					: VK_SHADER_STAGE_VERTEX_BIT ) ) );
		}
		
		if ( checkFlag( flags.programFlags, ProgramFlag::eInstanceMult ) )
		{
			uboBindings.emplace_back( makeDescriptorSetLayoutBinding( ModelInstancesUbo::BindingPoint//10
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, VK_SHADER_STAGE_VERTEX_BIT ) );
		}

		if ( checkFlag( flags.programFlags, ProgramFlag::eBillboards ) )
		{
			uboBindings.emplace_back( makeDescriptorSetLayoutBinding( BillboardUbo::BindingPoint//11
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, ( checkFlag( flags.programFlags, ProgramFlag::eHasGeometry )
					? VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_VERTEX_BIT
					: VK_SHADER_STAGE_VERTEX_BIT ) ) );
		}

		if ( checkFlag( flags.programFlags, ProgramFlag::ePicking ) )
		{
			uboBindings.emplace_back( makeDescriptorSetLayoutBinding( PickingUbo::BindingPoint//12
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		}

		return uboBindings;
	}

	void RenderPass::doUpdate( RenderQueueArray & queues )
	{
		queues.emplace_back( m_renderQueue );
	}

	ShaderPtr RenderPass::doGetVertexShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		VertexWriter writer;
		bool hasTextures = !flags.textures.empty();
		// Vertex inputs
		auto position = writer.declInput< Vec4 >( "position"
			, RenderPass::VertexInputs::PositionLocation );
		auto normal = writer.declInput< Vec3 >( "normal"
			, RenderPass::VertexInputs::NormalLocation );
		auto tangent = writer.declInput< Vec3 >( "tangent"
			, RenderPass::VertexInputs::TangentLocation );
		auto uv = writer.declInput< Vec3 >( "uv"
			, RenderPass::VertexInputs::TextureLocation
			, hasTextures );
		auto inBoneIds0 = writer.declInput< IVec4 >( "inBoneIds0"
			, RenderPass::VertexInputs::BoneIds0Location
			, checkFlag( flags.programFlags, ProgramFlag::eSkinning ) );
		auto inBoneIds1 = writer.declInput< IVec4 >( "inBoneIds1"
			, RenderPass::VertexInputs::BoneIds1Location
			, checkFlag( flags.programFlags, ProgramFlag::eSkinning ) );
		auto inWeights0 = writer.declInput< Vec4 >( "inWeights0"
			, RenderPass::VertexInputs::Weights0Location
			, checkFlag( flags.programFlags, ProgramFlag::eSkinning ) );
		auto inWeights1 = writer.declInput< Vec4 >( "inWeights1"
			, RenderPass::VertexInputs::Weights1Location
			, checkFlag( flags.programFlags, ProgramFlag::eSkinning ) );
		auto transform = writer.declInput< Mat4 >( "transform"
			, RenderPass::VertexInputs::TransformLocation
			, checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) );
		auto material = writer.declInput< Int >( "material"
			, RenderPass::VertexInputs::MaterialLocation
			, checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) );
		auto inPosition2 = writer.declInput< Vec4 >( "inPosition2"
			, RenderPass::VertexInputs::Position2Location
			, checkFlag( flags.programFlags, ProgramFlag::eMorphing ) );
		auto inNormal2 = writer.declInput< Vec3 >( "inNormal2"
			, RenderPass::VertexInputs::Normal2Location
			, checkFlag( flags.programFlags, ProgramFlag::eMorphing ) );
		auto inTangent2 = writer.declInput< Vec3 >( "inTangent2"
			, RenderPass::VertexInputs::Tangent2Location
			, checkFlag( flags.programFlags, ProgramFlag::eMorphing ) );
		auto inTexture2 = writer.declInput< Vec3 >( "inTexture2"
			, RenderPass::VertexInputs::Texture2Location
			, checkFlag( flags.programFlags, ProgramFlag::eMorphing ) && hasTextures );
		auto in = writer.getIn();

		UBO_MATRIX( writer, MatrixUbo::BindingPoint, 0 );
		UBO_SCENE( writer, SceneUbo::BindingPoint, 0 );
		UBO_MODEL_MATRIX( writer, ModelMatrixUbo::BindingPoint, 0 );
		UBO_MODEL( writer, ModelUbo::BindingPoint, 0 );
		auto skinningData = SkinningUbo::declare( writer, SkinningUbo::BindingPoint, 0, flags.programFlags );
		UBO_MORPHING( writer, MorphingUbo::BindingPoint, 0, flags.programFlags );

		// Outputs
		auto vtx_worldPosition = writer.declOutput< Vec3 >( "vtx_worldPosition"
			, RenderPass::VertexOutputs::WorldPositionLocation );
		auto vtx_tangentSpaceFragPosition = writer.declOutput< Vec3 >( "vtx_tangentSpaceFragPosition"
			, RenderPass::VertexOutputs::TangentSpaceFragPositionLocation );
		auto vtx_tangentSpaceViewPosition = writer.declOutput< Vec3 >( "vtx_tangentSpaceViewPosition"
			, RenderPass::VertexOutputs::TangentSpaceViewPositionLocation );
		auto vtx_normal = writer.declOutput< Vec3 >( "vtx_normal"
			, RenderPass::VertexOutputs::NormalLocation );
		auto vtx_tangent = writer.declOutput< Vec3 >( "vtx_tangent"
			, RenderPass::VertexOutputs::TangentLocation );
		auto vtx_bitangent = writer.declOutput< Vec3 >( "vtx_bitangent"
			, RenderPass::VertexOutputs::BitangentLocation );
		auto vtx_texture = writer.declOutput< Vec3 >( "vtx_texture"
			, RenderPass::VertexOutputs::TextureLocation
			, hasTextures );
		auto vtx_instance = writer.declOutput< UInt >( "vtx_instance"
			, RenderPass::VertexOutputs::InstanceLocation );
		auto vtx_material = writer.declOutput< UInt >( "vtx_material"
			, RenderPass::VertexOutputs::MaterialLocation );
		auto out = writer.getOut();

		writer.implementFunction< sdw::Void >( "main"
			, [&]()
			{
				auto curPosition = writer.declLocale( "curPosition"
					, vec4( position.xyz(), 1.0_f ) );
				auto v4Normal = writer.declLocale( "v4Normal"
					, vec4( normal, 0.0_f ) );
				auto v4Tangent = writer.declLocale( "v4Tangent"
					, vec4( tangent, 0.0_f ) );
				vtx_texture = uv;

				if ( checkFlag( flags.programFlags, ProgramFlag::eSkinning ) )
				{
					auto curMtxModel = writer.declLocale< Mat4 >( "curMtxModel"
						, SkinningUbo::computeTransform( skinningData, writer, flags.programFlags ) );
					writer.declLocale< Mat4 >( "prvMtxModel"
						, curMtxModel );
				}
				else if ( checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) )
				{
					auto curMtxModel = writer.declLocale< Mat4 >( "curMtxModel"
						, transform );
					writer.declLocale< Mat4 >( "prvMtxModel"
						, curMtxModel );
				}
				else
				{
					auto curMtxModel = writer.declLocale< Mat4 >( "curMtxModel"
						, c3d_curMtxModel );
					writer.declLocale< Mat4 >( "prvMtxModel"
						, c3d_prvMtxModel );
				}

				auto curMtxModel = writer.getVariable< Mat4 >( "curMtxModel" );
				auto prvMtxModel = writer.getVariable< Mat4 >( "prvMtxModel" );
				auto mtxNormal = writer.declLocale( "mtxNormal"
					, transpose( inverse( mat3( curMtxModel ) ) ) );

				if ( checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) )
				{
					vtx_material = writer.cast< UInt >( material );
				}
				else
				{
					vtx_material = writer.cast< UInt >( c3d_materialIndex );
				}

				if ( checkFlag( flags.programFlags, ProgramFlag::eMorphing ) )
				{
					curPosition = vec4( sdw::mix( curPosition.xyz(), inPosition2.xyz(), vec3( c3d_time ) ), 1.0_f );
					v4Normal = vec4( sdw::mix( v4Normal.xyz(), inNormal2.xyz(), vec3( c3d_time ) ), 1.0_f );
					v4Tangent = vec4( sdw::mix( v4Tangent.xyz(), inTangent2.xyz(), vec3( c3d_time ) ), 1.0_f );
					vtx_texture = sdw::mix( vtx_texture, inTexture2, vec3( c3d_time ) );
				}

				auto prvPosition = writer.declLocale( "prvPosition"
					, prvMtxModel * curPosition );
				curPosition = curMtxModel * curPosition;
				vtx_worldPosition = curPosition.xyz();
				prvPosition = c3d_prvViewProj * prvPosition;
				curPosition = c3d_curViewProj * curPosition;

				vtx_normal = normalize( mtxNormal * v4Normal.xyz() );
				vtx_tangent = normalize( mtxNormal * v4Tangent.xyz() );
				vtx_tangent = normalize( sdw::fma( -vtx_normal, vec3( dot( vtx_tangent, vtx_normal ) ), vtx_tangent ) );
				vtx_bitangent = cross( vtx_normal, vtx_tangent );

				if ( checkFlag( flags.programFlags, ProgramFlag::eInvertNormals ) )
				{
					vtx_normal = -vtx_normal;
					vtx_tangent = -vtx_tangent;
					vtx_bitangent = -vtx_bitangent;
				}

				vtx_instance = writer.cast< UInt >( in.instanceIndex );

				auto tbn = writer.declLocale( "tbn"
					, transpose( mat3( vtx_tangent, vtx_bitangent, vtx_normal ) ) );
				vtx_tangentSpaceFragPosition = tbn * vtx_worldPosition;
				vtx_tangentSpaceViewPosition = tbn * c3d_cameraPosition.xyz();

				// Convert the jitter from non-homogeneous coordinates to homogeneous
				// coordinates and add it:
				// (note that for providing the jitter in non-homogeneous projection space,
				//  pixel coordinates (screen space) need to multiplied by two in the C++
				//  code)
				curPosition.xy() -= c3d_jitter * curPosition.w();
				out.vtx.position = curPosition;

			} );
		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr RenderPass::doGetBillboardShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		VertexWriter writer;
		bool hasTextures = !flags.textures.empty();

		// Shader inputs
		auto position = writer.declInput< Vec4 >( "position", 0u );
		auto uv = writer.declInput< Vec2 >( "uv", 1u, hasTextures );
		auto center = writer.declInput< Vec3 >( "center", 2u );
		auto in = writer.getIn();
		UBO_MATRIX( writer, MatrixUbo::BindingPoint, 0 );
		UBO_SCENE( writer, SceneUbo::BindingPoint, 0 );
		UBO_MODEL_MATRIX( writer, ModelMatrixUbo::BindingPoint, 0 );
		UBO_MODEL( writer, ModelUbo::BindingPoint, 0 );
		UBO_BILLBOARD( writer, BillboardUbo::BindingPoint, 0 );

		// Shader outputs
		auto vtx_worldPosition = writer.declOutput< Vec3 >( "vtx_worldPosition"
			, RenderPass::VertexOutputs::WorldPositionLocation );
		auto vtx_viewPosition = writer.declOutput< Vec3 >( "vtx_viewPosition"
			, RenderPass::VertexOutputs::ViewPositionLocation );
		auto vtx_curPosition = writer.declOutput< Vec3 >( "vtx_curPosition"
			, RenderPass::VertexOutputs::CurPositionLocation );
		auto vtx_prvPosition = writer.declOutput< Vec3 >( "vtx_prvPosition"
			, RenderPass::VertexOutputs::PrvPositionLocation );
		auto vtx_tangentSpaceFragPosition = writer.declOutput< Vec3 >( "vtx_tangentSpaceFragPosition"
			, RenderPass::VertexOutputs::TangentSpaceFragPositionLocation );
		auto vtx_tangentSpaceViewPosition = writer.declOutput< Vec3 >( "vtx_tangentSpaceViewPosition"
			, RenderPass::VertexOutputs::TangentSpaceViewPositionLocation );
		auto vtx_normal = writer.declOutput< Vec3 >( "vtx_normal"
			, RenderPass::VertexOutputs::NormalLocation );
		auto vtx_tangent = writer.declOutput< Vec3 >( "vtx_tangent"
			, RenderPass::VertexOutputs::TangentLocation );
		auto vtx_bitangent = writer.declOutput< Vec3 >( "vtx_bitangent"
			, RenderPass::VertexOutputs::BitangentLocation );
		auto vtx_texture = writer.declOutput< Vec3 >( "vtx_texture"
			, RenderPass::VertexOutputs::TextureLocation
			, hasTextures );
		auto vtx_instance = writer.declOutput< UInt >( "vtx_instance"
			, RenderPass::VertexOutputs::InstanceLocation );
		auto vtx_material = writer.declOutput< UInt >( "vtx_material"
			, RenderPass::VertexOutputs::MaterialLocation );
		auto out = writer.getOut();

		writer.implementFunction< Void >( "main"
			, [&]()
			{
				auto curBbcenter = writer.declLocale( "curBbcenter"
					, ( c3d_curMtxModel * vec4( center, 1.0_f ) ).xyz() );
				auto prvBbcenter = writer.declLocale( "prvBbcenter" 
					, ( c3d_prvMtxModel * vec4( center, 1.0_f ) ).xyz() );
				auto curToCamera = writer.declLocale( "curToCamera"
					, c3d_cameraPosition.xyz() - curBbcenter );
				curToCamera.y() = 0.0_f;
				curToCamera = normalize( curToCamera );
				auto right = writer.declLocale( "right"
					, vec3( c3d_curView[0][0], c3d_curView[1][0], c3d_curView[2][0] ) );

				if ( checkFlag( flags.programFlags, ProgramFlag::eSpherical ) )
				{
					writer.declLocale( "up"
						, vec3( c3d_curView[0][1], c3d_curView[1][1], c3d_curView[2][1] ) );
				}
				else
				{
					right = normalize( vec3( right.x(), 0.0_f, right.z() ) );
					writer.declLocale( "up"
						, vec3( 0.0_f, 1.0f, 0.0f ) );
				}

				auto up = writer.getVariable< Vec3 >( "up" );
				vtx_material = writer.cast< UInt >( c3d_materialIndex );
				vtx_normal = curToCamera;
				vtx_tangent = up;
				vtx_bitangent = right;

				auto width = writer.declLocale( "width"
					, c3d_dimensions.x() );
				auto height = writer.declLocale( "height"
					, c3d_dimensions.y() );

				if ( checkFlag( flags.programFlags, ProgramFlag::eFixedSize ) )
				{
					width = c3d_dimensions.x() / c3d_clipInfo.x();
					height = c3d_dimensions.y() / c3d_clipInfo.y();
				}

				vtx_worldPosition = curBbcenter
					+ right * position.x() * width
					+ up * position.y() * height;
				auto prvPosition = writer.declLocale( "prvPosition"
					, vec4( prvBbcenter + right * position.x() * width + up * position.y() * height, 1.0_f ) );

				if ( hasTextures )
				{
					vtx_texture = vec3( uv, 0.0_f );
				}

				vtx_instance = writer.cast< UInt >( in.instanceIndex );
				auto curPosition = writer.declLocale( "curPosition"
					, c3d_curView * vec4( vtx_worldPosition, 1.0_f ) );
				prvPosition = c3d_prvView * vec4( prvPosition );
				curPosition = c3d_projection * curPosition;
				prvPosition = c3d_projection * prvPosition;
				vtx_viewPosition = curPosition.xyz();

				auto tbn = writer.declLocale( "tbn"
					, transpose( mat3( vtx_tangent, vtx_bitangent, vtx_normal ) ) );
				vtx_tangentSpaceFragPosition = tbn * vtx_worldPosition;
				vtx_tangentSpaceViewPosition = tbn * c3d_cameraPosition.xyz();
				// Convert the jitter from non-homogeneous coordiantes to homogeneous
				// coordinates and add it:
				// (note that for providing the jitter in non-homogeneous projection space,
				//  pixel coordinates (screen space) need to multiplied by two in the C++
				//  code)
				curPosition.xy() -= c3d_jitter * curPosition.w();
				prvPosition.xy() -= c3d_jitter * prvPosition.w();
				out.vtx.position = curPosition;

				vtx_curPosition = curPosition.xyw();
				vtx_prvPosition = prvPosition.xyw();
				// Positions in projection space are in [-1, 1] range, while texture
				// coordinates are in [0, 1] range. So, we divide by 2 to get velocities in
				// the scale (and flip the y axis):
				vtx_curPosition.xy() *= vec2( 0.5_f, -0.5_f );
				vtx_prvPosition.xy() *= vec2( 0.5_f, -0.5_f );
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}
}
