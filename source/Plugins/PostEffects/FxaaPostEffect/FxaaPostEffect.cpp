#include "FxaaPostEffect/FxaaPostEffect.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/SamplerCache.hpp>
#include <Castor3D/Cache/ShaderCache.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Render/RenderPassTimer.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>
#include <Castor3D/Texture/Sampler.hpp>
#include <Castor3D/Texture/TextureLayout.hpp>
#include <Castor3D/Texture/TextureUnit.hpp>

#include <ashespp/Image/Image.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

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
			auto uv = writer.declInput< Vec2 >( "uv", 1u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( "vtx_texture", 0u );
			auto vtx_posPos = writer.declOutput< Vec4 >( PosPos, 1u );
			auto out = writer.getOut();

			castor3d::shader::Utils utils{ writer };
			utils.declareInvertVec2Y();

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					vtx_texture = uv;
					out.gl_out.gl_Position = vec4( position.xy(), 0.0_f, 1.0_f );
					vtx_posPos.xy() = position.xy();
					vtx_posPos.zw() = position.xy() - ( c3d_pixelSize * ( 0.5_f + c3d_subpixShift ) );
				} );
			return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}

		std::unique_ptr< sdw::Shader > getFxaaFragmentProgram( castor3d::RenderSystem * renderSystem )
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			UBO_FXAA( writer, 0u, 0u );
			auto c3d_mapColor = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapColor", 1u, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );
			auto vtx_posPos = writer.declInput< Vec4 >( PosPos, 1u );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( "pxl_fragColor", 0 );

#define FXAA_REDUCE_MIN	Float{ 1.0 / 128.0 }

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					auto rgbNW = writer.declLocale( "rgbNW"
						, textureLodOffset( c3d_mapColor, vtx_texture, 0.0_f, ivec2( -1_i, -1_i ) ).rgb() );
					auto rgbNE = writer.declLocale( "rgbNE"
						, textureLodOffset( c3d_mapColor, vtx_texture, 0.0_f, ivec2( 1_i, -1_i ) ).rgb() );
					auto rgbSW = writer.declLocale( "rgbSW"
						, textureLodOffset( c3d_mapColor, vtx_texture, 0.0_f, ivec2( -1_i, 1_i ) ).rgb() );
					auto rgbSE = writer.declLocale( "rgbSE"
						, textureLodOffset( c3d_mapColor, vtx_texture, 0.0_f, ivec2( 1_i, 1_i ) ).rgb() );
					auto rgbM = writer.declLocale( "rgbM"
						, texture( c3d_mapColor, vtx_texture, 0.0_f ).rgb() );

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
						, writer.paren( texture( c3d_mapColor, texcoord0, 0.0_f ).rgb()
								+ texture( c3d_mapColor, texcoord1, 0.0_f ).rgb() )
							* writer.paren( 1.0_f / 2.0_f ) );

					texcoord0 = vtx_texture + dir * writer.paren( 0.0_f / 3.0_f - 0.5_f );
					texcoord1 = vtx_texture + dir * writer.paren( 3.0_f / 3.0_f - 0.5_f );

					auto rgbB = writer.declLocale( "rgbB"
						, writer.paren( rgbA * 1.0_f / 2.0_f )
							+ writer.paren( texture( c3d_mapColor, texcoord0, 0.0_f ).rgb()
									+ texture( c3d_mapColor, texcoord1, 0.0_f ).rgb() )
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
		descriptorSet.createSizedBinding( descriptorSetLayout.getBinding( 0u )
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
		, m_surface{ *renderSystem.getEngine(), cuT( "Fxaa" ) }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, "Fxaa" }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, "Fxaa" }
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
			m_sampler->setMinFilter( VK_FILTER_NEAREST );
			m_sampler->setMagFilter( VK_FILTER_NEAREST );
			m_sampler->setWrapS( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER );
			m_sampler->setWrapT( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER );
			m_sampler->setWrapR( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER );
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
			, VK_SHADER_STAGE_VERTEX_BIT
			, *m_vertexShader.shader );
		visitor.visit( cuT( "FXAA" )
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, *m_pixelShader.shader );
		visitor.visit( cuT( "FXAA" )
			, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
			, cuT( "FXAA" )
			, cuT( "SubPixShift" )
			, m_subpixShift );
		visitor.visit( cuT( "FXAA" )
			, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
			, cuT( "FXAA" )
			, cuT( "SpanMax" )
			, m_spanMax );
		visitor.visit( cuT( "FXAA" )
			, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
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
		auto & device = getCurrentRenderDevice( *this );
		VkExtent2D size{ m_target->getWidth(), m_target->getHeight() };

		// Create the render pass.
		ashes::VkAttachmentDescriptionArray attachments
		{
			{
				0u,
				m_target->getPixelFormat(),
				VK_SAMPLE_COUNT_1_BIT,
				VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				VK_ATTACHMENT_STORE_OP_STORE,
				VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				VK_ATTACHMENT_STORE_OP_DONT_CARE,
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			}
		};
		ashes::SubpassDescriptionArray subpasses;
		subpasses.emplace_back( ashes::SubpassDescription
			{
				0u,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				{},
				{ { 0u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } },
				{},
				ashes::nullopt,
				{},
			} );
		ashes::VkSubpassDependencyArray dependencies
		{
			{
				VK_SUBPASS_EXTERNAL,
				0u,
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				VK_ACCESS_SHADER_READ_BIT,
				VK_DEPENDENCY_BY_REGION_BIT,
			},
			{
				0u,
				VK_SUBPASS_EXTERNAL,
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				VK_ACCESS_SHADER_READ_BIT,
				VK_DEPENDENCY_BY_REGION_BIT,
			},
		};
		ashes::RenderPassCreateInfo createInfo
		{
			0u,
			std::move( attachments ),
			std::move( subpasses ),
			std::move( dependencies ),
		};
		m_renderPass = device->createRenderPass( std::move( createInfo ) );
		setDebugObjectName( device, *m_renderPass, "Fxaa" );

		// Create the FXAA quad renderer.
		ashes::VkDescriptorSetLayoutBindingArray bindings
		{
			castor3d::makeDescriptorSetLayoutBinding( 0u
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT ),
		};
		m_vertexShader.shader = getFxaaVertexProgram( getRenderSystem() );
		m_pixelShader.shader = getFxaaFragmentProgram( getRenderSystem() );

		ashes::PipelineShaderStageCreateInfoArray stages;
		stages.push_back( makeShaderState( device, m_vertexShader ) );
		stages.push_back( makeShaderState( device, m_pixelShader ) );

		m_fxaaQuad = std::make_unique< RenderQuad >( renderSystem
			, castor::Size{ size.width, size.height } );
		m_fxaaQuad->createPipeline( size
			, Position{}
			, stages
			, m_target->getDefaultView()
			, *m_renderPass
			, std::move( bindings )
			, {} );

		// Initialise the surface.
		auto result = m_surface.initialise( *m_renderPass
			, castor::Size{ size.width, size.height }
			, m_target->getPixelFormat() );
		castor3d::CommandsSemaphore commands
		{
			device.graphicsCommandPool->createCommandBuffer(),
			device->createSemaphore()
		};
		auto & cmd = *commands.commandBuffer;

		if ( result )
		{
			// Initialise the command buffer.
			auto & targetView = m_target->getDefaultView();

			cmd.begin();
			timer.beginPass( cmd );

			// Put target image in shader input layout.
			cmd.memoryBarrier( VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
				, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
				, targetView.makeShaderInputResource( VK_IMAGE_LAYOUT_UNDEFINED ) );

			// Render the effect.
			cmd.beginRenderPass( *m_renderPass
				, *m_surface.frameBuffer
				, { castor3d::transparentBlackClearColor }
				, VK_SUBPASS_CONTENTS_INLINE );
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
