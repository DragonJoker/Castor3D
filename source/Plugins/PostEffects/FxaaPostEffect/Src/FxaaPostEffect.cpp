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
			GlslWriter l_writer = p_renderSystem->CreateGlslWriter();

			// Shader inputs
			UBO_MATRIX( l_writer );
			UBO_FXAA( l_writer );
			auto position = l_writer.DeclAttribute< Vec2 >( Castor3D::ShaderProgram::Position );

			// Shader outputs
			auto vtx_texture = l_writer.DeclOutput< Vec2 >( cuT( "vtx_texture" ) );
			auto vtx_posPos = l_writer.DeclOutput< Vec4 >( PosPos );
			auto gl_Position = l_writer.DeclBuiltin< Vec4 >( cuT( "gl_Position" ) );

			l_writer.ImplementFunction< void >( cuT( "main" )
				, [&]()
				{
					auto l_invTargetSize = l_writer.DeclLocale( cuT( "l_invTargetSize" )
						, vec2( 1.0 / c3d_renderSize.x(), 1.0 / c3d_renderSize.y() ) );
					vtx_texture = position;
					gl_Position = c3d_mtxProjection * vec4( position.xy(), 0.0, 1.0 );
					vtx_posPos.xy() = position.xy();
					vtx_posPos.zw() = position.xy() - l_writer.Paren( l_invTargetSize * l_writer.Paren( 0.5 + c3d_subpixShift ) );
				} );
			return l_writer.Finalise();
		}

		GLSL::Shader GetFragmentProgram( Castor3D::RenderSystem * p_renderSystem )
		{
			using namespace GLSL;
			GlslWriter l_writer = p_renderSystem->CreateGlslWriter();

			// Shader inputs
			auto c3d_mapDiffuse = l_writer.DeclUniform< Sampler2D >( Castor3D::ShaderProgram::MapDiffuse );
			auto vtx_texture = l_writer.DeclInput< Vec2 >( cuT( "vtx_texture" ) );
			auto vtx_posPos = l_writer.DeclInput< Vec4 >( PosPos );

			UBO_FXAA( l_writer );

			// Shader outputs
			auto plx_v4FragColor = l_writer.DeclFragData< Vec4 >( cuT( "plx_v4FragColor" ), 0 );

#define FXAA_REDUCE_MIN	( 1.0 / 128.0 )

			l_writer.ImplementFunction< void >( cuT( "main" )
				, [&]()
				{
					auto l_invTargetSize = l_writer.DeclLocale( cuT( "l_invTargetSize" )
						, vec2( 1.0 / c3d_renderSize.x(), 1.0 / c3d_renderSize.y() ) );

					auto l_rgbNW = l_writer.DeclLocale( cuT( "l_rgbNW" )
						, textureLodOffset( c3d_mapDiffuse, vtx_texture, 0.0_f, ivec2( -1_i, -1_i ) ).rgb() );
					auto l_rgbNE = l_writer.DeclLocale( cuT( "l_rgbNE" )
						, textureLodOffset( c3d_mapDiffuse, vtx_texture, 0.0_f, ivec2( 1_i, -1_i ) ).rgb() );
					auto l_rgbSW = l_writer.DeclLocale( cuT( "l_rgbSW" )
						, textureLodOffset( c3d_mapDiffuse, vtx_texture, 0.0_f, ivec2( -1_i, 1_i ) ).rgb() );
					auto l_rgbSE = l_writer.DeclLocale( cuT( "l_rgbSE" )
						, textureLodOffset( c3d_mapDiffuse, vtx_texture, 0.0_f, ivec2( 1_i, 1_i ) ).rgb() );
					auto l_rgbM = l_writer.DeclLocale( cuT( "l_rgbM" )
						, texture( c3d_mapDiffuse, vtx_texture, 0.0_f ).rgb() );

					auto l_luma = l_writer.DeclLocale( cuT( "l_luma" )
						, vec3( 0.299_f, 0.587_f, 0.114_f ) );
					auto l_lumaNW = l_writer.DeclLocale( cuT( "l_lumaNW" )
						, dot( l_rgbNW, l_luma ) );
					auto l_lumaNE = l_writer.DeclLocale( cuT( "l_lumaNE" )
						, dot( l_rgbNE, l_luma ) );
					auto l_lumaSW = l_writer.DeclLocale( cuT( "l_lumaSW" )
						, dot( l_rgbSW, l_luma ) );
					auto l_lumaSE = l_writer.DeclLocale( cuT( "l_lumaSE" )
						, dot( l_rgbSE, l_luma ) );
					auto l_lumaM = l_writer.DeclLocale( cuT( "l_lumaM" )
						, dot( l_rgbM, l_luma ) );

					auto l_lumaMin = l_writer.DeclLocale( cuT( "l_lumaMin" )
						, min( l_lumaM, min( min( l_lumaNW, l_lumaNE ), min( l_lumaSW, l_lumaSE ) ) ) );
					auto l_lumaMax = l_writer.DeclLocale( cuT( "l_lumaMax" )
						, max( l_lumaM, max( max( l_lumaNW, l_lumaNE ), max( l_lumaSW, l_lumaSE ) ) ) );

					auto l_dir = l_writer.DeclLocale( cuT( "l_dir" )
						, vec2( -l_writer.Paren( l_writer.Paren( l_lumaNW + l_lumaNE ) - l_writer.Paren( l_lumaSW + l_lumaSE ) )
							, ( l_writer.Paren( l_lumaNW + l_lumaSW ) - l_writer.Paren( l_lumaNE + l_lumaSE ) ) ) );

					auto l_dirReduce = l_writer.DeclLocale( cuT( "l_dirReduce" )
						, max( l_writer.Paren( l_lumaNW + l_lumaNE + l_lumaSW + l_lumaSE ) * l_writer.Paren( 0.25_f * c3d_reduceMul ), FXAA_REDUCE_MIN ) );
					auto l_rcpDirMin = l_writer.DeclLocale( cuT( "l_rcpDirMin" )
						, 1.0 / ( min( GLSL::abs( l_dir.x() ), GLSL::abs( l_dir.y() ) ) + l_dirReduce ) );
					l_dir = min( vec2( c3d_spanMax, c3d_spanMax )
						, max( vec2( -c3d_spanMax, -c3d_spanMax )
							, l_dir * l_rcpDirMin ) ) * l_invTargetSize;

					auto l_rgbA = l_writer.DeclLocale( cuT( "l_rgbA" )
						, l_writer.Paren( texture( c3d_mapDiffuse, vtx_texture + l_dir * l_writer.Paren( 1.0_f / 3.0_f - 0.5_f ), 0.0_f ).rgb()
								+ texture( c3d_mapDiffuse, vtx_texture + l_dir * l_writer.Paren( 2.0_f / 3.0_f - 0.5_f ), 0.0_f ).rgb() )
							* l_writer.Paren( 1.0_f / 2.0_f ) );
					auto l_rgbB = l_writer.DeclLocale( cuT( "l_rgbB" )
						, l_writer.Paren( l_rgbA * 1.0_f / 2.0_f )
							+ l_writer.Paren( texture( c3d_mapDiffuse, vtx_texture + l_dir * l_writer.Paren( 0.0_f / 3.0_f - 0.5_f ), 0.0_f ).rgb()
									+ texture( c3d_mapDiffuse, vtx_texture + l_dir * l_writer.Paren( 3.0_f / 3.0_f - 0.5_f ), 0.0_f ).rgb() )
								* l_writer.Paren( 1.0_f / 4.0_f ) );
					auto l_lumaB = l_writer.DeclLocale( cuT( "l_lumaB" )
						, dot( l_rgbB, l_luma ) );

					plx_v4FragColor = vec4( l_writer.Ternary( Type{ cuT( "l_lumaB < l_lumaMin || l_lumaB > l_lumaMax" ) }, l_rgbA, l_rgbB ), 1.0_f );
				} );

			return l_writer.Finalise();
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
		String l_param;

		if ( p_parameters.Get( cuT( "SubpixShift" ), l_param ) )
		{
			m_subpixShift = string::to_float( l_param );
		}

		if ( p_parameters.Get( cuT( "MaxSpan" ), l_param ) )
		{
			m_spanMax = string::to_float( l_param );
		}

		if ( p_parameters.Get( cuT( "ReduceMul" ), l_param ) )
		{
			m_reduceMul = string::to_float( l_param );
		}

		String l_name = cuT( "FXAA" );

		if ( !m_renderTarget.GetEngine()->GetSamplerCache().Has( l_name ) )
		{
			m_sampler = m_renderTarget.GetEngine()->GetSamplerCache().Add( l_name );
			m_sampler->SetInterpolationMode( Castor3D::InterpolationFilter::eMin, Castor3D::InterpolationMode::eNearest );
			m_sampler->SetInterpolationMode( Castor3D::InterpolationFilter::eMag, Castor3D::InterpolationMode::eNearest );
			m_sampler->SetWrappingMode( Castor3D::TextureUVW::eU, Castor3D::WrapMode::eClampToBorder );
			m_sampler->SetWrappingMode( Castor3D::TextureUVW::eV, Castor3D::WrapMode::eClampToBorder );
			m_sampler->SetWrappingMode( Castor3D::TextureUVW::eW, Castor3D::WrapMode::eClampToBorder );
		}
		else
		{
			m_sampler = m_renderTarget.GetEngine()->GetSamplerCache().Find( l_name );
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
		bool l_result = false;
		auto & l_cache = GetRenderSystem()->GetEngine()->GetShaderProgramCache();
		Size l_size = m_renderTarget.GetSize();

		auto l_vertex = GetVertexProgram( GetRenderSystem() );
		auto l_fragment = GetFragmentProgram( GetRenderSystem() );

		auto l_program = l_cache.GetNewProgram( false );
		l_program->CreateObject( Castor3D::ShaderType::eVertex );
		l_program->CreateObject( Castor3D::ShaderType::ePixel );
		m_mapDiffuse = l_program->CreateUniform< Castor3D::UniformType::eSampler >( Castor3D::ShaderProgram::MapDiffuse
			, Castor3D::ShaderType::ePixel );
		l_program->SetSource( Castor3D::ShaderType::eVertex, l_vertex );
		l_program->SetSource( Castor3D::ShaderType::ePixel, l_fragment );
		l_program->Initialise();

		Castor3D::DepthStencilState l_dsstate;
		l_dsstate.SetDepthTest( false );
		l_dsstate.SetDepthMask( Castor3D::WritingMask::eZero );
		Castor3D::RasteriserState l_rsstate;
		l_rsstate.SetCulledFaces( Castor3D::Culling::eBack );
		m_pipeline = GetRenderSystem()->CreateRenderPipeline( std::move( l_dsstate )
			, std::move( l_rsstate )
			, Castor3D::BlendState{}
			, Castor3D::MultisampleState{}
			, *l_program
			, Castor3D::PipelineFlags{} );
		m_pipeline->AddUniformBuffer( m_matrixUbo.GetUbo() );
		m_pipeline->AddUniformBuffer( m_fxaaUbo.GetUbo() );

		return m_surface.Initialise( m_renderTarget, l_size, 0, m_sampler );
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
		auto l_attach = p_framebuffer.GetAttachment( Castor3D::AttachmentPoint::eColour, 0 );

		if ( l_attach && l_attach->GetAttachmentType() == Castor3D::AttachmentType::eTexture )
		{
			m_fxaaUbo.Update( l_attach->GetBuffer()->dimensions()
				, m_subpixShift
				, m_spanMax
				, m_reduceMul );
			m_pipeline->Apply();
			m_surface.m_fbo->Bind( Castor3D::FrameBufferTarget::eDraw );
			auto l_texture = std::static_pointer_cast< Castor3D::TextureAttachment >( l_attach )->GetTexture();
			m_surface.m_fbo->Clear( Castor3D::BufferComponent::eColour );
			m_mapDiffuse->SetValue( 0 );
			GetRenderSystem()->GetCurrentContext()->RenderTexture( m_surface.m_size
				, *l_texture
				, *m_pipeline
				, m_matrixUbo );
			m_surface.m_fbo->Unbind();

			p_framebuffer.Bind( Castor3D::FrameBufferTarget::eDraw );
			GetRenderSystem()->GetCurrentContext()->RenderTexture( l_texture->GetDimensions(), *m_surface.m_colourTexture.GetTexture() );
			p_framebuffer.Unbind();
		}

		return true;
	}

	bool PostEffect::DoWriteInto( TextFile & p_file )
	{
		return true;
	}
}
