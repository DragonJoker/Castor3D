#include "RenderPass.hpp"

#include "Engine.hpp"

#include "Mesh/Submesh.hpp"
#include "Mesh/Buffer/VertexBuffer.hpp"
#include "Scene/SceneNode.hpp"

using namespace Castor;

namespace Castor3D
{
	namespace
	{
		template< typename MapType, typename FuncType >
		void DoTraverseNodes( Camera const & p_camera, MapType & p_nodes, FuncType p_function )
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
		void DoRenderNonInstanced( Scene & p_scene
								   , Camera const & p_camera
								   , MapType & p_nodes
								   , bool p_register
								   , std::vector< std::reference_wrapper< ObjectRenderNodeBase const > > & p_renderedObjects )
		{
			for ( auto l_itPipelines : p_nodes )
			{
				l_itPipelines.first->SetProjectionMatrix( p_camera.GetViewport().GetProjection() );
				l_itPipelines.first->SetViewMatrix( p_camera.GetView() );
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

	void RenderPass::Update()
	{
		for ( auto & l_it : m_scenes )
		{
			for ( auto & l_camera : l_it.second )
			{
				m_renderQueue.Prepare( *l_camera, *l_it.first );
			}
		}
	}

	void RenderPass::AddScene( Scene & p_scene, Camera & p_camera )
	{
		m_scenes.insert( { &p_scene, std::vector< CameraRPtr >{} } ).first->second.push_back( &p_camera );
		m_renderQueue.AddScene( p_scene );
	}

	String RenderPass::GetPixelShaderSource( uint16_t p_textureFlags, uint8_t p_programFlags )const
	{
		String l_return;

		if ( CheckFlag( p_programFlags, ProgramFlag::AlphaBlending ) )
		{
			l_return = DoGetTransparentPixelShaderSource( p_textureFlags, p_programFlags );
		}
		else
		{
			l_return = DoGetOpaquePixelShaderSource( p_textureFlags, p_programFlags );
		}

		return l_return;
	}

	void RenderPass::PreparePipeline( uint16_t p_textureFlags, uint8_t p_programFlags, BlendMode p_colourBlendMode, BlendMode p_alphaBlendMode )
	{
		ShaderProgramSPtr l_program;

		if ( CheckFlag( p_programFlags, ProgramFlag::Billboards ) )
		{
			l_program = GetEngine()->GetShaderProgramCache().GetBillboardProgram( p_textureFlags, p_programFlags );

			if ( !l_program )
			{
				l_program = GetEngine()->GetRenderSystem()->CreateBillboardsProgram( *this, p_textureFlags, p_programFlags );
				GetEngine()->GetShaderProgramCache().AddBillboardProgram( l_program, p_textureFlags, p_programFlags );
			}
		}
		else
		{
			l_program = GetEngine()->GetShaderProgramCache().GetAutomaticProgram( *this, p_textureFlags, p_programFlags );
		}

		if ( CheckFlag( p_programFlags, ProgramFlag::AlphaBlending ) )
		{
			DoPrepareTransparentFrontPipeline( *l_program, { p_textureFlags, p_programFlags, p_colourBlendMode, p_alphaBlendMode } );
			DoPrepareTransparentBackPipeline( *l_program, { p_textureFlags, p_programFlags, p_colourBlendMode, p_alphaBlendMode } );
		}
		else
		{
			DoPrepareOpaquePipeline( *l_program, { p_textureFlags, p_programFlags, p_colourBlendMode, BlendMode::NoBlend } );
		}
	}

	Pipeline & RenderPass::GetOpaquePipeline( uint16_t p_textureFlags, uint8_t p_programFlags, BlendMode p_colourBlendMode )
	{
		auto l_it = m_opaquePipelines.find( { p_textureFlags, p_programFlags, p_colourBlendMode, BlendMode::NoBlend } );
		REQUIRE( l_it != m_opaquePipelines.end() );
		return *l_it->second;
	}

	Pipeline & RenderPass::GetTransparentPipelineFront( uint16_t p_textureFlags, uint8_t p_programFlags, BlendMode p_colourBlendMode, BlendMode p_alphaBlendMode )
	{
		auto l_it = m_frontTransparentPipelines.find( { p_textureFlags, p_programFlags, p_colourBlendMode, p_alphaBlendMode } );
		REQUIRE( l_it != m_frontTransparentPipelines.end() );
		return *l_it->second;
	}

	Pipeline & RenderPass::GetTransparentPipelineBack( uint16_t p_textureFlags, uint8_t p_programFlags, BlendMode p_colourBlendMode, BlendMode p_alphaBlendMode )
	{
		auto l_it = m_backTransparentPipelines.find( { p_textureFlags, p_programFlags, p_colourBlendMode, p_alphaBlendMode } );
		REQUIRE( l_it != m_backTransparentPipelines.end() );
		return *l_it->second;
	}

	void RenderPass::DoRenderStaticSubmeshesNonInstanced( Scene & p_scene, Camera const & p_camera, StaticGeometryRenderNodesByPipelineMap & p_nodes, bool p_register )
	{
		DoRenderNonInstanced( p_scene, p_camera, p_nodes, p_register, m_renderedObjects );
	}

	void RenderPass::DoRenderAnimatedSubmeshesNonInstanced( Scene & p_scene, Camera const & p_camera, AnimatedGeometryRenderNodesByPipelineMap & p_nodes, bool p_register )
	{
		DoRenderNonInstanced( p_scene, p_camera, p_nodes, p_register, m_renderedObjects );
	}

	void RenderPass::DoRenderInstancedSubmeshesNonInstanced( Scene & p_scene, Camera const & p_camera, Pipeline & p_pipeline, SubmeshStaticRenderNodesByPipelineMap & p_nodes, bool p_register )
	{
		DoTraverseNodes( p_camera, p_nodes, [&p_scene, &p_camera, &p_pipeline, &p_register, this]( Pipeline & p_pipeline, Pass & p_pass, Submesh & p_submesh, StaticGeometryRenderNodeArray & p_renderNodes )
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

	void RenderPass::DoRenderInstancedSubmeshesInstanced( Scene & p_scene, Camera const & p_camera, Pipeline & p_pipeline, SubmeshStaticRenderNodesByPipelineMap & p_nodes, bool p_register )
	{
		DoTraverseNodes( p_camera, p_nodes, [&p_scene, &p_camera, &p_pipeline, &p_register, this]( Pipeline & p_pipeline, Pass & p_pass, Submesh & p_submesh, StaticGeometryRenderNodeArray & p_renderNodes )
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
			l_it.second.get().m_scene.m_node.m_pipeline.Apply();
			l_it.second.get().Render( p_scene, p_camera );

			if ( p_register )
			{
				m_renderedObjects.push_back( l_it.second.get() );
			}
		}
	}

	void RenderPass::DoRenderBillboards( Scene & p_scene, Camera const & p_camera, BillboardRenderNodesByPipelineMap & p_nodes, bool p_register )
	{
		DoRenderNonInstanced( p_scene, p_camera, p_nodes, p_register, m_renderedObjects );
	}
}
