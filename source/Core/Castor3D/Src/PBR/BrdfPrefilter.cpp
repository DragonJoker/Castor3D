#include "BrdfPrefilter.hpp"

#include "Engine.hpp"

#include <Buffer/VertexBuffer.hpp>
#include <Command/CommandBuffer.hpp>
#include <Image/TextureView.hpp>
#include <Pipeline/DepthStencilState.hpp>
#include <Pipeline/Scissor.hpp>
#include <Pipeline/VertexLayout.hpp>
#include <Pipeline/Viewport.hpp>
#include <RenderPass/RenderPass.hpp>
#include <RenderPass/RenderPassState.hpp>
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
		m_vertexLayout = device.createVertexLayout( 0u, sizeof( NonTexturedQuad ) );
		m_vertexLayout->createAttribute< Point2f >( 0u, offsetof( NonTexturedQuad::Vertex, position ) );

		// Initialise the geometry buffers.
		m_geometryBuffers = device.createGeometryBuffers( *m_vertexBuffer
			, 0u
			, *m_vertexLayout );

		// Create the render pass.
		std::vector< renderer::PixelFormat > formats
		{
			dstTexture.getFormat()
		};
		renderer::RenderPassAttachmentArray rpAttaches
		{
			{ dstTexture.getFormat(), false }
		};
		renderer::RenderSubpassPtrArray subpasses;
		subpasses.emplace_back( device.createRenderSubpass( formats
			, renderer::RenderSubpassState{ renderer::PipelineStageFlag::eColourAttachmentOutput
			, renderer::AccessFlag::eColourAttachmentWrite } ) );
		m_renderPass = device.createRenderPass( rpAttaches
			, std::move( subpasses )
			, renderer::RenderPassState{ renderer::PipelineStageFlag::eColourAttachmentOutput
				, renderer::AccessFlag::eColourAttachmentWrite
				, { renderer::ImageLayout::eColourAttachmentOptimal } }
			, renderer::RenderPassState{ renderer::PipelineStageFlag::eColourAttachmentOutput
				, renderer::AccessFlag::eColourAttachmentWrite
				, { renderer::ImageLayout::eColourAttachmentOptimal } } );

		// Initialise the frame buffer.
		renderer::FrameBufferAttachmentArray attaches;
		attaches.emplace_back( *( m_renderPass->begin() ), dstTexture );
		m_frameBuffer = m_renderPass->createFrameBuffer( renderer::UIVec2{ size }
			, std::move( attaches ) );

		// Initialise the pipeline.
		m_pipelineLayout = device.createPipelineLayout();
		m_pipeline = m_pipelineLayout->createPipeline( doCreateProgram()
			, { *m_vertexLayout }
			, *m_renderPass
			, renderer::PrimitiveTopology::eTriangleStrip );
		m_pipeline->depthStencilState( renderer::DepthStencilState{ 0u, false, false } );
		m_pipeline->viewport( renderer::Viewport
		{
			size.getWidth(),
			size.getHeight(),
			0,
			0,
		} );
		m_pipeline->scissor( renderer::Scissor
		{
			0,
			0,
			size.getWidth(),
			size.getHeight(),
		} );
		m_pipeline->finish();

		m_commandBuffer = device.getGraphicsCommandPool().createCommandBuffer();
	}

	void BrdfPrefilter::render()
	{
		auto & device = *m_renderSystem.getCurrentDevice();

		if ( m_commandBuffer->begin( renderer::CommandBufferUsageFlag::eOneTimeSubmit ) )
		{
			m_commandBuffer->beginRenderPass( *m_renderPass
				, *m_frameBuffer
				, { renderer::RgbaColour{ 0, 0, 0, 0 } }
				, renderer::SubpassContents::eInline );
			m_commandBuffer->bindPipeline( *m_pipeline );
			m_commandBuffer->bindGeometryBuffers( *m_geometryBuffers );
			m_commandBuffer->draw( 4u, 1u, 0u, 0u );
			m_commandBuffer->endRenderPass();
			m_commandBuffer->end();
		}

		device.getGraphicsQueue().submit( *m_commandBuffer, nullptr );
	}

	renderer::ShaderProgram & BrdfPrefilter::doCreateProgram()
	{
		glsl::Shader vtx;
		{
			using namespace glsl;
			GlslWriter writer{ m_renderSystem.createGlslWriter() };

			// Inputs
			auto position = writer.declAttribute< Vec2 >( cuT( "position" ) );

			// Outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ) );
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
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ) );

			// Outputs
			auto pxl_fragColor = writer.declOutput< Vec2 >( cuT( "pxl_FragColor" ) );

			auto radicalInverse = writer.implementFunction< Float >( cuT( "RadicalInverse_VdC" )
				, [&]( UInt const & p_bits )
				{
					auto bits = writer.declLocale( cuT( "bits" )
						, p_bits );
					bits = writer.paren( bits << 16u ) | writer.paren( bits >> 16u );
					bits = writer.paren( writer.paren( bits & 0x55555555_ui ) << 1u ) | writer.paren( writer.paren( bits & 0xAAAAAAAA_ui ) >> 1u );
					bits = writer.paren( writer.paren( bits & 0x33333333_ui ) << 2u ) | writer.paren( writer.paren( bits & 0xCCCCCCCC_ui ) >> 2u );
					bits = writer.paren( writer.paren( bits & 0x0F0F0F0F_ui ) << 4u ) | writer.paren( writer.paren( bits & 0xF0F0F0F0_ui ) >> 4u );
					bits = writer.paren( writer.paren( bits & 0x00FF00FF_ui ) << 8u ) | writer.paren( writer.paren( bits & 0xFF00FF00_ui ) >> 8u );
					writer.returnStmt( writer.cast< Float >( bits ) * 2.3283064365386963e-10 ); // / 0x100000000
				}
				, InUInt{ &writer, cuT( "p_bits" ) } );

			auto hammersley = writer.implementFunction< Vec2 >( cuT( "Hammersley" )
				, [&]( UInt const & p_i
					, UInt const & p_n )
				{
					writer.returnStmt( vec2( writer.cast< Float >( p_i ) / writer.cast< Float >( p_n ), radicalInverse( p_i ) ) );
				}
				, InUInt{ &writer, cuT( "p_i" ) }
				, InUInt{ &writer, cuT( "p_n" ) } );

			auto importanceSample = writer.implementFunction< Vec3 >( cuT( "ImportanceSampleGGX" )
				, [&]( Vec2 const & p_xi
					, Vec3 const & p_n
					, Float const & p_roughness )
				{
					// From https://learnopengl.com/#!PBR/Lighting
					auto constexpr PI = 3.1415926535897932384626433832795028841968;
					auto a = writer.declLocale( cuT( "a" )
						, p_roughness * p_roughness );

					auto phi = writer.declLocale( cuT( "phi" )
						, 2.0_f * PI * p_xi.x() );
					auto cosTheta = writer.declLocale( cuT( "cosTheta" )
						, sqrt( writer.paren( 1.0 - p_xi.y() ) / writer.paren( 1.0 + writer.paren( a * a - 1.0 ) * p_xi.y() ) ) );
					auto sinTheta = writer.declLocale( cuT( "sinTheta" )
						, sqrt( 1.0 - cosTheta * cosTheta ) );

					// from spherical coordinates to cartesian coordinates
					auto H = writer.declLocale< Vec3 >( cuT( "H" ) );
					H.x() = cos( phi ) * sinTheta;
					H.y() = sin( phi ) * sinTheta;
					H.z() = cosTheta;

					// from tangent-space vector to world-space sample vector
					auto up = writer.declLocale( cuT( "up" )
						, writer.ternary( glsl::abs( p_n.z() ) < 0.999, vec3( 0.0_f, 0.0, 1.0 ), vec3( 1.0_f, 0.0, 0.0 ) ) );
					auto tangent = writer.declLocale( cuT( "tangent" )
						, normalize( cross( up, p_n ) ) );
					auto bitangent = writer.declLocale( cuT( "bitangent" )
						, cross( p_n, tangent ) );

					auto sampleVec = writer.declLocale( cuT( "sampleVec" )
						, tangent * H.x() + bitangent * H.y() + p_n * H.z() );
					writer.returnStmt( normalize( sampleVec ) );
				}
				, InVec2{ &writer, cuT( "p_xi" ) }
				, InVec3{ &writer, cuT( "p_n" ) }
				, InFloat{ &writer, cuT( "p_roughness" ) } );

			auto geometrySchlickGGX = writer.implementFunction< Float >( cuT( "GeometrySchlickGGX" )
				, [&]( Float const & p_product
					, Float const & p_roughness )
				{
					// From https://learnopengl.com/#!PBR/Lighting
					auto r = writer.declLocale( cuT( "r" )
						, p_roughness );
					auto k = writer.declLocale( cuT( "k" )
						, writer.paren( r * r ) / 2.0_f );

					auto nominator = writer.declLocale( cuT( "num" )
						, p_product );
					auto denominator = writer.declLocale( cuT( "denom" )
						, p_product * writer.paren( 1.0_f - k ) + k );

					writer.returnStmt( nominator / denominator );
				}
				, InFloat( &writer, cuT( "p_product" ) )
				, InFloat( &writer, cuT( "p_roughness" ) ) );

			auto geometrySmith = writer.implementFunction< Float >( cuT( "GeometrySmith" )
				, [&]( Float const & p_NdotV
					, Float const & p_NdotL
					, Float const & p_roughness )
				{
					// From https://learnopengl.com/#!PBR/Lighting
					auto ggx2 = writer.declLocale( cuT( "ggx2" )
						, geometrySchlickGGX( p_NdotV, p_roughness ) );
					auto ggx1 = writer.declLocale( cuT( "ggx1" )
						, geometrySchlickGGX( p_NdotL, p_roughness ) );

					writer.returnStmt( ggx1 * ggx2 );
				}
				, InFloat( &writer, cuT( "p_NdotV" ) )
				, InFloat( &writer, cuT( "p_NdotL" ) )
				, InFloat( &writer, cuT( "p_roughness" ) ) );

			auto integrateBRDF = writer.implementFunction< Vec2 >( cuT( "IntegrateBRDF" )
				, [&]( Float const & p_NdotV
					, Float const & p_roughness )
				{
					// From https://learnopengl.com/#!PBR/Lighting
					auto V = writer.declLocale< Vec3 >( cuT( "V" ) );
					V.x() = sqrt( 1.0 - p_NdotV * p_NdotV );
					V.y() = 0.0_f;
					V.z() = p_NdotV;
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
							, importanceSample( xi, N, p_roughness ) );
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
								, geometrySmith( p_NdotV, max( dot( N, L ), 0.0 ), p_roughness ) );
							auto G_Vis = writer.declLocale( cuT( "G_Vis" )
								, writer.paren( G * VdotH ) / writer.paren( NdotH * p_NdotV ) );
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
				, InFloat( &writer, cuT( "p_NdotV" ) )
				, InFloat( &writer, cuT( "p_roughness" ) ) );

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
				{
					pxl_fragColor.xy() = integrateBRDF( vtx_texture.x(), vtx_texture.y() );
				} );

			pxl = writer.finalise();
		}

		auto & cache = m_renderSystem.getEngine()->getShaderProgramCache();
		auto & program = cache.getNewProgram( false );
		program.createModule( vtx.getSource(), renderer::ShaderStageFlag::eVertex );
		program.createModule( pxl.getSource(), renderer::ShaderStageFlag::eFragment );
		return program;
	}
}
