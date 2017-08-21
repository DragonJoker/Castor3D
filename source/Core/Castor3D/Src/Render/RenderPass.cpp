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
							, itSubmeshes.second );

						doUnbindPass( details::getParentNode( itSubmeshes.second[0].m_instance )
							, itSubmeshes.second[0].m_passNode
							, scene
							, *itPipelines.first
							, shadowMaps
							, envMap );
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
							, itSubmeshes.second );

						doUnbindPass( details::getParentNode( itSubmeshes.second[0].m_instance )
							, itSubmeshes.second[0].m_passNode
							, scene
							, *itPipelines.first
							, shadowMaps
							, envMap );
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

					doUnbindPass( details::getParentNode( renderNode.m_instance )
						, renderNode.m_passNode
						, scene
						, *itPipelines.first
						, shadowMaps
						, envMap );
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

					doUnbindPass( details::getParentNode( renderNode.m_instance )
						, renderNode.m_passNode
						, scene
						, *itPipelines.first
						, shadowMaps
						, envMap );
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
					++info.m_drawCalls;

					doUnbindPass( details::getParentNode( renderNode.m_instance )
						, renderNode.m_passNode
						, scene
						, *itPipelines.first
						, shadowMaps
						, envMap );

					++info.m_visibleObjectsCount;
				}
			}
		}

		template< typename ArrayT >
		uint32_t CopyNodesMatrices( ArrayT const & renderNodes
			, VertexBuffer & matrixBuffer )
		{
			auto const mtxSize = sizeof( float ) * 16;
			auto const stride = matrixBuffer.getDeclaration().stride();
			auto const count = std::min( matrixBuffer.getSize() / stride, uint32_t( renderNodes.size() ) );
			REQUIRE( count <= renderNodes.size() );
			auto buffer = matrixBuffer.getData();
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
		m_timer = std::make_shared< RenderPassTimer >( *getEngine(), getName() );
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

	glsl::Shader RenderPass::getVertexShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, bool invertNormals )const
	{
		return doGetVertexShaderSource( textureFlags
			, programFlags
			, sceneFlags
			, invertNormals );
	}

	glsl::Shader RenderPass::getGeometryShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags )const
	{
		return doGetGeometryShaderSource( textureFlags
			, programFlags
			, sceneFlags );
	}

	glsl::Shader RenderPass::getPixelShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ComparisonFunc alphaFunc )const
	{
		glsl::Shader result;

		if ( checkFlag( programFlags, ProgramFlag::ePbrMetallicRoughness ) )
		{
			result = doGetPbrMRPixelShaderSource( textureFlags
				, programFlags
				, sceneFlags
				, alphaFunc );
		}
		else if ( checkFlag( programFlags, ProgramFlag::ePbrSpecularGlossiness ) )
		{
			result = doGetPbrSGPixelShaderSource( textureFlags
				, programFlags
				, sceneFlags
				, alphaFunc );
		}
		else
		{
			result = doGetLegacyPixelShaderSource( textureFlags
				, programFlags
				, sceneFlags
				, alphaFunc );
		}

		return result;
	}

	void RenderPass::preparePipeline( BlendMode colourBlendMode
		, BlendMode alphaBlendMode
		, ComparisonFunc alphaFunc
		, TextureChannels & textureFlags
		, ProgramFlags & programFlags
		, SceneFlags & sceneFlags
		, bool twoSided )
	{
		doUpdateFlags( textureFlags
			, programFlags
			, sceneFlags );

		if ( checkFlag( programFlags, ProgramFlag::eAlphaBlending ) != m_opaque
			&& ( !checkFlag( programFlags, ProgramFlag::eBillboards )
				|| !isShadowMapProgram( programFlags ) ) )
		{
			if ( m_opaque )
			{
				alphaBlendMode = BlendMode::eNoBlend;
			}

			auto backProgram = doGetProgram( textureFlags
				, programFlags
				, sceneFlags
				, alphaFunc
				, false );

			if ( !m_opaque )
			{
				auto frontProgram = doGetProgram( textureFlags
					, programFlags
					, sceneFlags
					, alphaFunc
					, true );
				auto flags = PipelineFlags{ colourBlendMode, alphaBlendMode, textureFlags, programFlags, sceneFlags };
				doPrepareFrontPipeline( *frontProgram, flags );
				doPrepareBackPipeline( *backProgram, flags );
			}
			else
			{
				auto flags = PipelineFlags{ colourBlendMode, alphaBlendMode, textureFlags, programFlags, sceneFlags };

				if ( twoSided || checkFlag( textureFlags, TextureChannel::eOpacity ) )
				{
					auto frontProgram = doGetProgram( textureFlags
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
		, ComparisonFunc alphaFunc
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags )const
	{
		if ( m_opaque )
		{
			alphaBlendMode = BlendMode::eNoBlend;
		}

		auto it = m_frontPipelines.find( { colourBlendMode, alphaBlendMode, textureFlags, programFlags, sceneFlags } );
		RenderPipeline * result{ nullptr };

		if ( it != m_frontPipelines.end() )
		{
			result = it->second.get();
		}

		return result;
	}

	RenderPipeline * RenderPass::getPipelineBack( BlendMode colourBlendMode
		, BlendMode alphaBlendMode
		, ComparisonFunc alphaFunc
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags )const
	{
		if ( m_opaque )
		{
			alphaBlendMode = BlendMode::eNoBlend;
		}

		auto it = m_backPipelines.find( { colourBlendMode, alphaBlendMode, textureFlags, programFlags, sceneFlags } );
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
		auto buffers = submesh.getGeometryBuffers( pipeline.getProgram() );
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
		auto buffers = submesh.getGeometryBuffers( pipeline.getProgram() );
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
		auto buffers = submesh.getGeometryBuffers( pipeline.getProgram() );
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
		auto buffers = billboard.getGeometryBuffers( pipeline.getProgram() );
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

	void RenderPass::updateFlags( TextureChannels & textureFlags
		, ProgramFlags & programFlags
		, SceneFlags & sceneFlags )const
	{
		doUpdateFlags( textureFlags, programFlags, sceneFlags );
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

	ShaderProgramSPtr RenderPass::doGetProgram( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ComparisonFunc alphaFunc
		, bool invertNormals )const
	{
		return getEngine()->getShaderProgramCache().getAutomaticProgram( *this
			, textureFlags
			, programFlags
			, sceneFlags
			, alphaFunc
			, invertNormals );
	}

	uint32_t RenderPass::doCopyNodesMatrices( StaticRenderNodeArray const & renderNodes
		, VertexBuffer & matrixBuffer )const
	{
		return CopyNodesMatrices( renderNodes, matrixBuffer );
	}

	uint32_t RenderPass::doCopyNodesMatrices( StaticRenderNodeArray const & renderNodes
		, VertexBuffer & matrixBuffer
		, RenderInfo & info )const
	{
		auto count = CopyNodesMatrices( renderNodes, matrixBuffer );
		info.m_visibleObjectsCount += count;
		return count;
	}

	uint32_t RenderPass::doCopyNodesMatrices( SkinningRenderNodeArray const & renderNodes
		, VertexBuffer & matrixBuffer )const
	{
		return CopyNodesMatrices( renderNodes, matrixBuffer );
	}

	uint32_t RenderPass::doCopyNodesMatrices( SkinningRenderNodeArray const & renderNodes
		, VertexBuffer & matrixBuffer
		, RenderInfo & info )const
	{
		auto count = CopyNodesMatrices( renderNodes, matrixBuffer );
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
				, StaticRenderNodeArray & renderNodes )
			{
				if ( !renderNodes.empty() && submesh.hasMatrixBuffer() )
				{
					uint32_t count = doCopyNodesMatrices( renderNodes, submesh.getMatrixBuffer() );
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
				, StaticRenderNodeArray & renderNodes )
			{
				if ( !renderNodes.empty() && submesh.hasMatrixBuffer() )
				{
					uint32_t count = doCopyNodesMatrices( renderNodes, submesh.getMatrixBuffer() );
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
			, [this]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, StaticRenderNodeArray & renderNodes )
			{
				if ( !renderNodes.empty() && submesh.hasMatrixBuffer() )
				{
					uint32_t count = doCopyNodesMatrices( renderNodes, submesh.getMatrixBuffer() );
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
			, [this]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, StaticRenderNodeArray & renderNodes )
			{
				if ( !renderNodes.empty() && submesh.hasMatrixBuffer() )
				{
					uint32_t count = doCopyNodesMatrices( renderNodes, submesh.getMatrixBuffer() );
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
				, StaticRenderNodeArray & renderNodes )
			{
				if ( !renderNodes.empty() && submesh.hasMatrixBuffer() )
				{
					uint32_t count = doCopyNodesMatrices( renderNodes, submesh.getMatrixBuffer(), info );
					submesh.drawInstanced( renderNodes[0].m_buffers, count );
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
				, SkinningRenderNodeArray & renderNodes )
			{
				if ( !renderNodes.empty()
					&& submesh.hasInstancedBonesBuffer() 
					&& submesh.hasMatrixBuffer() )
				{
					uint32_t count1 = doCopyNodesMatrices( renderNodes, submesh.getMatrixBuffer() );
					uint32_t count2 = doCopyNodesBones( renderNodes, submesh.getInstancedBonesBuffer() );
					REQUIRE( count1 == count2 );
					submesh.getInstancedBonesBuffer().bindTo( SkinningUbo::BindingPoint );
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
				, SkinningRenderNodeArray & renderNodes )
			{
				if ( !renderNodes.empty()
					&& submesh.hasInstancedBonesBuffer() 
					&& submesh.hasMatrixBuffer() )
				{
					uint32_t count1 = doCopyNodesMatrices( renderNodes, submesh.getMatrixBuffer() );
					uint32_t count2 = doCopyNodesBones( renderNodes, submesh.getInstancedBonesBuffer() );
					REQUIRE( count1 == count2 );
					submesh.getInstancedBonesBuffer().bindTo( SkinningUbo::BindingPoint );
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
				, SkinningRenderNodeArray & renderNodes )
			{
				if ( !renderNodes.empty()
					&& submesh.hasInstancedBonesBuffer() 
					&& submesh.hasMatrixBuffer() )
				{
					uint32_t count1 = doCopyNodesMatrices( renderNodes, submesh.getMatrixBuffer() );
					uint32_t count2 = doCopyNodesBones( renderNodes, submesh.getInstancedBonesBuffer() );
					REQUIRE( count1 == count2 );
					submesh.getInstancedBonesBuffer().bindTo( SkinningUbo::BindingPoint );
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
				, SkinningRenderNodeArray & renderNodes )
			{
				if ( !renderNodes.empty()
					&& submesh.hasInstancedBonesBuffer() 
					&& submesh.hasMatrixBuffer() )
				{
					uint32_t count1 = doCopyNodesMatrices( renderNodes, submesh.getMatrixBuffer() );
					uint32_t count2 = doCopyNodesBones( renderNodes, submesh.getInstancedBonesBuffer() );
					REQUIRE( count1 == count2 );
					submesh.getInstancedBonesBuffer().bindTo( SkinningUbo::BindingPoint );
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
				, SkinningRenderNodeArray & renderNodes )
			{
				if ( !renderNodes.empty()
					&& submesh.hasInstancedBonesBuffer() 
					&& submesh.hasMatrixBuffer() )
				{
					uint32_t count1 = doCopyNodesMatrices( renderNodes, submesh.getMatrixBuffer(), info );
					uint32_t count2 = doCopyNodesBones( renderNodes, submesh.getInstancedBonesBuffer(), info );
					REQUIRE( count1 == count2 );
					submesh.getInstancedBonesBuffer().bindTo( SkinningUbo::BindingPoint );
					submesh.drawInstanced( renderNodes[0].m_buffers, count1 );
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

	glsl::Shader RenderPass::doGetVertexShaderSource( TextureChannels const & textureFlags
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
		auto bone_ids0 = writer.declAttribute< IVec4 >( ShaderProgram::BoneIds0, checkFlag( programFlags, ProgramFlag::eSkinning ) );
		auto bone_ids1 = writer.declAttribute< IVec4 >( ShaderProgram::BoneIds1, checkFlag( programFlags, ProgramFlag::eSkinning ) );
		auto weights0 = writer.declAttribute< Vec4 >( ShaderProgram::Weights0, checkFlag( programFlags, ProgramFlag::eSkinning ) );
		auto weights1 = writer.declAttribute< Vec4 >( ShaderProgram::Weights1, checkFlag( programFlags, ProgramFlag::eSkinning ) );
		auto transform = writer.declAttribute< Mat4 >( ShaderProgram::Transform, checkFlag( programFlags, ProgramFlag::eInstantiation ) );
		auto position2 = writer.declAttribute< Vec4 >( ShaderProgram::Position2, checkFlag( programFlags, ProgramFlag::eMorphing ) );
		auto normal2 = writer.declAttribute< Vec3 >( ShaderProgram::Normal2, checkFlag( programFlags, ProgramFlag::eMorphing ) );
		auto tangent2 = writer.declAttribute< Vec3 >( ShaderProgram::Tangent2, checkFlag( programFlags, ProgramFlag::eMorphing ) );
		auto bitangent2 = writer.declAttribute< Vec3 >( ShaderProgram::Bitangent2, checkFlag( programFlags, ProgramFlag::eMorphing ) );
		auto texture2 = writer.declAttribute< Vec3 >( ShaderProgram::Texture2, checkFlag( programFlags, ProgramFlag::eMorphing ) );
		auto gl_InstanceID( writer.declBuiltin< Int >( cuT( "gl_InstanceID" ) ) );

		UBO_MATRIX( writer );
		UBO_MODEL_MATRIX( writer );
		UBO_SCENE( writer );
		SkinningUbo::declare( writer, programFlags );
		UBO_MORPHING( writer, programFlags );

		// Outputs
		auto vtx_position = writer.declOutput< Vec3 >( cuT( "vtx_position" ) );
		auto vtx_tangentSpaceFragPosition = writer.declOutput< Vec3 >( cuT( "vtx_tangentSpaceFragPosition" ) );
		auto vtx_tangentSpaceViewPosition = writer.declOutput< Vec3 >( cuT( "vtx_tangentSpaceViewPosition" ) );
		auto vtx_normal = writer.declOutput< Vec3 >( cuT( "vtx_normal" ) );
		auto vtx_tangent = writer.declOutput< Vec3 >( cuT( "vtx_tangent" ) );
		auto vtx_bitangent = writer.declOutput< Vec3 >( cuT( "vtx_bitangent" ) );
		auto vtx_texture = writer.declOutput< Vec3 >( cuT( "vtx_texture" ) );
		auto vtx_instance = writer.declOutput< Int >( cuT( "vtx_instance" ) );
		auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

		std::function< void() > main = [&]()
		{
			auto v4Vertex = writer.declLocale( cuT( "v4Vertex" ), vec4( position.xyz(), 1.0 ) );
			auto v4Normal = writer.declLocale( cuT( "v4Normal" ), vec4( normal, 0.0 ) );
			auto v4Tangent = writer.declLocale( cuT( "v4Tangent" ), vec4( tangent, 0.0 ) );
			auto v3Texture = writer.declLocale( cuT( "v3Texture" ), texture );
			auto mtxModel = writer.declLocale< Mat4 >( cuT( "mtxModel" ) );

			if ( checkFlag( programFlags, ProgramFlag::eSkinning ) )
			{
				mtxModel = SkinningUbo::computeTransform( writer, programFlags );
			}
			else if ( checkFlag( programFlags, ProgramFlag::eInstantiation ) )
			{
				mtxModel = transform;
			}
			else
			{
				mtxModel = c3d_mtxModel;
			}

			if ( checkFlag( programFlags, ProgramFlag::eMorphing ) )
			{
				auto time = writer.declLocale( cuT( "time" ), 1.0_f - c3d_fTime );
				v4Vertex = vec4( v4Vertex.xyz() * time + position2.xyz() * c3d_fTime, 1.0 );
				v4Normal = vec4( v4Normal.xyz() * time + normal2.xyz() * c3d_fTime, 1.0 );
				v4Tangent = vec4( v4Tangent.xyz() * time + tangent2.xyz() * c3d_fTime, 1.0 );
				v3Texture = v3Texture * writer.paren( 1.0_f - c3d_fTime ) + texture2 * c3d_fTime;
			}

			vtx_texture = v3Texture;
			v4Vertex = mtxModel * v4Vertex;
			vtx_position = v4Vertex.xyz();
			v4Vertex = c3d_mtxView * v4Vertex;
			mtxModel = transpose( inverse( mtxModel ) );

			if ( invertNormals )
			{
				vtx_normal = normalize( writer.paren( mtxModel * -v4Normal ).xyz() );
			}
			else
			{
				vtx_normal = normalize( writer.paren( mtxModel * v4Normal ).xyz() );
			}

			vtx_tangent = normalize( writer.paren( mtxModel * v4Tangent ).xyz() );
			vtx_tangent = normalize( vtx_tangent - vtx_normal * dot( vtx_tangent, vtx_normal ) );
			vtx_bitangent = cross( vtx_normal, vtx_tangent );
			vtx_instance = gl_InstanceID;
			gl_Position = c3d_mtxProjection * v4Vertex;

			auto tbn = writer.declLocale( cuT( "tbn" ), transpose( mat3( vtx_tangent, vtx_bitangent, vtx_normal ) ) );
			vtx_tangentSpaceFragPosition = tbn * vtx_position;
			vtx_tangentSpaceViewPosition = tbn * c3d_cameraPosition;
		};

		writer.implementFunction< void >( cuT( "main" ), main );
		return writer.finalise();
	}
}
