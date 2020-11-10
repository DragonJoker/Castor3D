#include "Castor3D/Render/PBR/RadianceComputer.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/SamplerCache.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Miscellaneous/DebugName.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <CastorUtils/Graphics/Size.hpp>
#include <CastorUtils/Math/Angle.hpp>

#include <ashespp/Image/Image.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>
#include <ashespp/Sync/Fence.hpp>

#include <ShaderWriter/Source.hpp>

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		ashes::ImagePtr doCreateRadianceTexture( RenderDevice const & device
			, Size const & size )
		{
			ashes::ImageCreateInfo image
			{
				VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT,
				VK_IMAGE_TYPE_2D,
				VK_FORMAT_R32G32B32A32_SFLOAT,
				{ size[0], size[1], 1u },
				1u,
				6u,
				VK_SAMPLE_COUNT_1_BIT,
				VK_IMAGE_TILING_OPTIMAL,
				VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			};
			auto result = makeImage( device
				, std::move( image )
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, "RadianceComputerResult" );
			return result;
		}

		SamplerSPtr doCreateSampler( Engine & engine
			, RenderDevice const & device )
		{
			SamplerSPtr result;
			auto name = cuT( "IblTexturesRadiance" );

			if ( engine.getSamplerCache().has( name ) )
			{
				result = engine.getSamplerCache().find( name );
			}
			else
			{
				result = engine.getSamplerCache().create( name );
				result->setMinFilter( VK_FILTER_LINEAR );
				result->setMagFilter( VK_FILTER_LINEAR );
				result->setWrapS( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
				result->setWrapT( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
				result->setWrapR( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
				engine.getSamplerCache().add( name, result );
			}

			result->initialise( device );
			return result;
		}

		ashes::ImageView doCreateSrcView( ashes::Image const & texture )
		{
			return texture.createView( VK_IMAGE_VIEW_TYPE_CUBE
				, texture.getFormat()
				, 0u
				, 1u
				, 0u
				, 6u );
		}

		ashes::PipelineShaderStageCreateInfoArray doCreateProgram( RenderDevice const & device )
		{
			ShaderModule vtx{ VK_SHADER_STAGE_VERTEX_BIT, "RadianceCompute" };
			{
				using namespace sdw;
				VertexWriter writer;

				// Inputs
				auto position = writer.declInput< Vec3 >( "position", 0u );
				Ubo matrix{ writer, "Matrix", 0u, 0u };
				auto c3d_viewProjection = matrix.declMember< Mat4 >( "c3d_viewProjection" );
				matrix.end();

				// Outputs
				auto vtx_worldPosition = writer.declOutput< Vec3 >( "vtx_worldPosition", 0u );
				auto out = writer.getOut();

				writer.implementFunction< Void >( "main"
					, [&]()
					{
						vtx_worldPosition = position;
						out.vtx.position = ( c3d_viewProjection * vec4( position, 1.0_f ) ).xyww();
					} );
				vtx.shader = std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
			}

			ShaderModule pxl{ VK_SHADER_STAGE_FRAGMENT_BIT, "RadianceCompute" };
			{
				using namespace sdw;
				FragmentWriter writer;

				// Inputs
				auto vtx_worldPosition = writer.declInput< Vec3 >( "vtx_worldPosition", 0u );
				auto c3d_mapEnvironment = writer.declSampledImage< FImgCubeRgba32 >( "c3d_mapEnvironment", 1u, 0u );

				// Outputs
				auto pxl_fragColor = writer.declOutput< Vec4 >( "pxl_FragColor", 0u );

				writer.implementFunction< Void >( "main"
					, [&]()
					{
						// From https://learnopengl.com/#!PBR/Lighting
						// the sample direction equals the hemisphere's orientation 
						auto normal = writer.declLocale( "normal"
							, normalize( vtx_worldPosition ) );

						auto irradiance = writer.declLocale( "irradiance"
							, vec3( 0.0_f ) );

						auto up = writer.declLocale( "up"
							, vec3( 0.0_f, 1.0_f, 0.0_f ) );
						auto right = writer.declLocale( "right"
							, cross( up, normal ) );
						up = cross( normal, right );

						auto sampleDelta = writer.declLocale( "sampleDelta"
							, 0.025_f );
						auto nrSamples = writer.declLocale( "nrSamples"
							, 0_i );

						FOR( writer, Float, phi, 0.0_f, phi < Float{ PiMult2< float > }, phi += sampleDelta )
						{
							FOR( writer, Float, theta, 0.0_f, theta < Float{ PiDiv2< float > }, theta += sampleDelta )
							{
								// spherical to cartesian (in tangent space)
								auto tangentSample = writer.declLocale( "tangentSample"
									, vec3( sin( theta ) * cos( phi ), sin( theta ) * sin( phi ), cos( theta ) ) );
								// tangent space to world
								auto sampleVec = writer.declLocale( "sampleVec"
									, right * tangentSample.x() + up * tangentSample.y() + normal * tangentSample.z() );

								irradiance += c3d_mapEnvironment.sample( sampleVec ).rgb() * cos( theta ) * sin( theta );
								nrSamples = nrSamples + 1;
							}
							ROF;
						}
						ROF;

						irradiance = irradiance * Float{ Pi< float > } * ( 1.0_f / writer.cast< Float >( nrSamples ) );
						pxl_fragColor = vec4( irradiance, 1.0_f );
					} );

				pxl.shader = std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
			}

			return ashes::PipelineShaderStageCreateInfoArray
			{
				makeShaderState( device, vtx ),
				makeShaderState( device, pxl ),
			};
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
				std::move( attaches ),
				std::move( subpasses ),
				std::move( dependencies ),
			};
			auto result = device->createRenderPass( "RadianceComputer"
				, std::move( createInfo ) );
			return result;
		}
	}

	//*********************************************************************************************

	RadianceComputer::RadianceComputer( Engine & engine
		, RenderDevice const & device
		, Size const & size
		, ashes::Image const & srcTexture )
		: RenderCube{ device, false }
		, m_result{ doCreateRadianceTexture( m_device, size ) }
		, m_resultView{ m_result->createView( "RadianceComputerResult", VK_IMAGE_VIEW_TYPE_CUBE, m_result->getFormat(), 0u, m_result->getMipmapLevels(), 0u, 6u ) }
		, m_sampler{ doCreateSampler( engine, m_device ) }
		, m_srcView{ doCreateSrcView( srcTexture ) }
		, m_renderPass{ doCreateRenderPass( m_device, m_result->getFormat() ) }
	{
		auto & dstTexture = *m_result;
		
		for ( auto face = 0u; face < 6u; ++face )
		{
			auto & facePass = m_renderPasses[face];
			auto name = "RadianceComputer" + string::toString( face );

			// Create the views.
			facePass.dstView = dstTexture.createView( name
				, VK_IMAGE_VIEW_TYPE_2D
				, dstTexture.getFormat()
				, 0u
				, 1u
				, face
				, 1u );
			// Initialise the frame buffer.
			ashes::ImageViewCRefArray attaches;
			attaches.emplace_back( facePass.dstView );
			facePass.frameBuffer = m_renderPass->createFrameBuffer( name
				, VkExtent2D{ size.getWidth(), size.getHeight() }
				, std::move( attaches ) );
		}

		auto program = doCreateProgram( m_device );
		createPipelines( { size.getWidth(), size.getHeight() }
			, Position{}
			, program
			, m_srcView
			, *m_renderPass
			, {} );

		m_commandBuffer = m_device.graphicsCommandPool->createCommandBuffer( "RadianceComputer" );
		m_commandBuffer->begin();
		m_commandBuffer->beginDebugBlock(
			{
				"Generating irradiance map",
				makeFloatArray( m_device.renderSystem.getEngine()->getNextRainbowColour() ),
			} );

		for ( auto face = 0u; face < 6u; ++face )
		{
			auto & facePass = m_renderPasses[face];
			m_commandBuffer->beginRenderPass( *m_renderPass
				, *facePass.frameBuffer
				, { transparentBlackClearColor }
				, VK_SUBPASS_CONTENTS_INLINE );
			registerFrame( *m_commandBuffer, face );
			m_commandBuffer->endRenderPass();
		}

		m_commandBuffer->endDebugBlock();
		m_commandBuffer->end();
	}

	void RadianceComputer::render()
	{
		m_device.graphicsQueue->submit( *m_commandBuffer, nullptr );
		m_device.graphicsQueue->waitIdle();
	}

	ashes::Sampler const & RadianceComputer::getSampler()const
	{
		return m_sampler->getSampler();
	}

	//*********************************************************************************************
}
