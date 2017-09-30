#include "FilmGrainPostEffect.hpp"

#include "Noise.hpp"

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

			UBO_MATRIX( writer );

			// Shader inputs
			Vec2 position = writer.declAttribute< Vec2 >( ShaderProgram::Position );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ) );
			auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				vtx_texture = position;
				gl_Position = c3d_mtxProjection * vec4( position.xy(), 0.0, 1.0 );
			} );
			return writer.finalise();
		}

		glsl::Shader getNoiseProgram( RenderSystem * renderSystem )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem->createGlslWriter();

			auto permute = writer.implementFunction< Vec3 >( cuT( "permute" )
				, [&]( Vec3 const & x )
				{
					writer.returnStmt( mod( ( ( x * 34.0 ) + 1.0 ) * x, 289.0_f ) );
				}
				, InVec3{ &writer, cuT( "x" ) } );

			auto snoise = writer.implementFunction< Vec3 >( cuT( "snoise" )
				, [&]( Vec2 const & v )
				{
					auto C = writer.declLocale( cuT( "C" )
						, vec4( 0.211324865405187_f
							, 0.366025403784439_f
							, -0.577350269189626_f
							, 0.024390243902439_f ) );
					auto i = writer.declLocale( cuT( "i" )
						, floor( v + dot( v, C.yy() ) ) );
					auto x0 = writer.declLocale( cuT( "x0" )
						, v - i + dot( i, C.xx() ) );
					auto i1 = writer.declLocale( cuT( "i1" )
						, writer.ternary( x0.x() > x0.y()
							, vec2( 1.0_f, 0.0 )
							, vec2( 0.0_f, 1.0 ) ) );
					auto x12 = writer.declLocale( cuT( "x12" )
						, vec4( x0.xy(), x0.xy() ) + C.xxzz() );
					x12.xy() -= i1;
					i = mod( i, 289.0 );
					auto p = writer.declLocale( cuT( "p" )
						, permute( permute( vec3( 0.0_f, i1.y(), 1.0 ) + i.y() )
							+ i.x() + vec3( 0.0_f, i1.x(), 1.0 ) ) );
					auto m = writer.declLocale( cuT( "m" )
						, max( vec3( 0.5_f ) - vec3( dot( x0, x0 ), dot( x12.xy(), x12.xy() ), dot( x12.zw(), x12.zw() ) )
							, vec3( 0.0_f ) ) );
					m = m * m;
					m = m * m;
					auto x = writer.declLocale( cuT( "x" )
						, 2.0_f * fract( p * C.www() ) - 1.0 );
					auto h = writer.declLocale( cuT( "h" )
						, abs( x ) - 0.5 );
					auto ox = writer.declLocale( cuT( "ox" )
						, floor( x + 0.5 ) );
					auto a0 = writer.declLocale( cuT( "a0" )
						, x - ox );
					m *= vec3( 1.79284291400159_f ) - 0.85373472095314_f * ( a0 * a0 + h * h );
					auto g = writer.declLocale( cuT( "g" )
						, vec3( a0.x() * x0.x() + h.x() * x0.y()
							, a0.yz() * x12.xz() + h.yz() * x12.yw() ) );
					writer.returnStmt( vec3( 130.0_f ) * dot( m, g ) );
				}
				, InVec2{ &writer, cuT( "v" ) } );

			// Shader inputs
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto plx_fragColor = writer.declFragData< Float >( cuT( "plx_fragColor" ), 0 );

			writer.implementFunction< Void >( cuT( "main" )
				, [&]()
				{
					plx_fragColor = snoise( vtx_texture ).r();
				} );

			return writer.finalise();
		}

		glsl::Shader getFragmentProgram( RenderSystem * renderSystem )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem->createGlslWriter();

			// Shader inputs
			glsl::Ubo filmGrain{ writer
				, FilmGrainUbo
				, BindingPoint };
			auto c3d_pixelSize = filmGrain.declMember< Vec2 >( PixelSize );
			auto c3d_noiseIntensity = filmGrain.declMember< Float >( NoiseIntensity );
			auto c3d_exposure = filmGrain.declMember< Float >( Exposure );
			auto c3d_time = filmGrain.declMember< Float >( Time );
			filmGrain.end();

			auto c3d_srcTex = writer.declUniform< Sampler2D >( SrcTex );
			auto c3d_noiseTex = writer.declUniform< Sampler3D >( NoiseTex );
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
				plx_fragColor = texture( c3d_noiseTex, vec3( vtx_texture, 0.0 ) );
			} );
			return writer.finalise();
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
			, params }
		, m_surface{ *renderSystem.getEngine() }
		, m_matrixUbo{ *renderSystem.getEngine() }
		, m_noise{ *renderSystem.getEngine() }
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
		m_mapSrc->setValue( 0 );
		m_mapNoise = program->createUniform< UniformType::eSampler >( NoiseTex
			, ShaderType::ePixel );
		m_mapNoise->setValue( 1 );
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
		result = m_surface.initialise( m_renderTarget
			, size
			, 0
			, m_sampler2D );

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
	}

	bool PostEffect::apply( FrameBuffer & framebuffer )
	{
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
		auto & cache = getRenderSystem()->getEngine()->getShaderProgramCache();
		auto texture = getRenderSystem()->createTexture( TextureType::eThreeDimensions
			, AccessType::eNone
			, AccessType::eRead | AccessType::eWrite
			, PixelFormat::eL32F
			, Point3ui{ 512u, 512u, 6u } );

		for ( auto & image : *texture )
		{
			image->initialiseSource();
		}

		m_noise.setTexture( texture );
		m_noise.setSampler( m_sampler3D );
		m_noise.setIndex( 1u );
		m_noise.initialise();

		auto vertex = getVertexProgram( getRenderSystem() );
		auto fragment = getNoiseProgram( getRenderSystem() );
		ShaderProgramSPtr program = getRenderSystem()->createShaderProgram();
		program->createObject( ShaderType::eVertex );
		program->createObject( ShaderType::ePixel );
		program->setSource( ShaderType::eVertex, vertex );
		program->setSource( ShaderType::ePixel, fragment );
		program->initialise();

		DepthStencilState dsstate;
		dsstate.setDepthTest( false );
		dsstate.setDepthMask( WritingMask::eZero );
		RasteriserState rsstate;
		rsstate.setCulledFaces( Culling::eBack );
		RenderPipelineSPtr noisePipeline = getRenderSystem()->createRenderPipeline( std::move( dsstate )
			, std::move( rsstate )
			, BlendState{}
			, MultisampleState{}
			, *program
			, PipelineFlags{} );
		noisePipeline->addUniformBuffer( m_matrixUbo.getUbo() );

		m_surface.m_fbo->bind( FrameBufferTarget::eDraw );
		auto attach = m_surface.m_fbo->createAttachment( texture );
		attach->setTarget( TextureType::eThreeDimensions );

		for ( uint32_t i = 0u; i < 6u; ++i )
		{
			attach->setLayer( i );
			attach->attach( AttachmentPoint::eColour );
			REQUIRE( m_surface.m_fbo->isComplete() );
			m_surface.m_fbo->setDrawBuffer( attach );
			getRenderSystem()->getCurrentContext()->renderTexture(
				m_surface.m_size
				, *texture
				, *noisePipeline
				, m_matrixUbo );
			attach->detach();
		}

		m_surface.m_fbo->attach( AttachmentPoint::eColour
			, 0
			, m_surface.m_colourAttach
			, m_surface.m_colourTexture.getType() );
		REQUIRE( m_surface.m_fbo->isComplete() );
		m_surface.m_fbo->setDrawBuffer( m_surface.m_colourAttach );
		m_surface.m_fbo->unbind();

		attach.reset();
		noisePipeline->cleanup();
	}
}
