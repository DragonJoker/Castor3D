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

		for ( auto & vertex : m_arrayVertex )
		{
			vertex = std::make_shared< BufferElementGroup >( &reinterpret_cast< uint8_t * >( m_bufferVertex.data() )[i++ * m_declaration.stride()] );
		}
	}

	RenderColourLayerToTexture::~RenderColourLayerToTexture()
	{
		for ( auto & vertex : m_arrayVertex )
		{
			vertex.reset();
		}
	}

	void RenderColourLayerToTexture::Initialise()
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
			auto c3d_mapDiffuse = writer.DeclUniform< Sampler2DArray >( ShaderProgram::MapDiffuse );
			auto c3d_iIndex = writer.DeclUniform< Int >( cuT( "c3d_iIndex" ) );
			auto vtx_texture = writer.DeclInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto plx_v4FragColor = writer.DeclFragData< Vec4 >( cuT( "plx_v4FragColor" ), 0 );

			writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				plx_v4FragColor = vec4( texture( c3d_mapDiffuse, vec3( vtx_texture, c3d_iIndex ) ).xyz(), 1.0 );
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
		m_layerIndexUniform = program->CreateUniform< UniformType::eInt >( cuT( "c3d_iIndex" ), ShaderType::ePixel );
		program->Initialise();
		return program;
	}
}
