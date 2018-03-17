#include "GaussianBlur.hpp"

#include "Engine.hpp"
#include "Render/RenderPassTimer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/Camera.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"
#include "Texture/TextureUnit.hpp"

#include <Buffer/UniformBuffer.hpp>
#include <Buffer/VertexBuffer.hpp>
#include <Descriptor/DescriptorSet.hpp>
#include <Descriptor/DescriptorSetLayout.hpp>
#include <Descriptor/DescriptorSetPool.hpp>
#include <RenderPass/FrameBuffer.hpp>
#include <RenderPass/RenderPass.hpp>
#include <RenderPass/RenderPassCreateInfo.hpp>
#include <RenderPass/RenderSubpass.hpp>
#include <RenderPass/RenderSubpassState.hpp>
#include <RenderPass/FrameBufferAttachment.hpp>
#include <Shader/ShaderProgram.hpp>

#include <GlslSource.hpp>
#include <GlslUtils.hpp>

#include "Shader/Shaders/GlslLight.hpp"
#include "Shader/Shaders/GlslShadow.hpp"

#include <random>

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	namespace
	{
		glsl::Shader getVertexProgram( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			auto position = writer.declAttribute< Vec2 >( cuT( "position" ), 0u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ), 0u );
			auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
				{
					vtx_texture = position;
					gl_Position = vec4( position.x(), position.y(), 0.0, 1.0 );
				} );
			return writer.finalise();
		}

		glsl::Shader getBlurXProgram( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			Ubo config{ writer, GaussianBlur::Config, 0u, 0u };
			auto c3d_textureSize = config.declMember< Vec2 >( GaussianBlur::TextureSize );
			auto c3d_coefficientsCount = config.declMember< UInt >( GaussianBlur::CoefficientsCount );
			auto c3d_coefficients = config.declMember< Vec4 >( GaussianBlur::Coefficients, GaussianBlur::MaxCoefficients / 4u );
			config.end();
			auto c3d_mapDiffuse = writer.declSampler< Sampler2D >( cuT( "c3d_mapDiffuse" ), 1u, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ), 0u );

			// Shader outputs
			auto pxl_fragColor = writer.declFragData< Vec4 >( cuT( "pxl_fragColor" ), 0u );
			auto gl_FragDepth = writer.declBuiltin< Float >( cuT( "gl_FragDepth" ) );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				auto base = writer.declLocale( cuT( "base" ), vec2( 1.0_f, 0 ) / c3d_textureSize );
				auto offset = writer.declLocale( cuT( "offset" ), vec2( 0.0_f, 0 ) );
				pxl_fragColor = texture( c3d_mapDiffuse, vtx_texture ) * c3d_coefficients[0];

				FOR( writer, UInt, i, 1u, cuT( "i < c3d_coefficientsCount" ), cuT( "++i" ) )
				{
					offset += base;
					pxl_fragColor += c3d_coefficients[i] * texture( c3d_mapDiffuse, vtx_texture - offset );
					pxl_fragColor += c3d_coefficients[i] * texture( c3d_mapDiffuse, vtx_texture + offset );
				}
				ROF;

				gl_FragDepth = pxl_fragColor.r();
			} );
			return writer.finalise();
		}

		glsl::Shader getBlurYProgram( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			Ubo config{ writer, GaussianBlur::Config, 0u, 0u };
			auto c3d_textureSize = config.declMember< Vec2 >( GaussianBlur::TextureSize );
			auto c3d_coefficientsCount = config.declMember< UInt >( GaussianBlur::CoefficientsCount );
			auto c3d_coefficients = config.declMember< Vec4 >( GaussianBlur::Coefficients, GaussianBlur::MaxCoefficients / 4u );
			config.end();
			auto c3d_mapDiffuse = writer.declSampler< Sampler2D >( cuT( "c3d_mapDiffuse" ), 1u, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ), 0u );

			// Shader outputs
			auto pxl_fragColor = writer.declFragData< Vec4 >( cuT( "pxl_fragColor" ), 0 );
			auto gl_FragDepth = writer.declBuiltin< Float >( cuT( "gl_FragDepth" ) );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				auto base = writer.declLocale( cuT( "base" ), vec2( 0.0_f, 1 ) / c3d_textureSize );
				auto offset = writer.declLocale( cuT( "offset" ), vec2( 0.0_f, 0 ) );
				pxl_fragColor = texture( c3d_mapDiffuse, vtx_texture ) * c3d_coefficients[0];

				FOR( writer, UInt, i, 1u, cuT( "i < c3d_coefficientsCount" ), cuT( "++i" ) )
				{
					offset += base;
					pxl_fragColor += c3d_coefficients[i] * texture( c3d_mapDiffuse, vtx_texture - offset );
					pxl_fragColor += c3d_coefficients[i] * texture( c3d_mapDiffuse, vtx_texture + offset );
				}
				ROF;

				gl_FragDepth = pxl_fragColor.r();
			} );
			return writer.finalise();
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
				sampler->setMinFilter( renderer::Filter::eNearest );
				sampler->setMagFilter( renderer::Filter::eNearest );
				sampler->setWrapS( renderer::WrapMode::eClampToEdge );
				sampler->setWrapT( renderer::WrapMode::eClampToEdge );
			}

			return sampler;
		}

		TextureUnit doCreateTexture( Engine & engine
			, renderer::Extent2D const & size
			, renderer::Format format
			, String const & name )
		{
			auto & renderSystem = *engine.getRenderSystem();
			renderer::ImageCreateInfo image{};
			image.flags = 0u;
			image.arrayLayers = 1u;
			image.extent.width = size.width;
			image.extent.height = size.height;
			image.extent.depth = 1u;
			image.format = format;
			image.imageType = renderer::TextureType::e2D;
			image.initialLayout = renderer::ImageLayout::eUndefined;
			image.mipLevels = 1u;
			image.samples = renderer::SampleCountFlag::e1;
			image.sharingMode = renderer::SharingMode::eExclusive;
			image.tiling = renderer::ImageTiling::eOptimal;
			image.usage = renderer::ImageUsageFlag::eColourAttachment | renderer::ImageUsageFlag::eSampled;
			auto sampler = doCreateSampler( engine, name );
			auto texture = std::make_shared< TextureLayout >( renderSystem
				, image
				, renderer::MemoryPropertyFlag::eDeviceLocal );
			texture->getDefaultImage().initialiseSource();
			TextureUnit unit{ engine };
			unit.setTexture( texture );
			unit.setSampler( sampler );
			unit.setIndex( MinBufferIndex );
			unit.initialise();
			return unit;
		}

		renderer::RenderPassPtr doCreateRenderPass( renderer::Device const & device
			, renderer::Format format )
		{
			renderer::RenderPassCreateInfo createInfo{};
			createInfo.flags = 0u;

			createInfo.attachments.resize( 1u );
			createInfo.attachments[0].index = 0u;
			createInfo.attachments[0].format = format;
			createInfo.attachments[0].samples = renderer::SampleCountFlag::e1;
			createInfo.attachments[0].loadOp = renderer::AttachmentLoadOp::eClear;
			createInfo.attachments[0].storeOp = renderer::AttachmentStoreOp::eStore;
			createInfo.attachments[0].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
			createInfo.attachments[0].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
			createInfo.attachments[0].initialLayout = renderer::ImageLayout::eUndefined;
			createInfo.attachments[0].finalLayout = renderer::ImageLayout::eShaderReadOnlyOptimal;

			renderer::AttachmentReference colourReference;
			colourReference.attachment = 0u;
			colourReference.layout = renderer::ImageLayout::eColourAttachmentOptimal;

			createInfo.subpasses.resize( 1u );
			createInfo.subpasses[0].flags = 0u;
			createInfo.subpasses[0].colorAttachments = { colourReference };

			createInfo.dependencies.resize( 2u );
			createInfo.dependencies[0].srcSubpass = renderer::ExternalSubpass;
			createInfo.dependencies[0].dstSubpass = 0u;
			createInfo.dependencies[0].srcStageMask = renderer::PipelineStageFlag::eBottomOfPipe;
			createInfo.dependencies[0].dstStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
			createInfo.dependencies[0].srcAccessMask = renderer::AccessFlag::eMemoryRead;
			createInfo.dependencies[0].dstAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
			createInfo.dependencies[0].dependencyFlags = renderer::DependencyFlag::eByRegion;

			createInfo.dependencies[1].srcSubpass = 0u;
			createInfo.dependencies[1].dstSubpass = renderer::ExternalSubpass;
			createInfo.dependencies[1].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
			createInfo.dependencies[1].dstStageMask = renderer::PipelineStageFlag::eBottomOfPipe;
			createInfo.dependencies[1].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
			createInfo.dependencies[1].dstAccessMask = renderer::AccessFlag::eMemoryRead;
			createInfo.dependencies[1].dependencyFlags = renderer::DependencyFlag::eByRegion;

			return device.createRenderPass( createInfo );
		}

		renderer::FrameBufferPtr doCreateFbo( renderer::RenderPass const & renderPass
			, renderer::TextureView const & view
			, renderer::Extent2D const & size )
		{
			renderer::FrameBufferAttachmentArray attaches;
			attaches.emplace_back( *( renderPass.getAttachments().begin() ), view );
			return renderPass.createFrameBuffer( size, std::move( attaches ) );
		}
	}

	//*********************************************************************************************

	GaussianBlur::RenderQuad::RenderQuad( RenderSystem & renderSystem
		, renderer::TextureView const & src
		, renderer::TextureView const & dst
		, renderer::UniformBuffer< Configuration > const & blurUbo
		, renderer::Format format
		, renderer::Extent2D const & size )
		: castor3d::RenderQuad{ renderSystem, false, false }
		, m_srcView{ src }
		, m_dstView{ dst }
		, m_blurUbo{ blurUbo }
	{
	}

	void GaussianBlur::RenderQuad::doFillDescriptorSet( renderer::DescriptorSetLayout & descriptorSetLayout
		, renderer::DescriptorSet & descriptorSet )
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
		, renderer::TextureView const & texture
		, renderer::Extent2D const & textureSize
		, renderer::Format format
		, uint32_t kernelSize )
		: OwnedBy< Engine >{ engine }
		, m_source{ texture }
		, m_size{ textureSize }
		, m_format{ format }
		, m_intermediate{ doCreateTexture( engine, textureSize, format, cuT( "GaussianBlur" ) ) }
		, m_blurUbo{ renderer::makeUniformBuffer< Configuration >( *engine.getRenderSystem()->getCurrentDevice()
			, 1u
			, renderer::BufferTarget::eTransferDst
			, renderer::MemoryPropertyFlag::eHostVisible ) }
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
	{
		REQUIRE( kernelSize < MaxCoefficients );
		m_blurUbo->getData( 0u ).blurCoeffsCount = uint32_t( m_kernel.size() );
		std::memcpy( m_blurUbo->getData( 0u ).blurCoeffs.data()->ptr()
			, m_kernel.data()
			, sizeof( float ) * std::min( size_t( MaxCoefficients ), m_kernel.size() ) );
		auto & device = *engine.getRenderSystem()->getCurrentDevice();
		m_commandBuffer = device.getGraphicsCommandPool().createCommandBuffer();
		doInitialiseBlurXProgram( engine );
		doInitialiseBlurYProgram( engine );

		if ( m_commandBuffer->begin() )
		{
			m_commandBuffer->beginRenderPass( *m_blurXPass
				, *m_blurXFbo
				, { renderer::ClearColorValue{ 0, 0, 0, 0 } }
			, renderer::SubpassContents::eSecondaryCommandBuffers );
			m_commandBuffer->executeCommands( { m_blurXQuad.getCommandBuffer() } );
			m_commandBuffer->endRenderPass();
			m_commandBuffer->beginRenderPass( *m_blurYPass
				, *m_blurYFbo
				, { renderer::ClearColorValue{ 0, 0, 0, 0 } }
			, renderer::SubpassContents::eSecondaryCommandBuffers );
			m_commandBuffer->executeCommands( { m_blurYQuad.getCommandBuffer() } );
			m_commandBuffer->endRenderPass();
			m_commandBuffer->end();
		}
	}

	void GaussianBlur::blur()
	{
		auto & device = *getEngine()->getRenderSystem()->getCurrentDevice();
		device.getGraphicsQueue().submit( *m_commandBuffer, nullptr );
	}

	bool GaussianBlur::doInitialiseBlurXProgram( Engine & engine )
	{
		auto & renderSystem = *engine.getRenderSystem();
		auto & device = *renderSystem.getCurrentDevice();
		auto & cache = engine.getShaderProgramCache();
		auto const vertex = getVertexProgram( engine );
		auto const blurX = getBlurXProgram( engine );

		auto program = cache.getNewProgram( false );
		program.push_back( { device.createShaderModule( renderer::ShaderStageFlag::eVertex ) } );
		program.push_back( { device.createShaderModule( renderer::ShaderStageFlag::eFragment ) } );
		program[0].module->loadShader( vertex.getSource() );
		program[1].module->loadShader( blurX.getSource() );

		m_blurXPass = doCreateRenderPass( device, m_format );
		m_blurXFbo = doCreateFbo( *m_blurXPass, m_intermediate.getTexture()->getDefaultView(), m_size );
		renderer::DescriptorSetLayoutBindingArray bindings
		{
			{ 0u, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eFragment }
		};
		m_blurXQuad.createPipeline( m_size
			, Position{}
			, program
			, m_source
			, *m_blurXPass
			, bindings
			, {} );
		m_blurXQuad.prepareFrame( *m_blurXPass, 0u );
		return true;
	}

	bool GaussianBlur::doInitialiseBlurYProgram( Engine & engine )
	{
		auto & renderSystem = *engine.getRenderSystem();
		auto & device = *renderSystem.getCurrentDevice();
		auto & cache = engine.getShaderProgramCache();
		auto const vertex = getVertexProgram( engine );
		auto const blurY = getBlurYProgram( engine );

		auto program = cache.getNewProgram( false );
		program.push_back( { device.createShaderModule( renderer::ShaderStageFlag::eVertex ) } );
		program.push_back( { device.createShaderModule( renderer::ShaderStageFlag::eFragment ) } );
		program[0].module->loadShader( vertex.getSource() );
		program[1].module->loadShader( blurY.getSource() );

		m_blurYPass = doCreateRenderPass( device, m_format );
		m_blurYFbo = doCreateFbo( *m_blurYPass, m_intermediate.getTexture()->getDefaultView(), m_size );
		renderer::DescriptorSetLayoutBindingArray bindings
		{
			{ 0u, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eFragment }
		};
		m_blurYQuad.createPipeline( m_size
			, Position{}
			, program
			, m_source
			, *m_blurYPass
			, bindings
			, {} );
		m_blurYQuad.prepareFrame( *m_blurYPass, 0u );
		return true;
	}
}
