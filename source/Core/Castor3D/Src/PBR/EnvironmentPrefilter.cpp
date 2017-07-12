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

		for ( auto & l_vertex : m_arrayVertex )
		{
			l_vertex = std::make_shared< BufferElementGroup >( &reinterpret_cast< uint8_t * >( m_bufferVertex.data() )[i++ * m_declaration.stride()] );
		}

		m_viewport.Initialise();
		m_viewport.SetPerspective( Angle::from_degrees( 90.0_r ), 1.0_r, 0.1_r, 10.0_r );
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
		m_pipeline->AddUniformBuffer( m_configUbo );

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

		for ( auto & l_vertex : m_arrayVertex )
		{
			l_vertex.reset();
		}
	}

	void EnvironmentPrefilter::Render( TextureLayout const & p_srcTexture
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
		p_srcTexture.Bind( 0u );
		m_sampler->Bind( 0u );
		m_frameBuffer->Bind( FrameBufferTarget::eDraw );
		REQUIRE( m_frameBuffer->IsComplete() );

		for ( unsigned int l_mip = 0; l_mip < GLSL::Utils::MaxIblReflectionLod + 1; ++l_mip )
		{
			Size l_mipSize{ uint32_t( m_size.width() * std::pow( 0.5, l_mip ) )
				, uint32_t( m_size.height() * std::pow( 0.5, l_mip ) ) };
			std::array< FrameBufferAttachmentSPtr, 6 > l_attachs
			{
				{
					m_frameBuffer->CreateAttachment( p_dstTexture, CubeMapFace::ePositiveX, l_mip ),
					m_frameBuffer->CreateAttachment( p_dstTexture, CubeMapFace::eNegativeX, l_mip ),
					m_frameBuffer->CreateAttachment( p_dstTexture, CubeMapFace::ePositiveY, l_mip ),
					m_frameBuffer->CreateAttachment( p_dstTexture, CubeMapFace::eNegativeY, l_mip ),
					m_frameBuffer->CreateAttachment( p_dstTexture, CubeMapFace::ePositiveZ, l_mip ),
					m_frameBuffer->CreateAttachment( p_dstTexture, CubeMapFace::eNegativeZ, l_mip ),
				}
			};

			m_depthBuffer->Resize( l_mipSize );
			m_viewport.Resize( l_mipSize );
			m_viewport.Apply();
			m_roughnessUniform.SetValue( l_mip / float( GLSL::Utils::MaxIblReflectionLod ) );
			m_configUbo.Update();
			m_configUbo.BindTo( 0u );

			for ( uint32_t i = 0u; i < 6u; ++i )
			{
				l_attachs[i]->Attach( AttachmentPoint::eColour, 0u );
				REQUIRE( m_frameBuffer->IsComplete() );
				m_frameBuffer->SetDrawBuffer( l_attachs[i] );
				m_frameBuffer->Clear( BufferComponent::eColour | BufferComponent::eDepth );
				m_matrixUbo.Update( l_views[i], m_viewport.GetProjection() );
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
			Ubo l_config{ l_writer, cuT( "Config" ), 0u };
			auto c3d_roughness = l_config.DeclMember< Float >( cuT( "c3d_roughness" ) );
			l_config.End();

			// Outputs
			auto plx_v4FragColor = l_writer.DeclOutput< Vec4 >( cuT( "pxl_FragColor" ) );

			auto l_distributionGGX = l_writer.ImplementFunction< Float >( cuT( "DistributionGGX" )
				, [&]( Vec3 const & p_N
					, Vec3 const & p_H
					, Float const & p_roughness )
				{
					auto constexpr PI = 3.1415926535897932384626433832795028841968;
					auto l_a = l_writer.DeclLocale( cuT( "l_a" )
						, p_roughness * p_roughness );
					auto l_a2 = l_writer.DeclLocale( cuT( "l_a2" )
						, l_a * l_a );
					auto l_NdotH = l_writer.DeclLocale( cuT( "l_NdotH" )
						, max( dot( p_N, p_H ), 0.0 ) );
					auto l_NdotH2 = l_writer.DeclLocale( cuT( "l_NdotH2" )
						, l_NdotH * l_NdotH );

					auto l_nom = l_writer.DeclLocale( cuT( "l_nom" )
						, l_a2 );
					auto l_denom = l_writer.DeclLocale( cuT( "l_denom" )
						, ( l_NdotH2 * l_writer.Paren( l_a2 - 1.0 ) + 1.0 ) );
					l_denom = l_denom * l_denom * PI;

					l_writer.Return( l_nom / l_denom );
				}
				, InVec3{ &l_writer, cuT( "p_N" ) }
				, InVec3{ &l_writer, cuT( "p_H" ) }
				, InFloat{ &l_writer, cuT( "p_roughness" ) } );

			auto l_radicalInverse = l_writer.ImplementFunction< Float >( cuT( "RadicalInverse_VdC" )
				, [&]( UInt const & p_bits )
				{
					// From https://learnopengl.com/#!PBR/Lighting
					auto l_bits = l_writer.DeclLocale( cuT( "l_bits" )
						, p_bits );
					l_bits = l_writer.Paren( l_bits << 16u ) | l_writer.Paren( l_bits >> 16u );
					l_bits = l_writer.Paren( l_writer.Paren( l_bits & 0x55555555_ui ) << 1u ) | l_writer.Paren( l_writer.Paren( l_bits & 0xAAAAAAAA_ui ) >> 1u );
					l_bits = l_writer.Paren( l_writer.Paren( l_bits & 0x33333333_ui ) << 2u ) | l_writer.Paren( l_writer.Paren( l_bits & 0xCCCCCCCC_ui ) >> 2u );
					l_bits = l_writer.Paren( l_writer.Paren( l_bits & 0x0F0F0F0F_ui ) << 4u ) | l_writer.Paren( l_writer.Paren( l_bits & 0xF0F0F0F0_ui ) >> 4u );
					l_bits = l_writer.Paren( l_writer.Paren( l_bits & 0x00FF00FF_ui ) << 8u ) | l_writer.Paren( l_writer.Paren( l_bits & 0xFF00FF00_ui ) >> 8u );
					l_writer.Return( l_writer.Cast< Float >( l_bits ) * 2.3283064365386963e-10 ); // / 0x100000000
				}
				, InUInt{ &l_writer, cuT( "p_bits" ) } );

			auto l_hammersley = l_writer.ImplementFunction< Vec2 >( cuT( "Hammersley" )
				, [&]( UInt const & p_i
					, UInt const & p_n )
				{
					// From https://learnopengl.com/#!PBR/Lighting
					l_writer.Return( vec2( l_writer.Cast< Float >( p_i ) / l_writer.Cast< Float >( p_n ), l_radicalInverse( p_i ) ) );
				}
				, InUInt{ &l_writer, cuT( "p_i" ) }
				, InUInt{ &l_writer, cuT( "p_n" ) } );

			auto l_importanceSample = l_writer.ImplementFunction< Vec3 >( cuT( "ImportanceSampleGGX" )
				, [&]( Vec2 const & p_xi
					, Vec3 const & p_n
					, Float const & p_roughness )
				{
					// From https://learnopengl.com/#!PBR/Lighting
					auto constexpr PI = 3.1415926535897932384626433832795028841968;
					auto l_a = l_writer.DeclLocale( cuT( "l_a" )
						, p_roughness * p_roughness );

					auto l_phi = l_writer.DeclLocale( cuT( "l_phi" )
						, 2.0_f * PI * p_xi.x() );
					auto l_cosTheta = l_writer.DeclLocale( cuT( "l_cosTheta" )
						, sqrt( l_writer.Paren( 1.0 - p_xi.y() ) / l_writer.Paren( 1.0 + l_writer.Paren( l_a * l_a - 1.0 ) * p_xi.y() ) ) );
					auto l_sinTheta = l_writer.DeclLocale( cuT( "l_sinTheta" )
						, sqrt( 1.0 - l_cosTheta * l_cosTheta ) );

					// from spherical coordinates to cartesian coordinates
					auto l_H = l_writer.DeclLocale< Vec3 >( cuT( "l_H" ) );
					l_H.x() = cos( l_phi ) * l_sinTheta;
					l_H.y() = sin( l_phi ) * l_sinTheta;
					l_H.z() = l_cosTheta;

					// from tangent-space vector to world-space sample vector
					auto l_up = l_writer.DeclLocale( cuT( "l_up" )
						, l_writer.Ternary( GLSL::abs( p_n.z() ) < 0.999, vec3( 0.0_f, 0.0, 1.0 ), vec3( 1.0_f, 0.0, 0.0 ) ) );
					auto l_tangent = l_writer.DeclLocale( cuT( "l_tangent" )
						, normalize( cross( l_up, p_n ) ) );
					auto l_bitangent = l_writer.DeclLocale( cuT( "l_bitangent" )
						, cross( p_n, l_tangent ) );

					auto l_sampleVec = l_writer.DeclLocale( cuT( "l_sampleVec" )
						, l_tangent * l_H.x() + l_bitangent * l_H.y() + p_n * l_H.z() );
					l_writer.Return( normalize( l_sampleVec ) );
				}
				, InVec2{ &l_writer, cuT( "p_xi" ) }
				, InVec3{ &l_writer, cuT( "p_n" ) }
				, InFloat{ &l_writer, cuT( "p_roughness" ) } );

			l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				auto constexpr PI = 3.1415926535897932384626433832795028841968;
				// From https://learnopengl.com/#!PBR/Lighting
				auto l_N = l_writer.DeclLocale( cuT( "l_N" )
					, normalize( vtx_position ) );
				auto l_R = l_writer.DeclLocale( cuT( "l_R" )
					, l_N );
				auto l_V = l_writer.DeclLocale( cuT( "l_V" )
					, l_R );

				auto l_sampleCount = l_writer.DeclLocale( cuT( "l_sampleCount" )
					, 1024_ui );
				auto l_totalWeight = l_writer.DeclLocale( cuT( "l_totalWeight" )
					, 0.0_f );
				auto l_prefilteredColor = l_writer.DeclLocale( cuT( "l_prefilteredColor" )
					, vec3( 0.0_f ) );

				FOR( l_writer, UInt, i, 0, "i < l_sampleCount", "++i" )
				{
					auto l_xi = l_writer.DeclLocale( cuT( "l_xi" )
						, l_hammersley( i, l_sampleCount ) );
					auto l_H = l_writer.DeclLocale( cuT( "l_H" )
						, l_importanceSample( l_xi, l_N, c3d_roughness ) );
					auto l_L = l_writer.DeclLocale( cuT( "l_L" )
						, normalize( vec3( 2.0_f ) * dot( l_V, l_H ) * l_H - l_V ) );

					auto l_NdotL = l_writer.DeclLocale( cuT( "l_NdotL" )
						, max( dot( l_N, l_L ), 0.0 ) );

					IF( l_writer, "l_NdotL > 0.0" )
					{
						auto l_D = l_writer.DeclLocale( cuT( "l_D" )
							, l_distributionGGX( l_N, l_H, c3d_roughness ) );
						auto l_NdotH = l_writer.DeclLocale( cuT( "l_NdotH" )
							, max( dot( l_N, l_H ), 0.0 ) );
						auto l_HdotV = l_writer.DeclLocale( cuT( "l_HdotV" )
							, max( dot( l_H, l_V ), 0.0 ) );
						auto l_pdf = l_writer.DeclLocale( cuT( "l_pdf" )
							, l_D * l_NdotH / l_writer.Paren( 4.0_f * l_HdotV ) + 0.0001 );

						auto l_resolution = l_writer.DeclLocale( cuT( "l_resolution" )
							, Float( int( m_size.width() ) ) ); // resolution of source cubemap (per face)
						auto l_saTexel = l_writer.DeclLocale( cuT( "l_saTexel" )
							, 4.0_f * PI / l_writer.Paren( 6.0 * l_resolution * l_resolution ) );
						auto l_saSample = l_writer.DeclLocale( cuT( "l_saSample" )
							, 1.0_f / l_writer.Paren( l_writer.Cast< Float >( l_sampleCount ) * l_pdf + 0.0001 ) );
						auto l_mipLevel = l_writer.DeclLocale( cuT( "l_mipLevel" )
							, l_writer.Ternary( c3d_roughness == 0.0_f, 0.0_f, 0.5_f * log2( l_saSample / l_saTexel ) ) );

						l_prefilteredColor += texture( c3d_mapDiffuse, l_L, l_mipLevel ).rgb() * l_NdotL;
						l_totalWeight += l_NdotL;
					}
					FI;
				}
				ROF;

				l_prefilteredColor = l_prefilteredColor / l_totalWeight;
				plx_v4FragColor = vec4( l_prefilteredColor, 1.0 );
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
