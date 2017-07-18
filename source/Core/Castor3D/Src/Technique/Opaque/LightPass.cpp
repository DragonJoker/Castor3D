#include "LightPass.hpp"

#include <Engine.hpp>
#include <Mesh/Buffer/GeometryBuffers.hpp>
#include <FrameBuffer/FrameBuffer.hpp>
#include <FrameBuffer/RenderBufferAttachment.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/Viewport.hpp>
#include <Scene/Camera.hpp>
#include <Scene/Scene.hpp>
#include <Shader/ModelMatrixUbo.hpp>
#include <Shader/SceneUbo.hpp>
#include <Shader/ShaderProgram.hpp>
#include <State/BlendState.hpp>
#include <State/DepthStencilState.hpp>
#include <State/MultisampleState.hpp>
#include <State/RasteriserState.hpp>
#include <Texture/Sampler.hpp>
#include <Texture/TextureLayout.hpp>
#include <Texture/TextureUnit.hpp>

#include <GlslSource.hpp>
#include <GlslLight.hpp>
#include <GlslShadow.hpp>
#include <GlslUtils.hpp>
#include <GlslPhongLighting.hpp>
#include <GlslMetallicBrdfLighting.hpp>
#include <GlslSpecularBrdfLighting.hpp>

using namespace Castor;
using namespace Castor3D;

namespace Castor3D
{
	//************************************************************************************************

	String GetTextureName( DsTexture p_texture )
	{
		static std::array< String, size_t( DsTexture::eCount ) > Values
		{
			{
				cuT( "c3d_mapDepth" ),
				cuT( "c3d_mapData1" ),
				cuT( "c3d_mapData2" ),
				cuT( "c3d_mapData3" ),
				cuT( "c3d_mapData4" ),
			}
		};

		return Values[size_t( p_texture )];
	}

	PixelFormat GetTextureFormat( DsTexture p_texture )
	{
		static std::array< PixelFormat, size_t( DsTexture::eCount ) > Values
		{
			{
				PixelFormat::eD24S8,
				PixelFormat::eRGBA16F32F,
				PixelFormat::eRGBA16F32F,
				PixelFormat::eRGBA16F32F,
				PixelFormat::eRGBA16F32F,
			}
		};

		return Values[size_t( p_texture )];
	}

	AttachmentPoint GetTextureAttachmentPoint( DsTexture p_texture )
	{
		static std::array< AttachmentPoint, size_t( DsTexture::eCount ) > Values
		{
			{
				AttachmentPoint::eDepthStencil,
				AttachmentPoint::eColour,
				AttachmentPoint::eColour,
				AttachmentPoint::eColour,
				AttachmentPoint::eColour,
			}
		};

		return Values[size_t( p_texture )];
	}

	uint32_t GetTextureAttachmentIndex( DsTexture p_texture )
	{
		static std::array< uint32_t, size_t( DsTexture::eCount ) > Values
		{
			{
				0,
				0,
				1,
				2,
				3,
			}
		};

		return Values[size_t( p_texture )];
	}

	float GetMaxDistance( LightCategory const & p_light
		, Point3f const & p_attenuation
		, float p_max )
	{
		constexpr float threshold = 0.000001f;
		auto constant = std::abs( p_attenuation[0] );
		auto linear = std::abs( p_attenuation[1] );
		auto quadratic = std::abs( p_attenuation[2] );
		float result = p_max;

		if ( constant >= threshold
			|| linear >= threshold
			|| quadratic >= threshold )
		{
			float maxChannel = std::max( std::max( p_light.GetColour()[0]
				, p_light.GetColour()[1] )
				, p_light.GetColour()[2] );
			auto c = 256.0f * maxChannel * p_light.GetDiffuseIntensity();

			if ( quadratic >= threshold )
			{
				if ( linear < threshold )
				{
					REQUIRE( c >= constant );
					result = sqrtf( ( c - constant ) / quadratic );
				}
				else
				{
					auto delta = linear * linear - 4 * quadratic * ( constant - c );
					REQUIRE( delta >= 0 );
					result = ( -linear + sqrtf( delta ) ) / ( 2 * quadratic );
				}
			}
			else if ( linear >= threshold )
			{
				result = ( c - constant ) / linear;
			}
		}

		return std::min( p_max, result );
	}

	void Declare_EncodeMaterial( GLSL::GlslWriter & p_writer )
	{
		using namespace GLSL;
		using GLSL::operator<<;
		auto encodeMaterial = p_writer.ImplementFunction< Void >( cuT( "EncodeMaterial" )
			, [&]( Int const & p_receiver
				, Int const & p_reflection
				, Int const & p_refraction
				, Int const & p_envMapIndex
				, Float p_encoded )
			{
				auto flags = p_writer.DeclLocale( cuT( "flags" )
					, p_writer.Paren( p_receiver << 7 )
						+ p_writer.Paren( p_refraction << 6 )
						+ p_writer.Paren( p_reflection << 5 )
						+ p_writer.Paren( p_envMapIndex ) );
				p_encoded = p_writer.Cast< Float >( flags );
			}, InInt{ &p_writer, cuT( "p_receiver" ) }
			, InInt{ &p_writer, cuT( "p_reflection" ) }
			, InInt{ &p_writer, cuT( "p_refraction" ) }
			, InInt{ &p_writer, cuT( "p_envMapIndex" ) }
			, OutFloat{ &p_writer, cuT( "p_encoded" ) } );
	}
	
	void Declare_DecodeMaterial( GLSL::GlslWriter & p_writer )
	{
		using namespace GLSL;
		auto decodeMaterial = p_writer.ImplementFunction< Void >( cuT( "DecodeMaterial" )
			, [&]( Float const & p_encoded
				, Int p_receiver
				, Int p_reflection
				, Int p_refraction
				, Int p_envMapIndex )
			{
				auto flags = p_writer.DeclLocale( cuT( "flags" ), p_writer.Cast< Int >( p_encoded ) );
				p_receiver = flags >> 7;
				flags -= p_writer.Paren( p_receiver << 7 );
				p_refraction = flags >> 6;
				flags -= p_writer.Paren( p_refraction << 6 );
				p_reflection = flags >> 5;
				flags -= p_writer.Paren( p_reflection << 5 );
				p_envMapIndex = flags;
			}, InFloat{ &p_writer, cuT( "p_encoded" ) }
			, OutInt{ &p_writer, cuT( "p_receiver" ) }
			, OutInt{ &p_writer, cuT( "p_reflection" ) }
			, OutInt{ &p_writer, cuT( "p_refraction" ) }
			, OutInt{ &p_writer, cuT( "p_envMapIndex" ) } );
	}

	void Declare_DecodeReceiver( GLSL::GlslWriter & p_writer )
	{
		using namespace GLSL;
		auto decodeReceiver = p_writer.ImplementFunction< Void >( cuT( "DecodeReceiver" )
			, [&]( Int const & p_encoded
				, Int p_receiver )
			{
				auto flags = p_writer.DeclLocale( cuT( "flags" ), p_encoded );
				p_receiver = flags >> 7;
			}, InInt{ &p_writer, cuT( "p_encoded" ) }
			, OutInt{ &p_writer, cuT( "p_receiver" ) } );
	}

	void EncodeMaterial( GLSL::GlslWriter & p_writer
		, GLSL::Int const & p_receiver
		, GLSL::Int const & p_reflection
		, GLSL::Int const & p_refraction
		, GLSL::Int const & p_envMapIndex
		, GLSL::Float const & p_encoded )
	{
		using namespace GLSL;
		p_writer << WriteFunctionCall< Void >( &p_writer
			, cuT( "EncodeMaterial" )
			, InInt{ p_receiver }
		, InInt{ p_reflection }
		, InInt{ p_refraction }
		, InInt{ p_envMapIndex }
		, OutFloat{ p_encoded } );
		p_writer << Endi{};
	}

	void DecodeMaterial( GLSL::GlslWriter & p_writer
		, GLSL::Float const & p_encoded
		, GLSL::Int const & p_receiver
		, GLSL::Int const & p_reflection
		, GLSL::Int const & p_refraction
		, GLSL::Int const & p_envMapIndex )
	{
		using namespace GLSL;
		p_writer << WriteFunctionCall< Void >( &p_writer
			, cuT( "DecodeMaterial" )
			, InFloat{ p_encoded }
			, OutInt{ p_receiver }
			, OutInt{ p_reflection }
			, OutInt{ p_refraction }
			, OutInt{ p_envMapIndex } );
		p_writer << Endi{};
	}

	void DecodeReceiver( GLSL::GlslWriter & p_writer
		, GLSL::Int & p_encoded
		, GLSL::Int const & p_receiver )
	{
		using namespace GLSL;
		p_writer << WriteFunctionCall< Void >( &p_writer
			, cuT( "DecodeReceiver" )
			, InInt{ p_encoded }
			, OutInt{ p_receiver } );
		p_writer << Endi{};
	}

	//************************************************************************************************

	LightPass::Program::Program( Engine & p_engine
		, GLSL::Shader const & p_vtx
		, GLSL::Shader const & p_pxl )
	{
		auto & renderSystem = *p_engine.GetRenderSystem();

		m_program = p_engine.GetShaderProgramCache().GetNewProgram( false );
		m_program->CreateObject( ShaderType::eVertex );
		m_program->CreateObject( ShaderType::ePixel );
		m_program->SetSource( ShaderType::eVertex, p_vtx );
		m_program->SetSource( ShaderType::ePixel, p_pxl );

		m_lightColour = m_program->CreateUniform< UniformType::eVec3f >( cuT( "light.m_lightBase.m_colour" ), ShaderType::ePixel );

		for ( int i = 0; i < int( DsTexture::eCount ); i++ )
		{
			m_program->CreateUniform< UniformType::eSampler >( GetTextureName( DsTexture( i ) ), ShaderType::ePixel )->SetValue( i );
		}
	}

	LightPass::Program::~Program()
	{
		m_firstPipeline->Cleanup();
		m_blendPipeline->Cleanup();
		m_firstPipeline.reset();
		m_blendPipeline.reset();
		m_geometryBuffers.reset();
		m_lightColour = nullptr;
		m_program.reset();
	}

	void LightPass::Program::Initialise( VertexBuffer & p_vbo
		, IndexBufferSPtr p_ibo
		, MatrixUbo & p_matrixUbo
		, SceneUbo & p_sceneUbo
		, UniformBuffer & p_gpInfoUbo
		, ModelMatrixUbo * p_modelMatrixUbo )
	{
		m_program->Initialise();

		m_firstPipeline = DoCreatePipeline( false );
		m_firstPipeline->AddUniformBuffer( p_matrixUbo.GetUbo() );
		m_firstPipeline->AddUniformBuffer( p_sceneUbo.GetUbo() );
		m_firstPipeline->AddUniformBuffer( p_gpInfoUbo );

		m_blendPipeline = DoCreatePipeline( true );
		m_blendPipeline->AddUniformBuffer( p_matrixUbo.GetUbo() );
		m_blendPipeline->AddUniformBuffer( p_sceneUbo.GetUbo() );
		m_blendPipeline->AddUniformBuffer( p_gpInfoUbo );

		if ( p_modelMatrixUbo )
		{
			m_firstPipeline->AddUniformBuffer( p_modelMatrixUbo->GetUbo() );
			m_blendPipeline->AddUniformBuffer( p_modelMatrixUbo->GetUbo() );
		}

		m_geometryBuffers = m_program->GetRenderSystem()->CreateGeometryBuffers( Topology::eTriangles, *m_program );
		m_geometryBuffers->Initialise( { p_vbo }, p_ibo.get() );
	}

	void LightPass::Program::Cleanup()
	{
		m_geometryBuffers->Cleanup();
		m_geometryBuffers.reset();
		m_program->Cleanup();
	}

	void LightPass::Program::Bind( Light const & p_light )
	{
		DoBind( p_light );
	}

	void LightPass::Program::Render( Size const & p_size
		, Point3f const & p_colour
		, uint32_t p_count
		, bool p_first )const
	{
		m_lightColour->SetValue( p_colour );

		if ( p_first )
		{
			m_firstPipeline->Apply();
		}
		else
		{
			m_blendPipeline->Apply();
		}

		m_geometryBuffers->Draw( p_count, 0 );
	}

	//************************************************************************************************

	LightPass::LightPass( Engine & p_engine
		, FrameBuffer & p_frameBuffer
		, FrameBufferAttachment & p_depthAttach
		, bool p_shadows )
		: m_engine{ p_engine }
		, m_shadows{ p_shadows }
		, m_matrixUbo{ p_engine }
		, m_frameBuffer{ p_frameBuffer }
		, m_depthAttach{ p_depthAttach }
	{
	}

	void LightPass::Render( Size const & p_size
		, GeometryPassResult const & p_gp
		, Light const & p_light
		, Camera const & p_camera
		, Matrix4x4r const & p_invViewProj
		, Castor::Matrix4x4r const & p_invView
		, Castor::Matrix4x4r const & p_invProj
		, bool p_first )
	{
		m_gpInfo->Update( p_size
			, p_camera
			, p_invViewProj
			, p_invView
			, p_invProj );

		DoUpdate( p_size
			, p_light
			, p_camera );

		m_program->Bind( p_light );

		DoRender( p_size
			, p_gp
			, p_light.GetColour()
			, p_first );
	}

	void LightPass::DoInitialise( Scene const & p_scene
		, LightType p_type
		, VertexBuffer & p_vbo
		, IndexBufferSPtr p_ibo
		, SceneUbo & p_sceneUbo
		, ModelMatrixUbo * p_modelMatrixUbo )
	{
		m_gpInfo = std::make_unique< GpInfoUbo >( m_engine );
		SceneFlags sceneFlags{ p_scene.GetFlags() };

		if ( CheckFlag( sceneFlags, SceneFlag::ePbrMetallicRoughness ) )
		{
			m_program = DoCreateProgram( DoGetVertexShaderSource( sceneFlags )
				, DoGetPbrMRPixelShaderSource( sceneFlags, p_type ) );
		}
		else if ( CheckFlag( sceneFlags, SceneFlag::ePbrSpecularGlossiness ) )
		{
			m_program = DoCreateProgram( DoGetVertexShaderSource( sceneFlags )
				, DoGetPbrSGPixelShaderSource( sceneFlags, p_type ) );
		}
		else
		{
			m_program = DoCreateProgram( DoGetVertexShaderSource( sceneFlags )
				, DoGetLegacyPixelShaderSource( sceneFlags, p_type ) );
		}

		m_program->Initialise( p_vbo
			, p_ibo
			, m_matrixUbo
			, p_sceneUbo
			, m_gpInfo->GetUbo()
			, p_modelMatrixUbo );
	}

	void LightPass::DoCleanup()
	{
		m_program->Cleanup();
		m_program.reset();
		m_gpInfo.reset();
		m_matrixUbo.GetUbo().Cleanup();
	}

	void LightPass::DoRender( Castor::Size const & p_size
		, GeometryPassResult const & p_gp
		, Point3f const & p_colour
		, bool p_first )
	{
		m_frameBuffer.Bind( FrameBufferTarget::eDraw );
		m_depthAttach.Attach( AttachmentPoint::eDepthStencil );
		m_frameBuffer.SetDrawBuffers();
		p_gp[size_t( DsTexture::eDepth )]->Bind();
		p_gp[size_t( DsTexture::eData1 )]->Bind();
		p_gp[size_t( DsTexture::eData2 )]->Bind();
		p_gp[size_t( DsTexture::eData3 )]->Bind();
		p_gp[size_t( DsTexture::eData4 )]->Bind();

		m_program->Render( p_size
			, p_colour
			, GetCount()
			, p_first );

		p_gp[size_t( DsTexture::eData4 )]->Unbind();
		p_gp[size_t( DsTexture::eData3 )]->Unbind();
		p_gp[size_t( DsTexture::eData2 )]->Unbind();
		p_gp[size_t( DsTexture::eData1 )]->Unbind();
		p_gp[size_t( DsTexture::eDepth )]->Unbind();
		m_frameBuffer.Unbind();
	}
	
	GLSL::Shader LightPass::DoGetLegacyPixelShaderSource( SceneFlags const & p_sceneFlags
		, LightType p_type )const
	{
		using namespace GLSL;
		GlslWriter writer = m_engine.GetRenderSystem()->CreateGlslWriter();

		// Shader inputs
		UBO_MATRIX( writer );
		UBO_SCENE( writer );
		UBO_GPINFO( writer );
		auto c3d_mapDepth = writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eDepth ) );
		auto c3d_mapData1 = writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eData1 ) );
		auto c3d_mapData2 = writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eData2 ) );
		auto c3d_mapData3 = writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eData3 ) );
		auto c3d_mapData4 = writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eData4 ) );
		auto gl_FragCoord = writer.DeclBuiltin< Vec4 >( cuT( "gl_FragCoord" ) );

		// Shader outputs
		auto pxl_v4FragColor = writer.DeclFragData< Vec4 >( cuT( "pxl_v4FragColor" ), 0 );

		// Utility functions
		auto lighting = legacy::CreateLightingModel( writer
			, p_type
			, m_shadows ? GetShadowType( p_sceneFlags ) : ShadowType::eNone );
		GLSL::Fog fog{ GetFogType( p_sceneFlags ), writer };
		GLSL::Utils utils{ writer };
		utils.DeclareCalcTexCoord();
		utils.DeclareCalcWSPosition();
		Declare_DecodeReceiver( writer );

		writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			auto texCoord = writer.DeclLocale( cuT( "texCoord" ), utils.CalcTexCoord() );
			auto v4Diffuse = writer.DeclLocale( cuT( "v4Diffuse" ), texture( c3d_mapData2, texCoord ) );
			auto v4Normal = writer.DeclLocale( cuT( "v4Normal" ), texture( c3d_mapData1, texCoord ) );
			auto v4Specular = writer.DeclLocale( cuT( "v4Specular" ), texture( c3d_mapData3, texCoord ) );
			auto v4Emissive = writer.DeclLocale( cuT( "v4Emissive" ), texture( c3d_mapData4, texCoord ) );
			auto flags = writer.DeclLocale( cuT( "flags" ), writer.Cast< Int >( v4Normal.w() ) );
			auto iShadowReceiver = writer.DeclLocale( cuT( "iShadowReceiver" ), 0_i );
			DecodeReceiver( writer, flags, iShadowReceiver );
			auto v3MapDiffuse = writer.DeclLocale( cuT( "v3MapDiffuse" ), v4Diffuse.xyz() );
			auto v3MapSpecular = writer.DeclLocale( cuT( "v3MapSpecular" ), v4Specular.xyz() );
			auto fMatShininess = writer.DeclLocale( cuT( "fMatShininess" ), v4Specular.w() );
			auto v3Specular = writer.DeclLocale( cuT( "v3Specular" ), vec3( 0.0_f, 0, 0 ) );
			auto v3Diffuse = writer.DeclLocale( cuT( "v3Diffuse" ), vec3( 0.0_f, 0, 0 ) );
			auto eye = writer.DeclLocale( cuT( "eye" ), c3d_v3CameraPosition );

			auto wsPosition = writer.DeclLocale( cuT( "wsPosition" ), utils.CalcWSPosition( texCoord, c3d_mtxInvViewProj ) );
			auto wsNormal = writer.DeclLocale( cuT( "wsNormal" ), v4Normal.xyz() );

			OutputComponents output{ v3Diffuse, v3Specular };

			switch ( p_type )
			{
			case LightType::eDirectional:
				{
					auto light = writer.GetBuiltin< GLSL::DirectionalLight >( cuT( "light" ) );
					lighting->ComputeOneDirectionalLight( light
						, eye
						, fMatShininess
						, iShadowReceiver
						, FragmentInput( wsPosition, wsNormal )
						, output );
				}
				break;

			case LightType::ePoint:
				{
					auto light = writer.GetBuiltin< GLSL::PointLight >( cuT( "light" ) );
					lighting->ComputeOnePointLight( light
						, eye
						, fMatShininess
						, iShadowReceiver
						, FragmentInput( wsPosition, wsNormal )
						, output );
				}
				break;

			case LightType::eSpot:
				{
					auto light = writer.GetBuiltin< GLSL::SpotLight >( cuT( "light" ) );
					lighting->ComputeOneSpotLight( light
						, eye
						, fMatShininess
						, iShadowReceiver
						, FragmentInput( wsPosition, wsNormal )
						, output );
				}
				break;
			}

			pxl_v4FragColor = vec4( v3Diffuse * v3MapDiffuse.xyz()
				+ v3Specular * v3MapSpecular.xyz(), 1.0 );
		} );

		return writer.Finalise();
	}
	
	GLSL::Shader LightPass::DoGetPbrMRPixelShaderSource( SceneFlags const & p_sceneFlags
		, LightType p_type )const
	{
		using namespace GLSL;
		GlslWriter writer = m_engine.GetRenderSystem()->CreateGlslWriter();

		// Shader inputs
		UBO_MATRIX( writer );
		UBO_SCENE( writer );
		UBO_GPINFO( writer );
		auto c3d_mapDepth = writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eDepth ) );
		auto c3d_mapData1 = writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eData1 ) );
		auto c3d_mapData2 = writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eData2 ) );
		auto c3d_mapData3 = writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eData3 ) );
		auto c3d_mapData4 = writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eData4 ) );
		auto gl_FragCoord = writer.DeclBuiltin< Vec4 >( cuT( "gl_FragCoord" ) );

		// Shader outputs
		auto pxl_v4FragColor = writer.DeclFragData< Vec4 >( cuT( "pxl_v4FragColor" ), 0 );

		// Utility functions
		auto lighting = pbr::mr::CreateLightingModel( writer
			, p_type
			, m_shadows ? GetShadowType( p_sceneFlags ) : ShadowType::eNone );
		GLSL::Fog fog{ GetFogType( p_sceneFlags ), writer };
		GLSL::Utils utils{ writer };
		utils.DeclareCalcTexCoord();
		utils.DeclareCalcWSPosition();
		Declare_DecodeReceiver( writer );

		writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			auto texCoord = writer.DeclLocale( cuT( "texCoord" ), utils.CalcTexCoord() );
			auto data1 = writer.DeclLocale( cuT( "data1" ), texture( c3d_mapData1, texCoord ) );
			auto data2 = writer.DeclLocale( cuT( "data2" ), texture( c3d_mapData2, texCoord ) );
			auto data3 = writer.DeclLocale( cuT( "data3" ), texture( c3d_mapData3, texCoord ) );
			auto data4 = writer.DeclLocale( cuT( "data4" ), texture( c3d_mapData4, texCoord ) );
			auto metallic = writer.DeclLocale( cuT( "metallic" ), data3.r() );
			auto roughness = writer.DeclLocale( cuT( "roughness" ), data3.g() );
			auto flags = writer.DeclLocale( cuT( "flags" ), writer.Cast< Int >( data1.w() ) );
			auto shadowReceiver = writer.DeclLocale( cuT( "shadowReceiver" ), 0_i );
			DecodeReceiver( writer, flags, shadowReceiver );
			auto albedo = writer.DeclLocale( cuT( "albedo" ), data2.xyz() );
			auto diffuse = writer.DeclLocale( cuT( "diffuse" ), vec3( 0.0_f ) );
			auto eye = writer.DeclLocale( cuT( "eye" ), c3d_v3CameraPosition );

			auto wsPosition = writer.DeclLocale( cuT( "wsPosition" ), utils.CalcWSPosition( texCoord, c3d_mtxInvViewProj ) );
			auto wsNormal = writer.DeclLocale( cuT( "wsNormal" ), data1.xyz() );

			switch ( p_type )
			{
			case LightType::eDirectional:
				{
					auto light = writer.GetBuiltin< GLSL::DirectionalLight >( cuT( "light" ) );
					diffuse = lighting->ComputeOneDirectionalLight( light
						, eye
						, albedo
						, metallic
						, roughness
						, shadowReceiver
						, FragmentInput( wsPosition, wsNormal ) );
				}
				break;

			case LightType::ePoint:
				{
					auto light = writer.GetBuiltin< GLSL::PointLight >( cuT( "light" ) );
					diffuse = lighting->ComputeOnePointLight( light
						, eye
						, albedo
						, metallic
						, roughness
						, shadowReceiver
						, FragmentInput( wsPosition, wsNormal ) );
				}
				break;

			case LightType::eSpot:
				{
					auto light = writer.GetBuiltin< GLSL::SpotLight >( cuT( "light" ) );
					diffuse = lighting->ComputeOneSpotLight( light
						, eye
						, albedo
						, metallic
						, roughness
						, shadowReceiver
						, FragmentInput( wsPosition, wsNormal ) );
				}
				break;
			}

			pxl_v4FragColor = vec4( diffuse, 1.0 );
		} );

		return writer.Finalise();
	}
	
	GLSL::Shader LightPass::DoGetPbrSGPixelShaderSource( SceneFlags const & p_sceneFlags
		, LightType p_type )const
	{
		using namespace GLSL;
		GlslWriter writer = m_engine.GetRenderSystem()->CreateGlslWriter();

		// Shader inputs
		UBO_MATRIX( writer );
		UBO_SCENE( writer );
		UBO_GPINFO( writer );
		auto c3d_mapDepth = writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eDepth ) );
		auto c3d_mapData1 = writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eData1 ) );
		auto c3d_mapData2 = writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eData2 ) );
		auto c3d_mapData3 = writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eData3 ) );
		auto c3d_mapData4 = writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eData4 ) );
		auto gl_FragCoord = writer.DeclBuiltin< Vec4 >( cuT( "gl_FragCoord" ) );

		// Shader outputs
		auto pxl_v4FragColor = writer.DeclFragData< Vec4 >( cuT( "pxl_v4FragColor" ), 0 );

		// Utility functions
		auto lighting = pbr::sg::CreateLightingModel( writer
			, p_type
			, m_shadows ? GetShadowType( p_sceneFlags ) : ShadowType::eNone );
		GLSL::Fog fog{ GetFogType( p_sceneFlags ), writer };
		GLSL::Utils utils{ writer };
		utils.DeclareCalcTexCoord();
		utils.DeclareCalcWSPosition();
		Declare_DecodeReceiver( writer );

		writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			auto texCoord = writer.DeclLocale( cuT( "texCoord" ), utils.CalcTexCoord() );
			auto data1 = writer.DeclLocale( cuT( "data1" ), texture( c3d_mapData1, texCoord ) );
			auto data2 = writer.DeclLocale( cuT( "data2" ), texture( c3d_mapData2, texCoord ) );
			auto data3 = writer.DeclLocale( cuT( "data3" ), texture( c3d_mapData3, texCoord ) );
			auto data4 = writer.DeclLocale( cuT( "data4" ), texture( c3d_mapData4, texCoord ) );
			auto specular = writer.DeclLocale( cuT( "specular" ), data3.rgb() );
			auto glossiness = writer.DeclLocale( cuT( "glossiness" ), data2.a() );
			auto flags = writer.DeclLocale( cuT( "flags" ), writer.Cast< Int >( data1.w() ) );
			auto shadowReceiver = writer.DeclLocale( cuT( "shadowReceiver" ), 0_i );
			DecodeReceiver( writer, flags, shadowReceiver );
			auto diffuse = writer.DeclLocale( cuT( "diffuse" ), data2.xyz() );
			auto litten = writer.DeclLocale( cuT( "litten" ), vec3( 0.0_f ) );
			auto eye = writer.DeclLocale( cuT( "eye" ), c3d_v3CameraPosition );

			auto wsPosition = writer.DeclLocale( cuT( "wsPosition" ), utils.CalcWSPosition( texCoord, c3d_mtxInvViewProj ) );
			auto wsNormal = writer.DeclLocale( cuT( "wsNormal" ), data1.xyz() );

			switch ( p_type )
			{
			case LightType::eDirectional:
				{
					auto light = writer.GetBuiltin< GLSL::DirectionalLight >( cuT( "light" ) );
					litten = lighting->ComputeOneDirectionalLight( light
						, eye
						, diffuse
						, specular
						, glossiness
						, shadowReceiver
						, FragmentInput( wsPosition, wsNormal ) );
				}
				break;

			case LightType::ePoint:
				{
					auto light = writer.GetBuiltin< GLSL::PointLight >( cuT( "light" ) );
					litten = lighting->ComputeOnePointLight( light
						, eye
						, diffuse
						, specular
						, glossiness
						, shadowReceiver
						, FragmentInput( wsPosition, wsNormal ) );
				}
				break;

			case LightType::eSpot:
				{
					auto light = writer.GetBuiltin< GLSL::SpotLight >( cuT( "light" ) );
					litten = lighting->ComputeOneSpotLight( light
						, eye
						, diffuse
						, specular
						, glossiness
						, shadowReceiver
						, FragmentInput( wsPosition, wsNormal ) );
				}
				break;
			}

			pxl_v4FragColor = vec4( litten, 1.0 );
		} );

		return writer.Finalise();
	}

	//************************************************************************************************
}
