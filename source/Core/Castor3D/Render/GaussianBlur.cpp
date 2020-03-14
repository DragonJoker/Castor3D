#include "Castor3D/Render/GaussianBlur.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/SamplerCache.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Shader/Program.hpp"

#include <ashespp/Sync/Semaphore.hpp>

#include <ShaderWriter/Source.hpp>

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
			auto c3d_coefficients = config.declMember< Vec4 >( GaussianBlur::Coefficients, GaussianBlur::MaxCoefficients / 4u );
			config.end();
			auto c3d_mapSource = writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapSource" ), DifImgIdx, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ), 0u );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( cuT( "pxl_fragColor" ), 0u );
			auto out = writer.getOut();

			writer.implementFunction< sdw::Void >( cuT( "main" )
				, [&]()
				{
					auto base = writer.declLocale( cuT( "base" ), vec2( 1.0_f, 0.0_f ) / c3d_textureSize );
					auto offset = writer.declLocale( cuT( "offset" ), vec2( 0.0_f, 0.0_f ) );
					pxl_fragColor = texture( c3d_mapSource, vtx_texture ) * c3d_coefficients[0_u][0_u];

					FOR( writer, UInt, i, 1_u, i < c3d_coefficientsCount, ++i )
					{
						offset += base;
						pxl_fragColor += c3d_coefficients[i / 4_u][i % 4_u] * texture( c3d_mapSource, vtx_texture - offset );
						pxl_fragColor += c3d_coefficients[i / 4_u][i % 4_u] * texture( c3d_mapSource, vtx_texture + offset );
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
			auto c3d_coefficients = config.declMember< Vec4 >( GaussianBlur::Coefficients, GaussianBlur::MaxCoefficients / 4u );
			config.end();
			auto c3d_mapSource = writer.declSampledImage< FImg2DArrayRgba32 >( cuT( "c3d_mapSource" ), DifImgIdx, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ), 0u );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( cuT( "pxl_fragColor" ), 0u );
			auto out = writer.getOut();

			writer.implementFunction< sdw::Void >( cuT( "main" ), [&]()
			{
				auto base = writer.declLocale( cuT( "base" ), vec2( 1.0_f, 0.0_f ) / c3d_textureSize );
				auto offset = writer.declLocale( cuT( "offset" ), vec2( 0.0_f, 0.0_f ) );
				pxl_fragColor = texture( c3d_mapSource, vec3( vtx_texture, Float( float( layer ) ) ) ) * c3d_coefficients[0_u][0_u];

				FOR( writer, UInt, i, 1_u, i < c3d_coefficientsCount, ++i )
				{
					offset += base;
					pxl_fragColor += c3d_coefficients[i / 4_u][i % 4_u] * texture( c3d_mapSource, vec3( vtx_texture - offset, Float( float( layer ) ) ) );
					pxl_fragColor += c3d_coefficients[i / 4_u][i % 4_u] * texture( c3d_mapSource, vec3( vtx_texture + offset, Float( float( layer ) ) ) );
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
			auto c3d_coefficients = config.declMember< Vec4 >( GaussianBlur::Coefficients, GaussianBlur::MaxCoefficients / 4u );
			config.end();
			auto c3d_mapSource = writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapSource" ), DifImgIdx, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ), 0u );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( cuT( "pxl_fragColor" ), 0 );
			auto out = writer.getOut();

			writer.implementFunction< sdw::Void >( cuT( "main" ), [&]()
			{
				auto base = writer.declLocale( cuT( "base" ), vec2( 0.0_f, 1.0_f ) / c3d_textureSize );
				auto offset = writer.declLocale( cuT( "offset" ), vec2( 0.0_f, 0.0_f ) );
				pxl_fragColor = texture( c3d_mapSource, vtx_texture ) * c3d_coefficients[0_u][0_u];

				FOR( writer, UInt, i, 1_u, i < c3d_coefficientsCount, ++i )
				{
					offset += base;
					pxl_fragColor += c3d_coefficients[i / 4_u][i % 4_u] * texture( c3d_mapSource, vtx_texture - offset );
					pxl_fragColor += c3d_coefficients[i / 4_u][i % 4_u] * texture( c3d_mapSource, vtx_texture + offset );
				}
				ROF;

				if ( isDepth )
				{
					out.gl_FragDepth = pxl_fragColor.r();
				}
			} );
			return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}

		ShaderPtr getPixelProgram( RenderSystem & renderSystem
			, uint32_t baseArrayLayer
			, uint32_t layerCount
			, bool isHorizontal
			, bool isDepth )
		{
			if ( isHorizontal )
			{
				if ( layerCount > 1u
					&& !renderSystem.getDescription().features.hasImageTexture )
				{
					return getBlurXProgramLayer( renderSystem
						, isDepth
						, baseArrayLayer );
				}

				return getBlurXProgram( renderSystem
					, isDepth );
			}

			return getBlurYProgram( renderSystem, isDepth );
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

		SamplerSPtr createSampler( Engine & engine
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

		TextureUnit createTexture( Engine & engine
			, VkExtent2D const & size
			, VkFormat format
			, String const & name )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto sampler = createSampler( engine, name );
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

		ashes::RenderPassPtr createRenderPass( RenderDevice const & device
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
					ashes::nullopt,
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

		ashes::FrameBufferPtr createFbo( ashes::RenderPass const & renderPass
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
		, UniformBuffer< Configuration > const & blurUbo
		, VkFormat format
		, VkExtent2D const & size )
		: castor3d::RenderQuad{ renderSystem, VK_FILTER_LINEAR, TexcoordConfig{} }
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

	castor::String Names[2]
	{
		"GaussianBlurY",
		"GaussianBlurX"
	};

	GaussianBlur::BlurPass::BlurPass( Engine & engine
		, ashes::ImageView const & input
		, ashes::ImageView const & output
		, UniformBuffer< GaussianBlur::Configuration > const & blurUbo
		, VkFormat format
		, VkExtent2D const & textureSize
		, ashes::RenderPass const & renderPass
		, bool isHorizontal )
		: quad
		{
			*engine.getRenderSystem(),
			input,
			output,
			blurUbo,
			format,
			textureSize,
		}
		, vertexShader
		{
			VK_SHADER_STAGE_VERTEX_BIT,
			( isHorizontal ? Names[1] : Names[0] ),
			getVertexProgram( *engine.getRenderSystem() ),
		}
		, pixelShader
		{
			VK_SHADER_STAGE_FRAGMENT_BIT,
			( isHorizontal ? Names[1] : Names[0] ),
			getPixelProgram( *engine.getRenderSystem()
				, input->subresourceRange.baseArrayLayer
				, input->subresourceRange.layerCount
				, isHorizontal
				, ashes::isDepthFormat( format ) ),
		}
		, semaphore{ getCurrentRenderDevice( engine )->createSemaphore() }
		, fbo{ createFbo( renderPass, output, textureSize ) }
	{
		auto & device = getCurrentRenderDevice( engine );

		ashes::PipelineShaderStageCreateInfoArray program
		{
			makeShaderState( device, vertexShader ),
			makeShaderState( device, pixelShader ),
		};

		ashes::VkDescriptorSetLayoutBindingArray bindings
		{
			makeDescriptorSetLayoutBinding( GaussCfgIdx
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, VK_SHADER_STAGE_FRAGMENT_BIT )
		};

		quad.createPipeline( textureSize
			, Position{}
			, program
			, input
			, renderPass
			, std::move( bindings )
			, {} );
		quad.prepareFrame( renderPass, 0u );
		commandBuffer = doGenerateCommandBuffer( device, renderPass );
	}
	
	ashes::CommandBufferPtr GaussianBlur::BlurPass::doGenerateCommandBuffer( RenderDevice const & device
		, ashes::RenderPass const & renderPass )
	{
		auto result = device.graphicsCommandPool->createCommandBuffer();
		result->begin();
		result->beginRenderPass( renderPass
			, *fbo
			, { transparentBlackClearColor }
			, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS );
		result->executeCommands( { quad.getCommandBuffer() } );
		result->endRenderPass();
		result->end();
		return result;
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
		, m_intermediate{ createTexture( engine, textureSize, format, cuT( "GaussianBlur" ) ) }
		, m_renderPass{ createRenderPass( getCurrentRenderDevice( engine ), m_format ) }
		, m_blurUbo{ makeUniformBuffer< Configuration >( *engine.getRenderSystem()
			, 1u
			, VK_BUFFER_USAGE_TRANSFER_DST_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT 
			, "GaussianBlurCfg" ) }
		, m_blurX
		{
			engine,
			texture,
			m_intermediate.getTexture()->getDefaultView(),
			*m_blurUbo,
			format,
			textureSize,
			*m_renderPass,
			true,
		}
		, m_blurY
		{
			engine,
			m_intermediate.getTexture()->getDefaultView(),
			texture,
			*m_blurUbo,
			format,
			textureSize,
			*m_renderPass,
			false,
		}
		, m_kernel{ getHalfPascal( kernelSize ) }
	{
		auto & device = getCurrentRenderDevice( engine );
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
	}

	ashes::Semaphore const & GaussianBlur::blur( ashes::Semaphore const & toWait )
	{
		auto * result = &toWait;
		auto & device = getCurrentRenderDevice( *this );

		device.graphicsQueue->submit( *m_blurX.commandBuffer
			, *result
			, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
			, *m_blurX.semaphore
			, nullptr );
		result = m_blurX.semaphore.get();

		device.graphicsQueue->submit( *m_blurY.commandBuffer
			, *result
			, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
			, *m_blurY.semaphore
			, nullptr );
		result = m_blurY.semaphore.get();

		return *result;
	}
}
