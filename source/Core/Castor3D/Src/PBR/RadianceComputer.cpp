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
	RadianceComputer::RadianceComputer( Engine & engine
		, Castor::Size const & p_size )
		: OwnedBy< Engine >{ engine }
		, m_matrixUbo{ engine }
		, m_viewport{ engine }
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

		for ( auto & vertex : m_arrayVertex )
		{
			vertex = std::make_shared< BufferElementGroup >( &reinterpret_cast< uint8_t * >( m_bufferVertex.data() )[i++ * m_declaration.stride()] );
		}

		m_viewport.Initialise();
		m_viewport.SetPerspective( Angle::from_degrees( 90.0_r ), 1.0_r, 0.1_r, 10.0_r );
		m_viewport.Resize( m_size );
		m_viewport.Update();
		auto & program = *DoCreateProgram();
		auto & renderSystem = *GetEngine()->GetRenderSystem();
		m_vertexBuffer = std::make_shared< VertexBuffer >( *GetEngine()
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

		m_frameBuffer = renderSystem.CreateFrameBuffer();

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

		for ( auto & vertex : m_arrayVertex )
		{
			vertex.reset();
		}
	}

	void RadianceComputer::Render( TextureLayout const & p_srcTexture
		, TextureLayoutSPtr p_dstTexture )
	{
		static Matrix4x4r const views[] =
		{
			matrix::look_at( Point3r{ 0.0f, 0.0f, 0.0f }, Point3r{ +1.0f, +0.0f, +0.0f }, Point3r{ 0.0f, -1.0f, +0.0f } ),
			matrix::look_at( Point3r{ 0.0f, 0.0f, 0.0f }, Point3r{ -1.0f, +0.0f, +0.0f }, Point3r{ 0.0f, -1.0f, +0.0f } ),
			matrix::look_at( Point3r{ 0.0f, 0.0f, 0.0f }, Point3r{ +0.0f, +1.0f, +0.0f }, Point3r{ 0.0f, +0.0f, +1.0f } ),
			matrix::look_at( Point3r{ 0.0f, 0.0f, 0.0f }, Point3r{ +0.0f, -1.0f, +0.0f }, Point3r{ 0.0f, +0.0f, -1.0f } ),
			matrix::look_at( Point3r{ 0.0f, 0.0f, 0.0f }, Point3r{ +0.0f, +0.0f, +1.0f }, Point3r{ 0.0f, -1.0f, +0.0f } ),
			matrix::look_at( Point3r{ 0.0f, 0.0f, 0.0f }, Point3r{ +0.0f, +0.0f, -1.0f }, Point3r{ 0.0f, -1.0f, +0.0f } )
		};
		REQUIRE( p_dstTexture->GetDimensions() == m_size );
		std::array< FrameBufferAttachmentSPtr, 6 > attachs
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
			attachs[i]->Attach( AttachmentPoint::eColour, 0u );
			m_frameBuffer->SetDrawBuffer( attachs[i] );
			m_frameBuffer->Clear( BufferComponent::eColour | BufferComponent::eDepth );
			REQUIRE( m_frameBuffer->IsComplete() );
			m_matrixUbo.Update( views[i], m_viewport.GetProjection() );
			m_pipeline->Apply();
			m_geometryBuffers->Draw( uint32_t( m_arrayVertex.size() ), 0u );
		}

		m_sampler->Unbind( 0u );
		p_srcTexture.Unbind( 0u );
		m_frameBuffer->Unbind();
	}

	ShaderProgramSPtr RadianceComputer::DoCreateProgram()
	{
		auto & renderSystem = *GetEngine()->GetRenderSystem();
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

			std::function< void() > main = [&]()
			{
				vtx_position = position;
				auto view = writer.DeclLocale( cuT( "normal" )
					, mat4( mat3( c3d_mtxView ) ) );
				gl_Position = writer.Paren( c3d_mtxProjection * view * vec4( position, 1.0 ) ).SWIZZLE_XYWW;
			};

			writer.ImplementFunction< void >( cuT( "main" ), main );
			vtx = writer.Finalise();
		}

		GLSL::Shader pxl;
		{
			using namespace GLSL;
			GlslWriter writer{ renderSystem.CreateGlslWriter() };

			// Inputs
			auto vtx_position = writer.DeclInput< Vec3 >( cuT( "vtx_position" ) );
			auto c3d_mapDiffuse = writer.DeclUniform< SamplerCube >( ShaderProgram::MapDiffuse );

			// Outputs
			auto plx_v4FragColor = writer.DeclOutput< Vec4 >( cuT( "pxl_FragColor" ) );

			writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				// From https://learnopengl.com/#!PBR/Lighting
				// the sample direction equals the hemisphere's orientation 
				auto normal = writer.DeclLocale( cuT( "normal" )
					, normalize( vtx_position ) );

				auto irradiance = writer.DeclLocale( cuT( "irradiance" )
					, vec3( 0.0_f ) );

				auto up = writer.DeclLocale( cuT( "up" )
					, vec3( 0.0_f, 1.0, 0.0 ) );
				auto right = writer.DeclLocale( cuT( "right" )
					, cross( up, normal ) );
				up = cross( normal, right );

				auto sampleDelta = writer.DeclLocale( cuT( "sampleDelta" )
					, 0.025_f );
				auto nrSamples = writer.DeclLocale( cuT( "nrSamples" )
					, 0_i );
				auto PI = writer.DeclLocale( cuT( "PI" )
					, 3.14159265359_f );

				FOR( writer, Float, phi, 0.0, "phi < 2.0 * PI", "phi += sampleDelta" )
				{
					FOR( writer, Float, theta, 0.0, "theta < 0.5 * PI", "theta += sampleDelta" )
					{
						// spherical to cartesian (in tangent space)
						auto tangentSample = writer.DeclLocale( cuT( "tangentSample" )
							, vec3( sin( theta ) * cos( phi ), sin( theta ) * sin( phi ), cos( theta ) ) );
						// tangent space to world
						auto sampleVec = writer.DeclLocale( cuT( "sampleVec" )
							, right * tangentSample.x() + up * tangentSample.y() + normal * tangentSample.z() );

						irradiance += texture( c3d_mapDiffuse, sampleVec ).rgb() * cos( theta ) * sin( theta );
						nrSamples = nrSamples + 1;
					}
					ROF;
				}
				ROF;

				irradiance = irradiance * PI * writer.Paren( 1.0_f / writer.Cast< Float >( nrSamples ) );
				plx_v4FragColor = vec4( irradiance, 1.0 );
			} );

			pxl = writer.Finalise();
		}

		auto & cache = GetEngine()->GetShaderProgramCache();
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
