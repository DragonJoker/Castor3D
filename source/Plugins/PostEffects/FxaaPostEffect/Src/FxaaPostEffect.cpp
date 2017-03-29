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

			l_writer.ImplementFunction< void >( cuT( "main" )
				, [&]()
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

#define FXAA_REDUCE_MIN	( 1.0 / 128.0 )

			l_writer.ImplementFunction< void >( cuT( "main" )
				, [&]()
				{
					auto l_invTargetSize = l_writer.GetLocale( cuT( "l_invTargetSize" )
						, vec2( Float( 1.0f ) / c3d_fRenderTargetWidth, Float( 1.0f ) / c3d_fRenderTargetHeight ) );

					auto l_rgbNW = l_writer.GetLocale( cuT( "l_rgbNW" )
						, textureLodOffset( c3d_mapDiffuse, vtx_texture, 0.0_f, ivec2( -1_i, -1_i ) ).rgb() );
					auto l_rgbNE = l_writer.GetLocale( cuT( "l_rgbNE" )
						, textureLodOffset( c3d_mapDiffuse, vtx_texture, 0.0_f, ivec2( 1_i, -1_i ) ).rgb() );
					auto l_rgbSW = l_writer.GetLocale( cuT( "l_rgbSW" )
						, textureLodOffset( c3d_mapDiffuse, vtx_texture, 0.0_f, ivec2( -1_i, 1_i ) ).rgb() );
					auto l_rgbSE = l_writer.GetLocale( cuT( "l_rgbSE" )
						, textureLodOffset( c3d_mapDiffuse, vtx_texture, 0.0_f, ivec2( 1_i, 1_i ) ).rgb() );
					auto l_rgbM = l_writer.GetLocale( cuT( "l_rgbM" )
						, texture( c3d_mapDiffuse, vtx_texture, 0.0_f ).rgb() );

					auto l_luma = l_writer.GetLocale( cuT( "l_luma" )
						, vec3( 0.299_f, 0.587_f, 0.114_f ) );
					auto l_lumaNW = l_writer.GetLocale( cuT( "l_lumaNW" )
						, dot( l_rgbNW, l_luma ) );
					auto l_lumaNE = l_writer.GetLocale( cuT( "l_lumaNE" )
						, dot( l_rgbNE, l_luma ) );
					auto l_lumaSW = l_writer.GetLocale( cuT( "l_lumaSW" )
						, dot( l_rgbSW, l_luma ) );
					auto l_lumaSE = l_writer.GetLocale( cuT( "l_lumaSE" )
						, dot( l_rgbSE, l_luma ) );
					auto l_lumaM = l_writer.GetLocale( cuT( "l_lumaM" )
						, dot( l_rgbM, l_luma ) );

					auto l_lumaMin = l_writer.GetLocale( cuT( "l_lumaMin" )
						, min( l_lumaM, min( min( l_lumaNW, l_lumaNE ), min( l_lumaSW, l_lumaSE ) ) ) );
					auto l_lumaMax = l_writer.GetLocale( cuT( "l_lumaMax" )
						, max( l_lumaM, max( max( l_lumaNW, l_lumaNE ), max( l_lumaSW, l_lumaSE ) ) ) );

					auto l_dir = l_writer.GetLocale( cuT( "l_dir" )
						, vec2( -l_writer.Paren( l_writer.Paren( l_lumaNW + l_lumaNE ) - l_writer.Paren( l_lumaSW + l_lumaSE ) )
							, ( l_writer.Paren( l_lumaNW + l_lumaSW ) - l_writer.Paren( l_lumaNE + l_lumaSE ) ) ) );

					auto l_dirReduce = l_writer.GetLocale( cuT( "l_dirReduce" )
						, max( l_writer.Paren( l_lumaNW + l_lumaNE + l_lumaSW + l_lumaSE ) * l_writer.Paren( 0.25_f * c3d_fReduceMul ), FXAA_REDUCE_MIN ) );
					auto l_rcpDirMin = l_writer.GetLocale( cuT( "l_rcpDirMin" )
						, 1.0 / ( min( GLSL::abs( l_dir.x() ), GLSL::abs( l_dir.y() ) ) + l_dirReduce ) );
					l_dir = min( vec2( c3d_fSpanMax, c3d_fSpanMax )
						, max( vec2( -c3d_fSpanMax, -c3d_fSpanMax )
							, l_dir * l_rcpDirMin ) ) * l_invTargetSize;

					auto l_rgbA = l_writer.GetLocale( cuT( "l_rgbA" )
						, l_writer.Paren( texture( c3d_mapDiffuse, vtx_texture + l_dir * l_writer.Paren( 1.0_f / 3.0_f - 0.5_f ), 0.0_f ).rgb()
								+ texture( c3d_mapDiffuse, vtx_texture + l_dir * l_writer.Paren( 2.0_f / 3.0_f - 0.5_f ), 0.0_f ).rgb() )
							* l_writer.Paren( 1.0_f / 2.0_f ) );
					auto l_rgbB = l_writer.GetLocale( cuT( "l_rgbB" )
						, l_writer.Paren( l_rgbA * 1.0_f / 2.0_f )
							+ l_writer.Paren( texture( c3d_mapDiffuse, vtx_texture + l_dir * l_writer.Paren( 0.0_f / 3.0_f - 0.5_f ), 0.0_f ).rgb()
									+ texture( c3d_mapDiffuse, vtx_texture + l_dir * l_writer.Paren( 3.0_f / 3.0_f - 0.5_f ), 0.0_f ).rgb() )
								* l_writer.Paren( 1.0_f / 4.0_f ) );
					auto l_lumaB = l_writer.GetLocale( cuT( "l_lumaB" )
						, dot( l_rgbB, l_luma ) );

					plx_v4FragColor = vec4( l_writer.Ternary( Type{ cuT( "l_lumaB < l_lumaMin || l_lumaB > l_lumaMax" ) }, l_rgbA, l_rgbB ), 1.0_f );
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
		m_matrixUbo.Cleanup();
		m_fxaaUbo.Cleanup();
		m_mapDiffuse.reset();
		m_surface.Cleanup();
	}

	bool FxaaPostEffect::Apply( FrameBuffer & p_framebuffer )
	{
		auto l_attach = p_framebuffer.GetAttachment( AttachmentPoint::eColour, 0 );

		if ( l_attach && l_attach->GetAttachmentType() == AttachmentType::eTexture )
		{
			m_pipeline->Apply();
			m_surface.m_fbo->Bind( FrameBufferTarget::eDraw );
			auto l_texture = std::static_pointer_cast< TextureAttachment >( l_attach )->GetTexture();
			m_surface.m_fbo->Clear( BufferComponent::eColour );
			m_mapDiffuse->SetValue( 0 );
			GetRenderSystem()->GetCurrentContext()->RenderTexture( m_surface.m_size
				, *l_texture
				, *m_pipeline
				, m_matrixUbo );
			m_surface.m_fbo->Unbind();

			p_framebuffer.Bind( FrameBufferTarget::eDraw );
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
