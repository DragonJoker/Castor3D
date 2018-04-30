#include "RenderPass.hpp"

#include "Castor3DPrerequisites.hpp"
#include "Engine.hpp"
#include "Material/Material.hpp"
#include "Material/Pass.hpp"
#include "Mesh/Submesh.hpp"
#include "Render/RenderPassTimer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderNode/RenderNode_Render.hpp"
#include "Scene/BillboardList.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Geometry.hpp"
#include "Scene/Scene.hpp"
#include "Scene/SceneNode.hpp"
#include "Scene/Animation/AnimatedSkeleton.hpp"
#include "Shader/PassBuffer/PassBuffer.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Shader/Shaders/GlslMaterial.hpp"

#include <Buffer/Buffer.hpp>
#include <Buffer/VertexBuffer.hpp>

#include <GlslSource.hpp>

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		template< typename MapType, typename FuncType >
		inline void doTraverseNodes( RenderPass const & pass
			, MapType & nodes
			, FuncType function )
		{
			for ( auto & itPipelines : nodes )
			{
				for ( auto & itPass : itPipelines.second )
				{
					for ( auto & itSubmeshes : itPass.second )
					{
						function( *itPipelines.first
							, *itPass.first
							, *itSubmeshes.first
							, itSubmeshes.first->getInstantiation()
							, itSubmeshes.second );
					}
				}
			}
		}

		template< typename MapType, typename FuncType >
		inline void doTraverseNodes( RenderPass const & pass
			, MapType & nodes
			, Scene & scene
			, ShadowMapLightTypeArray & shadowMaps
			, FuncType function )
		{
			for ( auto & itPipelines : nodes )
			{
				for ( auto & itPass : itPipelines.second )
				{
					for ( auto & itSubmeshes : itPass.second )
					{
						function( *itPipelines.first
							, *itPass.first
							, *itSubmeshes.first
							, itSubmeshes.first->getInstantiation()
							, itSubmeshes.second );
					}
				}
			}
		}

		template< typename MapType, typename FuncType >
		inline void doTraverseNodes( RenderPass const & pass
			, Camera const & camera
			, MapType & nodes
			, FuncType function )
		{
			for ( auto & itPipelines : nodes )
			{
				pass.updatePipeline( *itPipelines.first );

				for ( auto & itPass : itPipelines.second )
				{
					for ( auto & itSubmeshes : itPass.second )
					{
						function( *itPipelines.first
							, *itPass.first
							, *itSubmeshes.first
							, itSubmeshes.first->getInstantiation()
							, itSubmeshes.second );
					}
				}
			}
		}

		template< typename MapType, typename FuncType >
		inline void doTraverseNodes( RenderPass const & pass
			, Camera const & camera
			, MapType & nodes
			, Scene & scene
			, ShadowMapLightTypeArray & shadowMaps
			, FuncType function )
		{
			for ( auto & itPipelines : nodes )
			{
				pass.updatePipeline( *itPipelines.first );

				for ( auto & itPass : itPipelines.second )
				{
					for ( auto & itSubmeshes : itPass.second )
					{
						function( *itPipelines.first
							, *itPass.first
							, *itSubmeshes.first
							, itSubmeshes.first->getInstantiation()
							, itSubmeshes.second );
					}
				}
			}
		}

		template< typename MapType >
		inline void doRenderNonInstanced( RenderPass const & pass
			, Camera const & camera
			, MapType & nodes )
		{
			for ( auto & itPipelines : nodes )
			{
				pass.updatePipeline( *itPipelines.first );
			}
		}

		template< typename MapType >
		inline void doRenderNonInstanced( RenderPass const & pass
			, Camera const & camera
			, MapType & nodes
			, Scene & scene
			, ShadowMapLightTypeArray & shadowMaps )
		{
			for ( auto & itPipelines : nodes )
			{
				pass.updatePipeline( *itPipelines.first );
			}
		}

		template< typename MapType >
		inline void doRenderNonInstanced( RenderPass const & pass
			, Camera const & camera
			, MapType & nodes
			, Scene & scene
			, ShadowMapLightTypeArray & shadowMaps
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

		template< typename ArrayT >
		uint32_t copyNodesMatrices( ArrayT const & renderNodes
			, std::vector< InstantiationData > & matrixBuffer )
		{
			auto const mtxSize = sizeof( float ) * 16;
			auto const stride = sizeof( InstantiationData );
			auto const count = std::min( uint32_t( matrixBuffer.size() )
				, uint32_t( renderNodes.size() ) );
			auto buffer = matrixBuffer.data();
			auto it = renderNodes.begin();
			auto i = 0u;

			while ( i < count )
			{
				auto & node = *it;
				buffer->m_matrix = convert( node.sceneNode.getDerivedTransformationMatrix() );
				buffer->m_material = node.passNode.pass.getId();
				++buffer;
				++i;
				++it;
			}

			return count;
		}

		template< typename ArrayT >
		uint32_t copyNodesMatrices( ArrayT const & renderNodes
			, Camera const & camera
			, std::vector< InstantiationData > & matrixBuffer )
		{
			auto const mtxSize = sizeof( float ) * 16;
			auto const stride = sizeof( InstantiationData );
			auto const count = std::min( uint32_t( matrixBuffer.size() )
				, uint32_t( renderNodes.size() ) );
			auto buffer = matrixBuffer.data();
			auto it = renderNodes.begin();
			auto i = 0u;

			while ( i < count )
			{
				auto & node = *it;
				if ( node->sceneNode.isDisplayable()
					&& node->sceneNode.isVisible()
					&& camera.isVisible( node->instance, node->data ) )
				{
					buffer->m_matrix = convert( node->sceneNode.getDerivedTransformationMatrix() );
					buffer->m_material = node->passNode.pass.getId();
					++buffer;
				}
				++i;
				++it;
			}

			return count;
		}
	}

	//*********************************************************************************************

	RenderPass::RenderPass( String const & category
		, String const & name
		, Engine & engine
		, SceneNode const * ignored )
		: OwnedBy< Engine >{ engine }
		, Named{ name }
		, m_renderSystem{ *engine.getRenderSystem() }
		, m_category{ category }
		, m_oit{ true }
		, m_renderQueue{ *this, true, ignored }
		, m_opaque{ true }
		, m_matrixUbo{ engine }
		, m_sceneUbo{ engine }
	{
	}

	RenderPass::RenderPass( String const & category
		, String const & name
		, Engine & engine
		, bool oit
		, SceneNode const * ignored )
		: OwnedBy< Engine >{ engine }
		, Named{ name }
		, m_renderSystem{ *engine.getRenderSystem() }
		, m_category{ category }
		, m_oit{ oit }
		, m_renderQueue{ *this, false, ignored }
		, m_opaque{ false }
		, m_matrixUbo{ engine }
		, m_sceneUbo{ engine }
	{
	}

	RenderPass::~RenderPass()
	{
	}

	bool RenderPass::initialise( Size const & size )
	{
		m_timer = std::make_shared< RenderPassTimer >( *getEngine(), m_category, getName() );
		m_matrixUbo.initialise();
		m_sceneUbo.initialise();
		m_sceneUbo.setWindowSize( size );
		return doInitialise( size );
	}

	void RenderPass::cleanup()
	{
		m_matrixUbo.cleanup();
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

	glsl::Shader RenderPass::getVertexShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, bool invertNormals )const
	{
		return doGetVertexShaderSource( passFlags
			, textureFlags
			, programFlags
			, sceneFlags
			, invertNormals );
	}

	glsl::Shader RenderPass::getGeometryShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags )const
	{
		return doGetGeometryShaderSource( passFlags
			, textureFlags
			, programFlags
			, sceneFlags );
	}

	glsl::Shader RenderPass::getPixelShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, renderer::CompareOp alphaFunc )const
	{
		glsl::Shader result;

		if ( checkFlag( passFlags, PassFlag::ePbrMetallicRoughness ) )
		{
			result = doGetPbrMRPixelShaderSource( passFlags
				, textureFlags
				, programFlags
				, sceneFlags
				, alphaFunc );
		}
		else if ( checkFlag( passFlags, PassFlag::ePbrSpecularGlossiness ) )
		{
			result = doGetPbrSGPixelShaderSource( passFlags
				, textureFlags
				, programFlags
				, sceneFlags
				, alphaFunc );
		}
		else
		{
			result = doGetLegacyPixelShaderSource( passFlags
				, textureFlags
				, programFlags
				, sceneFlags
				, alphaFunc );
		}

		return result;
	}

	void RenderPass::preparePipeline( BlendMode colourBlendMode
		, BlendMode alphaBlendMode
		, renderer::CompareOp alphaFunc
		, PassFlags & passFlags
		, TextureChannels & textureFlags
		, ProgramFlags & programFlags
		, SceneFlags & sceneFlags
		, renderer::PrimitiveTopology topology
		, bool twoSided
		, renderer::VertexLayoutCRefArray const & layouts )
	{
		doUpdateFlags( passFlags
			, textureFlags
			, programFlags
			, sceneFlags );

		if ( checkFlag( passFlags, PassFlag::eAlphaBlending ) != m_opaque
			&& ( !checkFlag( programFlags, ProgramFlag::eBillboards )
				|| !isShadowMapProgram( programFlags ) ) )
		{
			if ( m_opaque )
			{
				alphaBlendMode = BlendMode::eNoBlend;
			}

			auto & backProgram = doGetProgram( passFlags
				, textureFlags
				, programFlags
				, sceneFlags
				, alphaFunc
				, false );

			if ( !m_opaque )
			{
				auto & frontProgram = doGetProgram( passFlags
					, textureFlags
					, programFlags
					, sceneFlags
					, alphaFunc
					, true );
				auto flags = PipelineFlags{ colourBlendMode
					, alphaBlendMode
					, passFlags
					, textureFlags
					, programFlags
					, sceneFlags
					, topology };
				doPrepareFrontPipeline( frontProgram, layouts, flags );
				doPrepareBackPipeline( backProgram, layouts, flags );
			}
			else
			{
				auto flags = PipelineFlags{ colourBlendMode
					, alphaBlendMode
					, passFlags
					, textureFlags
					, programFlags
					, sceneFlags
					, topology };

				if ( twoSided || checkFlag( textureFlags, TextureChannel::eOpacity ) )
				{
					auto & frontProgram = doGetProgram( passFlags
						, textureFlags
						, programFlags
						, sceneFlags
						, alphaFunc
						, true );
					doPrepareFrontPipeline( frontProgram, layouts, flags );
				}

				doPrepareBackPipeline( backProgram, layouts, flags );
			}
		}
	}

	RenderPipeline * RenderPass::getPipelineFront( BlendMode colourBlendMode
		, BlendMode alphaBlendMode
		, renderer::CompareOp alphaFunc
		, PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, renderer::PrimitiveTopology topology )const
	{
		if ( m_opaque )
		{
			alphaBlendMode = BlendMode::eNoBlend;
		}

		auto & pipelines = doGetFrontPipelines();
		auto it = pipelines.find( { colourBlendMode, alphaBlendMode, passFlags, textureFlags, programFlags, sceneFlags, topology } );
		RenderPipeline * result{ nullptr };

		if ( it != pipelines.end() )
		{
			result = it->second.get();
		}

		return result;
	}

	RenderPipeline * RenderPass::getPipelineBack( BlendMode colourBlendMode
		, BlendMode alphaBlendMode
		, renderer::CompareOp alphaFunc
		, PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, renderer::PrimitiveTopology topology )const
	{
		if ( m_opaque )
		{
			alphaBlendMode = BlendMode::eNoBlend;
		}

		auto & pipelines = doGetBackPipelines();
		auto it = pipelines.find( { colourBlendMode, alphaBlendMode, passFlags, textureFlags, programFlags, sceneFlags, topology } );
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
		auto & geometryEntry = scene.getGeometryCache().getUbos( primitive, submesh, pass );
		auto & animationEntry = scene.getAnimatedObjectGroupCache().getUbos( skeleton );

		return SkinningRenderNode
		{
			pipeline,
			doCreatePassRenderNode( pass, pipeline ),
			geometryEntry.modelMatrixUbo,
			geometryEntry.modelUbo,
			geometryEntry.pickingUbo,
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
		auto & geometryEntry = scene.getGeometryCache().getUbos( primitive, submesh, pass );
		auto & animationEntry = scene.getAnimatedObjectGroupCache().getUbos( mesh );

		return MorphingRenderNode
		{
			pipeline,
			doCreatePassRenderNode( pass, pipeline ),
			geometryEntry.modelMatrixUbo,
			geometryEntry.modelUbo,
			geometryEntry.pickingUbo,
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
			buffers,
			*billboard.getNode(),
			billboard
		};
	}

	void RenderPass::updatePipeline( RenderPipeline & pipeline )const
	{
		doUpdatePipeline( pipeline );
	}

	void RenderPass::updateFlags( PassFlags & passFlags
		, TextureChannels & textureFlags
		, ProgramFlags & programFlags
		, SceneFlags & sceneFlags )const
	{
		doUpdateFlags( passFlags
			, textureFlags
			, programFlags
			, sceneFlags );
	}

	void RenderPass::initialiseUboDescriptor( renderer::DescriptorSetPool const & descriptorPool
		, BillboardRenderNode & node )
	{
		auto & layout = descriptorPool.getLayout();
		uint32_t index = 0u;
		node.uboDescriptorSet = descriptorPool.createDescriptorSet( 0u );
		getEngine()->getMaterialCache().getPassBuffer().createBinding( *node.uboDescriptorSet
			, layout.getBinding( index++ ) );

		if ( checkFlag( node.pipeline.getFlags().programFlags, ProgramFlag::eLighting ) )
		{
			node.uboDescriptorSet->createBinding( layout.getBinding( LightBufferIndex )
				, node.sceneNode.getScene()->getLightCache().getBuffer()
				, node.sceneNode.getScene()->getLightCache().getView() );
		}

		node.uboDescriptorSet->createBinding( layout.getBinding( MatrixUbo::BindingPoint )
			, m_matrixUbo.getUbo() );
		node.uboDescriptorSet->createBinding( layout.getBinding( SceneUbo::BindingPoint )
			, m_sceneUbo.getUbo() );

		if ( !checkFlag( node.pipeline.getFlags().programFlags, ProgramFlag::eInstantiation ) )
		{
			node.uboDescriptorSet->createBinding( layout.getBinding( ModelMatrixUbo::BindingPoint )
				, node.modelMatrixUbo.buffer->getBuffer()
				, node.modelMatrixUbo.offset
				, 1u );
		}

		node.uboDescriptorSet->createBinding( layout.getBinding( ModelUbo::BindingPoint )
			, node.modelUbo.buffer->getBuffer()
			, node.modelUbo.offset
			, 1u );
		node.uboDescriptorSet->createBinding( layout.getBinding( BillboardUbo::BindingPoint )
			, node.billboardUbo.buffer->getBuffer()
			, node.billboardUbo.offset
			, 1u );
		doFillUboDescriptor( layout, index, node );
		node.uboDescriptorSet->update();
	}

	void RenderPass::initialiseUboDescriptor( renderer::DescriptorSetPool const & descriptorPool
		, MorphingRenderNode & node )
	{
		auto & layout = descriptorPool.getLayout();
		uint32_t index = 0u;
		node.uboDescriptorSet = descriptorPool.createDescriptorSet( 0u );
		getEngine()->getMaterialCache().getPassBuffer().createBinding( *node.uboDescriptorSet
			, layout.getBinding( index++ ) );

		if ( checkFlag( node.pipeline.getFlags().programFlags, ProgramFlag::eLighting ) )
		{
			node.uboDescriptorSet->createBinding( layout.getBinding( LightBufferIndex )
				, node.sceneNode.getScene()->getLightCache().getBuffer()
				, node.sceneNode.getScene()->getLightCache().getView() );
		}

		node.uboDescriptorSet->createBinding( layout.getBinding( MatrixUbo::BindingPoint )
			, m_matrixUbo.getUbo() );
		node.uboDescriptorSet->createBinding( layout.getBinding( SceneUbo::BindingPoint )
			, m_sceneUbo.getUbo() );

		if ( !checkFlag( node.pipeline.getFlags().programFlags, ProgramFlag::eInstantiation ) )
		{
			node.uboDescriptorSet->createBinding( layout.getBinding( ModelMatrixUbo::BindingPoint )
				, node.modelMatrixUbo.buffer->getBuffer()
				, node.modelMatrixUbo.offset
				, 1u );
		}

		node.uboDescriptorSet->createBinding( layout.getBinding( ModelUbo::BindingPoint )
			, node.modelUbo.buffer->getBuffer()
			, node.modelUbo.offset
			, 1u );
		node.uboDescriptorSet->createBinding( layout.getBinding( MorphingUbo::BindingPoint )
			, node.morphingUbo.buffer->getBuffer()
			, node.morphingUbo.offset
			, 1u );
		doFillUboDescriptor( layout, index, node );
		node.uboDescriptorSet->update();
	}

	void RenderPass::initialiseUboDescriptor( renderer::DescriptorSetPool const & descriptorPool
		, SkinningRenderNode & node )
	{
		auto & layout = descriptorPool.getLayout();
		uint32_t index = 0u;
		node.uboDescriptorSet = descriptorPool.createDescriptorSet( 0u );
		getEngine()->getMaterialCache().getPassBuffer().createBinding( *node.uboDescriptorSet
			, layout.getBinding( index++ ) );

		if ( checkFlag( node.pipeline.getFlags().programFlags, ProgramFlag::eLighting ) )
		{
			node.uboDescriptorSet->createBinding( layout.getBinding( LightBufferIndex )
				, node.sceneNode.getScene()->getLightCache().getBuffer()
				, node.sceneNode.getScene()->getLightCache().getView() );
		}

		node.uboDescriptorSet->createBinding( layout.getBinding( MatrixUbo::BindingPoint )
			, m_matrixUbo.getUbo() );
		node.uboDescriptorSet->createBinding( layout.getBinding( SceneUbo::BindingPoint )
			, m_sceneUbo.getUbo() );

		if ( !checkFlag( node.pipeline.getFlags().programFlags, ProgramFlag::eInstantiation ) )
		{
			node.uboDescriptorSet->createBinding( layout.getBinding( ModelMatrixUbo::BindingPoint )
				, node.modelMatrixUbo.buffer->getBuffer()
				, node.modelMatrixUbo.offset
				, 1u );
		}

		node.uboDescriptorSet->createBinding( layout.getBinding( ModelUbo::BindingPoint )
			, node.modelUbo.buffer->getBuffer()
			, node.modelUbo.offset
			, 1u );
		node.uboDescriptorSet->createBinding( layout.getBinding( SkinningUbo::BindingPoint )
			, node.skinningUbo.buffer->getBuffer()
			, node.skinningUbo.offset
			, 1u );
		doFillUboDescriptor( layout, index, node );
		node.uboDescriptorSet->update();
	}

	void RenderPass::initialiseUboDescriptor( renderer::DescriptorSetPool const & descriptorPool
		, StaticRenderNode & node )
	{
		auto & layout = descriptorPool.getLayout();
		uint32_t index = 0u;
		node.uboDescriptorSet = descriptorPool.createDescriptorSet( 0u );
		getEngine()->getMaterialCache().getPassBuffer().createBinding( *node.uboDescriptorSet
			, layout.getBinding( index++ ) );

		if ( checkFlag( node.pipeline.getFlags().programFlags, ProgramFlag::eLighting ) )
		{
			node.uboDescriptorSet->createBinding( layout.getBinding( LightBufferIndex )
				, node.sceneNode.getScene()->getLightCache().getBuffer()
				, node.sceneNode.getScene()->getLightCache().getView() );
		}

		node.uboDescriptorSet->createBinding( layout.getBinding( MatrixUbo::BindingPoint )
			, m_matrixUbo.getUbo() );
		node.uboDescriptorSet->createBinding( layout.getBinding( SceneUbo::BindingPoint )
			, m_sceneUbo.getUbo() );

		if ( !checkFlag( node.pipeline.getFlags().programFlags, ProgramFlag::eInstantiation ) )
		{
			node.uboDescriptorSet->createBinding( layout.getBinding( ModelMatrixUbo::BindingPoint )
				, node.modelMatrixUbo.buffer->getBuffer()
				, node.modelMatrixUbo.offset
				, 1u );
		}

		node.uboDescriptorSet->createBinding( layout.getBinding( ModelUbo::BindingPoint )
			, node.modelUbo.buffer->getBuffer()
			, node.modelUbo.offset
			, 1u );
		doFillUboDescriptor( layout, index, node );
		node.uboDescriptorSet->update();
	}

	void RenderPass::initialiseUboDescriptor( renderer::DescriptorSetPool const & descriptorPool
		, SubmeshSkinninRenderNodesByPassMap & nodes )
	{
		for ( auto & passNodes : nodes )
		{
			for ( auto & submeshNodes : passNodes.second )
			{
				initialiseUboDescriptor( descriptorPool, submeshNodes.second[0] );
			}
		}
	}

	void RenderPass::initialiseUboDescriptor( renderer::DescriptorSetPool const & descriptorPool
		, SubmeshStaticRenderNodesByPassMap & nodes )
	{
		for ( auto & passNodes : nodes )
		{
			for ( auto & submeshNodes : passNodes.second )
			{
				initialiseUboDescriptor( descriptorPool, submeshNodes.second[0] );
			}
		}
	}

	void RenderPass::initialiseTextureDescriptor( renderer::DescriptorSetPool const & descriptorPool
		, BillboardRenderNode & node )
	{
		auto & layout = descriptorPool.getLayout();
		uint32_t index = MinBufferIndex;
		node.texDescriptorSet = descriptorPool.createDescriptorSet( 1u );
		doFillTextureDescriptor( layout, index, node );
		node.texDescriptorSet->update();
	}

	void RenderPass::initialiseTextureDescriptor( renderer::DescriptorSetPool const & descriptorPool
		, MorphingRenderNode & node )
	{
		auto & layout = descriptorPool.getLayout();
		uint32_t index = MinBufferIndex;
		node.texDescriptorSet = descriptorPool.createDescriptorSet( 1u );
		doFillTextureDescriptor( layout, index, node );
		node.texDescriptorSet->update();
	}

	void RenderPass::initialiseTextureDescriptor( renderer::DescriptorSetPool const & descriptorPool
		, SkinningRenderNode & node )
	{
		auto & layout = descriptorPool.getLayout();
		uint32_t index = MinBufferIndex;
		node.texDescriptorSet = descriptorPool.createDescriptorSet( 1u );
		doFillTextureDescriptor( layout, index, node );
		node.texDescriptorSet->update();
	}

	void RenderPass::initialiseTextureDescriptor( renderer::DescriptorSetPool const & descriptorPool
		, StaticRenderNode & node )
	{
		auto & layout = descriptorPool.getLayout();
		uint32_t index = MinBufferIndex;
		node.texDescriptorSet = descriptorPool.createDescriptorSet( 1u );
		doFillTextureDescriptor( layout, index, node );
		node.texDescriptorSet->update();
	}

	void RenderPass::initialiseTextureDescriptor( renderer::DescriptorSetPool const & descriptorPool
		, SubmeshSkinninRenderNodesByPassMap & nodes )
	{
		for ( auto & passNodes : nodes )
		{
			for ( auto & submeshNodes : passNodes.second )
			{
				Pass & pass = submeshNodes.second[0].passNode.pass;

				if ( pass.getTextureUnitsCount() > 0u )
				{
					initialiseTextureDescriptor( descriptorPool, submeshNodes.second[0] );
				}
			}
		}
	}

	void RenderPass::initialiseTextureDescriptor( renderer::DescriptorSetPool const & descriptorPool
		, SubmeshStaticRenderNodesByPassMap & nodes )
	{
		for ( auto & passNodes : nodes )
		{
			for ( auto & submeshNodes : passNodes.second )
			{
				Pass & pass = submeshNodes.second[0].passNode.pass;

				if ( pass.getTextureUnitsCount() > 0u )
				{
					initialiseTextureDescriptor( descriptorPool, submeshNodes.second[0] );
				}
			}
		}
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

	ShaderProgramSPtr RenderPass::doGetProgram( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, renderer::CompareOp alphaFunc
		, bool invertNormals )const
	{
		return getEngine()->getShaderProgramCache().getAutomaticProgram( *this
			, passFlags
			, textureFlags
			, programFlags
			, sceneFlags
			, alphaFunc
			, invertNormals );
	}

	uint32_t RenderPass::doCopyNodesMatrices( StaticRenderNodeArray const & renderNodes
		, std::vector< InstantiationData > & matrixBuffer )const
	{
		return copyNodesMatrices( renderNodes
			, matrixBuffer );
	}

	uint32_t RenderPass::doCopyNodesMatrices( StaticRenderNodeArray const & renderNodes
		, std::vector< InstantiationData > & matrixBuffer
		, RenderInfo & info )const
	{
		auto count = copyNodesMatrices( renderNodes
			, matrixBuffer );
		info.m_visibleObjectsCount += count;
		return count;
	}

	uint32_t RenderPass::doCopyNodesMatrices( SkinningRenderNodeArray const & renderNodes
		, std::vector< InstantiationData > & matrixBuffer )const
	{
		return copyNodesMatrices( renderNodes
			, matrixBuffer );
	}

	uint32_t RenderPass::doCopyNodesMatrices( SkinningRenderNodeArray const & renderNodes
		, std::vector< InstantiationData > & matrixBuffer
		, RenderInfo & info )const
	{
		auto count = copyNodesMatrices( renderNodes
			, matrixBuffer );
		info.m_visibleObjectsCount += count;
		return count;
	}

	uint32_t RenderPass::doCopyNodesMatrices( StaticRenderNodePtrArray const & renderNodes
		, Camera const & camera
		, std::vector< InstantiationData > & matrixBuffer )const
	{
		return copyNodesMatrices( renderNodes
			, camera
			, matrixBuffer );
	}

	uint32_t RenderPass::doCopyNodesMatrices( StaticRenderNodePtrArray const & renderNodes
		, Camera const & camera
		, std::vector< InstantiationData > & matrixBuffer
		, RenderInfo & info )const
	{
		auto count = copyNodesMatrices( renderNodes
			, camera
			, matrixBuffer );
		info.m_visibleObjectsCount += count;
		return count;
	}

	uint32_t RenderPass::doCopyNodesMatrices( SkinningRenderNodePtrArray const & renderNodes
		, Camera const & camera
		, std::vector< InstantiationData > & matrixBuffer )const
	{
		return copyNodesMatrices( renderNodes
			, camera
			, matrixBuffer );
	}

	uint32_t RenderPass::doCopyNodesMatrices( SkinningRenderNodePtrArray const & renderNodes
		, Camera const & camera
		, std::vector< InstantiationData > & matrixBuffer
		, RenderInfo & info )const
	{
		auto count = copyNodesMatrices( renderNodes
			, camera
			, matrixBuffer );
		info.m_visibleObjectsCount += count;
		return count;
	}

	uint32_t RenderPass::doCopyNodesBones( SkinningRenderNodeArray const & renderNodes
		, ShaderBuffer & bonesBuffer )const
	{
		uint32_t const mtxSize = sizeof( float ) * 16;
		uint32_t const stride = mtxSize * 400u;
		auto const count = std::min( bonesBuffer.getSize() / stride, uint32_t( renderNodes.size() ) );
		REQUIRE( count <= renderNodes.size() );
		auto buffer = bonesBuffer.getPtr();
		auto it = renderNodes.begin();
		auto i = 0u;

		while ( i < count )
		{
			auto & node = *it;
			node.skeleton.fillBuffer( buffer );
			buffer += stride;
			++i;
			++it;
		}

		bonesBuffer.update( 0u, stride * count );
		return count;
	}

	uint32_t RenderPass::doCopyNodesBones( SkinningRenderNodeArray const & renderNodes
		, ShaderBuffer & bonesBuffer
		, RenderInfo & info )const
	{
		auto count = doCopyNodesBones( renderNodes, bonesBuffer );
		info.m_visibleObjectsCount += count;
		return count;
	}

	uint32_t RenderPass::doCopyNodesBones( SkinningRenderNodePtrArray const & renderNodes
		, Camera const & camera
		, ShaderBuffer & bonesBuffer )const
	{
		uint32_t const mtxSize = sizeof( float ) * 16;
		uint32_t const stride = mtxSize * 400u;
		auto const count = std::min( bonesBuffer.getSize() / stride, uint32_t( renderNodes.size() ) );
		REQUIRE( count <= renderNodes.size() );
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
		, Camera const & camera
		, ShaderBuffer & bonesBuffer
		, RenderInfo & info )const
	{
		auto count = doCopyNodesBones( renderNodes, camera, bonesBuffer );
		info.m_visibleObjectsCount += count;
		return count;
	}

	void RenderPass::doUpdate( SubmeshStaticRenderNodesByPipelineMap & nodes )const
	{
		doTraverseNodes( *this
			, nodes
			, [this]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, InstantiationComponent & instantiation
				, StaticRenderNodeArray & renderNodes )
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

	void RenderPass::doUpdate( SubmeshStaticRenderNodesByPipelineMap & nodes
		, ShadowMapLightTypeArray & shadowMaps )const
	{
		doTraverseNodes( *this
			, nodes
			, *getEngine()->getRenderSystem()->getTopScene()
			, shadowMaps
			, [this]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, InstantiationComponent & instantiation
				, StaticRenderNodeArray & renderNodes )
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
		, Camera const & camera )const
	{
		doTraverseNodes( *this
			, camera
			, nodes
			, [this, &camera]( RenderPipeline & pipeline
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
						, camera
						, it->second.data );
				}
			} );
	}

	void RenderPass::doUpdate( SubmeshStaticRenderNodesPtrByPipelineMap & nodes
		, Camera const & camera
		, ShadowMapLightTypeArray & shadowMaps )const
	{
		doTraverseNodes( *this
			, camera
			, nodes
			, *getEngine()->getRenderSystem()->getTopScene()
			, shadowMaps
			, [this, &camera]( RenderPipeline & pipeline
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
						, camera
						, it->second.data );
				}
			} );
	}

	void RenderPass::doUpdate( SubmeshStaticRenderNodesPtrByPipelineMap & nodes
		, Camera const & camera
		, ShadowMapLightTypeArray & shadowMaps
		, RenderInfo & info )const
	{
		doTraverseNodes( *this
			, camera
			, nodes
			, *getEngine()->getRenderSystem()->getTopScene()
			, shadowMaps
			, [this, &camera, &info]( RenderPipeline & pipeline
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
						, camera
						, it->second.data
						, info );
					info.m_visibleFaceCount += submesh.getFaceCount() * count;
					info.m_visibleVertexCount += submesh.getPointsCount() * count;
					++info.m_drawCalls;
				}
			} );
	}

	void RenderPass::doUpdate( StaticRenderNodesByPipelineMap & nodes )const
	{
	}

	void RenderPass::doUpdate( StaticRenderNodesByPipelineMap & nodes
		, ShadowMapLightTypeArray & shadowMaps )const
	{
	}

	void RenderPass::doUpdate( StaticRenderNodesPtrByPipelineMap & nodes
		, Camera const & camera )const
	{
		doRenderNonInstanced( *this
			, camera
			, nodes );
	}

	void RenderPass::doUpdate( StaticRenderNodesPtrByPipelineMap & nodes
		, Camera const & camera
		, ShadowMapLightTypeArray & shadowMaps )const
	{
		doRenderNonInstanced( *this
			, camera
			, nodes
			, *getEngine()->getRenderSystem()->getTopScene()
			, shadowMaps );
	}

	void RenderPass::doUpdate( StaticRenderNodesPtrByPipelineMap & nodes
		, Camera const & camera
		, ShadowMapLightTypeArray & shadowMaps
		, RenderInfo & info )const
	{
		doRenderNonInstanced( *this
			, camera
			, nodes
			, *getEngine()->getRenderSystem()->getTopScene()
			, shadowMaps
			, info );
	}

	void RenderPass::doUpdate( SkinningRenderNodesByPipelineMap & nodes )const
	{
	}

	void RenderPass::doUpdate( SkinningRenderNodesByPipelineMap & nodes
		, ShadowMapLightTypeArray & shadowMaps )const
	{
	}

	void RenderPass::doUpdate( SkinningRenderNodesPtrByPipelineMap & nodes
		, Camera const & camera )const
	{
		doRenderNonInstanced( *this
			, camera
			, nodes );
	}

	void RenderPass::doUpdate( SkinningRenderNodesPtrByPipelineMap & nodes
		, Camera const & camera
		, ShadowMapLightTypeArray & shadowMaps )const
	{
		doRenderNonInstanced( *this
			, camera
			, nodes
			, *getEngine()->getRenderSystem()->getTopScene()
			, shadowMaps );
	}

	void RenderPass::doUpdate( SkinningRenderNodesPtrByPipelineMap & nodes
		, Camera const & camera
		, ShadowMapLightTypeArray & shadowMaps
		, RenderInfo & info )const
	{
		doRenderNonInstanced( *this
			, camera
			, nodes
			, *getEngine()->getRenderSystem()->getTopScene()
			, shadowMaps
			, info );
	}

	void RenderPass::doUpdate( SubmeshSkinningRenderNodesByPipelineMap & nodes )const
	{
		doTraverseNodes( *this
			, nodes
			, [this]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, InstantiationComponent & instantiation
				, SkinningRenderNodeArray & renderNodes )
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
					REQUIRE( count1 == count2 );
				}
			} );
	}

	void RenderPass::doUpdate( SubmeshSkinningRenderNodesByPipelineMap & nodes
		, ShadowMapLightTypeArray & shadowMaps )const
	{
		doTraverseNodes( *this
			, nodes
			, *getEngine()->getRenderSystem()->getTopScene()
			, shadowMaps
			, [this]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, InstantiationComponent & instantiation
				, SkinningRenderNodeArray & renderNodes )
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
					REQUIRE( count1 == count2 );
				}
			} );
	}

	void RenderPass::doUpdate( SubmeshSkinningRenderNodesPtrByPipelineMap & nodes
		, Camera const & camera )const
	{
		doTraverseNodes( *this
			, camera
			, nodes
			, [this, &camera]( RenderPipeline & pipeline
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
					uint32_t count1 = doCopyNodesMatrices( renderNodes, camera, it->second.data );
					uint32_t count2 = doCopyNodesBones( renderNodes, camera, instantiatedBones.getInstancedBonesBuffer() );
					REQUIRE( count1 == count2 );
				}
			} );
	}

	void RenderPass::doUpdate( SubmeshSkinningRenderNodesPtrByPipelineMap & nodes
		, Camera const & camera
		, ShadowMapLightTypeArray & shadowMaps )const
	{
		doTraverseNodes( *this
			, camera
			, nodes
			, *getEngine()->getRenderSystem()->getTopScene()
			, shadowMaps
			, [this, &camera]( RenderPipeline & pipeline
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
					uint32_t count1 = doCopyNodesMatrices( renderNodes, camera, it->second.data );
					uint32_t count2 = doCopyNodesBones( renderNodes, camera, instantiatedBones.getInstancedBonesBuffer() );
					REQUIRE( count1 == count2 );
				}
			} );
	}

	void RenderPass::doUpdate( SubmeshSkinningRenderNodesPtrByPipelineMap & nodes
		, Camera const & camera
		, ShadowMapLightTypeArray & shadowMaps
		, RenderInfo & info )const
	{
		doTraverseNodes( *this
			, camera
			, nodes
			, *getEngine()->getRenderSystem()->getTopScene()
			, shadowMaps
			, [this, &camera, &info]( RenderPipeline & pipeline
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
					uint32_t count1 = doCopyNodesMatrices( renderNodes, camera, it->second.data, info );
					uint32_t count2 = doCopyNodesBones( renderNodes, camera, instantiatedBones.getInstancedBonesBuffer(), info );
					REQUIRE( count1 == count2 );
					info.m_visibleFaceCount += submesh.getFaceCount() * count1;
					info.m_visibleVertexCount += submesh.getPointsCount() * count1;
					++info.m_drawCalls;
				}
			} );
	}

	void RenderPass::doUpdate( MorphingRenderNodesByPipelineMap & nodes )const
	{
	}

	void RenderPass::doUpdate( MorphingRenderNodesByPipelineMap & nodes
		, ShadowMapLightTypeArray & shadowMaps )const
	{
	}

	void RenderPass::doUpdate( MorphingRenderNodesPtrByPipelineMap & nodes
		, Camera const & camera )const
	{
		doRenderNonInstanced( *this
			, camera
			, nodes );
	}

	void RenderPass::doUpdate( MorphingRenderNodesPtrByPipelineMap & nodes
		, Camera const & camera
		, ShadowMapLightTypeArray & shadowMaps )const
	{
		doRenderNonInstanced( *this
			, camera
			, nodes
			, *getEngine()->getRenderSystem()->getTopScene()
			, shadowMaps );
	}

	void RenderPass::doUpdate( MorphingRenderNodesPtrByPipelineMap & nodes
		, Camera const & camera
		, ShadowMapLightTypeArray & shadowMaps
		, RenderInfo & info )const
	{
		doRenderNonInstanced( *this
			, camera
			, nodes
			, *getEngine()->getRenderSystem()->getTopScene()
			, shadowMaps
			, info );
	}

	void RenderPass::doUpdate( BillboardRenderNodesByPipelineMap & nodes )const
	{
	}

	void RenderPass::doUpdate( BillboardRenderNodesByPipelineMap & nodes
		, ShadowMapLightTypeArray & shadowMaps )const
	{
	}

	void RenderPass::doUpdate( BillboardRenderNodesPtrByPipelineMap & nodes
		, Camera const & camera )const
	{
		doRenderNonInstanced( *this
			, camera
			, nodes );
	}

	void RenderPass::doUpdate( BillboardRenderNodesPtrByPipelineMap & nodes
		, Camera const & camera
		, ShadowMapLightTypeArray & shadowMaps )const
	{
		doRenderNonInstanced( *this
			, camera
			, nodes
			, *getEngine()->getRenderSystem()->getTopScene()
			, shadowMaps );
	}

	void RenderPass::doUpdate( BillboardRenderNodesPtrByPipelineMap & nodes
		, Camera const & camera
		, ShadowMapLightTypeArray & shadowMaps
		, RenderInfo & info )const
	{
		doRenderNonInstanced( *this
			, camera
			, nodes
			, *getEngine()->getRenderSystem()->getTopScene()
			, shadowMaps
			, info );
	}

	void RenderPass::doUpdateUbos( Camera const & camera
		, Point2r const & jitter )
	{
		auto jitterProjSpace = jitter * 2.0_r;
		jitterProjSpace[0] /= camera.getWidth();
		jitterProjSpace[1] /= camera.getHeight();
		m_matrixUbo.update( camera.getView()
			, camera.getViewport().getProjection()
			, jitterProjSpace );
	}

	std::map< PipelineFlags, RenderPipelineUPtr > & RenderPass::doGetFrontPipelines()
	{
		return getEngine()->isTopDown()
			? m_frontPipelines
			: m_backPipelines;
	}

	std::map< PipelineFlags, RenderPipelineUPtr > & RenderPass::doGetBackPipelines()
	{
		return getEngine()->isTopDown()
			? m_backPipelines
			: m_frontPipelines;
	}

	std::map< PipelineFlags, RenderPipelineUPtr > const & RenderPass::doGetFrontPipelines()const
	{
		return getEngine()->isTopDown()
			? m_frontPipelines
			: m_backPipelines;
	}

	std::map< PipelineFlags, RenderPipelineUPtr > const & RenderPass::doGetBackPipelines()const
	{
		return getEngine()->isTopDown()
			? m_backPipelines
			: m_frontPipelines;
	}

	renderer::DescriptorSetLayoutBindingArray RenderPass::doCreateUboBindings( PipelineFlags const & flags )const
	{
		renderer::DescriptorSetLayoutBindingArray uboBindings;

		if ( !checkFlag( flags.programFlags, ProgramFlag::eDepthPass )
			&& !checkFlag( flags.programFlags, ProgramFlag::eShadowMapDirectional )
			&& !checkFlag( flags.programFlags, ProgramFlag::eShadowMapPoint )
			&& !checkFlag( flags.programFlags, ProgramFlag::eShadowMapSpot ) )
		{
			uboBindings.emplace_back( getEngine()->getMaterialCache().getPassBuffer().createLayoutBinding() );
		}

		if ( checkFlag( flags.programFlags, ProgramFlag::eLighting ) )
		{
			uboBindings.emplace_back( LightBufferIndex, renderer::DescriptorType::eUniformTexelBuffer, renderer::ShaderStageFlag::eFragment );
		}

		uboBindings.emplace_back( MatrixUbo::BindingPoint, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eVertex );
		uboBindings.emplace_back( SceneUbo::BindingPoint, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eVertex | renderer::ShaderStageFlag::eFragment );
		uboBindings.emplace_back( ModelMatrixUbo::BindingPoint, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eVertex );
		uboBindings.emplace_back( ModelUbo::BindingPoint, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eVertex | renderer::ShaderStageFlag::eFragment );

		if ( checkFlag( flags.programFlags, ProgramFlag::eSkinning ) )
		{
			uboBindings.push_back( SkinningUbo::createLayoutBinding( SkinningUbo::BindingPoint, flags.programFlags ) );
		}

		if ( checkFlag( flags.programFlags, ProgramFlag::eMorphing ) )
		{
			uboBindings.emplace_back( MorphingUbo::BindingPoint, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eVertex );
		}

		if ( checkFlag( flags.programFlags, ProgramFlag::ePicking ) )
		{
			uboBindings.emplace_back( PickingUbo::BindingPoint, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eFragment );
		}

		if ( checkFlag( flags.programFlags, ProgramFlag::eBillboards ) )
		{
			uboBindings.emplace_back( BillboardUbo::BindingPoint, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eVertex );
		}

		return uboBindings;
	}

	glsl::Shader RenderPass::doGetVertexShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, bool invertNormals )const
	{
		using namespace glsl;
		auto writer = getEngine()->getRenderSystem()->createGlslWriter();
		// Vertex inputs
		auto position = writer.declAttribute< Vec4 >( cuT( "position" )
			, RenderPass::VertexInputs::PositionLocation );
		auto normal = writer.declAttribute< Vec3 >( cuT( "normal" )
			, RenderPass::VertexInputs::NormalLocation );
		auto tangent = writer.declAttribute< Vec3 >( cuT( "tangent" )
			, RenderPass::VertexInputs::TangentLocation );
		auto bitangent = writer.declAttribute< Vec3 >( cuT( "bitangent" )
			, RenderPass::VertexInputs::BitangentLocation );
		auto texture = writer.declAttribute< Vec3 >( cuT( "texcoord" )
			, RenderPass::VertexInputs::TextureLocation );
		auto bone_ids0 = writer.declAttribute< IVec4 >( cuT( "bone_ids0" )
			, RenderPass::VertexInputs::BoneIds0Location
			, checkFlag( programFlags, ProgramFlag::eSkinning ) );
		auto bone_ids1 = writer.declAttribute< IVec4 >( cuT( "bone_ids1" )
			, RenderPass::VertexInputs::BoneIds1Location
			, checkFlag( programFlags, ProgramFlag::eSkinning ) );
		auto weights0 = writer.declAttribute< Vec4 >( cuT( "weights0" )
			, RenderPass::VertexInputs::Weights0Location
			, checkFlag( programFlags, ProgramFlag::eSkinning ) );
		auto weights1 = writer.declAttribute< Vec4 >( cuT( "weights1" )
			, RenderPass::VertexInputs::Weights1Location
			, checkFlag( programFlags, ProgramFlag::eSkinning ) );
		auto transform = writer.declAttribute< Mat4 >( cuT( "transform" )
			, RenderPass::VertexInputs::TransformLocation
			, checkFlag( programFlags, ProgramFlag::eInstantiation ) );
		auto material = writer.declAttribute< Int >( cuT( "material" )
			, RenderPass::VertexInputs::MaterialLocation
			, checkFlag( programFlags, ProgramFlag::eInstantiation ) );
		auto position2 = writer.declAttribute< Vec4 >( cuT( "position2" )
			, RenderPass::VertexInputs::Position2Location
			, checkFlag( programFlags, ProgramFlag::eMorphing ) );
		auto normal2 = writer.declAttribute< Vec3 >( cuT( "normal2" )
			, RenderPass::VertexInputs::Normal2Location
			, checkFlag( programFlags, ProgramFlag::eMorphing ) );
		auto tangent2 = writer.declAttribute< Vec3 >( cuT( "tangent2" )
			, RenderPass::VertexInputs::Tangent2Location
			, checkFlag( programFlags, ProgramFlag::eMorphing ) );
		auto bitangent2 = writer.declAttribute< Vec3 >( cuT( "bitangent2" )
			, RenderPass::VertexInputs::Bitangent2Location
			, checkFlag( programFlags, ProgramFlag::eMorphing ) );
		auto texture2 = writer.declAttribute< Vec3 >( cuT( "texture2" )
			, RenderPass::VertexInputs::Texture2Location
			, checkFlag( programFlags, ProgramFlag::eMorphing ) );
		auto gl_InstanceID( writer.declBuiltin< Int >( writer.getInstanceID() ) );

		UBO_MATRIX( writer, MatrixUbo::BindingPoint, 0 );
		UBO_SCENE( writer, SceneUbo::BindingPoint, 0 );
		UBO_MODEL_MATRIX( writer, ModelMatrixUbo::BindingPoint, 0 );
		UBO_MODEL( writer, ModelUbo::BindingPoint, 0 );
		SkinningUbo::declare( writer, SkinningUbo::BindingPoint, 0, programFlags );
		UBO_MORPHING( writer, MorphingUbo::BindingPoint, 0, programFlags );

		// Outputs
		auto vtx_worldPosition = writer.declOutput< Vec3 >( cuT( "vtx_worldPosition" )
			, RenderPass::VertexOutputs::WorldPositionLocation );
		auto vtx_tangentSpaceFragPosition = writer.declOutput< Vec3 >( cuT( "vtx_tangentSpaceFragPosition" )
			, RenderPass::VertexOutputs::TangentSpaceFragPositionLocation );
		auto vtx_tangentSpaceViewPosition = writer.declOutput< Vec3 >( cuT( "vtx_tangentSpaceViewPosition" )
			, RenderPass::VertexOutputs::TangentSpaceViewPositionLocation );
		auto vtx_normal = writer.declOutput< Vec3 >( cuT( "vtx_normal" )
			, RenderPass::VertexOutputs::NormalLocation );
		auto vtx_tangent = writer.declOutput< Vec3 >( cuT( "vtx_tangent" )
			, RenderPass::VertexOutputs::TangentLocation );
		auto vtx_bitangent = writer.declOutput< Vec3 >( cuT( "vtx_bitangent" )
			, RenderPass::VertexOutputs::BitangentLocation );
		auto vtx_texture = writer.declOutput< Vec3 >( cuT( "vtx_texture" )
			, RenderPass::VertexOutputs::TextureLocation );
		auto vtx_instance = writer.declOutput< Int >( cuT( "vtx_instance" )
			, RenderPass::VertexOutputs::InstanceLocation );
		auto vtx_material = writer.declOutput< Int >( cuT( "vtx_material" )
			, RenderPass::VertexOutputs::MaterialLocation );
		auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

		std::function< void() > main = [&]()
		{
			auto v4Vertex = writer.declLocale( cuT( "v4Vertex" )
				, vec4( position.xyz(), 1.0 ) );
			auto v4Normal = writer.declLocale( cuT( "v4Normal" )
				, vec4( normal, 0.0 ) );
			auto v4Tangent = writer.declLocale( cuT( "v4Tangent" )
				, vec4( tangent, 0.0 ) );
			auto v3Texture = writer.declLocale( cuT( "v3Texture" )
				, texture );
			auto mtxModel = writer.declLocale< Mat4 >( cuT( "mtxModel" ) );

			if ( checkFlag( programFlags, ProgramFlag::eSkinning ) )
			{
				mtxModel = SkinningUbo::computeTransform( writer, programFlags );
				auto mtxNormal = writer.declLocale( cuT( "mtxNormal" )
					, transpose( inverse( mat3( mtxModel ) ) ) );
			}
			else if ( checkFlag( programFlags, ProgramFlag::eInstantiation ) )
			{
				mtxModel = transform;
				auto mtxNormal = writer.declLocale( cuT( "mtxNormal" )
					, transpose( inverse( mat3( mtxModel ) ) ) );
			}
			else
			{
				mtxModel = c3d_mtxModel;
				auto mtxNormal = writer.declLocale( cuT( "mtxNormal" )
					, mat3( c3d_mtxNormal ) );
			}

			if ( checkFlag( programFlags, ProgramFlag::eInstantiation ) )
			{
				vtx_material = material;
			}
			else
			{
				vtx_material = c3d_materialIndex;
			}

			if ( checkFlag( programFlags, ProgramFlag::eMorphing ) )
			{
				auto time = writer.declLocale( cuT( "time" )
					, vec3( 1.0_f - c3d_time ) );
				v4Vertex = vec4( glsl::fma( v4Vertex.xyz(), time, position2.xyz() * c3d_time ), 1.0 );
				v4Normal = vec4( glsl::fma( v4Normal.xyz(), time, normal2.xyz() * c3d_time ), 1.0 );
				v4Tangent = vec4( glsl::fma( v4Tangent.xyz(), time, tangent2.xyz() * c3d_time ), 1.0 );
				v3Texture = glsl::fma( v3Texture, time, texture2 * c3d_time );
			}

			vtx_texture = v3Texture;
			v4Vertex = mtxModel * v4Vertex;
			vtx_worldPosition = v4Vertex.xyz();
			v4Vertex = c3d_curView * v4Vertex;
			auto mtxNormal = writer.getBuiltin< Mat3 >( cuT( "mtxNormal" ) );

			if ( invertNormals )
			{
				vtx_normal = normalize( writer.paren( mtxNormal * -v4Normal.xyz() ) );
			}
			else
			{
				vtx_normal = normalize( writer.paren( mtxNormal * v4Normal.xyz() ) );
			}

			vtx_tangent = normalize( writer.paren( mtxNormal * v4Tangent.xyz() ) );
			vtx_tangent = normalize( glsl::fma( -vtx_normal, vec3( dot( vtx_tangent, vtx_normal ) ), vtx_tangent ) );
			vtx_bitangent = cross( vtx_normal, vtx_tangent );
			vtx_instance = gl_InstanceID;
			gl_Position = c3d_projection * v4Vertex;

			auto tbn = writer.declLocale( cuT( "tbn" )
				, transpose( mat3( vtx_tangent, vtx_bitangent, vtx_normal ) ) );
			vtx_tangentSpaceFragPosition = tbn * vtx_worldPosition;
			vtx_tangentSpaceViewPosition = tbn * c3d_cameraPosition.xyz();
		};

		writer.implementFunction< void >( cuT( "main" ), main );
		return writer.finalise();
	}
}
