#include "RenderColourToCube.hpp"

#include "Engine.hpp"
#include "Cache/ShaderCache.hpp"

#include "Render/RenderPipeline.hpp"
#include "Render/RenderSystem.hpp"

#include "FrameBuffer/DepthStencilRenderBuffer.hpp"
#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/RenderBufferAttachment.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "Mesh/Vertex.hpp"
#include "Mesh/Buffer/Buffer.hpp"
#include "Scene/Skybox.hpp"
#include "Shader/UniformBuffer.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <GlslSource.hpp>

using namespace Castor;

namespace Castor3D
{
	RenderColourToCube::RenderColourToCube( Context & p_context
		, UniformBuffer & p_matrixUbo )
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

		for ( auto & l_vertex : m_arrayVertex )
		{
			l_vertex = std::make_shared< BufferElementGroup >( &reinterpret_cast< uint8_t * >( m_bufferVertex.data() )[i++ * m_declaration.stride()] );
		}
	}

	RenderColourToCube::~RenderColourToCube()
	{
		for ( auto & l_vertex : m_arrayVertex )
		{
			l_vertex.reset();
		}
	}

	void RenderColourToCube::Initialise()
	{
		m_viewport.Initialise();
		auto & l_program = *DoCreateProgram();
		auto & l_renderSystem = *GetOwner()->GetRenderSystem();
		l_program.Initialise();
		m_vertexBuffer = std::make_shared< VertexBuffer >( *l_renderSystem.GetEngine()
			, m_declaration );
		m_vertexBuffer->Resize( uint32_t( m_arrayVertex.size()
			* m_declaration.stride() ) );
		m_vertexBuffer->LinkCoords( m_arrayVertex.begin(),
			m_arrayVertex.end() );
		m_vertexBuffer->Initialise( BufferAccessType::eStatic
			, BufferAccessNature::eDraw );
		m_geometryBuffers = l_renderSystem.CreateGeometryBuffers( Topology::eTriangles
			, l_program );
		m_geometryBuffers->Initialise( { *m_vertexBuffer }
			, nullptr );

		DepthStencilState l_dsState;
		l_dsState.SetDepthFunc( DepthFunc::eLEqual );
		l_dsState.SetDepthTest( false );
		l_dsState.SetDepthMask( WritingMask::eAll );

		RasteriserState l_rsState;
		l_rsState.SetCulledFaces( Culling::eFront );

		m_pipeline = l_renderSystem.CreateRenderPipeline( std::move( l_dsState )
			, std::move( l_rsState )
			, BlendState{}
			, MultisampleState{}
			, l_program
			, PipelineFlags{} );
		m_pipeline->AddUniformBuffer( m_matrixUbo );

		m_sampler = l_renderSystem.GetEngine()->GetSamplerCache().Add( cuT( "RenderColourToCube" ) );
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
		static Matrix4x4r const l_projection = matrix::perspective( Angle::from_degrees( 90.0_r ), 1.0_r, 0.1_r, 10.0_r );
		static Matrix4x4r const l_views[] =
		{
			matrix::look_at( Point3r{ 0.0f, 0.0f, 0.0f }, Point3r{ +1.0f, +0.0f, +0.0f }, Point3r{ 0.0f, -1.0f, +0.0f } ),
			matrix::look_at( Point3r{ 0.0f, 0.0f, 0.0f }, Point3r{ -1.0f, +0.0f, +0.0f }, Point3r{ 0.0f, -1.0f, +0.0f } ),
			matrix::look_at( Point3r{ 0.0f, 0.0f, 0.0f }, Point3r{ +0.0f, +1.0f, +0.0f }, Point3r{ 0.0f, +0.0f, +1.0f } ),
			matrix::look_at( Point3r{ 0.0f, 0.0f, 0.0f }, Point3r{ +0.0f, -1.0f, +0.0f }, Point3r{ 0.0f, +0.0f, -1.0f } ),
			matrix::look_at( Point3r{ 0.0f, 0.0f, 0.0f }, Point3r{ +0.0f, +0.0f, +1.0f }, Point3r{ 0.0f, -1.0f, +0.0f } ),
			matrix::look_at( Point3r{ 0.0f, 0.0f, 0.0f }, Point3r{ +0.0f, +0.0f, -1.0f }, Point3r{ 0.0f, -1.0f, +0.0f } )
		};

		m_pipeline->SetProjectionMatrix( l_projection );
		m_viewport.Resize( p_size );
		m_viewport.Apply();
		p_fbo->Bind( FrameBufferTarget::eDraw );
		p_fbo->Clear( BufferComponent::eColour | BufferComponent::eDepth );
		p_2dTexture.Bind( 0u );

		for ( uint32_t i = 0; i < 6; ++i )
		{
			m_pipeline->SetViewMatrix( l_views[i] );
			p_attachs[i]->Attach( AttachmentPoint::eColour, 0u );
			p_fbo->SetDrawBuffer( p_attachs[i] );
			m_pipeline->ApplyMatrices( m_matrixUbo, ~0u );
			m_matrixUbo.Update();
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
		auto & l_renderSystem = *GetOwner()->GetRenderSystem();
		String l_vtx;
		{
			using namespace GLSL;
			GlslWriter l_writer{ l_renderSystem.CreateGlslWriter() };

			// Inputs
			auto position = l_writer.GetAttribute< Vec3 >( ShaderProgram::Position );
			UBO_MATRIX( l_writer );

			// Outputs
			auto vtx_position = l_writer.GetOutput< Vec3 >( cuT( "vtx_position" ) );
			auto gl_Position = l_writer.GetBuiltin< Vec4 >( cuT( "gl_Position" ) );

			l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				vtx_position = position;
				gl_Position = l_writer.Paren( c3d_mtxProjection * c3d_mtxView * vec4( vtx_position, 1.0 ) );
			} );

			l_vtx = l_writer.Finalise();
		}

		String l_pxl;
		{
			using namespace GLSL;
			GlslWriter l_writer{ l_renderSystem.CreateGlslWriter() };

			// Inputs
			auto vtx_position = l_writer.GetInput< Vec3 >( cuT( "vtx_position" ) );
			auto c3d_mapDiffuse = l_writer.GetUniform< Sampler2D >( ShaderProgram::MapDiffuse );

			// Outputs
			auto plx_v4FragColor = l_writer.GetOutput< Vec4 >( cuT( "pxl_FragColor" ) );

			auto l_sampleSphericalMap = l_writer.ImplementFunction< Vec2 >( cuT( "SampleSphericalMap" ), [&]( Vec3 const & v )
			{
				auto uv = l_writer.GetLocale( cuT( "uv" ), vec2( atan( v.z(), v.x() ), asin( v.y() ) ) );
				uv *= vec2( 0.1591_f, 0.3183_f );
				uv += 0.5_f;
				l_writer.Return( uv );
			}, InParam< Vec3 >( &l_writer, cuT( "v" ) ) );

			l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				auto uv = l_writer.GetLocale( cuT( "uv" ), l_sampleSphericalMap( normalize( vtx_position ) ) );
				plx_v4FragColor = vec4( texture( c3d_mapDiffuse, vec2( uv.x(), 1.0_r - uv.y() ) ).rgb(), 1.0_f );
			} );

			l_pxl = l_writer.Finalise();
		}

		auto l_model = l_renderSystem.GetGpuInformations().GetMaxShaderModel();
		auto & l_cache = l_renderSystem.GetEngine()->GetShaderProgramCache();
		auto l_program = l_cache.GetNewProgram( false );
		l_program->CreateObject( ShaderType::eVertex );
		l_program->CreateObject( ShaderType::ePixel );
		l_program->SetSource( ShaderType::eVertex, l_model, l_vtx );
		l_program->SetSource( ShaderType::ePixel, l_model, l_pxl );
		l_program->CreateUniform< UniformType::eInt >( ShaderProgram::MapDiffuse, ShaderType::ePixel );
		l_program->Initialise();
		return l_program;
	}
}
