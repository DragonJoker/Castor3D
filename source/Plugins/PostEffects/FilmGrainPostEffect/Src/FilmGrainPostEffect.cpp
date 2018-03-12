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
#include <Texture/Sampler.hpp>
#include <Texture/TextureLayout.hpp>
#include <Texture/TextureUnit.hpp>

#include <Buffer/UniformBuffer.hpp>
#include <Image/Texture.hpp>
#include <RenderPass/RenderPass.hpp>
#include <RenderPass/RenderPassCreateInfo.hpp>
#include <RenderPass/RenderSubpass.hpp>
#include <RenderPass/RenderSubpassState.hpp>
#include <Shader/ShaderProgram.hpp>

#include <GlslSource.hpp>

#include <numeric>

using namespace castor;

namespace film_grain
{
	namespace
	{
		static String const FilmGrainUbo = cuT( "FilmGrainUbo" );
		static String const PixelSize = cuT( "c3d_pixelSize" );
		static String const NoiseIntensity = cuT( "c3d_noiseIntensity" );
		static String const Exposure = cuT( "c3d_exposure" );
		static String const Time = cuT( "c3d_time" );
		static String const SrcTex = cuT( "c3d_srcTex" );
		static String const NoiseTex = cuT( "c3d_noiseTex" );

		glsl::Shader getVertexProgram( castor3d::RenderSystem * renderSystem )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem->createGlslWriter();

			// Shader inputs
			Vec2 position = writer.declAttribute< Vec2 >( cuT( "position" ), 0u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ), 0u );
			auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				vtx_texture = position;
				gl_Position = vec4( position.xy(), 0.0, 1.0 );
			} );
			return writer.finalise();
		}

		glsl::Shader getFragmentProgram( castor3d::RenderSystem * renderSystem )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem->createGlslWriter();

			// Shader inputs
			glsl::Ubo filmGrain{ writer, FilmGrainUbo, 0u };
			auto c3d_pixelSize = filmGrain.declMember< Vec2 >( PixelSize );
			auto c3d_noiseIntensity = filmGrain.declMember< Float >( NoiseIntensity );
			auto c3d_exposure = filmGrain.declMember< Float >( Exposure );
			auto c3d_time = filmGrain.declMember< Float >( Time );
			filmGrain.end();

			auto c3d_srcTex = writer.declSampler< Sampler2D >( SrcTex, 1u, 0u );
			auto c3d_noiseTex = writer.declSampler< Sampler3D >( NoiseTex, 2u, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ), 0u );

			// Shader outputs
			auto plx_fragColor = writer.declFragData< Vec4 >( cuT( "plx_fragColor" ), 0 );
			
			auto overlay = writer.implementFunction< Vec3 >( cuT( "overlay" )
				, [&]( Vec3 const & a
					, Vec3 const & b )
				{
					auto comp = writer.declLocale( cuT( "comp" )
						, pow( abs( b ), vec3( 2.2_f ) ) );

					IF( writer, cuT( "comp.x < 0.5 && comp.y < 0.5 && comp.z < 0.5" ) )
					{
						writer.returnStmt( 2.0_f * a * b );
					}
					FI;

					writer.returnStmt( vec3( 1.0_f ) - 2.0_f * ( 1.0_f - a ) * ( 1.0_f - b ) );
				}
				, InVec3{ &writer, cuT( "a" ) }
				, InVec3{ &writer, cuT( "b" ) } );

			auto addNoise = writer.implementFunction< Vec3 >( cuT( "addNoise" )
				, [&]( Vec3 const & color
					, Vec2 const & texcoord )
				{
					auto coord = writer.declLocale( cuT( "coord" )
						, texcoord * 2.0 );
					coord.x() *= c3d_pixelSize.y() / c3d_pixelSize.x();
					auto noise = writer.declLocale( cuT( "noise" )
						, texture( c3d_noiseTex, vec3( coord, c3d_time ) ).r() );
					auto exposureFactor = writer.declLocale( cuT( "exposureFactor" )
						, c3d_exposure / 2.0 );
					exposureFactor = sqrt( exposureFactor );
					auto t = writer.declLocale( cuT( "t" )
						, mix( 3.5_f * c3d_noiseIntensity
							, 1.13_f * c3d_noiseIntensity
							, exposureFactor ) );
					writer.returnStmt( overlay( color
						, vec3( mix( 0.5_f, noise, t ) ) ) );
				}
				, InVec3{ &writer, cuT( "color" ) }
				, InVec2{ &writer, cuT( "texcoord" ) } );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				auto colour = writer.declLocale( cuT( "colour" )
					, texture( c3d_srcTex, vtx_texture ).xyz() );
				colour = addNoise( colour, vtx_texture );
				plx_fragColor = vec4( colour, 1.0 );
			} );
			return writer.finalise();
		}

		template< typename T, size_t N >
		inline size_t getCountOf( T const( &p_data )[N] )
		{
			return N;
		}
	}

	//*********************************************************************************************

	RenderQuad::RenderQuad( castor3d::RenderSystem & renderSystem
		, renderer::Extent2D const & size )
		: castor3d::RenderQuad{ renderSystem, false, false }
		, m_size{ size }
	{
		auto name = cuT( "FilmGrain_Noise" );
		castor3d::SamplerSPtr sampler;

		if ( !m_renderSystem.getEngine()->getSamplerCache().has( name ) )
		{
			m_sampler = m_renderSystem.getEngine()->getSamplerCache().add( name );
			m_sampler->setMinFilter( renderer::Filter::eLinear );
			m_sampler->setMagFilter( renderer::Filter::eLinear );
			m_sampler->setMipFilter( renderer::MipmapMode::eLinear );
			m_sampler->setWrapS( renderer::WrapMode::eRepeat );
			m_sampler->setWrapT( renderer::WrapMode::eRepeat );
			m_sampler->setWrapR( renderer::WrapMode::eRepeat );
		}
		else
		{
			m_sampler = m_renderSystem.getEngine()->getSamplerCache().find( name );
		}

		auto & device = *m_renderSystem.getCurrentDevice();

		renderer::ImageCreateInfo image{};
		image.flags = 0u;
		image.arrayLayers = 1u;
		image.extent.width = 512u;
		image.extent.height = 512u;
		image.extent.depth = 6u;
		image.format = renderer::Format::eR8G8B8_UNORM;
		image.imageType = renderer::TextureType::e3D;
		image.initialLayout = renderer::ImageLayout::eUndefined;
		image.mipLevels = FILTER_COUNT;
		image.samples = renderer::SampleCountFlag::e1;
		image.sharingMode = renderer::SharingMode::eExclusive;
		image.tiling = renderer::ImageTiling::eOptimal;
		image.usage = renderer::ImageUsageFlag::eSampled | renderer::ImageUsageFlag::eTransferDst;
		m_noise = device.createTexture( image, renderer::MemoryPropertyFlag::eDeviceLocal );

		XpmLoader loader;
		std::array< PxBufferBaseSPtr, 6u > buffers
		{
			loader.loadImage( NoiseLayer1_xpm, getCountOf( NoiseLayer1_xpm ) ),
			loader.loadImage( NoiseLayer2_xpm, getCountOf( NoiseLayer2_xpm ) ),
			loader.loadImage( NoiseLayer3_xpm, getCountOf( NoiseLayer3_xpm ) ),
			loader.loadImage( NoiseLayer4_xpm, getCountOf( NoiseLayer4_xpm ) ),
			loader.loadImage( NoiseLayer5_xpm, getCountOf( NoiseLayer5_xpm ) ),
			loader.loadImage( NoiseLayer6_xpm, getCountOf( NoiseLayer6_xpm ) ),
		};

		uint32_t maxSize = 0u;
		renderer::BufferImageCopyArray copyRegions( 6u );

		for ( size_t i = 0u; i < buffers.size(); ++i )
		{
			copyRegions[i].imageExtent.width = buffers[i]->dimensions()[0];
			copyRegions[i].imageExtent.height = buffers[i]->dimensions()[1];
			copyRegions[i].imageExtent.depth = 1u;
			copyRegions[i].imageOffset.x = 0;
			copyRegions[i].imageOffset.y = 0;
			copyRegions[i].imageOffset.z = uint32_t( i );
			copyRegions[i].imageSubresource.aspectMask = renderer::getAspectMask( image.format );
			copyRegions[i].imageSubresource.baseArrayLayer = 0u;
			copyRegions[i].imageSubresource.layerCount = 1u;
			copyRegions[i].imageSubresource.mipLevel = 0u;
			copyRegions[i].levelSize = buffers[i]->size();
			maxSize = std::max( maxSize, copyRegions[i].levelSize );
		}

		renderer::BufferBasePtr stagingBuffer = device.createBuffer( maxSize, renderer::BufferTarget::eTransferSrc );
		renderer::CommandBufferPtr cmdCopy = device.getGraphicsCommandPool().createCommandBuffer( true );

		for ( size_t i = 0u; i < copyRegions.size(); ++i )
		{
			if ( auto * data = stagingBuffer->lock( 0u, copyRegions[i].levelSize, renderer::MemoryMapFlag::eWrite ) )
			{
				std::memcpy( data, buffers[i]->constPtr(), buffers[i]->size() );
				stagingBuffer->flush( 0u, copyRegions[i].levelSize );
				stagingBuffer->unlock();

				if ( cmdCopy->begin() )
				{
					cmdCopy->copyToImage( copyRegions[i]
						, *stagingBuffer
						, *m_noise );
					cmdCopy->end();
				}
			}
		}

		renderer::ImageViewCreateInfo imageView{};
		imageView.components.r = renderer::ComponentSwizzle::eIdentity;
		imageView.components.g = renderer::ComponentSwizzle::eIdentity;
		imageView.components.b = renderer::ComponentSwizzle::eIdentity;
		imageView.components.a = renderer::ComponentSwizzle::eIdentity;
		imageView.format = image.format;
		imageView.viewType = renderer::TextureViewType::e3D;
		imageView.subresourceRange.aspectMask = renderer::getAspectMask( imageView.format );
		imageView.subresourceRange.baseMipLevel = 0u;
		imageView.subresourceRange.levelCount = 1u;
		imageView.subresourceRange.baseArrayLayer = 0u;
		imageView.subresourceRange.layerCount = 1u;
		m_noiseView = m_noise->createView( imageView );

		m_configUbo = renderer::makeUniformBuffer< Configuration >( *m_renderSystem.getCurrentDevice()
			, 1u
			, renderer::BufferTarget::eTransferDst
			, renderer::MemoryPropertyFlag::eHostVisible );
		m_configUbo->getData( 0 ).m_pixelSize = Point2f{ m_size.width, m_size.height };
		m_configUbo->getData( 0 ).m_noiseIntensity = 1.0f;
		m_configUbo->getData( 0 ).m_exposure = 1.0f;
	}

	void RenderQuad::update( float time )
	{
		m_configUbo->getData().m_time = time;
		m_configUbo->upload();
	}

	void RenderQuad::doFillDescriptorSet( renderer::DescriptorSetLayout & descriptorSetLayout
		, renderer::DescriptorSet & descriptorSet )
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
			, renderTarget
			, renderSystem
			, params
			, false }
		, m_surface{ *renderSystem.getEngine() }
	{
		String name = cuT( "FilmGrain2D" );

		if ( !m_renderTarget.getEngine()->getSamplerCache().has( name ) )
		{
			m_sampler = m_renderTarget.getEngine()->getSamplerCache().add( name );
			m_sampler->setMinFilter( renderer::Filter::eLinear );
			m_sampler->setMagFilter( renderer::Filter::eLinear );
			m_sampler->setMipFilter( renderer::MipmapMode::eLinear );
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
		return std::make_shared< PostEffect >( renderTarget
			, renderSystem
			, params );
	}

	bool PostEffect::initialise()
	{
		auto & device = *getRenderSystem()->getCurrentDevice();
		renderer::Extent2D size{ m_renderTarget.getSize()[0], m_renderTarget.getSize()[1] };

		auto vtx = getVertexProgram( getRenderSystem() );
		auto pxl = getFragmentProgram( getRenderSystem() );

		renderer::ShaderStageStateArray stages;
		stages.push_back( { device.createShaderModule( renderer::ShaderStageFlag::eVertex ) } );
		stages.push_back( { device.createShaderModule( renderer::ShaderStageFlag::eFragment ) } );
		stages[0].module->loadShader( vtx.getSource() );
		stages[1].module->loadShader( pxl.getSource() );

		m_commandBuffer = device.getGraphicsCommandPool().createCommandBuffer();

		// Create the render pass.
		renderer::RenderPassCreateInfo renderPass;
		renderPass.flags = 0u;

		renderPass.attachments.resize( 1u );
		renderPass.attachments[0].index = 0u;
		renderPass.attachments[0].format = m_renderTarget.getPixelFormat();
		renderPass.attachments[0].loadOp = renderer::AttachmentLoadOp::eClear;
		renderPass.attachments[0].storeOp = renderer::AttachmentStoreOp::eDontCare;
		renderPass.attachments[0].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
		renderPass.attachments[0].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
		renderPass.attachments[0].samples = renderer::SampleCountFlag::e1;
		renderPass.attachments[0].initialLayout = renderer::ImageLayout::eUndefined;
		renderPass.attachments[0].finalLayout = renderer::ImageLayout::eShaderReadOnlyOptimal;

		renderPass.subpasses.resize( 1u );
		renderPass.subpasses[0].flags = 0u;
		renderPass.subpasses[0].pipelineBindPoint = renderer::PipelineBindPoint::eGraphics;
		renderPass.subpasses[0].colorAttachments.push_back( { 0u, renderer::ImageLayout::eColourAttachmentOptimal } );

		renderPass.dependencies.resize( 2u );
		renderPass.dependencies[0].srcSubpass = renderer::ExternalSubpass;
		renderPass.dependencies[0].dstSubpass = 0u;
		renderPass.dependencies[0].srcStageMask = renderer::PipelineStageFlag::eFragmentShader;
		renderPass.dependencies[0].dstStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[0].srcAccessMask = renderer::AccessFlag::eShaderRead;
		renderPass.dependencies[0].dstAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[0].dependencyFlags = renderer::DependencyFlag::eByRegion;

		renderPass.dependencies[1].srcSubpass = 0u;
		renderPass.dependencies[1].dstSubpass = renderer::ExternalSubpass;
		renderPass.dependencies[1].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[1].dstStageMask = renderer::PipelineStageFlag::eFragmentShader;
		renderPass.dependencies[1].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[1].dstAccessMask = renderer::AccessFlag::eShaderRead;
		renderPass.dependencies[1].dependencyFlags = renderer::DependencyFlag::eByRegion;

		m_renderPass = device.createRenderPass( renderPass );

		renderer::DescriptorSetLayoutBindingArray bindings
		{
			{ 0u, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eFragment },
			{ 1u, renderer::DescriptorType::eSampledImage, renderer::ShaderStageFlag::eFragment },
		};

		m_quad = std::make_unique< RenderQuad >( *getRenderSystem(), size );
		m_quad->createPipeline( size
			, Position{}
			, stages
			, m_renderTarget.getTexture().getTexture()->getDefaultView()
			, *m_renderPass
			, bindings
			, {} );

		auto result = m_surface.initialise( m_renderTarget
			, *m_renderPass
			, m_renderTarget.getSize()
			, 0u
			, m_sampler );

		if ( result
			&& m_commandBuffer->begin( renderer::CommandBufferUsageFlag::eSimultaneousUse ) )
		{
			// Put image in the right state for rendering.
			m_commandBuffer->memoryBarrier( renderer::PipelineStageFlag::eColourAttachmentOutput
				, renderer::PipelineStageFlag::eFragmentShader
				, m_renderTarget.getTexture().getTexture()->getDefaultView().makeShaderInputResource( renderer::ImageLayout::eColourAttachmentOptimal
					, renderer::AccessFlag::eColourAttachmentWrite ) );

			m_commandBuffer->beginRenderPass( *m_renderPass
				, *m_surface.m_fbo
				, {}
				, renderer::SubpassContents::eInline );
			m_quad->registerFrame( *m_commandBuffer );
			m_commandBuffer->endRenderPass();

			// Blit the result to the render target image.
			renderer::ImageCopy imageCopy;
			imageCopy.dstOffset = { 0, 0, 0 };
			imageCopy.srcOffset = { 0, 0, 0 };
			imageCopy.extent = m_renderTarget.getTexture().getTexture()->getDimensions();
			imageCopy.dstSubresource.aspectMask = renderer::getAspectMask( m_renderTarget.getTexture().getTexture()->getDefaultView().getFormat() );
			imageCopy.dstSubresource.baseArrayLayer = 0u;
			imageCopy.dstSubresource.layerCount = 1u;
			imageCopy.dstSubresource.mipLevel = 0u;
			imageCopy.srcSubresource.aspectMask = renderer::getAspectMask( m_surface.m_colourTexture.getTexture()->getTexture().getFormat() );
			imageCopy.srcSubresource.baseArrayLayer = 0u;
			imageCopy.srcSubresource.layerCount = 1u;
			imageCopy.srcSubresource.mipLevel = 0u;
			m_commandBuffer->copyImage( imageCopy
				, m_surface.m_colourTexture.getTexture()->getTexture()
				, renderer::ImageLayout::eColourAttachmentOptimal
				, m_renderTarget.getTexture().getTexture()->getTexture()
				, renderer::ImageLayout::eShaderReadOnlyOptimal );
			m_commandBuffer->memoryBarrier( renderer::PipelineStageFlag::eTransfer
				, renderer::PipelineStageFlag::eColourAttachmentOutput
				, m_renderTarget.getTexture().getTexture()->getDefaultView().makeColourAttachment( renderer::ImageLayout::eTransferDstOptimal
					, renderer::AccessFlag::eTransferWrite ) );
			m_commandBuffer->end();
		}

		return result;
	}

	void PostEffect::cleanup()
	{
		m_quad.reset();
		m_commandBuffer.reset();
		m_renderPass.reset();
		m_surface.cleanup();
	}

	bool PostEffect::doWriteInto( TextFile & file )
	{
		return true;
	}
}
