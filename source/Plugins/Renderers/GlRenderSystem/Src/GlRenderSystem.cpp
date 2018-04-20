#include "GlRenderSystem.hpp"

#include <Engine.hpp>

#include <Core/Renderer.hpp>

#include <Log/Logger.hpp>

#include <Core/GlCreateRenderer.hpp>

using namespace castor;

namespace GlRender
{
	String RenderSystem::Name = cuT( "OpenGL Renderer" );
	String RenderSystem::Type = cuT( "opengl" );

	RenderSystem::RenderSystem( castor3d::Engine & engine )
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
}
