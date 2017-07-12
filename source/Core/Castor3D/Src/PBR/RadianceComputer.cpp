#include "RadianceComputer.hpp"

#include "Engine.hpp"

#include "FrameBuffer/DepthStencilRenderBuffer.hpp"
#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/RenderBufferAttachment.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "Mesh/Vertex.hpp"
#include "Mesh/Buffer/Buffer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <GlslSource.hpp>
#include <GlslUtils.hpp>

using namespace Castor;

namespace Castor3D
{
	RadianceComputer::RadianceComputer( Engine & p_engine
		, Castor::Size const & p_size )
		: OwnedBy< Engine >{ p_engine }
		, m_matrixUbo{ p_engine }
		, m_viewport{ p_engine }
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
		, m_size{ p_size }
	{
		uint32_t i = 0;

		for ( auto & l_vertex : m_arrayVertex )
		{
			l_vertex = std::make_shared< BufferElementGroup >( &reinterpret_cast< uint8_t * >( m_bufferVertex.data() )[i++ * m_declaration.stride()] );
		}

		m_viewport.Initialise();
		m_viewport.SetPerspective( Angle::from_degrees( 90.0_r ), 1.0_r, 0.1_r, 10.0_r );
		m_viewport.Resize( m_size );
		m_viewport.Update();
		auto & l_program = *DoCreateProgram();
		auto & l_renderSystem = *GetEngine()->GetRenderSystem();
		m_vertexBuffer = std::make_shared< VertexBuffer >( *GetEngine()
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
		m_pipeline->AddUniformBuffer( m_matrixUbo.GetUbo() );

		m_frameBuffer = l_renderSystem.CreateFrameBuffer();

		m_depthBuffer = m_frameBuffer->CreateDepthStencilRenderBuffer( PixelFormat::eD24 );
		m_depthBuffer->Create();
		m_depthBuffer->Initialise( m_size );
		m_depthAttach = m_frameBuffer->CreateAttachment( m_depthBuffer );

		m_frameBuffer->Create();
		m_frameBuffer->Initialise( m_size );
		m_frameBuffer->Bind();
		m_frameBuffer->Attach( AttachmentPoint::eDepth, m_depthAttach );
		REQUIRE( m_frameBuffer->IsComplete() );
		m_frameBuffer->Unbind();

		if ( !GetEngine()->GetSamplerCache().Has( cuT( "RadianceComputer" ) ) )
		{
			m_sampler = GetEngine()->GetSamplerCache().Add( cuT( "RadianceComputer" ) );
			m_sampler->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
			m_sampler->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
			m_sampler->SetWrappingMode( TextureUVW::eU, WrapMode::eClampToEdge );
			m_sampler->SetWrappingMode( TextureUVW::eV, WrapMode::eClampToEdge );
			m_sampler->SetWrappingMode( TextureUVW::eW, WrapMode::eClampToEdge );
			m_sampler->Initialise();
		}
		else
		{
			m_sampler = GetEngine()->GetSamplerCache().Find( cuT( "RadianceComputer" ) );
		}
	}

	RadianceComputer::~RadianceComputer()
	{
		m_sampler.reset();
		m_frameBuffer->Bind();
		m_frameBuffer->DetachAll();
		m_frameBuffer->Unbind();
		m_frameBuffer->Cleanup();
		m_frameBuffer->Destroy();
		m_frameBuffer.reset();
		m_depthAttach.reset();
		m_depthBuffer->Cleanup();
		m_depthBuffer->Destroy();
		m_depthBuffer.reset();
		m_pipeline->Cleanup();
		m_pipeline.reset();
		m_vertexBuffer->Cleanup();
		m_vertexBuffer.reset();
		m_geometryBuffers->Cleanup();
		m_geometryBuffers.reset();
		m_viewport.Cleanup();
		m_matrixUbo.GetUbo().Cleanup();

		for ( auto & l_vertex : m_arrayVertex )
		{
			l_vertex.reset();
		}
	}

	void RadianceComputer::Render( TextureLayout const & p_srcTexture
		, TextureLayoutSPtr p_dstTexture )
	{
		static Matrix4x4r const l_views[] =
		{
			matrix::look_at( Point3r{ 0.0f, 0.0f, 0.0f }, Point3r{ +1.0f, +0.0f, +0.0f }, Point3r{ 0.0f, -1.0f, +0.0f } ),
			matrix::look_at( Point3r{ 0.0f, 0.0f, 0.0f }, Point3r{ -1.0f, +0.0f, +0.0f }, Point3r{ 0.0f, -1.0f, +0.0f } ),
			matrix::look_at( Point3r{ 0.0f, 0.0f, 0.0f }, Point3r{ +0.0f, +1.0f, +0.0f }, Point3r{ 0.0f, +0.0f, +1.0f } ),
			matrix::look_at( Point3r{ 0.0f, 0.0f, 0.0f }, Point3r{ +0.0f, -1.0f, +0.0f }, Point3r{ 0.0f, +0.0f, -1.0f } ),
			matrix::look_at( Point3r{ 0.0f, 0.0f, 0.0f }, Point3r{ +0.0f, +0.0f, +1.0f }, Point3r{ 0.0f, -1.0f, +0.0f } ),
			matrix::look_at( Point3r{ 0.0f, 0.0f, 0.0f }, Point3r{ +0.0f, +0.0f, -1.0f }, Point3r{ 0.0f, -1.0f, +0.0f } )
		};
		REQUIRE( p_dstTexture->GetDimensions() == m_size );
		std::array< FrameBufferAttachmentSPtr, 6 > l_attachs
		{
			{
				m_frameBuffer->CreateAttachment( p_dstTexture, CubeMapFace::ePositiveX ),
				m_frameBuffer->CreateAttachment( p_dstTexture, CubeMapFace::eNegativeX ),
				m_frameBuffer->CreateAttachment( p_dstTexture, CubeMapFace::ePositiveY ),
				m_frameBuffer->CreateAttachment( p_dstTexture, CubeMapFace::eNegativeY ),
				m_frameBuffer->CreateAttachment( p_dstTexture, CubeMapFace::ePositiveZ ),
				m_frameBuffer->CreateAttachment( p_dstTexture, CubeMapFace::eNegativeZ ),
			}
		};

		m_viewport.Apply();
		m_frameBuffer->Bind( FrameBufferTarget::eDraw );
		p_srcTexture.Bind( 0u );
		m_sampler->Bind( 0u );

		for ( uint32_t i = 0u; i < 6u; ++i )
		{
			l_attachs[i]->Attach( AttachmentPoint::eColour, 0u );
			m_frameBuffer->SetDrawBuffer( l_attachs[i] );
			m_frameBuffer->Clear( BufferComponent::eColour | BufferComponent::eDepth );
			REQUIRE( m_frameBuffer->IsComplete() );
			m_matrixUbo.Update( l_views[i], m_viewport.GetProjection() );
			m_pipeline->Apply();
			m_geometryBuffers->Draw( uint32_t( m_arrayVertex.size() ), 0u );
		}

		m_sampler->Unbind( 0u );
		p_srcTexture.Unbind( 0u );
		m_frameBuffer->Unbind();
	}

	ShaderProgramSPtr RadianceComputer::DoCreateProgram()
	{
		auto & l_renderSystem = *GetEngine()->GetRenderSystem();
		GLSL::Shader l_vtx;
		{
			using namespace GLSL;
			GlslWriter l_writer{ l_renderSystem.CreateGlslWriter() };

			// Inputs
			auto position = l_writer.DeclAttribute< Vec3 >( ShaderProgram::Position );
			UBO_MATRIX( l_writer );

			// Outputs
			auto vtx_position = l_writer.DeclOutput< Vec3 >( cuT( "vtx_position" ) );
			auto gl_Position = l_writer.DeclBuiltin< Vec4 >( cuT( "gl_Position" ) );

			std::function< void() > l_main = [&]()
			{
				vtx_position = position;
				auto l_view = l_writer.DeclLocale( cuT( "l_normal" )
					, mat4( mat3( c3d_mtxView ) ) );
				gl_Position = l_writer.Paren( c3d_mtxProjection * l_view * vec4( position, 1.0 ) ).SWIZZLE_XYWW;
			};

			l_writer.ImplementFunction< void >( cuT( "main" ), l_main );
			l_vtx = l_writer.Finalise();
		}

		GLSL::Shader l_pxl;
		{
			using namespace GLSL;
			GlslWriter l_writer{ l_renderSystem.CreateGlslWriter() };

			// Inputs
			auto vtx_position = l_writer.DeclInput< Vec3 >( cuT( "vtx_position" ) );
			auto c3d_mapDiffuse = l_writer.DeclUniform< SamplerCube >( ShaderProgram::MapDiffuse );

			// Outputs
			auto plx_v4FragColor = l_writer.DeclOutput< Vec4 >( cuT( "pxl_FragColor" ) );

			l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				// From https://learnopengl.com/#!PBR/Lighting
				// the sample direction equals the hemisphere's orientation 
				auto l_normal = l_writer.DeclLocale( cuT( "l_normal" )
					, normalize( vtx_position ) );

				auto l_irradiance = l_writer.DeclLocale( cuT( "l_irradiance" )
					, vec3( 0.0_f ) );

				auto l_up = l_writer.DeclLocale( cuT( "l_up" )
					, vec3( 0.0_f, 1.0, 0.0 ) );
				auto l_right = l_writer.DeclLocale( cuT( "l_right" )
					, cross( l_up, l_normal ) );
				l_up = cross( l_normal, l_right );

				auto l_sampleDelta = l_writer.DeclLocale( cuT( "l_sampleDelta" )
					, 0.025_f );
				auto l_nrSamples = l_writer.DeclLocale( cuT( "l_nrSamples" )
					, 0_i );
				auto PI = l_writer.DeclLocale( cuT( "PI" )
					, 3.14159265359_f );

				FOR( l_writer, Float, phi, 0.0, "phi < 2.0 * PI", "phi += l_sampleDelta" )
				{
					FOR( l_writer, Float, theta, 0.0, "theta < 0.5 * PI", "theta += l_sampleDelta" )
					{
						// spherical to cartesian (in tangent space)
						auto l_tangentSample = l_writer.DeclLocale( cuT( "l_tangentSample" )
							, vec3( sin( theta ) * cos( phi ), sin( theta ) * sin( phi ), cos( theta ) ) );
						// tangent space to world
						auto l_sampleVec = l_writer.DeclLocale( cuT( "l_sampleVec" )
							, l_right * l_tangentSample.x() + l_up * l_tangentSample.y() + l_normal * l_tangentSample.z() );

						l_irradiance += texture( c3d_mapDiffuse, l_sampleVec ).rgb() * cos( theta ) * sin( theta );
						l_nrSamples = l_nrSamples + 1;
					}
					ROF;
				}
				ROF;

				l_irradiance = l_irradiance * PI * l_writer.Paren( 1.0_f / l_writer.Cast< Float >( l_nrSamples ) );
				plx_v4FragColor = vec4( l_irradiance, 1.0 );
			} );

			l_pxl = l_writer.Finalise();
		}

		auto & l_cache = GetEngine()->GetShaderProgramCache();
		auto l_program = l_cache.GetNewProgram( false );
		l_program->CreateObject( ShaderType::eVertex );
		l_program->CreateObject( ShaderType::ePixel );
		l_program->SetSource( ShaderType::eVertex, l_vtx );
		l_program->SetSource( ShaderType::ePixel, l_pxl );
		l_program->CreateUniform< UniformType::eInt >( ShaderProgram::MapDiffuse, ShaderType::ePixel );
		l_program->Initialise();
		return l_program;
	}
}
