#include "RenderColourCubeToTexture.hpp"

#include "Engine.hpp"

#include "Mesh/Vertex.hpp"
#include "Mesh/Buffer/Buffer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Shader/MatrixUbo.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <GlslSource.hpp>

using namespace Castor;

namespace Castor3D
{
	RenderColourCubeToTexture::RenderColourCubeToTexture( Context & p_context
		, MatrixUbo & p_matrixUbo )
		: OwnedBy< Context >{ p_context }
		, m_matrixUbo{ p_matrixUbo }
		, m_viewport{ *p_context.GetRenderSystem()->GetEngine() }
		, m_bufferVertex
		{
			{
				0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1
			}
		}
		, m_declaration
		{
			{
				BufferElementDeclaration{ ShaderProgram::Position, uint32_t( ElementUsage::ePosition ), ElementType::eVec2 },
				BufferElementDeclaration{ ShaderProgram::Texture, uint32_t( ElementUsage::eTexCoords ), ElementType::eVec2 }
			}
		}
	{
		uint32_t i = 0;

		for ( auto & vertex : m_arrayVertex )
		{
			vertex = std::make_shared< BufferElementGroup >( &reinterpret_cast< uint8_t * >( m_bufferVertex.data() )[i++ * m_declaration.stride()] );
		}
	}

	RenderColourCubeToTexture::~RenderColourCubeToTexture()
	{
	}

	void RenderColourCubeToTexture::Initialise()
	{
		m_viewport.Initialise();
		auto & program = *DoCreateProgram();
		auto & renderSystem = *GetOwner()->GetRenderSystem();
		program.Initialise();
		m_vertexBuffer = std::make_shared< VertexBuffer >( *renderSystem.GetEngine()
			, m_declaration );
		m_vertexBuffer->Resize( uint32_t( m_arrayVertex.size()
			* m_declaration.stride() ) );
		m_vertexBuffer->LinkCoords( m_arrayVertex.begin()
			, m_arrayVertex.end() );
		m_vertexBuffer->Initialise( BufferAccessType::eStatic
			, BufferAccessNature::eDraw );
		m_geometryBuffers = renderSystem.CreateGeometryBuffers( Topology::eTriangles
			, program );
		m_geometryBuffers->Initialise( { *m_vertexBuffer }
		, nullptr );

		DepthStencilState dsState;
		dsState.SetDepthTest( false );
		m_pipeline = renderSystem.CreateRenderPipeline( std::move( dsState )
			, RasteriserState{}
			, BlendState{}
			, MultisampleState{}
			, program
			, PipelineFlags{} );
		m_pipeline->AddUniformBuffer( m_matrixUbo.GetUbo() );

		m_sampler = GetOwner()->GetRenderSystem()->GetEngine()->GetSamplerCache().Add( cuT( "RenderColourCubeToTexture" ) );
		m_sampler->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
		m_sampler->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
		m_sampler->SetWrappingMode( TextureUVW::eU, WrapMode::eClampToEdge );
		m_sampler->SetWrappingMode( TextureUVW::eV, WrapMode::eClampToEdge );
		m_sampler->SetWrappingMode( TextureUVW::eW, WrapMode::eClampToEdge );
	}

	void RenderColourCubeToTexture::Cleanup()
	{
		m_sampler.reset();
		m_pipeline->Cleanup();
		m_pipeline.reset();
		m_vertexBuffer->Cleanup();
		m_vertexBuffer.reset();
		m_geometryBuffers->Cleanup();
		m_geometryBuffers.reset();
		m_viewport.Cleanup();
	}

	void RenderColourCubeToTexture::Render( Position const & p_position
		, Size const & p_size
		, TextureLayout const & p_texture )
	{
		int w = p_size.width();
		int h = p_size.height();
		DoRender( Position{ p_position.x() + w * 0, p_position.y() + h * 1 }
			, p_size
			, p_texture
			, Point3f{ -1, 0, 0 }
			, Point2f{ -1, 1 }
			, *m_pipeline
			, m_matrixUbo
			, *m_geometryBuffers );
		DoRender( Position{ p_position.x() + w * 1, p_position.y() + h * 1 }
			, p_size
			, p_texture
			, Point3f{ 0, -1, 0 }
			, Point2f{ 1, 1 }
			, *m_pipeline
			, m_matrixUbo
			, *m_geometryBuffers );
		DoRender( Position{ p_position.x() + w * 2, p_position.y() + h * 1 }
			, p_size
			, p_texture
			, Point3f{ 1, 0, 0 }
			, Point2f{ 1, 1 }
			, *m_pipeline
			, m_matrixUbo
			, *m_geometryBuffers );
		DoRender( Position{ p_position.x() + w * 3, p_position.y() + h * 1 }
			, p_size
			, p_texture
			, Point3f{ 0, 1, 0 }
			, Point2f{ 1, -1 }
			, *m_pipeline
			, m_matrixUbo
			, *m_geometryBuffers );
		DoRender( Position{ p_position.x() + w * 1, p_position.y() + h * 0 }
			, p_size
			, p_texture
			, Point3f{ 0, 0, -1 }
			, Point2f{ 1, 1 }
			, *m_pipeline
			, m_matrixUbo
			, *m_geometryBuffers );
		DoRender( Position{ p_position.x() + w * 1, p_position.y() + h * 2 }
			, p_size
			, p_texture
			, Point3f{ 0, 0, 1 }
			, Point2f{ 1, 1 }
			, *m_pipeline
			, m_matrixUbo
			, *m_geometryBuffers );
	}

	void RenderColourCubeToTexture::DoRender( Position const & p_position
		, Size const & p_size
		, TextureLayout const & p_texture
		, Point3f const & p_face
		, Castor::Point2f const & p_uvMult
		, RenderPipeline & p_pipeline
		, MatrixUbo & p_matrixUbo
		, GeometryBuffers const & p_geometryBuffers )
	{
		REQUIRE( p_texture.GetType() == TextureType::eCube );
		m_viewport.SetPosition( p_position );
		m_viewport.Resize( p_size );
		m_viewport.Update();
		m_viewport.Apply();

		REQUIRE( m_faceUniform );
		m_faceUniform->SetValue( p_face );

		p_matrixUbo.Update( m_viewport.GetProjection() );
		p_pipeline.Apply();

		p_texture.Bind( 0u );
		m_sampler->Bind( 0u );
		p_geometryBuffers.Draw( uint32_t( m_arrayVertex.size() ), 0 );
		m_sampler->Unbind( 0u );
		p_texture.Unbind( 0u );
	}

	ShaderProgramSPtr RenderColourCubeToTexture::DoCreateProgram()
	{
		auto & renderSystem = *GetOwner()->GetRenderSystem();
		GLSL::Shader vtx;
		{
			using namespace GLSL;
			auto writer = renderSystem.CreateGlslWriter();

			UBO_MATRIX( writer );

			// Shader inputs
			auto position = writer.DeclAttribute< Vec2 >( ShaderProgram::Position );
			auto texture = writer.DeclAttribute< Vec2 >( ShaderProgram::Texture );

			// Shader outputs
			auto vtx_texture = writer.DeclOutput< Vec2 >( cuT( "vtx_texture" ) );
			auto gl_Position = writer.DeclBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				vtx_texture = texture;
				gl_Position = c3d_mtxProjection * vec4( position.x(), position.y(), 0.0, 1.0 );
			} );
			vtx = writer.Finalise();
		}

		GLSL::Shader pxl;
		{
			using namespace GLSL;
			auto writer = renderSystem.CreateGlslWriter();

			// Shader inputs
			auto c3d_mapDiffuse = writer.DeclUniform< SamplerCube >( ShaderProgram::MapDiffuse );
			auto c3d_v3Face = writer.DeclUniform< Vec3 >( cuT( "c3d_v3Face" ) );
			auto vtx_texture = writer.DeclInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto plx_v4FragColor = writer.DeclFragData< Vec4 >( cuT( "plx_v4FragColor" ), 0 );

			writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				auto mapCoord = writer.DeclLocale( cuT( "mapCoord" ), vtx_texture * 2.0_f - 1.0_f );
				auto uv = writer.DeclLocale< Vec3 >( cuT( "uv" )
					, writer.Ternary( c3d_v3Face.x() != 0.0_f
						, vec3( c3d_v3Face.x(), mapCoord )
						, writer.Ternary( c3d_v3Face.y() != 0.0_f
							, vec3( mapCoord.x(), c3d_v3Face.y(), mapCoord.y() )
							, vec3( mapCoord, c3d_v3Face.z() ) ) ) );
				plx_v4FragColor = vec4( texture( c3d_mapDiffuse, uv ).xyz(), 1.0 );
			} );
			pxl = writer.Finalise();
		}

		auto & cache = renderSystem.GetEngine()->GetShaderProgramCache();
		auto program = cache.GetNewProgram( false );
		program->CreateObject( ShaderType::eVertex );
		program->CreateObject( ShaderType::ePixel );
		program->SetSource( ShaderType::eVertex, vtx );
		program->SetSource( ShaderType::ePixel, pxl );
		program->CreateUniform< UniformType::eInt >( ShaderProgram::MapDiffuse, ShaderType::ePixel )->SetValue( 0u );
		m_faceUniform = program->CreateUniform< UniformType::eVec3f >( cuT( "c3d_v3Face" ), ShaderType::ePixel );
		program->Initialise();
		return program;
	}
}
