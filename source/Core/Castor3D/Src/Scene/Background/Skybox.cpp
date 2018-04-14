#include "Skybox.hpp"

#include "Engine.hpp"

#include "Render/RenderPipeline.hpp"
#include "RenderToTexture/EquirectangularToCube.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Background/Visitor.hpp"
#include "EnvironmentMap/EnvironmentMap.hpp"
#include "Scene/Background/Visitor.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <RenderPass/FrameBuffer.hpp>
#include <RenderPass/FrameBufferAttachment.hpp>
#include <RenderPass/RenderPass.hpp>
#include <RenderPass/RenderPassCreateInfo.hpp>
#include <Shader/ShaderModule.hpp>

using namespace castor;
using namespace glsl;

namespace castor3d
{
	//************************************************************************************************

	namespace
	{
		renderer::ImageCreateInfo doGetImageCreate( renderer::Format format
			, Size const & dimensions
			, bool attachment )
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
			result.usage = renderer::ImageUsageFlag::eSampled
				| renderer::ImageUsageFlag::eTransferDst
				| ( attachment ? renderer::ImageUsageFlag::eColourAttachment : renderer::ImageUsageFlag( 0u ) );
			return result;
		}
	}

	//************************************************************************************************

	SkyboxBackground::TextWriter::TextWriter( String const & tabs )
		: castor::TextWriter< SkyboxBackground >{ tabs }
	{
	}

	bool SkyboxBackground::TextWriter::operator()( SkyboxBackground const & obj, TextFile & file )
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
			castor::TextWriter< SkyboxBackground >::checkError( result, "Skybox equi-texture" );

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
				castor::TextWriter< SkyboxBackground >::checkError( result, ( "Skybox " + faces[i] ).c_str() );
			}

			if ( result )
			{
				result = file.writeText( m_tabs + cuT( "}\n" ) ) > 0;
			}
		}

		return result;
	}

	//************************************************************************************************

	SkyboxBackground::SkyboxBackground( Engine & engine
		, Scene & scene )
		: SceneBackground{ engine, scene, BackgroundType::eSkybox }
	{
		m_texture = std::make_shared< TextureLayout >( *engine.getRenderSystem()
			, doGetImageCreate( renderer::Format::eR8G8B8A8_UNORM, { 512u, 512u }, false )
			, renderer::MemoryPropertyFlag::eDeviceLocal );
	}

	SkyboxBackground::~SkyboxBackground()
	{
	}

	void SkyboxBackground::accept( BackgroundVisitor & visitor )const
	{
		visitor.visit( *this );
	}

	bool SkyboxBackground::doInitialise( renderer::RenderPass const & renderPass )
	{
		REQUIRE( m_texture );
		return doInitialiseTexture();
	}

	void SkyboxBackground::doCleanup()
	{
	}

	void SkyboxBackground::doUpdate( Camera const & camera )
	{
		auto node = camera.getParent();
		m_viewport.setPerspective( 45.0_degrees
			, camera.getViewport().getRatio()
			, 0.1f
			, 2.0f );
		m_viewport.update();
		m_matrixUbo.update( camera.getView()
			, m_viewport.getProjection() );
	}

	void SkyboxBackground::setEquiTexture( TextureLayoutSPtr texture
		, castor::Size const & size )
	{
		m_equiTexturePath = castor::Path( texture->getDefaultImage().toString() );
		m_equiTexture = texture;
		m_equiSize = size;
	}

	bool SkyboxBackground::doInitialiseTexture()
	{
		if ( m_equiTexture )
		{
			doInitialiseEquiTexture();
		}

		m_sampler.lock()->initialise();
		return m_texture->initialise();
	}

	void SkyboxBackground::doInitialiseEquiTexture()
	{
		auto & engine = *getEngine();
		auto & renderSystem = *engine.getRenderSystem();
		m_equiTexture->initialise();

		// create the cube texture if needed.
		if ( m_texture->getPixelFormat() != renderer::Format::eR32G32B32A32_SFLOAT
			|| m_texture->getDimensions().width != m_equiSize.getWidth()
			|| m_texture->getDimensions().height != m_equiSize.getHeight() )
		{
			m_texture = std::make_shared< TextureLayout >( renderSystem
				, doGetImageCreate( renderer::Format::eR32G32B32A32_SFLOAT, m_equiSize, true )
				, renderer::MemoryPropertyFlag::eDeviceLocal );
			m_texture->getImage( uint32_t( CubeMapFace::ePositiveX ) ).initialiseSource();
			m_texture->getImage( uint32_t( CubeMapFace::eNegativeX ) ).initialiseSource();
			m_texture->getImage( uint32_t( CubeMapFace::ePositiveY ) ).initialiseSource();
			m_texture->getImage( uint32_t( CubeMapFace::eNegativeY ) ).initialiseSource();
			m_texture->getImage( uint32_t( CubeMapFace::ePositiveZ ) ).initialiseSource();
			m_texture->getImage( uint32_t( CubeMapFace::eNegativeZ ) ).initialiseSource();
			m_texture->initialise();

			m_hdr = m_equiTexture->getPixelFormat() == renderer::Format::eR32_SFLOAT
				|| m_equiTexture->getPixelFormat() == renderer::Format::eR32G32_SFLOAT
				|| m_equiTexture->getPixelFormat() == renderer::Format::eR32G32B32_SFLOAT
				|| m_equiTexture->getPixelFormat() == renderer::Format::eR32G32B32A32_SFLOAT
				|| m_equiTexture->getPixelFormat() == renderer::Format::eR16_SFLOAT
				|| m_equiTexture->getPixelFormat() == renderer::Format::eR16G16_SFLOAT
				|| m_equiTexture->getPixelFormat() == renderer::Format::eR16G16B16_SFLOAT
				|| m_equiTexture->getPixelFormat() == renderer::Format::eR16G16B16A16_SFLOAT;

			EquirectangularToCube equiToCube{ m_equiTexture->getTexture()
				, renderSystem
				, *m_texture };
			equiToCube.render();

			m_equiTexture->cleanup();
			m_equiTexture.reset();
		}
	}
}
