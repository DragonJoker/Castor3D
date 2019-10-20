#include "LinearMotionBlurPostEffect/LinearMotionBlurPostEffect.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/UniformBuffer.hpp>
#include <Castor3D/Buffer/GpuBuffer.hpp>
#include <Castor3D/Cache/SamplerCache.hpp>
#include <Castor3D/Cache/ShaderCache.hpp>
#include <Castor3D/Mesh/Vertex.hpp>
#include <Castor3D/Scene/ParticleSystem/ParticleDeclaration.hpp>
#include <Castor3D/Scene/ParticleSystem/ParticleElementDeclaration.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Render/RenderLoop.hpp>
#include <Castor3D/Render/RenderPass.hpp>
#include <Castor3D/Render/RenderPassTimer.hpp>
#include <Castor3D/Render/RenderPipeline.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Render/RenderWindow.hpp>
#include <Castor3D/Render/Viewport.hpp>
#include <Castor3D/Technique/RenderTechnique.hpp>
#include <Castor3D/Technique/RenderTechniquePass.hpp>
#include <Castor3D/Texture/Sampler.hpp>
#include <Castor3D/Texture/TextureLayout.hpp>
#include <Castor3D/Texture/TextureUnit.hpp>

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
		std::unique_ptr< sdw::Shader > getVertexProgram( castor3d::RenderSystem * renderSystem )
		{
			using namespace sdw;
			VertexWriter writer;

			// Shader inputs
			Vec2 position = writer.declInput< Vec2 >( cuT( "position" ), 0u );
			Vec2 uv = writer.declInput< Vec2 >( cuT( "uv" ), 1u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ), 0u );
			auto out = writer.getOut();

			writer.implementFunction< sdw::Void >( cuT( "main" ), [&]()
				{
					vtx_texture = uv;
					out.gl_out.gl_Position = vec4( position, 0.0, 1.0 );
				} );
			return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}

		std::unique_ptr< sdw::Shader > getFragmentProgram( castor3d::RenderSystem * renderSystem )
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			Ubo configuration{ writer, cuT( "Configuration" ), 0u, 0u };
			auto c3d_samplesCount = configuration.declMember< UInt >( cuT( "c3d_samplesCount" ) );
			auto c3d_vectorDivider = configuration.declMember< Float >( cuT( "c3d_vectorDivider" ) );
			auto c3d_blurScale = configuration.declMember< Float >( cuT( "c3d_blurScale" ) );
			configuration.end();
			auto c3d_mapVelocity = writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapVelocity" ), 1u, 0u );
			auto c3d_mapDiffuse = writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapDiffuse" ), 2u, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ), 0u );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( cuT( "pxl_fragColor" ), 0u );

			writer.implementFunction< sdw::Void >( cuT( "main" ), [&]()
				{
					auto blurVector = writer.declLocale( cuT( "vector" )
						, writer.paren( texture( c3d_mapVelocity, vtx_texture ).xy() / c3d_vectorDivider ) * c3d_blurScale );
					blurVector.y() = -blurVector.y();
					pxl_fragColor = texture( c3d_mapDiffuse, vtx_texture );

					FOR( writer, UInt, i, 0u, i < c3d_samplesCount, ++i )
					{
						auto offset = writer.declLocale( cuT( "offset" )
							, blurVector * writer.paren( writer.cast< Float >( i ) / writer.cast< Float >( c3d_samplesCount - 1u ) - 0.5f ) );
						pxl_fragColor += texture( c3d_mapDiffuse, vtx_texture + offset );
					}
					ROF;

					pxl_fragColor /= writer.cast< Float >( c3d_samplesCount );
					//IF( writer, vector == vec2( 0.0_f ) )
					//{
					//	pxl_fragColor = vec4( 1.0_f );
					//}
					//ELSE
					//{
					//	pxl_fragColor = vec4( vector, 0.0, 1.0 );
					//}
					//FI;
				} );
			return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}
	}

	//*********************************************************************************************

	PostEffect::Quad::Quad( castor3d::RenderSystem & renderSystem
		, castor3d::TextureUnit const & velocity
		, castor3d::UniformBuffer< Configuration > const & ubo )
		: castor3d::RenderQuad{ renderSystem, true }
		, m_velocityView{ velocity.getTexture()->getDefaultView() }
		, m_velocitySampler{ velocity.getSampler()->getSampler() }
		, m_ubo{ ubo }
	{
	}

	void PostEffect::Quad::doFillDescriptorSet( ashes::DescriptorSetLayout & descriptorSetLayout
		, ashes::DescriptorSet & descriptorSet )
	{
		descriptorSet.createSizedBinding( descriptorSetLayout.getBinding( 0u )
			, m_ubo
			, 0u );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( 1u )
			, m_velocityView
			, m_velocitySampler );
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
			, false }
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

	void PostEffect::update( castor::Nanoseconds const & elapsedTime )
	{
		if ( m_fpsScale )
		{
			auto current = Clock::now();
			auto duration = std::chrono::duration_cast< std::chrono::milliseconds >( current - m_saved );
			auto fps = 1000.0f / duration.count();
			auto & configuration = m_ubo->getData( 0u );
			configuration.samplesCount = m_configuration.samplesCount;
			configuration.vectorDivider = m_configuration.vectorDivider;
			configuration.blurScale = m_fpsScale
				? fps / getRenderSystem()->getEngine()->getRenderLoop().getWantedFps()
				: 1.0f;
			m_ubo->upload( 0u );
			m_saved = current;
		}
	}

	void PostEffect::accept( castor3d::PipelineVisitorBase & visitor )
	{
		visitor.visit( cuT( "MotionBlur" )
			, VK_SHADER_STAGE_VERTEX_BIT
			, *m_vertexShader.shader );
		visitor.visit( cuT( "MotionBlur" )
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, *m_pixelShader.shader );
	}

	bool PostEffect::doInitialise( castor3d::RenderPassTimer const & timer )
	{
		auto & renderSystem = *getRenderSystem();
		auto & device = getCurrentRenderDevice( *this );
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
				std::nullopt,
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
		setDebugObjectName( device, *m_renderPass, "LinearMotionBlur" );

		m_ubo = castor3d::makeUniformBuffer< Configuration >( renderSystem
			, 1u
			, 0u
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, "LinearMotionBlurCfg" );
		auto & configuration = m_ubo->getData( 0u );
		configuration.samplesCount = m_configuration.samplesCount;
		configuration.vectorDivider = m_configuration.vectorDivider;
		m_ubo->upload( 0u );

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
			, m_renderTarget.getVelocity()
			, *m_ubo );
		m_quad->createPipeline( size
			, castor::Position{}
			, stages
			, m_target->getDefaultView()
			, *m_renderPass
			, bindings
			, {} );

		auto result = m_surface.initialise( *m_renderPass
			, castor::Size{ m_target->getWidth(), m_target->getHeight() }
			, m_target->getPixelFormat() );
		castor3d::CommandsSemaphore commands
		{
			device.graphicsCommandPool->createCommandBuffer(),
			device->createSemaphore()
		};
		auto & cmd = *commands.commandBuffer;

		if ( result )
		{
			cmd.begin();
			timer.beginPass( cmd );

			// Put target image in shader input layout.
			cmd.memoryBarrier( VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
				, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
				, m_target->getDefaultView().makeShaderInputResource( VK_IMAGE_LAYOUT_UNDEFINED ) );

			cmd.beginRenderPass( *m_renderPass
				, *m_surface.frameBuffer
				, { castor3d::transparentBlackClearColor }
				, VK_SUBPASS_CONTENTS_INLINE );
			m_quad->registerFrame( cmd );
			cmd.endRenderPass();

			timer.endPass( cmd );
			cmd.end();
			m_commands.emplace_back( std::move( commands ) );
		}

		m_result = m_surface.colourTexture.get();
		m_saved = Clock::now();
		return result;
	}

	void PostEffect::doCleanup()
	{
		m_quad.reset();
		m_ubo.reset();
		m_renderPass.reset();
		m_surface.cleanup();
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
