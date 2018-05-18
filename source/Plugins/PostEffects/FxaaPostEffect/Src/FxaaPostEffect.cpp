#include "FxaaPostEffect.hpp"

#include <Engine.hpp>
#include <Cache/SamplerCache.hpp>
#include <Cache/ShaderCache.hpp>

#include <Miscellaneous/Parameter.hpp>
#include <Render/RenderPassTimer.hpp>
#include <Render/RenderSystem.hpp>
#include <Render/RenderTarget.hpp>
#include <Texture/Sampler.hpp>
#include <Texture/TextureLayout.hpp>
#include <Texture/TextureUnit.hpp>

#include <Image/Texture.hpp>
#include <Image/TextureView.hpp>
#include <RenderPass/RenderPass.hpp>
#include <RenderPass/RenderPassCreateInfo.hpp>
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
		static String const PosPos = cuT( "vtx_posPos" );

		glsl::Shader getFxaaVertexProgram( castor3d::RenderSystem * renderSystem )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem->createGlslWriter();

			// Shader inputs
			UBO_FXAA( writer, 0u, 0u );
			auto position = writer.declAttribute< Vec2 >( cuT( "position" ), 0u );
			auto texcoord = writer.declAttribute< Vec2 >( cuT( "texcoord" ), 1u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ), 0u );
			auto vtx_posPos = writer.declOutput< Vec4 >( PosPos, 1u );
			auto out = gl_PerVertex{ writer };

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
				{
					vtx_texture = texcoord;
					out.gl_Position() = vec4( position.xy(), 0.0, 1.0 );
					vtx_posPos.xy() = position.xy();
					vtx_posPos.zw() = position.xy() - writer.paren( c3d_pixelSize * writer.paren( 0.5 + c3d_subpixShift ) );
				} );
			return writer.finalise();
		}

		glsl::Shader getFxaaFragmentProgram( castor3d::RenderSystem * renderSystem )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem->createGlslWriter();

			// Shader inputs
			UBO_FXAA( writer, 0u, 0u );
			auto c3d_mapDiffuse = writer.declSampler< Sampler2D >( cuT( "c3d_mapDiffuse" ), 1u, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ), 0u );
			auto vtx_posPos = writer.declInput< Vec4 >( PosPos, 1u );

			// Shader outputs
			auto pxl_fragColor = writer.declFragData< Vec4 >( cuT( "pxl_fragColor" ), 0 );

#define FXAA_REDUCE_MIN	( 1.0 / 128.0 )

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
				{
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
							, dir * rcpDirMin ) ) * c3d_pixelSize;

					auto texcoord0 = writer.declLocale( cuT( "texcoord0" )
						, vtx_texture + dir * writer.paren( 1.0_f / 3.0_f - 0.5_f ) );
					auto texcoord1 = writer.declLocale( cuT( "texcoord1" )
						, vtx_texture + dir * writer.paren( 2.0_f / 3.0_f - 0.5_f ) );

					auto rgbA = writer.declLocale( cuT( "rgbA" )
						, writer.paren( texture( c3d_mapDiffuse, texcoord0, 0.0_f ).rgb()
								+ texture( c3d_mapDiffuse, texcoord1, 0.0_f ).rgb() )
							* writer.paren( 1.0_f / 2.0_f ) );

					texcoord0 = vtx_texture + dir * writer.paren( 0.0_f / 3.0_f - 0.5_f );
					texcoord1 = vtx_texture + dir * writer.paren( 3.0_f / 3.0_f - 0.5_f );

					auto rgbB = writer.declLocale( cuT( "rgbB" )
						, writer.paren( rgbA * 1.0_f / 2.0_f )
							+ writer.paren( texture( c3d_mapDiffuse, texcoord0, 0.0_f ).rgb()
									+ texture( c3d_mapDiffuse, texcoord1, 0.0_f ).rgb() )
								* writer.paren( 1.0_f / 4.0_f ) );
					auto lumaB = writer.declLocale( cuT( "lumaB" )
						, dot( rgbB, luma ) );

					pxl_fragColor = vec4( writer.ternary( Type{ cuT( "lumaB < lumaMin || lumaB > lumaMax" ) }, rgbA, rgbB ), 1.0_f );
				} );

			return writer.finalise();
		}
	}

	//*********************************************************************************************

	RenderQuad::RenderQuad( castor3d::RenderSystem & renderSystem
		, Size const & size )
		: castor3d::RenderQuad{ renderSystem, false, false }
		, m_fxaaUbo{ *renderSystem.getEngine(), size }
	{
	}

	void RenderQuad::update( float subpixShift
		, float spanMax
		, float reduceMul )
	{
		m_fxaaUbo.update( subpixShift
			, spanMax
			, reduceMul );
	}

	void RenderQuad::doFillDescriptorSet( renderer::DescriptorSetLayout & descriptorSetLayout
		, renderer::DescriptorSet & descriptorSet )
	{
		descriptorSet.createBinding( descriptorSetLayout.getBinding( 0u )
			, m_fxaaUbo.getUbo() );
	}

	//*********************************************************************************************

	String PostEffect::Type = cuT( "fxaa" );
	String PostEffect::Name = cuT( "FXAA PostEffect" );

	PostEffect::PostEffect( castor3d::RenderTarget & renderTarget
		, castor3d::RenderSystem & renderSystem
		, castor3d::Parameters const & parameters )
		: castor3d::PostEffect{ PostEffect::Type
			, PostEffect::Name
			, renderTarget
			, renderSystem
			, parameters }
		, m_surface{ *renderSystem.getEngine() }
	{
		String param;

		if ( parameters.get( cuT( "SubpixShift" ), param ) )
		{
			m_subpixShift = string::toFloat( param );
		}

		if ( parameters.get( cuT( "MaxSpan" ), param ) )
		{
			m_spanMax = string::toFloat( param );
		}

		if ( parameters.get( cuT( "ReduceMul" ), param ) )
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

	castor3d::PostEffectSPtr PostEffect::create( castor3d::RenderTarget & renderTarget
		, castor3d::RenderSystem & renderSystem
		, castor3d::Parameters const & params )
	{
		return std::make_shared< PostEffect >( renderTarget, renderSystem, params );
	}

	void PostEffect::accept( castor3d::PipelineVisitorBase & visitor )
	{
		visitor.visit( cuT( "FXAA" )
			, renderer::ShaderStageFlag::eVertex
			, m_vertexShader );
		visitor.visit( cuT( "FXAA" )
			, renderer::ShaderStageFlag::eFragment
			, m_pixelShader );
		visitor.visit( cuT( "FXAA" )
			, renderer::ShaderStageFlag::eVertex | renderer::ShaderStageFlag::eFragment
			, cuT( "FXAA" )
			, cuT( "SubPixShift" )
			, m_subpixShift );
		visitor.visit( cuT( "FXAA" )
			, renderer::ShaderStageFlag::eVertex | renderer::ShaderStageFlag::eFragment
			, cuT( "FXAA" )
			, cuT( "SpanMax" )
			, m_spanMax );
		visitor.visit( cuT( "FXAA" )
			, renderer::ShaderStageFlag::eVertex | renderer::ShaderStageFlag::eFragment
			, cuT( "FXAA" )
			, cuT( "ReduceMul" )
			, m_reduceMul );
	}

	bool PostEffect::doInitialise( castor3d::RenderPassTimer const & timer )
	{
		m_sampler->initialise();

		auto & device = *getRenderSystem()->getCurrentDevice();
		renderer::Extent2D size{ m_target->getWidth(), m_target->getHeight() };

		// Create the render pass.
		renderer::RenderPassCreateInfo renderPass;
		renderPass.flags = 0u;

		renderPass.attachments.resize( 1u );
		renderPass.attachments[0].format = m_target->getPixelFormat();
		renderPass.attachments[0].loadOp = renderer::AttachmentLoadOp::eDontCare;
		renderPass.attachments[0].storeOp = renderer::AttachmentStoreOp::eStore;
		renderPass.attachments[0].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
		renderPass.attachments[0].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
		renderPass.attachments[0].samples = renderer::SampleCountFlag::e1;
		renderPass.attachments[0].initialLayout = renderer::ImageLayout::eUndefined;
		renderPass.attachments[0].finalLayout = renderer::ImageLayout::eColourAttachmentOptimal;

		renderPass.subpasses.resize( 1u );
		renderPass.subpasses[0].pipelineBindPoint = renderer::PipelineBindPoint::eGraphics;
		renderPass.subpasses[0].colorAttachments.push_back( { 0u, renderer::ImageLayout::eColourAttachmentOptimal } );

		renderPass.dependencies.resize( 2u );
		renderPass.dependencies[0].srcSubpass = renderer::ExternalSubpass;
		renderPass.dependencies[0].dstSubpass = 0u;
		renderPass.dependencies[0].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[0].dstAccessMask = renderer::AccessFlag::eShaderRead;
		renderPass.dependencies[0].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[0].dstStageMask = renderer::PipelineStageFlag::eFragmentShader;
		renderPass.dependencies[0].dependencyFlags = renderer::DependencyFlag::eByRegion;

		renderPass.dependencies[1].srcSubpass = 0u;
		renderPass.dependencies[1].dstSubpass = renderer::ExternalSubpass;
		renderPass.dependencies[1].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[1].dstAccessMask = renderer::AccessFlag::eShaderRead;
		renderPass.dependencies[1].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[1].dstStageMask = renderer::PipelineStageFlag::eFragmentShader;
		renderPass.dependencies[1].dependencyFlags = renderer::DependencyFlag::eByRegion;

		m_renderPass = device.createRenderPass( renderPass );

		// Create the FXAA quad renderer.
		renderer::DescriptorSetLayoutBindingArray bindings
		{
			{ 0u, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eVertex | renderer::ShaderStageFlag::eFragment },
		};
		m_vertexShader = getFxaaVertexProgram( getRenderSystem() );
		m_pixelShader = getFxaaFragmentProgram( getRenderSystem() );

		renderer::ShaderStageStateArray stages;
		stages.push_back( { device.createShaderModule( renderer::ShaderStageFlag::eVertex ) } );
		stages.push_back( { device.createShaderModule( renderer::ShaderStageFlag::eFragment ) } );
		stages[0].module->loadShader( m_vertexShader.getSource() );
		stages[1].module->loadShader( m_pixelShader.getSource() );

		m_fxaaQuad = std::make_unique< RenderQuad >( *getRenderSystem()
			, castor::Size{ size.width, size.height } );
		m_fxaaQuad->createPipeline( size
			, Position{}
			, stages
			, m_target->getDefaultView()
			, *m_renderPass
			, bindings
			, {} );

		// Initialise the surface.
		auto result = m_surface.initialise( *m_renderPass
			, castor::Size{ size.width, size.height }
			, m_sampler
			, m_target->getPixelFormat() );
		castor3d::CommandsSemaphore commands
		{
			device.getGraphicsCommandPool().createCommandBuffer(),
			device.createSemaphore()
		};
		auto & cmd = *commands.commandBuffer;

		if ( result )
		{
			// Initialise the command buffer.
			if ( cmd.begin() )
			{
				auto & targetImage = m_target->getTexture();
				auto & targetView = m_target->getDefaultView();
				auto & surfaceImage = m_surface.colourTexture->getTexture();
				auto & surfaceView = m_surface.colourTexture->getDefaultView();

				cmd.resetQueryPool( timer.getQuery()
					, 0u
					, 2u );
				cmd.writeTimestamp( renderer::PipelineStageFlag::eTopOfPipe
					, timer.getQuery()
					, 0u );

				// Put target image in shader input layout.
				cmd.memoryBarrier( renderer::PipelineStageFlag::eColourAttachmentOutput
					, renderer::PipelineStageFlag::eFragmentShader
					, targetView.makeShaderInputResource( renderer::ImageLayout::eUndefined, 0u ) );

				// Render the effect.
				cmd.beginRenderPass( *m_renderPass
					, *m_surface.frameBuffer
					, { renderer::ClearColorValue{} }
					, renderer::SubpassContents::eInline );
				m_fxaaQuad->registerFrame( cmd );
				cmd.endRenderPass();

				cmd.writeTimestamp( renderer::PipelineStageFlag::eTopOfPipe
					, timer.getQuery()
					, 1u );
				cmd.end();
				m_commands.emplace_back( std::move( commands ) );
			}
		}

		m_fxaaQuad->update( m_subpixShift
			, m_spanMax
			, m_reduceMul );
		m_result = m_surface.colourTexture.get();
		return result;
	}

	void PostEffect::doCleanup()
	{
		m_fxaaQuad.reset();
		m_surface.cleanup();
		m_renderPass.reset();
	}

	bool PostEffect::doWriteInto( TextFile & p_file )
	{
		return true;
	}
}
