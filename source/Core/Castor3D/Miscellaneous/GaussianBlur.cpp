#include "Castor3D/Miscellaneous/GaussianBlur.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/UniformBuffer.hpp"
#include "Castor3D/Miscellaneous/DebugName.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Shader/GlslToSpv.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslShadow.hpp"
#include "Castor3D/Texture/Sampler.hpp"
#include "Castor3D/Texture/TextureLayout.hpp"
#include "Castor3D/Texture/TextureUnit.hpp"

#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Descriptor/DescriptorSetPool.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>

#include <random>

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	namespace
	{
		static uint32_t constexpr GaussCfgIdx = 0u;
		static uint32_t constexpr DifImgIdx = 1u;

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
			Ubo config{ writer, GaussianBlur::Config, GaussCfgIdx, 0u };
			auto c3d_textureSize = config.declMember< Vec2 >( GaussianBlur::TextureSize );
			auto c3d_coefficientsCount = config.declMember< UInt >( GaussianBlur::CoefficientsCount );
			auto c3d_dump = config.declMember< UInt >( cuT( "c3d_dump" ) ); // to keep a 16 byte alignment.
			auto c3d_coefficients = config.declMember< Vec4 >( GaussianBlur::Coefficients, GaussianBlur::MaxCoefficients / 4 );
			config.end();
			auto c3d_mapDiffuse = writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapDiffuse" ), DifImgIdx, 0u );
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
			Ubo config{ writer, GaussianBlur::Config, GaussCfgIdx, 0u };
			auto c3d_textureSize = config.declMember< Vec2 >( GaussianBlur::TextureSize );
			auto c3d_coefficientsCount = config.declMember< UInt >( GaussianBlur::CoefficientsCount );
			auto c3d_dump = config.declMember< UInt >( cuT( "c3d_dump" ) ); // to keep a 16 byte alignment.
			auto c3d_coefficients = config.declMember< Vec4 >( GaussianBlur::Coefficients, GaussianBlur::MaxCoefficients / 4 );
			config.end();
			auto c3d_mapDiffuse = writer.declSampledImage< FImg2DArrayRgba32 >( cuT( "c3d_mapDiffuse" ), DifImgIdx, 0u );
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
			Ubo config{ writer, GaussianBlur::Config, GaussCfgIdx, 0u };
			auto c3d_textureSize = config.declMember< Vec2 >( GaussianBlur::TextureSize );
			auto c3d_coefficientsCount = config.declMember< UInt >( GaussianBlur::CoefficientsCount );
			auto c3d_dump = config.declMember< UInt >( cuT( "c3d_dump" ) ); // to keep a 16 byte alignment.
			auto c3d_coefficients = config.declMember< Vec4 >( GaussianBlur::Coefficients, GaussianBlur::MaxCoefficients / 4 );
			config.end();
			auto c3d_mapDiffuse = writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapDiffuse" ), DifImgIdx, 0u );
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
				sampler->setMinFilter( VK_FILTER_NEAREST );
				sampler->setMagFilter( VK_FILTER_NEAREST );
				sampler->setWrapS( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
				sampler->setWrapT( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
			}

			return sampler;
		}

		TextureUnit doCreateTexture( Engine & engine
			, VkExtent2D const & size
			, VkFormat format
			, String const & name )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto sampler = doCreateSampler( engine, name );
			ashes::ImageCreateInfo image
			{
				0u,
				VK_IMAGE_TYPE_2D,
				format,
				{ size.width, size.height, 1u },
				1u,
				1u,
				VK_SAMPLE_COUNT_1_BIT,
				VK_IMAGE_TILING_OPTIMAL,
				VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			};
			auto texture = std::make_shared< TextureLayout >( renderSystem
				, std::move( image )
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, name );
			texture->getDefaultImage().initialiseSource();
			TextureUnit unit{ engine };
			unit.setTexture( texture );
			unit.setSampler( sampler );
			unit.initialise();
			return unit;
		}

		ashes::RenderPassPtr doCreateRenderPass( RenderDevice const & device
			, VkFormat format )
		{
			ashes::VkAttachmentDescriptionArray attaches
			{
				{
					0u,
					format,
					VK_SAMPLE_COUNT_1_BIT,
					VK_ATTACHMENT_LOAD_OP_CLEAR,
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
					std::nullopt,
					{},
				} );
			ashes::VkSubpassDependencyArray dependencies
			{
				{
					VK_SUBPASS_EXTERNAL,
					0u,
					VK_PIPELINE_STAGE_HOST_BIT,
					VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					VK_ACCESS_HOST_WRITE_BIT,
					VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
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
				}
			};
			ashes::RenderPassCreateInfo createInfo
			{
				0u,
				std::move( attaches ),
				std::move( subpasses ),
				std::move( dependencies ),
			};
			auto result = device->createRenderPass( std::move( createInfo ) );
			setDebugObjectName( device, *result, "GaussianBlurRenderPass" );
			return result;
		}

		ashes::FrameBufferPtr doCreateFbo( ashes::RenderPass const & renderPass
			, ashes::ImageView const & view
			, VkExtent2D const & size )
		{
			ashes::ImageViewCRefArray attaches;
			attaches.emplace_back( view );
			return renderPass.createFrameBuffer( size, std::move( attaches ) );
		}
	}

	//*********************************************************************************************

	GaussianBlur::RenderQuad::RenderQuad( RenderSystem & renderSystem
		, ashes::ImageView const & src
		, ashes::ImageView const & dst
		, ashes::UniformBuffer< Configuration > const & blurUbo
		, VkFormat format
		, VkExtent2D const & size )
		: castor3d::RenderQuad{ getCurrentRenderDevice( renderSystem ), false, false }
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
		, ashes::ImageView const & texture
		, VkExtent2D const & textureSize
		, VkFormat format
		, uint32_t kernelSize )
		: OwnedBy< Engine >{ engine }
		, m_source{ texture }
		, m_size{ textureSize }
		, m_format{ format }
		, m_intermediate{ doCreateTexture( engine, textureSize, format, cuT( "GaussianBlur" ) ) }
		, m_renderPass{ doCreateRenderPass( getCurrentRenderDevice( engine ), m_format ) }
		, m_blurUbo{ makeUniformBuffer< Configuration >( getCurrentRenderDevice( engine )
			, 1u
			, VK_BUFFER_USAGE_TRANSFER_DST_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT 
			, "GaussianBlurCfg" ) }
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
		, m_blurXVertexShader{ VK_SHADER_STAGE_VERTEX_BIT, "GaussianBlurX" }
		, m_blurXPixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, "GaussianBlurX" }
		, m_blurYVertexShader{ VK_SHADER_STAGE_VERTEX_BIT, "GaussianBlurY" }
		, m_blurYPixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, "GaussianBlurY" }
	{
		auto & renderSystem = *engine.getRenderSystem();
		auto & device = *renderSystem.getCurrentRenderDevice();
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
		m_horizCommandBuffer = device.graphicsCommandPool->createCommandBuffer();
		m_verticCommandBuffer = device.graphicsCommandPool->createCommandBuffer();
		m_horizSemaphore = device->createSemaphore();
		m_verticSemaphore = device->createSemaphore();
		doInitialiseBlurXProgram();
		doInitialiseBlurYProgram();

		m_horizCommandBuffer->begin();
		m_horizCommandBuffer->beginRenderPass( *m_renderPass
			, *m_blurXFbo
			, { ashes::makeClearValue( VkClearColorValue{ 0, 0, 0, 0 } ) }
		, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS );
		m_horizCommandBuffer->executeCommands( { m_blurXQuad.getCommandBuffer() } );
		m_horizCommandBuffer->endRenderPass();
		m_horizCommandBuffer->end();

		m_verticCommandBuffer->begin();
		m_verticCommandBuffer->beginRenderPass( *m_renderPass
			, *m_blurYFbo
			, { ashes::makeClearValue( VkClearColorValue{ 0, 0, 0, 0 } ) }
		, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS );
		m_verticCommandBuffer->executeCommands( { m_blurYQuad.getCommandBuffer() } );
		m_verticCommandBuffer->endRenderPass();
		m_verticCommandBuffer->end();
	}

	ashes::Semaphore const & GaussianBlur::blur( ashes::Semaphore const & toWait )
	{
		auto * result = &toWait;
		auto & renderSystem = *getEngine()->getRenderSystem();
		auto & device = *renderSystem.getCurrentRenderDevice();

		device.graphicsQueue->submit( *m_horizCommandBuffer
			, *result
			, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
			, *m_horizSemaphore
			, nullptr );
		result = m_horizSemaphore.get();

		device.graphicsQueue->submit( *m_verticCommandBuffer
			, *result
			, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
			, *m_verticSemaphore
			, nullptr );
		result = m_verticSemaphore.get();

		return *result;
	}

	bool GaussianBlur::doInitialiseBlurXProgram()
	{
		auto & renderSystem = *getEngine()->getRenderSystem();
		auto & device = *renderSystem.getCurrentRenderDevice();
		m_blurXVertexShader.shader = getVertexProgram( renderSystem );

		if ( m_source->subresourceRange.layerCount > 1u
			/*&& !device.getRenderer().getFeatures().hasImageTexture*/ )
		{
			m_blurXPixelShader.shader = getBlurXProgramLayer( *getEngine()->getRenderSystem()
				, ashes::isDepthFormat( m_format )
				, m_source->subresourceRange.baseArrayLayer );
		}
		else
		{
			m_blurXPixelShader.shader = getBlurXProgram( *getEngine()->getRenderSystem(), ashes::isDepthFormat( m_format ) );
		}

		ashes::PipelineShaderStageCreateInfoArray program
		{
			makeShaderState( device, m_blurXVertexShader ),
			makeShaderState( device, m_blurXVertexShader ),
		};

		m_blurXFbo = doCreateFbo( *m_renderPass, m_intermediate.getTexture()->getDefaultView(), m_size );
		ashes::VkDescriptorSetLayoutBindingArray bindings
		{
			makeDescriptorSetLayoutBinding( GaussCfgIdx
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, VK_SHADER_STAGE_FRAGMENT_BIT )
		};
		m_blurXQuad.createPipeline( m_size
			, Position{}
			, program
			, m_source
			, *m_renderPass
			, std::move( bindings )
			, {} );
		m_blurXQuad.prepareFrame( *m_renderPass, 0u );
		return true;
	}

	bool GaussianBlur::doInitialiseBlurYProgram()
	{
		auto & renderSystem = *getEngine()->getRenderSystem();
		auto & device = *renderSystem.getCurrentRenderDevice();
		m_blurYVertexShader.shader = getVertexProgram( renderSystem );
		m_blurYPixelShader.shader = getBlurYProgram( renderSystem, ashes::isDepthFormat( m_format ) );

		ashes::PipelineShaderStageCreateInfoArray program
		{
			makeShaderState( device, m_blurXVertexShader ),
			makeShaderState( device, m_blurXVertexShader ),
		};

		m_blurYFbo = doCreateFbo( *m_renderPass, m_source, m_size );
		ashes::VkDescriptorSetLayoutBindingArray bindings
		{
			makeDescriptorSetLayoutBinding( GaussCfgIdx
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, VK_SHADER_STAGE_FRAGMENT_BIT )
		};
		m_blurYQuad.createPipeline( m_size
			, Position{}
			, program
			, m_intermediate.getTexture()->getDefaultView()
			, *m_renderPass
			, std::move( bindings )
			, {} );
		m_blurYQuad.prepareFrame( *m_renderPass, 0u );
		return true;
	}
}
