#include "ShadowMapSpot.hpp"

#include "Engine.hpp"

#include "Render/RenderSystem.hpp"
#include "Scene/Light/Light.hpp"
#include "Scene/Light/SpotLight.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Shader/Shaders/GlslMaterial.hpp"
#include "ShadowMap/ShadowMapPassSpot.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureView.hpp"
#include "Texture/TextureLayout.hpp"

#include <Image/Texture.hpp>
#include <Image/TextureView.hpp>
#include <RenderPass/RenderPass.hpp>
#include <RenderPass/RenderPassState.hpp>
#include <RenderPass/RenderSubpass.hpp>
#include <RenderPass/RenderSubpassState.hpp>
#include <RenderPass/TextureAttachment.hpp>

#include <GlslSource.hpp>

#include <Graphics/Image.hpp>
#include <Miscellaneous/BlockTracker.hpp>

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	namespace
	{
		TextureUnit doInitialiseVariance( Engine & engine
			, Size const & size )
		{
			String const name = cuT( "ShadowMap_Spot_Variance" );
			SamplerSPtr sampler;

			if ( engine.getSamplerCache().has( name ) )
			{
				sampler = engine.getSamplerCache().find( name );
			}
			else
			{
				sampler = engine.getSamplerCache().add( name );
				sampler->setMinFilter( renderer::Filter::eLinear );
				sampler->setMagFilter( renderer::Filter::eLinear );
				sampler->setWrapS( renderer::WrapMode::eClampToBorder );
				sampler->setWrapT( renderer::WrapMode::eClampToBorder );
				sampler->setWrapR( renderer::WrapMode::eClampToBorder );
				sampler->setBorderColour( renderer::BorderColour::eFloatOpaqueWhite );
			}

			auto texture = std::make_shared< TextureLayout >( *engine.getRenderSystem()
				, renderer::TextureType::e2D
				, renderer::ImageUsageFlag::eColourAttachment | renderer::ImageUsageFlag::eSampled
				, renderer::MemoryPropertyFlag::eDeviceLocal
				, PixelFormat::eAL32F
				, size );
			TextureUnit unit{ engine };
			unit.setTexture( texture );
			unit.setSampler( sampler );

			for ( auto & image : *texture )
			{
				image->initialiseSource();
			}

			return unit;
		}

		TextureUnit doInitialiseLinearDepth( Engine & engine
			, Size const & size )
		{
			String const name = cuT( "ShadowMap_Spot_Depth" );
			SamplerSPtr sampler;

			if ( engine.getSamplerCache().has( name ) )
			{
				sampler = engine.getSamplerCache().find( name );
			}
			else
			{
				sampler = engine.getSamplerCache().add( name );
				sampler->setMinFilter( renderer::Filter::eLinear );
				sampler->setMagFilter( renderer::Filter::eLinear );
				sampler->setWrapS( renderer::WrapMode::eClampToEdge );
				sampler->setWrapT( renderer::WrapMode::eClampToEdge );
				sampler->setWrapR( renderer::WrapMode::eClampToEdge );
			}

			auto texture = std::make_shared< TextureLayout >( *engine.getRenderSystem()
				, renderer::TextureType::e2D
				, renderer::ImageUsageFlag::eColourAttachment | renderer::ImageUsageFlag::eSampled
				, renderer::MemoryPropertyFlag::eDeviceLocal
				, PixelFormat::eL32F
				, size );
			TextureUnit unit{ engine };
			unit.setTexture( texture );
			unit.setSampler( sampler );

			for ( auto & image : *texture )
			{
				image->initialiseSource();
			}

			return unit;
		}
	}

	ShadowMapSpot::ShadowMapSpot( Engine & engine
		, Scene & scene )
		: ShadowMap{ engine
			, doInitialiseVariance( engine, Size{ ShadowMapPassSpot::TextureSize, ShadowMapPassSpot::TextureSize } )
			, doInitialiseLinearDepth( engine, Size{ ShadowMapPassSpot::TextureSize, ShadowMapPassSpot::TextureSize } )
			, std::make_shared< ShadowMapPassSpot >( engine, scene, *this ) }
	{
	}

	ShadowMapSpot::~ShadowMapSpot()
	{
	}
	
	void ShadowMapSpot::update( Camera const & camera
		, RenderQueueArray & queues
		, Light & light
		, uint32_t index )
	{
		m_pass->update( camera, queues, light, index );
	}

	void ShadowMapSpot::render()
	{
		static renderer::RgbaColour const black = renderer::RgbaColour::fromPredefined( PredefinedRgbaColour::eOpaqueBlack );
		static renderer::DepthStencilClearValue const zero{ 1.0f, 0 };
		auto & device = *getEngine()->getRenderSystem()->getCurrentDevice();

		if ( m_commandBuffer->begin( renderer::CommandBufferUsageFlag::eOneTimeSubmit ) )
		{
			m_pass->startTimer( *m_commandBuffer );
			m_commandBuffer->beginRenderPass( *m_renderPass
				, *m_frameBuffer
				, { zero, black, black }
				, renderer::SubpassContents::eSecondaryCommandBuffers );
			m_commandBuffer->executeCommands( { m_pass->getCommandBuffer() } );
			m_commandBuffer->endRenderPass();
			m_pass->stopTimer( *m_commandBuffer );
			m_commandBuffer->end();

			device.getGraphicsQueue().submit( *m_commandBuffer, nullptr );
			m_blur->blur();
		}
	}

	void ShadowMapSpot::debugDisplay( castor::Size const & size, uint32_t index )
	{
		//Size displaySize{ 256u, 256u };
		//Position position{ int32_t( ( displaySize.getWidth() + 1 ) * index * 3u ), int32_t( displaySize.getHeight() * 2u ) };
		//getEngine()->getRenderSystem()->getCurrentDevice()->renderVariance( position
		//	, displaySize
		//	, *m_shadowMap.getTexture() );
		//position.offset( 2 * ( displaySize.getWidth() + 1 ), 0 );
		//getEngine()->getRenderSystem()->getCurrentDevice()->renderTexture( position
		//	, displaySize
		//	, *m_linearMap.getTexture() );
	}

	void ShadowMapSpot::doInitialise()
	{
		renderer::UIVec2 size{ ShadowMapPassSpot::TextureSize, ShadowMapPassSpot::TextureSize };
		auto & device = *getEngine()->getRenderSystem()->getCurrentDevice();
		m_depthBuffer = device.createTexture();
		m_depthBuffer->setImage( PixelFormat::eD24
			, size
			, renderer::ImageUsageFlag::eDepthStencilAttachment );
		m_depthBufferView = m_depthBuffer->createView( renderer::TextureType::e2D
			, PixelFormat::eD24
			, 0u
			, 1u
			, 0u
			, 1u );
		std::vector< renderer::PixelFormat > formats{ { PixelFormat::eD24, PixelFormat::eAL32F, PixelFormat::eL32F } };
		renderer::RenderSubpassPtrArray subpasses;
		subpasses.emplace_back( device.createRenderSubpass( formats
			, renderer::RenderSubpassState{ renderer::PipelineStageFlag::eColourAttachmentOutput
			, renderer::AccessFlag::eColourAttachmentWrite } ) );
		m_renderPass = device.createRenderPass( formats
			, subpasses
			, renderer::RenderPassState{ renderer::PipelineStageFlag::eColourAttachmentOutput
				, renderer::AccessFlag::eColourAttachmentWrite
				, { renderer::ImageLayout::eDepthStencilAttachmentOptimal, renderer::ImageLayout::eColourAttachmentOptimal, renderer::ImageLayout::eColourAttachmentOptimal } }
			, renderer::RenderPassState{ renderer::PipelineStageFlag::eColourAttachmentOutput
				, renderer::AccessFlag::eColourAttachmentWrite
				, { renderer::ImageLayout::eDepthStencilAttachmentOptimal, renderer::ImageLayout::eColourAttachmentOptimal, renderer::ImageLayout::eColourAttachmentOptimal } } );
		renderer::TextureAttachmentPtrArray attaches;
		attaches.emplace_back( std::make_unique< renderer::TextureAttachment >( m_shadowMap.getTexture()->getView() ) );
		attaches.emplace_back( std::make_unique< renderer::TextureAttachment >( m_linearMap.getTexture()->getView() ) );
		attaches.emplace_back( std::make_unique< renderer::TextureAttachment >( *m_depthBufferView ) );
		m_frameBuffer = m_renderPass->createFrameBuffer( size, std::move( attaches ) );

		m_commandBuffer = device.getGraphicsCommandPool().createCommandBuffer();

		m_blur = std::make_unique< GaussianBlur >( *getEngine()
			, *m_shadowMap.getTexture()
			, m_shadowMap.getTexture()->getDimensions()
			, m_shadowMap.getTexture()->getPixelFormat()
			, 5u );
	}

	void ShadowMapSpot::doCleanup()
	{
		m_frameBuffer.reset();
		m_renderPass.reset();
		m_blur.reset();
		m_depthBuffer.reset();
	}

	void ShadowMapSpot::doUpdateFlags( PassFlags & passFlags
		, TextureChannels & textureFlags
		, ProgramFlags & programFlags
		, SceneFlags & sceneFlags )const
	{
		addFlag( programFlags, ProgramFlag::eShadowMapSpot );
	}

	glsl::Shader ShadowMapSpot::doGetPixelShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, renderer::CompareOp alphaFunc )const
	{
		using namespace glsl;
		GlslWriter writer = getEngine()->getRenderSystem()->createGlslWriter();

		// Fragment Intputs
		Ubo shadowMap{ writer, ShadowMapPassSpot::ShadowMapUbo, ShadowMapPassSpot::UboBindingPoint };
		auto c3d_farPlane( shadowMap.declMember< Float >( ShadowMapPassSpot::FarPlane ) );
		shadowMap.end();

		auto vtx_texture = writer.declInput< Vec3 >( cuT( "vtx_texture" ) );
		auto vtx_worldPosition = writer.declOutput< Vec3 >( cuT( "vtx_worldPosition" ) );
		auto vtx_viewPosition = writer.declInput< Vec3 >( cuT( "vtx_viewPosition" ) );
		auto vtx_material = writer.declInput< Int >( cuT( "vtx_material" ) );
		auto c3d_mapOpacity( writer.declSampler< Sampler2D >( cuT( "c3d_mapOpacity" )
			, MinTextureIndex
			, 0u
			, checkFlag( textureFlags, TextureChannel::eOpacity ) ) );
		auto gl_FragCoord( writer.declBuiltin< Vec4 >( cuT( "gl_FragCoord" ) ) );

		auto materials = shader::createMaterials( writer, passFlags );
		materials->declare();

		// Fragment Outputs
		auto pxl_depth( writer.declFragData< Vec2 >( cuT( "pxl_depth" ), 0 ) );
		auto pxl_linear( writer.declFragData< Float >( cuT( "pxl_linear" ), 1 ) );

		writer.implementFunction< void >( cuT( "main" ), [&]()
		{
			doDiscardAlpha( writer
				, textureFlags
				, alphaFunc
				, vtx_material
				, *materials );

			auto depth = writer.declLocale( cuT( "depth" )
				, gl_FragCoord.z() );
			pxl_depth.x() = depth;
			pxl_depth.y() = pxl_depth.x() * pxl_depth.x();
			pxl_linear = length( vtx_viewPosition ) / c3d_farPlane;
			//pxl_linear = vtx_viewPosition.z()/* / c3d_farPlane*/;
			pxl_linear = abs( vtx_viewPosition.z() ) / c3d_farPlane;

			auto dx = writer.declLocale( cuT( "dx" )
				, dFdx( depth ) );
			auto dy = writer.declLocale( cuT( "dy" )
				, dFdy( depth ) );
			pxl_depth.y() += 0.25_f * writer.paren( dx * dx + dy * dy );
		} );

		return writer.finalise();
	}
}
