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

namespace deferred_common
{
	LightingPass::LightingPass( Engine & p_engine
		, Size const & p_size
		, Scene const & p_scene
		, OpaquePass & p_opaque
		, FrameBufferAttachment & p_depthAttach
		, SceneUbo & p_sceneUbo )
		: m_size{ p_size }
		, m_result{ p_engine }
		, m_frameBuffer{ p_engine.GetRenderSystem()->CreateFrameBuffer() }
	{
		m_frameBuffer->SetClearColour( Colour::from_predef( PredefinedColour::eTransparentBlack ) );
		bool l_return = m_frameBuffer->Create();

		if ( l_return )
		{
			l_return = m_frameBuffer->Initialise( p_size );
		}

		if ( l_return )
		{
			auto l_texture = p_engine.GetRenderSystem()->CreateTexture( TextureType::eTwoDimensions
				, AccessType::eNone
				, AccessType::eRead | AccessType::eWrite
				, PixelFormat::eRGBA16F32F
				, p_size );
			l_texture->GetImage().InitialiseSource();

			m_result.SetIndex( 0u );
			m_result.SetTexture( l_texture );
			m_result.SetSampler( p_engine.GetLightsSampler() );
			m_result.Initialise();

			m_resultAttach = m_frameBuffer->CreateAttachment( l_texture );

			m_frameBuffer->Bind();
			m_frameBuffer->Attach( AttachmentPoint::eColour
				, m_resultAttach
				, l_texture->GetType() );
			ENSURE( m_frameBuffer->IsComplete() );
			m_frameBuffer->SetDrawBuffers();
			m_frameBuffer->Unbind();

			m_lightPass[size_t( LightType::eDirectional )] = std::make_unique< DirectionalLightPass >( p_engine
				, *m_frameBuffer
				, p_depthAttach
				, false );
			m_lightPass[size_t( LightType::ePoint )] = std::make_unique< PointLightPass >( p_engine
				, *m_frameBuffer
				, p_depthAttach
				, false );
			m_lightPass[size_t( LightType::eSpot )] = std::make_unique< SpotLightPass >( p_engine
				, *m_frameBuffer
				, p_depthAttach
				, false );
			m_lightPassShadow[size_t( LightType::eDirectional )] = std::make_unique< DirectionalLightPassShadow >( p_engine
				, *m_frameBuffer
				, p_depthAttach
				, p_opaque.GetDirectionalShadowMap() );
			m_lightPassShadow[size_t( LightType::ePoint )] = std::make_unique< PointLightPassShadow >( p_engine
				, *m_frameBuffer
				, p_depthAttach
				, p_opaque.GetPointShadowMaps() );
			m_lightPassShadow[size_t( LightType::eSpot )] = std::make_unique< SpotLightPassShadow >( p_engine
				, *m_frameBuffer
				, p_depthAttach
				, p_opaque.GetSpotShadowMap() );

			for ( auto & l_lightPass : m_lightPass )
			{
				l_lightPass->Initialise( p_scene, p_sceneUbo );
			}

			for ( auto & l_lightPass : m_lightPassShadow )
			{
				l_lightPass->Initialise( p_scene, p_sceneUbo );
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
	}

	bool LightingPass::Render( Scene const & p_scene
		, Camera const & p_camera
		, GeometryPassResult const & p_gp
		, Matrix4x4r const & p_invViewProj
		, Matrix4x4r const & p_invView
		, Matrix4x4r const & p_invProj )
	{
		auto & l_cache = p_scene.GetLightCache();
		m_frameBuffer->Bind( FrameBufferTarget::eDraw );
		m_frameBuffer->Clear( BufferComponent::eColour );

		bool l_first{ true };

		if ( !l_cache.IsEmpty() )
		{
#if !defined( NDEBUG )

			g_index = 0;

#endif

			auto l_lock = make_unique_lock( l_cache );
			DoRenderLights( p_scene
				, p_camera
				, LightType::eDirectional
				, p_gp
				, p_invViewProj
				, p_invView
				, p_invProj
				, l_first );
			DoRenderLights( p_scene
				, p_camera
				, LightType::ePoint
				, p_gp
				, p_invViewProj
				, p_invView
				, p_invProj
				, l_first );
			DoRenderLights( p_scene
				, p_camera
				, LightType::eSpot
				, p_gp
				, p_invViewProj
				, p_invView
				, p_invProj
				, l_first );
			l_first = false;
		}

		return l_first;
	}

	void LightingPass::DoRenderLights( Scene const & p_scene
		, Camera const & p_camera
		, LightType p_type
		, GeometryPassResult const & p_gp
		, Matrix4x4r const & p_invViewProj
		, Matrix4x4r const & p_invView
		, Matrix4x4r const & p_invProj
		, bool & p_first )
	{
		auto & l_cache = p_scene.GetLightCache();

		if ( l_cache.GetLightsCount( p_type ) )
		{
			auto & l_lightPass = *m_lightPass[size_t( p_type )];
			auto & l_lightPassShadow = *m_lightPassShadow[size_t( p_type )];

			for ( auto & l_light : l_cache.GetLights( p_type ) )
			{
				if ( l_light->IsShadowProducer() )
				{
					l_lightPassShadow.Render( m_size
						, p_gp
						, *l_light
						, p_camera
						, p_invViewProj
						, p_invView
						, p_invProj
						, p_first );
				}
				else
				{
					l_lightPass.Render( m_size
						, p_gp
						, *l_light
						, p_camera
						, p_invViewProj
						, p_invView
						, p_invProj
						, p_first );
				}

				p_first = false;
			}
		}
	}
}
