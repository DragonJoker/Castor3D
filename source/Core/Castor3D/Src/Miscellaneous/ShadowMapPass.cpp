#include "ShadowMapPass.hpp"

#include "Engine.hpp"
#include "Cache/ShaderCache.hpp"

#include "FrameBuffer/ColourRenderBuffer.hpp"
#include "FrameBuffer/DepthStencilRenderBuffer.hpp"
#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/RenderBufferAttachment.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "Mesh/Submesh.hpp"
#include "Mesh/Buffer/BufferElementGroup.hpp"
#include "Mesh/Buffer/GeometryBuffers.hpp"
#include "Mesh/Buffer/VertexBuffer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Render/RenderTarget.hpp"
#include "Scene/BillboardList.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Geometry.hpp"
#include "Scene/Light/Light.hpp"
#include "Scene/Light/DirectionalLight.hpp"
#include "Scene/Light/SpotLight.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Shader/FrameVariableBuffer.hpp"
#include "State/DepthStencilState.hpp"
#include "State/MultisampleState.hpp"
#include "State/RasteriserState.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <GlslSource.hpp>

#include <Graphics/Image.hpp>

using namespace Castor;

namespace Castor3D
{
	namespace
	{
		static String const ShadowMap = cuT( "ShadowMap" );

		template< typename MapType, typename FuncType >
		void DoTraverseNodes(
			Camera const & p_camera,
			MapType & p_nodes,
			FuncType p_function )
		{
			for ( auto l_itPipelines : p_nodes )
			{
				l_itPipelines.first->SetProjectionMatrix( p_camera.GetViewport().GetProjection() );
				l_itPipelines.first->SetViewMatrix( p_camera.GetView() );
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

		template< typename MapType >
		void DoRenderNonInstanced(
			Scene & p_scene,
			Camera const & p_camera,
			MapType & p_nodes )
		{
			auto l_depthMaps = DepthMapArray{};

			for ( auto l_itPipelines : p_nodes )
			{
				l_itPipelines.first->SetProjectionMatrix( p_camera.GetViewport().GetProjection() );
				l_itPipelines.first->SetViewMatrix( p_camera.GetView() );
				l_itPipelines.first->Apply();

				for ( auto & l_renderNode : l_itPipelines.second )
				{
					if ( l_renderNode.m_data.IsInitialised() )
					{
						l_renderNode.Render( l_depthMaps );
					}
				}
			}
		}
	}

	ShadowMapPass::ShadowMapPass( Engine & p_engine, Scene & p_scene, Light & p_light, TextureUnit & p_shadowMap, uint32_t p_index )
		: RenderPass{ ShadowMap, p_engine, true }
		, m_light{ p_light }
		, m_scene{ p_scene }
		, m_shadowMap{ p_shadowMap }
		, m_index{ p_index }
	{
	}

	ShadowMapPass::~ShadowMapPass()
	{
	}

	bool ShadowMapPass::Initialise()
	{
		bool l_return = true;
		auto l_size = m_shadowMap.GetTexture()->GetDimensions();

		if ( !m_frameBuffer )
		{
			m_frameBuffer = GetEngine()->GetRenderSystem()->CreateFrameBuffer();
			l_return = m_frameBuffer->Create();

			if ( l_return )
			{
				l_return = m_frameBuffer->Initialise( l_size );
			}

			if ( l_return )
			{
				l_return = DoInitialise( l_size );
			}
		}

		return l_return;
	}

	void ShadowMapPass::Cleanup()
	{
		if ( m_frameBuffer )
		{
			DoCleanup();

			m_frameBuffer->Cleanup();
			m_frameBuffer->Destroy();
			m_frameBuffer.reset();
		}

		for ( auto l_buffer : m_geometryBuffers )
		{
			l_buffer->Cleanup();
		}

		m_geometryBuffers.clear();
	}

	void ShadowMapPass::Update()
	{
		DoUpdate();
	}

	void ShadowMapPass::Render()
	{
		DoRender();
	}

	AnimatedGeometryRenderNode ShadowMapPass::CreateAnimatedNode( Pass & p_pass
		, RenderPipeline & p_pipeline
		, Submesh & p_submesh
		, Geometry & p_primitive
		, AnimatedSkeletonSPtr p_skeleton
		, AnimatedMeshSPtr p_mesh )
	{
		auto l_modelBuffer = p_pipeline.GetProgram().FindFrameVariableBuffer( ShaderProgram::BufferModel );
		OneIntFrameVariableSPtr l_receiver;
		auto l_animationBuffer = p_pipeline.GetProgram().FindFrameVariableBuffer( ShaderProgram::BufferAnimation );
		auto l_buffer = p_submesh.GetGeometryBuffers( p_pipeline.GetProgram() );
		m_geometryBuffers.insert( l_buffer );

		return AnimatedGeometryRenderNode
		{
			DoCreateSceneRenderNode( *p_primitive.GetScene(), p_pipeline ),
			DoCreatePassRenderNode( p_pass, p_pipeline ),
			*l_buffer,
			*p_primitive.GetParent(),
			*l_modelBuffer->GetVariable( ShaderProgram::ShadowReceiver, l_receiver ),
			p_submesh,
			p_primitive,
			p_skeleton.get(),
			p_mesh.get(),
			*l_animationBuffer
		};
	}

	StaticGeometryRenderNode ShadowMapPass::CreateStaticNode( Pass & p_pass
		, RenderPipeline & p_pipeline
		, Submesh & p_submesh
		, Geometry & p_primitive )
	{
		auto l_modelBuffer = p_pipeline.GetProgram().FindFrameVariableBuffer( ShaderProgram::BufferModel );
		OneIntFrameVariableSPtr l_receiver;
		auto l_buffer = p_submesh.GetGeometryBuffers( p_pipeline.GetProgram() );
		m_geometryBuffers.insert( l_buffer );

		return StaticGeometryRenderNode
		{
			DoCreateSceneRenderNode( *p_primitive.GetScene(), p_pipeline ),
			DoCreatePassRenderNode( p_pass, p_pipeline ),
			*l_buffer,
			*p_primitive.GetParent(),
			*l_modelBuffer->GetVariable( ShaderProgram::ShadowReceiver, l_receiver ),
			p_submesh,
			p_primitive,
		};
	}

	BillboardRenderNode ShadowMapPass::CreateBillboardNode( Pass & p_pass
		, RenderPipeline & p_pipeline
		, BillboardBase & p_billboard )
	{
		auto l_modelBuffer = p_pipeline.GetProgram().FindFrameVariableBuffer( ShaderProgram::BufferModel );
		OneIntFrameVariableSPtr l_receiver;
		auto l_billboardBuffer = p_pipeline.GetProgram().FindFrameVariableBuffer( ShaderProgram::BufferBillboards );
		Point2iFrameVariableSPtr l_pt2i;
		auto l_buffer = p_billboard.GetGeometryBuffers( p_pipeline.GetProgram() );
		m_geometryBuffers.insert( l_buffer );

		return BillboardRenderNode
		{
			DoCreateSceneRenderNode( p_billboard.GetParentScene(), p_pipeline ),
			DoCreatePassRenderNode( p_pass, p_pipeline ),
			*l_buffer,
			*p_billboard.GetNode(),
			*l_modelBuffer->GetVariable( ShaderProgram::ShadowReceiver, l_receiver ),
			p_billboard,
			*l_billboardBuffer,
			*l_billboardBuffer->GetVariable( ShaderProgram::Dimensions, l_pt2i ),
			*l_billboardBuffer->GetVariable( ShaderProgram::WindowSize, l_pt2i )
		};
	}

	void ShadowMapPass::DoRenderNodes( SceneRenderNodes & p_nodes
		, Camera const & p_camera )
	{
		DoRenderInstancedSubmeshes( p_nodes.m_scene, p_camera, p_nodes.m_instancedGeometries.m_backCulled );
		DoRenderStaticSubmeshes( p_nodes.m_scene, p_camera, p_nodes.m_staticGeometries.m_backCulled );
		DoRenderAnimatedSubmeshes( p_nodes.m_scene, p_camera, p_nodes.m_animatedGeometries.m_backCulled );
		DoRenderBillboards( p_nodes.m_scene, p_camera, p_nodes.m_billboards.m_backCulled );
	}

	void ShadowMapPass::DoRenderInstancedSubmeshes( Scene & p_scene
		, Camera const & p_camera
		, SubmeshStaticRenderNodesByPipelineMap & p_nodes )
	{
		DoTraverseNodes( p_camera
			, p_nodes
			, [this]( RenderPipeline & p_pipeline
				, Pass & p_pass
				, Submesh & p_submesh
				, StaticGeometryRenderNodeArray & p_renderNodes )
			{
				if ( !p_renderNodes.empty() && p_submesh.HasMatrixBuffer() )
				{
					auto l_count = DoCopyNodesMatrices( p_renderNodes, p_submesh.GetMatrixBuffer() );
					auto l_depthMaps = DepthMapArray{};
					p_renderNodes[0].BindPass( l_depthMaps, MASK_MTXMODE_MODEL );
					p_submesh.DrawInstanced( p_renderNodes[0].m_buffers, l_count );
					p_renderNodes[0].UnbindPass( l_depthMaps );
				}
			} );
	}

	void ShadowMapPass::DoRenderStaticSubmeshes( Scene & p_scene
		, Camera const & p_camera
		, StaticGeometryRenderNodesByPipelineMap & p_nodes )
	{
		DoRenderNonInstanced( p_scene
			, p_camera
			, p_nodes );
	}

	void ShadowMapPass::DoRenderAnimatedSubmeshes( Scene & p_scene
		, Camera const & p_camera
		, AnimatedGeometryRenderNodesByPipelineMap & p_nodes )
	{
		DoRenderNonInstanced( p_scene
			, p_camera
			, p_nodes );
	}

	void ShadowMapPass::DoRenderBillboards( Scene & p_scene
		, Camera const & p_camera
		, BillboardRenderNodesByPipelineMap & p_nodes )
	{
		DoRenderNonInstanced( p_scene
			, p_camera
			, p_nodes );
	}

	void ShadowMapPass::DoUpdatePipeline( RenderPipeline & p_pipeline
		, DepthMapArray & p_depthMaps )const
	{
	}

	void ShadowMapPass::DoPrepareFrontPipeline( ShaderProgram & p_program
		, PipelineFlags const & p_flags )
	{
	}

	void ShadowMapPass::DoPrepareBackPipeline( ShaderProgram & p_program
		, PipelineFlags const & p_flags )
	{
		auto l_it = m_backPipelines.find( p_flags );

		if ( l_it == m_backPipelines.end() )
		{
			DoUpdateProgram( p_program );
			RasteriserState l_rsState;
			l_rsState.SetCulledFaces( Culling::eNone );
			l_it = m_backPipelines.emplace( p_flags
				, GetEngine()->GetRenderSystem()->CreateRenderPipeline( DepthStencilState{}
					, std::move( l_rsState )
					, BlendState{}
					, MultisampleState{}
					, p_program
					, p_flags ) ).first;
		}
	}

	void ShadowMapPass::DoUpdateFlags( TextureChannels & p_textureFlags
		, ProgramFlags & p_programFlags )const
	{
		RemFlag( p_programFlags, ProgramFlag::eLighting );
		RemFlag( p_programFlags, ProgramFlag::eAlphaBlending );
		RemFlag( p_textureFlags, TextureChannel::eAll );

		AddFlag( p_programFlags, ProgramFlag::eShadowMap );
	}
}
