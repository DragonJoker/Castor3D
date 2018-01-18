#include "RenderPass.hpp"

#include "Engine.hpp"

#include "Material/Pass.hpp"
#include "Mesh/Submesh.hpp"
#include "Mesh/Buffer/GeometryBuffers.hpp"
#include "Mesh/Buffer/VertexBuffer.hpp"
#include "Render/RenderPassTimer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderNode/RenderNode_Render.hpp"
#include "Scene/BillboardList.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Geometry.hpp"
#include "Scene/Scene.hpp"
#include "Scene/SceneNode.hpp"
#include "Shader/ShaderStorageBuffer.hpp"
#include "Shader/UniformBuffer.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Shader/Shaders/GlslMaterial.hpp"

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
			for ( auto itPipelines : nodes )
			{
				itPipelines.first->apply();

				for ( auto itPass : itPipelines.second )
				{
					for ( auto itSubmeshes : itPass.second )
					{
						doBindPassOpacityMap( itSubmeshes.second[0].m_passNode
							, itSubmeshes.second[0].m_passNode.m_pass );

						function( *itPipelines.first
							, *itPass.first
							, *itSubmeshes.first
							, itSubmeshes.first->getInstantiation()
							, itSubmeshes.second );

						doUnbindPassOpacityMap( itSubmeshes.second[0].m_passNode
							, itSubmeshes.second[0].m_passNode.m_pass );
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
			for ( auto itPipelines : nodes )
			{
				itPipelines.first->apply();

				for ( auto itPass : itPipelines.second )
				{
					for ( auto itSubmeshes : itPass.second )
					{
						EnvironmentMap * envMap = nullptr;
						doBindPass( details::getParentNode( itSubmeshes.second[0].m_instance )
							, itSubmeshes.second[0].m_passNode
							, scene
							, *itPipelines.first
							, shadowMaps
							, itSubmeshes.second[0].m_modelUbo
							, envMap );

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
			for ( auto itPipelines : nodes )
			{
				pass.updatePipeline( *itPipelines.first );
				itPipelines.first->apply();

				for ( auto itPass : itPipelines.second )
				{
					for ( auto itSubmeshes : itPass.second )
					{
						doBindPassOpacityMap( itSubmeshes.second[0].m_passNode
							, itSubmeshes.second[0].m_passNode.m_pass );

						function( *itPipelines.first
							, *itPass.first
							, *itSubmeshes.first
							, itSubmeshes.first->getInstantiation()
							, itSubmeshes.second );

						doUnbindPassOpacityMap( itSubmeshes.second[0].m_passNode
							, itSubmeshes.second[0].m_passNode.m_pass );
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
			for ( auto itPipelines : nodes )
			{
				pass.updatePipeline( *itPipelines.first );
				itPipelines.first->apply();

				for ( auto itPass : itPipelines.second )
				{
					for ( auto itSubmeshes : itPass.second )
					{
						EnvironmentMap * envMap = nullptr;
						doBindPass( details::getParentNode( itSubmeshes.second[0].m_instance )
							, itSubmeshes.second[0].m_passNode
							, scene
							, *itPipelines.first
							, shadowMaps
							, itSubmeshes.second[0].m_modelUbo
							, envMap );

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
			, MapType & nodes )
		{
			for ( auto itPipelines : nodes )
			{
				itPipelines.first->apply();

				for ( auto & renderNode : itPipelines.second )
				{
					doBindPassOpacityMap( renderNode.m_passNode
						, renderNode.m_passNode.m_pass );

					doRenderNodeNoPass( renderNode );

					doUnbindPassOpacityMap( renderNode.m_passNode
						, renderNode.m_passNode.m_pass );
				}
			}
		}

		template< typename MapType >
		inline void doRenderNonInstanced( RenderPass const & pass
			, MapType & nodes
			, Scene & scene
			, ShadowMapLightTypeArray & shadowMaps )
		{
			for ( auto itPipelines : nodes )
			{
				itPipelines.first->apply();

				for ( auto & renderNode : itPipelines.second )
				{
					EnvironmentMap * envMap = nullptr;
					doBindPass( details::getParentNode( renderNode.m_instance )
						, renderNode.m_passNode
						, scene
						, *itPipelines.first
						, shadowMaps
						, renderNode.m_modelUbo
						, envMap );
					doRenderNode( renderNode );
				}
			}
		}

		template< typename MapType >
		inline void doRenderNonInstanced( RenderPass const & pass
			, Camera const & camera
			, MapType & nodes )
		{
			for ( auto itPipelines : nodes )
			{
				pass.updatePipeline( *itPipelines.first );
				itPipelines.first->apply();

				for ( auto & renderNode : itPipelines.second )
				{
					doBindPassOpacityMap( renderNode.m_passNode
						, renderNode.m_passNode.m_pass );

					doRenderNodeNoPass( renderNode );

					doUnbindPassOpacityMap( renderNode.m_passNode
						, renderNode.m_passNode.m_pass );
				}
			}
		}

		template< typename MapType >
		inline void doRenderNonInstanced( RenderPass const & pass
			, Camera const & camera
			, MapType & nodes
			, Scene & scene
			, ShadowMapLightTypeArray & shadowMaps )
		{
			for ( auto itPipelines : nodes )
			{
				pass.updatePipeline( *itPipelines.first );
				itPipelines.first->apply();

				for ( auto & renderNode : itPipelines.second )
				{
					EnvironmentMap * envMap = nullptr;
					doBindPass( details::getParentNode( renderNode.m_instance )
						, renderNode.m_passNode
						, scene
						, *itPipelines.first
						, shadowMaps
						, renderNode.m_modelUbo
						, envMap );
					doRenderNode( renderNode );
				}
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
			for ( auto itPipelines : nodes )
			{
				pass.updatePipeline( *itPipelines.first );
				itPipelines.first->apply();

				for ( auto & renderNode : itPipelines.second )
				{
					EnvironmentMap * envMap = nullptr;
					doBindPass( details::getParentNode( renderNode.m_instance )
						, renderNode.m_passNode
						, scene
						, *itPipelines.first
						, shadowMaps
						, renderNode.m_modelUbo
						, envMap );

					doRenderNode( renderNode );
					info.m_visibleFaceCount += details::getPrimitiveCount( renderNode.m_data );
					info.m_visibleVertexCount += details::getVertexCount( renderNode.m_data );
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
			auto const count = std::min( matrixBuffer.size(), uint32_t( renderNodes.size() ) );
			auto buffer = matrixBuffer.data();
			auto it = renderNodes.begin();
			auto i = 0u;

			while ( i < count )
			{
				std::memcpy( buffer, it->m_sceneNode.getDerivedTransformationMatrix().constPtr(), mtxSize );
				auto id = it->m_passNode.m_pass.getId() - 1;
				std::memcpy( buffer + mtxSize, &id, sizeof( int ) );
				buffer += stride;
				++i;
				++it;
			}

			matrixBuffer.upload( 0u, stride * count, matrixBuffer.getData() );
			return count;
		}

		template< typename ArrayT >
		uint32_t copyNodesMatrices( ArrayT const & renderNodes
			, Camera const & camera
			, std::vector< InstantiationData > & matrixBuffer )
		{
			auto const mtxSize = sizeof( float ) * 16;
			auto const count = std::min( matrixBuffer.size(), uint32_t( renderNodes.size() ) );
			auto buffer = matrixBuffer.data();
			auto it = renderNodes.begin();
			auto i = 0u;

			while ( i < count )
			{
				if ( it->m_sceneNode.isDisplayable()
					&& it->m_sceneNode.isVisible()
					&& camera.isVisible( it->m_instance, it->m_data ) )
				{
					std::memcpy( buffer, it->m_sceneNode.getDerivedTransformationMatrix().constPtr(), mtxSize );
					auto id = it->m_passNode.m_pass.getId() - 1;
					std::memcpy( buffer + mtxSize, &id, sizeof( int ) );
					buffer += stride;
				}
				++i;
				++it;
			}

			matrixBuffer.upload( 0u, stride * count, matrixBuffer.getData() );
			return count;
		}
	}

	//*********************************************************************************************

	RenderPass::RenderPass( String const & name
		, Engine & engine
		, SceneNode const * ignored )
		: OwnedBy< Engine >{ engine }
		, Named{ name }
		, m_renderSystem{ *engine.getRenderSystem() }
		, m_oit{ true }
		, m_renderQueue{ *this, true, ignored }
		, m_opaque{ true }
		, m_matrixUbo{ engine }
		, m_modelMatrixUbo{ engine }
		, m_sceneUbo{ engine }
		, m_modelUbo{ engine }
		, m_billboardUbo{ engine }
		, m_skinningUbo{ engine }
		, m_morphingUbo{ engine }
	{
	}

	RenderPass::RenderPass( String const & name
		, Engine & engine
		, bool oit
		, SceneNode const * ignored )
		: OwnedBy< Engine >{ engine }
		, Named{ name }
		, m_renderSystem{ *engine.getRenderSystem() }
		, m_oit{ oit }
		, m_renderQueue{ *this, false, ignored }
		, m_opaque{ false }
		, m_matrixUbo{ engine }
		, m_modelMatrixUbo{ engine }
		, m_sceneUbo{ engine }
		, m_modelUbo{ engine }
		, m_billboardUbo{ engine }
		, m_skinningUbo{ engine }
		, m_morphingUbo{ engine }
	{
	}

	RenderPass::~RenderPass()
	{
	}

	bool RenderPass::initialise( Size const & size )
	{
		m_timer = std::make_shared< RenderPassTimer >( *getEngine(), getName(), getName() );
		return doInitialise( size );
	}

	void RenderPass::cleanup()
	{
		m_skinningUbo.getUbo().cleanup();
		m_morphingUbo.getUbo().cleanup();
		m_billboardUbo.getUbo().cleanup();
		m_modelUbo.getUbo().cleanup();
		m_modelMatrixUbo.getUbo().cleanup();
		m_matrixUbo.getUbo().cleanup();
		m_sceneUbo.getUbo().cleanup();
		doCleanup();

		for ( auto & buffers : m_geometryBuffers )
		{
			buffers->cleanup();
		}

		m_geometryBuffers.clear();
		m_timer.reset();
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
		, bool twoSided )
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

			auto backProgram = doGetProgram( passFlags
				, textureFlags
				, programFlags
				, sceneFlags
				, alphaFunc
				, false );

			if ( !m_opaque )
			{
				auto frontProgram = doGetProgram( passFlags
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
					, sceneFlags };
				doPrepareFrontPipeline( *frontProgram, flags );
				doPrepareBackPipeline( *backProgram, flags );
			}
			else
			{
				auto flags = PipelineFlags{ colourBlendMode
					, alphaBlendMode
					, passFlags
					, textureFlags
					, programFlags
					, sceneFlags };

				if ( twoSided || checkFlag( textureFlags, TextureChannel::eOpacity ) )
				{
					auto frontProgram = doGetProgram( passFlags
						, textureFlags
						, programFlags
						, sceneFlags
						, alphaFunc
						, true );
					doPrepareFrontPipeline( *frontProgram, flags );
				}

				doPrepareBackPipeline( *backProgram, flags );
			}
		}
	}

	RenderPipeline * RenderPass::getPipelineFront( BlendMode colourBlendMode
		, BlendMode alphaBlendMode
		, renderer::CompareOp alphaFunc
		, PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags )const
	{
		if ( m_opaque )
		{
			alphaBlendMode = BlendMode::eNoBlend;
		}

		auto it = m_frontPipelines.find( { colourBlendMode, alphaBlendMode, passFlags, textureFlags, programFlags, sceneFlags } );
		RenderPipeline * result{ nullptr };

		if ( it != m_frontPipelines.end() )
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
		, SceneFlags const & sceneFlags )const
	{
		if ( m_opaque )
		{
			alphaBlendMode = BlendMode::eNoBlend;
		}

		auto it = m_backPipelines.find( { colourBlendMode, alphaBlendMode, passFlags, textureFlags, programFlags, sceneFlags } );
		RenderPipeline * result{ nullptr };

		if ( it != m_backPipelines.end() )
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
		auto buffers = pipeline.getGeometryBuffers( submesh );
		m_geometryBuffers.insert( buffers );

		return SkinningRenderNode
		{
			pipeline,
			doCreatePassRenderNode( pass, pipeline ),
			m_modelMatrixUbo,
			m_modelUbo,
			*buffers,
			*primitive.getParent(),
			submesh,
			primitive,
			skeleton,
			m_skinningUbo
		};
	}

	MorphingRenderNode RenderPass::createMorphingNode( Pass & pass
		, RenderPipeline & pipeline
		, Submesh & submesh
		, Geometry & primitive
		, AnimatedMesh & mesh )
	{
		auto buffers = pipeline.getGeometryBuffers( submesh );
		m_geometryBuffers.insert( buffers );

		return MorphingRenderNode
		{
			pipeline,
			doCreatePassRenderNode( pass, pipeline ),
			m_modelMatrixUbo,
			m_modelUbo,
			*buffers,
			*primitive.getParent(),
			submesh,
			primitive,
			mesh,
			m_morphingUbo
		};
	}

	StaticRenderNode RenderPass::createStaticNode( Pass & pass
		, RenderPipeline & pipeline
		, Submesh & submesh
		, Geometry & primitive )
	{
		auto buffers = pipeline.getGeometryBuffers( submesh );
		m_geometryBuffers.insert( buffers );

		return StaticRenderNode
		{
			pipeline,
			doCreatePassRenderNode( pass, pipeline ),
			m_modelMatrixUbo,
			m_modelUbo,
			*buffers,
			*primitive.getParent(),
			submesh,
			primitive,
		};
	}

	BillboardRenderNode RenderPass::createBillboardNode( Pass & pass
		, RenderPipeline & pipeline
		, BillboardBase & billboard )
	{
		auto buffers = pipeline.getGeometryBuffers( billboard );
		m_geometryBuffers.insert( buffers );

		return BillboardRenderNode
		{
			pipeline,
			doCreatePassRenderNode( pass, pipeline ),
			m_modelMatrixUbo,
			m_modelUbo,
			*buffers,
			*billboard.getNode(),
			billboard,
			m_billboardUbo,
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

	PassRenderNode RenderPass::doCreatePassRenderNode( Pass & pass
		, RenderPipeline & pipeline )
	{
		return PassRenderNode
		{
			pass,
			pipeline.getProgram(),
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

	uint32_t RenderPass::doCopyNodesMatrices( StaticRenderNodeArray const & renderNodes
		, Camera const & camera
		, std::vector< InstantiationData > & matrixBuffer )const
	{
		return copyNodesMatrices( renderNodes
			, camera
			, matrixBuffer );
	}

	uint32_t RenderPass::doCopyNodesMatrices( StaticRenderNodeArray const & renderNodes
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

	uint32_t RenderPass::doCopyNodesMatrices( SkinningRenderNodeArray const & renderNodes
		, Camera const & camera
		, std::vector< InstantiationData > & matrixBuffer )const
	{
		return copyNodesMatrices( renderNodes
			, camera
			, matrixBuffer );
	}

	uint32_t RenderPass::doCopyNodesMatrices( SkinningRenderNodeArray const & renderNodes
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
		, ShaderStorageBuffer & bonesBuffer )const
	{
		uint32_t const mtxSize = sizeof( float ) * 16;
		uint32_t const stride = mtxSize * 400u;
		auto const count = std::min( bonesBuffer.getSize() / stride, uint32_t( renderNodes.size() ) );
		REQUIRE( count <= renderNodes.size() );
		auto buffer = bonesBuffer.getData();
		auto it = renderNodes.begin();
		auto i = 0u;

		while ( i < count )
		{
			auto & node = *it;
			node.m_skeleton.fillBuffer( buffer );
			buffer += stride;
			++i;
			++it;
		}

		bonesBuffer.upload( 0u, stride * count, bonesBuffer.getData() );
		return count;
	}

	uint32_t RenderPass::doCopyNodesBones( SkinningRenderNodeArray const & renderNodes
		, ShaderStorageBuffer & bonesBuffer
		, RenderInfo & info )const
	{
		auto count = doCopyNodesBones( renderNodes, bonesBuffer );
		info.m_visibleObjectsCount += count;
		return count;
	}

	void RenderPass::doRender( SubmeshStaticRenderNodesByPipelineMap & nodes )const
	{
		doTraverseNodes( *this
			, nodes
			, [this]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, InstantiationComponent & instantiation
				, StaticRenderNodeArray & renderNodes )
			{
				if ( !renderNodes.empty() && instantiation.hasMatrixBuffer() )
				{
					uint32_t count = doCopyNodesMatrices( renderNodes
						, instantiation.getData() );
					submesh.drawInstanced( renderNodes[0].m_buffers, count );
				}
			} );
	}

	void RenderPass::doRender( SubmeshStaticRenderNodesByPipelineMap & nodes
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
				if ( !renderNodes.empty() && instantiation.hasMatrixBuffer() )
				{
					uint32_t count = doCopyNodesMatrices( renderNodes
						, instantiation.getData() );
					submesh.drawInstanced( renderNodes[0].m_buffers, count );
				}
			} );
	}

	void RenderPass::doRender( SubmeshStaticRenderNodesByPipelineMap & nodes
		, Camera const & camera )const
	{
		doTraverseNodes( *this
			, camera
			, nodes
			, [this, &camera]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, InstantiationComponent & instantiation
				, StaticRenderNodeArray & renderNodes )
			{
				if ( !renderNodes.empty() && instantiation.hasMatrixBuffer() )
				{
					uint32_t count = doCopyNodesMatrices( renderNodes
						, camera
						, instantiation.getData() );
					submesh.drawInstanced( renderNodes[0].m_buffers, count );
				}
			} );
	}

	void RenderPass::doRender( SubmeshStaticRenderNodesByPipelineMap & nodes
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
				, StaticRenderNodeArray & renderNodes )
			{
				if ( !renderNodes.empty() && instantiation.hasMatrixBuffer() )
				{
					uint32_t count = doCopyNodesMatrices( renderNodes
						, camera
						, instantiation.getData() );
					submesh.drawInstanced( renderNodes[0].m_buffers, count );
				}
			} );
	}

	void RenderPass::doRender( SubmeshStaticRenderNodesByPipelineMap & nodes
		, Camera const & camera
		, ShadowMapLightTypeArray & shadowMaps
		, RenderInfo & info )const
	{
		doTraverseNodes( *this
			, camera
			, nodes
			, *getEngine()->getRenderSystem()->getTopScene()
			, shadowMaps
			, [this, &info]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, InstantiationComponent & instantiation
				, StaticRenderNodeArray & renderNodes )
			{
				if ( !renderNodes.empty() && instantiation.hasMatrixBuffer() )
				{
					uint32_t count = doCopyNodesMatrices( renderNodes
						, instantiation.getData()
						, info );
					submesh.drawInstanced( renderNodes[0].m_buffers, count );
					info.m_visibleFaceCount += submesh.getFaceCount() * count;
					info.m_visibleVertexCount += submesh.getPointsCount() * count;
					++info.m_drawCalls;
				}
			} );
	}

	void RenderPass::doRender( StaticRenderNodesByPipelineMap & nodes )const
	{
		doRenderNonInstanced( *this
			, nodes );
	}

	void RenderPass::doRender( StaticRenderNodesByPipelineMap & nodes
		, ShadowMapLightTypeArray & shadowMaps )const
	{
		doRenderNonInstanced( *this
			, nodes
			, *getEngine()->getRenderSystem()->getTopScene()
			, shadowMaps );
	}

	void RenderPass::doRender( StaticRenderNodesByPipelineMap & nodes
		, Camera const & camera )const
	{
		doRenderNonInstanced( *this
			, camera
			, nodes );
	}

	void RenderPass::doRender( StaticRenderNodesByPipelineMap & nodes
		, Camera const & camera
		, ShadowMapLightTypeArray & shadowMaps )const
	{
		doRenderNonInstanced( *this
			, camera
			, nodes
			, *getEngine()->getRenderSystem()->getTopScene()
			, shadowMaps );
	}

	void RenderPass::doRender( StaticRenderNodesByPipelineMap & nodes
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

	void RenderPass::doRender( SkinningRenderNodesByPipelineMap & nodes )const
	{
		doRenderNonInstanced( *this
			, nodes );
	}

	void RenderPass::doRender( SkinningRenderNodesByPipelineMap & nodes
		, ShadowMapLightTypeArray & shadowMaps )const
	{
		doRenderNonInstanced( *this
			, nodes
			, *getEngine()->getRenderSystem()->getTopScene()
			, shadowMaps );
	}

	void RenderPass::doRender( SkinningRenderNodesByPipelineMap & nodes
		, Camera const & camera )const
	{
		doRenderNonInstanced( *this
			, camera
			, nodes );
	}

	void RenderPass::doRender( SkinningRenderNodesByPipelineMap & nodes
		, Camera const & camera
		, ShadowMapLightTypeArray & shadowMaps )const
	{
		doRenderNonInstanced( *this
			, camera
			, nodes
			, *getEngine()->getRenderSystem()->getTopScene()
			, shadowMaps );
	}

	void RenderPass::doRender( SkinningRenderNodesByPipelineMap & nodes
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

	void RenderPass::doRender( SubmeshSkinningRenderNodesByPipelineMap & nodes )const
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

				if ( !renderNodes.empty()
					&& instantiatedBones.hasInstancedBonesBuffer()
					&& instantiation.hasMatrixBuffer() )
				{
					uint32_t count1 = doCopyNodesMatrices( renderNodes, instantiation.getData() );
					uint32_t count2 = doCopyNodesBones( renderNodes, instantiatedBones.getInstancedBonesBuffer() );
					REQUIRE( count1 == count2 );
					instantiatedBones.getInstancedBonesBuffer().bindTo( SkinningUbo::BindingPoint );
					submesh.drawInstanced( renderNodes[0].m_buffers, count1 );
				}
			} );
	}

	void RenderPass::doRender( SubmeshSkinningRenderNodesByPipelineMap & nodes
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

				if ( !renderNodes.empty()
					&& instantiatedBones.hasInstancedBonesBuffer()
					&& instantiation.hasMatrixBuffer() )
				{
					uint32_t count1 = doCopyNodesMatrices( renderNodes, instantiation.getData() );
					uint32_t count2 = doCopyNodesBones( renderNodes, instantiatedBones.getInstancedBonesBuffer() );
					REQUIRE( count1 == count2 );
					instantiatedBones.getInstancedBonesBuffer().bindTo( SkinningUbo::BindingPoint );
					submesh.drawInstanced( renderNodes[0].m_buffers, count1 );
				}
			} );
	}

	void RenderPass::doRender( SubmeshSkinningRenderNodesByPipelineMap & nodes
		, Camera const & camera )const
	{
		doTraverseNodes( *this
			, camera
			, nodes
			, [this]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, InstantiationComponent & instantiation
				, SkinningRenderNodeArray & renderNodes )
			{
				auto & instantiatedBones = submesh.getInstantiatedBones();

				if ( !renderNodes.empty()
					&& instantiatedBones.hasInstancedBonesBuffer()
					&& instantiation.hasMatrixBuffer() )
				{
					uint32_t count1 = doCopyNodesMatrices( renderNodes, instantiation.getData() );
					uint32_t count2 = doCopyNodesBones( renderNodes, instantiatedBones.getInstancedBonesBuffer() );
					REQUIRE( count1 == count2 );
					instantiatedBones.getInstancedBonesBuffer().bindTo( SkinningUbo::BindingPoint );
					submesh.drawInstanced( renderNodes[0].m_buffers, count1 );
				}
			} );
	}

	void RenderPass::doRender( SubmeshSkinningRenderNodesByPipelineMap & nodes
		, Camera const & camera
		, ShadowMapLightTypeArray & shadowMaps )const
	{
		doTraverseNodes( *this
			, camera
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

				if ( !renderNodes.empty()
					&& instantiatedBones.hasInstancedBonesBuffer()
					&& instantiation.hasMatrixBuffer() )
				{
					uint32_t count1 = doCopyNodesMatrices( renderNodes, instantiation.getData() );
					uint32_t count2 = doCopyNodesBones( renderNodes, instantiatedBones.getInstancedBonesBuffer() );
					REQUIRE( count1 == count2 );
					instantiatedBones.getInstancedBonesBuffer().bindTo( SkinningUbo::BindingPoint );
					submesh.drawInstanced( renderNodes[0].m_buffers, count1 );
				}
			} );
	}

	void RenderPass::doRender( SubmeshSkinningRenderNodesByPipelineMap & nodes
		, Camera const & camera
		, ShadowMapLightTypeArray & shadowMaps
		, RenderInfo & info )const
	{
		doTraverseNodes( *this
			, camera
			, nodes
			, *getEngine()->getRenderSystem()->getTopScene()
			, shadowMaps
			, [this, &info]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, InstantiationComponent & instantiation
				, SkinningRenderNodeArray & renderNodes )
			{
				auto & instantiatedBones = submesh.getInstantiatedBones();

				if ( !renderNodes.empty()
					&& instantiatedBones.hasInstancedBonesBuffer()
					&& instantiation.hasMatrixBuffer() )
				{
					uint32_t count1 = doCopyNodesMatrices( renderNodes, instantiation.getData(), info );
					uint32_t count2 = doCopyNodesBones( renderNodes, instantiatedBones.getInstancedBonesBuffer(), info );
					REQUIRE( count1 == count2 );
					instantiatedBones.getInstancedBonesBuffer().bindTo( SkinningUbo::BindingPoint );
					submesh.drawInstanced( renderNodes[0].m_buffers, count1 );
					info.m_visibleFaceCount += submesh.getFaceCount() * count1;
					info.m_visibleVertexCount += submesh.getPointsCount() * count1;
					++info.m_drawCalls;
				}
			} );
	}

	void RenderPass::doRender( MorphingRenderNodesByPipelineMap & nodes )const
	{
		doRenderNonInstanced( *this
			, nodes );
	}

	void RenderPass::doRender( MorphingRenderNodesByPipelineMap & nodes
		, ShadowMapLightTypeArray & shadowMaps )const
	{
		doRenderNonInstanced( *this
			, nodes
			, *getEngine()->getRenderSystem()->getTopScene()
			, shadowMaps );
	}

	void RenderPass::doRender( MorphingRenderNodesByPipelineMap & nodes
		, Camera const & camera )const
	{
		doRenderNonInstanced( *this
			, camera
			, nodes );
	}

	void RenderPass::doRender( MorphingRenderNodesByPipelineMap & nodes
		, Camera const & camera
		, ShadowMapLightTypeArray & shadowMaps )const
	{
		doRenderNonInstanced( *this
			, camera
			, nodes
			, *getEngine()->getRenderSystem()->getTopScene()
			, shadowMaps );
	}

	void RenderPass::doRender( MorphingRenderNodesByPipelineMap & nodes
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

	void RenderPass::doRender( BillboardRenderNodesByPipelineMap & nodes )const
	{
		doRenderNonInstanced( *this
			, nodes );
	}

	void RenderPass::doRender( BillboardRenderNodesByPipelineMap & nodes
		, ShadowMapLightTypeArray & shadowMaps )const
	{
		doRenderNonInstanced( *this
			, nodes
			, *getEngine()->getRenderSystem()->getTopScene()
			, shadowMaps );
	}

	void RenderPass::doRender( BillboardRenderNodesByPipelineMap & nodes
		, Camera const & camera )const
	{
		doRenderNonInstanced( *this
			, camera
			, nodes );
	}

	void RenderPass::doRender( BillboardRenderNodesByPipelineMap & nodes
		, Camera const & camera
		, ShadowMapLightTypeArray & shadowMaps )const
	{
		doRenderNonInstanced( *this
			, camera
			, nodes
			, *getEngine()->getRenderSystem()->getTopScene()
			, shadowMaps );
	}

	void RenderPass::doRender( BillboardRenderNodesByPipelineMap & nodes
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

	glsl::Shader RenderPass::doGetVertexShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, bool invertNormals )const
	{
		using namespace glsl;
		auto writer = getEngine()->getRenderSystem()->createGlslWriter();
		// Vertex inputs
		auto position = writer.declAttribute< Vec4 >( ShaderProgram::Position );
		auto normal = writer.declAttribute< Vec3 >( ShaderProgram::Normal );
		auto tangent = writer.declAttribute< Vec3 >( ShaderProgram::Tangent );
		auto bitangent = writer.declAttribute< Vec3 >( ShaderProgram::Bitangent );
		auto texture = writer.declAttribute< Vec3 >( ShaderProgram::Texture );
		auto bone_ids0 = writer.declAttribute< IVec4 >( ShaderProgram::BoneIds0
			, checkFlag( programFlags, ProgramFlag::eSkinning ) );
		auto bone_ids1 = writer.declAttribute< IVec4 >( ShaderProgram::BoneIds1
			, checkFlag( programFlags, ProgramFlag::eSkinning ) );
		auto weights0 = writer.declAttribute< Vec4 >( ShaderProgram::Weights0
			, checkFlag( programFlags, ProgramFlag::eSkinning ) );
		auto weights1 = writer.declAttribute< Vec4 >( ShaderProgram::Weights1
			, checkFlag( programFlags, ProgramFlag::eSkinning ) );
		auto transform = writer.declAttribute< Mat4 >( ShaderProgram::Transform
			, checkFlag( programFlags, ProgramFlag::eInstantiation ) );
		auto material = writer.declAttribute< Int >( ShaderProgram::Material
			, checkFlag( programFlags, ProgramFlag::eInstantiation ) );
		auto position2 = writer.declAttribute< Vec4 >( ShaderProgram::Position2
			, checkFlag( programFlags, ProgramFlag::eMorphing ) );
		auto normal2 = writer.declAttribute< Vec3 >( ShaderProgram::Normal2
			, checkFlag( programFlags, ProgramFlag::eMorphing ) );
		auto tangent2 = writer.declAttribute< Vec3 >( ShaderProgram::Tangent2
			, checkFlag( programFlags, ProgramFlag::eMorphing ) );
		auto bitangent2 = writer.declAttribute< Vec3 >( ShaderProgram::Bitangent2
			, checkFlag( programFlags, ProgramFlag::eMorphing ) );
		auto texture2 = writer.declAttribute< Vec3 >( ShaderProgram::Texture2
			, checkFlag( programFlags, ProgramFlag::eMorphing ) );
		auto gl_InstanceID( writer.declBuiltin< Int >( cuT( "gl_InstanceID" ) ) );

		UBO_MATRIX( writer, 0 );
		UBO_MODEL_MATRIX( writer, 0 );
		UBO_SCENE( writer, 0 );
		UBO_MODEL( writer, 0 );
		SkinningUbo::declare( writer, 0, programFlags );
		UBO_MORPHING( writer, 0, programFlags );

		// Outputs
		auto vtx_worldPosition = writer.declOutput< Vec3 >( cuT( "vtx_worldPosition" ) );
		auto vtx_tangentSpaceFragPosition = writer.declOutput< Vec3 >( cuT( "vtx_tangentSpaceFragPosition" ) );
		auto vtx_tangentSpaceViewPosition = writer.declOutput< Vec3 >( cuT( "vtx_tangentSpaceViewPosition" ) );
		auto vtx_normal = writer.declOutput< Vec3 >( cuT( "vtx_normal" ) );
		auto vtx_tangent = writer.declOutput< Vec3 >( cuT( "vtx_tangent" ) );
		auto vtx_bitangent = writer.declOutput< Vec3 >( cuT( "vtx_bitangent" ) );
		auto vtx_texture = writer.declOutput< Vec3 >( cuT( "vtx_texture" ) );
		auto vtx_instance = writer.declOutput< Int >( cuT( "vtx_instance" ) );
		auto vtx_material = writer.declOutput< Int >( cuT( "vtx_material" ) );
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
			vtx_tangentSpaceViewPosition = tbn * c3d_cameraPosition;
		};

		writer.implementFunction< void >( cuT( "main" ), main );
		return writer.finalise();
	}
}
