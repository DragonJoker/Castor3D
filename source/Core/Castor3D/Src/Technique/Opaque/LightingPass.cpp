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

#include <ShaderWriter/Source.hpp>

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
			ashes::ImageCreateInfo image{};
			image.arrayLayers = 1u;
			image.extent.width = size.getWidth();
			image.extent.height = size.getHeight();
			image.extent.depth = 1u;
			image.imageType = ashes::TextureType::e2D;
			image.mipLevels = 1u;
			image.samples = ashes::SampleCountFlag::e1;
			image.usage = ashes::ImageUsageFlag::eColourAttachment | ashes::ImageUsageFlag::eSampled;
			image.format = ashes::Format::eR16G16B16A16_SFLOAT;

			auto texture = std::make_shared< TextureLayout >( *engine.getRenderSystem()
				, image
				, ashes::MemoryPropertyFlag::eDeviceLocal );
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
		, ashes::TextureView const & depthView
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

	ashes::Semaphore const & LightingPass::render( Scene const & scene
		, Camera const & camera
		, GeometryPassResult const & gp
		, ashes::Semaphore const & toWait
		, RenderInfo & info )
	{
		auto & cache = scene.getLightCache();
		ashes::Semaphore const * result = &toWait;

		if ( !cache.isEmpty() )
		{
			auto timerBlock = m_timer->start();
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

	ashes::Semaphore const & LightingPass::doRenderLights( Scene const & scene
		, Camera const & camera
		, LightType type
		, GeometryPassResult const & gp
		, ashes::Semaphore const & toWait
		, uint32_t & index
		, RenderInfo & info )
	{
		auto result = &toWait;
		auto & cache = scene.getLightCache();

		if ( cache.getLightsCount( type ) )
		{
			auto & lightPass = *m_lightPass[size_t( type )];
			auto & lightPassShadow = *m_lightPassShadow[size_t( type )];

			for ( auto & light : cache.getLights( type ) )
			{
				if ( light->getLightType() == LightType::eDirectional
					|| camera.isVisible( light->getBoundingBox(), light->getParent()->getDerivedTransformationMatrix() ) )
				{
					if ( light->isShadowProducer() && light->getShadowMap() )
					{
						lightPassShadow.update( camera.getSize()
							, *light
							, camera );
						result = &lightPassShadow.render( index
							, *result
							, light->getShadowMap()
							, light->getShadowMapIndex() );
					}
					else
					{
						lightPass.update( camera.getSize()
							, *light
							, camera );
						result = &lightPass.render( index
							, *result
							, nullptr
							, 0u );
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
