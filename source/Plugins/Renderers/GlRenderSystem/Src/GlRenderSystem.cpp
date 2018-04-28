#include "GlRenderSystem.hpp"

#include <Engine.hpp>

#include <Core/Renderer.hpp>

#include <GlslWriter.hpp>

#include <Log/Logger.hpp>

#include <Core/GlCreateRenderer.hpp>

using namespace castor;

namespace GlRender
{
	String RenderSystem::Name = cuT( "OpenGL 4.x Renderer" );
	String RenderSystem::Type = cuT( "opengl4" );

	RenderSystem::RenderSystem( castor3d::Engine & engine
		, castor::String const & appName
		, bool enableValidation )
		: castor3d::RenderSystem( engine, Name, false )
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
			string::stringCast< char >( appName ),
			"Castor3D",
			enableValidation,
		} ) );
		Logger::logInfo( cuT( "Using " ) + Name );
		auto & gpu = m_renderer->getPhysicalDevice( 0u );
		m_memoryProperties = gpu.getMemoryProperties();
		m_properties = gpu.getProperties();
		m_features = gpu.getFeatures();
	}

	RenderSystem::~RenderSystem()
	{
	}

	castor3d::RenderSystemUPtr RenderSystem::create( castor3d::Engine & engine
		, castor::String const & appName
		, bool enableValidation )
	{
		return std::make_unique< RenderSystem >( engine
			, appName
			, enableValidation );
	}

	glsl::GlslWriter RenderSystem::createGlslWriter()
	{
		return glsl::GlslWriter{ glsl::GlslWriterConfig{ m_gpuInformations.getShaderLanguageVersion()
			, m_gpuInformations.hasConstantsBuffers()
			, m_gpuInformations.hasTextureBuffers()
			, m_gpuInformations.hasShaderStorageBuffers()
			, false
			, false
			, false
			, false
			, false } };
	}
}
