#include "DeferredRendering.hpp"

#include "Render/RenderPassTimer.hpp"
#include "Scene/Skybox.hpp"
#include "Shader/PassBuffer/PassBuffer.hpp"
#include "Technique/Opaque/OpaquePass.hpp"
#include "Texture/Sampler.hpp"

#include <RenderPass/FrameBuffer.hpp>
#include <RenderPass/FrameBufferAttachment.hpp>

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		SamplerSPtr doCreateSampler( Engine & engine
			, String const & name )
		{
			SamplerSPtr result;
			auto & cache = engine.getSamplerCache();

			if ( cache.has( name ) )
			{
				result = cache.find( name );
			}
			else
			{
				result = engine.getSamplerCache().add( cuT( "TextureProjection" ) );
				result->setMinFilter( renderer::Filter::eNearest );
				result->setMagFilter( renderer::Filter::eNearest );
				result->setWrapS( renderer::WrapMode::eClampToEdge );
				result->setWrapT( renderer::WrapMode::eClampToEdge );
				result->setWrapR( renderer::WrapMode::eClampToEdge );
				result->initialise();
			}

			return result;
		}

		TextureLayoutSPtr doCreateTexture( RenderSystem & renderSystem
			, renderer::Format format
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
			image.format = format;
			return std::make_shared< TextureLayout >( renderSystem
				, image
				, renderer::MemoryPropertyFlag::eDeviceLocal );
		}
	}

	//*********************************************************************************************

	DeferredRendering::DeferredRendering( Engine & engine
		, OpaquePass & opaquePass
		, TextureLayoutSPtr depthTexture
		, TextureLayoutSPtr velocityTexture
		, Size const & size
		, Scene const & scene
		, SsaoConfig const & config )
		: m_engine{ engine }
		, m_ssaoConfig{ config }
		, m_opaquePass{ opaquePass }
		, m_size{ size }
		, m_gpInfoUbo{ engine }
	{
		auto & renderSystem = *engine.getRenderSystem();

		m_geometryPassResult[0] = std::make_unique< TextureUnit >( engine );
		m_geometryPassResult[0]->setTexture( depthTexture );
		m_geometryPassResult[0]->setSampler( doCreateSampler( engine, getTextureName( DsTexture::eDepth ) ) );
		m_geometryPassResult[0]->initialise();

		m_geometryPassResult[uint32_t( DsTexture::eData5 )] = std::make_unique< TextureUnit >( engine );
		m_geometryPassResult[uint32_t( DsTexture::eData5 )]->setTexture( velocityTexture );
		m_geometryPassResult[uint32_t( DsTexture::eData5 )]->setSampler( doCreateSampler( engine, getTextureName( DsTexture::eData5 ) ) );
		m_geometryPassResult[uint32_t( DsTexture::eData5 )]->initialise();

		for ( auto i = uint32_t( DsTexture::eData1 ); i < uint32_t( DsTexture::eData5 ); i++ )
		{
			auto texture = doCreateTexture( renderSystem
				, getTextureFormat( DsTexture( i ) )
				, m_size );
			texture->getImage().initialiseSource();

			m_geometryPassResult[i] = std::make_unique< TextureUnit >( engine );
			m_geometryPassResult[i]->setTexture( texture );
			m_geometryPassResult[i]->setSampler( doCreateSampler( engine, getTextureName( DsTexture( i ) ) ) );
			m_geometryPassResult[i]->initialise();
		}

		m_lightingPass = std::make_unique< LightingPass >( engine
			, m_size
			, scene
			, m_geometryPassResult
			, m_opaquePass
			, depthTexture->getDefaultView()
			, m_opaquePass.getSceneUbo()
			, m_gpInfoUbo );
		m_subsurfaceScattering = std::make_unique< SubsurfaceScatteringPass >( engine
			, m_gpInfoUbo
			, m_opaquePass.getSceneUbo()
			, m_size
			, m_geometryPassResult
			, m_lightingPass->getDiffuse() );

		if ( scene.needsSubsurfaceScattering() )
		{
			m_reflection = std::make_unique< ReflectionPass >( engine
				, m_size
				, m_opaquePass.getSceneUbo()
				, m_gpInfoUbo
				, m_ssaoConfig
				, m_subsurfaceScattering->getResult()
				, m_lightingPass->getSpecular() );
		}
		else
		{
			m_reflection = std::make_unique< ReflectionPass >( engine
				, m_size
				, m_opaquePass.getSceneUbo()
				, m_gpInfoUbo
				, m_ssaoConfig
				, m_lightingPass->getDiffuse()
				, m_lightingPass->getSpecular() );
		}
	}

	DeferredRendering::~DeferredRendering()
	{
		m_geometryPassResult[0].reset();

		for ( auto i = uint32_t( DsTexture::eData1 ); i < uint32_t( DsTexture::eCount ); i++ )
		{
			m_geometryPassResult[i]->cleanup();
			m_geometryPassResult[i].reset();
		}

		m_reflection.reset();
		m_subsurfaceScattering.reset();
		m_lightingPass.reset();
	}

	void DeferredRendering::render( RenderInfo & info
		, Scene const & scene
		, Camera const & camera
		, ShadowMapLightTypeArray & shadowMaps
		, Point2r const & jitter
		, renderer::Semaphore const & toWait )
	{
		m_engine.setPerObjectLighting( false );
		auto invView = camera.getView().getInverse().getTransposed();
		auto invProj = camera.getViewport().getProjection().getInverse();
		auto invViewProj = ( camera.getViewport().getProjection() * camera.getView() ).getInverse();
		m_opaquePass.getSceneUbo().update( scene, camera );
		m_gpInfoUbo.update( m_size
			, camera
			, invViewProj
			, invView
			, invProj );
		m_opaquePass.update( info
			, shadowMaps
			, jitter );

		auto & device = *m_engine.getRenderSystem()->getCurrentDevice();
		device.getGraphicsQueue().submit( m_opaquePass.getCommandBuffer()
			, toWait
			, renderer::PipelineStageFlag::eAllCommands
			, m_opaquePass.getSemaphore()
			, nullptr );

		auto * semaphore = m_lightingPass->render( scene
			, camera
			, m_geometryPassResult
			, m_opaquePass.getSemaphore()
			, info );

		semaphore = semaphore ? semaphore : &m_opaquePass.getSemaphore();

		if ( scene.needsSubsurfaceScattering() )
		{
			device.getGraphicsQueue().submit( m_subsurfaceScattering->getCommandBuffer()
				, toWait
				, renderer::PipelineStageFlag::eAllCommands
				, *semaphore
				, nullptr );
			semaphore = &m_subsurfaceScattering->getSemaphore();
		}

		m_reflection->render( m_geometryPassResult
			, *semaphore
			, scene
			, camera );
	}

	void DeferredRendering::debugDisplay()const
	{
		//auto count = 8 + ( m_ssaoConfig.m_enabled ? 1 : 0 );
		//int width = int( m_size.getWidth() ) / count;
		//int height = int( m_size.getHeight() ) / count;
		//auto size = Size( width, height );
		//auto & context = *m_engine.getRenderSystem()->getCurrentContext();
		//auto index = 0;
		//context.renderDepth( Position{ width * index++, 0 }, size, *m_geometryPassResult[size_t( DsTexture::eDepth )]->getTexture() );
		//context.renderTexture( Position{ width * index++, 0 }, size, *m_geometryPassResult[size_t( DsTexture::eData1 )]->getTexture() );
		//context.renderTexture( Position{ width * index++, 0 }, size, *m_geometryPassResult[size_t( DsTexture::eData2 )]->getTexture() );
		//context.renderTexture( Position{ width * index++, 0 }, size, *m_geometryPassResult[size_t( DsTexture::eData3 )]->getTexture() );
		//context.renderTexture( Position{ width * index++, 0 }, size, *m_geometryPassResult[size_t( DsTexture::eData4 )]->getTexture() );
		//context.renderTexture( Position{ width * index++, 0 }, size, *m_geometryPassResult[size_t( DsTexture::eData5 )]->getTexture() );
		//context.renderTexture( Position{ width * index++, 0 }, size, *m_lightingPass->getDiffuse().getTexture() );
		//context.renderTexture( Position{ width * index++, 0 }, size, *m_lightingPass->getSpecular().getTexture() );

		//if ( m_ssaoConfig.m_enabled )
		//{
		//	context.renderTexture( Position{ width * ( index++ ), 0 }, size, m_reflection->getSsao() );
		//}

		//m_subsurfaceScattering->debugDisplay( m_size );
	}
}
