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
		static String const faces[]
		{
			cuT( "right" ),
			cuT( "left" ),
			cuT( "bottom" ),
			cuT( "top" ),
			cuT( "back" ),
			cuT( "front" ),
		};

		bool result = true;

		if ( Castor::File::FileExists( Path{ p_obj.m_texture->GetImage( size_t( CubeMapFace( 0 ) ) ).ToString() } )
			&& Castor::File::FileExists( Path{ p_obj.m_texture->GetImage( size_t( CubeMapFace( 1 ) ) ).ToString() } )
			&& Castor::File::FileExists( Path{ p_obj.m_texture->GetImage( size_t( CubeMapFace( 2 ) ) ).ToString() } )
			&& Castor::File::FileExists( Path{ p_obj.m_texture->GetImage( size_t( CubeMapFace( 3 ) ) ).ToString() } )
			&& Castor::File::FileExists( Path{ p_obj.m_texture->GetImage( size_t( CubeMapFace( 4 ) ) ).ToString() } )
			&& Castor::File::FileExists( Path{ p_obj.m_texture->GetImage( size_t( CubeMapFace( 5 ) ) ).ToString() } ) )
		{
			result = p_file.WriteText( cuT( "\n" ) + m_tabs + cuT( "skybox\n" ) ) > 0
				&& p_file.WriteText( m_tabs + cuT( "{\n" ) ) > 0;
			Path subfolder{ cuT( "Textures" ) };

			for ( uint32_t i = 0; i < 6 && result; ++i )
			{
				Path relative = Scene::TextWriter::CopyFile( Path{ p_obj.m_texture->GetImage( size_t( CubeMapFace( i ) ) ).ToString() }
					, p_file.GetFilePath()
					, subfolder );
				result = p_file.WriteText( m_tabs + cuT( "\t" ) + faces[i] + cuT( " \"" ) + relative + cuT( "\"\n" ) ) > 0;
				Castor::TextWriter< Skybox >::CheckError( result, ( "Skybox " + faces[i] ).c_str() );
			}

			if ( result )
			{
				result = p_file.WriteText( m_tabs + cuT( "}\n" ) ) > 0;
			}
		}

		return result;
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
		String const skybox = cuT( "Skybox" );

		if ( GetEngine()->GetSamplerCache().Has( skybox ) )
		{
			m_sampler = GetEngine()->GetSamplerCache().Find( skybox );
		}
		else
		{
			auto sampler = GetEngine()->GetSamplerCache().Add( skybox );
			sampler->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
			sampler->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
			sampler->SetWrappingMode( TextureUVW::eU, WrapMode::eClampToEdge );
			sampler->SetWrappingMode( TextureUVW::eV, WrapMode::eClampToEdge );
			sampler->SetWrappingMode( TextureUVW::eW, WrapMode::eClampToEdge );
			m_sampler = sampler;
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

		for ( auto & vertex : m_arrayVertex )
		{
			vertex = std::make_shared< BufferElementGroup >( &reinterpret_cast< uint8_t * >( m_bufferVertex.data() )[i++ * m_declaration.stride()] );
		}
	}

	Skybox::~Skybox()
	{
		for ( auto & vertex : m_arrayVertex )
		{
			vertex.reset();
		}
	}

	bool Skybox::Initialise()
	{
		REQUIRE( m_scene );
		REQUIRE( m_texture );
		bool result = DoInitialiseTexture();
		auto & program = DoInitialiseShader();

		if ( result )
		{
			if ( m_scene->GetMaterialsType() == MaterialType::ePbrMetallicRoughness
				|| m_scene->GetMaterialsType() == MaterialType::ePbrSpecularGlossiness )
			{
				m_ibl = std::make_unique< IblTextures >( *m_scene );
				m_ibl->Update( *m_texture );
			}

			result = DoInitialiseVertexBuffer()
				&& DoInitialisePipeline( program );
		}

		return result;
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
		auto sampler = m_sampler.lock();

		if ( sampler )
		{
			p_camera.Apply();
			auto & scene = *p_camera.GetScene();
			auto node = p_camera.GetParent();
			matrix::set_translate( m_mtxModel, node->GetDerivedPosition() );
			m_matrixUbo.Update( p_camera.GetView()
				, p_camera.GetViewport().GetProjection() );
			m_modelMatrixUbo.Update( m_mtxModel );
			m_configUbo.Update( scene.GetHdrConfig() );
			m_pipeline->Apply();
			m_texture->Bind( 0 );
			sampler->Bind( 0 );
			m_geometryBuffers->Draw( uint32_t( m_arrayVertex.size() ), 0 );
			sampler->Unbind( 0 );
			m_texture->Unbind( 0 );
		}
	}

	ShaderProgram & Skybox::DoInitialiseShader()
	{
		auto program = GetEngine()->GetShaderProgramCache().GetNewProgram( false );

		GLSL::Shader vtx;
		{
			GlslWriter writer{ GetEngine()->GetRenderSystem()->CreateGlslWriter() };

			// Inputs
			auto position = writer.DeclAttribute< Vec3 >( ShaderProgram::Position );
			UBO_MATRIX( writer );
			UBO_MODEL_MATRIX( writer );

			// Outputs
			auto vtx_texture = writer.DeclOutput< Vec3 >( cuT( "vtx_texture" ) );
			auto gl_Position = writer.DeclBuiltin< Vec4 >( cuT( "gl_Position" ) );

			std::function< void() > main = [&]()
			{
				gl_Position = writer.Paren( c3d_mtxProjection * c3d_mtxView * c3d_mtxModel * vec4( position, 1.0 ) ).SWIZZLE_XYWW;
				vtx_texture = position;
			};

			writer.ImplementFunction< void >( cuT( "main" ), main );
			vtx = writer.Finalise();
		}

		GLSL::Shader pxl;
		{
			GlslWriter writer{ GetEngine()->GetRenderSystem()->CreateGlslWriter() };

			// Inputs
			UBO_HDR_CONFIG( writer );
			auto vtx_texture = writer.DeclInput< Vec3 >( cuT( "vtx_texture" ) );
			auto skybox = writer.DeclUniform< SamplerCube >( cuT( "skybox" ) );
			GLSL::Utils utils{ writer };

			if ( !m_hdr )
			{
				utils.DeclareRemoveGamma();
			}

			// Outputs
			auto pxl_FragColor = writer.DeclOutput< Vec4 >( cuT( "pxl_FragColor" ) );

			std::function< void() > main = [&]()
			{
				auto colour = writer.DeclLocale( cuT( "colour" )
					, texture( skybox, vec3( vtx_texture.x(), -vtx_texture.y(), vtx_texture.z() ) ) );

				if ( !m_hdr )
				{
					pxl_FragColor = vec4( utils.RemoveGamma( c3d_gamma, colour.xyz() ), colour.w() );
				}
				else
				{
					pxl_FragColor = vec4( colour.xyz(), colour.w() );
				}
			};

			writer.ImplementFunction< void >( cuT( "main" ), main );
			pxl = writer.Finalise();
		}

		program->CreateObject( ShaderType::eVertex );
		program->CreateObject( ShaderType::ePixel );
		program->SetSource( ShaderType::eVertex, vtx );
		program->SetSource( ShaderType::ePixel, pxl );
		program->Initialise();
		return *program;
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

		auto result = m_texture->Initialise();

		if ( result )
		{
			auto sampler = m_sampler.lock();
			m_texture->Bind( 0 );
			m_texture->GenerateMipmaps();
			m_texture->Unbind( 0 );
		}

		return result;
	}

	void Skybox::DoInitialiseEquiTexture()
	{
		auto & engine = *GetEngine();
		auto & renderSystem = *engine.GetRenderSystem();
		m_equiTexture->Initialise();

		// Create the cube texture.
		m_texture = renderSystem.CreateTexture( TextureType::eCube
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
		auto fbo = renderSystem.CreateFrameBuffer();
		std::array< FrameBufferAttachmentSPtr, 6 > attachs
		{
			{
				fbo->CreateAttachment( m_texture, CubeMapFace::ePositiveX ),
				fbo->CreateAttachment( m_texture, CubeMapFace::eNegativeX ),
				fbo->CreateAttachment( m_texture, CubeMapFace::ePositiveY ),
				fbo->CreateAttachment( m_texture, CubeMapFace::eNegativeY ),
				fbo->CreateAttachment( m_texture, CubeMapFace::ePositiveZ ),
				fbo->CreateAttachment( m_texture, CubeMapFace::eNegativeZ ),
			}
		};
		// Create The depth RBO.
		auto depthRbo = fbo->CreateDepthStencilRenderBuffer( PixelFormat::eD24 );
		depthRbo->Create();
		depthRbo->Initialise( m_equiSize );
		auto depthAttach = fbo->CreateAttachment( depthRbo );

		// Fill the FBO
		fbo->Create();
		fbo->Initialise( m_equiSize );
		fbo->Bind();
		fbo->Attach( AttachmentPoint::eDepth, depthAttach );
		REQUIRE( fbo->IsComplete() );
		fbo->Unbind();

		// Render the equirectangular texture to the cube faces.
		renderSystem.GetCurrentContext()->RenderEquiToCube( m_equiSize
			, *m_equiTexture
			, m_texture
			, fbo
			, attachs );

		// Cleanup the one shot FBO and attaches
		fbo->Bind();
		fbo->DetachAll();
		fbo->Unbind();

		depthRbo->Cleanup();
		depthRbo->Destroy();

		for ( auto & attach : attachs )
		{
			attach.reset();
		}

		depthAttach.reset();
		fbo->Cleanup();
		fbo->Destroy();

		m_equiTexture->Cleanup();
		m_equiTexture.reset();
	}

	bool Skybox::DoInitialisePipeline( ShaderProgram & p_program )
	{
		DepthStencilState dsState;
		dsState.SetDepthTest( true );
		dsState.SetDepthMask( WritingMask::eZero );
		dsState.SetDepthFunc( DepthFunc::eLEqual );

		RasteriserState rsState;
		rsState.SetCulledFaces( Culling::eFront );

		m_pipeline = GetEngine()->GetRenderSystem()->CreateRenderPipeline( std::move( dsState ), std::move( rsState ), BlendState{}, MultisampleState{}, p_program, PipelineFlags{} );
		m_pipeline->AddUniformBuffer( m_matrixUbo.GetUbo() );
		m_pipeline->AddUniformBuffer( m_modelMatrixUbo.GetUbo() );
		m_pipeline->AddUniformBuffer( m_configUbo.GetUbo() );
		m_geometryBuffers = GetEngine()->GetRenderSystem()->CreateGeometryBuffers( Topology::eTriangles, m_pipeline->GetProgram() );
		return m_geometryBuffers->Initialise( { *m_vertexBuffer }, nullptr );
	}
}
