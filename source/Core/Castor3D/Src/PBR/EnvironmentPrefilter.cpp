#include "EnvironmentPrefilter.hpp"

#include "Engine.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <Buffer/UniformBuffer.hpp>
#include <Buffer/VertexBuffer.hpp>
#include <Image/Texture.hpp>
#include <Image/TextureView.hpp>
#include <Pipeline/DepthStencilState.hpp>
#include <Pipeline/Scissor.hpp>
#include <Pipeline/VertexLayout.hpp>
#include <Pipeline/Viewport.hpp>
#include <RenderPass/RenderPass.hpp>
#include <RenderPass/RenderPassState.hpp>
#include <RenderPass/RenderSubpass.hpp>
#include <RenderPass/RenderSubpassState.hpp>
#include <RenderPass/TextureAttachment.hpp>
#include <Shader/ShaderProgram.hpp>

#include <GlslSource.hpp>
#include <GlslUtils.hpp>

using namespace castor;

namespace castor3d
{
	namespace
	{
		TextureUnit doCreatePrefilteredTexture( Engine & engine
			, Size const & size )
		{
			auto texture = std::make_shared< TextureLayout >( *engine.getRenderSystem()
				, renderer::TextureType::eCube
				, renderer::ImageUsageFlag::eColourAttachment | renderer::ImageUsageFlag::eSampled
				, renderer::MemoryPropertyFlag::eDeviceLocal
				, PixelFormat::eRGB32F
				, size );
			SamplerSPtr sampler;
			auto name = cuT( "IblTexturesPrefiltered" );

			if ( engine.getSamplerCache().has( name ) )
			{
				sampler = engine.getSamplerCache().find( name );
			}
			else
			{
				sampler = engine.getSamplerCache().create( name );
				sampler->setMinFilter( renderer::Filter::eLinear );
				sampler->setMagFilter( renderer::Filter::eLinear );
				sampler->setMipFilter( renderer::MipmapMode::eLinear );
				sampler->setWrapS( renderer::WrapMode::eClampToEdge );
				sampler->setWrapT( renderer::WrapMode::eClampToEdge );
				sampler->setWrapR( renderer::WrapMode::eClampToEdge );
				engine.getSamplerCache().add( name, sampler );
			}

			TextureUnit result{ engine };
			result.setTexture( texture );
			result.setSampler( sampler );
			result.initialise();
			return result;
		}
	}

	EnvironmentPrefilter::EnvironmentPrefilter( Engine & engine
		, castor::Size const & size
		, renderer::Texture const & srcTexture )
		: m_renderSystem{ *engine.getRenderSystem() }
		, m_result{ doCreatePrefilteredTexture( engine, size ) }
		, m_vertexData
		{
			{
				Point3f{ -1, +1, -1 }, Point3f{ +1, -1, -1 }, Point3f{ -1, -1, -1 }, Point3f{ +1, -1, -1 }, Point3f{ -1, +1, -1 }, Point3f{ +1, +1, -1 },
				Point3f{ -1, -1, +1 }, Point3f{ -1, +1, -1 }, Point3f{ -1, -1, -1 }, Point3f{ -1, +1, -1 }, Point3f{ -1, -1, +1 }, Point3f{ -1, +1, +1 },
				Point3f{ +1, -1, -1 }, Point3f{ +1, +1, +1 }, Point3f{ +1, -1, +1 }, Point3f{ +1, +1, +1 }, Point3f{ +1, -1, -1 }, Point3f{ +1, +1, -1 },
				Point3f{ -1, -1, +1 }, Point3f{ +1, +1, +1 }, Point3f{ -1, +1, +1 }, Point3f{ +1, +1, +1 }, Point3f{ -1, -1, +1 }, Point3f{ +1, -1, +1 },
				Point3f{ -1, +1, -1 }, Point3f{ +1, +1, +1 }, Point3f{ +1, +1, -1 }, Point3f{ +1, +1, +1 }, Point3f{ -1, +1, -1 }, Point3f{ -1, +1, +1 },
				Point3f{ -1, -1, -1 }, Point3f{ +1, -1, -1 }, Point3f{ -1, -1, +1 }, Point3f{ +1, -1, -1 }, Point3f{ +1, -1, +1 }, Point3f{ -1, -1, +1 },
			}
		}
		, m_pushConstants
		{
			renderer::ShaderStageFlag::eFragment,
			{
				{ 0u, 0u, renderer::AttributeFormat::eFloat }, // Roughness
			}
		}
	{
		auto & dstTexture = m_result.getTexture()->getTexture();

		if ( !engine.getSamplerCache().has( cuT( "EnvironmentPrefilter" ) ) )
		{
			m_sampler = engine.getSamplerCache().add( cuT( "EnvironmentPrefilter" ) );
			m_sampler->setMinFilter( renderer::Filter::eLinear );
			m_sampler->setMagFilter( renderer::Filter::eLinear );
			m_sampler->setMipFilter( renderer::MipmapMode::eLinear );
			m_sampler->setWrapS( renderer::WrapMode::eClampToEdge );
			m_sampler->setWrapT( renderer::WrapMode::eClampToEdge );
			m_sampler->setWrapR( renderer::WrapMode::eClampToEdge );
			m_sampler->initialise();
		}
		else
		{
			m_sampler = engine.getSamplerCache().find( cuT( "EnvironmentPrefilter" ) );
		}
		
		auto & device = *m_renderSystem.getCurrentDevice();
		// Initialise the vertex buffer.
		m_vertexBuffer = renderer::makeVertexBuffer< NonTexturedCube >( device
			, 1u
			, 0u
			, renderer::MemoryPropertyFlag::eHostVisible );

		// Initialise the vertex layout.
		m_vertexLayout = device.createVertexLayout( 0u, sizeof( NonTexturedCube ) );
		m_vertexLayout->createAttribute< Point2f >( 0u, offsetof( NonTexturedCube::Quad::Vertex, position ) );

		// Initialise the geometry buffers.
		m_geometryBuffers = device.createGeometryBuffers( *m_vertexBuffer
			, 0u
			, *m_vertexLayout );

		// Initialise the UBO.
		m_configUbo = renderer::makeUniformBuffer< renderer::Mat4 >( device
			, 6u
			, renderer::BufferTarget::eTransferDst
			, renderer::MemoryPropertyFlag::eHostVisible );
		m_configUbo->getData( 0u ) = matrix::lookAt( Point3r{ 0.0f, 0.0f, 0.0f }, Point3r{ +1.0f, +0.0f, +0.0f }, Point3r{ 0.0f, -1.0f, +0.0f } );
		m_configUbo->getData( 1u ) = matrix::lookAt( Point3r{ 0.0f, 0.0f, 0.0f }, Point3r{ -1.0f, +0.0f, +0.0f }, Point3r{ 0.0f, -1.0f, +0.0f } );
		m_configUbo->getData( 2u ) = matrix::lookAt( Point3r{ 0.0f, 0.0f, 0.0f }, Point3r{ +0.0f, +1.0f, +0.0f }, Point3r{ 0.0f, +0.0f, +1.0f } );
		m_configUbo->getData( 3u ) = matrix::lookAt( Point3r{ 0.0f, 0.0f, 0.0f }, Point3r{ +0.0f, -1.0f, +0.0f }, Point3r{ 0.0f, +0.0f, -1.0f } );
		m_configUbo->getData( 4u ) = matrix::lookAt( Point3r{ 0.0f, 0.0f, 0.0f }, Point3r{ +0.0f, +0.0f, +1.0f }, Point3r{ 0.0f, -1.0f, +0.0f } );
		m_configUbo->getData( 5u ) = matrix::lookAt( Point3r{ 0.0f, 0.0f, 0.0f }, Point3r{ +0.0f, +0.0f, -1.0f }, Point3r{ 0.0f, -1.0f, +0.0f } );
		auto offset = m_configUbo->getOffset( 6u );

		if ( auto buffer = m_configUbo->getUbo().getBuffer().lock( 0u
			, offset
			, renderer::MemoryMapFlag::eWrite | renderer::MemoryMapFlag::eInvalidateBuffer ) )
		{
			for ( auto i = 0u; i < 6u; ++i )
			{
				std::memcpy( buffer, m_configUbo->getData( i ).constPtr(), sizeof( renderer::Mat4 ) );
				buffer += offset;
			}

			m_configUbo->getUbo().getBuffer().unlock( offset, true );
		}

		// Create the render passes.
		std::vector< renderer::PixelFormat > formats{ { dstTexture.getFormat() } };
		renderer::DescriptorSetLayoutBindingArray bindings;
		bindings.emplace_back( 0u
			, renderer::DescriptorType::eCombinedImageSampler
			, renderer::ShaderStageFlag::eFragment );
		bindings.emplace_back( 1u
			, renderer::DescriptorType::eUniformBuffer
			, renderer::ShaderStageFlag::eVertex );
		m_descriptorLayout = device.createDescriptorSetLayout( std::move( bindings ) );
		m_descriptorPool = m_descriptorLayout->createPool( 6u * ( glsl::Utils::MaxIblReflectionLod + 1 ) );
		m_pipelineLayout = device.createPipelineLayout( *m_descriptorLayout );
		auto & program = doCreateProgram( size );

		for ( auto mipLevel = 0u; mipLevel < glsl::Utils::MaxIblReflectionLod + 1u; ++mipLevel )
		{
			m_renderPasses.push_back( CubePasses{} );
			auto & mipPasses = m_renderPasses[mipLevel];

			for ( auto face = 0u; face < 6u; ++face )
			{
				auto & facePass = mipPasses[face];

				// Create the views.
				if ( mipLevel == 0u )
				{
					facePass.srcView = srcTexture.createView( renderer::TextureType::e2D
						, srcTexture.getFormat()
						, mipLevel
						, 1u
						, face
						, 1u );
				}
				else
				{
					// After first mip level, the source view is the previous mip level of destination.
					facePass.srcView = dstTexture.createView( renderer::TextureType::e2D
						, dstTexture.getFormat()
						, mipLevel - 1
						, 1u
						, face
						, 1u );
				}

				facePass.dstView = dstTexture.createView( renderer::TextureType::e2D
					, dstTexture.getFormat()
					, mipLevel
					, 1u
					, face
					, 1u );

				// Initialise the descriptor set.
				facePass.descriptorSet = m_descriptorPool->createDescriptorSet();
				facePass.descriptorSet->createBinding( m_descriptorLayout->getBinding( 0u )
					, *facePass.srcView
					, m_sampler->getSampler() );
				facePass.descriptorSet->createBinding( m_descriptorLayout->getBinding( 1u )
					, *m_configUbo
					, face
					, 1u );
				facePass.descriptorSet->update();

				// Create the render pass.
				renderer::RenderSubpassPtrArray subpasses;
				subpasses.emplace_back( device.createRenderSubpass( formats
					, renderer::RenderSubpassState{ renderer::PipelineStageFlag::eColourAttachmentOutput
					, renderer::AccessFlag::eColourAttachmentWrite } ) );
				facePass.renderPass = device.createRenderPass( formats
					, std::move( subpasses )
					, renderer::RenderPassState{ renderer::PipelineStageFlag::eColourAttachmentOutput
						, renderer::AccessFlag::eColourAttachmentWrite
						, { renderer::ImageLayout::eColourAttachmentOptimal } }
					, renderer::RenderPassState{ renderer::PipelineStageFlag::eColourAttachmentOutput
						, renderer::AccessFlag::eColourAttachmentWrite
						, { renderer::ImageLayout::eColourAttachmentOptimal } }
					, false );

				// Initialise the frame buffer.
				renderer::TextureAttachmentPtrArray attaches;
				attaches.emplace_back( std::make_unique< renderer::TextureAttachment >( *facePass.dstView ) );
				facePass.frameBuffer = facePass.renderPass->createFrameBuffer( renderer::UIVec2{ size }
					, std::move( attaches ) );

				// Initialise the pipeline.
				facePass.pipeline = device.createPipeline( *m_pipelineLayout
					, program
					, { *m_vertexLayout }
					, *facePass.renderPass
					, renderer::PrimitiveTopology::eTriangleStrip );
				facePass.pipeline->depthStencilState( renderer::DepthStencilState{ 0u, false, false } );
				facePass.pipeline->finish();
			}
		}

		m_commandBuffer = device.getGraphicsCommandPool().createCommandBuffer();

		if ( m_commandBuffer->begin( renderer::CommandBufferUsageFlag::eSimultaneousUse ) )
		{
			for ( unsigned int mipLevel = 0; mipLevel < glsl::Utils::MaxIblReflectionLod + 1; ++mipLevel )
			{
				auto & mipPasses = m_renderPasses[mipLevel];
				Size mipSize{ uint32_t( size.getWidth() * std::pow( 0.5, mipLevel ) )
					, uint32_t( size.getHeight() * std::pow( 0.5, mipLevel ) ) };
				auto roughness = mipLevel / float( glsl::Utils::MaxIblReflectionLod );
				std::memcpy( m_pushConstants.getData(), &roughness, sizeof( float ) );

				for ( auto face = 0u; face < 6u; ++face )
				{
					auto & facePass = mipPasses[face];
					m_commandBuffer->beginRenderPass( *facePass.renderPass
						, *facePass.frameBuffer
						, { renderer::RgbaColour{ 0, 0, 0, 0 } }
						, renderer::SubpassContents::eInline );
					m_commandBuffer->bindPipeline( *facePass.pipeline );
					m_commandBuffer->setViewport( {
						mipSize[0],
						mipSize[0],
						0,
						0,
					} );
					m_commandBuffer->setScissor( {
						0,
						0,
						mipSize[0],
						mipSize[0],
					} );
					m_commandBuffer->pushConstants( *m_pipelineLayout
						, m_pushConstants );
					m_commandBuffer->bindGeometryBuffers( *m_geometryBuffers );
					m_commandBuffer->draw( 36u, 1u, 0u, 0u );
					m_commandBuffer->endRenderPass();
				}
			}

			m_commandBuffer->end();
		}
	}

	void EnvironmentPrefilter::render()
	{
		auto & device = *m_renderSystem.getCurrentDevice();
		device.getGraphicsQueue().submit( *m_commandBuffer, nullptr );
	}

	renderer::ShaderProgram & EnvironmentPrefilter::doCreateProgram( Size const & size )
	{
		glsl::Shader vtx;
		{
			using namespace glsl;
			GlslWriter writer{ m_renderSystem.createGlslWriter() };

			// Inputs
			auto position = writer.declAttribute< Vec3 >( cuT( "position" ) );
			UBO_MATRIX( writer, 0 );

			// Outputs
			auto vtx_worldPosition = writer.declOutput< Vec3 >( cuT( "vtx_worldPosition" ) );
			auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

			std::function< void() > main = [&]()
			{
				vtx_worldPosition = position;
				auto view = writer.declLocale( cuT( "normal" )
					, mat4( mat3( c3d_curView ) ) );
				gl_Position = writer.paren( c3d_projection * view * vec4( position, 1.0 ) ).SWIZZLE_XYWW;
			};

			writer.implementFunction< void >( cuT( "main" ), main );
			vtx = writer.finalise();
		}

		glsl::Shader pxl;
		{
			using namespace glsl;
			GlslWriter writer{ m_renderSystem.createGlslWriter() };

			// Inputs
			auto vtx_worldPosition = writer.declInput< Vec3 >( cuT( "vtx_worldPosition" ) );
			auto c3d_mapDiffuse = writer.declSampler< SamplerCube >( cuT( "c3d_mapDiffuse" ), MinTextureIndex, 0u );
			Ubo config{ writer, cuT( "Config" ), 0u };
			auto c3d_roughness = config.declMember< Float >( cuT( "c3d_roughness" ) );
			config.end();

			// Outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( cuT( "pxl_FragColor" ) );

			auto distributionGGX = writer.implementFunction< Float >( cuT( "DistributionGGX" )
				, [&]( Vec3 const & p_N
					, Vec3 const & p_H
					, Float const & p_roughness )
				{
					auto constexpr PI = 3.1415926535897932384626433832795028841968;
					auto a = writer.declLocale( cuT( "a" )
						, p_roughness * p_roughness );
					auto a2 = writer.declLocale( cuT( "a2" )
						, a * a );
					auto NdotH = writer.declLocale( cuT( "NdotH" )
						, max( dot( p_N, p_H ), 0.0 ) );
					auto NdotH2 = writer.declLocale( cuT( "NdotH2" )
						, NdotH * NdotH );

					auto nom = writer.declLocale( cuT( "nom" )
						, a2 );
					auto denom = writer.declLocale( cuT( "denom" )
						, ( NdotH2 * writer.paren( a2 - 1.0 ) + 1.0 ) );
					denom = denom * denom * PI;

					writer.returnStmt( nom / denom );
				}
				, InVec3{ &writer, cuT( "p_N" ) }
				, InVec3{ &writer, cuT( "p_H" ) }
				, InFloat{ &writer, cuT( "p_roughness" ) } );

			auto radicalInverse = writer.implementFunction< Float >( cuT( "RadicalInverse_VdC" )
				, [&]( UInt const & p_bits )
				{
					// From https://learnopengl.com/#!PBR/Lighting
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
					// From https://learnopengl.com/#!PBR/Lighting
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

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				auto constexpr PI = 3.1415926535897932384626433832795028841968;
				// From https://learnopengl.com/#!PBR/Lighting
				auto N = writer.declLocale( cuT( "N" )
					, normalize( vtx_worldPosition ) );
				auto R = writer.declLocale( cuT( "R" )
					, N );
				auto V = writer.declLocale( cuT( "V" )
					, R );

				auto sampleCount = writer.declLocale( cuT( "sampleCount" )
					, 1024_ui );
				auto totalWeight = writer.declLocale( cuT( "totalWeight" )
					, 0.0_f );
				auto prefilteredColor = writer.declLocale( cuT( "prefilteredColor" )
					, vec3( 0.0_f ) );

				FOR( writer, UInt, i, 0u, "i < sampleCount", "++i" )
				{
					auto xi = writer.declLocale( cuT( "xi" )
						, hammersley( i, sampleCount ) );
					auto H = writer.declLocale( cuT( "H" )
						, importanceSample( xi, N, c3d_roughness ) );
					auto L = writer.declLocale( cuT( "L" )
						, normalize( vec3( 2.0_f ) * dot( V, H ) * H - V ) );

					auto NdotL = writer.declLocale( cuT( "NdotL" )
						, max( dot( N, L ), 0.0 ) );

					IF( writer, "NdotL > 0.0" )
					{
						auto D = writer.declLocale( cuT( "D" )
							, distributionGGX( N, H, c3d_roughness ) );
						auto NdotH = writer.declLocale( cuT( "NdotH" )
							, max( dot( N, H ), 0.0 ) );
						auto HdotV = writer.declLocale( cuT( "HdotV" )
							, max( dot( H, V ), 0.0 ) );
						auto pdf = writer.declLocale( cuT( "pdf" )
							, D * NdotH / writer.paren( 4.0_f * HdotV ) + 0.0001 );

						auto resolution = writer.declLocale( cuT( "resolution" )
							, Float( int( size.getWidth() ) ) ); // resolution of source cubemap (per face)
						auto saTexel = writer.declLocale( cuT( "saTexel" )
							, 4.0_f * PI / writer.paren( 6.0 * resolution * resolution ) );
						auto saSample = writer.declLocale( cuT( "saSample" )
							, 1.0_f / writer.paren( writer.cast< Float >( sampleCount ) * pdf + 0.0001 ) );
						auto mipLevel = writer.declLocale( cuT( "mipLevel" )
							, writer.ternary( c3d_roughness == 0.0_f, 0.0_f, 0.5_f * log2( saSample / saTexel ) ) );

						prefilteredColor += texture( c3d_mapDiffuse, L, mipLevel ).rgb() * NdotL;
						totalWeight += NdotL;
					}
					FI;
				}
				ROF;

				prefilteredColor = prefilteredColor / totalWeight;
				pxl_fragColor = vec4( prefilteredColor, 1.0 );
			} );

			pxl = writer.finalise();
		}

		auto & cache = m_renderSystem.getEngine()->getShaderProgramCache();
		auto & program = cache.getNewProgram( false );
		program.createModule( vtx.getSource(), renderer::ShaderStageFlag::eVertex );
		program.createModule( pxl.getSource(), renderer::ShaderStageFlag::eFragment );
		program.link();
		return program;
	}
}
