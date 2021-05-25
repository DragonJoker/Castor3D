#include "FilmGrainPostEffect/FilmGrainPostEffect.hpp"

#include "NoiseLayer1.xpm"
#include "NoiseLayer2.xpm"
#include "NoiseLayer3.xpm"
#include "NoiseLayer4.xpm"
#include "NoiseLayer5.xpm"
#include "NoiseLayer6.xpm"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/UniformBufferPools.hpp>
#include <Castor3D/Cache/SamplerCache.hpp>
#include <Castor3D/Cache/ShaderCache.hpp>
#include <Castor3D/Material/Texture/Sampler.hpp>
#include <Castor3D/Material/Texture/TextureLayout.hpp>
#include <Castor3D/Material/Texture/TextureUnit.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Render/RenderLoop.hpp>
#include <Castor3D/Render/RenderPassTimer.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>

#include <CastorUtils/Graphics/PixelBufferBase.hpp>
#include <CastorUtils/Graphics/RgbaColour.hpp>

#include <ashespp/Image/StagingTexture.hpp>
#include <ashespp/Image/Image.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>

#include <RenderGraph/RenderQuad.hpp>

#include <numeric>

using namespace castor;

namespace film_grain
{
	namespace
	{
		static std::string const FilmGrainUbo = "FilmGrainUbo";
		static std::string const PixelSize = "c3d_pixelSize";
		static std::string const NoiseIntensity = "c3d_noiseIntensity";
		static std::string const Exposure = "c3d_exposure";
		static std::string const Time = "c3d_time";
		static std::string const SrcTex = "c3d_srcTex";
		static std::string const NoiseTex = "c3d_noiseTex";

		enum Idx : uint32_t
		{
			FilmCfgUboIdx,
			NoiseTexIdx,
			SourceTexIdx,
		};

		std::unique_ptr< ast::Shader > getVertexProgram()
		{
			using namespace sdw;
			VertexWriter writer;

			// Shader inputs
			Vec2 position = writer.declInput< Vec2 >( "position", 0u );
			Vec2 uv = writer.declInput< Vec2 >( "uv", 1u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( "vtx_texture", 0u );
			auto out = writer.getOut();

			castor3d::shader::Utils utils{ writer };
			utils.declareInvertVec2Y();

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					vtx_texture = uv;
					out.vtx.position = vec4( position.xy(), 0.0_f, 1.0_f );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		std::unique_ptr< ast::Shader > getFragmentProgram()
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			sdw::Ubo filmGrain{ writer, FilmGrainUbo, FilmCfgUboIdx, 0u };
			auto c3d_pixelSize = filmGrain.declMember< Vec2 >( PixelSize );
			auto c3d_noiseIntensity = filmGrain.declMember< Float >( NoiseIntensity );
			auto c3d_exposure = filmGrain.declMember< Float >( Exposure );
			auto c3d_time = filmGrain.declMember< Float >( Time );
			filmGrain.end();

			auto c3d_noiseTex = writer.declSampledImage< FImg3DR32 >( NoiseTex, NoiseTexIdx, 0u );
			auto c3d_srcTex = writer.declSampledImage< FImg2DRgba32 >( SrcTex, SourceTexIdx, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( "pxl_fragColor", 0 );
			
			auto overlay = writer.implementFunction< Vec3 >( "overlay"
				, [&]( Vec3 const & a
					, Vec3 const & b )
				{
					auto comp = writer.declLocale( "comp"
						, pow( abs( b ), vec3( 2.2_f ) ) );

					IF( writer, comp.x() < 0.5_f && comp.y() < 0.5_f && comp.z() < 0.5_f )
					{
						writer.returnStmt( 2.0_f * a * b );
					}
					FI;

					writer.returnStmt( vec3( 1.0_f ) - 2.0_f * ( 1.0_f - a ) * ( 1.0_f - b ) );
				}
				, InVec3{ writer, "a" }
				, InVec3{ writer, "b" } );

			auto addNoise = writer.implementFunction< Vec3 >( "addNoise"
				, [&]( Vec3 const & color
					, Vec2 const & texcoord )
				{
					auto coord = writer.declLocale( "coord"
						, texcoord * 2.0_f );
					coord.x() *= c3d_pixelSize.y() / c3d_pixelSize.x();
					auto noise = writer.declLocale( "noise"
						, c3d_noiseTex.sample( vec3( coord, c3d_time ) ) );
					auto exposureFactor = writer.declLocale( "exposureFactor"
						, c3d_exposure / 2.0_f );
					exposureFactor = sqrt( exposureFactor );
					auto t = writer.declLocale( "t"
						, mix( 3.5_f * c3d_noiseIntensity
							, 1.13_f * c3d_noiseIntensity
							, exposureFactor ) );
					writer.returnStmt( overlay( color
						, vec3( mix( 0.5_f, noise, t ) ) ) );
				}
				, InVec3{ writer, "color" }
				, InVec2{ writer, "texcoord" } );

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					auto colour = writer.declLocale( "colour"
						, c3d_srcTex.sample( vtx_texture ).xyz() );
					colour = addNoise( colour, vtx_texture );
					pxl_fragColor = vec4( colour, 1.0 );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		std::array< castor::Image, PostEffect::NoiseMapCount > loadImages( castor3d::Engine const & engine )
		{
			auto & loader = engine.getImageLoader();
			return { loader.load( cuT( "FilmGrainNoise0" ), "xpm", reinterpret_cast< uint8_t const * >( NoiseLayer1_xpm ), uint32_t( castor::getCountOf( NoiseLayer1_xpm ) ), false, false )
				, loader.load( cuT( "FilmGrainNoise1" ), "xpm", reinterpret_cast< uint8_t const * >( NoiseLayer2_xpm ), uint32_t( castor::getCountOf( NoiseLayer2_xpm ) ), false, false )
				, loader.load( cuT( "FilmGrainNoise2" ), "xpm", reinterpret_cast< uint8_t const * >( NoiseLayer3_xpm ), uint32_t( castor::getCountOf( NoiseLayer3_xpm ) ), false, false )
				, loader.load( cuT( "FilmGrainNoise3" ), "xpm", reinterpret_cast< uint8_t const * >( NoiseLayer4_xpm ), uint32_t( castor::getCountOf( NoiseLayer4_xpm ) ), false, false )
				, loader.load( cuT( "FilmGrainNoise4" ), "xpm", reinterpret_cast< uint8_t const * >( NoiseLayer5_xpm ), uint32_t( castor::getCountOf( NoiseLayer5_xpm ) ), false, false )
				, loader.load( cuT( "FilmGrainNoise5" ), "xpm", reinterpret_cast< uint8_t const * >( NoiseLayer6_xpm ), uint32_t( castor::getCountOf( NoiseLayer6_xpm ) ), false, false ) };
		}
	}

	//*********************************************************************************************

	String PostEffect::Type = cuT( "film_grain" );
	String PostEffect::Name = cuT( "FilmGrain PostEffect" );

	PostEffect::PostEffect( castor3d::RenderTarget & renderTarget
		, castor3d::RenderSystem & renderSystem
		, castor3d::Parameters const & params )
		: castor3d::PostEffect{ PostEffect::Type
			, PostEffect::Name
			, renderTarget
			, renderSystem
			, params }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, "FilmGrain", getVertexProgram() }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, "FilmGrain", getFragmentProgram() }
		, m_stages{ makeShaderState( *renderSystem.getMainRenderDevice(), m_vertexShader )
			, makeShaderState( *renderSystem.getMainRenderDevice(), m_pixelShader ) }
		, m_configUbo{ renderSystem.getMainRenderDevice()->uboPools->getBuffer< Configuration >( 0u ) }
		, m_noiseImages{ loadImages( *renderTarget.getEngine() ) }
	{
		for ( auto & image : m_noiseImages )
		{
			auto format = image.getPixelFormat();

			if ( format == castor::PixelFormat::eR8G8B8_UNORM )
			{
				auto buffer = castor::PxBufferBase::create( image.getDimensions()
					, castor::PixelFormat::eR8G8B8A8_UNORM
					, image.getPxBuffer().getConstPtr()
					, image.getPxBuffer().getFormat()
					, image.getPxBuffer().getAlign() );
				image = castor::Image{ image.getName()
					, image.getPath()
					, *buffer };
			}
		}

		m_configUbo.getData().m_pixelSize = Point2f{ m_renderTarget.getSize().getWidth()
			, m_renderTarget.getSize().getHeight() };
		m_configUbo.getData().m_noiseIntensity = 1.0f;
		m_configUbo.getData().m_exposure = 1.0f;
	}

	PostEffect::~PostEffect()
	{
	}

	castor3d::PostEffectSPtr PostEffect::create( castor3d::RenderTarget & renderTarget
		, castor3d::RenderSystem & renderSystem
		, castor3d::Parameters const & params )
	{
		return std::make_shared< PostEffect >( renderTarget
			, renderSystem
			, params );
	}

	void PostEffect::accept( castor3d::PipelineVisitorBase & visitor )
	{
		visitor.visit( m_vertexShader );
		visitor.visit( m_pixelShader );
		visitor.visit( m_pixelShader.name
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, cuT( "FilmGrain" )
			, cuT( "Exposure" )
			, m_configUbo.getData().m_exposure );
		visitor.visit( m_pixelShader.name
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, cuT( "FilmGrain" )
			, cuT( "NoiseIntensity" )
			, m_configUbo.getData().m_noiseIntensity );
	}

	void PostEffect::update( castor3d::CpuUpdater & updater )
	{
		m_time += updater.time.count();
		m_configUbo.getData().m_time = ( m_time % NoiseMapCount ) / float( NoiseMapCount );
	}

	crg::ImageViewId const * PostEffect::doInitialise( castor3d::RenderDevice const & device
		, castor3d::RenderPassTimer const & timer
		, crg::FramePass const & previousPass )
	{
		VkExtent2D size{ m_target->data->image.data->info.extent.width
			, m_target->data->image.data->info.extent.height };
		auto & graph = m_renderTarget.getGraph();
		auto dim = m_noiseImages[0].getDimensions();
		auto format = castor3d::convert( m_noiseImages[0].getPixelFormat() );
		m_noiseImg = graph.createImage( crg::ImageData{ "FilmGrainNoise"
			, 0u
			, VK_IMAGE_TYPE_3D
			, format
			, { dim.getWidth(), dim.getHeight(), NoiseMapCount }
			, ( VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_TRANSFER_DST_BIT ) } );
		m_noiseView = graph.createView( crg::ImageViewData{ "FilmGrainNoise"
			, m_noiseImg
			, 0u
			, VK_IMAGE_VIEW_TYPE_3D
			, m_noiseImg.data->info.format
			, { VK_IMAGE_ASPECT_COLOR_BIT, 0u, 1u, 0u, 1u } } );
		m_resultImg = graph.createImage( crg::ImageData{ "FilmGrainResult"
			, 0u
			, VK_IMAGE_TYPE_2D
			, m_target->data->info.format
			, castor3d::makeExtent3D( m_renderTarget.getSize() )
			, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_TRANSFER_SRC_BIT ) } );
		m_resultView = m_renderTarget.getGraph().createView( crg::ImageViewData{ "FilmGrainResult"
			, m_resultImg
			, 0u
			, VK_IMAGE_VIEW_TYPE_2D
			, m_resultImg.data->info.format
			, { VK_IMAGE_ASPECT_COLOR_BIT, 0u, 1u, 0u, 1u } } );
		m_pass = &m_renderTarget.getGraph().createPass( "FilmGrainPass"
			, [this]( crg::FramePass const & pass
				, crg::GraphContext const & context
				, crg::RunnableGraph & graph )
			{
				auto dim = m_noiseImages[0].getDimensions();
				auto format = castor3d::convert( m_noiseImages[0].getPixelFormat() );
				auto & device = *getRenderSystem()->getMainRenderDevice();
				auto staging = device->createStagingTexture( format
					, VkExtent2D{ dim.getWidth(), dim.getHeight() } );
				ashes::ImagePtr noiseImg = std::make_unique< ashes::Image >( *device
					, graph.getImage( m_noiseImg )
					, m_noiseImg.data->info );
				ashes::ImageView noiseView{ ashes::ImageViewCreateInfo{ *noiseImg, m_noiseView.data->info }
					, graph.getImageView( m_noiseView )
					, noiseImg.get() };

				for ( uint32_t i = 0u; i < NoiseMapCount; ++i )
				{
					staging->uploadTextureData( *device.graphicsQueue
						, *device.graphicsCommandPool
						, { m_noiseView.data->info.subresourceRange.aspectMask
							, m_noiseView.data->info.subresourceRange.baseMipLevel
							, m_noiseView.data->info.subresourceRange.baseArrayLayer
							, m_noiseView.data->info.subresourceRange.layerCount }
						, format
						, { 0, 0, int32_t( i ) }
						, castor3d::makeExtent2D( dim )
						, m_noiseImages[i].getBuffer().data()
						, noiseView );
				}

				return crg::RenderQuadBuilder{}
					.renderPosition( {} )
					.renderSize( castor3d::makeExtent2D( m_renderTarget.getSize() ) )
					.texcoordConfig( {} )
					.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) )
					.build( pass, context, graph );
			} );
		m_pass->addDependency( previousPass );
		m_configUbo.createPassBinding( *m_pass
			, FilmCfgUboIdx );
		m_pass->addSampledView( m_noiseView
			, NoiseTexIdx
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, crg::SamplerDesc{ VK_FILTER_LINEAR
				, VK_FILTER_LINEAR
				, VK_SAMPLER_MIPMAP_MODE_LINEAR
				, VK_SAMPLER_ADDRESS_MODE_REPEAT
				, VK_SAMPLER_ADDRESS_MODE_REPEAT
				, VK_SAMPLER_ADDRESS_MODE_REPEAT } );
		m_pass->addSampledView( *m_target
			, SourceTexIdx
			, VK_IMAGE_LAYOUT_UNDEFINED );
		m_pass->addOutputColourView( m_resultView );
		return &m_resultView;
	}

	void PostEffect::doCleanup( castor3d::RenderDevice const & device )
	{
		device.uboPools->putBuffer( m_configUbo );
	}

	bool PostEffect::doWriteInto( StringStream & file, String const & tabs )
	{
		file << ( tabs + cuT( "postfx \"" ) + Type + cuT( "\"" ) );
		return true;
	}
}
