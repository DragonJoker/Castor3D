#include "EnvironmentPrefilter.hpp"

#include "Engine.hpp"
#include "Shader/Ubos/MatrixUbo.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <Miscellaneous/PushConstantRange.hpp>
#include <RenderPass/RenderPassCreateInfo.hpp>
#include <RenderPass/FrameBufferAttachment.hpp>
#include <Shader/GlslToSpv.hpp>
#include <Sync/Fence.hpp>
#include <Sync/ImageMemoryBarrier.hpp>

#include <GlslSource.hpp>
#include <GlslUtils.hpp>

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		ashes::TexturePtr doCreatePrefilteredTexture( RenderSystem const & renderSystem
			, Size const & size )
		{
			auto & device = getCurrentDevice( renderSystem );
			ashes::ImageCreateInfo image{};
			image.flags = ashes::ImageCreateFlag::eCubeCompatible;
			image.arrayLayers = 6u;
			image.extent.width = size[0];
			image.extent.height = size[1];
			image.extent.depth = 1u;
			image.format = ashes::Format::eR32G32B32A32_SFLOAT;
			image.imageType = ashes::TextureType::e2D;
			image.initialLayout = ashes::ImageLayout::eUndefined;
			image.mipLevels = glsl::Utils::MaxIblReflectionLod + 1;
			image.samples = ashes::SampleCountFlag::e1;
			image.sharingMode = ashes::SharingMode::eExclusive;
			image.tiling = ashes::ImageTiling::eOptimal;
			image.usage = ashes::ImageUsageFlag::eColourAttachment | ashes::ImageUsageFlag::eSampled;
			return device.createTexture( image
				, ashes::MemoryPropertyFlag::eDeviceLocal );
		}

		SamplerSPtr doCreateSampler( Engine & engine
			, uint32_t maxLod )
		{
			SamplerSPtr result;
			StringStream stream = makeStringStream();
			stream << cuT( "IblTexturesPrefiltered_" ) << maxLod;
			auto name = stream.str();

			if ( engine.getSamplerCache().has( name ) )
			{
				result = engine.getSamplerCache().find( name );
			}
			else
			{
				result = engine.getSamplerCache().create( name );
				result->setMinFilter( ashes::Filter::eLinear );
				result->setMagFilter( ashes::Filter::eLinear );
				result->setMipFilter( ashes::MipmapMode::eLinear );
				result->setWrapS( ashes::WrapMode::eClampToEdge );
				result->setWrapT( ashes::WrapMode::eClampToEdge );
				result->setWrapR( ashes::WrapMode::eClampToEdge );
				result->setMinLod( 0.0f );
				result->setMaxLod( float( maxLod ) );
				engine.getSamplerCache().add( name, result );
			}

			result->initialise();
			return result;
		}

		ashes::ShaderStageStateArray doCreateProgram( RenderSystem & renderSystem
			, ashes::Extent2D const & size
			, uint32_t mipLevel )
		{
			glsl::Shader vtx;
			{
				using namespace glsl;
				GlslWriter writer{ renderSystem.createGlslWriter() };

				// Inputs
				auto position = writer.declAttribute< Vec3 >( cuT( "position" ), 0u );
				Ubo matrix{ writer, cuT( "Matrix" ), 0u, 0u };
				auto c3d_viewProjection = matrix.declMember< Mat4 >( cuT( "c3d_viewProjection" ) );
				matrix.end();

				// Outputs
				auto vtx_worldPosition = writer.declOutput< Vec3 >( cuT( "vtx_worldPosition" ), 0u );
				auto out = gl_PerVertex{ writer };

				std::function< void() > main = [&]()
				{
					vtx_worldPosition = position;
					out.gl_Position() = writer.paren( c3d_viewProjection * vec4( position, 1.0 ) ).xyww();
				};

				writer.implementFunction< void >( cuT( "main" ), main );
				vtx = writer.finalise();
			}

			glsl::Shader pxl;
			{
				using namespace glsl;
				GlslWriter writer{ renderSystem.createGlslWriter() };

				// Inputs
				auto vtx_worldPosition = writer.declInput< Vec3 >( cuT( "vtx_worldPosition" ), 0u );
				auto c3d_mapDiffuse = writer.declSampler< SamplerCube >( cuT( "c3d_mapDiffuse" ), 1u, 0u );
				auto c3d_roughness = writer.declConstant< Float >( cuT( "c3d_roughness" ), mipLevel / float( glsl::Utils::MaxIblReflectionLod ) );

				// Outputs
				auto pxl_fragColor = writer.declFragData< Vec4 >( cuT( "pxl_FragColor" ), 0u );

				auto distributionGGX = writer.implementFunction< Float >( cuT( "DistributionGGX" )
					, [&]( Vec3 const & N
						, Vec3 const & H
						, Float const & roughness )
				{
					auto constexpr PI = 3.1415926535897932384626433832795028841968;
					auto a = writer.declLocale( cuT( "a" )
						, roughness * roughness );
					auto a2 = writer.declLocale( cuT( "a2" )
						, a * a );
					auto NdotH = writer.declLocale( cuT( "NdotH" )
						, max( dot( N, H ), 0.0 ) );
					auto NdotH2 = writer.declLocale( cuT( "NdotH2" )
						, NdotH * NdotH );

					auto nom = writer.declLocale( cuT( "nom" )
						, a2 );
					auto denom = writer.declLocale( cuT( "denom" )
						, ( NdotH2 * writer.paren( a2 - 1.0 ) + 1.0 ) );
					denom = denom * denom * PI;

					writer.returnStmt( nom / denom );
				}
					, InVec3{ &writer, cuT( "N" ) }
					, InVec3{ &writer, cuT( "H" ) }
				, InFloat{ &writer, cuT( "roughness" ) } );

				auto radicalInverse = writer.implementFunction< Float >( cuT( "RadicalInverse_VdC" )
					, [&]( UInt const & inBits )
				{
					// From https://learnopengl.com/#!PBR/Lighting
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
					// From https://learnopengl.com/#!PBR/Lighting
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
					auto a2 = writer.declLocale( cuT( "a2" )
						, a * a );

					auto phi = writer.declLocale( cuT( "phi" )
						, 2.0_f * PI * xi.x() );
					auto cosTheta = writer.declLocale( cuT( "cosTheta" )
						, sqrt( writer.paren( 1.0 - xi.y() ) / writer.paren( 1.0 + writer.paren( a2 - 1.0 ) * xi.y() ) ) );
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
								, Float( int( size.width ) ) ); // resolution of source cubemap (per face)
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

			ashes::ShaderStageStateArray program
			{
				{ getCurrentDevice( renderSystem ).createShaderModule( ashes::ShaderStageFlag::eVertex ) },
				{ getCurrentDevice( renderSystem ).createShaderModule( ashes::ShaderStageFlag::eFragment ) }
			};
			program[0].module->loadShader( compileGlslToSpv( getCurrentDevice( renderSystem )
				, ashes::ShaderStageFlag::eVertex
				, vtx.getSource() ) );
			program[1].module->loadShader( compileGlslToSpv( getCurrentDevice( renderSystem )
				, ashes::ShaderStageFlag::eFragment
				, pxl.getSource() ) );
			return program;
		}

		ashes::RenderPassPtr doCreateRenderPass( RenderSystem const & renderSystem
			, ashes::Format format )
		{
			auto & device = getCurrentDevice( renderSystem );
			ashes::RenderPassCreateInfo createInfo{};
			createInfo.flags = 0u;

			createInfo.attachments.resize( 1u );
			createInfo.attachments[0].format = format;
			createInfo.attachments[0].samples = ashes::SampleCountFlag::e1;
			createInfo.attachments[0].loadOp = ashes::AttachmentLoadOp::eClear;
			createInfo.attachments[0].storeOp = ashes::AttachmentStoreOp::eStore;
			createInfo.attachments[0].stencilLoadOp = ashes::AttachmentLoadOp::eDontCare;
			createInfo.attachments[0].stencilStoreOp = ashes::AttachmentStoreOp::eDontCare;
			createInfo.attachments[0].initialLayout = ashes::ImageLayout::eUndefined;
			createInfo.attachments[0].finalLayout = ashes::ImageLayout::eShaderReadOnlyOptimal;

			ashes::AttachmentReference colourReference;
			colourReference.attachment = 0u;
			colourReference.layout = ashes::ImageLayout::eColourAttachmentOptimal;

			createInfo.subpasses.resize( 1u );
			createInfo.subpasses[0].flags = 0u;
			createInfo.subpasses[0].colorAttachments = { colourReference };

			createInfo.dependencies.resize( 1u );
			createInfo.dependencies[0].srcSubpass = 0u;
			createInfo.dependencies[0].dstSubpass = ashes::ExternalSubpass;
			createInfo.dependencies[0].srcAccessMask = ashes::AccessFlag::eColourAttachmentWrite;
			createInfo.dependencies[0].dstAccessMask = ashes::AccessFlag::eShaderRead;
			createInfo.dependencies[0].srcStageMask = ashes::PipelineStageFlag::eColourAttachmentOutput;
			createInfo.dependencies[0].dstStageMask = ashes::PipelineStageFlag::eFragmentShader;
			createInfo.dependencies[0].dependencyFlags = ashes::DependencyFlag::eByRegion;

			return device.createRenderPass( createInfo );
		}
	}

	//*********************************************************************************************

	EnvironmentPrefilter::MipRenderCube::MipRenderCube( RenderSystem & renderSystem
		, ashes::RenderPass const & renderPass
		, uint32_t mipLevel
		, ashes::Extent2D const & originalSize
		, ashes::Extent2D const & size
		, ashes::TextureView const & srcView
		, ashes::Texture const & dstTexture
		, SamplerSPtr sampler )
		: RenderCube{ renderSystem, false, std::move( sampler ) }
		, m_device{ getCurrentDevice( renderSystem ) }
		, m_renderPass{ renderPass }
		, m_commandBuffer{ m_device.getGraphicsCommandPool().createCommandBuffer() }
		, m_fence{ m_device.createFence( ashes::FenceCreateFlag::eSignaled ) }
	{
		for ( auto face = 0u; face < 6u; ++face )
		{
			auto & facePass = m_frameBuffers[face];
			// Create the views.
			facePass.dstView = dstTexture.createView( ashes::TextureViewType::e2D
				, dstTexture.getFormat()
				, mipLevel
				, 1u
				, face
				, 1u );
			// Initialise the frame buffer.
			ashes::FrameBufferAttachmentArray attaches;
			attaches.emplace_back( *( renderPass.getAttachments().begin() ), *facePass.dstView );
			facePass.frameBuffer = renderPass.createFrameBuffer( ashes::Extent2D{ size.width, size.height }
			, std::move( attaches ) );
		}

		createPipelines( size
			, Position{}
			, doCreateProgram( renderSystem, originalSize, mipLevel )
			, srcView
			, renderPass
			, {} );
	}

	void EnvironmentPrefilter::MipRenderCube::registerFrames()
	{
		m_commandBuffer->begin();

		for ( uint32_t face = 0u; face < 6u; ++face )
		{
			auto & frameBuffer = m_frameBuffers[face];
			m_commandBuffer->beginRenderPass( m_renderPass
				, *frameBuffer.frameBuffer
				, { ashes::ClearColorValue{ 0, 0, 0, 0 } }
				, ashes::SubpassContents::eInline );
			registerFrame( *m_commandBuffer, face );
			m_commandBuffer->endRenderPass();
		}

		m_commandBuffer->end();
	}

	void EnvironmentPrefilter::MipRenderCube::render()
	{
		m_fence->reset();
		m_device.getGraphicsQueue().submit( *m_commandBuffer, m_fence.get() );
		m_fence->wait( ashes::FenceTimeout );
		m_device.waitIdle();
	}

	//*********************************************************************************************

	EnvironmentPrefilter::EnvironmentPrefilter( Engine & engine
		, castor::Size const & size
		, ashes::Texture const & srcTexture
		, SamplerSPtr sampler )
		: m_renderSystem{ *engine.getRenderSystem() }
		, m_srcView{ srcTexture.createView( ashes::TextureViewType::eCube, srcTexture.getFormat(), 0u, srcTexture.getMipmapLevels(), 0u, 6u ) }
		, m_result{ doCreatePrefilteredTexture( m_renderSystem, size ) }
		, m_resultView{ m_result->createView( ashes::TextureViewType::eCube, m_result->getFormat(), 0u, m_result->getMipmapLevels(), 0u, 6u ) }
		, m_sampler{ doCreateSampler( engine, m_result->getMipmapLevels() - 1u ) }
		, m_renderPass{ doCreateRenderPass( m_renderSystem, m_result->getFormat() ) }
	{
		auto & dstTexture = *m_result;
		ashes::Extent2D originalSize{ size.getWidth(), size.getHeight() };

		for ( auto mipLevel = 0u; mipLevel < glsl::Utils::MaxIblReflectionLod + 1u; ++mipLevel )
		{
			ashes::Extent2D mipSize{ uint32_t( originalSize.width * std::pow( 0.5, mipLevel ) )
				, uint32_t( originalSize.height * std::pow( 0.5, mipLevel ) ) };
			m_renderPasses.emplace_back( std::make_unique< MipRenderCube >( m_renderSystem
				, *m_renderPass
				, mipLevel
				, originalSize
				, mipSize
				, *m_srcView
				, dstTexture
				, sampler ) );
		}

		for ( auto & cubePass : m_renderPasses )
		{
			cubePass->registerFrames();
		}
	}

	void EnvironmentPrefilter::render()
	{
		for ( auto & cubePass : m_renderPasses )
		{
			cubePass->render();
		}
	}

	//*********************************************************************************************
}
