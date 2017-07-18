#include "EnvironmentPrefilter.hpp"

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
	EnvironmentPrefilter::EnvironmentPrefilter( Engine & p_engine
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
		, m_configUbo{ cuT( "Config" ), *p_engine.GetRenderSystem() }
		, m_roughnessUniform{ *m_configUbo.CreateUniform< UniformType::eFloat >( cuT( "c3d_roughness" ) ) }
	{
		uint32_t i = 0;

		for ( auto & vertex : m_arrayVertex )
		{
			vertex = std::make_shared< BufferElementGroup >( &reinterpret_cast< uint8_t * >( m_bufferVertex.data() )[i++ * m_declaration.stride()] );
		}

		m_viewport.Initialise();
		m_viewport.SetPerspective( Angle::from_degrees( 90.0_r ), 1.0_r, 0.1_r, 10.0_r );
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
		m_pipeline->AddUniformBuffer( m_configUbo );

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

		if ( !GetEngine()->GetSamplerCache().Has( cuT( "EnvironmentPrefilter" ) ) )
		{
			m_sampler = GetEngine()->GetSamplerCache().Add( cuT( "EnvironmentPrefilter" ) );
			m_sampler->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
			m_sampler->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
			m_sampler->SetInterpolationMode( InterpolationFilter::eMip, InterpolationMode::eLinear );
			m_sampler->SetWrappingMode( TextureUVW::eU, WrapMode::eClampToEdge );
			m_sampler->SetWrappingMode( TextureUVW::eV, WrapMode::eClampToEdge );
			m_sampler->SetWrappingMode( TextureUVW::eW, WrapMode::eClampToEdge );
			m_sampler->Initialise();
		}
		else
		{
			m_sampler = GetEngine()->GetSamplerCache().Find( cuT( "EnvironmentPrefilter" ) );
		}
	}

	EnvironmentPrefilter::~EnvironmentPrefilter()
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
		m_configUbo.Cleanup();

		for ( auto & vertex : m_arrayVertex )
		{
			vertex.reset();
		}
	}

	void EnvironmentPrefilter::Render( TextureLayout const & p_srcTexture
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
		p_srcTexture.Bind( 0u );
		m_sampler->Bind( 0u );
		m_frameBuffer->Bind( FrameBufferTarget::eDraw );
		REQUIRE( m_frameBuffer->IsComplete() );

		for ( unsigned int mip = 0; mip < GLSL::Utils::MaxIblReflectionLod + 1; ++mip )
		{
			Size mipSize{ uint32_t( m_size.width() * std::pow( 0.5, mip ) )
				, uint32_t( m_size.height() * std::pow( 0.5, mip ) ) };
			std::array< FrameBufferAttachmentSPtr, 6 > attachs
			{
				{
					m_frameBuffer->CreateAttachment( p_dstTexture, CubeMapFace::ePositiveX, mip ),
					m_frameBuffer->CreateAttachment( p_dstTexture, CubeMapFace::eNegativeX, mip ),
					m_frameBuffer->CreateAttachment( p_dstTexture, CubeMapFace::ePositiveY, mip ),
					m_frameBuffer->CreateAttachment( p_dstTexture, CubeMapFace::eNegativeY, mip ),
					m_frameBuffer->CreateAttachment( p_dstTexture, CubeMapFace::ePositiveZ, mip ),
					m_frameBuffer->CreateAttachment( p_dstTexture, CubeMapFace::eNegativeZ, mip ),
				}
			};

			m_depthBuffer->Resize( mipSize );
			m_viewport.Resize( mipSize );
			m_viewport.Apply();
			m_roughnessUniform.SetValue( mip / float( GLSL::Utils::MaxIblReflectionLod ) );
			m_configUbo.Update();
			m_configUbo.BindTo( 0u );

			for ( uint32_t i = 0u; i < 6u; ++i )
			{
				attachs[i]->Attach( AttachmentPoint::eColour, 0u );
				REQUIRE( m_frameBuffer->IsComplete() );
				m_frameBuffer->SetDrawBuffer( attachs[i] );
				m_frameBuffer->Clear( BufferComponent::eColour | BufferComponent::eDepth );
				m_matrixUbo.Update( views[i], m_viewport.GetProjection() );
				m_pipeline->Apply();
				m_geometryBuffers->Draw( uint32_t( m_arrayVertex.size() ), 0u );
			}
		}

		m_frameBuffer->Unbind();
		m_sampler->Unbind( 0u );
		p_srcTexture.Unbind( 0u );
	}

	ShaderProgramSPtr EnvironmentPrefilter::DoCreateProgram()
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
			Ubo config{ writer, cuT( "Config" ), 0u };
			auto c3d_roughness = config.DeclMember< Float >( cuT( "c3d_roughness" ) );
			config.End();

			// Outputs
			auto plx_v4FragColor = writer.DeclOutput< Vec4 >( cuT( "pxl_FragColor" ) );

			auto distributionGGX = writer.ImplementFunction< Float >( cuT( "DistributionGGX" )
				, [&]( Vec3 const & p_N
					, Vec3 const & p_H
					, Float const & p_roughness )
				{
					auto constexpr PI = 3.1415926535897932384626433832795028841968;
					auto a = writer.DeclLocale( cuT( "a" )
						, p_roughness * p_roughness );
					auto a2 = writer.DeclLocale( cuT( "a2" )
						, a * a );
					auto NdotH = writer.DeclLocale( cuT( "NdotH" )
						, max( dot( p_N, p_H ), 0.0 ) );
					auto NdotH2 = writer.DeclLocale( cuT( "NdotH2" )
						, NdotH * NdotH );

					auto nom = writer.DeclLocale( cuT( "nom" )
						, a2 );
					auto denom = writer.DeclLocale( cuT( "denom" )
						, ( NdotH2 * writer.Paren( a2 - 1.0 ) + 1.0 ) );
					denom = denom * denom * PI;

					writer.Return( nom / denom );
				}
				, InVec3{ &writer, cuT( "p_N" ) }
				, InVec3{ &writer, cuT( "p_H" ) }
				, InFloat{ &writer, cuT( "p_roughness" ) } );

			auto radicalInverse = writer.ImplementFunction< Float >( cuT( "RadicalInverse_VdC" )
				, [&]( UInt const & p_bits )
				{
					// From https://learnopengl.com/#!PBR/Lighting
					auto bits = writer.DeclLocale( cuT( "bits" )
						, p_bits );
					bits = writer.Paren( bits << 16u ) | writer.Paren( bits >> 16u );
					bits = writer.Paren( writer.Paren( bits & 0x55555555_ui ) << 1u ) | writer.Paren( writer.Paren( bits & 0xAAAAAAAA_ui ) >> 1u );
					bits = writer.Paren( writer.Paren( bits & 0x33333333_ui ) << 2u ) | writer.Paren( writer.Paren( bits & 0xCCCCCCCC_ui ) >> 2u );
					bits = writer.Paren( writer.Paren( bits & 0x0F0F0F0F_ui ) << 4u ) | writer.Paren( writer.Paren( bits & 0xF0F0F0F0_ui ) >> 4u );
					bits = writer.Paren( writer.Paren( bits & 0x00FF00FF_ui ) << 8u ) | writer.Paren( writer.Paren( bits & 0xFF00FF00_ui ) >> 8u );
					writer.Return( writer.Cast< Float >( bits ) * 2.3283064365386963e-10 ); // / 0x100000000
				}
				, InUInt{ &writer, cuT( "p_bits" ) } );

			auto hammersley = writer.ImplementFunction< Vec2 >( cuT( "Hammersley" )
				, [&]( UInt const & p_i
					, UInt const & p_n )
				{
					// From https://learnopengl.com/#!PBR/Lighting
					writer.Return( vec2( writer.Cast< Float >( p_i ) / writer.Cast< Float >( p_n ), radicalInverse( p_i ) ) );
				}
				, InUInt{ &writer, cuT( "p_i" ) }
				, InUInt{ &writer, cuT( "p_n" ) } );

			auto importanceSample = writer.ImplementFunction< Vec3 >( cuT( "ImportanceSampleGGX" )
				, [&]( Vec2 const & p_xi
					, Vec3 const & p_n
					, Float const & p_roughness )
				{
					// From https://learnopengl.com/#!PBR/Lighting
					auto constexpr PI = 3.1415926535897932384626433832795028841968;
					auto a = writer.DeclLocale( cuT( "a" )
						, p_roughness * p_roughness );

					auto phi = writer.DeclLocale( cuT( "phi" )
						, 2.0_f * PI * p_xi.x() );
					auto cosTheta = writer.DeclLocale( cuT( "cosTheta" )
						, sqrt( writer.Paren( 1.0 - p_xi.y() ) / writer.Paren( 1.0 + writer.Paren( a * a - 1.0 ) * p_xi.y() ) ) );
					auto sinTheta = writer.DeclLocale( cuT( "sinTheta" )
						, sqrt( 1.0 - cosTheta * cosTheta ) );

					// from spherical coordinates to cartesian coordinates
					auto H = writer.DeclLocale< Vec3 >( cuT( "H" ) );
					H.x() = cos( phi ) * sinTheta;
					H.y() = sin( phi ) * sinTheta;
					H.z() = cosTheta;

					// from tangent-space vector to world-space sample vector
					auto up = writer.DeclLocale( cuT( "up" )
						, writer.Ternary( GLSL::abs( p_n.z() ) < 0.999, vec3( 0.0_f, 0.0, 1.0 ), vec3( 1.0_f, 0.0, 0.0 ) ) );
					auto tangent = writer.DeclLocale( cuT( "tangent" )
						, normalize( cross( up, p_n ) ) );
					auto bitangent = writer.DeclLocale( cuT( "bitangent" )
						, cross( p_n, tangent ) );

					auto sampleVec = writer.DeclLocale( cuT( "sampleVec" )
						, tangent * H.x() + bitangent * H.y() + p_n * H.z() );
					writer.Return( normalize( sampleVec ) );
				}
				, InVec2{ &writer, cuT( "p_xi" ) }
				, InVec3{ &writer, cuT( "p_n" ) }
				, InFloat{ &writer, cuT( "p_roughness" ) } );

			writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				auto constexpr PI = 3.1415926535897932384626433832795028841968;
				// From https://learnopengl.com/#!PBR/Lighting
				auto N = writer.DeclLocale( cuT( "N" )
					, normalize( vtx_position ) );
				auto R = writer.DeclLocale( cuT( "R" )
					, N );
				auto V = writer.DeclLocale( cuT( "V" )
					, R );

				auto sampleCount = writer.DeclLocale( cuT( "sampleCount" )
					, 1024_ui );
				auto totalWeight = writer.DeclLocale( cuT( "totalWeight" )
					, 0.0_f );
				auto prefilteredColor = writer.DeclLocale( cuT( "prefilteredColor" )
					, vec3( 0.0_f ) );

				FOR( writer, UInt, i, 0, "i < sampleCount", "++i" )
				{
					auto xi = writer.DeclLocale( cuT( "xi" )
						, hammersley( i, sampleCount ) );
					auto H = writer.DeclLocale( cuT( "H" )
						, importanceSample( xi, N, c3d_roughness ) );
					auto L = writer.DeclLocale( cuT( "L" )
						, normalize( vec3( 2.0_f ) * dot( V, H ) * H - V ) );

					auto NdotL = writer.DeclLocale( cuT( "NdotL" )
						, max( dot( N, L ), 0.0 ) );

					IF( writer, "NdotL > 0.0" )
					{
						auto D = writer.DeclLocale( cuT( "D" )
							, distributionGGX( N, H, c3d_roughness ) );
						auto NdotH = writer.DeclLocale( cuT( "NdotH" )
							, max( dot( N, H ), 0.0 ) );
						auto HdotV = writer.DeclLocale( cuT( "HdotV" )
							, max( dot( H, V ), 0.0 ) );
						auto pdf = writer.DeclLocale( cuT( "pdf" )
							, D * NdotH / writer.Paren( 4.0_f * HdotV ) + 0.0001 );

						auto resolution = writer.DeclLocale( cuT( "resolution" )
							, Float( int( m_size.width() ) ) ); // resolution of source cubemap (per face)
						auto saTexel = writer.DeclLocale( cuT( "saTexel" )
							, 4.0_f * PI / writer.Paren( 6.0 * resolution * resolution ) );
						auto saSample = writer.DeclLocale( cuT( "saSample" )
							, 1.0_f / writer.Paren( writer.Cast< Float >( sampleCount ) * pdf + 0.0001 ) );
						auto mipLevel = writer.DeclLocale( cuT( "mipLevel" )
							, writer.Ternary( c3d_roughness == 0.0_f, 0.0_f, 0.5_f * log2( saSample / saTexel ) ) );

						prefilteredColor += texture( c3d_mapDiffuse, L, mipLevel ).rgb() * NdotL;
						totalWeight += NdotL;
					}
					FI;
				}
				ROF;

				prefilteredColor = prefilteredColor / totalWeight;
				plx_v4FragColor = vec4( prefilteredColor, 1.0 );
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
