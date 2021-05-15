#include "Castor3D/Render/Passes/GaussianBlur.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
#include "Castor3D/Cache/SamplerCache.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Miscellaneous/DebugName.hpp"
#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"
#include "Castor3D/Shader/Program.hpp"

#include <CastorUtils/Graphics/RgbaColour.hpp>

#include <ashespp/Image/Image.hpp>
#include <ashespp/Sync/Semaphore.hpp>

#include <ShaderWriter/Source.hpp>

CU_ImplementCUSmartPtr( castor3d, GaussianBlur )

using namespace castor;

namespace castor3d
{
	namespace
	{
		enum Idx
		{
			GaussCfgIdx,
			DifImgIdx,
		};

		ShaderPtr getVertexProgram( RenderSystem & renderSystem )
		{
			using namespace sdw;
			VertexWriter writer;

			// Shader inputs
			auto position = writer.declInput< Vec2 >( "position", 0u );
			auto uv = writer.declInput< Vec2 >( "uv", 1u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( "vtx_texture", 0u );
			auto out = writer.getOut();

			writer.implementFunction< Void >( "main"
				, [&]()
				{
					vtx_texture = uv;
					out.vtx.position = vec4( position, 0.0_f, 1.0_f );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		ShaderPtr getBlurXProgram( RenderSystem & renderSystem, bool isDepth )
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			Ubo config{ writer, GaussianBlur::Config, GaussCfgIdx, 0u };
			auto c3d_textureSize = config.declMember< Vec2 >( GaussianBlur::TextureSize );
			auto c3d_coefficientsCount = config.declMember< UInt >( GaussianBlur::CoefficientsCount );
			auto c3d_dump = config.declMember< UInt >( "c3d_dump" ); // to keep a 16 byte alignment.
			auto c3d_coefficients = config.declMember< Vec4 >( GaussianBlur::Coefficients, GaussianBlur::MaxCoefficients / 4u );
			config.end();
			auto c3d_mapSource = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapSource", DifImgIdx, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( "pxl_fragColor", 0u );
			auto out = writer.getOut();

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					auto base = writer.declLocale( "base", vec2( 1.0_f, 0.0_f ) / c3d_textureSize );
					auto offset = writer.declLocale( "offset", vec2( 0.0_f, 0.0_f ) );
					pxl_fragColor = c3d_mapSource.sample( vtx_texture ) * c3d_coefficients[0_u][0_u];

					FOR( writer, UInt, i, 1_u, i < c3d_coefficientsCount, ++i )
					{
						offset += base;
						pxl_fragColor += c3d_coefficients[i / 4_u][i % 4_u] * c3d_mapSource.sample( vtx_texture - offset );
						pxl_fragColor += c3d_coefficients[i / 4_u][i % 4_u] * c3d_mapSource.sample( vtx_texture + offset );
					}
					ROF;

					if ( isDepth )
					{
						out.fragDepth = pxl_fragColor.r();
					}
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
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
			auto c3d_dump = config.declMember< UInt >( "c3d_dump" ); // to keep a 16 byte alignment.
			auto c3d_coefficients = config.declMember< Vec4 >( GaussianBlur::Coefficients, GaussianBlur::MaxCoefficients / 4u );
			config.end();
			auto c3d_mapSource = writer.declSampledImage< FImg2DArrayRgba32 >( "c3d_mapSource", DifImgIdx, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( "pxl_fragColor", 0u );
			auto out = writer.getOut();

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					auto base = writer.declLocale( "base", vec2( 1.0_f, 0.0_f ) / c3d_textureSize );
					auto offset = writer.declLocale( "offset", vec2( 0.0_f, 0.0_f ) );
					pxl_fragColor = c3d_mapSource.sample( vec3( vtx_texture, Float( float( layer ) ) ) ) * c3d_coefficients[0_u][0_u];

					FOR( writer, UInt, i, 1_u, i < c3d_coefficientsCount, ++i )
					{
						offset += base;
						pxl_fragColor += c3d_coefficients[i / 4_u][i % 4_u] * c3d_mapSource.sample( vec3( vtx_texture - offset, Float( float( layer ) ) ) );
						pxl_fragColor += c3d_coefficients[i / 4_u][i % 4_u] * c3d_mapSource.sample( vec3( vtx_texture + offset, Float( float( layer ) ) ) );
					}
					ROF;

					if ( isDepth )
					{
						out.fragDepth = pxl_fragColor.r();
					}
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
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
			auto c3d_mapSource = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapSource", DifImgIdx, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( "pxl_fragColor", 0 );
			auto out = writer.getOut();

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					auto base = writer.declLocale( "base", vec2( 0.0_f, 1.0_f ) / c3d_textureSize );
					auto offset = writer.declLocale( "offset", vec2( 0.0_f, 0.0_f ) );
					pxl_fragColor = c3d_mapSource.sample( vtx_texture ) * c3d_coefficients[0_u][0_u];

					FOR( writer, UInt, i, 1_u, i < c3d_coefficientsCount, ++i )
					{
						offset += base;
						pxl_fragColor += c3d_coefficients[i / 4_u][i % 4_u] * c3d_mapSource.sample( vtx_texture - offset );
						pxl_fragColor += c3d_coefficients[i / 4_u][i % 4_u] * c3d_mapSource.sample( vtx_texture + offset );
					}
					ROF;

					if ( isDepth )
					{
						out.fragDepth = pxl_fragColor.r();
					}
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
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
			, RenderDevice const & device
			, VkExtent2D const & size
			, VkFormat format
			, VkImageSubresourceRange const & range
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
				range.levelCount,
				1u,
				VK_SAMPLE_COUNT_1_BIT,
				VK_IMAGE_TILING_OPTIMAL,
				( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
					| VK_IMAGE_USAGE_SAMPLED_BIT
					| ( range.levelCount > 1u
						? ( VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT )
						: 0u ) ),
			};
			auto texture = std::make_shared< TextureLayout >( renderSystem
				, std::move( image )
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, name );
			TextureUnit unit{ engine };
			unit.setTexture( texture );
			unit.setSampler( sampler );
			unit.initialise( device );
			return unit;
		}

		ashes::RenderPassPtr createRenderPass( RenderDevice const & device
			, castor::String const & name
			, VkFormat format
			, uint32_t subpassCount )
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
			auto result = device->createRenderPass( name
				, std::move( createInfo ) );
			return result;
		}

		VkImageViewType getNonArrayType( VkImageViewType in )
		{
			switch ( in )
			{
			case VK_IMAGE_VIEW_TYPE_CUBE:
				return VK_IMAGE_VIEW_TYPE_2D;
			case VK_IMAGE_VIEW_TYPE_1D_ARRAY:
				return VK_IMAGE_VIEW_TYPE_1D;
			case VK_IMAGE_VIEW_TYPE_2D_ARRAY:
				return VK_IMAGE_VIEW_TYPE_2D;
			case VK_IMAGE_VIEW_TYPE_CUBE_ARRAY:
				return VK_IMAGE_VIEW_TYPE_2D;
			default:
				return in;
			}
		}

		std::vector< ashes::FrameBufferPtrArray > createFbos( std::string const & prefix
			, ashes::RenderPass const & renderPass
			, ashes::ImageView const & view
			, ashes::ImageViewArray & views
			, VkExtent2D const & size )
		{
			std::vector< ashes::FrameBufferPtrArray > result;
			auto createInfo = view.createInfo;
			createInfo.subresourceRange.layerCount = 1u;
			createInfo.subresourceRange.levelCount = 1u;
			createInfo.viewType = getNonArrayType( createInfo.viewType );

			for ( uint32_t layerOff = 0u; layerOff < view->subresourceRange.layerCount; ++layerOff )
			{
				auto layerName = prefix + ", Layer " + string::toString( view->subresourceRange.baseArrayLayer );
				ashes::FrameBufferPtrArray layerFbos;
				VkExtent2D mipSize{ size };
				createInfo.subresourceRange.baseMipLevel = view->subresourceRange.baseMipLevel;

				for ( uint32_t levelOff = 0u; levelOff < view->subresourceRange.levelCount; ++levelOff )
				{
					auto name = layerName + ", Level " + string::toString( view->subresourceRange.baseMipLevel );
					ashes::ImageViewCRefArray attaches;
					views.emplace_back( view.image->createView( name, createInfo ) );
					attaches.emplace_back( views.back() );
					layerFbos.emplace_back( renderPass.createFrameBuffer( name
						, mipSize
						, std::move( attaches ) ) );
					++createInfo.subresourceRange.baseMipLevel;
					mipSize.width >>= 1u;
					mipSize.height >>= 1u;
				}

				result.emplace_back( std::move( layerFbos ) );
				++createInfo.subresourceRange.baseArrayLayer;
			}

			return result;
		}

		ashes::ImageView createImageView( castor::String const & name
			, ashes::ImageView const & view
			, VkImageSubresourceRange const & range
			, VkImageAspectFlags aspectMask )
		{
			auto createInfo = view.createInfo;
			createInfo.subresourceRange.aspectMask = aspectMask;
			createInfo.subresourceRange.baseMipLevel = range.baseMipLevel;
			createInfo.subresourceRange.levelCount = range.levelCount;
			auto result = view.image->createView( name
				, std::move( createInfo ) );
			return result;
		}

		ashes::ImageView createImageView( castor::String const & name
			, ashes::ImageView const & view
			, VkImageSubresourceRange const & range )
		{
			return createImageView( name
				, view
				, range
				, view->subresourceRange.aspectMask );
		}

		ashes::ImageView createSampledView( castor::String const & prefix
			, ashes::ImageView const & view )
		{
			return createImageView( prefix + "Sampled"
				, view
				, view->subresourceRange
				, VkImageAspectFlags( ( ashes::isDepthFormat( view->format ) || ashes::isDepthOrStencilFormat( view->format ) )
					? VK_IMAGE_ASPECT_DEPTH_BIT
					: ( ashes::isStencilFormat( view->format )
						? VK_IMAGE_ASPECT_STENCIL_BIT
						: VK_IMAGE_ASPECT_COLOR_BIT ) ) );
		}

		ashes::ImageView createTargetView( castor::String const & prefix
			, ashes::ImageView const & view )
		{
			return createImageView( prefix + "Target"
				, view
				, view->subresourceRange
				, VkImageAspectFlags( ( ashes::isDepthFormat( view->format ) || ashes::isDepthOrStencilFormat( view->format ) )
					? VK_IMAGE_ASPECT_DEPTH_BIT
					: ( ashes::isStencilFormat( view->format )
						? VK_IMAGE_ASPECT_STENCIL_BIT
						: VK_IMAGE_ASPECT_COLOR_BIT ) ) );
		}

		CommandsSemaphore doGetCommands( RenderDevice const & device
			, castor::String prefix
			, GaussianBlur::BlurPass const & blurX
			, GaussianBlur::BlurPass const & blurY
			, ashes::RenderPass const & renderPass
			, TextureView const & source
			, RenderPassTimer const * timer
			, bool generateMipmaps
			, uint32_t layer )
		{
			prefix += "GaussianBlur, Layer " + string::toString( layer );
			castor3d::CommandsSemaphore commands
			{
				device.graphicsCommandPool->createCommandBuffer( prefix ),
				device->createSemaphore( prefix )
			};
			auto & cmd = *commands.commandBuffer;
			cmd.begin();

			if ( timer )
			{
				timer->beginPass( cmd, layer );
			}

			blurX.getCommands( cmd, renderPass, layer, generateMipmaps );
			blurY.getCommands( cmd, renderPass, layer, generateMipmaps );

			if ( generateMipmaps )
			{
				cmd.beginDebugBlock(
					{
						prefix + " Mipmaps Generation",
						makeFloatArray( device.renderSystem.getEngine()->getNextRainbowColour() ),
					} );
				source.getSampledView().image->generateMipmaps( cmd
					, layer
					, 1u
					, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
					, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
					, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
				cmd.endDebugBlock();
			}

			if ( timer )
			{
				timer->endPass( cmd, layer );
			}

			cmd.end();
			return commands;
		}

		ashes::ImageView createMipView( castor::String name
			, ashes::ImageView const & source
			, uint32_t layer
			, uint32_t level )
		{
			auto createInfo = source.createInfo;
			createInfo.subresourceRange.baseArrayLayer = layer;
			createInfo.subresourceRange.baseMipLevel = level;
			createInfo.subresourceRange.layerCount = 1u;
			createInfo.subresourceRange.levelCount = 1u;

			if ( createInfo.viewType == VK_IMAGE_VIEW_TYPE_1D_ARRAY )
			{
				createInfo.viewType = VK_IMAGE_VIEW_TYPE_1D;
			}
			else if ( createInfo.viewType == VK_IMAGE_VIEW_TYPE_2D_ARRAY
				|| createInfo.viewType == VK_IMAGE_VIEW_TYPE_CUBE_ARRAY
				|| createInfo.viewType == VK_IMAGE_VIEW_TYPE_3D )
			{
				createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			}

			name += ", Layer " + string::toString( layer );
			name += ", Level " + string::toString( level );
			return source.image->createView( name, std::move( createInfo ) );
		}

		uint32_t getDescriptorSetIndex( uint32_t descriptorBaseIndex
			, uint32_t level
			, uint32_t levelCount )
		{
			return ( descriptorBaseIndex * levelCount ) + level;
		}

		rq::BindingDescriptionArray createBindings()
		{
			return
			{
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, std::nullopt },
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, std::nullopt },
			};
		}
	}

	//*********************************************************************************************

	GaussianBlur::RenderQuad::RenderQuad( RenderSystem & renderSystem
		, RenderDevice const & device
		, castor::String const & name
		, ashes::ImageView const & src
		, VkImageSubresourceRange const & srcRange
		, UniformBufferOffsetT< Configuration > const & blurUbo
		, ShaderModule const & vertexShader
		, ShaderModule const & pixelShader
		, ashes::RenderPass const & renderPass
		, VkExtent2D const & textureSize )
		: castor3d::RenderQuad{ device
			, name
			, VK_FILTER_LINEAR
			, { createBindings()
				, ashes::nullopt
				, rq::Texcoord{} } }
		, srcView{ createImageView( name, src, srcRange ) }
		, m_blurUbo{ blurUbo }
	{
		m_sampler->initialise( device );

		for ( auto layer = srcView->subresourceRange.baseArrayLayer; layer < srcView->subresourceRange.layerCount; ++layer )
		{
			for ( auto level = srcView->subresourceRange.baseMipLevel; level < srcView->subresourceRange.levelCount; ++level )
			{
				registerPassInputs( { makeDescriptorWrite( m_blurUbo, GaussCfgIdx )
					, makeDescriptorWrite( createMipView( name, srcView, layer, level ), m_sampler->getSampler(), DifImgIdx ) } );
			}
		}

		ashes::PipelineShaderStageCreateInfoArray program
		{
			makeShaderState( device, vertexShader ),
			makeShaderState( device, pixelShader ),
		};
		createPipeline( textureSize
			, Position{}
			, program
			, renderPass );
		initialisePasses();
	}

	//*********************************************************************************************

	GaussianBlur::BlurPass::BlurPass( Engine & engine
		, RenderDevice const & device
		, castor::String const & name
		, ashes::ImageView const & input
		, ashes::ImageView const & output
		, UniformBufferOffsetT< GaussianBlur::Configuration > const & blurUbo
		, VkFormat format
		, VkExtent2D const & textureSize
		, ashes::RenderPass const & renderPass
		, bool isHorizontal )
		: vertexShader
		{
			VK_SHADER_STAGE_VERTEX_BIT,
			name,
			getVertexProgram( *engine.getRenderSystem() ),
		}
		, pixelShader
		{
			VK_SHADER_STAGE_FRAGMENT_BIT,
			name,
			getPixelProgram( *engine.getRenderSystem()
				, input->subresourceRange.baseArrayLayer
				, input->subresourceRange.layerCount
				, isHorizontal
				, ashes::isDepthFormat( format ) ),
		}
		, quad{ *engine.getRenderSystem()
			, device
			, name
			, input
			, input->subresourceRange
			, blurUbo
			, vertexShader
			, pixelShader
			, renderPass
			, textureSize }
		, semaphore{ device->createSemaphore( name ) }
		, fbos{ createFbos( name, renderPass, output, views, textureSize ) }
		, m_engine{ engine }
		, isHorizontal{ isHorizontal }
	{
	}

	void GaussianBlur::BlurPass::getCommands( ashes::CommandBuffer & cmd
		, ashes::RenderPass const & renderPass
		, uint32_t layer
		, bool generateMipmaps )const
	{
		auto prefix = vertexShader.name
			+ ", Layer " + string::toString( layer )
			+ ", Level ";
		uint32_t targetLayer = isHorizontal ? 0u : layer;
		uint32_t sampledLayer = isHorizontal ? layer : 0u;
		uint32_t level = 0u;
		auto processLevel = [this, &cmd, &prefix, &renderPass, sampledLayer]( ashes::FrameBuffer const & levelFbo
			, uint32_t level )
		{
			cmd.beginDebugBlock(
				{
					prefix + string::toString( level ),
					makeFloatArray( m_engine.getNextRainbowColour() ),
				} );
			cmd.beginRenderPass( renderPass
				, levelFbo
				, { transparentBlackClearColor }
			, VK_SUBPASS_CONTENTS_INLINE );
			quad.registerPass( cmd
				, getDescriptorSetIndex( sampledLayer
					, level
					, quad.srcView->subresourceRange.levelCount ) );
			cmd.endRenderPass();
			cmd.endDebugBlock();
		};
		processLevel( *fbos[targetLayer][level], level );

		if ( !generateMipmaps )
		{
			while ( ++level < fbos[targetLayer].size() )
			{
				processLevel( *fbos[targetLayer][level], level );
			}
		}
	}

	//*********************************************************************************************
	
	String const GaussianBlur::Config = cuT( "Config" );
	String const GaussianBlur::Coefficients = cuT( "c3d_coefficients" );
	String const GaussianBlur::CoefficientsCount = cuT( "c3d_coefficientsCount" );
	String const GaussianBlur::TextureSize = cuT( "c3d_textureSize" );

	GaussianBlur::GaussianBlur( Engine & engine
		, RenderDevice const & device
		, castor::String const & prefix
		, TextureView const & view
		, uint32_t kernelSize )
		: OwnedBy< Engine >{ engine }
		, m_device{ device }
		, m_prefix{ prefix }
		, m_source{ view }
		, m_size{ m_source.getOwner()->getWidth(), m_source.getOwner()->getHeight() }
		, m_format{ m_source.getOwner()->getPixelFormat() }
		, m_intermediate{ createTexture( engine
			, m_device
			, m_size
			, m_format
			, m_source.getSubresourceRange()
			, m_prefix + cuT( "GaussianBlur" ) ) }
		, m_renderPass{ createRenderPass( m_device
			, m_prefix + cuT( "GaussianBlur" )
			, m_format
			, m_source.getSubresourceRange().levelCount ) }
		, m_blurUbo{ m_device.uboPools->getBuffer< Configuration >( 0u ) }
		, m_blurX{ engine
			, m_device
			, m_prefix + cuT( " - GaussianBlur - X Pass" )
			, m_source.getSampledView()
			, m_intermediate.getTexture()->getDefaultView().getSampledView()
			, m_blurUbo
			, m_format
			, m_size
			, *m_renderPass
			, true }
		, m_blurY{ engine
			, m_device
			, m_prefix + cuT( " - GaussianBlur - Y Pass" )
			, m_intermediate.getTexture()->getDefaultView().getSampledView()
			, m_source.getSampledView()
			, m_blurUbo
			, m_format
			, m_size
			, *m_renderPass
			, false }
		, m_kernel{ getHalfPascal( kernelSize ) }
	{
		CU_Require( kernelSize < MaxCoefficients );
		auto & data = m_blurUbo.getData();
		data.blurCoeffsCount = uint32_t( m_kernel.size() );
		data.dump = 0u;
		std::memcpy( data.blurCoeffs.data()->ptr()
			, m_kernel.data()
			, sizeof( float ) * std::min( size_t( MaxCoefficients ), m_kernel.size() ) );
		data.textureSize[0] = float( m_size.width );
		data.textureSize[1] = float( m_size.height );
	}

	GaussianBlur::~GaussianBlur()
	{
		getEngine()->getSamplerCache().remove( m_prefix + cuT( "GaussianBlur" ) );
	}

	void GaussianBlur::accept( PipelineVisitorBase & visitor )
	{
		if ( m_intermediate.isTextured() )
		{
			visitor.visit( m_prefix + " GaussianBlur Intermediate"
				, m_intermediate.getTexture()->getDefaultView().getSampledView()
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				, TextureFactors{}.invert( true ) );
		}

		visitor.visit( m_blurX.vertexShader );
		visitor.visit( m_blurX.pixelShader );

		visitor.visit( m_blurY.vertexShader );
		visitor.visit( m_blurY.pixelShader );
	}

	CommandsSemaphore GaussianBlur::getCommands( bool generateMipmaps
		, uint32_t layer )const
	{
		return doGetCommands( m_device
			, m_prefix
			, m_blurX
			, m_blurY
			, *m_renderPass
			, m_source
			, nullptr
			, generateMipmaps
			, layer );
	}

	CommandsSemaphore GaussianBlur::getCommands( RenderPassTimer const & timer
		, uint32_t layer
		, bool generateMipmaps )const
	{
		return doGetCommands( m_device
			, m_prefix
			, m_blurX
			, m_blurY
			, *m_renderPass
			, m_source
			, &timer
			, generateMipmaps
			, layer );
	}

	ashes::Semaphore const & GaussianBlur::blur( ashes::Semaphore const & toWait )
	{
		auto * result = &toWait;
		m_device.graphicsQueue->submit( *m_blurX.commandBuffer
			, *result
			, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
			, *m_blurX.semaphore
			, nullptr );
		result = m_blurX.semaphore.get();

		m_device.graphicsQueue->submit( *m_blurY.commandBuffer
			, *result
			, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
			, *m_blurY.semaphore
			, nullptr );
		result = m_blurY.semaphore.get();

		return *result;
	}
}
