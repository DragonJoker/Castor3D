#include "DeferredRenderTechnique.hpp"

#include "DeferredRenderTechniqueOpaquePass.hpp"

#include <Engine.hpp>
#include <Cache/CameraCache.hpp>
#include <Cache/LightCache.hpp>
#include <Cache/SceneCache.hpp>
#include <Cache/SceneNodeCache.hpp>
#include <Cache/ShaderCache.hpp>
#include <Cache/TargetCache.hpp>

#include <FrameBuffer/ColourRenderBuffer.hpp>
#include <FrameBuffer/DepthStencilRenderBuffer.hpp>
#include <FrameBuffer/FrameBuffer.hpp>
#include <FrameBuffer/RenderBufferAttachment.hpp>
#include <FrameBuffer/TextureAttachment.hpp>
#include <Mesh/Buffer/BufferDeclaration.hpp>
#include <Mesh/Buffer/BufferElementDeclaration.hpp>
#include <Mesh/Buffer/BufferElementGroup.hpp>
#include <Mesh/Buffer/GeometryBuffers.hpp>
#include <Mesh/Buffer/IndexBuffer.hpp>
#include <Mesh/Buffer/VertexBuffer.hpp>
#include <Render/Context.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Render/RenderTarget.hpp>
#include <Render/Viewport.hpp>
#include <Scene/Camera.hpp>
#include <Scene/Scene.hpp>
#include <Scene/Light/Light.hpp>
#include <Scene/Light/DirectionalLight.hpp>
#include <Scene/Light/PointLight.hpp>
#include <Scene/Light/SpotLight.hpp>
#include <Shader/UniformBuffer.hpp>
#include <Shader/ShaderProgram.hpp>
#include <State/BlendState.hpp>
#include <State/DepthStencilState.hpp>
#include <State/MultisampleState.hpp>
#include <State/RasteriserState.hpp>
#include <Technique/ForwardRenderTechniquePass.hpp>
#include <Texture/Sampler.hpp>
#include <Texture/TextureLayout.hpp>
#include <Texture/TextureUnit.hpp>

#include <Log/Logger.hpp>

#include <GlslSource.hpp>
#include <GlslLight.hpp>
#include <GlslShadow.hpp>

#define DEBUG_DEFERRED_BUFFERS 0

using namespace Castor;
using namespace Castor3D;

namespace deferred
{
	//************************************************************************************************

	String const RenderTechnique::Type = cuT( "deferred" );
	String const RenderTechnique::Name = cuT( "Deferred Lighting Render Technique" );

	RenderTechnique::RenderTechnique( RenderTarget & p_renderTarget, RenderSystem & p_renderSystem, Parameters const & p_params )
		: Castor3D::RenderTechnique( RenderTechnique::Type
			, p_renderTarget
			, p_renderSystem
			, std::make_unique< OpaquePass >( p_renderTarget, *this )
			, std::make_unique< ForwardRenderTechniquePass >( cuT( "deferred_transparent" ), p_renderTarget, *this, false, false )
			, p_params )
		, m_sceneUbo{ ShaderProgram::BufferScene, p_renderSystem }
		, m_directionalLightPass{ *p_renderTarget.GetEngine() }
		, m_pointLightPass{ *p_renderTarget.GetEngine() }
		, m_spotLightPass{ *p_renderTarget.GetEngine() }
		, m_directionalLightPassShadow{ *p_renderTarget.GetEngine() }
		, m_pointLightPassShadow{ *p_renderTarget.GetEngine() }
		, m_spotLightPassShadow{ *p_renderTarget.GetEngine() }
	{
		UniformBuffer::FillSceneBuffer( m_sceneUbo );

		m_sceneNode = std::make_unique< SceneRenderNode >( m_sceneUbo );
		Logger::LogInfo( cuT( "Using deferred shading" ) );
	}

	RenderTechnique::~RenderTechnique()
	{
	}

	RenderTechniqueSPtr RenderTechnique::CreateInstance( RenderTarget & p_renderTarget, RenderSystem & p_renderSystem, Parameters const & p_params )
	{
		// No make_shared because ctor is protected;
		return RenderTechniqueSPtr( new RenderTechnique( p_renderTarget, p_renderSystem, p_params ) );
	}

	bool RenderTechnique::DoCreate()
	{
		bool l_return = DoCreateGeometryPass();

		if ( l_return )
		{
			l_return = DoCreateLightPass();
		}

		return l_return;
	}

	void RenderTechnique::DoDestroy()
	{
		DoDestroyLightPass();
		DoDestroyGeometryPass();
	}

	bool RenderTechnique::DoInitialise( uint32_t & p_index )
	{
		bool l_return = DoInitialiseLightPass( p_index );

		if ( l_return )
		{
			l_return = DoInitialiseGeometryPass();
		}

		return l_return;
	}

	void RenderTechnique::DoCleanup()
	{
		DoCleanupGeometryPass();
		DoCleanupLightPass();
	}

	void RenderTechnique::DoRenderOpaque( uint32_t & p_visible )
	{
		GetEngine()->SetPerObjectLighting( false );
		m_renderTarget.GetCamera()->Apply();
		m_geometryPassFrameBuffer->Bind( FrameBufferMode::eAutomatic, FrameBufferTarget::eDraw );
		m_geometryPassFrameBuffer->Clear();
		m_opaquePass->Render( p_visible, m_renderTarget.GetScene()->HasShadows() );
		m_geometryPassFrameBuffer->Unbind();
		// Render the light pass.

#if DEBUG_DEFERRED_BUFFERS

		int l_width = int( m_size.width() );
		int l_height = int( m_size.height() );
		int l_thirdWidth = int( l_width / 3.0f );
		int l_twoThirdWidth = int( 2.0f * l_width / 3.0f );
		int l_thirdHeight = int( l_height / 3.0f );
		int l_twothirdHeight = int( 2.0f * l_height / 3.0f );
		auto l_size = Size( l_thirdWidth, l_thirdHeight );
		auto & l_context = *m_renderSystem.GetCurrentContext();
		m_frameBuffer.m_frameBuffer->Bind();
		l_context.RenderTexture( Position{ 0, 0 }, l_size, *m_lightPassTextures[size_t( DsTexture::ePosition )]->GetTexture() );
		l_context.RenderTexture( Position{ 0, l_thirdHeight }, l_size, *m_lightPassTextures[size_t( DsTexture::eDiffuse )]->GetTexture() );
		l_context.RenderTexture( Position{ 0, l_twothirdHeight }, l_size, *m_lightPassTextures[size_t( DsTexture::eNormals )]->GetTexture() );
		l_context.RenderTexture( Position{ l_thirdWidth, 0 }, l_size, *m_lightPassTextures[size_t( DsTexture::eTangent )]->GetTexture() );
		l_context.RenderTexture( Position{ l_thirdWidth, l_thirdHeight }, l_size, *m_lightPassTextures[size_t( DsTexture::eSpecular )]->GetTexture() );
		l_context.RenderTexture( Position{ l_thirdWidth, l_twothirdHeight }, l_size, *m_lightPassTextures[size_t( DsTexture::eEmissive )]->GetTexture() );

#else

		m_frameBuffer.m_frameBuffer->Bind( FrameBufferMode::eAutomatic, FrameBufferTarget::eDraw );
		auto & l_scene = *m_renderTarget.GetScene();
		auto & l_camera = *m_renderTarget.GetCamera();
		m_frameBuffer.m_frameBuffer->SetClearColour( l_scene.GetBackgroundColour() );
		m_frameBuffer.m_frameBuffer->Clear();

		auto & l_fog = l_scene.GetFog();
		m_sceneNode->m_fogType.SetValue( int( l_fog.GetType() ) );

		if ( l_fog.GetType() != GLSL::FogType::eDisabled )
		{
			m_sceneNode->m_fogDensity.SetValue( l_fog.GetDensity() );
		}

		auto & l_cache = l_scene.GetLightCache();
		auto l_lock = make_unique_lock( l_cache );
		m_sceneNode->m_ambientLight.SetValue( rgba_float( l_scene.GetAmbientLight() ) );

		m_sceneNode->m_backgroundColour.SetValue( rgba_float( l_scene.GetBackgroundColour() ) );
		m_sceneNode->m_cameraPos.SetValue( l_camera.GetParent()->GetDerivedPosition() );
		m_sceneUbo.Update();
		bool l_first{ true };

		if ( l_cache.GetLightsCount( LightType::eDirectional ) )
		{
			for ( auto & l_light : l_cache.GetLights( LightType::eDirectional ) )
			{
				m_directionalLightPass.Render( m_size
					, m_lightPassTextures
					, *l_light->GetDirectionalLight()
					, uint16_t( GetFogType( l_scene.GetFlags() ) )
					, l_first );
				l_first = false;
			}
		}

		if ( l_cache.GetLightsCount( LightType::ePoint ) )
		{
			for ( auto & l_light : l_cache.GetLights( LightType::ePoint ) )
			{
				m_frameBuffer.m_frameBuffer->ClearComponent( BufferComponent::eStencil );
				m_pointLightPass.Render( m_size
					, m_lightPassTextures
					, *l_light->GetPointLight()
					, l_camera
					, uint16_t( GetFogType( l_scene.GetFlags() ) )
					, l_first );
				l_first = false;
			}
		}

		if ( l_cache.GetLightsCount( LightType::eSpot ) )
		{
			for ( auto & l_light : l_cache.GetLights( LightType::eSpot ) )
			{
				m_frameBuffer.m_frameBuffer->ClearComponent( BufferComponent::eStencil );
				m_spotLightPass.Render( m_size
					, m_lightPassTextures
					, *l_light->GetSpotLight()
					, l_camera
					, uint16_t( GetFogType( l_scene.GetFlags() ) )
					, l_first );
				l_first = false;
			}
		}

		m_frameBuffer.m_frameBuffer->Unbind();
		m_geometryPassFrameBuffer->BlitInto( *m_frameBuffer.m_frameBuffer
			, Rectangle{ Position{}, m_size }
		, BufferComponent::eDepth | BufferComponent::eStencil );
		m_frameBuffer.m_frameBuffer->Bind( FrameBufferMode::eAutomatic, FrameBufferTarget::eDraw );

#endif
	}

	void RenderTechnique::DoRenderTransparent( uint32_t & p_visible )
	{
		GetEngine()->SetPerObjectLighting( true );
		m_renderTarget.GetCamera()->Apply();
		m_transparentPass->Render( p_visible, m_renderTarget.GetScene()->HasShadows() );
		m_frameBuffer.m_frameBuffer->Unbind();
	}

	bool RenderTechnique::DoWriteInto( TextFile & p_file )
	{
		return true;
	}

	bool RenderTechnique::DoCreateGeometryPass()
	{
		m_geometryPassFrameBuffer = m_renderSystem.CreateFrameBuffer();
		m_geometryPassFrameBuffer->SetClearColour( Colour::from_predef( PredefinedColour::eOpaqueBlack ) );
		m_lightPassDepthBuffer = m_geometryPassFrameBuffer->CreateDepthStencilRenderBuffer( PixelFormat::eD32FS8 );
		m_geometryPassDepthAttach = m_geometryPassFrameBuffer->CreateAttachment( m_lightPassDepthBuffer );
		bool l_return = m_geometryPassFrameBuffer->Create();

		if ( l_return )
		{
			l_return = m_lightPassDepthBuffer->Create();
		}

		return l_return;
	}

	bool RenderTechnique::DoCreateLightPass()
	{
		auto & l_scene = *m_renderTarget.GetScene();
		m_directionalLightPass.Create( l_scene );
		m_pointLightPass.Create( l_scene );
		m_spotLightPass.Create( l_scene );
		return true;
	}

	void RenderTechnique::DoDestroyGeometryPass()
	{
		m_lightPassDepthBuffer->Destroy();
		m_lightPassDepthBuffer.reset();
		m_geometryPassFrameBuffer->Destroy();
		m_geometryPassFrameBuffer.reset();
	}

	void RenderTechnique::DoDestroyLightPass()
	{
		m_directionalLightPass.Destroy();
		m_pointLightPass.Destroy();
		m_spotLightPass.Destroy();
		m_sceneUbo.Cleanup();
	}

	bool RenderTechnique::DoInitialiseGeometryPass()
	{
		bool l_return = m_geometryPassFrameBuffer->Initialise( m_size );

		if ( l_return )
		{
			m_geometryPassFrameBuffer->Bind( FrameBufferMode::eConfig );

			for ( int i = 0; i < size_t( DsTexture::eCount ) && l_return; i++ )
			{
				m_geometryPassFrameBuffer->Attach( GetTextureAttachmentPoint( DsTexture( i ) )
					, GetTextureAttachmentIndex( DsTexture( i ) )
					, m_geometryPassTexAttachs[i]
					, m_lightPassTextures[i]->GetType() );
			}

			m_geometryPassFrameBuffer->Attach( AttachmentPoint::eDepthStencil, m_geometryPassDepthAttach );
			ENSURE( m_geometryPassFrameBuffer->IsComplete() );
			m_geometryPassFrameBuffer->Unbind();
		}
		
		return l_return;
	}

	bool RenderTechnique::DoInitialiseLightPass( uint32_t & p_index )
	{
		bool l_return = true;
		m_directionalLightPass.Initialise( m_sceneUbo );
		m_pointLightPass.Initialise( m_sceneUbo );
		m_spotLightPass.Initialise( m_sceneUbo );

		for ( uint32_t i = 0; i < uint32_t( DsTexture::eCount ); i++ )
		{
			auto l_texture = m_renderSystem.CreateTexture( TextureType::eTwoDimensions
				, AccessType::eNone
				, AccessType::eRead | AccessType::eWrite
				, GetTextureFormat( DsTexture( i ) )
				, m_size );
			l_texture->GetImage().InitialiseSource();

			m_lightPassTextures[i] = std::make_unique< TextureUnit >( *GetEngine() );
			m_lightPassTextures[i]->SetIndex( i );
			m_lightPassTextures[i]->SetTexture( l_texture );
			m_lightPassTextures[i]->SetSampler( GetEngine()->GetLightsSampler() );
			m_lightPassTextures[i]->Initialise();

			m_geometryPassTexAttachs[i] = m_geometryPassFrameBuffer->CreateAttachment( l_texture );
			p_index++;
		}

		if ( l_return )
		{
			m_lightPassDepthBuffer->Initialise( m_size );
		}

		return l_return;
	}

	void RenderTechnique::DoCleanupGeometryPass()
	{
		m_geometryPassFrameBuffer->Bind( FrameBufferMode::eConfig );
		m_geometryPassFrameBuffer->DetachAll();
		m_geometryPassFrameBuffer->Unbind();
		m_geometryPassFrameBuffer->Cleanup();
	}

	void RenderTechnique::DoCleanupLightPass()
	{
		m_directionalLightPass.Cleanup();
		m_pointLightPass.Cleanup();
		m_spotLightPass.Cleanup();

		for ( auto & l_unit : m_lightPassTextures )
		{
			l_unit->Cleanup();
			l_unit.reset();
		}
	}
}
