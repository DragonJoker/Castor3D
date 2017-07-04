#include "RenderColourLayerToTexture.hpp"

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
	RenderColourLayerToTexture::RenderColourLayerToTexture( Context & p_context
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

		for ( auto & l_vertex : m_arrayVertex )
		{
			l_vertex = std::make_shared< BufferElementGroup >( &reinterpret_cast< uint8_t * >( m_bufferVertex.data() )[i++ * m_declaration.stride()] );
		}
	}

	RenderColourLayerToTexture::~RenderColourLayerToTexture()
	{
		for ( auto & l_vertex : m_arrayVertex )
		{
			l_vertex.reset();
		}
	}

	void RenderColourLayerToTexture::Initialise()
	{
		m_viewport.Initialise();
		auto & l_program = *DoCreateProgram();
		auto & l_renderSystem = *GetOwner()->GetRenderSystem();
		l_program.Initialise();
		m_vertexBuffer = std::make_shared< VertexBuffer >( *l_renderSystem.GetEngine()
			, m_declaration );
		m_vertexBuffer->Resize( uint32_t( m_arrayVertex.size()
			* m_declaration.stride() ) );
		m_vertexBuffer->LinkCoords( m_arrayVertex.begin()
			, m_arrayVertex.end() );
		m_vertexBuffer->Initialise( BufferAccessType::eStatic
			, BufferAccessNature::eDraw );
		m_geometryBuffers = l_renderSystem.CreateGeometryBuffers( Topology::eTriangles
			, l_program );
		m_geometryBuffers->Initialise( { *m_vertexBuffer }
		, nullptr );

		DepthStencilState l_dsState;
		l_dsState.SetDepthTest( false );
		m_pipeline = l_renderSystem.CreateRenderPipeline( std::move( l_dsState )
			, RasteriserState{}
			, BlendState{}
			, MultisampleState{}
			, l_program
			, PipelineFlags{} );
		m_pipeline->AddUniformBuffer( m_matrixUbo.GetUbo() );

		m_sampler = GetOwner()->GetRenderSystem()->GetEngine()->GetSamplerCache().Add( cuT( "RenderColourLayerToTexture" ) );
		m_sampler->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
		m_sampler->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
		m_sampler->SetWrappingMode( TextureUVW::eU, WrapMode::eClampToEdge );
		m_sampler->SetWrappingMode( TextureUVW::eV, WrapMode::eClampToEdge );
		m_sampler->SetWrappingMode( TextureUVW::eW, WrapMode::eClampToEdge );
	}

	void RenderColourLayerToTexture::Cleanup()
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

	void RenderColourLayerToTexture::Render( Position const & p_position
		, Size const & p_size
		, TextureLayout const & p_texture
		, MatrixUbo & p_matrixUbo
		, RenderPipeline & p_pipeline
		, uint32_t p_layer )
	{
		DoRender( p_position
			, p_size
			, p_texture
			, p_pipeline
			, p_matrixUbo
			, *m_geometryBuffers
			, p_layer );
	}

	void RenderColourLayerToTexture::Render( Position const & p_position
		, Size const & p_size
		, TextureLayout const & p_texture
		, uint32_t p_layer )
	{
		DoRender( p_position
			, p_size
			, p_texture
			, *m_pipeline
			, m_matrixUbo
			, *m_geometryBuffers
			, p_layer );
	}

	void RenderColourLayerToTexture::DoRender( Position const & p_position
		, Size const & p_size
		, TextureLayout const & p_texture
		, RenderPipeline & p_pipeline
		, MatrixUbo & p_matrixUbo
		, GeometryBuffers const & p_geometryBuffers
		, uint32_t p_layer )
	{
		REQUIRE( p_texture.GetLayersCount() > p_layer );
		m_viewport.SetPosition( p_position );
		m_viewport.Resize( p_size );
		m_viewport.Update();
		m_viewport.Apply();

		REQUIRE( m_layerIndexUniform );
		m_layerIndexUniform->SetValue( p_layer );

		p_matrixUbo.Update( m_viewport.GetProjection() );
		p_pipeline.Apply();
		m_layerIndexUniform->Update();

		p_texture.Bind( 0u );
		m_sampler->Bind( 0u );
		p_geometryBuffers.Draw( uint32_t( m_arrayVertex.size() ), 0 );
		m_sampler->Unbind( 0u );
		p_texture.Unbind( 0u );
	}

	ShaderProgramSPtr RenderColourLayerToTexture::DoCreateProgram()
	{
		auto & l_renderSystem = *GetOwner()->GetRenderSystem();
		GLSL::Shader l_vtx;
		{
			using namespace GLSL;
			auto l_writer = l_renderSystem.CreateGlslWriter();

			UBO_MATRIX( l_writer );

			// Shader inputs
			auto position = l_writer.DeclAttribute< Vec2 >( ShaderProgram::Position );
			auto texture = l_writer.DeclAttribute< Vec2 >( ShaderProgram::Texture );

			// Shader outputs
			auto vtx_texture = l_writer.DeclOutput< Vec2 >( cuT( "vtx_texture" ) );
			auto gl_Position = l_writer.DeclBuiltin< Vec4 >( cuT( "gl_Position" ) );

			l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				vtx_texture = texture;
				gl_Position = c3d_mtxProjection * vec4( position.x(), position.y(), 0.0, 1.0 );
			} );
			l_vtx = l_writer.Finalise();
		}

		GLSL::Shader l_pxl;
		{
			using namespace GLSL;
			auto l_writer = l_renderSystem.CreateGlslWriter();

			// Shader inputs
			auto c3d_mapDiffuse = l_writer.DeclUniform< Sampler2DArray >( ShaderProgram::MapDiffuse );
			auto c3d_iIndex = l_writer.DeclUniform< Int >( cuT( "c3d_iIndex" ) );
			auto vtx_texture = l_writer.DeclInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto plx_v4FragColor = l_writer.DeclFragData< Vec4 >( cuT( "plx_v4FragColor" ), 0 );

			l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				plx_v4FragColor = vec4( texture( c3d_mapDiffuse, vec3( vtx_texture, c3d_iIndex ) ).xyz(), 1.0 );
			} );
			l_pxl = l_writer.Finalise();
		}

		auto & l_cache = l_renderSystem.GetEngine()->GetShaderProgramCache();
		auto l_program = l_cache.GetNewProgram( false );
		l_program->CreateObject( ShaderType::eVertex );
		l_program->CreateObject( ShaderType::ePixel );
		l_program->SetSource( ShaderType::eVertex, l_vtx );
		l_program->SetSource( ShaderType::ePixel, l_pxl );
		l_program->CreateUniform< UniformType::eInt >( ShaderProgram::MapDiffuse, ShaderType::ePixel );
		m_layerIndexUniform = l_program->CreateUniform< UniformType::eInt >( cuT( "c3d_iIndex" ), ShaderType::ePixel );
		l_program->Initialise();
		return l_program;
	}
}
