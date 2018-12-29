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
#include <Sync/ImageMemoryBarrier.hpp>

#include <ShaderWriter/Source.hpp>

#include <numeric>

using namespace castor;

namespace fxaa
{
	namespace
	{
		static String const PosPos = cuT( "vtx_posPos" );

		std::unique_ptr< sdw::Shader > getFxaaVertexProgram( castor3d::RenderSystem * renderSystem )
		{
			using namespace sdw;
			VertexWriter writer;

			// Shader inputs
			UBO_FXAA( writer, 0u, 0u );
			auto position = writer.declInput< Vec2 >( "position", 0u );
			auto texcoord = writer.declInput< Vec2 >( "texcoord", 1u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( "vtx_texture", 0u );
			auto vtx_posPos = writer.declOutput< Vec4 >( PosPos, 1u );
			auto out = writer.getOut();

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					vtx_texture = texcoord;
					out.gl_out.gl_Position = vec4( position.xy(), 0.0, 1.0 );
					vtx_posPos.xy() = position.xy();
					vtx_posPos.zw() = position.xy() - ( c3d_pixelSize * ( 0.5 + c3d_subpixShift ) );
				} );
			return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}

		std::unique_ptr< sdw::Shader > getFxaaFragmentProgram( castor3d::RenderSystem * renderSystem )
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			UBO_FXAA( writer, 0u, 0u );
			auto c3d_mapDiffuse = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapDiffuse", 1u, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );
			auto vtx_posPos = writer.declInput< Vec4 >( PosPos, 1u );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( "pxl_fragColor", 0 );

#define FXAA_REDUCE_MIN	Float{ 1.0 / 128.0 }

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					auto rgbNW = writer.declLocale( "rgbNW"
						, textureLodOffset( c3d_mapDiffuse, vtx_texture, 0.0_f, ivec2( -1_i, -1_i ) ).rgb() );
					auto rgbNE = writer.declLocale( "rgbNE"
						, textureLodOffset( c3d_mapDiffuse, vtx_texture, 0.0_f, ivec2( 1_i, -1_i ) ).rgb() );
					auto rgbSW = writer.declLocale( "rgbSW"
						, textureLodOffset( c3d_mapDiffuse, vtx_texture, 0.0_f, ivec2( -1_i, 1_i ) ).rgb() );
					auto rgbSE = writer.declLocale( "rgbSE"
						, textureLodOffset( c3d_mapDiffuse, vtx_texture, 0.0_f, ivec2( 1_i, 1_i ) ).rgb() );
					auto rgbM = writer.declLocale( "rgbM"
						, texture( c3d_mapDiffuse, vtx_texture, 0.0_f ).rgb() );

					auto luma = writer.declLocale( "luma"
						, vec3( 0.299_f, 0.587_f, 0.114_f ) );
					auto lumaNW = writer.declLocale( "lumaNW"
						, dot( rgbNW, luma ) );
					auto lumaNE = writer.declLocale( "lumaNE"
						, dot( rgbNE, luma ) );
					auto lumaSW = writer.declLocale( "lumaSW"
						, dot( rgbSW, luma ) );
					auto lumaSE = writer.declLocale( "lumaSE"
						, dot( rgbSE, luma ) );
					auto lumaM = writer.declLocale( "lumaM"
						, dot( rgbM, luma ) );

					auto lumaMin = writer.declLocale( "lumaMin"
						, min( lumaM, min( min( lumaNW, lumaNE ), min( lumaSW, lumaSE ) ) ) );
					auto lumaMax = writer.declLocale( "lumaMax"
						, max( lumaM, max( max( lumaNW, lumaNE ), max( lumaSW, lumaSE ) ) ) );

					auto dir = writer.declLocale( "dir"
						, vec2( -writer.paren( writer.paren( lumaNW + lumaNE ) - writer.paren( lumaSW + lumaSE ) )
							, ( writer.paren( lumaNW + lumaSW ) - writer.paren( lumaNE + lumaSE ) ) ) );

					auto dirReduce = writer.declLocale( "dirReduce"
						, max( writer.paren( lumaNW + lumaNE + lumaSW + lumaSE ) * writer.paren( 0.25_f * c3d_reduceMul ), FXAA_REDUCE_MIN ) );
					auto rcpDirMin = writer.declLocale( "rcpDirMin"
						, 1.0_f / ( min( abs( dir.x() ), abs( dir.y() ) ) + dirReduce ) );
					dir = min( vec2( c3d_spanMax, c3d_spanMax )
						, max( vec2( -c3d_spanMax, -c3d_spanMax )
							, dir * rcpDirMin ) ) * c3d_pixelSize;

					auto texcoord0 = writer.declLocale( "texcoord0"
						, vtx_texture + dir * writer.paren( 1.0_f / 3.0_f - 0.5_f ) );
					auto texcoord1 = writer.declLocale( "texcoord1"
						, vtx_texture + dir * writer.paren( 2.0_f / 3.0_f - 0.5_f ) );

					auto rgbA = writer.declLocale( "rgbA"
						, writer.paren( texture( c3d_mapDiffuse, texcoord0, 0.0_f ).rgb()
								+ texture( c3d_mapDiffuse, texcoord1, 0.0_f ).rgb() )
							* writer.paren( 1.0_f / 2.0_f ) );

					texcoord0 = vtx_texture + dir * writer.paren( 0.0_f / 3.0_f - 0.5_f );
					texcoord1 = vtx_texture + dir * writer.paren( 3.0_f / 3.0_f - 0.5_f );

					auto rgbB = writer.declLocale( "rgbB"
						, writer.paren( rgbA * 1.0_f / 2.0_f )
							+ writer.paren( texture( c3d_mapDiffuse, texcoord0, 0.0_f ).rgb()
									+ texture( c3d_mapDiffuse, texcoord1, 0.0_f ).rgb() )
								* writer.paren( 1.0_f / 4.0_f ) );
					auto lumaB = writer.declLocale( "lumaB"
						, dot( rgbB, luma ) );

					pxl_fragColor = vec4( writer.ternary( lumaB < lumaMin || lumaB > lumaMax, rgbA, rgbB )
						, 1.0_f );
				} );

			return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
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

	void RenderQuad::doFillDescriptorSet( ashes::DescriptorSetLayout & descriptorSetLayout
		, ashes::DescriptorSet & descriptorSet )
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
		, m_vertexShader{ ashes::ShaderStageFlag::eVertex, "Fxaa" }
		, m_pixelShader{ ashes::ShaderStageFlag::eFragment, "Fxaa" }
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
			m_sampler->setMinFilter( ashes::Filter::eNearest );
			m_sampler->setMagFilter( ashes::Filter::eNearest );
			m_sampler->setWrapS( ashes::WrapMode::eClampToBorder );
			m_sampler->setWrapT( ashes::WrapMode::eClampToBorder );
			m_sampler->setWrapR( ashes::WrapMode::eClampToBorder );
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
			, ashes::ShaderStageFlag::eVertex
			, *m_vertexShader.shader );
		visitor.visit( cuT( "FXAA" )
			, ashes::ShaderStageFlag::eFragment
			, *m_pixelShader.shader );
		visitor.visit( cuT( "FXAA" )
			, ashes::ShaderStageFlag::eVertex | ashes::ShaderStageFlag::eFragment
			, cuT( "FXAA" )
			, cuT( "SubPixShift" )
			, m_subpixShift );
		visitor.visit( cuT( "FXAA" )
			, ashes::ShaderStageFlag::eVertex | ashes::ShaderStageFlag::eFragment
			, cuT( "FXAA" )
			, cuT( "SpanMax" )
			, m_spanMax );
		visitor.visit( cuT( "FXAA" )
			, ashes::ShaderStageFlag::eVertex | ashes::ShaderStageFlag::eFragment
			, cuT( "FXAA" )
			, cuT( "ReduceMul" )
			, m_reduceMul );
	}

	void PostEffect::update( castor::Nanoseconds const & elapsedTime )
	{
		if ( m_subpixShift.isDirty()
			|| m_spanMax.isDirty()
			|| m_reduceMul.isDirty() )
		{
			m_fxaaQuad->update( m_subpixShift
				, m_spanMax
				, m_reduceMul );
			m_subpixShift.reset();
			m_spanMax.reset();
			m_reduceMul.reset();
		}
	}

	bool PostEffect::doInitialise( castor3d::RenderPassTimer const & timer )
	{
		m_sampler->initialise();

		auto & renderSystem = *getRenderSystem();
		auto & device = getCurrentDevice( *this );
		ashes::Extent2D size{ m_target->getWidth(), m_target->getHeight() };

		// Create the render pass.
		ashes::RenderPassCreateInfo renderPass;
		renderPass.flags = 0u;

		renderPass.attachments.resize( 1u );
		renderPass.attachments[0].format = m_target->getPixelFormat();
		renderPass.attachments[0].loadOp = ashes::AttachmentLoadOp::eDontCare;
		renderPass.attachments[0].storeOp = ashes::AttachmentStoreOp::eStore;
		renderPass.attachments[0].stencilLoadOp = ashes::AttachmentLoadOp::eDontCare;
		renderPass.attachments[0].stencilStoreOp = ashes::AttachmentStoreOp::eDontCare;
		renderPass.attachments[0].samples = ashes::SampleCountFlag::e1;
		renderPass.attachments[0].initialLayout = ashes::ImageLayout::eUndefined;
		renderPass.attachments[0].finalLayout = ashes::ImageLayout::eColourAttachmentOptimal;

		renderPass.subpasses.resize( 1u );
		renderPass.subpasses[0].pipelineBindPoint = ashes::PipelineBindPoint::eGraphics;
		renderPass.subpasses[0].colorAttachments.push_back( { 0u, ashes::ImageLayout::eColourAttachmentOptimal } );

		renderPass.dependencies.resize( 2u );
		renderPass.dependencies[0].srcSubpass = ashes::ExternalSubpass;
		renderPass.dependencies[0].dstSubpass = 0u;
		renderPass.dependencies[0].srcAccessMask = ashes::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[0].dstAccessMask = ashes::AccessFlag::eShaderRead;
		renderPass.dependencies[0].srcStageMask = ashes::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[0].dstStageMask = ashes::PipelineStageFlag::eFragmentShader;
		renderPass.dependencies[0].dependencyFlags = ashes::DependencyFlag::eByRegion;

		renderPass.dependencies[1].srcSubpass = 0u;
		renderPass.dependencies[1].dstSubpass = ashes::ExternalSubpass;
		renderPass.dependencies[1].srcAccessMask = ashes::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[1].dstAccessMask = ashes::AccessFlag::eShaderRead;
		renderPass.dependencies[1].srcStageMask = ashes::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[1].dstStageMask = ashes::PipelineStageFlag::eFragmentShader;
		renderPass.dependencies[1].dependencyFlags = ashes::DependencyFlag::eByRegion;

		m_renderPass = device.createRenderPass( renderPass );

		// Create the FXAA quad renderer.
		ashes::DescriptorSetLayoutBindingArray bindings
		{
			{ 0u, ashes::DescriptorType::eUniformBuffer, ashes::ShaderStageFlag::eVertex | ashes::ShaderStageFlag::eFragment },
		};
		m_vertexShader.shader = getFxaaVertexProgram( getRenderSystem() );
		m_pixelShader.shader = getFxaaFragmentProgram( getRenderSystem() );

		ashes::ShaderStageStateArray stages;
		stages.push_back( { device.createShaderModule( ashes::ShaderStageFlag::eVertex ) } );
		stages.push_back( { device.createShaderModule( ashes::ShaderStageFlag::eFragment ) } );
		stages[0].module->loadShader( renderSystem.compileShader( m_vertexShader ) );
		stages[1].module->loadShader( renderSystem.compileShader( m_pixelShader ) );

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
			auto & targetImage = m_target->getTexture();
			auto & targetView = m_target->getDefaultView();
			auto & surfaceImage = m_surface.colourTexture->getTexture();
			auto & surfaceView = m_surface.colourTexture->getDefaultView();

			cmd.begin();
			timer.beginPass( cmd );

			// Put target image in shader input layout.
			cmd.memoryBarrier( ashes::PipelineStageFlag::eColourAttachmentOutput
				, ashes::PipelineStageFlag::eFragmentShader
				, targetView.makeShaderInputResource( ashes::ImageLayout::eUndefined, 0u ) );

			// Render the effect.
			cmd.beginRenderPass( *m_renderPass
				, *m_surface.frameBuffer
				, { ashes::ClearColorValue{} }
				, ashes::SubpassContents::eInline );
			m_fxaaQuad->registerFrame( cmd );
			cmd.endRenderPass();

			timer.endPass( cmd );
			cmd.end();
			m_commands.emplace_back( std::move( commands ) );
		}

		m_fxaaQuad->update( m_subpixShift
			, m_spanMax
			, m_reduceMul );
		m_subpixShift.reset();
		m_spanMax.reset();
		m_reduceMul.reset();
		m_result = m_surface.colourTexture.get();
		return result;
	}

	void PostEffect::doCleanup()
	{
		m_fxaaQuad.reset();
		m_surface.cleanup();
		m_renderPass.reset();
	}

	bool PostEffect::doWriteInto( TextFile & file, String const & tabs )
	{
		return file.writeText( tabs + cuT( "postfx \"" ) + Type + cuT( "\"" ) ) > 0;
	}
}
