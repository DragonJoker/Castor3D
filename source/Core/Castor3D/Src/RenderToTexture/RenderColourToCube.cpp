#include "RenderColourToCube.hpp"

#include "Engine.hpp"

#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/RenderBufferAttachment.hpp"
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
	RenderColourToCube::RenderColourToCube( Context & p_context
		, MatrixUbo & p_matrixUbo )
		: OwnedBy< Context >{ p_context }
		, m_matrixUbo{ p_matrixUbo }
		, m_viewport{ *p_context.GetRenderSystem()->GetEngine() }
		, m_bufferVertex
		{
			{
				-1, +1, -1, /**/+1, -1, -1, /**/-1, -1, -1, /**/+1, -1, -1, /**/-1, +1, -1, /**/+1, +1, -1,
				-1, -1, +1, /**/-1, +1, -1, /**/-1, -1, -1, /**/-1, +1, -1, /**/-1, -1, +1, /**/-1, +1, +1,
				+1, -1, -1, /**/+1, +1, +1, /**/+1, -1, +1, /**/+1, +1, +1, /**/+1, -1, -1, /**/+1, +1, -1,
				-1, -1, +1, /**/+1, +1, +1, /**/-1, +1, +1, /**/+1, +1, +1, /**/-1, -1, +1, /**/+1, -1, +1,
				-1, +1, -1, /**/+1, +1, +1, /**/+1, +1, -1, /**/+1, +1, +1, /**/-1, +1, -1, /**/-1, +1, +1,
				-1, -1, -1, /**/+1, -1, -1, /**/-1, -1, +1, /**/+1, -1, -1, /**/+1, -1, +1, /**/-1, -1, +1,
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
	}

	RenderColourToCube::~RenderColourToCube()
	{
		for ( auto & vertex : m_arrayVertex )
		{
			vertex.reset();
		}
	}

	void RenderColourToCube::Initialise()
	{
		m_viewport.Initialise();
		auto & program = *DoCreateProgram();
		auto & renderSystem = *GetOwner()->GetRenderSystem();
		program.Initialise();
		m_vertexBuffer = std::make_shared< VertexBuffer >( *renderSystem.GetEngine()
			, m_declaration );
		m_vertexBuffer->Resize( uint32_t( m_arrayVertex.size()
			* m_declaration.stride() ) );
		m_vertexBuffer->LinkCoords( m_arrayVertex.begin(),
			m_arrayVertex.end() );
		m_vertexBuffer->Initialise( BufferAccessType::eStatic
			, BufferAccessNature::eDraw );
		m_geometryBuffers = renderSystem.CreateGeometryBuffers( Topology::eTriangles
			, program );
		m_geometryBuffers->Initialise( { *m_vertexBuffer }
			, nullptr );

		DepthStencilState dsState;
		dsState.SetDepthFunc( DepthFunc::eLEqual );
		dsState.SetDepthTest( false );
		dsState.SetDepthMask( WritingMask::eAll );

		RasteriserState rsState;
		rsState.SetCulledFaces( Culling::eFront );

		m_pipeline = renderSystem.CreateRenderPipeline( std::move( dsState )
			, std::move( rsState )
			, BlendState{}
			, MultisampleState{}
			, program
			, PipelineFlags{} );
		m_pipeline->AddUniformBuffer( m_matrixUbo.GetUbo() );

		m_sampler = renderSystem.GetEngine()->GetSamplerCache().Add( cuT( "RenderColourToCube" ) );
		m_sampler->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
		m_sampler->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
		m_sampler->SetWrappingMode( TextureUVW::eU, WrapMode::eClampToEdge );
		m_sampler->SetWrappingMode( TextureUVW::eV, WrapMode::eClampToEdge );
		m_sampler->SetWrappingMode( TextureUVW::eW, WrapMode::eClampToEdge );
	}

	void RenderColourToCube::Cleanup()
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

	void RenderColourToCube::Render( Size const & p_size
		, TextureLayout const & p_2dTexture
		, TextureLayoutSPtr p_cubeTexture
		, FrameBufferSPtr p_fbo
		, std::array< FrameBufferAttachmentSPtr, 6 > const & p_attachs )
	{
		m_sampler->Initialise();
		static Matrix4x4r const projection = matrix::perspective( Angle::from_degrees( 90.0_r ), 1.0_r, 0.1_r, 10.0_r );
		static Matrix4x4r const views[] =
		{
			matrix::look_at( Point3r{ 0.0f, 0.0f, 0.0f }, Point3r{ +1.0f, +0.0f, +0.0f }, Point3r{ 0.0f, -1.0f, +0.0f } ),
			matrix::look_at( Point3r{ 0.0f, 0.0f, 0.0f }, Point3r{ -1.0f, +0.0f, +0.0f }, Point3r{ 0.0f, -1.0f, +0.0f } ),
			matrix::look_at( Point3r{ 0.0f, 0.0f, 0.0f }, Point3r{ +0.0f, +1.0f, +0.0f }, Point3r{ 0.0f, +0.0f, +1.0f } ),
			matrix::look_at( Point3r{ 0.0f, 0.0f, 0.0f }, Point3r{ +0.0f, -1.0f, +0.0f }, Point3r{ 0.0f, +0.0f, -1.0f } ),
			matrix::look_at( Point3r{ 0.0f, 0.0f, 0.0f }, Point3r{ +0.0f, +0.0f, +1.0f }, Point3r{ 0.0f, -1.0f, +0.0f } ),
			matrix::look_at( Point3r{ 0.0f, 0.0f, 0.0f }, Point3r{ +0.0f, +0.0f, -1.0f }, Point3r{ 0.0f, -1.0f, +0.0f } )
		};

		m_viewport.Resize( p_size );
		m_viewport.Apply();
		p_fbo->Bind( FrameBufferTarget::eDraw );
		p_fbo->Clear( BufferComponent::eColour | BufferComponent::eDepth );
		p_2dTexture.Bind( 0u );

		for ( uint32_t i = 0; i < 6; ++i )
		{
			p_attachs[i]->Attach( AttachmentPoint::eColour, 0u );
			p_fbo->SetDrawBuffer( p_attachs[i] );
			m_matrixUbo.Update( views[i], projection );
			m_pipeline->Apply();

			m_sampler->Bind( 0u );
			m_geometryBuffers->Draw( uint32_t( m_arrayVertex.size() ), 0u );
			m_sampler->Unbind( 0u );
		}

		p_2dTexture.Unbind( 0u );
		p_fbo->Unbind();
	}

	ShaderProgramSPtr RenderColourToCube::DoCreateProgram()
	{
		auto & renderSystem = *GetOwner()->GetRenderSystem();
		GLSL::Shader vtx;
		{
			using namespace GLSL;
			GlslWriter writer{ renderSystem.CreateGlslWriter() };

			// Inputs
			auto position = writer.DeclAttribute< Vec3 >( ShaderProgram::Position );
			UBO_MATRIX( writer );

			// Outputs
			auto vtx_position = writer.DeclOutput< Vec3 >( cuT( "vtx_position" ) );
			auto gl_Position = writer.DeclBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				vtx_position = position;
				gl_Position = writer.Paren( c3d_mtxProjection * c3d_mtxView * vec4( vtx_position, 1.0 ) );
			} );

			vtx = writer.Finalise();
		}

		GLSL::Shader pxl;
		{
			using namespace GLSL;
			GlslWriter writer{ renderSystem.CreateGlslWriter() };

			// Inputs
			auto vtx_position = writer.DeclInput< Vec3 >( cuT( "vtx_position" ) );
			auto c3d_mapDiffuse = writer.DeclUniform< Sampler2D >( ShaderProgram::MapDiffuse );

			// Outputs
			auto plx_v4FragColor = writer.DeclOutput< Vec4 >( cuT( "pxl_FragColor" ) );

			auto sampleSphericalMap = writer.ImplementFunction< Vec2 >( cuT( "SampleSphericalMap" ), [&]( Vec3 const & v )
			{
				auto uv = writer.DeclLocale( cuT( "uv" ), vec2( atan( v.z(), v.x() ), asin( v.y() ) ) );
				uv *= vec2( 0.1591_f, 0.3183_f );
				uv += 0.5_f;
				writer.Return( uv );
			}, InVec3( &writer, cuT( "v" ) ) );

			writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				auto uv = writer.DeclLocale( cuT( "uv" ), sampleSphericalMap( normalize( vtx_position ) ) );
				plx_v4FragColor = vec4( texture( c3d_mapDiffuse, vec2( uv.x(), 1.0_r - uv.y() ) ).rgb(), 1.0_f );
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
