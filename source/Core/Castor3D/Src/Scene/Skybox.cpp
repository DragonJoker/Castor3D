#include "Skybox.hpp"

#include "Engine.hpp"
#include "ShaderCache.hpp"

#include "DepthStencilStateCache.hpp"
#include "Engine.hpp"
#include "RasteriserStateCache.hpp"
#include "SamplerCache.hpp"

#include "Mesh/Buffer/BufferElementDeclaration.hpp"
#include "Mesh/Buffer/BufferElementGroup.hpp"
#include "Mesh/Buffer/GeometryBuffers.hpp"
#include "Mesh/Buffer/VertexBuffer.hpp"
#include "Render/Pipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Scene.hpp"
#include "Scene/SceneNode.hpp"
#include "Shader/ShaderProgram.hpp"
#include "State/DepthStencilState.hpp"
#include "State/RasteriserState.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"
#include "Texture/TextureImage.hpp"

#include <GlslSource.hpp>

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
		bool l_return = p_file.WriteText( cuT( "\n" ) + m_tabs + cuT( "skybox\n" ) ) > 0
			&& p_file.WriteText( m_tabs + cuT( "{\n" ) ) > 0;

		Path l_subfolder{ cuT( "Textures" ) };

		if ( l_return )
		{
			Path l_relative = Scene::TextWriter::CopyFile( Path{ p_obj.m_texture->GetImage( size_t( CubeMapFace::PositiveX ) ).ToString() }, p_file.GetFilePath(), l_subfolder );
			l_return = p_file.WriteText( m_tabs + cuT( "\tright \"" ) + l_relative + cuT( "\"\n" ) ) > 0;
		}

		if ( l_return )
		{
			Path l_relative = Scene::TextWriter::CopyFile( Path{ p_obj.m_texture->GetImage( size_t( CubeMapFace::NegativeX ) ).ToString() }, p_file.GetFilePath(), l_subfolder );
			l_return = p_file.WriteText( m_tabs + cuT( "\tleft \"" ) + l_relative + cuT( "\"\n" ) ) > 0;
		}

		if ( l_return )
		{
			Path l_relative = Scene::TextWriter::CopyFile( Path{ p_obj.m_texture->GetImage( size_t( CubeMapFace::NegativeY ) ).ToString() }, p_file.GetFilePath(), l_subfolder );
			l_return = p_file.WriteText( m_tabs + cuT( "\ttop \"" ) + l_relative + cuT( "\"\n" ) ) > 0;
		}

		if ( l_return )
		{
			Path l_relative = Scene::TextWriter::CopyFile( Path{ p_obj.m_texture->GetImage( size_t( CubeMapFace::PositiveY ) ).ToString() }, p_file.GetFilePath(), l_subfolder );
			l_return = p_file.WriteText( m_tabs + cuT( "\tbottom \"" ) + l_relative + cuT( "\"\n" ) ) > 0;
		}

		if ( l_return )
		{
			Path l_relative = Scene::TextWriter::CopyFile( Path{ p_obj.m_texture->GetImage( size_t( CubeMapFace::PositiveZ ) ).ToString() }, p_file.GetFilePath(), l_subfolder );
			l_return = p_file.WriteText( m_tabs + cuT( "\tback \"" ) + l_relative + cuT( "\"\n" ) ) > 0;
		}

		if ( l_return )
		{
			Path l_relative = Scene::TextWriter::CopyFile( Path{ p_obj.m_texture->GetImage( size_t( CubeMapFace::NegativeZ ) ).ToString() }, p_file.GetFilePath(), l_subfolder );
			l_return = p_file.WriteText( m_tabs + cuT( "\tfront \"" ) + l_relative + cuT( "\"\n" ) ) > 0;
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return l_return;
	}

	//************************************************************************************************

	Skybox::Skybox( Engine & p_engine )
		: OwnedBy< Engine >{ p_engine }
		, m_texture{ GetEngine()->GetRenderSystem()->CreateTexture( TextureType::Cube, 0, eACCESS_TYPE_READ ) }
		, m_declaration
		{
			{
				BufferElementDeclaration{ ShaderProgram::Position, eELEMENT_USAGE_POSITION, eELEMENT_TYPE_3FLOATS }
			}
		}
	{
		String const l_skybox = cuT( "Skybox" );

		if ( GetEngine()->GetSamplerCache().Has( l_skybox ) )
		{
			m_sampler = GetEngine()->GetSamplerCache().Find( l_skybox );
		}
		else
		{
			auto l_sampler = GetEngine()->GetSamplerCache().Add( l_skybox );
			l_sampler->SetInterpolationMode( InterpolationFilter::Min, InterpolationMode::Linear );
			l_sampler->SetInterpolationMode( InterpolationFilter::Mag, InterpolationMode::Linear );
			l_sampler->SetWrappingMode( TextureUVW::U, WrapMode::ClampToEdge );
			l_sampler->SetWrappingMode( TextureUVW::V, WrapMode::ClampToEdge );
			l_sampler->SetWrappingMode( TextureUVW::W, WrapMode::ClampToEdge );
			m_sampler = l_sampler;
		}

		if ( GetEngine()->GetDepthStencilStateCache().Has( l_skybox ) )
		{
			m_dss = GetEngine()->GetDepthStencilStateCache().Find( l_skybox );
		}
		else
		{
			auto l_dss = GetEngine()->GetDepthStencilStateCache().Add( l_skybox );
			l_dss->SetDepthFunc( eDEPTH_FUNC_LEQUAL );
			m_dss = l_dss;
		}

		if ( GetEngine()->GetRasteriserStateCache().Has( l_skybox ) )
		{
			m_rs = GetEngine()->GetRasteriserStateCache().Find( l_skybox );
		}
		else
		{
			auto l_rs = GetEngine()->GetRasteriserStateCache().Add( l_skybox );
			l_rs->SetCulledFaces( eFACE_FRONT );
			m_rs = l_rs;
		}

		m_bufferVertex =
		{
			-1.0_r, 1.0_r, -1.0_r,
			1.0_r, -1.0_r, -1.0_r,
			-1.0_r, -1.0_r, -1.0_r,
			1.0_r, -1.0_r, -1.0_r,
			-1.0_r, 1.0_r, -1.0_r,
			1.0_r, 1.0_r, -1.0_r,

			-1.0_r, -1.0_r, 1.0_r,
			-1.0_r, 1.0_r, -1.0_r,
			-1.0_r, -1.0_r, -1.0_r,
			-1.0_r, 1.0_r, -1.0_r,
			-1.0_r, -1.0_r, 1.0_r,
			-1.0_r, 1.0_r, 1.0_r,

			1.0_r, -1.0_r, -1.0_r,
			1.0_r, 1.0_r, 1.0_r,
			1.0_r, -1.0_r, 1.0_r,
			1.0_r, 1.0_r, 1.0_r,
			1.0_r, -1.0_r, -1.0_r,
			1.0_r, 1.0_r, -1.0_r,

			-1.0_r, -1.0_r, 1.0_r,
			1.0_r, 1.0_r, 1.0_r,
			-1.0_r, 1.0_r, 1.0_r,
			1.0_r, 1.0_r, 1.0_r,
			-1.0_r, -1.0_r, 1.0_r,
			1.0_r, -1.0_r, 1.0_r,

			-1.0_r, 1.0_r, -1.0_r,
			1.0_r, 1.0_r, 1.0_r,
			1.0_r, 1.0_r, -1.0_r,
			1.0_r, 1.0_r, 1.0_r,
			-1.0_r, 1.0_r, -1.0_r,
			-1.0_r, 1.0_r, 1.0_r,

			-1.0_r, -1.0_r, -1.0_r,
			1.0_r, -1.0_r, -1.0_r,
			-1.0_r, -1.0_r, 1.0_r,
			1.0_r, -1.0_r, -1.0_r,
			1.0_r, -1.0_r, 1.0_r,
			-1.0_r, -1.0_r, 1.0_r,
		};

		uint32_t i = 0;

		for ( auto & l_vertex : m_arrayVertex )
		{
			l_vertex = std::make_shared< BufferElementGroup >( &reinterpret_cast< uint8_t * >( m_bufferVertex.data() )[i++ * m_declaration.GetStride()] );
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
		return DoInitialiseShader()
			&& DoInitialiseTexture()
			&& DoInitialiseVertexBuffer();
	}

	void Skybox::Cleanup()
	{
		REQUIRE( m_texture );
		m_texture->Cleanup();
		m_texture->Destroy();
		m_texture.reset();
		m_geometryBuffers->Cleanup();
		m_geometryBuffers.reset();
		m_vertexBuffer->Cleanup();
		m_vertexBuffer->Destroy();
		m_vertexBuffer.reset();
	}

	void Skybox::Render( Camera const & p_camera, Pipeline & p_pipeline )
	{
		REQUIRE( m_texture );
		REQUIRE( !m_program.expired() );
		auto l_program = m_program.lock();
		auto l_sampler = m_sampler.lock();

		if ( l_program && l_sampler )
		{
			auto l_node = p_camera.GetParent();
			matrix::set_translate( m_mtxModel, l_node->GetDerivedPosition() );
			p_pipeline.SetModelMatrix( m_mtxModel );
			p_pipeline.ApplyMatrices( *m_matricesBuffer, 0xFFFFFFFFFFFFFFFF );
			m_dss.lock()->Apply();
			m_rs.lock()->Apply();
			l_program->Bind();
			m_texture->Bind( 0 );
			l_sampler->Bind( 0 );
			m_geometryBuffers->Draw( uint32_t( m_arrayVertex.size() ), 0 );
			l_sampler->Unbind( 0 );
			m_texture->Unbind( 0 );
			l_program->Unbind();
		}
	}

	bool Skybox::DoInitialiseShader()
	{
		auto l_program = GetEngine()->GetShaderProgramCache().GetNewProgram();
		m_program = l_program;

		String l_vtx;
		{
			GlslWriter l_writer{ GetEngine()->GetRenderSystem()->CreateGlslWriter() };

			// Inputs
			auto position = l_writer.GetAttribute< Vec3 >( ShaderProgram::Position );
			UBO_MATRIX( l_writer );

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
			auto vtx_texture = l_writer.GetInput< Vec3 >( cuT( "vtx_texture" ) );
			auto skybox = l_writer.GetUniform< SamplerCube >( cuT( "skybox" ) );

			// Outputs
			auto pxl_FragColor = l_writer.GetOutput< Vec4 >( cuT( "pxl_FragColor" ) );

			std::function< void() > l_main = [&]()
			{
				pxl_FragColor = textureCube( skybox, vec3( vtx_texture.SWIZZLE_X, -vtx_texture.SWIZZLE_Y, vtx_texture.SWIZZLE_Z ) );
			};

			l_writer.ImplementFunction< void >( cuT( "main" ), l_main );
			l_pxl = l_writer.Finalise();
		}

		auto l_model = GetEngine()->GetRenderSystem()->GetGpuInformations().GetMaxShaderModel();
		l_program->SetSource( eSHADER_TYPE_VERTEX, l_model, l_vtx );
		l_program->SetSource( eSHADER_TYPE_PIXEL, l_model, l_pxl );
		m_matricesBuffer = GetEngine()->GetShaderProgramCache().CreateMatrixBuffer( *l_program, eSHADER_TYPE_VERTEX );
		return l_program->Initialise();
	}

	bool Skybox::DoInitialiseVertexBuffer()
	{
		m_vertexBuffer = std::make_shared< VertexBuffer >( *GetEngine(), m_declaration );
		m_vertexBuffer->Resize( uint32_t( m_arrayVertex.size() * m_declaration.GetStride() ) );
		m_vertexBuffer->LinkCoords( m_arrayVertex.begin(), m_arrayVertex.end() );
		m_vertexBuffer->Create();
		m_geometryBuffers = GetEngine()->GetRenderSystem()->CreateGeometryBuffers( eTOPOLOGY_TRIANGLES, *m_program.lock() );

		return m_vertexBuffer->Initialise( eBUFFER_ACCESS_TYPE_STATIC, eBUFFER_ACCESS_NATURE_DRAW )
			&& m_geometryBuffers->Initialise( m_vertexBuffer, nullptr, nullptr, nullptr, nullptr );
	}

	bool Skybox::DoInitialiseTexture()
	{
		return m_texture->Create()
			&& m_texture->Initialise();
	}
}
