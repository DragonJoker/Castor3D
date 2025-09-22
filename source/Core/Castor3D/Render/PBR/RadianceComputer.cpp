#include "Castor3D/Render/PBR/RadianceComputer.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Miscellaneous/DebugName.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Shaders/GlslBaseIO.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/Graphics/Size.hpp>
#include <CastorUtils/Math/Angle.hpp>

#include <ashespp/Image/Image.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>
#include <ashespp/Sync/Fence.hpp>

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/TraditionalGraphicsWriter.hpp>

#include <RenderGraph/ResourceHandler.hpp>

namespace c3d
{
	//*********************************************************************************************

	namespace radcomp
	{
		static Texture doCreateRadianceTexture( RenderDevice const & device
			, crg::ResourcesCache & resources
			, Size const & size )
		{
			Texture result{ device
				, resources
				, cuT( "RadianceComputerResult" )
				, { ImageCreateFlags::eCubeCompatible
					, { size[0], size[1], 1u }, 6u, 1u
					, PixelFormat::eR32G32B32A32_SFLOAT
					, ImageUsageFlags::eColorAttachment | ImageUsageFlags::eSampled }
				, {} };
			result.create();
			return result;
		}

		static SamplerObs doCreateSampler( Engine & engine )
		{
			auto name = cuT( "IblTexturesRadiance" );
			auto result = engine.tryFindSampler( name );

			if ( !result )
			{
				auto created = engine.createSampler( name, engine );
				created->setMinFilter( FilterMode::eLinear );
				created->setMagFilter( FilterMode::eLinear );
				created->setWrapS( WrapMode::eClampToEdge );
				created->setWrapT( WrapMode::eClampToEdge );
				created->setWrapR( WrapMode::eClampToEdge );
				created->setSerialisable( false );
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

		template< ast::type::ImageFormat FormatT >
		static ashes::PipelineShaderStageCreateInfoArray doCreateProgram( RenderDevice const & device )
		{
			ProgramModule programModule{ cuT( "RadianceCompute" ) };
			{
				sdw::TraditionalGraphicsWriter writer{ &device.renderSystem.getEngine()->getShaderAllocator() };

				auto matrix = writer.declUniformBuffer( "Matrix", 0u, 0u );
				auto c3d_viewProjection = matrix.declMember< sdw::Mat4 >( "c3d_viewProjection" );
				matrix.end();
				auto c3d_mapEnvironment = writer.declCombinedImg< FormatT, ImgCube >( "c3d_mapEnvironment", 1u, 0u );

				writer.implementEntryPointT< shader::Position3FT, shader::Position3FT >( [&c3d_viewProjection]( sdw::VertexInT< shader::Position3FT > const & in
					, sdw::VertexOutT< shader::Position3FT > out )
					{
						out.position() = in.position();
						out.vtx.position = ( c3d_viewProjection * vec4( in.position(), 1.0_f ) ).xyww();
					} );

				writer.implementEntryPointT< shader::Position3FT, shader::Colour4FT >( [&writer, &c3d_mapEnvironment]( sdw::FragmentInT< shader::Position3FT > const & in
					, sdw::FragmentOutT< shader::Colour4FT > const & out )
					{
						// From https://learnopengl.com/#!PBR/Lighting
						// the sample direction equals the hemisphere's orientation 
						auto normal = writer.declLocale( "normal"
							, normalize( in.position() ) );

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

						sdwFOR( writer, sdw::Float, phi, 0.0_f, phi < sdw::Float{ PiMult2< float > }, phi += sampleDelta )
						{
							sdwFOR( writer, sdw::Float, theta, 0.0_f, theta < sdw::Float{ PiDiv2< float > }, theta += sampleDelta )
							{
								// spherical to cartesian (in tangent space)
								auto tangentSample = writer.declLocale( "tangentSample"
									, vec3( sin( theta ) * cos( phi ), sin( theta ) * sin( phi ), cos( theta ) ) );
								// tangent space to world
								auto sampleVec = writer.declLocale( "sampleVec"
									, right * tangentSample.x() + up * tangentSample.y() + normal * tangentSample.z() );

								irradiance += shader::makeVec3( writer, c3d_mapEnvironment.lod( sampleVec, 0.0_f ) ) * cos( theta ) * sin( theta );
								nrSamples = nrSamples + 1;
							}
							sdwROF
						}
						sdwROF

						irradiance = irradiance * sdw::Float{ Pi< float > } * ( 1.0_f / writer.cast< sdw::Float >( nrSamples ) );
						out.colour() = vec4( irradiance, 1.0_f );
					} );

				programModule.shader = writer.getBuilder().releaseShader();
			}

			return makeProgramStates( device, programModule );
		}

		static ashes::RenderPassPtr doCreateRenderPass( RenderDevice const & device
			, PixelFormat format )
		{
			ashes::VkAttachmentDescriptionArray attaches
			{
				{
					0u,
					convert( format ),
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
				c3d::move( attaches ),
				c3d::move( subpasses ),
				c3d::move( dependencies ),
			};
			auto result = device->createRenderPass( "RadianceComputer"
				, c3d::move( createInfo ) );
			return result;
		}
	}

	//*********************************************************************************************

	RadianceComputer::RadianceComputer( Engine & engine
		, RenderDevice const & device
		, Size const & size
		, Texture const & srcTexture )
		: RenderCube{ device, false }
		, m_result{ radcomp::doCreateRadianceTexture( m_device, *srcTexture.resources, size ) }
		, m_sampler{ radcomp::doCreateSampler( engine ) }
		, m_srcView{ srcTexture }
		, m_srcImage{ m_srcView.image.get() }
		, m_srcImageView{ radcomp::doCreateSrcView( *m_srcImage ) }
		, m_renderPass{ radcomp::doCreateRenderPass( m_device, m_result.getFormat() ) }
		, m_commands{ m_device, *m_device.graphicsData(), cuT( "RadianceComputer" ) }
	{
		auto & dstTexture = m_result;
		auto & context = m_device.makeContext();
		
		for ( auto face = 0u; face < 6u; ++face )
		{
			auto & facePass = m_renderPasses[face];
			auto name = "RadianceComputer" + string::toMbString( face );
			// Create the views.
			facePass.dstView = dstTexture.resources->createImageView( context, dstTexture.getTargetViewId( face ) );
			// Initialise the frame buffer.
			auto createInfo = makeVkStruct< VkFramebufferCreateInfo >( 0u
				, *m_renderPass
				, 1u
				, &facePass.dstView
				, size.getWidth()
				, size.getHeight()
				, 1u );
			facePass.frameBuffer = m_renderPass->createFrameBuffer( name
				, c3d::move( createInfo ) );
		}

		ast::type::ImageFormat format = getImageFormat( srcTexture.getFormat() );
		ashes::PipelineShaderStageCreateInfoArray program;
		switch ( format )
		{
		case ast::type::ImageFormat::eRgba32f: program = radcomp::doCreateProgram< ast::type::ImageFormat::eRgba32f >( m_device ); break;
		case ast::type::ImageFormat::eRgba16f: program = radcomp::doCreateProgram< ast::type::ImageFormat::eRgba16f >( m_device ); break;
		case ast::type::ImageFormat::eRg32f: program = radcomp::doCreateProgram< ast::type::ImageFormat::eRg32f >( m_device ); break;
		case ast::type::ImageFormat::eRg16f: program = radcomp::doCreateProgram< ast::type::ImageFormat::eRg16f >( m_device ); break;
		case ast::type::ImageFormat::eR32f: program = radcomp::doCreateProgram< ast::type::ImageFormat::eR32f >( m_device ); break;
		case ast::type::ImageFormat::eR16f: program = radcomp::doCreateProgram< ast::type::ImageFormat::eR16f >( m_device ); break;
		case ast::type::ImageFormat::eR11fG11fB10f: program = radcomp::doCreateProgram< ast::type::ImageFormat::eR11fG11fB10f >( m_device ); break;
		case ast::type::ImageFormat::eRgba32i: program = radcomp::doCreateProgram< ast::type::ImageFormat::eRgba32i >( m_device ); break;
		case ast::type::ImageFormat::eRgba16i: program = radcomp::doCreateProgram< ast::type::ImageFormat::eRgba16i >( m_device ); break;
		case ast::type::ImageFormat::eRgba8i: program = radcomp::doCreateProgram< ast::type::ImageFormat::eRgba8i >( m_device ); break;
		case ast::type::ImageFormat::eRg32i: program = radcomp::doCreateProgram< ast::type::ImageFormat::eRg32i >( m_device ); break;
		case ast::type::ImageFormat::eRg16i: program = radcomp::doCreateProgram< ast::type::ImageFormat::eRg16i >( m_device ); break;
		case ast::type::ImageFormat::eRg8i: program = radcomp::doCreateProgram< ast::type::ImageFormat::eRg8i >( m_device ); break;
		case ast::type::ImageFormat::eR32i: program = radcomp::doCreateProgram< ast::type::ImageFormat::eR32i >( m_device ); break;
		case ast::type::ImageFormat::eR16i: program = radcomp::doCreateProgram< ast::type::ImageFormat::eR16i >( m_device ); break;
		case ast::type::ImageFormat::eR8i: program = radcomp::doCreateProgram< ast::type::ImageFormat::eR8i >( m_device ); break;
		case ast::type::ImageFormat::eRgba32u: program = radcomp::doCreateProgram< ast::type::ImageFormat::eRgba32u >( m_device ); break;
		case ast::type::ImageFormat::eRgba16u: program = radcomp::doCreateProgram< ast::type::ImageFormat::eRgba16u >( m_device ); break;
		case ast::type::ImageFormat::eRgba8u: program = radcomp::doCreateProgram< ast::type::ImageFormat::eRgba8u >( m_device ); break;
		case ast::type::ImageFormat::eRg32u: program = radcomp::doCreateProgram< ast::type::ImageFormat::eRg32u >( m_device ); break;
		case ast::type::ImageFormat::eRg16u: program = radcomp::doCreateProgram< ast::type::ImageFormat::eRg16u >( m_device ); break;
		case ast::type::ImageFormat::eRg8u: program = radcomp::doCreateProgram< ast::type::ImageFormat::eRg8u >( m_device ); break;
		case ast::type::ImageFormat::eR32u: program = radcomp::doCreateProgram< ast::type::ImageFormat::eR32u >( m_device ); break;
		case ast::type::ImageFormat::eR16u: program = radcomp::doCreateProgram< ast::type::ImageFormat::eR16u >( m_device ); break;
		case ast::type::ImageFormat::eR8u: program = radcomp::doCreateProgram< ast::type::ImageFormat::eR8u >( m_device ); break;
		case ast::type::ImageFormat::eRgb10A2u: program = radcomp::doCreateProgram< ast::type::ImageFormat::eRgb10A2u >( m_device ); break;
		case ast::type::ImageFormat::eRgba16Snorm: program = radcomp::doCreateProgram< ast::type::ImageFormat::eRgba16Snorm >( m_device ); break;
		case ast::type::ImageFormat::eRgba8Snorm: program = radcomp::doCreateProgram< ast::type::ImageFormat::eRgba8Snorm >( m_device ); break;
		case ast::type::ImageFormat::eRg16Snorm: program = radcomp::doCreateProgram< ast::type::ImageFormat::eRg16Snorm >( m_device ); break;
		case ast::type::ImageFormat::eRg8Snorm: program = radcomp::doCreateProgram< ast::type::ImageFormat::eRg8Snorm >( m_device ); break;
		case ast::type::ImageFormat::eR16Snorm: program = radcomp::doCreateProgram< ast::type::ImageFormat::eR16Snorm >( m_device ); break;
		case ast::type::ImageFormat::eR8Snorm: program = radcomp::doCreateProgram< ast::type::ImageFormat::eR8Snorm >( m_device ); break;
		case ast::type::ImageFormat::eRgba16Unorm: program = radcomp::doCreateProgram< ast::type::ImageFormat::eRgba16Unorm >( m_device ); break;
		case ast::type::ImageFormat::eRgba8Unorm: program = radcomp::doCreateProgram< ast::type::ImageFormat::eRgba8Unorm >( m_device ); break;
		case ast::type::ImageFormat::eRg16Unorm: program = radcomp::doCreateProgram< ast::type::ImageFormat::eRg16Unorm >( m_device ); break;
		case ast::type::ImageFormat::eRg8Unorm: program = radcomp::doCreateProgram< ast::type::ImageFormat::eRg8Unorm >( m_device ); break;
		case ast::type::ImageFormat::eR16Unorm: program = radcomp::doCreateProgram< ast::type::ImageFormat::eR16Unorm >( m_device ); break;
		case ast::type::ImageFormat::eR8Unorm: program = radcomp::doCreateProgram< ast::type::ImageFormat::eR8Unorm >( m_device ); break;
		case ast::type::ImageFormat::eRgb10A2Unorm: program = radcomp::doCreateProgram< ast::type::ImageFormat::eRgb10A2Unorm >( m_device ); break;
		default:
			CU_Failure( "Unsupported ImageFormat" );
			break;
		}
		createPipelines( { size.getWidth(), size.getHeight() }
			, program
			, m_srcImageView
			, *m_renderPass
			, {} );

		auto const & cmd = *m_commands.commandBuffer;
		cmd.begin();
		cmd.beginDebugBlock( { "Generating irradiance map"
			, makeFloatArray( m_device.renderSystem.getEngine()->getNextRainbowColour() ) } );

		auto clearColor = convert( ClearValue{ transparentBlackClearColor } );
		for ( auto face = 0u; face < 6u; ++face )
		{
			auto const & facePass = m_renderPasses[face];
			cmd.beginRenderPass( *m_renderPass
				, *facePass.frameBuffer
				, { clearColor }
				, VK_SUBPASS_CONTENTS_INLINE );
			registerFrame( *m_commands.commandBuffer, face );
			cmd.endRenderPass();
		}

		cmd.endDebugBlock();
		cmd.end();
	}

	RadianceComputer::~RadianceComputer()noexcept
	{
		for ( auto face = 0u; face < 6u; ++face )
			m_renderPasses[face].frameBuffer.reset();

		m_result.destroy();
	}

	void RadianceComputer::render( QueueData const & queueData )const
	{
		m_commands.submit( *queueData.queue );
	}

	SemaphoreWaitArray RadianceComputer::render( SemaphoreWaitArray const & signalsToWait
		, ashes::Queue const & queue )const
	{
		return { 1u, { m_commands.submit( queue, signalsToWait )
			, PipelineStageFlags::eColorAttachmentOutput } };
	}

	ashes::Sampler const & RadianceComputer::getSampler()const noexcept
	{
		return m_sampler->getSampler();
	}

	//*********************************************************************************************
}
