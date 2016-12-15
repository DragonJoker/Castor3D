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
#include <Shader/ShaderProgram.hpp>
#include <State/BlendState.hpp>
#include <State/RasteriserState.hpp>
#include <Texture/Sampler.hpp>
#include <Texture/TextureLayout.hpp>
#include <Texture/TextureUnit.hpp>

#include <numeric>

#include <GlslSource.hpp>

using namespace Castor;
using namespace Castor3D;

namespace Fxaa
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

		String GetVertexProgram( RenderSystem * p_renderSystem )
		{
			using namespace GLSL;
			GlslWriter l_writer = p_renderSystem->CreateGlslWriter();

			// Shader inputs
			UBO_MATRIX( l_writer );
			Ubo l_fxaa{ l_writer, FxaaUbo };
			auto c3d_fSubpixShift = l_fxaa.GetUniform< Float >( SubpixShift );
			auto c3d_fSpanMax = l_fxaa.GetUniform< Float >( SpanMax );
			auto c3d_fReduceMul = l_fxaa.GetUniform< Float >( ReduceMul );
			auto c3d_fRenderTargetWidth = l_fxaa.GetUniform< Float >( RenderTargetWidth );
			auto c3d_fRenderTargetHeight = l_fxaa.GetUniform< Float >( RenderTargetHeight );
			l_fxaa.End();
			auto position = l_writer.GetAttribute< Vec2 >( ShaderProgram::Position );

			// Shader outputs
			auto vtx_texture = l_writer.GetOutput< Vec2 >( cuT( "vtx_texture" ) );
			auto vtx_posPos = l_writer.GetOutput< Vec4 >( PosPos );
			auto gl_Position = l_writer.GetBuiltin< Vec4 >( cuT( "gl_Position" ) );

			l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				vtx_texture = position;
				gl_Position = c3d_mtxProjection * vec4( position.xy(), 0.0, 1.0 );
				auto l_rcpFrame = l_writer.GetLocale( cuT( "l_rcpFrame" ), vec2( 1.0 / c3d_fRenderTargetWidth, 1.0 / c3d_fRenderTargetHeight ) );
				vtx_posPos.xy() = position.xy();
				vtx_posPos.zw() = position.xy() - l_writer.Paren( l_rcpFrame * l_writer.Paren( 0.5 + c3d_fSubpixShift ) );
			} );
			return l_writer.Finalise();
		}

		Castor::String GetFragmentProgram( RenderSystem * p_renderSystem )
		{
			using namespace GLSL;
			GlslWriter l_writer = p_renderSystem->CreateGlslWriter();

			// Shader inputs
			auto c3d_mapDiffuse = l_writer.GetUniform< Sampler2D >( ShaderProgram::MapDiffuse );
			auto vtx_texture = l_writer.GetInput< Vec2 >( cuT( "vtx_texture" ) );
			auto vtx_posPos = l_writer.GetInput< Vec4 >( PosPos );
			Ubo l_fxaa{ l_writer, FxaaUbo };
			auto c3d_fSubpixShift = l_fxaa.GetUniform< Float >( SubpixShift );
			auto c3d_fSpanMax = l_fxaa.GetUniform< Float >( SpanMax );
			auto c3d_fReduceMul = l_fxaa.GetUniform< Float >( ReduceMul );
			auto c3d_fRenderTargetWidth = l_fxaa.GetUniform< Float >( RenderTargetWidth );
			auto c3d_fRenderTargetHeight = l_fxaa.GetUniform< Float >( RenderTargetHeight );
			l_fxaa.End();

			// Shader outputs
			auto plx_v4FragColor = l_writer.GetFragData< Vec4 >( cuT( "plx_v4FragColor" ), 0 );

			l_writer.ImplementFunction< Vec3 >( cuT( "FxaaPixelShader" ), [&]( Vec4 const & p_posPos
																			   , Sampler2D const & p_tex
																			   , Vec2 const & p_rcpFrame )
			{
				/*---------------------------------------------------------*/
				auto rgbNW = l_writer.GetLocale( cuT( "rgbNW" ), texture( p_tex, p_posPos.zw(), Float( 0.0f ) ).xyz() );
				auto rgbNE = l_writer.GetLocale( cuT( "rgbNE" ), textureLodOffset( p_tex, p_posPos.zw(), Float( 0.0f ), ivec2( Int( 1 ), 0 ) ).xyz() );
				auto rgbSW = l_writer.GetLocale( cuT( "rgbSW" ), textureLodOffset( p_tex, p_posPos.zw(), Float( 0.0f ), ivec2( Int( 0 ), 1 ) ).xyz() );
				auto rgbSE = l_writer.GetLocale( cuT( "rgbSE" ), textureLodOffset( p_tex, p_posPos.zw(), Float( 0.0f ), ivec2( Int( 1 ), 1 ) ).xyz() );
				auto rgbM = l_writer.GetLocale( cuT( "rgbM" ), texture( p_tex, p_posPos.xy(), 0.0 ).xyz() );
				/*---------------------------------------------------------*/
				auto luma = l_writer.GetLocale( cuT( "luma" ), vec3( Float( 0.299f ), 0.587, 0.114 ) );
				auto lumaNW = l_writer.GetLocale( cuT( "lumaNW" ), dot( rgbNW, luma ) );
				auto lumaNE = l_writer.GetLocale( cuT( "lumaNE" ), dot( rgbNE, luma ) );
				auto lumaSW = l_writer.GetLocale( cuT( "lumaSW" ), dot( rgbSW, luma ) );
				auto lumaSE = l_writer.GetLocale( cuT( "lumaSE" ), dot( rgbSE, luma ) );
				auto lumaM = l_writer.GetLocale( cuT( "lumaM" ), dot( rgbM, luma ) );
				/*---------------------------------------------------------*/
				auto lumaMin = l_writer.GetLocale( cuT( "lumaMin" ), min( lumaM, min( min( lumaNW, lumaNE ), min( lumaSW, lumaSE ) ) ) );
				auto lumaMax = l_writer.GetLocale( cuT( "lumaMax" ), max( lumaM, max( max( lumaNW, lumaNE ), max( lumaSW, lumaSE ) ) ) );
				/*---------------------------------------------------------*/
				auto dir = l_writer.GetLocale< Vec2 >( "dir" );
				dir.x() = -l_writer.Paren( l_writer.Paren( lumaNW + lumaNE ) - l_writer.Paren( lumaSW + lumaSE ) );
				dir.y() = l_writer.Paren( l_writer.Paren( lumaNW + lumaSW ) - l_writer.Paren( lumaNE + lumaSE ) );
				/*---------------------------------------------------------*/
				auto dirReduce = l_writer.GetLocale( cuT( "dirReduce" ), max( l_writer.Paren( lumaNW + lumaNE + lumaSW + lumaSE ) * l_writer.Paren( c3d_fReduceMul * 0.25f ), l_writer.Paren( Float( 1.0f ) / 128.0f ) ) );
				auto rcpDirMin = l_writer.GetLocale( cuT( "rcpDirMin" ), Float( 1.0f ) / l_writer.Paren( min( GLSL::abs( dir.x() ), GLSL::abs( dir.y() ) ) + dirReduce ) );
				dir = min( vec2( c3d_fSpanMax, c3d_fSpanMax )
						   , max( vec2( -c3d_fSpanMax, -c3d_fSpanMax )
								  , dir * rcpDirMin ) ) * p_rcpFrame.xy();
				/*--------------------------------------------------------*/
				auto rgbA = l_writer.GetLocale( cuT( "rgbA" )
												, l_writer.Paren( l_writer.Paren(
														texture( p_tex, p_posPos.xy() + dir * l_writer.Paren( Float( 1.0f ) / 3.0 - 0.5 ), 0.0 ).xyz() +
														texture( p_tex, p_posPos.xy() + dir * l_writer.Paren( Float( 2.0f ) / 3.0 - 0.5 ), 0.0 ).xyz() ) * Float( 1.0f ) / 2.0 ) );
				auto rgbB = l_writer.GetLocale( cuT( "rgbB" )
												, rgbA * l_writer.Paren( Float( 1.0f ) / 2.0 ) + l_writer.Paren( Float( 1.0 ) / 4.0 ) * l_writer.Paren(
													texture( p_tex, p_posPos.xy() + dir * l_writer.Paren( Float( 0.0f ) / 3.0 - 0.5 ), 0.0 ).xyz() +
													texture( p_tex, p_posPos.xy() + dir * l_writer.Paren( Float( 3.0f ) / 3.0 - 0.5 ), 0.0 ).xyz() ) );
				auto lumaB = l_writer.GetLocale( cuT( "lumaB" ), dot( rgbB, luma ) );

				IF( l_writer, "( lumaB < lumaMin ) || ( lumaB > lumaMax )" )
				{
					l_writer.Return( rgbA );
				}
				FI;

				l_writer.Return( rgbB );
			}, InParam< Vec4 >( &l_writer, cuT( "p_posPos" ) )
				, InParam< Sampler2D >( &l_writer, cuT( "p_tex" ) )
				, InParam< Vec2 >( &l_writer, cuT( "p_rcpFrame" ) ) );

			l_writer.ImplementFunction< Vec4 >( cuT( "PostFX" ), [&]( Sampler2D const & p_tex
																	  , Vec2 const & p_uv
																	  , Float const & p_time )
			{
				auto c = l_writer.GetLocale( cuT( "c" ), vec4( Float( 0.0f ) ) );
				auto rcpFrame = l_writer.GetLocale( cuT( "rcpFrame" ), vec2( Float( 1.0f ) / c3d_fRenderTargetWidth, Float( 1.0f ) / c3d_fRenderTargetHeight ) );
				c.rgb() = WriteFunctionCall< Vec3 >( &l_writer, cuT( "FxaaPixelShader" ), vtx_posPos, p_tex, rcpFrame );
				//c.rgb = 1.0 - texture2D(p_tex, vtx_posPos.xy).rgb;
				c.a() = Float( 1.0f );
				l_writer.Return( c );
			}, InParam< Sampler2D >( &l_writer, cuT( "p_tex" ) )
				, InParam< Vec2 >( &l_writer, cuT( "p_uv" ) )
				, InParam< Float >( &l_writer, cuT( "p_time" ) ) );

			l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				plx_v4FragColor = WriteFunctionCall< Vec4 >( &l_writer, cuT( "PostFX" ), c3d_mapDiffuse, vtx_texture, 0.0 );
			} );

			return l_writer.Finalise();
		}
	}

	//*********************************************************************************************

	String FxaaPostEffect::Type = cuT( "fxaa" );
	String FxaaPostEffect::Name = cuT( "FXAA PostEffect" );

	FxaaPostEffect::FxaaPostEffect( RenderTarget & p_renderTarget, RenderSystem & p_renderSystem, Parameters const & p_param )
		: PostEffect{ FxaaPostEffect::Type, p_renderTarget, p_renderSystem, p_param }
		, m_surface{ *p_renderSystem.GetEngine() }
		, m_matrixUbo{ ShaderProgram::BufferMatrix, p_renderSystem }
		, m_fxaaUbo{ FxaaUbo, p_renderSystem }
	{
		UniformBuffer::FillMatrixBuffer( m_matrixUbo );
		m_uniformSubpixShift = m_fxaaUbo.CreateUniform< UniformType::eFloat >( SubpixShift );
		m_uniformSpanMax = m_fxaaUbo.CreateUniform< UniformType::eFloat >( SpanMax );
		m_uniformReduceMul = m_fxaaUbo.CreateUniform< UniformType::eFloat >( ReduceMul );
		m_uniformRenderTargetWidth = m_fxaaUbo.CreateUniform< UniformType::eFloat >( RenderTargetWidth );
		m_uniformRenderTargetHeight = m_fxaaUbo.CreateUniform< UniformType::eFloat >( RenderTargetHeight );

		String l_name = cuT( "FXAA" );

		if ( !m_renderTarget.GetEngine()->GetSamplerCache().Has( l_name ) )
		{
			m_sampler = m_renderTarget.GetEngine()->GetSamplerCache().Add( l_name );
			m_sampler->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eNearest );
			m_sampler->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eNearest );
			m_sampler->SetWrappingMode( TextureUVW::eU, WrapMode::eClampToBorder );
			m_sampler->SetWrappingMode( TextureUVW::eV, WrapMode::eClampToBorder );
			m_sampler->SetWrappingMode( TextureUVW::eW, WrapMode::eClampToBorder );
		}
		else
		{
			m_sampler = m_renderTarget.GetEngine()->GetSamplerCache().Find( l_name );
		}
	}

	FxaaPostEffect::~FxaaPostEffect()
	{
	}

	PostEffectSPtr FxaaPostEffect::Create( RenderTarget & p_renderTarget, RenderSystem & p_renderSystem, Parameters const & p_param )
	{
		return std::make_shared< FxaaPostEffect >( p_renderTarget, p_renderSystem, p_param );
	}

	bool FxaaPostEffect::Initialise()
	{
		bool l_return = false;
		auto & l_cache = GetRenderSystem()->GetEngine()->GetShaderProgramCache();
		ShaderModel l_model = GetRenderSystem()->GetGpuInformations().GetMaxShaderModel();
		Size l_size = m_renderTarget.GetSize();

		auto l_vertex = GetVertexProgram( GetRenderSystem() );
		auto l_fragment = GetFragmentProgram( GetRenderSystem() );

		if ( !l_vertex.empty() && !l_fragment.empty() )
		{
			ShaderProgramSPtr l_program = l_cache.GetNewProgram( false );
			l_program->CreateObject( ShaderType::eVertex );
			l_program->CreateObject( ShaderType::ePixel );
			m_mapDiffuse = l_program->CreateUniform< UniformType::eSampler >( ShaderProgram::MapDiffuse, ShaderType::ePixel );
			l_program->SetSource( ShaderType::eVertex, l_model, l_vertex );
			l_program->SetSource( ShaderType::ePixel, l_model, l_fragment );
			l_program->Initialise();

			m_uniformSubpixShift->SetValue( m_subpixShift );
			m_uniformSpanMax->SetValue( m_spanMax );
			m_uniformReduceMul->SetValue( m_reduceMul );
			m_uniformRenderTargetWidth->SetValue( float( l_size.width() ) );
			m_uniformRenderTargetHeight->SetValue( float( l_size.height() ) );

			DepthStencilState l_dsstate;
			l_dsstate.SetDepthTest( false );
			l_dsstate.SetDepthMask( WritingMask::eZero );
			RasteriserState l_rsstate;
			l_rsstate.SetCulledFaces( Culling::eBack );
			m_pipeline = GetRenderSystem()->CreateRenderPipeline( std::move( l_dsstate ), std::move( l_rsstate ), BlendState{}, MultisampleState{}, *l_program, PipelineFlags{} );
			m_pipeline->AddUniformBuffer( m_matrixUbo );
			m_pipeline->AddUniformBuffer( m_fxaaUbo );
		}

		return m_surface.Initialise( m_renderTarget, l_size, 0, m_sampler );
	}

	void FxaaPostEffect::Cleanup()
	{
		m_mapDiffuse.reset();
		m_surface.Cleanup();
	}

	bool FxaaPostEffect::Apply( FrameBuffer & p_framebuffer )
	{
		auto l_attach = p_framebuffer.GetAttachment( AttachmentPoint::eColour, 0 );

		if ( l_attach && l_attach->GetAttachmentType() == AttachmentType::eTexture )
		{
			m_pipeline->Apply();
			m_surface.m_fbo->Bind( FrameBufferMode::eAutomatic, FrameBufferTarget::eDraw );
			auto l_texture = std::static_pointer_cast< TextureAttachment >( l_attach )->GetTexture();
			m_surface.m_fbo->Clear();
			m_mapDiffuse->SetValue( 0 );
			GetRenderSystem()->GetCurrentContext()->RenderTexture( m_surface.m_size
				, *l_texture
				, *m_pipeline
				, m_matrixUbo );
			m_surface.m_fbo->Unbind();

			p_framebuffer.Bind( FrameBufferMode::eAutomatic, FrameBufferTarget::eDraw );
			GetRenderSystem()->GetCurrentContext()->RenderTexture( l_texture->GetDimensions(), *m_surface.m_colourTexture.GetTexture() );
			p_framebuffer.Unbind();
		}

		return true;
	}

	bool FxaaPostEffect::DoWriteInto( TextFile & p_file )
	{
		return true;
	}
}
