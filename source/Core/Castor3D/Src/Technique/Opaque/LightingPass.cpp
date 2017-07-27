#include "LightingPass.hpp"

#include "DirectionalLightPass.hpp"
#include "LightPassShadow.hpp"
#include "OpaquePass.hpp"
#include "PointLightPass.hpp"
#include "SpotLightPass.hpp"

#include <Engine.hpp>
#include <Mesh/Buffer/GeometryBuffers.hpp>
#include <Mesh/Buffer/VertexBuffer.hpp>
#include <FrameBuffer/FrameBuffer.hpp>
#include <FrameBuffer/RenderBufferAttachment.hpp>
#include <Render/RenderPassTimer.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Scene/Scene.hpp>
#include <Scene/Camera.hpp>
#include <Scene/Light/PointLight.hpp>
#include <Shader/MatrixUbo.hpp>
#include <Shader/ModelMatrixUbo.hpp>
#include <Shader/ShaderProgram.hpp>
#include <Technique/RenderTechniquePass.hpp>
#include <Texture/TextureLayout.hpp>
#include <Texture/TextureUnit.hpp>

#include <GlslSource.hpp>
#include <GlslLight.hpp>
#include <GlslShadow.hpp>

using namespace Castor;
using namespace Castor3D;

namespace Castor3D
{
	LightingPass::LightingPass( Engine & engine
		, Size const & size
		, Scene const & scene
		, OpaquePass & opaque
		, FrameBufferAttachment & depthAttach
		, SceneUbo & sceneUbo
		, GpInfoUbo & gpInfoUbo )
		: m_size{ size }
		, m_result{ engine }
		, m_frameBuffer{ engine.GetRenderSystem()->CreateFrameBuffer() }
		, m_timer{ std::make_shared< RenderPassTimer >( engine, cuT( "Lighting" ) ) }
	{
		m_frameBuffer->SetClearColour( Colour::from_predef( PredefinedColour::eTransparentBlack ) );
		bool result = m_frameBuffer->Create();

		if ( result )
		{
			result = m_frameBuffer->Initialise( size );
		}

		if ( result )
		{
			auto texture = engine.GetRenderSystem()->CreateTexture( TextureType::eTwoDimensions
				, AccessType::eNone
				, AccessType::eRead | AccessType::eWrite
				, PixelFormat::eRGBA16F32F
				, size );
			texture->GetImage().InitialiseSource();

			m_result.SetIndex( 0u );
			m_result.SetTexture( texture );
			m_result.SetSampler( engine.GetLightsSampler() );
			m_result.Initialise();

			m_resultAttach = m_frameBuffer->CreateAttachment( texture );

			m_frameBuffer->Bind();
			m_frameBuffer->Attach( AttachmentPoint::eColour
				, m_resultAttach
				, texture->GetType() );
			ENSURE( m_frameBuffer->IsComplete() );
			m_frameBuffer->SetDrawBuffers();
			m_frameBuffer->Unbind();

			m_lightPass[size_t( LightType::eDirectional )] = std::make_unique< DirectionalLightPass >( engine
				, *m_frameBuffer
				, depthAttach
				, gpInfoUbo
				, false );
			m_lightPass[size_t( LightType::ePoint )] = std::make_unique< PointLightPass >( engine
				, *m_frameBuffer
				, depthAttach
				, gpInfoUbo
				, false );
			m_lightPass[size_t( LightType::eSpot )] = std::make_unique< SpotLightPass >( engine
				, *m_frameBuffer
				, depthAttach
				, gpInfoUbo
				, false );
			m_lightPassShadow[size_t( LightType::eDirectional )] = std::make_unique< DirectionalLightPassShadow >( engine
				, *m_frameBuffer
				, depthAttach
				, gpInfoUbo
				, opaque.GetDirectionalShadowMap() );
			m_lightPassShadow[size_t( LightType::ePoint )] = std::make_unique< PointLightPassShadow >( engine
				, *m_frameBuffer
				, depthAttach
				, gpInfoUbo
				, opaque.GetPointShadowMaps() );
			m_lightPassShadow[size_t( LightType::eSpot )] = std::make_unique< SpotLightPassShadow >( engine
				, *m_frameBuffer
				, depthAttach
				, gpInfoUbo
				, opaque.GetSpotShadowMap() );

			for ( auto & lightPass : m_lightPass )
			{
				lightPass->Initialise( scene, sceneUbo );
			}

			for ( auto & lightPass : m_lightPassShadow )
			{
				lightPass->Initialise( scene, sceneUbo );
			}
		}
	}

	LightingPass::~LightingPass()
	{
		m_frameBuffer->Bind();
		m_frameBuffer->DetachAll();
		m_frameBuffer->Unbind();
		m_frameBuffer->Cleanup();
		m_frameBuffer->Destroy();
		m_resultAttach.reset();
		m_result.Cleanup();

		for ( auto & lightPass : m_lightPass )
		{
			lightPass->Cleanup();
			lightPass.reset();
		}

		for ( auto & lightPass : m_lightPassShadow )
		{
			lightPass->Cleanup();
			lightPass.reset();
		}
	}

	bool LightingPass::Render( Scene const & scene
		, Camera const & camera
		, GeometryPassResult const & gp
		, RenderInfo & info )
	{
		auto & cache = scene.GetLightCache();
		m_frameBuffer->Bind( FrameBufferTarget::eDraw );
		m_frameBuffer->Clear( BufferComponent::eColour );

		bool first{ true };

		if ( !cache.IsEmpty() )
		{
			m_timer->Start();
#if DEBUG_SHADOW_MAPS && !defined( NDEBUG )

			g_index = 0;

#endif

			auto lock = make_unique_lock( cache );
			DoRenderLights( scene
				, camera
				, LightType::eDirectional
				, gp
				, first );
			DoRenderLights( scene
				, camera
				, LightType::ePoint
				, gp
				, first );
			DoRenderLights( scene
				, camera
				, LightType::eSpot
				, gp
				, first );
			first = false;
			m_timer->Stop();
		}

		return first;
	}

	void LightingPass::DoRenderLights( Scene const & scene
		, Camera const & camera
		, LightType p_type
		, GeometryPassResult const & gp
		, bool & p_first )
	{
		auto & cache = scene.GetLightCache();

		if ( cache.GetLightsCount( p_type ) )
		{
			auto & lightPass = *m_lightPass[size_t( p_type )];
			auto & lightPassShadow = *m_lightPassShadow[size_t( p_type )];

			for ( auto & light : cache.GetLights( p_type ) )
			{
				if ( light->IsShadowProducer() )
				{
					lightPassShadow.Render( m_size
						, gp
						, *light
						, camera
						, p_first );
				}
				else
				{
					lightPass.Render( m_size
						, gp
						, *light
						, camera
						, p_first );
				}

				p_first = false;
			}
		}
	}
}
