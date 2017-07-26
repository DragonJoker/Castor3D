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

using namespace Castor;

namespace Castor3D
{
	//*********************************************************************************************

	namespace
	{
		template< typename MapType, typename FuncType >
		inline void DoTraverseNodes( RenderPass const & pass
			, MapType & nodes
			, FuncType function )
		{
			for ( auto itPipelines : nodes )
			{
				itPipelines.first->Apply();

				for ( auto itPass : itPipelines.second )
				{
					for ( auto itSubmeshes : itPass.second )
					{
						DoBindPassOpacityMap( itSubmeshes.second[0].m_passNode
							, itSubmeshes.second[0].m_passNode.m_pass );

						function( *itPipelines.first
							, *itPass.first
							, *itSubmeshes.first
							, itSubmeshes.second );

						DoUnbindPassOpacityMap( itSubmeshes.second[0].m_passNode
							, itSubmeshes.second[0].m_passNode.m_pass );
					}
				}
			}
		}

		template< typename MapType, typename FuncType >
		inline void DoTraverseNodes( RenderPass const & pass
			, MapType & nodes
			, Scene & scene
			, DepthMapArray & depthMaps
			, FuncType function )
		{
			for ( auto itPipelines : nodes )
			{
				itPipelines.first->Apply();

				for ( auto itPass : itPipelines.second )
				{
					for ( auto itSubmeshes : itPass.second )
					{
						EnvironmentMap * envMap = nullptr;
						DoBindPass( details::GetParentNode( itSubmeshes.second[0].m_instance )
							, itSubmeshes.second[0].m_passNode
							, scene
							, *itPipelines.first
							, depthMaps
							, itSubmeshes.second[0].m_modelUbo
							, envMap );

						function( *itPipelines.first
							, *itPass.first
							, *itSubmeshes.first
							, itSubmeshes.second );

						DoUnbindPass( details::GetParentNode( itSubmeshes.second[0].m_instance )
							, itSubmeshes.second[0].m_passNode
							, scene
							, *itPipelines.first
							, depthMaps
							, envMap );
					}
				}
			}
		}

		template< typename MapType, typename FuncType >
		inline void DoTraverseNodes( RenderPass const & pass
			, Camera const & camera
			, MapType & nodes
			, FuncType function )
		{
			for ( auto itPipelines : nodes )
			{
				pass.UpdatePipeline( *itPipelines.first );
				itPipelines.first->Apply();

				for ( auto itPass : itPipelines.second )
				{
					for ( auto itSubmeshes : itPass.second )
					{
						DoBindPassOpacityMap( itSubmeshes.second[0].m_passNode
							, itSubmeshes.second[0].m_passNode.m_pass );

						function( *itPipelines.first
							, *itPass.first
							, *itSubmeshes.first
							, itSubmeshes.second );

						DoUnbindPassOpacityMap( itSubmeshes.second[0].m_passNode
							, itSubmeshes.second[0].m_passNode.m_pass );
					}
				}
			}
		}

		template< typename MapType, typename FuncType >
		inline void DoTraverseNodes( RenderPass const & pass
			, Camera const & camera
			, MapType & nodes
			, Scene & scene
			, DepthMapArray & depthMaps
			, FuncType function )
		{
			for ( auto itPipelines : nodes )
			{
				pass.UpdatePipeline( *itPipelines.first );
				itPipelines.first->Apply();

				for ( auto itPass : itPipelines.second )
				{
					for ( auto itSubmeshes : itPass.second )
					{
						EnvironmentMap * envMap = nullptr;
						DoBindPass( details::GetParentNode( itSubmeshes.second[0].m_instance )
							, itSubmeshes.second[0].m_passNode
							, scene
							, *itPipelines.first
							, depthMaps
							, itSubmeshes.second[0].m_modelUbo
							, envMap );

						function( *itPipelines.first
							, *itPass.first
							, *itSubmeshes.first
							, itSubmeshes.second );

						DoUnbindPass( details::GetParentNode( itSubmeshes.second[0].m_instance )
							, itSubmeshes.second[0].m_passNode
							, scene
							, *itPipelines.first
							, depthMaps
							, envMap );
					}
				}
			}
		}

		template< typename MapType >
		inline void DoRenderNonInstanced( RenderPass const & pass
			, MapType & nodes )
		{
			for ( auto itPipelines : nodes )
			{
				itPipelines.first->Apply();

				for ( auto & renderNode : itPipelines.second )
				{
					DoBindPassOpacityMap( renderNode.m_passNode
						, renderNode.m_passNode.m_pass );

					DoRenderNodeNoPass( renderNode );

					DoUnbindPassOpacityMap( renderNode.m_passNode
						, renderNode.m_passNode.m_pass );
				}
			}
		}

		template< typename MapType >
		inline void DoRenderNonInstanced( RenderPass const & pass
			, MapType & nodes
			, Scene & scene
			, DepthMapArray & depthMaps )
		{
			for ( auto itPipelines : nodes )
			{
				itPipelines.first->Apply();

				for ( auto & renderNode : itPipelines.second )
				{
					EnvironmentMap * envMap = nullptr;
					DoBindPass( details::GetParentNode( renderNode.m_instance )
						, renderNode.m_passNode
						, scene
						, *itPipelines.first
						, depthMaps
						, renderNode.m_modelUbo
						, envMap );

					DoRenderNode( renderNode );

					DoUnbindPass( details::GetParentNode( renderNode.m_instance )
						, renderNode.m_passNode
						, scene
						, *itPipelines.first
						, depthMaps
						, envMap );
				}
			}
		}

		template< typename MapType >
		inline void DoRenderNonInstanced( RenderPass const & pass
			, Camera const & camera
			, MapType & nodes )
		{
			for ( auto itPipelines : nodes )
			{
				pass.UpdatePipeline( *itPipelines.first );
				itPipelines.first->Apply();

				for ( auto & renderNode : itPipelines.second )
				{
					DoBindPassOpacityMap( renderNode.m_passNode
						, renderNode.m_passNode.m_pass );

					DoRenderNodeNoPass( renderNode );

					DoUnbindPassOpacityMap( renderNode.m_passNode
						, renderNode.m_passNode.m_pass );
				}
			}
		}

		template< typename MapType >
		inline void DoRenderNonInstanced( RenderPass const & pass
			, Camera const & camera
			, MapType & nodes
			, Scene & scene
			, DepthMapArray & depthMaps )
		{
			for ( auto itPipelines : nodes )
			{
				pass.UpdatePipeline( *itPipelines.first );
				itPipelines.first->Apply();

				for ( auto & renderNode : itPipelines.second )
				{
					EnvironmentMap * envMap = nullptr;
					DoBindPass( details::GetParentNode( renderNode.m_instance )
						, renderNode.m_passNode
						, scene
						, *itPipelines.first
						, depthMaps
						, renderNode.m_modelUbo
						, envMap );

					DoRenderNode( renderNode );

					DoUnbindPass( details::GetParentNode( renderNode.m_instance )
						, renderNode.m_passNode
						, scene
						, *itPipelines.first
						, depthMaps
						, envMap );
				}
			}
		}

		template< typename MapType >
		inline void DoRenderNonInstanced( RenderPass const & pass
			, Camera const & camera
			, MapType & nodes
			, Scene & scene
			, DepthMapArray & depthMaps
			, RenderInfo & info )
		{
			for ( auto itPipelines : nodes )
			{
				pass.UpdatePipeline( *itPipelines.first );
				itPipelines.first->Apply();

				for ( auto & renderNode : itPipelines.second )
				{
					EnvironmentMap * envMap = nullptr;
					DoBindPass( details::GetParentNode( renderNode.m_instance )
						, renderNode.m_passNode
						, scene
						, *itPipelines.first
						, depthMaps
						, renderNode.m_modelUbo
						, envMap );

					DoRenderNode( renderNode );
					++info.m_drawCalls;

					DoUnbindPass( details::GetParentNode( renderNode.m_instance )
						, renderNode.m_passNode
						, scene
						, *itPipelines.first
						, depthMaps
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
			auto const stride = matrixBuffer.GetDeclaration().stride();
			auto const count = std::min( matrixBuffer.GetSize() / stride, uint32_t( renderNodes.size() ) );
			REQUIRE( count <= renderNodes.size() );
			auto buffer = matrixBuffer.GetData();
			auto it = renderNodes.begin();
			auto i = 0u;

			while ( i < count )
			{
				std::memcpy( buffer, it->m_sceneNode.GetDerivedTransformationMatrix().const_ptr(), mtxSize );
				auto id = it->m_passNode.m_pass.GetId() - 1;
				std::memcpy( buffer + mtxSize, &id, sizeof( int ) );
				buffer += stride;
				++i;
				++it;
			}

			matrixBuffer.Upload( 0u, stride * count, matrixBuffer.GetData() );
			return count;
		}
	}

	//*********************************************************************************************

	RenderPass::RenderPass( String const & name
		, Engine & engine
		, SceneNode const * ignored )
		: OwnedBy< Engine >{ engine }
		, Named{ name }
		, m_renderSystem{ *engine.GetRenderSystem() }
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
		, m_renderSystem{ *engine.GetRenderSystem() }
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

	bool RenderPass::Initialise( Size const & size )
	{
		m_timer = std::make_shared< RenderPassTimer >( *GetEngine(), GetName() );
		return DoInitialise( size );
	}

	void RenderPass::Cleanup()
	{
		m_skinningUbo.GetUbo().Cleanup();
		m_morphingUbo.GetUbo().Cleanup();
		m_billboardUbo.GetUbo().Cleanup();
		m_modelUbo.GetUbo().Cleanup();
		m_modelMatrixUbo.GetUbo().Cleanup();
		m_matrixUbo.GetUbo().Cleanup();
		m_sceneUbo.GetUbo().Cleanup();
		DoCleanup();

		for ( auto & buffers : m_geometryBuffers )
		{
			buffers->Cleanup();
		}

		m_geometryBuffers.clear();
		m_timer.reset();
	}

	void RenderPass::Update( RenderQueueArray & queues )
	{
		DoUpdate( queues );
	}

	GLSL::Shader RenderPass::GetVertexShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, bool invertNormals )const
	{
		return DoGetVertexShaderSource( textureFlags
			, programFlags
			, sceneFlags
			, invertNormals );
	}

	GLSL::Shader RenderPass::GetGeometryShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags )const
	{
		return DoGetGeometryShaderSource( textureFlags
			, programFlags
			, sceneFlags );
	}

	GLSL::Shader RenderPass::GetPixelShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ComparisonFunc alphaFunc )const
	{
		GLSL::Shader result;

		if ( CheckFlag( programFlags, ProgramFlag::ePbrMetallicRoughness ) )
		{
			result = DoGetPbrMRPixelShaderSource( textureFlags
				, programFlags
				, sceneFlags
				, alphaFunc );
		}
		else if ( CheckFlag( programFlags, ProgramFlag::ePbrSpecularGlossiness ) )
		{
			result = DoGetPbrSGPixelShaderSource( textureFlags
				, programFlags
				, sceneFlags
				, alphaFunc );
		}
		else
		{
			result = DoGetLegacyPixelShaderSource( textureFlags
				, programFlags
				, sceneFlags
				, alphaFunc );
		}

		return result;
	}

	void RenderPass::PreparePipeline( BlendMode colourBlendMode
		, BlendMode alphaBlendMode
		, ComparisonFunc alphaFunc
		, TextureChannels & textureFlags
		, ProgramFlags & programFlags
		, SceneFlags & sceneFlags
		, bool twoSided )
	{
		DoUpdateFlags( textureFlags
			, programFlags
			, sceneFlags );

		if ( CheckFlag( programFlags, ProgramFlag::eAlphaBlending ) != m_opaque
			&& ( !CheckFlag( programFlags, ProgramFlag::eBillboards )
				|| !IsShadowMapProgram( programFlags ) ) )
		{
			if ( m_opaque )
			{
				alphaBlendMode = BlendMode::eNoBlend;
			}

			auto backProgram = DoGetProgram( textureFlags
				, programFlags
				, sceneFlags
				, alphaFunc
				, false );

			if ( !m_opaque )
			{
				auto frontProgram = DoGetProgram( textureFlags
					, programFlags
					, sceneFlags
					, alphaFunc
					, true );
				auto flags = PipelineFlags{ colourBlendMode, alphaBlendMode, textureFlags, programFlags, sceneFlags };
				DoPrepareFrontPipeline( *frontProgram, flags );
				DoPrepareBackPipeline( *backProgram, flags );
			}
			else
			{
				auto flags = PipelineFlags{ colourBlendMode, alphaBlendMode, textureFlags, programFlags, sceneFlags };

				if ( twoSided || CheckFlag( textureFlags, TextureChannel::eOpacity ) )
				{
					auto frontProgram = DoGetProgram( textureFlags
						, programFlags
						, sceneFlags
						, alphaFunc
						, true );
					DoPrepareFrontPipeline( *frontProgram, flags );
				}

				DoPrepareBackPipeline( *backProgram, flags );
			}
		}
	}

	RenderPipeline * RenderPass::GetPipelineFront( BlendMode colourBlendMode
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

	RenderPipeline * RenderPass::GetPipelineBack( BlendMode colourBlendMode
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

	SkinningRenderNode RenderPass::CreateSkinningNode( Pass & pass
		, RenderPipeline & pipeline
		, Submesh & submesh
		, Geometry & primitive
		, AnimatedSkeleton & skeleton )
	{
		auto buffers = submesh.GetGeometryBuffers( pipeline.GetProgram() );
		m_geometryBuffers.insert( buffers );

		return SkinningRenderNode
		{
			pipeline,
			DoCreatePassRenderNode( pass, pipeline ),
			m_modelMatrixUbo,
			m_modelUbo,
			*buffers,
			*primitive.GetParent(),
			submesh,
			primitive,
			skeleton,
			m_skinningUbo
		};
	}

	MorphingRenderNode RenderPass::CreateMorphingNode( Pass & pass
		, RenderPipeline & pipeline
		, Submesh & submesh
		, Geometry & primitive
		, AnimatedMesh & mesh )
	{
		auto buffers = submesh.GetGeometryBuffers( pipeline.GetProgram() );
		m_geometryBuffers.insert( buffers );

		return MorphingRenderNode
		{
			pipeline,
			DoCreatePassRenderNode( pass, pipeline ),
			m_modelMatrixUbo,
			m_modelUbo,
			*buffers,
			*primitive.GetParent(),
			submesh,
			primitive,
			mesh,
			m_morphingUbo
		};
	}

	StaticRenderNode RenderPass::CreateStaticNode( Pass & pass
		, RenderPipeline & pipeline
		, Submesh & submesh
		, Geometry & primitive )
	{
		auto buffers = submesh.GetGeometryBuffers( pipeline.GetProgram() );
		m_geometryBuffers.insert( buffers );

		return StaticRenderNode
		{
			pipeline,
			DoCreatePassRenderNode( pass, pipeline ),
			m_modelMatrixUbo,
			m_modelUbo,
			*buffers,
			*primitive.GetParent(),
			submesh,
			primitive,
		};
	}

	BillboardRenderNode RenderPass::CreateBillboardNode( Pass & pass
		, RenderPipeline & pipeline
		, BillboardBase & billboard )
	{
		auto buffers = billboard.GetGeometryBuffers( pipeline.GetProgram() );
		m_geometryBuffers.insert( buffers );

		return BillboardRenderNode
		{
			pipeline,
			DoCreatePassRenderNode( pass, pipeline ),
			m_modelMatrixUbo,
			m_modelUbo,
			*buffers,
			*billboard.GetNode(),
			billboard,
			m_billboardUbo,
		};
	}

	void RenderPass::UpdatePipeline( RenderPipeline & pipeline )const
	{
		DoUpdatePipeline( pipeline );
	}

	void RenderPass::UpdateFlags( TextureChannels & textureFlags
		, ProgramFlags & programFlags
		, SceneFlags & sceneFlags )const
	{
		DoUpdateFlags( textureFlags, programFlags, sceneFlags );
	}

	PassRenderNode RenderPass::DoCreatePassRenderNode( Pass & pass
		, RenderPipeline & pipeline )
	{
		return PassRenderNode
		{
			pass,
			pipeline.GetProgram(),
		};
	}

	SceneRenderNode RenderPass::DoCreateSceneRenderNode( Scene & scene
		, RenderPipeline & pipeline )
	{
		return SceneRenderNode{};
	}

	ShaderProgramSPtr RenderPass::DoGetProgram( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ComparisonFunc alphaFunc
		, bool invertNormals )const
	{
		return GetEngine()->GetShaderProgramCache().GetAutomaticProgram( *this
			, textureFlags
			, programFlags
			, sceneFlags
			, alphaFunc
			, invertNormals );
	}

	uint32_t RenderPass::DoCopyNodesMatrices( StaticRenderNodeArray const & renderNodes
		, VertexBuffer & matrixBuffer )const
	{
		return CopyNodesMatrices( renderNodes, matrixBuffer );
	}

	uint32_t RenderPass::DoCopyNodesMatrices( StaticRenderNodeArray const & renderNodes
		, VertexBuffer & matrixBuffer
		, RenderInfo & info )const
	{
		auto count = CopyNodesMatrices( renderNodes, matrixBuffer );
		info.m_visibleObjectsCount += count;
		return count;
	}

	uint32_t RenderPass::DoCopyNodesMatrices( SkinningRenderNodeArray const & renderNodes
		, VertexBuffer & matrixBuffer )const
	{
		return CopyNodesMatrices( renderNodes, matrixBuffer );
	}

	uint32_t RenderPass::DoCopyNodesMatrices( SkinningRenderNodeArray const & renderNodes
		, VertexBuffer & matrixBuffer
		, RenderInfo & info )const
	{
		auto count = CopyNodesMatrices( renderNodes, matrixBuffer );
		info.m_visibleObjectsCount += count;
		return count;
	}

	uint32_t RenderPass::DoCopyNodesBones( SkinningRenderNodeArray const & renderNodes
		, ShaderStorageBuffer & bonesBuffer )const
	{
		uint32_t const mtxSize = sizeof( float ) * 16;
		uint32_t const stride = mtxSize * 400u;
		auto const count = std::min( bonesBuffer.GetSize() / stride, uint32_t( renderNodes.size() ) );
		REQUIRE( count <= renderNodes.size() );
		auto buffer = bonesBuffer.GetData();
		auto it = renderNodes.begin();
		auto i = 0u;

		while ( i < count )
		{
			auto & node = *it;
			node.m_skeleton.FillBuffer( buffer );
			buffer += stride;
			++i;
			++it;
		}

		bonesBuffer.Upload( 0u, stride * count, bonesBuffer.GetData() );
		return count;
	}

	uint32_t RenderPass::DoCopyNodesBones( SkinningRenderNodeArray const & renderNodes
		, ShaderStorageBuffer & bonesBuffer
		, RenderInfo & info )const
	{
		auto count = DoCopyNodesBones( renderNodes, bonesBuffer );
		info.m_visibleObjectsCount += count;
		return count;
	}

	void RenderPass::DoRender( SubmeshStaticRenderNodesByPipelineMap & nodes )const
	{
		DoTraverseNodes( *this
			, nodes
			, [this]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, StaticRenderNodeArray & renderNodes )
			{
				if ( !renderNodes.empty() && submesh.HasMatrixBuffer() )
				{
					uint32_t count = DoCopyNodesMatrices( renderNodes, submesh.GetMatrixBuffer() );
					submesh.DrawInstanced( renderNodes[0].m_buffers, count );
				}
			} );
	}

	void RenderPass::DoRender( SubmeshStaticRenderNodesByPipelineMap & nodes
		, DepthMapArray & depthMaps )const
	{
		DoTraverseNodes( *this
			, nodes
			, *GetEngine()->GetRenderSystem()->GetTopScene()
			, depthMaps
			, [this]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, StaticRenderNodeArray & renderNodes )
			{
				if ( !renderNodes.empty() && submesh.HasMatrixBuffer() )
				{
					uint32_t count = DoCopyNodesMatrices( renderNodes, submesh.GetMatrixBuffer() );
					submesh.DrawInstanced( renderNodes[0].m_buffers, count );
				}
			} );
	}

	void RenderPass::DoRender( SubmeshStaticRenderNodesByPipelineMap & nodes
		, Camera const & camera )const
	{
		DoTraverseNodes( *this
			, camera
			, nodes
			, [this]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, StaticRenderNodeArray & renderNodes )
			{
				if ( !renderNodes.empty() && submesh.HasMatrixBuffer() )
				{
					uint32_t count = DoCopyNodesMatrices( renderNodes, submesh.GetMatrixBuffer() );
					submesh.DrawInstanced( renderNodes[0].m_buffers, count );
				}
			} );
	}

	void RenderPass::DoRender( SubmeshStaticRenderNodesByPipelineMap & nodes
		, Camera const & camera
		, DepthMapArray & depthMaps )const
	{
		DoTraverseNodes( *this
			, camera
			, nodes
			, *GetEngine()->GetRenderSystem()->GetTopScene()
			, depthMaps
			, [this]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, StaticRenderNodeArray & renderNodes )
			{
				if ( !renderNodes.empty() && submesh.HasMatrixBuffer() )
				{
					uint32_t count = DoCopyNodesMatrices( renderNodes, submesh.GetMatrixBuffer() );
					submesh.DrawInstanced( renderNodes[0].m_buffers, count );
				}
			} );
	}

	void RenderPass::DoRender( SubmeshStaticRenderNodesByPipelineMap & nodes
		, Camera const & camera
		, DepthMapArray & depthMaps
		, RenderInfo & info )const
	{
		DoTraverseNodes( *this
			, camera
			, nodes
			, *GetEngine()->GetRenderSystem()->GetTopScene()
			, depthMaps
			, [this, &info]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, StaticRenderNodeArray & renderNodes )
			{
				if ( !renderNodes.empty() && submesh.HasMatrixBuffer() )
				{
					uint32_t count = DoCopyNodesMatrices( renderNodes, submesh.GetMatrixBuffer(), info );
					submesh.DrawInstanced( renderNodes[0].m_buffers, count );
					++info.m_drawCalls;
				}
			} );
	}

	void RenderPass::DoRender( StaticRenderNodesByPipelineMap & nodes )const
	{
		DoRenderNonInstanced( *this
			, nodes );
	}

	void RenderPass::DoRender( StaticRenderNodesByPipelineMap & nodes
		, DepthMapArray & depthMaps )const
	{
		DoRenderNonInstanced( *this
			, nodes
			, *GetEngine()->GetRenderSystem()->GetTopScene()
			, depthMaps );
	}

	void RenderPass::DoRender( StaticRenderNodesByPipelineMap & nodes
		, Camera const & camera )const
	{
		DoRenderNonInstanced( *this
			, camera
			, nodes );
	}

	void RenderPass::DoRender( StaticRenderNodesByPipelineMap & nodes
		, Camera const & camera
		, DepthMapArray & depthMaps )const
	{
		DoRenderNonInstanced( *this
			, camera
			, nodes
			, *GetEngine()->GetRenderSystem()->GetTopScene()
			, depthMaps );
	}

	void RenderPass::DoRender( StaticRenderNodesByPipelineMap & nodes
		, Camera const & camera
		, DepthMapArray & depthMaps
		, RenderInfo & info )const
	{
		DoRenderNonInstanced( *this
			, camera
			, nodes
			, *GetEngine()->GetRenderSystem()->GetTopScene()
			, depthMaps
			, info );
	}

	void RenderPass::DoRender( SkinningRenderNodesByPipelineMap & nodes )const
	{
		DoRenderNonInstanced( *this
			, nodes );
	}

	void RenderPass::DoRender( SkinningRenderNodesByPipelineMap & nodes
		, DepthMapArray & depthMaps )const
	{
		DoRenderNonInstanced( *this
			, nodes
			, *GetEngine()->GetRenderSystem()->GetTopScene()
			, depthMaps );
	}

	void RenderPass::DoRender( SkinningRenderNodesByPipelineMap & nodes
		, Camera const & camera )const
	{
		DoRenderNonInstanced( *this
			, camera
			, nodes );
	}

	void RenderPass::DoRender( SkinningRenderNodesByPipelineMap & nodes
		, Camera const & camera
		, DepthMapArray & depthMaps )const
	{
		DoRenderNonInstanced( *this
			, camera
			, nodes
			, *GetEngine()->GetRenderSystem()->GetTopScene()
			, depthMaps );
	}

	void RenderPass::DoRender( SkinningRenderNodesByPipelineMap & nodes
		, Camera const & camera
		, DepthMapArray & depthMaps
		, RenderInfo & info )const
	{
		DoRenderNonInstanced( *this
			, camera
			, nodes
			, *GetEngine()->GetRenderSystem()->GetTopScene()
			, depthMaps
			, info );
	}

	void RenderPass::DoRender( SubmeshSkinningRenderNodesByPipelineMap & nodes )const
	{
		DoTraverseNodes( *this
			, nodes
			, [this]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, SkinningRenderNodeArray & renderNodes )
			{
				if ( !renderNodes.empty()
					&& submesh.HasInstancedBonesBuffer() 
					&& submesh.HasMatrixBuffer() )
				{
					uint32_t count1 = DoCopyNodesMatrices( renderNodes, submesh.GetMatrixBuffer() );
					uint32_t count2 = DoCopyNodesBones( renderNodes, submesh.GetInstancedBonesBuffer() );
					REQUIRE( count1 == count2 );
					submesh.GetInstancedBonesBuffer().BindTo( SkinningUbo::BindingPoint );
					submesh.DrawInstanced( renderNodes[0].m_buffers, count1 );
				}
			} );
	}

	void RenderPass::DoRender( SubmeshSkinningRenderNodesByPipelineMap & nodes
		, DepthMapArray & depthMaps )const
	{
		DoTraverseNodes( *this
			, nodes
			, *GetEngine()->GetRenderSystem()->GetTopScene()
			, depthMaps
			, [this]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, SkinningRenderNodeArray & renderNodes )
			{
				if ( !renderNodes.empty()
					&& submesh.HasInstancedBonesBuffer() 
					&& submesh.HasMatrixBuffer() )
				{
					uint32_t count1 = DoCopyNodesMatrices( renderNodes, submesh.GetMatrixBuffer() );
					uint32_t count2 = DoCopyNodesBones( renderNodes, submesh.GetInstancedBonesBuffer() );
					REQUIRE( count1 == count2 );
					submesh.GetInstancedBonesBuffer().BindTo( SkinningUbo::BindingPoint );
					submesh.DrawInstanced( renderNodes[0].m_buffers, count1 );
				}
			} );
	}

	void RenderPass::DoRender( SubmeshSkinningRenderNodesByPipelineMap & nodes
		, Camera const & camera )const
	{
		DoTraverseNodes( *this
			, camera
			, nodes
			, [this]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, SkinningRenderNodeArray & renderNodes )
			{
				if ( !renderNodes.empty()
					&& submesh.HasInstancedBonesBuffer() 
					&& submesh.HasMatrixBuffer() )
				{
					uint32_t count1 = DoCopyNodesMatrices( renderNodes, submesh.GetMatrixBuffer() );
					uint32_t count2 = DoCopyNodesBones( renderNodes, submesh.GetInstancedBonesBuffer() );
					REQUIRE( count1 == count2 );
					submesh.GetInstancedBonesBuffer().BindTo( SkinningUbo::BindingPoint );
					submesh.DrawInstanced( renderNodes[0].m_buffers, count1 );
				}
			} );
	}

	void RenderPass::DoRender( SubmeshSkinningRenderNodesByPipelineMap & nodes
		, Camera const & camera
		, DepthMapArray & depthMaps )const
	{
		DoTraverseNodes( *this
			, camera
			, nodes
			, *GetEngine()->GetRenderSystem()->GetTopScene()
			, depthMaps
			, [this]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, SkinningRenderNodeArray & renderNodes )
			{
				if ( !renderNodes.empty()
					&& submesh.HasInstancedBonesBuffer() 
					&& submesh.HasMatrixBuffer() )
				{
					uint32_t count1 = DoCopyNodesMatrices( renderNodes, submesh.GetMatrixBuffer() );
					uint32_t count2 = DoCopyNodesBones( renderNodes, submesh.GetInstancedBonesBuffer() );
					REQUIRE( count1 == count2 );
					submesh.GetInstancedBonesBuffer().BindTo( SkinningUbo::BindingPoint );
					submesh.DrawInstanced( renderNodes[0].m_buffers, count1 );
				}
			} );
	}

	void RenderPass::DoRender( SubmeshSkinningRenderNodesByPipelineMap & nodes
		, Camera const & camera
		, DepthMapArray & depthMaps
		, RenderInfo & info )const
	{
		DoTraverseNodes( *this
			, camera
			, nodes
			, *GetEngine()->GetRenderSystem()->GetTopScene()
			, depthMaps
			, [this, &info]( RenderPipeline & pipeline
				, Pass & pass
				, Submesh & submesh
				, SkinningRenderNodeArray & renderNodes )
			{
				if ( !renderNodes.empty()
					&& submesh.HasInstancedBonesBuffer() 
					&& submesh.HasMatrixBuffer() )
				{
					uint32_t count1 = DoCopyNodesMatrices( renderNodes, submesh.GetMatrixBuffer(), info );
					uint32_t count2 = DoCopyNodesBones( renderNodes, submesh.GetInstancedBonesBuffer(), info );
					REQUIRE( count1 == count2 );
					submesh.GetInstancedBonesBuffer().BindTo( SkinningUbo::BindingPoint );
					submesh.DrawInstanced( renderNodes[0].m_buffers, count1 );
					++info.m_drawCalls;
				}
			} );
	}

	void RenderPass::DoRender( MorphingRenderNodesByPipelineMap & nodes )const
	{
		DoRenderNonInstanced( *this
			, nodes );
	}

	void RenderPass::DoRender( MorphingRenderNodesByPipelineMap & nodes
		, DepthMapArray & depthMaps )const
	{
		DoRenderNonInstanced( *this
			, nodes
			, *GetEngine()->GetRenderSystem()->GetTopScene()
			, depthMaps );
	}

	void RenderPass::DoRender( MorphingRenderNodesByPipelineMap & nodes
		, Camera const & camera )const
	{
		DoRenderNonInstanced( *this
			, camera
			, nodes );
	}

	void RenderPass::DoRender( MorphingRenderNodesByPipelineMap & nodes
		, Camera const & camera
		, DepthMapArray & depthMaps )const
	{
		DoRenderNonInstanced( *this
			, camera
			, nodes
			, *GetEngine()->GetRenderSystem()->GetTopScene()
			, depthMaps );
	}

	void RenderPass::DoRender( MorphingRenderNodesByPipelineMap & nodes
		, Camera const & camera
		, DepthMapArray & depthMaps
		, RenderInfo & info )const
	{
		DoRenderNonInstanced( *this
			, camera
			, nodes
			, *GetEngine()->GetRenderSystem()->GetTopScene()
			, depthMaps
			, info );
	}

	void RenderPass::DoRender( BillboardRenderNodesByPipelineMap & nodes )const
	{
		DoRenderNonInstanced( *this
			, nodes );
	}

	void RenderPass::DoRender( BillboardRenderNodesByPipelineMap & nodes
		, DepthMapArray & depthMaps )const
	{
		DoRenderNonInstanced( *this
			, nodes
			, *GetEngine()->GetRenderSystem()->GetTopScene()
			, depthMaps );
	}

	void RenderPass::DoRender( BillboardRenderNodesByPipelineMap & nodes
		, Camera const & camera )const
	{
		DoRenderNonInstanced( *this
			, camera
			, nodes );
	}

	void RenderPass::DoRender( BillboardRenderNodesByPipelineMap & nodes
		, Camera const & camera
		, DepthMapArray & depthMaps )const
	{
		DoRenderNonInstanced( *this
			, camera
			, nodes
			, *GetEngine()->GetRenderSystem()->GetTopScene()
			, depthMaps );
	}

	void RenderPass::DoRender( BillboardRenderNodesByPipelineMap & nodes
		, Camera const & camera
		, DepthMapArray & depthMaps
		, RenderInfo & info )const
	{
		DoRenderNonInstanced( *this
			, camera
			, nodes
			, *GetEngine()->GetRenderSystem()->GetTopScene()
			, depthMaps
			, info );
	}

	GLSL::Shader RenderPass::DoGetVertexShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, bool invertNormals )const
	{
		using namespace GLSL;
		auto writer = GetEngine()->GetRenderSystem()->CreateGlslWriter();
		// Vertex inputs
		auto position = writer.DeclAttribute< Vec4 >( ShaderProgram::Position );
		auto normal = writer.DeclAttribute< Vec3 >( ShaderProgram::Normal );
		auto tangent = writer.DeclAttribute< Vec3 >( ShaderProgram::Tangent );
		auto bitangent = writer.DeclAttribute< Vec3 >( ShaderProgram::Bitangent );
		auto texture = writer.DeclAttribute< Vec3 >( ShaderProgram::Texture );
		auto bone_ids0 = writer.DeclAttribute< IVec4 >( ShaderProgram::BoneIds0, CheckFlag( programFlags, ProgramFlag::eSkinning ) );
		auto bone_ids1 = writer.DeclAttribute< IVec4 >( ShaderProgram::BoneIds1, CheckFlag( programFlags, ProgramFlag::eSkinning ) );
		auto weights0 = writer.DeclAttribute< Vec4 >( ShaderProgram::Weights0, CheckFlag( programFlags, ProgramFlag::eSkinning ) );
		auto weights1 = writer.DeclAttribute< Vec4 >( ShaderProgram::Weights1, CheckFlag( programFlags, ProgramFlag::eSkinning ) );
		auto transform = writer.DeclAttribute< Mat4 >( ShaderProgram::Transform, CheckFlag( programFlags, ProgramFlag::eInstantiation ) );
		auto position2 = writer.DeclAttribute< Vec4 >( ShaderProgram::Position2, CheckFlag( programFlags, ProgramFlag::eMorphing ) );
		auto normal2 = writer.DeclAttribute< Vec3 >( ShaderProgram::Normal2, CheckFlag( programFlags, ProgramFlag::eMorphing ) );
		auto tangent2 = writer.DeclAttribute< Vec3 >( ShaderProgram::Tangent2, CheckFlag( programFlags, ProgramFlag::eMorphing ) );
		auto bitangent2 = writer.DeclAttribute< Vec3 >( ShaderProgram::Bitangent2, CheckFlag( programFlags, ProgramFlag::eMorphing ) );
		auto texture2 = writer.DeclAttribute< Vec3 >( ShaderProgram::Texture2, CheckFlag( programFlags, ProgramFlag::eMorphing ) );
		auto gl_InstanceID( writer.DeclBuiltin< Int >( cuT( "gl_InstanceID" ) ) );

		UBO_MATRIX( writer );
		UBO_MODEL_MATRIX( writer );
		UBO_SCENE( writer );
		SkinningUbo::Declare( writer, programFlags );
		UBO_MORPHING( writer, programFlags );

		// Outputs
		auto vtx_position = writer.DeclOutput< Vec3 >( cuT( "vtx_position" ) );
		auto vtx_tangentSpaceFragPosition = writer.DeclOutput< Vec3 >( cuT( "vtx_tangentSpaceFragPosition" ) );
		auto vtx_tangentSpaceViewPosition = writer.DeclOutput< Vec3 >( cuT( "vtx_tangentSpaceViewPosition" ) );
		auto vtx_normal = writer.DeclOutput< Vec3 >( cuT( "vtx_normal" ) );
		auto vtx_tangent = writer.DeclOutput< Vec3 >( cuT( "vtx_tangent" ) );
		auto vtx_bitangent = writer.DeclOutput< Vec3 >( cuT( "vtx_bitangent" ) );
		auto vtx_texture = writer.DeclOutput< Vec3 >( cuT( "vtx_texture" ) );
		auto vtx_instance = writer.DeclOutput< Int >( cuT( "vtx_instance" ) );
		auto gl_Position = writer.DeclBuiltin< Vec4 >( cuT( "gl_Position" ) );

		std::function< void() > main = [&]()
		{
			auto v4Vertex = writer.DeclLocale( cuT( "v4Vertex" ), vec4( position.xyz(), 1.0 ) );
			auto v4Normal = writer.DeclLocale( cuT( "v4Normal" ), vec4( normal, 0.0 ) );
			auto v4Tangent = writer.DeclLocale( cuT( "v4Tangent" ), vec4( tangent, 0.0 ) );
			auto v3Texture = writer.DeclLocale( cuT( "v3Texture" ), texture );
			auto mtxModel = writer.DeclLocale< Mat4 >( cuT( "mtxModel" ) );

			if ( CheckFlag( programFlags, ProgramFlag::eSkinning ) )
			{
				mtxModel = SkinningUbo::ComputeTransform( writer, programFlags );
			}
			else if ( CheckFlag( programFlags, ProgramFlag::eInstantiation ) )
			{
				mtxModel = transform;
			}
			else
			{
				mtxModel = c3d_mtxModel;
			}

			if ( CheckFlag( programFlags, ProgramFlag::eMorphing ) )
			{
				auto time = writer.DeclLocale( cuT( "time" ), 1.0_f - c3d_fTime );
				v4Vertex = vec4( v4Vertex.xyz() * time + position2.xyz() * c3d_fTime, 1.0 );
				v4Normal = vec4( v4Normal.xyz() * time + normal2.xyz() * c3d_fTime, 1.0 );
				v4Tangent = vec4( v4Tangent.xyz() * time + tangent2.xyz() * c3d_fTime, 1.0 );
				v3Texture = v3Texture * writer.Paren( 1.0_f - c3d_fTime ) + texture2 * c3d_fTime;
			}

			vtx_texture = v3Texture;
			v4Vertex = mtxModel * v4Vertex;
			vtx_position = v4Vertex.xyz();
			v4Vertex = c3d_mtxView * v4Vertex;
			mtxModel = transpose( inverse( mtxModel ) );

			if ( invertNormals )
			{
				vtx_normal = normalize( writer.Paren( mtxModel * -v4Normal ).xyz() );
			}
			else
			{
				vtx_normal = normalize( writer.Paren( mtxModel * v4Normal ).xyz() );
			}

			vtx_tangent = normalize( writer.Paren( mtxModel * v4Tangent ).xyz() );
			vtx_tangent = normalize( vtx_tangent - vtx_normal * dot( vtx_tangent, vtx_normal ) );
			vtx_bitangent = cross( vtx_normal, vtx_tangent );
			vtx_instance = gl_InstanceID;
			gl_Position = c3d_mtxProjection * v4Vertex;

			auto tbn = writer.DeclLocale( cuT( "tbn" ), transpose( mat3( vtx_tangent, vtx_bitangent, vtx_normal ) ) );
			vtx_tangentSpaceFragPosition = tbn * vtx_position;
			vtx_tangentSpaceViewPosition = tbn * c3d_v3CameraPosition;
		};

		writer.ImplementFunction< void >( cuT( "main" ), main );
		return writer.Finalise();
	}
}
