#include "RefractionPass.hpp"

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
			auto c3d_mapDiffuse = l_writer.GetUniform< Sampler2D >( GetTextureName( DsTexture::eDiffuse ) );
			auto c3d_mapEmissive = l_writer.GetUniform< Sampler2D >( GetTextureName( DsTexture::eEmissive ) );
			auto c3d_mapEnvironment = l_writer.GetUniform< SamplerCube >( ShaderProgram::MapEnvironment, 32u );
			auto vtx_texture = l_writer.GetInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto pxl_v4FragColor = l_writer.GetFragData< Vec4 >( cuT( "pxl_v4FragColor" ), 0 );

			GLSL::Utils l_utils{ l_writer };
			l_utils.DeclareCalcWSPosition();
			Declare_DecodeMaterial( l_writer );

			l_writer.ImplementFunction< void >( cuT( "main" )
				, [&]()
			{
				auto l_v4Normal = l_writer.GetLocale( cuT( "l_v4Normal" ), texture( c3d_mapNormal, vtx_texture ) );
				auto l_flags = l_writer.GetLocale( cuT( "l_flags" ), l_v4Normal.w() );
				auto l_envMapIndex = l_writer.GetLocale( cuT( "l_envMapIndex" ), 0_i );
				auto l_receiver = l_writer.GetLocale( cuT( "l_receiver" ), 0_i );
				auto l_reflection = l_writer.GetLocale( cuT( "l_reflection" ), 0_i );
				auto l_refraction = l_writer.GetLocale( cuT( "l_refraction" ), 0_i );
				DecodeMaterial( l_writer
					, l_flags
					, l_receiver
					, l_reflection
					, l_refraction
					, l_envMapIndex );

				IF( l_writer, l_envMapIndex < 1_i )
				{
					l_writer.Discard();
				}
				FI;

				pxl_v4FragColor = vec4( 0.0_f, 0.0_f, 0.0_f, 1.0_f );

				IF( l_writer, l_refraction != 0_i )
				{
					auto l_position = l_writer.GetLocale( cuT( "l_position" ), l_utils.CalcWSPosition( vtx_texture, c3d_mtxInvViewProj ) );
					auto l_normal = l_writer.GetLocale( cuT( "l_normal" ), normalize( l_v4Normal.xyz() ) );
					auto l_incident = l_writer.GetLocale( cuT( "l_incident" ), normalize( l_position - c3d_v3CameraPosition ) );
					l_envMapIndex = l_envMapIndex - 1_i;
					auto l_ratio = l_writer.GetLocale( cuT( "l_ratio" ), texture( c3d_mapEmissive, vtx_texture ).w() );
					auto l_refract = l_writer.GetLocale( cuT( "l_refract" ), refract( l_incident, l_normal, l_ratio ) );
					pxl_v4FragColor += vec4( texture( c3d_mapEnvironment[l_envMapIndex], l_refract ).xyz(), 1.0_f );
				}
				FI;
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
			l_result->CreateUniform< UniformType::eSampler >( GetTextureName( DsTexture::eDiffuse ), ShaderType::ePixel )->SetValue( 2u );
			l_result->CreateUniform< UniformType::eSampler >( GetTextureName( DsTexture::eEmissive ), ShaderType::ePixel )->SetValue( 3u );
			l_result->CreateUniform< UniformType::eSampler >( ShaderProgram::MapEnvironment, ShaderType::ePixel, 32u )->SetValues(
			{
				4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u,
				12u, 13u, 14u, 15u, 16u, 17u, 18u, 19u,
				20u, 21u, 22u, 23u, 24u, 25u, 26u, 27u,
				28u, 29u, 30u, 31u, 32u, 33u, 34u, 35u,
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
			l_blState.SetRgbBlend( BlendOperation::eAdd, BlendOperand::eSrcColour, BlendOperand::eDstColour );
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

	RefractionPass::RefractionPass( Engine & p_engine
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

	RefractionPass::~RefractionPass()
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

	void RefractionPass::Render( GeometryPassResult & p_gp
		, Scene const & p_scene
		, Camera const & p_camera
		, Matrix4x4r const & p_invViewProj
		, Matrix4x4r const & p_invView
		, Matrix4x4r const & p_invProj )
	{
		static uint32_t constexpr l_environmentStart = 4u;
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
		p_gp[size_t( DsTexture::eDiffuse )]->GetTexture()->Bind( 2u );
		p_gp[size_t( DsTexture::eDiffuse )]->GetSampler()->Bind( 2u );
		p_gp[size_t( DsTexture::eEmissive )]->GetTexture()->Bind( 3u );
		p_gp[size_t( DsTexture::eEmissive )]->GetSampler()->Bind( 3u );
		auto l_index = l_environmentStart;

		for ( auto & l_map : l_maps )
		{
			l_map.get().GetTexture().GetTexture()->Bind( l_index );
			l_map.get().GetTexture().GetSampler()->Bind( l_index );
			++l_index;
		}

		m_pipeline->Apply();
		m_geometryBuffers->Draw( 6u, 0 );
		l_index = l_environmentStart;

		for ( auto & l_map : l_maps )
		{
			l_map.get().GetTexture().GetTexture()->Unbind( l_index );
			l_map.get().GetTexture().GetSampler()->Unbind( l_index );
			++l_index;
		}

		p_gp[size_t( DsTexture::eEmissive )]->GetTexture()->Unbind( 3u );
		p_gp[size_t( DsTexture::eEmissive )]->GetSampler()->Unbind( 3u );
		p_gp[size_t( DsTexture::eDiffuse )]->GetTexture()->Unbind( 2u );
		p_gp[size_t( DsTexture::eDiffuse )]->GetSampler()->Unbind( 2u );
		p_gp[size_t( DsTexture::eNormal )]->GetTexture()->Unbind( 1u );
		p_gp[size_t( DsTexture::eNormal )]->GetSampler()->Unbind( 1u );
		p_gp[size_t( DsTexture::eDepth )]->GetTexture()->Unbind( 0u );
		p_gp[size_t( DsTexture::eDepth )]->GetSampler()->Unbind( 0u );
	}
}
