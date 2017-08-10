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

using namespace castor;
using namespace castor3d;

namespace castor3d
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
		, m_frameBuffer{ engine.getRenderSystem()->createFrameBuffer() }
		, m_timer{ std::make_shared< RenderPassTimer >( engine, cuT( "Lighting" ) ) }
	{
		m_frameBuffer->setClearColour( Colour::fromPredefined( PredefinedColour::eTransparentBlack ) );
		bool result = m_frameBuffer->create();

		if ( result )
		{
			result = m_frameBuffer->initialise( size );
		}

		if ( result )
		{
			auto texture = engine.getRenderSystem()->createTexture( TextureType::eTwoDimensions
				, AccessType::eNone
				, AccessType::eRead | AccessType::eWrite
				, PixelFormat::eRGBA16F32F
				, size );
			texture->getImage().initialiseSource();

			m_result.setIndex( 0u );
			m_result.setTexture( texture );
			m_result.setSampler( engine.getLightsSampler() );
			m_result.initialise();

			m_resultAttach = m_frameBuffer->createAttachment( texture );

			m_frameBuffer->bind();
			m_frameBuffer->attach( AttachmentPoint::eColour
				, m_resultAttach
				, texture->getType() );
			ENSURE( m_frameBuffer->isComplete() );
			m_frameBuffer->setDrawBuffers();
			m_frameBuffer->unbind();

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
				, opaque.getDirectionalShadowMap() );
			m_lightPassShadow[size_t( LightType::ePoint )] = std::make_unique< PointLightPassShadow >( engine
				, *m_frameBuffer
				, depthAttach
				, gpInfoUbo
				, opaque.getPointShadowMaps() );
			m_lightPassShadow[size_t( LightType::eSpot )] = std::make_unique< SpotLightPassShadow >( engine
				, *m_frameBuffer
				, depthAttach
				, gpInfoUbo
				, opaque.getSpotShadowMap() );

			for ( auto & lightPass : m_lightPass )
			{
				lightPass->initialise( scene, sceneUbo );
			}

			for ( auto & lightPass : m_lightPassShadow )
			{
				lightPass->initialise( scene, sceneUbo );
			}
		}
	}

	LightingPass::~LightingPass()
	{
		m_frameBuffer->bind();
		m_frameBuffer->detachAll();
		m_frameBuffer->unbind();
		m_frameBuffer->cleanup();
		m_frameBuffer->destroy();
		m_resultAttach.reset();
		m_result.cleanup();

		for ( auto & lightPass : m_lightPass )
		{
			lightPass->cleanup();
			lightPass.reset();
		}

		for ( auto & lightPass : m_lightPassShadow )
		{
			lightPass->cleanup();
			lightPass.reset();
		}
	}

	bool LightingPass::render( Scene const & scene
		, Camera const & camera
		, GeometryPassResult const & gp
		, RenderInfo & info )
	{
		auto & cache = scene.getLightCache();
		m_frameBuffer->bind( FrameBufferTarget::eDraw );
		m_frameBuffer->clear( BufferComponent::eColour );

		bool first{ true };

		if ( !cache.isEmpty() )
		{
			m_timer->start();
			auto lock = makeUniqueLock( cache );
			doRenderLights( scene
				, camera
				, LightType::eDirectional
				, gp
				, first );
			doRenderLights( scene
				, camera
				, LightType::ePoint
				, gp
				, first );
			doRenderLights( scene
				, camera
				, LightType::eSpot
				, gp
				, first );
			first = false;
			m_timer->stop();
		}

		return first;
	}

	void LightingPass::debugDisplay( Size const & size )const
	{
		m_lightPassShadow[0]->debugDisplay( Position{ 0, 256 } );
		m_lightPassShadow[2]->debugDisplay( Position{ 256, 256 } );
		m_lightPassShadow[1]->debugDisplay( Position{ 512, 256 } );
	}

	void LightingPass::doRenderLights( Scene const & scene
		, Camera const & camera
		, LightType p_type
		, GeometryPassResult const & gp
		, bool & p_first )
	{
		auto & cache = scene.getLightCache();

		if ( cache.getLightsCount( p_type ) )
		{
			auto & lightPass = *m_lightPass[size_t( p_type )];
			auto & lightPassShadow = *m_lightPassShadow[size_t( p_type )];

			for ( auto & light : cache.getLights( p_type ) )
			{
				if ( light->isShadowProducer() )
				{
					lightPassShadow.render( m_size
						, gp
						, *light
						, camera
						, p_first );
				}
				else
				{
					lightPass.render( m_size
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
