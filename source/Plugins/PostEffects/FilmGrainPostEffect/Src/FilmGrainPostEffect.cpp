#include "FilmGrainPostEffect.hpp"

#include "XpmLoader.hpp"

#include "NoiseLayer1.xpm"
#include "NoiseLayer2.xpm"
#include "NoiseLayer3.xpm"
#include "NoiseLayer4.xpm"
#include "NoiseLayer5.xpm"
#include "NoiseLayer6.xpm"

#include <Engine.hpp>
#include <Cache/SamplerCache.hpp>
#include <Cache/ShaderCache.hpp>

#include <Miscellaneous/Parameter.hpp>
#include <Render/RenderSystem.hpp>
#include <Render/RenderTarget.hpp>
#include <Render/RenderPassTimer.hpp>
#include <Shader/Shaders/GlslUtils.hpp>
#include <Texture/Sampler.hpp>
#include <Texture/TextureLayout.hpp>
#include <Texture/TextureUnit.hpp>

#include <Ashes/Buffer/UniformBuffer.hpp>
#include <Ashes/Image/StagingTexture.hpp>
#include <Ashes/Image/Texture.hpp>
#include <Ashes/RenderPass/RenderPass.hpp>
#include <Ashes/RenderPass/RenderPassCreateInfo.hpp>
#include <Ashes/RenderPass/RenderSubpass.hpp>
#include <Ashes/RenderPass/RenderSubpassState.hpp>
#include <Ashes/Sync/ImageMemoryBarrier.hpp>

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

			castor3d::shader::Utils utils{ writer, renderSystem->isTopDown() };
			utils.declareInvertVec2Y();

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					vtx_texture = utils.bottomUpToTopDown( uv );
					out.gl_out.gl_Position = vec4( position.xy(), 0.0, 1.0 );
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
						, c3d_exposure / 2.0 );
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

		template< typename T, size_t N >
		inline size_t getCountOf( T const( &p_data )[N] )
		{
			return N;
		}
	}

	//*********************************************************************************************

	RenderQuad::RenderQuad( castor3d::RenderSystem & renderSystem
		, ashes::Extent2D const & size )
		: castor3d::RenderQuad{ renderSystem, false, false }
		, m_size{ size }
	{
		auto name = cuT( "FilmGrain_Noise" );
		castor3d::SamplerSPtr sampler;

		if ( !m_renderSystem.getEngine()->getSamplerCache().has( name ) )
		{
			m_sampler = m_renderSystem.getEngine()->getSamplerCache().add( name );
			m_sampler->setMinFilter( ashes::Filter::eLinear );
			m_sampler->setMagFilter( ashes::Filter::eLinear );
			m_sampler->setMipFilter( ashes::MipmapMode::eLinear );
			m_sampler->setWrapS( ashes::WrapMode::eRepeat );
			m_sampler->setWrapT( ashes::WrapMode::eRepeat );
			m_sampler->setWrapR( ashes::WrapMode::eRepeat );
			m_sampler->initialise();
		}
		else
		{
			m_sampler = m_renderSystem.getEngine()->getSamplerCache().find( name );
		}

		auto & device = getCurrentDevice( m_renderSystem );

		ashes::ImageCreateInfo image{};
		image.flags = 0u;
		image.arrayLayers = 1u;
		image.extent.width = 512u;
		image.extent.height = 512u;
		image.extent.depth = NoiseMapCount;
		image.format = ashes::Format::eR8G8B8A8_UNORM;
		image.imageType = ashes::TextureType::e3D;
		image.initialLayout = ashes::ImageLayout::eUndefined;
		image.mipLevels = 1u;
		image.samples = ashes::SampleCountFlag::e1;
		image.sharingMode = ashes::SharingMode::eExclusive;
		image.tiling = ashes::ImageTiling::eOptimal;
		image.usage = ashes::ImageUsageFlag::eSampled | ashes::ImageUsageFlag::eTransferDst;
		m_noise = device.createTexture( image, ashes::MemoryPropertyFlag::eDeviceLocal );

		ashes::ImageViewCreateInfo imageView{};
		imageView.format = image.format;
		imageView.viewType = ashes::TextureViewType::e3D;
		imageView.subresourceRange.aspectMask = ashes::getAspectMask( imageView.format );
		imageView.subresourceRange.baseMipLevel = 0u;
		imageView.subresourceRange.levelCount = 1u;
		imageView.subresourceRange.baseArrayLayer = 0u;
		imageView.subresourceRange.layerCount = 1u;
		m_noiseView = m_noise->createView( imageView );

		XpmLoader loader;
		std::array< PxBufferBaseSPtr, NoiseMapCount > buffers
		{
			loader.loadImage( NoiseLayer1_xpm, getCountOf( NoiseLayer1_xpm ) ),
			loader.loadImage( NoiseLayer2_xpm, getCountOf( NoiseLayer2_xpm ) ),
			loader.loadImage( NoiseLayer3_xpm, getCountOf( NoiseLayer3_xpm ) ),
			loader.loadImage( NoiseLayer4_xpm, getCountOf( NoiseLayer4_xpm ) ),
			loader.loadImage( NoiseLayer5_xpm, getCountOf( NoiseLayer5_xpm ) ),
			loader.loadImage( NoiseLayer6_xpm, getCountOf( NoiseLayer6_xpm ) ),
		};

		uint32_t maxSize = buffers[0]->size();
		auto dim = buffers[0]->dimensions();
		auto format = castor3d::convert( buffers[0]->format() );
		auto staging = device.createStagingTexture( format
			, ashes::Extent2D{ dim.getWidth(), dim.getHeight() } );
		ashes::CommandBufferPtr cmdCopy = device.getGraphicsCommandPool().createCommandBuffer( true );

		for ( uint32_t i = 0u; i < NoiseMapCount; ++i )
		{
			staging->uploadTextureData( *cmdCopy
				, {
					m_noiseView->getSubResourceRange().aspectMask,
					m_noiseView->getSubResourceRange().baseMipLevel,
					m_noiseView->getSubResourceRange().baseArrayLayer,
					m_noiseView->getSubResourceRange().layerCount,
				}
				, format
				, { 0, 0, int32_t( i ) }
				, { image.extent.width, image.extent.height }
				, buffers[i]->constPtr()
				, *m_noiseView );
		}

		m_configUbo = ashes::makeUniformBuffer< Configuration >( getCurrentDevice( m_renderSystem )
			, 1u
			, ashes::BufferTarget::eTransferDst
			, ashes::MemoryPropertyFlag::eHostVisible );
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
			, *m_noiseView
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
		, m_surface{ *renderSystem.getEngine() }
		, m_vertexShader{ ashes::ShaderStageFlag::eVertex, "FilmGrain" }
		, m_pixelShader{ ashes::ShaderStageFlag::eFragment, "FilmGrain" }
	{
		String name = cuT( "FilmGrain2D" );

		if ( !m_renderTarget.getEngine()->getSamplerCache().has( name ) )
		{
			m_sampler = m_renderTarget.getEngine()->getSamplerCache().add( name );
			m_sampler->setMinFilter( ashes::Filter::eLinear );
			m_sampler->setMagFilter( ashes::Filter::eLinear );
			m_sampler->setWrapS( ashes::WrapMode::eRepeat );
			m_sampler->setWrapT( ashes::WrapMode::eRepeat );
			m_sampler->setWrapR( ashes::WrapMode::eRepeat );
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
			, ashes::ShaderStageFlag::eVertex
			, *m_vertexShader.shader );
		visitor.visit( cuT( "FilmGrain" )
			, ashes::ShaderStageFlag::eFragment
			, *m_pixelShader.shader );
		visitor.visit( cuT( "FilmGrain" )
			, ashes::ShaderStageFlag::eFragment
			, cuT( "FilmGrain" )
			, cuT( "Exposure" )
			, m_quad->getUbo().getData().m_exposure );
		visitor.visit( cuT( "FilmGrain" )
			, ashes::ShaderStageFlag::eFragment
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
		auto & device = getCurrentDevice( *this );
		ashes::Extent2D size{ m_target->getWidth(), m_target->getHeight() };
		m_sampler->initialise();
		m_vertexShader.shader = getVertexProgram( getRenderSystem() );
		m_pixelShader.shader = getFragmentProgram( getRenderSystem() );

		ashes::ShaderStageStateArray stages;
		stages.push_back( { device.createShaderModule( ashes::ShaderStageFlag::eVertex ) } );
		stages.push_back( { device.createShaderModule( ashes::ShaderStageFlag::eFragment ) } );
		stages[0].module->loadShader( renderSystem.compileShader( m_vertexShader ) );
		stages[1].module->loadShader( renderSystem.compileShader( m_pixelShader ) );

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
		renderPass.subpasses[0].flags = 0u;
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

		ashes::DescriptorSetLayoutBindingArray bindings
		{
			{ 0u, ashes::DescriptorType::eUniformBuffer, ashes::ShaderStageFlag::eFragment },
			{ 1u, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment },
		};

		m_quad = std::make_unique< RenderQuad >( *getRenderSystem(), size );
		m_quad->createPipeline( size
			, Position{}
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
			device.getGraphicsCommandPool().createCommandBuffer(),
			device.createSemaphore()
		};
		auto & cmd = *commands.commandBuffer;

		if ( result )
		{
			cmd.begin();
			timer.beginPass( cmd );
			// Put image in the right state for rendering.
			cmd.memoryBarrier( ashes::PipelineStageFlag::eColourAttachmentOutput
				, ashes::PipelineStageFlag::eFragmentShader
				, m_target->getDefaultView().makeShaderInputResource( ashes::ImageLayout::eUndefined, 0u ) );

			cmd.beginRenderPass( *m_renderPass
				, *m_surface.frameBuffer
				, { ashes::ClearColorValue{} }
				, ashes::SubpassContents::eInline );
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
