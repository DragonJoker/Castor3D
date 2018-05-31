#include "LightingPass.hpp"

#include "DirectionalLightPass.hpp"
#include "LightPassShadow.hpp"
#include "OpaquePass.hpp"
#include "PointLightPass.hpp"
#include "SpotLightPass.hpp"

#include "Engine.hpp"
#include "Render/RenderPassTimer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Light/PointLight.hpp"
#include "Shader/Ubos/MatrixUbo.hpp"
#include "Shader/Ubos/ModelMatrixUbo.hpp"
#include "Technique/RenderTechniquePass.hpp"
#include "Texture/TextureLayout.hpp"
#include "Texture/TextureUnit.hpp"

#include <Buffer/VertexBuffer.hpp>
#include <RenderPass/FrameBuffer.hpp>
#include <RenderPass/FrameBufferAttachment.hpp>

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
			, Size const & size )
		{
			renderer::ImageCreateInfo image{};
			image.arrayLayers = 1u;
			image.extent.width = size.getWidth();
			image.extent.height = size.getHeight();
			image.extent.depth = 1u;
			image.imageType = renderer::TextureType::e2D;
			image.mipLevels = 1u;
			image.samples = renderer::SampleCountFlag::e1;
			image.usage = renderer::ImageUsageFlag::eColourAttachment | renderer::ImageUsageFlag::eSampled;
			image.format = renderer::Format::eR16G16B16A16_SFLOAT;

			auto texture = std::make_shared< TextureLayout >( *engine.getRenderSystem()
				, image
				, renderer::MemoryPropertyFlag::eDeviceLocal );
			texture->getDefaultImage().initialiseSource();

			TextureUnit result{ engine };
			result.setTexture( texture );
			result.setSampler( engine.getLightsSampler() );
			result.initialise();
			return result;
		}
	}

	LightingPass::LightingPass( Engine & engine
		, Size const & size
		, Scene const & scene
		, GeometryPassResult const & gpResult
		, OpaquePass & opaque
		, renderer::TextureView const & depthView
		, SceneUbo & sceneUbo
		, GpInfoUbo & gpInfoUbo )
		: m_size{ size }
		, m_diffuse{ doCreateTexture( engine, size ) }
		, m_specular{ doCreateTexture( engine, size ) }
		, m_timer{ std::make_shared< RenderPassTimer >( engine, cuT( "Opaque" ), cuT( "Lighting pass" ) ) }
	{
		m_lightPass[size_t( LightType::eDirectional )] = std::make_unique< DirectionalLightPass >( engine
			, depthView
			, m_diffuse.getTexture()->getDefaultView()
			, m_specular.getTexture()->getDefaultView()
			, gpInfoUbo
			, false );
		m_lightPass[size_t( LightType::ePoint )] = std::make_unique< PointLightPass >( engine
			, depthView
			, m_diffuse.getTexture()->getDefaultView()
			, m_specular.getTexture()->getDefaultView()
			, gpInfoUbo
			, false );
		m_lightPass[size_t( LightType::eSpot )] = std::make_unique< SpotLightPass >( engine
			, depthView
			, m_diffuse.getTexture()->getDefaultView()
			, m_specular.getTexture()->getDefaultView()
			, gpInfoUbo
			, false );
		m_lightPassShadow[size_t( LightType::eDirectional )] = std::make_unique< DirectionalLightPassShadow >( engine
			, depthView
			, m_diffuse.getTexture()->getDefaultView()
			, m_specular.getTexture()->getDefaultView()
			, gpInfoUbo );
		m_lightPassShadow[size_t( LightType::ePoint )] = std::make_unique< PointLightPassShadow >( engine
			, depthView
			, m_diffuse.getTexture()->getDefaultView()
			, m_specular.getTexture()->getDefaultView()
			, gpInfoUbo );
		m_lightPassShadow[size_t( LightType::eSpot )] = std::make_unique< SpotLightPassShadow >( engine
			, depthView
			, m_diffuse.getTexture()->getDefaultView()
			, m_specular.getTexture()->getDefaultView()
			, gpInfoUbo );

		for ( auto & lightPass : m_lightPass )
		{
			lightPass->initialise( scene
				, gpResult
				, sceneUbo
				, *m_timer );
		}

		for ( auto & lightPass : m_lightPassShadow )
		{
			lightPass->initialise( scene
				, gpResult
				, sceneUbo
				, *m_timer );
		}
	}

	LightingPass::~LightingPass()
	{
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

		m_diffuse.cleanup();
		m_specular.cleanup();
	}

	renderer::Semaphore const & LightingPass::render( Scene const & scene
		, Camera const & camera
		, GeometryPassResult const & gp
		, renderer::Semaphore const & toWait
		, RenderInfo & info )
	{
		auto & cache = scene.getLightCache();
		renderer::Semaphore const * result = &toWait;

		if ( !cache.isEmpty() )
		{
			m_timer->start();
			auto lock = makeUniqueLock( cache );
			auto count = cache.getLightsCount( LightType::eDirectional )
				+ cache.getLightsCount( LightType::ePoint )
				+ cache.getLightsCount( LightType::eSpot );

			if ( m_timer->getCount() != count )
			{
				m_timer->updateCount( count );
			}

			uint32_t index = 0;
			result = &doRenderLights( scene
				, camera
				, LightType::eDirectional
				, gp
				, *result
				, index
				, info );
			result = &doRenderLights( scene
				, camera
				, LightType::ePoint
				, gp
				, *result
				, index
				, info );
			result = &doRenderLights( scene
				, camera
				, LightType::eSpot
				, gp
				, *result
				, index
				, info );
			m_timer->stop();
		}

		return *result;
	}

	void LightingPass::accept( RenderTechniqueVisitor & visitor )
	{
		m_lightPass[size_t( LightType::eDirectional )]->accept( visitor );
		m_lightPass[size_t( LightType::ePoint )]->accept( visitor );
		m_lightPass[size_t( LightType::eSpot )]->accept( visitor );
		m_lightPassShadow[size_t( LightType::eDirectional )]->accept( visitor );
		m_lightPassShadow[size_t( LightType::ePoint )]->accept( visitor );
		m_lightPassShadow[size_t( LightType::eSpot )]->accept( visitor );
	}

	renderer::Semaphore const & LightingPass::doRenderLights( Scene const & scene
		, Camera const & camera
		, LightType p_type
		, GeometryPassResult const & gp
		, renderer::Semaphore const & toWait
		, uint32_t & index
		, RenderInfo & info )
	{
		auto result = &toWait;
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
						lightPassShadow.update( camera.getViewport().getSize()
							, *light
							, camera );
						result = &lightPassShadow.render( index
							, *result
							, &light->getShadowMap()->getTexture() );
					}
					else
					{
						lightPass.update( camera.getViewport().getSize()
							, *light
							, camera );
						result = &lightPass.render( index
							, *result
							, nullptr );
					}

					++index;
					info.m_visibleLightsCount++;
				}

				info.m_totalLightsCount++;
			}
		}

		return *result;
	}
}
