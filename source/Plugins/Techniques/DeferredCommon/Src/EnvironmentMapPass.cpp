#include "EnvironmentMapPass.hpp"

#include <Engine.hpp>

#include <FrameBuffer/FrameBuffer.hpp>
#include <FrameBuffer/TextureAttachment.hpp>
#include <Mesh/Buffer/GeometryBuffers.hpp>
#include <Mesh/Buffer/VertexBuffer.hpp>
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
		VertexBufferSPtr DoCreateVbo( Engine & p_engine )
		{
			auto l_declaration = BufferDeclaration(
			{
				BufferElementDeclaration( ShaderProgram::Position, uint32_t( ElementUsage::ePosition ), ElementType::eVec2 ),
				BufferElementDeclaration{ ShaderProgram::Texture, uint32_t( ElementUsage::eTexCoords ), ElementType::eVec2 },
			} );

			float l_data[] =
			{
				0, 0, 0, 0,
				1, 1, 1, 1,
				0, 1, 0, 1,
				0, 0, 0, 0,
				1, 0, 1, 0,
				1, 1, 1, 1
			};

			auto & l_renderSystem = *p_engine.GetRenderSystem();
			auto l_vertexBuffer = std::make_shared< VertexBuffer >( p_engine, l_declaration );
			uint32_t l_stride = l_declaration.stride();
			l_vertexBuffer->Resize( uint32_t( sizeof( l_data ) ) );
			uint8_t * l_buffer = l_vertexBuffer->data();
			std::memcpy( l_buffer, l_data, sizeof( l_data ) );
			l_vertexBuffer->Initialise( BufferAccessType::eStatic
				, BufferAccessNature::eDraw );
			return l_vertexBuffer;
		}

		GeometryBuffersSPtr DoCreateVao( Engine & p_engine
			, ShaderProgram & p_program
			, VertexBuffer & p_vbo )
		{
			auto & l_renderSystem = *p_engine.GetRenderSystem();
			auto l_result = l_renderSystem.CreateGeometryBuffers( Topology::eTriangles
				, p_program );
			l_result->Initialise( { p_vbo }, nullptr );
			return l_result;
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
			UBO_GPINFO( l_writer );
			auto c3d_mapDepth = l_writer.GetUniform< Sampler2D >( GetTextureName( DsTexture::eDepth ) );
			auto c3d_mapNormal = l_writer.GetUniform< Sampler2D >( GetTextureName( DsTexture::eNormal ) );
			auto c3d_mapEnvironment = l_writer.GetUniform< SamplerCube >( ShaderProgram::MapEnvironment, 32u );
			auto vtx_texture = l_writer.GetInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto pxl_v4FragColor = l_writer.GetFragData< Vec4 >( cuT( "pxl_v4FragColor" ), 0 );

			GLSL::Utils l_utils{ l_writer };
			l_utils.DeclareCalcWSPosition();

			l_writer.ImplementFunction< void >( cuT( "main" )
				, [&]()
			{
				auto l_v4Normal = l_writer.GetLocale( cuT( "l_v4Normal" ), texture( c3d_mapNormal, vtx_texture ) );
				auto l_index = l_writer.GetLocale( cuT( "l_index" ), l_v4Normal.w() );

				IF( l_writer, l_index < 1.0_f )
				{
					l_writer.Discard();
				}
				FI;

				auto l_wsPosition = l_writer.GetLocale( cuT( "l_wsPosition" ), l_utils.CalcWSPosition( vtx_texture, c3d_mtxInvViewProj ) );
				auto l_wsNormal = l_writer.GetLocale( cuT( "l_wsNormal" ), l_v4Normal.xyz() );

				auto l_i = l_writer.GetLocale( cuT( "l_i" ), l_wsPosition - c3d_v3CameraPosition );
				auto l_r = l_writer.GetLocale( cuT( "l_r" ), reflect( l_i, l_wsNormal ) );
				pxl_v4FragColor = vec4( texture( c3d_mapEnvironment[l_writer.Cast< Int >( l_index ) - 1], l_r ).xyz(), 1.0_f );
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
			l_result->CreateUniform< UniformType::eSampler >( ShaderProgram::MapEnvironment, ShaderType::ePixel, 32u )->SetValues(
			{
				2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u,
				10u, 11u, 12u, 13u, 14u, 15u, 16u, 17u,
				18u, 19u, 20u, 21u, 22u, 23u, 24u, 25u,
				26u, 27u, 28u, 29u, 30u, 31u, 32u, 33u,
			} );
			l_result->Initialise();
			return l_result;
		}

		RenderPipelineUPtr DoCreateRenderPipeline( Engine & p_engine
			, ShaderProgram & p_program
			, UniformBuffer & p_matrixUbo
			, UniformBuffer & p_sceneUbo
			, UniformBuffer & p_gpInfoUbo )
		{
			UniformBuffer::FillMatrixBuffer( p_matrixUbo );
			UniformBuffer::FillSceneBuffer( p_sceneUbo );
			RasteriserState l_rsState;
			l_rsState.SetCulledFaces( Culling::eNone );
			DepthStencilState l_dsState;
			l_dsState.SetDepthTest( false );
			l_dsState.SetDepthMask( WritingMask::eZero );
			BlendState l_blState;
			l_blState.EnableBlend( true );
			l_blState.SetRgbBlend( BlendOperation::eAdd, BlendOperand::eDstColour, BlendOperand::eSrcColour );
			auto l_result = p_engine.GetRenderSystem()->CreateRenderPipeline( std::move( l_dsState )
				, std::move( l_rsState )
				, std::move( l_blState )
				, MultisampleState{}
				, p_program
				, PipelineFlags{} );
			l_result->AddUniformBuffer( p_matrixUbo );
			l_result->AddUniformBuffer( p_sceneUbo );
			l_result->AddUniformBuffer( p_gpInfoUbo );
			return l_result;
		}
	}

	EnvironmentMapPass::EnvironmentMapPass( Engine & p_engine
		, Size const & p_size )
		: OwnedBy< Engine >{ p_engine }
		, m_size{ p_size }
		, m_viewport{ p_engine }
		, m_vertexBuffer{ DoCreateVbo( p_engine ) }
		, m_program{ DoCreateProgram( p_engine ) }
		, m_geometryBuffers{ DoCreateVao( p_engine, *m_program, *m_vertexBuffer ) }
		, m_matrixUbo{ ShaderProgram::BufferMatrix, *p_engine.GetRenderSystem() }
		, m_sceneUbo{ ShaderProgram::BufferScene, *p_engine.GetRenderSystem() }
		, m_gpInfo{ p_engine }
		, m_pipeline{ DoCreateRenderPipeline( p_engine, *m_program, m_matrixUbo, m_sceneUbo, m_gpInfo.GetUbo() ) }
		, m_projectionUniform{ m_matrixUbo.GetUniform< UniformType::eMat4x4f >( RenderPipeline::MtxProjection ) }
		, m_viewUniform{ m_matrixUbo.GetUniform< UniformType::eMat4x4f >( RenderPipeline::MtxView ) }
		, m_cameraPosUniform{ m_sceneUbo.GetUniform< UniformType::eVec3f >( ShaderProgram::CameraPos ) }
	{
		m_viewport.SetOrtho( 0, 1, 0, 1, 0, 1 );
		m_viewport.Initialise();
		m_viewport.Resize( m_size );
		m_viewport.Update();
		m_projectionUniform->SetValue( m_viewport.GetProjection() );
		m_matrixUbo.Update();
	}

	EnvironmentMapPass::~EnvironmentMapPass()
	{
		m_viewUniform.reset();
		m_projectionUniform.reset();
		m_sceneUbo.Cleanup();
		m_matrixUbo.Cleanup();
		m_pipeline->Cleanup();
		m_pipeline.reset();
		m_geometryBuffers->Cleanup();
		m_geometryBuffers.reset();
		m_vertexBuffer->Cleanup();
		m_geometryBuffers.reset();
		m_program.reset();
	}

	void EnvironmentMapPass::Render( GeometryPassResult & p_gp
		, Scene const & p_scene
		, Camera const & p_camera
		, Matrix4x4r const & p_invViewProj
		, Matrix4x4r const & p_invView
		, Matrix4x4r const & p_invProj )
	{
		m_gpInfo.Update( m_size
			, p_camera
			, p_invViewProj
			, p_invView
			, p_invProj );
		m_cameraPosUniform->SetValue( p_camera.GetParent()->GetDerivedPosition() );
		m_sceneUbo.Update();
		auto & l_maps = p_scene.GetEnvironmentMaps();
		p_gp[size_t( DsTexture::eDepth )]->GetTexture()->Bind( 0u );
		p_gp[size_t( DsTexture::eDepth )]->GetSampler()->Bind( 0u );
		p_gp[size_t( DsTexture::eNormal )]->GetTexture()->Bind( 1u );
		p_gp[size_t( DsTexture::eNormal )]->GetSampler()->Bind( 1u );
		auto l_index = 2u;

		for ( auto & l_map : l_maps )
		{
			l_map.get().GetTexture().GetTexture()->Bind( l_index );
			l_map.get().GetTexture().GetSampler()->Bind( l_index );
			++l_index;
		}

		m_pipeline->Apply();
		m_geometryBuffers->Draw( 6u, 0 );
		l_index = 2u;

		for ( auto & l_map : l_maps )
		{
			l_map.get().GetTexture().GetTexture()->Unbind( l_index );
			l_map.get().GetTexture().GetSampler()->Unbind( l_index );
			++l_index;
		}

		p_gp[size_t( DsTexture::eNormal )]->GetTexture()->Unbind( 1u );
		p_gp[size_t( DsTexture::eNormal )]->GetSampler()->Unbind( 1u );
		p_gp[size_t( DsTexture::eDepth )]->GetTexture()->Unbind( 0u );
		p_gp[size_t( DsTexture::eDepth )]->GetSampler()->Unbind( 0u );
	}
}
