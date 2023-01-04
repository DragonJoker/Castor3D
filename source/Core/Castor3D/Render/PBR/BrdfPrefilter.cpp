#include "Castor3D/Render/PBR/BrdfPrefilter.hpp"

#include "Castor3D/Engine.hpp"
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

namespace castor3d
{
	BrdfPrefilter::BrdfPrefilter( Engine & engine
		, RenderDevice const & device
		, castor::Size const & size
		, Texture const & dstTexture )
		: m_renderSystem{ *engine.getRenderSystem() }
		, m_device{ device }
		, m_image{ std::make_unique< ashes::Image >( *m_device
			, dstTexture.image
			, dstTexture.imageId.data->info ) }
		, m_view{ dstTexture.targetViewId.data->info
			, dstTexture.targetView
			, m_image.get() }
		, m_commands{ m_device, *m_device.graphicsData(), "BrdfPrefilter" }
	{
		// Initialise the vertex buffer.
		auto queueData = m_device.graphicsData();
		m_vertexBuffer = device.vertexPools->getBuffer< TexturedQuad >( 1u );
		m_vertexBuffer.getData< TexturedQuad >( SubmeshFlag::ePositions ).front() = { { { castor::Point2f{ -1.0, -1.0 }, castor::Point2f{ 0.0, 0.0 } }
			, { castor::Point2f{ -1.0, +1.0 }, castor::Point2f{ 0.0, 1.0 } }
			, { castor::Point2f{ +1.0, -1.0 }, castor::Point2f{ 1.0, 0.0 } }
			, { castor::Point2f{ +1.0, -1.0 }, castor::Point2f{ 1.0, 0.0 } }
			, { castor::Point2f{ -1.0, +1.0 }, castor::Point2f{ 0.0, 1.0 } }
			, { castor::Point2f{ +1.0, +1.0 }, castor::Point2f{ 1.0, 1.0 } } } };
		m_vertexBuffer.directUpload( SubmeshFlag::ePositions 
			, *queueData->queue
			, *queueData->commandPool
			, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT
			, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT );

		// Initialise the vertex layout.
		m_vertexLayout = std::make_unique< ashes::PipelineVertexInputStateCreateInfo >( 0u
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
				dstTexture.getFormat(),
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
			std::move( attaches ),
			std::move( subpasses ),
			std::move( dependencies ),
		};
		m_renderPass = m_device->createRenderPass( "BrdfPrefilter"
			, std::move( createInfo ) );

		// Initialise the frame buffer.
		ashes::ImageViewCRefArray views;
		views.emplace_back( m_view );
		m_frameBuffer = m_renderPass->createFrameBuffer( "BrdfPrefilter"
			, VkExtent2D{ size.getWidth(), size.getHeight() }
			, std::move( views ) );

		// Initialise the pipeline.
		m_pipelineLayout = m_device->createPipelineLayout( "BrdfPrefilter" );
		m_pipeline = m_device->createPipeline( "BrdfPrefilter"
			, ashes::GraphicsPipelineCreateInfo
			{
				0u,
				doCreateProgram(),
				std::move( *m_vertexLayout ),
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

		auto & cmd = *m_commands.commandBuffer;
		cmd.begin();
		cmd.beginDebugBlock( { "Prefiltering BRDF"
			, makeFloatArray( m_renderSystem.getEngine()->getNextRainbowColour() ) } );
		cmd.beginRenderPass( *m_renderPass
			, *m_frameBuffer
			, { transparentBlackClearColor }
			, VK_SUBPASS_CONTENTS_INLINE );
		cmd.bindPipeline( *m_pipeline );
		cmd.bindVertexBuffer( 0u
			, m_vertexBuffer.getBuffer( SubmeshFlag::ePositions )
			, m_vertexBuffer.getOffset( SubmeshFlag::ePositions ) );
		cmd.draw( 6u );
		cmd.endRenderPass();
		cmd.endDebugBlock();
		cmd.end();
	}

	void BrdfPrefilter::render( QueueData const & queueData )
	{
		m_commands.submit( *queueData.queue );
	}

	ashes::PipelineShaderStageCreateInfoArray BrdfPrefilter::doCreateProgram()
	{
		ShaderModule vtx{ VK_SHADER_STAGE_VERTEX_BIT, "BRDFPrefilter" };
		{
			using namespace sdw;
			VertexWriter writer;

			// Inputs
			auto position = writer.declInput< Vec2 >( "position", 0u );
			auto uv = writer.declInput< Vec2 >( "uv", 1u );

			// Outputs
			auto vtx_texture = writer.declOutput< Vec2 >( "vtx_texture", 0u );

			writer.implementMainT< VoidT, VoidT >( [&]( VertexIn in
				, VertexOut out )
				{
					vtx_texture = uv;
					out.vtx.position = vec4( position, 0.0_f, 1.0_f );
				} );
			vtx.shader = std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		ShaderModule pxl{ VK_SHADER_STAGE_FRAGMENT_BIT, "BRDFPrefilter" };
		{
			using namespace sdw;
			FragmentWriter writer;
			shader::BRDFHelpers brdf{ writer };

			// Inputs
			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

			// Outputs
			auto outColour = writer.declOutput< Vec4 >( "outColour", 0u );

			auto integrateBRDF = writer.implementFunction< Vec3 >( "c3d_integrateBRDF"
				, [&]( Float const & NdotV
					, Float const & roughness )
				{
					// Compute spherical view vector: (sin(phi), 0, cos(phi))
					auto V = writer.declLocale( "V"
						, vec3( sqrt( 1.0_f - NdotV * NdotV ), 0.0_f, NdotV ) );

					// The macro surface normal just points up.
					auto N = writer.declLocale( "N"
						, vec3( 0.0_f, 0.0_f, 1.0_f ) );

					auto A = writer.declLocale( "A"
						, 0.0_f );
					auto B = writer.declLocale( "B"
						, 0.0_f );
					auto C = writer.declLocale( "C"
						, 0.0_f );

					auto sampleCount = writer.declLocale( "sampleCount"
						, 1024_u );

					FOR( writer, UInt, i, 0_u, i < sampleCount, ++i )
					{
						auto xi = writer.declLocale( "xi"
							, brdf.hammersley( i, sampleCount ) );

						// GGX
						{
							auto importanceSample = writer.declLocale( "importanceSample"
								, brdf.getImportanceSample( brdf.importanceSampleGGX( xi, roughness ), N ) );
							auto H = writer.declLocale( "H"
								,importanceSample.xyz() );
							auto L = writer.declLocale( "L"
								, normalize( vec3( 2.0_f ) * dot( V, H ) * H - V ) );

							auto NdotL = writer.declLocale( "NdotL"
								, max( L.z(), 0.0_f ) );
							auto NdotH = writer.declLocale( "NdotH"
								, max( H.z(), 0.0_f ) );
							auto VdotH = writer.declLocale( "VdotH"
								, max( dot( V, H ), 0.0_f ) );

							IF( writer, NdotL > 0.0_f )
							{
								auto G = writer.declLocale( "G"
									, brdf.visibilitySmithGGXCorrelated( NdotV, max( dot( N, L ), 0.0_f ), roughness ) );
								auto vis = writer.declLocale( "G_Vis"
									, ( G * VdotH ) / ( NdotH * NdotV ) );
								auto Fc = writer.declLocale( "Fc"
									, pow( 1.0_f - VdotH, 5.0_f ) );

								A += ( 1.0_f - Fc ) * vis;
								B += Fc * vis;
							}
							FI;
						}

						// Charlie
						auto importanceSample = writer.declLocale( "importanceSample"
							, brdf.getImportanceSample( brdf.importanceSampleCharlie( xi, roughness ), N ) );
						auto H = writer.declLocale( "H"
							, importanceSample.xyz() );
						auto L = writer.declLocale( "L"
							, normalize( vec3( 2.0_f ) * dot( V, H ) * H - V ) );

						auto NdotL = writer.declLocale( "NdotL"
							, max( L.z(), 0.0_f ) );
						auto NdotH = writer.declLocale( "NdotH"
							, max( H.z(), 0.0_f ) );
						auto VdotH = writer.declLocale( "VdotH"
							, max( dot( V, H ), 0.0_f ) );

						IF( writer, NdotL > 0.0_f )
						{
							auto sheenDistribution = writer.declLocale( "G"
								, brdf.distributionCharlie( roughness, NdotH ) );
							auto sheenVisibility = writer.declLocale( "G"
								, brdf.visibilityAshikhmin( NdotL, NdotV ) );
							C += sheenVisibility * sheenDistribution * NdotL * VdotH;
						}
						FI;
					}
					ROF;

					A /= writer.cast< Float >( sampleCount );
					B /= writer.cast< Float >( sampleCount );
					C /= writer.cast< Float >( sampleCount );
					writer.returnStmt( vec3( A, B, C ) );
				}
				, InFloat( writer, "NdotV" )
				, InFloat( writer, "roughness" ) );

			writer.implementMainT< VoidT, VoidT >( [&]( FragmentIn in
				, FragmentOut out )
				{
					outColour = vec4( integrateBRDF( vtx_texture.x(), vtx_texture.y() ), 1.0_f );
				} );

			pxl.shader = std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		return ashes::PipelineShaderStageCreateInfoArray
		{
			makeShaderState( m_device, vtx ),
			makeShaderState( m_device, pxl ),
		};
	}
}
