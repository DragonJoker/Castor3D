#include "RenderPass.hpp"

#include "Engine.hpp"

#include "Material/Pass.hpp"
#include "Mesh/Submesh.hpp"
#include "Mesh/Buffer/GeometryBuffers.hpp"
#include "Mesh/Buffer/VertexBuffer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Scene/BillboardList.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Geometry.hpp"
#include "Scene/Scene.hpp"
#include "Scene/SceneNode.hpp"
#include "Shader/UniformBuffer.hpp"
#include "Shader/ShaderProgram.hpp"

#include <GlslSource.hpp>

using namespace Castor;

namespace Castor3D
{
	namespace
	{
		template< typename MapType, typename FuncType >
		inline void DoTraverseNodes( RenderPass const & p_pass
			, Camera const & p_camera
			, MapType & p_nodes
			, DepthMapArray & p_depthMaps
			, FuncType p_function )
		{
			for ( auto l_itPipelines : p_nodes )
			{
				p_pass.UpdatePipeline( p_camera, *l_itPipelines.first, p_depthMaps );
				l_itPipelines.first->Apply();

				for ( auto l_itPass : l_itPipelines.second )
				{
					for ( auto l_itSubmeshes : l_itPass.second )
					{
						p_function( *l_itPipelines.first
							, *l_itPass.first
							, *l_itSubmeshes.first
							, l_itSubmeshes.second
							, p_depthMaps );
					}
				}
			}
		}

		template< typename MapType, typename FuncType >
		inline void DoTraverseNodes( RenderPass const & p_pass
			, MapType & p_nodes
			, DepthMapArray & p_depthMaps
			, FuncType p_function )
		{
			for ( auto l_itPipelines : p_nodes )
			{
				l_itPipelines.first->Apply();

				for ( auto l_itPass : l_itPipelines.second )
				{
					for ( auto l_itSubmeshes : l_itPass.second )
					{
						p_function( *l_itPipelines.first
							, *l_itPass.first
							, *l_itSubmeshes.first
							, l_itSubmeshes.second
							, p_depthMaps );
					}
				}
			}
		}

		template< typename MapType >
		inline void DoRenderNonInstanced( RenderPass const & p_pass
			, Camera const & p_camera
			, MapType & p_nodes
			, DepthMapArray & p_depthMaps )
		{
			for ( auto l_itPipelines : p_nodes )
			{
				p_pass.UpdatePipeline( p_camera, *l_itPipelines.first, p_depthMaps );
				l_itPipelines.first->Apply();

				for ( auto & l_renderNode : l_itPipelines.second )
				{
					l_renderNode.Render( p_depthMaps );
				}
			}
		}

		template< typename MapType >
		inline void DoRenderNonInstanced( RenderPass const & p_pass
			, MapType & p_nodes
			, DepthMapArray & p_depthMaps )
		{
			for ( auto l_itPipelines : p_nodes )
			{
				l_itPipelines.first->Apply();

				for ( auto & l_renderNode : l_itPipelines.second )
				{
					l_renderNode.Render( p_depthMaps );
				}
			}
		}

		template< typename MapType >
		inline void DoRenderNonInstanced( RenderPass const & p_pass
			, Camera const & p_camera
			, MapType & p_nodes
			, DepthMapArray & p_depthMaps
			, uint32_t & p_count )
		{
			for ( auto l_itPipelines : p_nodes )
			{
				p_pass.UpdatePipeline( p_camera, *l_itPipelines.first, p_depthMaps );
				l_itPipelines.first->Apply();

				for ( auto & l_renderNode : l_itPipelines.second )
				{
					l_renderNode.Render( p_depthMaps );
					++p_count;
				}
			}
		}

		template< typename MapType >
		inline void DoRenderNonInstanced( RenderPass const & p_pass
			, MapType & p_nodes
			, DepthMapArray & p_depthMaps
			, uint32_t & p_count )
		{
			for ( auto l_itPipelines : p_nodes )
			{
				l_itPipelines.first->Apply();

				for ( auto & l_renderNode : l_itPipelines.second )
				{
					l_renderNode.Render( p_depthMaps );
					++p_count;
				}
			}
		}
	}

	RenderPass::RenderPass( String const & p_name, Engine & p_engine, bool p_opaque, bool p_multisampling )
		: OwnedBy< Engine >{ p_engine }
		, Named{ p_name }
		, m_renderSystem{ *p_engine.GetRenderSystem() }
		, m_multisampling{ p_multisampling }
		, m_renderQueue{ *this, p_opaque }
		, m_opaque{ p_opaque }
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
		DoCleanup();

		for ( auto & l_buffers : m_geometryBuffers )
		{
			l_buffers->Cleanup();
		}

		m_geometryBuffers.clear();
	}

	void RenderPass::Update( RenderQueueArray & p_queues )
	{
		DoUpdate( p_queues );
	}

	String RenderPass::GetVertexShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, uint8_t p_sceneFlags, bool p_invertNormals )const
	{
		return DoGetVertexShaderSource( p_textureFlags, p_programFlags, p_sceneFlags, p_invertNormals );
	}

	String RenderPass::GetPixelShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, uint8_t p_sceneFlags )const
	{
		return DoGetPixelShaderSource( p_textureFlags, p_programFlags, p_sceneFlags );
	}

	String RenderPass::GetGeometryShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, uint8_t p_sceneFlags )const
	{
		return DoGetGeometryShaderSource( p_textureFlags, p_programFlags, p_sceneFlags );
	}

	void RenderPass::PreparePipeline( BlendMode p_colourBlendMode
		, BlendMode p_alphaBlendMode
		, TextureChannels & p_textureFlags
		, ProgramFlags & p_programFlags
		, uint8_t p_sceneFlags
		, bool p_twoSided )
	{
		DoUpdateFlags( p_textureFlags, p_programFlags );

		if ( CheckFlag( p_programFlags, ProgramFlag::eAlphaBlending ) != m_opaque
			&& ( !CheckFlag( p_programFlags, ProgramFlag::eBillboards )
				|| !CheckFlag( p_programFlags, ProgramFlag::eShadowMap ) ) )
		{
			if ( m_opaque )
			{
				p_alphaBlendMode = BlendMode::eNoBlend;
			}

			auto l_backProgram = DoGetProgram( p_textureFlags, p_programFlags, p_sceneFlags, false );

			if ( !m_opaque )
			{
				auto l_frontProgram = DoGetProgram( p_textureFlags, p_programFlags, p_sceneFlags, true );
				auto l_flags = PipelineFlags{ p_colourBlendMode, p_alphaBlendMode, p_textureFlags, p_programFlags, p_sceneFlags };
				DoPrepareFrontPipeline( *l_frontProgram, l_flags );
				DoPrepareBackPipeline( *l_backProgram, l_flags );
			}
			else
			{
				auto l_flags = PipelineFlags{ p_colourBlendMode, p_alphaBlendMode, p_textureFlags, p_programFlags, p_sceneFlags };

				if ( p_twoSided )
				{
					auto l_frontProgram = DoGetProgram( p_textureFlags, p_programFlags, p_sceneFlags, true );
					DoPrepareFrontPipeline( *l_frontProgram, l_flags );
				}

				DoPrepareBackPipeline( *l_backProgram, l_flags );
			}
		}
	}

	RenderPipeline * RenderPass::GetPipelineFront( BlendMode p_colourBlendMode
		, BlendMode p_alphaBlendMode
		, TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, uint8_t p_sceneFlags )
	{
		if ( m_opaque )
		{
			p_alphaBlendMode = BlendMode::eNoBlend;
		}

		auto l_it = m_frontPipelines.find( { p_colourBlendMode, p_alphaBlendMode, p_textureFlags, p_programFlags, p_sceneFlags } );
		RenderPipeline * l_return{ nullptr };

		if ( l_it != m_frontPipelines.end() )
		{
			l_return = l_it->second.get();
		}

		return l_return;
	}

	RenderPipeline * RenderPass::GetPipelineBack( BlendMode p_colourBlendMode
		, BlendMode p_alphaBlendMode
		, TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, uint8_t p_sceneFlags )
	{
		if ( m_opaque )
		{
			p_alphaBlendMode = BlendMode::eNoBlend;
		}

		auto l_it = m_backPipelines.find( { p_colourBlendMode, p_alphaBlendMode, p_textureFlags, p_programFlags, p_sceneFlags } );
		RenderPipeline * l_return{ nullptr };

		if ( l_it != m_backPipelines.end() )
		{
			l_return = l_it->second.get();
		}

		return l_return;
	}

	AnimatedGeometryRenderNode RenderPass::CreateAnimatedNode( Pass & p_pass
		, RenderPipeline & p_pipeline
		, Submesh & p_submesh
		, Geometry & p_primitive
		, AnimatedSkeletonSPtr p_skeleton
		, AnimatedMeshSPtr p_mesh )
	{
		auto l_modelBuffer = p_pipeline.GetProgram().FindUniformBuffer( ShaderProgram::BufferModel );
		auto l_animationBuffer = p_pipeline.GetProgram().FindUniformBuffer( ShaderProgram::BufferAnimation );
		auto l_buffer = p_submesh.GetGeometryBuffers( p_pipeline.GetProgram() );
		m_geometryBuffers.insert( l_buffer );

		return AnimatedGeometryRenderNode
		{
			DoCreateSceneRenderNode( *p_primitive.GetScene(), p_pipeline ),
			DoCreatePassRenderNode( p_pass, p_pipeline ),
			*l_buffer,
			*p_primitive.GetParent(),
			*l_modelBuffer->GetUniform< UniformType::eInt >( ShaderProgram::ShadowReceiver ),
			p_submesh,
			p_primitive,
			p_skeleton.get(),
			p_mesh.get(),
			*l_animationBuffer
		};
	}

	StaticGeometryRenderNode RenderPass::CreateStaticNode( Pass & p_pass
		, RenderPipeline & p_pipeline
		, Submesh & p_submesh
		, Geometry & p_primitive )
	{
		auto l_modelBuffer = p_pipeline.GetProgram().FindUniformBuffer( ShaderProgram::BufferModel );
		auto l_buffer = p_submesh.GetGeometryBuffers( p_pipeline.GetProgram() );
		m_geometryBuffers.insert( l_buffer );

		return StaticGeometryRenderNode
		{
			DoCreateSceneRenderNode( *p_primitive.GetScene(), p_pipeline ),
			DoCreatePassRenderNode( p_pass, p_pipeline ),
			*l_buffer,
			*p_primitive.GetParent(),
			*l_modelBuffer->GetUniform< UniformType::eInt >( ShaderProgram::ShadowReceiver ),
			p_submesh,
			p_primitive,
		};
	}

	BillboardRenderNode RenderPass::CreateBillboardNode( Pass & p_pass
		, RenderPipeline & p_pipeline
		, BillboardBase & p_billboard )
	{
		auto l_modelBuffer = p_pipeline.GetProgram().FindUniformBuffer( ShaderProgram::BufferModel );
		auto l_billboardBuffer = p_pipeline.GetProgram().FindUniformBuffer( ShaderProgram::BufferBillboards );
		auto l_buffer = p_billboard.GetGeometryBuffers( p_pipeline.GetProgram() );
		m_geometryBuffers.insert( l_buffer );

		return BillboardRenderNode
		{
			DoCreateSceneRenderNode( p_billboard.GetParentScene(), p_pipeline ),
			DoCreatePassRenderNode( p_pass, p_pipeline ),
			*l_buffer,
			*p_billboard.GetNode(),
			*l_modelBuffer->GetUniform< UniformType::eInt >( ShaderProgram::ShadowReceiver ),
			p_billboard,
			*l_billboardBuffer,
			*l_billboardBuffer->GetUniform< UniformType::eVec2i >( ShaderProgram::Dimensions ),
			*l_billboardBuffer->GetUniform< UniformType::eVec2i >( ShaderProgram::WindowSize )
		};
	}

	void RenderPass::UpdatePipeline( Camera const & p_camera
		, RenderPipeline & p_pipeline
		, DepthMapArray & p_depthMaps )const
	{
		p_pipeline.SetProjectionMatrix( p_camera.GetViewport().GetProjection() );
		p_pipeline.SetViewMatrix( p_camera.GetView() );
		DoUpdatePipeline( p_pipeline, p_depthMaps );
	}

	PassRenderNode RenderPass::DoCreatePassRenderNode( Pass & p_pass
		, RenderPipeline & p_pipeline )
	{
		auto l_passBuffer = p_pipeline.GetProgram().FindUniformBuffer( ShaderProgram::BufferPass );
		auto l_matrixBuffer = p_pipeline.GetProgram().FindUniformBuffer( ShaderProgram::BufferMatrix );
		auto l_animationBuffer = p_pipeline.GetProgram().FindUniformBuffer( ShaderProgram::BufferAnimation );
		Uniform4rSPtr l_pt4r;
		Uniform1fSPtr l_1f;

		auto l_node = PassRenderNode
		{
			p_pass,
			p_pipeline,
			*l_matrixBuffer,
			*l_passBuffer->GetUniform< UniformType::eVec4f >( ShaderProgram::MatAmbient ),
			*l_passBuffer->GetUniform< UniformType::eVec4f >( ShaderProgram::MatDiffuse ),
			*l_passBuffer->GetUniform< UniformType::eVec4f >( ShaderProgram::MatSpecular ),
			*l_passBuffer->GetUniform< UniformType::eVec4f >( ShaderProgram::MatEmissive ),
			*l_passBuffer->GetUniform< UniformType::eFloat >( ShaderProgram::MatShininess ),
			*l_passBuffer->GetUniform< UniformType::eFloat >( ShaderProgram::MatOpacity ),
		};
		p_pass.FillRenderNode( l_node );
		return l_node;
	}

	SceneRenderNode RenderPass::DoCreateSceneRenderNode( Scene & p_scene
		, RenderPipeline & p_pipeline )
	{
		UniformBufferSPtr l_sceneBuffer = p_pipeline.GetProgram().FindUniformBuffer( ShaderProgram::BufferScene );

		return SceneRenderNode
		{
			*l_sceneBuffer,
			*l_sceneBuffer->GetUniform< UniformType::eVec3f >( ShaderProgram::CameraPos )
		};
	}

	ShaderProgramSPtr RenderPass::DoGetProgram( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, uint8_t p_sceneFlags
		, bool p_invertNormals )const
	{
		return GetEngine()->GetShaderProgramCache().GetAutomaticProgram( *this, p_textureFlags, p_programFlags, p_sceneFlags, p_invertNormals );
	}

	uint32_t RenderPass::DoCopyNodesMatrices( StaticGeometryRenderNodeArray const & p_renderNodes
		, VertexBuffer & p_matrixBuffer )
	{
		constexpr uint32_t l_stride = 16 * sizeof( real );
		auto const l_count = std::min( p_matrixBuffer.GetSize() / l_stride, uint32_t( p_renderNodes.size() ) );
		auto l_buffer = p_matrixBuffer.data();
		auto l_it = p_renderNodes.begin();
		auto i = 0u;

		while ( i < l_count )
		{
			std::memcpy( l_buffer, l_it->m_sceneNode.GetDerivedTransformationMatrix().const_ptr(), l_stride );
			l_buffer += l_stride;
			++i;
			++l_it;
		}

		p_matrixBuffer.Upload( 0u, l_stride * l_count, p_matrixBuffer.data() );
		return l_count;
	}

	uint32_t RenderPass::DoCopyNodesMatrices( StaticGeometryRenderNodeArray const & p_renderNodes
		, VertexBuffer & p_matrixBuffer
		, uint32_t & p_count )
	{
		constexpr uint32_t l_stride = 16 * sizeof( real );
		auto const l_count = std::min( p_matrixBuffer.GetSize() / l_stride, uint32_t( p_renderNodes.size() ) );
		auto l_buffer = p_matrixBuffer.data();
		auto l_it = p_renderNodes.begin();
		auto i = 0u;

		while ( i < l_count )
		{
			std::memcpy( l_buffer, l_it->m_sceneNode.GetDerivedTransformationMatrix().const_ptr(), l_stride );
			++p_count;
			l_buffer += l_stride;
			++i;
			++l_it;
		}

		p_matrixBuffer.Upload( 0u, l_stride * l_count, p_matrixBuffer.data() );
		return l_count;
	}

	void RenderPass::DoRenderInstancedSubmeshes( SubmeshStaticRenderNodesByPipelineMap & p_nodes
		, Camera const & p_camera
		, DepthMapArray & p_depthMaps )
	{
		DoTraverseNodes( *this
			, p_camera
			, p_nodes
			, p_depthMaps
			, [this]( RenderPipeline & p_pipeline
				, Pass & p_pass
				, Submesh & p_submesh
				, StaticGeometryRenderNodeArray & p_renderNodes
				, DepthMapArray & p_depthMaps )
			{
				if ( !p_renderNodes.empty() && p_submesh.HasMatrixBuffer() )
				{
					uint32_t l_count = DoCopyNodesMatrices( p_renderNodes, p_submesh.GetMatrixBuffer() );
					p_renderNodes[0].BindPass( p_depthMaps, MASK_MTXMODE_MODEL );
					p_submesh.DrawInstanced( p_renderNodes[0].m_buffers, l_count );
					p_renderNodes[0].UnbindPass( p_depthMaps );
				}
			} );
	}

	void RenderPass::DoRenderInstancedSubmeshes( SubmeshStaticRenderNodesByPipelineMap & p_nodes
		, DepthMapArray & p_depthMaps )
	{
		DoTraverseNodes( *this
			, p_nodes
			, p_depthMaps
			, [this]( RenderPipeline & p_pipeline
				, Pass & p_pass
				, Submesh & p_submesh
				, StaticGeometryRenderNodeArray & p_renderNodes
				, DepthMapArray & p_depthMaps )
			{
				if ( !p_renderNodes.empty() && p_submesh.HasMatrixBuffer() )
				{
					uint32_t l_count = DoCopyNodesMatrices( p_renderNodes, p_submesh.GetMatrixBuffer() );
					p_renderNodes[0].BindPass( p_depthMaps, MASK_MTXMODE_MODEL );
					p_submesh.DrawInstanced( p_renderNodes[0].m_buffers, l_count );
					p_renderNodes[0].UnbindPass( p_depthMaps );
				}
			} );
	}

	void RenderPass::DoRenderInstancedSubmeshes( SubmeshStaticRenderNodesByPipelineMap & p_nodes
		, Camera const & p_camera
		, DepthMapArray & p_depthMaps
		, uint32_t & p_count )
	{
		DoTraverseNodes( *this
			, p_camera
			, p_nodes
			, p_depthMaps
			, [&p_count, this]( RenderPipeline & p_pipeline
				, Pass & p_pass
				, Submesh & p_submesh
				, StaticGeometryRenderNodeArray & p_renderNodes
				, DepthMapArray & p_depthMaps )
			{
				if ( !p_renderNodes.empty() && p_submesh.HasMatrixBuffer() )
				{
					uint32_t l_count = DoCopyNodesMatrices( p_renderNodes, p_submesh.GetMatrixBuffer(), p_count );
					p_renderNodes[0].BindPass( p_depthMaps, MASK_MTXMODE_MODEL );
					p_submesh.DrawInstanced( p_renderNodes[0].m_buffers, l_count );
					p_renderNodes[0].UnbindPass( p_depthMaps );
				}
			} );
	}

	void RenderPass::DoRenderInstancedSubmeshes( SubmeshStaticRenderNodesByPipelineMap & p_nodes
		, DepthMapArray & p_depthMaps
		, uint32_t & p_count )
	{
		DoTraverseNodes( *this
			, p_nodes
			, p_depthMaps
			, [&p_count, this]( RenderPipeline & p_pipeline
				, Pass & p_pass
				, Submesh & p_submesh
				, StaticGeometryRenderNodeArray & p_renderNodes
				, DepthMapArray & p_depthMaps )
			{
				if ( !p_renderNodes.empty() && p_submesh.HasMatrixBuffer() )
				{
					uint32_t l_count = DoCopyNodesMatrices( p_renderNodes, p_submesh.GetMatrixBuffer(), p_count );
					p_renderNodes[0].BindPass( p_depthMaps, MASK_MTXMODE_MODEL );
					p_submesh.DrawInstanced( p_renderNodes[0].m_buffers, l_count );
					p_renderNodes[0].UnbindPass( p_depthMaps );
				}
			} );
	}

	void RenderPass::DoRenderStaticSubmeshes( StaticGeometryRenderNodesByPipelineMap & p_nodes
		, Camera const & p_camera
		, DepthMapArray & p_depthMaps )
	{
		DoRenderNonInstanced( *this
			, p_camera
			, p_nodes
			, p_depthMaps );
	}

	void RenderPass::DoRenderStaticSubmeshes( StaticGeometryRenderNodesByPipelineMap & p_nodes
		, DepthMapArray & p_depthMaps )
	{
		DoRenderNonInstanced( *this
			, p_nodes
			, p_depthMaps );
	}

	void RenderPass::DoRenderStaticSubmeshes( StaticGeometryRenderNodesByPipelineMap & p_nodes
		, Camera const & p_camera
		, DepthMapArray & p_depthMaps
		, uint32_t & p_count )
	{
		DoRenderNonInstanced( *this
			, p_camera
			, p_nodes
			, p_depthMaps
			, p_count );
	}

	void RenderPass::DoRenderStaticSubmeshes( StaticGeometryRenderNodesByPipelineMap & p_nodes
		, DepthMapArray & p_depthMaps
		, uint32_t & p_count )
	{
		DoRenderNonInstanced( *this
			, p_nodes
			, p_depthMaps
			, p_count );
	}

	void RenderPass::DoRenderAnimatedSubmeshes( AnimatedGeometryRenderNodesByPipelineMap & p_nodes
		, Camera const & p_camera
		, DepthMapArray & p_depthMaps )
	{
		DoRenderNonInstanced( *this
			, p_camera
			, p_nodes
			, p_depthMaps );
	}

	void RenderPass::DoRenderAnimatedSubmeshes( AnimatedGeometryRenderNodesByPipelineMap & p_nodes
		, DepthMapArray & p_depthMaps )
	{
		DoRenderNonInstanced( *this
			, p_nodes
			, p_depthMaps );
	}

	void RenderPass::DoRenderAnimatedSubmeshes( AnimatedGeometryRenderNodesByPipelineMap & p_nodes
		, Camera const & p_camera
		, DepthMapArray & p_depthMaps
		, uint32_t & p_count )
	{
		DoRenderNonInstanced( *this
			, p_camera
			, p_nodes
			, p_depthMaps
			, p_count );
	}

	void RenderPass::DoRenderAnimatedSubmeshes( AnimatedGeometryRenderNodesByPipelineMap & p_nodes
		, DepthMapArray & p_depthMaps
		, uint32_t & p_count )
	{
		DoRenderNonInstanced( *this
			, p_nodes
			, p_depthMaps
			, p_count );
	}

	void RenderPass::DoRenderBillboards( BillboardRenderNodesByPipelineMap & p_nodes
		, Camera const & p_camera
		, DepthMapArray & p_depthMaps )
	{
		DoRenderNonInstanced( *this
			, p_camera
			, p_nodes
			, p_depthMaps );
	}

	void RenderPass::DoRenderBillboards( BillboardRenderNodesByPipelineMap & p_nodes
		, DepthMapArray & p_depthMaps )
	{
		DoRenderNonInstanced( *this
			, p_nodes
			, p_depthMaps );
	}

	void RenderPass::DoRenderBillboards( BillboardRenderNodesByPipelineMap & p_nodes
		, Camera const & p_camera
		, DepthMapArray & p_depthMaps
		, uint32_t & p_count )
	{
		DoRenderNonInstanced( *this
			, p_camera
			, p_nodes
			, p_depthMaps
			, p_count );
	}

	void RenderPass::DoRenderBillboards( BillboardRenderNodesByPipelineMap & p_nodes
		, DepthMapArray & p_depthMaps
		, uint32_t & p_count )
	{
		DoRenderNonInstanced( *this
			, p_nodes
			, p_depthMaps
			, p_count );
	}

	String RenderPass::DoGetVertexShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, uint8_t p_sceneFlags
		, bool p_invertNormals )const
	{
		using namespace GLSL;
		auto l_writer = GetEngine()->GetRenderSystem()->CreateGlslWriter();
		// Vertex inputs
		auto position = l_writer.GetAttribute< Vec4 >( ShaderProgram::Position );
		auto normal = l_writer.GetAttribute< Vec3 >( ShaderProgram::Normal );
		auto tangent = l_writer.GetAttribute< Vec3 >( ShaderProgram::Tangent );
		auto bitangent = l_writer.GetAttribute< Vec3 >( ShaderProgram::Bitangent );
		auto texture = l_writer.GetAttribute< Vec3 >( ShaderProgram::Texture );
		auto bone_ids0 = l_writer.GetAttribute< IVec4 >( ShaderProgram::BoneIds0, CheckFlag( p_programFlags, ProgramFlag::eSkinning ) );
		auto bone_ids1 = l_writer.GetAttribute< IVec4 >( ShaderProgram::BoneIds1, CheckFlag( p_programFlags, ProgramFlag::eSkinning ) );
		auto weights0 = l_writer.GetAttribute< Vec4 >( ShaderProgram::Weights0, CheckFlag( p_programFlags, ProgramFlag::eSkinning ) );
		auto weights1 = l_writer.GetAttribute< Vec4 >( ShaderProgram::Weights1, CheckFlag( p_programFlags, ProgramFlag::eSkinning ) );
		auto transform = l_writer.GetAttribute< Mat4 >( ShaderProgram::Transform, CheckFlag( p_programFlags, ProgramFlag::eInstantiation ) );
		auto position2 = l_writer.GetAttribute< Vec4 >( ShaderProgram::Position2, CheckFlag( p_programFlags, ProgramFlag::eMorphing ) );
		auto normal2 = l_writer.GetAttribute< Vec3 >( ShaderProgram::Normal2, CheckFlag( p_programFlags, ProgramFlag::eMorphing ) );
		auto tangent2 = l_writer.GetAttribute< Vec3 >( ShaderProgram::Tangent2, CheckFlag( p_programFlags, ProgramFlag::eMorphing ) );
		auto bitangent2 = l_writer.GetAttribute< Vec3 >( ShaderProgram::Bitangent2, CheckFlag( p_programFlags, ProgramFlag::eMorphing ) );
		auto texture2 = l_writer.GetAttribute< Vec3 >( ShaderProgram::Texture2, CheckFlag( p_programFlags, ProgramFlag::eMorphing ) );
		auto gl_InstanceID( l_writer.GetBuiltin< Int >( cuT( "gl_InstanceID" ) ) );

		UBO_MATRIX( l_writer );
		UBO_ANIMATION( l_writer, p_programFlags );

		// Outputs
		auto vtx_worldSpacePosition = l_writer.GetOutput< Vec3 >( cuT( "vtx_worldSpacePosition" ) );
		auto vtx_normal = l_writer.GetOutput< Vec3 >( cuT( "vtx_normal" ) );
		auto vtx_tangent = l_writer.GetOutput< Vec3 >( cuT( "vtx_tangent" ) );
		auto vtx_bitangent = l_writer.GetOutput< Vec3 >( cuT( "vtx_bitangent" ) );
		auto vtx_texture = l_writer.GetOutput< Vec3 >( cuT( "vtx_texture" ) );
		auto vtx_instance = l_writer.GetOutput< Int >( cuT( "vtx_instance" ) );
		auto gl_Position = l_writer.GetBuiltin< Vec4 >( cuT( "gl_Position" ) );

		std::function< void() > l_main = [&]()
		{
			auto l_v4Vertex = l_writer.GetLocale( cuT( "l_v4Vertex" ), vec4( position.xyz(), 1.0 ) );
			auto l_v4Normal = l_writer.GetLocale( cuT( "l_v4Normal" ), vec4( normal, 0.0 ) );
			auto l_v4Tangent = l_writer.GetLocale( cuT( "l_v4Tangent" ), vec4( tangent, 0.0 ) );
			auto l_v4Bitangent = l_writer.GetLocale( cuT( "l_v4Bitangent" ), vec4( bitangent, 0.0 ) );
			auto l_v3Texture = l_writer.GetLocale( cuT( "l_v3Texture" ), texture );
			auto l_mtxModel = l_writer.GetLocale< Mat4 >( cuT( "l_mtxModel" ) );

			if ( CheckFlag( p_programFlags, ProgramFlag::eSkinning ) )
			{
				auto l_mtxBoneTransform = l_writer.GetLocale< Mat4 >( cuT( "l_mtxBoneTransform" ) );
				l_mtxBoneTransform = c3d_mtxBones[bone_ids0[0_i]] * weights0[0_i];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids0[1_i]] * weights0[1_i];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids0[2_i]] * weights0[2_i];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids0[3_i]] * weights0[3_i];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids1[0_i]] * weights1[0_i];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids1[1_i]] * weights1[1_i];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids1[2_i]] * weights1[2_i];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids1[3_i]] * weights1[3_i];
				l_mtxModel = c3d_mtxModel * l_mtxBoneTransform;
			}
			else if ( CheckFlag( p_programFlags, ProgramFlag::eInstantiation ) )
			{
				l_mtxModel = transform;
			}
			else
			{
				l_mtxModel = c3d_mtxModel;
			}

			if ( CheckFlag( p_programFlags, ProgramFlag::eMorphing ) )
			{
				auto l_time = l_writer.GetLocale( cuT( "l_time" ), 1.0_f - c3d_fTime );
				l_v4Vertex = vec4( l_v4Vertex.xyz() * l_time + position2.xyz() * c3d_fTime, 1.0 );
				l_v4Normal = vec4( l_v4Normal.xyz() * l_time + normal2.xyz() * c3d_fTime, 1.0 );
				l_v4Tangent = vec4( l_v4Tangent.xyz() * l_time + tangent2.xyz() * c3d_fTime, 1.0 );
				l_v4Bitangent = vec4( l_v4Bitangent.xyz() * l_time + bitangent2.xyz() * c3d_fTime, 1.0 );
				l_v3Texture = l_v3Texture * l_writer.Paren( 1.0_f - c3d_fTime ) + texture2 * c3d_fTime;
			}

			vtx_texture = l_v3Texture;
			l_v4Vertex = l_mtxModel * l_v4Vertex;
			vtx_worldSpacePosition = l_v4Vertex.xyz();
			l_v4Vertex = c3d_mtxView * l_v4Vertex;

			if ( p_invertNormals )
			{
				vtx_normal = normalize( l_writer.Paren( l_mtxModel * -l_v4Normal ).xyz() );
			}
			else
			{
				vtx_normal = normalize( l_writer.Paren( l_mtxModel * l_v4Normal ).xyz() );
			}

			vtx_tangent = normalize( l_writer.Paren( l_mtxModel * l_v4Tangent ).xyz() );
			vtx_bitangent = normalize( l_writer.Paren( l_mtxModel * l_v4Bitangent ).xyz() );
			vtx_instance = gl_InstanceID;
			gl_Position = c3d_mtxProjection * l_v4Vertex;
		};

		l_writer.ImplementFunction< void >( cuT( "main" ), l_main );
		return l_writer.Finalise();
	}
}
