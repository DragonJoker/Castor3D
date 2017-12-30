#include "DeferredRendering.hpp"

#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "Render/RenderPassTimer.hpp"
#include "Scene/Skybox.hpp"
#include "Shader/PassBuffer/PassBuffer.hpp"
#include "Technique/Opaque/OpaquePass.hpp"
#include "Texture/Sampler.hpp"

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
				result = engine.getRenderSystem()->createSampler( name );
				result->setInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eNearest );
				result->setInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eNearest );
				result->setWrappingMode( TextureUVW::eU, WrapMode::eClampToEdge );
				result->setWrappingMode( TextureUVW::eV, WrapMode::eClampToEdge );
				result->setWrappingMode( TextureUVW::eW, WrapMode::eClampToEdge );
				result->initialise();
				cache.add( name, result );
			}

			return result;
		}
	}

	//*********************************************************************************************

	DeferredRendering::DeferredRendering( Engine & engine
		, OpaquePass & opaquePass
		, FrameBuffer & frameBuffer
		, TextureAttachment & depthAttach
		, Size const & size
		, Scene const & scene
		, SsaoConfig const & config )
		: m_engine{ engine }
		, m_ssaoConfig{ config }
		, m_opaquePass{ opaquePass }
		, m_frameBuffer{ frameBuffer }
		, m_depthAttach{ depthAttach }
		, m_size{ size }
		, m_gpInfoUbo{ engine }
	{
		auto & renderSystem = *engine.getRenderSystem();
		m_geometryPassFrameBuffer = renderSystem.createFrameBuffer();
		bool result = m_geometryPassFrameBuffer->initialise();

		if ( result )
		{
			m_geometryPassResult[0] = std::make_unique< TextureUnit >( engine );
			m_geometryPassResult[0]->setIndex( MinTextureIndex );
			m_geometryPassResult[0]->setTexture( m_depthAttach.getTexture() );
			m_geometryPassResult[0]->setSampler( doCreateSampler( engine, getTextureName( DsTexture::eDepth ) ) );
			m_geometryPassResult[0]->initialise();
			m_geometryPassTexAttachs[0] = m_geometryPassFrameBuffer->createAttachment( m_depthAttach.getTexture() );

			for ( auto i = uint32_t( DsTexture::eData1 ); i < uint32_t( DsTexture::eCount ); i++ )
			{
				auto texture = renderSystem.createTexture( TextureType::eTwoDimensions
					, AccessType::eNone
					, AccessType::eRead | AccessType::eWrite
					, getTextureFormat( DsTexture( i ) )
					, m_size );
				texture->getImage().initialiseSource();

				m_geometryPassResult[i] = std::make_unique< TextureUnit >( engine );
				m_geometryPassResult[i]->setIndex( MinTextureIndex + i );
				m_geometryPassResult[i]->setTexture( texture );
				m_geometryPassResult[i]->setSampler( doCreateSampler( engine, getTextureName( DsTexture( i ) ) ) );
				m_geometryPassResult[i]->initialise();

				m_geometryPassTexAttachs[i] = m_geometryPassFrameBuffer->createAttachment( texture );
			}

			m_geometryPassFrameBuffer->bind();

			for ( int i = 0; i < size_t( DsTexture::eCount ) && result; i++ )
			{
				m_geometryPassFrameBuffer->attach( getTextureAttachmentPoint( DsTexture( i ) )
					, getTextureAttachmentIndex( DsTexture( i ) )
					, m_geometryPassTexAttachs[i]
					, m_geometryPassResult[i]->getType() );
			}

			ENSURE( m_geometryPassFrameBuffer->isComplete() );
			m_geometryPassFrameBuffer->setDrawBuffers();
			m_geometryPassFrameBuffer->unbind();
		}

		if ( result )
		{
			m_lightingPass = std::make_unique< LightingPass >( engine
				, m_size
				, scene
				, m_opaquePass
				, m_depthAttach
				, m_opaquePass.getSceneUbo()
				, m_gpInfoUbo );
			m_subsurfaceScattering = std::make_unique< SubsurfaceScatteringPass >( engine
				, m_gpInfoUbo
				, m_opaquePass.getSceneUbo()
				, m_size );
			m_reflection = std::make_unique< ReflectionPass >( engine
				, m_size
				, m_opaquePass.getSceneUbo()
				, m_gpInfoUbo
				, m_ssaoConfig );
		}

		ENSURE( result );
	}

	DeferredRendering::~DeferredRendering()
	{
		m_geometryPassFrameBuffer->bind();
		m_geometryPassFrameBuffer->detachAll();
		m_geometryPassFrameBuffer->unbind();
		m_geometryPassFrameBuffer->cleanup();

		for ( auto & attach : m_geometryPassTexAttachs )
		{
			attach.reset();
		}

		m_geometryPassResult[0].reset();

		for ( uint32_t i = uint32_t( DsTexture::eData1 ); i < uint32_t( DsTexture::eCount ); i++ )
		{
			m_geometryPassResult[i]->cleanup();
			m_geometryPassResult[i].reset();
		}

		m_geometryPassFrameBuffer.reset();
		m_reflection.reset();
		m_subsurfaceScattering.reset();
		m_lightingPass.reset();
	}

	void DeferredRendering::render( RenderInfo & info
		, Scene const & scene
		, Camera const & camera
		, ShadowMapLightTypeArray & shadowMaps
		, Point2r const & jitter
		, TextureUnit const & velocity )
	{
		m_engine.setPerObjectLighting( false );
		auto invView = camera.getView().getInverse().getTransposed();
		auto invProj = camera.getViewport().getProjection().getInverse();
		auto invViewProj = ( camera.getViewport().getProjection() * camera.getView() ).getInverse();
		camera.apply();
		m_opaquePass.getSceneUbo().update( scene, camera );

		m_geometryPassFrameBuffer->bind( FrameBufferTarget::eDraw );
		auto velocityAttach = m_geometryPassFrameBuffer->createAttachment( velocity.getTexture() );
		m_geometryPassFrameBuffer->attach( AttachmentPoint::eColour
			, getTextureAttachmentIndex( DsTexture::eMax ) + 1u
			, velocityAttach
			, velocity.getType() );
		REQUIRE( m_geometryPassFrameBuffer->isComplete() );
		m_geometryPassTexAttachs[size_t( DsTexture::eDepth )]->attach( AttachmentPoint::eDepth );
		m_geometryPassFrameBuffer->setDrawBuffers();
		m_geometryPassFrameBuffer->clear( BufferComponent::eColour );
		m_opaquePass.render( info
			, shadowMaps
			, jitter );
		m_geometryPassFrameBuffer->detach( velocityAttach );
		m_geometryPassFrameBuffer->unbind();

		m_gpInfoUbo.update( m_size
			, camera
			, invViewProj
			, invView
			, invProj );
		m_lightingPass->render( scene
			, camera
			, m_geometryPassResult
			, info );

		if ( scene.needsSubsurfaceScattering() )
		{
			m_subsurfaceScattering->render( m_geometryPassResult
				, m_lightingPass->getDiffuse() );
			m_reflection->render( m_geometryPassResult
				, m_subsurfaceScattering->getResult()
				, m_lightingPass->getSpecular()
				, scene
				, camera
				, m_frameBuffer
				, info );
		}
		else
		{
			m_reflection->render( m_geometryPassResult
				, m_lightingPass->getDiffuse()
				, m_lightingPass->getSpecular()
				, scene
				, camera
				, m_frameBuffer
				, info );
		}
	}

	void DeferredRendering::debugDisplay()const
	{
		auto count = 8 + ( m_ssaoConfig.m_enabled ? 1 : 0 );
		int width = int( m_size.getWidth() ) / count;
		int height = int( m_size.getHeight() ) / count;
		auto size = Size( width, height );
		auto & context = *m_engine.getRenderSystem()->getCurrentContext();
		auto index = 0;
		context.renderDepth( Position{ width * index++, 0 }, size, *m_geometryPassResult[size_t( DsTexture::eDepth )]->getTexture() );
		context.renderTexture( Position{ width * index++, 0 }, size, *m_geometryPassResult[size_t( DsTexture::eData1 )]->getTexture() );
		context.renderTexture( Position{ width * index++, 0 }, size, *m_geometryPassResult[size_t( DsTexture::eData2 )]->getTexture() );
		context.renderTexture( Position{ width * index++, 0 }, size, *m_geometryPassResult[size_t( DsTexture::eData3 )]->getTexture() );
		context.renderTexture( Position{ width * index++, 0 }, size, *m_geometryPassResult[size_t( DsTexture::eData4 )]->getTexture() );
		context.renderTexture( Position{ width * index++, 0 }, size, *m_geometryPassResult[size_t( DsTexture::eData5 )]->getTexture() );
		context.renderTexture( Position{ width * index++, 0 }, size, *m_lightingPass->getDiffuse().getTexture() );
		context.renderTexture( Position{ width * index++, 0 }, size, *m_lightingPass->getSpecular().getTexture() );

		if ( m_ssaoConfig.m_enabled )
		{
			context.renderTexture( Position{ width * ( index++ ), 0 }, size, m_reflection->getSsao() );
		}

		m_subsurfaceScattering->debugDisplay( m_size );
	}
}
