#include "Skybox.hpp"

#include "Engine.hpp"

#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/DepthStencilRenderBuffer.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "Mesh/Buffer/BufferElementGroup.hpp"
#include "Mesh/Buffer/GeometryBuffers.hpp"
#include "Render/RenderPipeline.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Scene.hpp"
#include "EnvironmentMap/EnvironmentMap.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <GlslSource.hpp>
#include <GlslUtils.hpp>

using namespace castor;
using namespace glsl;

namespace castor3d
{
	Skybox::TextWriter::TextWriter( String const & tabs )
		: castor::TextWriter< Skybox >{ tabs }
	{
	}

	bool Skybox::TextWriter::operator()( Skybox const & obj, TextFile & file )
	{
		static String const faces[]
		{
			cuT( "right" ),
			cuT( "left" ),
			cuT( "bottom" ),
			cuT( "top" ),
			cuT( "back" ),
			cuT( "front" ),
		};

		bool result = true;

		if ( !obj.getEquiTexturePath().empty()
			&& castor::File::fileExists( obj.getEquiTexturePath() ) )
		{
			result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "skybox\n" ) ) > 0
				&& file.writeText( m_tabs + cuT( "{\n" ) ) > 0;
			Path subfolder{ cuT( "Textures" ) };
			Path relative = Scene::TextWriter::copyFile( obj.getEquiTexturePath()
				, file.getFilePath()
				, subfolder );
			result = file.writeText( m_tabs + cuT( "\tequirectangular" ) + cuT( " \"" ) + relative + cuT( "\" 1024 1024\n" ) ) > 0;
			castor::TextWriter< Skybox >::checkError( result, "Skybox equi-texture" );

			if ( result )
			{
				result = file.writeText( m_tabs + cuT( "}\n" ) ) > 0;
			}
		}
		else if ( castor::File::fileExists( Path{ obj.m_texture->getImage( size_t( CubeMapFace( 0 ) ) ).toString() } )
			&& castor::File::fileExists( Path{ obj.m_texture->getImage( size_t( CubeMapFace( 1 ) ) ).toString() } )
			&& castor::File::fileExists( Path{ obj.m_texture->getImage( size_t( CubeMapFace( 2 ) ) ).toString() } )
			&& castor::File::fileExists( Path{ obj.m_texture->getImage( size_t( CubeMapFace( 3 ) ) ).toString() } )
			&& castor::File::fileExists( Path{ obj.m_texture->getImage( size_t( CubeMapFace( 4 ) ) ).toString() } )
			&& castor::File::fileExists( Path{ obj.m_texture->getImage( size_t( CubeMapFace( 5 ) ) ).toString() } ) )
		{
			result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "skybox\n" ) ) > 0
				&& file.writeText( m_tabs + cuT( "{\n" ) ) > 0;
			Path subfolder{ cuT( "Textures" ) };

			for ( uint32_t i = 0; i < 6 && result; ++i )
			{
				Path relative = Scene::TextWriter::copyFile( Path{ obj.m_texture->getImage( size_t( CubeMapFace( i ) ) ).toString() }
					, file.getFilePath()
					, subfolder );
				result = file.writeText( m_tabs + cuT( "\t" ) + faces[i] + cuT( " \"" ) + relative + cuT( "\"\n" ) ) > 0;
				castor::TextWriter< Skybox >::checkError( result, ( "Skybox " + faces[i] ).c_str() );
			}

			if ( result )
			{
				result = file.writeText( m_tabs + cuT( "}\n" ) ) > 0;
			}
		}

		return result;
	}

	//************************************************************************************************

	Skybox::Skybox( Engine & engine )
		: OwnedBy< Engine >{ engine }
		, m_texture{ engine.getRenderSystem()->createTexture( TextureType::eCube
			, AccessType::eNone
			, AccessType::eRead ) }
		, m_matrixUbo{ engine }
		, m_modelMatrixUbo{ engine }
		, m_configUbo{ engine }
		, m_declaration
		{
			{
				BufferElementDeclaration{ ShaderProgram::Position, uint32_t( ElementUsage::ePosition ), ElementType::eVec3 }
			}
		}
	{
		String const skybox = cuT( "Skybox" );

		if ( getEngine()->getSamplerCache().has( skybox ) )
		{
			m_sampler = getEngine()->getSamplerCache().find( skybox );
		}
		else
		{
			auto sampler = getEngine()->getSamplerCache().add( skybox );
			sampler->setInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
			sampler->setInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
			sampler->setWrappingMode( TextureUVW::eU, WrapMode::eClampToEdge );
			sampler->setWrappingMode( TextureUVW::eV, WrapMode::eClampToEdge );
			sampler->setWrappingMode( TextureUVW::eW, WrapMode::eClampToEdge );
			m_sampler = sampler;
		}

		m_bufferVertex =
		{
			-1.0_r, +1.0_r, -1.0_r,/**/+1.0_r, -1.0_r, -1.0_r,/**/-1.0_r, -1.0_r, -1.0_r,/**/+1.0_r, -1.0_r, -1.0_r,/**/-1.0_r, +1.0_r, -1.0_r,/**/+1.0_r, +1.0_r, -1.0_r,/**/
			-1.0_r, -1.0_r, +1.0_r,/**/-1.0_r, +1.0_r, -1.0_r,/**/-1.0_r, -1.0_r, -1.0_r,/**/-1.0_r, +1.0_r, -1.0_r,/**/-1.0_r, -1.0_r, +1.0_r,/**/-1.0_r, +1.0_r, +1.0_r,/**/
			+1.0_r, -1.0_r, -1.0_r,/**/+1.0_r, +1.0_r, +1.0_r,/**/+1.0_r, -1.0_r, +1.0_r,/**/+1.0_r, +1.0_r, +1.0_r,/**/+1.0_r, -1.0_r, -1.0_r,/**/+1.0_r, +1.0_r, -1.0_r,/**/
			-1.0_r, -1.0_r, +1.0_r,/**/+1.0_r, +1.0_r, +1.0_r,/**/-1.0_r, +1.0_r, +1.0_r,/**/+1.0_r, +1.0_r, +1.0_r,/**/-1.0_r, -1.0_r, +1.0_r,/**/+1.0_r, -1.0_r, +1.0_r,/**/
			-1.0_r, +1.0_r, -1.0_r,/**/+1.0_r, +1.0_r, +1.0_r,/**/+1.0_r, +1.0_r, -1.0_r,/**/+1.0_r, +1.0_r, +1.0_r,/**/-1.0_r, +1.0_r, -1.0_r,/**/-1.0_r, +1.0_r, +1.0_r,/**/
			-1.0_r, -1.0_r, -1.0_r,/**/+1.0_r, -1.0_r, -1.0_r,/**/-1.0_r, -1.0_r, +1.0_r,/**/+1.0_r, -1.0_r, -1.0_r,/**/+1.0_r, -1.0_r, +1.0_r,/**/-1.0_r, -1.0_r, +1.0_r,/**/
		};

		uint32_t i = 0;

		for ( auto & vertex : m_arrayVertex )
		{
			vertex = std::make_shared< BufferElementGroup >( &reinterpret_cast< uint8_t * >( m_bufferVertex.data() )[i++ * m_declaration.stride()] );
		}
	}

	Skybox::~Skybox()
	{
		for ( auto & vertex : m_arrayVertex )
		{
			vertex.reset();
		}
	}

	bool Skybox::initialise()
	{
		REQUIRE( m_scene );
		REQUIRE( m_texture );
		bool result = doInitialiseTexture();
		auto & program = doInitialiseShader();

		if ( result )
		{
			if ( m_scene->getMaterialsType() == MaterialType::ePbrMetallicRoughness
				|| m_scene->getMaterialsType() == MaterialType::ePbrSpecularGlossiness )
			{
				m_ibl = std::make_unique< IblTextures >( *m_scene );
				m_ibl->update( *m_texture );
			}

			result = doInitialiseVertexBuffer()
				&& doInitialisePipeline( program );
		}

		return result;
	}

	void Skybox::cleanup()
	{
		REQUIRE( m_texture );
		m_texture->cleanup();
		m_texture.reset();
		m_geometryBuffers->cleanup();
		m_geometryBuffers.reset();
		m_vertexBuffer->cleanup();
		m_vertexBuffer.reset();
		m_matrixUbo.getUbo().cleanup();
		m_modelMatrixUbo.getUbo().cleanup();
		m_configUbo.getUbo().cleanup();
		m_pipeline->cleanup();
		m_pipeline.reset();
		m_ibl.reset();
	}

	void Skybox::render( Camera const & camera )
	{
		REQUIRE( m_texture );
		auto sampler = m_sampler.lock();

		if ( sampler )
		{
			camera.apply();
			auto & scene = *camera.getScene();
			auto node = camera.getParent();
			matrix::setTranslate( m_mtxModel, node->getDerivedPosition() );
			m_matrixUbo.update( camera.getView()
				, camera.getViewport().getProjection() );
			m_modelMatrixUbo.update( m_mtxModel );
			m_configUbo.update( scene.getHdrConfig() );
			m_pipeline->apply();
			m_texture->bind( 0 );
			sampler->bind( 0 );
			m_geometryBuffers->draw( uint32_t( m_arrayVertex.size() ), 0 );
			sampler->unbind( 0 );
			m_texture->unbind( 0 );
		}
	}

	void Skybox::setEquiTexture( TextureLayoutSPtr texture
		, castor::Size const & size )
	{
		m_equiTexturePath = castor::Path( texture->getImage().toString() );
		m_equiTexture = texture;
		m_equiSize = size;
	}

	ShaderProgram & Skybox::doInitialiseShader()
	{
		auto program = getEngine()->getShaderProgramCache().getNewProgram( false );

		glsl::Shader vtx;
		{
			GlslWriter writer{ getEngine()->getRenderSystem()->createGlslWriter() };

			// Inputs
			auto position = writer.declAttribute< Vec3 >( ShaderProgram::Position );
			UBO_MATRIX( writer );
			UBO_MODEL_MATRIX( writer );

			// Outputs
			auto vtx_texture = writer.declOutput< Vec3 >( cuT( "vtx_texture" ) );
			auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

			std::function< void() > main = [&]()
			{
				gl_Position = writer.paren( c3d_mtxProjection * c3d_mtxView * c3d_mtxModel * vec4( position, 1.0 ) ).SWIZZLE_XYWW;
				vtx_texture = position;
			};

			writer.implementFunction< void >( cuT( "main" ), main );
			vtx = writer.finalise();
		}

		glsl::Shader pxl;
		{
			GlslWriter writer{ getEngine()->getRenderSystem()->createGlslWriter() };

			// Inputs
			UBO_HDR_CONFIG( writer );
			auto vtx_texture = writer.declInput< Vec3 >( cuT( "vtx_texture" ) );
			auto skybox = writer.declUniform< SamplerCube >( cuT( "skybox" ) );
			glsl::Utils utils{ writer };

			if ( !m_hdr )
			{
				utils.declareRemoveGamma();
			}

			// Outputs
			auto pxl_FragColor = writer.declOutput< Vec4 >( cuT( "pxl_FragColor" ) );

			std::function< void() > main = [&]()
			{
				auto colour = writer.declLocale( cuT( "colour" )
					, texture( skybox, vec3( vtx_texture.x(), -vtx_texture.y(), vtx_texture.z() ) ) );

				if ( !m_hdr )
				{
					pxl_FragColor = vec4( utils.removeGamma( c3d_gamma, colour.xyz() ), colour.w() );
				}
				else
				{
					pxl_FragColor = vec4( colour.xyz(), colour.w() );
				}
			};

			writer.implementFunction< void >( cuT( "main" ), main );
			pxl = writer.finalise();
		}

		program->createObject( ShaderType::eVertex );
		program->createObject( ShaderType::ePixel );
		program->setSource( ShaderType::eVertex, vtx );
		program->setSource( ShaderType::ePixel, pxl );
		program->initialise();
		return *program;
	}

	bool Skybox::doInitialiseVertexBuffer()
	{
		m_vertexBuffer = std::make_shared< VertexBuffer >( *getEngine(), m_declaration );
		m_vertexBuffer->resize( uint32_t( m_arrayVertex.size() * m_declaration.stride() ) );
		m_vertexBuffer->linkCoords( m_arrayVertex.begin(), m_arrayVertex.end() );
		return m_vertexBuffer->initialise( BufferAccessType::eStatic, BufferAccessNature::eDraw );
	}

	bool Skybox::doInitialiseTexture()
	{
		if ( m_equiTexture )
		{
			doInitialiseEquiTexture();
			m_hdr = true;
		}

		auto result = m_texture->initialise();

		if ( result )
		{
			auto sampler = m_sampler.lock();
			m_texture->bind( 0 );
			m_texture->generateMipmaps();
			m_texture->unbind( 0 );
		}

		return result;
	}

	void Skybox::doInitialiseEquiTexture()
	{
		auto & engine = *getEngine();
		auto & renderSystem = *engine.getRenderSystem();
		m_equiTexture->initialise();

		// create the cube texture.
		m_texture = renderSystem.createTexture( TextureType::eCube
			, AccessType::eNone
			, AccessType::eRead | AccessType::eWrite
			, PixelFormat::eRGB32F
			, m_equiSize );
		m_texture->getImage( uint32_t( CubeMapFace::ePositiveX ) ).initialiseSource();
		m_texture->getImage( uint32_t( CubeMapFace::eNegativeX ) ).initialiseSource();
		m_texture->getImage( uint32_t( CubeMapFace::ePositiveY ) ).initialiseSource();
		m_texture->getImage( uint32_t( CubeMapFace::eNegativeY ) ).initialiseSource();
		m_texture->getImage( uint32_t( CubeMapFace::ePositiveZ ) ).initialiseSource();
		m_texture->getImage( uint32_t( CubeMapFace::eNegativeZ ) ).initialiseSource();
		m_texture->initialise();

		// create the one shot FBO and attaches
		auto fbo = renderSystem.createFrameBuffer();
		std::array< FrameBufferAttachmentSPtr, 6 > attachs
		{
			{
				fbo->createAttachment( m_texture, CubeMapFace::ePositiveX ),
				fbo->createAttachment( m_texture, CubeMapFace::eNegativeX ),
				fbo->createAttachment( m_texture, CubeMapFace::ePositiveY ),
				fbo->createAttachment( m_texture, CubeMapFace::eNegativeY ),
				fbo->createAttachment( m_texture, CubeMapFace::ePositiveZ ),
				fbo->createAttachment( m_texture, CubeMapFace::eNegativeZ ),
			}
		};
		// create The depth RBO.
		auto depthRbo = fbo->createDepthStencilRenderBuffer( PixelFormat::eD24 );
		depthRbo->create();
		depthRbo->initialise( m_equiSize );
		auto depthAttach = fbo->createAttachment( depthRbo );

		// Fill the FBO
		fbo->initialise();
		fbo->bind();
		fbo->attach( AttachmentPoint::eDepth, depthAttach );
		REQUIRE( fbo->isComplete() );
		fbo->unbind();

		// Render the equirectangular texture to the cube faces.
		renderSystem.getCurrentContext()->RenderEquiToCube( m_equiSize
			, *m_equiTexture
			, m_texture
			, fbo
			, attachs );

		// Cleanup the one shot FBO and attaches
		fbo->bind();
		fbo->detachAll();
		fbo->unbind();

		depthRbo->cleanup();
		depthRbo->destroy();

		for ( auto & attach : attachs )
		{
			attach.reset();
		}

		depthAttach.reset();
		fbo->cleanup();

		m_equiTexture->cleanup();
		m_equiTexture.reset();
	}

	bool Skybox::doInitialisePipeline( ShaderProgram & program )
	{
		DepthStencilState dsState;
		dsState.setDepthTest( true );
		dsState.setDepthMask( WritingMask::eZero );
		dsState.setDepthFunc( DepthFunc::eLEqual );

		RasteriserState rsState;
		rsState.setCulledFaces( Culling::eFront );

		m_pipeline = getEngine()->getRenderSystem()->createRenderPipeline( std::move( dsState )
			, std::move( rsState )
			, BlendState{}
			, MultisampleState{}
			, program
			, PipelineFlags{} );
		m_pipeline->addUniformBuffer( m_matrixUbo.getUbo() );
		m_pipeline->addUniformBuffer( m_modelMatrixUbo.getUbo() );
		m_pipeline->addUniformBuffer( m_configUbo.getUbo() );
		m_geometryBuffers = getEngine()->getRenderSystem()->createGeometryBuffers( Topology::eTriangles, m_pipeline->getProgram() );
		return m_geometryBuffers->initialise( { *m_vertexBuffer }, nullptr );
	}
}
