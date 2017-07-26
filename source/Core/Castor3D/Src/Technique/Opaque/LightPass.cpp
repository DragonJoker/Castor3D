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

	String GetTextureName( DsTexture texture )
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

		return Values[size_t( texture )];
	}

	PixelFormat GetTextureFormat( DsTexture texture )
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

		return Values[size_t( texture )];
	}

	AttachmentPoint GetTextureAttachmentPoint( DsTexture texture )
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

		return Values[size_t( texture )];
	}

	uint32_t GetTextureAttachmentIndex( DsTexture texture )
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

		return Values[size_t( texture )];
	}

	float GetMaxDistance( LightCategory const & light
		, Point3f const & attenuation
		, float max )
	{
		constexpr float threshold = 0.000001f;
		auto constant = std::abs( attenuation[0] );
		auto linear = std::abs( attenuation[1] );
		auto quadratic = std::abs( attenuation[2] );
		float result = max;

		if ( constant >= threshold
			|| linear >= threshold
			|| quadratic >= threshold )
		{
			float maxChannel = std::max( std::max( light.GetColour()[0]
				, light.GetColour()[1] )
				, light.GetColour()[2] );
			auto c = 256.0f * maxChannel * light.GetDiffuseIntensity();

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

		return std::min( max, result );
	}

	void Declare_EncodeMaterial( GLSL::GlslWriter & writer )
	{
		using namespace GLSL;
		using GLSL::operator<<;
		auto encodeMaterial = writer.ImplementFunction< Void >( cuT( "EncodeMaterial" )
			, [&]( Int const & receiver
				, Int const & reflection
				, Int const & refraction
				, Int const & envMapIndex
				, Float encoded )
			{
				auto flags = writer.DeclLocale( cuT( "flags" )
					, writer.Paren( receiver << 7 )
						+ writer.Paren( refraction << 6 )
						+ writer.Paren( reflection << 5 )
						+ writer.Paren( envMapIndex ) );
				encoded = writer.Cast< Float >( flags );
			}, InInt{ &writer, cuT( "receiver" ) }
			, InInt{ &writer, cuT( "reflection" ) }
			, InInt{ &writer, cuT( "refraction" ) }
			, InInt{ &writer, cuT( "envMapIndex" ) }
			, OutFloat{ &writer, cuT( "encoded" ) } );
	}
	
	void Declare_DecodeMaterial( GLSL::GlslWriter & writer )
	{
		using namespace GLSL;
		auto decodeMaterial = writer.ImplementFunction< Void >( cuT( "DecodeMaterial" )
			, [&]( Float const & encoded
				, Int receiver
				, Int reflection
				, Int refraction
				, Int envMapIndex )
			{
				auto flags = writer.DeclLocale( cuT( "flags" ), writer.Cast< Int >( encoded ) );
				receiver = flags >> 7;
				flags -= writer.Paren( receiver << 7 );
				refraction = flags >> 6;
				flags -= writer.Paren( refraction << 6 );
				reflection = flags >> 5;
				flags -= writer.Paren( reflection << 5 );
				envMapIndex = flags;
			}, InFloat{ &writer, cuT( "encoded" ) }
			, OutInt{ &writer, cuT( "receiver" ) }
			, OutInt{ &writer, cuT( "reflection" ) }
			, OutInt{ &writer, cuT( "refraction" ) }
			, OutInt{ &writer, cuT( "envMapIndex" ) } );
	}

	void Declare_DecodeReceiver( GLSL::GlslWriter & writer )
	{
		using namespace GLSL;
		auto decodeReceiver = writer.ImplementFunction< Void >( cuT( "DecodeReceiver" )
			, [&]( Int const & encoded
				, Int receiver )
			{
				auto flags = writer.DeclLocale( cuT( "flags" ), encoded );
				receiver = flags >> 7;
			}, InInt{ &writer, cuT( "encoded" ) }
			, OutInt{ &writer, cuT( "receiver" ) } );
	}

	void EncodeMaterial( GLSL::GlslWriter & writer
		, GLSL::Int const & receiver
		, GLSL::Int const & reflection
		, GLSL::Int const & refraction
		, GLSL::Int const & envMapIndex
		, GLSL::Float const & encoded )
	{
		using namespace GLSL;
		writer << WriteFunctionCall< Void >( &writer
			, cuT( "EncodeMaterial" )
			, InInt{ receiver }
			, InInt{ reflection }
			, InInt{ refraction }
			, InInt{ envMapIndex }
			, OutFloat{ encoded } );
		writer << Endi{};
	}

	void DecodeMaterial( GLSL::GlslWriter & writer
		, GLSL::Float const & encoded
		, GLSL::Int const & receiver
		, GLSL::Int const & reflection
		, GLSL::Int const & refraction
		, GLSL::Int const & envMapIndex )
	{
		using namespace GLSL;
		writer << WriteFunctionCall< Void >( &writer
			, cuT( "DecodeMaterial" )
			, InFloat{ encoded }
			, OutInt{ receiver }
			, OutInt{ reflection }
			, OutInt{ refraction }
			, OutInt{ envMapIndex } );
		writer << Endi{};
	}

	void DecodeReceiver( GLSL::GlslWriter & writer
		, GLSL::Int & encoded
		, GLSL::Int const & receiver )
	{
		using namespace GLSL;
		writer << WriteFunctionCall< Void >( &writer
			, cuT( "DecodeReceiver" )
			, InInt{ encoded }
			, OutInt{ receiver } );
		writer << Endi{};
	}

	//************************************************************************************************

	LightPass::Program::Program( Engine & engine
		, GLSL::Shader const & vtx
		, GLSL::Shader const & pxl )
	{
		auto & renderSystem = *engine.GetRenderSystem();

		m_program = engine.GetShaderProgramCache().GetNewProgram( false );
		m_program->CreateObject( ShaderType::eVertex );
		m_program->CreateObject( ShaderType::ePixel );
		m_program->SetSource( ShaderType::eVertex, vtx );
		m_program->SetSource( ShaderType::ePixel, pxl );

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

	void LightPass::Program::Initialise( VertexBuffer & vbo
		, IndexBufferSPtr ibo
		, MatrixUbo & matrixUbo
		, SceneUbo & sceneUbo
		, UniformBuffer & gpInfoUbo
		, ModelMatrixUbo * modelMatrixUbo )
	{
		m_program->Initialise();

		m_firstPipeline = DoCreatePipeline( false );
		m_firstPipeline->AddUniformBuffer( matrixUbo.GetUbo() );
		m_firstPipeline->AddUniformBuffer( sceneUbo.GetUbo() );
		m_firstPipeline->AddUniformBuffer( gpInfoUbo );

		m_blendPipeline = DoCreatePipeline( true );
		m_blendPipeline->AddUniformBuffer( matrixUbo.GetUbo() );
		m_blendPipeline->AddUniformBuffer( sceneUbo.GetUbo() );
		m_blendPipeline->AddUniformBuffer( gpInfoUbo );

		if ( modelMatrixUbo )
		{
			m_firstPipeline->AddUniformBuffer( modelMatrixUbo->GetUbo() );
			m_blendPipeline->AddUniformBuffer( modelMatrixUbo->GetUbo() );
		}

		m_geometryBuffers = m_program->GetRenderSystem()->CreateGeometryBuffers( Topology::eTriangles, *m_program );
		m_geometryBuffers->Initialise( { vbo }, ibo.get() );
	}

	void LightPass::Program::Cleanup()
	{
		m_geometryBuffers->Cleanup();
		m_geometryBuffers.reset();
		m_program->Cleanup();
	}

	void LightPass::Program::Bind( Light const & light )
	{
		DoBind( light );
	}

	void LightPass::Program::Render( Size const & size
		, Point3f const & colour
		, uint32_t count
		, bool first )const
	{
		m_lightColour->SetValue( colour );

		if ( first )
		{
			m_firstPipeline->Apply();
		}
		else
		{
			m_blendPipeline->Apply();
		}

		m_geometryBuffers->Draw( count, 0 );
	}

	//************************************************************************************************

	LightPass::LightPass( Engine & engine
		, FrameBuffer & frameBuffer
		, FrameBufferAttachment & depthAttach
		, GpInfoUbo & gpInfoUbo
		, bool hasShadows )
		: m_engine{ engine }
		, m_shadows{ hasShadows }
		, m_matrixUbo{ engine }
		, m_frameBuffer{ frameBuffer }
		, m_depthAttach{ depthAttach }
		, m_gpInfoUbo{ gpInfoUbo }
	{
	}

	void LightPass::Render( Size const & size
		, GeometryPassResult const & gp
		, Light const & light
		, Camera const & camera
		, bool first )
	{
		DoUpdate( size
			, light
			, camera );

		m_program->Bind( light );

		DoRender( size
			, gp
			, light.GetColour()
			, first );
	}

	void LightPass::DoInitialise( Scene const & scene
		, LightType type
		, VertexBuffer & vbo
		, IndexBufferSPtr ibo
		, SceneUbo & sceneUbo
		, ModelMatrixUbo * modelMatrixUbo )
	{
		SceneFlags sceneFlags{ scene.GetFlags() };

		if ( CheckFlag( sceneFlags, SceneFlag::ePbrMetallicRoughness ) )
		{
			m_program = DoCreateProgram( DoGetVertexShaderSource( sceneFlags )
				, DoGetPbrMRPixelShaderSource( sceneFlags, type ) );
		}
		else if ( CheckFlag( sceneFlags, SceneFlag::ePbrSpecularGlossiness ) )
		{
			m_program = DoCreateProgram( DoGetVertexShaderSource( sceneFlags )
				, DoGetPbrSGPixelShaderSource( sceneFlags, type ) );
		}
		else
		{
			m_program = DoCreateProgram( DoGetVertexShaderSource( sceneFlags )
				, DoGetLegacyPixelShaderSource( sceneFlags, type ) );
		}

		m_program->Initialise( vbo
			, ibo
			, m_matrixUbo
			, sceneUbo
			, m_gpInfoUbo.GetUbo()
			, modelMatrixUbo );
	}

	void LightPass::DoCleanup()
	{
		m_program->Cleanup();
		m_program.reset();
		m_matrixUbo.GetUbo().Cleanup();
	}

	void LightPass::DoRender( Castor::Size const & size
		, GeometryPassResult const & gp
		, Point3f const & colour
		, bool first )
	{
		m_frameBuffer.Bind( FrameBufferTarget::eDraw );
		m_depthAttach.Attach( AttachmentPoint::eDepthStencil );
		m_frameBuffer.SetDrawBuffers();
		gp[size_t( DsTexture::eDepth )]->Bind();
		gp[size_t( DsTexture::eData1 )]->Bind();
		gp[size_t( DsTexture::eData2 )]->Bind();
		gp[size_t( DsTexture::eData3 )]->Bind();
		gp[size_t( DsTexture::eData4 )]->Bind();

		m_program->Render( size
			, colour
			, GetCount()
			, first );

		gp[size_t( DsTexture::eData4 )]->Unbind();
		gp[size_t( DsTexture::eData3 )]->Unbind();
		gp[size_t( DsTexture::eData2 )]->Unbind();
		gp[size_t( DsTexture::eData1 )]->Unbind();
		gp[size_t( DsTexture::eDepth )]->Unbind();
		m_frameBuffer.Unbind();
	}
	
	GLSL::Shader LightPass::DoGetLegacyPixelShaderSource( SceneFlags const & sceneFlags
		, LightType type )const
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
			, type
			, m_shadows ? GetShadowType( sceneFlags ) : ShadowType::eNone );
		GLSL::Fog fog{ GetFogType( sceneFlags ), writer };
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

			switch ( type )
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
	
	GLSL::Shader LightPass::DoGetPbrMRPixelShaderSource( SceneFlags const & sceneFlags
		, LightType type )const
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
			, type
			, m_shadows ? GetShadowType( sceneFlags ) : ShadowType::eNone );
		GLSL::Fog fog{ GetFogType( sceneFlags ), writer };
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

			switch ( type )
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
	
	GLSL::Shader LightPass::DoGetPbrSGPixelShaderSource( SceneFlags const & sceneFlags
		, LightType type )const
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
			, type
			, m_shadows ? GetShadowType( sceneFlags ) : ShadowType::eNone );
		GLSL::Fog fog{ GetFogType( sceneFlags ), writer };
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

			switch ( type )
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
