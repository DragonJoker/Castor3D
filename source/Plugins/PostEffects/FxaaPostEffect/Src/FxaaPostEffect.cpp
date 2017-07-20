#include "FxaaPostEffect.hpp"

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
#include <Shader/MatrixUbo.hpp>
#include <Shader/ShaderProgram.hpp>
#include <State/BlendState.hpp>
#include <State/RasteriserState.hpp>
#include <Texture/Sampler.hpp>
#include <Texture/TextureLayout.hpp>
#include <Texture/TextureUnit.hpp>

#include <numeric>

#include <GlslSource.hpp>

using namespace Castor;

namespace fxaa
{
	namespace
	{
		static String const FxaaUbo = cuT( "FxaaUbo" );
		static String const SubpixShift = cuT( "c3d_fSubpixShift" );
		static String const RenderTargetWidth = cuT( "c3d_fRenderTargetWidth" );
		static String const RenderTargetHeight = cuT( "c3d_fRenderTargetHeight" );
		static String const SpanMax = cuT( "c3d_fSpanMax" );
		static String const ReduceMul = cuT( "c3d_fReduceMul" );
		static String const PosPos = cuT( "vtx_posPos" );

		GLSL::Shader GetVertexProgram( Castor3D::RenderSystem * p_renderSystem )
		{
			using namespace GLSL;
			GlslWriter writer = p_renderSystem->CreateGlslWriter();

			// Shader inputs
			UBO_MATRIX( writer );
			UBO_FXAA( writer );
			auto position = writer.DeclAttribute< Vec2 >( Castor3D::ShaderProgram::Position );

			// Shader outputs
			auto vtx_texture = writer.DeclOutput< Vec2 >( cuT( "vtx_texture" ) );
			auto vtx_posPos = writer.DeclOutput< Vec4 >( PosPos );
			auto gl_Position = writer.DeclBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.ImplementFunction< void >( cuT( "main" )
				, [&]()
				{
					auto invTargetSize = writer.DeclLocale( cuT( "invTargetSize" )
						, vec2( 1.0 / c3d_renderSize.x(), 1.0 / c3d_renderSize.y() ) );
					vtx_texture = position;
					gl_Position = c3d_mtxProjection * vec4( position.xy(), 0.0, 1.0 );
					vtx_posPos.xy() = position.xy();
					vtx_posPos.zw() = position.xy() - writer.Paren( invTargetSize * writer.Paren( 0.5 + c3d_subpixShift ) );
				} );
			return writer.Finalise();
		}

		GLSL::Shader GetFragmentProgram( Castor3D::RenderSystem * p_renderSystem )
		{
			using namespace GLSL;
			GlslWriter writer = p_renderSystem->CreateGlslWriter();

			// Shader inputs
			auto c3d_mapDiffuse = writer.DeclUniform< Sampler2D >( Castor3D::ShaderProgram::MapDiffuse );
			auto vtx_texture = writer.DeclInput< Vec2 >( cuT( "vtx_texture" ) );
			auto vtx_posPos = writer.DeclInput< Vec4 >( PosPos );

			UBO_FXAA( writer );

			// Shader outputs
			auto plx_v4FragColor = writer.DeclFragData< Vec4 >( cuT( "plx_v4FragColor" ), 0 );

#define FXAA_REDUCE_MIN	( 1.0 / 128.0 )

			writer.ImplementFunction< void >( cuT( "main" )
				, [&]()
				{
					auto invTargetSize = writer.DeclLocale( cuT( "invTargetSize" )
						, vec2( 1.0 / c3d_renderSize.x(), 1.0 / c3d_renderSize.y() ) );

					auto rgbNW = writer.DeclLocale( cuT( "rgbNW" )
						, textureLodOffset( c3d_mapDiffuse, vtx_texture, 0.0_f, ivec2( -1_i, -1_i ) ).rgb() );
					auto rgbNE = writer.DeclLocale( cuT( "rgbNE" )
						, textureLodOffset( c3d_mapDiffuse, vtx_texture, 0.0_f, ivec2( 1_i, -1_i ) ).rgb() );
					auto rgbSW = writer.DeclLocale( cuT( "rgbSW" )
						, textureLodOffset( c3d_mapDiffuse, vtx_texture, 0.0_f, ivec2( -1_i, 1_i ) ).rgb() );
					auto rgbSE = writer.DeclLocale( cuT( "rgbSE" )
						, textureLodOffset( c3d_mapDiffuse, vtx_texture, 0.0_f, ivec2( 1_i, 1_i ) ).rgb() );
					auto rgbM = writer.DeclLocale( cuT( "rgbM" )
						, texture( c3d_mapDiffuse, vtx_texture, 0.0_f ).rgb() );

					auto luma = writer.DeclLocale( cuT( "luma" )
						, vec3( 0.299_f, 0.587_f, 0.114_f ) );
					auto lumaNW = writer.DeclLocale( cuT( "lumaNW" )
						, dot( rgbNW, luma ) );
					auto lumaNE = writer.DeclLocale( cuT( "lumaNE" )
						, dot( rgbNE, luma ) );
					auto lumaSW = writer.DeclLocale( cuT( "lumaSW" )
						, dot( rgbSW, luma ) );
					auto lumaSE = writer.DeclLocale( cuT( "lumaSE" )
						, dot( rgbSE, luma ) );
					auto lumaM = writer.DeclLocale( cuT( "lumaM" )
						, dot( rgbM, luma ) );

					auto lumaMin = writer.DeclLocale( cuT( "lumaMin" )
						, min( lumaM, min( min( lumaNW, lumaNE ), min( lumaSW, lumaSE ) ) ) );
					auto lumaMax = writer.DeclLocale( cuT( "lumaMax" )
						, max( lumaM, max( max( lumaNW, lumaNE ), max( lumaSW, lumaSE ) ) ) );

					auto dir = writer.DeclLocale( cuT( "dir" )
						, vec2( -writer.Paren( writer.Paren( lumaNW + lumaNE ) - writer.Paren( lumaSW + lumaSE ) )
							, ( writer.Paren( lumaNW + lumaSW ) - writer.Paren( lumaNE + lumaSE ) ) ) );

					auto dirReduce = writer.DeclLocale( cuT( "dirReduce" )
						, max( writer.Paren( lumaNW + lumaNE + lumaSW + lumaSE ) * writer.Paren( 0.25_f * c3d_reduceMul ), FXAA_REDUCE_MIN ) );
					auto rcpDirMin = writer.DeclLocale( cuT( "rcpDirMin" )
						, 1.0 / ( min( GLSL::abs( dir.x() ), GLSL::abs( dir.y() ) ) + dirReduce ) );
					dir = min( vec2( c3d_spanMax, c3d_spanMax )
						, max( vec2( -c3d_spanMax, -c3d_spanMax )
							, dir * rcpDirMin ) ) * invTargetSize;

					auto rgbA = writer.DeclLocale( cuT( "rgbA" )
						, writer.Paren( texture( c3d_mapDiffuse, vtx_texture + dir * writer.Paren( 1.0_f / 3.0_f - 0.5_f ), 0.0_f ).rgb()
								+ texture( c3d_mapDiffuse, vtx_texture + dir * writer.Paren( 2.0_f / 3.0_f - 0.5_f ), 0.0_f ).rgb() )
							* writer.Paren( 1.0_f / 2.0_f ) );
					auto rgbB = writer.DeclLocale( cuT( "rgbB" )
						, writer.Paren( rgbA * 1.0_f / 2.0_f )
							+ writer.Paren( texture( c3d_mapDiffuse, vtx_texture + dir * writer.Paren( 0.0_f / 3.0_f - 0.5_f ), 0.0_f ).rgb()
									+ texture( c3d_mapDiffuse, vtx_texture + dir * writer.Paren( 3.0_f / 3.0_f - 0.5_f ), 0.0_f ).rgb() )
								* writer.Paren( 1.0_f / 4.0_f ) );
					auto lumaB = writer.DeclLocale( cuT( "lumaB" )
						, dot( rgbB, luma ) );

					plx_v4FragColor = vec4( writer.Ternary( Type{ cuT( "lumaB < lumaMin || lumaB > lumaMax" ) }, rgbA, rgbB ), 1.0_f );
				} );

			return writer.Finalise();
		}
	}

	//*********************************************************************************************

	String PostEffect::Type = cuT( "fxaa" );
	String PostEffect::Name = cuT( "FXAA PostEffect" );

	PostEffect::PostEffect( Castor3D::RenderTarget & p_renderTarget
		, Castor3D::RenderSystem & p_renderSystem
		, Castor3D::Parameters const & p_parameters )
		: Castor3D::PostEffect{ PostEffect::Type
			, p_renderTarget
			, p_renderSystem
			, p_parameters }
		, m_surface{ *p_renderSystem.GetEngine() }
		, m_matrixUbo{ *p_renderSystem.GetEngine() }
		, m_fxaaUbo{ *p_renderSystem.GetEngine() }
	{
		String param;

		if ( p_parameters.Get( cuT( "SubpixShift" ), param ) )
		{
			m_subpixShift = string::to_float( param );
		}

		if ( p_parameters.Get( cuT( "MaxSpan" ), param ) )
		{
			m_spanMax = string::to_float( param );
		}

		if ( p_parameters.Get( cuT( "ReduceMul" ), param ) )
		{
			m_reduceMul = string::to_float( param );
		}

		String name = cuT( "FXAA" );

		if ( !m_renderTarget.GetEngine()->GetSamplerCache().Has( name ) )
		{
			m_sampler = m_renderTarget.GetEngine()->GetSamplerCache().Add( name );
			m_sampler->SetInterpolationMode( Castor3D::InterpolationFilter::eMin, Castor3D::InterpolationMode::eNearest );
			m_sampler->SetInterpolationMode( Castor3D::InterpolationFilter::eMag, Castor3D::InterpolationMode::eNearest );
			m_sampler->SetWrappingMode( Castor3D::TextureUVW::eU, Castor3D::WrapMode::eClampToBorder );
			m_sampler->SetWrappingMode( Castor3D::TextureUVW::eV, Castor3D::WrapMode::eClampToBorder );
			m_sampler->SetWrappingMode( Castor3D::TextureUVW::eW, Castor3D::WrapMode::eClampToBorder );
		}
		else
		{
			m_sampler = m_renderTarget.GetEngine()->GetSamplerCache().Find( name );
		}
	}

	PostEffect::~PostEffect()
	{
	}

	Castor3D::PostEffectSPtr PostEffect::Create( Castor3D::RenderTarget & p_renderTarget
		, Castor3D::RenderSystem & p_renderSystem
		, Castor3D::Parameters const & p_param )
	{
		return std::make_shared< PostEffect >( p_renderTarget, p_renderSystem, p_param );
	}

	bool PostEffect::Initialise()
	{
		bool result = false;
		auto & cache = GetRenderSystem()->GetEngine()->GetShaderProgramCache();
		Size size = m_renderTarget.GetSize();

		auto vertex = GetVertexProgram( GetRenderSystem() );
		auto fragment = GetFragmentProgram( GetRenderSystem() );

		auto program = cache.GetNewProgram( false );
		program->CreateObject( Castor3D::ShaderType::eVertex );
		program->CreateObject( Castor3D::ShaderType::ePixel );
		m_mapDiffuse = program->CreateUniform< Castor3D::UniformType::eSampler >( Castor3D::ShaderProgram::MapDiffuse
			, Castor3D::ShaderType::ePixel );
		program->SetSource( Castor3D::ShaderType::eVertex, vertex );
		program->SetSource( Castor3D::ShaderType::ePixel, fragment );
		program->Initialise();

		Castor3D::DepthStencilState dsstate;
		dsstate.SetDepthTest( false );
		dsstate.SetDepthMask( Castor3D::WritingMask::eZero );
		Castor3D::RasteriserState rsstate;
		rsstate.SetCulledFaces( Castor3D::Culling::eBack );
		m_pipeline = GetRenderSystem()->CreateRenderPipeline( std::move( dsstate )
			, std::move( rsstate )
			, Castor3D::BlendState{}
			, Castor3D::MultisampleState{}
			, *program
			, Castor3D::PipelineFlags{} );
		m_pipeline->AddUniformBuffer( m_matrixUbo.GetUbo() );
		m_pipeline->AddUniformBuffer( m_fxaaUbo.GetUbo() );

		return m_surface.Initialise( m_renderTarget, size, 0, m_sampler );
	}

	void PostEffect::Cleanup()
	{
		m_matrixUbo.GetUbo().Cleanup();
		m_fxaaUbo.GetUbo().Cleanup();
		m_mapDiffuse.reset();
		m_surface.Cleanup();
	}

	bool PostEffect::Apply( Castor3D::FrameBuffer & p_framebuffer )
	{
		auto attach = p_framebuffer.GetAttachment( Castor3D::AttachmentPoint::eColour, 0 );

		if ( attach && attach->GetAttachmentType() == Castor3D::AttachmentType::eTexture )
		{
			m_fxaaUbo.Update( attach->GetBuffer()->dimensions()
				, m_subpixShift
				, m_spanMax
				, m_reduceMul );
			m_pipeline->Apply();
			m_surface.m_fbo->Bind( Castor3D::FrameBufferTarget::eDraw );
			auto texture = std::static_pointer_cast< Castor3D::TextureAttachment >( attach )->GetTexture();
			m_surface.m_fbo->Clear( Castor3D::BufferComponent::eColour );
			m_mapDiffuse->SetValue( 0 );
			GetRenderSystem()->GetCurrentContext()->RenderTexture( m_surface.m_size
				, *texture
				, *m_pipeline
				, m_matrixUbo );
			m_surface.m_fbo->Unbind();

			p_framebuffer.Bind( Castor3D::FrameBufferTarget::eDraw );
			GetRenderSystem()->GetCurrentContext()->RenderTexture( texture->GetDimensions(), *m_surface.m_colourTexture.GetTexture() );
			p_framebuffer.Unbind();
		}

		return true;
	}

	bool PostEffect::DoWriteInto( TextFile & p_file )
	{
		return true;
	}
}
