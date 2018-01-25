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

#include <FrameBuffer/BackBuffers.hpp>
#include <FrameBuffer/FrameBufferAttachment.hpp>
#include <FrameBuffer/TextureAttachment.hpp>
#include <Mesh/Vertex.hpp>
#include <Mesh/Buffer/BufferDeclaration.hpp>
#include <Mesh/Buffer/BufferElementDeclaration.hpp>
#include <Mesh/Buffer/GeometryBuffers.hpp>
#include <Mesh/Buffer/VertexBuffer.hpp>
#include <Miscellaneous/Parameter.hpp>
#include <Render/Context.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Render/RenderTarget.hpp>
#include <Render/RenderWindow.hpp>
#include <Render/Viewport.hpp>
#include <Shader/UniformBuffer.hpp>
#include <Shader/ShaderProgram.hpp>
#include <State/BlendState.hpp>
#include <State/RasteriserState.hpp>
#include <Texture/Sampler.hpp>
#include <Texture/TextureLayout.hpp>
#include <Texture/TextureUnit.hpp>

#include <numeric>

#include <GlslSource.hpp>


using namespace castor;
using namespace castor3d;

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
		static constexpr uint32_t BindingPoint = 2u;

		glsl::Shader getVertexProgram( RenderSystem * renderSystem )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem->createGlslWriter();

			UBO_MATRIX( writer, 0u );

			// Shader inputs
			Vec2 position = writer.declAttribute< Vec2 >( cuT( "position" ) );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ) );
			auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				vtx_texture = position;
				gl_Position = c3d_projection * vec4( position.xy(), 0.0, 1.0 );
			} );
			return writer.finalise();
		}

		glsl::Shader getFragmentProgram( RenderSystem * renderSystem )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem->createGlslWriter();

			// Shader inputs
			glsl::Ubo filmGrain{ writer, FilmGrainUbo, BindingPoint };
			auto c3d_pixelSize = filmGrain.declMember< Vec2 >( PixelSize );
			auto c3d_noiseIntensity = filmGrain.declMember< Float >( NoiseIntensity );
			auto c3d_exposure = filmGrain.declMember< Float >( Exposure );
			auto c3d_time = filmGrain.declMember< Float >( Time );
			filmGrain.end();

			auto c3d_srcTex = writer.declSampler< Sampler2D >( SrcTex, MinTextureIndex + 0u, 0u );
			auto c3d_noiseTex = writer.declSampler< Sampler3D >( NoiseTex, MinTextureIndex + 1u, 0u );
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

	String PostEffect::Type = cuT( "film_grain" );
	String PostEffect::Name = cuT( "FilmGrain PostEffect" );

	PostEffect::PostEffect( RenderTarget & renderTarget
		, RenderSystem & renderSystem
		, Parameters const & params )
		: castor3d::PostEffect{ PostEffect::Type
			, renderTarget
			, renderSystem
			, params
			, false }
		, m_surface{ *renderSystem.getEngine() }
		, m_matrixUbo{ *renderSystem.getEngine() }
		, m_noise{ *renderSystem.getEngine() }
		, m_configUbo{ FilmGrainUbo, renderSystem, BindingPoint }
		, m_pixelSize{ *m_configUbo.createUniform< UniformType::eVec2f >( PixelSize ) }
		, m_noiseIntensity{ *m_configUbo.createUniform< UniformType::eFloat >( NoiseIntensity ) }
		, m_exposure{ *m_configUbo.createUniform< UniformType::eFloat >( Exposure ) }
		, m_time{ *m_configUbo.createUniform< UniformType::eFloat >( Time ) }
	{
		String name = cuT( "FilmGrain2D" );

		if ( !m_renderTarget.getEngine()->getSamplerCache().has( name ) )
		{
			m_sampler2D = m_renderTarget.getEngine()->getSamplerCache().add( name );
			m_sampler2D->setInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
			m_sampler2D->setInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
			m_sampler2D->setInterpolationMode( InterpolationFilter::eMip, InterpolationMode::eLinear );
			m_sampler2D->setWrappingMode( TextureUVW::eU, WrapMode::eClampToBorder );
			m_sampler2D->setWrappingMode( TextureUVW::eV, WrapMode::eClampToBorder );
			m_sampler2D->setWrappingMode( TextureUVW::eW, WrapMode::eClampToBorder );
		}
		else
		{
			m_sampler2D = m_renderTarget.getEngine()->getSamplerCache().find( name );
		}

		name = cuT( "FilmGrain3D" );

		if ( !m_renderTarget.getEngine()->getSamplerCache().has( name ) )
		{
			m_sampler3D = m_renderTarget.getEngine()->getSamplerCache().add( name );
			m_sampler3D->setInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
			m_sampler3D->setInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
			m_sampler3D->setInterpolationMode( InterpolationFilter::eMip, InterpolationMode::eLinear );
			m_sampler3D->setWrappingMode( TextureUVW::eU, WrapMode::eRepeat );
			m_sampler3D->setWrappingMode( TextureUVW::eV, WrapMode::eRepeat );
			m_sampler3D->setWrappingMode( TextureUVW::eW, WrapMode::eRepeat );
		}
		else
		{
			m_sampler3D = m_renderTarget.getEngine()->getSamplerCache().find( name );
		}
	}

	PostEffect::~PostEffect()
	{
	}

	PostEffectSPtr PostEffect::create( RenderTarget & renderTarget
		, RenderSystem & renderSystem
		, Parameters const & params )
	{
		return std::make_shared< PostEffect >( renderTarget
			, renderSystem
			, params );
	}

	bool PostEffect::initialise()
	{
		bool result = false;
		auto & cache = getRenderSystem()->getEngine()->getShaderProgramCache();
		Size size = m_renderTarget.getSize();

		auto vertex = getVertexProgram( getRenderSystem() );
		auto fragment = getFragmentProgram( getRenderSystem() );
		ShaderProgramSPtr program = cache.getNewProgram( false );
		program->createObject( ShaderType::eVertex );
		program->createObject( ShaderType::ePixel );
		m_mapSrc = program->createUniform< UniformType::eSampler >( SrcTex
			, ShaderType::ePixel );
		m_mapSrc->setValue( MinTextureIndex + 0 );
		m_mapNoise = program->createUniform< UniformType::eSampler >( NoiseTex
			, ShaderType::ePixel );
		m_mapNoise->setValue( MinTextureIndex + 1 );
		program->setSource( ShaderType::eVertex, vertex );
		program->setSource( ShaderType::ePixel, fragment );
		program->initialise();

		DepthStencilState dsstate;
		dsstate.setDepthTest( false );
		dsstate.setDepthMask( WritingMask::eZero );
		RasteriserState rsstate;
		rsstate.setCulledFaces( Culling::eBack );
		m_pipeline = getRenderSystem()->createRenderPipeline( std::move( dsstate )
			, std::move( rsstate )
			, BlendState{}
			, MultisampleState{}
			, *program
			, PipelineFlags{} );
		m_pipeline->addUniformBuffer( m_matrixUbo.getUbo() );
		m_pipeline->addUniformBuffer( m_configUbo );
		result = m_surface.initialise( m_renderTarget
			, size
			, MinTextureIndex
			, m_sampler2D );

		m_pixelSize.setValue( Point2f{ size.getWidth(), size.getHeight() } );
		m_noiseIntensity.setValue( 1.0f );
		m_exposure.setValue( 1.0f );

		doGenerateNoiseTexture();

		return result;
	}

	void PostEffect::cleanup()
	{
		m_mapSrc.reset();
		m_mapNoise.reset();
		m_noise.cleanup();
		m_surface.cleanup();
		m_matrixUbo.getUbo().cleanup();
		m_configUbo.cleanup();
	}

	bool PostEffect::apply( FrameBuffer & framebuffer )
	{
		m_time.setValue( float( m_timer.getElapsed().count() ) / 6.0f );
		m_configUbo.update();
		m_configUbo.bindTo( BindingPoint );
		auto attach = framebuffer.getAttachment( AttachmentPoint::eColour, 0 );
		REQUIRE( attach && attach->getAttachmentType() == AttachmentType::eTexture );

		m_surface.m_fbo->bind( FrameBufferTarget::eDraw );
		auto texture = std::static_pointer_cast< TextureAttachment >( attach )->getTexture();
		m_surface.m_fbo->clear( BufferComponent::eColour );
		m_noise.bind();
		getRenderSystem()->getCurrentContext()->renderTexture( 
			m_surface.m_size
			, *texture
			, *m_pipeline
			, m_matrixUbo );
		m_noise.unbind();
		m_surface.m_fbo->unbind();

		framebuffer.bind( FrameBufferTarget::eDraw );
		getRenderSystem()->getCurrentContext()->renderTexture( texture->getDimensions()
			, *m_surface.m_colourTexture.getTexture() );
		framebuffer.unbind();

		return true;
	}

	bool PostEffect::doWriteInto( TextFile & file )
	{
		return true;
	}

	void PostEffect::doGenerateNoiseTexture()
	{
		auto texture = getRenderSystem()->createTexture( TextureType::eThreeDimensions
			, AccessType::eWrite
			, AccessType::eRead
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

		for ( uint32_t i = 0u; i < 6u; ++i )
		{
			texture->getImage( i ).initialiseSource( buffers[i] );
		}

		m_noise.setTexture( texture );
		m_noise.setSampler( m_sampler3D );
		m_noise.setIndex( MinTextureIndex + 1u );
		m_noise.initialise();
	}
}
