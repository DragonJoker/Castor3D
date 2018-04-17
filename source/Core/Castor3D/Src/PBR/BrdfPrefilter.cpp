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
#include <Shader/ShaderProgram.hpp>

#include <GlslSource.hpp>

using namespace castor;

namespace castor3d
{
	BrdfPrefilter::BrdfPrefilter( Engine & engine
		, castor::Size const & size
		, renderer::TextureView const & dstTexture )
		: m_renderSystem{ *engine.getRenderSystem() }
		, m_vertexData
		{
			{
				{ Point2f{ -1.0, -1.0 } },
				{ Point2f{ -1.0, +1.0 } },
				{ Point2f{ +1.0, -1.0 } },
				{ Point2f{ +1.0, +1.0 } },
			}
		}
	{
		auto & device = *m_renderSystem.getCurrentDevice();

		// Initialise the vertex buffer.
		m_vertexBuffer = renderer::makeVertexBuffer< NonTexturedQuad >( device
			, 1u
			, 0u
			, renderer::MemoryPropertyFlag::eHostVisible );

		// Initialise the vertex layout.
		m_vertexLayout = renderer::makeLayout< NonTexturedQuad >( 0u );
		m_vertexLayout->createAttribute( 0u, renderer::Format::eR32G32_SFLOAT, offsetof( NonTexturedQuad::Vertex, position ) );

		// Create the render pass.
		renderer::RenderPassCreateInfo createInfo{};
		createInfo.flags = 0u;

		createInfo.attachments.resize( 1u );
		createInfo.attachments[0].format = dstTexture.getFormat();
		createInfo.attachments[0].samples = renderer::SampleCountFlag::e1;
		createInfo.attachments[0].loadOp = renderer::AttachmentLoadOp::eClear;
		createInfo.attachments[0].storeOp = renderer::AttachmentStoreOp::eStore;
		createInfo.attachments[0].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
		createInfo.attachments[0].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
		createInfo.attachments[0].initialLayout = renderer::ImageLayout::eUndefined;
		createInfo.attachments[0].finalLayout = renderer::ImageLayout::eColourAttachmentOptimal;

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

		m_renderPass = device.createRenderPass( createInfo );

		// Initialise the frame buffer.
		renderer::FrameBufferAttachmentArray attaches;
		attaches.emplace_back( *( m_renderPass->getAttachments().begin() ), dstTexture );
		m_frameBuffer = m_renderPass->createFrameBuffer( renderer::Extent2D{ size.getWidth(), size.getHeight() }
			, std::move( attaches ) );

		// Initialise the pipeline.
		m_pipelineLayout = device.createPipelineLayout();
		m_pipeline = m_pipelineLayout->createPipeline( renderer::GraphicsPipelineCreateInfo{
			doCreateProgram(),
			*m_renderPass,
			renderer::VertexInputState::create( *m_vertexLayout ),
			renderer::InputAssemblyState{ renderer::PrimitiveTopology::eTriangleStrip },
			renderer::RasterisationState{},
			renderer::MultisampleState{},
			renderer::ColourBlendState::createDefault(),
			{},
			renderer::DepthStencilState{ 0u, false, false },
			std::nullopt,
			renderer::Viewport{ size.getWidth(), size.getHeight(), 0, 0, },
			renderer::Scissor{ 0, 0, size.getWidth(), size.getHeight() },
		} );

		m_commandBuffer = device.getGraphicsCommandPool().createCommandBuffer();
	}

	void BrdfPrefilter::render()
	{
		auto & device = *m_renderSystem.getCurrentDevice();

		if ( m_commandBuffer->begin( renderer::CommandBufferUsageFlag::eOneTimeSubmit ) )
		{
			m_commandBuffer->beginRenderPass( *m_renderPass
				, *m_frameBuffer
				, { renderer::ClearColorValue{ 0, 0, 0, 0 } }
				, renderer::SubpassContents::eInline );
			m_commandBuffer->bindPipeline( *m_pipeline );
			m_commandBuffer->bindVertexBuffer( 0u, m_vertexBuffer->getBuffer(), 0u );
			m_commandBuffer->draw( 4u, 1u, 0u, 0u );
			m_commandBuffer->endRenderPass();
			m_commandBuffer->end();
		}

		device.getGraphicsQueue().submit( *m_commandBuffer, nullptr );
	}

	renderer::ShaderStageStateArray BrdfPrefilter::doCreateProgram()
	{
		glsl::Shader vtx;
		{
			using namespace glsl;
			GlslWriter writer{ m_renderSystem.createGlslWriter() };

			// Inputs
			auto position = writer.declAttribute< Vec2 >( cuT( "position" ), 0u );

			// Outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ), 0u );
			auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

			std::function< void() > main = [&]()
			{
				vtx_texture = position;
				gl_Position = vec4( position.x(), position.y(), 0.0, 1.0 );
			};

			writer.implementFunction< void >( cuT( "main" ), main );
			vtx = writer.finalise();
		}

		glsl::Shader pxl;
		{
			using namespace glsl;
			GlslWriter writer{ m_renderSystem.createGlslWriter() };

			// Inputs
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ), 0u );

			// Outputs
			auto pxl_fragColor = writer.declFragData< Vec2 >( cuT( "pxl_FragColor" ), 0u );

			auto radicalInverse = writer.implementFunction< Float >( cuT( "RadicalInverse_VdC" )
				, [&]( UInt const & inBits )
				{
					auto bits = writer.declLocale( cuT( "bits" )
						, inBits );
					bits = writer.paren( bits << 16u ) | writer.paren( bits >> 16u );
					bits = writer.paren( writer.paren( bits & 0x55555555_ui ) << 1u ) | writer.paren( writer.paren( bits & 0xAAAAAAAA_ui ) >> 1u );
					bits = writer.paren( writer.paren( bits & 0x33333333_ui ) << 2u ) | writer.paren( writer.paren( bits & 0xCCCCCCCC_ui ) >> 2u );
					bits = writer.paren( writer.paren( bits & 0x0F0F0F0F_ui ) << 4u ) | writer.paren( writer.paren( bits & 0xF0F0F0F0_ui ) >> 4u );
					bits = writer.paren( writer.paren( bits & 0x00FF00FF_ui ) << 8u ) | writer.paren( writer.paren( bits & 0xFF00FF00_ui ) >> 8u );
					writer.returnStmt( writer.cast< Float >( bits ) * 2.3283064365386963e-10 ); // / 0x100000000
				}
				, InUInt{ &writer, cuT( "inBits" ) } );

			auto hammersley = writer.implementFunction< Vec2 >( cuT( "Hammersley" )
				, [&]( UInt const & i
					, UInt const & n )
				{
					writer.returnStmt( vec2( writer.cast< Float >( i ) / writer.cast< Float >( n ), radicalInverse( i ) ) );
				}
				, InUInt{ &writer, cuT( "i" ) }
				, InUInt{ &writer, cuT( "n" ) } );

			auto importanceSample = writer.implementFunction< Vec3 >( cuT( "ImportanceSampleGGX" )
				, [&]( Vec2 const & xi
					, Vec3 const & n
					, Float const & roughness )
				{
					// From https://learnopengl.com/#!PBR/Lighting
					auto constexpr PI = 3.1415926535897932384626433832795028841968;
					auto a = writer.declLocale( cuT( "a" )
						, roughness * roughness );

					auto phi = writer.declLocale( cuT( "phi" )
						, 2.0_f * PI * xi.x() );
					auto cosTheta = writer.declLocale( cuT( "cosTheta" )
						, sqrt( writer.paren( 1.0 - xi.y() ) / writer.paren( 1.0 + writer.paren( a * a - 1.0 ) * xi.y() ) ) );
					auto sinTheta = writer.declLocale( cuT( "sinTheta" )
						, sqrt( 1.0 - cosTheta * cosTheta ) );

					// from spherical coordinates to cartesian coordinates
					auto H = writer.declLocale< Vec3 >( cuT( "H" ) );
					H.x() = cos( phi ) * sinTheta;
					H.y() = sin( phi ) * sinTheta;
					H.z() = cosTheta;

					// from tangent-space vector to world-space sample vector
					auto up = writer.declLocale( cuT( "up" )
						, writer.ternary( glsl::abs( n.z() ) < 0.999, vec3( 0.0_f, 0.0, 1.0 ), vec3( 1.0_f, 0.0, 0.0 ) ) );
					auto tangent = writer.declLocale( cuT( "tangent" )
						, normalize( cross( up, n ) ) );
					auto bitangent = writer.declLocale( cuT( "bitangent" )
						, cross( n, tangent ) );

					auto sampleVec = writer.declLocale( cuT( "sampleVec" )
						, tangent * H.x() + bitangent * H.y() + n * H.z() );
					writer.returnStmt( normalize( sampleVec ) );
				}
				, InVec2{ &writer, cuT( "xi" ) }
				, InVec3{ &writer, cuT( "n" ) }
				, InFloat{ &writer, cuT( "roughness" ) } );

			auto geometrySchlickGGX = writer.implementFunction< Float >( cuT( "GeometrySchlickGGX" )
				, [&]( Float const & product
					, Float const & roughness )
				{
					// From https://learnopengl.com/#!PBR/Lighting
					auto r = writer.declLocale( cuT( "r" )
						, roughness );
					auto k = writer.declLocale( cuT( "k" )
						, writer.paren( r * r ) / 2.0_f );

					auto nominator = writer.declLocale( cuT( "num" )
						, product );
					auto denominator = writer.declLocale( cuT( "denom" )
						, product * writer.paren( 1.0_f - k ) + k );

					writer.returnStmt( nominator / denominator );
				}
				, InFloat( &writer, cuT( "product" ) )
				, InFloat( &writer, cuT( "roughness" ) ) );

			auto geometrySmith = writer.implementFunction< Float >( cuT( "GeometrySmith" )
				, [&]( Float const & NdotV
					, Float const & NdotL
					, Float const & roughness )
				{
					// From https://learnopengl.com/#!PBR/Lighting
					auto ggx2 = writer.declLocale( cuT( "ggx2" )
						, geometrySchlickGGX( NdotV, roughness ) );
					auto ggx1 = writer.declLocale( cuT( "ggx1" )
						, geometrySchlickGGX( NdotL, roughness ) );

					writer.returnStmt( ggx1 * ggx2 );
				}
				, InFloat( &writer, cuT( "NdotV" ) )
				, InFloat( &writer, cuT( "NdotL" ) )
				, InFloat( &writer, cuT( "roughness" ) ) );

			auto integrateBRDF = writer.implementFunction< Vec2 >( cuT( "IntegrateBRDF" )
				, [&]( Float const & NdotV
					, Float const & roughness )
				{
					// From https://learnopengl.com/#!PBR/Lighting
					auto V = writer.declLocale< Vec3 >( cuT( "V" ) );
					V.x() = sqrt( 1.0 - NdotV * NdotV );
					V.y() = 0.0_f;
					V.z() = NdotV;
					auto N = writer.declLocale( cuT( "N" )
						, vec3( 0.0_f, 0.0, 1.0 ) );

					auto A = writer.declLocale( cuT( "A" )
						, 0.0_f );
					auto B = writer.declLocale( cuT( "B" )
						, 0.0_f );

					auto sampleCount = writer.declLocale( cuT( "sampleCount" )
						, 1024_ui );

					FOR( writer, UInt, i, 0u, "i < sampleCount", "++i" )
					{
						auto xi = writer.declLocale( cuT( "xi" )
							, hammersley( i, sampleCount ) );
						auto H = writer.declLocale( cuT( "H" )
							, importanceSample( xi, N, roughness ) );
						auto L = writer.declLocale( cuT( "L" )
							, normalize( vec3( 2.0_f ) * dot( V, H ) * H - V ) );

						auto NdotL = writer.declLocale( cuT( "NdotL" )
							, max( L.z(), 0.0 ) );
						auto NdotH = writer.declLocale( cuT( "NdotH" )
							, max( H.z(), 0.0 ) );
						auto VdotH = writer.declLocale( cuT( "VdotH" )
							, max( dot( V, H ), 0.0 ) );

						IF( writer, "NdotL > 0.0" )
						{
							auto G = writer.declLocale( cuT( "G" )
								, geometrySmith( NdotV, max( dot( N, L ), 0.0 ), roughness ) );
							auto G_Vis = writer.declLocale( cuT( "G_Vis" )
								, writer.paren( G * VdotH ) / writer.paren( NdotH * NdotV ) );
							auto Fc = writer.declLocale( cuT( "Fc" )
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
				, InFloat( &writer, cuT( "NdotV" ) )
				, InFloat( &writer, cuT( "roughness" ) ) );

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
				{
					pxl_fragColor.xy() = integrateBRDF( vtx_texture.x(), vtx_texture.y() );
				} );

			pxl = writer.finalise();
		}

		renderer::ShaderStageStateArray program
		{
			{ m_renderSystem.getCurrentDevice()->createShaderModule( renderer::ShaderStageFlag::eVertex ) },
			{ m_renderSystem.getCurrentDevice()->createShaderModule( renderer::ShaderStageFlag::eFragment ) }
		};
		program[0].module->loadShader( vtx.getSource() );
		program[1].module->loadShader( pxl.getSource() );
		return program;
	}
}
