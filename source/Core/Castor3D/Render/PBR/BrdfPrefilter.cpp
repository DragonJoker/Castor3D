#include "Castor3D/Render/PBR/BrdfPrefilter.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/DirectUploadData.hpp"
#include "Castor3D/Buffer/InstantUploadData.hpp"
#include "Castor3D/Buffer/ObjectBufferPool.hpp"
#include "Castor3D/Miscellaneous/DebugName.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslBRDFHelpers.hpp"

#include <CastorUtils/Math/Angle.hpp>
#include <CastorUtils/Graphics/Size.hpp>

#include <ashespp/Buffer/VertexBuffer.hpp>
#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/Pipeline/PipelineDepthStencilStateCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineInputAssemblyStateCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineVertexInputStateCreateInfo.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>
#include <ashespp/Sync/Fence.hpp>

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/TraditionalGraphicsWriter.hpp>

namespace c3d
{
	BrdfPrefilter::BrdfPrefilter( Engine const & engine
		, RenderDevice const & device
		, Size const & size
		, Texture const & dstTexture )
		: m_renderSystem{ *engine.getRenderSystem() }
		, m_device{ device }
		, m_image{ makeRawUnique< ashes::Image >( *m_device
			, *dstTexture.image
			, ashes::ImageCreateInfo{ convert( dstTexture.imageId.data->info ) } ) }
		, m_view{ convert( dstTexture.getTargetViewId().data->info )
			, dstTexture.getTargetView()
			, m_image.get() }
		, m_commands{ m_device, *m_device.graphicsData(), cuT( "BrdfPrefilter" ) }
	{
		// Initialise the vertex buffer.
		auto queueData = m_device.graphicsData();
		m_vertexBuffer = device.vertexPools->getBuffer< TexturedQuad >( 1u );
		auto const & vb = m_vertexBuffer.getBufferChunk( SubmeshData::ePositions );
		TexturedQuad data{ { TexturedQuad::Vertex{ Point2f{ -1.0, -1.0 }, Point2f{ 0.0, 0.0 } }
			, TexturedQuad::Vertex{ Point2f{ -1.0, +1.0 }, Point2f{ 0.0, 1.0 } }
			, TexturedQuad::Vertex{ Point2f{ +1.0, -1.0 }, Point2f{ 1.0, 0.0 } }
			, TexturedQuad::Vertex{ Point2f{ +1.0, -1.0 }, Point2f{ 1.0, 0.0 } }
			, TexturedQuad::Vertex{ Point2f{ -1.0, +1.0 }, Point2f{ 0.0, 1.0 } }
			, TexturedQuad::Vertex{ Point2f{ +1.0, +1.0 }, Point2f{ 1.0, 1.0 } } } };
		{
			InstantDirectUploadData uploader{ *m_device.transferQueue
				, m_device, cuT( "BrdfPrefilterVBUpload" ), *queueData->commandPool };
			uploader->pushUpload( &data, sizeof( TexturedQuad )
				, vb.getBuffer(), vb.getOffset()
				, VertexAttributeInputState );
		}

		// Initialise the vertex layout.
		m_vertexLayout = makeRawUnique< ashes::PipelineVertexInputStateCreateInfo >( 0u
			, ashes::VkVertexInputBindingDescriptionArray
			{
				{ 0u, sizeof( TexturedQuad::Vertex ), VK_VERTEX_INPUT_RATE_VERTEX },
			}
			, ashes::VkVertexInputAttributeDescriptionArray
			{
				{ 0u, 0u, VK_FORMAT_R32G32_SFLOAT, offsetof( TexturedQuad::Vertex, position ) },
				{ 1u, 0u, VK_FORMAT_R32G32_SFLOAT, offsetof( TexturedQuad::Vertex, texture ) },
			} );

		// Create the render pass.
		ashes::VkAttachmentDescriptionArray attaches
		{
			{
				0u,
				convert( dstTexture.getFormat() ),
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
			}
		};
		ashes::RenderPassCreateInfo createInfo
		{
			0u,
			c3d::move( attaches ),
			c3d::move( subpasses ),
			c3d::move( dependencies ),
		};
		m_renderPass = m_device->createRenderPass( "BrdfPrefilter"
			, c3d::move( createInfo ) );

		// Initialise the frame buffer.
		ashes::ImageViewCRefArray views;
		views.emplace_back( m_view );
		m_frameBuffer = m_renderPass->createFrameBuffer( "BrdfPrefilter"
			, VkExtent2D{ size.getWidth(), size.getHeight() }
			, c3d::move( views ) );

		// Initialise the pipeline.
		m_pipelineLayout = m_device->createPipelineLayout( "BrdfPrefilter" );
		m_pipeline = m_device->createPipeline( "BrdfPrefilter"
			, ashes::GraphicsPipelineCreateInfo
			{
				0u,
				doCreateProgram(),
				c3d::move( *m_vertexLayout ),
				ashes::PipelineInputAssemblyStateCreateInfo{ 0u, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST },
				ashes::nullopt,
				ashes::PipelineViewportStateCreateInfo{ 0u, 1u, { VkViewport{ 0.0f, 0.0f, float( size.getWidth() ), float( size.getHeight() ), 0.0f, 1.0f } }, 1u, { VkRect2D{ 0, 0, size.getWidth(), size.getHeight() } } },
				ashes::PipelineRasterizationStateCreateInfo{},
				ashes::PipelineMultisampleStateCreateInfo{},
				ashes::PipelineDepthStencilStateCreateInfo{ 0u, false, false },
				ashes::PipelineColorBlendStateCreateInfo{},
				ashes::nullopt,
				*m_pipelineLayout,
				*m_renderPass
			} );

		auto const & cmd = *m_commands.commandBuffer;
		cmd.begin();
		cmd.beginDebugBlock( { "Prefiltering BRDF"
			, makeFloatArray( m_renderSystem.getEngine()->getNextRainbowColour() ) } );
		cmd.beginRenderPass( *m_renderPass
			, *m_frameBuffer
			, { convert( ClearValue{ transparentBlackClearColor } ) }
			, VK_SUBPASS_CONTENTS_INLINE );
		cmd.bindPipeline( *m_pipeline );
		cmd.bindVertexBuffer( 0u
			, m_vertexBuffer.getBuffer( SubmeshData::ePositions ).getBuffer()
			, m_vertexBuffer.getOffset( SubmeshData::ePositions ) );
		cmd.draw( 6u );
		cmd.endRenderPass();
		cmd.endDebugBlock();
		cmd.end();
	}

	void BrdfPrefilter::render( QueueData const & queueData )const
	{
		m_commands.submit( *queueData.queue );
	}

	ashes::PipelineShaderStageCreateInfoArray BrdfPrefilter::doCreateProgram()
	{
		ProgramModule programModule{ cuT( "BRDFPrefilter" ) };
		{
			sdw::TraditionalGraphicsWriter writer{ &m_renderSystem.getEngine()->getShaderAllocator() };

			// Inputs
			auto inPosition = writer.declInput< sdw::Vec2 >( "inPosition", sdw::EntryPoint::eVertex, 0u );
			auto inUv = writer.declInput< sdw::Vec2 >( "inUv", sdw::EntryPoint::eVertex, 1u );
			auto inTexture = writer.declInput< sdw::Vec2 >( "vtx_texture", sdw::EntryPoint::eFragment, 0u );

			// Outputs
			auto outTexture = writer.declOutput< sdw::Vec2 >( "outTexture", sdw::EntryPoint::eVertex, 0u );
			auto outColour = writer.declOutput< sdw::Vec4 >( "outColour", sdw::EntryPoint::eFragment, 0u );

			shader::BRDFHelpers brdf{ writer };

			auto visibilitySmithGGXCorrelated = writer.implementFunction< sdw::Float >( "visibilitySmithGGXCorrelated"
				, [&writer]( sdw::Float const & NdotV
					, sdw::Float const & NdotL
					, sdw::Float const & roughness )
				{
					auto a2 = writer.declLocale( "a2"
						, pow( roughness, 4.0_f ) );
					auto ggxV = writer.declLocale( "ggxV"
						, NdotL * sqrt( NdotV * NdotV * ( 1.0_f - a2 ) + a2 ) );
					auto ggxL = writer.declLocale( "ggxL"
						, NdotV * sqrt( NdotL * NdotL * ( 1.0_f - a2 ) + a2 ) );
					writer.returnStmt( 0.5_f / ( ggxV + ggxL ) );
				}
				, sdw::InFloat( writer, "NdotV" )
				, sdw::InFloat( writer, "NdotL" )
				, sdw::InFloat( writer, "roughness" ) );

			auto integrateBRDF = writer.implementFunction< sdw::Vec3 >( "c3d_integrateBRDF"
				, [&]( sdw::Float const & NdotV
					, sdw::Float const & roughness )
				{
					// Compute spherical view vector: (sin(phi), 0, cos(phi))
					auto V = writer.declLocale( "V"
						, vec3( sqrt( 1.0_f - NdotV * NdotV ), 0.0_f, NdotV ) );

					// The macro surface normal just points up.
					auto N = writer.declLocale( "N"
						, vec3( 0.0_f, 0.0_f, 1.0_f ) );

					// To make the LUT independant from the material's F0, which is part of the Fresnel term
					// when substituted by Schlick's approximation, we factor it out of the integral,
					// yielding to the form: F0 * I1 + I2
					// I1 and I2 are slighlty different in the Fresnel term, but both only depend on
					// NoL and roughness, so they are both numerically integrated and written into two channels.
					auto A = writer.declLocale( "A"
						, 0.0_f );
					auto B = writer.declLocale( "B"
						, 0.0_f );
					auto C = writer.declLocale( "C"
						, 0.0_f );

					auto sampleCount = writer.declLocale( "sampleCount"
						, 1024_u );

					sdwFOR( writer, sdw::UInt, i, 0_u, i < sampleCount, ++i )
					{
						auto xi = writer.declLocale( "xi"
							, brdf.hammersley( i, sampleCount ) );

						// GGX
						{
							auto importanceSample = writer.declLocale( "importanceSample"
								, brdf.getImportanceSample( brdf.importanceSampleGGX( xi, roughness ), N ) );
							auto H = writer.declLocale( "H"
								, importanceSample.xyz() );
							auto L = writer.declLocale( "L"
								, normalize( reflect( -V, H ) ) );

							auto NdotL = writer.declLocale( "NdotL"
								, clamp( L.z(), 0.0_f, 1.0_f ) );
							auto NdotH = writer.declLocale( "NdotH"
								, clamp( H.z(), 0.0_f, 1.0_f ) );
							auto VdotH = writer.declLocale( "VdotH"
								, clamp( dot( V, H ), 0.0_f, 1.0_f ) );

							sdwIF( writer, NdotL > 0.0_f )
							{
								auto G = writer.declLocale( "G"
									, visibilitySmithGGXCorrelated( NdotV, NdotL, roughness ) );
								auto vis = writer.declLocale( "G_Vis"
									, ( G * VdotH * NdotL ) / NdotH );
								auto Fc = writer.declLocale( "Fc"
									, pow( 1.0_f - VdotH, 5.0_f ) );

								A += ( 1.0_f - Fc ) * vis;
								B += Fc * vis;
							}
							sdwFI
						}
						// Charlie
						{
							auto importanceSample = writer.declLocale( "importanceSample"
								, brdf.getImportanceSample( brdf.importanceSampleCharlie( xi, roughness ), N ) );
							auto H = writer.declLocale( "H"
								, importanceSample.xyz() );
							auto L = writer.declLocale( "L"
								, normalize( reflect( -V, H ) ) );

							auto NdotL = writer.declLocale( "NdotL"
								, clamp( L.z(), 0.0_f, 1.0_f ) );
							auto NdotH = writer.declLocale( "NdotH"
								, clamp( H.z(), 0.0_f, 1.0_f ) );
							auto VdotH = writer.declLocale( "VdotH"
								, clamp( dot( V, H ), 0.0_f, 1.0_f ) );

							sdwIF( writer, NdotL > 0.0_f )
							{
								auto sheenDistribution = writer.declLocale( "G"
									, brdf.distributionCharlie( roughness, NdotH ) );
								auto sheenVisibility = writer.declLocale( "G"
									, brdf.visibilityAshikhmin( NdotL, NdotV ) );
								C += sheenVisibility * sheenDistribution * NdotL * VdotH;
							}
							sdwFI
						}
					}
					sdwROF

					writer.returnStmt( vec3( 4.0_f * A / writer.cast< sdw::Float >( sampleCount )
						, 4.0_f * B / writer.cast< sdw::Float >( sampleCount )
						, 4.0_f * 2.0_f * Pi< float > * C / writer.cast< sdw::Float >( sampleCount ) ) );
				}
				, sdw::InFloat( writer, "NdotV" )
				, sdw::InFloat( writer, "roughness" ) );

			writer.implementEntryPointT< sdw::VoidT, sdw::VoidT >( [&]( sdw::VertexIn const &
				, sdw::VertexOut out )
				{
					outTexture = inUv;
					out.vtx.position = vec4( inPosition, 0.0_f, 1.0_f );
				} );

			writer.implementEntryPointT< sdw::VoidT, sdw::VoidT >( [&]( sdw::FragmentIn const &
				, sdw::FragmentOut const & )
				{
					outColour = vec4( integrateBRDF( inTexture.x(), inTexture.y() ), 1.0_f );
				} );

			programModule.shader = writer.getBuilder().releaseShader();
		}

		return makeProgramStates( m_device, programModule );
	}
}
