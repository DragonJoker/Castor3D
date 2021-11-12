#include "Castor3D/Render/PBR/BrdfPrefilter.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Miscellaneous/DebugName.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Shader/Program.hpp"

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

using namespace castor;

namespace castor3d
{
	BrdfPrefilter::BrdfPrefilter( Engine & engine
		, RenderDevice const & device
		, castor::Size const & size
		, ashes::ImageView const & dstTexture )
		: m_renderSystem{ *engine.getRenderSystem() }
		, m_device{ device }
		, m_commands{ m_device, *m_device.graphicsData(), "BrdfPrefilter" }
	{
		// Initialise the vertex buffer.
		m_vertexBuffer = makeVertexBuffer< TexturedQuad >( m_device
			, 1u
			, 0u
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, "BrdfPrefilter" );

		if ( auto buffer = m_vertexBuffer->lock( 0u, 1u, 0u ) )
		{
			*buffer =
			{
				{
					{ Point2f{ -1.0, -1.0 }, Point2f{ 0.0, 0.0 } },
					{ Point2f{ -1.0, +1.0 }, Point2f{ 0.0, 1.0 } },
					{ Point2f{ +1.0, -1.0 }, Point2f{ 1.0, 0.0 } },
					{ Point2f{ +1.0, -1.0 }, Point2f{ 1.0, 0.0 } },
					{ Point2f{ -1.0, +1.0 }, Point2f{ 0.0, 1.0 } },
					{ Point2f{ +1.0, +1.0 }, Point2f{ 1.0, 1.0 } },
				}
			};
			m_vertexBuffer->flush( 0u, 1u );
			m_vertexBuffer->unlock();
		}

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
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
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
		views.emplace_back( dstTexture );
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
				ashes::PipelineDynamicStateCreateInfo{ 0u, { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR } },
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
		cmd.bindVertexBuffer( 0u, m_vertexBuffer->getBuffer(), 0u );
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

			// Inputs
			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

			// Outputs
			auto pxl_fragColor = writer.declOutput< Vec2 >( "pxl_FragColor", 0u );

			auto radicalInverse = writer.implementFunction< Float >( "RadicalInverse_VdC"
				, [&]( UInt const & inBits )
				{
					auto bits = writer.declLocale( "bits"
						, inBits );
					bits = ( bits << 16u ) | ( bits >> 16u );
					bits = ( ( bits & 0x55555555_u ) << 1u ) | ( ( bits & 0xAAAAAAAA_u ) >> 1u );
					bits = ( ( bits & 0x33333333_u ) << 2u ) | ( ( bits & 0xCCCCCCCC_u ) >> 2u );
					bits = ( ( bits & 0x0F0F0F0F_u ) << 4u ) | ( ( bits & 0xF0F0F0F0_u ) >> 4u );
					bits = ( ( bits & 0x00FF00FF_u ) << 8u ) | ( ( bits & 0xFF00FF00_u ) >> 8u );
					writer.returnStmt( writer.cast< Float >( bits ) * 2.3283064365386963e-10_f ); // / 0x100000000
				}
				, InUInt{ writer, "inBits" } );

			auto hammersley = writer.implementFunction< Vec2 >( "Hammersley"
				, [&]( UInt const & i
					, UInt const & n )
				{
					writer.returnStmt( vec2( writer.cast< Float >( i ) / writer.cast< Float >( n ), radicalInverse( i ) ) );
				}
				, InUInt{ writer, "i" }
				, InUInt{ writer, "n" } );

			auto importanceSample = writer.implementFunction< Vec3 >( "ImportanceSampleGGX"
				, [&]( Vec2 const & xi
					, Vec3 const & n
					, Float const & roughness )
				{
					// From https://learnopengl.com/#!PBR/Lighting
					auto a = writer.declLocale( "a"
						, roughness * roughness );

					auto phi = writer.declLocale( "phi"
						, PiMult2< float > * xi.x() );
					auto cosTheta = writer.declLocale( "cosTheta"
						, sqrt( ( 1.0_f - xi.y() ) / ( 1.0_f + ( a * a - 1.0_f ) * xi.y() ) ) );
					auto sinTheta = writer.declLocale( "sinTheta"
						, sqrt( 1.0 - cosTheta * cosTheta ) );

					// from spherical coordinates to cartesian coordinates
					auto H = writer.declLocale< Vec3 >( "H" );
					H.x() = cos( phi ) * sinTheta;
					H.y() = sin( phi ) * sinTheta;
					H.z() = cosTheta;

					// from tangent-space vector to world-space sample vector
					auto up = writer.declLocale( "up"
						, writer.ternary( sdw::abs( n.z() ) < 0.999_f
							, vec3( 0.0_f, 0.0_f, 1.0_f )
							, vec3( 1.0_f, 0.0_f, 0.0_f ) ) );
					auto tangent = writer.declLocale( "tangent"
						, normalize( cross( up, n ) ) );
					auto bitangent = writer.declLocale( "bitangent"
						, cross( n, tangent ) );

					auto sampleVec = writer.declLocale( "sampleVec"
						, tangent * H.x() + bitangent * H.y() + n * H.z() );
					writer.returnStmt( normalize( sampleVec ) );
				}
				, InVec2{ writer, "xi" }
				, InVec3{ writer, "n" }
				, InFloat{ writer, "roughness" } );

			auto geometrySchlickGGX = writer.implementFunction< Float >( "GeometrySchlickGGX"
				, [&]( Float const & product
					, Float const & roughness )
				{
					// From https://learnopengl.com/#!PBR/Lighting
					auto r = writer.declLocale( "r"
						, roughness );
					auto k = writer.declLocale( "k"
						, ( r * r ) / 2.0_f );

					auto numerator = writer.declLocale( "num"
						, product );
					auto denominator = writer.declLocale( "denom"
						, product * ( 1.0_f - k ) + k );

					writer.returnStmt( numerator / denominator );
				}
				, InFloat( writer, "product" )
				, InFloat( writer, "roughness" ) );

			auto geometrySmith = writer.implementFunction< Float >( "GeometrySmith"
				, [&]( Float const & NdotV
					, Float const & NdotL
					, Float const & roughness )
				{
					// From https://learnopengl.com/#!PBR/Lighting
					auto ggx2 = writer.declLocale( "ggx2"
						, geometrySchlickGGX( NdotV, roughness ) );
					auto ggx1 = writer.declLocale( "ggx1"
						, geometrySchlickGGX( NdotL, roughness ) );

					writer.returnStmt( ggx1 * ggx2 );
				}
				, InFloat( writer, "NdotV" )
				, InFloat( writer, "NdotL" )
				, InFloat( writer, "roughness" ) );

			auto integrateBRDF = writer.implementFunction< Vec2 >( "IntegrateBRDF"
				, [&]( Float const & NdotV
					, Float const & roughness )
				{
					// From https://learnopengl.com/#!PBR/Lighting
					auto V = writer.declLocale< Vec3 >( "V" );
					V.x() = sqrt( 1.0_f - NdotV * NdotV );
					V.y() = 0.0_f;
					V.z() = NdotV;
					auto N = writer.declLocale( "N"
						, vec3( 0.0_f, 0.0_f, 1.0_f ) );

					auto A = writer.declLocale( "A"
						, 0.0_f );
					auto B = writer.declLocale( "B"
						, 0.0_f );

					auto sampleCount = writer.declLocale( "sampleCount"
						, 1024_u );

					FOR( writer, UInt, i, 0_u, i < sampleCount, ++i )
					{
						auto xi = writer.declLocale( "xi"
							, hammersley( i, sampleCount ) );
						auto H = writer.declLocale( "H"
							, importanceSample( xi, N, roughness ) );
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
								, geometrySmith( NdotV, max( dot( N, L ), 0.0_f ), roughness ) );
							auto G_Vis = writer.declLocale( "G_Vis"
								, ( G * VdotH ) / ( NdotH * NdotV ) );
							auto Fc = writer.declLocale( "Fc"
								, pow( 1.0_f - VdotH, 5.0_f ) );

							A += ( 1.0_f - Fc ) * G_Vis;
							B += Fc * G_Vis;
						}
						FI;
					}
					ROF;

					A /= writer.cast< Float >( sampleCount );
					B /= writer.cast< Float >( sampleCount );
					writer.returnStmt( vec2( A, B ) );
				}
				, InFloat( writer, "NdotV" )
				, InFloat( writer, "roughness" ) );

			writer.implementMainT< VoidT, VoidT >( [&]( FragmentIn in
				, FragmentOut out )
				{
					pxl_fragColor.xy() = integrateBRDF( vtx_texture.x(), 1.0_f - vtx_texture.y() );
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
