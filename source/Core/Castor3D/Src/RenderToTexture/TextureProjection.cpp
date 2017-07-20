#include "TextureProjection.hpp"

#include "Engine.hpp"

#include "Mesh/Vertex.hpp"
#include "Mesh/Buffer/Buffer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Scene/Camera.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <GlslSource.hpp>

using namespace Castor;

namespace Castor3D
{
	TextureProjection::TextureProjection( Context & p_context )
		: OwnedBy< Context >{ p_context }
		, m_matrixUbo{ *p_context.GetRenderSystem()->GetEngine() }
		, m_modelMatrixUbo{ *p_context.GetRenderSystem()->GetEngine() }
		, m_bufferVertex
		{
			{
				-1, +1, -1,/**/+1, -1, -1,/**/-1, -1, -1,/**/+1, -1, -1,/**/-1, +1, -1,/**/+1, +1, -1,/**/
				-1, -1, +1,/**/-1, +1, -1,/**/-1, -1, -1,/**/-1, +1, -1,/**/-1, -1, +1,/**/-1, +1, +1,/**/
				+1, -1, -1,/**/+1, +1, +1,/**/+1, -1, +1,/**/+1, +1, +1,/**/+1, -1, -1,/**/+1, +1, -1,/**/
				-1, -1, +1,/**/+1, +1, +1,/**/-1, +1, +1,/**/+1, +1, +1,/**/-1, -1, +1,/**/+1, -1, +1,/**/
				-1, +1, -1,/**/+1, +1, +1,/**/+1, +1, -1,/**/+1, +1, +1,/**/-1, +1, -1,/**/-1, +1, +1,/**/
				-1, -1, -1,/**/+1, -1, -1,/**/-1, -1, +1,/**/+1, -1, -1,/**/+1, -1, +1,/**/-1, -1, +1,/**/
			}
		}
		, m_declaration
		{
			{
				BufferElementDeclaration{ ShaderProgram::Position, uint32_t( ElementUsage::ePosition ), ElementType::eVec3 }
			}
		}
	{
		uint32_t i = 0;

		for ( auto & vertex : m_arrayVertex )
		{
			vertex = std::make_shared< BufferElementGroup >( &reinterpret_cast< uint8_t * >( m_bufferVertex.data() )[i++ * m_declaration.stride()] );
		}

		m_sampler = p_context.GetRenderSystem()->GetEngine()->GetSamplerCache().Add( cuT( "TextureProjection" ) );
		m_sampler->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
		m_sampler->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
		m_sampler->SetWrappingMode( TextureUVW::eU, WrapMode::eClampToEdge );
		m_sampler->SetWrappingMode( TextureUVW::eV, WrapMode::eClampToEdge );
		m_sampler->SetWrappingMode( TextureUVW::eW, WrapMode::eClampToEdge );
	}

	TextureProjection::~TextureProjection()
	{
		for ( auto & vertex : m_arrayVertex )
		{
			vertex.reset();
		}
	}

	void TextureProjection::Initialise()
	{
		m_sampler->Initialise();
		auto & program = DoInitialiseShader();
		DoInitialiseVertexBuffer();
		DoInitialisePipeline( program );
	}

	void TextureProjection::Cleanup()
	{
		m_matrixUbo.GetUbo().Cleanup();
		m_modelMatrixUbo.GetUbo().Cleanup();
		m_sampler.reset();
		m_pipeline->Cleanup();
		m_pipeline.reset();
		m_vertexBuffer->Cleanup();
		m_vertexBuffer.reset();
		m_geometryBuffers->Cleanup();
		m_geometryBuffers.reset();
	}

	void TextureProjection::Render( TextureLayout const & p_texture
		, Camera const & p_camera )
	{
		static Matrix4x4r const Identity{ 1.0f };
		auto node = p_camera.GetParent();
		matrix::set_translate( m_mtxModel, node->GetDerivedPosition() );
		m_matrixUbo.Update( p_camera.GetView()
			, p_camera.GetViewport().GetProjection() );
		m_modelMatrixUbo.Update( m_mtxModel, Identity );
		m_sizeUniform->SetValue( Point2f{ p_camera.GetViewport().GetWidth()
			, p_camera.GetViewport().GetHeight() } );
		p_camera.Apply();
		m_pipeline->Apply();
		p_texture.Bind( 0 );
		m_sampler->Bind( 0 );
		m_geometryBuffers->Draw( uint32_t( m_arrayVertex.size() ), 0 );
		m_sampler->Unbind( 0 );
		p_texture.Unbind( 0 );
	}

	ShaderProgram & TextureProjection::DoInitialiseShader()
	{
		auto & renderSystem = *GetOwner()->GetRenderSystem();
		GLSL::Shader vtx;
		{
			using namespace GLSL;
			GlslWriter writer{ renderSystem.CreateGlslWriter() };

			// Inputs
			auto position = writer.DeclAttribute< Vec3 >( ShaderProgram::Position );
			UBO_MATRIX( writer );
			UBO_MODEL_MATRIX( writer );

			// Outputs
			auto gl_Position = writer.DeclBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				gl_Position = writer.Paren( c3d_mtxProjection * c3d_mtxView * c3d_mtxModel * vec4( position, 1.0 ) ).SWIZZLE_XYWW;
			} );

			vtx = writer.Finalise();
		}

		GLSL::Shader pxl;
		{
			using namespace GLSL;
			GlslWriter writer{ renderSystem.CreateGlslWriter() };

			// Inputs
			auto c3d_mapDiffuse = writer.DeclUniform< Sampler2D >( ShaderProgram::MapDiffuse );
			auto c3d_size = writer.DeclUniform< Vec2 >( cuT( "c3d_size" ) );
			auto gl_FragCoord = writer.DeclBuiltin< Vec4 >( cuT( "gl_FragCoord" ) );

			// Outputs
			auto pxl_FragColor = writer.DeclOutput< Vec4 >( cuT( "pxl_FragColor" ) );

			writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				pxl_FragColor = texture( c3d_mapDiffuse, gl_FragCoord.xy() / c3d_size );
			} );

			pxl = writer.Finalise();
		}

		auto & cache = renderSystem.GetEngine()->GetShaderProgramCache();
		auto program = cache.GetNewProgram( false );
		program->CreateObject( ShaderType::eVertex );
		program->CreateObject( ShaderType::ePixel );
		program->SetSource( ShaderType::eVertex, vtx );
		program->SetSource( ShaderType::ePixel, pxl );
		program->CreateUniform< UniformType::eInt >( ShaderProgram::MapDiffuse, ShaderType::ePixel )->SetValue( 0 );
		m_sizeUniform = program->CreateUniform< UniformType::eVec2f >( cuT( "c3d_size" ), ShaderType::ePixel );
		program->Initialise();
		return *program;
	}

	bool TextureProjection::DoInitialiseVertexBuffer()
	{
		m_vertexBuffer = std::make_shared< VertexBuffer >( *GetOwner()->GetRenderSystem()->GetEngine(), m_declaration );
		m_vertexBuffer->Resize( uint32_t( m_arrayVertex.size() * m_declaration.stride() ) );
		m_vertexBuffer->LinkCoords( m_arrayVertex.begin(), m_arrayVertex.end() );
		return m_vertexBuffer->Initialise( BufferAccessType::eStatic, BufferAccessNature::eDraw );
	}

	bool TextureProjection::DoInitialisePipeline( ShaderProgram & p_program )
	{
		DepthStencilState dsState;
		dsState.SetDepthTest( true );
		dsState.SetDepthMask( WritingMask::eZero );
		dsState.SetDepthFunc( DepthFunc::eLEqual );

		RasteriserState rsState;
		rsState.SetCulledFaces( Culling::eFront );

		m_pipeline = GetOwner()->GetRenderSystem()->CreateRenderPipeline( std::move( dsState )
			, std::move( rsState )
			, BlendState{}
			, MultisampleState{}
			, p_program
			, PipelineFlags{} );
		m_pipeline->AddUniformBuffer( m_matrixUbo.GetUbo() );
		m_pipeline->AddUniformBuffer( m_modelMatrixUbo.GetUbo() );
		m_geometryBuffers = GetOwner()->GetRenderSystem()->CreateGeometryBuffers( Topology::eTriangles
			, m_pipeline->GetProgram() );
		return m_geometryBuffers->Initialise( { *m_vertexBuffer }, nullptr );
	}
}
