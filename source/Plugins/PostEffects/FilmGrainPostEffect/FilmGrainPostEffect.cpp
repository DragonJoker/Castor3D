#include "FilmGrainPostEffect/FilmGrainPostEffect.hpp"

#include "NoiseLayer1.xpm"
#include "NoiseLayer2.xpm"
#include "NoiseLayer3.xpm"
#include "NoiseLayer4.xpm"
#include "NoiseLayer5.xpm"
#include "NoiseLayer6.xpm"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/UniformBuffer.hpp>
#include <Castor3D/Cache/SamplerCache.hpp>
#include <Castor3D/Cache/ShaderCache.hpp>
#include <Castor3D/Material/Texture/Sampler.hpp>
#include <Castor3D/Material/Texture/TextureLayout.hpp>
#include <Castor3D/Material/Texture/TextureUnit.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Render/RenderPassTimer.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>

#include <CastorUtils/Graphics/PixelBufferBase.hpp>

#include <ashespp/Image/StagingTexture.hpp>
#include <ashespp/Image/Image.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>

#include <numeric>

using namespace castor;

namespace film_grain
{
	namespace
	{
		static std::string const FilmGrainUbo = "FilmGrainUbo";
		static std::string const PixelSize = "c3d_pixelSize";
		static std::string const NoiseIntensity = "c3d_noiseIntensity";
		static std::string const Exposure = "c3d_exposure";
		static std::string const Time = "c3d_time";
		static std::string const SrcTex = "c3d_srcTex";
		static std::string const NoiseTex = "c3d_noiseTex";
		static uint32_t constexpr NoiseMapCount = 6u;

		std::unique_ptr< sdw::Shader > getVertexProgram( castor3d::RenderSystem * renderSystem )
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
					out.gl_out.gl_Position = vec4( position.xy(), 0.0_f, 1.0_f );
				} );
			return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}

		std::unique_ptr< sdw::Shader > getFragmentProgram( castor3d::RenderSystem * renderSystem )
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			sdw::Ubo filmGrain{ writer, FilmGrainUbo, 0u, 0u };
			auto c3d_pixelSize = filmGrain.declMember< Vec2 >( PixelSize );
			auto c3d_noiseIntensity = filmGrain.declMember< Float >( NoiseIntensity );
			auto c3d_exposure = filmGrain.declMember< Float >( Exposure );
			auto c3d_time = filmGrain.declMember< Float >( Time );
			filmGrain.end();

			auto c3d_noiseTex = writer.declSampledImage< FImg3DR32 >( NoiseTex, 1u, 0u );
			auto c3d_srcTex = writer.declSampledImage< FImg2DRgba32 >( SrcTex, 2u, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( "pxl_fragColor", 0 );
			
			auto overlay = writer.implementFunction< Vec3 >( "overlay"
				, [&]( Vec3 const & a
					, Vec3 const & b )
				{
					auto comp = writer.declLocale( "comp"
						, pow( abs( b ), vec3( 2.2_f ) ) );

					IF( writer, comp.x() < 0.5_f && comp.y() < 0.5_f && comp.z() < 0.5_f )
					{
						writer.returnStmt( 2.0_f * a * b );
					}
					FI;

					writer.returnStmt( vec3( 1.0_f ) - 2.0_f * ( 1.0_f - a ) * ( 1.0_f - b ) );
				}
				, InVec3{ writer, "a" }
				, InVec3{ writer, "b" } );

			auto addNoise = writer.implementFunction< Vec3 >( "addNoise"
				, [&]( Vec3 const & color
					, Vec2 const & texcoord )
				{
					auto coord = writer.declLocale( "coord"
						, texcoord * 2.0_f );
					coord.x() *= c3d_pixelSize.y() / c3d_pixelSize.x();
					auto noise = writer.declLocale( "noise"
						, texture( c3d_noiseTex, vec3( coord, c3d_time ) ) );
					auto exposureFactor = writer.declLocale( "exposureFactor"
						, c3d_exposure / 2.0_f );
					exposureFactor = sqrt( exposureFactor );
					auto t = writer.declLocale( "t"
						, mix( 3.5_f * c3d_noiseIntensity
							, 1.13_f * c3d_noiseIntensity
							, exposureFactor ) );
					writer.returnStmt( overlay( color
						, vec3( mix( 0.5_f, noise, t ) ) ) );
				}
				, InVec3{ writer, "color" }
				, InVec2{ writer, "texcoord" } );

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					auto colour = writer.declLocale( "colour"
						, texture( c3d_srcTex, vtx_texture ).xyz() );
					colour = addNoise( colour, vtx_texture );
					pxl_fragColor = vec4( colour, 1.0 );
				} );
			return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}
	}

	//*********************************************************************************************

	RenderQuad::RenderQuad( castor3d::RenderSystem & renderSystem
		, VkExtent2D const & size )
		: castor3d::RenderQuad{ renderSystem, false, false }
		, m_size{ size }
	{
		auto & device = getCurrentRenderDevice( renderSystem );
		auto & engine = *renderSystem.getEngine();
		auto name = cuT( "FilmGrain_Noise" );
		castor3d::SamplerSPtr sampler;

		if ( !engine.getSamplerCache().has( name ) )
		{
			m_sampler = engine.getSamplerCache().add( name );
			m_sampler->setMinFilter( VK_FILTER_LINEAR );
			m_sampler->setMagFilter( VK_FILTER_LINEAR );
			m_sampler->setMipFilter( VK_SAMPLER_MIPMAP_MODE_LINEAR );
			m_sampler->setWrapS( VK_SAMPLER_ADDRESS_MODE_REPEAT );
			m_sampler->setWrapT( VK_SAMPLER_ADDRESS_MODE_REPEAT );
			m_sampler->setWrapR( VK_SAMPLER_ADDRESS_MODE_REPEAT );
			m_sampler->initialise();
		}
		else
		{
			m_sampler = engine.getSamplerCache().find( name );
		}
		
		ashes::ImageCreateInfo image
		{
			0u,
			VK_IMAGE_TYPE_3D,
			VK_FORMAT_R8G8B8A8_UNORM,
			{ 512u, 512u, NoiseMapCount },
			1u,
			1u,
			VK_SAMPLE_COUNT_1_BIT,
			VK_IMAGE_TILING_OPTIMAL,
			( VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_TRANSFER_DST_BIT ),
		};
		m_noise = castor3d::makeImage( device
			, std::move( image )
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, "FilmGrainNoise" );

		ashes::ImageViewCreateInfo imageView
		{
			0u,
			*m_noise,
			VK_IMAGE_VIEW_TYPE_3D,
			m_noise->getFormat(),
			VkComponentMapping{},
			{ ashes::getAspectMask( m_noise->getFormat() ), 0u, 1u, 0u, 1u }
		};
		m_noiseView = m_noise->createView( imageView );

		auto & loader = engine.getImageLoader();
		//XpmLoader loader;
		std::array< PxBufferBaseSPtr, NoiseMapCount > buffers
		{
			loader.load( "xpm", reinterpret_cast< uint8_t const * >( NoiseLayer1_xpm ), uint32_t( castor::getCountOf( NoiseLayer1_xpm ) ) ).front(),
			loader.load( "xpm", reinterpret_cast< uint8_t const * >( NoiseLayer2_xpm ), uint32_t( castor::getCountOf( NoiseLayer2_xpm ) ) ).front(),
			loader.load( "xpm", reinterpret_cast< uint8_t const * >( NoiseLayer3_xpm ), uint32_t( castor::getCountOf( NoiseLayer3_xpm ) ) ).front(),
			loader.load( "xpm", reinterpret_cast< uint8_t const * >( NoiseLayer4_xpm ), uint32_t( castor::getCountOf( NoiseLayer4_xpm ) ) ).front(),
			loader.load( "xpm", reinterpret_cast< uint8_t const * >( NoiseLayer5_xpm ), uint32_t( castor::getCountOf( NoiseLayer5_xpm ) ) ).front(),
			loader.load( "xpm", reinterpret_cast< uint8_t const * >( NoiseLayer6_xpm ), uint32_t( castor::getCountOf( NoiseLayer6_xpm ) ) ).front(),
		};

		uint32_t maxSize = buffers[0]->getSize();
		auto dim = buffers[0]->getDimensions();
		auto format = castor3d::convert( buffers[0]->getFormat() );
		auto staging = device->createStagingTexture( format
			, VkExtent2D{ dim.getWidth(), dim.getHeight() } );
		ashes::CommandBufferPtr cmdCopy = device.graphicsCommandPool->createCommandBuffer();

		for ( uint32_t i = 0u; i < NoiseMapCount; ++i )
		{
			staging->uploadTextureData( *cmdCopy
				, {
					m_noiseView->subresourceRange.aspectMask,
					m_noiseView->subresourceRange.baseMipLevel,
					m_noiseView->subresourceRange.baseArrayLayer,
					m_noiseView->subresourceRange.layerCount,
				}
				, format
				, { 0, 0, int32_t( i ) }
				, { image->extent.width, image->extent.height }
				, buffers[i]->getConstPtr()
				, m_noiseView );
		}

		m_configUbo = castor3d::makeUniformBuffer< Configuration >( device.renderSystem
			, 1u
			, VK_BUFFER_USAGE_TRANSFER_DST_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, "FilmGrainCfg" );
		m_configUbo->getData( 0 ).m_pixelSize = Point2f{ m_size.width, m_size.height };
		m_configUbo->getData( 0 ).m_noiseIntensity = 1.0f;
		m_configUbo->getData( 0 ).m_exposure = 1.0f;
	}

	void RenderQuad::update( castor::Nanoseconds const & time )
	{
		m_time += time.count();
		m_configUbo->getData().m_time = ( m_time % NoiseMapCount ) / float( NoiseMapCount );
		m_configUbo->upload();
	}

	void RenderQuad::doFillDescriptorSet( ashes::DescriptorSetLayout & descriptorSetLayout
		, ashes::DescriptorSet & descriptorSet )
	{
		descriptorSet.createBinding( descriptorSetLayout.getBinding( 0u )
			, *m_configUbo
			, 0u
			, 1u );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( 1u )
			, m_noiseView
			, m_sampler->getSampler() );
	}

	//*********************************************************************************************

	String PostEffect::Type = cuT( "film_grain" );
	String PostEffect::Name = cuT( "FilmGrain PostEffect" );

	PostEffect::PostEffect( castor3d::RenderTarget & renderTarget
		, castor3d::RenderSystem & renderSystem
		, castor3d::Parameters const & params )
		: castor3d::PostEffect{ PostEffect::Type
			, PostEffect::Name
			, renderTarget
			, renderSystem
			, params
			, false }
		, m_surface{ *renderSystem.getEngine(), cuT( "FilmGrain" ) }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, "FilmGrain" }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, "FilmGrain" }
	{
		String name = cuT( "FilmGrain2D" );

		if ( !m_renderTarget.getEngine()->getSamplerCache().has( name ) )
		{
			m_sampler = m_renderTarget.getEngine()->getSamplerCache().add( name );
			m_sampler->setMinFilter( VK_FILTER_LINEAR );
			m_sampler->setMagFilter( VK_FILTER_LINEAR );
			m_sampler->setWrapS( VK_SAMPLER_ADDRESS_MODE_REPEAT );
			m_sampler->setWrapT( VK_SAMPLER_ADDRESS_MODE_REPEAT );
			m_sampler->setWrapR( VK_SAMPLER_ADDRESS_MODE_REPEAT );
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
		return std::make_shared< PostEffect >( renderTarget
			, renderSystem
			, params );
	}

	void PostEffect::accept( castor3d::PipelineVisitorBase & visitor )
	{
		visitor.visit( cuT( "FilmGrain" )
			, VK_SHADER_STAGE_VERTEX_BIT
			, *m_vertexShader.shader );
		visitor.visit( cuT( "FilmGrain" )
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, *m_pixelShader.shader );
		visitor.visit( cuT( "FilmGrain" )
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, cuT( "FilmGrain" )
			, cuT( "Exposure" )
			, m_quad->getUbo().getData().m_exposure );
		visitor.visit( cuT( "FilmGrain" )
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, cuT( "FilmGrain" )
			, cuT( "NoiseIntensity" )
			, m_quad->getUbo().getData().m_noiseIntensity );
	}

	void PostEffect::update( castor::Nanoseconds const & elapsedTime )
	{
		m_quad->update( elapsedTime );
	}

	bool PostEffect::doInitialise( castor3d::RenderPassTimer const & timer )
	{
		auto & renderSystem = *getRenderSystem();
		auto & device = getCurrentRenderDevice( *this );
		VkExtent2D size{ m_target->getWidth(), m_target->getHeight() };
		m_sampler->initialise();
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
		m_renderPass = device->createRenderPass( std::move( createInfo ) );
		setDebugObjectName( device, *m_renderPass, "FilmGrain" );

		ashes::VkDescriptorSetLayoutBindingArray bindings
		{
			castor3d::makeDescriptorSetLayoutBinding( 0u
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, VK_SHADER_STAGE_FRAGMENT_BIT ),
			castor3d::makeDescriptorSetLayoutBinding( 1u
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ),
		};

		m_quad = std::make_unique< RenderQuad >( renderSystem, size );
		m_quad->createPipeline( size
			, Position{}
			, stages
			, m_target->getDefaultView()
			, *m_renderPass
			, std::move( bindings )
			, {} );

		auto result = m_surface.initialise( *m_renderPass
			, castor::Size{ m_target->getWidth(), m_target->getHeight() }
			, m_target->getPixelFormat() );

		if ( result )
		{
			castor3d::CommandsSemaphore commands
			{
				device.graphicsCommandPool->createCommandBuffer(),
				device->createSemaphore()
			};
			auto & cmd = *commands.commandBuffer;
			cmd.begin();
			timer.beginPass( cmd );
			// Put image in the right state for rendering.
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
		return result;
	}

	void PostEffect::doCleanup()
	{
		m_quad.reset();
		m_renderPass.reset();
		m_surface.cleanup();
	}

	bool PostEffect::doWriteInto( TextFile & file, String const & tabs )
	{
		return file.writeText( tabs + cuT( "postfx \"" ) + Type + cuT( "\"" ) ) > 0;
	}
}
