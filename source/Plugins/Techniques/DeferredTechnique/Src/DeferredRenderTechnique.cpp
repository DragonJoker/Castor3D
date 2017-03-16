#include "DeferredRenderTechnique.hpp"

#include "DirectionalLightPass.hpp"
#include "LightPassShadow.hpp"
#include "PointLightPass.hpp"
#include "SpotLightPass.hpp"

#include <OpaquePass.hpp>

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
	String const RenderTechnique::Type = cuT( "deferred" );
	String const RenderTechnique::Name = cuT( "Deferred Lighting Render Technique" );

	RenderTechnique::RenderTechnique( RenderTarget & p_renderTarget
		, RenderSystem & p_renderSystem
		, Parameters const & p_params )
		: Castor3D::RenderTechnique( RenderTechnique::Type
			, p_renderTarget
			, p_renderSystem
			, std::make_unique< deferred_common::OpaquePass >( p_renderTarget, *this )
			, std::make_unique< ForwardRenderTechniquePass >( cuT( "deferred_transparent" )
				, p_renderTarget
				, *this
				, false
				, false )
			, p_params )
		, m_sceneUbo{ ShaderProgram::BufferScene, p_renderSystem }
	{
		UniformBuffer::FillSceneBuffer( m_sceneUbo );
		m_cameraPos = m_sceneUbo.GetUniform< UniformType::eVec3f >( ShaderProgram::CameraPos );
		m_fogType = m_sceneUbo.GetUniform< UniformType::eInt >( ShaderProgram::FogType );
		m_fogDensity = m_sceneUbo.GetUniform< UniformType::eFloat >( ShaderProgram::FogDensity );
		Logger::LogInfo( cuT( "Using deferred rendering" ) );
	}

	RenderTechnique::~RenderTechnique()
	{
	}

	RenderTechniqueSPtr RenderTechnique::CreateInstance( RenderTarget & p_renderTarget, RenderSystem & p_renderSystem, Parameters const & p_params )
	{
		// No make_shared because ctor is protected;
		return RenderTechniqueSPtr( new RenderTechnique( p_renderTarget, p_renderSystem, p_params ) );
	}

	bool RenderTechnique::DoInitialise( uint32_t & p_index )
	{
		bool l_return = DoInitialiseGeometryPass( p_index );

		if ( l_return )
		{
			l_return = DoInitialiseLightPass();
		}

		m_frameBuffer.m_frameBuffer->SetClearColour( Colour::from_predef( PredefinedColour::eHighAlphaBlack ) );
		return l_return;
	}

	void RenderTechnique::DoCleanup()
	{
		DoCleanupGeometryPass();
		DoCleanupLightPass();
	}

	void RenderTechnique::DoUpdateSceneUbo()
	{
		auto & l_fog = m_renderTarget.GetScene()->GetFog();
		auto l_fogType = l_fog.GetType();

		m_fogType->SetValue( int( l_fog.GetType() ) );

		if ( l_fog.GetType() != GLSL::FogType::eDisabled )
		{
			m_fogDensity->SetValue( l_fog.GetDensity() );
		}

		m_cameraPos->SetValue( m_renderTarget.GetCamera()->GetParent()->GetDerivedPosition() );
		m_sceneUbo.Update();
	}

	void RenderTechnique::DoRenderOpaque( uint32_t & p_visible )
	{
		GetEngine()->SetPerObjectLighting( false );
		m_renderTarget.GetCamera()->Apply();
		m_geometryPassFrameBuffer->Bind( FrameBufferTarget::eDraw );
		m_frameBuffer.m_depthAttach->Attach( AttachmentPoint::eDepthStencil );
		m_geometryPassFrameBuffer->Clear( BufferComponent::eColour | BufferComponent::eDepth | BufferComponent::eStencil );
		m_opaquePass->Render( p_visible, m_renderTarget.GetScene()->HasShadows() );
		m_geometryPassFrameBuffer->Unbind();

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

		m_frameBuffer.m_frameBuffer->Bind( FrameBufferTarget::eDraw );
		m_frameBuffer.m_depthAttach->Attach( AttachmentPoint::eDepthStencil );
		m_frameBuffer.m_frameBuffer->Clear( BufferComponent::eColour | BufferComponent::eStencil );
		m_frameBuffer.m_frameBuffer->Unbind();

		DoUpdateSceneUbo();

		auto & l_cache = m_renderTarget.GetScene()->GetLightCache();

		if ( !l_cache.IsEmpty() )
		{
#if !defined( NDEBUG )

			deferred_common::g_index = 0;

#endif

			auto l_lock = make_unique_lock( l_cache );
			bool l_first{ true };
			DoRenderLights( LightType::eDirectional, l_first );
			DoRenderLights( LightType::ePoint, l_first );
			DoRenderLights( LightType::eSpot, l_first );
		}

		m_frameBuffer.m_frameBuffer->Bind( FrameBufferTarget::eDraw );
		m_frameBuffer.m_frameBuffer->SetDrawBuffers();

#endif
	}

	void RenderTechnique::DoRenderTransparent( uint32_t & p_visible )
	{
		m_frameBuffer.m_frameBuffer->Unbind();
		GetEngine()->SetPerObjectLighting( true );
		m_transparentPass->RenderShadowMaps();
		m_renderTarget.GetCamera()->Apply();
		m_frameBuffer.m_frameBuffer->Bind( FrameBufferTarget::eDraw );
		m_transparentPass->Render( p_visible, m_renderTarget.GetScene()->HasShadows() );
		m_frameBuffer.m_frameBuffer->Unbind();
	}

	bool RenderTechnique::DoWriteInto( TextFile & p_file )
	{
		return true;
	}

	bool RenderTechnique::DoInitialiseGeometryPass( uint32_t & p_index )
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

		if ( l_return )
		{
			l_return = m_geometryPassFrameBuffer->Initialise( m_size );
		}

		if ( l_return )
		{
			for ( uint32_t i = 0; i < uint32_t( deferred_common::DsTexture::eCount ); i++ )
			{
				auto l_texture = m_renderSystem.CreateTexture( TextureType::eTwoDimensions
					, AccessType::eNone
					, AccessType::eRead | AccessType::eWrite
					, GetTextureFormat( deferred_common::DsTexture( i ) )
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

			m_lightPassDepthBuffer->Initialise( m_size );
			m_geometryPassFrameBuffer->Bind();

			for ( int i = 0; i < size_t( deferred_common::DsTexture::eCount ) && l_return; i++ )
			{
				m_geometryPassFrameBuffer->Attach( GetTextureAttachmentPoint( deferred_common::DsTexture( i ) )
					, GetTextureAttachmentIndex( deferred_common::DsTexture( i ) )
					, m_geometryPassTexAttachs[i]
					, m_lightPassTextures[i]->GetType() );
			}

			ENSURE( m_geometryPassFrameBuffer->IsComplete() );
			m_geometryPassFrameBuffer->SetDrawBuffers();
			m_geometryPassFrameBuffer->Unbind();
		}
		
		return l_return;
	}

	bool RenderTechnique::DoInitialiseLightPass()
	{
		auto & l_opaquePass = *reinterpret_cast< deferred_common::OpaquePass * >( m_opaquePass.get() );
		auto & l_scene = *m_renderTarget.GetScene();
		m_lightPass[size_t( LightType::eDirectional )] = std::make_unique< deferred_common::DirectionalLightPass >( *m_renderTarget.GetEngine()
			, *m_frameBuffer.m_frameBuffer
			, *m_frameBuffer.m_depthAttach
			, false );
		m_lightPass[size_t( LightType::ePoint )] = std::make_unique< deferred_common::PointLightPass >( *m_renderTarget.GetEngine()
			, *m_frameBuffer.m_frameBuffer
			, *m_frameBuffer.m_depthAttach
			, false );
		m_lightPass[size_t( LightType::eSpot )] = std::make_unique< deferred_common::SpotLightPass >( *m_renderTarget.GetEngine()
			, *m_frameBuffer.m_frameBuffer
			, *m_frameBuffer.m_depthAttach
			, false );
		m_lightPassShadow[size_t( LightType::eDirectional )] = std::make_unique< deferred_common::DirectionalLightPassShadow >( *m_renderTarget.GetEngine()
			, *m_frameBuffer.m_frameBuffer
			, *m_frameBuffer.m_depthAttach
			, l_opaquePass.GetDirectionalShadowMap() );
		m_lightPassShadow[size_t( LightType::ePoint )] = std::make_unique< deferred_common::PointLightPassShadow >( *m_renderTarget.GetEngine()
			, *m_frameBuffer.m_frameBuffer
			, *m_frameBuffer.m_depthAttach
			, l_opaquePass.GetPointShadowMaps() );
		m_lightPassShadow[size_t( LightType::eSpot )] = std::make_unique< deferred_common::SpotLightPassShadow >( *m_renderTarget.GetEngine()
			, *m_frameBuffer.m_frameBuffer
			, *m_frameBuffer.m_depthAttach
			, l_opaquePass.GetSpotShadowMap() );

		for ( auto & l_lightPass : m_lightPass )
		{
			l_lightPass->Initialise( l_scene, m_sceneUbo );
		}

		for ( auto & l_lightPass : m_lightPassShadow )
		{
			l_lightPass->Initialise( l_scene, m_sceneUbo );
		}

		return true;
	}

	void RenderTechnique::DoCleanupGeometryPass()
	{
		m_geometryPassFrameBuffer->Bind();
		m_geometryPassFrameBuffer->DetachAll();
		m_geometryPassFrameBuffer->Unbind();
		m_geometryPassFrameBuffer->Cleanup();
		m_lightPassDepthBuffer->Destroy();
		m_lightPassDepthBuffer.reset();
		m_geometryPassFrameBuffer->Destroy();
		m_geometryPassFrameBuffer.reset();
	}

	void RenderTechnique::DoCleanupLightPass()
	{
		for ( auto & l_unit : m_lightPassTextures )
		{
			l_unit->Cleanup();
			l_unit.reset();
		}

		for ( auto & l_lightPass : m_lightPass )
		{
			l_lightPass->Cleanup();
			l_lightPass.reset();
		}

		for ( auto & l_lightPass : m_lightPassShadow )
		{
			l_lightPass->Cleanup();
			l_lightPass.reset();
		}

		m_sceneUbo.Cleanup();
	}

	void RenderTechnique::DoRenderLights( LightType p_type, bool & p_first )
	{
		auto & l_scene = *m_renderTarget.GetScene();
		auto & l_cache = l_scene.GetLightCache();

		if ( l_cache.GetLightsCount( p_type ) )
		{
			auto l_fogType = l_scene.GetFog().GetType();
			auto & l_camera = *m_renderTarget.GetCamera();
			auto & l_lightPass = *m_lightPass[size_t( p_type )];
			auto & l_lightPassShadow = *m_lightPassShadow[size_t( p_type )];

			for ( auto & l_light : l_cache.GetLights( p_type ) )
			{
				if ( l_light->IsShadowProducer() )
				{
					l_lightPassShadow.Render( m_size
						, m_lightPassTextures
						, *l_light
						, l_camera
						, l_fogType
						, p_first );
				}
				else
				{
					l_lightPass.Render( m_size
						, m_lightPassTextures
						, *l_light
						, l_camera
						, l_fogType
						, p_first );
				}

				p_first = false;
			}
		}
	}
}
