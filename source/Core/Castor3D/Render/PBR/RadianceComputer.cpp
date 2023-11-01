#include "Castor3D/Render/PBR/RadianceComputer.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Miscellaneous/DebugName.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/Graphics/Size.hpp>
#include <CastorUtils/Math/Angle.hpp>

#include <ashespp/Image/Image.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>
#include <ashespp/Sync/Fence.hpp>

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/TraditionalGraphicsWriter.hpp>

#include <RenderGraph/ResourceHandler.hpp>

namespace castor3d
{
	//*********************************************************************************************

	namespace radcomp
	{
		static Texture doCreateRadianceTexture( RenderDevice const & device
			, crg::ResourcesCache & resources
			, castor::Size const & size )
		{
			Texture result{ device
				, resources
				, "RadianceComputerResult"
				, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT
				, { size[0], size[1], 1u }
				, 6u
				, 1u
				, VK_FORMAT_R32G32B32A32_SFLOAT
				, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT };
			result.create();
			return result;
		}

		static SamplerObs doCreateSampler( Engine & engine
			, RenderDevice const & device )
		{
			auto name = cuT( "IblTexturesRadiance" );
			auto result = engine.tryFindSampler( name );

			if ( !result )
			{
				auto created = engine.createSampler( name, engine );
				created->setMinFilter( VK_FILTER_LINEAR );
				created->setMagFilter( VK_FILTER_LINEAR );
				created->setWrapS( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
				created->setWrapT( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
				created->setWrapR( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
				result = engine.addSampler( name, created, false );
			}

			result->initialise( engine.getRenderSystem()->getRenderDevice() );
			return result;
		}

		static ashes::ImageView doCreateSrcView( ashes::Image const & texture )
		{
			return texture.createView( VK_IMAGE_VIEW_TYPE_CUBE
				, texture.getFormat()
				, 0u
				, 1u
				, 0u
				, 6u );
		}

		static ashes::PipelineShaderStageCreateInfoArray doCreateProgram( RenderDevice const & device )
		{
			ProgramModule module{ "RadianceCompute" };
			{
				sdw::TraditionalGraphicsWriter writer{ &device.renderSystem.getEngine()->getShaderAllocator() };

				auto matrix = writer.declUniformBuffer( "Matrix", 0u, 0u );
				auto c3d_viewProjection = matrix.declMember< sdw::Mat4 >( "c3d_viewProjection" );
				matrix.end();
				auto c3d_mapEnvironment = writer.declCombinedImg< FImgCubeRgba32 >( "c3d_mapEnvironment", 1u, 0u );

				// Inputs
				auto inPosition = writer.declInput< sdw::Vec3 >( "inPosition", sdw::EntryPoint::eVertex, 0u );
				auto inWorldPosition = writer.declInput< sdw::Vec3 >( "inWorldPosition", sdw::EntryPoint::eFragment, 0u );

				// Outputs
				auto outWorldPosition = writer.declOutput< sdw::Vec3 >( "outWorldPosition", sdw::EntryPoint::eVertex, 0u );
				auto outColour = writer.declOutput< sdw::Vec4 >( "outColour", sdw::EntryPoint::eFragment, 0u );

				writer.implementEntryPointT< sdw::VoidT, sdw::VoidT >( [&]( sdw::VertexIn in
					, sdw::VertexOut out )
					{
						outWorldPosition = inPosition;
						out.vtx.position = ( c3d_viewProjection * vec4( inPosition, 1.0_f ) ).xyww();
					} );

				writer.implementEntryPointT< sdw::VoidT, sdw::VoidT >( [&]( sdw::FragmentIn in
					, sdw::FragmentOut out )
					{
						// From https://learnopengl.com/#!PBR/Lighting
						// the sample direction equals the hemisphere's orientation 
						auto normal = writer.declLocale( "normal"
							, normalize( inWorldPosition ) );

						auto irradiance = writer.declLocale( "irradiance"
							, vec3( 0.0_f ) );

						auto up = writer.declLocale( "up"
							, vec3( 0.0_f, 1.0_f, 0.0_f ) );
						auto right = writer.declLocale( "right"
							, cross( up, normal ) );
						up = cross( normal, right );

						auto sampleDelta = writer.declLocale( "sampleDelta"
							, 0.025_f );
						auto nrSamples = writer.declLocale( "nrSamples"
							, 0_i );

						FOR( writer, sdw::Float, phi, 0.0_f, phi < sdw::Float{ castor::PiMult2< float > }, phi += sampleDelta )
						{
							FOR( writer, sdw::Float, theta, 0.0_f, theta < sdw::Float{ castor::PiDiv2< float > }, theta += sampleDelta )
							{
								// spherical to cartesian (in tangent space)
								auto tangentSample = writer.declLocale( "tangentSample"
									, vec3( sin( theta ) * cos( phi ), sin( theta ) * sin( phi ), cos( theta ) ) );
								// tangent space to world
								auto sampleVec = writer.declLocale( "sampleVec"
									, right * tangentSample.x() + up * tangentSample.y() + normal * tangentSample.z() );

								irradiance += c3d_mapEnvironment.lod( sampleVec, 0.0_f ).rgb() * cos( theta ) * sin( theta );
								nrSamples = nrSamples + 1;
							}
							ROF;
						}
						ROF;

						irradiance = irradiance * sdw::Float{ castor::Pi< float > } * ( 1.0_f / writer.cast< sdw::Float >( nrSamples ) );
						outColour = vec4( irradiance, 1.0_f );
					} );

				module.shader = std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
			}

			return makeProgramStates( device, module );
		}

		static ashes::RenderPassPtr doCreateRenderPass( RenderDevice const & device
			, VkFormat format )
		{
			ashes::VkAttachmentDescriptionArray attaches
			{
				{
					0u,
					format,
					VK_SAMPLE_COUNT_1_BIT,
					VK_ATTACHMENT_LOAD_OP_DONT_CARE,
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
			auto result = device->createRenderPass( "RadianceComputer"
				, std::move( createInfo ) );
			return result;
		}
	}

	//*********************************************************************************************

	RadianceComputer::RadianceComputer( Engine & engine
		, RenderDevice const & device
		, castor::Size const & size
		, Texture const & srcTexture )
		: RenderCube{ device, false }
		, m_result{ radcomp::doCreateRadianceTexture( m_device, *srcTexture.resources, size ) }
		, m_sampler{ radcomp::doCreateSampler( engine, m_device ) }
		, m_srcView{ srcTexture }
		, m_srcImage{ m_srcView.image.get() }
		, m_srcImageView{ radcomp::doCreateSrcView( *m_srcImage ) }
		, m_renderPass{ radcomp::doCreateRenderPass( m_device, m_result.getFormat() ) }
		, m_commands{ m_device, *m_device.graphicsData(), "RadianceComputer" }
	{
		auto & dstTexture = m_result;
		auto & context = m_device.makeContext();
		
		for ( auto face = 0u; face < 6u; ++face )
		{
			auto & facePass = m_renderPasses[face];
			auto name = "RadianceComputer" + castor::string::toString( face );
			// Create the views.
			facePass.dstView = dstTexture.resources->createImageView( context, dstTexture.subViewsId[face] );
			// Initialise the frame buffer.
			auto createInfo = makeVkStruct< VkFramebufferCreateInfo >( 0u
				, *m_renderPass
				, 1u
				, &facePass.dstView
				, size.getWidth()
				, size.getHeight()
				, 1u );
			facePass.frameBuffer = m_renderPass->createFrameBuffer( name
				, std::move( createInfo ) );
		}

		auto program = radcomp::doCreateProgram( m_device );
		createPipelines( { size.getWidth(), size.getHeight() }
			, castor::Position{}
			, program
			, m_srcImageView
			, *m_renderPass
			, {} );

		auto & cmd = *m_commands.commandBuffer;
		cmd.begin();
		cmd.beginDebugBlock( { "Generating irradiance map"
			, makeFloatArray( m_device.renderSystem.getEngine()->getNextRainbowColour() ) } );

		for ( auto face = 0u; face < 6u; ++face )
		{
			auto & facePass = m_renderPasses[face];
			cmd.beginRenderPass( *m_renderPass
				, *facePass.frameBuffer
				, { transparentBlackClearColor }
				, VK_SUBPASS_CONTENTS_INLINE );
			registerFrame( *m_commands.commandBuffer, face );
			cmd.endRenderPass();
		}

		cmd.endDebugBlock();
		cmd.end();
	}

	RadianceComputer::~RadianceComputer()
	{
		auto & dstTexture = m_result;

		for ( auto face = 0u; face < 6u; ++face )
		{
			m_renderPasses[face].frameBuffer.reset();
			dstTexture.resources->destroyImageView( dstTexture.subViewsId[face] );
		}

		m_result.destroy();
	}

	void RadianceComputer::render( QueueData const & queueData )
	{
		m_commands.submit( *queueData.queue );
	}

	crg::SemaphoreWaitArray RadianceComputer::render( crg::SemaphoreWaitArray signalsToWait
		, ashes::Queue const & queue )const
	{
		return { 1u, { m_commands.submit( queue, signalsToWait )
			, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT } };
	}

	ashes::Sampler const & RadianceComputer::getSampler()const
	{
		return m_sampler->getSampler();
	}

	//*********************************************************************************************
}
