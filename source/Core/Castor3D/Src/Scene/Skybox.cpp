#include "Skybox.hpp"

#include "Engine.hpp"

#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/DepthStencilRenderBuffer.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "Mesh/Buffer/BufferElementGroup.hpp"
#include "Mesh/Buffer/GeometryBuffers.hpp"
#include "Render/RenderPipeline.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Scene.hpp"
#include "EnvironmentMap/EnvironmentMap.hpp"
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
		, m_texture{ GetEngine()->GetRenderSystem()->CreateTexture( TextureType::eCube
			, AccessType::eNone
			, AccessType::eRead ) }
		, m_matrixUbo{ p_engine }
		, m_modelMatrixUbo{ p_engine }
		, m_configUbo{ p_engine }
		, m_declaration
		{
			{
				BufferElementDeclaration{ ShaderProgram::Position, uint32_t( ElementUsage::ePosition ), ElementType::eVec3 }
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
		REQUIRE( m_scene );
		REQUIRE( m_texture );
		bool l_result = DoInitialiseTexture();
		auto & l_program = DoInitialiseShader();

		if ( l_result )
		{
			if ( m_scene->GetMaterialsType() == MaterialType::ePbr )
			{
				m_ibl = std::make_unique< IblTextures >( *m_scene );
				m_ibl->Update( *m_texture );
			}

			l_result = DoInitialiseVertexBuffer()
				&& DoInitialisePipeline( l_program );
		}

		return l_result;
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
		m_configUbo.GetUbo().Cleanup();
		m_pipeline->Cleanup();
		m_pipeline.reset();
		m_ibl.reset();
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
			m_modelMatrixUbo.Update( m_mtxModel );
			m_configUbo.Update( l_scene.GetHdrConfig() );
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

		GLSL::Shader l_vtx;
		{
			GlslWriter l_writer{ GetEngine()->GetRenderSystem()->CreateGlslWriter() };

			// Inputs
			auto position = l_writer.DeclAttribute< Vec3 >( ShaderProgram::Position );
			UBO_MATRIX( l_writer );
			UBO_MODEL_MATRIX( l_writer );

			// Outputs
			auto vtx_texture = l_writer.DeclOutput< Vec3 >( cuT( "vtx_texture" ) );
			auto gl_Position = l_writer.DeclBuiltin< Vec4 >( cuT( "gl_Position" ) );

			std::function< void() > l_main = [&]()
			{
				gl_Position = l_writer.Paren( c3d_mtxProjection * c3d_mtxView * c3d_mtxModel * vec4( position, 1.0 ) ).SWIZZLE_XYWW;
				vtx_texture = position;
			};

			l_writer.ImplementFunction< void >( cuT( "main" ), l_main );
			l_vtx = l_writer.Finalise();
		}

		GLSL::Shader l_pxl;
		{
			GlslWriter l_writer{ GetEngine()->GetRenderSystem()->CreateGlslWriter() };

			// Inputs
			UBO_HDR_CONFIG( l_writer );
			auto vtx_texture = l_writer.DeclInput< Vec3 >( cuT( "vtx_texture" ) );
			auto skybox = l_writer.DeclUniform< SamplerCube >( cuT( "skybox" ) );
			GLSL::Utils l_utils{ l_writer };

			if ( !m_hdr )
			{
				l_utils.DeclareRemoveGamma();
			}

			// Outputs
			auto pxl_FragColor = l_writer.DeclOutput< Vec4 >( cuT( "pxl_FragColor" ) );

			std::function< void() > l_main = [&]()
			{
				auto l_skybox = l_writer.DeclLocale( cuT( "l_skybox" )
					, texture( skybox, vec3( vtx_texture.x(), -vtx_texture.y(), vtx_texture.z() ) ) );

				if ( !m_hdr )
				{
					pxl_FragColor = vec4( l_utils.RemoveGamma( c3d_gamma, l_skybox.xyz() ), l_skybox.w() );
				}
				else
				{
					pxl_FragColor = vec4( l_skybox.xyz(), l_skybox.w() );
				}
			};

			l_writer.ImplementFunction< void >( cuT( "main" ), l_main );
			l_pxl = l_writer.Finalise();
		}

		l_program->CreateObject( ShaderType::eVertex );
		l_program->CreateObject( ShaderType::ePixel );
		l_program->SetSource( ShaderType::eVertex, l_vtx );
		l_program->SetSource( ShaderType::ePixel, l_pxl );
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
		if ( m_equiTexture )
		{
			DoInitialiseEquiTexture();
			m_hdr = true;
		}

		auto l_result = m_texture->Initialise();

		if ( l_result )
		{
			auto l_sampler = m_sampler.lock();
			m_texture->Bind( 0 );
			m_texture->GenerateMipmaps();
			m_texture->Unbind( 0 );
		}

		return l_result;
	}

	void Skybox::DoInitialiseEquiTexture()
	{
		auto & l_engine = *GetEngine();
		auto & l_renderSystem = *l_engine.GetRenderSystem();
		m_equiTexture->Initialise();

		// Create the cube texture.
		m_texture = l_renderSystem.CreateTexture( TextureType::eCube
			, AccessType::eNone
			, AccessType::eRead | AccessType::eWrite
			, PixelFormat::eRGB32F
			, m_equiSize );
		m_texture->GetImage( uint32_t( CubeMapFace::ePositiveX ) ).InitialiseSource();
		m_texture->GetImage( uint32_t( CubeMapFace::eNegativeX ) ).InitialiseSource();
		m_texture->GetImage( uint32_t( CubeMapFace::ePositiveY ) ).InitialiseSource();
		m_texture->GetImage( uint32_t( CubeMapFace::eNegativeY ) ).InitialiseSource();
		m_texture->GetImage( uint32_t( CubeMapFace::ePositiveZ ) ).InitialiseSource();
		m_texture->GetImage( uint32_t( CubeMapFace::eNegativeZ ) ).InitialiseSource();
		m_texture->Initialise();

		// Create the one shot FBO and attaches
		auto l_fbo = l_renderSystem.CreateFrameBuffer();
		std::array< FrameBufferAttachmentSPtr, 6 > l_attachs
		{
			{
				l_fbo->CreateAttachment( m_texture, CubeMapFace::ePositiveX ),
				l_fbo->CreateAttachment( m_texture, CubeMapFace::eNegativeX ),
				l_fbo->CreateAttachment( m_texture, CubeMapFace::ePositiveY ),
				l_fbo->CreateAttachment( m_texture, CubeMapFace::eNegativeY ),
				l_fbo->CreateAttachment( m_texture, CubeMapFace::ePositiveZ ),
				l_fbo->CreateAttachment( m_texture, CubeMapFace::eNegativeZ ),
			}
		};
		// Create The depth RBO.
		auto l_depthRbo = l_fbo->CreateDepthStencilRenderBuffer( PixelFormat::eD24 );
		l_depthRbo->Create();
		l_depthRbo->Initialise( m_equiSize );
		auto l_depthAttach = l_fbo->CreateAttachment( l_depthRbo );

		// Fill the FBO
		l_fbo->Create();
		l_fbo->Initialise( m_equiSize );
		l_fbo->Bind();
		l_fbo->Attach( AttachmentPoint::eDepth, l_depthAttach );
		REQUIRE( l_fbo->IsComplete() );
		l_fbo->Unbind();

		// Render the equirectangular texture to the cube faces.
		l_renderSystem.GetCurrentContext()->RenderEquiToCube( m_equiSize
			, *m_equiTexture
			, m_texture
			, l_fbo
			, l_attachs );

		// Cleanup the one shot FBO and attaches
		l_fbo->Bind();
		l_fbo->DetachAll();
		l_fbo->Unbind();

		l_depthRbo->Cleanup();
		l_depthRbo->Destroy();

		for ( auto & l_attach : l_attachs )
		{
			l_attach.reset();
		}

		l_depthAttach.reset();
		l_fbo->Cleanup();
		l_fbo->Destroy();

		m_equiTexture->Cleanup();
		m_equiTexture.reset();
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
		m_pipeline->AddUniformBuffer( m_configUbo.GetUbo() );
		m_geometryBuffers = GetEngine()->GetRenderSystem()->CreateGeometryBuffers( Topology::eTriangles, m_pipeline->GetProgram() );
		return m_geometryBuffers->Initialise( { *m_vertexBuffer }, nullptr );
	}
}
