#include "GaussianBlur.hpp"

#include "Engine.hpp"
#include "Render/RenderPassTimer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/Camera.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"
#include "Texture/TextureUnit.hpp"

#include <Ashes/Buffer/UniformBuffer.hpp>
#include <Ashes/Buffer/VertexBuffer.hpp>
#include <Ashes/Core/Renderer.hpp>
#include <Ashes/Descriptor/DescriptorSet.hpp>
#include <Ashes/Descriptor/DescriptorSetLayout.hpp>
#include <Ashes/Descriptor/DescriptorSetPool.hpp>
#include <Ashes/RenderPass/FrameBuffer.hpp>
#include <Ashes/RenderPass/RenderPass.hpp>
#include <Ashes/RenderPass/RenderPassCreateInfo.hpp>
#include <Ashes/RenderPass/RenderSubpass.hpp>
#include <Ashes/RenderPass/RenderSubpassState.hpp>
#include <Ashes/RenderPass/FrameBufferAttachment.hpp>
#include <Shader/GlslToSpv.hpp>

#include <ShaderWriter/Source.hpp>
#include "Shader/Shaders/GlslUtils.hpp"

#include "Shader/Shaders/GlslLight.hpp"
#include "Shader/Shaders/GlslShadow.hpp"

#include <random>

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	namespace
	{
		ShaderPtr getVertexProgram( RenderSystem & renderSystem )
		{
			using namespace sdw;
			VertexWriter writer;

			// Shader inputs
			auto position = writer.declInput< Vec2 >( cuT( "position" ), 0u );
			auto uv = writer.declInput< Vec2 >( cuT( "uv" ), 1u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ), 0u );
			auto out = writer.getOut();

			writer.implementFunction< Void >( cuT( "main" )
				, [&]()
				{
					vtx_texture = uv;
					out.gl_out.gl_Position = vec4( position, 0.0_f, 1.0_f );
				} );
			return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}

		ShaderPtr getBlurXProgram( RenderSystem & renderSystem, bool isDepth )
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			Ubo config{ writer, GaussianBlur::Config, 0u, 0u };
			auto c3d_textureSize = config.declMember< Vec2 >( GaussianBlur::TextureSize );
			auto c3d_coefficientsCount = config.declMember< UInt >( GaussianBlur::CoefficientsCount );
			auto c3d_dump = config.declMember< UInt >( cuT( "c3d_dump" ) ); // to keep a 16 byte alignment.
			auto c3d_coefficients = config.declMember< Vec4 >( GaussianBlur::Coefficients, GaussianBlur::MaxCoefficients / 4 );
			config.end();
			auto c3d_mapDiffuse = writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapDiffuse" ), 1u, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ), 0u );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( cuT( "pxl_fragColor" ), 0u );
			auto out = writer.getOut();

			writer.implementFunction< sdw::Void >( cuT( "main" )
				, [&]()
				{
					auto base = writer.declLocale( cuT( "base" ), vec2( 1.0_f, 0.0_f ) / c3d_textureSize );
					auto offset = writer.declLocale( cuT( "offset" ), vec2( 0.0_f, 0.0_f ) );
					pxl_fragColor = texture( c3d_mapDiffuse, vtx_texture ) * c3d_coefficients[0][0];

					FOR( writer, UInt, i, 1u, i < c3d_coefficientsCount, ++i )
					{
						offset += base;
						pxl_fragColor += c3d_coefficients[i / 4_u][i % 4_u] * texture( c3d_mapDiffuse, vtx_texture - offset );
						pxl_fragColor += c3d_coefficients[i / 4_u][i % 4_u] * texture( c3d_mapDiffuse, vtx_texture + offset );
					}
					ROF;

					if ( isDepth )
					{
						out.gl_FragDepth = pxl_fragColor.r();
					}
				} );
			return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}

		ShaderPtr getBlurXProgramLayer( RenderSystem & renderSystem
			, bool isDepth
			, uint32_t layer )
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			Ubo config{ writer, GaussianBlur::Config, 0u, 0u };
			auto c3d_textureSize = config.declMember< Vec2 >( GaussianBlur::TextureSize );
			auto c3d_coefficientsCount = config.declMember< UInt >( GaussianBlur::CoefficientsCount );
			auto c3d_dump = config.declMember< UInt >( cuT( "c3d_dump" ) ); // to keep a 16 byte alignment.
			auto c3d_coefficients = config.declMember< Vec4 >( GaussianBlur::Coefficients, GaussianBlur::MaxCoefficients / 4 );
			config.end();
			auto c3d_mapDiffuse = writer.declSampledImage< FImg2DArrayRgba32 >( cuT( "c3d_mapDiffuse" ), 1u, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ), 0u );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( cuT( "pxl_fragColor" ), 0u );
			auto out = writer.getOut();

			writer.implementFunction< sdw::Void >( cuT( "main" ), [&]()
			{
				auto base = writer.declLocale( cuT( "base" ), vec2( 1.0_f, 0 ) / c3d_textureSize );
				auto offset = writer.declLocale( cuT( "offset" ), vec2( 0.0_f, 0 ) );
				pxl_fragColor = texture( c3d_mapDiffuse, vec3( vtx_texture, Float( float( layer ) ) ) ) * c3d_coefficients[0][0];

				FOR( writer, UInt, i, 1u, i < c3d_coefficientsCount, ++i )
				{
					offset += base;
					pxl_fragColor += c3d_coefficients[i / 4_u][i % 4_u] * texture( c3d_mapDiffuse, vec3( vtx_texture - offset, Float( float( layer ) ) ) );
					pxl_fragColor += c3d_coefficients[i / 4_u][i % 4_u] * texture( c3d_mapDiffuse, vec3( vtx_texture + offset, Float( float( layer ) ) ) );
				}
				ROF;

				if ( isDepth )
				{
					out.gl_FragDepth = pxl_fragColor.r();
				}
			} );
			return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}

		ShaderPtr getBlurYProgram( RenderSystem & renderSystem, bool isDepth )
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			Ubo config{ writer, GaussianBlur::Config, 0u, 0u };
			auto c3d_textureSize = config.declMember< Vec2 >( GaussianBlur::TextureSize );
			auto c3d_coefficientsCount = config.declMember< UInt >( GaussianBlur::CoefficientsCount );
			auto c3d_dump = config.declMember< UInt >( cuT( "c3d_dump" ) ); // to keep a 16 byte alignment.
			auto c3d_coefficients = config.declMember< Vec4 >( GaussianBlur::Coefficients, GaussianBlur::MaxCoefficients / 4 );
			config.end();
			auto c3d_mapDiffuse = writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapDiffuse" ), 1u, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ), 0u );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( cuT( "pxl_fragColor" ), 0 );
			auto out = writer.getOut();

			writer.implementFunction< sdw::Void >( cuT( "main" ), [&]()
			{
				auto base = writer.declLocale( cuT( "base" ), vec2( 0.0_f, 1 ) / c3d_textureSize );
				auto offset = writer.declLocale( cuT( "offset" ), vec2( 0.0_f, 0 ) );
				pxl_fragColor = texture( c3d_mapDiffuse, vtx_texture ) * c3d_coefficients[0][0];

				FOR( writer, UInt, i, 1u, i < c3d_coefficientsCount, ++i )
				{
					offset += base;
					pxl_fragColor += c3d_coefficients[i / 4_u][i % 4_u] * texture( c3d_mapDiffuse, vtx_texture - offset );
					pxl_fragColor += c3d_coefficients[i / 4_u][i % 4_u] * texture( c3d_mapDiffuse, vtx_texture + offset );
				}
				ROF;

				if ( isDepth )
				{
					out.gl_FragDepth = pxl_fragColor.r();
				}
			} );
			return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}

		std::vector< float > getHalfPascal( uint32_t height )
		{
			std::vector< float > result;
			result.resize( height );
			auto x = 1.0f;
			auto max = 1 + height;

			for ( uint32_t i = 0u; i <= max; ++i )
			{
				auto index = max - i;

				if ( index < height )
				{
					result[index] = x;
				}

				x = x * ( ( height + 1 ) * 2 - i ) / ( i + 1 );
			}

			// Normalize kernel coefficients
			float sum = result[0];

			for ( uint32_t i = 1u; i < height; ++i )
			{
				sum += result[i] * 2;
			}

			std::transform( std::begin( result )
				, std::end( result )
				, std::begin( result )
				, [&sum]( float & value )
				{
					return value /= sum;
				} );

			return result;
		}

		SamplerSPtr doCreateSampler( Engine & engine
			, String const & name )
		{
			SamplerSPtr sampler;

			if ( engine.getSamplerCache().has( name ) )
			{
				sampler = engine.getSamplerCache().find( name );
			}
			else
			{
				sampler = engine.getSamplerCache().add( name );
				sampler->setMinFilter( ashes::Filter::eNearest );
				sampler->setMagFilter( ashes::Filter::eNearest );
				sampler->setWrapS( ashes::WrapMode::eClampToEdge );
				sampler->setWrapT( ashes::WrapMode::eClampToEdge );
			}

			return sampler;
		}

		TextureUnit doCreateTexture( Engine & engine
			, ashes::Extent2D const & size
			, ashes::Format format
			, String const & name )
		{
			auto & renderSystem = *engine.getRenderSystem();
			ashes::ImageCreateInfo image{};
			image.flags = 0u;
			image.arrayLayers = 1u;
			image.extent.width = size.width;
			image.extent.height = size.height;
			image.extent.depth = 1u;
			image.format = format;
			image.imageType = ashes::TextureType::e2D;
			image.initialLayout = ashes::ImageLayout::eUndefined;
			image.mipLevels = 1u;
			image.samples = ashes::SampleCountFlag::e1;
			image.sharingMode = ashes::SharingMode::eExclusive;
			image.tiling = ashes::ImageTiling::eOptimal;
			image.usage = ashes::ImageUsageFlag::eColourAttachment | ashes::ImageUsageFlag::eSampled;
			auto sampler = doCreateSampler( engine, name );
			auto texture = std::make_shared< TextureLayout >( renderSystem
				, image
				, ashes::MemoryPropertyFlag::eDeviceLocal );
			texture->getDefaultImage().initialiseSource();
			TextureUnit unit{ engine };
			unit.setTexture( texture );
			unit.setSampler( sampler );
			unit.initialise();
			return unit;
		}

		ashes::RenderPassPtr doCreateRenderPass( ashes::Device const & device
			, ashes::Format format )
		{
			ashes::RenderPassCreateInfo createInfo{};
			createInfo.flags = 0u;

			createInfo.attachments.resize( 1u );
			createInfo.attachments[0].format = format;
			createInfo.attachments[0].samples = ashes::SampleCountFlag::e1;
			createInfo.attachments[0].loadOp = ashes::AttachmentLoadOp::eClear;
			createInfo.attachments[0].storeOp = ashes::AttachmentStoreOp::eStore;
			createInfo.attachments[0].stencilLoadOp = ashes::AttachmentLoadOp::eDontCare;
			createInfo.attachments[0].stencilStoreOp = ashes::AttachmentStoreOp::eDontCare;
			createInfo.attachments[0].initialLayout = ashes::ImageLayout::eUndefined;
			createInfo.attachments[0].finalLayout = ashes::ImageLayout::eShaderReadOnlyOptimal;

			ashes::AttachmentReference colourReference;
			colourReference.attachment = 0u;
			colourReference.layout = ashes::ImageLayout::eColourAttachmentOptimal;

			createInfo.subpasses.resize( 1u );
			createInfo.subpasses[0].flags = 0u;
			createInfo.subpasses[0].colorAttachments = { colourReference };

			createInfo.dependencies.resize( 2u );
			createInfo.dependencies[0].srcSubpass = ashes::ExternalSubpass;
			createInfo.dependencies[0].dstSubpass = 0u;
			createInfo.dependencies[0].srcStageMask = ashes::PipelineStageFlag::eHost;
			createInfo.dependencies[0].dstStageMask = ashes::PipelineStageFlag::eColourAttachmentOutput;
			createInfo.dependencies[0].srcAccessMask = ashes::AccessFlag::eHostWrite;
			createInfo.dependencies[0].dstAccessMask = ashes::AccessFlag::eColourAttachmentWrite;
			createInfo.dependencies[0].dependencyFlags = ashes::DependencyFlag::eByRegion;

			createInfo.dependencies[1].srcSubpass = 0u;
			createInfo.dependencies[1].dstSubpass = ashes::ExternalSubpass;
			createInfo.dependencies[1].srcStageMask = ashes::PipelineStageFlag::eColourAttachmentOutput;
			createInfo.dependencies[1].dstStageMask = ashes::PipelineStageFlag::eFragmentShader;
			createInfo.dependencies[1].srcAccessMask = ashes::AccessFlag::eColourAttachmentWrite;
			createInfo.dependencies[1].dstAccessMask = ashes::AccessFlag::eShaderRead;
			createInfo.dependencies[1].dependencyFlags = ashes::DependencyFlag::eByRegion;

			return device.createRenderPass( createInfo );
		}

		ashes::FrameBufferPtr doCreateFbo( ashes::RenderPass const & renderPass
			, ashes::TextureView const & view
			, ashes::Extent2D const & size )
		{
			ashes::FrameBufferAttachmentArray attaches;
			attaches.emplace_back( *( renderPass.getAttachments().begin() ), view );
			return renderPass.createFrameBuffer( size, std::move( attaches ) );
		}
	}

	//*********************************************************************************************

	GaussianBlur::RenderQuad::RenderQuad( RenderSystem & renderSystem
		, ashes::TextureView const & src
		, ashes::TextureView const & dst
		, ashes::UniformBuffer< Configuration > const & blurUbo
		, ashes::Format format
		, ashes::Extent2D const & size )
		: castor3d::RenderQuad{ renderSystem, false, false }
		, m_srcView{ src }
		, m_dstView{ dst }
		, m_blurUbo{ blurUbo }
	{
	}

	void GaussianBlur::RenderQuad::doFillDescriptorSet( ashes::DescriptorSetLayout & descriptorSetLayout
		, ashes::DescriptorSet & descriptorSet )
	{
		descriptorSet.createBinding( descriptorSetLayout.getBinding( 0u )
			, m_blurUbo
			, 0u
			, 1u );
	}

	//*********************************************************************************************

	String const GaussianBlur::Config = cuT( "Config" );
	String const GaussianBlur::Coefficients = cuT( "c3d_coefficients" );
	String const GaussianBlur::CoefficientsCount = cuT( "c3d_coefficientsCount" );
	String const GaussianBlur::TextureSize = cuT( "c3d_textureSize" );

	GaussianBlur::GaussianBlur( Engine & engine
		, ashes::TextureView const & texture
		, ashes::Extent2D const & textureSize
		, ashes::Format format
		, uint32_t kernelSize )
		: OwnedBy< Engine >{ engine }
		, m_source{ texture }
		, m_size{ textureSize }
		, m_format{ format }
		, m_intermediate{ doCreateTexture( engine, textureSize, format, cuT( "GaussianBlur" ) ) }
		, m_renderPass{ doCreateRenderPass( getCurrentDevice( engine ), m_format ) }
		, m_blurUbo{ ashes::makeUniformBuffer< Configuration >( getCurrentDevice( engine )
			, 1u
			, ashes::BufferTarget::eTransferDst
			, ashes::MemoryPropertyFlag::eHostVisible ) }
		, m_blurXQuad
		{
			*engine.getRenderSystem(),
			texture,
			m_intermediate.getTexture()->getDefaultView(),
			*m_blurUbo,
			format,
			textureSize
		}
		, m_blurYQuad
		{
			*engine.getRenderSystem(),
			m_intermediate.getTexture()->getDefaultView(),
			texture,
			*m_blurUbo,
			format,
			textureSize
		}
		, m_kernel{ getHalfPascal( kernelSize ) }
		, m_blurXVertexShader{ ashes::ShaderStageFlag::eVertex, "GaussianBlurX" }
		, m_blurXPixelShader{ ashes::ShaderStageFlag::eFragment, "GaussianBlurX" }
		, m_blurYVertexShader{ ashes::ShaderStageFlag::eVertex, "GaussianBlurY" }
		, m_blurYPixelShader{ ashes::ShaderStageFlag::eFragment, "GaussianBlurY" }
	{
		CU_Require( kernelSize < MaxCoefficients );
		auto & data = m_blurUbo->getData( 0u );
		data.blurCoeffsCount = uint32_t( m_kernel.size() );
		data.dump = 0u;
		std::memcpy( data.blurCoeffs.data()->ptr()
			, m_kernel.data()
			, sizeof( float ) * std::min( size_t( MaxCoefficients ), m_kernel.size() ) );
		data.textureSize[0] = float( textureSize.width );
		data.textureSize[1] = float( textureSize.height );
		m_blurUbo->upload();
		auto & device = getCurrentDevice( engine );
		m_horizCommandBuffer = device.getGraphicsCommandPool().createCommandBuffer();
		m_verticCommandBuffer = device.getGraphicsCommandPool().createCommandBuffer();
		m_horizSemaphore = device.createSemaphore();
		m_verticSemaphore = device.createSemaphore();
		doInitialiseBlurXProgram();
		doInitialiseBlurYProgram();

		m_horizCommandBuffer->begin();
		m_horizCommandBuffer->beginRenderPass( *m_renderPass
			, *m_blurXFbo
			, { ashes::ClearColorValue{ 0, 0, 0, 0 } }
		, ashes::SubpassContents::eSecondaryCommandBuffers );
		m_horizCommandBuffer->executeCommands( { m_blurXQuad.getCommandBuffer() } );
		m_horizCommandBuffer->endRenderPass();
		m_horizCommandBuffer->end();

		m_verticCommandBuffer->begin();
		m_verticCommandBuffer->beginRenderPass( *m_renderPass
			, *m_blurYFbo
			, { ashes::ClearColorValue{ 0, 0, 0, 0 } }
		, ashes::SubpassContents::eSecondaryCommandBuffers );
		m_verticCommandBuffer->executeCommands( { m_blurYQuad.getCommandBuffer() } );
		m_verticCommandBuffer->endRenderPass();
		m_verticCommandBuffer->end();
	}

	ashes::Semaphore const & GaussianBlur::blur( ashes::Semaphore const & toWait )
	{
		auto * result = &toWait;
		auto & device = getCurrentDevice( *this );

		device.getGraphicsQueue().submit( *m_horizCommandBuffer
			, *result
			, ashes::PipelineStageFlag::eColourAttachmentOutput
			, *m_horizSemaphore
			, nullptr );
		result = m_horizSemaphore.get();

		device.getGraphicsQueue().submit( *m_verticCommandBuffer
			, *result
			, ashes::PipelineStageFlag::eColourAttachmentOutput
			, *m_verticSemaphore
			, nullptr );
		result = m_verticSemaphore.get();

		return *result;
	}

	bool GaussianBlur::doInitialiseBlurXProgram()
	{
		auto & renderSystem = *getEngine()->getRenderSystem();
		auto & device = *renderSystem.getCurrentDevice();
		m_blurXVertexShader.shader = getVertexProgram( renderSystem );

		if ( m_source.getTexture().getLayerCount() > 1u
			&& !device.getRenderer().getFeatures().hasImageTexture )
		{
			m_blurXPixelShader.shader = getBlurXProgramLayer( *getEngine()->getRenderSystem()
				, ashes::isDepthFormat( m_format )
				, m_source.getSubResourceRange().baseArrayLayer );
		}
		else
		{
			m_blurXPixelShader.shader = getBlurXProgram( *getEngine()->getRenderSystem(), ashes::isDepthFormat( m_format ) );
		}

		ashes::ShaderStageStateArray program
		{
			{ device.createShaderModule( ashes::ShaderStageFlag::eVertex ) },
			{ device.createShaderModule( ashes::ShaderStageFlag::eFragment ) }
		};
		program[0].module->loadShader( renderSystem.compileShader( m_blurXVertexShader ) );
		program[1].module->loadShader( renderSystem.compileShader( m_blurXPixelShader ) );

		m_blurXFbo = doCreateFbo( *m_renderPass, m_intermediate.getTexture()->getDefaultView(), m_size );
		ashes::DescriptorSetLayoutBindingArray bindings
		{
			{ 0u, ashes::DescriptorType::eUniformBuffer, ashes::ShaderStageFlag::eFragment }
		};
		m_blurXQuad.createPipeline( m_size
			, Position{}
			, program
			, m_source
			, *m_renderPass
			, bindings
			, {} );
		m_blurXQuad.prepareFrame( *m_renderPass, 0u );
		return true;
	}

	bool GaussianBlur::doInitialiseBlurYProgram()
	{
		auto & renderSystem = *getEngine()->getRenderSystem();
		auto & device = *renderSystem.getCurrentDevice();
		m_blurYVertexShader.shader = getVertexProgram( renderSystem );
		m_blurYPixelShader.shader = getBlurYProgram( renderSystem, ashes::isDepthFormat( m_format ) );

		ashes::ShaderStageStateArray program
		{
			{ device.createShaderModule( ashes::ShaderStageFlag::eVertex ) },
			{ device.createShaderModule( ashes::ShaderStageFlag::eFragment ) }
		};
		program[0].module->loadShader( getEngine()->getRenderSystem()->compileShader( m_blurYVertexShader ) );
		program[1].module->loadShader( getEngine()->getRenderSystem()->compileShader( m_blurYPixelShader ) );

		m_blurYFbo = doCreateFbo( *m_renderPass, m_source, m_size );
		ashes::DescriptorSetLayoutBindingArray bindings
		{
			{ 0u, ashes::DescriptorType::eUniformBuffer, ashes::ShaderStageFlag::eFragment }
		};
		m_blurYQuad.createPipeline( m_size
			, Position{}
			, program
			, m_intermediate.getTexture()->getDefaultView()
			, *m_renderPass
			, bindings
			, {} );
		m_blurYQuad.prepareFrame( *m_renderPass, 0u );
		return true;
	}
}
