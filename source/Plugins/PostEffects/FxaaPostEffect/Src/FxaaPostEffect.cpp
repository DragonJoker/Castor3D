#include "FxaaPostEffect.hpp"

#include <Engine.hpp>
#include <SamplerCache.hpp>
#include <ShaderCache.hpp>

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
#include <Render/Pipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Render/RenderTarget.hpp>
#include <Render/RenderWindow.hpp>
#include <Render/Viewport.hpp>
#include <Shader/FrameVariableBuffer.hpp>
#include <Shader/OneFrameVariable.hpp>
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
				LOCALE_ASSIGN( l_writer, Vec2, l_rcpFrame, vec2( 1.0 / c3d_fRenderTargetWidth, 1.0 / c3d_fRenderTargetHeight ) );
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
				LOCALE_ASSIGN( l_writer, Vec3, rgbNW, texture( p_tex, p_posPos.zw(), Float( 0.0f ) ).xyz() );
				LOCALE_ASSIGN( l_writer, Vec3, rgbNE, textureLodOffset( p_tex, p_posPos.zw(), Float( 0.0f ), ivec2( Int( 1 ), 0 ) ).xyz() );
				LOCALE_ASSIGN( l_writer, Vec3, rgbSW, textureLodOffset( p_tex, p_posPos.zw(), Float( 0.0f ), ivec2( Int( 0 ), 1 ) ).xyz() );
				LOCALE_ASSIGN( l_writer, Vec3, rgbSE, textureLodOffset( p_tex, p_posPos.zw(), Float( 0.0f ), ivec2( Int( 1 ), 1 ) ).xyz() );
				LOCALE_ASSIGN( l_writer, Vec3, rgbM, texture( p_tex, p_posPos.xy(), 0.0 ).xyz() );
				/*---------------------------------------------------------*/
				LOCALE_ASSIGN( l_writer, Vec3, luma, vec3( Float( 0.299f ), 0.587, 0.114 ) );
				LOCALE_ASSIGN( l_writer, Float, lumaNW, dot( rgbNW, luma ) );
				LOCALE_ASSIGN( l_writer, Float, lumaNE, dot( rgbNE, luma ) );
				LOCALE_ASSIGN( l_writer, Float, lumaSW, dot( rgbSW, luma ) );
				LOCALE_ASSIGN( l_writer, Float, lumaSE, dot( rgbSE, luma ) );
				LOCALE_ASSIGN( l_writer, Float, lumaM, dot( rgbM, luma ) );
				/*---------------------------------------------------------*/
				LOCALE_ASSIGN( l_writer, Float, lumaMin, min( lumaM, min( min( lumaNW, lumaNE ), min( lumaSW, lumaSE ) ) ) );
				LOCALE_ASSIGN( l_writer, Float, lumaMax, max( lumaM, max( max( lumaNW, lumaNE ), max( lumaSW, lumaSE ) ) ) );
				/*---------------------------------------------------------*/
				auto dir = l_writer.GetLocale< Vec2 >( "dir" );
				dir.x() = -l_writer.Paren( l_writer.Paren( lumaNW + lumaNE ) - l_writer.Paren( lumaSW + lumaSE ) );
				dir.y() = l_writer.Paren( l_writer.Paren( lumaNW + lumaSW ) - l_writer.Paren( lumaNE + lumaSE ) );
				/*---------------------------------------------------------*/
				LOCALE_ASSIGN( l_writer, Float, dirReduce, max( l_writer.Paren( lumaNW + lumaNE + lumaSW + lumaSE ) * l_writer.Paren( c3d_fReduceMul * 0.25f ), l_writer.Paren( Float( 1.0f ) / 128.0f ) ) );
				LOCALE_ASSIGN( l_writer, Float, rcpDirMin, Float( 1.0f ) / l_writer.Paren( min( GLSL::abs( dir.x() ), GLSL::abs( dir.y() ) ) + dirReduce ) );
				dir = min( vec2( c3d_fSpanMax, c3d_fSpanMax )
						   , max( vec2( -c3d_fSpanMax, -c3d_fSpanMax )
								  , dir * rcpDirMin ) ) * p_rcpFrame.xy();
				/*--------------------------------------------------------*/
				LOCALE_ASSIGN( l_writer, Vec3, rgbA
							   , l_writer.Paren( Float( 1.0f ) / 2.0 ) * l_writer.Paren(
								   texture( p_tex, p_posPos.xy() + dir * l_writer.Paren( Float( 1.0f ) / 3.0 - 0.5 ), 0.0 ).xyz() +
								   texture( p_tex, p_posPos.xy() + dir * l_writer.Paren( Float( 2.0f ) / 3.0 - 0.5 ), 0.0 ).xyz() ) );
				LOCALE_ASSIGN( l_writer, Vec3, rgbB
							   , rgbA * l_writer.Paren( Float( 1.0f ) / 2.0 ) + l_writer.Paren( Float( 1.0 ) / 4.0 ) * l_writer.Paren(
								   texture( p_tex, p_posPos.xy() + dir * l_writer.Paren( Float( 0.0f ) / 3.0 - 0.5 ), 0.0 ).xyz() +
								   texture( p_tex, p_posPos.xy() + dir * l_writer.Paren( Float( 3.0f ) / 3.0 - 0.5 ), 0.0 ).xyz() ) );
				LOCALE_ASSIGN( l_writer, Float, lumaB, dot( rgbB, luma ) );

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
				LOCALE_ASSIGN( l_writer, Vec4, c, vec4( Float( 0.0f ) ) );
				LOCALE_ASSIGN( l_writer, Vec2, rcpFrame, vec2( Float( 1.0f ) / c3d_fRenderTargetWidth, Float( 1.0f ) / c3d_fRenderTargetHeight ) );
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
	{
		String l_name = cuT( "FXAA" );

		if ( !m_renderTarget.GetEngine()->GetSamplerCache().Has( l_name ) )
		{
			m_sampler = m_renderTarget.GetEngine()->GetSamplerCache().Add( l_name );
			m_sampler->SetInterpolationMode( InterpolationFilter::Min, InterpolationMode::Nearest );
			m_sampler->SetInterpolationMode( InterpolationFilter::Mag, InterpolationMode::Nearest );
			m_sampler->SetWrappingMode( TextureUVW::U, WrapMode::ClampToBorder );
			m_sampler->SetWrappingMode( TextureUVW::V, WrapMode::ClampToBorder );
			m_sampler->SetWrappingMode( TextureUVW::W, WrapMode::ClampToBorder );
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
			ShaderProgramSPtr l_program = l_cache.GetNewProgram();
			m_mapDiffuse = l_program->CreateFrameVariable< OneIntFrameVariable >( ShaderProgram::MapDiffuse, ShaderType::Pixel );
			l_cache.CreateMatrixBuffer( *l_program, 0u, MASK_SHADER_TYPE_VERTEX );
			auto & l_uboFxaa = l_program->CreateFrameVariableBuffer( FxaaUbo, MASK_SHADER_TYPE_VERTEX | MASK_SHADER_TYPE_PIXEL );
			m_uniformSubpixShift = l_uboFxaa.CreateVariable< OneFloatFrameVariable >( SubpixShift );
			m_uniformSpanMax = l_uboFxaa.CreateVariable< OneFloatFrameVariable >( SpanMax );
			m_uniformReduceMul = l_uboFxaa.CreateVariable< OneFloatFrameVariable >( ReduceMul );
			m_uniformRenderTargetWidth = l_uboFxaa.CreateVariable< OneFloatFrameVariable >( RenderTargetWidth );
			m_uniformRenderTargetHeight = l_uboFxaa.CreateVariable< OneFloatFrameVariable >( RenderTargetHeight );
			l_program->SetSource( ShaderType::Vertex, l_model, l_vertex );
			l_program->SetSource( ShaderType::Pixel, l_model, l_fragment );
			l_program->Initialise();

			m_uniformSubpixShift->SetValue( m_subpixShift );
			m_uniformSpanMax->SetValue( m_spanMax );
			m_uniformReduceMul->SetValue( m_reduceMul );
			m_uniformRenderTargetWidth->SetValue( float( l_size.width() ) );
			m_uniformRenderTargetHeight->SetValue( float( l_size.height() ) );

			DepthStencilState l_dsstate;
			l_dsstate.SetDepthTest( false );
			l_dsstate.SetDepthMask( WritingMask::Zero );
			RasteriserState l_rsstate;
			l_rsstate.SetCulledFaces( Culling::Back );
			m_pipeline = GetRenderSystem()->CreatePipeline( std::move( l_dsstate ), std::move( l_rsstate ), BlendState{}, MultisampleState{}, *l_program, PipelineFlags{} );
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
		auto l_attach = p_framebuffer.GetAttachment( AttachmentPoint::Colour, 0 );

		if ( l_attach && l_attach->GetAttachmentType() == AttachmentType::Texture )
		{
			m_pipeline->Apply();
			bool l_return = m_surface.m_fbo->Bind( FrameBufferMode::Automatic, FrameBufferTarget::Draw );
			auto l_texture = std::static_pointer_cast< TextureAttachment >( l_attach )->GetTexture();

			if ( l_return )
			{
				m_surface.m_fbo->Clear();
				m_mapDiffuse->SetValue( 0 );
				GetRenderSystem()->GetCurrentContext()->RenderTexture( m_surface.m_size, *l_texture, *m_pipeline );
				m_surface.m_fbo->Unbind();
			}

			if ( p_framebuffer.Bind( FrameBufferMode::Automatic, FrameBufferTarget::Draw ) )
			{
				GetRenderSystem()->GetCurrentContext()->RenderTexture( l_texture->GetImage().GetDimensions(), *m_surface.m_colourTexture.GetTexture() );
				p_framebuffer.Unbind();
			}
		}

		return true;
	}

	bool FxaaPostEffect::DoWriteInto( TextFile & p_file )
	{
		return true;
	}
}
