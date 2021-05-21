#include "FilmGrainPostEffect/FilmGrainPostEffect.hpp"

#include "NoiseLayer1.xpm"
#include "NoiseLayer2.xpm"
#include "NoiseLayer3.xpm"
#include "NoiseLayer4.xpm"
#include "NoiseLayer5.xpm"
#include "NoiseLayer6.xpm"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/UniformBufferPools.hpp>
#include <Castor3D/Cache/SamplerCache.hpp>
#include <Castor3D/Cache/ShaderCache.hpp>
#include <Castor3D/Material/Texture/Sampler.hpp>
#include <Castor3D/Material/Texture/TextureLayout.hpp>
#include <Castor3D/Material/Texture/TextureUnit.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Render/RenderLoop.hpp>
#include <Castor3D/Render/RenderPassTimer.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>

#include <CastorUtils/Graphics/PixelBufferBase.hpp>
#include <CastorUtils/Graphics/RgbaColour.hpp>

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

			castor3d::shader::Utils utils{ writer };
			utils.declareInvertVec2Y();

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					vtx_texture = uv;
					out.vtx.position = vec4( position.xy(), 0.0_f, 1.0_f );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		enum Idx : uint32_t
		{
			FilmCfgUboIdx,
			NoiseTexIdx,
			SourceTexIdx,
		};

		std::unique_ptr< ast::Shader > getFragmentProgram( castor3d::RenderSystem * renderSystem )
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			sdw::Ubo filmGrain{ writer, FilmGrainUbo, FilmCfgUboIdx, 0u };
			auto c3d_pixelSize = filmGrain.declMember< Vec2 >( PixelSize );
			auto c3d_noiseIntensity = filmGrain.declMember< Float >( NoiseIntensity );
			auto c3d_exposure = filmGrain.declMember< Float >( Exposure );
			auto c3d_time = filmGrain.declMember< Float >( Time );
			filmGrain.end();

			auto c3d_noiseTex = writer.declSampledImage< FImg3DR32 >( NoiseTex, NoiseTexIdx, 0u );
			auto c3d_srcTex = writer.declSampledImage< FImg2DRgba32 >( SrcTex, SourceTexIdx, 0u );
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
						, c3d_noiseTex.sample( vec3( coord, c3d_time ) ) );
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
						, c3d_srcTex.sample( vtx_texture ).xyz() );
					colour = addNoise( colour, vtx_texture );
					pxl_fragColor = vec4( colour, 1.0 );
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

	RenderQuad::RenderQuad( castor3d::RenderSystem & renderSystem
		, castor3d::RenderDevice const & device
		, VkExtent2D const & size )
		: castor3d::RenderQuad{ device
			, cuT( "FilmGrain" )
			, VK_FILTER_LINEAR
			, { createBindings()
				, ashes::nullopt
				, castor3d::rq::Texcoord{} } }
		, m_size{ size }
	{
		auto & engine = *renderSystem.getEngine();
		auto name = getName() + cuT( "Noise" );

		m_configUbo = m_device.uboPools->getBuffer< Configuration >( 0u );
		m_configUbo.getData().m_pixelSize = Point2f{ m_size.width, m_size.height };
		m_configUbo.getData().m_noiseIntensity = 1.0f;
		m_configUbo.getData().m_exposure = 1.0f;

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
			m_sampler->initialise( device );
		}
		else
		{
			m_sampler = engine.getSamplerCache().find( name );
		}
		
		auto & loader = engine.getImageLoader();
		//XpmLoader loader;
		std::array< castor::Image, NoiseMapCount > images
		{
			loader.load( cuT( "FilmGrainNoise0" ), "xpm", reinterpret_cast< uint8_t const * >( NoiseLayer1_xpm ), uint32_t( castor::getCountOf( NoiseLayer1_xpm ) ), false, false ),
			loader.load( cuT( "FilmGrainNoise1" ), "xpm", reinterpret_cast< uint8_t const * >( NoiseLayer2_xpm ), uint32_t( castor::getCountOf( NoiseLayer2_xpm ) ), false, false ),
			loader.load( cuT( "FilmGrainNoise2" ), "xpm", reinterpret_cast< uint8_t const * >( NoiseLayer3_xpm ), uint32_t( castor::getCountOf( NoiseLayer3_xpm ) ), false, false ),
			loader.load( cuT( "FilmGrainNoise3" ), "xpm", reinterpret_cast< uint8_t const * >( NoiseLayer4_xpm ), uint32_t( castor::getCountOf( NoiseLayer4_xpm ) ), false, false ),
			loader.load( cuT( "FilmGrainNoise4" ), "xpm", reinterpret_cast< uint8_t const * >( NoiseLayer5_xpm ), uint32_t( castor::getCountOf( NoiseLayer5_xpm ) ), false, false ),
			loader.load( cuT( "FilmGrainNoise5" ), "xpm", reinterpret_cast< uint8_t const * >( NoiseLayer6_xpm ), uint32_t( castor::getCountOf( NoiseLayer6_xpm ) ), false, false ),
		};

		for ( auto & image : images )
		{
			auto format = image.getPixelFormat();

			if ( format == castor::PixelFormat::eR8G8B8_UNORM )
			{
				auto buffer = castor::PxBufferBase::create( image.getDimensions()
					, castor::PixelFormat::eR8G8B8A8_UNORM
					, image.getPxBuffer().getConstPtr()
					, image.getPxBuffer().getFormat()
					, image.getPxBuffer().getAlign() );
				image = castor::Image{ image.getName()
					, image.getPath()
					, *buffer };
			}
		}

		auto dim = images[0].getDimensions();
		auto format = castor3d::convert( images[0].getPixelFormat() );
		auto staging = m_device->createStagingTexture( format
			, VkExtent2D{ dim.getWidth(), dim.getHeight() } );

		ashes::ImageCreateInfo image
		{
			0u,
			VK_IMAGE_TYPE_3D,
			format,
			{ dim.getWidth(), dim.getHeight(), NoiseMapCount },
			1u,
			1u,
			VK_SAMPLE_COUNT_1_BIT,
			VK_IMAGE_TILING_OPTIMAL,
			( VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_TRANSFER_DST_BIT ),
		};
		m_noise = castor3d::makeImage( m_device
			, std::move( image )
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, name );

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

		for ( uint32_t i = 0u; i < NoiseMapCount; ++i )
		{
			staging->uploadTextureData( *m_device.graphicsQueue
				, *m_device.graphicsCommandPool
				, {
					m_noiseView->subresourceRange.aspectMask,
					m_noiseView->subresourceRange.baseMipLevel,
					m_noiseView->subresourceRange.baseArrayLayer,
					m_noiseView->subresourceRange.layerCount,
				}
				, format
				, { 0, 0, int32_t( i ) }
				, { image->extent.width, image->extent.height }
				, images[i].getBuffer().data()
				, m_noiseView );
		}
	}

	void RenderQuad::update( castor3d::CpuUpdater & updater )
	{
		m_time += updater.time.count();
		m_configUbo.getData().m_time = ( m_time % NoiseMapCount ) / float( NoiseMapCount );
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
			, params }
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
		visitor.visit( m_vertexShader );
		visitor.visit( m_pixelShader );
		visitor.visit( m_pixelShader.name
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, cuT( "FilmGrain" )
			, cuT( "Exposure" )
			, m_quad->getUbo().getData().m_exposure );
		visitor.visit( m_pixelShader.name
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, cuT( "FilmGrain" )
			, cuT( "NoiseIntensity" )
			, m_quad->getUbo().getData().m_noiseIntensity );
	}

	void PostEffect::update( castor3d::CpuUpdater & updater )
	{
		m_quad->update( updater );
	}

	bool PostEffect::doInitialise( castor3d::RenderDevice const & device
		, castor3d::RenderPassTimer const & timer )
	{
		auto & renderSystem = *getRenderSystem();
		VkExtent2D size{ m_target->viewData.image.data->info.extent.width
			, m_target->viewData.image.data->info.extent.height };
		m_sampler->initialise( device );
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
				m_target->viewData.info.format,
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
		m_renderPass = device->createRenderPass( "FilmGrain"
			, std::move( createInfo ) );

		m_quad = std::make_unique< RenderQuad >( renderSystem
			, device
			, size );
		m_quad->createPipelineAndPass( size
			, Position{}
			, stages
			, *m_renderPass
			, {
				m_quad->makeDescriptorWrite( m_quad->getUbo()
					, FilmCfgUboIdx ),
				m_quad->makeDescriptorWrite( m_quad->getNoiseView()
					, m_sampler->getSampler()
					, NoiseTexIdx ),
				m_quad->makeDescriptorWrite( *m_target
					, m_sampler->getSampler()
					, SourceTexIdx ),
			} );

		auto result = m_surface.initialise( device
			, *m_renderPass
			, castor::Size{ size.width, size.height }
			, m_target->viewData.info.format );

		if ( result )
		{
			castor3d::CommandsSemaphore commands
			{
				device.graphicsCommandPool->createCommandBuffer( "FilmGrain" ),
				device->createSemaphore( "FilmGrain" )
			};
			auto & cmd = *commands.commandBuffer;
			cmd.begin();
			timer.beginPass( cmd );
			cmd.beginDebugBlock( { "FilmGrain"
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

		m_result = &m_surface.colourTexture->getDefaultView().getSampledView();
		return result;
	}

	void PostEffect::doCleanup( castor3d::RenderDevice const & device )
	{
		m_quad.reset();
		m_renderPass.reset();
		m_surface.cleanup( device );
	}

	bool PostEffect::doWriteInto( StringStream & file, String const & tabs )
	{
		file << ( tabs + cuT( "postfx \"" ) + Type + cuT( "\"" ) );
		return true;
	}
}
