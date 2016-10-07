#include "RenderPass.hpp"

#include "Engine.hpp"

#include "Material/Pass.hpp"
#include "Mesh/Submesh.hpp"
#include "Mesh/Buffer/VertexBuffer.hpp"
#include "Render/Pipeline.hpp"
#include "Scene/BillboardList.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Geometry.hpp"
#include "Scene/Scene.hpp"
#include "Scene/SceneNode.hpp"
#include "Shader/FrameVariableBuffer.hpp"
#include "Shader/ShaderProgram.hpp"

using namespace Castor;

namespace Castor3D
{
	namespace
	{
		template< bool Opaque >
		struct PipelineUpdater
		{
			static inline void Update( RenderPass const & p_pass, Camera const & p_camera, Pipeline & p_pipeline )
			{
				p_pass.UpdateOpaquePipeline( p_camera, p_pipeline );
			}
		};

		template<>
		struct PipelineUpdater< false >
		{
			static inline void Update( RenderPass const & p_pass, Camera const & p_camera, Pipeline & p_pipeline )
			{
				p_pass.UpdateTransparentPipeline( p_camera, p_pipeline );
			}
		};

		template< bool Opaque, typename MapType, typename FuncType >
		inline void DoTraverseNodes( RenderPass const & p_pass
									 , Camera const & p_camera
									 , MapType & p_nodes
									 , FuncType p_function )
		{
			for ( auto l_itPipelines : p_nodes )
			{
				PipelineUpdater< Opaque >::Update( p_pass, p_camera, *l_itPipelines.first );
				l_itPipelines.first->Apply();

				for ( auto l_itPass : l_itPipelines.second )
				{
					for ( auto l_itSubmeshes : l_itPass.second )
					{
						p_function( *l_itPipelines.first, *l_itPass.first, *l_itSubmeshes.first, l_itSubmeshes.second );
					}
				}
			}
		}

		template< bool Opaque, typename MapType >
		inline void DoRenderNonInstanced( RenderPass const & p_pass
										  , Scene & p_scene
										  , Camera const & p_camera
										  , MapType & p_nodes
										  , bool p_register
										  , std::vector< std::reference_wrapper< ObjectRenderNodeBase const > > & p_renderedObjects )
		{
			for ( auto l_itPipelines : p_nodes )
			{
				PipelineUpdater< Opaque >::Update( p_pass, p_camera, *l_itPipelines.first );
				l_itPipelines.first->Apply();

				for ( auto & l_renderNode : l_itPipelines.second )
				{
					l_renderNode.Render( p_scene, p_camera );

					if ( p_register )
					{
						p_renderedObjects.push_back( l_renderNode );
					}
				}
			}
		}
	}

	RenderPass::RenderPass( String const & p_name, Engine & p_engine, bool p_multisampling )
		: OwnedBy< Engine >{ p_engine }
		, Named{ p_name }
		, m_renderSystem{ *p_engine.GetRenderSystem() }
		, m_multisampling{ p_multisampling }
		, m_renderQueue{ *this }
	{
	}

	RenderPass::~RenderPass()
	{
	}

	String RenderPass::GetPixelShaderSource( uint16_t p_textureFlags, uint8_t p_programFlags, uint8_t p_sceneFlags )const
	{
		String l_return;

		if ( CheckFlag( p_programFlags, ProgramFlag::AlphaBlending ) )
		{
			l_return = DoGetTransparentPixelShaderSource( p_textureFlags, p_programFlags, p_sceneFlags );
		}
		else
		{
			l_return = DoGetOpaquePixelShaderSource( p_textureFlags, p_programFlags, p_sceneFlags );
		}

		return l_return;
	}

	void RenderPass::PreparePipeline( BlendMode p_colourBlendMode, BlendMode p_alphaBlendMode, uint16_t p_textureFlags, uint8_t & p_programFlags, uint8_t p_sceneFlags )
	{
		DoCompleteProgramFlags( p_programFlags );
		auto l_program = DoGetProgram( p_textureFlags, p_programFlags, p_sceneFlags );

		if ( CheckFlag( p_programFlags, ProgramFlag::AlphaBlending ) )
		{
			auto l_flags = PipelineFlags{ p_colourBlendMode, p_alphaBlendMode, p_textureFlags, p_programFlags, p_sceneFlags };
			DoPrepareTransparentFrontPipeline( *l_program, l_flags );
			DoPrepareTransparentBackPipeline( *l_program, l_flags );
		}
		else
		{
			DoPrepareOpaquePipeline( *l_program, { p_colourBlendMode, BlendMode::NoBlend, p_textureFlags, p_programFlags, p_sceneFlags } );
		}
	}

	Pipeline & RenderPass::GetOpaquePipeline( BlendMode p_colourBlendMode, uint16_t p_textureFlags, uint8_t p_programFlags, uint8_t p_sceneFlags )
	{
		auto l_it = m_opaquePipelines.find( { p_colourBlendMode, BlendMode::NoBlend, p_textureFlags, p_programFlags, p_sceneFlags } );
		REQUIRE( l_it != m_opaquePipelines.end() );
		return *l_it->second;
	}

	Pipeline & RenderPass::GetTransparentPipelineFront( BlendMode p_colourBlendMode, BlendMode p_alphaBlendMode, uint16_t p_textureFlags, uint8_t p_programFlags, uint8_t p_sceneFlags )
	{
		auto l_it = m_frontTransparentPipelines.find( { p_colourBlendMode, p_alphaBlendMode, p_textureFlags, p_programFlags, p_sceneFlags } );
		REQUIRE( l_it != m_frontTransparentPipelines.end() );
		return *l_it->second;
	}

	Pipeline & RenderPass::GetTransparentPipelineBack( BlendMode p_colourBlendMode, BlendMode p_alphaBlendMode, uint16_t p_textureFlags, uint8_t p_programFlags, uint8_t p_sceneFlags )
	{
		auto l_it = m_backTransparentPipelines.find( { p_colourBlendMode, p_alphaBlendMode, p_textureFlags, p_programFlags, p_sceneFlags } );
		REQUIRE( l_it != m_backTransparentPipelines.end() );
		return *l_it->second;
	}

	AnimatedGeometryRenderNode RenderPass::CreateAnimatedNode( Pass & p_pass
															   , Pipeline & p_pipeline
															   , Submesh & p_submesh
															   , Geometry & p_primitive
															   , AnimatedSkeletonSPtr p_skeleton
															   , AnimatedMeshSPtr p_mesh )
	{
		auto l_animationBuffer = p_pipeline.GetProgram().FindFrameVariableBuffer( ShaderProgram::BufferAnimation );

		return AnimatedGeometryRenderNode
		{
			DoCreateSceneRenderNode( *p_primitive.GetScene(), p_pipeline ),
			DoCreatePassRenderNode( p_pass, p_pipeline ),
			*p_submesh.GetGeometryBuffers( p_pipeline.GetProgram() ),
			*p_primitive.GetParent(),
			p_submesh,
			p_primitive,
			p_skeleton.get(),
			p_mesh.get(),
			*l_animationBuffer
		};
	}

	StaticGeometryRenderNode RenderPass::CreateStaticNode( Pass & p_pass
														   , Pipeline & p_pipeline
														   , Submesh & p_submesh
														   , Geometry & p_primitive )
	{
		return StaticGeometryRenderNode
		{
			DoCreateSceneRenderNode( *p_primitive.GetScene(), p_pipeline ),
			DoCreatePassRenderNode( p_pass, p_pipeline ),
			*p_submesh.GetGeometryBuffers( p_pipeline.GetProgram() ),
			*p_primitive.GetParent(),
			p_submesh,
			p_primitive,
		};
	}

	BillboardRenderNode RenderPass::CreateBillboardNode( Pass & p_pass
														 , Pipeline & p_pipeline
														 , BillboardList & p_billboard )
	{
		auto l_billboardBuffer = p_pipeline.GetProgram().FindFrameVariableBuffer( ShaderProgram::BufferBillboards );
		Point2iFrameVariableSPtr l_pt2i;

		return BillboardRenderNode
		{
			DoCreateSceneRenderNode( *p_billboard.GetScene(), p_pipeline ),
			DoCreatePassRenderNode( p_pass, p_pipeline ),
			*p_billboard.GetGeometryBuffers( p_pipeline.GetProgram() ),
			*p_billboard.GetParent(),
			p_billboard,
			*l_billboardBuffer,
			*l_billboardBuffer->GetVariable( ShaderProgram::Dimensions, l_pt2i )
		};
	}

	void RenderPass::UpdateOpaquePipeline( Camera const & p_camera, Pipeline & p_pipeline )const
	{
		p_pipeline.SetProjectionMatrix( p_camera.GetViewport().GetProjection() );
		p_pipeline.SetViewMatrix( p_camera.GetView() );
		DoUpdateOpaquePipeline( p_camera, p_pipeline );
	}

	void RenderPass::UpdateTransparentPipeline( Camera const & p_camera, Pipeline & p_pipeline )const
	{
		p_pipeline.SetProjectionMatrix( p_camera.GetViewport().GetProjection() );
		p_pipeline.SetViewMatrix( p_camera.GetView() );
		DoUpdateTransparentPipeline( p_camera, p_pipeline );
	}

	void RenderPass::DoRenderOpaqueStaticSubmeshesNonInstanced( Scene & p_scene, Camera const & p_camera, StaticGeometryRenderNodesByPipelineMap & p_nodes, bool p_register )
	{
		DoRenderNonInstanced< true >( *this, p_scene, p_camera, p_nodes, p_register, m_renderedObjects );
	}

	void RenderPass::DoRenderOpaqueAnimatedSubmeshesNonInstanced( Scene & p_scene, Camera const & p_camera, AnimatedGeometryRenderNodesByPipelineMap & p_nodes, bool p_register )
	{
		DoRenderNonInstanced< true >( *this, p_scene, p_camera, p_nodes, p_register, m_renderedObjects );
	}

	void RenderPass::DoRenderOpaqueInstancedSubmeshesNonInstanced( Scene & p_scene, Camera const & p_camera, SubmeshStaticRenderNodesByPipelineMap & p_nodes, bool p_register )
	{
		DoTraverseNodes< true >( *this, p_camera, p_nodes, [&p_scene, &p_camera, &p_register, this]( Pipeline & p_pipeline, Pass & p_pass, Submesh & p_submesh, StaticGeometryRenderNodeArray & p_renderNodes )
		{
			for ( auto & l_renderNode : p_renderNodes )
			{
				l_renderNode.Render( p_scene, p_camera );

				if ( p_register )
				{
					m_renderedObjects.push_back( l_renderNode );
				}
			}
		} );
	}

	void RenderPass::DoRenderOpaqueInstancedSubmeshesInstanced( Scene & p_scene, Camera const & p_camera, SubmeshStaticRenderNodesByPipelineMap & p_nodes, bool p_register )
	{
		DoTraverseNodes< true >( *this, p_camera, p_nodes, [&p_scene, &p_camera, &p_register, this]( Pipeline & p_pipeline, Pass & p_pass, Submesh & p_submesh, StaticGeometryRenderNodeArray & p_renderNodes )
		{
			if ( !p_renderNodes.empty() && p_submesh.HasMatrixBuffer() )
			{
				uint32_t l_count = uint32_t( p_renderNodes.size() );
				uint8_t * l_buffer = p_submesh.GetMatrixBuffer().data();
				const uint32_t l_stride = 16 * sizeof( real );

				for ( auto const & l_renderNode : p_renderNodes )
				{
					std::memcpy( l_buffer, l_renderNode.m_sceneNode.GetDerivedTransformationMatrix().const_ptr(), l_stride );
					l_buffer += l_stride;

					if ( p_register )
					{
						m_renderedObjects.push_back( l_renderNode );
					}
				}

				p_renderNodes[0].BindPass( p_scene, p_camera, MASK_MTXMODE_MODEL );
				p_submesh.DrawInstanced( p_renderNodes[0].m_buffers, l_count );
				p_renderNodes[0].UnbindPass( p_scene );
			}
		} );
	}

	void RenderPass::DoRenderTransparentStaticSubmeshesNonInstanced( Scene & p_scene, Camera const & p_camera, StaticGeometryRenderNodesByPipelineMap & p_nodes, bool p_register )
	{
		DoRenderNonInstanced< false >( *this, p_scene, p_camera, p_nodes, p_register, m_renderedObjects );
	}

	void RenderPass::DoRenderTransparentAnimatedSubmeshesNonInstanced( Scene & p_scene, Camera const & p_camera, AnimatedGeometryRenderNodesByPipelineMap & p_nodes, bool p_register )
	{
		DoRenderNonInstanced< false >( *this, p_scene, p_camera, p_nodes, p_register, m_renderedObjects );
	}

	void RenderPass::DoRenderTransparentInstancedSubmeshesNonInstanced( Scene & p_scene, Camera const & p_camera, SubmeshStaticRenderNodesByPipelineMap & p_nodes, bool p_register )
	{
		DoTraverseNodes< false >( *this, p_camera, p_nodes, [&p_scene, &p_camera, &p_register, this]( Pipeline & p_pipeline, Pass & p_pass, Submesh & p_submesh, StaticGeometryRenderNodeArray & p_renderNodes )
		{
			for ( auto & l_renderNode : p_renderNodes )
			{
				l_renderNode.Render( p_scene, p_camera );

				if ( p_register )
				{
					m_renderedObjects.push_back( l_renderNode );
				}
			}
		} );
	}

	void RenderPass::DoRenderTransparentInstancedSubmeshesInstanced( Scene & p_scene, Camera const & p_camera, SubmeshStaticRenderNodesByPipelineMap & p_nodes, bool p_register )
	{
		DoTraverseNodes< false >( *this, p_camera, p_nodes, [&p_scene, &p_camera, &p_register, this]( Pipeline & p_pipeline, Pass & p_pass, Submesh & p_submesh, StaticGeometryRenderNodeArray & p_renderNodes )
		{
			if ( !p_renderNodes.empty() && p_submesh.HasMatrixBuffer() )
			{
				uint32_t l_count = uint32_t( p_renderNodes.size() );
				uint8_t * l_buffer = p_submesh.GetMatrixBuffer().data();
				const uint32_t l_stride = 16 * sizeof( real );

				for ( auto const & l_renderNode : p_renderNodes )
				{
					std::memcpy( l_buffer, l_renderNode.m_sceneNode.GetDerivedTransformationMatrix().const_ptr(), l_stride );
					l_buffer += l_stride;

					if ( p_register )
					{
						m_renderedObjects.push_back( l_renderNode );
					}
				}

				p_renderNodes[0].BindPass( p_scene, p_camera, MASK_MTXMODE_MODEL );
				p_submesh.DrawInstanced( p_renderNodes[0].m_buffers, l_count );
				p_renderNodes[0].UnbindPass( p_scene );
			}
		} );
	}

	void RenderPass::DoRenderByDistance( Scene & p_scene, Camera const & p_camera, DistanceSortedNodeMap & p_nodes, bool p_register )
	{
		for ( auto & l_it : p_nodes )
		{
			l_it.second.get().m_pass.m_pipeline.Apply();
			DoUpdateTransparentPipeline( p_camera, l_it.second.get().m_pass.m_pipeline );
			l_it.second.get().Render( p_scene, p_camera );

			if ( p_register )
			{
				m_renderedObjects.push_back( l_it.second.get() );
			}
		}
	}

	void RenderPass::DoRenderOpaqueBillboards( Scene & p_scene, Camera const & p_camera, BillboardRenderNodesByPipelineMap & p_nodes, bool p_register )
	{
		DoRenderNonInstanced< true >( *this, p_scene, p_camera, p_nodes, p_register, m_renderedObjects );
	}

	void RenderPass::DoRenderTransparentBillboards( Scene & p_scene, Camera const & p_camera, BillboardRenderNodesByPipelineMap & p_nodes, bool p_register )
	{
		DoRenderNonInstanced< false >( *this, p_scene, p_camera, p_nodes, p_register, m_renderedObjects );
	}

	PassRenderNode RenderPass::DoCreatePassRenderNode( Pass & p_pass, Pipeline & p_pipeline )
	{
		FrameVariableBufferSPtr l_passBuffer = p_pipeline.GetProgram().FindFrameVariableBuffer( ShaderProgram::BufferPass );
		FrameVariableBufferSPtr l_matrixBuffer = p_pipeline.GetProgram().FindFrameVariableBuffer( ShaderProgram::BufferMatrix );
		FrameVariableBufferSPtr l_animationBuffer = p_pipeline.GetProgram().FindFrameVariableBuffer( ShaderProgram::BufferAnimation );
		Point4rFrameVariableSPtr l_pt4r;
		OneFloatFrameVariableSPtr l_1f;

		auto l_node = PassRenderNode
		{
			p_pass,
			p_pipeline,
			*l_matrixBuffer,
			*l_passBuffer,
			*l_passBuffer->GetVariable( ShaderProgram::MatAmbient, l_pt4r ),
			*l_passBuffer->GetVariable( ShaderProgram::MatDiffuse, l_pt4r ),
			*l_passBuffer->GetVariable( ShaderProgram::MatSpecular, l_pt4r ),
			*l_passBuffer->GetVariable( ShaderProgram::MatEmissive, l_pt4r ),
			*l_passBuffer->GetVariable( ShaderProgram::MatShininess, l_1f ),
			*l_passBuffer->GetVariable( ShaderProgram::MatOpacity, l_1f ),
		};
		p_pass.FillRenderNode( l_node );
		return l_node;
	}

	SceneRenderNode RenderPass::DoCreateSceneRenderNode( Scene & p_scene, Pipeline & p_pipeline )
	{
		FrameVariableBufferSPtr l_sceneBuffer = p_pipeline.GetProgram().FindFrameVariableBuffer( ShaderProgram::BufferScene );
		Point3rFrameVariableSPtr l_pt3r;

		return SceneRenderNode
		{
			*l_sceneBuffer,
			*l_sceneBuffer->GetVariable( ShaderProgram::CameraPos, l_pt3r )
		};
	}

	ShaderProgramSPtr RenderPass::DoGetProgram( uint16_t p_textureFlags, uint8_t p_programFlags, uint8_t p_sceneFlags )const
	{
		ShaderProgramSPtr l_program;

		if ( CheckFlag( p_programFlags, ProgramFlag::Billboards ) )
		{
			l_program = GetEngine()->GetShaderProgramCache().GetBillboardProgram( p_textureFlags, p_programFlags, p_sceneFlags );

			if ( !l_program )
			{
				l_program = GetEngine()->GetRenderSystem()->CreateBillboardsProgram( *this, p_textureFlags, p_programFlags, p_sceneFlags );
				GetEngine()->GetShaderProgramCache().AddBillboardProgram( l_program, p_textureFlags, p_programFlags, p_sceneFlags );
			}
		}
		else
		{
			l_program = GetEngine()->GetShaderProgramCache().GetAutomaticProgram( *this, p_textureFlags, p_programFlags, p_sceneFlags );
		}

		return l_program;
	}

	void RenderPass::DoCompleteProgramFlags( uint8_t & p_programFlags )const
	{
	}
}
