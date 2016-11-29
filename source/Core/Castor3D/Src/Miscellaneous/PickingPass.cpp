#include "PickingPass.hpp"

#include "Engine.hpp"
#include "ShaderCache.hpp"

#include "FrameBuffer/ColourRenderBuffer.hpp"
#include "FrameBuffer/DepthStencilRenderBuffer.hpp"
#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/RenderBufferAttachment.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "Mesh/Submesh.hpp"
#include "Mesh/Buffer/BufferElementGroup.hpp"
#include "Mesh/Buffer/GeometryBuffers.hpp"
#include "Mesh/Buffer/VertexBuffer.hpp"
#include "Render/Pipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Render/RenderTarget.hpp"
#include "Scene/BillboardList.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Geometry.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Shader/FrameVariableBuffer.hpp"
#include "State/DepthStencilState.hpp"
#include "State/MultisampleState.hpp"
#include "State/RasteriserState.hpp"
#include "Texture/TextureLayout.hpp"

#include <GlslSource.hpp>

#include <Graphics/Image.hpp>

using namespace Castor;

namespace Castor3D
{
	namespace
	{
		static String const Picking = cuT( "Picking" );
		static String const DrawIndex = cuT( "c3d_iDrawIndex" );
		static String const NodeIndex = cuT( "c3d_iNodeIndex" );

		template< bool Opaque >
		struct PipelineUpdater
		{
			static inline void Update( RenderPass const & p_pass, Camera const & p_camera, Pipeline & p_pipeline )
			{
				auto l_depthMaps = DepthMapArray{};
				p_pass.UpdateOpaquePipeline( p_camera, p_pipeline, l_depthMaps );
			}
		};

		template<>
		struct PipelineUpdater< false >
		{
			static inline void Update( RenderPass const & p_pass, Camera const & p_camera, Pipeline & p_pipeline )
			{
				auto l_depthMaps = DepthMapArray{};
				p_pass.UpdateTransparentPipeline( p_camera, p_pipeline, l_depthMaps );
			}
		};

		template< bool Opaque, typename MapType, typename FuncType >
		inline void DoTraverseNodes( RenderPass const & p_pass
									 , Camera const & p_camera
									 , MapType & p_nodes
									 , uint8_t p_index
									 , FuncType p_function )
		{
			uint32_t l_count{ 1u };

			for ( auto l_itPipelines : p_nodes )
			{
				PipelineUpdater< Opaque >::Update( p_pass, p_camera, *l_itPipelines.first );
				l_itPipelines.first->Apply();
				FrameVariableBufferSPtr l_ubo = l_itPipelines.first->GetProgram().FindFrameVariableBuffer( Picking );
				OneUIntFrameVariableSPtr l_drawIndex;
				OneUIntFrameVariableSPtr l_nodeIndex;
				l_ubo->GetVariable( DrawIndex, l_drawIndex );
				l_ubo->GetVariable( NodeIndex, l_nodeIndex );
				l_drawIndex->SetValue( p_index + ( ( l_count & 0x00FFFFFF ) << 8 ) );
				uint32_t l_index{ 0u };

				for ( auto l_itPass : l_itPipelines.second )
				{
					for ( auto l_itSubmeshes : l_itPass.second )
					{
						l_nodeIndex->SetValue( l_index++ );
						p_function( *l_itPipelines.first, *l_itPass.first, *l_itSubmeshes.first, l_itSubmeshes.second );
					}
				}

				l_count++;
			}
		}

		template< bool Opaque, typename MapType >
		inline void DoRenderNonInstanced( RenderPass const & p_pass
										  , Scene & p_scene
										  , Camera const & p_camera
										  , uint8_t p_index
										  , MapType & p_nodes )
		{
			auto l_depthMaps = DepthMapArray{};
			uint32_t l_count{ 1u };

			for ( auto l_itPipelines : p_nodes )
			{
				PipelineUpdater< Opaque >::Update( p_pass, p_camera, *l_itPipelines.first );
				l_itPipelines.first->Apply();
				FrameVariableBufferSPtr l_ubo = l_itPipelines.first->GetProgram().FindFrameVariableBuffer( Picking );
				OneUIntFrameVariableSPtr l_drawIndex;
				OneUIntFrameVariableSPtr l_nodeIndex;
				l_ubo->GetVariable( DrawIndex, l_drawIndex );
				l_ubo->GetVariable( NodeIndex, l_nodeIndex );
				l_drawIndex->SetValue( p_index + ( ( l_count & 0x00FFFFFF ) << 8 ) );
				uint32_t l_index{ 0u };

				for ( auto & l_renderNode : l_itPipelines.second )
				{
					l_nodeIndex->SetValue( l_index++ );

					if ( l_renderNode.m_data.IsInitialised() )
					{
						l_renderNode.Render( l_depthMaps );
					}
				}

				l_count++;
			}
		}

		inline void DoUpdateProgram( ShaderProgram & p_program )
		{
			if ( !p_program.FindFrameVariableBuffer( Picking ) )
			{
				auto & l_picking = p_program.CreateFrameVariableBuffer( Picking, ShaderTypeFlag::ePixel );
				l_picking.CreateVariable< OneUIntFrameVariable >( DrawIndex );
				l_picking.CreateVariable< OneUIntFrameVariable >( NodeIndex );

				if ( p_program.GetRenderSystem()->GetCurrentContext() )
				{
					p_program.Bind( false );
					l_picking.Initialise();
					p_program.Unbind();
				}
			}
		}

		template< typename MapType >
		inline void DoPickFromList( MapType const & p_map, Point3f const & p_index, GeometryWPtr & p_geometry, SubmeshWPtr & p_submesh, uint32_t & p_face )
		{
			uint32_t l_pipelineIndex{ ( uint32_t( p_index[0] ) >> 8 ) - 1 };
			uint32_t l_nodeIndex{ uint32_t( p_index[1] ) };
			uint32_t l_faceIndex{ uint32_t( p_index[3] ) };

			REQUIRE( p_map.size() > l_pipelineIndex );
			auto l_itPipeline = p_map.begin();

			while ( l_pipelineIndex )
			{
				++l_itPipeline;
				--l_pipelineIndex;
			}

			REQUIRE( l_itPipeline->second.size() > l_nodeIndex );
			auto l_itNode = l_itPipeline->second.begin() + l_nodeIndex;

			p_submesh = l_itNode->m_data.shared_from_this();
			p_geometry = std::static_pointer_cast< Geometry >( l_itNode->m_geometry.shared_from_this() );
			p_face = l_faceIndex;
		}

		template<>
		inline void DoPickFromList< SubmeshStaticRenderNodesByPipelineMap >( SubmeshStaticRenderNodesByPipelineMap const & p_map, Point3f const & p_index, GeometryWPtr & p_geometry, SubmeshWPtr & p_submesh, uint32_t & p_face )
		{
			uint32_t l_pipelineIndex{ ( uint32_t( p_index[0] ) >> 8 ) - 1 };
			uint32_t l_nodeIndex{ uint32_t( p_index[1] ) };
			uint32_t l_instanceIndex{ uint32_t( p_index[2] ) };
			uint32_t l_faceIndex{ uint32_t( p_index[3] ) };

			REQUIRE( p_map.size() > l_pipelineIndex );
			auto l_itPipeline = p_map.begin();

			while ( l_pipelineIndex )
			{
				++l_itPipeline;
				--l_pipelineIndex;
			}

			auto l_itPass = l_itPipeline->second.begin();
			REQUIRE( !l_itPass->second.empty() );
			auto l_itMesh = l_itPass->second.begin();

			while ( l_nodeIndex && l_itPass != l_itPipeline->second.end() )
			{
				while ( l_itMesh != l_itPass->second.end() && l_nodeIndex )
				{
					++l_itMesh;
					--l_nodeIndex;
				}

				if ( l_nodeIndex || l_itMesh == l_itPass->second.end() )
				{
					++l_itPass;

					if ( l_itPass != l_itPipeline->second.end() )
					{
						l_itMesh = l_itPass->second.begin();
					}
				}
			}

			if ( l_itPass != l_itPipeline->second.end()
					&& l_itMesh != l_itPass->second.end() )
			{
				REQUIRE( !l_itMesh->second.empty() );
				auto l_itNode = l_itMesh->second.begin() + l_instanceIndex;

				p_submesh = l_itNode->m_data.shared_from_this();
				p_geometry = std::static_pointer_cast< Geometry >( l_itNode->m_geometry.shared_from_this() );
				p_face = l_faceIndex;
			}
		}
	}

	PickingPass::PickingPass( Engine & p_engine )
		: RenderPass{ cuT( "Picking" ), p_engine }
	{
	}

	PickingPass::~PickingPass()
	{
	}

	bool PickingPass::Initialise( Size const & p_size )
	{
		m_colourTexture = GetEngine()->GetRenderSystem()->CreateTexture( TextureType::eTwoDimensions, AccessType::eRead, AccessType::eRead | AccessType::eWrite, PixelFormat::eRGB32F, p_size );
		m_colourTexture->GetImage().InitialiseSource();
		auto l_size = m_colourTexture->GetDimensions();
		bool l_return = m_colourTexture->Initialise();

		if ( l_return )
		{
			m_frameBuffer = GetEngine()->GetRenderSystem()->CreateFrameBuffer();
			m_frameBuffer->SetClearColour( Colour::from_predef( PredefinedColour::eOpaqueBlack ) );
			m_depthBuffer = m_frameBuffer->CreateDepthStencilRenderBuffer( PixelFormat::eD32F );
			l_return = m_depthBuffer->Create();
		}

		if ( l_return )
		{
			l_return = m_depthBuffer->Initialise( l_size );

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
			l_return = m_frameBuffer->Initialise( l_size );

			if ( l_return && m_frameBuffer->Bind( FrameBufferMode::eConfig ) )
			{
				m_frameBuffer->Attach( AttachmentPoint::eColour, 0, m_colourAttach, m_colourTexture->GetType() );
				m_frameBuffer->Attach( AttachmentPoint::eDepth, m_depthAttach );
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

	void PickingPass::Cleanup()
	{
		if ( m_frameBuffer )
		{
			m_frameBuffer->Bind( FrameBufferMode::eConfig );
			m_frameBuffer->DetachAll();
			m_frameBuffer->Unbind();
			m_frameBuffer->Cleanup();
			m_colourTexture->Cleanup();
			m_depthBuffer->Cleanup();

			m_depthBuffer->Destroy();
			m_frameBuffer->Destroy();

			m_depthAttach.reset();
			m_depthBuffer.reset();
			m_colourAttach.reset();
			m_colourTexture.reset();
			m_frameBuffer.reset();
		}

		for ( auto l_buffer : m_geometryBuffers )
		{
			l_buffer->Cleanup();
		}

		m_geometryBuffers.clear();
	}

	void PickingPass::AddScene( Scene & p_scene, Camera & p_camera )
	{
		auto l_itScn = m_scenes.emplace( &p_scene, CameraQueueMap{} ).first;
		auto l_itCam = l_itScn->second.emplace( &p_camera, RenderQueue{ *this } ).first;
		l_itCam->second.Initialise( p_scene, p_camera );
	}

	bool PickingPass::Pick( Castor::Position const & p_position, Camera const & p_camera )
	{
		bool l_return{ false };
		m_geometry.reset();
		m_submesh.reset();
		m_face = 0u;

		auto l_itScn = m_scenes.find( p_camera.GetScene() );

		if ( l_itScn != m_scenes.end() )
		{
			auto l_itCam = l_itScn->second.find( &p_camera );

			if ( l_itCam != l_itScn->second.end() )
			{
				l_itCam->second.Update();

				if ( m_frameBuffer->Bind( FrameBufferMode::eAutomatic, FrameBufferTarget::eDraw ) )
				{
					m_frameBuffer->Clear();
					p_camera.Apply();
					auto & l_nodes = l_itCam->second.GetRenderNodes();
					DoRenderOpaqueNodes( l_nodes, p_camera );
					DoRenderTransparentNodes( l_nodes, p_camera );
					m_frameBuffer->Unbind();

					if ( m_colourTexture->Bind( 0 ) )
					{
						Point3f l_pixel;
						auto l_data = m_colourTexture->Lock( AccessType::eRead, 0u );

						if ( l_data )
						{
							auto l_dimensions = m_colourTexture->GetDimensions();
							auto l_format = m_colourTexture->GetPixelFormat();
							Image l_image{ cuT( "tmp" ), l_dimensions, l_format, l_data, l_format };
							l_image.GetPixel( p_position.x(), l_dimensions.height() - 1 - p_position.y(), reinterpret_cast< uint8_t * >( l_pixel.ptr() ), l_format );

#if 0

							Image::BinaryWriter()( l_image, Engine::GetEngineDirectory() / cuT( "\\ColourBuffer_Picking.hdr" ) );

#endif
							m_colourTexture->Unlock( false, 0u );
						}

						m_colourTexture->Unbind( 0 );

						if ( Castor::point::distance_squared( l_pixel ) )
						{
							uint32_t l_index = uint32_t( l_pixel[0] );

							switch ( l_index & 0xFF )
							{
							case 0u:
								l_return = true;
								DoPickFromList( l_nodes.m_instancedGeometries.m_opaqueRenderNodesBack, l_pixel, m_geometry, m_submesh, m_face );
								break;

							case 1u:
								l_return = true;
								DoPickFromList( l_nodes.m_instancedGeometries.m_transparentRenderNodesBack, l_pixel, m_geometry, m_submesh, m_face );
								break;

							case 2u:
								l_return = true;
								DoPickFromList( l_nodes.m_staticGeometries.m_opaqueRenderNodesBack, l_pixel, m_geometry, m_submesh, m_face );
								break;

							case 3u:
								l_return = true;
								DoPickFromList( l_nodes.m_staticGeometries.m_transparentRenderNodesBack, l_pixel, m_geometry, m_submesh, m_face );
								break;

							case 4u:
								l_return = true;
								DoPickFromList( l_nodes.m_animatedGeometries.m_opaqueRenderNodesBack, l_pixel, m_geometry, m_submesh, m_face );
								break;

							case 5u:
								l_return = true;
								DoPickFromList( l_nodes.m_animatedGeometries.m_transparentRenderNodesBack, l_pixel, m_geometry, m_submesh, m_face );
								break;

							case 6u:
								//DoPickFromList( l_nodes.m_billboards.m_opaqueRenderNodesBack, l_pixel, m_geometry, m_submesh, m_face );
								break;

							case 7u:
								//DoPickFromList( l_nodes.m_billboards.m_transparentRenderNodesBack, l_pixel, m_geometry, m_submesh, m_face );
								break;

							default:
								FAILURE( "Unsupported index" );
								break;
							}
						}
					}
				}
			}
		}

		return l_return;
	}

	AnimatedGeometryRenderNode PickingPass::CreateAnimatedNode( Pass & p_pass
															   , Pipeline & p_pipeline
															   , Submesh & p_submesh
															   , Geometry & p_primitive
															   , AnimatedSkeletonSPtr p_skeleton
															   , AnimatedMeshSPtr p_mesh )
	{
		auto l_animationBuffer = p_pipeline.GetProgram().FindFrameVariableBuffer( ShaderProgram::BufferAnimation );
		auto l_buffer = p_submesh.GetGeometryBuffers( p_pipeline.GetProgram() );
		m_geometryBuffers.insert( l_buffer );

		return AnimatedGeometryRenderNode
		{
			DoCreateSceneRenderNode( *p_primitive.GetScene(), p_pipeline ),
			DoCreatePassRenderNode( p_pass, p_pipeline ),
			*l_buffer,
			*p_primitive.GetParent(),
			p_submesh,
			p_primitive,
			p_skeleton.get(),
			p_mesh.get(),
			*l_animationBuffer
		};
	}

	StaticGeometryRenderNode PickingPass::CreateStaticNode( Pass & p_pass
														   , Pipeline & p_pipeline
														   , Submesh & p_submesh
														   , Geometry & p_primitive )
	{
		auto l_buffer = p_submesh.GetGeometryBuffers( p_pipeline.GetProgram() );
		m_geometryBuffers.insert( l_buffer );

		return StaticGeometryRenderNode
		{
			DoCreateSceneRenderNode( *p_primitive.GetScene(), p_pipeline ),
			DoCreatePassRenderNode( p_pass, p_pipeline ),
			*l_buffer,
			*p_primitive.GetParent(),
			p_submesh,
			p_primitive,
		};
	}

	BillboardRenderNode PickingPass::CreateBillboardNode( Pass & p_pass
														 , Pipeline & p_pipeline
														 , BillboardBase & p_billboard )
	{
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
			p_billboard,
			*l_billboardBuffer,
			*l_billboardBuffer->GetVariable( ShaderProgram::Dimensions, l_pt2i ),
			*l_billboardBuffer->GetVariable( ShaderProgram::WindowSize, l_pt2i )
		};
	}

	void PickingPass::DoRenderOpaqueNodes( SceneRenderNodes & p_nodes, Camera const & p_camera )
	{
		DoRenderOpaqueInstancedSubmeshesInstanced( p_nodes.m_scene, p_camera, 0u, p_nodes.m_instancedGeometries.m_opaqueRenderNodesBack );
		DoRenderOpaqueStaticSubmeshesNonInstanced( p_nodes.m_scene, p_camera, 2u, p_nodes.m_staticGeometries.m_opaqueRenderNodesBack );
		DoRenderOpaqueAnimatedSubmeshesNonInstanced( p_nodes.m_scene, p_camera, 4u, p_nodes.m_animatedGeometries.m_opaqueRenderNodesBack );
		DoRenderOpaqueBillboards( p_nodes.m_scene, p_camera, 6u, p_nodes.m_billboards.m_opaqueRenderNodesBack );
	}

	void PickingPass::DoRenderTransparentNodes( SceneRenderNodes & p_nodes, Camera const & p_camera )
	{
		DoRenderTransparentInstancedSubmeshesInstanced( p_nodes.m_scene, p_camera, 1u, p_nodes.m_instancedGeometries.m_transparentRenderNodesBack );
		DoRenderTransparentStaticSubmeshesNonInstanced( p_nodes.m_scene, p_camera, 3u, p_nodes.m_staticGeometries.m_transparentRenderNodesBack );
		DoRenderTransparentAnimatedSubmeshesNonInstanced( p_nodes.m_scene, p_camera, 5u, p_nodes.m_animatedGeometries.m_transparentRenderNodesBack );
		DoRenderTransparentBillboards( p_nodes.m_scene, p_camera, 7u, p_nodes.m_billboards.m_transparentRenderNodesBack );
	}

	void PickingPass::DoRenderOpaqueInstancedSubmeshesInstanced( Scene & p_scene, Camera const & p_camera, uint8_t p_index, SubmeshStaticRenderNodesByPipelineMap & p_nodes )
	{
		DoTraverseNodes< true >( *this, p_camera, p_nodes, p_index, [&p_scene, &p_camera, this]( Pipeline & p_pipeline, Pass & p_pass, Submesh & p_submesh, StaticGeometryRenderNodeArray & p_renderNodes )
		{
			if ( !p_renderNodes.empty() && p_submesh.HasMatrixBuffer() )
			{
				uint32_t l_count = uint32_t( p_renderNodes.size() );
				auto & l_matrixBuffer = p_submesh.GetMatrixBuffer();
				uint8_t * l_buffer = l_matrixBuffer.data();
				const uint32_t l_stride = 16 * sizeof( real );

				for ( auto const & l_renderNode : p_renderNodes )
				{
					std::memcpy( l_buffer, l_renderNode.m_sceneNode.GetDerivedTransformationMatrix().const_ptr(), l_stride );
					l_buffer += l_stride;
				}

				l_matrixBuffer.Upload( 0u, l_matrixBuffer.GetSize(), l_matrixBuffer.data() );
				auto l_depthMaps = DepthMapArray{};
				p_renderNodes[0].BindPass( l_depthMaps, MASK_MTXMODE_MODEL );
				p_submesh.DrawInstanced( p_renderNodes[0].m_buffers, l_count );
				p_renderNodes[0].UnbindPass( l_depthMaps );
			}
		} );
	}

	void PickingPass::DoRenderOpaqueStaticSubmeshesNonInstanced( Scene & p_scene, Camera const & p_camera, uint8_t p_index, StaticGeometryRenderNodesByPipelineMap & p_nodes )
	{
		DoRenderNonInstanced< true >( *this, p_scene, p_camera, p_index, p_nodes );
	}

	void PickingPass::DoRenderOpaqueAnimatedSubmeshesNonInstanced( Scene & p_scene, Camera const & p_camera, uint8_t p_index, AnimatedGeometryRenderNodesByPipelineMap & p_nodes )
	{
		DoRenderNonInstanced< true >( *this, p_scene, p_camera, p_index, p_nodes );
	}

	void PickingPass::DoRenderOpaqueBillboards( Scene & p_scene, Camera const & p_camera, uint8_t p_index, BillboardRenderNodesByPipelineMap & p_nodes )
	{
		DoRenderNonInstanced< true >( *this, p_scene, p_camera, p_index, p_nodes );
	}

	void PickingPass::DoRenderTransparentInstancedSubmeshesInstanced( Scene & p_scene, Camera const & p_camera, uint8_t p_index, SubmeshStaticRenderNodesByPipelineMap & p_nodes )
	{
		DoTraverseNodes< false >( *this, p_camera, p_nodes, p_index, [&p_scene, &p_camera, this]( Pipeline & p_pipeline, Pass & p_pass, Submesh & p_submesh, StaticGeometryRenderNodeArray & p_renderNodes )
		{
			if ( !p_renderNodes.empty() && p_submesh.HasMatrixBuffer() )
			{
				uint32_t l_count = uint32_t( p_renderNodes.size() );
				auto & l_matrixBuffer = p_submesh.GetMatrixBuffer();
				uint8_t * l_buffer = l_matrixBuffer.data();
				const uint32_t l_stride = 16 * sizeof( real );

				for ( auto const & l_renderNode : p_renderNodes )
				{
					std::memcpy( l_buffer, l_renderNode.m_sceneNode.GetDerivedTransformationMatrix().const_ptr(), l_stride );
					l_buffer += l_stride;
				}

				l_matrixBuffer.Upload( 0u, l_matrixBuffer.GetSize(), l_matrixBuffer.data() );
				auto l_depthMaps = DepthMapArray{};
				p_renderNodes[0].BindPass( l_depthMaps, MASK_MTXMODE_MODEL );
				p_submesh.DrawInstanced( p_renderNodes[0].m_buffers, l_count );
				p_renderNodes[0].UnbindPass( l_depthMaps );
			}
		} );
	}

	void PickingPass::DoRenderTransparentStaticSubmeshesNonInstanced( Scene & p_scene, Camera const & p_camera, uint8_t p_index, StaticGeometryRenderNodesByPipelineMap & p_nodes )
	{
		DoRenderNonInstanced< false >( *this, p_scene, p_camera, p_index, p_nodes );
	}

	void PickingPass::DoRenderTransparentAnimatedSubmeshesNonInstanced( Scene & p_scene, Camera const & p_camera, uint8_t p_index, AnimatedGeometryRenderNodesByPipelineMap & p_nodes )
	{
		DoRenderNonInstanced< false >( *this, p_scene, p_camera, p_index, p_nodes );
	}

	void PickingPass::DoRenderTransparentBillboards( Scene & p_scene, Camera const & p_camera, uint8_t p_index, BillboardRenderNodesByPipelineMap & p_nodes )
	{
		DoRenderNonInstanced< false >( *this, p_scene, p_camera, p_index, p_nodes );
	}

	String PickingPass::DoGetGeometryShaderSource(
		FlagCombination< TextureChannel > const & p_textureFlags,
		FlagCombination< ProgramFlag > const & p_programFlags,
		uint8_t p_sceneFlags )const
	{
		return String{};
	}

	String PickingPass::DoGetOpaquePixelShaderSource(
		FlagCombination< TextureChannel > const & p_textureFlags,
		FlagCombination< ProgramFlag > const & p_programFlags,
		uint8_t p_sceneFlags )const
	{
		using namespace GLSL;
		GlslWriter l_writer = m_renderSystem.CreateGlslWriter();

		// UBOs
		UBO_MATRIX( l_writer );
		UBO_SCENE( l_writer );
		UBO_PASS( l_writer );

		Ubo l_uboPicking{ l_writer, Picking };
		auto c3d_iDrawIndex( l_uboPicking.GetUniform< UInt >( DrawIndex ) );
		auto c3d_iNodeIndex( l_uboPicking.GetUniform< UInt >( NodeIndex ) );
		l_uboPicking.End();

		// Fragment Intputs
		auto gl_PrimitiveID( l_writer.GetBuiltin< UInt >( cuT( "gl_PrimitiveID" ) ) );
		auto vtx_instance = l_writer.GetInput< Int >( cuT( "vtx_instance" ) );

		// Fragment Outputs
		auto pxl_v4FragColor( l_writer.GetFragData< Vec4 >( cuT( "pxl_v4FragColor" ), 0 ) );

		l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			pxl_v4FragColor = vec4( c3d_iDrawIndex, c3d_iNodeIndex, vtx_instance, gl_PrimitiveID );
		} );

		return l_writer.Finalise();
	}

	String PickingPass::DoGetTransparentPixelShaderSource(
		FlagCombination< TextureChannel > const & p_textureFlags,
		FlagCombination< ProgramFlag > const & p_programFlags,
		uint8_t p_sceneFlags )const
	{
		return DoGetOpaquePixelShaderSource( p_textureFlags, p_programFlags, p_sceneFlags );
	}

	void PickingPass::DoUpdateOpaquePipeline( Pipeline & p_pipeline, DepthMapArray & p_depthMaps )const
	{
	}

	void PickingPass::DoUpdateTransparentPipeline( Pipeline & p_pipeline, DepthMapArray & p_depthMaps )const
	{
	}

	void PickingPass::DoPrepareOpaqueFrontPipeline( ShaderProgram & p_program, PipelineFlags const & p_flags )
	{
	}

	void PickingPass::DoPrepareOpaqueBackPipeline( ShaderProgram & p_program, PipelineFlags const & p_flags )
	{
		auto l_it = m_backOpaquePipelines.find( p_flags );

		if ( l_it == m_backOpaquePipelines.end() )
		{
			DoUpdateProgram( p_program );
			RasteriserState l_rsState;
			l_rsState.SetCulledFaces( Culling::eBack );
			l_it = m_backOpaquePipelines.emplace( p_flags, GetEngine()->GetRenderSystem()->CreatePipeline( DepthStencilState{}, std::move( l_rsState ), BlendState{}, MultisampleState{}, p_program, p_flags ) ).first;
		}
	}

	void PickingPass::DoPrepareTransparentFrontPipeline( ShaderProgram & p_program, PipelineFlags const & p_flags )
	{
	}

	void PickingPass::DoPrepareTransparentBackPipeline( ShaderProgram & p_program, PipelineFlags const & p_flags )
	{
		auto l_it = m_backTransparentPipelines.find( p_flags );

		if ( l_it == m_backTransparentPipelines.end() )
		{
			DoUpdateProgram( p_program );
			RasteriserState l_rsState;
			l_rsState.SetCulledFaces( Culling::eBack );
			l_it = m_backTransparentPipelines.emplace( p_flags, GetEngine()->GetRenderSystem()->CreatePipeline( DepthStencilState{}, std::move( l_rsState ), BlendState{}, MultisampleState{}, p_program, p_flags ) ).first;
		}
	}

	void PickingPass::DoCompleteProgramFlags( FlagCombination< ProgramFlag > & p_programFlags )const
	{
		AddFlag( p_programFlags, ProgramFlag::ePicking );
	}
}
