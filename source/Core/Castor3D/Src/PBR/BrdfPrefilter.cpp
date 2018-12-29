#include "BrdfPrefilter.hpp"

#include "Engine.hpp"

#include <Buffer/VertexBuffer.hpp>
#include <Command/CommandBuffer.hpp>
#include <Image/TextureView.hpp>
#include <Pipeline/DepthStencilState.hpp>
#include <Pipeline/InputAssemblyState.hpp>
#include <Pipeline/Scissor.hpp>
#include <Pipeline/VertexLayout.hpp>
#include <Pipeline/Viewport.hpp>
#include <RenderPass/RenderPass.hpp>
#include <RenderPass/RenderPassCreateInfo.hpp>
#include <RenderPass/RenderSubpass.hpp>
#include <RenderPass/RenderSubpassState.hpp>
#include <RenderPass/FrameBufferAttachment.hpp>
#include <Shader/GlslToSpv.hpp>
#include <Sync/Fence.hpp>

#include <ShaderWriter/Source.hpp>

using namespace castor;

namespace castor3d
{
	BrdfPrefilter::BrdfPrefilter( Engine & engine
		, castor::Size const & size
		, ashes::TextureView const & dstTexture )
		: m_renderSystem{ *engine.getRenderSystem() }
	{
		auto & device = getCurrentDevice( m_renderSystem );

		// Initialise the vertex buffer.
		m_vertexBuffer = ashes::makeVertexBuffer< TexturedQuad >( device
			, 1u
			, 0u
			, ashes::MemoryPropertyFlag::eHostVisible );

		if ( auto buffer = m_vertexBuffer->lock( 0u, 1u, ashes::MemoryMapFlag::eWrite ) )
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
		m_vertexLayout = ashes::makeLayout< TexturedQuad::Vertex >( 0u );
		m_vertexLayout->createAttribute( 0u
			, ashes::Format::eR32G32_SFLOAT
			, offsetof( TexturedQuad::Vertex, position ) );
		m_vertexLayout->createAttribute( 1u
			, ashes::Format::eR32G32_SFLOAT
			, offsetof( TexturedQuad::Vertex, texture ) );

		// Create the render pass.
		ashes::RenderPassCreateInfo createInfo{};
		createInfo.flags = 0u;

		createInfo.attachments.resize( 1u );
		createInfo.attachments[0].format = dstTexture.getFormat();
		createInfo.attachments[0].samples = ashes::SampleCountFlag::e1;
		createInfo.attachments[0].loadOp = ashes::AttachmentLoadOp::eClear;
		createInfo.attachments[0].storeOp = ashes::AttachmentStoreOp::eStore;
		createInfo.attachments[0].stencilLoadOp = ashes::AttachmentLoadOp::eDontCare;
		createInfo.attachments[0].stencilStoreOp = ashes::AttachmentStoreOp::eDontCare;
		createInfo.attachments[0].initialLayout = ashes::ImageLayout::eUndefined;
		createInfo.attachments[0].finalLayout = ashes::ImageLayout::eColourAttachmentOptimal;

		ashes::AttachmentReference colourReference;
		colourReference.attachment = 0u;
		colourReference.layout = ashes::ImageLayout::eColourAttachmentOptimal;

		createInfo.subpasses.resize( 1u );
		createInfo.subpasses[0].flags = 0u;
		createInfo.subpasses[0].colorAttachments = { colourReference };

		createInfo.dependencies.resize( 2u );
		createInfo.dependencies[0].srcSubpass = ashes::ExternalSubpass;
		createInfo.dependencies[0].dstSubpass = 0u;
		createInfo.dependencies[0].srcAccessMask = ashes::AccessFlag::eColourAttachmentWrite;
		createInfo.dependencies[0].dstAccessMask = ashes::AccessFlag::eShaderRead;
		createInfo.dependencies[0].srcStageMask = ashes::PipelineStageFlag::eColourAttachmentOutput;
		createInfo.dependencies[0].dstStageMask = ashes::PipelineStageFlag::eFragmentShader;
		createInfo.dependencies[0].dependencyFlags = ashes::DependencyFlag::eByRegion;

		createInfo.dependencies[1].srcSubpass = 0u;
		createInfo.dependencies[1].dstSubpass = ashes::ExternalSubpass;
		createInfo.dependencies[1].srcAccessMask = ashes::AccessFlag::eColourAttachmentWrite;
		createInfo.dependencies[1].dstAccessMask = ashes::AccessFlag::eShaderRead;
		createInfo.dependencies[1].srcStageMask = ashes::PipelineStageFlag::eColourAttachmentOutput;
		createInfo.dependencies[1].dstStageMask = ashes::PipelineStageFlag::eFragmentShader;
		createInfo.dependencies[1].dependencyFlags = ashes::DependencyFlag::eByRegion;

		m_renderPass = device.createRenderPass( createInfo );

		// Initialise the frame buffer.
		ashes::FrameBufferAttachmentArray attaches;
		attaches.emplace_back( *( m_renderPass->getAttachments().begin() ), dstTexture );
		m_frameBuffer = m_renderPass->createFrameBuffer( ashes::Extent2D{ size.getWidth(), size.getHeight() }
			, std::move( attaches ) );

		// Initialise the pipeline.
		m_pipelineLayout = device.createPipelineLayout();
		m_pipeline = m_pipelineLayout->createPipeline( ashes::GraphicsPipelineCreateInfo{
			doCreateProgram(),
			*m_renderPass,
			ashes::VertexInputState::create( *m_vertexLayout ),
			ashes::InputAssemblyState{ ashes::PrimitiveTopology::eTriangleList },
			ashes::RasterisationState{},
			ashes::MultisampleState{},
			ashes::ColourBlendState::createDefault(),
			{},
			ashes::DepthStencilState{ 0u, false, false },
			std::nullopt,
			ashes::Viewport{ size.getWidth(), size.getHeight(), 0, 0, },
			ashes::Scissor{ 0, 0, size.getWidth(), size.getHeight() },
		} );

		m_commandBuffer = device.getGraphicsCommandPool().createCommandBuffer();
	}

	void BrdfPrefilter::render()
	{
		auto & device = getCurrentDevice( m_renderSystem );
		auto fence = device.createFence();

		m_commandBuffer->begin( ashes::CommandBufferUsageFlag::eOneTimeSubmit );
		m_commandBuffer->beginRenderPass( *m_renderPass
			, *m_frameBuffer
			, { ashes::ClearColorValue{ 0, 0, 0, 0 } }
			, ashes::SubpassContents::eInline );
		m_commandBuffer->bindPipeline( *m_pipeline );
		m_commandBuffer->bindVertexBuffer( 0u, m_vertexBuffer->getBuffer(), 0u );
		m_commandBuffer->draw( 6u );
		m_commandBuffer->endRenderPass();
		m_commandBuffer->end();

		device.getGraphicsQueue().submit( *m_commandBuffer, fence.get() );
		fence->wait( ashes::FenceTimeout );
		device.getGraphicsQueue().waitIdle();
	}

	ashes::ShaderStageStateArray BrdfPrefilter::doCreateProgram()
	{
		ShaderModule vtx{ ashes::ShaderStageFlag::eVertex, "BRDFPrefilter" };
		{
			using namespace sdw;
			VertexWriter writer;

			// Inputs
			auto position = writer.declInput< Vec2 >( "position", 0u );
			auto texcoord = writer.declInput< Vec2 >( "texcoord", 1u );

			// Outputs
			auto vtx_texture = writer.declOutput< Vec2 >( "vtx_texture", 0u );
			auto out = writer.getOut();

			writer.implementFunction< sdw::Void >( cuT( "main" )
				, [&]()
				{
					vtx_texture = texcoord;
					out.gl_out.gl_Position = vec4( position, 0.0, 1.0 );
				} );
			vtx.shader = std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}

		ShaderModule pxl{ ashes::ShaderStageFlag::eFragment, "BRDFPrefilter" };
		{
			using namespace sdw;
			FragmentWriter writer;

			// Inputs
			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

			// Outputs
			auto pxl_fragColor = writer.declOutput< Vec2 >( "pxl_FragColor", 0u );

			auto radicalInverse = writer.implementFunction< Float >( cuT( "RadicalInverse_VdC" )
				, [&]( UInt const & inBits )
				{
					auto bits = writer.declLocale( cuT( "bits" )
						, inBits );
					bits = writer.paren( bits << 16u ) | writer.paren( bits >> 16u );
					bits = writer.paren( writer.paren( bits & 0x55555555_u ) << 1u ) | writer.paren( writer.paren( bits & 0xAAAAAAAA_u ) >> 1u );
					bits = writer.paren( writer.paren( bits & 0x33333333_u ) << 2u ) | writer.paren( writer.paren( bits & 0xCCCCCCCC_u ) >> 2u );
					bits = writer.paren( writer.paren( bits & 0x0F0F0F0F_u ) << 4u ) | writer.paren( writer.paren( bits & 0xF0F0F0F0_u ) >> 4u );
					bits = writer.paren( writer.paren( bits & 0x00FF00FF_u ) << 8u ) | writer.paren( writer.paren( bits & 0xFF00FF00_u ) >> 8u );
					writer.returnStmt( writer.cast< Float >( bits ) * 2.3283064365386963e-10 ); // / 0x100000000
				}
				, InUInt{ writer, cuT( "inBits" ) } );

			auto hammersley = writer.implementFunction< Vec2 >( "Hammersley"
				, [&]( UInt const & i
					, UInt const & n )
				{
					writer.returnStmt( vec2( writer.cast< Float >( i ) / writer.cast< Float >( n ), radicalInverse( i ) ) );
				}
				, InUInt{ writer, "i" }
				, InUInt{ writer, "n" } );

			auto importanceSample = writer.implementFunction< Vec3 >( cuT( "ImportanceSampleGGX" )
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
						, sqrt( writer.paren( 1.0 - xi.y() ) / writer.paren( 1.0 + writer.paren( a * a - 1.0 ) * xi.y() ) ) );
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
						, writer.paren( r * r ) / 2.0_f );

					auto nominator = writer.declLocale( "num"
						, product );
					auto denominator = writer.declLocale( "denom"
						, product * writer.paren( 1.0_f - k ) + k );

					writer.returnStmt( nominator / denominator );
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
					V.x() = sqrt( 1.0 - NdotV * NdotV );
					V.y() = 0.0_f;
					V.z() = NdotV;
					auto N = writer.declLocale( "N"
						, vec3( 0.0_f, 0.0, 1.0 ) );

					auto A = writer.declLocale( "A"
						, 0.0_f );
					auto B = writer.declLocale( "B"
						, 0.0_f );

					auto sampleCount = writer.declLocale( "sampleCount"
						, 1024_u );

					FOR( writer, UInt, i, 0u, i < sampleCount, ++i )
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
								, writer.paren( G * VdotH ) / writer.paren( NdotH * NdotV ) );
							auto Fc = writer.declLocale( "Fc"
								, pow( 1.0 - VdotH, 5.0_f ) );

							A += writer.paren( 1.0_f - Fc ) * G_Vis;
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

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					pxl_fragColor.xy() = integrateBRDF( vtx_texture.x(), 1.0_f - vtx_texture.y() );
				} );

			pxl.shader = std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}

		ashes::ShaderStageStateArray program
		{
			{ getCurrentDevice( m_renderSystem ).createShaderModule( ashes::ShaderStageFlag::eVertex ) },
			{ getCurrentDevice( m_renderSystem ).createShaderModule( ashes::ShaderStageFlag::eFragment ) }
		};
		program[0].module->loadShader( m_renderSystem.compileShader( vtx ) );
		program[1].module->loadShader( m_renderSystem.compileShader( pxl ) );
		return program;
	}
}
