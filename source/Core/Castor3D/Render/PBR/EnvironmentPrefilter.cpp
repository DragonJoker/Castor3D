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

namespace c3d
{
	//*********************************************************************************************

	namespace envpref
	{
		static Texture doCreatePrefilteredTexture( RenderDevice const & device
			, crg::ResourcesCache & resources
			, Size const & size
			, String const & prefix )
		{
			Texture result{ device
				, resources
				, prefix + cuT( "EnvironmentPrefilterResult" )
				, { ImageCreateFlags::eCubeCompatible
					, { size[0], size[1], 1u }, 6u, MaxIblReflectionLod + 1u
					, PixelFormat::eR32G32B32A32_SFLOAT
					, ImageUsageFlags::eColorAttachment | ImageUsageFlags::eSampled }
				, {} };
			result.create();
			return result;
		}

		static SamplerObs doCreateSampler( Engine & engine
			, RenderDevice const & device
			, String const & prefix
			, uint32_t maxLod )
		{
			SamplerObs result{};
			auto stream = makeStringStream();
			stream << prefix << cuT( "IblTexturesPrefiltered_" ) << maxLod;

			if ( auto name = stream.str();
				engine.hasSampler( name ) )
			{
				result = engine.findSampler( name );
			}
			else
			{
				auto created = engine.createSampler( name, engine );
				created->setMinFilter( FilterMode::eLinear );
				created->setMagFilter( FilterMode::eLinear );
				created->setMipFilter( MipmapMode::eLinear );
				created->setWrapS( WrapMode::eClampToEdge );
				created->setWrapT( WrapMode::eClampToEdge );
				created->setWrapR( WrapMode::eClampToEdge );
				created->setMinLod( 0.0f );
				created->setMaxLod( float( maxLod ) );
				created->setSerialisable( false );
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
				: PosColStructT< FlagT >{ writer, c3d::move( expr ), enabled }
			{
			}

			auto position()const { return this->template getMember< "value" >(); }
			auto colour()const { return this->template getMember< "value" >(); }
		};

		static sdw::Vec3 makeVec3( sdw::ShaderWriter & writer, sdw::Int const in )
		{
			return vec3( writer.cast< sdw::Float >( in ) );
		}

		static sdw::Vec3 makeVec3( sdw::ShaderWriter const &, sdw::IVec2 const in )
		{
			return vec3( vec2( in ), 0.0_f );
		}

		static sdw::Vec3 makeVec3( sdw::ShaderWriter const &, sdw::IVec4 const in )
		{
			return vec3( in.xyz() );
		}

		static sdw::Vec3 makeVec3( sdw::ShaderWriter & writer, sdw::UInt const in )
		{
			return vec3( writer.cast< sdw::Float >( in ) );
		}

		static sdw::Vec3 makeVec3( sdw::ShaderWriter const &, sdw::UVec2 const in )
		{
			return vec3( vec2( in ), 0.0_f );
		}

		static sdw::Vec3 makeVec3( sdw::ShaderWriter const &, sdw::UVec4 const in )
		{
			return vec3( in.xyz() );
		}

		static sdw::Vec3 makeVec3( sdw::ShaderWriter const &, sdw::Float const in )
		{
			return vec3( in );
		}

		static sdw::Vec3 makeVec3( sdw::ShaderWriter const &, sdw::Vec2 const in )
		{
			return vec3( in, 0.0_f );
		}

		static sdw::Vec3 makeVec3( sdw::ShaderWriter const &, sdw::Vec3 const in )
		{
			return in;
		}

		static sdw::Vec3 makeVec3( sdw::ShaderWriter const &, sdw::Vec4 const in )
		{
			return in.xyz();
		}

		template< ast::type::ImageFormat FormatT >
		static ashes::PipelineShaderStageCreateInfoArray doCreateProgram( RenderDevice const & device
			, Extent2D const & size
			, uint32_t mipLevel
			, bool isCharlie )
		{
			String prefix = isCharlie ? String{ cuT( "Sheen" ) } : String{};
			ProgramModule program{ prefix + cuT( "EnvironmentPrefilter" ) };
			{
				sdw::TraditionalGraphicsWriter writer{ &device.renderSystem.getEngine()->getShaderAllocator() };

				shader::BRDFHelpers brdf{ writer };

				auto matrix = writer.declUniformBuffer( "Matrix", { 0u, 0u } );
				auto c3d_viewProjection = matrix.declMember< sdw::Mat4 >( "c3d_viewProjection" );
				matrix.end();

				auto c3d_mapEnvironment = writer.declCombinedImg< FormatT, ImgCube >( "c3d_mapEnvironment", 1u, 0u );

				auto c3d_roughness = writer.declConstant< sdw::Float >( "c3d_roughness"
					, writer.cast< sdw::Float >( float( mipLevel ) / float( MaxIblReflectionLod ) ) );
				auto c3d_sampleCount = writer.declConstant( "sampleCount"
					, 4096_u );

				writer.implementEntryPointT< PosColT, PosColT >( [&c3d_viewProjection]( sdw::VertexInT< PosColT > const & in
					, sdw::VertexOutT< PosColT > out )
					{
						out.position() = in.position();
						out.vtx.position = ( c3d_viewProjection * vec4( in.position().xyz(), 1.0_f ) ).xyww();
					} );

				writer.implementEntryPointT< PosColT, PosColT >( [&writer, &c3d_sampleCount, &c3d_roughness, &c3d_mapEnvironment, &brdf, &size, isCharlie]( sdw::FragmentInT< PosColT > const & in
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

						sdwFOR( writer, sdw::UInt, i, 0_u, i < c3d_sampleCount, ++i )
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

							sdwIF( writer, NdotL > 0.0_f )
							{
								auto pdf = writer.declLocale( "pdf"
									, importanceSample.w() );
								auto resolution = sdw::Float{ float( size.width ) };
								auto omegaP = writer.declLocale( "omegaP"
									, ( 4.0f * Pi< float > ) / ( 6.0_f * resolution * resolution ) );
								auto omegaS = writer.declLocale( "omegaS"
									, 1.0_f / ( writer.cast< sdw::Float >( c3d_sampleCount ) * pdf + 0.0001_f ) );
								auto lod = writer.declLocale( "lod"
									, writer.ternary( c3d_roughness == 0.0_f
										, 0.0_f
										, 0.5_f * log2( omegaS / omegaP ) ) );

								prefilteredColor += makeVec3( writer, c3d_mapEnvironment.lod( L, lod ) ) * NdotL;
								totalWeight += NdotL;
							}
							sdwFI
						}
						sdwROF

						sdwIF( writer, totalWeight != 0.0f )
						{
							prefilteredColor /= totalWeight;
						}
						sdwELSE
						{
							prefilteredColor /= writer.cast< sdw::Float >( c3d_sampleCount );
						}
						sdwFI

						out.colour() = vec4( prefilteredColor, 1.0_f );
					} );

				program.shader = writer.getBuilder().releaseShader(); 
			}

			return makeProgramStates( device, program );
		}

		static ashes::RenderPassPtr doCreateRenderPass( RenderDevice const & device
			, String const & prefix
			, PixelFormat format )
		{
			ashes::VkAttachmentDescriptionArray attaches
			{
				{
					0u,
					convert( format ),
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
				c3d::move( attaches ),
				c3d::move( subpasses ),
				c3d::move( dependencies ),
			};
			auto result = device->createRenderPass( toUtf8( prefix + cuT( "EnvironmentPrefilter" ) )
				, c3d::move( createInfo ) );
			return result;
		}
	}

	//*********************************************************************************************

	EnvironmentPrefilter::MipRenderCube::MipRenderCube( RenderDevice const & device
		, QueueData const & queueData
		, crg::ResourcesCache & resources
		, ashes::RenderPass const & renderPass
		, uint32_t mipLevel
		, Extent2D const & originalSize
		, Extent2D const & size
		, ashes::ImageView const & srcView
		, Texture const & dstTexture
		, SamplerObs sampler
		, bool isCharlie )
		: RenderCube{ device, false, c3d::move( sampler ) }
		, m_renderPass{ renderPass }
		, m_prefix{ isCharlie ? String{ cuT( "Sheen" ) } : String{} }
		, m_commands{ m_device, queueData, m_prefix + cuT( "EnvironmentPrefilter" ) }
	{
		auto & handler = resources.getHandler();
		auto & context = m_device.makeContext();

		for ( auto face = 0u; face < 6u; ++face )
		{
			auto name = toUtf8( m_prefix + cuT( "EnvironmentPrefilterL" ) + string::toString( face ) + cuT( "M" ) + string::toString( mipLevel ) );
			auto & facePass = m_frameBuffers[face];
			// Create the views.
			auto data = *dstTexture.getWholeViewId().data;
			data.name = name;
			data.info.viewType = ImageViewType::e2D;
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
				, c3d::move( createInfo ) );
		}

		ast::type::ImageFormat format = getImageFormat( convert( srcView.getFormat() ) );
		ashes::PipelineShaderStageCreateInfoArray program;
		switch ( format )
		{
		case ast::type::ImageFormat::eRgba32f: program = envpref::doCreateProgram< ast::type::ImageFormat::eRgba32f >( m_device, originalSize, mipLevel, isCharlie ); break;
		case ast::type::ImageFormat::eRgba16f: program = envpref::doCreateProgram< ast::type::ImageFormat::eRgba16f >( m_device, originalSize, mipLevel, isCharlie ); break;
		case ast::type::ImageFormat::eRg32f: program = envpref::doCreateProgram< ast::type::ImageFormat::eRg32f >( m_device, originalSize, mipLevel, isCharlie ); break;
		case ast::type::ImageFormat::eRg16f: program = envpref::doCreateProgram< ast::type::ImageFormat::eRg16f >( m_device, originalSize, mipLevel, isCharlie ); break;
		case ast::type::ImageFormat::eR32f: program = envpref::doCreateProgram< ast::type::ImageFormat::eR32f >( m_device, originalSize, mipLevel, isCharlie ); break;
		case ast::type::ImageFormat::eR16f: program = envpref::doCreateProgram< ast::type::ImageFormat::eR16f >( m_device, originalSize, mipLevel, isCharlie ); break;
		case ast::type::ImageFormat::eR11fG11fB10f: program = envpref::doCreateProgram< ast::type::ImageFormat::eR11fG11fB10f >( m_device, originalSize, mipLevel, isCharlie ); break;
		case ast::type::ImageFormat::eRgba32i: program = envpref::doCreateProgram< ast::type::ImageFormat::eRgba32i >( m_device, originalSize, mipLevel, isCharlie ); break;
		case ast::type::ImageFormat::eRgba16i: program = envpref::doCreateProgram< ast::type::ImageFormat::eRgba16i >( m_device, originalSize, mipLevel, isCharlie ); break;
		case ast::type::ImageFormat::eRgba8i: program = envpref::doCreateProgram< ast::type::ImageFormat::eRgba8i >( m_device, originalSize, mipLevel, isCharlie ); break;
		case ast::type::ImageFormat::eRg32i: program = envpref::doCreateProgram< ast::type::ImageFormat::eRg32i >( m_device, originalSize, mipLevel, isCharlie ); break;
		case ast::type::ImageFormat::eRg16i: program = envpref::doCreateProgram< ast::type::ImageFormat::eRg16i >( m_device, originalSize, mipLevel, isCharlie ); break;
		case ast::type::ImageFormat::eRg8i: program = envpref::doCreateProgram< ast::type::ImageFormat::eRg8i >( m_device, originalSize, mipLevel, isCharlie ); break;
		case ast::type::ImageFormat::eR32i: program = envpref::doCreateProgram< ast::type::ImageFormat::eR32i >( m_device, originalSize, mipLevel, isCharlie ); break;
		case ast::type::ImageFormat::eR16i: program = envpref::doCreateProgram< ast::type::ImageFormat::eR16i >( m_device, originalSize, mipLevel, isCharlie ); break;
		case ast::type::ImageFormat::eR8i: program = envpref::doCreateProgram< ast::type::ImageFormat::eR8i >( m_device, originalSize, mipLevel, isCharlie ); break;
		case ast::type::ImageFormat::eRgba32u: program = envpref::doCreateProgram< ast::type::ImageFormat::eRgba32u >( m_device, originalSize, mipLevel, isCharlie ); break;
		case ast::type::ImageFormat::eRgba16u: program = envpref::doCreateProgram< ast::type::ImageFormat::eRgba16u >( m_device, originalSize, mipLevel, isCharlie ); break;
		case ast::type::ImageFormat::eRgba8u: program = envpref::doCreateProgram< ast::type::ImageFormat::eRgba8u >( m_device, originalSize, mipLevel, isCharlie ); break;
		case ast::type::ImageFormat::eRg32u: program = envpref::doCreateProgram< ast::type::ImageFormat::eRg32u >( m_device, originalSize, mipLevel, isCharlie ); break;
		case ast::type::ImageFormat::eRg16u: program = envpref::doCreateProgram< ast::type::ImageFormat::eRg16u >( m_device, originalSize, mipLevel, isCharlie ); break;
		case ast::type::ImageFormat::eRg8u: program = envpref::doCreateProgram< ast::type::ImageFormat::eRg8u >( m_device, originalSize, mipLevel, isCharlie ); break;
		case ast::type::ImageFormat::eR32u: program = envpref::doCreateProgram< ast::type::ImageFormat::eR32u >( m_device, originalSize, mipLevel, isCharlie ); break;
		case ast::type::ImageFormat::eR16u: program = envpref::doCreateProgram< ast::type::ImageFormat::eR16u >( m_device, originalSize, mipLevel, isCharlie ); break;
		case ast::type::ImageFormat::eR8u: program = envpref::doCreateProgram< ast::type::ImageFormat::eR8u >( m_device, originalSize, mipLevel, isCharlie ); break;
		case ast::type::ImageFormat::eRgb10A2u: program = envpref::doCreateProgram< ast::type::ImageFormat::eRgb10A2u >( m_device, originalSize, mipLevel, isCharlie ); break;
		case ast::type::ImageFormat::eRgba16Snorm: program = envpref::doCreateProgram< ast::type::ImageFormat::eRgba16Snorm >( m_device, originalSize, mipLevel, isCharlie ); break;
		case ast::type::ImageFormat::eRgba8Snorm: program = envpref::doCreateProgram< ast::type::ImageFormat::eRgba8Snorm >( m_device, originalSize, mipLevel, isCharlie ); break;
		case ast::type::ImageFormat::eRg16Snorm: program = envpref::doCreateProgram< ast::type::ImageFormat::eRg16Snorm >( m_device, originalSize, mipLevel, isCharlie ); break;
		case ast::type::ImageFormat::eRg8Snorm: program = envpref::doCreateProgram< ast::type::ImageFormat::eRg8Snorm >( m_device, originalSize, mipLevel, isCharlie ); break;
		case ast::type::ImageFormat::eR16Snorm: program = envpref::doCreateProgram< ast::type::ImageFormat::eR16Snorm >( m_device, originalSize, mipLevel, isCharlie ); break;
		case ast::type::ImageFormat::eR8Snorm: program = envpref::doCreateProgram< ast::type::ImageFormat::eR8Snorm >( m_device, originalSize, mipLevel, isCharlie ); break;
		case ast::type::ImageFormat::eRgba16Unorm: program = envpref::doCreateProgram< ast::type::ImageFormat::eRgba16Unorm >( m_device, originalSize, mipLevel, isCharlie ); break;
		case ast::type::ImageFormat::eRgba8Unorm: program = envpref::doCreateProgram< ast::type::ImageFormat::eRgba8Unorm >( m_device, originalSize, mipLevel, isCharlie ); break;
		case ast::type::ImageFormat::eRg16Unorm: program = envpref::doCreateProgram< ast::type::ImageFormat::eRg16Unorm >( m_device, originalSize, mipLevel, isCharlie ); break;
		case ast::type::ImageFormat::eRg8Unorm: program = envpref::doCreateProgram< ast::type::ImageFormat::eRg8Unorm >( m_device, originalSize, mipLevel, isCharlie ); break;
		case ast::type::ImageFormat::eR16Unorm: program = envpref::doCreateProgram< ast::type::ImageFormat::eR16Unorm >( m_device, originalSize, mipLevel, isCharlie ); break;
		case ast::type::ImageFormat::eR8Unorm: program = envpref::doCreateProgram< ast::type::ImageFormat::eR8Unorm >( m_device, originalSize, mipLevel, isCharlie ); break;
		case ast::type::ImageFormat::eRgb10A2Unorm: program = envpref::doCreateProgram< ast::type::ImageFormat::eRgb10A2Unorm >( m_device, originalSize, mipLevel, isCharlie ); break;
		default:
			CU_Failure( "Unsupported ImageFormat" );
			break;
		}
		createPipelines( size, move( program ), srcView, renderPass, {} );
	}

	void EnvironmentPrefilter::MipRenderCube::registerFrames()
	{
		auto const & cmd = *m_commands.commandBuffer;
		cmd.begin();
		cmd.beginDebugBlock( { "Prefiltering " + toUtf8( m_prefix ) + " Environment map"
			, makeFloatArray( m_device.renderSystem.getEngine()->getNextRainbowColour() ) } );

		for ( uint32_t face = 0u; face < 6u; ++face )
		{
			auto const & frameBuffer = m_frameBuffers[face];
			cmd.beginRenderPass( m_renderPass
				, *frameBuffer.frameBuffer
				, { convert( ClearValue{ transparentBlackClearColor } ) }
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

	SemaphoreWaitArray EnvironmentPrefilter::MipRenderCube::render( SemaphoreWaitArray const & signalsToWait
		, ashes::Queue const & queue )const
	{
		return { 1u
			, { m_commands.submit( queue, signalsToWait )
				, PipelineStageFlags::eColorAttachmentOutput } };
	}

	//*********************************************************************************************

	EnvironmentPrefilter::EnvironmentPrefilter( Engine & engine
		, RenderDevice const & device
		, Size const & size
		, Texture const & srcTexture
		, SamplerObs sampler
		, bool isCharlie )
		: m_device{ device }
		, m_srcView{ srcTexture }
		, m_prefix{ isCharlie ? String{ cuT( "Sheen" ) } : String{} }
		, m_srcImage{ m_srcView.image.get() }
		, m_srcImageView{ m_srcImage->createView( toUtf8( m_prefix ) + "EnvironmentPrefilterSrc", VK_IMAGE_VIEW_TYPE_CUBE, convert( m_srcView.getFormat() ), 0u, m_srcView.getMipLevels(), 0u, 6u ) }
		, m_result{ envpref::doCreatePrefilteredTexture( m_device, *m_srcView.resources, size, m_prefix ) }
		, m_sampler{ envpref::doCreateSampler( engine, m_device, m_prefix, m_result.getMipLevels() - 1u ) }
		, m_renderPass{ envpref::doCreateRenderPass( m_device, m_prefix, m_result.getFormat() ) }
	{
		Extent2D originalSize{ size.getWidth(), size.getHeight() };
		auto data = m_device.graphicsData();

		for ( auto mipLevel = 0u; mipLevel < MaxIblReflectionLod + 1u; ++mipLevel )
		{
			Extent2D mipSize{ originalSize.width >> mipLevel
				, originalSize.height >> mipLevel };
			m_renderPasses.emplace_back( makeRawUnique< MipRenderCube >( m_device
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

	SemaphoreWaitArray EnvironmentPrefilter::render( SemaphoreWaitArray signalsToWait
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
