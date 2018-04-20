#include "VkRenderSystem.hpp"

#include <Engine.hpp>

#include <GlslWriter.hpp>

#include <Log/Logger.hpp>

#include <Core/VkCreateRenderer.hpp>

using namespace castor;

namespace VkRender
{
	String RenderSystem::Name = cuT( "Vulkan Renderer" );
	String RenderSystem::Type = cuT( "vulkan" );

	RenderSystem::RenderSystem( castor3d::Engine & engine )
		: castor3d::RenderSystem( engine, Name, true )

	{
		renderer::Logger::setDebugCallback( []( std::string const & msg, bool newLine )
		{
			if ( newLine )
			{
				Logger::logDebug( msg );
			}
			else
			{
				Logger::logDebugNoNL( msg );
			}
		} );
		renderer::Logger::setInfoCallback( []( std::string const & msg, bool newLine )
		{
			if ( newLine )
			{
				Logger::logInfo( msg );
			}
			else
			{
				Logger::logInfoNoNL( msg );
			}
		} );
		renderer::Logger::setWarningCallback( []( std::string const & msg, bool newLine )
		{
			if ( newLine )
			{
				Logger::logWarning( msg );
			}
			else
			{
				Logger::logWarningNoNL( msg );
			}
		} );
		renderer::Logger::setErrorCallback( []( std::string const & msg, bool newLine )
		{
			if ( newLine )
			{
				Logger::logError( msg );
			}
			else
			{
				Logger::logErrorNoNL( msg );
			}
		} );
		m_renderer.reset( createRenderer( renderer::Renderer::Configuration
		{
			"Castor3D",
			"Castor3D",
#ifdef NDEBUG
			false,
#else
			true,
#endif
		} ) );
	}

	RenderSystem::~RenderSystem()
	{
	}

	castor3d::RenderSystemUPtr RenderSystem::create( castor3d::Engine & engine )
	{
		return std::make_unique< RenderSystem >( engine );
	}

	glsl::GlslWriter RenderSystem::createGlslWriter()
	{
		return glsl::GlslWriter{ glsl::GlslWriterConfig{ m_gpuInformations.getShaderLanguageVersion()
			, m_gpuInformations.hasConstantsBuffers()
			, m_gpuInformations.hasTextureBuffers()
			, m_gpuInformations.hasShaderStorageBuffers()
			, true } };
	}
}
