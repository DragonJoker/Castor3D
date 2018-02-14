#include "LightingPass.hpp"

#include "DirectionalLightPass.hpp"
#include "LightPassShadow.hpp"
#include "OpaquePass.hpp"
#include "PointLightPass.hpp"
#include "SpotLightPass.hpp"

#include <Engine.hpp>
#include <Buffer/GeometryBuffers.hpp>
#include <Buffer/VertexBuffer.hpp>
#include <FrameBuffer/FrameBuffer.hpp>
#include <FrameBuffer/RenderBufferAttachment.hpp>
#include <Render/RenderPassTimer.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Scene/Scene.hpp>
#include <Scene/Camera.hpp>
#include <Scene/Light/PointLight.hpp>
#include <Shader/Ubos/MatrixUbo.hpp>
#include <Shader/Ubos/ModelMatrixUbo.hpp>
#include <Shader/ShaderProgram.hpp>
#include <Technique/RenderTechniquePass.hpp>
#include <Texture/TextureLayout.hpp>
#include <Texture/TextureUnit.hpp>

#include <GlslSource.hpp>

#include "Shader/Shaders/GlslLight.hpp"
#include "Shader/Shaders/GlslShadow.hpp"

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	namespace
	{
		TextureUnit doCreateTexture( Engine & engine
			, Size const & size
			, uint32_t index )
		{
			auto texture = engine.getRenderSystem()->createTexture( TextureType::eTwoDimensions
				, AccessType::eNone
				, AccessType::eRead | AccessType::eWrite
				, PixelFormat::eRGB16F32F
				, size );
			texture->getImage().initialiseSource();

			TextureUnit result{ engine };
			result.setIndex( index );
			result.setTexture( texture );
			result.setSampler( engine.getLightsSampler() );
			result.initialise();
			return result;
		}
	}

	LightingPass::LightingPass( Engine & engine
		, Size const & size
		, Scene const & scene
		, OpaquePass & opaque
		, FrameBufferAttachment & depthAttach
		, SceneUbo & sceneUbo
		, GpInfoUbo & gpInfoUbo )
		: m_size{ size }
		, m_diffuse{ doCreateTexture( engine, size, MinTextureIndex + 0u ) }
		, m_specular{ doCreateTexture( engine, size, MinTextureIndex + 1u ) }
		, m_frameBuffer{ engine.getRenderSystem()->createFrameBuffer() }
		, m_timer{ std::make_shared< RenderPassTimer >( engine, cuT( "Lighting" ), cuT( "Lighting" ) ) }
	{
		m_frameBuffer->setClearColour( RgbaColour::fromPredefined( PredefinedRgbaColour::eTransparentBlack ) );
		bool result = m_frameBuffer->initialise();

		if ( result )
		{
			m_diffuseAttach = m_frameBuffer->createAttachment( m_diffuse.getTexture() );
			m_specularAttach = m_frameBuffer->createAttachment( m_specular.getTexture() );

			m_frameBuffer->bind();
			m_frameBuffer->attach( AttachmentPoint::eColour
				, 0u
				, m_diffuseAttach
				, TextureType::eTwoDimensions );
			m_frameBuffer->attach( AttachmentPoint::eColour
				, 1u
				, m_specularAttach
				, TextureType::eTwoDimensions );
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
				, gpInfoUbo );
			m_lightPassShadow[size_t( LightType::ePoint )] = std::make_unique< PointLightPassShadow >( engine
				, *m_frameBuffer
				, depthAttach
				, gpInfoUbo );
			m_lightPassShadow[size_t( LightType::eSpot )] = std::make_unique< SpotLightPassShadow >( engine
				, *m_frameBuffer
				, depthAttach
				, gpInfoUbo );

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
		m_diffuseAttach.reset();
		m_specularAttach.reset();
		m_diffuse.cleanup();
		m_specular.cleanup();

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
				, first
				, info );
			doRenderLights( scene
				, camera
				, LightType::ePoint
				, gp
				, first
				, info );
			doRenderLights( scene
				, camera
				, LightType::eSpot
				, gp
				, first
				, info );
			first = false;
			m_timer->stop();
		}

		return first;
	}

	void LightingPass::doRenderLights( Scene const & scene
		, Camera const & camera
		, LightType p_type
		, GeometryPassResult const & gp
		, bool & p_first
		, RenderInfo & info )
	{
		auto & cache = scene.getLightCache();

		if ( cache.getLightsCount( p_type ) )
		{
			auto & lightPass = *m_lightPass[size_t( p_type )];
			auto & lightPassShadow = *m_lightPassShadow[size_t( p_type )];

			for ( auto & light : cache.getLights( p_type ) )
			{
				if ( light->getLightType() == LightType::eDirectional
					|| camera.isVisible( light->getBoundingBox(), light->getParent()->getDerivedTransformationMatrix() ) )
				{
					if ( light->isShadowProducer() && light->getShadowMap() )
					{
						lightPassShadow.render( m_size
							, gp
							, *light
							, camera
							, p_first
							, light->getShadowMap() );
					}
					else
					{
						lightPass.render( m_size
							, gp
							, *light
							, camera
							, p_first
							, nullptr );
					}

					p_first = false;
					info.m_visibleLightsCount++;
				}

				info.m_totalLightsCount++;
			}
		}
	}
}
