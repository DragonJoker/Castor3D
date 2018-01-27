#include "FxaaPostEffect.hpp"

#include <Engine.hpp>
#include <Cache/SamplerCache.hpp>
#include <Cache/ShaderCache.hpp>

#include <Miscellaneous/Parameter.hpp>
#include <Render/RenderSystem.hpp>
#include <Render/RenderTarget.hpp>
#include <Texture/Sampler.hpp>
#include <Texture/TextureLayout.hpp>
#include <Texture/TextureUnit.hpp>

#include <Image/Texture.hpp>
#include <Image/TextureView.hpp>
#include <RenderPass/RenderPass.hpp>
#include <RenderPass/RenderPassState.hpp>
#include <RenderPass/RenderSubpass.hpp>
#include <RenderPass/RenderSubpassState.hpp>
#include <Shader/ShaderProgram.hpp>
#include <Sync/ImageMemoryBarrier.hpp>

#include <GlslSource.hpp>

#include <numeric>

using namespace castor;

namespace fxaa
{
	namespace
	{
		static String const FxaaUbo = cuT( "FxaaUbo" );
		static String const SubpixShift = cuT( "c3d_fSubpixShift" );
		static String const RenderTargetWidth = cuT( "c3d_fRenderTargetWidth" );
		static String const RenderTargetHeight = cuT( "c3d_fRenderTargetHeight" );
		static String const SpanMax = cuT( "c3d_fSpanMax" );
		static String const ReduceMul = cuT( "c3d_fReduceMul" );
		static String const PosPos = cuT( "vtx_posPos" );

		glsl::Shader getFxaaVertexProgram( castor3d::RenderSystem * renderSystem )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem->createGlslWriter();

			// Shader inputs
			UBO_FXAA( writer, 0u );
			auto position = writer.declAttribute< Vec2 >( cuT( "position" ) );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ) );
			auto vtx_posPos = writer.declOutput< Vec4 >( PosPos );
			auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
				{
					auto invTargetSize = writer.declLocale( cuT( "invTargetSize" )
						, vec2( 1.0 / c3d_renderSize.x(), 1.0 / c3d_renderSize.y() ) );
					vtx_texture = position;
					gl_Position = vec4( position.xy(), 0.0, 1.0 );
					vtx_posPos.xy() = position.xy();
					vtx_posPos.zw() = position.xy() - writer.paren( invTargetSize * writer.paren( 0.5 + c3d_subpixShift ) );
				} );
			return writer.finalise();
		}

		glsl::Shader getFxaaFragmentProgram( castor3d::RenderSystem * renderSystem )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem->createGlslWriter();

			// Shader inputs
			UBO_FXAA( writer, 0u );
			auto c3d_mapDiffuse = writer.declSampler< Sampler2D >( cuT( "c3d_mapDiffuse" ), 1u, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ) );
			auto vtx_posPos = writer.declInput< Vec4 >( PosPos );

			// Shader outputs
			auto pxl_fragColor = writer.declFragData< Vec4 >( cuT( "pxl_fragColor" ), 0 );

#define FXAA_REDUCE_MIN	( 1.0 / 128.0 )

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
				{
					auto invTargetSize = writer.declLocale( cuT( "invTargetSize" )
						, vec2( 1.0 / c3d_renderSize.x(), 1.0 / c3d_renderSize.y() ) );

					auto rgbNW = writer.declLocale( cuT( "rgbNW" )
						, textureLodOffset( c3d_mapDiffuse, vtx_texture, 0.0_f, ivec2( -1_i, -1_i ) ).rgb() );
					auto rgbNE = writer.declLocale( cuT( "rgbNE" )
						, textureLodOffset( c3d_mapDiffuse, vtx_texture, 0.0_f, ivec2( 1_i, -1_i ) ).rgb() );
					auto rgbSW = writer.declLocale( cuT( "rgbSW" )
						, textureLodOffset( c3d_mapDiffuse, vtx_texture, 0.0_f, ivec2( -1_i, 1_i ) ).rgb() );
					auto rgbSE = writer.declLocale( cuT( "rgbSE" )
						, textureLodOffset( c3d_mapDiffuse, vtx_texture, 0.0_f, ivec2( 1_i, 1_i ) ).rgb() );
					auto rgbM = writer.declLocale( cuT( "rgbM" )
						, texture( c3d_mapDiffuse, vtx_texture, 0.0_f ).rgb() );

					auto luma = writer.declLocale( cuT( "luma" )
						, vec3( 0.299_f, 0.587_f, 0.114_f ) );
					auto lumaNW = writer.declLocale( cuT( "lumaNW" )
						, dot( rgbNW, luma ) );
					auto lumaNE = writer.declLocale( cuT( "lumaNE" )
						, dot( rgbNE, luma ) );
					auto lumaSW = writer.declLocale( cuT( "lumaSW" )
						, dot( rgbSW, luma ) );
					auto lumaSE = writer.declLocale( cuT( "lumaSE" )
						, dot( rgbSE, luma ) );
					auto lumaM = writer.declLocale( cuT( "lumaM" )
						, dot( rgbM, luma ) );

					auto lumaMin = writer.declLocale( cuT( "lumaMin" )
						, min( lumaM, min( min( lumaNW, lumaNE ), min( lumaSW, lumaSE ) ) ) );
					auto lumaMax = writer.declLocale( cuT( "lumaMax" )
						, max( lumaM, max( max( lumaNW, lumaNE ), max( lumaSW, lumaSE ) ) ) );

					auto dir = writer.declLocale( cuT( "dir" )
						, vec2( -writer.paren( writer.paren( lumaNW + lumaNE ) - writer.paren( lumaSW + lumaSE ) )
							, ( writer.paren( lumaNW + lumaSW ) - writer.paren( lumaNE + lumaSE ) ) ) );

					auto dirReduce = writer.declLocale( cuT( "dirReduce" )
						, max( writer.paren( lumaNW + lumaNE + lumaSW + lumaSE ) * writer.paren( 0.25_f * c3d_reduceMul ), FXAA_REDUCE_MIN ) );
					auto rcpDirMin = writer.declLocale( cuT( "rcpDirMin" )
						, 1.0 / ( min( glsl::abs( dir.x() ), glsl::abs( dir.y() ) ) + dirReduce ) );
					dir = min( vec2( c3d_spanMax, c3d_spanMax )
						, max( vec2( -c3d_spanMax, -c3d_spanMax )
							, dir * rcpDirMin ) ) * invTargetSize;

					auto rgbA = writer.declLocale( cuT( "rgbA" )
						, writer.paren( texture( c3d_mapDiffuse, vtx_texture + dir * writer.paren( 1.0_f / 3.0_f - 0.5_f ), 0.0_f ).rgb()
								+ texture( c3d_mapDiffuse, vtx_texture + dir * writer.paren( 2.0_f / 3.0_f - 0.5_f ), 0.0_f ).rgb() )
							* writer.paren( 1.0_f / 2.0_f ) );
					auto rgbB = writer.declLocale( cuT( "rgbB" )
						, writer.paren( rgbA * 1.0_f / 2.0_f )
							+ writer.paren( texture( c3d_mapDiffuse, vtx_texture + dir * writer.paren( 0.0_f / 3.0_f - 0.5_f ), 0.0_f ).rgb()
									+ texture( c3d_mapDiffuse, vtx_texture + dir * writer.paren( 3.0_f / 3.0_f - 0.5_f ), 0.0_f ).rgb() )
								* writer.paren( 1.0_f / 4.0_f ) );
					auto lumaB = writer.declLocale( cuT( "lumaB" )
						, dot( rgbB, luma ) );

					pxl_fragColor = vec4( writer.ternary( Type{ cuT( "lumaB < lumaMin || lumaB > lumaMax" ) }, rgbA, rgbB ), 1.0_f );
				} );

			return writer.finalise();
		}
		
		glsl::Shader getResultVertexProgram( castor3d::RenderSystem * renderSystem )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem->createGlslWriter();

			// Shader inputs
			UBO_FXAA( writer, 0u );
			auto position = writer.declAttribute< Vec2 >( cuT( "position" ) );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ) );
			auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
				{
					vtx_texture = position;
					gl_Position = vec4( position.xy(), 0.0, 1.0 );
				} );
			return writer.finalise();
		}

		glsl::Shader getResultFragmentProgram( castor3d::RenderSystem * renderSystem )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem->createGlslWriter();

			// Shader inputs
			UBO_FXAA( writer, 0u );
			auto c3d_mapDiffuse = writer.declSampler< Sampler2D >( cuT( "c3d_mapDiffuse" ), 1u, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto pxl_fragColor = writer.declFragData< Vec4 >( cuT( "pxl_fragColor" ), 0 );

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
				{
					pxl_fragColor = texture( c3d_mapDiffuse, vtx_texture );
				} );

			return writer.finalise();
		}
	}

	//*********************************************************************************************

	RenderQuad::RenderQuad( castor3d::RenderSystem & renderSystem
		, Size const & size )
		: castor3d::RenderQuad{ renderSystem, false, false }
		, m_size{ size }
	{
		m_configUbo = renderer::makeUniformBuffer< Configuration >( *renderSystem.getCurrentDevice()
			, 1u
			, renderer::BufferTarget::eTransferDst
			, renderer::MemoryPropertyFlag::eHostVisible );
		m_configUbo->getData( 0 ).m_renderSize = Point2f{ size };
	}

	void RenderQuad::update( float subpixShift
		, float spanMax
		, float reduceMul )
	{
		m_configUbo->getData( 0 ).m_subpixShift = subpixShift;
		m_configUbo->getData( 0 ).m_spanMax = spanMax;
		m_configUbo->getData( 0 ).m_reduceMul = reduceMul;

		if ( auto buffer = m_configUbo->getUbo().getBuffer().lock( 0u
			, sizeof( Configuration )
			, renderer::MemoryMapFlag::eWrite | renderer::MemoryMapFlag::eInvalidateRange ) )
		{
			std::memcpy( buffer, m_configUbo->getDatas().data(), sizeof( Configuration ) );
			m_configUbo->getUbo().getBuffer().unlock( sizeof( Configuration ), true );
		}
	}

	void RenderQuad::doFillDescriptorSet( renderer::DescriptorSetLayout & descriptorSetLayout
		, renderer::DescriptorSet & descriptorSet )
	{
		descriptorSet.createBinding( descriptorSetLayout.getBinding( 0u )
			, *m_configUbo
			, 0u
			, 1u );
	}

	//*********************************************************************************************

	String PostEffect::Type = cuT( "fxaa" );
	String PostEffect::Name = cuT( "FXAA PostEffect" );

	PostEffect::PostEffect( castor3d::RenderTarget & p_renderTarget
		, castor3d::RenderSystem & renderSystem
		, castor3d::Parameters const & p_parameters )
		: castor3d::PostEffect{ PostEffect::Type
			, p_renderTarget
			, renderSystem
			, p_parameters }
		, m_surface{ *renderSystem.getEngine() }
	{
		String param;

		if ( p_parameters.get( cuT( "SubpixShift" ), param ) )
		{
			m_subpixShift = string::toFloat( param );
		}

		if ( p_parameters.get( cuT( "MaxSpan" ), param ) )
		{
			m_spanMax = string::toFloat( param );
		}

		if ( p_parameters.get( cuT( "ReduceMul" ), param ) )
		{
			m_reduceMul = string::toFloat( param );
		}

		String name = cuT( "FXAA" );

		if ( !m_renderTarget.getEngine()->getSamplerCache().has( name ) )
		{
			m_sampler = m_renderTarget.getEngine()->getSamplerCache().add( name );
			m_sampler->setMinFilter( renderer::Filter::eNearest );
			m_sampler->setMagFilter( renderer::Filter::eNearest );
			m_sampler->setWrapS( renderer::WrapMode::eClampToBorder );
			m_sampler->setWrapT( renderer::WrapMode::eClampToBorder );
			m_sampler->setWrapR( renderer::WrapMode::eClampToBorder );
		}
		else
		{
			m_sampler = m_renderTarget.getEngine()->getSamplerCache().find( name );
		}
	}

	PostEffect::~PostEffect()
	{
	}

	castor3d::PostEffectSPtr PostEffect::create( castor3d::RenderTarget & p_renderTarget
		, castor3d::RenderSystem & renderSystem
		, castor3d::Parameters const & p_param )
	{
		return std::make_shared< PostEffect >( p_renderTarget, renderSystem, p_param );
	}

	bool PostEffect::initialise()
	{
		auto & cache = getRenderSystem()->getEngine()->getShaderProgramCache();
		Size size = m_renderTarget.getSize();

		auto & device = *getRenderSystem()->getCurrentDevice();

		// Prepare the render pass.
		std::vector< renderer::PixelFormat > formats{ { m_renderTarget.getPixelFormat(), m_renderTarget.getPixelFormat() } };
		renderer::RenderSubpassPtrArray subpasses;
		subpasses.emplace_back( device.createRenderSubpass( formats
			, renderer::RenderSubpassState{ renderer::PipelineStageFlag::eColourAttachmentOutput
			, renderer::AccessFlag::eColourAttachmentWrite } ) );
		subpasses.emplace_back( device.createRenderSubpass( formats
			, renderer::RenderSubpassState{ renderer::PipelineStageFlag::eColourAttachmentOutput
			, renderer::AccessFlag::eColourAttachmentWrite } ) );
		m_renderPass = device.createRenderPass( formats
			, std::move( subpasses )
			, renderer::RenderPassState{ renderer::PipelineStageFlag::eColourAttachmentOutput
				, renderer::AccessFlag::eColourAttachmentWrite
				, { renderer::ImageLayout::eColourAttachmentOptimal, renderer::ImageLayout::eColourAttachmentOptimal } }
			, renderer::RenderPassState{ renderer::PipelineStageFlag::eColourAttachmentOutput
				, renderer::AccessFlag::eColourAttachmentWrite
				, { renderer::ImageLayout::eColourAttachmentOptimal, renderer::ImageLayout::eColourAttachmentOptimal } }
			, false );

		// Create the FXAA quad renderer.
		renderer::DescriptorSetLayoutBindingArray bindings
		{
			{ 0u, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eFragment },
		};
		auto vtx = getFxaaVertexProgram( getRenderSystem() );
		auto pxl = getFxaaFragmentProgram( getRenderSystem() );
		auto & fxaaProgram = cache.getNewProgram( false );
		fxaaProgram.createModule( vtx.getSource(), renderer::ShaderStageFlag::eVertex );
		fxaaProgram.createModule( pxl.getSource(), renderer::ShaderStageFlag::eFragment );
		fxaaProgram.link();
		m_fxaaQuad = std::make_unique< RenderQuad >( *getRenderSystem(), size );
		m_fxaaQuad->createPipeline( size
			, Position{}
			, fxaaProgram
			, m_renderTarget.getTexture().getTexture()->getView()
			, *m_renderPass
			, bindings );

		// Initialise the surface.
		auto result = m_surface.initialise( m_renderTarget
			, *m_renderPass
			, size
			, 0u
			, m_sampler );

		if ( result )
		{
			// Create the result quad renderer.
			auto vtx = getResultVertexProgram( getRenderSystem() );
			auto pxl = getResultFragmentProgram( getRenderSystem() );
			auto & resultProgram = cache.getNewProgram( false );
			resultProgram.createModule( vtx.getSource(), renderer::ShaderStageFlag::eVertex );
			resultProgram.createModule( pxl.getSource(), renderer::ShaderStageFlag::eFragment );
			resultProgram.link();
			m_resultQuad = std::make_unique< RenderQuad >( *getRenderSystem(), size );
			m_resultQuad->createPipeline( size
				, Position{}
				, resultProgram
				, m_surface.m_colourTexture.getTexture()->getView()
				, *m_renderPass
				, bindings );

			// Initialise the command buffer.
			m_commandBuffer = device.getGraphicsCommandPool().createCommandBuffer();

			if ( m_commandBuffer->begin( renderer::CommandBufferUsageFlag::eSimultaneousUse ) )
			{
				auto & targetImage = m_renderTarget.getTexture().getTexture()->getTexture();
				auto & targetView = m_renderTarget.getTexture().getTexture()->getView();
				auto & surfaceImage = m_surface.m_colourTexture.getTexture()->getTexture();
				auto & surfaceView = m_surface.m_colourTexture.getTexture()->getView();

				// Put images in the right state for rendering.
				m_commandBuffer->memoryBarrier( renderer::PipelineStageFlag::eTopOfPipe
					, renderer::PipelineStageFlag::eFragmentShader
					, targetImage.makeShaderInputResource( targetView.getSubResourceRange() ) );
				m_commandBuffer->memoryBarrier( renderer::PipelineStageFlag::eTopOfPipe
					, renderer::PipelineStageFlag::eColourAttachmentOutput
					, surfaceImage.makeShaderInputResource( surfaceView.getSubResourceRange() ) );

				// Render the effect.
				m_commandBuffer->beginRenderPass( *m_renderPass
					, *m_surface.m_fbo
					, {}
					, renderer::SubpassContents::eInline );
				m_fxaaQuad->registerFrame( *m_commandBuffer );

				// Put images in the right state for blitting.
				m_commandBuffer->memoryBarrier( renderer::PipelineStageFlag::eColourAttachmentOutput
					, renderer::PipelineStageFlag::eFragmentShader
					, surfaceImage.makeShaderInputResource( surfaceView.getSubResourceRange() ) );
				m_commandBuffer->memoryBarrier( renderer::PipelineStageFlag::eFragmentShader
					, renderer::PipelineStageFlag::eColourAttachmentOutput
					, targetImage.makeColourAttachment( targetView.getSubResourceRange() ) );

				// Blit the result to the render target image.
				m_commandBuffer->nextSubpass( renderer::SubpassContents::eInline );
				m_resultQuad->registerFrame( *m_commandBuffer );

				m_commandBuffer->endRenderPass();
				m_commandBuffer->end();
			}
		}

		return result;
	}

	void PostEffect::cleanup()
	{
		m_resultQuad.reset();
		m_fxaaQuad.reset();
		m_commandBuffer.reset();
		m_renderPass.reset();
		m_surface.cleanup();
	}

	bool PostEffect::apply()
	{
		return getRenderSystem()->getCurrentDevice()->getGraphicsQueue().submit( *m_commandBuffer, nullptr );
	}

	bool PostEffect::doWriteInto( TextFile & p_file )
	{
		return true;
	}
}
