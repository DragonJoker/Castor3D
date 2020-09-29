#include "Castor3D/Render/PBR/EnvironmentPrefilter.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/SamplerCache.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"

#include <CastorUtils/Graphics/Size.hpp>
#include <CastorUtils/Math/Angle.hpp>

#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>
#include <ashespp/Sync/Fence.hpp>

#include <ShaderWriter/Source.hpp>

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		ashes::ImagePtr doCreatePrefilteredTexture( RenderDevice const & device
			, Size const & size )
		{
			ashes::ImageCreateInfo image
			{
				VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT,
				VK_IMAGE_TYPE_2D,
				VK_FORMAT_R32G32B32A32_SFLOAT,
				{ size[0], size[1], 1u },
				shader::Utils::MaxIblReflectionLod + 1u,
				6u,
				VK_SAMPLE_COUNT_1_BIT,
				VK_IMAGE_TILING_OPTIMAL,
				VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			};
			return makeImage( device
				, std::move( image )
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, "EnvironmentPrefilterResult" );
		}

		SamplerSPtr doCreateSampler( Engine & engine
			, RenderDevice const & device
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
				result->setMinFilter( VK_FILTER_LINEAR );
				result->setMagFilter( VK_FILTER_LINEAR );
				result->setMipFilter( VK_SAMPLER_MIPMAP_MODE_LINEAR );
				result->setWrapS( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
				result->setWrapT( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
				result->setWrapR( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
				result->setMinLod( 0.0f );
				result->setMaxLod( float( maxLod ) );
				engine.getSamplerCache().add( name, result );
			}

			result->initialise( device );
			return result;
		}

		ashes::PipelineShaderStageCreateInfoArray doCreateProgram( RenderDevice const & device
			, VkExtent2D const & size
			, uint32_t mipLevel )
		{
			ShaderModule vtx{ VK_SHADER_STAGE_VERTEX_BIT, "EnvironmentPrefilter" };
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

			ShaderModule pxl{ VK_SHADER_STAGE_FRAGMENT_BIT, "EnvironmentPrefilter" };
			{
				using namespace sdw;
				FragmentWriter writer;

				// Inputs
				auto vtx_worldPosition = writer.declInput< Vec3 >( "vtx_worldPosition", 0u );
				auto c3d_mapEnvironment = writer.declSampledImage< FImgCubeRgba32 >( "c3d_mapEnvironment", 1u, 0u );
				auto c3d_roughness = writer.declConstant< Float >( "c3d_roughness"
					, writer.cast< Float >( mipLevel / float( shader::Utils::MaxIblReflectionLod ) ) );

				// Outputs
				auto pxl_fragColor = writer.declOutput< Vec4 >( "pxl_FragColor", 0u );

				auto distributionGGX = writer.implementFunction< Float >( "DistributionGGX"
					, [&]( Vec3 const & N
						, Vec3 const & H
						, Float const & roughness )
					{
						auto a = writer.declLocale( "a"
							, roughness * roughness );
						auto a2 = writer.declLocale( "a2"
							, a * a );
						auto NdotH = writer.declLocale( "NdotH"
							, max( dot( N, H ), 0.0_f ) );
						auto NdotH2 = writer.declLocale( "NdotH2"
							, NdotH * NdotH );

						auto nom = writer.declLocale( "nom"
							, a2 );
						auto denom = writer.declLocale( "denom"
							, fma( NdotH2, a2 - 1.0_f, 1.0_f ) );
						denom = denom * denom * Float{ Pi< float > };

						writer.returnStmt( nom / denom );
					}
					, InVec3{ writer, "N" }
					, InVec3{ writer, "H" }
					, InFloat{ writer, "roughness" } );

				auto radicalInverse = writer.implementFunction< Float >( "RadicalInverse_VdC"
					, [&]( UInt const & inBits )
				{
					// From https://learnopengl.com/#!PBR/Lighting
					auto bits = writer.declLocale( "bits"
						, inBits );
					bits = ( bits << 16_u ) | ( bits >> 16_u );
					bits = ( ( bits & 0x55555555_u ) << 1_u ) | ( ( bits & 0xAAAAAAAA_u ) >> 1_u );
					bits = ( ( bits & 0x33333333_u ) << 2_u ) | ( ( bits & 0xCCCCCCCC_u ) >> 2_u );
					bits = ( ( bits & 0x0F0F0F0F_u ) << 4_u ) | ( ( bits & 0xF0F0F0F0_u ) >> 4_u );
					bits = ( ( bits & 0x00FF00FF_u ) << 8_u ) | ( ( bits & 0xFF00FF00_u ) >> 8_u );
					writer.returnStmt( writer.cast< Float >( bits ) * 2.3283064365386963e-10_f ); // / 0x100000000
				}
				, InUInt{ writer, "inBits" } );

				auto hammersley = writer.implementFunction< Vec2 >( "Hammersley"
					, [&]( UInt const & i
						, UInt const & n )
					{
						// From https://learnopengl.com/#!PBR/Lighting
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
						auto a2 = writer.declLocale( "a2"
							, a * a );

						auto phi = writer.declLocale( "phi"
							, Float{ PiMult2< float > } * xi.x() );
						auto cosTheta = writer.declLocale( "cosTheta"
							, sqrt( ( 1.0_f - xi.y() ) / ( 1.0_f + ( a2 - 1.0_f ) * xi.y() ) ) );
						auto sinTheta = writer.declLocale( "sinTheta"
							, sqrt( 1.0_f - cosTheta * cosTheta ) );

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

				writer.implementFunction< sdw::Void >( "main"
					, [&]()
					{
						// From https://learnopengl.com/#!PBR/Lighting
						auto N = writer.declLocale( "N"
							, normalize( vtx_worldPosition ) );
						auto R = writer.declLocale( "R"
							, N );
						auto V = writer.declLocale( "V"
							, R );

						auto sampleCount = writer.declLocale( "sampleCount"
							, 1024_u );
						auto totalWeight = writer.declLocale( "totalWeight"
							, 0.0_f );
						auto prefilteredColor = writer.declLocale( "prefilteredColor"
							, vec3( 0.0_f ) );

						FOR( writer, UInt, i, 0_u, i < sampleCount, ++i )
						{
							auto xi = writer.declLocale( "xi"
								, hammersley( i, sampleCount ) );
							auto H = writer.declLocale( "H"
								, importanceSample( xi, N, c3d_roughness ) );
							auto L = writer.declLocale( "L"
								, normalize( vec3( 2.0_f ) * dot( V, H ) * H - V ) );

							auto NdotL = writer.declLocale( "NdotL"
								, max( dot( N, L ), 0.0_f ) );

							IF( writer, NdotL > 0.0_f )
							{
								auto D = writer.declLocale( "D"
									, distributionGGX( N, H, c3d_roughness ) );
								auto NdotH = writer.declLocale( "NdotH"
									, max( dot( N, H ), 0.0_f ) );
								auto HdotV = writer.declLocale( "HdotV"
									, max( dot( H, V ), 0.0_f ) );
								auto pdf = writer.declLocale( "pdf"
									, ( D * NdotH ) / ( 4.0_f * HdotV ) + 0.0001_f );

								auto resolution = writer.declLocale( "resolution"
									, Float( float( size.width ) ) ); // resolution of source cubemap (per face)
								auto saTexel = writer.declLocale( "saTexel"
									, Float{ 4.0f * Pi< float > } / ( 6.0_f * resolution * resolution ) );
								auto saSample = writer.declLocale( "saSample"
									, 1.0_f / ( writer.cast< Float >( sampleCount ) * pdf + 0.0001_f ) );
								auto mipLevel = writer.declLocale( "mipLevel"
									, writer.ternary( c3d_roughness == 0.0_f
										, 0.0_f
										, 0.5_f * log2( saSample / saTexel ) ) );

								prefilteredColor += texture( c3d_mapEnvironment, L, mipLevel ).rgb() * NdotL;
								totalWeight += NdotL;
							}
							FI;
						}
						ROF;

						prefilteredColor = prefilteredColor / totalWeight;
						pxl_fragColor = vec4( prefilteredColor, 1.0_f );
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
			auto result = device->createRenderPass( "EnvironmentPrefilter"
				, std::move( createInfo ) );
			return result;
		}
	}

	//*********************************************************************************************

	EnvironmentPrefilter::MipRenderCube::MipRenderCube( RenderDevice const & device
		, ashes::RenderPass const & renderPass
		, uint32_t mipLevel
		, VkExtent2D const & originalSize
		, VkExtent2D const & size
		, ashes::ImageView const & srcView
		, ashes::Image const & dstTexture
		, SamplerSPtr sampler )
		: RenderCube{ device, false, std::move( sampler ) }
		, m_renderPass{ renderPass }
		, m_commandBuffer{ m_device.graphicsCommandPool->createCommandBuffer( "EnvironmentPrefilter" ) }
		, m_fence{ m_device->createFence( "EnvironmentPrefilter", VK_FENCE_CREATE_SIGNALED_BIT ) }
	{
		for ( auto face = 0u; face < 6u; ++face )
		{
			auto name = "EnvironmentPrefilter" + string::toString( face );
			auto & facePass = m_frameBuffers[face];
			// Create the views.
			facePass.dstView = dstTexture.createView( name
				, VK_IMAGE_VIEW_TYPE_2D
				, dstTexture.getFormat()
				, mipLevel
				, 1u
				, face
				, 1u );
			// Initialise the frame buffer.
			ashes::ImageViewCRefArray attaches;
			attaches.emplace_back( facePass.dstView );
			facePass.frameBuffer = renderPass.createFrameBuffer( name
				, VkExtent2D{ size.width, size.height }
				, std::move( attaches ) );
		}

		createPipelines( size
			, Position{}
			, doCreateProgram( m_device, originalSize, mipLevel )
			, srcView
			, renderPass
			, {} );
	}

	void EnvironmentPrefilter::MipRenderCube::registerFrames()
	{
		m_commandBuffer->begin();
		m_commandBuffer->beginDebugBlock(
			{
				"Prefiltering Environment map",
				makeFloatArray( m_device.renderSystem.getEngine()->getNextRainbowColour() ),
			} );

		for ( uint32_t face = 0u; face < 6u; ++face )
		{
			auto & frameBuffer = m_frameBuffers[face];
			m_commandBuffer->beginRenderPass( m_renderPass
				, *frameBuffer.frameBuffer
				, { transparentBlackClearColor }
				, VK_SUBPASS_CONTENTS_INLINE );
			registerFrame( *m_commandBuffer, face );
			m_commandBuffer->endRenderPass();
		}

		m_commandBuffer->endDebugBlock();
		m_commandBuffer->end();
	}

	void EnvironmentPrefilter::MipRenderCube::render()
	{
		m_fence->reset();
		m_device.graphicsQueue->submit( *m_commandBuffer, m_fence.get() );
		m_fence->wait( ashes::MaxTimeout );
		m_device->waitIdle();
	}

	//*********************************************************************************************

	EnvironmentPrefilter::EnvironmentPrefilter( Engine & engine
		, RenderDevice const & device
		, castor::Size const & size
		, ashes::Image const & srcTexture
		, SamplerSPtr sampler )
		: m_device{ device }
		, m_srcView{ srcTexture.createView( "EnvironmentPrefilterSrc", VK_IMAGE_VIEW_TYPE_CUBE, srcTexture.getFormat(), 0u, srcTexture.getMipmapLevels(), 0u, 6u ) }
		, m_result{ doCreatePrefilteredTexture( m_device, size ) }
		, m_resultView{ m_result->createView( "EnvironmentPrefilterDst", VK_IMAGE_VIEW_TYPE_CUBE, m_result->getFormat(), 0u, m_result->getMipmapLevels(), 0u, 6u ) }
		, m_sampler{ doCreateSampler( engine, m_device, m_result->getMipmapLevels() - 1u ) }
		, m_renderPass{ doCreateRenderPass( m_device, m_result->getFormat() ) }
	{
		VkExtent2D originalSize{ size.getWidth(), size.getHeight() };

		for ( auto mipLevel = 0u; mipLevel < shader::Utils::MaxIblReflectionLod + 1u; ++mipLevel )
		{
			VkExtent2D mipSize{ uint32_t( originalSize.width * std::pow( 0.5, mipLevel ) )
				, uint32_t( originalSize.height * std::pow( 0.5, mipLevel ) ) };
			m_renderPasses.emplace_back( std::make_unique< MipRenderCube >( m_device
				, *m_renderPass
				, mipLevel
				, originalSize
				, mipSize
				, m_srcView
				, *m_result
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
