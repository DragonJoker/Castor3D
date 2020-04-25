#include "Castor3D/Render/RenderPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/AnimatedObjectGroupCache.hpp"
#include "Castor3D/Cache/BillboardUboPools.hpp"
#include "Castor3D/Cache/GeometryCache.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Cache/ShaderCache.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/Node/RenderNode_Render.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Animation/AnimatedSkeleton.hpp"
#include "Castor3D/Shader/PassBuffer/PassBuffer.hpp"
#include "Castor3D/Shader/TextureConfigurationBuffer/TextureConfigurationBuffer.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"

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
		inline void doSubTraverseNodes( MapType & nodes
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
		inline void doTraverseNodes( MapType & nodes
			, FuncType function )
		{
			doSubTraverseNodes( nodes
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

	}

	//*********************************************************************************************

	RenderPass::RenderPass( String const & category
		, String const & name
		, Engine & engine
		, MatrixUbo & matrixUbo
		, SceneCuller & culler )
		: OwnedBy< Engine >{ engine }
		, Named{ name }
		, m_renderSystem{ *engine.getRenderSystem() }
		, m_matrixUbo{ matrixUbo }
		, m_culler{ culler }
		, m_category{ category }
		, m_oit{ true }
		, m_renderQueue{ *this, true, nullptr }
		, m_opaque{ true }
		, m_sceneUbo{ engine }
	{
	}

	RenderPass::RenderPass( String const & category
		, String const & name
		, Engine & engine
		, MatrixUbo & matrixUbo
		, SceneCuller & culler
		, bool oit )
		: OwnedBy< Engine >{ engine }
		, Named{ name }
		, m_renderSystem{ *engine.getRenderSystem() }
		, m_matrixUbo{ matrixUbo }
		, m_culler{ culler }
		, m_category{ category }
		, m_oit{ oit }
		, m_renderQueue{ *this, false, nullptr }
		, m_opaque{ false }
		, m_sceneUbo{ engine }
	{
	}

	RenderPass::RenderPass( String const & category
		, String const & name
		, Engine & engine
		, MatrixUbo & matrixUbo
		, SceneCuller & culler
		, SceneNode const * ignored )
		: OwnedBy< Engine >{ engine }
		, Named{ name }
		, m_renderSystem{ *engine.getRenderSystem() }
		, m_matrixUbo{ matrixUbo }
		, m_culler{ culler }
		, m_category{ category }
		, m_oit{ true }
		, m_renderQueue{ *this, true, ignored }
		, m_opaque{ true }
		, m_sceneUbo{ engine }
	{
	}

	RenderPass::RenderPass( String const & category
		, String const & name
		, Engine & engine
		, MatrixUbo & matrixUbo
		, SceneCuller & culler
		, bool oit
		, SceneNode const * ignored )
		: OwnedBy< Engine >{ engine }
		, Named{ name }
		, m_renderSystem{ *engine.getRenderSystem() }
		, m_matrixUbo{ matrixUbo }
		, m_culler{ culler }
		, m_category{ category }
		, m_oit{ oit }
		, m_renderQueue{ *this, false, ignored }
		, m_opaque{ false }
		, m_sceneUbo{ engine }
	{
	}

	RenderPass::~RenderPass()
	{
	}

	bool RenderPass::initialise( Size const & size )
	{
		m_timer = std::make_shared< RenderPassTimer >( *getEngine(), m_category, getName() );
		m_sceneUbo.initialise();
		m_sceneUbo.setWindowSize( size );
		m_size = size;
		return doInitialise( size );
	}

	void RenderPass::cleanup()
	{
		m_sceneUbo.cleanup();
		m_renderPass.reset();
		doCleanup();
		m_timer.reset();
		m_backPipelines.clear();
		m_frontPipelines.clear();
	}

	void RenderPass::update( RenderQueueArray & queues )
	{
		doUpdate( queues );
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

	PipelineFlags RenderPass::prepareBackPipeline( BlendMode colourBlendMode
		, BlendMode alphaBlendMode
		, VkCompareOp alphaFunc
		, PassFlags const & passFlags
		, TextureFlags const & textures
		, uint32_t texturesCount
		, uint32_t heightMapIndex
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, VkPrimitiveTopology topology
		, ashes::PipelineVertexInputStateCreateInfoCRefArray const & layouts )
	{
		auto flags = PipelineFlags{ colourBlendMode
			, alphaBlendMode
			, passFlags
			, textures
			, texturesCount
			, heightMapIndex
			, programFlags
			, sceneFlags
			, topology
			, alphaFunc };
		doUpdateFlags( flags );
		remFlag( flags.programFlags, ProgramFlag::eInvertNormals );

		if ( checkFlag( passFlags, PassFlag::eAlphaBlending ) != m_opaque
			&& ( !checkFlag( programFlags, ProgramFlag::eBillboards )
				|| !isShadowMapProgram( programFlags ) ) )
		{
			if ( m_opaque )
			{
				flags.alphaBlendMode = BlendMode::eNoBlend;
			}

			auto program = doGetProgram( flags );
			doPrepareBackPipeline( program, layouts, flags );
		}

		return flags;
	}

	PipelineFlags RenderPass::prepareFrontPipeline( BlendMode colourBlendMode
		, BlendMode alphaBlendMode
		, VkCompareOp alphaFunc
		, PassFlags const & passFlags
		, TextureFlags const & textures
		, uint32_t texturesCount
		, uint32_t heightMapIndex
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, VkPrimitiveTopology topology
		, ashes::PipelineVertexInputStateCreateInfoCRefArray const & layouts )
	{
		auto flags = PipelineFlags{ colourBlendMode
			, alphaBlendMode
			, passFlags
			, textures
			, texturesCount
			, heightMapIndex
			, programFlags
			, sceneFlags
			, topology
			, alphaFunc };
		doUpdateFlags( flags );
		addFlag( flags.programFlags, ProgramFlag::eInvertNormals );

		if ( checkFlag( passFlags, PassFlag::eAlphaBlending ) != m_opaque
			&& ( !checkFlag( programFlags, ProgramFlag::eBillboards )
				|| !isShadowMapProgram( programFlags ) ) )
		{
			if ( m_opaque )
			{
				flags.alphaBlendMode = BlendMode::eNoBlend;
			}

			auto program = doGetProgram( flags );
			doPrepareFrontPipeline( program, layouts, flags );
		}

		return flags;
	}

	RenderPipeline * RenderPass::getPipelineFront( PipelineFlags flags )const
	{
		if ( m_opaque )
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
		if ( m_opaque )
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
		auto & buffers = submesh.getGeometryBuffers( pass.getOwner()->shared_from_this() );
		auto & scene = *primitive.getScene();
		auto geometryEntry = scene.getGeometryCache().getUbos( primitive, submesh, pass );
		auto animationEntry = scene.getAnimatedObjectGroupCache().getUbos( skeleton );

		return SkinningRenderNode
		{
			pipeline,
			doCreatePassRenderNode( pass, pipeline ),
			geometryEntry.modelMatrixUbo,
			geometryEntry.modelUbo,
			geometryEntry.pickingUbo,
			geometryEntry.texturesUbo,
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
		auto & buffers = submesh.getGeometryBuffers( pass.getOwner()->shared_from_this() );
		auto & scene = *primitive.getScene();
		auto geometryEntry = scene.getGeometryCache().getUbos( primitive, submesh, pass );
		auto animationEntry = scene.getAnimatedObjectGroupCache().getUbos( mesh );

		return MorphingRenderNode
		{
			pipeline,
			doCreatePassRenderNode( pass, pipeline ),
			geometryEntry.modelMatrixUbo,
			geometryEntry.modelUbo,
			geometryEntry.pickingUbo,
			geometryEntry.texturesUbo,
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
		auto & buffers = submesh.getGeometryBuffers( pass.getOwner()->shared_from_this() );
		auto & scene = *primitive.getScene();
		auto entry = scene.getGeometryCache().getUbos( primitive, submesh, pass );

		return StaticRenderNode
		{
			pipeline,
			doCreatePassRenderNode( pass, pipeline ),
			entry.modelMatrixUbo,
			entry.modelUbo,
			entry.pickingUbo,
			entry.texturesUbo,
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
		auto entry = scene.getBillboardPools().getUbos( billboard, pass );

		return BillboardRenderNode
		{
			pipeline,
			doCreatePassRenderNode( pass, pipeline ),
			entry.modelMatrixUbo,
			entry.modelUbo,
			entry.pickingUbo,
			entry.billboardUbo,
			entry.texturesUbo,
			buffers,
			*billboard.getNode(),
			billboard
		};
	}

	void RenderPass::updatePipeline( RenderPipeline & pipeline )const
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
			attach.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
			attach.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
			break;

		case BlendMode::eAdditive:
			attach.blendEnable = true;
			attach.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
			attach.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
			break;

		case BlendMode::eMultiplicative:
			attach.blendEnable = true;
			attach.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
			attach.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
			break;

		case BlendMode::eInterpolative:
			attach.blendEnable = true;
			attach.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_COLOR;
			attach.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
			break;

		default:
			attach.blendEnable = true;
			attach.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_COLOR;
			attach.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
			break;
		}

		switch ( alphaBlendMode )
		{
		case BlendMode::eNoBlend:
			attach.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
			attach.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
			break;

		case BlendMode::eAdditive:
			attach.blendEnable = true;
			attach.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
			attach.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
			break;

		case BlendMode::eMultiplicative:
			attach.blendEnable = true;
			attach.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
			attach.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			attach.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
			attach.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			break;

		case BlendMode::eInterpolative:
			attach.blendEnable = true;
			attach.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
			attach.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			attach.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
			attach.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			break;

		default:
			attach.blendEnable = true;
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

			if ( node.pipeline.getFlags().texturesCount )
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

			uboDescriptorSet.createSizedBinding( layout.getBinding( MatrixUbo::BindingPoint )
				, *matrixUbo.getUbo().buffer
				, matrixUbo.getUbo().offset );
			uboDescriptorSet.createSizedBinding( layout.getBinding( SceneUbo::BindingPoint )
				, sceneUbo.getUbo() );

			if ( !checkFlag( node.pipeline.getFlags().programFlags, ProgramFlag::eInstantiation ) )
			{
				uboDescriptorSet.createSizedBinding( layout.getBinding( ModelMatrixUbo::BindingPoint )
					, *node.modelMatrixUbo.buffer
					, node.modelMatrixUbo.offset
					, 1u );
			}

			uboDescriptorSet.createSizedBinding( layout.getBinding( ModelUbo::BindingPoint )
				, *node.modelUbo.buffer
				, node.modelUbo.offset
				, 1u );
			uboDescriptorSet.createSizedBinding( layout.getBinding( TexturesUbo::BindingPoint )
				, *node.texturesUbo.buffer
				, node.texturesUbo.offset
				, 1u );
		}
	}

	void RenderPass::initialiseUboDescriptor( ashes::DescriptorSetPool const & descriptorPool
		, BillboardRenderNode & node )
	{
		auto & layout = descriptorPool.getLayout();
		node.uboDescriptorSet = descriptorPool.createDescriptorSet( 0u );
		initialiseCommonUboDescriptor( *getEngine()
			, layout
			, node
			, m_matrixUbo
			, m_sceneUbo );
		node.uboDescriptorSet->createSizedBinding( layout.getBinding( BillboardUbo::BindingPoint )
			, *node.billboardUbo.buffer
			, node.billboardUbo.offset
			, 1u );
		doFillUboDescriptor( layout, node );
		node.uboDescriptorSet->update();
	}

	void RenderPass::initialiseUboDescriptor( ashes::DescriptorSetPool const & descriptorPool
		, MorphingRenderNode & node )
	{
		auto & layout = descriptorPool.getLayout();
		node.uboDescriptorSet = descriptorPool.createDescriptorSet( 0u );
		initialiseCommonUboDescriptor( *getEngine()
			, layout
			, node
			, m_matrixUbo
			, m_sceneUbo );
		node.uboDescriptorSet->createSizedBinding( layout.getBinding( MorphingUbo::BindingPoint )
			, *node.morphingUbo.buffer
			, node.morphingUbo.offset
			, 1u );
		doFillUboDescriptor( layout, node );
		node.uboDescriptorSet->update();
	}

	void RenderPass::initialiseUboDescriptor( ashes::DescriptorSetPool const & descriptorPool
		, SkinningRenderNode & node )
	{
		auto & layout = descriptorPool.getLayout();
		node.uboDescriptorSet = descriptorPool.createDescriptorSet( 0u );
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
			node.uboDescriptorSet->createSizedBinding( layout.getBinding( SkinningUbo::BindingPoint )
				, *node.skinningUbo.buffer
				, node.skinningUbo.offset
				, 1u );
		}

		doFillUboDescriptor( layout, node );
		node.uboDescriptorSet->update();
	}

	void RenderPass::initialiseUboDescriptor( ashes::DescriptorSetPool const & descriptorPool
		, StaticRenderNode & node )
	{
		auto & layout = descriptorPool.getLayout();
		node.uboDescriptorSet = descriptorPool.createDescriptorSet( 0u );
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
		node.texDescriptorSet = descriptorPool.createDescriptorSet( 1u );
		doFillTextureDescriptor( layout, index, node, shadowMaps );
		node.texDescriptorSet->update();
	}

	void RenderPass::initialiseTextureDescriptor( ashes::DescriptorSetPool const & descriptorPool
		, MorphingRenderNode & node
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		auto & layout = descriptorPool.getLayout();
		uint32_t index = getMinTextureIndex();
		node.texDescriptorSet = descriptorPool.createDescriptorSet( 1u );
		doFillTextureDescriptor( layout, index, node, shadowMaps );
		node.texDescriptorSet->update();
	}

	void RenderPass::initialiseTextureDescriptor( ashes::DescriptorSetPool const & descriptorPool
		, SkinningRenderNode & node
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		auto & layout = descriptorPool.getLayout();
		uint32_t index = getMinTextureIndex();
		node.texDescriptorSet = descriptorPool.createDescriptorSet( 1u );
		doFillTextureDescriptor( layout, index, node, shadowMaps );
		node.texDescriptorSet->update();
	}

	void RenderPass::initialiseTextureDescriptor( ashes::DescriptorSetPool const & descriptorPool
		, StaticRenderNode & node
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		auto & layout = descriptorPool.getLayout();
		uint32_t index = getMinTextureIndex();
		node.texDescriptorSet = descriptorPool.createDescriptorSet( 1u );
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
			doSubTraverseNodes( nodes
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
		uint32_t copyNodesMatrices( std::vector< NodeT > const & renderNodes
			, std::vector< InstantiationData > & matrixBuffer )
		{
			auto const count = std::min( uint32_t( matrixBuffer.size() )
				, uint32_t( renderNodes.size() ) );
			auto buffer = matrixBuffer.data();
			auto it = renderNodes.begin();
			auto i = 0u;

			while ( i < count )
			{
				auto & node = *it;
				buffer->m_matrix = node.sceneNode.getDerivedTransformationMatrix();
				buffer->m_material = node.passNode.pass.getId();
				++buffer;
				++i;
				++it;
			}

			return count;
		}

		template< typename NodeT >
		uint32_t copyNodesMatrices( std::vector< NodeT * > const & renderNodes
			, std::vector< InstantiationData > & matrixBuffer )
		{
			auto const count = std::min( uint32_t( matrixBuffer.size() )
				, uint32_t( renderNodes.size() ) );
			auto buffer = matrixBuffer.data();
			auto it = renderNodes.begin();
			auto i = 0u;

			while ( i < count )
			{
				auto & node = *it;
				buffer->m_matrix = node->sceneNode.getDerivedTransformationMatrix();
				buffer->m_material = node->passNode.pass.getId();
				++buffer;
				++i;
				++it;
			}

			return count;
		}
	}

	uint32_t RenderPass::doCopyNodesMatrices( StaticRenderNodePtrArray const & renderNodes
		, std::vector< InstantiationData > & matrixBuffer )const
	{
		return copyNodesMatrices( renderNodes, matrixBuffer );
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
		return copyNodesMatrices( renderNodes, matrixBuffer );
	}

	uint32_t RenderPass::doCopyNodesMatrices( SkinningRenderNodePtrArray const & renderNodes
		, std::vector< InstantiationData > & matrixBuffer
		, RenderInfo & info )const
	{
		auto count = this->doCopyNodesMatrices( renderNodes, matrixBuffer );
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
			node->skeleton.fillBuffer( buffer );
			buffer += stride;
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

	void RenderPass::doUpdate( SubmeshStaticRenderNodesPtrByPipelineMap & nodes )const
	{
		doTraverseNodes( nodes
			, [this]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, InstantiationComponent & instantiation
				, StaticRenderNodePtrArray & renderNodes )
			{
				auto it = instantiation.find( pass.getOwner()->shared_from_this() );

				if ( !renderNodes.empty()
					&& it != instantiation.end()
					&& it->second.buffer )
				{
					doCopyNodesMatrices( renderNodes
						, it->second.data );
				}
			} );
	}

	void RenderPass::doUpdate( SubmeshStaticRenderNodesPtrByPipelineMap & nodes
		, RenderInfo & info )const
	{
		doTraverseNodes( nodes
			, [this, &info]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, InstantiationComponent & instantiation
				, StaticRenderNodePtrArray & renderNodes )
			{
				auto it = instantiation.find( pass.getOwner()->shared_from_this() );

				if ( !renderNodes.empty()
					&& it != instantiation.end()
					&& it->second.buffer )
				{
					uint32_t count = doCopyNodesMatrices( renderNodes
						, it->second.data
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
		inline void doRenderNonInstanced( RenderPass const & pass
			, MapType & nodes )
		{
			for ( auto & itPipelines : nodes )
			{
				pass.updatePipeline( *itPipelines.first );
			}
		}

		template< typename MapType >
		inline void doRenderNonInstanced( RenderPass const & pass
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

	void RenderPass::doUpdate( StaticRenderNodesPtrByPipelineMap & nodes )const
	{
		doRenderNonInstanced( *this
			, nodes );
	}

	void RenderPass::doUpdate( StaticRenderNodesPtrByPipelineMap & nodes
		, RenderInfo & info )const
	{
		doRenderNonInstanced( *this
			, nodes
			, info );
	}

	void RenderPass::doUpdate( SkinningRenderNodesPtrByPipelineMap & nodes )const
	{
		doRenderNonInstanced( *this
			, nodes );
	}

	void RenderPass::doUpdate( SkinningRenderNodesPtrByPipelineMap & nodes
		, RenderInfo & info )const
	{
		doRenderNonInstanced( *this
			, nodes
			, info );
	}

	void RenderPass::doUpdate( SubmeshSkinningRenderNodesPtrByPipelineMap & nodes )const
	{
		doTraverseNodes( nodes
			, [this]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, InstantiationComponent & instantiation
				, SkinningRenderNodePtrArray & renderNodes )
			{
				auto & instantiatedBones = submesh.getInstantiatedBones();
				auto it = instantiation.find( pass.getOwner()->shared_from_this() );

				if ( !renderNodes.empty()
					&& it != instantiation.end()
					&& it->second.buffer
					&& instantiatedBones.hasInstancedBonesBuffer() )
				{
					uint32_t count1 = doCopyNodesMatrices( renderNodes, it->second.data );
					uint32_t count2 = doCopyNodesBones( renderNodes, instantiatedBones.getInstancedBonesBuffer() );
					CU_Require( count1 == count2 );
				}
			} );
	}

	void RenderPass::doUpdate( SubmeshSkinningRenderNodesPtrByPipelineMap & nodes
		, RenderInfo & info )const
	{
		doTraverseNodes( nodes
			, [this, &info]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, InstantiationComponent & instantiation
				, SkinningRenderNodePtrArray & renderNodes )
			{
				auto & instantiatedBones = submesh.getInstantiatedBones();
				auto it = instantiation.find( pass.getOwner()->shared_from_this() );

				if ( !renderNodes.empty()
					&& it != instantiation.end()
					&& it->second.buffer
					&& instantiatedBones.hasInstancedBonesBuffer() )
				{
					uint32_t count1 = doCopyNodesMatrices( renderNodes, it->second.data, info );
					uint32_t count2 = doCopyNodesBones( renderNodes, instantiatedBones.getInstancedBonesBuffer(), info );
					CU_Require( count1 == count2 );
					info.m_visibleFaceCount += submesh.getFaceCount() * count1;
					info.m_visibleVertexCount += submesh.getPointsCount() * count1;
					++info.m_drawCalls;
				}
			} );
	}

	void RenderPass::doUpdate( MorphingRenderNodesPtrByPipelineMap & nodes )const
	{
		doRenderNonInstanced( *this
			, nodes );
	}

	void RenderPass::doUpdate( MorphingRenderNodesPtrByPipelineMap & nodes
		, RenderInfo & info )const
	{
		doRenderNonInstanced( *this
			, nodes
			, info );
	}

	void RenderPass::doUpdate( BillboardRenderNodesPtrByPipelineMap & nodes )const
	{
		doRenderNonInstanced( *this
			, nodes );
	}

	void RenderPass::doUpdate( BillboardRenderNodesPtrByPipelineMap & nodes
		, RenderInfo & info )const
	{
		doRenderNonInstanced( *this
			, nodes
			, info );
	}

	void RenderPass::doUpdateUbos( Camera const & camera
		, castor::Point2f const & jitter )
	{
		auto jitterProjSpace = jitter * 2.0f;
		jitterProjSpace[0] /= camera.getWidth();
		jitterProjSpace[1] /= camera.getHeight();
		m_matrixUbo.update( camera.getView()
			, camera.getProjection()
			, jitterProjSpace );
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
				, std::make_unique< RenderPipeline >( *getEngine()->getRenderSystem()
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

			getEngine()->sendEvent( makeFunctorEvent( EventType::ePreRender
				, [this, &pipeline, flags]()
				{
					auto & device = getCurrentRenderDevice( *this );
					auto uboBindings = doCreateUboBindings( flags );
					auto texBindings = doCreateTextureBindings( flags );
					auto uboLayout = device->createDescriptorSetLayout( std::move( uboBindings ) );
					auto texLayout = device->createDescriptorSetLayout( std::move( texBindings ) );
					std::vector< ashes::DescriptorSetLayoutPtr > layouts;
					layouts.emplace_back( std::move( uboLayout ) );
					layouts.emplace_back( std::move( texLayout ) );
					pipeline.setDescriptorSetLayouts( std::move( layouts ) );
					pipeline.initialise( getRenderPass() );
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
				, std::make_unique< RenderPipeline >( *getEngine()->getRenderSystem()
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

			getEngine()->sendEvent( makeFunctorEvent( EventType::ePreRender
				, [this, &pipeline, flags]()
				{
					auto & device = getCurrentRenderDevice( *this );
					auto uboBindings = doCreateUboBindings( flags );
					auto texBindings = doCreateTextureBindings( flags );
					auto uboLayout = device->createDescriptorSetLayout( std::move( uboBindings ) );
					auto texLayout = device->createDescriptorSetLayout( std::move( texBindings ) );
					std::vector< ashes::DescriptorSetLayoutPtr > layouts;
					layouts.emplace_back( std::move( uboLayout ) );
					layouts.emplace_back( std::move( texLayout ) );
					pipeline.setDescriptorSetLayouts( std::move( layouts ) );
					pipeline.initialise( getRenderPass() );
				} ) );
		}
	}

	ashes::VkDescriptorSetLayoutBindingArray RenderPass::doCreateUboBindings( PipelineFlags const & flags )const
	{
		ashes::VkDescriptorSetLayoutBindingArray uboBindings;
		uboBindings.emplace_back( getEngine()->getMaterialCache().getPassBuffer().createLayoutBinding() );

		if ( flags.texturesCount )
		{
			uboBindings.emplace_back( getEngine()->getMaterialCache().getTextureBuffer().createLayoutBinding() );
		}

		if ( checkFlag( flags.programFlags, ProgramFlag::eLighting ) )
		{
			uboBindings.emplace_back( makeDescriptorSetLayoutBinding( getLightBufferIndex()
				, VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER
				, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		}

		uboBindings.emplace_back( makeDescriptorSetLayoutBinding( MatrixUbo::BindingPoint
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, VK_SHADER_STAGE_VERTEX_BIT ) );
		uboBindings.emplace_back( makeDescriptorSetLayoutBinding( SceneUbo::BindingPoint
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT ) );

		if ( !checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) )
		{
			uboBindings.emplace_back( makeDescriptorSetLayoutBinding( ModelMatrixUbo::BindingPoint
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, VK_SHADER_STAGE_VERTEX_BIT ) );
		}

		uboBindings.emplace_back( makeDescriptorSetLayoutBinding( ModelUbo::BindingPoint
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT ) );
		uboBindings.emplace_back( makeDescriptorSetLayoutBinding( TexturesUbo::BindingPoint
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, VK_SHADER_STAGE_FRAGMENT_BIT )  );

		if ( checkFlag( flags.programFlags, ProgramFlag::eSkinning ) )
		{
			uboBindings.push_back( SkinningUbo::createLayoutBinding( SkinningUbo::BindingPoint
				, flags.programFlags ) );
		}

		if ( checkFlag( flags.programFlags, ProgramFlag::eMorphing ) )
		{
			uboBindings.emplace_back( makeDescriptorSetLayoutBinding( MorphingUbo::BindingPoint
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, VK_SHADER_STAGE_VERTEX_BIT ) );
		}

		if ( checkFlag( flags.programFlags, ProgramFlag::ePicking ) )
		{
			uboBindings.emplace_back( makeDescriptorSetLayoutBinding( PickingUbo::BindingPoint
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		}

		if ( checkFlag( flags.programFlags, ProgramFlag::eBillboards ) )
		{
			uboBindings.emplace_back( makeDescriptorSetLayoutBinding( BillboardUbo::BindingPoint
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, VK_SHADER_STAGE_VERTEX_BIT ) );
		}

		return uboBindings;
	}

	ShaderPtr RenderPass::doGetVertexShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		VertexWriter writer;
		// Vertex inputs
		auto position = writer.declInput< Vec4 >( "position"
			, RenderPass::VertexInputs::PositionLocation );
		auto normal = writer.declInput< Vec3 >( "normal"
			, RenderPass::VertexInputs::NormalLocation );
		auto tangent = writer.declInput< Vec3 >( "tangent"
			, RenderPass::VertexInputs::TangentLocation );
		auto uv = writer.declInput< Vec3 >( "uv"
			, RenderPass::VertexInputs::TextureLocation );
		auto bone_ids0 = writer.declInput< IVec4 >( "bone_ids0"
			, RenderPass::VertexInputs::BoneIds0Location
			, checkFlag( flags.programFlags, ProgramFlag::eSkinning ) );
		auto bone_ids1 = writer.declInput< IVec4 >( "bone_ids1"
			, RenderPass::VertexInputs::BoneIds1Location
			, checkFlag( flags.programFlags, ProgramFlag::eSkinning ) );
		auto weights0 = writer.declInput< Vec4 >( "weights0"
			, RenderPass::VertexInputs::Weights0Location
			, checkFlag( flags.programFlags, ProgramFlag::eSkinning ) );
		auto weights1 = writer.declInput< Vec4 >( "weights1"
			, RenderPass::VertexInputs::Weights1Location
			, checkFlag( flags.programFlags, ProgramFlag::eSkinning ) );
		auto transform = writer.declInput< Mat4 >( "transform"
			, RenderPass::VertexInputs::TransformLocation
			, checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) );
		auto material = writer.declInput< Int >( "material"
			, RenderPass::VertexInputs::MaterialLocation
			, checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) );
		auto position2 = writer.declInput< Vec4 >( "position2"
			, RenderPass::VertexInputs::Position2Location
			, checkFlag( flags.programFlags, ProgramFlag::eMorphing ) );
		auto normal2 = writer.declInput< Vec3 >( "normal2"
			, RenderPass::VertexInputs::Normal2Location
			, checkFlag( flags.programFlags, ProgramFlag::eMorphing ) );
		auto tangent2 = writer.declInput< Vec3 >( "tangent2"
			, RenderPass::VertexInputs::Tangent2Location
			, checkFlag( flags.programFlags, ProgramFlag::eMorphing ) );
		auto texture2 = writer.declInput< Vec3 >( "texture2"
			, RenderPass::VertexInputs::Texture2Location
			, checkFlag( flags.programFlags, ProgramFlag::eMorphing ) );
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
			, RenderPass::VertexOutputs::TextureLocation );
		auto vtx_instance = writer.declOutput< UInt >( "vtx_instance"
			, RenderPass::VertexOutputs::InstanceLocation );
		auto vtx_material = writer.declOutput< UInt >( "vtx_material"
			, RenderPass::VertexOutputs::MaterialLocation );
		auto out = writer.getOut();

		writer.implementFunction< sdw::Void >( "main"
			, [&]()
			{
				auto v4Vertex = writer.declLocale( "v4Vertex"
					, vec4( position.xyz(), 1.0_f ) );
				auto v4Normal = writer.declLocale( "v4Normal"
					, vec4( normal, 0.0_f ) );
				auto v4Tangent = writer.declLocale( "v4Tangent"
					, vec4( tangent, 0.0_f ) );
				auto v3Texture = writer.declLocale( "v3Texture"
					, uv );
				auto curMtxModel = writer.declLocale< Mat4 >( "curMtxModel" );
				auto prvMtxModel = writer.declLocale< Mat4 >( "prvMtxModel" );

				if ( checkFlag( flags.programFlags, ProgramFlag::eSkinning ) )
				{
					curMtxModel = SkinningUbo::computeTransform( skinningData, writer, flags.programFlags );
					prvMtxModel = curMtxModel;
				}
				else if ( checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) )
				{
					curMtxModel = transform;
					prvMtxModel = curMtxModel;
				}
				else
				{
					curMtxModel = c3d_curMtxModel;
					prvMtxModel = c3d_prvMtxModel;
				}

				auto curMtxNormal = writer.declLocale( "curMtxNormal"
					, transpose( inverse( mat3( curMtxModel ) ) ) );
				auto prvMtxNormal = writer.declLocale( "prvMtxNormal"
					, transpose( inverse( mat3( prvMtxModel ) ) ) );

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
					auto time = writer.declLocale( "time"
						, vec3( 1.0_f - c3d_time ) );
					v4Vertex = vec4( sdw::fma( v4Vertex.xyz(), time, position2.xyz() * c3d_time ), 1.0_f );
					v4Normal = vec4( sdw::fma( v4Normal.xyz(), time, normal2.xyz() * c3d_time ), 1.0_f );
					v4Tangent = vec4( sdw::fma( v4Tangent.xyz(), time, tangent2.xyz() * c3d_time ), 1.0_f );
					v3Texture = sdw::fma( v3Texture, time, texture2 * c3d_time );
				}

				vtx_texture = v3Texture;
				v4Vertex = curMtxModel * v4Vertex;
				vtx_worldPosition = v4Vertex.xyz();
				v4Vertex = c3d_curView * v4Vertex;
				auto mtxNormal = writer.getVariable< Mat3 >( "mtxNormal" );

				if ( checkFlag( flags.programFlags, ProgramFlag::eInvertNormals ) )
				{
					vtx_normal = normalize( mtxNormal * -v4Normal.xyz() );
				}
				else
				{
					vtx_normal = normalize( mtxNormal * v4Normal.xyz() );
				}

				vtx_tangent = normalize( mtxNormal * v4Tangent.xyz() );
				vtx_tangent = normalize( sdw::fma( -vtx_normal, vec3( dot( vtx_tangent, vtx_normal ) ), vtx_tangent ) );
				vtx_bitangent = cross( vtx_normal, vtx_tangent );
				vtx_instance = writer.cast< UInt >( in.instanceIndex );
				out.vtx.position = c3d_projection * v4Vertex;

				auto tbn = writer.declLocale( "tbn"
					, transpose( mat3( vtx_tangent, vtx_bitangent, vtx_normal ) ) );
				vtx_tangentSpaceFragPosition = tbn * vtx_worldPosition;
				vtx_tangentSpaceViewPosition = tbn * c3d_cameraPosition.xyz();
			} );
		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr RenderPass::doGetBillboardShaderSource( PipelineFlags const & flags )const
	{
		ShaderProgramSPtr result = std::make_shared< ShaderProgram >( *getEngine()->getRenderSystem() );
		auto & engine = *getEngine();
		using namespace sdw;
		VertexWriter writer;

		// Shader inputs
		auto position = writer.declInput< Vec4 >( "position", 0u );
		auto uv = writer.declInput< Vec2 >( "uv", 1u );
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
			, RenderPass::VertexOutputs::TextureLocation );
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

				vtx_texture = vec3( uv, 0.0_f );
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
