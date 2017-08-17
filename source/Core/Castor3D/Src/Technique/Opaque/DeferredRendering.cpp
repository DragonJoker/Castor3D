#include "DeferredRendering.hpp"

#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "Render/RenderPassTimer.hpp"
#include "Scene/Skybox.hpp"
#include "Shader/PassBuffer/PassBuffer.hpp"
#include "Technique/Opaque/OpaquePass.hpp"
#include "Texture/Sampler.hpp"

using namespace castor;

#define DISPLAY_SHADOW_MAPS 0

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
		, FrameBufferAttachment & depthAttach
		, Size const & size
		, Scene const & scene
		, SsaoConfig const & config )
		: m_engine{ engine }
		, m_ssaoConfig{ config }
		, m_opaquePass{ opaquePass }
		, m_frameBuffer{ frameBuffer }
		, m_depthAttach{ depthAttach }
		, m_size{ size }
		, m_sceneUbo{ engine }
		, m_gpInfoUbo{ engine }
	{
		auto & renderSystem = *engine.getRenderSystem();
		m_geometryPassFrameBuffer = renderSystem.createFrameBuffer();
		bool result = m_geometryPassFrameBuffer->create();

		if ( result )
		{
			result = m_geometryPassFrameBuffer->initialise( m_size );
		}

		if ( result )
		{
			for ( uint32_t i = 0; i < uint32_t( DsTexture::eCount ); i++ )
			{
				auto texture = renderSystem.createTexture( TextureType::eTwoDimensions
					, AccessType::eNone
					, AccessType::eRead | AccessType::eWrite
					, getTextureFormat( DsTexture( i ) )
					, m_size );
				texture->getImage().initialiseSource();

				m_geometryPassResult[i] = std::make_unique< TextureUnit >( engine );
				m_geometryPassResult[i]->setIndex( i );
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
				, m_sceneUbo
				, m_gpInfoUbo );
			m_reflection = std::make_unique< ReflectionPass >( engine
				, m_size
				, m_sceneUbo
				, m_gpInfoUbo );
			m_combinePass = std::make_unique< CombinePass >( engine
				, m_size
				, m_sceneUbo
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

		for ( auto & texture : m_geometryPassResult )
		{
			texture->cleanup();
			texture.reset();
		}

		m_geometryPassFrameBuffer->destroy();
		m_geometryPassFrameBuffer.reset();
		m_combinePass.reset();
		m_reflection.reset();
		m_lightingPass.reset();
		m_sceneUbo.getUbo().cleanup();
	}

	void DeferredRendering::render( RenderInfo & info
		, Scene const & scene
		, Camera const & camera
		, ShadowMapLightTypeArray & shadowMaps )
	{
		m_engine.setPerObjectLighting( false );
		auto invView = camera.getView().getInverse().getTransposed();
		auto invProj = camera.getViewport().getProjection().getInverse();
		auto invViewProj = ( camera.getViewport().getProjection() * camera.getView() ).getInverse();
		camera.apply();
		m_geometryPassFrameBuffer->bind( FrameBufferTarget::eDraw );
		m_geometryPassTexAttachs[size_t( DsTexture::eDepth )]->attach( AttachmentPoint::eDepth );
		m_geometryPassFrameBuffer->clear( BufferComponent::eColour | BufferComponent::eDepth | BufferComponent::eStencil );
		m_opaquePass.render( info
			, shadowMaps );
		m_geometryPassFrameBuffer->unbind();

		blitDepthInto( m_frameBuffer );

		m_sceneUbo.update( scene, camera );
		m_gpInfoUbo.update( m_size
			, camera
			, invViewProj
			, invView
			, invProj );
		m_lightingPass->render( scene
			, camera
			, m_geometryPassResult
			, info );

		if ( scene.hasSkybox() )
		{
			m_reflection->render( m_geometryPassResult
				, m_lightingPass->getResult()
				, scene
				, info );

			if ( scene.getMaterialsType() == MaterialType::ePbrMetallicRoughness
				|| scene.getMaterialsType() == MaterialType::ePbrSpecularGlossiness )
			{
				m_combinePass->render( m_geometryPassResult
					, m_lightingPass->getResult()
					, m_reflection->getReflection()
					, m_reflection->getRefraction()
					, scene.getSkybox().getIbl()
					, scene.getFog()
					, m_frameBuffer
					, info );
			}
			else
			{
				m_combinePass->render( m_geometryPassResult
					, m_lightingPass->getResult()
					, m_reflection->getReflection()
					, m_reflection->getRefraction()
					, scene.getFog()
					, m_frameBuffer
					, info );
			}
		}
		else
		{
			m_combinePass->render( m_geometryPassResult
				, m_lightingPass->getResult()
				, m_reflection->getReflection()
				, m_reflection->getRefraction()
				, scene.getFog()
				, m_frameBuffer
				, info );
		}
	}

	void DeferredRendering::debugDisplay()const
	{
		auto count = 8 + ( m_ssaoConfig.m_enabled ? 1 : 0 );
		int width = int( m_size.getWidth() ) / count;
		int height = int( m_size.getHeight() ) / count;
		int left = int( m_size.getWidth() ) - width;
		auto size = Size( width, height );
		auto & context = *m_engine.getRenderSystem()->getCurrentContext();
		auto index = 0;
		context.renderDepth( Position{ width * index++, 0 }, size, *m_geometryPassResult[size_t( DsTexture::eDepth )]->getTexture() );
		context.renderTexture( Position{ width * index++, 0 }, size, *m_geometryPassResult[size_t( DsTexture::eData1 )]->getTexture() );
		context.renderTexture( Position{ width * index++, 0 }, size, *m_geometryPassResult[size_t( DsTexture::eData2 )]->getTexture() );
		context.renderTexture( Position{ width * index++, 0 }, size, *m_geometryPassResult[size_t( DsTexture::eData3 )]->getTexture() );
		context.renderTexture( Position{ width * index++, 0 }, size, *m_geometryPassResult[size_t( DsTexture::eData4 )]->getTexture() );
		context.renderTexture( Position{ width * index++, 0 }, size, *m_lightingPass->getResult().getTexture() );
		context.renderTexture( Position{ width * index++, 0 }, size, *m_reflection->getReflection().getTexture() );
		context.renderTexture( Position{ width * index++, 0 }, size, *m_reflection->getRefraction().getTexture() );

		if ( m_ssaoConfig.m_enabled )
		{
			context.renderTexture( Position{ width * ( index++ ), 0 }, size, m_combinePass->getSsao() );
		}
	}

	void DeferredRendering::blitDepthInto( FrameBuffer & fbo )
	{
		m_geometryPassFrameBuffer->blitInto( fbo
			, Rectangle{ Position{}, m_size }
		, BufferComponent::eDepth | BufferComponent::eStencil );
	}
}
