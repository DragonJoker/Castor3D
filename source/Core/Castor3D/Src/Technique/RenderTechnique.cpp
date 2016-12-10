#if defined( _MSC_VER )
#	pragma warning( disable:4503 )
#endif

#include "RenderTechnique.hpp"

#include "Engine.hpp"
#include "Cache/AnimatedObjectGroupCache.hpp"
#include "Cache/BillboardCache.hpp"
#include "Cache/CameraCache.hpp"
#include "Cache/GeometryCache.hpp"
#include "Cache/LightCache.hpp"
#include "Cache/OverlayCache.hpp"
#include "Cache/SamplerCache.hpp"
#include "Cache/ShaderCache.hpp"
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
#include "Render/RenderPipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Render/RenderTarget.hpp"
#include "Scene/BillboardList.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Geometry.hpp"
#include "Scene/ParticleSystem/ParticleSystem.hpp"
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
#include "Technique/RenderTechniquePass.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureImage.hpp"
#include "Texture/TextureLayout.hpp"

#include <GlslSource.hpp>
#include <GlslShadow.hpp>

using namespace Castor;

namespace Castor3D
{
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

			if ( l_return )
			{
				m_frameBuffer->Bind( FrameBufferMode::eConfig );
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

	RenderTechnique::RenderTechnique( String const & p_name
		, RenderTarget & p_renderTarget
		, RenderSystem & p_renderSystem
		, std::unique_ptr< RenderTechniquePass > && p_opaquePass
		, std::unique_ptr< RenderTechniquePass > && p_transparentPass
		, Parameters const & CU_PARAM_UNUSED( p_params )
		, bool p_multisampling )

		: OwnedBy< Engine >{ *p_renderSystem.GetEngine() }
		, Named{ p_name }
		, m_renderTarget{ p_renderTarget }
		, m_renderSystem{ p_renderSystem }
		, m_opaquePass{ std::move( p_opaquePass ) }
		, m_transparentPass{ std::move( p_transparentPass ) }
		, m_initialised{ false }
		, m_frameBuffer{ *this }
		, m_directionalShadowMap{ *p_renderTarget.GetEngine() }
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
				m_initialised = DoInitialiseDirectionalShadowMap( Size{ 4096, 4096 } );
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

			if ( m_initialised )
			{
				l_scene.GetLightCache().ForEach( [&l_scene, this]( Light & p_light )
				{
					if ( p_light.IsShadowProducer() )
					{
						TextureUnit * l_unit{ nullptr };

						switch ( p_light.GetLightType() )
						{
						case LightType::eDirectional:
							l_unit = &m_directionalShadowMap;
							break;

						case LightType::ePoint:
							l_unit = &m_pointShadowMap;
							break;

						case LightType::eSpot:
							l_unit = &m_spotShadowMap;
							break;
						}

						auto l_pass = GetEngine()->GetShadowMapPassFactory().Create( p_light.GetLightType(), *GetEngine(), l_scene, p_light, *l_unit, 0u );
						auto l_insit = m_shadowMaps.insert( { &p_light, l_pass } ).first;
						l_pass->Initialise( l_unit->GetTexture()->GetDimensions() );
					}
				} );
			}

			if ( m_initialised )
			{
				m_opaquePass->Initialise( m_size );
				m_transparentPass->Initialise( m_size );
			}
		}

		return m_initialised;
	}

	void RenderTechnique::Cleanup()
	{
		m_transparentPass->Cleanup();
		m_opaquePass->Cleanup();
		DoCleanupPointShadowMap();
		DoCleanupSpotShadowMap();
		DoCleanupDirectionalShadowMap();
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
		m_opaquePass->Update();
		m_transparentPass->Update();

		for ( auto & l_it: m_shadowMaps )
		{
			l_it.second->Update();
		}
	}

	void RenderTechnique::Render( uint32_t p_frameTime, uint32_t & p_visible, uint32_t & p_particles )
	{
		auto & l_scene = *m_renderTarget.GetScene();
		l_scene.GetLightCache().UpdateLights();
		m_renderSystem.PushScene( &l_scene );
		bool l_shadows = l_scene.HasShadows();

		if ( l_shadows )
		{
			for ( auto & l_shadowMap : m_shadowMaps )
			{
				l_shadowMap.second->Render();
			}
		}

		DoBeginRender();
		l_scene.RenderBackground( GetSize() );
		auto & l_camera = *m_renderTarget.GetCamera();
		l_camera.Resize( m_size );
		l_camera.Update();
		l_camera.Apply();

		DoBeginOpaqueRendering();
		m_opaquePass->Render( p_frameTime, p_visible, l_shadows );
		DoEndOpaqueRendering();

		if ( l_scene.GetFog().GetType() == FogType::eDisabled )
		{
			l_scene.RenderForeground( GetSize(), l_camera );
		}

		l_scene.GetParticleSystemCache().ForEach( [this, &p_particles]( ParticleSystem & p_particleSystem )
		{
			p_particleSystem.Update();
			p_particles += p_particleSystem.GetParticlesCount();
		} );

		DoBeginTransparentRendering();
		m_transparentPass->Render( p_frameTime, p_visible, l_shadows );
		DoEndTransparentRendering();

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

	bool RenderTechnique::DoInitialiseDirectionalShadowMap( Size const & p_size )
	{
		auto l_sampler = GetEngine()->GetSamplerCache().Add( GetName() + cuT( "_DirectionalShadowMap" ) );
		l_sampler->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
		l_sampler->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
		l_sampler->SetWrappingMode( TextureUVW::eU, WrapMode::eClampToBorder );
		l_sampler->SetWrappingMode( TextureUVW::eV, WrapMode::eClampToBorder );
		l_sampler->SetWrappingMode( TextureUVW::eW, WrapMode::eClampToBorder );
		l_sampler->SetComparisonMode( ComparisonMode::eRefToTexture );
		l_sampler->SetComparisonFunc( ComparisonFunc::eLEqual );

		auto l_texture = GetEngine()->GetRenderSystem()->CreateTexture(
			TextureType::eTwoDimensions,
			AccessType::eNone,
			AccessType::eRead | AccessType::eWrite,
			PixelFormat::eD32F, p_size );
		m_directionalShadowMap.SetTexture( l_texture );
		m_directionalShadowMap.SetSampler( l_sampler );

		for ( auto & l_image : *l_texture )
		{
			l_image->InitialiseSource();
		}

		return m_directionalShadowMap.Initialise();
	}

	bool RenderTechnique::DoInitialiseSpotShadowMap( Size const & p_size )
	{
		auto l_sampler = GetEngine()->GetSamplerCache().Add( GetName() + cuT( "_SpotShadowMap" ) );
		l_sampler->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
		l_sampler->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
		l_sampler->SetWrappingMode( TextureUVW::eU, WrapMode::eClampToBorder );
		l_sampler->SetWrappingMode( TextureUVW::eV, WrapMode::eClampToBorder );
		l_sampler->SetWrappingMode( TextureUVW::eW, WrapMode::eClampToBorder );
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
		l_sampler->SetWrappingMode( TextureUVW::eU, WrapMode::eClampToBorder );
		l_sampler->SetWrappingMode( TextureUVW::eV, WrapMode::eClampToBorder );
		l_sampler->SetWrappingMode( TextureUVW::eW, WrapMode::eClampToBorder );
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

	void RenderTechnique::DoCleanupDirectionalShadowMap()
	{
		m_directionalShadowMap.Cleanup();
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
