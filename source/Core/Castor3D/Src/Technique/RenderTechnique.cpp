#if defined( _MSC_VER )
#	pragma warning( disable:4503 )
#endif

#include "RenderTechnique.hpp"

#include "AnimatedObjectGroupCache.hpp"
#include "BillboardCache.hpp"
#include "CameraCache.hpp"
#include "Engine.hpp"
#include "GeometryCache.hpp"
#include "LightCache.hpp"
#include "OverlayCache.hpp"
#include "SamplerCache.hpp"
#include "ShaderCache.hpp"

#include "FrameBuffer/ColourRenderBuffer.hpp"
#include "FrameBuffer/DepthStencilRenderBuffer.hpp"
#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/RenderBufferAttachment.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "Material/Material.hpp"
#include "Material/Pass.hpp"
#include "Mesh/Mesh.hpp"
#include "Mesh/Submesh.hpp"
#include "Mesh/Buffer/GeometryBuffers.hpp"
#include "Mesh/Buffer/VertexBuffer.hpp"
#include "Mesh/Skeleton/Skeleton.hpp"
#include "PostEffect/PostEffect.hpp"
#include "Render/Context.hpp"
#include "Render/Pipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Render/RenderTarget.hpp"
#include "Scene/BillboardList.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Geometry.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Animation/AnimatedMesh.hpp"
#include "Scene/Animation/AnimatedObjectGroup.hpp"
#include "Scene/Animation/AnimatedSkeleton.hpp"
#include "Scene/Animation/Mesh/MeshAnimationInstance.hpp"
#include "Scene/Animation/Mesh/MeshAnimationInstanceSubmesh.hpp"
#include "Shader/FrameVariableBuffer.hpp"
#include "Shader/ShaderProgram.hpp"
#include "State/DepthStencilState.hpp"
#include "State/MultisampleState.hpp"
#include "State/RasteriserState.hpp"
#include "Texture/TextureLayout.hpp"

#include <GlslSource.hpp>

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

		AnimatedObjectSPtr DoFindAnimatedObject( Scene & p_scene, String const & p_name )
		{
			AnimatedObjectSPtr l_return;
			auto & l_cache = p_scene.GetAnimatedObjectGroupCache();
			auto l_lock = make_unique_lock( l_cache );

			for ( auto l_group : l_cache )
			{
				if ( !l_return )
				{
					auto l_it = l_group.second->GetObjects().find( p_name );

					if ( l_it != l_group.second->GetObjects().end() )
					{
						l_return = l_it->second;
					}
				}
			}

			return l_return;
		}

		template< typename MapType >
		void DoSortAlpha( MapType & p_input
						  , Camera const & p_camera
						  , RenderTechnique::DistanceSortedNodeMap & p_output )
		{
			for ( auto & l_itPipelines : p_input )
			{
				l_itPipelines.first->SetProjectionMatrix( p_camera.GetViewport().GetProjection() );
				l_itPipelines.first->SetViewMatrix( p_camera.GetView() );

				for ( auto & l_renderNode : l_itPipelines.second )
				{
					Matrix4x4r l_mtxMeshGlobal = l_renderNode.m_sceneNode.GetDerivedTransformationMatrix().get_inverse().transpose();
					Point3r l_position = p_camera.GetParent()->GetDerivedPosition();
					Point3r l_ptCameraLocal = l_position * l_mtxMeshGlobal;
					l_renderNode.m_data.SortByDistance( l_ptCameraLocal );
					l_ptCameraLocal -= l_renderNode.m_sceneNode.GetPosition();
					p_output.insert( { point::distance_squared( l_ptCameraLocal ), l_renderNode } );
				}
			}
		}

		BlendState DoCreateBlendState( BlendMode p_colourBlendMode, BlendMode p_alphaBlendMode )
		{
			BlendState l_state;
			bool l_blend = false;

			switch ( p_colourBlendMode )
			{
			case BlendMode::NoBlend:
				l_state.SetRgbSrcBlend( BlendOperand::One );
				l_state.SetRgbDstBlend( BlendOperand::Zero );
				break;

			case BlendMode::Additive:
				l_blend = true;
				l_state.SetRgbSrcBlend( BlendOperand::One );
				l_state.SetRgbDstBlend( BlendOperand::One );
				break;

			case BlendMode::Multiplicative:
				l_blend = true;
				l_state.SetRgbSrcBlend( BlendOperand::Zero );
				l_state.SetRgbDstBlend( BlendOperand::InvSrcColour );
				break;

			case BlendMode::Interpolative:
				l_blend = true;
				l_state.SetRgbSrcBlend( BlendOperand::SrcColour );
				l_state.SetRgbDstBlend( BlendOperand::InvSrcColour );
				break;

			default:
				l_blend = true;
				l_state.SetRgbSrcBlend( BlendOperand::SrcColour );
				l_state.SetRgbDstBlend( BlendOperand::InvSrcColour );
				break;
			}

			switch ( p_alphaBlendMode )
			{
			case BlendMode::NoBlend:
				l_blend = true;
				l_state.SetAlphaSrcBlend( BlendOperand::One );
				l_state.SetAlphaDstBlend( BlendOperand::Zero );
				break;

			case BlendMode::Additive:
				l_blend = true;
				l_state.SetAlphaSrcBlend( BlendOperand::One );
				l_state.SetAlphaDstBlend( BlendOperand::One );
				break;

			case BlendMode::Multiplicative:
				l_blend = true;
				l_state.SetAlphaSrcBlend( BlendOperand::Zero );
				l_state.SetAlphaDstBlend( BlendOperand::InvSrcAlpha );
				l_state.SetRgbSrcBlend( BlendOperand::Zero );
				l_state.SetRgbDstBlend( BlendOperand::InvSrcAlpha );
				break;

			case BlendMode::Interpolative:
				l_blend = true;
				l_state.SetAlphaSrcBlend( BlendOperand::SrcAlpha );
				l_state.SetAlphaDstBlend( BlendOperand::InvSrcAlpha );
				l_state.SetRgbSrcBlend( BlendOperand::SrcAlpha );
				l_state.SetRgbDstBlend( BlendOperand::InvSrcAlpha );
				break;

			default:
				l_blend = true;
				l_state.SetAlphaSrcBlend( BlendOperand::SrcAlpha );
				l_state.SetAlphaDstBlend( BlendOperand::InvSrcAlpha );
				l_state.SetRgbSrcBlend( BlendOperand::SrcAlpha );
				l_state.SetRgbDstBlend( BlendOperand::InvSrcAlpha );
				break;
			}

			l_state.EnableBlend( l_blend );
			return l_state;
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

	//*************************************************************************************************

	RenderTechnique::stFRAME_BUFFER::stFRAME_BUFFER( RenderTechnique & p_technique )
		: m_technique{ p_technique }
	{
	}

	bool RenderTechnique::stFRAME_BUFFER::Initialise( Size p_size )
	{
		m_colourTexture = m_technique.GetEngine()->GetRenderSystem()->CreateTexture( TextureType::TwoDimensions, AccessType::Read, AccessType::Read | AccessType::Write );
		m_colourTexture->GetImage().SetSource( p_size, PixelFormat::RGBA16F32F );
		p_size = m_colourTexture->GetImage().GetDimensions();

		bool l_return = m_colourTexture->Create();

		if ( l_return )
		{
			l_return = m_colourTexture->Initialise();

			if ( !l_return )
			{
				m_colourTexture->Destroy();
			}
		}

		if ( l_return )
		{
			m_frameBuffer = m_technique.GetEngine()->GetRenderSystem()->CreateFrameBuffer();
			m_depthBuffer = m_frameBuffer->CreateDepthStencilRenderBuffer( PixelFormat::D32F );
			l_return = m_depthBuffer->Create();
		}

		if ( l_return )
		{
			l_return = m_depthBuffer->Initialise( p_size );

			if ( !l_return )
			{
				m_depthBuffer->Destroy();
			}
		}

		if ( l_return )
		{
			m_colourAttach = m_frameBuffer->CreateAttachment( m_colourTexture );
			m_depthAttach = m_frameBuffer->CreateAttachment( m_depthBuffer );
			l_return = m_frameBuffer->Create();
		}

		if ( l_return )
		{
			l_return = m_frameBuffer->Initialise( p_size );

			if ( l_return && m_frameBuffer->Bind( FrameBufferMode::Config ) )
			{
				m_frameBuffer->Attach( AttachmentPoint::Colour, 0, m_colourAttach, m_colourTexture->GetType() );
				m_frameBuffer->Attach( AttachmentPoint::Depth, m_depthAttach );
				l_return = m_frameBuffer->IsComplete();
				m_frameBuffer->Unbind();
			}
			else
			{
				m_frameBuffer->Destroy();
			}
		}

		return l_return;
	}

	void RenderTechnique::stFRAME_BUFFER::Cleanup()
	{
		if ( m_frameBuffer )
		{
			m_frameBuffer->Bind( FrameBufferMode::Config );
			m_frameBuffer->DetachAll();
			m_frameBuffer->Unbind();
			m_frameBuffer->Cleanup();
			m_colourTexture->Cleanup();
			m_depthBuffer->Cleanup();

			m_colourTexture->Destroy();
			m_depthBuffer->Destroy();
			m_frameBuffer->Destroy();

			m_depthAttach.reset();
			m_depthBuffer.reset();
			m_colourAttach.reset();
			m_colourTexture.reset();
			m_frameBuffer.reset();
		}
	}

	//*************************************************************************************************

	RenderTechnique::RenderTechnique( String const & p_name, RenderTarget & p_renderTarget, RenderSystem & p_renderSystem, Parameters const & CU_PARAM_UNUSED( p_params ), bool p_multisampling )
		: OwnedBy< Engine >{ *p_renderSystem.GetEngine() }
		, Named{ p_name }
		, m_renderTarget{ p_renderTarget }
		, m_renderSystem{ p_renderSystem }
		, m_initialised{ false }
		, m_frameBuffer{ *this }
		, m_multisampling{ p_multisampling }
		, m_renderQueue{ *this }
	{
	}

	RenderTechnique::~RenderTechnique()
	{
	}

	bool RenderTechnique::Create()
	{
		return DoCreate();
	}

	void RenderTechnique::Destroy()
	{
		DoDestroy();
	}

	bool RenderTechnique::Initialise( uint32_t & p_index )
	{
		if ( !m_initialised )
		{
			m_size = m_renderTarget.GetSize();
			m_initialised = DoInitialise( p_index );

			if ( m_initialised )
			{
				m_initialised = m_frameBuffer.Initialise( m_size );
			}
		}

		return m_initialised;
	}

	void RenderTechnique::Cleanup()
	{
		m_initialised = false;
		m_frameBuffer.Cleanup();
		DoCleanup();
	}

	void RenderTechnique::Update()
	{
		for ( auto & l_it : m_scenes )
		{
			for ( auto & l_camera : l_it.second )
			{
				m_renderQueue.Prepare( *l_camera, *l_it.first );
			}
		}
	}

	void RenderTechnique::AddScene( Scene & p_scene, Camera & p_camera )
	{
		m_scenes.insert( { &p_scene, std::vector< CameraRPtr >{} } ).first->second.push_back( &p_camera );
		m_renderQueue.AddScene( p_scene );
	}

	void RenderTechnique::Render( Scene & p_scene, Camera & p_camera, uint32_t p_frameTime, uint32_t & p_visible )
	{
		auto & l_nodes = m_renderQueue.GetRenderNodes( p_camera, p_scene );

		m_renderSystem.PushScene( &p_scene );

		if ( DoBeginRender( p_scene, p_camera ) )
		{
			p_scene.RenderBackground( GetSize() );
			DoRender( m_size, l_nodes, p_camera, p_frameTime );
			p_visible = uint32_t( m_renderedObjects.size() );
			DoEndRender( p_scene, p_camera );
		}

		for ( auto l_effect : m_renderTarget.GetPostEffects() )
		{
			l_effect->Apply( *m_frameBuffer.m_frameBuffer );
		}

		m_renderSystem.PopScene();
	}

	String RenderTechnique::GetPixelShaderSource( uint16_t p_textureFlags, uint8_t p_programFlags )const
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

	void RenderTechnique::PreparePipeline( uint16_t p_textureFlags, uint8_t p_programFlags, BlendMode p_colourBlendMode, BlendMode p_alphaBlendMode )
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

	Pipeline & RenderTechnique::GetOpaquePipeline( uint16_t p_textureFlags, uint8_t p_programFlags, BlendMode p_colourBlendMode )
	{
		auto l_it = m_opaquePipelines.find( { p_textureFlags, p_programFlags, p_colourBlendMode, BlendMode::NoBlend } );
		REQUIRE( l_it != m_opaquePipelines.end() );
		return *l_it->second;
	}

	Pipeline & RenderTechnique::GetTransparentPipelineFront( uint16_t p_textureFlags, uint8_t p_programFlags, BlendMode p_colourBlendMode, BlendMode p_alphaBlendMode )
	{
		auto l_it = m_frontTransparentPipelines.find( { p_textureFlags, p_programFlags, p_colourBlendMode, p_alphaBlendMode } );
		REQUIRE( l_it != m_frontTransparentPipelines.end() );
		return *l_it->second;
	}

	Pipeline & RenderTechnique::GetTransparentPipelineBack( uint16_t p_textureFlags, uint8_t p_programFlags, BlendMode p_colourBlendMode, BlendMode p_alphaBlendMode )
	{
		auto l_it = m_backTransparentPipelines.find( { p_textureFlags, p_programFlags, p_colourBlendMode, p_alphaBlendMode } );
		REQUIRE( l_it != m_backTransparentPipelines.end() );
		return *l_it->second;
	}

	bool RenderTechnique::WriteInto( Castor::TextFile & p_file )
	{
		return DoWriteInto( p_file );
	}

	void RenderTechnique::DoRenderStaticSubmeshesNonInstanced( Scene & p_scene, Camera const & p_camera, StaticGeometryRenderNodesByPipelineMap & p_nodes, bool p_register )
	{
		DoRenderNonInstanced( p_scene, p_camera, p_nodes, p_register, m_renderedObjects );
	}

	void RenderTechnique::DoRenderAnimatedSubmeshesNonInstanced( Scene & p_scene, Camera const & p_camera, AnimatedGeometryRenderNodesByPipelineMap & p_nodes, bool p_register )
	{
		DoRenderNonInstanced( p_scene, p_camera, p_nodes, p_register, m_renderedObjects );
	}

	void RenderTechnique::DoRenderBillboards( Scene & p_scene, Camera const & p_camera, BillboardRenderNodesByPipelineMap & p_nodes, bool p_register )
	{
		DoRenderNonInstanced( p_scene, p_camera, p_nodes, p_register, m_renderedObjects );
	}

	void RenderTechnique::DoRenderInstancedSubmeshesNonInstanced( Scene & p_scene, Camera const & p_camera, SubmeshStaticRenderNodesByPipelineMap & p_nodes, bool p_register )
	{
		DoTraverseNodes( p_camera, p_nodes, [&p_scene, &p_camera, &p_register, this]( Pipeline & p_pipeline, Pass & p_pass, Submesh & p_submesh, StaticGeometryRenderNodeArray & p_renderNodes )
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

	void RenderTechnique::DoRenderInstancedSubmeshesInstanced( Scene & p_scene, Camera const & p_camera, SubmeshStaticRenderNodesByPipelineMap & p_nodes, bool p_register )
	{
		DoTraverseNodes( p_camera, p_nodes, [&p_scene, &p_camera, &p_register, this]( Pipeline & p_pipeline, Pass & p_pass, Submesh & p_submesh, StaticGeometryRenderNodeArray & p_renderNodes )
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

	void RenderTechnique::DoRenderByDistance( Scene & p_scene, Camera const & p_camera, DistanceSortedNodeMap & p_nodes, bool p_register )
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

	void RenderTechnique::DoRender( Size const & p_size, SceneRenderNodes & p_nodes, Camera & p_camera, uint32_t p_frameTime )
	{
		m_renderedObjects.clear();
		p_camera.GetViewport().Resize( p_size );
		p_camera.Update();
		DoRenderOpaqueNodes( p_nodes, p_camera );
		p_nodes.m_scene.RenderForeground( p_size, p_camera );
		DoRenderTransparentNodes( p_nodes, p_camera );
	}

	void RenderTechnique::DoRenderOpaqueNodes( SceneRenderNodes & p_nodes, Camera const & p_camera )
	{
		if ( !p_nodes.m_staticGeometries.m_opaqueRenderNodes.empty()
			 || !p_nodes.m_instancedGeometries.m_opaqueRenderNodes.empty()
			 || !p_nodes.m_animatedGeometries.m_opaqueRenderNodes.empty()
			 || !p_nodes.m_billboards.m_opaqueRenderNodes.empty() )
		{
			DoBeginOpaqueRendering();

			if ( !p_nodes.m_staticGeometries.m_opaqueRenderNodes.empty() )
			{
				DoRenderStaticSubmeshesNonInstanced( p_nodes.m_scene, p_camera, p_nodes.m_staticGeometries.m_opaqueRenderNodes );
			}

			if ( !p_nodes.m_instancedGeometries.m_opaqueRenderNodes.empty() )
			{
				if ( m_renderSystem.GetGpuInformations().HasInstancing() )
				{
					DoRenderInstancedSubmeshesInstanced( p_nodes.m_scene, p_camera, p_nodes.m_instancedGeometries.m_opaqueRenderNodes );
				}
				else
				{
					DoRenderInstancedSubmeshesNonInstanced( p_nodes.m_scene, p_camera, p_nodes.m_instancedGeometries.m_opaqueRenderNodes );
				}
			}

			if ( !p_nodes.m_animatedGeometries.m_opaqueRenderNodes.empty() )
			{
				DoRenderAnimatedSubmeshesNonInstanced( p_nodes.m_scene, p_camera, p_nodes.m_animatedGeometries.m_opaqueRenderNodes );
			}

			if ( !p_nodes.m_billboards.m_opaqueRenderNodes.empty() )
			{
				DoRenderBillboards( p_nodes.m_scene, p_camera, p_nodes.m_billboards.m_opaqueRenderNodes );
			}

			DoEndOpaqueRendering();
		}
	}

	void RenderTechnique::DoRenderTransparentNodes( SceneRenderNodes & p_nodes, Camera const & p_camera )
	{
		if ( !p_nodes.m_staticGeometries.m_transparentRenderNodesFront.empty()
			|| !p_nodes.m_animatedGeometries.m_transparentRenderNodesFront.empty()
			|| !p_nodes.m_billboards.m_transparentRenderNodesFront.empty() )
		{
			DoBeginTransparentRendering();

			if ( m_multisampling )
			{
				DoRenderInstancedSubmeshesInstanced( p_nodes.m_scene, p_camera, p_nodes.m_instancedGeometries.m_transparentRenderNodesFront, false );
				DoRenderStaticSubmeshesNonInstanced( p_nodes.m_scene, p_camera, p_nodes.m_staticGeometries.m_transparentRenderNodesFront, false );
				DoRenderAnimatedSubmeshesNonInstanced( p_nodes.m_scene, p_camera, p_nodes.m_animatedGeometries.m_transparentRenderNodesFront, false );
				DoRenderBillboards( p_nodes.m_scene, p_camera, p_nodes.m_billboards.m_transparentRenderNodesFront, false );

				DoRenderInstancedSubmeshesInstanced( p_nodes.m_scene, p_camera, p_nodes.m_instancedGeometries.m_transparentRenderNodesBack, true );
				DoRenderStaticSubmeshesNonInstanced( p_nodes.m_scene, p_camera, p_nodes.m_staticGeometries.m_transparentRenderNodesBack, true );
				DoRenderAnimatedSubmeshesNonInstanced( p_nodes.m_scene, p_camera, p_nodes.m_animatedGeometries.m_transparentRenderNodesBack, true );
				DoRenderBillboards( p_nodes.m_scene, p_camera, p_nodes.m_billboards.m_transparentRenderNodesBack, true );
			}
			else
			{
				{
					DistanceSortedNodeMap l_distanceSortedRenderNodes;
					DoSortAlpha( p_nodes.m_staticGeometries.m_transparentRenderNodesFront, p_camera, l_distanceSortedRenderNodes );
					DoSortAlpha( p_nodes.m_animatedGeometries.m_transparentRenderNodesFront, p_camera, l_distanceSortedRenderNodes );
					DoSortAlpha( p_nodes.m_billboards.m_transparentRenderNodesFront, p_camera, l_distanceSortedRenderNodes );

					if ( !l_distanceSortedRenderNodes.empty() )
					{
						DoRenderByDistance( p_nodes.m_scene, p_camera, l_distanceSortedRenderNodes, false );
					}
				}
				{
					DistanceSortedNodeMap l_distanceSortedRenderNodes;
					DoSortAlpha( p_nodes.m_staticGeometries.m_transparentRenderNodesBack, p_camera, l_distanceSortedRenderNodes );
					DoSortAlpha( p_nodes.m_animatedGeometries.m_transparentRenderNodesBack, p_camera, l_distanceSortedRenderNodes );
					DoSortAlpha( p_nodes.m_billboards.m_transparentRenderNodesBack, p_camera, l_distanceSortedRenderNodes );

					if ( !l_distanceSortedRenderNodes.empty() )
					{
						DoRenderByDistance( p_nodes.m_scene, p_camera, l_distanceSortedRenderNodes, false );
					}
				}
			}

			DoEndTransparentRendering();
		}
	}

	Pipeline & RenderTechnique::DoPrepareOpaquePipeline( ShaderProgram & p_program, PipelineFlags const & p_flags )
	{
		auto l_it = m_opaquePipelines.find( p_flags );

		if ( l_it == m_opaquePipelines.end() )
		{
			DepthStencilState l_dsState;
			RasteriserState l_rsState;
			l_rsState.SetCulledFaces( Culling::Back );
			MultisampleState l_msState;
			l_msState.SetMultisample( m_multisampling );
			l_it = m_opaquePipelines.emplace( p_flags, GetEngine()->GetRenderSystem()->CreatePipeline( std::move( l_dsState )
																									   , std::move( l_rsState )
																									   , DoCreateBlendState( p_flags.m_colourBlendMode, p_flags.m_alphaBlendMode )
																									   , std::move( l_msState )
																									   , p_program ) ).first;
		}

		return *l_it->second;
	}

	Pipeline & RenderTechnique::DoPrepareTransparentFrontPipeline( ShaderProgram & p_program, PipelineFlags const & p_flags )
	{
		auto l_it = m_frontTransparentPipelines.find( p_flags );

		if ( l_it == m_frontTransparentPipelines.end() )
		{
			DepthStencilState l_dsState;
			l_dsState.SetDepthMask( m_multisampling ? WritingMask::All : WritingMask::Zero );
			RasteriserState l_rsState;
			l_rsState.SetCulledFaces( Culling::Front );
			MultisampleState l_msState;
			l_msState.SetMultisample( m_multisampling );
			l_msState.EnableAlphaToCoverage( m_multisampling );
			l_it = m_frontTransparentPipelines.emplace( p_flags, GetEngine()->GetRenderSystem()->CreatePipeline( std::move( l_dsState )
																												 , std::move( l_rsState )
																												 , DoCreateBlendState( p_flags.m_colourBlendMode, p_flags.m_alphaBlendMode )
																												 , std::move( l_msState )
																												 , p_program ) ).first;
		}

		return *l_it->second;
	}

	Pipeline & RenderTechnique::DoPrepareTransparentBackPipeline( ShaderProgram & p_program, PipelineFlags const & p_flags )
	{
		auto l_it = m_backTransparentPipelines.find( p_flags );

		if ( l_it == m_backTransparentPipelines.end() )
		{
			DepthStencilState l_dsState;
			l_dsState.SetDepthMask( m_multisampling ? WritingMask::All : WritingMask::Zero );
			RasteriserState l_rsState;
			l_rsState.SetCulledFaces( Culling::Back );
			MultisampleState l_msState;
			l_msState.SetMultisample( m_multisampling );
			l_msState.EnableAlphaToCoverage( m_multisampling );
			l_it = m_backTransparentPipelines.emplace( p_flags, GetEngine()->GetRenderSystem()->CreatePipeline( std::move( l_dsState )
																												, std::move( l_rsState )
																												, DoCreateBlendState( p_flags.m_colourBlendMode, p_flags.m_alphaBlendMode )
																												, std::move( l_msState )
																												, p_program ) ).first;
		}

		return *l_it->second;
	}

	String RenderTechnique::DoGetOpaquePixelShaderSource( uint16_t p_textureFlags, uint8_t p_programFlags )const
	{
		using namespace GLSL;
		GlslWriter l_writer = m_renderSystem.CreateGlslWriter();

		// UBOs
		UBO_MATRIX( l_writer );
		UBO_SCENE( l_writer );
		UBO_PASS( l_writer );

		// Fragment Intputs
		auto vtx_vertex( l_writer.GetInput< Vec3 >( cuT( "vtx_vertex" ) ) );
		auto vtx_normal( l_writer.GetInput< Vec3 >( cuT( "vtx_normal" ) ) );
		auto vtx_tangent( l_writer.GetInput< Vec3 >( cuT( "vtx_tangent" ) ) );
		auto vtx_bitangent( l_writer.GetInput< Vec3 >( cuT( "vtx_bitangent" ) ) );
		auto vtx_texture( l_writer.GetInput< Vec3 >( cuT( "vtx_texture" ) ) );

		auto c3d_mapColour( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapColour, CheckFlag( p_textureFlags, TextureChannel::Colour ) ) );
		auto c3d_mapAmbient( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapAmbient, CheckFlag( p_textureFlags, TextureChannel::Ambient ) ) );
		auto c3d_mapDiffuse( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapDiffuse, CheckFlag( p_textureFlags, TextureChannel::Diffuse ) ) );
		auto c3d_mapNormal( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapNormal, CheckFlag( p_textureFlags, TextureChannel::Normal ) ) );
		auto c3d_mapSpecular( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapSpecular, CheckFlag( p_textureFlags, TextureChannel::Specular ) ) );
		auto c3d_mapEmissive( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapEmissive, CheckFlag( p_textureFlags, TextureChannel::Emissive ) ) );
		auto c3d_mapHeight( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapHeight, CheckFlag( p_textureFlags, TextureChannel::Height ) ) );
		auto c3d_mapGloss( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapGloss, CheckFlag( p_textureFlags, TextureChannel::Gloss ) ) );

		if ( l_writer.HasTextureBuffers() )
		{
			auto c3d_sLights = l_writer.GetUniform< SamplerBuffer >( cuT( "c3d_sLights" ) );
		}
		else
		{
			auto c3d_sLights = l_writer.GetUniform< Sampler1D >( cuT( "c3d_sLights" ) );
		}

		std::unique_ptr< LightingModel > l_lighting = l_writer.CreateLightingModel( PhongLightingModel::Name, p_textureFlags );

		// Fragment Outtputs
		auto pxl_v4FragColor( l_writer.GetFragData< Vec4 >( cuT( "pxl_v4FragColor" ), 0 ) );

		l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			auto l_v3Normal = l_writer.GetLocale< Vec3 >( cuT( "l_v3Normal" ), normalize( vec3( vtx_normal.x(), vtx_normal.y(), vtx_normal.z() ) ) );
			auto l_v3Ambient = l_writer.GetLocale< Vec3 >( cuT( "l_v3Ambient" ), c3d_v4AmbientLight.xyz() );
			auto l_v3Diffuse = l_writer.GetLocale< Vec3 >( cuT( "l_v3Diffuse" ), vec3( Float( 0.0f ), 0, 0 ) );
			auto l_v3Specular = l_writer.GetLocale< Vec3 >( cuT( "l_v3Specular" ), vec3( Float( 0.0f ), 0, 0 ) );
			auto l_fMatShininess = l_writer.GetLocale< Float >( cuT( "l_fMatShininess" ), c3d_fMatShininess );
			auto l_v3Emissive = l_writer.GetLocale< Vec3 >( cuT( "l_v3Emissive" ), c3d_v4MatEmissive.xyz() );
			auto l_worldEye = l_writer.GetLocale< Vec3 >( cuT( "l_worldEye" ), vec3( c3d_v3CameraPosition.x(), c3d_v3CameraPosition.y(), c3d_v3CameraPosition.z() ) );
			pxl_v4FragColor = vec4( Float( 0.0f ), 0.0f, 0.0f, 0.0f );
			Vec3 l_v3MapNormal( &l_writer, cuT( "l_v3MapNormal" ) );

			if ( CheckFlag( p_textureFlags, TextureChannel::Normal ) )
			{
				auto l_v3MapNormal = l_writer.GetLocale< Vec3 >( cuT( "l_v3MapNormal" ), texture( c3d_mapNormal, vtx_texture.xy() ).xyz() );
				l_v3MapNormal = Float( &l_writer, 2.0f ) * l_v3MapNormal - vec3( Int( &l_writer, 1 ), 1.0, 1.0 );
				l_v3Normal = normalize( mat3( vtx_tangent, vtx_bitangent, vtx_normal ) * l_v3MapNormal );
			}

			if ( CheckFlag( p_textureFlags, TextureChannel::Gloss ) )
			{
				l_fMatShininess = texture( c3d_mapGloss, vtx_texture.xy() ).r();
			}

			if ( CheckFlag( p_textureFlags, TextureChannel::Emissive ) )
			{
				l_v3Emissive = texture( c3d_mapEmissive, vtx_texture.xy() ).xyz();
			}

			auto l_begin = l_writer.GetLocale< Int >( cuT( "l_begin" ), Int( 0 ) );
			auto l_end = l_writer.GetLocale< Int >( cuT( "l_end" ), c3d_iLightsCount.x() );

			FOR( l_writer, Int, i, l_begin, cuT( "i < l_end" ), cuT( "++i" ) )
			{
				OutputComponents l_output { l_v3Ambient, l_v3Diffuse, l_v3Specular };
				l_lighting->ComputeDirectionalLight( l_lighting->GetDirectionalLight( i ), l_worldEye, l_fMatShininess,
													 FragmentInput { vtx_vertex, l_v3Normal, vtx_tangent, vtx_bitangent },
													 l_output );
			}
			ROF;

			l_begin = l_end;
			l_end += c3d_iLightsCount.y();

			FOR( l_writer, Int, i, l_begin, cuT( "i < l_end" ), cuT( "++i" ) )
			{
				OutputComponents l_output { l_v3Ambient, l_v3Diffuse, l_v3Specular };
				l_lighting->ComputePointLight( l_lighting->GetPointLight( i ), l_worldEye, l_fMatShininess,
											   FragmentInput { vtx_vertex, l_v3Normal, vtx_tangent, vtx_bitangent },
											   l_output );
			}
			ROF;

			l_begin = l_end;
			l_end += c3d_iLightsCount.z();

			FOR( l_writer, Int, i, l_begin, cuT( "i < l_end" ), cuT( "++i" ) )
			{
				OutputComponents l_output { l_v3Ambient, l_v3Diffuse, l_v3Specular };
				l_lighting->ComputeSpotLight( l_lighting->GetSpotLight( i ), l_worldEye, l_fMatShininess,
											  FragmentInput { vtx_vertex, l_v3Normal, vtx_tangent, vtx_bitangent },
											  l_output );
			}
			ROF;

			if ( CheckFlag( p_textureFlags, TextureChannel::Colour ) )
			{
			l_v3Ambient += texture( c3d_mapColour, vtx_texture.xy() ).xyz();
			}

			if ( CheckFlag( p_textureFlags, TextureChannel::Ambient ) )
			{
				l_v3Ambient += texture( c3d_mapAmbient, vtx_texture.xy() ).xyz();
			}

			if ( CheckFlag( p_textureFlags, TextureChannel::Diffuse ) )
			{
				l_v3Diffuse *= texture( c3d_mapDiffuse, vtx_texture.xy() ).xyz();
			}

			if ( CheckFlag( p_textureFlags, TextureChannel::Specular ) )
			{
				l_v3Specular *= texture( c3d_mapSpecular, vtx_texture.xy() ).xyz();
			}

			pxl_v4FragColor = vec4( l_writer.Paren( l_v3Ambient + c3d_v4MatAmbient.xyz() ) +
									l_writer.Paren( l_v3Diffuse * c3d_v4MatDiffuse.xyz() ) +
									l_writer.Paren( l_v3Specular * c3d_v4MatSpecular.xyz() ) +
									l_v3Emissive, 1.0 );
		} );

		return l_writer.Finalise();
	}

	String RenderTechnique::DoGetTransparentPixelShaderSource( uint16_t p_textureFlags, uint8_t p_programFlags )const
	{
		using namespace GLSL;
		GlslWriter l_writer = m_renderSystem.CreateGlslWriter();

		// UBOs
		UBO_MATRIX( l_writer );
		UBO_SCENE( l_writer );
		UBO_PASS( l_writer );

		// Fragment Intputs
		auto vtx_vertex( l_writer.GetInput< Vec3 >( cuT( "vtx_vertex" ) ) );
		auto vtx_normal( l_writer.GetInput< Vec3 >( cuT( "vtx_normal" ) ) );
		auto vtx_tangent( l_writer.GetInput< Vec3 >( cuT( "vtx_tangent" ) ) );
		auto vtx_bitangent( l_writer.GetInput< Vec3 >( cuT( "vtx_bitangent" ) ) );
		auto vtx_texture( l_writer.GetInput< Vec3 >( cuT( "vtx_texture" ) ) );

		if ( l_writer.HasTextureBuffers() )
		{
			auto c3d_sLights = l_writer.GetUniform< SamplerBuffer >( cuT( "c3d_sLights" ) );
		}
		else
		{
			auto c3d_sLights = l_writer.GetUniform< Sampler1D >( cuT( "c3d_sLights" ) );
		}

		auto c3d_mapColour( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapColour, CheckFlag( p_textureFlags, TextureChannel::Colour ) ) );
		auto c3d_mapAmbient( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapAmbient, CheckFlag( p_textureFlags, TextureChannel::Ambient ) ) );
		auto c3d_mapDiffuse( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapDiffuse, CheckFlag( p_textureFlags, TextureChannel::Diffuse ) ) );
		auto c3d_mapNormal( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapNormal, CheckFlag( p_textureFlags, TextureChannel::Normal ) ) );
		auto c3d_mapOpacity( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapOpacity, CheckFlag( p_textureFlags, TextureChannel::Opacity ) ) );
		auto c3d_mapSpecular( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapSpecular, CheckFlag( p_textureFlags, TextureChannel::Specular ) ) );
		auto c3d_mapEmissive( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapEmissive, CheckFlag( p_textureFlags, TextureChannel::Emissive ) ) );
		auto c3d_mapHeight( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapHeight, CheckFlag( p_textureFlags, TextureChannel::Height ) ) );
		auto c3d_mapGloss( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapGloss, CheckFlag( p_textureFlags, TextureChannel::Gloss ) ) );

		std::unique_ptr< LightingModel > l_lighting = l_writer.CreateLightingModel( PhongLightingModel::Name, p_textureFlags );

		// Fragment Outtputs
		auto pxl_v4FragColor( l_writer.GetFragData< Vec4 >( cuT( "pxl_v4FragColor" ), 0 ) );

		l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			auto l_v3Normal = l_writer.GetLocale< Vec3 >( cuT( "l_v3Normal" ), normalize( vec3( vtx_normal.x(), vtx_normal.y(), vtx_normal.z() ) ) );
			auto l_v3Ambient = l_writer.GetLocale< Vec3 >( cuT( "l_v3Ambient" ), c3d_v4AmbientLight.xyz() );
			auto l_v3Diffuse = l_writer.GetLocale< Vec3 >( cuT( "l_v3Diffuse" ), vec3( Float( 0.0f ), 0, 0 ) );
			auto l_v3Specular = l_writer.GetLocale< Vec3 >( cuT( "l_v3Specular" ), vec3( Float( 0.0f ), 0, 0 ) );
			auto l_fAlpha = l_writer.GetLocale< Float >( cuT( "l_fAlpha" ), c3d_fMatOpacity );
			auto l_fMatShininess = l_writer.GetLocale< Float >( cuT( "l_fMatShininess" ), c3d_fMatShininess );
			auto l_v3Emissive = l_writer.GetLocale< Vec3 >( cuT( "l_v3Emissive" ), c3d_v4MatEmissive.xyz() );
			auto l_worldEye = l_writer.GetLocale< Vec3 >( cuT( "l_worldEye" ), vec3( c3d_v3CameraPosition.x(), c3d_v3CameraPosition.y(), c3d_v3CameraPosition.z() ) );
			pxl_v4FragColor = vec4( Float( 0.0f ), 0.0f, 0.0f, 0.0f );
			Vec3 l_v3MapNormal( &l_writer, cuT( "l_v3MapNormal" ) );

			if ( CheckFlag( p_textureFlags, TextureChannel::Normal ) )
			{
				auto l_v3MapNormal = l_writer.GetLocale< Vec3 >( cuT( "l_v3MapNormal" ), texture( c3d_mapNormal, vtx_texture.xy() ).xyz() );
				l_v3MapNormal = Float( &l_writer, 2.0f ) * l_v3MapNormal - vec3( Int( &l_writer, 1 ), 1.0, 1.0 );
				l_v3Normal = normalize( mat3( vtx_tangent, vtx_bitangent, vtx_normal ) * l_v3MapNormal );
			}

			if ( CheckFlag( p_textureFlags, TextureChannel::Gloss ) )
			{
				l_fMatShininess = texture( c3d_mapGloss, vtx_texture.xy() ).r();
			}

			if ( CheckFlag( p_textureFlags, TextureChannel::Emissive ) )
			{
				l_v3Emissive = texture( c3d_mapEmissive, vtx_texture.xy() ).xyz();
			}

			auto l_begin = l_writer.GetLocale< Int >( cuT( "l_begin" ), Int( 0 ) );
			auto l_end = l_writer.GetLocale< Int >( cuT( "l_end" ), c3d_iLightsCount.x() );

			FOR( l_writer, Int, i, l_begin, cuT( "i < l_end" ), cuT( "++i" ) )
			{
				OutputComponents l_output { l_v3Ambient, l_v3Diffuse, l_v3Specular };
				l_lighting->ComputeDirectionalLight( l_lighting->GetDirectionalLight( i ), l_worldEye, l_fMatShininess,
													 FragmentInput { vtx_vertex, l_v3Normal, vtx_tangent, vtx_bitangent },
													 l_output );
			}
			ROF;

			l_begin = l_end;
			l_end += c3d_iLightsCount.y();

			FOR( l_writer, Int, i, l_begin, cuT( "i < l_end" ), cuT( "++i" ) )
			{
				OutputComponents l_output { l_v3Ambient, l_v3Diffuse, l_v3Specular };
				l_lighting->ComputePointLight( l_lighting->GetPointLight( i ), l_worldEye, l_fMatShininess,
											   FragmentInput { vtx_vertex, l_v3Normal, vtx_tangent, vtx_bitangent },
											   l_output );
			}
			ROF;

			l_begin = l_end;
			l_end += c3d_iLightsCount.z();

			FOR( l_writer, Int, i, l_begin, cuT( "i < l_end" ), cuT( "++i" ) )
			{
				OutputComponents l_output { l_v3Ambient, l_v3Diffuse, l_v3Specular };
				l_lighting->ComputeSpotLight( l_lighting->GetSpotLight( i ), l_worldEye, l_fMatShininess,
											  FragmentInput { vtx_vertex, l_v3Normal, vtx_tangent, vtx_bitangent },
											  l_output );
			}
			ROF;

			if ( CheckFlag( p_textureFlags, TextureChannel::Opacity ) )
			{
				l_fAlpha = texture( c3d_mapOpacity, vtx_texture.xy() ).r() * c3d_fMatOpacity;
			}

			if ( CheckFlag( p_textureFlags, TextureChannel::Colour ) )
			{
			l_v3Ambient += texture( c3d_mapColour, vtx_texture.xy() ).xyz();
			}

			if ( CheckFlag( p_textureFlags, TextureChannel::Ambient ) )
			{
				l_v3Ambient += texture( c3d_mapAmbient, vtx_texture.xy() ).xyz();
			}

			if ( CheckFlag( p_textureFlags, TextureChannel::Diffuse ) )
			{
				l_v3Diffuse *= texture( c3d_mapDiffuse, vtx_texture.xy() ).xyz();
			}

			if ( CheckFlag( p_textureFlags, TextureChannel::Specular ) )
			{
				l_v3Specular *= texture( c3d_mapSpecular, vtx_texture.xy() ).xyz();
			}

			pxl_v4FragColor = vec4( l_fAlpha * l_writer.Paren( l_writer.Paren( l_v3Ambient + c3d_v4MatAmbient.xyz() ) +
															   l_writer.Paren( l_v3Diffuse * c3d_v4MatDiffuse.xyz() ) +
															   l_writer.Paren( l_v3Specular * c3d_v4MatSpecular.xyz() ) +
															   l_v3Emissive ), l_fAlpha );
		} );

		return l_writer.Finalise();
	}
}
