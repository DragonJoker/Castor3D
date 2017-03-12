#include "DirectionalLightPassShadow.hpp"

#include <Engine.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Scene/Scene.hpp>
#include <Scene/Light/DirectionalLight.hpp>
#include <Shader/ShaderProgram.hpp>
#include <Texture/Sampler.hpp>
#include <Texture/TextureImage.hpp>
#include <Texture/TextureLayout.hpp>

#include <GlslSource.hpp>
#include <GlslLight.hpp>
#include <GlslShadow.hpp>

using namespace Castor;
using namespace Castor3D;

namespace deferred
{
	//*********************************************************************************************

	namespace
	{
		static constexpr uint32_t VertexCount = 6u;

		String DoGetShadowPixelShaderSource( Engine const & p_engine
			, TextureChannels const & p_textureFlags
			, ProgramFlags const & p_programFlags
			, SceneFlags const & p_sceneFlags )
		{
			using namespace GLSL;
			GlslWriter l_writer = p_engine.GetRenderSystem()->CreateGlslWriter();

			// Fragment Intputs
			auto vtx_texture = l_writer.GetInput< Vec3 >( cuT( "vtx_texture" ) );
			auto c3d_mapOpacity( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapOpacity, CheckFlag( p_textureFlags, TextureChannel::eOpacity ) ) );
			auto gl_FragCoord( l_writer.GetBuiltin< Vec4 >( cuT( "gl_FragCoord" ) ) );

			// Fragment Outputs
			auto pxl_fFragDepth( l_writer.GetFragData< Float >( cuT( "pxl_fFragDepth" ), 0 ) );

			l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				Float l_fAlpha;

				if ( CheckFlag( p_textureFlags, TextureChannel::eOpacity ) )
				{
					l_fAlpha = l_writer.GetLocale( cuT( "l_fAlpha" ), texture( c3d_mapOpacity, vtx_texture.xy() ).r() );

					IF( l_writer, l_fAlpha < 1.0_f )
					{
						l_writer.Discard();
					}
					FI;
				}
				else
				{
					pxl_fFragDepth = gl_FragCoord.z();
				}
			} );

			return l_writer.Finalise();
		}
	}

	//*********************************************************************************************

	void DirectionalLightPassShadow::Program::Create( Scene const & p_scene
		, String const & p_vtx
		, String const & p_pxl
		, uint16_t p_fogType )
	{
		DoCreate( p_scene, p_vtx, p_pxl, p_fogType );
		m_program->CreateUniform< UniformType::eSampler >( GLSL::Shadow::MapShadowDirectional, ShaderType::ePixel )->SetValue( int( DsTexture::eCount ) );
		m_lightDirection = m_program->CreateUniform< UniformType::eVec3f >( cuT( "light.m_v3Direction" ), ShaderType::ePixel );
		m_lightTransform = m_program->CreateUniform< UniformType::eMat4x4f >( cuT( "light.m_mtxLightSpace" ), ShaderType::ePixel );

		DepthStencilState l_dsstate1;
		l_dsstate1.SetDepthTest( false );
		l_dsstate1.SetDepthMask( WritingMask::eZero );
		m_firstPipeline = m_program->GetRenderSystem()->CreateRenderPipeline( std::move( l_dsstate1 )
			, RasteriserState{}
			, BlendState{}
			, MultisampleState{}
			, *m_program
			, PipelineFlags{} );

		DepthStencilState l_dsstate2;
		l_dsstate2.SetDepthTest( false );
		l_dsstate2.SetDepthMask( WritingMask::eZero );
		BlendState l_blstate;
		l_blstate.EnableBlend( true );
		l_blstate.SetRgbBlendOp( BlendOperation::eAdd );
		l_blstate.SetRgbSrcBlend( BlendOperand::eOne );
		l_blstate.SetRgbDstBlend( BlendOperand::eOne );
		m_blendPipeline = m_program->GetRenderSystem()->CreateRenderPipeline( std::move( l_dsstate2 )
			, RasteriserState{}
			, std::move( l_blstate )
			, MultisampleState{}
			, *m_program
			, PipelineFlags{} );
	}

	void DirectionalLightPassShadow::Program::Destroy()
	{
		m_lightDirection = nullptr;
		m_lightTransform = nullptr;
		DoDestroy();
	}

	void DirectionalLightPassShadow::Program::Initialise( VertexBuffer & p_vbo
		, UniformBuffer & p_matrixUbo
		, UniformBuffer & p_sceneUbo )
	{
		DoInitialise( p_matrixUbo, p_sceneUbo );
		m_geometryBuffers = m_program->GetRenderSystem()->CreateGeometryBuffers( Topology::eTriangles, *m_program );
		m_geometryBuffers->Initialise( { p_vbo }, nullptr );
	}

	void DirectionalLightPassShadow::Program::Cleanup()
	{
		m_geometryBuffers->Cleanup();
		m_geometryBuffers.reset();
		DoCleanup();
	}

	void DirectionalLightPassShadow::Program::Render( Size const & p_size
		, Castor3D::DirectionalLight const & p_light
		, bool p_first )
	{
		DoBind( p_size, p_light, p_first );
		m_lightDirection->SetValue( p_light.GetDirection() );
		m_lightTransform->SetValue( p_light.GetLightSpaceTransform() );
		m_currentPipeline->Apply();
		m_geometryBuffers->Draw( VertexCount, 0 );
	}

	//*********************************************************************************************

	DirectionalLightPassShadow::DirectionalLightPassShadow( Engine & p_engine
		, Castor3D::TextureUnit & p_shadowMap )
		: LightPassShadow{ p_engine }
		, m_viewport{ p_engine }
		, m_shadowMap{ p_shadowMap }
	{
		m_shadowMap.SetIndex( uint32_t( DsTexture::eCount ) );
	}

	void DirectionalLightPassShadow::Create( Scene const & p_scene )
	{
		uint16_t l_fogType{ 0u };

		for ( auto & l_program : m_programs )
		{
			SceneFlags l_sceneFlags{ p_scene.GetFlags() };
			RemFlag( l_sceneFlags, SceneFlag::eFogSquaredExponential );
			AddFlag( l_sceneFlags, SceneFlag( l_fogType ) );
			l_program.Create( p_scene
				, DoGetVertexShaderSource( l_sceneFlags )
				, DoGetPixelShaderSource( l_sceneFlags, LightType::eDirectional )
				, l_fogType++ );
		}

		auto l_declaration = BufferDeclaration(
		{
			BufferElementDeclaration( ShaderProgram::Position, uint32_t( ElementUsage::ePosition ), ElementType::eVec2 ),
		} );

		real l_data[] =
		{
			0, 0,
			1, 1,
			0, 1,
			0, 0,
			1, 0,
			1, 1,
		};

		m_vertexBuffer = std::make_shared< VertexBuffer >( m_engine, l_declaration );
		uint32_t l_stride = l_declaration.stride();
		m_vertexBuffer->Resize( sizeof( l_data ) );
		uint8_t * l_buffer = m_vertexBuffer->data();
		std::memcpy( l_buffer, l_data, sizeof( l_data ) );
		m_viewport.SetOrtho( 0, 1, 0, 1, 0, 1 );
	}

	void DirectionalLightPassShadow::Destroy()
	{
		m_vertexBuffer.reset();

		for ( auto & l_program : m_programs )
		{
			l_program.Destroy();
		}
	}

	void DirectionalLightPassShadow::Initialise( Castor3D::UniformBuffer & p_sceneUbo )
	{
		m_projectionUniform = m_matrixUbo.GetUniform< UniformType::eMat4x4f >( RenderPipeline::MtxProjection );
		m_vertexBuffer->Initialise( BufferAccessType::eStatic, BufferAccessNature::eDraw );
		
		for ( auto & l_program : m_programs )
		{
			l_program.Initialise( *m_vertexBuffer
				, m_matrixUbo
				, p_sceneUbo );
		}
		
		m_viewport.Initialise();
	}

	void DirectionalLightPassShadow::Cleanup()
	{
		m_viewport.Cleanup();

		for ( auto & l_program : m_programs )
		{
			l_program.Cleanup();
		}

		m_vertexBuffer->Cleanup();
		m_projectionUniform = nullptr;
		m_matrixUbo.Cleanup();
	}

	void DirectionalLightPassShadow::Render( Size const & p_size
		, GeometryPassResult const & p_gp
		, Castor3D::DirectionalLight const & p_light
		, uint16_t p_fogType
		, bool p_first )
	{
		m_viewport.Resize( p_size );
		m_viewport.Update();

		DoBeginRender( p_size, p_gp );
		m_shadowMap.Bind();
		auto & l_program = m_programs[uint16_t( GetFogType( p_fogType ) )];
		m_projectionUniform->SetValue( m_viewport.GetProjection() );
		m_matrixUbo.Update();
		l_program.Render( p_size
			, p_light
			, p_first );
		m_shadowMap.Unbind();
		DoEndRender( p_gp );
	}
	
	String DirectionalLightPassShadow::DoGetVertexShaderSource( SceneFlags const & p_sceneFlags )const
	{
		using namespace GLSL;
		GlslWriter l_writer = m_engine.GetRenderSystem()->CreateGlslWriter();

		// Shader inputs
		UBO_MATRIX( l_writer );
		auto vertex = l_writer.GetAttribute< Vec2 >( ShaderProgram::Position );

		// Shader outputs
		auto gl_Position = l_writer.GetBuiltin< Vec4 >( cuT( "gl_Position" ) );

		l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			gl_Position = c3d_mtxProjection * vec4( vertex.x(), vertex.y(), 0.0, 1.0 );
		} );

		return l_writer.Finalise();
	}
}
