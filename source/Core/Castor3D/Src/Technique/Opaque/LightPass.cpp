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

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	//************************************************************************************************

	String getTextureName( DsTexture texture )
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

	PixelFormat getTextureFormat( DsTexture texture )
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

	AttachmentPoint getTextureAttachmentPoint( DsTexture texture )
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

	uint32_t getTextureAttachmentIndex( DsTexture texture )
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

	float getMaxDistance( LightCategory const & light
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
			float maxChannel = std::max( std::max( light.getColour()[0]
				, light.getColour()[1] )
				, light.getColour()[2] );
			auto c = 256.0f * maxChannel * light.getDiffuseIntensity();

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

	void declareEncodeMaterial( glsl::GlslWriter & writer )
	{
		using namespace glsl;
		using glsl::operator<<;
		auto encodeMaterial = writer.implementFunction< Void >( cuT( "encodeMaterial" )
			, [&]( Int const & receiver
				, Int const & reflection
				, Int const & refraction
				, Int const & envMapIndex
				, Float encoded )
			{
				auto flags = writer.declLocale( cuT( "flags" )
					, writer.paren( receiver << 7 )
						+ writer.paren( refraction << 6 )
						+ writer.paren( reflection << 5 )
						+ writer.paren( envMapIndex ) );
				encoded = writer.cast< Float >( flags );
			}, InInt{ &writer, cuT( "receiver" ) }
			, InInt{ &writer, cuT( "reflection" ) }
			, InInt{ &writer, cuT( "refraction" ) }
			, InInt{ &writer, cuT( "envMapIndex" ) }
			, OutFloat{ &writer, cuT( "encoded" ) } );
	}
	
	void declareDecodeMaterial( glsl::GlslWriter & writer )
	{
		using namespace glsl;
		auto decodeMaterial = writer.implementFunction< Void >( cuT( "decodeMaterial" )
			, [&]( Float const & encoded
				, Int receiver
				, Int reflection
				, Int refraction
				, Int envMapIndex )
			{
				auto flags = writer.declLocale( cuT( "flags" ), writer.cast< Int >( encoded ) );
				receiver = flags >> 7;
				flags -= writer.paren( receiver << 7 );
				refraction = flags >> 6;
				flags -= writer.paren( refraction << 6 );
				reflection = flags >> 5;
				flags -= writer.paren( reflection << 5 );
				envMapIndex = flags;
			}, InFloat{ &writer, cuT( "encoded" ) }
			, OutInt{ &writer, cuT( "receiver" ) }
			, OutInt{ &writer, cuT( "reflection" ) }
			, OutInt{ &writer, cuT( "refraction" ) }
			, OutInt{ &writer, cuT( "envMapIndex" ) } );
	}

	void declareDecodeReceiver( glsl::GlslWriter & writer )
	{
		using namespace glsl;
		auto decodeReceiver = writer.implementFunction< Void >( cuT( "decodeReceiver" )
			, [&]( Int const & encoded
				, Int receiver )
			{
				auto flags = writer.declLocale( cuT( "flags" ), encoded );
				receiver = flags >> 7;
			}, InInt{ &writer, cuT( "encoded" ) }
			, OutInt{ &writer, cuT( "receiver" ) } );
	}

	void encodeMaterial( glsl::GlslWriter & writer
		, glsl::Int const & receiver
		, glsl::Int const & reflection
		, glsl::Int const & refraction
		, glsl::Int const & envMapIndex
		, glsl::Float const & encoded )
	{
		using namespace glsl;
		writer << writeFunctionCall< Void >( &writer
			, cuT( "encodeMaterial" )
			, InInt{ receiver }
			, InInt{ reflection }
			, InInt{ refraction }
			, InInt{ envMapIndex }
			, OutFloat{ encoded } );
		writer << Endi{};
	}

	void decodeMaterial( glsl::GlslWriter & writer
		, glsl::Float const & encoded
		, glsl::Int const & receiver
		, glsl::Int const & reflection
		, glsl::Int const & refraction
		, glsl::Int const & envMapIndex )
	{
		using namespace glsl;
		writer << writeFunctionCall< Void >( &writer
			, cuT( "decodeMaterial" )
			, InFloat{ encoded }
			, OutInt{ receiver }
			, OutInt{ reflection }
			, OutInt{ refraction }
			, OutInt{ envMapIndex } );
		writer << Endi{};
	}

	void decodeReceiver( glsl::GlslWriter & writer
		, glsl::Int & encoded
		, glsl::Int const & receiver )
	{
		using namespace glsl;
		writer << writeFunctionCall< Void >( &writer
			, cuT( "decodeReceiver" )
			, InInt{ encoded }
			, OutInt{ receiver } );
		writer << Endi{};
	}

	//************************************************************************************************

	LightPass::Program::Program( Engine & engine
		, glsl::Shader const & vtx
		, glsl::Shader const & pxl )
	{
		auto & renderSystem = *engine.getRenderSystem();

		m_program = engine.getShaderProgramCache().getNewProgram( false );
		m_program->createObject( ShaderType::eVertex );
		m_program->createObject( ShaderType::ePixel );
		m_program->setSource( ShaderType::eVertex, vtx );
		m_program->setSource( ShaderType::ePixel, pxl );

		m_lightColour = m_program->createUniform< UniformType::eVec3f >( cuT( "light.m_lightBase.m_colour" ), ShaderType::ePixel );

		for ( int i = 0; i < int( DsTexture::eCount ); i++ )
		{
			m_program->createUniform< UniformType::eSampler >( getTextureName( DsTexture( i ) ), ShaderType::ePixel )->setValue( i );
		}
	}

	LightPass::Program::~Program()
	{
		m_firstPipeline->cleanup();
		m_blendPipeline->cleanup();
		m_firstPipeline.reset();
		m_blendPipeline.reset();
		m_geometryBuffers.reset();
		m_lightColour = nullptr;
		m_program.reset();
	}

	void LightPass::Program::initialise( VertexBuffer & vbo
		, IndexBufferSPtr ibo
		, MatrixUbo & matrixUbo
		, SceneUbo & sceneUbo
		, UniformBuffer & gpInfoUbo
		, ModelMatrixUbo * modelMatrixUbo )
	{
		m_program->initialise();

		m_firstPipeline = doCreatePipeline( false );
		m_firstPipeline->addUniformBuffer( matrixUbo.getUbo() );
		m_firstPipeline->addUniformBuffer( sceneUbo.getUbo() );
		m_firstPipeline->addUniformBuffer( gpInfoUbo );

		m_blendPipeline = doCreatePipeline( true );
		m_blendPipeline->addUniformBuffer( matrixUbo.getUbo() );
		m_blendPipeline->addUniformBuffer( sceneUbo.getUbo() );
		m_blendPipeline->addUniformBuffer( gpInfoUbo );

		if ( modelMatrixUbo )
		{
			m_firstPipeline->addUniformBuffer( modelMatrixUbo->getUbo() );
			m_blendPipeline->addUniformBuffer( modelMatrixUbo->getUbo() );
		}

		m_geometryBuffers = m_program->getRenderSystem()->createGeometryBuffers( Topology::eTriangles, *m_program );
		m_geometryBuffers->initialise( { vbo }, ibo.get() );
	}

	void LightPass::Program::cleanup()
	{
		m_geometryBuffers->cleanup();
		m_geometryBuffers.reset();
		m_program->cleanup();
	}

	void LightPass::Program::bind( Light const & light )
	{
		doBind( light );
	}

	void LightPass::Program::render( Size const & size
		, Point3f const & colour
		, uint32_t count
		, bool first )const
	{
		m_lightColour->setValue( colour );

		if ( first )
		{
			m_firstPipeline->apply();
		}
		else
		{
			m_blendPipeline->apply();
		}

		m_geometryBuffers->draw( count, 0 );
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

	void LightPass::render( Size const & size
		, GeometryPassResult const & gp
		, Light const & light
		, Camera const & camera
		, bool first
		, ShadowMap * shadowMapOpt )
	{
		doUpdate( size
			, light
			, camera );

		m_program->bind( light );

		doRender( size
			, gp
			, light.getColour()
			, first );
	}

	void LightPass::doInitialise( Scene const & scene
		, LightType type
		, VertexBuffer & vbo
		, IndexBufferSPtr ibo
		, SceneUbo & sceneUbo
		, ModelMatrixUbo * modelMatrixUbo )
	{
		SceneFlags sceneFlags{ scene.getFlags() };

		if ( checkFlag( sceneFlags, SceneFlag::ePbrMetallicRoughness ) )
		{
			m_program = doCreateProgram( doGetVertexShaderSource( sceneFlags )
				, doGetPbrMRPixelShaderSource( sceneFlags, type ) );
		}
		else if ( checkFlag( sceneFlags, SceneFlag::ePbrSpecularGlossiness ) )
		{
			m_program = doCreateProgram( doGetVertexShaderSource( sceneFlags )
				, doGetPbrSGPixelShaderSource( sceneFlags, type ) );
		}
		else
		{
			m_program = doCreateProgram( doGetVertexShaderSource( sceneFlags )
				, doGetLegacyPixelShaderSource( sceneFlags, type ) );
		}

		m_program->initialise( vbo
			, ibo
			, m_matrixUbo
			, sceneUbo
			, m_gpInfoUbo.getUbo()
			, modelMatrixUbo );
	}

	void LightPass::doCleanup()
	{
		m_program->cleanup();
		m_program.reset();
		m_matrixUbo.getUbo().cleanup();
	}

	void LightPass::doRender( castor::Size const & size
		, GeometryPassResult const & gp
		, Point3f const & colour
		, bool first )
	{
		m_frameBuffer.bind( FrameBufferTarget::eDraw );
		m_depthAttach.attach( AttachmentPoint::eDepthStencil );
		m_frameBuffer.setDrawBuffers();
		gp[size_t( DsTexture::eDepth )]->bind();
		gp[size_t( DsTexture::eData1 )]->bind();
		gp[size_t( DsTexture::eData2 )]->bind();
		gp[size_t( DsTexture::eData3 )]->bind();
		gp[size_t( DsTexture::eData4 )]->bind();

		m_program->render( size
			, colour
			, getCount()
			, first );

		gp[size_t( DsTexture::eData4 )]->unbind();
		gp[size_t( DsTexture::eData3 )]->unbind();
		gp[size_t( DsTexture::eData2 )]->unbind();
		gp[size_t( DsTexture::eData1 )]->unbind();
		gp[size_t( DsTexture::eDepth )]->unbind();
		m_frameBuffer.unbind();
	}
	
	glsl::Shader LightPass::doGetLegacyPixelShaderSource( SceneFlags const & sceneFlags
		, LightType type )const
	{
		using namespace glsl;
		GlslWriter writer = m_engine.getRenderSystem()->createGlslWriter();

		// Shader inputs
		UBO_MATRIX( writer );
		UBO_SCENE( writer );
		UBO_GPINFO( writer );
		auto c3d_mapDepth = writer.declUniform< Sampler2D >( getTextureName( DsTexture::eDepth ) );
		auto c3d_mapData1 = writer.declUniform< Sampler2D >( getTextureName( DsTexture::eData1 ) );
		auto c3d_mapData2 = writer.declUniform< Sampler2D >( getTextureName( DsTexture::eData2 ) );
		auto c3d_mapData3 = writer.declUniform< Sampler2D >( getTextureName( DsTexture::eData3 ) );
		auto c3d_mapData4 = writer.declUniform< Sampler2D >( getTextureName( DsTexture::eData4 ) );
		auto gl_FragCoord = writer.declBuiltin< Vec4 >( cuT( "gl_FragCoord" ) );

		// Shader outputs
		auto pxl_v4FragColor = writer.declFragData< Vec4 >( cuT( "pxl_v4FragColor" ), 0 );

		// Utility functions
		auto lighting = legacy::createLightingModel( writer
			, type
			, m_shadows ? getShadowType( sceneFlags ) : ShadowType::eNone );
		glsl::Fog fog{ getFogType( sceneFlags ), writer };
		glsl::Utils utils{ writer };
		utils.declareCalcTexCoord();
		utils.declareCalcWSPosition();
		declareDecodeReceiver( writer );

		writer.implementFunction< void >( cuT( "main" ), [&]()
		{
			auto texCoord = writer.declLocale( cuT( "texCoord" ), utils.calcTexCoord() );
			auto v4Diffuse = writer.declLocale( cuT( "v4Diffuse" ), texture( c3d_mapData2, texCoord ) );
			auto v4Normal = writer.declLocale( cuT( "v4Normal" ), texture( c3d_mapData1, texCoord ) );
			auto v4Specular = writer.declLocale( cuT( "v4Specular" ), texture( c3d_mapData3, texCoord ) );
			auto v4Emissive = writer.declLocale( cuT( "v4Emissive" ), texture( c3d_mapData4, texCoord ) );
			auto flags = writer.declLocale( cuT( "flags" ), writer.cast< Int >( v4Normal.w() ) );
			auto iShadowReceiver = writer.declLocale( cuT( "iShadowReceiver" ), 0_i );
			decodeReceiver( writer, flags, iShadowReceiver );
			auto v3MapDiffuse = writer.declLocale( cuT( "v3MapDiffuse" ), v4Diffuse.xyz() );
			auto v3MapSpecular = writer.declLocale( cuT( "v3MapSpecular" ), v4Specular.xyz() );
			auto fMatShininess = writer.declLocale( cuT( "fMatShininess" ), v4Specular.w() );
			auto v3Specular = writer.declLocale( cuT( "v3Specular" ), vec3( 0.0_f, 0, 0 ) );
			auto v3Diffuse = writer.declLocale( cuT( "v3Diffuse" ), vec3( 0.0_f, 0, 0 ) );
			auto eye = writer.declLocale( cuT( "eye" ), c3d_v3CameraPosition );

			auto wsPosition = writer.declLocale( cuT( "wsPosition" ), utils.calcWSPosition( texCoord, c3d_mtxInvViewProj ) );
			auto wsNormal = writer.declLocale( cuT( "wsNormal" ), v4Normal.xyz() );

			OutputComponents output{ v3Diffuse, v3Specular };

			switch ( type )
			{
			case LightType::eDirectional:
				{
					auto light = writer.getBuiltin< glsl::DirectionalLight >( cuT( "light" ) );
					lighting->computeOneDirectionalLight( light
						, eye
						, fMatShininess
						, iShadowReceiver
						, FragmentInput( wsPosition, wsNormal )
						, output );
				}
				break;

			case LightType::ePoint:
				{
					auto light = writer.getBuiltin< glsl::PointLight >( cuT( "light" ) );
					lighting->computeOnePointLight( light
						, eye
						, fMatShininess
						, iShadowReceiver
						, FragmentInput( wsPosition, wsNormal )
						, output );
				}
				break;

			case LightType::eSpot:
				{
					auto light = writer.getBuiltin< glsl::SpotLight >( cuT( "light" ) );
					lighting->computeOneSpotLight( light
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

		return writer.finalise();
	}
	
	glsl::Shader LightPass::doGetPbrMRPixelShaderSource( SceneFlags const & sceneFlags
		, LightType type )const
	{
		using namespace glsl;
		GlslWriter writer = m_engine.getRenderSystem()->createGlslWriter();

		// Shader inputs
		UBO_MATRIX( writer );
		UBO_SCENE( writer );
		UBO_GPINFO( writer );
		auto c3d_mapDepth = writer.declUniform< Sampler2D >( getTextureName( DsTexture::eDepth ) );
		auto c3d_mapData1 = writer.declUniform< Sampler2D >( getTextureName( DsTexture::eData1 ) );
		auto c3d_mapData2 = writer.declUniform< Sampler2D >( getTextureName( DsTexture::eData2 ) );
		auto c3d_mapData3 = writer.declUniform< Sampler2D >( getTextureName( DsTexture::eData3 ) );
		auto c3d_mapData4 = writer.declUniform< Sampler2D >( getTextureName( DsTexture::eData4 ) );
		auto gl_FragCoord = writer.declBuiltin< Vec4 >( cuT( "gl_FragCoord" ) );

		// Shader outputs
		auto pxl_v4FragColor = writer.declFragData< Vec4 >( cuT( "pxl_v4FragColor" ), 0 );

		// Utility functions
		auto lighting = pbr::mr::createLightingModel( writer
			, type
			, m_shadows ? getShadowType( sceneFlags ) : ShadowType::eNone );
		glsl::Fog fog{ getFogType( sceneFlags ), writer };
		glsl::Utils utils{ writer };
		utils.declareCalcTexCoord();
		utils.declareCalcWSPosition();
		declareDecodeReceiver( writer );

		writer.implementFunction< void >( cuT( "main" ), [&]()
		{
			auto texCoord = writer.declLocale( cuT( "texCoord" ), utils.calcTexCoord() );
			auto data1 = writer.declLocale( cuT( "data1" ), texture( c3d_mapData1, texCoord ) );
			auto data2 = writer.declLocale( cuT( "data2" ), texture( c3d_mapData2, texCoord ) );
			auto data3 = writer.declLocale( cuT( "data3" ), texture( c3d_mapData3, texCoord ) );
			auto data4 = writer.declLocale( cuT( "data4" ), texture( c3d_mapData4, texCoord ) );
			auto metallic = writer.declLocale( cuT( "metallic" ), data3.r() );
			auto roughness = writer.declLocale( cuT( "roughness" ), data3.g() );
			auto flags = writer.declLocale( cuT( "flags" ), writer.cast< Int >( data1.w() ) );
			auto shadowReceiver = writer.declLocale( cuT( "shadowReceiver" ), 0_i );
			decodeReceiver( writer, flags, shadowReceiver );
			auto albedo = writer.declLocale( cuT( "albedo" ), data2.xyz() );
			auto diffuse = writer.declLocale( cuT( "diffuse" ), vec3( 0.0_f ) );
			auto eye = writer.declLocale( cuT( "eye" ), c3d_v3CameraPosition );

			auto wsPosition = writer.declLocale( cuT( "wsPosition" ), utils.calcWSPosition( texCoord, c3d_mtxInvViewProj ) );
			auto wsNormal = writer.declLocale( cuT( "wsNormal" ), data1.xyz() );

			switch ( type )
			{
			case LightType::eDirectional:
				{
					auto light = writer.getBuiltin< glsl::DirectionalLight >( cuT( "light" ) );
					diffuse = lighting->computeOneDirectionalLight( light
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
					auto light = writer.getBuiltin< glsl::PointLight >( cuT( "light" ) );
					diffuse = lighting->computeOnePointLight( light
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
					auto light = writer.getBuiltin< glsl::SpotLight >( cuT( "light" ) );
					diffuse = lighting->computeOneSpotLight( light
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

		return writer.finalise();
	}
	
	glsl::Shader LightPass::doGetPbrSGPixelShaderSource( SceneFlags const & sceneFlags
		, LightType type )const
	{
		using namespace glsl;
		GlslWriter writer = m_engine.getRenderSystem()->createGlslWriter();

		// Shader inputs
		UBO_MATRIX( writer );
		UBO_SCENE( writer );
		UBO_GPINFO( writer );
		auto c3d_mapDepth = writer.declUniform< Sampler2D >( getTextureName( DsTexture::eDepth ) );
		auto c3d_mapData1 = writer.declUniform< Sampler2D >( getTextureName( DsTexture::eData1 ) );
		auto c3d_mapData2 = writer.declUniform< Sampler2D >( getTextureName( DsTexture::eData2 ) );
		auto c3d_mapData3 = writer.declUniform< Sampler2D >( getTextureName( DsTexture::eData3 ) );
		auto c3d_mapData4 = writer.declUniform< Sampler2D >( getTextureName( DsTexture::eData4 ) );
		auto gl_FragCoord = writer.declBuiltin< Vec4 >( cuT( "gl_FragCoord" ) );

		// Shader outputs
		auto pxl_v4FragColor = writer.declFragData< Vec4 >( cuT( "pxl_v4FragColor" ), 0 );

		// Utility functions
		auto lighting = pbr::sg::createLightingModel( writer
			, type
			, m_shadows ? getShadowType( sceneFlags ) : ShadowType::eNone );
		glsl::Fog fog{ getFogType( sceneFlags ), writer };
		glsl::Utils utils{ writer };
		utils.declareCalcTexCoord();
		utils.declareCalcWSPosition();
		declareDecodeReceiver( writer );

		writer.implementFunction< void >( cuT( "main" ), [&]()
		{
			auto texCoord = writer.declLocale( cuT( "texCoord" ), utils.calcTexCoord() );
			auto data1 = writer.declLocale( cuT( "data1" ), texture( c3d_mapData1, texCoord ) );
			auto data2 = writer.declLocale( cuT( "data2" ), texture( c3d_mapData2, texCoord ) );
			auto data3 = writer.declLocale( cuT( "data3" ), texture( c3d_mapData3, texCoord ) );
			auto data4 = writer.declLocale( cuT( "data4" ), texture( c3d_mapData4, texCoord ) );
			auto specular = writer.declLocale( cuT( "specular" ), data3.rgb() );
			auto glossiness = writer.declLocale( cuT( "glossiness" ), data2.a() );
			auto flags = writer.declLocale( cuT( "flags" ), writer.cast< Int >( data1.w() ) );
			auto shadowReceiver = writer.declLocale( cuT( "shadowReceiver" ), 0_i );
			decodeReceiver( writer, flags, shadowReceiver );
			auto diffuse = writer.declLocale( cuT( "diffuse" ), data2.xyz() );
			auto litten = writer.declLocale( cuT( "litten" ), vec3( 0.0_f ) );
			auto eye = writer.declLocale( cuT( "eye" ), c3d_v3CameraPosition );

			auto wsPosition = writer.declLocale( cuT( "wsPosition" ), utils.calcWSPosition( texCoord, c3d_mtxInvViewProj ) );
			auto wsNormal = writer.declLocale( cuT( "wsNormal" ), data1.xyz() );

			switch ( type )
			{
			case LightType::eDirectional:
				{
					auto light = writer.getBuiltin< glsl::DirectionalLight >( cuT( "light" ) );
					litten = lighting->computeOneDirectionalLight( light
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
					auto light = writer.getBuiltin< glsl::PointLight >( cuT( "light" ) );
					litten = lighting->computeOnePointLight( light
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
					auto light = writer.getBuiltin< glsl::SpotLight >( cuT( "light" ) );
					litten = lighting->computeOneSpotLight( light
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

		return writer.finalise();
	}

	//************************************************************************************************
}
