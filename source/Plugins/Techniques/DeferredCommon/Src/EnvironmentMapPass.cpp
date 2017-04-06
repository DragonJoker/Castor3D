#include "EnvironmentMapPass.hpp"

#include <Engine.hpp>

#include <FrameBuffer/FrameBuffer.hpp>
#include <FrameBuffer/TextureAttachment.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Scene/Scene.hpp>
#include <Shader/ShaderProgram.hpp>
#include <Shader/UniformBuffer.hpp>
#include <Texture/Sampler.hpp>
#include <Texture/TextureImage.hpp>
#include <Texture/TextureLayout.hpp>

#include <GlslSource.hpp>
#include <GlslUtils.hpp>

#include <Graphics/Image.hpp>

using namespace Castor;
using namespace Castor3D;

namespace deferred_common
{
	namespace
	{
		TextureUnit DoCreateResult( Engine & p_engine
			, Size const & p_size )
		{
			SamplerSPtr l_sampler;
			String const l_name = cuT( "EnvironmentMapPassResult" );

			if ( p_engine.GetSamplerCache().Has( l_name ) )
			{
				l_sampler = p_engine.GetSamplerCache().Find( l_name );
			}
			else
			{
				l_sampler = p_engine.GetSamplerCache().Add( l_name );
				l_sampler->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
				l_sampler->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
				l_sampler->SetWrappingMode( TextureUVW::eU, WrapMode::eClampToBorder );
				l_sampler->SetWrappingMode( TextureUVW::eV, WrapMode::eClampToBorder );
				l_sampler->SetWrappingMode( TextureUVW::eW, WrapMode::eClampToBorder );
			}

			auto l_texture = p_engine.GetRenderSystem()->CreateTexture(
				TextureType::eTwoDimensions,
				AccessType::eNone,
				AccessType::eRead | AccessType::eWrite,
				PixelFormat::eRGBA32F, p_size );
			TextureUnit l_unit{ p_engine };
			l_unit.SetTexture( l_texture );
			l_unit.SetSampler( l_sampler );
			l_unit.SetIndex( 2u );

			for ( auto & l_image : *l_texture )
			{
				l_image->InitialiseSource();
			}

			l_unit.Initialise();
			return l_unit;
		}

		String DoCreateVertexProgram( RenderSystem & p_renderSystem )
		{
			using namespace GLSL;
			auto l_writer = p_renderSystem.CreateGlslWriter();

			// Shader inputs
			UBO_MATRIX( l_writer );
			auto position = l_writer.GetAttribute< Vec2 >( ShaderProgram::Position );
			auto texture = l_writer.GetAttribute< Vec2 >( ShaderProgram::Texture );

			// Shader outputs
			auto vtx_texture = l_writer.GetOutput< Vec2 >( cuT( "vtx_texture" ) );
			auto gl_Position = l_writer.GetBuiltin< Vec4 >( cuT( "gl_Position" ) );

			l_writer.ImplementFunction< void >( cuT( "main" )
				, [&]()
				{
					vtx_texture = texture;
					gl_Position = c3d_mtxProjection * vec4( position.x(), position.y(), 0.0, 1.0 );
				} );
			return l_writer.Finalise();
		}

		String DoCreatePixelProgram( RenderSystem & p_renderSystem )
		{
			using namespace GLSL;
			auto l_writer = p_renderSystem.CreateGlslWriter();

			// Shader inputs
			UBO_SCENE( l_writer );
			auto c3d_mapDepth = l_writer.GetUniform< Sampler2D >( GetTextureName( DsTexture::eDepth ) );
			auto c3d_mapNormals = l_writer.GetUniform< Sampler2D >( GetTextureName( DsTexture::eNormal ) );
			auto c3d_mapEnvironment = l_writer.GetUniform< SamplerCube >( ShaderProgram::MapEnvironment, 32u );
			auto vtx_texture = l_writer.GetInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto pxl_v4FragColor = l_writer.GetFragData< Vec4 >( cuT( "pxl_v4FragColor" ), 0 );

			GLSL::Utils l_utils{ l_writer };
			l_utils.DeclareCalcTexCoord();
			l_utils.DeclareCalcWSPosition();

			l_writer.ImplementFunction< void >( cuT( "main" )
				, [&]()
				{
					auto l_texCoord = l_writer.GetLocale( cuT( "l_texCoord" ), l_utils.CalcTexCoord() );
					auto l_v4Normal = l_writer.GetLocale( cuT( "l_v4Normal" ), texture( c3d_mapNormals, l_texCoord ) );
					auto l_index = l_writer.GetLocale( cuT( "l_index" ), l_writer.Cast< Int >( l_v4Normal.z() ) );

					IF( l_writer, l_index <= 0 )
					{
						l_writer.Discard();
					}
					FI;

					auto l_wsPosition = l_writer.GetLocale( cuT( "l_wsPosition" ), l_utils.CalcWSPosition( l_texCoord ) );
					auto l_wsNormal = l_writer.GetLocale( cuT( "l_wsNormal" ), l_v4Normal.xyz() );

					auto l_i = l_writer.GetLocale( cuT( "l_i" ), l_wsPosition - c3d_v3CameraPosition );
					auto l_r = l_writer.GetLocale( cuT( "l_r" ), reflect( l_i, l_wsNormal ) );
					pxl_v4FragColor = vec4( texture( c3d_mapEnvironment[l_index], l_r ).xyz(), 1.0_f );
				} );
			return l_writer.Finalise();
		}

		ShaderProgramSPtr DoCreateProgram( Engine & p_engine )
		{
			auto & l_renderSystem = *p_engine.GetRenderSystem();
			String l_vtx = DoCreateVertexProgram( l_renderSystem );
			String l_pxl = DoCreatePixelProgram( l_renderSystem );
			auto l_result = p_engine.GetShaderProgramCache().GetNewProgram( false );
			ShaderModel l_model = l_renderSystem.GetGpuInformations().GetMaxShaderModel();
			l_result->CreateObject( ShaderType::eVertex );
			l_result->CreateObject( ShaderType::ePixel );
			l_result->SetSource( ShaderType::eVertex, l_model, l_vtx );
			l_result->SetSource( ShaderType::ePixel, l_model, l_pxl );
			l_result->CreateUniform< UniformType::eSampler >( GetTextureName( DsTexture::eDepth ), ShaderType::ePixel )->SetValue( 0u );
			l_result->CreateUniform< UniformType::eSampler >( GetTextureName( DsTexture::eNormal ), ShaderType::ePixel )->SetValue( 1u );
			l_result->CreateUniform< UniformType::eSampler >( GetTextureName( DsTexture::eNormal ), ShaderType::ePixel, 32u )->SetValues(
			{
				 2u,  3u,  4u,  5u,  6u,  7u,  8u,  9u,
				10u, 11u, 12u, 13u, 14u, 15u, 16u, 17u,
				18u, 19u, 20u, 21u, 22u, 23u, 24u, 25u,
				26u, 27u, 28u, 29u, 30u, 31u, 32u, 33u,
			} );
			l_result->Initialise();
			return l_result;
		}

		RenderPipelineUPtr DoCreateRenderPipeline( Engine & p_engine
			, ShaderProgram & p_program
			, UniformBuffer & p_matrixUbo )
		{
			UniformBuffer::FillMatrixBuffer( p_matrixUbo );
			RasteriserState l_rsState;
			l_rsState.SetCulledFaces( Culling::eBack );
			DepthStencilState l_dsState;
			l_dsState.SetDepthTest( false );
			l_dsState.SetDepthMask( WritingMask::eZero );
			auto l_result = p_engine.GetRenderSystem()->CreateRenderPipeline( std::move( l_dsState )
				, std::move( l_rsState )
				, BlendState{}
				, MultisampleState{}
				, p_program
				, PipelineFlags{} );
			l_result->AddUniformBuffer( p_matrixUbo );
			return l_result;
		}
	}

	EnvironmentMapPass::EnvironmentMapPass( Engine & p_engine
		, Size const & p_size )
		: OwnedBy< Engine >{ p_engine }
		, m_frameBuffer{ p_engine.GetRenderSystem()->CreateFrameBuffer() }
		, m_result{ DoCreateResult( p_engine, p_size ) }
		, m_colourAttach{ m_frameBuffer->CreateAttachment( m_result.GetTexture() ) }
		, m_program{ DoCreateProgram( p_engine ) }
		, m_matrixUbo{ ShaderProgram::BufferMatrix, *p_engine.GetRenderSystem() }
		, m_pipeline{ DoCreateRenderPipeline( p_engine, *m_program, m_matrixUbo ) }
		, m_projectionUniform{ m_matrixUbo.GetUniform< UniformType::eMat4x4f >( RenderPipeline::MtxProjection ) }
		, m_viewUniform{ m_matrixUbo.GetUniform< UniformType::eMat4x4f >( RenderPipeline::MtxView ) }
	{
		m_frameBuffer->Create();
		m_frameBuffer->Initialise( p_size );
		m_frameBuffer->Bind();
		m_frameBuffer->Attach( AttachmentPoint::eColour, 0u, m_colourAttach, TextureType::eTwoDimensions );
		ENSURE( m_frameBuffer->IsComplete() );
		m_frameBuffer->Unbind();
	}

	EnvironmentMapPass::~EnvironmentMapPass()
	{
		m_viewUniform.reset();
		m_projectionUniform.reset();
		m_matrixUbo.Cleanup();
		m_pipeline->Cleanup();
		m_pipeline.reset();
		m_program.reset();
		m_frameBuffer->Bind();
		m_frameBuffer->DetachAll();
		m_frameBuffer->Unbind();
		m_frameBuffer->Cleanup();
		m_frameBuffer->Destroy();
		m_frameBuffer.reset();
		m_colourAttach.reset();
		m_result.Cleanup();
	}

	void EnvironmentMapPass::Render( GeometryPassResult & p_gp
		, Scene & p_scene )
	{
		auto & l_maps = p_scene.GetEnvironmentMaps();
		auto & l_result = *m_result.GetTexture();
		m_frameBuffer->Bind();
		p_gp[size_t( DsTexture::eDepth )]->Bind();
		p_gp[size_t( DsTexture::eNormal )]->Bind();
		auto l_index = 2u;

		for ( auto & l_map : l_maps )
		{
			l_map.get().GetTexture().GetTexture()->Bind( l_index );
			l_map.get().GetTexture().GetSampler()->Bind( l_index );
			++l_index;
		}

		GetEngine()->GetRenderSystem()->GetMainContext()->RenderTexture( l_result.GetDimensions()
			, l_result );
		l_index = 2u;

		for ( auto & l_map : l_maps )
		{
			l_map.get().GetTexture().GetTexture()->Unbind( l_index );
			l_map.get().GetTexture().GetSampler()->Unbind( l_index );
			++l_index;
		}

		p_gp[size_t( DsTexture::eNormal )]->Unbind();
		p_gp[size_t( DsTexture::eDepth )]->Unbind();
		m_frameBuffer->Unbind();
	}
}
