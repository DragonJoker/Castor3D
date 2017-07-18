#include "RenderColourToTexture.hpp"

#include "Engine.hpp"

#include "Mesh/Vertex.hpp"
#include "Mesh/Buffer/Buffer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <GlslSource.hpp>

using namespace Castor;

namespace Castor3D
{
	RenderColourToTexture::RenderColourToTexture( Context & p_context
		, MatrixUbo & p_matrixUbo
		, bool p_invertU )
		: OwnedBy< Context >{ p_context }
		, m_matrixUbo{ p_matrixUbo }
		, m_viewport{ *p_context.GetRenderSystem()->GetEngine() }
		, m_bufferVertex
		{
			{
				0, 0, ( p_invertU ? 1.0f : 0.0f ), 0,
				1, 1, ( p_invertU ? 0.0f : 1.0f ), 1,
				0, 1, ( p_invertU ? 1.0f : 0.0f ), 1,
				0, 0, ( p_invertU ? 1.0f : 0.0f ), 0,
				1, 0, ( p_invertU ? 0.0f : 1.0f ), 0,
				1, 1, ( p_invertU ? 0.0f : 1.0f ), 1
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

	RenderColourToTexture::~RenderColourToTexture()
	{
		for ( auto & vertex : m_arrayVertex )
		{
			vertex.reset();
		}
	}

	void RenderColourToTexture::Initialise()
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
		dsState.SetDepthMask( WritingMask::eZero );
		m_pipeline = renderSystem.CreateRenderPipeline( std::move( dsState )
			, RasteriserState{}
			, BlendState{}
			, MultisampleState{}
			, program
			, PipelineFlags{} );
		m_pipeline->AddUniformBuffer( m_matrixUbo.GetUbo() );

		m_sampler = GetOwner()->GetRenderSystem()->GetEngine()->GetSamplerCache().Add( cuT( "RenderColourToTexture" ) );
		m_sampler->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
		m_sampler->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
		m_sampler->SetWrappingMode( TextureUVW::eU, WrapMode::eClampToEdge );
		m_sampler->SetWrappingMode( TextureUVW::eV, WrapMode::eClampToEdge );
		m_sampler->SetWrappingMode( TextureUVW::eW, WrapMode::eClampToEdge );
		m_viewport.Update();
	}

	void RenderColourToTexture::Cleanup()
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

	void RenderColourToTexture::Render( Position const & p_position
		, Size const & p_size
		, TextureLayout const & p_texture
		, MatrixUbo & p_matrixUbo
		, RenderPipeline & p_pipeline )
	{
		DoRender( p_position
			, p_size
			, p_texture
			, p_pipeline
			, p_matrixUbo
			, *m_geometryBuffers );
	}

	void RenderColourToTexture::Render( Position const & p_position
		, Size const & p_size
		, TextureLayout const & p_texture )
	{
		DoRender( p_position
			, p_size
			, p_texture
			, *m_pipeline
			, m_matrixUbo
			, *m_geometryBuffers );
	}

	void RenderColourToTexture::DoRender( Position const & p_position
		, Size const & p_size
		, TextureLayout const & p_texture
		, RenderPipeline & p_pipeline
		, MatrixUbo & p_matrixUbo
		, GeometryBuffers const & p_geometryBuffers )
	{
		m_viewport.SetPosition( p_position );
		m_viewport.Resize( p_size );
		m_viewport.Apply();

		p_matrixUbo.Update( m_viewport.GetProjection() );
		p_pipeline.Apply();

		p_texture.Bind( 0u );
		m_sampler->Bind( 0u );
		p_geometryBuffers.Draw( uint32_t( m_arrayVertex.size() ), 0 );
		m_sampler->Unbind( 0u );
		p_texture.Unbind( 0u );
	}

	ShaderProgramSPtr RenderColourToTexture::DoCreateProgram()
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
			auto c3d_mapDiffuse = writer.DeclUniform< Sampler2D >( ShaderProgram::MapDiffuse );
			auto vtx_texture = writer.DeclInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto plx_v4FragColor = writer.DeclFragData< Vec4 >( cuT( "plx_v4FragColor" ), 0 );

			writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				plx_v4FragColor = vec4( texture( c3d_mapDiffuse, vec2( vtx_texture.x(), vtx_texture.y() ) ).xyz(), 1.0 );
			} );
			pxl = writer.Finalise();
		}

		auto & cache = renderSystem.GetEngine()->GetShaderProgramCache();
		auto program = cache.GetNewProgram( false );
		program->CreateObject( ShaderType::eVertex );
		program->CreateObject( ShaderType::ePixel );
		program->SetSource( ShaderType::eVertex, vtx );
		program->SetSource( ShaderType::ePixel, pxl );
		program->CreateUniform< UniformType::eInt >( ShaderProgram::MapDiffuse, ShaderType::ePixel );
		program->Initialise();
		return program;
	}
}
