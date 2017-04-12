#include "Skybox.hpp"

#include "Engine.hpp"

#include "Mesh/Buffer/BufferElementGroup.hpp"
#include "Mesh/Buffer/GeometryBuffers.hpp"
#include "Render/RenderPipeline.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Scene.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <GlslSource.hpp>
#include <GlslUtils.hpp>

using namespace Castor;
using namespace GLSL;

namespace Castor3D
{
	Skybox::TextWriter::TextWriter( String const & p_tabs )
		: Castor::TextWriter< Skybox >{ p_tabs }
	{
	}

	bool Skybox::TextWriter::operator()( Skybox const & p_obj, TextFile & p_file )
	{
		static String const l_faces[]
		{
			cuT( "right" ),
			cuT( "left" ),
			cuT( "bottom" ),
			cuT( "top" ),
			cuT( "back" ),
			cuT( "front" ),
		};

		bool l_return = true;

		if ( Castor::File::FileExists( Path{ p_obj.m_texture->GetImage( size_t( CubeMapFace( 0 ) ) ).ToString() } )
			&& Castor::File::FileExists( Path{ p_obj.m_texture->GetImage( size_t( CubeMapFace( 1 ) ) ).ToString() } )
			&& Castor::File::FileExists( Path{ p_obj.m_texture->GetImage( size_t( CubeMapFace( 2 ) ) ).ToString() } )
			&& Castor::File::FileExists( Path{ p_obj.m_texture->GetImage( size_t( CubeMapFace( 3 ) ) ).ToString() } )
			&& Castor::File::FileExists( Path{ p_obj.m_texture->GetImage( size_t( CubeMapFace( 4 ) ) ).ToString() } )
			&& Castor::File::FileExists( Path{ p_obj.m_texture->GetImage( size_t( CubeMapFace( 5 ) ) ).ToString() } ) )
		{
			l_return = p_file.WriteText( cuT( "\n" ) + m_tabs + cuT( "skybox\n" ) ) > 0
				&& p_file.WriteText( m_tabs + cuT( "{\n" ) ) > 0;
			Path l_subfolder{ cuT( "Textures" ) };

			for ( uint32_t i = 0; i < 6 && l_return; ++i )
			{
				Path l_relative = Scene::TextWriter::CopyFile( Path{ p_obj.m_texture->GetImage( size_t( CubeMapFace( i ) ) ).ToString() }
					, p_file.GetFilePath()
					, l_subfolder );
				l_return = p_file.WriteText( m_tabs + cuT( "\t" ) + l_faces[i] + cuT( " \"" ) + l_relative + cuT( "\"\n" ) ) > 0;
				Castor::TextWriter< Skybox >::CheckError( l_return, ( "Skybox " + l_faces[i] ).c_str() );
			}

			if ( l_return )
			{
				l_return = p_file.WriteText( m_tabs + cuT( "}\n" ) ) > 0;
			}
		}

		return l_return;
	}

	//************************************************************************************************

	Skybox::Skybox( Engine & p_engine )
		: OwnedBy< Engine >{ p_engine }
		, m_texture{ GetEngine()->GetRenderSystem()->CreateTexture( TextureType::eCube, AccessType::eNone, AccessType::eRead ) }
		, m_matrixUbo{ p_engine }
		, m_modelMatrixUbo{ p_engine }
		, m_configUbo{ ShaderProgram::BufferHdrConfig, *p_engine.GetRenderSystem() }
		, m_declaration
		{
			{
				BufferElementDeclaration{ ShaderProgram::Position, uint32_t( ElementUsage::ePosition ), ElementType::eVec3 }
			}
		}
	{
		m_gammaUniform = m_configUbo.CreateUniform< UniformType::eFloat >( ShaderProgram::Gamma );
		m_exposureUniform = m_configUbo.CreateUniform< UniformType::eFloat >( ShaderProgram::Exposure );
		String const l_skybox = cuT( "Skybox" );

		if ( GetEngine()->GetSamplerCache().Has( l_skybox ) )
		{
			m_sampler = GetEngine()->GetSamplerCache().Find( l_skybox );
		}
		else
		{
			auto l_sampler = GetEngine()->GetSamplerCache().Add( l_skybox );
			l_sampler->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
			l_sampler->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
			l_sampler->SetWrappingMode( TextureUVW::eU, WrapMode::eClampToEdge );
			l_sampler->SetWrappingMode( TextureUVW::eV, WrapMode::eClampToEdge );
			l_sampler->SetWrappingMode( TextureUVW::eW, WrapMode::eClampToEdge );
			m_sampler = l_sampler;
		}

		m_bufferVertex =
		{
			-1.0_r, +1.0_r, -1.0_r,/**/+1.0_r, -1.0_r, -1.0_r,/**/-1.0_r, -1.0_r, -1.0_r,/**/+1.0_r, -1.0_r, -1.0_r,/**/-1.0_r, +1.0_r, -1.0_r,/**/+1.0_r, +1.0_r, -1.0_r,/**/
			-1.0_r, -1.0_r, +1.0_r,/**/-1.0_r, +1.0_r, -1.0_r,/**/-1.0_r, -1.0_r, -1.0_r,/**/-1.0_r, +1.0_r, -1.0_r,/**/-1.0_r, -1.0_r, +1.0_r,/**/-1.0_r, +1.0_r, +1.0_r,/**/
			+1.0_r, -1.0_r, -1.0_r,/**/+1.0_r, +1.0_r, +1.0_r,/**/+1.0_r, -1.0_r, +1.0_r,/**/+1.0_r, +1.0_r, +1.0_r,/**/+1.0_r, -1.0_r, -1.0_r,/**/+1.0_r, +1.0_r, -1.0_r,/**/
			-1.0_r, -1.0_r, +1.0_r,/**/+1.0_r, +1.0_r, +1.0_r,/**/-1.0_r, +1.0_r, +1.0_r,/**/+1.0_r, +1.0_r, +1.0_r,/**/-1.0_r, -1.0_r, +1.0_r,/**/+1.0_r, -1.0_r, +1.0_r,/**/
			-1.0_r, +1.0_r, -1.0_r,/**/+1.0_r, +1.0_r, +1.0_r,/**/+1.0_r, +1.0_r, -1.0_r,/**/+1.0_r, +1.0_r, +1.0_r,/**/-1.0_r, +1.0_r, -1.0_r,/**/-1.0_r, +1.0_r, +1.0_r,/**/
			-1.0_r, -1.0_r, -1.0_r,/**/+1.0_r, -1.0_r, -1.0_r,/**/-1.0_r, -1.0_r, +1.0_r,/**/+1.0_r, -1.0_r, -1.0_r,/**/+1.0_r, -1.0_r, +1.0_r,/**/-1.0_r, -1.0_r, +1.0_r,/**/
		};

		uint32_t i = 0;

		for ( auto & l_vertex : m_arrayVertex )
		{
			l_vertex = std::make_shared< BufferElementGroup >( &reinterpret_cast< uint8_t * >( m_bufferVertex.data() )[i++ * m_declaration.stride()] );
		}
	}

	Skybox::~Skybox()
	{
		for ( auto & l_vertex : m_arrayVertex )
		{
			l_vertex.reset();
		}
	}

	bool Skybox::Initialise()
	{
		REQUIRE( m_texture );
		auto & l_program = DoInitialiseShader();
		return DoInitialiseTexture()
			   && DoInitialiseVertexBuffer()
			   && DoInitialisePipeline( l_program );
	}

	void Skybox::Cleanup()
	{
		REQUIRE( m_texture );
		m_texture->Cleanup();
		m_texture.reset();
		m_geometryBuffers->Cleanup();
		m_geometryBuffers.reset();
		m_vertexBuffer->Cleanup();
		m_vertexBuffer.reset();
		m_matrixUbo.GetUbo().Cleanup();
		m_modelMatrixUbo.GetUbo().Cleanup();
		m_exposureUniform.reset();
		m_gammaUniform.reset();
		m_configUbo.Cleanup();
		m_pipeline->Cleanup();
		m_pipeline.reset();
	}

	void Skybox::Render( Camera const & p_camera )
	{
		REQUIRE( m_texture );
		auto l_sampler = m_sampler.lock();

		if ( l_sampler )
		{
			p_camera.Apply();
			auto & l_scene = *p_camera.GetScene();
			auto l_node = p_camera.GetParent();
			matrix::set_translate( m_mtxModel, l_node->GetDerivedPosition() );
			m_matrixUbo.Update( p_camera.GetView()
				, p_camera.GetViewport().GetProjection() );
			m_modelMatrixUbo.Update( m_mtxModel, m_mtxModel );
			m_exposureUniform->SetValue( l_scene.GetHdrConfig().GetExposure() );
			m_gammaUniform->SetValue( l_scene.GetHdrConfig().GetGamma() );
			m_configUbo.Update();
			m_pipeline->Apply();
			m_texture->Bind( 0 );
			l_sampler->Bind( 0 );
			m_geometryBuffers->Draw( uint32_t( m_arrayVertex.size() ), 0 );
			l_sampler->Unbind( 0 );
			m_texture->Unbind( 0 );
		}
	}

	ShaderProgram & Skybox::DoInitialiseShader()
	{
		auto l_program = GetEngine()->GetShaderProgramCache().GetNewProgram( false );

		String l_vtx;
		{
			GlslWriter l_writer{ GetEngine()->GetRenderSystem()->CreateGlslWriter() };

			// Inputs
			auto position = l_writer.GetAttribute< Vec3 >( ShaderProgram::Position );
			UBO_MATRIX( l_writer );
			UBO_MODEL_MATRIX( l_writer );

			// Outputs
			auto vtx_texture = l_writer.GetOutput< Vec3 >( cuT( "vtx_texture" ) );
			auto gl_Position = l_writer.GetBuiltin< Vec4 >( cuT( "gl_Position" ) );

			std::function< void() > l_main = [&]()
			{
				gl_Position = l_writer.Paren( c3d_mtxProjection * c3d_mtxView * c3d_mtxModel * vec4( position, 1.0 ) ).SWIZZLE_XYWW;
				vtx_texture = position;
			};

			l_writer.ImplementFunction< void >( cuT( "main" ), l_main );
			l_vtx = l_writer.Finalise();
		}

		String l_pxl;
		{
			GlslWriter l_writer{ GetEngine()->GetRenderSystem()->CreateGlslWriter() };

			// Inputs
			Ubo l_config{ l_writer, ShaderProgram::BufferHdrConfig };
			auto c3d_fExposure = l_config.GetUniform< Float >( ShaderProgram::Exposure );
			auto c3d_fGamma = l_config.GetUniform< Float >( ShaderProgram::Gamma );
			l_config.End();
			auto vtx_texture = l_writer.GetInput< Vec3 >( cuT( "vtx_texture" ) );
			auto skybox = l_writer.GetUniform< SamplerCube >( cuT( "skybox" ) );
			GLSL::Utils l_utils{ l_writer };
			l_utils.DeclareRemoveGamma();

			// Outputs
			auto pxl_FragColor = l_writer.GetOutput< Vec4 >( cuT( "pxl_FragColor" ) );

			std::function< void() > l_main = [&]()
			{
				auto l_skybox = l_writer.GetLocale( cuT( "l_skybox" )
					, texture( skybox, vec3( vtx_texture.x(), -vtx_texture.y(), vtx_texture.z() ) ) );
				pxl_FragColor = vec4( l_utils.RemoveGamma( c3d_fGamma, l_skybox.xyz() ), l_skybox.w() );
			};

			l_writer.ImplementFunction< void >( cuT( "main" ), l_main );
			l_pxl = l_writer.Finalise();
		}

		auto l_model = GetEngine()->GetRenderSystem()->GetGpuInformations().GetMaxShaderModel();
		l_program->CreateObject( ShaderType::eVertex );
		l_program->CreateObject( ShaderType::ePixel );
		l_program->SetSource( ShaderType::eVertex, l_model, l_vtx );
		l_program->SetSource( ShaderType::ePixel, l_model, l_pxl );
		l_program->Initialise();
		return *l_program;
	}

	bool Skybox::DoInitialiseVertexBuffer()
	{
		m_vertexBuffer = std::make_shared< VertexBuffer >( *GetEngine(), m_declaration );
		m_vertexBuffer->Resize( uint32_t( m_arrayVertex.size() * m_declaration.stride() ) );
		m_vertexBuffer->LinkCoords( m_arrayVertex.begin(), m_arrayVertex.end() );
		return m_vertexBuffer->Initialise( BufferAccessType::eStatic, BufferAccessNature::eDraw );
	}

	bool Skybox::DoInitialiseTexture()
	{
		return m_texture->Initialise();
	}

	bool Skybox::DoInitialisePipeline( ShaderProgram & p_program )
	{
		DepthStencilState l_dsState;
		l_dsState.SetDepthTest( true );
		l_dsState.SetDepthMask( WritingMask::eZero );
		l_dsState.SetDepthFunc( DepthFunc::eLEqual );

		RasteriserState l_rsState;
		l_rsState.SetCulledFaces( Culling::eFront );

		m_pipeline = GetEngine()->GetRenderSystem()->CreateRenderPipeline( std::move( l_dsState ), std::move( l_rsState ), BlendState{}, MultisampleState{}, p_program, PipelineFlags{} );
		m_pipeline->AddUniformBuffer( m_matrixUbo.GetUbo() );
		m_pipeline->AddUniformBuffer( m_modelMatrixUbo.GetUbo() );
		m_pipeline->AddUniformBuffer( m_configUbo );
		m_geometryBuffers = GetEngine()->GetRenderSystem()->CreateGeometryBuffers( Topology::eTriangles, m_pipeline->GetProgram() );
		return m_geometryBuffers->Initialise( { *m_vertexBuffer }, nullptr );
	}
}
