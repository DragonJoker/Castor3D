#include "LinearMotionBlurPostEffect/LinearMotionBlurPostEffect.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/UniformBufferPools.hpp>
#include <Castor3D/Buffer/GpuBuffer.hpp>
#include <Castor3D/Cache/SamplerCache.hpp>
#include <Castor3D/Cache/ShaderCache.hpp>
#include <Castor3D/Material/Texture/Sampler.hpp>
#include <Castor3D/Material/Texture/TextureLayout.hpp>
#include <Castor3D/Material/Texture/TextureUnit.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Model/Vertex.hpp>
#include <Castor3D/Render/RenderLoop.hpp>
#include <Castor3D/Render/RenderPass.hpp>
#include <Castor3D/Render/RenderPassTimer.hpp>
#include <Castor3D/Render/RenderPipeline.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Render/RenderWindow.hpp>
#include <Castor3D/Render/Viewport.hpp>
#include <Castor3D/Render/Technique/RenderTechnique.hpp>
#include <Castor3D/Render/Technique/RenderTechniquePass.hpp>
#include <Castor3D/Scene/ParticleSystem/ParticleDeclaration.hpp>
#include <Castor3D/Scene/ParticleSystem/ParticleElementDeclaration.hpp>
#include <Castor3D/Shader/Program.hpp>

#include <ashespp/Pipeline/GraphicsPipeline.hpp>
#include <ashespp/Pipeline/GraphicsPipelineCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineLayout.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>

#include <numeric>

namespace motion_blur
{
	namespace
	{
		std::unique_ptr< ast::Shader > getVertexProgram( castor3d::RenderSystem * renderSystem )
		{
			using namespace sdw;
			VertexWriter writer;

			// Shader inputs
			Vec2 position = writer.declInput< Vec2 >( "position", 0u );
			Vec2 uv = writer.declInput< Vec2 >( "uv", 1u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( "vtx_texture", 0u );
			auto out = writer.getOut();

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					vtx_texture = uv;
					out.vtx.position = vec4( position, 0.0_f, 1.0_f );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		enum Idx : uint32_t
		{
			BlurCfgUboIdx,
			VelocityTexIdx,
			ColorTexIdx,
		};

		std::unique_ptr< ast::Shader > getFragmentProgram( castor3d::RenderSystem * renderSystem )
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			Ubo configuration{ writer, "Configuration", BlurCfgUboIdx, 0u };
			auto c3d_samplesCount = configuration.declMember< UInt >( "c3d_samplesCount" );
			auto c3d_vectorDivider = configuration.declMember< Float >( "c3d_vectorDivider" );
			auto c3d_blurScale = configuration.declMember< Float >( "c3d_blurScale" );
			configuration.end();
			auto c3d_mapVelocity = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapVelocity", VelocityTexIdx, 0u );
			auto c3d_mapColor = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapColor", ColorTexIdx, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( "pxl_fragColor", 0u );

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					auto blurVector = writer.declLocale( "vector"
						, ( texture( c3d_mapVelocity, vtx_texture ).xy() / c3d_vectorDivider ) * c3d_blurScale );
					blurVector.y() = -blurVector.y();
					pxl_fragColor = texture( c3d_mapColor, vtx_texture );

					FOR( writer, UInt, i, 0u, i < c3d_samplesCount, ++i )
					{
						auto offset = writer.declLocale( "offset"
							, blurVector * ( writer.cast< Float >( i ) / writer.cast< Float >( c3d_samplesCount - 1_u ) - 0.5f ) );
						pxl_fragColor += texture( c3d_mapColor, vtx_texture + offset );
					}
					ROF;

					pxl_fragColor /= writer.cast< Float >( c3d_samplesCount );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		castor3d::rq::BindingDescriptionArray createBindings()
		{
			return
			{
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, std::nullopt },
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_IMAGE_VIEW_TYPE_2D },
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_IMAGE_VIEW_TYPE_2D },
			};
		}
	}

	//*********************************************************************************************

	PostEffect::Quad::Quad( castor3d::RenderSystem & renderSystem
		, castor3d::RenderDevice const & device )
		: castor3d::RenderQuad{ device
			, cuT( "LinearMotionBlur" )
			, VK_FILTER_NEAREST
			, { createBindings()
				, ashes::nullopt
				, castor3d::rq::Texcoord{} } }
	{
	}

	//*********************************************************************************************

	castor::String PostEffect::Type = cuT( "linear_motion_blur" );
	castor::String PostEffect::Name = cuT( "LinearMotionBlur PostEffect" );

	PostEffect::PostEffect( castor3d::RenderTarget & renderTarget
		, castor3d::RenderSystem & renderSystem
		, castor3d::Parameters const & parameters )
		: castor3d::PostEffect{ PostEffect::Type
			, PostEffect::Name
			, renderTarget
			, renderSystem
			, parameters
			, 1u }
		, m_surface{ *renderSystem.getEngine(), cuT( "LinearMotionBlur" ) }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, "LinearMotionBlur" }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, "LinearMotionBlur" }
	{
		parameters.get( cuT( "vectorDivider" ), m_configuration.vectorDivider );
		parameters.get( cuT( "samplesCount" ), m_configuration.samplesCount );
		parameters.get( cuT( "fpsScale" ), m_fpsScale );
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

	void PostEffect::update( castor3d::CpuUpdater & updater )
	{
		if ( m_fpsScale )
		{
			auto current = Clock::now();
			auto duration = std::chrono::duration_cast< std::chrono::milliseconds >( current - m_saved );
			auto fps = 1000.0f / duration.count();
			auto & configuration = m_ubo.getData();
			configuration.samplesCount = m_configuration.samplesCount;
			configuration.vectorDivider = m_configuration.vectorDivider;
			configuration.blurScale = m_fpsScale
				? fps / getRenderSystem()->getEngine()->getRenderLoop().getWantedFps()
				: 1.0f;
			m_saved = current;
		}
	}

	void PostEffect::accept( castor3d::PipelineVisitorBase & visitor )
	{
		visitor.visit( m_vertexShader );
		visitor.visit( m_pixelShader );
	}

	bool PostEffect::doInitialise( castor3d::RenderDevice const & device
		, castor3d::RenderPassTimer const & timer )
	{
		auto & renderSystem = *getRenderSystem();
		VkExtent2D size{ m_target->getWidth(), m_target->getHeight() };
		m_vertexShader.shader = getVertexProgram( getRenderSystem() );
		m_pixelShader.shader = getFragmentProgram( getRenderSystem() );
		ashes::PipelineShaderStageCreateInfoArray stages;
		stages.push_back( makeShaderState( device, m_vertexShader ) );
		stages.push_back( makeShaderState( device, m_pixelShader ) );

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
		m_renderPass = device->createRenderPass( "LinearMotionBlur"
			, std::move( createInfo ) );

		m_ubo = device.uboPools->getBuffer< Configuration >( 0u );
		auto & configuration = m_ubo.getData();
		configuration.samplesCount = m_configuration.samplesCount;
		configuration.vectorDivider = m_configuration.vectorDivider;

		ashes::VkDescriptorSetLayoutBindingArray bindings
		{
			castor3d::makeDescriptorSetLayoutBinding( 0u
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, VK_SHADER_STAGE_FRAGMENT_BIT ),
			castor3d::makeDescriptorSetLayoutBinding( 1u
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ),
		};
		m_quad = std::make_unique< Quad >( renderSystem
			, device );
		m_quad->createPipelineAndPass( size
			, castor::Position{}
			, stages
			, *m_renderPass
			, {
				m_quad->makeDescriptorWrite( m_ubo
					, BlurCfgUboIdx ),
				m_quad->makeDescriptorWrite( m_renderTarget.getVelocity().getTexture()->getDefaultView().getSampledView()
					, m_renderTarget.getVelocity().getSampler()->getSampler()
					, VelocityTexIdx ),
				m_quad->makeDescriptorWrite( m_target->getDefaultView().getSampledView()
					, m_quad->getSampler().getSampler()
					, VelocityTexIdx ),
			} );

		auto result = m_surface.initialise( device
			, *m_renderPass
			, castor::Size{ m_target->getWidth(), m_target->getHeight() }
			, m_target->getPixelFormat() );

		if ( result )
		{
			castor3d::CommandsSemaphore commands
			{
				device.graphicsCommandPool->createCommandBuffer( "LinearMotionBlur" ),
				device->createSemaphore( "LinearMotionBlur" )
			};
			auto & cmd = *commands.commandBuffer;
			cmd.begin();
			timer.beginPass( cmd );

			// Put target image in shader input layout.
			cmd.memoryBarrier( VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
				, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
				, m_target->getDefaultView().getSampledView().makeShaderInputResource( VK_IMAGE_LAYOUT_UNDEFINED ) );

			cmd.beginRenderPass( *m_renderPass
				, *m_surface.frameBuffer
				, { castor3d::transparentBlackClearColor }
				, VK_SUBPASS_CONTENTS_INLINE );
			m_quad->registerPass( cmd );
			cmd.endRenderPass();

			timer.endPass( cmd );
			cmd.end();
			m_commands.emplace_back( std::move( commands ) );
		}

		m_result = m_surface.colourTexture.get();
		m_saved = Clock::now();
		return result;
	}

	void PostEffect::doCleanup( castor3d::RenderDevice const & device )
	{
		m_quad.reset();
		device.uboPools->putBuffer( m_ubo );
		m_renderPass.reset();
		m_surface.cleanup( device );
	}

	bool PostEffect::doWriteInto( castor::TextFile & file, castor::String const & tabs )
	{
		static Configuration const ref;
		auto result = file.writeText( cuT( "\n" ) + tabs + Type + cuT( "\n" ) ) > 0
			&& file.writeText( tabs + cuT( "{\n" ) ) > 0;

		if ( result && m_configuration.vectorDivider != ref.vectorDivider )
		{
			result = file.writeText( tabs + cuT( "\tvectorDivider " ) + castor::string::toString( m_configuration.vectorDivider, std::locale{ "C" } ) + cuT( "\n" ) ) > 0;
		}

		if ( result && m_configuration.samplesCount != ref.samplesCount )
		{
			result = file.writeText( tabs + cuT( "\tsamples " ) + castor::string::toString( m_configuration.samplesCount, std::locale{ "C" } ) + cuT( "\n" ) ) > 0;
		}

		if ( result && !m_fpsScale )
		{
			result = file.writeText( tabs + cuT( "\tfpsScale false\n" ) ) > 0;
		}

		if ( result )
		{
			result = file.writeText( tabs + cuT( "}\n" ) ) > 0;
		}

		return result;
	}

	//*********************************************************************************************
}
