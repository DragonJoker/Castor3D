#include "RenderPass.hpp"

#include "Engine.hpp"

#include "Material/Pass.hpp"
#include "Mesh/Submesh.hpp"
#include "Mesh/Buffer/GeometryBuffers.hpp"
#include "Mesh/Buffer/VertexBuffer.hpp"
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
		inline void DoTraverseNodes( RenderPass const & p_pass
			, MapType & p_nodes
			, FuncType p_function )
		{
			for ( auto itPipelines : p_nodes )
			{
				itPipelines.first->Apply();

				for ( auto itPass : itPipelines.second )
				{
					for ( auto itSubmeshes : itPass.second )
					{
						DoBindPassOpacityMap( itSubmeshes.second[0].m_passNode
							, itSubmeshes.second[0].m_passNode.m_pass );

						p_function( *itPipelines.first
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
		inline void DoTraverseNodes( RenderPass const & p_pass
			, MapType & p_nodes
			, Scene & p_scene
			, DepthMapArray & p_depthMaps
			, FuncType p_function )
		{
			for ( auto itPipelines : p_nodes )
			{
				itPipelines.first->Apply();

				for ( auto itPass : itPipelines.second )
				{
					for ( auto itSubmeshes : itPass.second )
					{
						EnvironmentMap * envMap = nullptr;
						DoBindPass( details::GetParentNode( itSubmeshes.second[0].m_instance )
							, itSubmeshes.second[0].m_passNode
							, p_scene
							, *itPipelines.first
							, p_depthMaps
							, itSubmeshes.second[0].m_modelUbo
							, envMap );

						p_function( *itPipelines.first
							, *itPass.first
							, *itSubmeshes.first
							, itSubmeshes.second );

						DoUnbindPass( details::GetParentNode( itSubmeshes.second[0].m_instance )
							, itSubmeshes.second[0].m_passNode
							, p_scene
							, *itPipelines.first
							, p_depthMaps
							, envMap );
					}
				}
			}
		}

		template< typename MapType, typename FuncType >
		inline void DoTraverseNodes( RenderPass const & p_pass
			, Camera const & p_camera
			, MapType & p_nodes
			, FuncType p_function )
		{
			for ( auto itPipelines : p_nodes )
			{
				p_pass.UpdatePipeline( *itPipelines.first );
				itPipelines.first->Apply();

				for ( auto itPass : itPipelines.second )
				{
					for ( auto itSubmeshes : itPass.second )
					{
						DoBindPassOpacityMap( itSubmeshes.second[0].m_passNode
							, itSubmeshes.second[0].m_passNode.m_pass );

						p_function( *itPipelines.first
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
		inline void DoTraverseNodes( RenderPass const & p_pass
			, Camera const & p_camera
			, MapType & p_nodes
			, Scene & p_scene
			, DepthMapArray & p_depthMaps
			, FuncType p_function )
		{
			for ( auto itPipelines : p_nodes )
			{
				p_pass.UpdatePipeline( *itPipelines.first );
				itPipelines.first->Apply();

				for ( auto itPass : itPipelines.second )
				{
					for ( auto itSubmeshes : itPass.second )
					{
						EnvironmentMap * envMap = nullptr;
						DoBindPass( details::GetParentNode( itSubmeshes.second[0].m_instance )
							, itSubmeshes.second[0].m_passNode
							, p_scene
							, *itPipelines.first
							, p_depthMaps
							, itSubmeshes.second[0].m_modelUbo
							, envMap );

						p_function( *itPipelines.first
							, *itPass.first
							, *itSubmeshes.first
							, itSubmeshes.second );

						DoUnbindPass( details::GetParentNode( itSubmeshes.second[0].m_instance )
							, itSubmeshes.second[0].m_passNode
							, p_scene
							, *itPipelines.first
							, p_depthMaps
							, envMap );
					}
				}
			}
		}

		template< typename MapType >
		inline void DoRenderNonInstanced( RenderPass const & p_pass
			, MapType & p_nodes )
		{
			for ( auto itPipelines : p_nodes )
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
		inline void DoRenderNonInstanced( RenderPass const & p_pass
			, MapType & p_nodes
			, Scene & p_scene
			, DepthMapArray & p_depthMaps )
		{
			for ( auto itPipelines : p_nodes )
			{
				itPipelines.first->Apply();

				for ( auto & renderNode : itPipelines.second )
				{
					EnvironmentMap * envMap = nullptr;
					DoBindPass( details::GetParentNode( renderNode.m_instance )
						, renderNode.m_passNode
						, p_scene
						, *itPipelines.first
						, p_depthMaps
						, renderNode.m_modelUbo
						, envMap );

					DoRenderNode( renderNode );

					DoUnbindPass( details::GetParentNode( renderNode.m_instance )
						, renderNode.m_passNode
						, p_scene
						, *itPipelines.first
						, p_depthMaps
						, envMap );
				}
			}
		}

		template< typename MapType >
		inline void DoRenderNonInstanced( RenderPass const & p_pass
			, Camera const & p_camera
			, MapType & p_nodes )
		{
			for ( auto itPipelines : p_nodes )
			{
				p_pass.UpdatePipeline( *itPipelines.first );
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
		inline void DoRenderNonInstanced( RenderPass const & p_pass
			, Camera const & p_camera
			, MapType & p_nodes
			, Scene & p_scene
			, DepthMapArray & p_depthMaps )
		{
			for ( auto itPipelines : p_nodes )
			{
				p_pass.UpdatePipeline( *itPipelines.first );
				itPipelines.first->Apply();

				for ( auto & renderNode : itPipelines.second )
				{
					EnvironmentMap * envMap = nullptr;
					DoBindPass( details::GetParentNode( renderNode.m_instance )
						, renderNode.m_passNode
						, p_scene
						, *itPipelines.first
						, p_depthMaps
						, renderNode.m_modelUbo
						, envMap );

					DoRenderNode( renderNode );

					DoUnbindPass( details::GetParentNode( renderNode.m_instance )
						, renderNode.m_passNode
						, p_scene
						, *itPipelines.first
						, p_depthMaps
						, envMap );
				}
			}
		}

		template< typename MapType >
		inline void DoRenderNonInstanced( RenderPass const & p_pass
			, Camera const & p_camera
			, MapType & p_nodes
			, Scene & p_scene
			, DepthMapArray & p_depthMaps
			, RenderInfo & p_info )
		{
			for ( auto itPipelines : p_nodes )
			{
				p_pass.UpdatePipeline( *itPipelines.first );
				itPipelines.first->Apply();

				for ( auto & renderNode : itPipelines.second )
				{
					EnvironmentMap * envMap = nullptr;
					DoBindPass( details::GetParentNode( renderNode.m_instance )
						, renderNode.m_passNode
						, p_scene
						, *itPipelines.first
						, p_depthMaps
						, renderNode.m_modelUbo
						, envMap );

					DoRenderNode( renderNode );
					++p_info.m_drawCalls;

					DoUnbindPass( details::GetParentNode( renderNode.m_instance )
						, renderNode.m_passNode
						, p_scene
						, *itPipelines.first
						, p_depthMaps
						, envMap );

					++p_info.m_visibleObjectsCount;
				}
			}
		}

		template< typename ArrayT >
		uint32_t CopyNodesMatrices( ArrayT const & p_renderNodes
			, VertexBuffer & p_matrixBuffer )
		{
			auto const mtxSize = sizeof( float ) * 16;
			auto const stride = p_matrixBuffer.GetDeclaration().stride();
			auto const count = std::min( p_matrixBuffer.GetSize() / stride, uint32_t( p_renderNodes.size() ) );
			REQUIRE( count <= p_renderNodes.size() );
			auto buffer = p_matrixBuffer.GetData();
			auto it = p_renderNodes.begin();
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

			p_matrixBuffer.Upload( 0u, stride * count, p_matrixBuffer.GetData() );
			return count;
		}
	}

	//*********************************************************************************************

	RenderPass::RenderPass( String const & p_name
		, Engine & p_engine
		, SceneNode const * p_ignored )
		: OwnedBy< Engine >{ p_engine }
		, Named{ p_name }
		, m_renderSystem{ *p_engine.GetRenderSystem() }
		, m_oit{ true }
		, m_renderQueue{ *this, true, p_ignored }
		, m_opaque{ true }
		, m_matrixUbo{ p_engine }
		, m_modelMatrixUbo{ p_engine }
		, m_sceneUbo{ p_engine }
		, m_modelUbo{ p_engine }
		, m_billboardUbo{ p_engine }
		, m_skinningUbo{ p_engine }
		, m_morphingUbo{ p_engine }
	{
	}

	RenderPass::RenderPass( String const & p_name
		, Engine & p_engine
		, bool p_oit
		, SceneNode const * p_ignored )
		: OwnedBy< Engine >{ p_engine }
		, Named{ p_name }
		, m_renderSystem{ *p_engine.GetRenderSystem() }
		, m_oit{ p_oit }
		, m_renderQueue{ *this, false, p_ignored }
		, m_opaque{ false }
		, m_matrixUbo{ p_engine }
		, m_modelMatrixUbo{ p_engine }
		, m_sceneUbo{ p_engine }
		, m_modelUbo{ p_engine }
		, m_billboardUbo{ p_engine }
		, m_skinningUbo{ p_engine }
		, m_morphingUbo{ p_engine }
	{
	}

	RenderPass::~RenderPass()
	{
	}

	bool RenderPass::Initialise( Size const & p_size )
	{
		return DoInitialise( p_size );
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
	}

	void RenderPass::Update( RenderQueueArray & p_queues )
	{
		DoUpdate( p_queues );
	}

	GLSL::Shader RenderPass::GetVertexShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, SceneFlags const & p_sceneFlags
		, bool p_invertNormals )const
	{
		return DoGetVertexShaderSource( p_textureFlags
			, p_programFlags
			, p_sceneFlags
			, p_invertNormals );
	}

	GLSL::Shader RenderPass::GetGeometryShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, SceneFlags const & p_sceneFlags )const
	{
		return DoGetGeometryShaderSource( p_textureFlags
			, p_programFlags
			, p_sceneFlags );
	}

	GLSL::Shader RenderPass::GetPixelShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, SceneFlags const & p_sceneFlags
		, ComparisonFunc p_alphaFunc )const
	{
		GLSL::Shader result;

		if ( CheckFlag( p_programFlags, ProgramFlag::ePbrMetallicRoughness ) )
		{
			result = DoGetPbrMRPixelShaderSource( p_textureFlags
				, p_programFlags
				, p_sceneFlags
				, p_alphaFunc );
		}
		else if ( CheckFlag( p_programFlags, ProgramFlag::ePbrSpecularGlossiness ) )
		{
			result = DoGetPbrSGPixelShaderSource( p_textureFlags
				, p_programFlags
				, p_sceneFlags
				, p_alphaFunc );
		}
		else
		{
			result = DoGetLegacyPixelShaderSource( p_textureFlags
				, p_programFlags
				, p_sceneFlags
				, p_alphaFunc );
		}

		return result;
	}

	void RenderPass::PreparePipeline( BlendMode p_colourBlendMode
		, BlendMode p_alphaBlendMode
		, ComparisonFunc p_alphaFunc
		, TextureChannels & p_textureFlags
		, ProgramFlags & p_programFlags
		, SceneFlags & p_sceneFlags
		, bool p_twoSided )
	{
		DoUpdateFlags( p_textureFlags
			, p_programFlags
			, p_sceneFlags );

		if ( CheckFlag( p_programFlags, ProgramFlag::eAlphaBlending ) != m_opaque
			&& ( !CheckFlag( p_programFlags, ProgramFlag::eBillboards )
				|| !IsShadowMapProgram( p_programFlags ) ) )
		{
			if ( m_opaque )
			{
				p_alphaBlendMode = BlendMode::eNoBlend;
			}

			auto backProgram = DoGetProgram( p_textureFlags
				, p_programFlags
				, p_sceneFlags
				, p_alphaFunc
				, false );

			if ( !m_opaque )
			{
				auto frontProgram = DoGetProgram( p_textureFlags
					, p_programFlags
					, p_sceneFlags
					, p_alphaFunc
					, true );
				auto flags = PipelineFlags{ p_colourBlendMode, p_alphaBlendMode, p_textureFlags, p_programFlags, p_sceneFlags };
				DoPrepareFrontPipeline( *frontProgram, flags );
				DoPrepareBackPipeline( *backProgram, flags );
			}
			else
			{
				auto flags = PipelineFlags{ p_colourBlendMode, p_alphaBlendMode, p_textureFlags, p_programFlags, p_sceneFlags };

				if ( p_twoSided || CheckFlag( p_textureFlags, TextureChannel::eOpacity ) )
				{
					auto frontProgram = DoGetProgram( p_textureFlags
						, p_programFlags
						, p_sceneFlags
						, p_alphaFunc
						, true );
					DoPrepareFrontPipeline( *frontProgram, flags );
				}

				DoPrepareBackPipeline( *backProgram, flags );
			}
		}
	}

	RenderPipeline * RenderPass::GetPipelineFront( BlendMode p_colourBlendMode
		, BlendMode p_alphaBlendMode
		, ComparisonFunc p_alphaFunc
		, TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, SceneFlags const & p_sceneFlags )const
	{
		if ( m_opaque )
		{
			p_alphaBlendMode = BlendMode::eNoBlend;
		}

		auto it = m_frontPipelines.find( { p_colourBlendMode, p_alphaBlendMode, p_textureFlags, p_programFlags, p_sceneFlags } );
		RenderPipeline * result{ nullptr };

		if ( it != m_frontPipelines.end() )
		{
			result = it->second.get();
		}

		return result;
	}

	RenderPipeline * RenderPass::GetPipelineBack( BlendMode p_colourBlendMode
		, BlendMode p_alphaBlendMode
		, ComparisonFunc p_alphaFunc
		, TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, SceneFlags const & p_sceneFlags )const
	{
		if ( m_opaque )
		{
			p_alphaBlendMode = BlendMode::eNoBlend;
		}

		auto it = m_backPipelines.find( { p_colourBlendMode, p_alphaBlendMode, p_textureFlags, p_programFlags, p_sceneFlags } );
		RenderPipeline * result{ nullptr };

		if ( it != m_backPipelines.end() )
		{
			result = it->second.get();
		}

		return result;
	}

	SkinningRenderNode RenderPass::CreateSkinningNode( Pass & p_pass
		, RenderPipeline & p_pipeline
		, Submesh & p_submesh
		, Geometry & p_primitive
		, AnimatedSkeleton & p_skeleton )
	{
		auto buffers = p_submesh.GetGeometryBuffers( p_pipeline.GetProgram() );
		m_geometryBuffers.insert( buffers );

		return SkinningRenderNode
		{
			p_pipeline,
			DoCreatePassRenderNode( p_pass, p_pipeline ),
			m_modelMatrixUbo,
			m_modelUbo,
			*buffers,
			*p_primitive.GetParent(),
			p_submesh,
			p_primitive,
			p_skeleton,
			m_skinningUbo
		};
	}

	MorphingRenderNode RenderPass::CreateMorphingNode( Pass & p_pass
		, RenderPipeline & p_pipeline
		, Submesh & p_submesh
		, Geometry & p_primitive
		, AnimatedMesh & p_mesh )
	{
		auto buffers = p_submesh.GetGeometryBuffers( p_pipeline.GetProgram() );
		m_geometryBuffers.insert( buffers );

		return MorphingRenderNode
		{
			p_pipeline,
			DoCreatePassRenderNode( p_pass, p_pipeline ),
			m_modelMatrixUbo,
			m_modelUbo,
			*buffers,
			*p_primitive.GetParent(),
			p_submesh,
			p_primitive,
			p_mesh,
			m_morphingUbo
		};
	}

	StaticRenderNode RenderPass::CreateStaticNode( Pass & p_pass
		, RenderPipeline & p_pipeline
		, Submesh & p_submesh
		, Geometry & p_primitive )
	{
		auto buffers = p_submesh.GetGeometryBuffers( p_pipeline.GetProgram() );
		m_geometryBuffers.insert( buffers );

		return StaticRenderNode
		{
			p_pipeline,
			DoCreatePassRenderNode( p_pass, p_pipeline ),
			m_modelMatrixUbo,
			m_modelUbo,
			*buffers,
			*p_primitive.GetParent(),
			p_submesh,
			p_primitive,
		};
	}

	BillboardRenderNode RenderPass::CreateBillboardNode( Pass & p_pass
		, RenderPipeline & p_pipeline
		, BillboardBase & p_billboard )
	{
		auto buffers = p_billboard.GetGeometryBuffers( p_pipeline.GetProgram() );
		m_geometryBuffers.insert( buffers );

		return BillboardRenderNode
		{
			p_pipeline,
			DoCreatePassRenderNode( p_pass, p_pipeline ),
			m_modelMatrixUbo,
			m_modelUbo,
			*buffers,
			*p_billboard.GetNode(),
			p_billboard,
			m_billboardUbo,
		};
	}

	void RenderPass::UpdatePipeline( RenderPipeline & p_pipeline )const
	{
		DoUpdatePipeline( p_pipeline );
	}

	void RenderPass::UpdateFlags( TextureChannels & p_textureFlags
		, ProgramFlags & p_programFlags
		, SceneFlags & p_sceneFlags )const
	{
		DoUpdateFlags( p_textureFlags, p_programFlags, p_sceneFlags );
	}

	PassRenderNode RenderPass::DoCreatePassRenderNode( Pass & p_pass
		, RenderPipeline & p_pipeline )
	{
		return PassRenderNode
		{
			p_pass,
			p_pipeline.GetProgram(),
		};
	}

	SceneRenderNode RenderPass::DoCreateSceneRenderNode( Scene & p_scene
		, RenderPipeline & p_pipeline )
	{
		return SceneRenderNode{};
	}

	ShaderProgramSPtr RenderPass::DoGetProgram( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, SceneFlags const & p_sceneFlags
		, ComparisonFunc p_alphaFunc
		, bool p_invertNormals )const
	{
		return GetEngine()->GetShaderProgramCache().GetAutomaticProgram( *this
			, p_textureFlags
			, p_programFlags
			, p_sceneFlags
			, p_alphaFunc
			, p_invertNormals );
	}

	uint32_t RenderPass::DoCopyNodesMatrices( StaticRenderNodeArray const & p_renderNodes
		, VertexBuffer & p_matrixBuffer )const
	{
		return CopyNodesMatrices( p_renderNodes, p_matrixBuffer );
	}

	uint32_t RenderPass::DoCopyNodesMatrices( StaticRenderNodeArray const & p_renderNodes
		, VertexBuffer & p_matrixBuffer
		, RenderInfo & p_info )const
	{
		auto count = CopyNodesMatrices( p_renderNodes, p_matrixBuffer );
		p_info.m_visibleObjectsCount += count;
		return count;
	}

	uint32_t RenderPass::DoCopyNodesMatrices( SkinningRenderNodeArray const & p_renderNodes
		, VertexBuffer & p_matrixBuffer )const
	{
		return CopyNodesMatrices( p_renderNodes, p_matrixBuffer );
	}

	uint32_t RenderPass::DoCopyNodesMatrices( SkinningRenderNodeArray const & p_renderNodes
		, VertexBuffer & p_matrixBuffer
		, RenderInfo & p_info )const
	{
		auto count = CopyNodesMatrices( p_renderNodes, p_matrixBuffer );
		p_info.m_visibleObjectsCount += count;
		return count;
	}
	
	uint32_t RenderPass::DoCopyNodesBones( SkinningRenderNodeArray const & p_renderNodes
		, ShaderStorageBuffer & p_bonesBuffer )const
	{
		uint32_t const mtxSize = sizeof( float ) * 16;
		uint32_t const stride = mtxSize * 400u;
		auto const count = std::min( p_bonesBuffer.GetSize() / stride, uint32_t( p_renderNodes.size() ) );
		REQUIRE( count <= p_renderNodes.size() );
		auto buffer = p_bonesBuffer.GetData();
		auto it = p_renderNodes.begin();
		auto i = 0u;

		while ( i < count )
		{
			auto & node = *it;
			node.m_skeleton.FillBuffer( buffer );
			buffer += stride;
			++i;
			++it;
		}

		p_bonesBuffer.Upload( 0u, stride * count, p_bonesBuffer.GetData() );
		return count;
	}

	uint32_t RenderPass::DoCopyNodesBones( SkinningRenderNodeArray const & p_renderNodes
		, ShaderStorageBuffer & p_bonesBuffer
		, RenderInfo & p_info )const
	{
		auto count = DoCopyNodesBones( p_renderNodes, p_bonesBuffer );
		p_info.m_visibleObjectsCount += count;
		return count;
	}

	void RenderPass::DoRender( SubmeshStaticRenderNodesByPipelineMap & p_nodes )const
	{
		DoTraverseNodes( *this
			, p_nodes
			, [this]( RenderPipeline & p_pipeline
				, Pass & p_pass
				, Submesh & p_submesh
				, StaticRenderNodeArray & p_renderNodes )
			{
				if ( !p_renderNodes.empty() && p_submesh.HasMatrixBuffer() )
				{
					uint32_t count = DoCopyNodesMatrices( p_renderNodes, p_submesh.GetMatrixBuffer() );
					p_submesh.DrawInstanced( p_renderNodes[0].m_buffers, count );
				}
			} );
	}

	void RenderPass::DoRender( SubmeshStaticRenderNodesByPipelineMap & p_nodes
		, DepthMapArray & p_depthMaps )const
	{
		DoTraverseNodes( *this
			, p_nodes
			, *GetEngine()->GetRenderSystem()->GetTopScene()
			, p_depthMaps
			, [this]( RenderPipeline & p_pipeline
				, Pass & p_pass
				, Submesh & p_submesh
				, StaticRenderNodeArray & p_renderNodes )
			{
				if ( !p_renderNodes.empty() && p_submesh.HasMatrixBuffer() )
				{
					uint32_t count = DoCopyNodesMatrices( p_renderNodes, p_submesh.GetMatrixBuffer() );
					p_submesh.DrawInstanced( p_renderNodes[0].m_buffers, count );
				}
			} );
	}

	void RenderPass::DoRender( SubmeshStaticRenderNodesByPipelineMap & p_nodes
		, Camera const & p_camera )const
	{
		DoTraverseNodes( *this
			, p_camera
			, p_nodes
			, [this]( RenderPipeline & p_pipeline
				, Pass & p_pass
				, Submesh & p_submesh
				, StaticRenderNodeArray & p_renderNodes )
			{
				if ( !p_renderNodes.empty() && p_submesh.HasMatrixBuffer() )
				{
					uint32_t count = DoCopyNodesMatrices( p_renderNodes, p_submesh.GetMatrixBuffer() );
					p_submesh.DrawInstanced( p_renderNodes[0].m_buffers, count );
				}
			} );
	}

	void RenderPass::DoRender( SubmeshStaticRenderNodesByPipelineMap & p_nodes
		, Camera const & p_camera
		, DepthMapArray & p_depthMaps )const
	{
		DoTraverseNodes( *this
			, p_camera
			, p_nodes
			, *GetEngine()->GetRenderSystem()->GetTopScene()
			, p_depthMaps
			, [this]( RenderPipeline & p_pipeline
				, Pass & p_pass
				, Submesh & p_submesh
				, StaticRenderNodeArray & p_renderNodes )
			{
				if ( !p_renderNodes.empty() && p_submesh.HasMatrixBuffer() )
				{
					uint32_t count = DoCopyNodesMatrices( p_renderNodes, p_submesh.GetMatrixBuffer() );
					p_submesh.DrawInstanced( p_renderNodes[0].m_buffers, count );
				}
			} );
	}

	void RenderPass::DoRender( SubmeshStaticRenderNodesByPipelineMap & p_nodes
		, Camera const & p_camera
		, DepthMapArray & p_depthMaps
		, RenderInfo & p_info )const
	{
		DoTraverseNodes( *this
			, p_camera
			, p_nodes
			, *GetEngine()->GetRenderSystem()->GetTopScene()
			, p_depthMaps
			, [this, &p_info]( RenderPipeline & p_pipeline
				, Pass & p_pass
				, Submesh & p_submesh
				, StaticRenderNodeArray & p_renderNodes )
			{
				if ( !p_renderNodes.empty() && p_submesh.HasMatrixBuffer() )
				{
					uint32_t count = DoCopyNodesMatrices( p_renderNodes, p_submesh.GetMatrixBuffer(), p_info );
					p_submesh.DrawInstanced( p_renderNodes[0].m_buffers, count );
					++p_info.m_drawCalls;
				}
			} );
	}

	void RenderPass::DoRender( StaticRenderNodesByPipelineMap & p_nodes )const
	{
		DoRenderNonInstanced( *this
			, p_nodes );
	}

	void RenderPass::DoRender( StaticRenderNodesByPipelineMap & p_nodes
		, DepthMapArray & p_depthMaps )const
	{
		DoRenderNonInstanced( *this
			, p_nodes
			, *GetEngine()->GetRenderSystem()->GetTopScene()
			, p_depthMaps );
	}

	void RenderPass::DoRender( StaticRenderNodesByPipelineMap & p_nodes
		, Camera const & p_camera )const
	{
		DoRenderNonInstanced( *this
			, p_camera
			, p_nodes );
	}

	void RenderPass::DoRender( StaticRenderNodesByPipelineMap & p_nodes
		, Camera const & p_camera
		, DepthMapArray & p_depthMaps )const
	{
		DoRenderNonInstanced( *this
			, p_camera
			, p_nodes
			, *GetEngine()->GetRenderSystem()->GetTopScene()
			, p_depthMaps );
	}

	void RenderPass::DoRender( StaticRenderNodesByPipelineMap & p_nodes
		, Camera const & p_camera
		, DepthMapArray & p_depthMaps
		, RenderInfo & p_info )const
	{
		DoRenderNonInstanced( *this
			, p_camera
			, p_nodes
			, *GetEngine()->GetRenderSystem()->GetTopScene()
			, p_depthMaps
			, p_info );
	}

	void RenderPass::DoRender( SkinningRenderNodesByPipelineMap & p_nodes )const
	{
		DoRenderNonInstanced( *this
			, p_nodes );
	}

	void RenderPass::DoRender( SkinningRenderNodesByPipelineMap & p_nodes
		, DepthMapArray & p_depthMaps )const
	{
		DoRenderNonInstanced( *this
			, p_nodes
			, *GetEngine()->GetRenderSystem()->GetTopScene()
			, p_depthMaps );
	}

	void RenderPass::DoRender( SkinningRenderNodesByPipelineMap & p_nodes
		, Camera const & p_camera )const
	{
		DoRenderNonInstanced( *this
			, p_camera
			, p_nodes );
	}

	void RenderPass::DoRender( SkinningRenderNodesByPipelineMap & p_nodes
		, Camera const & p_camera
		, DepthMapArray & p_depthMaps )const
	{
		DoRenderNonInstanced( *this
			, p_camera
			, p_nodes
			, *GetEngine()->GetRenderSystem()->GetTopScene()
			, p_depthMaps );
	}

	void RenderPass::DoRender( SkinningRenderNodesByPipelineMap & p_nodes
		, Camera const & p_camera
		, DepthMapArray & p_depthMaps
		, RenderInfo & p_info )const
	{
		DoRenderNonInstanced( *this
			, p_camera
			, p_nodes
			, *GetEngine()->GetRenderSystem()->GetTopScene()
			, p_depthMaps
			, p_info );
	}

	void RenderPass::DoRender( SubmeshSkinningRenderNodesByPipelineMap & p_nodes )const
	{
		DoTraverseNodes( *this
			, p_nodes
			, [this]( RenderPipeline & p_pipeline
				, Pass & p_pass
				, Submesh & p_submesh
				, SkinningRenderNodeArray & p_renderNodes )
			{
				if ( !p_renderNodes.empty()
					&& p_submesh.HasInstancedBonesBuffer() 
					&& p_submesh.HasMatrixBuffer() )
				{
					uint32_t count1 = DoCopyNodesMatrices( p_renderNodes, p_submesh.GetMatrixBuffer() );
					uint32_t count2 = DoCopyNodesBones( p_renderNodes, p_submesh.GetInstancedBonesBuffer() );
					REQUIRE( count1 == count2 );
					p_submesh.GetInstancedBonesBuffer().BindTo( SkinningUbo::BindingPoint );
					p_submesh.DrawInstanced( p_renderNodes[0].m_buffers, count1 );
				}
			} );
	}

	void RenderPass::DoRender( SubmeshSkinningRenderNodesByPipelineMap & p_nodes
		, DepthMapArray & p_depthMaps )const
	{
		DoTraverseNodes( *this
			, p_nodes
			, *GetEngine()->GetRenderSystem()->GetTopScene()
			, p_depthMaps
			, [this]( RenderPipeline & p_pipeline
				, Pass & p_pass
				, Submesh & p_submesh
				, SkinningRenderNodeArray & p_renderNodes )
			{
				if ( !p_renderNodes.empty()
					&& p_submesh.HasInstancedBonesBuffer() 
					&& p_submesh.HasMatrixBuffer() )
				{
					uint32_t count1 = DoCopyNodesMatrices( p_renderNodes, p_submesh.GetMatrixBuffer() );
					uint32_t count2 = DoCopyNodesBones( p_renderNodes, p_submesh.GetInstancedBonesBuffer() );
					REQUIRE( count1 == count2 );
					p_submesh.GetInstancedBonesBuffer().BindTo( SkinningUbo::BindingPoint );
					p_submesh.DrawInstanced( p_renderNodes[0].m_buffers, count1 );
				}
			} );
	}

	void RenderPass::DoRender( SubmeshSkinningRenderNodesByPipelineMap & p_nodes
		, Camera const & p_camera )const
	{
		DoTraverseNodes( *this
			, p_camera
			, p_nodes
			, [this]( RenderPipeline & p_pipeline
				, Pass & p_pass
				, Submesh & p_submesh
				, SkinningRenderNodeArray & p_renderNodes )
			{
				if ( !p_renderNodes.empty()
					&& p_submesh.HasInstancedBonesBuffer() 
					&& p_submesh.HasMatrixBuffer() )
				{
					uint32_t count1 = DoCopyNodesMatrices( p_renderNodes, p_submesh.GetMatrixBuffer() );
					uint32_t count2 = DoCopyNodesBones( p_renderNodes, p_submesh.GetInstancedBonesBuffer() );
					REQUIRE( count1 == count2 );
					p_submesh.GetInstancedBonesBuffer().BindTo( SkinningUbo::BindingPoint );
					p_submesh.DrawInstanced( p_renderNodes[0].m_buffers, count1 );
				}
			} );
	}

	void RenderPass::DoRender( SubmeshSkinningRenderNodesByPipelineMap & p_nodes
		, Camera const & p_camera
		, DepthMapArray & p_depthMaps )const
	{
		DoTraverseNodes( *this
			, p_camera
			, p_nodes
			, *GetEngine()->GetRenderSystem()->GetTopScene()
			, p_depthMaps
			, [this]( RenderPipeline & p_pipeline
				, Pass & p_pass
				, Submesh & p_submesh
				, SkinningRenderNodeArray & p_renderNodes )
			{
				if ( !p_renderNodes.empty()
					&& p_submesh.HasInstancedBonesBuffer() 
					&& p_submesh.HasMatrixBuffer() )
				{
					uint32_t count1 = DoCopyNodesMatrices( p_renderNodes, p_submesh.GetMatrixBuffer() );
					uint32_t count2 = DoCopyNodesBones( p_renderNodes, p_submesh.GetInstancedBonesBuffer() );
					REQUIRE( count1 == count2 );
					p_submesh.GetInstancedBonesBuffer().BindTo( SkinningUbo::BindingPoint );
					p_submesh.DrawInstanced( p_renderNodes[0].m_buffers, count1 );
				}
			} );
	}

	void RenderPass::DoRender( SubmeshSkinningRenderNodesByPipelineMap & p_nodes
		, Camera const & p_camera
		, DepthMapArray & p_depthMaps
		, RenderInfo & p_info )const
	{
		DoTraverseNodes( *this
			, p_camera
			, p_nodes
			, *GetEngine()->GetRenderSystem()->GetTopScene()
			, p_depthMaps
			, [this, &p_info]( RenderPipeline & p_pipeline
				, Pass & p_pass
				, Submesh & p_submesh
				, SkinningRenderNodeArray & p_renderNodes )
			{
				if ( !p_renderNodes.empty()
					&& p_submesh.HasInstancedBonesBuffer() 
					&& p_submesh.HasMatrixBuffer() )
				{
					uint32_t count1 = DoCopyNodesMatrices( p_renderNodes, p_submesh.GetMatrixBuffer(), p_info );
					uint32_t count2 = DoCopyNodesBones( p_renderNodes, p_submesh.GetInstancedBonesBuffer(), p_info );
					REQUIRE( count1 == count2 );
					p_submesh.GetInstancedBonesBuffer().BindTo( SkinningUbo::BindingPoint );
					p_submesh.DrawInstanced( p_renderNodes[0].m_buffers, count1 );
					++p_info.m_drawCalls;
				}
			} );
	}

	void RenderPass::DoRender( MorphingRenderNodesByPipelineMap & p_nodes )const
	{
		DoRenderNonInstanced( *this
			, p_nodes );
	}

	void RenderPass::DoRender( MorphingRenderNodesByPipelineMap & p_nodes
		, DepthMapArray & p_depthMaps )const
	{
		DoRenderNonInstanced( *this
			, p_nodes
			, *GetEngine()->GetRenderSystem()->GetTopScene()
			, p_depthMaps );
	}

	void RenderPass::DoRender( MorphingRenderNodesByPipelineMap & p_nodes
		, Camera const & p_camera )const
	{
		DoRenderNonInstanced( *this
			, p_camera
			, p_nodes );
	}

	void RenderPass::DoRender( MorphingRenderNodesByPipelineMap & p_nodes
		, Camera const & p_camera
		, DepthMapArray & p_depthMaps )const
	{
		DoRenderNonInstanced( *this
			, p_camera
			, p_nodes
			, *GetEngine()->GetRenderSystem()->GetTopScene()
			, p_depthMaps );
	}

	void RenderPass::DoRender( MorphingRenderNodesByPipelineMap & p_nodes
		, Camera const & p_camera
		, DepthMapArray & p_depthMaps
		, RenderInfo & p_info )const
	{
		DoRenderNonInstanced( *this
			, p_camera
			, p_nodes
			, *GetEngine()->GetRenderSystem()->GetTopScene()
			, p_depthMaps
			, p_info );
	}

	void RenderPass::DoRender( BillboardRenderNodesByPipelineMap & p_nodes )const
	{
		DoRenderNonInstanced( *this
			, p_nodes );
	}

	void RenderPass::DoRender( BillboardRenderNodesByPipelineMap & p_nodes
		, DepthMapArray & p_depthMaps )const
	{
		DoRenderNonInstanced( *this
			, p_nodes
			, *GetEngine()->GetRenderSystem()->GetTopScene()
			, p_depthMaps );
	}

	void RenderPass::DoRender( BillboardRenderNodesByPipelineMap & p_nodes
		, Camera const & p_camera )const
	{
		DoRenderNonInstanced( *this
			, p_camera
			, p_nodes );
	}

	void RenderPass::DoRender( BillboardRenderNodesByPipelineMap & p_nodes
		, Camera const & p_camera
		, DepthMapArray & p_depthMaps )const
	{
		DoRenderNonInstanced( *this
			, p_camera
			, p_nodes
			, *GetEngine()->GetRenderSystem()->GetTopScene()
			, p_depthMaps );
	}

	void RenderPass::DoRender( BillboardRenderNodesByPipelineMap & p_nodes
		, Camera const & p_camera
		, DepthMapArray & p_depthMaps
		, RenderInfo & p_info )const
	{
		DoRenderNonInstanced( *this
			, p_camera
			, p_nodes
			, *GetEngine()->GetRenderSystem()->GetTopScene()
			, p_depthMaps
			, p_info );
	}

	GLSL::Shader RenderPass::DoGetVertexShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, SceneFlags const & p_sceneFlags
		, bool p_invertNormals )const
	{
		using namespace GLSL;
		auto writer = GetEngine()->GetRenderSystem()->CreateGlslWriter();
		// Vertex inputs
		auto position = writer.DeclAttribute< Vec4 >( ShaderProgram::Position );
		auto normal = writer.DeclAttribute< Vec3 >( ShaderProgram::Normal );
		auto tangent = writer.DeclAttribute< Vec3 >( ShaderProgram::Tangent );
		auto bitangent = writer.DeclAttribute< Vec3 >( ShaderProgram::Bitangent );
		auto texture = writer.DeclAttribute< Vec3 >( ShaderProgram::Texture );
		auto bone_ids0 = writer.DeclAttribute< IVec4 >( ShaderProgram::BoneIds0, CheckFlag( p_programFlags, ProgramFlag::eSkinning ) );
		auto bone_ids1 = writer.DeclAttribute< IVec4 >( ShaderProgram::BoneIds1, CheckFlag( p_programFlags, ProgramFlag::eSkinning ) );
		auto weights0 = writer.DeclAttribute< Vec4 >( ShaderProgram::Weights0, CheckFlag( p_programFlags, ProgramFlag::eSkinning ) );
		auto weights1 = writer.DeclAttribute< Vec4 >( ShaderProgram::Weights1, CheckFlag( p_programFlags, ProgramFlag::eSkinning ) );
		auto transform = writer.DeclAttribute< Mat4 >( ShaderProgram::Transform, CheckFlag( p_programFlags, ProgramFlag::eInstantiation ) );
		auto position2 = writer.DeclAttribute< Vec4 >( ShaderProgram::Position2, CheckFlag( p_programFlags, ProgramFlag::eMorphing ) );
		auto normal2 = writer.DeclAttribute< Vec3 >( ShaderProgram::Normal2, CheckFlag( p_programFlags, ProgramFlag::eMorphing ) );
		auto tangent2 = writer.DeclAttribute< Vec3 >( ShaderProgram::Tangent2, CheckFlag( p_programFlags, ProgramFlag::eMorphing ) );
		auto bitangent2 = writer.DeclAttribute< Vec3 >( ShaderProgram::Bitangent2, CheckFlag( p_programFlags, ProgramFlag::eMorphing ) );
		auto texture2 = writer.DeclAttribute< Vec3 >( ShaderProgram::Texture2, CheckFlag( p_programFlags, ProgramFlag::eMorphing ) );
		auto gl_InstanceID( writer.DeclBuiltin< Int >( cuT( "gl_InstanceID" ) ) );

		UBO_MATRIX( writer );
		UBO_MODEL_MATRIX( writer );
		UBO_SCENE( writer );
		SkinningUbo::Declare( writer, p_programFlags );
		UBO_MORPHING( writer, p_programFlags );

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

			if ( CheckFlag( p_programFlags, ProgramFlag::eSkinning ) )
			{
				mtxModel = SkinningUbo::ComputeTransform( writer, p_programFlags );
			}
			else if ( CheckFlag( p_programFlags, ProgramFlag::eInstantiation ) )
			{
				mtxModel = transform;
			}
			else
			{
				mtxModel = c3d_mtxModel;
			}

			if ( CheckFlag( p_programFlags, ProgramFlag::eMorphing ) )
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

			if ( p_invertNormals )
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
