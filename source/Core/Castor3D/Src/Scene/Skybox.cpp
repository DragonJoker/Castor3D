#include "Skybox.hpp"

#include "Engine.hpp"

#include "Render/RenderPipeline.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Scene.hpp"
#include "EnvironmentMap/EnvironmentMap.hpp"
#include "RenderToTexture/EquirectangularToCube.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <RenderPass/FrameBuffer.hpp>
#include <RenderPass/FrameBufferAttachment.hpp>
#include <Shader/ShaderModule.hpp>

#include <GlslSource.hpp>
#include <GlslUtils.hpp>

using namespace castor;
using namespace glsl;

namespace castor3d
{
	//************************************************************************************************
	namespace
	{
		renderer::ImageCreateInfo doGetImageCreate( renderer::Format format
			, Size const & dimensions )
		{
			renderer::ImageCreateInfo result;
			result.flags = renderer::ImageCreateFlag::eCubeCompatible;
			result.arrayLayers = 6u;
			result.extent.width = dimensions.getWidth();
			result.extent.height = dimensions.getHeight();
			result.extent.depth = 1u;
			result.format = format;
			result.initialLayout = renderer::ImageLayout::eUndefined;
			result.imageType = renderer::TextureType::e2D;
			result.mipLevels = 1u;
			result.samples = renderer::SampleCountFlag::e1;
			result.sharingMode = renderer::SharingMode::eExclusive;
			result.tiling = renderer::ImageTiling::eOptimal;
			result.usage = renderer::ImageUsageFlag::eSampled | renderer::ImageUsageFlag::eTransferDst;
			return result;
		}
	}

	//************************************************************************************************

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
		else if ( castor::File::fileExists( Path{ obj.m_texture->getImage( 0u ).toString() } )
			&& castor::File::fileExists( Path{ obj.m_texture->getImage( 1u ).toString() } )
			&& castor::File::fileExists( Path{ obj.m_texture->getImage( 2u ).toString() } )
			&& castor::File::fileExists( Path{ obj.m_texture->getImage( 3u ).toString() } )
			&& castor::File::fileExists( Path{ obj.m_texture->getImage( 4u ).toString() } )
			&& castor::File::fileExists( Path{ obj.m_texture->getImage( 5u ).toString() } ) )
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
		, m_texture{ std::make_shared< TextureLayout >( *engine.getRenderSystem()
			, doGetImageCreate( renderer::Format::eR8G8B8A8_UNORM, { 512u, 512u } )
			, renderer::MemoryPropertyFlag::eDeviceLocal ) }
		, m_matrixUbo{ engine }
		, m_modelMatrixUbo{ engine }
		, m_configUbo{ engine }
		, m_declaration{ std::make_unique< renderer::VertexLayout >( 0u, uint32_t( sizeof( Point3f ) ), renderer::VertexInputRate::eVertex ) }
	{
		String const skybox = cuT( "Skybox" );

		if ( getEngine()->getSamplerCache().has( skybox ) )
		{
			m_sampler = getEngine()->getSamplerCache().find( skybox );
		}
		else
		{
			auto sampler = getEngine()->getSamplerCache().add( skybox );
			sampler->setMinFilter( renderer::Filter::eLinear );
			sampler->setMagFilter( renderer::Filter::eLinear );
			sampler->setWrapS( renderer::WrapMode::eClampToEdge );
			sampler->setWrapT( renderer::WrapMode::eClampToEdge );
			sampler->setWrapR( renderer::WrapMode::eClampToEdge );
			m_sampler = sampler;
		}

		m_bufferVertex =
		{
			{ Point3f{ -1.0_r, +1.0_r, -1.0_r }, Point3f{ +1.0_r, -1.0_r, -1.0_r }, Point3f{ -1.0_r, -1.0_r, -1.0_r }, Point3f{ +1.0_r, -1.0_r, -1.0_r }, Point3f{ -1.0_r, +1.0_r, -1.0_r }, Point3f{ +1.0_r, +1.0_r, -1.0_r } },
			{ Point3f{ -1.0_r, -1.0_r, +1.0_r }, Point3f{ -1.0_r, +1.0_r, -1.0_r }, Point3f{ -1.0_r, -1.0_r, -1.0_r }, Point3f{ -1.0_r, +1.0_r, -1.0_r }, Point3f{ -1.0_r, -1.0_r, +1.0_r }, Point3f{ -1.0_r, +1.0_r, +1.0_r } },
			{ Point3f{ +1.0_r, -1.0_r, -1.0_r }, Point3f{ +1.0_r, +1.0_r, +1.0_r }, Point3f{ +1.0_r, -1.0_r, +1.0_r }, Point3f{ +1.0_r, +1.0_r, +1.0_r }, Point3f{ +1.0_r, -1.0_r, -1.0_r }, Point3f{ +1.0_r, +1.0_r, -1.0_r } },
			{ Point3f{ -1.0_r, -1.0_r, +1.0_r }, Point3f{ +1.0_r, +1.0_r, +1.0_r }, Point3f{ -1.0_r, +1.0_r, +1.0_r }, Point3f{ +1.0_r, +1.0_r, +1.0_r }, Point3f{ -1.0_r, -1.0_r, +1.0_r }, Point3f{ +1.0_r, -1.0_r, +1.0_r } },
			{ Point3f{ -1.0_r, +1.0_r, -1.0_r }, Point3f{ +1.0_r, +1.0_r, +1.0_r }, Point3f{ +1.0_r, +1.0_r, -1.0_r }, Point3f{ +1.0_r, +1.0_r, +1.0_r }, Point3f{ -1.0_r, +1.0_r, -1.0_r }, Point3f{ -1.0_r, +1.0_r, +1.0_r } },
			{ Point3f{ -1.0_r, -1.0_r, -1.0_r }, Point3f{ +1.0_r, -1.0_r, -1.0_r }, Point3f{ -1.0_r, -1.0_r, +1.0_r }, Point3f{ +1.0_r, -1.0_r, -1.0_r }, Point3f{ +1.0_r, -1.0_r, +1.0_r }, Point3f{ -1.0_r, -1.0_r, +1.0_r } },
		};
	}

	Skybox::~Skybox()
	{
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
				m_ibl = std::make_unique< IblTextures >( *m_scene, m_texture->getTexture() );
				m_ibl->update();
			}

			result = doInitialiseVertexBuffer()
				&& doInitialisePipeline( program );
		}

		return result;
	}

	void Skybox::cleanup()
	{
		if ( m_texture )
		{
			m_texture->cleanup();
			m_texture.reset();
		}

		m_vertexBuffer.reset();
		m_matrixUbo.cleanup();
		m_modelMatrixUbo.cleanup();
		m_configUbo.cleanup();
		m_pipeline.reset();
		m_ibl.reset();
	}

	void Skybox::update( Camera const & camera )
	{
		auto & scene = *camera.getScene();
		auto node = camera.getParent();
		matrix::setTranslate( m_mtxModel, node->getDerivedPosition() );
		m_viewport.setPerspective( 45.0_degrees
			, camera.getViewport().getRatio()
			, 0.1f
			, 2.0f );
		m_viewport.update();
		m_matrixUbo.update( camera.getView()
			//, m_viewport.getProjection() );
			, camera.getViewport().getProjection() );
		m_modelMatrixUbo.update( m_mtxModel );

		if ( !m_hdr )
		{
			m_configUbo.update( scene.getHdrConfig() );
		}
	}

	void Skybox::setEquiTexture( TextureLayoutSPtr texture
		, castor::Size const & size )
	{
		m_equiTexturePath = castor::Path( texture->getDefaultImage().toString() );
		m_equiTexture = texture;
		m_equiSize = size;
	}

	renderer::ShaderStageStateArray Skybox::doInitialiseShader()
	{
		auto & renderSystem = *getEngine()->getRenderSystem();

		glsl::Shader vtx;
		{
			GlslWriter writer{ renderSystem.createGlslWriter() };

			// Inputs
			auto position = writer.declAttribute< Vec3 >( cuT( "position" ), 0u );
			UBO_MATRIX( writer, 0, 0 );
			UBO_MODEL_MATRIX( writer, 1, 0 );

			// Outputs
			auto vtx_texture = writer.declOutput< Vec3 >( cuT( "vtx_texture" ), 0u );
			auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

			std::function< void() > main = [&]()
			{
				gl_Position = writer.paren( c3d_projection * c3d_curView * c3d_mtxModel * vec4( position, 1.0 ) ).SWIZZLE_XYWW;
				vtx_texture = position;
			};

			writer.implementFunction< void >( cuT( "main" ), main );
			vtx = writer.finalise();
		}

		glsl::Shader pxl;
		{
			GlslWriter writer{ renderSystem.createGlslWriter() };

			// Inputs
			UBO_HDR_CONFIG( writer, 2, 0 );
			auto vtx_texture = writer.declInput< Vec3 >( cuT( "vtx_texture" ), 3u );
			auto skybox = writer.declSampler< SamplerCube >( cuT( "skybox" ), MinBufferIndex, 0u );
			glsl::Utils utils{ writer };

			if ( !m_hdr )
			{
				utils.declareRemoveGamma();
			}

			// Outputs
			auto pxl_FragColor = writer.declFragData< Vec4 >( cuT( "pxl_FragColor" ), 0u );

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

		renderer::ShaderStageStateArray result;
		result.push_back( { renderSystem.getCurrentDevice()->createShaderModule( renderer::ShaderStageFlag::eVertex ) } );
		result.push_back( { renderSystem.getCurrentDevice()->createShaderModule( renderer::ShaderStageFlag::eFragment ) } );
		result[0].module->loadShader( vtx.getSource() );
		result[0].module->loadShader( pxl.getSource() );
		return result;
	}

	bool Skybox::doInitialiseVertexBuffer()
	{
		m_vertexBuffer = renderer::makeVertexBuffer< NonTexturedCube >( *getEngine()->getRenderSystem()->getCurrentDevice()
			, 1u
			, 0u
			, renderer::MemoryPropertyFlag::eHostVisible );

		if ( auto * buffer = m_vertexBuffer->lock( 0u, 1u, renderer::MemoryMapFlag::eWrite ) )
		{
			std::memcpy( buffer, m_bufferVertex.data(), m_vertexBuffer->getSize() );
			m_vertexBuffer->flush( 0u, 1u );
			m_vertexBuffer->unlock();
		}

		return true;
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
			m_texture->generateMipmaps();
		}

		return result;
	}

	void Skybox::doInitialiseEquiTexture()
	{
		auto & engine = *getEngine();
		auto & renderSystem = *engine.getRenderSystem();
		m_equiTexture->initialise();

		// create the cube texture.
		m_texture = std::make_shared< TextureLayout >( renderSystem
			, doGetImageCreate( renderer::Format::eR32G32B32_SFLOAT, m_equiSize )
			, renderer::MemoryPropertyFlag::eDeviceLocal );
		m_texture->getImage( uint32_t( CubeMapFace::ePositiveX ) ).initialiseSource();
		m_texture->getImage( uint32_t( CubeMapFace::eNegativeX ) ).initialiseSource();
		m_texture->getImage( uint32_t( CubeMapFace::ePositiveY ) ).initialiseSource();
		m_texture->getImage( uint32_t( CubeMapFace::eNegativeY ) ).initialiseSource();
		m_texture->getImage( uint32_t( CubeMapFace::ePositiveZ ) ).initialiseSource();
		m_texture->getImage( uint32_t( CubeMapFace::eNegativeZ ) ).initialiseSource();
		m_texture->initialise();

		EquirectangularToCube equiToCube{ m_equiTexture->getTexture()
			, renderSystem
			, m_texture->getTexture() };
		equiToCube.render();

		m_equiTexture->cleanup();
		m_equiTexture.reset();
	}

	bool Skybox::doInitialisePipeline( renderer::ShaderStageStateArray & program )
	{
		auto & device = *getEngine()->getRenderSystem()->getCurrentDevice();
		renderer::DescriptorSetLayoutBindingArray setLayoutBindings
		{
			{ 0u, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eVertex },
			{ 1u, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eVertex },
			{ 2u, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eFragment },
			{ 3u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment },
		};
		m_descriptorSetLayout = device.createDescriptorSetLayout( std::move( setLayoutBindings ) );
		m_pipelineLayout = device.createPipelineLayout( *m_descriptorSetLayout );

		m_descriptorSetPool = m_descriptorSetLayout->createPool( 1u );
		m_descriptorSet = m_descriptorSetPool->createDescriptorSet( 0u );
		m_descriptorSet->createBinding( m_descriptorSetLayout->getBinding( 0u )
			, m_matrixUbo.getUbo() );
		m_descriptorSet->createBinding( m_descriptorSetLayout->getBinding( 1u )
			, m_modelMatrixUbo.getUbo() );
		m_descriptorSet->createBinding( m_descriptorSetLayout->getBinding( 2u )
			, m_configUbo.getUbo() );
		m_descriptorSet->createBinding( m_descriptorSetLayout->getBinding( 3u )
			, m_texture->getDefaultView()
			, m_sampler.lock()->getSampler() );
		m_descriptorSet->update();

		m_pipeline = m_pipelineLayout->createPipeline( renderer::GraphicsPipelineCreateInfo
		{
			program,
			*m_renderPass,
			renderer::VertexInputState::create( *m_declaration ),
			renderer::InputAssemblyState{ renderer::PrimitiveTopology::eTriangleList },
			renderer::RasterisationState{ 0u, false, false, renderer::PolygonMode::eFill, renderer::CullModeFlag::eFront },
			renderer::MultisampleState{},
			renderer::ColourBlendState::createDefault(),
			{ renderer::DynamicState::eViewport, renderer::DynamicState::eScissor },
			renderer::DepthStencilState{ 0u, true, false, renderer::CompareOp::eLessEqual }
		} );
		return true;
	}

	bool Skybox::doPrepareFrame()
	{
		auto & device = *getEngine()->getRenderSystem()->getCurrentDevice();
		m_semaphore = device.createSemaphore();

		if ( !m_commandBuffer )
		{
			m_commandBuffer = device.getGraphicsCommandPool().createCommandBuffer( false );
		}

		auto result = m_commandBuffer->begin();

		if ( result )
		{
			m_commandBuffer->bindPipeline( *m_pipeline );
			m_commandBuffer->bindDescriptorSet( *m_descriptorSet, *m_pipelineLayout );
			m_commandBuffer->bindVertexBuffer( 0u, m_vertexBuffer->getBuffer(), 0u );
			m_commandBuffer->draw( m_vertexBuffer->getCount() );
		}

		return result;
	}
}
