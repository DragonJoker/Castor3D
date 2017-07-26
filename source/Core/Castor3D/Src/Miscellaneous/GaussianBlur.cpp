#include "GaussianBlur.hpp"

#include "Engine.hpp"
#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "Mesh/Buffer/BufferElementGroup.hpp"
#include "Mesh/Buffer/GeometryBuffers.hpp"
#include "Mesh/Buffer/VertexBuffer.hpp"
#include "Render/RenderPassTimer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/Camera.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Shader/UniformBuffer.hpp"
#include "Shader/Uniform.hpp"
#include "State/BlendState.hpp"
#include "State/DepthStencilState.hpp"
#include "State/MultisampleState.hpp"
#include "State/RasteriserState.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"
#include "Texture/TextureUnit.hpp"

#include <GlslSource.hpp>
#include <GlslLight.hpp>
#include <GlslShadow.hpp>
#include <GlslUtils.hpp>

#include <random>

using namespace Castor;
using namespace Castor3D;

namespace Castor3D
{
	namespace
	{
		VertexBufferSPtr DoCreateVbo( Engine & engine )
		{
			auto declaration = BufferDeclaration(
			{
				BufferElementDeclaration( ShaderProgram::Position, uint32_t( ElementUsage::ePosition ), ElementType::eVec2 ),
			} );

			float data[] =
			{
				0, 0,
				1, 1,
				0, 1,
				0, 0,
				1, 0,
				1, 1
			};

			auto & renderSystem = *engine.GetRenderSystem();
			auto vertexBuffer = std::make_shared< VertexBuffer >( engine, declaration );
			uint32_t stride = declaration.stride();
			vertexBuffer->Resize( uint32_t( sizeof( data ) ) );
			uint8_t * buffer = vertexBuffer->GetData();
			std::memcpy( buffer, data, sizeof( data ) );
			vertexBuffer->Initialise( BufferAccessType::eStatic
				, BufferAccessNature::eDraw );
			return vertexBuffer;
		}

		GLSL::Shader GetVertexProgram( Engine & engine )
		{
			auto & renderSystem = *engine.GetRenderSystem();
			using namespace GLSL;
			auto writer = renderSystem.CreateGlslWriter();

			UBO_MATRIX( writer );

			// Shader inputs
			auto position = writer.DeclAttribute< Vec2 >( ShaderProgram::Position );

			// Shader outputs
			auto vtx_texture = writer.DeclOutput< Vec2 >( cuT( "vtx_texture" ) );
			auto gl_Position = writer.DeclBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.ImplementFunction< void >( cuT( "main" )
				, [&]()
				{
					vtx_texture = position;
					gl_Position = c3d_mtxProjection * vec4( position.x(), position.y(), 0.0, 1.0 );
				} );
			return writer.Finalise();
		}

		GLSL::Shader GetBlurXProgram( Engine & engine )
		{
			auto & renderSystem = *engine.GetRenderSystem();
			using namespace GLSL;
			auto writer = renderSystem.CreateGlslWriter();

			// Shader inputs
			Ubo config{ writer, GaussianBlur::Config, 2u };
			auto c3d_coefficientsCount = config.DeclMember< UInt >( GaussianBlur::CoefficientsCount );
			auto c3d_coefficients = config.DeclMember< Float >( GaussianBlur::Coefficients, GaussianBlur::MaxCoefficients );
			config.End();
			auto c3d_mapDiffuse = writer.DeclUniform< Sampler2D >( ShaderProgram::MapDiffuse );
			auto vtx_texture = writer.DeclInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto plx_v4FragColor = writer.DeclFragData< Vec4 >( cuT( "plx_v4FragColor" ), 0 );
			auto gl_FragDepth = writer.DeclBuiltin< Float >( cuT( "gl_FragDepth" ) );

			writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				auto base = writer.DeclLocale( cuT( "base" ), vec2( 1.0_f, 0 ) / textureSize( c3d_mapDiffuse, 0 ) );
				auto offset = writer.DeclLocale( cuT( "offset" ), vec2( 0.0_f, 0 ) );
				plx_v4FragColor = texture( c3d_mapDiffuse, vtx_texture ) * c3d_coefficients[0];

				FOR( writer, Int, i, 1, cuT( "i < c3d_coefficientsCount" ), cuT( "++i" ) )
				{
					offset += base;
					plx_v4FragColor += c3d_coefficients[i] * texture( c3d_mapDiffuse, vtx_texture - offset );
					plx_v4FragColor += c3d_coefficients[i] * texture( c3d_mapDiffuse, vtx_texture + offset );
				}
				ROF;

				gl_FragDepth = plx_v4FragColor.r();
			} );
			return writer.Finalise();
		}

		GLSL::Shader GetBlurYProgram( Engine & engine )
		{
			auto & renderSystem = *engine.GetRenderSystem();
			using namespace GLSL;
			auto writer = renderSystem.CreateGlslWriter();

			// Shader inputs
			Ubo config{ writer, GaussianBlur::Config, 2u };
			auto c3d_coefficientsCount = config.DeclMember< UInt >( GaussianBlur::CoefficientsCount );
			auto c3d_coefficients = config.DeclMember< Float >( GaussianBlur::Coefficients, GaussianBlur::MaxCoefficients );
			config.End();
			auto c3d_mapDiffuse = writer.DeclUniform< Sampler2D >( ShaderProgram::MapDiffuse );
			auto vtx_texture = writer.DeclInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto plx_v4FragColor = writer.DeclFragData< Vec4 >( cuT( "plx_v4FragColor" ), 0 );
			auto gl_FragDepth = writer.DeclBuiltin< Float >( cuT( "gl_FragDepth" ) );

			writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				auto base = writer.DeclLocale( cuT( "base" ), vec2( 0.0_f, 1 ) / textureSize( c3d_mapDiffuse, 0 ) );
				auto offset = writer.DeclLocale( cuT( "offset" ), vec2( 0.0_f, 0 ) );
				plx_v4FragColor = texture( c3d_mapDiffuse, vtx_texture ) * c3d_coefficients[0];

				FOR( writer, Int, i, 1, cuT( "i < c3d_coefficientsCount" ), cuT( "++i" ) )
				{
					offset += base;
					plx_v4FragColor += c3d_coefficients[i] * texture( c3d_mapDiffuse, vtx_texture - offset );
					plx_v4FragColor += c3d_coefficients[i] * texture( c3d_mapDiffuse, vtx_texture + offset );
				}
				ROF;

				gl_FragDepth = plx_v4FragColor.r();
			} );
			return writer.Finalise();
		}

		std::vector< float > GetHalfPascal( uint32_t height )
		{
			std::vector< float > result;
			result.resize( height );
			auto x = 1.0f;
			auto max = 1 + height;

			for ( uint32_t i = 0u; i <= max; ++i )
			{
				auto index = max - i;

				if ( index < height )
				{
					result[index] = x;
				}

				x = x * ( ( height + 1 ) * 2 - i ) / ( i + 1 );
			}

			// Normalize kernel coefficients
			float sum = result[0];

			for ( uint32_t i = 1u; i < height; ++i )
			{
				sum += result[i] * 2;
			}

			std::transform( std::begin( result )
				, std::end( result )
				, std::begin( result )
				, [&sum]( float & p_value )
			{
				return p_value /= sum;
			} );

			return result;
		}

		RenderPipelineUPtr DoCreatePipeline( Engine & engine
			, ShaderProgram & program )
		{
			DepthStencilState dsstate;
			dsstate.SetDepthTest( false );
			dsstate.SetDepthMask( WritingMask::eZero );
			RasteriserState rsstate;
			rsstate.SetCulledFaces( Culling::eNone );
			return engine.GetRenderSystem()->CreateRenderPipeline( std::move( dsstate )
				, std::move( rsstate )
				, BlendState{}
				, MultisampleState{}
				, program
				, PipelineFlags{} );
		}

		SamplerSPtr DoCreateSampler( Engine & engine
			, String const & name )
		{
			SamplerSPtr sampler;

			if ( engine.GetSamplerCache().Has( name ) )
			{
				sampler = engine.GetSamplerCache().Find( name );
			}
			else
			{
				sampler = engine.GetSamplerCache().Add( name );
				sampler->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eNearest );
				sampler->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eNearest );
				sampler->SetWrappingMode( TextureUVW::eU, WrapMode::eClampToEdge );
				sampler->SetWrappingMode( TextureUVW::eV, WrapMode::eClampToEdge );
			}

			return sampler;
		}

		TextureUnit DoCreateTexture( Engine & engine
			, Size const & size
			, PixelFormat format
			, String const & name )
		{
			auto & renderSystem = *engine.GetRenderSystem();
			auto sampler = DoCreateSampler( engine, name );
			auto texture = renderSystem.CreateTexture( TextureType::eTwoDimensions
				, AccessType::eNone
				, AccessType::eRead | AccessType::eWrite
				, format
				, size );
			texture->GetImage().InitialiseSource();
			TextureUnit unit{ engine };
			unit.SetTexture( texture );
			unit.SetSampler( sampler );
			unit.SetIndex( 0u );
			unit.Initialise();
			return unit;
		}

		FrameBufferSPtr DoCreateFbo( Engine & engine
			, Size const & size )
		{
			auto & renderSystem = *engine.GetRenderSystem();
			auto fbo = renderSystem.CreateFrameBuffer();
			fbo->Create();
			fbo->Initialise( size );
			return fbo;
		}

		AttachmentPoint DoGetAttachmentPoint( PixelFormat format )
		{
			return format == PixelFormat::eD16
					|| format == PixelFormat::eD24
					|| format == PixelFormat::eD24S8
					|| format == PixelFormat::eD32
					|| format == PixelFormat::eD32F
					|| format == PixelFormat::eD32FS8
				? AttachmentPoint::eDepth
				: AttachmentPoint::eColour;
		}

		TextureAttachmentSPtr DoCreateAttach( FrameBuffer & fbo
			, TextureUnit const & unit
			, AttachmentPoint point )
		{
			auto attach = fbo.CreateAttachment( unit.GetTexture() );
			fbo.Bind();
			fbo.Attach( point, 0u, attach, unit.GetTexture()->GetType() );
			fbo.SetDrawBuffer( attach );
			ENSURE( fbo.IsComplete() );
			fbo.Unbind();
			return attach;
		}
	}

	//*********************************************************************************************

	String const GaussianBlur::Config = cuT( "Config" );
	String const GaussianBlur::Coefficients = cuT( "c3d_coefficients" );
	String const GaussianBlur::CoefficientsCount = cuT( "c3d_coefficientsCount" );

	GaussianBlur::GaussianBlur( Engine & engine
		, Size const & textureSize
		, PixelFormat format
		, uint32_t kernelSize )
		: OwnedBy< Engine >{ engine }
		, m_size{ textureSize }
		, m_matrixUbo{ engine }
		, m_point{ DoGetAttachmentPoint( format ) }
		, m_colour{ DoCreateTexture( engine, textureSize, format, cuT( "GaussianBlur" ) ) }
		, m_fbo{ DoCreateFbo( engine, textureSize ) }
		, m_colourAttach{ DoCreateAttach( *m_fbo, m_colour, m_point ) }
		, m_kernel{ GetHalfPascal( kernelSize ) }
		, m_blurXUbo{ GaussianBlur::Config, *engine.GetRenderSystem(), 2u }
		, m_blurYUbo{ GaussianBlur::Config, *engine.GetRenderSystem(), 2u }
		, m_blurXCoeffCount{ m_blurXUbo.CreateUniform< UniformType::eUInt >( GaussianBlur::CoefficientsCount ) }
		, m_blurXCoeffs{ m_blurXUbo.CreateUniform< UniformType::eFloat >( GaussianBlur::Coefficients, GaussianBlur::MaxCoefficients ) }
		, m_blurYCoeffCount{ m_blurYUbo.CreateUniform< UniformType::eUInt >( GaussianBlur::CoefficientsCount ) }
		, m_blurYCoeffs{ m_blurYUbo.CreateUniform< UniformType::eFloat >( GaussianBlur::Coefficients, GaussianBlur::MaxCoefficients ) }
	{
		DoInitialiseBlurXProgram( engine );
		DoInitialiseBlurYProgram( engine );
	}

	GaussianBlur::~GaussianBlur()
	{
		m_blurXUbo.Cleanup();
		m_blurXCoeffCount.reset();
		m_blurXCoeffs.reset();
		m_blurXMapDiffuse.reset();
		m_blurYPipeline->Cleanup();
		m_blurYPipeline.reset();

		m_blurYUbo.Cleanup();
		m_blurYCoeffCount.reset();
		m_blurYCoeffs.reset();
		m_blurYMapDiffuse.reset();
		m_blurXPipeline->Cleanup();
		m_blurXPipeline.reset();

		m_matrixUbo.GetUbo().Cleanup();
		m_fbo->Bind();
		m_fbo->DetachAll();
		m_fbo->Unbind();
		m_fbo->Cleanup();
		m_fbo->Destroy();
		m_fbo.reset();
		m_colourAttach.reset();
		m_colour.Cleanup();
	}

	void GaussianBlur::Blur( TextureLayoutSPtr texture )
	{
		Blur( texture, m_colour.GetTexture() );
	}

	void GaussianBlur::Blur( TextureLayoutSPtr texture
		, TextureLayoutSPtr intermediate )
	{
		REQUIRE( texture->GetPixelFormat() == intermediate->GetPixelFormat() );
		DoBlur( texture, intermediate, *m_blurXPipeline, m_blurXUbo );
		DoBlur( intermediate, texture, *m_blurYPipeline, m_blurYUbo );
	}

	void GaussianBlur::DoBlur( TextureLayoutSPtr & source
		, TextureLayoutSPtr & destination
		, RenderPipeline & pipeline
		, UniformBuffer & ubo )
	{
		auto context = GetEngine()->GetRenderSystem()->GetCurrentContext();
		ubo.BindTo( 2u );
		m_fbo->Bind( FrameBufferTarget::eDraw );
		auto attach = m_fbo->CreateAttachment( destination );
		attach->SetLayer( 0u );
		attach->SetTarget( TextureType::eTwoDimensions );
		attach->Attach( m_point, 0u );
		m_fbo->SetDrawBuffer( attach );
		m_fbo->Clear( BufferComponent::eColour );
		context->RenderTexture( source->GetDimensions()
			, *source
			, pipeline
			, m_matrixUbo );
		m_fbo->Unbind();
	}

	bool GaussianBlur::DoInitialiseBlurXProgram( Engine & engine )
	{
		auto & renderSystem = *engine.GetRenderSystem();
		auto & cache = engine.GetShaderProgramCache();
		auto const vertex = GetVertexProgram( engine );
		auto const blurX = GetBlurXProgram( engine );

		ShaderProgramSPtr program = cache.GetNewProgram( false );
		program->CreateObject( ShaderType::eVertex );
		program->CreateObject( ShaderType::ePixel );
		m_blurXMapDiffuse = program->CreateUniform< UniformType::eSampler >( ShaderProgram::MapDiffuse
			, ShaderType::ePixel );
		m_blurXCoeffCount->SetValue( uint32_t( m_kernel.size() ) );
		m_blurXCoeffs->SetValues( m_kernel );

		program->SetSource( ShaderType::eVertex, vertex );
		program->SetSource( ShaderType::ePixel, blurX );
		bool result = program->Initialise();

		if ( result )
		{
			DepthStencilState dsstate;
			dsstate.SetDepthTest( false );
			dsstate.SetDepthMask( WritingMask::eZero );
			m_blurXPipeline = renderSystem.CreateRenderPipeline( std::move( dsstate )
				, RasteriserState{}
				, BlendState{}
				, MultisampleState{}
				, *program
				, PipelineFlags{} );
			m_blurXPipeline->AddUniformBuffer( m_matrixUbo.GetUbo() );
			m_blurXPipeline->AddUniformBuffer( m_blurXUbo );
		}

		return result;
	}

	bool GaussianBlur::DoInitialiseBlurYProgram( Engine & engine )
	{
		auto & renderSystem = *engine.GetRenderSystem();
		auto & cache = engine.GetShaderProgramCache();
		auto const vertex = GetVertexProgram( engine );
		auto const blurY = GetBlurYProgram( engine );

		ShaderProgramSPtr program = cache.GetNewProgram( false );
		program->CreateObject( ShaderType::eVertex );
		program->CreateObject( ShaderType::ePixel );
		m_blurYMapDiffuse = program->CreateUniform< UniformType::eSampler >( ShaderProgram::MapDiffuse
			, ShaderType::ePixel );
		m_blurYCoeffCount->SetValue( uint32_t( m_kernel.size() ) );
		m_blurYCoeffs->SetValues( m_kernel );

		program->SetSource( ShaderType::eVertex, vertex );
		program->SetSource( ShaderType::ePixel, blurY );
		bool result = program->Initialise();

		if ( result )
		{
			DepthStencilState dsstate;
			dsstate.SetDepthTest( false );
			dsstate.SetDepthMask( WritingMask::eZero );
			m_blurYPipeline = renderSystem.CreateRenderPipeline( std::move( dsstate )
				, RasteriserState{}
				, BlendState{}
				, MultisampleState{}
				, *program
				, PipelineFlags{} );
			m_blurYPipeline->AddUniformBuffer( m_matrixUbo.GetUbo() );
			m_blurYPipeline->AddUniformBuffer( m_blurYUbo );
		}

		return result;
	}
}
