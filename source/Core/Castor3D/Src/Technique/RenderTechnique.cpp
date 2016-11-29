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

#include "Event/Frame/FunctorEvent.hpp"
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
#include "Miscellaneous/ShadowMapPass.hpp"
#include "PostEffect/PostEffect.hpp"
#include "Render/Context.hpp"
#include "Render/Pipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Render/RenderTarget.hpp"
#include "Scene/BillboardList.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Geometry.hpp"
#include "Scene/ParticleSystem.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Skybox.hpp"
#include "Scene/Animation/AnimatedMesh.hpp"
#include "Scene/Animation/AnimatedObjectGroup.hpp"
#include "Scene/Animation/AnimatedSkeleton.hpp"
#include "Scene/Animation/Mesh/MeshAnimationInstance.hpp"
#include "Scene/Animation/Mesh/MeshAnimationInstanceSubmesh.hpp"
#include "Shader/FrameVariableBuffer.hpp"
#include "Shader/OneFrameVariable.hpp"
#include "Shader/ShaderProgram.hpp"
#include "State/DepthStencilState.hpp"
#include "State/MultisampleState.hpp"
#include "State/RasteriserState.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureImage.hpp"
#include "Texture/TextureLayout.hpp"

#include <GlslSource.hpp>
#include <GlslShadow.hpp>

using namespace Castor;

namespace Castor3D
{
	namespace
	{
		template< bool Opaque >
		struct PipelineUpdater
		{
			static inline void Update( RenderPass const & p_pass, Camera const & p_camera, DepthMapArray & p_depthMaps, Pipeline & p_pipeline )
			{
				p_pass.UpdateOpaquePipeline( p_camera, p_pipeline, p_depthMaps );
			}
		};

		template<>
		struct PipelineUpdater< false >
		{
			static inline void Update( RenderPass const & p_pass, Camera const & p_camera, DepthMapArray & p_depthMaps, Pipeline & p_pipeline )
			{
				p_pass.UpdateTransparentPipeline( p_camera, p_pipeline, p_depthMaps );
			}
		};

		template< bool Opaque, typename MapType, typename FuncType >
		inline void DoTraverseNodes(
			RenderPass const & p_pass,
			Camera const & p_camera,
			MapType & p_nodes,
			DepthMapArray & p_depthMaps,
			FuncType p_function )
		{
			for ( auto l_itPipelines : p_nodes )
			{
				PipelineUpdater< Opaque >::Update( p_pass, p_camera, p_depthMaps, *l_itPipelines.first );
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
		inline void DoRenderNonInstanced(
			RenderPass const & p_pass,
			Camera const & p_camera,
			MapType & p_nodes,
			DepthMapArray & p_depthMaps,
			bool p_register,
			std::vector< std::reference_wrapper< ObjectRenderNodeBase const > > & p_renderedObjects )
		{
			for ( auto l_itPipelines : p_nodes )
			{
				PipelineUpdater< Opaque >::Update( p_pass, p_camera, p_depthMaps, *l_itPipelines.first );
				l_itPipelines.first->Apply();

				for ( auto & l_renderNode : l_itPipelines.second )
				{
					l_renderNode.Render( p_depthMaps );

					if ( p_register )
					{
						p_renderedObjects.push_back( l_renderNode );
					}
				}
			}
		}

		AnimatedObjectSPtr DoFindAnimatedObject( Scene const & p_scene, String const & p_name )
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
		void DoSortAlpha(
			MapType & p_input,
			Camera const & p_camera,
			RenderTechnique::DistanceSortedNodeMap & p_output )
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

		inline void DoUpdateProgram(
			ShaderProgram & p_program,
			Castor::FlagCombination< ProgramFlag > const & p_programFlags )
		{
			if ( CheckFlag( p_programFlags, ProgramFlag::eShadows )
				 && !p_program.FindFrameVariable< OneIntFrameVariable >( GLSL::Shadow::MapShadow2D, ShaderType::ePixel ) )
			{
				p_program.CreateFrameVariable< OneIntFrameVariable >( GLSL::Shadow::MapShadow2D, ShaderType::ePixel );
				p_program.CreateFrameVariable< OneIntFrameVariable >( GLSL::Shadow::MapShadowCube, ShaderType::ePixel );
			}
		}

		void DoFillShaderDepthMaps( Pipeline & p_pipeline, DepthMapArray & p_depthMaps )
		{
			if ( !p_depthMaps.empty() )
			{
				auto l_index = p_pipeline.GetTexturesCount() + 1;
				auto & l_spot = p_pipeline.GetSpotShadowMapsVariable();
				auto & l_point = p_pipeline.GetPointShadowMapsVariable();

				for ( auto & l_depthMap : p_depthMaps )
				{
					if ( l_depthMap.get().GetType() == TextureType::eTwoDimensions )
					{
						l_depthMap.get().SetIndex( l_index );
						l_spot.SetValue( l_index++ );
					}
					else if ( l_depthMap.get().GetType() == TextureType::eTwoDimensionsArray )
					{
						l_depthMap.get().SetIndex( l_index );
						l_spot.SetValue( l_index++ );
					}
					else if ( l_depthMap.get().GetType() == TextureType::eCube )
					{
						l_depthMap.get().SetIndex( l_index );
						l_point.SetValue( l_index++ );
					}
					else if ( l_depthMap.get().GetType() == TextureType::eCubeArray )
					{
						l_depthMap.get().SetIndex( l_index );
						l_point.SetValue( l_index++ );
					}
				}
			}
		}

		inline BlendState DoCreateBlendState( BlendMode p_colourBlendMode, BlendMode p_alphaBlendMode )
		{
			BlendState l_state;
			bool l_blend = false;

			switch ( p_colourBlendMode )
			{
			case BlendMode::eNoBlend:
				l_state.SetRgbSrcBlend( BlendOperand::eOne );
				l_state.SetRgbDstBlend( BlendOperand::eZero );
				break;

			case BlendMode::eAdditive:
				l_blend = true;
				l_state.SetRgbSrcBlend( BlendOperand::eOne );
				l_state.SetRgbDstBlend( BlendOperand::eOne );
				break;

			case BlendMode::eMultiplicative:
				l_blend = true;
				l_state.SetRgbSrcBlend( BlendOperand::eZero );
				l_state.SetRgbDstBlend( BlendOperand::eInvSrcColour );
				break;

			case BlendMode::eInterpolative:
				l_blend = true;
				l_state.SetRgbSrcBlend( BlendOperand::eSrcColour );
				l_state.SetRgbDstBlend( BlendOperand::eInvSrcColour );
				break;

			default:
				l_blend = true;
				l_state.SetRgbSrcBlend( BlendOperand::eSrcColour );
				l_state.SetRgbDstBlend( BlendOperand::eInvSrcColour );
				break;
			}

			switch ( p_alphaBlendMode )
			{
			case BlendMode::eNoBlend:
				l_state.SetAlphaSrcBlend( BlendOperand::eOne );
				l_state.SetAlphaDstBlend( BlendOperand::eZero );
				break;

			case BlendMode::eAdditive:
				l_blend = true;
				l_state.SetAlphaSrcBlend( BlendOperand::eOne );
				l_state.SetAlphaDstBlend( BlendOperand::eOne );
				break;

			case BlendMode::eMultiplicative:
				l_blend = true;
				l_state.SetAlphaSrcBlend( BlendOperand::eZero );
				l_state.SetAlphaDstBlend( BlendOperand::eInvSrcAlpha );
				l_state.SetRgbSrcBlend( BlendOperand::eZero );
				l_state.SetRgbDstBlend( BlendOperand::eInvSrcAlpha );
				break;

			case BlendMode::eInterpolative:
				l_blend = true;
				l_state.SetAlphaSrcBlend( BlendOperand::eSrcAlpha );
				l_state.SetAlphaDstBlend( BlendOperand::eInvSrcAlpha );
				l_state.SetRgbSrcBlend( BlendOperand::eSrcAlpha );
				l_state.SetRgbDstBlend( BlendOperand::eInvSrcAlpha );
				break;

			default:
				l_blend = true;
				l_state.SetAlphaSrcBlend( BlendOperand::eSrcAlpha );
				l_state.SetAlphaDstBlend( BlendOperand::eInvSrcAlpha );
				l_state.SetRgbSrcBlend( BlendOperand::eSrcAlpha );
				l_state.SetRgbDstBlend( BlendOperand::eInvSrcAlpha );
				break;
			}

			l_state.EnableBlend( l_blend );
			return l_state;
		}
	}

	//*************************************************************************************************

	RenderTechnique::stFRAME_BUFFER::stFRAME_BUFFER( RenderTechnique & p_technique )
		: m_technique{ p_technique }
	{
	}

	bool RenderTechnique::stFRAME_BUFFER::Initialise( Size p_size )
	{
		m_colourTexture = m_technique.GetEngine()->GetRenderSystem()->CreateTexture( TextureType::eTwoDimensions, AccessType::eRead, AccessType::eRead | AccessType::eWrite, PixelFormat::eRGBA16F32F, p_size );
		m_colourTexture->GetImage().InitialiseSource();
		p_size = m_colourTexture->GetDimensions();

		bool l_return = m_colourTexture->Initialise();

		if ( l_return )
		{
			m_frameBuffer = m_technique.GetEngine()->GetRenderSystem()->CreateFrameBuffer();
			m_depthBuffer = m_frameBuffer->CreateDepthStencilRenderBuffer( PixelFormat::eD32F );
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

	void RenderTechnique::stFRAME_BUFFER::Cleanup()
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
	}

	//*************************************************************************************************

	RenderTechnique::RenderTechnique( String const & p_name, RenderTarget & p_renderTarget, RenderSystem & p_renderSystem, Parameters const & CU_PARAM_UNUSED( p_params ), bool p_multisampling )
		: RenderPass{ p_name, *p_renderSystem.GetEngine(), p_multisampling }
		, m_renderTarget{ p_renderTarget }
		, m_initialised{ false }
		, m_frameBuffer{ *this }
		, m_spotShadowMap{ *p_renderTarget.GetEngine() }
		, m_pointShadowMap{ *p_renderTarget.GetEngine() }
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

			if ( m_initialised )
			{
				m_initialised = DoInitialiseSpotShadowMap( Size{ 1024, 1024 } );
			}

			if ( m_initialised )
			{
				m_initialised = DoInitialisePointShadowMap( Size{ 512, 512 } );
			}

			auto & l_scene = *m_renderTarget.GetScene();

			if (m_initialised)
			{
				l_scene.GetLightCache().ForEach( [&l_scene, this]( Light & p_light )
				{
					if ( p_light.IsShadowProducer()
						 && p_light.GetLightType() != LightType::eDirectional )
					{
						TextureUnit * l_unit{ nullptr };

						switch ( p_light.GetLightType() )
						{
						case LightType::ePoint:
							l_unit = &m_pointShadowMap;
							break;

						case LightType::eSpot:
							l_unit = &m_spotShadowMap;
							break;
						}

						auto l_pass = GetEngine()->GetShadowMapPassFactory().Create( p_light.GetLightType(), *GetEngine(), l_scene, p_light, *l_unit, 0u );
						auto l_insit = m_shadowMaps.insert( { &p_light, l_pass } ).first;
						l_pass->Initialise( m_size );
					}
				} );
			}

			if ( m_initialised )
			{
				auto l_camera = m_renderTarget.GetCamera();

				if ( l_camera )
				{
					m_renderQueue.Initialise( l_scene, *m_renderTarget.GetCamera() );
				}
				else
				{
					m_renderQueue.Initialise( l_scene );
				}
			}
		}

		return m_initialised;
	}

	void RenderTechnique::Cleanup()
	{
		DoCleanupPointShadowMap();
		DoCleanupSpotShadowMap();
		m_initialised = false;
		m_frameBuffer.Cleanup();
		DoCleanup();

		for ( auto & l_it: m_shadowMaps )
		{
			l_it.second->Cleanup();
		}
	}

	void RenderTechnique::Update()
	{
		m_renderQueue.Update();

		for ( auto & l_it: m_shadowMaps )
		{
			l_it.second->Update();
		}
	}

	void RenderTechnique::Render( uint32_t p_frameTime, uint32_t & p_visible, uint32_t & p_particles )
	{
		auto & l_nodes = m_renderQueue.GetRenderNodes();
		auto & l_scene = *m_renderTarget.GetScene();
		l_scene.GetLightCache().UpdateLights();

		m_renderSystem.PushScene( &l_scene );
		DepthMapArray l_depthMaps;

		if ( l_scene.HasShadows() )
		{
			l_depthMaps.push_back( std::ref( m_spotShadowMap ) );
			l_depthMaps.push_back( std::ref( m_pointShadowMap ) );

			for ( auto & l_shadowMap : m_shadowMaps )
			{
				l_shadowMap.second->Render();
			}
		}

		if ( DoBeginRender() )
		{
			l_scene.RenderBackground( GetSize() );
			DoRender( l_nodes, l_depthMaps, p_frameTime );
			p_visible = uint32_t( m_renderedObjects.size() );
			p_particles = m_particlesCount;

#if !defined( NDEBUG )

			if ( !m_shadowMaps.empty() )
			{
				auto l_it = m_shadowMaps.begin();
				auto & l_depthMap = l_it->second->GetShadowMap();
				auto l_size = l_depthMap.GetTexture()->GetDimensions();
				auto l_lightNode = l_it->first->GetParent();

				switch ( l_depthMap.GetType() )
				{
				case TextureType::eTwoDimensions:
					m_renderSystem.GetCurrentContext()->RenderDepth( Size{ l_size.width() / 4, l_size.height() / 4 }, *l_depthMap.GetTexture() );
					break;

				case TextureType::eTwoDimensionsArray:
					m_renderSystem.GetCurrentContext()->RenderDepth( Size{ l_size.width() / 4, l_size.height() / 4 }, *l_depthMap.GetTexture(), 0u );
					break;

				case TextureType::eCube:
					m_renderSystem.GetCurrentContext()->RenderDepth( l_lightNode->GetDerivedPosition(), l_lightNode->GetDerivedOrientation(), Size{ l_size.width() / 2, l_size.height() / 2 }, *l_depthMap.GetTexture() );
					break;

				case TextureType::eCubeArray:
					m_renderSystem.GetCurrentContext()->RenderDepth( l_lightNode->GetDerivedPosition(), l_lightNode->GetDerivedOrientation(), Size{ l_size.width() / 2, l_size.height() / 2 }, *l_depthMap.GetTexture(), 0u );
					break;
				}
			}

#endif

			DoEndRender();
		}

		for ( auto l_effect : m_renderTarget.GetPostEffects() )
		{
			l_effect->Apply( *m_frameBuffer.m_frameBuffer );
		}

		m_renderSystem.PopScene();
	}

	bool RenderTechnique::WriteInto( Castor::TextFile & p_file )
	{
		return DoWriteInto( p_file );
	}

	void RenderTechnique::DoRender( SceneRenderNodes & p_nodes, DepthMapArray & p_depthMaps, uint32_t p_frameTime )
	{
		m_renderedObjects.clear();
		m_particlesCount = 0;
		auto & l_camera = *m_renderTarget.GetCamera();
		l_camera.Resize( m_size );
		l_camera.Update();
		l_camera.Apply();

		DoRenderOpaqueNodes( p_nodes, p_depthMaps );

		if ( p_nodes.m_scene.GetFog().GetType() == FogType::eDisabled )
		{
			p_nodes.m_scene.RenderForeground( GetSize(), l_camera );
		}

		p_nodes.m_scene.GetParticleSystemCache().ForEach( [this]( ParticleSystem & p_particleSystem )
		{
			p_particleSystem.Update();
			m_particlesCount += p_particleSystem.GetParticlesCount();
		} );

		DoRenderTransparentNodes( p_nodes, p_depthMaps );
	}

	void RenderTechnique::DoRenderOpaqueNodes( SceneRenderNodes & p_nodes, DepthMapArray & p_depthMaps )
	{
		DoBeginOpaqueRendering();

		if ( !p_nodes.m_staticGeometries.m_opaqueRenderNodesBack.empty()
			 || !p_nodes.m_instancedGeometries.m_opaqueRenderNodesBack.empty()
			 || !p_nodes.m_animatedGeometries.m_opaqueRenderNodesBack.empty()
			 || !p_nodes.m_billboards.m_opaqueRenderNodesBack.empty() )
		{
			DoRenderOpaqueInstancedSubmeshesInstanced( p_nodes.m_instancedGeometries.m_opaqueRenderNodesFront, p_depthMaps );
			DoRenderOpaqueStaticSubmeshesNonInstanced( p_nodes.m_staticGeometries.m_opaqueRenderNodesFront, p_depthMaps );
			DoRenderOpaqueAnimatedSubmeshesNonInstanced( p_nodes.m_animatedGeometries.m_opaqueRenderNodesFront, p_depthMaps );
			DoRenderOpaqueBillboards( p_nodes.m_billboards.m_opaqueRenderNodesFront, p_depthMaps );

			DoRenderOpaqueInstancedSubmeshesInstanced( p_nodes.m_instancedGeometries.m_opaqueRenderNodesBack, p_depthMaps );
			DoRenderOpaqueStaticSubmeshesNonInstanced( p_nodes.m_staticGeometries.m_opaqueRenderNodesBack, p_depthMaps );
			DoRenderOpaqueAnimatedSubmeshesNonInstanced( p_nodes.m_animatedGeometries.m_opaqueRenderNodesBack, p_depthMaps );
			DoRenderOpaqueBillboards( p_nodes.m_billboards.m_opaqueRenderNodesBack, p_depthMaps );
		}

		DoEndOpaqueRendering();
	}

	void RenderTechnique::DoRenderTransparentNodes( SceneRenderNodes & p_nodes, DepthMapArray & p_depthMaps )
	{
		DoBeginTransparentRendering();

		if ( !p_nodes.m_staticGeometries.m_transparentRenderNodesFront.empty()
			|| !p_nodes.m_animatedGeometries.m_transparentRenderNodesFront.empty()
			|| !p_nodes.m_billboards.m_transparentRenderNodesFront.empty() )
		{
			if ( m_multisampling )
			{
				DoRenderTransparentInstancedSubmeshesInstanced( p_nodes.m_instancedGeometries.m_transparentRenderNodesFront, p_depthMaps, false );
				DoRenderTransparentStaticSubmeshesNonInstanced( p_nodes.m_staticGeometries.m_transparentRenderNodesFront, p_depthMaps, false );
				DoRenderTransparentAnimatedSubmeshesNonInstanced( p_nodes.m_animatedGeometries.m_transparentRenderNodesFront, p_depthMaps, false );
				DoRenderTransparentBillboards( p_nodes.m_billboards.m_transparentRenderNodesFront, p_depthMaps, false );

				DoRenderTransparentInstancedSubmeshesInstanced( p_nodes.m_instancedGeometries.m_transparentRenderNodesBack, p_depthMaps, true );
				DoRenderTransparentStaticSubmeshesNonInstanced( p_nodes.m_staticGeometries.m_transparentRenderNodesBack, p_depthMaps, true );
				DoRenderTransparentAnimatedSubmeshesNonInstanced( p_nodes.m_animatedGeometries.m_transparentRenderNodesBack, p_depthMaps, true );
				DoRenderTransparentBillboards( p_nodes.m_billboards.m_transparentRenderNodesBack, p_depthMaps, true );
			}
			else
			{
				{
					DistanceSortedNodeMap l_distanceSortedRenderNodes;
					DoSortAlpha( p_nodes.m_staticGeometries.m_transparentRenderNodesFront, *m_renderTarget.GetCamera(), l_distanceSortedRenderNodes );
					DoSortAlpha( p_nodes.m_animatedGeometries.m_transparentRenderNodesFront, *m_renderTarget.GetCamera(), l_distanceSortedRenderNodes );
					DoSortAlpha( p_nodes.m_billboards.m_transparentRenderNodesFront, *m_renderTarget.GetCamera(), l_distanceSortedRenderNodes );

					if ( !l_distanceSortedRenderNodes.empty() )
					{
						DoRenderByDistance( l_distanceSortedRenderNodes, p_depthMaps, false );
					}
				}
				{
					DistanceSortedNodeMap l_distanceSortedRenderNodes;
					DoSortAlpha( p_nodes.m_staticGeometries.m_transparentRenderNodesBack, *m_renderTarget.GetCamera(), l_distanceSortedRenderNodes );
					DoSortAlpha( p_nodes.m_animatedGeometries.m_transparentRenderNodesBack, *m_renderTarget.GetCamera(), l_distanceSortedRenderNodes );
					DoSortAlpha( p_nodes.m_billboards.m_transparentRenderNodesBack, *m_renderTarget.GetCamera(), l_distanceSortedRenderNodes );

					if ( !l_distanceSortedRenderNodes.empty() )
					{
						DoRenderByDistance( l_distanceSortedRenderNodes, p_depthMaps, false );
					}
				}
			}
		}

		DoEndTransparentRendering();
	}

	void RenderTechnique::DoRenderOpaqueStaticSubmeshesNonInstanced( StaticGeometryRenderNodesByPipelineMap & p_nodes, DepthMapArray & p_depthMaps, bool p_register )
	{
		DoRenderNonInstanced< true >( *this, *m_renderTarget.GetCamera(), p_nodes, p_depthMaps, p_register, m_renderedObjects );
	}

	void RenderTechnique::DoRenderOpaqueAnimatedSubmeshesNonInstanced( AnimatedGeometryRenderNodesByPipelineMap & p_nodes, DepthMapArray & p_depthMaps, bool p_register )
	{
		DoRenderNonInstanced< true >( *this, *m_renderTarget.GetCamera(), p_nodes, p_depthMaps, p_register, m_renderedObjects );
	}

	void RenderTechnique::DoRenderOpaqueInstancedSubmeshesNonInstanced( SubmeshStaticRenderNodesByPipelineMap & p_nodes, DepthMapArray & p_depthMaps, bool p_register )
	{
		DoTraverseNodes< true >(
			*this,
			*m_renderTarget.GetCamera(),
			p_nodes,
			p_depthMaps,
			[&p_depthMaps, &p_register, this](
				Pipeline & p_pipeline,
				Pass & p_pass,
				Submesh & p_submesh,
				StaticGeometryRenderNodeArray & p_renderNodes )
		{
			for ( auto & l_renderNode : p_renderNodes )
			{
				l_renderNode.Render( p_depthMaps );

				if ( p_register )
				{
					m_renderedObjects.push_back( l_renderNode );
				}
			}
		} );
	}

	void RenderTechnique::DoRenderOpaqueInstancedSubmeshesInstanced( SubmeshStaticRenderNodesByPipelineMap & p_nodes, DepthMapArray & p_depthMaps, bool p_register )
	{
		DoTraverseNodes< true >(
			*this,
			*m_renderTarget.GetCamera(),
			p_nodes,
			p_depthMaps,
			[&p_depthMaps, &p_register, this](
				Pipeline & p_pipeline,
				Pass & p_pass,
				Submesh & p_submesh,
				StaticGeometryRenderNodeArray & p_renderNodes )
		{
			if ( !p_renderNodes.empty() && p_submesh.HasMatrixBuffer() )
			{
				uint32_t l_count = uint32_t( p_renderNodes.size() );
				auto & l_matrixBuffer = p_submesh.GetMatrixBuffer();

				if ( l_matrixBuffer.Bind() )
				{
					real * l_buffer = reinterpret_cast< real * >( l_matrixBuffer.Lock( 0, l_count * 16 * sizeof( real ), AccessType::eWrite ) );

					if ( l_buffer )
					{
						constexpr uint32_t l_stride = 16;

						for ( auto const & l_renderNode : p_renderNodes )
						{
							std::memcpy( l_buffer, l_renderNode.m_sceneNode.GetDerivedTransformationMatrix().const_ptr(), l_stride * sizeof( real ) );
							l_buffer += l_stride;

							if ( p_register )
							{
								m_renderedObjects.push_back( l_renderNode );
							}
						}

						l_matrixBuffer.Unlock();
					}

					l_matrixBuffer.Unbind();
					p_renderNodes[0].BindPass( p_depthMaps, MASK_MTXMODE_MODEL );
					p_submesh.DrawInstanced( p_renderNodes[0].m_buffers, l_count );
					p_renderNodes[0].UnbindPass( p_depthMaps );
				}
			}
		} );
	}

	void RenderTechnique::DoRenderTransparentStaticSubmeshesNonInstanced( StaticGeometryRenderNodesByPipelineMap & p_nodes, DepthMapArray & p_depthMaps, bool p_register )
	{
		DoRenderNonInstanced< false >( *this, *m_renderTarget.GetCamera(), p_nodes, p_depthMaps, p_register, m_renderedObjects );
	}

	void RenderTechnique::DoRenderTransparentAnimatedSubmeshesNonInstanced( AnimatedGeometryRenderNodesByPipelineMap & p_nodes, DepthMapArray & p_depthMaps, bool p_register )
	{
		DoRenderNonInstanced< false >( *this, *m_renderTarget.GetCamera(), p_nodes, p_depthMaps, p_register, m_renderedObjects );
	}

	void RenderTechnique::DoRenderTransparentInstancedSubmeshesNonInstanced( SubmeshStaticRenderNodesByPipelineMap & p_nodes, DepthMapArray & p_depthMaps, bool p_register )
	{
		DoTraverseNodes< false >(
			*this,
			*m_renderTarget.GetCamera(),
			p_nodes,
			p_depthMaps,
			[&p_depthMaps, &p_register, this](
				Pipeline & p_pipeline,
				Pass & p_pass,
				Submesh & p_submesh,
				StaticGeometryRenderNodeArray & p_renderNodes )
		{
			for ( auto & l_renderNode : p_renderNodes )
			{
				l_renderNode.Render( p_depthMaps );

				if ( p_register )
				{
					m_renderedObjects.push_back( l_renderNode );
				}
			}
		} );
	}

	void RenderTechnique::DoRenderTransparentInstancedSubmeshesInstanced( SubmeshStaticRenderNodesByPipelineMap & p_nodes, DepthMapArray & p_depthMaps, bool p_register )
	{
		DoTraverseNodes< false >(
			*this,
			*m_renderTarget.GetCamera(),
			p_nodes,
			p_depthMaps,
			[&p_depthMaps, &p_register, this](
				Pipeline & p_pipeline,
				Pass & p_pass,
				Submesh & p_submesh,
				StaticGeometryRenderNodeArray & p_renderNodes )
		{
			if ( !p_renderNodes.empty() && p_submesh.HasMatrixBuffer() )
			{
				uint32_t l_count = uint32_t( p_renderNodes.size() );
				auto & l_matrixBuffer = p_submesh.GetMatrixBuffer();

				if ( l_matrixBuffer.Bind() )
				{
					real * l_buffer = reinterpret_cast< real * >( l_matrixBuffer.Lock( 0, l_count * 16 * sizeof( real ), AccessType::eWrite ) );

					if ( l_buffer )
					{
						constexpr uint32_t l_stride = 16;

						for ( auto const & l_renderNode : p_renderNodes )
						{
							std::memcpy( l_buffer, l_renderNode.m_sceneNode.GetDerivedTransformationMatrix().const_ptr(), l_stride * sizeof( real ) );
							l_buffer += l_stride;

							if ( p_register )
							{
								m_renderedObjects.push_back( l_renderNode );
							}
						}

						l_matrixBuffer.Unlock();
					}

					l_matrixBuffer.Unbind();
					p_renderNodes[0].BindPass( p_depthMaps, MASK_MTXMODE_MODEL );
					p_submesh.DrawInstanced( p_renderNodes[0].m_buffers, l_count );
					p_renderNodes[0].UnbindPass( p_depthMaps );
				}
			}
		} );
	}

	void RenderTechnique::DoRenderByDistance( DistanceSortedNodeMap & p_nodes, DepthMapArray & p_depthMaps, bool p_register )
	{
		for ( auto & l_it : p_nodes )
		{
			l_it.second.get().m_pass.m_pipeline.Apply();
			UpdateTransparentPipeline( *m_renderTarget.GetCamera(), l_it.second.get().m_pass.m_pipeline, p_depthMaps );
			l_it.second.get().Render( p_depthMaps );

			if ( p_register )
			{
				m_renderedObjects.push_back( l_it.second.get() );
			}
		}
	}

	void RenderTechnique::DoRenderOpaqueBillboards( BillboardRenderNodesByPipelineMap & p_nodes, DepthMapArray & p_depthMaps, bool p_register )
	{
		DoRenderNonInstanced< true >( *this, *m_renderTarget.GetCamera(), p_nodes, p_depthMaps, p_register, m_renderedObjects );
	}

	void RenderTechnique::DoRenderTransparentBillboards( BillboardRenderNodesByPipelineMap & p_nodes, DepthMapArray & p_depthMaps, bool p_register )
	{
		DoRenderNonInstanced< false >( *this, *m_renderTarget.GetCamera(), p_nodes, p_depthMaps, p_register, m_renderedObjects );
	}

	void RenderTechnique::DoCompleteProgramFlags( FlagCombination< ProgramFlag > & p_programFlags )const
	{
		AddFlag( p_programFlags, ProgramFlag::eLighting );
	}

	String RenderTechnique::DoGetGeometryShaderSource(
		FlagCombination< TextureChannel > const & p_textureFlags,
		FlagCombination< ProgramFlag > const & p_programFlags,
		uint8_t p_sceneFlags )const
	{
		return String{};
	}

	String RenderTechnique::DoGetOpaquePixelShaderSource(
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

		// Fragment Intputs
		auto vtx_worldSpacePosition = l_writer.GetInput< Vec3 >( cuT( "vtx_worldSpacePosition" ) );
		auto vtx_normal = l_writer.GetInput< Vec3 >( cuT( "vtx_normal" ) );
		auto vtx_tangent = l_writer.GetInput< Vec3 >( cuT( "vtx_tangent" ) );
		auto vtx_bitangent = l_writer.GetInput< Vec3 >( cuT( "vtx_bitangent" ) );
		auto vtx_texture = l_writer.GetInput< Vec3 >( cuT( "vtx_texture" ) );
		auto vtx_instance = l_writer.GetInput< Int >( cuT( "vtx_instance" ) );

		if ( l_writer.HasTextureBuffers() )
		{
			auto c3d_sLights = l_writer.GetUniform< SamplerBuffer >( cuT( "c3d_sLights" ) );
		}
		else
		{
			auto c3d_sLights = l_writer.GetUniform< Sampler1D >( cuT( "c3d_sLights" ) );
		}

		auto c3d_mapColour( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapColour, CheckFlag( p_textureFlags, TextureChannel::eColour ) ) );
		auto c3d_mapAmbient( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapAmbient, CheckFlag( p_textureFlags, TextureChannel::eAmbient ) ) );
		auto c3d_mapDiffuse( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapDiffuse, CheckFlag( p_textureFlags, TextureChannel::eDiffuse ) ) );
		auto c3d_mapNormal( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapNormal, CheckFlag( p_textureFlags, TextureChannel::eNormal ) ) );
		auto c3d_mapSpecular( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapSpecular, CheckFlag( p_textureFlags, TextureChannel::eSpecular ) ) );
		auto c3d_mapEmissive( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapEmissive, CheckFlag( p_textureFlags, TextureChannel::eEmissive ) ) );
		auto c3d_mapHeight( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapHeight, CheckFlag( p_textureFlags, TextureChannel::eHeight ) ) );
		auto c3d_mapGloss( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapGloss, CheckFlag( p_textureFlags, TextureChannel::eGloss ) ) );

		auto gl_FragCoord( l_writer.GetBuiltin< Vec4 >( cuT( "gl_FragCoord" ) ) );

		auto l_lighting = l_writer.CreateLightingModel( PhongLightingModel::Name, CheckFlag( p_programFlags, ProgramFlag::eShadows ) ? ShadowType::ePoisson : ShadowType::eNone );
		GLSL::Fog l_fog{ p_sceneFlags, l_writer };

		// Fragment Outtputs
		auto pxl_v4FragColor( l_writer.GetFragData< Vec4 >( cuT( "pxl_v4FragColor" ), 0 ) );

		l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			auto l_v3Normal = l_writer.GetLocale( cuT( "l_v3Normal" ), normalize( vec3( vtx_normal.x(), vtx_normal.y(), vtx_normal.z() ) ) );
			auto l_v3Ambient = l_writer.GetLocale( cuT( "l_v3Ambient" ), c3d_v4AmbientLight.xyz() );
			auto l_v3Diffuse = l_writer.GetLocale( cuT( "l_v3Diffuse" ), vec3( 0.0_f, 0, 0 ) );
			auto l_v3Specular = l_writer.GetLocale( cuT( "l_v3Specular" ), vec3( 0.0_f, 0, 0 ) );
			auto l_fMatShininess = l_writer.GetLocale( cuT( "l_fMatShininess" ), c3d_fMatShininess );
			auto l_v3Emissive = l_writer.GetLocale( cuT( "l_v3Emissive" ), c3d_v4MatEmissive.xyz() );
			auto l_worldEye = l_writer.GetLocale( cuT( "l_worldEye" ), vec3( c3d_v3CameraPosition.x(), c3d_v3CameraPosition.y(), c3d_v3CameraPosition.z() ) );
			pxl_v4FragColor = vec4( 0.0_f, 0.0f, 0.0f, 0.0f );

			ComputePreLightingMapContributions( l_writer, l_v3Normal, l_fMatShininess, p_textureFlags, p_programFlags, p_sceneFlags );

			OutputComponents l_output { l_v3Ambient, l_v3Diffuse, l_v3Specular };
			l_lighting->ComputeCombinedLighting(
				l_worldEye,
				l_fMatShininess,
				FragmentInput { vtx_worldSpacePosition, l_v3Normal },
				l_output );

			ComputePostLightingMapContributions( l_writer, l_v3Ambient, l_v3Diffuse, l_v3Specular, l_v3Emissive, p_textureFlags, p_programFlags, p_sceneFlags );

			pxl_v4FragColor = vec4(
				l_writer.Paren( l_v3Ambient * c3d_v4MatAmbient.xyz() ) +
				l_writer.Paren( l_v3Diffuse * c3d_v4MatDiffuse.xyz() ) +
				l_writer.Paren( l_v3Specular * c3d_v4MatSpecular.xyz() ) +
				l_v3Emissive,
				1.0 );

			if ( p_sceneFlags != 0 )
			{
				auto l_wvPosition = l_writer.GetLocale( cuT( "l_wvPosition" ), l_writer.Paren( c3d_mtxView * vec4( vtx_worldSpacePosition, 1.0 ) ).xyz() );
				l_fog.ApplyFog( pxl_v4FragColor, length( l_wvPosition ), l_wvPosition.y() );
			}
		} );

		return l_writer.Finalise();
	}

	String RenderTechnique::DoGetTransparentPixelShaderSource(
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

		// Fragment Intputs
		auto vtx_worldSpacePosition = l_writer.GetInput< Vec3 >( cuT( "vtx_worldSpacePosition" ) );
		auto vtx_normal = l_writer.GetInput< Vec3 >( cuT( "vtx_normal" ) );
		auto vtx_tangent = l_writer.GetInput< Vec3 >( cuT( "vtx_tangent" ) );
		auto vtx_bitangent = l_writer.GetInput< Vec3 >( cuT( "vtx_bitangent" ) );
		auto vtx_texture = l_writer.GetInput< Vec3 >( cuT( "vtx_texture" ) );
		auto vtx_instance = l_writer.GetInput< Int >( cuT( "vtx_instance" ) );

		if ( l_writer.HasTextureBuffers() )
		{
			auto c3d_sLights = l_writer.GetUniform< SamplerBuffer >( cuT( "c3d_sLights" ) );
		}
		else
		{
			auto c3d_sLights = l_writer.GetUniform< Sampler1D >( cuT( "c3d_sLights" ) );
		}

		auto c3d_mapColour( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapColour, CheckFlag( p_textureFlags, TextureChannel::eColour ) ) );
		auto c3d_mapAmbient( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapAmbient, CheckFlag( p_textureFlags, TextureChannel::eAmbient ) ) );
		auto c3d_mapDiffuse( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapDiffuse, CheckFlag( p_textureFlags, TextureChannel::eDiffuse ) ) );
		auto c3d_mapNormal( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapNormal, CheckFlag( p_textureFlags, TextureChannel::eNormal ) ) );
		auto c3d_mapOpacity( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapOpacity, CheckFlag( p_textureFlags, TextureChannel::eOpacity ) ) );
		auto c3d_mapSpecular( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapSpecular, CheckFlag( p_textureFlags, TextureChannel::eSpecular ) ) );
		auto c3d_mapEmissive( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapEmissive, CheckFlag( p_textureFlags, TextureChannel::eEmissive ) ) );
		auto c3d_mapHeight( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapHeight, CheckFlag( p_textureFlags, TextureChannel::eHeight ) ) );
		auto c3d_mapGloss( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapGloss, CheckFlag( p_textureFlags, TextureChannel::eGloss ) ) );

		auto gl_FragCoord( l_writer.GetBuiltin< Vec4 >( cuT( "gl_FragCoord" ) ) );

		auto l_lighting = l_writer.CreateLightingModel( PhongLightingModel::Name, CheckFlag( p_programFlags, ProgramFlag::eShadows ) ? ShadowType::ePoisson : ShadowType::eNone );
		GLSL::Fog l_fog{ p_sceneFlags, l_writer };

		// Fragment Outputs
		auto pxl_v4FragColor( l_writer.GetFragData< Vec4 >( cuT( "pxl_v4FragColor" ), 0 ) );

		l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			auto l_v3Normal = l_writer.GetLocale( cuT( "l_v3Normal" ), normalize( vec3( vtx_normal.x(), vtx_normal.y(), vtx_normal.z() ) ) );
			auto l_v3Ambient = l_writer.GetLocale( cuT( "l_v3Ambient" ), c3d_v4AmbientLight.xyz() );
			auto l_v3Diffuse = l_writer.GetLocale( cuT( "l_v3Diffuse" ), vec3( 0.0_f, 0, 0 ) );
			auto l_v3Specular = l_writer.GetLocale( cuT( "l_v3Specular" ), vec3( 0.0_f, 0, 0 ) );
			auto l_fAlpha = l_writer.GetLocale( cuT( "l_fAlpha" ), c3d_fMatOpacity );
			auto l_fMatShininess = l_writer.GetLocale( cuT( "l_fMatShininess" ), c3d_fMatShininess );
			auto l_v3Emissive = l_writer.GetLocale( cuT( "l_v3Emissive" ), c3d_v4MatEmissive.xyz() );
			auto l_worldEye = l_writer.GetLocale( cuT( "l_worldEye" ), vec3( c3d_v3CameraPosition.x(), c3d_v3CameraPosition.y(), c3d_v3CameraPosition.z() ) );
			pxl_v4FragColor = vec4( 0.0_f, 0.0f, 0.0f, 0.0f );
			Vec3 l_v3MapNormal( &l_writer, cuT( "l_v3MapNormal" ) );

			ComputePreLightingMapContributions( l_writer, l_v3Normal, l_fMatShininess, p_textureFlags, p_programFlags, p_sceneFlags );

			OutputComponents l_output{ l_v3Ambient, l_v3Diffuse, l_v3Specular };
			l_lighting->ComputeCombinedLighting(
				l_worldEye,
				l_fMatShininess,
				FragmentInput( vtx_worldSpacePosition, l_v3Normal ),
				l_output );

			ComputePostLightingMapContributions( l_writer, l_v3Ambient, l_v3Diffuse, l_v3Specular, l_v3Emissive, p_textureFlags, p_programFlags, p_sceneFlags );

			if ( CheckFlag( p_textureFlags, TextureChannel::eOpacity ) )
			{
				l_fAlpha = texture( c3d_mapOpacity, vtx_texture.xy() ).r() * c3d_fMatOpacity;
			}

			pxl_v4FragColor = vec4( l_fAlpha * l_writer.Paren(
				l_writer.Paren(
					l_v3Ambient * c3d_v4MatAmbient.xyz() ) +
					l_writer.Paren( l_v3Diffuse * c3d_v4MatDiffuse.xyz() ) +
					l_writer.Paren( l_v3Specular * c3d_v4MatSpecular.xyz() ) +
					l_v3Emissive ),
				l_fAlpha );

			if ( p_sceneFlags != 0 )
			{
				auto l_wvPosition = l_writer.GetLocale( cuT( "l_wvPosition" ), l_writer.Paren( c3d_mtxView * vec4( vtx_worldSpacePosition, 1.0 ) ).xyz() );
				l_fog.ApplyFog( pxl_v4FragColor, length( l_wvPosition ), l_wvPosition.y() );
			}
		} );

		return l_writer.Finalise();
	}

	void RenderTechnique::DoUpdateOpaquePipeline( Pipeline & p_pipeline, DepthMapArray & p_depthMaps )const
	{
		auto & l_sceneUbo = p_pipeline.GetSceneUbo();
		auto & l_camera = *m_renderTarget.GetCamera();
		l_camera.GetScene()->GetLightCache().FillShader( l_sceneUbo );
		l_camera.GetScene()->GetFog().FillShader( l_sceneUbo );
		l_camera.GetScene()->FillShader( l_sceneUbo );
		DoFillShaderDepthMaps( p_pipeline, p_depthMaps );
	}

	void RenderTechnique::DoUpdateTransparentPipeline( Pipeline & p_pipeline, DepthMapArray & p_depthMaps )const
	{
		auto & l_sceneUbo = p_pipeline.GetSceneUbo();
		auto & l_camera = *m_renderTarget.GetCamera();
		l_camera.GetScene()->GetLightCache().FillShader( l_sceneUbo );
		l_camera.GetScene()->GetFog().FillShader( l_sceneUbo );
		l_camera.GetScene()->FillShader( l_sceneUbo );
		DoFillShaderDepthMaps( p_pipeline, p_depthMaps );

	}

	void RenderTechnique::DoPrepareOpaqueFrontPipeline( ShaderProgram & p_program, PipelineFlags const & p_flags )
	{
		auto l_it = m_frontOpaquePipelines.find( p_flags );

		if ( l_it == m_frontOpaquePipelines.end() )
		{
			DoUpdateProgram( p_program, p_flags.m_programFlags );
			RasteriserState l_rsState;
			l_rsState.SetCulledFaces( Culling::eFront );
			MultisampleState l_msState;
			l_msState.SetMultisample( m_multisampling );
			l_it = m_frontOpaquePipelines.emplace( p_flags, GetEngine()->GetRenderSystem()->CreatePipeline(
				DepthStencilState(),
				std::move( l_rsState ),
				DoCreateBlendState( p_flags.m_colourBlendMode, p_flags.m_alphaBlendMode ),
				std::move( l_msState ),
				p_program,
				p_flags ) ).first;
		}
	}

	void RenderTechnique::DoPrepareOpaqueBackPipeline( ShaderProgram & p_program, PipelineFlags const & p_flags )
	{
		auto l_it = m_backOpaquePipelines.find( p_flags );

		if ( l_it == m_backOpaquePipelines.end() )
		{
			DoUpdateProgram( p_program, p_flags.m_programFlags );
			RasteriserState l_rsState;
			l_rsState.SetCulledFaces( Culling::eBack );
			MultisampleState l_msState;
			l_msState.SetMultisample( m_multisampling );
			l_it = m_backOpaquePipelines.emplace( p_flags, GetEngine()->GetRenderSystem()->CreatePipeline(
				DepthStencilState(),
				std::move( l_rsState ),
				DoCreateBlendState( p_flags.m_colourBlendMode, p_flags.m_alphaBlendMode ),
				std::move( l_msState ),
				p_program,
				p_flags ) ).first;
		}
	}

	void RenderTechnique::DoPrepareTransparentFrontPipeline( ShaderProgram & p_program, PipelineFlags const & p_flags )
	{
		auto l_it = m_frontTransparentPipelines.find( p_flags );

		if ( l_it == m_frontTransparentPipelines.end() )
		{
			DoUpdateProgram( p_program, p_flags.m_programFlags );
			DepthStencilState l_dsState;
			l_dsState.SetDepthMask( m_multisampling ? WritingMask::eAll : WritingMask::eZero );
			RasteriserState l_rsState;
			l_rsState.SetCulledFaces( Culling::eFront );
			MultisampleState l_msState;
			l_msState.SetMultisample( m_multisampling );
			l_msState.EnableAlphaToCoverage( m_multisampling );
			l_it = m_frontTransparentPipelines.emplace( p_flags, GetEngine()->GetRenderSystem()->CreatePipeline(
				std::move( l_dsState ),
				std::move( l_rsState ),
				DoCreateBlendState( p_flags.m_colourBlendMode, p_flags.m_alphaBlendMode ),
				std::move( l_msState ),
				p_program,
				p_flags ) ).first;
		}
	}

	void RenderTechnique::DoPrepareTransparentBackPipeline( ShaderProgram & p_program, PipelineFlags const & p_flags )
	{
		auto l_it = m_backTransparentPipelines.find( p_flags );

		if ( l_it == m_backTransparentPipelines.end() )
		{
			DoUpdateProgram( p_program, p_flags.m_programFlags );
			DepthStencilState l_dsState;
			l_dsState.SetDepthMask( m_multisampling ? WritingMask::eAll : WritingMask::eZero );
			RasteriserState l_rsState;
			l_rsState.SetCulledFaces( Culling::eBack );
			MultisampleState l_msState;
			l_msState.SetMultisample( m_multisampling );
			l_msState.EnableAlphaToCoverage( m_multisampling );
			l_it = m_backTransparentPipelines.emplace( p_flags, GetEngine()->GetRenderSystem()->CreatePipeline(
				std::move( l_dsState ),
				std::move( l_rsState ),
				DoCreateBlendState( p_flags.m_colourBlendMode, p_flags.m_alphaBlendMode ),
				std::move( l_msState ),
				p_program,
				p_flags ) ).first;
		}
	}

	bool RenderTechnique::DoInitialiseSpotShadowMap( Size const & p_size )
	{
		auto l_sampler = GetEngine()->GetSamplerCache().Add( GetName() + cuT( "_SpotShadowMap" ) );
		l_sampler->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
		l_sampler->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
		l_sampler->SetWrappingMode( TextureUVW::eU, WrapMode::eClampToEdge );
		l_sampler->SetWrappingMode( TextureUVW::eV, WrapMode::eClampToEdge );
		l_sampler->SetWrappingMode( TextureUVW::eW, WrapMode::eClampToEdge );
		l_sampler->SetComparisonMode( ComparisonMode::eRefToTexture );
		l_sampler->SetComparisonFunc( ComparisonFunc::eLEqual );

		auto l_texture = GetEngine()->GetRenderSystem()->CreateTexture(
			TextureType::eTwoDimensionsArray,
			AccessType::eNone,
			AccessType::eRead | AccessType::eWrite,
			PixelFormat::eD32F,
			Point3ui{ p_size.width(), p_size.height(), GLSL::SpotShadowMapCount } );
		m_spotShadowMap.SetTexture( l_texture );
		m_spotShadowMap.SetSampler( l_sampler );

		for ( auto & l_image : *l_texture )
		{
			l_image->InitialiseSource();
		}

		return m_spotShadowMap.Initialise();
	}

	bool RenderTechnique::DoInitialisePointShadowMap( Size const & p_size )
	{
		auto l_sampler = GetEngine()->GetSamplerCache().Add( GetName() + cuT( "_PointShadowMap" ) );
		l_sampler->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
		l_sampler->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
		l_sampler->SetWrappingMode( TextureUVW::eU, WrapMode::eClampToEdge );
		l_sampler->SetWrappingMode( TextureUVW::eV, WrapMode::eClampToEdge );
		l_sampler->SetWrappingMode( TextureUVW::eW, WrapMode::eClampToEdge );
		bool l_return{ true };

		auto l_texture = GetEngine()->GetRenderSystem()->CreateTexture(
			TextureType::eCube,
			AccessType::eNone,
			AccessType::eRead | AccessType::eWrite,
			PixelFormat::eD32F,
			p_size );
		m_pointShadowMap.SetTexture( l_texture );
		m_pointShadowMap.SetSampler( l_sampler );

		for ( auto & l_image : *l_texture )
		{
			l_image->InitialiseSource();
		}

		return m_pointShadowMap.Initialise();
	}

	void RenderTechnique::DoCleanupSpotShadowMap()
	{
		m_spotShadowMap.Cleanup();
	}

	void RenderTechnique::DoCleanupPointShadowMap()
	{
		m_pointShadowMap.Cleanup();
	}
}
