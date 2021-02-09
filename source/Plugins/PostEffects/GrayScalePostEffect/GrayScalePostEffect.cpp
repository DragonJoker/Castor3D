#include "GrayScalePostEffect/GrayScalePostEffect.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/UniformBufferPools.hpp>
#include <Castor3D/Buffer/GpuBuffer.hpp>
#include <Castor3D/Cache/SamplerCache.hpp>
#include <Castor3D/Cache/ShaderCache.hpp>
#include <Castor3D/Material/Texture/Sampler.hpp>
#include <Castor3D/Material/Texture/TextureLayout.hpp>
#include <Castor3D/Material/Texture/TextureUnit.hpp>
#include <Castor3D/Model/Vertex.hpp>
#include <Castor3D/Scene/ParticleSystem/ParticleDeclaration.hpp>
#include <Castor3D/Scene/ParticleSystem/ParticleElementDeclaration.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Render/RenderPassTimer.hpp>
#include <Castor3D/Render/RenderPipeline.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Render/RenderWindow.hpp>
#include <Castor3D/Render/Viewport.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>

#include <ashespp/Pipeline/GraphicsPipeline.hpp>
#include <ashespp/Pipeline/GraphicsPipelineCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineLayout.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>

#include <numeric>

namespace grayscale
{
	namespace
	{
		std::unique_ptr< ast::Shader > getVertexProgram( castor3d::RenderSystem const & renderSystem )
		{
			using namespace sdw;
			VertexWriter writer;

			// Shader inputs
			Vec2 position = writer.declInput< Vec2 >( "position", 0u );
			Vec2 uv = writer.declInput< Vec2 >( "uv", 1u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( "vtx_texture", 0u );
			auto out = writer.getOut();

			castor3d::shader::Utils utils{ writer };
			utils.declareInvertVec2Y();

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
			GrayCfgUboIdx,
			ColorTexIdx,
		};

		std::unique_ptr< ast::Shader > getFragmentProgram()
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			auto configUbo = Ubo{ writer, "Configuration", GrayCfgUboIdx, 0u };
			auto c3d_factors = configUbo.declMember< Vec3 >( "c3d_factors" );
			configUbo.end();
			auto c3d_mapColor = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapColor", ColorTexIdx, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( "pxl_fragColor", 0u );

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					auto colour = writer.declLocale( "colour"
						, c3d_mapColor.sample( vtx_texture ).xyz() );
					pxl_fragColor = vec4( vec3( dot( c3d_factors, colour ) ), 1.0_f );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		castor3d::rq::BindingDescriptionArray createBindings()
		{
			return
			{
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, std::nullopt },
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_IMAGE_VIEW_TYPE_2D },
			};
		}
	}

	//*********************************************************************************************

	PostEffect::Quad::Quad( castor3d::RenderSystem & renderSystem
		, castor3d::RenderDevice const & device )
		: castor3d::RenderQuad{ device
			, cuT( "GrayScale" )
			, VK_FILTER_NEAREST
			, { createBindings()
				, ashes::nullopt
				, castor3d::rq::Texcoord{} } }
	{
	}

	//*********************************************************************************************

	castor::String PostEffect::Type = cuT( "grayscale" );
	castor::String PostEffect::Name = cuT( "GrayScale PostEffect" );

	PostEffect::PostEffect( castor3d::RenderTarget & renderTarget
		, castor3d::RenderSystem & renderSystem
		, castor3d::Parameters const & params )
		: castor3d::PostEffect{ PostEffect::Type
			, PostEffect::Name
			, renderTarget
			, renderSystem
			, params }
		, m_surface{ *renderSystem.getEngine(), cuT( "GrayScale" ) }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, "GrayScale" }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, "GrayScale" }
	{
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
		if ( m_factors.isDirty() )
		{
			m_configUbo.getData() = m_factors.value();
			m_factors.reset();
		}
	}

	void PostEffect::accept( castor3d::PipelineVisitorBase & visitor )
	{
		visitor.visit( m_vertexShader );
		visitor.visit( m_pixelShader );
		visitor.visit( cuT( "GrayScale" )
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, cuT( "Configuration" )
			, cuT( "Factors" )
			, m_factors );
	}

	bool PostEffect::doInitialise( castor3d::RenderDevice const & device
		, castor3d::RenderPassTimer const & timer )
	{
		auto & renderSystem = *getRenderSystem();
		VkExtent2D size{ m_target->getWidth(), m_target->getHeight() };
		m_vertexShader.shader = getVertexProgram( renderSystem );
		m_pixelShader.shader = getFragmentProgram();
		ashes::PipelineShaderStageCreateInfoArray stages;
		stages.push_back( makeShaderState( device, m_vertexShader ) );
		stages.push_back( makeShaderState( device, m_pixelShader ) );

		m_configUbo = device.uboPools->getBuffer< castor::Point3f >( 0u );
		m_configUbo.getData() = m_factors.value();
		m_factors.reset();

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
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
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
		m_renderPass = device->createRenderPass( "GrayScale"
			, std::move( createInfo ) );

		ashes::VkDescriptorSetLayoutBindingArray bindings
		{
			castor3d::makeDescriptorSetLayoutBinding( 0u
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, VK_SHADER_STAGE_FRAGMENT_BIT )
		};
		m_quad = std::make_unique< Quad >( renderSystem
			, device );
		m_quad->createPipelineAndPass( size
			, castor::Position{}
			, stages
			, *m_renderPass
			, {
				m_quad->makeDescriptorWrite( m_configUbo
					, GrayCfgUboIdx ),
				m_quad->makeDescriptorWrite( m_target->getDefaultView().getSampledView()
					, m_quad->getSampler().getSampler()
					, ColorTexIdx ),
			} );

		auto result = m_surface.initialise( device
			, *m_renderPass
			, castor::Size{ m_target->getWidth(), m_target->getHeight() }
			, m_target->getPixelFormat() );

		if ( result )
		{
			castor3d::CommandsSemaphore commands
			{
				device.graphicsCommandPool->createCommandBuffer( "GrayScale" ),
				device->createSemaphore( "GrayScale" )
			};
			auto & cmd = *commands.commandBuffer;
			cmd.begin();
			timer.beginPass( cmd );
			cmd.beginDebugBlock( { "GrayScale"
				, castor3d::makeFloatArray( getOwner()->getEngine()->getNextRainbowColour() ) } );
			cmd.beginRenderPass( *m_renderPass
				, *m_surface.frameBuffer
				, { castor3d::transparentBlackClearColor }
				, VK_SUBPASS_CONTENTS_INLINE );
			m_quad->registerPass( cmd );
			cmd.endRenderPass();
			cmd.endDebugBlock();
			timer.endPass( cmd );
			cmd.end();
			m_commands.emplace_back( std::move( commands ) );
		}

		m_result = m_surface.colourTexture.get();
		return result;
	}

	void PostEffect::doCleanup( castor3d::RenderDevice const & device )
	{
		m_quad.reset();
		m_renderPass.reset();
		m_surface.cleanup( device );
		device.uboPools->putBuffer( m_configUbo );
	}

	bool PostEffect::doWriteInto( castor::TextFile & file, castor::String const & tabs )
	{
		return file.writeText( tabs + cuT( "postfx \"" ) + Type + cuT( "\"" ) ) > 0;
	}

	//*********************************************************************************************
}
