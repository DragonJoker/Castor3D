#include "Castor3D/Render/PBR/EnvironmentPrefilter.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Limits.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslBRDFHelpers.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/Graphics/Size.hpp>
#include <CastorUtils/Math/Angle.hpp>

#include <ashespp/Image/Image.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>
#include <ashespp/Sync/Fence.hpp>

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/TraditionalGraphicsWriter.hpp>
#include <ShaderWriter/CompositeTypes/IOStructHelper.hpp>
#include <ShaderWriter/CompositeTypes/IOStructInstanceHelper.hpp>

#include <RenderGraph/ResourceHandler.hpp>

namespace castor3d
{
	//*********************************************************************************************

	namespace envpref
	{
		static Texture doCreatePrefilteredTexture( RenderDevice const & device
			, crg::ResourcesCache & resources
			, castor::Size const & size
			, castor::String const & prefix )
		{
			Texture result{ device
				, resources
				, prefix + cuT( "EnvironmentPrefilterResult" )
				, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT
				, { size[0], size[1], 1u }
				, 6u
				, MaxIblReflectionLod + 1u
				, VK_FORMAT_R32G32B32A32_SFLOAT
				, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT };
			result.create();
			return result;
		}

		static SamplerObs doCreateSampler( Engine & engine
			, RenderDevice const & device
			, castor::String const & prefix
			, uint32_t maxLod )
		{
			SamplerObs result{};
			auto stream = castor::makeStringStream();
			stream << prefix << cuT( "IblTexturesPrefiltered_" ) << maxLod;

			if ( auto name = stream.str();
				engine.hasSampler( name ) )
			{
				result = engine.findSampler( name );
			}
			else
			{
				auto created = engine.createSampler( name, engine );
				created->setMinFilter( VK_FILTER_LINEAR );
				created->setMagFilter( VK_FILTER_LINEAR );
				created->setMipFilter( VK_SAMPLER_MIPMAP_MODE_LINEAR );
				created->setWrapS( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
				created->setWrapT( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
				created->setWrapR( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
				created->setMinLod( 0.0f );
				created->setMaxLod( float( maxLod ) );
				result = engine.addSampler( name, created, false );
			}

			result->initialise( device );
			return result;
		}

		template< sdw::var::Flag FlagT >
		using PosColStructT = sdw::IOStructInstanceHelperT< FlagT
			, "PosCol"
			, sdw::IOVec4Field< "value", 0u > >;

		template< sdw::var::Flag FlagT >
		struct PosColT
			: public PosColStructT< FlagT >
		{
			PosColT( sdw::ShaderWriter & writer
				, sdw::expr::ExprPtr expr
				, bool enabled = true )
				: PosColStructT< FlagT >{ writer, castor::move( expr ), enabled }
			{
			}

			auto position()const { return this->template getMember< "value" >(); }
			auto colour()const { return this->template getMember< "value" >(); }
		};

		static ashes::PipelineShaderStageCreateInfoArray doCreateProgram( RenderDevice const & device
			, VkExtent2D const & size
			, uint32_t mipLevel
			, bool isCharlie )
		{
			castor::String prefix = isCharlie ? castor::String{ cuT( "Sheen" ) } : castor::String{};
			ProgramModule program{ prefix + cuT( "EnvironmentPrefilter" ) };
			{
				sdw::TraditionalGraphicsWriter writer{ &device.renderSystem.getEngine()->getShaderAllocator() };

				shader::BRDFHelpers brdf{ writer };

				auto matrix = writer.declUniformBuffer( "Matrix", { 0u, 0u } );
				auto c3d_viewProjection = matrix.declMember< sdw::Mat4 >( "c3d_viewProjection" );
				matrix.end();

				auto c3d_mapEnvironment = writer.declCombinedImg< FImgCubeRgba32 >( "c3d_mapEnvironment", 1u, 0u );

				auto c3d_roughness = writer.declConstant< sdw::Float >( "c3d_roughness"
					, writer.cast< sdw::Float >( float( mipLevel ) / float( MaxIblReflectionLod ) ) );
				auto c3d_sampleCount = writer.declConstant( "sampleCount"
					, 1024_u );

				writer.implementEntryPointT< PosColT, PosColT >( [&]( sdw::VertexInT< PosColT > const & in
					, sdw::VertexOutT< PosColT > out )
					{
						out.position() = in.position();
						out.vtx.position = ( c3d_viewProjection * vec4( in.position().xyz(), 1.0_f ) ).xyww();
					} );

				writer.implementEntryPointT< PosColT, PosColT >( [&]( sdw::FragmentInT< PosColT > const & in
					, sdw::FragmentOutT< PosColT > const & out )
					{
						// From https://learnopengl.com/#!PBR/Lighting
						auto N = writer.declLocale( "N"
							, normalize( in.position().xyz() ) );
						auto R = writer.declLocale( "R"
							, N );
						auto V = writer.declLocale( "V"
							, R );
						auto totalWeight = writer.declLocale( "totalWeight"
							, 0.0_f );
						auto prefilteredColor = writer.declLocale( "prefilteredColor"
							, vec3( 0.0_f ) );

						FOR( writer, sdw::UInt, i, 0_u, i < c3d_sampleCount, ++i )
						{
							auto xi = writer.declLocale( "xi"
								, brdf.hammersley( i, c3d_sampleCount ) );
							auto importanceSample = writer.declLocale( "importanceSample"
								, ( isCharlie
									? brdf.getImportanceSample( brdf.importanceSampleCharlie( xi, c3d_roughness ), N )
									: brdf.getImportanceSample( brdf.importanceSampleGGX( xi, c3d_roughness ), N ) ) );
							auto H = writer.declLocale( "H"
								, importanceSample.xyz() );

							auto L = writer.declLocale( "L"
								, normalize( vec3( 2.0_f ) * dot( V, H ) * H - V ) );
							auto NdotL = writer.declLocale( "NdotL"
								, dot( N, L ) );

							IF( writer, NdotL > 0.0_f )
							{
								auto pdf = writer.declLocale( "pdf"
									, importanceSample.w() );
								auto resolution = sdw::Float{ float( size.width ) };
								auto omegaP = writer.declLocale( "omegaP"
									, ( 4.0f * castor::Pi< float > ) / ( 6.0_f * resolution * resolution ) );
								auto omegaS = writer.declLocale( "omegaS"
									, 1.0_f / ( writer.cast< sdw::Float >( c3d_sampleCount ) * pdf + 0.0001_f ) );
								auto lod = writer.declLocale( "lod"
									, writer.ternary( c3d_roughness == 0.0_f
										, 0.0_f
										, 0.5_f * log2( omegaS / omegaP ) ) );

								prefilteredColor += c3d_mapEnvironment.lod( L, lod ).rgb() * NdotL;
								totalWeight += NdotL;
							}
							FI
						}
						ROF

						IF( writer, totalWeight != 0.0f )
						{
							prefilteredColor /= totalWeight;
						}
						ELSE
						{
							prefilteredColor /= writer.cast< sdw::Float >( c3d_sampleCount );
						}
						FI

						out.colour() = vec4( prefilteredColor, 1.0_f );
					} );

				program.shader = writer.getBuilder().releaseShader(); 
			}

			return makeProgramStates( device, program );
		}

		static ashes::RenderPassPtr doCreateRenderPass( RenderDevice const & device
			, castor::String const & prefix
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
				castor::move( attaches ),
				castor::move( subpasses ),
				castor::move( dependencies ),
			};
			auto result = device->createRenderPass( castor::toUtf8( prefix + cuT( "EnvironmentPrefilter" ) )
				, castor::move( createInfo ) );
			return result;
		}
	}

	//*********************************************************************************************

	EnvironmentPrefilter::MipRenderCube::MipRenderCube( RenderDevice const & device
		, QueueData const & queueData
		, crg::ResourcesCache & resources
		, ashes::RenderPass const & renderPass
		, uint32_t mipLevel
		, VkExtent2D const & originalSize
		, VkExtent2D const & size
		, ashes::ImageView const & srcView
		, Texture const & dstTexture
		, SamplerObs sampler
		, bool isCharlie )
		: RenderCube{ device, false, castor::move( sampler ) }
		, m_renderPass{ renderPass }
		, m_prefix{ isCharlie ? castor::String{ cuT( "Sheen" ) } : castor::String{} }
		, m_commands{ m_device, queueData, m_prefix + cuT( "EnvironmentPrefilter" ) }
	{
		auto & handler = resources.getHandler();
		auto & context = m_device.makeContext();

		for ( auto face = 0u; face < 6u; ++face )
		{
			auto name = castor::toUtf8( m_prefix + cuT( "EnvironmentPrefilterL" ) + castor::string::toString( face ) + cuT( "M" ) + castor::string::toString( mipLevel ) );
			auto & facePass = m_frameBuffers[face];
			// Create the views.
			auto data = *dstTexture.wholeViewId.data;
			data.name = name;
			data.info.viewType = VK_IMAGE_VIEW_TYPE_2D;
			data.info.subresourceRange.baseArrayLayer = face;
			data.info.subresourceRange.layerCount = 1u;
			data.info.subresourceRange.baseMipLevel = mipLevel;
			data.info.subresourceRange.levelCount = 1u;
			auto viewId = handler.createViewId( data );
			facePass.dstView = resources.createImageView( context, viewId );
			// Initialise the frame buffer.
			auto createInfo = makeVkStruct< VkFramebufferCreateInfo >( 0u
				, renderPass
				, 1u
				, &facePass.dstView
				, size.width
				, size.height
				, 1u );
			facePass.frameBuffer = renderPass.createFrameBuffer( name
				, castor::move( createInfo ) );
		}

		createPipelines( size
			, envpref::doCreateProgram( m_device, originalSize, mipLevel, isCharlie )
			, srcView
			, renderPass
			, {} );
	}

	void EnvironmentPrefilter::MipRenderCube::registerFrames()
	{
		auto const & cmd = *m_commands.commandBuffer;
		cmd.begin();
		cmd.beginDebugBlock( { "Prefiltering " + castor::toUtf8( m_prefix ) + " Environment map"
			, makeFloatArray( m_device.renderSystem.getEngine()->getNextRainbowColour() ) } );

		for ( uint32_t face = 0u; face < 6u; ++face )
		{
			auto const & frameBuffer = m_frameBuffers[face];
			cmd.beginRenderPass( m_renderPass
				, *frameBuffer.frameBuffer
				, { transparentBlackClearColor }
				, VK_SUBPASS_CONTENTS_INLINE );
			registerFrame( *m_commands.commandBuffer, face );
			cmd.endRenderPass();
		}

		cmd.endDebugBlock();
		cmd.end();
	}

	void EnvironmentPrefilter::MipRenderCube::render( QueueData const & queueData )const
	{
		m_commands.submit( *queueData.queue );
	}

	crg::SemaphoreWaitArray EnvironmentPrefilter::MipRenderCube::render( crg::SemaphoreWaitArray const & signalsToWait
		, ashes::Queue const & queue )const
	{
		return { 1u
			, { m_commands.submit( queue, signalsToWait )
				, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT } };
	}

	//*********************************************************************************************

	EnvironmentPrefilter::EnvironmentPrefilter( Engine & engine
		, RenderDevice const & device
		, castor::Size const & size
		, Texture const & srcTexture
		, SamplerObs sampler
		, bool isCharlie )
		: m_device{ device }
		, m_srcView{ srcTexture }
		, m_prefix{ isCharlie ? castor::String{ cuT( "Sheen" ) } : castor::String{} }
		, m_srcImage{ m_srcView.image.get() }
		, m_srcImageView{ m_srcImage->createView( castor::toUtf8( m_prefix ) + "EnvironmentPrefilterSrc", VK_IMAGE_VIEW_TYPE_CUBE, m_srcView.getFormat(), 0u, m_srcView.getMipLevels(), 0u, 6u ) }
		, m_result{ envpref::doCreatePrefilteredTexture( m_device, *m_srcView.resources, size, m_prefix ) }
		, m_sampler{ envpref::doCreateSampler( engine, m_device, m_prefix, m_result.getMipLevels() - 1u ) }
		, m_renderPass{ envpref::doCreateRenderPass( m_device, m_prefix, m_result.getFormat() ) }
	{
		VkExtent2D originalSize{ size.getWidth(), size.getHeight() };
		auto data = m_device.graphicsData();

		for ( auto mipLevel = 0u; mipLevel < MaxIblReflectionLod + 1u; ++mipLevel )
		{
			VkExtent2D mipSize{ originalSize.width >> mipLevel
				, originalSize.height >> mipLevel };
			m_renderPasses.emplace_back( castor::make_unique< MipRenderCube >( m_device
				, *data
				, *m_srcView.resources
				, *m_renderPass
				, mipLevel
				, originalSize
				, mipSize
				, m_srcImageView
				, m_result
				, sampler
				, isCharlie ) );
		}

		for ( auto const & cubePass : m_renderPasses )
		{
			cubePass->registerFrames();
		}
	}

	EnvironmentPrefilter::~EnvironmentPrefilter()noexcept
	{
		m_result.destroy();
	}

	void EnvironmentPrefilter::render( QueueData const & queueData )const
	{
		for ( auto & cubePass : m_renderPasses )
		{
			cubePass->render( queueData );
		}
	}

	crg::SemaphoreWaitArray EnvironmentPrefilter::render( crg::SemaphoreWaitArray signalsToWait
		, ashes::Queue const & queue )const
	{
		for ( auto & cubePass : m_renderPasses )
		{
			signalsToWait = cubePass->render( signalsToWait, queue );
		}

		return signalsToWait;
	}

	//*********************************************************************************************
}
