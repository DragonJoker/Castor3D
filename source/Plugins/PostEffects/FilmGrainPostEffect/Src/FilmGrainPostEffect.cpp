#include "FilmGrainPostEffect.hpp"

#include "XpmLoader.hpp"

#include "NoiseLayer1.xpm"
#include "NoiseLayer2.xpm"
#include "NoiseLayer3.xpm"
#include "NoiseLayer4.xpm"
#include "NoiseLayer5.xpm"
#include "NoiseLayer6.xpm"

#include <Engine.hpp>
#include <Cache/SamplerCache.hpp>
#include <Cache/ShaderCache.hpp>

#include <Miscellaneous/Parameter.hpp>
#include <Render/RenderSystem.hpp>
#include <Render/RenderTarget.hpp>
#include <Texture/Sampler.hpp>
#include <Texture/TextureLayout.hpp>
#include <Texture/TextureUnit.hpp>

#include <RenderPass/RenderPass.hpp>
#include <RenderPass/RenderPassState.hpp>
#include <RenderPass/RenderSubpass.hpp>
#include <RenderPass/RenderSubpassState.hpp>
#include <Shader/ShaderProgram.hpp>

#include <GlslSource.hpp>

#include <numeric>

using namespace castor;

namespace film_grain
{
	namespace
	{
		static String const FilmGrainUbo = cuT( "FilmGrainUbo" );
		static String const PixelSize = cuT( "c3d_pixelSize" );
		static String const NoiseIntensity = cuT( "c3d_noiseIntensity" );
		static String const Exposure = cuT( "c3d_exposure" );
		static String const Time = cuT( "c3d_time" );
		static String const SrcTex = cuT( "c3d_srcTex" );
		static String const NoiseTex = cuT( "c3d_noiseTex" );

		glsl::Shader getVertexProgram( castor3d::RenderSystem * renderSystem )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem->createGlslWriter();

			// Shader inputs
			Vec2 position = writer.declAttribute< Vec2 >( cuT( "position" ) );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ) );
			auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				vtx_texture = position;
				gl_Position = vec4( position.xy(), 0.0, 1.0 );
			} );
			return writer.finalise();
		}

		glsl::Shader getFragmentProgram( castor3d::RenderSystem * renderSystem )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem->createGlslWriter();

			// Shader inputs
			glsl::Ubo filmGrain{ writer, FilmGrainUbo, 0u };
			auto c3d_pixelSize = filmGrain.declMember< Vec2 >( PixelSize );
			auto c3d_noiseIntensity = filmGrain.declMember< Float >( NoiseIntensity );
			auto c3d_exposure = filmGrain.declMember< Float >( Exposure );
			auto c3d_time = filmGrain.declMember< Float >( Time );
			filmGrain.end();

			auto c3d_srcTex = writer.declSampler< Sampler2D >( SrcTex, 1u, 0u );
			auto c3d_noiseTex = writer.declSampler< Sampler3D >( NoiseTex, 2u, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto plx_fragColor = writer.declFragData< Vec4 >( cuT( "plx_fragColor" ), 0 );
			
			auto overlay = writer.implementFunction< Vec3 >( cuT( "overlay" )
				, [&]( Vec3 const & a
					, Vec3 const & b )
				{
					auto comp = writer.declLocale( cuT( "comp" )
						, pow( abs( b ), vec3( 2.2_f ) ) );

					IF( writer, cuT( "comp.x < 0.5 && comp.y < 0.5 && comp.z < 0.5" ) )
					{
						writer.returnStmt( 2.0_f * a * b );
					}
					FI;

					writer.returnStmt( vec3( 1.0_f ) - 2.0_f * ( 1.0_f - a ) * ( 1.0_f - b ) );
				}
				, InVec3{ &writer, cuT( "a" ) }
				, InVec3{ &writer, cuT( "b" ) } );

			auto addNoise = writer.implementFunction< Vec3 >( cuT( "addNoise" )
				, [&]( Vec3 const & color
					, Vec2 const & texcoord )
				{
					auto coord = writer.declLocale( cuT( "coord" )
						, texcoord * 2.0 );
					coord.x() *= c3d_pixelSize.y() / c3d_pixelSize.x();
					auto noise = writer.declLocale( cuT( "noise" )
						, texture( c3d_noiseTex, vec3( coord, c3d_time ) ).r() );
					auto exposureFactor = writer.declLocale( cuT( "exposureFactor" )
						, c3d_exposure / 2.0 );
					exposureFactor = sqrt( exposureFactor );
					auto t = writer.declLocale( cuT( "t" )
						, mix( 3.5_f * c3d_noiseIntensity
							, 1.13_f * c3d_noiseIntensity
							, exposureFactor ) );
					writer.returnStmt( overlay( color
						, vec3( mix( 0.5_f, noise, t ) ) ) );
				}
				, InVec3{ &writer, cuT( "color" ) }
				, InVec2{ &writer, cuT( "texcoord" ) } );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				auto colour = writer.declLocale( cuT( "colour" )
					, texture( c3d_srcTex, vtx_texture ).xyz() );
				colour = addNoise( colour, vtx_texture );
				plx_fragColor = vec4( colour, 1.0 );
			} );
			return writer.finalise();
		}

		template< typename T, size_t N >
		inline size_t getCountOf( T const( &p_data )[N] )
		{
			return N;
		}
	}

	//*********************************************************************************************

	RenderQuad::RenderQuad( castor3d::RenderSystem & renderSystem
		, Size const & size )
		: castor3d::RenderQuad{ renderSystem, false, false }
		, m_size{ size }
		, m_noise{ *renderSystem.getEngine() }
	{
		auto name = cuT( "FilmGrain_Noise" );
		castor3d::SamplerSPtr sampler;

		if ( !m_renderSystem.getEngine()->getSamplerCache().has( name ) )
		{
			sampler = m_renderSystem.getEngine()->getSamplerCache().add( name );
			sampler->setMinFilter( renderer::Filter::eLinear );
			sampler->setMagFilter( renderer::Filter::eLinear );
			sampler->setMipFilter( renderer::MipmapMode::eLinear );
			sampler->setWrapS( renderer::WrapMode::eRepeat );
			sampler->setWrapT( renderer::WrapMode::eRepeat );
			sampler->setWrapR( renderer::WrapMode::eRepeat );
		}
		else
		{
			sampler = m_renderSystem.getEngine()->getSamplerCache().find( name );
		}

		auto texture = std::make_shared< castor3d::TextureLayout >( m_renderSystem
			, renderer::TextureType::e3D
			, renderer::ImageUsageFlag::eSampled
			, renderer::MemoryPropertyFlag::eDeviceLocal
			, PixelFormat::eR8G8B8
			, Point3ui{ 512u, 512u, 6u } );

		XpmLoader loader;
		std::array< PxBufferBaseSPtr, 6u > buffers
		{
			loader.loadImage( NoiseLayer1_xpm, getCountOf( NoiseLayer1_xpm ) ),
			loader.loadImage( NoiseLayer2_xpm, getCountOf( NoiseLayer2_xpm ) ),
			loader.loadImage( NoiseLayer3_xpm, getCountOf( NoiseLayer3_xpm ) ),
			loader.loadImage( NoiseLayer4_xpm, getCountOf( NoiseLayer4_xpm ) ),
			loader.loadImage( NoiseLayer5_xpm, getCountOf( NoiseLayer5_xpm ) ),
			loader.loadImage( NoiseLayer6_xpm, getCountOf( NoiseLayer6_xpm ) ),
		};

		for ( uint32_t i = 1u; i <= 6u; ++i )
		{
			texture->getImage( i ).initialiseSource( buffers[i] );
		}

		m_noise.setTexture( texture );
		m_noise.setSampler( sampler );
		m_noise.initialise();

		m_configUbo = renderer::makeUniformBuffer< Configuration >( *m_renderSystem.getCurrentDevice()
			, 1u
			, renderer::BufferTarget::eTransferDst
			, renderer::MemoryPropertyFlag::eHostVisible );
		m_configUbo->getData( 0 ).m_pixelSize = Point2f{ m_size };
		m_configUbo->getData( 0 ).m_noiseIntensity = 1.0f;
		m_configUbo->getData( 0 ).m_exposure = 1.0f;
	}

	void RenderQuad::update( float time )
	{
		m_configUbo->getData( 0 ).m_time = time;

		if ( auto buffer = m_configUbo->getUbo().getBuffer().lock( 0u
			, sizeof( Configuration )
			, renderer::MemoryMapFlag::eWrite | renderer::MemoryMapFlag::eInvalidateRange ) )
		{
			std::memcpy( buffer, m_configUbo->getDatas().data(), sizeof( Configuration ) );
			m_configUbo->getUbo().getBuffer().unlock( sizeof( Configuration ), true );
		}
	}

	void RenderQuad::doFillDescriptorSet( renderer::DescriptorSetLayout & descriptorSetLayout
		, renderer::DescriptorSet & descriptorSet )
	{
		descriptorSet.createBinding( descriptorSetLayout.getBinding( 0u )
			, *m_configUbo
			, 0u
			, 1u );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( 1u )
			, m_noise.getTexture()->getView()
			, m_noise.getSampler()->getSampler() );
	}

	//*********************************************************************************************

	String PostEffect::Type = cuT( "film_grain" );
	String PostEffect::Name = cuT( "FilmGrain PostEffect" );

	PostEffect::PostEffect( castor3d::RenderTarget & renderTarget
		, castor3d::RenderSystem & renderSystem
		, castor3d::Parameters const & params )
		: castor3d::PostEffect{ PostEffect::Type
			, renderTarget
			, renderSystem
			, params
			, false }
		, m_surface{ *renderSystem.getEngine() }
	{
		String name = cuT( "FilmGrain2D" );

		if ( !m_renderTarget.getEngine()->getSamplerCache().has( name ) )
		{
			m_sampler = m_renderTarget.getEngine()->getSamplerCache().add( name );
			m_sampler->setMinFilter( renderer::Filter::eLinear );
			m_sampler->setMagFilter( renderer::Filter::eLinear );
			m_sampler->setMipFilter( renderer::MipmapMode::eLinear );
			m_sampler->setWrapS( renderer::WrapMode::eClampToBorder );
			m_sampler->setWrapT( renderer::WrapMode::eClampToBorder );
			m_sampler->setWrapR( renderer::WrapMode::eClampToBorder );
		}
		else
		{
			m_sampler = m_renderTarget.getEngine()->getSamplerCache().find( name );
		}
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

	bool PostEffect::initialise()
	{
		auto & cache = getRenderSystem()->getEngine()->getShaderProgramCache();
		Size size = m_renderTarget.getSize();

		auto vtx = getVertexProgram( getRenderSystem() );
		auto pxl = getFragmentProgram( getRenderSystem() );
		auto & program = cache.getNewProgram( false );

		program.createModule( vtx.getSource(), renderer::ShaderStageFlag::eVertex );
		program.createModule( pxl.getSource(), renderer::ShaderStageFlag::eFragment );
		program.link();

		auto & device = *getRenderSystem()->getCurrentDevice();
		m_commandBuffer = device.getGraphicsCommandPool().createCommandBuffer();

		std::vector< renderer::PixelFormat > formats{ { m_renderTarget.getPixelFormat() } };
		renderer::RenderSubpassPtrArray subpasses;
		subpasses.emplace_back( device.createRenderSubpass( formats
			, renderer::RenderSubpassState{ renderer::PipelineStageFlag::eColourAttachmentOutput
			, renderer::AccessFlag::eColourAttachmentWrite } ) );
		m_renderPass = device.createRenderPass( formats
			, std::move( subpasses )
			, renderer::RenderPassState{ renderer::PipelineStageFlag::eColourAttachmentOutput
				, renderer::AccessFlag::eColourAttachmentWrite
				, { renderer::ImageLayout::eColourAttachmentOptimal } }
			, renderer::RenderPassState{ renderer::PipelineStageFlag::eColourAttachmentOutput
				, renderer::AccessFlag::eColourAttachmentWrite
				, { renderer::ImageLayout::eColourAttachmentOptimal } }
			, false );

		renderer::DescriptorSetLayoutBindingArray bindings
		{
			{ 0u, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eFragment },
			{ 1u, renderer::DescriptorType::eSampledImage, renderer::ShaderStageFlag::eFragment },
		};

		m_quad = std::make_unique< RenderQuad >( *getRenderSystem(), size );
		m_quad->createPipeline( size
			, Position{}
			, program
			, m_renderTarget.getTexture().getTexture()->getView()
			, *m_renderPass
			, bindings );

		auto result = m_surface.initialise( m_renderTarget
			, *m_renderPass
			, size
			, 0u
			, m_sampler );

		if ( result
			&& m_commandBuffer->begin( renderer::CommandBufferUsageFlag::eSimultaneousUse ) )
		{
			m_commandBuffer->beginRenderPass( *m_renderPass
				, *m_surface.m_fbo
				, {}
				, renderer::SubpassContents::eInline );
			m_quad->registerFrame( *m_commandBuffer );
			m_commandBuffer->endRenderPass();
			m_commandBuffer->end();
		}

		return result;
	}

	void PostEffect::cleanup()
	{
		m_quad.reset();
		m_commandBuffer.reset();
		m_renderPass.reset();
		m_surface.cleanup();
	}

	bool PostEffect::apply()
	{
		m_quad->update( float( m_timer.getElapsed().count() ) / 6.0f );
		return getRenderSystem()->getCurrentDevice()->getGraphicsQueue().submit( *m_commandBuffer, nullptr );
	}

	bool PostEffect::doWriteInto( TextFile & file )
	{
		return true;
	}
}
